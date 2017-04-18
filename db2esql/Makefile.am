#	MAKEFILE.am - DB2/ifmx esqlc source automake defs
#
#	The Structured Query Scripting Language
#	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)
#
#	Initial release: Jan 09
#	Current release: Jan 17
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

# Have to pass along INFORMIXC and cater for all autoconf amenities...
COMPILE = INFORMIXC="$(INFORMIXC)"; export INFORMIXC; \
	  $(ESQL) $(CC_RELOC) $(CC_SRC_EXTRA) $(DNEED_XSL) $(DNT) \
	  -I$(top_srcdir)/sqsl -I$(top_srcdir)/oslib

# esql/c on *ix relies on $INSTHOME which is seldom set
# we help it out passing the db2 lib directory along
LINK = INFORMIXC="$(INFORMIXC) $(CC_SO_FLAG) $(LD_SRC_EXTRA)"; \
       export INFORMIXC; $(ESQL) $(DNT) "-L$(SQLLIBDIR)/lib"

# change noinst to sqsl for automatic installation
nodist_noinst_SCRIPTS = fgwdb2esql$(DLLEXT)
dist_noinst_DATA = csqdc.ec

.ec.o:
	$(COMPILE) -db2 -n -nowarn -DESQLVER=$(ESQLVER) -c -o $@ $<
	rm $*.c

# rats! only gmake supports $^
fgwdb2esql$(DLLEXT): csqdc.o
	$(LINK) -db2 -o $@ csqdc.o

clean-local:
	rm -f *$(DLLEXT) *.o
