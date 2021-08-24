/*
	CEXPC.h  -  SQSL interpreter: expansion facility defines

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)

	Initial release: May 05
	Current release: Aug 21

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

#include "csqpc.h"

#define TOK_READ 1
#define TOK_EXEC 2
#define TOK_SELECT 3
#define TOK_GET 4
#define TOK_PROMPT 5
#define TOK_PASSWD 6

#define TOK_PUT 7

#define TOK_INCLUDE 0x10
#define TOK_SILENT 0x20
#define TOK_CODED 0x100
#define TOK_LONE 0x200

#define TOK_QUOTES 0x40
#define TOK_SEPARATOR 0x80
#define TOK_SINGLE 0x1000
#define TOK_MULTI 0x2000

#define MOD_OP   0x00f0
#define MOD_COLS 0x0f00
#define MOD_ROWS 0xf000
#define MOD_OPTS 0xfff0

extern int fgw_sqlexplode(char *src, int srclen, fgw_loc_t *vars, int verbose, int expstate,
                   int iidx, int *oidx, char **expptr, int *s, int *e, int *n, execinfo_t *execinfo);
