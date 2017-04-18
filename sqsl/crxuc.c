/*
	CRXUC.c  -  Expression stack utilities

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

/* FIXME - no documented way to include esql/c prototypes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ccmpc.h"
#include "coslc.h"
#include "ccmnc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "csqec.h"
#include "crxuc.h"

static int rxu_qualifier(int q1,int q2, int i);

extern TGLOB parserstate_t *pstate;

/*
** errorinfo area (for external functions)
*/
char *rxu_errorinfo()
{
    return pstate->errorinfo->extra;
}

/*
** API generated errors
*/
void rxu_getmessage(int e, char *b, int l)
{
    char m[256];
    int len;

    rgetlmsg(e, (char *) &m, (l<sizeof(m))? l: sizeof(m)-1, &len);
    sprintf(b, "%i ", e);
    strncat(b, m, l-strlen(b));
}

/*
** add fd
*/
int rxu_addfd(int fd, ssize_t (*r)(), ssize_t (*w)(), int (*c)())
{
    int fd_id;
    fgw_fdesc *fd_p;

    fd_id=fgw_addfd(fd, r, w, c);
    if (fd_id>=0)
    {
	fd_p=fgw_findfd(fd_id, 0);
	fd_p->fd_entry.signature=pstate->signature|SS_INTERNAL;
    }
    return fd_id;
}
/*
** Nulls 
**
** FIXME tests for null stack element
*/
int rxu_isnull(exprstack_t *r)
{
    return (r->length<0);
}
 
/*
** assigns null int value to stack entry
*/
void rxu_setnull(exprstack_t *r, int t)
{
    r->type=t;
    r->length=-1;
}

/*
** Conversion to base types
**
** converts stack entries to booleans
*/
int rxu_toboolean(exprstack_t *r, int *n)
{
    int i, isnull;

    isnull=rxu_isnull(r);
    if (n)
	*n=isnull;
    switch (r->type)
    {
      case CINTTYPE:
      case CDATETYPE:
	return (r->val.intvalue!=0);
      case CDOUBLETYPE:
	return (r->val.real!=0);
      case CDECIMALTYPE:
      case CMONEYTYPE:
	if (isnull)
	    return 0;
	status=dectoint(r->val.decimal, &i);
	return (i!=0);
      case CDTIMETYPE:
	return 1;
      case CINVTYPE:
	if (isnull)
	    return 0;
	status=dectoint(&(r->val.interval->in_dec), &i);
	return (i!=0);
      case CSTRINGTYPE:
	return (strlen(r->val.string)>1 || *r->val.string!='0');
      default:
	if (n)
	    *n=1;
	return 0;
    }
}

/*
** converts stack entries to integers
*/
int rxu_toint(r, n)
exprstack_t *r;
int *n;
{
    int i;
    char *e;

    if (n)
	*n=rxu_isnull(r);

    /* we use a predictable value if we can't convert, input is null */
    i=-1;
    switch (r->type)
    {
      case CINTTYPE:
      case CDATETYPE:
      case FGWIDTYPE:
	return r->val.intvalue;
      case CDOUBLETYPE:
	i=r->val.real;
	break;
      case CDECIMALTYPE:
      case CMONEYTYPE:
	if (status=dectoint(r->val.decimal, &i))
	    goto cantconvert;
	break;
      case CSTRINGTYPE:
/*
** we could have a float so try it first
*/
	i=strtod(r->val.string, &e);
	if (e!=r->val.string)
	{
	    if (!*e)
		break;
/*
** didn't go thru, could it be an int?
*/
	    i=strtol(r->val.string, &e, 0);
	    if (!*e)
		break;
	}
      default:
cantconvert:
	if (n)
	    *n=1;
    }
    return i;
}

/*
** converts stack entries to floats
*/
double rxu_todouble(exprstack_t *r, int *n)
{
    double f;
    char *e;

    if (n)
	*n=rxu_isnull(r);

    /* we use a predictable value if we can't convert, input is null */
    f=-1;
    switch (r->type)
    {
      case CINTTYPE:
      case CDATETYPE:
	f=r->val.intvalue;
	break;
      case CDOUBLETYPE:
	return r->val.real;
      case CDECIMALTYPE:
      case CMONEYTYPE:
	if (status=dectodbl(r->val.decimal, &f))
	    goto cantconvert;
	break;
      case CSTRINGTYPE:
	f=strtod(r->val.string, &e);
	if (e!=r->val.string && !*e)
	     break;
      default:
cantconvert:
	if (n)
	    *n=1;
    }
    return f;
}

/*
** converts stack entries to decimals
*/
fgw_dec_t *rxu_todecimal(exprstack_t *r, fgw_dec_t *b, int *n)
{
    int isnull;

    isnull=rxu_isnull(r);
    if (n)
	*n=isnull;
    switch (r->type)
    {
      case CDECIMALTYPE:
      case CMONEYTYPE:
	return r->val.decimal;
      case CSTRINGTYPE:
	status=deccvasc(r->val.string, strlen(r->val.string), b);
	break;
      case CDATETYPE:
      case CINTTYPE:
	status=deccvint(r->val.intvalue, b);
	break;
      case CDOUBLETYPE:
	status=deccvdbl(r->val.real, b);
	break;
      default:
	if (n)
	    *n=1;
    }

    /* return a predictable value on null or error */
    if (status || isnull)
	status=deccvint(-1, b);
    return b;
}

