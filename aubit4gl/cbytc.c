/*
	CBYTC.c  -  AUBIT 4gl byte synchronization functions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2012 Marco Greco (marco@4glworks.com)

	Initial release: Jan 09
	Current release: Dec 12

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

#include "ctypc.h"
#include <a4gl_incl_4glhdr.h>
#define FGW_LOC_USER_ENV	0x01000000
#define FGW_LOC_USER_KEY	0x60000000
#define FGW_LOC_USER_MASK	0xf0000000
#define FGW_LOC_SIZE_MASK	0x00ffffff

/*
** build locator from aubit blob
*/
fgw_copylocator(loc, byte)
fgw_loc_t *loc;
fglbyte **byte;
{
    fglbyte blob;
    int s, t;

    A4GL_get_top_of_stack(1, &t, &s, byte);
    A4GL_pop_var2(&blob, t, 0);
    memset(loc, 0, sizeof(fgw_loc_t));
/*
 * the popped value is unreliable (depending on whether it's a null blob
 * or not) hence we use the pointer at the top of the stack
 */
    loc->loc_loctype=((*byte)->where=='M')? LOCMEMORY:
					(((*byte)->where=='F')? LOCFILE: 0);

/*  no loc_fname -- we don't do file */
    loc->loc_type=((t & 255) == 12)? SQLTEXT:
				     (((t & 255) == 11)? SQLBYTES: 0);
    loc->loc_bufsize=(*byte)->loc_user_bufsize & FGW_LOC_SIZE_MASK;
    loc->loc_size=(*byte)->memsize;
    loc->loc_indicator=((*byte)->isnull=='Y');
    loc->loc_buffer=(char *) (*byte)->ptr;
    loc->loc_user_env=(*byte)->loc_user_env;
    if (((*byte)->loc_user_bufsize & FGW_LOC_USER_MASK) == FGW_LOC_USER_KEY)
    {
	loc->loc_mflags=FGW_LOC_USER_ENV;
	loc->loc_oflags=FGW_LOC_USER_ENV;
    }
}

/*
** restore aubit blob from locator
*/
fgw_restorelocator(loc, blob)
fgw_loc_t *loc;
fglbyte *blob;
{
    A4GL_assertion(blob==NULL, "blob locator not initialized");
    blob->where=(loc->loc_loctype==LOCMEMORY)? 'M': 'F';
/*  no filename -- we don't do file */
/*  no loc type */
    blob->memsize=loc->loc_size;
    blob->loc_user_bufsize=loc->loc_bufsize;
    blob->isnull=(loc->loc_buffer)? 'N': 'Y';
    blob->ptr=loc->loc_buffer;
    blob->loc_user_env=loc->loc_user_env;
    if ((loc->loc_oflags & FGW_LOC_USER_ENV) &&
	(loc->loc_mflags & FGW_LOC_USER_ENV))
	blob->loc_user_bufsize |= FGW_LOC_USER_KEY;
}
