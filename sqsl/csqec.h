/*
	CSQEC.h  -  E/C specific abilities

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2002 Marco Greco (marco@4glworks.com)

	Initial release: Jan 97
	Current release: Jan 02

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

#ifndef CSQEC_H
#include <sqlstype.h>

#ifndef SQ_UNKNOWN
#define NEED_DTINVS		/* many dt & inv functs exists only
				   from esql/c 6 onwards */
#endif

#ifdef SQ_CREPROC
#define HAS_DTFMT		/* ????fmtasc exist from esql/5 onwards */
#endif

#define CSQEC_H
#endif
