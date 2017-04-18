/*
	CHSTC.c - Hash table routines

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Apr 99
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

#include "coslc.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "ccmpc.h"
#include "ccmnc.h"
#include "chstc.h"
#include "ctypc.h"

/* align every struct allocated to 16 bytes boundaries */
#define fgw_entrysize(arg) ((arg & 0xf)? ((arg | 0xf)+1): arg);

#define K_hashmin	  8
#define K_hashmax	512
#define K_hashmult	  2
#define K_hashlim	  3

/*
** test if bytevar is a usable blob
*/
int fgw_testhash(fgw_loc_t *bytevar)
{
    return ((bytevar->loc_loctype!=LOCMEMORY) ||
	    (bytevar->loc_type!=SQLBYTES) ||
	    !bytevar->loc_buffer ||
	    (fgw_hashhead(bytevar)->magic!=HASHKEY));
}

/*
** frees space in hash
*/
static void fgw_hstfree(fgw_loc_t *bytevar, int o)
{
    fgw_hstentry *e, *f, *g;
    int *n;

/*
** mark entry as free & adjust free count
*/
    e=fgw_hashentry(bytevar, o);
    e->type=T_FREE;
    e->refcnt=0;
    fgw_hashhead(bytevar)->freecnt+=e->size;
    n=&(fgw_hashhead(bytevar)->freeptr);
/*
** scan all free list entries
*/
    while (*n)
    {
	f=fgw_hashentry(bytevar, *n);
/*
** new free entry is next to previous
*/
	if (*n+f->size==o)
	{
	    f->size+=e->size;
/*
** and together they are next to the next
*/
	    if (*n+f->size==f->next)
	    {
		g=fgw_hashentry(bytevar, f->next);
		f->size+=g->size;
		f->next=g->next;
	    }
	    return;
	}
/*
** new free entry is next to next
*/
	else if (o+e->size==f->next)
	{
	    g=fgw_hashentry(bytevar, f->next);
	    e->next=g->next;
	    e->size+=g->size;
	    f->next=o;
	    return;
	}
/*
** keep entries ordered
*/
	else if (o<*n)
	{
	    e->next=*n;
	    *n=o;
	    return;
	}
	n=&(f->next);
    }
/*
** last entry int the free list
*/
    *n=o;
    e->next=0;
}

/*
** allocates new entry
*/
static int fgw_hstnew(fgw_loc_t *bytevar, hst_t le, hst_t ld)
{
    fgw_hstentry *f, *g;
    hst_t l, o, *p, r, s;
    char *b;

/*
** negative definition length means absolute allocation
*/
    if (ld<0)
	l=le;
    else
	l=fgw_entrysize(ld+le+sizeof(fgw_hstentry)); /* includes term char */
    p=&(fgw_hashhead(bytevar)->freeptr);
/*
** scan all free list entries
*/
    while (*p)
    {
	f=fgw_hashentry(bytevar, *p);
/*
** a greater fit, will have to split it in two
*/
	if (f->size>=l+sizeof(fgw_hstentry))
	{
	    r=*p;
	    *p+=l;
	    g=fgw_hashentry(bytevar, *p);
	    g->type=T_FREE;
	    g->refcnt=0;
	    g->size=f->size-l;
	    g->next=f->next;
	    f->size=l;
	    f->next=0;
	    fgw_hashentry(bytevar, r)->magic=r;	/* mark it a good entry */
	    return r;
	}
/*
** an almost exact fit
*/
	else if (f->size>=l)
	{
	    r=*p;
	    *p=f->next;
	    fgw_hashhead(bytevar)->freecnt-=l;
	    fgw_hashentry(bytevar, r)->magic=r;		/* mark it a good entry */
	    f->next=0;
	    return r;
	}
	p=&(f->next);
    }
/*
** no free space found, reallocate byte buffer
*/
   o=(int) ((char *) p-bytevar->loc_buffer);
   s=(l>STARTHASHSIZE)? l: STARTHASHSIZE;
   if (b=(char *) realloc(bytevar->loc_buffer, bytevar->loc_bufsize+s))
   {
	bytevar->loc_buffer=b;
	p=(int *) b+o;
	r=bytevar->loc_bufsize;
	bytevar->loc_bufsize+=s;
	bytevar->loc_size=bytevar->loc_bufsize;
/*
** if we have reallocated more space than needed, create new free entry
*/
	if (s>l)
	{
	    f=fgw_hashentry(bytevar, r+l);
	    f->next=0;
	    f->type=T_FREE;
	    f->refcnt=0;
	    f->size=s-l;
/*
** and fix last entry's next
*/
	    *p=r+l;
	}
	fgw_hashentry(bytevar, r)->magic=r;		/* mark it a good entry */
	return r;
   }
   return 0;
}

