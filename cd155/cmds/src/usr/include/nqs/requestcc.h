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
 
/*++ requestcc.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/requestcc.h,v $
 *
 * DESCRIPTION:
 *
 *
 *	NQS request completion codes file (RCM_).
 *
 *	WARNING NOTE *** WARNING NOTE *** WARNING NOTE *** WARNING NOTE
 *	WARNING NOTE *** WARNING NOTE *** WARNING NOTE *** WARNING NOTE
 *	WARNING NOTE *** WARNING NOTE *** WARNING NOTE *** WARNING NOTE
 *
 *
 *	    If you add, delete, or change any of the RCM_
 *	    codes in this file, then you MUST update
 *	    ../lib/rcmmsgs.c, and the valid[] array in
 *	    ../src/nqs_reqexi.c as necessary.
 *
 *
 *	END OF WARNING NOTE *** END OF WARNING NOTE *** END OF WARNING NOTE
 *	END OF WARNING NOTE *** END OF WARNING NOTE *** END OF WARNING NOTE
 *	END OF WARNING NOTE *** END OF WARNING NOTE *** END OF WARNING NOTE
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
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 02:44:17 $ $State: Exp $)
 * $Log: requestcc.h,v $
 * Revision 1.5  1994/11/19  02:44:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/02  22:58:36  mwan
 * Fixed PTS #7838 and #7858
 *
 *  Reviewer: jkearns
 *  Risk: L
 *  Benefit or PTS #: 7838, 7858
 *  Testing:
 *  Module(s):usr/include/nqs/requestcc.h, usr/include/nqs/buddy.h,
 *                    usr/ccs/lib/libnqs/rcmmsgs.c,
 *                    usr/lib/nqs/nqs_reqser.c, usr/lib/nqs/nqs_reqexi.c,
 *                    usr/lib/nqs/nqs_reqcom.c, usr/lib/nqs/nqs_spawn.c
 * 		   usr/lib/macs_rootp.c.
 *
 * Revision 1.3  1993/05/28  20:14:23  mwan
 * Added RCM_BADTSCHED
 *
 * Revision 1.2  1992/10/09  22:18:24  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:59  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:45:16  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*
 *
 *
 *	All NQS request completion codes must fit in a long
 *	integer.  Each NQS request completion code is broken
 *	down into four (4) principal sections:
 *
 *	.----------------------------------------------------------.
 *	| 0 | 21-bits of additional information | 0 | 9-bit reason |
 *	`----------------------------------------------------------'
 *
 *	The high-order bit (bit 31) must be zero, to make the
 *	completion code positive for "32-bit machines."
 *
 *	Bit 9 must be zero to indicate that the completion code
 *	is a REQUEST completion code.
 *
 *	The 9-bit reason field describes what generally happened
 *	with the request (success, failure, and why).
 *
 *	The 21-information bits are defined in informcc.h, and
 *	have a meaning dependent upon the 9-bit reason.
 *
 *
 * 
 *	Request completion codes and descriptions:
 *
 *	The server type applicability set for a given request completion
 *	code is denoted by [B,D,N,P] where:
 *
 *		B: code is valid for a batch shell process;
 *		D: code is valid for a device server;
 *		N: code is valid for a network queue server;
 *		P: code is valid for a pipe queue server;
 *
 *
 *	Thus, the set [B,D,P] implies that the corresponding completion
 *	code can be returned from a batch shell process, a device server,
 *	and a pipe queue server, but NOT from a network queue server.
 *
 *
 *
 *	NQS request completion codes (RCM_) are as follows:
 */
#define	RCM_2MANYENVARS	00000L
/*
 *	[B]		Too many environment variables existed to
 *			execute the batch request.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_2MANYSVARGS	00001L
/*
 *	[D,N,P]		Too many server arguments in server execve().
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, and the device or
 *			queue is stopped as appropriate.
 *
 *
 */
