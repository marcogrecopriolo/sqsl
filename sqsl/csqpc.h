/*
	CSQPC.h  -  SQSL interpreter definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
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

#ifndef CSQPC_H
#include "ccmpc.h"
#include "ccmnc.h"
#include "cfioc.h"
#include "ctypc.h"
#include "crqxd.h"
#include "crxuc.h"

#define FNAMESIZE	256
#define ERRMSGLEN	256

#define PUREPARSER

typedef struct errorinfo
{
    int line_count;
    char near[ERRMSGLEN];
    char extra[ERRMSGLEN];
} errorinfo_t;

typedef struct execinfo
{
    errorinfo_t errorinfo;
    int verbose;
} execinfo_t;

typedef struct phase1state
{
    int input_char;
    int prev_input_char;
    int stmt_start;
    int next_stmt_start;
    int output_buffer_len;
    int line_count;
    int expcount;
    int expstart;
} phase1state_t;

typedef struct controlstack
{
    int stmt_type;
    int state;
    int flags;
    fgw_stmttype *stmt;
    int style;
    fgw_fdesc *curr_fd;
    int text_start;
    int text_end;
    pcode_t destvar;
    pcode_t clone;
    int element;
    int forcount;
    phase1state_t phase1;
    phase1state_t nextphase1;
} controlstack_t;

/*
** statement types
*/
#define T_IF		 1
#define T_FOR		 2
#define T_WHILE		 3
#define T_FOREACH	 4
#define T_CLONE		 5
#define T_WAIT		 6
#define T_FUNCTION	 7
#define	T_PROCEDURE	 8
#define T_IMMEDIATE	 9
#define T_COMPOUND	10

/*
** control stack states
** assumption is that states where expansions are allowed are positive
*/
#define S_WHILE		 5
#define	S_IMMEDIATE	 4
#define S_COMPOUND	 3
#define S_INPROC	 2
#define S_ACTIVE	 1
#define S_DISABLED	 0
#define S_DISPROC	-2
#define S_DISCOMP	-3
#define S_DISIMM	-4
#define S_CONTINUE	-5
#define S_IF		-6
#define S_DISCLD	-7
#define S_DISPAR	-8

/*
** control stack styles
*/
#define F_NONE		0
#define F_PRE		1
#define F_TABLE		2

/*
** control stack flags
*/
#define CS_ISCHILD	1
#define CS_FORKED	2

typedef struct parsestack
{
    int type;
    instruction_t *instr;
} parsestack_t;

/*
** sql move op: statement stack
*/
typedef struct {
    fgw_stmttype *stmt;
    int line_count;
    int sqlstart;
    int sqlend;
    char sqlsv;
} stmtstack_t;

#define STMTSTACKSZ 2

typedef struct parserstate
{
    phase1state_t phase1;	/* outer layer */
    phase1state_t prevphase1;
    char *i_query;
    char *o_query;
    int i_size;
    int o_size;

    int expcount;		/* expansion facility */
    int expstate;

    int ibufp;			/* scanner & parser buffers */
    int tokstart;
    int prevtoken;		/* we look ahead 2 */
    int svyychar;		/* for reentrant parser calls */
    void *prevlval;
    void *svyylval;

    errorinfo_t *errorinfo;	/* error reporting */
    int prevtokstart;
    int toklen;
    int prevtoklen;

    int states;			/* various parser/runner states */
    int parseroptions;		/* used for the parser hacks */
    int runneroptions;		/* used to turn on features at runtime */
    fgw_tsstype *lexhead;	/* scanner temporary storage space */
    fgw_tsstype *exphead;	/* per expression temporary storage space */

    fgw_stacktype parsestack;	/* parser jump stack */
    pcode_t pcode;		/* pcode generation storage */
    instruction_t *instrp;	/* current instruction pointer */

    int returncode;		/* child return code */
    fgw_stacktype control;	/* control stetement stack */
    fgw_stacktype exprlist;	/* expression stack */

    stmtstack_t stmtstack[STMTSTACKSZ];	/* statement redirection */
    stmtstack_t *ssp;		/* always points to output side
				   used by output clauses */
    int stmtcnt;
    fgw_stmttype *curstmt;	/* always points to input side
				   used by input side */
    
    fgw_loc_t *vars;		/* storage space */
    int signature;

    ca_t ca;			/* sql */
    ca_t oserr;			/* os - can't use errno */
    int childstatus;		/* last terminated child return code */

    char errorlog[FNAMESIZE];	/* location of error log if any */
    fgw_fdesc *def_fd;		/* pointer to default stream: see below
				   for special values */
    int flags;			/* ever present */
    int touch;

    int width;
    int style;
    int newstyle;

    fgw_fdesc *curr_fd;		/* same meaning as def_fd */
    fgw_loc_t *txtvar;
    void *sighlr;
} parserstate_t;

/*
** expansion states
*/
#define ES_DISABLED		0
#define ES_VERBOSE		1
#define ES_SILENT		-1

/*
** parser options
*/
#define EXPRONLY		0x10000000	/* evaluate an expression */
#define EXPRLIST		0x20000000	/* evaluate an expression list */
#define VARONLY			0x40000000	/* ... a target variable */
#define VASONLY			0x80000000	/* ... an assignment */

/*
** parser & runner states
*/
#define PS_DONEDISPLAY		    0x0001
#define PS_ISCHILD		    0x0002
#define PS_ISREENTRANT		    0x0004

/*
** flags
*/
#define PF_VERBOSE		    0x0001
#define PF_ISHTML		    0x0002
#define PF_QUIT			    0x0004
#define PF_ERRORCONT		    0x0008
#define PF_INJECTION		    0x0010

/*
** special fds
*/
#define FD_TOBLOB	((fgw_fdesc *) NULL)
#define FD_IGNORE	((fgw_fdesc *) -1)

/*
** fgw_sqlexec input flags
*/
#define K_verbose	    	    0x0001
#define K_html		    	    0x0002
#define K_errorcont	    	    0x0004
#define K_dml		    	    0x0008
#define K_noconf	 	    0x0010
#define K_preserve	 	    0x0020
#define K_connection	 	    0x0040
#define K_quit		 	    0x0080
#define K_isbatch		    0x0100

/*
** query access
*/
#define TOKPTR(s) (pstate->o_query+(s))

/*
** sqsl parser
*/
extern int fgw_sqlexec(char *i_query, int i_size, int def_fd, int flags, int width,
		       fgw_loc_t *vars, fgw_loc_t *txtvar, execinfo_t *execinfo);
extern void fgw_sqlrelease(fgw_loc_t *vars);
extern int fgw_evalassign(char *instr, fgw_loc_t *vars);
extern void fgw_errmsg(int e, errorinfo_t *ei, char *b, int l);

extern void sql_postcmd(parserstate_t *pstate);
extern int sql_execcmd(parserstate_t *pstate);
extern int sql_parser(parserstate_t *state);
extern void sql_parserinit(parserstate_t *state);
extern void sql_parserfree(parserstate_t *state);
extern void sql_parsersave(parserstate_t *state);
extern void sql_parserrestore(parserstate_t *state);

/*
** external user interaction
*/
extern void sqsl_numrows(int rows);
extern void sqsl_getmessage(int e, char *b, int l);
extern int sqsl_asktouch(int touch);
extern int sqsl_promptpasswd(int tok, int opts, char *txt,
			     char *e_buf, int *len, int verbose);
extern int sqsl_picklist(int tok, int opts, char *txt, char *e_buf,
			 int *len, char *sep, char *quotes, int verbose);

#define CSQPC_H
#endif
