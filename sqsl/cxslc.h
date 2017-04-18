/*
	CXSLC.h  -  SQSL interpreter exported symbol list

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Jan 17
	Current release: Jan 17

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

#ifndef CXLSC_H

#ifdef NEED_XSL
#include "cfioc.h"
#include "crxuc.h"
#include "ccmnc.h"
typedef struct {
    int (*rxu_addfd)(int fd, ssize_t (*r)(), ssize_t (*w)(), int (*c)());
    char *(*rxu_errorinfo)();
    exprstack_t *(*rxu_arglist)(fgw_stacktype *s, int *c);
    void (*rxu_pushresult)(fgw_stacktype *s, exprstack_t *o);
    char *(*rxu_malloc)(int s);
    char *(*rxu_realloc)(char *p, int s);
    int (*rxu_isnull)(exprstack_t *r);
    void (*rxu_setnull)(exprstack_t *r, int t);
    int (*rxu_toint)(exprstack_t *r, int *n);
    char *(*rxu_tostring)(exprstack_t *r, char *b, int *n);
    char *(*rxu_topictstring)(exprstack_t *r, char *b, int l, char *p, int t, int n);
    int (*rxu_cvdecimal)(exprstack_t *r, char *s);
    int (*rxu_cvfmtdtime)(exprstack_t *r, char *s, char *p);
    int (*rxu_cvfmtinv)(exprstack_t *r, char *s, char *p);
    int (*rxu_cvfmtdate)(exprstack_t *r, char *s, char *p);
    int (*rxu_decgetprec)(fgw_dec_t *d);
    int (*rxu_decgetscale)(fgw_dec_t *d);
    int (*rxu_decgetsign)(fgw_dec_t *d);
    int (*rxu_decgetdigit)(fgw_dec_t *d, int *p);
    int (*rxu_decsetprec)(fgw_dec_t *d, int p);
    int (*rxu_decsetscale)(fgw_dec_t *d, int s);
    int (*rxu_decsetsign)(fgw_dec_t *d, int s);
    int (*rxu_decsetdigit)(fgw_dec_t *d, int n, int *p);
    int (*fgw_hash)(char *s, long b);
    void (*fgw_clip)(char *c);
    int (*fgw_pos)(char *s1, char *s2);
    void (*fgw_move)(void *d, void *s, int c);
    void (*fgw_escmove)(void *d, void *s, int c, char e);
    int (*fgw_strcasecmp)(char *s1, char *s2);
    char *(*fgw_strcasestr)(char *s1, char *s2);

} xsl_t;
#define XSL_EXPORTED static xsl_t xsl_exported = { 		\
    rxu_addfd,							\
    rxu_errorinfo,						\
    rxu_arglist,						\
    rxu_pushresult,						\
    rxu_malloc,							\
    rxu_realloc,						\
    rxu_isnull,							\
    rxu_setnull,						\
    rxu_toint,							\
    rxu_tostring,						\
    rxu_topictstring,						\
    rxu_cvdecimal,						\
    rxu_cvfmtdtime,						\
    rxu_cvfmtinv,						\
    rxu_cvfmtdate,						\
    rxu_decgetprec,						\
    rxu_decgetscale,						\
    rxu_decgetsign,						\
    rxu_decgetdigit,						\
    rxu_decsetprec,						\
    rxu_decsetscale,						\
    rxu_decsetsign,						\
    rxu_decsetdigit,						\
    fgw_hash,							\
    fgw_clip,							\
    fgw_pos,							\
    fgw_move,							\
    fgw_escmove,						\
    fgw_strcasecmp,						\
    fgw_strcasestr						\
};
#define XSL_PARMS xsl_t *xsl_exported
#define XSL_ARGS &xsl_exported
#define XSL_STATE static xsl_t *xsl_imported;
#define XSL_SETSTATE xsl_imported=xsl_exported;
#define XSL_CALL(s) (xsl_imported->s)

#else
#define XSL_EXPORTED
#define XSL_PARMS
#define XSL_ARGS
#define XSL_STATE
#define XSL_SETSTATE
#define XSL_CALL(s) s
#endif

#define CXLSC_H
#endif
