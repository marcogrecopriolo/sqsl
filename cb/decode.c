/*
	DECODE.c  -  SQSL interpreter json decoding primitive

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2020 Marco Greco (marco@4glworks.com)

	Initial release: Jun 16
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

#include <stdlib.h>
#include "ccmnc.h"
#include "crxuc.h"
#include "cb.h"


/*
**determine document type
*/
enum objtype json_doctype(char *buf, int *d, int *l)
{
    char c;

    if (*l==0)
	return T_EMPTY;

    while (*l>0)
    {
	switch (c=*(buf+*d))
	{
	  /* skip blanks */
	  case ' ':
          case '\t':
          case '\r':
          case '\n':
	    break;
	  case 't':
	  case 'T':
	  case 'f':
	  case 'F':
	    return T_BOOL;
	  case 'n':
	  case 'N':
	    return T_NULL;
	  case '"':
	    return T_STRING;
	  case '{':
	    return T_OBJECT;
	  case '[':
	    return T_ARRAY;
	  default:
	    if (c>='0' && c<='9')
		return T_NUM;
	    return T_UNKNOWN;
	}
	(*l)--;
	(*d)++;
    }

    return T_EMPTY;
}

/*
** search an object for a field
*/
enum objtype json_searchobject(char *field, char *buf, int *d, int *l)
{
    char c;
    enum objtype t;
    int colcount=0;
    sqslda_t name;

    (*d)++;
    (*l)--;
    while (*l>0)
    {
	c=*(buf+*d);

	/* skip blanks */
	if (c==' ' || c=='\n' || c=='\r' || c=='\t')
	{
	    (*d)++;
	    (*l)--;
	    continue;
	}

	/* found the outher end */
	if (c=='}')
	{
	    (*l)--;
	    (*d)++;
	    return T_NOTFOUND;
	}

	/* if not finished, we must have a separator */
	if (colcount!=0)
	{
	    if (c!=',')
		return T_UNKNOWN;
	    (*l)--;
	    (*d)++;
	}

	if (json_doctype(buf, d, l)!=T_STRING)
	    return T_UNKNOWN;

	/* get the field name */
	if (!json_parsestring(buf, d, l, NULL, &name, NULL))
	    return T_UNKNOWN;

	/* expect a colon */
	for (;;)
	{
	    if (*l==0)
		return T_UNKNOWN;
	    c=*(buf+*d);
	    if (c==' ' || c=='\n' || c=='\r' || c=='\t')
	    {
		(*l)--;
		(*d)++;
	    }
	    break;
	}
	if (c!= ':' || *l==0)
	     return T_UNKNOWN;
	(*l)--;
	(*d)++;

	/* get the field type */
	t=json_doctype(buf, d, l);
	if (t==T_UNKNOWN || t==T_EMPTY)
	    return T_UNKNOWN;

	/* if we have found it, we're done */
	if (strlen(field)==name.sqllen && !strncmp(buf+name.sqldata, field, name.sqllen))
	    return t;

	/* get the field */
	switch (t)
	{
	  case T_OBJECT:
	    if (!json_parseobject(buf, d, l, NULL, NULL, NULL, NULL, NULL))
		return T_UNKNOWN;
	    break;
	  case T_ARRAY:
	    if (!json_parsearray(buf, d, l, NULL, NULL, NULL, NULL, NULL))
		return T_UNKNOWN;
	    break;
	  case T_STRING:
	    if (!json_parsestring(buf, d, l, NULL, NULL, NULL))
		return T_UNKNOWN;
	    break;
	  case T_NUM:
	    if (!json_parsenum(buf, d, l, NULL))
		return T_UNKNOWN;
	    break;
	  case T_BOOL:
	    if (!json_parsebool(buf, d, l, NULL))
		return T_UNKNOWN;
	    break;
	  case T_NULL:
	    if (!json_parsenull(buf, d, l, NULL))
		return T_UNKNOWN;
	    break;
	}
	colcount++;
    }
    return T_UNKNOWN;
}

/*
** search an array for an element
*/
enum objtype json_searcharray(int element, char *buf, int *d, int *l)
{
    char c;
    enum objtype t;
    int colcount=0;

