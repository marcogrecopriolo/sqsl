/*
	CSQDC.c  -  Sql interpreter ifx4 dynamic sql driver

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Jan 97
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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

/*
**  This is one dirty HACK, plus I have to maintain two files as there is
**  no way you can feed both the hack and 5.x esql/c to c4gl 4 & 6, and keep
**  them both happy
**
**  WARNING: I have last compiled this in 2000, and it's unlikely that I'll
**  compile it ever again, as getting hold of a c4gl V4 is not exactly trivial
**  You are well and truly on your own
*/
#include <sqlca.h>
#include <sqlda.h>
#include <sqlstype.h>
#include <sqltypes.h>
#include <varchar.h>
#include <sqlhdr.h>
#ifdef CASTVALP
#include <value.h>
#endif
#include <string.h>
#include "cosdc.h"
#include "ccmnc.h"
#include "chstc.h"
#include "csqec.h"
#include "csqoc.h"
#include "csqrc.h"
#include "crxcc.h"
#include "csqsc.h"

#define SQ_EXECPROC 56		/* not defined in 4.x */
#ifndef MASKNONULL
#define MASKNONULL(x) (x & 0xFF)
#endif

typedef struct sqlvar_struct sqlva;

#define IDLENGTH 18
typedef struct fgw_ifxstmt
{
    _SQCURSOR _SQ1;
    char curname[IDLENGTH];	/* name of cursor */
    char *sqlbuf;		/* data returned from engine */
    struct sqlda *sqlda_ptr;	/* sqlda area */
    struct sqlda sqlda_in;	/* input sqlda area */
    int sqlda_entries;		/* current # of allocated placeholder entries */`
    exprstack_t *exprlist;	/* stack containing one row of data */
} fgw_ifxstmttype;

/*
**  esql/c 4 does not like _SQCURSOR structs on the heap (segv assured)
**  so we spend our time moving structures to and forth this static one
*/
static _SQCURSOR _SQ1;

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
**  connect to an engine
*/
DLLDECL sqd_connect(host, as, user, passwd, opt, ca, private)
char *host;
char *as;
char *user;
char *passwd;
char *opt;
ca_t *ca;
char **private;
{
    if (host)
	ca->sqlcode=RC_NIMPL;
}
 
/*
** set current connection
*/
DLLDECL sqd_setconnection(host, name,  ca, private)
char *host, *name;
ca_t *ca;
char *private;
{
    if (host || name)
	ca->sqlcode=RC_NIMPL;
}
 
/*
** disconnect from engine
*/
DLLDECL sqd_disconnect(host, name, ca, private)
char *host, name;
ca_t *ca;
char *private;
{
    if (host==(char *) -1 || name==(char *) -1)
	sqldetach();
    else
	ca->sqlcode=RC_NIMPL;
}

/*
**  instantiate stmt structure
*/
DLLDECL fgw_ifxstmttype *sqd_newstatement()
{
    fgw_ifxstmttype *st;

    if ((st=(fgw_ifxstmttype *) malloc(sizeof(fgw_ifxstmttype)))!=NULL)
	memset(st, 0, sizeof(fgw_ifxstmttype));
    return st;
}

/*
**  free used resources
*/
DLLDECL sqd_freestatement(st, clean)
fgw_stmttype *st;
int clean;
{
    fgw_ifxstmttype *st_p;
    int i;
    sqlva *c;

    if (st && (st_p=(fgw_ifxstmttype *) st->sqlstmt))
    {
	if (clean)
	{
 	    fgw_move((void *) &_SQ1, (void *) &st_p->_SQ1, sizeof(_SQCURSOR));
	    if (st_p->curstate>=ST_NEWLYOPENED)
		_iqclose(&_SQ1);
	    if (st_p->curstate>=ST_PREPARED)
		_iqfree(&_SQ1);
	}
/*
**  output sqlda: look for blob columns & free buffer
*/
	if (st_p->sqlda_ptr)
	{
	    for (i=st_p->sqlda_ptr->sqld, c=st_p->sqlda_ptr->sqlvar;
		 i; i--, c++)
		if ((c->sqltype==SQLTEXT || c->sqltype==SQLBYTES) &&
		    ((loc_t *) c->sqldata)->loc_buffer)
		    free(((loc_t *) c->sqldata)->loc_buffer);
	    free(st_p->sqlda_ptr);
	}
/*
**  input sqlda: look for blob columns & free buffer
*/
	if (st_p->sqlda_in.sqlvar)
	{
	    for (i=st_p->sqlda_entries, c=st_p->sqlda_in.sqlvar;
		 i; i--, c++)
		if (c->sqltype==SQLTEXT || c->sqltype==SQLBYTES)
		{
		    if (((loc_t *) c->sqldata)->loc_mflags)
			free(((loc_t *) c->sqldata)->loc_buffer);
		    free(c->sqldata);
		}
	    free(st_p->sqlda_in.sqlvar);
        }
	if (st_p->sqlbuf)
	    free(st_p->sqlbuf);
	free(st_p);
    }
}

/*
**  mark statement for later reuse
*/
DLLDECL sqd_closestatement(st)
fgw_stmttype *st;
{
    fgw_ifxstmttype *st_p;
    int i;
    sqlva *c;

    if (st && (st_p=(fgw_ifxstmttype *) st->sqlstmt))
    {
 	fgw_move((void *) &_SQ1, (void *) &st_p->_SQ1, sizeof(_SQCURSOR));
	if (st_p->curstate>=ST_NEWLYOPENED)
	{
	    _iqclose(&_SQ1);
	    st->curstate=ST_DECLARED;
	}
    }
}

/*
** set cursor name
*/
DLLDECL sqd_setcursor(st_p, curs)
fgw_stmttype *st_p;
char *curs;
{
}

/*
**  converts col to string
*/
static char *col2text(type, value, b)
int type;
char *value;
char *b;
{
    int s;
    char *c;
 
    if (risnull(type, value))
	return "";
    switch(type)
    {
      case CDECIMALTYPE:
      case SQLDECIMAL:
/* FIXME: I should probably handle DBMONEY here */
      case CMONEYTYPE:
      case SQLMONEY:
	s=2+2*(((dec_t *) value)->dec_ndgts>((dec_t *) value)->dec_exp?
	        ((dec_t *) value)->dec_ndgts : ((dec_t *) value)->dec_exp);
	if (dectoasc((dec_t *) value, b, s, -1))
	    *b='\0';
	else
	{
	    c=b+s;
	    while (*--c==' ');
	    *++c='\0';
	}
	break;
      case CDOUBLETYPE:
      case SQLFLOAT:
	sprintf(b, "%f", *(double *) value);
	break;
      case CINTTYPE:
      case SQLINT:
	sprintf(b, "%i", *(int *) value);
	break;
      case CDTIMETYPE:
      case SQLDTIME:
	dttoasc((dtime_t *) value, b);
	break;
      case CINVTYPE:
      case SQLINTERVAL:
	intoasc((intrvl_t *) value, b);
	break;
      case CDATETYPE:
      case SQLDATE:
	rdatestr(*(int *) value, b);
	break;
    }
    return b;
} 

/*
**  prepare statement
*/
DLLDECL sqd_prepare(st, query)
struct fgw_stmt *st;
char *query;
{
    struct fgw_ifxstmt *st_p;

    if (st && st->curstate==ST_UNINITIALIZED &&
	(st_p=(fgw_ifxstmttype *) st->sqlstmt))
    {
	memset(st->ca, 0, sizeof(ca_t));
	memset(_SQ1, 0, sizeof(_SQCURSOR));
	_iqprepare(&_SQ1, query);
        if (sqlca.sqlcode<0)
           goto badexit;
	st_p->curstate=ST_PREPARED;
	_iqdscribe(&_SQ1, &st_p->sqlda_ptr);
	if (sqlca.sqlcode<0)
	    goto badexit;
	if (sqlca.sqlcode==0 ||
	    (sqlca.sqlcode==SQ_EXECPROC && st_p->sqlda_ptr->sqld>0))
	{
	    long colsize, bufsize=0;
	    int i;
	    sqlva *c;
	    exprstack_t *e;
	    short *n;
 
/*
** allocate expression holder
*/
	    i=sizeof(exprstack_t)*st_p->sqlda_ptr->sqld;
	    if (!(st_p->exprlist=(exprstack_t *) malloc(i)))
	    {
		sqlca.sqlcode=RC_NOMEM;
		goto badexit;
	    }
	    memset(st_p->exprlist, 0, i);
/*
** Now's the tricky bit: first we scan the sqlda area converting types &
** adjusting sizes if necessary, adding up the total memory required
*/
	    for (i=st_p->sqlda_ptr->sqld, c=st_p->sqlda_ptr->sqlvar,
		 e=st_p->exprlist; i; i--, c++)
	    {
		c->sqldata=NULL;
		c->sqltype=MASKNONULL(c->sqltype);
		switch (c->sqltype)
		{
		  case CSHORTTYPE:
		  case CLONGTYPE:
		  case SQLSMINT:
		  case SQLINT:
		  case SQLSERIAL:
		    c->sqltype=CINTTYPE;
		    goto doint;
		  case SQLDATE:
		    c->sqltype=CDATETYPE;
		  case CINTTYPE:
		  case CDATETYPE:
doint:
		    c->sqllen=sizeof(int);
		    c->sqldata=&e->val.intvalue;
		    break;
		  case CFLOATTYPE:
		  case CDOUBLETYPE:
		  case SQLFLOAT:
		  case SQLSMFLOAT:
		    c->sqltype=CDOUBLETYPE;
		    c->sqllen=sizeof(double);
		    c->sqldata=&e->val.real;
		    break;
		  case CCHARTYPE:
		  case CFIXCHARTYPE:
		  case CSTRINGTYPE:
		  case SQLCHAR:
		    c->sqltype=CSTRINGTYPE;
		    c->sqllen++;
		    break;
		  case CVCHARTYPE:
		  case SQLVCHAR:
		    c->sqltype=CSTRINGTYPE;
		    c->sqllen=VCMAX(c->sqllen)+1;
		    break;
		  case CDECIMALTYPE:
		  case CMONEYTYPE:
		  case CDTIMETYPE:
		  case CINVTYPE:
		  case SQLTEXT:
		  case SQLBYTES:
		    break;
		  case SQLDECIMAL:
		    c->sqltype=CDECIMALTYPE;
		    break;
		  case SQLMONEY:
		    c->sqltype=CMONEYTYPE;
		    break;
		  case SQLDTIME:
		    c->sqltype=CDTIMETYPE;
		    break;
		  case SQLINTERVAL:
		    c->sqltype=CINVTYPE;
		    break;
/*
** to be on the safe side for unknown types - lets just hope that the relay
** module is nice enough to change LVARCHARs into legacy char types
*/
		  default:
		    c->sqltype=CSTRINGTYPE;
		    c->sqllen=256;
		    break;
		}
		e->type=c->sqltype;
		if (!c->sqldata)
		{
		    colsize=rtypmsize(c->sqltype, c->sqllen);
		    bufsize=rtypalign(bufsize, c->sqltype);
		    e->val.string=(char *) bufsize; /* just an offset for now! */
		    bufsize+=colsize;
		}
	    }
/*
** then we allocate memory & fixup the pointers
*/
	    if (!(st_p->sqlbuf=(char *) malloc(bufsize)))
	    {
		sqlca.sqlcode=RC_NOMEM;
		goto badexit;
	    }
	    for (i=st_p->sqlda_ptr->sqld, c=st_p->sqlda_ptr->sqlvar,
		 n=(short *) (st_p->sqlbuf+bufsize), e=st_p->exprlist;
		 i; i--, c++, n++)
	    {
		if (!c->sqldata)
		{
		    e->val.string+=(long) (st_p->sqlbuf);
		    c->sqldata=e->val.string;
		}
		c->sqlind=n;
		switch (c->sqltype)
		{
		  case SQLTEXT:
		    e->type=FGWVARTYPE;
		    ((loc_t *) c->sqldata)->loc_loctype=LOCMEMORY;
		    ((loc_t *) c->sqldata)->loc_bufsize=-1;
		    ((loc_t *) c->sqldata)->loc_buffer=NULL;
		    ((loc_t *) c->sqldata)->loc_indicator=0;
		    ((loc_t *) c->sqldata)->loc_mflags=0;
		  case SQLBYTES:
		    e->type=FGWBYTETYPE;
		    ((loc_t *) c->sqldata)->loc_loctype=LOCMEMORY;
		    ((loc_t *) c->sqldata)->loc_buffer=NULL;
		    if (st->options & SO_ALLOCBYTE)
		    {
			((loc_t *) c->sqldata)->loc_bufsize=-1;
			((loc_t *) c->sqldata)->loc_indicator=0;
			((loc_t *) c->sqldata)->loc_mflags=0;
		    }
		    else
		    {
			((loc_t *) c->sqldata)->loc_bufsize=0;
			((loc_t *) c->sqldata)->loc_indicator=-1;
		    }
		}
	    }
	    if (!st_p->curname[0])
		sprintf(st_p->curname, "fgwc%i", st_p->st_entry.int_id);
	    _iqddclcur(&_SQ1, st_p->curname, st_p->options & SO_WITHHOLD?
				4096: 0);
            if (sqlca.sqlcode!=0)
	    {
		st_p->curname[0]='\0';
 		goto badexit;
	    }
	    st_p->curstate=ST_DECLARED;
	    st->options|=SO_STATICBUF;
	    sqlca.sqlcode=0;
	}
/*
** just an executable statement with placeholders
*/
	else
	{
	    if (sqlca.sqlcode==SQ_EXECPROC || st_p->sqlda_ptr->sqld)
/*
** either an insert statement, or a SP that returns nothing)
*/
		st->options=|SO_OUTPUT;
	    st->curstate=ST_EXECUTABLE;
	    sqlca.sqlcode=0;
	}
badexit:
	if (st_p)
	    fgw_move((void *) &st_p->_SQ1, (void *) &_SQ1, sizeof(_SQCURSOR));
	st->ca->sqlcode=sqlca.sqlcode;
	st->ca->sqlerrd1=sqlca.sqlerrd[1];
	st->ca->sqlerrd2=sqlca.sqlerrd[2];
    }
    else if (st)
	st->ca->sqlcode=RC_INSID;
}

/*
** allocates input sqlda
*/
DLLDECL sqd_allocateda(st, entries)
fgw_stmttype *st;
int entries;
{
    fgw_ifxstmttype *st_p=(fgw_ifxstmttype *) st->sqlstmt;
    struct sqlvar_struct *sqlvar;
    sqlva *c;
    int i;
    short *n;

    if (st_p==NULL)
	st->ca->sqlcode=RC_INSID;
/*
** free prexisting blobs
*/
    if (st_p->sqlda_in.sqlvar)
    {
	for (i=st_p->sqlda_entries, c=st_p->sqlda_in.sqlvar;
	     i; i--, c++)
	    if (c->sqltype==SQLTEXT || c->sqltype==SQLBYTES)
	    {
		if (((ifx_loc_t *) c->sqldata)->loc_mflags)
		    free(((ifx_loc_t *) c->sqldata)->loc_buffer);
		free(c->sqldata);
		c->sqltype=CINTTYPE;
	    }
    }
    st_p->sqlda_in.sqld=0;
    if (st_p->sqlda_entries>=entries)
        return;
    if (sqlvar=(struct sqlvar_struct *) realloc(st_p->sqlda_in.sqlvar,
			entries*(sizeof(struct sqlvar_struct)+sizeof(int))))
    {
	st_p->sqlda_in.sqlvar=sqlvar;
	st_p->sqlda_entries=entries;
/*
** just to be on the safe side
*/
	for (i=0, sqlvar=st_p->sqlda_in.sqlvar,
	     n=(short *) (st_p->sqlda_in.sqlvar+entries);
	     i<entries; i++, sqlvar++, n++)
	{
	    memset(sqlvar, 0, sizeof(struct sqlvar_struct));
	    sqlvar->sqltype=CSTRINGTYPE;
	    sqlvar->sqldata="";
	    sqlvar->ind=n;
	}
    }
    else
	st->ca->sqlcode=RC_NOMEM;
}

/*
** sets next placeholder
*/
DLLDECL sqd_bindda(st, entry, type, size, value)
fgw_stmttype *st;
int entry;
int type;
int size;
char *value;
{
    fgw_ifxstmttype *st_p=(fgw_ifxstmttype *) st->sqlstmt;
    struct sqlvar_struct *sqlvar;

    if (st_p==NULL || entry<0 || entry>=st_p->sqlda_entries)
	return;
    st_p->sqlda_in.sqld++;
    
    sqlvar=st_p->sqlda_in.sqlvar+entry;
/*
** clean up previous loc_t allocations, if any
*/
    if ((sqlvar->sqltype==SQLTEXT || sqlvar->sqltype==SQLBYTES) &&
	sqlvar->sqldata)
    {
	if (((loc_t *) sqlvar->sqldata)->loc_mflags)
	    free(((loc_t *) sqlvar->sqldata)->loc_buffer);
	free(sqlvar->sqldata);
    }
/*
** setup default values
** we leave sqllen to 0 in most cases on purpose - the engine will
** truncate strings or extend datetimes/intervals if necessary
*/
    memset(sqlvar, 0, sizeof(sqlvar));
    switch (type)
    {
      case FGWBYTETYPE:
	sqlvar->sqltype=CSTRINGTYPE;
	sqlvar->sqldata="";
	break;
      case FGWVARTYPE:
	sqlvar->sqltype=CSTRINGTYPE;
	sqlvar->sqldata=value;
	break;
      default:
	sqlvar->sqltype=type;
	sqlvar->sqldata=value;
	*sqlvar->sqlind=(size<0)? -1: 0;
    }
    if (st_p->sqlda_ptr && st_p->sqlda_ptr->sqld==st_p->sqlda_in.sqld &&
	st->curstate==ST_EXECUTABLE)
    {
	loc_t *blob;
	sqlva *s;

/*
** see if we have blobs, we'll build a nice loc_t for them
*/
	s=st_p->sqlda_ptr->sqlvar+entry;
	switch(s->sqltype)
	{
	  case SQLTEXT:
	    if (!(blob=(loc_t *) malloc(sizeof(loc_t))))
	    {
		st->ca->sqlcode=RC_NOMEM;
		return;
	    }
	    if (sqlvar->sqltype==CSTRINGTYPE)
	    {
		blob->loc_buffer=value;
		blob->loc_bufsize=stleng(value);
		blob->loc_size=blob->loc_bufsize;
		blob->loc_mflags=0;
	    }
	    else if (blob->loc_buffer=(char *) malloc(MAXNUMLEN))
	    {
		blob->loc_bufsize=MAXNUMLEN;
		col2text(type, value, blob->loc_buffer);
		blob->loc_size=stleng(blob->loc_buffer);
		blob->loc_mflags=LOC_ALLOC;
	    }
	    else
	    {
		st->ca->sqlcode=RC_NOMEM;
		return;
	    }
	    blob->loc_indicator=(blob->loc_size<=0? -1: 0);
	    blob->loc_loctype=LOCMEMORY;
	    sqlvar->sqltype=SQLTEXT;
	    sqlvar->sqldata=(char *) blob;
	    break;
	  case SQLBYTES:
/*
** we only allow proper bytes into byte columns - better -617 than to silently
** corrupt stuff
*/
	    if (type==FGWBYTETYPE)
	    {
		if (!(blob=(loc_t *) malloc(sizeof(loc_t))))
		{
		    st->ca->sqlcode=RC_NOMEM;
		    return;
		}
		blob->loc_buffer=value;
		blob->loc_bufsize=size;
		blob->loc_size=blob->loc_bufsize;
		blob->loc_mflags=0;
		blob->loc_indicator=(blob->loc_size<=0? -1: 0);
		blob->loc_loctype=LOCMEMORY;
		sqlvar->sqltype=SQLBYTES;
		sqlvar->sqldata=(char *) blob;
	    }
	}
    }
}

/*
**  execute non select statement
*/
DLLDECL sqd_immediate(st_p)
fgw_stmttype *st_p;
{
    struct fgw_ifxstmt *st_p;

    if (!st)
	return;
    else if (!(st_p=(fgw_ifxstmttype *) st->sqlstmt))
	st->ca->sqlcode=RC_INSID;
    else if (st->curstate!=ST_EXECUTABLE)
	st->ca->sqlcode=RC_NEXEC;
    else
    {
	fgw_move((void *) &_SQ1, (void *) &st_p->_SQ1, sizeof(_SQCURSOR));
	_iqxecute(&_SQ1, 0, (struct sqlvar_struct *) 0,
		  (struct sqlda *) /* &st_p->sqlda_in */ 0, (struct value *) 0);
	fgw_move((void *) &st_p->_SQ1, (void *) &_SQ1, sizeof(_SQCURSOR));
	st->ca->sqlcode=sqlca.sqlcode;
	st->ca->sqlerrd1=sqlca.sqlerrd[1];
	st->ca->sqlerrd2=sqlca.sqlerrd[2];
    }
}

/*
**  loads next row
*/
DLLDECL sqd_nextrow(st)
fgw_stmttype *st;
{
    struct fgw_ifxstmt *st_p;

    if (!st)
	return;
    else if (!(st_p=(fgw_ifxstmttype *) st->sqlstmt))
    {
	st->ca->sqlcode=RC_INSID;
	return;
    }
    else if (st->curstate<ST_DECLARED)
    {
	st->ca->sqlcode=RC_NDECL;
	return;
    }
    fgw_move((void *) &_SQ1, (void *) &st_p->_SQ1, sizeof(_SQCURSOR));
    if (st->curstate==ST_DECLARED)
    {
	_iqcopen(&_SQ1, 0, (struct sqlvar_struct *) 0,
	     (struct sqlda *) /* &st_p->sqlda_in */ 0,
	     (struct value *) 0, 0);
	if (sqlca.sqlcode!=0)
	    goto badexit;
	st_p->curstate=ST_NEWLYOPENED;
    }
    _iqnftch(&_SQ1, 0, (struct sqlvar_struct *) 0, st_p->sqlda_ptr,
	     1, (long) 0, 0, (struct sqlvar_struct *) 0, (struct sqlda *) 0, 0);
    fgw_move((void *) &st_p->_SQ1, (void *) &_SQ1, sizeof(_SQCURSOR));
    if (!sqlca.sqlcode)
	st->curstate=ST_OPENED;
badexit:
    st_p->ca->sqlcode=sqlca.sqlcode;
    st_p->ca->sqlerrd1=sqlca.sqlerrd[1];
    st_p->ca->sqlerrd2=sqlca.sqlerrd[2];
}

/*
** returns next field in buffer
*/
DLLDECL exprstack_t *sqd_nexttoken(field, st_p)
int field;
fgw_stmttype *st_p;
{
    exprstack_t *e;
    sqlva *col;
    char *c;
    loc_t *b;
 
    if (field>=((fgw_ifxstmttype *) st_p->sqlstmt)->sqlda_ptr->sqld)
	return NULL;
    col=((fgw_ifxstmttype *) st_p->sqlstmt)->sqlda_ptr->sqlvar+field;
    e=((fgw_ifxstmttype *) st_p->sqlstmt)->exprlist+field;
    switch (col->sqltype)
    {
      case SQLTEXT:
	b=(loc_t *) col->sqldata;
	if (!b->loc_buffer || !b->loc_size || b->loc_indicator)
	{
	    e->length=-1;
	    e->val.string=""
	}
	else
	{
	    if (b->loc_bufsize==b->loc_size &&
		(c=(char *) realloc(b->loc_buffer, b->loc_bufsize+1)))
	    {
		b->loc_buffer=c;
		b->loc_bufsize++;
	    }
	    else
		b->loc_size--;
	    *(b->loc_buffer+b->loc_size)='\0';
	    e->val.string=c;
	    e->length=b->loc_size;
	}
	break;
      case SQLBYTES:
	b=(loc_t *) col->sqldata;
	e->val.string=((loc_t *) col->sqldata)->loc_buffer;
	if (b->loc_indicator)
	    e->length=-1;
	else
	    e->length=b->loc_size;
	break;
      default:
	e->length=(*col->sqlind<0)? -1: col->sqllen;
    }
    return e;
}

/*
** returns field count
*/
DLLDECL int sqd_counttokens(st_p)
fgw_stmttype *st_p;
{
    return ((fgw_ifxstmttype *) st_p->sqlstmt)->sqlda_ptr->sqld;
}

/*
** returns error message string
*/
DLLDECL sqd_errmsg(err, buf, len)
int err;
char *buf;
int len;
{
    char b1[256], b2[256]; 
 
    rgetmsg(err, (char *) &b1, (len<sizeof(b1))? len: sizeof(b1)-1);
    sprintf(b2, b1, sqlca.sqlerrm);
    sprintf(buf, "%i ", err);
    strncat(buf, b2, len-strlen(buf));
}
