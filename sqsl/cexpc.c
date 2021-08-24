/*
	CEXPC.c  -  SQSL interpreter: expansion facility

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)

	Initial release: May 05
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

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "ccmnc.h"
#include "ctypc.h"
#include "cfioc.h"
#include "cexpc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqxp.h"

#define SCRIPTBUF 32767

#define TK_DONE 0
#define TK_BEGEXP 1
#define TK_ENDEXP 2
#define TK_BEGCMT 3
#define TK_ENDCMT 4
#define TK_ESCAPE 5
#define TK_NEWLINE 6
#define TK_BORING 7

typedef struct
{
    int tokid;
    char *token;
    int minlen;
} toklist_t;

static toklist_t toklist[] = {
    TOK_READ, "read", 1,
    TOK_EXEC, "exec", 1,
    TOK_SELECT, "select", 6,
    TOK_GET, "get", 1,
    TOK_PROMPT, "prompt", 2,
    TOK_PASSWD, "passwd", 2,

    TOK_SILENT, "silent", 3,
    TOK_CODED, "coded", 1,
    TOK_LONE, "lone", 1,
    TOK_INCLUDE, "include", 1,
    TOK_PUT, "put", 2,

    TOK_QUOTES, "quotes", 1,
    TOK_SEPARATOR, "separator", 3,
    TOK_SINGLE, "single", 3,
    TOK_MULTI, "multiple", 1,
    0, NULL, 0
};

typedef struct
{
    char *ep_storage;
    char *ep_current;
    int ep_size;
    int ep_len;
} expbuf_t;

static int get_exptoken(char *c, char *e);
static int fgw_evalget(char *instr, expbuf_t *expbuf, char *separator,
                       char *quotes, fgw_loc_t *vars, int opts);
static int fgw_evalput(char *instr, hst_t *outvar, fgw_loc_t *vars, char **n);
static int fgw_evalstr(char *instr, expbuf_t *expbuf, fgw_loc_t *vars, char **n);
static int fgw_evalopt(char *instr, char *expbuf, int l, fgw_loc_t *vars, char **n);
static int fgw_evalsilent(int tokid, int opts, char *instr,
                          expbuf_t *expbuf, char *separator, char *quotes);
static int fgw_streamsilent(fgw_fdesc *sd, expbuf_t *expbuf, char *quotes,
                            char *separator, int sl, int multi);
static int fgw_streaminclude(fgw_fdesc *sd, expbuf_t *expbuf);
static int fgw_expandlist(expbuf_t *expbuf, int loops, char *quotes,
                          char *separator, int sl, char *instr);
static int allocate_ep(expbuf_t *ebp);
static int appendchar_ep(expbuf_t *ebp, char c);
static int appendstring_ep(expbuf_t *ebp, char *c, int l);

#undef TOKPTR
#define TOKPTR(s)    (state.o_query+(s))

/*
** expansion facility
*/
int fgw_sqlexplode(char *src, int srclen, fgw_loc_t *vars, int verbose, int expstate,
		   int iidx, int *oidx, char **expptr, int *s, int *e, int *n, execinfo_t *execinfo)
{
    int qbase=0;
    char *cbase;
    char *iptr, *sptr, *eptr;
    int rc;
    int ccnt=0;
    fgw_stacktype expstack;

    char *t, *ot;
    toklist_t *tl;
    int tokid, opts;

    char quotes[3], separator[81];
    hst_t putdest;

    expbuf_t outbuf;
    expbuf_t expbuf;
    expbuf_t quebuf;
    int buflen=SCRIPTBUF;

    memset(&outbuf, 0, sizeof(outbuf));
    memset(&expbuf, 0, sizeof(expbuf));
    memset(&quebuf, 0, sizeof(quebuf));
    fgw_stackinit(&expstack, sizeof(int));
    *expptr=NULL;
    if (n)
	*n=0;

    /* nothing to expand in an empty string */
    if (srclen==0 && iidx==1)
	goto nomore;
/*
** just to be on the safe side
*/
    if (iidx<1 || iidx>srclen)
    {
	rc=-1;
	goto oopsadaisy;
    }
    sptr=src+iidx-1;
    eptr=src+srclen;
    iptr=sptr;
    for (;;)
    {
	switch (get_exptoken(iptr, eptr))
	{
/*
** we have an escape
*/
	  case TK_ESCAPE:
	    switch (get_exptoken(iptr+1, eptr))
	    {
/*
** alas, there's not much after the escape - so don't have much else to do
*/
	      case TK_DONE:
		break;
/*
** keep track of newlines as we encounter them inside expansions
*/
	      case TK_NEWLINE:
		if (n && (ccnt || expstack.count))
		    (*n)++;
/*
** nothing interesting, save and continue
*/
	      case TK_BORING:
		if (!ccnt & expstack.count &&
		    (rc=appendstring_ep(&quebuf, iptr, 2)))
		    goto oopsadaisy;
		iptr+=2;
		continue;
/*
** valid expansion to be escaped
*/
	      default:
/*
** inside comment, just skip
*/
		if (ccnt)
		    iptr+=3;
/*
** inside expansion, copy in buffer
*/
		else if (expstack.count)
		{
		    if (rc=appendstring_ep(&quebuf, ++iptr, 2))
			goto oopsadaisy;
		    iptr+=2;
		}
/*
** neither, pass back to previous layer
*/
		else
		{
		    *e=iptr-sptr;
		    if (rc=appendstring_ep(&outbuf, ++iptr, 2))
			goto oopsadaisy;
		    iptr+=2;
		    goto wayout;
		}
		continue;
	    }
	    /* fallthrough */
/*
** we're done
*/
	  case TK_DONE:
	    goto nomore;
/*
** Comment start
*/
	  case TK_BEGCMT:
	    if (!ccnt && !expstack.count)
		cbase=iptr;
	    ccnt++;
	    iptr+=2;
	    break;
/*
** Comment end
*/
	  case TK_ENDCMT:
	    ccnt--;
	    iptr+=2;
	    if (ccnt>0)
		continue;
	    else if (ccnt<0)
	    {
		rc=RC_XNBLC;
		goto oopsadaisy;
	    }
	    else if (expstack.count)
		continue;
	    else
	    {
		*e=cbase-sptr;
		goto wayout;
	    }
/*
** Expansion start
*/
	  case TK_BEGEXP:
/*
** within comment, skip
*/
	    if (ccnt)
	    {
		iptr+=2;
		continue;
	    }
/*
** we postpone expansion execution until after the sql interpreter has
** parsed & executed whatever command is already available, so that
** we can take advantage of it. this does not affect the usage of the
** expansion facility by other modules
*/
	    else if (expstack.count==0)
	    {
		if (*e=iptr-sptr)
		    goto wayout;
/*
** we don't have to allocate now, but having an allocated expansion
** buffer, even if empty, signals to the caller that there has been
** an expansion, and the expansion buffer has to be allocated
** for those function that don't handle automatic reallocation
*/
		if ((rc=allocate_ep(&outbuf)) ||
		    (rc=allocate_ep(&expbuf)))
		    goto oopsadaisy;
	    }
	    iptr+=2;
	    fgw_stackpush((fgw_stacktype *) &expstack, (char *) &quebuf.ep_len);
	    break;
/*
** Expansion end
*/
	  case TK_ENDEXP:
	    iptr+=2;
/*
** within comment, skip
*/
	    if (ccnt)
		continue;
/*
** unbalanced expansion, tsk! tsk! tsk!
*/
	    else if (!expstack.count)
	    {
		rc=RC_XNBLC;
		goto oopsadaisy;
	    }
	    qbase=*(int *) fgw_stackpop((fgw_stacktype *) &expstack);
	    t=quebuf.ep_storage+qbase;
	    opts=0;
	    putdest=0;
	    quotes[0]='\0';
	    separator[0]='\0';
/*
** parse expansion and expansion options
*/
	    for (;;)
	    {
/*
** skip separators
*/
		while (*t && strchr(" \t\n", *t))
		    t++;
		ot=t;
		tokid=0;
/*
** identify next token
*/
		for (tl=(toklist_t *) &toklist; tl->tokid; tl++)
		{
		    char *tptr=tl->token;
		    int tlen=tl->minlen;

/*
** do lowercase comparison
*/
		    for (;tolower(*t)==*tptr; t++, tptr++, tlen--);
/*
** if at the end of a token and on a separator boundary, skip separators
** and return the match
*/
		    if (tlen<=0 && strchr(" \t\n", *t))
		    {
			while (*t && strchr(" \t\n", *t))
			    t++;
			tokid=tl->tokid;
			break;
		    }
		    else
			t=ot;
		}
		if (tokid==TOK_READ)
		    break;
		else if (tokid==TOK_EXEC)
		    break;
		else if (tokid==TOK_SELECT)
		{
		    t=ot;
		    break;
		}
		else if (tokid==TOK_GET && !(opts & MOD_OP))
		    break;
		else if (tokid==TOK_PROMPT && !(opts & MOD_OPTS))
		    break;
		else if (tokid==TOK_PASSWD && !(opts & MOD_OPTS))
		    break;

		else if (tokid==TOK_LONE && !(opts &
					(MOD_COLS|TOK_SILENT|TOK_INCLUDE)))
		    opts|=tokid;
		else if (tokid==TOK_CODED && !(opts &
						(MOD_COLS|TOK_SILENT|
						 TOK_INCLUDE)))
		    opts|=tokid;
		else if (tokid==TOK_SILENT && !(opts & (MOD_COLS|TOK_INCLUDE)))
		    opts|=tokid;
		else if (tokid==TOK_INCLUDE && !(opts & (MOD_COLS|TOK_SILENT)))
		    opts|=tokid;

		else if (tokid==TOK_PUT)
		{
		    if (rc=fgw_evalput(t, &putdest, vars, &ot))
			goto oopsadaisy;
		    t=ot;
		}
		else if (tokid==TOK_QUOTES && !(opts & TOK_SINGLE))
		{
		    if (rc=fgw_evalopt(t, (char *) &quotes, sizeof(quotes)-1, vars,
					(char **) &ot))
			goto oopsadaisy;
		    t=ot;
		}
		else if (tokid==TOK_SEPARATOR && !(opts & TOK_SINGLE))
		{
		    if (rc=fgw_evalopt(t, (char *) &separator, sizeof(separator)-1,
					vars, (char **) &ot))
			goto oopsadaisy;
		    t=ot;
		}
		else if (tokid==TOK_SINGLE && !(opts & MOD_ROWS))
		    opts|=tokid;
		else if (tokid==TOK_MULTI && !(opts & TOK_SINGLE))
		    opts|=tokid;
/*
** whatever it is it's not right
*/
		else
		{
		    rc=RC_EXSTX;
		    goto oopsadaisy;
		}
	    }
/*
** fix some defaults
*/
	   if (tokid!=TOK_PROMPT && tokid!=TOK_PASSWD &&
	       !(opts & MOD_ROWS))
	   {
		if (expstate==ES_SILENT)
		    opts|=TOK_SILENT;
		else
		    opts|=TOK_SINGLE;
	   }
	   if (tokid!=TOK_PROMPT && tokid!=TOK_PASSWD &&
	       !(opts & (TOK_SILENT|MOD_COLS)))
		opts|=TOK_CODED;
/*
** and now that we have something to do, better not hang about
*/
	   if (expstate!=ES_DISABLED)
	   {
		expbuf.ep_current=expbuf.ep_storage;
		expbuf.ep_len=0;
		*expbuf.ep_current='\0';
		if (tokid==TOK_GET)
		{
		    if (rc=fgw_evalget(t, &expbuf, separator,
					quotes, vars, opts))
			goto oopsadaisy;
		}
		else if (opts & (TOK_SILENT|TOK_INCLUDE))
		{
		    rc=fgw_evalsilent(tokid, opts, t, &expbuf,
						separator, quotes);
		    if (rc==RC_NOTFOUND)
			rc=RC_NDATA;
		    if (rc)
			goto oopsadaisy;
/*
** recursively process included text
*/
		    if ((opts & TOK_INCLUDE) && expbuf.ep_len)
		    {
			int ip, op, s, e, l, len, n;
			char *src, *eb=NULL;

			src=expbuf.ep_storage;
			len=expbuf.ep_len;
			memset(&expbuf, 0 , sizeof(expbuf));
			for (ip=1;;)
			{
			    if (rc=fgw_sqlexplode(src, len, vars, verbose,
					      expstate, ip, &op, &eb,
					      &s, &e, &n, execinfo))
				break;
			    if (s>=0 && (l=e-s+1) &&
				(rc=appendstring_ep(&expbuf, src+s-1, l)))
				break;
			    if (eb)
			    {
				if (rc=appendstring_ep(&expbuf, eb, 0))
				    break;
				free(eb);
			    }
			    eb=NULL;
			    if (op<0)
				break;
			    ip=op;
			}
			if (eb)
			    free(eb);
			free(src);
			if (rc)
			     goto oopsadaisy;
		    }
		}
		else if (expstate==ES_SILENT)
		{
		    rc=RC_NWNDW;
		    goto oopsadaisy;
		}
		else if (tokid==TOK_PROMPT || tokid==TOK_PASSWD)
		{
		    char *d;
		    int l=expbuf.ep_size;

		    if (rc=fgw_evalstr(t, &expbuf, vars, &d))
			goto oopsadaisy;
		    if (rc=sqsl_promptpasswd(tokid, opts, expbuf.ep_storage,
					     expbuf.ep_storage, &l, verbose, execinfo))
			goto oopsadaisy;
		}
		else
		{
		    int l=expbuf.ep_size;
		    if (rc=sqsl_picklist(tokid,  opts, t, expbuf.ep_storage,
					 &l, separator, quotes, verbose, execinfo))
			goto oopsadaisy;
		}
/*
** store result if necessary
*/
		if (putdest)
		    fgw_hstidxadd(vars, putdest, CSTRINGTYPE, expbuf.ep_len,
				  expbuf.ep_storage);

/* FIXME we dont handle expansions coming from selects or files */
/*
** we done yet, or still expansions in the stack?
*/
		if (expstack.count)
		{
/*
** bit of a hack, but truncate the storage
*/
		    quebuf.ep_len=qbase;
		    quebuf.ep_current=quebuf.ep_storage+qbase;
		    *quebuf.ep_current='\0';
		    appendstring_ep(&quebuf, expbuf.ep_storage, expbuf.ep_len);
		    continue;
		}
		else
		{
		    appendstring_ep(&outbuf, expbuf.ep_storage, expbuf.ep_len);
		    goto wayout;
		}
	    }
	    else if (expstack.count)
		continue;
	    else
		goto wayout;
/*
** keep track of newlines as we encounter them inside expansions
*/
	  case TK_NEWLINE:
	    if (n && (ccnt || expstack.count))
		(*n)++;
/*
** as we were saying, nothing interesting
*/
	  case TK_BORING:
/*
** copy in expansion buffer if in an active expansion
*/
	     if (!ccnt && expstack.count &&
		 (rc=appendchar_ep(&quebuf, *iptr)))
		goto oopsadaisy;
	     iptr++;
	}
    }

/*
** *e carries the length of the string that can be copied verbatim
** iptr is the character we're supposed to restart from
*/ 
wayout:
    fgw_stackfree(&expstack, NULL);
    if (!*e)
    {
	*s=-1;
	*e=-1;
    }
    else
    {
	*e+=iidx-1;
	*s=iidx;
    }
    if ((*oidx=iptr-src+1)>srclen)
	*oidx=-1;
    *expptr=outbuf.ep_storage;
    if (expbuf.ep_storage)
	free(expbuf.ep_storage);
    if (quebuf.ep_storage)
	free(quebuf.ep_storage);
    return 0;

nomore:
    if (ccnt || expstack.count)
    {
	rc=RC_XNBLC;
	goto oopsadaisy;
    }

thatsallfolks:
    fgw_stackfree(&expstack, NULL);
    *oidx=-1;
    *s=iidx;
    *e=srclen;
    *expptr=outbuf.ep_storage;
    if (expbuf.ep_storage)
	free(expbuf.ep_storage);
    if (quebuf.ep_storage)
	free(quebuf.ep_storage);
    return 0;

oopsadaisy:
    fgw_stackfree(&expstack, NULL);
    *oidx=-1;
    *expptr=outbuf.ep_storage;
    if (expbuf.ep_storage)
	free(expbuf.ep_storage);
    if (quebuf.ep_storage)
	free(quebuf.ep_storage);
    *s=-1;
    *e=-1;
    return rc;
}

