/*
	CRQXP.h  -  SQSL interpreter dynamic query execution definitions

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

#ifndef CRQXP_H
#include "ccmnc.h"
#include "cfioc.h"
#include "crxuc.h"
#include "crqxd.h"
#include "csqpc.h"
#include "ctypc.h"

extern int rqx_rc(int rc, int src_id);
extern void rqx_sendbreak();
extern fgw_objtype *rqx_getobject(parserstate_t *pstate, char *s,  char *n);
extern int rqx_init(parserstate_t *pstate, int lastcon);
extern void rqx_connect(parserstate_t *pstate, char *host, char *source,
			char *as, char *user, char *passwd, char *opt);
extern void rqx_setconnection(parserstate_t *pstate, char *cname, char *source);
extern void rqx_disconnect(parserstate_t *pstate, char *cname, char *source);
extern void rqx_setstmtcon(fgw_stmttype *st_p, char *as, char *source,
			   parserstate_t *pstate);
extern char *rqx_getconnection();
extern char *rqx_getsource();
extern fgw_stmttype *rqx_findstmt(int st_id, int s);
extern int rqx_registerstatement(parserstate_t *pstate, fgw_stmttype *st_n);
extern fgw_stmttype *rqx_newstatement(parserstate_t *pstate);
extern void rqx_freestatement(fgw_stmttype *st_p);
extern void rqx_closestatement(fgw_stmttype *st_p);
extern void rqx_zapstmt(int signature);
extern void rqx_setcursor(fgw_stmttype *st_p, char *curs);
extern void rqx_run(char *i_query, fgw_stmttype *st_p, int touch);
extern void rqx_prepare(fgw_stmttype *st_p, char *query);
extern void rqx_immediate(fgw_stmttype *st_p);
extern void rqx_move(stmtstack_t *ss_1, stmtstack_t *ss_2, errorinfo_t *ei);
extern void rqx_nextrow(fgw_stmttype *st_p);
extern exprstack_t *rqx_nexttoken(int field, fgw_stmttype *st_p);
extern void rqx_describecol(fgw_stmttype *st_p, int f, exprstack_t *e);
extern int rqx_counttokens(fgw_stmttype *st_p);
extern void rqx_errmsg(int s, int e, char *b, int l);
extern void rqx_rejcopy(fgw_stmttype *st, char *source, int len);
extern void rqx_rejdump(fgw_stmttype *st);
 
#define CRQXP_H
#endif