    (*d)++;
    (*l)--;
    while (*l>0)
    {
	c=*(buf+*d);

	/* skip blanks */
	if (c==' ' || c=='\n' || c=='\r' || c=='\t')
	{
	    (*d)++;
	    (*l)--;
	    continue;
	}

	/* found the outher end */
	if (c==']')
	{
	    (*l)--;
	    (*d)++;
	    return T_NOTFOUND;
	}

	/* if not finished, we must have a separator */
	if (colcount!=0)
	{
	    if (c!=',')
		return T_UNKNOWN;
	    (*l)--;
	    (*d)++;
	}

	/* get the field type */
	t=json_doctype(buf, d, l);
	if (t==T_UNKNOWN || t==T_EMPTY)
	    return T_UNKNOWN;

	/* found it */
	if (colcount==element)
	    return t;

	/* get the field */
	switch (t)
	{
	  case T_OBJECT:
	    if (!json_parseobject(buf, d, l, NULL, NULL, NULL, NULL, NULL))
		return T_UNKNOWN;
	    break;
	  case T_ARRAY:
	    if (!json_parsearray(buf, d, l, NULL, NULL, NULL, NULL, NULL))
		return T_UNKNOWN;
	    break;
	  case T_STRING:
	    if (!json_parsestring(buf, d, l, NULL, NULL, NULL))
		return T_UNKNOWN;
	    break;
	  case T_NUM:
	    if (!json_parsenum(buf, d, l, NULL))
		return T_UNKNOWN;
	    break;
	  case T_BOOL:
	    if (!json_parsebool(buf, d, l, NULL))
		return T_UNKNOWN;
	    break;
	  case T_NULL:
	    if (!json_parsenull(buf, d, l, NULL))
		return T_UNKNOWN;
	    break;
	}
	colcount++;
    }
    return T_UNKNOWN;
}

/*
** parse an incoming object
*/
int json_parseobject(char *buf, int *d, int *l, exprstack_t **retval, sqslda_t **sqslda, int *elements, int *count, storage_t *docbuf)
{
    char c;
    enum objtype t;
    int colcount=0;
    sqslda_t name;
    sqslda_t *colda=NULL;
    exprstack_t *colexpr=NULL;

    (*d)++;
    (*l)--;
    while (*l>0)
    {
	c=*(buf+*d);

	/* skip blanks */
	if (c==' ' || c=='\n' || c=='\r' || c=='\t')
	{
	    (*d)++;
	    (*l)--;
	    continue;
	}

	/* found the outher end */
	if (c=='}')
	{
	    (*l)--;
	    (*d)++;
	    return 1;
	}

	/* if not finished, we must have a separator */
	if (colcount!=0)
	{
	    if (c!=',')
		return 0;
	    (*l)--;
	    (*d)++;
	}

	if (json_doctype(buf, d, l)!=T_STRING)
	    return 0;

	/* get the field name */
	if (!json_parsestring(buf, d, l, NULL, &name, docbuf))
	    return 0;

	/* expect a colon */
	for (;;)
	{
	    if (*l==0)
		return 0;
	    c=*(buf+*d);
	    if (c==' ' || c=='\n' || c=='\r' || c=='\t')
	    {
		(*l)--;
		(*d)++;
	    }
	    break;
	}
	if (c!= ':' || *l==0)
	     return 0;
	(*l)--;
	(*d)++;

	/* get the field type */
	t=json_doctype(buf, d, l);
	if (t==T_UNKNOWN || t==T_EMPTY)
	    return 0;

	/* extend the expression list and da */
	if (elements!=NULL && *elements<=colcount)
	{
	    int new_elements = *elements+10;
	    exprstack_t *new_retval;
	    sqslda_t *new_sqslda;

	    new_retval=(exprstack_t *) realloc(*retval, sizeof(exprstack_t)*new_elements);
	    new_sqslda=(sqslda_t *) realloc(*sqslda, sizeof(sqslda_t)*new_elements);
	    if (new_retval==NULL || new_sqslda==NULL)
		return 0;
	    *retval=new_retval;
	    *sqslda=new_sqslda;
	    *elements= new_elements;
	}

	/* store the field name */
	if (sqslda!=NULL)
	{
	    colda=(*sqslda)+colcount;
	    colda->colname=name.sqldata;
	    colda->collen=name.sqllen;

	    /* for object and array processing */
	    colda->sqldata=*d;
	}
	if (retval!=NULL)
	    colexpr=(*retval)+colcount;

	/* get the field */
	switch (t)
	{
	  case T_OBJECT:
	    if (!json_parseobject(buf, d, l, NULL, NULL, NULL, NULL, NULL))
		return 0;
	    if (colexpr!=NULL)
		colexpr->type=CSTRINGTYPE;
	    if (colda!=NULL)
		colda->sqllen=*d-colda->sqllen;
	    break;
	  case T_ARRAY:
	    if (!json_parsearray(buf, d, l, NULL, NULL, NULL, NULL, NULL))
		return 0;
	    if (colexpr!=NULL)
		colexpr->type=CSTRINGTYPE;
	    if (colda!=NULL)
		colda->sqllen=*d-colda->sqllen;
	    break;
	  case T_STRING:
	    if (!json_parsestring(buf, d, l, colexpr, colda, docbuf))
		return 0;
	    break;
	  case T_NUM:
	    if (!json_parsenum(buf, d, l, colexpr))
		return 0;
	    break;
	  case T_BOOL:
	    if (!json_parsebool(buf, d, l, colexpr))
		return 0;
	    break;
	  case T_NULL:
	    if (!json_parsenull(buf, d, l, colexpr))
		return 0;
	    break;
	}
	colcount++;
	if (count!=NULL)
	    (*count)++;
    }
    return 0;
}

