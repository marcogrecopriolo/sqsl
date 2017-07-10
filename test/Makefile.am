#       MAKEFILE.am - tests automake defs
# 
#       The Structured Query Scripting Language
#       Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)
#
#       Initial release: Apr 17
#       Current release: Jun 17
#
#       This library is free software; you can redistribute it and/or
#       modify it under the terms of the GNU Lesser General Public
#       License as published by the Free Software Foundation; either
#       version 2.1 of the License, or (at your option) any later version.
#
#       This library is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#       Lesser General Public License for more details.
#
#       You should have received a copy of the GNU Lesser General Public
#       License along with this library; if not, write to the Free Software
#       Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
dist_noinst_DATA = test_runner.sh \
		   arith.out arith.sqsl format.out format.sqsl \
		   control.out control.sqsl simple.out simple.sqsl \
		   aggregate.out aggregate.sqsl pattern.sqsl pattern.out \
		   regex.out regex.sqsl ifmx.out ifmx.sqsl
				
TESTS = arith.sqsl format.sqsl control.sqsl simple.sqsl aggregate.sqsl \
        pattern.sqsl $(REGEXTEST) $(IFMXTEST)
TEST_EXTENSIONS = .sqsl
SQSL_LOG_COMPILER = ./test_runner.sh
AM_SQSL_LOG_FLAGS = -x=cli$(EXEEXT)

test_runner.sh: cli$(EXEEXT) fgwifmx$(DLLEXT)

cli$(EXEEXT): ../cli/cli$(EXEEXT)
	ln -s ../cli/cli$(EXEEXT) .


fgwifmx$(DLLEXT): ../ifmx/fgwifmx$(DLLEXT)
	ln -s ../ifmx/fgwifmx$(DLLEXT) .

clean-local:
	rm -f *.res cli$(EXEEXT) fgwifmx$(DLLEXT)
