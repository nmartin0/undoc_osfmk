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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: bsd_machdep.c,v $
 * Revision 1.6  1995/02/01  21:31:34  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:30:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/25  22:21:02  yazz
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
 * Added new machine-dependent routines to save and to display the current
 * call chain.  These routines are only compiled into assertful servers.
 *
 * Revision 1.3  1993/07/14  17:55:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:14:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:13  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:12:40  cfj
 * Bump major revision number.
 *
 * Revision 2.12  1992/10/06  12:12:14  roman
 * Fix types for a clean gcc compilation.
 *
 * Revision 2.11  92/08/06  13:35:53  klh
 * 	Revision 2.8  92/07/29  08:23:31  rabii
 * 		Replaced put_thread_state_pc with put_thread_state_pc_err which
 * 		also sets the error register to the error code (rabii)
 * 
 * Revision 2.10  92/07/07  15:16:26  roman
 * Fix format to match OSF source exactly.
 * 
 * Revision 2.9  92/06/10  14:21:25  chrisp
 * [Bug #24] Honor requirement to clear most significant bit of sigreturn 
 *	address unless the handler is within the emulator's space.
 * 
 * Revision 2.8  92/06/05  13:57:23  klh
 * 	Revision 2.6  92/05/24  14:27:27  pjg
 * 		92/03/24  21:03:30  barbou
 * 		Fix for bug #117: fixed the page 0 protection.
 * 		[92/05/18            srl]
 * 
 * Revision 2.7  92/06/02  07:33:49  chrisp
 * For TNC, don't clear the most significant bit of the sigreturn address
 * 	since by default this is inside the emulator and hence this bit
 * 	is most significant.
 * 
 * Revision 2.6  92/04/28  09:29:55  chrisp
 * For TNC, sendsig() must set the parameter code to be the signal argument
 * 	when the signal is SIGMIGRATE.
 * 
 * Revision 2.5  92/03/09  12:03:12  durriya
 * 	[Revision 3.14  92/02/28  00:11:24  condict]
 * 	Add machine-independent interface for getting and setting thread 
 *	state and (optionally) modifying the pc.
 * 
 * 	[Revision 3.13  91/12/24  12:37:56  sp]
 * 	use psl.h rather than the old bsd eflags.h
 * 
 * Revision 2.4  91/12/16  18:47:46  roy
 * 	91/10/17  18:33:18  barbou
 * 	Deleted include of "cmucs.h" (defunct option).
 * 
 * 	91/10/09  18:37:51  emcmanus
 * 	Implemented coredump.
 * 
 * Revision 2.3  91/10/14  12:14:36  sjs
 * 	Revision 3.10  91/10/03  10:38:11  jose
 * 	Adapted ptrace related routines and data
 * 	to OSF/1 requirements
 * 
 * Revision 2.2  91/08/31  13:33:01  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.9  91/08/27  15:29:51  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.8  91/08/09  12:14:59  barbou
 * Added get_thread_pc(). Used by kern_sig.c to check if in the emulator.
 * 
 * Revision 3.7  91/07/26  15:51:30  jose
 * Changed set_entry_address to support COFF entry point
 * 
 * Revision 3.6  91/07/02  14:44:56  condict
 * Delete old load_emulator function and machine_getxfile (new version of
 * load_emulator is on kern_exec.c).
 * 
 * Revision 3.5  91/06/25  17:06:09  condict
 * Moved sys/*.h files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.4  91/05/15  17:49:13  barbou
 * i386 uses u_sigreturn instead of u_sigtramp.
 * 
 * Revision 3.3  91/05/07  15:41:24  condict
 * Fix bug in loading executables (need to add size of struct exec to text size
 * 
 * Revision 3.2  91/03/08  16:03:19  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/02/27  16:38:33  condict
 * Redefine namei, access, rdwri, iput and irele to call version in
 * empty_server.c (makes calls to parent UNIX server).  Also, fix bug in
 * calculation of copy_size, and use default pager instead of inode pager.
 * 
 * Revision 3.0  91/01/17  12:10:19  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.10  91/08/12  22:37:24  rvb
 * 	Fixed machine_getxfile so data isn't executable.
 * 	[91/04/17            rpd]
 * 
 * 	Fixed text protection bug in machine_getxfile, from Keith Loepere.
 * 	[91/04/10            rpd]
 * 
 * Revision 2.9  91/03/20  14:58:50  dbg
 * 	Fixed copy_size bug in machine_getxfile, from Michael Condict.
 * 	[91/02/20            rpd]
 * 
 * Revision 2.8  91/01/08  17:44:17  rpd
 * 	i386:	Use low 2 bytes for magic tests.  The high byte is a flag;
 * 	0x01 indicate that a_trsize is really the text relocation which
 * 	is used vs USRTEXT.
 * 	[90/12/14            rvb]
 * 
 * Revision 2.7  90/09/09  22:32:04  rpd
 * 	Fixed 407 & 410 images in machine_getxfile().
 * 	[90/09/04            af]
 * 
 * Revision 2.6  90/08/06  15:32:48  rwd
 * 	Added locking for p->p_sig*.
 * 	[90/06/08            rwd]
 * 
 * Revision 2.5  90/06/19  23:10:35  rpd
 * 	Picked up correct mapped signal locking in sendsig.
 * 	[90/06/08            rpd]
 * 
 * 	Temporarily turned off partial-page vm_write use.
 * 	[90/06/04            rpd]
 * 
 * Revision 2.4  90/06/02  15:23:24  rpd
 * 	Removed u_text_start.
 * 	[90/06/02            rpd]
 * 
 * 	Converted to new IPC.  Picked up sendsig fixes.
 * 	[90/06/01            rpd]
 * 
 * Revision 2.3  90/05/29  20:23:56  rwd
 * 	Don't deallocate shared files on exec.
 * 	[90/05/22            rwd]
 * 
 * Revision 2.2  90/05/21  13:51:57  dbg
 * 	Created i386 version from VAX version.
 * 	[90/02/06            dbg]
 * 
 */

#include <map_uarea.h>
#include <mach_nbc.h>

/*
 *	Machine-dependent routines evicted from rest of BSD
 *	files.
 */

#include <i386/psl.h>
#include <i386/reg.h>

#include <i386/vmparam.h>

#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <ufs/inode.h>
#include <sys/uio.h>
#include <sys/exec.h>
#include <sys/systm.h>		/* boothowto */
#include <kern/parallel.h>
#include <sys/syscall.h>

#include <uxkern/import_mach.h>

#define	LOADER_PAGE_SIZE	(4096)
#define loader_round_page(x)	((vm_offset_t)((((vm_offset_t)(x)) \
						+ LOADER_PAGE_SIZE - 1) \
					& ~(LOADER_PAGE_SIZE-1)))
