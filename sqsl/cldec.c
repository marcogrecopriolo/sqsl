/*
	CLDEC.c  -  SQSL stream input functions, arbitrary extractor

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: May 16
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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ccmnc.h"
#include "cfioc.h"
#include "csqrc.h"
#include "crqxp.h"

static void lde_setconnection(char *host, char *name, ca_t *ca, char *private_area);
static void lde_freestatement(fgw_stmttype *st, int clean);
static void lde_prepare(fgw_stmttype *st, char *query);
static void lde_nextrow(fgw_stmttype *st);
static exprstack_t *lde_nexttoken(int field, fgw_stmttype *st_p);
static int lde_counttokens(fgw_stmttype *st_p);

typedef struct fgw_stream
{
    exprstack_t *exprlist;	/* stack containing one row of data */
    int fields, max;
    int eof;
    int scanstart;
} fgw_streamtype;

/*
** this source doesn't go in the source list, can't be used directly,
** so NULLs are OK in most places
*/
static fgw_sourcetype ldesrc = {
    { NULL },			/* some c don't like empty initializer
				   lists */
    { NULL }, 
    SO_SRCFLG,
    NULL,
    { 0 },			/* ditto */
    NULL,			/* there's only the default connection */
    NULL,			/* just to fool the dynamic loader code.... */
    NULL,
    lde_setconnection,		/* this just to be on the safe side */
    NULL,
    NULL,
    lde_freestatement,
    NULL,
    NULL,
    lde_prepare,
    NULL,
    NULL,
    NULL,
    lde_nextrow,
    lde_nexttoken,
    lde_counttokens,
    NULL,
    NULL
};

/*
** decode binary format row
*/
int lde_decode(char *source, int *currfield, exprstack_t *expr, fgw_tsstype **tss)
{
    return RC_NIMPL;
}

/*
** just in case the dynamic loader code erroneusly decides to set a connection
*/
static void lde_setconnection(char *host, char *name, ca_t *ca, char *private_area)
{
}

/*
**  opens stream
*/
static void lde_prepare(fgw_stmttype *st, char *query)
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
** returns field count in stream buffer
*/
static int lde_counttokens(fgw_stmttype *st_p)
{
    return ((fgw_streamtype *) st_p->sqlstmt)->fields;
}

/*
**  instantiate stmt structure
*/
int lde_newstatement(fgw_stmttype *st, char *m, char *e)
{
    fgw_streamtype *st_p;
 
    if ((st_p=(fgw_streamtype *) malloc(sizeof(fgw_streamtype)))!=NULL)
    {
	memset(st_p, 0, sizeof(fgw_streamtype));
	st->sqlstmt=st_p;
	st->source=&ldesrc;
	return 0;
    }
    return RC_NOMEM;
}

/*
**  free used resources
*/
static void lde_freestatement(fgw_stmttype *st, int clean)
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
**  loads next line from stream
*/
#define NEXTMATCH (st_p->scanstart)
static void lde_nextrow(fgw_stmttype *st)
{
    fgw_streamtype *st_p;
    fgw_fdesc *fd_p;
    int lastmatch, needmore;
    int fieldcount=0, newline=0;
    exprstack_t *e, expr;
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
    st->lastrowlen=0;		/* ditch previous reject row */
/*
** the last row is still in memory to allow lde_nexttoken() to return
** meaningful values. check if we need space and get rid of it now
*/
    if (st_p->exprlist || NEXTMATCH)
    {
        if (NEXTMATCH>fd_p->fd_bufsize/2)
        {
            fd_p->fd_buflen-=NEXTMATCH;
            fgw_move(fd_p->fd_buf, fd_p->fd_buf+NEXTMATCH, fd_p->fd_buflen);
            NEXTMATCH=0;
        }
    }
    else
	st->curstate=ST_NEWLYOPENED;
/*
** external loop
*/
    for (;;)
    {
	needmore=0;
/*
** while there is something to process
*/
	if (fd_p->fd_buflen-NEXTMATCH>0)
	{
/*
** look for next field
*/
	    lastmatch=NEXTMATCH;
	    if (!(r=lde_decode(fd_p->fd_buf, &NEXTMATCH, &expr, NULL)))
	    {
		fieldcount++;
/*
** still counting input fields: have to reallocate expression buffer
*/
		if (fieldcount>st_p->max)
		{
		    if (!(buf=realloc(st_p->exprlist, sizeof(exprstack_t)*
							fieldcount)))
		    {
			st->ca->sqlcode=RC_NOMEM;
			return;
		    }
		    st_p->exprlist=(exprstack_t *) buf;
		    st_p->max=fieldcount;
		    e=st_p->exprlist+fieldcount-1;
		}
		else
		    e=st_p->exprlist+fieldcount-1;
		memcpy(e, &expr, sizeof(exprstack_t));
/*
** automatic reallocation means that the buffer can change as we are processing 
** fields hence we only store an offset initially
*/
		if (e->type==CSTRINGTYPE)
		{
		    e->flags|=EF_RELADDR;
		    e->val.string=(char *) ((long) e->val.string-
					(long) fd_p->fd_buf);
		}
		rqx_rejcopy(st, fd_p->fd_buf+lastmatch, NEXTMATCH-lastmatch);
	    }
/*
** we're done
*/
	    else if (r==RC_NOTFOUND)
	    {
		if (fieldcount)
		{
		    st->curstate=ST_OPENED;
		    st_p->fields=fieldcount;
		    return;
		}
		else if (st_p->eof)
		{
		    st->ca->sqlcode=RC_NOTFOUND;
		    return;
		}
	    }
/*
** we may have not had enough input to get to the next pattern, in which
** case, read more and try again; any other error, get out!
*/
	    
	    else if (r==EAGAIN)
		needmore=1;
	    else
	    {
		st->ca->sqlcode=r;
		return;
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
	    if (needmore)
		st->ca->sqlcode=RC_NOMTC;
	    else
		st->ca->sqlcode=RC_NOTFOUND;
	    return;
	}
    }
}

/*
** returns next field in stream buffer
*/
static exprstack_t *lde_nexttoken(int field, fgw_stmttype *st_p)
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
