/*
	DB2CLI.c  -  SQSL interpreter db2 cli dynamic sql driver

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Oct 05
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

/* FIXME in & out da: convert decimals properly */
/* FIXME default connection */
/* FIXME connection private data */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "sqlcli1.h"
#include "sqlenv.h"

/* must be here because sqlcli1.h includes windows.h and that redefines IGNORE */
#include "cosdc.h"
#include "ccmnc.h"
#include "chstc.h"
#include "csqrc.h"
#include "crxuc.h"
#include "crqxd.h"
#include "cxslc.h"
#include "db2cli.h"

extern int db2api_parser(char *buffer, int mode, int *sc);

#define NONOCHARS "\n\r\t"	/* characters db2 doesn't like */
#define DTIMESIZE 32		/* multiple of 8, needs to be large enough to 
				   accomodate datetimes & decimals */
#define CNVSTRSIZE 32		/* as above, thankyou */

#define COLNAMESIZE 256
typedef struct sqslda
{
    char colname[COLNAMESIZE];
    SQLSMALLINT sqltype;
    SQLSMALLINT convtype;
    SQLUINTEGER sqllen;
    SQLSMALLINT scale;
    SQLSMALLINT nullable;
    SQLINTEGER rlen;
    char *sqldata;
} sqslda_t;

typedef struct fgw_db2stmt
{
    SQLHANDLE stmtid;		/* statement identifier */
    int	stmttype;		/* statement type (internally handled) */
    char *outbuf;		/* data returned from engine */
    SQLSMALLINT colcount;
    sqslda_t *sqslda_out;
    char *inbuf;		/* placeholders buffer */
    int phcount;
    int sqslda_entries;		/* current # of allocated placeholder entries */
    sqslda_t *sqslda_in;
    exprstack_t *exprlist;	/* stack containing one row of data */
} fgw_db2stmttype;

static SQLHANDLE envhandle;

#define ERRBUFLEN 256
static char errbuf[ERRBUFLEN];
static int errbuflen, sqlcode;

XSL_STATE

/*
**  initialize shared object
*/
DLLDECL int sqd_init(XSL_PARMS)
{
    int rc;

    XSL_SETSTATE
    if ((rc=SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE,
			&envhandle))==SQL_SUCCESS &&
	(rc=SQLSetEnvAttr(envhandle, SQL_ATTR_ODBC_VERSION,
		      (void *)SQL_OV_ODBC3, 0))==SQL_SUCCESS)
    {
/*
** Although this should be the default, it's best reiterated here, or the
** CLI layer might decide to return a CLI0126E
*/
	SQLSetEnvAttr(envhandle, SQL_ATTR_CONNECTTYPE,
			  (SQLPOINTER) SQL_CONCURRENT_TRANS, 0);
	return SO_MASK|SO_SRCFLG;
    }
    else
    {
	errno=rc;
        return -1;
    }
}

/*
** obtain error description from troublesome handle
**
** Do not get me started on CLI error handling, or CLI itself...
*/
static void sqd_fillca(ca_t *ca, int htype, SQLHANDLE handle)
{
    char dummy[5];
    
    IGNORE SQLGetDiagRec(htype, handle,
			 (SQLSMALLINT) 1, (char *) &dummy,
			 (SQLINTEGER *) &sqlcode,
			 (char *) &errbuf, (SQLSMALLINT) ERRBUFLEN,
			 (SQLSMALLINT *) &errbuflen);
    if (errbuflen>=0 && errbuflen<ERRBUFLEN)
	errbuf[errbuflen]='\0';
    ca->sqlcode=sqlcode;
}

static void sqd_zeroca(ca_t *ca)
{
    memset(ca, 0, sizeof(ca_t));
    sqlcode=0;
    errbuflen=0;
    errbuf[errbuflen]='\0';
}

