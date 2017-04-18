/*
	CLI.c  -  sample SQSL command line interface application

	The Structured Query Scripting Language
	Copyright (C) 1992-2016 Marco Greco (marco@4glworks.com)

	Initial release: Apr 04
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

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cascc.h"
#include "ccmpc.h"
#include "cfioc.h"
#include "csqpc.h"
#include "csqrc.h"

/*
 * this is sloppy, but given the use that we make of readline, going through
 * all the hoops of discovering an installed (vs just built) library plus
 * all possible variations of header locations was too much of a hassle
 */
#ifdef READLINE_LIBRARY
extern char *readline(char *p);
extern int rl_variable_bind(char *variable, char *value);
extern void add_history(char *command);
#endif

#define ERRMSGSZ 256

execinfo_t execinfo;

/*
 * usage output
 */
static void usage(char *argv)
{
    printf("Usage:\n\n");
    printf("%s [-V|[-N] [-D var=expr...] [file arg...]]]\n", argv);
    exit(0);
}

/*
 * version output
 */
static void version()
{
    printf ("%s:%s Version %s (%s)\n\n", PACKAGE_NAME, COMP_NAME, VERSION,
	    PACKAGE_DATE);
}

/*
 * store extra args in ARGV
 */
static void addargv(fgw_loc_t *vars, int cnt, char *arg)
{
    char buf[10];
    fgw_hstentry *hid;
    int d;

    sprintf((char *) &buf, "%d", cnt);
    hid=fgw_hstget(vars, 0, "argv", &d);
    fgw_hstadd(vars, hid->magic, (char *) &buf, CSTRINGTYPE, strlen(arg), arg);
}

/*
 * the actual beast
 */