#define loader_trunc_page(x)	((vm_offset_t)(((vm_offset_t)(x)) \
					& ~(LOADER_PAGE_SIZE-1)))

vm_prot_t	page_0_protection = VM_PROT_NONE;

set_arg_addr(arg_size,
	     arg_start_p,
	     arg_size_p)
	u_int		arg_size;
	vm_offset_t	*arg_start_p;
	vm_size_t	*arg_size_p;
{
	/*
	 * Round arg size to fullwords
	 */
	arg_size = (arg_size + NBPW-1) & ~(NBPW - 1);

	/*
	 * Put argument list at top of stack.
	 */
	*arg_start_p = USRSTACK - arg_size;
	*arg_size_p = arg_size;
}

extern boolean_t	first_exec;

set_entry_address(hdr_entry, entry, entry_count)
	int		hdr_entry;
	int		*entry;		/* pointer to OUT array */
	unsigned int	*entry_count;	/* out */
{
	entry[0] = hdr_entry;
	*entry_count = 1;
}

/*
 * Set up the process' registers to start the emulator - duplicate
 * what execve() in the emulator would do.
 */
set_emulator_state(entry, entry_count, arg_addr)
	int		*entry;		/* An array of size entry_count */
	int		entry_count;	/* For now, must be 1 */
	vm_offset_t	arg_addr;
{
	struct i386_thread_state	ts;
	unsigned int			count;

	/*
	 * Read the registers first, to get the correct
	 * segments.
	 */
	count = i386_THREAD_STATE_COUNT;
	(void) thread_get_state(u.u_procp->p_thread,
				i386_THREAD_STATE,
				(thread_state_t)&ts,
				&count);
	ts.eip = entry[0];
	ts.uesp = (int)arg_addr;
	ts.efl = EFL_USERSET;

	(void) thread_set_state(u.u_procp->p_thread,
				i386_THREAD_STATE,
				(thread_state_t)&ts,
				i386_THREAD_STATE_COUNT);
}

