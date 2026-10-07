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
 
/*++ transactcc.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/transactcc.h,v $
 *
 * DESCRIPTION:
 *
 *
 *	NQS transaction completion codes file (TCMx_).
 *
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *
 *
 *	    If you ADD, DELETE, or CHANGE the TCMx_ codes in
 *	    this module IN ANY WAY, then you MUST change:
 *
 *			../lib/tcmmsgs.c, and
 *			../lib/tcmident.c
 *
 *	    as necessary!!!!
 *
 *	    Now, raise your right paw, and repeat after me:
 *
 *		I, <your-name>, will NOT change the
 *		numerical value of any transaction
 *		completion codes defined in this file.
 *
 *	    It is critical to understand that many of the
 *	    transaction codes defined below, are sent FROM
 *	    NQS server processes TO NQS client processes
 *	    running on completely DIFFERENT machines.
 *
 *	    As time passes, the probability also increases
 *	    that there will be DIFFERENT versions of NQS
 *	    running at different sites.
 *
 *	    In all cases however, EVERY NQS implementation
 *	    MUST use these transaction completion codes as
 *	    defined below, and MUST NOT change them, otherwise
 *	    NQS will have to be renamed 'QS' with the 'N' for
 *	    networking removed.
 *
 *	    Don't change this file, unless you also plan to
 *	    change every other version of NQS in the world.
 *
 *	    Note however that the brave software developer
 *	    MAY elect to ADD new transaction code types.
 *	    Remember though that the unmodified versions of
 *	    NQS will not understand.  Furthermore, once you
 *	    add a transaction completion code, you can never
 *	    remove it.  Adding a new transaction completion
 *	    code is alot like adding a new system call to
 *	    an operating system.  Once added, it cannot be
 *	    removed.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:44:27 $ $State: Exp $)
 * $Log: transactcc.h,v $
 * Revision 1.3  1994/11/19  02:44:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:18:30  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:02  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:45:26  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*
 *
 *
 *	All NQS transaction completion codes must fit in a long
 *	integer.  Each NQS transaction completion code is broken
 *	down into five (5) principal sections:
 *
 *	.--------------------------------------------------------------.
 *	| 0 | 21-bits of additional information | 1 | P | 8-bit reason |
 *	`--------------------------------------------------------------'
 *
 *	The high-order bit (bit 31) must be zero, to make the
 *	completion code positive for "32-bit machines."
 *
 *	Bit 9 must be set to indicate that the completion code
 *	is a TRANSACTION completion code.
 *
 *	Bit 8 is the 'P' bit.  If set, then this transaction
 *	completion code identifies a condition at the network (P)eer
 *	(the remote server host) for a transaction that is occurring
 *	across a network connection between a client and server
 *	process pair.  Otherwise, this transaction code refers
 *	to a condition at the local host.
 *
 *	The 8-bit reason field describes what generally happened
 *	with the transaction (success, failure, and why).
 *
 *	The 21-information bits are defined in informcc.h, and
 *	have a meaning dependent upon the 8-bit reason.
 *
 *
 *
 *	NQS transaction completion codes (TCMx_) are as follows:
 */
#define	TCML_ACCESSDEN	01000L
/*
 *			The transaction cannot be performed, because
 *			some permission condition at the local machine
 *			denies access to the user associated with the
 *			transaction (i.e. a queue denies access to a
 *			user invoking a local instance of the 'qsub'
 *			command).
 *
 *			If access is denied because of the lack of an
 *			account database authorization, or the transac-
 *			tion site does not recognize an account mapping
 *			for the associated user, then TCMx_NOACCAUTH
 *			should be returned instead.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_ACCESSDEN	01400L
/*
 *			The transaction cannot be performed, because
 *			some permission condition at the peer machine
 *			denies access to the user associated with the
 *			transaction (i.e. a queue denies access to the
 *			owner of a request that we are trying to queue
 *			remotely).
 *
 *			If access is denied because of the lack of an
 *			account database authorization, or the transac-
 *			tion site does not recognize an account mapping
 *			for the associated user, then TCMx_NOACCAUTH
 *			should be returned instead.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_ALREADACC	01001L
/*
 *			The transaction specified the addition of a
 *			user or group-id to a queue access set, and the
 *			user or group-id was already present in the
 *			queue access set.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_ALREADEXI	01002L
/*
 *			The transaction specified the addition of a
 *			set element, and the element was already
 *			present in the set.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_BADCDTFIL	01003L
/*
 *			The transaction involved an operation upon
 *			request control and/or data file(s), and the
 *			request control and/or data file(s) were
 *			found to be corrupt at the local machine.
 *			
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_BADCDTFIL	01403L
/*
 *			The transaction involved an operation upon
 *			request control and/or data file(s), and the
 *			request control and/or data file(s) were
 *			found to be corrupt at the peer machine.
 *			
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_CLIMIDUNKN	01404L
/*
 *			This completion code is returned from a remote
 *			NQS transaction process when no machine-id can
 *			be determined for the client's network address
 *			by examining the network tables at the transaction
 *			peer machine.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO equivalent
 *			of TCML_CLIMIDUNKN.
 *
 *			The transaction fails.  The remote peer should be
 *			marked as failed, and the transaction should NOT
 *			be retried.
 *
 *
 */
