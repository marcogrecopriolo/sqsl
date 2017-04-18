#	MAKEFILE.am - I4gl SQSL library automake defs
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

# Have to pass along INFORMIXC and cater for all autoconf amenities...
COMPILE = INFORMIXC="$(INFORMIXC)"; export INFORMIXC; $(C4GL) \
	  -I$(top_srcdir)/oslib $(DNEED_XSL) $(DNT) $(REGEX_DEFINE)
lib_LIBRARIES = libsqsli4gl.a
libsqsli4gl_a_SOURCES = cascc.c ccmnc.c cexpc.c cfioc.c chstc.c cldec.c \
			clddc.c cldpc.c csqpc.c csqic.c crqfc.c crqxc.c \
			crsxc.c crxvc.c crxuc.c crxoc.c crxfc.c crxcc.c \
			crxxc.c csqpy.y \
			cascc.h ccmnc.h ccmpc.h cexpc.h cfioc.h cfnmc.h \
			chstc.h cldpc.h csqec.h csqhc.h csqrc.h csqpc.h \
			crqfc.h crqxd.h crqxp.h crsxc.h crxuc.h crxoc.h \
			crxfc.h crxcc.h crxvc.h crxxd.h crxxp.h ctypc.h \
			cxslc.h
libsqsli4gl_a_LIBADD = $(OSLIBADD)

.y.c:
	$(YACCCMP)
