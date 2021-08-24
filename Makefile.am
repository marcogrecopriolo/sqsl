#	MAKEFILE.am - Master automake defs
#
#	The Structured Query Scripting Language
#	Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)
#
#	Initial release: Feb 05
#	Current release: Aug 21
#
#	This library is free software; you can redistribute it and/or
#	modify it under the terms of the GNU Lesser General Public
#	License as published by the Free Software Foundation; either
#	version 2.1 of the License, or (at your option) any later version.
#
#	This library is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#	Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General Public
#	License along with this library; if not, write to the Free Software
#	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

SUBDIRS = oslib sqsl cli $(QTDEMO) $(I4GLUI) $(A4GLUI) \
	  $(DB2SO) $(IFMXSO) $(SOLIDSO) $(NETSO) $(CBSO) json \
	  doc demo test tools tools/mvs
DIST_SUBDIRS = sqsl oslib cli i4gl i4gldemo aubit4gl aubitdemo \
	       ifmx ifmx4 db2sqc db2cli db2esql solid net cb json \
	       doc demo test tools tools/mvs
topdir = $(exec_prefix)

dist_noinst_DATA = linkslist configure configure.ac INSTALL README.md
dist_top_DATA = COPYING README
