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
 * HISTORY
 * $Log: emul_callback.c,v $
 * Revision 1.24  1995/02/03  23:05:17  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: 12314
 *  Testing: Emulator debug builds now compile
 *  Module(s): svr/emulator/emul_callback.c
 * Corrected typo so EPRINT macro would compile when DEBUG was set mode.
 *
 * Revision 1.23  1995/01/27  23:44:39  rlg
 * Two problems were found with fix for PTS #11035.  There was an uninitialized
 * variable (bi_count) in the new call to thread_info().  There were also ports
 * that were not "cleaned up".  These problems were fixed.
 *
 *  Reviewer: Stan Smith
 *  Risk: low
 *  PTS #12236
 *  Testing: pfs and fileio EATs, I/O mode integration test
 *  Module(s):  emulator/emul_callback.c
 *
 * Revision 1.22  1995/01/17  22:40:04  rlg
 * A timing window was hit in the routine semul_release_state().  This
 * window was identified by the original implementor, who felt that it
 * was "too much work for such an unlikely occurrence" to fix.  This
 * window has now been closed.
 *
 *  Reviewer: Nandini Ajmani
 *  Risk: medium - no failing test case associated with this problem
 *  Benefit or PTS #: 11035
 *  Testing: pfs and fileio EATs; iomode integration test
 *  Module(s): emulator/emul_callback.c
 *
 * Revision 1.21  1994/11/18  20:22:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.20  1994/03/29  17:28:08  rlg
 * Merged the changes from 1.13.4.6 on the R1.2 branch into the R1.3 branch.
 *
 * Revision 1.19  1994/03/19  00:11:29  jlitvin
 * Fix minor merge error.
 *
 * Revision 1.18  1994/03/18  20:20:10  nandy
 * Merged from R1_2 branch fix for 8101.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.17  1994/03/04  22:22:56  dbm
 * Mainline merge for bug 6919, (1.2 rev 1.13.4.4)
 *
 * Revision 1.16  1994/02/16  00:19:10  dbm
 * Merge from the 1.2 branch version 1.13.4.3.
 *  Reviewer:None.
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.15  1994/01/06  22:24:40  cfj
 * Merged from the R1_2 branch.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.14  1994/01/05  17:07:33  brad
 * Fixed lint warnings in PFS-related code.
 *
 * Revision 1.13.4.6  1994/03/29  16:13:18  rlg
 * The warnings found by lint were corrected as required
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  low
 *  Benefit or PTS #:  7719
 *
 * Revision 1.13.4.5  1994/03/18  19:42:24  nandy
 * Start using inprogress field in the transaction to make sure the entr
 * is not in the middle of being registered or deregistered.
 *
 *  Reviewer: dbm
 *  Risk: M
 *  Benefit or PTS #: 8101
 *  Testing: PTS test case
 *  Module(s): emul_callback.c
 *
 * Revision 1.13.4.4  1994/03/04  21:36:56  dbm
 * Added parameter to tmgr_release_to_server() function call.
 *  Reviewer: Brad Rullman
 *  Risk:M
 *  Benefit or PTS #:6919
 *  Testing: PFS Eats, overlapping PFS SAT's
 *
 * Revision 1.13.4.3  1994/02/15  23:25:52  dbm
 * Fixed bugs related to registering operations from asynchronous I/O
 * threads.
 *  Reviewer: Bob Godley
 *  Risk:L
 *  Benefit or PTS #:6919, 8067, and 8100
 *  Testing: Specific test cases, did ^C tests at various times during
 * 	  the tests.  Also ran PFS eats several times.
 *  Module(s):
 * 	emul_callback.c
 *
 * Revision 1.13.4.2  1994/01/09  00:18:04  brad
 * Fixed bug found by lint (wrong number pf params to pfs_free() in
 * pfs2_user_side.c); also fixed lint warnings in PFS-related code.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Some PFS source now passes lint
 *  Testing: Ran PFS EATs
 *  Module(s): emulator/emul_callback.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs2_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_fdt.h
 *             emulator/pfs_iomode.c
 *             emulator/pfs_tokenmgt.c
 *             emulator/pfs_user_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr2.defs
 *             server/uxkern/fsvr2_server_side.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.13.4.1  1994/01/06  22:22:00  cfj
 * Lint discovered a missing parameter to all calls to the intr_delivery()
 * function.  Added the missing parameter.
 *  Reviewer: dbm
 *  Risk: Low
 *  Benefit or PTS #: 7660
 *  Testing: ^C EATs
 *  Module(s): emulator/emul_callback.c
 *
 * Revision 1.13  1993/08/04  01:24:50  dbm
 * Fixed a race condition in PFS I/O mode 3 between the revoke and the re-acquire
 * which can cause a deadlock.
 *
 * Revision 1.12  1993/08/03  20:48:38  cfj
 * Fixed ramdisk emulator build.
 *
 * Revision 1.11  1993/07/30  15:59:15  cfj
 * Remove #ifdef MAPPED_FILES around declaration of revoke_port.
 *
 * Revision 1.10  1993/07/27  16:08:00  nandy
 * Now that the blocking kernel bug is fixed (dlb, norma/ipc_output.c)
 * destroy the revoke port before letting ourselves get suspended.
 * This avoids a dead-lock, which was originally conjectured to exist,
 * but has now been seen. (loverso)
 *
 * Revision 1.9  1993/07/16  03:04:53  dbm
 * Added token optimization functionality.
 *
 * Revision 1.8  1993/07/14  17:30:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:22:19  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/25  18:33:19  dbm
 * Added interrupt support for asyncronous calls and support for PFS.
 *
 * Revision 1.6  1993/05/06  20:14:43  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:20  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.25  93/06/16  13:49:26  klh
 * 	Revision 2.23  93/05/16  23:05:50  loverso
 * 		Prevent leak in the fast path for semul_release_state() (chrisp).
 * 		Implement EINTR/ERESTART for fsvr signal delivery path.
 * 		Implement interrupt-all-on-suspend (bugs #244, #548).
 * 		(loverso)
 * 
 * 	Revision 2.22  93/05/13  16:44:48  roy
 * 		Removed semul_token_get_size.
 * 		[93/05/05            roy]
 * 
 * 	Revision 2.21  93/01/06  14:58:39  loverso
 * 		Correctly propogate is_exit condition. (loverso)
 * 
 * Revision 2.24  93/04/29  13:53:42  klh
 * 	Revision 2.20  92/11/17  19:46:04  loverso
 * 		Fix deadlock where a get size message can be queued just as the
 * 		semul_release_state finishes and the task is suspended.  We do
 * 		this by destroying and recreating the revoke port, causing any
 * 		outstanding messages from the server to fail (which will then
 * 		notice that it has the token). (offshoot of bug 509)
 * 		[NOTE: this fix is currently disabled pending a kernel fix]
 *
 * 		Move initial thread suspend logic from (server) unix_task_suspend
 * 		to (emulator) semul_release_state to fix deadlock. (bug 387)
 *
 * 		If we can't find a thread to interrupt, then set a global tested
 * 		at the end of emul_syscall.
 * 		(bug 459)
 *
 * Revision 1.5  1993/04/03  03:17:28  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 2.23  93/03/10  14:14:40  yazz
 * Synchronous close merge from Intel.
 * 
 * 	Revision 1.3.6.1  1993/02/16  20:39:16  cfj
 * 	Synchronous close from OSF.
 *
 * 	Revision 2.21  93/01/06  14:58:39  loverso
 * 	 Correctly propogate is_exit condition. (loverso)
 * 
 * 	Revision 1.3  1992/12/11  02:51:23  cfj
 * 	Merged 12-1-92 bug drop from Locus.
 *
 * 	Revision 1.2  1992/11/30  22:08:20  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.1  1992/11/05  22:15:34  dleslie
 * 	cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 1.2  1992/11/30  22:08:20  dleslie
 * Revision 1.1.2.1.2.6  1993/03/23  01:55:50  dbm
 * Fixed up so that the correct value is passed to release_state().  Not all
 * callers were passing the required parameter.  Also fixed use of is_exit
 * so that release_state has the correct value.
 *
 * Revision 1.1.2.1.2.5  1993/03/19  01:31:38  dbm
 * Added a parameter to close_on_exit() to allow I/O modes to work
 * correctly.
 *
 * Revision 1.1.2.1.2.4  1993/03/11  00:28:52  dbm
 * Bumped the number of transactions to 1024 from 64 to allow more stripe
 * stripe directories.
 *
 * Revision 1.4  1993/02/16  21:08:14  cfj
 * Merge sync close into main stem.
 *
 * Revision 1.3.6.1  1993/02/16  20:39:16  cfj
 * Synchronous close from OSF.
 *
 * Revision 2.21  93/01/06  14:58:39  loverso
 * 	Correctly propogate is_exit condition. (loverso)
 * 
 * Revision 1.1.2.1.2.3  1992/12/16  05:56:47  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.1.2.2  1992/12/11  21:01:36  dbm
 * Added ifdef's to remove mapped file dependencies on file tokens.
 *
 * Revision 1.3  1992/12/11  02:51:23  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:00:40  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.2  1992/11/30  22:08:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:15:34  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.22  92/11/23  16:03:27  klh
 * 	Revision 2.20  92/11/17  19:46:04  loverso
 * 		Fix deadlock where a get size message can be queued just as the
 * 		semul_release_state finishes and the task is suspended.  We do
 * 		this by destroying and recreating the revoke port, causing any
 * 		outstanding messages from the server to fail (which will then
 * 		notice that it has the token). (offshoot of bug 509)
 * 		[NOTE: this fix is currently disabled pending a kernel fix]
 * 
 * 		Move initial thread suspend logic from (server) unix_task_suspend
 * 		to (emulator) semul_release_state to fix deadlock. (bug 387)
 * 
 * 		If we can't find a thread to interrupt, then set a global tested
 * 		at the end of emul_syscall.
 * 		(bug 459)
 * 
 * 	Revision 2.19  92/11/05  17:25:52  roy
 * 		bsd_set_callback changed to bsd_register_proc, and added rlimit_fsize
 * 		out arg.
 * 		[92/10/26            roy]
 * 
 * 	Revision 2.18  92/11/03  11:02:05  loverso
 * 		Don't do silliness with "interrupt = interrupt || TRUE;".
 * 		If we don't malloc() in our special mach_msg_server()
 * 		then don't free(), either.
 * 		No longer need to pass reply port to emul_vm_map().
 * 		Don't register callback thread/port before starting the thread!
 * 		(loverso)
 * 
 * Revision 2.21  92/11/16  10:40:28  chrisp
 * Back out change to fix bug #100, unix_task_suspend(): OSF will be
 * 	providing a definitive fix. There are problems with the current
 * 	solution during migration on the Cube.
 * 
 * Revision 2.19  92/10/08  10:55:46  roman
 * Get rid of extra send right to callback thread (added in last
 * 	submission). No longer required.
 * 
 * Revision 2.18  92/10/06  12:03:26  roman
 * Allocate an extra send right to callback port so that TNC can send
 * emulator->emulator messages. Possibly a temporary need.
 * 
 * Revision 2.17  92/09/24  16:48:22  rabii
 * 	Panic on must_suspend==TRUE before attempting to take SUSPEND_LOCK.
 * 
 * Revision 2.16  92/09/11  09:25:14  rabii
 * 	Fix another leak from task_threads in semul_forward_signal().
 * 
 * Revision 2.15  92/08/28  13:55:14  loverso
 * 	Fix leak from task_threads in semul_release_state().
 * 
 * Revision 2.14  92/08/26  12:09:11  loverso
 * 	Renamed from emul_isc.c.  Added new call semul_release_state() to
 * 	allow emulator to clean up before task_suspend.  Added must_suspend
 * 	global to indicate when this cleanup is in progress.
 * 	syscall_suspend_barrier() is the means of indicating a clean suspend
 * 	point for a thread; it is called from the end of emul_syscall and
 * 	from isc_register.  Also, callback thread now registers its thread
 * 	port with the server, so that the server can (1) thread_suspend all
 * 	threads in this task other than the callback and (2) make sure it
 * 	doesn't force the callback to run signal handlers!
 * 
 * 	In semul_forward_signal(), make sure we also don't attempt to make
 * 	the callback thread take a signal.
 * 
 * 	Combine both isc_register interfaces into a single function.
 * 	`Local' syscalls (i.e., select/poll) pass MACH_PORT_NULL for the
 * 	forward port.  Added new state - blocked but uninterruptable (via
 * 	isc) by passing MACH_PORT_DEAD.  This is TT_BUSY, which indicates
 * 	the thread is safely blocked, but not in a fsvr (i.e., it went to
 * 	the PM or a bsd_xxx interface).  Removed the transid param to
 * 	isc_deregister, as it once again uses a thread_id.
 * 
 * 	In isc_{de,}register, allow re-use of transaction table entries
 * 	so that MAX_THREADS is not an absolute limit on threads, but instead
 * 	a limit on threads concurrently in system calls.
 * 
 * 	Since contention is low or non-existent, remove TTE_LOCK and just
 * 	use the single TRANS_LOCK.  This means that the typical case of a
 * 	single threaded process only needs to take one spin lock during
 * 	isc_register instead of two.  The real reason of doing this was
 * 	to make release_state processing much easier.
 * 
 * 	Add optimization to count number of threads blocked safely.
 * 	Add "inprogress" indication for isc_* calls, because release_state
 * 	doesn't obey locking.
 * 
 * 	Replaced calls to the expensive mach_thread_self() with the cheaper
 * 	emul_thread_id_self().  The expensive is deferred to later calls to
 * 	discover thread state to reconstruct its thread id.
 * 
 * 	Clean up panic messages.
 * 	(loverso)
 * 
 * Revision 2.13  92/08/13  19:12:24  rabii
 * 	[92/07/07  15:01:10  roman]
 * 	Change callback_init() to handle the revoke_port in a manner similar 
 * 	to the callback_port.
 * 
 * Revision 2.12  92/07/08  14:53:15  rabii
 * 	Fixed line justification (loverso)
 * 
 * Revision 2.11  92/06/30  22:43:07  loverso
 * 	Revision 2.11  92/06/15  15:15:34  roman
 * 	New parameters to callback_init() to allow the callback_port to be
 * 	inherited/extracted from another task rather than always being
 * 	freshly allocated.
 * 
 * Revision 2.10  92/05/18  12:26:08  roy
 * 	Revision 2.5.1.2  92/05/08  12:02:47  roy
 * 	Diddled semul_token_get_size.
 * 	[92/04/22            roy]
 * 
 * 	Revision 2.5.1.1  92/04/22  09:48:09  roy
 * 	Added semul_token_revoke, semul_token_get_size.  Change name of 
 * 	isc_init() to callback_init() because it now handles the mapped
 * 	file revoke mechanism as well.  Cleaned up includes, externs, etc.
 * 	[92/04/06            roy]
 * 
 * Revision 2.8  92/05/01  09:39:31  rabii
 * 	Added version of mach_msg_server that allocates memory with
 * 	VM_INHERIT_NONE attribute to fix memory leak on fork (pjg).
 * 
 * 	Add appropriate locking to prevent multithreaded programs from
 * 	stepping over themselves. (loverso)
 * 
 * Revision 2.7  92/04/07  13:39:23  pjg
 * 	Extend to be able to interrupt operations blocked (locally) in the
 * 	emulator.  New isc_register_local() interface.  (loverso)
 * 
 * Revision 2.6  92/03/20  11:54:21  pjg
 * 	Panic if mach_msg_server ever returns (pjg).
 * 	Make sure that transid is never 0.  Pass new arg to intr_delivery()
 * 	indicating we are not exit (loverso).
 * 
 * Revision 2.5  92/03/09  11:03:30  durriya
 * 	deallocate thread ports in semul_forward_signal
 * 
 * Revision 2.4  92/03/01  18:48:56  pjg
 * 	Rename the signature thread and port to callback_thread and
 * 	callback_port. Re-visit the code (loverso).
 * 
 * Revision 2.3  92/02/11  18:50:40  pjg
 * 	Call emulthread_stack_alloc instead of emul_stack_alloc.
 * 	Cleanup debugging (loverso).
 * 
 * Revision 2.2  92/01/17  17:15:30  roy
 * 	Initial revision (srl).
 * 
 * $EndLog
 */

/*
 * Emulator Callback Server.
 */

#include <sys/errno.h>
#include <machine/vmparam.h>		/* for EMULATOR_{BASE|END} */
#include <mach/mach.h>
#include <mach/kern_return.h>
#include "emul_stack.h"
#include "emul.h"
#ifdef PFS
#include "fdt.h"
#include "pfs_iomode.h"
#endif


typedef unsigned long transaction_id_t;

typedef struct trans_tbl {
	unsigned long		flags;		/* bitmask */
	emul_thread_id		thread_id;
	transaction_id_t	transaction_id;
	mach_port_t		forward_port;
	/* inprogress can't be part of flags (and beware BOGUS_MEMORY) */
	boolean_t		inprogress;	/* in progress of add/del */
#ifdef PFS
	int			*interrupt;
	int			abort;		/* If set, indicates a thread
						 * abort is required. 
						 */
	transaction_id_t	transaction_id_cnt; /* transaction_id range */
	pfs_fd_t		*pfs_fd; /* Has the list of forward ports */
#endif
	unsigned int		generation;	/* used by release_state */
	thread_t		thread;		/* only valid when insuspend */
	boolean_t		resume;		/* should be resumed */
} TRANS_TBL_T;

/* flags */
#define TT_IDLE		0x0000	/* entry unused  */
#define TT_LOCAL	0x0001	/* blocked locally but interruptable */
#define TT_FORWARDED	0x0002	/* forwarded remotely */
#define TT_BUSY		0x0004	/* blocked locally but NOT interruptable */
#ifdef PFS
#define TT_ASYNC	0x0010	/* Asynchronous I/O thread. */
#endif
#define TT_INTERRUPT	0x0080	/* Interrupt received on callback */

/* A thread limit is BOGUS, but so are the linear searches */
#define MAX_THREADS 	64

#ifdef PFS
/*
 * Define type of operations that can be registered in an PFS
 * asynchronous I/O thread.
 */
#define PFS_ABORT_ASYNC_NOP	0	/* Indidicates no operation is
					 * outstanding on the I/O thread.
					 */
#define PFS_ABORT_ASYNC_IPC	0x1	/* Indicates that a Mach IPC is in
					 * progress that needs to be aborted.
					 */
#define PFS_ABORT_ASYNC_RPC	0x2	/* Indicates that a MIG RPC is in
					 * progress that needs to be 
					 * interrupted.
					 */
#endif

TRANS_TBL_T     	transactions[MAX_THREADS];
transaction_id_t	transaction_id;
unsigned long		trans_tbl_hi = 0;
unsigned long		trans_forwardable = 0;
unsigned long		trans_blocked = 0;

/*
 * This lock protects all the above variables.
 * We used to have separate locks for each trans table entry, but we
 * found contention low/non-existent.
 */
spin_lock_t     		trans_lock;
#define TRANS_LOCK_INIT()	spin_lock_init(&trans_lock)
#define TRANS_LOCK()		spin_lock(&trans_lock)
#define TRANS_UNLOCK()		spin_unlock(&trans_lock)

mach_port_t		callback_port_set;	
thread_t        	callback_thread;
mach_port_t    		callback_port;	      /* Port used for callbacks. */
#ifdef MAPPED_FILES
mach_port_t		revoke_port;	      /* Port used for token revokes. */
#endif


/*
 * Global `there was an interrupt but no-one to handle it' flag
 */
boolean_t		emul_interrupt;

/*
 * task suspend barrier lock and suspend count of "threads to go"
 * suspend_count is only valid if must_suspend is TRUE.
 */
spin_lock_t     	suspend_lock;
unsigned long		suspend_count;
boolean_t		suspend_is_exit = FALSE;
boolean_t		suspend_is_intr = FALSE;
unsigned int		suspend_generation = 0;
boolean_t		must_suspend = FALSE;

void			release_state(), syscall_suspend_barrier();

#define SUSPEND_LOCK_INIT()	spin_lock_init(&suspend_lock)
#define SUSPEND_LOCK()		spin_lock(&suspend_lock)
#define SUSPEND_UNLOCK()	spin_unlock(&suspend_lock)

#define SUSPEND_TIMEOUT		1000	/* millisecs */

/*
 * Place holder for release_state reply
 */
mach_port_t		rs_reply_port = MACH_PORT_NULL;
mach_msg_type_name_t	rs_reply_port_type;

#ifdef DEBUG
int     callback_debug = 0;
int     suspend_debug = 0;
#endif

#ifdef	PFS
extern esize_t	ex_zero;
extern esize_t	ex_neg_one;
#endif


void
callback_server()
{
	kern_return_t 		rc;
	extern int		emul_server();

	rc = mach_msg_server(emul_server, 4096, callback_port_set);
	/*
	 * Should never return
	 */
	e_emulator_error("mach_msg_server returned! error=0x%x", rc);
	emul_panic("callback_server: mach_msg_server returned");
}

/*
 * Initialize the callback thread.
 */
void
callback_init(cport, rport, extract_port_task)
	mach_port_t		cport;
	mach_port_t		rport;
	mach_port_t		extract_port_task;
{
	kern_return_t		rc;
	emul_stack_t		new_stack;
	int			count;
	extern kern_return_t	emul_thread_set_state();
	mach_msg_type_name_t	dummy;
	unsigned int		rlimfsize;

	SUSPEND_LOCK_INIT();
	must_suspend = FALSE;
	suspend_count = 0;

	TRANS_LOCK_INIT();
	transaction_id = 0;

	for (count = 0; count < MAX_THREADS; count++) {
		transactions[count].thread_id = (emul_thread_id)0;
		transactions[count].forward_port = MACH_PORT_NULL;
		transactions[count].transaction_id = 0;
		transactions[count].flags = TT_IDLE;
		transactions[count].inprogress = FALSE;
#ifdef PFS
		transactions[count].interrupt = NULL;
		transactions[count].abort = PFS_ABORT_ASYNC_NOP;
		transactions[count].transaction_id_cnt = 0;
		transactions[count].pfs_fd = NULL; 
#endif
		transactions[count].generation = 0;
		transactions[count].thread = 0;
		transactions[count].resume = FALSE;
	}
	trans_tbl_hi = 0;
	trans_blocked = 0;
	trans_forwardable = 0;

	/*
	 * Allocate the callback port set.
	 */
	rc = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET,
				&callback_port_set);
	if (rc != KERN_SUCCESS) {
		EPRINT(("mach_port_allocate(callback port set)=%x\n", rc));
		emul_panic("callback_init: can't alloc callback port set");
	}	
	
	/*
	 * Allocate the callback port for this task (or extract it from
	 * the task it is being inherited from) and move it into the port set.
	 */
	if (cport == MACH_PORT_NULL) {
		rc = mach_port_allocate(mach_task_self(), 
					MACH_PORT_RIGHT_RECEIVE,
					&callback_port);
		if (rc != KERN_SUCCESS) {
			EPRINT(("mach_port_allocate(callback port)=%x\n", rc));
			emul_panic("callback_init: can't alloc callback port");
		}	
	} else {
		/* Migration only */
		rc = mach_port_extract_right(extract_port_task, cport,
					     MACH_MSG_TYPE_MOVE_RECEIVE,
					     &callback_port, &dummy);
		if (rc != KERN_SUCCESS) {
			EPRINT(("mach_port_extract_right(callback port)=%x\n", 
				rc));
			emul_panic("callback_init: can't extract callback port");
		}	
	}
	rc = mach_port_move_member(mach_task_self(), callback_port, 
				   callback_port_set);
	if (rc != KERN_SUCCESS) {
		EPRINT(("mach_port_move_member(callback port)=%x\n", rc));
		emul_panic("callback_init: can't insert callback port");
	}	

#if	MAPPED_FILES | PFS
	/*
	 * Allocate the token revoke port for this task (or extract it from
	 * the task it is being inherited from) and move it into the port set.
	 */
	if (cport == MACH_PORT_NULL) {
		rc = mach_port_allocate(mach_task_self(), 
					MACH_PORT_RIGHT_RECEIVE,
					&revoke_port);
		if (rc != KERN_SUCCESS) {
			EPRINT(("mach_port_allocate(revoke port)=%x\n", rc));
			emul_panic("callback_init: can't alloc revoke port");
		}	
	} else {
		rc = mach_port_extract_right(extract_port_task, rport,
					     MACH_MSG_TYPE_MOVE_RECEIVE,
					     &revoke_port, &dummy);
		if (rc != KERN_SUCCESS) {
			EPRINT(("mach_port_extract_right(revoke port)=%x\n", 
				rc));
			emul_panic("callback_init: can't extract revoke port");
		}	
	}

	rc = mach_port_move_member(mach_task_self(), revoke_port, 
				   callback_port_set);
	if (rc != KERN_SUCCESS) {
		EPRINT(("mach_port_move_member(revoke port)=%x\n", rc));
		emul_panic("callback_init: can't insert revoke port into set");
	}	
#endif

	/*
	 * Create the callback thread 
	 */
	rc = thread_create(mach_task_self(), &callback_thread);
	if (rc != KERN_SUCCESS) {
		EPRINT(("thread_create(callback thread)=%x\n", rc));
		emul_panic("callback_init: can't create callback thread");
	}

	/*
	 * Start the callback thread
	 */
	new_stack = emulthread_stack_alloc();

	rc = emul_thread_set_state(callback_thread,
			(unsigned int)new_stack,
			(unsigned int)&callback_server);
	if (rc != KERN_SUCCESS) {
		EPRINT(("emul_thread_set_state(callback thread)=%x\n", rc));
		emul_panic("callback_init: failed starting callback thread");
	}

	rc = thread_resume(callback_thread);
	if (rc != KERN_SUCCESS) {
		EPRINT(("thread_resume(callback thread)=%x\n", rc));
		emul_panic("callback_init: failed resuming callback thread");
	}

	/*
	 * Finally, register the callback thread and its port with the server,
	 * and receive the rlimit_fsize in return (only the MAPPED_FILES
	 * configuration uses rlimit_fsize).
	 */
	rc = bsd_register_proc(our_bsd_server_port, callback_thread,
		 	      callback_port, &rlimfsize);
	if (rc != KERN_SUCCESS) {
		EPRINT(("bsd_register_proc(t=%x,p=%x)=%x\n",
			callback_thread, callback_port, rc));
		emul_panic("callback_init: failed registering callback");
	}	
#ifdef	MAPPED_FILES
	rlimit_fsize = rlimfsize;	
#endif
}

