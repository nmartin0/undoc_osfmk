/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: credentials.c,v $
 * Revision 1.15  1995/02/18  01:41:11  yazz
 *  Reviewer: John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 12240, including emul console logging cleanup
 *  Testing: EATs controlc, sched
 *  Module(s):
 * 	svr/emulator/bsd_user_side.c
 * 	svr/emulator/emul_chkpnt.c
 * 	svr/emulator/emul_init.c
 * 	svr/emulator/emul_mapped.c
 * 	svr/emulator/fsvr_user_side.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/bsd/mach_signal.c
 * 	svr/server/bsd/subr_prf.c
 * 	svr/server/tnc/dvp_vpops.c
 * 	svr/server/uxkern/boot_config.c
 * 	svr/server/uxkern/bsd_server_side.c
 * 	svr/server/uxkern/credentials.c
 * 	svr/server/uxkern/rpm_clock.c
 *
 * General cleanup of emulator console logging.  Added bootnode_printf()
 * routine to server.  Added server bootmagic variable ENABLE_RPM_TIMESTAMP
 * so printf() and bootnode_printf() messages are timestamped with the
 * 56-bit RPM global clock value.  This enables very fine timings to be
 * observable in console log output.
 *
 * Revision 1.14  1994/11/18  20:47:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/05/06  17:10:15  dbm
 * Mainline merge of R1.2 version 1.11.2.1
 *
 * Revision 1.12  1994/01/12  17:47:15  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.11.2.1  1994/05/05  22:01:19  dbm
 * Fix missing locks around cred_master_unlock_entry and
 * cred_cache_unlock_entry().  Also changed condition_signal calls to
 * condition broadcast.
 *
 *  Reviewer:Brad Rullman, Charlie Johnson
 *  Risk:Low
 *  Benefit or PTS #:9069
 *  Testing:Specific test case, fileio,pfs,unix interfaces.
 *  Module(s): uxkern/credentials.c
 *
 * Revision 1.11  1993/09/08  21:12:34  dbm
 * Fix for PTS bug #6353 when processes with would hang on exit if they
 * had open files.  New credentials_register() and cred_cache_allocate_entry()
 * functions from OSF. (rabii@osf.org)
 *
 * Revision 1.10  1993/07/14  18:40:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  20:59:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.9  1993/05/07  18:18:24  nandy
 * Fixed a merge conflict.
 *
 * Revision 1.8  1993/05/06  19:26:04  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:51:10  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4.6.2  1993/04/04  00:44:53  cfj
 * Put ux_server_thread_blocking/unblocking around the call to cred_info_request().
 *
 * Revision 1.6  1993/04/03  03:11:23  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/08  18:26:25  nandy
 * Merged from T9 branch.
 *
 * Revision 1.4.6.1  1993/03/06  23:35:00  nandy
 * Changes from OSF for new locking mechanism.
 *
 * Revision 1.28  93/02/02  15:24:27  rabii
 * 	Fixed printf in credentials_deregister (rabii)
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:04:45  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/11/30  22:53:29  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:34:17  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:42:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.27  93/01/26  11:04:55  durriya
 * 	Increased initial size of the various creds zones.             (durriya)
 * 
 * Revision 1.26  92/12/17  11:30:13  rabii
 * 	[92/11/24  14:28:54  chrisp]
 * 	In credentials_allocate_body(), move creation of send right to new cred
 * 		port into main code path so that this is done for CM_MIGRATE also.
 * 	In credentials_deallocate_body(), use mach_port_mod_refs() to remove
 * 		additional send right in CM_MIGRATE case.
 * 	Clean up panic message texts thereabouts also.
 * 
 * Revision 1.25  92/11/17  19:53:01  loverso
 * 	Fix bug where lock wasn't being released. (rabii)
 * 
 * Revision 1.24  92/11/11  10:38:34  rabii
 * 	Modified the master to hold a send right to the creds port (rabii)
 * 
 * Revision 1.23  92/11/09  15:26:29  rabii
 * 	[92/10/28  16:00:22  roman]
 * 	Fix RCS comments.
 * 
 * 	[92/10/06  12:17:18  roman]
 * 	Fix RCS comments.
 * 	Fix bug in parameter order from last OSF merge.
 * 
 * Revision 1.22  92/09/29  16:48:30  rabii
 * 	Cleanup panic messages. (loverso)
 * 
 * Revision 1.21  92/09/11  09:29:07  rabii
 * 	Added the extra flag "CM_EXITING" indicating that a cred master entry 
 * 	is being deallocated. If so, then the routine cred_master_lock_entry
 * 	will examine it's new parameter "nowait" and will return NULL. 
 * 	This fixes a deadlock caused by a race condition with ISC and
 * 	a file open. (rabii)
 * 
 * Revision 1.20  92/06/30  22:47:41  loverso
 * 	Add support for passing ruid and rgid to fix the access bug. (rabii)
 * 
 * 	Change interfaces to credentials_allocate() and
 * 	credentials_migrate_in() (and thus also to the common subroutine
 * 	credentials_allocate_body()) to allow the full setup of a credentials
 * 	structure during the call, rather than having to do a subsequent
 * 	credentials_set_state() call. (roman)
 * 
 * Revision 1.19  92/05/12  00:03:21  loverso
 * 	Add credentials_get_send_right(), which gives away a single send
 * 	right on the credentials port.  (loverso)
 * 
 * Revision 1.18  92/05/01  09:49:03  rabii
 * 	Fix the way panic messages are spread over two lines.
 * 	Fix dyslexic comment. (roman)
 * 
 * Revision 1.17  92/04/07  13:45:36  pjg
 * 	Moved handling of creds port reference counting from 
 * 	fsvr_thread_terminate to the cred service, which now
 * 	keeps a manual count of the send rights per cache entry and
 * 	will only decrement them evey MAX_CPORT_REF times or at
 * 	exit time. (rabii)
 * 
 * 	Also removed the mig stubs from this file and put them
 * 	in cred_servers.c. And fixed some printfs (rabii)
 * 
 * Revision 1.29  1993/04/29  13:50:09  klh
 * 	Revision 1.30  93/04/08  11:38:20  loverso
 * 		Where 1.29 added blocking/unblocking around the cache_getrusage()
 * 		RPC, this does to the cred_info_request() RPC.
 *
 * 	Revision 1.29  93/03/22  23:58:25  condict
 * 		Added calls to ux_server_thread_blocking/unblocking around the RPC,
 * 		to avoid starving unwired threads and hanging server.
 *
 * 	Revision 1.28  93/02/02  15:24:27  rabii
 * 		Fixed printf in credentials_deregister (rabii)
 *
 * 	Revision 1.27  93/01/26  11:04:55  durriya
 * 		Increased initial size of the various creds zones.             (durriya)
 *
 * Revision 1.28  93/03/22  21:16:43  yazz
 * OSF lock changes.  Call ux_server_thread_blocking/unblocking() when remote
 * cred services requested.  These routines drop and re-acquire the master
 * lock, among other things.
 * 
 * Revision 1.27  92/11/24  14:28:54  chrisp
 * In credentials_allocate_body(), move creation of send right to new cred
 * 	port into main code path so that this is done for CM_MIGRATE also.
 * In credentials_deallocate_body(), use mach_port_mod_refs() to remove
 * 	additional send right in CM_MIGRATE case.
 * Clean up panic message texts thereabouts also.
 * 
 * Revision 1.26  92/11/23  15:51:54  klh
 * 	Revision 1.25  92/11/17  19:53:01  loverso
 * 		Fix bug where lock wasn't being released. (rabii)
 * 
 * 	Revision 1.24  92/11/11  10:38:34  rabii
 * 		Modified the master to hold a send right to the creds port (rabii)
 * 
 * Revision 1.25  92/10/28  16:00:22  roman
 * Fix RCS comments.
 * 
 * Revision 1.24  92/10/06  12:17:18  roman
 * Fix RCS comments.
 * Fix bug in parameter order from last OSF merge.
 * 
 * Revision 1.23  92/10/05  15:21:20  klh
 * 	Revision 1.22  92/09/29  16:48:30  rabii
 * 		Cleanup panic messages. (loverso)
 * 
 * 	Revision 1.21  92/09/11  09:29:07  rabii
 * 		Added the extra flag "CM_EXITING" indicating that a cred 
 *		master entry is being deallocated. If so, then the routine 
 *		cred_master_lock_entry will examine it's new parameter 
 *		"nowait" and will return NULL. This fixes a deadlock caused 
 *		by a race condition with ISC and a file open. (rabii)
 * 
 * Revision 1.22  92/07/07  13:10:37  klh
 * 	Revision 1.20  92/06/30  22:47:41  loverso
 * 		Add support for passing ruid and rgid to fix the access bug. 
 *		(rabii)
 * 
 * Revision 1.21  92/06/05  14:11:42  klh
 * 	Revision 1.19  92/05/12  00:03:21  loverso
 * 		Add credentials_get_send_right(), which gives away a single 
 *		send right on the credentials port.  (loverso)
 * 
 * 	Revision 1.18  92/05/01  09:49:03  rabii
 * 		Put several  debugging printfs under MACH_DEBUG and made 
 *		comments indicating the conditions they reveal are not illegal 
 *		(rabii)
 * 
 * 	Revision 1.17  92/04/07  13:45:36  pjg
 * 		Moved handling of creds port reference counting from 
 * 		fsvr_thread_terminate to the cred service, which now
 * 		keeps a manual count of the send rights per cache entry and
 * 		will only decrement them evey MAX_CPORT_REF times or at
 * 		exit time. (rabii)
 * 
 * 		Also removed the mig stubs from this file and put them
 * 		in cred_servers.c. And fixed some printfs (rabii)
 * 
 * Revision 1.20  92/05/01  07:49:32  roman
 * Change interfaces to credentials_allocate() and credentials_migrate_in()
 * 	(and thus also to the common subroutine credentials_allocate_body())
 * 	to allow the full setup of a credentials structure during the
 * 	call, rather than having to do a subsequent credentials_set_state()
 * 	call.
 * Correct some minor typos.
 * 
 * Revision 1.19  92/04/14  10:47:13  roman
 * Fix the way panic messages are spread over two lines.
 * Fix dyslexic comment.
 * 
 * Revision 1.18  92/04/06  19:08:36  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 1.16  92/04/05  17:08:02  pjg
 * 	Revision 1.17  92/03/27  17:44:47  roman
 * 	Delete the task parameter from credentials_allocate_body(),
 * 		credentials_migrate_in(), and credentials_allocate().
 * 	Add a task parameter to credentials_set_state() and pass the task
 * 		parameter on to the cache code via the cache_update() RPC.
 * 	Put the code in credentials_deallocate_body() closer to the way
 * 		it was in the prior release with respect to the handling
 * 		of the CM_MIGRATE case (the last fix was found to be
 * 		incorrect conceptually).
 * 	Fix the assertion in credentials_deallocate_body() to check the
 * 		"oldcport" parameter for non-NULL rather than the "cport"
 * 		parameter.
 * 
 * 	Revision 1.16  92/03/27  11:41:25  roman
 * 	Change the CM_MIGRATE action of credentials_deallocate_body() to 
 *		deallocate all the client caches (must be this way until 
 *		Mach tasks can truly migrate).
 * 	Fix coding error in deallocate_caches() for the CM_MIGRATE case.
 * 	Write the new routine credentials_migrate_in().
 * 
 * Revision 1.15  92/03/20  11:31:04  pjg
 * 	Moved node_lookup to fsvr_port.c and renamed to node_to_fileserver_port
 * 	(pjg).
 * 	Enable exit processing (loverso).
 * 
 * Revision 1.14  92/03/15  14:30:58  roy
 * 	Added caching back and re-did master entry handling (rabii)
 * 
 * Revision 1.11  92/02/12  15:10:40  pjg
 * 	Added interfaces needed for TNC to move credentials from one node
 * 	to another during migrate/rexec.
 * 	Change code to not keep around extra send right on the server side all
 * 	the time (roman@locus).
 * 
 * Revision 1.10  92/01/16  17:25:09  roy
 * 	Eliminate MAP_UAREA ifdefs.
 *
 * Revision 1.9  92/01/14  11:31:05  roy
 * 	Simplified credentials service for now (no caching).
 *
 * Revision 1.8  92/01/09  15:58:46  roy
 * 	Set p->p_cred to null in deallocate_credentials().
 *
 * Revision 1.7  92/01/05  20:22:49  roy
 * 	Remove temporary setting of cdir and rdir in uarea_register.
 * 	Unlock properly in cs_master_rusage_info.
 * 	Don't destroy proc's p_rcred reference count in uarea_register.
 *
 * Revision 1.6  91/12/18  16:42:19  roy
 * 	Make types proper.
 *
 * Revision 1.5  91/12/13  10:21:52  roy
 * 	Removed rlimit from master_rusage_info (rabii)
 *
 * Revision 1.4  91/12/08  09:37:16  rabii
 * 	Re-wrote from scratch to implement the real credentials service
 *
 * Revision 1.3  91/11/22  15:20:30  rabii
 * 	locus merge
 * 	pgid and sid added to credentials service for more complete handling
 * 	of controlling terminals. (chrisp)
 *
 * Revision 1.2  91/10/04  15:18:45  chrisp
 * Change credentials_uarea_register to correctly deal with a failure
 * of the message to the credentials port. This should deallocate the
 * credentials port, even of the port is now a dead name (as could
 * potentially happen). Add the credentials port as a parameter to
 * credentials_uarea_deregister(). Have credentials_uarea_deregister()
 * do the deallocation of the credentials port.
 *
 * Revision 1.1  91/09/17  09:18:50  sjs
 * Initial revision	roman
 * New file for handling credentials ports.
 *
 */

