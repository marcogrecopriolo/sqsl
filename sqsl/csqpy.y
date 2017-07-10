%{
/*
	CSQPY.y  -  SQSL interpreter parser and scanner

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
	Current release: Jun 17

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

#include "coslc.h"
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "ccmnc.h"
#include "cfioc.h"
#include "cfnmc.h"
#include "chstc.h"
#include "cexpc.h"
#include "cldpc.h"
#include "crqfc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqxp.h"
#include "crsxc.h"
#include "crxuc.h"
#include "crxcc.h"
#include "crxxp.h"
#include "crxfc.h"
#include "crxoc.h"
#include "crxvc.h"

#ifndef YYBISON
#define yyparse yyparse_sqp     /* just in case we decide to have */
#define yylex yylex_sqp         /* multiple parsers */
#define yyerror yyerror_sqp
#define yyval yyval_sqp
#define yylval yylval_sqp
#define yyvs yyvs_sqp
#define yycheck yycheck_sqp
#define yychar yychar_sqp
#define yylhs yylhs_sqp
#define yylen yylen_sqp
#define yydefred yydefred_sqp
#define yydgoto yydgoto_sqp
#define yysindex yysindex_sqp
#define yyrindex yyrindex_sqp
#define yygindex yygindex_sqp
#define yytable yytable_sqp
#endif

/* bison tends to complain about missing ';' for macros
   at the end of a rule - don't be surprised if you see
   random ';', they are there for a reason!
*/
#define FAIL(e)		{ \
			    status=e; \
			    yyerror(""); \
			    YYERROR; \
			}

#define FAILSYN		{ \
			    yyerror(""); \
			    YYERROR; \
			}

#define FAILCHECK	if (status) \
			{ \
			    yyerror(""); \
			    YYERROR; \
			}

#define FAILTEST(a)	if (status==a) \
			{ \
			    yyerror(""); \
			    YYERROR; \
			}

#define NUMTOKENS (sizeof(tokenlist)/sizeof(tokenlist_t))

/*
** function & reserved words descriptor list
*/
typedef struct tokenlist
{
    char *tl_name;
    int (*tl_func)();
    int tl_type;
    int tl_alttype;
    int tl_parmcount;
    struct tokenlist *tl_next;
} tokenlist_t;

/*
** parser kludges
*/
#define NOOPTION	0x0000
#define STMTHACK	0x0001
#define EXPRHACK	0x0002
#define LASTHACK	0x0004

#define PREPAREHACK	0x0008
#define DMLHACK		0x0010
#define SELECTHACK	0x0020
#define SQLHACK		0x0040
#define FOREACHHACK	0x0080

#define DTHACK		0x0200
#define FUNCHACK	0x0400

#define RESVARHACK	0x0800
#define SAFECOPYHACK	0x1000

#define AGGREGATEHACK	0x2000
#define VALUESHACK	0x4000

/*
** parser and scanner state
*/
TGLOB parserstate_t *pstate;
static int delimident=-1;

static TGLOB int (*thischar)();

#ifdef PUREPARSER
#define BUFSIZE 32768
#define NEXT_STMT	{ \
			    draintokens_sql(); \
			    YYACCEPT; \
			}
#define MARK_STMT	{ \
			    draintokens_sql(); \
			    markend_sql(pstate->ibufp-1); \
			}
#define DRAIN_STMT	{ \
			    draintokens_sql(); \
			}
#else
#define	NEXT_STMT	YYACCEPT;
#define MARK_STMT
#define DRAIN_STMT
#endif

#define STMTPTR(s) (TOKPTR((s)->sqlstart))

/*
** forward declarations
**
** others
*/
static void markend_sql(int c);
static void endstmt_sql();

/*
** parser
*/
#ifdef PUREPARSER
static void draintokens_sql();
static int fillbuffer_sql();
static int expthischar_sql();
#endif
static int thischar_sql();
static char *getstring_sql(fgw_tsstype **h);
static char *getvar_sql(fgw_tsstype **hd);
static int getdigits_sql(int base);
static int getfloat_sql(int i, double *f);
static int scanner_sql();
static void yyerror(char *s);
static int yylex();

%}

%union {
char		*var;		/* variable names */
tokenlist_t	*fntok;		/* function descriptors */
int 		(*func)();	/* function pointers */
pcode_t		pcode;		/* executable expression code */
instruction_t	instr;		/* intermediate instructions */
instruction_t	*insptr;	/* intermediate instruction pointers */
exprstack_t	stack;		/* expression result */
int 		tstart;		/* token start in source - for sql hacks */
fgw_fdesc	*fd;		/* output statements */
}

%type <fd>	append
%type <stack>	user as con con1 src
%type <tstart>	conopt
%type <var>	fselect fsel1 cursor
%type <instr>	assign4
%type <stack>	patcl pat1
%type <stack>	aggas6
%type <tstart>  dstcl1 dstcl2
%type <pcode>	format ftype
%type <stack>	retlist retl1 retl2
%type <stack>	fnproc done
%type <fntok>	fncl1
%type <insptr>	flist
%type <stack>	alist
%type <instr>	ovarcl1 ivarcl dvarcl
%type <stack>	ivarcl1
%type <instr>	rangecl
%type <insptr>	t_elstc
%type <stack>	t_elist t_elst1 t_elst3
%type <fntok>	t_elst2
%type <stack>	incl nullcl unitcl unitcls unitcle fractcl
%type <instr>	qualcl
%type <instr>	typecl
%type <stack>	castql
%type <stack>	betweencl mrexcl lrexcl rrexcl
%type <tstart>	draincl

%token <tstart>	END INSERT SELECT EXECUTE CALL INTO USING FORMAT
%token <stack>	CNST
%token <var>	VAR
%token <fntok>	FNCT FNCS	/* plain & system functions */
%token <fntok>	FNCL FNCV	/* list functions and pseudo variables */
%token <fntok>	FNDP FNDN	/* need to differentiate between datetime
				   functions with & without args */
%token <fntok> PFIXFNCT

/* this flags an error from the scanner */
%nonassoc TOK_ERROR

/* these are used to force different parsing modes, and ensure
   that all tokens have been shifted before executing commands */
%nonassoc XPRTOK LSTTOK VARTOK VASTOK LASTOK

%nonassoc BEGIN IMMEDIATE COMPOUND S_END
%nonassoc QUIT
%nonassoc FOR FOREACH WHILE CLONE PARENT WAIT IF ELIF ELSE FI THEN DO DONE C_END
%nonassoc CONTINUE BREAK EXIT
%nonassoc DISPLAY LET DISPOSE INVOKE STORE LOAD UNLOAD WHENEVER
%nonassoc PIPE APPEND OUTPUT WRITE INPUT READ WIDTH REJECT BLOCKSIZE
%nonassoc SELECT INSERT DELETE UPDATE EXECUTE CREATE END PREPARE FREE
%nonassoc CONNECT DISCONNECT S_SET CONNECTION AS USER SOURCE
%nonassoc CURSOR WITH HOLD
%nonassoc SQLTOK PROCEDURE FUNCTION
%nonassoc ERROR LOG STOP DEFAULT PLAIN HTML RETURNING
%nonassoc IN AGGREGATE AUTOCOMMIT INTO USING FORMAT PATTERN
%nonassoc EXTRACTOR FROM TO BY DELIMITER
%nonassoc BRIEF VERTICAL FULL DELIMITED REGEXP HEADERS
%nonassoc TEMP SCRATCH EXTERNAL
%nonassoc SPLIT COUNT COLUMN PICTURE CASE WHEN IS NULLVALUE
%nonassoc BETWEEN MATCHES LIKE ESCAPE
%nonassoc KWCAST TYPEINT TYPEFLOAT TYPEDEC TYPEMONEY TYPEDTIME TYPEINTVL TYPEDATE
%nonassoc TYPESTRING TYPEBYTE
%nonassoc YEAR MONTH DAY HOUR MINUTE SECOND FRACTION

%right ASSIGN
%right PFIXFNCT
%left OR
%left AND
%nonassoc '=' EQ NEQ '<' LEQ '>' GEQ
%right NOT
%left DPIPE
%left '-' '+'
%left '*' '/'
%nonassoc OPCAST
%left NEG PLUS
%left UNITS
%right '$'

/*
** Grammar
*/

%% 

/* FIXME: we need to be more intelligent with the RT_CAN.... flags and disable
   system functions inside hashes, substrings and possibly AGGREGATE clauses */
target:   XPRTOK { pstate->runneroptions|=RT_CANRUNSLEEP; } t_aexp	{
/*
** reposition lex pointer to first unparsed char
*/
			    if (yychar==0)
				pstate->ibufp--;
			    else if (yychar>0)
				pstate->ibufp=pstate->tokstart;
			    status=rxx_execute(NULL, NULL);
			    FAILCHECK
			    YYACCEPT;
			}
	| LSTTOK t_elstc	{
/*
** reposition lex pointer to first unparsed char
*/
			    if (yychar==0)
				pstate->ibufp--;
			    else if (yychar>0)
				pstate->ibufp=pstate->tokstart;
			    status=rxx_execute(NULL, NULL);
			    FAILCHECK
			    YYACCEPT;
			}
	| VARTOK ovarcl	{
/*
** reposition lex pointer to first unparsed char
*/
			    if (yychar==0)
				pstate->ibufp--;
			    else if (yychar>0)
				pstate->ibufp=pstate->tokstart;
			    status=rxx_execute(NULL, NULL);
			    FAILCHECK
			    YYACCEPT;
			}
	| VASTOK assign	{
/*
** reposition lex pointer to first unparsed char
*/
			    if (yychar==0)
				pstate->ibufp--;
			    else if (yychar>0)
				pstate->ibufp=pstate->tokstart;
			    status=rxx_execute(NULL, NULL);
			    FAILCHECK
			    YYACCEPT;
			}
	| {
		pstate->runneroptions|=(RT_CANRUNSLEEP|RT_CANSYSTEM);
	  } stmt
;
/*
** pay attention to the kludges in yylex! we essentially look ahead 2 tokens
** to substantially simplify actions on a whole host of END, S_SET and
** EXECUTE statements
*/
stmt:     S_END COMPOUND LASTOK {
			    if (!pstate->control.count)
				FAILSYN;
			    switch (((controlstack_t *)
					fgw_stackpeek(&pstate->control))->state)
			    {
			      case S_COMPOUND:
			      case S_DISCOMP:
				break;
			      default:
				FAILSYN;
			    }
			} newstcl tmpcon {
			    controlstack_t *c;

			    if ((c=((controlstack_t *)
				fgw_stackpeek(&pstate->control)))->state)
			    {
				pstate->ssp->sqlend=c->text_end;
				endstmt_sql();
				if (rsx_sqlrun(TOKPTR(c->text_start)))
				    FAILCHECK
			    }
			    rsx_endctrl();
			}
	| S_END IMMEDIATE LASTOK	{
			    if (!pstate->control.count)
				FAILSYN;
			    switch (((controlstack_t *)
					fgw_stackpeek(&pstate->control))->state)
			    {
			      case S_DISIMM:
			      case S_IMMEDIATE:
				break;
			      default:
				FAILSYN;
			    }
			    rsx_endctrl();
			}
	| BEGIN IMMEDIATE lastok	{
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    if (pstate->control.count)
				switch (((controlstack_t *)
				    fgw_stackpeek(&pstate->control))->state)
				{
				  case S_ACTIVE:
				    c.state=S_IMMEDIATE;
				    break;
				  case S_IMMEDIATE:
				  case S_DISIMM:
				  case S_COMPOUND:
				  case S_DISCOMP:
				  case S_INPROC:
				  case S_DISPROC:
				    FAILSYN;
				    break;
				  default:
				    c.state=S_DISIMM;
				}
			    else
				c.state=S_IMMEDIATE;
			    fgw_stackpush(&pstate->control, (char *) &c);
			} lastok1
	| BEGIN COMPOUND lastok	{
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    if (pstate->control.count)
				switch (((controlstack_t *)
				    fgw_stackpeek(&pstate->control))->state)
				{
				  case S_ACTIVE:
				    c.state=S_COMPOUND;
				    break;
				  case S_IMMEDIATE:
				  case S_DISIMM:
				  case S_COMPOUND:
				  case S_DISCOMP:
				  case S_INPROC:
				  case S_DISPROC:
				    FAILSYN;
				    break;
				  default:
				    c.state=S_DISCOMP;
				}
			    else
				c.state=S_COMPOUND;
			    fgw_stackpush(&pstate->control, (char *) &c);
			} lastok1
	| LASTOK
	| {
	    int s;
	    instruction_t *i;
	    controlstack_t *c;

	    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
	    s=pstate->control.count? c->state: S_ACTIVE;
	    switch (s)
	    {
	      case S_IMMEDIATE:
		MARK_STMT
		endstmt_sql();
		if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
		    FAIL(RC_NOMEM)
		i=rxx_newinstr(pstate);
		FAILCHECK
		i->func=rsx_curcon;
		if (rsx_sqlrun(STMTPTR(pstate->ssp)))
		    FAILCHECK
		YYACCEPT;
	      case S_COMPOUND:
		MARK_STMT

		/* accrue beginning and end as we go along, and leave the statement unmarked */
		if (!c->text_start)
		    c->text_start=pstate->ssp->sqlstart;
		c->text_end=pstate->ssp->sqlend;
		pstate->ssp->sqlend=0;
		YYACCEPT;
	      case S_DISIMM:
	      case S_DISCOMP:
		NEXT_STMT
		break;
	    }
	  } stmt1
