/*
	CLDDC.c  -  SQSL stream input functions, [regex] delimited pattern

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Jan 97
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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

/* FIXME: oversimplified
   - don't check that rows have the same number of fields (rely on the source
     to complain...)
*/

#define MAXDELSIZE 1024
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_REGEX
#include <regex.h>
#endif
#include "ccmnc.h"
#include "cfioc.h"
#include "csqrc.h"
#include "crqxp.h"

static void ldd_setconnection(char *host, char *name, ca_t *ca, char *private_area);
static void ldd_freestatement(fgw_stmttype *st, int clean);
static void ldd_prepare(fgw_stmttype *st, char *query);
static void ldd_nextrow(fgw_stmttype *st);
static exprstack_t *ldd_nexttoken(int field, fgw_stmttype *st_p);
static int ldd_counttokens(fgw_stmttype *st_p);

typedef struct fgw_stream
{
    exprstack_t *exprlist;	/* stack containing one row of data */
    int fields, max;
    int eof;
    int scanstart;
    char del, esc;
#ifdef HAVE_REGEX
    regex_t pattern;
#endif
} fgw_streamtype;

/*
** this source doesn't go in the source list, can't be used directly,
** so NULLs are OK in most places
*/
static fgw_sourcetype lddsrc = {
    { NULL },			/* some c don't like empty initializer
				   lists */
    { NULL }, 
    SO_SRCFLG,
    NULL,
    { 0 },			/* ditto */
    NULL,			/* there's only the default connection */
    NULL,			/* just to fool the dynamic loader code.... */
    NULL,
    ldd_setconnection,		/* this just to be on the safe side */
    NULL,
    NULL,
    ldd_freestatement,
    NULL,
    NULL,
    ldd_prepare,
    NULL,
    NULL,
    NULL,
    ldd_nextrow,
    ldd_nexttoken,
    ldd_counttokens,
    NULL,
    NULL
};

#ifdef HAVE_REGEX
static void ldr_freestatement(fgw_stmttype *st, int clean);
static void ldr_nextrow(fgw_stmttype *st);
static exprstack_t *ldr_nexttoken(int field, fgw_stmttype *st_p);

/*
** ditto
*/
static fgw_sourcetype ldrsrc = {
    { NULL },			/* some c don't like empty initializer
				   lists */
    { NULL }, 
    SO_SRCFLG,
    NULL,
    { 0 },			/* ditto */
    NULL,			/* there's only the default connection */
    NULL,			/* just to fool the dynamic loader code.... */
    NULL,
    ldd_setconnection,		/* this just to be on the safe side */
    NULL,
    NULL,
    ldr_freestatement,
    NULL,
    NULL,
    ldd_prepare,
    NULL,
    NULL,
    NULL,
    ldr_nextrow,
    ldr_nexttoken,
    ldd_counttokens,
    NULL,
    NULL
};
#endif

/*
** just in case the dynamic loader code erroneusly decides to set a connection
*/
static void ldd_setconnection(char *host, char *name, ca_t *ca, char *private_area)
{
}

/*
**  instantiate stmt structure
*/
int ldd_newstatement(fgw_stmttype *st, char *d, char *e)
{
    fgw_streamtype *st_p;
 
    if ((st_p=(fgw_streamtype *) malloc(sizeof(fgw_streamtype)))!=NULL)
    {
	memset(st_p, 0, sizeof(fgw_streamtype));
	st->sqlstmt=st_p;
	st->source=&lddsrc;
	if (*d)
	    st_p->del=*d;
	if (*e)
	    st_p->esc=*e;
	return 0;
    }
    return RC_NOMEM;
}

/*
**  free used resources
*/
static void ldd_freestatement(fgw_stmttype *st, int clean)
{
    fgw_streamtype *st_p;

    if (st && (st_p=(fgw_streamtype *) st->sqlstmt))
    {
	if (st_p->exprlist)
	    free(st_p->exprlist);
	if (st->stream && st->stream->fd_entry.signature>0 &&
	    !(st->stream->fd_entry.signature & SS_INTERNAL))
	    fgw_fdclose(st->stream);
	free(st_p);
    }
}

