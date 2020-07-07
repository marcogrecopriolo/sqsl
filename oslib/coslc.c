/*
	COSLC.c  -  SQSL OS dependent calls

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Jul 09
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

#include "cosdc.h"
#include "csqrc.h"
#include <fcntl.h>
#if (! defined(NT)) || defined(CYGSTATIC) || defined(CYGDYNAMIC)
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#else
#include <process.h>
#endif
#if defined(NT) && !defined(CYGSTATIC)
#include <windows.h>
#include <errno.h>
#include <stdlib.h>
#endif

/*
** returns user name
*/
DLLDECL void fgw_username(char *s, int l)
{
#if (! defined(NT)) || defined(CYGSTATIC)
    struct passwd *p;

    if (p=getpwuid(getuid()))
	strncpy(s, p->pw_name, l);
#else
    DWORD l1=l;

    /* IGNORE */ GetUserName(s, (DWORD *) &l1);
#endif
}

#if (! defined(NT)) || defined(CYGSTATIC) || defined(CYGDYNAMIC)
/*
** open pipe and execute process
*/
DLLDECL int fgw_popen(char *cmd, int pin, int pout, void (*init)(), int *pid)
{
    int fds[2];

    if (pipe((int *) &fds)<0)
	return -1;
/*
** we aim to live even if the pipe is closed
*/
    signal(SIGPIPE, SIG_IGN);
    if ((*pid=fork())==0)
    {
/*
** child branch
*/
	if (init)
	    (*init)();
	close(fds[pout]);
	if (fds[pin]!=pin)
	{
	    dup2(fds[pin], pin);
	    close(fds[pin]);
	}
	execl("/bin/sh", "sh", "-c", cmd, NULL);
/*
** just in case anything goes wrong
*/
	_exit(127);
    }
    else
    {
	int save_errno=errno;

	close(fds[pin]);
	if (*pid<0)
	{

	    close(fds[pout]);
	    errno=save_errno;
	    fds[pout]=-1;
	}
    }
    return fds[pout];
}

/*
** execute process
*/
DLLDECL int fgw_spawn(char *cmd, void (*init)())
{
    int pid;

    if ((pid=fork())==0)
    {
/*
** child branch
*/
	if (init)
	    (*init)();
	execl("/bin/sh", "sh", "-c", cmd, NULL);
/*
** just in case anything goes wrong
*/
	_exit(127);
    }
    return pid;
}

/*
** wait for child process to complete
*/
DLLDECL int fgw_waitpid(int pid, int *rc)
{
    int r;

    do
    {
	r=waitpid(pid, rc, WUNTRACED);
    } while (r==-1 && errno==EINTR);
    return r;
}

/*
** locks a file
*/
DLLDECL int fgw_flock(int fd, int m)
{
    struct flock lck;

    if (m)
	lck.l_type=F_WRLCK;
    else
	lck.l_type=F_RDLCK;
    lck.l_whence=0;
    lck.l_start=0;
    lck.l_len=0;
    if (fcntl(fd, F_SETLK, &lck)==-1)
	return errno;
    else
	return 0;
}

/*
** unlocks a file
*/
DLLDECL int fgw_funlock(int fd)
{
    struct flock lck;

    lck.l_type=F_UNLCK;
    lck.l_whence=0;
    lck.l_start=0;
    lck.l_len=0;
    if (fcntl(fd, F_SETLK, &lck)==-1)
	return errno;
    else
	return 0;
}

static char errstring[20];
/*
** return dynamic loader error string
*/
DLLDECL char *fgw_dlerror()
{
#ifdef HAVE_DLERRNO
    sprintf((char*) &errstring, "%d", dlerrno());
    return (char *) &errstring;
#else
/*
** this only works in AIX afaik
*/
    if (errno)
    {
	sprintf((char*) &errstring, "%d", errno);
	return (char *) &errstring;
    }
    else
	return dlerror();
#endif
}
#else
#define MAXCHILDREN 256
static HANDLE children[MAXCHILDREN];
static int childpid[MAXCHILDREN];
static int childcount=0;

