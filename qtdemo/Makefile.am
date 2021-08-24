#	MAKEFILE.am - Qt Interface automake defs
#
#	The Structured Query Scripting Language
#	Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)
#
#	Initial release: Aug 21
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

QT_CXXFLAGS = @QT_CXXFLAGS@
QT_LIBS     = @QT_LIBS@
MOC         = @QT_MOC@
UIC         = @QT_UIC@
RCC         = @QT_RCC@

# FIXME c4gl and esql can't handle -D defines with embedded spaces
COMPILE = $(CC) $(CC_EXTRA) -I$(ESQLINC) -I$(top_srcdir)/sqsl \
	    -DESQLVER=$(ESQLVER) -I$(top_srcdir)/oslib \
            $(DNT) -DVERSION="\"$(VERSION)\"" \
            -DPACKAGE_NAME="\"$(PACKAGE_NAME)\"" \
            -DPACKAGE_DATE="\"$(PACKAGE_DATE)\"" -DCOMP_NAME="\"qtdemo\""

CXXCOMPILE = $(CXX) $(CC_EXTRA) -I$(ESQLINC) -I$(top_srcdir)/sqsl \
	    $(QT_CXXFLAGS) \
	    -DESQLVER=$(ESQLVER) -I$(top_srcdir)/oslib $(DNT)

# Have to pass along INFORMIXC and cater for all autoconf amenities...
# CC_LD_STATIC would map to -static on gcc, which clashes with esql/cs -static
CXXLD = INFORMIXC="$(CXX) $(CC_LD_FLAG) $(CC_LD_STATIC) $(LD_EXTRA)"; \
	export INFORMIXC; $(ESQL) $(ESQL_STATIC)

LDADD = $(QT_LIBS) qrc_resources.o moc_qtdemo.o $(top_srcdir)/sqsl/libsqsl.a $(OSLINK)

bin_PROGRAMS = qtdemo
qtdemo_SOURCES = qtdemo.cpp qtdemo.h args.c csqmc.c csqmc.h qtdemo.ui resources.qrc
EXTRA_qtdemo_DEPENDENCIES = ui_qtdemo.h

ui_qtdemo.h: qtdemo.ui
	$(UIC) -o $@ $<

qrc_resources.cpp: resources.qrc
	$(RCC) -o $@ $<

moc_qtdemo.cpp: qtdemo.h
	$(MOC) -o $@ $<

clean-local:
	rm -f ui_qtdemo.h moc_qtdemo.cpp moc_predefs.h qrc_resources.cpp
