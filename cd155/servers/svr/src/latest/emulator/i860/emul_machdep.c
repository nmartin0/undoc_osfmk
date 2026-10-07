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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: emul_machdep.c,v $
 * Revision 1.44  1995/04/04  18:05:58  tnt
 * PTS #: 12348
 * Mandatory?:  Tensor Mandatory (H0).
 * Description: When running ipd the SIGTRAP signal used to stop the process
 *              under debug was interferring with the operation of the
 *              asynchronous pfs system calls.  The pfs system calls detect
 *              the interruption due to the receipt of the signal, and return
 *              the EINTR errno.  The solution is to override the error returned
 *              if, after querying the server, we find that no signals are
 *              pending for the process.  A status indicating that the operation
 *              completed normally is then returned.
 * Reviewer(s): Bob Godley, John Litvin
 * Risk:        Medium
 * Testing:     gprof, prof, VSX, and ipd EATs.  Parallel SATs.
 * Module(s):   Server/svr/src/svr/emulator/async_io.c
 *              Server/svr/src/svr/emulator/i860/emul_machdep.c
 *
 * If the error status is EINTR and take_signals() returns that no signals
 * were pending for the process, treat the situation as if no signal had
 * occurred.
 *
 * Revision 1.43  1995/03/02  02:03:23  stans
 *  Lint picking
 *
 *  Reviewer:lenb,suri
 *  Risk:low
 *  Benefit or PTS #:8129
 *  Testing:WW07 sats
 *
 * Revision 1.42  1995/02/01  21:24:36  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.41  1994/11/18  20:25:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.40  1994/09/23  13:54:43  nandy
 * In take_signals(), if bsd_take_signal() return with an error, call syscall_suspend_barrier() again.
 *
 *  Reviewer: John Loverso
 *  Risk: L
 *  Benefit or PTS #: 11020
 *  Testing: IPD on 400 nodes
 *  Module(s): uxkern/syscall_subr.c
 * 	    emulator/i860/emul_machdep.c
 *
 * Revision 1.39  1994/06/24  00:27:40  yazz
 *  Reviewer: Chris Peak
 *  Risk: very lo
 *  Benefit or PTS #: #9952
 *  Testing: none
 *  Module(s): emulator/i386/emul_machdep.c
 *             emulator/i860/emul_machdep.c
 *
 * Call emul_panic() instead of pretending that divide by zero will halt the
 * emulator on all architectures.
 *
 * Revision 1.38  1994/06/01  20:33:22  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): server/sys/errno.h, server/sys/version.h server/nx/nx.c
 * 	    server/nx/nx.defs server/nx/nx_svr.defs server/nx/nx_types.defs,
 * 	    server/nx/nx_types.h, server/nx/nx_create_attr.c (new)
 * 	    emulator/i860/emul_machdep.c
 *  Related: libnx, allocator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.37  1994/05/27  23:43:24  cfj
 * Check if nx_initve() has been previously called, and if so return
 * EAEXIST.
 *
 *  Reviewer:suri
 *  Risk:L
 *  Benefit or PTS #:2974 & 9345
 *  Testing:Test cases and rmcall EAT
 *  Module(s):emulator/i860/emul_machdep.c
 *
 * Revision 1.36  1994/03/14  01:45:36  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, Checkpoint Restart specific, EATS
 *  Module(s):
 *
 * Revision 1.35  1994/01/27  22:29:24  brad
 * Merge of revision 1.34.2.1 from the R1.2 branch.
 *
 * Revision 1.34.2.1  1994/01/27  01:41:04  brad
 * Added a workaround for PTS #7082 ... when the -plk NX switch is used,
 * vm_copy() does not preserve the 'wired' state of the VM pages in the
 * user's buffer.  Workaround by not using vm_copy() when -plk used,
 * resulting in possible PFS performance degradation at higher bandwidths.
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 7082
 *  Testing: Verified workaround with test from PTS report, ran PFS EATs
 *     on 64 nodes, ran PFS SAT on 64 nodes.
 *  Module(s): emulator/{pfs2_user_side.c,pfs_msgutil.c,i860/emul_machdep.c}
 *
 * Revision 1.34  1993/12/01  00:06:44  carbajal
 * Added nx_pspart_rpc() and nx_alloc_exit() calls.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.33  1993/11/17  02:32:36  carbajal
 * R1.2 User Model support
 *
 *  Reviewer: cfj
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.32  1993/11/16  15:29:28  cfj
 * Ifdef out the force_migrate() code which was experimental and
 * never ment to be used.
 *
 *  Reviewer:nandy,jlitvin
 *  Risk:M
 *  Benefit or PTS #:7151
 *  Testing:Make sure IPD still functions
 *  Module(s):server/nx/nx.c
 * 	   server/conf/syscall.master
 * 	   emulator/i860/emul_machdep.c
 *
 * Revision 1.31  1993/10/27  01:29:52  carbajal
 * Changed parameters to e__nx_init()
 * Added e_nx_app_node_list()
 *
 * Revision 1.30  1993/10/04  18:51:34  cfj
 * Have nx_init() check the magic number passed from the library and return
 * EINCOMPAT if the NX application is incompatible with the OS.
 *
 * Revision 1.29  1993/10/01  11:10:50  stefan
 * Modified the former fix for bug #6366 that did not cover nx_join():
 * introduced a new variable sll_force_local that is used instead of
 * nx_application in e_fork_call() to determine if the process should be
 * forced on the same node. sll_force_local is set to TRUE whenever the
 * node mapping of a process changes (e.g. nx_initve(), nx_join()).
 * Also put #ifdef NX around definition of nx_application again.
 *
 * Revision 1.28  1993/09/27  14:22:56  cfj
 * Reset tnc_mynode to -1 after a sucessful nx_join to force the emulator
 * to update its node number information.
 *
 * Revision 1.27  1993/09/21  17:17:51  cfj
 * Removed <<< lines from cvs merge conflict.
 *
 * Revision 1.26  1993/09/17  17:54:12  cfj
 * Merge R1.1 bug fix.
 *
 * Revision 1.25  1993/09/14  23:53:20  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.24.2.2  1993/09/17  17:50:18  cfj
 * Fix for PTS bug #6674.  Remove the #ifdef NX around the declaration
 * of nx_application so that the ramdisk emulator now builds.
 *
 * Revision 1.24.2.1  1993/09/14  23:51:30  cfj
 * Fix for PTS bug #6366.  Modified e_fork() so that when it calls
 * e_fork_call(), it uses the nx_application boolean to determine if the
 * process is forced to the local node.  NX applications always force the
 * process to the local node.
 *
 * Revision 1.24  1993/08/28  00:51:17  carbajal
 * Added e_nx_register_daemon() for PTS #6141
 *
 * Revision 1.23  1993/08/11  18:36:57  stefan
 * Fixed a bug in SLL that could make rfork(localnode) actually
 * go to a remote node.
 *
 * Revision 1.22  1993/07/18  19:42:28  carbajal
 * Added nx_chpart_rpc call to server
 *
 * Revision 1.21  1993/07/14  21:16:01  carbajal
 * Added support for allocator system calls
 *
 * Revision 1.20  1993/07/14  17:32:48  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.8  1993/07/01  18:25:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.19  1993/05/19  17:32:00  cfj
 * The real fix so that the emulator does not loose a E_SIGRETURN status
 * in the case where take_signals() returns zero.
 *
 * Revision 1.18  1993/05/19  16:36:24  cfj
 * Remove the take_signals() fix which tried to preserve E_SIGRETURN
 * properly.
 *
 * Revision 1.17  1993/05/17  19:05:35  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.16  1993/05/06  18:58:48  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.6  1993/05/03  17:19:02  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.15  1993/04/13  16:08:07  cfj
 * Merge with T9.5.
 *
 * Revision 1.10.4.3  1993/04/12  22:33:12  cfj
 * Do full context restore on sigreturn.
 *
 * Revision 1.14  1993/04/12  16:51:53  cfj
 * Added join_root_part() syscall.
 *
 * Revision 1.13  1993/04/05  15:11:41  cfj
 * Merge with T9.
 *
 * Revision 1.10.4.2  1993/04/04  16:53:04  cfj
 * Add magic number parameter to _nx_init().
 *
 * Revision 1.12  1993/04/03  03:19:06  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.11  1993/02/24  18:50:46  nandy
 * Changes brought in from T9
 *
 * Revision 1.10.4.1  1993/02/22  17:45:36  nandy
 * nx_init() now takes an extra argument.
 *
 * Revision 1.1.2.3.2.3  1993/02/16  20:01:50  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.10  1993/01/21  19:06:52  nandy
 * 1/20/93 drop from locus.
 *   In take_signal(), take note whether the server has returned an error
 *   	from bsd_take_signal() and return immediately if so.
 *
 * Revision 1.9  1993/01/14  23:54:03  nandy
 * Added emul_blocking/emul_unblocking around the RPC to the server for 
 * nx_ syscalls.
 *
 * Revision 1.1.2.3.2.2  1993/01/09  00:02:55  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.8  1993/01/05  19:57:56  shala
 * Added stub for a new syscall nx_pri().
 *
 * Revision 1.1.2.3.2.1  1992/12/16  05:57:42  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.7  1992/12/15  00:53:16  cfj
 * 	Revision 2.25  92/12/14  14:43:18  chrisp
 * 	In take_signal(), take note whether the server has returned an error
 * 	        from bsd_take_signal() and return immediately if so.
 *
 * Revision 1.6  1992/12/14  18:38:03  nandy
 * struct emul_stack now has a field for the user fp and user return addr.
 *
 * nx_initve sets tnc_mynode to -1 to force NX_node_self to be called.
 *
 * Revision 1.4  1992/11/30  22:09:56  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/09  17:53:24  cfj
 * Conflict resolution of 11/05/92 bug fix drop from Locus.
 *
 * Revision 1.1.2.2  1992/11/06  18:21:18  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/05  22:30:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.23  1992/11/02  21:51:45  cfj
 * Final integration and testing of IPD modifications
 *
 * Revision 2.28  93/06/16  15:25:15  klh
 * 	Revision 2.24  93/06/03  17:34:56  rabii
 * 		Added non TNC rfork, also e_fork_call now take a node argument (rabii)
 * 
 * 	Revision 2.23  93/06/02  17:28:27  rabii
 * 		Add in fix for E_SIGRETURN handling from intel (rabii,cfj@ssd.intel.com)
 * 
 * Revision 2.27  93/05/05  20:58:39  yazz
 * I860 assembly language changes from Intel.
 * 
 * Revision 2.26  93/04/24  18:46:54  klh
 * 	Revision 2.22  93/02/02  15:21:44  rabii
 * 		Change the test for error in take_signals from 0 to ESUCCESS
 *
 * Revision 2.25  92/12/14  14:43:18  chrisp
 * In take_signal(), take note whether the server has returned an error
 *         from bsd_take_signal() and return immediately if so.
 * 
 * Revision 2.24  92/11/23  16:05:27  klh
 * 	Revision 2.20  92/11/17  19:46:51  loverso
 * 		Test global interrupt flag for non-handled forwarded signals
 * 		(bug 459).
 * 
 * 	Revision 2.19  92/11/11  10:42:25  rabii
 * 		[1992/10/19  21:27:35  stans]
 * 		Charlie's latest r29 vs fir & restartable syscalls fixes.
 * 
 * 	Revision 2.17  92/11/03  11:03:10  loverso
 * 		bsd_sigreturn was not saying it was emul_blocked.
 * 
 * Revision 2.23  92/11/05  10:37:57  klh
 * R29 bug fixes received from Intel
 * 
 * Revision 2.22  92/10/28  14:47:46  roman
 * Fix some types for a cleaner compilation.
 * 
 * Revision 2.21  92/10/08  11:27:02  roman
 * Fix up types for clean compilation.
 * Get rid of routine rexecve_init() - now located in emul_init.c.
 * Add new routine e_rexecve_arrival(). Looks much like e_execve()
 * 	and is called via the standard emulator system call
 * 	interface. Necessary to get correct handling of
 * 	traced rexec-ed processes.
 * 
 * Revision 2.20  92/10/06  12:07:22  roman
 * Fix RCS comments.
 * 
 * Revision 2.19  92/10/05  13:47:18  klh
 * 	Revision 2.16  92/09/29  16:51:37  rabii
 * 		[92/09/08  09:42:15  cfj@ssd.intel.com]
 * 		Modified take_signals() so that it no longer trashes fir with 
 *		r29. sigreturn() now depends on sigcontext.fir to be set up 
 *		correctly.
 * 
 * 	Revision 2.15  92/09/24  16:49:02  rabii
 * 		If traced (STRC), SIGTRAP from exec _after_ requisite state 
 *		is set up. (dwm; #376)
 * 
 * 	Revision 2.14  92/08/26  12:09:53  loverso
 * 		Add syscall-end barrier at end of emul_syscall.
 * 		Add emul_thread_id_self(), emul_thread_id_of(), and
 * 		emul_thread_get_state().
 * 		(loverso)
 * 
 * Revision 2.18  92/10/01  10:17:39  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 2.17  92/07/28  12:53:37  chrisp
 * Remove EPRINT notifying failure of default SIGMIGRATE handler.
 * 
 * Revision 2.16  92/07/07  13:06:56  klh
 * 	Revision 2.12  92/07/02  09:55:30  loverso
 * 		Add commented out printf debug stmts.
 * 		[92/07/01            roy]
 * 
 * Revision 2.15  92/06/10  12:19:46  klh
 * 	Revision 2.11  92/06/09  16:38:48  pjg
 * 		Revision 3.18  92/04/08  20:44:22  barbou
 * 		Fix for bug #127: handle trashed user stack when delivering 
 *		signals. (i860 version)
 * 
 * 	Revision 2.10  92/06/08  18:17:25  pjg
 * 		Added missing code to emul_syscall as per request from Intel 
 *		(rabii)
 * 		Use bcopy() to copy thread state info.
 * 		Include fsr in thread state (stans@intel)
 * 
 * Revision 2.14  92/04/14  09:59:16  roman
 * Change typo where e_rforkmulti() was typed in as e_rfork().
 * 
 * Revision 2.13  92/04/01  16:14:17  roman
 * Add support for machine-dependent portion of rforkmulti() system call.
 * Remove stub for rexecmulti() system call, which is no longer in the plan.
 * Minor formatting fixes.
 * 
 * Revision 2.12  92/03/27  17:31:30  roman
 * emul_tnc_mynode() is now a subroutine rather than a variable (used for
 * 	TNC calculation of the current node).
 * 
 * Revision 2.11  92/03/27  10:51:38  roman
 * Put in "ifdef TNC" around all the TNC code in this file.
 * 
 * Revision 2.10  92/03/18  11:58:02  roman
 * Add code to check for rexecve() to our own node, and short-circuit call
 * (successfully) in this case.
 * 
 * Revision 2.9  92/02/26  08:43:11  roman
 * Fix RCS comments.
 * 
 * Revision 2.8  92/02/26  08:13:22  roman
 * Integrate Intel changes.
 * 
 * Revision 2.7  92/02/11  18:43:24  pjg
 * 	Fixed emul_setup loop (rabii).
 * 
 * Revision 2.6  92/01/28  10:04:13  rabii
 * 	Fixed passing of env variables and removed 
 * 	unneeded error checking
 * 
 * Revision 2.5  92/01/17  17:14:46  roy
 * 	Added emul_thread_set_state (rabii).
 * 
 * Revision 2.4  92/01/16  17:43:15  roy
 * 	Wed Jan 15 12:21:13 cfj@ssd.intel.com
 * 	Moved the emulator stack pointer structures into sys/ushared.h
 * 	for the case where MAP_UAREA is true. 
 * 
 * Revision 2.3  92/01/09  15:08:06  roy
 * 	Minor fixes from Intel.
 * 
 * 
 * Revision 2.2  91/11/14  15:34:15  rabii
 * 	Initial checkin
 * 
 * Revision 2.7  91/10/16  11:24:27  stans
 * working mk63 & osf1ad V2.0 from grn
 * 
 * Revision 2.6  91/09/03  11:11:33  jsb
 * 	First checkin. Derived from mips sources by Intel SSD.
 * 	[91/09/03  08:32:41  jsb]
 * 
 * Revision 2.5  90/10/25  15:06:38  rwd
 * 	Initialize eflags on exec instead of letting it get a random
 * 	value off the stack.
 * 	[90/09/19            rwd]
 * 
 * Revision 2.4  90/08/06  15:30:39  rwd
 * 	Added pid_by_task.
 * 	[90/07/31            rwd]
 * 	Include sys/types.h
 * 	[90/07/17            rwd]
 * 
 * Revision 2.3  90/06/02  14:36:14  rpd
 * 	Converted to new IPC.
 * 	[90/06/02            rpd]
 * 
 * Revision 2.2  90/05/21  13:46:11  dbg
 * 	i386 version.
 * 	[90/04/23            dbg]
 * 
 * Revision 2.3  89/10/17  11:24:11  rwd
 * 	Added syscall init_process(-41).  Mach_Init needs this.
 * 	[89/09/29            rwd]
 * 
 * 	Removed reference to ERROR
 * 	[89/09/26            rwd]
 * 
 * Revision 2.2.1.1  89/09/21  20:35:37  dbg
 * 	Add interrupt return parameter to all calls.
 * 	[89/09/21            dbg]
 * 
 * Revision 2.2  89/08/31  16:28:19  rwd
 * 	No special case for syscall needed.  Fix up user stack whenever
 * 	ERESTART occurs.
 * 	[89/08/24            rwd]
 * 	Changed to reflect change in trampline code that pushes syscall
 * 	# on stack.
 * 	[89/08/23            rwd]
 * 
 * 	Make ERESTART code work and special case for syscall
 * 	[89/08/21            rwd]
 * 
 * Revision 2.1  89/08/04  14:04:49  rwd
 * Created.
 * 
 * 22-Jun-89  Randall Dean (rwd) at Carnegie-Mellon University
 *	Added copyright and history to file by dbg.  Fixed register
 *	format.
 *
 * $EndLog$
 */

/*
 * Take/Return from signals - machine-dependent.
 */
#include <mach/mach.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/version.h>
#include <mach/mig_errors.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <machine/psl.h>
#include <machine/vmparam.h>
#include <uxkern/bsd_msg.h>	/* error code definitions */
#include "emul.h"
#include "emul_stack.h"

#include <syscall_table.h>

#include <nx/nx_types.h>
#ifdef NX
#include <nx/nx_svr.h>
#endif /* NX */

#ifdef	MAP_UAREA
#include <sys/ushared.h>

extern int shared_enabled;
extern struct ushared_ro *shared_base_ro;
extern struct ushared_rw *shared_base_rw;
#else	MAP_UAREA

/*
 * User's registers are all stored on the user's stack.
 * The non-volatiles are saved (obviously), as well as
 * the parameters to the syscall (ie, r16-r27).
 *
 * The psr is saved so we can manipulate the CC bit and
 * clear any of the trap bits.
 *
 * Ordinarily, we wouldn't need to preserve r16-r27, but they contain
 * the parameters for the system call and the machine indep. code
 * would really like them spread out in memory.
 *
 * r28 follows i860 PRM convention for structure passing.
 *
 * r29 is magic in that it contains the address of the instruction
 * following the "trap" instruction -- we need to know where to return to.
 * The trap handler (i860/trap.c) saves the value of (fir + 4) in r29.
 * The original contents of the user's r29 are lost way back in the
 * trap handler, but it's defined to be volatile anyway.  See i860/trap.c
 * in the kernel.  (all because the i860 doesn't have a user-visible pc).
 *
 * r31 contains the syscall number.
 *
 * The non-volatile FP regs are saved as a precaution and might be
 * removed in the future during tuning.
 *
 * DON'T EVEN THINK ABOUT REARRANGING THIS WITHOUT CONSULTING
 * emul_vector.s and sys/signal.h !!
 */
struct emul_exception_frame {
	unsigned	onstack;	/* see sigcontext */
	unsigned	mask;
	unsigned	u_r0;		/* always 0 */
	unsigned	u_r1;		/* user's return address */
	unsigned	u_sp;		/* stack pointer */
	unsigned	u_fp;		/* frame pointer */
	unsigned	u_r4;		/* non-volatile */
	unsigned	u_r5;		/* non-volatile */
	unsigned	u_r6;		/* non-volatile */
	unsigned	u_r7;		/* non-volatile */
	unsigned	u_r8;		/* non-volatile */
	unsigned	u_r9;		/* non-volatile */
	unsigned	u_r10;		/* non-volatile */
	unsigned	u_r11;		/* non-volatile */
	unsigned	u_r12;		/* non-volatile */
	unsigned	u_r13;		/* non-volatile */
	unsigned	u_r14;		/* non-volatile */
	unsigned	u_r15;		/* non-volatile */
	unsigned	u_r16;		/* syscall param  1 */
	unsigned	u_r17;		/* syscall param  2 */
	unsigned	u_r18;		/* syscall param  3 */
	unsigned	u_r19;		/* syscall param  4 */
	unsigned	u_r20;		/* syscall param  5 */
	unsigned	u_r21;		/* syscall param  6 */
	unsigned	u_r22;		/* syscall param  7 */
	unsigned	u_r23;		/* syscall param  8 */
	unsigned	u_r24;		/* syscall param  9 */
	unsigned	u_r25;		/* syscall param 10 */
	unsigned	u_r26;		/* syscall param 11 */
	unsigned	u_r27;		/* syscall param 12 */
	unsigned	u_r28;		/* for structure passing or spills */
	unsigned	u_r29;		/* emulator will return thru here */
	unsigned	u_r30;		/* volatile */
	unsigned	u_r31;		/* system call number */
	unsigned	u_f0;		/* always 0 */
	unsigned	u_f1;		/* always 0 */
	unsigned	u_f2;		/* non-volatile */
	unsigned	u_f3;		/* non-volatile */
	unsigned	u_f4;		/* non-volatile */
	unsigned	u_f5;		/* non-volatile */
	unsigned	u_f6;		/* non-volatile */
	unsigned	u_f7;		/* non-volatile */
	unsigned	u_psr;		/* processor status reg */
	unsigned	u_fsr;		/* floating-point status reg */
	unsigned	u_epsr;		/* extended processor status reg */
	unsigned	u_fir;		/* pc, sometimes... */
	unsigned	__pad[2];	/* unused -- 16-byte alignment */
};


/*
 * following structure is KNOWN by assembly language preceeding the call
 * to emul_syscall() in emul_vector.s. 'unaligned_emul_stack_ptr' is what
 * 'emul_stack_alloc()' returned before we i860 (16-byte) stack-aligned it.
 */
struct emul_stack {
	struct emul_exception_frame	*usp;
	int				unaligned_emul_stack_ptr;
	
	/* 
	 * the frame pointer and the return addr of the user call is now
	 * pushed on the emulator stack
	 */

	int				frame_pointer;
	int				return_addr;	
	struct emul_stack_top		unused;
};

#endif    MAP_UAREA

extern mach_port_t	our_bsd_server_port;

extern boolean_t	must_suspend, emul_interrupt;
extern void		syscall_suspend_barrier();

#define	E_JUSTRETURN	255	/* return without changing registers */
#define E_SIGRETURN	254	/* return and reload saved registers */


int	take_signals();	/* forward */

int	emul_low_entry = -9;

extern  int nsysent;
extern	emul_common();
extern  spin_lock_t  in_emulator_lock;

#ifdef NX
boolean_t nx_application = FALSE;
boolean_t nx_process_lock = FALSE;
#endif /* NX */

#if defined(SLL) || defined(SLLCOMPAT)
boolean_t sll_force_local = FALSE;
#endif /* SLL || SLLCOMPAT */

void
emul_setup(task)
	task_t	task;
{
	register int i;
	register kern_return_t	rc;

	for (i = emul_low_entry; i < nsysent; i++) {
		rc = task_set_emulation(task, (vm_address_t)emul_common, i);
	}
	rc = task_set_emulation(task, (vm_address_t)emul_common, -33);
	rc = task_set_emulation(task, (vm_address_t)emul_common, -34);
	rc = task_set_emulation(task, (vm_address_t)emul_common, -41);
	rc = task_set_emulation(task, (vm_address_t)emul_common, -52);
}

/*
 * System calls enter here.
 */
int
emul_syscall(esp, usp)
	struct emul_stack		*esp;
	struct emul_exception_frame	*usp;
{
	register int	syscode;
	register int	error, take_signals_err;
	register struct sysent	*callp;
	register int	*args = (int *) &usp->u_r16;
	int		rval[2];
	boolean_t	interrupt = FALSE;

#ifdef	MAP_UAREA
	if (shared_enabled) {
	    spin_lock(&in_emulator_lock);
	    shared_base_rw->us_in_emulator++;
		 shared_base_rw->us_emul_regs = esp;
	    spin_unlock(&in_emulator_lock);
	    if (shared_base_ro->us_cursig) {
		error = ERESTART;
		goto signal;
	    }
	}
#endif	MAP_UAREA

	syscode = usp->u_r31;

	if (syscode == 0) {
	    /*
	     * Indirect system call.
	     */
	    syscode = *args++;
	}

	/* e_emulator_error("emulator IN: syscall = %d", syscode); */

	/*
	 * Find system call table entry for the system call.
	 */
	if (syscode >= nsysent)
	    callp = &sysent[SYS_nosys];	/* nosysent */
	else if (syscode >= 0)
	    callp = &sysent[syscode];
	else {
	    /*
	     * Negative system call numbers are CMU extensions.
	     */
	    if (syscode == -33)
		callp = &sysent_task_by_pid;
	    else if (syscode == -34)
		callp = &sysent_pid_by_task;
	    else if (syscode == -41)
		callp = &sysent_init_process;
	    else if (syscode == -59)
		callp = &sysent_htg_ux_syscall;
            else
                callp = &sysent[SYS_nosys];    /* nosysent */
	}

	/*
	 * Set up the initial return values.
	 */
	rval[0] = 0;
	rval[1] = usp->u_r17;

	/*
	 * Call the routine, passing arguments according to the table
	 * entry.
	 */
	switch (callp->nargs) {
	    case 0:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				rval);
		break;
	    case 1:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				*args,
				rval);
		break;
	    case 2:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1],
				rval);
		break;
	    case 3:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1], args[2],
				rval);
		break;
	    case 4:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1], args[2], args[3],
				rval);
		break;
	    case 5:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1], args[2], args[3], args[4],
				rval);
		break;
	    case 6:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1], args[2],
				args[3], args[4], args[5],
				rval);
		break;

	    case -1:	/* generic */
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				syscode,
				args,
				rval);
		break;

	    case -2:	/* pass registers to modify */
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args,
				rval,
				esp);
		usp = esp->usp;
		break;
	}

	/*
	 * Set up return values.
	 */