/*
 * Do weird things to the first process' address space before loading
 * the emulator.
 */
void
emul_init_process(p)
	struct proc	*p;
{
	/*
	 * Clear out the entire address space.
	 */
	(void) vm_deallocate(p->p_task,
			VM_MIN_ADDRESS,
			(vm_size_t)(VM_MAX_ADDRESS - VM_MIN_ADDRESS));

}


#ifdef NOT_YET
/*
 * This needs work.  It is currently based on the use of the rdwri func:
 */
machine_core(p, ip)
	register struct proc	*p;
	register struct inode	*ip;
{
	vm_offset_t	stack_addr;
	vm_size_t	stack_size;
	off_t		offset = 0;
	int		error;

	struct i386_thread_state	ts;
	unsigned int		count;

	/*
	 * Get the registers for the thread.
	 */

	count = i386_THREAD_STATE_COUNT;
	(void) thread_get_state(p->p_thread,
				i386_THREAD_STATE,
				(thread_state_t)&ts,
				&count);

	/*
	 * Fake the stack segment size from the current
	 * stack pointer value.
	 */
	stack_addr = trunc_page(ts.uesp);
	stack_size = round_page(USRSTACK - stack_addr);
	p->p_utask.uu_ssize = btoc(stack_size);

	/*
	 * Build a fake U-area for the bottom of the
	 * 'kernel-stack' in the core file.
	 */
	{
	    struct user fake_uarea;

	    fake_u(&fake_uarea, p, p->p_thread);

	    error = rdwri(UIO_WRITE, ip,
		(caddr_t)&fake_uarea, sizeof(struct user),
		offset, UIO_SYSSPACE, (int *)0);
	}
	if (error)
	    goto out;

	/*
	 * Build a fake register save area for the top of
	 * the 'kernel-stack' in the core file.
	 */
	{
	    int		regs[21];
#define	LOCR0_OFFSET	21

	    bzero((caddr_t)regs, sizeof(regs));

	    regs[EAX + LOCR0_OFFSET] = ts.eax;
	    regs[EBX + LOCR0_OFFSET] = ts.ebx;
	    regs[ECX + LOCR0_OFFSET] = ts.ecx;
	    regs[EDX + LOCR0_OFFSET] = ts.edx;
	    regs[ESP + LOCR0_OFFSET] = ts.esp;
	    regs[EBP + LOCR0_OFFSET] = ts.ebp;
	    regs[ESI + LOCR0_OFFSET] = ts.esi;
	    regs[EDI + LOCR0_OFFSET] = ts.edi;
	    regs[EIP + LOCR0_OFFSET] = ts.eip;
	    regs[EFL + LOCR0_OFFSET] = ts.efl;
	    regs[UESP+ LOCR0_OFFSET] = ts.uesp;
	    regs[CS  + LOCR0_OFFSET] = ts.cs;
	    regs[SS  + LOCR0_OFFSET] = ts.ss;
	    regs[DS  + LOCR0_OFFSET] = ts.ds;
	    regs[ES  + LOCR0_OFFSET] = ts.es;
	    regs[FS  + LOCR0_OFFSET] = ts.fs;
	    regs[GS  + LOCR0_OFFSET] = ts.gs;

	    offset = ctob(UPAGES);
	    error = rdwri(UIO_WRITE, ip,
		(caddr_t)regs, (int) sizeof(regs),
		offset - sizeof(regs),
		UIO_SYSSPACE, (int *)0);
	}
	if (error)
	    goto out;

	/*
	 * Write the data segment.
	 */
	error = core_file_write(ip, &offset, p->p_task,
			p->p_utask.uu_data_start,
			(int)ctob(p->p_utask.uu_dsize));
	if (error)
	    goto out;

	/*
	 * Write the stack.
	 */
	error = core_file_write(ip, &offset, p->p_task,
			stack_addr, stack_size);

    out:
	return (error);

#undef	LOCR0_OFFSET
}
#endif NOT_YET


