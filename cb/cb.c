/*
	CB.c  -  SQSL interpreter Couchbase dynamic sql driver

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Jun 16
	Current release: Jul 17

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* FIXME default connection */
/* FIXME connection private data */
/* FIXME timestamps and decimal natively */

/* 1 accumulare opzioni in connect */
/* 2 eliminare lcb_mkcmd */
/* 3 settare opzioni in prepare */
/* 4 accumulare args */
/* 5 unire args in doexec */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>
#include "cosdc.h"
#include "ccmnc.h"
#include "chstc.h"
#include "csqrc.h"
#include "crxuc.h"
#include "crqxd.h"
#include "cxslc.h"

#define NONOCHARS "\n\r\t"	/* characters cb doesn't like */
#define DTIMESIZE 32		/* multiple of 8, needs to be large enough to 
				   accomodate datetimes & decimals */
#define CNVSTRSIZE 32		/* as above, thankyou */

#define MAXBLOBSIZE (10*1024*1024)

#define COLNAMESIZE 256

#define ROWBUFINC 1024
#define DOCBUFINC 1024*16

#define CBQUOTECHAR '\"'
#define CBQUOTE "\""

char *cursory[]= {
    "INFER",
    "SELECT",
    "EXPLAIN",
    "PREPARE",
    "RETURNING",
    NULL
};

typedef struct cbconn 
{
    char *connection;
    char *user;
    char *passwd;
    lcb_t instance;
} cbconn_t;

typedef struct sqslda
{
    int colname;	/* offset into parse buffer for name */
    int sqldata;	/* ditto for data string */
    int sqllen;
} sqslda_t;

typedef struct {
    int data;		/* offset into buffer */
    int len;		/* string length */
} lenstring_t;

typedef struct {
    char *buf;
    int buflen;
    int bufmax;
} storage_t;

typedef struct fgw_cbstmt
{
    lcb_CMDN1QL command;
    lcb_N1QLPARAMS *params;
    int phcount;
    int mutations;
    enum { RUNNING, PAUSING, COMPLETE } runstate;
    lenstring_t meta;		/* request results */
    lenstring_t *docs;		/* documents returned from engine */
    int doccount;		/* documents in buffer */
    int maxdocs;		/* document array size */
    int curdoc;			/* current document in array */
    int processed_length;	/* buffer already processed */
    storage_t docbuf;		/* placeholder for data */
    storage_t inbuf;		/* conversion space for placeholders */
    sqslda_t *sqslda;
    exprstack_t *exprlist;	/* stack containing one row of data */
    int colcount;
    int maxcols;
} fgw_cbstmttype;

#define ERRBUFLEN 256
static lcb_N1QLHANDLE handle=NULL;

XSL_STATE

/*
**  initialize shared object
*/
DLLDECL int sqd_init(XSL_PARMS)
{
    XSL_SETSTATE
    return SO_MASK|SO_SRCFLG;
}

/*
** zap ca
*/
static void sqd_zeroca(ca_t *ca)
{
    memset(ca, 0, sizeof(ca_t));
}

