/*
        CRXXC.c  -  Expression stack runtime execution: allocation and runner

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

#include <string.h>
#include "coslc.h"
#include "csqpc.h"
#include "crqxd.h"
#include "crqxp.h"
#include "crxoc.h"
#include "crxxd.h"

extern TGLOB parserstate_t *pstate;

#define TAPEBLOCK	64

/*
** true if pstate->pcode has runnable code
*/
int rxx_runnable()
{
     return (pstate->pcode.curbuf && pstate->pcode.parsecnt<TAPEBLOCK);
}

/*
** allocate new node
*/
instruction_t *rxx_newinstr(parserstate_t *state)
{
    instruction_t *next, *last;
    
    if (state->pcode.parsecnt>1)
	next=state->pcode.curbuf+TAPEBLOCK-((state->pcode.parsecnt)--);
    else
    {
	if (!(next=(instruction_t *) fgw_tssmalloc(&state->pcode.pcodehead,
					   TAPEBLOCK*sizeof(instruction_t))))
	    return NULL;
	if (state->pcode.curbuf)
	{
	    last=state->pcode.curbuf+TAPEBLOCK-((state->pcode.parsecnt)--);
	    memset(last, 0 , sizeof(instruction_t));
	    last->func=rxo_jump;
	    last->skip_to=next;
	}
	state->pcode.curbuf=next;
	state->pcode.parsecnt=TAPEBLOCK-1;
	if (!pstate->pcode.entry)
	    pstate->pcode.entry=next;
    }
    memset(next, 0 , sizeof(instruction_t));
    return next;
}

/*
** returns last pcode entry as a jump address
*/
instruction_t *rxx_setjump(parserstate_t *state)
{
    if (state->pcode.parsecnt>0)
	return state->pcode.curbuf+TAPEBLOCK-((state->pcode.parsecnt));
    else
	return NULL;
}

/*
** leave one empty pcode block
*/
void rxx_emptypcode(parserstate_t *state)
{
    fgw_tsstype *next;

    if (state->pcode.pcodehead)
    {
	if (next=state->pcode.pcodehead->next)	/* being naughty here.... */
	    fgw_tssdrop(&next);
	state->pcode.curbuf=(instruction_t *) &(state->pcode.pcodehead->string);
	state->pcode.pcodehead->next=NULL;
	state->pcode.parsecnt=TAPEBLOCK;
	state->pcode.entry=state->pcode.curbuf;
	fgw_tssdrop(&state->pcode.parsehead);
    }
}

/*
** frees pcode blocks
*/
void rxx_droppcode(pcode_t *pcode)
{
    if (pcode && pcode->pcodehead)
    {
	fgw_tssdrop(&pcode->parsehead);
	fgw_tssdrop(&pcode->pcodehead);
	memset(pcode, 0, sizeof(pcode_t));
    }
}

/*
** swaps two pcodes
*/
void rxx_swappcode(pcode_t *s1, pcode_t *s2)
{
    pcode_t d;

    fgw_move(&d, s1, sizeof(d));
    fgw_move(s1, s2, sizeof(d));
    fgw_move(s2, &d, sizeof(d));
}

/*
** execute pcode
**
** FIXME: here's a whole collections of hacks (pcode, stack, exphead, stmt)
*/
int rxx_execute(pcode_t *pcode, fgw_stmttype *stmt)
{
    int rc;
    fgw_stacktype *stack;
    instruction_t *last, *saveinstrp=pstate->instrp;
    fgw_tsstype *saveexphead=pstate->exphead;

    if (stmt)
    {
	stack=&stmt->stack;
	pstate->exphead=stmt->usinghead;
	pstate->runneroptions|=RT_CONSTOPT;
    }
    else
	stack=&pstate->exprlist;
    if (pcode)
    {
	pstate->instrp=pcode->entry;
	last=pcode->curbuf+TAPEBLOCK-pcode->parsecnt;
    }
    else
    {
	if (!pstate->instrp)
	     pstate->instrp=pstate->pcode.entry;
	last=pstate->pcode.curbuf+TAPEBLOCK-pstate->pcode.parsecnt;
    }
    while (pstate->instrp!=last)
    {
	if (rc=pstate->instrp->func(stack))
	    {
	    pstate->runneroptions&=~RT_CONSTOPT;
	    return rqx_rc(rc, pstate->instrp->src_id) ;
	    }
	pstate->instrp++;
    }
    if (pcode)
	pstate->instrp=saveinstrp;
    if (stmt)
    {
	stmt->usinghead=pstate->exphead;
	pstate->exphead=saveexphead;
    }
    pstate->runneroptions&=~RT_CONSTOPT;
    return 0;
}
