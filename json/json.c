/*
	json.c  -  SQSL interpreter json functions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Jun 16
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

#include <string.h>
#include <stdio.h>
#include "cosdc.h"
#include "csqrc.h"
#include "crxuc.h"
#include "crqxd.h"
#include "cxslc.h"

#define KEYSIZE 250
#define JSONQUOTE "\""
#define JSONQUOTECHAR '\"'

XSL_STATE

/*
**  initialize shared object
*/
DLLDECL int sqd_init(XSL_PARMS)
{
    XSL_SETSTATE
    return SO_MASK|SO_XPRFLG;
}

/* FIXME improve to use strncat if no escape necessary */
static void key_pushstring(exprstack_t *o, char *src, int len, int escape)
{
    char *dest=o->val.string+o->length;

    if (escape)
    {
	while (*src && (o->count--)>0)
	{
	    if (*src==JSONQUOTECHAR)
	    {
		*dest++=' ';
		src++;
	    }
	    else
		*dest++=*src++;
	    o->length++;
	}
	*dest='\0';
    }
    else
    {
	int sz=o->count>len? len: o->count;

	strncat(dest, src, sz);
	o->length+=sz;
	o->count-=sz;
    }
}

/*
** given a separator and some expressions, produce a key
*/
DLLDECL int key(fgw_stacktype *stk)
{
    exprstack_t *sep, *fld, o;
    char *strsep, *strfld;
    char sepcb[STRINGCONVSIZE], fldcb[STRINGCONVSIZE];
    int fldlen, seplen, sepesc;
    int ac, rc=0, isnull;

    sep=XSL_CALL(rxu_arglist)(stk, &ac);
    memset(&o, 0, sizeof(o));

    /* need to at least have 2 parameters */
    if (ac<2)
    {
	rc=RC_BFINV;
	goto exit;
    }

    /* and some memory */
    if ((o.val.string=XSL_CALL(rxu_malloc)(KEYSIZE))==NULL)
    {
	rc=RC_NOMEM;
	goto exit;
    }
    *o.val.string='\0';
    o.type=CSTRINGTYPE;
    o.count=KEYSIZE-1;

    /* first argument is the separator */
    strsep=XSL_CALL(rxu_tostring)(sep, (char *) &sepcb, &isnull);
    if (isnull)
	strsep="";
    seplen=strlen(strsep);
    sepesc=(strchr(strsep, JSONQUOTECHAR)!=NULL);

    /* the rest is fields */
    fld=sep+1;

    /* add fields, while there is space */
    while (--ac && o.count>0)
    {
	int escape=(fld->type!=CINTTYPE &&
		    fld->type!=CDOUBLETYPE &&
		    fld->type!=CDECIMALTYPE);
	strfld=XSL_CALL(rxu_tostring)(fld, (char *) &fldcb, &isnull);
	if (!isnull)
	{
	    fldlen=strlen(strfld);
	    key_pushstring(&o, strfld, fldlen, escape);
	}

	/* and a separator, if not the last field */
	if (ac>1)
	    key_pushstring(&o, strsep, seplen, sepesc);
	fld++;
    }


    o.count=0;
    XSL_CALL(rxu_pushresult)(stk, &o);
    return 0;

exit:
    XSL_CALL(rxu_setnull)(&o, CSTRINGTYPE);
    o.count=0;
    XSL_CALL(rxu_pushresult)(stk, &o);
    return rc;
}

/* appends a string into a string expression */
static int doc_pushstring(exprstack_t *o, char *src, int len, int escape)
{
    int size=(len+1>KEYSIZE)? len+1: KEYSIZE;

    if (len<=0)
	return 0;
    if (o->count<o->length+len+1)
    {
	char *n=XSL_CALL(rxu_realloc)(o->val.string, o->count+size);

	if (!n)
	    return RC_NOMEM;
	o->val.string=n;
	o->count+=size;
    }
    if (escape)
    {
	int l;
	char *s, *d;

	for (l=len, s=src, d=o->val.string+o->length;
	     l; l--)
	{
	    if (*s==JSONQUOTECHAR)
	    {
		size++;
		len++;
		if (o->count<o->length+len+1)
		{
		    char *n=XSL_CALL(rxu_realloc)(o->val.string, o->count+size);

		    if (!n)
			return RC_NOMEM;
		    o->val.string=n;
		    o->count+=size;
		}
		*d++='\\';
	    }
	    *d++=*s++;
	}
	*d='\0';
    }
    else
	strncat(o->val.string+o->length, src, len);
    o->length+=len;
    return 0;
}

