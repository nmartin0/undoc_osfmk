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
 * Revision 1.17  1995/02/01  21:31:57  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.16  1994/11/18  20:31:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/10/25  22:24:57  yazz
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
 * Revision 1.14  1994/05/16  17:46:36  cfj
 * No longer allow read access to page 0 for applications.
 *
 *  Reviewer:suri
 *  Risk:L
 *  Benefit or PTS #:6156
 *  Testing:Test case.
 *  Module(s):server/i860/bsd_machdep.c
 *            server/uxkern/ux_exception.c
 *
 * Revision 1.13  1994/03/14  02:02:05  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.12  1993/08/24  18:03:09  nandy
 * Cleaned up some ugly code in ptrace_set_u_word().
 *
 * Revision 1.11  1993/08/18  15:37:59  robboy
 * In sendsig(), cleared bits in fsr so SIGFPE signal handler will not get
 * spurious exceptions. Also removed code clearing psr bits, which cfj had
 * removed but snuck back in.
 *
 * Revision 1.10  1993/07/14  17:58:08  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:17:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.9  1993/06/07  23:41:05  nandy
 * To allow ptrace to write to double registers, ptrace_set_u_word() goes
 * by word alignmnet and not by register offset.
 *
 * Revision 1.8  1993/05/06  19:08:51  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:29:41  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.7  1993/04/27  16:43:43  cfj
 * Add a cast to remove annoying compiler warning.
 *
 * Revision 1.6  1993/04/13  15:55:10  cfj
 * Merge with T9.5.
 *
 * Revision 1.3.6.2  1993/04/12  22:34:10  cfj
 * Do full context restore on sigreturn.
 *
 * Revision 1.5  1993/04/03  03:05:53  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4  1993/04/01  23:48:19  cfj
 * Merge with T9.
 *
 * Revision 1.3.6.1  1993/04/01  23:46:55  cfj
 * Check EPSR_DI bit and if set, decrement the fir because the thread needs
 * to re-execute an instruction which had a branch delay slot.
 *
 * Revision 1.1.2.1.2.2  1993/01/09  00:04:17  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.3  1992/12/31  17:29:40  nandy
 * Charlie's changes for turning off dual instruction mode
 *
 * Revision 1.1.2.1.2.1  1992/12/14  23:12:25  brad
 * Merged tip of old NX branch with PFS branch.
 * 
 * Revision 1.2  1992/11/30  22:21:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/17  20:15:49  cfj
 * Added _i860_cpu_type() which is used when initializing ufsname.machine.
 *
 * Revision 1.1.2.1  1992/11/05  23:20:41  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:14:10  cfj
 * Bump major revision number.
 *
 * Revision 2.15  1992/10/17  18:25:37  cfj
 * IPD & Allocator Support.
 *
 * Revision 2.14  1992/10/16  18:15:28  cfj
 * Modification so that NX programs can run.
 *
 * Revision 2.13  1992/10/14  17:17:01  cfj
 * NX integration.
 *
 * Revision 2.12  1992/10/06  12:12:47  roman
 * Fix RCS comments.
 *
 * Revision 2.11  92/10/05  13:38:05  klh
 * 	Revision 2.12  92/09/29  16:49:59  rabii
 * 		[92/09/08  09:48:51  cfj@ssd.intel.com]
 * 		No longer trash r29 in sendsig().  sigreturn() now depends on 
 * 		sigcontext.sc_fir to determine where to return in user code.
 * 
 * 	Revision 2.11  92/09/24  16:50:08  rabii
 * 		Made page_0_protection VM_PROT_READ. (rabii)
 *
 * 	Revision 2.10  92/09/17  13:42:56  rabii
 * 		[1992/09/13  23:02:53  stans]
 * 		ipcreg[] needed to be redefined with respect to new 
 *		i860_thread_status and i860/reg.h file from the micro-kernel.
 * 
 * 		[1992/09/13  22:52:33  stans]
 * 		new i860_thread_state structure; bcopy() in rtn: 
 *		machine_core_info().
 * 
 * Revision 2.10  92/08/06  13:36:20  klh
 * 	Revision 2.9  92/07/29  08:30:58  rabii
 * 		Replaced put_thread_state_pc with put_thread_state_pc_err which
 * 		also sets the error register to the error code (rabii)
 * 
 * Revision 2.9  92/06/10  12:21:34  klh
 * 	Revision 2.7  92/06/09  16:41:39  pjg
 * 		Set page 0 protection to NONE.
 * 
 * Revision 2.8  92/06/07  23:12:29  klh
 * Fix syntax errors introduced in previous check-in.
 * 
 * Revision 2.7  92/06/05  15:37:09  roman
 * For SIGMIGRATE in TNC, set up the migrate destination as an argument to
 * 	the signal handler routine.
 * 
 * Revision 2.6  92/05/31  22:28:39  loverso
 * 	Replaced get_thead_state_pc with correct version.
 * 	Added put_thread_state_pc.
 * 
 * Revision 2.5  92/03/15  14:49:20  roy
 * 	changed name of get_thread_pc to get_thread_state_pc (srl)
 * 	added missing ) to reg_off #define
 * 
 * Revision 2.4  92/03/01  18:47:26  pjg
 * 	Fixed define of reg_off (bug 109) rabii 
 * 
 * Revision 2.3  92/01/09  16:47:11  roy
 * 	91/12/23  10:30:55  stans
 * 	removed CMUCS ifdef's
 * 	machine_core_info() routined implemented from i386 prototype.
 * 
 * Revision 2.2  91/11/13  12:17:29  rabii
 * 	Initial checkin
 *
 * [Fri Sep 13 15:07:47 PDT 1991] cfj@ssd.intel.com
 *      Copied from CMU ux28 and removed machine_getxfile() function.
 *
 * Revision 2.2  91/09/03  11:12:18  jsb
 * 	First checkin. Derived from i386 sources by Intel SSD.
 * 	[91/09/03  10:15:56  jsb]
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
 * $EndLog
 */

