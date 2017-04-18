/*
	CRQFC.c  -  SQSL interpreter output/format support functions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Jan 97
	Current release: Jan 16

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
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ccmnc.h"
#include "cascc.h"
#include "cfioc.h"
#include "coslc.h"
#include "ctypc.h"
#include "crqfc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crxuc.h"
#include "crqxp.h"

#define STARTENTRYSIZE	10

#define FLD_NEXT 0
#define FLD_NL 1
#define FLD_TAB 2
#define FLD_BLANK 3
#define FLD_STRING 4
#define FLD_pict 5
#define FLD_date 6
#define FLD_s 7
#define FLD_i 8
#define FLD_f 9
#define FLD_ds 10

#define UNKHLEN -1

static fgw_fmttype *rqf_fmtstart();
static char *rqf_fmtnew(fgw_fmttype **h, int s);
static int rqf_fmtadd(fgw_fmttype **h, char *e);

static void rqf_ffheaders(fgw_stmttype *st_p, fgw_loc_t *textvar);
static void rqf_vertical(fgw_stmttype *st_p, fgw_loc_t *textvar);
static void rqf_oneperline(fgw_stmttype *st_p, fgw_loc_t *textvar,
                           exprstack_t *e, int o);
static void rqf_fullformat(fgw_stmttype *st_p, fgw_loc_t *textvar, int finish);
static void rqf_brief(fgw_stmttype *st_p, fgw_loc_t *textvar);
static void rqf_delimited(fgw_stmttype *st_p, fgw_loc_t *textvar);
static void rqf_format(fgw_loc_t *t, exprstack_t *e, fgw_stmttype *st_p, int ro);
static int rqf_rightmargin(fgw_loc_t *t, int r, int rl, int ro);
static int rqf_leftmargin(fgw_loc_t *t, int r, int rl, int ro);
static int rqf_charlen(fgw_loc_t *t, int r);
static void rqf_addstring(fgw_loc_t *textvar, char *str, fgw_stmttype *st_p);
static void rqf_addstringdel(fgw_loc_t *textvar, char *str, fgw_stmttype *st_p);
static char *rqf_tostring(exprstack_t *e, char *b, int *n);
static void rqf_tobytestring(fgw_loc_t *t, exprstack_t *e);
static void rqf_padline(fgw_loc_t *textvar, int c);

static TGLOB int fieldcount=0;
static TGLOB fgw_fdesc *ofd=FD_TOBLOB;
static TGLOB int txtoffset=0;
static TGLOB int txtlines=0;

/*
** sets output stream
*/
void rqf_openfile(fgw_loc_t *txtvar, fgw_fdesc *fd)
{
    ofd=fd;
    if (ofd!=FD_TOBLOB && ofd!=FD_IGNORE)
    {
	txtoffset=txtvar->loc_size;
	txtlines=fgw_text(txtvar)->nolines;
    }
}

/*
** flushes output stream
*/
void rqf_flush(fgw_loc_t *txtvar, int all)
{
    int l;
 
    if (!(txtvar->loc_size-txtoffset) || ofd==FD_TOBLOB)
	return;
    if (ofd==FD_IGNORE)
    {
	txtlines=0;
	txtoffset=0;
	txtvar->loc_size=0;
	fgw_recompindex(txtvar, 0);
    }
    if (all || !txtvar->loc_size ||
	*(txtvar->loc_buffer+txtvar->loc_size-1)=='\n')
	l=txtvar->loc_size;
    else
	l=fgw_text(txtvar)->textindex[fgw_text(txtvar)->nolines-1];
    if (l-txtoffset)
    {
/*
** FIXME: formalize IO error handling. right now a display will fail
** a select might survive
*/
	fgw_fdwrite(ofd, txtvar->loc_buffer+txtoffset, l-txtoffset);
	fgw_move(txtvar->loc_buffer+txtoffset, txtvar->loc_buffer+l,
	    txtvar->loc_size-l);
	txtvar->loc_size-=(l-txtoffset);
	fgw_recompindex(txtvar, txtlines);
    }
}