#define	RCM_ABORTED	00002L
/*
 *	[B,D]		A shell process, or device server was terminated
 *			by a signal, that did not have anything to
 *			do with NQS shutting down (unless the request
 *			does something wierd, like catching SIGTERM
 *			signals and then killing itself with some other
 *			signal besides SIGTERM or SIGKILL).
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_BADCDTFIL	00003L
/*
 *	[B,D,N,P]	An invalid request control file and/or data
 *			file was detected.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is placed in the failed directory
 *			on the local machine for later analysis.
 *
 *
 */
#define	RCM_BADSRVARG	00004L
/*
 *	[D,N,P]		Bad argument or environment variable given to
 *			server.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, and the device or
 *			queue is stopped as appropriate.
 *
 *
 */
#define	RCM_DELIVERED	00005L
/*
 *	[N]		The associated request as previously routed by
 *			a pipe queue server, has been successfully
 *			delivered to its destination.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			Internal state updates are made as required.
 *
 *
 */
#define	RCM_DELIVEREXP	00006L
/*
 *	[N]		The associated request as previously routed by
 *			a pipe queue server, has not been delivered to
 *			its destination.  The original completion code
 *			from the server was RCM_RETRYLATER, and the
 *			delivery expiration time on the request has been
 *			reached.
 *
 *			The original completion code is CONVERTED to
 *			this completion code when the expiration time
 *			has elapsed.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_DELIVERFAI	00007L
/*
 *	[N]		The associated request as previously routed by
 *			a pipe queue server, could not be delivered to
 *			its destination.  This completion code indicates
 *			a disastrous situation in which the request can
 *			never be delivered.
 *
 *			The information field of this return code MUST
 *			contain the TCM_ code that caused the failure
 *			(see ../lib/mergertcm.c).
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_DELIVERRETX	00010L
/*
 *	[N]		The associated request as previously routed by
 *			a pipe queue server, has not been delivered to
 *			its destination, and the retry limit for such a
 *			transaction has been reached.
 *
 *			The information field of this return code MUST
 *			contain the TCM_ code that caused the failure
 *			(see ../lib/mergertcm.c).
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_DEVOPEFAI	00011L
/*
 *	[D]		The device to handle the request could not be
 *			successfully opened.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, and the device is
 *			stopped.
 *
 *
 */
#define	RCM_ENFILERUN	00012L
/*
 *	[B,D,N,P]	The request or transaction cannot be handled
 *			because of a file descriptor shortage on the
 *			local machine.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, to be retried again at
 *			a later time.  All NQS queues on the local machine
 *			are stopped, to conserve remaining available file
 *			descriptors (if any).
 *
 *
 */
#define	RCM_ENOSPCRUN	00013L
/*
 *	[B,D,N,P]	The request cannot be handled because of
 *			insufficient file system space on the LOCAL
 *			machine.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, to be retried again at
 *			a later time.  All NQS queues on the local machine
 *			are stopped, to not place any additional file
 *			system space demands on the local machine.
 *
 *
 */
#define	RCM_EXECUTING	00014L
/*
 *	[]		Used in the internal message protocol between
 *			the server and shepherd processes when spawning
 *			an NQS batch request.
 *
 *			This request completion code must NEVER be
 *			returned.
 *
 *
 */
#define	RCM_EXITED	00015L
/*
 *	[B,D]		The batch, or device request server has exited
 *			normally.  In the case of device request servers,
 *			this implies that the request has completed
 *			successfully.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			All local information concerning the request is
 *			deleted.
 *
 *
 */
#define	RCM_INSUFFMEM	00016L
/*
 *	[B,D,N,P]	The server to handle the request could not be
 *			spawned because of insufficient memory or swap
 *			space.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, and all queues are
 *			stopped.
 *
 *
 */
#define	RCM_INTERRUPTED	00017L
/*
 *	[N,P]		The network queue or pipe queue server has broken
 *			off transaction processing because of an NQS
 *			shutdown.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued.
 *
 *
 */
