#	MAKEFILE.am - Esql/c SQSL library automake defs
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

# sqsllib uses relocatable code just in case we want to link it in a DLL
COMPILE = $(INFORMIXC) $(CC_RELOC) $(CC_EXTRA) -I$(ESQLINC) -I$(top_srcdir)/oslib \
	  $(DNEED_XSL) $(DNT) $(REGEX_DEFINE) -DESQLVER=$(ESQLVER)
lib_LIBRARIES = libsqsl.a
libsqsl_a_SOURCES = cascc.c cexpc.c chstc.c csqpc.c ccmnc.c cfioc.c \
                    cldec.c clddc.c cldpc.c crqfc.c crqxc.c crsxc.c \
		    crxvc.c crxxc.c crxuc.c crxoc.c crxfc.c crxcc.c \
		    csqpy.y \
		    ccmnc.h cascc.h ccmpc.h cexpc.h cfioc.h cfnmc.h \
		    chstc.h cldpc.h csqhc.h csqpc.h csqec.h csqrc.h \
		    crqfc.h crqxd.h crqxp.h crsxc.h crxuc.h crxoc.h \
		    crxfc.h crxcc.h crxxd.h crxxp.h crxvc.h ctypc.h \
		    cxslc.h
libsqsl_a_LIBADD = $(OSLIBADD)

.y.c:
	$(YACCCMP)
