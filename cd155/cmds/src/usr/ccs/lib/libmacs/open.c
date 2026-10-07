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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	open.c
 *
 * c_open()
 *
 * Abstract:	This routine opens a TCP/IP connection to MACD.
 *
 * Arguments:	None
 *
 * Return value:	0	successful
 *			-1	error
 *
 * Note:	The socket number of the connection is stored in
 *		a global variable c_sock which is initially set
 *		to -1.
 */

#ifdef	LIBONET
#define OLDTCP
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>

#include <netdb.h>
#include <errno.h>
#include "macd.h"
#include "mac.h"

int c_open()
{
/*
 * Global variables defined in cpuctl.c
 */
	extern int c_sock;

/*
 * Local variables
 */
	int timo = 1, pid, oldmask;
	struct sockaddr_in sin;
	int lport;
	struct hostent *hp;
#ifdef	LIBONET
	long t;
#endif	/* LIBONET */
#ifdef CRAY_YMP
	static char local_host_addr[4] = {'\177','\0','\0','\1'};
#endif
#ifdef IPSC
#define HOSTNAMELENGTH (64)
        char host[HOSTNAMELENGTH];
#endif
	extern struct client_request reqst_hdr;
	extern struct macd_reply reply_hdr;
	extern u_short htons();

/*
 * No more than one connection per client
 */
	if (c_sock > 0) {
		errno = EISCONN;
		return(-1);
	}

/*	Intel does not want this info message
	if (isatty(fileno(stdin)))
		(void) printf("Trying to connect to the MACD...\n");

 * Open a socket, bind it with a previledged port then connect to MACD
 */
        c_sock = socket (AF_INET, SOCK_STREAM, 0);
        if (c_sock < 0) {
	    perror("Unable to open socket");
            return (-1);
        }
        /* must zero out sockaddr_in for bind to work */
        bzero ((char *) &sin, sizeof (struct sockaddr_in));
        hp = gethostbyname("localhost");
        sin.sin_family = AF_INET;
        bcopy(hp->h_addr_list[0], (caddr_t)&sin.sin_addr, hp->h_length);
 
        lport = IPPORT_RESERVED - 1;
        for (;;) {
            sin.sin_port = htons (lport);
            if (bind (c_sock, (caddr_t) &sin, sizeof (sin)) >= 0)
                break;
            if (errno != EADDRINUSE && errno != EADDRNOTAVAIL) {
		perror("Unable to bind");
                return (-1);
            }
            lport--;
            if (lport == IPPORT_RESERVED / 4) {
                fprintf(stderr, "c_open : All ports in use\n");
                return (-1);
            }
        }

	pid = getpid();
#ifdef CRAY_YMP
	hp = gethostbyaddr(local_host_addr,8,2);
#endif
#ifdef IPSC
        hp = gethostbyname("localhost");
#endif
#ifdef CRAY_YMP
	oldmask = sigblock(sigmask(SIGURG));
#endif
        bzero ((char *) &sin, sizeof (struct sockaddr_in));
	sin.sin_family = hp->h_addrtype;
#ifndef	LIBONET
	bcopy(hp->h_addr_list[0], (caddr_t)&sin.sin_addr, hp->h_length);
#else
	bcopy(hp->h_addr_list[0], (char *)&t, sizeof(t));
	sin.sin_addr = t;
#endif
	sin.sin_port = htons(MACD_PORT);
	if (connect(c_sock, (struct sockaddr *)&sin, sizeof (sin)) != 0) {
	    (void) fprintf (stderr, 
		"c_open: unable to connect, errno=%d\n", errno);
	    (void) close(c_sock);
	    return (-1);
	}

/*
 * Pack a request messge and send to MACD
 */
	reqst_hdr.command = C_OPEN;
	reqst_hdr.size = 0;
	if (write(c_sock, (char *)&reqst_hdr, sizeof(struct client_request)) < 
		sizeof(struct client_request)) {
		errno = EPIPE;
		return(-1);
	}

/*
 * Get reply message from MACD
 */
	if (read(c_sock, (char *)&reply_hdr, sizeof(struct macd_reply)) < 
		sizeof(struct macd_reply)) {
		errno = EPIPE;
		return(-1);
	}
	errno = reply_hdr.errno;

/*
 * Unseccessful open: close the socket
 */
	if (reply_hdr.returnvalue == -1) {
		close(c_sock);
		c_sock = -1;
	}

	return(reply_hdr.returnvalue);
}