#define	TCML_COMPLETE	01005L
/*
 *			The transaction has concluded successfully.
 *
 *			This transaction completion code is of the "generic"
 *			variety, and can be used as the final result code
 *			from the local machine for any transaction.
 *
 *			If a more specific transaction code exists for the
 *			completed transaction (i.e. TCMx_SUBMITTED), then
 *			the more specific completion code should be used.
 *
 *			No information bits are present.
 *
 *			The transaction has succeeded.
 *
 *
 */
#define	TCMP_COMPLETE	01405L
/*
 *			The transaction has concluded successfully.
 *
 *			This transaction completion code is of the "generic"
 *			variety, and can be used as the final result code
 *			from the peer machine for any transaction.
 *
 *			If a more specific transaction code exists for the
 *			completed transaction (i.e. TCMx_SUBMITTED), then
 *			the more specific completion code should be used.
 *
 *			No information bits are present.
 *
 *			The transaction has succeeded.
 *
 *
 */
#define	TCMP_CONNBROKEN	01406L
/*
 *			The transaction failed because the network
 *			connection established for the transaction was
 *			severed for some unknown reason.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO local equivalent
 *			of TCML_CONNBROKEN.
 *
 *			The transaction fails.  The transaction should
 *			be retried at a later time.
 *
 *
 */
#define	TCMP_CONNTIMOUT	01407L
/*
 *			The transaction failed because of a timeout waiting
 *			for the transaction peer to respond.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO local equivalent
 *			of TCML_CONNTIMOUT.
 *
 *			The transaction fails.  The transaction should
 *			be retried at a later time.
 *
 *
 */
#define	TCMP_CONTINUE	01410L
/*
 *			The transaction has begun successfully (i.e. access
 *			granted, etc.), or else the previous subtransaction
 *			of the transaction completed successfully.  The
 *			server in the client/server paradigm is ready to
 *			perform the next subtransaction, as directed by the
 *			client.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO local equivalent
 *			of TCML_CONTINUE.
 *
 *			The transaction continues.
 *
 *
 */
