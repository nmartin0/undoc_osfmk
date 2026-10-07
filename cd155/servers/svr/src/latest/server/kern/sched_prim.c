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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: sched_prim.c,v $
 * Revision 1.11  1994/11/18  20:32:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/10/25  22:32:07  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: Lo
 *  Benefit or PTS #: 11128
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/i386/bsd_machdep.c
 * 	server/i386/slock.s
 * 	server/i860/bsd_machdep.c
 * 	server/i860/slock.s
 * 	server/kern/parallel.h
 * 	server/kern/sched_prim.c
 * 	server/sys/unix_defs.h
 * 	server/tnc/rvp_subr.c
 * 	server/uxkern/cred_servers.c
 * 	server/uxkern/emul_user.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/ux_server_loop.c
 *
 * For assertful servers ($CONFIG contains "test" and "MACH_ASSERT" is defined
 * to be 1 instead of 0), make taking the master lock be a subroutine call,
 * so that more error checking can be done, and so that the routine name
 * will appear in debug tracebacks.
 * For servers with lock debugging ($CONFIG contains "ldebug" and "MACH_LDEBUG"
 * is defined to be 1 instead of 0), do even more master lock consistency
 * checking, including saving the call chain of the last locker and unlocker
 * of the master lock.
 *
 * Revision 1.9  1993/07/14  18:02:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  19:21:15  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/05/06  19:17:08  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:30:57  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.7  1993/04/03  03:06:13  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.6  1993/03/19  14:58:19  cfj
 * Merged from T9.
 *
 * Revision 1.4.6.3  1993/03/19  01:23:54  cfj
 * Fix to new locking code from OSF.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:00:11  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/11/30  22:23:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:25:51  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:22:17  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.13  1993/04/29  14:00:59  klh
 * 	Revision 2.13  93/04/08  11:30:00  loverso
 * 		Fixed the thread_should_halt() handling.
 * 		[1992/09/23  12:30:17  barbou]
 *
 * 		Move thread count manipulations to ux_server_loop.c, where they belong,
 * 		and change the condition_wait to a call to ux_server_thread_suspend.
 * 		[1992/06/12  08:03:05  condict]
 *
 * 		Revision 3.17  92/04/22  16:49:17  barbou
 * 		Fix for bug #133: try to cooperate if the current thread has been
 * 		requested to halt (exiting process).
 *
 * 	Revision 2.12  93/03/22  23:57:42  condict
 * 		Added thread_yield function, for yielding to every other Mach thread
 * 		and every other runnable C-thread (if unwired threads exist).
 *
 * 		Fixed bug in assert_wait(0,...).  It was causing the subsequent
 * 		thread_block to be a no-op, instead of doing a non-event-based
 * 		sleep.  Added uu_state field to tell thread_block whether the
 * 		thread has been "clear_waited" since the assert_wait.
 *
 * Revision 2.12  93/03/22  21:13:25  yazz
 * OSF lock changes.  Implement scheduling of queue of waiting wannabe
 * lock-holders.
 * 
 * Revision 2.11  92/11/09  15:31:22  rabii
 * 	[92/10/29  19:32:51  bhk]
 * 	Fixed a debug message to allow compilation with DEBUG enabled.
 * 
 * Revision 2.10  92/05/31  18:58:47  loverso
 * 	Undid the last change. Keep the master lock as a mutex and if
 * 	MACH_LDEBUG is set define an auxiliary structure master_mutex_aux
 * 	to record who acquired and released the lock (pjg).
 * 
 * Revision 2.9  92/05/27  20:04:15  pjg
 * 	Define the master lock as a simple lock if NCPUS > 1 and MACH_LDEBUG
 * 	to ease debugging of deadlocks in the master lock.
 * 
 * Revision 2.8  92/05/24  14:29:39  pjg
 * 	Took Grenoble's V3.6 version of the file replacing only the history
 * 	comments.
 * 
 * 	Revision 3.16  92/04/03  14:13:47  condict
 * 	Extensive rewrite to re-use more of the OSF/1 IK code, including using
 * 	one lock per wait queue, instead of a single global sleep_lock.
 * 	Change the uniproc case (NCPUS == 1) so that thread_lock/unlock are null
 * 	and it uses the master lock as the lock associated with the condition
 * 	wait/signal.  This gives better performance.
 * 
 * 	Revision 3.15  92/03/24  21:03:41  barbou
 * 	Fix for bug #118: null pointer in thread_block().
 * 
 * 	Revision 3.14  92/03/23  18:03:29  condict
 * 	Bug fix 1: On recursive call to thread_sleep, do cthread_yield instead of
 * 	assert_wait/thread_block.
 * 	Bug fix 2: sleep_lock and thread_lock were both being used by different funcs
 * 	to protect the uu_wait_event and other uthread fields.  Now sleep_lock only
 * 	protects sleep queue and thread_lock protects all other fields.  Also, moved
 * 	curr_ipl to u.uu_ipl.
 * 
 * 	Revision 3.13  92/03/13  15:18:54  condict
 * 	Make assert_wait look more like integrated kernel version.  Panic if
 * 	recursive sleep call (uu_event != 0).  Also, in thread_block, don't call
 * 	Netintr if sleeping on a lock (avoids deadlock in networking).
 * 
 * 	Revision 3.12  92/03/07  18:00:18  sp
 * 	Count interrupts for table(TBL_INTR) (Bug #90)
 * 
 * Revision 2.7  92/05/01  10:27:44  rabii
 * 	Put in fixes for page 0 protection bug from jose
 * 
 * Revision 2.6  92/03/09  14:37:41  durriya
 * 	92/02/28  17:59:52  barbou
 * 	Kind of fix for bug #43: don't return from thread_block() while stopped.
 * 
 * 	92/01/07  23:34:06  condict
 * 	Call Netintr at thread_block and interrupt_exit, if scheduled.
 * 
 * Revision 2.5  91/12/16  20:50:44  roy
 * 	91/10/17  15:45:15  condict
 * 	Get rid of calls to spl_n and ux_server_thread_busy/active in 
 * 	thread_block.  Also, add a non-null version of spl_n (spl_n_lock), 
 * 	for use by imported code that depends on spl's for mutual exclusion 
 * 	(see sys/synch.h).
 * 
 * Revision 2.4  91/12/13  10:11:36  roy
 * 	Add panic to catch sleep queue corruption.
 * 
 * Revision 2.3  91/10/14  12:34:00  sjs
 * 	91/10/01  14:07:53  condict
 * 	Delete all traces of the spl implementation (spls are now a no-op).
 * 
 * 	91/09/27  12:00:55  emcmanus
 * 	Allow assert_wait on event 0 (used by streams code).  Added splstr().
 * 
 * 	91/09/20  21:27:25  barbou
 * 	Fixed previous bug fix: new_level wrong (should be i instead of i-1).
 * 
 * 	91/09/20  17:27:09  condict
 * 	Fix spl_n so that an interrupt handler currently at level n can do:
 * 	spl_n(0); . . . ; spl_n(n) without damage.
 * 
 * Revision 2.2  91/08/31  13:38:11  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/28  14:02:39  condict
 * Update to OSF 1.0.2.
 * 
 * Revision 1.7  90/10/07  13:55:42  devrcs
 * 	Turn off LASSERT of simple locks in thread_block() for now.
 * 	Clean code of non-ANSI-C-isms, add volatile in idle thread().
 * 	Wrap printf's in DEBUG like others in same file. Clean up.
 * 	[90/10/03  16:30:47  tmt]
 * 
 * 	Add and use sleep_stamp to get correct thread sleep times.
 * 	[90/10/03  09:44:11  jvs]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:57:35  gm]
 * 
 * 	Verify that no simple locks are held across a context switch.
 * 	[90/09/28  12:37:34  nags]
 * 
 * Revision 1.6  90/08/24  12:03:58  devrcs
 * 	Added swtch_tsk_ctxt_cnt and swtch_thrd_ctxt_cnt definitions to this
 * 	file to keep track of context switches in a machine indenpendent way.
 * 	Also modified switch_context #defines to perform these counts.
 * 	[90/08/13  23:01:24  knight]
 * 
 * Revision 1.5  90/04/27  19:07:49  devrcs
 * 	Fixed the "zombie" bug (or at least one cause for it.
 * 	Leave "interruptible" state of thread TRUE during all the time
 * 	where it is not explicitly waiting in uninterruptible state.
 * 	[90/04/19  14:55:48  nolting]
 * 
 * Revision 1.4  90/01/02  20:06:14  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.3  89/12/26  09:29:24  gm
 * 	BSD4.4 changes.
 * 	[89/12/25            gmf]
 * 
 * Revision 6.1  89/07/26  16:06:21  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 0.0  89/03/09            dlb
 * 	Always set low hint when adding thread to an empty runq.
 * 	Removed bound processor case of preempt check in thread_setrun.
 * 	[89/03/09            dlb]
 * 
 * Revision 0.0  89/03/01            dlb
 * 	Fix locking in choose_thread().
 * 	[89/03/01            dlb]
 * 
 * Revision 0.0  89/02/09            dlb
 * 	Removed sched_load declaration.  Now in processor_set structure.
 * 
 * 	Revision 2.10.3.1  89/01/30  18:34:53  dlb
 * 	Fix locking in thread_wakeup_one().
 * 	[89/01/25            dlb]
 * 
 * 	Change ticks to unsigned in update_priority. (found by jjc).
 * 	[89/01/07            dlb]
 * 
 * 	Convert to processor allocation logic.
 * 	[88/08/16            dlb]
 * 
 * 	Support ast mechanism for threads.
 * 	[88/08/11            dlb]
 * 
 * 	Added scanner to unstick stuck threads.  Move dispatch acknowledge
 * 	logic to idle_thread().  Replace runrun.  Avoid needless context
 * 	switches.
 * 	[88/08/09            dlb]
 * 
 * 	Rewrote context switch code under FAST_CSW switch.
 * 	[88/05/25            dlb]
 * 
 * 	Revision 2.19  89/10/11  14:24:39  dlb
 * 	Massive rewrite:
 * 
 * 	Interprocessor interrupts to perform ast_check()'s on remote
 * 	       processors for bound threads (XXX except for master).
 * 	Rewrite and clean up context switch code.  Macros to use old
 * 		machine-dependent primitives under !FAST_CSW.
 * 	Change preempt check in thread_setrun() to preempt on any
 * 	       priority difference instead of 2 or greater.
 * 	Kernel monitor support.
 * 	Use task->kernel_vm_space instead of pmap to figure out whether
 * 	       to do PMAP_DEACTIVATE and PMAP_ACTIVATE.
 * 	Add sched_thread() and scanner to unstick stuck threads.
 * 	Rename thread_switch to thread_run to allow use of former name
 * 	       for a trap.
 * 	Support fixed priority policy.
 * 	Set up quantum for dispatched threads in idle_thread.
 * 	Convert scheduler priorities to 0-31 from 0-127.
 * 	HW_FOOTPRINT: dispatch to last processor a thread ran on if
 * 	       possible in thread_setrun().
 * 	Change ticks to unsigned in update_priority. (found by jjc).
 * 	Convert to processor allocation logic.
 * 	Support ast mechanism for threads.
 * 
 * 	Revision 2.18  89/10/03  19:25:45  rpd
 * 	Fixed locking problem:  thread_block/thread_switch now unlock
 * 	the current thread before calling thread_wakeup.  Also
 * 	reorganized thread_wakeup_with_result/thread_wakeup_one into
 * 	a common function, thread_wakeup_prim, with macro wrappers.
 * 	[89/09/01  01:28:54  rpd]
 * 
 * 	Revision 2.17  89/05/30  10:37:44  rvb
 * 	In the idle_thread()'s loop, call some function for mips, so
 * 	that the compiler does not play too smart with caching pointers.
 * 	[89/04/26            af]
 * 
 * 	Revision 2.16  89/04/22  15:24:45  gm0w
 * 	Removed MACH_NFS dependency from thread_wakeup_one().
 * 	[89/04/14            gm0w]
 * 
 * 	Revision 2.15  89/03/09  20:15:19  rpd
 * 	More cleanup.
 * 
 * 	Revision 2.14  89/03/07  18:01:49  rpd
 * 	Picked up fix from dlb for a starvation bug.
 * 
 * 	Revision 2.13  89/02/25  18:08:04  gm0w
 * 	Changes for cleanup.
 * 
 * 	Revision 2.12  89/01/30  22:07:30  rpd
 * 	Added declarations of variables from kern/sched.h.
 * 	(The declarations there use "extern" now.)
 * 	[89/01/25  15:18:44  rpd]
 * 
 * 	Changed "NCPUS > 1" conditionals to MACH_SLOCKS in thread_wakeup_one.
 * 	[89/01/24  13:29:42  rpd]
 * 
 * 	Revision 2.11  89/01/27  10:21:03  rvb
 * 	Fix locking in thread_wakeup_one().
 * 	[89/01/25            dlb]
 * 
 * 	Revision 2.10  89/01/18  00:50:06  jsb
 * 	NFS: Added thread_wakeup_one.
 * 	[89/01/17  10:18:12  jsb]
 * 
 * 	Revision 2.9  89/01/15  16:26:20  rpd
 * 	Use decl_simple_lock_data.
 * 	[89/01/15  15:05:58  rpd]
 * 
 * 	Revision 2.8  88/12/19  02:46:33  mwyoung
 * 	Corrected include file references.  Use <kern/macro_help.h>.
 * 	[88/11/22            mwyoung]
 * 
 * 	In thread_wakeup_with_result(), only lock threads that have the
 * 	appropriate wait_event.  Both the wait_event and the hash bucket
 * 	links are only modified with both the thread *and* hash bucket
 * 	locked, so it should be safe to read them with either locked.
 * 
 * 	Documented the wait event mechanism.
 * 
 * 	Summarized ancient history.
 * 	[88/11/21            mwyoung]
 * 
 * 	Revision 2.7  88/08/25  18:18:00  mwyoung
 * 	Corrected include file references.
 * 	[88/08/22            mwyoung]
 * 
 * 	Avoid unsigned computation in wait_hash.
 * 	[88/08/16  00:29:51  mwyoung]
 * 
 * 	Add priority check to thread_check; make queue index unsigned,
 * 	so that checking works correctly at all.
 * 	[88/08/11  18:47:55  mwyoung]
 * 
 * 	Revision 2.6  88/08/06  18:25:03  rpd
 * 	Eliminated use of kern/mach_ipc_defs.h.
 * 
 * 	Revision 2.5  88/07/20  16:39:35  rpd
 * 	Changed "NCPUS > 1" conditionals that were eliminating dead
 * 	simple locking code to MACH_SLOCKS conditionals.
 * 	[89/02/09            dlb]
 * 
 * $EndLog$
 */
/*
 *	File:	sched_prim.c
 *	Author:	Avadis Tevanian, Jr.
 *
 *	Copyright (C) 1986, Avadis Tevanian, Jr.
 *
 *	Scheduling primitives
 *
 * Historical summary:
 *
 *	Redo priority recomputation. [dlb, 29 feb 88]
 *	New accurate timing. [dlb, 19 feb 88]
 *	Simplified choose_thread and thread_block. [dlb, 18 dec 87]
 *	Add machine-dependent hooks in idle loop. [dbg, 24 nov 87]
 *	Quantum scheduling changes. [dlb, 14 oct 87]
 *	Replaced scheduling logic with a state machine, and included
 *	 timeout handling. [dbg, 05 oct 87]
 *	Deactivate kernel pmap in idle_thread. [dlb, 23 sep 87]
 *	Favor local_runq in choose_thread. [dlb, 23 sep 87]
 *	Hacks for master processor handling. [rvb, 12 sep 87]
 *	Improved idle cpu and idle threads logic. [dlb, 24 aug 87]
 *	Priority computation improvements. [dlb, 26 jun 87]
 *	Quantum-based scheduling. [avie, dlb, apr 87]
 *	Improved thread swapper. [avie, 13 mar 87]
 *	Lots of bug fixes. [dbg, mar 87]
 *	Accurate timing support. [dlb, 27 feb 87]
 *	Reductions in scheduler lock contention. [dlb, 18 feb 87]
 *	Revise thread suspension mechanism. [avie, 17 feb 87]
 *	Real thread handling [avie, 31 jan 87]
 *	Direct idle cpu dispatching. [dlb, 19 jan 87]
 *	Initial processor binding. [avie, 30 sep 86]
 *	Initial sleep/wakeup. [dbg, 12 jun 86]
 *	Created. [avie, 08 apr 86]
 */

/*
 * Replacements for thread_wakeup, thread_block, assert_wait, spl*, etc.
 */

#include <mach_ltracks.h>
#include <cpus.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/user.h>
#include <kern/parallel.h>
#include <kern/queue.h>
#include <sys/signal_macros.h>
#include <sys/kernel.h>
#include <sys/synch.h>
#include <sys/time.h>

#include <mach/boolean.h>
#include <mach/thread_switch.h>
#include <uxkern/import_mach.h>
#include <uxkern/syscalltrace.h>
#include <kern/sched_prim.h>

#include <net/net_globals.h>	/* For NETISR_THREAD */
#include <net/netisr.h>		/* For netisr */

extern int	timeout_special();

void thread_block();

#define TH_WAIT			0x01	/* thread is queued for waiting */
#define TH_RUN			0x04	/* thread is running or on runq */

/*
 *	State machine
 *
 * states are combinations of:
 *  R	running
 *  W	waiting (or on wait queue)
 *  S	suspended (or will suspend)
 *  N	non-interruptible
 *  O	swapped out
 *
 * init	action 								 swap
 *	assert_wait	thread_block	clear_wait	suspend	resume	out in
 *
 * R	RW, RWN		R;   setrun	-		RS	-	-
 * RS	RWS, RWNS	S;  wake_active	-		-	R	-
 * RN	RWN		RN;  setrun	-		RNS	-	-
 * RNS	RWNS		RNS; setrun	-		-	RN	-
 *
 * RW			W		R		RWS	-	-
 * RWN			WN		RN		RWNS	-	-
 * RWS			WS; wake_active	RS		-	RW	-
 * RWNS			WNS		RNS		-	RWN	-
 *
 * W					R;   setrun	WS	-	WO
 * WN					RN;  setrun	WNS	-	-
 * WNS					RNS; setrun	-	WN	-
 * WO					RO;  swapin	WSO	-	-
 *
 * S					-		-	R	SO
 * SO					-		-	RO	-
 * WS					S		-	W	WSO
 * WSO					SO		-	WO	-
 *
 * RO					-		RSO	-	     R
 * RSO					-		-	RO	     RS
 */

/*
 *	Waiting protocols and implementation:
 *
 *	Each thread may be waiting for exactly one event; this event
 *	is set using assert_wait().  That thread may be awakened either
 *	by performing a thread_wakeup_prim() on its event,
 *	or by directly waking that thread up with clear_wait().
 *
 *	The implementation of wait events uses a hash table.  Each
 *	bucket is queue of threads having the same hash function
 *	value; the chain for the queue (linked list) is the run queue
 *	field.  [It is not possible to be waiting and runnable at the
 *	same time.]
 *
 *	Locks on both the thread and on the hash buckets govern the
 *	wait event field and the queue chain field.  Because wakeup
 *	operations only have the event as an argument, the event hash
 *	bucket must be locked before any thread.
 *
 *	Scheduling operations may also occur at interrupt level; therefore,
 *	interrupts below splsched() must be prevented when holding
 *	thread or hash bucket locks.
 *
 *	The wait event hash table declarations are as follows:
 */
#define NUMQUEUES	59

queue_head_t		wait_queue[NUMQUEUES];
decl_simple_lock_data(,	wait_lock[NUMQUEUES])

#define wait_hash(event) \
	(((int)((event) < 0 ? ((event) ^ -1) : (event)))%NUMQUEUES)

struct mutex	master_mutex = MUTEX_INITIALIZER;

#if MACH_ASSERT
struct mutex_aux master_mutex_aux;
int master_debug = 0;
#endif	/* MACH_ASSERT */


#if MACH_ASSERT
void
trace_master_lock(destp, max, string)
	int	*destp;
	int	max;
	char	*string;
{
	save_call_chain(destp, max);

	if (master_debug >= 4) {
		print_call_chain(destp, max, string);
	}
}

/*
 * Define these as routines so they appear in tracebacks.
 */
void do_master_lock()
{
	int i;

	mutex_lock(&master_mutex)

#if MACH_LDEBUG
	if (master_debug >= 1) {
		if (master_mutex_aux.slck_addr != -1) {
			printf("do_master_lock: wrong prev locker, "
					"expected -1 found 0x%x\n",
					master_mutex_aux.slck_addr);
			print_call_chain(&master_mutex_aux.slock_trace[0],
					MASTER_TRACE_COUNT, "  last LOCKER:");
			print_call_chain(&master_mutex_aux.sunlock_trace[0],
					MASTER_TRACE_COUNT, "  last UNLOCKER:");
		}
	}
#endif	/* MACH_LDEBUG */
	ASSERT(master_mutex_aux.slck_addr == -1);

	master_mutex_aux.slthread = (char *)&u.uu_master_lock;
	master_mutex_aux.slck_addr = (int) current_pc();
	master_mutex_aux.sunlck_addr = (int) -1;

#if MACH_LDEBUG
	trace_master_lock(&master_mutex_aux.slock_trace[0], MASTER_TRACE_COUNT,
			"MLlock");
#endif	/* MACH_LDEBUG */
}

void do_master_unlock()
{
	int i;

#if MACH_LDEBUG
	if (master_debug >= 1) {

		if (mutex_try_lock(&master_mutex)) {
			/* keep lock now that we have it; we unlock below */
			printf("do_master_unlock: master already unlocked!\n");
			print_call_chain(&master_mutex_aux.slock_trace[0],
					MASTER_TRACE_COUNT, "  last LOCKER:");
			print_call_chain(&master_mutex_aux.sunlock_trace[0],
					MASTER_TRACE_COUNT, "  last UNLOCKER:");

		} else if (master_mutex_aux.sunlck_addr != -1) {
			printf("do_master_unlock: wrong prev unlocker, "
					"expected -1 found 0x%x\n",
					master_mutex_aux.sunlck_addr);
			print_call_chain(&master_mutex_aux.slock_trace[0],
					MASTER_TRACE_COUNT, "  last LOCKER:");
			print_call_chain(&master_mutex_aux.sunlock_trace[0],
					MASTER_TRACE_COUNT, "  last UNLOCKER:");
		}
	}
#endif	/* MACH_LDEBUG */

	ASSERT(master_mutex_aux.sunlck_addr == -1);

#if MACH_LDEBUG
	if (master_debug >= 1) {
		if (master_mutex_aux.slthread != (char *)&u.uu_master_lock) {
			printf("do_master_unlock: unlocker (self) != locker "
					"self= 0x%x[%d] locker's=0x%x[%d]\n",
					&u.uu_master_lock,
					u.uu_master_lock,
					master_mutex_aux.slthread,
					*((int *)master_mutex_aux.slthread));

		}
	}
#endif	/* MACH_LDEBUG */

	ASSERT(master_mutex_aux.slthread == &u.uu_master_lock);

	master_mutex_aux.slthread = &u.uu_master_lock;
	master_mutex_aux.slck_addr = (int) -1;
	master_mutex_aux.sunlck_addr = (int) current_pc();

#if MACH_LDEBUG
	trace_master_lock(&master_mutex_aux.sunlock_trace[0],
			MASTER_TRACE_COUNT, "unlock");
#endif	/* MACH_LDEBUG */

#if MACH_LDEBUG
	if (master_debug >= 3) {
		if (u.uu_master_lock) {
			printf("do_master_unlock: u.uu_master=%d (&=0x%x)\n",
					u.uu_master_lock, &u.uu_master_lock);
			print_call_chain(&master_mutex_aux.slock_trace[0],
					MASTER_TRACE_COUNT, "  last LOCKER:");
			print_call_chain(&master_mutex_aux.sunlock_trace[0],
					MASTER_TRACE_COUNT, "  last UNLOCKER:");
		}
	}
#endif	/* MACH_LDEBUG */
	mutex_unlock(&master_mutex);
}

void
do_unix_master()
{
	/*
	 * Note that uu_master_lock is unsigned so "negative" values
	 * will trigger this assert too.
	 */
	if (u.uu_master_lock > MASTER_LOCK_SANITY_MAX) {
		panic("do_unix_master: uu_master_lock not sane =0x%x",
				u.uu_master_lock);
	}
	if (u.uu_master_lock == 0) {
		master_lock();
	}
	++u.uu_master_lock;		/* alter count only while locked */

#if MACH_LDEBUG
	trace_master_lock(&u.uu_slock_trace[0], MASTER_TRACE_COUNT,
			"unix_master");
#endif	/* MACH_LDEBUG */
}
void
do_unix_release()
{
	/*
	 * Note that uu_master_lock is unsigned so "negative" values
	 * will trigger this assert too.
	 */
	if (u.uu_master_lock == 0 ||
			u.uu_master_lock > MASTER_LOCK_SANITY_MAX) {
		panic("do_unix_release: uu_master_lock not sane =0x%x",
				u.uu_master_lock);
	}

#if MACH_LDEBUG
	trace_master_lock(&u.uu_sunlock_trace[0], MASTER_TRACE_COUNT,
			"unix_release");
#endif	/* MACH_LDEBUG */

	if (--u.uu_master_lock == 0) { /* alter count only while locked */
		master_unlock();
	}
}
#endif /* MACH_ASSERT */

/*
 * The wait_locks protect only the sleep queues and the uu_sleep_link field
 * in threads.  Thread_lock protects the uu_wait_event/interruptible/timeout
 * fields in threads.  To avoid deadlock, locking policy is that the following
 * locks must be acquired in the specified order (if two or more of them are to
 * be simultaneously held):
 *
 *	1) master lock
 *	2) any wait_lock
 *	3) any thread lock
 */

#if NCPUS == 1
#define thread_lock(th)
#define thread_unlock(th)
#else
#define thread_lock(th)		mutex_lock(&(th)->uu_lock)
#define thread_unlock(th)	mutex_unlock(&(th)->uu_lock)
#endif


void sched_init()
{
	wait_queue_init();
#if MACH_LTRACKS
	master_mutex_aux.slthread = (char*) -1;
	master_mutex_aux.slck_addr = (int) -1;
	master_mutex_aux.sunlck_addr = (int) -1;
#endif
}

wait_queue_init()
{
	register int i;

	for (i = 0; i < NUMQUEUES; i++) {
		queue_init(&wait_queue[i]);
		simple_lock_init(&wait_lock[i]);
	}
}

#if DEBUG
int	sched_debug = 0;
#endif

/*
 *	Thread timeout routine, called when timer expires.
 *	Called at splhigh.
 */
thread_timeout(thread)
	register uthread_t thread;
{
	clear_wait(thread, THREAD_TIMED_OUT, FALSE);
}


/*
 *	thread_set_timeout:
 *
 *	Set a timer for the current thread, if the thread
 *	is ready to wait.  Must be called between assert_wait()
 *	and thread_block().
 */
void thread_set_timeout(t)
	int	t;	/* timeout interval in ticks */
{
        
	register uthread_t	thread = current_thread();

	thread_lock(thread);
	thread->uu_timeout = t;
	thread_unlock(thread);
}

/*
 *	assert_wait:
 *
 *	Assert that the current thread is about to go to
 *	sleep until the specified event occurs.
 */
void assert_wait(event, interruptible)
	int		event;
	boolean_t	interruptible;
{
	register queue_t	q;
	register int		index;
	register uthread_t	thread;
#if	MACH_SLOCKS
	register simple_lock_t	lock;
#endif

	thread = current_thread();
#if	DEBUG
	if (sched_debug)
		printf("assert_wait: thread = 0x%x, event = 0x%x\n", thread, event);
#endif
	if (thread->uu_wait_event != 0) {
#if	DEBUG
		printf("assert_wait: already asserted event 0x%x\n",
			thread->uu_wait_event);
#endif
		panic("assert_wait");
	}
#if	MACH_LTRACKS && NCPUS == 1
	if (!thread->uu_master_lock)
		/* Detect failure to serialize for uniprocessor: */
		panic("Master lock not held in assert_wait.\n");
#endif
	if (event != 0) {
		index = wait_hash(event);
		q = &wait_queue[index];
#if	MACH_SLOCKS
		lock = &wait_lock[index];
#endif
		simple_lock(lock);
		thread_lock(thread);
		queue_enter(q, thread, uthread_t, uu_sleep_link);

		thread->uu_wait_event = event;
		thread->uu_state = TH_WAIT;
		thread->uu_timeout = 0;
		thread->uu_interruptible = interruptible;
		thread_unlock(thread);
		simple_unlock(lock);
	} else {
		thread_lock(thread);
		thread->uu_state = TH_WAIT;
		thread->uu_timeout = 0;
		thread->uu_interruptible = interruptible;
		thread_unlock(thread);
	}
}

/*
 *	clear_wait:
 *
 *	Clear the wait condition for the specified thread.  Start the thread
 *	executing if that is appropriate.
 *
 *	parameters:
 *	  thread		thread to awaken
 *	  result		Wakeup result the thread should see
 *	  interrupt_only	Don't wake up the thread if it isn't
 *				interruptible.
 */
void clear_wait(thread, result, interrupt_only)
	register uthread_t	thread;
	int			result;
	boolean_t		interrupt_only;
{
	register int		index;
	register queue_t	q;
#if	MACH_SLOCKS
	register simple_lock_t	lock;
#endif
	register int		event;

#if	MACH_LTRACKS && NCPUS == 1
	if (!u.uu_master_lock)
		/* Detect failure to serialize for uniprocessor: */
		panic("Master lock not held in clear_wait.\n");
#endif
	thread_lock(thread);
	if (interrupt_only && !thread->uu_interruptible) {
		/*
		 *	can't interrupt thread
		 */
		thread_unlock(thread);
		return;
	}

	event = thread->uu_wait_event;
	if (event != 0) {
		/* Obey lock-ordering policy: */
		thread_unlock(thread);
		index = wait_hash(event);
		q = &wait_queue[index];
#if	MACH_SLOCKS
		lock = &wait_lock[index];
#endif
		simple_lock(lock);
		/*
		 *	If the thread is still waiting on that event,
		 *	then remove it from the list.  If it is waiting
		 *	on a different event, or no event at all, then
		 *	someone else did our job for us.
		 */
		thread_lock(thread);
		if (thread->uu_wait_event == event) {
			queue_remove(q, thread, uthread_t, uu_sleep_link);
			thread->uu_wait_event = 0;
			event = 0;		/* cause to run below */
		}
		simple_unlock(lock);
	}
	if (event == 0) {
		if (thread->uu_state == TH_WAIT) {
			thread->uu_state = TH_RUN;
			thread->uu_wait_result = result;
			condition_signal(&thread->uu_condition);
		}
	}
	thread_unlock(thread);
}

/*
 *	thread_wakeup_prim:
 *
 *	Common routine for thread_wakeup, thread_wakeup_with_result,
 *	and thread_wakeup_one.
 *
 */
void thread_wakeup_prim(event, one_thread, result)
	register int	event;
	boolean_t	one_thread;
	int		result;
{
	register queue_t	q;
	register int		index;
	register uthread_t	thread, next_th;
#if	MACH_SLOCKS
	register simple_lock_t	lock;
#endif

#if	MACH_LTRACKS && NCPUS == 1
	if (u.uu_master_lock == 0 || u.uu_master_lock > 7)
		/* Detect failure to serialize for uniprocessor: */
		panic("Master lock not held in thread_wakeup_prim.\n");
#endif
	index = wait_hash(event);
	q = &wait_queue[index];
#if	MACH_SLOCKS
	lock = &wait_lock[index];
#endif
	simple_lock(lock);

	thread = (uthread_t) queue_first(q);
	while (!queue_end(q, (queue_entry_t)thread)) {
		next_th = (uthread_t) queue_next(&thread->uu_sleep_link);

		if (thread->uu_wait_event == event) {
			thread_lock(thread);
#if	DEBUG
			if (sched_debug)
				printf("thread_wakeup: thread 0x%x woken, event = 0x%x\n", thread, event);
#endif
			queue_remove(q, thread, uthread_t, uu_sleep_link);
			thread->uu_wait_event = 0;
			thread->uu_wait_result = result;
			thread->uu_state = TH_RUN;
			/*
			 * Do this *before* condition signal, so wakened thread
			 * doesn't run into the lock and go back to sleep:
			 */
			thread_unlock(thread);
			/*
			 * wakeup thread
			 */
			condition_signal(&thread->uu_condition);
			if (one_thread)
				break;
		}
		thread = next_th;
	}
	simple_unlock(lock);
}

/*
 *	thread_sleep:
 *
 *	Cause the current thread to wait until the specified event
 *	occurs.  The specified lock is unlocked before releasing
 *	the cpu.  (This is a convenient way to sleep without manually
 *	calling assert_wait).
 */
void thread_sleep(event, lock, interruptible)
	int		event;
	simple_lock_t	lock;
	boolean_t	interruptible;
{
	register uthread_t	thread = &u;

#if	DEBUG
	if (sched_debug)
		printf("thread_sleep: event = 0x%x\n", event);
#endif
	if (thread->uu_wait_event != 0) {
		printf("WARNING: Recursive sleep call.  Yielding instead.\n");
		simple_unlock(lock);		/* release the lock */
		thread_yield();			/* let another cthread run */
		return;
	}
	assert_wait(event, interruptible);	/* assert event */
	simple_unlock(lock);			/* release the lock */
	thread_block();				/* block ourselves */
}

/*
 * Suspend the current thread, after first arranging to be awakened in the
 * future, if uu_timeout is set.  Uses condition_wait with an associated
 * lock to ensure that we can do the following as an atomic action:
 * check uu_wait_event and sleep only if it is non-zero, indicating we
 * have not been awakened between assert_wait and here.  The associated
 * lock is the master mutex in the uni-processor configuration and is the
 * thread lock, otherwise.
 */
void
thread_block()
{
	register uthread_t	thread = current_thread();
	int			timeout;
	int			timeout_block;

#if	!NETISR_THREAD
	/*
	 * Simulate software interrupts for network.  It is only safe to
	 * call this if we are not already trying to get a lock, otherwise
	 * we may end up doing a recursive sleep.
	 */
	if (netisr && !thread->uu_lock_sleep)
		Netintr();
#endif

#if	NCPUS == 1
#if	MACH_ASSERT
	if (!thread->uu_master_lock)
		/* Detect failure to serialize for uniprocessor: */
		panic("Master lock not held in thread_block.\n");
#endif	/* MACH_ASSERT */
#else
	if (thread->uu_master_lock)
		master_unlock();
	thread_lock(thread);
#endif

	if (thread_should_halt(thread)) {
		/*
		 * XXX - don't know exactly what the integrated kernel
		 * would have done in this case...
		 *
		 * Note that the thread_unlock/thread_lock are no-ops
		 * if NCPUS == 1
		 */
		thread_unlock(thread);
		clear_wait(thread, THREAD_SHOULD_TERMINATE, FALSE);
		thread_lock(thread);
	}

	if (thread->uu_state == TH_WAIT) {
		timeout = thread->uu_timeout;
		if (timeout)
			timeout_block = timeout_special(thread_timeout, thread ,
							timeout, 1);
		/* Go to sleep: */
#if	NCPUS == 1
		ux_server_thread_suspend(thread, &master_mutex);
#else
		ux_server_thread_suspend(thread, &thread->uu_lock);
#endif

		if (timeout) untimeout_special(timeout_block);
	}

	if (thread->uu_procp && thread->uu_procp->p_stat == SSTOP) {
		/* 
		 * active loop: on what could we wait here: another condition ?
		 * Only one thing is sure: we can't exit from this loop
		 * without being interrupted by a SIGCONT, so change the
		 * wait result.
		 */
		thread->uu_wait_result = THREAD_INTERRUPTED;
#if	NCPUS == 1
		master_unlock()
#else
		thread_unlock(thread);
#endif
		do {
			/* we don't have anything to do anymore... */
			(void) thread_yield();
		} while (thread->uu_procp && thread->uu_procp->p_stat == SSTOP);
#if	NCPUS == 1
		master_lock()
#else
		thread_lock(thread);
#endif
	}

#if	NCPUS > 1
	thread_unlock(thread);
	if (thread->uu_master_lock)
		master_lock();
#endif
}


/* Used to let every other thread run before trying something
 * again, e.g. acquiring a simple lock (see simple_lock_solid).
 *
 * For unwired threads, it's important to use both cthread_yield, which tries
 * to do a user-mode context switch to a different runnable C-thread, and a
 * Mach priority depression, which tries to context switch to a different
 * runnable Mach thread, because we don't know if the thread that we are
 * waiting for is on the Cthread run queue and doesn't currently have
 * possession of a Mach thread, or is a Cthread currently running in a
 * different Mach thread.
 *
 * Under the assumption that threads probably would not do a blocking Cthread
 * operation while holding a spin lock (or holding something else that makes
 * other threads spin-wait), it makes more sense to switch to a different
 * Mach thread first, since this is much more likely to pay off:
 */
extern int wired_threads;
void
thread_yield()
{
	(void) thread_switch(MACH_PORT_NULL, SWITCH_OPTION_DEPRESS, 10);
	if (!wired_threads)
		(void) cthread_yield();
}


void
spl_init() { }

#undef spl0
int spl0()
{
    return (spl_n(0));
}

#undef splsoftclock
int splsoftclock()
{
    return (spl_n(SPLSOFTCLOCK));
}

#undef splnet
int splnet()
{
    return (spl_n(SPLNET));
}

#undef splbio
int splbio()
{
    return (spl_n(SPLBIO));
}

#undef spltty
int spltty()
{
    return (spl_n(SPLTTY));
}

#undef splimp
int splimp()
{
    return (spl_n(SPLIMP));
}

#undef splstr
int splstr()
{
    return (spl_n(SPLHIGH));
}

#undef splhigh
int splhigh()
{
    return (spl_n(SPLHIGH));
}

#undef splx
int splx(s)
    int s;
{
    return(spl_n(s));
}

/*
 * Version of spl_n for modules that depend on real spl locking (all other
 * modules define spl_n to be a no-op).  See sys/synch.h for its use in the
 * definition of the spl calls:
 */
int spl_n_lock(x)
	int x;
{
	int result;
	SPL_N_LOCK_BODY(x, result);
	return result;
}

int intr_cnt = 0;

/*
 * Interrupt routines start at raised spl:
 */
void
interrupt_enter(level)
	int level;
{
#if	NCPUS == 1
	unix_master();	/* Compiling for uni-processor; serialize everything */
#endif
	spl_n(level);
	intr_cnt++;
}

void
interrupt_exit(level)
	int level;
{
	spl_n(0);

#if	!NETISR_THREAD
	/*
	 * Simulate software interrupts for network.
	 */
	if (netisr)
		Netintr();
#endif

#if	NCPUS == 1
	unix_release();	/* Compiling for uni-processor; serialize everything */
#endif
	
	ASSERT(u.uu_master_lock == 0);
}

/*
 *      Just in case someone doesn't use the macro
 */
#undef  thread_wakeup
void            thread_wakeup(x)
        register int    x;
{
        thread_wakeup_with_result(x, THREAD_AWAKENED);
}

/*
 *	Just in case someone doesn't use the macro (see user.h)
 */
#undef	current_thread
uthread_t current_thread()
{
	return &u;
}

int
handler_stats(type)
int     type;
{
        return(intr_cnt);
}
