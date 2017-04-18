#	IUSRL.4gl  -  Console I/O
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2004 Marco Greco (marco@4glworks.com)
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

#include "idefl.4gh"

define cur_msg		char(80),
       cur_width	smallint,
       isbatch		smallint,
       fd		smallint,
       help_no_cur,
       help_no_def	smallint

#
#  module initialization
#
function usr_init()
    whenever error call trap_err
    let cur_width=80
    let cur_msg=null
    let isbatch=false
    let help_no_cur=""
    let help_no_def=""
    let fd=stdfd_open(2)
end function
#
#  returns user interaction mode
#
function usr_isbatch()
    return isbatch
end function
#
#  sets user interaction mode
#
function usr_setbatch(mode)
    define mode         smallint
 
    if (mode)
    then
	let isbatch=true
    else
	let isbatch=false
    end if
    return isbatch
end function
#
#   draw user greeting screen
#
function usr_greet(txt, lm, s)
    define txt	char(40),
	   c	char(1),
	   lm	smallint,
	   s	smallint,
	   i,j	smallint


    if (isbatch)
    then
	return
    end if
    let i=length(txt)
    case
      when (i=0)
        return
      when (i=1)
    	let s=1
    end case
    case
      when (s<1) or (s>77)
	let s=1
	let lm=1+(80-i)/2
      when s is not null
        if lm is null or lm<1 or lm>77
        then
            let lm=1+(80-i*(s+1))/2
        end if
      when (lm>1) and (lm<77)
        let s=(80-i-lm+1)/(i-1)
      otherwise
        let s=(80-i)/(i-1)
        let lm=1+((80-i) mod (i-1))/2
    end case
    let i=1
    let j=length(txt)
    while (lm<80) and (i<=j)
	let c=txt[i]
	display c at 2, lm
	let lm=lm+s+1
	let i=i+1
    end while
    call fgl_drawbox(20, 80, 3, 1)
    call usr_status("wa")
end function
#
#  Display non standard message on rows 1 & 2 (input mode)
#
function usr_header(m, c, a)
    define m	char(78),
	   c, a	smallint,
#
#  t used to avoid TSS problems
#
	   t	char(78)

    case
      when isbatch
	return
      when c
	let t=txt_retrieve("accept")
	let m=m clipped, ": ", t clipped
    end case
    let t=txt_retrieve("abort")
    case
      when not a
      when c
	let m=m clipped, ", ", t clipped
      otherwise
	let m=m clipped, ": ", t clipped
    end case
    display m, "" at 1, 1 	#clipped
    display "" at 2, 1
end function
#
#  Display nice box around form
#
function usr_area()
    define h	char(20),
	   l	smallint

    if (not isbatch)
    then
	let h=txt_retrieve("help")
	let l=79-length(h)
	call fgl_drawbox(20, cur_width, 3, 1)
	display h clipped at 22, l attribute(reverse)
	call pop_warning()
    end if
end function
#
#  end_modal filters messages produced by input/construct modules
#  and takes all the appropriate actions needed to revert to browse
#  mode. printer checks are also done here, so as to allow input/
#  construct modules to issue multiple print jobs and perform all
#  the disk IO they need to w/o having to bother about the status of
#  all printers involved
#
function end_modal(im, ip)
    define im, ip,
           om, op       smallint

    if (ip=MP_fulldisplay)
    then
	call hold_warning()
    end if
    let op=ip
    if int_flag
    then
	let om=MB_display
	case
	  when (im=MB_recdeleted)
	    call usr_warn("ea")
	  when (im=MB_recinserted)
	    call usr_warn("ia")
	  when (im=MB_recchanged)
	    call usr_warn("ma")
	  when (im=MB_newset)
	    call usr_warn("oi")
	end case
    else
	case
	  when (im=MB_recdeleted)
	    call usr_warn("ee")
	    let om=MB_open
	  when (im=MB_notfound)
	    let om=MB_open
	  when (im=MB_doneprint)
	    call usr_warn("cptr.sa")
	    let om=MB_display
	  when (im=MB_newset)
	    call usr_status("")
	    let om=MB_declare
	  when (im=MB_recinserted) or (im=MB_recchanged)
	    call usr_warn("re")
	    let om=MB_open
	  otherwise
	    let om=im
	end case
    end if
    let int_flag=false
    return om, op
end function
#
#  similar to usr_action but for input arrays
#
function usr_direction(sr, mr, empty)
    define sr, mr	smallint,
	   empty	smallint,
	   lk		integer,
	   c, m, s	smallint

    let c=arr_curr()
    let m=arr_count()
    let s=scr_line()
    let lk=fgl_lastkey()
    case
      when (c=1) and (lk=fgl_keyval("up") or lk=fgl_keyval("left"))
	return -1					#exit & go back
      when (c<=sr) and (lk=fgl_keyval("prevpage"))
	return -1
      when (c=mr) and (lk=fgl_keyval("down") or lk=13)
	return 1					#exit & go on
      when (c>m-s) and (lk=fgl_keyval("nextpage"))
	return 1
      when (c<m) or not empty
      when (lk=fgl_keyval("down") or lk=13)
	return 1
    end case
    return 0
