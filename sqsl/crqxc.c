/*
	CRQXC.c  -  SQSL interpreter dynamic query execution

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ccmnc.h"
#include "cfnmc.h"
#include "coslc.h"
#include "crqfc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqxp.h"
#include "crxuc.h"
#include "crxxp.h"
#include "cxslc.h"

#ifndef FGW_DEFSOURCE
#define FGW_DEFSOURCE "ifmx"
#endif

/* some cc's think hex constants >0x8000000 are unsigned ints */
static FGW_LISTSERIAL(stlist, (int) RC_INSID);
static FGW_LISTCHAR(connlist, (int) RC_INCON);

/* we maintain two lists from sources: by name and by id */
static FGW_LISTCHAR(srclist, (int) RC_INSRC);
static FGW_LISTSERIAL(srcser, (int) RC_INSRC);

static fgw_sourcetype *currsrc;

#define SRCNEEDED  1
#define MANDATORY -1
#define OPTIONAL   0

typedef struct
{
    char *name;
    int state;
} funclist_t;

static funclist_t funclist[]=
{
    "sqd_connect", SRCNEEDED,
    "sqd_setconnection", SRCNEEDED,
    "sqd_disconnect", SRCNEEDED,
    "sqd_newstatement", SRCNEEDED,
    "sqd_freestatement", SRCNEEDED,
    "sqd_closestatement", SRCNEEDED,
    "sqd_setcursor", SRCNEEDED,
    "sqd_prepare", SRCNEEDED,
    "sqd_allocateda", SRCNEEDED,
    "sqd_bindda", SRCNEEDED,
    "sqd_immediate", SRCNEEDED,
    "sqd_nextrow", SRCNEEDED,
    "sqd_nexttoken", SRCNEEDED,
    "sqd_counttokens", SRCNEEDED,
    "sqd_errmsg", MANDATORY,
    "sqd_break", OPTIONAL,
    "sqd_describecol", OPTIONAL,
    "sqd_autocommit", OPTIONAL,
    NULL, 0
};

TGLOB void (*sqd_break)()=NULL;

static exprstack_t nullexpr;
static char n[2];

XSL_EXPORTED

/*
**  maps sql driver sql code to parser sqlcodes
*/
static int fromsqlcode(int c, int d)
{
    if (!c || c==RC_NOTFOUND || SQ_ISPARSER(SQ_TOSRC(c)))
	return c;
    else
	return SQ_FROMSQLCODE(c, d);
}

/*
** hystorically there was a need to keep track of all sources and connections
** under one roof. this is no longer the case
*/
#define fgw_tssappend(s) strdup(s)
#define fgw_tssditch(s) free(s)

/*
** maps error returned by a UDF to the individual shared library
*/
int rqx_rc(int rc, int src_id)
{
    if (src_id==0)
	return rc;
    return SQ_FROMSQLCODE(rc, src_id);
}
	
/*
** SIGINT signal handler
*/
void rqx_sendbreak()
{
    if (sqd_break)
	(*sqd_break)();
    int_flag=1;
}

/*
** opens shared object file
*/
static fgw_sourcetype *rqx_dlload(parserstate_t *pstate, char *s)
{
    char fname[FNAMESIZE], *c;
    int l;
    void *handle;
    int flags, (*init)(XSL_PARMS);
    funclist_t *f;
    void **p;
    fgw_sourcetype *src;

    if (srcser.base==FGW_MAXDLL)
    {
	errno=RC_TMDLL;
	return NULL;
    }
    if (!(c=(char *) getenv(FGW_RESOURCEDIR)))
	c=".";
    l=sizeof(fname)-strlen(c)-1;
    fname[FNAMESIZE-1]='\0';
    strncpy((char *) &fname, c, (l>0? l: 0));
    l=sizeof(fname)-strlen(fname)-1;
    strncat((char *) &fname, SL_SLASH, (l>0? l: 0));
    l=sizeof(fname)-strlen(fname)-1;
    strncat((char *) &fname, "fgw", (l>0? l: 0));
    l=sizeof(fname)-strlen(fname)-1;
    strncat((char *) &fname, s, (l>0? l: 0));
    l=sizeof(fname)-strlen(fname)-1;
    strncat((char *) &fname, SL_EXT, (l>0? l: 0));
    pstate->ca.sqlcode=RC_INSRC;
    if (!(handle=fgw_dlopen((char *) &fname)))
    {
	if (pstate->errorinfo)
	    sprintf((char *) &pstate->errorinfo->extra, "(%s)", fgw_dlerror());
	return NULL;
    }
    if (!(init=(int (*)()) fgw_dlsym(handle, "sqd_init")))
	goto badexit3;
    flags=(*init)(XSL_ARGS);
    if (flags==-1)
    {
	if (pstate->errorinfo)
	    sprintf((char *) &pstate->errorinfo->extra, "(%d)", errno);
	goto badexit3;
    }
    else if ((flags & SO_MASK)!=SO_MASK)
	goto badexit3;
    flags&=~SO_MASK;
    if (!(src=(fgw_sourcetype *) malloc(sizeof(fgw_sourcetype))))
	goto badexit2;
    memset(src, 0, sizeof(fgw_sourcetype));
    if (!(src->sr_entry.char_id=fgw_tssappend(s)))
	goto badexit;
    src->object.cmpr=fgw_stringcomp;
    src->object.hash=fgw_stringhash;
    src->object.root.size=FGW_LISTHASHSZ;
/*
** look for dynamic sql routines
*/
    p=(void **) &src->sqd_connect;
    for (f=(funclist_t *) &funclist; f->name; f++)
    {
	if (!(*p++=fgw_dlsym(handle, f->name)))
	{
	    if (f->state== SRCNEEDED)
		flags&=~SO_SRCFLG;
	    else if (f->state==MANDATORY)
	    {
		free(src);
		goto badexit3;
	    }
	}
    }
    fgw_newentry((fgw_listentry *) src, &srclist);
    src->sr_serial.signature=0;
    fgw_newentry(&src->sr_serial, &srcser);
    src->handle=handle;
    src->flags=flags;
    pstate->ca.sqlcode=0;
    return src;
badexit:
    free(src);
badexit2:
    pstate->ca.sqlcode=RC_NOMEM;
badexit3:
    fgw_dlclose(handle);
    return NULL;
}

