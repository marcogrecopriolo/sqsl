/*
	CRXFC.c  -  Expression stack runtime execution: functions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: May 00
	Current release: asc 16

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_REGEX
#include <regex.h>
#endif
#include "coslc.h"
#include "ccmnc.h"
#include "cascc.h"
#include "cfioc.h"
#include "ctypc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqfc.h"
#include "crqxp.h"
#include "crxuc.h"
#include "crxcc.h"
#include "crxfc.h"

extern TGLOB parserstate_t *pstate;

/*
** list functions
**
** display labels
*/
int rxf_displaylabels(fgw_stacktype *s)
{
    int lc, ac, n, ind;
    exprstack_t *i, o, d;

    if (!pstate->curstmt || !pstate->curstmt->source)
	return RC_CNUHR;
    memset(&o, 0, sizeof(o));
    memset(&d, 0, sizeof(d));
    if ((lc=rqx_counttokens(pstate->curstmt)))
	rqx_describecol(pstate->curstmt, 0, &d);

    /* count */
    if (pstate->instrp->constant.flags & EF_FUNCCOUNT)
    {
	o.type=CINTTYPE;
	if (lc && d.val.string)
	    o.val.intvalue=lc;
	else
	    rxu_setnull(&o, CINTTYPE);
	rxu_pushresult(s, &o);
    }

    /* list of values */
    else if (pstate->instrp->constant.flags & EF_FUNCHASH)
    {

	/* we trust the compiler to have the argument and
	   element list right!
	   we also ignore the argument list as it's empty
	 */
	i=rxu_arglist(s, &ac);
	while (ac-- && d.val.string)
	{
	    n=rxu_toint(i, &ind);
	    if (ind || n<1 || n>lc)
		rxu_setnull(&o, CSTRINGTYPE);
	    else
	    {
		o.type=CSTRINGTYPE;
		o.flags|=EF_NOTONTSS;
	        rqx_describecol(pstate->curstmt, n-1, &d);
	        o.val.string=d.val.string;
	    }
	    rxu_pushresult2(s, i, &o);
	    i++;
	}
    }

    /* all values */
    else if (lc && d.val.string)
    {
	o.type=CSTRINGTYPE;
	o.val.string=d.val.string;
        o.flags|=EF_NOTONTSS;
	rxu_pushresult(s, &o);
	for (n=1; n<lc; n++)
	{
	    rqx_describecol(pstate->curstmt, n, &d);
	    o.val.string=d.val.string;
	    rxu_pushresult(s, &o);
	}
    }
    else
    {
   	rxu_setnull(&o, CSTRINGTYPE);
	rxu_pushresult(s, &o);
    }
    return 0;
}
/*
** columns
*/
int rxf_columns(fgw_stacktype *s)
{
    int vc, ac, n, ind;
    exprstack_t *i, o, d, *e;

    if (!pstate->curstmt || !pstate->curstmt->source)
	return RC_CNUHR;
    memset(&o, 0, sizeof(o));
    memset(&d, 0, sizeof(d));
    vc=rqx_counttokens(pstate->curstmt);

    /* count */
    if (pstate->instrp->constant.flags & EF_FUNCCOUNT)
    {
	o.type=CINTTYPE;
	if (vc>0)
	    o.val.intvalue=vc;
	else
	    rxu_setnull(&o, CINTTYPE);
	rxu_pushresult(s, &o);
    }

    /* list of values */
    else if (pstate->instrp->constant.flags & EF_FUNCHASH)
    {
	
	/* we trust the compiler to have the argument and
	   element list right!
	   we also ignore the argument list as it's empty
	 */
	i=rxu_arglist(s, &ac);
	while (ac--)
	{
	    n=rxu_toint(i, &ind);
	    if (ind || n<1 || n>vc)
		rxu_setnull(&o, CSTRINGTYPE);
	    else
	    {
	        e=rqx_nexttoken(n-1, pstate->curstmt);
		memcpy(&o, e, sizeof(o));
		o.count=0;
		o.flags|=EF_NOTONTSS;
	    }
	    rxu_pushresult2(s, i, &o);
	    i++;
	}
    }

    /* all values */
    else if (vc>0)
    {
	for (n=0; n<vc; n++)
	{
	    e=rqx_nexttoken(n, pstate->curstmt);
	    memcpy(&o, e, sizeof(o));
	    o.flags|=EF_NOTONTSS;
	    rxu_pushresult(s, &o);
	}
    }
    else
    {
   	rxu_setnull(&o, CSTRINGTYPE);
	rxu_pushresult(s, &o);
    }
    return 0;
}
/*
** known functions
**
** displayformat
*/
int rxf_displayformat(fgw_stacktype *s)
{
    int ac, n, cc;
    int cf, f=0;
    int al=0, cl=0, l;
    exprstack_t *i, o, d;
    char cvbuf[12];
    int rc=0;

    if (!pstate->curstmt || !pstate->curstmt->source)
	return RC_CNUHR;
    i=rxu_arglist(s, &ac);
    memset(&o, 0, sizeof(o));
    cc=rqx_counttokens(pstate->curstmt);
    for(;;)
    {
	cf=f++;
	if (ac)
	{
	    if (f>ac)
		break;
	    cf=rxu_toint(i++, &n);
	    if (n || cf<1 || cf>cc)
	    {
		rxu_pushresult(s, &o);
		return RC_WRCNO;
	    }
	    cf--;
	}
	else if (cf>=cc)
	    break;
	rqx_describecol(pstate->curstmt, cf, &d);
	if (!cf && !d.val.string)
	{
	    rxu_setnull(&o, CSTRINGTYPE);
	    break;
	}
	switch (d.type)
	{
	  case CDTIMETYPE:
	  case CINVTYPE:
	  case CSTRINGTYPE:
	    if (d.length)
	        sprintf((char *) &cvbuf, "%d", d.length);
	    else
		strcpy((char *) &cvbuf, "");
	    cl+=strlen(cvbuf)+3;
	    break;
	  default:
	    cl+=5;		/* "%ddt " enough for most types */
	}
	if (al<cl)
	{
	    char *nb;

	    al+=256;
	    nb=fgw_srealloc(o.val.string, al);
	    if (!nb)
	    {
		rxu_setnull(&o, CSTRINGTYPE);
		rc=RC_NOMEM;
		break;
	    }
	    o.val.string=nb;
	}
	if (cf>0)
	    strcat(o.val.string, " ");
	else
	    *o.val.string='\0';
	switch (d.type)
	{
	  case CINTTYPE:
	    strcat(o.val.string, "%11i");
	    break;
	  case CDOUBLETYPE:
	    strcat(o.val.string, "%12.6g");
	    break;
	  case CDATETYPE:
	    strcat(o.val.string, "%11s");
	    break;
	  case CDTIMETYPE:
	  case CINVTYPE:
	    strcat(o.val.string, "%");
	    if (strlen(cvbuf))
		strcat(o.val.string, cvbuf);
	    strcat(o.val.string, "s");
	    break;
	  case CSTRINGTYPE:
	    strcat(o.val.string, "%");
	    if (strlen(cvbuf))
	    {
		strcat(o.val.string, "-");
		strcat(o.val.string, cvbuf);
	    }
	    strcat(o.val.string, "s");
	    break;
	  case CDECIMALTYPE:
	  case CMONEYTYPE:
	    l=rxu_exprwidth(&d);
	    sprintf((char *) cvbuf, "%i", l);
	    strcat(o.val.string, "%");
	    strcat(o.val.string, cvbuf);
	    strcat(o.val.string, "s");
	    break;
	  case FGWBYTETYPE:
	    strcat(o.val.string, "%11s");
	    break;
	}
    }

    o.type=CSTRINGTYPE;
    rxu_pushresult(s, &o);
    return rc;
}
/*
** blobs:
**
** filetotext
*/
int rxf_filetotext(fgw_stacktype *s)
{
    exprstack_t *i, o;
    int n, r, rc;
    char *c, cbuf[STRINGCONVSIZE];
    fgw_fdesc *fd;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    if (!(fd=fgw_fileopen(c, "r")))
	goto bad;
    fd->fd_linelen=0;			/* automatic reallocation */
    while ((r=fgw_fdread(fd))>0);
    if (r<0)
	goto bad;
    o.type=CSTRINGTYPE;
    if (!(o.val.string=fgw_smalloc(fd->fd_buflen)))
    {
	rc=RC_NOMEM;
	goto bad1;
    }
    fgw_move(o.val.string, fd->fd_buf, fd->fd_buflen);
    o.length=fd->fd_buflen;
    (void) fgw_fdclose(fd);
    rxu_pushresult(s, &o);
    return 0;

bad:
    rc=pstate->oserr.sqlcode=errno;
bad1:
    if (fd)
	(void) fgw_fdclose(fd);
    rxu_setnull(&o, CSTRINGTYPE);
    rxu_pushresult(s, &o);
    return rc;
}

