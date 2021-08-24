/*
	CSQPC.c  -  SQSL interpreter outer layer

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

#include "coslc.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ccmnc.h"
#include "cascc.h"
#include "chstc.h"
#include "coslc.h"
#include "crqfc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqfc.h"
#include "crxuc.h"
#include "crqxp.h"
#include "crxxp.h"

#define BUFSIZE	32768

static int signature=0;		/* FIXME: MT this should probably be protected
				   but currently its use is so limited that
				   this is fine */
FGW_GLOBALS

/*
**  preprocessor (expansion, comments, and statement breakup) statement fixups
*/
void sql_postcmd(parserstate_t *pstate)
{
/*
**  remove processed statements from ouput buffer
*/
    if (!pstate->control.count)
    {
	if (pstate->phase1.next_stmt_start>=pstate->o_size ||
	    pstate->phase1.next_stmt_start>pstate->phase1.output_buffer_len)
	{
	    pstate->phase1.output_buffer_len=0;
	    pstate->phase1.next_stmt_start=0;
	}
	else if (pstate->o_query)
	{
	    pstate->phase1.output_buffer_len-= pstate->phase1.next_stmt_start;
	    fgw_move(pstate->o_query,
		   pstate->o_query+pstate->phase1.next_stmt_start, 
		   pstate->phase1.output_buffer_len);
	    pstate->phase1.next_stmt_start=0;
	}
     }
/*
**  terminate output buffer again - just in case we are looping back
*/
     if (pstate->o_query)
	*(pstate->o_query+pstate->phase1.output_buffer_len)='\0';
/*
**  skip to next statement
*/
    pstate->phase1.stmt_start=pstate->phase1.next_stmt_start;
}