#define	TCML_CPUALRESVD	01011L
/*
 *			The transaction specified the allocation of a
 *			local CPU to a local batch queue, and the CPU
 *			is already allocated to another local batch
 *			queue.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_DEVACTIVE	01012L
/*
 *			The transaction specified the deletion of an
 *			active device.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_DEVENABLE	01013L
/*
 *			The transaction specified the deletion of an
 *			enabled device.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_EACCESS	01014L
/*
 *			The transaction involved some file operation that
 *			could not be performed because the protection
 *			mechanisms at the local machine denied access
 *			to the associated user.
 *
 *			This transaction error code is remarkably similar
 *			to TCMx_EPERM, but there are situations in which
 *			the two codes identify noticeably different
 *			conditions.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_EACCESS	01414L
/*
 *			The transaction involved some file operation that
 *			could not be performed because the protection
 *			mechanisms at the peer machine denied access
 *			to the associated user.
 *
 *			This transaction error code is remarkably similar
 *			to TCMx_EPERM, but there are situations in which
 *			the two codes identify noticeably different
 *			conditions.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_EFBIG	01015L
/*
 *			The transaction involved some file operation that
 *			would have increased the size of the file beyond
 *			the maximum as supported at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_EFBIG	01415L
/*
 *			The transaction involved some file operation that
 *			would have increased the size of the file beyond
 *			the maximum as supported at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_EISDIR	01016L
/*
 *			The transaction involved an illegal operation on
 *			a directory file as determined by the protection
 *			mechanisms at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_EISDIR	01416L
/*
 *			The transaction involved an illegal operation on
 *			a directory file as determined by the protection
 *			mechanisms at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_ELOOP	01017L
/*
 *			The transaction required the translation of symbolic
 *			links at the local machine, and the symbolic link
 *			translation limit was exceeded at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_ELOOP	01417L
/*
 *			The transaction required the translation of symbolic
 *			links at the peer machine, and the symbolic link
 *			translation limit was exceeded at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_ENFILE	01020L
/*
 *			The transaction cannot be attempted or completed
 *			because of a file descriptor shortage at the local
 *			machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_ENFILE	01420L
/*
 *			The transaction cannot be attempted or completed
 *			because of a file descriptor shortage at the peer
 *			machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_ENOBUFS	01021L
/*
 *			The transaction cannot be attempted because there
 *			are not enough buffers available at the local
 *			machine to establish the network connection required
 *			for the transaction at this time.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_ENOBUFS	01421L
/*
 *			The transaction cannot be attempted because there
 *			are not enough buffers available at the peer
 *			machine to establish the network connection required
 *			for the transaction at this time.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_ENOENT	01022L
/*
 *			The transaction involved a file operation in which
 *			some element of the file path did not exist at the
 *			local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_ENOENT	01422L
/*
 *			The transaction involved a file operation in which
 *			some element of the file path did not exist at the
 *			peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_ENOMEM	01023L
/*
 *			The transaction cannot be performed because there
 *			is not sufficient memory or swap space at the
 *			local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_ENOMEM	01423L
/*
 *			The transaction cannot be performed because there
 *			is not sufficient memory or swap space at the
 *			peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_ENOSPC	01024L
/*
 *			The transaction cannot be completed or performed
 *			because of some file system resource shortage at
 *			the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_ENOSPC	01424L
/*
 *			The transaction cannot be completed or performed
 *			because of some file system resource shortage at
 *			the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_ENOTDIR	01025L
/*
 *			The transaction involved a file operation in which
 *			some element in the file path was not a directory
 *			at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_ENOTDIR	01425L
/*
 *			The transaction involved a file operation in which
 *			some element in the file path was not a directory
 *			at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_ENXIO	01026L
/*
 *			The transaction involved some operation on a special
 *			file (i.e. UNIX subdevice), and the subdevice does
 *			not exist, or some other error condition exists with
 *			the subdevice (i.e. tape drive not on line, disk
 *			pack not loaded in drive, beyond the limits of the
 *			device, etc.), at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_ENXIO	01426L
/*
 *			The transaction involved some operation on a special
 *			file (i.e. UNIX subdevice), and the subdevice does
 *			not exist, or some other error condition exists with
 *			the subdevice (i.e. tape drive not on line, disk
 *			pack not loaded in drive, beyond the limits of the
 *			device, etc.), at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_EPERM	01027L
/*
 *			The transaction involved some operation that
 *			could not be performed because the protection
 *			mechanisms at the local machine denied permission
 *			to the associated user.
 *
 *			This transaction error code is remarkably similar
 *			to TCMx_EACCESS, but there are situations in which
 *			the two codes identify noticeably different
 *			conditions.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_EPERM	01427L
/*
 *			The transaction involved some operation that
 *			could not be performed because the protection
 *			mechanisms at the peer machine denied permission
 *			to the associated user.
 *
 *			This transaction error code is remarkably similar
 *			to TCMx_EACCESS, but there are situations in which
 *			the two codes identify noticeably different
 *			conditions.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_EPIPE	01030L
/*
 *			The transaction involved some file operation on a
 *			UNIX pipe that could not be performed because no
 *			other process at the local machine had the other
 *			end of the pipe open.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_EPIPE	01430L
/*
 *			The transaction involved some file operation on a
 *			UNIX pipe that could not be performed because no
 *			other process at the peer machine had the other
 *			end of the pipe open.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_EROFS	01031L
/*
 *			The transaction involved some file operation that
 *			would have altered some portion of a read-only
 *			file system at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_EROFS	01431L
/*
 *			The transaction involved some file operation that
 *			would have altered some portion of a read-only
 *			file system at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_ERRORRETRY	01032L
/*
 *			The transaction cannot be concluded successfully
 *			because of some error condition at the local
 *			machine that MAY go away in the future.
 *
 *			This transaction completion code is of the "generic"
 *			variety, and can be used as the final result code
 *			of any transaction.
 *
 *			If a more specific transaction code exists to
 *			describe the error condition, then the more specific
 *			transaction code should be returned instead.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *			NOTE:	An explanatory text message MUST appear
 *				as the next element in any message packet
 *				returning this completion code.
 *
 *
 */
