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
 
/*++ getsockch.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/getsockch.c,v $
 *
 * DESCRIPTION:
 *
 *	Routines for reading from a socket.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 21, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:53 $ $State: Exp $)
 * $Log: getsockch.c,v $
 * Revision 1.3  1994/11/19  02:26:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:32  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:46  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:49:37  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if	NETWORKED
#if	STREAM_SOCKETS

#include <stdio.h>
#include "nqs.h"			/* NQS types and definitions */
					/* + sys/types.h + nmap.h + quolim.h */
#include <errno.h>

/*
 *	External variables.
 */
extern int errno;			/* System call error codes */

/*
 *	Internal variables.
 */
static int sockfd;			/* Socket file descriptor */
static char buffer [MAX_PACKET];	/* Receive buffer */
static int bufptr = 0;			/* Index of next char in buffer */
static int n_read = 0;			/* Index of 1st empty slot in buffer */


/*** bufsockch
 *
 *
 *	char *bufsockch():
 *
 *	Return a pointer to the characters that
 *	1) this module has read from the socket, and
 *	2) have not yet been turned over with getsockch().
 *
 */
char *bufsockch ()
{
	return (&buffer[bufptr]);
}


/*** extrasockch
 *
 *
 *	int extrasockch():
 *
 *	Report the number of characters that
 *	1) this module has read from the socket, and
 *	2) have not yet been turned over with getsockch().
 */
int extrasockch ()
{
	return (n_read - bufptr);
}


/*** getsockch
 *
 *
 *	int getsockch():
 *
 *	Return a character from a socket.
 */
int getsockch ()
{
	register int no_packet_yet;

	if (bufptr < n_read) {
		return (buffer [bufptr++] & 0377);
	}
	bufptr = 1;			/* Return buffer [1] NEXT time */
	no_packet_yet = 1;
	do {
		if ((n_read = read (sockfd, buffer, MAX_PACKET)) == -1) {
			if (errno != EINTR) {
				return (-1);
			}
		}
		else no_packet_yet = 0;
	} while (no_packet_yet);
	if (n_read == 0) {
		return (-1);
	}
	return (buffer [0] & 0377);
}


/*** setsockfd
 *
 *
 *	void setsockfd():
 *
 *	Notify this module which socket it is that it should read on.
 */
void setsockfd (fd)
int fd;					/* File descriptor */
{
	sockfd = fd;
}

#else
BAD NETWORK TYPE
#endif	STREAM_SOCKETS
#endif	NETWORKED