/*
** allocates buckets entry
*/
static hst_t fgw_hstalloc(fgw_loc_t *bytevar, hst_t node, char *def, int buckets)
{
    hst_t o, l;
    fgw_hstentry *e;
    fgw_hstnode *n;
    int i;
    char c;

    l=sizeof(fgw_hstnode)+sizeof(hst_t)*(buckets-1);
    if (!(o=fgw_hstnew(bytevar, l, fgw_strlen(def))))
	return 0;
    e=fgw_hashentry(bytevar, o);
    e->def=l;
    e->node=node;
    e->type=T_HASH;
    fgw_move(&(e->entry)+l, def, fgw_strlen(def)+1);
    n=(fgw_hstnode *) &e->entry;
    n->entries=0;
    n->buckets=buckets;
    n->flags=0;
    for (i=0; i<buckets; i++)
	n->bucket[i]=0;
    return o;
}

/*
**  free an old blob
*/
void fgw_freehash(fgw_loc_t *bytevar)
{
/*
**  try to avoid memory leaks & segv's :-)
*/
    if (bytevar->loc_loctype==LOCMEMORY &&
	bytevar->loc_bufsize>0 &&
	bytevar->loc_buffer)
    {
	free(bytevar->loc_buffer);
	bytevar->loc_buffer=NULL;
    }
}

/*
**  starts a hash table
*/
void fgw_locatehash(fgw_loc_t *bytevar, int buckets)
{
    int i;
    fgw_hstentry *h;

    status=0;
    if (buckets<K_hashmin || buckets>K_hashmax)
    {
	status=FGW_EINVAL;
	return;
    }
    fgw_freehash(bytevar);
    bytevar->loc_buffer=(char *) malloc(STARTHASHSIZE);
    if (!bytevar->loc_buffer)
    {
	status=FGW_ENOMEM;
	return;
    }
/*
** init blob...
*/
    bytevar->loc_type=SQLBYTES;
    bytevar->loc_loctype=LOCMEMORY;
    bytevar->loc_bufsize=STARTHASHSIZE;
    bytevar->loc_size=bytevar->loc_bufsize;
    bytevar->loc_indicator=0;
/*
** ...and hash head...
*/
    fgw_hashhead(bytevar)->magic=HASHKEY;
    fgw_hashhead(bytevar)->entries=0;
    fgw_hashhead(bytevar)->freecnt=STARTHASHSIZE-sizeof(fgw_hsthead);
    fgw_hashhead(bytevar)->freeptr=sizeof(int)*buckets+sizeof(fgw_hsthead);
/*
** init free list
*/
    h=fgw_hashentry(bytevar, fgw_hashhead(bytevar)->freeptr);
    h->magic=fgw_hashhead(bytevar)->freeptr;
    h->next=0;
    h->size=fgw_hashhead(bytevar)->freecnt;
    h->type=T_FREE;
    h->refcnt=0;
/*
** ...and buckets
*/
    fgw_hashhead(bytevar)->root=fgw_hstalloc(bytevar, 0, "", buckets);
}

/*
** copy nodes from one hash to another
*/
static void copynode_hst(fgw_loc_t *ob, fgw_loc_t *nb, hst_t on, hst_t nn)
{
    fgw_hstnode *node1, *node2;
    fgw_hstentry *h1, *h2, *oh2;
    hst_t ol, nl;
    int i;

    node1=(fgw_hstnode *) &fgw_hashentry(ob, on)->entry;
    node2=(fgw_hstnode *) &fgw_hashentry(nb, nn)->entry;
    for (i=0; i<node1->buckets; i++)
    {
	node2->bucket[i]=0;
	ol=node1->bucket[i];
	oh2=NULL;
	while (ol)
	{
	    h1=fgw_hashentry(ob, ol);
/*
** allocate new entry (lack of space should never happen...)
*/
	    if (!(nl=fgw_hstnew(nb, h1->size, -1)))
	    {
		status=FGW_ENOMEM;
		return;
	    }
/*
** copy info
*/
	    h2=fgw_hashentry(nb, nl);
	    fgw_move(h2, h1, h1->size);
	    h2->magic=nl;
	    h2->node=nn;
	    h2->next=0;
/*
** if we have entries in the chain, attach to the end;
*/
	    if (oh2)
		oh2->next=nl;
	    else
		node2->bucket[i]=nl;
/*
** it's a hash! recurse on...
*/
	    if (h1->type==T_HASH)
	    {
		copynode_hst(ob, nb, ol, nl);
		if (status)
		    return;
	    }
	    oh2=h2;
	    ol=h1->next;
	}
    }
}