/*
**  connect to an engine
*/
DLLDECL void sqd_connect(char *host, char *as, char *user, char *passwd, char *opt,
			 ca_t *ca, char **private)
{
    char *h;
    cbconn_t *cbconn;
    struct lcb_create_st crst;

    sqd_zeroca(ca);
    h=as && !host? as: host;

    /* no default connection */
    if (!h)
    {
	ca->sqlcode=RC_NIMPL;
	return;
    }

    /* no options for now */
    if (opt)
	for (; *opt; opt++)
	    if (*opt != ' ')
	    {
		ca->sqlcode=RC_NIMPL;
		return;
	    }
    if ((cbconn=(cbconn_t *)malloc(sizeof(cbconn_t)))==NULL)
        goto bad;
    memset(cbconn, 0, sizeof(cbconn_t));
    memset(&crst, 0, sizeof(crst));
    if ((cbconn->connection=(char *)malloc(strlen(h)+1))==NULL)
	goto bad;
    strcpy(cbconn->connection, h);

    /* FIXME these are in clear! */
    if (user)
    {
	if ((cbconn->user=(char *)malloc(strlen(user)+1))==NULL)
	    goto bad;
	strcpy(cbconn->user, user);
    }
    if (passwd)
    {
	if ((cbconn->passwd=(char *)malloc(strlen(passwd)+1))==NULL)
	    goto bad;
	strcpy(cbconn->passwd, passwd);
    }
    crst.version=3;
    crst.v.v3.connstr=h;
    if ((ca->sqlcode=lcb_create(&cbconn->instance, &crst))==0 &&
	(ca->sqlcode=lcb_connect(cbconn->instance))==0)
    {
	lcb_wait(cbconn->instance);
	ca->sqlcode=lcb_get_bootstrap_status(cbconn->instance);
    }

    *private=(char *) cbconn;
    return;

bad:
    ca->sqlcode=RC_NOMEM;
}
 
/*
** set current connection
*/
DLLDECL void sqd_setconnection(char *host, char *name, ca_t *ca, char *private)
{
    if (!host && !name)
	ca->sqlcode=RC_NIMPL;
    else
	ca->sqlcode=0;
}
 
/*
** disconnect from engine
*/
DLLDECL void sqd_disconnect(char *host, char *name, ca_t *ca, char *private)
{
    if (host==(void *) -1 || name==(void *) -1)
	return;					/* functionality is obsolete */
    else if (!host && !name)
	ca->sqlcode=RC_NIMPL;
    else if (private)
    {
	cbconn_t *cbconn=(cbconn_t *) private;

	lcb_destroy(cbconn->instance);
	if (cbconn->connection)
	    free(cbconn->connection);
	if (cbconn->user)
	    free(cbconn->user);
	if (cbconn->passwd)
	    free(cbconn->passwd);
	free(cbconn);
    }
    else
	ca->sqlcode=EINVAL;
}

/*
** buffer management for incoming document and placeholders
*/
static int sqd_pushstring(storage_t *store, const char *data, int len, int term, int escape)
{
    int size;

    /* just an offset for now */
    int retval=store->buflen;

    if (len<0)
	len=strlen(data);
    if (len==0)
	return retval;
    if (store->buflen+len+1>store->bufmax)
    {
	char *newbuf;

	size=(len+1>DOCBUFINC)? len+1: DOCBUFINC;
	newbuf=realloc(store->buf, store->bufmax+size);

	/* FIXME we fail silently */
	if (newbuf==NULL)
	    return retval;
	store->buf=newbuf;
	store->bufmax+=size;
    }
    if (escape)
    {
	int l;
	char *d;
	const char *s;

	for (l=len, s=data, d=store->buf+store->buflen;
	     l; l--)
	{
	     if (*s==CBQUOTECHAR)
	     {
		len++;
		size++;
		if (store->buflen+len+1>store->bufmax)
		{
		    char *newbuf=realloc(store->buf, store->bufmax+size);

		    /* FIXME we fail silently */
		    if (newbuf==NULL)
		        return retval;
		    store->buf=newbuf;
		    store->bufmax+=size;
		}
		*d++='\\';
	     }
	     *d++=*s++;
	}
    }
    else
	memcpy(store->buf+store->buflen, data, len);
    store->buflen+=len;
    if (term)
	store->buf[store->buflen++]='\0';
    return retval;
}

/*
** removes a string from the storage area
*/
static int sqd_zapstring(storage_t *store, int offset, int len)
{
    int src;

    if (len<0)
	len=strlen(store->buf+offset);
    len++;
    src=offset+len;
    if (len<=0)
	return 0;
    else if (len>=store->buflen)
	store->buflen=0;
    else
    {
	store->buflen-=len;
	XSL_CALL(fgw_move)(store->buf, store->buf+src, store->buflen);
    }
    return len;
}

