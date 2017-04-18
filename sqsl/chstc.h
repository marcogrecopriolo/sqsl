/*
	CHSTC.h  -  Hashes, definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Dec 99
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

/*
** the locator structure is limited to 2G blobs - hence the ints.
** not that this will be a problem before 2010...
*/

#ifndef CHSTC_H
#include "ctypc.h"

typedef int hst_t;

typedef struct
{
    hst_t	buckets;	/* # of buckets */
    hst_t	entries;	/* # of entries in this node */
    int		flags;		/* for now only reorg flag */
    hst_t	bucket[1];	/* array of buckets */
} fgw_hstnode;

typedef struct h_entry_s
{
    hst_t	magic;		/* our own offset */
    hst_t	node;		/* node id */
    hst_t	next;		/* next entry */
    hst_t	size;		/* total size of entry */
    short	type;		/* entry type (see sqltypes.h & values below) */
    short	refcnt;		/* if latched, entry can't be moved */
    hst_t	len;		/* used only for unkown types */
    hst_t	def;		/* offset to definition string */
    char	entry;		/* needs to be hst_t aligned */
} fgw_hstentry;

typedef struct
{
    int		magic;		/* just to make sure it's our type of blobs */
    int		entries;	/* # of entries */
    hst_t	root;		/* offset to buckets entry */
    hst_t	freeptr;	/* free space handling */
    hst_t	freecnt;
    int		refcnt;		/* # of of latched entries */
} fgw_hsthead;

#define T_FREE -1
#define T_HASH -2
#define T_FWD -3

#define H_REORG	0x1

#define STARTHASHSIZE 0x8000
#define HASHKEY 0x04d47000	/* builds up my ego! */
#define fgw_hashhead(arg1) ((fgw_hsthead *) ((arg1)->loc_buffer))
#define fgw_hashentry(arg1, arg2) ((fgw_hstentry *) (((arg1)->loc_buffer)+(arg2)))
#define fgw_defentry(arg1) ((char *) &((arg1)->entry)+((arg1)->def))

extern int fgw_testhash(fgw_loc_t *bytevar);
extern void fgw_locatehash(fgw_loc_t *bytevar, int buckets);

extern fgw_hstentry *fgw_hstget(fgw_loc_t *bytevar, hst_t node, char *def, int *b);
extern fgw_hstentry *fgw_hstidxget(fgw_loc_t *bytevar, hst_t leaf);

extern void fgw_hstidxadd(fgw_loc_t *bytevar, hst_t leaf, int type, hst_t len,
			  char *entry);
extern void fgw_hstadd(fgw_loc_t *bytevar, hst_t node, char *def,
		      int type, hst_t len, char *entry);
extern int fgw_hstaddnode(fgw_loc_t *bytevar, hst_t node,
			  char *def, int buckets);
extern fgw_hstentry *fgw_hstreserve(fgw_loc_t *bytevar, hst_t node,
				    char *def, hst_t size);

extern void fgw_hstidxdrop(fgw_loc_t *bytevar, hst_t node, int bucket);

extern void fgw_hstlatch(fgw_loc_t *bytevar, hst_t leaf, int op);

extern void fgw_freehash(fgw_loc_t *bytevar);

#define CHSTC_H
#endif
