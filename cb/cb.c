/*
	CB.c  -  SQSL interpreter Couchbase dynamic N1QL driver

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2020 Marco Greco (marco@4glworks.com)

	Initial release: Jun 16
	Current release: Jun 20

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
/* FIXME avoid double string copy on processing results */
/* FIXME timestamps and decimal natively */
/* FIXME displayheaders only evaluated with first row */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>
#include "cosdc.h"
#include "ccmnc.h"
#include "chstc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crxuc.h"
#include "crqxd.h"
#include "cxslc.h"
#include "cb.h"

#define NONOCHARS "\n\r\t"	/* characters cb doesn't like */
#define DTIMESIZE 32		/* multiple of 8, needs to be large enough to 
				   accomodate datetimes & decimals */
#define CNVSTRSIZE 32		/* as above, thankyou */

#define MAXBLOBSIZE (10*1024*1024)

#define COLNAMESIZE 256

#define ROWBUFINC 1024
#define DOCBUFINC 1024*16

char *cursory[]= {
    "INFER",
    "SELECT",
    "EXPLAIN",
    "PREPARE",
    "EXECUTE",
    "RETURNING",
    NULL
};

typedef struct {
    int data;		/* offset into buffer */
    int len;		/* string length */
} lenstring_t;

typedef struct cbconn 
{
    char *connection;
    char *user;
    char *passwd;
    storage_t write_options;
    storage_t read_options;
    lcb_t instance;
} cbconn_t;