#include <sys/proc.h>
#include <mach/mach_port.h>
#include <uxkern/bsd_types.h>
#include <cthreads.h>
#include <uxkern/credentials.h>
#include <mach/norma_special_ports.h>

extern node_t	this_node;
int		ops_in_progress = 0;    /* debug: # fsvr ops in progress */
int		master_entries_inuse = 0; /* debug counts number of currently
					   * zalloced master entries 
					   */   
int		master_callers_inuse = 0;
int		cache_entries_inuse = 0;
queue_head_t	master_hash_queue[HASH_Q_NO];
queue_head_t	cache_hash_queue[HASH_Q_NO];
zone_t		cred_master_entry_zone;
zone_t		cred_master_caller_zone;
zone_t		cred_cache_entry_zone;
struct mutex	master_global_lock;
struct mutex	cache_global_lock;
struct condition	cache_entries;
extern mach_port_t	ux_server_port_set;
mach_port_t	node_to_fileserver_port();
mach_port_t	credentials_allocate();
struct condition	master_entries;

/*
 * Routine to initialize master server
 */
cm_init()
{
	int	i;

	/* Initialize hash queues */
	for (i = 0; i < HASH_Q_NO; i++) {
		queue_init(&master_hash_queue[i]);
	}

	/* Initialize master global lock and condition */
	mutex_init(&master_global_lock);
	condition_init(&master_entries);

	/* Initilalize expandable master zones */
	cred_master_entry_zone = zinit(sizeof(struct master_entry),
				4*vm_page_size, 0, "cred_master_entry");

	cred_master_caller_zone = zinit(sizeof(struct master_servers),
				3*vm_page_size, 0, "cred_master_caller_entry");
}

