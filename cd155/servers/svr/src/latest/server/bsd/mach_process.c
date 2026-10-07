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
 * $Log: mach_process.c,v $
 * Revision 1.9  1995/03/27  19:28:35  nandy
 * n pproc_ptrace(), on resme, change the physical and the vproc states together.
 *
 *  Reviewer: BOB YAZZI
 *  Risk: L
 *  Benefit or PTS #: 12515
 *  Testing: PTS test case
 *  Module(s): bsd/mach_process.c
 *
 * Revision 1.8  1994/11/18  20:27:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/08/02  22:15:15  nandy
 * Don't mark the process "unreaped" in ptrace.
 *
 * Revision 1.6  1993/07/14  17:48:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:48:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:04:29  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:24:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:03:10  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/27  17:00:52  cfj
 * In pproc_ptrace() mark the process as VPROC_GANG_UNSTOP before resuming the task.
 *
 * Revision 1.2  1992/11/30  22:16:25  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:41  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:07:44  cfj
 * Bump major revision number.
 *
 * Revision 2.11  1992/09/28  16:33:36  roman
 * Change VPOP_SET_STATE() to VPOP_REPORT_STATE() (more modern name).
 *
 * Revision 2.10  92/07/07  15:09:42  roman
 * Change unix_master() and unix_release() during pproc operations to invoke
 * 	TNC_unix_master() and TNC_unix_release() instead.
 * 
 * Revision 2.9  92/07/06  09:01:52  chrisp
 * To avoid races, update state in proc. structure before invoking
 * 	VPOP_SET_STATE.
 * 
 * Revision 2.8  92/06/16  10:32:32  chrisp
 * [Bug #25] ptrace(PT_CONTINUE) corrected to call VPOP_SET_STATE(..,VPROC_UNSTOP)
 * 	to inform the vproc layer.
 * 
 * Revision 2.7  92/04/06  15:52:51  chrisp
 * Add master locking inside pproc_ptrace().
 * 
 * Revision 2.6  92/02/17  14:37:27  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.5  92/02/11  22:17:48  pjg
 * 	Introduce pproc_ptrace() to be called by the vproc layer in 
 * 	support of the ptrace() system call. ptrace() is now moved 
 * 	into vproc/vp_syscalls.c (chrisp@locus).
 * 
 * Revision 2.4  91/11/13  12:51:53  rabii
 * 	i860 support
 * 
 * Revision 2.3  91/10/17  09:51:02  sjs
 * fixed rcs comments
 * 
 * Revision 2.2  91/10/14  12:08:59  sjs
 * Check in
 * 
 * Revision 3.0  91/10/03  10:27:17  jose
 * Adapted from 1.0.2 to OSF/1 server
 * 
 * Revision 1.6.6.2  91/03/11  16:12:03  jph
 * 	Clear SWTED flag on all ptrace() calls.
 * 	[91/03/11  16:05:23  jph]
 * 
 * Revision 1.6  90/10/07  13:18:29  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:51:39  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:58:11  gm]
 * 
 * 	Interrupt target thread when delivering a signal.  Bug found by
 * 	af@cs.cmu.edu, fix by af and dlb.
 * 	[90/09/28  12:47:27  dlb]
 * 
 * Revision 1.5  90/08/24  11:18:00  devrcs
 * 	Changed ptrace to use new syscall interface.  Also changed set_ptrace_u
 * 	interface to take a return value pointer instead of u.u_rval1.
 * 	[90/08/17  17:38:44  nags]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:35:55  mcg]
 * 
 * Revision 1.4  90/02/05  15:46:58  robert
 * 	Removed include of <sys/dir.h>
 * 	[90/01/18  19:13:44  gmf]
 * 
 * Revision 1.3  90/01/03  11:50:39  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:26:30  gm]
 * 
 * Revision 1.2  89/10/26  07:47:06  gm
 * 	MACH X115 Update
 * 
 * Revision 6.1  89/07/26  15:30:22  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.13  89/10/19  13:14:02  af
 * 	Fixed Mips case that got botched in previous merge.
 * 	Also, added a 'flush' argument to the ptrace_write_data()
 * 	function, in anticipation of other architectures (besides MIPS)
 * 	which will need to flush the Icache in ptrace.
 * 	[89/10/14            af]
 * 
 * Revision 2.12  89/10/10  10:47:04  mwyoung
 * 	Separate out the routines to read/write user address spaces
 * 	so that they may be used in architecture-specific ptrace modules.
 * 	[89/10/07            mwyoung]
 * 	Use new vm_map_copy technology.
 * 	[89/10/07  19:13:32  mwyoung]
 * 
 * Revision 2.11  89/08/08  21:45:58  jsb
 * 	Instruction cache was not flushed properly on mips
 * 	after setting a breakpoint.
 * 	[89/07/28            af]
 * 
 * Revision 2.10  89/05/31  12:27:17  rvb
 * 	Remove cachectl.h and inline it. [af]
 * 
 * Revision 2.9  89/05/30  10:34:59  rvb
 * 	Made it right for mips.
 * 	Removed inclusion of psl.h for ibmrt and mips.
 * 	Cleaned up a little the ibmrt/else case for reading/writing
 * 	the u-area.
 * 	Other minor cleanups.
 * 	[89/04/03            af]
 * 
 * Revision 2.8  89/03/09  19:29:34  rpd
 * 	More cleanup.
 * 
 * Revision 2.7  89/02/25  14:42:52  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.6  89/02/09  04:31:54  mwyoung
 * 	Code cleanup cataclysm.
 * 
 * Revision 2.5  89/01/23  22:04:58  af
 * 	Changes for I386: cut in 386 processor registers
 * 	[89/01/09            rvb]
 * 
 * 	Started fixes for Mips: pulled over code, most of which
 * 	is disabled.
 * 	[89/01/05            af]
 * 
 * Revision 2.4  88/10/18  03:15:08  mwyoung
 * 	Watch out for zero return value from kmem_alloc_wait.
 * 	[88/09/13            mwyoung]
 * 
 * Revision 0.0  88/05/04            dlb
 * 	Make sure p_stat is SSTOP in ptrace().
 * 	[88/05/04            dlb]
 * 
 * Revision 0.0  88/03/13            dbg
 * 	Use vm_map_copy instead of playing with physical pages.
 * 	[88/03/13            dbg]
 * 
 * Revision 0.0  88/03/03            mwyoung
 * 	De-linted.
 * 	[88/03/03            mwyoung]
 * 
 * Revision 0.0  87/10/13            dlb
 * 	run_state --> user_stop_count.
 * 	[87/10/13            dlb]
 * 
 * Revision 0.0  87/07/24            dlb
 * 	Set modified bit on any pages modified by copy_to_phys.
 * 	[87/07/24            dlb]
 * 
 * Revision 0.0  87/07/13            dlb
 * 	If delivering a thread signal, set thread's u.u_cursig.
 * 	Optimize and clean up register references.
 * 	[87/07/13            dlb]
 * 
 * Revision 0.0  87/07/02            dlb
 * 	Derived from sys_process.c via major rewrite to eliminate
 * 	ipc structure and procxmt.
 * 	[87/07/02            dlb]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)sys_process.c	7.1 (Berkeley) 6/5/86
 */