/*
**  parse & set format specifiers
*/
void rqf_setformat(fgw_stmttype *st_p, char *f)
{
    char *p, *q, *s, *t, *e;
    int l, et;
    int explicit; /* true if all columns have an explicit field delimiter */
    int html;	  /* true if there's no explicit formatting */
    int field, fieldlen;
    int fieldpos, templen;
    int nulldel, partcnt;
    int haveheader, implicit;

/*
**  Overall goal is to have a UNKHLEN length for headers whose field size
**  cannot be readily recognised. DO_MOD to avoid problems (?) with %
*/
#define SET_HEADER_SIZE(arg1, arg2) \
	arg1=(arg1==UNKHLEN||arg2==UNKHLEN)? UNKHLEN: arg1+arg2
#define DO_MOD(arg1, arg2) (arg1==UNKHLEN)? 0: arg1 % arg2

    if (f==NULL ||
	st_p==NULL)
    {
	status=RC_INSID;
        return;
    }
    status=0;
    st_p->formats=rqf_fmtstart();
    s=f;
    field=0;
    fieldlen=0;
    fieldpos=0;
    implicit=0;
    explicit=1;
    nulldel=0;
    partcnt=0;
/*
**  there's a couple of places where we should set html and we don't
**  truth is, it either doesn't break the logic, or it isn't really necessary
*/
    html=1;
    haveheader=st_p->headers? st_p->headers->entries: 0;
    while (t=strpbrk(s, "|%\\\n\t["))
    {
/*
**  see if we have a string to save, first
*/
	if (l=t-s)
	    if (p=rqf_fmtnew(&st_p->formats, l+1))
	    {
		strncpy(p, s, l);
		*(p+l)=0;
		html=html && (strspn(p, " ")==l);
		st_p->formats->fmt[st_p->formats->entries-1].etype=FLD_STRING;
		SET_HEADER_SIZE(fieldlen, l);
	    }
	partcnt++;
	switch (*t)
	{
/*
**  backslash only applies to t & n, anything else we just skip it
**  it will not escape %, [, and |
*/
	  case '\\':
	    if (*(++t)=='n')
		goto TOKEN_n;
	    else if (*t!='t')
	    {
		s=t;
		break;
	    }
/*
**  a real TAB
*/
	  case '\t':
	    html=0;
	    if (!rqf_fmtadd(&st_p->formats, NULL))
	    {
		status=RC_NOMEM;
		return;
	    }
	    st_p->formats->fmt[st_p->formats->entries-1].etype=FLD_TAB;
	    l=DO_MOD((fieldlen+fieldpos), st_p->width);
	    SET_HEADER_SIZE(fieldlen, 8*(1+l/8));
	    s=++t;
	    break;
/*
**  a real NL
*/
	  case '\n':
TOKEN_n:
	    html=0;
	    if (!rqf_fmtadd(&st_p->formats, NULL))
	    {
		status=RC_NOMEM;
		return;
	    }
/*
**  for NL we just pretend the current line is filled with blanks
*/
	    st_p->formats->fmt[st_p->formats->entries-1].etype=FLD_NL;
	    l=DO_MOD((fieldlen+fieldpos), st_p->width);
	    SET_HEADER_SIZE(fieldlen, st_p->width-l);
	    s=++t;
	    break;
/*
**  a format specifier or field delimiter
*/
	  case '[':
	  case '%':
	  case '|':
/*
**  see if it is escaped
*/
	    if (*t==*(t+1))
	    {
		if (!(p=rqf_fmtnew(&st_p->formats, 2)))
		{
		    status=RC_NOMEM;
		    return;
		}
		st_p->formats->fmt[st_p->formats->entries-1].etype=FLD_STRING;
		*p++=*t;
		*p=0;
		s=t+2;
		break;
	    }
/*
**  set start, entry & length as if this was an invalid entry
*/
	    e=t;
	    l=1;
	    et=FLD_STRING;
	    s=t+1;
	    templen=1;
/*
**  a c format specifier
*/
	    if (*t=='%')
	    {
		if (p=strpbrk(t, "diouxXeEfgs"))
		{
/*
**  determine the type...
*/
		    switch (*p)
		    {
		      case 's':
			et=FLD_s;
			nulldel=1;
			break;
		      case 'e':
		      case 'E':
		      case 'f':
		      case 'g':
			et=FLD_f;
			break;
		      default:
			et=FLD_i;
		    }
/*
**  ...the length of the formatted string...
*/
		    e=t;
		    while(!isdigit(*e) && e<p)
			e++;
		    if (p-e)
		    {
			templen=atoi(e);
			nulldel=0;
		    }
		    else
			templen=UNKHLEN;
/*
**  ...and start, length, new scan ptr
*/
		    l=p-t+1;
		    e=t;
		    s=p+1;
		    implicit++;
		}
	    }
/*
**  a 4gl format specifier
*/
	    else if (*t=='[')
	    {
/*
**  find specifier terminator
*/
		q=t;
		do
		{
		   p=strpbrk(q, "]");
		   e=q+1;
		}
/*
**  skipping escaped ones
*/
		while (p && *(p+1)==*p);
		if (p)
		{
/*
**  found it, set stuff as above
*/
		    e=t+1;
		    l=p-t-1;
		    s=p+1;
		    et=FLD_pict;
		}
		templen=l;
		implicit++;
	    }
/*
**  format specifiers might act as implicit field separators
*/
	    if (*t=='|' || (et!=FLD_STRING && implicit>1))
	    {
		explicit=explicit && (*t=='|');
/*
** if explicit fields, then the implicit count starts from 0
** otherwise, we are about to put a field, so implicit is one already
*/
		implicit=(*t!='|');
		if (haveheader)
		{
		    st_p->headers->fmt[field].esize=fieldlen;
		    haveheader=st_p->headers->entries>field+1;
		}
/*
** set previous entry to allow empty non null string markers
** if string, no length specified, and no other formatting entry in field
*/
		if (partcnt==2 && nulldel)
		    st_p->formats->fmt[st_p->formats->entries-1].etype=FLD_ds;
/*
**  reset field info
*/
		partcnt=0;
		nulldel=0;
		if (fieldlen+fieldpos>st_p->width)
		    fieldpos=(fieldlen+fieldpos) % st_p->width;
		else if  (fieldlen!=UNKHLEN)
		    fieldpos=fieldlen;
		else
		    fieldpos=0;
		fieldlen=0;
		field++;
		if (!rqf_fmtadd(&st_p->formats, NULL))
		{
		    status=RC_NOMEM;
		    return;
		}
		st_p->formats->fmt[st_p->formats->entries-1].etype=FLD_NEXT;
	    }
/*
**  field separator, we're thru with this one
*/
	    if (*t=='|')
		continue;
/*
**  we have the format entry, go store it
*/
	    if (!(p=rqf_fmtnew(&st_p->formats, l+1)))
	    {
		status=RC_NOMEM;
		return;
	    }
	    strncpy(p, e, l);
	    *(p+l)=0;
/*
**  for formats with 0 length, we have a special entry
*/
	    if (!templen)
		et=FLD_BLANK;
#ifdef BROKEN_DATE_FORMATTING
/*
**  try to figure out whether this is a date format
*/
	    else if (et==FLD_pict)
		if (q=strpbrk(p, "dmy"))
		    if (*q==*++q)
			et=FLD_date;
#endif
	    st_p->formats->fmt[st_p->formats->entries-1].etype=et;
	    st_p->formats->fmt[st_p->formats->entries-1].esize=templen;
	    SET_HEADER_SIZE(fieldlen, templen);
	}
    }
/*
**  enter any remainder string
*/
    if (l=strlen(s))
	if (p=rqf_fmtnew(&st_p->formats, l+1))
	{
	    strcpy(p, s);
	    st_p->formats->fmt[st_p->formats->entries-1].etype=FLD_STRING;
	    SET_HEADER_SIZE(fieldlen, l);
	}
/*
**  field width is saved only to make sure that last header does not exceed
**  prescribed width
*/
    if (haveheader && fieldlen)
	st_p->headers->fmt[field].esize=fieldlen;
    if (explicit || html)
	st_p->options|=SO_TABULAR;
}