/*
** searches and remove child from children array
*/
static void removechild(HANDLE c)
{
    int i, j;

    for (i=0; i<childcount; i++)
	if (children[i]==c)
	{
	    for (j=i+1; j<childcount; i++, j++)
	    {
		children[i]=children[j];
		childpid[i]=childpid[j];
	    }
	    childcount--;
	    break;
	}
}

/*
** adds latest child to children array
*/
static void insertchild(HANDLE h, int p)
{
    childpid[childcount]=p;
    children[childcount++]=h;
}

static int fgw_dospawn(char *cmd, PROCESS_INFORMATION *pi)
{
    char *comspec;
    char *cl;
    int err=0;
    STARTUPINFO si;

    if (!(cl=malloc(strlen(cmd)+4)))
        return ENOMEM;
    sprintf(cl, "/C %s", cmd);
    memset(&si, 0, sizeof(si));
    memset(pi, 0, sizeof(pi));
    si.cb=sizeof(si);
    if (!(comspec=getenv("COMSPEC")))
	comspec="c:\\windows\\system32\\cmd.exe";
    if (!CreateProcess(comspec, cl, NULL, NULL, TRUE, 0, NULL, NULL, &si, pi))
	err=GetLastError();
    free(cl);
    return err;
}

/*
** open pipe and execute process
*/
DLLDECL int fgw_popen(char *cmd, int pin, int pout, void (*init)(), int *pid)
{
    int fds[2];
    int orgfd;
    int save_errno;
    PROCESS_INFORMATION pi;

    if (childcount==MAXCHILDREN-1)
    {
	errno=EAGAIN;
	return -1;
    }
    if (_pipe((int *) &fds, 8192, O_NOINHERIT )<0)
	return -1;
/*
** the fork() way is much more elegant, we have to do a bit of three cards
** game ourselves to get the fds across the right way
*/
    orgfd=_dup(pin);
    _dup2(fds[pin], pin);
    close(fds[pin]);
    save_errno=fgw_dospawn(cmd, &pi);
/*
** set everything back as it was
*/
    _dup2(orgfd, pin);
    close(orgfd);
    if (save_errno)
    {
	close(fds[pout]);
	errno=save_errno;
	return -1;
    }
    errno=save_errno;
    *pid=pi.dwProcessId;
    insertchild(pi.hProcess, pi.dwProcessId);
    return fds[pout];
}

/*
** execute process
*/
DLLDECL int fgw_spawn(char *cmd, void (*init)())
{
    PROCESS_INFORMATION pi;

    if (childcount==MAXCHILDREN-1)
    {
	errno=EAGAIN;
	return -1;
    }
    errno=fgw_dospawn(cmd, &pi);
    if (errno)
	return -1;
    insertchild(pi.hProcess, pi.dwProcessId);
    return pi.dwProcessId;
}

/*
** wait for child process to complete
*/
DLLDECL int fgw_waitpid(int pid, int *rc)
{
    DWORD r;
    int i;

    for (i=0; i<childcount; i++)
	if (childpid[i]==pid)
	{
	    if (WaitForSingleObject(children[i], INFINITE)==WAIT_FAILED)
	    {
		errno=ECHILD;
		return -1;
	    }
	    GetExitCodeProcess(children[i], &r);
	    removechild(children[1]);
	    *rc=r;
	    return pid;
	}
    errno=ECHILD;
    return -1;
}

/*
** locks a file
*/
DLLDECL int fgw_flock(int fd, int m)
{
     return RC_NIMPL;
}

/*
** unlocks a file
*/
DLLDECL int fgw_funlock(int fd)
{
     return RC_NIMPL;
}
#endif

#ifdef NT
#ifndef CYGSTATIC

/* the dl class of functions could have been a macro, but down to the
   way winbase.h and windows.h are written, they are best left here */