#include <machine/reg.h>
#ifdef	ibmrt
#include <ca/scr.h>
#include <mach/ca/vm_param.h>		/* for KERNEL_STACK_SIZE */
#endif	ibmrt
#if	!defined(ibmrt) && !defined(mips)
#include <machine/psl.h>
#endif	!defined(ibmrt) && !defined(mips)

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/ptrace.h>
#include <sys/vproc.h>

#include <kern/sched_prim.h>
#ifndef	OSF1_SERVER
#include <kern/task.h>
#include <kern/thread.h>

#include <vm/vm_map.h>
#include <mach/vm_param.h>
#include <vm/vm_kern.h>
#else	/* OSF1_SERVER */
#include <kern/parallel.h>
#endif	/* OSF1_SERVER */
#include <mach/vm_prot.h>

#ifdef	sun
/* Use Sun version of ptrace */
#else	sun

#ifdef	vax
#define NIPCREG 16
int ipcreg[NIPCREG] =
	{R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,AP,FP,SP,PC};
#define	PROGRAM_COUNTER		PC
#define	PROCESSOR_STATUS_WORD	PS
#endif	vax

#ifndef	OSF1_SERVER
#ifdef	i386
#define NIPCREG 16
int ipcreg[NIPCREG] =
	{EAX,EBX,ECX,EDX,ESI,EDI,EBP,EIP,ESP};
