/*
	CRXOC.c  -  Expression stack runtime execution: operators

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: May 00
	Current release: Aug 16

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
#ifdef HAVE_REGEX
#include <regex.h>
#endif
#include "coslc.h"
#include "ccmnc.h"
#include "chstc.h"
#include "cldpc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqxp.h"
#include "crxuc.h"
#include "crxvc.h"
#include "crxoc.h"

extern TGLOB parserstate_t *pstate;

/*
** aggregate boolean tests: break clause
*/
int rxo_aggrbreak(fgw_stacktype *s)
{
    if (pstate->instrp->flags & RT_HASREJECT)
	rqx_rejdump(pstate->curstmt);
    return RC_NOTFOUND;
}

/*
** aggregate boolean tests: continue clause
*/
int rxo_aggrcont(fgw_stacktype *s)
{
    if (pstate->instrp->flags & RT_HASREJECT)
	rqx_rejdump(pstate->curstmt);
    return RC_CONTINUE;
}

/*
** aggregate clause: retrieve nth column
*/
static int rxo_colget(int c, exprstack_t *o)
{
    exprstack_t *e;

    if (!(e=rqx_nexttoken(c, pstate->curstmt)))
	return 1;
    fgw_move(o, e, sizeof(exprstack_t));
    switch (e->type)
    {
      case CDECIMALTYPE:
      case CMONEYTYPE:
      case CDTIMETYPE:
      case CINVTYPE:
      case CSTRINGTYPE:
	o->flags|=EF_NOTONTSS;
    }
    return 0;
}

/*
** aggregate clause: push nth column
*/
int rxo_colvalue(fgw_stacktype *s)
{
    exprstack_t *e, o;
    int f, nullind;

    memset(&o, 0, sizeof(o));
/*
** shouldn't happen, but just in case
*/
    if (!pstate->curstmt)
	return RC_PSNTX;
    e=(exprstack_t *) fgw_stackpop(s);
    e->count=0;				/* just to be sure */
    f=rxu_toint(e, &nullind);
    if (nullind || f<1 || rxo_colget(f-1, &o))
	rxu_setnull(&o, CINTTYPE);
    fgw_stackpush(s, &o);
    return 0;
}

/*
** aggregate clause: column id (assignment)
*/
int rxo_column(fgw_stacktype *s)
{
    exprstack_t *e, o;
    int f, nullind;

    memset(&o, 0, sizeof(o));
/*
** shouldn't happen, but just in case
*/
    if (!pstate->curstmt)
	return RC_PSNTX;
    e=(exprstack_t *) fgw_stackpop(s);
    e->count=0;				/* just to be sure */
    f=rxu_toint(e, &nullind);
    if (nullind || f<1 || f>rqx_counttokens(pstate->curstmt))
    {
	rxu_setnull(&o, CINTTYPE);
	rxu_pushresult(s, &o);
	return RC_WRCNO;
    }
    o.type=FGWCOLTYPE;
    o.val.intvalue=f-1;
    fgw_stackpush(s, &o);
    return 0;
}

/*
** aggregate clause: column assignment
*/
static int rxo_colassign(int c, exprstack_t *e)
{
    exprstack_t *d;
    int s;

/*
** shouldn't happen, but just in case
*/
    if (!pstate->curstmt)
	return RC_PSNTX;
    if (!pstate->curstmt->columns)
	if (pstate->curstmt->columns=
		malloc(s=sizeof(exprstack_t)*rqx_counttokens(pstate->curstmt)))
	    memset(pstate->curstmt->columns, 0, s);
	else
	    return RC_NOMEM;
    d=(pstate->curstmt->columns)+c;
    fgw_move(d, e, sizeof(exprstack_t));
    d->flags|=(EF_NOTONTSS|EF_SUBSTITUTE);
    if (!(e->flags & EF_NOTONTSS) && (e->type!=CINTTYPE) &&
	(e->type!=CDATETYPE) && (e->type!=CDOUBLETYPE))
    {
	fgw_tssdetach(&pstate->exphead, e->val.string);
	fgw_tssattach(&pstate->curstmt->columnhead, e->val.string);
	e->flags|=EF_NOTONTSS;
    }
    return 0;
}

/*
** push constant
*/
int rxo_constant(fgw_stacktype *s)
{
    fgw_stackpush(s, &(pstate->instrp->constant));
    if (pstate->instrp->constant.type==CSTRINGTYPE)
    {
	exprstack_t *e;

	e=(exprstack_t *) fgw_stackpeek(s);
	e->flags|=EF_NOTONTSS;
    }
    return 0;
}

/*
** push variable contents
*/
int rxo_variable(fgw_stacktype *s)
{
    exprstack_t *e, *t, *x, v, vs;
    int rs=-1, re, n1, n2;
    int l;
    int rc;
    hst_t hid;
    char *c, sbuf[STRINGCONVSIZE];

    memset(&v, 0, sizeof(v));
    memset(&vs, 0, sizeof(vs));
    if (pstate->instrp->flags & RT_HASSUB1)
    {
	t=(exprstack_t *) fgw_stackpop(s);
	re=rxu_toint(t, &n1);
	if (re<=0 || re>0x7fff || n1)
	    return RC_ISUBS;
	re--;
	rs=re;
    }
    else if (pstate->instrp->flags & RT_HASSUB2)
    {
	t=(exprstack_t *) fgw_stackpop(s);
	re=rxu_toint(t, &n1);
	t=(exprstack_t *) fgw_stackpop(s);
	rs=rxu_toint(t, &n2);
	if (re<=0 || rs<=0 || rs>re || re>0x7fff || n1 || n2)
	    return RC_ISUBS;
	rs--;
	re--;
    }
    if (!pstate->instrp->constant.count)
	hid=pstate->instrp->constant.val.identifier;
    else if (rc=sqv_evalhash(s, &hid))
	return rc;

    if (rc=sqv_cvvar(&v, hid, (pstate->instrp->flags & RT_SAFECOPY)))
	return rc;
    if (rs<0)
    {
	fgw_stackpush(s, &v);
	return 0;
    }
    c=rxu_tostring(&v, (char *) &sbuf, &n1);
    vs.type=CSTRINGTYPE;
    if (!(vs.val.string=fgw_smalloc(l=re-rs+1)))
	return(RC_NOMEM);
    if (rs>strlen(c) || n1)
	rxu_setnull(&vs, CSTRINGTYPE);
    else
    {
	vs.length=l;
	fgw_move(vs.val.string, c+rs, l);
	*(vs.val.string+l)='\0';
    }
    fgw_stackpush(s, &vs);
    return 0;
}

/*
** push variable contents (short hand assignment)
*/
int rxo_variable2(fgw_stacktype *s)
{
    exprstack_t *t, v;
    int rc;
    hst_t hid;

    t=(exprstack_t *) fgw_stackpeek(s);
    memset(&v, 0, sizeof(v));
    if (t->type==FGWCOLTYPE)
    {
	if (rxo_colget(t->val.intvalue, &v))
	    rxu_setnull(&v, CINTTYPE);
    }
    else
    {
	hid=t->val.identifier;
	if (rc=sqv_cvvar(&v, hid, (pstate->instrp->flags & RT_SAFECOPY)))
	    return rc;
    }
    fgw_stackpush(s, &v);
    return 0;
}

/*
** push hash contents
*/
int rxo_dohashstar(fgw_stacktype *s)
{
    exprstack_t o, *v;
    fgw_hstentry *e, *h;
    fgw_hstnode *n;
    hst_t eid, hid;
    int l, rc;

    memset(&o, 0, sizeof(o));
    if (!pstate->instrp->constant.count)
	hid=pstate->instrp->constant.val.identifier;
    else if (rc=sqv_evalhash(s, &hid))
	return rc;

/*
** nullify if symbol undefined
** FIXME return error instead?
*/
    if (!hid || !(h=fgw_hstidxget(pstate->vars, hid)))
    {
	rxu_setnull(&o, CINTTYPE);
	o.count=1;
	fgw_stackpush(s, &o);
	return 0;
    }
    else if (h->type!=T_HASH)
	return RC_INSCL;
/*
** FIXME should this be in chstc.c?
** FIXME there should never be a hash with no elements, but just in case?
*/
    o.count=1;
    n=(fgw_hstnode *) &h->entry;
    for (l=0; l<n->buckets; l++)
    {
	if (!(eid=n->bucket[l]))
	    continue;
	while (eid)
	{
	    e=fgw_hashentry(pstate->vars, eid);
	    if (e->type==T_HASH)
		return RC_INHSH;
	    if (rc=sqv_cvvar(&o, eid, (pstate->instrp->flags & RT_SAFECOPY)))
		return rc;
	    fgw_stackpush(s, &o);
	    o.count++;
	    eid=e->next;
	}
    }
    return rc;
}