/*
**  sets next header
**  all the headers should be in place before calling rqf_setformat
*/
void rqf_setheader(fgw_stmttype *st_p, char *c)
{
    char *d;
    int l;

    if (st_p==NULL)
	return;
    if (!st_p->headers)
    {
	st_p->headers=rqf_fmtstart();
	st_p->headwidth=0;
    }
    l=strlen(c);
    if (!(d=rqf_fmtnew(&st_p->headers, l+1)))
	status=RC_NOMEM;
    else
    {
	strcpy(d, c); 
	if ((st_p->headers->fmt[st_p->headers->entries-1].etype=fgw_strlen(c))>
	    st_p->headwidth)
	    st_p->headwidth=st_p->headers->fmt[st_p->headers->entries-1].etype;
	st_p->headers->fmt[st_p->headers->entries-1].esize=UNKHLEN;
    }
}

/*
**  sets field count flags (HTML only)
*/
void rqf_countfields(fgw_stmttype *st_p, int countfields, int clear)
{
    if (st_p==NULL)
	return;
    st_p->countfields=countfields;
    if (clear)
	if (countfields && st_p->headers)
	    fieldcount=st_p->headers->entries;
	else
	    fieldcount=0;
}

/*
**  gets field count (HTML only)
*/
int rqf_fieldcount()
{
    return fieldcount;
}

/*
**  sets output width
*/
void rqf_setwidth(fgw_stmttype *st_p, int w)
{
    if (st_p==NULL)
	return;
    if (w<MINWIDTH)
	st_p->width=MINWIDTH;
    else if (w>MAXWIDTH)
	st_p->width=MAXWIDTH;
    else
	st_p->width=w;
}

/*
**  executes a statement
*/
void rqf_dorows(fgw_stmttype *st_p, fgw_loc_t *textvar, int finish)
{
    int display;

    display=st_p->fmt_type!=FMT_NULL;
    if (st_p==NULL)
    {
	status=RC_INSID;
	return;
    }
    else if (st_p->curstate<ST_DECLARED)
    {
	status=RC_INSID;
	return;
    }
    rqx_nextrow(st_p);
    if (int_flag || (status=st_p->ca->sqlcode))
    {
	finish=1;
	goto allrows_end;
    }
    if (display  &&st_p->pretable)
	    rqf_addstring(textvar, st_p->pretable, NULL);
	switch(st_p->fmt_type)
	{
	  case FMT_BRIEF:
	    do
		rqf_brief(st_p, textvar);
	    while (finish && !(status || int_flag ||
			       (rqx_nextrow(st_p), status=st_p->ca->sqlcode)));
	    break;
	  case FMT_FULL:
	    rqf_ffheaders(st_p, textvar);
	    rqf_fullformat(st_p, textvar, finish);
	    break;
	  case FMT_VERTICAL:
	    do
	        rqf_vertical(st_p, textvar);
	    while (finish && !(status || int_flag ||
			       (rqx_nextrow(st_p), status=st_p->ca->sqlcode)));
	    break;
	  case FMT_DELIMITED:
	    do
		rqf_delimited(st_p, textvar);
	    while (finish && !(status || int_flag ||
			       (rqx_nextrow(st_p), status=st_p->ca->sqlcode)));
	    break;
	  case FMT_NULL:
	    while (finish && !(status || int_flag ||
			       (rqx_nextrow(st_p), status=st_p->ca->sqlcode)));
	    break;
	}
    if (display && st_p->posttable)
	rqf_addstring(textvar, st_p->posttable, NULL);
allrows_end:
    if (st_p->field>fieldcount && st_p->countfields)
	fieldcount=st_p->field;
}

