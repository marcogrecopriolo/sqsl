#	MAKEFILE.am - OS vagaries libs automake defs
#
#	The Structured Query Scripting Language
#	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)
#
#	Initial release: Feb 05
#	Current release: Jul 16
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

#  oslib uses relocatable code just in case we want to place it in a DLL
COMPILE = $(CC) $(CC_RELOC) $(CC_OSL_EXTRA) $(DNT) -I$(top_srcdir)/sqsl
LINK = $(CC) $(CC_SO_FLAG)
sqsldir = $(bindir)

nodist_sqsl_SCRIPTS = $(OSLIBD)
EXTRA_SCRIPTS = libsqslos$(DLLEXT)
#
# I don't really need the library, but anyway...
#
noinst_LIBRARIES = $(OSLIBS)
EXTRA_LIBRARIES = libsqslos.a
libsqslos_a_SOURCES = cosdc.h coslc.h coslc.c

# rats! only gmake supports $^
libsqslos$(DLLEXT): coslc.o
	$(LINK) -o $@ coslc.o

clean-local:
	rm -f *$(DLLEXT) *.o
