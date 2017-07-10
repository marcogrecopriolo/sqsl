/*
	CSQDC.ec  -  SQSL interpreter ifx5+ dynamic sql driver

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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/*
   This code can be compiled verbatim in db2 mode using CliSDK 2.90 or
   greater, with the following limitations:

 * DATABASE cannot be prepared, thus there is no way to lock a database
   exclusively. If you have time you could enhance the parser to detect
   database statements and execute them statically

 * more often than not, the esql/c compiler uses the same symbol name
   when compiling in both db2 and ifmx mode, hence you cannot produce
   two sources out of this code and use them at the same time - in
   general wrong turns will be taken at symbol resolution time, and
   the db2 source will end up in the asf, or the ifmx source will try
   to use drda, depending on which source has been loaded first.
   For this reason the db2 version of the ifmx source is built but
   not installed by default. Copy it by hand if you really want it

 * esql/c db2 mode has been broken since CliSDK 3.50 or thereabout
   anyway! (the compiler throws a syntax error at the first EXEC SQL
   it encounters, no matter how simple the code you compile)

 * I need to experiment a bit with default connections. Db2 doesn't support
   them, but CliSDK seems to be simulating them. It the worse case,
   sqd_setconnection() can be changed to reject default connections
*/

/*
  The port to Cygwin requires that this code be linked as a dll:
  Cygwin has its own malloc()/free() implementation which clashes with
  the native Win32 one, as used by Microsoft, Borland or MinGW compilers,
  and by extension by the Informix esql/c runtime libraries.
  The net result is that freeing an sqlda_ptr populated by a DESCRIBE (which
  uses the Win32 malloc()) will fragorously segv, which isn't good.
  On the other hand, we require full Cygwin functionality to take advantage
  of a whole host of POSIX goodies.
  Thankfully this can be easily fixed detaching this source from the SQSL
  library, encapsulating it in its own source dll, which in turn will have
  been compiled and linked with MinGW.
  This has the added advantage that switching the default source is as
  easy as changing a #define in sqsl/csqsc.c
*/

#define ENABLE_CONNECT ((I4GLVER + 0) > 699) || \
		       ((ESQLVER + 0) < 500) || ((ESQLVER + 0) > 600)

#include <sqlca.h>
#include <sqlda.h>
#include <sqlstype.h>
#include <varchar.h>
#include <locator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cosdc.h"
EXEC SQL include "crxuc.h";
#include "crqfc.h"
#include "csqrc.h"
#include "crqxd.h"
#include "cxslc.h"

typedef struct sqlvar_struct sqlva;

EXEC SQL BEGIN DECLARE SECTION;
#define IDLENGTH 18

typedef struct fgw_ifxstmt
{
    char stmtname[IDLENGTH];	/* name of statement */
    char curname[IDLENGTH];	/* name of cursor */
    char *sqlbuf;		/* data returned from engine */
    struct sqlda *sqlda_ptr;	/* output sqlda area */
    struct sqlda sqlda_in;	/* input sqlda area */
    int sqlda_entries;		/* current # of allocated placeholder entries */
    exprstack_t *exprlist;	/* stack containing one row of data */
} fgw_ifxstmttype;
EXEC SQL END DECLARE SECTION;

#ifndef MASKNONULL
#define MASKNONULL(x) (x & 0xFF)
#endif

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
DLLDECL void sqd_connect(host, as, user, passwd, opt, ca, private)

/* esql/c compilers tend not to be ANSI compliant - at least,
   that's my excuse... */
EXEC SQL BEGIN DECLARE SECTION;
char *host;
char *as;
char *user;
char *passwd;
EXEC SQL END DECLARE SECTION;
char *opt;
ca_t *ca;
char **private;
{
    if (opt)
    {
	for (; *opt; opt++)
	    if (*opt != ' ')
		{
		sqlca.sqlcode=RC_PSNTX;
		return;
		}
    }
#if (ENABLE_CONNECT)
    if (as && host)
	if (user)
	    EXEC SQL connect to :host as :as user :user using :passwd
		with concurrent transaction;
	else
	    EXEC SQL connect to :host as :as
		with concurrent transaction;
    else if (host)
	if (user)
	    EXEC SQL connect to :host user :user using :passwd
		with concurrent transaction;
	else
	    EXEC SQL connect to :host
		with concurrent transaction;
    else
	EXEC SQL connect to default with concurrent transaction;
    ca->sqlcode=sqlca.sqlcode;
#else
    else if (host)
	ca->sqlcode=RC_NIMPL;
#endif
}
 
