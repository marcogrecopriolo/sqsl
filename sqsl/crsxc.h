/*
	CRSXC.h  -  SQSL interpreter script execution definitions

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

#ifndef CRSXC_H

/*
** compound / control statements
*/
extern int rsx_sqlrun(char *stmt);
extern void rsx_breaks(int state);

extern void rsx_for();
extern int rsx_foreach(pcode_t *fmt);
extern int rsx_setcursor(fgw_stacktype *s);
extern void rsx_while();

extern void rsx_clone(controlstack_t *c);
extern void rsx_parent(controlstack_t *c);
extern void rsx_wait(controlstack_t *c);

extern void rsx_if();
extern void rsx_elif();
extern void rsx_else(controlstack_t *c);

extern void rsx_endfor(controlstack_t *c);
extern void rsx_endforeach(controlstack_t *c);
extern void rsx_endwhile(controlstack_t *c);
extern int rsx_endclone(controlstack_t *c);
extern void rsx_endwait(controlstack_t *c);

extern void rsx_endctrl();

/*
** simple statements
*/
extern void rsx_quit();
extern int rsx_exit();
extern void rsx_dispose();
extern void rsx_display(pcode_t *fmt);
extern void rsx_errorlog();
extern void rsx_evalexpr();	/* most expression related plain statements */
extern void rsx_flagset(int f);
extern void rsx_flagreset(int f);
extern int rsx_load(char *s);
extern int rsx_unload(char *s);
extern void rsx_appenddefault();
extern void rsx_outputto(char *mode);
extern void rsx_pipeto();
extern void rsx_writeto();

/*
** connections
*/
extern int rsx_connect(char *opts);
extern int rsx_defconnect(char *opts);
extern int rsx_setconnection();
extern int rsx_setsource(char *src);
extern int rsx_setdefault();
extern int rsx_disconnect();
extern int rsx_srcdisconnect(char *src);
extern int rsx_defdisconnect();

/*
** temporary (per statement) connections
*/
extern int rsx_curcon(fgw_stacktype *s);
extern int rsx_srcdefault(fgw_stacktype *s);
extern int rsx_curdefault(fgw_stacktype *s);
extern int rsx_tmpcon(fgw_stacktype *s);

/*
** SQL
*/
extern int rsx_prepare(pcode_t *fmt);
extern int rsx_executeout(fgw_stacktype *s);
extern int rsx_executein(fgw_stacktype *s);
extern int rsx_free();
extern int rsx_nonredirected(pcode_t *fmt);
extern int rsx_redirected();
extern int rsx_redirect(fgw_stacktype *s);
extern int rsx_autocommit(fgw_stacktype *s);

/*
** stream execution
*/
extern int rsx_inputfrom(fgw_stacktype *s);
extern int rsx_pipefrom(fgw_stacktype *s);
extern int rsx_readfrom(fgw_stacktype *s);

extern int rsx_rejectfile(fgw_stacktype *s);
extern int rsx_rejectsize(fgw_stacktype *s);

extern int rsx_delimited(fgw_stacktype *s);
extern int rsx_regex(fgw_stacktype *s);
extern int rsx_extractor(fgw_stacktype *s);
extern int rsx_pattern(fgw_stacktype *s);

#define CRSXC_H
#endif