/*
 * Register an operation that will block, either locally (in the emulator;
 * give forwport as MACH_PORT_NULL) or remotely (when a forward port is
 * given).
 *
 *
 * While it would be possible for a thread to be registered as both
 * TT_LOCAL and TT_FORWARDED, there is no need for this ability,
 * and thus this interface prevents it.
 */
void
isc_register(forwport, transidp)
	mach_port_t		forwport;
	transaction_id_t	*transidp;
{
	thread_t		my_thread_id = emul_thread_id_self();
	transaction_id_t	transid;
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;
	unsigned short		flags;

	flags = (forwport == MACH_PORT_NULL)
			? TT_LOCAL
			: (forwport == MACH_PORT_DEAD)
				? TT_BUSY
				: TT_FORWARDED;

	TRANS_LOCK();
	ttend = &transactions[trans_tbl_hi];

	/*
	 * It is OK to wrap, as long we we don't use the value 0.
	 * transaction_id_t must be large enough to assure that
	 * two system calls with the same transaction_id won't be
	 * around at the same time.  You'd be surprised, but this
	 * really could be a problem.
	 */
	if (!++transaction_id)
		++transaction_id;
	transid = transaction_id;
	if (transidp)
		*transidp = transid;

	for (; tt < ttend; tt++) {
		if (!(tt->inprogress) && (tt->flags == TT_IDLE)) {
			tt->inprogress = TRUE;
			tt->thread_id = my_thread_id;
			tt->transaction_id = transid;
			tt->forward_port = forwport;
#ifdef PFS
			tt->transaction_id_cnt = 0;
			tt->abort = PFS_ABORT_ASYNC_NOP;
			tt->pfs_fd = NULL;
			tt->interrupt = NULL;
#endif
			break;
		}
#ifndef	PFS
		else {
			EASSERT(tt->thread_id != my_thread_id);
		}
#endif
	}

	/*
	 * Extend the table.
	 */
	if (tt == ttend) {
		EASSERT(tt == &transactions[trans_tbl_hi]);

		/* A thread limit is BOGUS */
		if (trans_tbl_hi >= MAX_THREADS)
			emul_panic("isc_register: too many threads");

		tt->inprogress = TRUE;
		tt->flags = TT_IDLE;
		tt->thread_id = my_thread_id;
		tt->transaction_id = transid;
		tt->forward_port = forwport;
#ifdef PFS
		tt->abort = PFS_ABORT_ASYNC_NOP;
		tt->transaction_id_cnt = 0;
		tt->pfs_fd = NULL;
#endif

		++trans_tbl_hi;
	}

	/*
	 * We must increment this inside the locked region.
	 * However, semul_release_state() might decide not to resume
	 * anyone because of it.  This would mean we could be blocked
	 * holding the TRANS_LOCK.  This is OK, as NO-ONE would need
	 * to be resumed, so no dead-lock can result.
	 */
	trans_blocked++;
	if (MACH_PORT_VALID(forwport))
		trans_forwardable++;
	TRANS_UNLOCK();
	tt->flags = flags;
	tt->inprogress = FALSE;
	if(tt->resume) {
		tt->resume = FALSE;
		if (tt->flags & TT_BUSY)
			syscall_suspend_barrier();
	}
}