#ifdef OSF1_SERVER
/* machine_core_info - get machine-dependent information needed to
   produce a core file.  stack_addr will be the user stack pointer;
   from here to USRSTACK will be written to the core file as the
   user stack.  fake_kernel_stack will be a fabricated stack that
   will be written at the opposite end of the trailing UPAGES from
   the user structure, because debuggers expect user registers to
   have been saved on entry to the "kernel".  The size of this faked
   stack is fake_kernel_stack_size, and it is expected to be kfree'd
   after use.  This size should be < KERNEL_STACK_SIZE.  */
void
machine_core_info(p, stack_addrp, fake_kernel_stackp, fake_kernel_stack_sizep)
struct proc *p;
vm_offset_t *stack_addrp;
caddr_t *fake_kernel_stackp;
int *fake_kernel_stack_sizep;
{
    unsigned int count;
    int *regs;
    thread_t th;
    struct i386_thread_state	ts;
#define NFAKEREGS 19
    if (task_first_thread(p->p_task, &th) != KERN_SUCCESS) {
	*stack_addrp = NULL;
	*fake_kernel_stackp = NULL;
	return;
    }
    /* Get the registers for the thread.  */
    count = i386_THREAD_STATE_COUNT;
    (void) thread_get_state(th, i386_THREAD_STATE,
			    (thread_state_t)&ts, &count);
    /* don't need the thread port anymore */
    (void) mach_port_deallocate(mach_task_self(), th);
#if MAP_UAREA
    if (p->p_shared_rw->us_emul_regs && ts.eip >= EMULATOR_BASE)
	adjust_emul_regs(p, &ts);
#endif
    *stack_addrp = ts.uesp;
    if ((regs = (int *) kalloc(NFAKEREGS * sizeof(int))) != NULL) {
	bzero((caddr_t)regs, NFAKEREGS * sizeof(int));
	regs[SS] = ts.ss;
	regs[UESP] = ts.uesp;
	regs[EFL] = ts.efl;
	regs[CS] = ts.cs;
	regs[EIP] = ts.eip;
	regs[EAX] = ts.eax;
	regs[ECX] = ts.ecx;
	regs[EDX] = ts.edx;
	regs[EBX] = ts.ebx;
	regs[ESP] = ts.esp;
	regs[EBP] = ts.ebp;
	regs[ESI] = ts.esi;
	regs[EDI] = ts.edi;
	regs[DS] = ts.ds;
	regs[ES] = ts.es;
	regs[FS] = ts.fs;
	regs[GS] = ts.gs;
	/* Could use ipcreg[] table instead of these assignments. */
    }
    *fake_kernel_stackp = (caddr_t) regs;
    *fake_kernel_stack_sizep = NFAKEREGS * sizeof(int);
}


#if MAP_UAREA	/* Necessary info (saved user sp) not available otherwise. */