/*
** returns pointer to external function
*/
fgw_objtype *rqx_getobject(parserstate_t *pstate, char *s,  char *n)
{
    fgw_listentry e;
    fgw_sourcetype *src=NULL;
    fgw_objtype *o;
    int (*func)();

    e.char_id=s;
    e.signature=0;
    if (!(src=(fgw_sourcetype *) fgw_listsearch(&e, &srclist)))
	src=rqx_dlload(pstate, s);
    if (src && (src->flags | SO_XPRFLG))
    {
	e.char_id=n;
	if (o=(fgw_objtype *) fgw_listsearch(&e, &src->object))
	    return o;
	else
	{
	    if (!(func=(int (*)()) fgw_dlsym(src->handle, n)))
	    {
		pstate->ca.sqlcode=RC_INOBJ;
		return NULL;
	    }
	    if (o=(fgw_objtype *) malloc(sizeof(fgw_objtype)))
	    {
		o->ob_entry.signature=0;
		o->src_id=src->sr_serial.int_id;
		o->obj=func;
		if (o->ob_entry.char_id=fgw_tssappend(n))
		{
		    fgw_newentry((fgw_listentry *) o, &src->object);
		    return o;
		}
	    }
	}
    }
    else if (!pstate->ca.sqlcode)
	pstate->ca.sqlcode=status;

    return NULL;
}

/*
** finds a sql source structure
*/
static fgw_sourcetype *rqx_findsource(parserstate_t *pstate, char *c, int load)
{
    fgw_listentry e;
    fgw_sourcetype *src=NULL;

    e.char_id=c;
    e.signature=0;
    if ((!(src=(fgw_sourcetype *) fgw_listsearch(&e, &srclist))) && load)
	src=rqx_dlload(pstate, c);
    if (src && (src->flags & SO_SRCFLG))
	return src;
    return NULL;
}

/*
** finds a connection structure
*/
static fgw_conntype *rqx_findconn(char *c, int s)
{
    fgw_listentry e;

    e.char_id=c;
    e.signature=s;
    return (fgw_conntype *) fgw_listsearch(&e, &connlist);
}

/*
 * source initialization
 */
int rqx_init(parserstate_t *pstate, int lastcon)
{
    char extramsg_save[ERRMSGLEN];

    if (!srclist.head)
    {
	currsrc=rqx_dlload(pstate, FGW_DEFSOURCE);
	if (pstate->ca.sqlcode)
	    goto badexit;
	if (currsrc && !(currsrc->flags & SO_SRCFLG))
	{
	    pstate->ca.sqlcode=RC_INSRC;
	    goto badexit;
	}
/*
** if no connection has been attempted, then we have a current implicit
** default connection, otherwise the default connection is not current
** (we try and avoid spurious -1803)
*/
	if (lastcon)
	    currsrc->lastcon=(fgw_conntype *) -1;
	nullexpr.val.string=(char *) &n;
	rxu_setnull(&nullexpr, CSTRINGTYPE);
    }
    return 0;

badexit:
    if (pstate->errorinfo)
    {
	if (pstate->errorinfo->extra[0])
	    strcpy(extramsg_save, pstate->errorinfo->extra);
	sprintf(pstate->errorinfo->extra, "(%s)", FGW_DEFSOURCE);
	if (extramsg_save[0])
	{
	    strcat(pstate->errorinfo->extra, " ");
	    strcat(pstate->errorinfo->extra, extramsg_save);
	}
    }
    return pstate->ca.sqlcode;
}

