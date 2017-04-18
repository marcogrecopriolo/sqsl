/*
	CASCC.c  -  ASCII text manipulation functions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Nov 98
	Current release: Jan 17

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

#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "coslc.h"
#include "ccmpc.h"
#include "ccmnc.h"
#include "cascc.h"
#include "ctypc.h"

#define FGW_LOC_USER_ENV	0x1000000

/*
**  test user env buffer
*/
static int fgw_testuserenv(fgw_loc_t *textvar)
{
/*
** Problem: 4gl does not use loc_user_env at all, which means that
** we get whatever trash was on the stack - invalid pointers, valid
** pointers to entirely unrelated data, pointers loc_user_env's related
** to other indexed blobs (and yes, I've seen them all).
** This causes all sorts of unextpected behaviours when freeing loc_user_envs
** of blobs we do not know the history of (asc_locate & asc_copy)
** In the past we have tried to ascertain the validity of loc_user_env
** by maintaining our homegrown _end and sbrk, but with newer versions
** of malloc using private areas for larger blocks, this no longer works.
** The current strategy is to use a private flag (ie one not used in the
** context of loc_t's) and set it both in loc_mflags and loc_oflags:
** the chances of both having been left mistakenly turned on is very slim.
** This is also open to the chance that said private flag gets used by IBM.
** Having a pointer to the original blob descriptor has also been found to work,
** but is by no means foolproof. A further test could be
**
** fgw_text(textvar)->textindex[fgw_text(textvar)->nolines]==textvar->loc_size
*/
    return (textvar->loc_user_env!=NULL &&
	    (textvar->loc_mflags & FGW_LOC_USER_ENV) &&
	    (textvar->loc_oflags & FGW_LOC_USER_ENV) &&
	    fgw_text(textvar)->key==TEXTKEY &&
	    fgw_text(textvar)->signature==SIGNATURE(textvar));
}

/*
** test if textvar is a usable blob
*/
int fgw_testtext(fgw_loc_t *textvar)
{
    return ((textvar->loc_loctype!=LOCMEMORY) ||
	    (textvar->loc_type!=SQLTEXT) ||
	    (!fgw_testuserenv(textvar)));
}

/*
**  index resize
*/
int fgw_resizeindex(fgw_loc_t *textvar, int idx)
{
    char *p;

    if (idx>=fgw_text(textvar)->size)
	if (p=(char *) realloc(textvar->loc_user_env,
	    sizeof(fgw_texttype)+
	    sizeof(int)*(fgw_text(textvar)->size+STARTLINESIZE)))
	{
	    textvar->loc_mflags|=FGW_LOC_USER_ENV;
	    textvar->loc_oflags|=FGW_LOC_USER_ENV;
	    textvar->loc_user_env=p;
	    fgw_text(textvar)->size+=STARTLINESIZE;
	}
	else
	    return 1;
    return 0;
}

/*
**  recompute the line index
*/
void fgw_recompindex(fgw_loc_t *textvar, int idx)
{
    int c;

/*
**  while we are at it, set the indicator
*/
    if (textvar->loc_size)
	textvar->loc_indicator=0;
    else
	textvar->loc_indicator=-1;
/*
**  scan the text from the line # in the parm
*/
    for (c=fgw_text(textvar)->textindex[idx]; c<textvar->loc_size; c++)
    {
	if (*(textvar->loc_buffer+c)=='\n')
/*
**  resize line index if necessary
*/
	    if (fgw_resizeindex(textvar, idx))
/*
**  FIXME: not handling out of mem!
*/
		return;
	    else
		fgw_text(textvar)->textindex[++idx]=c+1;
    }
/*
**  just in case the text var does not terminate with a '\n'
*/
    if (textvar->loc_size && *(textvar->loc_buffer+c-1)!='\n' &&
	!fgw_resizeindex(textvar, idx))
	fgw_text(textvar)->textindex[++idx]=c;
    fgw_text(textvar)->nolines=idx;
}