/*
** compress hash
*/
static void compress_hst(fgw_loc_t *bytevar)
{
    fgw_loc_t newbyte;
    fgw_hstentry *h;
    fgw_hstnode *n;

    if (bytevar->loc_bufsize==STARTHASHSIZE)
	return;
    newbyte=*bytevar;
    if (!(newbyte.loc_buffer=(char *) malloc(bytevar->loc_bufsize/2)))
    {
	status=FGW_ENOMEM;
	return;
    }
    newbyte.loc_bufsize/=2;
    newbyte.loc_size/=2;
/*
**  initialize hash head...
*/
    n=(fgw_hstnode *) &fgw_hashentry(bytevar, fgw_hashhead(bytevar)->root)->entry;
    fgw_hashhead(&newbyte)->magic=HASHKEY;
    fgw_hashhead(&newbyte)->entries=0;
    fgw_hashhead(&newbyte)->freecnt=STARTHASHSIZE-sizeof(fgw_hsthead);
    fgw_hashhead(&newbyte)->freeptr=sizeof(int)*n->buckets+sizeof(fgw_hsthead);
/*
** ...free list...
*/
    h=fgw_hashentry(&newbyte, fgw_hashhead(&newbyte)->freeptr);
    h->next=0;
    h->size=fgw_hashhead(&newbyte)->freecnt;
    h->type=T_FREE;
    h->refcnt=0;
/*
** ...and buckets
*/
    fgw_hashhead(&newbyte)->root=fgw_hstalloc(&newbyte, 0, "", n->buckets);
/*
** copy all entries in root node and below
*/
    copynode_hst(bytevar, &newbyte,
		fgw_hashhead(bytevar)->root, fgw_hashhead(&newbyte)->root);
/*
** and if all is fine, switch buffers and free the old one
*/
    if (status)
    {
    	free(newbyte.loc_buffer);
/*
** we're not going to pass errors back
*/
	status=0;
    }
    else
    {
	free(bytevar->loc_buffer);
	bytevar->loc_buffer=newbyte.loc_buffer;
	bytevar->loc_bufsize=newbyte.loc_bufsize;
	bytevar->loc_size=newbyte.loc_size;
    }
}

/*
** reorganize hash
*/
static void reorg_hst(fgw_loc_t *bytevar, hst_t node, int buckets)
{
    hst_t newnode;
    fgw_hstentry *e, *oe, *ne;
    fgw_hstnode *on, *nn;
    int b, i;
    hst_t o;

    oe=fgw_hashentry(bytevar, node);
    newnode=fgw_hstalloc(bytevar, oe->node, fgw_defentry(oe), buckets);
/*
** do nothing if you can't get memory
*/
    if (!newnode)
    {
	status=0;
	return;
    }
    on=(fgw_hstnode *) &oe->entry;
/*
** initialize node
*/
    ne=fgw_hashentry(bytevar, newnode);
    ne->next=oe->next;
    nn=(fgw_hstnode *) &ne->entry;
    nn->buckets=buckets;
    nn->entries=on->entries;
    nn->flags=on->flags;
/*
** rehash every entry, detaching it from old node and attaching it to new one
*/
    for (i=0; i<on->buckets; i++)
    {
	o=on->bucket[i];
	while (o)
	{
	    e=fgw_hashentry(bytevar, o);
	    b=fgw_hash(fgw_defentry(e), buckets);
	    e->node=newnode;
	    o=e->next;
	    e->next=nn->bucket[b];
	    nn->bucket[b]=e->magic;
	}
    }
/*
** attach node to parent
*/
    if (node)
    {
	e=fgw_hashentry(bytevar,
		oe->node? oe->node: fgw_hashhead(bytevar)->root);
	on=(fgw_hstnode *) &e->entry;
	b=fgw_hash(fgw_defentry(ne), on->buckets);
	if (on->bucket[b]==node)
	    on->bucket[b]=newnode;
	else
	{
	    e=fgw_hashentry(bytevar, on->bucket[b]);
	    while (e->next && oe->next!=node)
		e=fgw_hashentry(bytevar, oe->next);
	    e->next=newnode;
	}
    }
    else
	fgw_hashhead(bytevar)->root=newnode;
    fgw_hstfree(bytevar, node);
}

/*
** checks validit of hash entry
*/
static int testentry_hst(fgw_loc_t *bytevar, fgw_hstentry *e, hst_t node)
{
    return (!e) ||
	   (e->magic!=node) ||
	   (e->magic==fgw_hashhead(bytevar)->root) ||
	   (e->type<0 && e->type!=T_HASH && e->type!=T_FWD);
}

/*
** retrieve entry
*/
fgw_hstentry *fgw_hstidxget(fgw_loc_t *bytevar, hst_t leaf)
{
    fgw_hstentry *e;

    status=0;
    e=fgw_hashentry(bytevar, leaf);
    if (testentry_hst(bytevar, e, leaf))
    {
	status=FGW_EINVAL;
	return NULL;
    }
    if (e->type!=T_FWD)
	return e;
    return fgw_hashentry(bytevar, *(hst_t *) &e->entry);
}

/*
** retrieve entry
*/
fgw_hstentry *fgw_hstget(fgw_loc_t *bytevar, hst_t node, char *def, int *b)
{
    hst_t o;
    fgw_hstnode *r;
    fgw_hstentry *e, *p=NULL;

    status=0;
    if (fgw_testhash(bytevar))
    {
	status=FGW_EINVAL;
	return NULL;
    }
    if (risnull(CSTRINGTYPE, def))
	*def='\0';
    if (!node)
	node=fgw_hashhead(bytevar)->root;
    e=fgw_hashentry(bytevar, node);
    if (!e || e->magic!=node || e->type!=T_HASH)
    {
	status=FGW_EINVAL;
	return NULL;
    }
    r=(fgw_hstnode *) &e->entry;
    *b=fgw_hash(def, r->buckets);
    o=r->bucket[*b];
/*
** scan bucket chain until entry found
*/
    while (o)
    {
	e=fgw_hashentry(bytevar, o);
	if (!fgw_strcmp(fgw_defentry(e), def))
/*
** found match - move it on top of the bucket chain
*/
	{
	    if (p)
	    {
		p->next=e->next;
		e->next=r->bucket[*b];
		r->bucket[*b]=o;
	    }
	    return e;
	}
	p=e;
        o=e->next;
    }
    return NULL;
}