/*
** lcb N1QL callback
*/
static void sqd_callback(lcb_t instance, int cbtype, const lcb_RESPN1QL *resp)
{
    fgw_stmttype *st=(fgw_stmttype *) resp->cookie;
    fgw_cbstmttype *st_p;

    /* not sure what to do! */
    if (!st || !(st_p=(fgw_cbstmttype *) st->sqlstmt))
	return;

    if (resp->rflags & LCB_RESP_F_FINAL)
    {
	st->ca->sqlcode=resp->rc;
	st_p->meta.data=sqd_pushstring(&st_p->docbuf, resp->row, resp->nrow, 1, 0);
	st_p->meta.len=resp->nrow;
	st_p->runstate=COMPLETE;
	return;
    }
    if (st_p->doccount==st_p->maxdocs)
    {
	lenstring_t *newrows=(lenstring_t *) realloc(st_p->docs,
		sizeof(lenstring_t)*(st_p->maxdocs+ROWBUFINC));

	if (newrows==NULL)
	{
	    st->ca->sqlcode=RC_NOMEM;
	    goto oopsie;
	}
	st_p->docs=newrows;
	st_p->maxdocs+=ROWBUFINC;
    }
    st->ca->sqlcode=0;
    st_p->docs[st_p->doccount].data=sqd_pushstring(&st_p->docbuf, resp->row,
						   resp->nrow, 1, 0);
    st_p->docs[st_p->doccount++].len=resp->nrow;

    /* for cursory statements, try to pause every so often */
    if (st->curstate!=ST_EXECUTABLE && st_p->runstate==RUNNING)
    {
	lcb_breakout(instance);
	st_p->runstate=PAUSING;
    }
    return;

oopsie:
    /* hara kiri, you've seen right! */
    lcb_n1ql_cancel(NULL, (lcb_N1QLHANDLE) st_p->command.handle);
}

/*
**  instantiate stmt structure
*/
DLLDECL fgw_cbstmttype *sqd_newstatement()
{
    fgw_cbstmttype *st;
    int i;

    if ((st=(fgw_cbstmttype *) malloc(sizeof(fgw_cbstmttype)))!=NULL)
	memset(st, 0, sizeof(fgw_cbstmttype));
    if ((st->params=lcb_n1p_new())==NULL)
    {
	free(st);
	st=NULL;
    }
    else
    {
	st->command.callback=sqd_callback;
    }
    return st;
}

/*
**  free used statement resources
*/
DLLDECL void sqd_freestatement(fgw_stmttype *st, int clean)
{
    struct fgw_cbstmt *st_p;
    int i;
    struct sqlvar *c;

    if (st && (st_p=(fgw_cbstmttype *) st->sqlstmt))
    {
	/* FIXME for prepared statements there should be a function
	   to get this out of the SDK cache and out of
	   system:prepareds
	 */
	if (st_p->params)
	    lcb_n1p_free(st_p->params);
	if (st_p->inbuf.buf)
	    free(st_p->inbuf.buf);
	if (st_p->sqslda)
	    free(st_p->sqslda);
	if (st_p->docs)
	    free(st_p->docs);
	if (st_p->exprlist)
	    free(st_p->exprlist);
	if (st_p->docbuf.buf)
	    free(st_p->docbuf.buf);
	free(st_p);
    }
}

/*
**  mark statement resources for later usage
*/
DLLDECL void sqd_closestatement(fgw_stmttype *st)
{
    struct fgw_cbstmt *st_p;
    int i;
    struct sqlvar *c;

    if (st && (st_p=(fgw_cbstmttype *) st->sqlstmt) &&
	st->curstate>=ST_NEWLYOPENED)
    {
	st_p->colcount=0;
	st_p->mutations=0;
	st_p->docbuf.buflen=0;
	st->curstate=ST_DECLARED;
    }
}

