/*
	IUSRC.c  -  Consolle IO routines

	The 4glWorks application framework
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
#include "ccmnc.h"
#include "ccmpc.h"

/* FIXME showhelp does not set any error currently */

/*
**  tests for "missing help message" error
*/
FGLGLOB(usr_testhelp)(nargs)
int nargs;
{
    int r;

    pushint(1);
    return(1);
}