/*
**  connect to an engine
*/
DLLDECL void sqd_connect(char *host, char *as, char *user, char *passwd,
			 char *opt, ca_t *ca, char **private_area)
{
    int exclusive=0;
    char *h;
    SQLHANDLE conhandle;

    sqd_zeroca(ca);
    if (opt)
	exclusive=db2api_parser(opt, 1, &ca->sqlcode);
    if (ca->sqlcode)
	return;
    h=as && !host? as: host;
    if (!h || exclusive)
    {
	ca->sqlcode=RC_NIMPL;
	return;
    }
    if (SQLAllocHandle(SQL_HANDLE_DBC, envhandle, &conhandle)!=SQL_SUCCESS)
    {
	sqd_fillca(ca, SQL_HANDLE_ENV, envhandle);
	return;
    }

/* FIXME: exclusive
    if (exclusive && (SQLSetConnectAttr(&conhandle,...)!=SQL_SUCCESS)
    {
	sqd_fillca(ca, SQL_HANDLE_ENV, envhandle);
	return;
    } */
    if (SQLSetConnectAttr(conhandle, SQL_ATTR_AUTOCOMMIT,
			  (SQLPOINTER) SQL_AUTOCOMMIT_OFF, 0)!=SQL_SUCCESS)
	goto bad_exit;
/*
** Although this is the documented default, it has to be set somewhere
** and it's best not done here, because, although all other connections
** have been set to SQL_CONCURRENT_TRANS, this call might still fail with
** a CLI0126E
** 
    if (SQLSetConnectAttr(conhandle, SQL_ATTR_CONNECTTYPE,
			  (SQLPOINTER) SQL_CONCURRENT_TRANS, 0)!=SQL_SUCCESS)
	goto bad_exit;
*/    if (SQLConnect(conhandle, h, SQL_NTS, user,
		   SQL_NTS, passwd, SQL_NTS)!=SQL_SUCCESS)
    {
	sqd_fillca(ca, SQL_HANDLE_DBC, conhandle);
	IGNORE SQLFreeHandle((SQLSMALLINT) SQL_HANDLE_DBC,
			     conhandle);
	return;
    }
allcool:
    if (*private_area=malloc(sizeof(SQLHANDLE)))
	memcpy(*private_area, &conhandle, sizeof(SQLHANDLE));
    return;

bad_exit:
    sqd_fillca(ca, SQL_HANDLE_DBC, conhandle);
}
 
/*
** set current connection
*/
DLLDECL void sqd_setconnection(char *host, char *name, ca_t *ca,
			       char *private_area)
{
    if (!host && !name)
	ca->sqlcode=RC_NIMPL;
    else
	ca->sqlcode=0;
}
 
/*
** disconnect from engine
*/
DLLDECL void sqd_disconnect(char *host, char *name, ca_t *ca, char *private_area)
{
    if (host==(void *) -1 || name==(void *) -1)
	return;					/* functionality is obsolete */
    else if (!host && !name)
	ca->sqlcode=RC_NIMPL;
    else if (private_area)
    {
	SQLHANDLE conhandle=*((SQLHANDLE *) private_area);

	sqd_zeroca(ca);
/*
** and there I thought that database engines would rollback open transactions
** on disconnection....
*/
	IGNORE SQLEndTran(SQL_HANDLE_DBC, conhandle, SQL_ROLLBACK);
	if (SQLDisconnect(conhandle)!=SQL_SUCCESS)
	    sqd_fillca(ca, SQL_HANDLE_DBC, conhandle);
	IGNORE SQLFreeHandle((SQLSMALLINT) SQL_HANDLE_DBC, conhandle);
	free(private_area);
    }
    else
	ca->sqlcode=EINVAL;
}

/*
**  instantiate stmt structure
*/
DLLDECL fgw_db2stmttype *sqd_newstatement()
{
    fgw_db2stmttype *st;
    int i;

    if ((st=(fgw_db2stmttype *) malloc(sizeof(fgw_db2stmttype)))!=NULL)
	memset(st, 0, sizeof(fgw_db2stmttype));
    return st;
}

