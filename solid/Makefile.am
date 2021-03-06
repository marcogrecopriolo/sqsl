#	MAKEFILE.am - solid source automake defs
#
#	The Structured Query Scripting Language
#	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)
#
#	Initial release: Feb 05
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
COMPILE = $(CC) $(CC_RELOC) $(CC_SRC_EXTRA) $(SOLIDDEFS) $(DNT) -DFGWSOURCE \
	  $(DNEED_XSL) -I$(top_srcdir)/sqsl -I$(top_srcdir)/oslib "-I$(SOLIDDIR)/include"
sqsldir = $(bindir)

nodist_sqsl_SCRIPTS = fgwsolid$(DLLEXT)
dist_noinst_DATA = solid.c

# FIXME - work out dependencies
.c.o:
	$(COMPILE) -c -o $@ $<

$(top_srcdir)/solidimpodbcu.a:
	nm "$(SOLIDDIR)/lib/solidimpodbcu.lib" | $(AWK) \
	    'BEGIN { print "EXPORTS" } \
	     /T _/ { sub("_+", ""); print $$3; sub("@.*", ""); print $$3; } \
	     { next }' > $(top_srcdir)/solid.def; \
	dlltool --def $(top_srcdir)/solid.def \
	        --dll "$(SOLIDDLL)" \
	        -k --output-lib $@

#rats! only gmake supports $^
#order is important!
fgwsolid$(DLLEXT): solid.o $(SOLIDEXTRA)
	$(SOLIDLINK) $(LD_SRC_EXTRA) -o $@ solid.o $(SOLIDEXTRA)

clean-local:
	rm -f *$(DLLEXT) *.o
