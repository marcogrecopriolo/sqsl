#	MAKEFILE.am - couchbase source automake defs
#
#	The Structured Query Scripting Language
#	Copyright (C) 1992-2019 Marco Greco (marco@4glworks.com)
#
#	Initial release: Jun 16
#	Current release: Mar 19
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

#
# I know about libtool, and I know this is a dirty hack, but heck, automake is
# the perfect example of an inflexible tool unwilling to facilitate tasks.
# I want to name shared objects my way, place them where I want and require
# flexible extensions, all of which neither libtool nor automake, for that
# matter, are able to provide.
# With bin_PROGRAMS, EXEEXT gets in the way. LIBRARIES can't go in bin and 
# and get built statically anyway. This is the least dirty option.
#

# FIXME these has to come from configure
sqslsrcdir = $(top_srcdir)
sqsldir = $(bindir)
LINKCMD = $(CC) $(CC_SO_FLAG)

COMPILE = $(CC) $(CC_RELOC) $(CC_SRC_EXTRA) $(DNT) -DFGWSOURCE \
	  $(DNEED_XSL) -I$(sqslsrcdir)/sqsl -I$(sqslsrcdir)/oslib


nodist_sqsl_SCRIPTS = fgwcb$(DLLEXT)
dist_noinst_DATA = cb.c cb.h decode.c

# FIXME - work out dependencies
.c.o:
	$(COMPILE) -I $(CBINCLDIR) -c -o $@ $<

#rats! only gmake supports $^
#order is important!
fgwcb$(DLLEXT): cb.o decode.o $(CBEXTRA)
	$(LINKCMD) $(LD_SRC_EXTRA) -L $(CBLIBDIR) -lcouchbase -o $@ cb.o decode.o $(CBEXTRA)

clean-local:
	rm -f *$(DLLEXT) *.o