#ifdef	MAP_UAREA
signal:
#endif	MAP_UAREA

	/*
	 * Convert MiG errors into UNIX errors.
	 */
	switch (error) {
	    case MIG_ARRAY_TOO_LARGE:
		error = ENAMETOOLONG;
		break;
	}

	switch (error) {
	    case E_JUSTRETURN:
	    case E_SIGRETURN:
		/* Do not alter registers */
		break;

	    case 0:
		/* Success */
		usp->u_r16 = rval[0];
		usp->u_r17 = rval[1];
		usp->u_psr &= ~PSR_CC;
		break;

	    case ERESTART:
		/* restart call */
		usp->u_fir -= 4;	/* back up to the trap r31,r31,r0 */
		break;

	    default:
		/* error */
		usp->u_r16 = error;
		usp->u_psr |= PSR_CC;
		break;
	}

#ifdef  MAP_UAREA
        if (shared_enabled) {
            spin_lock(&in_emulator_lock);
            shared_base_rw->us_in_emulator--;
            shared_base_rw->us_emul_regs = 0;
            spin_unlock(&in_emulator_lock);

            if (!interrupt &&
                (shared_base_ro->us_cursig ||
                 (shared_base_rw->us_sig & ~(shared_base_rw->us_sigmask))))
                interrupt = TRUE;
        }
