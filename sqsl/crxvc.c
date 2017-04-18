/*
	CRXVC.c  -  Expression stack variable primitives

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
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

#include <string.h>
#include "coslc.h"
#include "chstc.h"
#include "crxuc.h"
#include "crxvc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqxd.h"

extern TGLOB parserstate_t *pstate;

/*
** assigns stack contents to variable
*/
void sqv_setvar(fgw_loc_t *h, exprstack_t *e, exprstack_t *v)
{
    hst_t hid;
    exprstack_t cnv, *s;
    fgw_dec_t dc;
    fgw_dtime_t dt;
    fgw_intrvl_t inv;
    char buf[STRINGCONVSIZE];

    if (!(hid=v->val.identifier))
	return;
    if (v->totype)
    {
	if (rxu_isnull(e))
	    rxu_setnull(&cnv, v->totype);
	else
	switch (v->totype)
	{
	  case CINTTYPE:
	    cnv.val.intvalue=rxu_toint(e, NULL);
	    break;
	  case CDOUBLETYPE:
	    cnv.val.real=rxu_todouble(e, NULL);
	    break;
	  case CDECIMALTYPE:
	  case CMONEYTYPE:
	    cnv.val.decimal=rxu_todecimal(e, &dc, NULL);
	    break;
	  case CSTRINGTYPE:
	    cnv.val.string=rxu_tostring(e, (char *) &buf, NULL);
	    break;
	  case CDATETYPE:
	    cnv.val.intvalue=rxu_todate(e, NULL);
	    break;
	  case CDTIMETYPE:
	    cnv.val.datetime=rxu_todtime(e, &dt, v->qual, NULL);
	    break;
	  case CINVTYPE:
	    cnv.val.interval=rxu_toinv(e, &inv, v->qual, NULL);
	    break;
	  case FGWBYTETYPE:
	    cnv.val.string=rxu_tobyte(e, (char *) &buf, &cnv.length, NULL);
	}
	cnv.type=v->totype;
	s=&cnv;
    }
    else
	s=e;
    switch (s->type)
    {
      case CSTRINGTYPE:
	fgw_hstidxadd(h, hid, CSTRINGTYPE,
		      rxu_isnull(s)? -1: strlen(s->val.string), s->val.string);
	break;
      case CDOUBLETYPE:
      case CINTTYPE:
      case CDATETYPE:
	fgw_hstidxadd(h, hid, s->type, s->length, (char *) &(s->val.real));
	break;
      case FGWBYTETYPE:
	fgw_hstidxadd(h, hid, s->type, s->length, s->val.string);
	break;
      default:
	fgw_hstidxadd(h, hid, s->type, s->length, (char *) s->val.datetime);
    }
}

/*
** creates leaf and assigns stack contents to it
*/
void sqv_newvar(fgw_loc_t *h, exprstack_t *e, char *l, exprstack_t *v)
{
    hst_t hid;
    exprstack_t cnv, *s;
    fgw_dec_t dc;
    fgw_dtime_t dt;
    fgw_intrvl_t inv;
    char buf[STRINGCONVSIZE];

    if (!(hid=v->val.identifier))
	return;
    if (v->totype)
    {
	if (rxu_isnull(e))
	    rxu_setnull(&cnv, v->totype);
	else
	switch (v->totype)
	{
	  case CINTTYPE:
	    cnv.val.intvalue=rxu_toint(e, NULL);
	    break;
	  case CDOUBLETYPE:
	    cnv.val.real=rxu_todouble(e, NULL);
	    break;
	  case CDECIMALTYPE:
	  case CMONEYTYPE:
	    cnv.val.decimal=rxu_todecimal(e, &dc, NULL);
	    break;
	  case CSTRINGTYPE:
	    cnv.val.string=rxu_tostring(e, (char *) &buf, NULL);
	    break;
	  case CDATETYPE:
	    cnv.val.intvalue=rxu_todate(e, NULL);
	    break;
	  case CDTIMETYPE:
	    cnv.val.datetime=rxu_todtime(e, &dt, v->qual, NULL);
	    break;
	  case CINVTYPE:
	    cnv.val.interval=rxu_toinv(e, &inv, v->qual, NULL);
	    break;
	  case FGWBYTETYPE:
	    cnv.val.string=rxu_tobyte(e, (char *) &buf, &cnv.length, NULL);
	}
	cnv.type=v->totype;
	s=&cnv;
    }
    else
	s=e;
    switch (s->type)
    {
      case CSTRINGTYPE:
	fgw_hstadd(h, hid, l, CSTRINGTYPE, s->length, s->val.string);
	break;
      case CDOUBLETYPE:
      case CINTTYPE:
      case CDATETYPE:
	fgw_hstadd(h, hid, l, s->type, s->length, (char *) &(s->val.real));
	break;
      case FGWBYTETYPE:
	fgw_hstadd(h, hid, l, s->type, s->length, s->val.string);
	break;
      default:
	fgw_hstadd(h, hid, l, s->type, s->length, (char *) s->val.datetime);
    }
}