/*
** allocates expansion storage
*/
static int allocate_ep(expbuf_t *ebp)
{
    char *buf;

    if (!(buf=realloc(ebp->ep_storage, ebp->ep_size+SCRIPTBUF)))
	return RC_NOMEM;
    if (!ebp->ep_storage)
	*buf='\0';
    ebp->ep_storage=buf;
    ebp->ep_current=ebp->ep_storage+ebp->ep_len;
    ebp->ep_size+=SCRIPTBUF;
    return 0;
}

/*
** appends char to expansion storage
*/
static int appendchar_ep(expbuf_t *ebp, char c)
{
    int rc;

    if (ebp->ep_len+1>=ebp->ep_size &&
	(rc=allocate_ep(ebp)))
	return rc;
    ebp->ep_len++;
    *ebp->ep_current++=c;
    *ebp->ep_current='\0';
    return 0;
}

/*
** appends string to expansion storage
*/
static int appendstring_ep( expbuf_t *ebp, char *c, int l)
{
    int el, rc;

    if (!l)
	l=strlen(c);
    el=ebp->ep_len+l;
    if (el>=ebp->ep_size &&
	(rc=allocate_ep(ebp)))
	return rc;
    strncpy(ebp->ep_storage+ebp->ep_len, c, l);
    ebp->ep_len=el;
    ebp->ep_current+=l;
    *ebp->ep_current='\0';
    return 0;
}

