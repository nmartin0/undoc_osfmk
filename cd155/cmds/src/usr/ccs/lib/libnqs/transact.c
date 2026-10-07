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
 
/*++ transact.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/transact.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module contains the functions:
 *
 *		tra_release(),
 *		tra_allocate(),
 *		tra_setstate(),
 *		tra_setinfo(), and
 *		tra_read().
 *
 *	which respectively:
 *
 *		mark a transaction descriptor as released (tra_release),
 *		allocate and completely configure a transaction descr-
 *		iptor (tra_allocate), set the transaction-state of the
 *		transaction (tra_setstate), set the information field in
 *		the transaction descriptor (tra_setinfo), and read the
 *		entire state of a transaction descriptor (tra_read).
 *
 *
 *
 *	WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
 *
 *
 *		If the implementation of transaction descriptors within
 *		this module is ever changed, then the NQS installation
 *		utility program:
 *
 *			../utility/src/nqsmktrans.c
 *
 *		will also have to be changed as appropriate.
 *
 *		../utility/src/nqsmktrans.c is intimately aware of the
 *		format of an NQS transaction descriptor.
 *
 *
 *	END-OF-WARNING *** END-OF-WARNING *** END-OF-WARNING *** END-OF-WARNING
 *
 *
 *
 *	Transaction descriptor PREFACE:
 *	-------------------------------
 *
 *	NQS will always need the transaction descriptors implemented herein,
 *	until the mighty mountain of AT&T heaves and surges, releasing
 *	System V.3 containing the ability to perform synchronous write I/O
 *	operations.  This has been promised Real Soon Now for a long time.
 *	Unfortunately, I cannot wait.  NQS needs to work NOW.
 *
 *	While it is true that some vendors have thoughtfully provided some
 *	synchronous write I/O mechanisms, these mechanisms remain non-standard
 *	and make the porting of NQS that much worse.
 *
 *	As soon as synchronous write file I/O is supported on all UNIX
 *	systems (as discussed below), then this module should be rewritten,
 *	using the RESERVED fields in the control file for each request to
 *	store the transaction state information now presently stored in
 *	the transaction descriptor allocated for the request.  Transaction
 *	descriptors should be made extinct as soon as possible!
 *
 *	Lastly, ALL of the comments below refer to UNIX implementations as
 *	they existed in general at the time of March 26, 1986 (as known by
 *	the ignorant author--that's me).
 *
 *
 *
 *	Transaction descriptor OVERVIEW:
 *	--------------------------------
 *
 *	NQS allocates one transaction descriptor per queued request.
 *	Each allocated descriptor describes the transaction state of
 *	its parent request--which is NOT in the process of being
 *	deleted (requests being deleted have their transaction descr-
 *	iptors deallocated first).  The request transaction states are:
 *
 *
 *		RTS_STASIS:	The state of the transaction when it is not
 *				in any of the other RTS_ states.
 *
 *		RTS_PREARRIVE:	Preparing to arrive.  The peer machine has
 *				successfully enqueued this request on this
 *				machine.  We are waiting for the remote pipe
 *				queue server to confirm that the peer machine
 *				has recorded this machine as the destination
 *				for this request.
 *
 *		RTS_ARRIVE:	The request is in the arriving state (this
 *				does NOT mean that the request files are
 *				actually being RECEIVED at this time, but
 *				they might be)....
 *
 *		RTS_PREDEPART:	Preparing to depart.  The peer machine has
 *				successfully enqueued this request in the
 *				RTS_PREARRIVE state, and the control file
 *				header has been delivered to the peer
 *				machine, so that resource TCML_GRANFATHER
 *				checks function correctly, and queue access
 *				permissions cannot later fail to admit this
 *				request.  However, we have not yet COMMITTED
 *				to routing this request to the peer
 *				destination machine.
 *
 *		RTS_DEPART:	The request has been committed to the
 *				peer machine for delivery.  The request might
 *				even be getting transferred to the destination
 *				machine at this very moment.
 *
 *		RTS_STAGEIN:	The batch request is presently staging files
 *				in for execution, or has completed staging in
 *				zero or more stage-in files.
 *
 *		RTS_EXECUTING:	The request is presently EXECUTING (this state
 *				does NOT include the states of a server
 *				routing, sending, or receiving the request
 *				across a machine boundary).
 *
 *		RTS_STAGEOUT:	The batch request is queued to stage-out files,
 *				or is presently staging files out, to complete
 *				its life cycle.
 *
 *
 *
 *
 *	Transaction descriptor IMPLEMENTATION (constraints):
 *	----------------------------------------------------
 *
 *	Transaction descriptors exist because there are times when NQS must
 *	ABSOLUTELY, and RELIABLY know that some PERMANENT memory of the
 *	request state will exist across machine shutdowns or crashes.
 *
 *	This permanent memory is implemented in the UNIX file system, and
 *	is therefore vulnerable to the physical destruction of the actual
 *	disk medium.
 *
 *	This design does not deal with such catastrophic occurrences.
 *
 *	It might seem that the state information required for a request
 *	could simply be stored in the control file describing each request.
 *	This is not however, the case.  The standard UNIX file system does
 *	not physically perform disk I/O, until no more disk cache buffers
 *	are available in the system to perform some new I/O request.
 *
 *	Furthermore, some UNIX implementations provide enormous buffer
 *	caches containing upwards of 1000 disk cache buffers.  It can be a
 *	very long time indeed before the action defined by a write(2)
 *	system call takes place on the physical disk medium.  If few people
 *	are logged onto the system, or I/O traffic is low, many minutes
 *	can elapse before a given disk cache buffer allocated by a write(2)
 *	call is physically written to the disk.
 *
 *	There are however three exceptions to this situation.  First, many
 *	UNIX implementations configure a 'sync' process that runs every
 *	N seconds or minutes (or sometimes every N hours), that schedules
 *	all written disk cache buffers for writing to the disk.  Shortly
 *	thereafter, all of the allocated disk cache buffers are written
 *	to disk.
 *
 *	Second, the link(2) system call in all UNIX implementations known
 *	to the author (UNICOS, UTS, System V, Berkeley UNIX, and the UNIX
 *	implementation running on the Silicon Graphics IRIS machines),
 *	synchronously updates the target inode of the link(2) system call.
 *	By synchronous, I mean that the link(2) system call does not return
 *	until the modified inode of the target file (the file whose link
 *	count is being increased), has been physically written to disk.
 *	In this way, we can store a small amount of information in a file
 *	inode structure and then make a link to the same file, forcing the
 *	synchronous update the file inode.
 *
 *	The link(2) call is synchronous for at least the good reason that
 *	the link count for an inode will always be greater than or equal to
 *	the actual number of links to the file.  (The link count can be
 *	greater than the number of actual links to the file if the system
 *	crashes before the disk cache buffers associated with the new
 *	directory entry are written to disk.)  The constraint on the link
 *	count values of being greater than or equal to the actual number
 *	of links to the file makes it possible for programs like 'fsck' to
 *	work, rebuilding the file system after a system crash.
 *	
 *	A third method of avoiding the standard UNIX disk buffering
 *	mechanism has been implemented in the UNICOS version of UNIX.
 *	UNICOS provides an I/O mode that can be used on ordinary files to
 *	read and write 4K bytes at a time, bypassing the standard buffer-
 *	ing mechanism of the UNIX kernel.
 *
 *	Of these three alternatives, only one is portable and synchronous
 *	to the extent that we do not have to wait N seconds after the
 *	completion of the system call for permanent memory to record the
 *	transaction state.
 *
 *	Thus, transaction descriptors with their need for reasonably quick
 *	and very reliable recording onto the physical disk medium are
 *	implemented using file inode structures.
 *
 *	Lastly, it is still possible for NQS to loose requests, or
 *	suffer some file system damage upon a system crash using this
 *	implementation.  This implementation only records the CRITICAL
 *	state information associated with a request.  To record all data
 *	reliably on disk requires the use of non-standard system calls
 *	(if provided), to perform disk updates synchronously when necessary.
 *	Also, a 'sync' process running on the host UNIX operating system
 *	that has a reasonable update interval, will help to reduce the
 *	possibility of loosing the less volatile information stored in
 *	in the NQS request files.
 *
 *
 *
 *	Transaction descriptor IMPLEMENTATION:
 *	--------------------------------------
 *
 *	The transaction descriptor implementation using file-inodes is
 *	very portable, but also has some limitations.  Only one field of
 *	an inode can be safely used, namely the file modification time
 *	field.  Use of the 'last-accessed' time field for example, would
 *	make the NQS database vulnerable to system-wide backup procedures,
 *	that would open the file for reading, and change the access-time....
 *
 *	Restricting the data space used in a file inode to the 32-bits of
 *	file modification time, has the unpleasant side effect that four
 *	(4) (two (2) under UNICOS implementations) inodes are required to
 *	record all of the state information in an NQS transaction descriptor.
 *	(It must be noted that the modification time field of a UNICOS inode
 *	is 64-bits in length, and we are therefore able to cut our inode
 *	consumption by half).
 *
 *	Furthermore, the file inodes comprising any NQS transaction
 *	descriptor cannot be created and deleted on demand.  Instead, a
 *	finite number of file inodes must be preallocated to create a fixed
 *	maximum number of transaction descriptors.
 *
 *	The preallocation of the file inodes comprising an NQS transaction
 *	descriptor is required because the disk blocks comprising the
 *	directory file that contains the names assigned to the given file-
 *	inode set go through the same disk cache buffering mechanism that
 *	can go for many seconds, minutes, or even days without being
 *	completely written to disk.
 *
 *	When 'fsck' locates a inode without any links to it, the file
 *	is destroyed, or linked into the temporary directory depending on
 *	the system administrator's response.  In either case however, the
 *	original name tied to the inode is lost.  Thus, a finite number of
 *	inodes comprising a finite number of transaction descriptors are
 *	allocated by the NQS installation procedures, thus avoiding the
 *	link/unlink traffic that would make NQS vulnerable to delayed
 *	directory block updates.
 *
 *	
 *
 *	Transaction descriptor DEFINITION
 *	---------------------------------
 *
 *	There are two (2) transaction descriptor types.  They are:
 *
 *		1.  An UNALLOCATED_TRANSACTION descriptor.
 *
 *		2.  A REQUEST_STATE_TRANSACTION descriptor.
 *		    This name is somewhat misleading (see #3).
 *		    Every request that is NOT presently being
 *		    deleted, has a single request transaction
 *		    descriptor of this type allocated to it
 *		    at all times.
 *
 *
 *	The four (4) (two (2) under UNICOS) inodes comprising a request
 *	transaction descriptor, reside in the NQS transactions subdirectory
 *	hierarchy.  Within their containing directory, each transaction
 *	inode has a name of the form:
 *
 *		tN
 *
 *	where 't' represents the 6-character formatted 6-bit alphabet ASCII
 *	string of the transaction descriptor transaction-id number.
 *
 *	N represents the single character formatted 6-bit alphabet ASCII
 *	string of one of the integer values [0, 1, 2, 3] corresponding to
 *	the four respective inodes comprising the transaction descriptor
 *	(or one of the integer values [0, 1] corresponding to the two re-
 *	spective inodes under UNICOS used to represent a transaction
 *	descriptor).
 *
 *	By convention, the TRANSACTION-ID assigned to a particular trans-
 *	action is a number in [1..number-of-transaction-descriptors].
 *
 *	When a request is first queued on the local host, it is assigned
 *	a transaction descriptor.  The number of this transaction descriptor
 *	is called the transaction-id, and this value is recorded in the
 *	control file for the request.  It should also be noted that upon
 *	NQS rebuild, all allocated transaction descriptors can be easily
 *	"reattached" to their parent request, since ALL allocated
 *	transaction descriptors record their owner request-id.
 *
 *	When viewed together, the four (4) (two (2) under UNICOS) inodes
 *	comprising a transaction descriptor form an array of 4 integers,
 *	each of which is at least 32-bits in size.  The following paragraphs
 *	provide the detailed definition of the bit usage for both transac-
 *	tion descriptor types, when viewed as an array of 4 integers.
 *
 *
 *	*****************************************************************
 *	*								*
 *	*  UNICOS implementations pack the integers: Int1, and Int2	*
 *	*  into the modification time of inode #0 of the transaction	*
 *	*  descriptor, while the integers: Int3, and Int4 are packed	*
 *	*  into the modification time of inode #1 of the transaction	*
 *	*  descriptor.							*
 *	*								*
 *	*****************************************************************
 *
 *
 *
 *	UNALLOCATED_TRANSACTION descriptor:
 *	-----------------------------------
 *	  .-------------------------------------------------------------------.
 *	  | TYPE  |	RESERVED/UNDEFINED				      |
 *  Int1: |   |   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *	  `-------------------------------------------------------------------'
 *	    31 30 29 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *	  .-------------------------------------------------------------------.
 *	  |		RESERVED/UNDEFINED				      |
 *  Int2: |   |   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *	  `-------------------------------------------------------------------'
 *	    31 30 29 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *	  .-------------------------------------------------------------------.
 *	  |		RESERVED/UNDEFINED				      |
 *  Int3: |   |   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *	  `-------------------------------------------------------------------'
 *	    31 30 29 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *	  .-------------------------------------------------------------------.
 *	  |		RESERVED/UNDEFINED				      |
 *  Int4: |   |   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *	  `-------------------------------------------------------------------'
 *	    31 30 29 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *
 *
 *	  TYPE contains the 2-bit transaction descriptor type: ^B00.
 *	  ALL RESERVED/UNDEFINED bits are not presently used, and
 *	  	their value is indeterminate.
 *
 *
 *
 *	REQUEST_STATE_TRANSACTION descriptor:
 *	-------------------------------------
 *	  .-------------------------------------------------------------------.
 *	  | TYPE  |	REQUEST SEQUENCE#				      |
 *  Int1: |   |   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *	  `-------------------------------------------------------------------'
 *	    31 30 29 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *	  .-------------------------------------------------------------------.
 *	  |		REQUEST MACHINE-ID				      |
 *  Int2: |   |   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *	  `-------------------------------------------------------------------'
 *	    31 30 29 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *	  .-------------------------------------------------------------------.
 *	  |		LAST STATE CHANGE TIME				|STATE|
 *  Int3: |   |   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *	  `-------------------------------------------------------------------'
 *	    31 30 29 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *	  .-------------------------------------------------------------------.
 *	  |		PEER MACHINE-ID/PENDING FILE STAGING EVENTS MASK      |
 *  Int4: |   |   | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *	  `-------------------------------------------------------------------'
 *	    31 30 29 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *
 *
 *
 *	  TYPE contains the 2-bit transaction descriptor type: ^B01.
 *	  REQUEST SEQUENCE# contains the 30-bit request sequence#.
 *	  REQUEST MACHINE-ID contains the 32-bit request machine-id.
 *	  LAST STATE CHANGE TIME contains the 29-bit time stamp of the time
 *		at which the request state was last changed DIV 8.  This
 *		time is used by the NQS rebuild algorithm upon NQS startup.
 *	  STATE contains the 3-bit transaction state:
 *		[RTS_STASIS, RTS_PREARRIVE, RTS_ARRIVE, RTS_PREDEPART,
 *		 RTS_DEPART, RTS_STAGEIN, RTS_EXECUTING, RTS_STAGEOUT].
 *	  PEER MACHINE-ID/PENDING FILE STAGING EVENTS MASK contains
 *		either:
 *
 *			1.  The 32-bit network peer machine-id involved
 *			    in the request transfer operation (STATE is
 *			    one of [RTS_PREARRIVE, RTS_ARRIVE,
 *			    RTS_PREDEPART, RTS_DEPART];
 *
 *		or:
 *
 *			2.  The 32-bit pending file staging events mask
 *			    identifying the uncompleted file/hierarchy
 *			    staging events [0..29] (STATE is one of
 *			    [RTS_STAGEIN, RTS_STAGEOUT] and the request
 *			    is a batch request and not a device request).
 *
 *			    File staging events [30..31] are respectively
 *			    allocated to the return of the stdout and
 *			    stderr files.
 *
 *				Bit n=0 implies that staging event #n
 *					is complete, or never existed.
 *				Bit n=1 implies that staging event #n
 *					exists, and is not complete.
 *
 *			    Note that a hard limit of 30 file/hierarchy
 *			    staging events exists for a batch request.
 *			    Thus, a maximum of 30 file staging specif-
 *			    ications can be given for stagein, exclusive
 *			    of the number of stageout events.  An identical
 *			    maximum exists for stageout, exclusive of the
 *			    number of stagein events.  (This count does NOT
 *			    include the staging events for returning stderr
 *			    and stdout files.)
 *
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 26, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:28:02 $ $State: Exp $)
 * $Log: transact.c,v $
 * Revision 1.3  1994/11/19  02:28:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:00:02  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:17  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:52  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)transact.c	1.2 (transact.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nqs.h"		/* NQS types and definitions */
