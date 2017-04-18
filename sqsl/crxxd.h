/*
	CRXXD.h  -  SQSL interpreter dynamic query execution definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Jan 97
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

#ifndef CRXXD_H
#include "ccmnc.h"
#include "cfioc.h"
#include "crxuc.h"
#include "ctypc.h"

typedef struct instruction
{
    int (*func)();
    int src_id;
    exprstack_t constant;
    int flags;
    struct instruction *skip_to;
} instruction_t;

/*
** instruction runtime flags
*/
#define RT_HASHASH      0x0001
#define RT_HASSUB1      0x0002
#define RT_HASSUB2      0x0004
#define RT_SAFECOPY     0x0008
#define RT_RESVAR       0x0010
#define RT_HASHVAR      0x0020
#define RT_DITCHRES     0x0040
#define RT_HASREJECT    0x0080

/* to aid the SIMULATED_FORK implementation and to avoid major script mishaps
   such as runaway forks, system calls are only allowed in specific places */
#define RT_CANFORK      0x0100
#define RT_CANRUNSLEEP  0x0200
#define RT_CANSYSTEM    0x0400

#define RT_CONSTOPT     0x0800

typedef struct pcode
{
    fgw_tsstype *parsehead;	/* per execution temporary storage space */
    fgw_tsstype *pcodehead;	/* expression intermediate code storage */
    instruction_t *entry;	/* initial instruction */
    instruction_t *curbuf;	/* current pcode block */
    int parsecnt;		/* available parse entries */
} pcode_t;

#define CRXXD_H
#endif
