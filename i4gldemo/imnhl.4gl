#	IMNHL.4gl - Horizontal menu library
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2005 Marco Greco (marco@4glworks.com)
#
#	Initial release: Mar 93
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

define
    cur_opt	smallint,
    next_opt	smallint,
    errid	smallint,
    menu_name	char(8),
    p_opt	array[25] of record
		    cmd		char(25),
		    desc	char(78),
		    mkey	char(1),
		    msg		smallint,
		    parm	smallint,
		    enabled	smallint
		end record,
    p_key	array[40] of record
		    mkey	smallint,
		    msg		smallint,
		    parm	smallint,
		    enabled	smallint
		end record,
    last_key	smallint,
    last_opt	smallint,
    c		char(1),
    cur_help,
    def_help	smallint

#
#  module initialization
#
function mnh_init()
    whenever error call trap_err
    let def_help=""
    call mnh_start("")
end function
#
#  horizontal menu initialization
#
function mnh_start(name)
    define name		char(8),
	   i		smallint

    let cur_opt=1
    let next_opt=0
    for i=1 to 25
	let p_opt[i].cmd=""
	let p_opt[i].desc=""
	let p_opt[i].mkey=""
	let p_opt[i].msg=""
	let p_opt[i].parm=""
	let p_opt[i].enabled=false
    end for
    for i=1 to 40
	let p_key[i].mkey=""
	let p_key[i].msg=""
	let p_key[i].parm=""
	let p_key[i].enabled=false
    end for
    let last_key=0
    let last_opt=0
    let menu_name=name
    let cur_help=def_help
end function
#
#  help message set_up
#
function mnh_help(hn, def)
    define hn, def	smallint

    let cur_help=hn
    if def
    then
	let def_help=hn
    end if
end function
#
#  option set_up
#
function mnh_opt(c, d, qk, o, m, p)
    define c		char(25),
	   d		char(78),
	   qk		char(1),
	   o, m, p,
	   i, j, k	smallint,
	   uc		char(25)

    let errid=""
    case
      when (last_opt=25) or (m=MB_allmessages) or (p=MP_allbodies)
	return false
      when (o is null) or (o<1) or (o>last_opt)
	let o=last_opt+1
    end case
    let k=char2int(qk)
    for i=1 to last_key
	if p_key[i].mkey=k
	then
	    let errid=i+40
	    return ""
	end if
    end for
    let uc=upshift(c)
    for i=1 to last_opt
	if upshift(p_opt[i].cmd)=uc
	then
	    let errid=i
	    return ""
	end if
    end for
    for i=last_opt to o step -1
	let j=i+1
	let p_opt[j].*=p_opt[i].*
    end for
    let p_opt[o].cmd=c
    let p_opt[o].desc=d
    let p_opt[o].mkey=qk
    let p_opt[o].msg=m
    let p_opt[o].parm=p
    let p_opt[o].enabled=true
    let last_opt=last_opt+1
    return o
end function
#
#  quick key setup
#
function mnh_key(k, m, p)
    define k, m, p,
	   i, j		smallint,
	   c		char(1)

    let errid=""
    if (last_key=40) or (m=MB_allmessages) or (p=MP_allbodies)
    then
	return false
    end if
    for i=1 to last_key
	if p_key[i].mkey=k
	then
	    let errid=i+40
	    return ""
	end if
    end for
    let c=ascii k
    for i=1 to last_opt
	if p_opt[i].mkey=c
	then
	    let errid=i
	    return ""
	end if
    end for
    let last_key=last_key+1
    let p_key[last_key].mkey=k
    let p_key[last_key].msg=m
    let p_key[last_key].parm=p
    let p_key[last_key].enabled=true
    return last_key
end function
#
#  return identity of existing conflicting entry
#
function mnh_error()
    define i	smallint

    case
      when (errid is null)
	return "", "", ""
      when (errid>40)
	let i=errid-40
	return "", p_key[i].msg, p_key[i].parm
      otherwise
	return p_opt[errid].cmd, p_opt[errid].msg, p_opt[errid].parm
    end case
