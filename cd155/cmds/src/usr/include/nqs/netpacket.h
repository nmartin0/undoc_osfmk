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
 
/*++ netpacket.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/netpacket.h,v $
 *
 * DESCRIPTION:
 *
 *	NQS network packet definitions file.
 *
 *
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *
 *
 *	    ALL symbols starting with the characters:  NPK_
 *	    as defined in this file are TOXIC, and should be
 *	    left completely alone.
 *
 *	    Raise your right paw, and repeat after me:
 *
 *		I, <your-name>, will NOT change the
 *		numerical value of any symbol defined
 *		in this file starting with the
 *		characters:
 *
 *		    NPK_
 *
 *		I understand that all such symbols
 *		defined in this file denote NQS
 *		network packet types, or related
 *		network packet constants.
 *
 *		I furthermore promise NEVER to change
 *		the format of an NQS network packet.
 *
 *	    It is critical to understand that network packets
 *	    are exactly what they sound like.  They are
 *	    message packets which transmit information from
 *	    one NQS process to another NQS process on another
 *	    completely DIFFERENT machine.
 *
 *	    To be even more precise, network packets of the
 *	    NPK_ form are always sent FROM NQS client processes,
 *	    and tell remote NQS server processes what to do.
 *	    NQS remote server processes then return transaction
 *	    completion codes (see ../h/transactcc.h) telling
 *	    the client process the result of their orders.
 *
 *	    As time passes, the probability increases that
 *	    there will be DIFFERENT versions of NQS running
 *	    at different installations.
 *
 *	    In all cases however, EVERY NQS implementation
 *	    MUST use the SAME network packet types, and the
 *	    SAME network packet formats so that other remote
 *	    machines running other implementations of NQS
 *	    will be able to understand each other.  Otherwise,
 *	    NQS will have to be renamed 'QS' with the 'N' for
 *	    networking removed.
 *
 *	    Note however that the brave software developer
 *	    MAY elect to ADD new network packet types, which
 *	    other unmodified versions of NQS will not understand.
 *
 *	    Furthermore, if you ever add a network packet
 *	    type, you can never delete it.  Adding a new
 *	    network packet type is a lot like adding a new
 *	    system call to an operating system.  Once added,
 *	    it cannot be removed.
 *
 *	    Don't add new network packet types recklessly.
 *	    You'll have to live with your decisions forever.
 *
 *
 *
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	END OF WARNING *** END OF WARNING *** END OF WARNING *** END OF WARNING
 *	    
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 22, 1986.
 *
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:43:58 $ $State: Exp $)
 * $Log: netpacket.h,v $
 * Revision 1.3  1994/11/19  02:43:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:17:40  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:39  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:43:42  hender
 * Sterling version 4/22/87
 * 
 *
 */


#define	NPK_MAGIC1	0x4E515301	/* Magic# used in establish() */
					/* connect protocol */


/*
 *	NQS network packet types.
 *
 *	Batch request and device request packets.
 */
#define	NPK_QUEREQ	0		/* Remotely queue a request */
#define	NPK_DELREQ	1		/* Remotely delete a request */
					/* RESERVED for future use */
#define	NPK_COMMIT	2		/* Remotely commit some action, */
					/* such as the queueing of a request */

/*
 *	NQS local server connection packet.
 *
 *	This packet is sent by a local NQS client process to the local
 *	NQS network daemon, whenever a connection must be established
 *	from a local client process to the local NQS daemon.
 *
 *	This packet is used ONLY for UNIX implementations that do NOT
 *	support named-pipes (i.e. Berkeley 4.2 and 4.3).
 */
#define	NPK_SERVERCONN	90		/* Local server connect packet */

/*
 *	File system packets.
 *
 *	The following actions involving the file system are
 *	undertaken in such a way as to insure their failure
 *	if the user on whose behalf they are being executed
 *	lacks the necessary permissions.
 */
#define NPK_DONE	100		/* Marks the end of an iterative */
					/* conversation */
#define NPK_MKDIR	101		/* Make a directory */
					/* RESERVED for future use */
#define	NPK_REQFILE	102		/* Deliver a file for a previously */
					/* queued request */
#define	NPK_CPINFILE	103		/* Copy-in a file */
					/* RESERVED for future use */
#define	NPK_CPOUTFILE	104		/* Copy-out a file */
					/* RESERVED for future use */
#define	NPK_MVINFILE	105		/* Move-in a file */
					/* RESERVED for future use */
#define	NPK_MVOUTFILE	106		/* Move-out a file */


/*
 *	Status packets.
 */
#define	NPK_QDEV	200		/* Remote Qdev startup packet*/
					/* RESERVED for future use */
#define	NPK_QLIMIT	201		/* Remote Qlimit startup packet */
					/* RESERVED for future use */
#define	NPK_QMGR	202		/* Remote Qmgr startup packet */
					/* RESERVED for future use */
#define	NPK_QSTAT	203		/* Remote Qstat startup packet */
