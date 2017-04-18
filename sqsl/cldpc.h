/*
	CLDPC.h  -  SQSL stream input functions, all types

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Jan 13
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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#ifndef CLDPC_H
#include "crqxd.h"
#include "crxuc.h"

extern int lde_decode(char *source, int *currfield, exprstack_t *expr, fgw_tsstype **tss);
extern void ldp_patternfree(char *pat);
extern int ldp_patterncomp(char *source, fgw_tsstype **tss, char **dest);
extern int ldp_patternexec(char **patp, char *source, int *currfield,
			   exprstack_t *expr, fgw_tsstype **tss);

extern int ldd_newstatement(fgw_stmttype *st, char *d, char *e);
extern int ldr_newstatement(fgw_stmttype *st, char *d);
extern int lde_newstatement(fgw_stmttype *st, char *m, char *e);
extern int ldp_newstatement(fgw_stmttype *st, char *d);

#define CLDPC_H
#endif