/* Can't use copyin() because of the u_reply_msg nonsense. */
#define COPYIN(p, from, to, len, result) MACRO_BEGIN \
    vm_offset_t xfrom = (vm_offset_t)from; \
    int xlen = len; \
    vm_address_t start = trunc_page(xfrom); \
    vm_address_t end = round_page(xfrom + xlen); \
    vm_address_t data; \
    vm_size_t data_len; \
    result = vm_read(p->p_task, start, (vm_size_t)(end - start), \
			 &data, &data_len); \
    if (result == KERN_SUCCESS) { \
	bcopy(data + (xfrom - start), to, xlen); \
	(void) vm_deallocate(mach_task_self(), data, data_len); \
    } \
MACRO_END


adjust_emul_regs(p, ts)
struct proc *p;
struct i386_thread_state *ts;
{
    struct emul_regs emul_regs;
    struct emul_regs_2 emul_regs_2;
    int result;
    if (u.u_master_lock)
	master_unlock();
    COPYIN(p, p->p_shared_rw->us_emul_regs, &emul_regs, sizeof emul_regs,
	   result);
    if (result != KERN_SUCCESS)
	goto fail;
    COPYIN(p, emul_regs.uesp, &emul_regs_2, sizeof emul_regs_2, result);
    if (result != KERN_SUCCESS)
	goto fail;
    ts->edx = emul_regs_2.edx;
    ts->ecx = emul_regs_2.ecx;
    ts->eax = emul_regs_2.eax;
    ts->eip = emul_regs_2.eip;	/* Or we could leave it in the emulator. */
    ts->uesp = (int) emul_regs.uesp;
    ts->ebp = emul_regs.ebp;
    ts->edi = emul_regs.ebx;
    ts->esi = emul_regs.esi;
    ts->ebx = emul_regs.ebx;
fail:
    if (u.u_master_lock)
	master_lock();
}

#endif	/* MAP_UAREA */

#endif	/* OSF1_SERVER */


/* 
 * We need this structure to calculate the offset associated with registers
 * so that requests from OSF/1 debuggers can be satisfied with no changes
 */
struct i386_saved_state {
	unsigned int	gs;
	unsigned int	fs;
	unsigned int	es;
	unsigned int	ds;
	unsigned int	edi;
	unsigned int	esi;
	unsigned int	ebp;
	unsigned int	esp;
	unsigned int	ebx;
	unsigned int	edx;
	unsigned int	ecx;
	unsigned int	eax;
	unsigned int	trapno;
	unsigned int	err;
	unsigned int	eip;
	unsigned int	cs;
	unsigned int	efl;
	unsigned int	uesp;
	unsigned int	ss;
	struct i386_v86_info {
		/* v86 mode registers (optional) */
		unsigned int	oldes;
		unsigned int	oldds;
		unsigned int	oldfs;
		unsigned int	oldgs;
	} v86;
};

struct reg_offsets {
	int	reg_offset;
	int	state_offset;
};
#define	state_off(s)	(int)&(((struct i386_thread_state *)0)->s)
#define	reg_off(r)	(ctob(UPAGES) - sizeof(struct i386_saved_state) + sizeof(int)*(r))
#define regs(r,s)	{ reg_off(r), state_off(s) }

struct reg_offsets ipcreg[] = {
	regs(GS, gs),
	regs(FS, fs),
	regs(ES, es),
	regs(DS, ds),
	regs(EDI, edi),
	regs(ESI, esi),
	regs(EBP, ebp),
	regs(ESP, esp),
	regs(EBX, ebx),
	regs(EDX, edx),
	regs(ECX, ecx),
	regs(EAX, eax),
	regs(EIP, eip),
	regs(CS, cs),
	regs(EFL, efl),
	regs(UESP, uesp),
	regs(SS, ss),
};
#define	NIPCREG	(sizeof(ipcreg)/sizeof(ipcreg[0]))

#undef	regs
#undef	reg_off
#undef	state_off

