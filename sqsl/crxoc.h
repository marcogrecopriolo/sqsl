/*
	CRXOC.h  -  SQSL interpreter definitions, expression runtime execution

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

#ifndef CRXOC_H

#include "crxuc.h"

extern int rxo_aggrbreak(fgw_stacktype *s);
extern int rxo_aggrcont(fgw_stacktype *s);
extern int rxo_column(fgw_stacktype *s);
extern int rxo_colvalue(fgw_stacktype *s);

extern int rxo_constant(fgw_stacktype *s);
extern int rxo_identifier(fgw_stacktype *s);
extern int rxo_identlist(fgw_stacktype *s);
extern int rxo_variable(fgw_stacktype *s);
extern int rxo_variable2(fgw_stacktype *s);
extern int rxo_dohashstar(fgw_stacktype *s);
extern int rxo_varlist(fgw_stacktype *s);
extern int rxo_delsplit(fgw_stacktype *s);
extern int rxo_patsplit(fgw_stacktype *s);
extern int rxo_regdelsplit(fgw_stacktype *s);
extern int rxo_extsplit(fgw_stacktype *s);
extern int rxo_to(fgw_stacktype *s);
extern int rxo_emptylist(fgw_stacktype *s);
extern int rxo_listcount(fgw_stacktype *s);
extern int rxo_checkfunc(fgw_stacktype *s);
extern int rxo_assign(fgw_stacktype *s);
extern int rxo_assign2(fgw_stacktype *s);
extern int rxo_into(fgw_stacktype *s);
extern int rxo_counthash(fgw_stacktype *s);
extern int rxo_countstar(fgw_stacktype *s);
extern int rxo_whenbool(fgw_stacktype *s);
extern int rxo_whenexpr(fgw_stacktype *s);
extern int rxo_elseexpr(fgw_stacktype *s);
extern int rxo_jump(fgw_stacktype *s);

extern int rxo_pause(fgw_stacktype *s);

extern int rxo_dpipe(fgw_stacktype *s);
extern int rxo_picture(fgw_stacktype *s);
extern int rxo_add(fgw_stacktype *s);
extern int rxo_sub(fgw_stacktype *s);
extern int rxo_mul(fgw_stacktype *s);
extern int rxo_div(fgw_stacktype *s);
extern int rxo_units(fgw_stacktype *s);
extern int rxo_tointeger(fgw_stacktype *s);
extern int rxo_todouble(fgw_stacktype *s);
extern int rxo_todecimal(fgw_stacktype *s);
extern int rxo_todate(fgw_stacktype *s);
extern int rxo_todatetime(fgw_stacktype *s);
extern int rxo_tointerval(fgw_stacktype *s);
extern int rxo_tostring(fgw_stacktype *s);
extern int rxo_tobyte(fgw_stacktype *s);
extern int rxo_plus(fgw_stacktype *s);
extern int rxo_minus(fgw_stacktype *s);

extern int rxo_in(fgw_stacktype *s);
extern int rxo_inend(fgw_stacktype *s);
extern int rxo_ni(fgw_stacktype *s);
extern int rxo_niend(fgw_stacktype *s);
extern int rxo_between(fgw_stacktype *s);
extern int rxo_matches(fgw_stacktype *s);
extern int rxo_like(fgw_stacktype *s);
extern int rxo_regexp(fgw_stacktype *s);
extern int rxo_and(fgw_stacktype *s);
extern int rxo_or(fgw_stacktype *s);
extern int rxo_not(fgw_stacktype *s);
extern int rxo_isnull(fgw_stacktype *s);
extern int rxo_eq(fgw_stacktype *s);
extern int rxo_ne(fgw_stacktype *s);
extern int rxo_gt(fgw_stacktype *s);
extern int rxo_ge(fgw_stacktype *s);
extern int rxo_lt(fgw_stacktype *s);
extern int rxo_le(fgw_stacktype *s);

#define CRXOC_H
#endif
