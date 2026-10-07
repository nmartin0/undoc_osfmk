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
 
/*++ nqs_wakdes.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_wakdes.c,v $
 *
 * DESCRIPTION:
 *
 *	Re-enable pipe queue destinations that are presently in retry
 *	mode, whose retry wait time has elapsed.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 30, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:33 $ $State: Exp $)
 * $Log: nqs_wakdes.c,v $
 * Revision 1.3  1994/11/19  02:53:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:59  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:56  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:10:40  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"
#include "nqsxvars.h"			/* Global vars and directories */

extern void nqs_vtimer();		/* Set virtual timer */
extern void psc_spawn();		/* Maybe spawn some pipe reqs */
extern time_t time();			/* GMT time since 0:00:00 Jan 1, 1970 */
extern void udb_destination();		/* Update destination */

/*** nqs_wakdes
 *
 *
 *	void nqs_wakdes():
 *
 *	Re-enable pipe queue destinations that are presently in retry
 *	mode, whose retry wait time has elapsed.
 */
void nqs_wakdes()
{
	register struct pipeto *dest;	/* Pipe queue destination */
	time_t timenow;			/* Current GMT time */

	time (&timenow);		/* Get current time */
	dest = Pipetoset;		/* Global pipe queue destination set */
	while (dest != (struct pipeto *) 0) {
		if (dest->status == DEST_RETRY) {
			/*
			 *  This destination is in the retry state, and
			 *  has NOT been re-enabled.
			 */
			if (dest->retry_at > timenow) {
				/*
				 *  Set alarm for destination retry in
				 *  the future.
				 */
				nqs_vtimer (&dest->retry_at, nqs_wakdes);
			}
			else {
				/*
				 *  This destination must be enabled
				 *  (leaving the retry bit set)!
				 */
				dest->status |= DEST_ENABLED;
				udb_destination (dest);	/* Update database */
			}
		}
		dest = dest->next;	/* Examine next destination */
	}
	/*
	 *  Maybe spawn some pipe queue requests, and update and
	 *  pipe queue request-ordering files that need to be
	 *  modified.
	 */
	psc_spawn();			/* Maybe spawn some pipe requests */
}