end function
#
#  entry removal
#
function mnh_drop(m, p)
    define m, p	smallint,
	   i, j	smallint,
	   c, o	smallint

    let o=0
    for i=1 to last_opt
	case
	  when mnh_compare(p_opt[i].msg, p_opt[i].parm, m, p)
	    let o=o+1
	  when (o)
	    let j=i-o
	    let p_opt[j].*=p_opt[i].*
	end case
    end for
    for i=last_opt-o+1 to last_opt
	let p_opt[i].cmd=""
	let p_opt[i].desc=""
	let p_opt[i].mkey=""
	let p_opt[i].msg=""
	let p_opt[i].parm=""
	let p_opt[i].enabled=false
    end for
    let last_opt=last_opt-o
    let c=o
    let o=0
    for i=1 to last_key
	case
	  when mnh_compare(p_key[i].msg, p_key[i].parm, m, p)
	    let o=o+1
	  when (o)
	    let j=i-o
	    let p_key[j].*=p_key[i].*
	end case
    end for
    for i=last_key-o+1 to last_key
	let p_key[i].mkey=""
	let p_key[i].msg=""
	let p_key[i].parm=""
	let p_key[i].enabled=false
    end for
    let last_key=last_key-o
    return c+o
end function
#
#  temporary option disable
#
function mnh_hide(m, p, v)
    define m, p,
	   v, i, c	smallint

    if (v is null)
    then
	let v=0
    end if
    let c=0
    for i=1 to last_opt
	if mnh_compare(p_opt[i].msg, p_opt[i].parm, m, p)
	then
	    let p_opt[i].enabled=not v
	    let c=c+1
	end if
    end for
    for i=1 to last_key
	if mnh_compare(p_key[i].msg, p_key[i].parm, m, p)
	then
	    let p_key[i].enabled=not v
	    let c=c+1
	end if
    end for
    return c
end function
#
#  selects next option
#
function mnh_nextoption(m, p)
    define m, p, i	smallint

    for i=1 to last_opt
	if mnh_compare(p_opt[i].msg, p_opt[i].parm, m, p)
	then
	    let cur_opt=i
	    let next_opt=0
	    exit for
	end if
    end for
end function
#
#  moves current option to previous
#
function mnh_back()
    let next_opt=-1
end function
#
#  moves current option to next
#
function mnh_forth()
    let next_opt=1
end function
#
#  returns option position
#
function mnh_getoptpos(m, p)
    define m, p,
	   i	smallint

    for i=1 to last_opt
	if mnh_compare(p_opt[i].msg, p_opt[i].parm, m, p)
	then
	    return i
	end if
    end for
    return ""