/*
** set current connection
*/
DLLDECL void sqd_setconnection(host, name, ca, private)
EXEC SQL BEGIN DECLARE SECTION;
char *host, *name;
EXEC SQL END DECLARE SECTION;
ca_t *ca;
char *private;
{
#if (ENABLE_CONNECT)
    if (name)
	EXEC SQL set connection :name;
    else
	EXEC SQL set connection default;
    ca->sqlcode=sqlca.sqlcode;
#else
    if (name)
	ca->sqlcode=RC_NIMPL;
#endif
}
 
/*
** disconnect from engine
*/
DLLDECL void sqd_disconnect(host, name, ca, private)
EXEC SQL BEGIN DECLARE SECTION;
char *host, *name;
EXEC SQL END DECLARE SECTION;
ca_t *ca;
char *private;
{
    if (host==(char *) -1 || name==(char *) -1)
	sqldetach();
#if (ENABLE_CONNECT)
    else if (name)
	EXEC SQL disconnect :name;
    else
	EXEC SQL disconnect default;
    ca->sqlcode=sqlca.sqlcode;
#else
    else if (name)
	ca->sqlcode=RC_NIMPL;
#endif
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
**  free used statement resources
*/
DLLDECL void sqd_freestatement(st, clean)
fgw_stmttype *st;
int clean;
{
EXEC SQL BEGIN DECLARE SECTION;
    struct fgw_ifxstmt *st_p;
EXEC SQL END DECLARE SECTION;
    int i;
    sqlva *c;

    if (st && (st_p=(fgw_ifxstmttype *) st->sqlstmt))
    {
	if (clean)
	{
	    if (st->curstate>=ST_NEWLYOPENED || (st->options & SO_INSCURS))
		EXEC SQL CLOSE :st_p->curname;
	    if (st_p->curname[0])
		EXEC SQL FREE :st_p->curname;
	    if (st_p->stmtname[0])
		EXEC SQL FREE :st_p->stmtname;
	}
/*
**  output sqlda: look for blob columns & free buffer
*/
	if (st_p->sqlda_ptr)
	{
	    for (i=st_p->sqlda_ptr->sqld, c=st_p->sqlda_ptr->sqlvar;
		 i; i--, c++)
		if ((c->sqltype==SQLTEXT || c->sqltype==SQLBYTES) &&
		    c->sqldata && ((ifx_loc_t *) c->sqldata)->loc_buffer)
			    free(((ifx_loc_t *)c->sqldata)->loc_buffer);
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
		    if (((ifx_loc_t *) c->sqldata)->loc_mflags)
			free(((ifx_loc_t *) c->sqldata)->loc_buffer);
		    free(c->sqldata);
		}
	    free(st_p->sqlda_in.sqlvar);
	}
	if (st_p->sqlbuf)
	    free(st_p->sqlbuf);
	if (st_p->exprlist)
	    free(st_p->exprlist);
	free(st_p);
    }
}

/*
**  close statement for later reusage
*/
DLLDECL void sqd_closestatement(st)
fgw_stmttype *st;
{
EXEC SQL BEGIN DECLARE SECTION;
    struct fgw_ifxstmt *st_p;
EXEC SQL END DECLARE SECTION;
    int i;
    sqlva *c;

    if (st && (st_p=(fgw_ifxstmttype *) st->sqlstmt) &&
	(st->curstate>=ST_NEWLYOPENED || (st->options & SO_INSCURS)))
    {
	EXEC SQL CLOSE :st_p->curname;
	st->curstate=ST_DECLARED;
    }
}