#define	RCM_MIDUNKNOWN	00020L
/*
 *	[B,D,N,P]	The LOCAL batch shell process, LOCAL device server,
 *			LOCAL network queue server, or LOCAL pipe server
 *			cannot identify the machine that the request
 *			originated from.
 *
 *			When the request was originally accepted by the
 *			local system, the owner machine-id was known to
 *			the local system.  Since that time however, the
 *			local host tables have been changed, and the owner
 *			machine-id of the request is no longer recognized.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted.
 *
 *			WARNING:  This code must NEVER be returned by
 *				  pipe or network queue servers when their
 *				  corresponding remote server returns
 *				  TCMP_NOACCAUTH.  Instead, the proper
 *				  RCM_DELIVERFAI, RCM_ROUTEFAI, or
 *				  RCM_STAGEOUTFAI code should be returned
 *				  (if appropriate), with TCMP_NOACCAUTH
 *				  indicated in the information bits of
 *				  the correct aforementioned RCM_ code.
 *
 *
 */
#define	RCM_NETREQDEL	00021L
/*
 *	[N]		The request as previously routed by a pipe queue
 *			server, could not be delivered to its destination
 *			because the request expired, or was deleted at the
 *			destination.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_NOACCAUTH	00022L
/*
 *	[B,D,N,P]	The request cannot be handled because the
 *			LOCAL machine has no password entry for the
 *			request owner user-id.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			This can occur if the local password database
 *			is changed, deleting an account AFTER a request
 *			owned by the deleted account has been queued.
 *
 *			The request is deleted.
 *
 *			WARNING:  This code must NEVER be returned by
 *				  pipe or network queue servers when their
 *				  corresponding remote server returns
 *				  TCMP_NOACCAUTH.  Instead, the proper
 *				  RCM_DELIVERFAI, RCM_ROUTEFAI, or
 *				  RCM_STAGEOUTFAI code should be returned
 *				  (if appropriate), with TCMP_NOACCAUTH
 *				  indicated in the information bits of
 *				  the correct aforementioned RCM_ code.
 *
 *
 */
#define	RCM_NOMOREPROC	00023L
/*
 *	[B,D,N,P]	The request cannot be executed because of a
 *			process shortage on the local machine.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, to be retried again at
 *			a later time.  All NQS queues on the local
 *			machine are stopped, to conserve remaining
 *			available processes (if any).
 *
 *
 */
#define	RCM_NONSECPORT	00024L
/*
 *	[N,P]		The pipe queue or network queue server connected
 *			to the transaction server on a non-secure port.
 *			The queue server is flawed.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, and the containing queue
 *			is stopped.
 *
 *
 */
#define	RCM_NORESTART	00025L
/*
 *	[B,D]		The request was running at the time of an NQS
 *			shutdown, or system crash--and the request was
 *			not defined as restartable (as determined by
 *			the request owner).
 *
 *			This completion code is returned directly from
 *			the local NQS daemon, and can only occur when NQS
 *			is being rebooted.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_NOSVRETCODE	00026L
/*
 *	[D,N,P]		The device server, network queue server, or pipe
 *			queue server failed to report a completion code
 *			on exit.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is placed in the failed directory,
 *			and the appropriate device or queue is stopped.
 *
 *
 */
#define	RCM_PATHLEN	00027L
/*
 *	[P]		The stdout or stderr pathname of a batch request
 *			when resolved at the selected pipe queue dest-
 *			ination (that would otherwise accept the request
 *			in the absence of this error), exceeds the maximum
 *			request path length supported by the NQS
 *			implementation at the receiving machine.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_PIPREQDEL	00030L
/*
 *	[P]		The routing of the request was interrrupted by
 *			a delete request at the local machine.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_REBUILDFAI	00031L
/*
 *			A request that was NOT running at the time of an
 *			NQS shutdown, or system crash, could not be
 *			requeued because of an internal NQS error.
 *
 *			This completion code is returned directly from
 *			the local NQS daemon, and can only occur when NQS
 *			is being rebooted.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is placed in the NQS failed directory.
 *
 *
 */
#define	RCM_REQCOLLIDE	00032L
/*
 *	[P]		The request being routed was found to collide with
 *			a previously existing request on a potential
 *			destination machine.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_RETRYLATER	00033L
/*
 *	[N,P]		The transaction for this request or subrequest cannot
 *			be presently carried out successfully, and should be
 *			retried later.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			Internal state updates are made as necessary.
 *
 *
 */
