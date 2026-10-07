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
 * @OSF_COPYRIGHT@
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: subr_select.c,v $
 * Revision 1.16  1995/02/01  21:28:56  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.15  1994/11/18  20:27:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/08/31  22:46:34  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.12.2.1  1994/08/10  18:43:38  slk
 * Changed the assert from PTS# 10508 (NFS EAT perturbed an assert
 * in server/bsd/subr_select.c) to a panic in order to catch more
 * information.  This assert happens during the first run of NFS
 * EATS after an OS installation.  This change only affects servers
 * built with MACH_ASSERT on.
 *
 *  NOT A FIX -- ADDITION OF SIMPLE INSTRUMENTATION ONLY.
 *                 The failing assert is replaced by a panic containing much
 *                 more info.  This panic is only compiled in if assertions
 *                 are enabled.
 * Reviewer(s): Bob Yasi, Surender Brahmaroutu
 * Risk: Low
 * Testing: NFS EATS.
 * Module(s): server/bsd/subr_select.c
 *
 * Revision 1.12  1994/05/04  21:38:04  mjl
 * TNC select rewrite.  Code protected by "(... & SQ_VSOCK)" is done only
 * for TNC network virtual sockets.  The drp now held in a "selid/drp map
 * entry" that appears on the primary socket's node only ("havedrp",
 * SQ_DRP flag set).  Primary vs.  secondary socket functionality is
 * separated out using #ifdef's and movement of code to subroutines
 * callable (indirectly) from vsoo_select().
 *
 *  Reviewer: Charlie Johnson (Intel), Bob Yasi (Locus)
 *  Risk: Medium
 *  Benefit or PTS #: #7537 + select rewrite
 *  Testing: VSX, EATS, bobtest, Eval
 *  Module(s):
 * 	server/bsd/subr_select.c
 * 	server/sys/select.h
 * 	server/sys/socketvar.h
 * 	server/sys/user.h
 * 	server/tnc/un_debug.c
 * 	server/tnc/un_debug.h
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/bsd_server_side.c
 * 	server/uxkern/fsvr.defs
 * 	server/uxkern/fsvr2_server_side.c
 * 	server/uxkern/fsvr_port.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/port_hash.c
 * 	server/uxkern/port_hash.h
 * 	server/vsocket/mi_config.c
 * 	server/vsocket/sys_vsocket.c
 * 	server/vsocket/two_way_hash.h
 * 	server/vsocket/vs.defs
 * 	server/vsocket/vs_chouse.c
 * 	server/vsocket/vs_debug.c
 * 	server/vsocket/vs_init.c
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_subr.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vsocket.h
 *
 * Revision 1.11  1994/01/13  17:52:41  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.10  1993/09/29  20:52:42  jlitvin
 * Finish removing the ux_server_thread_blocking/unblocking pairs around
 * fsvr_file_unref_svr.
 *
 * Revision 1.9  1993/09/15  16:07:22  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.8.2.1  1993/09/15  16:03:56  cfj
 * Fix for PTS bug #6623.  Remove the #if FULLSERVER from select_wakeup()
 * and select_wakeup_poll() since they are required for LITE server.
 *
 * Revision 1.8  1993/09/01  01:34:37  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.7  1993/08/04  03:54:22  cfj
 * 08-03-93 Code drop from Locus.
 *
 * Revision 1.6  1993/07/19  22:58:44  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.5  1993/07/14  17:49:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.4  1993/05/06  19:05:23  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  18:49:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:05:23  nandy
 * ad103+tnc merged with Intel code.
 *
 * Initial 1.0.3 code drop
 *
 * Revision 1.2.8.1  1993/04/04  01:20:19  cfj
 * Yet another select fix. (bhk@locus.com)
 *
 * Revision 2.18  93/08/11  08:53:42  bhk
 * Removed the ux_server_thread_blocking/unblocking pairs around 
 * fsvr_file_unref_svr
 * 
 * Revision 2.17  93/08/03  16:57:09  mjl
 * Put SELQDEBUG(...) around a printf.  Some temporary d-n related debug code.
 * 
 * Revision 2.16  93/07/31  15:28:12  mjl
 * For TNC the file struct associated with the select queue's file port may
 * be on another node, so use an RPC to decrement the corresponding f_svrsend
 * file reference. (mjl for bhk)
 * 
 * Revision 2.15  93/07/13  15:56:28  slively
 *
 *      Revision 2.14  93/06/29  16:18:16  rabii
 *      Lite server mods (rabii)
 *
 * Revision 2.14  93/06/29  15:34:40  bhk
 * Partial update to OSF/1 AD V1.05b1 to fix select problems
 * 
 * Revision 2.13  93/06/16  13:50:48  klh
 * 	Revision 2.11  93/05/26  14:42:09  loverso
 * 		Release the FP_REF associated with the file port send right.
 * 		(loverso)
 * 
 * Revision 2.12  93/04/03  11:46:37  klh
 * Add user delay port in select_deadname()
 * 
 * Revision 1.2  1992/11/30  22:16:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.11  1992/07/26  17:18:19  bhk
 * Revision 2.12  1993/04/03  11:46:37  klh
 * Add user delay port in select_deadname()
 *
 * Revision 2.11  92/07/26  17:18:19  bhk
 * Added support for TNC for selecting remote sockets
 * 
 * Revision 2.10  92/06/22  12:11:47  bhk
 * Added data structures for remote select
 * 
 * Revision 2.9  92/05/31  18:58:25  loverso
 * 	Revision 2.8.1.4  92/05/29  16:00:36  loverso
 * 	Allow SELQDEBUG to be enabled at runtime.
 * 
 * 	Revision 2.8.1.3  92/05/26  15:50:10  loverso
 * 	Massive cleanup to drop dead-name notification while the port is not
 * 	in the select queue.  This closes race between dead-name and wakeup.
 * 	Move most common processing into new "select_cleanup()" function.
 * 
 * 	Revision 2.8.1.2  92/05/25  21:58:25  loverso
 * 	Cleanup.  Use file_port for dead-name notification, not fileserver_port.
 * 
 * 	Revision 2.8.1.1  92/05/21  15:14:12  loverso
 * 	Revert to using file ports instead of file pointers, to support
 * 	file structure migration.
 * 
 * Revision 2.8  92/05/13  14:08:55  loverso
 * 	Break out code for dead-name notification deletion and delay port
 * 	destruction.  Remove unused mpportid_* macros.  Fix comments.
 * 
 * Revision 2.7  92/05/12  00:24:59  loverso
 * 	Disable debugging output.
 * 
 * Revision 2.6  92/05/12  00:06:26  loverso
 * 	Rework all select queue handling.  Add dead-name processing.
 * 	This requires other changes in bsd_server_side.c and fsvr_port.c.
 * 
 * Revision 2.5  92/03/15  14:39:27  roy
 * 	Added new routine for streams (rabii)
 * 
 * Revision 2.4  91/10/04  14:51:52  chrisp
 *	Change subr_select() to not use the credentials port any more, and to
 *	store the process's ucred structure instead.
 * 
 * Revision 2.3  91/09/16  15:44:17  rabii
 * 	Merge of V2.0 and Locus (locus check-in by yazz)
 * 	Major changes due to select operation being done in emulator.
 * 
 * Revision 2.2  91/08/31  13:23:16  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/05  13:54:23  sp
 *	Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  13:18:58  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:59:13  gm]
 * 
 * Revision 1.4  90/06/22  20:06:49  devrcs
 * 	Select event is now in thread structure.
 * 	[90/06/18  16:58:48  gmf]
 * 
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Remove pageable flag from zinit argument list	jvs@osf.org
 * 	Higher routines dequeue and zfree elements.	sue@encore.com
 * 	Created.					alan@encore.com
 * 	[90/06/12  19:07:22  gmf]
 * 
 * $EndLog$
 */
