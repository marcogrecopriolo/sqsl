/*
	CRSXC.c  -  SQSL interpreter script execution

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

#include "stdio.h"
#include "ccmnc.h"
#include "cfnmc.h"
#include "coslc.h"
#include "cldpc.h"
#include "crxcc.h"
#include "crxvc.h"
#include "csqhc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqfc.h"
#include "crqxp.h"
#include "crsxc.h"
#include "crxxp.h"

extern TGLOB parserstate_t *pstate;

static int rsx_setformat(fgw_stmttype *st_p, pcode_t *fmt);
static int rsx_primeformat(fgw_stmttype *st_p);
static int rsx_conwhenever();
static int rsx_whenever();
static int rsx_doformat();
static void rsx_closeoutput();
static void rsx_dooutput(fgw_fdesc *fd);
static int rsx_openstream(fgw_stmttype *st, exprstack_t *e);
static int rsx_doconnect(int isdefault, char *opts);

/*
** This file includes all script execution points.
** Hopefully the only strategy needed if we decide to linearize this is to
** handle statement registration and string constants in the parsehead
** (they are all tagged with the NOTONTSS flag, although this covers
** other cases too).
** Linearization will require more consistency checks we do right now.
*/

/*
** Statement blocks
** execute an SQL statement
*/
int rsx_sqlrun(char *stmt)
{
    int res=0;

    if (rxx_runnable() && (status=rxx_execute(NULL, NULL)))
	return -1;

    rqx_run(stmt, pstate->ssp->stmt, pstate->touch);
    status=pstate->ssp->stmt->ca->sqlcode;
    rqx_freestatement(pstate->ssp->stmt);
    pstate->ssp->stmt=NULL;
    if (status!=RC_NROWS)
        if (pstate->flags & PF_ERRORCONT)
            status=0;
        else
	    res=-1;
    return res;
}

/*
** control statements
** break
*/
void rsx_breaks(int state)
{
    controlstack_t *c;
    int i;

    i=pstate->control.count;
    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
    while (i && c->stmt_type==T_IF)
    {
	i--;
	c->state=S_DISABLED;
	c--;
    }
    if (!i)
	status=RC_PSNTX;
    else
	c->state=state;
}

/* TODO most openers have to allocate control stack if implementing pcode */
/*
** for opener
** expression list in pstate->pcode, destination in c->destvar
*/
void rsx_for()
{
    controlstack_t *c;
    exprstack_t *e, *v;
    int i;

    status=rxx_execute(NULL, NULL);
    if (status)
	return;
    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
    status=rxx_execute(&c->destvar, NULL);
    if (status)
	return;
    v=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    e=rxu_arglist(&pstate->exprlist, &c->forcount);
    c->element=1;
    c->state=S_ACTIVE;
    c->phase1=pstate->phase1;
    sqv_setvar(pstate->vars, e, v);
/*
** FOR expression list strings on the parsehead (ie, constants), need to be
** moved to the expression head or all sorts of weird thing will happen at the
** next FOR loop iteration
*/
    for (i=c->forcount; i; i--, e++)
	if (e->type==CSTRINGTYPE && (e->flags & EF_NOTONTSS))
	{
	    fgw_tssdetach(&pstate->pcode.parsehead, e->val.string);
	    fgw_tssattach(&pstate->exphead, e->val.string);
	    e->flags&=~EF_NOTONTSS;
	}
}

/*
** foreach opener
*/
int rsx_foreach(pcode_t *fmt)
{
    controlstack_t c;
    exprstack_t *e, *v;
    int r=0, ret=0;

    /* TODO whenever error continue for file open failures */
    if (rxx_runnable() && (status=rxx_execute(NULL, NULL)))
	goto bad;
    memset(&c, 0, sizeof(c));
    c.stmt_type=T_FOREACH;
    c.phase1=pstate->phase1;
    c.state=S_DISABLED;
    if (pstate->ssp->stmt->curstate==ST_UNINITIALIZED)
    {
        rqx_prepare(pstate->ssp->stmt, TOKPTR(pstate->ssp->sqlstart));
        r=pstate->ssp->stmt->ca->sqlcode;
    }
    if ((status=r))
	goto bad;
    else
    {
        if (pstate->ssp->stmt->curstate!=ST_DECLARED)
	{
            status=RC_NDECL;
	    goto bad;
	}
        else
        {
	    if ((status=rsx_primeformat(pstate->ssp->stmt)) || (status=rsx_setformat(pstate->ssp->stmt, fmt)))
		goto bad;
            if (pstate->ssp->stmt->fmt_type!=FMT_NULL)
            {
                c.style=pstate->style;
                pstate->style=pstate->newstyle;
                c.curr_fd=pstate->curr_fd;
                if (pstate->curr_fd!=FD_TOBLOB &&
                    pstate->curr_fd!=FD_IGNORE)
                    pstate->curr_fd->fd_refcount++;
            }
            rqf_dorows(pstate->ssp->stmt, pstate->txtvar, 0);
	    if (status!=RC_NOTFOUND)
		ret=rsx_conwhenever();
        }
        if (status==RC_NOTFOUND)
            status=0;
	else if (status==0)
	{

	    /* cant free the statement while used in an active foreach */
            pstate->ssp->stmt->refcnt++;
	    c.state=S_ACTIVE;
	}
    }
    c.stmt=pstate->ssp->stmt;
    pstate->ssp->stmt=NULL;
    fgw_stackpush(&pstate->control, (char *) &c);
    return ret;

bad:
    pstate->ssp->stmt=NULL;
    return -1;
}

/*
** set cursor name
*/
int rsx_setcursor(fgw_stacktype *s)
{
    char *cur, cb[STRINGCONVSIZE];

    cur=rxu_tostring(&pstate->instrp->constant, (char *) &cb, NULL);
    rqx_setcursor(pstate->ssp->stmt, cur);
    return status;
}

/*
** while opener
*/
void rsx_while()
{
    controlstack_t c, *oc;
    exprstack_t *s;
    int r, n;

#ifndef SIMULATED_FORK
    pstate->runneroptions|=RT_CANFORK;
#endif
    status=rxx_execute(NULL, NULL);
#ifndef SIMULATED_FORK
    pstate->runneroptions&=~RT_CANFORK;
#endif
    if (status)
	return;
    s=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    r=rxu_toboolean(s, &n);
    rxu_freetssstack(s);
    if (pstate->control.count)
    {
        oc=(controlstack_t *) fgw_stackpeek(&pstate->control);
	if (oc->state==S_WHILE &&
	    oc->phase1.stmt_start==pstate->phase1.stmt_start)
	{
	    if (n || !r)
		if (pstate->expcount!=pstate->phase1.expcount)
		    oc->state=S_DISABLED;
		else
		{
		    pstate->phase1=oc->nextphase1;
		    rsx_endctrl();
		}
	    else
		oc->state=S_ACTIVE;
	    return;
	}
    }
    memset(&c, 0, sizeof(c));
    c.stmt_type=T_WHILE;
    if (n || !r)
	c.state=S_DISABLED;
    else
	c.state=S_ACTIVE;
    c.phase1=pstate->prevphase1;
    fgw_stackpush(&pstate->control, (char *) &c);
}

