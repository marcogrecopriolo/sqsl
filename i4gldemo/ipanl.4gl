#	IPANL.4gl - Pane and [screen] cursor movement library
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2002 Marco Greco (marco@4glworks.com)
#
#	Initial release: Oct 96
#	Current release: Oct 02
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

#include "idefl.4gh"

  define pan_rec	array[K_maxpane] of record
#
#  page start, current line number, total no of lines,
#  page length of the current pane
#
			      fln,ln,mln	integer,
			      pl		smallint,
			      brief		integer,
#
#  nr_map, pr_map are used to eventually convert change row/page messages
#  to the corresponding change record messages.
#
			      nr_map, pr_map	smallint
      			end record,
			cnt_pane,
			cur_pane,
			focus_pane,
			use_field,
			next_access	smallint
#
#  reset pane library status
#
function pan_clear()
     define i	smallint

     whenever error call trap_err
     let cnt_pane=1
     let cur_pane=1
     let focus_pane=1
     let next_access=true
     let use_field=true
     for i=1 to K_maxpane
         let pan_rec[i].fln=0
         let pan_rec[i].mln=0
         let pan_rec[i].ln=1
	 let pan_rec[i].brief=false
	 let pan_rec[i].nr_map=null
	 let pan_rec[i].pr_map=null
     end for
     call mnu_mask(MB_nextpane, MB_nextpane, 0)
end function
#
#  adds more panes
#
function pan_add(np)
    define np	smallint

    if (cnt_pane+np<=K_maxpane)
    then
    	let cnt_pane=cnt_pane+np
	call mnu_mask(MB_nextpane, MB_nextpane, K_allpanes)
	return cnt_pane
    else
	return 0
    end if
end function
#
#  starts pan_next pane access
#
function pan_enter()
    let cur_pane=1
    let next_access=true
end function
#
#  sets the current pane to the next
#
function pan_next()
    case
      when next_access
	let next_access=false
	let cur_pane=1
      when (cur_pane=cnt_pane)
        let cur_pane=1
      otherwise
    	let cur_pane=cur_pane+1
    end case
end function
#
#  sets current pane
#
function pan_set_current(p)
    define p	smallint

    if (p>0) and (p<=cnt_pane)
    then
        let cur_pane=p
	let next_access=false
	return true
    else
	return false
    end if
end function
#
#  returns current pane
#
function pan_current()
    return cur_pane
end function
#
#  sets focussed pane
#
function pan_set_focussed(p)
    define p	smallint

    if (p>0) and (p<=cnt_pane)
    then
        let focus_pane=p
    end if
end function
#
#  true if current pane is focussed
#
function pan_focussed()
    return (cur_pane=focus_pane)
end function
#
#  reset browse filter status for current pane
#
function browse_start(state)
    define state	smallint

    let pan_rec[cur_pane].brief=(state=MB_brief)
    call mnu_enable(MB_prevrow, MB_nextrow, cur_pane, pan_rec[cur_pane].brief)
    call mnu_enable(MB_prevrec, MB_end, cur_pane, true)
    case
      when state=MB_detailed
	let pan_rec[cur_pane].nr_map=MB_nextpage
	let pan_rec[cur_pane].pr_map=MB_prevpage
      when state=MB_brief
	let pan_rec[cur_pane].nr_map=MB_nextrow
	let pan_rec[cur_pane].pr_map=MB_prevrow
      when state=MB_userdetailed
	let pan_rec[cur_pane].nr_map=""
	let pan_rec[cur_pane].pr_map=""
    end case