/*
**  opens stream
*/
static void ldd_prepare(fgw_stmttype *st, char *query)
{
    int r;
    fgw_streamtype *st_p;

    if (st && st->curstate==ST_UNINITIALIZED &&
        (st_p=(fgw_streamtype *) st->sqlstmt))
    {
	memset(st->ca, 0, sizeof(ca_t));
	if (st->stream)
	{
	    if (!st->stream->fd_entry.signature)
		st->stream->fd_entry.signature=st->st_entry.signature;

	    /* automatic reallocation */
	    st->stream->fd_linelen=0;
	    if (st->blocksize>st->stream->fd_bufinc)
		st->stream->fd_bufinc=st->blocksize;
	}
	else
	{
		st->ca->sqlcode=errno; /* FIXME internal error */
		return;
	}
	st->curstate=ST_DECLARED;
    }
    else if (st)
        st->ca->sqlcode=RC_INSID;
}

/*
**  loads next line from stream
*/
#define TOKSTART (st_p->scanstart)
static void ldd_nextrow(fgw_stmttype *st)
{
    fgw_streamtype *st_p;
    fgw_fdesc *fd_p;
    char c, *curchar;
    int nextline;
    int linelen=0;
    int fieldcount=0, newline=0;
    exprstack_t *e;
    char *buf;
    int r;

    if (!st)
	return;
    else if (!(st_p=(fgw_streamtype *) st->sqlstmt))
    {
	st->ca->sqlcode=RC_INSID;
	return;
    }
    else if (st->curstate<ST_DECLARED)
    {
	st->ca->sqlcode=RC_NDECL;
	return;
    }
    memset(st->ca, 0, sizeof(st->ca));
    fd_p=st->stream;
    st->lastrowlen=0;           /* ditch previous reject row */
/*
** the last row is still in memory to allow ldd_nexttoken() to return
** meaningful values. check if we need space and get rid of it now
*/
    if (st_p->exprlist || TOKSTART)
    {
	if (TOKSTART>fd_p->fd_bufsize/2)
	{
	    fd_p->fd_buflen-=TOKSTART;
	    fgw_move(fd_p->fd_buf, fd_p->fd_buf+TOKSTART, fd_p->fd_buflen);
	    TOKSTART=0;
	}
    }
    else
	st->curstate=ST_NEWLYOPENED;
/*
** external loop
*/
    for (;;)
    {
/*
** while there is something to process
*/
	if (fd_p->fd_buflen-TOKSTART>0)
	{

/*
** look for next delimiter
*/
	    for (curchar=fd_p->fd_buf+TOKSTART; c=*curchar; curchar++)
	    {
		linelen++;
		if (r=(c=='\n' || c==st_p->del))
		    break;
		else if (c==st_p->esc)
		{
/*
** '\ ' at the beginning of the line is a non null zero length string
** (do not get me started) - this special case will be handled later
*/
		    if (curchar==fd_p->fd_buf+TOKSTART)
		    {
			curchar++;
			linelen++;
		    }
		    else
		    {
			fgw_move(curchar, curchar+1, fd_p->fd_buflen-linelen);
			fd_p->fd_buflen--;
		    }
		}
	    }
	    if (r)
	    {
		if (c=='\n')
		{
		    newline=1;
		    nextline=(long) (curchar-fd_p->fd_buf+1);
/*
** skip carriage returns
*/
		    if (*(curchar-1)=='\r')
			*--curchar='\0';
/*
** standard delimited files have a final delimiter before the newline.
** here we cater for this and skip the extra null field
*/
		    if (!*(curchar-1))
		    {
			st->curstate=ST_OPENED;
			st_p->fields=fieldcount;
			TOKSTART=nextline;
			return;
		    }
		}
	    }
	    else if (st_p->eof)
	    {
		newline=1;
		nextline=(long) (curchar-fd_p->fd_buf+1);
		r=(fieldcount||(curchar>fd_p->fd_buf+TOKSTART));
	    }
/*
** process next field
*/
	    if (r)
	    {
		fieldcount++;
/*
** still counting input fields: have to reallocate expression buffer
*/
		if (fieldcount>st_p->max)
		{
		    if (!(buf=realloc(st_p->exprlist, sizeof(exprstack_t)*fieldcount)))
		    {
			st->ca->sqlcode=RC_NOMEM;
			return;
		    }
		    st_p->exprlist=(exprstack_t *) buf;
		    st_p->max=fieldcount;
		    e=st_p->exprlist+fieldcount-1;
		    memset(e, 0, sizeof(exprstack_t));
		    e->type=CSTRINGTYPE;
		}
		else
		    e=st_p->exprlist+fieldcount-1;
/*
** automatic reallocation means that the buffer can change as we are processing 
** fields hence we only store an offset initially
*/
		e->flags|=EF_RELADDR;
                e->val.string=(char *) ((long) TOKSTART);
		rqx_rejcopy(st, fd_p->fd_buf+TOKSTART,
			    (int) (curchar-(fd_p->fd_buf+TOKSTART)));
                *curchar='\0';
/*
** we're done
*/
		if (newline)
		{
		    st->curstate=ST_OPENED;
		    st_p->fields=fieldcount;
		    TOKSTART=nextline;
		    return;
		}
		else
		{
		    TOKSTART=(long) (curchar-fd_p->fd_buf+1);
		    continue;
		}
	    }
	}
/*
** need to read some more & maybe realloc buffer
*/
	if ((r=fgw_fdread(st->stream))<0)
	{
	    st->ca->sqlcode=errno;
	    return;
	}
/*
** nothing more to read
*/
	else if (r==0)
	{
/*
** no buffer to process, we're done
*/
	    if (st_p->eof)
	    {
		st->ca->sqlcode=RC_NOTFOUND;
		return;
	    }
	    else
		st_p->eof=1;
	}
    }
}