/*
** set cursor name
*/
DLLDECL void sqd_setcursor(st_p, curs)
fgw_stmttype *st_p;
char *curs;
{
    strncpy(((fgw_ifxstmttype *) st_p->sqlstmt)->curname, curs, IDLENGTH); 
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
DLLDECL void sqd_prepare(st, query)
fgw_stmttype *st;
EXEC SQL BEGIN DECLARE SECTION;
char *query;
EXEC SQL END DECLARE SECTION;
{
EXEC SQL BEGIN DECLARE SECTION;
    struct fgw_ifxstmt *st_p;
    struct sqlda *sqlda_ptr;	/* esql/c not so flexible with sqlda's */
    char sname[IDLENGTH];
EXEC SQL END DECLARE SECTION;

    if (st && st->curstate==ST_UNINITIALIZED &&
	(st_p=(fgw_ifxstmttype *) st->sqlstmt))
    {
	memset(st->ca, 0, sizeof(ca_t));
	sprintf(sname, "fgws%i", st->st_entry.int_id);
	EXEC SQL PREPARE :sname FROM :query;
	if (sqlca.sqlcode<0)
	   goto badexit;
	strcpy(st_p->stmtname, sname);
	EXEC SQL DESCRIBE :st_p->stmtname INTO sqlda_ptr;
	if (sqlca.sqlcode<0)
	    goto badexit;
	st_p->sqlda_ptr=sqlda_ptr;    /* esql/c not so flexible with sqlda's */
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
		 e=st_p->exprlist; i; i--, c++, e++)
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
		    c->sqldata=(char *) &e->val.intvalue;
		    break;
		  case CFLOATTYPE:
		  case CDOUBLETYPE:
		  case SQLFLOAT:
		  case SQLSMFLOAT:
		    c->sqltype=CDOUBLETYPE;
		    c->sqllen=sizeof(double);
		    c->sqldata=(char *) &e->val.real;
		    break;
		  case CCHARTYPE:
		  case CFIXCHARTYPE:
		  case SQLCHAR:
#ifdef SQLNCHAR
		  case SQLNCHAR:
#endif
		    c->sqltype=CSTRINGTYPE;
		  case CSTRINGTYPE:
		    c->sqllen++;
		    break;
		  case CVCHARTYPE:
		  case SQLVCHAR:
#ifdef SQLNVCHAR
		  case SQLNVCHAR:
#endif
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
** catch all for UDT's, extended data types etc. known to work with
** 4gl 7.3+, or more generally anything built over CliSDK. Known not
** to work with earlier products.
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
		    e->val.string=(char *) bufsize; /* just an offset for now */
		    bufsize+=colsize;
		}
	    }
/*
** then we allocate memory & fixup the pointers
*/
	    if (!(st_p->sqlbuf=(char *) malloc(bufsize+
					   st_p->sqlda_ptr->sqld*sizeof(int))))
	    {
		sqlca.sqlcode=RC_NOMEM;
		goto badexit;
	    }
	    for (i=st_p->sqlda_ptr->sqld, c=st_p->sqlda_ptr->sqlvar,
	         n=(short *) (st_p->sqlbuf+bufsize), e=st_p->exprlist;
		 i; i--, c++, e++, n++)
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
		    e->type=CSTRINGTYPE;
		    ((ifx_loc_t *) c->sqldata)->loc_loctype=LOCMEMORY;
		    ((ifx_loc_t *) c->sqldata)->loc_bufsize=-1;
		    ((ifx_loc_t *) c->sqldata)->loc_buffer=NULL;
		    ((ifx_loc_t *) c->sqldata)->loc_indicator=0;
		    ((ifx_loc_t *) c->sqldata)->loc_mflags=0;
		    break;
		  case SQLBYTES:
		    e->type=FGWBYTETYPE;
		    ((ifx_loc_t *) c->sqldata)->loc_loctype=LOCMEMORY;
		    ((ifx_loc_t *) c->sqldata)->loc_buffer=NULL;
		    if (st->options & SO_ALLOCBYTE)
		    {
			((ifx_loc_t *) c->sqldata)->loc_bufsize=-1;
			((ifx_loc_t *) c->sqldata)->loc_indicator=0;
			((ifx_loc_t *) c->sqldata)->loc_mflags=0;
		    }
		    else
		    {
			((ifx_loc_t *) c->sqldata)->loc_bufsize=0;
			((ifx_loc_t *) c->sqldata)->loc_indicator=-1;
		    }
                }
	    }
	    if (!st_p->curname[0])
	    {
		sname[3]='c';
		strcpy(st_p->curname, sname);
	    }
	    if (st->options & SO_WITHHOLD)
		EXEC SQL DECLARE :st_p->curname
		    CURSOR WITH HOLD FOR :st_p->stmtname;
	    else
		EXEC SQL DECLARE :st_p->curname CURSOR FOR :st_p->stmtname;
	    if (sqlca.sqlcode!=0)
	    {
		st_p->curname[0]='\0';
		goto badexit;
	    }
	    st->curstate=ST_DECLARED;
	    st->options|=SO_STATICBUF;
	    sqlca.sqlcode=0;
	}