/*
 * Mach Operating System
 * Copyright (c) 1988 Encore Computer Corporation
 * All rights reserved.
 */

/*
 * subr_select.c:  a multi-threaded object select implementation.
 * Each object (e.g., socket or tty) includes a select queue, upon
 * which a thread may insert an entry describing the event awaited.  When
 * the object becomes active, its queue is traversed and a routine is
 * called that will send to the emulator the delayed reply message that
 * it is awaiting.
 *
 * Important Note:  the select queues themselves provide no mutual
 * exclusion.  The caller of these subroutines must use a lock to
 * guarantee mutual exclusion to the queue being manipulated.
 */

/*
 * NOTE on implementation:
 *	The object passed to the routines below is really a "queue_t" and
 *	NOT a "sel_queue_t".  This is OK, as we only the next and prev
 *	pointers within the top object.  This means that "sel_queue_t"
 *	MUST start with a "queue_t".
 */

#ifndef OSF1_SERVER
#include <mach_debug.h>		/* XXX */
#endif

#include <fullserver.h>
#include <sys/unix_defs.h>
#include <sys/proc.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/user.h>
#include <sys/file.h>
#include <kern/event.h>
#ifndef OSF1_SERVER
#include <kern/thread.h>
#endif
#include <kern/zalloc.h>
#ifndef OSF1_SERVER
#include <mach/vm_param.h>
#endif
#include <kern/queue.h>

#include <uxkern/port_hash.h>			/* ugly */
#include <uxkern/syscall_subr.h>		/* ugly */

#ifdef	TNC
#include <vsocket/vsocket.h>
#include <sys/socketvar.h>
#include <vsocket/vs_subr.h>			/* no beauty queen either */
#endif

zone_t			select_zone;
extern int		select_max_elements;
extern int		select_chunk_elements;
extern node_t		this_node;