/*
** identifies next expansion token
*/
static int get_exptoken(char *c, char *e)
{

    if (c>=e)
	return TK_DONE;
    switch (*c)
    {
      case '\0':
	return TK_DONE;
      case '\n':
	return TK_NEWLINE;
      case '\\':
	return TK_ESCAPE;
      case '<':
	if (*(c+1)=='+')
	    return TK_BEGEXP;
	else if (*(c+1)=='*')
	    return TK_BEGCMT;
	return TK_BORING;
      case '+':
	if (*(c+1)=='>')
	    return TK_ENDEXP;
	return TK_BORING;
      case '*':
	if (*(c+1)=='>')
	    return TK_ENDCMT;
      default:
	return TK_BORING;
    }
}

/*
** parses get expression list
*/
static int fgw_evalget(char *instr, expbuf_t *expbuf, char *separator,
		       char *quotes, fgw_loc_t *vars, int opts)
{
    parserstate_t state;                /* passed in bulk to yyparse */
    char *r, cbuf[30];
    int cnt, loops;
    int sl=separator? strlen(separator): 0;
    int rc;
    exprstack_t *exprlist;
 
    memset(&state, 0, sizeof(parserstate_t));
    state.vars=vars;
    state.o_query=instr;
    state.phase1.output_buffer_len=strlen(instr);
    state.parseroptions=EXPRLIST;
    fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
    sql_parserinit(&state);
    if ((rc=sql_parser(&state))!=0)
    	goto exit;
    else if (!state.ibufp)
    {
	rc=RC_XEVAL;
	goto exit;
    }
    exprlist=rxu_arglist(&state.exprlist, &cnt);
    if (!cnt || ((opts & TOK_LONE) && cnt>1))
    {
	rc=RC_XEVAL;
	goto exit;
    }

    for (loops=0; loops<cnt; loops++, exprlist++)
	if ((rc=fgw_expandlist(expbuf, loops, quotes, separator, sl,
			   rxu_tostring(exprlist, (char *) &cbuf, NULL))))
	    break;

exit:
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    sql_parserfree(&state);
    return rc;
}