int ptrace_get_u_word(p, offset, value)
	struct proc *p;
	register int	offset;
	int	*value;	/* out */
{
	if (offset < 0 || offset >= ctob(UPAGES))
	    return (EIO);

	if (offset < sizeof(struct user)) {
	    struct user fake_uarea;

	    bzero((caddr_t)&fake_uarea, sizeof(struct user));
	    fake_u(&fake_uarea, p, p->p_thread);
	    *value = *(int *)(((caddr_t)&fake_uarea) + offset);
	}
	else {
	    struct i386_thread_state	thread_state;
	    unsigned int		count;
	    register int		j;

	    *value = 0;

	    for (j = 0; j < NIPCREG; j++) {
		if (ipcreg[j].reg_offset == offset) {
		    break;
		}
	    }

	    if (j < NIPCREG) {
		    count = i386_THREAD_STATE_COUNT;
		    (void) thread_get_state(p->p_thread,
					    i386_THREAD_STATE,
					    (thread_state_t)&thread_state,
					    &count);

		    *value = *(int *)(((caddr_t)&thread_state)
				      + ipcreg[j].state_offset);
	    }
	}
	return (0);
}

int ptrace_set_u_word(p, offset, value, old)
	struct proc *p;
	register int offset;
	int	value;
	int	*old;
{
	/*
	 *	Write victim's registers.
	 *	Offsets are into BSD kernel stack, and must
	 *	be faked to match MACH.
	 */
	struct i386_thread_state	thread_state;
	unsigned int	count;
	register int	j;

	for (j = 0; j < NIPCREG; j++)
	    if (ipcreg[j].reg_offset == offset)
		break;

	if (j == NIPCREG) {
	    /* wrong register */
	    return (EIO);
	}

	count = i386_THREAD_STATE_COUNT;
	(void) thread_get_state(p->p_thread,
				i386_THREAD_STATE,
				(thread_state_t)&thread_state,
				&count);

	/* thread_set_state handles checking PSL for valid values */
	*(int *)(((caddr_t)&thread_state)
			+ ipcreg[j].state_offset)
		      = value;

	(void) thread_set_state(p->p_thread,
				i386_THREAD_STATE,
				(thread_state_t)&thread_state,
				count);
	return (0);
}

void ptrace_set_trace(p, new_addr, trace_on)
	struct proc *p;
	int	new_addr;
	int	trace_on;
{
	struct i386_thread_state	thread_state;
	unsigned int		count;

	count = i386_THREAD_STATE_COUNT;
	(void) thread_get_state(p->p_thread,
				i386_THREAD_STATE,
				(thread_state_t)&thread_state,
				&count);

	if (new_addr != 1)
		thread_state.eip = new_addr;

	if (trace_on)
		thread_state.efl   |= EFL_TF;
	else
		thread_state.efl   &= ~EFL_TF;

	(void) thread_set_state(p->p_thread,
				i386_THREAD_STATE,
				(thread_state_t)&thread_state,
				count);
}

/*
 * Get the PC (and the rest of the state as well) for the specified thread,
 * using a machine-independent interface.
 *
 * Caller must supply the buffer for the thread state info, and this buffer
 * should be declared as thread_data_t, if it is to be machine-independent.
 * Upon return, the complete thread state is in the buffer and can be
 * modified and used in a thread_set_state (see put_thread_state_pc, below):
 */
kern_return_t
get_thread_state_pc(thread, pc, thread_state_p)
	thread_t	thread;
	int		*pc;
	thread_state_t	thread_state_p;
{
	unsigned int			size;
	kern_return_t			rc;

	if (thread == NULL) {
		return KERN_FAILURE;
	}
	/*
	 * Get the registers for the thread.
	 */
	size = i386_THREAD_STATE_COUNT;
	rc = thread_get_state(thread, i386_THREAD_STATE, thread_state_p, &size);
	if (rc != KERN_SUCCESS)
		return rc;
	*pc = ((struct i386_thread_state *)thread_state_p)->eip;
	return KERN_SUCCESS;
}

/*
 * Set the PC (and the rest of the state as well) for the specified thread,
 * using a machine-independent interface.
 *
 * Caller must supply a buffer containing the desired thread state info
 * (except for PC value).  The PC register in the buffer is modified, and
 * then the state of the thread is set as specified by the modified buffer.
 */