#define	SELECT_DEBUG	MACH_ASSERT
#if	SELECT_DEBUG
simple_lock_data_t	select_element_count_lock;
int			select_element_count;
static void		increment_select_element_count();
static void		decrement_select_element_count();
#define	SELDEBUG(s)	s
#else
#define	SELDEBUG(s)
#endif

#define	SELQDEBUG	MACH_ASSERT
#ifndef SELQDEBUG
#define SELQDEBUG(s)
#else
#undef SELQDEBUG
#define SELQDEBUG(s) if (selqdebug) s
int selqdebug = 0;
#endif

/*
 * Macros to handle hash table for delayed replay ports
 *
 * NOTE: It is no longer allowed for a selq to have the same port enqueued
 * on it more than once.  Some OSF/1 1.0 code did call select_enqueue() more
 * than once for the same selq.  This was at the least wasteful, and more
 * possibly incorrect.  Enqueuing more than once is now a mortal syn.
 * (Such code has been updated to resemble 1.1 in this regard).
 */
#define SEL_PORT_HASHSZ 	63

#define SEL_PORT_HASHINIT()						\
		sel_port_hash_table = mpport_hash_init(SEL_PORT_HASHSZ)
#define SEL_PORT_ENTER(port, v)						\
		mpport_hash_enter(sel_port_hash_table, (port), (char *)(v))
#define SEL_PORT_REMOVE(port)						\
		mpport_hash_remove(sel_port_hash_table, (port))
#define SEL_PORT_LOOKUP(port)						\
		mpport_hash_lookup_lock(sel_port_hash_table, (port))
#define SEL_PORT_UNLOCK(port)						\
		mpport_hash_unlock(sel_port_hash_table, (port))

mpport_hash_table_t sel_port_hash_table;


void
select_init()
{
	if (select_max_elements < 1024)
		panic("select_init:  select_max_elements %d\n",
		      select_max_elements);
	if (select_chunk_elements < 128)
		panic("select_init:  select_chunk_elements %d\n",
		      select_chunk_elements);
	select_zone = zinit(sizeof(struct sel_queue),
		round_page(select_max_elements * sizeof(struct sel_queue)),
		round_page(select_chunk_elements * sizeof(struct sel_queue)),
		"Select queues");
#if	SELECT_DEBUG
	simple_lock_init(&select_element_count_lock);
	select_element_count = 0;
#endif

	SEL_PORT_HASHINIT();
}


/*
 *  This requests deadname notification for a select delayed reply port.
 *  Formerly this code was inline in select_enqueue(), but TNC needs to
 *  set up deadname notification somewhat differently.
 */
boolean_t
select_request_deadname(
	mach_port_t	delay_port,
	mach_port_t	file_port)
{
	kern_return_t	ret;
	mach_port_t	previous = MACH_PORT_NULL;

	/*
	 * Once entered, a dead-name can be processed.
	 * We do this before requesting the notification, so that we don't
	 * lose it.
	 */
	if (!SEL_PORT_ENTER(delay_port, file_port))
		panic("select_request_deadname: SEL_PORT_ENTER");

	/*
	 * Arrange for a 'dead-name' notification on drp TO THE FILE PORT.
	 * Since sync==0, this can NOT happen immediately, as we'd rather
	 * do clean up here.
	 */
	ret = mach_port_request_notification(mach_task_self(),
		delay_port, MACH_NOTIFY_DEAD_NAME, 0,
		file_port, MACH_MSG_TYPE_MAKE_SEND_ONCE,
		&previous);
	/*
	 * This means delay_port no longer represents a right;
	 * a dead-name has already been generated!
	 */
	if (ret == KERN_INVALID_ARGUMENT || ret == KERN_INVALID_NAME) {
		SELQDEBUG(printf("select_request_deadname(drp=%x fp=%x): "
				 "req_not(%x)=%d (DEAD)\n",
				 delay_port, file_port, ret));
		ASSERT(previous == MACH_PORT_NULL);

		/*
		 * back off what work we've done
		 */
		if (!SEL_PORT_REMOVE(delay_port))
			panic("select_request_deadname: SEL_PORT_REMOVE");
		return FALSE;

	} else if (ret != KERN_SUCCESS || previous != MACH_PORT_NULL) {
		panic("select_request_deadname(drp=%x fp=%x): req_not(%x)=%d "
		      "(FAILED) prev=%x\n",
			delay_port, file_port, ret, previous);
	}
	return TRUE;
}


#ifdef	TNC
/*
 *  Primary socket TNC code needs this to handle remote secondaries
 *  properly.  I am too lazy to move the SEL_* macros to a shared
 *  include file....
 */
int
select_sel_port_remove(
	mach_port_t	delay_port)
{
	return SEL_PORT_REMOVE(delay_port);
}
#endif	/* TNC */