/*
** returns next field in stream buffer
*/
static exprstack_t *ldd_nexttoken(int field, fgw_stmttype *st_p)
{
    exprstack_t *e;

    if (field>=((fgw_streamtype *) st_p->sqlstmt)->fields)
	return NULL;
    e=((fgw_streamtype *) st_p->sqlstmt)->exprlist+field;
/*
** fixup pointer and length
*/
    if (e->flags & EF_RELADDR)
	e->val.string+=(long) (st_p->stream->fd_buf);
    e->flags &=~ EF_RELADDR;
    if (!*e->val.string)
	e->length=-1;
    else
    {
	e->length=0;
/*
** "\ " is empty non null string
*/
	if (*e->val.string==((fgw_streamtype *) st_p->sqlstmt)->esc)
	{
	    if (*(e->val.string+1)==' ' && !*(e->val.string+2))
		*e->val.string='\0';
	    else
		e->val.string++;
	}
    }
    return e;
}

/*
** returns field count in stream buffer
*/
static int ldd_counttokens(fgw_stmttype *st_p)
{
    return ((fgw_streamtype *) st_p->sqlstmt)->fields;
}

#ifdef HAVE_REGEX
/*
**  instantiate stmt structure
*/
int ldr_newstatement(fgw_stmttype *st, char *d)
{
    fgw_streamtype *st_p;
    char buf[MAXDELSIZE+1];
 
    if ((st_p=(fgw_streamtype *) malloc(sizeof(fgw_streamtype)))!=NULL)
    {
	memset(st_p, 0, sizeof(fgw_streamtype));
	st->sqlstmt=st_p;
	st->source=&ldrsrc;
	sprintf((char *) &buf, "((%s)|$)", d);
	if (regcomp(&st_p->pattern, buf, REG_EXTENDED|REG_NEWLINE))
	{
	    free(st_p);
	    return RC_RESYN;
	}
	return 0;
    }
    return RC_NOMEM;
}

/*
**  free used resources
*/
static void ldr_freestatement(fgw_stmttype *st, int clean)
{
    fgw_streamtype *st_p;

    if (st && (st_p=(fgw_streamtype *) st->sqlstmt))
    {
	if (st_p->exprlist)
	    free(st_p->exprlist);
	if (st->stream && st->stream->fd_entry.signature>0 &&
	    !(st->stream->fd_entry.signature & SS_INTERNAL))
	    fgw_fdclose(st->stream);
	regfree(&st_p->pattern);
	free(st_p);
    }
}