/*
**  expression display
*/
void rqf_display(fgw_stmttype *st_p, fgw_loc_t *textvar, exprstack_t *e,
		 int c)
{
    char *f, b[MAXNUMLEN];
    int r;

    if (st_p==NULL)
	return;
    if (st_p->pretable)
	rqf_addstring(textvar, st_p->pretable, NULL);
    if (st_p->fmt_type==FMT_FULL)
	rqf_ffheaders(st_p, textvar);
    if (st_p->prerow)
	rqf_addstring(textvar, st_p->prerow, NULL);
    st_p->field=0;
    st_p->format=0;
    switch(st_p->fmt_type)
    {
      case FMT_BRIEF:
	for (; c; e++, c--)
	{
	    f=rqf_tostring(e, (char *) &b, NULL);
	    if (((st_p->field? rqf_charlen(textvar,
					   fgw_text(textvar)->nolines)+1: 0)+
		 fgw_strlen(f))<=st_p->width)
	    {
		if (st_p->field)
		    rqf_addstring(textvar, ",", NULL);
		rqf_addstring(textvar, f, NULL);
	    }
	    st_p->field++;
	}
	break;
      case FMT_FULL:
	for (; c; e++, c--)
	{
	    if (st_p->prefield)
		rqf_addstring(textvar, st_p->prefield, NULL);
	    rqf_format(textvar, e, st_p, 0);
	    if (st_p->postfield)
		rqf_addstring(textvar, st_p->postfield, NULL);
	    st_p->field++;
	}
	break;
      case FMT_VERTICAL:
	for (; c; e++, c--)
	{
	    if (st_p->prefield)
		rqf_oneperline(st_p, textvar, e, 0);
	    else
		rqf_oneperline(st_p, textvar, e, st_p->headwidth);
	}
	break;
      case FMT_DELIMITED:
	for (; c; e++, c--)
	{
	    rqf_format(textvar, e, st_p, -1);
	    rqf_addstring(textvar, st_p->del, NULL);
	    st_p->field++;
	}
	break;
    }
    if (st_p->postrow)
    {
	rqf_addstring(textvar, st_p->postrow, NULL);
	if (st_p->fmt_type==FMT_VERTICAL)
	    rqf_addstring(textvar, "\n", NULL);
    }
    else
	rqf_addstring(textvar, "\n", NULL);
    if (st_p->posttable)
	rqf_addstring(textvar, st_p->posttable, NULL);
    if (st_p->field>fieldcount && st_p->countfields)
	fieldcount=st_p->field;
    rqf_flush(textvar, 0);
}

/*
**  outputs full format headers
*/
static void rqf_ffheaders(fgw_stmttype *st_p, fgw_loc_t *textvar)
{
    char *f, p[20], *q;
    int c, i, r;

/*
**  first row is headers, if HTML, not too much of a problem
*/
    if (st_p->headers)
    {
	if (st_p->prerow)
	    rqf_addstring(textvar, st_p->prerow, NULL);
	if (st_p->preheader)
	    for (c=0; c<st_p->headers->entries; c++)
	    {
		if (st_p->headers->fmt[c].esize)
		{
		    rqf_addstring(textvar, st_p->preheader, NULL);
		    rqf_addstring(textvar, st_p->headers->fmt[c].entry, NULL);
		    rqf_addstring(textvar, st_p->postheader, NULL);
		}
	    }
	else
/*
**  plain text, here
*/
	    for (c=0; c<st_p->headers->entries; c++)
	    {
		r=fgw_text(textvar)->nolines;
/*
**  we have some kind of format - in which case, the length of the field
**  has already been worked out in rqf_setformat. will enter the header
**  padded to that length
*/
		if (st_p->headers->fmt[c].esize==UNKHLEN)
		{
/*
** no format for this header - just stick it in
*/
		    rqf_addstring(textvar, st_p->headers->fmt[c].entry, NULL);
		    rqf_addstring(textvar, " ", NULL);
		}
		else if (st_p->headers->fmt[c].esize)
		{
/*
**  make sure that the header does not exceed the calculated width
*/
		    if (q=(char *) malloc(st_p->headers->fmt[c].esize+1))
		    {
			sprintf((char *) &p, "%%-%i.%is",
				st_p->headers->fmt[c].esize,
				st_p->headers->fmt[c].esize);
			sprintf(q, p, st_p->headers->fmt[c].entry);
			rqf_addstring(textvar, q, NULL);
			free(q);
		    }
		}
/*
**  and now, make sure we do not exceed the maximum width
**  rqf_rightmargin will also get rid of extra spaces we have created
**  to take into account NL's. Not entirely efficient, but this is only
**  done once, so who cares
*/
		while ((i=rqf_rightmargin(textvar, r++, st_p->width, 0))>0);
		if (i)
		    return;
	    }
	if (st_p->postrow)
	    rqf_addstring(textvar, st_p->postrow, NULL);
	else
	    rqf_addstring(textvar, "\n", NULL);
    }
}