/*
** modify existing entry
*/
void fgw_hstidxadd(fgw_loc_t *bytevar, hst_t leaf, int type, hst_t len, char *entry)
{
    fgw_hstentry *e, *ne;
    fgw_hstnode *n;
    hst_t f=0, o, l;
    char *def;
    int b, hashassign;

    status=0;
    e=fgw_hashentry(bytevar, leaf);
    if (testentry_hst(bytevar, e, leaf))
    {
	status=FGW_EINVAL;
	return;
    }
/*
** value could be an hash entry, so need to keep track of this if we
** reallocate the hash
*/
    hashassign=(entry>=bytevar->loc_buffer &&
		entry<=bytevar->loc_buffer+bytevar->loc_bufsize)?
	       (int) (entry-bytevar->loc_buffer): 0;
    def=fgw_defentry(e);
    switch (e->type)
    {
      case T_HASH:
/*
** can't replace a hash with a scalar
*/
	status=FGW_EINVAL;
	return;
      case T_FWD:
/*
** forward pointers need a bit more work - just drop old reference
*/
	f=*(hst_t *) &e->entry;
	    
    }
    switch(type)
    {
      case CSTRINGTYPE:
	l=(len>0)? len+1: ((len<0)? 0: fgw_strlen(entry)+1);
	break;
      case CDOUBLETYPE:
	l=sizeof(double);
	break;
      case CINTTYPE:
      case CDATETYPE:
	l=sizeof(int);
	break;
      case CDTIMETYPE:
	l=sizeof(fgw_dtime_t);
	break;
      case CINVTYPE:
	l=sizeof(fgw_intrvl_t);
	break;
      case CDECIMALTYPE:
      case CMONEYTYPE:
	l=sizeof(fgw_dec_t);
	break;
      default:
	if (type<0 || len<-1)
	{
	    status=FGW_EINVAL;
	    return;
	}
	l=len>0? len: 0;
    }
/*
** new value fits (leave entry untouched, makes further changes faster)
*/
    if (e->def>l)
    {
	e->type=type;
	if (len==-1)
	    e->len=-1;
	else
	{
	    e->len=l;
	    fgw_move(&(e->entry), entry, l);
	}
    }
/*
** does not fit, latched, so we will have to use a forward pointer
*/
    else if (e->refcnt)
    {
	if (o=fgw_hstnew(bytevar, l, fgw_strlen(def)))
	{
	     ne=fgw_hashentry(bytevar, o);
	     ne->type=type;
	     ne->def=l;
	     ne->len=len==-1? len: l;
	     fgw_move(&(ne->entry),
			hashassign? bytevar->loc_buffer+hashassign: entry, l);
	     fgw_move(&(ne->entry)+l, "", 1);
	     e->type=T_FWD;
	     fgw_move(&(e->entry), &o, sizeof(hst_t));
	}
	else
	{
	    status=FGW_ENOMEM;
	    return;
	}
    }
/*
** does not fit, hence free old entry & allocate new
*/
    else if (o=fgw_hstnew(bytevar, l, fgw_strlen(def)))
    {
	ne=fgw_hashentry(bytevar, o);
/*
** construct new entry
*/
	ne->type=type;
	ne->def=l;
	ne->len=len==-1? len: l;
	fgw_move(&(ne->entry),
		 hashassign? bytevar->loc_buffer+hashassign: entry, l);
	fgw_move(&(ne->entry)+l, def, fgw_strlen(def)+1);
	ne->node=e->node;
	n=(fgw_hstnode *) &fgw_hashentry(bytevar, e->node)->entry;
/*
** get rid of old entry
*/
	b=fgw_hash(def, n->buckets);
	if (n->bucket[b]==leaf)
	    n->bucket[b]=e->next;
	else
	{
	    fgw_hstentry *d;

	    d=fgw_hashentry(bytevar, n->bucket[b]);
	    while (d->next && d->next!=leaf)
		d=d=fgw_hashentry(bytevar, d->next);
	    if (d->next)
		d->next=e->next;
	}
	fgw_hstfree(bytevar, leaf);
/*
** place new entry on top of the bucket chain
*/
	ne->next=n->bucket[b];
	n->bucket[b]=o;
    }
    else
    {
	status=FGW_ENOMEM;
	return;
    }
    if (f)
	fgw_hstfree(bytevar, f);
}