/*
 * This is used to remove the dead-name notification on the delay reply port
 * The dead-name notification request is canceled to avoid the generation
 * of a port-deleted notification on the send-once right given for the
 * dead-name message.
 *
 * Returns TRUE if d-n notification was canceled (and send-once right on
 * the file_port reaped).
 * Returns FALSE if the drp was already a dead-name.
 */
boolean_t
select_cancel_deadname(delay_port)
	mach_port_t	delay_port;
{
	mach_port_t	previous = MACH_PORT_NULL;
	kern_return_t	ret;
	boolean_t	result;

	ret = mach_port_request_notification(mach_task_self(),
		delay_port, MACH_NOTIFY_DEAD_NAME, 0,
		MACH_PORT_NULL, MACH_MSG_TYPE_MAKE_SEND_ONCE,
		&previous);
	switch (ret) {
	case KERN_SUCCESS:
		result = TRUE;
		/*
		 * Must clean up send-once right on file port that was given
		 * for notification message.
		 */
		(void)mach_port_deallocate(mach_task_self(), previous);
		break;

	case KERN_INVALID_ARGUMENT:
		result = FALSE;
		/*
		 * Delay_reply_port already a dead-name!
		 * assume d-n processing will deallocate the other uref
		 */
		break;

	default:
#ifdef	TNC
		{ selid_drp_map_t *sdm;
		  DRP_SELID_LOOKUP(delay_port, sdm);
		  print_port_info(delay_port, "drp");
		  panic("select_deallocate_delayport(%x): req_not=%d (FAILED) "
			"prev=%x sdm=0x%x\n",
			delay_port, ret, previous, sdm);
		}
		/*NOTREACHED*/
#endif	/* TNC */
		panic("select_deallocate_delayport(%x): req_not=%d (FAILED) prev=%x\n",
			delay_port, ret, previous);
	}
	return result;
}


/*
 * Free state associated with a selq entry.  Caller is presumed to be
 * holding queue lock.
 *
 * This drops the d-n notification request.
 * If the port has become a d-n or `drop' is TRUE (we are dropping the entry),
 * then 1 uref and the file port are de-ref'd.
 *
 * If `wakeup' is TRUE, then we are in wakeup doing basic house cleaning.
 * (i.e., DO NOT free the creds port unless the drp is a d-n).
 *
 * Returns TRUE if we dropped the drp before it became a d-n.
 * Returns FALSE if drp was a dead-name.
 */
boolean_t
select_cleanup(qp, drop, wakeup)
	sel_queue_t	*qp;
	boolean_t	drop;
	boolean_t	wakeup;
{
	kern_return_t	ret;
	boolean_t	wasdead;	/* TRUE if drp was a d-n */
#ifdef	TNC
	struct uthread	*uth = current_thread();
	boolean_t	havedrp = TRUE;
	selid_drp_map_t	*sdm;

	if (qp->selq_flags & SQ_VSOCK) {
		/*
		 *  Remove select id from svs hash table, as it's no longer
		 *  queued.  We only clean up after the drp if we've got one
		 *  (we won't if this was queued on a remote secondary socket.)
		 *  Note VS_SECNDRY_LOCK is held until after SEL_PORT table is
		 *  also updated.
		 */
		ASSERT(drop == FALSE);	/* sel_dq_all not called for sockets */
		VS_SECNDRY_LOCK;
		SELID_TO_SVS_REMOVE(qp->selq_sel_id);
		if ((qp->selq_flags & SQ_DRP) == 0) {
			/*
			 *  On remote secondaries, select_cleanup()
			 *  should set wasdead to clean up iff this is
			 *  a cleanup/dequeue call. If on the other
			 *  hand it's an actual wakeup, don't pretend
			 *  it's dead---if you do, the primary will
			 *  never find out about the event!
			 */
			wasdead = !wakeup;
			havedrp = FALSE;
			goto skip_drp_cleanup;
		}
	}
#endif	/* TNC */

	/*
	 * This will prevent any processing on a dead-name for this port.
	 */
	if (!SEL_PORT_REMOVE(qp->selq_delay_port))
		panic("select_cleanup(%x): SEL_PORT_REMOVE", qp);

	wasdead = !select_cancel_deadname(qp->selq_delay_port);

#ifdef	TNC
skip_drp_cleanup:
	if (qp->selq_flags & SQ_VSOCK) {
		if (havedrp && wasdead) {
			uth->uu_sel_flags |= SQ_DEAD;
			/*
			 *  If wakeup, we've won a race against a
			 *  deadname notification.  However, we're
			 *  being called from a thread that won't be
			 *  doing a FOP_SELECT/cleanup.  In fact,
			 *  since we've just called SEL_PORT_REMOVE,
			 *  no d-n msg can ever be relied on to do a
			 *  cleanup either.  So we call scrub_remote_selects()
			 *  to do it ourselves.
			 */
			if (wakeup) {
				DRP_SELID_LOOKUP(qp->selq_delay_port, sdm);
				if (sdm) {
					sdm->sdm_flags |= SDM_DEAD;
					ret = r_vs_scrub_remote_selects(
						    (mach_port_t) sdm->sdm_so,
						    qp->selq_creds_port,
						    qp->selq_delay_port);
					if (ret != KERN_SUCCESS)
					    panic("selclnup: scrub: kr 0x%x\n",
						  ret);
					SELID_DRP_LOOKUP_DONE(sdm);
				}
			}
		}
		VS_SECNDRY_UNLOCK;
	}
#endif
	if (drop || wasdead) {
		SELQDEBUG(printf("select_cleanup(%x): already d-n %x\n",
			qp, qp->selq_delay_port));

#if	TNC
		if (havedrp == FALSE) {
			ret = KERN_SUCCESS;
		} else
#endif
		ret = mach_port_deallocate(mach_task_self(),
				qp->selq_delay_port);
		if (ret != KERN_SUCCESS) {
		    SELQDEBUG(
			printf("select_cleanup(%x): deallocate(d-n %x)=%d\n",
			       qp, qp->selq_delay_port, ret));
		}
		/*
		 * Release the file port and file ref
		 */
#ifdef	TNC
		if (havedrp == FALSE) {
			/* MiG takes care of ux_server_{un,}blocking() calls */
			ret = fsvr_file_unref_svr(qp->selq_file_port,
						  qp->selq_creds_port,
						  qp->selq_file_port,
						  -1);
			if (ret != KERN_SUCCESS)
				SELQDEBUG(printf(
			       "select_cleanup: fsvr_file_unref_svr: kr 0x%x\n",
					 ret));
		} else
#endif
		fp_unref_port((struct file*)qp->selq_file_port, -1);
	}

	if (!wakeup)
		(void)mach_port_deallocate(mach_task_self(),
				qp->selq_creds_port);

	return !wasdead;
}


