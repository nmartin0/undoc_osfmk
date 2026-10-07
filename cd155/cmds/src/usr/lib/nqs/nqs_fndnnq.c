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
 
/*++ nqs_fndnnq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_fndnnq.c,v $
 *
 * DESCRIPTION:
 *
 *	Return pointer to queue structure for the named non-network
 *	queue.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	January 23, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:57 $ $State: Exp $)
 * $Log: nqs_fndnnq.c,v $
 * Revision 1.3  1994/11/19  02:52:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:32  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:07  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:06:10  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"
#include "nqsxvars.h"		/* External variables */

extern int strcmp();		/* String comparison */


/*** nqs_fndnnq
 *
 *
 *	struct queue *nqs_fndnnq():
 *
 *	Return pointer to queue structure for the named non-network
 *	queue.
 *
 *	Returns:
 *		A pointer to the queue structure for the named non-
 *		network queue, if the named non-network queue exists;
 *		otherwise (struct queue *)0 is returned.
 */
struct queue *nqs_fndnnq (quename)
register char *quename;
{
	register struct queue *queue;
	register int res;

	/*
	 *  We take advantage of the fact that the non-network queue set
	 *  is lexicographically ordered.
	 */
	queue = Nonnet_queueset;
	while (queue != (struct queue *)0) {
		if ((res = strcmp (queue->q.namev.name, quename)) < 0) {
			/*
			 *  We have not yet found the queue, but we should
			 *  keep on searching.
			 */
			queue = queue->next;
		}
		else if (res == 0) return (queue);	/* Found! */
		else return ((struct queue *)0);	/* Not found */
	}
	return (queue);					/* Not found */
}