/*
** just an executable statement with placeholders
*/
	else
	{
/*
** have to set up an insert cursor
*/
	    if ((st->options & SO_INSCURS) && 
		(sqlca.sqlcode==SQ_INSERT) && st_p->sqlda_ptr->sqld)
	    {
		sname[3]='c';
		strcpy(st_p->curname, sname);
		EXEC SQL DECLARE :st_p->curname CURSOR FOR :st_p->stmtname;
		if (sqlca.sqlcode!=0)
		    goto badexit;
		EXEC SQL OPEN :st_p->curname;
		if (sqlca.sqlcode!=0)
		{
		    if (sqlca.sqlcode!=-255)
			goto badexit;

		/*
		** CliSDK has the nasty habit of throwing a Not In Transaction
		** error if we try to declare / open an insert cursor on a 
		** logged database if we are not in transaction, even if
		** the table is raw or temp with no log.
		** we try to work around that
		*/
		EXEC SQL FREE :st_p->curname;
		st->options&=~SO_INSCURS;
		}
	    }
	    st->curstate=ST_EXECUTABLE;
	    sqlca.sqlcode=0;
	}
badexit:
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
DLLDECL void sqd_allocateda(st, entries)
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
	    sqlvar->sqlind=n;
	}
    }
    else
	st->ca->sqlcode=RC_NOMEM;
}

/*
** sets next placeholder
*/
DLLDECL void sqd_bindda(st, entry, type, size, value)
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
** clean up previous ifx_loc_t allocations, if any
*/
    if ((sqlvar->sqltype==SQLTEXT || sqlvar->sqltype==SQLBYTES) &&
	sqlvar->sqldata)
    {
	if (((ifx_loc_t *) sqlvar->sqldata)->loc_mflags)
	    free(((ifx_loc_t *) sqlvar->sqldata)->loc_buffer);
	free(sqlvar->sqldata);
    }