/*
**  vertical format
*/
static void rqf_vertical(fgw_stmttype *st_p, fgw_loc_t *textvar)
{
    exprstack_t *e;
    int o;
    int doable;

    if (st_p->prefield)
	o=0;
    else
	o=st_p->headwidth;
    st_p->field=0;
    st_p->format=0;
    if (st_p->prerow)
	rqf_addstring(textvar, st_p->prerow, NULL);
    while ((!status) && (e=rqx_nexttoken(st_p->field, st_p)))
	rqf_oneperline(st_p, textvar, e, o);
    if (st_p->postrow)
	rqf_addstring(textvar, st_p->postrow, NULL);
    rqf_addstring(textvar, "\n", NULL);
    rqf_flush(textvar, 0);
}

/*
**  one field per line field output
*/
static void rqf_oneperline(fgw_stmttype *st_p, fgw_loc_t *textvar,
			   exprstack_t *e, int o)
{
    int s;

/*
** headers first, if we have them
*/
    s=o+1;
    if (st_p->preheader)
	rqf_addstring(textvar, st_p->preheader, NULL);
    if (st_p->headers && st_p->headers->entries>st_p->field)
    {
	 rqf_addstring(textvar, st_p->headers->fmt[st_p->field].entry, NULL);
/*
**  work out how much padding we might need
*/
	 s-=st_p->headers->fmt[st_p->field].etype;
    }
    if (st_p->postheader)
	rqf_addstring(textvar, st_p->postheader, NULL);
    else if (s>0)
	rqf_padline(textvar, s);
/*
**  and then the actual field
*/
    if (st_p->prefield)
	rqf_addstring(textvar, st_p->prefield, NULL);
    rqf_format(textvar, e, st_p, o);
    if (st_p->postfield)
	rqf_addstring(textvar, st_p->postfield, NULL);
    else
	rqf_addstring(textvar, "\n", NULL);
    st_p->field++;
}

/* FIXME: plain full mode with headers but missing field format might want to
   trim/extend field to header width? (not last header) */
/*
**  full format mode
*/
static void rqf_fullformat(fgw_stmttype *st_p, fgw_loc_t *textvar, int finish)
{
    exprstack_t *e;
    char *p, *q;
    int c, i, r;

    st_p->format=0;
/*
**  output next format
*/
    do
    {
	st_p->field=0;
	if (!st_p->formats && st_p->prerow && !st_p->format)
	    rqf_addstring(textvar, st_p->prerow, NULL);
/*
**  format all fields
*/
	while ((!status) && (e=rqx_nexttoken(st_p->field, st_p)))
	{
	    st_p->field++;
	    if (st_p->prefield)
		rqf_addstring(textvar, st_p->prefield, NULL);
	    rqf_format(textvar, e, st_p, 0);
	    if (st_p->postfield)
		rqf_addstring(textvar, st_p->postfield, NULL);
	}
/*
**  are we through with this format?
*/
	if (!st_p->formats || st_p->format>=st_p->formats->entries)
	{
	    if (st_p->postrow)
		rqf_addstring(textvar, st_p->postrow, NULL);
	    else
		rqf_addstring(textvar, "\n", NULL);
	    rqf_flush(textvar, 0);
	    if (finish)
		st_p->format=0;
	    else
		return;
	}
    }
    while (!(status || int_flag ||
	     (rqx_nextrow(st_p), status=st_p->ca->sqlcode)));
}

/*
**  brief format
*/
static void rqf_brief(fgw_stmttype *st_p, fgw_loc_t *textvar)
{
    exprstack_t *e;
    char *f, b[MAXNUMLEN];
    int l, ll=0, ml=st_p->width;

    st_p->field=0;
    while ((!status) && (e=rqx_nexttoken(st_p->field++, st_p)))
    {
	f=rqf_tostring(e, (char *) &b, NULL);
	l=fgw_strlen(f);
	if (ll+l>ml)
	   break;
	if (ml!=st_p->width)
	    rqf_addstring(textvar, ",", NULL);
	rqf_addstring(textvar, f, NULL);
	ml=st_p->width-1;
	ll+=l;
    }
    rqf_addstring(textvar, "\n", NULL);
    rqf_flush(textvar, 0);
}

