/*
	CSQUC.c  -  SQSL interpreter cli user interface

	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
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

#include <sqlca.h>
#include <string.h>
#include <stdio.h>
#include "ccmpc.h"
#include "csqrc.h"
#include "csqpc.h"

typedef struct msg
{
    int code;
    char *msg;
    struct msg *next;
} msg_t;

msg_t messages[] =
{
#include "csqmc.h"
};

#define NUMMSG (sizeof(messages)/sizeof(msg_t))

static int hash=1;

/*
** hash function for error messages hash
*/
static int hash_func(int c, int b)
{
    unsigned long h;

    h=c*65599;
#ifndef __STDC__
    h&=(((long) -1)>>1);	/* avoid negative hashes */
#endif
    return (h%b);
}

/*
** populates error messages hash
*/
static void hash_messages()
{
    msg_t  src[NUMMSG], temp[NUMMSG], *t;
    int i, b, f;
    long h;

/*
** initialize token lists
*/
    for (i=0; i<NUMMSG; i++)
        src[i]=messages[i];
    memset(&messages, 0, sizeof(messages));
    memset(&temp, 0, sizeof(temp));
    b=0;
    for (i=0; i<NUMMSG; i++)
    {
/*
** hash code, determine bucket
*/
	h=hash_func(src[i].code, NUMMSG);
/*
** oops, bucket already in use
*/
	if (messages[h].code)
	{
	    temp[b]=src[i];
	    temp[b++].next=&messages[h];
	}
	else
	    messages[h]=src[i];
    }
/*
** insert remaining tokens
*/
   f=0;
   for (i=0; i<b; i++)
   {
/*
** scan bucket chain
*/
        t=temp[i].next;
        while (t->next)
            t=t->next;
/*
** find next free entry
*/
        while (messages[f].code)
            f++;
        messages[f]=temp[i];
        messages[f].next=NULL;
        t->next=&messages[f];
    }
    hash=0;
}

/*
** display no of affected rows
*/
void sqsl_numrows(int rows)
{
    fprintf(stderr, "\nRows processed: %i\n\n", rows);
}

/*
** returns an error message text
*/
void sqsl_getmessage(int e, char *b, int l)
{
    msg_t *ms;

    if (hash)
	hash_messages();
    ms=&messages[hash_func(e, NUMMSG)];
    while (ms && ms->code!=e)
	ms=ms->next;
    if (ms)
	strcpy(b, ms->msg);
    else
	sprintf(b, "%i?", e);
}

/*
**  confirm database operation
*/
int sqsl_asktouch(int touch)
{
    return 1;
}

/*
**  expansion facility prompt/passwd functionality
*/
int sqsl_promptpasswd(int tok, int opts, char *txt,
		      char *e_buf, int *len, int verbose)
{
    return RC_NWNDW;
}

/*
** expansion facility pick list functionality
*/
int sqsl_picklist(int tok, int opts, char *txt, char *e_buf,
		  int *len, char *sep, char *quotes, int verbose)
{
    return RC_NWNDW;
}
