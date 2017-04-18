/*
	CASCC.h  -  ASCII text manipulation functions, definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2009 Marco Greco (marco@4glworks.com)

	Initial release: Nov 98
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

#ifndef CASCC_H
#include "ctypc.h"

typedef struct
{
    int key;			/* just to make sure it's our type of blobs */
    void *signature;		/* ...and the right blob too */
    int size;			/* size of the textindex array */
    int nolines;		/* lines in blob */
/*
** textindex[0] is always 0
** textindex[nolines] is always loc_t->loc_size
** simplifies logic, and could be used for further consistency checks
*/
    int textindex[1];		/* index of each line */
} fgw_texttype;

#define STARTTEXTSIZE 0x8000
#define STARTLINESIZE 500
#define MAXSTRINGSIZE 512
#define TEXTKEY 0x4d4700	/* builds up my ego! */

/*
** Aubit 4gl does not pass blob by reference hence there isn't a
** direct link to the original locator. loc_buffer is not as foolproof
** but it will have to do
*/
#ifdef A4GL
#define SIGNATURE(x)		((char *) x->loc_buffer)
#else
#define SIGNATURE(x)		((char *) x)
#endif

#define fgw_text(arg) ((fgw_texttype *) arg->loc_user_env)

extern void fgw_freetext(fgw_loc_t *textvar);
extern void fgw_locatetext(fgw_loc_t *textvar);

extern void fgw_recompindex(fgw_loc_t *textvar, int idx);
extern int fgw_resizebuffer(fgw_loc_t *textvar, int sz);
#define CASCC_H
#endif