/*
** push partial hash contents
*/
int rxo_varlist(fgw_stacktype *s)
{
    exprstack_t *e, *v;
    hst_t hid, tid;
    fgw_hstentry *he;
    int i, ec, rc;
    char *n, buff[STRINGCONVSIZE];

    e=(exprstack_t *) fgw_stackpeek(s);
    if (ec=e->count)
	e-=ec;
    else
    {
	ec++;
	e--;
    }
    if (!(hid=e->val.identifier))
	for (v=e+1, i=1; i<=ec; i++, e++, v++)
	{
	    rxu_freetssstack(v);
	    rxu_setnull(e, CINTTYPE);
	    e->count=i;
	    e->flags&=~EF_CONSTANT;
	}
    else
    {
	he=fgw_hstidxget(pstate->vars, hid);
	if (!he || status)
	    return status;
	if (he->type!=T_HASH)
	    return RC_INSCL;
	for (v=e+1, i=1; i<=ec; i++, e++, v++)
	{
	    e->flags&=~EF_CONSTANT;
	    n=rxu_tostring(v, (char *) &buff, NULL);
	    he=fgw_hstreserve(pstate->vars, hid, n, 0);
	    if (status)
		return status;
	    rxu_freetssstack(v);
	    if (!he)
		rxu_setnull(e, CINTTYPE);
	    else if (rc=sqv_cvvar(e, he->magic,
				 (pstate->instrp->flags & RT_SAFECOPY)))
		return rc;
	    e->count=i;
	}
    }
    fgw_stackdrop(s, 1);
    return 0;
}

/*
** split pattern delimited
*/
int rxo_delsplit(fgw_stacktype *s)
{
    char *i, *d, *e,
	 sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE],
	 sbuf3[STRINGCONVSIZE],
	 *fi, *fd, *fe,
	 *p;
    exprstack_t *a, r;
    int n1, n2, n3, l, rc=0;

    a=rxu_arglist(s, NULL);
    i=rxu_tostring(a, (char *) &sbuf1, &n1);
    fi=rxu_tsssave(a++);
    d=rxu_tostring(a, (char *) &sbuf2, &n2);
    fd=rxu_tsssave(a++);
    e=rxu_tostring(a, (char *) &sbuf3, &n3);
    fe=rxu_tsssave(a++);
    memset(&r, 0, sizeof(r));
    if (n1||n2)
    {
	rxu_setnull(&r, CSTRINGTYPE);
	rxu_pushresult(s, &r);
	goto end;
    }
    else
    {
	if (n3)
	   e="\\";
	r.type=CSTRINGTYPE;
	r.count=1;
	for (p=i, l=0; !rc; p++)
	{
	    if (*p==*e)
	    {
		p++;
		if (*p==*d || *p==*e)
		{
		   l++;
		   continue;
		}
	    }
	    if (!*p)
	    {
		if (!(r.val.string=fgw_smalloc(l+1)))
		{
		    rxu_setnull(&r, CSTRINGTYPE);
		    rc=RC_NOMEM;
		}
		else
		    fgw_escmove(r.val.string, i, l+1, *e); 
		rxu_pushresult(s, &r);
		break;
	    }
	    l++;
	    if (*p!=*d)
		continue;
	    if (!(r.val.string=fgw_smalloc(l)))
	    {
		rxu_setnull(&r, CSTRINGTYPE);
		rc=RC_NOMEM;
	    }
	    else
	    {
		fgw_escmove(r.val.string, i, l-1, *e);
		*(r.val.string+l-1)='\0';
	    }
	    rxu_pushresult(s, &r);
	    i=p+1;
	    r.count++;
	    l=0;
	}
    }

end:
    rxu_tssfree(fi);
    rxu_tssfree(fd);
    rxu_tssfree(fe);
    return rc;
}

/*
** split pattern
*/
int rxo_patsplit(fgw_stacktype *s)
{
    char *i, *p,
	 sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE],
	 *fi;
    exprstack_t *a, r;
    int patternopt;
    int n1, n2=0, l, t, rc=0;
    int cf;
    char *pp;

    a=rxu_arglist(s, NULL);
    i=rxu_tostring(a, (char *) &sbuf1, &n1);
    fi=rxu_tsssave(a++);
/*
** if we can we will only compile the pattern once
** this requires that 1) we haven't done it already, 2) the expression
** gets execute more than once and 3) the pattern is a string
*/
    if (!pstate->instrp->constant.val.string)
    {
	patternopt=((pstate->runneroptions & RT_CONSTOPT) &&
		    (a->flags & EF_CONSTANT));
	p=rxu_tostring(a, (char *) &sbuf2, &n2);
    }
    memset(&r, 0, sizeof(r));
    if (n1||n2)
    {
	rxu_setnull(&r, CSTRINGTYPE);
	rxu_pushresult(s, &r);
	goto end;
    }
    else
    {
	if (!(pp=pstate->instrp->constant.val.string))
	{
	    if (ldp_patterncomp(p, &pstate->pcode.parsehead, &pp))
	    {
		rxu_setnull(&r, CSTRINGTYPE);
		rc=RC_RESYN;
		goto end;
	    }
	    if (patternopt)
		pstate->instrp->constant.val.string=pp;
	}

	r.count=1;
	for (cf=0;!rc;)
	{
	    if (!(rc=ldp_patternexec(&pp, i, &cf, &r, &pstate->exphead)))
		rxu_pushresult(s, &r);
	    r.count++;
	}

	if (!pstate->instrp->constant.val.string)
	    ldp_patternfree(pp);
    }

end:
    if (rc==RC_NOTFOUND)
	rc=0;
    rxu_tssfree(fi);
    return rc;
}

/*
** split pattern regexp delimited
*/
int rxo_regdelsplit(fgw_stacktype *s)
{
    char *i, *d=NULL,
	 sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE],
	 *fi;
    exprstack_t *a, r;
    int n1, n2=0, l, t, rc=0;
    int patternopt=0;
#ifdef HAVE_REGEX
    regex_t p, *pp;
    regmatch_t m;
#endif

    a=rxu_arglist(s, NULL);
    i=rxu_tostring(a, (char *) &sbuf1, &n1);
    fi=rxu_tsssave(a++);
/*
** if we can we will only compile the pattern once
** this requires that 1) we haven't done it already, 2) the expression
** gets execute more than once and 3) the pattern is a string
*/
    if (!pstate->instrp->constant.val.string)
    {
	patternopt=((pstate->runneroptions & RT_CONSTOPT) &&
		    (a->flags & EF_CONSTANT));
	d=rxu_tostring(a, (char *) &sbuf2, &n2);
    }
    memset(&r, 0, sizeof(r));
    if (n1||n2)
    {
	rxu_setnull(&r, CSTRINGTYPE);
	rxu_pushresult(s, &r);
	goto end;
    }
    else
    {
#ifdef HAVE_REGEX
	if (!(pp=(regex_t *) pstate->instrp->constant.val.string))
	{
	    if (regcomp(&p, d, REG_EXTENDED))
	    {
		rxu_setnull(&r, CSTRINGTYPE);
		rxu_pushresult(s, &r);
		rc=RC_RESYN;
		goto end;
	    }
	    if (patternopt &&
		(pstate->instrp->constant.val.string=fgw_tssmallocx(
				&pstate->pcode.parsehead,
				sizeof(p), regfree)))
		fgw_move(pstate->instrp->constant.val.string, &p, sizeof(p));
	    pp=&p;
	}
	r.type=CSTRINGTYPE;
	r.count=1;
	for (;!rc;)
	{
	    t=regexec(pp, i, 1, &m, 0);
	    if (t)
	    {
		if (!(r.val.string=fgw_smalloc(l=strlen(i)+1)))
		{
		    rxu_setnull(&r, CSTRINGTYPE);
		    rc=RC_NOMEM;
		}
		else
		    fgw_move(r.val.string, i, l); 
		rxu_pushresult(s, &r);
		break;
	    }
	    if (!(r.val.string=fgw_smalloc(m.rm_so+1)))
	    {
		rxu_setnull(&r, CSTRINGTYPE);
		rc=RC_NOMEM;
	    }
	    else
	    {
		fgw_move(r.val.string, i, m.rm_so);
		*(r.val.string+m.rm_so)='\0';
	    }
	    rxu_pushresult(s, &r);
	    i+=m.rm_eo;
	    r.count++;
	}
	if (!pstate->instrp->constant.val.string)
	    regfree(&p);
#else
	rxu_setnull(&r, CSTRINGTYPE);
	rxu_pushresult(s, &r);
	rc=RC_NLNKD;
#endif
    }

end:
    rxu_tssfree(fi);
    return rc;
}

/*
** split extractor
*/
int rxo_extsplit(fgw_stacktype *s)
{
    char *fi;
    exprstack_t *a, r;
    int n1, rc=0;
    int cf;

    a=rxu_arglist(s, NULL);
    fi=rxu_tsssave(a++);
    memset(&r, 0, sizeof(r));
    if (n1)
    {
	rxu_setnull(&r, CSTRINGTYPE);
	rxu_pushresult(s, &r);
	goto end;
    }
    else if (a->type!= FGWBYTETYPE)
    {
	rxu_setnull(&r, CSTRINGTYPE);
	rxu_pushresult(s, &r);
	rc=RC_BFINV;
	goto end;
    }
    else
    {
	r.count=1;
	for (cf=0; !rc;)
	{
	    if (!(rc=lde_decode(fi, &cf, &r, &pstate->exphead)))
		rxu_pushresult(s, &r);
	    r.count++;
	}
    }

end:
    if (rc==RC_NOTFOUND)
	rc=0;
    rxu_tssfree(fi);
    return rc;
}

