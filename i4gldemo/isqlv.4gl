#	ISQLV.4gl - Query Language Viewer
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

#include "idefl.4gh"
#include "csqll.4gh"
#define K_edlines 200
#globals "itedg.4gl"
globals "isqlg.4gl"

#
#  query language viewer
#
function sql_viewer()
    define om, op,
	   im, ip,
	   i, j, k	smallint,
	   touch	integer,
	   dest		char(80)

    whenever error call trap_err
    call menu_start()
    let i=pan_add(1)
    call uni_setid(1)
    call uni_looks("sqlf", 8)
    call uni_setid(2)
    call uni_looks("sqlf", 8)
    call mnu_mask(MB_print, MB_print, K_allpanes)
    call mnu_mask(MB_sqledit, MB_filewrite, K_allpanes)
    call mnu_record(MB_print, MB_print, 2)
    call mnu_record(MB_sqlexec, MB_sqlexec, 1)
    call mnu_record(MB_filewrite, MB_filewrite, 1)
    let touch=K_dml+K_noconf
    call mnu_mask(MB_sqluse, MB_sqluse, K_allpanes)
    let om=MB_init
    let op=""
    while true
	call pan_enter()
	call uni_upstream(om, op) returning om, op
	call uni_upstream(om, op) returning om, op
	call menu_get(om, op) returning om, op
	call pan_enter()
	call uni_downstream(om, op) returning om, op
	call uni_downstream(om, op) returning im, ip
	let om=""
	let op=""
	case
	  when (im=MB_exit)
	    call sql_release(vars)
	    call hst_dispose(vars)
	    exit while
	  when (im=MB_sqledit)
	    call query_input()
	      returning om, op
	  when (im=MB_sqluse)
	    call dbedit_input()
	      returning om, op
	  when (im=MB_sqlexec)
	    call query_execute(vars, touch)
	      returning om, op
	  when (im=MB_fileread)
	    call pan_enter()
	    call uni_setid(1)
	    let i=ted_fileread("", K_edlines, 1, 0)
	    if (i=0 and not int_flag)
	    then
		call ted_touni()
		let om=MB_open
		let op=1
	    else
		call ted_checkop(i)
	    end if
	  when (im=MB_filewrite)
	    call pan_enter()
	    call uni_setid(1)
	    call ted_fromuni()
	    call ted_checkop(ted_filewrite("", pan_max()))
	  when (im!=MB_display) and (im!=MB_open) and (im!=MB_declare)
	    call invalid_message(im)
	    let om=MB_init
	    let op=""
	end case
    end while
end function
#
#  sql statement input
#
function query_input()
    define i	integer

    let i=pan_set_current(1)
    call uni_setid(1)
    call ted_fromuni()
    let int_flag=false
    call usr_header(txt_retrieve("HD"), true, true)
    call ted_start(8, K_edlines, true, true)
    call ted_features(1, -1)
    let i=ted_input("", false)
    if not int_flag
    then
	call ted_touni()
	call pan_set_focussed(2)
	call mnh_nextoption(MB_sqlexec, "")
    end if
    return MB_recchanged, 1		#redisplay editor pane only
end function
#
#  sql statement input using an external editor
#
function dbedit_input()
    define i	integer

    let i=pan_set_current(1)
    call uni_setid(1)
    call ted_fromuni()
    let int_flag=false
    call ted_start(8, K_edlines, true, true)
    let i=ted_dbedit(pan_max())
    if (i=0)
    then
	call ted_touni()
	call pan_set_focussed(2)
	call mnh_nextoption(MB_sqlexec, "")
    else
	call ted_checkop(i)
    end if
    return MB_recchanged, 1		#redisplay editor pane only
end function
#
#  execute query
#
function query_execute(vars, touch)
    define vars		byte,
	   touch	integer,
	   query	text,
	   textvar	text,
	   s, r		integer,
	   om, op	smallint,
	   txt  	char(80)

    let om=MB_display
    let op=MP_nodisplay
    let s=pan_set_current(1)
    call uni_setid(1)
    call uni_gettext(query)
    if (not status)
    then
	call usr_status("wa")
	let r=sql_execute2(query, 0, touch+K_verbose+K_preserve,
			78, vars, textvar)
	call usr_status("")
	if (r!=0)
	then
	    call usr_notify(sql_message(r))
	    call pan_set_focussed(1)
	    call mnh_nextoption(MB_sqledit, "")
	else
	    call uni_setid(2)
	    call uni_settext(textvar)
	    call pan_set_focussed(2)
	    let om=MB_declare
	    let op=2
	end if
	call asc_free(query)
	call asc_free(textvar)
    end if
    return om, op
end function
