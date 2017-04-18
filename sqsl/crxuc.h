/*
	CRXUC.h  -  SQSL interpreter definitions, expression stack utilities

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
	Current release: Apr 17

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

#ifndef CRXUC_H
#include "ccmnc.h"
#include "ctypc.h"
#include "chstc.h"

typedef struct exprstack
{
    int type;			/* see sqltypes.h */
    int flags;			/* ever present */
    int length;			/* length indicator for blobs
				   -1 is a null indicator */
    int count;			/* # of expressions in list */
    int totype;			/* in aid of INTO clause */
    int qual;			/* in line conversion */
    union
    {
	char *string;		/* strings */
	int intvalue;		/* integers */
	double real;		/* floats */
	fgw_dec_t *decimal;	/* decimals & money */
	fgw_dtime_t *datetime;	/* datetimes */
	fgw_intrvl_t *interval;	/* intervals */
	hst_t identifier;	/* identifiers */
    } val; 
} exprstack_t;

/* should be safe values */
#define FGWIDTYPE	1000		/* hash table entry */
#define FGWBYTETYPE	1001		/* unhandleable expression type! */
#define FGWCOLTYPE	1002		/* assignment index into select lists */
#define FGWVARTYPE	CSTRINGTYPE	/* backwards compatibility */

/* flag values */
#define EF_RETSTACK	0x0001
#define EF_VARIABLELIST	0x0002
#define EF_NOTONTSS	0x0004
#define EF_SUBSTITUTE	0x0008
#define EF_FUNCHASH	0x0010
#define EF_FUNCCOUNT	0x0020
#define EF_RELADDR	0x0040
#define EF_VARIABLEDEST	0x0080
#define EF_EXPANDHASH	0x0100
#define EF_CONSTANT	0x0200

/* stack argument list markers */
#define EMPTYMARKER	-1

/*
** error messages
*/
extern char *rxu_errorinfo();
extern void rxu_getmessage(int e, char *b, int l);

/*
** stream API
*/
extern int rxu_addfd(int fd, ssize_t (*r)(), ssize_t (*w)(), int (*c)());

/*
** nulls and stuff
*/
extern int rxu_isnull(exprstack_t *r);
extern void rxu_setnull(exprstack_t *r, int t);

/*
** conversion functions forward declarations
*/
extern int rxu_toboolean(exprstack_t *r, int *n);
extern int rxu_toint(exprstack_t *r, int *n);
extern int rxu_todate(exprstack_t *r, int *n);
extern int rxu_mdy2date(const short *d, int *n);
extern double rxu_todouble(exprstack_t *r, int *n);
extern fgw_dtime_t *rxu_todtime(exprstack_t *r, fgw_dtime_t *dt, int q, int *n);
extern fgw_intrvl_t *rxu_toinv(exprstack_t *r, fgw_intrvl_t *in, int q, int *n);
extern fgw_dec_t *rxu_todecimal(exprstack_t *r, fgw_dec_t *b, int *n);

extern char *rxu_tostring(exprstack_t *r, char *b, int *n);
extern char *rxu_tofmtstring(exprstack_t *r, char *b, int l, char *p, int t, int n);
extern char *rxu_topictstring(exprstack_t *r, char *b, int l, char *p, int t, int n);
extern char *rxu_tobyte(exprstack_t *r, char *b, int *l, int *n);

extern int rxu_cvdecimal(exprstack_t *r, char *s);
extern int rxu_cvfmtdtime(exprstack_t *r, char *s, char *p);
extern int rxu_cvfmtinv(exprstack_t *r, char *s, char *p);
extern int rxu_cvfmtdate(exprstack_t *r, char *s, char *p);

extern void rxu_setint(exprstack_t *r, int v, int n);
extern void rxu_setdouble(exprstack_t *r, double v, int n);

/*
** string conversion helpers
*/
extern int rxu_exprwidth(exprstack_t *e);

