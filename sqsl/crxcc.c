/*
	CRXCC.c  -  Expression stack runtime execution: clone / wait

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: May 00
	Current release: Sep 16

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coslc.h"
#include "ccmnc.h"
#include "cascc.h"
#include "ctypc.h"
#include "chstc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqfc.h"
#include "crqxp.h"
#include "crxuc.h"
#include "crxcc.h"
#include "crxxp.h"

extern TGLOB parserstate_t *pstate;

#ifdef SIMULATED_FORK
/*
** allocates and copies one Temporary Storage Space entry from parent
*/
static int rxc_tsscopy(fgw_tsstype **heap, exprstack_t *expr)
{
    fgw_tsstype *e, b;
    char *nv;

    if (expr->type==CSTRINGTYPE || expr->type==CDECIMALTYPE ||
	expr->type==CDTIMETYPE || expr->type==CINVTYPE)
    {
	if (expr->length<0)
	    expr->val.string=NULL;
	else
	{
	    e=FGW_PTR2TSS(&expr->val.string);
	    if (!fgw_forkcopy2((char *) e, (char *) &b, sizeof(fgw_tsstype)))
		return -1;
	    if (!(nv=fgw_tssmalloc(heap, b.size)))
		return -1;
	    if (!fgw_forkcopy2((char *) &expr->val.string, nv, b.size))
		return -1;
	    expr->val.string=nv;
	}
    }
    return 0;
}

typedef struct
{
    instruction_t * sourcestart;
    instruction_t * sourceend;
    instruction_t * targetstart;
} instrlist_t;

/*
** convert instruction addresses from parent to child
*/
static instruction_t  *rxc_searchinstr(instruction_t *addr,
				       instrlist_t *list, int count)
{
    instrlist_t *p=list;

    while (count--)
    {
	if (addr>=p->sourcestart && addr<=p->sourceend)
	    return p->targetstart+((int) (addr-p->sourcestart));
	p--;
    }
    return NULL;
}

/*
** allocates, copies and fixes up addresses in one pcode set from parent
*/
static int rxc_pcodecopy(pcode_t *pcode, instruction_t **instr)
{
    instrlist_t *conv;
    int in_count=0, in_max=10;
    fgw_tsstype *pp, b;

/*
** copy pcode blocks keeping track of old and new address ranges
*/
    if (!(conv=(instrlist_t *) malloc(in_max*sizeof(instrlist_t))))
	return -1;
    pp=pcode->pcodehead;
    pcode->pcodehead=NULL;
    while (pp)
    {
	instruction_t *e;

	if (!fgw_forkcopy2((char *) pp, (char *) &b, sizeof(fgw_tsstype)))
	    return -1;
    	if (!(e=(instruction_t *) fgw_tssmalloc(&pcode->pcodehead, b.size)))
	    return -1;
    	if (!fgw_forkcopy2((char *) &pp->string, (char *) e, b.size))
	    return -1;
        conv[in_count].sourcestart=(instruction_t *) &pp->string;
	conv[in_count].sourceend=(instruction_t *) &pp->string+b.size-1;
	conv[in_count].targetstart=e;
	if (pcode->curbuf==conv[in_count].sourcestart)
	    pcode->curbuf=e;
	if (++in_count==in_max)
	{
	    in_max+=10;
	    if (!(conv=(instrlist_t *) realloc(conv,
						in_max*sizeof(instrlist_t))))
		return -1;
	}
	pp=b.next;
    }
/*
** fixup current instruction pointer
*/
    if (instr && !(*instr=rxc_searchinstr(*instr, conv, in_count)))
	return -1;
/*
** scan instructions and copy constants / fixup jump pointers
*/
    pcode->parsehead=NULL;
    pp=pcode->pcodehead;
    while (pp)
    {
	int c;
	instruction_t *i=(instruction_t *) &(pp->string);

	c=pp->size/sizeof(instruction_t);
	if (i==pcode->curbuf)
	    c-=pcode->parsecnt;
	while (c--)
	{
	    if (rxc_tsscopy(&pcode->parsehead, &i->constant))
		return -1;
	    if (i->skip_to &&
		!(i->skip_to=rxc_searchinstr(i->skip_to, conv, in_count)))
		return -1;
	    i++;
	}
	pp=pp->next;
    }
    free(conv);
    return 0;
}

