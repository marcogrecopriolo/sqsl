#	IUNIS.4gl - simple text scroller
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2009 Marco Greco (marco@4glworks.com)
#
#	Initial release: Oct 92
#	Current release: Jan 09
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

#include "cfnml.4gh"
#include "idefl.4gh"

define
  uni_text	array[K_maxpane] of text,
#
# reflects the state of the text variable: 0 - none
#					   1 - located
#					  -1 - newly located
  uni_tmpstate	array[K_maxpane] of smallint,
  uni_off	array[K_maxpane] of smallint,
#
# pane to screen field id conversion
#
  uni_pane2id	array[K_maxpane] of smallint,
  uni_id2pane	array[K_maxpane] of smallint,
#
#  working pane for text & line oriented ops
#
  pane_id,
  cur_pane	smallint

#
#  module initialization
#
function uni_init()
    define i    smallint

    whenever error call trap_err
    for i=1 to K_maxpane
        let uni_tmpstate[i]=0
    end for
    let pane_id=1
end function
#
#  sets working pane
#
function uni_setid(n)
    define n	smallint

    if (n<1 or n>5)
    then
	let pane_id=1
    else
	let pane_id=n
    end if
end function
#
#  form selection
#
function uni_looks(frm, p)
    define frm	char(14),
	   fp	char(PATHSIZE),
	   p, c	smallint

    call pan_next()
    if (frm is null) or (p is null) or (p<1)
    then
	let frm="txtf"
	let p=18
    end if
    let fp=resourcepath(frm)
    let c=pan_current()
    if (c=1)
    then
	open form uni_form from fp
    end if
    call pan_set_length(p)
    call mnu_enable(MB_prevrow, MB_right, pan_current(), true)
    call mnu_enable(MB_prevrec, MB_nextrec, pan_current(), false)
    call mnu_record(MB_print, MB_print, K_focussedpane)
    let uni_pane2id[c]=pane_id
    let uni_id2pane[pane_id]=c
    let pane_id=1
end function
#
#  text scroller
#
function uni_scroller(im, ip)
    define im, ip,
	   om, op	smallint

    call uni_upstream(im, ip)
      returning om, op
    while true
	call menu_get(om, op) returning im, ip
	call uni_downstream(im, ip)
	  returning om, op
	if (om>=MB_exit)
	then
	    return om, op
	end if
    end while
end function
#
#  upstream filter
#
function uni_upstream(im, ip)
    define im, ip,
	   om, op, c	smallint

    call pan_next()
    let c=pan_current()
    let cur_pane=uni_pane2id[c]
    let pane_id=1
    let om=MB_display
    let op=MP_nodisplay
    case
      when (im=MB_init)
	let om=im
	let op=ip
	let im=MB_declare
	let ip=MP_fulldisplay
      when (im>MB_init) and (im<=MB_maxupstream)
	call end_modal(im, ip) returning im, ip
    end case
    call pan_filter(im, ip) returning im, ip
    case
      when (im<MB_display) or (im>MB_declare)
	call pan_status()
      when (im is not null)
	call restore_uni(im, ip)
	if (om!=MB_init)
	then
	    let om=im
	    let op=ip
	end if
    end case
    return om, op
end function
#
#  downstream filter
#
function uni_downstream(im, ip)
    define im, ip	smallint,
	   ol, ln,
	   pl, mln, c	integer

    call pan_next()
    let c=pan_current()
    let cur_pane=uni_pane2id[c]
    let pane_id=1
    let ol=pan_row()
    call scroll_filter(im, ip) returning im, ip
    call pan_filter(im, ip) returning im, ip
    let ln=pan_row()
    let mln=pan_max()
    let pl=pan_length()
    case
      when (im=MB_display) or (im=MB_open) or (im=MB_declare)
	call restore_uni(im, ip)
      when (im=MB_exit)
	if (c=1)
	then
	    close form uni_form
	end if
	call drop_uni()
      when (not pan_focussed())
      when (im=MB_left)
	if (uni_off[cur_pane]>1)
	then
	    let uni_off[cur_pane]=uni_off[cur_pane]-1
	end if
	call display_uni(false)
	return MB_display, MP_nodisplay
      when (im=MB_right)
	let uni_off[cur_pane]=uni_off[cur_pane]+1
	call display_uni(false)
	return MB_display, MP_nodisplay
      when (im=MB_scroll)
	while scroll_findtext(length(asc_retrieveline(uni_text[cur_pane],
						      pan_row()))>0, ip, ol)
	end while
	let ln=pan_row()
#
#  It's my firm convincement that scrolling a line is faster than
#  redisplaying the entire array. If you think it isn't, then just
#  skip to the otherwise code
#
	case ln-ol
	  when 0
	  when 1
	    case cur_pane
	      when 1
		scroll s_uni.* up
	      when 2
		scroll s_uni2.* up
	      when 3
		scroll s_uni3.* up
	      when 4
		scroll s_uni4.* up
	      when 5
		scroll s_uni5.* up
	    end case
	    if (ln+pl-1<=mln)
	    then
		call drow_uni(ln+pl-1)
	    end if
	    call pan_status()
	  when -1
	    case cur_pane
	      when 1
		scroll s_uni.* down
	      when 2
		scroll s_uni2.* down
	      when 3
		scroll s_uni3.* down
	      when 4
		scroll s_uni4.* down
	      when 5
		scroll s_uni5.* down
	    end case
	    call drow_uni(ln)
	    call pan_status()
	  otherwise
	    call display_uni(true)
	end case
	return MB_display, MP_nodisplay
    end case
    return im, ip