/*
**  generates stack entry from var
**
**  WARNING: set safecopy for expressions that will last across statements,
**  eg FOR or WAIT FOR loops, or for assignments done inside the
**  expression parser: hashes do reoptimize, and in such cases a safe
**  copy (as opposed to just copying the pointer) will avoid nasty crashes
*/
int sqv_cvvar(exprstack_t *s, hst_t v, int safecopy)
{
    fgw_hstentry *e;
    int rc;

/*
** nullify if symbol undefined
*/
    if (!v || !(e=fgw_hstidxget(pstate->vars, v)))
    {
	rxu_setnull(s, CINTTYPE);
	return 0;
    }
/*
** or it contains a null
*/
    else if (e->len<0)
    {
	rxu_setnull(s, e->type);
	return 0;
    }
    else switch (s->type=e->type)
    {
      case CSTRINGTYPE:
	if (safecopy)
	{
	    int l=strlen((char *) &(e->entry))+1;

	    s->val.string=fgw_smalloc(l);
	    fgw_move(s->val.string, (char *) &(e->entry), l);
	}
	else
	{
	    s->flags|=EF_NOTONTSS;
	    s->val.string=(char *) &(e->entry);
	}
	break;
      case CDTIMETYPE:
      case CINVTYPE:
	if (safecopy)
	{
	    s->val.datetime=(fgw_dtime_t *) fgw_smalloc(sizeof(fgw_dtime_t));
	    fgw_move((char *) s->val.datetime, (char *) &(e->entry),
		    sizeof(fgw_dtime_t));
	}
	else
	    s->flags|=EF_NOTONTSS;
	    s->val.datetime=(fgw_dtime_t *) &(e->entry);
	{
	}
	break;
      case CDECIMALTYPE:
      case CMONEYTYPE:
	if (safecopy)
	{
	    s->val.decimal=(fgw_dec_t *) fgw_smalloc(sizeof(fgw_dec_t));
	    fgw_move((char *) s->val.decimal, (char *) &(e->entry),
		    sizeof(fgw_dec_t));
	}
	else
	{
	    s->flags|=EF_NOTONTSS;
	    s->val.decimal=(fgw_dec_t *) &(e->entry);
	}
	break;
      case CDOUBLETYPE:
	s->val.real=*(double *) &(e->entry);
	break;
/*
** byte we don't know how to handle, hence we set to null
** FIXME: disparity with csqoc
*/
      case FGWBYTETYPE:
	rxu_setnull(s, FGWBYTETYPE);
	break;
      case T_HASH:
	return RC_INHST;
      default:
	s->val.intvalue=*(int *) &(e->entry);
    }
    s->length=e->len;
    return 0;
}

/*
** evaluates hash expressions and returns hst pointing to target variable
*/
int sqv_evalhash(fgw_stacktype *s, hst_t *hid)
{
    fgw_hstentry *e;
    exprstack_t *t, *x;
    int c, d, i;
    int rc=0;
    int sz=(pstate->instrp->flags & RT_RESVAR)? 32: 0;
    hst_t m=0;
    char *n, buff[STRINGCONVSIZE];


    if (!(*hid=pstate->instrp->constant.val.identifier))
    {
	if (pstate->instrp->flags & RT_RESVAR)
	    rc=RC_NOVAR;
	goto exit;
    }
    if (!(e=fgw_hstidxget(pstate->vars,
			  pstate->instrp->constant.val.identifier)))
    {
	rc=RC_NOVAR;
	goto exit;
    }
    if (c=pstate->instrp->constant.count)
    {
/*
** keep track of expanded hashes on list
*/
	if (pstate->instrp->constant.flags & EF_VARIABLELIST)
	    for (i=c, t=(exprstack_t *) fgw_stackpeek(s); i; i--, t--)
		if (t->count)
		{
		    c+=t->count-1;
		    t-=(t->count-1);
		}
	t=((exprstack_t *) fgw_stackpeek(s))-c+1;
/*
** create hash nodes as required
*/
	for (i=1, x=t; ; i++, x++)
	{
	    int isnull;

	    if (e->type!=T_HASH)
	    {
		rc=RC_INSCL;
		goto exit;
	    }

	    n=rxu_tostring(x, (char *) &buff, &isnull);

	    /* we treat empty strings and nulls differently */
	    if (isnull)

		/* FIXME ain't that cheeky */
		n="\377NULL";
	    m=e->magic;
/*
** we're done
*/
	    if (i==c)
		break;
	    if (!(e=fgw_hstget(pstate->vars, m, n, &d)))
	        if (sz)
		{
		    if (!(m=fgw_hstaddnode(pstate->vars, m, n, 8)))
		    {
			rc=status;
			goto exit;
		    }
		    else
			e=fgw_hstidxget(pstate->vars, m);
		}
		else
		    break;
	}
	for (i=c, x=t; i; i--, x++)
	    rxu_freetssstack(x);
/*
** star assignment - we need to find or allocate a hash
*/
	if ((pstate->instrp->flags & RT_HASHVAR) &&
	    (pstate->instrp->flags & RT_RESVAR))
	{
	    if (e=fgw_hstget(pstate->vars, m, n, &d))
	    {
		if (e->type!=T_HASH)
		{
		    rc=RC_INSCL;
		    goto exit;
	 	}
		*hid=e->magic;
	    }
	    else if (!(*hid=fgw_hstaddnode(pstate->vars, m, n, 8)))
	    {
		rc=status;
		goto exit;
	    }
	}
/*
** find or allocate entry
*/
	else if (e=fgw_hstreserve(pstate->vars, m, n, sz))
	{
	    if (e->type==T_HASH &&
		!(pstate->instrp->flags & RT_HASHVAR))
	    {
		rc=RC_INHST;
		goto exit;
	    }
	    else
		*hid=e->magic;
	}
/*
** could not allocate
*/
	else if (pstate->parseroptions & RT_RESVAR)
	{
	    rc=status;
	    goto exit;
	}
	else
	    *hid=0;
	fgw_stackdrop(s, pstate->instrp->constant.count);
	return 0;
    }
    else if (e->type==T_HASH && !(pstate->instrp->flags & RT_HASHVAR))
	rc=RC_INHST;
    else
	return 0;

exit:
    *hid=0;
    if (pstate->instrp->constant.count)
	fgw_stackdrop(s, pstate->instrp->constant.count);
    return rc;
}