/*
** add entry
*/
void fgw_hstadd(fgw_loc_t *bytevar, hst_t node, char *def,
		int type, hst_t len, char *entry)
{
    int b, hashassign;
    hst_t o, l;
    fgw_hstentry *e;
    fgw_hstnode *n;

    if (e=fgw_hstget(bytevar, node, def, &b))
	fgw_hstidxadd(bytevar, e->magic, type, len, entry);
    else if (status)
	return;
    else
    {
/*
** ditto
*/
	hashassign=(entry>=bytevar->loc_buffer &&
		    entry<=bytevar->loc_buffer+bytevar->loc_bufsize)?
		   (int) (entry-bytevar->loc_buffer): 0;
	switch(type)
	{
	  case CSTRINGTYPE:
	    l=(len>0)? len+1: ((len<0)? 0: fgw_strlen(entry)+1);
	    break;
	  case CDOUBLETYPE:
	    l=sizeof(double);
	    break;
	  case CINTTYPE:
	  case CDATETYPE:
	    l=sizeof(int);
	    break;
	  case CDTIMETYPE:
	    l=sizeof(fgw_dtime_t);
	    break;
	  case CINVTYPE:
	    l=sizeof(fgw_intrvl_t);
	    break;
	  case CDECIMALTYPE:
	  case CMONEYTYPE:
	    l=sizeof(fgw_dec_t);
	    break;
	  default:
	    if (type<0 || len<-1)
	    {
		status=FGW_EINVAL;
		return;
	    }
	    l=len>0? len: 0;
	}
	if (o=fgw_hstnew(bytevar, l, fgw_strlen(def)))
	{
	    e=fgw_hashentry(bytevar, o);
/*
** construct new entry
*/
	    e->type=type;
	    e->len=len==-1? len: l;
	    e->def=l;
	    fgw_move(&(e->entry),
		     hashassign? bytevar->loc_buffer+hashassign: entry, l);
	    fgw_move(&(e->entry)+l, def, fgw_strlen(def)+1);
/*
** work out node entry
*/
	    if (!node)
		node=fgw_hashhead(bytevar)->root;
	    e->node=node;
	    n=(fgw_hstnode *) &fgw_hashentry(bytevar, node)->entry;
/*
** place it on top of the bucket chain
*/
	    e->next=n->bucket[b];
	    n->bucket[b]=o;
/*
** fix header * reorg if necessary
*/
	    fgw_hashhead(bytevar)->entries++;
	    n->entries++;
	    if (n->entries>n->buckets)
		n->flags|=H_REORG;
	    if (n->entries>n->buckets*K_hashlim &&
		(n->buckets*K_hashmult<=K_hashmax))
		reorg_hst(bytevar, node, n->buckets*K_hashmult);
	}
	else
	    status=FGW_ENOMEM;
    }
}

/*
** create dummy entry
*/
fgw_hstentry *fgw_hstreserve(fgw_loc_t *bytevar, hst_t node,
			     char *def, hst_t size)
{
    hst_t o;
    fgw_hstentry *e=NULL;
    fgw_hstnode *n;
    int b;

    if (e=fgw_hstget(bytevar, node, def, &b))
	return e;
    else if (status || !size)
	return NULL;
    else
    {
	if (o=fgw_hstnew(bytevar, size, fgw_strlen(def)))
	{
	    e=fgw_hashentry(bytevar, o);
/*
** construct new entry
*/
	    e->type=CSTRINGTYPE;
	    e->def=size;
	    fgw_move(&(e->entry)+size, def, fgw_strlen(def)+1);
	    e->entry='\0';
/*
** work out node entry
*/
	    if (!node)
		node=fgw_hashhead(bytevar)->root;
	    e->node=node;
	    n=(fgw_hstnode *) &fgw_hashentry(bytevar, node)->entry;
/*
** place it on top of the bucket chain
*/
	    e->next=n->bucket[b];
	    n->bucket[b]=o;
/*
** fix header * reorg if necessary
*/
	    fgw_hashhead(bytevar)->entries++;
	    n->entries++;
	    if (n->entries>n->buckets)
		n->flags|=H_REORG;
	    if (n->entries>n->buckets*K_hashlim &&
		(n->buckets*K_hashmult<=K_hashmax))
		reorg_hst(bytevar, node, n->buckets*K_hashmult);
	}
	else
	    status=FGW_ENOMEM;
    }
    return e;
}

/*
** add node
*/
int fgw_hstaddnode(fgw_loc_t *bytevar, hst_t node,
		   char *def, int buckets)
{
    fgw_hstentry *e;
    fgw_hstnode *n;
    int b;
    hst_t o;

    if (e=fgw_hstget(bytevar, node, def, &b))
	status=FGW_EINVAL;
    else if (!status)	/* catches stuff like invalid hash */
    {
	if (buckets<K_hashmin || buckets>K_hashmax)
	    status=FGW_EINVAL;
	else
	{
/*
** allocate & initialize new node
*/
	    o=fgw_hstalloc(bytevar, node, def, buckets);
	    e=fgw_hashentry(bytevar, o);
	    if (!node)
		node=fgw_hashhead(bytevar)->root;
	    n=(fgw_hstnode *) &fgw_hashentry(bytevar, node)->entry;
/*
** place it on top of the bucket chain
*/
	    e->next=n->bucket[b];
	    n->bucket[b]=o;
/*
** fix header * reorg if necessary
*/
	    fgw_hashhead(bytevar)->entries++;
	    n->entries++;
	    if (n->entries>n->buckets)
		n->flags|=H_REORG;
	    if (n->entries>n->buckets*K_hashlim &&
		(n->buckets*K_hashmult<=K_hashmax))
		reorg_hst(bytevar, node, n->buckets*K_hashmult);
	    return o;
	}
    }
    return 0;
}
/*
** check that all children leaves are not latched
*/
static int chkrefcnt_hst(fgw_loc_t * bytevar, fgw_hstentry *e)
{
    fgw_hstentry *c;
    fgw_hstnode *n;
    hst_t o;
    int i;

    n=(fgw_hstnode *) &e->entry;
    for (i=0; i<n->buckets; i++)
    {
	o=n->bucket[i];
	while (o)
	{
	    c=fgw_hstidxget(bytevar, o);
	    if (c->refcnt ||
		(c->type==T_HASH &&
		chkrefcnt_hst(bytevar, c)))
		    return 1;
	    o=c->next;
	}
    }
    return 0;
}