/*
**  resize the blob buffer
*/
int fgw_resizebuffer(fgw_loc_t *textvar, int sz)
{
    char *p;

    if (textvar->loc_bufsize>textvar->loc_size+sz)
	return 0;
    if (p=(char *) realloc(textvar->loc_buffer,
			   textvar->loc_bufsize+STARTTEXTSIZE))
    {
	textvar->loc_bufsize=textvar->loc_bufsize+STARTTEXTSIZE;
	textvar->loc_buffer=p;
	fgw_text(textvar)->signature=SIGNATURE(textvar);
	return 0;
    }
    return 1;
}

/*
**  free an old blob
*/
void fgw_freetext(fgw_loc_t *textvar)
{
/*
**  try to avoid memory leaks & segv's :-)
*/
    if (textvar->loc_loctype==LOCMEMORY &&
	textvar->loc_bufsize>0 &&
	textvar->loc_buffer)
	free(textvar->loc_buffer);
    if (fgw_testuserenv(textvar))
	free(textvar->loc_user_env);
/*
** to avoid problems, leave descriptor in an impeccable state
*/
    textvar->loc_buffer=NULL;
    textvar->loc_user_env=NULL;		/* avoid freeing memory twice! */
    textvar->loc_bufsize=0;
    textvar->loc_size=0;
    textvar->loc_indicator=-1;
    textvar->loc_mflags&=~FGW_LOC_USER_ENV;
    textvar->loc_oflags&=~FGW_LOC_USER_ENV;
}

/*
** locate a text variable
*/
void fgw_locatetext(fgw_loc_t *textvar)
{
    fgw_freetext(textvar);
    textvar->loc_buffer=(char *) malloc(STARTTEXTSIZE);
    if (!textvar->loc_buffer)
	status=FGW_ENOMEM;
    textvar->loc_user_env=(char *) malloc(sizeof(fgw_texttype)+
					  (STARTLINESIZE)*sizeof(int));
    if (!textvar->loc_user_env)
    {
	free(textvar->loc_buffer);
	status=FGW_ENOMEM;
    }
    textvar->loc_type=SQLTEXT;
    textvar->loc_loctype=LOCMEMORY;
    textvar->loc_bufsize=STARTTEXTSIZE;
    textvar->loc_size=0;
    textvar->loc_indicator=-1;
    textvar->loc_mflags|=FGW_LOC_USER_ENV;
    textvar->loc_oflags|=FGW_LOC_USER_ENV;
    fgw_text(textvar)->key=TEXTKEY;
    fgw_text(textvar)->signature=SIGNATURE(textvar);
    fgw_text(textvar)->size=STARTLINESIZE;
    fgw_text(textvar)->nolines=0;
    fgw_text(textvar)->textindex[0]=0;
}

