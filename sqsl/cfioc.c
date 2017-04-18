/*
	CFIOC.c  -  File IO routines

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Oct 92
	Current release: Jan 17

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

#include "coslc.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "ccmpc.h"
#include "ccmnc.h"
#include "cfioc.h"
#include "crqxp.h"
#include "ctypc.h"

static FGW_LISTSERIAL(fd_list, EBADF);

/*
** finds an fd
*/
fgw_fdesc *fgw_findfd(int fd, int s)
{
    fgw_listentry e;
    fgw_fdesc *desc;

    e.int_id=fd;
    e.signature=s;
    desc=(fgw_fdesc *) fgw_listsearch(&e, &fd_list);
    errno=status;
    return desc;
}

/*
**  create new fd descriptor
*/
static fgw_fdesc *fgw_newdesc()
{
    fgw_fdesc *fd_n;

    if ((fd_n=(struct fgw_iodesc *) malloc(sizeof(struct fgw_iodesc)))!=NULL);
    {
	memset(fd_n, 0, sizeof(struct fgw_iodesc));
	fd_n->fd=-1;		/* avoid erroneously closing fd 0! */
	fd_n->fd_eolstatus=1;
	fd_n->fd_linelen=78;
	fd_n->fd_bufinc=FGW_BUFSIZE;
	fd_n->fd_bufsize=fd_n->fd_bufinc;
	if (!(fd_n->fd_buf=(char *) malloc(fd_n->fd_bufinc+1)))
	{
	    free(fd_n);
	    return NULL;
	}
	*(fd_n->fd_buf)='\0';
	fgw_newentry((fgw_listentry *) fd_n, &fd_list);	/* NOTE: fd_entry position */
    }
    return fd_n;
}

/*
**  drop a desc
*/
static void fgw_dropfdesc(fgw_fdesc *fd_p)
{
    if (fd_p->fd_buf)
	free(fd_p->fd_buf);
    fgw_dropentry((fgw_listentry *) fd_p, &fd_list);
}

/*
**  add a desc to the fd list
*/
int fgw_addfd(int fd, ssize_t (*r)(), ssize_t (*w)(), int (*c)())
{
    fgw_fdesc *fd_p;

    if ((fd_p=fgw_newdesc())!=NULL)
    {
	fd_p->fd=fd;
	fd_p->fd_read=r;
	fd_p->fd_write=w;
	fd_p->fd_close=c;
	return fd_p->fd_entry.int_id;
    }
    else
    {
	errno=ENFILE;
	return -1;
    }
}

/*
**  opens a file (internal)
*/
fgw_fdesc *fgw_fileopen(char *f, char *m)
{
    fgw_fdesc *fd_p;
    int flags;
    int mode;

    errno=0;
    switch (m[0])
    {
      case 'r':
	mode=O_RDONLY;
	flags=0;
	break;
      case 'w':
	mode=O_WRONLY;
	flags=O_CREAT|O_TRUNC;
	break;
      case 'a':
	mode=O_WRONLY;
	flags=O_CREAT|O_APPEND;
	break;
      default:
	errno=EINVAL;
	return NULL;
    }
    switch (m[1])
    {
      case '+':
	mode=O_RDWR;
	break;
      case ' ':
      case 0:
	break;
      default:
	errno=EINVAL;
	return NULL;
    }
    if ((fd_p=fgw_newdesc())==NULL)
    {
	errno=ENFILE;
	return NULL;
    }
    if ((fd_p->fd=open(f, mode|flags, 0600))==-1)
    {
	fgw_dropfdesc(fd_p);
	return NULL;
    }
    else
    {
	fd_p->fd_flags|=FD_ISFILE;
	fd_p->fd_read=read;
	fd_p->fd_write=write;
	fd_p->fd_close=close;
	return fd_p;
    }
}

/*
**  opens a pipe & spawns a child (internal)
*/
fgw_fdesc *fgw_pipeopen(char *c, char *m)
{
    fgw_fdesc *fd_p;
    int p_pipe, c_pipe;

    errno=0;
    switch (m[0])
    {
      case 'r':
	p_pipe=0;
	c_pipe=1;
	break;
      case 'w':
	p_pipe=1;
	c_pipe=0;
	break;
      default:
	errno=EINVAL;
	return NULL;
    }
    if ((fd_p=fgw_newdesc())==NULL)
    {
	errno=ENFILE;
	return NULL;
    }
    fd_p->fd=-1;	/* we aren't done yet */
    if ((fd_p->fd=fgw_popen(c, c_pipe, p_pipe, fgw_childzap, &fd_p->fd_pid))<0)
    {
	fgw_dropfdesc(fd_p);
	return NULL;
    }
    else
    {
	fd_p->fd_read=read;
	fd_p->fd_write=write;
	fd_p->fd_close=close;
	return fd_p;
    }
}