end function
#
#  browse messages filter
#
function browse_filter(im, ip)
    define im, ip,
	   ol, op 	smallint

    case
      when (cur_pane!=focus_pane)
        return im, ip
      when (im>=MB_detailed) and (im<=MB_userdetailed)
      when (im<MB_prevrow) or (im>MB_end)
	return im, ip
      when (im=pan_rec[cur_pane].nr_map)
	let im=MB_nextrec
      when (im=pan_rec[cur_pane].pr_map)
	let im=MB_prevrec
    end case
    let ol=pan_rec[cur_pane].ln
    let op=MP_noscroll
    if pan_rec[cur_pane].brief
    then
	case
	  when (im=MB_prevrec)
	    case
	      when (pan_rec[cur_pane].ln-pan_rec[cur_pane].fln>1)
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln-1
	      when (pan_rec[cur_pane].ln>1)
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln-1
		let pan_rec[cur_pane].fln=pan_rec[cur_pane].fln-1
		let op=MP_scrolldown
	    end case
	  when (im=MB_nextrec)
	    case
	      when (pan_rec[cur_pane].ln>=pan_rec[cur_pane].mln)
	      when (pan_rec[cur_pane].ln-pan_rec[cur_pane].fln<
	      	    pan_rec[cur_pane].pl)
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln+1
	      otherwise
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln+1
		let pan_rec[cur_pane].fln=pan_rec[cur_pane].fln+1
		let op=MP_scrollup
	    end case
	  when (im=MB_prevpage)
	    case
	      when (pan_rec[cur_pane].fln>=pan_rec[cur_pane].pl-1)
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln
					 -pan_rec[cur_pane].pl+1
		let pan_rec[cur_pane].fln=pan_rec[cur_pane].fln
					  -pan_rec[cur_pane].pl+1
		return MB_display, MP_partdisplay
	      when (pan_rec[cur_pane].fln>0) and
	      	   (pan_rec[cur_pane].ln>pan_rec[cur_pane].pl-1)
		let pan_rec[cur_pane].fln=0
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln
					 -pan_rec[cur_pane].pl+1
		return MB_display, MP_partdisplay
	      when (pan_rec[cur_pane].fln>0)
		let pan_rec[cur_pane].fln=0
		let pan_rec[cur_pane].ln=1
		return MB_display, MP_partdisplay
	      when (pan_rec[cur_pane].ln>1)
		let pan_rec[cur_pane].ln=1
	    end case
	  when (im=MB_nextpage)
	    case
	      when (pan_rec[cur_pane].ln+pan_rec[cur_pane].pl-1
	      	    <pan_rec[cur_pane].mln)
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln
					 +pan_rec[cur_pane].pl-1
		let pan_rec[cur_pane].fln=pan_rec[cur_pane].fln
					  +pan_rec[cur_pane].pl-1
		return MB_display, MP_partdisplay
	      when (pan_rec[cur_pane].fln+pan_rec[cur_pane].pl-1
		    <pan_rec[cur_pane].mln)
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
		let pan_rec[cur_pane].fln=pan_rec[cur_pane].fln
					  +pan_rec[cur_pane].pl-1
		return MB_display, MP_partdisplay
	      when (pan_rec[cur_pane].ln<pan_rec[cur_pane].mln)
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
	    end case
	  when (im=MB_home)
	    case
	      when (pan_rec[cur_pane].ln=1)
	      when (pan_rec[cur_pane].fln=0)
		let pan_rec[cur_pane].ln=1
	      otherwise
		let pan_rec[cur_pane].ln=1
		let pan_rec[cur_pane].fln=0
		return MB_display, MP_partdisplay
	    end case
	  when (im=MB_end)
	    case
	      when (pan_rec[cur_pane].ln>=pan_rec[cur_pane].mln)
	      when (pan_rec[cur_pane].ln-pan_rec[cur_pane].fln
	      	    <pan_rec[cur_pane].pl) and
		   (pan_rec[cur_pane].mln-pan_rec[cur_pane].fln
		    <=pan_rec[cur_pane].pl)
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
	      otherwise
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
		let pan_rec[cur_pane].fln=pan_rec[cur_pane].ln
					  -pan_rec[cur_pane].pl
		return MB_display, MP_partdisplay
	    end case
	  when (im=MB_detailed) or (im=MB_userdetailed)
	    call mnu_enable(im, im, cur_pane, false)
	    call mnu_enable(MB_brief, MB_brief, cur_pane, true)
	    call browse_start(im)
	    return im, ""
	end case
	if (ol=pan_rec[cur_pane].ln)
	then
	    return MB_display, MP_nodisplay
	else
	    call pan_status()
	    return MB_move, op
	end if
    else
	case
	  when (im=MB_prevrec)
	    if (pan_rec[cur_pane].ln>1)
	    then
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln-1
		return MB_display, MP_partdisplay
	    else
		return MB_display, MP_nodisplay
	    end if
	  when (im=MB_nextrec)
	    if (pan_rec[cur_pane].ln<pan_rec[cur_pane].mln)
	    then
		let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln+1
		return MB_display, MP_partdisplay
	    else
		return MB_display, MP_nodisplay
	    end if
	  when (im=MB_home)
	    let pan_rec[cur_pane].ln=1
	    return MB_display, MP_partdisplay
	  when (im=MB_end)
	    let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
	    return MB_display, MP_partdisplay
	  when (im=MB_brief)
	    call mnu_enable(MB_brief, MB_brief, cur_pane, false)
	    if pan_rec[cur_pane].nr_map is null
	    then
		call mnu_enable(MB_userdetailed, MB_userdetailed,
				cur_pane, true)
	    else
		call mnu_enable(MB_detailed, MB_detailed, cur_pane, true)
	    end if
	    call browse_start(im)
	    if (pan_rec[cur_pane].fln>=pan_rec[cur_pane].ln) or
	       (pan_rec[cur_pane].fln+pan_rec[cur_pane].pl<pan_rec[cur_pane].ln)
	    then
		let pan_rec[cur_pane].fln=pan_rec[cur_pane].ln-1
	    end if
	    return MB_brief, ""
	  otherwise
	    return im, ip
	end case
    end if