int main(int argc, char **argv)
{
    fgw_loc_t vars, txtvar, q;
    fgw_fdesc *ifd;
    char *inf=NULL;
    int cnt=1;
    int dorc=1;
    int ofd;
    int flags=K_isbatch|K_verbose|K_dml|K_noconf|K_preserve;
    int width=80;
    int r;

    memset(&vars, 0, sizeof(vars));
    vars.loc_loctype=LOCMEMORY;
    vars.loc_indicator=-1;
    memset(&txtvar, 0, sizeof(txtvar));
    txtvar.loc_loctype=LOCMEMORY;
    txtvar.loc_indicator=-1;
    memset(&q, 0, sizeof(q));
    q.loc_loctype=LOCMEMORY;
    q.loc_indicator=-1;
    r=fgw_sqlexec(" ", 1, 0, flags, width, &vars, &txtvar, &execinfo);
    fgw_freetext(&txtvar);
    if (r)
    {
	char errmsg[ERRMSGSZ];

	fgw_errmsg(r, &execinfo.errorinfo, (char *) errmsg, sizeof(errmsg));
	fprintf(stderr, "\n%s\n\n", errmsg);
	exit(1);
    }
    if ((ofd=fgw_addfd(1, read, write, close))==-1)
    {
	fprintf(stderr, "cannot register stdout: probable lack of memory\n");
	exit(1);
    }
/*
 * parse command line
 */
    {
	int rc, opt;
	char *optstring="D:NV";

	while ((opt=getopt(argc, argv, optstring))!=EOF)
	    switch(opt)
	    {
	      case 'D':
		rc=fgw_evalassign(optarg, &vars);
		if (rc)
		{
		    char errmsg[ERRMSGSZ];

		    fgw_errmsg(rc, &execinfo.errorinfo, (char *) errmsg, sizeof(errmsg));
		    fprintf(stderr, "\n%s\n\n", errmsg);
		    exit(1);
		}
		break;
	      case 'N':
		dorc=0;
		break;
	      case 'V':
		version();
		if (argc!=2)
		    usage(argv[0]);
		exit(0);
	      default:
		usage(argv[0]);
	    }
/*
 * from here on pupulate ARGV
 */
	fgw_hstaddnode(&vars, 0, "argv", 10);
	while (optind<argc)
	{
	    if (cnt==1)
		inf=argv[optind];
	    addargv(&vars, cnt, argv[optind]);
	    cnt++;
	    optind++;
	}
/*
 * complete ARGV, ARGC
 */
	addargv(&vars, 0, argv[0]);
	fgw_hstadd(&vars, 0, "argc", CINTTYPE, 0, (char *) &cnt);
    }
/*
 * fire rc script
 */
    if (dorc)
    {
	char rcp[256];
	fgw_fdesc *rfd;

#ifdef NT
	char *hp, *hd, *h;

	if (strlen(h=getenv("HOME")))
	    sprintf((char *) &rcp, "%s\\.sqslrc", h);
	else if (strlen(hd=getenv("HOMEDRIVE")) &&
		 strlen(hp=getenv("HOMEPATH")))
	    sprintf((char *) &rcp, "%s%s\\.sqslrc", hd, hp);
	else
	    strcat((char *) &rcp, "\\.sqslrc");
#else
	sprintf((char *) &rcp, "%s/.sqslrc", getenv("HOME"));
#endif
	if (rfd=fgw_fileopen(rcp, "r"))
	{
	    fgw_blobread(rfd->fd_entry.int_id, &q);
	    r=fgw_sqlexec(q.loc_buffer, q.loc_size, 0, flags&~K_verbose, width,
			  &vars, &txtvar, &execinfo);
	    if (r)
	    {
		char errmsg[ERRMSGSZ];

		fgw_errmsg(r, &execinfo.errorinfo, (char *) errmsg, sizeof(errmsg));
		fprintf(stderr, "\n%s\n\n", errmsg);
		exit(1);
	    }
	    fgw_freetext(&txtvar);
	    if (q.loc_buffer)
		free(q.loc_buffer);
	    q.loc_size=0;
	    q.loc_bufsize=0;
	    q.loc_indicator=-1;
	    fgw_fdclose(rfd);
	}
    }
/*
 * ignoring SIGINT should work on NT too
 */
    signal(SIGINT, SIG_IGN);
/*
 * interactive
 */
    if (!inf)
    {
	char prompt[30], *c, *c1;

	flags|=K_quit|K_preserve;
/*
 * remove directory prefix and extension from program name
 */
	c=strrchr(argv[0], '/');
#ifdef NT
	if ((c1=strrchr(argv[0], '\\')) && (!c || (c && c1>c)))
	    c=c1;
#endif
	if (c)
	    c++;
	else
	    c=argv[0];
	sprintf(prompt, "%.27s> ", c);
	if (c=strchr((char *) &prompt, '.'))
	    strcpy (c, "> ");

/* FIXME: change in between vi and emacs modes */
#ifdef READLINE_LIBRARY
	rl_variable_bind("editing-mode", "emacs"); 
#endif
	while (1)
	{
	    char *bp, *c;

/*
 * query user
 */
#ifndef READLINE_LIBRARY
	    char buff[200];

	    printf(prompt);
	    if (fgets(bp=(char *) &buff, sizeof(buff)-1, stdin))
	    {
#else
	    if (bp=readline(prompt))
	    {
		if (*bp)
		    add_history(bp);
#endif
/*
 * service bang
 * FIXME: this should be more sofisticated, eg skip blanks, remove trailer
 *	  [CR]LF. Also we should handle a lone !, rather than forcing the
 *	  user to !exec $SHELL. But then, Informix GA apps don't, so why
 *	  should I bother?
 */
		if (*bp=='!')
		    system(bp+1);
/*
 * service request
 */
		else
		{
		    if ((r=fgw_sqlexec(bp, strlen(bp), ofd, flags, width,
					&vars, &txtvar, &execinfo))==RC_QUIT)
			exit(0);
		    if (r)
		    {
			char errmsg[ERRMSGSZ];

			fgw_errmsg(r, &execinfo.errorinfo, (char *) errmsg, sizeof(errmsg));
			fprintf(stderr, "\n%s\n\n", errmsg);
		    }
		}
	    }
	}
    }
/*
 * Standard input
 */
    else if (!strcmp(inf, "-"))
    {
	int i;

	if ((i=fgw_addfd(0, read, write, close))==-1)
	{
	    fprintf(stderr, "cannot register stdin: probable lack of memory\n");
	    exit(1);
	}
	ifd=fgw_findfd(i, 0);
    }
/*
 * File
 */
    else if (!(ifd=fgw_fileopen(inf, "r")))
    {
	fprintf(stderr, "error %i while opening %s\n", errno, argv[1]);
	exit(1);
    }
    fgw_blobread(ifd->fd_entry.int_id, &q);
    fgw_fdclose(ifd);
/*
 * process query
 */

    r=fgw_sqlexec(q.loc_buffer, q.loc_size, ofd, flags, width, &vars, &txtvar, &execinfo);
    if (r)
    {
	char errmsg[ERRMSGSZ];

	fgw_errmsg(r, &execinfo.errorinfo, (char *) errmsg, sizeof(errmsg));
	fprintf(stderr, "\n%s\n\n", errmsg);
	exit(1);
    }
    exit(0);
}
