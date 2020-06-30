/*
	CSQRC.h  -  SQSL return codes

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Mar 03
	Current release: Jun 17

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

#ifndef CSQRC_H

/*
** user actions and informational messages
*/
#define RC_NOTFOUND	       100	/* SQLNOTFOUND, etc */
#define RC_NROWS	0xfe000065	/* SQL succesfull, %i rows procesed */
#define RC_UINTR	0xfe000066	/* Script interrupted by user */
#define RC_EXIT		0xfe000067	/* explicit EXIT statement */
#define RC_QUIT		0xfe000068	/* explicit QUIT statement */
#define RC_CONTINUE     0xfe000069	/* AGGREGATE clause skips row */
#define RC_PAUSE	0xfe00006a	/* internally used to pause pcode */

#define RC_ISFLOW(s) ((s)>=RC_NROWS && (s)<=RC_PAUSE)

/*
** scanner errors
*/
#define RC_BADQT	0xfd000096	/* unbalanced quote */
#define RC_BADCMT	0xfd000097	/* unbalanced comment */
#define RC_EXPINJ	0xfd000098	/* expansion crosses statements */

#define RC_ISSCAN(s) ((s)>=RC_BADQT && (s)<=RC_EXPINJ)

/*
** internal errors
*/
#define RC_NOMEM	0xfe0000c8	/* Out of memory */
#define RC_INSID	0xfe0000c9	/* Invalid statement identifier */
#define RC_NDECL	0xfe0000ca	/* Not a cursory statement */
#define RC_NEXEC	0xfe0000cb	/* Not an executable statement */
#define RC_CRNGE	0xfe0000cc	/* Column out of range */
#define RC_NOVAR	0xfe0000cd	/* Invalid variable handle */

/*
** expansion facility syntax errors
*/
#define RC_EXSTX	0xfe00012c	/* Syntax error in expansion facility */
#define RC_NWNDW	0xfe00012d	/* Not in form mode, can't open window */
#define RC_XNBLC	0xfe00012e	/* Unbalanced comment or expansion */
#define RC_XEVAL	0xfe00012f	/* Invalid expression */
#define RC_NACTN	0xfe000130	/* No expansion action specified */
#define RC_NDATA	0xfe000131	/* Expansion returned no data */

#define RC_ISEXP(s) ((s)>=RC_EXSTX && (s)<=RC_NDATA)

/*
** parser syntax errors and runtime errors
*/
#define RC_PSNTX	0xfd000190	/* syntax error */
#define RC_IDTIV	0xfe000191	/* incompatible datetime or interval */
#define RC_IQUAL	0xfe000192	/* invalid qualifier specified */
#define RC_NOFMT	0xfe00019a	/* cannot specify output format */
#define RC_BADDE	0xfe00019b	/* empty delimiter or escape */
#define RC_BADPA	0xfe00019c	/* invalid pattern syntax */
#define RC_BADAC	0xfe00019d	/* autocommits are +ve ints */
#define RC_BADBS	0xfe00019e	/* block sizes are +ve ints */
#define RC_NOBIN	0xfe00019f	/* binary format disallowed here */
#define RC_NOMTC	0xfe0001a0	/* pattern not matched */
#define RC_ERANG	0xfe0001a1	/* value out of range */
#define RC_DZERO	0xfe0001a5	/* division by 0 */
#define RC_ISUBS	0xfe0001a6	/* invalid subscript */
#define RC_INHST	0xfe0001a7	/* hash used in scalar context */
#define RC_INSCL	0xfe0001a8	/* scalar used in hash context */
#define RC_INHSH	0xfe0001a9	/* hash contains hashes */
#define RC_WRASG	0xfe0001aa	/* mismatching assignment */
#define RC_WRCNO	0xfe0001ab	/* non existent column, can't assign */
#define RC_UBBLK	0xfe0001ae	/* unbalanced statement block */

/*
** function errors
*/
#define RC_BFINV	0xfe0001f4	/* invalid parm passed to builtin func */
#define RC_WACNT	0xfe0001f5	/* invalid no of args to builtin func */
#define RC_CNUHR	0xfe0001f6	/* func cannot be used in this context */
#define RC_IDBIF	0xfe0001f7	/* invalid parameter to dbinfo */
#define RC_WRCNT	0xfe0001f8	/* # of parms returned != expected */
#define RC_NOFRK	0xfe0001f9	/* you are not allowed to fork */
#define RC_RESYN	0xfe0001fa	/* invalid regular expression */
#define RC_FNIPS	0xfe0001fb	/* source cannot support this function in prepared statements */

/*
** dynamic sql errors
*/
#define RC_INCON	0xfe000258	/* no such connection */
#define RC_DUPCN	0xfe000259	/* duplicate connection name */
#define RC_INSRC	0xfe00025a	/* no such source */
#define RC_INOBJ	0xfe00025b	/* no such object */
#define RC_INCUR	0xfe00025c	/* invalid cursor name */
#define RC_USCUR	0xfe00025d	/* cursor in use */
#define RC_NOCUR	0xfe00025e	/* out of cursors */
#define RC_NISTR	0xfe000262	/* not an input stream */
#define RC_NOSTR	0xfe000263	/* not an output stream */
#define RC_BTCNT	0xfe000264	/* mismatching token count */

#define RC_USSTM	0xfe00026c	/* statement can't be freed */
#define RC_SPDST	0xfe00026d	/* statement prepared with INTO clause */
#define RC_SPPHD	0xfe00026e	/* statement prepared with USING clause */
#define RC_SPFMT	0xfe00026f	/* statement prepared with FORMAT */
#define RC_SPAGG	0xfe000270	/* statement prepared with AGGREGATE */
#define RC_NRJCT	0xfe000276	/* statement has no reject file */
/*
** miscellaneous
*/
#define RC_NLNKD	0xfe0003e5	/* Feature not linked */
#define RC_TMDLL	0xfe0003e6	/* no more space for DLLs */
#define RC_NIMPL	0xfe0003e7	/* not implemented */

#define RC_ERRSKIP(x) ((x) != RC_PSNTX && (x) != RC_NOTFOUND && \
			!RC_ISEXP(x) && !RC_ISSCAN(x) && !RC_ISFLOW(x))

#define CSQRC_H
#endif