/*
**  free used statement resources
*/
DLLDECL void sqd_freestatement(fgw_stmttype *st, int clean)
{
    struct fgw_db2stmt *st_p;
    int i;
    struct sqlvar *c;

    if (st && (st_p=(fgw_db2stmttype *) st->sqlstmt))
    {
	if (clean && st->curstate>=ST_EXECUTABLE)
	    IGNORE SQLFreeHandle((SQLSMALLINT) SQL_HANDLE_STMT, st_p->stmtid);
	if (st_p->sqslda_out)
	    free(st_p->sqslda_out);
	if (st_p->sqslda_in)
	    free(st_p->sqslda_in);
	if (st_p->outbuf)
	    free(st_p->outbuf);
	if (st_p->inbuf)
	    free(st_p->inbuf);
	if (st_p->exprlist)
	    free(st_p->exprlist);
	free(st_p);
    }
}

/*
**  mark statement resources for later usage
*/
DLLDECL void sqd_closestatement(fgw_stmttype *st)
{
    struct fgw_db2stmt *st_p;
    int i;
    struct sqlvar *c;

    if (st && (st_p=(fgw_db2stmttype *) st->sqlstmt) &&
	st->curstate>=ST_NEWLYOPENED)
    {
	i=SQLFreeStmt(st_p->stmtid, SQL_CLOSE);
	st->curstate=ST_DECLARED;
    }
}

/*
** set cursor name
*/
DLLDECL void sqd_setcursor(fgw_stmttype *st, char *curs)
{
    char *end;

    IGNORE SQLSetCursorName(((fgw_db2stmttype *) st->sqlstmt)->stmtid, curs,
			    strlen(curs));
}
 
