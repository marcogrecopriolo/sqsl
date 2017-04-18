/*
        CRXXP.h  -  Expression stack runtime execution: allocation and runner

        The 4glWorks application framework
        The Structured Query Scripting Language
        Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

        Initial release: May 00
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

#ifndef CRXXP_H
#include "crxxd.h"
#include "crqxd.h"
#include "csqpc.h"

extern int rxx_runnable();
extern instruction_t *rxx_newinstr(parserstate_t *state);
extern instruction_t *rxx_setjump(parserstate_t *state);
extern void rxx_emptypcode(parserstate_t *state);
extern void rxx_droppcode(pcode_t *pcode);
extern void rxx_swappcode(pcode_t *s1, pcode_t *s2);
extern int rxx_execute(pcode_t *pcode, fgw_stmttype *stmt);

#define CRXXP_H
#endif