/*
 * Unregister an operation and give up the transaction id.
 * Return indication as to if there has been an interrupt request.
 * Note: we do not initialize "*interrupt"; this allows multiple calls
 * to isc_reg/isc_dereg calls to properly carry forward an interrupt value.
 */
void
isc_deregister(interrupt)
	int			*interrupt;
{
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;
	thread_t		my_thread_id = emul_thread_id_self();

	/*
	 * ALERT.
	 * We DO NOT take the TRANS_LOCK() here to prevent a dead-lock
	 * in the semul_release_state code.  If we did, there would be a
	 * window where we could get thread_suspend'd before clearing TT_IDLE,
	 * leaving us marked as a "safely" blocked thread.  BUT, in fact
	 * we'd be holding an important lock.
	 * Workaround is to add an inprogress indication so that release_state
	 * can see that to determine we are "no longer safe".  We can't
	 * just set flags to TT_IDLE 'cause this could lead to the bucket
	 * being reused before we are done clearing it.
	 * I had thought to make the bucket free operation atomic by looking
	 * at TT_INTERRUPT first and then just setting TT_IDLE, w/o scrubbing
	 * the bucket contents and w/o taking TRANS_LOCK.  However, you then 
	 * lose synchronization with semul_forward_signal.
	 */
	ttend = &transactions[trans_tbl_hi];

	for (; tt < ttend; tt++) {
		if (tt->thread_id == my_thread_id) {
#ifdef PFS
			if (tt->flags & TT_ASYNC) { 

				if (tt->abort == PFS_ABORT_ASYNC_IPC){
				  /*
				   * If we are an asynchronous thread and
				   * the abort flag was set (via a register
				   * operation from an PFS I/O mode operation)
				   * just reset the abort flag and return.
				   */
					tt->abort = PFS_ABORT_ASYNC_NOP;

				} else if ( tt->abort == PFS_ABORT_ASYNC_RPC){
				  /*
				   * If we are an asynchronous thread and the
				   * abort flag was set to indicate an RPC.
				   * Then reset the flag and do some cleanup.
				   */
					tt->abort = PFS_ABORT_ASYNC_NOP;
					TRANS_LOCK();
					if (MACH_PORT_VALID(tt->forward_port))
						trans_forwardable--;
					tt->transaction_id = 0;
					tt->transaction_id_cnt = 0;
					tt->forward_port = MACH_PORT_NULL;
					TRANS_UNLOCK();
				}
				return;
			}
#endif
			tt->inprogress = TRUE;
			TRANS_LOCK();
			trans_blocked--;
			if (MACH_PORT_VALID(tt->forward_port))
				trans_forwardable--;
			if (tt->flags & TT_INTERRUPT && interrupt)
				*interrupt = TRUE;
			tt->flags = TT_IDLE;
			tt->thread_id = (emul_thread_id)0;
			tt->resume = FALSE;
			tt->transaction_id = 0;
			tt->forward_port = MACH_PORT_NULL;
#ifdef PFS
			tt->transaction_id_cnt = 0;
			tt->abort = PFS_ABORT_ASYNC_NOP;
			tt->pfs_fd = NULL;
			tt->interrupt = NULL;
#endif
			TRANS_UNLOCK();
			tt->inprogress = FALSE;
#ifdef DEBUG
			if (callback_debug) {
				EPRINT(("*****isc_dereg id=%x intr %d",
					my_thread_id,
					interrupt ? *interrupt : -1));
			}
#endif
			break;
		}
	}
	if (tt == ttend) {
		EPRINT(("isc_deregister: id %x not found (%d in use)!\n",
			my_thread_id, trans_tbl_hi));
	}
}