end function
#
#  selects help message for confirm & cont based routines
#
function usr_help(cur, def)
    define cur, def	smallint

    let help_no_cur=cur
    if def
    then
	let help_no_def=cur
    end if
end function
#
#  waits for a generic confirmation
#
function usr_ask(s, i)
    define s	char(60),
	   i, h	integer,
	   y, n	char(1),
	   cnf	char(18),
	   ans	char(1)

    let int_flag=false
    let cnf=txt_retrieve("YES")
    let y=upshift(cnf[1])
    let cnf=txt_retrieve("NO")
    let n=upshift(cnf[1])
    let cnf="(", y, "/", n, ")"
    while  true
	case
	  when (length(cur_msg)=0)
	  when isbatch
	    call fd_writeln(fd, cur_msg)
	  otherwise
	    error " ", cur_msg clipped, " "
	end case
	let ans=" "
	if isbatch
	then
	    display " ", s clipped, " ", cnf clipped
	    let ans=ascii(fgl_getkey())
	else
	    message " ", s clipped, " ", cnf clipped
	    let ans=ascii(fgl_getkey())
	end if
	case
	  when int_flag and i
	    let i=false
	  when int_flag or upshift(ans)=n
	    let int_flag=false
	    let i=false
	  when upshift(ans)=y
	    let i=true
	  when (not isbatch) and fgl_lastkey()=fgl_keyval("help")
	    call help_show(help_no_cur)
	    continue while
	  otherwise
	    let cnf="(", txt_retrieve("hit") clipped, " ", y, "/", n, ")"
	    continue while
	end case
	message ""
	let cur_msg=""
	let help_no_cur=help_no_def
	return i
    end while
end function
#
#  waits for a key before continuing execution / special case for shell funct
#
function shl_hitkey()
    define p    char(78),
	   ans	char(1)

    let p=txt_retrieve("hitkey")
    let p[1]=upshift(p[1])
#
# can't use display: writes a CR
#	    message, display at: clears screen
#
    prompt " ", p clipped for char ans
        on key (accept, escape)
	  goto ex_prompt
    end prompt
label ex_prompt:
    let int_flag=false
end function
#
#  issues error message and checks whether to continue
#
function usr_confirm(em)
    define em   char(14),
           s    char(60)

    let s=txt_retrieve("proceed") clipped, "?"
    case
      when length(em)=0
      when isbatch
	let s=txt_retrieve(em) clipped, " ", s clipped
      otherwise
	call hold_warning()
	call usr_warn(em)
    end case
    return usr_ask(s, false)
end function
#
#  show a help message (w/o crashing...)
#
function help_show(hm)
    define hm	smallint

    if (hm is not null)
    then
	call showhelp(hm)
#
#  4GL sets a global variable named efcode (which can't be used within
#  4GL code) instead of setting status, the result of this being that the
#  poor programmer has no way of knowing about (or trapping, for that matter)
#  a "missing message" error.
#  To make things worse, on entry menus, inputs, etc copy efcode value into
#  status so that the poor programmer gets a "no help message" error miles
#  away from where the error happened!
#  as a workaround usr_testhelp checks & clears efcode
#  
	if usr_testhelp()
	then
	    return
	end if
    end if
    call usr_warn("NH")
end function
#
#  nul - self-explaining
#
function nul(arg)
    define   arg   char(40)

    if arg is not null and arg[1]!=" "
    then
        return(false)
    else
        call usr_warn(-1305)
        return(true)
    end if
end function
#
#  flags to postpone messages until next major screen output
#
function hold_warning()
    if cur_msg is null
    then
	let cur_msg=" "
    end if
end function
#
#  display saved error message
#
function pop_warning()
    if length(cur_msg)>0
    then
	error " ", cur_msg clipped, " "
    end if
    let cur_msg=""
end function
#
#  main error message dispatcher
#
function usr_warn(msg)
    define msg	char(14),
	   str	char(80)

    case
      when isbatch
	if (length(msg))
	then
            call fd_writeln(fd, txt_retrieve(msg))
	end if
      when cur_msg is not null
	let cur_msg=txt_retrieve(msg)
      otherwise
	let str=txt_retrieve(msg)
	error " ", str clipped, " "
    end case
end function
#
#  displayes error text
#
function usr_notify(str)
    define str	char(78)

    case
      when (length(str)=0)
      when (isbatch)
	call fd_writeln(fd, str)
      otherwise
	error " ", str clipped, " "
    end case
end function
#
#  main status message dispatcher
#
function usr_status(msg)
    define msg	char(14),
	   str	char(80)

    case
      when isbatch
	if (length(msg))
	then
	    let str=txt_retrieve(msg)
	    display msg
	end if
      when (length(msg)=0)
	message ""
      otherwise
	let str=txt_retrieve(msg)
	message " ", str clipped, " "
    end case
end function
#
#  just a synonym for message
#
function usr_inform(str)
    define str	char(80)

    if (length(str)!=0)
    then
	message ""
    else
	message " ", str clipped, " "
    end if
end function