/*
**  preprocessor main loop
**  (expansion, comments, and statement breakup if non pure parser)
*/
int sql_execcmd(parserstate_t *pstate)
{
    int lastcode=0;
    int innerloop;
    int copyphase1=1;
    int stmtend;

    pstate->phase1.expcount=0;
    do
    {
/*
**  this ought to be part of the fork() code, but doesn't do any harm here
**
**  for children, we have to cut down on verbage & disallow interactive
**  expansions and confirmations
*/
	if ((pstate->flags & PF_VERBOSE) && (pstate->states & PS_ISCHILD))
	{
	    pstate->flags&=~PF_VERBOSE;
	    pstate->expstate=ES_SILENT;
	}
	if (pstate->touch==K_dml && (pstate->states & PS_ISCHILD))
	   pstate->touch=0;
	pstate->expcount=pstate->phase1.expcount;

#ifndef PUREPARSER
/*
**  fire expansion
*/
	if (pstate->phase1.input_char<pstate->i_size)
	{
	    int r, ip, s, e, l, nlc;
	    char *txt=NULL, *tp;
	    controlstack_t *c;

	    pstate->phase1.expstart=-1;
	    r=fgw_sqlexplode(pstate->i_query,
			     pstate->i_size, pstate->vars,
			     (pstate->flags & PF_VERBOSE),
			     ((!pstate->control.count ||
				(c=(controlstack_t *)
				   fgw_stackpeek(&pstate->control),
				     c->state>S_DISABLED))?
				   (pstate->expstate): ES_DISABLED),
			     pstate->phase1.input_char+1, &ip, &txt,
			     &s, &e, &nlc, pstate->execinfo);
/*
** error in expansion? abort
*/
	    if (r!=0)
	    {
		lastcode=r;
		pstate->errorinfo->line_count=pstate->phase1.line_count+nlc+1;
		if (txt)
		    free(txt);
		goto bad_exit;
	    }
/*
** resize output buffer
*/
	    if (pstate->o_size<
		(s<0? 0: e-s)+
		(txt? strlen(txt): 0)+2+nlc)
	    {
		char *newbuf;

		newbuf=realloc(pstate->o_query, pstate->o_size+BUFSIZE);
		if (!newbuf)
		{
		    lastcode=RC_NOMEM;
		    if (txt)
			free(txt);
		    goto bad_exit;
		}
		pstate->o_query=newbuf;
		pstate->o_size+=BUFSIZE;
	    }
/*
** finally, copy strings
*/
	    if (s>=0 && (l=e-s+1))
	    {
		fgw_move(pstate->o_query+pstate->phase1.output_buffer_len,
		       pstate->i_query+s-1, l);
		pstate->phase1.output_buffer_len+=l;
		*(pstate->o_query+pstate->phase1.output_buffer_len)='\0';
	    }
	    if (txt)
	    {
		pstate->phase1.expstart=pstate->phase1.output_buffer_len;
/*
** if this is the first expansion in the statement, this is where
** we are going to restart
*/
		if (pstate->expcount==pstate->phase1.expcount)
		    pstate->phase1.prev_input_char=pstate->phase1.input_char;
		l=strlen(txt);
		strcpy(pstate->o_query+pstate->phase1.output_buffer_len, txt);
		pstate->phase1.output_buffer_len+=l;
		pstate->phase1.expcount++;
		free(txt);
	    }
/*
** add new lines to the end of the expansion to keep track
** of newlines in expansion text
*/
	    if (nlc)
	    {
		tp=pstate->o_query+pstate->phase1.output_buffer_len;
		pstate->phase1.output_buffer_len+=nlc;
		while (nlc--)
		    *tp++='\n';
		*tp='\0';
	    }
	    if (ip<0)
		pstate->phase1.input_char=pstate->i_size;
	    else
		pstate->phase1.input_char=ip-1;
	}
	innerloop=pstate->phase1.output_buffer_len;
	while (innerloop)
	{
	    char *c, *c1, *r, *s;

	    if (copyphase1)
		pstate->prevphase1=pstate->phase1;
	    copyphase1=0;
	    stmtend=0;
	    pstate->phase1.next_stmt_start=pstate->phase1.stmt_start;
/*
**  identify the next statement, cleaning comments in the way
*/
	    while (pstate->phase1.next_stmt_start<pstate->phase1.output_buffer_len &&
		   !stmtend)
	    {
		if (!(c=strpbrk(pstate->o_query+pstate->phase1.next_stmt_start,
				"{-\\\"\';")))
		{
		    pstate->phase1.next_stmt_start=pstate->phase1.output_buffer_len;
		    break;
		}
		pstate->phase1.next_stmt_start=(int) (c-pstate->o_query)+1;
		c1=c+1;
/*
**  found it!
*/
		if (*c==';')
		{
		    stmtend=(int) (c-pstate->o_query);
		    break;
		}
/*
**  found something, but not enough chars to see what it is
**  will postpone till more chars available
*/
		else if (pstate->phase1.next_stmt_start>=
			 pstate->phase1.output_buffer_len)
		{
		    pstate->phase1.next_stmt_start--;
		    break;
		}
/*
** comments
*/
		else if (*c=='-' && *c1=='-')
		    s="\n";
		else if (*c=='{')
		    s="}";
/*
**  skip escaped chars
*/
		else if (*c=='\\')
		{
		    pstate->phase1.next_stmt_start++;
		    continue;
		}
/*
**  quotes
*/
		else if (*c=='\'')
		    s="\'\\";
	        else if (*c=='\"')
		    s="\"\\";
/*
** not interesting
*/
		else
		    continue;
		r=pstate->o_query+pstate->phase1.next_stmt_start;
/*
**  find the other side of the comment or quote
*/
		while (c=strpbrk(r, s))
		{
		    if (*c!='\\')
			break;
		    else if (!*(c+1))
		    {
			c=NULL;
			break;
		    }
		    else
			r=c+1;
		}
/*
**  the other side not found
*/
		if (!c)
		{
/*
**  and there is nothing else to expand -- syntax error,
**  but we'll let the parser complain
*/
		    if (pstate->phase1.input_char==pstate->i_size)
		    {
                        pstate->phase1.next_stmt_start=pstate->phase1.output_buffer_len;
		        break;
/*			lastcode=RC_PSNTX;
			goto bad_exit;
*/		    }
/*
**  well, we'll start from scratch next time around
*/
		    pstate->phase1.next_stmt_start--;
		    break;
		}
/*
** found end of comment or quote, can now resume looking for statement end
*/
		pstate->phase1.next_stmt_start=c-pstate->o_query+1;
	    }
#endif
/*
**  test for the interrupt key every so often
*/
	    if (int_flag)
	    {
		lastcode=RC_UINTR;
		goto bad_exit;
	    }
/*
**  process statement
*/
#ifndef PUREPARSER
	    if (pstate->phase1.input_char==pstate->i_size || stmtend)
	    {
#endif
		pstate->parseroptions=0;
		lastcode=sql_parser(pstate);
		switch(lastcode)
		{
		  case RC_NROWS:
		    if (pstate->flags & PF_VERBOSE)
		    	sqsl_numrows(pstate->ca.sqlerrd2, pstate->execinfo);
		  case RC_NOTFOUND:
		    lastcode=0;
		  case 0:
		    break;
		  case RC_EXIT:
		    lastcode=0;
		  default:
		    goto bad_exit;
		}
		sql_postcmd(pstate);

#ifndef PUREPARSER
		copyphase1=1;
	    }
	    innerloop=(pstate->phase1.stmt_start<
		       pstate->phase1.output_buffer_len) &&
		       (pstate->phase1.input_char==pstate->i_size || stmtend);
	}
    }
    while (pstate->phase1.input_char<pstate->i_size);
#else
	pstate->prevphase1=pstate->phase1;
    }
    while (pstate->phase1.input_char<pstate->i_size ||
	   pstate->phase1.stmt_start<pstate->phase1.output_buffer_len);