#if defined(A4GL) || defined(I4GLVER)
/*
** locate a text variable
*/
int FGLGLOB(asc_locate)(nargs)
int nargs;
{
    LOC_T(textvar)

    POPLOCATOR(textvar)
    fgw_locatetext(textvar);
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** free a text variable (*never* use 4gl FREE on a line indexed blob!)
*/
int FGLGLOB(asc_free)(nargs)
int nargs;
{
    LOC_T(textvar)

    POPLOCATOR(textvar)
    if (fgw_testtext(textvar))
	status=FGW_EINVAL;
    else
    {
	fgw_freetext(textvar);
	RESTORELOCATOR(textvar)
    }
    return 0;
}

/*
** duplicate a text variable
*/
int FGLGLOB(asc_copy)(nargs)
int nargs;
{
    LOC_T(textvar1)
    LOC_T(textvar2)

    POPLOCATOR(textvar2)
    POPLOCATOR(textvar1)
    if (fgw_testtext(textvar1))
    {
	status=FGW_EINVAL;
	return 0;
    }
    fgw_freetext(textvar2);
/*
**  just in case we bomb out
*/
    textvar2->loc_type=SQLTEXT;
    textvar2->loc_loctype=LOCMEMORY;
    textvar2->loc_bufsize=0;
    textvar2->loc_size=0;
    textvar2->loc_indicator=-1;
    textvar2->loc_buffer=(char *) malloc(textvar1->loc_bufsize);
    if (!textvar2->loc_buffer)
    {
	status=FGW_ENOMEM;
	return 0;
    }
    textvar2->loc_user_env=(char *) malloc(sizeof(fgw_texttype)+
					   sizeof(int)*fgw_text(textvar1)->size);
    if (!textvar2->loc_user_env)
    {
	free(textvar2->loc_buffer);
	status=FGW_ENOMEM;
	return 0;
    }
    fgw_move(textvar2->loc_buffer, textvar1->loc_buffer, textvar1->loc_size);
    fgw_move(textvar2->loc_user_env, textvar1->loc_user_env,
	sizeof(fgw_texttype)+fgw_text(textvar1)->nolines*sizeof(int));
    fgw_text(textvar2)->signature=SIGNATURE(textvar2);
    textvar2->loc_bufsize=textvar1->loc_bufsize;
    textvar2->loc_size=textvar1->loc_size;
    textvar2->loc_indicator=textvar1->loc_indicator;
    textvar2->loc_mflags|=FGW_LOC_USER_ENV;
    textvar2->loc_oflags|=FGW_LOC_USER_ENV;
    RESTORELOCATOR(textvar2)
    return 0;
}

/*
**  add line tracking to existing blob
*/
int FGLGLOB(asc_initialize)(nargs)
int nargs;
{
    LOC_T(textvar)

    POPLOCATOR(textvar)
/*
**  this is what a text var in memory should look like
*/
    if (textvar->loc_type!=SQLTEXT ||
	textvar->loc_loctype!=LOCMEMORY ||
	textvar->loc_bufsize<=0 ||
	textvar->loc_size<=0 ||
	fgw_testuserenv(textvar) ||
	textvar->loc_indicator)
    {
	status=FGW_EINVAL;
	return 0;
    }
    textvar->loc_user_env=(char *) malloc(sizeof(fgw_texttype)+
					  STARTLINESIZE*sizeof(int));
    if (!textvar->loc_user_env)
    {
	status=FGW_ENOMEM;
	return 0;
    }
    textvar->loc_mflags|=FGW_LOC_USER_ENV;
    textvar->loc_oflags|=FGW_LOC_USER_ENV;
    fgw_text(textvar)->key=TEXTKEY;
    fgw_text(textvar)->signature=SIGNATURE(textvar);
    fgw_text(textvar)->size=STARTLINESIZE;
    fgw_text(textvar)->nolines=0;
    fgw_text(textvar)->textindex[0]=0;
    fgw_recompindex(textvar, 0);
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** add a string by lineno
*/
int FGLGLOB(asc_addline)(nargs)
int nargs;
{
    LOC_T(textvar)
    char *str;
    int l, r, s;

    popint(&l);
    str=fgw_popquote();
    POPLOCATOR(textvar)
    if (fgw_testtext(textvar) || str==NULL)
    {
	status=FGW_EINVAL;
	free(str);
	return 0;
    }
    s=strlen(str);
    *(str+s)='\n';
    s++;
    *(str+s)=0;
    if (fgw_resizebuffer(textvar, s))
    {
	status=FGW_ENOMEM;
	free(str);
	return 0;
    }
    l--;
    if (l>=0 && l<fgw_text(textvar)->nolines)
    {
	fgw_move(textvar->loc_buffer+fgw_text(textvar)->textindex[l]+s,
		 textvar->loc_buffer+fgw_text(textvar)->textindex[l],
		 fgw_text(textvar)->textindex[fgw_text(textvar)->nolines]-
		 fgw_text(textvar)->textindex[l]);
	r=l;
    }
    else
    {
/*
**  the last line might not be NL terminated, so it is a good idea
**  to recompute the index from the previous one
*/
	l=fgw_text(textvar)->nolines;
	r=l? l-1: 0;
    }
    fgw_move(textvar->loc_buffer+fgw_text(textvar)->textindex[l], str, s);
    textvar->loc_size+=s;
    fgw_recompindex(textvar, r);
    free(str);
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** change a substring by line
*/
int FGLGLOB(asc_changeline)(nargs)
int nargs;
{
    LOC_T(textvar)
    char *str;
    int l, s, e, sz;

    popint(&l);
    l--;
    str=fgw_popquote();
    POPLOCATOR(textvar)
    if (str==NULL ||
	fgw_testtext(textvar) || l<0 || l>=fgw_text(textvar)->nolines)
    {
	status=FGW_EINVAL;
	free(str);
	return 0;
    }
    sz=strlen(str);
    *(str+sz)='\n';
    sz++;
    *(str+sz)=0;
    s=fgw_text(textvar)->textindex[l];
    e=fgw_text(textvar)->textindex[l+1];
    if (fgw_resizebuffer(textvar, sz+s-e))
    {
	status=FGW_ENOMEM;
	free(str);
	return 0;
    }
    if (e<textvar->loc_size && e-s!=sz)
	fgw_move(textvar->loc_buffer+s+sz,
		 textvar->loc_buffer+e,
		 textvar->loc_size-e);
    fgw_move(textvar->loc_buffer+s, str, sz);
    textvar->loc_size+=sz+s-e;
    if (sz-e+s)
	fgw_recompindex(textvar, l);
    free(str);
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** drop a string by lineno
*/
int FGLGLOB(asc_dropline)(nargs)
int nargs;
{
    LOC_T(textvar)
    int l;

    popint(&l);
    POPLOCATOR(textvar)
    if (fgw_testtext(textvar)||
	l<1||l>fgw_text(textvar)->nolines)
    {
	status=FGW_EINVAL;
	return 0;
    }
    if (l==fgw_text(textvar)->nolines)
    {
	fgw_text(textvar)->nolines--;
	textvar->loc_size=fgw_text(textvar)->textindex[fgw_text(textvar)->nolines];
	return 0;
    }
    fgw_move(textvar->loc_buffer+fgw_text(textvar)->textindex[l-1],
	     textvar->loc_buffer+fgw_text(textvar)->textindex[l],
	     fgw_text(textvar)->textindex[fgw_text(textvar)->nolines]-
	     fgw_text(textvar)->textindex[l]);
    textvar->loc_size+=fgw_text(textvar)->textindex[l-1]-
		       fgw_text(textvar)->textindex[l];
    fgw_recompindex(textvar, l-1);
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** retrieve a string by lineno
*/
int FGLGLOB(asc_retrieveline)(nargs)
int nargs;
{
    LOC_T(textvar)
    int l, c, s;
    char d[MAXSTRINGSIZE+1];

    popint(&l);
    POPLOCATOR(textvar)
    if (fgw_testtext(textvar)||
	l<1||l>fgw_text(textvar)->nolines)
    {
	status=FGW_EINVAL;
	retquote("");
	return 1;
    }
    s=fgw_text(textvar)->textindex[l-1];
    c=fgw_text(textvar)->textindex[l]-s;
    strncpy(d, textvar->loc_buffer+s, c);
    if (d[--c]!='\n')			/* drop possible trailing cr */
	c++;
    d[c]=0;
    retquote(d);
    return 1;
}

/*
** return start of line
*/
int FGLGLOB(asc_linepos)(nargs)
int nargs;
{
    LOC_T(textvar)
    int l, r;

    SETNULLINT((char *) &r);
    popint(&l);
    POPLOCATOR(textvar)
    if (fgw_testtext(textvar)||l<1||l>fgw_text(textvar)->nolines)
	status=FGW_EINVAL;
    else
	r=fgw_text(textvar)->textindex[l-1]+1;
    retint(r);
    return 1;
}

/*
** return length of line
*/
int FGLGLOB(asc_linelength)(nargs)
int nargs;
{
    LOC_T(textvar)
    int l, s;

    SETNULLINT((char *) &s);
    popint(&l);
    POPLOCATOR(textvar)
    if (fgw_testtext(textvar)||l<1||l>fgw_text(textvar)->nolines)
	status=FGW_EINVAL;
    else
    {
	s=fgw_text(textvar)->textindex[l]-fgw_text(textvar)->textindex[l-1];
	if (l!=fgw_text(textvar)->nolines ||
	    *(textvar->loc_buffer+textvar->loc_size-1)=='\n')
	    s--;
    }
    retint(s);
    return 1;
}

/*
** add a substring by index
*/
int FGLGLOB(asc_addstring)(nargs)
int nargs;
{
    LOC_T(textvar)
    char *str;
    int st, sz;

    popint(&st);
    st--;
    str=fgw_popquote();
    POPLOCATOR(textvar)
    if (fgw_testtext(textvar) || str==NULL)
    {
	status=FGW_EINVAL;
	free(str);
	return 0;
    }
    sz=strlen(str);
    if (fgw_resizebuffer(textvar, sz))
    {
	status=FGW_ENOMEM;
	free(str);
	return 0;
    }
    if (st>=0 && st<textvar->loc_size)
	fgw_move(textvar->loc_buffer+st+sz,
		 textvar->loc_buffer+st,
		 textvar->loc_size-st);
    else
	st=textvar->loc_size;
    fgw_move(textvar->loc_buffer+st, str, sz);
    textvar->loc_size+=sz;
    fgw_recompindex(textvar, 0);
    free(str);
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** change a substring by index
*/
int FGLGLOB(asc_changestring)(nargs)
int nargs;
{
    LOC_T(textvar)
    char *str;
    int s, e, sz;

    popint(&e);
    popint(&s);
    s--;
    str=fgw_popquote();
    POPLOCATOR(textvar)
    if (str==NULL || fgw_testtext(textvar) || s<0 ||
	e-s>MAXSTRINGSIZE || s>e || e-1>=textvar->loc_size)
    {
	status=FGW_EINVAL;
	free(str);
	return 0;
    }
    sz=strlen(str);
    if (fgw_resizebuffer(textvar, sz+s-e))
    {
	status=FGW_ENOMEM;
	free(str);
	return 0;
    }
    if (e<textvar->loc_size && e-s!=sz)
	fgw_move(textvar->loc_buffer+s+sz,
		 textvar->loc_buffer+e,
		 textvar->loc_size-e);
    fgw_move(textvar->loc_buffer+s, str, sz);
    textvar->loc_size+=sz+s-e;
    if (sz-e+s)
	fgw_recompindex(textvar, 0);
    free(str);
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** drop a substring by index
*/
int FGLGLOB(asc_dropstring)(nargs)
int nargs;
{
    LOC_T(textvar)
    int s, e;

    popint(&e);
    popint(&s);
    POPLOCATOR(textvar)
    s--;
    if (fgw_testtext(textvar)||s<0||
	e-s>MAXSTRINGSIZE||s>e||e-1>=textvar->loc_size)
    {
	status=FGW_EINVAL;
	return 0;
    }
    if (e==textvar->loc_size)
    {
	textvar->loc_size=s;
	while (fgw_text(textvar)->textindex[fgw_text(textvar)->nolines]>s)
	    fgw_text(textvar)->nolines--;
	fgw_text(textvar)->textindex[++(fgw_text(textvar)->nolines)]=textvar->loc_size;
	return 0;
    }
    fgw_move(textvar->loc_buffer+s,
	     textvar->loc_buffer+e,
	     textvar->loc_size-e);
    textvar->loc_size+=s-e;
    fgw_recompindex(textvar, 0);
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** return a substring by index
*/
int FGLGLOB(asc_retrievestring)(nargs)
int nargs;
{
    LOC_T(textvar)
    int s, e, c;
    char d[MAXSTRINGSIZE+1];

    popint(&e);
    popint(&s);
    POPLOCATOR(textvar)
    c=e-s+1;
    s--;
    if (fgw_testtext(textvar)||s<0||
	c>MAXSTRINGSIZE||c<0||e-1>textvar->loc_size)
    {
	status=FGW_EINVAL;
	retquote("");
	return 1;
    }
    strncpy(d, textvar->loc_buffer+s, c);
    d[c]=0;
    retquote(d);
    return 1;
}

/*
** return length of buffer
*/
int FGLGLOB(asc_length)(nargs)
int nargs;
{
    LOC_T(textvar)
    int l;

    SETNULLINT((char *) &l);
    POPLOCATOR(textvar)
    if (fgw_testtext(textvar))
	status=FGW_EINVAL;
    else
	l=textvar->loc_size;
    retint(l);
    return 1;
}

/*
** return no of lines
*/
int FGLGLOB(asc_lines)(nargs)
int nargs;
{
    LOC_T(textvar)
    int l;

    rsetnull(CINTTYPE, (char *) &l);
    POPLOCATOR(textvar)
    if (fgw_testtext(textvar))
	status=FGW_EINVAL;
    else
	l=fgw_text(textvar)->nolines;
    retint(l);
    return 1;
}
#endif