/*
** filetoblob
*/
int rxf_filetoblob(fgw_stacktype *s)
{
    exprstack_t *i, o;
    int n, r, rc;
    char *c, cbuf[STRINGCONVSIZE];
    fgw_fdesc *fd;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    if (!(fd=fgw_fileopen(c, "r")))
	goto bad;
    fd->fd_linelen=0;			/* automatic reallocation */
    while ((r=fgw_fdread(fd))>0);
    if (r<0)
	goto bad;
    o.type=FGWBYTETYPE;
    if (!(o.val.string=fgw_smalloc(fd->fd_buflen)))
    {
	rc=RC_NOMEM;
	goto bad1;
    }
    fgw_move(o.val.string, fd->fd_buf, fd->fd_buflen);
    o.length=fd->fd_buflen;
    (void) fgw_fdclose(fd);
    rxu_pushresult(s, &o);
    return 0;

bad:
    rc=pstate->oserr.sqlcode=errno;
bad1:
    if (fd)
	(void) fgw_fdclose(fd);
    rxu_setnull(&o, FGWBYTETYPE);
    rxu_pushresult(s, &o);
    return rc;
}

/*
** string
**
** ascii
*/
int rxf_ascii(fgw_stacktype *s)
{
    int c, n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
/* FIXME: not GLS aware */
    if (!(o.val.string=fgw_smalloc(1)))
	 return RC_NOMEM;
    else
    {
	o.type=CSTRINGTYPE;
	c=rxu_toint(i, &n);
	if (n)
	   rxu_setnull(&o, CSTRINGTYPE);
	else if (c<1 || c>255)
	   return RC_BFINV;
	else
	{
	    *o.val.string=(char) c;
	    *(o.val.string+1)='\0';
	}
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** clipped
*/
int rxf_clipped(fgw_stacktype *s)
{
    int n, l;
    exprstack_t *i, o;
    char *c, cbuf[STRINGCONVSIZE], *p;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    if (n)
	l=0;
    else if (l=strlen(c))
    {
	p=c+l-1;
	while (*p--==' ')
	    l--;
    }
    if (!(o.val.string=fgw_smalloc(l)))
	return RC_NOMEM;
    else
    {
	o.type=CSTRINGTYPE;
	if (n)
	    rxu_setnull(&o, CSTRINGTYPE);
	else
	{
	    if (l)
		fgw_move(o.val.string, c, l);
	    *(o.val.string+l)='\0';
	}
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** does actual padding
*/
static void rxf_dopadding(int cnt, char *ist, char *ost)
{
    int l;

    l=strlen(ist);
    while (cnt>0)
    {
	fgw_move(ost, ist, cnt);
	cnt-=l;
	ost+=l;
    }
}

/*
** spaces
*/
int rxf_spaces(fgw_stacktype *s)
{
    int sp, n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    sp=rxu_toint(i, &n);
    if (n)
	sp=1;
    if (sp<=0)
	return RC_BFINV;
    else if (!(o.val.string=(char *) fgw_smalloc(sp)))
	return RC_NOMEM;
    if (n)
	rxu_setnull(&o, CSTRINGTYPE);
    else
    {
	o.type=CSTRINGTYPE;
	rxf_dopadding(sp, " ", o.val.string);
	*(o.val.string+sp)='\0';
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** sleep
*/
int rxf_sleep(fgw_stacktype *s)
{
    int t, n;
    char *c, cbuf[STRINGCONVSIZE];
    exprstack_t *i, o;

    i=rxu_arglist(s, NULL);
    if (!(pstate->runneroptions & RT_CANRUNSLEEP))
	return RC_CNUHR;
    t=rxu_toint(i, &n);
    memset(&o, 0, sizeof(o));
    o.type=CINTTYPE;
    if (n || t<1 || t>1000)
	rxu_setnull(&o, CINTTYPE);
    else
    {
	o.val.intvalue=t;
	fgw_millisleep(t*1000);
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** fork
*/
int rxf_fork(fgw_stacktype *s)
{
    exprstack_t o;

    memset(&o, 0, sizeof(o));
    if (!(pstate->runneroptions & RT_CANFORK) ||
	(pstate->states & PS_ISREENTRANT))
	return RC_NOFRK;
    o.type=CINTTYPE;
    o.val.intvalue=rxc_dofork();
    rxu_pushresult(s, &o);

/* we're only going to allow one fork per expression */
    pstate->runneroptions&=~RT_CANFORK;
    return 0;
}

/*
** exec
*/
int rxf_exec(fgw_stacktype *s)
{
    int n;
    char *c, cbuf[STRINGCONVSIZE];
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    if (!(pstate->runneroptions & RT_CANSYSTEM))
	return RC_CNUHR;
    else
    {
	c=rxu_tostring(i, (char *) &cbuf, &n);
/*
** a bit too overprotective, but only children can exec
*/
	if (pstate->states & PS_ISCHILD)
	{
	    fgw_childzap();
	    fgw_execl(c);
	    pstate->oserr.sqlcode=errno;
	}
	else
	    return RC_CNUHR;
	o.type=CINTTYPE;
	o.val.intvalue=-1;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** waitpid
*/
int rxf_waitpid(fgw_stacktype *s)
{
    int pid, n1;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    if (!(pstate->runneroptions & RT_CANSYSTEM))
	return RC_CNUHR;
    else
    {
	pid=rxu_toint(i, &n1);
	o.type=CINTTYPE;
	o.val.intvalue=rxc_dowaitpid(pid);
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** wait
*/
int rxf_wait(fgw_stacktype *s)
{
    exprstack_t o;

    memset(&o, 0, sizeof(o));
    if (!(pstate->runneroptions & RT_CANSYSTEM))
	return RC_CNUHR;
    o.type=CINTTYPE;
    do
    {
        o.val.intvalue=fgw_wait(&pstate->childstatus);
    } while (o.val.intvalue==-1 && errno==EINTR);
    if (o.val.intvalue==-1)
        pstate->oserr.sqlcode=errno;
    else
        pstate->childstatus>>=8;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** spawn
*/
int rxf_spawn(fgw_stacktype *s)
{
    int n;
    char *c, cbuf[STRINGCONVSIZE];
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    if (!(pstate->runneroptions & RT_CANSYSTEM))
	return RC_CNUHR;
    else
    {
	c=rxu_tostring(i, (char *) &cbuf, &n);
	o.type=CINTTYPE;
	o.val.intvalue=fgw_spawn(c, fgw_childzap);
	pstate->oserr.sqlcode=errno;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** run
*/
int rxf_run(fgw_stacktype *s)
{
    int n;
    char *c, cbuf[STRINGCONVSIZE];
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    if (!(pstate->runneroptions & RT_CANRUNSLEEP))
	return RC_CNUHR;
    else
    {
	c=rxu_tostring(i, (char *) &cbuf, &n);
	o.type=CINTTYPE;
	o.val.intvalue=fgw_spawn(c, fgw_childzap);
	if (o.val.intvalue!=-1 &&
	    (o.val.intvalue=rxc_dowaitpid(o.val.intvalue)!=-1))
		o.val.intvalue=pstate->childstatus;
	pstate->oserr.sqlcode=errno;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** fopen
*/
int rxf_fopen(fgw_stacktype *s)
{
    exprstack_t *i, o;
    fgw_fdesc *fd_p;
    char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    memset(&o, 0, sizeof(o));
    if (!(pstate->runneroptions & RT_CANSYSTEM))
	return RC_CNUHR;
    i=rxu_arglist(s, NULL);
    sh1=rxu_tostring(i, (char *) &sbuf1, NULL);
    sh2=rxu_tostring(i+1, (char *) &sbuf2, NULL);
    if (fd_p=fgw_fileopen(sh1, sh2))
    {
	fd_p->fd_entry.signature=pstate->signature | SS_INTERNAL;
	o.type=CINTTYPE;
	o.val.intvalue=fd_p->fd_entry.int_id;
    }
    else
    {
	rxu_setnull(&o, CINTTYPE);
	pstate->oserr.sqlcode=errno;
	status=0;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** popen
*/
int rxf_popen(fgw_stacktype *s)
{
    exprstack_t *i, o;
    fgw_fdesc *fd_p;
    char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    memset(&o, 0, sizeof(o));
    if (!(pstate->runneroptions & RT_CANSYSTEM))
	return RC_CNUHR;
    i=rxu_arglist(s, NULL);
    sh1=rxu_tostring(i, (char *) &sbuf1, NULL);
    sh2=rxu_tostring(i+1, (char *) &sbuf2, NULL);
    if (fd_p=fgw_pipeopen(sh1, sh2))
    {
	fd_p->fd_entry.signature=pstate->signature | SS_INTERNAL;
	o.type=CINTTYPE;
	o.val.intvalue=fd_p->fd_entry.int_id;
    }
    else
    {
	rxu_setnull(&o, CINTTYPE);
	pstate->oserr.sqlcode=errno;
	status=0;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** sclose
*/
int rxf_sclose(fgw_stacktype *s)
{
    exprstack_t *i, o;
    fgw_fdesc *fd_p;

    memset(&o, 0, sizeof(o));
    if (!(pstate->runneroptions & RT_CANSYSTEM))
	return RC_CNUHR;
    i=rxu_arglist(s, NULL);
    if (fd_p=fgw_findfd(rxu_toint(i, NULL), pstate->signature | SS_INTERNAL))
	pstate->childstatus=fgw_fdclose(fd_p);
    o.type=CINTTYPE;
    o.val.intvalue=status? -1: 0;
    pstate->oserr.sqlcode=status;
    status=0;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** getenv
*/
int rxf_getenv(fgw_stacktype *s)
{
    int n, l;
    char *e, ebuf[STRINGCONVSIZE], *v;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    l=0;
    i=rxu_arglist(s, NULL);
    e=rxu_tostring(i, (char *) &ebuf, &n);
    if (!n && (v=getenv(e)))
	l=strlen(v);
    if (!(o.val.string=fgw_smalloc(l)))
	return RC_NOMEM;
    else
    {
	o.type=CSTRINGTYPE;
	if (n)
	    rxu_setnull(&o, CSTRINGTYPE);
	else
	{
	    if (l)
		fgw_move(o.val.string, v, l);
	    *(o.val.string+l)='\0';
	}
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** eval
*/
int rxf_eval(fgw_stacktype *s)
{
#ifdef PUREPARSER
    exprstack_t *i, o;
    int n, l, rc;
    char *e, ebuf[STRINGCONVSIZE];
    parserstate_t state;
    fgw_loc_t txtvar;

    memset(&o, 0, sizeof(o));
    l=0;
    i=rxu_arglist(s, NULL);
    e=rxu_tostring(i, (char *) &ebuf, &n);
    if (n)
	rxu_setnull(&o, CSTRINGTYPE);
    else
    {
	o.type=CSTRINGTYPE;

	/* set up parser state from current one */
	memset(&state, 0, sizeof(parserstate_t));
	state.sighlr=fgw_sigint((void(*)(int)) rqx_sendbreak);

	/* so that statements and file descriptos can be recognised */
	state.signature=pstate->signature;
	state.vars=pstate->vars;
	state.i_query=e;
	state.i_size=strlen(e);
	fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
	fgw_stackinit(&state.control, sizeof(controlstack_t));
	sql_parserinit(&state);
	state.width=pstate->width;
	state.states=pstate->states|PS_ISREENTRANT;
	state.flags=pstate->flags & ~(PF_QUIT|PF_VERBOSE);
	state.expstate=pstate->expstate;
	state.touch=pstate->touch;
	memcpy(&state.errorlog, pstate->errorlog, sizeof(state.errorlog));
	state.txtvar=&txtvar;
	fgw_locatetext(state.txtvar);
	state.def_fd=FD_TOBLOB;
	state.curr_fd=state.def_fd;
	rqf_openfile(state.txtvar, state.curr_fd);

	/* save yacc state */
	sql_parsersave(pstate);

	/* off we go */
	rc=sql_execcmd(&state);

	/* restore, get result and free */
	sql_parserrestore(pstate);
	rqf_openfile(pstate->txtvar, pstate->curr_fd);

	memcpy(&pstate->ca, &state.ca, sizeof(state.ca));
	memcpy(&pstate->oserr, &state.oserr, sizeof(state.oserr));
	if (!rc && (l=txtvar.loc_size))
	{
	    if (!(o.val.string=fgw_smalloc(l)))
		rc=RC_NOMEM;
	    else
	    {
		fgw_move(o.val.string, txtvar.loc_buffer, l);
		for (; l; l--)
		    if (*(o.val.string+l-1)!='\n')
			break;
		*(o.val.string+l)='\0';
	    }
	}
	else
	    rxu_setnull(&o, CSTRINGTYPE);
	fgw_stackfree(&state.control, NULL);
	fgw_stackfree(&state.exprlist, NULL);
	fgw_tssdrop(&state.exphead);
	fgw_freetext(state.txtvar);
	sql_parserfree(&state);
    }
    rxu_pushresult(s, &o);
    return rc;
#else
    return RC_NLNKD;
#endif
}

/*
** index
*/
int rxf_index(fgw_stacktype *s)
{
    exprstack_t *i, o;
    int n1, n2;
    char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);

    sh1=rxu_tostring(i, (char *) &sbuf1, &n1);
    sh2=rxu_tostring(i+1, (char *) &sbuf2, &n2);
    if (n1||n2)
	rxu_setnull(&o, CINTTYPE);
    else
    {
	o.type=CINTTYPE;
	o.val.intvalue=fgw_pos(sh1, sh2);
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** match
*/
int rxf_match(fgw_stacktype *s)
{
    exprstack_t *i, o;
    int n1, n2=0, r;
    char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];
    int patternopt=0;
#ifdef HAVE_REGEX
    regex_t p, *pp;
    regmatch_t m;
#endif

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    sh1=rxu_tostring(i, (char *) &sbuf1, &n1);
/*
** if we can, we will only compile the pattern once
** this requires that:
**  1) we haven't done it already,
**  2) the expression gets execute more than once, and
**  3) the pattern is a string
*/
    if (!pstate->instrp->constant.val.string)
	sh2=rxu_tostring(i+1, (char *) &sbuf2, &n2);
#ifdef HAVE_REGEX
    if (n1||n2)
	rxu_setnull(&o, CINTTYPE);
    else
    {
	o.type=CINTTYPE;
	if (!(pp=(regex_t *) pstate->instrp->constant.val.string))
	{
	    patternopt=((pstate->runneroptions & RT_CONSTOPT) &&
                    ((i+1)->flags & EF_CONSTANT));
	    if (regcomp(&p, sh2, REG_EXTENDED))
		return RC_RESYN;
	    if (patternopt &&
		(pstate->instrp->constant.val.string=fgw_tssmallocx(
				&pstate->pcode.parsehead,
				sizeof(p), regfree)))
		fgw_move(pstate->instrp->constant.val.string, &p, sizeof(p));
            pp=&p;
	}
	if (regexec(&p, sh1, 1, &m, 0) || m.rm_so<0)
	    rxu_setnull(&o, CINTTYPE);
	if (!pstate->instrp->constant.val.string)
	    regfree(&p);
	o.val.intvalue=m.rm_so+1;
    }
    rxu_pushresult(s, &o);
    return 0;
#else
    return RC_NLNKD;
#endif
}

/*
** pad
*/
int rxf_pad(fgw_stacktype *s)
{
    int sp, n1, n2;
    char *ps, cbuf[STRINGCONVSIZE];
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    ps=rxu_tostring(i++, (char *) &cbuf, &n2);
    sp=rxu_toint(i, &n1);
    if (n1||n2)
	sp=1;
    else if (sp<=0)
	return RC_BFINV;
    else if (!(o.val.string=(char *) fgw_smalloc(sp)))
	return RC_NOMEM;
    if (n1||n2)
	rxu_setnull(&o, CSTRINGTYPE);
    else
    {
	o.type=CSTRINGTYPE;
	rxf_dopadding(sp, ps, o.val.string);
	*(o.val.string+sp)='\0';
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** random
*/
int rxf_random(fgw_stacktype *s)
{
    int newseed, top, p, n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, &p);
    o.type=CINTTYPE;
    switch (p)
    {
      case 2:
	newseed=rxu_toint(i+1, &n);
	if (n)
	    return RC_BFINV;	
	else
	    fgw_srand(newseed);
      case 1:
	top=rxu_toint(i, &n);
	if (n)
	    rxu_setnull(&o, CINTTYPE);
	else
	    o.val.intvalue=fgw_rand() % top;
	break;
      default:
	return RC_WACNT;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** current
*/
int rxf_current(fgw_stacktype *s)
{
    int q;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    if ((q=i->val.intvalue)<0)
	q=TU_DTENCODE(TU_YEAR, TU_SECOND);
    o.type=CDTIMETYPE;
    if (!(o.val.datetime=(fgw_dtime_t *) fgw_smalloc(sizeof(fgw_dtime_t))))
    {
	rxu_setnull(&o, o.type);
	return RC_NOMEM;
    }
    rxu_current(&o, q);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** today
*/
int rxf_today(fgw_stacktype *s)
{
    exprstack_t o;

    rxu_today(&o);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** user
*/
int rxf_user(fgw_stacktype *s)
{
    exprstack_t o;

    memset(&o, 0, sizeof(o));
    o.type=CSTRINGTYPE;
    if (o.val.string=fgw_smalloc(USERNAMELEN+1))
    {
	*o.val.string='\0';
	fgw_username(o.val.string, USERNAMELEN);
	if (!*o.val.string)
	    rxu_setnull(&o, CSTRINGTYPE);
	else
	    *(o.val.string+USERNAMELEN)='\0';
    }
    else
	return RC_NOMEM;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** decode
*/
int rxf_decode(fgw_stacktype *s)
{
    exprstack_t *i, *w, o;
    int p, c, nullind;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, &p);
    rxu_setnull(&o, CINTTYPE);
    w=i+1;
    c=p-1;
    while (c)
    {
	int r;

	if (c>1)
	{
	    r=rxu_compare(i->type, i, w, &nullind);
	    w++;
	    c--;
	    if (r)
	    {
		w++;
		c--;
		continue;
	    }
	}
	o=*w;
	rxu_setint(w, 0, nullind);	/* hack: avoid freeing strings */
	break;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** nvl
*/
int rxf_nvl(fgw_stacktype *s)
{
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    if (rxu_isnull(i))
        i++;
    o=*i;
    rxu_setint(i, 0, 0);		/* hack: avoid freeing strings */
    rxu_pushresult(s, &o);
    return 0;
}

/*
** abs
*/
int rxf_abs(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=fabs(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** mod
*/
int rxf_mod(fgw_stacktype *s)
{
    exprstack_t *i, o;
    int ih1, ih2;
    int n1, n2;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    ih1=rxu_toint(i, &n1);
    ih2=rxu_toint(i+1, &n2);
    o.type=CINTTYPE;
    if (n1||n2)
	rxu_setnull(&o, CINTTYPE);
    else if (ih2==0)
	return RC_BFINV;
    else
	o.val.intvalue=ih1-(ih1/ih2)*ih2;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** pow
*/
int rxf_pow(fgw_stacktype *s)
{
    exprstack_t *i, o;
    double fh1, fh2;
    int n1, n2;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    fh1=rxu_todouble(i, &n1);
    fh2=rxu_todouble(i+1, &n2);
    o.type=CDOUBLETYPE;
    if (n1||n2)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=pow(fh1, fh2);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** root
*/
int rxf_root(fgw_stacktype *s)
{
    exprstack_t *i, o;
    double fh1, fh2;
    int n1, n2;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    fh1=rxu_todouble(i, &n1);
    fh2=rxu_todouble(i+1, &n2);
    o.type=CDOUBLETYPE;
    if (n1||n2)
	rxu_setnull(&o, CDOUBLETYPE);
    else if (fh2==0.0)
	return RC_BFINV;
    else
	o.val.real=pow(fh1, 1.0/fh2);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** does actual rounding
*/
static int fgw_round(fgw_stacktype *s, double r)
{
    double f;
    int k, n1, n2, p;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
/*
** check args & convert
*/
    i=rxu_arglist(s, &p);
    f=rxu_todouble(i, &n1);
    if (p>2)
	return RC_WACNT;
    else
    {
	if (p==2)
	    k=rxu_toint(i+1, &n2);
	else
	{
	    k=0;
	    n2=0;
	}
	o.type=CDOUBLETYPE;
	if (n1||n2)
	    rxu_setnull(&o, CDOUBLETYPE);
	else
	{
	    double d;

	    d=pow(10.0, -k);
	    if (f>0.0)
		o.val.real=floor((f/d)+r)*d;
	    else
		o.val.real=floor((f/d)-r)*d;
	}
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** round
*/
int rxf_round(fgw_stacktype *s)
{
    return fgw_round(s, 0.5);
}

/*
** sqrt
*/
int rxf_sqrt(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=sqrt(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** trunc
*/
int rxf_trunc(fgw_stacktype *s)
{
    return fgw_round(s, 0.0);
}

/*
** dbinfo
*/
int rxf_dbinfo(fgw_stacktype *s)
{
    int n;
    char *c, cbuf[STRINGCONVSIZE];
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    o.type=CINTTYPE;
    if (n)
	return RC_IDBIF;
    else if (!fgw_strcasecmp(c, "sqlca.sqlerrd1"))
	o.val.intvalue=pstate->ca.sqlerrd1;
    else if (!fgw_strcasecmp(c, "sqlca.sqlerrd2"))
	o.val.intvalue=pstate->ca.sqlerrd2;
    else if (!fgw_strcasecmp(c, "sqlca.sqlcode"))
	o.val.intvalue=SQ_TOSQLCODE(pstate->ca.sqlcode);
    else if (!fgw_strcasecmp(c, "errno"))
	o.val.intvalue=pstate->oserr.sqlcode;
    else if (!fgw_strcasecmp(c, "$?"))
	o.val.intvalue=pstate->childstatus;
    else
	return RC_IDBIF;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** sqlcode
*/
int rxf_sqlcode(fgw_stacktype *s)
{
    exprstack_t o;

    memset(&o, 0, sizeof(o));
    o.type=CINTTYPE;
    o.val.intvalue=SQ_TOSQLCODE(pstate->ca.sqlcode);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** sqlerrd1
*/
int rxf_sqlerrd1(fgw_stacktype *s)
{
    exprstack_t o;

    memset(&o, 0, sizeof(o));
    o.type=CINTTYPE;
    o.val.intvalue=pstate->ca.sqlerrd1;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** sqlerrd2
*/
int rxf_sqlerrd2(fgw_stacktype *s)
{
    exprstack_t o;

    memset(&o, 0, sizeof(o));
    o.type=CINTTYPE;
    o.val.intvalue=pstate->ca.sqlerrd2;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** errno
*/
int rxf_errno(fgw_stacktype *s)
{
    exprstack_t o;

    memset(&o, 0, sizeof(o));
    o.type=CINTTYPE;
    o.val.intvalue=pstate->oserr.sqlcode;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** exp
*/
int rxf_exp(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=exp(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** log10
*/
int rxf_log10(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else if (f<=0.0)
	return RC_BFINV;
    else
	o.val.real=log10(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** logn
*/
int rxf_logn(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else if (f<=0.0)
	return RC_BFINV;
    else
	o.val.real=log(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** hex
*/
int rxf_hex(fgw_stacktype *s)
{
    int k, n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    if (!(o.val.string=fgw_smalloc(11)))
	return RC_NOMEM;
    else
    {
	k=rxu_toint(i, &n);
	o.type=CSTRINGTYPE;
	if (n)
	    rxu_setnull(&o, CSTRINGTYPE);
	else
	    sprintf(o.val.string, "0x%x", k);
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** char_length
*/
int rxf_charlength(fgw_stacktype *s)
{
    char *c, cbuf[STRINGCONVSIZE];
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    o.type=CINTTYPE;
    if (n)
	o.val.intvalue=0;
    else
	o.val.intvalue=fgw_strlen(c);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** length
*/
int rxf_length(fgw_stacktype *s)
{
    char *c, *p, cbuf[STRINGCONVSIZE];
    int l, n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    o.type=CINTTYPE;
    if (n || !(l=strlen(c)))
	o.val.intvalue=0;
    else
    {
	p=c+l-1;
	while (l && *p--==' ')
	    l--;
	o.val.intvalue=l;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** datetime
*/
int rxf_datetime(fgw_stacktype *s)
{
    int n1, q, pc, r=0;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, &pc);
    pc--;
    if ((q=(i+pc)->val.intvalue)<0)
	q=TU_DTENCODE(TU_YEAR, TU_SECOND);
    o.type=CDTIMETYPE;
    if (!(o.val.datetime=(fgw_dtime_t *) fgw_smalloc(sizeof(fgw_dtime_t))))
	return RC_NOMEM;
#ifdef HAS_DTFMT
    if (pc==2)
    {
        char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

	o.val.datetime->dt_qual=q;
	sh1=rxu_tostring(i, (char *) &sbuf1, NULL);
	sh2=rxu_tostring(i+1, (char *) &sbuf1, NULL);
	r=rxu_cvfmtdtime(&o, sh1, sh2);
    }
    else if (pc>2)
	return RC_WACNT;
    else
#endif
    {
	IGNORE rxu_todtime(i, o.val.datetime, q, &n1);
	if (n1)
	    rxu_setnull(&o, CDTIMETYPE);
    }
    rxu_pushresult(s, &o);
    return r;
}

/*
** interval
*/
int rxf_interval(fgw_stacktype *s)
{
    int n1, q, pc, r=0;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, &pc);
    pc--;
    if ((q=(i+pc)->val.intvalue)<0)
	q=TU_IENCODE(5, TU_DAY, TU_F5);
    else
	q=TU_IENCODE(5, TU_START(q), TU_END(q));
    o.type=CINVTYPE;
    if (!(o.val.interval=(fgw_intrvl_t *) fgw_smalloc(sizeof(fgw_intrvl_t))))
	return RC_NOMEM;
#ifdef HAS_DTFMT
    if (pc==2)
    {
	char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

	o.val.interval->in_qual=q;
	sh1=rxu_tostring(i, (char *) &sbuf1, NULL);
	sh2=rxu_tostring(i+1, (char *) &sbuf2, NULL);
	r=rxu_cvfmtinv(&o, sh1, sh2)
    }
    else if (pc>2)
	return RC_WACNT;
    else
#endif
    {
	IGNORE rxu_toinv(i, o.val.interval, q, &n1);
	if (n1)
	    rxu_setnull(&o, CINVTYPE);
    }
    rxu_pushresult(s, &o);
    return r;
}

/*
** date
*/
int rxf_date(fgw_stacktype *s)
{
    int n1, p, r=0;
    exprstack_t *i, o;
    char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, &p);
    o.type=CDATETYPE;
    switch(p)
    {
      case 1:
	o.val.intvalue=rxu_todate(i, &n1);
	if (n1)
	    rxu_setnull(&o, CDATETYPE);
	break;
      case 2:
	sh1=rxu_tostring(i, (char *) &sbuf1, NULL);
	sh2=rxu_tostring(i+1, (char *) &sbuf2, NULL);
	r=rxu_cvfmtdate(&o, sh1, sh2);
	break;
      default:
	return RC_WACNT;
    }
    rxu_pushresult(s, &o);
    return r;
}

/*
** day
*/
int rxf_day(fgw_stacktype *s)
{
    short mdy[3];
    int n, r=0;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    rxu_mdy(i, mdy, &n);
    if (n)
	rxu_setnull(&o, CINTTYPE);
    else
    {
	o.type=CINTTYPE;
	o.val.intvalue=mdy[1];
    }
    rxu_pushresult(s, &o);
    return r;
}

/*
** month
*/
int rxf_month(fgw_stacktype *s)
{
    short mdy[3];
    int n, r=0;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    rxu_mdy(i, mdy, &n);
    if (n)
	rxu_setnull(&o, CINTTYPE);
    else
    {
	o.type=CINTTYPE;
	o.val.intvalue=mdy[0];
    }
    rxu_pushresult(s, &o);
    return r;
}

/*
** year
*/
int rxf_year(fgw_stacktype *s)
{
    short mdy[3];
    int n, r=0;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    rxu_mdy(i, mdy, &n);
    if (n)
	rxu_setnull(&o, CINTTYPE);
    else
    {
	o.type=CINTTYPE;
	o.val.intvalue=mdy[2];
    }
    rxu_pushresult(s, &o);
    return r;
}

/*
** weekday
*/
int rxf_weekday(fgw_stacktype *s)
{
    int d, n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    d=rxu_weekday(i, &n);
    if (n)
	rxu_setnull(&o, CINTTYPE);
    else
    {
	o.type=CINTTYPE;
	o.val.intvalue=d;
    }
    rxu_pushresult(s, &o);
    return 0;
}

/*
** mdy
*/
int rxf_mdy(fgw_stacktype *s)
{
    short mdy[3];
    int c, d, n, r=0;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    for (c=0; c<3; c++, i++)
    {
	d=rxu_toint(i, &n);
	if (n)
	{
	    rxu_setnull(&o, CDATETYPE);
	    break;
	}
	mdy[c]=d;
    }
    o.type=CDATETYPE;
    r=rxu_mdy2date(mdy, &o.val.intvalue);
    rxu_pushresult(s, &o);
    return r;
}

/*
** cos
*/
int rxf_cos(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=cos(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** sin
*/
int rxf_sin(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=sin(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** tan
*/
int rxf_tan(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=tan(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** acos
*/
int rxf_acos(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else
/* if not in [-1..1] will return nan on most systems */
	o.val.real=acos(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** asin
*/
int rxf_asin(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else

/* if not in [-1..1] will return nan on most systems */
	o.val.real=asin(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** atan
*/
int rxf_atan(fgw_stacktype *s)
{
    double f;
    int n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    f=rxu_todouble(i, &n);
    o.type=CDOUBLETYPE;
    if (n)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=atan(f);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** atan2
*/
int rxf_atan2(fgw_stacktype *s)
{
    exprstack_t *i, o;
    double fh1, fh2;
    int n1, n2;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    fh1=rxu_todouble(i, &n1);
    fh2=rxu_todouble(i+1, &n2);
    o.type=CDOUBLETYPE;
    if (n1||n2)
	rxu_setnull(&o, CDOUBLETYPE);
    else
	o.val.real=atan2(fh1, fh2);
    rxu_pushresult(s, &o);
    return 0;
}

/*
** substr
*/
int rxf_substr(fgw_stacktype *s)
{
    int l, n1, n2;
    int ih1, ih2;
    char *c, cbuf[STRINGCONVSIZE];
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    o.type=CSTRINGTYPE;
    c=rxu_tostring(i, (char *) &cbuf, &n1);
    ih1=rxu_toint(i+1, &n1);
    ih2=rxu_toint(i+2, &n2);
    if (n1||n2)
    {
	if (!(o.val.string=fgw_smalloc(0)))
	    return RC_NOMEM;
	rxu_setnull(&o, CSTRINGTYPE);
	goto end;
    }
/*
** can't have a negative length
*/
    if (ih2<1)
	return RC_BFINV;
/*
**  if start is negative, we count from the tail
*/
    l=strlen(c);
    if (ih1<0)
	ih1=l+ih1;
    else if (ih1)
	ih1--;
/*
** can't have a negative start
*/
    if (ih1<0)
	return RC_BFINV;
/*
** and avoid stupid segvs
*/
    if (ih1+ih2>l)
	ih2=l-ih1;
    if (!(o.val.string=fgw_smalloc(ih2+1)))
	return RC_NOMEM;
    if (ih1>l)
	*o.val.string='\0';
    else
    {
	fgw_move(o.val.string, c+ih1, ih2);
	*(o.val.string+ih2)='\0';
    }
end:
    rxu_pushresult(s, &o);
    return 0;
}

/*
** replace
*/
int rxf_replace(fgw_stacktype *s)
{
    int pc, n, ni, idx, st, l, fl, tl, ol, il;
    exprstack_t *p;
    char *ob;
    char *sb, sbuf[STRINGCONVSIZE];
    char *fb, fbuf[STRINGCONVSIZE];
    char *tb, tbuf[STRINGCONVSIZE];
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
/*
** sort out parms:
*/
    i=rxu_arglist(s, &pc);
    if (pc<2 || pc>5)
	return RC_WACNT;
/*
** original string
*/
    sb=rxu_tostring(i, (char *) &sbuf, &n);
    if (n)
	ol=0;
    else
	ol=strlen(sb);
/*
** from string
*/
    fb=rxu_tostring(i+1, (char *) &fbuf, &ni);
    if (ni)
	fl=0;
    else
	fl=strlen(fb);
    n|=ni;
/*
** optional to string
*/
    if (pc>2)
    {
	tb=rxu_tostring(i+2, (char *) &tbuf, &ni);
	if (ni)
	    tl=0;
	else
	    tl=strlen(tb);
/*	n|=ni; */	/* we allow removal of substrings */
    }
    else
    {
	tb=(char *) &tbuf;
	*tb='\0';
	tl=0;
    }
/*
** optional start
*/
    if (pc>3)
    {
	st=rxu_toint(i+3, &ni);
	n|=ni;
	if (st<0)
	   st=ol+st;
	else if (st)
	    st--;
    }
    else
	st=0;
/*
** optional length
*/
    if (pc>4)
    {
	il=rxu_toint(i+4, &ni);
	n|=ni;
    }
    else
	il=ol;
/*
** allocate memory
*/
    o.type=CSTRINGTYPE;
    if (!(o.val.string=fgw_smalloc(ol)))
	return RC_NOMEM;
/*
** & return null if any parm null
*/
    if (n)
	rxu_setnull(&o, CSTRINGTYPE);
/*
** wrong parms
*/
    else if (il<1 || st<0)
	return RC_BFINV;
/*
** empty from string, nothing to change
*/
    else if (!*fb)
	strcpy(o.val.string, sb);
    else
    {
	l=0;
	ob=o.val.string;
	if (st)
	{
	    fgw_move(ob, sb, st);
	    sb+=st;
	    ob+=st;
	    il-=st;
	}
/*
** scan input string
*/
	for (;;)
	{
/*
** search next instance of 'from' text
*/
	    if (!(idx=fgw_pos(sb, fb)) || (idx--)>il)
	    {
/*
** not there or beyond limit
*/
		strcpy(ob, sb);
		goto end;
	    }
	    if (l+il+tl-fl>ol)
/*
** output buffer doesn't fit, resize
*/
	    	if (ob=fgw_srealloc(o.val.string, l+il+tl-fl))
		{
		    o.val.string=ob;
		    ob+=l;
		}
		else
		    return RC_NOMEM;
/*
** copy and adjust pointers
*/
	    if (idx)
		fgw_move(ob, sb, idx);
	    ob+=idx;
	    if (tl)
		fgw_move(ob, tb, tl);
	    l+=tl+idx;
	    ob+=tl;
	    sb+=idx+fl;
	    il-=(idx+fl);
	}
    }
end:
    rxu_pushresult(s, &o);
    return 0;
}

/*
** common bit of lpad & rpad
*/
static int rxf_dorlpad(fgw_stacktype *s, exprstack_t *o, char **c, char *cbuf,
		       char **p, char *pbuf, int *l, int *il, int *pc)
{
    exprstack_t *i;
    int n1, n2, n3;

    memset(o, 0, sizeof(exprstack_t));
    i=rxu_arglist(s, pc);
    if (*pc!=2 && *pc!=3)
	return RC_WACNT;
    else
    {
	o->type=CSTRINGTYPE;
	*c=rxu_tostring(i, (char *) cbuf, &n1);
	*l=rxu_toint(i+1, &n2);
	if (*pc==3)
	    *p=rxu_tostring(i+2, (char *) pbuf, &n3);
	else
	{
	    *p=" ";
	    n3=0;
	}
	if (n2)
	   *l=1;
	if (!(o->val.string=fgw_smalloc(*l)))
	    return RC_NOMEM;
	else if (n1 || n2 || n3)
	    rxu_setnull(o, CSTRINGTYPE);
	else if (*l<=(*il=strlen(*c)))
	{
	    fgw_move(o->val.string, *c, *l);
	    *(o->val.string+*l)='\0';
	}
    }
    return 0;
}

/*
** lpad
*/
int rxf_lpad(fgw_stacktype *s)
{
    exprstack_t o;
    char *c, cbuf[STRINGCONVSIZE], *p, pbuf[STRINGCONVSIZE];
    int l, il, pc, r;

    memset(&o, 0, sizeof(o));
    r=rxf_dorlpad(s, &o, &c, (char *) &cbuf, &p, (char *) &pbuf, &l , &il, &pc);
    if (l>il && !r)
    {
	rxf_dopadding(l-il, p, o.val.string);
	fgw_move(o.val.string+l-il, c, il);
	*(o.val.string+l)='\0';
    }
    rxu_pushresult(s, &o);
    return r;
}

/*
** rpad
*/
int rxf_rpad(fgw_stacktype *s)
{
    exprstack_t o;
    char *c, cbuf[STRINGCONVSIZE], *p, pbuf[STRINGCONVSIZE];
    int l, il, pc, r;

    r=rxf_dorlpad(s, &o, &c, (char *) &cbuf, &p, (char *) &pbuf, &l , &il, &pc);
    if (l>il && !r)
    {
	fgw_move(o.val.string, c, il);
	rxf_dopadding(l-il, p, o.val.string+il);
	*(o.val.string+l)='\0';
    }
    rxu_pushresult(s, &o);
    return r;
}

/*
** upper
*/
int rxf_upper(fgw_stacktype *s)
{
    int n;
    char *c, cbuf[STRINGCONVSIZE];
    char *ip, *op;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    if (o.val.string=fgw_smalloc(fgw_strlen(c)))
    {
	o.type=CSTRINGTYPE;
	if (n)
	    rxu_setnull(&o, CSTRINGTYPE);
	else
	    for (ip=c, op=o.val.string; *ip; fgw_chrnext(ip), fgw_chrnext(op))
		*op=fgw_toupper(*ip);
    }
    else
	return RC_NOMEM;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** lower
*/
int rxf_lower(fgw_stacktype *s)
{
    int n;
    char *c, cbuf[STRINGCONVSIZE];
    char *ip, *op;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    if (o.val.string=fgw_smalloc(fgw_strlen(c)))
    {
	o.type=CSTRINGTYPE;
	if (n)
	    rxu_setnull(&o, CSTRINGTYPE);
	else
	    for (ip=c, op=o.val.string; *ip; fgw_chrnext(ip), fgw_chrnext(op))
		*op=fgw_tolower(*ip);
    }
    else
	return RC_NOMEM;
    rxu_pushresult(s, &o);
    return 0;
}

/*
** initcap
*/
int rxf_initcap(fgw_stacktype *s)
{
    char *c, cbuf[STRINGCONVSIZE];
    char *ip, *op;
    int notaletter, n;
    exprstack_t *i, o;

    memset(&o, 0, sizeof(o));
    i=rxu_arglist(s, NULL);
    c=rxu_tostring(i, (char *) &cbuf, &n);
    if (n)
	o.val.intvalue=0;
    else if (!(o.val.string=fgw_smalloc(strlen(c))))
	return RC_NOMEM;
    else
    {
	o.type=CSTRINGTYPE;
	ip=c;
	op=o.val.string;
	notaletter=1;
	for (ip=c, op=o.val.string; *ip; fgw_chrnext(ip), fgw_chrnext(op))
	    if (fgw_islower(*ip) && notaletter)
	    {
		*op=fgw_toupper(*ip);
		notaletter=0;
	    }
	    else
	    {
		notaletter=!fgw_isalpha(*ip);
		*op=*ip;
	    }
    }
    rxu_pushresult(s, &o);
    return 0;
}
