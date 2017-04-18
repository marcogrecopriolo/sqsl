#	CSTRL.4gl  -  string related functions
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2002 Marco Greco (marco@4glworks.com)
#
#	Initial release: Oct 92
#	Current release: Apr 04
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
#  left justifies a pkey
#
function str_left(pkey)
    define pkey	char(8),
	   i	smallint

    whenever error call trap_err
    let i=1
    if length(pkey)=0
    then
	return ""
    end if
    while pkey[i]=" "
	let i=i+1
    end while
    return pkey[i,8]
end function
#
#  right justifies a pkey
#
function str_right(pkey)
    define pkey	char(8),
	   i	smallint

    let i=length(pkey)+1
    if (i<9)
    then
	let pkey=pkey[i,8], pkey clipped
    end if
    return pkey
end function