/*
** clone opener
** expression in pstate->code, targets in c->destcar, c->clone
*/
void rsx_clone(controlstack_t *c)
{
    exprstack_t *e;
    hst_t iv, pv;
    int pid, nullind;

    /* execute count */
    status=rxx_execute(NULL, NULL);
    if (status)
	return;
    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    c->forcount=rxu_toint(e, &nullind);
    if (c->forcount<1 || c->forcount>MAXFORKCOUNT ||
	nullind)
    {
	status=RC_ERANG;
	return;
    }

    /* execute counter target */
    status=rxx_execute(&c->destvar, NULL);
    if (status)
	return;
    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    iv=e->val.identifier;
    c->element=1;
    c->phase1=pstate->phase1;
    fgw_hstidxadd(pstate->vars, iv, CINTTYPE, 0,
		  (char *) &c->element);

    /* execute pid target */
    status=rxx_execute(&c->clone, NULL);
    if (status)
	return;

/* SIMULATED_FORK implementation requires that pid target variable is on
   the stack at the time of the fork */
    pid=rxc_dofork();
    if (pid==-1)
    {
	status=pstate->oserr.sqlcode;
	return;
    }
    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    pv=e->val.identifier;
    fgw_hstidxadd(pstate->vars, pv, CINTTYPE, 0,
		  (char *) &pid);
    c->flags|=(pid? 0: CS_ISCHILD);
    c->state=(pid? S_DISPAR: S_ACTIVE);
}

/*
** parent
*/
void rsx_parent(controlstack_t *c)
{
    if (c->flags & CS_ISCHILD)
	c->state=S_DISCLD;
    else
	c->state=S_ACTIVE;

}

/*
** wait opener
** expression list in pstate->pcode, target in c->destvar
*/
void rsx_wait(controlstack_t *c)
{
    exprstack_t *e, *v;
    hst_t rv;
    int pid, ind;

    status=rxx_execute(NULL, NULL);
    if (status)
	return;
    status=rxx_execute(&c->destvar, NULL);
    if (status)
	return;
    c->element=1;
    c->state=S_ACTIVE;
    c->phase1=pstate->phase1;
    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    rv=e->val.identifier;
    v=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    e=rxu_arglist(&pstate->exprlist, &c->forcount);
    if (c->forcount<=0)
    {
	status=RC_ERANG;
	return;
    }
    sqv_setvar(pstate->vars, e, v);
    pid=rxu_toint(e, &ind);
    pstate->childstatus=0;
    if (ind || pid<=0)
    {
	status=RC_ERANG;
	return;
    }
    pid=rxc_dowaitpid(pid);
    fgw_hstidxadd(pstate->vars, rv, CINTTYPE,
	  0, (char *) &pstate->childstatus);
}

/*
** if
** expression in pstate->pcode
*/
void rsx_if()
{
    controlstack_t c;
    exprstack_t *s;
    int r, n;

#ifndef SIMULATED_FORK
    pstate->runneroptions|=RT_CANFORK;
#endif
    status=rxx_execute(NULL, NULL);
#ifndef SIMULATED_FORK
    pstate->runneroptions&=~RT_CANFORK;
#endif
    if (status)
	return;
    s=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    r=rxu_toboolean(s, &n);
    rxu_freetssstack(s);
    memset(&c, 0, sizeof(c));
    c.stmt_type=T_IF;
    if (n || !r)
	c.state=S_IF;
    else
	c.state=S_ACTIVE;
    fgw_stackpush(&pstate->control, (char *) &c);
}

/*
** else if
** expression in pstate->pcode
*/
void rsx_elif()
{
    controlstack_t *c;
    exprstack_t *s;
    int r, n;

    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
/*
** already hit a positive test, nothing to do
*/
    if (c->state!=S_IF)
    {
	c->state=S_DISABLED;
	return;
    }
#ifndef SIMULATED_FORK
    pstate->runneroptions|=RT_CANFORK;
#endif
    status=rxx_execute(NULL, NULL);
#ifndef SIMULATED_FORK
    pstate->runneroptions&=~RT_CANFORK;
#endif
    if (status)
	return;
    s=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    r=rxu_toboolean(s, &n);
    rxu_freetssstack(s);
    if (n || !r)
	c->state=S_IF;
    else
	c->state=S_ACTIVE;
}

/*
** else
*/
void rsx_else(controlstack_t *c)
{
    if (c->state==S_IF)
	c->state=S_ACTIVE;
    else
	c->state=S_DISABLED;
}

/*
** end for
** value list on the stack, destvar in control stack
*/
void rsx_endfor(controlstack_t *c)
{
    exprstack_t *e, *v;

    if (c->state!=S_DISABLED &&
	c->element<c->forcount)
    {
	status=rxx_execute(&c->destvar, NULL);
	if (status)
	    return;
	v=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
	c->element++;
	e=(exprstack_t *) fgw_stackpeek(&pstate->exprlist)-
		    c->forcount+c->element;
	sqv_setvar(pstate->vars, e, v);
	c->state=S_ACTIVE;
	pstate->phase1=c->phase1;
    }
    else
    {
	if (c->forcount)
	{
	    e=(exprstack_t *) fgw_stackpeek(&pstate->exprlist)-
		c->element;
	    while (c->element--)
		rxu_freetssstack(++e);
	    fgw_stackdrop(&pstate->exprlist, c->forcount);
	}
	rsx_endctrl();
    }
}

/*
** end foreach
*/
void rsx_endforeach(controlstack_t *c)
{
/*
** Calling FORK() inside loops can lead to all sorts of undesired effects
** (including uncontrolled fork explosions!), but while there isn't any good
** reason to terminate children forked inside WHILE or FOR loops at the end
** of said loops (after all, the programmer could have coded some sound
** logic that takes care to avoid the ill effects), for FOREACH loops we have
** good reason to be pedantic - in the child environment, the statement
** associated to the loop is no longer valid. We will therefore enforce that
** the child reaches the end of the loop as a result of a BREAK, or at the
** very least there's a WHENEVER ERROR CONTINUE in effect.
*/
    if (!c->stmt && c->state!=S_DISABLED) 
    {
        if (pstate->flags & PF_ERRORCONT)
	    c->state=S_DISABLED;
	else
	{
	    status=RC_INSID;
	    return;
	}
    }
    if (c->state!=S_DISABLED)
    {
/*
** the output stream has changed, reflect this in the cursor
*/
	if (c->stmt->fmt_type!=FMT_NULL &&
	    pstate->curr_fd!=c->curr_fd)
	{
	    rsx_closeoutput();
	    rqf_openfile(pstate->txtvar, c->curr_fd);
	    pstate->curr_fd=c->curr_fd;
	}
	c->state=S_ACTIVE;
	rqx_registerstatement(pstate, c->stmt);
	rqf_dorows(c->stmt, pstate->txtvar, 0);
	switch (status)
	{
	  case RC_NOTFOUND:
	    if (c->stmt->fmt_type!=FMT_NULL)
		    pstate->style=c->style;
	    status=0;

	    /* getting out of the loop: can free now */
	    c->stmt->refcnt--;
	    c->state=S_DISABLED;
	    break;
	  case 0:
	    break;
	  default:
	    if (pstate->flags & PF_ERRORCONT)
		status=0;
	    else
		return;
	}
    }
    if (c->state==S_DISABLED)
    {
	if (c->stmt)
	    if (c->stmt->st_entry.signature & SS_INTERNAL)
		rqx_closestatement(c->stmt);
	    else
		rqx_freestatement(c->stmt);
	if (c->curr_fd!=FD_TOBLOB && c->curr_fd!=FD_IGNORE)
	    c->curr_fd->fd_refcount--;
	rsx_endctrl();
    }
    else
	pstate->phase1=c->phase1;
}

