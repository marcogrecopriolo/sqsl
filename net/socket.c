/*
	socket.c  -  SQSL interpreter socket data streams

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Jun 16
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

#include "cosdc.h"
#include "errno.h"
#include <string.h>
#include <stdio.h>
#include "csqrc.h"
#include "crxuc.h"
#include "crqxd.h"
#include "cxslc.h"

#ifdef NT
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#define RC_BADIP     1
#define RC_BADPORT   2
#define RC_NOSOCK    3
#define RC_NOCONNECT 4
#define RC_BADFD     5
#define RC_BADWSA    6

static char *messages[] = {
    "invalid ip address or host name",
    "invalid port or service name",
    "could not obtain socket",
    "connection failed",
    "unable to add file descriptor",
    "error initializing winsock"
};

XSL_STATE

/*
**  initialize shared object
*/
DLLDECL int sqd_init(XSL_PARMS)
{
    XSL_SETSTATE
    return SO_MASK|SO_XPRFLG;
}

/*
**  read routine passed to fd_read
*/
static ssize_t sokread(int s, void *b, int l)
{
    return recv(s, b, l, 0);
}

/*
**  write routine passed to fd_write
*/
static ssize_t sokwrite(int s, void *b, int l)
{
    return send(s, b, l, 0);
}

DLLDECL int tcp(fgw_stacktype *stk)
{
    exprstack_t *host, *port, o;
    int ac, rc=0;
    struct sockaddr_in name;
    struct servent *servrec;
    struct hostent *hostrec;
    int sock;
    int fd_id;
    char *errorinfo=XSL_CALL(rxu_errorinfo)();
#ifdef NT
    WSADATA wsa;
#endif

    host=XSL_CALL(rxu_arglist)(stk, &ac);

    /* need 2 parameters */
    if (ac!=2)
    {
	rc=RC_BFINV;
	goto exit;
    }
    memset(&o, 0, sizeof(o));
    port=host+1;

#ifdef NT
    if (WSAStartup(MAKEWORD(2,2), &wsa)!=0)
    {
	rc=RC_BADWSA;
	goto exit;
    }
#endif

    /* we need usable args here */
    if (host->type!=CSTRINGTYPE || XSL_CALL(rxu_isnull)(host) ||
        ((hostrec=gethostbyname(host->val.string))==NULL &&
	 (hostrec=gethostbyaddr(host->val.string, strlen(host->val.string), AF_INET))==NULL))
    {
	rc=RC_BADIP;
	goto exit;
    }
    if (XSL_CALL(rxu_isnull)(port) || (port->type!=CINTTYPE && port->type!=CSTRINGTYPE))
    {
	rc=RC_BADPORT;
	goto exit;
    }

    if ((port->type!=CSTRINGTYPE || (servrec=getservbyname(port->val.string, "tcp"))==NULL) &&
	(servrec=getservbyport(htons(XSL_CALL(rxu_toint)(port, NULL)), "tcp"))==NULL)
    {
	rc=RC_BADPORT;
	goto exit;
    }
    name.sin_port=servrec->s_port;
    name.sin_family=hostrec->h_addrtype;
    XSL_CALL(fgw_move)((char *) &name.sin_addr, (char *) hostrec->h_addr,
	hostrec->h_length);
    if ((sock=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
	rc=RC_NOSOCK;
	goto exit1;
    }
    if (connect(sock, (struct sockaddr *) &name, sizeof(name))==-1)
    {
	rc=RC_NOCONNECT;
	goto exit1;
    }
    if ((fd_id=XSL_CALL(rxu_addfd)(sock, sokread, sokwrite, close))==-1)
    {
	close(sock);
	rc=RC_BADFD;
	goto exit;
    }

    o.type=CINTTYPE;
    o.val.intvalue=fd_id;
    XSL_CALL(rxu_pushresult)(stk, &o);
    return 0;

exit1:
#ifdef NT
    sprintf(errorinfo, "(%i)", WSAGetLastError());
#else
    sprintf(errorinfo, "(%i)", errno);
#endif

exit:

#ifdef NT
    WSACleanup();
#endif
    XSL_CALL(rxu_setnull)(&o, CSTRINGTYPE);
    XSL_CALL(rxu_pushresult)(stk, &o);
    return rc;
}

/*
** returns error message
*/
DLLDECL void sqd_errmsg(int err, char *buf, int len)
{   
    char b1[256], b2[256];
    int l;
    
    if (err<1 || err>=sizeof(messages))
	return;
    strncat(buf, messages[err-1], len);
}
