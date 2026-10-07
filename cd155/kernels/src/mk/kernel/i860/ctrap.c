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
 * Copyright (c) 1991 Carnegie Mellon University
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
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include <cpus.h>
#include <mach_kdb.h>
#include <ctrap_history.h>

#include <i860/psl.h>
#include <i860/trap.h>

#include <kern/cpu_number.h>
#include <i860/cpu_master.h>
#include <i860paragon/rpm.h>	/* RPMSOFT counters */

#include <intel/pmap.h>

#include <kern/thread.h>
#include <kern/ast.h>
#include <mach/machine/thread_status.h>

#include <i860/fpe/fpe.h>

#if	PARAGON860
#include <i860paragon/dp.h>
#include <i860paragon/led.h>	/* fptrap_verbose */
#include <i860paragon/ecc.h>	/* fptrap_verbose */

#if	TIME_TRACE
#include <sys/types.h>
#include <i860paragon/time_trace.h>
#endif	/* TIME_TRACE */
#endif	PARAGON860


#define STATIC

#define SYS_CALL	0x47e0f800	/* trap r31,r31,r0 */
#define	SYS_CALL2	0x44008000      /* trap r16,r0,r0 */
#define BRK_TRAP	0x44000000	/* trap r0,r0,r0 */
#define IEEE_FP_TRAP	0x44200000	/* trap r0,r1,r0 - IEEE FP exception */
#define	RANGE_CHECK	0x44400000	/* trap r0,r2,r0 - range check error */
#define SIGNAL_MA 	0x44600000	/* trap r0,r3,r0 - signal misaligned access */
#define IEEE_DISABLE	0x44800000	/* trap r0,r4,r0 - signal any FPE */
#define IEEE_ENABLE	0x44a00000	/* trap r0,r5,r0 - signal only IEEE FPE */

					/* trap r0,r16,r0  - through ... */
					/* trap r0,r31,r0  - unspecified */

#define	IEEE_GET_FPMASK	0x46000000	/* trap r0,r16,r0 - get status mask */
#define	IEEE_SET_FPMASK	0x46200000	/* trap r0,r17,r0 - set status mask */

#define	DONT_SIGNAL_MA	0x46400000	/* trap r0,r18,r0 - dont signal misaligned access */
#define	SIGNAL_PFLDQ	0x46600000	/* trap r0,r19,r0 - signal pfld.q's on XR */
#define	DONT_SIGNAL_PFLDQ 0x46a00000	/* trap r0,r21,r0 - don't signal
					 * pfld.q's on XR.
					 */

/*
 *	Experimental traps...
 *
 *	TRAP_FLUSH	- user-mode access to the flush() routine.
 *	TRAP_SETJMP	- copy the saved state into a buffer, return 0.
 *	TRAP_LONGJMP	- restore the saved state, return value of r17.
 */
#define TRAP_FLUSH	0x46c00000	/* trap r0,r22,r0 flush() */
#define TRAP_SETJMP	0x46e00000	/* trap r0,r23,r0 like setjmp() */
#define TRAP_LONGJMP	0x47000000	/* trap r0,r24,r0 like longjmp() */
/* Note:                                   trap r0,r25,r0 used below */
#define	TRAP_SIGRET	0x47400000	/* trap r0,r26,r0 sigreturn trap */
#define	TRAP_UNSNOOPED	0x47600000	/* trap r0,r27,r0 unsnooped task XXX */
#define TIME_TRACE_TRAP	0x47800000	/* trap r0,r28,r0 time tracing */

#define	INTOVR_INST	0x4c000004	/* intovr - trap on overflow in epsr */

#define	PFLDQ_INST	0x60000004	/* pfld.q 0(r0),f0 */
#define	PFLDQ_MASK	0xf8000004

#if	iPSC860
#define LED_CHAR	0x44c00000	/* trap r0,r6,r0 XXX should remove */
#endif	iPSC860

#if	PARAGON860
#define TRAP_TRACE	0x44c00000	/* trap r0,r6,r0 XXX perf. work */
#define START_WATCHDOG	0x44e00000	/* trap r0,r7,r0 - Enable watchdog */
#define TRAP_BOOTMAGIC	0x47200000	/* trap r0,r25,r0 - get big bootmagic */
#endif	PARAGON860

#define OP_LOAD		0		/* op was a "load" */
#define OP_STORE	T_PF_WRITE	/* op was a "store" */

boolean_t	page_fault_huh_verbose = 0;
boolean_t	impossible_DAT_verbose = 0;
unsigned int	fptrap_verbose;

STATIC int	lockrestart();		/* forward */
STATIC void	dattrap();		/* forward */
STATIC void	datld();		/* forward */
STATIC void	datst();		/* forward */
STATIC void	fptrap();		/* forward */
STATIC void	insttrap();		/* forward */
STATIC void	badlock();		/* forward */

#if	CTRAP_HISTORY

#define	CTRAP_COUNT(val) val[cpu_number()]++

unsigned long	ctrap_nobits[NCPUS];	/* total reset traps (!) */
unsigned long	ctrap_pagefaults[NCPUS];/* total calls to page_fault() */
unsigned long	ctrap_unaligned[NCPUS];	/* unaligned load or store */
unsigned long	ctrap_tlb_miss[NCPUS];	/* inconsistant TLB's */

#else	/* CTRAP_HISTORY */

/* interesting trap accounting disappears */ 
#define	CTRAP_COUNT(val)

#endif	/* CTRAP_HISTORY */

unsigned long	last_unknown_psr_nobits[NCPUS];

unsigned long	unknown_fp_traps[NCPUS];
STATIC void ieee_set_fpmask();
STATIC void ieee_get_fpmask();

/* for pretty_lights() */
unsigned long	previous_user_count[NCPUS];

STATIC void	i860_trap_setjmp();
STATIC void	i860_trap_longjmp();
STATIC void	i860_trap_sigreturn();

/*
 * format of instructions
 */
struct igen {
	unsigned int	ig_offset:11;	/* immediate, offset, or null */
	unsigned int	ig_src1:5;	/* register source 1 */
	unsigned int	ig_dest:5;	/* register destination */
	unsigned int	ig_src2:5;	/* register source 2 */
	unsigned int	ig_op:6;	/* opcode/i */
};
struct iimed {
	int		ii_offset:16;	/* immediate */
	unsigned int	ii_dest:5;	/* register destination */
	unsigned int	ii_src2:5;	/* register source 2 */
	unsigned int	ii_op:6;	/* opcode/i */
};

#define	OVERFLOW_CHECK	0

#if	CTRAP_HISTORY
#define	CTRAP_HISTLEN	32

int	ctrap_history_enabled = 1;

struct ctrap_history {
	unsigned long	ch_trapn;	/* snapshot of ctrap_total */
	unsigned long	ch_regs;	/* pointer to the saved state */
	unsigned long	ch_psr;		/* psr at time of trap */
	unsigned long	ch_fir;		/* fir at time of trap */
#if	PARAGON860
	/* XXX these can be removed in the future */
	unsigned long	ch_time;	/* timestamp */
	int		ch_tracing;	/* is tracing on? */
	int		ch_wascow;	/* was it a cow? */
	int		ch_waszer;	/* was it a zero fill? */
	int		ch_wasmod;	/* set the dirty bit? */
	int		ch_wasref;	/* set the ref bit under lock? */
#endif	PARAGON860
};

struct ctrap_history ctrap_history[CTRAP_HISTLEN];
int ctrap_history_next;

int	trap_tracing = 0;
int	trap_tracing_stop = 0;

#if	PARAGON860
#include <mach/vm_statistics.h>
int	trap_cow_snap = 0;
int	trap_zer_snap = 0;
int	trap_mod_snap = 0;
int	trap_ref_snap = 0;
#endif	PARAGON860

void ctrap_log_exit(regs)
	register struct i860_saved_state *regs;
{
	struct ctrap_history *ch;
	int mycpu = cpu_number();

	if ((ctrap_history_enabled == 0) || (mycpu != master_cpu))
		return;

	ch = &ctrap_history[ctrap_history_next];
	ch->ch_trapn = RPMSOFT_STAT(mycpu, rpms_alltraps);
	ch->ch_regs = (unsigned long) regs;
	ch->ch_psr = (unsigned long) regs->psr;
	ch->ch_fir = (unsigned long) regs->pc;
#if	PARAGON860
	ch->ch_time = inl(DP_EPOCH_LO);
	ch->ch_tracing = trap_tracing;
	{
		ch->ch_wascow = trap_cow_snap != vm_stat.cow_faults;
		ch->ch_waszer = trap_zer_snap != vm_stat.zero_fill_count;
		ch->ch_wasmod = trap_mod_snap != RPMSOFT_STAT(mycpu, rpms_notdirty);
		ch->ch_wasref = trap_ref_snap != RPMSOFT_STAT(mycpu, rpms_notref);

	}
#endif	PARAGON860

	if (++ctrap_history_next >= CTRAP_HISTLEN) {
		ctrap_history_next = 0;
	}
}