#include "nqsxdirs.h"		/* NQS directories and sys/types.h */
#include <sys/stat.h>		/* Stat structure */


extern uid_t getuid();		/* Get real user-id */
extern gid_t getgid();		/* Get real group-id */
extern void pack6name();	/* Compute name on 6-bit alphabet */
extern int setmtime();		/* Set file modification time synchronously */


/*** tra_release
 *
 *
 *	int tra_release():
 *
 *	Mark the specified transaction descriptor as released (available
 *	for use by another NQS request).
 *
 *	WARNING:
 *		This function should never be called by any
 *		process other than the local NQS daemon.
 *
 *	Returns:
 *		 0: if successful.
 *		-1: if an error occurs (errno has error code).
 */
int tra_release (tid)
int tid;				/* Transaction-id */
{
	void tdescrpath();		/* Compute trans-descr path */

	char path [MAX_PATHNAME+1];	/* Transaction-descriptor name */

	/*
	 *  Make the NQS daemon the owner of the transaction
	 *  descriptor.
	 */
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	tdescrpath (path, tid, 3);
	if (chown (path, (int) getuid(), (int) getgid()) == -1) return (-1);
	tdescrpath (path, tid, 2);
	if (chown (path, (int) getuid(), (int) getgid()) == -1) return (-1);
	tdescrpath (path, tid, 1);
	if (chown (path, (int) getuid(), (int) getgid()) == -1) return (-1);
	tdescrpath (path, tid, 0);
	if (chown (path, (int) getuid(), (int) getgid()) == -1) return (-1);
	return (set_mtime (tid, 0, (unsigned long) TRA_UNALLOCATED << 30));
#else
#if	UNICOS
	tdescrpath (path, tid, 1);
	if (chown (path, (int) getuid(), (int) getgid()) == -1) return (-1);
	tdescrpath (path, tid, 0);
	if (chown (path, (int) getuid(), (int) getgid()) == -1) return (-1);
	return (set_mtime (tid, 0, (unsigned long) TRA_UNALLOCATED << 62));
#else
BAD SYSTEM TYPE
#endif
#endif
}


