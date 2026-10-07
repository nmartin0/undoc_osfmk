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
 * Revision 1.12  1995/02/01  21:23:57  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/11/18  20:25:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/06/24  00:25:26  yazz
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
 * Revision 1.9  1994/03/14  01:45:10  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, Checkpoint Restart specific, EATS
 *  Module(s):
 *
 * Revision 1.8  1993/07/14  17:32:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.7  1993/07/01  18:25:03  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  18:59:00  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.5  1993/05/03  17:18:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:18:59  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.2  1993/02/16  20:01:35  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.5  1993/01/21  18:03:32  nandy
 * 1/20/93 code drop from locus1/20/93 code drop from locus
 *
 * Revision 1.1.2.2.2.1  1992/12/16  05:57:34  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 2.34  93/06/16  13:49:43  klh
 * 	Revision 2.24  93/06/03  17:34:53  rabii
 * 		Added non TNC rfork, also e_fork_call now take a node argument (rabii)
 * 
 * Revision 2.33  93/04/24  18:46:46  klh
 * 	Revision 2.23  93/02/02  15:21:36  rabii
 * 		Change the test for error in take_signals from 0 to ESUCCESS
 *
 * 	Revision 2.22  93/01/06  14:58:20  loverso
 * 		Missing argument in sysV-compat signal(). (loverso)
 * 		Can get a ESRCH from bsd_take_signals(). (chrisp)
 * 		Map-uarea + XMM debugging. (loverso)
 *
 * Revision 2.32  92/12/14  14:42:58  chrisp
 * In take_signal(), take note whether the server has returned an error
 * 	from bsd_take_signal() and return immediately if so.
 * 
 * Revision 2.31  92/11/23  15:59:33  klh
 * 	Revision 2.21  92/11/17  19:46:39  loverso
 * 		Test global interrupt flag for non-handled forwarded signals
 * 		(bug 459).
 * 
 * Revision 2.30  92/10/28  14:47:08  roman
 * Fix some types for a cleaner compilation.
 * 
 * Revision 2.29  92/10/08  11:20:08  roman
 * Routine rexecve_init() moved to emul_init.c.
 * Add new routine e_rexecve_arrival() which looks much like the code
 * 	in e_execve(). This change was necessary to correctly
 * 	handle SIGTRAP signals after an rexecve() of a traced
 * 	process.
 * 
 * Revision 2.28  92/10/06  12:05:27  roman
 * Fix RCS comments.
 * 
 * Revision 2.27  92/10/05  15:26:17  klh
 * 	Revision 2.18  92/09/24  16:48:50  rabii
 * 		If traced (STRC), SIGTRAP from exec _after_ requisite state is 
 *		set up. (dwm; #376)
 * 
 * 	Revision 2.17  92/08/26  12:09:44  loverso
 * 		Add syscall-end barrier at end of emul_syscall.
 * 		Add emul_thread_id_self(), emul_thread_id_of(), and
 * 		emul_thread_get_state().  Fix some lint.
 * 		(loverso)
 * 
 * Revision 2.26  92/10/01  10:17:11  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 2.25  92/07/28  12:53:20  chrisp
 * Remove EPRINT notifying failure of default SIGMIGRATE handler.
 * 
 * Revision 2.24  92/07/08  09:01:20  roman
 * Do typecasting correctly to get a clean compilation for TNC code.
 * 
 * Revision 2.23  92/07/07  15:05:06  roman
 * Minor format corrections to exactly match OSF source code.
 * 
 * Revision 2.22  92/06/19  14:39:26  chrisp
 * [Bug #28] kill3 -32 1 0 hangs - emulator routine take_signals() still
 * 	mishandling sigreturn address - "&" missed in previous change.
 * 
 * Revision 2.21  92/06/10  14:13:30  chrisp
 * [Bug #24] For TNC, honor the most significant bit of a sigreturn address
 * 	needing to be cleared - unless the address is within the emulator's
 * 	address space.
 * 
 * Revision 2.20  92/06/05  13:54:45  klh
 * 	Revision 2.14  92/05/31  18:55:41  loverso
 * 		Revision 3.19  92/04/29  16:51:05  barbou
 * 		Fix for bug #127 again: the stack validity check was not 
 *		checking in the right direction.
 * 
 * 		Revision 3.18  92/04/08  20:44:22  barbou
 * 		Fix for bug #127: handle trashed user stack when delivering 
 *		signals.
 * 
 * Revision 2.19  92/04/17  09:46:58  chrisp
 * Amend emul_sigmigrate_handler() not to reference errno.
 * 
 * Revision 2.18  92/04/14  09:58:24  roman
 * Minor formatting fixes.
 * 
 * Revision 2.17  92/04/01  16:13:13  roman
 * Add support for machine dependent portion of rforkmulti() system call.
 * Remove stub for rexecmulti() system call, which is no longer in the plan.
 * Minor formatting fixes.
 * 
 * Revision 2.16  92/03/27  17:30:43  roman
 * emul_tnc_mynode() is now a subroutine rather than a variable (used for
 * 	TNC calculation of the current node).
 * 
 * Revision 2.15  92/03/27  10:50:30  roman
 * For rexecve(), allow the call to short-circuit and suceed if the node
 * 	number specified is the current node.
 * 
 * Revision 2.14  92/03/24  10:18:33  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.11  92/03/09  11:04:27  durriya
 * 	[Revision 3.17  91/12/24  12:37:40  sp]
 * 	use psl.h rather than the old bsd eflags.h
 * 
 * Revision 2.10  92/02/11  18:41:07  pjg
 * 	Add code for rfork()/rexec()/migrate() part of TNC (roman@locus).
 * 
 * Revision 2.9  92/01/17  17:15:15  roy
 * 	Added emul_thread_set_state (loverso).
 * 
 * Revision 2.8  92/01/14  10:28:47  roy
 * 	Define register structures for !MAP_UAREA case (sjs).
 * 
 * Revision 2.7  91/12/18  09:58:31  roy
 * 	Fix arg to e_owait.
 * 
 * Revision 2.6  91/12/16  09:51:24  roy
 * 	91/12/13  13:02:08  sp
 * 	Remove CMUCS conditionals
 * 
 * 	91/11/13  14:55:20  barbou
 * 	Fix for bug #1: redirect invalid syscalls to a real "nosys".
 * 	Fix for bug #39 and maybe others: force the emulator to check for 
 * 	pending signals more often.
 * 
 * 	91/11/08  13:55:18  bernadat
 * 	Added description of fpu initilalization
 * 
 * 	91/10/29  16:10:57  barbou
 * 	Fix for bug #25: convert MIG_ARRAY_TOO_LARGE into ENAMETOOLONG.
 * 
 * 	91/10/10  15:56:18  barbou
 * 	Check for pending SIGPIPE if errno==EPIPE.
 * 
 * 	91/10/09  18:32:22  emcmanus
 * 	Moved the emul_regs structures into ushared.h so the server can know 
 * 	about them.  Save the emulator's initial sp in the shared data so it        
 * 	is accessible to the server, which uses it to unwind the stack back 
 * 	to the user when producing a coredump.
 * 
 * 	91/10/01  10:37:09  sp
 * 	continue with the emulation even though all system calls are not
 * 	emulated. This allows new servers to work with old kernels.
 * 
 * 	91/09/25  15:26:00  sp
 * 	delete emul_high_entry and use nsysent instead, which is the right 
 * 	number Also check for the return on task_set_emulation as some 
 * 	kernels will not allow you to register enough system calls and this
 * 	should tell us.
 * 
 * Revision 2.5  91/12/13  10:05:54  roy
 * 	Add syscall tracing (commented out).
 * 
 * Revision 2.4  91/10/14  13:16:18  srl
 * (Grenoble V2.2 merge)
 * 	91/10/01  10:37:09  sp
 * 	continue with emulation even though all system calls are not emulated.
 * 	This allows new servers to work with old kernels.
 * 
 * 	91/09/25  15:26:00  sp
 * 	delete emul_high_entry and use nsysent instead, which is right number.
 * 	Also check for the return on task_set_emulation as some kernels will
 * 	not allow you to register enough system calls and this should tell us.
 * 
 * Revision 2.3  91/09/17  12:17:52  sjs
 * integrate Locus changes
 * e_fork() divided into machine dependent portion and machine
 * independent portion.  Machine dependent portion (in this file)
 * calls machine independent portion (in bsd_user_side.c).  Slight
 * change to interface to wait to now use a vproc port.
 * 
 * Revision 2.2  91/08/30  16:40:32  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.8  91/08/09  11:43:07  barbou
 * Added i386_signal(). Added the in_emulator counter.
 * 
 * Revision 3.7  91/07/18  15:36:51  benjamin
 * Increase emulated sys call upper bound from 181
 * to 188 to permit syscall 188 (mmap_hi_res_clock)
 * to be defined.
 * 
 * Revision 3.6  91/06/12  09:33:19  condict
 * Eliminate oexecve syscall and stubs (no longer needed for bootstrap).
 * 
 * Revision 3.5  91/05/30  17:25:00  jose
 * Added o prefix to compatibility syscalls.
 * 
 * Revision 3.4  91/05/24  11:26:50  jose
 * Changed wait() arguments according to OSF/1 server.
 * 
 * Revision 3.3  91/05/15  17:22:29  barbou
 * New entry point: i386_sigaction() replacing e_sigaction in the sysent[].
 * Fatal and obsolete kludge on sigreturn address discarded.
 * 
 * Revision 3.2  91/05/07  15:28:13  condict
 * Add OSF/1 e_execve function (and rename 4.3 version to oexecve).
 * 
 * Revision 3.1  91/02/27  16:23:15  condict
 * Eliminate CMU syscalls.
 * 
 * Revision 3.0  91/01/17  12:05:13  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
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
 * $EndLog
 *
 */
/*
 * Take/Return from signals - machine-dependent.
 */
#include <mach/mach.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <mach/mig_errors.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <machine/psl.h>
#include <machine/vmparam.h>
#include <uxkern/bsd_msg.h>	/* error code definitions */
#include "emul_stack.h"
#include "emul.h"

#include <syscall_table.h>

#ifdef	MAP_UAREA
#include <sys/ushared.h>

extern int shared_enabled;
extern struct ushared_ro *shared_base_ro;
extern struct ushared_rw *shared_base_rw;
#else	MAP_UAREA
/* copied out of sys/ushared.h */
/* The emulator saves its initial sp here so that coredumps can dump the user's
   stack accurately, ignoring the emulator.  If there is more than one thread
   this value may be inaccurate, but the coredump format cannot accomodate
   more than one thread anyway. */

/*
 * User's registers are stored on partially on the user's stack
 * and partially on the emulator's stack.
 */
struct emul_regs_2 {
	int	edx;
	int	ecx;
	int	eax;
	int	eflags;
	int	eip;
};

struct emul_regs {
	int	ebp;
	int	edi;
	int	esi;
	int	ebx;
	struct emul_regs_2 *uesp;
				/* pointer to rest of user registers */
};

#endif	MAP_UAREA

extern mach_port_t	our_bsd_server_port;

extern boolean_t	must_suspend, emul_interrupt;
extern void		syscall_suspend_barrier();

vm_offset_t	sigreturnaddr;	/* signal trampoline, set by sigvec */

#define	E_JUSTRETURN	255	/* return without changing registers */

void	take_signals();			/* forward */

/*
 * Change system call table for sigvec to point to
 * our own routine.
 */

#ifdef	COMPAT_43
int	i386_osigvec();		/* forward */
struct sysent i386_syscall_osigvec =
	{ E_CHANGE_REGS, i386_osigvec };
#endif	/* COMPAT_43 */
int	i386_sigaction(), i386_signal() ;	/* forward */
struct sysent i386_syscall_sigaction =
        { E_CHANGE_REGS, i386_sigaction };
struct sysent i386_syscall_signal = 
	{ E_CHANGE_REGS, i386_signal };

int	emul_low_entry = -9;

extern emul_common();
extern void emul_panic();

#ifdef	MAP_UAREA
extern spin_lock_t in_emulator_lock;
#endif 

void
emul_setup(task)
	task_t	task;
{
	register int i;
	register kern_return_t	rc;


	for (i = emul_low_entry; i < nsysent; i++) { 
		rc = task_set_emulation(task, (vm_address_t)emul_common, i);
		if (rc != KERN_SUCCESS) {
			e_emulator_error("Could not set emulation for syscall %d error %d\n", i, rc);
			break;
		}
	}
	rc = task_set_emulation(task,
			(vm_address_t)emul_common,
			-33);
	rc = task_set_emulation(task,
			(vm_address_t)emul_common,
			-34);
	rc = task_set_emulation(task,
			(vm_address_t)emul_common,
			-41);
	rc = task_set_emulation(task,
			(vm_address_t)emul_common,
			-52);

	/*
	 * Change sigvec to point to our routine - the signal
	 * trampoline address is passed in VERY strangely.
	 */
#ifdef	COMPAT_43
	sysent[SYS_osigvec] = i386_syscall_osigvec;
#endif	/* COMPAT_43 */
	sysent[SYS_signal] = i386_syscall_signal;
	sysent[SYS_sigaction] = i386_syscall_sigaction;
}

/*
 * System calls enter here.
 */
void
emul_syscall(regs)
	register struct emul_regs *regs;
{
	register int	syscode;
	register int	error;
	register struct sysent *callp;
	int		rval[2];
	boolean_t	interrupt = FALSE;
	register struct emul_regs_2 *regs2;
	register int	*args;

	regs2 = regs->uesp;
	args = (int *)(regs2+1);	/* args on stack top */
	args++;	/* point to first argument - skip return address */

	syscode = regs2->eax;

#ifdef	MAP_UAREA
	if (shared_enabled) {
#ifdef MUDEBUG
	    extern unsigned long	on_emul_stack;
#endif

	    spin_lock(&in_emulator_lock);

#ifdef MUDEBUG
EASSERT(0 == shared_base_rw->us_in_emulator);
EASSERT(1 == on_emul_stack);
#endif

	    shared_base_rw->us_in_emulator++;
	    shared_base_rw->us_emul_regs = regs;
	    spin_unlock(&in_emulator_lock);
	    if (shared_base_ro->us_cursig) {
		error = ERESTART;
		goto signal;
	    }
	}
#endif	MAP_UAREA

	if (syscode == 0) {
	    /*
	     * Indirect system call.
	     */
	    syscode = *args++;
	}

/*e_emulator_error("emulator IN: syscall = %d", syscode);*/

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
		callp = &sysent[SYS_nosys];	/* nosysent */
	}

	/*
	 * Set up the initial return values.
	 */
	rval[0] = 0;
	rval[1] = regs2->edx;


#ifdef MUDEBUG
EASSERT(shared_base_rw->us_in_emulator == 1 && 10);
#endif

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
				args[0],
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
				regs);
		regs2 = regs->uesp;	/* if changed */
		break;
	}

	/*
	 * Set up return values.
	 */

#ifdef	MAP_UAREA
#ifdef MUDEBUG
EASSERT(shared_base_rw->us_in_emulator == 1 && 11);
#endif
signal:
#endif	MAP_UAREA

	/*
	 * Convert MiG errors into UNIX errors.
	 */
	switch (error) {
	    case MIG_ARRAY_TOO_LARGE:
		error = ENAMETOOLONG;
		break;
	    case MACH_SEND_INVALID_DEST:
		/* Nobody cares */
		(void)	task_terminate(mach_task_self());
		break;
	}

	switch (error) {
	    case E_JUSTRETURN:
		/* Do not alter registers */
		break;

	    case 0:
		/* Success */
		regs2->eflags &= ~EFL_CF;
		regs2->eax = rval[0];
		regs2->edx = rval[1];
		break;

	    case ERESTART:
		/* restart call */
		regs2->eip -= 7;
		break;

	    default:
		/* error */
		regs2->eflags |= EFL_CF;
		regs2->eax = error;
		break;
	}

/*	if (error > ERELOCATED)
		e_emulator_error("emulator error: syscall=%d, error=0x%x",
				 syscode, error); */

#ifdef	MAP_UAREA
#ifdef MUDEBUG
EASSERT(shared_base_rw->us_in_emulator == 1 && 12);
#endif
	if (shared_enabled) {
		spin_lock(&in_emulator_lock);
		shared_base_rw->us_in_emulator--;

#ifdef MUDEBUG
EASSERT(0 == shared_base_rw->us_in_emulator && 2);      /* XXX */
#endif

		shared_base_rw->us_emul_regs = 0;
		spin_unlock(&in_emulator_lock);

		if (!interrupt &&
		    (shared_base_ro->us_cursig ||
		     (shared_base_rw->us_sig & ~(shared_base_rw->us_sigmask))))
			interrupt = TRUE;
	}
#endif	/* MAP_UAREA */

/*e_emulator_error("emulator OUT: syscall = %d", syscode);*/

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
	if (error == ERESTART || error == EINTR || error == EPIPE || interrupt)
	    take_signals(regs);
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
	struct emul_regs	*regs;
{
	struct execa		execa;

	execa.fname = argp->fname;
	execa.argp  = argp->argp;
	execa.envp  = (char **)0;

	return (e_execve(serv_port, interrupt, &execa, rval, regs));
}

int
e_execve(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register struct execa	*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2;
	int		entry[2];
	unsigned int	entry_count;
	vm_offset_t	arg_addr;
	register int	error;
	boolean_t       traced = FALSE;

	/*
	 * Do not have to save user registers on old stack;
	 * they will all be cleared.
	 */

	/*
	 * Call exec.  If error, return without changing registers.
	 */
	entry_count = 2;
	error = e_exec_call(serv_port,
			    interrupt,
			    argp->fname,
			    argp->argp,
			    argp->envp,
			    &arg_addr,
			    entry,
			    &entry_count,
			    &traced);
	if (error)
	    return (error);

	/*
	 * Put new user stack just below arguments.
	 */
	regs2 = ((struct emul_regs_2 *)arg_addr) - 1;

	regs2->eip = entry[0];
	regs2->eflags = EFL_USERSET;
	regs->uesp = regs2;

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
 * Take a signal.
 */
void
take_signals(regs)
	register struct emul_regs *regs;
{
	struct emul_regs_2	save_regs2;
	register struct emul_regs_2	*regs2;

	register struct sigcontext *scp;
	register struct sigframe {
	    int		(*sf_retadr)();
	    int		sf_signum;
	    int		sf_code;
	    struct sigcontext *sf_scp;
	    struct sigcontext *sf_scpcopy;	/* for return */
	} *fp;

	int	old_mask, old_onstack, sig, code, handler, new_sp, error;
	boolean_t	interrupt;

	/*
	 * Get anything valuable off user stack first.
	 */
	save_regs2 = *regs->uesp;

	/*
	 * Get the signal to take from the server.  It also
	 * switches the signal mask and the stack, so we must
	 * be off the old user stack before calling it.
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
	if (sig == 0 || error != ESUCCESS)
	    return;

	/*
	 * Put the signal context and signal frame on the signal stack.
	 */
	if (new_sp == 0) {
	    /*
	     * Build signal frame and context on user's stack.
	     */
	    new_sp = (int)regs->uesp;
	}

#if	nomore	/* XXX barbou@gr.osf.org */
	/* The following kludge detects when we're executing an old binary
	 * that was linked with the original signal implementation. Once the
	 * world is relinked this check and the routines osendsig() and
	 * osigreturn() should be removed and the library code in sigvec()
	 * should be fixed to not set the high order bit.
	 * -- lance & tommy 15-mar-89
	 */
	if (((unsigned long) sigreturnaddr & 0x80000000) == 0) {
		emul_panic("take_signals: obsolete user binary");
	}
#endif	/* nomore XXX barbou@gr.osf.org */

	scp = ((struct sigcontext *)new_sp) - 1;
	fp  = ((struct sigframe *)scp) - 1;

	if (!user_rwcheck(fp, new_sp - (int) fp)) {
		/*
		 * Process has trashed its stack;
		 * Simulate an illegal instruction abort.
		 */
		emul_blocking();			/*XXX*/
		(void) bsd_proc_exit(our_bsd_server_port,
				     &interrupt,
				     SIGILL,
				     TRUE);
		emul_unblocking();			/*XXX*/
		emul_panic("take_signals: bsd_proc_exit failed");
	}

	/*
	 * Build the argument list for the signal handler.
	 */
	fp->sf_signum = sig;
	fp->sf_code = code;
	fp->sf_scp = scp;

	/*
	 * Build the stack frame to be used to call sigreturn.
	 */
	fp->sf_scpcopy = scp;
#ifndef	TNC
	fp->sf_retadr = (int (*)())
		((unsigned long) sigreturnaddr & ~0x80000000);
#else	/* !TNC */
	{
		extern vm_offset_t emul_sigreturn;
		fp->sf_retadr = (int (*)())
			((sigreturnaddr == (vm_offset_t) &emul_sigreturn)
				? sigreturnaddr
				: (unsigned long) sigreturnaddr & ~0x80000000);
	}
#endif	/* !TNC */

	/*
	 * Build the signal context to be used by sigreturn.
	 */
	scp->sc_onstack = old_onstack;
	scp->sc_mask = old_mask;

/*	scp->sc_gs = ... */
/*	scp->sc_fs = ... */
/*	scp->sc_es = ... */
/*	scp->sc_ds = ... */

	scp->sc_edi = regs->edi;
	scp->sc_esi = regs->esi;
	scp->sc_ebp = regs->ebp;
	scp->sc_esp = 0;			/* kernel stack XXX */
	scp->sc_ebx = regs->ebx;
	scp->sc_edx = save_regs2.edx;
	scp->sc_ecx = save_regs2.ecx;
	scp->sc_eax = save_regs2.eax;
/*	scp->sc_trapno = ... */
	scp->sc_err = 0;
	scp->sc_eip = save_regs2.eip;
/*	scp->sc_cs = ... */
	scp->sc_efl = save_regs2.eflags;
	scp->sc_uesp = (int)(regs->uesp+1);	/* user sp after return */
/*	scp->sc_ss = ... */

	/*
	 * Set up the new stack and handler addresses.
	 */
	regs2 = ((struct emul_regs_2 *)fp) - 1;
	*regs2 = save_regs2;
	regs->uesp = regs2;
	regs2->eip = handler;
}

int
e_sigreturn(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	struct emul_regs_2		save_regs2;
	register struct emul_regs_2	*regs2;
	register int			rc;
	struct sigcontext		sc;
	struct a {
	    struct sigcontext *sigcp;
	} *uap = (struct a *)argp;

	/*
	 * Copy in signal context and regs2.
	 */
	save_regs2 = *regs->uesp;
	sc = *uap->sigcp;

	/*
	 * Change signal stack and mask.  If new signals are pending,
	 * do not take them until we switch user stack.
	 */
#ifdef	MAP_UAREA
    if (shared_enabled) {
	rc = e_shared_sigreturn(serv_port,
				interrupt,
				sc.sc_onstack & 01,
				sc.sc_mask);

    } else {
#endif	MAP_UAREA
	emul_blocking();
	rc = bsd_sigreturn(serv_port,
			interrupt,
			sc.sc_onstack & 01,
			sc.sc_mask);
	emul_unblocking();
#ifdef	MAP_UAREA
    }
#endif	MAP_UAREA

	/*
	 * Change registers.
	 */
	regs2 = ((struct emul_regs_2 *)sc.sc_uesp) - 1;

	*regs2 = save_regs2;
	regs->uesp = regs2;

/*	... = sc.sc_gs; */
/*	... = sc.sc_fs; */
/*	... = sc.sc_es; */
/*	... = sc.sc_ds; */
	regs->edi = sc.sc_edi;
	regs->esi = sc.sc_esi;
	regs->ebp = sc.sc_ebp;
	regs->ebx = sc.sc_ebx;
	regs2->edx = sc.sc_edx;
	regs2->ecx = sc.sc_ecx;
	regs2->eax = sc.sc_eax;
	regs2->eip = sc.sc_eip;
/*	... = sc.sc_cs; */
	regs2->eflags = (sc.sc_efl & ~EFL_USERCLR) | EFL_USERSET;
/*	... = sc.sc_ss; */

	return (E_JUSTRETURN);
}

/*
 * Compatibility with 4.2 chmk $139 used by longjmp()
 */
e_osigcleanup()
{
}

#ifdef	COMPAT_43
/*
 * Sigvec hides the signal trampoline address in a VERY
 * strange place.
 */
int
i386_osigvec(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register int		*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register int	rc;
	register vm_offset_t	sigtramp;

	sigtramp = regs->uesp->edx;	/* (!) */

	rc = e_osigvec(serv_port,
			interrupt,
			argp[0],			/* signo */
			(struct sigvec *)argp[1],	/* nsv */
			(struct sigvec *)argp[2],	/* osv */
			sigtramp);

	if (rc == 0)
	    sigreturnaddr = sigtramp;

	return (rc);
}
#endif	COMPAT_43

int
i386_signal(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register int		*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register int	rc;
	register vm_offset_t	sigtramp;

	sigtramp = regs->uesp->edx;	/* (!) */

	rc = e_signal(serv_port,
		     interrupt,
		     argp[0],			/* signo */
		     (int (*)())argp[1],	/* handler */
		     sigtramp,
		     rval);

	if (rc == 0)
		sigreturnaddr = sigtramp;

	return (rc);
}

int
i386_sigaction(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register int		*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register int	rc;
	register vm_offset_t	sigtramp;	/* sigreturn() in fact */

	sigtramp = regs->uesp->edx;	/* (!) */

	rc = e_sigaction(serv_port,
			interrupt,
			argp[0],			/* signo */
			(struct sigaction *)argp[1],	/* nsv */
			(struct sigaction *)argp[2],	/* osv */
			sigtramp);

	if (rc == 0)
	    sigreturnaddr = sigtramp;

	return (rc);
}

/*
 * Wait has a weird parameter passing mechanism.
 */
int
e_owait(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->uesp;

	int	new_args[4];		/* POSSIBLE HACK (jqr) */

	if ((regs2->eflags & EFL_ALLCC) == EFL_ALLCC) {
	    new_args[2] = regs2->ecx;	/* options */
	    new_args[3] = regs2->edx;	/* rusage_p */
	}
	else {
	    new_args[2] = 0;
	    new_args[3] = 0;
	}
	return (emul_vproc_generic(serv_port, interrupt,
			SYS_wait, &new_args[0], rval));
}

int
e_fork(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->uesp;
	register int error;

	struct i386_thread_state	child_regs;
	int				seg_regs[6];
	
	extern int	child_fork();
	extern void	get_seg_regs();

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
	get_seg_regs(seg_regs);

	child_regs.gs = seg_regs[5];
	child_regs.fs = seg_regs[4];
	child_regs.es = seg_regs[3];
	child_regs.ds = seg_regs[2];
	child_regs.edi = regs->edi;
	child_regs.esi = regs->esi;
	child_regs.ebp = regs->ebp;
     /*	child_regs.esp */
	child_regs.ebx = regs->ebx;
	child_regs.edx = regs2->edx;
	child_regs.ecx = regs2->ecx;
	child_regs.eax = regs2->eax;
	child_regs.eip = (int)child_fork;
	child_regs.cs = seg_regs[0];
	child_regs.efl = regs2->eflags;
	child_regs.uesp = (int)regs->uesp;
	child_regs.ss = seg_regs[1];
	
	/* FP regs!!!! */
	/* 
	 * OSF1 does not inheritate fp regs on fork. Only the csw
	 * is initialized on first fp op with a default OSF/1 kernel
	 * value. Unfortunately the OSF/1 default value is not the micro
	 * kernel one. So the csw is initialized in child_fork()
	 */

	return(e_fork_call(serv_port,
			   interrupt,
			   NONODE,
			   (thread_state_t)&child_regs,
			   i386_THREAD_STATE_COUNT,
			   rval));
}

#ifdef	TNC

int
e_rfork(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->uesp;
	register int error;

	struct i386_thread_state	child_regs;
	int				seg_regs[6];

	extern int	child_fork();
	extern void	get_seg_regs();

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
	get_seg_regs(seg_regs);

	child_regs.gs = seg_regs[5];
	child_regs.fs = seg_regs[4];
	child_regs.es = seg_regs[3];
	child_regs.ds = seg_regs[2];
	child_regs.edi = regs->edi;
	child_regs.esi = regs->esi;
	child_regs.ebp = regs->ebp;
     /*	child_regs.esp */
	child_regs.ebx = regs->ebx;
	child_regs.edx = regs2->edx;
	child_regs.ecx = regs2->ecx;
	child_regs.eax = regs2->eax;
	child_regs.eip = (int)child_fork;	/* this is special */
	child_regs.cs = seg_regs[0];
	child_regs.efl = regs2->eflags;
	child_regs.uesp = (int)regs->uesp;
	child_regs.ss = seg_regs[1];

	/* FP regs!!!! */
	/* 
	 * OSF1 does not inheritate fp regs on fork. Only the csw
	 * is initialized on first fp op with a default OSF/1 kernel
	 * value. Unfortunately the OSF/1 default value is not the micro
	 * kernel one. So the csw is initialized in child_fork()
	 */

	return(e_rfork_call(serv_port, 
			    interrupt, 
			    argp[0],		/* argp[0] is the new node */
			    (thread_state_t)&child_regs, 
			    i386_THREAD_STATE_COUNT, 
			    rval));
}

int
e_migrate(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->uesp;

	struct i386_thread_state	child_regs;
	int				seg_regs[6];

	extern int	migrate_arrival();
	extern void	get_seg_regs();

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
	get_seg_regs(seg_regs);

	child_regs.gs = seg_regs[5];
	child_regs.fs = seg_regs[4];
	child_regs.es = seg_regs[3];
	child_regs.ds = seg_regs[2];
	child_regs.edi = regs->edi;
	child_regs.esi = regs->esi;
	child_regs.ebp = regs->ebp;
     /*	child_regs.esp */
	child_regs.ebx = regs->ebx;
	child_regs.edx = regs2->edx;
	child_regs.ecx = regs2->ecx;
	child_regs.eax = regs2->eax;
	child_regs.cs = seg_regs[0];
	child_regs.efl = regs2->eflags;
	child_regs.uesp = (int)regs->uesp;
	child_regs.ss = seg_regs[1];

	/* FP regs!!!! */

	if (argp[0] >= 0) {     /* migrate - argp[0] is the new node */
		child_regs.eip = (int)migrate_arrival;  /* this is special */
		return( e_migrate_call(serv_port, 
			       interrupt, 
			       argp[0],		/* argp[0] is the new node */
			       (thread_state_t)&child_regs, 
			       i386_THREAD_STATE_COUNT));
	} else {
#ifdef  CHKPNT
		child_regs.eip = regs2->eip;
		/*
		 * Prepare the user stack to return with a value
		 * of abs(checkpoint arg) on restart.
		 */
		regs2->eflags &= ~EFL_CF;
		regs2->eax = -argp[0];
		return( emul_chkpnt_self(serv_port,
					interrupt,
					argp[0],
					(thread_state_t)&child_regs,
					i386_THREAD_STATE_COUNT,
					(caddr_t) sigreturnaddr));
#else   /* CHKPNT */
                return(ENOSYS);
#endif  /* CHKPNT */
	}
}

#ifdef  CHKPNT
struct execra {
	char	*chkpnt_prefix;
	int	options;
};

int
e_forkfamily(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->uesp;
	register int error;

	struct i386_thread_state	child_regs;
	int				seg_regs[6];

	extern int	child_fork();
	extern void	get_seg_regs();

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
	get_seg_regs(seg_regs);

	child_regs.gs = seg_regs[5];
	child_regs.fs = seg_regs[4];
	child_regs.es = seg_regs[3];
	child_regs.ds = seg_regs[2];
	child_regs.edi = regs->edi;
	child_regs.esi = regs->esi;
	child_regs.ebp = regs->ebp;
     /*	child_regs.esp */
	child_regs.ebx = regs->ebx;
	child_regs.edx = regs2->edx;
	child_regs.ecx = regs2->ecx;
	child_regs.eax = regs2->eax;
	child_regs.eip = (int)child_fork;	/* this is special */
	child_regs.cs = seg_regs[0];
	child_regs.efl = regs2->eflags;
	child_regs.uesp = (int)regs->uesp;
	child_regs.ss = seg_regs[1];

	/* FP regs!!!! */
	/* 
	 * OSF1 does not inheritate fp regs on fork. Only the csw
	 * is initialized on first fp op with a default OSF/1 kernel
	 * value. Unfortunately the OSF/1 default value is not the micro
	 * kernel one. So the csw is initialized in child_fork()
	 */

	return(e_forkfamily_call(serv_port, 
				 interrupt, 
				 argp[0],	/* nproc */
				 argp[1],	/* pid_array */
				 (thread_state_t)&child_regs, 
				 i386_THREAD_STATE_COUNT, 
				 rval));
}

int
e_exec_restart(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register struct execra	*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	struct i386_thread_state	chkpnt_state;
	unsigned int			chkpnt_state_count
						= i386_THREAD_STATE_COUNT;
	int				chkpnt_seg_regs[6];
	register int			error;
	boolean_t      			traced = FALSE;
	boolean_t      			stop = FALSE;

	/*
	 * Do not have to save user registers on old stack;
	 * they will all be cleared.
	 */

	/*
	 * Call exec_restart.  If error, return without changing registers.
	 */
	error = e_exec_restart_call(serv_port,
				    interrupt,
				    argp->chkpnt_prefix,
				    argp->options,
				    &chkpnt_state,
				    &chkpnt_state_count,
				    &stop,
				    &traced,
				    &sigreturnaddr);
	if (error)
	    return (error);

	/*
	 * Restore registers to state at the of checkpoint.
	 */
/*
	chkpnt_seg_regs[5] = chkpnt_state.gs;
	chkpnt_seg_regs[4] = chkpnt_state.fs;
	chkpnt_seg_regs[3] = chkpnt_state.es;
	chkpnt_seg_regs[2] = chkpnt_state.ds;
	chkpnt_seg_regs[1] = chkpnt_state.ss;
	chkpnt_seg_regs[0] = chkpnt_state.cs;
*/
	regs->edi = chkpnt_state.edi;
	regs->esi = chkpnt_state.esi;
	regs->ebp = chkpnt_state.ebp;
	regs->ebx = chkpnt_state.ebx;

	(int)regs->uesp = chkpnt_state.uesp;

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
	 * Return to new stack (with return code set by e_migrate() and thus
	 * seen as the return from the migrate() invoking the checkpoint).
	 */
	return (E_JUSTRETURN);
}
#endif	CHKPNT


int
e_rexecve(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register struct rexeca	*argp;		/* that's rexeca */
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->uesp;
	struct i386_thread_state	child_regs;
	int				seg_regs[6];

	extern int	rexecve_arrival();
	extern void	get_seg_regs();
	extern node_t	emul_tnc_mynode();

	/*
	 * If we're already on the requested node, do a plain execve
	 * instead.
	 */
	if (argp->node == emul_tnc_mynode()) {
		return(e_execve(serv_port, interrupt, argp, rval, regs));
	}

	/*
	 * Set up the registers we'll have when we transition to the
	 * other node.  More registers, in fact, than we really need.
	 */
	get_seg_regs(seg_regs);

	child_regs.gs = seg_regs[5];
	child_regs.fs = seg_regs[4];
	child_regs.es = seg_regs[3];
	child_regs.ds = seg_regs[2];
	child_regs.edi = regs->edi;
	child_regs.esi = regs->esi;
	child_regs.ebp = regs->ebp;
     /*	child_regs.esp */
	child_regs.ebx = regs->ebx;
	child_regs.edx = regs2->edx;
	child_regs.ecx = regs2->ecx;
	child_regs.eax = regs2->eax;
	child_regs.eip = (int)rexecve_arrival;	/* this is special */
	child_regs.cs = seg_regs[0];
	child_regs.efl = regs2->eflags;
	child_regs.uesp = (int)regs->uesp;
	child_regs.ss = seg_regs[1];

	/* No FP regs needed for exec */

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
			      i386_THREAD_STATE_COUNT));
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
	struct emul_regs	*regs;
{
	vm_offset_t	arg_addr;
	int		entry[16];
	int		entry_count = 16;
	boolean_t	traced = FALSE;
	register struct emul_regs_2 *regs2;

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
	regs2 = ((struct emul_regs_2 *)arg_addr) - 1;

	regs2->eip = entry[0];
	regs2->eflags = EFL_USERSET;
	regs->uesp = regs2;

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
 * If, however, the code is negative, a checkpoint operation is
 * carried out.
 */
void
emul_sigmigrate_handler(int sig, int code)
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
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->uesp;
	register int error;

	struct i386_thread_state	child_regs;
	int				seg_regs[6];

	extern int	child_fork();
	extern void	get_seg_regs();

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
	get_seg_regs(seg_regs);

	child_regs.gs = seg_regs[5];
	child_regs.fs = seg_regs[4];
	child_regs.es = seg_regs[3];
	child_regs.ds = seg_regs[2];
	child_regs.edi = regs->edi;
	child_regs.esi = regs->esi;
	child_regs.ebp = regs->ebp;
     /*	child_regs.esp */
	child_regs.ebx = regs->ebx;
	child_regs.edx = regs2->edx;
	child_regs.ecx = regs2->ecx;
	child_regs.eax = regs2->eax;
	child_regs.eip = (int)child_fork;	/* this is special */
	child_regs.cs = seg_regs[0];
	child_regs.efl = regs2->eflags;
	child_regs.uesp = (int)regs->uesp;
	child_regs.ss = seg_regs[1];

	/* FP regs!!!! */
	/* 
	 * OSF1 does not inheritate fp regs on fork. Only the csw
	 * is initialized on first fp op with a default OSF/1 kernel
	 * value. Unfortunately the OSF/1 default value is not the micro
	 * kernel one. So the csw is initialized in child_fork()
	 */

	return(e_rforkmulti_call(serv_port, 
				 interrupt, 
				 argp[0],	/* count */
				 argp[1],	/* node_array */
				 argp[2],	/* rval_array */
				 argp[3],	/* pid_array */
				 (thread_state_t)&child_regs, 
				 i386_THREAD_STATE_COUNT, 
				 rval));
}

#else	/* TNC */
int
e_rfork(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->uesp;
	register int error;

	struct i386_thread_state	child_regs;
	int				seg_regs[6];

	extern int	child_fork();
	extern void	get_seg_regs();

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
	get_seg_regs(seg_regs);

	child_regs.gs = seg_regs[5];
	child_regs.fs = seg_regs[4];
	child_regs.es = seg_regs[3];
	child_regs.ds = seg_regs[2];
	child_regs.edi = regs->edi;
	child_regs.esi = regs->esi;
	child_regs.ebp = regs->ebp;
     /*	child_regs.esp */
	child_regs.ebx = regs->ebx;
	child_regs.edx = regs2->edx;
	child_regs.ecx = regs2->ecx;
	child_regs.eax = regs2->eax;
	child_regs.eip = (int)child_fork;	/* this is special */
	child_regs.cs = seg_regs[0];
	child_regs.efl = regs2->eflags;
	child_regs.uesp = (int)regs->uesp;
	child_regs.ss = seg_regs[1];

	/* FP regs!!!! */
	/* 
	 * OSF1 does not inheritate fp regs on fork. Only the csw
	 * is initialized on first fp op with a default OSF/1 kernel
	 * value. Unfortunately the OSF/1 default value is not the micro
	 * kernel one. So the csw is initialized in child_fork()
	 */

	return(e_fork_call(serv_port, 
			    interrupt, 
			    argp[0],		/* argp[0] is the new node */
			    (thread_state_t)&child_regs, 
			    i386_THREAD_STATE_COUNT, 
			    rval));
}
#endif	/* TNC */

vm_offset_t
set_arg_addr(arg_size)
	vm_size_t	arg_size;
{
	/*
	 * Round arg size to fullwords
	 */
	arg_size = (arg_size + NBPW-1) & ~(NBPW - 1);

	/*
	 * Put argument list at top of stack.
	 */
	return (USRSTACK - arg_size);
}

kern_return_t
emul_thread_set_state(th, stack, start)
	thread_t	th;
	unsigned int	stack, start;
{
	struct i386_thread_state ts;
	unsigned int	count = i386_THREAD_STATE_COUNT;
	kern_return_t	rc;


	rc = thread_get_state(th, i386_THREAD_STATE, (thread_state_t)&ts,
				&count);
	if (rc != KERN_SUCCESS)
		return rc;
	if (count != i386_THREAD_STATE_COUNT)
		emul_panic("emul_thread_set_state: bad count");

	ts.ebp = 0;				/* clear frame ptr */
	ts.uesp = stack;
	ts.eip = start;

	rc = thread_set_state(th, i386_THREAD_STATE, (thread_state_t)&ts,
				i386_THREAD_STATE_COUNT);
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
	struct i386_thread_state ts;
	unsigned int	count = i386_THREAD_STATE_COUNT;
	kern_return_t	rc;

	if (tsp == NULL)
		tsp = (thread_state_t)&ts;

	rc = thread_get_state(th, i386_THREAD_STATE, tsp, &count);
	if (rc != KERN_SUCCESS) {
		if (pc)
			*pc = 0;
		if (id)
			*id = 0;
		return rc;
	}
	if (count != i386_THREAD_STATE_COUNT)
		emul_panic("emul_thread_set_state: bad count");

	if (pc)
		*pc = ((struct i386_thread_state *)tsp)->eip;
	if (id)
		*id = (emul_thread_id)EMUL_STACK_BASE(
				((struct i386_thread_state *)tsp)->uesp);

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
				((struct i386_thread_state *)tsp)->uesp);
	} else {
		rc = emul_thread_get_state(th, tsp, (unsigned int *)NULL, &id);
		if (rc == KERN_SUCCESS)
			return id;
		else
			return 0;
	}
}