/*
 * Routine to allocate a new master entry. 
 *
 * A global lock is used to insure integrity of hash queues, while a
 * per entry lock is used to protect individual entries. This routine
 * will always  return an individually locked entry.
 *
 */
struct master_entry *
cred_master_allocate_entry(cport)
mach_port_t	cport;
{
	struct master_entry	*mep = NULL;
	queue_head_t		*qp;

	qp = &master_hash_queue[CPORT_HASH(cport)];
	ZALLOC(cred_master_entry_zone, mep, struct master_entry *);
	if (mep == NULL) {
		panic("cred_master_allocate_entry: No more entries");
	}

	/*
	 * Keep track of number of zalloced entries
	 */
	master_entries_inuse++;

	/*
 	 * Initialize entry
 	 */
	bzero((caddr_t)mep, sizeof(struct master_entry));
	queue_init(&mep->serversq);
	mep->cport = cport;
	mep->nsvr = 0;
	mep->flag = CM_BUSY;

	/*
	 * enqueue entry on a hash queue
	 */
	mutex_lock(&master_global_lock);
	enqueue(qp, &mep->hlink);

	/*
	 * We now drop hash queue lock and retrun an individually
	 * locked entry
	 */
	mutex_unlock(&master_global_lock);
	return(mep);
}

/*
 * Routine to unlock, and deallocate an exisiting master entry. 
 * 
 * The routine will remove the entry from it's hash queue, unlock, and free 
 * the entry and return.
 */
void
cred_master_deallocate_entry(mep)
struct master_entry	*mep;
{
	queue_head_t		*qp;

	/*
	 * The entry is to be dequeued and freed. We will: 
	 * 1) Find the hash queue
	 * 2) Get global master lock
	 * 3) Remove entry from queue
	 * 4) Free the entry and decrement number of in use entries
	 */
	qp = &master_hash_queue[CPORT_HASH(mep->cport)];
	mutex_lock(&master_global_lock);
	remqueue(qp, &mep->hlink);
	mep->flag = CM_FREE;
	ZFREE(cred_master_entry_zone, mep);
	master_entries_inuse--;
	condition_broadcast(&master_entries);
	mutex_unlock(&master_global_lock); 
}

/*
 * Routine to lookup an exisiting master entry. 
 *
 * A global lock is used to insure integrity of hash queues, while a
 * per entry lock is used to protect individual entries. This routine
 * will always  return an individually locked entry.
 *
 */
struct master_entry *
cred_master_lock_entry(cport, nowait)
mach_port_t	cport;
int		nowait;
{
	struct master_entry	*mep = NULL;
	struct master_entry	*tmep = NULL;
	queue_head_t		*qp;

	qp = &master_hash_queue[CPORT_HASH(cport)];

	/* 
	 * Look in the hash queues using cport as an index
	 */
	mutex_lock(&master_global_lock);
retry:
	tmep = (struct master_entry *) queue_first(qp);
	while (!queue_end(tmep, (struct master_entry *) qp)) {
		if (tmep->cport == cport) {
			/*
			 * Found it, if it is marked exiting, then and the
			 * caller asked to notwait, we tell the caller that
			 * the entry is being deallocated, so it does
			 * not have to wait. This breaks a deadlock involving
			 * ISC. Otherwise, now try to lock it
			 */
			if (tmep->flag == CM_EXITING && nowait == TRUE) {
				mep = (struct master_entry *)NULL;
				break;
			}
			if (tmep->flag == CM_BUSY) {
				/* have to wait for per entry lock */
				condition_wait(&master_entries, 
							&master_global_lock);
				goto retry;
			} else {
				mep = tmep;
				mep->flag = CM_BUSY;
				break;
			}
		}
		tmep = (struct master_entry *)queue_next(&tmep->hlink);
	}

	/*
	 * We now drop hash queue lock and return an individually
	 * locked entry
	 */
	mutex_unlock(&master_global_lock);
	return(mep);
}

/*
 * Unlock, an exisiting master entry. 
 */
#define cred_master_unlock_entry(mep)				\
	{							\
		mutex_lock(&master_global_lock);		\
		mep->flag = CM_FREE;				\
		condition_broadcast(&master_entries); 		\
                mutex_unlock(&master_global_lock);              \
	}
		
	

/*
 * Routine to register a cache in a master entry
 */
void
cred_master_addcaller(mep, node)
	struct master_entry	*mep;
	node_t			node;
{
	struct master_servers		*callerp;
	mach_port_t		server_port;

	server_port = node_to_fileserver_port(node);
	ZALLOC(cred_master_caller_zone, callerp, struct master_servers *);
	if (callerp == (struct master_servers *) NULL) {
		panic("master_cred: cred_master_addcaller"
				"unable to allocate server entry");
	}
	master_callers_inuse++ ;
	callerp->fileserver_port = server_port;
	callerp->node = node;
	enqueue(&mep->serversq, &callerp->link);
	mep->nsvr++;
}

