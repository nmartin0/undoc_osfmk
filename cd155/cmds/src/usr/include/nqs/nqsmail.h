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
 
/*++ nqsmail.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqsmail.h,v $
 *
 * DESCRIPTION:
 *
 *	NQS mail suffix codes and mail file definitions.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 26, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:44:08 $ $State: Exp $)
 * $Log: nqsmail.h,v $
 * Revision 1.3  1994/11/19  02:44:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:18:02  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:49  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:44:27  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*
 *	Mail suffix codes.
 */
#define	MSX_ABORTED	0	/* "aborted" */
#define	MSX_ABORTSHUTDN	1	/* "aborted for NQS shutdown" */
#define	MSX_BEGINNING	2	/* "beginning" */
#define	MSX_DELETED	3	/* "deleted" */
#define	MSX_DELIVERED	4	/* "delivered" */
#define	MSX_ENDED	5	/* "ended */
#define	MSX_FAILED	6	/* "files being placed in NQS failed */
				/*  directory" */
#define	MSX_REQUEUED	7	/* "requeued" */
#define	MSX_RESTARTING	8	/* "restarting" */
#ifdef SDSC
#define MSX_TIMEOUT     9       /* "time exceeded" */
#define MSX_BADTSCHED   10	/* "bad tennis court schedule" */
#define	MSX_MAXMSX	MSX_BADTSCHED 
#else
#define	MSX_MAXMSX	MSX_RESTARTING
#endif
				/* Used in ../src/nqs_mai.c */


/*
 *	NQS mail requestlog structure.
 *
 *	This structure is passed to nqs_mai() for both batch and device
 *	requests, to report completion information.  However, this
 *	structure is not stored in permanent memory (i.e. disk), except
 *	for batch requests, which use this structure to preserve information
 *	across crashes and system shutdowns.
 *
 *	WARNING:
 *	    This structure must NOT be larger than ATOMICBLKSIZ bytes
 *	    in size (see ../h/nqs.h) for the crash recovery mechanisms
 *	    of NQS to properly work.  This restriction is verified in
 *	    ../src/nqs_ldconf.c.
 */
struct requestlog {
	short reqrcmknown;		/* Boolean request RCM code known */
	struct servermssg svm;		/* Request completion code and */
					/* server message */
	long outrcmknown;		/* Bit mask of known RCM_ */
					/* stageout event completion codes */
	long outrcm [MAX_OUSTAPERREQ+2];/* RCM_ codes for stageout events */
};
