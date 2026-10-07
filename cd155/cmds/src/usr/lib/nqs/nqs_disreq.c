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
 
/*++ nqs_disreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_disreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Dispose of an NQS request or subrequest.  This can
 *	entail deleting the control and data files for the
 *	request (not a subrequest), as well as the memory-
 *	resident version of the request or subrequest structure,
 *	and associated transaction descriptor (not a subrequest).
 *	The Extreqcount global var is also updated as appropriate.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 30, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:53 $ $State: Exp $)
 * $Log: nqs_disreq.c,v $
 * Revision 1.3  1994/11/19  02:52:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:17  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:59  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:05:28  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* NQS external vars and dirs  */

extern char *asciierrno();		/* Return ASCII errno */
extern void free();			/* Release memory allocated from */
					/* heap */
extern void nqs_delrfs();		/* Delete all control and data */
					/* files associated with a request */
extern void tid_deallocate();		/* Deallocate the specified */
					/* transaction-id */
extern int tra_release();		/* Update the NQS database to */
					/* record that the specified */
					/* transaction descriptor is */
					/* now unallocated */

/*** nqs_disreq
 *
 *
 *	void nqs_disreq():
 *
 *	Dispose of an NQS request or subrequest.  This can
 *	entail deleting the control and data files for the
 *	request (not a subrequest), as well as the memory-
 *	resident version of the request or subrequest structure,
 *	and associated transaction descriptor (not a subrequest).
 *	The Extreqcount global var is also updated as appropriate.
 */
void nqs_disreq (request, deleteflag)
struct request *request;		/* Pointer to the request structure */
					/* to be disposed of */
short deleteflag;			/* Boolean request files are to */
					/* also be deleted flag */
{
	register int tid;		/* Transaction-id */

	/*
	 *  MOREHERE someday to delete child subrequest(s) as well.
	 */
	if ((request->status & RQF_SUBREQUEST) == 0) {
		/*
		 *  This request is not a subrequest.
		 */
		if (request->status & RQF_EXTERNAL) {
			/*
			 *  The request was queued from an external source.
			 *  Update the number of externally queued requests
			 *  as necessary.
			 */
			Extreqcount--;	/* One less externally queued */
		}			/* request */
		tid = request->v1.req.trans_id;
		/*
		 *  This request still has a transaction-id/descriptor
		 *  assigned to it.  Free up the transaction-id so that
		 *  it can be used by another request.
		 */
		tid_deallocate (tid);
		/*
		 *  Update the permanent memory NQS database to record
		 *  that the corresponding transaction descriptor is now
		 *  unallocated.
		 *
		 *  WARNING:  It is CRITICAL that the transaction-id/descr
		 *	      for a request be released PRIOR to actually
		 *	      deleting any files associated with the
		 *	      request.  This temporal sequence is
		 *	      CRITICAL and ESSENTIAL for the correct
		 *	      operation of nqs_rbuild.c.
		 */
		if (tra_release (tid) == -1) {
			/*
			 *  Something went wrong updating the database.
			 */
			printf ("E$Tra_release() error in nqs_disreq.\n");
			printf ("I$%s.\n", asciierrno());
			printf ("I$Transaction-id was: %1d.\n", tid);
			fflush (stdout);
		}
		if (deleteflag) {
			/*
			 *  The request is NOT being placed in the
			 *  failed directory for analysis.  The request
			 *  control and data files are to be deleted.
			 */
			nqs_delrfs (request->v1.req.orig_seqno,
				    request->v1.req.orig_mid,
				    request->v1.req.ndatafiles);
		}
	}
	free ((char *) request);	/* Delete the request structure */
}