/*
**  connect to an engine
*/
void rqx_connect(parserstate_t *pstate, char *host, char *source,
		 char *as, char *user, char *passwd, char *opt)
{
    fgw_sourcetype *src;
    fgw_conntype *con;
    char *cname=as? as: host;
    char *private_area=NULL;

    memset(&pstate->ca, 0, sizeof(ca_t));
/*
** cannot have duplicate connection names
*/
    if (cname && rqx_findconn(cname, pstate->signature))
    {
	pstate->ca.sqlcode=RC_DUPCN;
	return;
    }
    if (!source)
	src=currsrc;
    else if (!(src=rqx_findsource(pstate, source, 1)))
    {
	if (!pstate->ca.sqlcode)
	     pstate->ca.sqlcode=RC_INSRC;
	return;
    }
    sqd_break=src->sqd_break;
    (src->sqd_connect)(host, as, user, passwd, opt, &pstate->ca, &private_area);
    sqd_break=NULL;
    pstate->ca.sqlcode=fromsqlcode(pstate->ca.sqlcode, src->sr_serial.int_id);
    if (cname && !pstate->ca.sqlcode)
    {
	if (!(con=(fgw_conntype *) malloc(sizeof(fgw_conntype))))
	    goto badexit2;
	else
	{
	    con->host=NULL;
	    if (!(con->cn_entry.char_id=fgw_tssappend(cname)))
		goto badexit;
	    if (host && cname!=host && !(con->host=fgw_tssappend(host)))
		goto badexit1;
	    con->cn_entry.signature=pstate->signature;
	    con->source=src;
	    con->refcnt=0;
	    con->state=1;
	    con->private_area=private_area;
	    fgw_newentry((fgw_listentry *) con, &connlist);
	    src->currcon=con;
	    src->lastcon=con;
	    currsrc=src;
	}
    }
    return;
badexit:
    fgw_tssditch(con->cn_entry.char_id);
badexit1:
    free(con);
badexit2:
    sqd_break=src->sqd_break;
    (src->sqd_disconnect)(host, cname, &pstate->ca, private_area);
    sqd_break=NULL;
    pstate->ca.sqlcode=RC_NOMEM;
}
 
/*
** set current connection
*/
void rqx_setconnection(parserstate_t *pstate, char *cname, char *source)
{
    fgw_sourcetype *src=NULL;
    fgw_conntype *c=NULL;

    memset(&pstate->ca, 0, sizeof(ca_t));
    if (cname)
    {
	if (c=rqx_findconn(cname, pstate->signature))
	    src=c->source;
    }
    else if (!source)
	src=currsrc;
    else
	src=rqx_findsource(pstate, source, 1);
    if (src)
    {
	if (src->lastcon!=c)
	{
	    sqd_break=src->sqd_break;
	    (src->sqd_setconnection)(c? c->host: NULL, cname, &pstate->ca,
				     c? c->private_area: NULL);
	    sqd_break=NULL;
	    if (!(pstate->ca.sqlcode=fromsqlcode(pstate->ca.sqlcode,
						 src->sr_serial.int_id)))
	    {
		src->currcon=c;
		src->lastcon=c;
	    }
	}
	currsrc=src;
    }
    else if (!pstate->ca.sqlcode)
	pstate->ca.sqlcode=status;
}
 
/*
** disconnect from engine
*/
void rqx_disconnect(parserstate_t *pstate, char *cname, char *source)
{
    fgw_sourcetype *src=NULL;
    fgw_conntype *c=NULL;

    memset(&pstate->ca, 0, sizeof(ca_t));
    if (cname)
    {
	if (c=rqx_findconn(cname, pstate->signature))
	    src=c->source;
    }
    else if (!source)
	src=currsrc;
    else
	src=rqx_findsource(pstate, source, 0);
    if (src)
    {
	sqd_break=src->sqd_break;
	(src->sqd_disconnect)(c? c->host: NULL, cname, &pstate->ca, 
			      c? c->private_area: NULL);
	sqd_break=NULL;
	if (!(pstate->ca.sqlcode=fromsqlcode(pstate->ca.sqlcode,
					     src->sr_serial.int_id)))
	{
/*
** avoid affecting the current source if at all possible
*/
	    src->currcon=NULL;
	    src->lastcon=(fgw_conntype *) -1;
	    if (c)
		if (c->refcnt)
		    c->state=0;
		else
		{
		    if (c->host && c->host!=c->cn_entry.char_id)
			fgw_tssditch(c->host);
		    fgw_tssditch(c->cn_entry.char_id);
		    fgw_dropentry((fgw_listentry *) c, &connlist);
		}
	}
    }
    else if (!pstate->ca.sqlcode)
	pstate->ca.sqlcode=status;
}

