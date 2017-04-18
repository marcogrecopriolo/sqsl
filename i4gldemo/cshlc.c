/*
	CSHLC.c  -  OS related routines

	The Structured Query Scripting Language
	(Borrowed from) The 4glWorks application framework
	Copyright (C) 1992-2009 Marco Greco (marco@4glworks.com)

	Initial release: Oct 92
	Current release: Aug 09

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

#ifdef NT
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

/*
**  returns the current process id
*/
int FGLGLOB(get_process)(nargs)
int nargs;
{
    retlong(getpid());
    return(1);
}

/*
**  returns the current user id
*/
int FGLGLOB(get_login)(nargs)
int nargs;
{
    char u[256];

    fgw_username(&u, 256);
    retquote(u);
    return(1);
}
