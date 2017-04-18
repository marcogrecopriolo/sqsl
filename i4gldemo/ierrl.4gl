#	IERRL.4gl  -  Error trappers
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2002 Marco Greco (marco@4glworks.com)
#
#	Initial release: Oct 92
#	Current release: Jan 02
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

#	IERRL.4gh  -  Error trappers strings definitions, eng
#
#	The 4glWorks application framework
#	Copyright (C) 1992-2002 Marco Greco (marco@4glworks.com)
#
#	Initial release: Apr 96
#	Current release: Jul 03
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

#define KS_useractions	"Message chain: "
#define KS_userid	"User: "
#define KS_nl		ascii(10)
#define KS_exit		": hit E to exit"
#define KS_contorexit	": hit <Interrupt> to continue, E to exit "
#define KS_error	" Error "
#define KS_closed	" The engine has been shut down"
#define KS_access       " Access rights violation"
#define KS_keyexit	"E"

#define KS_badmessage	"Unexpected message: "
#define KS_badfile	"Text file not found: "

define opt_count	smallint,
       opt_list		array[10] of record
			    msg, parm	smallint
			end record,
       fgw_error	integer

#
#  default error trapper
#
function trap_err()
    define
	l_int   integer,
	c_flg	integer,
	l_err	integer,
	l_ans   integer,
	t_err   char(78),
	i	smallint

    whenever error continue
    let l_err=status
    let l_int=int_flag
#
#  OK, I'm probably asking for trouble with the TSS, but at least I
#  take the time to convert the option list to a string only in case
#  of error
#
    let t_err=""
    for i=1 to opt_count
	let t_err=t_err clipped, " ", str_left(opt_list[i].msg) clipped,
				 "/", str_left(opt_list[i].parm)
    end for
    let t_err=KS_useractions, t_err clipped, KS_nl,
	      KS_userid, get_login() clipped, KS_nl
    call errorlog(t_err)
    let int_flag=false
    open window err_win at 23,1 with 1 rows, 79 columns
    let c_flg=true
    case
      when (l_err is null)
	let t_err=KS_error, fgw_error using "-<<<<&",
		  KS_contorexit
	let c_flg=false
      when (l_err=0)
	let l_err=-1318
	let t_err=KS_error, l_err using "-<<<<&", KS_exit
      when (l_err=-1) or (l_err=-459)
	let t_err=KS_closed, KS_exit
      when (l_err>=-275 and l_err<=-272) or (l_err>=-389 and l_err<=-387)
	let t_err=KS_access, KS_exit
      otherwise
	let t_err=KS_error, l_err using "-<<<<&",
		  KS_contorexit
	let c_flg=false
    end case
    while (not int_flag) or c_flg
      display t_err at 1, 1
	let l_ans=fgl_getkey()
	if upshift(ascii(l_ans))=KS_keyexit
	then
	    clear screen
	    exit program
	end if
    end while
    close window err_win
    let int_flag=l_int
    let status=l_err
end function
#
#  "unknown/unexpected message" error handling
#
function invalid_message(m)
    define m	smallint,
	   s	char(70)

    let s=KS_badmessage, m using "<<<<&"
    call errorlog(s)
    let fgw_error=m
    let status=""
    call trap_err()
end function
#
#  "text file not found" error handling
#
function invalid_file(f)
    define f	char(14),
	   s	char(70)

    let s=KS_badfile, f clipped
    call errorlog(s)
    let fgw_error=100
    let status=""
    call trap_err()
end function
#
#  put current option in option list
#
function opt_put(msg, parm)
    define msg, parm	smallint

    let opt_count=opt_count+1
    let opt_list[opt_count].msg=msg
    let opt_list[opt_count].parm=parm
end function
#
#  clear option list
#
function opt_clear()
    let opt_count=0
end function