/*
** set temporary connection
*/
void rqx_setstmtcon(fgw_stmttype *st_p, char *as, char *source,
		    parserstate_t *pstate)
{
    fgw_sourcetype *src=NULL;

/*
** we allocate the connection as follows
** 1- it is specified and we find it
** 2- it is specified and is the current connection of the current source
** 3- a source is specified: we use that source's default connection
** 4- we use the current source default connection
*/
    memset(st_p->ca, 0, sizeof(ca_t));
    if (as)
    {
	if (pstate && (st_p->con=rqx_findconn(as, pstate->signature)))
	{
	    st_p->con->refcnt++;
	    st_p->source=st_p->con->source;
	}
	else if (currsrc && currsrc->currcon &&
		 !strcmp(currsrc->currcon->cn_entry.char_id, as))
	{
	    st_p->con=currsrc->currcon;
	    st_p->con->refcnt++;
	    st_p->source=st_p->con->source;
	}
    }
    else if (source)
	st_p->source=rqx_findsource(pstate, source, 1);
    else
	st_p->source=currsrc;
    if (!st_p->con && !st_p->source)
	st_p->ca->sqlcode=status;
    else if (!(st_p->sqlstmt=(st_p->source->sqd_newstatement)()))
	st_p->ca->sqlcode=RC_NOMEM;
}

/*
** returns current source name
*/
char *rqx_getsource()
{
    return currsrc->sr_entry.char_id;
}

/*
** returns current connection name
*/
char *rqx_getconnection()
{
    if (currsrc->currcon)
	return currsrc->currcon->cn_entry.char_id;
    else
	return NULL;
}

/*
**  finds a statement
*/
fgw_stmttype *rqx_findstmt(int st_id, int s)
{
    fgw_listentry e;

    e.int_id=st_id;
    e.signature=s;
    return (fgw_stmttype *) fgw_listsearch(&e, &stlist);
}

/*
** register statement in the statement stack
*/
int rqx_registerstatement(parserstate_t *pstate, fgw_stmttype *st_n)
{
    if (!st_n)
	return -1;
    if (pstate->stmtcnt++==0)
	pstate->curstmt=st_n;
    else
	pstate->ssp++;
    pstate->ssp->stmt=st_n;
    return 0;
}
/*
**  instantiate statement structure
**  FIXME, might not need to instantiate source, connection
*/
fgw_stmttype *rqx_newstatement(parserstate_t *pstate)
{
    fgw_stmttype *st_n;
    static ca_t ca;

    if ((st_n=(struct fgw_stmt *) malloc(sizeof(struct fgw_stmt)))!=NULL)
    {
	memset(st_n, 0, sizeof(struct fgw_stmt));
	st_n->width=MINWIDTH;
	if (pstate)
	    st_n->st_entry.signature=pstate->signature;
	else
	    st_n->st_entry.signature=0;
	if (pstate)
	{
	    st_n->ca=&(pstate->ca);
	    st_n->oserr=&(pstate->oserr);
	    st_n->vars=pstate->vars;
	}
	else
	{
	    st_n->ca=&ca;
	    st_n->oserr=&ca;
	}
	fgw_stackinit(&st_n->stack, sizeof(exprstack_t));
	fgw_newentry((fgw_listentry *) st_n, &stlist);
    }
    return st_n;
}

/*
**  free used statement resources
*/
void rqx_freestatement(fgw_stmttype *st_p)
{
    int i;

    if (st_p)
    {
	if (st_p->reject)
	{
	    fgw_fdclose(st_p->reject);
	    st_p->oserr->sqlcode=errno;
	    status=0;
	    st_p->reject=NULL;
	}
	if (st_p->source)
	    (st_p->source->sqd_freestatement)(st_p, 1);
	if (st_p->formats)
	    rqf_fmtclear(st_p->formats);
	if (st_p->headers)
	    rqf_fmtclear(st_p->headers);
	if (st_p->htmlhead)
	    fgw_tssdrop(&st_p->htmlhead);
	if (st_p->columns)
	{
	    fgw_tssdrop(&st_p->columnhead);
	    free(st_p->columns);
	}
	rxx_droppcode(&st_p->intovars);
	rxx_droppcode(&st_p->aggregates);
	rxx_droppcode(&st_p->usingvars);
	fgw_tssdrop(&st_p->usinghead);
	fgw_stackfree(&st_p->stack, NULL);
	if (st_p->con)
	{
	    st_p->con->refcnt--;
/*
**  need to free connection too
*/
	    if (!st_p->con->state && !st_p->con->refcnt)
	    {
		if (st_p->con->host!=st_p->con->cn_entry.char_id)
		    fgw_tssditch(st_p->con->host);
		fgw_tssditch(st_p->con->cn_entry.char_id);
		fgw_dropentry((fgw_listentry *) st_p->con, &connlist);
	    }
	}
	fgw_dropentry((fgw_listentry *) st_p, &stlist);
    }
}

