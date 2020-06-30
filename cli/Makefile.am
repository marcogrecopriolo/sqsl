#	MAKEFILE.am - Command Line Interface automake defs
#
#	The Structured Query Scripting Language
#	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)
#
#	Initial release: Feb 05
#	Current release: Jun 20
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

# FIXME c4gl and esql can't handle -D defines with embedded spaces
COMPILE = $(CC) $(CC_EXTRA) -I$(ESQLINC) -I$(top_srcdir)/sqsl \
	    -DESQLVER=$(ESQLVER) -I$(top_srcdir)/oslib \
	    $(READLINE_DEFINE) $(DNT) -DVERSION="\"$(VERSION)\"" \
	    -DPACKAGE_NAME="\"$(PACKAGE_NAME)\"" \
            -DPACKAGE_DATE="\"$(PACKAGE_DATE)\"" -DCOMP_NAME="\"cli\""

# Have to pass along INFORMIXC and cater for all autoconf amenities...
# CC_LD_STATIC would map to -static on gcc, which clashes with esql/cs -static
CCLD = INFORMIXC="$(INFORMIXC) $(CC_LD_FLAG) $(CC_LD_STATIC) $(LD_EXTRA)"; \
	export INFORMIXC; $(ESQL) $(ESQL_STATIC)
bin_PROGRAMS = cli
cli_SOURCES = cli.c csquc.c csqmc.h
LDADD = $(top_srcdir)/sqsl/libsqsl.a $(OSLINK) $(READLINE_LIBRARY) $(REGEX_LIBRARY)