/*
**  delimited format
*/
static void rqf_delimited(fgw_stmttype *st_p, fgw_loc_t *textvar)
{
    exprstack_t *e;
    int r;

    st_p->field=0;
    while ((!status) && (e=rqx_nexttoken(st_p->field++, st_p)))
    {
	rqf_format(textvar, e, st_p, -1);
	rqf_addstring(textvar, st_p->del, NULL);
    }
    rqf_addstring(textvar, "\n", NULL);
    rqf_flush(textvar, 0);
}

/*
**  adds non null empty string representation
*/
static void emptystring_sql(fgw_loc_t *t, exprstack_t *e, fgw_stmttype *st_p)
{
    if (st_p->del[0] && (e->type==CSTRINGTYPE) &&
	!rxu_isnull(e) && !strlen(e->val.string))
    {
	rqf_addstring(t, st_p->esc, NULL);
	rqf_addstring(t, " ", NULL);
    }
}

/*
**  format field
*/
static void rqf_format(fgw_loc_t *t, exprstack_t *e, fgw_stmttype *st_p, int ro)
{
    int tok, nl, i, n, r, cnv;
    double d;
    char fld[MAXNUMLEN];
    char *p;
    fgw_fmtentry *fmt;
    void (*addstring)()=st_p->del[0]? rqf_addstringdel: rqf_addstring;

    nl=0;
    do
    {
	r=fgw_text(t)->nolines;
/*
** no format entry, here
*/
	if (!st_p->formats || st_p->format>=st_p->formats->entries)
	{
	    if (st_p->del[0])
	    {
		if (e->type==FGWBYTETYPE)
		    rqf_tobytestring(t, e);
		else
		    emptystring_sql(t, e, st_p);
	    }
/*
** if not first field & fully formatted, leave a space here
*/
	    else if (st_p->format && st_p->fmt_type==FMT_FULL)
		(*addstring)(t, " ", st_p);
	    (*addstring)(t, rqf_tostring(e, (char *) &fld, NULL), st_p);
	    tok=FLD_NEXT;
	}
/*
** if valid format entry
*/
	else if ((tok=(fmt=&st_p->formats->fmt[st_p->format])->etype)!=FLD_NEXT)
	{
/*
** if start of line & offset, pad with blanks here
*/
	    if (nl && ro>0)
	    {
		nl--;
		rqf_padline(t, ro);
		r++;
	    }
	    fld[0]='\0';		/* in case of formatting errors */
/*
** got a format entry
*/
	    switch (tok)
	    {
	      case FLD_BLANK:
		break;
	      case FLD_NL:
/*
**  drop needless blanks other formats might have left
*/
		p=t->loc_buffer+t->loc_size-1;
		while (t->loc_size && *p--==' ')
		    t->loc_size--;
		(*addstring)(t, "\n", st_p);
		nl++;
		break;
	      case FLD_STRING:
		(*addstring)(t, fmt->entry, st_p);
		break;
	      case FLD_TAB:
		if (*(t->loc_buffer+t->loc_size-1)=='\n')
		    i=0;
		else
		    i=rqf_charlen(t, r);
		rqf_padline(t, 8*(1+i/8)-i);
		break;
	      case FLD_pict:
/*
**  we expect the picture to match the type.
**  for strings we make a bit more effort
*/
		if (e->type==CSTRINGTYPE)
		{
		    char *q;

		    cnv=CDOUBLETYPE;
		    if ((q=strpbrk(fmt->entry, "dmy")) && *q==*++q)
			cnv=CDATETYPE;
		    else if ((q=strpbrk(fmt->entry, "%")))
		    {
			if (strchr("bBdmyY", *(q+1)))
			    cnv=CDTIMETYPE;
			else if (strchr("HMSF", *(q+1)))
			    cnv=CINVTYPE;
		    }
		}
		else
		    cnv=0;
		p=rxu_topictstring(e, fld, sizeof(fld), fmt->entry, cnv, 1);
		if (!status)
		    (*addstring)(t, p, st_p);
		break;
#ifdef BROKEN_DATE_FORMATTING
	      case FLD_date:
		p=rxu_topictstring(e, fld, sizeof(fld), fmt->entry, CDATETYPE, 1)
		else if (!status)
		    (*addstring)(t, fld, st_p);
		break;
#endif
	      case FLD_s:
	      case FLD_ds:
		if (rxu_isnull(e))
		    i=0;
		else switch (e->type)
		{
		  case CSTRINGTYPE:
		    i=strlen(e->val.string);
		    break;
		  default:
		    i=MAXNUMLEN;
		}
		if (i<fmt->esize)
		    i=fmt->esize;
		if (i<MAXNUMLEN)
		    i=MAXNUMLEN;
/*
** strings & plain format we can do straight
*/
		if (addstring==rqf_addstring)
		{
		    if (fgw_resizebuffer(t, i))
		    {
			status=RC_NOMEM;
			return;
		    }
		    sprintf(t->loc_buffer+t->loc_size, fmt->entry,
			rqf_tostring(e, (char *) &fld, NULL));
		    t->loc_size+=strlen(t->loc_buffer+t->loc_size);
		    if (fgw_text(t)->nolines)
			i=fgw_text(t)->nolines-1;
		    else
			i=0;
		    fgw_recompindex(t, i);
		}
/*
** strings & all other formats we have to do it in two steps
*/
		else
		{
		    char *b, *s;

		    if (!(b=(char *) malloc(i+1)))
		    {
			status=RC_NOMEM;
			return;
		    }
		    sprintf(b, fmt->entry, rqf_tostring(e, (char *) &fld, NULL));
		    if (strlen(b))
			(*addstring)(t, b, st_p);
		    else if (tok==FLD_ds)
			emptystring_sql(t, e, st_p);
		    free(b);
		}
		break;
	      case FLD_i:
		p=rxu_tofmtstring(e, fld, sizeof(fld), fmt->entry, CINTTYPE, 1);
		if (!status)
                    (*addstring)(t, p, st_p);
		break;
	      case FLD_f:
		p=rxu_tofmtstring(e, fld, sizeof(fld), fmt->entry, CDOUBLETYPE, 1);
		if (!status)
                    (*addstring)(t, p, st_p);
		break;
	    }
	}
	if (status)		/* catch ENOMEMs */
	    return;
	if (ro>=0)
	{
/*
**  and now, make sure we do not exceed the maximum width
*/
	    while ((i=rqf_rightmargin(t, r++, st_p->width, ro))>0);
	    if (i)
		return;
/*
**  and finally, if we had multiple lines (maybe a text blob?)
**  make sure that they are left margined decently
*/
	    while ((i=rqf_leftmargin(t, r++, st_p->width, ro))>0);
	    if (i)
		return;
	}
	nl=*(t->loc_buffer+t->loc_size-1)=='\n';
	st_p->format++;
	fmt++;
    }
    while (tok!=FLD_NEXT && st_p->formats->entries>st_p->format);
}

