/*
	CSQIC.c  -  SQSL interpreter 4gl interface

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
	Current release: Aug 21

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

#include <string.h>

#include "ccmpc.h"
#include "ccmnc.h"
#include "cexpc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "ctypc.h"
#include "crqxp.h"

#ifndef FGW_DEFSOURCE
#define FGW_DEFSOURCE "ifmx"
#endif

static execinfo_t execinfo;

/*
** sql_execute entry point
*/
int FGLGLOB(sql_execute)(int nargs)
{
    char *i_query;
    int def_fd, flags, width;
    fgw_loc_t vars;
    LOC_T(txtvar)

    POPLOCATOR(txtvar)
    popint(&width);
    popint(&flags);
    popint(&def_fd);
    memset(&vars, 0, sizeof(fgw_loc_t));
    if (!(i_query=fgw_popquote()))
	retint(RC_NOMEM);
    else
    {
	int r;

	r=fgw_sqlexec(i_query, strlen(i_query)+1, def_fd, flags&~K_preserve, 
			   width, &vars, txtvar, &execinfo);

	/* traditionally, the error code is null if the script is 
	   succesfull and has produced output */
	if (r==0 && execinfo.verbose)
	    rsetnull(CINTTYPE, (char *) &r);
	retint(r);
	free(i_query);
	RESTORELOCATOR(txtvar)
    }
    return(1);
}

/*
** sql_execute2 entry point
*/
int FGLGLOB(sql_execute2)(int nargs)
{
    LOC_T(i_query)
    int def_fd, flags, width;
    LOC_T(vars)
    LOC_T(txtvar)
    char *query;
    int qsize;
    int r;

    POPLOCATOR(txtvar)
    POPLOCATOR(vars)
    popint(&width);
    popint(&flags);
    popint(&def_fd);
    POPLOCATOR(i_query)
    if (i_query->loc_type!=SQLTEXT || i_query->loc_loctype!=LOCMEMORY)
    {
	retint(FGW_EINVAL);
	return(1);
    }
    else if (!i_query->loc_buffer || !i_query->loc_size ||
	     i_query->loc_indicator)
    {
	query=" ";
	qsize=1;
    }
    else
    {
	query=i_query->loc_buffer;
	qsize=i_query->loc_size;
    }

    r=fgw_sqlexec(query, qsize, def_fd, flags, width, vars, txtvar, &execinfo);

    /* traditionally, the error code is null if the script is 
       succesfull and has produced output */
    if (r==0 && execinfo.verbose)
	rsetnull(CINTTYPE, (char *) &r);
    retint(r);
    RESTORELOCATOR(txtvar)
    RESTORELOCATOR(vars)
    return(1);
}

/*
** parses an inline expression, returns index of first invalid char
*/
int FGLGLOB(sql_evalexpr)(int nargs)
{
    parserstate_t state;                /* passed in bulk to yyparse */
    char *buf, *r, cbuf[30];
    int s, e, l;
    exprstack_t *exprlist;
    LOC_T(vars)
 
    memset(&state, 0, sizeof(parserstate_t));
    state.errorinfo=&execinfo.errorinfo;
    POPLOCATOR(vars)
    state.vars=vars;
    popint(&e);
    popint(&s);
    buf=fgw_popquote();
    l=strlen(buf);
/*
** only parse if we have a proper string, start & end
*/
    if (buf && s>0 && s<=l && s<=e)
    {
/*
** adjust string start & zero terminate end
*/
	state.o_query=buf+s-1;
	if (e>l)
	    e=l;
	*(buf+e)='\0';
	state.phase1.output_buffer_len=s-e+1;
	state.parseroptions=EXPRONLY;
	fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
/*
** do the parsing & pop the result
*/
	if (status=sql_parser(&state))
	    retquote("");
	else
	{
	    exprlist=(exprstack_t *) fgw_stackpop(&state.exprlist);
	    switch (exprlist->type)
	    {
#ifndef A4GL
	      case CDTIMETYPE:
		retdtime(exprlist->val.datetime);
		break;
	      case CINVTYPE:
		retinv(exprlist->val.interval);
		break;
	      case CDECIMALTYPE:
	      case CMONEYTYPE:
		retdec(exprlist->val.decimal);
		break;
#endif
	      case CDOUBLETYPE:
		retdub(&exprlist->val.real);
		break;
	      case CINTTYPE:
		retint(exprlist->val.intvalue);
		break;
	      case CDATETYPE:
		retdate(exprlist->val.intvalue);
		break;
	      case FGWBYTETYPE:
		retquote("");
		break;
	      default:
#ifdef A4GL
		retquote(rxu_tostring(exprlist, (char *) &cbuf, NULL));
#else
		retquote(exprlist->val.string);
#endif
	    }
/*
** work out next character
*/
	    s=state.ibufp+1;
	}
	free(buf);
    }
    else
	retquote("");
    retint(s);
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    RESTORELOCATOR(vars)
    return(2);
}