end function
#
#  scroll messages filter
#
function scroll_filter(im, ip)
    define im, ip,
	   ol, op 	smallint

    if (cur_pane!=focus_pane) or
       (im<MB_prevrow) or (im>MB_end) or
       (im=MB_prevrec) or (im=MB_nextrec)
    then
	return im, ip
    end if
    let ol=pan_rec[cur_pane].ln
    let op=MP_noscroll
    case
      when (im=MB_home)
	if (pan_rec[cur_pane].ln>1)
	then
	    let pan_rec[cur_pane].ln=1
	end if
      when (im=MB_end)
	if (pan_rec[cur_pane].ln+pan_rec[cur_pane].pl-1<pan_rec[cur_pane].mln)
	then
	    let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
	    			     -pan_rec[cur_pane].pl+1
	    let op=MP_scrollup
	end if
      when (im=MB_prevrow)
	if (pan_rec[cur_pane].ln>1)
	then
	    let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln-1
	    let op=MP_scrolldown
	end if
      when (im=MB_nextrow)
	if (pan_rec[cur_pane].ln<pan_rec[cur_pane].mln)
	then
	    let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln+1
	    let op=MP_scrollup
	end if
      when (im=MB_prevpage)
	let op=MP_scrolldown
	case
	  when (pan_rec[cur_pane].ln>pan_rec[cur_pane].pl)
	    let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln-pan_rec[cur_pane].pl+1
	  when (pan_rec[cur_pane].ln>1)
	    let pan_rec[cur_pane].ln=1
	end case
      when (im=MB_nextpage)
	let op=MP_scrollup
	case
	  when (pan_rec[cur_pane].ln+pan_rec[cur_pane].pl-1
	        <pan_rec[cur_pane].mln)
	    let pan_rec[cur_pane].ln=pan_rec[cur_pane].ln+pan_rec[cur_pane].pl-1
	  when (pan_rec[cur_pane].ln<pan_rec[cur_pane].mln)
	    let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
	end case
    end case
    if (ol=pan_rec[cur_pane].ln)
    then
	return MB_display, MP_nodisplay
    else
	return MB_scroll, op
    end if