#endif  /* MAP_UAREA */


	/* e_emulator_error("emulator OUT: syscall = %d", syscode); */

 	/*
	 * suspension barrier - DO THIS BEFORE CHECKING SIGNALS
	 */
	if (must_suspend)
		syscall_suspend_barrier();

        /*
	 * If we are to do this because of emul_interrupt, then we should
	 * check that we are the 1st (signal) thread.  But, it is not
	 * worth the overhead.
	 * (anyway, any sync signal can cause the wrong thread to take
	 * the "next" signal anyway).
	 */
	interrupt = interrupt || emul_interrupt;
	emul_interrupt = FALSE;

	/*
         * Handle interrupt request
         */
	if (error == ERESTART || error == EINTR || 
	    error == EPIPE || interrupt) {
		take_signals_err = take_signals(esp);
		if (take_signals_err != 0) {
		    return (take_signals_err);
		} else if (error == EINTR) {
			/* If no signal found and interrupted call, catch case where
			 * process is under debug and the server consumed the signal.
			 * In this case we want to return to the user as if the
			 * signal never occurred.
			 */
			usp->u_r16 = rval[0];
			usp->u_r17 = rval[1];
			usp->u_psr &= ~PSR_CC;
			error = ESUCCESS;
		}
	}

        return (error == E_SIGRETURN) ? 1 : 0;
}