typedef struct fgw_cbstmt
{
    lcb_CMDN1QL command;	/* cb command structure */
    int phcount;
    enum { RUNNING, PAUSING, COMPLETE } runstate;
    lenstring_t meta;		/* request results */
    lenstring_t *docs;		/* documents returned from engine */
    int doccount;		/* documents in buffer */
    int maxdocs;		/* document array size */
    int curdoc;			/* current document in array */
    int processed_length;	/* buffer already processed */
    storage_t docbuf;		/* placeholder for data */
    storage_t inbuf;		/* conversion space for placeholders */
    storage_t stmttext;		/* the actual statement */
    sqslda_t *sqslda;
    exprstack_t *exprlist;	/* stack containing one first level unmarshalled document */
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
static void sqd_zeroca(fgw_stmttype *st)
{
    memset(st->ca, 0, sizeof(ca_t));
    if (st->sqltxt!=NULL)
	*(st->sqltxt)=0;
}

/*
** buffer management for incoming document and placeholders
*/
int sqd_pushstring(storage_t *store, const char *data, int len, int term, int escape)
{
    int size;

    /* just an offset for now */
    int retval=store->buflen;

    if (len<0)
	len=strlen(data);
    if (len==0 && !term)
	return retval;
    if (store->buflen+len+1>store->bufmax)
    {
	char *newbuf;

	size=(len+1>DOCBUFINC)? len+1: DOCBUFINC;
	newbuf=realloc(store->buf, store->bufmax+size);

	if (newbuf==NULL)
	    return -1;
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
	     if (*s=='\"')
	     {
		len++;
		size++;
		if (store->buflen+len+1>store->bufmax)
		{
		    char *newbuf=realloc(store->buf, store->bufmax+size);

		    if (newbuf==NULL)
		        return -1;
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
** connection cleanup workhorse
*/
static void freeconn(cbconn_t *cbconn)
{
	if (cbconn->instance)
	    lcb_destroy(cbconn->instance);
	if (cbconn->connection)
	    free(cbconn->connection);
	if (cbconn->user)
	    free(cbconn->user);
	if (cbconn->passwd)
	    free(cbconn->passwd);
	if (cbconn->read_options.buf)
	    free(cbconn->read_options.buf);
	if (cbconn->write_options.buf)
	    free(cbconn->write_options.buf);
	free(cbconn);
}

/* dummy callback to test connections */
static void sqd_concallback(lcb_t instance, int cbtype, const lcb_RESPN1QL *resp)
{
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
    lcb_CMDN1QL command;
    storage_t storage;
    int set_scan = 1;

    memset(ca, 0, sizeof(ca_t));
    memset(&storage, 0, sizeof(storage));
    h=as && !host? as: host;

    /* no default connection */
    if (!h)
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
	crst.v.v3.username=cbconn->user;
    }
    if (passwd)
    {
	if ((cbconn->passwd=(char *)malloc(strlen(passwd)+1))==NULL)
	    goto bad;
	strcpy(cbconn->passwd, passwd);
	crst.v.v3.passwd=cbconn->passwd;
    }
    crst.version=3;
    crst.v.v3.connstr=h;
    if ((ca->sqlcode=lcb_create(&cbconn->instance, &crst))==0 &&
	(ca->sqlcode=lcb_connect(cbconn->instance))==0)
    {
	lcb_wait(cbconn->instance);
	ca->sqlcode=lcb_get_bootstrap_status(cbconn->instance);
    }

    if (ca->sqlcode!=0)
	goto bad1;

    /* if we have options test the connection */
    if (opt)
    {       
	int d, l;

        if (sqd_pushstring(&cbconn->read_options, opt, -1, 0, 0)<0)
            goto bad;
        if (XSL_CALL(fgw_strcasestr)(opt, "\"scan_consistency\":"))
            set_scan=0;
        else if (sqd_pushstring(&cbconn->read_options, ", ", -1, 0, 0)<0)
            goto bad;
	if (set_scan && sqd_pushstring(&cbconn->read_options,
                        "\"scan_consistency\":\"statement_plus\"", -1, 0, 0)<0)
	    goto bad;

	if (sqd_pushstring(&storage, "{\"statement\": \"select 1\", ", -1, 0, 0)<0)
	    goto bad;
	if (sqd_pushstring(&storage, cbconn->read_options.buf, cbconn->read_options.buflen, 0, 0)<0)
	    goto bad;
	if (sqd_pushstring(&storage, "}", 1, 1, 0)<0)
	    goto bad;

	/* libcouchbase has the bad habit of aborting on bad input jsons */
	d=0;
	l=storage.buflen;
	if (!json_parseobject(storage.buf, &d, &l, NULL, NULL, NULL, NULL, NULL))
	{
	    ca->sqlcode=LCB_EINVAL;
	    goto bad1;
	}

	command.query=storage.buf;
	command.nquery=storage.buflen;
	command.content_type="application/json";
	command.callback=sqd_concallback;
	ca->sqlcode=lcb_n1ql_query(cbconn->instance, NULL, &command);
	if (ca->sqlcode==0)
	    lcb_wait(cbconn->instance);
	free(storage.buf);
	if (ca->sqlcode!=0)
	    goto bad1;
    }
    *private=(char *) cbconn;
    return;

bad:
    ca->sqlcode=RC_NOMEM;
bad1:
    if (storage.buf)
	free(storage.buf);
    if (cbconn)
	freeconn(cbconn);
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
	freeconn((cbconn_t *) private);
    else
	ca->sqlcode=EINVAL;
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
** save storage for later restore
*/
static void sqd_pushstore(storage_t *store, storage_t *save)
{
    save->buflen = store->buflen;
}

/*
** restore storage to previous state
*/
static void sqd_popstore(storage_t *store, storage_t *save)
{
    store->buflen = save->buflen;
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
	st_p->runstate=COMPLETE;
	if ((st_p->meta.data=sqd_pushstring(&st_p->docbuf, resp->row, resp->nrow, 1, 0))<0)
	    goto oopsie;
	st_p->meta.len=resp->nrow;
	return;
    }
    if (st_p->doccount==st_p->maxdocs)
    {
	lenstring_t *newrows=(lenstring_t *) realloc(st_p->docs,
		sizeof(lenstring_t)*(st_p->maxdocs+ROWBUFINC));

	if (newrows==NULL)
	    goto oopsie;
	st_p->docs=newrows;
	st_p->maxdocs+=ROWBUFINC;
    }
    st->ca->sqlcode=0;
    if ((st_p->docs[st_p->doccount].data=sqd_pushstring(&st_p->docbuf, resp->row,
						   resp->nrow, 1, 0))<0)
	goto oopsie;
    st_p->docs[st_p->doccount++].len=resp->nrow;

    /* for cursory statements, try to pause every so often */
    if (st->curstate!=ST_EXECUTABLE && st_p->runstate==RUNNING)
    {
	lcb_breakout(instance);
	st_p->runstate=PAUSING;
    }
    return;

oopsie:
    st->ca->sqlcode=RC_NOMEM;

    /* hara kiri, you've seen right! */
    lcb_n1ql_cancel(NULL, (lcb_N1QLHANDLE) st_p->command.handle);
}

/*
**  instantiate stmt structure
*/
DLLDECL fgw_cbstmttype *sqd_newstatement()
{
    fgw_cbstmttype *st;

    if ((st=(fgw_cbstmttype *) malloc(sizeof(fgw_cbstmttype)))!=NULL)
    {
	memset(st, 0, sizeof(fgw_cbstmttype));
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
	if (st_p->stmttext.buf)
	    free(st_p->stmttext.buf);
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
	st_p->docbuf.buflen=0;
	st->curstate=ST_DECLARED;
    }
}

/*
** set cursor name
*/
DLLDECL void sqd_setcursor(fgw_stmttype *st, char *curs)
{
    sqd_zeroca(st);
    /* no cursors yet */
}
 
static void sqd_describe(fgw_stmttype *st)
{
    fgw_cbstmttype *st_p=(fgw_cbstmttype *) st->sqlstmt;

/*
** this all gets done after the select
** no cols for now
*/
    sqd_zeroca(st);
    st_p->colcount=0;
}

/*
**  prepare statement
*/
DLLDECL void sqd_prepare(fgw_stmttype *st, char *query)
{
    struct fgw_cbstmt *st_p;
    cbconn_t *cbconn=(cbconn_t *) st->con->private_area;
    char *nono, **cur_cursory;
    int declared=0;
    int rc;

    if (st && st->curstate==ST_UNINITIALIZED &&
	(st_p=(fgw_cbstmttype *) st->sqlstmt))
    {
	sqd_zeroca(st);
	if ((st->options & SO_WITHHOLD))
	{
	    st->ca->sqlcode=RC_NIMPL;
	    return;
	}
/*
** determine if this thing returns values
*/
	for (cur_cursory=cursory; *cur_cursory; cur_cursory++)
	    if (XSL_CALL(fgw_strcasestr)(query, *cur_cursory))
		declared=1;
	if (declared)
	    st->curstate=ST_DECLARED;
/*
** just an executable statement with placeholders
*/
	else
	    st->curstate=ST_EXECUTABLE;
/*
** we don't use an lcb_N1QLPARAMS structure and lcb_n1p_mkcmd () to assemble
** the command, but rather we assemble it by hand: it's marginally faster and
** uses less memory.
** here we prepare the statement and REST options ready to be appended to the
** REST API text
*/
	/* push options if any */
	if (declared && cbconn->read_options.buf && 
	    (sqd_pushstring(&st_p->stmttext,
		cbconn->read_options.buf, cbconn->read_options.buflen, 0, 0)<0 ||
	     sqd_pushstring(&st_p->stmttext, ",", 1, 0, 0)<0))
	{
	    st->ca->sqlcode=RC_NOMEM;
	    return;
	}

	/* push options if any */
	if (!declared && cbconn->write_options.buf && 
	    (sqd_pushstring(&st_p->stmttext,
		cbconn->write_options.buf, cbconn->write_options.buflen, 0, 0)<0 ||
	     sqd_pushstring(&st_p->stmttext, ",", 1, 0, 0)<0))
	{
	    st->ca->sqlcode=RC_NOMEM;
	    return;
	}

	/* push statement after options */
	if (sqd_pushstring(&st_p->stmttext, "\"statement\":\"", -1, 0, 0)<0 ||
	    sqd_pushstring(&st_p->stmttext, query, -1, 0, 1)<0 ||
	    sqd_pushstring(&st_p->stmttext, "\"}", 2, 1, 0)<0)
	{
	    st->ca->sqlcode=RC_NOMEM;
	    return;
	}
/*
** cb doesn't like certain chars so if we find any, we just get
** them out of the way
** FIXME: this fixes string constants too, while it probably ought to
** return an error
*/
	nono=st_p->stmttext.buf;
	while ((nono=strpbrk(nono, NONOCHARS)))
		*nono=' ';

	if (st->options & (SO_REALPREPARED | SO_INSCURS))
	    st_p->command.cmdflags|=LCB_CMDN1QL_F_PREPCACHE;
	st->options |= SO_LATE_DESCRIBE;
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

	/* REST API text opener - we use application/json */
	if (sqd_pushstring(&st_p->inbuf, "{", 1, 0, 0)<0)
	    st->ca->sqlcode=RC_NOMEM;
    }
}

/*
** sets next placeholder
*/
DLLDECL void sqd_bindda(fgw_stmttype *st, int entry, int type, int size, char *value)
{
    fgw_cbstmttype *st_p=(fgw_cbstmttype *) st->sqlstmt;
    exprstack_t e;
    int ind, rc;
    char cnvbuf[CNVSTRSIZE];
    char *val=NULL;

    if (st_p==NULL || entry<0 || entry>=st_p->phcount)
	return;
    
    /* for the first parameter, we need the args field and array opener */
    if (entry==0)
	rc=sqd_pushstring(&st_p->inbuf, "\"args\":[", -1, 0, 0);

    /* all other fields are comma separated */
    else
	rc=sqd_pushstring(&st_p->inbuf, ",", 1, 0, 0);
    if (rc<0)
	goto oopsie;

    e.type=type;
    e.length=size;
    switch (type)
    {
      case CDATETYPE:
      case CINTTYPE:
	e.val.intvalue=*(int *) value;
	break;
      case CDOUBLETYPE:
	e.val.real=*(double *) value;
	break;
      default:
	e.val.string=value;
    }
   val=XSL_CALL(rxu_tostring)(&e, (char *) &cnvbuf, &ind);
    
    /* we need to bind over and over again */
    st->options|=SO_BUFCNV;

    /* handle nulls */
    if (ind)
    {
	if (sqd_pushstring(&st_p->inbuf, "null", 4, 0, 0)<0)
	    goto oopsie;
    }

    /* hack alert: don't quote objects! */
    else if (type==CSTRINGTYPE && *value=='{')
    {
	if (sqd_pushstring(&st_p->inbuf, val, -1, 0, 0)<0)
	    goto oopsie;
    }

    else
	switch (type)
	{
	  case CDATETYPE:
	  case CINTTYPE:
	  case CDOUBLETYPE:
	    if (sqd_pushstring(&st_p->inbuf, val, -1, 0, 0)<0)
		goto oopsie;
	    break;
	  default:
	    if (sqd_pushstring(&st_p->inbuf, "\"", 1, 0, 0)<0 ||
	        sqd_pushstring(&st_p->inbuf, val, -1, 0, 1)<0 ||
	        sqd_pushstring(&st_p->inbuf, "\"", 1, 0, 0)<0)
		goto oopsie;
	}
    return;

oopsie:
    st->ca->sqlcode=RC_NOMEM;
}

/*
** handy helper to set up and execute the query
*/
static void sqd_doexec(fgw_stmttype *st, fgw_cbstmttype *st_p, cbconn_t *cbconn)
{
    /* prone to race conditions, but... */
    handle=(lcb_N1QLHANDLE) st_p->command.handle;
    st_p->runstate=RUNNING;

    /* populate the command structure */
    if ((st_p->phcount>0 && sqd_pushstring(&st_p->inbuf, "],", 2, 0, 0)<0) ||
        sqd_pushstring(&st_p->inbuf, st_p->stmttext.buf, st_p->stmttext.buflen-1, 0, 0)<0)
    {
	st->ca->sqlcode=RC_NOMEM;
	return;
    }
    st_p->command.query=st_p->inbuf.buf;
    st_p->command.nquery=st_p->inbuf.buflen;
    st_p->command.content_type="application/json";

    if ((st->ca->sqlcode=lcb_n1ql_query(cbconn->instance, st, &st_p->command))==0)
	lcb_wait(cbconn->instance);
    handle=NULL;
    st_p->command.query=NULL;
    st_p->command.content_type="NULL";
}

/*
** populate mutations and errors
*/
static void sqd_handlemeta(fgw_stmttype *st)
{
    struct fgw_cbstmt *st_p=(fgw_cbstmttype *) st->sqlstmt;
    int r, d, l;
    enum objtype t;
    exprstack_t retval;

    /* do we have mutations? */
    d=st_p->meta.data;
    l=st_p->meta.len;
    t=json_searchobject("metrics", st_p->docbuf.buf, &d, &l);
    if (t==T_OBJECT)
    {
	int d1=d, l1=l;

	t=json_searchobject("mutationCount", st_p->docbuf.buf, &d, &l);
	if (t==T_NUM && json_parsenum(st_p->docbuf.buf, &d, &l, &retval))
	    st->ca->sqlerrd2=(int) retval.val.real;
	else
	{
	    t=json_searchobject("resultCount", st_p->docbuf.buf, &d1, &l1);
	    if (t==T_NUM && json_parsenum(st_p->docbuf.buf, &d1, &l1, &retval))
		st->ca->sqlerrd2=(int) retval.val.real;
	}
    }

    /* do we have errors? */
    d=st_p->meta.data;
    l=st_p->meta.len;
    t=json_searchobject("errors", st_p->docbuf.buf, &d, &l);
    if (t==T_ARRAY)
    {

	/* report the first error */
	t=json_searcharray(0, st_p->docbuf.buf, &d, &l);
	if (t==T_OBJECT)
	{
	    int d1=d, l1=l;
	    int max;
	    sqslda_t name;

	    t=json_searchobject("code", st_p->docbuf.buf, &d, &l);
	    if (t==T_NUM && json_parsenum(st_p->docbuf.buf, &d, &l, &retval))
		st->ca->sqlcode=(int) retval.val.real;
	    t=json_searchobject("msg", st_p->docbuf.buf, &d1, &l1);
	    if (t==T_STRING && json_parsestring(st_p->docbuf.buf, &d1, &l1, NULL, &name, NULL))
	    {
		int max=name.sqllen<ERRMSGLEN? name.sqllen: ERRMSGLEN-1;

		strncpy(st->sqltxt, st_p->docbuf.buf+name.sqldata, max);
		st->sqltxt[max]=0;
	    }
	}
    }
}

/*
**  execute non select statement
*/
DLLDECL void sqd_immediate(fgw_stmttype *st)
{
    struct fgw_cbstmt *st_p;
    int i, r=0;
    cbconn_t *cbconn=(cbconn_t *) st->con->private_area;

    sqd_zeroca(st);
    if (!st)
	return;
    else if (!(st_p=(fgw_cbstmttype *) st->sqlstmt))
	st->ca->sqlcode=RC_INSID;
    else if (st->curstate!=ST_EXECUTABLE)
	st->ca->sqlcode=RC_NEXEC;
    else
    {
	sqd_doexec(st, st_p, cbconn);

	sqd_handlemeta(st);
/*	if (st->ca->sqlcode==0 && st_p->ca->sqlerrd2==0)
	    st->ca->sqlcode=RC_NOTFOUND;
*/
    }
}

/*
**  loads next document
*/
DLLDECL void sqd_nextrow(fgw_stmttype *st)
{
    struct fgw_cbstmt *st_p;
    storage_t docbuf_save;
    int r, d, l;
    enum objtype t;
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
	sqd_zeroca(st);
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
	sqd_handlemeta(st);
	if (st->ca->sqlcode==0)
	    st->ca->sqlcode=RC_NOTFOUND;
	return;
    }

    sqd_pushstore(&st_p->docbuf, &docbuf_save);
    st_p->colcount=0;

    if (!st_p->exprlist)
    {
	if (st_p->maxcols == 0)
		st_p->maxcols = 1;
	st_p->exprlist=(exprstack_t *) malloc(sizeof(exprstack_t)*st_p->maxcols);
    }

    if (!st_p->sqslda)
	st_p->sqslda=(sqslda_t *) malloc(sizeof(sqslda_t)*st_p->maxcols);

    d = st_p->docs[st_p->curdoc].data;
    l = st_p->docs[st_p->curdoc].len;
    switch (t=json_doctype(st_p->docbuf.buf, &d, &l))
    {
      case T_EMPTY:
	st_p->colcount=1;
	st_p->sqslda->colname=sqd_pushstring(&st_p->docbuf, "json", 4, 1, 0);
	st_p->sqslda->sqldata=sqd_pushstring(&st_p->docbuf, "{}", 2, 1, 0);
	st_p->sqslda->sqllen=2;
	st_p->exprlist->type=CSTRINGTYPE;
	break;
      case T_OBJECT:
	if (!json_parseobject(st_p->docbuf.buf, &d, &l, &st_p->exprlist, &st_p->sqslda,
			      &st_p->maxcols, &st_p->colcount, &st_p->docbuf) ||
	    json_skipblanks(st_p->docbuf.buf, &d, &l))
	{
	    sqd_popstore(&st_p->docbuf, &docbuf_save);
	    t=T_UNKNOWN;
	}
	break;

      /* array we return as a string */
      case T_ARRAY:
	t=T_UNKNOWN;
	break;
      case T_STRING:
	if (!json_parsestring(st_p->docbuf.buf, &d, &l, st_p->exprlist, st_p->sqslda, &st_p->docbuf) ||
	    json_skipblanks(st_p->docbuf.buf, &d, &l))
	{
	    sqd_popstore(&st_p->docbuf, &docbuf_save);
	    t=T_UNKNOWN;
	}
	break;
      case T_NUM:
	if (!json_parsenum(st_p->docbuf.buf, &d, &l, st_p->exprlist) ||
	    json_skipblanks(st_p->docbuf.buf, &d, &l))
	    t=T_UNKNOWN;
	break;
      case T_BOOL:
	if (!json_parsebool(st_p->docbuf.buf, &d, &l, st_p->exprlist) ||
	    json_skipblanks(st_p->docbuf.buf, &d, &l))
	    t=T_UNKNOWN;
	break;
      case T_NULL:
	if (!json_parsenull(st_p->docbuf.buf, &d, &l, st_p->exprlist) ||
	    json_skipblanks(st_p->docbuf.buf, &d, &l))
	    t=T_UNKNOWN;
    }

    
    switch (t)
    {

      /* what we can't work out we return verbatim */
      case T_UNKNOWN:
	st_p->colcount=1;
	st_p->sqslda->sqldata=st_p->docs[st_p->curdoc].data;
	st_p->sqslda->sqllen=st_p->docs[st_p->curdoc].len;
	st_p->sqslda->colname=sqd_pushstring(&st_p->docbuf, "json", 4, 1, 0);
	st_p->exprlist->type=CSTRINGTYPE;
	break;
      case T_OBJECT:
	break;
      default:
	st_p->colcount=1;
	st_p->sqslda->colname=sqd_pushstring(&st_p->docbuf, "json", 4, 1, 0);
    }

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
    if (e->type==CSTRINGTYPE)
    {
	e->val.string=st_p->docbuf.buf+col->sqldata;
	e->length=col->sqllen;
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