/*
** returns variable associated with first name, continuation from char buffer
*/
int FGLGLOB(sql_getvar)(int nargs)
{
    parserstate_t state;                /* passed in bulk to yyparse */
    char *buf, *r;
    int s, e, l;
    exprstack_t *exprlist;
    LOC_T(vars)

    memset(&state, 0, sizeof(parserstate_t));
    state.errorinfo=&execinfo.errorinfo;
    POPLOCATOR(vars)
    state.vars=vars;
    popint(&e);
    popint(&s);
    buf=fgw_popquote();
    l=strlen(buf);
/*
** only parse if we have a proper string, start & end
*/
    if (buf && s>0 && s<=l && s<=e)
    {
	char c;
/*
** adjust string start & zero terminate end
*/
	state.o_query=buf+s-1;
	if (e>l)
	    e=l;
	*(buf+e)='\0';
	state.phase1.output_buffer_len=s-e+1;
	state.parseroptions=VARONLY;
	fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
/*
** do the parsing & pop the result
*/
	if (status=sql_parser(&state))
	    retint(0);
	else
	{
	    exprlist=(exprstack_t *) fgw_stackpop(&state.exprlist);
	    retint(rxu_toint(exprlist, NULL));
/*
** work out next character
*/
	    s=state.ibufp+1;
	}
	free(buf);
    }
    else
	retint(0);
    retint(s);
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    sql_parserfree(&state);
    RESTORELOCATOR(vars)
    return(2);
}

/*
** assigns an inline expression to a variable,
** returns index of first invalid char
*/
int FGLGLOB(sql_evalassign)(int nargs)
{
    parserstate_t state;                /* passed in bulk to yyparse */
    char *buf, *r, cbuf[30];
    int s, e, l;
    exprstack_t *exprlist;
    LOC_T(vars)
 
    memset(&state, 0, sizeof(parserstate_t));
    state.errorinfo=&execinfo.errorinfo;
    POPLOCATOR(vars)
    state.vars=vars;
    popint(&e);
    popint(&s);
    buf=fgw_popquote();
    l=strlen(buf);
/*
** only parse if we have a proper string, start & end
*/
    if (buf && s>0 && s<=l && s<=e)
    {
/*
** adjust string start & zero terminate end
*/
	state.o_query=buf+s-1;
	if (e>l)
	    e=l;
	*(buf+e)='\0';
	state.phase1.output_buffer_len=s-e+1;
	state.parseroptions=VASONLY;
	fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
/*
** do the parsing & pop the result
*/
	if (status=sql_parser(&state))
	    retquote("");
	else
	{
/*
** work out next character
*/
	    s=state.ibufp+1;
	}
	free(buf);
    }
    else
	status=-1;
    retint(s);
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    RESTORELOCATOR(vars)
    return(1);
}

/*
** frees script resources
*/
int FGLGLOB(sql_release)(int nargs)
{
    LOC_T(vars)

    POPLOCATOR(vars)
    fgw_sqlrelease(vars);
    RESTORELOCATOR(vars)
    return(0);
}

/*
** returns appropriate error message string
*/
int FGLGLOB(sql_message)(int nargs)
{
    int e;
    char b[256];

    popint(&e);
    fgw_errmsg(e, &execinfo.errorinfo, (char *) &b, 256);
    retquote(b);
    return(1);
}

/*
** set default source /default connection (rather than current)
*/
int FGLGLOB(sql_sourceinit)(int nargs)
{
    parserstate_t state;

    memset(&state, 0, sizeof(parserstate_t));
    rqx_init(&state, 1);
    rqx_setconnection(&state, NULL, FGW_DEFSOURCE);
    return 0;
}