/*
** close functions for fds, connections, statements
*/
void fgw_childzap()
{
    fgw_zapfd(0);
    rqx_zapstmt(0);
}

/*
** close all open file descriptors
*/
void fgw_zapfd(int s)
{
    fgw_fdesc *fd_p, *fd_f;

/*
**  Here, we should have also closed all the file streams currently open.
*/
    fd_p=(fgw_fdesc *) fgw_listnext(&fd_list, NULL);
    while (fd_p!=NULL)
    {
	fd_f=fd_p;
	fd_p=(fgw_fdesc *) fgw_listnext((fgw_listtype *) &fd_list,
					(fgw_listentry *) fd_p);
	if (s==fd_f->fd_entry.signature || !s)
	{
/*
** avoid closing standard input, output, error and incomplete fd's
*/
	    if (fd_f->fd>2)
		(*fd_f->fd_close) (fd_f->fd);
	    fgw_dropfdesc(fd_f);
	}
    }
}

/*
**  closes a stream
*/
int fgw_fdclose(fgw_fdesc *fd_p)
{
    int r=0;
    int rc;

    if ((*fd_p->fd_close) (fd_p->fd)==-1)
	status=errno;
    if (fd_p->fd_pid)
    {
	r=fgw_waitpid(fd_p->fd_pid, &rc);
	if (r==-1)
	{
	    status=errno;
	    r=-1;
	}
	else
	    r=(rc>>8);
    }
    fgw_dropfdesc(fd_p);
    return(r);
}

/*
** generic read
*/
int fgw_fdread(fgw_fdesc *fd_p)
{
    char *p;
    int r;

/*
** reallocate buffer if needs be, and if we can
*/
    if (fd_p->fd_bufsize-fd_p->fd_buflen<(fd_p->fd_linelen?
		fd_p->fd_linelen: fd_p->fd_bufinc))
    {
	if (p=(char *) realloc(fd_p->fd_buf,
				fd_p->fd_bufsize+fd_p->fd_bufinc+1))
	{
	    fd_p->fd_bufsize+=fd_p->fd_bufinc;
	    fd_p->fd_buf=p;
	}
	else
	{
	    errno=ENOMEM;
	    return -1;
	}
    }
/*
** read how much we can
*/
    if ((r=(*fd_p->fd_read) (fd_p->fd, fd_p->fd_buf+fd_p->fd_buflen,
			    fd_p->fd_bufsize-fd_p->fd_buflen))>0)
    {
	fd_p->fd_buflen+=r;
	fd_p->fd_offset+=r;
	*(fd_p->fd_buf+fd_p->fd_buflen)='\0';
    }
    return r;
}

/*
** Read a blob from disk
*/
void fgw_blobread(int fd_id, fgw_loc_t *textvar)
{
    fgw_fdesc *fd_p;
    char *newbuf;
    int r, bufsize;
    struct stat fileinfo;

    errno=0;
    if ((fd_p=fgw_findfd(fd_id, 0))==NULL)
	return;

/*
** at this point we allow to read the reminder of the file
** into the blob
** 
    if (fd_p->fd_buflen || fd_p->fd_offset)
    {
	errno=EINVAL;
	return;
    }
*/
/*
**  try to avoid memory leaks & segv's :-)
*/
    if (textvar->loc_loctype==LOCMEMORY &&
	textvar->loc_bufsize>0 &&
	textvar->loc_buffer)
	free(textvar->loc_buffer);
/*
** WARNING we do not deal with loc_user_env. Mem leaks will result from
** text vars used with the text manipulation package, and not properly
** freed prior to calling fd_blobread
*/
    textvar->loc_buffer=NULL;
    textvar->loc_bufsize=0;
    textvar->loc_size=0;
    textvar->loc_indicator=-1;
    textvar->loc_loctype=LOCMEMORY;
/*
** work out how much we have to read - might not be possible if it is a pipe
*/
    if (fstat(fd_p->fd, &fileinfo) || fileinfo.st_size<=0)
    {
	bufsize=16*FGW_BUFSIZE;
	fileinfo.st_size=0;
    }
    else
        bufsize=fileinfo.st_size;
/*
** keep on reading until we hit the end (single read for files)
*/
    for (;;)
    {
	textvar->loc_bufsize+=bufsize;
	if ((newbuf=(char *) realloc(textvar->loc_buffer, textvar->loc_bufsize)))
	{
	    textvar->loc_buffer=newbuf;
	    if ((r=(*fd_p->fd_read) (fd_p->fd,
				     textvar->loc_buffer+textvar->loc_size,
				     bufsize))>0)
	    {
		textvar->loc_size+=r;
		fd_p->fd_offset+=r;
		textvar->loc_indicator=0;
		if (r==fileinfo.st_size)
		    fd_p->fd_eof=1;
		else
		    continue;
	    }
	    else if (!r || errno==EINVAL)
		fd_p->fd_eof=1;
	}
	else
	{
	    free(textvar->loc_buffer);
	    textvar->loc_buffer=NULL;
	    textvar->loc_bufsize=0;
	    textvar->loc_size=0;
	    errno=ENOMEM;
	}
	return;
    }
}

