/*
	CFIOC.h  -  File IO routines defines

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Oct 92
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

#ifndef CFIOC_H
#define FGW_BUFSIZE 4096
#include "coslc.h"
#include "ccmnc.h"
#include "ctypc.h"

typedef struct fgw_iodesc
{
    fgw_listentry fd_entry;	/* list management */
    int fd;			/* file descriptor */
    int fd_refcount;		/* used by sql parser */
    pid_t fd_pid;		/* child pid */
    int fd_eolstatus;		/* last read terminated with a NL? */
    int fd_eof;			/* EOF marker */
    off_t fd_offset;		/* offset within file */
    int fd_buflen;		/* buffer contents */
    int fd_bufsize;		/* current buffer size */
    int fd_bufinc;		/* buffer increment */
    char *fd_buf;		/* the actual buffer */

    /* this may yield a warning in legacy systems where read/write retun ints */
    ssize_t (*fd_read)();	/* pointer to read function */
    ssize_t (*fd_write)();	/* pointer to write function */
    int (*fd_close)();		/* pointer to close function */
    int fd_flags;		/* gotta have flags */
    int fd_linelen;		/* input line length */
    int fd_eolidx;		/* index of word separators to be saved at the
				   end of the current line. chars below this
				   are returned at the beginning of the next */
    char fd_wdsplit[9];		/* word separator characters */
    char fd_lninit1[5];		/* 'paragraph' line header */
    char fd_lninit2[5];		/* 'continuation' line header */
} fgw_fdesc;

#define FD_ISFILE	0x1	/* seek, tell, lock ops valid? */
#define FD_CR		0x2	/* send a CR together with the NL? */

extern fgw_fdesc *fgw_findfd(int fd, int s);
extern int fgw_addfd(int fd, ssize_t (*r)(), ssize_t (*w)(), int (*c)());
extern fgw_fdesc *fgw_fileopen(char *f, char *m);
extern fgw_fdesc *fgw_pipeopen(char *c, char *m);
extern void fgw_childzap();
extern void fgw_zapfd(int s);
extern int fgw_fdclose(fgw_fdesc *fd_p);
extern int fgw_fdread(fgw_fdesc *fd_p);
extern void fgw_blobread(int fd_id, fgw_loc_t *textvar);
extern void fgw_fdwrite(fgw_fdesc *fd_p, char *bf, int l);

#define CFIOC_H
#endif