/*
** function API forward declariations
*/
extern exprstack_t *rxu_arglist(fgw_stacktype *s, int *c);
extern exprstack_t *rxu_arglist2(fgw_stacktype *s, int *c);
extern void rxu_pushresult(fgw_stacktype *s, exprstack_t *o);
extern void rxu_pushresult2(fgw_stacktype *s, exprstack_t *i, exprstack_t *o);
extern void rxu_noresults(fgw_stacktype *s);
extern char *rxu_tsssave(exprstack_t *e);
extern void rxu_tssfree(char *p);

/*
** expression types and comparisons
*/
extern int rxu_exprtype(int t1, int t2);
extern int rxu_optype(exprstack_t *s1, exprstack_t *s2, exprstack_t **sl, exprstack_t **sr);
extern int rxu_compare(int t, exprstack_t *r1, exprstack_t *r2, int *nullind);

/*
** stack memory (dis)allocation
*/
#define FGW_EXPRDURATION 0
#define FGW_STMTDURATION 1

#define fgw_smalloc(s) fgw_tssmalloc(&pstate->exphead, s)
#define fgw_srealloc(p, s) fgw_tssrealloc(&pstate->exphead, p, s)
#define fgw_sfree(p) fgw_tssfree(&pstate->exphead, p)

extern char *rxu_malloc(int s);
extern char *rxu_realloc(char *p, int s);
extern void rxu_freetssstack(exprstack_t *e);

/*
** Date, Datetime and Interval ops
*/
extern int rxu_inaddsub(exprstack_t *i1, exprstack_t *i2,
                 fgw_intrvl_t *oi1, fgw_intrvl_t *oi2, exprstack_t *rs);
extern int rxu_inadd(fgw_intrvl_t *ii, fgw_intrvl_t *ii2, fgw_intrvl_t *oi);
extern int rxu_insub(fgw_intrvl_t *ii, fgw_intrvl_t *ii2, fgw_intrvl_t *oi);
extern void rxu_dtaddinv(exprstack_t *r1, exprstack_t *r2, exprstack_t *rs);
extern void rxu_dtsubinv(exprstack_t *r1, exprstack_t *r2, exprstack_t *rs);
extern void rxu_dtsub(exprstack_t *r1, exprstack_t *r2, exprstack_t *rs);
extern void rxu_current(exprstack_t *e, int q);
extern void rxu_today(exprstack_t *e);
extern void rxu_mdy(exprstack_t *e, short mdy[3], int *isnull);
extern int rxu_weekday(exprstack_t *e, int *isnull);
extern int rxu_invmuldbl(fgw_intrvl_t *i, double f, fgw_intrvl_t *ir);
extern int rxu_invdivdbl(fgw_intrvl_t *i, double f, fgw_intrvl_t *ir);
extern int rxu_invdivinv(fgw_intrvl_t *i1, fgw_intrvl_t *i2, double *r);

/*
** Decimal and Money ops
*/
extern int rxu_decadd(fgw_dec_t *d1, fgw_dec_t *d2, fgw_dec_t *dr);
extern int rxu_decsub(fgw_dec_t *d1, fgw_dec_t *d2, fgw_dec_t *dr);
extern int rxu_decmul(fgw_dec_t *d1, fgw_dec_t *d2, fgw_dec_t *dr);
extern int rxu_decdiv(fgw_dec_t *d1, fgw_dec_t *d2, fgw_dec_t *dr);

extern int rxu_decgetprec(fgw_dec_t *d);
extern int rxu_decgetscale(fgw_dec_t *d);
extern int rxu_decgetsign(fgw_dec_t *d);
extern int rxu_decgetdigit(fgw_dec_t *d, int *p);

extern int rxu_decsetprec(fgw_dec_t *d, int p);
extern int rxu_decsetscale(fgw_dec_t *d, int s);
extern int rxu_decsetsign(fgw_dec_t *d, int s);
extern int rxu_decsetdigit(fgw_dec_t *d, int n, int *p);

/*
** various allocation sizes
*/
#define STRINGCONVSIZE 30
#define USERNAMELEN 32

#define CRXUC_H
#endif
