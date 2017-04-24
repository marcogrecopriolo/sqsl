#!/bin/sh
#       test_runner.sh - Simple test runner driver
#
#       The Structured Query Scripting Language
#       Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)
#
#       Initial release: Apr 17
#       Current release: Apr 17
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

if test $# -ne 2
then
    echo "use this script from make check"
    exit 1
fi

for arg in "$@"
do
    case $arg in
	-x=*)
	    exe="${arg#*=}"
	    shift
	    ;;
	*)
	    script=`basename $arg .sqsl`
	    shift
	    ;;
    esac
done

if test "$script" == "" -o "$exe" == ""
then
    echo "missing script name or shared object type"
fi

./$exe ${script}.sqsl > ${script}.res
diff ${script}.out ${script}.res > ${script}.err
if test -s ${script}.err 
then
    exit 1
else
    rm ${script}.err
    exit 0
fi
