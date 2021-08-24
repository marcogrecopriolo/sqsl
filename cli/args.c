/*
	ARGS.c  -  sample SQSL c applications parameter parsing

	The Structured Query Scripting Language
	Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)

	Initial release: Apr 04
	Current release: Aug 21

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

#include <stdlib.h>
#include "cascc.h"
#include "cfioc.h"
#include "csqpc.h"

#define ERRMSGSZ 256

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
char *parse_args(int argc, char **argv, fgw_loc_t *vars)
{
    execinfo_t execinfo;
    fgw_loc_t txtvar, q;
    fgw_fdesc *ifd;
    char *file_arg=NULL;
    int cnt=1;
    int dorc=1;
    int flags=K_isbatch|K_verbose|K_dml|K_noconf|K_preserve;
    int width=80;
    int r;

    memset(vars, 0, sizeof(fgw_loc_t));
    vars->loc_loctype=LOCMEMORY;
    vars->loc_indicator=-1;
    memset(&txtvar, 0, sizeof(txtvar));
    txtvar.loc_loctype=LOCMEMORY;
    txtvar.loc_indicator=-1;
    memset(&q, 0, sizeof(q));
    q.loc_loctype=LOCMEMORY;
    q.loc_indicator=-1;
    r=fgw_sqlexec(" ", 1, 0, flags, width, vars, &txtvar, &execinfo);
    fgw_freetext(&txtvar);
    if (r)
    {
	char errmsg[ERRMSGSZ];

	fgw_errmsg(r, &execinfo.errorinfo, (char *) errmsg, sizeof(errmsg));
	fprintf(stderr, "\n%s\n\n", errmsg);
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
		rc=fgw_evalassign(optarg, vars);
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
	fgw_hstaddnode(vars, 0, "argv", 10);
	while (optind<argc)
	{
	    if (cnt==1)
		file_arg=argv[optind];
	    addargv(vars, cnt, argv[optind]);
	    cnt++;
	    optind++;
	}
/*
 * complete ARGV, ARGC
 */
	addargv(vars, 0, argv[0]);
	fgw_hstadd(vars, 0, "argc", CINTTYPE, 0, (char *) &cnt);
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
			  vars, &txtvar, &execinfo);
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
    return file_arg;
}
