/*
	CTYPC.h  -  data types and locators

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
	Current release: Sep 16

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

#ifndef CTYPC_H

/* blobs */
#if defined(I4GLVER) || defined(ESQLVER)
#include <locator.h>
typedef ifx_loc_t fgw_loc_t;

#elif defined(FGWSOURCE)
#define fgw_loc_t void

#else
typedef int	 	fgw_int4;
typedef short	 	fgw_int2;

typedef struct
{
    fgw_int2  loc_loctype;          /* type of locator - see types    */
    fgw_int4  loc_bufsize;          /* buffer size                    */
    char     *loc_buffer;           /* memory buffer to use           */
    fgw_int4  loc_mflags;           /* memory flags                   */
    fgw_int4  loc_indicator;        /* null indicator                 */
    fgw_int4  loc_type;             /* type of blob, see below        */
    fgw_int4  loc_size;             /* num bytes in blob or -1        */
    char     *loc_user_env;         /* for the user's PRIVATE use     */
    fgw_int4  loc_oflags;           /* more flags                     */
} fgw_loc_t;

#define LOCMEMORY   1           /* memory storage */
#define LOCFILE     2           /* file storage   */

#endif /* I4GLVER */

/* types, datetimes, intervals, decimals */
#ifndef FGWSOURCE
#include <sqlhdr.h>
#include <sqltypes.h>
#include <datetime.h>
#include <decimal.h>

typedef dec_t fgw_dec_t;
typedef dtime_t fgw_dtime_t;
typedef intrvl_t fgw_intrvl_t;
#else
/*
** sources can't be expected to include IFMX esql/c stuff, as in general
** two different include paths containing files by the same name (think sqlca.h
** or sqlda.h) lead to conflicts, yet they need to know about these mostly for
** conversion issues. so in case of emergency, we let them know about the
** bare minimum:
*/

/* FIXME: this needs to be in sync with IFMX esql/c header files! */

#define CINTTYPE        102
#define CDOUBLETYPE     105
#define CDECIMALTYPE    107
#define CSTRINGTYPE     109
#define CDATETYPE       110
#define CMONEYTYPE      111
#define CDTIMETYPE      112
#define CINVTYPE        115

#define DECSIZE 16
typedef struct decimal
{
    short dec_exp;		/* base 100 exponent */
    short dec_pos;		/* positive: 1, negative: 0, null: -1 */
    short dec_ndgts;		/* significant digits */
    char dec_dgts[DECSIZE];	/* digits (base 100) */
} fgw_dec_t;

#define PRECTOT(x)      (((x)>>8) & 0xff)
#define PRECDEC(x)      ((x) & 0xff)

typedef struct dtime
{
    short dt_qual;
    fgw_dec_t dt_dec;
} fgw_dtime_t;

typedef struct intrvl
{
    short in_qual;
    fgw_dec_t in_dec;
} fgw_intrvl_t;

#define TU_YEAR 0
#define TU_MONTH 2
#define TU_DAY 4
#define TU_HOUR 6
#define TU_MINUTE 8
#define TU_SECOND 10
#define TU_FRAC 12
#define TU_F1 11
#define TU_F2 12
#define TU_F3 13
#define TU_F4 14
#define TU_F5 15

#define TU_END(qual) (qual & 0xf)
#define TU_START(qual) ((qual>>4) & 0xf)
#define TU_LEN(qual) ((qual>>8) & 0xff)

#define TU_ENCODE(len,s,e) (((len)<<8) | ((s)<<4) | (e))
#define TU_DTENCODE(s,e) TU_ENCODE(((e)-(s)+((s)==TU_YEAR?4:2)), s, e)
#define TU_IENCODE(len,s,e) TU_ENCODE(((e)-(s)+(len)),s,e)

#endif

#define CTYPC_H
#endif