static void sqd_describe(fgw_stmttype *st)
{
    fgw_db2stmttype *st_p=(fgw_db2stmttype *) st->sqlstmt;
    long colsize, bufsize=0;
    int i, es, ss;
    SQLSMALLINT dummyl;
    
    sqslda_t *c;
    exprstack_t *e;
    short *n;

    sqd_zeroca(st->ca);
/*
** allocate expression holder and sqslda
*/
    i=sizeof(exprstack_t)*st_p->colcount;
    if (!(st_p->exprlist=(exprstack_t *) malloc(es=sizeof(exprstack_t)*
						   st_p->colcount)) ||
	!(st_p->sqslda_out=(sqslda_t *) malloc(ss=sizeof(sqslda_t)*
						   st_p->colcount)))
    {
	st->ca->sqlcode=RC_NOMEM;
	return;
    }
    memset(st_p->exprlist, 0, es);
    memset(st_p->sqslda_out, 0, ss);
/*
** Now's the tricky bit: first we scan the columns converting types &
** adjusting sizes if necessary, adding up the total memory required
*/
    for (i=0, c=st_p->sqslda_out, e=st_p->exprlist;
	 i<st_p->colcount; i++, c++, e++)
    {
	if (st->ca->sqlcode=SQLDescribeCol(st_p->stmtid, i+1,
					   (char *) &c->colname,
					   (SQLSMALLINT) COLNAMESIZE,
					   &dummyl, &c->sqltype,
					   &c->sqllen, &c->scale, &c->nullable))
	{
	    sqd_fillca(st->ca, SQL_HANDLE_STMT, st_p->stmtid);
	    return;
	}
	switch (c->convtype=c->sqltype)
	{
	  case SQL_INTEGER:
	  case SQL_BIGINT:
	  case SQL_SMALLINT:
	    e->type=CINTTYPE;
	    c->convtype=SQL_INTEGER;
	    c->sqllen=sizeof(int);
	    c->sqldata=(char *) &e->val.intvalue;
	    break;
/*
** have the engine do the decimal conversion for us:
** sqlbindcol() has a hangup with SQL_C_DECIMAL and SQL_C_DECIMAL_IBM
** (throws a CLI0125E / HY010 - telling that for decimals SQL_C_DEFAULT
** is SQL_CHAR!)
*/
	  case SQL_DECIMAL:
	    e->type=CDECIMALTYPE;
	    e->val.string=(char *) bufsize; /* just an offset for now! */
	    c->convtype=SQL_CHAR;
	    colsize=1+((c->sqllen & 0xFF)| 7);
	    bufsize+=DTIMESIZE+colsize;
	    c->sqllen=c->sqllen*256+c->scale;
	    break;
	  case SQL_FLOAT:
	  case SQL_DOUBLE:
	  case SQL_NUMERIC:
	  case SQL_REAL:
	    e->type=CDOUBLETYPE;
	    c->convtype=SQL_DOUBLE;
	    c->sqllen=sizeof(double);
	    c->sqldata=(char *) &e->val.real;
	    break;
	  case SQL_BINARY:
	  case SQL_VARBINARY:
	  case SQL_LONGVARBINARY:
	  case SQL_BLOB:
	    e->type=FGWBYTETYPE;
	    c->convtype=SQL_C_BINARY;
	    e->val.string=(char *) bufsize; /* just an offset for now! */
	    colsize=c->rlen=c->sqllen;
	    bufsize+=colsize;
	    break;
	  case SQL_CLOB:
	  case SQL_DBCLOB:
	    e->type=CSTRINGTYPE;
	    c->convtype=SQL_CHAR;
	    e->val.string=(char *) bufsize; /* just an offset for now! */
	    colsize=c->rlen=c->sqllen;
	    bufsize+=colsize;
	    break;
	  case SQL_DATETIME:
	  case SQL_TYPE_TIME:
	  case SQL_TYPE_TIMESTAMP:
	    e->type=CDTIMETYPE;
	    e->val.string=(char *) bufsize; /* just an offset for now! */
	    c->convtype=SQL_CHAR;
	    c->sqllen++;	/* take into account string terminator */
	    colsize=1+(c->sqllen | 7);
	    bufsize+=DTIMESIZE+colsize;
	    break;
	  case SQL_TYPE_DATE:
	    e->type=CDATETYPE;
/* dirty, as we will overwrite the string union member with a date, but works */
	    e->val.string=(char *) bufsize; /* just an offset for now! */
	    c->convtype=SQL_CHAR;
	    c->sqllen++;	/* take into account string terminator */
	    colsize=1+(c->sqllen | 7);
	    bufsize+=colsize;
	    break;
	  default:
	    e->type=CSTRINGTYPE;
	    e->val.string=(char *) bufsize; /* just an offset for now! */
	    c->convtype=SQL_CHAR;
	    c->sqllen++;	/* take into account string terminator */
	    colsize=1+(c->sqllen | 7);
	    bufsize+=colsize;
	    break;
	}
    }
/*
** then we allocate memory & fixup the pointers
*/
    if (!(st_p->outbuf=(char *) malloc(bufsize+
				   st_p->colcount*sizeof(int))))
    {
	st->ca->sqlcode=RC_NOMEM;
	return;
    }
    for (i=st_p->colcount, c=st_p->sqslda_out,
	 e=st_p->exprlist, n=(short *) (st_p->outbuf+bufsize);
	 i; i--, c++, e++, n++)
    {
	if (!c->sqldata)
	{
	    e->val.string+=(long) (st_p->outbuf);
	    if (e->type==CDTIMETYPE)
	    {
		c->sqldata=e->val.string+DTIMESIZE;
		if (c->sqltype==SQL_TYPE_TIME)
		    e->val.datetime->dt_qual=TU_DTENCODE(TU_HOUR, TU_SECOND);
		else
		    e->val.datetime->dt_qual=TU_DTENCODE(TU_YEAR, TU_F5);
	    }
	    else if (e->type==CDECIMALTYPE)
		c->sqldata=e->val.string+DTIMESIZE;
	    else
		c->sqldata=e->val.string;
	}
    }
/*
** finally go bind the buffers
*/
    for (i=1, c=st_p->sqslda_out; i<=st_p->colcount; i++, c++)
    {
	if (st->ca->sqlcode=SQLBindCol(st_p->stmtid, (SQLUSMALLINT) i,
				       c->convtype, c->sqldata, c->sqllen,
				       &c->rlen))
	{
	    sqd_fillca(st->ca, SQL_HANDLE_STMT, st_p->stmtid);
	    return;
	}
    }
    st->options|=SO_STATICBUF;
    st->ca->sqlcode=0;
}

