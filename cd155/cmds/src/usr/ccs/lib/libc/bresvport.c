/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: bresvport.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:43 $";
#endif
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * bresvport.c	1.2  com/lib/c/net,3.1,8943 7/21/89 08:03:41
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

/*
 * Copyright (c) 1987 by Sun Microsystems, Inc.
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
 * Bind a socket to a privileged IP port
 */
bindresvport(sd, sin)
	int sd;
	struct sockaddr_in *sin;
{
	int res;
	static short port;
	struct sockaddr_in myaddr;
	extern int errno;
	int i;

#define STARTPORT 600
#define ENDPORT (IPPORT_RESERVED - 1)
#define NPORTS	(ENDPORT - STARTPORT + 1)

	if (sin == (struct sockaddr_in *)0) {
		sin = &myaddr;
		bzero(sin, sizeof (*sin));
		sin->sin_family = AF_INET;
	} else if (sin->sin_family != AF_INET) {
		errno = EPFNOSUPPORT;
		return (-1);
	}
	if (port == 0) {
		port = (getpid() % NPORTS) + STARTPORT;
	}
	res = -1;
	errno = EADDRINUSE;
	for (i = 0; i < NPORTS && res < 0 && errno == EADDRINUSE; i++) {
		sin->sin_port = htons(port++);
		if (port > ENDPORT) {
			port = STARTPORT;
		}
		res = bind(sd, sin, sizeof(struct sockaddr_in));
	}
	return (res);
}