#ifdef PFS
/*
 * Register a PFS operation that will block remotely. A forward port is
 * always given either by forwport or by obtaining the port from the 
 * pfs_fd data structure.
 */
void
isc_multi_register(forwport, num_opts, pfs_fd, transidp)
	mach_port_t		forwport;	/* Port operation sent on. */
	uint_t			num_opts;	/* Number of operations. */
	pfs_fd_t		*pfs_fd;	/* If not NULL, contains the 
						   file ports to be used 
						   instead of forwport. */
	transaction_id_t	*transidp;	/* Transaction id base 
						   pointer. */
{

	thread_t		my_thread_id = emul_thread_id_self();
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;
	unsigned short		flags;
	transaction_id_t	base_transid;

	flags = TT_FORWARDED;

	TRANS_LOCK();
	ttend = &transactions[trans_tbl_hi];

	/*
	 * It is OK to wrap, as long we we don't use the value 0.
	 * transaction_id_t must be large enough to assure that
	 * two system calls with the same transaction_id won't be
	 * around at the same time.  You'd be surprised, but this
	 * really could be a problem.
	 */
	if (!++transaction_id)
		++transaction_id;
	base_transid = transaction_id;
	transaction_id += num_opts;

	if (transaction_id < base_transid) {
		base_transid = 1;
		transaction_id = base_transid + num_opts;
	}

	if (transidp)
		*transidp = base_transid;

	for (; tt < ttend; tt++) {
		if (!(tt->inprogress) && (tt->flags == TT_IDLE)) {
			tt->inprogress = TRUE;
			tt->thread_id = my_thread_id;
			tt->transaction_id = base_transid;
			tt->forward_port = forwport;
			tt->transaction_id_cnt = num_opts;
			tt->pfs_fd = pfs_fd;
			tt->interrupt = NULL;
			tt->abort = PFS_ABORT_ASYNC_NOP;
			break;
		}
	}

	/*
	 * Extend the table.
	 */
	if (tt == ttend) {
		EASSERT(tt == &transactions[trans_tbl_hi]);

		/* A thread limit is BOGUS */
		if (trans_tbl_hi >= MAX_THREADS)
			emul_panic("isc_register: too many threads");

		tt->inprogress = TRUE;
		tt->flags = TT_IDLE;
		tt->thread_id = my_thread_id;
		tt->transaction_id = base_transid;
		tt->forward_port = forwport;
		tt->transaction_id_cnt = num_opts;
		tt->pfs_fd = pfs_fd;
		tt->interrupt = NULL;
		tt->abort = PFS_ABORT_ASYNC_NOP;
		++trans_tbl_hi;
	}

	/*
	 * We must increment this inside the locked region.
	 * However, semul_release_state() might decide not to resume
	 * anyone because of it.  This would mean we could be blocked
	 * holding the TRANS_LOCK.  This is OK, as NO-ONE would need
	 * to be resumed, so no dead-lock can result.
	 */
	trans_blocked++;
	TRANS_UNLOCK();
	tt->flags = flags;
	tt->inprogress = FALSE;
        if(tt->resume) {
                tt->resume = FALSE;
		if (tt->flags & TT_BUSY)
                	syscall_suspend_barrier();
        }

}

/*
 * Register an asynchronous operation.
 */