/*
** parses put variable
*/
static int fgw_evalput(char *instr, hst_t *outvar, fgw_loc_t *vars, char **n)
{
    parserstate_t state;                /* passed in bulk to yyparse */
    int rc;
    exprstack_t *exprlist;
 
    memset(&state, 0, sizeof(parserstate_t));
    state.vars=vars;
    state.o_query=instr;
    state.phase1.output_buffer_len=strlen(instr);
    state.parseroptions=VARONLY;
    fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
    sql_parserinit(&state);
    if (!(rc=sql_parser(&state)))
	if (!state.ibufp)
	    rc=RC_XEVAL;
	else
	{
	    exprlist=(exprstack_t *) fgw_stackpop(&state.exprlist);
	    *outvar=(hst_t) rxu_toint(exprlist, NULL);
	    *n=TOKPTR(state.ibufp);
	}
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    sql_parserfree(&state);
    return rc;
}

/*
** parses an inline expression, returns index of first invalid char
*/
static int fgw_evalstr(char *instr, expbuf_t *expbuf, fgw_loc_t *vars, char **n)
{
    parserstate_t state;                /* passed in bulk to yyparse */
    char cbuf[30];
    int rc;
    exprstack_t *exprlist;
 
    memset(&state, 0, sizeof(parserstate_t));
    state.vars=vars;
    state.o_query=instr;
    state.phase1.output_buffer_len=strlen(instr);
    state.parseroptions=EXPRONLY;
    fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
    sql_parserinit(&state);
    if (!(rc=sql_parser(&state)))
	if (!state.ibufp)
	    rc=RC_XEVAL;
	else
	{
	    exprlist=(exprstack_t *) fgw_stackpop(&state.exprlist);
	    rc=appendstring_ep(expbuf,
			rxu_tostring(exprlist, (char *) &cbuf, NULL), 0);
	    *n=TOKPTR(state.ibufp);
	}
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    sql_parserfree(&state);
    return rc;
}