/*
 * Exec starts here to save registers.
 */
struct execa {
    char	*fname;
    char	**argp;
    char	**envp;
};
struct rexeca {
    char	*fname;
    char	**argp;
    char	**envp;
    node_t	node;		/* node on which we'll rexec */
};

int
e_execv(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
        register struct execa	*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	return (e_execvx(serv_port, interrupt, argp, FALSE, regs));
}

int
e_execve(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
        register struct execa	*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	return (e_execvx(serv_port, interrupt, argp, TRUE, regs));
}

e_execvx(serv_port, interrupt, argp, use_env, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register struct execa	*argp;
	struct emul_stack	*regs;
	boolean_t		use_env;
{
	int		entry[2];
	unsigned int	entry_count;
	vm_offset_t	arg_addr;
	register int	error;
	struct emul_exception_frame *regs2;
	boolean_t       traced = FALSE;

	/*
	 * Call exec.  If error, return without changing registers.
	 */
	entry_count = 2;	/* ??? */
	error = e_exec_call(serv_port,
			    interrupt,
			    argp->fname,
			    argp->argp,
			    (use_env) ? argp->envp : (char **)0,
			    &arg_addr,
			    entry,
			    &entry_count,
			    &traced);
	if (error)
	    return (error);

	/*
	 * Put new user stack just below arguments.
	 */
	regs2 = ((struct emul_exception_frame *)arg_addr) - 1;
	regs->usp = regs2;

	/*
	 * Set new pc, and clear frame pointer for traceback.
	 */
	regs2->u_fir = entry[0];
	regs2->u_sp = (unsigned)arg_addr;	/* User stack pointer. */

        /*
         * setup i860 argument registers r16(argc), r17(argv) & r18 (envp)
         * according to i860 calling conventions. 'crt0.o' behaves as if it
         * were called from a higher level 'C' function.
         */
        regs2->u_r16 = *(int *)arg_addr;                        /* argc */
        regs2->u_r17 = (int)(arg_addr + 4);                     /* argv */
        regs2->u_r18 = (regs2->u_r16*4) + regs2->u_r17 + 4;     /* envp */

	regs2->u_fp = 0;	/* cosmetic */
	regs2->u_r1 = 0xdeadc0de;

        if (traced) {   /* STRC flag */
#if     MAP_UAREA
                if (shared_enabled) {
                        share_lock(&shared_base_rw->us_siglock);
                        shared_base_rw->us_sig |= sigmask(SIGTRAP);
                        share_unlock(&shared_base_rw->us_siglock);
                } else
#endif  /* MAP_UAREA */
                {
			emul_blocking();
			(void) bsd_psignal(serv_port, interrupt, SIGTRAP);
			emul_unblocking();
                }
        }

	/*
	 * Return to new stack.
	 */
	return (E_JUSTRETURN);
}


/*
 * Wait has a weird parameter passing mechanism
 * on the VAX, and we have to put up with it.
 */
int
e_owait(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame *regs2 = regs->usp;
	int	new_args[2], error;

	new_args[0] = argp[1];	/* options */
	new_args[1] = argp[2];	/* rusage_p */

	error = emul_vproc_generic(serv_port, interrupt,
			SYS_wait, &new_args[0], rval);
	if (error == 0 && argp[0])
		*((int *)argp[0]) = rval[1];
	return error;
}


/*
 * Take a signal.
 */
unsigned sigtramp;

int
take_signals(regs)
	register struct emul_stack *regs;
{
	register struct emul_exception_frame	*regs2 = regs->usp;
	register struct sigcontext *scp;
	int	old_mask, old_onstack, sig, code, handler, new_sp, error;
	boolean_t	interrupt;
	struct sigframe {
		unsigned	pc;
		unsigned	r16;
		unsigned	r17;
		unsigned	r18;
		unsigned	r19;
	} *fp;

	/*
	 * Get the signal to take from the server.  It also
	 * switches the signal mask and the stack, so we must
	 * be off the old user stack before calling it.
	 * [That's the bottom line for having an emulator stack]
	 */
	emul_blocking();
	error = bsd_take_signal(our_bsd_server_port,
			&interrupt,
			&old_mask,
			&old_onstack,
			&sig,
			&code,
			&handler,
			&new_sp);
	emul_unblocking();

	/*
	 * If there really were no signals to take, return.
	 */
	if (sig == 0 || error != ESUCCESS) {
		if(must_suspend)
			syscall_suspend_barrier();
		return 0;
	}

	if (new_sp != 0) {
		scp = ((struct sigcontext *)new_sp) - 1;
		/* Make sure 16 byte aligned */
		scp = (struct sigcontext *)((unsigned)scp & ~0xf);
	} else {
		/* Build signal frame and context on user's stack. */
		new_sp = (int)regs2;
		scp = (struct sigcontext *) regs2;
	}

	/*
	 * Build the stack frame that we'll return to
	 */
	fp  = ((struct sigframe *)scp) - 1;

	if (!user_rwcheck(fp, new_sp - (int) fp)) {
		/*
		 * Process has trashed its stack;
		 * Simulate an illegal instruction abort.
		 */
		emul_blocking();				/*XXX*/
		(void) bsd_proc_exit(our_bsd_server_port,
				     &interrupt,
				     SIGILL,
				     TRUE);
		emul_unblocking();				/*XXX*/

		/*
		 * If that didn't stop us, panic the emulator.
		 */
		emul_panic("take_signals: bsd_proc_exit failed");
		/* emul_panic does not return */
	}

	if (new_sp != (int)regs2) {
		/*
		 * Copy the sigcontext on new stack
		 * (This is really continued from newsp!=0 case above)
		 * but has to be after user_rwcheck for safety.
		 */
		*scp = *((struct sigcontext *)regs2);
	}

	fp->pc = sigtramp;
	/*
	 * Args to sigtramp
	 */
	fp->r16 = sig;
	fp->r17 = code;
	fp->r18 = (unsigned)scp;
	fp->r19 = handler;

	/*
	 * Fixup what's missing in the signal context.
	 */
	scp->sc_onstack = old_onstack;
	scp->sc_mask = old_mask;

	return (int)fp;
}

/*
 * New sigreturn.
 */
int
e_sigreturn(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	struct a {
		struct sigcontext *sigcp;
	}			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame	*regs2 = regs->usp;
	register int				 rc;
	register struct sigcontext		*scp = argp->sigcp;

	/*
	 * Change signal stack and mask.  If new signals are pending,
	 * do not take them until we switch user stack.
	 */
#ifdef	MAP_UAREA
    if (shared_enabled) {
	rc = e_shared_sigreturn(serv_port,
				interrupt,
				scp->sc_onstack & 01,
				scp->sc_mask);

    } else {
#endif	MAP_UAREA
	emul_blocking();
	rc = bsd_sigreturn(serv_port,
			interrupt,
			scp->sc_onstack & 01,
			scp->sc_mask);
	emul_unblocking();

#ifdef	MAP_UAREA
    }
#endif	MAP_UAREA

	/*
	 * merge the sigcontext structure into the i860_thread_structure
	 * if the sighandler was initiated by the sigsend() of 
	 * the OSF-server.
	 */
	merge_i860regs(scp);

	/*
	 * Change registers: replace user's saved exception state with signal
	 * saved signal context.
	 */
	
#if 0
	bcopy( scp, regs2, sizeof(struct sigcontext) );
#else
	*regs2 = * ((struct emul_exception_frame *)scp);
#endif

	return (E_SIGRETURN);
}

/*
 * Compatibility with 4.2 chmk $139 used by longjmp()
 */
e_osigcleanup()
{
	return (EINVAL);
}



int
e_fork(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame *regs2 = regs->usp;
	register int error;
	struct i860_thread_state	child_regs;
	extern	child_fork();
	
	bzero((char *) &child_regs, sizeof(struct i860_thread_state));

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
        bcopy( (char *)&regs2->u_r1, (char *)&child_regs.r1, 39*4 );
	child_regs.r29 = regs2->u_fir;	/* needed!! */

	child_regs.fsr = regs2->u_fsr;
	child_regs.psr = PSR_PU | PSR_PIM;
	child_regs.pc = (int) child_fork;

	/*
	 * Create the child.
	 */
	error = e_fork_call(serv_port, interrupt, NONODE,
			 &child_regs,
			 i860_THREAD_STATE_COUNT,
#if ! defined(SLL) && ! defined(SLLCOMPAT)
			 &rval[0]);
#else /* ! SLL && ! SLLCOMPAT */
			 &rval[0],
			 sll_force_local);
#endif /* ! SLL && ! SLLCOMPAT */
#ifdef NX
        if (error == ERFORK)
            error = e_rfork_call(serv_port,
                                 interrupt,
                                 0,
                                 (thread_state_t)&child_regs,
                                 i860_THREAD_STATE_COUNT, TRUE, rval);
#endif /* NX */

	if (error == 0)
	    rval[1] = 0;

	return (error);
}


