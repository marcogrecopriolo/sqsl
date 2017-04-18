#	CTXTL.4gl  -  language dependent text retrieval
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2008 Marco Greco (marco@4glworks.com)
#
#	Initial release: Oct 92
#	Current release: May 08
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
 
define
    fcnt	smallint,
    acc		integer,
    modname	char(14),
    hash	byte
#
#  module initialization
#
function txt_init()
    whenever error call trap_err
    let fcnt=0
    let acc=0
    let modname=filesplit(arg_val(0), false, true, false) clipped, ".txt"
    call hst_locate(hash, 20)
end function
#
#  "implicit file name" & informix messages text retrieval
#
function txt_retrieve(msg)
    define msg		char(14),
	   fname	char(14),
	   t		char(78),
	   i,j		integer

    let i=pos(msg, ".")
    if (i>0)
    then
	let fname=msg[1,i], "txt"
	let i=i+1
	let j=length(msg)
	let msg=msg[i,j]
	let t=txt_fetch(fname, msg)
    else
	let i=str2int(msg)
	if (i is not null)
	then
	    let t=err_get(i)
#
#  drop trailing LF left by err_get
#
	    let i=length(t)
	    while (i>0) and t[i, i]<" "
		let i=i-1
	    end while
	    if (i>0)
	    then
		let t=t[1, i]
	    else
		let t=""
	    end if
	else
	    let t=txt_fetch(modname, msg)
	end if
    end if
    return t
end function
#
#  "esplicit file name" text retrieval
#
function txt_fetch(fname, msg)
    define fname	char(14),
	   msg		char(8),
	   desc		char(80),
	   n		integer

    let n=load_txt(fname)
    if (n is not null)
    then
	call hst_get(hash, n, msg)
	  returning n, desc
	if (desc is not null)
	then
	    return desc
	end if
    end if
    let msg=msg clipped,"?!"
    return msg
end function
#
#  loads txt file
#
function load_txt(fname)
    define fname	char(14),
	   msg		char(40),
	   l_buf	char(80),
	   fd		smallint,
	   i, j		smallint,
	   n, f		integer

    call hst_get(hash, 0, fname)
      returning n, f
    if (n>0)
    then
	return n
    end if
    let n=hst_addnode(hash, 0, fname, 32)
    if (status!=0 or n<=0)
    then
	return ""
    end if
#
#  just in case we are trying to recover from something else!
#
    let status=0
    let fd=file_open(filepath(fgl_getenv("SQSLDIR"), fname), "r")
    if (status!=0)
    then
	call invalid_file(fname)
	return ""
    end if
    while (not fd_eof(fd))
	let l_buf=fd_read(fd)
	call fd_droptoeol(fd)
	let l_buf=txt_commentdrop(l_buf)
	call get_token(l_buf)
	  returning msg, l_buf
	if (l_buf is not null)
	then
	    call hst_add(hash, n, msg, l_buf, -1)
	end if
    end while
    let j=fd_close(fd)
    return n
end function
#
#  removes comments from lines of text
#
function txt_commentdrop(desc)
    define desc	char(78),
	   i	smallint
	   
    let i=pos(desc, "#")
    case
      when (i>1)
        let desc=desc[1, i]
      when (i=1)
	return ""
    end case
    return desc clipped
end function