#define	TCMP_ERRORRETRY	01432L
/*
 *			The transaction cannot be concluded successfully
 *			because of some error condition at the peer
 *			machine that MAY go away in the future.
 *
 *			This transaction completion code is of the "generic"
 *			variety, and can be used as the final result code
 *			of any transaction.
 *
 *			If a more specific transaction code exists to
 *			describe the error condition, then the more specific
 *			transaction code should be returned instead.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *			NOTE:	An explanatory text message MUST appear
 *				as the next element in any message packet
 *				returning this completion code.
 *
 *
 */
#define	TCML_ETIMEDOUT	01033L
/*
 *			The transaction cannot be completed.  A connect()
 *			request on the local machine failed because the
 *			connected party did not properly respond after
 *			a period of time.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_ETXTBSY	01034L
/*
 *			The transaction cannot be completed because the
 *			involved file operation dealt with a busy text
 *			file in circumstances under which such action is
 *			prohibited at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_ETXTBSY	01434L
/*
 *			The transaction cannot be completed because the
 *			involved file operation dealt with a busy text
 *			file in circumstances under which such action is
 *			prohibited at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_FATALABORT	01035L
/*
 *			The transaction cannot be concluded successfully
 *			because of some error condition that will NOT go
 *			away in the forseeable future at the local machine.
 *
 *			This transaction completion code is of the "generic"
 *			variety, and can be used as the final result code
 *			of any transaction.
 *
 *			If a more specific transaction code exists to
 *			describe the error condition, then the more specific
 *			transaction code should be returned instead.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *			NOTE:	An explanatory text message MUST appear
 *				as the next element in the message packet
 *				returning this completion code.
 *
 *
 */
#define	TCMP_FATALABORT	01435L
/*
 *			The transaction cannot be concluded successfully
 *			because of some error condition that will NOT go
 *			away in the forseeable future at the peer machine.
 *
 *			This transaction completion code is of the "generic"
 *			variety, and can be used as the final result code
 *			of any transaction.
 *
 *			If a more specific transaction code exists to
 *			describe the error condition, then the more specific
 *			transaction code should be returned instead.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *			NOTE:	An explanatory text message MUST appear
 *				as the next element in the message packet
 *				returning this completion code.
 *
 *
 */
#define	TCML_FIXBYNQS	01036L
/*
 *			The transaction specified the enforcement of a
 *			quota limit value for a batch queue which cannot
 *			be enforced by the local machine.  The quota
 *			limit has instead been brought to within the limits
 *			enforceable at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction is successful.
 *
 *
 */
#define	TCML_GRANFATHER	01037L
/*
 *			The transaction modified a batch queue quota limit
 *			to below the limit set for one or more previously
 *			queued batch request limits.
 *
 *			No information bits are present.
 *
 *			The transaction is successful.
 *
 *
 */
#define	TCML_INSHUTDOWN	01040L
/*
 *			The transaction was a "sense NQS state" transaction,
 *			and the local NQS daemon on the local machine is
 *			in the process of shutting down.
 *
 *			No information bits are present.
 *
 *
 */
#define	TCML_INSQUESPA	01041L
/*
 *			The transaction failed because of insufficient
 *			queue space at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_INSQUESPA	01441L
/*
 *			The transaction failed because of insufficient
 *			queue space at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_INSUFFMEM	01042L
/*
 *			The transaction cannot complete because there is
 *			insufficient memory at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_INSUFFPRV	01043L
/*
 *			The transaction cannot be performed by the
 *			associated user is not sufficiently privileged
 *			by NQS.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_INTERNERR	01044L
/*
 *			The transaction could not be completed because of
 *			an internal error at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_INTERNERR	01444L
/*
 *			The transaction could not be completed because of
 *			an internal error at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_LOGFILERR	01045L
/*
 *			The transaction designated the creation or opening
 *			of a new NQS log file, and the open/create failed.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCMP_MAXNETCONN	01446L
/*
 *			The transaction cannot begin because there are
 *			already too many ongoing network transactions at
 *			the peer machine.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO local equivalent
 *			of TCML_MAXNETCONN.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_MAXQDESTS	01047L
/*
 *			The transaction specified the addition of a new
 *			queue destination for a pipe queue, that would
 *			otherwise exceed the maximum cardinality of a
 *			destination set for a single pipe queue.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCMP_MIDCONFLCT	01450L
/*
 *			The transaction cannot begin because the remote
 *			machine (the server machine), and the local machine
 *			(the client machine), disagree on their respective
 *			machine-id values.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO local equivalent
 *			of TCML_MIDCONFLCT.
 *
 *			The transaction fails.  The remote site should be
 *			marked as failed, and the transaction should NOT
 *			be retried.
 *
 *
 */