/*
** converts stack entries to strings
*/
char *rxu_tostring(exprstack_t *r, char *b, int *n)
{
    int isnull;
    char *p;

    isnull=rxu_isnull(r);
    if (n)
	*n=isnull;
    if (isnull)
    {
	*b='\0';
    }
    else switch (r->type)
    {
      case CSTRINGTYPE:
	return r->val.string;
      case CDTIMETYPE:
	status=dttoasc(r->val.datetime, b);
	break;
      case CINVTYPE:
	status=intoasc(r->val.interval, b);
	break;
      case CDATETYPE:
	rdatestr(r->val.intvalue, b);
	break;
      case CINTTYPE:
	sprintf(b, "%i", r->val.intvalue);
	break;
      case CDOUBLETYPE:
	sprintf(b, "%.6g", r->val.real);
	break;
      case CDECIMALTYPE:
      case CMONEYTYPE:
/* FIXME: I should probably handle DBMONEY here */
	status=dectoasc(r->val.decimal, b, STRINGCONVSIZE-1, -1);
	p=b+STRINGCONVSIZE-2;
	while (*p==' ')
	    p--;
	*++p='\0';
	break;
      case FGWBYTETYPE:
	return "";
      default:
	if (n)
	    *n=1;
	*b='\0';
    }
    return b;
}

/*
** converts stack entries to bytes
*/
char *rxu_tobyte(exprstack_t *r, char *b, int *l, int *n)
{
    int isnull;
    char *p;

    isnull=rxu_isnull(r);
    if (n)
	*n=isnull;
    switch (r->type)
    {
      case FGWBYTETYPE:
	if (l)
	    *l=r->length;
	return r->val.string;
      case CSTRINGTYPE:
	if (l)
	    *l=strlen(r->val.string);
	return r->val.string;
      default:
	if (n)
	    *n=1;
	if (l)
	    *l=0;
	return "";
    }
    return b;
}
/*
** converts stack entries to dates
*/
int rxu_todate(exprstack_t *r, int *n)
{
    char *e;
    int d, isnull;
    char str[STRINGCONVSIZE];
    fgw_dtime_t dt1;

    isnull=rxu_isnull(r);
    if (n)
	*n=isnull;
    if (isnull)
	return 0;

    switch (r->type)
    {
      case CINTTYPE:
      case CDATETYPE:
	d=r->val.intvalue;
	break;
      case CDTIMETYPE:
#ifdef I4GLVER
	pushdtime(r->val.datetime);
	popdate(&d);		/* FIXME in some 4gl versions popdate
				   takes an int4 *, which in turn in some 4gl
				   versions is a long and not an int */
#else
	dt1.dt_qual=TU_DTENCODE(TU_YEAR, TU_DAY);
	IGNORE dtextend(r->val.datetime, &dt1);
	IGNORE dttoasc(&dt1, (char *) &str);
	IGNORE rdefmtdate(&d, "yyyy-mm-dd", &dt1);
#endif
	break;
      case CDOUBLETYPE:
	d=r->val.real;
	break;
      case CDECIMALTYPE:
      case CMONEYTYPE:
	status=dectoint(r->val.decimal, &d);
	break;
      case CSTRINGTYPE:
	rstrdate(r->val.string, &d);
	break;
      default:
	if (n)
	    *n=1;
	d=0;
    }
    return d;
}

/*
** converts 3 date parts into a date
*/
int rxu_mdy2date(const short *d, int *n)
{
     return rmdyjul(d, n);
}

/*
** converts stack entries to datetimes
*/
fgw_dtime_t *rxu_todtime(exprstack_t *r, fgw_dtime_t *dt, int q, int *n)
{
    char str[STRINGCONVSIZE];
    fgw_dtime_t dt1;

    if (n)
	*n=rxu_isnull(r);
    dt->dt_qual=q;
    switch (r->type)
    {
      case CDTIMETYPE:
	if (q<0)
	    return r->val.datetime;
	status=dtextend(r->val.datetime, dt);
	return dt;
      case CDATETYPE:
	if (q<0)
	    dt->dt_qual=TU_DTENCODE(TU_YEAR, TU_DAY);
#ifdef I4GLVER
	pushdate(r->val.intvalue);
	popdtime(dt, dt->dt_qual);
#else
	IGNORE rfmtdate(r->val.intvalue, "yyyy-mm-dd", &str);
	dt1.dt_qual=TU_DTENCODE(TU_YEAR, TU_DAY);
	if (q<0)
	    IGNORE dtcvasc(str, dt);
	else
	{
	    IGNORE dtcvasc(str, &dt1);
	    IGNORE dtextend(&dt1, dt);
	}
#endif
	return dt;
      case CSTRINGTYPE:
	if (q<0)
	     dt->dt_qual=TU_DTENCODE(TU_YEAR, TU_SECOND);
	status=dtcvasc(r->val.string, dt);
	return dt;
      default:
	if (n)
	    *n=1;
	rsetnull(CDTIMETYPE, (char *) dt); /* FIXME */
	return dt;
    }
}