/*
**  close statement for later reuse
*/
void rqx_closestatement(fgw_stmttype *st_p)
{
    int i;

    if (st_p)
    {
	if (st_p->reject)
	{
	    fgw_fdclose(st_p->reject);
	    st_p->oserr->sqlcode=errno;
	    status=0;
	    st_p->reject=NULL;
	}
	if (st_p->source)
	    (st_p->source->sqd_closestatement)(st_p);
	if (st_p->options & SO_CLRFMT)
	{
	    if (st_p->htmlhead)
		fgw_tssdrop(&st_p->htmlhead);
	    if (st_p->formats)
		rqf_fmtclear(st_p->formats);
	    if (st_p->headers)
		rqf_fmtclear(st_p->headers);
	    st_p->formats=NULL;
	    st_p->headers=NULL;
	    st_p->options&=~(SO_NOFMT|SO_DONEFMT);
	    st_p->fmt_type=0;
	}
	if (st_p->options & SO_CLRDST)
	    rxx_droppcode(&st_p->intovars);
	if (st_p->options & SO_CLRAGG)
	    rxx_droppcode(&st_p->aggregates);
	if (st_p->options & SO_CLRVAR)
	    rxx_droppcode(&st_p->usingvars);
    }
}

/*
** free all connections and statements of a give signature
**
** a negative signature means clean all statements and connections
** (necessary for forked children), while a zero signature just closes
** all connections (sufficient for executing a new image)
*/
void rqx_zapstmt(int signature)
{
    fgw_stmttype *st_p, *st_f;
    fgw_conntype *cn_p, *cn_f;
    fgw_sourcetype *src;
    ca_t ca;
    int i;

/*
** zap statements
*/
    st_p=(fgw_stmttype *) fgw_listnext(&stlist, NULL);
    if (signature)
	while (st_p)
	{
	    st_f=st_p;
	    st_p=(fgw_stmttype *) fgw_listnext(&stlist, (fgw_listentry *) st_p);
	    if (signature<0 || st_f->st_entry.signature==signature)
	    {
		(st_f->source->sqd_freestatement)(st_f, (signature>0));
		if (st_f->formats)
		    rqf_fmtclear(st_f->formats);
		if (st_f->headers)
		    rqf_fmtclear(st_f->headers);
		if (st_f->htmlhead)
		    fgw_tssdrop(&st_f->htmlhead);
		rxx_droppcode(&st_p->intovars);
		rxx_droppcode(&st_p->aggregates);
		rxx_droppcode(&st_p->usingvars);
		fgw_tssdrop(&st_p->usinghead);
		fgw_stackfree(&st_p->stack, NULL);

/* FIXME: may want to just have the statement return RC_NOTFOUND */
		if (signature<0)
		    st_f->curstate=ST_UNINITIALIZED;
		else
		    fgw_dropentry((fgw_listentry *) st_f, &stlist);
	    }
	}
/*
** zap connections
*/
    cn_p=(fgw_conntype *) fgw_listnext(&connlist, NULL);
    while (cn_p)
    {
	cn_f=cn_p;
	cn_p=(fgw_conntype *) fgw_listnext(&connlist, (fgw_listentry *) cn_p);
	if (cn_f->cn_entry.signature==signature || signature<=0)
	{
	    src=cn_f->source;
	    if (cn_f==src->currcon)
	    {
		src->currcon=NULL;
		src->lastcon=(fgw_conntype *) -1;
	    }
	    sqd_break=src->sqd_break;
	    (src->sqd_disconnect)(((signature > 0)? cn_f->host: (void *) -1),
				  cn_f->cn_entry.char_id, &ca,
				  cn_f->private_area);
	    sqd_break=NULL;
	    cn_f->state=0;
	    if (!cn_f->refcnt)
	    {
		if (cn_f->host && cn_f->cn_entry.char_id!=cn_f->host)
			fgw_tssditch(cn_f->host);
		fgw_tssditch(cn_f->cn_entry.char_id);
		fgw_dropentry((fgw_listentry *) cn_f, &connlist);
	    }
	}
    }
}

/*
** set cursor name
** needs to be called after rqx_setstmtcon
*/
void rqx_setcursor(fgw_stmttype *st_p, char *curs)
{
    sqd_break=st_p->source->sqd_break;
    (st_p->source->sqd_setcursor)(st_p, curs);
    sqd_break=NULL;
}
 
/*
**  execute a statement
*/
void rqx_run(char *i_query, fgw_stmttype *st_p, int touch)
{
    if (st_p->curstate==ST_UNINITIALIZED)
    {
	rqx_prepare(st_p, i_query);
	if (st_p->ca->sqlcode)
	   return;
    }
    if (st_p->curstate==ST_EXECUTABLE)
    {
	if ((touch & K_noconf) || sqsl_asktouch(touch))
	{
	    rqx_immediate(st_p);
	    if (!st_p->ca->sqlcode)
	    {
		st_p->ca->sqlcode=RC_NROWS;
		return;
	    }
	}
	else
	    st_p->ca->sqlcode=RC_UINTR;
    }
}

