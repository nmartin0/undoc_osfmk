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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: syscall_subr.c,v $
 * Revision 1.19  1995/04/08  00:19:24  yazz
 *  Authors of fix: Ray Shapouri and Bob Yasi
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 12851
 *  Testing: EATs controlc, sched, os_interfaces MUNOPS SAT runs
 *  Module(s):
 * 	server/uxkern/fsvr_msg.c
 * 	server/uxkern/fsvr_server_side.c
 * 	server/uxkern/syscall_subr.c
 * 	server/uxkern/syscall_subr.h
 * Added new routines vm_allocate_strict() and vm_deallocate_strict().  They
 * do what their corresponding Mach OS calls do, but panic on any failure.
 *
 * Revision 1.18  1995/04/08  00:10:34  yazz
 *
 * Revision 1.17  1995/02/01  22:29:40  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.16  1995/01/31  00:22:36  bolsen
 *  Reviewer: Jerry Toman, Bob Yasi, Surender Brahmaroutu
 *  Risk: Medium
 *  Benefit or PTS #: 12264
 *  Testing: ran several commands and checked syscall counts with a test
 *           program.
 *  Module(s): server/uxkern/fsvr_subr.c	1.20
 * 			 syscall_subr.c		1.15
 *
 *  Added code to increment the sccount[] array for system calls in the rest
 *  of the start system call functions.  This fixes the count values for all
 *  system calls.  Also, bumped the MAXSYSCALLS value from 300 to 400 since
 *  there are currently 365 system calls.
 *
 * Revision 1.15  1994/11/18  20:49:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/11/03  16:13:52  yazz
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: med
 *  Benefit or PTS #: 11459
 *  Testing: corefile EAT
 *  Module(s):
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/tnc/dvp_pvpops.c
 * 	server/tnc/rtask_cli_pproc.c
 * 	server/uxkern/syscall_subr.c
 * Abolish use of the short lived SBLOCKEMUL proc flag bit.
 *
 * Revision 1.13  1994/10/25  23:22:10  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: High -- many lines changed in many files
 *  Benefit or PTS #: 9853
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/bsd/init_main.c
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_fork.c
 * 	server/bsd/kern_prot.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/sys/user.h
 * 	server/tnc/chkpnt_pproc.c
 * 	server/tnc/rvp_subr.c
 * 	server/tnc/tnc_svipc.c
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/syscall_subr.c
 * Side-thread changes.  Add new routine to request a side thread.  Change
 * uarea & register routines to handle side thread cases, including more
 * consistent setup of a thread's master lock hold counter at thread startup
 * time, and more assertions that this counter has returned to zero at thread
 * teardown time.  Make the wakeup of &p->p_flag that every thread has to
 * perform at thread deregister time, take place right in the deregister
 * routine itself.  Before returning to the emulator, respect the proc's new
 * SBLOCKEMUL bit, and if it's set return MIG_NO_REPLY to keep the emulator
 * from running (since proc is exiting).
 *
 * Revision 1.12  1994/10/21  16:54:34  nandy
 * Modification to the last change. If an issig_psig thread is already present
 * and the process is traced, don't call issig()/psig().
 *
 *  Reviewer: OSF
 *  Risk: L
 *  Benefit or PTS #: 9678
 *  Testing: os_interface eats
 *  Module(s): uxkern/syscall_subr.c
 *
 * Revision 1.11  1994/09/23  13:50:32  nandy
 * In end_server_op(), don't call issig()/psig() if the pending
 * signal is SIGTRAP.
 *
 *  Reviewer: John Loverso
 *  Risk: L
 *  Benefit or PTS #: 11020
 *  Testing: IPD on 400 nodes
 *  Module(s): emulator/i860/emul_machdep.c
 * 	    server.uxkern/syscall_subr.c
 *
 * Revision 1.10  1994/04/28  19:18:03  chrisp
 * Changes for introduction of tncgen and support of i386 builds.
 *
 *  Reviewer: dleslie, cfj
 *  Risk: M
 *  Benefit or PTS #: 9188
 *  Testing: Builds and tested on i386 platform.
 *  Module(s): server_init.c syscall_subr.c ux_server_loop.c
 *
 * Revision 1.9  1993/10/21  23:37:46  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.8  1993/07/14  18:44:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:06:24  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  19:33:24  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.6  1993/04/03  03:12:37  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/11  16:17:40  cfj
 * Merged in final version of the new forkmulti() from Locus but kept the
 * workaround.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:08:38  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/26  18:11:48  cfj
 * Force the interrupt in end_vprocserver_op() to be false.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:05:50  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:06:47  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:56:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:51  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.34  93/09/17  10:48:53  chrisp
 * [SPE 0030] Generic Spanning Trees: Include dpvproc_struct.h rather than
 * 	dpvproc.h -- so that make clean works.
 * 
 * Revision 2.33  93/06/16  15:28:42  klh
 * 	Revision 2.25  93/05/27  10:30:42  bolinger
 * 		Add initialization for new lock protecting task's vm_mmap region list.
 * 
 * Revision 2.32  93/06/02  12:38:23  yazz
 * For Sys V IPC under TNC add message 1007 for the bsd_get_my_svipc_port
 * RPC.
 * 
 * Revision 2.31  93/04/29  14:07:08  klh
 * 	Revision 2.24  93/03/05  17:46:44  loverso
 * 		Redo previous change by providing an explicit argument to
 * 		end_vprocserver_op.  (This is not needed in end_server_op, as nothing
 * 		currently would use it) 
 * 
 * 	Revision 2.23  93/02/04  11:49:13  durriya
 * 		Allow the signal checks in end_server_op and end_vprocserver_op
 * 		to be overridden.
 * 
 * 	Revision 2.22  93/01/06  10:37:14  loverso
 * 		Fix notice.
 * 
 * Revision 2.30  93/03/01  11:32:02  roman
 * [SPE #18] Ensure that fork() always creates a task. rforkmulti() will fill
 * 	in a different value in the uarea field if necessary.
 * 
 * Revision 2.29  93/02/24  15:49:10  slively
 * Verify interrupt pointer is non-null before dereferencing it.
 * 
 * Revision 2.28  93/01/26  14:40:40  yazz
 * Added bug number to previous RCS comment.
 * 
 * Revision 2.27  93/01/26  12:26:12  yazz
 * For bug #0156:
 * Allow callers of end_vprocserver_op() to skip the signal processing by
 * passing a NULL value for boolean pointer 'interrupt'.
 * 
 * Revision 2.26  92/11/12  14:14:57  chrisp
 * For TNC, ensure that proc and vproc port sequence counts are updated
 * 	in error return paths.
 * 
 * Revision 2.25  92/10/06  12:23:49  roman
 * Fix RCS comment.
 * 
 * Revision 2.24  92/10/05  13:49:23  klh
 * 	Revision 2.20  92/09/24  16:51:33  rabii
 * 		Added fake system call number for bsd_psignal().  (dwm; #376)
 * 
 * Revision 2.23  92/08/11  11:37:58  chrisp
 * [Bug #46] Add #ifdef TNC around VPROC_HOLD in start_vprocserver_op()
 * 
 * Revision 2.22  92/07/30  16:26:48  chrisp
 * Increment the reference count of the vproc for which a system call is
 * 	the target so that it will exist until the operation is complete
 * 	and the port sequence counting is performed.
 * 
 * Revision 2.21  92/07/10  09:15:50  chrisp
 * For TNC, call vproc_end_port_op() on completion of each proc port
 * 	operation so that message sequence counting may be performed.
 * 
 * Revision 2.20  92/06/05  14:01:04  klh
 * 	Revision 2.18  92/05/31  18:59:57  loverso
 * 		Define fake "syscalls" 1004, 1005 (loverso).
 * 
 * 		Remove duplicated include of parallel.h (pjg).
 * 
 * 	Revision 2.17  92/05/24  14:00:33  pjg
 * 		Set uu_procp in uarea_init rather than server_thread_register, 
 *		and reset it in server_thread_deregister.
 * 		Grenoble's v4.0 will fix this.
 * 
 * 		The corresponding operations for the file server still set and
 * 		reset uu_procp in fsvr_{get,release}_proc rather than
 * 		fsvr_uarea_{init,terminate} because they don't support 
 *		issig_psig and fsvr_thread_inititalize needs uu_procp to be 
 *		set before calling fsvr_uarea_init (pjg).
 * 
 * 		Revision 3.16  92/03/07  18:00:22  sp
 * 		Count System calls for table(TBL_INTR) (bug #90)
 * 
 * 		Propagated the changes described below to 
 *		{start,end}_server_op (pjg).
 * 		Revision 3.12  92/03/24  21:04:22  barbou
 * 		Fix for bug #113: use cprintf() instead of printf() for 
 *		syscall trace.
 * 		Renamed TRACE() to SC_TRACE() to avoid conflict with 
 *		sys/trace.h.
 * 
 * Revision 2.19  92/03/27  11:45:39  roman
 * Add check for vproc magic number in start_vprocserver_op().
 * 
 * Revision 2.18  92/03/24  10:21:06  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.15  92/03/20  11:33:11  pjg
 * 	Undo the last change. The creds port is now stored in the
 * 	{dummy} proc only (pjg).
 * 	Remove XXX. Rename: fsvr_op_{,de}register --> oip_{,de}register
 * 	(loverso).
 * 
 * Revision 2.14  92/03/15  14:33:32  roy
 * 	Set ni_credsport in the uth structure in uarea_init (durriya).
 * 
 * Revision 2.13  92/03/09  13:53:18  durriya
 * 	Revision 3.15  92/01/07  23:35:42  condict
 * 	Call Netintr at end of each system call, if softnet intr was scheduled.
 * 
 * 	Revision 3.14  91/12/20  17:57:42  barbou
 * 	Fix for unreported bug in the mapped file code (this code is normally
 * 	not used, but it has been activated once accidentally...).
 * 
 * Revision 2.12  92/03/01  18:34:23  pjg
 * 	Return MIG_REMOTE_ERROR instead of MIG_NO_REPLY in the situations
 * 	where no reply should be returned but the message should be destroyed
 * 	in ux_server_loop.
 * 
 * Revision 2.11  92/02/11  21:57:51  pjg
 * 	Revision 2.10.1.1  92/01/30  11:13:30  loverso
 * 	    Register non-interruptable calls with fsvr_op_register.
 * 
 * Revision 2.10  92/01/05  20:13:36  roy
 * 	91/12/28  16:24:56  roy
 * 	Set uu_procp in server_thread_register rather than uarea_init.
 * 	Move no-senders functionality to fsvr_subr.c
 * 
 * 	1991/10/14  21:04:26  noemi
 * 	Moved start_fileserver_op, end_fileserver_op, start_vnodeserver_op,
 * 	end_vnodeserver_op and fsproc_init to fsvr_subr.c.
 * 
 * Revision 2.9  91/12/18  20:19:35  roy
 *      Initialize p_stat flag to SRUN for dummy procs.
 *
 * Revision 2.8  91/12/18  15:44:52  roy
 *      91/12/13  13:04:21  sp
 *      Remove CMUCS and VICE conditionals and the remains of the
 *      non-functioning BSD restartable system call mechanism
 *
 *      91/11/15  16:27:55  bernadat
 *      Removed an extra simple_unlock() [barbou]
 *
 *      91/10/29  16:13:53  barbou
 *      Fix for bug #17: synchronize exit() and server_thread_register().
 *
 *      91/10/17  18:34:12  barbou
 *      Deleted include of "cmucs.h" (defunct option).
 *
 * Revision 2.7  91/12/13  10:23:47  roy
 *      91/11/12  10:21:17  roy
 *
 * Revision 2.6  91/11/22  15:20:40  rabii
 *      locus merge
 *      Call start_fileserver_op_nocred with pid and rcred pointer saved from
 *      select(). (chrisp)
 *
 *      Revision 2.10  91/11/14  14:54:18  chrisp
 *      Vproc pointer obtained using LOCATE_VPROC_PID rather than VPROCPTR.
 *      VPROC_RELEASEs added correspondingly. (chrisp)
 *
 *      Revision 2.9  91/11/11  18:17:03  klh
 *      reflect the correct include files for TNC distribution (klh)
 *
 *      Fix up some bogus comments. (roman)
 *
 *      Change the manipulation of dummy processes to put the process
 *      back on the free list after the server_thread_deregister call
 *      rather than before. (roman)
 *
 *      Change include of pvproc.h to bpvproc.h. (roman)
 *
 * Revision 2.5  91/10/14  13:24:42  sjs
 *      91/09/10  12:04:29  barbou
 *      Use uarea_terminate() to decrement reference count on the credentials.
 *
 * Revision 2.4  91/10/04  15:25:27  chrisp
 * Add routines start_fileserver_op_nocred() and end_fileserver_op_nocred()
 * for use by routine sbsd_sel_poll_reply(). Add extra credentials port
 * parameter to end_fileserver_op() and end_vnodeserver_op(). Pass
 * in credentials port to credentials_uarea_deregister().
 *
 * Revision 2.3  91/09/17  09:12:31  sjs
 * integrate Locus changes	roman
 * Add new routines start_fileserver_op(), start_vprocserver_op(),
 * start_vnodeserver_op() and the corresponding end_*server_op()s.
 * These routines are used rather than start_server_op() and
 * end_server_op() when the destination of an message from the
 * emulator to the server is a file port, vproc port or vnode port
 * rather than a proc port.  Fixed problem in last submission with
 * syscall tracing of syscalls > 1000. Removed code dealing with
 * mapped files.
 * 
 * Revision 2.2  91/08/31  14:28:14  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.8  91/08/09  12:24:15  barbou
 * Added missing comas to the extra_syscallnames array, and check for
 * negative syscode.
 * 
 * Revision 3.7  91/07/29  16:33:45  barbou
 * Print a more explicit message when tracing a syscode >= 1000.
 * 
 * Revision 3.6  91/06/25  17:14:11  condict
 * Moved sys/*.h files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.5  91/05/29  16:22:09  condict
 * Delete CMUCS_RFS conditional code.  Change default syscalltrace to "off".
 * 
 * Revision 3.4  91/05/07  15:57:11  condict
 * Initialize syscalltrace to -1 to trace sys calls from all processes.
 * 
 * Revision 3.3  91/04/12  14:24:17  condict
 * Add code for initializing namei credentials in uarea_init().
 * 
 * Revision 3.2  91/03/08  16:06:39  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/02/27  16:29:16  condict
 * Turn off VICE
 * 
 * Revision 3.0  91/01/17  12:06:18  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.9  90/11/05  16:59:05  rpd
 * 	Fix to work w/o MAP_UAREA
 * 	[90/11/01            rwd]
 * 
 * Revision 2.8  90/08/06  15:35:20  rwd
 * 	Added share_lock panic.
 * 	[90/07/05            rwd]
 * 
 * Revision 2.7  90/06/19  23:16:15  rpd
 * 	Initialize uu_proc_exit in uarea_init.
 * 	[90/06/05            rpd]
 * 
 * Revision 2.6  90/06/02  15:28:28  rpd
 * 	Improved sanity checking in server_thread_deregister.
 * 	Initialize new uu_xxx field in uarea_init.
 * 	[90/05/23            rpd]
 * 
 * 	Removed uu_arg.
 * 	[90/05/13            rpd]
 * 
 * 	Added sanity checks to server_thread_register/server_thread_deregister.
 * 	Modified end_server_op to handle a null uu_procp.
 * 	[90/05/10            rpd]
 * 	Revised for new reply msg technology.
 * 	[90/04/27            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  20:23:18  rpd]
 * 
 * Revision 2.5  90/05/29  20:25:21  rwd
 * 	Added sanity checks to server_thread_register/server_thread_deregister.
 * 	Modified end_server_op to handle a null uu_procp.
 * 	[90/05/10            rpd]
 * 	Added panic for attempt to exit while holding the master lock.
 * 	[90/05/09            rwd]
 * 	Added check for us_closefile on all syscalls.
 * 	[90/04/23            rwd]
 * 	Don't check for signals on all syscalls > 1000.  These are all
 * 	special case communication calls.
 * 	[90/03/30            rwd]
 * 
 * Revision 2.4  89/11/15  13:27:44  dbg
 * 	end_server_op must call fspause() with master lock held.
 * 	[89/10/30            dbg]
 * 
 * Revision 2.3  89/10/17  11:27:18  rwd
 * 	Better signal/exit checks.  Remove check for pre-set
 * 	u_procp.
 * 	[89/09/21            dbg]
 * 
 * 	Add interrupt return parameter to all calls.  Set it in
 * 	end_server_op.
 * 	[89/09/21            dbg]
 * 	Add interrupt return parameter to all calls.  Set it in
 * 	end_server_op.
 * 	[89/09/21            dbg]
 * 
 * Revision 2.2  89/08/31  16:29:21  rwd
 * 	Check for signals at the beginning of all syscalls.  If there is
 * 	one, return ERESTART.  Added sctrace().
 * 	[89/08/21            rwd]
 * 
 * $EndLog$
 */
/*
 * Server thread service routines.
 */
#include <uxkern/import_mach.h>

#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#ifdef  TNC
#include <tnc/dpvproc.h>
#else   /* !TNC */
#include <vproc/bpvproc.h>
#endif  /* !TNC */
#include <sys/signal_macros.h>

#include <uxkern/bsd_msg.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/syscall_subr.h>

#include <uxkern/syscalltrace.h>

#include <net/net_globals.h>	/* For NETISR_THREAD */
#include <net/netisr.h>		/* For netisr */

#ifdef	SYSCALLTRACE
extern int	nsysent;
int		syscalltrace = 0;
extern char	*syscallnames[];
extern void	save_call_chain();
#endif

#ifdef	OSF1_ADFS
#include <uxkern/sthread.h>
#endif

#define	MAXSYSCALLS	400		/* SEE conf/syscalls.master */
int	scmax = MAXSYSCALLS;		/* max number of system call counts */
int	sccount[MAXSYSCALLS];		/* system call counts */

/*
 * Register a server thread as serving a process.
 */
int
server_thread_register(
	register uthread_t	uth,
	register struct proc	*p)
{
	ASSERT(uth->uu_procp == 0);

	simple_lock(&p->p_lock);

	if ((p->p_flag&SWEXIT) || (p->p_stat == 0)) {
		simple_unlock(&p->p_lock);
		return -1;
	}

	++p->p_ref_count;		/* counts queue members */
	queue_enter(&p->p_servers, uth, uthread_t, uu_server_list);
	if (uth->uu_side_flag)
		++p->p_side_ref_count;	/* bump count of side threads */

	simple_unlock(&p->p_lock);
	return 0;
}

/*
 * Unregister a server thread.
 */
void
server_thread_deregister(
	register uthread_t	uth,
	register struct proc	*p)
{
	ASSERT(uth->uu_procp == p);

	unix_master();
	simple_lock(&p->p_lock);

	if (p->p_flag & SWEXIT)
		wakeup((caddr_t)&p->p_flag);

	if ((--p->p_ref_count <= 0) || queue_empty(&p->p_servers))
		panic("server_thread_deregister");

	if (uth->uu_side_flag) {		/* this was a side thread */
		--p->p_side_ref_count;
		uth->uu_side_flag = 0;
	}
	queue_remove(&p->p_servers, uth, uthread_t, uu_server_list);

	simple_unlock(&p->p_lock);
	unix_release();

	uth->uu_procp = 0;
}


/*
 * Find a server thread currently serving a process.
 */
uthread_t
server_thread_find(
	register struct proc	*p)
{
	register uthread_t	uth;

	simple_lock(&p->p_lock);

	if (queue_empty(&p->p_servers))
		uth = 0;
	else
		uth = (uthread_t)queue_first(&p->p_servers);
	simple_unlock(&p->p_lock);
	return (uth);
}

/*
 * Set up per-thread U area for the current thread.
 */
uarea_init1(uth, p, initmasterflag)
	register struct uthread *uth;
	struct proc *p;
	int initmasterflag;	/* initialize master lock or leave alone? */
{
	ASSERT(uth->uu_procp == 0);
	uth->uu_procp = p;
	uth->uu_proc_exit = FALSE;

	if (initmasterflag)
		uth->uu_master_lock = 0;

#if     MAP_UAREA
	uth->uu_share_lock_count = 0;
#endif  MAP_UAREA

	uth->uu_reply_msg = 0;

	uth->uu_nd.ni_iov = &uth->uu_nd.ni_iovec;
	/*
	 * The nameidata structure in the thread must point into the user
	 * task structure so that current and root directories
	 * can be shared among all threads in a task.
	 * If we want to change this, much file system code must be changed
	 * to know where these entities are stored.  Currently, it assumes
	 * that it's in the nameidata struct.
	 */
	uth->uu_nd.ni_utnd = &uth->uu_procp->p_utask.uu_utnd;
	/*
	 * Initialize per-thread credentials.  This for two types of
	 * clients -- server threads and fork().
	 */
	uth->uu_nd.ni_cred = NOCRED;
	cr_threadinit(uth);

#ifdef	TNC
	uth->uu_fork_task = MACH_PORT_NULL;
#endif
}
uarea_init(uth, p)
	register struct uthread	*uth;
	struct proc		*p;
{
	uarea_init1(uth, p, TRUE);
}
uarea_init_side(uth, p)
	register struct uthread	*uth;
	struct proc		*p;
{
	uarea_init1(uth, p, FALSE);
}

/*
 * Clean out the per thread uarea after use.
 */
uarea_terminate(uth)
	register struct uthread *uth;
{
	if (uth->uu_nd.ni_cred != NOCRED) {
		crfree(uth->uu_nd.ni_cred);
		uth->uu_nd.ni_cred = NOCRED;
	}
}

uarea_lock_init(utaskp)
	struct utask *utaskp;
{
#if	UNIX_LOCKS && MACH_SLOCKS
	/*
	 * The accounting flag should simply use the u-area handy lock.
	 */
	utaskp->uu_acflag.fi_lock = &utaskp->uu_handy_lock;
#endif
	U_HANDY_LOCK_INIT(utaskp);
	U_TIMER_LOCK_INIT(utaskp);
	UTND_LOCK_INIT(&utaskp->uu_utnd);
	U_MMAP_LOCK_INIT(utaskp);
	/*
	 * Init proc timer lock here rather than hack vm/vmunix_.c:procdup().
	 */
	PROC_TIMER_LOCK_INIT(utaskp->uu_procp);
}


/*
 * Start UX server call: map port to process, set up
 * U area.
 */
int
start_server_op(port, syscode)
	mach_port_t	port;
	int		syscode;
{
	register struct proc	*p;
	register struct uthread	*uth = &u;
	int error = 0;

	if ((p = port_to_proc_lookup(port)) == (struct proc *)0)
		return (ESRCH);

	if (p->p_cursig && syscode < 1000) {
#ifdef	TNC
		vproc_end_port_op(p->p_vproc, "start_server_op");
#endif	/* TNC */
		return (ERESTART);
	}

	/*
	 * If this happens, destroy the message
	 */
	if (server_thread_register(uth, p) != 0) {
#ifdef	TNC
		vproc_end_port_op(p->p_vproc, "start_server_op");
#endif	/* TNC */
		return (MIG_REMOTE_ERROR);
	}

	uth->uu_syscode = syscode;
	uarea_init(uth, p);

	/* Initialize the server_oip to "do nothing" */
	oip_register(uth, MACH_PORT_NULL, 0);

#ifdef	SYSCALLTRACE
	if (syscalltrace && (syscalltrace == p->p_pid || syscalltrace < 0)) {
		char *s;
		char num[10];
		static char *extra_syscallnames[] = {
			"(take_signal)",		/* 1000 */
			"(task_by_pid)",		/* 1001 */
			"(init_process)",		/* 1002 */
			"(exec_args_set)",		/* 1003 */
			"(proc_exit)",			/* 1004 */
			"(setports)",			/* 1005 */
			"(bsd_psignal)",		/* 1006 */
			"(bsd_get_my_svipc_port)"	/* 1007 */
		};
		static int extra_nsysent = sizeof(extra_syscallnames) / 
						sizeof(extra_syscallnames[0]);

		if (syscode >= nsysent || syscode < 0) {
			if (syscode - 1000 >= extra_nsysent || syscode < 0) {
				sprintf(num, "%d", syscode);
				s = num;
			} else {
				s = extra_syscallnames[syscode - 1000];
			}
		}
		else {
			s = syscallnames[syscode];
		}
		SC_TRACE(("[%d]%s", p->p_pid, s));
	}
#endif	/* SYSCALLTRACE */

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */

	return(error);
}

int
end_server_op(error, interrupt)
	register int	error;
	boolean_t	*interrupt;
{
	register struct uthread *uth = &u;
	register struct proc	*p = uth->uu_procp;

#if     !NETISR_THREAD
        /*
         * Simulate software interrupts for network.
         */
        if (netisr)
                Netintr();
#endif

	if (p == 0) {

#if	MACH_ASSERT
	    if (uth->uu_master_lock)
		panic("end_server_op1: Master still held (0x%x)",
				uth->uu_master_lock);
#if	MAP_UAREA
	    if (uth->uu_share_lock_count) {
		panic("end_server_op1: Share lock still held (0x%x)",
				uth->uu_share_lock_count);
		uth->uu_share_lock_count = 0;
	    }
#endif	MAP_UAREA
#endif	MACH_ASSERT

	    uarea_terminate(uth);
	    return (MIG_NO_REPLY);
	}

	*interrupt = FALSE;
	if (!EXITING(p)) {
	  if((p->p_issig_flag != 0 ) && !(p->p_flag & STRC)) {
	    if (p->p_cursig != 0 || HAVE_SIGNALS(p)) {
		unix_master();
		if (p->p_cursig != 0 || issig())
		    psig();
		if (p->p_cursig)	/* user should take signal */
		    *interrupt = TRUE;
		unix_release();

		/* psig might have killed us, and completed exit processing */
		if (uth->uu_procp == 0) {

#if	MACH_ASSERT
		    if (uth->uu_master_lock)
			panic("end_server_op2: Master still held (0x%x)",
					uth->uu_master_lock);
#if	MAP_UAREA
		    if (uth->uu_share_lock_count) {
			 panic("end_server_op2: Share lock still held (0x%x)",
					uth->uu_share_lock_count);
			uth->uu_share_lock_count = 0;
		    }
#endif	MAP_UAREA
#endif	MACH_ASSERT

		    uarea_terminate(uth);

#ifdef	TNC
		    vproc_end_port_op(p->p_vproc, "end_server_op:MIG_NO_REPLY");
#endif	TNC
		    return (MIG_NO_REPLY);
		}
	    }
	  }
	}

#ifdef	SYSCALLTRACE
	SC_TRACE(("    [%d] returns %d%s\n", p->p_pid, error,
		  (*interrupt) ? " Interrupt" : ""));
#endif	/* SYSCALLTRACE */

#if	MACH_ASSERT
	if (uth->uu_master_lock)
	    panic("end_server_op3: Master still held (0x%x)",
			uth->uu_master_lock);

#if	MAP_UAREA
	if (uth->uu_share_lock_count) {
	    panic("end_server_op3: Share lock still held (0x%x)",
			uth->uu_share_lock_count);
	    uth->uu_share_lock_count = 0;
	}
#endif	MAP_UAREA
#endif	MACH_ASSERT

	oip_deregister(uth);

	uarea_terminate(uth);
	server_thread_deregister(uth, p);

#ifdef	TNC
	vproc_end_port_op(p->p_vproc, "end_server_op");
#endif	/* TNC */
	return (error);
}


/*
 * Start vproc server call: map port to vproc and process, set up
 * U area.
 */
int
start_vprocserver_op(port, syscode)
	mach_port_t	port;
	int		syscode;
{
	register struct vproc	*v;
	register struct proc	*p;
	register struct uthread	*uth = &u;
	int error = 0;

	if ((v = port_to_vproc_lookup(port)) == (struct vproc *)0)
		return (ESRCH);
	if (v->vp_magic != VP_MAGIC)
		return (EBADPORT);
	p = PVP(v)->pvp_pproc;

	if (p->p_cursig && syscode < 1000) {
#ifdef	TNC
		vproc_end_vp_port_op(v, "start_vprocserver_op");
#endif	/* TNC */
		return (ERESTART);
	}

	/*
	 * If this happens, destroy the message
	 */
	if (server_thread_register(uth, p) != 0) {
#ifdef	TNC
		vproc_end_vp_port_op(v, "start_vprocserver_op");
#endif	/* TNC */
		return (MIG_REMOTE_ERROR);
	}

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */

	uarea_init(uth, p);

	/* Initialize the server_oip to "do nothing" */
	oip_register(uth, MACH_PORT_NULL, 0);
	SC_TRACE(("[%d]%s", p->p_pid, syscallnames[syscode]));

#ifdef	TNC
	/* Make sure the vproc stays around for the duration */
	VPROC_HOLD(v, "start_vprocserver_op");
#endif	/* TNC */

	return(error);
}

int
end_vprocserver_op(port, error, interrupt, nosigchk)
	mach_port_t	port;
	register int	error;
	boolean_t	*interrupt;
	boolean_t	nosigchk;
{
	register struct vproc	*v = port_to_vproc_lookup(port);
	register struct uthread *uth = &u;
	register struct proc	*p = uth->uu_procp;

	if (p == 0) {
	    uarea_terminate(uth);
	    if (uth->uu_master_lock)
		panic("end_vprocserver_op: Master still held");

#if	MAP_UAREA
	    if (uth->uu_share_lock_count) {
		panic("end_vprocserver_op: Share lock still held");
		uth->uu_share_lock_count = 0;
	    }
#endif	MAP_UAREA

#ifdef	TNC
	    vproc_end_vp_port_op(v, "end_vprocserver_op");
	    VPROC_RELEASE(v, "end_vprocserver_op");
#endif	/* TNC */
	    return (MIG_NO_REPLY);
	}

	if (nosigchk) {
		*interrupt = p->p_cursig || HAVE_SIGNALS(p);
		goto skipsig;
	}

	*interrupt = FALSE;
	if (!EXITING(p)) {
	    if (p->p_cursig != 0 || HAVE_SIGNALS(p)) {
		unix_master();
		if (p->p_cursig != 0 || issig())
		    psig();
		if (p->p_cursig)	/* user should take signal */
		    *interrupt = TRUE;
		unix_release();

		/* psig might have killed us, and completed exit processing */
		if (uth->uu_procp == 0) {
		    uarea_terminate(uth);
		    if (uth->uu_master_lock)
			panic("end_vprocserver_op: Master still held");

#if	MAP_UAREA
		    if (uth->uu_share_lock_count) {
			 panic("end_vprocserver_op: Share lock still held");
			uth->uu_share_lock_count = 0;
		    }
#endif	MAP_UAREA


#ifdef	TNC
		    vproc_end_vp_port_op(v, "end_vprocserver_op");
		    VPROC_RELEASE(v, "end_vprocserver_op");
#endif	/* TNC */
		    return (MIG_NO_REPLY);
		}
	    }
	}


skipsig:
	SC_TRACE(("    [%d] returns %d%s", p->p_pid, error,
		  (interrupt && *interrupt) ? " Interrupt" : ""));

	oip_deregister(uth);

        uarea_terminate(uth);
	server_thread_deregister(uth, p);

	if (uth->uu_master_lock)
	    panic("end_vprocserver_op: Master still held");

#if	MAP_UAREA
	if (uth->uu_share_lock_count) {
	    panic("end_vprocserver_op: Share lock still held");
	    uth->uu_share_lock_count = 0;
	}
#endif	MAP_UAREA

#ifdef	TNC
	vproc_end_vp_port_op(v, "end_vprocserver_op");
	VPROC_RELEASE(v, "end_vprocserver_op");
#endif	/* TNC */
	return (error);
}

/*
 * request_side_thread - do client-side setup for creation of a
 * proc side thread.
 */
int
request_side_thread(
	struct proc	*p,
	int		(*side_func)(),
	int		intarg)
{
	int		error;

	ASSERT(p != NULL);

	/*
	 * Make sure the proc stays is still around for the startup
	 * of the side thread.  The server side (always on our own
	 * node) is responsible for decrmenting this hold counter
	 * and performing a wakeup if needed (i.e. if someone is
	 * waiting on it, and the decrement reduces it to zero).
	 * However, if the thread is already exiting, don't start
	 * the side thread at all.
	 */
	simple_lock(&p->p_lock);
	if (p->p_flag&SWEXIT) {
		simple_unlock(&p->p_lock);
		return(-1);
	}
	p->p_exit_hold_count++;
	simple_unlock(&p->p_lock);

	(void)ubsd_side_thread_server(
			proc_to_port_lookup(p),
			side_func, 
			intarg);
	return(0);
}

/*
 * vm_allocate_strict() and vm_deallocate_strict() panic on any kind of
 * failure.  Other than this, they are identical to the corresponding
 * Mach calls.
 */
void
vm_allocate_strict(
	mach_port_t	target_task,
	vm_address_t	*addressp,
	vm_size_t	size,
	boolean_t	anywhere)
{
	kern_return_t	kr;

	kr = vm_allocate(target_task, addressp, size, anywhere);
	if (kr != KERN_SUCCESS) {
		panic("vm_allocate_strict(): kr=0x%x addressP=0x%x "
				"size=0x%x anywhereflag=%d\n",
				kr, addressp, size, anywhere);
	}
	return;
}

void
vm_deallocate_strict(
	mach_port_t	target_task,
	vm_address_t	address,
	vm_size_t	size)
{
	kern_return_t	kr;

	kr = vm_deallocate(target_task, address, size);
	if (kr != KERN_SUCCESS) {
		panic("vm_deallocate_strict(): kr=0x%x address=0x%x "
				"size=0x%x\n",
				kr, address, size);
	}
	return;
}