/*
** converts stack entries to intervals
*/
fgw_intrvl_t *rxu_toinv(exprstack_t *r, fgw_intrvl_t *in, int q, int *n)
{
    char cbuf[STRINGCONVSIZE], *s;
 
    if (n)
	*n=rxu_isnull(r);
    in->in_qual=q;
    switch (r->type)
    {
      case CINVTYPE:
	if (q<0)
	    return r->val.interval;
	status=invextend(r->val.interval, in);
	return in;
      case CSTRINGTYPE:
	if (q<0)
	    in->in_qual=TU_IENCODE(5, TU_DAY, TU_F5);
	status=incvasc(r->val.string, in);
	return in;
      default:
	if (q>0)
	    switch (r->type)
	    {
	      case CINTTYPE:
	      case CDECIMALTYPE:
	      case CMONEYTYPE:
	      case CDOUBLETYPE:
		s=rxu_tostring(r, (char *) &cbuf, NULL);
		status=incvasc(s, in);
		return in;
	    }
	if (n)
	    *n=1;
	rsetnull(CINVTYPE, (char *) in); /* FIXME */
	return in;
    }
}

/*
** converts stack entries to pictorial strings
*/
char *rxu_topictstring(exprstack_t *r, char *b, int l, char *p, int t, int n)
{

    /* FIXME: conversion / preserve length not implemented */
    if (rxu_isnull(r))
	*b='\0';
    switch (r->type)
    {
      case CDECIMALTYPE:
      case CMONEYTYPE:
	status=rfmtdec(r->val.decimal, p, b);
	break;
      case CDOUBLETYPE:
	status=rfmtdouble(r->val.real, p, b);
	break;
      case CDATETYPE:
	status=rfmtdate(r->val.intvalue, p, b);
	break;
#ifdef HAS_DTFMT
      case CDTIMETYPE:
	status=dttofmtasc(r->val.datetime, b, l, p);
	break;
      case CINVTYPE:
	status=intofmtasc(r->val.interval, b, l, p);
	break;
#endif
      case CINTTYPE:
       	status=rfmtlong(r->val.intvalue, p, b);
	break;
      default:
	strncpy(b, p, l-1);
	b[l]='\0';
    }
    return b;
}

/*
** converts stack entries to sprintf strings
*/
char *rxu_tofmtstring(exprstack_t *r, char *b, int l, char *p, int t, int n)
{
    exprstack_t *e, cnv;
    *b='\0';

    /* FIXME: conversion / preserve space not implemented */
    if (rxu_isnull(r))
	return b;

    e=r;
    if (t && e->type != t)
    {
	e=&cnv;
	memset(e, 0, sizeof(cnv));
	e->type=t;
	switch(t)
        {
	  case CINTTYPE:
	    cnv.val.intvalue=rxu_toint(r, NULL);
	    break;
	  case CDOUBLETYPE:
	    cnv.val.real=rxu_todouble(r, NULL);
	    break;
	  default:
	    status=RC_NIMPL;
	    return b;
	}
    }
    switch (e->type)
    {
      case CDECIMALTYPE:
      case CMONEYTYPE:

      case CDOUBLETYPE:
	sprintf(b, p, e->val.real);
	break;
      case CDATETYPE:
#ifdef HAS_DTFMT
      case CDTIMETYPE:
      case CINVTYPE:
#endif
	status=RC_NIMPL;
      case CINTTYPE:
	sprintf(b, p, e->val.intvalue);
	break;
      default:
	status=RC_NIMPL;
    }
    return b;
}

/*
** expression character width
*/
int rxu_exprwidth(exprstack_t *e)
{
    int l=0;

    switch(e->type)
    {
      case CMONEYTYPE:
	l++;
	/* FALLTHROUGH */
      case CDECIMALTYPE:
	l+=PRECTOT(e->length);
	if (PRECDEC(e->length))
	   l++;
	break;
      default:
	l=e->length;
    }
    return l;
}

/*
** Conversion from strings
**
** Where buffers are required, they need to be preallocated
** converts a string into a decimal exprstack_t
*/
int rxu_cvdecimal(exprstack_t *r, char *s)
{
    return deccvasc(s, strlen(s), r->val.decimal);
}

/*
** converts a string into a datetime exprstack_t
*/
int rxu_cvfmtdtime(exprstack_t *r, char *s, char *p)
{
    return dtcvfmtasc(s, p, r->val.datetime);
}

/*
** converts a string into an interval exprstack_t
*/
int rxu_cvfmtinv(exprstack_t *r, char *s, char *p)
{
    return incvfmtasc(s, p, r->val.interval);
}

/*
** converts a string into a date exprstack_t
*/
int rxu_cvfmtdate(exprstack_t *r, char *s, char *p)
{
    return rdefmtdate(&r->val.intvalue, p, s);
}

/*
** Conversion from base types
**
** assigns int value to stack entry
*/
void rxu_setint(exprstack_t *r, int v, int n)
{
    
    r->type=CINTTYPE;
    if (n)
	rxu_setnull(r, CINTTYPE);
    else
    {
	r->val.intvalue=v;
	r->length=0;
    }
}

