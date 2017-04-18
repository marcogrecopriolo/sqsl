/*
	CCMPC.h  -  4gl Compiler properties

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2012 Marco Greco (marco@4glworks.com)

	Initial release: Oct 02
	Current release: Jan 12

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

#ifndef CCMPC_H

#ifdef A4GL

#include <a4gl_incl_4glhdr.h>
#include <a4gl_incl_infx.h>
#define FGLGLOB(x) aclfgl_ ## x
#define fgl_call(x, p) aclfgl_ ## x(p)

/* 2 is for longs, which breaks 64 bit builds */
/* #define popint(x) (void) A4GL_pop_var2(x, 7, 0) */
/* #define popdate(x) (void) A4GL_pop_var2(x, 7, 0) */
/* #define popdub(x) (void) A4GL_pop_var2(x, 3, 0) */
/* #define popquote(x, s) (void) A4GL_pop_var2(x, 0, (s)-1) */
/* #define popdec	*/
/* #define popdtime	*/
/* #define popinv	*/

/* #define retint A4GL_push_long */
/* #define pushint A4GL_push_long */
/* #define retlong A4GL_push_long */
/* #define retdate A4GL_push_date */
/* #define retdub(x) A4GL_push_double(*(x)) */
/* #define retquote A4GL_push_char */
/* #define pushquote(x, l) A4GL_push_char(x) */
/* #define retdec */
/* #define retdtime */
/* #define retinv */
/* #define pushlocator */

#define LOC_T(x)		fgw_loc_t x ## _static, *x=(&x ## _static); \
			 	fglbyte *x ## _a4gl;
#define POPLOCATOR(x)		fgw_copylocator(x, &x ## _a4gl);
#define RESTORELOCATOR(x)	fgw_restorelocator(x, x ## _a4gl);

#define SETNULLINT(x)		A4GL_setnull(2, x, 0);
extern long a4gl_status;
#define status a4gl_status

extern long int_flag;

#define FGW_GLOBALS

#else	/* A4GL */

#if I4GLVER>730
#include <fglsys.h>
#endif	/* I4GLVER */

#ifdef I4GLVER
#define FGLGLOB(x) x
#include <fglapi.h>
#define LOC_T(x)		fgw_loc_t *x;
#define POPLOCATOR(x)		poplocator(&x);
#define RESTORELOCATOR(x)
#define FGW_GLOBALS
#else
#define FGW_GLOBALS		\
TGLOB int status;		\
int int_flag;
#endif	/* I4GLVER */

#if I4GLVER>414 && (I4GLVER<600 || I4GLVER>602)
#ifdef popquote
#undef popquote
#endif
#define popquote popvchar	/* since this is what we really want */
#endif	/* I4GLVER */

#define SETNULLINT(x)		rsetnull(CINTTYPE, x);
/*
** yes there are remnants of 4gl even in the esql/c code!
*/
extern int status;
extern int int_flag;

#endif /* A4GL */

#include "ctypc.h"
#ifdef __STDC__
#define IGNORE (void)
#else
#define IGNORE
#define void int
#define unsigned
#endif

#define CCMPC_H
#endif
