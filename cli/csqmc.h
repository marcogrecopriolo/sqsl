/*
	CSQMC.h  -  sqsl interpreter messages, eng

	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Mar 03
	Current release: Jul 16

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
/* 100 is reserved for SQLNOTFOUND */
100, "No more rows or no rows found", NULL, 
101, "Row processed: %i", NULL, 
102, "Statement interrupted by user", NULL, 
/* 103 is reserved to signal an EXIT clause */
150, "Unbalanced quote", NULL, 
151, "Unbalanced comment", NULL, 
151, "Possible statement injection from expansion", NULL,
200, "Out of memory", NULL, 
201, "Invalid statement identifier", NULL, 
202, "Statement has no associated cursor", NULL, 
203, "Statement is not executable", NULL, 
204, "Not enough columns selected, or column id out of range", NULL, 
205, "Invalid variable handle", NULL, 
300, "Syntax error in expansion facility", NULL, 
301, "Cannot open window while not in form mode", NULL, 
302, "Unbalanced expansion or comment", NULL, 
303, "Syntax error while evaluation expansion expression", NULL, 
304, "No expansion action specified", NULL, 
305, "Expansion returned no data", NULL, 
400, "Syntax error", NULL, 
401, "Datetime or interval incompatible with operation", NULL, 
402, "Invalid datetime or interval qualifier", NULL, 
410, "Cannot specify output format in this context", NULL, 
411, "Null or empty delimiter or escape", NULL, 
412, "Invalid pattern specifier", NULL, 
413, "Autocommit not a positive integer", NULL, 
414, "Block size not a positive integer", NULL, 
415, "Binary format disallowed on this fd", NULL, 
416, "Input string does not match pattern", NULL, 
417, "Value out of range", NULL, 
421, "Division by zero", NULL, 
422, "Invalid subscript", NULL, 
423, "Hash used in scalar context", NULL, 
424, "Scalar used in hash context", NULL, 
425, "Hash cannot be expanded as it contains hashes", NULL,
426, "Mismatching assign and target lists", NULL,
427, "Invalid column number", NULL,
430, "Unterminated statement block", NULL,
500, "Invalid argument passed to builtin function", NULL, 
501, "Invalid number of arguments passed to builtin function", NULL, 
502, "Function cannot be used in this context", NULL, 
503, "Invalid argument passed to dbinfo", NULL, 
504, "Function returned unexpected number of values", NULL, 
505, "You are not allowed to use fork", NULL,
506, "Invalid regular expression", NULL,
600, "Invalid connection identifier", NULL, 
601, "Duplicate connection identifier", NULL, 
602, "No such shared library, or not a valid source", NULL, 
603, "No such object in shared library", NULL, 
604, "Invalid cursor name", NULL, 
605, "Cursor already in use", NULL, 
606, "No more cursors", NULL, 
610, "Not a valid input stream", NULL, 
611, "Not a valid output stream", NULL, 
612, "Mismatching input and output field count", NULL, 
620, "Statement in use", NULL, 
621, "Statement prepared with destination variables", NULL, 
622, "Statement prepared with placeholder variables", NULL, 
623, "Statement prepared with FORMAT clause", NULL, 
624, "Statement prepared with AGGREGATE clause", NULL, 
630, "Statement does not specify a reject file", NULL, 
997, "Feature not linked in", NULL,
998, "DLL storage full", NULL,
999, "Not implemented", NULL