#define	TCML_NETDBERR	01051L
/*
 *			This completion code is returned when an error,
 *			or some inconsistency is detected while a local
 *			client process accesses the network database on
 *			the client machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_NETDBERR	01451L
/*
 *			This completion code is returned when an error,
 *			or some inconsistency is detected while a server
 *			process accesses the network database on the server
 *			peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NETNOTSUPP	01052L
/*
 *			The transaction cannot begin because the local
 *			host implementation of NQS does not support the
 *			networking implementation required to perform the
 *			transaction.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO peer equivalent
 *			of TCMP_NETNOTSUPP.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_NETPASSWD	01453L
/*
 *			The transaction cannot begin because the client
 *			has failed to supply the proper NQS network server
 *			password to the remote server peer machine.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO local equivalent
 *			of TCML_NETPASSWD.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NOACCAUTH	01054L
/*
 *			The transaction cannot be performed, because
 *			there is no account database authorization for
 *			the user associated with the transaction at
 *			the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_NOACCAUTH	01454L
/*
 *			The transaction cannot be performed, because
 *			there is no account database authorization for
 *			the user associated with the transaction at
 *			the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NOACCNOW	01055L
/*
 *			The transaction specified the deletion of a
 *			user or group-id from a queue access set, and
 *			the specified user or group-id was not previously
 *			present in the set.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOESTABLSH	01056L
/*
 *			The transaction could not be performed because
 *			the connection between the client transaction
 *			process and the local NQS daemon at the local
 *			machine, could not be established.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_NOESTABLSH	01456L
/*
 *			The transaction could not be performed because
 *			the connection between the transaction server and
 *			peer NQS daemon at the peer machine, could not be
 *			established.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_NOLOCALDAE	01057L
/*
 *			The transaction cannot be performed because the
 *			NQS daemon at the local machine is not running.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_NOLOCALDAE	01457L
/*
 *			The transaction cannot be performed because the
 *			NQS daemon at the peer machine is not running.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_NOMOREPROC	01060L
/*
 *			This completion code is returned when there are
 *			not enough processes available on the local host
 *			to perform the transaction.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_NOMOREPROC	01460L
/*
 *			This completion code is returned when there are
 *			not enough processes available on the remote
 *			peer machine to perform the transaction.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_NONETDAE	01461L
/*
 *			The transaction cannot be performed because the
 *			NQS netdaemon at the peer machine is not running.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_NONSECPORT	01462L
/*
 *			The transaction cannot begin because the client
 *			process has connected to the remote NQS network
 *			daemon process using a non-secure port.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO local equivalent
 *			of TCML_NONSECPORT.
 *
 *			The transaction fails.  The client server should
 *			be identified as failed, and the transaction
 *			should be retried at a later time, when the
 *			client server program is repaired.
 *
 *
 */
