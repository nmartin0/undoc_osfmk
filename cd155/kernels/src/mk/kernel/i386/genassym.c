/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: genassym.c,v $
 * Revision 1.5  1994/11/18  20:32:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:23:51  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:22:29  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:22:08  dleslie
 * First R1_0 release
 *
 * Revision 2.12.7.2  92/03/03  16:15:30  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  11:10:26  jeffreyh]
 * 
 * Revision 2.13  92/01/03  20:06:12  dbg
 * 	Add DISP_MIN.
 * 	[91/10/31            dbg]
 * 
 * Revision 2.12.7.1  92/02/18  18:45:09  jeffreyh
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.12  91/07/31  17:36:13  dbg
 * 	Add microsecond timing.
 * 
 * 	Save user registers in PCB and switch to separate kernel stack
 * 	on entry.
 * 	[91/07/30  16:50:29  dbg]
 * 
 * Revision 2.11  91/06/19  11:55:05  rvb
 * 	cputypes.h->platforms.h
 * 	[91/06/12  13:44:45  rvb]
 * 
 * Revision 2.10  91/05/14  16:08:23  mrt
 * 	Correcting copyright
 * 
 * Revision 2.9  91/05/08  12:37:41  dbg
 * 	Add definitions for multiprocessors and for Sequent Symmetry.
 * 	Change '#define' to '.set' to help fool GCC preprocessor.
 * 	[91/04/26  14:35:03  dbg]
 * 
 * Revision 2.8  91/03/16  14:44:15  rpd
 * 	Removed k_ipl.
 * 	[91/03/01            rpd]
 * 	Added TH_SWAP_FUNC.
 * 	[91/02/24            rpd]
 * 
 * 	Added PCB_SIZE.
 * 	[91/02/01            rpd]
 * 
 * Revision 2.7  91/02/05  17:11:56  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:22  mrt]
 * 
 * Revision 2.6  91/01/09  22:41:19  rpd
 * 	Removed user_regs, k_stack_top.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.5  91/01/08  15:10:36  rpd
 * 	Reorganized the pcb.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.4  90/12/04  14:45:58  jsb
 * 	Changes for merged intel/pmap.{c,h}.
 * 	[90/12/04  11:15:00  jsb]
 * 
 * Revision 2.3  90/08/27  21:56:44  dbg
 * 	Use new names from new seg.h.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.2  90/05/03  15:27:32  dbg
 * 	Created.
 * 	[90/02/11            dbg]
 * 
 */

#include <platforms.h>
#include <cpus.h>
#include <mach_kdb.h>
#include <stat_time.h>

/*
 * Pass field offsets to assembly code.
 */
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/syscall_emulation.h>
#include <i386/thread.h>
#include <mach/i386/vm_param.h>
#include <i386/seg.h>
#include <i386/pmap.h>
#include <i386/tss.h>
#if	MACH_KDB
#include <sys/reboot.h>
#endif
#if	NCPUS > 1
#include <i386/mp_desc.h>
#endif
#ifdef	SYMMETRY
#include <sqt/slic.h>
#include <sqt/intctl.h>
#endif
#ifdef	CBUS
#include <cbus/cbus.h>
#endif	CBUS

