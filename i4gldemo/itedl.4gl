#	ITEDL.4gl - Text editor library
#
#	The 4glWorks application framework
#	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)
#
#	Initial release: Oct 96
#	Current release: May 16
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

#ifdef NT
#define DEFAULT_EDITOR	"notepad"
#define RM_FILE		"del"
#else
#define DEFAULT_EDITOR	"vi"
#define RM_FILE		"rm"
#endif

#include "idefl.4gh"

#define K_maxtedrows	200
#define K_maxtedwidth	80
#define K_pathsize	256


define
    p_ted	array[K_maxtedrows] of record
		    txt		char(K_maxtedwidth),
		    d		char(1)
		end record,
    ted_count	smallint,
    ted_file	char(K_pathsize),
    max_screen_row,
    max_text_row,
    no_empty_lines,
    some_text_needed,
    autowrap,
    read_write_allowed	smallint

#
#  module initialization
#
function ted_init()
    whenever error call trap_err
    let ted_file=""
    let ted_count=""
    let autowrap=true
    let read_write_allowed=1
end function
#
#  text editor initialization (don't call after input stmt initiated)
#
function ted_start(s, t, e, n)
    define s, t, e, n	smallint

    let max_screen_row=s
    if (t<0 or t>K_maxtedrows)
    then
	let max_text_row=K_maxtedrows
    else
	let max_text_row=t
    end if
    let no_empty_lines=e
    let some_text_needed=n
end function
#
#  text editor features setup (call anytime)
#
function ted_features(a, i)
    define a, i	smallint

    let autowrap=a
    let read_write_allowed=i
end function
#
#  copies the ted buffer onto the current uni_scroller pane
#  needs pan_set_current & uni_setid beforehand!
#
function ted_touni()
    define i	integer

    call uni_create()
    for i=1 to ted_count
	call uni_newline(p_ted[i].txt)
    end for
end function
#
#  copies the current uni_scroller pane onto the ted buffer
#  needs pan_set_current & uni_setid beforehand!
#
function ted_fromuni()
    define i	integer

    let ted_count=minint(pan_max(), K_maxtedrows)
    for i=1 to ted_count
        let p_ted[i].txt=uni_retrieve(i)
    end for
end function
#
#  copies the ted buffer onto a text variable
#
function ted_totext(t)
    define t	text,
	   i	integer

    call asc_locate(t)
    for i=1 to ted_count
	call asc_addline(t, p_ted[i].txt, i)
    end for
end function
#
#  copies a text variable onto the ted buffer
#
function ted_fromtext(t)
    define t	text,
	   i, j	integer

    let ted_count=minint(asc_lines(t), K_maxtedrows)
    for i=1 to ted_count
        let p_ted[i].txt=asc_retrieveline(t, i)
    end for
end function
#
#  read file into buffer
#
function ted_fileread(f, m, s, z)
    define f	char(70),
	   m, s,
	   z	smallint,
	   t	char(78),
	   i, j	smallint,
	   fd	smallint

    let int_flag=false
    if (f is null)
    then
	let ted_file=file_input(ted_file, "")
	if int_flag
	then
	    return 0
	else
	    let f=ted_file
	end if
    end if
    let fd=file_open(filepath("", f), "r")
    if (status!=0)
    then
	return status
    end if
    let ted_count=z
    while (not fd_eof(fd) and ted_count<m)
	let t=fd_read(fd)
#
#  don't consider last NL as a blank line
#
	if (fd_eof(fd) and length(t)=0)
	then
	    exit while
	end if
	for i=ted_count to s step -1
	    let j=i+1
	    let p_ted[j].txt=p_ted[i].txt
	end for
	let p_ted[s].txt=t
	let ted_count=ted_count+1
	let s=s+1
    end while
    let i=fd_close(fd)
    while (ted_count>0)
	if (length(p_ted[ted_count].txt)=0)
	then
	    let ted_count=ted_count-1
	else
	    exit while
	end if
    end while
    return 0
end function
#
#  write buffer contents to file
#
function ted_filewrite(f, m)
    define f	char(70),
	   m, fd,
	   i, k smallint

    let int_flag=false
    if (f is null)
    then
	let ted_file=file_input(ted_file, "")
	if int_flag
	then
	    return 0
	else
	    let f=ted_file
	end if
    end if
    let fd=file_open(filepath("", f), "w")
    if (status=0)
    then
	for k=1 to m
	    call fd_writeln(fd, p_ted[k].txt)
	end for
	let i=fd_close(fd)
	return -1
    else
	return status
    end if
end function
#
#  returns individual text tine
#
function ted_getline(l)
    define l integer

    if (l>0) and (l<=ted_count)
    then
	return p_ted[l].txt
    else
	return ""
    end if
end function
#
#  sets individual line
#
function ted_setline(t, l)
    define t char(K_maxtedwidth),
    	   l integer

    if (l>0) and (l<=ted_count)
    then
	let p_ted[l].txt=t
    end if
end function
#
#  append line
#
function ted_appendline(t)
    define t char(K_maxtedwidth)

    if (ted_count<K_maxtedrows)
    then
	let ted_count=ted_count+1
	let p_ted[ted_count].txt=t
    end if
