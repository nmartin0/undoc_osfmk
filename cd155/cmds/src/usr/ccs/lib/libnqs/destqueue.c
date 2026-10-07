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
 
/*++ destqueue.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/destqueue.c,v $
 *
 * DESCRIPTION:
 *
 *	Return a pointer to the queue-name portion of a remote
 *	queue specification.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 15, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:35 $ $State: Exp $)
 * $Log: destqueue.c,v $
 * Revision 1.3  1994/11/19  02:26:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:51  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:27  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:47:56  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"			/* NQS types and definitions */

/*** destqueue
 *
 *
 *	char *destqueue():
 *
 *	Return a pointer to the queue-name portion of a remote
 *	queue specification.
 */
char *destqueue (remote_queue)
register char *remote_queue;		/* Remote queue specification text */
{
	static char quename [MAX_QUEUENAME+2];
					/* Remote queue name of one */
					/* extra byte so that mgr_scan.c */
					/* can tell a remote queue */
					/* name that is too long. */
	register char *copyto;		/* Copyto pointer */
	register unsigned size;		/* Copy size */

	size = 0;			/* Nothing copied yet */
	copyto = quename;		/* Where to copy */
	while (*remote_queue && *remote_queue != '@' &&
	       size < MAX_QUEUENAME+1) {
		*copyto++ = *remote_queue++;
		size++;			/* One more character copied */
	}				/* specification */
	*copyto = '\0';			/* Null terminate */
	return (quename);		/* Return pointer to queue name */
}
