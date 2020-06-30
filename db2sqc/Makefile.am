#	MAKEFILE.am - db2/sqc source automake defs
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
COMPILE = $(CC) $(CC_RELOC) $(CC_SRC_EXTRA) $(DNEED_XSL) $(DNT) $(SQC_CC_EXTRA) \
	  -I$(top_srcdir)/sqsl -I$(top_srcdir)/oslib \
	  -I"$(SQLLIBDIR)/include" -DFGWSOURCE
sqsldir = $(bindir)

nodist_sqsl_DATA = fgwdb2.bnd
nodist_sqsl_SCRIPTS = fgwdb2sqc$(DLLEXT)
dist_noinst_DATA = db2api.y db2api.c db2src.h db2src.sqc

.y.c:
	$(YACCCMP)

# certain versions of gmake insist in deleting temporary file db2src.c only
# to remake the whole of fgwdb2sqc.so because db2src.c is missing....
# if this is happening to you, change the rule to .sqc.o and append
#
#	$(COMPILE) -c -o $@ $*.c
#
# at the end. The downside is that certain gmakes are unable to see that
# db2src.sqc has changed, and won't produce a new db2src.c
.sqc.c:
	db2 connect to $(DB2BUILDDB); \
	db2 prep $< bindfile using fgwdb2.bnd \
	    blocking no collection fgwdb2cs connect 2 dynamicrules run \
	    datetime iso deferred_prepare no disconnect explicit \
	    insert buf isolation cs \
	    package using fgwdb2 sqlrules std syncpoint onephase; \
	db2 bind fgwdb2.bnd collection fgwdb2cs isolation cs grant public; \
	db2 bind fgwdb2.bnd collection fgwdb2rr isolation rr grant public; \
	db2 bind fgwdb2.bnd collection fgwdb2ur isolation ur grant public; \
	db2 bind fgwdb2.bnd collection fgwdb2rs isolation rs grant public; \
	db2 connect reset; \
	db2 terminate 

# need to have some sort of phony rule for fgwdb2.bnd... or some older SYSV
# make will decide that building target all has failed...
fgwdb2.bnd:
	touch fgwdb2.bnd

# FIXME - work out dependencies
.c.o:
	$(COMPILE) -c -o $@ $<

$(top_srcdir)/libdb2.a:
	nm "$(SQLLIBDIR)/lib/db2api.lib" | $(AWK) \
	    'BEGIN { print "EXPORTS" } \
	     /T _/ { sub("_+", ""); print $$3; } { next }' > \
		$(top_srcdir)/db2.def; \
	dlltool --def $(top_srcdir)/db2.def \
		--dll "$(SQLLIBDIR)/bin/db2app.dll" \
		--output-lib $@

# rats! only gmake supports $^
fgwdb2sqc$(DLLEXT): db2api.o db2src.o $(DB2EXTRA) $(OSLINK) $(OSLIBADD)
	$(DB2LINK) $(LD_SRC_EXTRA) -o $@ db2api.o db2src.o $(DB2EXTRA) \
		$(SQC_LD_EXTRA)

clean-local:
	rm -f *$(DLLEXT) *.o *.bnd
