#	IMNUL.4gl - A few handy menu related routines
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2005 Marco Greco (marco@4glworks.com)
#
#	Initial release: May 95
#	Current release: Jun 05
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

#include "idefl.4gh"

#
#  tipycal quick key browsing set up
#
function set_browse_keys()
    define i	smallint

    whenever error call trap_err
    let i=mnh_key(fgl_keyval("tab"), MB_nextpane, "")
    let i=mnh_key(fgl_keyval("up"), MB_prevrow, "")
    let i=mnh_key(fgl_keyval("down"), MB_nextrow, "")
    let i=mnh_key(fgl_keyval("f2"), MB_prevrec, "")
    let i=mnh_key(fgl_keyval("f3"), MB_nextrec, "")
    let i=mnh_key(fgl_keyval("prevpage"), MB_prevpage, "")
    let i=mnh_key(fgl_keyval("nextpage"), MB_nextpage, "")
    let i=mnh_key(fgl_keyval("control-g"), MB_prevpage, "")
    let i=mnh_key(fgl_keyval("control-f"), MB_nextpage, "")
    let i=mnh_key(fgl_keyval("f1"), MB_home, "")
    let i=mnh_key(fgl_keyval("f4"), MB_end, "")
    let i=mnh_key(fgl_keyval("f7"), MB_left, "")
    let i=mnh_key(fgl_keyval("f8"), MB_right, "")
end function