;
stmt1:	  QUIT LASTOK	{
			    if (pstate->flags & PF_QUIT)
				rsx_quit();
			    else
				FAILSYN;
			}
	| END fnproc	{
			    if (!pstate->control.count)
				FAILSYN;
			    if (((controlstack_t *)
				fgw_stackpeek(&pstate->control))->stmt_type!=
				$2.type)
				FAILSYN;
			} newstcl tmpcon LASTOK	{
			    controlstack_t *c;

			    if ((c=((controlstack_t *)
				fgw_stackpeek(&pstate->control)))->state)
			    {
				if (rsx_sqlrun(TOKPTR(c->text_start)))
				    FAILCHECK
			    }
			    rsx_endctrl();
			}
	| CREATE fnproc draincl {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    c.stmt_type=$2.type;
			    if (pstate->control.count)
				switch (((controlstack_t *)
				    fgw_stackpeek(&pstate->control))->state)
				{
				  case S_ACTIVE:
				    c.state=S_INPROC;
				    break;
				  case S_INPROC:
				  case S_DISPROC:
				    FAILSYN;
				    break;
				  default:
				    c.state=S_DISPROC;
				}
			    else
				c.state=S_INPROC;
			    c.text_start=$<tstart>1;
			    fgw_stackpush(&pstate->control, (char *) &c);
			}
	| {
	    int s;

	    s=pstate->control.count? ((controlstack_t *)
		fgw_stackpeek(&pstate->control))->state: S_ACTIVE;
	    switch (s)
	    {
	      case S_INPROC:
	      case S_DISPROC:
		NEXT_STMT
	    }
	  } stmt2
;
fnproc:	  FUNCTION	{ $$.type=T_FUNCTION; }
	| PROCEDURE	{ $$.type=T_PROCEDURE; }
;
/*
** control statements: terminators
*/
stmt2:	  {
/*
** if we don't have to do anything, we don't parse, just accept
*/
	    if (pstate->control.count &&
		((controlstack_t *) fgw_stackpeek(&pstate->control))->state<
							S_ACTIVE)
	    {
		controlstack_t c;

		memset(&c, 0, sizeof(c));
		switch (yychar)
		{
		  case IF:
		    c.stmt_type=T_IF;
		    break;
		  case WHILE:
		    c.stmt_type=T_WHILE;
		    break;
		  case FOR:
		    c.stmt_type=T_FOR;
		    break;
		  case FOREACH:
		    c.stmt_type=T_FOREACH;
		    break;
		  case CLONE:
		    c.stmt_type=T_CLONE;
		    break;
		  case WAIT:
		    c.stmt_type=T_WAIT;
		    break;
		}
		c.state=S_DISABLED;
		fgw_stackpush(&pstate->control, (char *) &c);
		NEXT_STMT
	    }
	  } ctrl
	| done LASTOK	{
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if ($1.type!=DONE && $1.type!=c->stmt_type)
				FAILSYN
			    switch(c->stmt_type)
			    {
			      case T_FOR:
				rsx_endfor(c);
				FAILCHECK
				break;
			      case T_FOREACH:
				rsx_endforeach(c);
				FAILCHECK
				break;
			      case T_WHILE:
				rsx_endwhile(c);
				break;
			      case T_CLONE:
				if (rsx_endclone(c))
				    FAILCHECK
				break;
			      case T_WAIT:
				rsx_endwait(c);
				FAILCHECK
				break;
			      default:
				FAILSYN;
			    }
			}
	| elif		{
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			} t_aexp then lastok {
			    rsx_elif();
                            FAILCHECK;
			} lastok1
	| ELSE lastok	{
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			    rsx_else(c);
			} lastok1
	| endif LASTOK	{
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			    rsx_endctrl();
			}
	| PARENT lastok {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_CLONE)
				FAILSYN
			    else if (c->flags & CS_ISCHILD)
			    {
				if (c->state==S_DISCLD)
				    FAILSYN
			    }
			    else if (c->state==S_ACTIVE)
				FAILSYN;
			    rsx_parent(c);
			} lastok1
	| { 
	    if (pstate->control.count &&
		((controlstack_t *) fgw_stackpeek(&pstate->control))->state!=
		  S_ACTIVE)
		{
		    NEXT_STMT
		}
	  } plain

;
done:	  DONE		{ $$.type=DONE; }
	| C_END FOR	{ $$.type=T_FOR; }
	| C_END FOREACH	{ $$.type=T_FOREACH; }
	| C_END WHILE	{ $$.type=T_WHILE; }
	| C_END CLONE	{ $$.type=T_CLONE; }
	| C_END WAIT	{ $$.type=T_WAIT; }
;
elif:	  ELIF
	| ELSE IF
;
endif:	  FI
	| C_END IF
;
/*
** control statements: openers
*/
ctrl:	  FOR ovarcl	{
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_FOR;
			    fgw_stackpush(&pstate->control, (char *) &c);
			    pstate->parseroptions|=SAFECOPYHACK;
			} IN t_elstc do lastok {
			    pstate->parseroptions&=~SAFECOPYHACK;
			    rsx_for();
			    FAILCHECK;
			} lastok1
	| FOREACH fselect aggcl into format do lastok {
			    endstmt_sql();
			    if ($2)
			    {
				instruction_t *i;

				i=rxx_newinstr(pstate);
				FAILCHECK
				i->func=rsx_setcursor;
				i->constant.type=CSTRINGTYPE;
				i->constant.val.string= $2;
			        i->constant.flags|=EF_NOTONTSS;
			    }
			    if (rsx_foreach(&$5))
				FAILCHECK;
			} lastok1
	| WHILE t_aexp do lastok	{
			    rsx_while();
			    FAILCHECK;
			} lastok1
	| CLONE t_exp INTO {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));

			    /* count is in destvar */
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_CLONE;
			    fgw_stackpush(&pstate->control, (char *) &c);
			} ovarcl ',' {
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);

			    /* destvar is in destvar, count in pcode */
			    rxx_swappcode(&pstate->pcode, &c->destvar);

			    /* count is in clone */
			    rxx_swappcode(&pstate->pcode, &c->clone);

			} ovarcl lastok {
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);

			    /* pid is in clone, count in pcode */
			    rxx_swappcode(&pstate->pcode, &c->clone);
			    rsx_clone(c);
			    FAILCHECK;
			} lastok1
	| WAIT FOR	{
			    pstate->parseroptions|=SAFECOPYHACK;
			} t_elstc {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_WAIT;
			    fgw_stackpush(&pstate->control, (char *) &c);
			    pstate->parseroptions&=~SAFECOPYHACK;
			} INTO ovarcl ',' ovarcl lastok	{
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    rxx_swappcode(&pstate->pcode, &c->destvar);
			    rsx_wait(c);
			    FAILCHECK;
			} lastok1
	| IF t_aexp then lastok	{
			    rsx_if();
                            FAILCHECK;
			} lastok1
;
do:	  _empty_
	| DO
;
then:	  _empty_
	| THEN
;
/*
** this pair of rules is used to bail out early succesfully on many control
** statements. I suppose I could get the grammar to be smarter and simply keep
** on going with statements while they are available
*/
lastok:	  _empty_ 	{
			    pstate->phase1.next_stmt_start=pstate->tokstart;
			}
	| LASTOK
;
lastok1:  _empty_	{
			    YYACCEPT;
			}
;
fselect:  newstcl fsel1	{ $$=$2; }
	| EXECUTE SQLTOK newstcl using tmpcon 	{
			    pstate->ssp->sqlstart=$<tstart>1;
			    $$=NULL;
			}
	| CALL newstcl using tmpcon 	{
			    pstate->ssp->sqlstart=$<tstart>1;
			    $$=NULL;
			}
  	| EXECUTE newstcl t_exp using	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_executeout;
			    markend_sql($<tstart>1);
			    endstmt_sql();
			    $$=NULL;
			}
; 	 
fsel1:	  cursor SELECT using tmpcon {
			    pstate->ssp->sqlstart=$<tstart>2;
			    $$=$1;
			}
	| input1 pattern reject { $$=NULL; }
;
cursor:	  VAR CURSOR whold FOR	{
				    $$=$1;
				}
	| whold		{ $$=NULL; }
;
whold:	  WITH HOLD	{ pstate->ssp->stmt->options|=SO_WITHHOLD; }
	| _empty_
;

/*
** simple statements
*/
plain:	  BREAK LASTOK	{
			    rsx_breaks(S_DISABLED);
			    FAILCHECK;
			}
	| CONTINUE LASTOK {
			    rsx_breaks(S_CONTINUE);
			    FAILCHECK;
			}
	| DISPOSE dvarcl LASTOK {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &$2, sizeof(instruction_t));
			    rsx_dispose();
			    FAILCHECK;
			}
	| DISPLAY newstcl {
			    rqf_setwidth(pstate->ssp->stmt, pstate->width);
	  		} t_elstc format LASTOK	{
			    rsx_display(&$5);
			    FAILCHECK;
			}
	| output
	| OUTPUT FORMAT outfmt LASTOK
	| OUTPUT WIDTH t_exp LASTOK {
			    rsx_width();
			    FAILCHECK;
			}
	| input destcl
	| ERROR LOG TO t_exp LASTOK {
			    rsx_errorlog();
			    FAILCHECK;
			}
	| connect
        | sql
	| WHENEVER ERROR errcl LASTOK
	| LOAD FROM t_exp del
			{
/* FIXME: implemented only if using 4gl - DEPRECATE */
#if defined(I4GLVER)
			    char *s=TOKPTR(pstate->tokstart);

			    MARK_STMT
			    endstmt_sql();
			    if (rsx_load(s))
				FAILCHECK
			    YYACCEPT;
#else
			    FAIL(RC_NLNKD);
#endif
			}
	| UNLOAD TO t_exp del
			{
/* FIXME: implemented only if using 4gl - DEPRECATE */
#if defined(I4GLVER)
			    char *s=TOKPTR(pstate->tokstart);

			    MARK_STMT
			    endstmt_sql();
			    if (rsx_unload(s))
				FAILCHECK
			    YYACCEPT;
#else
			    FAIL(RC_NLNKD)
#endif
			}
	| EXIT exitcl LASTOK	{
			    if (rsx_exit())
				FAILCHECK;
			}
	| plainsub LASTOK	{
			    rsx_evalexpr();
			    FAILCHECK;
			}
	| _empty_
;
output:	  APPEND TO append LASTOK
	| OUTPUT TO t_exp LASTOK {
			    rsx_outputto("w");
			    FAILCHECK;
			}
	| PIPE TO t_exp LASTOK 	{
			    rsx_pipeto();
			    FAILCHECK;
			}
	| WRITE TO t_exp LASTOK 	{
			    rsx_writeto();
			    FAILCHECK;
			}
append:	  DEFAULT	{
			    rsx_appenddefault();
			    FAILCHECK;
			}
	| t_exp		{
			    rsx_outputto("a");
			    FAILCHECK;
			}
;
input:	  input1 newstcl pattern reject 	/* avoid s/r conflict with PIPE TO */
;
input1:	  INPUT FROM t_exp	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_inputfrom;
			}
 	| PIPE FROM t_exp	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_pipefrom;
			}
	| READ FROM t_exp	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_readfrom;
			}
;
outfmt:	  PLAIN		{
			    if (pstate->style!=F_NONE)
				FAIL(RC_NOFMT)
			    rsx_flagreset(PF_ISHTML);
			}
	| HTML		{ rsx_flagset(PF_ISHTML); }
;
errcl:	  CONTINUE	{ rsx_flagset(PF_ERRORCONT); }
	| STOP		{ rsx_flagreset(PF_ERRORCONT); }
;
del:	  DELIMITER t_exp
	| pdel2	
;
exitcl:	  _empty_
	| t_aexp
;
connect:  CONNECT TO con src as user conopt {
                            instruction_t *i;
			    int r;

                            i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=$4.val.string;
			    i->constant.flags|=EF_NOTONTSS;
			    if ($5.type)
				i->constant.count++;
			    if ($6.type)
				i->constant.count+=2;
			    switch ($3.type)
			    {
			      case DEFAULT:
				r=rsx_defconnect(($7<0)? NULL: TOKPTR($7));
				break;
			      default:
				r=rsx_connect(($7<0)? NULL: TOKPTR($7));
			    }
			    if (r)
				FAILCHECK;
			}
	| S_SET CONNECTION con1 LASTOK {
			    int r;

			    switch ($3.type)
			    {
			      case CONNECTION:
				r=rsx_setconnection();
				break;
			      case SOURCE:
				r=rsx_setsource($3.val.string);
				break;
			      default:
				r=rsx_setdefault();
			    }
			    if (r)
				FAILCHECK;
			}
	| DISCONNECT con1 LASTOK {
			    int r;

			    switch ($2.type)
			    {
			      case CONNECTION:
				r=rsx_disconnect();
				break;
			      case SOURCE:
				r=rsx_srcdisconnect($2.val.string);
				break;
			      default:
				r=rsx_defdisconnect();
			    }
			    if (r)
				FAILCHECK;
			}
;
as:	  _empty_	{ $$.type=0; }
	| AS t_exp	{ $$.type=AS; }
;
user:	  _empty_	{ $$.type=0; }
	| USER t_exp USING t_exp { $$.type=USER; }
;
conopt:	  LASTOK	{ $$=-1; }
	| draincl	{ $$=$1; }
;
con:	  DEFAULT	{ $$.type=DEFAULT; }
	| t_exp		{ $$.type=CONNECTION; }
;
con1:	  DEFAULT src	{ $$=$2; }
	| t_exp		{ $$.type=CONNECTION; }
;
src:	 _empty_	{
			    $$.val.string=NULL;
			    $$.type=DEFAULT;
			}
	| SOURCE VAR	{
			    $$.val.string=$2;
			    $$.type=SOURCE;
			}