/*
** parses an inline expression, returns index of first invalid char
*/
static int fgw_evalopt(char *instr, char *expbuf, int l, fgw_loc_t *vars, char **n)
{
    parserstate_t state;                /* passed in bulk to yyparse */
    char cbuf[30];
    int rc=0;
    exprstack_t *exprlist;
 
    memset(&state, 0, sizeof(parserstate_t));
    state.vars=vars;
    state.o_query=instr;
    state.phase1.output_buffer_len=strlen(instr);
    state.parseroptions=EXPRONLY;
    fgw_stackinit(&state.exprlist, sizeof(exprstack_t));
    sql_parserinit(&state);
    if (!(rc=sql_parser(&state)))
	if (!state.ibufp)
	    rc=RC_XEVAL;
	else
	{
	    exprlist=(exprstack_t *) fgw_stackpop(&state.exprlist);
	    strncat(expbuf,
		    rxu_tostring(exprlist, (char *) &cbuf, NULL), l);
	    *n=TOKPTR(state.ibufp);
	}
    fgw_stackfree(&state.exprlist, NULL);
    fgw_tssdrop(&state.exphead);
    sql_parserfree(&state);
    return rc;
}

/*
** execs silent / include expansions
*/
static int fgw_evalsilent(int tokid, int opts, char *instr,
			  expbuf_t *expbuf, char *separator, char *quotes)
{
    int r, rc;
    fgw_stmttype *stmt;
    fgw_fdesc *sd;
    exprstack_t *e;
    char cbuf[30];
    int loops=0;
    int sl=separator? strlen(separator): 0;
    char *space;
    char *filename;

/* FIXME: file & pipe to provide for filenames with spaces */
    switch (tokid)
    {
/*
** file?
*/
      case TOK_READ:
	filename=instr;
	for (;;)
	{
	    while (*filename && strchr(" \t\n", *filename))
		filename++;
	    if (!*filename)
		 break;
	    if (space=strpbrk(filename, " \t\n"))
		*space='\0';
	    if (!(sd=fgw_fileopen(filename, "r")))
	    {
		rc=errno;
		break;
	    }
	    sd->fd_linelen=0;		/* automatic reallocation */
	    if (opts & TOK_SILENT)
		r=fgw_streamsilent(sd, expbuf, quotes, separator, sl,
			       opts & TOK_MULTI);
	    else
		r=fgw_streaminclude(sd, expbuf);
	    rc=errno;
	    fgw_fdclose(sd);
	    if (r || !space || !(opts & TOK_MULTI))
		break;
	    filename=space+1;
	}
	break;
/*
** pipe
*/
      case TOK_EXEC:
	if (!(sd=fgw_pipeopen(instr, "r")))
	{
	    rc=errno;
	    break;
	}
	sd->fd_linelen=0;		/* automatic reallocation */
	if (opts & TOK_SILENT)
	    r=fgw_streamsilent(sd, expbuf, quotes, separator, sl,
			   opts & TOK_MULTI);
	else
	    r=fgw_streaminclude(sd, expbuf);
	rc=errno;
	fgw_fdclose(sd);
	break;
/*
** SQL is what is required of us
*/
      case TOK_SELECT:
/*
** initial stuff: grab a statement descriptor, set connection
*/
	if (!(stmt=rqx_newstatement(NULL)))
	{
	    rc=RC_NOMEM;
	    goto rats;
	}
	rqx_setstmtcon(stmt, rqx_getconnection(), NULL,  NULL);
/*
** prepare the statement, complain if it's not cursory
*/
	rqx_prepare(stmt, instr);
	if (rc=stmt->ca->sqlcode)
	    goto didntworkout;
	if (stmt->curstate==ST_EXECUTABLE)
	{
	    rc=RC_NDECL;
	    goto forcryingoutloud;
	}
/*
** now grab all rows
*/
	for (;;)
	{
	    rqx_nextrow(stmt);
	    if (rc=stmt->ca->sqlcode)
	    {
/*
** no more rows, not a prob if we have found something
*/
		if (rc==RC_NOTFOUND && loops)
		    rc=0;
		goto didntworkout;
	    }
	    if (!(e=(exprstack_t *) rqx_nexttoken(0, stmt)))
	    {
		rc=RC_CRNGE;
		goto forcryingoutloud;
	    }
	    if (opts & TOK_SILENT)
		rc=fgw_expandlist(expbuf, loops, quotes, separator, sl,
			   rxu_tostring(e, (char *) &cbuf, NULL));
	    else
		rc=appendstring_ep(expbuf, rxu_tostring(e, (char *) &cbuf, NULL), 0);
/*
** unless we only wanted one, that is
*/
	    if (!(opts & TOK_MULTI))
		break;
	    loops++;
	}
	    
forcryingoutloud:
didntworkout:
	rqx_freestatement(stmt);
    }
rats:
    return rc;
}