/*
** assigns double value to stack entry
*/
void rxu_setdouble(exprstack_t *r, double v, int n)
{
    
    r->type=CDOUBLETYPE;
    if (n)
	rxu_setnull(r, CDOUBLETYPE);
    else
    {
	r->val.real=v;
	r->length=0;
    }
}

/*
** function API
**
** returns first element in list, element count
*/
exprstack_t *rxu_arglist(fgw_stacktype *s, int *c)
{
    exprstack_t *i;

    if (i=(exprstack_t *) fgw_stackpeek(s))
    {
	if (c)
	    *c=i->count;
	if (i->type==-1)
	    return NULL;
	else
	    return i-i->count+1;
    }
/*
** there should always be an empty arg list marker, but you never know
*/
    else
    {
	if (c)
	     *c=0;
	return NULL;
    }
}

/*
** returns first element in list, element count
*/
exprstack_t *rxu_arglist2(fgw_stacktype *s, int *c)
{
    exprstack_t *i;
    int a;

    if (i=(exprstack_t *) fgw_stackpeek(s))
    {

	/* skip element list */
	if (i->type=-1)
	    a=1;
	else
	    a=i->count;

	/* deal with argument list */
	i=i-a;
	if (c)
	    *c=i->count;
	if (i->type==-1)
	    return NULL;
	else
	    return i-i->count+1;
    }
/*
** there should always be an empty arg list marker, but you never know
*/
    else
    {
	if (c)
	     *c=0;
	return NULL;
    }
}

/*
** drops args list
*/
void rxu_noresult(fgw_stacktype *s)
{
    exprstack_t *i;
    int c, c1;

    i=(exprstack_t *) fgw_stackpeek(s);
    if ((c=i->count) && (i->type!=EMPTYMARKER))
	for (c1=c; c1; c1--, i--)
	    rxu_freetssstack(i);
    if (c>1)
	fgw_stackdrop(s, c-1);
    i=(exprstack_t *) fgw_stackpeek(s);
    i->type=EMPTYMARKER;
    i->count=0;
}

/*
** drops args list, pushes return value (can be called repeatedly)
*/
void rxu_pushresult(fgw_stacktype *s, exprstack_t *o)
{
    exprstack_t *t;
    int c, c1;
 
    t=(exprstack_t *) fgw_stackpeek(s);
    o->count=1;
    o->flags|=EF_RETSTACK;
    if (t->type==EMPTYMARKER)
	fgw_stackdrop(s, 1);
    else if ((c=t->count) && !(t->flags & EF_RETSTACK))
    {
	for (c1=c; c1; c1--, t--)
	    rxu_freetssstack(t);
	fgw_stackdrop(s, c);
    }
    else
	o->count=c+1;
    fgw_stackpush(s, o);
}

/*
** drops args list, pushes return value (can be called repeatedly)
** element + arg list case
*/
void rxu_pushresult2(fgw_stacktype *s, exprstack_t *i, exprstack_t *o)
{
    exprstack_t *t;
    int c, c1;
 
    t=(exprstack_t *) fgw_stackpeek(s);
    o->count=1;
    o->flags|=EF_RETSTACK;
    if (!(t->flags & EF_RETSTACK))
    {

	/* drop element list - tss space freed on individual calls */
	if (t->type==EMPTYMARKER)
	    fgw_stackdrop(s, 1);
	else if ((c=t->count))
	    fgw_stackdrop(s, c);

	/* drop argument list - tss freed now */
	t=(exprstack_t *) fgw_stackpeek(s);
	if (t->type==EMPTYMARKER)
	    fgw_stackdrop(s, 1);
	else if ((c=t->count))
	{
	    for (c1=c; c1; c1--, t--)
		rxu_freetssstack(t);
	    fgw_stackdrop(s, c);
	}
    }
    else
	o->count=t->count+1;
    rxu_freetssstack(i);
    fgw_stackpush(s, o);
}

/*
** expression stack TSS memory allocation
*/
char *rxu_malloc(int s)
{
    return fgw_tssmalloc(&pstate->exphead, s);
}

/*
** expression stack TSS memory reallocation
*/
char *rxu_realloc(char *p, int s)
{
    return fgw_tssrealloc(&pstate->exphead, p, s);
}

/*
** free TSS allocated memory if necessary
*/
void rxu_freetssstack(exprstack_t *e)
{
    switch(e->type)
    {
      case CSTRINGTYPE:
      case CINVTYPE:
      case CDTIMETYPE:
      case CDECIMALTYPE:
      case CMONEYTYPE:
        if (e->val.string && !(e->flags & EF_NOTONTSS))
	{
            fgw_tssfree(&pstate->exphead, e->val.string);
	    e->val.string=NULL;
	}
    }
}

/*
** Multi value returns stack handling
**
** When a function returns multiple values, and an argument on the stack is used 
** repeatedly across return value pushes, TSS on the stack can't be freed by the
** push itself, as it would lead to corruption / crashes etc.
** This pair of functions handles the TSS correctly: save the affected TSS at
** function entry, free it after all the pushes.
** This does not need to be done for those stack entries only used before the
** first result push.
**
** TSS save
*/
char *rxu_tsssave(e)
exprstack_t *e;
{
    switch (e->type)
    {
      case CSTRINGTYPE:
      case FGWBYTETYPE:
      case CDECIMALTYPE:
      case CMONEYTYPE:
      case CDTIMETYPE:
      case CINVTYPE:
	if (!e->flags & EF_NOTONTSS)
	{
	    e->flags|=EF_NOTONTSS;
	    return e->val.string;
	}
    }
    return NULL;
}