end function
#
#  blob var location (needs correct pane being current)
#
function uni_create()
    if (uni_tmpstate[pane_id])
    then
	call asc_free(uni_text[pane_id])
    end if
    call asc_locate(uni_text[pane_id])
    let uni_tmpstate[pane_id]=-1
    let uni_off[pane_id]=1
end function
#
#  inserts new text line
#
function uni_newline(txt)
    define txt	char(78)

    call asc_addline(uni_text[pane_id], txt, "")
end function
#
#  row fetch (used for prints)
#
function uni_retrieve(r)
    define r	integer

    return asc_retrieveline(uni_text[pane_id], r)
end function
#
#  sets text for a pane
#
function uni_settext(t)
    define t	text

    if (uni_tmpstate[pane_id])
    then
	call asc_free(uni_text[pane_id])
    end if
    let uni_tmpstate[pane_id]=0
    let uni_off[pane_id]=1
    if (t is not null)
    then
	call asc_copy(t, uni_text[pane_id])
	if (status=0)
	then
	    let uni_tmpstate[pane_id]=-1
	end if
    end if 
end function
#
#  returns text for a pane
#
function uni_gettext(t)
    define t	text

    if (not uni_tmpstate[pane_id])
    then
	let sqlca.sqlcode=notfound
	return
    end if
    call asc_copy(uni_text[pane_id], t)
end function
#
#  returns text length for a pane
#
function uni_getlength()
    if (not uni_tmpstate[pane_id])
    then
	let sqlca.sqlcode=notfound
        return 0
    end if
    call asc_lines(uni_text[pane_id])
end function
#
#  free text vars
#
function drop_uni()
    define p smallint

    for p=1 to K_maxpane
	if (uni_tmpstate[p]>0)
	then
	    call asc_free(uni_text[p])
	    let uni_tmpstate[p]=0
	end if
    end for
end function
#
#  "display" messages servicing function
#
function restore_uni(rl, rd)
    define rl, rd	smallint

    case
      when (rl!=MB_open and rl!=MB_declare)
      when (not uni_tmpstate[cur_pane])
	call pan_set_max(0)
      when (uni_tmpstate[cur_pane]=-1)
	let uni_tmpstate[cur_pane]=1
	call pan_set_max(asc_lines(uni_text[cur_pane]))
	call pan_home()
    end case
    case
      when (rd=MP_fulldisplay)
	display form uni_form
	call usr_area()
	call display_uni(false)
      when (rd=MP_partdisplay) or (rd=pan_current())
	call display_uni(true)
      otherwise
	call pan_status()
    end case
end function
#
#  page display
#
function display_uni(clr)
    define clr		integer,
	   f, l, s, k	integer

    call pan_status()
    let f=pan_row()
    call scroll_last() returning l, s
    for k=f to l
        call drow_uni(k)
    end for
    if clr
    then
	case cur_pane
	  when 1
	    for k=s to pan_length()
#
#  a small word of warning: if you put color/intensity attributes in your form
#  clear will use them. If you need to completely blank the fields, use
#
#		display " " to s_uni[k].* attribute(normal)
#
#  rather that this and the following clears
#
		clear s_uni[k].*
	    end for
	  when 2
	    for k=s to pan_length()
		clear s_uni2[k].*
	    end for
	  when 3
	    for k=s to pan_length()
		clear s_uni3[k].*
	    end for
	  when 4
	    for k=s to pan_length()
		clear s_uni4[k].*
	    end for
	  when 5
	    for k=s to pan_length()
		clear s_uni5[k].*
	    end for
	end case
    end if
end function
#
#  row display
#
function drow_uni(r)
    define r, l, o, e	integer,
	   txt 		char(78)

    let l=r-pan_row()+1
    if (uni_off[cur_pane]=1)
    then
	let txt=asc_retrieveline(uni_text[cur_pane], r)
    else
#
# FIXME: move left is there's nothing to display?
#
	let o=asc_linepos(uni_text[cur_pane], r)
	let e=o+asc_linelength(uni_text[cur_pane], r)-1
	let o=o+uni_off[cur_pane]-1
	if (e>=o)
	then
	    let txt=asc_retrievestring(uni_text[cur_pane], o, e)
	else
	    let txt=""
	end if
    end if
    case cur_pane
      when 1
	display txt to s_uni[l].*
      when 2
	display txt to s_uni2[l].*
      when 3
	display txt to s_uni3[l].*
      when 4
	display txt to s_uni4[l].*
      when 5
	display txt to s_uni5[l].*
    end case
end function