#define	TCML_NOPORTAVAI	01063L
/*
 *			The transaction cannot begin because no network
 *			port is available with which to perform the network
 *			operations required by the transaction at the local
 *			machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_NOSUCHACC	01064L
/*
 *			The transaction referred to a non-existent account
 *			of the local machine, where the account must exist
 *			in order for the transaction to complete successfully.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHCPU	01065L
/*
 *			The transaction referred to a non-existent CPU
 *			at the local machine, where the CPU must exist for
 *			the transaction to complete successfully.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHDES	01066L
/*
 *			The transaction referred to a non-existent queue
 *			destination for a pipe queue at the local machine,
 *			where the destination must exist for the transaction
 *			to complete successfully.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHDEV	01067L
/*
 *			The transaction referred to a non-existent device
 *			for a local device queue, where the device must
 *			exist for the transaction to complete successfully.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHFORM	01070L
/*
 *			The transaction refers to non-existent NQS device
 *			forms at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_NOSUCHFORM	01470L
/*
 *			The transaction refers to non-existent NQS device
 *			forms at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NOSUCHGRP	01071L
/*
 *			The transaction refers to a non-existent group at
 *			the local machine, where the group must exist in
 *			order for the transaction to complete successfully.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHMAC	01072L
/*
 *			The transaction refers to a machine that is not
 *			known at the local host, where the machine name
 *			must be known to the local host in order for the
 *			transaction to complete successfully.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHMAN	01073L
/*
 *			The transaction refers to a non-existent NQS
 *			manager or operator account, where the account
 *			must have been previously defined as an NQS
 *			manager or operator in order for the request
 *			to complete successfully.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHMAP	01074L
/*
 *			The transaction refers to a non-existent queue/
 *			device mapping, where the mapping must exist in
 *			order for the transaction to complete successfully.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHQUE	01075L
/*
 *			The transaction involved an operation referring
 *			to a non-existent queue at the local machine, and
 *			could therefore not be performed.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_NOSUCHQUE	01475L
/*
 *			The transaction involved an operation referring
 *			to a non-existent queue at the peer machine, and
 *			could therefore not be performed.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NOSUCHQUO	01076L
/*
 *			The transaction specified the setting of a batch
 *			queue quota limit, where the quota limit type
 *			in question, cannot be enforced at the local
 *			machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_NOSUCHREQ	01077L
/*
 *			The transaction could not be performed because
 *			it references a request that does not exist at
 *			the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_NOSUCHREQ	01477L
/*
 *			The transaction could not be performed because
 *			it references a request that does not exist at
 *			the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NOSUCHSIG	01100L
/*
 *			The transaction could not be performed because
 *			it refers to a signal that does not exist at the
 *			local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_NOSUCHSIG	01500L
/*
 *			The transaction could not be performed because
 *			it refers to a signal that does not exist at the
 *			peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NOTREQOWN	01101L
/*
 *			The transaction could not be performed because it
 *			specified an operation upon a request when the
 *			user associated with the transaction was NOT the
 *			request owner at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_NOTREQOWN	01501L
/*
 *			The transaction could not be performed because it
 *			specified an operation upon a request when the
 *			user associated with the transaction was NOT the
 *			request owner at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_PATHLEN	01102L
/*
 *			This completion code is returned from a local
 *			transaction process, when reserving a queue slot
 *			on the local machine for a new batch request, and
 *			the resolved stdout or stderr pathname of the
 *			request exceeds the maximum request path length
 *			supported by the NQS implementation at the local
 *			machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *			The associated request must be deleted.
 *
 *
 */
#define	TCMP_PATHLEN	01502L
/*
 *			This completion code is returned from a remote
 *			transaction process, when reserving a queue slot
 *			on the remote machine for a batch request that
 *			is being queued remotely, and the resolved stdout
 *			or stderr pathname of the request exceeds the
 *			maximum request path length supported by the NQS
 *			implementation at the remote peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *			The associated request must be deleted.
 *
 *
 */
#define	TCML_PEERARRIVE	01103L
/*
 *			The transaction specified the deletion of a request
 *			not presently in transit between two machines in
 *			the network, and the designated request is presently
 *			in the arriving state (to be delivered or is being
 *			delivered from a remote machine).
 *
 *			No information bits are present.
 *
 *			The transaction fails, and the coordinator process
 *			attempting the transaction, must now retry the
 *			delete operation using the networked form of the
 *			transaction.
 *
 *
 */
#define	TCML_PEERDEPART	01104L
/*
 *			The transaction specified the deletion of a request
 *			not presently in transit between two machines in
 *			the network, and the designated request is presently
 *			in the departing state (to be delivered or is being
 *			delivered to a remote machine).
 *
 *			No information bits are present.
 *
 *			The transaction fails, and the coordinator process
 *			attempting the transaction, must now retry the
 *			delete operation using the networked form of the
 *			transaction.
 *
 *
 */
#define	TCML_PLOCKFAIL	01105L
/*
 *			The transaction specified that the local NQS
 *			daemon should lock itself into the memory of the
 *			local machine, and the plock() call failed.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_PROTOFAIL	01106L
/*
 *			The transaction could not be completed because of
 *			an NQS message protocol failure on the local
 *			machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_PROTOFAIL	01506L
/*
 *			The transaction could not be completed because of
 *			an NQS message protocol failure between a local
 *			client transaction process and a remote server
 *			transaction process, or a protocol failure between
 *			the remote server process and the network daemon
 *			at the remote machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_QUEDISABL	01107L
/*
 *			The transaction involved an operation that could
 *			not be completed because the referenced queue was
 *			disabled at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCMP_QUEDISABL	01507L
/*
 *			The transaction involved an operation that could
 *			not be completed because the referenced queue was
 *			disabled at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The transaction should be
 *			retried at a later time.
 *
 *
 */