/*
** TSS free
*/
void rxu_tssfree(p)
char *p;
{
    if (p)
	fgw_tssfree(&pstate->exphead, p);
}

/*
** expression types and comparisons
**
** determines overall expression type
*/
int rxu_exprtype(int t1, int t2)
{
    if (t1==CDTIMETYPE || t2==CDTIMETYPE)
	return CDTIMETYPE;
    if (t1==CDATETYPE || t2==CDATETYPE)
	return CDATETYPE;
    if (t1==CINVTYPE || t2==CINVTYPE)
	return CINVTYPE;
    if (t1==CMONEYTYPE || t2==CMONEYTYPE)
	return CMONEYTYPE;
    if (t1==CDECIMALTYPE || t2==CDECIMALTYPE)
	return CDECIMALTYPE;
    if (t1==CDOUBLETYPE || t2==CDOUBLETYPE)
	return CDOUBLETYPE;
    if (t1==CINTTYPE || t2==CINTTYPE)
	return CINTTYPE;
    return CSTRINGTYPE;
}

/*
** determines overall expression type for arithmetic operators
*/
int rxu_optype(exprstack_t *s1, exprstack_t *s2, exprstack_t **sl, exprstack_t **sr)
{
    int t;

    if ((t=rxu_exprtype(s1->type, s2->type))==s1->type)
    {
	*sl=s1;
	*sr=s2;
    }
    else
    {
	*sl=s2;
	*sr=s1;
    }
    return t;
}

/*
** compares r1 & r2 converting to type t first
*/
int rxu_compare(int t, exprstack_t *r1, exprstack_t *r2, int *nullind)
{
    int ih1, ih2;
    double fh1, fh2;
    char *sh1, *sh2, sbuf1[STRINGCONVSIZE], sbuf2[STRINGCONVSIZE];
    fgw_dtime_t dt1, dt2, dt3, dt4, *dp1, *dp2;
    fgw_intrvl_t in1, in2, in3, in4, *ip1, *ip2;
    fgw_dec_t *decp1, *decp2, dec1, dec2;
    int r, q;
    int d1, d2;
    int n1, n2;

    switch (t)
    {
      case CDATETYPE:
	d1=rxu_todate(r1, &n1);
	d2=rxu_todate(r2, &n2);
	if (*nullind=n1||n2)
	    r=0;
	else
	    r=(d1-d2);
	break;
      case CINTTYPE:
	ih1=rxu_toint(r1, &n1);
	ih2=rxu_toint(r2, &n2);
	*nullind=n1||n2;
	r=(ih1-ih2);
	break;
      case CDECIMALTYPE:
      case CMONEYTYPE:
	decp1=rxu_todecimal(r1, &dec1, &n1);
	decp2=rxu_todecimal(r2, &dec2, &n2);
	if (*nullind=n1||n2)
	    r=0;
	else
	    r=deccmp(decp1, decp2);
	break;
      case CDOUBLETYPE:
	fh1=rxu_todouble(r1, &n1);
	fh2=rxu_todouble(r2, &n2);
	*nullind=n1||n2;
	r=(fh1>fh2? 1: (fh1<fh2? -1: 0));
	break;
      case CSTRINGTYPE:
	sh1=rxu_tostring(r1, (char *) &sbuf1, &n1);
	sh2=rxu_tostring(r2, (char *) &sbuf2, &n2);
	*nullind=n1||n2;
	r=fgw_strcmp(sh1, sh2);
	break;
      case CDTIMETYPE:
	dp1=rxu_todtime(r1, &dt1, -1, &n1);
	dp2=rxu_todtime(r2, &dt2, -1, &n2);
	if (*nullind=n1||n2)
	    r=0;
	else
	{
	    q=rxu_qualifier(dp1->dt_qual, dp2->dt_qual, 0);
	    dt3.dt_qual=q;
	    status=dtextend(dp1, &dt3);
	    dt4.dt_qual=q;
	    status=dtextend(dp2, &dt4);
	    r=deccmp(&dt3.dt_dec, &dt4.dt_dec);
	}
	break;
      case CINVTYPE:
	ip1=rxu_toinv(r1, &in1, -1, &n1);
	ip2=rxu_toinv(r2, &in2, -1, &n2);
	if (*nullind=n1||n2)
	    r=0;
	else
	{
	    q=rxu_qualifier(ip1->in_qual, ip2->in_qual, 1);
/*
** check correct qualifier range
*/
	    if (TU_START(q)<TU_DAY && TU_END(q)>=TU_DAY)
		return RC_IDTIV;
	    else
	    {
		in3.in_qual=q;
		IGNORE invextend(ip1, &in3);
		in4.in_qual=q;
		IGNORE invextend(ip2, &in4);
		r=deccmp(&in3.in_dec, &in4.in_dec);
	    }
	}
    }
    return r;
}

/*
** Decimal and money
**
** add two decimals
*/
int rxu_decadd(fgw_dec_t *d1, fgw_dec_t *d2, fgw_dec_t *dr)
{
    return decadd(d1, d2, dr);
}