/*
** recursively drop children nodes and leaves
*/
static int drop_hst(fgw_loc_t *bytevar, fgw_hstentry *e)
{
    fgw_hstentry *c;
    fgw_hstnode *n;
    hst_t o;
    int i;

    n=(fgw_hstnode *) &e->entry;
    for (i=0; i<n->buckets; i++)
    {
	o=n->bucket[i];
	while (o)
	{
	    c=fgw_hstidxget(bytevar, o);
	    if (c->type==T_HASH)
		drop_hst(bytevar, c);
	    o=c->next;
	    if (c->type==T_FWD)
		fgw_hstfree(bytevar, *(hst_t *) &(c->entry));
	    fgw_hstfree(bytevar, c->magic);
	    fgw_hashhead(bytevar)->entries--;
	}
    }
}

/*
** drop entry
*/
void fgw_hstidxdrop(fgw_loc_t *bytevar, hst_t node, int bucket)
{
    fgw_hstentry *d, *e;
    fgw_hstnode *n;
    hst_t p;

    if (!node || fgw_testhash(bytevar))
    {
        status=FGW_EINVAL;
        return;
    }
    e=fgw_hstidxget(bytevar, node);
    if (testentry_hst(bytevar, e, node))
	status=FGW_EINVAL;
    else if (e->refcnt ||
	     e->type==T_HASH && chkrefcnt_hst(bytevar, e))
	status=FGW_EBUSY;
    else
    {
	p=e->node;
	if (e->type==T_HASH)
	    drop_hst(bytevar, e);
	else if (e->type==T_FWD)
	    fgw_hstfree(bytevar, *(hst_t *) &(e->entry));
/*
** work out node entry
*/
	if (!p)
	    p=fgw_hashhead(bytevar)->root;
	n=(fgw_hstnode *) &fgw_hashentry(bytevar, p)->entry;
	if (bucket<0)
	    bucket=fgw_hash(fgw_defentry(e), n->buckets);
	if (node=n->bucket[bucket])
	    n->bucket[bucket]=e->next;
	else
	{
	    d=fgw_hashentry(bytevar, n->bucket[bucket]);
	    for (;;)
	    {
		if (d->next==node)
		{
		    d->next=e->next;
		    break;
		}
		else if (d->next)
		    d=fgw_hashentry(bytevar, d->next);
		else
		    break;
	    }
	}
	n->entries--;
	fgw_hashhead(bytevar)->entries--;
	fgw_hstfree(bytevar, node);
/*
** reorg and compress if necessary
*/
	if ((n->flags & H_REORG) && (n->entries*K_hashlim<n->buckets) &&
	    (n->buckets/K_hashmult>=K_hashmin))
		reorg_hst(bytevar, p, n->buckets/K_hashmult);
	if (fgw_hashhead(bytevar)->freecnt*2>bytevar->loc_bufsize)
	    compress_hst(bytevar);
    }
}

/*
** latch entry
*/
void fgw_hstlatch(fgw_loc_t *bytevar, hst_t leaf, int op)
{
    fgw_hstentry *e;

    e=fgw_hashentry(bytevar, leaf);
    if (testentry_hst(bytevar, e, leaf))
    {
	status=FGW_EINVAL;
	return;
    }
    if (op>0)
    {
	e->refcnt++;
	fgw_hashhead(bytevar)->refcnt++;
    }
    else if (op<0 && e->refcnt)
    {
	e->refcnt--;
	fgw_hashhead(bytevar)->refcnt--;
    }
}

#if defined(A4GL) || defined(I4GLVER)
/*
**  starts a hash table
*/
int FGLGLOB(hst_locate)(nargs)
int nargs;
{
    LOC_T(bytevar)
    int buckets, i;
    fgw_hstentry *h;

    popint(&buckets);
    POPLOCATOR(bytevar)
    fgw_locatehash(bytevar, buckets);
    RESTORELOCATOR(bytevar)
    return 0;
}

/*
**  frees a hash table
*/
int FGLGLOB(hst_dispose)(nargs)
int nargs;
{
    LOC_T(bytevar)

    POPLOCATOR(bytevar)
    fgw_freehash(bytevar);
    if (!bytevar->loc_buffer)
	bytevar->loc_loctype=0;
    RESTORELOCATOR(bytevar)
    return(0);
}