;
/*
** what we don't understand from the first token, we just pass to the engine
*/
sql:	  sql1
	| PREPARE ovarcl FROM newstcl psql usingres tmpcon
			      aggcl into format LASTOK {
			    endstmt_sql();
			    if (rsx_prepare(&$10))
				FAILCHECK;
			}

	  /* we allocate a placeholder statement to hold optional clauses */
	| EXECUTE newstcl t_exp		{
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_executeout;
			} using destcl
	| FREE t_exp LASTOK	{
			    if (rsx_free())
				FAILCHECK;
			}

;
psql:	  SELECT	{ pstate->ssp->sqlstart=$<tstart>1; }
	| EXECUTE SQLTOK	{ pstate->ssp->sqlstart=$<tstart>1; }
	| CALL 		{ pstate->ssp->sqlstart=$<tstart>1; }
	| dml

			/* FIXME should set tstart */
	| SQLTOK	{ pstate->ssp->sqlstart=pstate->tokstart; }
;
sql1:	  select destcl
	| sql2 tmpcon LASTOK	{
			    endstmt_sql();
			    if (rsx_sqlrun(STMTPTR(pstate->ssp)))
		    		FAILCHECK;
			}
;
/* ditto */
select:	  SELECT newstcl using tmpcon
	| EXECUTE SQLTOK newstcl using tmpcon
	| CALL newstcl using tmpcon
;
destcl:	  aggcl dstcl
;
aggcl:	  _empty_
	| AGGREGATE	{
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->aggregates);
			    pstate->parseroptions|=AGGREGATEHACK;
	  } agglst	{
			    pstate->parseroptions&=~AGGREGATEHACK;
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->aggregates);
			    markend_sql($<tstart>1);
			}
;
agglst:	  aglst1
	| aglst2
;
aglst1:	  aglst1 ',' agasn1
	| agasn1
;
agasn1:	  '(' t_bexp ')' {
			    instruction_t *i;
		   	    parsestack_t p;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_whenbool;
			    p.type=AGGREGATE;
			    p.instr=i;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			} aggas1 {
			    instruction_t *e;
			    parsestack_t *p;

			    p=(parsestack_t *) fgw_stackpop(&pstate->parsestack);
			    p->instr->skip_to=rxx_setjump(pstate);
			}
	| assign
;
aggas1:	  assign
	| aggas5
;
aglst2:	  aglst2 agasn2
	| agasn2
;
agasn2:	  WHEN t_bexp THEN aggas2 aggas3
	| plainsub
;
aggas2: 		{
			    instruction_t *i;
		   	    parsestack_t p;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_whenbool;
			    p.type=AGGREGATE;
			    p.instr=i;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			} aggas4 {
			    instruction_t *e;
			    parsestack_t *p;

			    p=(parsestack_t *) fgw_stackpop(&pstate->parsestack);
			    p->instr->skip_to=rxx_setjump(pstate);
			}
;
aggas3:	  _empty_
	| ELSE aggas4
;
aggas4:	  plainsub
	| aggas5
;
aggas5:	  aggas6 BREAK	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_aggrbreak;
			    if ($1.type==REJECT)
				i->flags|=RT_HASREJECT;
			}
	| aggas6 CONTINUE	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_aggrcont;
			    if ($1.type==REJECT)
				i->flags|=RT_HASREJECT;
			}
;
aggas6:	  _empty_	{ $$.type=0; }
	| REJECT AND	{
			    if (!pstate->ssp->stmt->reject)
				FAIL(RC_NRJCT)
			    $$.type=REJECT;
			}
;
plainsub: LET assign
	| INVOKE funcl retcl	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			}
	| FNCS alist	{
			    instruction_t *i;

			    if ($2.count!=$1->tl_parmcount && $1->tl_parmcount>=0)
				FAIL(RC_WACNT)
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=$1->tl_func;
			} retcl {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			    i->flags|=RT_DITCHRES;
			}
	| STORE alist intocl	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			}
;
assign:  ovarcl1  	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &$1, sizeof(instruction_t));
			} assign2
;
assign2:  '=' t_aexp %prec ASSIGN {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
	| assign3 typecl {
			    pstate->parseroptions&=~FUNCHACK;
			} '=' t_aexp %prec ASSIGN {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=$2.func;
			    i->constant.type=$2.constant.type;
			    i->constant.val.intvalue=
			        $2.constant.val.intvalue;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
	| assign3 '=' typecl {
			    instruction_t *i;

			    pstate->parseroptions&=~FUNCHACK;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_variable2;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=$3.func;
			    i->constant.type=$3.constant.type;
			    i->constant.val.intvalue=
			        $3.constant.val.intvalue;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
		   }
	| assign4 '='	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_variable2;
			} t_exp {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=$1.func;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
	| rangecl '=' t_aexp %prec ASSIGN {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			    i->flags=$1.flags;
			}
;
/*
** needed to have FUNCHACK to work properly
*/
assign3:  OPCAST { pstate->parseroptions|=FUNCHACK; }
;
assign4:  '+'		{
			    $$.func=rxo_add;
			}
	| '-'		{
			    $$.func=rxo_sub;
			}
	| '*'		{
			    $$.func=rxo_mul;
			}
	| '/'		{
			    $$.func=rxo_div;
			}
	| DPIPE		{
			    $$.func=rxo_dpipe;
			}
;
funcl:	  funcl1
	| FNCL flist	{
			    instruction_t *i;

			    if ($1->tl_parmcount>=0)
				$2->constant.val.intvalue=$1->tl_parmcount;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=$1->tl_func;
			}
;
retcl:	 _empty_	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_emptylist;
			    i->constant.count=0;
			}
	| RETURNING retlist	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=$2.count;
			    i->constant.flags|=$2.flags;
			}
;
retlist:		{
			    pstate->parseroptions|=RESVARHACK;
			} retl1 {
			    pstate->parseroptions&=~RESVARHACK;
			    $$=$2;
			}
;
retl1:	  retl1 ',' retl2	{
			    $$.count=$1.count+1;
			    $$.flags=$1.flags|$3.flags;
			}
	| retl2		{
			    $$.count=1;
			    $$.flags=$1.flags;
			}
;
retl2:	  ovarcl	{
			    $$.flags=0;
			}
	| ovarcl1 OPCAST { pstate->parseroptions|=FUNCHACK; } typecl {
			    instruction_t *i;

			    pstate->parseroptions&=~FUNCHACK;
			    $$.flags=0;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &$1, sizeof(instruction_t));
			    i->constant.totype=$4.constant.totype;
			    i->constant.qual=$4.constant.val.intvalue;
			}
	| dvarcl '.' '*' {
			    instruction_t *i;

			    $$.flags=EF_VARIABLEDEST;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &$1, sizeof(instruction_t));
			    i->flags|=RT_RESVAR;
			    i->constant.flags|=EF_EXPANDHASH;
			}
	| dvarcl '.' '(' {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &$1, sizeof(instruction_t));
			    i->flags|=RT_RESVAR;
			} alist ')' {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_identlist;
			    $$.flags=EF_VARIABLELIST;
			}
;
alist:	  t_elist 	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=$1.count;
			    i->constant.flags=$1.flags;
			    $$.count=$1.count;
			    $$.flags=$1.flags;
			}
;
intocl:	  INTO retlist	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=$2.count;
			    i->constant.flags|=$2.flags;
			}
;
dstcl:	  into format LASTOK 	{
			    endstmt_sql();
			    if (rsx_nonredirected(&$2))
				FAILCHECK;
			}
	 | 		{
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
			    i->func=rsx_redirect;
			} dstcl1 usingagg dstcl3 LASTOK {
			    endstmt_sql();
/*
** only an insert with values clause or execute procedure, both with
** placeholders will do!
*/
			    if ((pstate->parseroptions & VALUESHACK))
				FAIL(RC_NOSTR);
			    if (rsx_redirected())
				FAILCHECK;
			}
;
dstcl1:   dstcl2	{
			    markend_sql($<tstart>1-1);
			    endstmt_sql();
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM)
			    pstate->ssp->sqlstart=$<tstart>1;
			    pstate->ssp->stmt->options|=SO_INSCURS;
			    if (pstate->errorinfo)
				pstate->ssp->line_count=pstate->errorinfo->line_count;
			} tmpcon
	| EXECUTE t_exp	{
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_executein;
			    markend_sql($<tstart>1);
			    endstmt_sql();
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM);
			}
;
dstcl2:	  INSERT	{ $$=$1; }
	| EXECUTE SQLTOK { $$=$1; }
	| CALL		{ $$=$1; }
;
dstcl3:	  _empty_
	| AUTOCOMMIT t_exp {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_autocommit;
			}
;
sql2:	  dml newstcl using	{}
	| SELECT SQLTOK newstcl using	{}		/* select into temp */
	| SQLTOK newstcl using	{}
;
dml:	  INSERT	{ pstate->ssp->sqlstart=$<tstart>1; }
	| UPDATE	{ pstate->ssp->sqlstart=$<tstart>1; }
	| DELETE	{ pstate->ssp->sqlstart=$<tstart>1; }
;
newstcl:		{
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM);
			}
;
into:	  _empty_
	| INTO  {
			    rxx_swappcode(&pstate->pcode, &pstate->ssp->stmt->intovars);
	  } retlist	{
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rxo_listcount;
                            i->constant.count=$3.count;
			    i->constant.flags|=$3.flags;
                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rxo_into;
			    rxx_swappcode(&pstate->pcode, &pstate->ssp->stmt->intovars);
			    markend_sql($<tstart>1);
			}
;
tmpcon:	  _empty_	{
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_curcon;
			}
	| CONNECTION con	{
                            instruction_t *i;

			    markend_sql($<tstart>1);
                            i=rxx_newinstr(pstate);
                            FAILCHECK
			    if ($2.type==SOURCE)
			    {
				i->func=rsx_srcdefault;
				i->constant.type=CSTRINGTYPE;
				i->constant.val.string=$2.val.string;
				i->constant.flags|=EF_NOTONTSS;
			    }
			    else if ($2.type==CONNECTION)
				i->func=rsx_tmpcon;
			    else
				i->func=rsx_curdefault;
			}
;
usingres:		{ pstate->parseroptions|=RESVARHACK; } using
			{ pstate->parseroptions&=~RESVARHACK; }
usingagg:		{ pstate->parseroptions|=AGGREGATEHACK; } using
			{ pstate->parseroptions&=~AGGREGATEHACK; }
;
using:	  _empty_
	| USING {
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->usingvars);
	  } t_elstc	{
			    markend_sql($<tstart>1);
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->usingvars);
			}
;
/*
** reminder: at the end of the format evaluation pstate->pcode must be untouched!
*/
format:  _empty_	{
			    pstate->ssp->stmt->options|=SO_TABULAR;
			    memset(&$$, 0, sizeof(pcode_t));
			}
	| FORMAT ftype	{
			    markend_sql($<tstart>1);
			    pstate->ssp->stmt->width=pstate->width;
			    if (!$2.pcodehead)
				pstate->ssp->stmt->options|=SO_TABULAR;
			    memcpy(&$$, &$2, sizeof(pcode_t));
			}
;
ftype:	  BRIEF		{
			    memset(&$$, 0, sizeof(pcode_t));
			    pstate->ssp->stmt->fmt_type=FMT_BRIEF;
			}
	| ftype1 	{
			    memset(&$<pcode>$, 0, sizeof(pcode_t));
			    rxx_swappcode(&pstate->pcode, &$<pcode>$);
			} fspec headers {
			    $$=$<pcode>2;
			    rxx_swappcode(&pstate->pcode, &$$);
			}
	| DELIMITED 	{
			    memset(&$<pcode>$, 0, sizeof(pcode_t));
			    rxx_swappcode(&pstate->pcode, &$<pcode>$);
			} fspec pdel {
			    $$=$<pcode>2;
			    rxx_swappcode(&pstate->pcode, &$$);
			    pstate->ssp->stmt->fmt_type=FMT_DELIMITED;
			}
;
ftype1:	  FULL		{ pstate->ssp->stmt->fmt_type=FMT_FULL; }
	| VERTICAL 	{ pstate->ssp->stmt->fmt_type=FMT_VERTICAL; }
;
fspec:	  _empty_	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    rxu_setnull(&i->constant, CINTTYPE);
		        }
	| t_exp
;
headers:  _empty_	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_emptylist;
			}
	| HEADERS t_elstc
;
pattern:  patcl		{
			    instruction_t *i;
	
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    switch ($1.type)
			    {
			      case DELIMITED:
				i->func=rsx_delimited;
				break;
			      case ~DELIMITED:
				i->func=rsx_regex;
				break;
			      case EXTRACTOR:
				i->func=rsx_extractor;
				break;
			      case PATTERN:
				i->func=rsx_pattern;
				break;
			    }
			}
;
patcl:	  PATTERN pat1	{
			    $$.type=$2.type;
			}
	| EXTRACTOR VAR ':' VAR {
			    instruction_t *i;
	
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=$2;
			    i->constant.flags|=EF_NOTONTSS;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=$4;
			    i->constant.flags|=EF_NOTONTSS;
			    $$.type=EXTRACTOR;
			}
;
pat1:	  t_exp		{
			    $$.type=PATTERN;
			}
	| DELIMITED pdel	{
			    $$.type=DELIMITED;
			}
	| REGEXP DELIMITED rdel {
			    $$.type=~DELIMITED;
			}
;
pdel:	  pdel1 delesc
;
pdel1:	  pdel2
	| BY t_exp
;
pdel2:    _empty_	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.flags|=EF_NOTONTSS;
			    if (!(i->constant.val.string=getenv("DBDELIMITER")))
				i->constant.val.string="|";
			}
;
delesc:   _empty_	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.flags|=EF_NOTONTSS;
			    i->constant.val.string="\\";
			}
	| ESCAPE t_exp
