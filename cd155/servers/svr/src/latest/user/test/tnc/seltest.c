/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: seltest.c,v $
 * Revision 1.6  1995/02/02  01:01:14  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  21:05:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:54:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:33:25  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:22:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:05:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:50:44  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:06:43  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:59:19  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.1  1992/04/14  15:04:42  klh
 * Fix RCS Log.
 *
 * $EndLog$
 */
#include <sys/types.h>
#include <sys/time.h>

main(argc)
int argc;
{
	int nfound, nfds;
	extern int errno;
	fd_set readfds, writefds, exceptfds;
	struct timeval timeout;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	FD_SET(0,&readfds);
	FD_SET(1,&readfds);
	FD_SET(2,&readfds);
#ifdef 0
	FD_SET(0,&writefds);
	FD_SET(1,&writefds);
	FD_SET(2,&writefds);
	FD_SET(0,&exceptfds);
	FD_SET(1,&exceptfds);
	FD_SET(2,&exceptfds);
#endif

	nfds = 5;			/* max of 5 file descriptors */

	if( argc <= 1 ) {		/* zero timeout value */
		timeout.tv_sec = 0;
	}
	else {				/* 5 second timeout value */
		timeout.tv_sec = 5;
		printf("starting %d second timeout value\n",timeout.tv_sec);
	}
	timeout.tv_usec = 0;

	errno = 0;
	nfound = select(nfds, &readfds, &writefds, &exceptfds, &timeout);

	printf("nfound = %d (errno=%d)\n", nfound, errno);

	exit(0);
} 


/*
 * FD_SET(fd, &fdset)
 * FD_CLR(fd, &fdset)
 * FD_ISSET(fd, &fdset)
 * FD_ZERO(&fdset)
 * int fd;
 * fd_set fdset;
 */
