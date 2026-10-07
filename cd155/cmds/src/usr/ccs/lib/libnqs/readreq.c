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
 
/*++ readreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/readreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Read the request header portion of the control file for an
 *	NQS request.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:40 $ $State: Exp $)
 * $Log: readreq.c,v $
 * Revision 1.3  1994/11/19  02:27:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:30  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:43  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:54:11  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"

extern int errno;
extern long lseek();			/* Long seek */
extern int verifyreq();			/* Verify request header */


/*** readreq
 *
 *
 *	int readreq():
 *
 *	Read the request header portion of the control file for an
 *	NQS request.
 *
 *	If the return value of this function is 0, then the lseek()
 *	position of the control file descriptor upon return, is
 *	ALWAYS left at the first byte of the varying portion of the
 *	control file.
 *
 *
 *	Returns:
 *		0: if the successful, and the request is valid;
 *	       -1: otherwise.
 *
 *			Note:	errno is returned 0, unless a
 *				system call error occurred.
 */
int readreq (fd, rawreq)
int fd;				/* Control file descriptor */
struct rawreq *rawreq;		/* Raw request information to be returned */
{
	register int sizeread;
	register int headersize;

	/*
	 *  Read the entire control file header in one read() system
	 *  call.
	 *
	 *  We do this for reasons of efficiency, and also so that the
	 *  first block of the control file is read in one atomic I/O
	 *  operation so that concurrent Qmod updates (being done by the
	 *  careful indivisible rewrite of the first control file block),
	 *  and multiple read accesses do not step on each other.
	 */
	errno = 0;			/* Clear errno */
	if (lseek (fd, 0L, 0) == -1) {	/* Seek to the start of the file */
		return (-1);		/* Return EBADF */
	}
	sizeread = read (fd, (char *) rawreq, sizeof (struct rawreq));
	headersize = ((char *) &rawreq->v) - ((char *) rawreq);
	if (rawreq->type == RTYPE_DEVICE) {
		/*
		 *  The request is a device-oriented request.
		 */
		headersize += sizeof (struct rawdevreq);
		if (sizeread < headersize) {
			/*
			 *  Part of the control file is missing!
			 */
			return (-1);
		}
	}
	else {
		/*
		 *  The request is not device-oriented.
		 */
		headersize += sizeof (struct rawbatreq);
		if (sizeread < headersize) {
			/*
			 *  Part of the control file is missing!
			 */
			return (-1);
		}
	}
	/*
	 *  Move file pointer to the end of the header area.
	 *  This is CRITICAL.
	 */
	lseek (fd, (long) headersize, 0);
	/*
	 *  Return results of header verification.
	 */
	return (verifyreq (rawreq));
}