void
isc_async_register(transidp, interrupt)
	transaction_id_t	*transidp;
	int			*interrupt;
{
	thread_t		my_thread_id = emul_thread_id_self();
	transaction_id_t	transid;
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;
	unsigned short		flags;
	extern unsigned long	on_emul_stack;

	flags = TT_ASYNC;

	TRANS_LOCK();
	ttend = &transactions[trans_tbl_hi];

	/*
	 * It is OK to wrap, as long we we don't use the value 0.
	 * transaction_id_t must be large enough to assure that
	 * two system calls with the same transaction_id won't be
	 * around at the same time.  You'd be surprised, but this
	 * really could be a problem.
	 */
	if (!++transaction_id)
		++transaction_id;
	transid = transaction_id;
	if (transidp)
		*transidp = transid;

	for (; tt < ttend; tt++) {
		if (!(tt->inprogress) && (tt->flags == TT_IDLE)) {
			tt->inprogress = TRUE;
			tt->thread_id = my_thread_id;
			tt->transaction_id = transid;
			tt->forward_port = MACH_PORT_NULL;
			tt->transaction_id_cnt = 0;
			tt->pfs_fd = NULL;
			tt->interrupt = interrupt;
			tt->abort = PFS_ABORT_ASYNC_NOP;
			break;
		}
		else {
			EASSERT(tt->thread_id != my_thread_id);
		}
	}

	/*
	 * Extend the table.
	 */
	if (tt == ttend) {
		EASSERT(tt == &transactions[trans_tbl_hi]);

		/* A thread limit is BOGUS */
		if (trans_tbl_hi >= MAX_THREADS)
			emul_panic("isc_register: too many threads");

		tt->inprogress = TRUE;
		tt->flags = TT_IDLE;
		tt->thread_id = my_thread_id;
		tt->transaction_id = transid;
		tt->forward_port = NULL;
		tt->transaction_id_cnt = 0;
		tt->pfs_fd = NULL;
		tt->interrupt = interrupt;
		tt->abort = PFS_ABORT_ASYNC_NOP;

		++trans_tbl_hi;
	}

	/*
	 * We must increment this inside the locked region.
	 * However, semul_release_state() might decide not to resume
	 * anyone because of it.  This would mean we could be blocked
	 * holding the TRANS_LOCK.  This is OK, as NO-ONE would need
	 * to be resumed, so no dead-lock can result.
	 */
	trans_blocked++;
	on_emul_stack++;
	TRANS_UNLOCK();
	tt->flags = flags;
	tt->inprogress = FALSE;
        if(tt->resume) {
                tt->resume = FALSE;
		if(tt->flags & TT_BUSY)
                	syscall_suspend_barrier();
        }

#ifdef DEBUG_ISC
	e_printf("isc_async_register: Registering tt = %x, thread = %x\n",
		tt, tt->thread_id);
#endif
}

/*
 * Deregister an asynchronous operation.
 */

void
isc_async_deregister(thread_id)
	thread_t		thread_id;
{
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;
	extern unsigned long	on_emul_stack;

	/*
	 * ALERT.
	 * We DO NOT take the TRANS_LOCK() here to prevent a dead-lock
	 * in the semul_release_state code.  If we did, there would be a
	 * window where we could get thread_suspend'd before clearing TT_IDLE,
	 * leaving us marked as a "safely" blocked thread.  BUT, in fact
	 * we'd be holding an important lock.
	 * Workaround is to add an inprogress indication so that release_state
	 * can see that to determine we are "no longer safe".  We can't
	 * just set flags to TT_IDLE 'cause this could lead to the bucket
	 * being reused before we are done clearing it.
	 * I had thought to make the bucket free operation atomic by looking
	 * at TT_INTERRUPT first and then just setting TT_IDLE, w/o scrubbing
	 * the bucket contents and w/o taking TRANS_LOCK.  However, you then 
	 * lose synchronization with semul_forward_signal.
	 */
	ttend = &transactions[trans_tbl_hi];

#ifdef DEBUG_ISC
	e_printf("isc_async_deregister: thread = %x\n", thread_id);
#endif

	for (; tt < ttend; tt++) {
		if (tt->thread_id == thread_id) {
#ifdef DEBUG_ISC
	e_printf("isc_async_deregister: tt = %x\n", tt);
#endif
			tt->inprogress = TRUE;
			TRANS_LOCK();
			trans_blocked--;
			tt->flags = TT_IDLE;
			tt->inprogress = FALSE;
			tt->thread_id = (emul_thread_id)0;
			tt->resume = FALSE;
			tt->transaction_id = 0;
			tt->forward_port = MACH_PORT_NULL;
			tt->transaction_id_cnt = 0;
			tt->pfs_fd = NULL;
			tt->interrupt = NULL;
			tt->abort=PFS_ABORT_ASYNC_NOP;
			on_emul_stack--;
			TRANS_UNLOCK();
			break;
		}
	}
	if (tt == ttend) {
		EPRINT(("isc_async_deregister: id %x not found (%d in use)!\n",
			thread_id, trans_tbl_hi));
	}
}

/*
 * This function is a post processor to the isc_register function in that
 * it checks first to see if the thread is asynchrounous and handles this
 * case differently otherwise the isc_register() function is called.
 */
void
isc_register_chk_async(forwport, transidp)
	mach_port_t		forwport;
	transaction_id_t	*transidp;
{
	thread_t		my_thread_id = emul_thread_id_self();
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;
        transaction_id_t        transid;

	TRANS_LOCK();
	ttend = &transactions[trans_tbl_hi];
	/*
	 * Check to see if there is an async thread
	 * already registered for this thread id.
	 */
	for(; tt<ttend; tt++) {
		if (tt->flags & TT_ASYNC) {
			if (tt->thread_id == my_thread_id) {
				if ( forwport == MACH_PORT_NULL) {
					/*
					 * We are registering a mach 
					 * message suspension due to 
					 * PFS I/O mode code.
					 */
					tt->abort = PFS_ABORT_ASYNC_IPC;
					TRANS_UNLOCK();
					return;
				} else {
					/*
					 * We are registering an operation.
					 */
				        if (!++transaction_id)
                				++transaction_id;
        				transid = transaction_id;
        				if (transidp)
                				*transidp = transid;
                        		tt->transaction_id = transid;
                        		tt->forward_port = forwport;
					tt->abort = PFS_ABORT_ASYNC_RPC;
					TRANS_UNLOCK();
					return;
				}
			}
		}
	}
	/*
	 * This thread is not asynchronous, so just register the 
	 * operation normally.
	 */
	TRANS_UNLOCK();
	isc_register(forwport, transidp);
	return;
}


/*
 * This function is a post processor to the isc_multi_register function in that
 * it checks first to see if the thread is asynchrounous and handles this
 * case differently otherwise the isc_multi_register() function is called.
 */
void
isc_multi_register_chk_async(forwport, num_opts, pfs_fd, transidp)
        mach_port_t             forwport;       /* Port operation sent on. */
        uint_t                  num_opts;       /* Number of operations. */
        pfs_fd_t                *pfs_fd;        /* If not NULL, contains the
                                                   file ports to be used
                                                   instead of forwport. */
        transaction_id_t        *transidp;      /* Transaction id base
                                                   pointer. */
{
	thread_t		my_thread_id = emul_thread_id_self();
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;
        transaction_id_t        base_transid;

	TRANS_LOCK();
	ttend = &transactions[trans_tbl_hi];
	/*
	 * Check to see if there is an async thread
	 * already registered for this thread id.
	 */
	for(; tt<ttend; tt++) {
		if (tt->flags & TT_ASYNC) {
			if (tt->thread_id == my_thread_id) {
				/*
				 * We are registering an operation.
				 */
			        if (!++transaction_id)
                			++transaction_id;
				base_transid = transaction_id;
				transaction_id += num_opts;

				if (transaction_id < base_transid) {
					base_transid = 1;
					transaction_id = base_transid + 
							 num_opts;
				}
				if (transidp)
					*transidp = base_transid;

				tt->transaction_id = base_transid;
                        	tt->forward_port = forwport;
				tt->transaction_id_cnt = num_opts;
				tt->pfs_fd = pfs_fd;
				tt->abort = PFS_ABORT_ASYNC_RPC;
				TRANS_UNLOCK();
				return;
			}
		}
	}
	/*
	 * This thread is not asynchronous, so just register the 
	 * operation normally.
	 */
	TRANS_UNLOCK();
	isc_multi_register(forwport, num_opts, pfs_fd, transidp);
	return;
}
#endif

/*
 * Interrupt the operation described by the given transaction table entry.
 * This assumes the transaction table is locked.
 */