vm_offset_t
set_arg_addr(arg_size)
	vm_size_t	arg_size;
{
	/*
	 * Round to 4-byte alignment
	 */
	arg_size = (arg_size + NBPW-1) & ~(NBPW - 1);

	/*
	 * Put argument list at top of stack.
	 */
	return (USRSTACK - arg_size) & ~0xf;
}


#if 1
ledpanic(s)
char	*s;
{
	for (;;) {
		ledputs("ledpanic: ");
		ledputs(s);
		ledputc('\n');
	}
}

ledputs(s)
char	*s;
{
	char	*p, c;

	p = s;
	while (*p) {
		ledputc((c = *p++));
		if (c == '\n') {
			ledputc('\r');
		}
	}
}


ledputc(c)
int	c;
{
	asm("trap r0,r6,r0");
}
#endif


#ifdef TNC

/*
 * The TNC version of rfork().
 */

int
e_rfork(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame *regs2 = regs->usp;
	register int error;

	struct i860_thread_state	child_regs;

	extern int	child_fork();

	bzero((char *) &child_regs, sizeof(struct i860_thread_state));

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
        bcopy( (char *)&regs2->u_r1, (char *)&child_regs.r1, 39*4 );
	child_regs.r29 = regs2->u_fir;	/* needed!! */

	child_regs.fsr = regs2->u_fsr;
	child_regs.psr = PSR_PU | PSR_PIM;
	child_regs.pc = (int) child_fork;


#ifdef NX
        return(e_rfork_call(serv_port,
                            interrupt,
                            argp[0],            /* argp[0] is the new node */
                            (thread_state_t)&child_regs,
                            i860_THREAD_STATE_COUNT, FALSE, rval));
#else
	return(e_rfork_call(serv_port, 
			    interrupt, 
			    argp[0],		/* argp[0] is the new node */
			    (thread_state_t)&child_regs,
			    i860_THREAD_STATE_COUNT, rval));
#endif /* NX */
}

int
e_migrate(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame *regs2 = regs->usp;

	struct i860_thread_state	child_regs;

	extern int	migrate_arrival();
	extern void	get_seg_regs();

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */

	bzero((char *) &child_regs, sizeof(struct i860_thread_state));

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
        bcopy( (char *)&regs2->u_r1, (char *)&child_regs.r1, 39*4 );
	child_regs.r29 = regs2->u_fir;	/* needed!! */

	child_regs.fsr = regs2->u_fsr;

	if (argp[0] >= 0) {	/* migrate - argp[0] is the new node */
		child_regs.psr = PSR_PU | PSR_PIM;
		child_regs.pc = (int) migrate_arrival;
		return(e_migrate_call(serv_port, 
				      interrupt, 
				      argp[0],
				      (thread_state_t)&child_regs,
						i860_THREAD_STATE_COUNT
#ifdef NX
						,FALSE
#endif /* NX */
						));

	} else {		/* checkpoint - argp[0] is the chkpnt arg */
#ifdef	CHKPNT
		/*
		 * Prepare the user stack to return with a value
		 * of abs(checkpoint arg) on restart.
		 */
		int	arg = argp[0];
		regs2->u_r16 = -arg;
		regs2->u_psr &= ~PSR_CC;
		return(emul_chkpnt_self(serv_port, 
					interrupt, 
					arg,
					(thread_state_t)&child_regs,
					i860_THREAD_STATE_COUNT,
					sigtramp));
#else	/* CHKPNT */
		return(ENOSYS);
#endif	/* CHKPNT */
	}
}

#ifdef	CHKPNT
int
e_forkfamily(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame *regs2 = regs->usp;
	register int error;

	struct i860_thread_state	child_regs;

	extern int	child_fork();

	bzero((char *) &child_regs, sizeof(struct i860_thread_state));

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
        bcopy( (char *)&regs2->u_r1, (char *)&child_regs.r1, 39*4 );
	child_regs.r29 = regs2->u_fir;	/* needed!! */

	child_regs.fsr = regs2->u_fsr;
	child_regs.psr = PSR_PU | PSR_PIM;
	child_regs.pc = (int) child_fork;

	return(e_forkfamily_call(serv_port, 
				 interrupt, 
				 argp[0],	/* nproc */
				 argp[1],	/* pid_list */
				 (thread_state_t)&child_regs, 
				 i860_THREAD_STATE_COUNT, 
				 rval));
}