end function
#
#  deletes individual line
#
function ted_delline(l)
    define l int

    if (l>0) and (l<=ted_count)
    then
	while (l<ted_count)
	     let p_ted[l].*=p_ted[l+1].*
	     let l=l+1
	end while
	let ted_count=ted_count-1
    end if
end function
#
#  deletes all text
#
function ted_cleartext()
    let ted_count=0
end function
#
#  returns number of lines
#
function ted_getcount()
     return ted_count
end function
#
#  check text editor file io operation
#
function ted_checkop(r)
    define r smallint

    case
      when int_flag
	let int_flag=false
      when (r=-1)
	call usr_warn("re")
      when (r=2)
	call usr_warn("ishl.fm")
      when (r!=0)
	call usr_warn("np")
    end case
end function
#
#  invokes an external editor to modify the contents of the edit buffer
#
function ted_dbedit(m)
    define m, i	smallint,
	   e, f	char(K_pathsize)

    let f=tmp_filename("dbe")
    let e=fgl_getenv("DBEDIT")
    if (length(e)=0)
    then
	let e=DEFAULT_EDITOR
    end if
    let i=ted_filewrite(f, m)
    if (i=-1)
    then
	let e=e clipped, " ", f
	run e returning i
	case
	  when (i!=0)
	  when (status!=0)
	    let i=status
	  otherwise
	    let i=ted_fileread(f, max_text_row, 1, 0)
	end case
	let e=RM_FILE, " ", f
	run e
    end if
    return i
end function
#
#  text redisplay on input entry
#
function ted_show()
    define i, j	smallint

    let j=minint(ted_count, max_screen_row)
    for i=1 to j
	display p_ted[i].* to s_ted[i].*
    end for
    for i=j+1 to max_screen_row
	clear s_ted[i].*
    end for
end function
#
#  clear screen record
#
function ted_clear()
    define i	smallint

    for i=1 to max_screen_row
	clear s_ted[i].*
    end for
end function
#
#  text input
#
function ted_input(dir, enb)
    define dir, enb	smallint,
	   b_ted	array[K_maxtedrows] of char(K_maxtedwidth),
	   b, cr	smallint,
	   d, ld, sr	smallint,
	   i, j		smallint

    if (dir<0)
    then
	let dir=ted_count
    else
	let dir=1
    end if
    while dir is not null
	call set_count(ted_count)
	let ld=false
	for i=1 to ted_count
	    let b_ted[i]=p_ted[i].txt
	end for
	input array p_ted without defaults from s_ted.*

	  before row
	    let cr=arr_curr()
	    let sr=scr_line()
	    if (cr<dir)
	    then
		next field d
	    else
		let dir=""
	    end if

	  before insert
	    let ld=(cr>=arr_count())

	  after field txt
	    case
	      when not no_empty_lines
	      when ld
	      when (length(p_ted[cr].txt)=0)
		call usr_warn(-1305)
		next field txt
	    end case
	    if (fgl_lastkey()=fgl_keyval("autonext") and autowrap)
	    then
		let dir=cr+1
		let ted_count=arr_count()
		next field next
	    end if

	  after insert, delete
	    let ld=false

	  on key (control-o, F7)
	    if (read_write_allowed)
	    then
		let p_ted[cr].txt=get_fldbuf(txt)
		let i=maxint(cr, arr_count())
		let j=ted_fileread("", max_text_row, cr, i)
		if (j=0 and not int_flag)
		then
		    let dir=cr+ted_count-i
		    exit input
		else
		    call ted_checkop(j)
		end if
	    end if

	  on key (control-p, F8)
	    if (read_write_allowed)
	    then
		let p_ted[cr].txt=get_fldbuf(txt)
		call ted_checkop(ted_filewrite("", maxint(arr_curr(),
							  arr_count())))
	    end if

	  on key (control-u)
	    if (read_write_allowed<0)
	    then
		let p_ted[cr].txt=get_fldbuf(txt)
		let j=ted_dbedit(maxint(arr_curr(), arr_count()))
		if (j=0)
		then
		    let dir=cr+ted_count-i
		    exit input
		else
		    call ted_checkop(j)
		end if
	    end if

	  on key (control-w, help)
	    if (read_write_allowed)
	    then
		call help_show(4)
	    else
		call help_show(3)
	    end if

	  after row
	    let ld=false
	    if dir is null
	    then
		let d=usr_direction(max_screen_row, max_text_row,
				    p_ted[cr].txt is null and
				    (fgl_lastkey()!=13 or no_empty_lines))
		case
		  when (d=0)
		  when enb
		    goto after_input
		  otherwise
		    call usr_warn(-1309)
		    next field txt
		end case
#
#  wordwrap goes here
#
	    end if

	  after input
label after_input:
	    if int_flag
	    then
		for i=1 to ted_count
		    let p_ted[i].txt=b_ted[i]
		end for
		exit input
	    end if
	    let ted_count=arr_count()
	    if some_text_needed
	    then
		for i=1 to ted_count
		    if (length(p_ted[i].txt)>0)
		    then
			exit input
		    end if
		end for
		call usr_warn("NT")
#
#  can't use continue input here - might move cursor to next row
#  (that's the only reason why we need the while
#
		let dir=cr
	    end if
	end input
    end while
    while (ted_count>0)
	if (length(p_ted[ted_count].txt)>0)
	then
	    exit while
	else
	    let ted_count=ted_count-1
	end if
    end while
    return d
end function