kern_return_t
put_thread_state_pc_err(thread, pc, err, thread_state_p)
	thread_t	thread;
	int		pc;
	int		err;
	thread_state_t	thread_state_p;
{
	kern_return_t			rc;

	((struct i386_thread_state *)thread_state_p)->eip = pc;
	((struct i386_thread_state *)thread_state_p)->eax = err;
	rc = thread_set_state(thread, i386_THREAD_STATE, thread_state_p,
						     i386_THREAD_STATE_COUNT);
	return rc;
}

/*
 * Send an interrupt to process.
 *
 * Stack is set up to allow sigcode stored in u. to call routine,
 * followed by chmk to sigreturn routine below.  After sigreturn
 * resets the signal mask, the stack, the frame pointer, and the
 * argument pointer, it returns to the user-specified pc/psl.
 */
sendsig(p, thread, sig_hdlr, sig, code, mask)
	struct proc	*p;
	thread_t	thread;
	int (*sig_hdlr)(), sig, code, mask;
{
	register struct sigcontext *scp;
	register struct sigframe {
	    int		(*sf_retadr)();
	    int		sf_signum;
	    int		sf_code;
	    struct sigcontext *sf_scp;
	    struct sigcontext *sf_scpcopy;
	} *fp;
	int	oonstack;

	struct i386_thread_state regs;
	unsigned int		reg_size;

	vm_offset_t	user_start_addr, user_end_addr, kern_addr;
	vm_size_t	user_size;

	register struct sigcontext *kscp;
	register struct sigframe   *kfp;

	/*
	 * Get the registers for the thread.
	 */
	reg_size = i386_THREAD_STATE_COUNT;
	(void) thread_get_state(thread, i386_THREAD_STATE,
				(thread_state_t)&regs, &reg_size);

#if	MAP_UAREA
	share_lock(&p->p_shared_rw->us_lock, p);
#endif	MAP_UAREA
	oonstack = p->p_utask.uu_sigstack.ss_onstack;

	if (!oonstack && (p->p_utask.uu_sigonstack & sigmask(sig))) {
	    scp = (struct sigcontext *)p->p_utask.uu_sigstack.ss_sp - 1;
	    p->p_utask.uu_sigstack.ss_onstack = 1;
	}
	else {
	    scp = (struct sigcontext *)regs.uesp - 1;
	}
#if	MAP_UAREA
	share_unlock(&p->p_shared_rw->us_lock, p);
#endif	MAP_UAREA
	fp = (struct sigframe *)scp - 1;

	/*
	 * Copy the signal frame from the user into the kernel,
	 * to modify it.
	 */
	user_start_addr = trunc_page((vm_offset_t)fp);
	user_end_addr   = round_page((vm_offset_t)fp
					+ sizeof(*fp)
					+ sizeof(*scp));
	user_size = user_end_addr - user_start_addr;

	if (vm_read(p->p_task, user_start_addr, user_size,
			&kern_addr, &user_size) != KERN_SUCCESS)
	    goto error;

	kfp = (struct sigframe *)((vm_offset_t)fp - user_start_addr
					+ kern_addr);
	kscp = (struct sigcontext *)((vm_offset_t)scp - user_start_addr
					+ kern_addr);

	/*
	 * Build the argument list for the signal handler.
	 */
	kfp->sf_signum = sig;
	if (sig == SIGILL || sig == SIGFPE)
	    kfp->sf_code = code;
#ifdef	TNC
	else if (sig == SIGMIGRATE)
	    kfp->sf_code = p->p_sig_arg;
#endif	/* TNC */
	else
	    kfp->sf_code = 0;
	kfp->sf_scp = scp;

	/*
	 * Build the stack frame to be used to call sigreturn.
	 */
	kfp->sf_scpcopy = scp;
#ifdef	TNC
	{
		unsigned long retadr = (unsigned long) p->p_utask.uu_sigreturn;
		kfp->sf_retadr = (int (*)())
			((EMULATOR_BASE <= retadr && retadr <= EMULATOR_END)
			? retadr
			: retadr & ~0x80000000);
	}
#else	/* TNC */
	kfp->sf_retadr = (int (*)())
		((unsigned long) p->p_utask.uu_sigreturn & ~0x80000000);
#endif	/* TNC */

	/*
	 * Build the signal context to be used by sigreturn.
	 */
	kscp->sc_onstack = oonstack;
	kscp->sc_mask = mask;
	/* copy all registers */
	kscp->sc_gs = regs.gs;
	kscp->sc_fs = regs.fs;
	kscp->sc_es = regs.es;
	kscp->sc_ds = regs.ds;
	kscp->sc_edi = regs.edi;
	kscp->sc_esi = regs.esi;
	kscp->sc_ebp = regs.ebp;
	kscp->sc_esp = 0;		/* kernel stack pointer? */
	kscp->sc_ebx = regs.ebx;
	kscp->sc_edx = regs.edx;
	kscp->sc_ecx = regs.ecx;
	kscp->sc_eax = regs.eax;
	kscp->sc_trapno = 0;
	kscp->sc_err = 0;
	kscp->sc_eip = regs.eip;
	kscp->sc_cs = regs.cs;
	kscp->sc_efl = regs.efl;
	kscp->sc_uesp = regs.uesp;
	kscp->sc_ss = regs.ss;

	/*
	 * Set up the new stack and handler address.
	 */
	regs.uesp = (int)fp;
	regs.eip = (int)sig_hdlr;

	/*
	 * Write signal frame and context back to user.
	 */
	if (vm_write(p->p_task, user_start_addr, kern_addr, user_size)
		!= KERN_SUCCESS) {
		(void)vm_deallocate(mach_task_self(), kern_addr, user_size);
		goto error;
	}

	/*
	 * Write changed registers back to thread.
	 */
	(void) thread_set_state(thread, i386_THREAD_STATE,
				(thread_state_t)&regs, reg_size);

	(void)vm_deallocate(mach_task_self(), kern_addr, user_size);
	return;

    error:
	/*
	 * Process has trashed its stack; kill it and core dump it.
	 * The idea is to imitate the default action for a SIGILL.
	 */
	proc_exit(p, SIGILL, TRUE);
}

