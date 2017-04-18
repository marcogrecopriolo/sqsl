#	IMINV.4gl  -  main program
#
#	The Structured Query Scripting Language
#	(Borrowed from) 4gwEd, a dbaccess like 4glWorks based application
#	Copyright (C) 1999-2009 Marco Greco (marco@4glworks.com)
#
#	Initial release: Oct 92
#	Current release: Nov 09
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

#include "csqll.4gh"
globals "isqlg.4gl"

#
#  main program
#
main
    whenever error call trap_err
    defer interrupt
    defer quit
    call parse_options()
    call sql_viewer()
    clear screen
end main
#
#  initializes the application
#
function app_init()
    define modname	char(64),
	   b		integer
    
    whenever error call trap_err
    call startlog(resourcepath("error.log"))
    let b=usr_isbatch()
    call txt_init()
    call usr_init()
    if (b)
    then
	let b=usr_setbatch(b)
	return
    end if
    call usr_greet("SQSL", "", "")
    let modname=filesplit(arg_val(0), false, true, false) clipped, ".hlp"
    call mnh_init()
    call ted_init()
    call uni_init()	--
    call hor_init()
    options
	input		wrap,
	sql interrupt	off,
	field order	constrained,
	menu		line first,
	prompt		line last-1,
	form		line first+3,
	comment		line first+1,
	message		line last-1,
	error		line last,
	help key	control-w,
	insert key	control-n,
	delete key	control-e,
	next key	control-f,
	previous key	control-g,
	accept key	esc,
	help file	modname
    call mnh_help(10, true)
    call usr_help(12, true)
    call usr_status("")
end function
#
#  parses options, executes script from the command line
#
function parse_options()
    define query	text,
	   rcscript	text,
	   opt, inf 	char(255),
	   fdi, fdo,
	   fdr,
	   i, r, d, 
	   dorc,
	   cnt,
	   hid,
	   flags, w	integer,
	   txtvar	text

    whenever error call trap_err
    let w=""
    let inf=""
    let dorc=true
    let cnt=1
#
# initialize symbol table
#
    let flags=K_verbose+K_dml+K_noconf+K_isbatch+K_preserve
    locate vars in memory
    let vars=null
    locate rcscript in memory
    let rcscript=null
    let r=sql_execute2(rcscript, 0, flags, w, vars, txtvar)
    call asc_free(txtvar)
    if (r)
    then
	let i=usr_setbatch(true)
	call app_init()
	call usr_notify(sql_message(r))
	exit program(1)
    end if
#
# parse options
#
    let i=1
    while (i<=num_args())
	let opt=arg_val(i)
	case
	  when (opt="-") or (opt[1]!="-")
	    exit while
	  when (opt="--")
	    let i=i+1
	    exit while
	  when (opt="-w") and (w is null) and (i<num_args())
	    let i=i+1
	    let w=str2int(arg_val(i))
	    if (w is null)
	    then
		call print_usage()
	    end if
	  when (opt="-H") and not (bwand(flags, K_html))
		let flags=flags+K_html
	  when (opt="-V")
	    call print_version()
	    if (num_args()!=1)
	    then
		call print_usage()
	    end if
	    exit program(0)
	  when (opt="-N")
	    let dorc=false
	  when (opt="-D") and (i<num_args())
	    let i=i+1
#
# my version of 4gl throws a wobbly here if let is used
#
	    call sql_evalassign(arg_val(i), 1, length(arg_val(i)), vars)
		returning r
	    if (r<=length(arg_val(i)))
	    then
		let status=-201
	    end if
	    if (status)
	    then
		let r=status
		let d=usr_setbatch(true)
		call app_init()
		call usr_notify(sql_message(r))
		exit program(2)
	    end if
	  otherwise
	    call print_usage()
	end case
	let i=i+1
    end while
#
# set command line arguments from here on
#
    let hid=hst_addnode(vars, 0, "argv", 10)
    while (i<=num_args())
	if (cnt=1)
	then
	    let inf=arg_val(i)
	end if
	call hst_add(vars, hid, cnt, arg_val(i), 109)
	let cnt=cnt+1
	let i=i+1
    end while
    if (inf is null) and (w or bwand(flags, K_html))
    then
	let inf="-"
    end if 
#
# complete ARGV, ARGC initialization
#
    call hst_add(vars, hid, "0", arg_val(0), 109)
    call hst_add(vars, 0, "argc", cnt, 102)
#
# fire rc script
#
    if (dorc)
    then
	let fdr=file_open(filepath(fgl_getenv("HOME"), ".sqslrc"), "r")
	if (not status)
	then
	    call fd_blobread(fdr, rcscript)
	    let r=status
	    let i=fd_close(fdi)
	    if (not r)
	    then
		call asc_initialize(rcscript)
		let r=sql_execute2(rcscript, 0, flags-K_verbose, w, vars,
				   txtvar)
		call asc_free(rcscript)
		call asc_free(txtvar)
		if (r)
		then
		    let i=usr_setbatch(true)
		    call app_init()
		    call usr_notify(sql_message(r))
		    exit program(3)
		end if
	    end if
	end if
    end if
#
# initialize output streams
#
    if (inf is null)
    then
	let fdo=0
    else
	let i=usr_setbatch(true)
	let fdo=stdfd_open(1)
    end if
#
# initialize modules - if form mode, we're done
#
    call app_init()
    if (inf is null)
    then
	return
    end if
#
# initialize input stream
#
    if (w is null)
    then
	let w=78
    end if
    if (inf="-")
    then
	let fdi=stdfd_open(0)
    else
#
# ditto
#
	let fdi=file_open(inf, "r")
	let i=status
	if i
	then
	    call usr_warn(i)
	    exit program(4)
	end if
    end if
#
#  execue the script
#
    locate query in memory
    call fd_blobread(fdi, query)
    let i=fd_close(fdi)
    call asc_initialize(query)
    let r=sql_execute2(query, fdo, flags, w, vars, txtvar)
    call asc_free(query)
    call asc_free(txtvar)
    call sql_release(vars)
    call hst_dispose(vars)
    if (r)
    then
	call usr_notify(sql_message(r))
	let r=1
    end if
    exit program(r)
end function
#
# display usage
#
function print_usage()
    define usage	char(80)

    display "Usage:"
    display ""
    let usage=arg_val(0),
	      " [-V |[-H] [-w width] [-N] [-D var=expr...] [filename [arg...]]]"
    display usage
    exit program(128)
end function
#
# display version
#
function print_version()
    display PACKAGE_NAME, ":", COMP_NAME, " Version ", VERSION,
	    " (", PACKAGE_DATE, ")"
    display " "
end function
