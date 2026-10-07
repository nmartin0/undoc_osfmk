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
 
/*++ openqord.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/openqord.c,v $
 *
 * DESCRIPTION:
 *
 *	Open the queue ordering file for the specified queue descriptor,
 *	if there are requests in the queue.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:29 $ $State: Exp $)
 * $Log: openqord.c,v $
 * Revision 1.3  1994/11/19  02:27:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:02  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:29  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:52:53  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)openqord.c	1.2 (openqord.c OSF/1 NQS2.0 GJK) 6/30/92";
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
#define	MAX_RETRY	50	/* Maximum queue ordering file retry limit */

/*
 *	External functions.
 */
extern long blockdb();		/* Get block number for file offset */
extern struct gendescr *nextdb();
				/* Read the next allocated descriptor */
				/* from an NQS database file */
extern void seekdb();		/* Seek on an NQS database file */
extern char *strcpy();		/* String copy */
extern int strcmp();		/* String comparison */
extern long telldb();		/* Tell offset of last read descr */


/*** openqord
 *
 *
 *	int openqord():
 *
 *	Open the queue ordering file for the specified queue descriptor,
 *	of there are requests in the queue.
 *
 *	Returns:
 *	      >=0: if the queue has requests, and the queue ordering
 *		   file was successfully opened.  The value returned
 *		   is the file descriptor opened to read the queue
 *		   ordering file.
 *	       -1: if there are no requests in the queue (no queue
 *		   ordering file is opened).
 *	       -2: if there ARE requests in the queue, but some terrible
 *		   error prevented us from opening the queue ordering
 *		   file.
 *	       -3: if the queue has been deleted.
 */
int openqord (queuefile, descr)
struct confd *queuefile;		/* Queue description file */
struct gendescr *descr;			/* Queue descriptor */
{
	char path [MAX_PATHNAME+1];	/* Queue ordering file pathname */
	char quename [MAX_QUEUENAME+1];	/* Name of queue */
	register int fd;		/* Queue ordering file descriptor */
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
	 *  Open the request queue ordering file.
	 */
	retry = 0;			/* Retry count */
	fd = -1;			/* Queue ordering file not open */
	while (retry <= MAX_RETRY &&
	      (descr->v.que.departcount || descr->v.que.runcount ||
	       descr->v.que.stagecount || descr->v.que.queuedcount ||
	       descr->v.que.waitcount || descr->v.que.holdcount ||
	       descr->v.que.arrivecount)) {
		if (descr->v.que.type == QUE_NET) {
			sprintf (path, "%s/n%08x%04x", Nqs_qorder,
				 telldb(queuefile), descr->v.que.orderversion);
		}
		else {
			sprintf (path, "%s/q%08x%04x", Nqs_qorder,
				 telldb(queuefile), descr->v.que.orderversion);
		}
		if ((fd = open (path, O_RDONLY)) == -1) {
			/*
			 *  We did not succeed in opening the queue
			 *  ordering file (because a queue update JUST
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
	if (fd == -1) {			/* Queue ordering file not open */
		if (descr->v.que.departcount || descr->v.que.runcount ||
		    descr->v.que.stagecount || descr->v.que.queuedcount ||
		    descr->v.que.waitcount || descr->v.que.holdcount ||
		    descr->v.que.arrivecount) {
			/*
			 *  Something is wrong since the queue exists and
			 *  has reqs, but no queue ordering file.
			 */
			return (-2);	/* Unable to open queue file! */
		}
		return (-1);		/* No requests in the queue */
	}
	return (fd);			/* Return the opened file descriptor */
}