/*** tra_allocate
 *
 *
 *	int tra_allocate():
 *
 *	Allocate and initialize the specified transaction descriptor
 *	with the given transaction descriptor values, and owner.  Note
 *	that the transaction update time is set by this procedure....
 *
 *	Returns:
 *		 0: if successful.
 *		-1: if an error occurs (errno has error code).
 */
int tra_allocate (tid, inittransact, owner_uid, owner_gid)
int tid;				/* Transaction-id */
struct transact *inittransact;		/* Initial configuration */
uid_t owner_uid;			/* Owner user-id */
gid_t owner_gid;			/* Owner group-id */
{
	void tdescrpath();		/* Compute trans-descr path */

	char path [MAX_PATHNAME+1];	/* Transaction-descriptor name */
	register unsigned long ulongval;/* Working variable */

	/*
	 *  It is IMPERATIVE that we update element #0 LAST, so that a
	 *  transaction descriptor NEVER appears allocated when it has
	 *  only been partially defined because of a system crash at
	 *  the inopportune time....
	 *
	 *  But first, we make the transaction descriptor owned by the
	 *  request upon whose behalf this descriptor is being allocated.
	 */
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	tdescrpath (path, tid, 3);
	if (chown (path, (int) owner_uid, (int) owner_gid) == -1) return (-1);
	tdescrpath (path, tid, 2);
	if (chown (path, (int) owner_uid, (int) owner_gid) == -1) return (-1);
	tdescrpath (path, tid, 1);
	if (chown (path, (int) owner_uid, (int) owner_gid) == -1) return (-1);
	tdescrpath (path, tid, 0);
	if (chown (path, (int) owner_uid, (int) owner_gid) == -1) return (-1);
	time (&inittransact->update);	/* Set update time */
	if (set_mtime (tid, 1, (unsigned long) inittransact->orig_mid) == -1 ||
	    set_mtime (tid, 2,
		     ((unsigned long) inittransact->update << 3)
		   + ((unsigned long) inittransact->state)) == -1) {
		return (-1);
	}
	ulongval = 0;
	if (inittransact->state != RTS_STAGEIN &&
	    inittransact->state != RTS_STAGEOUT) {
		ulongval = (unsigned long) inittransact->v.peer_mid;
	}
	else ulongval = (unsigned long) inittransact->v.events31;
	if (set_mtime (tid, 3, (unsigned long) ulongval) == -1) {
		return (-1);
	}
	return (set_mtime (tid, 0,
			  ((unsigned long) inittransact->tra_type << 30)
			+ ((unsigned long) inittransact->orig_seqno)));
#else
#if	UNICOS
	tdescrpath (path, tid, 1);
	if (chown (path, (int) owner_uid, (int) owner_gid) == -1) return (-1);
	tdescrpath (path, tid, 0);
	if (chown (path, (int) owner_uid, (int) owner_gid) == -1) return (-1);
	time (&inittransact->update);	/* Set update time */
	ulongval = ((unsigned long) inittransact->update << 35)
		 + ((unsigned long) inittransact->state << 32);
	if (inittransact->state != RTS_STAGEIN &&
	    inittransact->state != RTS_STAGEOUT) {
		ulongval += (unsigned long) inittransact->v.peer_mid;
	}
	else ulongval += (unsigned long) inittransact->v.events31;
	if (set_mtime (tid, 1, ulongval) == -1) {
		return (-1);
	}
	return (set_mtime (tid, 0,
			  ((unsigned long) inittransact->tra_type << 62)
			+ ((unsigned long) inittransact->orig_seqno << 32)
			+ ((unsigned long) inittransact->orig_mid)));
#else
BAD SYSTEM TYPE
#endif
#endif
}