#include <map_uarea.h>
#include <mach_nbc.h>

/*
 *	Machine-dependent routines evicted from rest of BSD
 *	files.
 */

#include <i860/reg.h>
#include <i860/psl.h>

#include <i860/vmparam.h>

#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/uio.h>
#include <sys/exec.h>
#include <sys/systm.h>
#include <kern/parallel.h>
#include <sys/syscall.h>

#include <uxkern/import_mach.h>

vm_prot_t       page_0_protection = VM_PROT_NONE;


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
	*arg_start_p = (USRSTACK - arg_size) & ~0xf;
	*arg_size_p = arg_size;
}

extern boolean_t	first_exec;

set_entry_address(hdr_entry, entry, entry_count)
	int		*entry;		/* An array of size entry_count */
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
	struct i860_thread_state	ts;
	unsigned int			count;

	/*
	 * Read the registers first, to get the correct
	 * segments.
	 */
	count = i860_THREAD_STATE_COUNT;
	(void) thread_get_state(u.u_procp->p_thread,
				i860_THREAD_STATE,
				(thread_state_t)&ts,
				&count);
	ts.pc = entry[0];
	ts.sp = (int)arg_addr;

        /*
         * setup i860 argument registers r16(argc), r17(argv) & r18 (envp)
         * according to i860 calling conventions. 'crt0.o' behaives as if it
         * were called from a higher level 'C' function.
         */
	ts.r16 = *(int *)arg_addr;                 /* argc */
	ts.r17 = (int)(arg_addr + 4);              /* argv */
	ts.r18 = (ts.r16*4 + 4) + ts.r17;          /* envp */

	(void) thread_set_state(u.u_procp->p_thread,
				i860_THREAD_STATE,
				(thread_state_t)&ts,
				i860_THREAD_STATE_COUNT);
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


#ifdef OSF1_SERVER

/* machine_core_info - get machine-dependent information needed to
   produce a core file.  stack_addr will be the user stack pointer;
   from here to USRSTACK will be written to the core file as the
   user stack.  fake_kernel_stack will be a fabricated stack that
   will be written at the opposite end of the trailing UPAGES from
   the user structure, because debuggers expect user registers to
   have been saved on entry to the "kernel".  The size of this faked
   stack is fake_kernel_stack_size, and it is expected to be kfree'd
   after use.  This size should be < KERNEL_STACK_SIZE.
 */