end function
#
#  adj display parms for secondary scrollers
#
function pan_filter(im, ip)
    define im, ip  smallint

    case
      when (im=MB_nextpane)
        if (focus_pane=cnt_pane)
	then
	    let focus_pane=1
	else
	    let focus_pane=focus_pane+1
	end if
	return MB_display, MP_nodisplay
      when (im<MB_display) or (im>MB_declare)
      when (cur_pane>1) and (ip=MP_fulldisplay)
	let ip=MP_partdisplay
    end case
    return im, ip
end function
#
#  displays the current row number
#
function pan_status()
    define txt	char(20)

    if use_field and (pan_rec[cur_pane].mln=0)
    then
	let txt="0/0"
    else
	let txt=pan_rec[cur_pane].ln using "<<<<<&", "/",
	    pan_rec[cur_pane].mln using "<<<<<&"
    end if
    if use_field
    then
	whenever error continue
	if (focus_pane=cur_pane)
	then
	    display txt to s_pan[cur_pane].* attribute(reverse)
	else
	    display txt to s_pan[cur_pane].*
	end if
	whenever error call trap_err
	if status!=0
	then
	    let use_field=false
	end if
    end if
    case
      when use_field
      when pan_rec[cur_pane].mln=0
	call usr_status("nd")
      when pan_rec[cur_pane].mln=1
	call usr_status("")
      otherwise
	message " ", txt, " "
    end case
end function
#
#  test if message is enabled
#
function pan_test(m)
    define m smallint,
	   r integer

    if (m is null) or (m<MB_minmask) or (m>MB_maxmask)
    then
	return true
    end if
    let r=mnu_test(m, focus_pane)
    case
      when (r<0)
	return true
      when (r>0) and (r<=cnt_pane)
	return (pan_rec[r].mln>0)
      otherwise
	return false
    end case
end function
#
#  true if focussed pane has records
#
function pan_record()
    return pan_rec[focus_pane].mln>0
end function
#
#  scans rows to find a non empty one
#
function scroll_findtext(f, ip, ol)
    define f,
	   ip, ol, nr	integer

    let nr=pan_rec[cur_pane].ln+ip
    case
      when f or (pan_rec[cur_pane].mln=0) or (ip=0)
	return false
      when (nr>0) and (nr<pan_rec[cur_pane].mln)
	let pan_rec[cur_pane].ln=nr
	return true
      otherwise
	let pan_rec[cur_pane].ln=ol
	return false
    end case
end function
#
#  used to determine the number of rows in the current pane
#
function browse_search(istate)
    define istate,
	   ostate	smallint
    case
#
#  enter search
#
      when istate is null
#
#  this routine relies on the engine to set the estimated number
#  of rows returned in sqlca.sqlerrd[1]. this AFAIK only happens
#  in OL5+ engines.
#
#  *TWO WORDS OF WARNING*
#
#  of course the engine expects to find statistics up to date.
#  set mln to 0 if you have problems.
#
#  Note that (and the FM is not clear on this point) the engine
#  sets sqlca.sqlerrd[1] after declaring a cursor. Open cursor
#  leaves sqlerrd[1] *unchanged*.
#  Of course, it could be that it works the other way round
#  You have been warned!
#
	let pan_rec[cur_pane].mln=sqlca.sqlerrd[1]
	if (pan_rec[cur_pane].mln=0)
	then
	    let pan_rec[cur_pane].mln=1
	end if
	let ostate=2
#
#  no more rows and forward search - exit now
#
      when (sqlca.sqlcode!=0) and (istate=1)
	let ostate=0
	let pan_rec[cur_pane].mln=pan_rec[cur_pane].mln-1
#
#  no more rows and backward search - exit now
#
      when (sqlca.sqlcode!=0) and (pan_rec[cur_pane].mln=1)
	let ostate=0
	let pan_rec[cur_pane].mln=0
#
#  row not found and backward search - try one less row
#
      when (sqlca.sqlcode!=0)
	let pan_rec[cur_pane].mln=pan_rec[cur_pane].mln-1
	let ostate=-1