/*
** subtract two decimals
*/
int rxu_decsub(fgw_dec_t *d1, fgw_dec_t *d2, fgw_dec_t *dr)
{
    return decsub(d1, d2, dr);
}

/*
** multiply two decimals
*/
int rxu_decmul(fgw_dec_t *d1, fgw_dec_t *d2, fgw_dec_t *dr)
{
    return decmul(d1, d2, dr);
}

/*
** divide two decimals
*/
int rxu_decdiv(fgw_dec_t *d1, fgw_dec_t *d2, fgw_dec_t *dr)
{
    return decdiv(d1, d2, dr);
}

/*
** get decimal precision
*/
int rxu_decgetprec(fgw_dec_t *d)
{
    return (d->dec_ndgts>d->dec_exp? d->dec_ndgts: d->dec_exp)*2;
}

/*
** get decimal scale
*/
int rxu_decgetscale(fgw_dec_t *d)
{
    if (d->dec_ndgts>=d->dec_exp)
	return (d->dec_ndgts-d->dec_exp)/2;
    else
	return 0;
}

/*
** get decimal sign
*/
int rxu_decgetsign(fgw_dec_t *d)
{
    return (d->dec_pos>0? 1: -1);
}

/*
** get decimal digit
*/
int rxu_decgetdigit(fgw_dec_t *d, int *p)
{
    int max=(d->dec_ndgts>d->dec_exp? d->dec_ndgts: d->dec_exp);
    int odd=(*p & 1);
    int idx=*p>>1;

    (*p)++;
    if (idx<0 || idx>=max)
	return -1;
    else if (idx>=d->dec_ndgts)
	return 0;
    else if (odd)
	return d->dec_dgts[idx] % 10;
    return d->dec_dgts[idx]/10;
}

#define DEC_ADDDIGIT	0x80
/*
** set decimal precision - start with this
*/
int rxu_decsetprec(fgw_dec_t *d, int p)
{
    if (p<0 || p>DECSIZE)
	return -1;

    memset(d, 0, sizeof(fgw_dec_t));
    d->dec_ndgts=p;
    return 0;
}

/*
** set decimal scale - this follows precision
*/
int rxu_decsetscale(fgw_dec_t *d, int s)
{
    if (s<0 ||s>d->dec_ndgts)
	return -1;
    d->dec_exp=d->dec_ndgts-s;

    /* ouch, we have to add a leading 0 */
    if (d->dec_exp & 1)
    {
	d->dec_exp|=DEC_ADDDIGIT;

	/* and a trailing digit */
	if (!(d->dec_ndgts & 1))
	    d->dec_ndgts++;
    }

    /* no digit shifting required */
    else
    {

        /* we have to add a trailing 0 to the scale */
	if (d->dec_ndgts & 1)
	     d->dec_ndgts+=1;

        /* but otherwise no digit shifting required */
	d->dec_exp/=2;
	d->dec_ndgts/=2;
    }
    return 0;
}

/*
** set decimal digit - precision and scale must have been set
** position is for our internal use and caller must have their
** own counter
*/
int rxu_decsetdigit(fgw_dec_t *d, int n, int *p)
{
    int odd=(*p & 1);
    int idx=*p>>1;

/*
** deal with odd length integer part
** we have already made the scale even
*/
    if (d->dec_exp & DEC_ADDDIGIT)
    {
	d->dec_exp&=~DEC_ADDDIGIT;
	if (*p!=0)
	    return -1;
/*
** if the first digit is non zero with add a zero in front
*/
	if (n!=0)
	{
	    *p=2;
	    d->dec_dgts[0]=n;
	    d->dec_ndgts=(d->dec_ndgts+1)/2;
	    d->dec_exp=(d->dec_exp+1)/2;
	}
/*
** otherwise we ditch a digit
*/
	else
	{
	    d->dec_ndgts/=2;
	    d->dec_exp/=2;
	}
	return 0;
    }
/*
** skip non significant zeros
*/
    if (*p==2 && d->dec_dgts[0]==0)
    {
	d->dec_exp--;
	d->dec_ndgts--;
	idx--;
	*p=0;
    }
    (*p)++;
    if (idx<0 || idx>=d->dec_ndgts || n<0 || n>9)
	return -1;
    if (odd)
	d->dec_dgts[idx]+=n;
    else
	d->dec_dgts[idx]+=(n*10);
    return 0;
}

/*
** set decimal sign - whenever
*/
int rxu_decsetsign(fgw_dec_t *d, int s)
{
    if (s>0)
	d->dec_pos=1;
    else
	d->dec_pos=0;
    return 0;
}

/*
** Datetimes and intervals
**
** determines largest qualifier for 2 intervals or datetimes
*/
static int rxu_qualifier(int q1, int q2, int i)
{
    int s1, s2, s, l1, l2, e;

    e=TU_END(q1);
    if (e<TU_END(q2))
	e=TU_END(q2);
    s1=TU_START(q1);
    s2=TU_START(q2);
    if (s1>s2)
	s=s2;
    else
	s=s1;
/*
**  for intervals we need to work out the length as well
*/
    if (i)
    {
	l1=TU_FLEN(q1);
	l2=TU_FLEN(q2);
	if (s1==s2 && l2>l1)
	    l1=l2;
	return TU_IENCODE(l1+1, s, e);
    }
    else
	return TU_DTENCODE(s, e);
}