#endif

/*
** non terminated loops are an error
*/
    if (pstate->control.count)
    {
	controlstack_t *c;

	lastcode=RC_UBBLK;
	c=(controlstack_t *) fgw_stackpeek(&pstate->control);
	pstate->errorinfo->line_count=c->phase1.line_count+1;
    }
bad_exit:
    while (pstate->control.count)
    {
	controlstack_t *c;

/*
** close output streams
*/
	c=(controlstack_t *) fgw_stackpop(&pstate->control);
	if (c->curr_fd!=FD_TOBLOB && c->curr_fd!=FD_IGNORE &&
	    !(--(c->curr_fd->fd_refcount)))
	{
	    rqf_flush(pstate->txtvar, 1);
	    if (c->curr_fd->fd_entry.signature>0 &&
		!(c->curr_fd->fd_entry.signature & SS_INTERNAL))
		fgw_fdclose(c->curr_fd);
	    if (c->curr_fd==pstate->curr_fd)
		pstate->curr_fd=FD_TOBLOB;
	}
/*
** free pcode and expression string space
*/
	rxx_droppcode(&c->destvar);
	rxx_droppcode(&c->clone);
/*
** and free FOREACH prepared statements, while we are at it
*/
	if (c->stmt)
	    if (c->stmt->st_entry.signature>0)
		rqx_freestatement(c->stmt);
	    else
		c->stmt->refcnt--;
    }
    if (pstate->curr_fd!=FD_TOBLOB && pstate->curr_fd!=FD_IGNORE)
	rqf_flush(pstate->txtvar, 1);
    if (lastcode && lastcode!=RC_QUIT && strlen(pstate->errorlog))
    {
	fgw_fdesc *fd;
	char errmsg[80];

	fgw_errmsg(lastcode, pstate->errorinfo, (char *) errmsg, sizeof(errmsg));
	if (fd=fgw_fileopen((char *) &pstate->errorlog, "a"))
	{
	    fgw_fdwrite(fd, errmsg, strlen(errmsg));
	    fgw_fdwrite(fd, "\n", 1);
	    IGNORE fgw_fdclose(fd);
	}
    }
    return lastcode;
}