/*
 * Check to see if the port is in the hash queue of registered entries.
 * If so, arrange for the selq entry to be dequeued.
 *
 * Return TRUE if we handled the port.
 */
boolean_t
select_deadname(drp)
mach_port_t	drp;
{
	boolean_t	ret;
	mach_port_t	file_port;
	struct file	*fp;
	struct uthread	*uth = current_thread();
	boolean_t	dummy;
	short events = POLLIN|POLLOUT|POLLERR|POLLHUP|POLLNVAL|POLLPRI;
#ifdef	TNC
	selid_drp_map_t	*sdm;
#endif

	SELQDEBUG(printf("select_deadname(%x)\n", drp));

#ifdef TNC
	/*
	 *  First check for drp in the selid/drp map, taking a ref on
	 *  the entry.  We must do this before SEL_PORT_LOOKUP, to
	 *  prevent the map entry from going away between
	 *  SEL_PORT_LOOKUP and the call to FOP_SELECT().
	 *
	 *  be a select on a TNC virtual socket.  (If we can't
	 *  find one, it may still be such---but we have lost
	 *  a race against sbsd_sel_poll_reply().)
	 */
	DRP_SELID_LOOKUP(drp, sdm);
	if (sdm != NULL) {
		/* This is a TNC network virtual socket. */
		uth->uu_sel_id = sdm->sdm_selid;
		uth->uu_sel_flags = (SQ_VSOCK|SQ_DEAD|SQ_DRP);
	} else {
		/* May be a vsocket, but we lost race vs. ..._reply(). */
		uth->uu_sel_id = SEL_ID_NULL;
		uth->uu_sel_flags = 0;
	}
#endif

	/*
	 * Iff SEL_PORT_LOOKUP finds the entry, it will return with the
	 * hash queue locked.
	 */
	if (file_port = (mach_port_t)SEL_PORT_LOOKUP(drp)) {
		SELQDEBUG(printf("select_deadname(%x) file_port=%x\n",
			drp, file_port);)

		/*
		 * We have the hash queue locked.  We take an ADDITIONAL
		 * FP_REF on the file port to hold for the duration
		 * of our work (via PORT_TO_FILE_LOOKUP) and then unlock
		 * the hash queue.
		 * This assures us that the file remains open.
		 * We then reap the select queue entry, and free our
		 * new reference.
		 */
		PORT_TO_FILE_LOOKUP(file_port, fp);
		if (fp == (struct file *)0) {
			SEL_PORT_UNLOCK(drp);
			return FALSE;
		}

		SEL_PORT_UNLOCK(drp);

		/*
		 * select_dequeue will want these.
		 */
		uth->uu_sel_delay_port = drp;
		uth->uu_sel_index = 0;

		/*
		 * select_dequeue will notice (via select_cleanup) that drp
		 * is a d-n and drop the appropriate port references.
		 *
		 * If there is a race between the generation of a dead-name
		 * and a call to select_wakeup, the one with the SEL_PORT_LOCK
		 * wins.
		 */
		FOP_SELECT(fp, &events, &dummy, 0, dummy);

		/* Drop our new reference; this could close the file */
		FILE_LOOKUP_DONE(fp);

		ret = TRUE;
	} else {
		ret = FALSE;
	}

#ifdef	TNC
	/* Drop ref on the selid/drp entry; may scrub it from the map */
	if (sdm)
		SELID_DRP_LOOKUP_DONE(sdm);
#endif
	return ret;
}