struct execra {
	char	*chkpnt_prefix;
	int	options;
};

int
e_exec_restart(serv_port, interrupt, argp, use_env, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register struct execra	*argp;
	struct emul_stack	*regs;
	boolean_t		use_env;
{
	struct i860_thread_state	chkpnt_state;
	unsigned int			chkpnt_state_count =
						i860_THREAD_STATE_COUNT;
	register int			error;
	struct emul_exception_frame	*regs2;
	boolean_t      			traced = FALSE;
	boolean_t     			stop = FALSE;

	/*
	 * Call exec.  If error, return without changing registers.
	 */
	error = e_exec_restart_call(serv_port,
				    interrupt,
				    argp->chkpnt_prefix,
				    argp->options,
				    &chkpnt_state,
				    &chkpnt_state_count,
				    &stop,
				    &traced,
				    &sigtramp);
	if (error)
	    return (error);

	/*
	 * Prepare to pick up the changed stack already loaded into the
	 * user task.
	 */
	regs2 = ((struct emul_exception_frame *)chkpnt_state.sp) - 1;
	regs->usp = regs2;

        if (traced) {   /* STRC flag */
#if     MAP_UAREA
                if (shared_enabled) {
                        share_lock(&shared_base_rw->us_siglock);
                        shared_base_rw->us_sig |= sigmask(SIGTRAP);
                        share_unlock(&shared_base_rw->us_siglock);
                } else
#endif  /* MAP_UAREA */
                {
			emul_blocking();
			(void) bsd_psignal(serv_port, interrupt, SIGTRAP);
			emul_unblocking();
                }
        }

	/*
	 *  Stop if so requested.
	 */
	if (stop) {
#if     MAP_UAREA
                if (shared_enabled) {
                        share_lock(&shared_base_rw->us_siglock);
                        shared_base_rw->us_sig |= sigmask(SIGSTOP);
                        share_unlock(&shared_base_rw->us_siglock);
                } else
#endif  /* MAP_UAREA */
                {
			emul_blocking();
			(void) bsd_psignal(serv_port, interrupt, SIGSTOP);
			emul_unblocking();
                }
	}

	/*
	 * Return to new stack.
	 */
	return (E_SIGRETURN);
}
#endif	CHKPNT

int
e_rexecve(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register struct rexeca	*argp;		/* that's rexeca */
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame *regs2 = regs->usp;
	struct i860_thread_state	child_regs;

	extern int	rexecve_arrival();
	extern node_t	emul_tnc_mynode();

	/*
	 * If we're already on the requested node, do a plain execve
	 * instead.
	 */
	if(argp->node == emul_tnc_mynode()) {
		return(e_execve(serv_port, interrupt, argp, rval, regs));
	}

	/*
	 * Set up the registers we'll have when we transition to the
	 * other node.  More registers, in fact, than we really need.
	 */
	bzero((char *) &child_regs, sizeof(struct i860_thread_state));

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
        bcopy( (char *)&regs2->u_r1, (char *)&child_regs.r1, 39*4 );
	child_regs.r29 = regs2->u_fir;	/* needed!! */

	child_regs.fsr = regs2->u_fsr;
	child_regs.psr = PSR_PU | PSR_PIM;
	child_regs.pc = (int)rexecve_arrival;	/* this is special */

	/*
	 * This call will not return if the rexec is successful.
	 * If it is not successful then we will return here and
	 * return the generated error to the caller.
	 */
	return(e_rexecve_call(serv_port, interrupt,
			      argp->fname,
			      argp->argp,
			      argp->envp,
			      argp->node,
			      (thread_state_t)&child_regs,
			      i860_THREAD_STATE_COUNT));
}

/*
 * Machine-dependent portion of rexec(), called after the
 * transition to the new node. This routine is executed after
 * initializing the emulator's stack and then faking a trap
 * with an emulator-internal, pseudo-system call.
 */
int
e_rexecve_arrival(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	vm_offset_t	arg_addr;
	int		entry[16];
	int		entry_count = 16;
	boolean_t	traced = FALSE;
	struct emul_exception_frame *regs2;

	/*
	 * Have the machine-independent routine fill in these args from
	 * our server.  Variable "arg_addr" is the stack location where
	 * all that string info is properly formatted and copied into
	 * memory that will become the user task's stack.  Variable
	 * "entry" gets the starting address (obtained from a special
	 * call to the server).  We have no idea what to do if there
	 * is more than one entry; at most there can be two.
	 */
	e_rexecve_arrival_call(serv_port, interrupt,
			       &arg_addr, entry, &entry_count, &traced);

	/*
	 * Put new user stack just below arguments.
	 */
	regs2 = ((struct emul_exception_frame *)arg_addr) - 1;
	regs->usp = regs2;

	/*
	 * Set new pc, and clear frame pointer for traceback.
	 */
	regs2->u_fir = entry[0];
	regs2->u_sp = (unsigned)arg_addr;	/* User stack pointer. */

        /*
         * setup i860 argument registers r16(argc), r17(argv) & r18 (envp)
         * according to i860 calling conventions. 'crt0.o' behaves as if it
         * were called from a higher level 'C' function.
         */
        regs2->u_r16 = *(int *)arg_addr;                        /* argc */
        regs2->u_r17 = (int)(arg_addr + 4);                     /* argv */
        regs2->u_r18 = (regs2->u_r16*4) + regs2->u_r17 + 4;     /* envp */

	regs2->u_fp = 0;	/* cosmetic */
	regs2->u_r1 = 0xdeadc0de;

        if (traced) {   /* STRC flag */
#if     MAP_UAREA
                if (shared_enabled) {
                        share_lock(&shared_base_rw->us_siglock);
                        shared_base_rw->us_sig |= sigmask(SIGTRAP);
                        share_unlock(&shared_base_rw->us_siglock);
                } else
#endif  /* MAP_UAREA */
                {
			emul_blocking();
			(void) bsd_psignal(serv_port, interrupt, SIGTRAP);
			emul_unblocking();
                }
        }

	/*
	 * Return to new stack.
	 */
	return (E_JUSTRETURN);
}

/*
 * This handler is entered on delivery of the SIGMIGRATE signal.
 * code is the argument to the signal which specifies the node
 * to which migration is to be attempted.
 * The handler effectively calls the migrate() system call - however,
 * because we're inside the emulator, we fake this call through an
 * assembler routine, emul_migrate(). 
 */
void emul_sigmigrate_handler(int sig, int code)
{
	extern int emul_migrate();
	(void) emul_migrate(code);
}

int
e_rforkmulti(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame *regs2 = regs->usp;
	register int error;

	struct i860_thread_state	child_regs;

	extern int	child_fork();

	bzero((char *) &child_regs, sizeof(struct i860_thread_state));

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
        bcopy( (char *)&regs2->u_r1, (char *)&child_regs.r1, 39*4 );
	child_regs.r29 = regs2->u_fir;	/* needed!! */

	child_regs.fsr = regs2->u_fsr;
	child_regs.psr = PSR_PU | PSR_PIM;
	child_regs.pc = (int) child_fork;

	return(e_rforkmulti_call(serv_port, 
				 interrupt, 
				 argp[0],	/* count */
				 argp[1],	/* node_array */
				 argp[2],	/* rval_array */
				 argp[3],	/* pid_array */
				 (thread_state_t)&child_regs, 
				 i860_THREAD_STATE_COUNT, 
				 rval));
}

#else /* TNC */

/*
 * The non-TNC version of rfork().
 */
int
e_rfork(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_stack	*regs;
{
	register struct emul_exception_frame *regs2 = regs->usp;
	register int error;

	struct i860_thread_state	child_regs;

	extern int	child_fork();

	bzero((char *) &child_regs, sizeof(struct i860_thread_state));

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
        bcopy( (char *)&regs2->u_r1, (char *)&child_regs.r1, 39*4 );
	child_regs.r29 = regs2->u_fir;	/* needed!! */

	child_regs.fsr = regs2->u_fsr;
	child_regs.psr = PSR_PU | PSR_PIM;
	child_regs.pc = (int) child_fork;

	return(e_fork_call(serv_port, 
			    interrupt, 
			    argp[0],		/* argp[0] is the new node */
			    (thread_state_t)&child_regs,
#if ! defined(SLL) && ! defined(SLLCOMPAT)
			    i860_THREAD_STATE_COUNT, rval));
#else /* ! SLL && ! SLLCOMPAT */
			    i860_THREAD_STATE_COUNT, rval, TRUE));
#endif /*  ! SLL && ! SLLCOMPAT */
}
#endif /* TNC */