/*
** entry point for child processes on non fork capable platforms
**
** NOTE - for every new dynamic structure added to parserstate_t,
** please add code to copy it from the parent here!
*/
static int fgw_forkentry(parserstate_t *source)
{
    fgw_loc_t txtvar;
    int lastcode;
    controlstack_t *c;
    exprstack_t *e;
    int i;
    int is_clone;

/*
** start off by copying parser state, queries, stacks, heaps, pcode, vars
*/
    if (!(pstate=(parserstate_t *) fgw_forkcopy((char *) source, sizeof(parserstate_t))))
	return -1;

/* OK not to copy errorinfo - it points to a static address anyway */
    if (!(pstate->i_query=fgw_forkcopy(pstate->i_query, pstate->i_size)))
	return -1;
    if (!(pstate->o_query=fgw_forkcopy(pstate->o_query, pstate->o_size)))
	return -1;
    if (pstate->control.count)
    {
	if (!(pstate->control.stack=fgw_forkcopy(pstate->control.stack,
			pstate->control.len*pstate->control.size)))
	    return -1;
    }
    else
	fgw_stackinit(&pstate->control, sizeof(controlstack_t));
    if (pstate->exprlist.count)
    {
	if (!(pstate->exprlist.stack=fgw_forkcopy(pstate->exprlist.stack,
			pstate->exprlist.len*pstate->exprlist.size)))
	    return -1;
    }
    else
	fgw_stackinit(&pstate->exprlist, sizeof(exprstack_t));
    if (!(pstate->vars=(fgw_loc_t *) fgw_forkcopy((char *) pstate->vars, sizeof(fgw_loc_t))))
	return -1;
    if (!(pstate->vars->loc_buffer=fgw_forkcopy(pstate->vars->loc_buffer,
						pstate->vars->loc_size)))
	return -1;
    pstate->lexhead=NULL;
    pstate->exphead=NULL;
    if (i=pstate->exprlist.count)
        e=(exprstack_t *) fgw_stackpeek(&pstate->exprlist);
    while (i--)
    {
	if (rxc_tsscopy(&pstate->exphead, e))
	    return -1;
	e->flags&=~EF_NOTONTSS;
        e--;
    }
    if (i=pstate->control.count)
        c=(controlstack_t *) fgw_stackpeek(&pstate->control);
    while (i--)
    {
/*
** we only need to handle FORs, as everything else is either invalid in the
** context of the child, or has been handled already
*/
	if (c->stmt_type==T_FOR)
        {
	    if (rxc_pcodecopy(&c->destvar, NULL))
		return -1;
	}
	else
	    memset(&c->destvar, 0, sizeof(pcode_t));
	memset(&c->clone, 0, sizeof(pcode_t));
/*
** invalidate streams and statements in control stack
** we haven't copied and they don't pertain to us anyway, so if nothing else
** these pointers, left untached, would be dangling references...
*/
        c->curr_fd=FD_IGNORE;
        c->stmt=NULL;
        c--;
    }
    if (is_clone=!(pstate->runneroptions & RT_CANFORK))
    {
        c=(controlstack_t *) fgw_stackpeek(&pstate->control);
	c->flags|=CS_ISCHILD;
	c->state=S_ACTIVE;
	pstate->phase1=c->phase1;
	memset(&pstate->pcode, 0, sizeof(pcode_t));
	pstate->instrp=NULL;
    }
    else if (rxc_pcodecopy(&pstate->pcode, &pstate->instrp))
	return -1;
    fgw_forkrelease();
/*
** do all the required initialization
*/
    if (lastcode=rqx_init(pstate, 0))
	return -1;
    sql_parserinit(pstate);
/*
** set up the child output
*/
    pstate->signature=1;	/* FIXME restore from private area */
    pstate->states|=PS_ISCHILD;
    memset(&txtvar, 0, sizeof(txtvar));
    txtvar.loc_loctype=LOCMEMORY;
    txtvar.loc_indicator=-1;
    fgw_locatetext(&txtvar);
    pstate->txtvar=&txtvar;
    pstate->def_fd=FD_IGNORE;
    pstate->curr_fd=pstate->def_fd;
    rqf_openfile(pstate->txtvar, pstate->def_fd);
/*
** FIXME if we switch CLONE loops to have a single pcode embedding fork_sqp()
** we could simplify some of this - not worth the effort, probably
*/
    if (is_clone)
    {
	int pid=0;
	hst_t pv;

	e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
	pv=e->val.identifier;
	fgw_hstidxadd(pstate->vars, pv, CINTTYPE, 0, (char *) &pid);
    }
/*
** complete fork -> push on stack, skip to next instruction, continue execution
*/
    else
    {
	exprstack_t v;

	v.type=CINTTYPE;
	v.length=0;
	v.val.intvalue=0;
	rxu_pushresult(&pstate->exprlist, &v);
	pstate->instrp++;
	pstate->runneroptions&=~RT_CANFORK;
	if (rxx_execute(NULL, NULL))
	    return -1;
    }
/*
** and finally, skip to next statement and continue script
*/
    sql_postcmd(pstate);
    lastcode=sql_execcmd(pstate);
    rqx_zapstmt(pstate->signature);
    rqx_zapstmt(pstate->signature | SS_INTERNAL);
    fgw_zapfd(pstate->signature);
    fgw_zapfd(pstate->signature | SS_INTERNAL);
/*
** there's a whole bunch of other cleanup we should be doing, but it's only
** memory, and we are bailing out, so who cares?
*/
    exit(pstate->returncode & 0x7f);

/* just to keep the compiler happy */
    return 0;
}
#endif

/*
** fork
*/
int rxc_dofork()
{
    int pid;

    pid=(int) fgw_fork(pstate, fgw_forkentry);
/*
** parent branch
*/
    if (pid)
	pstate->oserr.sqlcode=errno;
/*
** child branch
*/
    else
    {
	controlstack_t *c;
	int i;
/*
** children do not have a default output and don't mess with the parent
** current output
*/
	pstate->def_fd=FD_IGNORE;
	pstate->curr_fd=pstate->def_fd;
	rqf_openfile(pstate->txtvar, pstate->def_fd);
	rqf_flush(pstate->txtvar, 1);
	fgw_zapfd(SS_ALLCON);
	rqx_zapstmt(SS_ALL);
	pstate->states|=PS_ISCHILD;
/*
** invalidate streams and statements in control stack
** we have zapped them already, so if nothing else these pointers, left
** untached, would be dangling references...
*/
	if (i=pstate->control.count)
	    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
	while (i--)
	{
	    c->curr_fd=FD_IGNORE;
	    c->stmt=NULL;
	    c--;
	}
    }
    return pid;
}

/*
** waitpid
*/
int rxc_dowaitpid(int p)
{
    int r;

/* FIXME handle NULLs */
    r=fgw_waitpid(p, &pstate->childstatus);
    if (r==-1)
        pstate->oserr.sqlcode=errno;
    else
        pstate->childstatus>>=8;
    return r;
}