sigreturn()
{
    panic("sigreturn - MiG interface");
}
osigcleanup()
{
    panic("osigcleanup - MiG interface");
}
	
/*
 * Clone the parent's registers into the child thread for fork.
 */
boolean_t
thread_dup(child_thread, new_state, new_state_count, parent_pid, rc)
	thread_t	child_thread;
	thread_state_t	new_state;
	unsigned int	new_state_count;
	int		parent_pid, rc;
{
	struct i386_thread_state *regs = (struct i386_thread_state *)new_state;

	if (new_state_count != i386_THREAD_STATE_COUNT)
	    return (FALSE);

	regs->eax = parent_pid;
	regs->edx = rc;

	(void) thread_set_state(child_thread, i386_THREAD_STATE,
				new_state, new_state_count);
	return (TRUE);
}

#if MACH_ASSERT
void
save_call_chain(
	int		*destp,
	int		max)
{
	int		*srcp;
	int		i;
	static int	seq = 0;		/* bumped once for each call */
	int		*current_fp(void);

	destp[0] = seq++;

	srcp = (int *)current_fp();		/* current frame pointer */
	for (i=1; i<max-1 && srcp; ++i) {
		destp[i] = srcp[1];
		srcp = (int *)srcp[0];
	}
	for (; i<max; ++i) {
		destp[i] = 0x77777777;
	}
}

void
print_call_chain(
	int		*srcp,
	int		max,
	char		*string)
{
	int		i;

	printf("%s seq=%d: %x", string, srcp[0], srcp[1]);
	for (i=2; i<max; ++i) {
		printf("<-%x", srcp[i]);
	}
	printf("\n");
}
#endif	/* MACH_ASSERT */