void
tt_interrupt(tt, thread, intr, issignal)
	TRANS_TBL_T     	*tt;
	thread_t		thread;
	boolean_t		intr;
	boolean_t		issignal;
{
	kern_return_t		rc;
	mach_port_t		forwport = MACH_PORT_NULL;
	transaction_id_t	transid;

	if (tt->flags != TT_IDLE) {
		/*
		 * We've got something to interrupt
		 */
		if (issignal)
			tt->flags |= TT_INTERRUPT;

		if (tt->flags & TT_LOCAL) {
			/* stop the mach_msg */
			rc = thread_abort(thread);
				if (rc != KERN_SUCCESS)
			         EPRINT(("tt_interrupt: thread_abort(%x) returns %x\n",
						thread, rc));
		} else
		if (tt->flags & TT_FORWARDED) {
			forwport = tt->forward_port;
			transid = tt->transaction_id;
		} else
		if (tt->flags & TT_BUSY) {
			/* do nothing */
		} else
#ifdef PFS
		if (tt->flags & TT_ASYNC) {
			if (tt->interrupt) {
				*tt->interrupt = 1;
			}
			if (tt->abort == PFS_ABORT_ASYNC_IPC) {
				rc = thread_abort(thread);
				if (rc != KERN_SUCCESS) {
					EPRINT(("tt_interrupt: thread_abort(%x) returns %x\n",
						thread, rc));
				}
			} else if (tt->abort == PFS_ABORT_ASYNC_RPC) {
				/* Fall through to let the regular abort
				 * logic take over.
				 */
			} else {
				/* 
				 * Nothing to interrupt:
				 */
				return;
			}
		} else
#endif
			emul_panic(("tt_interrupt: no interrupt action!?"));
	} else {
		/*
		 * No thread in the emulator to take the signal, yet we're
		 * only here 'cause the PM didn't find the thread in
		 * user-space.  It might be doing only mapped-files ops,
		 * and never checking signals.  Solution: mark a global
		 * flag checked at the end of emul_syscall().
		 * No locking done around this(!!).
		 */
		if (issignal)
			emul_interrupt = TRUE;
	}

#ifdef PFS
	if (!tt->transaction_id_cnt) { 
		/*
		 * Do normal interrupt handling if tt->transaction_id_cnt
		 * is set to zero.  This indicates that the operation is
		 * not on a PFS file.
		 */
		if (MACH_PORT_VALID(forwport))
#ifdef DEBUG_ISC
	printf("Sending interrupt delivery\n"); 
#endif

			intr_delivery(	forwport, 
					credentials_port, 	
					transid, 
				        intr,
					FALSE);
	} else {
		/*
		 *	All PFS operations have a transaction id_cnt 
		 *	greater than 0.  The transaction_id_cnt indicates
		 *      the number of asynchronous RPCs that were sent
		 *	to carry out the PFS operation.  In order to 
		 *	signal each of the different servers involved 
		 *   	in the operation, the intr_delivery() function
		 * 	must be called for each server.
		 */
		int i;
		transaction_id_t snd_trans_id = transid;

		if (tt->pfs_fd != NULL) {
			stripe_fd_t *sf_fdt = tt->pfs_fd->p_stripe_fdt;
			/*
			 * Need to send the intr_delivery to each of the 
			 * stripe file servers indicated in the pfs_fd.
			 * data structure.
			 */
			for(i=0; i<tt->transaction_id_cnt; i++) {
				intr_delivery(	sf_fdt[i].s_fp, 
						credentials_port, 
						snd_trans_id++, 
					        intr,
						FALSE);

			}

		} else {
			/* 
			 * Need to send multiple intr_delivery to the port
			 * specified by forwport since all operations
			 * originated there.
			 */
			for(i=0; i<tt->transaction_id_cnt; i++) {
				if (MACH_PORT_VALID(forwport)) {
					intr_delivery(	forwport, 
							credentials_port, 
							snd_trans_id++, 
						        intr,
							FALSE);
				}
			}
		}
	}
#else
	/* have message follow to where syscall was forwarded to */
	if (MACH_PORT_VALID(forwport))
		intr_delivery(forwport, credentials_port, transid, intr, FALSE);
#endif
}

/*
 * Process an interrupt "operation-in-progress" request from PM server.
 * This has been recv'd on the callback port (port).
 *
 * Following OSF/1 semantics, we only interrupt the first thread of the task.
 */
kern_return_t
semul_forward_signal(port, intr)
	mach_port_t	port;
	boolean_t	intr;
{
	kern_return_t		rc;
	mach_msg_type_number_t	thread_cnt;
	int			th;
	thread_array_t		thread_table;
	thread_t		signal_thread;
	emul_thread_id		signal_thread_id;
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;

	/*
	 * Can't task_suspend around this, as that would stop us, too!
	 */
	rc = task_threads(mach_task_self(), &thread_table, &thread_cnt);
	if (rc != KERN_SUCCESS) {
                EPRINT(("semul_forward_signal: task_threads failed\n"));
		(void)task_terminate(mach_task_self());
		/*NOTREACHED*/
	}
	if (thread_cnt < 2) {
		EPRINT(("semul_forward_signal: no other threads\n"));
		(void)task_terminate(mach_task_self());
		/*NOTREACHED*/
	}

	signal_thread = thread_table[0];
	if (signal_thread == callback_thread)
		signal_thread = thread_table[1];
	signal_thread_id = emul_thread_id_of(signal_thread, NULL);

	TRANS_LOCK();
	ttend = &transactions[trans_tbl_hi];

	/*
	 * Find the thread to interrupt.
	 * By not checking TT_IDLE, we may cause the loop to terminate quicker
	 */
	for (; tt < ttend; tt++)
		if (tt->thread_id == signal_thread_id)
			break;

	/** Fire off async interruption message */
	if (tt < ttend && tt->flags != TT_IDLE)
		tt_interrupt(tt, signal_thread, intr, TRUE);

#ifdef PFS
	if (thread_cnt > 2) {
		tt = &transactions[0];
		for(; tt < ttend; tt++) {
			if (tt->flags &  TT_ASYNC) {
				tt_interrupt(tt, signal_thread, intr, TRUE);
			}
		}
	}
#endif
	TRANS_UNLOCK();

	for (th = 0; th < thread_cnt; th++)
		(void)mach_port_deallocate(mach_task_self(), thread_table[th]);
	(void)vm_deallocate(mach_task_self(), (vm_offset_t)thread_table, 
			     (vm_size_t)(thread_cnt * sizeof(thread_table)));

	/* we're an async message; no real return code */
	return (KERN_SUCCESS);
}

kern_return_t
semul_token_revoke(revoke_port, token_port)
	mach_port_t		revoke_port;
	mach_port_t		token_port;
{
#if	MAPPED_FILES | PFS
	token_revoke(token_port);
#else
	EPRINT(("Error: semul_token_revoke shouldn't be called!"));
#endif	
	return(KERN_SUCCESS);
}

#ifdef PFS
kern_return_t
semul_token_acquire(revoke_port, fdte_ref, node, offset, length)
	mach_port_t	revoke_port;	/* in */
	int		fdte_ref;	/* in */
	int		node;		/* in */
	esize_t		*offset;	/* out */
	esize_t		*length;	/* out */
{
	fdt_entry_t	*fdte = (fdt_entry_t *)fdte_ref;
	int		error= ESUCCESS;
	pfs_token_mgr_info *info;

	info = fdte->pfs_iomode_info->token_mgr_info;
	/*
	 * Check to see if in the middle of a revoke, if
	 * so then make the client retry:
	 */
        if (info->tmgr_token_state == PFS_TOKEN_RIP) {
		*offset = ex_neg_one;	/* offset will be -1 only if unable to
					 * acquire the token and should retry
					 */
		*length = ex_neg_one;
		return KERN_SUCCESS;
	}

	/*
	 * Check the flags to see if the token is already
	 * acquired.
	 */
	if (!info->flags) {
		/*
		 * Re-acquire the token from the server:
		 */
		error = tmgr_token_acquire_from_server(fdte, 
                                               	    offset, 
				        	    length);
	}
	/*
	 * Update the length and offset.
	 */
	*offset = ex_zero;	/* Offset only used to signal token has 
				 * been acquired.
				 */
	if (!error) {
		*length = info->length;
		info->clnt_tok[node].token_state = PFS_TOKEN_ACQUIRED;
        	info->token_cnt++;
	} else {
		*length = ex_neg_one;
	}
	return(KERN_SUCCESS);
}


semul_token_release(revoke_port, fdte_ref, node, accessed, modified, 
		    offset, length)
	mach_port_t	revoke_port;
	int		fdte_ref;
	int		node;
	int		accessed;
	int		modified;
	esize_t		offset;
	esize_t		length;
{
	fdt_entry_t	*fdte = (fdt_entry_t *)fdte_ref;
        pfs_token_mgr_info      *info;

	fdte_lock(fdte);
        info = fdte->pfs_iomode_info->token_mgr_info;
#ifdef DEBUG_SETIO
	e_printf("semul_token_release: info = %x, info->tmgr_token_state = %d\n", info, info->tmgr_token_state);
#endif

        if (info->tmgr_token_state == PFS_TOKEN_RIP) {
		/*
	 	 * Update the file length to reflect greatest value:
	 	 */
#ifdef DEBUG_SETIO
	e_printf("semul_token_release: info->token_cnt = %d\n", info->token_cnt);
#endif
                info->clnt_tok[node].token_state = PFS_TOKEN_RELEASED;
                info->token_cnt--;
                /*
                 * Update the file offset and flag information:
                 */
                info->offset = EMAX(info->offset, offset);
                info->length = EMAX(info->length, length);
                info->accessed |= accessed;
                info->modified |= modified;
		/*
		 * See if this is the last client, if so then release the
		 * token back to the server.
		 */	
		if (info->token_cnt == 0) {
       			info->tmgr_token_state = PFS_TOKEN_RELEASED;
			tmgr_release_to_server(fdte, 1);
		}
	}
	fdte_unlock(fdte);
	return(KERN_SUCCESS);
}

#endif

/*
 * WARNING.
 *
 * This assumes we are being called on the behalf of unix_task_suspend().
 *
 * WE CANNOT TAKE ANY LOCKS OTHER THAN THE SUSPEND_LOCK.
 */