#define	RCM_ROUTED	00034L
/*
 *	[P]		The request was successfully routed and queued at
 *			one of the REMOTE destinations (different host)
 *			for the request, as selected by the pipe queue.
 *
 *			Quota information bits are present.
 *
 *			No output files are returned.
 *
 *			Internal state updates are made as necessary.
 *
 *
 */
#define	RCM_ROUTEDLOC	00035L
/*
 *	[P]		The request was successfully routed and queued at
 *			one of the LOCAL (same host) destinations for the
 *			request, as selected by the pipe queue.
 *
 *			Quota information bits are present.
 *
 *			No output files are returned.
 *
 *			Internal state updates are made as necessary.
 *
 *
 */
#define	RCM_ROUTEEXP	00036L
/*
 *	[P]		The request has not been routed to a destination.
 *			The original completion code from the server was
 *			RCM_RETRYLATER, and the routing expiration time
 *			on the request has been reached.
 *
 *			The original completion code is CONVERTED to this
 *			completion code when the expiration time has elapsed.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_ROUTEFAI	00037L
/*
 *	[P]		The request cannot be routed.  No destination
 *			will accept it.  Every destination selected for
 *			the request by the pipe queue server has been
 *			contacted, and indicated that it will not accept
 *			the request.
 *
 *			Failed pipe routing information bits are present,
 *			defining the set of failure conditions.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_ROUTERETX	00040L
/*
 *	[P]		The request was not routed to any destination, and
 *			the the retry limit for this type of transaction
 *			has been reached.
 *
 *			Failed pipe routing information bits are present,
 *			defining the set of failure conditions.
 *
 *			No output files are returned.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_SERBRKPNT	00041L
/*
 *	[D,N,P]		Server breakpoint encountered.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, and the device or queue
 *			is stopped as appropriate.
 *
 *
 */
#define	RCM_SEREXEFAI	00042L
/*
 *	[D,N,P]		Server execve() operation failed.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, and the device or queue
 *			is stopped as appropriate.
 *
 *
 */
#define	RCM_SERVESIGERR	00043L
/*
 *	[N,P]		Network or pipe server was killed by a signal
 *			that should NOT have killed it.
 *
 *			No information bits are present.
 *
 *			No output files are returned.
 *
 *			The request is requeued, and the device or queue
 *			is stopped as appropriate.
 *
 *
 */
#define	RCM_SHEXEF2BIG	00044L
/*
 *	[B]		One or both of the argv[] or envp[] argument sets
 *			to the batch request shell exceeded the maximum
 *			size supported by the underlying UNIX implement-
 *			ation.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_SHUTDNABORT	00045L
/*
 *	[B,D]		The server or shell process for the request exited
 *			because of a signal sent because of an NQS shutdown,
 *			and the request is NOT restartable.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_SHUTDNREQUE	00046L
/*
 *	[B,D]		The server or shell process for the request exited
 *			because of a signal sent because of an NQS shutdown,
 *			and the request IS restartable.
 *
 *			No information bits are present.
 *
 *			The request is requeued for continued execution, when
 *			NQS is restarted.
 *
 *
 */
#define	RCM_SSHBRKPNT	00047L
/*
 *	[B]		The system chosen shell to execute the batch
 *			request shell script stopped at a breakpoint.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted, and the queue is
 *			stopped.
 *
 *
 */
#define	RCM_SSHEXEFAI	00050L
/*
 *	[B]		The system chosen shell to execute the batch
 *			request shell script could not be successfully
 *			execve'd.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is requeued, and the queue is
 *			stopped.
 *
 *
 */
#define	RCM_STAGEOUT	00051L
/*
 *	[N]		The output file was successfully returned to
 *			its intended destination.
 *
 *			No information bits are present.
 *
 *			The output file is deleted, and internal
 *			state updates are made as necessary.
 *
 *
 */
#define	RCM_STAGEOUTBAK	00052L
/*
 *	[N]		The output file could not be returned to its
 *			intended destination because of non-retriable
 *			circumstances.  Instead, the file was successfully
 *			returned to its backup destination.
 *
 *			The information field of this return code MUST
 *			contain the TCM_ code that caused the failure
 *			at the primary destination (see ../lib/mergertcm.c).
 *
 *			The output file is deleted, and internal
 *			state updates are made as necessary.
 *
 *
 */
