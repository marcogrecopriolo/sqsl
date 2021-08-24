/*
	CSQUC.c  -  SQSL interpreter 4gl user interface

	The Structured Query Scripting Language
	(Borrowed from) The 4glWorks application framework
	Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
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

#include <sqlca.h>
#include <sqltypes.h>

#include "ccmpc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "cexpc.h"

/* FIXME: we require empty stubs for usr_warn & usr_confirm if
   used within cgi scripts */

/*
** display no of affected rows
*/
void sqsl_numrows(int rows, execinfo_t *execinfo)
{
    char txt1[256], txt2[256];

    pushquote("csql.txt", 8);
    pushint(SQ_TOSQLCODE(RC_NROWS));
    fgl_call(txt_fetch, 2);
    popquote((char *) &txt1, 255);
    fgw_clip(txt1);
    sprintf(txt2, txt1, rows);
    pushquote(txt2, strlen(txt2));
    fgl_call(usr_notify, 1);
}

/*
** returns an error message text
*/
void sqsl_getmessage(int e, char *b, int l)
{
    char *p, m[256];

/* FIXME: using 4gl user interaction */
	pushquote("csql.txt", 8);
	pushint(e);
	fgl_call(txt_fetch, 2);
	popquote(b, l);
	fgw_clip(b);
}

/*
**  confirm database operation
*/
int sqsl_asktouch(int touch, execinfo_t *execinfo)
{
    int r;

    if (touch==K_dml)
    {
	pushquote("cdbm.nq", 7);
	fgl_call(usr_confirm, 1);
	popint(&r);
	return r;
    }
    else if (touch==K_dml+K_noconf)
	return 1;
    else
    {
	pushquote("cdbm.nq", 7);
	fgl_call(usr_warn, 1);
	return 0;
    }
}

/*
**  expansion facility prompt/passwd functionality
*/
int sqsl_promptpasswd(int tok, int opts, char *txt, char *e_buf, int *len, int verbose, execinfo_t *execinfo)
{
    pushint(tok==TOK_PROMPT);
    pushquote(txt, strlen(txt));
    pushint(verbose);
    fgl_call(prompt_sql, 3);
    popquote(e_buf, *len);
    if (int_flag)
	return RC_UINTR;
    fgw_clip(e_buf);
    *len-=strlen(e_buf);
    return 0;
}

/*
** expansion facility pick list functionality
*/
int sqsl_picklist(int tok, int opts, char *txt, char *e_buf, int *len, char *sep, char *quotes, int verbose, execinfo_t *execinfo)
{
    char *q_mode;

    switch (tok)
    {
      case TOK_SELECT:
	q_mode="sql";
	break;
      case TOK_READ:
	q_mode="file";
	break;
      case TOK_EXEC:
	q_mode="pipe";
	break;
    }
    pushint(opts & (MOD_ROWS|MOD_COLS));
    pushquote(q_mode, strlen(q_mode));
    pushquote(txt, strlen(txt));
    pushint(*len); 
    pushquote(sep, strlen(sep));
    pushquote(quotes, strlen(quotes));
    pushint(verbose);
    fgl_call(picklist_sql, 7);
    popquote(e_buf, *len);
    if (status==SQLNOTFOUND)
	return RC_NDATA;
    if (status)
	return status;
    if (!strlen(e_buf))
	return RC_UINTR;
    fgw_clip(e_buf);
    *len-=strlen(e_buf);
    return 0;
}
