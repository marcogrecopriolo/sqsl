/*
	CB.h  -  SQSL interpreter Couchbase dynamic N1QL driver

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2020 Marco Greco (marco@4glworks.com)

	Initial release: Jun 16
	Current release: Jul 20

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

/* 1 accumulare opzioni in connect */
/* 3 settare opzioni in prepare */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>
#include "cosdc.h"
#include "ccmnc.h"
#include "chstc.h"
#include "csqrc.h"
#include "crxuc.h"
#include "crqxd.h"
#include "cxslc.h"

enum objtype { T_OBJECT, T_ARRAY, T_STRING, T_NUM, T_BOOL, T_NULL, T_UNKNOWN, T_EMPTY, T_NOTFOUND };

typedef struct {
    char *buf;
    int buflen;
    int bufmax;
} storage_t;

typedef struct sqslda
{
    int sqldata;        /* offset into parse buffer for data */
    int sqllen;         /* data length */
    int colname;        /* offset into parse buffer for name */
    int collen;         /* field name length */
} sqslda_t;

enum objtype json_doctype(char *buf, int *d, int *l);
enum objtype json_searchobject(char *field, char *buf, int *d, int *l);
enum objtype json_searcharray(int element, char *buf, int *d, int *l);
int json_parseobject(char *buf, int *d, int *l, exprstack_t **retval, sqslda_t **sqslda, int *values, int *count, storage_t *docbuf);
int json_parsearray(char *buf, int *d, int *l, exprstack_t **retval, sqslda_t **sqslda, int *values, int *count, storage_t *docbuf);
int json_parsestring(char *buf, int *d, int *l, exprstack_t *retval, sqslda_t *sqslda, storage_t *docbuf);
int json_parsebool(char *buf, int *d, int *l, exprstack_t *retval);
int json_parsenull(char *buf, int *d, int *l, exprstack_t *retval);
int json_parsenum(char *buf, int *d, int *l, exprstack_t *retval);
int json_skipblanks(char *buf, int *d, int *l);
int json_encodesurrogate(storage_t *docbuf, int surrogate, int u);
int json_encodeunicode(storage_t *docbuf, int u);

int sqd_pushstring(storage_t *store, const char *data, int len, int term, int escape);
int sqd_pushbyte(storage_t *store, char data);