end function
#
#  menu display & option selection
#
function mnh_get()
    define i, j, k, l	smallint,
	   left_opt,
	   base_opt,
	   cnt_opt,
	   old_opt,
	   left_char,
	   base_char	smallint,
	   p_sta	array[25] of record
			    ambigue	integer,
			    x1, x2	smallint,
			    p, n	smallint
			end record,
	   rdwop	smallint,
	   gotit	integer,
	   om, op	smallint,
	   keyhit	char(1)

    let int_flag=false
    call opt_clear()
    let base_char=1
    if length(menu_name)>0
    then
	display menu_name clipped, ": " at 1, 1
	let base_char=length(menu_name)+2
    end if
    let left_char=0
    let l=base_char
    let cnt_opt=0
    let i=""
    for k=1 to last_opt
	initialize p_sta[k].* to null
	case
	  when (p_opt[k].enabled=0)
	  when pan_test(p_opt[k].msg)
	    let p_sta[k].x1=l+1
	    let l=l+length(p_opt[k].cmd)+2
	    let p_sta[k].x2=l
	    if i is null
	    then
		let base_opt=k
	    else
		let p_sta[k].p=i
		let p_sta[i].n=k
	    end if
	    let i=k
	    let p_sta[k].ambigue=false
	    let j=base_opt
	    while j!=k
		if p_opt[j].mkey=p_opt[k].mkey
		then
		    let p_sta[j].ambigue=true
		    let p_sta[k].ambigue=true
		end if
		let j=p_sta[j].n
	    end while
	    let cnt_opt=cnt_opt+1
	end case
    end for
    let p_sta[base_opt].p=i
    let p_sta[i].n=base_opt
    case
      when p_sta[cur_opt].x1 is null
	let cur_opt=base_opt
      when (next_opt>0)
	let cur_opt=p_sta[cur_opt].n
      when (next_opt<0)
	let cur_opt=p_sta[cur_opt].p
    end case
    let left_opt=base_opt
    let rdwop=true
    let gotit=""
    let next_opt=0
    while true
	case
	  when rdwop
	    display "" at 1, base_char
	    case
	      when p_sta[cur_opt].x2-left_char>80
		let left_char=0
		let left_opt=base_opt
		while p_sta[cur_opt].x2-left_char>80
		    let left_opt=p_sta[left_opt].n
		    let left_char=p_sta[left_opt].x1-base_char-1
		end while
	      when cur_opt<left_opt
		let left_opt=cur_opt
		let left_char=p_sta[cur_opt].x1-base_char-1
	    end case
	    let k=left_opt
	    while (p_sta[k].x2-left_char<=80)
	        let l=p_sta[k].x1-left_char
		if (k!=cur_opt)
		then
		    display " ", p_opt[k].cmd clipped, " "
		       at 1, l attribute (normal)
		else
		    display " ", p_opt[k].cmd clipped, " "
		       at 1, l attribute (reverse)
		end if
		let k=p_sta[k].n
		if (k=base_opt)
		then
		    exit while
		end if
	    end while
	    display p_opt[cur_opt].desc clipped, "" at 2, 1
	  when (old_opt!=cur_opt)
	    let l=p_sta[cur_opt].x1-left_char
	    display " ", p_opt[cur_opt].cmd clipped, " "
	      at 1, l attribute (reverse)
	    let l=p_sta[old_opt].x1-left_char
	    display " ", p_opt[old_opt].cmd clipped, " "
	      at 1, l attribute (normal)
	    display p_opt[cur_opt].desc clipped, "" at 2, 1
	end case
	let rdwop=false
	let old_opt=cur_opt
	case gotit
	  when false
	    call opt_put(p_opt[cur_opt].msg, p_opt[cur_opt].parm)
	    return p_opt[cur_opt].msg, p_opt[cur_opt].parm
	  when true
	    call usr_warn("dc")
	end case
	let gotit=""
	let i=fgl_getkey()
	let keyhit=ascii(i)
	case
	  when int_flag
	    let int_flag=false
	  when i=fgl_keyval("help") or i=fgl_keyval("control-w")
	    call help_show(cur_help)
	    let int_flag=false
	  when i=13
	    call opt_put(p_opt[cur_opt].msg, p_opt[cur_opt].parm)
	    return p_opt[cur_opt].msg, p_opt[cur_opt].parm
	  when i=fgl_keyval("left")
	    let cur_opt=p_sta[cur_opt].p
	    let rdwop=(cur_opt<left_opt or p_sta[cur_opt].x2-left_char>80)
	  when i=fgl_keyval("right") or (i=32)
	    let cur_opt=p_sta[cur_opt].n
	    let rdwop=(cur_opt<left_opt or p_sta[cur_opt].x2-left_char>80)
	  otherwise
	    for k=1 to last_key
		if (p_key[k].mkey=i)
		then
		    if pan_test(p_key[k].msg) and
		       (p_key[k].enabled)
		    then
			call opt_put(p_key[k].msg, p_key[k].parm)
			return p_key[k].msg, p_key[k].parm
		    end if
		end if
	    end for
	    if keyhit is null
	    then
		continue while
	    end if
	    let keyhit=upshift(keyhit)
	    let k=cur_opt
	    let j=cnt_opt
	    while (j>0)
		let k=p_sta[k].n
		if (p_opt[k].mkey=keyhit)
		then
		    let gotit=p_sta[k].ambigue
		    let cur_opt=k
		    let rdwop=(cur_opt<left_opt or
				p_sta[cur_opt].x2-left_char>80)
		    exit while
		end if
	        let j=j-1
	    end while
	end case
    end while
end function
#
#  compares two messages
#
function mnh_compare(m1, p1, m2, p2)
    define m1, p1,
	   m2, p2	smallint

    return ((m1=m2) or (m1 is null and m2 is null) or
	    (m1=MB_allmessages) or (m2=MB_allmessages)) and
	   ((p1=p2) or (p1 is null and p2 is null) or
	    (p1=MP_allbodies) or (p2=MP_allbodies))
end function
