/*
	COSDC.h  -  OS dependent definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Jun 09
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

#ifndef COSDC_H

/*
** Microsoft Visual Studio
*/
#ifdef MVS
#include <windows.h>
#define DLLDECL __declspec(dllexport)

/* hack alert! no unistd.h - trick XSL into compiling */
#define pid_t int
#define ssize_t long
/*
** everything else
*/
#else
#define DLLDECL
#include "unistd.h"
#include "sys/types.h"
#endif

/*
** for old time's sake
*/
#undef IGNORE			/* some MinGW have it, I should move to FGW_IGNORE */
#ifdef __STDC__
#define IGNORE (void)
#else
#define IGNORE
#define void int
#define unsigned
#endif

#define COSDC_H
#endif