/*
 * Routine to contact caches currently caching this entry
 * This is dual purpose:
 * action == CM_GETRUSAGE means collect rusage from all caches
 * action == CM_UPDATE means push new information to all caches
 *
 * In both cases, all cache entries remain vaild
 */
void
contact_caches(mep, action)
	struct master_entry	*mep;
	int			action;
{
	struct master_servers	*sp;
	queue_head_t		*qp;
	kern_return_t		ret;
	int			blkin, blkout;

	qp = &mep->serversq;
	sp = (struct master_servers *)queue_first(qp);
	while (!queue_end(qp, (struct queue_entry *) sp)) {
		if (action == CM_GETRUSAGE) {
			/* RPC's require informing the ux_server_loop module:
			 */
			ux_server_thread_blocking();
			ret = cache_getrusage(sp->fileserver_port, mep->cport, 
						mep->pid, &blkin, 
						&blkout, FALSE);
			ux_server_thread_unblocking();
			if (ret != KERN_SUCCESS) {
				panic("contact_caches: cache_getrusage returns %x",
					ret);
			}
			mep->blkin += blkin;
			mep->blkout += blkout;
		}
		if (action == CM_UPDATE) {
			ret = cache_update(sp->fileserver_port, mep->cport, 
						mep->pid, mep->rcred, 
						mep->cmask, mep->sid, 
						mep->pgid, mep->flimcur, 
						mep->flimmax,
						mep->ruid, mep->rgid,
						mep->task);
			if (ret != KERN_SUCCESS) {
				panic("contact_caches: cache_update returns %x",
					ret);
			}
		}
		sp = (struct master_servers *) queue_next(&sp->link);
	}
}

/*
 * Routine called at exit on migrate time to clean up the cache list
 *
 * This is dual purpose:
 * action == CM_DEALLOCATE means collect rusage from all caches and 
 * tell the caches to invalidate. This may require retrying in cases
 * of busy caches.
 *
 * action == CM_MIGRATE means build a list of caches and pass them to
 * the PM. Do not actually contact any caches since a new master
 * server will now take over.
 *
 */
#define MAX_RETRIES 0
void
deallocate_caches(mep, action, cache_vector)
	struct master_entry	*mep;
	int			action;
	node_t			*cache_vector;
{
	struct master_servers	*sp;
	queue_head_t		*qp;
	int			blkin, blkout;
	kern_return_t		ret;
	node_t			*cvp;
	
	cvp = cache_vector;
	qp = &mep->serversq;
	while (!queue_empty(qp)) {
		sp = (struct master_servers *)queue_first(qp);
		if (action == CM_MIGRATE) {
			/*
			 * Here we should build the mig vec
			 */
			*cvp++ = sp->node;
			remqueue(qp, &sp->link);
			ZFREE(cred_master_caller_zone, sp);
			mep->nsvr--;
			master_callers_inuse --;
		} else {
			/*
			 * Here we call each cache
			 *
			 * (RPC's require informing the ux_server_loop module:)
			 */
			ux_server_thread_blocking();
			ret = cache_getrusage(sp->fileserver_port, mep->cport, 
						mep->pid, &blkin, 
						&blkout, TRUE);
			ux_server_thread_unblocking();
			/*
			 * The server is removed from the queue when the call 
			 * to caches succeeds. However if the call fails due
			 * to a busy cache, then we re-try
			 */
			if (ret == KERN_SUCCESS) {
				mep->blkin += blkin;
				mep->blkout += blkout;
				remqueue(qp, &sp->link);
				ZFREE(cred_master_caller_zone, sp);
				mep->nsvr--;
				master_callers_inuse --;
			} else {
				panic("deallocate_caches: cache_getrusage returns %x",
					ret);
			}
		}
	}
	/*
	 * When we get here, there should be no more servers left
	 */
	if (mep->nsvr != 0) {
		panic("deallocate_caches: missing server");
	}
}

/*******  Procedure-based Master Server Interfaces *******/

/*
 * Called by process manager to allocate a cred port.
 */
mach_port_t
credentials_allocate_body(pid, pgid, sid, cred, cmask, flim_cur, flim_max, 
		ruid, rgid, task, action, oldcport, cache_vector, size)
	pid_t			pid;
	pid_t			pgid;
	pid_t			sid;
	struct ucred		*cred;
	int			cmask;
	unsigned int		flim_cur;
	unsigned int		flim_max;
	uid_t			ruid;
	uid_t			rgid;
	task_t			task;
	int			action;
	mach_port_t 		oldcport;
	node_t			*cache_vector;	/* caches holding coprt */
	int			size;		/* elements in cache_vector */
{
	mach_port_t 		cport;
	kern_return_t 		ret;
	struct master_entry	*mep;
	int			i;

	if (action == CM_MIGRATE) {
		/* make sure a valid cache_vector is passed in */
		ASSERT(oldcport != MACH_PORT_NULL);
		ASSERT(cache_vector != (node_t *) NULL);
		ASSERT(size >= 0);
		cport = oldcport;
	} else {
		/* Allocate a credential port */
		ret = mach_port_allocate(mach_task_self(), 
					MACH_PORT_RIGHT_RECEIVE, &cport);
		if (ret != KERN_SUCCESS)
			panic("credentials_allocate_body: "
			      "unable to allocate new credentials port: %x",
			      ret);
	}
	ret = mach_port_insert_right(mach_task_self(), cport,
				     cport, MACH_MSG_TYPE_MAKE_SEND);
	if (ret != KERN_SUCCESS)
		panic("credentials_allocate_body: "
		      "unable to insert send right in new credentials port: %x",
		      ret);

	/*
	 * Allocate a locked entry (panic if an entry is not available)
	 */
	mep = cred_master_allocate_entry(cport);

	/*
	 * Next we insert the Rcv rt into the master port set.
	 */
	ret = mach_port_move_member(mach_task_self(), cport,
				    ux_server_port_set);
	if (ret != KERN_SUCCESS)
		panic("credentials_allocate_body: unable move new port to set: %x",
			ret);

	/*
	 * Initialize entry from the parameters.
	 */
	mep->pid = pid;
	mep->pgid = pgid;
	mep->sid = sid;
	bcopy(cred, &mep->rcred, sizeof(struct ucred));
	mep->cmask = cmask;
	mep->flimmax = flim_max;
	mep->flimcur = flim_cur;
	mep->ruid = ruid;
	mep->rgid = rgid;
	mep->task = task;

	/*
	 * TNC: need to fill in the cache list
	 */
	if (action == CM_MIGRATE) {
		for (i = 0; i < size; i++ ) {
			cred_master_addcaller(mep, cache_vector[i]);
		}
	}

	/*
	 * Set the cred port in the proc structure.  It is used to
	 * identify the master entry on future calls into the credentials
	 * serveice.
	 */
	cred_master_unlock_entry(mep);

	return(cport);
}