#
#  found row and forward search - try one more
#
      when (istate>0)
	let pan_rec[cur_pane].mln=pan_rec[cur_pane].mln+1
	let ostate=1
      otherwise
	let ostate=0
    end case
#
#  and now adj row pointer and related vars
#
    if (ostate=0)
    then
	call browse_test_row()
    end if
    return ostate
end function
#
#  adj screen cursor position
#
function browse_test_row()
    case
      when (pan_rec[cur_pane].mln=0)
	let pan_rec[cur_pane].fln=0
	let pan_rec[cur_pane].ln=1
      when (pan_rec[cur_pane].ln>pan_rec[cur_pane].mln)
	let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
	let pan_rec[cur_pane].fln=maxint(pan_rec[cur_pane].mln
					 -pan_rec[cur_pane].pl, 0)
      when (pan_rec[cur_pane].ln>pan_rec[cur_pane].fln+pan_rec[cur_pane].pl)
	let pan_rec[cur_pane].fln=pan_rec[cur_pane].ln-pan_rec[cur_pane].pl
      when (pan_rec[cur_pane].fln>=pan_rec[cur_pane].ln)
	let pan_rec[cur_pane].fln=pan_rec[cur_pane].ln-1
    end case
end function
#
#  adj screen cursor position
#
function scroll_test_row()
    case
      when (pan_rec[cur_pane].mln=0)
	let pan_rec[cur_pane].fln=0
	let pan_rec[cur_pane].ln=1
      when (pan_rec[cur_pane].ln>pan_rec[cur_pane].mln)
	let pan_rec[cur_pane].ln=pan_rec[cur_pane].mln
	let pan_rec[cur_pane].fln=maxint(pan_rec[cur_pane].mln
					 -pan_rec[cur_pane].pl, 0)
    end case
end function
#
#  resets the current row to the first
#
function pan_home()
     let pan_rec[cur_pane].ln=1
     let pan_rec[cur_pane].fln=0
end function
#
#  resets the current row to the last
#
function pan_end()
    if pan_rec[cur_pane].mln>pan_rec[cur_pane].pl
    then
	let pan_rec[cur_pane].fln=pan_rec[cur_pane].mln-pan_rec[cur_pane].pl
    else
	let pan_rec[cur_pane].fln=0
    end if
    let pan_rec[cur_pane].ln=pan_rec[cur_pane].fln+1
end function
#
#  sets current row
#
function pan_set_row(l)
    define l	integer

    let pan_rec[cur_pane].ln=l
end function
#
#  sets current screen array length
#
function pan_set_length(p)
    define p	smallint

    let pan_rec[cur_pane].pl=p
end function
#
#  sets current screen array highest row index
#
function pan_set_max(m)
    define m	integer

    let pan_rec[cur_pane].mln=m
end function
#
#  returns the current row index
#
function pan_row()
    return pan_rec[cur_pane].ln
end function
#
#  returns the current screen offset
#
function browse_offset()
    return pan_rec[cur_pane].fln
end function
#
#  returns current screen array length
#
function pan_length()
    return pan_rec[cur_pane].pl
end function
#
#  returns max row index
#
function pan_max()
    return pan_rec[cur_pane].mln
end function
#
#  returns current display state
#
function browse_brief()
    return pan_rec[cur_pane].brief
end function
#
#  returns first & last displayable row
#
function browse_range()
    define l	integer

    let l=minint(pan_rec[cur_pane].fln+pan_rec[cur_pane].pl,
    		 pan_rec[cur_pane].mln)
    return pan_rec[cur_pane].fln+1, l, l-pan_rec[cur_pane].fln+1
end function
#
#  returns first & last displayable row
#
function scroll_last()
    define l	integer

    let l=minint(pan_rec[cur_pane].ln+pan_rec[cur_pane].pl-1,
    		 pan_rec[cur_pane].mln)
    return l, l-pan_rec[cur_pane].ln+2
end function
