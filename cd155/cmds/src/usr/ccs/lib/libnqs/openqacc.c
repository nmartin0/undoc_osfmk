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
 
/*++ openqacc.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/openqacc.c,v $
 *
 * DESCRIPTION:
 *
 *	Open the queue access file for the specified queue descriptor,
 *	if queue access is by gid and uid.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	February 27, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:29 $ $State: Exp $)
 * $Log: openqacc.c,v $
 * Revision 1.3  1994/11/19  02:27:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:00  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:28  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:52:50  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)openqacc.c	1.2 (openqacc.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nqs.h"		/* NQS definitions */
#include "nqsxdirs.h"		/* NQS directories */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>		/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif

/*
 *	Configurable parameters.
 */
#define	MAX_RETRY	50	/* Maximum queue access file retry limit */

/*
 *	External functions.
 */
extern long blockdb();		/* Get block number for file offset */
extern struct gendescr *nextdb(); /* Read the next allocated descriptor */
				/* from an NQS database file */
extern void seekdb();		/* Seek on an NQS database file */
extern char *strcpy();		/* String copy */
extern int strcmp();		/* String comparison */
extern long telldb();		/* Tell offset of last read descr */


/*** openqacc
 *
 *
 *	int openqacc():
 *
 *	Open the queue access file for the specified queue descriptor,
 *	if access is by gid and uid.
 *
 *	Returns:
 *	      >=0: if queue access is by gid and uid, and the queue access
 *		   file was successfully opened.  The value returned
 *		   is the file descriptor opened to read the queue
 *		   access file.
 *	       -1: if queue access is unrestricted (no queue access
 *		   file is opened).
 *	       -2: if queue access IS by gid and uid, but some terrible
 *		   error prevented us from opening the queue access
 *		   file.
 *	       -3: if the queue has been deleted.
 */
int openqacc (queuefile, descr)
struct confd *queuefile;		/* Queue description file */
struct gendescr *descr;			/* Queue descriptor */
{
	char path [MAX_PATHNAME+1];	/* Queue access file pathname */
	char quename [MAX_QUEUENAME+1];	/* Name of queue */
	register int fd;		/* Queue access file descriptor */
	int retry;			/* Race condition retry */
	long position;			/* Offset in NQS database file of */
					/* the queue file descriptor */
	long blockpos;			/* The block offset in the NQS */
					/* queue file descriptor file of the */
					/* queue descriptor */

	position = telldb (queuefile);	/* Remember offset of descriptor */
	blockpos = blockdb (position) * ATOMICBLKSIZ;
	strcpy (quename, descr->v.que.namev.name);
	/*
	 *  Open the queue access file.
	 */
	retry = 0;			/* Retry count */
	fd = -1;			/* Queue access file not open */
	while (retry <= MAX_RETRY &&
	      (descr->v.que.status & QUE_BYGIDUID)) {
		sprintf (path, "%s/q%08x%04x", Nqs_qaccess,
			 telldb (queuefile), descr->v.que.accessversion);
		if ((fd = open (path, O_RDONLY)) == -1) {
			/*
			 *  We did not succeed in opening the queue
			 *  access file (because a queue update JUST
			 *  occurred)....  We may get in a race here
			 *  with the daemon.  However the likelihood
			 *  of this race going on for more than 1 or
			 *  2 iterations is VERY low.  We are simply
			 *  paying the price for not using locks (which
			 *  would also slow everyone down all of the
			 *  time, and make this whole thing less portable....
			 */
			seekdb (queuefile, blockpos);	/* Seek back to load */
							/* block */
			descr = nextdb (queuefile);	/* Read descr */
			while (descr != (struct gendescr *)0 &&
			       telldb (queuefile) < position) {
				descr = nextdb (queuefile);
			}
			if (descr == (struct gendescr *)0 ||
			    telldb(queuefile) > position ||
			    strcmp (descr->v.que.namev.name, quename)) {
				/*
				 *  The queue has been deleted, or
				 *  replaced by another queue!
				 */
				return (-3);		/* No such queue */
			}
			else retry += 1;		/* Keep counter */
		} 
		else retry = MAX_RETRY+1;		/* Set for exit */
	}
	if (fd == -1) {			/* Queue access file not open */
		if (descr->v.que.status & QUE_BYGIDUID) {
			/*
			 *  Something is wrong since the queue exists and
			 *  access is by gid and uid, but the queue access
			 *  file was not opened.
			 */
			return (-2);	/* Unable to open queue access file! */
		}
		return (-1);		/* Queue access is unrestricted */
	}
	return (fd);			/* Return the opened file descriptor */
}