/*
**  right margin specific line in blob
*/
static int rqf_rightmargin(fgw_loc_t *t, int r, int rl, int ro)
{
    int i, j, l;
    char *pd, *ps;

    if (r>fgw_text(t)->nolines || !fgw_text(t)->nolines)
	return 0;
    if ((l=rqf_charlen(t, r))>rl)
    {
/*
**  see if we have to do clipping
*/
	pd=t->loc_buffer+fgw_text(t)->textindex[r-1]+rl;
	ps=pd;
	i=1;
	j=rl;
	while (j-- && *--pd==' ')
	    i--;
	pd++;
/*
**  resizing might be necessary
*/
	if (fgw_resizebuffer(t, i+ro))
	{
	    status=RC_NOMEM;
	    return -1;
	}
/*
**  make room for NL & padding
*/
	fgw_move(pd+ro+1, ps, t->loc_size-fgw_text(t)->textindex[r-1]-rl);
/*
**  adj blob size
*/
	t->loc_size+=i+ro;
/*
**  add NL & padding
*/
	*pd++='\n';
	i=ro;
	while (i--)
	    *pd++=' ';
/*
** finally recompute line index
*/
	fgw_recompindex(t, r-1);
	return 1;
    }
    return 0;
}

/*
**  left & optionally right margin specific line in blob
*/
static int rqf_leftmargin(fgw_loc_t *t, int r, int rl, int ro)
{
    int i, l, nl, lo;
    char *pd, *ps;

    if (r>fgw_text(t)->nolines || !fgw_text(t)->nolines)
	return 0;
    l=rqf_charlen(t, r);
    if (l+ro>rl)
    {
	nl=rl;
	i=1;
	lo=l+ro-rl;
    }
    else
    {
	nl=l+ro;
	i=0;
	lo=0;
    }
    ps=t->loc_buffer+fgw_text(t)->textindex[r-1]+nl-ro;
/*
**  either margin or split to do
*/
    if (i || ro)
    {
/*
**  see if we have to do clipping
*/
	while (ps>t->loc_buffer && *--ps==' ')
	    i--;
	ps++;
	pd=ps+i+ro;
/*
**  resizing might be necessary
*/
	if (fgw_resizebuffer(t, i+ro))
	{
	    status=RC_NOMEM;
	    return -1;
	}
/*
**  make room for NL & padding
*/
	fgw_move(pd, ps, t->loc_size-fgw_text(t)->textindex[r]+lo);
/*
**  adj blob size
*/
	t->loc_size+=i+ro;
/*
**  add a NL
*/
	*--pd='\n';
/*
**  make room for padding;
*/
	pd=t->loc_buffer+fgw_text(t)->textindex[r-1]+ro;
	fgw_move(pd, pd-ro, nl-ro);
/*
**  pad
*/
	i=ro;
	while (i--)
	    *--pd=' ';
/*
** finally recompute line index
*/
	fgw_recompindex(t, r-1);
    }
    return 1;
}

/*
**  returns the length of the last line in the blob
**  FIXME needs to be GLS aware
*/
static int rqf_charlen(fgw_loc_t *t, int r)
{
    int s;

    if (!r)
	return 0;
    s=fgw_text(t)->textindex[r]-fgw_text(t)->textindex[r-1];
    if (r<fgw_text(t)->nolines || *(t->loc_buffer+t->loc_size-1)=='\n')
	s--;
    return s;
}