void
machine_core_info(
    struct proc *p,
    vm_offset_t *stack_addrp,
    caddr_t *fake_kernel_stackp,
    int *fake_kernel_stack_sizep )
{
    unsigned int count;
    int *regs;
    thread_t th;
    struct i860_thread_state	ts;

#define NFAKEREGS (sizeof(struct i860_thread_state)/sizeof(int))

    if (task_first_thread(p->p_task, &th) != KERN_SUCCESS) {
	*stack_addrp = NULL;
	*fake_kernel_stackp = NULL;
	return;
    }

    /*
     * Get the registers for the thread.
     */
    count = i860_THREAD_STATE_COUNT;
    (void) thread_get_state(th, i860_THREAD_STATE, (thread_state_t)&ts, &count);
#if MAP_UAREA
    if (p->p_shared_rw->us_emul_regs && ts.pc >= EMULATOR_BASE)
        adjust_emul_regs(p, &ts);
#endif
    *stack_addrp = ts.sp;		/* user's stack pointer */

    /*
     * allocate a fake exception stack frame and fill in current values
     */
    if ((regs = (int *) kalloc(NFAKEREGS * sizeof(int))) != NULL) {
	bzero((caddr_t)regs, NFAKEREGS * sizeof(int));

	/* fast copy */
        bcopy( &ts.r0, &regs[R0], sizeof(struct i860_thread_state) );
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
struct i860_thread_state *ts;
{
    struct emul_stack		emul_regs;
    struct emul_exception_frame emul_regs2;
    int result;
    if (u.u_master_lock)
	master_unlock();
    COPYIN(p, p->p_shared_rw->us_emul_regs, &emul_regs, sizeof emul_regs,
	   result);
    if (result != KERN_SUCCESS)
	goto fail;
    COPYIN(p, emul_regs.usp, &emul_regs2, sizeof emul_regs2, result);
    if (result != KERN_SUCCESS)
	goto fail;
    bcopy((char *)&emul_regs2.u_r0, (char *)ts, 40*4 );
    ts->fsr=emul_regs2.u_fsr;
    ts->pc=emul_regs2.u_fir;
fail:
    if (u.u_master_lock)
	master_lock();
}

#endif	/* MAP_UAREA */
#endif	/* OSF1_SERVER */

struct reg_offsets {
	int	reg_offset;
	int	state_offset;
};
#define	state_off(s)	(int)&(((struct i860_thread_state *)0)->s)
#define reg_off(r)      (ctob(UPAGES) - sizeof(struct i860_thread_state) \
				 + sizeof(int)*(r))
#define regs(r,s)	{ reg_off(r), state_off(s) }

struct reg_offsets ipcreg[] = {
        regs(R0, r0),
        regs(R1, r1),
        regs(SP, sp),
        regs(FP, fp),
        regs(R4, r4),
        regs(R5, r5),
        regs(R6, r6),
        regs(R7, r7),
        regs(R8, r8),
        regs(R9, r9),
        regs(R10, r10),
        regs(R11, r11),
        regs(R12, r12),
        regs(R13, r13),
        regs(R14, r14),
        regs(R15, r15),
        regs(R16, r16),
        regs(R17, r17),
        regs(R18, r18),
        regs(R19, r19),
        regs(R20, r20),
        regs(R21, r21),
        regs(R22, r22),
        regs(R23, r23),
        regs(R24, r24),
        regs(R25, r25),
        regs(R26, r26),
        regs(R27, r27),
        regs(R28, r28),
        regs(R29, r29),
        regs(R30, r30),
        regs(R31, r31),
        regs(F0, f0),
        regs(F1, f1),
        regs(F2, f2),
        regs(F3, f3),
        regs(F4, f4),
        regs(F5, f5),
        regs(F6, f6),
        regs(F7, f7),
        regs(F8, f8),
        regs(F9, f9),
        regs(F10, f10),
        regs(F11, f11),
        regs(F12, f12),
        regs(F13, f13),
        regs(F14, f14),
        regs(F15, f15),
        regs(F16, f16),
        regs(F17, f17),
        regs(F18, f18),
        regs(F19, f19),
        regs(F20, f20),
        regs(F21, f21),
        regs(F22, f22),
        regs(F23, f23),
        regs(F24, f24),
        regs(F25, f25),
        regs(F26, f26),
        regs(F27, f27),
        regs(F28, f28),
        regs(F29, f29),
        regs(F30, f30),
        regs(F31, f31),
        regs(SPC_T, spc_t),
        regs(SPC_KI, spc_ki),
        regs(SPC_KR, spc_kr),
        regs(SPC_MERGE, spc_merge),
#ifndef OLD_SMALL_I860_THREAD_STATE
# define PARAGON
#endif
#ifdef	PARAGON
        regs(PSV_L1, psv_l1[0]),
        regs(PSV_L2, psv_l2[0]),
        regs(PSV_L3, psv_l3[0]),
#else	/* PARAGON */
        regs(PSV_L1, psv_l1),
        regs(PSV_L2, psv_l2),
        regs(PSV_L3, psv_l3),
#endif	/* PARAGON */
        regs(PSV_A1, psv_a1),
        regs(PSV_A2, psv_a2),
        regs(PSV_A3, psv_a3),
        regs(PSV_M1, psv_m1),
        regs(PSV_M2, psv_m2),
        regs(PSV_M3, psv_m3),
        regs(PSV_I1, psv_i1),
        regs(PSV_FSR1, fsr1),
        regs(PSV_FSR2, fsr2),
        regs(FSR, fsr),
        regs(PSR, psr),
        regs(PC, pc),
        regs(DIRBASE, dirbase),
        regs(DB, db),
        regs(EPSR, epsr),
        regs(FPE_FP1, fp1),
        regs(FPE_FP2, fp2),
        regs(FPE_FP3, fp3),
        regs(FPE_FP4, fp4),
        regs(FPE_TRAPPED_OP, fpe_trapped_op),
        regs(FPE_RDEST, fpe_rdest),
        regs(FPE_SRC1, fpe_src1),
        regs(FPE_IEEE_STATUS, fpe_ieee_status)
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
	    struct i860_thread_state	thread_state;
	    unsigned int		count;
	    register int		j;

	    count = i860_THREAD_STATE_COUNT;
	    (void) thread_get_state(p->p_thread,
				i860_THREAD_STATE,
				(thread_state_t)&thread_state,
				&count);

	    *value = 0;

	    for (j = 0; j < NIPCREG; j++) {
		if (ipcreg[j].reg_offset == offset) {
		    *value = *(int *)(((caddr_t)&thread_state)
					+ ipcreg[j].state_offset);
		    break;
		}
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
	struct i860_thread_state	thread_state;
	unsigned int	count;
	register int	j,i,diff;
	boolean_t       is_offset_match = FALSE;

	count = i860_THREAD_STATE_COUNT;
	(void) thread_get_state(p->p_thread,
				i860_THREAD_STATE,
				(thread_state_t)&thread_state,
				&count);

        for (j = 0; j < NIPCREG; j++) {
            if (ipcreg[j].reg_offset == offset)
                break;

            if ( j != (NIPCREG - 1))  {
                diff = ipcreg[j+1].reg_offset - ipcreg[j].reg_offset;
                if( diff == 4 )
                        continue;
                for( i = 4; i < diff; i += 4) {
                        if((ipcreg[j].reg_offset + i ) == offset ) {
                                is_offset_match = TRUE;
                                break;
                        }
                }
            }

            if ( is_offset_match )
                        break;
            is_offset_match = FALSE;
        }


	if (j == NIPCREG) {
	    /* wrong register */
	    return (EIO);
	}

	/* thread_set_state handles checking PSL for valid values */
	*(int *)(((caddr_t)&thread_state)
			+ ipcreg[j].state_offset)
		      = value;
	(void) thread_set_state(p->p_thread,
				i860_THREAD_STATE,
				(thread_state_t)&thread_state,
				count);
	return (0);
}

void ptrace_set_trace(p, new_addr, trace_on)
	struct proc *p;
	int	new_addr;
	int	trace_on;
{
	struct i860_thread_state	thread_state;
	unsigned int		count;

	count = i860_THREAD_STATE_COUNT;
	(void) thread_get_state(p->p_thread,
				i860_THREAD_STATE,
				(thread_state_t)&thread_state,
				&count);

	if (new_addr != 1)
		thread_state.pc = new_addr;

		(void) thread_set_state(p->p_thread,
				i860_THREAD_STATE,
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
	size = i860_THREAD_STATE_COUNT;
	rc = thread_get_state(thread, i860_THREAD_STATE, thread_state_p, &size);
	if (rc != KERN_SUCCESS)
		return rc;
	*pc = ((struct i860_thread_state *)thread_state_p)->pc;
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

	((struct i860_thread_state *)thread_state_p)->pc = pc;
	((struct i860_thread_state *)thread_state_p)->r16 = err;
	rc = thread_set_state(thread, i860_THREAD_STATE, thread_state_p,
						     i860_THREAD_STATE_COUNT);
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
	int	oonstack;
	struct i860_thread_state regs;
	unsigned int		reg_size;

	vm_offset_t	user_start_addr, user_end_addr, kern_addr;
	vm_size_t	user_size;

	register struct sigcontext *kscp;
	vm_offset_t	thp, kthp;

	/*
	 * Get the registers for the thread.
	 */
	reg_size = i860_THREAD_STATE_COUNT;
	(void) thread_get_state(thread, i860_THREAD_STATE,
				(thread_state_t)&regs, &reg_size);

	/*
	 * Choose stack and allocate scp on it
	 */
#if	MAP_UAREA
	share_lock(&p->p_shared_rw->us_lock, p);
#endif	MAP_UAREA
	oonstack = p->p_utask.uu_sigstack.ss_onstack;

	if (!oonstack && (p->p_utask.uu_sigonstack & sigmask(sig))) {
	    vm_offset_t nsp = (vm_offset_t)p->p_utask.uu_sigstack.ss_sp;
	    /* 
	     * create space for the i860_thread_state structure on 
	     * the signal-stack for the kernel assisted SIGRETURN
	     */
	    nsp -= sizeof(struct i860_thread_state);
	    thp = (vm_offset_t)((unsigned)nsp & ~0xf);	
	    nsp = thp - sizeof(struct sigcontext);
	    scp = (struct sigcontext *) nsp;
	    p->p_utask.uu_sigstack.ss_onstack = 1;
	}
	else {
	    vm_offset_t nsp = (vm_offset_t)regs.sp;
	    /* 
	     * create space for the i860_thread_state structure on 
	     * the signal-stack for the kernel assisted SIGRETURN
	     */
	    nsp -= sizeof(struct i860_thread_state);
	    thp = (vm_offset_t)((unsigned)nsp & ~0xf);	
	    nsp = thp - sizeof(struct sigcontext);
	    scp = (struct sigcontext *) nsp;
	}

	/* Make sure 16 byte aligned */
	scp = (struct sigcontext *)((unsigned)scp & ~0xf);

#if	MAP_UAREA
	share_unlock(&p->p_shared_rw->us_lock, p);
#endif	MAP_UAREA

	/*
	 * Copy the signal frame from the user into the kernel,
	 * to modify it.
	 */
	user_start_addr = trunc_page((vm_offset_t)scp);
	user_end_addr   = round_page((vm_offset_t)thp
					+ sizeof(struct i860_thread_state)); 
	user_size = user_end_addr - user_start_addr;

	if (vm_read(p->p_task, user_start_addr, user_size,
			&kern_addr, &user_size) != KERN_SUCCESS)
	    goto error;

	kscp = (struct sigcontext *)((vm_offset_t)scp - user_start_addr
					+ kern_addr);

	/*
	 * Build the argument list for the signal handler.
	 */
/* akp
 * XXX	THIS REALLY NEEDS TO CHANGE SO THAT THE SIGCONTEXT STRUCTURE
 * XXX	MATCHES struct i860_saved_state, OTHERWISE A sigreturn(2)
 * XXX	FROM A SIGNAL HANDLER BACK TO CODE IN THE MIDDLE OF PIPELINING
 * XXX	*WILL* FIND HALF OF ITS FP REGS BLOWN AWAY, AND ALL OF ITS
 * XXX	PIPELINES EMPTY!!!
 */
	bcopy(&regs.r0, &(kscp->sc_regs[0]), 32 * sizeof(int));
	bcopy(&regs.f0, &(kscp->sc_fpregs[0]), 8 * sizeof(int)); /* XXX */
	kscp->sc_psr = (int) regs.psr;
	kscp->sc_fsr = (int) regs.fsr;
	kscp->sc_epsr = (int) regs.epsr;
	kscp->sc_fir = (int) regs.pc;

	/*
	 * Build the rest of the signal context to be used by sigreturn.
	 */
	kscp->sc_onstack = oonstack;
	kscp->sc_mask = mask;

	/* 
	 * let's use the slot for sc_r0 to hold the base address for
	 * the i860_thread_state.
	 * 
	 */
	kscp->sc_r0 = (int) thp;
	kthp = (vm_offset_t)thp - user_start_addr + kern_addr;
	/*
	 * copy the whole i860_thread_state structure
	 */
	bcopy(&regs.r0, kthp, sizeof(struct i860_thread_state));
	/*
	 * Write signal frame and context back to user.
	 */
	if (vm_write(p->p_task, user_start_addr, kern_addr, user_size)
		!= KERN_SUCCESS) {
		(void)vm_deallocate(mach_task_self(), kern_addr, user_size);
		goto error;
	}

	/*
	 * Build the argument list for the signal handler.
	 */
	regs.pc = (int)p->p_utask.uu_sigtramp;
	regs.r16 = sig;
	if (sig == SIGFPE || sig == SIGSEGV || sig == SIGILL ||
	    sig == SIGBUS || sig == SIGTRAP) {
		regs.r17 = code;
#ifdef	TNC
	} else if (sig == SIGMIGRATE) {
		regs.r17 = p->p_sig_arg;
#endif	/* TNC */
	} else
		regs.r17 = 0;
	regs.r18 = (int)scp;
	regs.r19 = (int)sig_hdlr;
	regs.sp = (int)scp;

	/* Clear bits in fsr that might cause spurious FP traps in the
	   signal handler */
	regs.fsr &= ~(FSR_SI | FSR_SE | FSR_MU | FSR_MO | FSR_MI | FSR_AU |
		FSR_AO | FSR_AI ) ;

	/*
	 * Write changed registers back to thread.
	 */
	(void) thread_set_state(thread, i860_THREAD_STATE,
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
	struct i860_thread_state *regs = (struct i860_thread_state *)new_state;

	if (new_state_count != i860_THREAD_STATE_COUNT)
	    return (FALSE);

	regs->r16 = parent_pid;
	regs->r17 = rc;

	(void) thread_set_state(child_thread, i860_THREAD_STATE,
				new_state, new_state_count);
	return (TRUE);
}

#include <i860/debug_portal.h>

/*
 * Intel debug interface:
 *      enable syscall tracing from user mode
 *
 * inputs:
 *   uap->func == DP_syscalltrac
 *      -1              trace all syscalls
 *       0               disable syscall tracing
 *      >0              syscall trace for this pid ONLY.
 */
debug_portal(p, args, retval)
        struct proc     *p;
        void            *args;
        int             *retval;
{
        int     rc=0;
        struct args {
                int     func;
                int     opt;
        } *uap = (struct args *) args;
        extern int syscalltrace;

        switch( uap->func ) {

        case    DP_syscalltrace:
                if ( uap->opt < -1 )
                        rc = EINVAL;
                else {
                        rc = syscalltrace;      /* return previous value */
                        syscalltrace = uap->opt;
                }
                break;

        case    DP_prof_start:
                break;

        case    DP_prof_stop:
                break;

        case    DP_prof_clear_data:
                break;

        case    DP_prof_return_data:
                break;

        default:
                rc = EINVAL;
                break;
        }

        *retval = rc;   /* syscall return value */

        return TRUE;
}

/*
 * return boot parameters (server environment) to the caller
 *
 * inputs:
 *	buf	= adrs.
 *	size	= byte size of buffer.
 */

boot_parms(p, args, retval)
        struct proc     *p;
        void            *args;
        int             *retval;
{
        int     rc=0;
        struct args {
                int     buf;
                int     size;
        } *uap = (struct args *) args;

        return TRUE;
}

/*
 *  _860_cpu_type returns 1 for XR, 2 for XP.
 */
int
_i860_cpu_type()
{
    asm(" ld.c epsr,r16");
    asm(" and  0xff,r16,r16");
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