kern_return_t
semul_release_state(callback_port, reply_port, reply_port_type, intr, is_exit)
	mach_port_t		callback_port;
	mach_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	boolean_t		intr;
	boolean_t		is_exit;
{
	kern_return_t    	rc;
	mach_msg_type_number_t	thread_cnt;
	thread_array_t   	thread_table;
	int              	th, resumable_threads=0, running_threads=0;
	TRANS_TBL_T     	*tt;
	TRANS_TBL_T     	*ttend = &transactions[trans_tbl_hi];
	unsigned int		pc, id;
	extern unsigned long	on_emul_stack;

	struct thread_basic_info bi;
	unsigned int		bi_count;


#ifdef DEBUG
	if (suspend_debug)
		EPRINT(("semul_release_state(%x, i%d e%d) oes=%d tb=%d tbf=%d",
			callback_port, intr, is_exit, on_emul_stack,
			trans_blocked, trans_forwardable));
#endif

	if (must_suspend) {
		emul_panic("semul_release_state: already must_suspend!");
	}

	SUSPEND_LOCK();

	rc = task_threads(mach_task_self(), &thread_table, &thread_cnt);
	if (rc != KERN_SUCCESS) {
                EPRINT(("semul_release_state: task threads failed, error = 0x%x\n",
			rc));
		/* XXX: should call exit? */
		(void)task_terminate(mach_task_self());
		/*NOTREACHED*/
	}
	if (thread_cnt < 2) {
		EPRINT(("semul_release_state: no other threads\n"));
		/* XXX: should call exit? */
		(void)task_terminate(mach_task_self());
		/*NOTREACHED*/
	}
	/*
	 * First, stop all threads in this task, ASAP.
	 */
	for (th = 0; th < thread_cnt; th++) {
		if (thread_table[th] != callback_thread) {
			rc = thread_suspend(thread_table[th]);
			if (rc != KERN_SUCCESS) {
				EPRINT(("semul_release_state: thread_suspend(%x) returns %x\n",
					thread_table[th], rc));
			}
		}
	}

	/* XXX
	 * Should check if another thread was just created by a thread we
	 * just suspended.  You cannot just check the thread count, but
	 * instead must check that either (1) every thread is still the
	 * same, or (2) every thread has a susp count of at least 1, or
	 * both (1) and (2).
	 *
	 * This is deemed too much work for such an unlikely occurrence.
	 *
	 *
	 * (January 17, 1995)  This "unlikely occurrence" was encountered
	 * and reported by PTS #11035.  Rather than ignore this case, as
	 * implied by the comments around the emul_panic, check if another
	 * thread was just created by a thread we just suspended:
	 */

	for (th = 0; th < thread_cnt; th++) {
	  (void)mach_port_deallocate(mach_task_self(),
				     thread_table[th]);
	}
	(void)vm_deallocate(mach_task_self(), (vm_offset_t)thread_table, 
			    (vm_size_t)thread_cnt*sizeof(thread_table));

	rc = task_threads(mach_task_self(), &thread_table, &thread_cnt);
	if (rc != KERN_SUCCESS) {
	  EPRINT(("semul_release_state: task threads failed, error = 0x%x\n",
		  rc));
	  /* XXX: should call exit? */
	  (void)task_terminate(mach_task_self());
	  /*NOTREACHED*/
	}

	for (th = 0; th < thread_cnt; th++) {
	  if (thread_table[th] != callback_thread) {

	    bi_count = THREAD_BASIC_INFO_COUNT;
	    rc = thread_info(thread_table[th],
			     THREAD_BASIC_INFO,
			     (thread_info_t)&bi,
			     &bi_count);
	    if (rc != KERN_SUCCESS) {
	      EPRINT(("semul_release_state: task info failed, error = 0x%x\n",
		      rc));
	      break;
	    }

	    if (bi.suspend_count == 0) {
	      /*
	       * We found a case where a new thread was created by a
	       * thread suspended in the above loop!  Suspend this new
	       * thread now:
	       */
	      rc = thread_suspend(thread_table[th]);
	      if (rc != KERN_SUCCESS) {
		EPRINT(("semul_release_state: thread_suspend(%x) returns %x\n",
			thread_table[th], rc));
	      }
	    }
	  }
	}


	suspend_is_exit = is_exit;
	suspend_is_intr = intr;
	suspend_count = 0;
	must_suspend = TRUE;

	if (!++suspend_generation)
		/* clear values when we wrap */
		for (tt = &transactions[0]; tt < ttend; tt++)
			tt->generation = 0;

	/*
	 * If we have no threads in the emulator,
	 * then we have no hard work to do; we can just release state.
	 */
	if (on_emul_stack == 0) {
		SUSPEND_UNLOCK();
		/* clean up from task_threads */
		for (th = 0; th < thread_cnt; th++)
			(void)mach_port_deallocate(mach_task_self(),
						   thread_table[th]);
		(void)vm_deallocate(mach_task_self(), (vm_offset_t)thread_table,
				    (vm_size_t)thread_cnt*sizeof(thread_table));

		release_state(is_exit);
		return KERN_SUCCESS;
	}

	/*
	 * Threads in this task are in one of a few states:
	 *	user
	 *	emulator
	 *	local-syscall (select/poll)
	 *	fsvr-syscall
	 *	pm-syscall
	 *	callback (ourself)
	 * We toss of out the thread_table ourselves, any threads in user
	 * space, and anything we get an error on.  Any thread registered
	 * as interrupible gets hit immediately.  All remaing threads will
	 * be resumed when we are done processing the whole table.
	 * We do it this way to assure us the emulator state stays quiescent
	 * while we're peeking in transactions.
	 */
	for (th = 0; th < thread_cnt; th++) {
		if (thread_table[th] == callback_thread)
			goto disavow;
		if (emul_thread_get_state(thread_table[th],
					   (thread_state_t)NULL,
					   &pc, &id) != KERN_SUCCESS) {
			EPRINT(("semul_release_state: emul_thread_get_state(%x) returns %x\n",
					thread_table[th], rc));
			goto disavow;
		}
#ifdef DEBUG
		if (suspend_debug > 1)
			EPRINT(("semul_release_state: thread %d %x pc=%x id=%x",
				th, thread_table[th], pc, id));
#endif

		if (pc >= EMULATOR_BASE && pc <= EMULATOR_END) {
			/*
			 * Thread is in the emulator; we scan the
			 * transactions table to see if is registered.
			 * If so, and it is not currently being manipulated,
			 * it is "safe" and will not need to be resumed.
			 */
			for (tt = &transactions[0]; tt < ttend; tt++)
				if (tt->flags != TT_IDLE &&
				    tt->thread_id == id)
					break;

			if (tt != ttend) {
				tt->generation = suspend_generation;
				tt->thread = thread_table[th];
				if(!(tt->inprogress)) {
					if (tt->flags & TT_BUSY)
						goto disavow;
				} else
					tt->resume = TRUE;
				tt_interrupt(tt, thread_table[th], intr, FALSE);
			}

			/* this thread will be resumed */
			resumable_threads++;
			continue;
		}

		/* in user space */

disavow:
		/* We won't resume this one, dump the port */
		(void)mach_port_deallocate(mach_task_self(), thread_table[th]);
		thread_table[th] = MACH_PORT_NULL;
	}

#ifdef DEBUG
	if (suspend_debug)
		EPRINT(("semul_release_state: resumable %d",
			resumable_threads));
#endif

/*#ifdef MACH_ASSERT*/
	/*
	 * Make sure we've accounted for everything
	 */
	for (tt = &transactions[0]; tt < ttend; tt++)
		if (tt->flags != TT_IDLE &&
		    tt->generation != suspend_generation) {
			EPRINT(("tt=%x gen=%d flags=%d port=%x sgen=%d",
				tt, tt->generation, tt->flags, tt->forward_port,
				suspend_generation));
			emul_panic("release_state: no thread matching transaction");
		}
/*#endif*/

	suspend_count = resumable_threads;
	SUSPEND_UNLOCK();

#ifdef DEBUG_ISC
if (suspend_debug)
	EPRINT(("semul_release_state resuming %d", resumable_threads));
#endif

	/*
	 * Resume any threads that need resuming
	 */
	if (resumable_threads > 0)
		for (th = 0; th < thread_cnt; th++) {
			if (thread_table[th] == MACH_PORT_NULL)
				continue;
			running_threads++;
			if (thread_resume(thread_table[th]) != KERN_SUCCESS) {
				EPRINT(("semul_release_state: thread_resume(%x) returns %x\n",
					thread_table[th], rc));
			}
			(void)mach_port_deallocate(mach_task_self(), thread_table[th]);
		}

	(void)vm_deallocate(mach_task_self(), (vm_offset_t)thread_table, 
			     (vm_size_t)thread_cnt*sizeof(thread_table));

	EASSERT(running_threads == resumable_threads);

	if (resumable_threads == 0) {
		/*
		 * No thread to run, so we clean up ourselves
		 */
		release_state(is_exit);

		/* reply ourselves */
		return KERN_SUCCESS;
	} else {
		/*
		 * When the last resumed thread hits the barrier, it will
		 * signal the callback thread (*this thread*) to generate
		 * the reply message.
		 */
		rs_reply_port = reply_port;
		rs_reply_port_type = reply_port_type;

		return MIG_NO_REPLY;
	}
}

void
suspend_interrupt_all()
{
	TRANS_TBL_T     	*tt = &transactions[0];
	TRANS_TBL_T     	*ttend;
	int			kicked = 0;

	TRANS_LOCK();
	ttend = &transactions[trans_tbl_hi];

#ifdef DEBUG
	if (suspend_debug)
		EPRINT(("suspend_intr_all count=%d", suspend_count));
#endif

	/*
	 * Find each thread needing a kick...
	 */
	for (; tt < ttend; tt++)
		if (tt->flags == TT_IDLE &&
		    tt->generation == suspend_generation) {
			tt_interrupt(tt, tt->thread, suspend_is_intr, FALSE);
			kicked++;
		}
	TRANS_UNLOCK();

	SUSPEND_LOCK();
	if (kicked != suspend_count) {
#ifdef DEBUG
		if (suspend_debug)
			EPRINT(("suspend_intr_all: kicked %d of %d",
				kicked, suspend_count));
#endif
	}
	SUSPEND_UNLOCK();
}