/*
** parse an incoming array
*/
int json_parsearray(char *buf, int *d, int *l, exprstack_t **retval, sqslda_t **sqslda, int *elements, int *count, storage_t *docbuf)
{
    char c;
    enum objtype t;
    int colcount=0;
    sqslda_t *colda=NULL;
    exprstack_t *colexpr=NULL;

    (*d)++;
    (*l)--;
    while (*l>0)
    {
	c=*(buf+*d);

	/* skip blanks */
	if (c==' ' || c=='\n' || c=='\r' || c=='\t')
	{
	    (*d)++;
	    (*l)--;
	    continue;
	}

	/* found the outher end */
	if (c==']')
	{
	    (*l)--;
	    (*d)++;
	    return 1;
	}

	/* if not finished, we must have a separator */
	if (colcount!=0)
	{
	    if (c!=',')
		return 0;
	    (*l)--;
	    (*d)++;
	}

	/* get the field type */
	t=json_doctype(buf, d, l);
	if (t==T_UNKNOWN || t==T_EMPTY)
	    return 0;

	/* extend the expression list and da */
	if (elements!=NULL && *elements<=colcount)
	{
	    int new_elements = *elements+10;
	    exprstack_t *new_retval;
	    sqslda_t *new_sqslda;

	    new_retval=(exprstack_t *) realloc(*retval, sizeof(exprstack_t)*new_elements);
	    new_sqslda=(sqslda_t *) realloc(*sqslda, sizeof(sqslda_t)*new_elements);
	    if (new_retval==NULL || new_sqslda==NULL)
		return 0;
	    *retval=new_retval;
	    *sqslda=new_sqslda;
	    *elements= new_elements;
	}

	if (sqslda!=NULL)
	{
	    colda=(*sqslda)+colcount;

	    /* for object and array processing */
	    colda->sqldata=*d;
	}
	if (retval!=NULL)
	    colexpr=(*retval)+colcount;

	/* get the field */
	switch (t)
	{
	  case T_OBJECT:
	    if (!json_parseobject(buf, d, l, NULL, NULL, NULL, NULL, NULL))
		return 0;
	    if (colexpr!=NULL)
		colexpr->type=CSTRINGTYPE;
	    if (colda!=NULL)
		colda->sqllen=*d-colda->sqllen;
	    break;
	  case T_ARRAY:
	    if (!json_parsearray(buf, d, l, NULL, NULL, NULL, NULL, NULL))
		return 0;
	    if (colexpr!=NULL)
		colexpr->type=CSTRINGTYPE;
	    if (colda!=NULL)
		colda->sqllen=*d-colda->sqllen;
	    break;
	  case T_STRING:
	    if (!json_parsestring(buf, d, l, colexpr, colda, docbuf))
		return 0;
	    break;
	  case T_NUM:
	    if (!json_parsenum(buf, d, l, colexpr))
		return 0;
	    break;
	  case T_BOOL:
	    if (!json_parsebool(buf, d, l, colexpr))
		return 0;
	    break;
	  case T_NULL:
	    if (!json_parsenull(buf, d, l, colexpr))
		return 0;
	    break;
	}
	colcount++;
	if (count!=NULL)
	     (*count)++;
    }
    return 0;
}