/*
**  write operation
*/
void fgw_fdwrite(fgw_fdesc *fd_p, char *bf, int l)
{
    int r;

    if (fd_p->fd_buflen && (fd_p->fd_flags & FD_ISFILE))
	if (lseek(fd_p->fd, fd_p->fd_offset-fd_p->fd_buflen, SEEK_SET)==-1)
	    status=errno;
	else
	{
	    fd_p->fd_offset-=fd_p->fd_buflen;
	    fd_p->fd_buflen=0;
	    *(fd_p->fd_buf)=0;
	}
    if ((r=(*fd_p->fd_write) (fd_p->fd,bf,l))>=0)
    {
	if (fd_p->fd_eof)
	    fd_p->fd_eof=0;
	fd_p->fd_offset+=r;
    }
    else
	status=errno;
}

#if defined(I4GLVER) || defined(A4GL)
/*
**  opens a file
*/
int FGLGLOB(file_open)(nargs)
int nargs;
{
    char f[90];
    char m[3];
    fgw_fdesc *fd_p;

    popquote((char *) &m, sizeof(m));
    popquote((char *) &f, sizeof(f));
    fgw_clip(f);
    if (fd_p=fgw_fileopen(f, m))
	retint(fd_p->fd_entry.int_id);
    else
    {
	status=errno;
	retquote("");
    }
    return(1);
}

/*
**  opens a pipe & spawns a child
*/
int FGLGLOB(pipe_open)(nargs)
int nargs;
{
    char *c;
    char m[2];
    fgw_fdesc *fd_p;

    popquote((char *) &m, sizeof(m));
    if (!(c=fgw_popquote()))
    {
	status=FGW_ENOMEM;
	retquote("");
	return(1);
    }
    if (fd_p=fgw_pipeopen(c, m))
	retint(fd_p->fd_entry.int_id);
    else
    {
	status=errno;
	retquote("");
    }
    free(c);
    return(1);
}

/*
**  open stdin, stdout, stderr
*/
int FGLGLOB(stdfd_open)(nargs)
int nargs;
{
    int stdfd;
    int fd_id;

    popint(&stdfd);
    SETNULLINT((char *) &fd_id);
    if (stdfd<0 || stdfd>2)
	status=FGW_EINVAL;
    else if ((fd_id=fgw_addfd(stdfd, read, write, close))==-1)
    {
	status=errno;
	SETNULLINT((char *) &fd_id);
    }
    retint(fd_id);
    return(1);
}

/*
** Lock an opened file
*/
int FGLGLOB(file_lock)(nargs)
int nargs;
{
    fgw_fdesc *fd_p;
    int mandatory;
    int fd_id;

    popint(&mandatory);
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
    {
	if (fd_p->fd_flags & FD_ISFILE)
	    errno=fgw_flock(fd_p->fd, mandatory);
	else
	    status=FGW_EINVAL;
    }
    return(0);
}

/*
** Terminate lock
*/
int FGLGLOB(file_unlock)(nargs)
int nargs;
{
    fgw_fdesc *fd_p;
    int fd_id;

    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
    {
	if (fd_p->fd_flags & FD_ISFILE)
	    status=fgw_funlock(fd_p->fd);
	else
	    status=FGW_EINVAL;
    }
    return(0);
}