#endif	CTRAP_HISTORY


/*
 * Call the proper handlers.
 * Big overhaul for handoff scheduling support.
 * -- Andy Pfiffer, Intel SSD
 */

void ctrap(regs)
	register struct i860_saved_state *regs;
{
	unsigned	psr;
	int		mycpu = cpu_number();
	extern int	mcmsg_mp_enable;
	int		i_am_mcp =  ((cpu_number1() == 1) && mcmsg_mp_enable);

	regs->save_r15 = 0; /* save_r15 is flag for emulation has taken place */
	RPMSOFT_STAT_INC(mycpu, rpms_alltraps);
	psr = regs->psr;

	if (psr & PSR_PU) {
		previous_user_count[mycpu]++;
	}
#if	CTRAP_HISTORY
	if ( ctrap_history_enabled && (mycpu == master_cpu) ) {
		struct ctrap_history *ch;

		ch = &ctrap_history[ctrap_history_next];
		ch->ch_trapn = RPMSOFT_STAT(mycpu, rpms_alltraps);
		ch->ch_regs = (unsigned long) regs;
		ch->ch_psr = (unsigned long) regs->psr;
		ch->ch_fir = (unsigned long) regs->pc;
#if	PARAGON860
		ch->ch_time = inl(DP_EPOCH_LO);
		ch->ch_tracing = trap_tracing;
		{
			trap_cow_snap = vm_stat.cow_faults;
			trap_zer_snap = vm_stat.zero_fill_count;
			trap_mod_snap = RPMSOFT_STAT(mycpu, rpms_notdirty);
			trap_ref_snap = RPMSOFT_STAT(mycpu, rpms_notref);
		}
#endif	PARAGON860
		ctrap_history_next++;

		if (ctrap_history_next >= CTRAP_HISTLEN) {
			ctrap_history_next = 0;
		}
	}
#endif	CTRAP_HISTORY
#if	OVERFLOW_CHECK
	/*
 	 * stack check.
	 * for now, doesn't apply to the MCP
	 */
	{
		extern int	kernel_stack_check();

		if ( !(i_am_mcp) && (kernel_stack_check(regs))
		{
			panic("kernel stack overflow");
		}
	}
#endif	OVERFLOW_CHECK

#if	i860XP
	/*
	 * Trap was due to parity or bus error.
	 * (GP and MP node hardware is wired to never give bus error)
	 */
	if ((regs->epsr & (EPSR_PEF | EPSR_BEF)) != 0)
	{
		extern void handle_parity_trap(struct i860_saved_state *regs);

		/*
		 * if handle_parity_trap() returns, we continue.
		 */
		handle_parity_trap(regs);

		/*
		 * clear the PEF & BEF bits in the regs EPSR so they'll
		 * be restored cleared and will re-enable this trap.
		 */
		regs->epsr &= ~(EPSR_PEF | EPSR_BEF);

		return;
	}
#endif	i860XP

#if	MCMSG && NCPUS == 1
	/*
	 * MCP took a trap.
	 * uni-processor kernel stops it here.
	 * MP kernel will treat it (nearly) like any other cpu.
	 */
	if (i_am_mcp)
	{
		/*
		 * If breakpoint trap
		 */
		if ((psr & PSR_IT) &&
			(*((unsigned int *) regs->pc) == BRK_TRAP))
		{
			extern void gimmeabreak();

			mcmsg_msgp_assert("trap.c MCP breakpoint",__LINE__);
			if ((void (*)()) regs->pc == gimmeabreak)
				regs->pc += 4;
		} else
		/*
		 * all other MCP traps
		 */
		{
			mcmsg_msgp_assert("trap.c MCP trap",__LINE__);
		}
		return;
	}
#endif	/* MCMSG && NCPUS == 1 */

	assert((regs->epsr & EPSR_WP) == EPSR_WP);

	/*
	 * instruction trap?
	 * Reasons for an instruction trap:
	 *	1) "trap" instruction
	 *	2) "intovr" instruction
	 *	3) lock/unlock protocol violation
	 *	4) unimplemented instruction (ldio,stdio,scyc,ldint,pfld.q)
	 */
	if (psr & PSR_IT) {
		RPMSOFT_STAT_INC(mycpu, rpms_it);
		/*
		 * the PRM says that trap and intovr instructions must not be
		 * used within a locked sequence.
		 */
		if (regs->epsr & EPSR_IL) {
			badlock(regs);
		} else {
			insttrap(regs);
		}
	}

	/*
	 * instruction access trap?
	 * Reasons for an instruction-access trap:
	 *	1) instruction fetch from a not-present page (P == 0).
	 *	2) user mode instruction fetch of supervisor mode page.
	 *	3) fetch from an unaccessed page (A == 0) during a
	 *	   locked sequence.
	 *
	 * XXX 3) isn't handled yet...
	 */
	else if (psr & PSR_IAT) {
		RPMSOFT_STAT_INC(mycpu, rpms_iat);
		regs->vaddr = regs->pc;
		regs->code = (regs->psr & PSR_PU) ? T_PF_USER : 0;
		regs->trapno = T_PAGE_FAULT;
		trap(regs);
	}

	/*
	 * data access trap?
	 * Reasons for a data-access trap:
	 *	1) the operand is in a not-present page (P == 0)
	 *	2) write to a clean page (D == 0).
	 *	3) user/supervisor protection violation.
	 *	4) fetch from an unaccessed page (A == 0) during a
	 *	   locked sequence.
	 *	5) a memory operand is misaligned.
	 *	6) the address in the db register is equal to one of the
	 *	   addresses spanned by the operand.
	 */
	else if (psr & PSR_DAT) {
		dattrap(regs);
	}

	/*
	 * floating-point trap?
	 */
	else if (psr & PSR_FT) {
		fptrap(regs);
	}

	/*
	 * interrupt?
	 */
	else if (psr & PSR_IN) {
		extern void interrupt(struct i860_saved_state *regs);

		RPMSOFT_STAT_INC(mycpu, rpms_in);
		interrupt(regs);
	}

	/*
	 * no trap bits are set -- this is most curious...
	 */
	if ((psr & (PSR_IT|PSR_IN|PSR_IAT|PSR_DAT|PSR_FT)) == 0) {
		CTRAP_COUNT(ctrap_nobits);
		/* RED_ON(RED_DEBUG); */
		last_unknown_psr_nobits[mycpu] = psr;
	}


	/*
	 * Are we trying to return to user-mode?
	 */
	if (regs->psr & PSR_PU) {
		/*
		 * with interrupts disabled?
		 */
		if ( !(regs->psr & PSR_PIM))
			panic("bad pu/pim in ctrap");

		thread_ctrap_return();
	}
}


/*
 * an instruction trap, usually a system call or breakpoint
 */
STATIC void
insttrap(regs)
	register struct i860_saved_state *regs;
{
	unsigned int	instr;

	/*
	 * this shouldn't trap because we executed the instruction
	 * not too long ago.
	 */
	instr = *((unsigned int *) regs->pc);
	/*
	 * if DIM then next instruction (the core instruction)
	 */
	if (regs->psr & PSR_DIM) {
		instr = *((unsigned long *) (regs->pc + 4));
	}

	switch (instr) {
	case SYS_CALL:
	case SYS_CALL2:
		regs->vaddr = regs->pc;
		regs->trapno = T_SYSCALL;
		trap(regs);
		break;

	case BRK_TRAP:
		regs->vaddr = regs->pc;
		regs->trapno = T_BPTFLT;
		trap(regs);
		break;

	case IEEE_DISABLE:	/* disable IEEE arithmetic */
		current_thread()->pcb->flags &= ~(PCB_IEEE_FP);
		regs->pc += 4;
		break;

	case IEEE_ENABLE:	/* enable IEEE arithmetic */
		current_thread()->pcb->flags |= PCB_IEEE_FP;
		regs->pc += 4;
		break;

	case IEEE_SET_FPMASK:	/* set IEEE FP mask word */
		ieee_set_fpmask(regs);
		break;

	case IEEE_GET_FPMASK:	/* get IEEE FP mask word */
		ieee_get_fpmask(regs);
		break;

	case IEEE_FP_TRAP:	/* signal ieee Floating Point trap (SIGFPE) */
		regs->code = instr;
		regs->vaddr = regs->pc;
		regs->trapno = T_FLOATING_POINT_ERROR;
		trap(regs);
		break;

	case INTOVR_INST:	/* intovr instruction */
		regs->code = instr;
		regs->vaddr = regs->pc;
		regs->trapno = T_OVERFLOW;
		trap(regs);
		break;

#if	iPSC860
	case LED_CHAR:		/* user-mode led_char() */
	  {
		led_char(regs->r16);
		regs->pc += 4;
		break;
	  }
#endif	iPSC860
#if	PARAGON860
#if	0
	case TRAP_UNSNOOPED:
		paragon_set_unsnooped_pmap(vm_map_pmap(current_thread()->task->map));
		regs->pc += 4;
		break;
#endif	0

	case START_WATCHDOG:	/* Start the watchdog program */
		fscan_start_watchdog(regs->r16);
		regs->pc += 4;
		break;

	case TRAP_BOOTMAGIC:
		get_bootmagic(regs);
		regs->pc += 4;
		break;

	case TRAP_TRACE:
#if	CTRAP_HISTORY
		if (trap_tracing) {
			trap_tracing = 0;
			if (trap_tracing_stop) {
				kdb_kintr(regs);
			}
		} else {
			trap_tracing = 1;
		}
#endif
		regs->pc += 4;
		break;

#if	TIME_TRACE

	/*
	 * handle rkl's time tracing trap interface. Good routine timings
	 * come thru here.
	 */
	case TIME_TRACE_TRAP: {
		extern	void	trace_calibrate();
		extern	void	ttrace_on();
		extern	void	ttrace_off();
		extern	int	ttrace_trap_get();
		int		size;

		switch( regs->r16 ) {
		  case TR_RESET:
			db_printf("mk: time trace clear\n");
			trace_calibrate();
			regs->r16 = 0;
			break;

		  case TR_SET_THREAD:
			db_printf("mk: time trace set thread\n");
			ttrace_set_thread( regs->r17 );
			regs->r16 = 0;
			break;

		  case TR_START:
			db_printf("mk: time trace on, %d\n",regs->r17);
			ttrace_set_thread( regs->r17 );
			ttrace_on();
			regs->r16 = 0;
			break;

		  case TR_STOP:
			db_printf("mk: time trace off\n");
			ttrace_off();
			regs->r16 = 0;
			break;

		  default:
			break;
		}

		regs->pc += 4;
		break;
	}
#endif	/* TIME_TRACE */

#endif	PARAGON860

	case SIGNAL_MA:		/* signal misaligned access */
		regs->r16 = current_thread()->pcb->flags; /* DEBUG */
		current_thread()->pcb->flags |= PCB_SIGNAL_MA;
		regs->pc += 4;
		break;

	case DONT_SIGNAL_MA:		/* don't signal misaligned access */
		regs->r16 = current_thread()->pcb->flags; /* DEBUG */
		current_thread()->pcb->flags &= ~PCB_SIGNAL_MA;
		regs->pc += 4;
		break;

	case SIGNAL_PFLDQ:		/* signal pfld.q access */
		regs->r16 = current_thread()->pcb->flags; /* DEBUG */
		current_thread()->pcb->flags |= PCB_SIGNAL_PFLDQ;
		regs->pc += 4;
		break;

	case TRAP_FLUSH:	/* simulate a flush() from user-mode */
		flush();
		regs->pc += 4;
		break;

	case TRAP_SETJMP:	/* mk-assisted setjmp() */
		i860_trap_setjmp(regs);
		break;

	case TRAP_LONGJMP:	/* mk-assisted longjmp() */
		i860_trap_longjmp(regs);
		break;

	case TRAP_SIGRET:	/* kernel assisted sigreturn */
		i860_trap_sigreturn(regs);
		break;

	case DONT_SIGNAL_PFLDQ:		/* don't signal pfld.q access */
		regs->r16 = current_thread()->pcb->flags; /* DEBUG */
		current_thread()->pcb->flags &= ~PCB_SIGNAL_PFLDQ;
		regs->pc += 4;
		break;

	/*case RANGE_CHECK: same as default */
	default:
#if	i860XP
		regs->trapno = T_INVALID_OPCODE;
		regs->code = instr;
		regs->vaddr = regs->pc;
		trap(regs);
		break;
#else	i860XP
        	if (regs->psr & PSR_DIM) { /* if DIM then next instruction */
			instr = *((unsigned long *) (regs->pc + 4));
        	}
		if ( (instr & PFLDQ_MASK) == PFLDQ_INST ) {
			if ( emulate_pfldq(regs,instr) == 0 )
				regs->save_r15 = 1;   /* ???? */
				return;
		}
		else {
			regs->trapno = T_INVALID_OPCODE;
			regs->code = instr;
			regs->vaddr = regs->pc;
		}
		trap(regs);
		break;
#endif	i860XP
	}
}


STATIC get_bootmagic(regs)
	register struct i860_saved_state *regs;
{
	char	*src, *dst, *ipsc_boot_environ();
	int	len;

	src = ipsc_boot_environ();
	dst = (char *) regs->r16;
	len = strlen(src);
	copyout(src, dst, len);
	regs->r16 = len;
}


/*
 * restart a locked sequence by scanning backwards
 * (up to about 30 instructions) for the lock instruction.
 */
#define BACKUP	33
#define LOCKINSTR	0x4C000001

STATIC int lockrestart(regs)
	register struct i860_saved_state *regs;
{
	unsigned int	*ip, instr, umode;
	int	i, lock;

	ip = (unsigned int *) regs->pc;
	lock = LOCKINSTR;
	umode = regs->psr & PSR_PU;
	for (i = 0; i < BACKUP; i++) {
		if (umode)
			copyin(ip, &instr, 4);
		else
			instr = *ip;
		if (instr == lock) {
			regs->pc = (unsigned) ip;
			return 0;
		}
		ip--;
	}

	/*
	 * can't find a "lock" in the recent past...
	 * maybe they branched out of a locked sequence??!!
	 */
	return -1;
}


/*
 * set the A bit of the PDE
 */
STATIC void ctrap_pde_set_accessed(pdep, pde, addr)
	pt_entry_t	*pdep, pde;
	vm_offset_t	addr;
{
	RPMSOFT_STAT_INC(cpu_number(), rpms_notref);
	*pdep = pde | INTEL_PTE_REF;
#if	i860 && !i860XP
	flush_tlb();
#endif	i860 && !i860XP
}


/*
 * set the A bit of the PTE
 */
STATIC void ctrap_pte_set_accessed(ptep, pte, addr)
	pt_entry_t	*ptep, pte;
	vm_offset_t	addr;
{
	RPMSOFT_STAT_INC(cpu_number(), rpms_notref);
	*ptep = pte | INTEL_PTE_REF;

#if	i860 && !i860XP
	flush_tlb();
#endif	i860 && !i860XP
}


/*
 * set the D bit of the PTE
 */
STATIC void ctrap_set_dirty(ptep, pte, addr, regs)
	pt_entry_t	*ptep, pte;
	vm_offset_t	addr;
	struct i860_saved_state	*regs;
{
	RPMSOFT_STAT_INC(cpu_number(), rpms_notdirty);
	*ptep = pte | INTEL_PTE_MOD;

#if	i860 && !i860XP
	flush_tlb();
#endif	i860 && !i860XP
}

#define MA_LD		0		/* misaligned load */
#define MA_ST		1		/* misaligned store */
#define MA_FLD		2		/* misaligned floating point load */
#define MA_FST		3		/* misaligned floating point store */
#define MA_INT		0		/* misaligned integer register */
#define MA_FLOAT	2		/* misaligned floating point register */

/*
 * all data-access traps eventually flow through here.
 * given a pointer to the register set, the faulting virtual
 * address, the direction (load or store), and the alignment
 * restrictions, we build a bitfield-code for the next-higher
 * level of trap management.
 * if the page fault can be handled here (A or D bit maintenance),
 * we do so.
 */
STATIC void page_fault(regs, addr, store, align, regnum, ip)
	register struct i860_saved_state *regs;
	register unsigned int addr, store, align, regnum;
	unsigned *ip;
{
	int		code, psr, epsr;
	int		fetch;
	pt_entry_t	*dir, *tab, *pdep, *ptep, pde, pte;

	CTRAP_COUNT(ctrap_pagefaults);

	code = 0;
	regs->vaddr = addr;
	psr = regs->psr;
	epsr = regs->epsr;

	code |= store;

	if (psr & PSR_PU) {
		code |= T_PF_USER;
	}

/***************************************************************
       VM related problem here, need to discover cause.
       Take this assert out for T6.2 due to time constraints.
       Greg
***************************************************************/
#if 0
#if	MCMSG && MACH_ASSERT
	{
		extern mcmsg_reentry;

		assert(mcmsg_reentry == 0);
	}
#endif	MCMSG && MACH_ASSERT
#endif


	/*
	 * fetch the PDE of the virtual address that caused
	 * the fault.
	 */
	dir = (pt_entry_t *) (regs->dirbase & ~INTEL_OFFMASK);
	pdep = &dir[pdenum(addr)];
	pde = *pdep;

	regs->trapno = T_PAGE_FAULT;

	/*
	 * PDE not present?
	 */
	if ((pde & INTEL_PTE_VALID) == 0) {
		RPMSOFT_STAT_INC(cpu_number(), rpms_pdenotp);
		regs->code = code;
		trap(regs);
		return;
	}

	/*
 	 * priv violation on PDE?
	 */
	if ((psr & PSR_PU) && !(pde & INTEL_PTE_USER)) {
		RPMSOFT_STAT_INC(cpu_number(), rpms_pdenotu);
		regs->code = code | T_PF_PROT;
		trap(regs);
		return;
	}

	/*
	 * first access during a locked sequence?
	 */
	if ((epsr & EPSR_IL) && ((pde & INTEL_PTE_REF) == 0)) {
		ctrap_pde_set_accessed(pdep, pde, addr);
		return;
	}

	/*
	 * fetch the PTE for the virtual address that caused the fault.
	 * the address of the PTE is needed because we might need to
	 * modify it here (e.g., mark it dirty).
	 */
	tab = (pt_entry_t *) ((unsigned long) pde & ~INTEL_OFFMASK);
	ptep = &tab[ptenum(addr)];
	pte = *ptep;
	if ((pte & INTEL_PTE_VALID) == 0) {
		RPMSOFT_STAT_INC(cpu_number(), rpms_ptenotp);
		regs->code = code;
		trap(regs);
		return;
	}

	/* we now know that the page is present... */

	/*
	 * second level protection violation?
	 */
	if ((psr & PSR_PU) && ((pte & INTEL_PTE_USER) == 0)) {
		RPMSOFT_STAT_INC(cpu_number(), rpms_ptenotu);
		regs->code = code | T_PF_PROT;
		trap(regs);
		return;
	}

	/*
	 * first access during a locked sequence?
	 */
	if ((epsr & EPSR_IL) && ((pte & INTEL_PTE_REF) == 0)) {
		ctrap_pte_set_accessed(ptep, pte, addr);
		return;
	}

	/*
	 * write error?
	 */
	if (store == OP_STORE) {
		/*
		 * write-protected?
		 */
		if (((pde & INTEL_PTE_WRITE) == 0) ||
		    ((pte & INTEL_PTE_WRITE) == 0)) {
			RPMSOFT_STAT_INC(cpu_number(), rpms_notwr);
			regs->code = code;
			trap(regs);
			return;
		}
		/*
		 * write to a clean page?
		 */
		if ((pte & INTEL_PTE_MOD) == 0) {
			ctrap_set_dirty(ptep, pte, addr, regs);
			return;
		}
	}


	/*
	 * match on the debugging register?
	 *
	 * does not have to match exactly so that
	 * data fetch of different alignment will still break
	 *
	 */
	if(((regs->psr & PSR_BR) && (!store)) || ((regs->psr & PSR_BW) && (store))) {
		if ((addr <= regs->db) && (((addr+align)) >= regs->db)) {
			regs->code = T_WATCHPOINT;
			regs->trapno = T_BPTFLT;
			trap(regs);
			return;
		}
	}

	/*
	 * defer unaligned access handling to the next level
	 */
	if (addr & align) {
		CTRAP_COUNT(ctrap_unaligned);
		/*
		 * fetch is the type of fetch:
		 *	integer load	MA_LD | MA_INT
		 *	integer store	MA_ST | MA_INT
		 *	floating load	MA_LD | MA_FLOAT
		 *	floating store	MA_ST | MA_FLOAT
		 */
		fetch = (store & OP_STORE) ? MA_ST : MA_LD;
		/*
		 * regnum: 0-31 integer register
		 *	   32-63 floating point register
		 */
		fetch |= (regnum>=32) ? MA_FLOAT : MA_INT;
		if(regnum >= 32)
			regnum-=32;
		/*
		 * alignement:
		 *	0x0 - byte (should never get here)
		 *	0x1 - short (2-bytes)
		 *	0x3 - long (4-bytes)
		 *	0x7 - double (8 bytes)
		 *	0xf - quad (16 bytes)
		 *
		 *	size = align + 1
		 */

		if(misalign(fetch,addr,align+1,regnum,regs,ip) == 0) {
			regs->save_r15 = 1; /* ????? */
			return;
		}
		/*
		 * error during processing of misaligned error
		 */
		regs->code = code;
		regs->vaddr = addr;
		regs->trapno = T_UNALIGNED;
		trap(regs);
		return;
	}
#if	NCPUS > 1
	/*
	 * Inconsistant TBL entries between processors? has the Dirty bit
	 * already been set?
	 * senario:
	 *	cpuX takes a clean page trap on a store; sets dirty bit.
	 *	cpuY's TLB does NOT reflect this as we do NOT spend the time
	 *	for a cross-processor TBL shootdown interrupt on every
	 *	clean-page access. So we check for the condition and dismiss
	 *	the exception. As a side effect the TBL is marked invalid which
	 *	is what we wanted in the first place.
	 */
	if ( (store == OP_STORE) && (pte & INTEL_PTE_MOD) ) {
		CTRAP_COUNT(ctrap_tlb_miss);
		return;
	}
#endif
	/*
	 * Maybe a bogus pte?
	 *
	 * Perhaps the icache and dcache are incoherent and we
	 * can't see the actual instruction sequence that was executed...
	 *
	 */
	regs->code = code | T_PF_PROT;	/* enough to raise an exception */
	if (page_fault_huh_verbose || ((regs->psr & PSR_PU) == 0)) {
		printf("page_fault: Huh? regs=%x, psr=%x, pc=%x, *pc=%x,\n",
			regs, psr, regs->pc, *((unsigned long *) regs->pc));
		printf("page_fault: ... code=%d, store=%d, align=%d,\n",
			code, store, align);
		printf("page_fault: ... vaddr=%x dirbase=%x, pde=%x, pte=%x,\n",
			addr, regs->dirbase, pde, pte);
		printf("page_fault: ... pdep=%x, ptep=%x\n", pdep, ptep);
	}
	if ((regs->psr & PSR_PU) == 0) {
		assert(0);
	}
	trap(regs);
	flush();
}

/*
 * misalign(fetch,va,size,breg,regs,ip)
 *   where:
 *	fetch = type of fetch taking place (load,store,float load, float
 *		store).
 *	va    = virtual address of fetch or store).
 *
 *	size  = size of fetch in bytes (2,4,8,16).
 *
 *	breg  = base integer or floating point register for fetch or
 *		store.
 *	regs  = base or save/restore area for this thread
 *
 *	ip  = pointer to instruction causing interrupt
 *
 *  returns:
 * 	0  		= everything ok and misalignement fixed on store or load.
 *	T_UNALIGNED	= alignement could not be fixed.
 *
 *   This routine fetches mis-aligned data and puts into appropriate
 *   registers to emulate fetches as if the hardware did the fetch.  It will
 *   be on about 2-3 orders of magnitude slower than if the hardware
 *   could do the fetch.  It also stores misaligned data from registers
 *   into memory.  Also this routine must handle any auto incrementing for
 *   fld's and fst's.
 */

int misalign(fetch,va,size,breg,reg,ip)
int fetch;
char *va;
int size;
int breg;
struct i860_saved_state *reg;
unsigned int *ip;
{
	unsigned int error = 0;  /* error return value */
	unsigned int *ireg,*fpreg;
	pcb_t   pcb = current_thread()->pcb;
	unsigned int incr;
	struct igen igen;

	error = 0;

	/*
	 * check to see if mis-aligned is an error or if the fetch should
	 * be done quietly.
	 */
	if( (pcb->flags & PCB_SIGNAL_MA) == PCB_SIGNAL_MA) {
		error = T_UNALIGNED;
	} 

	/*
	 * quietly perform mis-aligned data fetches
	 */

	else {
		switch(fetch) {

		case MA_LD: /* integer register load from memory */
			ireg = (unsigned *)&reg->r0;/*integer registers */
			if( (copyin(va,&ireg[breg],size)) != 0) {
				error = T_UNALIGNED;
			}
			break;

		case MA_ST: /* integer register store to memory */
			ireg = (unsigned *)&reg->r0; /* integer registers */
			if( (copyout(&ireg[breg],va,size)) != 0) {
				error = T_UNALIGNED;
			}
			break;

		case MA_FLD: /* floating register load from memory */
			fpreg = (unsigned *)&reg->f0;  /* fp registers */
			if( (copyin(va,&fpreg[breg],size)) != 0) {
				error = T_UNALIGNED;
				break;
			}
			/*
			 * now check for auto incrementing
			 */
			igen = *((struct igen *) ip);
			if(igen.ig_offset & 1) {
				ireg = (unsigned *)&reg->r0; /* integer registers */
				incr =  (igen.ig_op & 1) ? 
					((igen.ig_offset & 0x2) ? igen.ig_offset & ~0x3 :
					igen.ig_offset & ~0x7) : ireg[igen.ig_src1];
				ireg[igen.ig_src2] += incr;
			}
			break;

		case MA_FST: /* floating register store to memory */
			fpreg = (unsigned *)&reg->f0;  /* fp registers */
			if( (copyout(&fpreg[breg],va,size)) != 0) {
				error = T_UNALIGNED;
				break;
			}
			/*
			 * now check for auto incrementing xxxxx
			 */
			igen = *((struct igen *) ip);
			if(igen.ig_offset & 1) {
				ireg = (unsigned *)&reg->r0; /* integer registers */
				incr =  (igen.ig_op & 1) ? 
					((igen.ig_offset & 0x2) ? igen.ig_offset & ~0x3 :
					igen.ig_offset & ~0x7) : ireg[igen.ig_src1];
				ireg[igen.ig_src2] += incr;
			}
			break;

		default:
			/*
			 * should never happen
			 */
			error = T_UNALIGNED;
			printf("unknown misaligned data type\n");
			break;

		}
	}
	return(error);
}
/*
 * data-access trap on a ld.x
 */
STATIC void datld(regs, instr, ip, igen)
	register struct i860_saved_state *regs;
	unsigned long instr, *ip;
	struct igen igen;
{
	int	src1, reg2, reg2p, src2, align;

	RPMSOFT_STAT_INC(cpu_number(), rpms_datld);

	reg2 = igen.ig_src2;
	reg2p = (unsigned) regs + (reg2 << 2);
	src2 = *((int *) reg2p);

	if (igen.ig_op & 4) {
		/* ld.s -or- ld.l */
		align = (igen.ig_offset & 1) ? 3 : 1;
		igen.ig_offset &= ~1;
	} else {
		/* ld.b	*/
		align = 0;
	}

	/* #const(src2) -or- src1(src2) */
	if (igen.ig_op & 1) {
		struct iimed im = *((struct iimed *) ip);
		if ((src1 = im.ii_offset) & 0x8000) {
			src1 |= 0xffff0000;
		}
		if (align) {
			/* not byte-aligned */
			src1 &= ~1;
		}
	} else {
		int	reg1p;
		reg1p = (unsigned) regs + (igen.ig_src1 << 2);
		src1 = *((int *) reg1p);
	}

	page_fault(regs, src1 + src2, OP_LOAD, align, (unsigned)igen.ig_dest,igen,ip);
}


/*
 * data-access trap on a st.x
 */
STATIC void datst(regs, instr, ip, igen)
	register struct i860_saved_state *regs;
	unsigned long instr, *ip;
	struct igen igen;
{
	int	reg2, reg2p, src2, align, addr;
	int	offset;

	RPMSOFT_STAT_INC(cpu_number(), rpms_datst);

	reg2 = igen.ig_src2;
	reg2p = (unsigned) regs + (reg2 << 2);
	src2 = *((int *) reg2p);

	if (igen.ig_op & 4) {
		/* st.s -or- st.l */
		align = (igen.ig_offset & 1) ? 3 : 1;
		igen.ig_offset &= ~1;
	} else {
		/* st.b	*/
		align = 0;
	}

	/* top 5 bits of offset are where dest would be */
	offset = (igen.ig_dest << 11) | igen.ig_offset;
	if (offset & 0x8000)
		offset |= 0xffff0000;

	page_fault(regs, src2 + offset, OP_STORE, align, (unsigned)igen.ig_src1,igen,ip);
}


/*
 * data-access trap on any of: fld.x, fst.x, pfld.y
 */
STATIC void datldstfp(regs, instr, ip, igen)
	register struct i860_saved_state *regs;
	unsigned long instr, *ip;
	struct igen igen;
{
	register int	src1, reg2, reg2p, src2, align, store, autoinc;

	reg2 = igen.ig_src2;
	reg2p = (unsigned) regs + (reg2 << 2);
	src2 = *((int *) reg2p);

	store = igen.ig_op & OP_STORE;

	autoinc = igen.ig_offset & 1;
	align = (igen.ig_offset & 2) ? 3 : (igen.ig_offset & 4) ? 15 : 7;
	igen.ig_offset &= ~7;

	/*
	 *  #const(src2) -or- src1(src2), bit 26 == 1 implies #const
	 */
	if (igen.ig_op & 1) {
		/* #const(src2) */
		register struct iimed im = *((struct iimed *) ip);

		/*
		 * extract immediate constant forcing correct bits to zero.
		 * clear bit 0 (autoincr flag) clear bit 1 (maintain alignment),
		 * sign extend correctly the immediate constant.
		 * 
		 * see i860 Programmers Ref Man. Appendix B (instruction
		 * encoding) pages B-2 & B-3.
		 */
		if ((src1 = (im.ii_offset & ~3)) & 0x8000)
			src1 |= 0xffff0000;
		/*
		 * 128 bit (quad) size? if bit 1 of the instruction immediate
		 * value is zero then bit 2 must also be forced to zero.
		 *
		 * Seem the i860 Programmers Ref Man. Appendix B (instruction
		 * encoding) page B3, table for fld, fst, pfld,pst & flush
		 * operand size encodings is incorrect. The combination of
		 * bit 1 == 1 & bit 2 == 0 is NOT really a valid 32 bit operand
		 * size. Must special case the check for this so we do NOT
		 * step on (bit 1 == 1 & bit 2 == 1) == 32 bit operand.
		 * In otherwords, only clear bit 2 if 128 operand size.
		 */
		if ( (im.ii_offset & 6) == 4 )
			src1 &= ~4;
	} else {
		/* src1(src2) */
		register int	reg1p;

		reg1p = (unsigned) regs + (igen.ig_src1 << 2);
		src1 = *((int *) reg1p);
	}

	/* undo the autoincrement if not a floating-point trap */
	if (autoinc && !(regs->psr & PSR_FT)) {
		RPMSOFT_STAT_INC(cpu_number(), rpms_datauto);
		src2 -= src1;
		*((int *) reg2p) = src2;
	}
/*
 * reg number is +32 to distingish it from integer registers
 */
	page_fault(regs, src1 + src2, store, align, (unsigned)igen.ig_dest + 32,igen,ip);

}


/*
 * data-access trap on a pst.d
 *
 * XXX not tested
 */
STATIC void datpst(regs, instr, ip, igen)
	register struct i860_saved_state *regs;
	unsigned long instr, *ip;
	struct igen igen;
{
	RPMSOFT_STAT_INC(cpu_number(), rpms_datpst);

	datldstfp(regs, instr, ip, igen);
}


/*
 * data-access trap on an inappropriate instruction
 */
STATIC void datill(regs, instr, ip, igen)
	register struct i860_saved_state *regs;
	unsigned long instr, *ip;
	struct igen igen;
{
	if (impossible_DAT_verbose) {
		printf("datill(regs=%x, instr=%x): pc=%x, psr=%x\n",
			regs, instr, regs->pc, regs->psr);
	}
	if ((regs->psr & PSR_PU) == 0) {
		panic("datill");
	}
}

/*
 * data-access traps are darn complicated.
 */

STATIC void dattrap(regs)
	register struct i860_saved_state *regs;
{
	unsigned long	instr, *ip, op;
	struct igen	igen;

	RPMSOFT_STAT_INC(cpu_number(), rpms_dat);

	if (regs->psr & PSR_DIM) {
		ip = (unsigned long *) (regs->pc + 4);
	} else {
		ip = (unsigned long *) regs->pc;
	}


	instr = *ip;
	igen = *((struct igen *) ip);
	op = igen.ig_op;

	switch (op) {
	case 0x00:
	case 0x01:
	case 0x04:
	case 0x05: /* ld.x */
		datld(regs, instr, ip, igen);
		break;

	case 0x03:
	case 0x07: /* st.x */
		datst(regs, instr, ip, igen);
		break;

	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b: /* fld.x or fst.x */
		RPMSOFT_STAT_INC(cpu_number(), rpms_datfldfst);
		datldstfp(regs, instr, ip, igen);
		break;

	case 0x0f: /* pst.d */
		datpst(regs, instr, ip, igen);
		break;

	case 0x18:
	case 0x19: /* pfld.y */
		RPMSOFT_STAT_INC(cpu_number(), rpms_datpfld);
		datldstfp(regs, instr, ip, igen);
		break;

	default: /* this should never happen... */
		datill(regs, instr, ip, igen);
		regs->code = instr;
		regs->vaddr = regs->pc;
		regs->trapno = T_INVALID_OPCODE;
		trap(regs);
		break;
	}
}

/*
 *	Set the IEEE mask bits and return the previous
 *	mask bits.
 */
STATIC void ieee_set_fpmask(regs)
	register struct i860_saved_state *regs;
{
	pcb_t		pcb;
	unsigned long	old, new;

	pcb = current_thread()->pcb;
	old = pcb->ieee_status & IEEE_MASKS;
	new = regs->r16 & IEEE_MASKS;
	pcb->ieee_status = new;
	regs->r16 = old;
	regs->pc += 4;
}


/*
 *	Return the current mask bits.
 */
STATIC void ieee_get_fpmask(regs)
	register struct i860_saved_state *regs;
{
	pcb_t		pcb;

	pcb = current_thread()->pcb;
	regs->r16 = pcb->ieee_status & IEEE_MASKS;
	regs->pc += 4;
}

/*
 * floating-point trap -- glue into the semi-off-the-shelf FPE handler.
 */
STATIC void fptrap(regs)
	register struct i860_saved_state *regs;
{
	pcb_t	pcb = current_thread()->pcb;
	int	mycpu = cpu_number();
	int	i;

	assert((regs->psr & PSR_PU) == PSR_PU);

	RPMSOFT_STAT_INC(mycpu, rpms_ft);

	/*
	 * instrumentation to catch nodes thrashing in fptrap().
	 */
	if (fptrap_verbose & (ECC_CONTROL_PRINT_ALL | ECC_CONTROL_PRINT_2))
	{
		extern int popcnt(int);
		rpm_counter_t count = RPMSOFT_STAT(mycpu, rpms_ft);

		if ((count >= 1024*1024) && (popcnt(count) == 1))
		{
			printf("NOTICE: cpu%d: %d fptraps.\n", mycpu, count);

			if (fptrap_verbose & ECC_CONTROL_RED_LED)
			{
				RED_ON(RED_ECC);
			}
		}
	}

	if (regs->fsr & FSR_SI)
		RPMSOFT_STAT_INC(mycpu, rpms_fpe_si);
	if (regs->fsr & FSR_MA)
		RPMSOFT_STAT_INC(mycpu, rpms_fpe_ma);
	if (regs->fsr & FSR_AA)
		RPMSOFT_STAT_INC(mycpu, rpms_fpe_aa);

	/*
	 * source exception?
	 */
	if (regs->fsr & FSR_SE) {
		RPMSOFT_STAT_INC(mycpu, rpms_fpe_se);

		if ( check_if_to_ignore_SE( regs ) ) {
			return;	/* ignore SE */
		}
		/* handle SE downstream. */
	}

	/*
	 * are we doing IEEE floating point? 
	 */
	if ( pcb->flags & PCB_IEEE_FP ) {
		/*	NB: the following workaround was merged out
		 *	between revs 1.32.2.2 and 1.32.2.3
		 *
		 *	The while loop below is for some undetermined errata
		 *	for the i860 chip.  Under some rare circumstances,
		 *	it appears as though the instruction being checked
		 *	is returned from the wrong way of the data cache.
		 *	The workaround is to flush the caches and look
		 *	at the instruction again.
		 *
		 *	It *might* not catch all cases correctly -- especially
		 *	if the erroneous data returned looks like a valid
		 *	floating point instruction (but wasn't the one that
		 *	triggered the floating-point trap).
		 */
		i = 2;
		while (i--) {
			regs->trapped_opcode =
			    set_trapped_opcode(*((unsigned long *) regs->pc));
			if ((regs->fsr & FSR_SE)&&(regs->trapped_opcode == 0)) {
				unknown_fp_traps[mycpu]++;
				flush();
				continue;
			} else if (fp_exception(regs) == 0) {
				return;
			}
			break;
		}
	}

	/*
	 * error handling FP exception, send a unix signal SIGFPE.
	 */
	regs->vaddr = regs->pc;
	regs->trapno = T_FLOATING_POINT_ERROR;
	trap(regs);

	return;
}


unsigned long taken_ast_count;		/* XXX -- see pcb.c */

/*
 * check for ast's -- called from near the bottom of the trap handler.
 * Previous processor mode has been verified to be USER by our caller.
 */
int i860_astcheck(psr)
	unsigned int	psr;
{
	if (ast_needed(cpu_number())) {
		(void) splsched();
		taken_ast_count++;
		ast_taken();
		thread_ctrap_return();
		/*NOTREACHED*/
	}
	return 0;
}

#define	SCALL_SHORTCUT	1
#if	SCALL_SHORTCUT
/*
 *	check for ast's -- called from near the bottom of the trap handler.
 */
int i860_call_asttaken()
{
	(void) splsched();
	taken_ast_count++;
	ast_taken();
	thread_ctrap_return();
	/*NOTREACHED*/
}
#endif	SCALL_SHORTCUT


/*
 *	Experimental trap, perhaps useful for the emulator,
 *	to save "complex" processor state (like the load,
 *	multiplier, and adder pipes, KR/KI/T, etc.).
 */
STATIC void i860_trap_setjmp(regs)
	register struct i860_saved_state *regs;
{
	register struct i860_saved_state *buf;

	buf = (struct i860_saved_state *) regs->r16;
	(void) copyout((vm_offset_t) regs, (vm_offset_t) buf, sizeof(*buf));
	regs->pc += 4;
}


/*
 *	Experimental trap, perhaps useful for the emulator,
 *	to restore "complex" processor state (like the load,
 *	multiplier, and adder pipes, KR/KI/T, etc.).
 */
STATIC void i860_trap_longjmp(regs)
	register struct i860_saved_state *regs;
{
	register struct i860_saved_state *buf;
	unsigned val;

	buf = (struct i860_saved_state *) regs->r16;
	(void) copyin((vm_offset_t) buf, (vm_offset_t) regs, sizeof(*buf));
	regs->psr |= (PSR_PU | PSR_PIM);
	regs->pc += 4;
}


/*
 *	Microkernel assisted sigreturn.
 *
 *	regs->r16 points to an i860_thread_state; copy it in
 *	and merge it into an i860_saved_state.  enforce user-mode
 *	bits within the psr.
 *
 *	XXX When a "struct i860_thread_state" EXACTLY matches
 *	XXX a "struct i860_saved_state" we could copyin()
 *	XXX directly into the pcb, and then enforce the user-mode
 *	XXX bits in the psr.  Until then, we'll use thread_setstatus()
 *	XXX to merge into the pcb.
 */
STATIC void i860_trap_sigreturn(regs)
	register struct i860_saved_state *regs;
{
	struct i860_thread_state context;

	(void) copyin((vm_offset_t) regs->r16,
		(vm_offset_t) &context, sizeof(context));

	(void) thread_setstatus(current_thread(), i860_THREAD_STATE,
		&context, i860_THREAD_STATE_COUNT);
}


/*
 * ctrap should never return to user mode directly...
 * we should *always* return to user mode via thread_exception_return()
 * ctrap_safetynet() is called if ctrap() attempts to return to user mode.
 */
void ctrap_safetynet(regs)
	register struct i860_saved_state *regs;
{
	if (regs->psr & PSR_PU) {
		printf("ctrap_safetynet(regs=%x): caught one...\n", regs);
		panic("ctrap_safetynet");
	}
}


/*
 * restart a locked sequence
 *
 *	-- called from near the bottom of the trap handler.
 */
void i860_restart_lock(regs, epsr)
	register struct i860_saved_state *regs;
	unsigned int epsr;
{
	RPMSOFT_STAT_INC(cpu_number(), rpms_lockseq);

	if (lockrestart(regs)) {
		badlock(regs);
	}

	regs->epsr = epsr & ~EPSR_IL;
	RPMSOFT_STAT_INC(cpu_number(), rpms_lockres);
}


/*
 * lock protocol violation
 */
STATIC void badlock(regs)
	register struct i860_saved_state *regs;
{
	RPMSOFT_STAT_INC(cpu_number(), rpms_lockexp);

	regs->vaddr = regs->pc;
	regs->trapno = T_LOCKEXPIRED;
	trap(regs);
}

#if	!defined(i860XP)
/*
 * encountered an instruction trap for a pfld.q instruction. Emulate it...
 *
 * inputs:
 *	rp	pointer to exception state
 *	inst	actual instruction pattern which caused the trap.
 * outputs:
 *	0 == successful emulation of pfld.q
 *	1 == failure code.
 * side effects:
 *	on success the destination fp registers are updated.
 *	failure, set up reg structure so trap(regs) can be called by our caller.
 *
 * This routine will be on the order of 2-3 orders of magnitude slower then if
 * the pfld.q was done on the XP.
 */

emulate_pfldq( rp, pinst )
	struct i860_saved_state	*rp;
	unsigned		pinst;
{
	int		incr;
	unsigned	*ireg, *fpreg, user_va;
	struct igen	*inst;
	extern	int	chip_step;	/* ../i860ipsc/model_dep.c */
	pcb_t	pcb = current_thread()->pcb;

	/*
	 * does the thread allow pfld.q emulation or is it an error
	 */
	if( (pcb->flags & PCB_SIGNAL_PFLDQ) == PCB_SIGNAL_PFLDQ) {
		return(T_INVALID_OPCODE);
	}

	/*
	 * must be C1 chip step or greater.
	 */
	if ( chip_step < 6 )
		return( T_INVALID_OPCODE );

	/*
	 * inst is the instruction that caused the interrupt.
	 * that is the pfld.q instruction
	 */
	inst = (struct igen *)&pinst;
	/*
	 * ireg and fpreg are the base addresses of the saved
	 * registers.
	 */
	ireg = (unsigned *)&rp->r0;	/* saved integer registers */
	fpreg = (unsigned *)&rp->f0;	/* saved fp registers */

	/*
	 * Check for immediate data or register (bit #26); fetch increment,
	 * must be at least 16-byte aligned.
	 */
	incr = (inst->ig_op & 1) ? inst->ig_offset & ~0x7 : ireg[inst->ig_src1];

	/*
	 * calculate the virtual address and then check alignement
	 * disallow misaligned fetches if they are currently disabled.
	 * Otherwise go ahead and do the alignment
	 */
	user_va = ireg[inst->ig_src2] + incr;	/* src2 VA + src1 */
	if (user_va & 0xf) {
		if( (pcb->flags & PCB_SIGNAL_MA) == PCB_SIGNAL_MA) {
			rp->vaddr = user_va;
			return(T_INVALID_OPCODE);
		}
	}


	/*
	 * auto-increment? update src2 register with src1.
	 */
	if ( inst->ig_offset & 1 )
		ireg[inst->ig_src2] += incr;	/* yes, pre-increment */

	/*
	 * fetch the data from user-space to our saved state.
	 */

	/*
	 * push the pfld.q pipeline
	 */

	if((rp->trapno = push_pfdlq_pipe(pcb,(double *)fpreg,inst->ig_dest,user_va,rp)) == 0 ) {
		return(0);
	}
	return(T_INVALID_OPCODE);
}

/* pushes the quad word memory pipe line kept in software
 * should be noted that with this fix it is not possible to mix pfld.q with
 * other types of pfld instructions. Example:
 *
 *					pfld.q  0(rn),fn0
 *					pfld.d 16(rn),fn1
 *					pfld.d 24(rn),fn2
 *					pfld.q 32(rn),fn3
 *
 * This will not work as the pipe line will be out of sequence
 */

push_pfdlq_pipe(pcb,fpreg,index,va,rp)
	pcb_t	pcb;
	double	*fpreg;
	int	index;
	unsigned int va;
	struct i860_saved_state	*rp;
{
	/*
	 * if index (destination register) is zero then
	 * don't do anything except push pipe
	 * if none zero then push the values in the top
	 * of the pipe in to the appropiate floating point registers.
	 * since fp regs are only 4 bytes and double is 8 and quad word
	 * fetches are 16 bytes. If index = 20 then fpreg[10] (register
	 * 20 and 21) and fpreg[11] (register 22 and 23) should be updated.
	 */
	if(index != 0) {
		index = index/2;
		fpreg[index] = pcb->pfldq_pipe[4];
		fpreg[index+1] = pcb->pfldq_pipe[5];
	}
	/*
	 * now push the software pipe lines
	 */
	pcb->pfldq_pipe[4] = pcb->pfldq_pipe[2];
	pcb->pfldq_pipe[5] = pcb->pfldq_pipe[3];
	pcb->pfldq_pipe[2] = pcb->pfldq_pipe[0];
	pcb->pfldq_pipe[3] = pcb->pfldq_pipe[1];
	/*
	 * and lastly update the bottom of the
	 * pipe with the value from memory
	 */
	if ( copyin( va, &pcb->pfldq_pipe[0], 16 ) ) {
		return( T_INVALID_OPCODE );
	}
/*
 * probably should push the hardware pipeline too but if the program is really intermixing
 * pfld.q and (pfld.l or pfld.d) then it won't work anyway
 */
	return(0);
}

#endif	!defined(i860XP)


#if	CTRAP_HISTORY && MACH_KDB

#if	PARAGON860
static long tickstous(ticks)
	long	ticks;
{
	double	tick, time, mega;
	extern int paragon_node_mhz;

	mega = 1000000.0;
	tick = 1.0 / ((double) paragon_node_mhz * mega);
	time = tick * ticks;
	return (long) ((time * mega) + 0.5);
}


void db_show_trap_hist()
{
	int	i, j, ticks, us;
	unsigned long	psr;
	struct ctrap_history	*cur, *prev;
	char	*dash = " - ";
	char	*s = "%s ";

	if (ctrap_history_enabled == 0) {
		db_printf("ctrap_history_enabled==0\n");
		return;
	}
	db_printf(" nth trap     time  ticks( us )      fir      psr (decode)\n");

	j = ctrap_history_next - 1;
	prev = 0;
	for (i = 0; i < CTRAP_HISTLEN; i++) {

		if (j < 0)
			j = CTRAP_HISTLEN - 1;

		cur = &ctrap_history[j];

		psr = cur->ch_psr;

		if (prev) {
			ticks = prev->ch_time - cur->ch_time;
			us = tickstous(ticks);
		} else {
			ticks = 0;
			us = 0;
		}
		db_printf("%c%8u %8x %6d(%4d) %8x %8x ",
			(cur->ch_tracing) ? '*' : ' ',
			cur->ch_trapn,
			cur->ch_time,
			ticks, us,
			cur->ch_fir,
			psr);

#if	0
		db_printf(s, (psr & PSR_CC)  ? "cc " : dash);
		db_printf(s, (psr & PSR_LCC) ? "lcc" : dash);
#endif	0
		db_printf(s, (psr & PSR_PIM) ? "pim" : dash);
		db_printf(s, (psr & PSR_PU)  ? "pu " : dash);
		db_printf(s, (psr & PSR_IT)  ? "it " : dash);
		db_printf(s, (psr & PSR_IN)  ? "in " : dash);
		db_printf(s, (psr & PSR_IAT) ? "iat" : dash);
		if (psr & PSR_DAT) {
			if (cur->ch_wascow)
				db_printf(s, "cow");
			else if (cur->ch_waszer)
				db_printf(s, "zer");
			else if (cur->ch_wasmod)
				db_printf(s, "mod");
			else if (cur->ch_wasref)
				db_printf(s, "ref");
			else
				db_printf(s, "dat");
		} else {
			db_printf(s, dash);
		}
		db_printf(s, (psr & PSR_FT)  ? "ft " : dash);
		db_printf("\n");

		prev = cur;

		j--;
	}
}

/*
 * display i860XP processor protected registers
 *
 * used by kdb cmd "show pregs"
 */
void
db_show_pregs()
{
	unsigned        pregs[4];

	get_pregs( pregs );     /* i860/misc.s */
	db_printf("p0 0x%x p1 0x%x p2 0x%x p3 0x%x\n",
		pregs[0],pregs[1],pregs[2],pregs[3]);
}

#else	PARAGON860

void db_show_trap_hist()
{
	int	i, j;
	unsigned long	psr;
	char	*dash = " - ";
	char	*s = "%s ";

	if (ctrap_history_enabled == 0) {
		db_printf("ctrap_history_enabled==0\n");
		return;
	}

	db_printf("nth trap     regs=       fir       psr  (decode)\n");

	j = ctrap_history_next - 1;
	for (i = 0; i < CTRAP_HISTLEN; i++) {

		if (j < 0)
			j = CTRAP_HISTLEN - 1;

		psr = ctrap_history[j].ch_psr;

		db_printf("%8u  %8x  %8x  %8x  ",
			ctrap_history[j].ch_trapn,
			ctrap_history[j].ch_regs,
			ctrap_history[j].ch_fir,
			psr);

		db_printf(s, (psr & PSR_CC)  ? "cc " : dash);
		db_printf(s, (psr & PSR_LCC) ? "lcc" : dash);
		db_printf(s, (psr & PSR_PIM) ? "pim" : dash);
		db_printf(s, (psr & PSR_PU)  ? "pu " : dash);
		db_printf(s, (psr & PSR_IT)  ? "it " : dash);
		db_printf(s, (psr & PSR_IN)  ? "in " : dash);
		db_printf(s, (psr & PSR_IAT) ? "iat" : dash);
		db_printf(s, (psr & PSR_DAT) ? "dat" : dash);
		db_printf(s, (psr & PSR_FT)  ? "ft " : dash);
		db_printf("\n");

		j--;
	}
}

#endif	PARAGON860
#endif	/* CTRAP_HISTORY && MACH_KDB */

char *i860_cpu_type[] = {
	"i860XR",
	"i860XP"
};

#if	MACH_KDB


show_860_regs(regs)
	struct i860_saved_state *regs;
{
	/*db_show_i860_regs(regs, TRUE, TRUE, FALSE, FALSE, FALSE);*/
	db_show_i860_regs(regs, TRUE, TRUE, TRUE, TRUE, TRUE);
}


show_860_fregs(regs)
	struct i860_saved_state *regs;
{
	db_show_i860_regs(regs, TRUE, TRUE, TRUE, TRUE, TRUE);
}


static char *i860_rounding[4] = { "nr", "dn", "up", "ch" };
void db_show_i860_fsr(fsr)
	unsigned long fsr;
{
	db_printf("fsr=0x%08x [", fsr);
	if (fsr & FSR_FZ)
		db_printf(" fz");
	if (fsr & FSR_TI)
		db_printf(" ti");
	db_printf(" rm=%s", i860_rounding[(fsr & FSR_RM) >> 2]);
	if (fsr & FSR_U)
		db_printf(" u");
	if (fsr & FSR_FTE)
		db_printf(" fte");
	if (fsr & FSR_SI)
		db_printf(" si");
	if (fsr & FSR_SE)
		db_printf(" se");
	if (fsr & FSR_MU)
		db_printf(" mu");
	if (fsr & FSR_MO)
		db_printf(" mo");
	if (fsr & FSR_MI)
		db_printf(" mi");
	if (fsr & FSR_MA)
		db_printf(" ma");
	if (fsr & FSR_AU)
		db_printf(" au");
	if (fsr & FSR_AO)
		db_printf(" ao");
	if (fsr & FSR_AI)
		db_printf(" ai");
	if (fsr & FSR_AA)
		db_printf(" aa");
	db_printf(" rr=%d", (fsr >> 17) & 0x1f);
	db_printf(" ae=%d", (fsr >> 22) & 0x07);
	db_printf(" lrp=%d", 2 << ((fsr >> 25) & 0x3));
	db_printf(" irp=%d", 4 << ((fsr >> 27) & 0x1));
	db_printf(" mrp=%d", 4 << ((fsr >> 28) & 0x1));
	db_printf(" arp=%d ]\n", 4 << ((fsr >> 29) & 0x1));
}


db_show_i860_psr(psr)
	unsigned long	psr;
{
	db_printf("psr=0x%08x [", psr);
	if (psr & PSR_BR)
		db_printf(" br");
	if (psr & PSR_BW)
		db_printf(" bw");
	if (psr & PSR_CC)
		db_printf(" cc");
	if (psr & PSR_LCC)
		db_printf(" lcc");
	if (psr & PSR_IM)
		db_printf(" im");
	if (psr & PSR_PIM)
		db_printf(" pim");
	if (psr & PSR_U)
		db_printf(" u");
	if (psr & PSR_PU)
		db_printf(" pu");
	if (psr & PSR_IT)
		db_printf(" it");
	if (psr & PSR_IN)
		db_printf(" in");
	if (psr & PSR_IAT)
		db_printf(" iat");
	if (psr & PSR_DAT)
		db_printf(" dat");
	if (psr & PSR_FT)
		db_printf(" ft");
	if (psr & PSR_DS)
		db_printf(" ds");
	if (psr & PSR_DIM)
		db_printf(" dim");
	if (psr & PSR_KNF)
		db_printf(" knf");
	db_printf(" sc=%d", (psr >> 17) & 0x1f);
	db_printf(" ps=%d", 0x8 << ((psr >> 22) & 0x03));
	db_printf(" pm=%x ]\n", (psr >> 24) & 0xff);
}


void db_show_i860_epsr(epsr)
	unsigned long	epsr;
{
	db_printf("epsr=0x%08x [", epsr);
	db_printf(" type=%x", epsr & 0xff);
	db_printf(" step=%d", (epsr >> 8) & 0x1f);
	if (epsr & EPSR_IL)
		db_printf(" il");
	if (epsr & EPSR_WP)
		db_printf(" wp");
	if (epsr & EPSR_PEF)
		db_printf(" pef");
	if (epsr & EPSR_BEF)
		db_printf(" bef");
	if (epsr & EPSR_INT)
		db_printf(" int");
	db_printf(" dcs=%dK", 4 << ((epsr >> 18) & 0x0f));
	if (epsr & EPSR_PBM)
		db_printf(" pbm");
	if (epsr & EPSR_BE)
		db_printf(" be");
	if (epsr & EPSR_OF)
		db_printf(" of");
	if (epsr & EPSR_BS)
		db_printf(" bs");
	if (epsr & EPSR_DI)
		db_printf(" di");
	if (epsr & EPSR_TAI)
		db_printf(" tai");
	if (epsr & EPSR_PT)
		db_printf(" pt");
	if (epsr & EPSR_PI)
		db_printf(" pi");
	if (epsr & EPSR_SO)
		db_printf(" so ]\n");
}


static void db_show_64(name, ptr)
	char		*name;
	unsigned int	*ptr;
{
	db_printf("%s=%08x%08x ", name, ptr[1], ptr[0]);
}


static void db_show_load_pipe(stage, fsr, pipe)
	int		stage;
	unsigned int	fsr, *pipe;
{
	int	prec;

	prec = (fsr >> 25) & 0x3;
	db_printf("load.%c[%d]=", "?sdq"[prec], stage);

	switch (prec) {
	case 0: db_printf("[%d]\n");
		break;

	case 3: db_printf("%08x %08x ", pipe[3], pipe[2]); pipe -= 2;
	case 2: db_printf("%08x ", pipe[1]); pipe -= 1;
	case 1: db_printf("%08x\n", pipe[0]);
		break;
	}
}

static void db_show_mult_pipe(stage, fsr, pipe)
	int		stage;
	unsigned int	fsr, *pipe;
{
	int	prec;

	prec = (fsr >> 28) & 0x1;
	db_printf("mult.%c[%d]=", "sd"[prec], stage);

	if (prec) {
		db_printf("%08x ", pipe[1]);
		pipe -= 1;
	}
	db_printf("%08x\n", pipe[0]);
}


static void db_show_add_pipe(stage, fsr, pipe)
	int		stage;
	unsigned int	fsr, *pipe;
{
	int	prec;

	prec = (fsr >> 29) & 0x1;
	db_printf(" add.%c[%d]=", "sd"[prec], stage);

	if (prec) {
		db_printf("%08x ", pipe[1]);
		pipe -= 1;
	}
	db_printf("%08x\n", pipe[0]);
}


db_show_i860_regs(regs, ctrl, iregs, fregs, kikrt, pipes)
	register struct i860_saved_state *regs;
	boolean_t	ctrl, iregs, fregs, kikrt, pipes;
{
	unsigned int	psr, fsr, epsr, *regp;
	int		x, y, r;
	char		*s;

	psr = regs->psr;
	fsr = regs->fsr;
	epsr = regs->epsr;

	/*
	 *	display the control registers
	 */
	if (ctrl) {
		db_printf("fir=0x%08x\n", regs->pc);
		db_show_i860_psr(psr);
		db_show_i860_fsr(fsr);
		db_show_i860_epsr(epsr);
		db_printf("dirbase=0x%08x db=0x%08x\n\n",
			regs->dirbase, regs->db);
	}

	/*
	 *	display the integer registers
	 */
	if (iregs) {
		regp = &regs->r0;
		for (y = 0; y < 8; y++) {
			for (x = 0; x < 4; x++) {
				db_printf("r%02d=%08x    ", (y<<2)+x, *regp++);
			}
			db_printf("\n");
		}
		db_printf("\n");
	}

	/*
	 *	display the floating-point registers
	 */
	if (fregs) {
		regp = &regs->f0;
		for (y = 0; y < 32; y += 2) {
			db_printf("f%02d=%08x %08x=f%02d\n",
				y + 1, regp[1], regp[0], y);
			regp += 2;
		}
		db_printf("\n");
	}


	/*
	 *	display the special purpose registers
	 */
	if (kikrt) {
		db_show_64("ki", (unsigned int *) &regs->spc_ki);
		db_show_64("kr", (unsigned int *) &regs->spc_kr);
		db_show_64("t", (unsigned int *) &regs->spc_t);
		/*db_show_64("merge", (unsigned int *) &regs->spc_merge);*/
		db_printf("\n\n");
	}


	/*
	 *	display the pipelines
	 */
	if (pipes) {
		db_show_load_pipe(3, fsr, (unsigned int *) &regs->psv_l3[0]);
		db_show_load_pipe(2, regs->fsr2,
				(unsigned int *) &regs->psv_l2[0]);
		db_show_load_pipe(1, regs->fsr1,
				(unsigned int *) &regs->psv_l1[0]);

		db_show_mult_pipe(3, fsr, (unsigned int *) &regs->psv_m3);
		db_show_mult_pipe(2, regs->fsr2,
				(unsigned int *) &regs->psv_m2);
		db_show_mult_pipe(1, regs->fsr1,
				(unsigned int *) &regs->psv_m1);

		db_show_add_pipe(3, fsr, (unsigned int *) &regs->psv_a3);
		db_show_add_pipe(2, regs->fsr2,
				(unsigned int *) &regs->psv_a2);
		db_show_add_pipe(1, regs->fsr1,
				(unsigned int *) &regs->psv_a1);
	}

}
#endif	MACH_KDB
