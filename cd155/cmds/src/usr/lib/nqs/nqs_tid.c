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
 
/*++ nqs_tid.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_tid.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the two public functions of:
 *
 *		tid_allocate(), and
 *		tid_deallocate()
 *
 *	which respectively allocate and deallocate transaction-ids.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 28, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:21 $ $State: Exp $)
 * $Log: nqs_tid.c,v $
 * Revision 1.3  1994/11/19  02:53:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:26  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:38  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:08:22  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"
#include "nqsxvars.h"			/* External variables and dirs */
#define	TRANSACTARRAYSIZE ((MAX_TRANSACTS+PRECISION_ULONG-1)/PRECISION_ULONG)
					/* Number of elements in the */
					/* freetransaction descriptor */
					/* bitmap array */

/*
 *	Variables local to this module.
 */
static short initialized = 0;		/* Bitmap initialized flag */
static unsigned long freetransact [TRANSACTARRAYSIZE];
					/* Bitmap of free transaction-ids */
					/* 1=free; 0=allocated */


/*** tid_allocate
 *
 *
 *	int tid_allocate():
 *	Allocate a transaction-id.
 *
 *	Returns:
 *		   0: No free transaction-id exists that
 *		      can be allocated for the request at
 *		      at the present time.
 *		>= 1: Return value denotes the allocated
 *		      transaction descriptor-id.  This
 *		      value will equal 'reserved_tid' if
 *		      'reserved_tid' was non-zero, AND
 *		      the specified tid was not already
 *		      reserved.
 */
int tid_allocate (reserved_tid, request_is_external)
int reserved_tid;			/* If non-zero, then the caller */
					/* wants this tid reserved */
int request_is_external;		/* Boolean true if the reserving */
					/* request was queued externally */
{
	void tid_initialize();		/* Initialize the state of this */
					/* module */

	register int i;			/* Index var */
	register int j;			/* Loop var */
	register unsigned long mask;	/* Bit mask */
	register unsigned long bitmap;	/* Bit map word */

	if (!initialized) tid_initialize();
	if (request_is_external) {
		/*
		 *  The associated request was queued from a remote
		 *  machine.  Each NQS machine places a limit on the
		 *  number of externally queued requests, so that
		 *  locally generated requests have a few extra
		 *  transaction-ids available only to them.
		 *
		 *  Without such a reservation, it is possible for
		 *  an NQS system to "drown" when too many requests
		 *  are queued from external sources.
		 */
		if (Extreqcount >= Maxextrequests) {
			/*
			 *  The limit on the number of externally
			 *  queued requests has already been
			 *  reached.
			 */
			return (0);
		}
	}
	if (reserved_tid) {
		/*
		 *  The caller wants this tid reserved.
		 */
		if (reserved_tid < 0 || reserved_tid > MAX_TRANSACTS) {
			/*
			 *  The tid to reserve does not exist.
			 */
			printf ("E$Invalid tid requested in tid_allocate().\n");
			fflush (stdout);
			return (0);
		}
		i = (reserved_tid - 1) / PRECISION_ULONG;
		mask = (1L << ((reserved_tid - 1) % PRECISION_ULONG));
		if (freetransact [i] & mask) {
			/*
			 *  The specified transaction descriptor
			 *  has not been previously reserved.
			 */
			mask = ~mask;
			freetransact[i] &= mask;/* Reserve the requested tid */
			return (reserved_tid);	/* Reservation succeeded */
		}
		/*
		 *  The specified tid is already reserved.
		 */
		return (0);	/* Reservation failed */
	}
	/*
	 *  The caller wants some tid allocation.  Any tid will do.
	 */
	for (i = 0; i < TRANSACTARRAYSIZE; i++) {
		bitmap = freetransact [i];	/* Get bitmap integer */
		if (bitmap) {
			/*
			 *  A free transaction-id has been discovered.
			 */
			mask = 1;
			for (j = 1; j <= PRECISION_ULONG; j++) {
				if (bitmap & mask) {
					/*
					 *  We have located a free
					 *  transaction-id.  Allocate it.
					 */
					mask = ~mask;
					freetransact [i] &= mask;
					return (i * PRECISION_ULONG + j);
				}
				mask <<= 1;
			}
		}
	}
	return (0);			/* No free transaction-ids */
}


/*** tid_deallocate
 *
 *
 *	void tid_deallocate():
 *	Deallocate a transaction-id.
 */
void tid_deallocate (trans_id)
register int trans_id;			/* Transaction-id */
{
	void tid_initialize();		/* Initialize the state of this */
					/* module */

	register int i;			/* Index var */
	register unsigned long mask;	/* Mask */

	if (!initialized) tid_initialize();
	if (trans_id > 0 && trans_id <= MAX_TRANSACTS) {
		/*
		 *  The transaction-id is valid.
		 */
		trans_id--;
		i = trans_id / PRECISION_ULONG;
		mask = (1L << (trans_id % PRECISION_ULONG));
		freetransact [i] |= mask;	/* Mark as free */
	}
}


/*** tid_initialize
 *
 *
 *	void tid_initialize():
 *	Initialize the state of this module [nqs_tid.c].
 */
static void tid_initialize ()
{
	register int i;
	register unsigned long freemask;
	register unsigned long buildmask;

	initialized = 1;		/* Set initialized flag */
	/*
	 *  Just in case PRECISION_ULONG has been configured as LESS than
	 *  the number of actual bits present in an unsigned long, we
	 *  carefully build the free mask to only set the bits indicated
	 *  by PRECISION_ULONG, rather than just saying ~0.
	 *
	 *  If we don't do this, and PRECISION_ULONG has indeed been
	 *  designated a value LESS than the real number of bits present
	 *  in an unsigned long, then the tid_allocate() procedure in
	 *  this module will break!  Thus, we're simply being paranoid.
	 */
	buildmask = 1;
	freemask = buildmask;
	for (i = 1; i < PRECISION_ULONG; i++) {
		buildmask <<= 1;
		freemask |= buildmask;
	}
	/*
	 *  Make sure that PRECISION_ULONG is not GREATER than the
	 *  number of bits present in an unsigned long.
	 */
	if (buildmask == 0) {
		printf ("F$PRECISION_ULONG exceeds actual value\n");
		printf ("F$in nqs_tid.c\n");
		nqs_abort();
	}
	/*
	 *  Set the free transaction allocation bitmap.
	 */
	i = 0;
	while (i < TRANSACTARRAYSIZE-1) {
		freetransact [i++] = freemask;
	}
	freemask >>= (TRANSACTARRAYSIZE * PRECISION_ULONG - MAX_TRANSACTS);
	freetransact [i] = freemask;
}