;
rdel:	  _empty_	{
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.flags|=EF_NOTONTSS;
			    i->constant.val.string="[ \t]+";
			}
	| BY t_exp
;
reject:	  reject1 reject2
;
reject1:  _empty_
	| REJECT TO t_exp {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_rejectfile;
			}
;
reject2:  _empty_
	| BLOCKSIZE t_exp {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_rejectsize;
			}
;
/*
** had a single exp rule, but the BETWEEN and ESCAPE clauses would result
** in 40 s/r & 7 r/r conflicts. This grammar is less powerful, but cleaner
*/
t_aexp:	  t_bexp
	| t_exp
;
/*
** WARNING: see sqv_cvvar if you want to do assignments inside t_fexp!
*/
t_exp:	  t_exp1
	| NULLVALUE		{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    rxu_setnull(&i->constant, CINTTYPE);
				}

/* FIXME: index, replace, pads & substrings use byte (as opposed to char)
   starts & ends/lengths. they should be ok as far as GLS goes, but this 
   needs to be checked */
	| ivarcl rangecl	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    $1.func=rxo_variable;
				    $1.flags=$2.flags;
				    if (pstate->parseroptions & SAFECOPYHACK)
					$1.flags|=RT_SAFECOPY;
				    fgw_move(i, &$1, sizeof(instruction_t));
				}
	| funcl1		{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_checkfunc;
				}
	| FNCV '(' t_exp ')'	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$1->tl_func;
				    i->constant.flags=EF_FUNCHASH;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_checkfunc;
				}
	| t_exp PFIXFNCT	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_listcount;
				    i->constant.count=1;
				    i=rxx_newinstr(pstate);
				    i->func=$2->tl_func;
				}
	| CASE {
		   parsestack_t p;

		   p.type=CASE;
		   fgw_stackpush(&pstate->parsestack, (char *) &p);
	       } casecl END	{
				    parsestack_t *p;
				    instruction_t *e;

				    e=rxx_setjump(pstate);
				    for (;;)
				    {
					p=(parsestack_t *) fgw_stackpop(
							&pstate->parsestack);
					if (p->type==CASE)
					    break;
				        else if (p->type==THEN)
					    p->instr->skip_to=e;
				    }
				}
	| t_exp DPIPE t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_dpipe;
				}
	| t_exp PICTURE t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_picture;
				}
	| t_exp '+' t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_add;
				}
	| t_exp '-' t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_sub;
				}
	| t_exp '*' t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_mul;
				}
	| t_exp '/' t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_div;
				}
	| t_exp UNITS { pstate->parseroptions|=FUNCHACK; } unitcle {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_units;
				    i->constant.type=CINTTYPE;
				    i->constant.val.intvalue=TU_IENCODE(5,
						$4.type, $4.type);
				}
	| t_exp OPCAST { pstate->parseroptions|=FUNCHACK; } typecl {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$4.func;
				    i->constant.type=$4.constant.type;
				    i->constant.val.intvalue=
					$4.constant.val.intvalue;
			}
	| KWCAST '(' t_exp ',' { pstate->parseroptions|=FUNCHACK; } typecl ')' {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$6.func;
				    i->constant.type=$6.constant.type;
				    i->constant.val.intvalue=
					$6.constant.val.intvalue;
				}
	| '-' t_exp  %prec NEG	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_minus;
				}
	| '+' t_exp  %prec PLUS	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_plus;
				}
	| t_exp2		{
				    instruction_t *i;

				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    i=rxx_newinstr(pstate);
				    i->func=rxo_colvalue;
				}
;
/*
** have to separate this out of t_exp to avoid S/R conflicts in column
** assignments
*/
t_exp1:	  '(' t_exp ')'
	| CNST			{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    fgw_move(&i->constant, &$1,
						sizeof(exprstack_t));
				    i->constant.flags|=EF_CONSTANT;
				    if ($1.type==CSTRINGTYPE)
				    {
					fgw_tssdetach(&pstate->lexhead,
						      $1.val.string);
					fgw_tssattach(&pstate->pcode.parsehead,
						      $1.val.string);
				    }
				}
	| ivarcl		{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    $1.func=rxo_variable;
				    if (pstate->parseroptions & SAFECOPYHACK)
					$1.flags|=RT_SAFECOPY;
				    fgw_move(i, &$1, sizeof(instruction_t));
				}
;
t_exp2:	  '$' t_exp1 %prec PLUS
	|  COLUMN '(' t_exp ')'
;
/*
** WARNING: see sqv_cvvar if you want to do assignments inside t_bexp!
*/
t_bexp:  t_exp incl 	{
			    parsestack_t p;

			    p.type=IN;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			}'(' inlist ')'	{
				    parsestack_t *p;
				    instruction_t *e;

				    e=rxx_setjump(pstate);
				    p=(parsestack_t *)
					fgw_stackpop(&pstate->parsestack);
				    p->instr->func=($2.type==IN)?
							rxo_inend: rxo_niend;
				    for (;;)
				    {
					p=(parsestack_t *)
					    fgw_stackpop(&pstate->parsestack);
					if (p->type==IN)
					    break;
				        else if (p->type==~IN)
					{
					    p->instr->func=($2.type==IN)?
							   rxo_in: rxo_ni;
					    p->instr->skip_to=e;
					}
				    }
				}
	| t_exp betweencl t_exp AND t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_between;
				    i->constant.type=($2.type==BETWEEN);
				}
	| t_exp mrexcl t_exp escape {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_matches;
				    i->constant.type=($2.type==MATCHES);
				}
	| t_exp lrexcl t_exp escape {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_like;
				    i->constant.type=($2.type==LIKE);
				}
	| t_exp rrexcl t_exp	{
#ifdef HAVE_REGEX
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_regexp;
				    i->constant.type=($2.type==REGEXP);
#else
				    FAIL(RC_NLNKD)
#endif
				}
	| t_aexp OR t_aexp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_or;
				}
	| t_aexp AND t_aexp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_and;
				}
	| NOT t_aexp		{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_not;
				}
	| t_exp IS nullcl	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_isnull;
				    i->constant.type=CINTTYPE;
				    i->constant.val.intvalue=($3.type==NULLVALUE);
				}
	| t_exp eqcl t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_eq;
				}
	| t_exp NEQ t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_ne;
				}
	| t_exp LEQ t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_le;
				}
	| t_exp '<' t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_lt;
				}
	| t_exp GEQ t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_ge;
				}
	| t_exp '>' t_exp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_gt;
				}
	| '(' t_bexp ')'
;
typecl:	  TYPEINT		{
				    $$.func=rxo_tointeger;
				    $$.constant.totype=CINTTYPE;
				}
	| TYPEFLOAT		{
				    $$.func=rxo_todouble;
				    $$.constant.totype=CFLOATTYPE;
				}
	| TYPEDEC		{
				    $$.func=rxo_todecimal;
				    $$.constant.type=CDECIMALTYPE;
				    $$.constant.totype=CDECIMALTYPE;
				}
	| TYPEMONEY		{
				    $$.func=rxo_todecimal;
				    $$.constant.type=CMONEYTYPE;
				    $$.constant.totype=CMONEYTYPE;
				}
	| TYPEDATE		{
				    $$.func=rxo_todate;
				    $$.constant.totype=CDATETYPE;
				}
	| TYPEDTIME castql	{
				    $$.func=rxo_todatetime;
				    $$.constant.type=CINTTYPE;
				    $$.constant.totype=CDTIMETYPE;
				    $$.constant.val.intvalue=$2.val.intvalue;
				}
	| TYPEINTVL castql	{
				    $$.func=rxo_tointerval;
				    $$.constant.type=CINTTYPE;
				    $$.constant.totype=CINVTYPE;
				    $$.constant.val.intvalue=$2.val.intvalue;
				}
	| TYPESTRING		{
				    $$.func=rxo_tostring;
				    $$.constant.totype=CSTRINGTYPE;
				}
	| TYPEBYTE		{
				    $$.func=rxo_tobyte;
				    $$.constant.totype=FGWBYTETYPE;
				}
;
ovarcl:	  ovarcl1		{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &$1, sizeof(instruction_t));
				}
;
ovarcl1:  VAR ivarcl1		{
				    fgw_hstentry *e;
				    int d;

				    memset(&$$, 0, sizeof(instruction_t));
				    $$.func=rxo_identifier;
				    $$.flags|=$2.type;
				    $$.constant.type=FGWIDTYPE;
				    $$.constant.count=$2.count;
				    $$.constant.flags=$2.flags;
				    if ($2.count)
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 $1, &d))
					{
					    if (e->type!=T_HASH) 
						FAIL(RC_INSCL)
					    $$.constant.val.identifier=e->magic;
				    	}
					else if (status)
					    FAIL(status)
					else
					{
					    $$.constant.val.identifier=
						fgw_hstaddnode(pstate->vars, 0,
							       $1, 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
							      $1, 32))
				    {
					if (e->type==T_HASH)
					     FAIL(RC_INHST)
					$$.constant.val.identifier=e->magic;
				    }
				    else
					FAILCHECK
				    fgw_tssfree(&pstate->lexhead, $1);
				    $$.flags=RT_RESVAR;
				}
	| t_exp2		{
				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    $$.func=rxo_column;
				}
;
ivarcl:	  VAR ivarcl1		{
				    fgw_hstentry *e;
				    int d;

				    memset(&$$, 0, sizeof(instruction_t));
				    $$.func=rxo_identifier;
				    $$.flags|=$2.type;
				    $$.constant.type=FGWIDTYPE;
				    $$.constant.count=$2.count;
				    $$.constant.flags=$2.flags;
				    if ($2.count)
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 $1, &d))
					{
					    if (e->type!=T_HASH) 
						FAIL(RC_INSCL)
					    $$.constant.val.identifier=e->magic;
				    	}
					else if (status)
					    FAIL(status)
					else
					{
					    $$.constant.val.identifier=
						fgw_hstaddnode(pstate->vars, 0,
							   $1, 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
					     $1, (pstate->parseroptions &
						  RESVARHACK)?  32: 0))
				    {
					if (e->type==T_HASH)
					     FAIL(RC_INHST)
					$$.constant.val.identifier=e->magic;
				    }
				    else  if (pstate->parseroptions & RESVARHACK)
					FAIL(status)
				    else
					$$.constant.val.identifier=0;
				    fgw_tssfree(&pstate->lexhead, $1);
				}
;
dvarcl:	  VAR ivarcl1		{
				    fgw_hstentry *e;
				    int d;

				    memset(&$$, 0, sizeof(instruction_t));
				    $$.func=rxo_identifier;
				    $$.flags|=($2.type|RT_HASHVAR);
				    $$.constant.type=FGWIDTYPE;
				    $$.constant.count=$2.count;
				    $$.constant.flags=$2.flags;
				    if ($2.count || (pstate->parseroptions & RESVARHACK))
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 $1, &d))
					{
					    if (e->type!=T_HASH) 
						FAIL(RC_INSCL)
					    $$.constant.val.identifier=e->magic;
				    	}
					else if (status)
					    FAIL(status)
					else
					{
					    $$.constant.val.identifier=
						fgw_hstaddnode(pstate->vars, 0,
							   $1, 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
					     $1, 0))
					$$.constant.val.identifier=e->magic;
				    else
					$$.constant.val.identifier=0;
				    fgw_tssfree(&pstate->lexhead, $1);
				}
;
ivarcl1:  _empty_		{
				    $$.type=0;
				    $$.count=0;
				}
	| '(' t_elist ')'	{
				    $$.type=RT_HASHASH;
				    $$.count=$2.count;
				    $$.flags=$2.flags;
				}
;
/*
** rats: connect requires special case for user - way too complicated to
** use an approach similar to FUNCHACK
*/
funcl1:	  USER			{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxf_user;
				}
/*
** and WAIT!
*/
	| WAIT			{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxf_wait;
				}
/*
** and COUNT!
*/
	| countcl '(' '*' ')'	{
				    instruction_t *i;

				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_countstar;
				}
	| countcl '(' dvarcl ')' {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &$3, sizeof(instruction_t));
				    i->func=rxo_counthash;
				    i->flags|=RT_RESVAR;
				}
	| countcl '(' FNCV ')'	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$3->tl_func;
				    i->constant.flags=EF_FUNCCOUNT;
				}
	| VAR ':' VAR flist	{
				    instruction_t *i;
				    fgw_objtype *obj;

				    if (!(obj=rqx_getobject(pstate, $1, $3)))
					 FAIL(pstate->ca.sqlcode)
				    fgw_tssfree(&pstate->lexhead, $1);
				    fgw_tssfree(&pstate->lexhead, $3);
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=obj->obj;
				    i->src_id=obj->src_id;
				}
	| fncl1 flist		{
				    instruction_t *i;

				    if ($2->constant.count>0 && $1->tl_parmcount==0)
					FAIL(RC_WACNT)
				    if ($1->tl_parmcount>0)
					$2->constant.val.intvalue=$1->tl_parmcount;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$1->tl_func;
				}
	| FNDN { pstate->parseroptions|=FUNCHACK; } qualcl {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$3.func;
				    i->constant.type=$3.constant.type;
				    i->constant.val=$3.constant.val;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_listcount;
				    i->constant.count=1;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$1->tl_func;
				}
/*
** change to
**
	| FNDP '(' { pstate->parseroptions|=DTHACK } CNST ')'
	  { pstate->parseroptions|=FUNCHACK; } qualcl	{
**
** for true datetime/interval syntax
*/
	| FNDP '(' t_elist ')' { pstate->parseroptions|=FUNCHACK; } qualcl {
				    exprstack_t *s;
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    if ($3.count>0 && $1->tl_parmcount==0)
					FAIL(RC_WACNT)
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$6.func;
				    i->constant.type=$6.constant.type;
				    i->constant.val=$6.constant.val;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_listcount;
				    i->constant.count=$3.count+1;
				    i->constant.flags|=$3.flags;
				    if ($1->tl_parmcount>0)
					i->constant.val.intvalue=$1->tl_parmcount+1;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$1->tl_func;
				}
