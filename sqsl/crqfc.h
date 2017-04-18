/*
	CRQFC.h  -  SQSL interpreter output/format support functions definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Jan 97
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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef CRQFC_H
#include "crxuc.h"
#include "crqxd.h"

#define MINWIDTH 10
#define MAXWIDTH 256
#define MAXNUMLEN 256

#define FMT_NULL -1
#define FMT_BRIEF 1
#define FMT_FULL 2
#define FMT_VERTICAL 3
#define FMT_DELIMITED 4

extern void rqf_fmtclear(fgw_fmttype *h);

extern void rqf_openfile(fgw_loc_t *txtvar, fgw_fdesc *fd);
extern void rqf_flush(fgw_loc_t *txtvar, int all);

extern void rqf_setformat(fgw_stmttype *st_p, char *f);
extern void rqf_setheader(fgw_stmttype *st_p, char *c);

extern void rqf_countfields(fgw_stmttype *st_p, int countfields, int clear);
extern int rqf_fieldcount();

extern void rqf_setwidth(fgw_stmttype *st_p, int w);

extern void rqf_dorows(fgw_stmttype *st_p, fgw_loc_t *textvar, int finish);
extern void rqf_display(fgw_stmttype *st_p, fgw_loc_t *textvar, exprstack_t *e,
			int c);

#define CRQFC_H
#endif
