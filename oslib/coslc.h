/*
	COSLC.h  -  SQSL OS dependent calls definitions

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Jul 09
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

#ifndef COSLC

extern void fgw_username(char *s, int l);
extern int fgw_popen(char *cmd, int pin, int pout, void (*init)(), int *pid);
extern int fgw_spawn(char *cmd, void (*init)());
extern int fgw_waitpid(int pid, int *rc);
extern int fgw_flock(int fd, int m);
extern int fgw_funlock(int fd);
extern char *fgw_dlerror();

#include <signal.h>
#define fgw_sigint(s) signal(SIGINT, s)

#if !defined(NT) || defined(CYGSTATIC)
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TGLOB

#define fgw_fork(d, e) fork()
#define fgw_wait wait
#define fgw_execl(c) execl("/bin/sh", "sh", "-c", c, NULL)
#define fgw_dlopen(f) dlopen(f, RTLD_LAZY) /*NOW) */
#define fgw_dlclose dlclose
#define fgw_dlsym dlsym
#define fgw_sbrk sbrk
#define fgw_rand lrand48
#define fgw_srand srand48
#define fgw_millisleep(ms) IGNORE usleep((ms)*1000)

#else

/* SEEK_... stuff */
#ifdef MVS
#include "stdio.h"
#else
#include "unistd.h"
#include "sys/types.h"
#endif

#if defined(MTFORK) && !defined(CYGDYNAMIC)
#define TGLOB __declspec(thread)
#else
#define TGLOB
#endif

#if defined(CYGDYNAMIC)
#define fgw_fork(d, e) fgw_realfork()
extern int fgw_realfork();
#else
#define SIMULATED_FORK
extern int fgw_fork(void *s, int (*e)());
extern char *fgw_forkcopy(char *a, int s);
extern char *fgw_forkcopy2(char *a, char *t, int s);
extern int fgw_forkrelease();
#endif

extern int fgw_wait(int *rc);
extern int fgw_execl(char *c);

extern void *fgw_dlopen(char *f);
extern void *fgw_dlsym(void *h, char *s);

/* we can afford ditching some blobs consistency checking on non 4gl ports */
#define fgw_sbrk(i) ((void *) -1)
#define fgw_rand rand
#define fgw_srand srand
#define fgw_millisleep(ms) Sleep(ms)

#endif /* NT*/

#define COSLC
#endif
