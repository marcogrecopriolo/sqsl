/*
	CCMNC.c  -  Functions used by C functions only

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2011 Marco Greco (marco@4glworks.com)

	Initial release: Oct 92
	Current release: Aug 16

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

#include <stdlib.h>
#include <string.h>
#include "ccmpc.h"
#include "ccmnc.h"
#define MINHASHLIST 4

/*
**  Internal routine to clip strings
*/
void fgw_clip(char *c)
{
    char *p;
    int i;

    i=1;
    if (c)
    {
	while (*c)
	{
	    if (*c!=' ')
		i=1;
	    else if (i)
	    {
		p=c;
		i=0;
	    }
	    c++;
	}
	if (!i)
	    *p=0;
    }
}

/*
**  returns the position of a substring within a string
*/
int fgw_pos(char *s1, char *s2)
{
    char *p, *q, *r;

    if (!*s2)
	return 0;
    for (p=s1; *p;)
    {
	r=p++;
	q=s2;
	while (*r==*q)
	{
	    r++;
	    q++;
	    if (!*q)
		return p-s1;
	}
    }
    return 0;
}

/*
**  moves buffer contents (memmove not available in all systems)
*/
void fgw_move(void *d, void *s, int c)
{
    char *ss=(char *) s;
    char *dd=(char *) d;

    if (dd>ss)
    {
	dd+=c;
	ss+=c;
	while (c--)
	    *--dd=*--ss;
    }
    else
	while (c--)
	    *dd++=*ss++;
}

/*
**  moves buffer contents skipping escapes (buffers may not overlap)
*/
void fgw_escmove(void *d, void *s, int c, char e)
{
    char *ss=(char *) s;
    char *dd=(char *) d;

    while (c--)
    {
	if (*ss==e)
	    ss++;
	*dd++=*ss++;
    }
}

/*
** case insensitive string comparison
*/
int fgw_strcasecmp(char *s1, char *s2)
{
    for (;; fgw_chrnext(s1), fgw_chrnext(s2))
    {
	int d=fgw_tolower(*s1)-fgw_tolower(*s2);
	if (d || !*s1)
	    return d;
    }
}
/*
** case insensitive string comparison
*/
int fgw_strncasecmp(char *s1, char *s2, int l)
{
    for (;; fgw_chrnext(s1), fgw_chrnext(s2))
    {
	int d=fgw_tolower(*s1)-fgw_tolower(*s2);
	if (d || !*s1)
	    return d;
	if (--l==0)
	    return l;
    }
}

/*
** case insensitive string search
*/
char *fgw_strcasestr(char *s1, char *s2)
{
    if (!s1 || !s2)
	return NULL;

    for (; *s1; fgw_chrnext(s1))
    {

	char *h, *n;

	for (h=s1, n=s2;  *h && *n && fgw_tolower(*h)==fgw_tolower(*n);
	     fgw_chrnext(h), fgw_chrnext(n));
	if (!*n)
	    return s1;
    }
    return NULL;
}

#if defined (I4GLVER) || defined(A4GL)
/*
**  pops a char, allocating appropriate mem amount
*/
char *fgw_popquote()
{
    char *p, *q;
    int l;
    char b[2];

    p=(char *) malloc(36000);
    if (p)
    {
	popquote(p, 32000);
	fgw_clip(p);
/*
** buffer is going to be 10% bigger than the string being popped and at
** least 10 chars more. if you exceed this amount, expect a core dump!
*/
	if ((l=strlen(p))<100)
	   l+=10;
	else
	   l+=(l/10);
	if (q=(char *) realloc(p, l))
	    p=q;		/* defensive code, really */
    }
    else
	popquote((char *) &b, 2);	/* clear stack */
    return(p);
}
#endif

/*
** int comparison
*/
int fgw_intcomp(fgw_listentry *e1, fgw_listentry *e2)
{
    return e1->int_id-e2->int_id;
}

/*
** string comparison
*/
int fgw_stringcomp(fgw_listentry *e1, fgw_listentry *e2)
{
    return strcmp(e1->char_id, e2->char_id);
}

/*
** ints we don't really hash
*/
unsigned int fgw_inthash( fgw_listentry *e)
{
#ifdef __STDC__
    return (unsigned int) e->int_id;;
#else
    return e->int_id & (((int) -1)>>1);
#endif
}

/*
**  string hash function
*/
unsigned int fgw_stringhash(fgw_listentry *e)
{
    char *c;
    unsigned long h;
 
    h=0;
    c=e->char_id;
    while (*c)
	h=*c++ +65599*h;
#ifndef __STDC__
    h&=(((int) -1)>>1);     /* avoid negative hashes! */
#endif
    return h;
}

/*
**  search list entry
*/
fgw_listentry *fgw_listsearch(fgw_listentry *e, fgw_listtype *list)
{
    unsigned int h;
    int p;
    fgw_listhash *hp=&list->root;
    fgw_listentry *t;

    h=(list->hash)(e);
    for (;;)
    {
	p=h % hp->size;
/*
** we have hit a list, finally
*/
	if (hp->hash[p].count>0)
	{
	    t=(fgw_listentry *) hp->hash[p].down;
	    while (t)
	    {
		if (!(list->cmpr)(t, e) && t->signature==e->signature)
		    return t;
		t=t->hashprev;
	    }
	    status=list->notfound;
	    return NULL;
	}
/*
** yet another hash
*/
	else if (hp->hash[p].count<0)
	    hp=hp->hash[p].down;
/*
** empty, exit
*/
	else
	{
	    status=list->notfound;
	    return NULL;
	}
    }
}