mach_port_t
credentials_allocate(pid, pgid, sid, cred, cmask, flim_cur, flim_max, 
			ruid, rgid, task)
	pid_t			pid;
	pid_t			pgid;
	pid_t			sid;
	struct ucred		*cred;
	int			cmask;
	unsigned int		flim_cur;
	unsigned int		flim_max;
	uid_t			ruid;
	uid_t			rgid;
	task_t			task;
{
	/*
	 * Tell the allocator to get a new cport
	 */
	return(credentials_allocate_body(pid, pgid, sid, cred, cmask, 
					flim_cur, flim_max, 
					ruid, rgid, task,
					!CM_MIGRATE, MACH_PORT_NULL, 
					(node_t *) NULL, 0));
}

mach_port_t
credentials_migrate_in(pid, pgid, sid, cred, cmask, flim_cur, flim_max, 
		ruid, rgid, task, cport, cache_vector, size)
	pid_t			pid;
	pid_t			pgid;
	pid_t			sid;
	struct ucred		*cred;
	int			cmask;
	unsigned int		flim_cur;
	unsigned int		flim_max;
	uid_t			ruid;
	uid_t			rgid;
	task_t			task;
	mach_port_t		cport;
	node_t			*cache_vector;
	int			size;
{
	/*
	 * Tell the allocator to use the supplied port
	 */
	return(credentials_allocate_body(pid, pgid, sid, cred, cmask, 
					flim_cur, flim_max, 
					ruid, rgid, task, 
					CM_MIGRATE, cport,
					cache_vector, size));
}

/*
 * Called by process manager to deallocate a cred port and setup
 * for exit or migration.
 * If action == CM_EXIT, call caches, invalidate caches entires,
 * and accumulate rusage
 * If action == CM_MIGRATE, make a list of caches and pass them
 * back to the caller
 */
kern_return_t
credentials_deallocate_body(cport, blkinp, blkoutp, action, cache_vector, sizep)
	mach_port_t		cport;
	int			*blkinp;
	int			*blkoutp;
	int			action;
	node_t			*cache_vector;
	int			*sizep;
{
	struct master_entry 	*mep;
#ifdef	MACH_DEBUG
	mach_port_urefs_t	refs;
#endif	/* MACH_DEBUG */
	kern_return_t		ret;

	/*
	 * Locate the locked entry (this will panic if entry is not found)
	 */
	mep = cred_master_lock_entry(cport, FALSE);
	if (mep == (struct master_entry *) NULL) {
		panic("credentials_deallocate_body: no entry found");
	}

	/*
	 * If action == CM_MIGRATE, this is a migrate call
	 */
	if (action == CM_MIGRATE) {
		/* Make sure caller provided all required parameter */
		ASSERT(cache_vector != (node_t *) NULL);
		ASSERT(sizep != (int *) NULL);

		/* 
		 * check to see if the size of provided node array is enough
		 * if not, return an error to caller who will then pass in
		 * a bigger one
		 */
		if (mep->nsvr > *sizep) {
			*sizep = mep->nsvr;	/* tell caller required size */
			cred_master_unlock_entry(mep);
			return(EINVAL);
		}
	} else {
		mep->flag = CM_EXITING;
	}

	/*
	 * Handle disposition of port.  First remove the Rcv rt from the
	 * port set. 
	 */
	ret = mach_port_move_member(mach_task_self(), cport, MACH_PORT_NULL);
	if (ret != KERN_SUCCESS) {
		panic("credentials_deallocate_body: unable to remove port from set: %x",
			ret);
	}

	if (action == CM_MIGRATE) {
		/*
		 * Here we want to somehow pass the list of
	 	 * caches to PM.
		 */
		*sizep = mep->nsvr;	/* tell caller real size */
		deallocate_caches(mep, CM_MIGRATE, cache_vector);
		ret = mach_port_mod_refs(mach_task_self(), cport,
					MACH_PORT_RIGHT_SEND, -1);
		if (ret != KERN_SUCCESS)
			panic("credentials_deallocate_body: "
			      "mach_port_get_refs returned 0x%x", ret);
	} else {
		/*
		 * Contact caches and ask them for rusage
		 */
		deallocate_caches(mep, CM_DEALLOCATE, (node_t *)NULL);

		/*
		 * Accumulate rusage information.
		 */
		*blkinp += mep->blkin;
		*blkoutp += mep->blkout;

#ifdef	MACH_DEBUG
		/*
		 * Do a sanity check to make sure no more send right exists
		 */
		ret = mach_port_get_refs(mach_task_self(), cport,
					MACH_PORT_RIGHT_SEND, &refs);
		if (ret != KERN_SUCCESS) {
			panic("credentials_deallocate_body: returned 0x%x"
					"mach_port_get_refs", ret);
		}

		/*
		 * This check may fail because the emulator still
		 * has send rights to this port and it may send 
		 * requests to server(s) while another thread is 
		 * exiting. Note that we hold one  send right to
		 * the port.
		 */
		if (refs > 1) {
				printf("credentials_deallocate_body: send "
					"right count > 0\n");
		}
#endif	/* MACH_DEBUG */

		/*
		 * Deallocate the recieve right
		 */
		ret = mach_port_destroy(mach_task_self(), cport);
		if (ret != KERN_SUCCESS) {
			 panic("credentials_deallocate_body: returned %x"
					"mach_port_destroy", ret);
		}
	}

	/*
	 * Unlock and free the master entry
	 */
	cred_master_deallocate_entry(mep);
	return(KERN_SUCCESS);
}

kern_return_t
credentials_deallocate(cport, blkinp, blkoutp)
	mach_port_t	cport;
	int		*blkinp;
	int		*blkoutp;
{
	return(credentials_deallocate_body(cport, blkinp, blkoutp, 
					CM_DEALLOCATE, (node_t *) NULL, 
					(int *) NULL));
}

/*
 * Called by TNC code to collect state info on a cport and
 * move them to another "master" site. To be used in conjunction
 * with credentials_migrate_in.
 */
kern_return_t
credentials_migrate_out(cport, cache_vector, sizep)
	mach_port_t	cport;
	node_t		*cache_vector;
	int		*sizep;
{
	return(credentials_deallocate_body(cport, (int *)NULL, (int *)NULL, 
					CM_MIGRATE, cache_vector, sizep));
}

/*
 * Called by process manager to gather rusage
 */