#define	TCML_QUEENABLE	01110L
/*
 *			The transaction specified the deletion of a local
 *			queue which is enabled.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_QUEHASREQ	01111L
/*
 *			The transaction specified the deletion of a local
 *			queue which contains requests.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_QUOTALIMIT	01112L
/*
 *			The transaction involved the queueing of a batch
 *			request, or the modification of some batch request
 *			limit(s), such that the successful completion of
 *			the transaction would have caused the batch request
 *			to have quota limits greater than one or more
 *			corresponding quota limits for the containing
 *			batch queue at the local machine.
 *			
 *			Quota information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_QUOTALIMIT	01512L
/*
 *			The transaction involved the queueing of a batch
 *			request, or the modification of some batch request
 *			limit(s), such that the successful completion of
 *			the transaction would have caused the batch request
 *			to have quota limits greater than one or more
 *			corresponding quota limits for the containing
 *			batch queue at the peer machine.
 *			
 *			Quota information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_REQCOLLIDE	01113L
/*
 *			The transaction involved the queueing of request,
 *			and another request with the same request-id
 *			already existed at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *			The associated request must be deleted.
 *
 *
 */
#define	TCMP_REQCOLLIDE	01513L
/*
 *			The transaction involved the queueing of request,
 *			and another request with the same request-id
 *			already existed at the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *			The associated request must be deleted.
 *
 *
 */
#define	TCML_REQDELETE	01114L
/*
 *			The transaction specified the deletion of a specific
 *			request at the local machine, and the request was
 *			deleted successfully.
 *
 *			No information bits are present.
 *
 *			The transaction succeeded.
 *
 *
 */
#define	TCMP_REQDELETE	01514L
/*
 *			The transaction specified the deletion of a specific
 *			request at the peer machine, and the request was
 *			deleted successfully.
 *
 *			No information bits are present.
 *
 *			The transaction succeeded.
 *
 *
 */
#define	TCML_REQRUNNING	01115L
/*
 *			The transaction specified the deletion of a specific
 *			request at the local machine, but the request was
 *			already running, and the transaction did not specify
 *			a signal.  The request is not deleted and continues
 *			execution.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_REQRUNNING	01515L
/*
 *			The transaction specified the deletion of a specific
 *			request at the peer machine, but the request was
 *			already running, and the transaction did not specify
 *			a signal.  The request is not deleted and continues
 *			execution.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_REQSIGNAL	01116L
/*
 *			The transaction specified the deletion or signalling
 *			of a specific request at the local machine, and
 *			the request has been signalled successfully.
 *
 *			No information bits are present.
 *
 *			The transaction succeeded.
 *
 *
 */
#define	TCMP_REQSIGNAL	01516L
/*
 *			The transaction specified the deletion or signalling
 *			of a specific request at the peer machine, and
 *			the request has been signalled successfully.
 *
 *			No information bits are present.
 *
 *			The transaction succeeded.
 *
 *
 */
#define	TCML_ROOTINDEL	01117L
/*
 *			The transaction specified the deletion of root
 *			as an NQS manager at the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCMP_RRFUNKNMID	01520L
/*
 *			The transaction involved the queueing of a request
 *			at a queue destination where the request referred
 *			to machine-ids not known to the transaction peer
 *			machine.
 *
 *			No information bits are present.
 *
 *			This transaction code has NO local equivalent
 *			of TCML_RRFUNKNMID.
 *
 *			The transaction fails (the request is not queued),
 *			and should not be retried.
 *
 *
 */
#define	TCML_SELMIDUNKN	01121L
/*
 *			The transaction could not be performed because
 *			the local client transaction process at the
 *			local machine was unable to determine the machine-id
 *			of the local machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *			The containing queue should be stopped, and the
 *			request requeued, if the transaction is on behalf
 *			of a previously queued NQS request.
 *
 *
 */
#define	TCMP_SELMIDUNKN	01521L
/*
 *			The transaction could not be performed because
 *			the local server transaction process at the
 *			peer machine was unable to determine the machine-id
 *			of the peer machine.
 *
 *			No information bits are present.
 *
 *			The transaction fails.  The remote site should be
 *			marked as failed, and the transaction should NOT
 *			be retried.
 *
 *
 */
#define	TCML_SELREFDES	01122L
/*
 *			The transaction specified the addition of a pipe
 *			queue destination that would have caused a self-
 *			referential pipe queue destination set.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_SHUTERROR	01123L
/*
 *			The transaction specified that the local NQS
 *			daemon at the local host be shutdown, and an
 *			error occurred in the shutdown process.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_SUBMITTED	01124L
/*
 *			The transaction involved the queueing of a request
 *			at the local machine, and the queueing transaction
 *			was successful.
 *
 *			Quota information bits are present.
 *
 *			The transaction succeeds.
 *
 *
 */