/*
 * select_enqueue:  add the current thread to the list
 * of threads waiting for something to happen.
 *
 * This inherits an extra send right on the file port and an extra
 * file ref.  These are passed onto the queued structure.
 *
 * N.B.:  The lock protecting this queue must be held
 * while calling this routine.
 */
void
select_enqueue(selq)
queue_t		selq;
{
	sel_queue_t		*qp;
	struct uthread		*uth = current_thread();
	mach_port_t		previous = MACH_PORT_NULL;
	kern_return_t		ret;
	int			isdead;
#ifdef	TNC
	struct socket		*so;
#endif

	/*
	 * select is just polling; we have no work to do.
	 * In fact, we shouldn't even have been called.
	 */
	if (uth->uu_sel_delay_port == MACH_PORT_NULL) {
		SELQDEBUG(printf("select_enqueue(%x) on MACH_PORT_NULL\n",
			selq);)
		return;
	}
	ASSERT(uth->uu_sel_index != -1);		/* erroneous */

	/* get an empty selq element to fill */
	qp = (sel_queue_t *) zalloc(select_zone);

	SELQDEBUG(printf("select_enqueue(%x) drp=%x qp=%x fp=%x\n",
		selq, uth->uu_sel_delay_port, qp, uth->uu_sel_file_port);)

	/*
	 * We store the file port.  Note that this is the *send* right
	 * we are holding onto; it was created by the original MiG RPC
	 * and only released at the end of sbsd_sel_poll_reply().
	 */
	qp->selq_file_port = uth->uu_sel_file_port;
#ifdef TNC
	qp->selq_sel_id = uth->uu_sel_id;
	qp->selq_flags = uth->uu_sel_flags;
#endif

	/*
	 * We snarf the creds_port, and also make sure we have a send
	 * right to go along with it.
	 */
	qp->selq_creds_port = uth->uu_procp->p_cred;
	credentials_get_send_right(qp->selq_creds_port);

	qp->selq_index = uth->uu_sel_index;
	qp->selq_events = uth->uu_sel_events;

	/*
	 * The send-once right is now owned by qp.
	 */
	qp->selq_delay_port = uth->uu_sel_delay_port;

#ifdef	TNC
	if (uth->uu_sel_flags & SQ_VSOCK) {
		/*
		 *  If the select id is already in the secondary
		 *  virtual socket map, we don't need to requeue.
		 *  (This is normal: if select_wakeup() is called
		 *  on one of several secondaries but no event
		 *  has occurred, the select is requeued for all
		 *  secondaries, not just the one that woke up.)
		 *  Otherwise store the select id in the svs map to
		 *  indicate it is queued.  Don't release the
		 *  VS_SECNDRY_LOCK until the SEL_PORT_* hash table
		 *  is also updated!
		 */
		VS_SECNDRY_LOCK;
		SELID_TO_SVS_LOOKUP(uth->uu_sel_id, so);
		if (so != NULL) {
			/* Select id is already queued! */
#if MACH_ASSERT
#ifdef	NOTDEF
			ASSERT(so == selq2so(selq));
#else
			/* MJL */
			if (so != selq2so(selq)) {
				selid_drp_map_t	*sdm;
				SELID_DRP_LOOKUP(uth->uu_sel_id, sdm);
				panic("select_enqueue(selq=0x%x): sid=0x%x is "
				      "already queued for so=0x%x, not 0x%x! "
				      "(sdm=0x%x)\n",
				      selq, uth->uu_sel_id, so, selq2so(selq),
				      sdm);
			}
#endif
#endif
			VS_SECNDRY_UNLOCK;
			zfree(select_zone, (vm_offset_t) qp);
			return;
		}

		SELID_TO_SVS_ENTER(qp->selq_sel_id, selq2so(selq));

		/*
		 *  Only set up dead-name mechanism if this is the primary
		 *  socket, i.e. SQ_DRP is set indicating we have a real
		 *  delayed reply port that can go dead.
		 */
		if ((uth->uu_sel_flags & SQ_DRP) == 0)
			goto skip_deadname_setup;

		/*
		 *  If called from a remote reply, d-n notification
		 *  is still on (the select_cleanup() on the remote
		 *  node couldn't turn it off here on the primary).
		 *  Don't request d-n notification twice!
		 */
		if (uth->uu_sel_flags & SQ_REMOTE_REPLY)
			goto skip_deadname_setup;
	}
#endif	/* TNC */

	/* Set up deadname notification for drp on the file port */
	isdead = !select_request_deadname(qp->selq_delay_port,
					  qp->selq_file_port);
	if (isdead) {

		/* Remove the new creds send right */
		(void)mach_port_deallocate(mach_task_self(),
			qp->selq_creds_port);

		zfree(select_zone, (vm_offset_t) qp);
#ifdef	TNC
		if (uth->uu_sel_flags & SQ_VSOCK) {
			uth->uu_sel_flags |= SQ_DEAD;
			SELID_TO_SVS_REMOVE(uth->uu_sel_id);
			VS_SECNDRY_UNLOCK;
		}
#endif
		/* wish we could return an explicit error here */
		return;
	}

#ifdef	TNC
 skip_deadname_setup:
	if (uth->uu_sel_flags & SQ_VSOCK) {
		VS_SECNDRY_UNLOCK;
	}
#endif

	/*
	 * This does TWO things:
	 * 1. assures that a driver cannot (errantly) call select_enqueue()
	 *    twice for the same request.
	 * 2. signals bsd_sel_poll_delay that select_enqueue() was actually
	 *    called.  Thus, that routine can decide whether or not it needs
	 *    to free port rights.  Once we do this, WE take responsibility
	 *    for all port rights (and the extra FP ref) passed into here.
	 *
	 * You might consider this gross.
	 */
	uth->uu_sel_delay_port = MACH_PORT_NULL;

	enqueue_tail(selq, &qp->links);
	SELDEBUG(increment_select_element_count());

	/* wish we could return an error here */
}