/*
** TO list clause
*/
int rxo_to(fgw_stacktype *s)
{
    exprstack_t *f, *t, r;
    int fi, ti, fn, tn, d;

    t=(exprstack_t *) fgw_stackpop(s);
    f=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    fi=rxu_toint(f, &fn);
    ti=rxu_toint(t, &tn);
    rxu_freetssstack(f);
    rxu_freetssstack(t);
    if (fn || tn)
    {
        rxu_setnull(&r, CINTTYPE);
	fgw_stackpush(s, &r);
    }
    else
    {
	d=(fi<ti)? 1: -1;
	r.type=CINTTYPE;
	r.count=1;
	r.val.intvalue=fi;
	for (;;)
	{
	    fgw_stackpush(s, &r);
	    if (status!=0)
		return status;
	    if (r.val.intvalue==ti)
		break;
	    r.val.intvalue+=d;
	    r.count++;
	}
    }
    return 0;
}

/*
** evaluate identifier
*/
int rxo_identifier(fgw_stacktype *s)
{
    exprstack_t r;
    int rc=0;

    memset(&r, 0, sizeof(r));
    r.type=FGWIDTYPE;
    if (pstate->instrp->constant.totype)
    {
	r.totype=pstate->instrp->constant.totype;
	r.qual=pstate->instrp->constant.qual;
    }
    rc=sqv_evalhash(s, &r.val.identifier);
    if (pstate->instrp->constant.flags & EF_EXPANDHASH)
	r.flags=EF_EXPANDHASH;
    fgw_stackpush(s, &r);
    return rc;
}

/*
** evaluate partial hash list
*/
int rxo_identlist(fgw_stacktype *s)
{
    exprstack_t *e, *v;
    fgw_hstentry *he;
    int i, ec, rc;
    char *n, buff[STRINGCONVSIZE];
    hst_t hid;

    e=(exprstack_t *) fgw_stackpeek(s);
    if (ec=e->count)
	e-=ec;
    else
    {
	ec++;
	e--;
    }
    hid=e->val.identifier;
    for (v=e+1, i=1; i<=ec; i++, e++, v++)
    {
	n=rxu_tostring(v, (char *) &buff, NULL);
	if (!(he=fgw_hstreserve(pstate->vars, hid, n, 32)))
	     return status;
	rxu_freetssstack(v);
	e->flags&=~EF_CONSTANT;
	e->type=FGWIDTYPE;
	e->val.identifier=he->magic;
	e->count=i;
    }
    fgw_stackdrop(s, 1);
    return 0;
}

/*
** empty argument list marker
*/
int rxo_emptylist(fgw_stacktype *s)
{
    exprstack_t e;

    if (pstate->instrp->constant.val.intvalue!=0)
	return RC_WACNT;
    memset(&e, 0, sizeof(e));
    e.type=EMPTYMARKER;
    fgw_stackpush(s, &e);
    return 0;
}

/*
** set list length, check for function argument count
*/
int rxo_listcount(fgw_stacktype *s)
{
    exprstack_t *o, *e;
    int i, c;

    o=(exprstack_t *) fgw_stackpeek(s);
    if (!o)
	return RC_WACNT;
    c=pstate->instrp->constant.count;
/*
** keep track of expanded hashes on list
*/
    if (pstate->instrp->constant.flags & EF_VARIABLELIST)
	for (i=c, e=o; i; i--, e--)
	    if (e->count)
	    {
		c+=e->count-1;
		e-=(e->count-1);
	    }
/*
** mark return values as checked
*/
    o->flags&=~EF_RETSTACK;
    o->count=c;
    if (pstate->instrp->constant.flags & EF_VARIABLEDEST)
	o->flags|=EF_VARIABLEDEST;
    if (pstate->instrp->constant.val.intvalue &&
	pstate->instrp->constant.val.intvalue!=c)
	return RC_WACNT;
    return 0;
}

/*
** check return list from function
*/
int rxo_checkfunc(fgw_stacktype *s)
{
    exprstack_t *o;

    o=(exprstack_t *) fgw_stackpeek(s);

    if (!(o->flags & EF_RETSTACK) || o->count!=1)
	return RC_WRCNT;
    o->count=0;
    o->flags&=~EF_RETSTACK;
    return 0;
}

/*
** assignment
*/
int rxo_assign(fgw_stacktype *s)
{
    exprstack_t cv, *t, *e, *sub;
    int rs=-1, re, n1, n2;
    int vl, el, ol, sp;
    int rc;
    char *tb, *eb, ebuf[STRINGCONVSIZE];
    char *v, vbuf[STRINGCONVSIZE];

    e=(exprstack_t *) fgw_stackpop(s);
    if (pstate->instrp->flags & RT_HASSUB1)
    {
	t=(exprstack_t *) fgw_stackpop(s);
	re=rxu_toint(t, &n1);
	if (re<=0 || re>0x7fff || n1)
	    return RC_ISUBS;
	re--;
	rs=re;
    }
    else if (pstate->instrp->flags & RT_HASSUB2)
    {
	t=(exprstack_t *) fgw_stackpop(s);
	re=rxu_toint(t, &n1);
	t=(exprstack_t *) fgw_stackpop(s);
	rs=rxu_toint(t, &n2);
	if (re<=0 || rs<=0 || rs>re || re>0x7fff || n1 || n2)
	    return RC_ISUBS;
	rs--;
	re--;
    }
    t=(exprstack_t *) fgw_stackpop(s);
    if (rs<0)
    {
	if (t->type==FGWCOLTYPE)
	    return rxo_colassign(t->val.intvalue, e);
	if (t->type!=FGWIDTYPE)
	    return RC_INHST;
	sqv_setvar(pstate->vars, e, t);
	rxu_freetssstack(e);
	return 0;
    }
    if (t->type==FGWCOLTYPE)
	(void) rxo_colget(t->val.intvalue, &cv);
    else if (t->type!=FGWIDTYPE)
	return RC_INHST;

/* FIXME: this could be optimized */
    else if (rc=sqv_cvvar(&cv, t->val.identifier, 0))
	return rc;
    v=rxu_tostring(&cv, (char *) &vbuf, &n1);
    vl=strlen(v);
    if (!(tb=fgw_smalloc(ol=rs-re+1)))
	return RC_NOMEM;
    strcpy(tb, v);
    eb=rxu_tostring(e, (char *) &ebuf, &n2);
    if ((el=strlen(eb))>ol)
	el=ol;
    fgw_move(tb+rs, eb, el);
/*
** fill with spaces as appropriate
*/
    if (rs>vl)
	memset(tb+vl, ' ', rs-vl);
    if ((sp=rs+el)<=re)
	memset(tb+sp, ' ', re-sp+1);
/*
** zero terminate if appropriate
*/
    if (re>=vl)
        *(tb+re+1)='\0';
    rxu_freetssstack(e);
    if (t->type==FGWCOLTYPE)
    {
	exprstack_t v;

	v.type=CSTRINGTYPE;
	v.val.string=tb;
	return rxo_colassign(t->val.intvalue, &v);
    }
    fgw_hstidxadd(pstate->vars, t->val.identifier, CSTRINGTYPE, 0, tb);
    fgw_sfree(tb);
    return 0;
}

/*
** multiple assignment
*/
int rxo_assign2(fgw_stacktype *s)
{
    exprstack_t *t, *e;
    int i, tc, dc, ec, idx=1;
    char buf[10], *bp=(char *) &buf;
    fgw_hstentry *he;
    int rc=0;

    t=(exprstack_t *) fgw_stackpeek(s);
    dc=tc=t->count;
    if (tc)
	t-=(tc-1);
    e=t-1;
    if ((ec=e->count)>1)
	e-=(ec-1);
    if (ec==tc || ((t->flags & EF_VARIABLEDEST) && tc<ec))
    {
	for (i=1; i<=ec; i++, e++)
	{
	    if (t->flags & EF_EXPANDHASH)
	    {
		sprintf(bp, "%d", idx);
		if (!(he=fgw_hstidxget(pstate->vars,
		        t->val.identifier)))
		    return RC_NOVAR;
		if (he->type!=T_HASH)
		    return RC_INSCL;
	        sqv_newvar(pstate->vars, e, bp, t);
		if (dc<ec)
		{
		    dc++;
		    idx++;
		}
		else
		{
		    t++;
		    idx=1;
		}
	    }
	    else
	        sqv_setvar(pstate->vars, e, t++);
	    if (rc=status)
		break;
	    rxu_freetssstack(e);
	}
	fgw_stackdrop(s, tc? tc: 1);
	fgw_stackdrop(s, ec? ec: 1);
	return rc;
    }
    else
    {
	for (i=ec; i; i--, e--);
	    rxu_freetssstack(e);
	fgw_stackdrop(s, tc? tc: 1);
	fgw_stackdrop(s, ec? ec: 1);
	return (pstate->instrp->flags & RT_DITCHRES)?
		   (tc==0? 0: RC_WRCNT): RC_WRASG;
    }
}