/*** tra_setstate
 *
 *
 *	int tra_setstate():
 *
 *	Set the transaction state of the specified transaction
 *	descriptor.  Note that the transaction update time is
 *	set by this procedure....
 *
 *	Returns:
 *		 0: if successful.
 *		-1: if an error occurs (errno has error code).
 */
int tra_setstate (tid, transact)
int tid;				/* Transaction-id */
struct transact *transact;		/* Transaction */
{
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
#else
#if	UNICOS
	register unsigned long ulongval;
#else
BAD SYSTEM TYPE
#endif
#endif

	time (&transact->update);
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	return (set_mtime (tid, 2,
			  ((unsigned long) transact->update << 3)
			+ ((unsigned long) transact->state)));
#else
#if	UNICOS
	ulongval = ((unsigned long) transact->update << 35)
		 + ((unsigned long) transact->state << 32);
	if (transact->state != RTS_STAGEIN &&
	    transact->state != RTS_STAGEOUT) {
		ulongval += (unsigned long) transact->v.peer_mid;
	}
	else ulongval += (unsigned long) transact->v.events31;
	return (set_mtime (tid, 1, (unsigned long) ulongval));
#else
BAD SYSTEM TYPE
#endif
#endif
}


/*** tra_setinfo
 *
 *
 *	int tra_setinfo():
 *	Set the information field of a transaction descriptor.
 *
 *	Returns:
 *		 0: if successful.
 *		-1: if an error occurs (errno has error code).
 */