/*
**  prepare statement
*/
void rqx_prepare(fgw_stmttype *st_p, char *query)
{
    fgw_sourcetype *src=st_p->source;

    if (src->lastcon!=st_p->con)
    {
	sqd_break=src->sqd_break;
	(src->sqd_setconnection)(st_p->con? st_p->con->host: NULL,
				 st_p->con? st_p->con->cn_entry.char_id: NULL,
				 st_p->ca);
	sqd_break=NULL;
	if (!(st_p->ca->sqlcode=fromsqlcode(st_p->ca->sqlcode,
					    src->sr_serial.int_id)))
	    src->lastcon=src->currcon;
	else
	    return;
    }
    memset(st_p->ca, 0, sizeof(ca_t));
    sqd_break=src->sqd_break;
    if (query)
	(src->sqd_prepare)(st_p, query);
    else if (st_p)
	st_p->ca->sqlcode=RC_INSID;
    sqd_break=NULL;
    st_p->ca->sqlcode=fromsqlcode(st_p->ca->sqlcode, src->sr_serial.int_id);
}

/*
** executes USING pcode
*/
static int rqx_processusing(fgw_stmttype *st_p, fgw_sourcetype *src, int bind)
{
    int rc, i;
    exprstack_t *e;

    if (!bind)
	return 0;
    if (st_p->usingvars.pcodehead)
    {
	st_p->ca->sqlcode=0;
        if (rc=rxx_execute(&st_p->usingvars, st_p))
            return rc;
	sqd_break=src->sqd_break;
	(src->sqd_allocateda)(st_p, st_p->stack.count);
	sqd_break=NULL;
	if (st_p->ca->sqlcode)
	    goto bad_exit;
	st_p->options|=SO_ALLOCD;
	for (i=0, e=(exprstack_t *) st_p->stack.stack;
	     i<st_p->stack.count; i++, e++)
	{
	    sqd_break=src->sqd_break;
	    (src->sqd_bindda)(st_p, i, e->type, e->length,
			      (e->type==CDOUBLETYPE || e->type==CINTTYPE ||
			       e->type==CDATETYPE)? (char *) &e->val.real:
			       e->val.string);
	    sqd_break=NULL;
	    if (st_p->ca->sqlcode)
		goto bad_exit;
	}
	st_p->options|=SO_BOUND;
	
	/* FIXME: we are relying on the stack not being used between here
	** and the open or execute
	*/
	fgw_stackdrop(&st_p->stack, st_p->stack.count);
    }
    else if (src->sqd_allocateda)
    {
	sqd_break=src->sqd_break;
	(src->sqd_allocateda)(st_p, 0);
	sqd_break=NULL;
	st_p->options&=~SO_BOUND;
    }
    return 0;
bad_exit:
    fgw_stackdrop(&st_p->stack, st_p->stack.count);
    return fromsqlcode(st_p->ca->sqlcode, src->sr_serial.int_id);
}

/*
**  execute non select statement
*/
void rqx_immediate(fgw_stmttype *st_p)
{
    fgw_sourcetype *src=st_p->source;
    int i, rc;

    if (src->lastcon!=st_p->con)
    {
	sqd_break=src->sqd_break;
	(src->sqd_setconnection)(st_p->con? st_p->con->host: NULL,
				 st_p->con? st_p->con->cn_entry.char_id: NULL,
				 st_p->ca);
	sqd_break=NULL;
	if (st_p->ca->sqlcode)
	    goto badexit;
	src->lastcon=st_p->con;
    }
    if (st_p->ca->sqlcode=rqx_processusing(st_p, src, 1))
	return;
    sqd_break=src->sqd_break;
    (src->sqd_immediate)(st_p);
    sqd_break=NULL;
/* FIXME: need to remove using expr from stack */
badexit:
    st_p->ca->sqlcode=fromsqlcode(st_p->ca->sqlcode, src->sr_serial.int_id);
}