#ifdef NEED_DTINVS
/*
**  need our homegrown invextend
*/
static int invextend(fgw_intrvl_t *ii, fgw_intrvl_t *oi)
{
    int q;

    q=oi->in_qual;
/* FIXME: using 4gl internals */
    pushinv(ii);
    popinv(oi, q);
    return status;
}

/*
**  need our homegrown dtaddinv
*/
static int dtaddinv(fgw_dtime_t *id, fgw_intrvl_t *ii, fgw_dtime_t *od)
{
    int q;

    q=id->dt_qual;
/* FIXME: using 4gl internals */
    pushdtime(id);
    pushinv(ii);
    _doadd();
    popdtime(od, q);
    return status;
}

/*
**  need our homegrown dtsub
*/
static int dtsub(fgw_dtime_t *id, fgw_dtime_t *id2, fgw_intrvl_t *oi)
{
    int q;

    q=oi->in_qual;
/* FIXME: using 4gl internals */
    pushdtime(id);
    pushdtime(id2);
    _dosub();
    popinv(oi, q);
    return status;
}

/*
**  need our homegrown dtsubinv
*/
static int dtsubinv(fgw_dtime_t *id, fgw_intrvl_t *ii, fgw_dtime_t *od)
{
    int q;

    q=id->dt_qual;
/* FIXME: using 4gl internals */
    pushdtime(id);
    pushinv(ii);
    _dosub();
    popdtime(od, q);
    return status;
}

/*
**  need our homegrown invmuldbl
*/
static int invmuldbl(fgw_intrvl_t *ii, double f, fgw_intrvl_t *oi)
{
    int q;

    q=ii->in_qual;
/* FIXME: using 4gl internals */
    pushinv(ii);
    pushdub(&f);
    _domult();
    popinv(oi, q);
    return status;
}

/*
**  need our homegrown invdivdbl
*/
static int invdivdbl(fgw_intrvl_t *ii, double f, fgw_intrvl_t *oi)
{
    int q;

    q=ii->in_qual;
/* FIXME: using 4gl internals */
    pushinv(ii);
    pushdub(&f);
    _dodiv();
    popinv(oi, q);
    return status;
}

/*
**  need our homegrown invdivinv
*/
static int invdivinv(fgw_intrvl_t *ii, fgw_intrvl_t *ii2, double *of)
{
/* FIXME: using 4gl internals */
/* FIXME: apparently always yields null with 4.x */
    pushinv(ii);
    pushinv(ii2);
    _dodiv();
    popdub(of);
    return status;
}
#endif

/*
**  need our homegrown inadd
*/
int rxu_inadd(fgw_intrvl_t *ii, fgw_intrvl_t *ii2, fgw_intrvl_t *oi)
{
#ifdef I4GLVER
    int q;

    q=oi->in_qual;
    pushinv(ii);
    pushinv(ii2);
    _doadd();
    popinv(oi, q);
#else
    fgw_dtime_t d1, d2, d3;

    d2.dt_qual=TU_DTENCODE(TU_YEAR, TU_YEAR);
    IGNORE dtcvasc("0", &d2);
    d1.dt_qual=TU_DTENCODE(TU_YEAR, TU_END(oi->in_qual));
    IGNORE dtextend(&d2, &d1);
    status=dtaddinv(&d1, ii, &d2);
    status=dtaddinv(&d2, ii2, &d3);
    status=dtsub(&d3, &d1, oi);
#endif
    return status;
}

/*
**  need our homegrown insub
*/
int rxu_insub(fgw_intrvl_t *ii, fgw_intrvl_t *ii2, fgw_intrvl_t *oi)
{
#ifdef I4GLVER
    int q;

    q=oi->in_qual;
    pushinv(ii);
    pushinv(ii2);
    _dosub();
    popinv(oi, q);
#else
    fgw_dtime_t d1, d2, d3;

    d2.dt_qual=TU_DTENCODE(TU_YEAR, TU_YEAR);
    IGNORE dtcvasc("0", &d2);
    d1.dt_qual=TU_DTENCODE(TU_YEAR, TU_END(oi->in_qual));
    IGNORE dtextend(&d2, &d1);
    status=dtaddinv(&d1, ii, &d2);
    status=dtsubinv(&d2, ii2, &d3);
    status=dtsub(&d3, &d1, oi);
#endif
    return status;
}

/*
** does datetime interval addition
*/
void rxu_dtaddinv(exprstack_t *r1, exprstack_t *r2, exprstack_t *rs)
{
    int n1, n2;
    fgw_dtime_t *dp1, dt1, dte;
    fgw_intrvl_t *ip1, in1;

    dp1=rxu_todtime(r1, &dt1, -1, &n1);
    ip1=rxu_toinv(r2, &in1, -1, &n2);
/*
**  need to extend the resulting datetime
*/
    dte.dt_qual=rxu_qualifier(dp1->dt_qual, ip1->in_qual, 0);
    status=dtextend(dp1, &dte);
    if (!(rs->val.datetime=(fgw_dtime_t *)
	fgw_smalloc(sizeof(fgw_dtime_t))))
	status=RC_NOMEM;
    else
    {
	rs->type=CDTIMETYPE;
	if (n1||n2)
	    rxu_setnull(rs, CDTIMETYPE);
	else
	    status=dtaddinv(&dte, ip1, rs->val.datetime);
    }
}