/*
** set cursor name
*/
DLLDECL void sqd_setcursor(fgw_stmttype *st, char *curs)
{
    sqd_zeroca(st->ca);
    /* no cursors yet */
}
 
static void sqd_describe(fgw_stmttype *st)
{
    fgw_cbstmttype *st_p=(fgw_cbstmttype *) st->sqlstmt;

/*
** this all gets done after the select
** no cols for now
*/
    sqd_zeroca(st->ca);
    st_p->colcount=0;
}

/*
**  prepare statement
*/
DLLDECL void sqd_prepare(fgw_stmttype *st, char *query)
{
    struct fgw_cbstmt *st_p;
    char **cur_cursory;
    char *qtext, *qbuff=NULL;
    int declared=0;
    int rc;

    if (st && st->curstate==ST_UNINITIALIZED &&
	(st_p=(fgw_cbstmttype *) st->sqlstmt))
    {
	sqd_zeroca(st->ca);
	if ((st->options & SO_WITHHOLD))
	{
	    st->ca->sqlcode=RC_NIMPL;
	    return;
	}
	if (st->ca->sqlcode)
	    return;
/*
** cb doesn't like certain chars so if we find any, we just get
** them out of the way
** FIXME: this fixes string constants too, while it probably ought to
** return an error
*/
	if (strpbrk(query, NONOCHARS))
	{
	    char *s, *d;

	    if (!qbuff)
	    {
		if (!(qbuff=(char *) malloc(strlen(query)+1)))
		{
		    st->ca->sqlcode=RC_NOMEM;
		    return;
		}
		qtext=qbuff;
	    }
	    for (s=query, d=qtext; *s; s++, d++)
	    {
		if (strchr(NONOCHARS, *s))
		    *d=' ';
		else
		    *d=*s;
	    }
	    *d='\0';
	}
	else
	    qtext=query;

/*
** determine if this thing returns values
*/
	for (cur_cursory=cursory; *cur_cursory; cur_cursory++)
	    if (XSL_CALL(fgw_strcasestr)(qtext, *cur_cursory))
		declared=1;
	if (declared)
	{
	    st->curstate=ST_DECLARED;
	}
/*
** just an executable statement with placeholders
*/
	else
	{
	    st->curstate=ST_EXECUTABLE;
	}
    if (st->options & (SO_REALPREPARED | SO_INSCURS))
	st_p->command.cmdflags|=LCB_CMDN1QL_F_PREPCACHE;
    lcb_n1p_setconsistency(st_p->params, LCB_N1P_CONSISTENCY_STATEMENT);
    st->ca->sqlcode=lcb_n1p_setstmtz(st_p->params, qtext);
    if (qbuff)
	free(qbuff);
    }
    else if (st)
	st->ca->sqlcode=RC_INSID;
}

/*
** allocates input sqlda
*/
DLLDECL void sqd_allocateda(fgw_stmttype *st, int entries)
{
    fgw_cbstmttype *st_p=(fgw_cbstmttype *) st->sqlstmt;

    if (st_p==NULL)
	st->ca->sqlcode=RC_INSID;
    else
    {
	st_p->inbuf.buflen=0;
	st_p->phcount=entries;
    }
}