#define	PROGRAM_COUNTER		EIP
#define	PROCESSOR_STATUS_WORD	EFL
#define	PSL_USERSET		EFL_USERSET
#define	PSL_USERCLR		EFL_USERCLR
#define	PSL_T			EFL_TF
#endif	i386
#endif	/* OSF1_SERVER */

#if	defined(mc68000) || defined(__mc68000)
#define NIPCREG 17
int ipcreg[NIPCREG] =
        {R0,R1,R2,R3,R4,R5,R6,R7,AR0,AR1,AR2,AR3,AR4,AR5,AR6,AR7,PC};
#define	PROGRAM_COUNTER		PC
#define	PROCESSOR_STATUS_WORD	PS
#endif	mc68000

#ifdef	ibmrt
#define NIPCREG 18	 /* allow modification of only these u area variables */
int ipcreg[NIPCREG] =
	{R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,IAR,MQ};
#define	PROGRAM_COUNTER		IAR
#define	PROCESSOR_STATUS_WORD	ICSCS
#define	PSL_USERSET		ICSCS_USERSET
#define	PSL_USERCLR		ICSCS_USERCLR
#define	PSL_T			ICSCS_INSTSTEP
#endif	ibmrt

#ifdef	balance
int	ipcreg[] = {R0,R1,R2,R3,R4,R5,R6,R7,FP,SP,PC};
#define NIPCREG	(sizeof(ipcreg)/sizeof(ipcreg[0]))
#define	PROGRAM_COUNTER		PC
#define	PROCESSOR_STATUS_WORD	PS
#endif	balance

#ifdef	multimax
/* 
 *	Multimax does this differently; see mmax/mmax_ptrace.c
 *	and mmax/mmax_ptrace.h for details.
 */
#define	get_ptrace_u mmax_get_ptrace_u
#define set_ptrace_u mmax_set_ptrace_u
#define	PROGRAM_COUNTER		PC
#define	PROCESSOR_STATUS_WORD	PS
#endif	multimax

#ifdef	mips
#define	PROGRAM_COUNTER		EF_EPC
#endif	mips

#define ALIGNED(addr,size)	(((unsigned)(addr)&((size)-1))==0)

/*
 * physical sys-trace system call.
 */