;
fncl1:	  FNCS			{ $$=$1; }
	| FNCT			{ $$=$1; }
;
countcl:  COUNT			{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				}
;
flist:	  _empty_		{
				    $$=rxx_newinstr(pstate);
				    FAILCHECK
				    $$->func=rxo_emptylist;
				}
	| '(' t_elstc ')'		{
				    $$=$2;
				}
;
rangecl:  '[' t_exp ']'		{
				    $$.flags=RT_HASSUB1;
				}
	| '[' t_exp ',' t_exp ']'	{
				    $$.flags=RT_HASSUB2;
				}
;
qualcl:	_empty_		{
				    $$.func=rxo_constant;
				    $$.constant.type=CINTTYPE;
				    $$.constant.val.intvalue=-1;
				}
	| unitcls TO unitcle	{
				    if ($1.type>$3.type)
					FAIL(RC_IQUAL)
				    $$.func=rxo_constant;
				    $$.constant.type=CINTTYPE;
				    $$.constant.val.intvalue=TU_DTENCODE($1.type,
								     $3.type);
				}
;
castql:	  _empty_		{
				    $$.type=CINTTYPE;
				    $$.val.intvalue=-1;
				}
	| unitcls TO unitcle	{
				    if ($1.type>$3.type)
					FAIL(RC_IQUAL)
				    $$.type=CINTTYPE;
				    $$.val.intvalue=TU_DTENCODE($1.type,
								$3.type);
				}
;
unitcls:  unitcl		{
				    $$=$1;
				}
	| FRACTION		{
				    $$.type=TU_F1;
				}
;
unitcle:  unitcl		{
				    $$=$1;
				}
	| FRACTION fractcl	{
				    $$=$2;
				}
;
fractcl:  _empty_		{
				    $$.type=TU_F5;
				}
	| '(' CNST ')'		{
				    if ($2.type!=CINTTYPE)
					FAILSYN;
				    switch($2.val.intvalue)
				    {
				      case 1:
					$$.type=TU_F1;
					break;
				      case 2:
					$$.type=TU_F2;
					break;
				      case 3:
					$$.type=TU_F3;
					break;
				      case 4:
					$$.type=TU_F4;
					break;
				      case 5:
					$$.type=TU_F5;
					break;
				      default:
					FAILSYN;
				    }
				}
;
unitcl:	  YEAR			{
				    $$.type=TU_YEAR;
				}
	| MONTH			{
				    $$.type=TU_MONTH;
				}
	| DAY			{
				    $$.type=TU_DAY;
				}
	| HOUR			{
				    $$.type=TU_HOUR;
				}
	| MINUTE		{
				    $$.type=TU_MINUTE;
				}
	| SECOND		{
				    $$.type=TU_SECOND;
				}
;
casecl:	  t_exp olcase lelse
	| obcase elsecl
;
olcase:	  olcase lcase
	| lcase
;
obcase:	  obcase bcase
	| bcase
;
lcase:	  WHEN t_exp	{
			    instruction_t *i;
			    parsestack_t p;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_whenexpr;
			    p.type=WHEN;
			    p.instr=i;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			} THEN t_exp 	{
				    instruction_t *i;
				    parsestack_t p, *pp;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_jump;
				    pp=(parsestack_t *)
					   fgw_stackpop(&pstate->parsestack);
				    pp->instr->skip_to=rxx_setjump(pstate);
				    p.type=THEN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
;
lelse:	  			{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_elseexpr;
				} elsecl
;
bcase:	  WHEN t_aexp	{
			    instruction_t *i;
			    parsestack_t p;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_whenbool;
			    p.type=WHEN;
			    p.instr=i;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			} THEN t_exp 	{
				    instruction_t *i;
				    parsestack_t p, *pp;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_jump;
				    pp=(parsestack_t *)
					    fgw_stackpop(&pstate->parsestack);
				    pp->instr->skip_to=rxx_setjump(pstate);
				    p.type=THEN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
;
elsecl:	  ELSE t_exp
	| _empty_		{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    rxu_setnull(&i->constant, CINTTYPE);
				}
;
nullcl:	  NULLVALUE		{
				    $$.type=NULLVALUE;
				}
	| NOT NULLVALUE		{
				    $$.type=~NULLVALUE;
				}
;
eqcl:	  '='
	| EQ
;
t_elstc:  t_elist	{
			    $$=rxx_newinstr(pstate);
			    FAILCHECK
			    $$->func=rxo_listcount;
			    $$->constant.count=$1.count;
			    $$->constant.flags=$1.flags;
			}
;
t_elist:  t_elist ',' t_elst1 	{
				    $$.count=$1.count+1;
				    $$.flags=$1.flags|$3.flags;
				}
	| t_elst1		{
				    $$.count=1;
				    $$.flags|=$1.flags;
				}
;
t_elst1:  t_aexp		{
				    $$.flags=0;
				}
	| t_elst2  t_elst3	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=$1->tl_func;
				    i->constant.flags|=$2.flags;
				    $$.flags=EF_VARIABLELIST;
				}
	| dvarcl '.' '*'	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &$1, sizeof(instruction_t));
				    i->func=rxo_dohashstar;
				    if (pstate->parseroptions & SAFECOPYHACK)
					 i->flags|=RT_SAFECOPY;
				    $$.flags=EF_VARIABLELIST;
				}
	| dvarcl '.' '(' 	{
				    instruction_t *i;
	
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &$1, sizeof(instruction_t));
				} alist ')' {
				    instruction_t *i;
	
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_varlist;
				    if (pstate->parseroptions & SAFECOPYHACK)
					 i->flags|=RT_SAFECOPY;
				    $$.flags=EF_VARIABLELIST;
				}
	| SPLIT t_aexp patcl	{
				    instruction_t *ic, *i;

				    ic=rxx_newinstr(pstate);
				    FAILCHECK
				    ic->func=rxo_listcount;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    switch ($3.type)
				    {
				      case DELIMITED:
					i->func=rxo_delsplit;
					ic->constant.count=3;
					break;
				      case PATTERN:
					i->func=rxo_patsplit;
					ic->constant.count=2;
					break;
				      case ~DELIMITED:
#ifdef HAVE_REGEX
					i->func=rxo_regdelsplit;
					ic->constant.count=2;
#else
					FAIL(RC_NLNKD)
#endif
					break;
				      case EXTRACTOR:
					i->func=rxo_extsplit;
					ic->constant.count=3;
					break;
				    }
				    $$.flags=EF_VARIABLELIST;
				}
	| t_aexp TO t_aexp	{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_to;
				    $$.flags=EF_VARIABLELIST;
				}
;
t_elst2:  FNCL flist		{
				    if ($2->constant.count && $1->tl_parmcount==0)
					FAIL(RC_WACNT)
				    if ($1->tl_parmcount>0)
					$2->constant.val.intvalue=$1->tl_parmcount;
				    $$=$1;
				}
	| FNCV			{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    $$=$1;
				}
;
t_elst3:  _empty_		{
				    $$.flags=0;
				}
	| '.' '(' alist ')'	{
				    $$.flags=EF_FUNCHASH;
				}
;
incl:	  IN			{
				    $$.type=IN;
				}
	| NOT IN		{
				    $$.type=~IN;
				}