/*
 * select_dequeue:  remove the current thread from
 * the list of threads waiting for something to happen.
 *
 * N.B.:  The lock protecting this queue must be held
 * while calling this routine.
 * N.B.:  We can't assume that the element won't be re-
 * allocated as soon as it is zfree'd.
 */
void
select_dequeue(selq)
queue_t		selq;
{
	register sel_queue_t	*qp;
	register sel_queue_t	*next;
	struct uthread		*uth = current_thread();

	/*
	 * select was just polling; we have no work to do.
	 */
	if (uth->uu_sel_delay_port == MACH_PORT_NULL) {
		SELQDEBUG(printf("select_dequeue(%x) on MACH_PORT_NULL\n",
			selq);)
		return;
	}
	ASSERT(uth->uu_sel_index != -1);		/* ditto */

	qp = (sel_queue_t *) queue_first(selq);
	while (!queue_end(&qp->links, selq)) {
#ifdef	TNC
		if (((qp->selq_flags & SQ_VSOCK)
		     ? (qp->selq_sel_id == uth->uu_sel_id)
		     : (qp->selq_delay_port == uth->uu_sel_delay_port))) {
#else
		if (qp->selq_delay_port == uth->uu_sel_delay_port) {
#endif
			remqueue(selq, &qp->links);
			next = (sel_queue_t *) queue_next(&qp->links);

			SELQDEBUG(printf("select_dequeue(%x) drp=%x qp=%x\n",
				selq, qp->selq_delay_port, qp);)

			/*
			 * We can't deallocate the drp or the file_port
			 * reference, because there is a chance that this
			 * will be requeued or the drp used; thus we pass
			 * FALSE for "drop".
			 *
			 * However, if the drp has become a dead-name, then
			 * select_cleanup will drop the references on it and
			 * the file_port there.
			 *
			 * NOTE: when we are called from select_deadname(),
			 * this is where the last uref for drp is dropped.
			 */
			(void)select_cleanup(qp, FALSE, FALSE);
#ifdef	TNC
			if (qp->selq_flags & SQ_VSOCK)
				uth->uu_sel_flags |= SQ_DEQUEUED;
#endif
			zfree(select_zone, (vm_offset_t) qp);
			SELDEBUG(decrement_select_element_count());
			qp = next;
		}
		else
			qp = (sel_queue_t *) queue_next(&qp->links);
	}
}


void
select_dequeue_all(selq)
queue_t		selq;
{
	register sel_queue_t	*qp;

	while (!queue_empty(selq)) {
		qp = (sel_queue_t *) dequeue_head(selq);

#ifdef	TNC
		ASSERT(qp->selq_flags == 0 && qp->selq_sel_id == 0);
#endif
		/*
		 * Unlike select_dequeue, we assume that all entries
		 * are being totally discarded.
		 */
		(void)select_cleanup(qp, TRUE, FALSE);

		zfree(select_zone, (vm_offset_t) qp);
		SELDEBUG(decrement_select_element_count());
	}
}


/*
 * select_wakeup:  Cause a server thread to handle the actual select event
 * at a later time.
 *
 * N.B.:  The lock protecting this queue must be held
 * while calling this routine.
 */
void
select_wakeup(selq)
queue_t		selq;
{
	register sel_queue_t	*qp;

	while (!queue_empty(selq)) {
		qp = (sel_queue_t *) dequeue_head(selq);

		/*
		 * If we didn't mind port-deleted and send-once notifications,
		 * this wouldn't be so complicated
		 *
		 * If we get back FALSE, then drp is a dead-name, and we've
		 * nothing more to do.
		 */
		if (!select_cleanup(qp, FALSE, TRUE)) {
			SELQDEBUG(printf("select_wakeup(%x) qp=%x drp=%x D-N\n",
				selq, qp, qp->selq_delay_port));
			/* drp was a d-n and was dropped */
		} else {
			/*
			 * This calls sbsd_sel_poll_reply() in the context of
			 * a new thread by sending back to the file_port.
			 *
			 * We hand off responsibility for the second file
			 * port and second file ref to sel_poll_reply.
			 */
#ifdef TNC
			struct socket *tmp = (struct socket *)selq2so(selq);
#endif /* TNC */
			SELQDEBUG(printf("select_wakeup(%x) qp=%x drp=%x\n",
				selq, qp, qp->selq_delay_port);)
			(void) ubsd_sel_poll_reply(qp->selq_file_port,
						qp->selq_file_port, 
						qp->selq_creds_port, 
						qp->selq_delay_port,
#ifdef TNC
						qp->selq_sel_id,
						this_node,   
#endif
						qp->selq_index,
						qp->selq_events);
		}
		SELDEBUG(decrement_select_element_count());
		zfree(select_zone, (vm_offset_t) qp);
	}
}

/*
 * select_wakeup_poll: Cause a server thread to handle the actual poll 
 * event at a later time. If poll_event is non NULL, then the routine
 * checks to only handle events the process asked for.
 * 
 * N.B.:  The lock protecting this queue must be held
 * while calling this routine.
 */
void
select_wakeup_poll(selq, poll_event)
queue_t		selq;
int		poll_event;
{
	register sel_queue_t	*qp;
	kern_return_t		ret;

	while (!queue_empty(selq)) {
		/* do we want to always remove this entry ? */
		qp = (sel_queue_t *) dequeue_head(selq);

		/*
		 * If we get back FALSE, then drp is a dead-name, and we've
		 * nothing more to do.
		 */
		if (!select_cleanup(qp, FALSE, TRUE)) {
			SELQDEBUG(printf("select_wakeup_poll(%x) qp=%x drp=%x D-N\n",
				selq, qp, qp->selq_delay_port));
			/* drp was a d-n and was dropped */
		} else {
			/*
			 * If poll_event is non NULL, Only inform process,
			 * if it asked for the specific event otherwise 
			 * inform the process no matter what
			 *
			 * We hand off responsibility for the second file
			 * port and second file ref to sel_poll_reply.
			 */
			if (poll_event == NULL || 
			    ((poll_event & (qp->selq_events | POLLERR)) || 
			     (poll_event == POLLHUP && 
			      (qp->selq_events & POLLOUT)))) {
				(void) ubsd_sel_poll_reply(qp->selq_file_port,
						qp->selq_file_port,
						qp->selq_creds_port,
						qp->selq_delay_port,
#ifdef TNC
						qp->selq_sel_id,
						this_node,
#endif
						qp->selq_index,
						qp->selq_events);
			} else {
				/*
				 * We must drop everything else we hold.
				 * This is really the second half of
				 * select_cleanup(), but we can't call
				 * that since we've already dequeued this
				 * entry.
				 */
#ifdef	TNC
				if ((qp->selq_flags & SQ_DRP) == 0) {
					ret = KERN_SUCCESS;
				} else
#endif
				ret = mach_port_deallocate(mach_task_self(),
						qp->selq_delay_port);
				if (ret != KERN_SUCCESS) {
					SELQDEBUG(printf("select_wakeup_poll(%x): deallocate(%x)=%d\n",
						selq,
						qp->selq_delay_port,
						ret));
				}

				(void)mach_port_deallocate(mach_task_self(),
						qp->selq_file_port);
				/*
				 * Release the file port and file ref
				 */
#ifdef	TNC
				if ((qp->selq_flags & SQ_DRP) == 0) {
					ret = fsvr_file_unref_svr(
							  qp->selq_file_port,
							  qp->selq_creds_port,
							  qp->selq_file_port,
							  -1);
					if (ret != KERN_SUCCESS)
						printf(
			"select_wakeup_poll: fsvr_file_unref_svr: kr 0x%x\n",
						       ret);
				} else
#else
				fp_unref_port((struct file*)qp->selq_file_port,
					      -1);
#endif

				(void)mach_port_deallocate(mach_task_self(),
						qp->selq_creds_port);
			}
		}
		SELDEBUG(decrement_select_element_count());
		zfree(select_zone, (vm_offset_t) qp);
	}
        return;
}

#if	SELECT_DEBUG
static void
increment_select_element_count()
{
	simple_lock(&select_element_count_lock);
	++select_element_count;
	simple_unlock(&select_element_count_lock);
}

static void
decrement_select_element_count()
{
	simple_lock(&select_element_count_lock);
	--select_element_count;
	simple_unlock(&select_element_count_lock);
}
#endif