void
syscall_suspend_barrier()
{
	kern_return_t	rc;
#ifdef DEBUG
	if (suspend_debug)
		EPRINT(("syscall_suspend_barrier() count=%d", suspend_count));
#endif

	SUSPEND_LOCK();
	if (--suspend_count == 0) {
		/*
		 * We are the last thread to suspend, hand off to the 
		 * callback thread.
		 */
		SUSPEND_UNLOCK();

		/*
		 * We do the following to avoid a race condition where
		 * this thread would send the uemul_release_state_reply()
		 * followed by thread_suspend()ing ourselves.
		 * We send a message to our own callback thread to have
		 * ourselves suspended.  In the mean time, we are blocked
		 * waiting for the RPC to return.  The callback thread will
		 * suspend us, and reply to us, so that we will continue
		 * to execute once the server resumes our task.
		 */
		rc = uemul_at_barrier(callback_port, mach_thread_self());
		if (rc != ESUCCESS) {
			/* WE'RE DOOMED */
			EPRINT(("uemul_at_barrier returned %x", rc));
			(void)task_terminate(mach_task_self());
		}
		/* THIS THREAD DOESN'T SUSPEND ITSELF */
	} else {
		SUSPEND_UNLOCK();
		rc = thread_suspend(mach_thread_self());
		if (rc != ESUCCESS) {
			/* WE'RE DOOMED */
			EPRINT(("barrier: thread_suspend returned %x", rc));
			(void)task_terminate(mach_task_self());
		}
	}
}


/*
 * Common code to release emulator state when all is safe
 * (Will the last thread to run please turn out the lights?)
 */
void
release_state(is_exit)
int is_exit;
{
	kern_return_t	rc;

	EASSERT(suspend_count == 0);

	/*
	 * Destroy the revoke port.  We're about to release all our
	 * tokens and none of our threads can run, so this is safe.
	 * This will cause any queued messages to fail, so that we
	 * can be safely task_suspend'd.  In particular, it will
	 * prevent a dead-lock where a token_revoke() is queue'd
	 * waiting for the callback thread (*this thread*) to recv it.
	 */

#ifdef MAPPED_FILES
	rc = mach_port_destroy(mach_task_self(), revoke_port);
	if (rc != KERN_SUCCESS) {
		EPRINT(("mach_port_destroy(revoke port)=%x\n", rc));
		emul_panic("release_state: can't destroy revoke port");
	}	
#endif

	if (is_exit) 
		close_on_exit(1); /* close files AND release tokens */
#if MAPPED_FILES | PFS
	else {
		token_release_all(); /* release tokens */
		rc = mach_port_allocate(mach_task_self(), 
				MACH_PORT_RIGHT_RECEIVE,
				&revoke_port);
		if (rc != KERN_SUCCESS) {
			EPRINT(("mach_port_allocate(revoke port)=%x\n", rc));
			emul_panic("release_state: can't alloc revoke port");
		}	
		rc = mach_port_move_member(mach_task_self(), revoke_port, 
				   callback_port_set);
		if (rc != KERN_SUCCESS) {
			EPRINT(("mach_port_move_member(revoke port)=%x\n", rc));
			emul_panic("release_state: can't insert revoke port into set");
		}
	}

#endif

	must_suspend = FALSE;
}


kern_return_t
semul_at_barrier(callback_port, last_thread)
	mach_port_t	callback_port;
	thread_t	last_thread;
{
	kern_return_t	rc;

#ifdef DEBUG
	if (suspend_debug)
		EPRINT(("semul_at_barrier() last_thread=%x", last_thread));
#endif
	/*
	 * This thread is waiting on our RPC; we'll suspend it so that
	 * the server sees the right suspend count.
	 */
	rc = thread_suspend(last_thread);
	if (rc != ESUCCESS) {
		EPRINT(("semul_at_barrier: thread_suspend(%x) returned %x",
			last_thread, rc));
		/*XXX*/
	}

	/*
	 * Finally, we can release emulator state cleanly.
	 */
	release_state(suspend_is_exit);

	/*
	 * Send the (deferred) reply to the server from the original
	 * semul_release_state.
	 */
	rc = uemul_release_state_reply(rs_reply_port, rs_reply_port_type,
				       ESUCCESS);
	if (rc != ESUCCESS) {
		EPRINT(("semul_at_barrier: uemul_rel_st_reply returned %x",
			rc));
		/* The server is waiting on us! */
		(void)task_terminate(mach_task_self());
	}
	rs_reply_port = MACH_PORT_NULL;

	return ESUCCESS;
}


/*
 * Note: the emulator uses this version of mach_msg_server (as
 * opposed to the one in libmach) because:
 *	- it allocates memory using VM_INHERIT_NONE (as opposed to
 *	  VM_INHERIT_COPY).
 *	- it implements a rcv timeout for isc retry
 */

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/mig_errors.h>

/*
 *	Routine:	mach_msg_server
 *	Purpose:
 *		A simple generic server function.
 */

mach_msg_return_t
mach_msg_server(demux, max_size, rcv_name)
    boolean_t (*demux)();
    mach_msg_size_t max_size;
    mach_port_t rcv_name;
{
    register mig_reply_header_t *bufRequest, *bufReply, *bufTemp;
    register mach_msg_return_t mr;

#ifdef _EMULATOR
    vm_address_t address;

    address = 0;	/* mapped region will be saved across exec */
    mr = emul_vm_map(mach_task_self(), &address, 2*max_size, 0,
		     TRUE, MEMORY_OBJECT_NULL, (vm_offset_t)0, FALSE,
		     VM_PROT_DEFAULT, VM_PROT_ALL, VM_INHERIT_NONE);
    if (mr != KERN_SUCCESS)
    	return KERN_RESOURCE_SHORTAGE;
    bufRequest = (mig_reply_header_t *) address;
    bufReply = (mig_reply_header_t *) ((char *) address + max_size);

#else

    bufRequest = (mig_reply_header_t *) malloc(max_size);
    if (bufRequest == 0)
	return KERN_RESOURCE_SHORTAGE;
    bufReply = (mig_reply_header_t *) malloc(max_size);
    if (bufReply == 0)
	return KERN_RESOURCE_SHORTAGE;
#endif

    for (;;) {
      get_request:
	mr = mach_msg(&bufRequest->Head, MACH_RCV_MSG |
			(suspend_count ? MACH_RCV_TIMEOUT : 0),
		      0, max_size, rcv_name,
		      (suspend_count ? SUSPEND_TIMEOUT: MACH_MSG_TIMEOUT_NONE),
		      MACH_PORT_NULL);
	while (mr == MACH_MSG_SUCCESS) {
	    /* we have a request message */

	    (void) (*demux)(&bufRequest->Head, &bufReply->Head);

	    if (bufReply->RetCode != KERN_SUCCESS) {
		if (bufReply->RetCode == MIG_NO_REPLY)
		    goto get_request;

		/* don't destroy the reply port right,
		   so we can send an error message */
		bufRequest->Head.msgh_remote_port = MACH_PORT_NULL;
		mach_msg_destroy(&bufRequest->Head);
	    }

	    if (bufReply->Head.msgh_remote_port == MACH_PORT_NULL) {
		/* no reply port, so destroy the reply */
		if (bufReply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX)
		    mach_msg_destroy(&bufReply->Head);

		goto get_request;
	    }

	    /* send reply and get next request */

	    bufTemp = bufRequest;
	    bufRequest = bufReply;
	    bufReply = bufTemp;

	    /*
	     *	We don't want to block indefinitely because the client
	     *	isn't receiving messages from the reply port.
	     *	If we have a send-once right for the reply port, then
	     *	this isn't a concern because the send won't block.
	     *	If we have a send right, we need to use MACH_SEND_TIMEOUT.
	     *	To avoid falling off the kernel's fast RPC path unnecessarily,
	     *	we only supply MACH_SEND_TIMEOUT when absolutely necessary.
	     */

	    mr = mach_msg(&bufRequest->Head,
			  MACH_SEND_MSG|MACH_RCV_MSG|
			  ((MACH_MSGH_BITS_REMOTE(bufRequest->Head.msgh_bits) ==
						MACH_MSG_TYPE_MOVE_SEND_ONCE) ?
			  	MACH_SEND_TIMEOUT : 0)|
			  (suspend_count ? MACH_RCV_TIMEOUT : 0),
			  bufRequest->Head.msgh_size, max_size, rcv_name,
			  (suspend_count ? SUSPEND_TIMEOUT : 0),
			  MACH_PORT_NULL);
	}

	/* a message error occurred */

	switch (mr) {
	  case MACH_RCV_TIMED_OUT:
	    /* We need to retry suspend interruption */
	    if (suspend_count)
		suspend_interrupt_all();
	    break;

	  case MACH_SEND_INVALID_DEST:
	  case MACH_SEND_TIMED_OUT:
	    /* the reply can't be delivered, so destroy it */
	    mach_msg_destroy(&bufRequest->Head);
	    break;

	  case MACH_RCV_TOO_LARGE:
	    /* the kernel destroyed the request */
	    break;

	  default:
	    /* should only happen if the server is buggy */
#ifdef _EMULATOR
	    (void)vm_deallocate(mach_task_self(), address, 2*max_size);
#else
	    free((char *) bufRequest);
	    free((char *) bufReply);
#endif
	    return mr;
	}
    }
}