/*
**  enters/modifies an entry
*/
int FGLGLOB(hst_add)(nargs)
int nargs;
{
    LOC_T(bytevar)
    char *def;
    hst_t node;
    int type;
    char *c_entry;
    int i_entry;
    double f_entry;
#ifndef A4GL
    fgw_dec_t d_entry;
    fgw_dtime_t dt_entry;
    fgw_intrvl_t iv_entry;
#endif

    popint(&type);
    switch (type)
    {
      case CINTTYPE:
      case CDATETYPE:
	popint(&i_entry);
	c_entry=(char *) &i_entry;
	break;
      case CDOUBLETYPE:
	popdub(&f_entry);
	c_entry=(char *) &f_entry;
	break;
#ifndef A4GL
      case CMONEYTYPE:
      case CDECIMALTYPE:
	popdec(&d_entry);
	c_entry=(char *) &d_entry;
	break;
      case CDTIMETYPE:
	popdtime(&dt_entry, 0);	/* heck, I don't know the qualifier yet! */
	c_entry=(char *) &dt_entry;
	break;
      case CINVTYPE:
	popinv(&iv_entry, 0);	/* heck, I don't know the qualifier yet! */
	c_entry=(char *) &iv_entry;
	break;
#endif
      default:
	type=CSTRINGTYPE;
	c_entry=fgw_popquote();
    }
    def=fgw_popquote();
    popint(&node);
    POPLOCATOR(bytevar)
    if (def && c_entry)
	fgw_hstadd(bytevar, node, def, type, 0, c_entry);
    if (def)
	free(def);
    if (type==CSTRINGTYPE && c_entry)
	free(c_entry);
    RESTORELOCATOR(bytevar)
    return 0;
}

/*
**  enters/modifies an entry
*/
int FGLGLOB(hst_update)(nargs)
int nargs;
{
    LOC_T(bytevar)
    hst_t leaf;
    int type;
    char *c_entry;
    int i_entry;
    double f_entry;
#ifndef A4GL
    fgw_dec_t d_entry;
    fgw_dtime_t dt_entry;
    fgw_intrvl_t iv_entry;
#endif

    popint(&type);
    switch (type)
    {
      case CINTTYPE:
      case CDATETYPE:
	popint(&i_entry);
	c_entry=(char *) &i_entry;
	break;
      case CDOUBLETYPE:
	popdub(&f_entry);
	c_entry=(char *) &f_entry;
	break;
#ifndef A4GL
      case CMONEYTYPE:
      case CDECIMALTYPE:
	popdec(&d_entry);
	c_entry=(char *) &d_entry;
	break;
      case CDTIMETYPE:
	popdtime(&dt_entry, 0);	/* heck, I don't know the qualifier yet! */
	c_entry=(char *) &dt_entry;
	break;
      case CINVTYPE:
	popinv(&iv_entry, 0);	/* heck, I don't know the qualifier yet! */
	c_entry=(char *) &iv_entry;
	break;
#endif
      default:
	type=CSTRINGTYPE;
	c_entry=fgw_popquote();
    }
    popint(&leaf);
    POPLOCATOR(bytevar)
    if (c_entry)
	fgw_hstidxadd(bytevar, leaf, type, 0, c_entry);
    if (type==CSTRINGTYPE && c_entry)
	free(c_entry);
    RESTORELOCATOR(bytevar)
    return 0;
}

/*
**  drops an entry
*/
int FGLGLOB(hst_delete)(nargs)
int nargs;
{
    LOC_T(bytevar)
    char *def;
    hst_t node;
    int b;
    fgw_hstentry *e;

    def=fgw_popquote();
    popint(&node);
    POPLOCATOR(bytevar)
    if (def && (e=fgw_hstget(bytevar, node, def, &b)))
    {
	fgw_hstidxdrop(bytevar, e->magic, b);
	free(def);
	RESTORELOCATOR(bytevar)
    }
    return 0;
}

/*
**  retrieve an entry
*/
int FGLGLOB(hst_get)(nargs)
int nargs;
{
    LOC_T(bytevar)
    char *def;
    hst_t node;
    fgw_hstentry *entry;
    int d;

    def=fgw_popquote();
    popint(&node);
    POPLOCATOR(bytevar)
    if (def && (entry=fgw_hstget(bytevar, node, def, &d)))
    {
 	retint(entry->magic);
	entry=fgw_hstidxget(bytevar, entry->magic);
	switch (entry->type)
	{
	  case CSTRINGTYPE:
	    retquote(&(entry->entry));
	    break;
	  case CINTTYPE:
	    retint(*(int *) &(entry->entry));
	    break;
	  case CDATETYPE:
	    retdate(*(int *) &(entry->entry));
	    break;
	  case CDOUBLETYPE:
	    retdub((double *) &(entry->entry));
	    break;
#ifndef A4GL
	  case CMONEYTYPE:
	  case CDECIMALTYPE:
	    retdec((fgw_dec_t *) &(entry->entry));
	    break;
	  case CDTIMETYPE:
	    retdtime((fgw_dtime_t *) &(entry->entry));
	    break;
	  case CINVTYPE:
	    retinv((fgw_intrvl_t *) &(entry->entry));
#endif
	  default:		/* FIXME: OK for hashes, what about bytes? */
	    retquote("");
	}
    }
    else
    {
	retquote("");
	retquote("");
    }
    if (def)
	free(def);
    return 2;
}