#define SLST_READ 0
#define SLST_NEWLINE 1
#define SLST_NEWTOKEN 2
#define SLST_NEXTTOKEN 3
#define SLST_ENDTOKEN 4
#define SLST_SKIPLINE 5
#define SLST_END 6

/*
** expand input from stream
*/
static int fgw_streamsilent(fgw_fdesc *sd, expbuf_t *expbuf, char *quotes,
			    char *separator, int sl, int multi)
{
    int r;
    char *tok=NULL;
    int state=SLST_READ;
    int nextstate=SLST_NEWLINE;
    int loops=0;
    char qopen, qclose='\0';

    if ((qopen=*quotes++) && !(qclose=*quotes))
	qclose=qopen;
	
    for (;;)
    {
	switch (state)
	{
/*
** state: read some more
*/
	  case SLST_READ:
	    if ((r=fgw_fdread(sd))<0)
		return -1;
	    else if (!r)
	    {
		if (loops)
	        {
		    if (nextstate!=SLST_NEXTTOKEN)
			return 0;
		    else
		    {
			state=SLST_ENDTOKEN;
			nextstate=SLST_END;
			break;
		    }
		}
		else
		{
		    errno=RC_NOTFOUND;
		    return -1;
		}
	    }
	    tok=sd->fd_buf;
	    state=nextstate;
	    break;
/*
** state: at the beginning of the line
*/
	  case SLST_NEWLINE:
/*
** if single, then we're happy with one value
*/
	    if (loops && !multi)
		return 0;
/*
** skip leading spaces
*/
	    while (*tok==' ' || *tok=='\t')
		*tok++;
	    switch (*tok++)
	    {
/*
** need to read some more
*/
	      case '\0':
		 nextstate=SLST_NEWLINE;
		 state=SLST_READ;
		 break;
/*
** skip comment
*/
	      case '#':
		state=SLST_SKIPLINE;
		break;
/*
** EOL, restart from scratch
*/
	      case '\n':
		state=SLST_NEWLINE;
		break;
/*
** have a token
*/
	      default:
		tok--;
		state=SLST_NEWTOKEN;
	    }
	    break;
/*
** state: copy token, part 1: separator and opening quote
*/
	  case SLST_NEWTOKEN:
/*
** if not first time round, then a comma is in order
*/
	    if (expbuf->ep_len && loops && *separator &&
		(errno=appendstring_ep(expbuf, separator, sl)))
		return -1;
/*
** quote before the result
*/
	    if (qopen && (errno=appendchar_ep(expbuf, qopen)))
		return -1;
	    state=SLST_NEXTTOKEN;
	    /* fallthrough */
/*
** state: copy token, part 2: copy the actual token
*/
	  case SLST_NEXTTOKEN:
	    for (;; tok++)
		switch (*tok)
		{
/*
** have to read some more & then continue
*/
		  case '\0':
		    state=SLST_READ;
		    nextstate=SLST_NEXTTOKEN;
		    goto switchstate;
/*
** next line
*/
		  case '\n':
		    nextstate=SLST_NEWLINE;
		    goto endofthetoken;
/*
** skip to the end of the line
*/
/*		  case ' ':
		  case '\t':
*/		  case '#':
		    nextstate=SLST_SKIPLINE;
		    goto endofthetoken;
		  default:
		    if (errno=appendchar_ep(expbuf, *tok))
			return -1;
		}
endofthetoken:
	    loops++;
	    state=SLST_ENDTOKEN;
	    /* fallthrough */
	case SLST_ENDTOKEN:
/*
** state: copy token, part 3 place quote after token
*/
	    if (qclose && (errno=appendchar_ep(expbuf, qclose)))
		return -1;
	    state=nextstate;
switchstate:
	    break;
/*
** state: skip to end of line
*/
	  case SLST_SKIPLINE:
	    for (;; tok++)
		if (!*tok)
		{
		    state=SLST_READ;
		    nextstate=SLST_SKIPLINE;
		    break;
		}
		else if (*tok=='\n')
		{
		    state=SLST_NEWLINE;
		    tok++;
		    break;
		}
	    break;
/*
** state: all done
*/
	  case SLST_END:
	    return 0;
	  default:
	    return -1;
	}
    }
}