/*
**  prepare statement
*/
DLLDECL void sqd_prepare(fgw_stmttype *st, char *query)
{
    struct fgw_db2stmt *st_p;
    char *nq=NULL;
    int params;
    SQLHANDLE conhandle;

    if (st && st->curstate==ST_UNINITIALIZED &&
	(st_p=(fgw_db2stmttype *) st->sqlstmt))
    {
	sqd_zeroca(st->ca);
/*
** api kind of statement?
** STOP PRESS - SET ISOLATION made it in V8, these are implemented not to
** upset the DB2 hard cores...
*/
	if ((st_p->stmttype=db2api_parser(query, 0, &st->ca->sqlcode))>=CLI_UR)
	{
	    st->curstate=ST_EXECUTABLE;
	    return;
	}
	else if (st->ca->sqlcode)
	    return;
/*
** db2 doesn't like certain chars so if we find any, we just get
** them out of the way
** FIXME: this fixes string constants too, while it probably ought to
** return an error
*/
	if (strpbrk(query, NONOCHARS))
	{
	    char *s, *d;

	    if (!(nq=(char *) malloc(strlen(query)+1)))
	    {
		st->ca->sqlcode=RC_NOMEM;
		return;
	    }
	    for (s=query, d=nq; *s; s++, d++)
	    {
		if (strchr(NONOCHARS, *s))
		    *d=' ';
		else
		    *d=*s;
	    }
	    *d='\0';
	}
	conhandle=*((SQLHANDLE *) st->con->private_area);
	if (SQLAllocHandle(SQL_HANDLE_STMT, conhandle,
			   &st_p->stmtid)!=SQL_SUCCESS)
	{
	    sqd_fillca(st->ca, SQL_HANDLE_DBC, conhandle);
	    if (nq)
		free(nq);
	    return;
	}
	if (SQLPrepare(st_p->stmtid, (char *) (nq? nq: query),
		       SQL_NTS)!=SQL_SUCCESS)
	    goto badexit;
	if (SQLNumResultCols(st_p->stmtid, (SQLSMALLINT *) &st_p->colcount)!=
		SQL_SUCCESS)
	    goto badexit;
	if (st_p->colcount || (st_p->stmttype==CLI_CALL &&
			      !(st->options & SO_INSCURS)))
	{
	    sqd_describe(st);
	    if (st->ca->sqlcode)
	    {
		if (nq)
		    free(nq);
		return;
	    }
	    if (SQLSetStmtAttr(st_p->stmtid, SQL_ATTR_CURSOR_HOLD,

			/* This throws a warning if I don't cast the
			   32 bit option to a long. blame CLI. */
			(SQLPOINTER) ((long) ((st->options & SO_WITHHOLD)?
			 SQL_CURSOR_HOLD_ON: SQL_CURSOR_HOLD_OFF)), 0)!=
		  SQL_SUCCESS)
		goto badexit;
	    st->curstate=ST_DECLARED;
	}
/*
** just an executable statement with placeholders
*/
	else
	{
	    st->curstate=ST_EXECUTABLE;
	    st->ca->sqlcode=0;
	}
    }
    else if (st)
	st->ca->sqlcode=RC_INSID;
    if (nq)
	free(nq);
    return;

badexit:
    if (nq)
	free(nq);
    sqd_fillca(st->ca, SQL_HANDLE_STMT, st_p->stmtid);
    SQLFreeHandle((SQLSMALLINT) SQL_HANDLE_STMT, st_p->stmtid);
}

/*
** allocates input sqlda
*/
DLLDECL void sqd_allocateda(fgw_stmttype *st, int entries)
{
    fgw_db2stmttype *st_p=(fgw_db2stmttype *) st->sqlstmt;
    sqslda_t *c;
    int i, clear_entries;
    char *s, *inbuf;

    if (st_p==NULL)
	st->ca->sqlcode=RC_INSID;
    st_p->phcount=0;
    clear_entries=(st_p->sqslda_entries>0);
    if (st_p->sqslda_entries>=entries)
        return;
    if (inbuf=realloc(st_p->inbuf, entries*CNVSTRSIZE))
	st_p->inbuf=inbuf;
    else
    {
	st->ca->sqlcode=RC_NOMEM;
	return;
    }
    if ((c=(sqslda_t *) realloc(st_p->sqslda_in, entries*sizeof(sqslda_t))))
    {
	st_p->sqslda_in=c;
	st_p->sqslda_entries=entries;
/*
** initialize placeholders
*/
	for (i=0, c=st_p->sqslda_in, s=st_p->inbuf;
	     i<entries; i++, c++)
	{
	    c->sqltype=SQL_CHAR;
	    c->sqldata=s;
	    *s='\0';
	    s+=CNVSTRSIZE;
/*
** unbind existing parameters
*/
	if (clear_entries)
	    IGNORE SQLBindParameter(st_p->stmtid, (SQLUSMALLINT) i+1, SQL_PARAM_INPUT,
                            c->sqltype, c->sqltype, 0, (SQLSMALLINT) 0, NULL, 0, &c->rlen);
	}
    }
    else
	st->ca->sqlcode=RC_NOMEM;
}

