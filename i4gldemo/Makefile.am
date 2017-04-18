#	MAKEFILE.am - Informix 4gl curses interface demo automake defs
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

INCLUDES = -I$(top_srcdir)/i4gl -I$(top_srcdir)/oslib

# Have to pass along INFORMIXC and cater for all autoconf amenities...
COMPILE = INFORMIXC="$(INFORMIXC)"; export INFORMIXC; $(C4GL) $(INCLUDES) $(DNT)
CCLD = INFORMIXC="$(INFORMIXC) $(CC_LD_FLAG) $(CC_LD_STATIC)" c4gl $(ESQL_STATIC)
sqsldir = $(bindir)

bin_PROGRAMS = curses.4ge
nodist_sqsl_DATA = hl1w.frm hl2w.frm hl3w.frm hl4w.frm sqlf.frm expw.frm \
		      shlw.frm curses.hlp
dist_sqsl_DATA = csql.txt ishl.txt curses.txt
dist_sqsl_SCRIPTS = curses
dist_noinst_DATA = hl1w.per hl2w.per hl3w.per hl4w.per sqlf.per expw.per \
		      shlw.per curses.msg
curses_4ge_SOURCES = ccnvc.c cstrc.c ihorm.4gl imnhl.4gl ishll.4gl iun2h.4gl \
	       iusrl.4gl cmatc.c cstrl.4gl iminv.4gl imnuc.c isqlg.4gl \
	       isqlv.4gl iunih.4gl cshlc.c ctxtl.4gl iml2h.4gl imnul.4gl \
	       iunis.4gl cfnml.4gl cfnml.4gh ierrl.4gl imlth.4gl \
	       ipanl.4gl itedl.4gl iusrc.c csqll.4gh idefl.4gh iunih.4gh \
	       iexpl.4gl csquc.c
LDADD = $(top_srcdir)/i4gl/libsqsli4gl.a $(OSLINK) $(REGEX_LIBRARY)
tmpdir = $(top_srcdir)/tmp
toolsdir = $(top_srcdir)/tools

.4gl.o:
	$(AWK) -f $(toolsdir)/fglpp $(AWKDEL) $(DNT) \
				-DVERSION="'$(VERSION)'" \
				-DPACKAGE_NAME="'$(PACKAGE_NAME)'" \
				-DPACKAGE_DATE="'$(PACKAGE_DATE)'" \
				-DCOMP_NAME="'i4gldemo'" \
				-DSQ_freecurs="$(SQ_freecurs)" \
		$< $(tmpdir)/$*glb.4gl $(tmpdir)/$< . $(top_srcdir)/i4gl; \
	$(COMPILE) -c -o $@ $(tmpdir)/$< && \
	rm $(tmpdir)/$< $(tmpdir)/$*glb.4gl

.per.frm:
	form4gl $<

.msg.hlp:
	mkmessage $< $@

clean-local:
	rm -f *.frm *.hlp