/*ARGSUSED*/
main(argc,argv)
	int	argc;
	char	**argv;
{
	struct i386_interrupt_state	*iis = (struct i386_interrupt_state *)0;
	struct i386_kernel_state	*iks = STACK_IKS(0);
	struct i386_saved_state		*iss = (struct i386_saved_state *)0;
	pcb_t				pcb = (struct pcb *)0;
	struct i386_fpsave_state	*ifps = (struct i386_fpsave_state *)0;
	thread_t			th = (thread_t)0;
	task_t				task = (task_t)0;
	eml_dispatch_t			eml = (eml_dispatch_t)0;
	struct i386_tss			*tss = (struct i386_tss *)0;
#if	NCPUS > 1
	struct mp_desc_table		*mpt = (struct mp_desc_table *)0;
#endif
#if	!STAT_TIME
	struct timer			*timer = (struct timer *)0;
#endif
#ifdef	SYMMETRY
	struct cpuslic			*slic = (struct cpuslic *)0;
	struct bin_header		*bh = (struct bin_header *)0;
#endif

	printf("\t.set\tTH_PCB,\t%d\n", &th->pcb);
	printf("\t.set\tTH_TASK,\t%d\n", &th->task);
	printf("\t.set\tTH_RECOVER,\t%d\n", &th->recover);
	printf("\t.set\tTH_KERNEL_STACK,\t%d\n", &th->kernel_stack);
	printf("\t.set\tTH_SWAP_FUNC,\t%d\n", &th->swap_func);

	printf("\t.set\tTASK_EMUL,\t%d\n", &task->eml_dispatch);

	printf("\t.set\tDISP_MIN,\t%d\n", &eml->disp_min);
	printf("\t.set\tDISP_COUNT,\t%d\n", &eml->disp_count);
	printf("\t.set\tDISP_VECTOR,\t%d\n", &eml->disp_vector[0]);

	printf("\t.set\tKSS_EBX,\t%d\n", &iks->k_ebx);
	printf("\t.set\tKSS_ESP,\t%d\n", &iks->k_esp);
	printf("\t.set\tKSS_EBP,\t%d\n", &iks->k_ebp);
	printf("\t.set\tKSS_EDI,\t%d\n", &iks->k_edi);
	printf("\t.set\tKSS_ESI,\t%d\n", &iks->k_esi);
	printf("\t.set\tKSS_EIP,\t%d\n", &iks->k_eip);

	printf("\t.set\tIKS_SIZE,\t%d\n", sizeof(struct i386_kernel_state));
	printf("\t.set\tIEL_SIZE,\t%d\n", sizeof(struct i386_exception_link));

	printf("\t.set\tPCB_FPS,\t%d\n", &pcb->ims.ifps);

	printf("\t.set\tFP_VALID,\t%d\n", &ifps->fp_valid);
	printf("\t.set\tFP_SAVE_STATE,\t%d\n", &ifps->fp_save_state);

	printf("\t.set\tR_CS,\t%d\n", &iss->cs);
	printf("\t.set\tR_UESP,\t%d\n", &iss->uesp);
	printf("\t.set\tR_EAX,\t%d\n", &iss->eax);
	printf("\t.set\tR_TRAPNO,\t%d\n", &iss->trapno);
	printf("\t.set\tR_ERR,\t%d\n", &iss->err);
	printf("\t.set\tR_EFLAGS,\t%d\n", &iss->efl);
	printf("\t.set\tR_EIP,\t%d\n", &iss->eip);
	printf("\t.set\tR_CR2,\t%d\n", &iss->cr2);

	printf("\t.set\tI_EIP,\t%d\n", &iis->eip);
	printf("\t.set\tI_CS,\t%d\n", &iis->cs);
	printf("\t.set\tI_EFL,\t%d\n", &iis->efl);

	printf("\t.set\tNBPG,\t%d\n", I386_PGBYTES);
	printf("\t.set\tVM_MIN_ADDRESS,\t%d\n", VM_MIN_ADDRESS);
	printf("\t.set\tVM_MAX_ADDRESS,\t%d\n", VM_MAX_ADDRESS);
	printf("\t.set\tKERNELBASE,\t%d\n", VM_MIN_KERNEL_ADDRESS);
	printf("\t.set\tKERNEL_STACK_SIZE,\t%d\n", KERNEL_STACK_SIZE);

	printf("\t.set\tPDESHIFT,\t%d\n", PDESHIFT);
	printf("\t.set\tPTESHIFT,\t%d\n", PTESHIFT);
	printf("\t.set\tPTEMASK,\t%d\n", PTEMASK);

	printf("\t.set\tPTE_PFN,\t%d\n", INTEL_PTE_PFN);
	printf("\t.set\tPTE_V,\t%d\n", INTEL_PTE_VALID);
	printf("\t.set\tPTE_W,\t%d\n", INTEL_PTE_WRITE);
	printf("\t.set\tPTE_INVALID,\t%d\n", ~INTEL_PTE_VALID);

	printf("\t.set\tIDTSZ,\t%d\n", IDTSZ);
	printf("\t.set\tGDTSZ,\t%d\n", GDTSZ);
	printf("\t.set\tLDTSZ,\t%d\n", LDTSZ);

	printf("\t.set\tKERNEL_CS,\t%d\n", KERNEL_CS);
	printf("\t.set\tKERNEL_DS,\t%d\n", KERNEL_DS);
	printf("\t.set\tKERNEL_TSS,\t%d\n", KERNEL_TSS);
	printf("\t.set\tKERNEL_LDT,\t%d\n", KERNEL_LDT);

	printf("\t.set\tPTES_PER_PAGE,\t%d\n", NPTES);
	printf("\t.set\tINTEL_PTE_KERNEL,\t%d\n",
		INTEL_PTE_VALID|INTEL_PTE_WRITE);

	printf("\t.set\tKERNELBASEPDE,\t%d\n",
		(VM_MIN_KERNEL_ADDRESS >> PDESHIFT)
		*sizeof(pt_entry_t));
#ifdef	CBUS
	printf("\t.set\tCBUS_START_PDE,\t%d\n",
		(CBUS_START >> PDESHIFT)
		*sizeof(pt_entry_t));
#endif	CBUS

	printf("\t.set\tTSS_ESP0,\t%d\n", &tss->esp0);
	printf("\t.set\tTSS_SS0,\t%d\n", &tss->ss0);

#if	MACH_KDB
	printf("\t.set\tRB_KDB,\t%d\n", RB_KDB);
#endif	MACH_KDB

	printf("\t.set\tK_TRAP_GATE,\t%d\n",
		ACC_P|ACC_PL_K|ACC_TRAP_GATE);
	printf("\t.set\tU_TRAP_GATE,\t%d\n",
		ACC_P|ACC_PL_U|ACC_TRAP_GATE);
	printf("\t.set\tK_INTR_GATE,\t%d\n",
		ACC_P|ACC_PL_K|ACC_INTR_GATE);
	printf("\t.set\tK_TSS, %d\n",
		ACC_P|ACC_PL_K|ACC_TSS);

#if	NCPUS > 1
	printf("\t.set\tMP_GDT,\t%d\n", &mpt->gdt[0]);
	printf("\t.set\tMP_IDT,\t%d\n", &mpt->idt[0]);
	printf("\t.set\tINTSTACK_SIZE,\t%d\n", INTSTACK_SIZE);
#endif	NCPUS > 1
#if	!STAT_TIME
	printf("\t.set\tLOW_BITS,\t%d\n", &timer->low_bits);
	printf("\t.set\tHIGH_BITS,\t%d\n", &timer->high_bits);
	printf("\t.set\tHIGH_BITS_CHECK,\t%d\n", &timer->high_bits_check);
	printf("\t.set\tTIMER_HIGH_UNIT,\t%d\n", TIMER_HIGH_UNIT);
	printf("\t.set\tTH_SYS_TIMER,\t%d\n", &th->system_timer);
	printf("\t.set\tTH_USER_TIMER,\t%d\n", &th->user_timer);
#endif
#ifdef	SYMMETRY
	printf("\t.set\tVA_SLIC,\t%d\n", VA_SLIC);
	printf("\t.set\tVA_ETC,\t%d\n", VA_ETC);
	printf("\t.set\tSL_PROCID,\t%d\n", &slic->sl_procid);
	printf("\t.set\tSL_LMASK,\t%d\n", &slic->sl_lmask);
	printf("\t.set\tSL_BININT,\t%d\n", &slic->sl_binint);
	printf("\t.set\tSL_B0INT,\t%d\n", &slic->sl_b0int);
	printf("\t.set\tBH_SIZE,\t%d\n", &bh->bh_size);
	printf("\t.set\tBH_HDLRTAB,\t%d\n", &bh->bh_hdlrtab);
#endif	SYMMETRY

	return (0);
}