/*
** scans list
*/
fgw_listentry *fgw_listnext(fgw_listtype *list, fgw_listentry *el)
{
    if (el)
	return el->prev;
    else
	return list->head;
}

/*
**  add entry to list
*/
void fgw_newentry(fgw_listentry *entry, fgw_listtype *list)
{
    unsigned int h;
    int p;
    fgw_listhash *hp=&list->root, *nhp;
    fgw_listentry *t, *nt;

    if (list->base)
	entry->int_id=list->base++;
    entry->prev=list->head;
    if (list->head)
	list->head->next=entry;
    entry->next=NULL;
    list->head=entry;
    h=(list->hash)(entry);
/*
** follow hash hierarchy
*/
    for (;;)
    {
	p=h % hp->size;
	if (hp->hash[p].count<0)
	    hp=hp->hash[p].down;
/*
** found list, there is no space, split
*/
	else if (hp->hash[p].count>=hp->size-1 && hp->size>MINHASHLIST)
	{
/*
** undo insert in case of error
*/
	    if (!(nhp=(fgw_listhash *) malloc(sizeof(fgw_listhash))))
	    {
		status=FGW_ENOMEM;
		list->head=entry->prev;
		if (entry->prev)
		    entry->prev->next=NULL;
		return;
	    }
	    memset(nhp, 0, sizeof(fgw_listhash));
	    nhp->size=hp->size-1;
	    nhp->up=hp;
	    nhp->pos=p;
	    t=(fgw_listentry *) hp->hash[p].down;
	    hp->hash[p].down=nhp;
	    hp->hash[p].count=-1;
/*
** append all entries
*/
	    while (t)
	    {
		nt=t;
		t=t->hashprev;
		p=nt->hash % nhp->size;
		nhp->hash[p].count++;
		nt->up=nhp;
		nt->hashprev=(fgw_listentry *) nhp->hash[p].down;
		nt->hashnext=NULL;
		nhp->hash[p].down=(fgw_listhash *) nt;
		if (nt->hashprev)
		    nt->hashprev->hashnext=nt;
	    }
/*
** next loop will append entry
*/
	    hp=nhp;
	}
/*
** add to this hash
*/
	else
	{
	    entry->up=hp;
	    entry->hash=h;
	    entry->hashnext=NULL;
	    entry->hashprev=(fgw_listentry *) hp->hash[p].down;
	    if (entry->hashprev)
		entry->hashprev->hashnext=entry;
	    hp->hash[p].down=(fgw_listhash *) entry;
	    hp->hash[p].count++;
	    return;
	}
    }
}

/*
**  remove entry from list
*/
void fgw_dropentry(fgw_listentry *entry, fgw_listtype *list)
{
    int p;
    fgw_listhash *hp;
    fgw_listentry *t;

/*
** detach from list
*/
    if (list->head==entry)
    {
	list->head=entry->next;
	if (list->head)
	   list->head->next=NULL;
    }
    else
    {
	entry->next->prev=entry->prev;
	if (entry->prev)
	    entry->prev->next=entry->next;
    }
/*
** detach from hash
*/
    hp=entry->up;
    p=entry->hash % hp->size;
    hp->hash[p].count--;
    if (hp->hash[p].down==(fgw_listhash *) entry)
    {
	hp->hash[p].down=(fgw_listhash *) entry->hashprev;
	if (entry->hashprev)
	    entry->hashprev->hashnext=NULL;
    }
    else
    {
	entry->hashnext->hashprev=entry->hashprev;
	if (entry->hashprev)
	    entry->hashprev->hashnext=entry->hashnext;
    }
/*
** shrink hierarchy
*/
    if (hp->up)
    {
	int c=0;

	for (p=0; p<hp->size; p++)
	{
	    if (hp->hash[p].count<0)
	    {
		c=-1;
		break;
	    }
	    c+=hp->hash[p].count;
	}
/*
** only if all buckets contain lists (and not other hashes)
** and there are less than size entries
*/
	if (c>=0 && c<hp->size)
	{
	    hp->up->hash[hp->pos].count=c;
	    hp->up->hash[hp->pos].down=NULL;
	    for (p=0; p<hp->size; p++)
	    {
		if (hp->hash[p].count)
		{
		    if (hp->up->hash[hp->pos].down)
		    {
			t=(fgw_listentry *) hp->hash[p].down;
			while (t->hashprev)
			    t=t->hashprev;
			t->hashprev=(fgw_listentry *) hp->up->hash[hp->pos].down;
			t->hashprev->hashnext=t;
			hp->up->hash[hp->pos].down=hp->hash[p].down;
		    }
		    else
			hp->up->hash[hp->pos].down=hp->hash[p].down;
		}
	    }
	}
    }
    free(entry);
}