/*
** end while
** expression gets evaluated by while statement
*/
void rsx_endwhile(controlstack_t *c)
{
    if (c->state!=S_DISABLED)
    {
	c->nextphase1=pstate->phase1;
	pstate->phase1=c->phase1;
	c->state=S_WHILE;
    }
    else
	rsx_endctrl();
}

/*
** end clone
*/
int rsx_endclone(controlstack_t *c)
{
    exprstack_t *e;
    hst_t iv, pv;

    if (c->flags & CS_ISCHILD)
    {
	pstate->ca.sqlcode=status=RC_EXIT;
	rsx_endctrl();
	return 0;
    }
    else if (c->state!=S_DISABLED && c->element<c->forcount)
    {
	int pid;

	status=rxx_execute(&c->destvar, NULL);
	if (status)
	    return 1;
	e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
	iv=e->val.identifier;
	c->element++;
	fgw_hstidxadd(pstate->vars, iv, CINTTYPE,
		  0, (char *) &c->element);
	status=rxx_execute(&c->clone, NULL);
	if (status)
	    return 1;

/* SIMULATED_FORK implementation requires that pid target variable is on
   the stack at the time of the fork */
	pid=rxc_dofork();
	if (pid==-1)
	{
	    status=pstate->oserr.sqlcode;
	    return 1;
	}
	e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
	pv=e->val.identifier;
	fgw_hstidxadd(pstate->vars, pv, CINTTYPE,
		  0, (char *) &pid);
	pstate->phase1=c->phase1;
    	c->flags|=(pid? 0: CS_ISCHILD);
	c->state=(pid? S_DISPAR: S_ACTIVE);
    }
    else
	rsx_endctrl();
    return 0;
}

/*
** end wait
*/
void rsx_endwait(controlstack_t *c)
{
    exprstack_t *e, *v;
    hst_t rv;
    int ind, pid;

    if (c->state!=S_DISABLED && c->element<c->forcount)
    {
	status=rxx_execute(&c->destvar, NULL);
	if (status)
	    return;
	e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
	rv=e->val.identifier;
	v=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
	c->element++;
	e=(exprstack_t *) fgw_stackpeek(&pstate->exprlist)-
		    c->forcount+c->element;
	sqv_setvar(pstate->vars, e, v);
	pid=rxu_toint(e, &ind);
	pstate->childstatus=0;
	if (ind || pid<=0)
	{
	    status=RC_ERANG;
	    return;
	}
	pid=rxc_dowaitpid(pid);
	fgw_hstidxadd(pstate->vars, rv, CINTTYPE,
		  0, (char *) &pstate->childstatus);
	c->state=S_ACTIVE;
	pstate->phase1=c->phase1;
    }
    else
    {
	if (c->forcount)
	{
	    e=(exprstack_t *) fgw_stackpeek(&pstate->exprlist)-
			c->element;
	    while (c->element--)
		rxu_freetssstack(++e);
	    fgw_stackdrop(&pstate->exprlist, c->forcount);
	}
	rsx_endctrl();
    }
}

/*
** control statement blocks end
*/
void rsx_endctrl()
{
    controlstack_t *c;

    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
    rxx_droppcode(&c->destvar);
    rxx_droppcode(&c->clone);
    fgw_stackdrop(&pstate->control, 1);
}

/*
** Simple statements
**
** quit
*/
void rsx_quit()
{
    pstate->ca.sqlcode=status=RC_QUIT;
}

/*
** exit
** expression in pstate->pcode (if specified)
*/
int rsx_exit()
{
    if (rxx_runnable())
    {
	if (pstate->states & PS_ISCHILD)
	{
            exprstack_t *e;
            int r, n;

	    status=rxx_execute(NULL, NULL);
	    if (status)
		return 1;
	    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
	    r=rxu_toint(e, &n);
	    rxu_freetssstack(e);
	    if (n || r<0 || r>127)
	    {
		status=RC_ERANG;
		return 1;
	    }
	    pstate->returncode=r;
	}
	else
	{
	    status=RC_PSNTX;
	    return 1;
	}
    }
    pstate->ca.sqlcode=status=RC_EXIT;
    return 0;
}

/*
** output width
** expression in pstate->pcode
*/
void rsx_width()
{
    exprstack_t *s;
    int r, n;

    status=rxx_execute(NULL, NULL);
    if (status)
	return;
    s=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    r=rxu_toint(s, &n);
    rxu_freetssstack(s);
    if (r>=MINWIDTH && r<=MAXWIDTH)
        pstate->width=r;
}

/*
** error log
** expression in pstate->pcode
*/
void rsx_errorlog()
{
    exprstack_t *s;
    char *c;

    status=rxx_execute(NULL, NULL);
    if (status)
	return;
    s=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    c=rxu_tostring(s, (char *) &pstate->errorlog, NULL);

    /* no need to zero terminate */
    if (c!=(char *) &pstate->errorlog)
	strncpy((char *) &pstate->errorlog, c,
		sizeof(pstate->errorlog));
    rxu_freetssstack(s);
}

/*
** dispose
** target in pstate->pcode
*/
void rsx_dispose()
{
    exprstack_t *m;

    if ((status=rxx_execute(NULL, NULL)))
	return;

    m=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    if (m->val.identifier)
	fgw_hstidxdrop(pstate->vars, m->val.identifier, -1);
}

/*
** display
** expression list is in pstate->pcode
*/
void rsx_display(pcode_t *fmt)
{
    exprstack_t *s, *e;
    int c, i;

    if ((status=rxx_execute(NULL, NULL)))
	return;
    s=rxu_arglist(&pstate->exprlist, &c);
    if ((status=rsx_setformat(pstate->ssp->stmt, fmt)))
        return;
    rqf_display(pstate->ssp->stmt, pstate->txtvar, s, c);
    for (i=c, e=s; i; i--, e++)
        rxu_freetssstack(e);
    rqx_freestatement(pstate->ssp->stmt);
    fgw_stackdrop(&pstate->exprlist, c);
    pstate->ssp->stmt=NULL;
}

/*
** evaluate expression
** expression in pstate->pcode
*/
void rsx_evalexpr()
{
    pstate->runneroptions|=RT_CANFORK;
    status=rxx_execute(NULL, NULL);
    pstate->runneroptions&=~RT_CANFORK;
}

/*
** change state flags
*/
void rsx_flagset(int f)
{
    pstate->flags|=f;
}

void rsx_flagreset(int f)
{
    pstate->flags&=~f;
}

#if defined(I4GLVER)
#include <sqlca.h>

