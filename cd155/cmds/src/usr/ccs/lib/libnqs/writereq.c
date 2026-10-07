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
 
/*++ writereq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/writereq.c,v $
 *
 * DESCRIPTION:
 *
 *	[Re]write a control file request header.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:28:06 $ $State: Exp $)
 * $Log: writereq.c,v $
 * Revision 1.3  1994/11/19  02:28:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:00:13  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:23  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:57:24  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"
#include "errno.h"

extern int errno;
extern long lseek();			/* Long seek */


/*** writereq
 *
 *
 *	int writereq():
 *
 *	[Re]write the request header portion of the control file
 *	for an NQS request.
 *
 *	Unless errno is returned non-zero, the lseek() position
 *	of the control file descriptor upon return, is ALWAYS at
 * 	the first byte of the varying portion of the control file.
 *
 *
 *	Returns:
 *		0: if successful;
 *	       -1: otherwise.
 *
 *			Note:	errno will be returned 0 by this function
 *				unless a system call error has occurred.
 */
int writereq (fd, rawreq)
int fd;					/* File descriptor for control file */
struct rawreq *rawreq;			/* Raw request information */
{
	register int size;		/* Header size */

	errno = 0;			/* Clear errno */
	if (lseek (fd, 0L, 0) == -1) {	/* Seek to start of control file */
		return (-1);		/* Return EBADF */
	}
	/*
	 *  [Re]write the header portion of an NQS control file, in one
	 *  single write() system call.
	 *
	 *  We do this for reasons of efficiency, and also so that the first
	 *  block of the control file is [re]written in a single atomic
	 *  I/O operation so that Qmod updates, and multiple readers of
	 *  the control file (who are also carefully reading the first
	 *  block of the control in an indivisible fashion), will not
	 *  step on each other.
	 */
	size = ((char *) &rawreq->v) - ((char *) rawreq);
	if (rawreq->type == RTYPE_DEVICE) {
		/*
		 *  We are [re]writing the control file header of a
		 *  device-oriented request.
		 */
		size += sizeof (struct rawdevreq);
	}
	else {
		/*
		 *  We are [re]writing the control file header of a
		 *  non device-oriented request.
		 */
		size += sizeof (struct rawbatreq);
	}
	if (write (fd, (char *) rawreq, size) != size) {
		if (errno == 0) {
			/*
			 *  We presume that the receiving device has been
			 *  completely filled-up, and artifically set errno
			 *  to ENOSPC, for the benefit of the caller.
			 */
			errno = ENOSPC;	/* Insufficient space */
		}
		return (-1);		/* Write() failed */
	}
	return (0);
}