/*
** dlopen
*/
DLLDECL void *fgw_dlopen(char *f)
{
    return (void *) LoadLibrary(f);
}

/*
** dlsym
*/
DLLDECL void *fgw_dlsym(void *h, char *s)
{
    return (void *) GetProcAddress(h, s);
}

/*
** dlclose
*/
DLLDECL int fgw_dlclose(void *h)
{
    return FreeLibrary(h);
}

static char errbuf[20];
/*
** dlerror
*/
DLLDECL char *fgw_dlerror()
{
    int rc;

    rc=GetLastError();
    sprintf((char *) &errbuf, "%d", rc);
    return (char *) &errbuf;
}

/*
** execl
*/
DLLDECL int fgw_execl(char *c)
{
    char *comspec;

    if (!(comspec=getenv("COMSPEC")))
	comspec="c:\\windows\\system32\\cmd.exe";
    return _execl(comspec, "cmd", "/C", c, NULL);
}
#endif			/* CYGSTATIC */

#ifdef CYGDYNAMIC
/*
** initialize cygwin library
*/
DLLDECL void __wrap_main(int argc; char **argv)
{
    cygwin_dll_init();
    __real_main(argc, argv);
}

/*
** fork a child process
*/
DLLDECL int fgw_realfork()
{
    return (int) fork();
}

/*
** wait for a child process
*/
DLLDECL int fgw_wait(int *rc)
{
    return wait(rc);
}

/*
** native Win32
*/
#elif !defined(CYGSTATIC)
#define FORK_MAGIC "_fgw_fork_"
#define FORK_TIMEOUT 2000		/* if we're not done in 2 seconds,
					   tough! */
#define FORK_EXITCODE 0x7f

typedef struct
{
   char *data;
   int (*entry)();
   HANDLE forker_event;
} forkinfo_t;

static forkinfo_t forkinfo;
static HANDLE forker, forker_event;
static char *argv0;

/*
** fork a child process
*/
DLLDECL int fgw_fork(void *s, int (*e)())
{
/*
** Just to end now the diatribe in between threads and processes:
** some database engines do not accept multiple connections from the same
** process to the same database. Others serialize thread access to the same
** resource, neither of which would allow threads to provide good workload
** generators or parallel loaders.
**
** To top this off, multithreaded implementations of the parser must prevent
** usage of default connections (which would be shared among threads, not a
** great idea), make unnamed connections impossible to use and named
** connections and cursors a nightmare to maintain, and to add insult to injury
** duplicating the parser environment is quite an effort (more costly than
** the fork() call itself, as you can gather from the code in fgw_forkentry()),
** and access to certain classes of internal resources needs now to be mutexed.
**
** On this occasion, a proper fork is a much better way to go, lacking which,
** we'll make do with a simple though inelegant strategy:
**
** at process startup we check if we have been forked, if not, we proceed
** with normal startup, if we have, we copy the initial fork information from
** the parent process (which sits at a known address), gather the parser state
** and startup function, execute this, which in turn copies all the memory
** it needs from the parent process which is in turn waiting for us to complete
** the job.
** Once we're done, we release the parent process and continue doing our own
** stuff
** We don't need to take care of shared libraries (the parser will load them
** as and when it needs them), and as for existing connections and open files,
** well, the parser has no knowledge of them, hence it won't use them: we'll
** live dangerously, but it's unlikely to harm us in any way.
*/
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    char *cmd;
    int w;

    if (childcount==MAXCHILDREN-1)
    {
	errno=EAGAIN;
	return -1;
    }
    if (!(cmd=malloc(5+strlen(argv0)+strlen(FORK_MAGIC)+10)))
    {
	errno=ENOMEM;
	return -1;
    }
    sprintf(cmd, "\"%s\" %s %d", argv0, FORK_MAGIC, GetCurrentProcessId());
    forkinfo.data=s;
    forkinfo.entry=e;
    memset(&sa, 0, sizeof(sa));
    sa.nLength=sizeof(sa);
    sa.bInheritHandle=TRUE;
    if ((forkinfo.forker_event=CreateEvent(&sa, FALSE, FALSE, NULL))==NULL)
    {
	free(cmd);
	errno=GetLastError();
	return -1;
    }
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb=sizeof(si);
    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
	free(cmd);
	errno=GetLastError();
	CloseHandle(forkinfo.forker_event);
	return -1;
    }
    switch (WaitForSingleObject(forkinfo.forker_event, FORK_TIMEOUT))
    {
      case WAIT_OBJECT_0:
	insertchild(pi.hProcess, pi.dwProcessId);
	free(cmd);
	return pi.dwProcessId;
      case WAIT_TIMEOUT:
	errno=EAGAIN;

/* if the child didn't copy stuff successfully, it's not a good idea to let
   it hang around...*/
	TerminateProcess(pi.hProcess, 0);
	break;
      case WAIT_FAILED:
	errno=GetLastError();

/* ditto */
	TerminateProcess(pi.hProcess, 0);
	break;
      case WAIT_ABANDONED:
	errno=EAGAIN;
    }
    CloseHandle(forkinfo.forker_event);
    CloseHandle(pi.hProcess);
    free(cmd);
    return -1;
}

