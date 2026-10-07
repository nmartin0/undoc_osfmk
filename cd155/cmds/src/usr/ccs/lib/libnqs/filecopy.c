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
 
/*++ filecopy.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/filecopy.c,v $
 *
 * DESCRIPTION:
 *
 *	Copy a file.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:40 $ $State: Exp $)
 * $Log: filecopy.c,v $
 * Revision 1.3  1994/11/19  02:26:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:03  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:33  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:48:25  hender
 * Sterling version 4/22/87
 * 
 *
 */

extern int errno;			/* System call error number */

/*** filecopyentire
 *
 *
 *	int filecopyentire():
 *	Copy a file from one open file descriptor to another.
 *	Don't stop until EOF.
 *
 *	Returns:
 *	      >=0: if successful (number of bytes transferred);
 *	       -1: if an error occurs (in which case errno is set).
 *		   Errno will equal zero, if we simply ran out of
 *		   file space writing the copy.
 */
long filecopyentire (from, to)
register int from;			/* "From" file descriptor */
register int to;			/* "To" file descriptor */
{
	char buffer [8192];		/* Copy buffer */
	register int nread;		/* One time number of bytes read */
	register int nwritten;		/* One time number of bytes written */
	register long cumulative;	/* Sum of nwritten's */

	errno = 0;			/* So that file limit returns 0 errno */
	cumulative = 0;
	while ((nread = read (from, buffer, sizeof (buffer))) != 0) {
		if (nread < 0) return (-1L);
		if ((nwritten = write (to, buffer, nread)) != nread)
		{	
			/* TAC the printf is an Intergraph change */
			printf("E$INTERGRAPH nread=%d, nwritten=%d, errno=%d\n",
				nread, nwritten, errno);
			return(-1L);
		}
		cumulative += nwritten;
	}
	return (cumulative);
}


/*** filecopysized
 *
 *
 *	int filecopysized():
 *	Copy a file from one open file descriptor to another.
 *	Stop as soon as the specified number of bytes have been moved.
 *
 *	Returns:
 *	      >=0: if successful, or if it reached a premature EOF
 *		   (number of bytes transferred);
 *	       -1: if an error occurs (in which case errno is set).
 *		   Errno will equal zero, if we simply ran out of
 *		   file space writing the copy.
 */
long filecopysized (from, to, bytes)
register int from;			/* "From" file descriptor */
register int to;			/* "To" file descriptor */
long bytes;				/* How many bytes to move */
{
	char buffer [8192];		/* Copy buffer */
	register int nread;		/* One time number of bytes read */
	register int nwritten;		/* One time number of bytes written */
	register long need;		/* Number of bytes still needed */

	errno = 0;			/* So that file limit returns 0 errno */
	for (need = bytes; need > 0; need -= nwritten) {
		if ((nread = read (from, buffer,
			need < sizeof buffer ? need : sizeof buffer)) != 0) {
			if (nread < 0) return (-1L);
			if ((nwritten = write (to, buffer, nread)) != nread)
				return (-1L);
		}
		else return (bytes - need);
	}
	return (bytes);
}