/*
** moves data from stream to stream
*/
void rqx_move(stmtstack_t *ss_1, stmtstack_t *ss_2, errorinfo_t *ei)
{
    fgw_stmttype *st_1=ss_1->stmt, *st_2=ss_2->stmt;
    fgw_sourcetype *src1=st_1->source,
		   *src2=st_2->source;
    exprstack_t *x;
    int i, colcount, rc, nrows=0;

/*
** set connection for stmt 1, bind using variables
*/
    if (src1->lastcon!=st_1->con)
    {
	sqd_break=src1->sqd_break;
	(src1->sqd_setconnection)(st_1->con? st_1->con->host: NULL,
				  st_1->con? st_1->con->cn_entry.char_id: NULL,
				  st_1->ca);
	sqd_break=NULL;
	if (st_1->ca->sqlcode)
	    goto badexit1;
	src1->lastcon=st_1->con;
    }
    if (st_1->ca->sqlcode=rqx_processusing(st_1, src1, 1))
	return;
/*
** now process all rows
*/
    for (;;)
    {
/*
** check for interrupts
*/
	if (int_flag)
	    return;
/*
** get from source stream
*/
	if (src1->lastcon!=st_1->con)
	{
	    sqd_break=src1->sqd_break;
	    (src1->sqd_setconnection)(st_1->con? st_1->con: NULL,
				      st_1->con? st_1->con->cn_entry.char_id:
				      NULL, st_1->ca);
	    sqd_break=NULL;
	    if (st_1->ca->sqlcode)
		goto badexit1;
	    src1->lastcon=st_1->con;
	}

	for (;;)
	{
	    sqd_break=src1->sqd_break;
	    (src1->sqd_nextrow)(st_1);
	    sqd_break=NULL;

/* FIXME: cleanup expression stack after USING */
/*
** that's our exit point
*/
	    if (st_1->ca->sqlcode==RC_NOTFOUND)
	    {
		st_1->ca->sqlcode=0;		/* order is important */
		st_2->ca->sqlcode=RC_NROWS;
		st_2->ca->sqlerrd2=nrows;
		return;
	    }
	    else if (st_1->ca->sqlcode)
		goto badexit1;
/*
** process aggregates
*/
	    if (st_1->aggregates.pcodehead)
	    {
		st_1->ca->sqlcode=rxx_execute(&st_1->aggregates, st_1);
		fgw_stackdrop(&st_1->stack, st_1->stack.count);
		if (!st_1->ca->sqlcode)
		    break;
		else if (st_1->ca->sqlcode!=RC_CONTINUE)
		    return;
	    }
	    else
		break;
	}
	if (src2->lastcon!=st_2->con)
	{
	    sqd_break=src2->sqd_break;
	    (src2->sqd_setconnection)(st_2->con? st_2->con->host: NULL, 
				      st_2->con? st_1->con->cn_entry.char_id:
				      NULL, st_2->ca);
	    sqd_break=NULL;
	    if (st_2->ca->sqlcode)
		goto badexit2;
	    src2->lastcon=st_2->con;
	}
/*
** bind on target stream if needed, and execute
*/
	if (st_2->usingvars.pcodehead)
	{
	    if (st_2->ca->sqlcode=rqx_processusing(st_2, src2, 1))
        	goto badexit2;
	}
	else
	{
	    sqd_break=src1->sqd_break;
	    colcount=(src1->sqd_counttokens)(st_1);
	    sqd_break=src2->sqd_break;
	    (src2->sqd_allocateda)(st_2, colcount);
	    sqd_break=NULL;
	    if (st_2->ca->sqlcode)
		goto badexit2;
	    st_2->options|=SO_ALLOCD;
	    if (colcount!=(src1->sqd_counttokens)(st_1))
	    {
		st_2->ca->sqlcode=RC_BTCNT;
		goto badexit2;
	    }
	    for (i=0; i<colcount; i++)
	    {
		x=rqx_nexttoken(i, st_1);
		sqd_break=src2->sqd_break;
		(src2->sqd_bindda)(st_2, i, x->type, x->length,
				   (x->type==CINTTYPE ||
				   x->type==CDATETYPE ||
				   x->type==CDOUBLETYPE)?
				   (char *) &x->val.intvalue:
				   (char *) x->val.string);
		sqd_break=NULL;
		if (st_2->ca->sqlcode)
		    goto badexit2;
	    }
	    st_2->options|=SO_BOUND;
	}
	sqd_break=src2->sqd_break;
	(src2->sqd_immediate)(st_2);
/*
** this is unreliable if using SO_INSCUR: the PUT statement may not set
** sqlerrd2 at all until the CLOSE cursor, hence we do it the dirty way
**
	nrows+=st_2->ca->sqlerrd2; */
	sqd_break=NULL;
	if (st_2->ca->sqlcode)
	    goto badexit2;
	nrows++;
    }
/*
** one ought to be enough, but for completeness...
*/
badexit1:
    st_1->ca->sqlcode=fromsqlcode(st_1->ca->sqlcode, src1->sr_serial.int_id);
    return;
badexit2:
/*
** report the error at the line of the target
*/
    if (ei)
	ei->line_count=ss_2->line_count;
    st_2->ca->sqlcode=fromsqlcode(st_2->ca->sqlcode, src2->sr_serial.int_id);
    return;
}