/*
** sets next placeholder
*/
DLLDECL void sqd_bindda(fgw_stmttype *st, int entry, int type,
			int size, char *value)
{
    fgw_db2stmttype *st_p=(fgw_db2stmttype *) st->sqlstmt;
    sqslda_t *c;
    exprstack_t e;
    int ind;

    if (st_p==NULL || entry<0 || entry>=st_p->sqslda_entries)
	return;
    st_p->phcount++;
    c=st_p->sqslda_in+entry;
    e.type=type;
    e.length=size;
    c->sqllen=size;
    c->scale=0;
    c->rlen=XSL_CALL(rxu_isnull)(&e)? SQL_NULL_DATA: size;
    switch (type)
    {
      case FGWBYTETYPE:
	c->sqltype=c->convtype=SQL_BLOB;
	c->sqldata=value;
	e.val.string=value;
	break;
      case CINTTYPE:
	c->sqltype=c->convtype=SQL_INTEGER;
	c->sqldata=value;
	break;
      case CDOUBLETYPE:
	c->sqltype=c->convtype=SQL_DOUBLE;
	c->sqldata=value;
	break;
      case CDECIMALTYPE:
      case CMONEYTYPE:
	st->options|=SO_BUFCNV;
/*
** same issue as SqlBindCol() with SqlBindParameter()
** with SQL_C_DECIMAL_IBM and SQL_DECIMAL and CLI0125E
** convert to char and let the engine sort this out
*/
	e.val.decimal=(fgw_dec_t *) value;
	c->sqltype=c->convtype=SQL_CHAR;
	c->sqldata=XSL_CALL(rxu_tostring)(&e, c->sqldata, NULL);
	if (!XSL_CALL(rxu_isnull)(&e))
	{
	    c->sqllen=strlen(c->sqldata);
	    c->rlen=c->sqllen;
	    size=c->sqllen;
	}
	break;
      case CDATETYPE:
	st->options|=SO_BUFCNV;
	c->sqltype=c->convtype=SQL_CHAR;
	e.val.intvalue=*(int *) value;
/* ditto */
	c->sqldata=XSL_CALL(rxu_topictstring)(&e, c->sqldata, CNVSTRSIZE,
			 	    "yyyy-mm-dd", CDATETYPE, 0);
	if (!XSL_CALL(rxu_isnull)(&e))
	{
	    c->sqllen=strlen(c->sqldata);
	    c->rlen=c->sqllen;
	    size=c->sqllen;
	}
	break;
/*
** timestamps handled here as db2 accepts ODBC format. datetimes
** need to be either year to second/fraction or hour to second
*/
      default:
	if (type!=CSTRINGTYPE)
	    st->options|=SO_BUFCNV;
	c->sqltype=c->convtype=SQL_CHAR;
	e.val.string=value;
/* ditto */
	c->sqldata=XSL_CALL(rxu_tostring)(&e, c->sqldata, &ind);
	if (!ind)
	{
	    c->sqllen=strlen(c->sqldata);
	    c->rlen=c->sqllen;
	    size=c->sqllen;
	}
	break;
    }
    if (SQLBindParameter(st_p->stmtid, (SQLUSMALLINT) entry+1, SQL_PARAM_INPUT,
			    c->sqltype, c->convtype, c->sqllen,
			    (SQLSMALLINT) c->scale, c->sqldata, size,
			    &c->rlen)!=SQL_SUCCESS)
	sqd_fillca(st->ca, SQL_HANDLE_STMT, st_p->stmtid);
}

