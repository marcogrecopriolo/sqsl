/*
	CRXVC.h  -  SQSL interpreter definitions, expression runtime execution

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

#ifndef CRXVC_H
#include "ccmnc.h"
#include "crxuc.h"
#include "ctypc.h"

extern void sqv_setvar(fgw_loc_t *h, exprstack_t *e, exprstack_t *v);
extern void sqv_newvar(fgw_loc_t *h, exprstack_t *e, char *l, exprstack_t *v);
extern int sqv_cvvar(exprstack_t *s, hst_t v, int safecopy);
extern int sqv_evalhash(fgw_stacktype *s, hst_t *hid);

#define CRXVC_H
#endif