/*
**  loads next row
*/
void rqx_nextrow(fgw_stmttype *st_p)
{
    fgw_sourcetype *src=st_p->source;
    int i, rc;

    if (src->lastcon!=st_p->con)
    {
	sqd_break=src->sqd_break;
	(src->sqd_setconnection)(st_p->con? st_p->con->host: NULL,
				 st_p->con? st_p->con->cn_entry.char_id: NULL,
				 st_p->ca);
	sqd_break=NULL;
	if (st_p->ca->sqlcode)
	    goto badexit;
	src->lastcon=st_p->con;
    }
    if (st_p->columns)
    {
	fgw_tssdrop(&st_p->columnhead);
	free(st_p->columns);
	st_p->columns=NULL;
    }
    if (st_p->ca->sqlcode=rqx_processusing(st_p, src,
			(st_p->curstate==ST_DECLARED)))
	return;
    for (;;)
    {
	sqd_break=src->sqd_break;
	(src->sqd_nextrow)(st_p);
	sqd_break=NULL;

/* FIXME: clean up stack after USING here */
	if (st_p->ca->sqlcode)
	    goto badexit;
/*
** process aggregates
*/
	if (st_p->aggregates.pcodehead)
	{
	    st_p->ca->sqlcode=rxx_execute(&st_p->aggregates, st_p);
	    fgw_stackdrop(&st_p->stack, st_p->stack.count);
	    if (!st_p->ca->sqlcode)
		break;
	    else if (st_p->ca->sqlcode!=RC_CONTINUE)
		return;
	}
	else
	    break;
    }
/*
** stores values into vars, if needs be the case
** FIXME - throw an error here if no format, multiple rows, same target?
*/
    if (st_p->intovars.pcodehead &&
        (st_p->ca->sqlcode=rxx_execute(&st_p->intovars, st_p)))
    {
	fgw_stackdrop(&st_p->stack, st_p->stack.count);
        return;
    }
badexit:
    st_p->ca->sqlcode=fromsqlcode(st_p->ca->sqlcode, src->sr_serial.int_id);
}

/*
** returns next field in buffer
*/
exprstack_t *rqx_nexttoken(int field, fgw_stmttype *st_p)
{
    fgw_sourcetype *src=st_p->source;
    exprstack_t *e;

    if (st_p->columns && field<rqx_counttokens(st_p) &&
	((e=((st_p->columns)+field))->flags & EF_SUBSTITUTE))
	return e;
    sqd_break=src->sqd_break;
    e=(src->sqd_nexttoken)(field, st_p);
    sqd_break=NULL;
    st_p->ca->sqlcode=fromsqlcode(st_p->ca->sqlcode, src->sr_serial.int_id);
    if (e && e->length<0)
	if (e->type==CINTTYPE || e->type==CDOUBLETYPE)
	    rxu_setnull(e, e->type);
	else
	    return &nullexpr;
    return e;
}

/*
** returns  column name / display label,  column type & size
*/
void rqx_describecol(fgw_stmttype *st_p, int f, exprstack_t *e)
{
    fgw_sourcetype *src=st_p->source;

    if (src && src->sqd_describecol)
	(src->sqd_describecol)(st_p, f, e);
    else
	memset(e, 0, sizeof(exprstack_t));
}

/*
** returns number of columns in select list
*/
int rqx_counttokens(fgw_stmttype *st_p)
{
    fgw_sourcetype *src=st_p->source;

    if (src)
	return (src->sqd_counttokens)(st_p);
    else
	return 0;
}

/*
** returns error message string from source
*/
void rqx_errmsg(int s, int e, char *b, int l)
{
    fgw_listentry le, *lf;
    fgw_sourcetype *src;
    int tl;

    le.int_id=s;
    le.signature=0;
    if ((lf=fgw_listsearch(&le, &srcser)) &&
	(src=(fgw_sourcetype *) (lf-1),		/* FIXME a hack */
	src->sqd_errmsg))
    {
	sprintf(b, "[%s] ", src->sr_entry.char_id);
	tl=strlen(b);
	sqd_break=src->sqd_break;
	(src->sqd_errmsg)(e, b+tl, l-tl);
	 sqd_break=NULL;
    }
    else
	sprintf(b, "Unknown source %i, error %i", s, e);
}

#define LRBINC	1024

/*
** maintain last row buffer for reject file
*/
void rqx_rejcopy(fgw_stmttype *st, char *source, int len)
{
    char *newbuf;

    if (!st->reject)
	return;
    if (st->lastrowsize-st->lastrowlen<=len)
    {
	if (newbuf=(char *) realloc(st->lastrow, st->lastrowsize+LRBINC))
	{
	    st->lastrow=newbuf;
	    st->lastrowsize+=LRBINC;
	}
    }
    fgw_move(st->lastrow+st->lastrowlen, source, len);
    st->lastrowlen+=len;
    *(st->lastrow+st->lastrowlen)='\0';
}

void rqx_rejdump(fgw_stmttype *st)
{
    fgw_fdwrite(st->reject, st->lastrow, st->lastrowlen);
    st->oserr->sqlcode=errno;
    status=0;
}