/*
** parse an incoming string
*/
int json_parsestring(char *buf, int *d, int *l, exprstack_t *retval, sqslda_t *sqslda, storage_t *docbuf)
{
    char c;
    int i, u;
    int copy_from=*d+1;
    int original=copy_from;
    int string_len=0;
    int string_start;
    int surrogate=-1, surrogate_start=-1;

    if (*l<2 || (*(buf+*d))!='"')
	return 0;

    if (docbuf!=NULL)
	string_start=docbuf->buflen;

    /* look for the other side */
    while (*l>0)
    {
	(*l)--;
	(*d)++;
	switch (*(buf+*d))
	{

	  /* found the other side */
	  case '"':

	    /* we had an initial surrogate code point, never seen the second */
	    if (surrogate_start>=0)
		return 0;
	    if (retval!=NULL)
	        retval->type=CSTRINGTYPE;

	    if (docbuf!=NULL && sqslda!=NULL)
	    {

		/* TODO fast allocation for string without special characters */
		/* copy reminder */
		if (sqd_pushstring(docbuf, buf+copy_from, *d-copy_from, 0, 0) < 0)
		    return 0;

		/* and terminate */
		if (sqd_pushstring(docbuf, "", -1, 1, 0) < 0)
		    return 0;
		sqslda->sqldata=string_start;
		sqslda->sqllen=string_len;
	    }
	    else if (sqslda!=NULL)
	    {
		sqslda->sqldata=original;
		sqslda->sqllen=string_len;
	    }
	    (*l)--;
	    (*d)++;
	    return 1;
	  case '\\':

	    /* we have to have at least another character */
	    if (*l==0)
		return 0;

	    /* copy outstanding string */
	    if (sqslda!=NULL && surrogate<0 &&
		copy_from>0 && sqd_pushstring(docbuf, buf+copy_from, *d-copy_from, 0, 0) < 0)
		return 0;

	    (*d)++;
	    (*l)--;
	    switch (*(buf+*d))
	    {
	      case '"':
	      case '\\':
	      case '/':
		copy_from = *d;
		break;
	      case 'b':
		if (sqslda!=NULL && sqd_pushstring(docbuf, "\b", 1, 0, 0) < 0)
		    return 0;
		copy_from = *d+1;
		break;
	      case 'f':
		if (sqslda!=NULL && sqd_pushstring(docbuf, "\f", 1, 0, 0) < 0)
		    return 0;
		copy_from = *d+1;
		break;
	      case 'n':
		if (sqslda!=NULL && sqd_pushstring(docbuf, "\n", 1, 0, 0) < 0)
		    return 0;
		copy_from = *d+1;
		break;
	      case 't':
		if (sqslda!=NULL && sqd_pushstring(docbuf, "\t", 1, 0, 0) < 0)
		    return 0;
		copy_from = *d+1;
		break;

	      /* libcouchbase handles unicode code points, but for completenes */
	      case 'u':
		if (*l<4)
		    return 0;
		u=0;
		for (i=0; i<4; i++)
		{
		    (*d)++;
		    (*l)--;
		    c=*(buf+*d);
		    if (c>='0' && c<='9')
			u=u*16+c-'0';
		    else if (c>='A' && c<='F')
			u=u*16+c-'A'+10;
		    else if (c>='a' && c<='f')
			u=u*16+c-'a'+10;
		    else
			return 0;
		}

		// first of a pair
		if (surrogate<0)
		{
		    if (u>=0xD800 && u<=0xDBFF)
		    {
		 	surrogate=u;
			surrogate_start=*d;
		 	break;

		    }

		    /* invalid unicode */
		    else if (u>=0xDC00 && u<=0xDFFF)
			return 0;
		}
		else
		{
		    /* invalid unicode */
		    if (u<0xDC00 || u>0xDFFF)
			return 0;

		    /* 2nd codepoint does't immediately follow first */
		    if (*d!=surrogate_start+6)
			return 0;
		}
		if (sqslda!=NULL)
		{
		    if (surrogate>=0)
		    {
			if (json_encodesurrogate(docbuf, surrogate, u)<0)
		    	    return 0;
		    }
		    else if (json_encodeunicode(docbuf, u)<0)
			return 0;
		}
		copy_from=*d+1;
		surrogate=-1;
		surrogate_start=-1;
		break;
	      default:

		/* no control characters */
		if (c<' ')
		    return 0;
	    }
	    break;
	}
	string_len++;
    }

    return 0;
}

