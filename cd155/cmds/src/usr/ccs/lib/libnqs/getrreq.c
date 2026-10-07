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
 
/*++ getrreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/getrreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Read the entire control file header for an NQS control file.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:52 $ $State: Exp $)
 * $Log: getrreq.c,v $
 * Revision 1.3  1994/11/19  02:26:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:30  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:45  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:49:31  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)getrreq.c	1.2 (getrreq.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#include "nqs.h"
#include "nqsxdirs.h"			/* Get Nqs_control defn */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif

extern void pack6name();		/* Compute name on 6-bit alphabet */
extern int readreq();			/* Read entire request header */

/*** getrreq
 *
 *
 *	int getrreq():
 *	Open/read the request header portion of the control file for an
 *	NQS request.
 *
 *	Returns:
 *	      >=0: if successful, in which case the value returned
 *		   is the file descriptor opened for reading (but NOT
 *		   writing) the control file;
 *	       -1: if the req control file was invalid, or an open
 *		   error occurred.
 *
 *			Note:	errno is returned 0, unless some system
 *				call error has occurred.
 */
int getrreq (orig_seqno, orig_mid, rawreq)
long orig_seqno;		/* Sequence number of req */
mid_t orig_mid;			/* Machine-id for req */
struct rawreq *rawreq;		/* Raw request information to be returned */
{
	char path [MAX_PATHNAME+1];	/* Control directory pathname */
	register int fd;		/* File descriptor for control file */

	pack6name (path, Nqs_control, (int) (orig_seqno % MAX_CTRLSUBDIRS),
		  (char *) 0, (long) orig_seqno, 5, (long) orig_mid, 6, 0, 0);
	if ((fd = open (path, O_RDONLY)) == -1) {
		/*
		 *  Unable to open the control file.
		 *  Errno has error code.
		 */
		return (-1);
	}
	if (readreq (fd, rawreq) == 0) return (fd);	/* Success */
	close (fd);					/* Close the file */
	return (-1);					/* Bad control file */
}