/*
** load
** file and delimiter in pstate->pcode
*/
int rsx_load(char * s)
{
    exprstack_t f, d;
    char *sh1, *sh2;
    char sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    status=rxx_execute(NULL, NULL);
    if (status)
	return -1;
    d=*(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    f=*(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    sh1=rxu_tostring(&f, (char *) &sbuf1, NULL);
    sh2=rxu_tostring(&d, (char *) &sbuf2, NULL);

/* FIXME: using 4gl internals */
#if I4GLVER>730
    ibm_efm_loadInFormMode(sh1, sh2, s);
#else
    _ffload(sh1, sh2, TOKPTR(pstate->tokstart));
#endif
    rxu_freetssstack(&f);
    rxu_freetssstack(&d);
    if ((status=sqlca.sqlcode))
	return -1;
    status=RC_NROWS;
    return 0;
}

/*
** unload
** file and delimiter in pstate->pcode
*/
int rsx_unload(char * s)
{
    exprstack_t f, d;
    char *sh1, *sh2;
    char sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    status=rxx_execute(NULL, NULL);
    if (status)
	return -1;
    d=*(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    f=*(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    sh1=rxu_tostring(&f, (char *) &sbuf1, NULL);
    sh2=rxu_tostring(&d, (char *) &sbuf2, NULL);

/* FIXME: using 4gl internals */
#if I4GLVER>730
    ibm_efm_unloadInFormMode(sh1, sh2, s, 0,
                             (struct sqlvar_struct *) 0);
#else
    _ffunload(sh1, sh2, s, 0, (struct sqlvar_struct *)0);
#endif
    rxu_freetssstack(&f);
    rxu_freetssstack(&d);
    if ((status=sqlca.sqlcode))
	return -1;
    status=RC_NROWS;
    return 0;
}
#endif /* I4GLVER */

/*
** append to default
*/
void rsx_appenddefault()
{
    rsx_dooutput(pstate->def_fd);
}

/*
** append to / output to
** file in pstate->pcode
*/
void rsx_outputto(char *mode)
{
    exprstack_t *s;
    char cbuf[STRINGCONVSIZE];
    fgw_fdesc *fd;

    status=rxx_execute(NULL, NULL);
    if (status)
        return;
    s=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    fd=fgw_fileopen(rxu_tostring(s, (char *) &cbuf, NULL), mode);
    status=errno;
    rxu_freetssstack(s);
    rsx_dooutput(fd);
}

/*
** pipe to
** ind string n pstate->pcode
*/
void rsx_pipeto()
{
    exprstack_t *s;
    char cbuf[STRINGCONVSIZE];
    fgw_fdesc *fd;

    status=rxx_execute(NULL, NULL);
    if (status)
        return;
    s=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    fd=fgw_pipeopen(rxu_tostring(s, (char *) &cbuf, NULL), "w");
    status=errno;
    rxu_freetssstack(s);
    rsx_dooutput(fd);
}

/*
** write to
** file descriptor in pstate->pcode
*/
void rsx_writeto()
{
    exprstack_t *s;
    fgw_fdesc *fd;

    status=rxx_execute(NULL, NULL);
    if (status)
        return;
    s=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    fd=fgw_findfd(rxu_toint(s, NULL), pstate->signature | SS_INTERNAL);
    status=errno;
    rxu_freetssstack(s);
    rsx_dooutput(fd);
}

/*
** connections
**
** connect
** connection strings in pstate->pcode
*/
int rsx_connect(char *opts)
{
    return rsx_doconnect(0, opts);
}

/* connect
** connection strings in pstate->pcode
*/
int rsx_defconnect(char *opts)
{
    return rsx_doconnect(1, opts);
}

/*
** set named connection
** connection name on pstate->pcode
*/
int rsx_setconnection()
{
    exprstack_t *e;
    char *h, hb[STRINGCONVSIZE];

    status=rxx_execute(NULL, NULL);
    if (status)
	return -1;
    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    h=rxu_tostring(e, (char *) &hb, NULL);
    rqx_setconnection(pstate, h, NULL);
    status=pstate->ca.sqlcode;
    rxu_freetssstack(e);
    return rsx_conwhenever();
}

/*
** set named source default connection
*/
int rsx_setsource(char *src)
{
    rqx_setconnection(pstate, NULL, src);
    status=pstate->ca.sqlcode;
    return rsx_conwhenever();
}

/*
** set current source default connection
*/
int rsx_setdefault()
{
    rqx_setconnection(pstate, NULL, NULL);
    status=pstate->ca.sqlcode;
    return rsx_conwhenever();
}

/*
** disconnect named connection
** connection name on pstate->pcode
*/
int rsx_disconnect()
{
    exprstack_t *e;
    char *h, hb[STRINGCONVSIZE];

    status=rxx_execute(NULL, NULL);
    if (status)
	return -1;
    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    h=rxu_tostring(e, (char *) &hb, NULL);
    rqx_disconnect(pstate, h, NULL);
    status=pstate->ca.sqlcode;
    rxu_freetssstack(e);
    return rsx_conwhenever();
}

/*
** disconnect named source default connection
*/
int rsx_srcdisconnect(char *src)
{
    rqx_disconnect(pstate, NULL, src);
    status=pstate->ca.sqlcode;
    return rsx_conwhenever();
}

/*
** disconnect current source default connection
*/
int rsx_defdisconnect()
{
    rqx_disconnect(pstate, NULL, NULL);
    status=pstate->ca.sqlcode;
    return rsx_conwhenever();
}

/*
** temporary (per statement) connections
**
** Set current connection
*/
int rsx_curcon(fgw_stacktype *s)
{
    rqx_setstmtcon(pstate->ssp->stmt, rqx_getconnection(), NULL, pstate);
    return pstate->ssp->stmt->ca->sqlcode;
}

/*
** set default connection for named source
*/
int rsx_srcdefault(fgw_stacktype *s)
{
    char *src, sb[STRINGCONVSIZE];
    exprstack_t *e;

    src=rxu_tostring(&pstate->instrp->constant, (char *) &sb, NULL);
    rqx_setstmtcon(pstate->ssp->stmt, NULL, src, pstate);
    return pstate->ssp->stmt->ca->sqlcode;
}

/*
** set default connection for current source
*/
int rsx_curdefault(fgw_stacktype *s)
{
    rqx_setstmtcon(pstate->ssp->stmt, NULL, NULL, pstate);
    return pstate->ssp->stmt->ca->sqlcode;
}

/*
** set explicit connection
*/
int rsx_tmpcon(fgw_stacktype *s)
{
    char *h, hb[STRINGCONVSIZE];
    exprstack_t *e;

    e=(exprstack_t *) fgw_stackpop(s);
    h=rxu_tostring(e, (char *) &hb, NULL);
    rqx_setstmtcon(pstate->ssp->stmt, h, NULL, pstate);
    rxu_freetssstack(e);
    return pstate->ssp->stmt->ca->sqlcode;
}

/*
** SQL statements
**
** prepare
** target variable in pstate->pcode
*/
int rsx_prepare(pcode_t *fmt)
{
    exprstack_t *v;
    int r;

    if ((status=rxx_execute(NULL, NULL)))
	return -1;

    v=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    if (!pstate->ssp->stmt->fmt_type)
	pstate->ssp->stmt->options|=SO_CLRFMT;
    if (!pstate->ssp->stmt->usingvars.pcodehead)
	pstate->ssp->stmt->options|=SO_CLRVAR;
    if (!pstate->ssp->stmt->intovars.pcodehead)
	pstate->ssp->stmt->options|=SO_CLRDST;
    if (!pstate->ssp->stmt->aggregates.pcodehead)
	pstate->ssp->stmt->options|=SO_CLRAGG;
    pstate->ssp->stmt->options|=SO_REALPREPARED;
    rqx_prepare(pstate->ssp->stmt, TOKPTR(pstate->ssp->sqlstart));
    if (!(status=pstate->ssp->stmt->ca->sqlcode))
    {
	if (pstate->ssp->stmt->fmt_type &&
	    (r=rsx_setformat(pstate->ssp->stmt, fmt)))
	{
	    rqx_freestatement(pstate->ssp->stmt);
	    pstate->ssp->stmt=NULL;
	    status=r;
	    return -1;
	}
	if (pstate->ssp->stmt->curstate==ST_EXECUTABLE &&
	    (!(pstate->ssp->stmt->options & SO_CLRFMT) ||
	     !(pstate->ssp->stmt->options & SO_CLRAGG) ||
	     !(pstate->ssp->stmt->options & SO_CLRDST)))
	{
	    rqx_freestatement(pstate->ssp->stmt);
	    pstate->ssp->stmt=NULL;
	    status=RC_NDECL;
	    return -1;
	}
	else
	{
	    pstate->ssp->stmt->st_entry.signature=
		pstate->signature | SS_INTERNAL;
	    fgw_hstidxadd(pstate->vars,
		v->val.identifier, CINTTYPE, 0,
		(char *) &pstate->ssp->stmt->st_entry.int_id);
	    pstate->ssp->stmt=NULL;
/*
** provide some feedback that the prepare was successfull
*/
	    status=RC_NROWS;
	}
    }
    else
    {
	rqx_freestatement(pstate->ssp->stmt);
	pstate->ssp->stmt=NULL;
	if (pstate->flags & PF_ERRORCONT)
	    status=0;
	else
	    return -1;
     }
     return 0;
}

/*
** standalone / source execute
** statement selection is executed in the expression runner
*/
int rsx_executeout(fgw_stacktype *s)
{
    int stid;
    exprstack_t *e;
    fgw_stmttype *st, *dummyst=pstate->ssp->stmt;
    int rc=0;

    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    stid=rxu_toint(e, NULL);
    pstate->ssp->stmt=NULL;
    if (!(st=rqx_findstmt(stid, pstate->signature | SS_INTERNAL)))
         rc=RC_INSID;

    /* if the statement is already being run as part of a foreach, we can't execute it */
    else if (st->refcnt)
        rc=RC_USSTM;
    else
    {
	pstate->ssp->stmt=st;
	if (pstate->curstmt==dummyst)
	    pstate->curstmt=st;
	if (dummyst->fmt_type && st->fmt_type)
	    rc=RC_SPFMT;
	else if (dummyst->usingvars.pcodehead && st->usingvars.pcodehead)
	    rc=RC_SPPHD;
	else if (dummyst->aggregates.pcodehead && st->aggregates.pcodehead)
	    rc=RC_SPPHD;
	else if (dummyst->intovars.pcodehead && st->intovars.pcodehead)
	    rc=RC_SPDST;
	if (rc)
	    goto bad;

	if (dummyst->fmt_type)
		st->fmt_type=dummyst->fmt_type;
	/* move code from placeholder statement to prepared
	   swappcode is necessary so that freestatement doesn't actually zap the pcode */
	if (dummyst->usingvars.pcodehead)
	    rxx_swappcode(&dummyst->usingvars, &st->usingvars);
	if (dummyst->intovars.pcodehead)
	    rxx_swappcode(&dummyst->intovars, &st->intovars);
	if (dummyst->aggregates.pcodehead)
	    rxx_swappcode(&dummyst->aggregates, &st->aggregates);
	st->options|=SO_NOTEXT;
    }

bad:
    rqx_freestatement(dummyst);
    rxu_freetssstack(e);
    if (rc)
	pstate->ca.sqlcode=rc;
    return rc;
}

/*
** redirected execute
** statement selection is executed in the expression runner
*/
int rsx_executein(fgw_stacktype *s)
{
    int stid;
    exprstack_t *e;
    fgw_stmttype *st, *dummyst=pstate->ssp->stmt;
    int rc=0;

    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    stid=rxu_toint(e, NULL);
    pstate->ssp->stmt=NULL;
    if (!(st=rqx_findstmt(stid, pstate->signature | SS_INTERNAL)))
         rc=RC_INSID;

    /* if the statement is already being run as part of a foreach, we can't execute it */
    else if (st->refcnt)
        rc=RC_USSTM;
    else
    {
	pstate->ssp->stmt=st;
	if (pstate->curstmt==dummyst)
	    pstate->curstmt=st;
	if (dummyst->options & SO_DONEFMT)
	    rc=RC_SPFMT;
	else if (dummyst->usingvars.pcodehead && st->usingvars.pcodehead)
	    rc=RC_SPPHD;
	else if (st->aggregates.pcodehead)
	    rc=RC_SPPHD;
	else if (st->intovars.pcodehead)
	    rc=RC_SPDST;
	if (rc)
	    goto bad;

	/* move code from placeholder statement to prepared
	   swappcode is necessary so that freestatement doesn't actually zap the pcode */
	if (dummyst->usingvars.pcodehead)
	    rxx_swappcode(&dummyst->usingvars, &st->usingvars);
	st->options|=SO_NOTEXT;
    }

bad:
    rqx_freestatement(dummyst);
    rxu_freetssstack(e);
    if (rc)
	pstate->ca.sqlcode=rc;
    return rc;
}

/* free
** statement id in pstate->pcode
*/
int rsx_free()
{
    fgw_stmttype *st;
    int s;
    exprstack_t *e;

    rxx_execute(NULL, NULL);
    if (status)
	return -1;
    e=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    s=rxu_toint(e, NULL);
    if (!(st=rqx_findstmt(s, pstate->signature | SS_INTERNAL)))
    {
        status=RC_INSID;
	goto bad;
    }
    if (st->refcnt)
    {
        status=RC_USSTM;
	goto bad;
    }
    rqx_freestatement(st);
    return 0;

bad:
    pstate->ca.sqlcode=status;
    return -1;
}

/*
** non redirected statement (includes input et all)
** (input streams in pstate->pcode)
*/
int rsx_nonredirected(pcode_t *fmt)
{
    int r=0;
    char *query;

    /* set up input stream if not an sql statement */
    /* TODO whenever error continue for file open */
    if (rxx_runnable() && (status=rxx_execute(NULL, NULL)))
	goto bad;
    if (pstate->ssp->stmt->options & SO_NOTEXT)
	query="";
    else
	query=TOKPTR(pstate->ssp->sqlstart);
    if (pstate->ssp->stmt->curstate==ST_UNINITIALIZED)
    {
        rqx_prepare(pstate->ssp->stmt, query);
        r=pstate->ssp->stmt->ca->sqlcode;
    }
    if (pstate->ssp->stmt->curstate==ST_EXECUTABLE)
    {
        if (!pstate->ssp->stmt->intovars.pcodehead &&
            (pstate->ssp->stmt->options & SO_NOFMT))
        {
            rqx_run("", pstate->ssp->stmt, pstate->touch);
            status=pstate->ssp->stmt->ca->sqlcode;
        }
        else
            status=RC_NDECL;
    }
    else if (!r)
    {
        if (!(r=rsx_primeformat(pstate->ssp->stmt)) && !(r=rsx_setformat(pstate->ssp->stmt, fmt)))
        {
            rqf_dorows(pstate->ssp->stmt, pstate->txtvar, 1);
            status=pstate->ssp->stmt->ca->sqlcode;
        }
    }
    if (!status)
        status=r;
    else if (status==RC_NOTFOUND)
        status=0;

bad:
    return rsx_whenever();
}

/*
** autocommit
** executed by the expression runner
*/
int rsx_autocommit(fgw_stacktype *s)
{
    exprstack_t *e;
    int n, sz, rc=0;

    e=(exprstack_t *) fgw_stackpop(s);
    sz=rxu_toint(e, &n);
    rxu_freetssstack(e);
    if (n || sz<1)
        rc=RC_BADAC;
    pstate->ssp->stmt->autocommit=sz;
    return rc;
}

/*
** switch to redirected statement
** executed by the expression runner
*/
int rsx_redirect(fgw_stacktype *s)
{
    pstate->ssp++;
    return 0;
}

/*
** redirected statement (includes input et all)
** (input streams in pstate->pcode)
*/
int rsx_redirected(pcode_t *fmt)
{
    int r=0;
    char *query;

    /* start from source statement */
    pstate->ssp--;

    /* set up input streams, etc */
    /* TODO whenever error continue for file open */
    if ((status=rxx_execute(NULL, NULL)))
	goto bad;
    if (pstate->stmtstack[1].stmt->phcount==0)
    {
	status=RC_NOSTR;
	goto bad;
    }
    if (pstate->stmtstack[0].stmt->options & SO_NOTEXT)
        query="";
    else
        query=TOKPTR(pstate->stmtstack[0].sqlstart);
    if (pstate->stmtstack[0].stmt->curstate==ST_UNINITIALIZED)
    {
        rqx_prepare(pstate->stmtstack[0].stmt, query);
        r=pstate->stmtstack[0].stmt->ca->sqlcode;
    }
    if (!r)
    {
        if (pstate->stmtstack[0].stmt->curstate!=ST_DECLARED)
	{
            status=RC_NISTR;
	    goto bad;
	}
	if (pstate->stmtstack[1].stmt->options & SO_NOTEXT)
	    query="";
	else
	    query=TOKPTR(pstate->stmtstack[1].sqlstart);
        if (pstate->stmtstack[1].stmt->curstate==ST_UNINITIALIZED)
        {
             rqx_prepare(pstate->stmtstack[1].stmt, query);
             r=pstate->stmtstack[1].stmt->ca->sqlcode;
        }
        if (!r)
        {
	    if (pstate->stmtstack[1].stmt->curstate!=ST_EXECUTABLE)
	    {
		status=RC_NOSTR;
		goto bad;
	    }
            if (sqsl_asktouch(pstate->touch))
                rqx_move(&pstate->stmtstack[0], &pstate->stmtstack[1],
                         pstate->errorinfo);
            r=pstate->ca.sqlcode;
            if (pstate->ssp->stmt->options & SO_REALPREPARED)
                rqx_closestatement(pstate->ssp->stmt);
            else
                rqx_freestatement(pstate->ssp->stmt);
        }
    }
    (pstate->ssp--)->stmt=NULL;
    status=r;

bad:
    return rsx_whenever();
}

/*
** stream execution - these are executed on the expression runner
**
** open a file for reading
*/
int rsx_inputfrom(fgw_stacktype *s)
{
    exprstack_t *e;

    e=(exprstack_t *) fgw_stackpeek(s);
    e->totype=0;
    return 0;
}

/*
** open a file for reading
*/
int rsx_pipefrom(fgw_stacktype *s)
{
    exprstack_t *e;

    e=(exprstack_t *) fgw_stackpeek(s);
    e->totype=-1;
    return 0;
}

/*
** set a file descriptor for reading
*/
int rsx_readfrom(fgw_stacktype *s)
{
    exprstack_t *e;

    e=(exprstack_t *) fgw_stackpeek(s);
    e->totype=1;
    return 0;
}

/*
** reject file name
*/
int rsx_rejectfile(fgw_stacktype *s)
{
    exprstack_t *e;
    char cbuf[STRINGCONVSIZE];
    int rc;

    e=(exprstack_t *) fgw_stackpop(s);
    pstate->ssp->stmt->reject= fgw_fileopen(rxu_tostring(e,
					(char *) &cbuf, NULL), "a");
    rc=errno;
    rxu_freetssstack(e);
    return rc;
}

/*
** reject block size
*/
int rsx_rejectsize(fgw_stacktype *s)
{
    exprstack_t *e;
    int n, sz, rc=0;

    e=(exprstack_t *) fgw_stackpop(s);
    sz=rxu_toint(e, &n);
    rxu_freetssstack(e);
    if (n || sz<MINBLOCKSIZE || sz>MAXBLOCKSIZE)
        rc=RC_BADBS;
    pstate->ssp->stmt->blocksize=sz;
    return rc;
}

/*
** set stream to delimited
*/
int rsx_delimited(fgw_stacktype *s)
{
    char *d, *e;
    char bd[STRINGCONVSIZE], be[STRINGCONVSIZE];
    int nd, ne;
    exprstack_t *ed, *ee, *ei;
    int rc=0;

    pstate->ssp->stmt->ca=&pstate->oserr;
    ee=(exprstack_t *) fgw_stackpop(s);
    ed=(exprstack_t *) fgw_stackpop(s);
    ei=(exprstack_t *) fgw_stackpop(s);
    d=rxu_tostring(ed, (char *) &bd, &nd);
    e=rxu_tostring(ee, (char *) &be, &ne);
    if (nd || ne || !*d || !*e)
    {
        rc=RC_BADDE;
	goto bad; 
    }
    if ((rc=ldd_newstatement(pstate->curstmt, d, e)))
	goto bad;
    rc=rsx_openstream(pstate->curstmt, ei);

bad:
    rxu_freetssstack(ee);
    rxu_freetssstack(ed);
    rxu_freetssstack(ei);
    return rc;
}

/*
** set stream to regex delimited
*/
int rsx_regex(fgw_stacktype *s)
{
    char *d;
    char bd[STRINGCONVSIZE];
    int nd;
    exprstack_t *ed, *ei;
    int rc=0;

    pstate->ssp->stmt->ca=&pstate->oserr;
#ifdef HAVE_REGEX
    ed=(exprstack_t *) fgw_stackpop(s);
    ei=(exprstack_t *) fgw_stackpop(s);
    d=rxu_tostring(ed, (char *) &bd, &nd);
    if (nd || !*d)
    {
        rc=RC_BADDE;
	goto bad; 
    }
    if ((rc=ldr_newstatement(pstate->curstmt, d)))
	goto bad;
    rc=rsx_openstream(pstate->curstmt, ei);

bad:
    rxu_freetssstack(ei);
    rxu_freetssstack(ed);
    return rc;
#else
    return RC_NLNKD;
#endif
}

/*
** set stream to extractor
*/
int rsx_extractor(fgw_stacktype *s)
{
    char *e, *m;
    char be[STRINGCONVSIZE], bm[STRINGCONVSIZE];
    int ne, nm;
    exprstack_t *ee, *em, *ei;
    int rc=0;

    pstate->ssp->stmt->ca=&pstate->oserr;
    ee=(exprstack_t *) fgw_stackpop(s);
    em=(exprstack_t *) fgw_stackpop(s);
    ei=(exprstack_t *) fgw_stackpop(s);
    m=rxu_tostring(em, (char *) &bm, &nm);
    e=rxu_tostring(ee, (char *) &be, &ne);
    if ((rc=lde_newstatement(pstate->curstmt, m, e)))
	goto bad;
    rc=rsx_openstream(pstate->curstmt, ei);

bad:
    rxu_freetssstack(ei);
    rxu_freetssstack(ee);
    rxu_freetssstack(em);
    return rc;
}

/*
** set stream to pattern
*/
int rsx_pattern(fgw_stacktype *s)
{
    char *p;
    char bp[STRINGCONVSIZE];
    int np;
    exprstack_t *ep, *et;
    int rc=0;

    pstate->ssp->stmt->ca=&pstate->oserr;
    ep=(exprstack_t *) fgw_stackpop(s);
    et=(exprstack_t *) fgw_stackpop(s);
    p=rxu_tostring(ep, (char *) &bp, &np);
    if (np || !*p)
    {
        rc=RC_BADDE;
	goto bad; 
    }
    if ((rc=ldp_newstatement(pstate->ssp->stmt, p)))
	goto bad;
    rc=rsx_openstream(pstate->curstmt, et);

bad:
    rxu_freetssstack(et);
    rxu_freetssstack(ep);
    return rc;
}

/*
** statics
**
** determine default format and set html tags
*/
static int rsx_doformat()
{
    char *pre_table="", *post_table="";

    if (!pstate->ssp->stmt->fmt_type)
    {
	pstate->ssp->stmt->options|=SO_NOFMT;
	if (pstate->ssp->stmt->curstate!=ST_EXECUTABLE)
/*
** turn off output if INTO clause and no explicit FORMAT clause
*/
	    pstate->ssp->stmt->fmt_type=(
		pstate->ssp->stmt->intovars.pcodehead?
		FMT_NULL:
/*
** default format is BRIEF
*/
		((pstate->flags & PF_ISHTML)?
			FMT_FULL: FMT_BRIEF));
    }
    else if (pstate->flags & PF_ISHTML)
	switch (pstate->ssp->stmt->fmt_type)
	{
	  case FMT_BRIEF:
	    pstate->ssp->stmt->fmt_type==FMT_FULL;
	    break;
	}
    pstate->ssp->stmt->width=pstate->width;
    if (pstate->ssp->stmt->fmt_type!=FMT_NULL)
    {
	pstate->states|=PS_DONEDISPLAY;
/*
** set html strings here
*/
	if (pstate->flags & PF_ISHTML)
	{
	    if (pstate->style!=F_TABLE)
	    {
		pre_table=H_PRETABLE;
		post_table=H_POSTTABLE;
	    }
/*
** no tags within a <PRE> block
*/
	    if (pstate->style!=F_PRE)
	    {
/*
** vertical format
*/
		if (pstate->ssp->stmt->fmt_type==FMT_VERTICAL)
		{
		    pstate->ssp->stmt->pretable="";
		    pstate->ssp->stmt->posttable="";
		    pstate->ssp->stmt->prerow=pre_table;
		    pstate->ssp->stmt->postrow=post_table;
		    pstate->ssp->stmt->preheader=H_VERTHEADER;
		    pstate->ssp->stmt->postheader=H_VERTPOSTHEA;
		    pstate->ssp->stmt->prefield=H_VERTFIELD;
		    pstate->ssp->stmt->postfield=H_VERTPOSTFIELD;
		    rqf_countfields(pstate->ssp->stmt,
			(strlen(pre_table)==0),
			strlen(post_table));
		}
/*
** full format and explicit fields
*/
		else if (pstate->ssp->stmt->fmt_type==FMT_FULL &&
		         (pstate->ssp->stmt->options & SO_TABULAR))
		{
		    pstate->ssp->stmt->pretable=pre_table;
		    pstate->ssp->stmt->posttable=post_table;
		    pstate->ssp->stmt->prerow="<TR>\n";
		    pstate->ssp->stmt->postrow="</TR>\n";
		    pstate->ssp->stmt->preheader=H_FULLHEADER;
		    pstate->ssp->stmt->postheader=H_FULLPOSTHEA;
		    pstate->ssp->stmt->prefield=H_FULLFIELD;
		    pstate->ssp->stmt->postfield=H_FULLPOSTFIELD;
		    rqf_countfields(pstate->ssp->stmt, 1,
			strlen(pre_table));
		    pstate->newstyle=F_TABLE;
		}
/*
** full or delimited format and inside a table already
*/
		else if (pstate->style==F_TABLE)
		{
		    char *s;

		    fgw_tssmalloc(&pstate->ssp->stmt->htmlhead, 32);
		    sprintf(s, H_FULLTABLE, rqf_fieldcount());
		    pstate->ssp->stmt->pretable=s;
		    pstate->ssp->stmt->posttable=H_FULLPOSTTABLE;
		    pstate->ssp->stmt->prerow="";
		    pstate->ssp->stmt->postrow="";
		    pstate->ssp->stmt->preheader="";
		    pstate->ssp->stmt->postheader="";
		    pstate->ssp->stmt->prefield="";
		    pstate->ssp->stmt->postfield="";
		    pstate->newstyle=F_PRE;
		}
/*
** full format and no explicit fields, delimited
*/
		else
		{
		    pstate->ssp->stmt->pretable="<PRE>\n";
		    pstate->ssp->stmt->posttable="</PRE>\n";
		    pstate->ssp->stmt->prerow="";
		    pstate->ssp->stmt->postrow="";
		    pstate->ssp->stmt->preheader="";
		    pstate->ssp->stmt->postheader="";
		    pstate->ssp->stmt->prefield="";
		    pstate->ssp->stmt->postfield="";
		    rqf_countfields(pstate->ssp->stmt, 0, 1);
		    pstate->newstyle=F_PRE;
		}
	    }
	}
    }
}

/*
** common code to set formats and headers
*/
static int rsx_primeformat(fgw_stmttype *st_p)
{
    if (st_p==NULL)
	return RC_INSID;
    if (st_p->options & SO_LATE_DESCRIBE)
    {
	if (st_p->curstate<ST_DECLARED)
	    return RC_INSID;
	rqx_nextrow(st_p);
	if (st_p->ca->sqlcode==0)
	    st_p->options|=SO_PRIMED;
	return st_p->ca->sqlcode;
    }
    return 0;
}

static int rsx_setformat(fgw_stmttype *st_p, pcode_t *fmt)
{
    int i, c, rc;
    exprstack_t *t, *f;
    char cbuf[STRINGCONVSIZE];

    /* already been here */
    if (st_p->options & SO_DONEFMT)
	return 0;

    /* set defaults */
    rsx_doformat();

    /* no formats */
    if (!fmt || !fmt->pcodehead)
	return 0;
    if ((rc=rxx_execute(fmt, st_p)))
	return rc;
    if (st_p->fmt_type==FMT_DELIMITED)
    {
	char bd[STRINGCONVSIZE], be[STRINGCONVSIZE];
	int nd, ne;
	exprstack_t *ed, *ee;

	ee=(exprstack_t *) fgw_stackpop(&st_p->stack);
	ed=(exprstack_t *) fgw_stackpop(&st_p->stack);
	st_p->del[0]=*rxu_tostring(ed, (char *) &bd, &nd);
	st_p->esc[0]=*rxu_tostring(ee, (char *) &be, &ne);
	if (nd || ne || !pstate->ssp->stmt->del[0] ||
	    !pstate->ssp->stmt->esc[0])
	    return RC_BADDE;
	rxu_freetssstack(ed);
	rxu_freetssstack(ee);
    }
    t=(exprstack_t *) fgw_stackpeek(&st_p->stack);
    if (c=t->count)
	t=t-c+1;
    if (st_p->fmt_type!=FMT_DELIMITED)
    {
	if (t->type!=EMPTYMARKER)
	{
	    for (i=c, f=t; i; i--, f++)
		rqf_setheader(st_p, rxu_tostring(f, (char *) &cbuf, NULL));
	}
	else
	    c=1;
	t--;
	fgw_stackdrop(&st_p->stack, c);
    }
    rqf_setformat(st_p, rxu_tostring(t, (char *) &cbuf, NULL));
    fgw_tssdrop(&st_p->usinghead);
    fgw_stackdrop(&st_p->stack, 1);
    rxx_droppcode(fmt);
    return 0;
}

/*
** close output stream
*/
static void rsx_closeoutput()
{
    if (pstate->curr_fd!=FD_TOBLOB &&
        pstate->curr_fd!=FD_IGNORE)
    {   
	rqf_flush(pstate->txtvar, 1);
	if (pstate->curr_fd!=pstate->def_fd &&
	    !pstate->curr_fd->fd_refcount &&
	    !(pstate->curr_fd->fd_entry.signature & SS_INTERNAL))
	{   
	    pstate->childstatus=fgw_fdclose(pstate->curr_fd);
	    pstate->oserr.sqlcode=status;
	}   
    }
}

/*
** set output stream
*/
static void rsx_dooutput(fgw_fdesc *fd)
{

/*
** if it is a pipe, see if we can write to it, or child has gone away
*/
    if (!status && fd!=FD_TOBLOB &&
	fd!=FD_IGNORE && fd->fd_pid)
    {
/*
** give the child shell the time to exec the correct image
*/
        fgw_millisleep(100);
        fgw_fdwrite(fd, "", 0);
        if (status)
	{
	    pstate->oserr.sqlcode=status;
	    if (!(fd->fd_entry.signature & SS_INTERNAL))
		pstate->childstatus=fgw_fdclose(fd);
	}
    }
    if (!status)
    {
	rsx_closeoutput();
	rqf_openfile(pstate->txtvar, fd);
	pstate->curr_fd=fd;
	pstate->style=F_NONE;
/*
** we sign file descriptors (bar the default one) so that we don't interfere
** with anybody else and viceversa
*/
	if (fd!=pstate->def_fd && !fd->fd_entry.signature)
	    fd->fd_entry.signature=pstate->signature;
    }
    else
    {
	pstate->oserr.sqlcode=status;
	status=0;
    }
}

/*
** whenever error checks
** connection statements
*/
static int rsx_conwhenever()
{
    if (pstate->flags & PF_ERRORCONT)
        status=0;
    else
	return -1;
    return 0;
}

/*
** whenever error checks
** statement cleanup
*/
static int rsx_whenever()
{
    int rc=0;

    if (status!=RC_NROWS)
    {
        if (pstate->flags & PF_ERRORCONT)
            status=0;
        else
	    rc=-1;
    }
    if (!pstate->ssp->stmt)
	return rc;
    else if (pstate->ssp->stmt->options & SO_REALPREPARED)
	rqx_closestatement(pstate->ssp->stmt);
    else
	rqx_freestatement(pstate->ssp->stmt);
    pstate->ssp->stmt=NULL;
    return rc;
}

/*
** open input stream
*/
static int rsx_openstream(fgw_stmttype *st, exprstack_t *e)
{
    char *fname, fnbuf[STRINGCONVSIZE];
    int fd;

    switch (e->totype)
    {
      case 0:
        fname=rxu_tostring(e, (char *) &fnbuf, NULL);
        st->stream=fgw_fileopen(fname, "r");
        break;
      case -1:
        fname=rxu_tostring(e, (char *) &fnbuf, NULL);
        st->stream=fgw_pipeopen(fname, "r");
        break;
      default:
	fd=rxu_toint(e, NULL);
        st->stream=fgw_findfd(fd, st->st_entry.signature|SS_INTERNAL);

    }
    rxu_freetssstack(e);
    st->options|=SO_NOTEXT;
    return errno;
}

/*
** actual connection code
*/
static int rsx_doconnect(int isdefault, char *opts)
{
    int hasuser, hasas;
    char *host, hostb[STRINGCONVSIZE];
    char *as=NULL, asb[STRINGCONVSIZE];
    char *src;
    char *sh1=NULL, *sh2=NULL;
    char  sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];
    exprstack_t *s1, *s2, *s3, *s4, *s5;

    if (rxx_runnable() && (status=rxx_execute(NULL, NULL)))
	return -1;
/*
** this is expected to already be a string - null if no source specified
*/
    s5=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
    src=s5->val.string;
    hasuser=s5->count>=2;
    hasas=s5->count & 1;
    
    if (hasuser)
    {
        s4=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
        s3=(exprstack_t *) fgw_stackpop(&pstate->exprlist);

        sh1=rxu_tostring(s3, (char *) &sbuf1, NULL);
        sh2=rxu_tostring(s4, (char *) &sbuf2, NULL);
    }
    if (hasas)
    {
        s2=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
        as=rxu_tostring(s2, (char *) &asb, NULL);
    }
    if (isdefault)
	host=NULL;
    else
    {
	s1=(exprstack_t *) fgw_stackpop(&pstate->exprlist);
	host=rxu_tostring(s1, (char *) &hostb, NULL);
    }
    rqx_connect(pstate, host, src, as, sh1, sh2, opts);

    /* technically, no need but for completeness */
    rxu_freetssstack(s5);
    if (hasuser)
    {
        rxu_freetssstack(s4);
        rxu_freetssstack(s3);
    }
    if (hasas)
        rxu_freetssstack(s2);
    if (!isdefault)
	rxu_freetssstack(s1);
    status=pstate->ca.sqlcode;
    return rsx_conwhenever();
}
