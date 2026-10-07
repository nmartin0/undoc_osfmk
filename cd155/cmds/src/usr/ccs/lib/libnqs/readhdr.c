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
 
/*++ readhdr.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/readhdr.c,v $
 *
 * DESCRIPTION:
 *
 *	Read and verify the common header portion of a request
 *	control file.
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
 * $Log: readhdr.c,v $
 * Revision 1.3  1994/11/19  02:27:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:28  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:42  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:54:07  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"

extern int errno;			/* System call error number */
extern long lseek();			/* Seek on a file */
extern int verifyhdr();			/* Verify common request header */

/*** readhdr
 *
 *
 *	int readhdr():
 *
 *	Read and verify the common header portion of a request
 *	control file.
 *
 *	If the return value of this function is 0, then the lseek()
 *	position of the control file descriptor upon return, is
 *	ALWAYS left at the first byte of the varying portion of the
 *	control file.
 *
 *
 *	Returns:
 *		0: if successful, and the request header is valid;
 *	       -1: otherwise.
 *
 *			Note:  errno will be returned 0 by this function
 *			       unless a system call error has occurred.
 */
int readhdr (fd, rawreq)
int fd;					/* Control file descriptor */
struct rawreq *rawreq;			/* Raw request info to be returned */
{
	register int size;		/* Size of common request header */

	errno = 0;			/* Clear errno */
	if (lseek (fd, 0L, 0) == -1) {	/* Seek to the beginning of the file */
		return (-1);		/* Report EBADF */
	}
	/*
	 *  We read the common header of the control file in a single
	 *  read() system call.
	 *
	 *  We do this for reasons of efficiency, and also so that the
	 *  first block of the control file is read in one atomic I/O
	 *  operation so that concurrent Qmod updates (being done by the
	 *  careful indivisible rewrite of the first control file block),
	 *  and multiple read accesses do not step on each other.
	 */
	size = ((char *) &rawreq->v) - ((char *) rawreq);
	if (read (fd, (char *) rawreq, size) != size) {
		/*
		 *  Something bad happened trying to read the control
		 *  file header.
		 */
		return (-1);
	}
	if (verifyhdr (rawreq) < 0) return (-1);
	/*
	 *  Leave the file pointer pointing at the varying part
	 *  of the request control file.
	 */
	if (rawreq->type == RTYPE_DEVICE) {
		/*
		 *  We are [re]writing the control file common
		 *  header of a device-oriented request.
		 */
		size = sizeof (struct rawdevreq);
	}
	else {
		/*
		 *  We are [re]writing the control file common
		 *  header of a non device-oriented request.
		 */
		size = sizeof (struct rawbatreq);
	}
	lseek (fd, (long) size, 1);	/* Relative seek */
	errno = 0;			/* Extreme paranoia */
	return (0);
}