/*
** instantiates dynamic sql query
*/
int FGLGLOB(sql_open)(int nargs)
{
    char *buf;
    fgw_stmttype *stmt;
    int s;

    SETNULLINT((char *) &s)
    if (!(buf=fgw_popquote()))
    {
	status=RC_NOMEM;
	goto badexit;
    }
    if (!(stmt=(fgw_stmttype *) rqx_newstatement(NULL)))
    {
	status=RC_NOMEM;
	goto badexit;
    }
    rqx_setstmtcon(stmt, NULL, NULL, NULL);

    rqx_prepare(stmt, buf);
    if (status=stmt->ca->sqlcode)
	rqx_freestatement(stmt);
    else
	s=stmt->st_entry.int_id;
badexit:
    retint(s);
    return(1);
}

/*
** gets next row
*/
int FGLGLOB(sql_fetch)(int nargs)
{
    int st_id, r;
    fgw_stmttype *stmt;

    popint(&st_id);
    if (stmt=rqx_findstmt(st_id, 0))
    {
	if (stmt->curstate==ST_EXECUTABLE)
	    retint((status=RC_NDECL));
	else
	{
	    rqx_nextrow(stmt);
	    retint((status=stmt->ca->sqlcode));
	}
    }
    else
	retint((status=RC_INSID));
    return(1);
}

/*
** gets n'th column
*/
int FGLGLOB(sql_field)(int nargs)
{
    int st_id, f;
    fgw_stmttype *stmt;
    exprstack_t *e;
    char cbuf[30];

    popint(&f);
    popint(&st_id);
    if (stmt=rqx_findstmt(st_id, 0))
    {
	if (stmt->curstate==ST_EXECUTABLE)
	    status=RC_NDECL;
	else if (f<1)
	    status=RC_CRNGE;
	else if (e=(exprstack_t *) rqx_nexttoken(f-1, stmt))
	{
	    switch (e->type)
	    {
#ifndef A4GL
	      case CDTIMETYPE:
	      case SQLDTIME:
		retdtime(e->val.datetime);
		break;
	      case CINVTYPE:
	      case SQLINTERVAL:
		retinv(e->val.interval);
		break;
	      case CDECIMALTYPE:
	      case SQLDECIMAL:
	      case CMONEYTYPE:
	      case SQLMONEY:
		retdec(e->val.decimal);
		break;
#endif
	      case CDOUBLETYPE:
		retdub(&e->val.real);
		break;
	      case CINTTYPE:
		retint(e->val.intvalue);
		break;
	      case CDATETYPE:
	      case SQLDATE:
		retdate(e->val.intvalue);
		break;
	      case SQLBYTES:
/* FIXME */	retquote("");
		break;
	      default:
#ifdef A4GL
		retquote(rxu_tostring(e, (char *) &cbuf, NULL));
#else
		retquote(e->val.string);
#endif
	    }
	    return(1);
	}
	else
	    status=RC_CRNGE;
    }
    else
	status=RC_INSID;
    retquote("");
    return(1);
}

/*
** frees dynamic sql resources
*/
int FGLGLOB(sql_free)(int nargs)
{
    int st_id, r;
    fgw_stmttype *stmt;

    popint(&st_id);
    if (stmt=rqx_findstmt(st_id, 0))
	rqx_freestatement(stmt);
    else
	status=RC_INSID;
    return 0;
}

/*
** sql_explode entry point
*/
int FGLGLOB(sql_explode)(int nargs)
{
    char *i_query;
    LOC_T(vars)
    int verbose, expstate, iidx, oidx;
    int s, e, r;
    char *exp=NULL;

    popint(&iidx);
    popint(&expstate);
    popint(&verbose);
    POPLOCATOR(vars)
    i_query=fgw_popquote();
    status=fgw_sqlexplode(i_query, strlen(i_query), vars, verbose, expstate, 
			  iidx, &oidx, &exp, &s, &e, NULL, NULL);

    if (i_query)
	free(i_query);
    if (exp)
	free(exp);

    /* different 4GL implementations use different representations for SQL nulls
       and we are trying to keep nulls out of the C code, so we have to set nulls,
       if present, before reentering 4gl
     */
    if (s<0)
	SETNULLINT(&s);
    if (e<0)
	SETNULLINT(&e);
    if (oidx<0)
	SETNULLINT(&oidx);
    retint(s);
    retint(e);
    retquote(exp);
    retint(oidx);
    RESTORELOCATOR(vars)
    return 4;
}