/*
 * Routine to set the emulator thread's state
 */

kern_return_t
emul_thread_set_state(th, stack, start)
	thread_t	th;
	unsigned int	stack, start;
{
	struct i860_thread_state ts;
	unsigned int	count = i860_THREAD_STATE_COUNT;
	kern_return_t	rc;


	rc = thread_get_state(th, i860_THREAD_STATE, (thread_state_t)&ts,
				&count);
	if (rc != KERN_SUCCESS)
		return rc;
	if (count != i860_THREAD_STATE_COUNT)
		emul_panic("emul_thread_set_state: bad count");

	ts.fp = 0;				/* clear frame ptr */
	ts.sp = stack;				/* set stack pointer */
	ts.pc = start;				/* set program counter */

	rc = thread_set_state(th, i860_THREAD_STATE, (thread_state_t)&ts,
				i860_THREAD_STATE_COUNT);
	if (rc != KERN_SUCCESS)
		return rc;

	return rc;
}

/*
 * Get thread state of emulator thread named by thread port, possibly
 * returning it into caller-supplied buffer.
 * If caller requests, explicitly return pc and "thread id".
 */
kern_return_t
emul_thread_get_state(th, tsp, pc, id)
	thread_t	th;
	thread_state_t	tsp;
	unsigned int	*pc;
	emul_thread_id	*id;
{
	struct i860_thread_state ts;
	unsigned int	count = i860_THREAD_STATE_COUNT;
	kern_return_t	rc;

	if (tsp == NULL)
		tsp = (thread_state_t)&ts;

	rc = thread_get_state(th, i860_THREAD_STATE, tsp, &count);
	if (rc != KERN_SUCCESS) {
		if (pc)
			*pc = 0;
		if (id)
			*id = 0;
		return rc;
	}
	if (count != i860_THREAD_STATE_COUNT)
		emul_panic("emul_thread_set_state: bad count");

	if (pc)
		*pc = ((struct i860_thread_state *)tsp)->pc;
	if (id)
		*id = (emul_thread_id)EMUL_STACK_BASE(
				((struct i860_thread_state *)tsp)->sp);

	return rc;
}

/*
 * Return my "thread id".
 *
 * If you can't implement it via emulator stack addresses, you can fall back
 * on the expensive mach_thread_self() call.
 */
emul_thread_id
emul_thread_id_self()
{
	volatile int	on_the_stack;
	return (emul_thread_id)EMUL_STACK_BASE(&on_the_stack);
}


/*
 * Return thread id of emulator thread named via EITHER the thread_t
 * or the thread_state_t (as returned by emul_thread_get_state).
 *
 * A return of 0 indicates the thread_id could not be computed.
 */
emul_thread_id
emul_thread_id_of(th, tsp)
	thread_t	th;
	thread_state_t	tsp;
{
	emul_thread_id	id;
	kern_return_t	rc;

	if (tsp) {
		return (emul_thread_id)EMUL_STACK_BASE(
				((struct i860_thread_state *)tsp)->sp);
	} else {
		rc = emul_thread_get_state(th, tsp, (unsigned int *)NULL, &id);
		if (rc == KERN_SUCCESS)
			return id;
		else
			return 0;
	}
}

#ifdef NX

extern mach_port_t vproc_port;
extern node_t tnc_mynode;

int
e__nx_init(serv_port, interrupt, nx_info, appl_info, magic_num, selector_cnt,
  selector_buf, selector_buflen, rval)
        mach_port_t     serv_port;
        boolean_t       *interrupt;
        NX_INIT_T       *nx_info;
        APPLINFO_T      *appl_info;
	int		selector_cnt;
	char		*selector_buf;
	int		selector_buflen;
        int             magic_num;
        int             *rval;
{
        int             error;
        int             numnodes;
	int		len_cmd_buf;
	long            process_lock;

	/*
	 *  Validate that this application is compatible with the
	 *  current version of the server/emulator.
	 */
	if (magic_num != NX_APP_VERSION)
		return (EINCOMPAT);

	/*
	 *  nx_initve() can be called only once per application.
	 */
	if (nx_application)
	    return EAEXIST;

	if (!user_rcheck(appl_info, sizeof *appl_info))
		return EFAULT;

	if (!user_rcheck(nx_info, sizeof *nx_info))
		return EFAULT;

	process_lock = appl_info->process_lock;

	emul_blocking();
#ifdef	CHKPNT
	if (!user_strlen(nx_info->cmd_line, &len_cmd_buf))
		return EFAULT;
#endif	/* CHKPNT */
	error = NX__nx_init(vproc_port, interrupt, *nx_info, &numnodes,
			    *appl_info, selector_cnt, selector_buf,
			    selector_buflen);
	emul_unblocking();

        *rval = numnodes;
	if (error == ESUCCESS){
		 nx_application = TRUE;
		 nx_process_lock = (boolean_t) process_lock;

	/*
	 * get a new tnc_mynode.
	 */
		 tnc_mynode = -1;

#ifdef SLL
		/*
		 * No more SLL for this process.
		 */
		sll_force_local = TRUE;
#endif /* SLL */
	}
        return(error);
}

int
e_nx_attach_pgrp(serv_port, interrupt, pgrp, rval)
        mach_port_t     serv_port;
        boolean_t       *interrupt;
        pid_t            pgrp;
        int             *rval;
{
        int             error;

	emul_blocking();
        error = NX_attach_pgrp(vproc_port, interrupt,
                               pgrp);
	emul_unblocking();

        return(error);
}

int
e_nx_detach_pgrp(serv_port, interrupt, pgrp, rval)
        mach_port_t     serv_port;
        boolean_t       *interrupt;
        pid_t            pgrp;
        int             *rval;
{
        int             error;

	emul_blocking();
        error = NX_detach_pgrp(vproc_port, interrupt,
                               pgrp);

	emul_unblocking();
        return(error);
}

int
e_nx_join(serv_port, interrupt, argp, rval, regs)
        mach_port_t     serv_port;
        boolean_t       *interrupt;
        int             *argp;
        int             *rval;
        struct emul_stack       *regs;
{
        int             error;
        pid_t           pgrp = argp[0];

	emul_blocking();
        error = NX_join(vproc_port, interrupt,
                               pgrp, rval);
	emul_unblocking();

	if (error == ESUCCESS){
	/*
	 * get a new tnc_mynode.
	 */
		 tnc_mynode = -1;

#ifdef SLL
		/*
		 * No more SLL for this process.
		 */
		sll_force_local = TRUE;
#endif /* SLL */
	}

#ifdef FORCE_MIGRATION
        if (error == EREMOTE) {
            error = emul_force_migrate(0);
        }
#endif /* FORCE_MIGRATION */
        return(error);
}

int
e_nx_pri(serv_port, interrupt, pid, pri, rval )
        mach_port_t     serv_port;
        boolean_t       *interrupt;
        pid_t           pid;
        int             pri;
        int             *rval;
{

        int error;


	emul_blocking();
        error = NX_nx_pri(vproc_port, interrupt, pid, pri);
	emul_unblocking();
        return(error);

}

int
e_nx_part_attributes(serv_port, interrupt,inode, partinfo)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	ino_t		inode;
	PARTREQ_T	*partinfo;
{
	int	error;

	emul_blocking();
	error = NX_nx_part_attributes(vproc_port,interrupt,inode,partinfo);
	emul_unblocking();
	return(error);
}

int
e_nx_node_list(serv_port, interrupt,inode, node_list,node_list_size)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	ino_t			inode;
	LP_MAP_T		*node_list;
	mach_msg_type_number_t	*node_list_size;
{
	int	error;

	emul_blocking();
	error = NX_nx_node_list(vproc_port,interrupt,inode,node_list,node_list_size);
	emul_unblocking();
	return(error);
}

int
e_nx_part_bitmap(serv_port, interrupt,inode, bitmap,bitmap_size)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	ino_t			inode;
	MIG_BITMAP_T		*bitmap;
	mach_msg_type_number_t	*bitmap_size;
{
	int	error;

	emul_blocking();
	error = NX_nx_part_bitmap(vproc_port,interrupt,inode,bitmap,bitmap_size);
	emul_unblocking();
	return(error);
}