/*
** setup default values
** we leave sqllen to 0 in most cases on purpose - the engine will
** truncate strings or extend datetimes/intervals if necessary
*/
    switch (type)
    {
      case FGWBYTETYPE:
	sqlvar->sqltype=CSTRINGTYPE;
	sqlvar->sqldata="";
	break;
      default:
	sqlvar->sqltype=type;
	sqlvar->sqldata=value;
	*sqlvar->sqlind=(size<0)? -1: 0;
    }
    if (st_p->sqlda_ptr && st_p->sqlda_ptr->sqld==st_p->sqlda_in.sqld &&
	st->curstate==ST_EXECUTABLE)
    {
	ifx_loc_t *blob;
	sqlva *s;

/*
** see if we have blobs, we'll build a nice ifx_loc_t for them
*/
	s=st_p->sqlda_ptr->sqlvar+entry;
	switch(s->sqltype)
	{
	  case SQLTEXT:
	    if (!(blob=(ifx_loc_t *) malloc(sizeof(ifx_loc_t))))
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
		if (!(blob=(ifx_loc_t *) malloc(sizeof(ifx_loc_t))))
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
DLLDECL void sqd_immediate(st)
fgw_stmttype *st;
{
EXEC SQL BEGIN DECLARE SECTION;
    struct fgw_ifxstmt *st_p;
    struct sqlda *sqlda_ptr;	/* esql/c not so flexible with sqlda's */
EXEC SQL END DECLARE SECTION;

    if (!st)
	return;
    else if (!(st_p=(fgw_ifxstmttype *) st->sqlstmt))
	st->ca->sqlcode=RC_INSID;
    else if (st->curstate!=ST_EXECUTABLE)
	st->ca->sqlcode=RC_NEXEC;
    sqlda_ptr=&(st_p->sqlda_in);	/* ditto */
    switch (st->options & SO_INSCURS)
    {
      case SO_INSCURS:
	EXEC SQL PUT :st_p->curname USING DESCRIPTOR sqlda_ptr;
	if ((st->ca->sqlcode=sqlca.sqlcode)!=-413)
	    break;
/*
** just in case we opened an insert cursor in error (DESCRIBE could do a
** better job, in all truth), we close it and continue as if nothing has
** happened
*/
	st->options&=~SO_INSCURS;
	EXEC SQL CLOSE :st_p->curname;
      default:
	EXEC SQL EXECUTE :st_p->stmtname USING DESCRIPTOR sqlda_ptr;
	st->ca->sqlcode=sqlca.sqlcode;
    }
    st->ca->sqlerrd1=sqlca.sqlerrd[1];
    st->ca->sqlerrd2=sqlca.sqlerrd[2];
}

/*
**  loads next row
*/
DLLDECL void sqd_nextrow(st)
fgw_stmttype *st;
{
EXEC SQL BEGIN DECLARE SECTION;
    struct fgw_ifxstmt *st_p;
EXEC SQL END DECLARE SECTION;
    struct sqlda *sqlda_ptr;	/* esql/c not so flexible with sqlda's */

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
    if (st->curstate==ST_DECLARED)
    {
	sqlda_ptr=&(st_p->sqlda_in);	/* esql/c not so flexible with sqlda's */
	EXEC SQL OPEN :st_p->curname USING DESCRIPTOR sqlda_ptr;
	if (sqlca.sqlcode!=0)
	    goto badexit;
	st->curstate=ST_NEWLYOPENED;
    }
    sqlda_ptr=st_p->sqlda_ptr;	/* esql/c not so flexible with sqlda's */
    EXEC SQL FETCH :st_p->curname USING DESCRIPTOR sqlda_ptr;
    if (!sqlca.sqlcode)
	st->curstate=ST_OPENED;
badexit:
    st->ca->sqlcode=sqlca.sqlcode;
    st->ca->sqlerrd1=sqlca.sqlerrd[1];
    st->ca->sqlerrd2=sqlca.sqlerrd[2];
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
    ifx_loc_t *b;

    if (field>=((fgw_ifxstmttype *) st_p->sqlstmt)->sqlda_ptr->sqld)
	return NULL;
    col=((fgw_ifxstmttype *) st_p->sqlstmt)->sqlda_ptr->sqlvar+field;
    e=((fgw_ifxstmttype *) st_p->sqlstmt)->exprlist+field;
    switch (col->sqltype)
    {
      case SQLTEXT:
	b=(ifx_loc_t *) col->sqldata;
	if (!b->loc_buffer || !b->loc_size || b->loc_indicator)
	{
	    e->length=-1;
	    e->val.string="";
	}
	else
	{
	    if (b->loc_bufsize==b->loc_size)
	    {
		if (c=(char *) realloc(b->loc_buffer, b->loc_bufsize+1))
		{
		    b->loc_buffer=c;
		    b->loc_bufsize++;
		}
		else
		    b->loc_size--;
	    }
	    *(b->loc_buffer+b->loc_size)='\0';
	    e->val.string=b->loc_buffer;
	    e->length=b->loc_size;
	}
	break;
      case SQLBYTES:
	b=(ifx_loc_t *) col->sqldata;
	e->val.string=((ifx_loc_t *) col->sqldata)->loc_buffer;
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
DLLDECL void sqd_errmsg(err, buf, len)
int err;
char *buf;
int len;
{
    char b1[256], b2[256];
    int l;
    
    rgetlmsg(err, (char *) &b1, (len<sizeof(b1))? len: sizeof(b1)-1, &l);
    sprintf(b2, b1, sqlca.sqlerrm);
    sprintf(buf, "%i ", err);
    strncat(buf, b2, len-strlen(buf));
}

/*
** sends a break to the engine
*/
DLLDECL void sqd_break()
{
    sqlbreak();
}

/*
** returns a description of the selected columns
*/
DLLDECL void sqd_describecol(st_p, column, d)
fgw_stmttype *st_p;
int column;
exprstack_t *d;
{
    sqlva *col;
    exprstack_t *el;
    int e;

    if (column<0 ||
	column>=((fgw_ifxstmttype *) st_p->sqlstmt)->sqlda_ptr->sqld)
	memset(d, 0, sizeof(exprstack_t));
    else
    {
	col=((fgw_ifxstmttype *) st_p->sqlstmt)->sqlda_ptr->sqlvar+column;
	el=((fgw_ifxstmttype *) st_p->sqlstmt)->exprlist+column;
	d->type=el->type;
	d->length=col->sqllen;
	switch (d->type)
	{
	  case CSTRINGTYPE:
	    d->length--;
	    break;
	  case CDTIMETYPE:
	  case CINVTYPE:
	    e=TU_END(d->length);
	    if (e>=TU_F1)
		e=TU_FRAC;
	    d->length=TU_LEN(d->length)+(e-TU_START(d->length))/2;
	}
	d->val.string=col->sqlname;
    }
}