kern_return_t
credentials_get_rusage(cport, blkinp, blkoutp)
	mach_port_t	cport;
	int		*blkinp;
	int		*blkoutp;
{
	struct master_entry	*mep;

	/*
	 * Locate the locked entry 
	 */
	mep = cred_master_lock_entry(cport, FALSE);
	if (mep == (struct master_entry *) NULL) {
		panic("credentials_get_rusage: no entry found");
	}

	/*
	 * Call up each cache and update it's info
	 */
	(void) contact_caches(mep, CM_GETRUSAGE);

	/*
	 * Pass rusage infromation
	 */
	*blkinp = mep->blkin;
	*blkoutp = mep->blkout;

	/*
	 * Unlock master entry
	 */
	cred_master_unlock_entry(mep);

	return(KERN_SUCCESS);
}

/*
 * Called by process manager to update credentials information.
 */
kern_return_t
credentials_set_state(cport, pgidp, sidp, credp, cmaskp, 
		flim_curp, flim_maxp, ruidp, rgidp, taskp)
	mach_port_t		cport;
	pid_t			*pgidp;
	pid_t			*sidp;
	struct ucred		*credp;
	int			*cmaskp;
	unsigned int		*flim_curp;
	unsigned int		*flim_maxp;
	uid_t			*ruidp;
	uid_t			*rgidp;
	task_t			*taskp;
{
	struct master_entry	*mep;

	/*
	 * Locate the locked entry (this will panic if entry is not found)
	 */
	mep = cred_master_lock_entry(cport, FALSE);
	if (mep == (struct master_entry *) NULL) {
		panic("credentials_set_state: no entry found");
	}

	/*
	 * Modify the master's state for changed fields only
	 */
	if (credp != (struct ucred *) NULL) {
		bcopy(credp, &mep->rcred, sizeof(struct ucred));
	}
	if (sidp != (pid_t *) NULL) {
		mep->sid = *sidp;
	}
	if (pgidp != (pid_t *) NULL) {
		mep->pgid = *pgidp;
	}
	if (cmaskp != (int *) NULL) {
		mep->cmask = *cmaskp;
	}
	if (flim_curp != (unsigned int *) NULL) {
		mep->flimcur = *flim_curp;
	}
	if (flim_maxp != (unsigned int *) NULL) {
		mep->flimmax = *flim_maxp;
	}
	if (ruidp != (uid_t *) NULL) {
		mep->ruid = *ruidp;
	}
	if (rgidp != (uid_t *) NULL) {
		mep->rgid = *rgidp;
	}
	if (taskp != (task_t *) NULL) {
		/* Note that we did not acquire another send right */
		mep->task = *taskp;
	}
	
	/*
	 * Call up each cache and update it's info
	 */
	(void) contact_caches(mep, CM_UPDATE);

	cred_master_unlock_entry(mep);

	return(KERN_SUCCESS);
}


/*******  RPC-based Master Server Interfaces *******/

/*
 * Master routine called from cache to request credential information.
 */
kern_return_t
cm_cred_info_request(cport, node, credp, cmaskp, pidp, sidp, pgidp,
		     taskp, flimcur, flimmax, ruidp, rgidp)
	mach_port_t		cport;
	node_t			node;
	cred_t			*credp;
	int			*cmaskp;
	pid_t			*pidp;
	pid_t			*sidp;
	pid_t			*pgidp;
	task_t			*taskp;
	int			*flimcur;
	int			*flimmax;
	uid_t			*ruidp;
	uid_t			*rgidp;
{
	struct master_entry 	*mep;

	/*
	 * Locate the locked entry 
	 */
	mep = cred_master_lock_entry(cport, TRUE);
	if (mep == (struct master_entry *) NULL) {
		/* the entry was deallocated under us, tell the cache */
		return((kern_return_t) CE_GONE);
	}

	/*
	 * Record the cache in the master entry
	 */
	cred_master_addcaller(mep, node);
	bcopy(&mep->rcred, credp, sizeof(struct ucred));
	*cmaskp = mep->cmask;
	*pidp = mep->pid;
	*sidp = mep->sid;
	*pgidp = mep->pgid;
	*taskp = mep->task;
	*flimmax = mep->flimmax;
	*flimcur = mep->flimcur;
	*ruidp = mep->ruid;
	*rgidp = mep->rgid;

	cred_master_unlock_entry(mep);

	return(KERN_SUCCESS);
}


/*******  Cache Server routines *******/

/* 
 * Routine to initialize the credentials server cache
 */
cc_init()
{
	int	i;

	/* Initialize hash queues */
	for (i = 0; i < HASH_Q_NO; i++) {
		queue_init(&cache_hash_queue[i]);
	}

	/* Initialize cache global lock and condition */
	mutex_init(&cache_global_lock);
	condition_init(&master_entries);

	/* Initilalize cache zone and make it expandible */
	cred_cache_entry_zone = zinit(sizeof(struct cache_entry),
				3*vm_page_size, 0, "cred_cache");
}

/*
 * Routine to allocate a new cache entry. 
 * A) First get global lock
 * B) Second look up in the cache, if found set fill_entry to FALSE and
 * return
 * C) Third, if not found in cache, get new entry, enqueue, set 
 * fill_entry to TRUE, and return
 *
 * A global lock is used to insure integrity of hash queues, while a
 * per entry lock is used to protect individual entries. This routine
 * will always  return an individually locked entry.
 */
struct cache_entry *
cred_cache_allocate_entry(cport, fill_entryp)
mach_port_t	cport;
boolean_t	*fill_entryp;
{
	struct cache_entry	*cep = NULL;
	struct cache_entry	*tcep = NULL;
	queue_head_t		*qp;

	qp = &cache_hash_queue[CPORT_HASH(cport)];

	/* 
	 * Look in the hash queues using cport as an index
	 */
	mutex_lock(&cache_global_lock);
retry:
	tcep = (struct cache_entry *) queue_first(qp);
	while (!queue_end(tcep, (struct cache_entry *) qp)) {
		if (tcep->cport == cport) {
			/*
			 * Found it, now try to lock it
			 */
			if (tcep->flag == CC_BUSY) {
				/* have to wait for per entry lock */
				condition_wait(&cache_entries, 
							&cache_global_lock);
				goto retry;
			} else {
				cep = tcep;
				cep->flag = CC_BUSY;
				break;
			}
		}
		tcep = (struct cache_entry *)queue_next(&tcep->hlink);
	}
	if (cep != NULL) {
		*fill_entryp = FALSE;
		/*
		 * now drop hash queue lock and return an individually 
	 	 * locked entry
		 */
		mutex_unlock(&cache_global_lock);
		return(cep);
	}

	/*
	 * We did not find it in the cache, we will now allocate a new one.
	 */

	ZALLOC(cred_cache_entry_zone, cep, struct cache_entry *);
	if (cep == NULL) {
		panic("cred_cache_allocate_entry: No more entries");
	}

	/*
	 * Keep track of number of zalloced entries
	 */
	cache_entries_inuse++;

	/*
	 * Initialize entry
	 */
	bzero((caddr_t)cep, sizeof(struct cache_entry));
	cep->flag = CC_BUSY;
	cep->cport = cport;
	/*
	 * enqueue entry on a hash queue
	 */
	enqueue(qp, &cep->hlink);

	/*
	 * We now drop hash queue lock and return an individually
	 * locked entry
	 */
	mutex_unlock(&cache_global_lock);
	*fill_entryp = TRUE;
	return(cep);
}