/*
** encode a unicode rune
*/
int json_encodesurrogate(storage_t *docbuf, int surrogate, int u)
{
    return json_encodeunicode(docbuf, 0x10000+(((surrogate&0x3FF) << 10)|(u&0x3FF)));
}

int json_encodeunicode(storage_t *docbuf, int u)
{
    char c[4];
    int l;

    /* ascii */
    if (u<0x80)
        return sqd_pushbyte(docbuf, (char) (u&0x7F));

    /* two bytes */
    else if (u<0x800)
    {
	c[1]=(u&0xBF)|0x80;
	u >>=6;
        c[0]=(u&0x1F)|0xC0;
	l=2;
    }

    /* three */
    else if (u<0x10000)
    {
	c[2]=(u&0xBF)|0x80;
	u >>=6;
	c[1]=(u&0xBF)|0x80;
	u >>=6;
        c[0]=(u&0x0F)|0xE0;
	l=3;
    }

    /* four */
    else if (u <= 0x1FFFFF)
    {
	c[3]=(u&0xBF)|0x80;
	u >>=6;
	c[2]=(u&0xBF)|0x80;
	u >>=6;
	c[1]=(u&0xBF)|0x80;
	u >>=6;
        c[0]=(u&0x07)|0xF0;
	l=4;
    }
    else
	return -1;
    return sqd_pushstring(docbuf, (char *) c, l, 0, 0);
}

/*
** parse an incoming boolean
*/
int json_parsebool(char *buf, int *d, int *l, exprstack_t *retval)
{
    char c;

    if (*l == 0)
	return 0;

    if (*l>=4 && !fgw_strncasecmp((buf+*d), "true", 4))
    {
	if (retval!=NULL)
	{
	    retval->type=CINTTYPE;
	    retval->val.intvalue=0;
	    retval->length=sizeof(int);
	}
	(*l)-=4;
	(*d)+=4;
    }
    else if (*l>=5 && !fgw_strncasecmp((buf+*d), "false", 5))
    {
	if (retval!=NULL)
	{
	    retval->type=CINTTYPE;
	    retval->val.intvalue=0;
	    retval->length=sizeof(int);
	}
	(*l)-=5;
	(*d)+=5;
    }
    else
	return 0;

    return 1;
}

/*
** parse an incoming null
*/
int json_parsenull(char *buf, int *d, int *l, exprstack_t *retval)
{
    char c;

    if (*l == 0)
	return 0;

    if (*l>=4 && !fgw_strncasecmp((buf+*d), "null", 4))
    {
	if (retval!=NULL)
	{
	    retval->type=CINTTYPE;
	    retval->length=-1;
	}
	(*l)-=4;
	(*d)+=4;
    }
    else
	return 0;

    return 1;
}

/*
** parse an incoming number
*/
int json_parsenum(char *buf, int *d, int *l, exprstack_t *retval)
{
    char c, *newd;
    int newl;
    double real;

    if (*l == 0)
	return 0;

    if (*l==0)
	return 0;

    real=strtod(buf+*d, &newd);
    newl = (int) (newd-(buf+*d));

    /* conversion did not work out */
    if (real==0.0 && newl==0)
	return 0;

    if (retval!=NULL)
    {
	retval->val.real=real;
	retval->type=CDOUBLETYPE;
	retval->length=sizeof(double);
    }
    (*l)-=newl;
    (*d)+=newl;

    return 1;
}

/*
** skip blanks
*/
int json_skipblanks(char *buf, int *d, int *l)
{
    while (*l > 0)
    {
	switch (*(buf+*d))
	{
	  /* skip blanks */
	  case ' ':
          case '\t':
          case '\r':
          case '\n':
	    (*l)--;
	    (*d)++;
	    continue;
	}
	
	/* extra stuff found! */
	return 1;
    }

    return 0;
}