;
inlist:	  inlist ',' t_aexp 	{
				    parsestack_t p;
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    p.type=~IN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
	| t_aexp		{
				    parsestack_t p;
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    p.type=~IN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
;
betweencl:  BETWEEN		{
				    $$.type=BETWEEN;
				}
 	| NOT BETWEEN		{
				    $$.type=~BETWEEN;
				}
;
mrexcl:	  MATCHES		{
				    $$.type=MATCHES;
				}
 	| NOT MATCHES		{
				    $$.type=~MATCHES;
				}
;
lrexcl:	  LIKE			{
				    $$.type=LIKE;
				}
 	| NOT LIKE		{
				    $$.type=~LIKE;
				}
;
rrexcl:	  REGEXP LIKE		{
				    $$.type=REGEXP;
				}
	| NOT REGEXP LIKE	{
				    $$.type=~REGEXP;
				}
;
escape:	  ESCAPE t_exp
	| _empty_		{
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    i->constant.type=CSTRINGTYPE;
				    i->constant.flags|=EF_NOTONTSS;
				    i->constant.val.string="\\";
				}

;

/* This is a dummy rule to trigger errors when generated by the scanner.
   We can't explicitly have a TOK_ERROR in the grammar as this will lead
   to shift reduce conflicts. Instead we check it in the code.
*/
_empty_:  /* empty */		{
				    if (yychar==TOK_ERROR)
					YYERROR;
				}
;

/* This is a dummy rule to drain tokens when we don't care about the rest */
draincl:  /* empty */		{
				    $$=pstate->tokstart;

/* hack alert - we want the parsing to terminate, but we don't want to accept yet
   hence we fool the parser into thinking that the input has ended */
				    yychar=0;
				    DRAIN_STMT;
				}
%%

/*
** csqpc interface
*/
static void init_sqp();

/*
** the parser
*/
int sql_parser(parserstate_t *state)
{
    int r;
    parserstate_t *save_pstate;
    int (*save_thischar)();

    save_pstate=pstate;
    save_thischar=thischar;
    pstate=state;
#ifdef PUREPARSER
    if (!pstate->parseroptions)
	thischar=expthischar_sql;
    else
#endif
	thischar=thischar_sql;
    if (pstate->parseroptions)
	pstate->parseroptions|=EXPRHACK;
    else
  	pstate->parseroptions=STMTHACK|LASTHACK;
    if (pstate->errorinfo)
    {
	pstate->errorinfo->line_count=0;
	pstate->errorinfo->near[0]='\0';
	pstate->errorinfo->extra[0]='\0';
    }
    pstate->prevtokstart=-1;
    pstate->ibufp=pstate->phase1.stmt_start;
    pstate->prevtoken=-1;
    pstate->prevlval=malloc(sizeof(YYSTYPE));
    pstate->svyylval=malloc(sizeof(YYSTYPE));
 
    pstate->stmtcnt=0;
    memset(&pstate->stmtstack, 0, sizeof(pstate->stmtstack));
    pstate->stmtstack[0].sqlstart=pstate->ibufp;
    pstate->ssp=(stmtstack_t *) &pstate->stmtstack;
    pstate->curstmt=NULL;

    pstate->instrp=NULL;
    pstate->runneroptions=0;
    init_sqp();
    status=0;
    rxx_emptypcode(pstate);
    yyparse();

/*
** clear error if WHENEVER ERROR CONTINUE,
** unless it's a syntax error
*/
    if ((pstate->flags & PF_ERRORCONT) && RC_ERRSKIP(status))
	status=0;
/*
** stored procedures and outer layers require buffer untouched
** restore whatever damage we have done
*/
    if (pstate->stmtstack[0].sqlsv)
	*TOKPTR(pstate->stmtstack[0].sqlend)=pstate->stmtstack[0].sqlsv;
    if (pstate->stmtstack[1].sqlsv)
	*TOKPTR(pstate->stmtstack[1].sqlend)=pstate->stmtstack[1].sqlsv;
    free(pstate->prevlval);
    free(pstate->svyylval);

bad:
/*
** free string resources
*/
    fgw_tssdrop(&(pstate->lexhead));
    pstate=save_pstate;
    thischar=save_thischar;
/*
** status cleared defensively not to affect 4gl code
*/
    r=status;
    status=0;
    return r;
}

/*
** instantiates internal parser resources
*/
void sql_parserinit(parserstate_t *state)
{
    fgw_stackinit(&state->parsestack, sizeof(parsestack_t));
}

/*
** frees internal parser resources
*/
void sql_parserfree(parserstate_t *state)
{
    fgw_tssdrop(&state->lexhead);
    fgw_stackfree(&state->parsestack, NULL);
    rxx_droppcode(&state->pcode);
}

/*
** saves parser state for recursive call
*/

void sql_parsersave(parserstate_t *state)
{
    state->svyychar=yychar;
    *((YYSTYPE *) state->svyylval)=yylval;
}

/*
** restores parser state after recursive call
*/
void sql_parserrestore(parserstate_t *state)
{
    yychar=state->svyychar;
    yylval=*((YYSTYPE *) state->svyylval);
}

/*
** returns delimident flag
*/
int fgw_delimident()
{
    return delimident;
}

/*
** statement related ops
**
** mark the end of an sql segment
*/
static void markend_sql(int c)
{
    if (!pstate->ssp->sqlend || c<pstate->ssp->sqlend)
	pstate->ssp->sqlend=c;
}

/*
** temporarily end sql segment for execution
*/
static void endstmt_sql()
{
    if (pstate->ssp->sqlend)
    {
	pstate->ssp->sqlsv=*TOKPTR(pstate->ssp->sqlend);
	*TOKPTR(pstate->ssp->sqlend)='\0';
    }
}

/*
** scanner & parser hacks
**
** reserved words list
*/
static tokenlist_t tokenlist[]=
{
    "quit", NULL, QUIT, 0, 0, NULL,

    "for", NULL, FOR, 0, 0, NULL,
    "foreach", NULL, FOREACH, 0, 0, NULL,
    "while", NULL, WHILE, 0, 0, NULL,
    "clone", NULL, CLONE, 0, 0, NULL,
    "parent", NULL, PARENT, 0, 0, NULL,
    "if", NULL, IF, 0, 0, NULL,
    "elif", NULL, ELIF, 0, 0, NULL,
    "else", NULL, ELSE, 0, 0, NULL,
    "then", NULL, THEN, 0, 0, NULL,
    "fi", NULL, FI, 0, 0, NULL,
    "done", NULL, DONE, 0, 0, NULL,
    "do", NULL, DO, 0, 0, NULL,
    "continue", NULL, CONTINUE, 0, 0, NULL,
    "break", NULL, BREAK, 0, 0, NULL,
    "exit", NULL, EXIT, 0, 0, NULL,

    "display", NULL, DISPLAY, 0, 0, NULL,
    "let", NULL, LET, 0, 0, NULL,
    "dispose", NULL, DISPOSE, 0, 0, NULL,
    "invoke", NULL, INVOKE, 0, 0, NULL,
    "store", NULL, STORE, 0, 0, NULL,

    "pipe", NULL, PIPE, 0, 0, NULL,
    "append", NULL, APPEND, 0, 0, NULL,
    "output", NULL, OUTPUT, 0, 0, NULL,
    "write", NULL, WRITE, 0, 0, NULL,
    "read", NULL, READ, 0, 0, NULL,
    "input", NULL, INPUT, 0, 0, NULL,
    "reject", NULL, REJECT, 0, 0, NULL,
    "blocksize", NULL, BLOCKSIZE, 0, 0, NULL,

    "whenever", NULL, WHENEVER, 0, 0, NULL,
    "load", NULL, LOAD, 0, 0, NULL,
    "unload", NULL, UNLOAD, 0, 0, NULL,
    "select", NULL, SELECT, 0, 0, NULL,

    /* this is a bit naughty - but we can use upserts
       where inserts are used, and the grammar doesn't
       even need to know about it...
     */
    "upsert", NULL, INSERT, 0, 0, NULL,
    "insert", NULL, INSERT, 0, 0, NULL,
    "delete", NULL, DELETE, 0, 0, NULL,
    "update", NULL, UPDATE, 0, 0, NULL,
    "execute", NULL, EXECUTE, 0, 0, NULL,
    "call", NULL, CALL, 0, 0, NULL,
    "create", NULL, CREATE, 0, 0, NULL,
    "end", NULL, END, 0, 0, NULL,
    "prepare", NULL, PREPARE, 0, 0, NULL,
    "free", NULL, FREE, 0, 0, NULL,

    "connect", NULL, CONNECT, 0, 0, NULL,
    "disconnect", NULL, DISCONNECT, 0, 0, NULL,
    "set", NULL, S_SET, 0, 0, NULL,
    "connection", NULL, CONNECTION, 0, 0, NULL,

    "begin", NULL, BEGIN, 0, 0, NULL,
    "immediate", NULL, IMMEDIATE, 0, 0, NULL,
    "compound", NULL, COMPOUND, 0, 0, NULL,

    "error", NULL, ERROR, 0, 0, NULL,
    "log", NULL, LOG, 0, 0, NULL,
    "stop", NULL, STOP, 0, 0, NULL,
    "procedure", NULL, PROCEDURE, 0, 0, NULL,
    "function", NULL, FUNCTION, 0, 0, NULL,
    "cursor", NULL, CURSOR, 0, 0, NULL,
    "with", NULL, WITH, 0, 0, NULL,
    "hold", NULL, HOLD, 0, 0, NULL,
    "as", NULL, AS, 0, 0, NULL,
    "source", NULL, SOURCE, 0, 0, NULL,
    "default", NULL, DEFAULT, 0, 0, NULL,

    "plain", NULL, PLAIN, 0, 0, NULL,
    "html", NULL, HTML, 0, 0, NULL,
    "width", NULL, WIDTH, 0, 0, NULL,

    "returning", NULL, RETURNING, 0, 0, NULL,

    "in", NULL, IN, 0, 0, NULL,
    "aggregate", NULL, AGGREGATE, 0, 0, NULL,
    "autocommit", NULL, AUTOCOMMIT, 0, 0, NULL,
    "into", NULL, INTO, 0, 0, NULL,
    "using", NULL, USING, 0, 0, NULL,
    "format", NULL, FORMAT, 0, 0, NULL,
    "brief", NULL, BRIEF, 0, 0, NULL,
    "vertical", NULL, VERTICAL, 0, 0, NULL,
    "full", NULL, FULL, 0, 0, NULL,
    "delimited", NULL, DELIMITED, 0, 0, NULL,
    "regexp", NULL, REGEXP, 0, 0, NULL,
    "headers", NULL, HEADERS, 0, 0, NULL,
    "pattern", NULL, PATTERN, 0, 0, NULL,
    "extractor", NULL, EXTRACTOR, 0, 0, NULL,
    "from", NULL, FROM, 0, 0, NULL,
    "to", NULL, TO, 0, 0, NULL,
    "by", NULL, BY, 0, 0, NULL,
    "delimiter", NULL, DELIMITER, 0, 0, NULL,

    "temp", NULL, TEMP, 0, 0, NULL,
    "scratch", NULL, SCRATCH, 0, 0, NULL,
    "external", NULL, EXTERNAL, 0, 0, NULL,

    "cast", NULL, KWCAST, 0, 0, NULL,
    "integer", NULL, TYPEINT, 0, 0, NULL,
    "int", NULL, TYPEINT, 0, 0, NULL,
    "float", NULL, TYPEFLOAT, 0, 0, NULL,
    "decimal", NULL, TYPEDEC, 0, 0, NULL,
    "dec", NULL, TYPEDEC, 0, 0, NULL,
    "money", NULL, TYPEMONEY, 0, 0, NULL,
    "string", NULL, TYPESTRING, 0, 0, NULL,
    "byte", NULL, TYPEBYTE, 0, 0, NULL,

    "split", NULL, SPLIT, 0, 0, NULL,

    "and", NULL, AND, 0, 0, NULL,
    "or", NULL, OR, 0, 0, NULL,
    "not", NULL, NOT, 0, 0, NULL,
    "is", NULL, IS, 0, 0, NULL,
    "case", NULL, CASE, 0, 0, NULL,
    "when", NULL, WHEN, 0, 0, NULL,
    "then", NULL, THEN, 0, 0, NULL,
    "null", NULL, NULLVALUE, 0, 0, NULL,
    "between", NULL, BETWEEN, 0, 0, NULL,
    "matches", NULL, MATCHES, 0, 0, NULL,
    "like", NULL, LIKE, 0, 0, NULL,
    "escape", NULL, ESCAPE, 0, 0, NULL,
    "units", NULL, UNITS, 0, 0, NULL,
    "hour", NULL, HOUR, 0, 0, NULL,
    "minute", NULL, MINUTE, 0, 0, NULL,
    "second", NULL, SECOND, 0, 0, NULL,
    "fraction", NULL, FRACTION, 0, 0, NULL,

    "column", NULL, COLUMN, 0, 0, NULL,
    "columns", rxf_columns, FNCV, 0, 0, NULL,
    "count", NULL, COUNT, 0, 0, NULL,

    "displaylabels", rxf_displaylabels, FNCV, 0, 0, NULL,
    "displayformat", rxf_displayformat, FNCT, 0, -1, NULL,

    "filetotext", rxf_filetotext, FNCT, 0, 1, NULL,
    "filetoblob", rxf_filetoblob, FNCT, 0, 1, NULL,

    "ascii", rxf_ascii, FNCT, 0, 1, NULL,
    "clipped", rxf_clipped, PFIXFNCT, 0, 1, NULL,
    "spaces", rxf_spaces, PFIXFNCT, 0, 1, NULL,
    "picture", NULL, PICTURE, 0, 0, NULL,

    "sleep", rxf_sleep, FNCS, 0, 1, NULL,
    "system", rxf_run, FNCS, 0, 1, NULL,
    "run", rxf_run, FNCS, 0, 1, NULL,
    "spawn", rxf_spawn, FNCS, 0, 1, NULL,
    "exec", rxf_exec, FNCS, 0, 1, NULL,
    "waitpid", rxf_waitpid, FNCS, 0, 1, NULL,
/*    "wait", rxf_wait, FNCS, 0, 0, NULL, */
    "wait", NULL, WAIT, 0, 0, NULL,
    "fork", rxf_fork, FNCT, 0, 0, NULL,
    "fopen", rxf_fopen, FNCT, 0, 2, NULL,
    "popen", rxf_popen, FNCT, 0, 2, NULL,
    "sclose", rxf_sclose, FNCT, 0, 1, NULL,
    "getenv", rxf_getenv, FNCT, 0, 1, NULL,
    "eval", rxf_eval, FNCT, 0, 1, NULL,

    "index", rxf_index, FNCT, 0, 2, NULL,
    "match", rxf_match, FNCT, 0, 2, NULL,
    "pad", rxf_pad, FNCT, 0, 2, NULL,
    "random", rxf_random, FNCT, 0, -1, NULL,

    "current", rxf_current, FNDN, 0, 0, NULL,
    "today", rxf_today, FNCT, 0, 0, NULL,
    "user", NULL, USER, 0, 0, NULL,		/* damn connect statement */
/*    "user", rxf_user, FCNT, 0, 0, NULL, */

/* dbservername */
/* sitename */

    "decode", rxf_decode, FNCT, 0, -1, NULL,
    "nvl", rxf_nvl, FNCT, 0, 2, NULL,

    "abs", rxf_abs, FNCT, 0, 1, NULL,
    "mod", rxf_mod, FNCT, 0, 2, NULL,
    "pow", rxf_pow, FNCT, 0, 2, NULL,
    "root", rxf_root, FNCT, 0, 2, NULL,
    "round", rxf_round, FNCT, 0, -1, NULL,
    "sqrt", rxf_sqrt, FNCT, 0, 1, NULL,
    "trunc", rxf_trunc, FNCT, 0, -1, NULL,

    "dbinfo", rxf_dbinfo, FNCT, 0, 1, NULL,
    "sqlcode", rxf_sqlcode, FNCT, 0, 0, NULL,
    "sqlerrd1", rxf_sqlerrd1, FNCT, 0, 0, NULL,
    "sqlerrd2", rxf_sqlerrd2, FNCT, 0, 0, NULL,
    "errno", rxf_errno, FNCT, 0, 0, NULL,

    "exp", rxf_exp, FNCT, 0, 1, NULL,
    "log10", rxf_log10, FNCT, 0, 1, NULL,
    "logn", rxf_logn, FNCT, 0, 1, NULL,
    "hex", rxf_hex, FNCT, 0, 1, NULL,

    "char_length", rxf_charlength, FNCT, 0, 1, NULL,
    "character_length", rxf_charlength, FNCT, 0, 1, NULL,
    "length", rxf_length, FNCT, 0, 1, NULL,
/* octet_length e */

#ifdef HAS_DTFMT
    "timestamp", rxf_datetime, FNDP, TYPEDTIME, -1, NULL,
    "datetime", rxf_datetime, FNDP, TYPEDTIME, -1, NULL,
    "duration", rxf_interval, FNDP, TYPEINTVL, -1, NULL,
    "interval", rxf_interval, FNDP, TYPEINTVL, -1, NULL,
#else
    "timestamp", rxf_datetime, FNDP, TYPEDTIME, 1, NULL,
    "datetime", rxf_datetime, FNDP, TYPEDTIME, 1, NULL,
    "duration", rxf_interval, FNDP, TYPEINTVL, 1, NULL,
    "interval", rxf_interval, FNDP, TYPEINTVL, 1, NULL,
#endif
    "date", rxf_date, FNCT, TYPEDATE, -1, NULL,
    "day", rxf_day, FNCT, DAY, 1, NULL,
    "month", rxf_month, FNCT, MONTH, 1, NULL,
    "year", rxf_year, FNCT, YEAR, 1, NULL,
    "weekday", rxf_weekday, FNCT, 0, 1, NULL,
    "mdy", rxf_mdy, FNCT, 0, 3, NULL,
/* extend e / f to l */
/* to_char e /fmt */
/* to_date e /fmt */

    "cos", rxf_cos, FNCT, 0, 1, NULL,
    "sin", rxf_sin, FNCT, 0, 1, NULL,
    "tan", rxf_tan, FNCT, 0, 1, NULL,
    "acos", rxf_acos, FNCT, 0, 1, NULL,
    "asin", rxf_asin, FNCT, 0, 1, NULL,
    "atan", rxf_atan, FNCT, 0, 1, NULL,
    "atan2", rxf_atan2, FNCT, 0, 2, NULL,

/* trim leading/trailing/both e from e */
/* substring e from start for length */
    "substr", rxf_substr, FNCT, 0, 3, NULL,
    "replace", rxf_replace, FNCT, 0, -1, NULL,
    "lpad", rxf_lpad, FNCT, 0, -1, NULL,
    "rpad", rxf_rpad, FNCT, 0, -1, NULL,

    "lower", rxf_lower, FNCT, 0, 1, NULL,
    "upper", rxf_upper, FNCT, 0, 1, NULL,
    "initcap", rxf_initcap, FNCT, 0, 1, NULL
};

#define ERRCONTEXTLEN	10
/*
** error handler
*/
static void yyerror(char *s)
{
    int i, eoi=(!pstate->tokstart) || *TOKPTR(pstate->tokstart)==';';
    char *n;

/*
** set error if not alread set
*/
    if (!status)
	status=RC_PSNTX;
/*
** copy error context
*/
    if (pstate->errorinfo && SQ_ISSYNTAX(SQ_TOSRC(status)))
    {
/*
** first copy portions of token prior to the error
*/
	if (pstate->prevtokstart>=0)
	{
	    if (pstate->prevtoklen<=ERRCONTEXTLEN)
		strncat((char *) &pstate->errorinfo->near,
			 TOKPTR(pstate->prevtokstart),
			 pstate->prevtoklen);
	    else
	    {
		char *s=TOKPTR(pstate->prevtokstart)+
			pstate->prevtoklen-ERRCONTEXTLEN;

		strcat((char *) &pstate->errorinfo->near, "...");
		strncat((char *) &pstate->errorinfo->near, s, ERRCONTEXTLEN);
	    }
	}
/*
** then portions of token resulting in the error
*/
	strcat((char *) &pstate->errorinfo->near, " >>");
	if (!eoi)
	{
	    if (pstate->toklen<=ERRCONTEXTLEN)
		strncat((char *) &pstate->errorinfo->near,
			TOKPTR(pstate->tokstart), pstate->toklen);
	    else
	    {
	        strncat((char *) &pstate->errorinfo->near,
			TOKPTR(pstate->tokstart), ERRCONTEXTLEN);
	         strcat((char *) &pstate->errorinfo->near, "...");
	    }
	}
	strcat((char *) &pstate->errorinfo->near, "<< ");
	if (*TOKPTR(pstate->tokstart) && eoi)
	    strncat((char *) &pstate->errorinfo->near,
			TOKPTR(pstate->tokstart), pstate->toklen);
/*
** clear newlines and tabs from 'near' text
*/
	for (n=(char *) &pstate->errorinfo->near; *n; n++)
	    if (*n=='\n' || *n=='\t')
		*n=' ';
    }
/*
** free resources
*/
    for (i=0; i<=pstate->stmtcnt-1; i++)
	if (pstate->stmtstack[i].stmt)
	{
	    if (pstate->stmtstack[i].stmt->st_entry.signature & SS_INTERNAL)
		rqx_closestatement(pstate->stmtstack[i].stmt);
	    else
		rqx_freestatement(pstate->stmtstack[i].stmt);
	    pstate->stmtstack[i].stmt=NULL;
	}
#ifdef PUREPARSER
    draintokens_sql();
#endif
}

#define PD_ESCAPE	1
#define PD_IDENT	2

typedef struct
{
    char *dirname;
    int dirvalue;
} dirlist_t;

dirlist_t dirlist[] =
{
    "escape", PD_ESCAPE,
    "esc", PD_ESCAPE,
    "ignore", PD_ESCAPE,
    "ign", PD_ESCAPE,
    "ident", PD_IDENT,
    NULL, 0
};

/*
** parser directives
*/
static int getdir_sql(char *s, char *e)
{
    dirlist_t *d;
    char *c;
    int l;

/*
** scan directives
*/
    for (d=dirlist; d->dirname; d++)
    {
	c=s;
/*
** skip initial spaces
*/
	while (*c==' ' || *c=='\n' || *c=='\t')
	  c++;
/*
** compare directive name
*/
	l=strlen(d->dirname);
	if (e-c+1<l)
	   continue;
	if (strncmp(c, d->dirname, l))
	    continue;
	c+=l;
/*
** skip final spaces and make sure there isn't anything else
*/
	for (;;)
	    if (c>e)
		return d-> dirvalue;
	    else if (*c==' ' || *c=='\n' || *c=='\t')
		c++;
	    else
		break;
    }
}

#ifdef PUREPARSER
/*
** early YYACCEPT: purge remaining statement tokens
*/
static void draintokens_sql()
{
    int c;
    int old_status=status;

    status=0;
    if ((c=*TOKPTR(pstate->tokstart))==';' || c=='\0')
	goto end;
    for (;;)
	switch (scanner_sql())
	{
	  case VAR:
	    fgw_tssfree(&pstate->lexhead, yylval.var);
	    break;
	  case CNST:
	    if (yylval.stack.type==CSTRINGTYPE)
		fgw_tssfree(&pstate->lexhead, yylval.stack.val.string);
	    break;
	  case TOK_ERROR:
	  case ';':
	  case 0:
	    pstate->phase1.next_stmt_start=pstate->ibufp;
	    goto end;
	}
end:
    status=old_status;
}

/*
** pure parser: fill buffers and fire expansions
*/
static int fillbuffer_sql()
{
    int r, ip, s, e, l, nlc;
    char *txt, *tp;
    controlstack_t *c;

/*
** in case of consecutive comments, no character could make it to
** the buffer, so have to try repeatedly
*/
   for (;;)
   {
/*
** still characters to go in the expansion buffer
*/
	if (pstate->phase1.output_buffer_len>pstate->ibufp)
	    return 0;

/*
** reached the end of the script
*/
	if (pstate->o_size && pstate->phase1.input_char==pstate->i_size)
	    return 0;
	txt=NULL;
/*
** fire expansion
*/
	pstate->svyychar=yychar;
	*((YYSTYPE *) pstate->svyylval)=yylval;
	pstate->phase1.expstart=-1;
	r=fgw_sqlexplode(pstate->i_query,
			 pstate->i_size, pstate->vars,
			 (pstate->flags & PF_VERBOSE),
			 ((!pstate->control.count ||
			   (c=(controlstack_t *)
				fgw_stackpeek(&pstate->control),
				 c->state>S_DISABLED))?
			  (pstate->expstate): ES_DISABLED),
			 pstate->phase1.input_char+1, &ip, &txt,
			 &s, &e, &nlc);
	yychar=pstate->svyychar;
	yylval=*((YYSTYPE *) pstate->svyylval);
/*
** error in expansion? abort
*/
	if (r!=0)
	{
	    if (pstate->errorinfo)
		pstate->errorinfo->line_count=pstate->phase1.line_count+nlc+1;
/* FIXME - handle non expansion facility errors better */
	    if (txt)
		free(txt);
	    return r;
	}
/*
** resize output buffer
*/
	if (pstate->o_size<
	    (s<0? 0: e-s)+
	    (txt? strlen(txt): 0)+2+nlc)
	{
	    char *newbuf;

	    newbuf=realloc(pstate->o_query, pstate->o_size+BUFSIZE);
	    if (!newbuf)
	    {
		if (txt)
		    free(txt);
		return RC_NOMEM;
	    }
	    pstate->o_query=newbuf;
	    pstate->o_size+=BUFSIZE;
	}
/*
** finally, copy strings
*/
	if (s>=0 && (l=e-s+1))
	{
	    fgw_move(pstate->o_query+pstate->phase1.output_buffer_len,
		    pstate->i_query+s-1, l);
	    pstate->phase1.output_buffer_len+=l;
	    *(pstate->o_query+pstate->phase1.output_buffer_len)='\0';
	}
	if (txt)
	{
	    pstate->phase1.expstart=pstate->phase1.output_buffer_len;
	    if (pstate->expcount==pstate->phase1.expcount)
		pstate->phase1.prev_input_char=pstate->phase1.input_char;
	    l=strlen(txt);
	    strcpy(pstate->o_query+pstate->phase1.output_buffer_len, txt);
	    pstate->phase1.output_buffer_len+=l;
	    pstate->phase1.expcount++;
	    free(txt);
	}
/*
** add new lines to the end of the expansion to keep track
** of newlines in expansion text
*/
	if (nlc)
	{
	     tp=pstate->o_query+pstate->phase1.output_buffer_len;
	     pstate->phase1.output_buffer_len+=nlc;
	     while (nlc--)
		*tp++='\n';
	     *tp='\0';
	}
/*
** set next expansion search start
*/
	if (ip<0)
	    pstate->phase1.input_char=pstate->i_size;
	else
	    pstate->phase1.input_char=ip-1;
    }
    return 0;
}

/*
** advance scanner current char - expansion handling version
*/
static int expthischar_sql()
{
    int r;

    if (r=fillbuffer_sql())
    {
	status=r;
	return -1;
    }
    return *TOKPTR(pstate->ibufp);
}
#endif 

/*
** advance scanner current char - non reentrant parser version
*/
static int thischar_sql()
{
    return *TOKPTR(pstate->ibufp);
}

/*
** scanner
*/
static int scanner_sql()
{
    int c, c1;
    int r;
    tokenlist_t *t;
    int newdir=0, dir;

    memset(&yylval.stack, 0, sizeof(yylval.stack));

/* if we ever want to revert to true datetime/interval constant syntax,
   here's the place to

    if (pstate->parseroptions & DTHACK)
    {
	pstate->parseroptions&=~DTHACK;
	while (c=*TOKPTR(pstate->ibufp)!=')')
...etc
    }
    else
*/
    while ((c=(thischar)())>0)
    {
	pstate->ibufp++;
/*
** skip spaces, tabs & newlines
*/
	if (c==' ' || c=='\t')
	    continue;
	if (c=='\n')
	{
	    pstate->phase1.line_count++;
	    continue;
	}
	if (pstate->errorinfo && !pstate->errorinfo->line_count)
	    pstate->errorinfo->line_count=pstate->phase1.line_count+1;
	pstate->prevtokstart=pstate->tokstart;
	pstate->prevtoklen=pstate->toklen;
	pstate->tokstart=pstate->ibufp-1;
	pstate->toklen=0;
	dir=newdir;
	newdir=0;
/*
** looks like an hex or number
*/
	if (c=='0')
	{
/*
** we return TOK_ERROR to trigger a YYERROR if there's an expansion error
*/
	    if ((c1=(thischar)())<0)
		return TOK_ERROR;
	    if (c1=='x' || c1=='X')
	    {
		pstate->ibufp++;
		yylval.stack.type=CINTTYPE;
		if ((yylval.stack.val.intvalue=getdigits_sql(16))<0)
		    return 0;
		return CNST;
	    }
	    else if (c1>'0' && c1<'8')
	    {
		yylval.stack.type=CINTTYPE;
		if ((yylval.stack.val.intvalue=getdigits_sql(8))<0)
		    return 0;
		return CNST;
	    }
	}
/*
** integer or float
*/
	if (c>='0' && c<='9')
	{
	    int tmp;

	    pstate->ibufp--;
	    if ((tmp=getdigits_sql(10))<0)
		return TOK_ERROR;

	    /* FIXME: GLS */
	    if ((c=(thischar)())<0)
		return TOK_ERROR;
	    if (c=='.')
	    {
		pstate->ibufp++;
		yylval.stack.type=CDOUBLETYPE;
		if (getfloat_sql(tmp, &yylval.stack.val.real))
		    return TOK_ERROR;
	    }
	    else
	    {
		yylval.stack.type=CINTTYPE;
		yylval.stack.val.intvalue=tmp;
	    }
	    return CNST;
	}
/*
** variable, function or keyword
*/
	else if ((c>='A' && c<='Z') || (c>='a' &&c<='z'))
	{
	    if (!(yylval.var=getvar_sql(&(pstate->lexhead))))
		return 0;
	    pstate->toklen=pstate->ibufp-pstate->tokstart;
	    t=&tokenlist[fgw_hash(yylval.var, NUMTOKENS)];
	    while (t)
	    {
		if (strcmp(t->tl_name, yylval.var))
		    t=t->tl_next;
		else
		{
		    /* escaped identifier */
		    if (dir==PD_ESCAPE)
			break;
		    fgw_tssfree(&pstate->lexhead, yylval.var);
		    if (t->tl_alttype && (pstate->parseroptions & FUNCHACK))
		    {
			yylval.tstart=pstate->tokstart;
			return t->tl_alttype;
		    }
		    else if (t->tl_func)
		    {
			yylval.fntok=t;
			return t->tl_type;
		    }
		    else
		    {
			yylval.tstart=pstate->tokstart;
			return t->tl_type;
		    }
		}
	    }
	    return VAR;
	}
/*
** delimited identifiers
*/
	else if (c=='\"' && (delimident || (dir==PD_IDENT)))
	{
	    yylval.var=getstring_sql(&(pstate->lexhead));
	    pstate->toklen=pstate->ibufp-pstate->tokstart;
	    if (yylval.var)
		return VAR;
	    else
		return TOK_ERROR;
	}
/*
** strings
*/
	else if (c=='\'' || c=='\"')
	{
	    yylval.stack.type=CSTRINGTYPE;
	    yylval.stack.val.string=getstring_sql(&(pstate->lexhead));
	    pstate->toklen=pstate->ibufp-pstate->tokstart;
	    if (yylval.stack.val.string)
		return CNST;
	    else
		return TOK_ERROR;
	}
/*
** operators
*/
	if ((c1=(thischar)())<0)
	    return TOK_ERROR;
	if (c=='<' && c1=='>')
	{
	    pstate->ibufp++;
	    pstate->toklen=2;
	    return NEQ;
	}
	else if ((r=GEQ, c=='>') || (r=LEQ, c=='<') ||
		 (r=EQ, c=='=') || (r=NEQ, c=='!'))
	{
	    if (c1=='=')
	    {
		pstate->ibufp++;
		pstate->toklen=2;
	    	return r;
	    }
	}
	else if (c=='|' && c1=='|')
	{
	    pstate->ibufp++;
	    pstate->toklen=2;
	    return DPIPE;
	}
	else if (c==':' && c1==':')
	{
	    pstate->ibufp++;
	    pstate->toklen=2;
	    return OPCAST;
	}
/*
** comments
*/
	else if (c=='{')
	{
	    char *cs=TOKPTR(pstate->ibufp);

	    c=(thischar)();
	    while (c && c!='}')
	    {
		if (c<0)
		    return TOK_ERROR;
		if (c=='\n')
		    pstate->phase1.line_count++;
	 	pstate->ibufp++;
		c=(thischar)();
	    }
	    newdir=getdir_sql(cs, TOKPTR(pstate->ibufp-1));
	    pstate->toklen=pstate->ibufp-pstate->tokstart;
	    if (!c)
	    {
		status=RC_BADCMT;
		return 0;
	    }
	    pstate->ibufp++;
	    continue;
	}
	else if (c=='-')
	{
	    if ((c1=(thischar)())<0)
		return TOK_ERROR;
	    if (c1=='-')
	    {
		char *cs=TOKPTR(++pstate->ibufp); 

		c=(thischar)();
		while (c && c!='\n')
		{
		    if (c<0)
			return TOK_ERROR;
		    pstate->ibufp++;
		    c=(thischar)();
		}

		newdir=getdir_sql(cs, TOKPTR(pstate->ibufp-1));
		pstate->toklen=pstate->ibufp-pstate->tokstart;

		/* last line, was a comment bail out */
		if (!c)
		    break;
		pstate->ibufp++;
		pstate->phase1.line_count++;
		continue;
	    }
	}
/*
** everything else
*/
	pstate->toklen=1;
/*
** in order to avoid sql injection from the expansion facility,
** we can disallow statement terminators coming from the expansion
*/
	if (c==';' && (pstate->flags & PF_INJECTION) &&
	    pstate->phase1.expstart>=0 &&
	    pstate->ibufp>pstate->phase1.expstart)
	{
	    status=RC_EXPINJ;
	    return TOK_ERROR;
	}
	return c;
    }
    pstate->prevtokstart=pstate->tokstart;
    pstate->prevtoklen=pstate->toklen;
    pstate->tokstart=pstate->ibufp;
    pstate->toklen=0;
    pstate->ibufp++;
    return (status? TOK_ERROR: 0);
}

/*
** parser hacks
*/
static int yylex()
{
    YYSTYPE swlval;
    int t;

    if (pstate->prevtoken>=0)
    {
	t=pstate->prevtoken;
	pstate->prevtoken=-1;
	yylval=*((YYSTYPE *) pstate->prevlval);
	return t;
    }
    else if (pstate->parseroptions & EXPRHACK)
    {
	pstate->parseroptions&=~EXPRHACK;
	if (pstate->parseroptions & EXPRONLY)
	    return XPRTOK;
	if (pstate->parseroptions & EXPRLIST)
	    return LSTTOK;
	else if (pstate->parseroptions & VARONLY)
	    return VARTOK;
	else
	    return VASTOK;
    }
    else if (pstate->parseroptions & (DMLHACK | SELECTHACK | SQLHACK))
    {
	for(;;)
	{
	    t=scanner_sql();
	    switch (t)
	    {
/*
** we could actually postpone cleaning up those to when lexhead is dropped
** we do it here to avoid allocating needlessly memory
** FIXME: we could get scanner_sql to avoid allocating memory in the first place
*/
	      case VAR:
/*
** we try to capture the VALUES clause of an INSERT statement for the benefit
** of the redirection clause
*/
		if ((pstate->parseroptions & VALUESHACK) &&
		    !strcmp(yylval.var, "values"))
		    pstate->parseroptions&=~VALUESHACK;
		fgw_tssfree(&pstate->lexhead, yylval.var);
		break;
	      case CNST:
		if (yylval.stack.type==CSTRINGTYPE)
		    fgw_tssfree(&pstate->lexhead, yylval.stack.val.string);
		break;
	      case '$':
	      case '%':
	      case '?':
		if (pstate->ssp && pstate->ssp->stmt)
		    pstate->ssp->stmt->phcount++;
		break;
	      case INTO:
		if (!(pstate->parseroptions & SELECTHACK))
		    break;
		swlval=yylval;
		t=scanner_sql();
		if (t==VAR)
		{
		    pstate->parseroptions&=~(DMLHACK | SELECTHACK);
		    pstate->prevtoken=t;
		    *((YYSTYPE *) pstate->prevlval)=yylval;
		    yylval=swlval;
		    return INTO;
		}
		if (t==CNST && yylval.stack.type==CSTRINGTYPE)
		    fgw_tssfree(&pstate->lexhead, yylval.stack.val.string);
		break;
	      case INSERT:
	      case EXECUTE:
	      case CALL:
		if (!(pstate->parseroptions & SELECTHACK))
		    break;
		pstate->parseroptions=(pstate->parseroptions | STMTHACK) &
			~SELECTHACK;
		goto redirhack;
	      case DO:
		if (!(pstate->parseroptions & FOREACHHACK))
		    break;
	      case AGGREGATE:
	      case FORMAT:
		if (!(pstate->parseroptions & SELECTHACK))
		    break;

		/* if not using the using clause in dml, remove SQLHACK for USING */
	      case USING:
	      case CONNECTION:
		pstate->parseroptions&=~(DMLHACK | SELECTHACK | SQLHACK);
		return t;
	      case ';':
		markend_sql(pstate->ibufp-1);
		t=0;
	      case 0:
		if (pstate->parseroptions & LASTHACK)
		{
		    pstate->prevtoken=t;
		    t=LASTOK;
		}
#ifdef PUREPARSER
		pstate->phase1.next_stmt_start=pstate->ibufp;
#endif
	      case TOK_ERROR:
/*
** syntax error
*/
		return t;
	    }
	}
    }
    t=scanner_sql();
redirhack:
    if (pstate->parseroptions & (STMTHACK | FOREACHHACK))
    {
	swlval=yylval;
	switch (t)
	{
/*
** differentiate between EXECUTE FUNCTION/PROCEDURE & EXECUTE
*/
	  case EXECUTE:
	    pstate->prevtoken=scanner_sql();
	    if (pstate->prevtoken==FUNCTION || pstate->prevtoken==PROCEDURE)
	    {
		pstate->prevtoken=SQLTOK;
		pstate->parseroptions|=SELECTHACK;
	    }
	    else
		*((YYSTYPE *) pstate->prevlval)=yylval;
	    pstate->parseroptions&=~(STMTHACK|FOREACHHACK);
	    break;
	}
	yylval=swlval;
    }
    if (pstate->parseroptions & STMTHACK)
    {
	swlval=yylval;
	switch (t)
	{
	  case VAR:
	    t=SQLTOK;
	    break;
/*
** avoid one R/R conflict between create proc and create something else
** VAR or CNST removed from tss at stmt end
*/
	  case CREATE:
	    pstate->prevtoken=scanner_sql();
	    if (pstate->prevtoken!=FUNCTION && pstate->prevtoken!=PROCEDURE)
	    {
		t=SQLTOK;
		pstate->prevtoken=-1;
	    }
	    break;
/*
** same for set connection & set something else
*/
	  case S_SET:
	    pstate->prevtoken=scanner_sql();
	    if (pstate->prevtoken!=CONNECTION)
	    {
		t=SQLTOK;
		pstate->prevtoken=-1;
	    }
	    break;
/*
** same for begin immediate/compound & begin work
*/
	  case BEGIN:
	    pstate->prevtoken=scanner_sql();
	    if (pstate->prevtoken!=IMMEDIATE && pstate->prevtoken!=COMPOUND)
	    {
		t=SQLTOK;
		pstate->prevtoken=-1;
	    }
	    break;
/*
** avoid shift reduce conflicts for END IMMEDIATE & COMPOUND vs
** END PROCEDURE & FUNCTION vs END IF & FOR & FOREACH & WHILE (keeps grammar
** simpler)
*/
	  case END:
	    switch (pstate->prevtoken=scanner_sql())
	    {
	      case IMMEDIATE:
	      case COMPOUND:
		t=S_END;
		break;
	      case IF:
	      case FOR:
	      case FOREACH:
	      case WHILE:
	      case CLONE:
	      case WAIT:
		t=C_END;
		break;
	    }
	    break;
	}
	yylval=swlval;
	pstate->parseroptions&=~STMTHACK;
    }
    switch (t)
    {
      case INSERT:
	pstate->parseroptions|=VALUESHACK;
      case DELETE:
      case UPDATE:
	pstate->parseroptions|=DMLHACK;
	break;
      case SELECT:
      case CALL:
	pstate->parseroptions|=SELECTHACK;
	break;
      case SQLTOK:
	pstate->parseroptions|=SQLHACK;
	break;
      case FOREACH:
	pstate->parseroptions|=FOREACHHACK;
	break;
      case PREPARE:
	pstate->parseroptions|=PREPAREHACK;
	break;
      case FROM:
	if (pstate->parseroptions & PREPAREHACK)
	    pstate->parseroptions|=STMTHACK;
	pstate->parseroptions&=~PREPAREHACK;
	break;
      case ';':
	if (pstate->parseroptions & (EXPRONLY|EXPRLIST|VARONLY))
	    break;
      case 0:
	if (pstate->parseroptions & LASTHACK)
	{
	    pstate->prevtoken=0;
	    t=LASTOK;
	}
#ifdef PUREPARSER
	pstate->phase1.next_stmt_start=pstate->ibufp;
#endif
    }
    return t;
}

/*
** initiliazes module
*/
static void init_sqp()
{
    tokenlist_t src[NUMTOKENS], temp[NUMTOKENS], *t;
    int i, b, f;
    char *c;
    long h;

    if (delimident>=0)
	return;
    delimident=(getenv("DELIMIDENT")!=NULL);
    fgw_srand(time(NULL));
/*
** initialize token lists
*/
    for (i=0; i<NUMTOKENS; i++)
	src[i]=tokenlist[i];
    memset(&tokenlist, 0, sizeof(tokenlist));
    memset(&temp, 0, sizeof(temp));
    b=0;
    for (i=0; i<NUMTOKENS; i++)
    {
/*
** hash name, determine bucket
*/
	h=fgw_hash(src[i].tl_name, NUMTOKENS);
/*
** oops, bucket already in use
*/
	if (tokenlist[h].tl_name)
	{
	    temp[b]=src[i];
	    temp[b++].tl_next=&tokenlist[h];
	}
	else
	    tokenlist[h]=src[i];
    }
/*
** insert remaining tokens
*/
   f=0;
   for (i=0; i<b; i++)
   {
/*
** scan bucket chain
*/
	t=temp[i].tl_next;
	while (t->tl_next)
	    t=t->tl_next;
/*
** find next free entry
*/
	while (tokenlist[f].tl_name)
	    f++;
	tokenlist[f]=temp[i];
	tokenlist[f].tl_next=NULL;
	t->tl_next=&tokenlist[f];
    }
}

/*
** scans input buffer for variable name
*/
static char *getvar_sql(fgw_tsstype **hd)
{
    char *v, *s, *d;
/*
** this takes into account the fact that we have already scanned
** the first character of the name in scanner_sql()
*/
    int n, l=1;

    if ((n=(thischar)())<0)
	return NULL;
    while ((n>='a' && n<='z') || (n>='A' && n<='Z') ||
	   (n>='0' && n<='9') || (n=='_'))
    {
	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    return NULL;
	l++;
    }
    if (v=(char *) fgw_tssmalloc(hd, l))
    {
	d=v;
	s=TOKPTR(pstate->ibufp-l);
	while (l--)
	{
	   *d=fgw_tolower(*s);
	   fgw_chrnext(s);
	   fgw_chrnext(d);
	}
	*d='\0';
    }
    else
	status=RC_NOMEM;
    return v;
}

/*
** scans input buffer for integer
*/
static int getdigits_sql(int base)
{
    int n, max='0'+base, hexmax=0;
    int r=0;

    if (max>'9')
    {
	max='9';
	hexmax='a'+base-11;
    }
    if ((n=(thischar)())<0)
	return -1;
    while (n)
    {
	if (n>='0' && n<=max)
	    r=r*base+n-'0';
	else if (hexmax && (n=tolower(n))>='a' && n<=hexmax)
	    r=r*base+n-'a'+10;
	else
	    break;
	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    return -1;
    }
    return r;
}

/*
** scans input buffer for double decimal places and exponent
*/
static int getfloat_sql(int i, double *f)
{
    int n;
    double d=10.0;

    *f=i;
    if ((n=(thischar)())<0)
	return -1;
    while (n>='0' && n<='9')
    {
	*f+=((n-'0')/d);
	d*=10.0;
	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    return -1;
    }
    if (n=='e' || n=='E')
    {
	int mult=1, exp=0;

	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    return -1;
	if (n=='-')
	{
	    mult=-1;
	    pstate->ibufp++;
	    if ((n=(thischar)())<0)
		return -1;
	}
	else if ((n=='+'))
	{
	    pstate->ibufp++;
	    if ((n=(thischar)())<0)
		return -1;
	}
	if ((exp=getdigits_sql(10))<0)
	    return -1;
	else
	    *f*=pow(10, mult*exp);
    }
    return 0;
}

/*
** scans input buffer for string
*/
static char *getstring_sql(fgw_tsstype **h)
{
    char *sbuf=NULL;
    int n, d;
    int s, sz=0, newsz=0;

    /* we should normally use (thischar)() but here it's fine */
    d=*TOKPTR(pstate->ibufp-1);
    s=pstate->ibufp;
    for(;;)
    {
	if ((n=(thischar)())<0)
	    goto bad_exit;
/*
** scan till end of quote
*/
	while ((n) && (n!=d))
	{
	    if (n=='\n')
		pstate->phase1.line_count++;
	    pstate->ibufp++;
	    if ((n=(thischar)())<0)
		goto bad_exit;
	    newsz++;
	}
/*
** none found, bail out
*/
	if (!n)
	{
	    status=RC_BADQT;
	    goto bad_exit;
	}
/*
** allocate new memory
*/
	if (!(sbuf=(char *) fgw_tssrealloc(h, sbuf, sz+newsz)))
	{
	    status=RC_NOMEM;
	    goto bad_exit;
	}
/*
** copy string & adjust sizes
*/
	if (newsz)
	    fgw_move(sbuf+sz, TOKPTR(s), newsz);
	sz+=newsz;
	*(sbuf+sz)='\0';
/*
** single quote: we're thru
*/
	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    goto bad_exit;
	if (n!=d)
	    return sbuf;
/*
** if it was two quotes instead, will have to start again
*/
	else
	{
	    newsz=1;
	    s=pstate->ibufp++;
	}
    }

bad_exit:
    if (sbuf)
	fgw_tssfree(h, sbuf);
    return NULL;
}