/*
**  loads next line from stream
*/
#define LASTMATCH (st_p->scanstart)
static void ldr_nextrow(fgw_stmttype *st)
{
    fgw_streamtype *st_p;
    fgw_fdesc *fd_p;
    regmatch_t match;
    int fieldcount=0, newline=0;
    exprstack_t *e;
    char *buf;
    int r;

    if (!st)
	return;
    else if (!(st_p=(fgw_streamtype *) st->sqlstmt))
    {
	st->ca->sqlcode=RC_INSID;
	return;
    }
    else if (st->curstate<ST_DECLARED)
    {
	st->ca->sqlcode=RC_NDECL;
	return;
    }
    memset(st->ca, 0, sizeof(st->ca));
    fd_p=st->stream;
    st->lastrowlen=0;			/* ditch previous reject row */
/*
** the last row is still in memory to allow ldr_nexttoken() to return
** meaningful values. check if we need space and get rid of it now
*/
    if (st_p->exprlist || LASTMATCH)
    {
        if (LASTMATCH>fd_p->fd_bufsize/2)
        {
            fd_p->fd_buflen-=LASTMATCH;
            fgw_move(fd_p->fd_buf, fd_p->fd_buf+LASTMATCH, fd_p->fd_buflen);
            LASTMATCH=0;
        }
    }
    else
	st->curstate=ST_NEWLYOPENED;
/*
** external loop
*/
    for (;;)
    {
/*
** while there is something to process
*/
	if (fd_p->fd_buflen-LASTMATCH>0)
	{
/*
** look for next delimiter
*/
	    if (r=(!regexec(&st_p->pattern, fd_p->fd_buf+LASTMATCH, 1, &match,
			     REG_NOTBOL|REG_NOTEOL)))
	    {
/*
** new line found
*/
		if (*(fd_p->fd_buf+LASTMATCH+match.rm_so)=='\n')
		{
		    newline=1;
/*
** skip carriage returns as well
*/
		    if (*(fd_p->fd_buf+LASTMATCH+match.rm_so-1)=='\r')
			match.rm_so--;
		}
	    }
	    else if (st_p->eof)
	    {
		match.rm_so=fd_p->fd_buflen-LASTMATCH;
		match.rm_eo=fd_p->fd_buflen-LASTMATCH;
		newline=1;
		r=(fieldcount || match.rm_so);
	    }
/*
** process next field
*/
	    if (r)
	    {
		fieldcount++;
/*
** still counting input fields: have to reallocate expression buffer
*/
		if (fieldcount>st_p->max)
		{
		    if (!(buf=realloc(st_p->exprlist, sizeof(exprstack_t)*fieldcount)))
		    {
			st->ca->sqlcode=RC_NOMEM;
			return;
		    }
		    st_p->exprlist=(exprstack_t *) buf;
		    st_p->max=fieldcount;
		    e=st_p->exprlist+fieldcount-1;
		    memset(e, 0, sizeof(exprstack_t));
		    e->type=CSTRINGTYPE;
		}
		else
		    e=st_p->exprlist+fieldcount-1;
/*
** automatic reallocation means that the buffer can change as we are processing 
** fields hence we only store an offset initially
*/
		e->flags|=EF_RELADDR;
		e->val.string=(char *) ((long) LASTMATCH);
		rqx_rejcopy(st, fd_p->fd_buf+LASTMATCH, match.rm_eo+newline);
		*(fd_p->fd_buf+LASTMATCH+match.rm_so)='\0';
		LASTMATCH+=match.rm_eo+newline;
/*
** we're done
*/
		if (newline)
		{
		    st->curstate=ST_OPENED;
		    st_p->fields=fieldcount;
		    return;
		}
		else
		    continue;
	    }
	}
/*
** need to read some more & maybe realloc buffer
*/
	if ((r=fgw_fdread(st->stream))<0)
	{
	    st->ca->sqlcode=errno;
	    return;
	}
/*
** nothing more to read
*/
	else if (r==0)
	{
/*
** no buffer to process, we're done
*/
	    if (st_p->eof)
	    {
		st->ca->sqlcode=RC_NOTFOUND;
		return;
	    }
/*
** do the last row
*/
	    else
		st_p->eof=1;
	}
    }
}

/*
** returns next field in stream buffer
*/
static exprstack_t *ldr_nexttoken(int field, fgw_stmttype *st_p)
{
    exprstack_t *e;

    if (field>=((fgw_streamtype *) st_p->sqlstmt)->fields)
	return NULL;
    e=((fgw_streamtype *) st_p->sqlstmt)->exprlist+field;
/*
** fixup pointer and length
*/
    if (e->flags & EF_RELADDR)
	e->val.string+=(long) (st_p->stream->fd_buf);
    e->flags &=~ EF_RELADDR;
    if (!*e->val.string)
	e->length=-1;
    else
	e->length=0;
    return e;
}
#endif