/*
**  execute non select statement
*/
DLLDECL void sqd_immediate(fgw_stmttype *st)
{
    struct fgw_db2stmt *st_p;
    int i, r=0;

    sqd_zeroca(st->ca);
    if (!st)
	return;
    else if (!(st_p=(fgw_db2stmttype *) st->sqlstmt))
	st->ca->sqlcode=RC_INSID;
    else if (st->curstate!=ST_EXECUTABLE)
	st->ca->sqlcode=RC_NEXEC;
    else switch(st_p->stmttype)
    {
      case CLI_UR:
	r=SQLSetConnectAttr((SQLHDBC) *((SQLHANDLE *) st->con->private_area),
				   SQL_ATTR_TXN_ISOLATION,
				   (SQLPOINTER) SQL_TXN_READ_UNCOMMITTED, 0);
	break;
      case CLI_CS:
	r=SQLSetConnectAttr((SQLHDBC) *((SQLHANDLE *) st->con->private_area),
				   SQL_ATTR_TXN_ISOLATION,
				   (SQLPOINTER) SQL_TXN_READ_COMMITTED, 0);
	break;
      case CLI_RR:
	r=SQLSetConnectAttr((SQLHDBC) *((SQLHANDLE *) st->con->private_area),
				   SQL_ATTR_TXN_ISOLATION,
				   (SQLPOINTER) SQL_TXN_REPEATABLE_READ, 0);
	break;
      case CLI_RS:
	r=SQLSetConnectAttr((SQLHDBC) *((SQLHANDLE *) st->con->private_area),
				   SQL_ATTR_TXN_ISOLATION,
				   (SQLPOINTER) SQL_TXN_SERIALIZABLE, 0);
	break;
      default:
	r=SQLExecute(st_p->stmtid);
	if (r==SQL_SUCCESS ||
/*
** updates and deletes with no where clause fail with SQL_NO_DATA 
** or SQL_SUCCESS_WITH_INFO but succeed!
** how annoying!
*/
	    r==SQL_SUCCESS_WITH_INFO)
	{
	    IGNORE SQLRowCount(st_p->stmtid, (SQLINTEGER *) &st->ca->sqlerrd2);
/*
** this returns -1 for anything that isn't an insert, update, delete, merge
** how annoying!
*/
	    if (st->ca->sqlerrd2<0)
		st->ca->sqlerrd2=0;
	}
	else if (r==SQL_NO_DATA)
	    st->ca->sqlcode=RC_NOTFOUND;
	else
	    sqd_fillca(st->ca, SQL_HANDLE_STMT, st_p->stmtid);
	return;
    }
    if (r!=SQL_SUCCESS)
	sqd_fillca(st->ca, SQL_HANDLE_STMT, st_p->stmtid);
}

/*
**  loads next row
*/
DLLDECL void sqd_nextrow(fgw_stmttype *st)
{
    struct fgw_db2stmt *st_p;
    int r;

    if (!st)
	return;
    else if (!(st_p=(fgw_db2stmttype *) st->sqlstmt))
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
	sqd_zeroca(st->ca);
	if (SQLExecute(st_p->stmtid)!=SQL_SUCCESS)
	{
	    sqd_fillca(st->ca, SQL_HANDLE_STMT, st_p->stmtid);
	    return;
	}
/*
** for CALL statements, we do not get to know if a procedure returns anything
** or not until we execute it, which is a bit of a pain. We assume all routines
** to be cursory, unless part of the redirection clause, and if they return
** nothing, we set RC_NOTFOUND to skip FOREACH blocks
**
** the moron who thought that implementing routines this way was a good
** idea ought to be lined against a wall for humanity's and his own good...
*/
	if (st_p->stmttype==CLI_CALL)
	{
	    sqd_describe(st);
	    if (st->ca->sqlcode)
		return;
	    if (!st_p->colcount)
	    {
		st->ca->sqlcode=RC_NOTFOUND;
		return;
	    }
	}
    }
    if ((r=SQLFetch(st_p->stmtid))==SQL_SUCCESS || r==SQL_SUCCESS_WITH_INFO)
	st->curstate=ST_OPENED;
    else if (r==SQL_NO_DATA)
	st->ca->sqlcode=RC_NOTFOUND;
    else
	sqd_fillca(st->ca, SQL_HANDLE_STMT, st_p->stmtid);
}

