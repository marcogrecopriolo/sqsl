/*
	CRXFC.h  -  SQSL interpreter definitions, expression runtime execution

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

#ifndef CRXFC_H

#include "crxuc.h"

/*
** known functions
*/
extern int rxf_columns(fgw_stacktype *s);

extern int rxf_displaylabels(fgw_stacktype *s);
extern int rxf_displayformat(fgw_stacktype *s);

extern int rxf_filetotext(fgw_stacktype *s);
extern int rxf_filetoblob(fgw_stacktype *s);

extern int rxf_ascii(fgw_stacktype *s);
extern int rxf_clipped(fgw_stacktype *s);
extern int rxf_spaces(fgw_stacktype *s);

extern int rxf_sleep(fgw_stacktype *s);
extern int rxf_run(fgw_stacktype *s);
extern int rxf_spawn(fgw_stacktype *s);
extern int rxf_exec(fgw_stacktype *s);
extern int rxf_waitpid(fgw_stacktype *s);
extern int rxf_wait(fgw_stacktype *s);
extern int rxf_fork(fgw_stacktype *s);
extern int rxf_fopen(fgw_stacktype *s);
extern int rxf_popen(fgw_stacktype *s);
extern int rxf_sclose(fgw_stacktype *s);
extern int rxf_getenv(fgw_stacktype *s);
extern int rxf_eval(fgw_stacktype *s);

extern int rxf_index(fgw_stacktype *s);
extern int rxf_match(fgw_stacktype *s);
extern int rxf_pad(fgw_stacktype *s);
extern int rxf_random(fgw_stacktype *s);

extern int rxf_current(fgw_stacktype *s);
extern int rxf_today(fgw_stacktype *s);
extern int rxf_user(fgw_stacktype *s);

extern int rxf_decode(fgw_stacktype *s);
extern int rxf_nvl(fgw_stacktype *s);

extern int rxf_abs(fgw_stacktype *s);
extern int rxf_mod(fgw_stacktype *s);
extern int rxf_pow(fgw_stacktype *s);
extern int rxf_root(fgw_stacktype *s);
extern int rxf_round(fgw_stacktype *s);
extern int rxf_sqrt(fgw_stacktype *s);
extern int rxf_trunc(fgw_stacktype *s);

extern int rxf_dbinfo(fgw_stacktype *s);
extern int rxf_sqlcode(fgw_stacktype *s);
extern int rxf_sqlerrd1(fgw_stacktype *s);
extern int rxf_sqlerrd2(fgw_stacktype *s);
extern int rxf_errno(fgw_stacktype *s);

extern int rxf_exp(fgw_stacktype *s);
extern int rxf_log10(fgw_stacktype *s);
extern int rxf_logn(fgw_stacktype *s);
extern int rxf_hex(fgw_stacktype *s);

extern int rxf_charlength(fgw_stacktype *s);
extern int rxf_length(fgw_stacktype *s);

extern int rxf_datetime(fgw_stacktype *s);
extern int rxf_interval(fgw_stacktype *s);
extern int rxf_date(fgw_stacktype *s);
extern int rxf_day(fgw_stacktype *s);
extern int rxf_month(fgw_stacktype *s);
extern int rxf_year(fgw_stacktype *s);
extern int rxf_weekday(fgw_stacktype *s);
extern int rxf_mdy(fgw_stacktype *s);

extern int rxf_cos(fgw_stacktype *s);
extern int rxf_sin(fgw_stacktype *s);
extern int rxf_tan(fgw_stacktype *s);
extern int rxf_acos(fgw_stacktype *s);
extern int rxf_asin(fgw_stacktype *s);
extern int rxf_atan(fgw_stacktype *s);
extern int rxf_atan2(fgw_stacktype *s);

extern int rxf_substr(fgw_stacktype *s);
extern int rxf_replace(fgw_stacktype *s);
extern int rxf_lpad(fgw_stacktype *s);
extern int rxf_rpad(fgw_stacktype *s);

extern int rxf_lower(fgw_stacktype *s);
extern int rxf_upper(fgw_stacktype *s);
extern int rxf_initcap(fgw_stacktype *s);

#define CRXFC_H
#endif
