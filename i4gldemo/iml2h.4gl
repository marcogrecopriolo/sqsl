#	IML2H.4gl  -  multiple selection help module
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
#  Multiple choice pick list
#
function multi_desc(tbl, cond, ln, sep, quotes, flags, buf)
    define
	tbl	char(4),
	cond	char(300),
	ln	smallint,
	sep	char(1),
	quotes	char(2),
	flags	integer,
	buf	char(8),
	fname	char(64),
	str	char(80),
	p_hlp	text,
	errcode	integer,
        i,j,k,l	integer,
	fd,
	rdwop,
	linenum,
	oldline,
	firstline,
	maxlines,
	pglen	smallint,
	ds	char(80),
	sn	smallint,
	getit	integer,
	keyhit	char(1),
	fp	char(PATHSIZE)

    whenever error call trap_err
    let errcode=0
    call asc_locate(p_hlp)
    case
      when (tbl="file")
	call get_token(cond)
	   returning fname, cond
	while (length(fname)>0)
	    let fd=file_open(resourcepath(fname), "r")
	    while (not fd_eof(fd))
		let str=txt_commentdrop(fd_read(fd))
		call fd_droptoeol(fd)
		if length(str)>0
		then
		    let str="  ", str clipped
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
	    call fd_droptoeol(fd)
	    if length(str)>0
	    then
		let str="  ", str clipped
		call asc_addline(p_hlp, str, "")
	    end if
	end while
	let j=fd_close(fd)
      when (tbl="sql")
	let fd=sql_open(cond)
	if (status=0)
	then
	    while (sql_fetch(fd)=0)
		let str="  ", sql_field(fd, 1) clipped
		if length(str)>0
		then
		    call asc_addline(p_hlp, str, "")
		end if
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
	    let cond="select desc_", tbl clipped," from ", tbl,
		     " ", cond clipped, " order by desc_", tbl
	end if
	whenever error continue
	prepare ml2_search from cond
	let errcode=status
	whenever error call trap_err
	if (status=0)
	then
	    whenever error continue
	    declare ml2_hlp cursor for ml2_search
	    let errcode=status
	    whenever error call trap_err
	    foreach ml2_hlp into str
		let str="  ", str clipped
		call asc_addline(p_hlp, str, "")
	    end foreach
	    SQ_freecurs ml2_hlp
	    free ml2_search
	end if
    end case
    let int_flag=false
    let linenum=1
    let maxlines=asc_lines(p_hlp)
    let sn=0
    if (maxlines!=0 and errcode=notfound)
    then
	let errcode=0
    end if
    case
      when (maxlines=0)
	let errcode=notfound
	if (bwand(flags, KH_error))
	then
	    call usr_warn("nv")
	end if
      when (errcode)
	if (bwand(flags, KH_error))
	then
	    call usr_warn(errcode)
	end if
      when (maxlines>1)
	let pglen=15
	let j=0
	let buf=str_left(upshift(buf))
	if (sep clipped!=" ")
	then
	    let i=pos(buf, sep)-1
	    if (i>0)
	    then
		let buf=buf[1, i]
	    end if
	end if
	let l=length(buf)
	for i=1 to maxlines
	    let str=asc_retrieveline(p_hlp, i)
	    let k=str_compare(upshift(str[3, 80]), buf)
	    case
	      when (k=l)
		let linenum=i
		exit for
	      when (k>j)
		let linenum=i
		let j=k
	    end case
	end for
	if (linenum>pglen)
	then
	    let firstline=linenum-1
	else
	    let firstline=0
	end if
	let fp=resourcepath("hl4w")
	open window mld_help_win at 4,33
	  with form fp
	  attribute (border, form line first, comment line last,
		     prompt line last, message line last)
	let rdwop=1
	let getit=true
	while getit
	    case
	      when rdwop!=0
		let l=1
		let j=minint(firstline+pglen, maxlines)
		for k=firstline+1 to j
		    let str=asc_retrieveline(p_hlp, k)
		    if (k!=linenum)
		    then
			display str to s_hlp[l].* attribute (normal)
		    else
			display str to s_hlp[l].* attribute (reverse)
		    end if
		    let l=l+1
		end for
		if (rdwop<0)
		then
		    for k=l to pglen
			display "" to s_hlp[k].* attribute (normal)
		    end for
		end if
	      when (oldline!=linenum)
		if (oldline>0)
		then
		    let str=asc_retrieveline(p_hlp, oldline)
		    let l=oldline-firstline
		    display str to s_hlp[l].* attribute (normal)
		end if
		let l=linenum-firstline
		let str=asc_retrieveline(p_hlp, linenum)
		display str to s_hlp[l].* attribute (reverse)
	    end case
	    let rdwop=0
	    let oldline=linenum
	    let i=fgl_getkey()
	    let int_flag=(i=fgl_keyval("interrupt"))
	    let keyhit=ascii(i)
	    case
	      when int_flag
		let getit=false
	       when i=fgl_keyval("help")
		call showhelp(24)
		let int_flag=false
	      when i=fgl_keyval("accept") or i=fgl_keyval("escape")
		let getit=false
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
	      when (i=13)
		call descsel_ml2(p_hlp, linenum, ln, sn)
		  returning sn
		goto fake_nobreak
	      when i=fgl_keyval("down")
label fake_nobreak:
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
	      when (i=32)
		call descsel_ml2(p_hlp, linenum, ln, sn)
		  returning sn
		let oldline=-1
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
		    let i=asc_linepos(p_hlp, k)+2
		    if (keyhit=upshift(asc_retrievestring(p_hlp, i, i)))
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
	close window mld_help_win
    end case
    let ds=""
    case
      when int_flag or (maxlines=0)
      when sn=0
	let str=asc_retrieveline(p_hlp, j)
	let ds=quotes[1, 1] clipped, str[3, 80] clipped,
	       quotes[2, 2] clipped
      otherwise
	let k=true
	for j=1 to maxlines
	    let str=asc_retrieveline(p_hlp, j)
	    case
	      when (str[1, 1]="*") and k
		let ds=quotes[1, 1] clipped, str[3, 80] clipped,
		       quotes[2, 2] clipped
		let k=false
	      when (str[1, 1]="*")
		let ds=ds clipped, sep, quotes[1, 1] clipped,
		       str[3, 80] clipped, quotes[2, 2] clipped
	    end case
	end for
    end case
    let int_flag=false
    let status=errcode
    return ds
end function
#
#  selection flag adj
#
function descsel_ml2(p_hlp, linenum, ln, sn)
    define p_hlp	text,
	   linenum	smallint,
	   ln, sn	smallint,
	   str		char(80),
	   l		smallint

    let str=asc_retrieveline(p_hlp, linenum)
    case
      when str[1]="*"
	let str[1]=" "
	let sn=sn-length(str[3, 80])-1
	if (sn<0)
	then
	    let sn=0
	end if
      when (sn=0)
	let str[1]="*"
	let sn=length(str[3, 80])
      when (sn+length(str[3, 80])<ln)
	let str[1]="*"
	let sn=sn+1+length(str[3, 80])
      otherwise
	call usr_warn("ns")
    end case
    let l=asc_linepos(p_hlp, linenum)
    call asc_changestring(p_hlp, str[1, 3], l, l+2)
    return sn
end function