/*
 * Routine to to unlock, and deallocate an exisiting cache entry. 
 * 
 * The routine will remove the entry from it's hash queue, unlock, and free 
 * the entry and return.
 */
void
cred_cache_deallocate_entry(cep)
struct cache_entry	*cep;
{
	queue_head_t		*qp;

	/*
	 * The entry is to be dequeued and freed. We will: 
	 * 1) Find the hash queue
	 * 2) Get global master lock
	 * 3) Remove entry from queue
	 * 4) Free the entry and decrement number of in use entries
	 */
	qp = &cache_hash_queue[CPORT_HASH(cep->cport)];
	mutex_lock(&cache_global_lock);
	remqueue(qp, &cep->hlink);
	cep->flag = CC_FREE;
	ZFREE(cred_cache_entry_zone, cep);
	cache_entries_inuse--;
	condition_broadcast(&cache_entries);
	mutex_unlock(&cache_global_lock);
}

/*
 * Routine to lookup an exisiting cache entry. 
 *
 * A global lock is used to insure integrity of hash queues, while a
 * per entry lock is used to protect individual entries. This routine
 * will always  return an individually locked entry.
 *
 */
struct cache_entry *
cred_cache_lock_entry(cport)
mach_port_t	cport;
{
	struct cache_entry	*cep = NULL;
	struct cache_entry	*tcep = NULL;
	queue_head_t		*qp;

	qp = &cache_hash_queue[CPORT_HASH(cport)];

	/* 
	 * Look in the hash queues using cport as an index
	 */
	mutex_lock(&cache_global_lock);
retry:
	tcep = (struct cache_entry *) queue_first(qp);
	while (!queue_end(tcep, (struct cache_entry *) qp)) {
		if (tcep->cport == cport) {
			/*
			 * Found it, now try to lock it
			 */
			if (tcep->flag == CC_BUSY) {
				/* have to wait for per entry lock */
				condition_wait(&cache_entries, 
							&cache_global_lock);
				goto retry;
			} else {
				cep = tcep;
				cep->flag = CC_BUSY;
				break;
			}
		}
		tcep = (struct cache_entry *)queue_next(&tcep->hlink);
	}

	/*
	 * We now drop hash queue lock and return an individually
	 * locked entry
	 */
	mutex_unlock(&cache_global_lock);
	return(cep);
}

/*
 * Unlock, and drop the reference on an exisiting cache entry. 
 */
#define cred_cache_unlock_entry(cep) 				\
		{						\
			mutex_lock(&cache_global_lock);		\
			cep->flag = CC_FREE;			\
			condition_broadcast(&cache_entries);	\
			mutex_unlock(&cache_global_lock);	\
		}

/*******  Procedure-based Cache Server Interfaces *******/

/*
 * Called by fileserver to initialize a proc structure given a cred port.
 * Must be called in conjunction with credentials_deregister.
 */
kern_return_t
credentials_register(cport, p)
	mach_port_t		cport;
	struct proc		*p;
{
	kern_return_t 		ret;
	struct ucred		tcred;
	struct cache_entry	*cep;
	boolean_t		fill_entry;

	/*
	 * Get a global lock, look in the cache, if found, then
	 * release global lock and return setting fill_entry to FALSE,
	 * otherwise allocate a new one, release global lock and return 
	 * setting fill_entry to TRUE
	 */
	cep = cred_cache_allocate_entry(cport, &fill_entry);
	if (fill_entry == FALSE) {
		goto copy;
	}

	/*
	 * Now bump up the send right count by one to keep
	 * port alive while being cached
	 */
	ret = mach_port_mod_refs(mach_task_self(), cport,
				MACH_PORT_RIGHT_SEND, +1);
	if (ret != KERN_SUCCESS) {
#ifdef	MACH_DEBUG
		/*
		 * This is not illegal since a thread exiting could have
		 * deallocated the send right already
		 */
		printf("credentials_register: unable to bump cport ref");
#endif	/* MACH_DEBUG */
		goto bad;
	}
	/* Update send right counter */
	cep->cport_ref++;

	/*
	 * Contact the master for cred information 
	 */
	ux_server_thread_blocking();
	ret = cred_info_request(cport, this_node, &tcred, &cep->cmask, 
				&cep->pid, &cep->sid, &cep->pgid, 
				&cep->task, &cep->flimcur, &cep->flimmax,
				&cep->ruid, &cep->rgid);
	ux_server_thread_unblocking();

	if (ret != KERN_SUCCESS) {
#ifdef	MACH_DEBUG
		/*
		 * This is not illegal since a thread exiting could have
		 * deallocated the master entry already
		 */
		printf("credentials_register: cred_info_request ret = 0x%x\n",
						ret);
#endif	/* MACH_DEBUG */
		goto bad;
	}
	cep->blkin = cep->blkout = 0;
	cep->rcredp = crdup(&tcred);

copy:
	/*
	 * Copy the info from the cache entry to the proc structure.
	 */
	ASSERT(p->p_rcred == NOCRED);
	p->p_rcred = cep->rcredp;
	crhold(p->p_rcred);
	p->p_cred = cport;
	p->p_pid = cep->pid;
	p->p_sid = cep->sid;
	p->p_pgid = cep->pgid;
	p->p_ruid = cep->ruid;
	p->p_rgid = cep->rgid;
	p->p_task = cep->task;
	p->p_utask.uu_cmask = cep->cmask;
	p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_cur = cep->flimcur;
	p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_max = cep->flimmax;

	ops_in_progress++;  	/* debug */
	cep->ref++;
	cep->cport_ref++;
	cred_cache_unlock_entry(cep);
	return(KERN_SUCCESS);
bad:
	/* free up the entry */
	cred_cache_deallocate_entry(cep);
	return(EINTR);
}


/*
 * Called by fileserver to return information to the cred cache.
 * Must be called in conjunction with credentials_register.
 */