/*
** tell current fd position
*/
int FGLGLOB(file_tell)(nargs)
int nargs;
{
    fgw_fdesc *fd_p;
    int fd_id;
    int offset;

    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
	if (fd_p->fd_flags & FD_ISFILE)
	    retint(fd_p->fd_offset-fd_p->fd_buflen);
	else
	    status=ESPIPE;
    if (status)
	retquote("");
    return(1);
}

/*
** position fd current offset
*/
int FGLGLOB(file_seek)(nargs)
int nargs;
{
    fgw_fdesc *fd_p;
    int offset;
    int fd_id;

    popint(&offset);
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
    {
	if (!fd_p->fd_flags & FD_ISFILE)
	    status=ESPIPE;
	else if (lseek(fd_p->fd, offset, SEEK_SET)==-1)
	    status=errno;
	else
	{
	    *(fd_p->fd_buf)=0;
	    fd_p->fd_buflen=0;
	    fd_p->fd_offset=offset;
	}
    }
    return(0);
}

/*
** Read operation
*/
int FGLGLOB(fd_read)(nargs)
int nargs;
{
    char bf[FGW_BUFSIZE+1];
    int fd_id;
    fgw_fdesc *fd_p;
    char *p, *q, *s;
    int	r, ol, ml, cl, tl;

    popint(&fd_id);
    status=0;
/*
** see what fd we're talking about
*/
    if ((fd_p=fgw_findfd(fd_id, 0))==NULL)
    {
	retquote("");
	return(1);
    }
/*
**  the maximum length we can return depends on whether this is the
**  first line after a <NL> (a paragraph header) or not
*/
    if (fd_p->fd_eolstatus)
	tl=strlen(fd_p->fd_lninit1);
    else
	tl=strlen(fd_p->fd_lninit2);
    ml=fd_p->fd_linelen-tl;
/*
**  try to mimic fgets functionality
*/
    for (;;)
    {
	if (fd_p->fd_buflen)
	{
/*
**  identify [<CR>]<NL> position, if any
*/
	    cl=1;
	    r=0;
	    p=strchr(fd_p->fd_buf, '\n');
	    if (p!=NULL)
	    {
		if (p>fd_p->fd_buf)
		{
		    q=p;
		    if (*--q=='\r')
		    {
			p=q;
			cl++;
		    }
		}
		r=p-fd_p->fd_buf;
	    }
/*
** line break within client buffer limits
*/
	    if (p!=NULL && r<=ml)
	    {
/*
**  put the correct line header, and then the buffer contents
*/
		if (fd_p->fd_eolstatus)
		    strcpy(bf, fd_p->fd_lninit1);
		else
		    strcpy(bf, fd_p->fd_lninit2);
		strncat(bf, fd_p->fd_buf, r);
		bf[tl+r]='\0';
		fgw_clip(bf);
/*
**  remove line from buffer
*/
		p=p+cl;
		fd_p->fd_buflen=fd_p->fd_buflen-r-cl;
		fgw_move(fd_p->fd_buf, p, fd_p->fd_buflen+1);
		fd_p->fd_eolstatus=1;
		retquote(bf);
		return(1);
	    }
/*
** buffer holds more than client is willing to accept, and no [<CR>]<LF>
*/
	    else if (r>ml || fd_p->fd_buflen>=ml)
	    {
/*
** this is what we're going to return if no word splitting requested, or no
** valid word separator found
*/
		s=fd_p->fd_buf+ml;
		fd_p->fd_buflen=fd_p->fd_buflen-ml;
		r=ml;
/*
** search for a word separator
*/
		if (strlen(fd_p->fd_wdsplit))
		    for (p=fd_p->fd_buf+ml-1; p>=fd_p->fd_buf; p--)
			for (q=fd_p->fd_wdsplit; *q; q++)
			    if (*p==*q)
			    {
				if (q-fd_p->fd_wdsplit>=fd_p->fd_eolidx)
				    p++;
				s=p;
				r=p-fd_p->fd_buf;
				fd_p->fd_buflen=fd_p->fd_buflen+ml-r;
				goto ws_label;
			    }
ws_label:
/*
**  put the correct line header, and then the buffer contents
*/
		if (fd_p->fd_eolstatus)
		    strcpy(bf, fd_p->fd_lninit1);
		else
		    strcpy(bf, fd_p->fd_lninit2);
		strncat(bf, fd_p->fd_buf, r);
		bf[tl+r]=0;
		fgw_clip(bf);
/*
**  remove line from buffer
*/
		fgw_move(fd_p->fd_buf, s, fd_p->fd_buflen+1);
		fd_p->fd_eolstatus=0;
		retquote(bf);
		return(1);
	    }
/*
**  last string in buffer and (hugh!) no [<CR>]<NL>
*/
	    else if (fd_p->fd_eof)
	    {
		if (fd_p->fd_eolstatus)
		    strcpy(bf, fd_p->fd_lninit1);
		else
		    strcpy(bf, fd_p->fd_lninit2);
		strcat(bf, fd_p->fd_buf);
		fgw_clip(bf);
		*(fd_p->fd_buf)=0;
		fd_p->fd_buflen=0;
		retquote(bf);
		return(1);
	    }
	}
/*
**  nothing in buffer and eof
*/
	else if (fd_p->fd_eof)
	{
	    retquote("");
	    return(1);
	}
/*
** not there yet, just keep on reading
*/
	ol=fd_p->fd_buflen;
	if ((r=fgw_fdread(fd_p))>0)
	{
/*
**	change tabs into spaces or the screen screws up
*/
	    p=strchr(fd_p->fd_buf+ol, '\t');
	    while (p!=NULL)
	    {
		*p=' ';
		p=strchr(p, '\t');
	    }
	}
	else if (!r || errno==EINVAL)
	    fd_p->fd_eof=1;
	else
	{
	    retquote("");
	    status=errno;
	    return(1);
	}
    }
}