/*
** test for being a forked child before entering main program
*/
DLLDECL void __wrap_main(int argc, char **argv)
{
    argv0=argv[0];
    if (argc!=3 || strcmp(argv[1], FORK_MAGIC))
	__real_main(argc, argv);
    else
    {
	int pid=-1;
	long re=0;

	pid=atol(argv[2]);
	forker=OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

/* it's ok to exit half way - the parent will do the cleanup */
	if (!forker)
	    _exit(FORK_EXITCODE);
	(void) ReadProcessMemory(forker, &forkinfo, &forkinfo,
				 sizeof(forkinfo_t), (long *) &re);
	if (re!=sizeof(forkinfo_t))
	    _exit(FORK_EXITCODE);
	if (!DuplicateHandle(forker, forkinfo.forker_event,
				GetCurrentProcess(),
				&forker_event, 0, FALSE,
				DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS))
	    _exit(FORK_EXITCODE);
	if ((forkinfo.entry)(forkinfo.data))
	    _exit(FORK_EXITCODE);
    }
}

/*
** returns memory block from parent process
*/
DLLDECL char *fgw_forkcopy(char *a, int s)
{
    char *d;
    long re=0;

    if (!a)
	return NULL;
    if (!(d=malloc(s)))
	return NULL;
    (void) ReadProcessMemory(forker, a, d, s, (long *) &re);
    if (re==s)
	return d;
    free(d);
    return NULL;
}

/*
** returns memory block from parent process on preallocated buffer
*/
DLLDECL char *fgw_forkcopy2(char *a, char *t, int s)
{
    long re=0;

    if (!a || !t)
	return NULL;
    (void) ReadProcessMemory(forker, a, t, s, (long *) &re);
    if (re==s)
	return t;
    return NULL;
}

/*
** releases parent
*/
DLLDECL int fgw_forkrelease()
{
    (void) SetEvent(forker_event);
    (void) CloseHandle(forkinfo.forker_event);
    (void) CloseHandle(forker_event);
    (void) CloseHandle(forker);
}

/*
** wait for a child process
*/
DLLDECL int fgw_wait(int *rc)
{
    int i, p;
    DWORD r;

    if ((i=WaitForMultipleObjects(childcount, (HANDLE *) &children, FALSE,
			       INFINITE))==WAIT_FAILED)
    {
	errno=ECHILD;
	return -1;
    }
    p=(int) children[i-WAIT_OBJECT_0];
    GetExitCodeProcess(children[i-WAIT_OBJECT_0], &r);
    *rc=(int) r;
    removechild(children[i-WAIT_OBJECT_0]);
    return p;
}

#endif	/* CYGDYNAMIC / !CYGSTATIC  */
#endif	/* NT */