/*
** given a list of labels and values, produce a basic json document
*/
DLLDECL int row2doc(fgw_stacktype *stk)
{
    exprstack_t *lbl, *fld, o;
    char *strfld;
    char fldcb[STRINGCONVSIZE];
    int fldlen;
    int ac, rc=0, isnull;
    int osize=0;
    int f;

    lbl=XSL_CALL(rxu_arglist)(stk, &ac);
    memset(&o, 0, sizeof(o));
    o.type=CSTRINGTYPE;

    /* need to at least have 2 parameters, and an even number of parameters */
    if (ac<2 || (ac & 1)==1)
    {
	rc=RC_BFINV;
	goto exit;
    }

    for (f=ac, fld=lbl; f; f--, fld++)
	if (fld->length>0)
	    osize+=fld->length;
    if (osize==0)
	osize=KEYSIZE;
	
    /* and some memory */
    if ((o.val.string=XSL_CALL(rxu_malloc)(osize*2))==NULL)
    {
	rc=RC_NOMEM;
	goto exit;
    }
    *o.val.string='\0';

    /* HACK ALERT we use count as a temporary size tracker */
    o.count=osize*2;

    /* first half is labels, second half values */
    ac/=2;
    fld=lbl+ac;

    /* open the object */
    if ((rc=doc_pushstring(&o, "{ ", 2, 0)))
	goto cleanup;

    /* add fields */
    for (f=1; f<=ac; f++, fld++, lbl++)
    {

	/* first append the label */
	strfld=XSL_CALL(rxu_tostring)(lbl, (char *) &fldcb, &isnull);

	/* if no label, just give it a number */
	if (isnull)
	{
	    strfld=(char *) &fldcb;
	    sprintf(strfld, "$%i", f);
	}
	if ((rc=doc_pushstring(&o, JSONQUOTE, 1, 0)))
	    goto cleanup;
	if ((rc=doc_pushstring(&o, strfld, strlen(strfld), 1)))
	    goto cleanup;
	if ((rc=doc_pushstring(&o, JSONQUOTE, 1, 0)))
	    goto cleanup;
	if ((rc=doc_pushstring(&o, ": ", 2, 0)))
	    goto cleanup;

	/* and then the value */
	strfld=XSL_CALL(rxu_tostring)(fld, (char *) &fldcb, &isnull);
	if (isnull)
	{
	    if ((rc=doc_pushstring(&o, "null", 4, 0)))
		goto cleanup;
	}
	else switch (fld->type)
	{
	  case CINTTYPE:
	  case CDOUBLETYPE:
	  case CDECIMALTYPE:
	    if ((rc=doc_pushstring(&o, strfld, strlen(strfld), 0)))
		goto cleanup;
	    break;
	  default:
	    if ((rc=doc_pushstring(&o, JSONQUOTE, 1, 0)))
		goto cleanup;
	    if ((rc=doc_pushstring(&o, strfld, strlen(strfld), 1)))
		goto cleanup;
	    if ((rc=doc_pushstring(&o, JSONQUOTE, 1, 0)))
		goto cleanup;
	}

	/* finally add a comma if needed */
	if (f<ac && (rc=doc_pushstring(&o, ", ", 2, 0)))
	     goto cleanup;
    }

    /* and complete the object */
    if ((rc=doc_pushstring(&o, " }", 2, 0)))
	goto cleanup;

    /* undo count hack */
    o.count=0;
    XSL_CALL(rxu_pushresult)(stk, &o);
    return 0;

cleanup:
    o.count=0;
    XSL_CALL(rxu_setnull)(&o, CSTRINGTYPE);
    
exit:
    XSL_CALL(rxu_pushresult)(stk, &o);
    return rc;
}

/*
** given a list of values, produce a basic json array
*/
DLLDECL int row2array(fgw_stacktype *stk)
{
    exprstack_t *fld, *cf, o;
    char *strfld;
    char fldcb[STRINGCONVSIZE];
    int fldlen;
    int ac, rc=0, isnull;
    int osize=0;
    int f;

    fld=XSL_CALL(rxu_arglist)(stk, &ac);
    memset(&o, 0, sizeof(o));
    o.type=CSTRINGTYPE;

    /* need to at least have 1 parameter */
    if (ac<1)
    {
	rc=RC_BFINV;
	goto exit;
    }

    for (f=ac, cf=fld; f; f--, cf++)
	if (cf->length>0)
	    osize+=cf->length;
    if (osize==0)
	osize=KEYSIZE;
	
    /* and some memory */
    if ((o.val.string=XSL_CALL(rxu_malloc)(osize))==NULL)
    {
	rc=RC_NOMEM;
	goto exit;
    }
    *o.val.string='\0';

    /* HACK ALERT we use count as a temporary size tracker */
    o.count=osize;

    /* open the array */
    if ((rc=doc_pushstring(&o, "[ ", 2, 0)))
	goto cleanup;

    /* add fields */
    for (f=1; f<=ac; f++, fld++)
    {

	/* append the value */
	strfld=XSL_CALL(rxu_tostring)(fld, (char *) &fldcb, &isnull);
	if (isnull)
	{
	    if ((rc=doc_pushstring(&o, "null", 4, 0)))
		goto cleanup;
	}
	else switch (fld->type)
	{
	  case CINTTYPE:
	  case CDOUBLETYPE:
	  case CDECIMALTYPE:
	    if ((rc=doc_pushstring(&o, strfld, strlen(strfld), 0)))
		goto cleanup;
	    break;
	  default:
	    if ((rc=doc_pushstring(&o, JSONQUOTE, 1, 0)))
		goto cleanup;
	    if ((rc=doc_pushstring(&o, strfld, strlen(strfld), 1)))
		goto cleanup;
	    if ((rc=doc_pushstring(&o, JSONQUOTE, 1, 0)))
		goto cleanup;
	}

	/* finally add a comma if needed */
	if (f<ac && (rc=doc_pushstring(&o, ", ", 2, 0)))
	     goto cleanup;
    }

    /* and complete the object */
    if ((rc=doc_pushstring(&o, " ]", 2, 0)))
	goto cleanup;

    /* undo count hack */
    o.count=0;
    XSL_CALL(rxu_pushresult)(stk, &o);
    return 0;

cleanup:
    o.count=0;
    XSL_CALL(rxu_setnull)(&o, CSTRINGTYPE);
    
exit:
    XSL_CALL(rxu_pushresult)(stk, &o);
    return rc;
}

/*
 * ** returns error message
 * */
DLLDECL void sqd_errmsg(int err, char *buf, int len)
{
     /* we don't issue ad hoc errors */
}