/*
**  SQSL engine entry point - state salvage and retrieval
*/
int fgw_sqlexec(char *i_query, int i_size, int def_fd, int flags, int width,
		fgw_loc_t *vars, fgw_loc_t *txtvar, execinfo_t *execinfo)
{
    parserstate_t state;		/* passed in bulk to yyparse */
    int lastcode=0;			/* sql error */

    int_flag=0;
    memset(&state, 0, sizeof(parserstate_t));
    memset(execinfo, 0, sizeof(execinfo_t));
    state.sighlr=fgw_sigint((void(*)(int)) rqx_sendbreak);
    state.execinfo=execinfo;
    state.errorinfo=&execinfo->errorinfo;
    state.i_query=i_query;
    state.i_size=i_size;

    state.vars=vars;

    if (lastcode=rqx_init(&state, (flags & K_connection)))
	return lastcode;
/*
** initialize variables and signature
** FIXME: improper use of K_preserve, or tampering with hash might
** cause a connection and statement memory leak
*/
    if ((flags & K_preserve) && !fgw_testhash(vars))
    {
	fgw_hstentry *e;
	int dummy;

/*
** find private node
*/
	e=fgw_hstget(vars, 0, "\377private", &dummy);
	if (e && e->type==T_HASH)
	{
	    hst_t private=e->magic;
/*
** restore signature
*/
	    e=fgw_hstget(vars, private, "signature", &dummy);
	    if (e && e->type==CINTTYPE)
	    {
		state.signature=*(int *) &(e->entry);
/*
** output format
*/

		e=fgw_hstget(vars, private, "html", &dummy);
		if (e && e->type==CINTTYPE && *(int *) &(e->entry))
		    state.flags|=PF_ISHTML;
/*
** current connection if any
*/
		e=fgw_hstget(vars, private, "connection", &dummy);
		if (e && e->type==CSTRINGTYPE)
		    rqx_setconnection(&state, &e->entry, NULL);
		else
		{
		    e=fgw_hstget(vars, private, "source", &dummy);
		    if (e && e->type==CSTRINGTYPE)
			rqx_setconnection(&state, NULL, &e->entry);
		}
/*
** stream
*/
		state.ca.sqlcode=0;
	        e=fgw_hstget(vars, private, "stream", &dummy);
		if (e && e->type==CINTTYPE)
		{
		    int signature=(*(int *) &(e->entry)) & SS_INTERNAL;
		    int fd_id=(*(int *) &(e->entry)) & (~SS_INTERNAL);
 
		    /* search for streams opened via API too */
		    state.curr_fd=fgw_findfd(fd_id, state.signature|signature);
		}
/*
** output width
*/
	        e=fgw_hstget(vars, private, "width", &dummy);
		if (e && e->type==CINTTYPE)
		    state.width=*(int *) &(e->entry);
/*
** and error log
*/
	        e=fgw_hstget(vars, private, "error log", &dummy);
		if (e && e->type==CSTRINGTYPE)
		    strcpy((char *) &state.errorlog, (char *) &(e->entry));
	    }
	    fgw_hstidxdrop(vars, private, -1);
	}
    }
    else
	rqx_setconnection(&state, NULL, NULL);
    if (!state.signature)
    {
	fgw_locatehash(vars, 32);
	state.signature=++signature;
    }

    if (flags & K_verbose)
	state.flags|=PF_VERBOSE;
    if (flags & K_html)
	state.flags|=PF_ISHTML;
    if (flags & K_errorcont)
	state.flags|=PF_ERRORCONT;
    if (flags & K_quit)
	state.flags|=PF_QUIT;
    state.expstate=(flags & K_isbatch? ES_SILENT: ES_VERBOSE);
    state.touch=flags & (K_dml+K_noconf);
    if (state.touch==K_noconf || 
	(state.touch==K_dml && state.expstate!=ES_VERBOSE))
	state.touch=0;
    if (state.width<MINWIDTH)
	state.width=width;
    state.def_fd=fgw_findfd(def_fd, 0);
    if (!state.curr_fd)
	state.curr_fd=state.def_fd;
    state.txtvar=txtvar;
    fgw_locatetext(state.txtvar);
    rqf_openfile(state.txtvar, state.curr_fd);
    fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
    fgw_stackinit(&state.control, sizeof(controlstack_t));
    sql_parserinit(&state);
    lastcode=sql_execcmd(&state);
    if (state.flags & PS_DONEDISPLAY)
	execinfo->verbose=1;
/*
** save stream, statement and connection info, if required
*/
    if ((flags & K_preserve) && !(state.states & PS_ISCHILD))
    {
	hst_t private;
	int dummy = state.flags & PF_ISHTML;

	private=fgw_hstaddnode(vars, 0, "\377private", 8);
	fgw_hstadd(vars, private, "signature", CINTTYPE, 0,
		   (char *) &state.signature);
	if (state.curr_fd!=state.def_fd &&
	    state.curr_fd!=FD_IGNORE &&
	    state.curr_fd!=FD_TOBLOB)
	{
	    int fd_id=state.curr_fd->fd_entry.int_id;

	    /* preserve streams opened via stream API too*/
	    if (state.curr_fd->fd_entry.signature & SS_INTERNAL)
		fd_id |= SS_INTERNAL;
	    fgw_hstadd(vars, private, "stream", CINTTYPE, 0, (char *) &fd_id);
	}
	fgw_hstadd(vars, private, "html", CINTTYPE, 0, (char *) &dummy);
	fgw_hstadd(vars, private, "width", CINTTYPE, 0, (char *) &state.width);
	if (rqx_getconnection())
	    fgw_hstadd(vars, private, "connection", CSTRINGTYPE, 0,
		       (char *) rqx_getconnection());
	if (rqx_getsource())
	    fgw_hstadd(vars, private, "source", CSTRINGTYPE, 0,
		       (char *) rqx_getsource());
	if (strlen(state.errorlog))
	    fgw_hstadd(vars, private, "error log", CSTRINGTYPE, 0,
		       (char *) &state.errorlog);
    }
/*
** or free them now
*/
    else
    {
	rqx_zapstmt(state.signature);
	rqx_zapstmt(state.signature | SS_INTERNAL);
	fgw_zapfd(state.signature);
	fgw_zapfd(state.signature | SS_INTERNAL);
    }
    fgw_stackfree(&state.control, NULL);
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    sql_parserfree(&state);
    if (state.o_query)
	free(state.o_query);
    if (state.states & PS_ISCHILD)
	exit(state.returncode & 0x7f);
    state.sighlr=fgw_sigint(state.sighlr);

    return lastcode;
}