/*
** sets next placeholder
*/
DLLDECL void sqd_bindda(fgw_stmttype *st, int entry, int type, int size, char *value)
{
    fgw_cbstmttype *st_p=(fgw_cbstmttype *) st->sqlstmt;
    exprstack_t e;
    int ind;
    long offs;
    char cnvbuf[CNVSTRSIZE];
    char *val=NULL;

    if (st_p==NULL || entry<0 || entry>=st_p->phcount)
	return;
    e.type=type;
    e.length=size;

    /* we need to bind over and over again */
    st->options|=SO_BUFCNV;
    switch (type)
    {
      case CINTTYPE:
	e.val.intvalue=*(int *) value;
	break;
      case CDOUBLETYPE:
	e.val.real=*(double *) value;
	break;
/*
** timestamps and decimals handled here
** datetimes need to be either year to second/fraction or hour to second
*/
      default:
	if (type==CSTRINGTYPE && *value=='{')
	{
	    offs=(long) sqd_pushstring(&st_p->inbuf, value, -1, 1, 0);
	    val=offs+st_p->inbuf.buf;
	    break;
	}

	e.val.string=value;
	offs=(long) sqd_pushstring(&st_p->inbuf, CBQUOTE, 1, 0, 0);
	IGNORE sqd_pushstring(&st_p->inbuf, XSL_CALL(rxu_tostring)(&e, (char *) &cnvbuf, &ind), -1, 0, 1);
	IGNORE sqd_pushstring(&st_p->inbuf, CBQUOTE, 1, 1, 0);
	val=offs+st_p->inbuf.buf;
	break;
    }
    if (val==NULL)
	val=XSL_CALL(rxu_tostring)(&e, (char *) &cnvbuf, &ind);

    /* FIXME do something if null */
    if (ind)
    {
    }
    st->ca->sqlcode=lcb_n1p_posparam(st_p->params, val, strlen(val));
}

/*
** handy helper to set up and execute the query
*/
static void sqd_doexec(fgw_stmttype *st, fgw_cbstmttype *st_p, cbconn_t *cbconn)
{
    /* prone to race conditions, but... */
    handle=(lcb_N1QLHANDLE) st_p->command.handle;
    st_p->runstate=RUNNING;
    if ((st->ca->sqlcode=lcb_n1p_mkcmd(st_p->params, &st_p->command)==0 &&
	(st->ca->sqlcode=lcb_n1ql_query(cbconn->instance, st, &st_p->command)==0)))

	/* FIXME ideally there should be a lcb_fetch, so that we could
 	   get one document at a time rather than having all in memory
	 */
	lcb_wait(cbconn->instance);
    handle=NULL;

    /* now that we are done, zap the positional parameters */
    /* FIXME there should be a function to do this */
    /* FIXME ideally this should go sqd_bindda / sqd_freestatement */
    lcb_n1p_setopt(st_p->params, "args", 4, "[]", 2);
}

/*
**  execute non select statement
*/
DLLDECL void sqd_immediate(fgw_stmttype *st)
{
    struct fgw_cbstmt *st_p;
    int i, r=0;
    cbconn_t *cbconn=(cbconn_t *) st->con->private_area;

    sqd_zeroca(st->ca);
    if (!st)
	return;
    else if (!(st_p=(fgw_cbstmttype *) st->sqlstmt))
	st->ca->sqlcode=RC_INSID;
    else if (st->curstate!=ST_EXECUTABLE)
	st->ca->sqlcode=RC_NEXEC;
    else
    {
	sqd_doexec(st, st_p, cbconn);

	/* FIXME handle mutations here or in sqd_callback() */
	st->ca->sqlerrd2=st_p->mutations;
/*	if (st->ca->sqlcode==0 && st_p->mutations==0)
	    st->ca->sqlcode=RC_NOTFOUND;
*/
    }
}