/*
** Read a blob from disk;
*/
int FGLGLOB(fd_blobread)(nargs)
int nargs;
{
    LOC_T(textvar)
    int fd_id;

    POPLOCATOR(textvar)
    popint(&fd_id);
    fgw_blobread(fd_id, textvar);
    if (!status)
	status=errno;
    RESTORELOCATOR(textvar)
    return 0;
}

/*
** Write operation
*/
int FGLGLOB(fd_write)(nargs)
int nargs;
{
    char *bf;
    int fd_id;
    fgw_fdesc *fd_p;

    if (!(bf=fgw_popquote()))
    {
	status=FGW_ENOMEM;
	return(0);
    }
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
    {
	fgw_clip(bf);
	fgw_fdwrite(fd_p, bf, strlen(bf));
    }
    free(bf);
    return(0);
}

/*
** Write operation, with [<CR>]<LN> postpended
*/
int FGLGLOB(fd_writeln)(nargs)
int nargs;
{
    char *bf;
    int fd_id;
    fgw_fdesc *fd_p;

    if (!(bf=fgw_popquote()))
    {
	status=FGW_ENOMEM;
	return(0);
    }
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
    {
	fgw_clip(bf);
	if (fd_p->fd_flags & FD_CR)
	   strcat(bf, "\r\n");
	else
	   strcat(bf, "\n");
	fgw_fdwrite(fd_p, bf, strlen(bf));
    }
    free(bf);
    return(0);
}

/*
** Dump a blob to disk;
*/
int FGLGLOB(fd_blobwrite)(nargs)
int nargs;
{
    LOC_T(textvar)
    int fd_id;
    fgw_fdesc *fd_p;

    POPLOCATOR(textvar)
    popint(&fd_id);
    if (textvar->loc_loctype!=LOCMEMORY)
    {
	status=FGW_EINVAL;
	return(0);
    }
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL &&
	textvar->loc_bufsize && !textvar->loc_indicator)
	fgw_fdwrite(fd_p, textvar->loc_buffer, textvar->loc_bufsize);
    return(0);
}

/*
** set output buffer size
*/
int FGLGLOB(fd_linelen)(nargs)
int nargs;
{
    int fd_id;
    int ln;
    fgw_fdesc *fd_p;

    popint(&ln);
    popint(&fd_id);
    if ((fd_p=fgw_findfd(fd_id, 0))==NULL)
	return(0);
    status=0;
    if (ln>0 && ln<fd_p->fd_bufsize)
	fd_p->fd_linelen=ln;
    else
	status=FGW_EINVAL;
    return(0);
}

/*
** set eol marker
*/
int FGLGLOB(fd_crnl)(nargs)
int nargs;
{
    int fd_id;
    int crnl;
    fgw_fdesc *fd_p;

    popint(&crnl);
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
	if (crnl)
	    fd_p->fd_flags|=FD_CR;
	else
	    fd_p->fd_flags&=~FD_CR;
    return(0);
}

