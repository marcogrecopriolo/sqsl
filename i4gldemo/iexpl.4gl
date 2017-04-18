#	IEXPL.4gl - SQL interpreter: expansion add ons, interactive applications
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2009 Marco Greco (marco@4glworks.com)
#
#	Initial release: Jun 98
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

# bug in the popquote/char routines with 32766 & 7

#define	K_varsize	300
#define	K_exprsize	8192
#define	K_scriptbuf	8192

#
#  remove/display "please wait" messages in between pick lists
#
function givestatus_sqsl(verbose, msg)
    define verbose	integer,
	   msg		char(2)

    whenever error call trap_err
    if verbose
    then
	call usr_status(msg)
    end if
end function
#
#  prompt user
#
function prompt_sql(d_mode, desc, verbose)
    define
	d_mode	integer,
	desc	char(K_varsize),
	verbose	integer,
	fp	char(PATHSIZE),
	o_str	char(K_varsize)

    let o_str=""
    call givestatus_sqsl(verbose, "")
    let fp=resourcepath("expw")
    open window exp_win at 12,3
      with form fp
      attribute (border, form line first, comment line last,
		 prompt line last, message line last)
    display by name desc
    if (d_mode)
    then
	input by name o_str
	  attribute (reverse)
    else
	input by name o_str
	  attribute (reverse, invisible)
    end if
    close window exp_win
    call givestatus_sqsl(verbose, "wa")
    if (int_flag)
    then
	return ""
    end if
    return o_str
end function
#
#  open various picklist
#
function picklist_sql(p_mode, q_mode, query, len, sep, quotes, verbose)
    define p_mode int,
	   q_mode char(20),
	   query char(K_exprsize),
	   len int,
	   sep char(K_varsize),
	   quotes char(2),
	   verbose int,
	   txt char(K_exprsize),
	   dummy char(1)

    call givestatus_sqsl(verbose, "")
    case p_mode
      when 8704			# 0x2200
	let txt=multi_desc(q_mode, query, len, sep, quotes, 0, "")
      when 4608			# 0x1200
	let txt=uni_desc(q_mode, query, 0, "")
      when 8448			# 0x2100
	let txt=multi_help(q_mode, query, len, sep, quotes, 0, "")
      when 4352			# 0x1100
	call uni_help(q_mode, query, 0, "")
	  returning txt, dummy
    end case
    call givestatus_sqsl(verbose, "wa")
    return txt
end function