#define	RCM_STAGEOUTFAI	00053L
/*
 *	[N]		The output file cannot be staged-out by the network
 *			queue server.
 *
 *			The output file could not be returned to its
 *			intended destination, nor could it be returned
 *			to its backup destination, because of non-
 *			retriable error conditions.
 *
 *			The information field of this return code MUST
 *			contain the TCM_ code that caused the failure
 *			at the primary destination (see ../lib/mergertcm.c).
 *
 *			The output file must be deleted.
 *
 *
 */
#define	RCM_UNABLETOEXE	00054L
/*
 *	[B,D,N,P]	The request could not be executed for reason(s)
 *			identified in the information bit vector of
 *			the completion code.
 *
 *			Quota information bits are defined.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_UNAFAILURE	00055L
/*
 *	[B,D,N,P]	An unanticipated error condition occurred trying
 *			execute the request, or perform a transaction
 *			operation for the request.
 *
 *			No information bits are present.
 *
 *			If the operation was a stageout file transaction,
 *			then the associated output file is deleted, and
 *			the output file loss is recorded for the batch
 *			request.
 *
 *			All other operations result in the request being
 *			placed in the failed directory, and any associated
 *			output files are deleted.
 *
 *
 */
#define	RCM_UNCRESTDERR	00056L
/*
 *	[B]		The batch request specified a stderr access mode
 *			of -re, and the stderr file could not be created
 *			for reasons other than ENFILE and ENOSPC.
 *
 *			The information field of this return code MUST
 *			contain the TCM_ code that caused the failure
 *			(see ../lib/mergertcm.c).
 *
 *			Output files (if any) are queued for return.
 *
 *			The batch request is deleted.
 *
 *
 */
#define	RCM_UNCRESTDOUT	00057L
/*
 *	[B]		The batch request specified a stdout access mode
 *			of -ro, and the stdout file could not be created
 *			for reasons other than ENFILE and ENOSPC.
 *
 *			The information field of this return code MUST
 *			contain the TCM_ code that caused the failure
 *			(see ../lib/mergertcm.c).
 *
 *			Output files (if any) are queued for return.
 *
 *			The batch request is deleted.
 *
 *
 */
#define	RCM_UNDEFINED	00060L
/*
 *	[]		This completion code is returned if the completion
 *			code received from a shell process, or server was
 *			illegal.  No server or shell process can directly
 *			return this completion code.
 *
 *			No information bits are present.
 *
 *			If the operation was a stageout file transaction,
 *			then the associated output file is deleted, and
 *			the output file loss is recorded for the batch
 *			request.
 *
 *			All other operations result in the request being
 *			placed in the failed directory, and any associated
 *			output files are deleted.
 *
 *
 */
#define	RCM_USHBRKPNT	00061L
/*
 *	[B]		The user chosen shell to execute the batch
 *			request stopped at a breakpoint.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted.
 *
 *
 */
#define	RCM_USHEXEFAI	00062L
/*
 *	[B]		The user chosen shell to execute the batch
 *			request could not be execve'd.
 *
 *			No information bits are present.
 *
 *			Output files (if any) are queued for return.
 *
 *			The request is deleted.
 *
 *
 */
#ifdef SDSC
#define	RCM_BADTSCHED	00063L
/*
 *	[B,P]		Problem getting the requested dedicated time slot.
 *                      No information bits are present.
 *
 *                      Output files (if any) are queued for return.
 *
 *                      The request is deleted.
 *
 *
 */
#define RCM_NQSABORT    00064L
/*
 *      [B]             Abort NQS
 *                      No information bits are present.
 *
 *                    
 *
 *
 *
 */

#define	RCM_MAXRCM	RCM_NQSABORT	/* Max RCM_ (see ../lib/rcmmsgs.c */
					/* and ../src/nqs_reqexi.c) */
#else
#define	RCM_MAXRCM	RCM_USHEXEFAI	/* Max RCM_ (see ../lib/rcmmsgs.c */
#endif
					/* and ../src/nqs_reqexi.c) */
