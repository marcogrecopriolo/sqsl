/*
	CRQXD.h  -  SQSL interpreter dynamic query execution definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2020 Marco Greco (marco@4glworks.com)

	Initial release: Jan 97
	Current release: Jun 20

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

#ifndef CRQXD_H
#include "ccmnc.h"
#include "cfioc.h"
#include "crxuc.h"
#include "crxxd.h"
#include "ctypc.h"

typedef struct
{
    char *entry;	/* pointer to entry string */
    int etype;		/* type of entry */
    int esize;		/* size of entry, strings & headers */
} fgw_fmtentry;

typedef struct
{
    int size;			/* max no of entries */
    int entries;		/* cur no of entries */
    fgw_fmtentry fmt[1];
} fgw_fmttype;

typedef struct			/* sqlca data holders */
{
    unsigned int sqlcode;
    int sqlerrd1;
    int sqlerrd2;
} ca_t;

typedef struct
{
    fgw_listentry ob_entry;
    int src_id;
    int (*obj)();
} fgw_objtype;
   
typedef struct fgw_source
{
/* FIXME: these are expected to be the first two elements of
   this structure, and in this order */
    fgw_listentry sr_entry;		/* name list management */
    fgw_listentry sr_serial;		/* serial list management */
    int flags;				/* where would we be without them? */
    void *handle;			/* dlopen handle */
    fgw_listtype object;		/* known external functions */
    struct fgw_conn *currcon;		/* currect connection */
    struct fgw_conn *lastcon;		/* last connection */
    void (*sqd_connect)();		/* the real stuff! */
    void (*sqd_setconnection)();
    void (*sqd_disconnect)();
    char *(*sqd_newstatement)();
    void (*sqd_freestatement)();
    void (*sqd_closestatement)();
    void (*sqd_setcursor)();
    void (*sqd_prepare)();
    void (*sqd_allocateda)();
    void (*sqd_bindda)();
    void (*sqd_immediate)();
    void (*sqd_nextrow)();
    exprstack_t *(*sqd_nexttoken)();
    int (*sqd_counttokens)();
    void (*sqd_errmsg)();
    void (*sqd_break)();
    void (*sqd_describecol)();
    void (*sqd_autocommit)();
    int (*sqd_error)();
} fgw_sourcetype;

#define SO_SRCFLG 0x1
#define SO_XPRFLG 0x2
#define SO_EXTFLG 0x4
#define SO_MASK	0x4d470000

typedef struct fgw_conn
{
    fgw_listentry cn_entry;	/* list management */
    fgw_sourcetype *source;	/* sql source in use */
    char *host;			/* host name - in aid of those sources who don't
				   handle connection synonyms */
    int refcnt;			/* statements using this connection */
    int state;			/* true if connected */
    char *private_area;		/* source specific connection stuff */
} fgw_conntype;

typedef struct fgw_stmt
{
    fgw_listentry st_entry;	/* list management */
    int refcnt;			/* prepared statements & foreach... */
    int fmt_type,		/* type of format */
	headwidth,		/* max header width */
	width;			/* format width */
    fgw_fmttype *headers,	/* headers list */
		*formats;	/* formats list */
    char del[2],
	 esc[2];		/* for delimited formats */
    char *pretable,		/* various (html) strings */
	 *posttable,
	 *prerow,
	 *postrow,
	 *preheader,
	 *postheader,
	 *prefield,
	 *postfield;
    fgw_tsstype *htmlhead;	/* dynamic html strings */ 
    int countfields;		/* HTML column count flag */
    int field;			/* field count (needed for HTML field count
				   & vertical & brief formats displays) */
    int format;			/* format count (displays) */

    pcode_t usingvars;		/* USING expression list */
    pcode_t aggregates;		/* aggregates assignment list */
    pcode_t intovars;		/* destination variables list */
    fgw_stacktype stack;	/* expression stack for the above */
    exprstack_t *columns;	/* storage for column assignments */
    fgw_tsstype *columnhead;	/* TSS for column assignments */ 
    fgw_tsstype *usinghead;	/* TSS for using expressions */
    fgw_loc_t *vars;		/* storage space */
    ca_t *ca;			/* sqlca info */
    char *sqltxt;		/* for those sources that can't return error strings asynchronously */
    ca_t *oserr;		/* for file system failures */
    fgw_conntype *con;		/* connection identifier */
    fgw_sourcetype *source;	/* source identifier */
    int phcount;		/* placeholder count */
    int curstate;		/* state: see ST_... defines below */
    int options;		/* statement options: see SO... defines below */
    int autocommit;		/* avoid long tx's */
    int blocksize;		/* IO tuning */

    fgw_fdesc *stream;		/* input stream */
    fgw_fdesc *reject;		/* reject stream */
    char *lastrow;		/* copy of last read row for reject
				   purposes */
    int lastrowsize;		/* lastrow buffer size */
    int lastrowlen;		/* current used part of lastrow buffer */

    void *sqlstmt;		/* pointer to private dynamic sql data */
} fgw_stmttype;

#define SS_INTERNAL		0x4000000	/* flags prepared statements
						   opened streams */
#define SS_ALLCON		0		/* zap connections */
#define SS_ALL			-1		/* zap connections and
						   statements */

#define ST_UNINITIALIZED	0x0
#define ST_EXECUTABLE		0x1
#define ST_DECLARED		0x2
#define ST_NEWLYOPENED		0x3
#define ST_OPENED		0x4


#define SO_WITHHOLD		0x00000001
#define SO_ALLOCBYTE		0x00000002
#define SO_SQLONLY		0x00000004
#define SO_INSCURS		0x00000008
#define SO_REALPREPARED		0x00000010
#define SO_NOTEXT		0x00000020	/* prepare or input streams */

#define SO_CLRFMT		0x00000100	/* prepared statements need */
#define SO_CLRDST		0x00000200	/* some clause cleaning */
#define SO_CLRVAR		0x00000400
#define SO_CLRAGG		0x00000800

#define SO_DONEFMT		0x00001000	/* format states */
#define SO_NOFMT		0x00002000
#define SO_TABULAR		0x00004000

#define SO_LATE_DESCRIBE	0x00008000	/* describe is only available after fetching a row */
#define SO_PRIMED		0x00010000	/* cursor opened, describe available, skip fetching row */

#define SO_ALLOCD		0x10000000	/* placehoders allocated */
#define SO_BOUND		0x20000000	/* placeholders bound */
#define SO_BUFCNV		0x40000000	/* target statement needs conversion */
#define SO_STATICBUF		0x80000000	/* source statement has static buffer */

#define FGW_MAXDLL		0xfc
/*
** sqlca.sqlcode macros
** we preserve small +ve & -ve #'s generated by 4gl & stuff like
** cfioc/cascc/chstc
** 0xfe###### are reserved for the whole of the interpreter bar the sql layer
** 0x01###### to 0xfd###### are left for the sql layer
*/
#define SQ_TOSQLCODE(arg) \
	(((arg) & 0x800000)? (arg) | 0xff000000: (arg) & 0xffffff)
#define SQ_TOSRC(arg) (((arg)>>24) & 0xff)
#define SQ_FROMSQLCODE(arg1, arg2) \
	(((arg1) & 0xffffff)|(((arg2) & 0xff)<<24))
#define SQ_ISPARSER(arg) ((arg)==0xfd || (arg)==0xfe)
#define SQ_ISSYNTAX(arg) ((arg)==0xfd)
#define SQ_ISAPI(arg) ((arg)==0 || (arg)==0xff)

#define CRQXD_H
#endif
