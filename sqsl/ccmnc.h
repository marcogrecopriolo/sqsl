/*
	CCMNC.h  -  Functions used by C functions only, definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Oct 99
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

#ifndef CCMNC_H

#include <errno.h>
#include <string.h>
#include <ctype.h>

/*
** we do not require ordered access, but need to locate single list elements
** quickly. to this end, we use a hierarchical hash structure: a tree of 
** hash buckets of decreasing sizes. the cost of walking the tree is always
** FGW_LISTHASHSZ comparisons at most (unless all the keys share the same
** value, that is), independently of the tree depth, and since there's no
** sorting or rebalancing to do, tree maintenance is fairly cheap.
*/
#define FGW_LISTHASHSZ 10

typedef struct listhash
{
    int size;
    struct listhash *up;
    int pos;
    struct
    {
	struct listhash *down;
	int count;
    } hash[FGW_LISTHASHSZ];
} fgw_listhash;

typedef struct fgw_list
{
    char *char_id;
    int int_id;
    unsigned int hash;
    int signature;
    struct fgw_list *next,
		    *prev,
		    *hashnext,
		    *hashprev;
    fgw_listhash *up;
} fgw_listentry;

typedef struct
{
    int base;
    int notfound;
/* FIXME: MT have to place a mutex here */
    int (*cmpr)();
    unsigned int (*hash)();
    fgw_listhash root;
    fgw_listentry *head;
} fgw_listtype;

extern int fgw_intcomp(fgw_listentry *e1, fgw_listentry *e2);
extern int fgw_stringcomp(fgw_listentry *e1, fgw_listentry *e2);
extern unsigned int fgw_inthash(fgw_listentry *e);
extern unsigned int fgw_stringhash(fgw_listentry *e);

#define FGW_LISTSERIAL(arg1, arg2) fgw_listtype arg1 = {		\
   1,									\
   arg2,								\
   fgw_intcomp,								\
   fgw_inthash,								\
   FGW_LISTHASHSZ							\
}

#define FGW_LISTINT(arg1, arg2) fgw_listtype arg1 = {			\
   0,									\
   arg2,								\
   fgw_intcomp,								\
   fgw_inthash,								\
   FGW_LISTHASHSZ							\
}

#define FGW_LISTCHAR(arg1, arg2) fgw_listtype arg1 = {			\
   0,									\
   arg2,								\
   fgw_stringcomp,							\
   fgw_stringhash,							\
   FGW_LISTHASHSZ							\
}

#define FGW_LISTRAW(arg1, arg2, arg3, arg4) fgw_listtype arg1 = {	\
   0,									\
   arg2,								\
   arg3,								\
   arg4,								\
   FGW_LISTHASHSZ							\
}

typedef struct fgw_stack
{
    int len;
    int count;
    int size;
    char *stack;
} fgw_stacktype;

typedef struct fgw_tss
{
    struct fgw_tss *next;
    struct fgw_tss *prev;
    long size;			/* in aid to SIMULATED_FORK,
				   long for alignment on 64bits */
    void (*func)();
    char string;
} fgw_tsstype;

#define FGW_PTR2TSS(p) \
	((fgw_tsstype *) ((char *) p- \
		(((char *) ((&((fgw_tsstype *) p)->string)-(char *) p)))))

#if defined (I4GLVER) || defined(A4GL)
extern char *fgw_popquote();
#endif
extern fgw_listentry *fgw_listsearch(fgw_listentry *e, fgw_listtype *list);
extern fgw_listentry *fgw_listnext(fgw_listtype *list, fgw_listentry *el);
extern void fgw_newentry(fgw_listentry *entry, fgw_listtype *list);
extern void fgw_dropentry(fgw_listentry *entry, fgw_listtype *list);
extern void fgw_stackinit(fgw_stacktype *stack, int size);
extern void fgw_stackfree(fgw_stacktype *stack, void (*clear)());

extern void fgw_stackpush(fgw_stacktype *stack, void *element);
extern char *fgw_stackpop(fgw_stacktype *stack);
extern char *fgw_stackpeek(fgw_stacktype *stack);
extern void fgw_stackdrop(fgw_stacktype *stack, int n);

extern void fgw_tssattach(fgw_tsstype **head, char *element);
extern void fgw_tssdetach(fgw_tsstype **head, char *element);
extern char *fgw_tssmallocx(fgw_tsstype **head, int size, void (*func)());
extern char *fgw_tssrealloc(fgw_tsstype **head, char *element, int size);
extern void fgw_tssfree(fgw_tsstype **head, char *element);
extern void fgw_tssdrop(fgw_tsstype **head);

extern int fgw_hash(char *s, long b);

extern void fgw_clip(char *c);
extern int fgw_pos(char *s1, char *s2);
extern void fgw_move(void *d, void *s, int c);
extern void fgw_escmove(void *d, void *s, int c, char e);

#define fgw_tssmalloc(head, size) fgw_tssmallocx(head, size, NULL)

/* Placeholder for GLS aware functions */
#define fgw_strcmp		strcmp
#define fgw_strncmp		strncmp
#define fgw_strchr(p, c)	strchr(p, *(c))
#define fgw_strstr		strstr
#define fgw_strlen		strlen
#define fgw_chrnext(arg)	arg++
#define fgw_toupper		toupper
#define fgw_tolower		tolower
#define fgw_isupper		isupper
#define fgw_islower		islower
#define fgw_isalpha		isalpha
extern int fgw_strcasecmp(char *s1, char *s2);
extern int fgw_strncasecmp(char *s1, char *s2, int l);
extern char *fgw_strcasestr(char *s1, char *s2);

/* these are returned via status, errno returns OS Exxxx errors */
#define FGW_EINVAL	EINVAL	/* invalid parameter */
#define FGW_ENOMEM	-1319	/* out of memory */
#define FGW_EBUSY	-107	/* item is locked */
#define CCMNC_H
#endif