/*
** row storage
*/
int rxo_into(fgw_stacktype *s)
{
    exprstack_t *t, *e;
    int i, tc, dc, ec, idx=1;
    char buf[10], *bp=(char *) &buf;
    fgw_hstentry *he;
    int rc=0;

    t=(exprstack_t *) fgw_stackpeek(s);
    dc=tc=t->count;
    if (tc>1)
	t-=(tc-1);
    ec=rqx_counttokens(pstate->curstmt);
    if (ec==tc || ((t->flags & EF_VARIABLEDEST) && tc<ec))
    {
	for (i=1; i<=ec; i++)
	{
	    e=rqx_nexttoken(i-1, pstate->curstmt);
	    if (t->flags & EF_EXPANDHASH)
	    {
		sprintf(bp, "%d", idx);
		if (!(he=fgw_hstidxget(pstate->vars,
		        t->val.identifier)))
		    return RC_NOVAR;
		if (he->type!=T_HASH)
		    return RC_INSCL;
	        sqv_newvar(pstate->vars, e, bp, t);
		if (dc<ec)
		{
		    dc++;
		    idx++;
		}
		else
		{
		    t++;
		    idx=1;
		}
	    }
	    else
	        sqv_setvar(pstate->vars, e, t++);
	    if (rc=status)
		break;
	}
	fgw_stackdrop(s, tc? tc: 1);
	return rc;
    }
    else
    {
	fgw_stackdrop(s, tc? tc: 1);
	return RC_WRASG;
    }
}

/*
** count hash
*/
int rxo_counthash(fgw_stacktype *s)
{
    int rc=0;
    exprstack_t r;
    fgw_hstentry *e;
    hst_t hid;

    memset(&r, 0, sizeof(r));
    r.count=1;
    r.type=CINTTYPE;
    if (!pstate->instrp->constant.count)
	hid=pstate->instrp->constant.val.identifier;
    else if (rc=sqv_evalhash(s, &hid))
	return rc;
    if (!hid ||
	!(e=fgw_hstidxget(pstate->vars, hid)) ||
	e->type!=T_HASH)
	rxu_setnull(&r, CINTTYPE);
    else
	r.val.intvalue=((fgw_hstnode *) &(e->entry))->entries;
    rxu_pushresult(s, &r);
    return rc;
}

/*
** count star
*/
int rxo_countstar(fgw_stacktype *s)
{
    int rc=0;
    exprstack_t r;

    memset(&r, 0, sizeof(r));
    r.type=CINTTYPE;
    r.val.intvalue=rqx_counttokens(pstate->stmtstack[0].stmt);
    rxu_pushresult(s, &r);
    return rc;
}

/*
** when operator (boolean expression)
*/
int rxo_whenbool(fgw_stacktype *s)
{
    exprstack_t *e;
    int t, nullind;

    e=(exprstack_t *) fgw_stackpop(s);
    t=rxu_toboolean(e, &nullind);
    rxu_freetssstack(e);
    if  (nullind || !t)
	return rxo_jump(s);
    return 0;
}

/*
** when operator (expression comparison)
*/
int rxo_whenexpr(fgw_stacktype *s)
{
    int t, nullind;
    exprstack_t *o1, *o2;

    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpeek(s);
    t=rxu_compare(rxu_exprtype(o1->type, o2->type), o1, o2, &nullind);
    rxu_freetssstack(o2);
    if  (nullind || t)
	return rxo_jump(s);
    rxu_freetssstack(o1);
    fgw_stackdrop(s, 1);
    return 0;
}

/*
** else operator (expression comparison): cleanup expression from stack
*/
int rxo_elseexpr(fgw_stacktype *s)
{
    exprstack_t *e;

    e=(exprstack_t *) fgw_stackpop(s);
    rxu_freetssstack(e);
    return 0;
}

/*
** unconditional jump
*/
int rxo_jump(fgw_stacktype *s)
{
    pstate->instrp=(pstate->instrp->skip_to)-1;
    return 0;
}

/*
** unconditional jump
*/
int rxo_pause(fgw_stacktype *s)
{
    return RC_PAUSE;
}

/*
** concatenation
*/
int rxo_dpipe(fgw_stacktype *s)
{
    exprstack_t *o1, *o2, r;
    int rc=0;
    char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    memset(&r, 0, sizeof(r));
    rxu_setnull(&r, CINTTYPE);
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    sh1=rxu_tostring(o1, (char *) &sbuf1, NULL);
    sh2=rxu_tostring(o2, (char *) &sbuf2, NULL);
    r.val.string=(char *) fgw_smalloc(strlen(sh1)+strlen(sh2)+1);
    if (!(r.val.string))
    	rc=RC_NOMEM;
    else
    {
	r.type=CSTRINGTYPE;
	r.length=0;
	strcpy(r.val.string, sh1);
	strcat(r.val.string, sh2);
    }
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** picture
*/
int rxo_picture(fgw_stacktype *s)
{
    char *f;
    int rc=0;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    r.type=CSTRINGTYPE;
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    if (rxu_isnull(o1) || rxu_isnull(o2))
	rxu_setnull(&r, CSTRINGTYPE);
    else if (!(f=fgw_smalloc(64)))
    {
	rxu_setnull(&r, CSTRINGTYPE);
	rc=RC_NOMEM;
    }
    else if (o2->type==CSTRINGTYPE)
    {
	r.val.string=rxu_topictstring(o1, f, 64, o2->val.string, 0, 0);
	rc=status;
    }

    /* this is a bit weird, but if the picture is not a string,
       the only logical result is the picture itself
     */
    else
	r.val.string=rxu_tostring(o2, f, NULL);
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** addition
*/
int rxo_add(fgw_stacktype *s)
{
    exprstack_t *l, *r;
    exprstack_t *o1, *o2, d;
    int ih1, ih2;
    double fh1, fh2;
    fgw_intrvl_t in1, in2;
    fgw_dec_t *dh1, *dh2, db1, db2;
    int rc=0, n1, n2;

    memset(&d, 0, sizeof(d));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    switch(d.type=rxu_optype(o1, o2, &l, &r))
    {
      case CDTIMETYPE:
	rxu_dtaddinv(l, r, &d);
	break;
      case CDATETYPE:
	if (rxu_isnull(l)||rxu_isnull(r))
	    rxu_setnull(&d, CDATETYPE);
	else switch (r->type)
	{
	  case CINTTYPE:
	  case CDOUBLETYPE:
	    d.type=CDATETYPE;
	    d.val.intvalue=l->val.intvalue+rxu_toint(r, NULL);
	    break;
	  default:
	    rxu_dtaddinv(l, r, &d);
	}
	break;
      case CINVTYPE:
	if (rxu_inaddsub(o1, o2, &in1, &in2, &d) &&
	    (rc=rxu_inadd(&in1, &in2, d.val.interval)))
	    rxu_setnull(&d, CINVTYPE);
	break;
      case CDECIMALTYPE:
      case CMONEYTYPE:
	if (d.val.decimal=(fgw_dec_t *) fgw_smalloc(sizeof(fgw_dec_t)))
	{
	    dh1=rxu_todecimal(o1, &db1, &n1);
	    dh2=rxu_todecimal(o2, &db2, &n2);
	}
	else
	    rc=RC_NOMEM;
	if (rc||n1||n2||(rc=rxu_decadd(dh1, dh2, d.val.decimal)))
	    rxu_setnull(&d, d.type);
	break;
      case CDOUBLETYPE:
      case CSTRINGTYPE:
	fh1=rxu_todouble(o1, &n1);
	fh2=rxu_todouble(o2, &n2);
	rxu_setdouble(&d, fh1+fh2, n1||n2);
	break;
      case CINTTYPE:
	ih1=rxu_toint(o1, &n1);
	ih2=rxu_toint(o2, &n2);
	rxu_setint(&d, ih1+ih2, (n1||n2));
	break;
      default:
	rxu_setnull(&d, CINTTYPE);
    }
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &d);
    return rc;
}

/*
** subtraction
*/
int rxo_sub(fgw_stacktype *s)
{
    exprstack_t *o1, *o2, d;
    int ih1, ih2;
    double fh1, fh2;
    fgw_dec_t *dh1, *dh2, db1, db2;
    fgw_intrvl_t in1, in2;
    int dt, rc=0, n1, n2;

    memset(&d, 0, sizeof(d));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    if (o1->type==CDTIMETYPE)
    {
	if (o2->type==CINVTYPE)
	    rxu_dtsubinv(o1, o2, &d);
	else
	    rxu_dtsub(o1, o2, &d);
    }
    else if (o1->type==CDATETYPE)
    {
	if (rxu_isnull(o1)||rxu_isnull(o2))
	    rxu_setnull(&d, CDATETYPE);
	else switch (o2->type)
	{
	  case CINTTYPE:
	  case CDOUBLETYPE:
	    d.type=CDATETYPE;
	    d.val.intvalue=o1->val.intvalue-rxu_toint(o2, NULL);
	    break;
	  case CINVTYPE:
	    rxu_dtsubinv(o1, o2, &d);
	    break;
	  case CDTIMETYPE:
	    rxu_dtsub(o1, o2, &d);
	    break;
	  default:
	    dt=rxu_todate(o2, NULL);
	    d.type=CINTTYPE;
	    d.val.intvalue=o1->val.intvalue-dt;
	}
    }
    else if (o1->type==CINVTYPE)
    {
	if (rxu_inaddsub(o1, o2, &in1, &in2, &d) &&
	    (rc=rxu_insub(&in1, &in2, d.val.interval)))
	    rxu_setnull(&d, CINVTYPE);
    }
    else switch (d.type=rxu_exprtype(o1->type, o2->type))
    {
      case CDECIMALTYPE:
      case CMONEYTYPE:
	if (d.val.decimal=(fgw_dec_t *) fgw_smalloc(sizeof(fgw_dec_t)))
	{
	    dh1=rxu_todecimal(o1, &db1, &n1);
	    dh2=rxu_todecimal(o2, &db2, &n2);
	}
	else
	    rc=RC_NOMEM;
	if (rc||n1||n2||(rc=rxu_decsub(dh1, dh2, d.val.decimal)))
	    rxu_setnull(&d, d.type);
	break;
      case CSTRINGTYPE:
      case CDOUBLETYPE:
	fh1=rxu_todouble(o1, &n1);
	fh2=rxu_todouble(o2, &n2);
	rxu_setdouble(&d, fh1-fh2, n1||n2);
	break;
      case CINTTYPE:
	ih1=rxu_toint(o1, &n1);
	ih2=rxu_toint(o2, &n2);
	rxu_setint(&d, ih1-ih2, n1||n2);
	break;
      default:
	rxu_setnull(&d, CINTTYPE);
    }
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &d);
    return rc;
}