/*
**  loads next row
*/
DLLDECL void sqd_nextrow(fgw_stmttype *st)
{
    struct fgw_cbstmt *st_p;
    int r;
    cbconn_t *cbconn=(cbconn_t *) st->con->private_area;

    if (!st)
	return;
    else if (!(st_p=(fgw_cbstmttype *) st->sqlstmt))
    {
	st->ca->sqlcode=RC_INSID;
	return;
    }
    else if (st->curstate<ST_DECLARED)
    {
	st->ca->sqlcode=RC_NDECL;
	return;
    }
    if (st->curstate==ST_DECLARED)
    {
	st->curstate=ST_NEWLYOPENED;
	st_p->doccount=0;
	st_p->curdoc=0;
	st_p->processed_length=0;
	sqd_zeroca(st->ca);
	sqd_doexec(st, st_p, cbconn);
	if (st->ca->sqlcode==0)
	    st->curstate=ST_OPENED;
	else
	    return;
    }
    else if (st_p->runstate==PAUSING && st_p->curdoc==st_p->doccount)
    {
/*	if (st_p->doccount>0)
	    IGNORE sqd_zapstring(&st_p->docbuf, 0, st_p->processed_length);
	st_p->doccount=0;
	st_p->curdoc=0;
	st_p->processed_length=0;
*/	handle=(lcb_N1QLHANDLE) st_p->command.handle;
	st_p->runstate=RUNNING;
	lcb_wait(cbconn->instance);
	handle=NULL;
    }

    if (st_p->curdoc>=st_p->doccount)
    {
	st->ca->sqlcode=RC_NOTFOUND;
	return;
    }

    st_p->colcount=0;

    /* FIXME doc parsing goes here */
    st_p->colcount=1;
    if (!st_p->exprlist)
	st_p->exprlist=(exprstack_t *) malloc(sizeof(exprstack_t));

    /* FIXME fill sqslda properly! */
    if (!st_p->sqslda)
	st_p->sqslda=(sqslda_t *) malloc(sizeof(sqslda_t));
    st_p->sqslda->sqldata=st_p->docs[st_p->curdoc].data;
    st_p->sqslda->sqllen=st_p->docs[st_p->curdoc].len;
    st_p->sqslda->colname=sqd_pushstring(&st_p->docbuf, "json", 4, 1, 0);
    st_p->exprlist->type=CSTRINGTYPE;

    st_p->processed_length+=st_p->docs[st_p->curdoc].len+1;
    st_p->curdoc++;
    return;

oopsie:
    st->ca->sqlcode=RC_NOMEM;
}

/*
** returns next field in buffer
*/
DLLDECL exprstack_t *sqd_nexttoken(int field, fgw_stmttype *st)
{
    exprstack_t *e;
    sqslda_t *col;
    char *c;
    fgw_cbstmttype *st_p=(fgw_cbstmttype *)st->sqlstmt;

    if (field>=st_p->colcount)
	return NULL;
    col=st_p->sqslda+field;
    e=st_p->exprlist+field;
    if (col->sqllen==-1)
    {
	e->length=-1;
	e->val.string=NULL;
    }
    else
    {
	e->length=col->sqllen;
	if (e->type==CSTRINGTYPE)
	    e->val.string=col->sqldata+st_p->docbuf.buf;
    }
    return e;
}

/*
** returns field count
*/
DLLDECL int sqd_counttokens(fgw_stmttype *st_p)
{
    return ((fgw_cbstmttype *) st_p->sqlstmt)->colcount;
}

/*
** returns error message string
*/
DLLDECL void sqd_errmsg(int err, char *buf, int len)
{
    const char *errstr=lcb_strerror(NULL, err);
    int l=strlen(errstr);

    if (l>0)
	memcpy(buf, errstr, l);
    buf[l]='\0';
}

/*
** sends a break to the engine
*/
DLLDECL void sqd_break()
{
    /* FIXME sqd_break is meant to be called by a signal handler
       and as such it does not have knowledge of requests
       keeping track of the current request works, but it is so
       prone to race conditions...
     */
    if (handle!=NULL)
	lcb_n1ql_cancel(NULL, handle);
    handle=NULL;
}

/*
** returns a description of the selected columns
*/
DLLDECL void sqd_describecol(fgw_stmttype *st, int column, exprstack_t *d)
{
    sqslda_t *col;
    exprstack_t *el;
    fgw_cbstmttype *st_p=(fgw_cbstmttype *) st->sqlstmt;

    if (column<0 ||
	column>=st_p->colcount)
	memset(d, 0, sizeof(exprstack_t));
    else
    {
	col=st_p->sqslda+column;
	el=st_p->exprlist+column;

	/* we don't have any special columns to deal with */
	d->type=el->type;
	d->length=el->length;
	d->val.string=st_p->docbuf.buf+col->colname;
    }
}