int
e_nx_matching_bitmap(serv_port, interrupt, bitmap, bitmap_size,
  selector_buf, selector_buflen, exact )
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	MIG_BITMAP_T		*bitmap;
	mach_msg_type_number_t	*bitmap_size;
	char*			selector_buf;
	int			selector_buflen;
	int			exact;
{
	int	error;

	emul_blocking();
	error = NX_nx_matching_bitmap(vproc_port,interrupt,bitmap,
			bitmap_size,selector_buf,selector_buflen,exact);
	emul_unblocking();
	return(error);
}

int
e_nx_mypart(serv_port, interrupt, rows, cols)
        mach_port_t     serv_port;
        boolean_t       *interrupt;
        int             *rows;
        int             *cols;
{
        int     error;
        emul_blocking();
        error = NX_nx_mypart(vproc_port,interrupt,rows,cols);
        emul_unblocking();
        return(error);
}

int
e_nx_app_info(serv_port, interrupt,pgroup, app_info)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	pid_t		pgroup;
	nx_app_info_t	*app_info;
{
	int	error;

	emul_blocking();
	error = NX_nx_app_info(vproc_port,interrupt,pgroup,app_info);
	emul_unblocking();
	return(error);
}

int
e_nx_app_node_list(serv_port, interrupt,pgroup, node_list,node_list_size)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	pid_t			pgroup;
	LP_MAP_T		*node_list;
	mach_msg_type_number_t	*node_list_size;
{
	int	error;

	emul_blocking();
	error = NX_nx_app_node_list(vproc_port,interrupt,pgroup,node_list,node_list_size);
	emul_unblocking();
	return(error);
}

int
e_nx_free_node_list(serv_port, interrupt,inode, bitmap,bitmap_size)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	ino_t			inode;
	MIG_BITMAP_T		*bitmap;
	mach_msg_type_number_t	*bitmap_size;
{
	int	error;

	emul_blocking();
	error = NX_nx_free_node_list(vproc_port,interrupt,inode,
			bitmap,bitmap_size);
	emul_unblocking();
	return(error);
}
int
e_nx_mkpart_rpc(serv_port, interrupt, buffer,buf_len,num_nodes)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	MIG_BITMAP_T	buffer;
	int		buf_len;
	int		*num_nodes;
{
	int	error;

	emul_blocking();
	error = NX_nx_mkpart_rpc(vproc_port,interrupt,buffer,buf_len,num_nodes);
	emul_unblocking();
	return(error);
}

int
e_nx_rmpart_rpc(serv_port, interrupt, pathname, pathlen, inode, flags)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*pathname;
	int		pathlen;
	ino_t		inode;
	int		flags;
{
	int	error;

	emul_blocking();
	error = NX_nx_rmpart_rpc(vproc_port,interrupt,pathname,pathlen,inode,flags);
	emul_unblocking();
	return(error);
}

int
e_nx_chpart_rpc(serv_port, interrupt,buffer, buf_len)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*buffer;
	int		buf_len;
{
	int	error;
	emul_blocking();
	error = NX_nx_chpart_rpc(vproc_port,interrupt,buffer,buf_len);
	emul_unblocking();
	return(error);
}

int
e_nx_register_daemon(serv_port, interrupt, daemon_id, socket_addr, sock_addr_len)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		daemon_id;
	u_char		*socket_addr;
	int		sock_addr_len;
{
	int	error;
	emul_blocking();
	error = NX_nx_register_daemon(vproc_port,interrupt,daemon_id,socket_addr,sock_addr_len);
	emul_unblocking();
	return(error);
}

int
e_nx_pspart_rpc(serv_port, interrupt,inode,buffer, buf_len)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	ino_t			inode;
	MIG_BITMAP_T		*buffer;
	mach_msg_type_number_t	*buf_len;
{
	int	error;
	emul_blocking();
	error = NX_nx_pspart_rpc(vproc_port,interrupt,inode,buffer,buf_len);
	emul_unblocking();
	return(error);
}

int
e_nx_alloc_exit_rpc(serv_port, interrupt, pgroup)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	pid_t		pgroup;
{
	int	error;
	emul_blocking();
	error = NX_nx_alloc_exit_rpc(vproc_port,interrupt,pgroup);
	emul_unblocking();
	return(error);
}

int
e_join_root_part(serv_port, interrupt, rval )
        mach_port_t     serv_port;
        boolean_t       *interrupt;
        int             *rval;
{

        int error;


	emul_blocking();
        error = NX_join_root_part(vproc_port, interrupt);
	emul_unblocking();
	if (error == ESUCCESS){
	/*
	 * get a new tnc_mynode.
	 */
	    tnc_mynode = -1;

#ifdef SLL
		/*
		 * No more SLL for this process.
		 */
		sll_force_local = TRUE;
#endif /* SLL */
	}
        return(error);

}

#ifdef FORCE_MIGRATION
int
e_force_migrate(serv_port, interrupt, argp, rval, regs)
        mach_port_t             serv_port;
        boolean_t               *interrupt;
        int                     *argp;
        int                     *rval;
        struct emul_stack       *regs;
{
        register struct emul_exception_frame *regs2 = regs->usp;

        struct i860_thread_state        child_regs;

        extern int      migrate_arrival();
        extern void     get_seg_regs();

        /*
         * Set up registers for child.  It resumes on its own stack.
         */

        bzero((char *) &child_regs, sizeof(struct i860_thread_state));
        /*
         * Set up registers for child.  It resumes on its own stack.
         */

        /* 39 registers total */
        bcopy( &regs2->u_r1, &child_regs.r1, 39*4 );
	child_regs.r29 = regs2->u_fir;	/* needed!! */

        child_regs.fsr = regs2->u_fsr;
        child_regs.psr = PSR_PU | PSR_PIM;
        child_regs.pc = (int) migrate_arrival;

        return(e_migrate_call(serv_port,
                              interrupt,
                              argp[0],          /* argp[0] is the new node */
                              (thread_state_t)&child_regs,
                              i860_THREAD_STATE_COUNT, TRUE));
}
#endif /* FORCE_MIGRATION */

#endif /* NX */

/*
 * merge_i860regs() merges the sigcontext structure into the 
 * i860_thread_state structure if u_r0 of the sigcontext
 * structure is non-zero. The sendsig() of the OSF server passes
 * the based address of the i860_thread_state via u_r0 to be able
 * to completely restore the user state using the kernel assisted 
 * SIGRETURN trap.
 */
merge_i860regs(regs2)
struct emul_exception_frame *regs2;
{
        struct i860_thread_state        *regs;

	if (regs2->u_r0 != 0) {
		regs = (struct i860_thread_state *) regs2->u_r0;
		regs->r1 = regs2->u_r1;
        	regs->sp = regs2->u_sp;
        	regs->fp = regs2->u_fp;
        	regs->r4 = regs2->u_r4;
	        regs->r5 = regs2->u_r5;
        	regs->r6 = regs2->u_r6;
	        regs->r7 = regs2->u_r7;
        	regs->r8 = regs2->u_r8;
	        regs->r9 = regs2->u_r9;
        	regs->r10 = regs2->u_r10;
        	regs->r11 = regs2->u_r11;
	        regs->r12 = regs2->u_r12;
        	regs->r13 = regs2->u_r13;
	        regs->r14 = regs2->u_r14;
        	regs->r15 = regs2->u_r15;
        	regs->r16 = regs2->u_r16;
	        regs->r17 = regs2->u_r17;
        	regs->r18 = regs2->u_r18;  
	        regs->r19 = regs2->u_r19;  
        	regs->r20 = regs2->u_r20;  
	        regs->r21 = regs2->u_r21;  
        	regs->r22 = regs2->u_r22;  
	        regs->r23 = regs2->u_r23;  
        	regs->r24 = regs2->u_r24;  
	        regs->r25 = regs2->u_r25;  
        	regs->r26 = regs2->u_r26;  
	        regs->r27 = regs2->u_r27;  
        	regs->r28 = regs2->u_r28;  
	        regs->r29 = regs2->u_r29;
        	regs->r30 = regs2->u_r30;  
	        regs->r31 = regs2->u_r31;  
        	regs->f2 = regs2->u_f2;
	        regs->f3 = regs2->u_f3;
        	regs->f4 = regs2->u_f4;
	        regs->f5 = regs2->u_f5;
        	regs->f6 = regs2->u_f6;
	        regs->f7 = regs2->u_f7;
        	regs->fsr = regs2->u_fsr;
	        regs->psr = regs2->u_psr;
		regs->epsr = regs2->u_epsr;
	        regs->pc = regs2->u_fir;
	}
}

/*
 *  Send all traps which have become incompatible with
 *  the current server/emulator here.
 *
 */
int
e_incompatible(serv_port, interrupt, rval)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*rval;
{
    return (EINCOMPAT);
}
