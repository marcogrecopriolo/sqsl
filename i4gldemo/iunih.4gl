#	IUNIH.4gl  -  single selection field help display
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
#include "iunih.4gh"

#
#  single selection field help display
#
function uni_help(tbl, cond, flags, buf)
    define
	tbl	char(4),
	cond	char(300),
	flags	integer,
	buf	char(8),
	pkey	char(8),
	fname	char(64),
	str	char(80),
	p_hlp	text,
	errcode	integer,
	cmt	char(36),
	ask	integer,
        i,j,k,
	l,m	integer,
	fd,
	dsrc,
	rdwop,
	linenum,
	oldline,
	firstline,
	maxlines,
	pglen	smallint,
	getit	integer,
	keyhit	char(1),
	fp	char(PATHSIZE)

    whenever error call trap_err
    let errcode=0
    call asc_locate(p_hlp)
    let ask=false
    case
      when (tbl="file")
	call get_token(cond)
	   returning fname, cond
	while (length(fname)>0)
	    let fd=file_open(resourcepath(fname), "r")
	    while (not fd_eof(fd))
		let str=txt_commentdrop(fd_read(fd))
		call fd_droptoeol(fd)
		call get_token(str)
		  returning pkey, str
		if length(str)>0
		then
		    let str=pkey clipped, " ", str clipped
		    call asc_addline(p_hlp, str, "")
		end if
	    end while
	    let j=fd_close(fd)
	    call get_token(cond)
	      returning fname, cond
	end while
      when (tbl="pipe")
	let fd=pipe_open(cond, "r")
	while (not fd_eof(fd))
	    let str=txt_commentdrop(fd_read(fd))
	    call get_token(str)
	      returning pkey, str
	    if length(str)>0
	    then
		let str=pkey clipped, " ", str clipped
		call asc_addline(p_hlp, str, "")
	    end if
	end while
	let j=fd_close(fd)
      when (tbl="sql")
	let fd=sql_open(cond)
	if (status=0)
	then
	    while (sql_fetch(fd)=0)
		let pkey=sql_field(fd, 1)
		let str=sql_field(fd, 2)
		case
		  when status
		    exit while
		  when length(pkey) and length(str)
		    let str="  ", pkey clipped, " ", str clipped
		    call asc_addline(p_hlp, str, "")
		end case
	    end while
	end if
	let errcode=status
	call sql_free(fd)
      otherwise
	if (tbl is not null)
	then
	    if cond clipped!=" "
	    then
		let cond="where ", cond clipped
	    end if
	    let cond="select code_", tbl,", desc_", tbl," from ", tbl,
		     " ", cond clipped, " order by code_", tbl
	end if
	whenever error continue
	prepare uni_search from cond
	let errcode=status
	whenever error call trap_err
	if (status=0)
	then
	    whenever error continue
	    declare uni_hlp cursor for uni_search
	    let errcode=status
	    whenever error call trap_err
	    foreach uni_hlp into pkey, str
		let str=pkey clipped, " ", str clipped
		call asc_addline(p_hlp, str, "")
	    end foreach
	    SQ_freecurs uni_hlp
	    free uni_search
	end if
    end case
    let int_flag=false
    let maxlines=asc_lines(p_hlp)
    if (maxlines!=0 and errcode=notfound)
    then
	let errcode=0
    end if
    let linenum=1
    case
      when (maxlines=0)
	let errcode=notfound
	let ask=bwand(flags, KH_insnew)
	if (bwand(flags, KH_error))
	then
	    call usr_warn("nv")
	end if
     when (errcode)
	if (bwand(flags, KH_error))
	then
	    call usr_warn(errcode)
	end if
      when (maxlines>1) or (flags>0)
	let pglen=14
	let j=0
	let buf=str_left(upshift(buf))
	let l=length(buf)
	for i=1 to maxlines
	    call get_token(asc_retrieveline(p_hlp, i))
		returning pkey, str
	    let k=str_compare(pkey, buf)
	    let m=str_compare(str, buf)
	    case
	      when (k=l) or (m=l)
		let linenum=i
		exit for
	      when (k>j)
		let linenum=i
		let j=k
	      when (m>j)
		let linenum=i
		let j=m
	    end case
	end for
	if (linenum>pglen)
	then
	    let firstline=linenum-1
	else
	    let firstline=0
	end if
	let fp=resourcepath("hl1w")
	open window uni_help_win at 4,29
	  with form fp
	  attribute (border, form line first, comment line last,
		     prompt line last, message line last)
	if (flags>0)
	then
	    let cmt=txt_retrieve("in")
	    display by name cmt
	end if
	let rdwop=1
	let dsrc=false
	let getit=true
	while getit
	    case
	      when rdwop!=0
		let l=1
		let j=minint(firstline+pglen, maxlines)
		for k=firstline+1 to j
		    call get_token(asc_retrieveline(p_hlp, k))
		      returning pkey, str
		    let pkey=str_right(pkey)
		    case
		      when (k!=linenum)
			display pkey, str to s_hlp[l].* attribute (normal)
		      when dsrc
			display pkey to s_hlp[l].pkey
			  attribute (normal)
			display str to s_hlp[l].description
			  attribute (reverse)
		      otherwise
			display pkey to s_hlp[l].pkey
			  attribute (reverse)
			display str to s_hlp[l].description
			  attribute (normal)
		    end case
		    let l=l+1
		end for
		if (rdwop<0)
		then
		    for k=l to pglen
			display "", "" to s_hlp[k].* attribute (normal)
		    end for
		end if
	      when (oldline!=linenum)
		if (oldline>0)
		then
		    let l=oldline-firstline
		    call get_token(asc_retrieveline(p_hlp, oldline))
		      returning pkey, str
		    let pkey=str_right(pkey)
		    display pkey, str
		      to s_hlp[l].* attribute (normal)
		end if
		let l=linenum-firstline
		call get_token(asc_retrieveline(p_hlp, linenum))
		  returning pkey, str
		let pkey=str_right(pkey)
		if dsrc
		then
		    display pkey to s_hlp[l].pkey
		      attribute (normal)
		    display str to s_hlp[l].description
		      attribute (reverse)
		else
		    display pkey to s_hlp[l].pkey
		      attribute (reverse)
		    display str to s_hlp[l].description
		      attribute (normal)
		end if
	    end case
	    let rdwop=0
	    let oldline=linenum
	    let i=fgl_getkey()
	    run "echo "||i||" > test.out"
	    let int_flag=(i=fgl_keyval("interrupt"))
	    let keyhit=ascii(i)
	    case
	      when int_flag
		let getit=false
	      when i=fgl_keyval("insert")
		if (flags>0)
		then
		    let ask=true
		    let getit=false
		end if
	      when i=fgl_keyval("help")
		call showhelp(21)
		let int_flag=false
	      when i=fgl_keyval("accept") or i=fgl_keyval("escape")
		let getit=false
	      when i=9
		let dsrc=not dsrc
		let oldline=-1
	      when i=fgl_keyval("nextpage") or i=fgl_keyval("control-f")
		case
		  when (linenum+pglen-1<maxlines)
		    let linenum=linenum+pglen-1
		    let firstline=firstline+pglen-1
		    let rdwop=-1
		  when (firstline+pglen-1<maxlines)
		    let linenum=maxlines
		    let firstline=firstline+pglen-1
		    let rdwop=-1
		  when (linenum<maxlines)
		    let linenum=maxlines
		end case
	      when i=fgl_keyval("prevpage") or i=fgl_keyval("control-g")
		case
		  when (firstline>=pglen-1)
		    let linenum=linenum-pglen+1
		    let firstline=firstline-pglen+1
		    let rdwop=1
		  when (firstline>0) and (linenum>pglen-1)
		    let firstline=0
		    let linenum=linenum-pglen+1
		    let rdwop=-1
		  when (firstline>0)
		    let firstline=0
		    let linenum=1
		    let rdwop=-1
		  when (linenum>1)
		    let linenum=1
		end case
	      when i=fgl_keyval("F1")
		case
		  when (linenum=1)
		  when (firstline=0)
		    let linenum=1
		  otherwise
		    let linenum=1
		    let firstline=0
		    let rdwop=-1
		end case
	      when i=fgl_keyval("up")
		case
		  when (linenum-firstline>1)
		    let linenum=linenum-1
		  when (linenum>1)
		    scroll s_hlp.* down
		    let linenum=linenum-1
		    let firstline=firstline-1
		  otherwise
		    case
		      when (linenum>=maxlines)
		      when (linenum-firstline<pglen) and
			   (maxlines-firstline<=pglen)
			let linenum=maxlines
		      otherwise
			let linenum=maxlines
			let firstline=linenum-pglen
			let rdwop=-1
		   end case
		end case
	      when i=fgl_keyval("down") or (i=13)
		case
		  when (linenum>=maxlines)
		    case
		      when (linenum=1)
		      when (firstline=0)
			let linenum=1
		      otherwise
			let linenum=1
			let firstline=0
			let rdwop=-1
		    end case
		  when (linenum-firstline<pglen)
		    let linenum=linenum+1
		  otherwise
		    scroll s_hlp.* up
		    let linenum=linenum+1
		    let firstline=firstline+1
		end case
	      when i=fgl_keyval("F4")
		case
		  when (linenum>=maxlines)
		  when (linenum-firstline<pglen) and (maxlines-firstline<=pglen)
		    let linenum=maxlines
		  otherwise
		    let linenum=maxlines
		    let firstline=linenum-pglen
		    let rdwop=-1
		end case
	      when keyhit is null
	      otherwise
		let keyhit=upshift(keyhit)
		let j=maxlines
		let k=linenum
		while (j!=0)
		    if (k=maxlines)
		    then
			let k=1
		    else
			let k=k+1
		    end if
		    call get_token(asc_retrieveline(p_hlp, k))
		      returning pkey, str
		    if dsrc
		    then
			let l=(keyhit=upshift(str[1]))
		    else
			let l=(keyhit=upshift(pkey[1]))
		    end if
		    if (l)
		    then
		        let linenum=k
			if (k<=firstline) or (k>firstline+pglen)
			then
			    let firstline=linenum-1
			    let rdwop=-1
			end if
			exit while
		    end if
		    let j=j-1
		end while
	    end case
	end while
	close window uni_help_win
    end case
    if int_flag or (maxlines=0) or ask
    then
        let pkey=""
	let str=""
        let int_flag=false
    else
	call get_token(asc_retrieveline(p_hlp, linenum))
	  returning pkey, str
    end if
    if ask
    then
	if usr_ask(txt_retrieve("IN"), false)
	then
	    return "", " ?"
	end if
    end if
    let status=errcode
    return pkey, str
end function