/*
** add a string to text variable
*/
static void rqf_addstring(fgw_loc_t *textvar, char *str, fgw_stmttype *st_p)
{
    int l, s;

    if (!str)
	return;
    s=strlen(str);
    if (!s)
	return;
    if (fgw_resizebuffer(textvar, s))
    {
	status=RC_NOMEM;
	return;
    }
    fgw_move(textvar->loc_buffer+textvar->loc_size, str, s);
    textvar->loc_size+=s;
    if (fgw_text(textvar)->nolines)
	l=fgw_text(textvar)->nolines-1;
    else
	l=0;
    fgw_recompindex(textvar, l);
}

/*
** add a string to text variable
*/
static void rqf_addstringdel(fgw_loc_t *textvar, char *str, fgw_stmttype *st_p)
{
    int l, s;
    char *d;

    if (!str)
	return;
    s=strlen(str);
    if (!s)
	return;
    if (fgw_resizebuffer(textvar, 2*s))
    {
	status=RC_NOMEM;
	return;
    }
    for (d=textvar->loc_buffer+textvar->loc_size, l=s; l; l--)
    {
        if (*str=='\n' || *str==st_p->del[0])
	{
	    *d++=st_p->esc[0];
	    textvar->loc_size++;
	}
	*d++=*str++;
	textvar->loc_size++;
    }
    if (fgw_text(textvar)->nolines)
	l=fgw_text(textvar)->nolines-1;
    else
	l=0;
    fgw_recompindex(textvar, l);
}

/*
**  converts expr to string taking care of byte columns
**  for display purposes
*/
static char *rqf_tostring(exprstack_t *e, char *b, int *n)
{
    if (e->type==FGWBYTETYPE)
    {
	if (n)
	    *n=rxu_isnull(e);
	return "<BYTE value>";
    }
    else
	return rxu_tostring(e, b, n);
}

/*
**  converts byte to hex string sequence
*/
static void rqf_tobytestring(fgw_loc_t *t, exprstack_t *e)
{
    int l, n;
    char *d, *s;

    if (e->length<=0)
	return;
    if (fgw_resizebuffer(t, 2*e->length))
    {
	status=RC_NOMEM;
	return;
    }
    for (d=t->loc_buffer+t->loc_size, 
	 s=e->val.string, l=e->length; l; l--, s++)
    {
	n=*s >> 4;
	*d++=n>9? 'A'-10+n: '0'+n;
	n=*s & 0xF;
	*d++=n>9? 'A'-10+n: '0'+n;
	t->loc_size+=2;
    }
}

/*
**  adds spaces to last line in blob
*/
static void rqf_padline(fgw_loc_t *textvar, int c)
{
    int i, l;
    char *p;

    if (c<1)
	return;
    if (fgw_resizebuffer(textvar, c))
    {
	status=RC_NOMEM;
	return;
    }
    p=textvar->loc_buffer+textvar->loc_size;
    for (i=0; i<c; i++)
	*p++=' ';
    textvar->loc_size+=c;
    if (fgw_text(textvar)->nolines)
	l=fgw_text(textvar)->nolines-1;
    else
	l=0;
    fgw_recompindex(textvar, l);
}

/*
**  allocates a fmt structure
*/
static fgw_fmttype *rqf_fmtstart()
{
    fgw_fmttype *h;

    h=(fgw_fmttype *) malloc(sizeof(fgw_fmttype)+
			  (STARTENTRYSIZE-1)*sizeof(fgw_fmtentry));
    h->entries=0;
    h->size=STARTENTRYSIZE;
    return(h);
}

/*
**  adds an entry to a fmt structure
*/
static int rqf_fmtadd(fgw_fmttype **h, char *e)
{
    fgw_fmttype *s;
    fgw_fmtentry *el;
    int c;

    if ((*h)->size==(*h)->entries)
    {
	s=*h;
	(*h)->size+=STARTENTRYSIZE;
	if (!(*h=(fgw_fmttype *) realloc(*h, sizeof(fgw_fmttype)+
			((*h)->size-1)*sizeof(fgw_fmtentry))))
	{
/*
** not enough space to realloc - we have lost whatever entry we had in
** the fgw_fmt struct, thus free everything there to avoid a memory leak
*/
	    for (c=0; c<s->entries; c++)
		free(s->fmt[c].entry);
	    return(0);
	}
    }
    el=(fgw_fmtentry *) &(*h)->fmt+(*h)->entries++;
    el->entry=e;
    el->etype=0;
    el->esize=0;
    return(1);
}

/*
**  allocates & adds an entry to a fmt structure
*/
static char *rqf_fmtnew(fgw_fmttype **h, int s)
{
    char *c;

    if (c=(char *) malloc(s))
	if (!rqf_fmtadd(h, c))
	{
	    free(c);
	    c=NULL;
	}
    return(c);
}

/*
**  deallocates a fmt structure
*/
void rqf_fmtclear(fgw_fmttype *h)
{
    int i;

    for (i=0; i<h->entries; i++)
	free(h->fmt[i].entry);
    free(h);
}
