#	MAKEFILE.am - Aubit 4gl curses interface demo automake defs
#
#	The Structured Query Scripting Language
#	Copyright (C) 1992-2020 Marco Greco (marco@4glworks.com)
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

#
# EXEEXT, sigh! see the comments in configure.ac
#
EXEEXT = $(AUBEXT)

INCLUDES = -I$(top_srcdir)/aubit4gl -I$(top_srcdir)/oslib
COMPILE = INFORMIXC="$(INFORMIXC) $(CC_EXTRA) $(ESQLDEFS)"; A4GL_SQLTYPE=$(A4GL_SQLTYPE); \
	  A4GL_LEXTYPE=$(A4GL_LEXTYPE); A4GL_LEXDIALECT=$(A4GL_LEXDIALECT); \
	  A4GL_EC_COMP="$(A4GL_EC_COMP)"; A4GL_C_COMP="$(A4GL_EC_COMP)"; \
	  FGLPCSETTINGSDIR="$(A4GL_SETTINGSDIR)"; \
	  export INFORMIXC A4GL_SQLTYPE A4GL_LEXTYPE A4GL_EC_COMP A4GL_C_COMP; \
	  export FGLPCSETTINGSDIR; $(A4GL) $(DNT)
CCLD = INFORMIXC="$(INFORMIXC) $(CC_LD_FLAG) $(CC_LD_STATIC) $(LD_EXTRA)";  \
       A4GL_SQLTYPE=$(A4GL_SQLTYPE); A4GL_LEXTYPE=$(A4GL_LEXTYPE); \
       A4GL_LEXDIALECT=$(A4GL_LEXDIALECT); A4GL_LINK="$(A4GL_LINK)"; \
       FGLPCSETTINGSDIR="$(A4GL_SETTINGSDIR)"; \
       export INFORMIXC A4GL_SQLTYPE A4GL_LEXTYPE A4GL_LINK FGLPCSETTINGSDIR; \
       $(A4GL) $(ESQL_STATIC) $(DNT)
#COMPILE = INFORMIXC="$(INFORMIXC) $(CC_EXTRA)"; A4GL_SQLTYPE=$(A4GL_SQLTYPE); \
#	  A4GL_LEXTYPE=$(A4GL_LEXTYPE); A4GL_LEXDIALECT=$(A4GL_LEXDIALECT); \
#	  export INFORMIXC A4GL_SQLTYPE A4GL_LEXTYPE; $(A4GL) $(DNT)
#CCLD = INFORMIXC="$(INFORMIXC) $(CC_LD_FLAG) $(LD_EXTRA)"; \
#       A4GL_SQLTYPE=$(A4GL_SQLTYPE); A4GL_LEXTYPE=$(A4GL_LEXTYPE); \
#       A4GL_LEXDIALECT=$(A4GL_LEXDIALECT); \
#       export INFORMIXC A4GL_SQLTYPE A4GL_LEXTYPE; $(A4GL) $(DNT)
LDADD = $(top_srcdir)/aubit4gl/libsqslaubit.a $(OSLINK) $(REGEX_LIBRARY)
sqsldir = $(bindir)
tmpdir = $(top_srcdir)/tmp
toolsdir = $(top_srcdir)/tools

nodist_sqsl_DATA = hl1w.afr.dat hl2w.afr.dat hl3w.afr.dat hl4w.afr.dat \
		   sqlf.afr.dat expw.afr.dat shlw.afr.dat curses.hlp
dist_sqsl_DATA = csql.txt ishl.txt curses.txt
dist_noinst_DATA = hl1w.per hl2w.per hl3w.per hl4w.per sqlf.per expw.per \
		      shlw.per curses.msg
dist_sqsl_SCRIPTS = curses

bin_PROGRAMS = curses
curses_SOURCES = ccnvc.c cstrc.c ihorm.4gl imnhl.4gl ishll.4gl iun2h.4gl \
	       iusrl.4gl cmatc.c cstrl.4gl iminv.4gl imnuc.c isqlg.4gl \
	       isqlv.4gl iunih.4gl cshlc.c ctxtl.4gl iml2h.4gl imnul.4gl \
	       iunis.4gl cfnml.4gl cfnml.4gh ierrl.4gl imlth.4gl \
	       ipanl.4gl itedl.4gl iusrc.c csqll.4gh idefl.4gh iunih.4gh \
	       iexpl.4gl csquc.c

.4gl.o:
	$(AWK) -f $(toolsdir)/fglpp $(AWKDEL) $(DNT) \
				-DVERSION="'$(VERSION)'" \
				-DPACKAGE_NAME="'$(PACKAGE_NAME)'" \
				-DPACKAGE_DATE="'$(PACKAGE_DATE)'" \
				-DCOMP_NAME="'aubitdemo'" \
				-DSQ_freecurs="--free" \
		$< $(tmpdir)/$*glb.4gl $(tmpdir)/$< . $(INCLUDES); \
	$(COMPILE) $(INCLUDES) -c -o $@ $(tmpdir)/$< && \
	rm $(tmpdir)/$< $(tmpdir)/$*glb.4gl $*.c 

#FIXME - work out dependencies
.c.o:
	$(COMPILE) -I'$(ESQLINC)' $(INCLUDES) -c -o $@ $<

# FIXME: double extension, implicit rules won't fire. requires gmake.
%.afr.dat: %.per
	fcompile $<

.msg.hlp:
	amkmessage $< $@

clean-local:
	rm -f *.hlp *.afr.dat