#define	TCMP_SUBMITTED	01524L
/*
 *			The transaction involved the queueing of a request
 *			at the peer machine, and the queueing transaction
 *			was successful.
 *
 *			Quota information bits are present.
 *
 *			The transaction succeeds.
 *
 *
 */
#define	TCML_TOOMANDEV	01125L
/*
 *			The transaction specified the creation of a new
 *			device that would otherwise exceed the maximum
 *			number of devices as supportable by the local
 *			NQS implementation.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_UNAFAILURE	01126L
/*
 *			An unanticipated and totally unexpected error
 *			condition occurred trying to process the transaction
 *			at the local machine.  This transaction completion
 *			code is of the "generic" variety, and can be used as
 *			the final result code for any local transaction
 *			process.
 *
 *			If a more specific transaction code exists, then
 *			the more specific code should be used to describe
 *			the failure condition.  If the error condition was
 *			not wholly unexpected, but equally fatal with no
 *			other transaction code matching the circumstances,
 *			then TCMx_FATALABORT should be used instead.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_UNAFAILURE	01526L
/*
 *			An unanticipated and totally unexpected error
 *			condition occurred trying to process the transaction
 *			at the peer machine.  This transaction completion
 *			code is of the "generic" variety, and can be used as
 *			the final result code for any peer transaction
 *			process.
 *
 *			If a more specific transaction code exists, then
 *			the more specific code should be used to describe
 *			the failure condition.  If the error condition was
 *			not wholly unexpected, but equally fatal with no
 *			other transaction code matching the circumstances,
 *			then TCMx_FATALABORT should be used instead.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define TCML_UNDEFINED	01127L
/*
 *			This transaction completion code should never
 *			be returned.  It does however act as a place-
 *			holder value in ../lib/mkreq.c to detect
 *			instances of where the local NQS daemon was
 *			unable to open the control file of the request
 *			when the request was being queued for the first
 *			time in a batch queue.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define TCMP_UNDEFINED	01527L
/*
 *			This transaction completion code should never
 *			be returned from a peer machine transaction
 *			process.  It indicates an error in the NQS
 *			communication protocol, or other disaster.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_UNRESTR	01130L
/*
 *			This transaction specified that a queue access
 *			set be made unrestricted, and the queue access
 *			set for the named queue was already unrestricted.
 *
 *			No information bits are present.
 *
 *			The transaction fails.
 *
 *
 */
#define	TCML_WROQUETYP	01131L
/*
 *			The transaction involved the queueing of a request,
 *			but the target queue on the local machine was of the
 *			wrong type for the request (i.e. submitting a batch
 *			request to a device queue, and vice-versa).
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCMP_WROQUETYP	01531L
/*
 *			The transaction involved the queueing of a request,
 *			but the target queue on the peer machine was of the
 *			wrong type for the request (i.e. submitting a batch
 *			request to a device queue, and vice-versa via a
 *			pipe queue).
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NOSUCHCOM	01132L
/*
 *			The transaction involved an operation referring
 *			to a non-existent queue complex at the local machine.
 *			The operation could therefore not be performed.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_TOOMANQUE	01133L
/*
 *			The transaction tried to add a queue to a queue
 *			complex which already had the maximum number of
 *			queues defined.  The operation could therefore not
 *			be performed.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_TOOMANCOM	01134L
/*
 *			The transaction tried to add a queue to a queue
 *			complex but the queue already had the maximum
 *			number of queue complexes allowed.  The operation
 *			could therefore not be performed.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_ALREADCOM	01135L
/*
 *			The transaction tried to add a queue to a queue
 *			complex but the queue was already a member of the queue
 *			complex.  The operation could therefore not be
 *			performed.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCML_NOTMEMCOM	01136L
/*
 *			The transaction tried to remove a queue from a queue
 *			complex but the queue was not a member of the queue
 *			complex.  The operation could therefore not be
 *			performed.
 *
 *			No information bits are present.
 *
 *			The transaction fails, and should not be retried.
 *
 *
 */
#define	TCM_MAXTCM	TCML_NOTMEMCOM | 0400L
				/* Maximum TCMx (see ../lib/tcmsgs.c */
					/* and ../lib/tcmident.c) */