/*
** handles quotes and separator clauses in expansion
*/
static int fgw_expandlist(expbuf_t *expbuf, int loops, char *quotes,
			  char *separator, int sl, char *instr)
{
    char qopen, qclose='\0';
    int rc;

    if ((qopen=*quotes++) && !(qclose=*quotes))
	qclose=qopen;
/*
** if not first time round, then a comma is in order
*/
    if (expbuf->ep_len && loops && *separator &&
	(rc=appendstring_ep(expbuf, separator, sl)))
	return rc;
/*
** quote before the result
*/
    if (qopen && (rc=appendchar_ep(expbuf, qopen)))
	return rc;
/*
** next the result
*/
    if (rc=appendstring_ep(expbuf, instr, 0))
	return rc;
/*
** and a quote after
*/
    if (qclose && (rc=appendchar_ep(expbuf, qclose)))
	return rc;
    return 0;
}

/*
** include input from stream
*/
static int fgw_streaminclude(fgw_fdesc *sd, expbuf_t *expbuf)
{
    int r;
    int loops=0;
    char *i;

    for (;;)
    {
	if ((r=fgw_fdread(sd))<0)
	    return -1;
	else if (!r)
	{
	    if (loops)
		return 0;
	    else
	    {
		errno=RC_NOTFOUND;
		return -1;
	    }
	}
	else
	{
	    loops++;
	    if (errno=appendstring_ep(expbuf, sd->fd_buf, 0))
		return -1;
	}
    }
}
