%{
/*
	DB2API.y  -  DB2 source API parser

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Mar 03
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

#include <ctype.h>
#include <string.h>
#include "cosdc.h"
#include "coslc.h"
#include "csqrc.h"
#include "db2cli.h"


#ifndef YYBISON
#define yyparse yyparse_db2api     /* just in case we decide to have */
#define yylex yylex_db2api         /* multiple parsers */
#define yyerror yyerror_db2api
#define yyval yyval_db2api
#define yylval yylval_db2api
#define yyvs yyvs_db2api
#define yycheck yycheck_db2api
#define yychar yychar_db2api
#define yylhs yylhs_db2api
#define yylen yylen_db2api
#define yydefred yydefred_db2api
#define yydgoto yydgoto_db2api
#define yysindex yysindex_db2api
#define yyrindex yyrindex_db2api
#define yygindex yygindex_db2api
#define yytable yytable_db2api
#endif

/*
** statement id, error code
*/
static TGLOB int result;
static TGLOB int *sqlcode;

/*
** parser kludges
*/
static int kludges;

#define NOOPTION 0
#define CONNECTHACK 1
#define APIHACK 2

/*
** scanner state
*/
char *ibufp;

/*
** forward declarations
*/
static void yyerror(char *s);
static int yylex();
%}

%nonassoc CONNECT API
%nonassoc IN SHARE EXCLUSIVE MODE
%nonassoc CALL
%nonassoc CHANGE ISOLATION TO UR CS RR RS SQLTOK

/*
** Grammar
*/

%% 
target:   CONNECT connect {
			}
	
	| API api	{
			}
;
connect:  /* empty */	{
			}
	| IN SHARE MODE {
			}
	| IN EXCLUSIVE MODE {
			    result=CLI_EXCLUSIVE;
			}
;
api:	  CHANGE ISOLATION TO iso {
			}
	| CHANGE SQLTOK	{
			    YYACCEPT;
			}
	| CALL SQLTOK	{
			    result=CLI_CALL;
			}
	| SQLTOK	{
			    YYACCEPT;
			}
;
iso:	  UR		{
			    result=CLI_UR;
			}
	| CS		{
			    result=CLI_CS;
			}
	| RR		{
			    result=CLI_RR;
			}
	| RS		{
			    result=CLI_RS;
			}
;
%%

/*
** the parser
*/
int db2api_parser(char *buffer, int mode, int *sc)
{
    kludges=mode? CONNECTHACK: APIHACK;
    ibufp=buffer;
    result=0;
    sqlcode=sc;
    *sqlcode=0;
    yyparse();
    return result;
}

/*
** error handler
*/
static void yyerror(char *s)
{
    if (!result)
	*sqlcode=RC_PSNTX;
}

static struct tlist {
    char *name;
    int token;
} tokenlist[] = {
    "in", IN,
    "share", SHARE,
    "exclusive", EXCLUSIVE,
    "mode", MODE,
    "call", CALL,
    "change", CHANGE,
    "isolation", ISOLATION,
    "to", TO,
    "ur", UR,
    "cs", CS,
    "rr", RR,
    "rs", RS,
    NULL, 0
};

/*
** scanner
*/
static int db2api_scanner()
{
    struct tlist *t;
    char *s, name[40]="";
    int l, i;

    s=(char *) &name;
    i=sizeof(name)-1;
    while (*ibufp)
    {
/*
** either side of a token
*/
	if (*ibufp==' ' || *ibufp=='\t' || *ibufp=='\n')
	{
	    ibufp++;
/*
** haven't scanned for a token yet - keep on skipping
*/
	    if (!strlen(name))
		continue;
	    else
		break;
	}
/*
** good character
*/
	*s=tolower(*ibufp++);
	*++s=0;
	if (!--i)
	   break;
    }
/*
** end of statement
*/
    if (!strlen(name))
	return 0;
/*
** scan keywords
*/
    for (t=(struct tlist *) &tokenlist; t->name; t++)
	if (!strcmp(name, t->name))
	    return t->token;
    return SQLTOK;			/* let the parser complain */
}

/*
** parser hacks
*/
static int yylex()
{
    if (kludges & CONNECTHACK)
    {
	kludges&=~CONNECTHACK;
	return CONNECT;
    }
    else if (kludges & APIHACK)
    {
	kludges&=~APIHACK;
	return API;
    }
    else return db2api_scanner();
}