/*
** returns next field in buffer
*/
DLLDECL exprstack_t *sqd_nexttoken(int field, fgw_stmttype *st_p)
{
    exprstack_t *e;
    sqslda_t *col;
    char *c;

    if (field>=((fgw_db2stmttype *) st_p->sqlstmt)->colcount)
	return NULL;
    col=((fgw_db2stmttype *) st_p->sqlstmt)->sqslda_out+field;
    e=((fgw_db2stmttype *) st_p->sqlstmt)->exprlist+field;
    if (col->rlen==SQL_NULL_DATA)
	e->length=-1;
    else switch (e->type)
    {
      case FGWBYTETYPE:
	e->length=col->rlen;
	break;
      case CSTRINGTYPE:
	*(e->val.string+col->rlen)='\0';
	break;
/*
** sqldata gets nullified to avoid needless conversions in subsequent
** calls over the same row+column
*/
      case CDTIMETYPE:
	if (*col->sqldata)
	{
	    if (col->rlen>10)			/* dirty but it works */
	    {
/*
** db2 returns a 6 digit fraction and this upsets dtcvfmtasc(), however,
** setting sqllen to 26 doesn't seem to work: fraction still comes up with
** 6 digits!  db2, as expected, truncates the fraction to 4 digits when
** sqllen is set to 25. To avoid any further debate, we just axe it by hand!
*/
		*(col->sqldata+25)='\0';
		IGNORE XSL_CALL(rxu_cvfmtdtime)(e, col->sqldata, "%Y-%m-%d-%H.%M.%S%F5");
	    }
	    else
		IGNORE XSL_CALL(rxu_cvfmtdtime)(e, col->sqldata, "%H.%M.%S");
	    *col->sqldata='\0';
	}
	break;
      case CDATETYPE:
	if (*col->sqldata)
	{
	    IGNORE XSL_CALL(rxu_cvfmtdate)(e, col->sqldata, "yyyy-mm-dd");
	    *col->sqldata='\0';
	}
	break;
      case CDECIMALTYPE:
	if (*col->sqldata)
	{
	    IGNORE XSL_CALL(rxu_cvdecimal)(e, col->sqldata);
	    *col->sqldata='\0';
	}
	break;
    }
    return e;
}

/*
** returns field count
*/
DLLDECL int sqd_counttokens(fgw_stmttype *st_p)
{
    return ((fgw_db2stmttype *) st_p->sqlstmt)->colcount;
}

/*
** returns error message string
*/
DLLDECL void sqd_errmsg(int err, char *buf, int len)
{
    int l=(errbuflen>len)? len: errbuflen;

    if (l<1)
	l=1;

    /* FIXME */
    if (err==sqlcode)
    {
	memcpy(buf, errbuf, l);
	buf[l]='\0';
    }
}

/*
** sends a break to the database engine
*/
DLLDECL void sqd_break()
{
    IGNORE sqleintr();
}

/*
** returns a description of the selected columns
*/
DLLDECL void sqd_describecol(fgw_stmttype *st_p, int column, exprstack_t *d)
{
    sqslda_t *col;
    exprstack_t *el;

    if (column<0 ||
	column>=((fgw_db2stmttype *) st_p->sqlstmt)->colcount)
	memset(d, 0, sizeof(exprstack_t));
    else
    {
	col=((fgw_db2stmttype *) st_p->sqlstmt)->sqslda_out+column;
	el=((fgw_db2stmttype *) st_p->sqlstmt)->exprlist+column;
	d->type=el->type;
	switch (d->type)
	{
	  case CDECIMALTYPE:
	    d->length=((char *) &col->rlen)[0]+1;
	    break;
	  default:
	    d->length=el->length;
	}
	d->val.string=col->colname;
    }
}