/*
** initializes stack structure
*/
void fgw_stackinit(fgw_stacktype *stack, int size)
{
    stack->size=size;
    stack->count=0;
    stack->len=0;
    stack->stack=NULL;
}

/*
** frees stack structure
*/
void fgw_stackfree(fgw_stacktype *stack, void (*clear)())
{
    int i;
    char *s;

    if (clear)
        for (i=0, s=stack->stack; i<stack->count; i++, s+=stack->size)
	    (*clear)(s);
    if (stack->stack)
	free(stack->stack);
    stack->len=0;
    stack->count=0;
    stack->size=0;
    stack->stack=NULL;
}

/*
** pushes element onto stack
*/
void fgw_stackpush(fgw_stacktype *stack, void *element)
{
    char *dummy;

    if (stack->len==stack->count || !stack->len)
    {
	if (!(dummy=(char *) realloc(stack->stack, (stack->len+10)*stack->size)))
	{
	    status=FGW_ENOMEM;
	    return;
	}
	stack->len+=10;
	stack->stack=dummy;
    }
    fgw_move(stack->stack+(stack->count*stack->size), element, stack->size);
    stack->count++;
}

/*
** pops element off stack
*/
char *fgw_stackpop(fgw_stacktype *stack)
{
    if (stack->count)
    {
	stack->count--;
	return stack->stack+(stack->count*stack->size);
    }
    else
	return NULL;
}

/*
** peeks top of stack
*/
char *fgw_stackpeek(fgw_stacktype *stack)
{
    if (stack->count)
	return stack->stack+((stack->count-1)*stack->size);
    else
	return NULL;
}

/*
** drops n elements off stack
*/
void fgw_stackdrop(fgw_stacktype *stack, int n)
{
    if (n>0)
	if (stack->count>=n)
	    stack->count-=n;
	else
	    stack->count=0;	/* FIXME: tighter error checking */
}

/* circular linked list going backwards (allows fast ordered inserts)
   linked list going forward (easy end of list test) */


/*
** attach element to temporary space
*/
void fgw_tssattach(fgw_tsstype **head, char *element)
{
    fgw_tsstype *p;
 
    
    p=FGW_PTR2TSS(element);
    if (*head)
    {
	(*head)->prev->next=p;
	p->prev=(*head)->prev;
	(*head)->prev=p;
    }
    else
    {
	p->prev=p;
	(*head)=p;
    }
    p->next=NULL;
}

/*
** detach element from temporary space
*/
void fgw_tssdetach(fgw_tsstype **head, char *element)
{
    fgw_tsstype *s;
 
    s=FGW_PTR2TSS(element);
    if (s==*head)
    {
	*head=s->next;
	if (*head)
	    (*head)->prev=s->prev;    
    }
    else
    {
	if (s->next)
	{
	    s->prev->next=s->next;
	    s->next->prev=s->prev;
	}
	else
	{
	    s->prev->next=NULL;
	    (*head)->prev=s->prev;
	}
    }
}

/*
** allocate temporary space
*/
char *fgw_tssmallocx(fgw_tsstype **head, int size, void (*func)())
{
    fgw_tsstype *p;
 
    if (!(p=(fgw_tsstype *) malloc(size+sizeof(fgw_tsstype))))
    {
	status=FGW_ENOMEM;
	return NULL;
    }
    p->size=size;
    p->func=func;
    fgw_tssattach(head, &(p->string));
    return &(p->string);
}

/*
** reallocate temporary space
*/
char *fgw_tssrealloc(fgw_tsstype **head, char *element, int size)
{
    fgw_tsstype *p, *n;
 
    if (!element)
	return fgw_tssmalloc(head, size);
    p=FGW_PTR2TSS(element);

    /* detach before reallocing, so as not to corrupt memory */
    fgw_tssdetach(head, element);
    if (n=(fgw_tsstype *) realloc(p, size+sizeof(fgw_tsstype)))
    {
	n->size=size;
	fgw_tssattach(head, &(n->string));
	return &(n->string);
    }
    else
    {
	fgw_tssattach(head, element);
	status=FGW_ENOMEM;
	return NULL;
    }
}

/*
** free temporary space
*/
void fgw_tssfree(fgw_tsstype **head, char *element)
{
    fgw_tsstype *s;
 
    s=FGW_PTR2TSS(element);
    fgw_tssdetach(head, element);
    if (s->func)
	(s->func)(s->string);
    free(s);
}

/*
** drop temporary space
*/
void fgw_tssdrop(fgw_tsstype **head)
{
    fgw_tsstype *h, *n;
 
    h=*head;
    while (n=h)         /* not an error */
    {
        h=h->next;
	if (n->func)
	   (n->func)(n->string);
        free(n);
    }
    *head=NULL;
}

/*
** hash function
*/
int fgw_hash(char *s, long b)
{
    char *c;
    unsigned long h;
 
    h=0;
    c=s;
    while (*c)
	h=*c++ +65599*h;
#ifndef __STDC__
    h&=(((int) -1)>>1);     /* avoid negative hashes! */
#endif
    return (h%b);
}
