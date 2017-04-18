/*
	CSQHC.h  -  HTML format tags

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2002 Marco Greco (marco@4glworks.com)

	Initial release: Oct 02
	Current release: Oct 02

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

/*
** html strings need to be NL terminated to ease wordwrap
*/

#ifndef CSQHC_H
#define H_PRETABLE "<TABLE BORDER>\n"
#define H_POSTTABLE "</TABLE>\n"

#define H_VERTHEADER "<TR><TH VALIGN=baseline ALIGN=left NOWRAP>\n"
#define H_VERTPOSTHEA "</TH>\n"
#define H_VERTFIELD "<TD VALIGN=baseline ALIGN=left WIDTH=100%>\n"
#define H_VERTPOSTFIELD "</TD></TR>\n"

#define H_FULLHEADER "<TH VALIGN=baseline ALIGN=center NOWRAP>\n"
#define H_FULLPOSTHEA "</TH>\n"
#define H_FULLFIELD "<TD VALIGN=baseline ALIGN=center>\n"
#define H_FULLPOSTFIELD "</TD>\n"

#define H_FULLTABLE "<TR><TD COLSPAN=%i><PRE>\n"
#define H_FULLPOSTTABLE "</PRE></TD></TR>\n"

#define CSQHC_H
#endif
