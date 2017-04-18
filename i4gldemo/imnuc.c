/*
	IMNUC.c  -  Message masking routines

	The Structured Query Scripting Language
	(Borrowed from) The 4glWorks application framework
	Copyright (C) 1992-2002 Marco Greco (marco@4glworks.com)

	Initial release: May 96
	Current release: Oct 02

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

#include "ccmpc.h"
#include "ccmnc.h"
#define T_message	char
#define K_maxpane	5
#define K_shiftenable	3
#define K_andenable	0xF8
#define K_andpane	7
#define K_focussedpane	7
#define MB_minmask	20
#define MB_maxmask	511

static unsigned T_message messages[MB_maxmask-MB_minmask];

/*
**  disables all messages
*/
int FGLGLOB(mnu_clear)(nargs)
int nargs;
{
    int i;

    for (i=0; i<MB_maxmask-MB_minmask; i++)
	messages[i]=0;
    return(0);
}

/*
**  enables a range of messages for panes as set forth in mask
*/
int FGLGLOB(mnu_mask)(nargs)
int nargs;
{
    int s, e;
    int c;
    unsigned T_message m;

    popint(&c);
    popint(&e);
    popint(&s);
    if (s<MB_minmask) s=MB_minmask;
    if (e>MB_maxmask) e=MB_maxmask;
    if (s>e) return(0);
    s-=MB_minmask;
    e-=MB_minmask;
    m=c<<K_shiftenable;
    for (c=s; c<=e; c++)
	messages[c]=(messages[c]&K_andpane)|m;
    return(0);
}

/*
**  enables a range of messages in a particular pane
*/
int FGLGLOB(mnu_enable)(nargs)
int nargs;
{
    int s, e, op;
    int c;
    unsigned T_message m;

    popint(&op);
    popint(&c);
    popint(&e);
    popint(&s);
    if (s<MB_minmask) s=MB_minmask;
    if (e>MB_maxmask) e=MB_maxmask;
    if (s>e || c<1 || c>K_maxpane) return(0);
    s-=MB_minmask;
    e-=MB_minmask;
    m=1<<(c+K_shiftenable-1);
    if (op!=0)
	for (c=s; c<=e; c++)
	    messages[c]=messages[c]|m;
    else
	for (c=s; c<=e; c++)
	    messages[c]=messages[c]&~m;
    return(0);
}

/*
**  flag that a message operates on records
*/
int FGLGLOB(mnu_record)(nargs)
int nargs;
{
    int s, e;
    int c;
    unsigned T_message m;

    popint(&c);
    popint(&e);
    popint(&s);
    if (s<MB_minmask) s=MB_minmask;
    if (e>MB_maxmask) e=MB_maxmask;
    if (s>e) return(0);
    s-=MB_minmask;
    e-=MB_minmask;
    if (c<1) m=0;
    else if (c>K_maxpane) m=K_focussedpane;
    else m=c;
    for (c=s; c<=e; c++)
	messages[c]=(messages[c]&K_andenable)|m;
    return(0);
}

/*
**  tests if a message is enabled
*/
int FGLGLOB(mnu_test)(nargs)
int nargs;
{
    int msg, pane;
    int e, p;

    popint(&pane);
    popint(&msg);
    if ((msg<MB_minmask) || (msg>MB_maxmask))
    {
	retint(-1);
	return(1);
    }
    msg-=MB_minmask;
    e=messages[msg]&(1<<(pane+K_shiftenable-1));
    p=messages[msg]&K_andpane;
    if (e==0) retint(0);
    else if (p==0) retint(-1);
    else if (p==K_focussedpane) retint(pane);
    else retint(p);
    return(1);
}