/*
** set word prepend separator
*/
int FGLGLOB(fd_prewordsep)(nargs)
int nargs;
{
    int fd_id;
    char sep[5], post[5];
    fgw_fdesc *fd_p;

    popquote((char *) &sep, sizeof(sep));
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))==NULL)
	return(0);
    strcpy(post, fd_p->fd_wdsplit+fd_p->fd_eolidx);
    strcpy(fd_p->fd_wdsplit, sep);
    strcat(fd_p->fd_wdsplit, post);
    fd_p->fd_eolidx=strlen(sep);    
    return(0);
}

/*
** set word postpend separator
*/
int FGLGLOB(fd_postwordsep)(nargs)
int nargs;
{
    int fd_id;
    char sep[5];
    fgw_fdesc *fd_p;

    popquote((char *) &sep, sizeof(sep));
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
	strcpy(fd_p->fd_wdsplit+fd_p->fd_eolidx, sep);
    return(0);
}

/*
** set paragraph line prepend text
*/
int FGLGLOB(fd_1stlineheader)(nargs)
int nargs;
{
    int fd_id;
    char hea[5];
    fgw_fdesc *fd_p;

    popquote((char *) &hea, sizeof(hea));
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
	strcpy(fd_p->fd_lninit1, hea);
    return(0);
}

/*
**  set continuation line prepend text
*/
int FGLGLOB(fd_2ndlineheader)(nargs)
int nargs;
{
    int fd_id;
    char hea[5];
    fgw_fdesc *fd_p;

    popquote((char *) &hea, sizeof(hea));
    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
	strcpy(fd_p->fd_lninit2, hea);
    return(0);
}

/*
** Flag to discard buffer contents up to the next eol
*/
int FGLGLOB(fd_droptoeol)(nargs)
int nargs;
{
    char bf[FGW_BUFSIZE];
    int fd_id;
    fgw_fdesc *fd_p;
    char *p;
    int	r;

    popint(&fd_id);
    status=0;
/*
** see what fd we're talking about
*/
    if ((fd_p=fgw_findfd(fd_id, 0))==NULL)
	return(0);
    else if (fd_p->fd_eolstatus)
	return(0);
    for (;;)
    {
	if (fd_p->fd_buflen)
	{
/*
**  identify [<CR>]<NL> position, if any
*/
	    if ((p=strchr(fd_p->fd_buf, '\n'))!=NULL)
	    {
		p++;
		fd_p->fd_buflen=fd_p->fd_buflen-(p-fd_p->fd_buf);
		fgw_move(fd_p->fd_buf, p, fd_p->fd_buflen+1);
		fd_p->fd_eolstatus=1;
		return(0);
	    }
	    else
	    {
		fd_p->fd_buflen=0;
		*(fd_p->fd_buf)=0;
	    }
/*
** not there yet, just keep on reading
*/
	}
	else if (fd_p->fd_eof)
	    return(0);
	if ((r=(*fd_p->fd_read) (fd_p->fd, bf, sizeof(bf)-fd_p->fd_buflen-1))>0)
	{
	    bf[r]=0;
/*
**	change tabs into spaces or the screen screws up
*/
	    p=strchr(bf, '\t');
	    while (p!=NULL)
	    {
		*p=' ';
		p=strchr(p, '\t');
	    }
	    strcat(fd_p->fd_buf, bf);
	    fd_p->fd_buflen+=r;
	    fd_p->fd_offset+=r;
	}
	else if (!r || errno==EINVAL)
	    fd_p->fd_eof=1;
	else
	{
	    status=errno;
	    return(0);
	}
    }
}

/*
**  return eol status
*/
int FGLGLOB(fd_eol)(nargs)
int nargs;
{
    int fd_id;
    fgw_fdesc *fd_p;

    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
	retint(fd_p->fd_eolstatus);
    else
	retquote("");
    return(1);
}

/*
**  return eof status
*/
int FGLGLOB(fd_eof)(nargs)
int nargs;
{
    int fd_id;
    fgw_fdesc *fd_p;

    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))!=NULL)
	retint(fd_p->fd_eof && !fd_p->fd_buflen);
    else
	retquote("");
    return(1);
}

/*
**  Done with IO, close fd & free mem
*/
int FGLGLOB(fd_close)(nargs)
int nargs;
{
    int fd_id, r;
    fgw_fdesc *fd_p;

    popint(&fd_id);
    status=0;
    if ((fd_p=fgw_findfd(fd_id, 0))==NULL)
    {
	retquote("");
	return(1);
    }
    r=fgw_fdclose(fd_p);
    if (errno)
	rsetnull(CINTTYPE, &r);
    retint(r);
    return(1);
}
#endif
