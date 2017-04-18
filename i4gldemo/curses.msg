#	CURSES.msg  -  Main help messages include file, eng
#
#	The Structured Query Scripting Language
#	(Borrowed from) The 4glWorks application framework
#	Copyright (C) 1992-2010 Marco Greco (marco@4glworks.com)
#
#	Initial release: Apr 96
#	Current release: Jul 10
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
.2
                        Editing keys

ctrl-A    toggles between insert and typeover mode

ctrl-D    erases all characters to the right of the cursor

ctrl-H    moves the cursor one character to the left (other keys: <BS>,
	  <left arrow>)

ctrl-J	  moves the cursor to the next field (other keys: <Enter>,
	  <down arrow>, <ctrl-M>)

ctrl-K	  moves the cursor to the previous field (other key: <up arrow>)

ctrl-L    moves the cursor one character to the right (other key: <right arrow>)

ctrl-R    redraws the screen

ctrl-X    erases the character under the cursor
.3
                        Editing keys

ctrl-A    toggles between insert and typeover mode

ctrl-D    erases all characters to the right of the cursor

ctrl-E    removes the row on which the cursor is (other key: <remove>)

ctrl-F    moves the cursor to the next page (other key: <Next Scrn>)

ctrl-G    moves the cursor to the previous page (other key: <Prev Scrn>)

ctrl-H    moves the cursor one character to the left (other keys: <BS>,
	  <left arrow>)

ctrl-J	  moves the cursor to the next field (other keys: <Enter>,
	  <down arrow>, <ctrl-M>)

ctrl-K	  moves the cursor to the previous field (other key: <up arrow>)

ctrl-L    moves the cursor one character to the right (other key: <right arrow>)

ctrl-N    Inserts a blank row before the row on which the cursor is (other
          key: <Insert Here>)

ctrl-R    redraws the screen

ctrl-X    erases the character under the cursor
.4
                        Editing keys

ctrl-A    toggles between insert and typeover mode

ctrl-D    erases all characters to the right of the cursor

ctrl-E    removes the row on which the cursor is (other key: <remove>)

ctrl-F    moves the cursor to the next page (other key: <Next Scrn>)

ctrl-G    moves the cursor to the previous page (other key: <Prev Scrn>)

ctrl-H    moves the cursor one character to the left (other keys: <BS>,
	  <left arrow>)

ctrl-J	  moves the cursor to the next field (other keys: <Enter>,
	  <down arrow>, <ctrl-M>)

ctrl-K	  moves the cursor to the previous field (other key: <up arrow>)

ctrl-L    moves the cursor one character to the right (other key:
	  <right arrow>)

ctrl-N    Inserts a blank row before the row on which the cursor is (other
          key: <Insert Here>)

ctrl-O    reads a file into the editing buffer

ctrl-P    writes the editing buffer contents into a file

ctrl-R    redraws the screen

ctrl-U	  Invokes an external editor to modify the editing buffer

ctrl-X    erases the character under the cursor
.10
To select one of the options:

o   move over it with the right or left arrow, or with the space bar and
    hit <RETURN> o <ENTER>;

o   simply hit the key corresponding to the first capital letter within
    the option

To repaint the screen after a message from the superuser

o   hit "Control" and "R" at the same time

If the screen contains data

o   <up arrow> or <control-J> moves to the previous row

o   <down arrow> or <control-K> moves to the next row

o   <Prev Scrn> or <control-G> moves to the previous page

o   <Next Scrn> or <control-F> moves to the next page

o   <PF1> moves to the first row (record, page)

o   <PF2> moves to the previous record

o   <PF3> moves to the next record

o   <PF4> moves to the last row (record, page)

If the screen has multiple panes, use <TAB> to cycle thru them
.11
To select one of the options:

o   move over it with the up or down arrow, or with the space bar and
    hit <RETURN> o <ENTER>;

o   simply hit the key corresponding to the first capital letter within
    the option

If the menu does not fit the screen

o   <Next Scrn> shows the next command page

o   <Prev Scrn> shows the previous command page

o   <PF1> moves to the first option

o   <PF4> moves to the last option

To repaint the screen after a message from the superuser

o   hit "Control" and "R" at the same time
.12
The application is informing you of something, and depending on the 
message on the status line, expects you to:

o    hit any key, or
o    hit "Y" (to give an affermative answer) or "N" (to give a 
     negative one), or
o    hit "Y", "N", or <Interrupt> should you wish to abort whatever
     operation is being carried out
.21
                         Code Help

This windows shows a complete list of the codes available for the field
about which you have asked for help, together with their meaning.
Choose the relevant code by moving over it with the up or down arrows,
<Next Scrn>, <Prev Scrn> or <RET>, and hit <ESC>.
If you need no help, just hit <Interrupt>.
.22
                         Code Help

This windows shows a complete list of the codes available for the field
about which you have asked for help, together with their meaning.
Choose one or more code by moving over each with the up or down arrows,
<Next Scrn>, <Prev Scrn>, and hit <Space> or <RET> to mark it (another
<Space> or <RET> if you make up your mind).
Confirm your choices by hitting <ESC>.
If you need no help, just hit <Interrupt>.
.23
                         Code Help

This windows shows a complete list of the codes available for the field
about which you have asked for help.
Choose the relevant code by moving over it with the up or down arrows,
<Next Scrn>, <Prev Scrn> or <RET>, and hit <ESC>.
If you need no help, just hit <Interrupt>.
.24
                         Code Help

This windows shows a complete list of the codes available for the field
about which you have asked for help.
Choose one or more code by moving over each with the up or down arrows,
<Next Scrn>, <Prev Scrn>, and hit <Space> or <RET> to mark it (another
<Space> or <RET> if you make up your mind).
Confirm your choices by hitting <ESC>.
If you need no help, just hit <Interrupt>.