void
credentials_deregister(p, error)
	struct proc		*p;
	int			error;
{
	struct cache_entry	*cep;
	kern_return_t 		ret;

	cep = cred_cache_lock_entry(p->p_cred);
	if (cep == (struct cache_entry *) NULL) 
		panic("credentials_deregister: entry not found");

	/* accumulate rusage */
	cep->blkin += p->p_utask.uu_ru.ru_inblock;
	cep->blkout += p->p_utask.uu_ru.ru_oublock;

	/* cleanup dummy proc */
	p->p_cred = MACH_PORT_NULL;

	/* drop reference on dummy proc's ucred */
	crfree(p->p_rcred);
	ops_in_progress--;  	/* debug */
	cep->ref--;
	if (error) {
		cep->cport_ref--;
	}

	/* Make sure the refs do not wrap */
	if (cep->cport_ref > MAX_CPORT_REF) {
		ret = mach_port_mod_refs(mach_task_self(), cep->cport,
					MACH_PORT_RIGHT_SEND,
					-MAX_CPORT_REF);
		if (ret != KERN_SUCCESS)
			printf("credentials_deregister: cport 0x%x\n"
					"deallocate ret = ", ret);
		cep->cport_ref -= MAX_CPORT_REF;
	}

	/* 
	 * there may be cc_cache_getrusage waiting for op termination 
	 * if so, we wake it up now
	 */
	if (cep->ref == 0) {
		wakeup((caddr_t)&cep->ref);
	}
	cred_cache_unlock_entry(cep);
}

/*
 * Give away a send right on the credentials port
 */
void
credentials_get_send_right(cport)
	mach_port_t		cport;
{
	kern_return_t 		ret;
	struct cache_entry	*cep;

	cep = cred_cache_lock_entry(cport);
	if (cep == (struct cache_entry *)NULL) {
		panic("credentials_get_send_right: entry not found");
	}

	/*
	 * Just give up one of the send rights we already have.
	 * If we are on our last one, cons some new ones up.
	 */
	if (--cep->cport_ref == 0) {
		/*
		 * Now bump up the send right count by one to keep
		 * port alive while being cached
		 */
		ret = mach_port_mod_refs(mach_task_self(), cport,
					MACH_PORT_RIGHT_SEND, +10);
		if (ret != KERN_SUCCESS) {
			panic("credentials_get_send_right: unable to mod refs: %x",
				ret);
		}
		/* Update send right counter */
		cep->cport_ref += 10;
	}

	cred_cache_unlock_entry(cep);
	return;
}
/*******  RPC-based Cache Server Interfaces *******/

/*
 * Routine called by master to update a cache's information regarding
 * an entry. 
 * "cport" indentifies the cache entry.
 * "pid" is used as a sanity check since it cannot change to insure 
 * proper mapping of cport to a cache structure.
 */
kern_return_t
cc_cache_update(port, cport, pid, rcred, cmask, sid, pgid, flimcur, flimmax,
		ruid, rgid, task)
	mach_port_t	port;
	mach_port_t	cport;
	pid_t		pid;
	cred_t		rcred;
	int		cmask;
	pid_t		sid;
	pid_t		pgid;
	int		flimcur;
	int		flimmax;
	uid_t		ruid;
	uid_t		rgid;
	task_t		task;
{
	struct cache_entry	*cep;
	kern_return_t		ret;

	cep = cred_cache_lock_entry(cport);
	if (cep == (struct cache_entry *)NULL)
		panic("cc_cache_update: entry not found");
	
	/* sanity check to make sure pid did not change */
	if (cep->pid != pid) {
		panic("cc_cache_update: pid mismatch");
	}

	crfree(cep->rcredp);
	cep->rcredp = crdup((struct ucred *) &rcred);
	cep->cmask = cmask;
	cep->sid = sid;
	cep->pgid = pgid;
	cep->flimcur = flimcur;
	cep->flimmax = flimmax;
	cep->ruid = ruid;
	cep->rgid = rgid;
	(void) mach_port_deallocate(mach_task_self(), cep->task);
	cep->task = task;

	cred_cache_unlock_entry(cep);

	/*
	 * Drop extra cport right just obtained in this call
	 */
	ret = mach_port_deallocate(mach_task_self(), cport);
	if (ret != KERN_SUCCESS)
		printf("cc_cache_update: port_deallocate fails ret = 0x%x\n",
						ret);

	return(KERN_SUCCESS);
}
	
/*
 * Routine called by master to request a cache's rusage
 * information regarding an entry. 
 * "cport" indentifies the cache entry.
 * "pid" is used as a sanity check since it cannot change to insure 
 * proper mapping of cport to a cache structure.
 */
kern_return_t
cc_cache_getrusage(port, cport, pid, blkin, blkout, deallocate)
	mach_port_t	port;
	mach_port_t	cport;
	pid_t		pid;
	int		*blkin;
	int		*blkout;
	int		deallocate;
{
	struct cache_entry	*cep;
	kern_return_t		ret;

retry:
	cep = cred_cache_lock_entry(cport);
	if (cep == (struct cache_entry *)NULL)
		panic("cc_cache_getrusage: entry not found");

	/* sanity check to make sure pid did not change */
	if (cep->pid != pid) {
		panic("cc_cache_getrusage: pid mismatch");
	}

	/* 
	 * If there are operations in progress, signal the op
	 * to exit, and re-try.
	 */
	if ((deallocate == TRUE) && (cep->ref > 0)) {
		extern int hz;

#ifdef DEBUG
printf("DBG: cred in use, calling oip_intr_all(%x)\n", cport);
#endif

		cred_cache_unlock_entry(cep);
		(void)oip_intr_all(cport);

		/*
		 * Either wait until cep->ref woken or 1 second
		 */
		tsleep((caddr_t)&cep->ref, PZERO-1,
			"cc_cache_getrusage", hz);
		goto retry;
	}

	/* Record rusage info, and reset cache entry's rusage */
	*blkin = cep->blkin;
	*blkout = cep->blkout;
	cep->blkin = 0;
	cep->blkout = 0;
	if (deallocate == TRUE) {
		/*
		 * Deallocate the task port obtained in 
		 * credentials_uarea_register.
		 */
		ret = mach_port_deallocate(mach_task_self(), cep->task);
		if (ret != KERN_SUCCESS)
			printf("cc_cache_getrusage: task port 0x%x deallocate "
					"fail, ret=0x%x\n", cep->task, ret);

		/*
		 * Deallocate extra rights on cport obtained during
		 * calls to the cache
		 */
		ret = mach_port_mod_refs(mach_task_self(), cep->cport,
					MACH_PORT_RIGHT_SEND,
					-(cep->cport_ref + 1));
		if (ret != KERN_SUCCESS)
			printf("cc_cache_getrusage: creds port 0x%x deallocate "
					"fail, ret=0x%x\n", cep->cport, ret);

		/*
		 * Free the cred structure
		 */
		crfree(cep->rcredp);
		cred_cache_deallocate_entry(cep);
		return(KERN_SUCCESS);
	} else {
		cred_cache_unlock_entry(cep);
		/*
		 * Drop extra cport right just obtained in this call
		 */
		ret = mach_port_deallocate(mach_task_self(), cport);
		if (ret != KERN_SUCCESS)
			printf("cc_cache_getrusage: port_deallocate 0x%x\n"
						"ret = ", ret);
		return(KERN_SUCCESS);
	}
}