/*
** evaluate assignment expression
*/
int fgw_evalassign(char *instr, fgw_loc_t *vars)
{
    parserstate_t state;		/* passed in bulk to yyparse */
    int rc;

    memset(&state, 0, sizeof(parserstate_t));
    state.vars=vars;
    state.o_query=instr;
    state.phase1.output_buffer_len=strlen(instr);
    state.parseroptions=VASONLY;
    fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
    sql_parserinit(&state);
    rc=sql_parser(&state);
    if (strlen(instr)!=state.ibufp)
	rc=RC_PSNTX;
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    sql_parserfree(&state);
    return rc;
}

/*
** release script resources
*/
void fgw_sqlrelease(fgw_loc_t *vars)
{
    if (!fgw_testhash(vars))
    {
	fgw_hstentry *e;
	int dummy;

/*
** find private node and signature
*/
	e=fgw_hstget(vars, 0, "\377private", &dummy);
	if (e && e->type==T_HASH)
	{
	    e=fgw_hstget(vars, e->magic, "signature", &dummy);
	    if (e && e->type==CINTTYPE)
	    {
/*
** free statement and stream resources (all streams will have been 
** flushed on exit from sql_sqlexec()
*/
		rqx_zapstmt(*(int *) &(e->entry));
		rqx_zapstmt(*(int *) &(e->entry) | SS_INTERNAL);
		fgw_zapfd(*(int *) &(e->entry));
		fgw_zapfd(*(int *) &(e->entry) | SS_INTERNAL);
	    }
	}
    }
}

/*
** returns error message string from source
*/
void fgw_errmsg(int e, errorinfo_t *ei, char *b, int l)
{
    int s, len;
    char *p;
    char m[256];

    if (e==RC_NOTFOUND)
        e=RC_NROWS;             /* FIXME rgetlmsg bug with SQLNOTFOUND */
    s=SQ_TOSRC(e);
    e=SQ_TOSQLCODE(e);
    if (SQ_ISAPI(s))
	rxu_getmessage(e, b, l);
    else if (SQ_ISPARSER(s))
    {
        sqsl_getmessage(e, b, l);
        if (ei->extra[0])
        {
            strncat(b, " ", l-strlen(b));
            strncat(b, ei->extra, l-strlen(b));
        }
    }

    /* for those sources that might return error strings with the result packet */
    else if (ei->extra[0])
	rqx_errtxt(s, e, ei->extra, b, l);
    else
	rqx_errmsg(s, e, b, l);
/*
** remove CR's, LF's, etc
*/
    p=(b)+strlen(b)-1;
    while (p>b && *p<' ')
        *p--='\0';
    if (ei->line_count)
    {
        strncat(b, ", line ", l-strlen(b));
        sprintf((char *) &m, "%d", ei->line_count);
        strncat(b, m, l-strlen(b));
    }
    if (strlen(ei->near))
    {
        strncat(b, ", near ", l-strlen(b));
        strncat(b, ei->near, l-strlen(b));
    }
}