/*
** does datetime interval subtraction
*/
void rxu_dtsubinv(exprstack_t *r1, exprstack_t *r2, exprstack_t *rs)
{
    int n1, n2;
    fgw_dtime_t *dp1, dt1, dte;
    fgw_intrvl_t *ip1, in1;

    dp1=rxu_todtime(r1, &dt1, -1, &n1);
    ip1=rxu_toinv(r2, &in1, -1, &n2);
/*
**  need to extend the resulting datetime
*/
    dte.dt_qual=rxu_qualifier(dp1->dt_qual, ip1->in_qual, 0);
    status=dtextend(dp1, &dte);
    if (!(rs->val.datetime=(fgw_dtime_t *)
	fgw_smalloc(sizeof(fgw_dtime_t))))
	status=RC_NOMEM;
    else
    {
	rs->type=CDTIMETYPE;
	if (n1||n2)
	    rxu_setnull(rs, CDTIMETYPE);
	else
	    status=dtsubinv(&dte, ip1, rs->val.datetime);
    }
}

/*
** does datetime datetime subtraction
*/
void rxu_dtsub(exprstack_t *r1, exprstack_t *r2, exprstack_t *rs)
{
    int n1, n2;
    fgw_dtime_t *dp1, *dp2, dt1, dt2;

    dp1=rxu_todtime(r1, &dt1, -1, &n1);
    dp2=rxu_todtime(r2, &dt2, -1, &n2);
    if (!(rs->val.interval=(fgw_intrvl_t *)
	fgw_smalloc(sizeof(fgw_intrvl_t))))
	status=RC_NOMEM;
    else
    {
	rs->type=CINVTYPE;
/*
**  need to initialize the resulting interval qualifier
**  FIXME: as there's no way in the syntax to specify the class, we will use
**  day to ...
*/
	rs->val.interval->in_qual=rxu_qualifier(dp1->dt_qual, dp2->dt_qual, 0);
	if (TU_START(rs->val.interval->in_qual)<TU_DAY)
	    rs->val.interval->in_qual=TU_IENCODE(5, TU_DAY,
					TU_END(rs->val.interval->in_qual));
	if (n1||n2)
	    rxu_setnull(rs, CINVTYPE);
	else
	    status=dtsub(dp1, dp2, rs->val.interval);
    }
}

/*
**  prepares for interval addition/subtraction
*/
int rxu_inaddsub(exprstack_t *i1, exprstack_t *i2,
		 fgw_intrvl_t *oi1, fgw_intrvl_t *oi2, exprstack_t *rs)
{
    int n1, n2, q;
    fgw_intrvl_t *ip1, *ip2, in1, in2;

    ip1=rxu_toinv(i1, &in1, -1, &n1);
    ip2=rxu_toinv(i2, &in2, -1, &n2);
    rs->type=CINVTYPE;
    if (!(rs->val.interval=(fgw_intrvl_t *) fgw_smalloc(sizeof(fgw_intrvl_t))))
    {
	status=RC_NOMEM;
	return 0;
    }
    if (n1||n2)
    {
	rxu_setnull(rs, CINVTYPE);
	return 0;
    }
    else
    {
	oi1->in_qual=rxu_qualifier(ip1->in_qual, ip2->in_qual, 1);
	oi2->in_qual=oi1->in_qual;
	rs->val.interval->in_qual=oi1->in_qual;
	if (status=invextend(ip1, oi1))
	   return 0;
	status=invextend(ip2, oi2);
	return !status;
    }
}

/*
** returns the current time
*/
void rxu_current(exprstack_t *e, int q)
{
     e->val.datetime->dt_qual=q;
     dtcurrent(e->val.datetime);
}

/*
** returns today
*/
void rxu_today(exprstack_t *e)
{
    e->type=CDATETYPE;
    rtoday(&e->val.intvalue);
}

/*
** returns weekday
*/
int rxu_weekday(exprstack_t *e, int *isnull)
{
    *isnull=rxu_isnull(e);
    if (*isnull)
	return -1;
    else
	return rdayofweek(e->val.intvalue);
}

/*
** returns date split in mdy parts
*/
void rxu_mdy(exprstack_t *e, short mdy[3], int *isnull)
{
    int d;

    d=rxu_todate(e, isnull);
    if (!isnull)
	rjulmdy(d, mdy);
}

/*
** multiply an interval by a double
*/
int rxu_invmuldbl(fgw_intrvl_t *i, double f, fgw_intrvl_t *ir)
{
    return invmuldbl(i, f, ir);
}

/*
** divide an interval by a double
*/
int rxu_invdivdbl(fgw_intrvl_t *i, double f, fgw_intrvl_t *ir)
{
    return invdivdbl(i, f, ir);
}

/*
** divide an interval by an interval
*/
int rxu_invdivinv(fgw_intrvl_t *i1, fgw_intrvl_t *i2, double *r)
{
    return invdivinv(i1, i2, r);
}
