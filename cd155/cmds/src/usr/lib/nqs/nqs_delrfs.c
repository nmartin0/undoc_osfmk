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
 
/*++ nqs_delrfs.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_delrfs.c,v $
 *
 * DESCRIPTION:
 *
 *	Delete the control and data files for the specified
 *	request.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:51 $ $State: Exp $)
 * $Log: nqs_delrfs.c,v $
 * Revision 1.3  1994/11/19  02:52:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:11  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:57  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:05:11  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxdirs.h"			/* NQS global variables */

extern void pack6name();		/* Compute name on 6-bit alphabet */


/*** void nqs_delrfs()
 *
 *
 *	void nqs_delrfs():
 *	Delete the specified control and data request files.
 */
void nqs_delrfs (orig_seqno, orig_mid, ndatafiles)
register long orig_seqno;		/* Request seq# */
register mid_t orig_mid;		/* Request machine-id */
register short ndatafiles;		/* #of request data files */
{
	char path [MAX_PATHNAME+1];

	while (ndatafiles) {
		/*
		 *  Loop to delete data files first.
		 */
		--ndatafiles;
		pack6name (path, Nqs_data, (int) (orig_seqno % MAX_DATASUBDIRS),
			  (char *) 0, (long) orig_seqno, 5,
			  (long) orig_mid, 6, ndatafiles, 3);
		unlink (path);		/* Unlink request */
	}

	/*
	 *  Now, unlink the request control file.
	 */
	pack6name (path, Nqs_control, (int) (orig_seqno % MAX_CTRLSUBDIRS),
		  (char *) 0, (long) orig_seqno, 5, (long) orig_mid, 6, 0, 0);
	unlink (path);

	/*
	 *  Now, unlink log file if it exists.
	 */
	pack6name (path, Nqs_control, (int) (orig_seqno % MAX_CTRLSUBDIRS),
		  (char *) 0, (long) orig_seqno, 5, (long) orig_mid, 6, 0, 0);
	strcat (path,"l");
	unlink (path);
}