int
pproc_ptrace(p, req, addr, data, retval)
	struct proc	*p;
	int		req;
	int		*addr;
	int		data;
	int		*retval;
{
	int		*locr0;
	int		error = 0;

	TNC_unix_master();

	/*
	 *	Intercept and deal with "please trace me" request.
	 */
	if (req <= PT_TRACE_ME) {
		p->p_flag |= STRC;
		TNC_unix_release();
		return (0);
	}

	/*
	 *	Make sure it is traceable.
	 */
	if (p->p_stat != SSTOP || !(p->p_flag & STRC)) {
		TNC_unix_release();
		return (ESRCH);
	}

#ifdef	OSF1_SERVER
	/*
	 *	If the user thread to trace has not already been set.
	 *	pick one at random.  (We never said that ptrace really
	 *	worked on multi-threaded programs.)
	 */
	while (!MACH_PORT_VALID(p->p_thread)) {
	    thread_array_t	thread_list;
	    unsigned int	i, count;

	    if (task_threads(p->p_task, &thread_list, &count)
		    != KERN_SUCCESS) {
		TNC_unix_release();
		return (EIO);
	    }

	    /*
	     * Use the first thread.
	     */
	    p->p_thread = thread_list[0];

	    /*
	     * Deallocate the rest, and deallocate the memory.
	     */

	    for (i = 1; i < count; i++)
		if (MACH_PORT_VALID(thread_list[i]))
		    (void) mach_port_deallocate(mach_task_self(),
						thread_list[i]);

	    (void) vm_deallocate(mach_task_self(),
				 (vm_offset_t)thread_list,
				 (vm_size_t) (count * sizeof(thread_list[0])));
	}
#endif	OSF1_SERVER

	/*
	 *	Mach version of ptrace executes request directly here,
	 *	thus simplifying the interaction of ptrace and signals.
	 */

	/* reset flag for waiters */
	/*
	p->p_flag &= ~SWTED;
	*/

	switch (req) {

	case PT_READ_I:
	case PT_READ_D:
		error = ptrace_read_data(p,
				(vm_offset_t)addr,
				sizeof(int),
				(caddr_t)retval);
		break;



	case PT_WRITE_I:
	case PT_WRITE_D:
		error = ptrace_write_data(p,
				(vm_offset_t)addr,
				sizeof(int),
				(caddr_t)&data,
				(caddr_t)retval,
				(req == PT_WRITE_I));
		break;

	case PT_READ_U:
		/*
		 *	Read victim's U-area or registers.
		 *	Offsets are into BSD kernel stack, and must
		 *	be faked to match MACH.
		 */
#if	defined(multimax) || defined(mips) || defined(i386) || defined(i860)
#if	defined(i386) || defined(i860)	/* OSF1_SERVER */
		if (ptrace_get_u_word(p, addr, retval) != 0)
#else	/* defined(i386) || defined(i860) */
		if (get_ptrace_u(addr, retval, p->thread) != 0)
#endif	/* defined(i386) || defined(i860) */
			goto errout;
		break;
#else	/* defined(multimax) || defined(mips) || defined(i386)||defined(i860) */
	    {
		register int	i, off;
		struct user	fake_uarea;

		i = (int)addr;

		if (i < 0 || i >= ctob(UPAGES))
			goto errout;

#ifdef	ibmrt
		if (!ALIGNED(i, sizeof(int)))
			goto errout;
		/*
		 *	U-area and kernel stack are swapped
		 */
		i -= (ctob(UPAGES) - sizeof(struct user));
		off = i + ctob(UPAGES);
		if (i >= 0) {
#else	ibmrt
		off = i;
		if (i < sizeof(struct user)) {
#endif	ibmrt
		    /*
		     *	We want data from the U area.  Fake it up,
		     *	then pull out the desired int.
		     */
		    bzero((caddr_t)&fake_uarea, sizeof(struct user));
		    fake_u(&fake_uarea, p->thread);
		    *retval = *(int *)(((caddr_t)&fake_uarea) + i);
		}
		else {
		    /*
		     *	Assume we want data from the kernel stack, most
		     *	likely the user's registers.
		     *
		     */
		    *retval = *(int *)(
			((caddr_t)p->thread->kernel_stack)
			+ (KERNEL_STACK_SIZE - ctob(UPAGES))
			+ off);
		}
		break;
	    }
#endif	/* defined(multimax) || defined(mips) || defined(i386)||defined(i860) */

	case PT_WRITE_U:
		/*
		 *	Write victim's registers.
		 *	Offsets are into BSD kernel stack, and must
		 *	be faked to match MACH.
		 */
#if	defined(multimax) || defined(mips) || defined(i386) || defined(i860)
#if	defined(i386) || defined(i860)	/* OSF1_SERVER */
		if (ptrace_set_u_word(p, addr, data, retval) != 0)
#else	/* defined(i386) || defined(i860) */
		if (set_ptrace_u(addr, data, p->thread, retval) != 0)
#endif	/* defined(i386) || defined(i860) */
			goto errout;
		break;
#else	/* defined(multimax)||defined(mips)||defined(i386)||defined(i860) */
	    {
		register int	i, off;
		register int	*reg_addr;

		i = (int)addr;
#ifdef	ibmrt
		i -= (ctob(UPAGES) - sizeof(struct user));
		off = i - ctob(UPAGES);
#else	ibmrt
		off = i;
#endif	ibmrt
		/*
		 *	Write one of the user's registers.
		 *	Convert the offset (in old-style Uarea/kernel stack)
		 *	into the corresponding offset into the saved
		 *	register set.
		 */
		reg_addr = (int *)(((caddr_t)p->thread->kernel_stack)
				+ (KERNEL_STACK_SIZE - ctob(UPAGES))
				+ off);

		locr0 = p->thread->u_address.uthread->uu_ar0;

		for (i = 0; i < NIPCREG; i++)
			if (reg_addr == &locr0[ipcreg[i]])
				goto ok;

		if (reg_addr == &locr0[PROCESSOR_STATUS_WORD]) {
			data = (data | PSL_USERSET) & ~PSL_USERCLR;
#ifdef	vax
			/* special problems with compatibility mode */
			if (data & PSL_CM)
			    data &= ~(PSL_FPD|PSL_DV|PSL_FU|PSL_IV);
#endif	vax
		} else {
		    goto errout;
		}

	ok:
		*reg_addr = data;
		break;
	    }
#endif	/* defined(multimax) || defined(mips) || defined(i386)||defined(i860) */

	case PT_KILL:
		/*
		 *	Force process to exit.
		 */
		proc_exit(p, p->p_cursig, FALSE);
		break;

#ifdef	OSF1_SERVER
	case PT_STEP:
		/*
		 *	Single step the child.
		 */
		if ((unsigned)data > NSIG) {
			goto errout;
		}
		ptrace_set_trace(p, (int)addr, TRUE);
		p->p_cursig = data;	/* see issig */
		goto resume;

	case PT_CONTINUE:
		/*
		 *	Continue the child.
		 */
		if ((unsigned)data > NSIG) {
			goto errout;
		}
		ptrace_set_trace(p, (int)addr, FALSE);
		p->p_cursig = data;	/* see issig */

	resume:
		(void) task_resume(p->p_task);
		unsleep_proc(p, FALSE);		/* wakeup uninterruptible threads */
		p->p_stat = SRUN;
		p->p_stopsig = 0;
	
#ifdef NX
                        (void) VPOP_REPORT_STATE(p->p_vproc,
                                                 VPROC_GANG_UNSTOP);
#endif /* NX */

		(void) VPOP_REPORT_STATE(p->p_vproc, VPROC_UNSTOP);
		break;

#else	/* OSF1_SERVER */
	case PT_STEP:			/* single step the child */
	case PT_CONTINUE:		/* continue the child */
		locr0 = p->thread->u_address.uthread->uu_ar0;
		if ((int)addr != 1) {
#ifdef	mips
			if (!ALIGNED(addr, sizeof(int)))
				goto errout;
#endif	mips
			locr0[PROGRAM_COUNTER] = (int)addr;
		}

		if ((unsigned)data > NSIG)
			goto errout;

		if (sigmask(p->p_cursig) & threadmask)
		   p->thread->u_address.uthread->uu_cursig = 0;
		p->p_cursig = data;	/* see issig */
		if (sigmask(data) & threadmask)
		    p->thread->u_address.uthread->uu_cursig = data;

#ifdef	mips
		p->thread->pcb->pcb_sstep = req;
#else	mips
		if (req == PT_STEP) 
			locr0[PROCESSOR_STATUS_WORD] |= PSL_T;
#endif	mips

	resume:
		p->p_stat = SRUN;
		if (p->thread && p->p_cursig)
			clear_wait(p->thread, THREAD_INTERRUPTED, TRUE);
		task_resume(p->task);
		break;

#endif	/* OSF1_SERVER */
		
	default:
	errout:
		error = EIO;
	}
	TNC_unix_release();
	return (error);
}
#endif	sun

/*
 *	Convenient routines to read/write process address spaces
 *
 *	XXX If anyone feels like doing some public service,
 *	XXX these routines should be reimplemented using only
 *	XXX the exported Mach interface calls.
 */

int ptrace_read_data(p, address, amount, resulting_data)
	struct proc	*p;
	vm_offset_t	address;
	vm_size_t	amount;
	caddr_t		resulting_data;
{
	vm_offset_t	start_addr, end_addr,
			kern_addr, offset;
	vm_size_t	size;
	int		result;

	/*
	 *	Read victim's memory
	 */

#ifdef	mips
	if (!ALIGNED((int *) address, amount))
		return(EIO);
#endif	mips
	
#ifdef	OSF1_SERVER
	start_addr = trunc_page(address);
	end_addr = round_page(address + amount);
	size = end_addr - start_addr;
	
	/*
	 * Unlock master lock to touch user data.
	 */
	if (u.u_master_lock)
		master_unlock();
	
	if (vm_read(p->p_task, start_addr, size,
		    &kern_addr, &size)
	    != KERN_SUCCESS) {
		result = EIO;
		if (u.u_master_lock)
			master_lock();
		goto end;
	}
	/*
	 *	Read the data from the copy in the server map.
	 *	Use bcopy to avoid alignment restrictions.
	 */
	offset = (vm_offset_t)address - start_addr;
	bcopy((caddr_t)(kern_addr + offset), resulting_data, amount);
	result = 0;
	
	/*
	 *	Discard the server's copy.
	 */
	(void)vm_deallocate(mach_task_self(), kern_addr, size);
	if (u.u_master_lock)
		master_lock();
 end:

#else	/* OSF1_SERVER */
		/* This section is intentionally left blank (jqr) */
#endif	/* OSF1_SERVER */

	return(result);
}


int ptrace_write_data(p, address, amount, new_data, old_data, flush)
	struct proc	*p;
	vm_offset_t	address;
	vm_size_t	amount;
	caddr_t		new_data;
	caddr_t		old_data;
	boolean_t	flush;
{
	vm_offset_t	start_addr, end_addr,
			kern_addr, offset;
	vm_size_t	size;
	int		result = 0;

#if	!defined(mips) && defined(lint)
	if (flush) old_data++;
#endif	!defined(mips) && defined(lint)

	/*
	 *	Write victim's memory
	 */
#ifdef	mips
	if (!ALIGNED(address, amount))
		return(EIO);
#endif	mips

	start_addr = trunc_page(address);
	end_addr = round_page(address + amount);
	size = end_addr - start_addr;

#ifdef	OSF1_SERVER
	/*
	 *	Allocate some pageable memory in the server map,
	 *	and copy the victim's memory to it.
	 */
	
	/*
	 * Unlock master lock to touch user data.
	 */
	if (u.u_master_lock)
		master_unlock();
	
	if (vm_read(p->p_task, start_addr, size,
		    &kern_addr, &size)
	    != KERN_SUCCESS) {
		result = EIO;
		if (u.u_master_lock)
			master_lock();
		goto end;
	}
	/*
	 *	Change the data in the copy in the server map.
	 *	Use bcopy to avoid alignment restrictions.
	 */
	offset = (vm_offset_t)address - start_addr;
#ifdef	mips
	bcopy((caddr_t) (kern_addr + offset), old_data, amount);
#endif	mips
	bcopy(new_data, (caddr_t) (kern_addr + offset), amount);
	
	/*
	 *	Copy it back to the victim.
	 */
	if (vm_write(p->p_task, start_addr, kern_addr, size) != KERN_SUCCESS) {
		/*
		 * Area may not be writable.  Try changing
		 * its protection.
		 */
		if (vm_protect(p->p_task, start_addr, size, FALSE, VM_PROT_ALL)
		    != KERN_SUCCESS)
			result = EIO;
		else {
			if (vm_write(p->p_task, start_addr,  kern_addr, size)
			    != KERN_SUCCESS)
				result = EIO;
			(void) vm_protect(p->p_task, start_addr, size,
					  FALSE,
					  VM_PROT_READ|VM_PROT_EXECUTE);
		}
	}
#ifdef	mips
	/*
	 *	Flush Icache, as we might have changed the victim's code.
	 */
	if (flush) {
		vm_machine_attribute_val_t value = MATTR_VAL_ICACHE_FLUSH;
		kern_return_t ret;
		ret = vm_machine_attribute(p->p_task, address, amount,
					   MATTR_CACHE, &value);
	}
#endif	mips
	
	/*
	 *	Discard the server's copy.
	 */
	(void)vm_deallocate(mach_task_self(), kern_addr, size);
	
	if (u.u_master_lock)
		master_lock();
 end:

#else	/* OSF1_SERVER */
		/* This section is intentionally left blank (jqr) */
#endif	/* OSF1_SERVER */

	return(result);
}