int tra_setinfo (tid, transact)
int tid;				/* Transaction-id */
struct transact *transact;		/* Transaction */
{
	register unsigned long ulongval;		/* Working variable */

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	if (transact->state != RTS_STAGEIN &&
	    transact->state != RTS_STAGEOUT) {
		ulongval = (unsigned long) transact->v.peer_mid;
	}
	else ulongval = (unsigned long) transact->v.events31;
	return (set_mtime (tid, 3, (unsigned long) ulongval));
#else
#if	UNICOS
	ulongval = ((unsigned long) transact->update << 35)
		 + ((unsigned long) transact->state << 32);
	if (transact->state != RTS_STAGEIN &&
	    transact->state != RTS_STAGEOUT) {
		ulongval += (unsigned long) transact->v.peer_mid;
	}
	else ulongval += (unsigned long) transact->v.events31;
	return (set_mtime (tid, 1, (unsigned long) ulongval));
#else
BAD SYSTEM TYPE
#endif
#endif
}


/*** tra_read
 *
 *
 *	int tra_read():
 *
 *	Return the configuration of the specified transaction
 *	descriptor.
 */
int tra_read (tid, transact)
int tid;				/* Transaction-id */
struct transact *transact;		/* Transaction */
{
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	unsigned long ulong1, ulong2, ulong3, ulong4;