/*
**  retrieve an entry
*/
int FGLGLOB(hst_fetch)(nargs)
int nargs;
{
    LOC_T(bytevar)
    hst_t leaf;
    fgw_hstentry *entry;
    int d;

    popint(&leaf);
    POPLOCATOR(bytevar)
    if (entry=fgw_hstidxget(bytevar, leaf))
    {
	switch (entry->type)
	{
	  case CSTRINGTYPE:
	    retquote(&(entry->entry));
	    break;
	  case CINTTYPE:
	    retint(*(int *) &(entry->entry));
	    break;
	  case CDATETYPE:
	    retdate(*(int *) &(entry->entry));
	    break;
	  case CDOUBLETYPE:
	    retdub((double *) &(entry->entry));
	    break;
#ifndef A4GL
	  case CMONEYTYPE:
	  case CDECIMALTYPE:
	    retdec((fgw_dec_t *) &(entry->entry));
	    break;
	  case CDTIMETYPE:
	    retdtime((fgw_dtime_t *) &(entry->entry));
	    break;
	  case CINVTYPE:
	    retinv((fgw_intrvl_t *) &(entry->entry));
#endif
	  default:		/* FIXME: OK for hashes, what about bytes? */
	    retquote("");
	}
    }
    else
	retquote("");
    return 1;
}

/*
**  creates a new node
*/
int FGLGLOB(hst_addnode)(nargs)
int nargs;
{
    LOC_T(bytevar)
    hst_t node;
    char *def;
    int buckets;

    popint(&buckets);
    def=fgw_popquote();
    popint(&node);
    POPLOCATOR(bytevar)
    if (def)
    {
	retint(fgw_hstaddnode(bytevar, node, def, buckets));
	free(def);
	RESTORELOCATOR(bytevar)
    }
    else
	retquote("");
    return 1;
}

/*
**  hash scan (for member in hash...)
**
**  FIXME: hst_get, hst_delete, hst_update, hst_add, hst_addnode all change
**  the order of entries and affect the outcome of the scan...
*/
int FGLGLOB(hst_walknode)(nargs)
int nargs;
{
    LOC_T(bytevar)
    hst_t node, leaf, nleaf=0;	/* should never be used uninitialized, but just in case */
    fgw_hstnode *n;
    fgw_hstentry *h;
    int l;

    popint(&leaf);
    popint(&node);
    POPLOCATOR(bytevar)
    if (fgw_testhash(bytevar))
	goto badexit1;
/*
** starting scan, return first entry in node
*/
    if (risnull(CINTTYPE, (char *) &leaf)) /* FIXME */
    {
	if (!node)
	    h=fgw_hashentry(bytevar, fgw_hashhead(bytevar)->root);
	else
	{
	    if (node<0 || node>bytevar->loc_size)
		goto badexit1;
	    h=fgw_hashentry(bytevar, node);
	    if (h->magic!=node || h->type!=T_HASH)
		goto badexit1;
	}
	n=(fgw_hstnode *) &h->entry;
	l=0;
	while (l<n->buckets && !n->bucket[l])
	    l++;
	if (l>=n->buckets)
	    goto badexit;
	nleaf=n->bucket[l];
    }
/*
** already started
*/
    else
    {
	if (leaf<0 || leaf>bytevar->loc_size)
	    goto badexit1;
	h=fgw_hashentry(bytevar, leaf);
	if (h->magic!=leaf || (h->type<0 && h->type!=T_HASH))
	    goto badexit1;
/*
** return next entry if we have one
*/
	if (h->next)
	    nleaf=h->next;
/*
** hmmm more complicated: work out next bucket in node, and scan that
*/
	else
	{
	    n=(fgw_hstnode *) &fgw_hashentry(bytevar, h->node)->entry;
	    l=fgw_hash(fgw_defentry(h), n->buckets);
	    while (++l<n->buckets && !n->bucket[l]);
	    if (l>=n->buckets)
		goto badexit;
	    nleaf=n->bucket[l];
	}
    }
    retint(nleaf);
    h=fgw_hashentry(bytevar, nleaf);
    retquote(fgw_defentry(h));
    if (h->type==T_FWD)
	h=fgw_hashentry(bytevar, *(hst_t *) &h->entry);
    switch (h->type)
    {
      case CSTRINGTYPE:
	    retquote(&(h->entry));
	    break;
      case CINTTYPE:
	    retint(*(int *) &(h->entry));
	    break;
      case CDATETYPE:
	    retdate(*(int *) &(h->entry));
	    break;
      case CDOUBLETYPE:
	    retdub((double *) &(h->entry));
	    break;
#ifndef A4GL
      case CMONEYTYPE:
      case CDECIMALTYPE:
	    retdec((fgw_dec_t *) &(h->entry));
	    break;
      case CDTIMETYPE:
	    retdtime((fgw_dtime_t *) &(h->entry));
	    break;
      case CINVTYPE:
	    retinv((fgw_intrvl_t *) &(h->entry));
#endif
      default:		/* FIXME: OK for hashes, what about bytes? */
	retquote("");
    }
    return(3);
badexit1:
    status=FGW_EINVAL;
badexit:
    SETNULLINT((char *) &nleaf);
    retint(nleaf);
    retquote("");
    retquote("");
    return(3);
}
#endif
