/*
	CSTRC.c  -  String manipulation related functions

	The structured Query Scripting Language
	(Borrowed from) The 4glWorks application framework
	Copyright (C) 1992-2009 Marco Greco (marco@4glworks.com)

	Initial release: Oct 92
	Current release: Jan 09

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
#include <string.h>

#define SHORTSTRSIZE 133
#define SPACES " \t"

/*
**  returns the position of a substring within a string
*/
int FGLGLOB(pos)(nargs)
int nargs;
{
    short i, j, k;
    char *str1;
    char *str2;

    str2=fgw_popquote();
    str1=fgw_popquote();
    retint(fgw_pos(str1, str2));
    free(str1);
    free(str2);
    return(1);
}

/*
**  returns the position of a char within a string
*/
int FGLGLOB(poschr)(nargs)
int nargs;
{
    short i, j, k;
    char *str1;
    char str2[2];

    popquote((char *) &str2, sizeof(str2));
    str1=fgw_popquote();
    retint(fgw_pos(str1, str2));
    free(str1);
    return(1);
}

/*
**  returns the position of the first of a set of chars within a string
*/
int FGLGLOB(posbrk)(nargs)
int nargs;
{
    char *str1;
    char str2[SHORTSTRSIZE];
    char *p;

    popquote((char *) &str2, sizeof(str2));
    str1=fgw_popquote();
    fgw_clip(str2);
    if ((p=strpbrk(str1, str2))==NULL)
	retint(0);
    else
	retint(p-str1+1);
    free(str1);
    return(1);
}

/*
**  returns the position of the first char not in a set  within a string
*/
int FGLGLOB(posnbrk)(nargs)
int nargs;
{
    short i;
    char *str1;
    char str2[SHORTSTRSIZE];

    popquote((char *) &str2, sizeof(str2));
    str1=fgw_popquote();
    fgw_clip(str2);
    if ((i=strspn(str1, str2))==strlen(str1))
	retint(0);
    else
	retint(i+1);
    free(str1);
    return(1);
}

/*
**  returns argument's first token & rest of string
*/
int FGLGLOB(postoken)(nargs)
int nargs;
{
    int i, c, r;
    char *p;
    char *bf;
    char sep[SHORTSTRSIZE];

    p="";
    SETNULLINT((char *) &r)
    popquote((char *) &sep, sizeof(sep));
    if (bf=fgw_popquote())
	if ((i=strspn(bf, sep))!=strlen(bf))
	{
	    p=bf+i;
	    i+=strcspn(p, sep);
	    c=*(bf+i);
	    *(bf+i)=0;
	    if (c)
		r=i+2;
	}
    retquote(p);
    retint(r);
    if (bf)
	free(bf);
    return(2);
}

/*
**  returns argument's first token & rest of string
*/
int FGLGLOB(get_token)(nargs)
int nargs;
{
    int i, c;
    char *p,*q;
    char *bf;

    p="";
    q="";
    if (bf=fgw_popquote())
	if ((i=strspn(bf, SPACES))!=strlen(bf))
	{
	    p=bf+i;
	    i+=strcspn(p, SPACES);
	    c=*(q=bf+i);
	    *q=0;
	    if (c)
	    {
		++q;
		q+=strspn(q, SPACES);
	    }
	}
    retquote(p);
    retquote(q);
    if (bf)
	free(bf);
    return(2);
}

/*
**  returns the length of the initial matching segment of two strings
*/
int FGLGLOB(str_compare)(nargs)
int nargs;
{
    char *s1, *s2;
    char *c1, *c2;
    int i;

    s2=fgw_popquote();
    s1=fgw_popquote();
    for (c1=s1, c2=s2, i=0; *c1 && *c2; i++)
	if (*c1++!=*c2++)
	    break;
    free(s1);
    free(s2);
    retint(i);
    return(1);
}