	if (get_mtime (tid, 0, &ulong1) == -1 ||
	    get_mtime (tid, 1, &ulong2) == -1 ||
	    get_mtime (tid, 2, &ulong3) == -1 ||
	    get_mtime (tid, 3, &ulong4) == -1) {
		return (-1);
	}
#else
#if	UNICOS
	unsigned long ulong1, ulong2;

	if (get_mtime (tid, 0, &ulong1) == -1 ||
	    get_mtime (tid, 1, &ulong2) == -1) {
		return (-1);
	}
#else
BAD SYSTEM TYPE
#endif
#endif
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	transact->tra_type = ((ulong1 >> 30) & 03);
	transact->orig_seqno = (ulong1 & 07777777777);
	transact->orig_mid = (mid_t) ulong2;
	transact->update = ((ulong3 >> 3) & 03777777777);
	transact->state = (ulong3 & 07);
	if (transact->state != RTS_STAGEIN &&
	    transact->state != RTS_STAGEOUT) {
		transact->v.peer_mid = (mid_t) ulong4;
	}
	else transact->v.events31 = ulong4;
#else
#if	UNICOS
	transact->tra_type = ((ulong1 >> 62) & 03);
	transact->orig_seqno = ((ulong1 >> 32) & 07777777777);
	transact->orig_mid = (ulong1 & 037777777777);
	transact->update = ((ulong2 >> 35) & 03777777777);
	transact->state = ((ulong2 >> 32) & 07);
	if (transact->state != RTS_STAGEIN &&
	    transact->state != RTS_STAGEOUT) {
		transact->v.peer_mid = (mid_t) (ulong2 & 037777777777);
	}
	else transact->v.events31 = (ulong2 & 037777777777);
#else
BAD SYSTEM TYPE
#endif
#endif
	return (0);
}


