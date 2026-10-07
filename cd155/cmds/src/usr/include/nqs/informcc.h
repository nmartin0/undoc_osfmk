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
 
/*++ informcc.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/informcc.h,v $
 *
 * DESCRIPTION:
 *
 *	NQS information completion flags file [RCI_, TCI_, XCI_].
 *
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *
 *
 *
 *	    Raise your right paw, and repeat after me:
 *
 *		I, <your-name>, will NOT change the
 *		numerical value of any existing
 *		information code symbol beginning
 *		with the prefix:
 *
 *			TCI_  or
 *			XCI_
 *
 *		as defined in this file.
 *
 *	    The TCI_ and XCI_ values defined in this file
 *	    are an integral part of the NQS transaction
 *	    code definitions.  NQS transaction codes are
 *	    used by (possibly different) versions of NQS
 *	    to exchange transaction completion code information
 *	    BETWEEN machines.
 *
 *	    As such, one cannot go willy nilly changing
 *	    transaction completion codes (see ../h/transactcc.h).
 *	    Since the TCI_ and XCI_ values below are part of
 *	    a transaction completion code, these definitions
 *	    cannot be changed either, UNLESS every version
 *	    of NQS on every machine is modified to reflect
 *	    the changes.
 *
 *	    Don't change the TCI_ or XCI_ definitions.  You'll
 *	    break every implementation of NQS, and then you'll
 *	    have to put them all back together.
 *
 *	    Note however that the RCI_ definitions below CAN
 *	    be changed without disaster, since they are only
 *	    used within the local machine.
 *
 *	    Be careful.
 *
 *
 *
 *	  NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE
 *	  NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE
 *
 *
 *	    If you change any definitions in this module, then
 *	    you must be careful to change (when necessary) the
 *	    numerous references throughout NQS to the symbols
 *	    defined in this file.
 *
 *
 *	  END OF NOTE *** END OF NOTE *** END OF NOTE *** END OF NOTE
 *	  END OF NOTE *** END OF NOTE *** END OF NOTE *** END OF NOTE
 *
 *
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	January 25, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:43:53 $ $State: Exp $)
 * $Log: informcc.h,v $
 * Revision 1.3  1994/11/19  02:43:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:17:34  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:36  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:42:37  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*
 *
 *
 *	This file describes the information flags that are valid
 *	for the 21-bits of information data in an NQS completion
 *	code.
 *
 *	All NQS request and transaction completion codes fit in
 *	a long integer.  Each NQS completion code is broken down
 *	into four (4) sections:
 *
 *	.----------------------------------------------------------.
 *	| 0 | 21-bits of additional information | T | 9-bit reason |
 *	`----------------------------------------------------------'
 *
 *	The highest-order bit must be zero, to make the value positive,
 *	for "32-bit machines."
 *
 *	The 'T' bit when set, indicates that the code is a TRANSACTION
 *	completion code.  Otherwise, the code is a REQUEST completion
 *	code.
 *
 *
 *
 *	Information field (XCI_) masks:
 */
#define	XCI_FULREA_MASK	000000001777L	/*Full reason mask */
#define	XCI_REASON_MASK	000000000777L	/*Reason bits */
#define	XCI_TRANSA_MASK	000000001000L	/*Transaction code bit mask */
#define	XCI_PEER_MASK	000000000400L	/*Transaction code from peer mask */
#define	XCI_INFORM_MASK	017777776000L	/*Information bits */
/*
 *
 *	Quota limit violation transaction information (TCI_) flags:
 */
#define	TCI_COPLIMEXC	000000002000L	/* Copy limit exceeded */
#define	TCI_PP_CFLEXC	000000004000L	/* Per-proc corefile limit too big */
#define	TCI_PP_CTLEXC	000000010000L	/* Per-proc CPU time limit too big */
#define	TCI_PP_DSLEXC	000000020000L	/* Per-proc dataseg limit too big */
#define	TCI_PP_MSLEXC	000000040000L	/* Per-proc memsize limit too big */
#define	TCI_PP_NELEXC	000000100000L	/* Per-proc nice value limit too big*/
#define	TCI_PP_PFLEXC	000000200000L	/* Per-proc permfile limit too big */
#define	TCI_PP_QFLEXC	000000400000L	/* Per-proc quick file limit too big */
#define	TCI_PP_SSLEXC	000001000000L	/* Per-proc stackseg limit too big */
#define	TCI_PP_TFLEXC	000002000000L	/* Per-proc tempfile limit too big */
#define	TCI_PP_WSLEXC	000004000000L	/* Per-proc workset quota too big */
#define	TCI_PRILIMEXC	000010000000L	/* Print file too large */
#define	TCI_PR_CTLEXC	000020000000L	/* Per-req CPU time limit too big */
#define TCI_PR_DRIEXC	000040000000L	/* Per-req tape drives limit too big */
#define	TCI_PR_MSLEXC	000100000000L	/* Per-req memsize limit too big */
#define	TCI_PR_NCPEXC	000200000000L	/* Per-req # of cpus limit too big */
#define	TCI_PR_PFLEXC	000400000000L	/* Per-req permfile limit too big */
#define	TCI_PR_QFLEXC	001000000000L	/* Per-req quick file limit too big */
#define	TCI_PR_TFLEXC	002000000000L	/* Per-req tempfile limit too big */
/*
 *
 *	Request completion code information (RCI_) flags:
 */
#define	RCI_ACCESSDEN	000000002000L	/* Access denied */
#define RCI_CLIMIDUNKN	000000004000L	/* Client mid unknown at peer */
#define	RCI_EFBIG	000000010000L	/* File size limit exceeded */
#define	RCI_FATALABORT	000000020000L	/* Fatal transaction error */
#define	RCI_MIDCONFLICT	000000040000L	/* Client/dest machine-id conflict */
#define	RCI_NETNOTSUPP	000000100000L	/* Networking not supported */
#define	RCI_NETPASSWD	000000200000L	/* Net password verification error */
#define	RCI_NOSUCHFORM	000000400000L	/* No such device forms */
#define	RCI_NOSUCHQUE	000001000000L	/* No such queue */
#define	RCI_PEERINTERR	000002000000L	/* Local NQS internal error at peer */
#define	RCI_PEERMIDUNKN	000004000000L	/* Local machine-id unknown at peer */
#define	RCI_PEERNETDB	000010000000L	/* Network database error at peer */
#define	RCI_PEERNOACATH	000020000000L	/* No account authorization at peer */
#define	RCI_PROTOFAIL	000040000000L	/* NQS protocol failure */
#define	RCI_QUOTALIMIT	000100000000L	/* Quota limits exceed maximums */
#define RCI_RRFUNKNMID	000200000000L	/* Request refers to unknown mid */
#define	RCI_WROQUETYP	000400000000L	/* Wrong queue type for request */
#define	RCI_UNAFAILURE	001000000000L	/* Unanticipated failure */