/*
** multiplication
*/
int rxo_mul(fgw_stacktype *s)
{
    exprstack_t *l, *r;
    exprstack_t *o1, *o2, d;
    int ih1, ih2;
    double fh1, fh2;
    fgw_intrvl_t *ip1, in1;
    fgw_dec_t *dh1, *dh2, db1, db2;
    int n1, n2, rc=0;
    double f;

    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    memset(&d, 0, sizeof(d));
    switch (d.type=rxu_optype(o1, o2, &l, &r))
    {
      case CDECIMALTYPE:
      case CMONEYTYPE:
	if (d.val.decimal=(fgw_dec_t *) fgw_smalloc(sizeof(fgw_dec_t)))
	{
	    dh1=rxu_todecimal(o1, &db1, &n1);
	    dh2=rxu_todecimal(o2, &db2, &n2);
	}
	else
	    rc=RC_NOMEM;
	if (rc||n1||n2||(rc=rxu_decmul(dh1, dh2, d.val.decimal)))
	    rxu_setnull(&d, d.type);
	break;
      case CDOUBLETYPE:
      case CSTRINGTYPE:
	fh1=rxu_todouble(o1, &n1);
	fh2=rxu_todouble(o2, &n2);
	rxu_setdouble(&d, fh1*fh2, n1||n2);
	break;
      case CINTTYPE:
	ih1=rxu_toint(o1, &n1);
	ih2=rxu_toint(o2, &n2);
	rxu_setint(&d, ih1*ih2, n1||n2);
	break;
      case CINVTYPE:
	ip1=rxu_toinv(l, &in1, -1, &n1);
	f=rxu_todouble(r, &n2);
	if (!(d.val.interval=(fgw_intrvl_t *) fgw_smalloc(sizeof(fgw_intrvl_t))))
	{
	    rxu_setnull(&d, CINVTYPE);
	    rc=RC_NOMEM;
	}
	else
	{
	    if (n1||n2)
		rxu_setnull(&d, CINVTYPE);
	    rc=rxu_invmuldbl(ip1, f, d.val.interval);
	}
	break;
      default:
	rxu_setnull(&d, CINTTYPE);
    }
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &d);
    return rc;
}

/*
** division
*/
int rxo_div(fgw_stacktype *s)
{
    exprstack_t *o1, *o2, d;
    int ih1, ih2;
    double fh1, fh2;
    fgw_dec_t *dh1, *dh2, db1, db2;
    int rc=0, n1, n2;

    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    memset(&d, 0, sizeof(d));
    if (o1->type==CINVTYPE)
    {
	int n1, n2;
	fgw_intrvl_t *ip1, *ip2, in2;
	double f;

	if (o2->type==CINTTYPE || o2->type==CDOUBLETYPE)
	{
	    ip1=rxu_toinv(o1, &in2, -1, &n1);
	    f=rxu_todouble(o2, &n2);
	    d.type=CINVTYPE;
	    if (!(d.val.interval=(fgw_intrvl_t *) fgw_smalloc(sizeof(fgw_intrvl_t))))
	    {
		rxu_setnull(&d, CINVTYPE);
		rc=RC_NOMEM;
	    }
	    else
	    {
		d.val.interval->in_qual=ip1->in_qual;
		if (n1||n2 || (rc=rxu_invdivdbl(ip1, f, d.val.interval)))
		    rxu_setnull(&d, CINVTYPE);
	    }
	}
	else
	{
	    ip1=rxu_toinv(o1, &in2, -1, &n1);
	    ip2=rxu_toinv(o2, &in2, -1, &n2);
	    d.type=CDOUBLETYPE;
	    if (n1||n2 || (rc=rxu_invdivinv(ip1, ip2, &(d.val.real))))
		rxu_setnull(&d, CDOUBLETYPE);
	}
    }
    else switch (d.type=rxu_exprtype(o1->type, o2->type))
    {
      case CDECIMALTYPE:
      case CMONEYTYPE:
	if (d.val.decimal=(fgw_dec_t *) fgw_smalloc(sizeof(fgw_dec_t)))
	{
	    dh1=rxu_todecimal(o1, &db1, &n1);
	    dh2=rxu_todecimal(o2, &db2, &n2);
	}
	else
	    rc=RC_NOMEM;
	if (rc||n1||n2||(rc=rxu_decdiv(dh1, dh2, d.val.decimal)))
	    rxu_setnull(&d, d.type);
	break;
      case CSTRINGTYPE:
      case CDOUBLETYPE:
	fh1=rxu_todouble(o1, &n1);
	fh2=rxu_todouble(o2, &n2);
/*
** avoid problems with null or 0 divisors...
*/
	if (n1||n2)
	    rxu_setnull(&d, CDOUBLETYPE);
	else if (fh2==0)
	    rc=RC_DZERO;
	else
	    rxu_setdouble(&d, fh1/fh2, 0);
	break;
      case CINTTYPE:
	ih1=rxu_toint(o1, &n1);
	ih2=rxu_toint(o2, &n2);
	if (n1||n2)
	    rxu_setnull(&d, CINTTYPE);
	else if (ih2==0)
	    rc=RC_DZERO;
	else
	    rxu_setint(&d, ih1/ih2, 0);
	break;
      default:
	rxu_setnull(&d, CINTTYPE);
    }
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &d);
    return rc;
}