/*** get_mtime
 *
 *
 *	int get_mtime():
 *
 *	Get the modification time of the specified transaction
 *	descriptor element inode.
 *
 *	Returns:
 *		 0: if successful.
 *		-1: if unsuccessful (errno has reason).
 */
static int get_mtime (tid, elementno, ulongval)
int tid;				/* Transaction-id */
int elementno;				/* Element# */
unsigned long *ulongval;		/* Unsigned long mtime */
{
	void tdescrpath();		/* Compute trans-descr path */

	struct stat stat_buf;		/* Stat() buffer */
	char path [MAX_PATHNAME+1];	/* Transaction-descriptor name */
	register int res;		/* System call result */

	tdescrpath (path, tid, elementno);
	if ((res = stat (path, &stat_buf)) != -1) {
		/*
		 *  The stat() call was successful (how nice).
		 */
		*ulongval = stat_buf.st_mtime;
	}
	return (res);			/* Return result of stat() call */
}


/*** set_mtime
 *
 *
 *	int set_mtime():
 *
 *	Set the modification time of the specified transaction
 *	descriptor element inode.
 *
 *	Returns:
 *		 0: if successful.
 *		-1: if unsuccessful (errno has reason).
 */
static int set_mtime (tid, elementno, ulongval)
int tid;				/* Transaction-id */
int elementno;				/* Element# */
unsigned long ulongval;			/* Unsigned long datum */
{
	void tdescrpath();		/* Compute trans-descr path */
	char path [MAX_PATHNAME+1];	/* Transaction-descriptor name */

	tdescrpath (path, tid, elementno);
	return (setmtime (path, ulongval));
}					/* Synchronously set mtime */


/*** tdescrpath
 *
 *
 *	void tdescrpath():
 *	Compute transaction descriptor path.
 */
static void tdescrpath (path, tid, element)
char *path;				/* Transaction descriptor path */
int tid;				/* Transaction descriptor-id */
int element;				/* Element# */
{
	pack6name (path, Nqs_transact, (int) ((tid-1) % MAX_TDSCSUBDIRS),
		  (char *) 0, (long) tid-1, 6, 0L, 0, element, 1);
}