/*
** units
*/
int rxo_units(fgw_stacktype *s)
{
    exprstack_t i, *o, r;
    int n, rc=0;

    memset(&r, 0, sizeof(r));
    rxu_setnull(&r, CINVTYPE);
    o=(exprstack_t *) fgw_stackpop(s);
/*
**  we complicate things a bit to have units behave as in the engine
**  rather than an interval shorthand
*/
    i.type=CINTTYPE;
    i.val.intvalue=rxu_toint(o, NULL);
    r.type=CINVTYPE;
    if (!(r.val.interval=(fgw_intrvl_t *) fgw_smalloc(sizeof(fgw_intrvl_t))))
    {
	rxu_setnull(&r, CINVTYPE);
	rc=RC_NOMEM;
    }
    else
    {
	rxu_toinv(&i, r.val.interval, pstate->instrp->constant.val.intvalue,
		  &n);
	if (n)
	    rxu_setnull(&r, CINVTYPE);
    }
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** casts: integer
*/
int rxo_tointeger(fgw_stacktype *s)
{
    exprstack_t *o, r;

    o=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    if (rxu_isnull(o))
        rxu_setnull(&r, CINTTYPE);
    else
	r.val.intvalue=rxu_toint(o, NULL);
    r.type=CINTTYPE;
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** casts: double
*/
int rxo_todouble(fgw_stacktype *s)
{
    exprstack_t *o, r;

    o=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    if (rxu_isnull(o))
        rxu_setnull(&r, CDOUBLETYPE);
    else
	r.val.real=rxu_todouble(o, NULL);
    r.type=CDOUBLETYPE;
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** casts: decimal, money
*/
int rxo_todecimal(fgw_stacktype *s)
{
    exprstack_t *o, r;
    fgw_dec_t *b;
    int rc=0;

    o=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    if (rxu_isnull(o))
        rxu_setnull(&r, pstate->instrp->constant.type);
    else if (!(b=(fgw_dec_t *) fgw_smalloc(sizeof(fgw_dec_t))))
	rc=RC_NOMEM;
    else
	r.val.decimal=rxu_todecimal(o, b, NULL);
    r.type=pstate->instrp->constant.type;
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** casts: date
*/
int rxo_todate(fgw_stacktype *s)
{
    exprstack_t *o, r;

    o=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    if (rxu_isnull(o))
        rxu_setnull(&r, CDATETYPE);
    else
	r.val.intvalue=rxu_todate(o, NULL);
    r.type=CDATETYPE;
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** casts: datetime
*/
int rxo_todatetime(fgw_stacktype *s)
{
    exprstack_t *o, r;
    fgw_dtime_t *b;
    int rc=0;

    o=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    if (rxu_isnull(o))
        rxu_setnull(&r, CDTIMETYPE);
    else if (!(b=(fgw_dtime_t *) fgw_smalloc(sizeof(fgw_dtime_t))))
	rc=RC_NOMEM;
    else
	r.val.datetime=rxu_todtime(o, b, pstate->instrp->constant.val.intvalue,
				   NULL);
    r.type=CDTIMETYPE;
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** casts: interval
*/
int rxo_tointerval(fgw_stacktype *s)
{
    exprstack_t *o, r;
    fgw_intrvl_t *b;
    int rc=0;

    o=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    if (rxu_isnull(o))
        rxu_setnull(&r, CINVTYPE);
    else if (!(b=(fgw_intrvl_t *) fgw_smalloc(sizeof(fgw_intrvl_t))))
	rc=RC_NOMEM;
    else
	r.val.interval=rxu_toinv(o, b, pstate->instrp->constant.val.intvalue,
				 NULL);
    r.type=CINVTYPE;
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** casts: string
*/
int rxo_tostring(fgw_stacktype *s)
{
    exprstack_t *o, r;
    char *b;
    int rc=0;

    o=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    if (rxu_isnull(o))
        rxu_setnull(&r, CSTRINGTYPE);
    else if (o->type==CSTRINGTYPE)
    {
	r.type=o->type;
	r.flags=o->flags;
	r.val.string=o->val.string;
	o->val.string=NULL;
    }
    else
    {
	r.type=CSTRINGTYPE;
	if (!(b=fgw_smalloc(STRINGCONVSIZE)))
	    rc=RC_NOMEM;
	else
	    r.val.string=rxu_tostring(o, b, NULL);
    }
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** casts: byte
*/
int rxo_tobyte(fgw_stacktype *s)
{
    exprstack_t *o, r;
    char *b;
    int rc=0;

    o=(exprstack_t *) fgw_stackpop(s);
    memset(&r, 0, sizeof(r));
    if (rxu_isnull(o))
        rxu_setnull(&r, FGWBYTETYPE);
    else if (o->type==FGWBYTETYPE || o->type==CSTRINGTYPE)
    {
	r.type=FGWBYTETYPE;
	r.flags=o->flags;
	r.val.string=o->val.string;
	r.length=(o->type==FGWBYTETYPE)? r.length=o->length: strlen(o->val.string);
	o->val.string=NULL;
    }
    else
    {
	r.type=FGWBYTETYPE;
	if (!(b=fgw_smalloc(STRINGCONVSIZE)))
	    rc=RC_NOMEM;
	else
	    r.val.string=rxu_tobyte(o, b, &r.length, NULL);
    }
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** unary plus
*/
int rxo_plus(fgw_stacktype *s)
{
    exprstack_t *o, r;
    int nullind;

    memset(&r, 0, sizeof(r));
    o=(exprstack_t *) fgw_stackpop(s);
    if (rxu_isnull(o))
	r=*o;
    else
	switch (o->type)
 	{
 	  case CSTRINGTYPE:
	    rxu_setdouble(&r, rxu_todouble(o, &nullind), nullind);
	    break;
 	  case CDECIMALTYPE:
 	  case CMONEYTYPE:
 	  case CDOUBLETYPE:
	  case CINTTYPE:
	  case CINVTYPE:
	    r=*o;
	    break;
/* FIXME datetime */
	  default:
	    rxu_setnull(&r, CINTTYPE);
	}
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** unary minus
*/
int rxo_minus(fgw_stacktype *s)
{
    int rc=0, nullind;
    fgw_dec_t dec1;
    exprstack_t *o, r;

    memset(&r, 0, sizeof(r));
    o=(exprstack_t *) fgw_stackpop(s);
    if (rxu_isnull(o))
	r=*o;
    else
	switch (o->type)
 	{
 	  case CSTRINGTYPE:
	    rxu_setdouble(&r, -rxu_todouble(o, &nullind), nullind);
	    break;
	  case CDECIMALTYPE:
	  case CMONEYTYPE:
	    rc=deccvlong(-1, &dec1);
	    r.type=o->type;
	    if (!(r.val.decimal=(fgw_dec_t *) fgw_smalloc(sizeof(fgw_dec_t))))
		rc=RC_NOMEM;
	    else
		rxu_decmul(o->val.decimal, &dec1, r.val.decimal);
	    break;
 	  case CDOUBLETYPE:
	    r.type=o->type;
	    r.val.real=-o->val.real;
	    break;
	  case CINTTYPE:
	    r.type=o->type;
	    r.val.intvalue=-o->val.intvalue;
	    break;
	  case CINVTYPE:
	    r.type=o->type;
	    if (!(r.val.interval=(fgw_intrvl_t *) fgw_smalloc(sizeof(fgw_intrvl_t))))
		rc=RC_NOMEM;
	    else
		rxu_invmuldbl(o->val.interval, -1.0, r.val.interval);
	    break;

/* FIXME: date, datetime */
	  default:
	    rxu_setnull(&r, r.type);
	}
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** in: member comparison
*/
int rxo_in(s)
fgw_stacktype *s;
{
    exprstack_t *m, *e, r;
    int t, nullind;

    memset(&r, 0, sizeof(r));
    m=(exprstack_t *) fgw_stackpop(s);
    e=(exprstack_t *) fgw_stackpeek(s);
    if (rxu_isnull(e))
	rxu_setnull(&r, CINTTYPE);
    else
    {
	t=rxu_compare(e->type, e, m, &nullind);
	if (!nullind && !t)
	{
	    r.type=CINTTYPE;
	    r.val.intvalue=1;
	}
	else 
	{
	    rxu_freetssstack(m);
	    return 0;
	}
    }
    rxu_freetssstack(m);
    rxu_freetssstack(e);
    fgw_stackdrop(s, 1);
    fgw_stackpush(s, &r);
    return rxo_jump(s);
}

/*
** in: last member comparison - test & cleanup
*/
int rxo_inend(fgw_stacktype *s)
{
    exprstack_t *m, *e, r;
    int t, nullind;

    memset(&r, 0, sizeof(r));
    m=(exprstack_t *) fgw_stackpop(s);
    e=(exprstack_t *) fgw_stackpop(s);
    if (rxu_isnull(e))
	rxu_setnull(&r, CINTTYPE);
    else
    {
	r.type=CINTTYPE;
	r.val.intvalue=0;

	t=rxu_compare(e->type, e, m, &nullind);
	if (!nullind && !t)
	    r.val.intvalue=1;
    }
    rxu_freetssstack(m);
    rxu_freetssstack(e);
    fgw_stackpush(&pstate->exprlist, &r);
    return 0;
}

/*
** not in: member comparison
*/
int rxo_ni(fgw_stacktype *s)
{
    exprstack_t *m, *e, r;
    int t, nullind;

    memset(&r, 0, sizeof(r));
    m=(exprstack_t *) fgw_stackpop(s);
    e=(exprstack_t *) fgw_stackpeek(s);
    if (rxu_isnull(e))
	rxu_setnull(&r, CINTTYPE);
    else
    {
	t=rxu_compare(e->type, e, m, &nullind);
/*
** a null in a NOT IN clause will yield NULL
** (NOT exp IN & exp NOT IN are semantically different. is this right?)
*/
	if (nullind)
	    rxu_setnull(&r, CINTTYPE);
	else if (!t)
	{
	    r.type=CINTTYPE;
	    r.val.intvalue=0;
	}
	else
	{
	    rxu_freetssstack(m);
	    return 0;
	}
    }
    rxu_freetssstack(m);
    rxu_freetssstack(e);
    fgw_stackdrop(s, 1);
    fgw_stackpush(s, &r);
    return rxo_jump(s);
}

/*
** not in: last member comparison - test & cleanup
*/
int rxo_niend(fgw_stacktype *s)
{
    exprstack_t *m, *e, r;
    int t, nullind;

    memset(&r, 0, sizeof(r));
    m=(exprstack_t *) fgw_stackpop(s);
    e=(exprstack_t *) fgw_stackpop(s);
    if (rxu_isnull(e))
	rxu_setnull(&r, CINTTYPE);
    else
    {
	r.type=CINTTYPE;
	r.val.intvalue=1;

	t=rxu_compare(e->type, e, m, &nullind);
/*
** a null in a NOT IN clause will yield NULL
** (NOT exp IN & exp NOT IN are semantically different. is this right?)
*/
	if (nullind)
	    rxu_setnull(&r, CINTTYPE);
	else if (!t)
	    r.val.intvalue=0;
    }
    rxu_freetssstack(m);
    rxu_freetssstack(e);
    fgw_stackpush(&pstate->exprlist, &r);
    return 0;
}

/*
** between
*/
int rxo_between(fgw_stacktype *s)
{
    int rl, rh, nullind;
    exprstack_t *e, *el, *eh, r;

    memset(&r, 0, sizeof(r));
    eh=(exprstack_t *) fgw_stackpop(s);
    el=(exprstack_t *) fgw_stackpop(s);
    e=(exprstack_t *) fgw_stackpop(s);
    if (rxu_isnull(e))
	rxu_setnull(&r, CINTTYPE);
    else
    {
/*
** el needs to be smaller than eh, but that's ok the engine
** behaves the same way
*/
	rl=rxu_compare(e->type, e, el, &nullind);
	if (nullind)
	    rxu_setnull(&r, CINTTYPE);
	else
	{
	    rh=rxu_compare(e->type, e, eh, &nullind);
	    if (nullind)
		rxu_setnull(&r, CINTTYPE);
	    else if (pstate->instrp->constant.type)
		rxu_setint(&r, rl>=0 && rh<=0, nullind);
	    else
		rxu_setint(&r, rl<0 || rh>0, nullind);
	}
    }
    rxu_freetssstack(e);
    rxu_freetssstack(el);
    rxu_freetssstack(eh);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** get int from pattern
*/
static int rxo_pattern2int(char *where)
{
    int i=sizeof(int), r=0;

    for (;i; i--, where++)
	r=(r<<8)+*where;
    return r;
}

typedef struct
{
    char *pattern;
    char *source;
} restart_t;

/*
** parsed like / matches executor
*/
static int rxo_execmatches(char *source, char *pattern, int *res)
{
    char *s=source, *s1;
    char *p=pattern, *p1;
    int pl, l;
    fgw_stacktype matchstack;
    restart_t push, *pop;

    *res=0;
    fgw_stackinit(&matchstack, sizeof(restart_t));
    for (;;)
    {
	switch(*p++)
	{

	  /* find this string */
	  case 'f':
	    l=rxo_pattern2int(p1=p);
	    p+=sizeof(int);
	    if (!(s=fgw_strstr(s1=s, p)))
		break;

	    /* make sure that we have the minimum length
	       stipulated by patterns of the sort of %__
	     */
	    if (s-s1<l)
	        break;

	    /* push restart point for matches all, in case
	       we fail to match further on
	     */
	    pl=fgw_strlen(p);
	    s+=pl;
	    push.pattern=p1-1;
	    push.source=s;
	    fgw_stackpush(&matchstack, &push);

	    /* now get on with the rest */
	    p+=pl+1;
	    continue;

	  /* %[: skip until any of the matching characters found */
	  case 's':
	    l=rxo_pattern2int(p1=p);
	    p+=sizeof(int);
	    s1=s;
	    while (*s && !fgw_strchr(p, s))
		s++;

	    /* make sure that we have the minimum length
	       stipulated by patterns of the sort of %__
	     */
	    if (!*s || s-s1<l)
	        break;

	    /* push restart point for matches all, in case
	       we fail to match further on
	     */
	    pl=fgw_strlen(p);
	    s++;
	    push.pattern=p1-1;
	    push.source=s;
	    fgw_stackpush(&matchstack, &push);

	    /* now get on with the rest */
	    p+=pl+1;
	    continue;

	  /* %[~: skip until none of the matching characters found */
	  case 'S':
	    l=rxo_pattern2int(p1=p);
	    p+=sizeof(int);
	    s1=s;
	    while (*s && fgw_strchr(p, s))
		s++;

	    /* make sure that we have the minimum length
	       stipulated by patterns of the sort of %__
	     */
	    if (!*s || s-s1<l)
	        break;

	    /* push restart point for matches all, in case
	       we fail to match further on
	     */
	    pl=fgw_strlen(p);
	    s++;
	    push.pattern=p1-1;
	    push.source=s;
	    fgw_stackpush(&matchstack, &push);

	    /* now get on with the rest */
	    p+=pl+1;
	    continue;

	  /* match one of these characters */
	  case 'm':
	    if (fgw_strchr(p, s))
		break;
	    fgw_chrnext(s);
	    p+=fgw_strlen(p)+1;
	    continue;


	  /* make sure that none of these characters is matched */
	  case 'M':
	    if (!fgw_strchr(p, s))
		break;
	    fgw_chrnext(s);
	    p+=fgw_strlen(p)+1;
	    continue;

	  /* match any character */
	  case 'a':
	    if (!*s)
		break;
	    fgw_chrnext(s);
	    p++;
	    continue;

	  /* match this string */
	  case 'c':
	    pl=fgw_strlen(p);
	    if (fgw_strncmp(s, p, pl))
		break;
	    s+=pl;
	    p+=pl+1;
	    continue;

	  /* ignore the reminder */
	  case 'i':
	    l=rxo_pattern2int(p);
	    *res=(fgw_strlen(s)>=l);
	    break;

	  /* end of pattern reached */
	  case 'e':
	    *res=(!*s);
	    break;
	}

	/* either we found it or we have nowhere else
	   from where to restart
	 */
	if (*res || (!(pop=(restart_t *) fgw_stackpop(&matchstack))))
	{
	    fgw_stackfree(&matchstack, NULL);
	    return 0;
	}

	/* it didn't work out, so resume from the last place
	   we found a match all
	 */
	s=pop->source;
	p=pop->pattern;
    }

    /* we never get here */
    return 0;
}

#define PATSTEP 	16
/*
** push actions in the parsed matches / like pattern
*/
static int rxo_addtopattern(char *what, char **which, char **where,
			    int *left, int *size)
{
    if (!*which || *left<=1)
    {
	char *b=realloc(*which, PATSTEP);

	if (!b)
	    return RC_NOMEM;
	else
	{
	    *which=b;
	    *where=b+*size-*left;
	    *left+=PATSTEP;
	    *size+=PATSTEP;
	}
    }
    **where=*what;	/* FIXME: GLS */
    (*where)++;
    **where='\0';
    *left--;
    return 0;
}

/*
** push integer in the parsed matches / like pattern
*/
static int inttopattern_sqt(int what, char **which, char **where,
			    int *left, int *size)
{
    int i=sizeof(int);
    int m=8*(i-1);

    if (!*which || *left<=i)
    {
	char *b=realloc(*which, PATSTEP);

	if (!b)
	    return RC_NOMEM;
	else
	{
	    *which=b;
	    *where=b+*size-*left;
	    *left+=PATSTEP;
	    *size+=PATSTEP;
	}
    }
    for (;i; (*where)++, i--, *left--, m-=8)
        **where=(what>>m) & 0xff;
    **where='\0';
    return 0;
}

/*
** common body for like / matches
*/
static int rxo_domatches(char *source, char *pattern, exprstack_t *escape,
			 char many, char one, int bracket, int patternopt,
			 int *res)
{
    char *e, esc[STRINGCONVSIZE];
    char *s=source;
    char *p=pattern;
    int state=0;

#define DM_FOUNDMANY	0x01
#define DM_FOUNDESC	0x02
#define DM_ADDTOSTRING	0x04

    int minlen=0;
    char *cp=NULL, *cpp=NULL;
    int cpl=0, cps=0;
    int rc;
    
    /* already optimized, get on with it */
    if (!pattern)
	return rxo_execmatches(source, pstate->instrp->constant.val.string,
		res);

    *res=0;
    e=rxu_tostring(escape, (char *) &esc, NULL);
    
    /* build optimized pattern... */
    for (;;)
    {
	if (*p==many)
	{
	    if (!(state & DM_FOUNDMANY))
		minlen=0;
	    if ((state & DM_ADDTOSTRING) &&
		    (rc=rxo_addtopattern("", &cp, &cpp, &cpl, &cps)))
		return rc;
	    state|=DM_FOUNDMANY;
	    state&=~DM_ADDTOSTRING;
	    p++;
	}
	else if (*p==one)
	{
	    if ((state & DM_ADDTOSTRING) &&
		(rc=rxo_addtopattern("", &cp, &cpp, &cpl, &cps)))
		return rc;

	    /* match any after match many is ignored and
	       only contributes to the minimum length to
	       match
	     */
	    if ((state & DM_FOUNDMANY))
		minlen++;
	    else
	    {
		if ((rc=rxo_addtopattern("a", &cp, &cpp, &cpl, &cps)) ||
		    (rc=rxo_addtopattern("", &cp, &cpp, &cpl, &cps)))
		    return rc;
		state&=~DM_ADDTOSTRING;
	    }
	    p++;
	}
	else if (*p=='[' && bracket)
	{
	    char *type=(state & DM_FOUNDMANY)? "s": "m";

	    if (*++p=='~')
	    {
		type=(state & DM_FOUNDMANY)? "S":"M";
		p++;
	    }
	    if ((state & DM_ADDTOSTRING) &&
		(rc=rxo_addtopattern("", &cp, &cpp, &cpl, &cps)))
		return rc;
	    if ((rc=rxo_addtopattern(type, &cp, &cpp, &cpl, &cps)))
		return rc;
	    if ((state & DM_FOUNDMANY) &&
		(rc=inttopattern_sqt(minlen, &cp, &cpp, &cpl, &cps)))
		return rc;
	    state&=~DM_FOUNDMANY;

	    /* FIXME: complete */
	    while (*p!=']')
		if ((rc=rxo_addtopattern(p++, &cp, &cpp, &cpl, &cps)))
		    return rc;
	    if ((rc=rxo_addtopattern("", &cp, &cpp, &cpl, &cps)))
		return rc;
	    p++;
	}
	else
	{

	    /* treat next char normally */
	    if (*p==*e &&

		/* dodgy: escape is last character - we'll take it to
		   mean match an escape
		 */
		 *(p+1))
		p++;

	    /* end of pattern */
	    if (!*p)
	    {
		if ((state & DM_ADDTOSTRING) &&
		    (rc=rxo_addtopattern("", &cp, &cpp, &cpl, &cps)))
		    return rc;
		if (state & DM_FOUNDMANY)
		{
		    if ((rc=rxo_addtopattern("i", &cp, &cpp, &cpl, &cps)) ||
		        (rc=inttopattern_sqt(minlen, &cp, &cpp, &cpl, &cps)))
			return rc;
		}
		else if (rc=rxo_addtopattern("e", &cp, &cpp, &cpl, &cps))
			return rc;
		break;
	    }
	    else
	    {
		if (state & DM_FOUNDMANY)
		{
		    if ((rc=rxo_addtopattern("f", &cp, &cpp, &cpl, &cps)) ||
		        (rc=inttopattern_sqt(minlen, &cp, &cpp, &cpl, &cps)))
			return rc;
		    state|=DM_ADDTOSTRING;
		}
		else if (!(state & DM_ADDTOSTRING))
		{
		    if (rc=rxo_addtopattern("c", &cp, &cpp, &cpl, &cps))
			return rc;
		    state|=DM_ADDTOSTRING;
		}

		if (rc=rxo_addtopattern(p, &cp, &cpp, &cpl, &cps))
		    return rc;
		fgw_chrnext(p);
	    }
	    state &=~DM_FOUNDMANY;
	}
    }

    /* ...execute it... */
    rc=rxo_execmatches(source, cp, res);

    /* ...and save it for reuse */
    if (patternopt)
    {
	if (!(pstate->instrp->constant.val.string=
		    fgw_tssmalloc(&pstate->pcode.parsehead, cps)))
	   return RC_NOMEM;
	fgw_move(pstate->instrp->constant.val.string, cp, cps);
    }
    else
	free(cp);
    return rc;
}

/*
** matches
*/
int rxo_matches(fgw_stacktype *s)
{
    int t, rc=0;
    char *e, esc[STRINGCONVSIZE];
    exprstack_t *o1, *o2, *o3, r;
    int n1, n2=0;
    char *sh1, *sh2=NULL;
    char sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    memset(&r, 0, sizeof(r));
    o3=(exprstack_t *) fgw_stackpop(s);
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    sh1=rxu_tostring(o1, (char *) &sbuf1, &n1);
/*
** if we can we will only compile the pattern once
** this requires that 1) we haven't done it already, 2) the expression
** gets execute more than once and 3) the pattern is a string
*/
    if (!pstate->instrp->constant.val.string)
	sh2=rxu_tostring(o2, (char *) &sbuf2, &n2);
    if (n1||n2)
	rxu_setnull(&r, CINTTYPE);
    else
    {
#ifdef I4GLVER
/* FIXME: should I be testing for a null escape here? */
 	e=rxu_tostring(o3, (char *) &esc, NULL);
	retquote(sh1);
	retquote(sh2);
/* FIXME: using 4gl internals */
	_domatches((char) *e);
	popint(&t);
#else
	rc=rxo_domatches(sh1, sh2, o3, '*', '?', 1,
			 ((pstate->runneroptions & RT_CONSTOPT) &&
	 	           (o2->flags & EF_CONSTANT)), &t);
#endif
	if (pstate->instrp->constant.type)
	    rxu_setint(&r, t, 0);
	else
	    rxu_setint(&r, !t, 0);
    }
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    rxu_freetssstack(o3);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** like
*/
int rxo_like(fgw_stacktype *s)
{
    int t, rc=0;
    char *e, esc[STRINGCONVSIZE];
    exprstack_t *o1, *o2, *o3, r;
    int n1, n2=0;
    char *sh1, *sh2=NULL;
    char sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];

    memset(&r, 0, sizeof(r));
    o3=(exprstack_t *) fgw_stackpop(s);
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);

    sh1=rxu_tostring(o1, (char *) &sbuf1, &n1);
/*
** if we can, we will only compile the pattern once.
** this requires that:
**  1) we haven't done it already,
**  2) the expression gets execute more than once, and
**  3) the pattern is a string
*/
    if (!pstate->instrp->constant.val.string)
	sh2=rxu_tostring(o2, (char *) &sbuf2, &n2);
    if (n1||n2)
	rxu_setnull(&r, CINTTYPE);
    else
    {
#ifdef I4GLVER
/* FIXME: should I be testing for a null escape here? */
 	e=rxu_tostring(o3, (char *) &esc, NULL);
	retquote(sh1);
	retquote(sh2);
/* FIXME: using 4gl internals */
	_dolike((char) *e);
	popint(&t);
#else
	rc=rxo_domatches(sh1, sh2, o3, '%', '_', 0,
			 ((pstate->runneroptions & RT_CONSTOPT) &&
	 	           (o2->flags & EF_CONSTANT)), &t);
#endif
	if (pstate->instrp->constant.type)
	    rxu_setint(&r, t, 0);
	else
	    rxu_setint(&r, !t, 0);
    }
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    rxu_freetssstack(o3);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** regexp like
*/
int rxo_regexp(fgw_stacktype *s)
{
    int t, rc=0;
    exprstack_t *o1, *o2, r;
    char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];
    int n1, n2=0;
    int patternopt=0;
#ifdef HAVE_REGEX
    regex_t p, *pp;
    regmatch_t m;
#endif

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    sh1=rxu_tostring(o1, (char *) &sbuf1, &n1);
/*
** if we can, we will only compile the pattern once.
** this requires that:
**  1) we haven't done it already,
**  2) the expression gets execute more than once, and
**  3) the pattern is a string
*/
    if (!pstate->instrp->constant.val.string)
	sh2=rxu_tostring(o2, (char *) &sbuf2, &n2);
#ifdef HAVE_REGEX
    if (n1||n2)
	rxu_setnull(&r, CINTTYPE);
    else
    {
        if (!(pp=(regex_t *) pstate->instrp->constant.val.string))
        {
	    patternopt=((pstate->runneroptions & RT_CONSTOPT) &&
                    (o2->flags & EF_CONSTANT));
	    if (regcomp(&p, sh2, REG_EXTENDED))
		return RC_RESYN;
	    if (patternopt &&
		(pstate->instrp->constant.val.string=fgw_tssmallocx(
				&pstate->pcode.parsehead,
				sizeof(p), regfree)))
		fgw_move(pstate->instrp->constant.val.string, &p, sizeof(p));
	    pp=&p;
	}
	t=(regexec(pp, sh1, 1, &m, 0)==0 && m.rm_so==0 && m.rm_eo==strlen(sh1));
	if (!pstate->instrp->constant.val.string)
	    regfree(&p);
	if (pstate->instrp->constant.type)
	    rxu_setint(&r, t, 0);
	else
	    rxu_setint(&r, !t, 0);
    }
#else
    rc=RC_NLNKD;
    rxu_setnull(&r, CINTTYPE);
#endif
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return rc;
}

/*
** or
*/
int rxo_or(fgw_stacktype *s)
{
    int b1, b2, n1, n2, t;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    b1=rxu_toboolean(o1, &n1);
    b2=rxu_toboolean(o2, &n2);
    t=b1 || b2;
    rxu_setint(&r, t, (n1 && n2) || ((n1 || n2) && !(b1 || b2)));
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** and
*/
int rxo_and(fgw_stacktype *s)
{
    int b1, b2, n1, n2;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    b1=rxu_toboolean(o1, &n1);
    b2=rxu_toboolean(o2, &n2);
    rxu_setint(&r, b1 && b2, n1||n2);
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** not
*/
int rxo_not(fgw_stacktype *s)
{
    int b, nullind;
    exprstack_t *o, r;

    memset(&r, 0, sizeof(r));
    o=(exprstack_t *) fgw_stackpop(s);
    b=rxu_toboolean(o, &nullind);
    rxu_setint(&r, !b, nullind);
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** is [not] null
*/
int rxo_isnull(fgw_stacktype *s)
{
    int t;
    exprstack_t *o, r;

    memset(&r, 0, sizeof(r));
    o=(exprstack_t *) fgw_stackpop(s);
    t=rxu_isnull(o);
    if (pstate->instrp->constant.val.intvalue)
	rxu_setint(&r, t, 0);
    else
	rxu_setint(&r, !t, 0);
    rxu_freetssstack(o);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** equal
*/
int rxo_eq(fgw_stacktype *s)
{
    int t, nullind;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    t=rxu_compare(rxu_exprtype(o1->type, o2->type), o1, o2, &nullind);
    rxu_setint(&r, !t, nullind);
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** not equal
*/
int rxo_ne(fgw_stacktype *s)
{
    int t, nullind;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    t=rxu_compare(rxu_exprtype(o1->type, o2->type), o1, o2, &nullind);
    rxu_setint(&r, (t!=0), nullind);
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** greater
*/
int rxo_gt(fgw_stacktype *s)
{
    int t, nullind;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    t=rxu_compare(rxu_exprtype(o1->type, o2->type), o1, o2, &nullind);
    rxu_setint(&r, (t>0), nullind);
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** greater or equal
*/
int rxo_ge(fgw_stacktype *s)
{
    int t, nullind;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    t=rxu_compare(rxu_exprtype(o1->type, o2->type), o1, o2, &nullind);
    rxu_setint(&r, (t>=0), nullind);
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** less than
*/
int rxo_lt(fgw_stacktype *s)
{
    int t, nullind;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    t=rxu_compare(rxu_exprtype(o1->type, o2->type), o1, o2, &nullind);
    rxu_setint(&r, (t<0), nullind);
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return 0;
}

/*
** less than or equal
*/
int rxo_le(fgw_stacktype *s)
{
    int t, nullind;
    exprstack_t *o1, *o2, r;

    memset(&r, 0, sizeof(r));
    o2=(exprstack_t *) fgw_stackpop(s);
    o1=(exprstack_t *) fgw_stackpop(s);
    t=rxu_compare(rxu_exprtype(o1->type, o2->type), o1, o2, &nullind);
    rxu_setint(&r, (t<=0), nullind);
    rxu_freetssstack(o1);
    rxu_freetssstack(o2);
    fgw_stackpush(s, &r);
    return 0;
}
