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
/*
 * $Id: trap.c,v 2.31 1995/03/14 08:00:27 lenb Exp $
 */
/*
 * Hardware trap/fault handler.
 */

#include <cpus.h>
#include <mach_kdb.h>
#include <mach_assert.h>

#include <i860/trap.h>

#include <mach/exception.h>
#include <mach/kern_return.h>
#include <mach/vm_param.h>

#include <vm/vm_kern.h>
#include <vm/vm_map.h>

#include <kern/thread.h>
#include <kern/task.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/syscall_sw.h>

#include <i860/psl.h>
#if	PARAGON860
#include <i860paragon/baton.h>
#include <i860/cpu_master.h>
#endif	/* PARAGON860 */
#include <ddb/db_break.h>

extern void gimmeabreak();

#define	STATIC
#define FAST_EMUL_SYSCALL 0	/* now handled in ttrap.s */

int	trap_syscall_trace = 0;

#if	MACH_KDB
boolean_t	debug_all_traps_with_kdb = FALSE;
extern struct db_watchpoint *db_watchpoint_list;
extern boolean_t db_watchpoints_inserted;

extern db_breakpoint_t db_not_taken_bkpt;
extern db_breakpoint_t db_taken_bkpt;

void
thread_kdb_return()
{
	register thread_t self = current_thread();
	register struct i860_saved_state *regs = USER_REGS(self);

	assert(self->pcb != 0);
	if (kdb_trap(regs->trapno, regs->code, regs)) {
		thread_exception_return();
		/*NOTREACHED*/
	}
}
#endif	MACH_KDB


/*
 * This looks like a continuation, smells like a continuation, and
 * is even passed as a continuation.  But it isn't a continuation;
 * continuations aren't allowed to take parameters.  See vm/vm_fault.c
 * to see how this routine is actually called...
 */
void
user_page_fault_continue(kr)
	kern_return_t	kr;
{
	register thread_t thread = current_thread();
	register struct i860_saved_state *regs = USER_REGS(thread);

	if (kr == KERN_SUCCESS) {
#if	MACH_KDB
		if (db_watchpoint_list &&
		    db_watchpoints_inserted &&
		    (regs->code & T_PF_WRITE) &&
		    db_find_watchpoint(thread->task->map,
				(vm_offset_t)regs->vaddr,
				regs))
			kdb_trap(T_WATCHPOINT, 0, regs);
#endif	MACH_KDB
		thread_exception_return();
		/*NOTREACHED*/
	}

#if	MACH_KDB
	if (debug_all_traps_with_kdb &&
	    kdb_trap(regs->trapno, regs->code, regs)) {
		thread_exception_return();
		/*NOTREACHED*/
	}
#endif	MACH_KDB

	exception(EXC_BAD_ACCESS, kr, regs->vaddr);
}

/*
 * Recovery from Successful fault in copyout does not
 * return directly - it retries the pte check, since
 * the 386 ignores write protection in kernel mode.
 * ... and we are running the 860 in emulation of that mode
 */
extern char		copyout_copy[];		/* copy instruction */
extern char		copyout_retry[];	/* retry starting here */

char *	trap_type[] = {
	"Debug trap",
	"System call",
	"Breakpoint",
	"Overflow",
	"Invalid opcode",
	"Page fault",
	"Floating point error",
	"Watchpoint",
	"Expired locked sequence",
	"Misaligned data access"
};
int	TRAP_TYPES = sizeof(trap_type)/sizeof(trap_type[0]);

#if	MACH_KDB
struct i860_saved_state *kdb_regs[NCPUS];
#endif


STATIC void	syscall(), kernel_trap(), user_trap();
STATIC int	emulated_syscall();

/*
 * arrive at sploff()
 * user_trap() and kernel_trap() generally splon(PSR_IM)
 * return at sploff().
 */
void
trap(register struct i860_saved_state *regs)
{

	if (regs->psr & PSR_PU)
		user_trap(regs);
	else
		kernel_trap(regs);

	sploff();
}


/*
 * Trap from kernel mode.  Only page-fault errors are recoverable,
 * and then only in special circumstances.  All other errors are
 * fatal.
 *
 */
struct i860_saved_state	*debug_last_kernel_trap;

STATIC void kernel_trap(regs)
	register struct i860_saved_state *regs;
{
	int	exc, code, subcode, type;
	vm_map_t	map;
	vm_prot_t	prot;
	kern_return_t	result;
	register thread_t	thread;

	debug_last_kernel_trap = regs;

	type = regs->trapno;
	code = regs->code;
	thread = current_thread();

#if	MACH_KDB
	kdb_regs[cpu_number()] = regs;
#endif

	assert(type != T_SYSCALL);	/* syscalls not permitted from supervisor mode */

	/*
	 * Service and return from breakpoint at sploff.
	 * Otherwise we couldn't breakpoint at sploff with interrupts pending
	 */
	if (type == T_BPTFLT)
	{
#if	MACH_KDB
		(void) kdb_trap(type, code, regs);
		{
			if ((void (*)()) regs->pc == gimmeabreak) {
				regs->pc += 4;
			}
		}
#endif	MACH_KDB
		return;
	}

	/*
	 * MCP traps get this far so that the type & codes are set up
	 * exactly like they are for application cpus.  But the MCP
	 * should never enable interrupts or handle faults, so skip ahead.
	 */
	{
		extern int	mcmsg_mp_enable;

		if ((cpu_number() == 1) && mcmsg_mp_enable)
			goto ktrap_death;
	}

	splon(PSR_IM);		/* enable interrupts */

	switch (type) {


	case T_PAGE_FAULT:
		/*
		 * If the current map is a submap of the kernel map,
		 * and the address is within that map, fault on that
		 * map.  If the same check is done in vm_fault
		 * (vm_map_lookup), we may deadlock on the kernel map
		 * lock.
		 */
		subcode = regs->vaddr;	/* get the faulting address */

		if (thread == THREAD_NULL)
			map = kernel_map;
		else {
			map = thread->task->map;
			if ((vm_offset_t)subcode < vm_map_min(map) ||
			    (vm_offset_t)subcode >= vm_map_max(map))
				map = kernel_map;
		}

#if	ASMP
		baton_enter();
#endif	/* ASMP */

		prot = VM_PROT_READ;
		if (code & T_PF_WRITE)
			prot |= VM_PROT_WRITE;
		result = vm_fault(map,
			trunc_page((vm_offset_t) subcode),
			prot,
			FALSE,
			FALSE,
			(void (*)()) 0);
		if (result == KERN_SUCCESS) {
#if	MACH_KDB
			if (db_watchpoint_list &&
				db_watchpoints_inserted &&
				(code & T_PF_WRITE) &&
				db_find_watchpoint(map,
					(vm_offset_t)subcode, regs))
				kdb_trap(T_WATCHPOINT, 0, regs);
#endif	MACH_KDB
#if	ASMP
			baton_exit();
#endif	/* ASMP */
			return;
		}

		/*
		 * recoverable fault? see user.s for copyin()
		 */
		{
			extern void	ALLOW_FAULT_START();
			extern void	FAULT_ERROR();

			if ((regs->pc >= (unsigned) ALLOW_FAULT_START) &&
			    (regs->pc < (unsigned) FAULT_ERROR)) {
				if (thread != THREAD_NULL) {
					if (thread->recover != (vm_offset_t)0) {
						regs->pc = thread->recover;
						thread->recover = 0;
#if	ASMP
					  	baton_exit();
#endif	/* ASMP */
						return;
					}
				}
			}

			/*
			 * raise an access exception if we have an exception
			 * port. Check here for an exception port, as kernel
			 * threads without a thread or task exception port can
			 * silently die. We really want to know when a kernel
			 * thread goes away! IP_NULL == 0.
			 */
			if (thread != THREAD_NULL) {
				if ( thread->ith_exception ||
				     thread->task->itk_exception )
				{
					exception(EXC_BAD_ACCESS, result,
								regs->vaddr);
#if	ASMP
					baton_exit();
#endif	/* ASMP */
					return;
				}
			}
		}

		/*
		 * Unanticipated page-fault errors in kernel
		 * should not happen.
		 */
		/* fall through */
	default:
		break;
	}

ktrap_death:

	printf("\nkernel fault address 0x%x at pc 0x%x\n", regs->vaddr, regs->pc);

#if	MACH_KDB
	if (kdb_trap(type, code, regs)) {
		return;
	}
#endif	MACH_KDB

	printf("kernel trap type %d, code = %x, pc = %x\n",
		type, code, regs->pc);
	panic("trap");
}

#if	PARAGON860
#if	MACH_KDB
void exception_debug_print(text, exception, code, subcode)
	char	*text;
	int	exception, code, subcode;
{
	thread_t 	self = current_thread();

	extern struct i860_saved_state	*debug_last_kernel_trap;

	db_printf("MK: %s exception: e=0x%x code=0x%x subcode=0x%x th=0x%x\n",
		text, exception, code, subcode, self);
	db_printf("MK: ... user-mode state: regs=0x%x\n",
		USER_REGS(self));
	show_860_regs(USER_REGS(self));

	db_printf("MK: ... kern-mode state: regs=0x%x\n",
		debug_last_kernel_trap);
	if (debug_last_kernel_trap != 0) {
		show_860_regs(debug_last_kernel_trap);
	}
}

void exception_debug_hint(exception, code, subcode)
	int	exception, code, subcode;
{
	struct i860_saved_state	*regs;
	unsigned		nybble;
	extern task_t		server_task;

	/*
	 *	If we're about to deliver an exception to the server,
	 *	dump out user and kernel thread state.
	 */
	if (current_task() == server_task) {
		exception_debug_print("server", exception, code, subcode);
		return;
	}

	/*
 	 *	If we're about to deliver an exception to something
	 *	that looks like it's running in the emulator, dump
	 *	out user and kernel thread state.
	 *
	 *	NOTE: this is a gross hack -- it just checks to see if
	 *	the PC is anywhere within 6xxxxxxx.
	 */
	regs = USER_REGS(current_thread());
	if (((regs->pc >> 28) & 0xf) == 0x6) {
		exception_debug_print("emulator", exception, code, subcode);
		return;
	}
}

#endif	MACH_KDB
#endif	PARAGON860

#ifdef	FORCE_MASTER
int	pagefaults_on_master=1;	/* can be overriden by bootmagic */
int	syscalls_on_master=1;	/* can be overriden by bootmagic */
#endif	/* FORCE_MASTER */

/*
 *	Trap from user mode.
 */
STATIC void user_trap(regs)
	register struct i860_saved_state *regs;
{
	register thread_t	thread;
	vm_map_t		map;
	kern_return_t		result;
	int			exc, code, subcode, type;

	splon(PSR_IM);	/* enable interrupts */

#if	NCPUS > 1
	/*
	 * allow exceptions to be handled on the current processor. Any
	 * manditory rescheduling is handled in by the exception specific
	 * routines.
	 */
#if	ASMP
	baton_enter();
	ASSERT_BATON_OWNER();
#endif	/* ASMP */
#endif	/* NCPUS > 1 */

	code = regs->code;
	thread = current_thread();

	/*
	 * dispatch on syscall number.
	 */
	switch ( (type = regs->trapno) ) {

	case T_SYSCALL:
#if FAST_EMUL_SYSCALL
		/* emulated syscalls are now detected in ttrap.s and
		 * dispatched there.
		 */
		if (emulated_syscall(regs)) {
			/* trampoline into the emulator */
			return;
		}
#endif
		/*
		 * handle the Mach syscall.
		 */
		syscall(regs);

		printf("user_trap: syscall(regs=%x) returned\n", regs);
		panic("user_trap");
		/*NOTREACHED*/

	case T_BPTFLT:
#if	MACH_KDB
	      {
		 boolean_t db_find_breakpoint_here();

                  /*
                   * is this a regular breakpoint set by kdb ??
                   */
		  if (db_find_breakpoint_here(
		     (current_thread())? current_thread()->task: TASK_NULL,
		      regs->pc)) {

		      if (kdb_trap(type, code, regs)) {

			  if ((void (*)()) regs->pc == gimmeabreak) {
			      regs->pc += 4;
			  }
			  return;
		      }
		  }
                  /*
                   * if not a regular breakpoint set by kdb is it a temp. breakpoint
                   */
                  else {
                      if(((db_not_taken_bkpt!=0)&&(db_not_taken_bkpt->address==regs->pc)) ||
                         ((db_taken_bkpt != 0)&&(db_taken_bkpt->address==regs->pc))) {
		         if (kdb_trap(type, code, regs)) {

			     if ((void (*)()) regs->pc == gimmeabreak) {
			         regs->pc += 4;
			     }
			     return;
		         }
                      }
	          }
              }
#endif	MACH_KDB
                /*
                 * not either kdb break point so must be a user break point
                 */
		exc = EXC_BREAKPOINT;
		code = 0;
		break;

	case T_OVERFLOW:
		exc = EXC_ARITHMETIC;
		code = 0;
		break;

	case T_LOCKEXPIRED:
	case T_INVALID_OPCODE:
		exc = EXC_BAD_INSTRUCTION;
		code = 0;
		break;

	case T_FLOATING_POINT_ERROR:
		exc = EXC_ARITHMETIC,
		code = 0;
		break;

       	case  T_UNALIGNED:
		exc = EXC_BAD_ACCESS;
		code = 0;
		subcode = regs->vaddr;
		break;

	case T_PAGE_FAULT:

#ifdef	FORCE_MASTER
		/*
		 * reschedule to master_cpu if required at this level, otherwise
		 * a reschedule may happen at a lower level. Attempt to be
		 * parallel most of the time.
		 */
		if ( pagefaults_on_master ) {
			FORCE_MASTER_CPU();
			thread = current_thread();
		}
		ASSERT_BATON_OWNER();
#endif	/* FORCE_MASTER */
		subcode = regs->vaddr;
		(void) vm_fault(thread->task->map,
				trunc_page((vm_offset_t)subcode),
				(code & T_PF_WRITE)
					? VM_PROT_READ|VM_PROT_WRITE
					: VM_PROT_READ,
				FALSE,
				FALSE,
				user_page_fault_continue);
		/*NOTREACHED*/
		break;
		
	default:

#if	MACH_KDB
		if (kdb_trap(type, code, regs)) {
		    return;
		}
#endif	MACH_KDB
		break;
	}

#if	MACH_KDB
	if (debug_all_traps_with_kdb) {
		(void) kdb_trap(type, code, regs);
	}
	else
#endif	MACH_KDB

#ifdef	FORCE_MASTER
	FORCE_MASTER_CPU();
#endif	/* FORCE_MASTER */
	exception(exc, code, subcode);
	/*NOTREACHED*/

}


#if	MACH_KDB
/*
 * KDB kernel debugger interrupt, fake like it was a keyboard interrupt '^p'.
 *
 * inputs:
 *	regs	pointer to an i860 saved exception state
 */

kdb_kintr(regs)
	 register struct i860_saved_state *regs;
{
	kdb_kbd_trap(regs);
}
#endif

#if	FAST_EMUL_SYSCALL

/***************************************************************************
 *
 *  Name: emulated_syscall(regs)
 *
 *  Description: Determine if the trap is for an emulated system call and
 *               if so, fix up the registers so that upon return from the
 *               trap handler, control is transfered to the emulator.  The
 *               function returns TRUE in this case.
 *
 *               Otherwise this is a mach kernel call and return a function
 *               result of FALSE.
 *
 *  Return Value: (int) TRUE     - System call is an emulated call.
 *                      FALSE    - System call is a mach kernel call.
 *
 ***************************************************************************/

int
emulated_syscall(regs)
	register struct i860_saved_state *regs;
{
	register eml_dispatch_t		emd;
	register int      		dispatch_index;
	int				code;	/* System call code. */
	eml_routine_t			emul_entry;

	/* Check if emulator dispatch table is defined. */
	if ( (emd = current_task()->eml_dispatch) == EML_DISPATCH_NULL ) {
		/* No dispatch table, let the mach kernel handle it. */
		return FALSE;
	}

	code = (int) regs->r31;  /* Get syscall code. */

	/* Calculate the index into the dispatch table. */
	if ( (dispatch_index = code - emd->disp_min) < 0) {
		/* Not an emulated syscall, let the mach kernel handle it. */
		return FALSE;
	}

	/* Make sure the syscall is not out of range of the dispatch table. */
	if (dispatch_index >= emd->disp_count) {
		/* Not an emulated syscall, let the mach kernel handle it. */
		return FALSE;
	}

	/* If the vector entry is NULL then it is also treated as native. */
	if ((emul_entry=emd->disp_vector[dispatch_index]) == EML_ROUTINE_NULL) {
		return FALSE;
	}

	/* 
	 * It's an emulated system call.  Fixup the registers so that the
	 * trap handler will return to the emulator instead of the user code
	 * which did the trap.  We sacrifice the contents of r29 as a hook
	 * for the emulator to return to the instruction after the trap.
	 * (r29 is volatile anyway).  The alternative was stashing the
	 * equivalent of the fir (the pc) address on the stack somewhere.
	 */
	regs->r29 = regs->pc + 4;
	regs->pc = (unsigned int) emul_entry;

	return TRUE;
}
#endif	/* FAST_EMUL_SYSCALL */


#if	NOCACHE
int	flush_on_syscall = 0;
#else	NOCACHE
#if	PARAGON860
int	flush_on_syscall = 0;
#else	PARAGON860
int	flush_on_syscall = 1;
#endif	PARAGON860
#endif	NOCACHE

#ifdef	FORCE_MASTER
int	max_psyscall=75;	/* max/min parallel Mach syscall #'s */
int	min_psyscall=26;
#endif	FORCE_MASTER

/*
 * Perform a Mach 3.0 system call
 *
 * NCPUS > 1 case we can be on another CPU other than the master_cpu.
 */

STATIC void syscall(regs)
	register struct i860_saved_state *regs;
{
	register int		code;
	mach_trap_t		*traptab;
	unsigned int		*a, ret;
	int			(*f)();

	/*
	 * range check the syscall code and retrieve the syscall function
	 * pointer. If out-of-range then become an invalid syscall.
	 */
	code = - (int) regs->r31;
	if (code < 0 || code >= mach_trap_count)
		code = 0;	/* XXX */
	traptab = &mach_trap_table[code];

#ifdef	FORCE_MASTER
	/*
	 * forcing ALL Mach syscalls on the master processor?
	 */
	if ( syscalls_on_master ) {
		FORCE_MASTER_CPU();
       		MUST_BE_MASTER("syscall()",my_caller());
	}
	else {
		/* 
		 * range check the syscall number (see kern/syscall_sw.c) to
		 * determine if we can run in parallel. Determination has so
		 * far been empirical.....DO NOT let mach_msg_trap() run in
		 * parallel as MUCH NORMA work(surgery) needs to be performed.
		 * 	76 fails (hangs in MU boot)??
		 */
		if ((code > 19 && code < min_psyscall) || (code > max_psyscall))
		{
			FORCE_MASTER_CPU();
        		MUST_BE_MASTER("syscall()",my_caller());
		}
	}
#endif	FORCE_MASTER

#if	MACH_ASSERT
	if (trap_syscall_trace) {
		unsigned int	i, ac;

		ac = traptab->mach_trap_arg_count;
		a = &regs->r16;
		db_printf("%s syscall %d: *%x(",
			(regs->psr & PSR_PU) ? "user" : "kern",
			code,
			traptab->mach_trap_function);
		for (i = 0; i < ac; i++) {
			db_printf("0x%x", a[i]);
			if (i != (ac - 1)) {
				db_printf(",");
			}
		}
		db_printf(")\n");
	}
#endif	/* MACH_ASSERT */

	if (flush_on_syscall) flush();

	a = &regs->r16;
	f = traptab->mach_trap_function;
	switch (traptab->mach_trap_arg_count) {
	case  0:
		ret = (*f)();
		break;

	case  1:
		ret = (*f)(a[0]);
		break;

	case  2:
		ret = (*f)(a[0], a[1]);
		break;

	case  3:
		ret = (*f)(a[0], a[1], a[2]);
		break;

	case  4:
		ret = (*f)(a[0], a[1], a[2], a[3]);
		break;

	case  5:
		ret = (*f)(a[0], a[1], a[2], a[3],
		     a[4]);
		break;

	case  6:
		ret = (*f)(a[0], a[1], a[2], a[3],
			   a[4], a[5]);
		break;

	case  7:
		ret = (*f)(a[0], a[1], a[2], a[3],
			   a[4], a[5], a[6]);
		break;

	case  8:
		ret = (*f)(a[0], a[1], a[2], a[3],
			   a[4], a[5], a[6], a[7]);
		break;

	case  9:
		ret = (*f)(a[0], a[1], a[2], a[3],
			   a[4], a[5], a[6], a[7],
			   a[8]);
		break;

	case 10:
		ret = (*f)(a[0], a[1], a[2], a[3],
			   a[4], a[5], a[6], a[7],
			   a[8], a[9]);
		break;

	case 11:
		ret = (*f)(a[0], a[1], a[2], a[3],
			   a[4], a[5], a[6], a[7],
			   a[8], a[9], a[10]);
		break;

	case 12:
		ret = (*f)(a[0], a[1], a[2], a[3],
			   a[4], a[5], a[6], a[7],
			   a[8], a[9], a[10], a[11]);
		break;

	default:
		panic("too many args to mach_trap");
		break;
	}

	if (regs->psr & PSR_PU) {
		thread_syscall_return(ret);
		/*NOTREACHED*/
	} else {
		regs->r16 = ret;
		printf("syscall: from a kernel thread? regs=%x, ret=%x\n",
			regs, ret);
		panic("syscall from a kernel thread");
	}
}


#define REDZONE	2048

static int	no_kernel_stack_check = 0;

int kernel_stack_check(regs)
	register struct i860_saved_state *regs;
{
	vm_offset_t	base, kstk;
	thread_t	self;

	if (regs->psr & PSR_PU)
		return 0;
	self = current_thread();
	if (!self)
		return 0;
	if (no_kernel_stack_check)
		return 0;
	base = self->kernel_stack;
	kstk = (vm_offset_t) regs->sp;
	if ((kstk > (base + REDZONE)) && (kstk < (base + KERNEL_STACK_SIZE)))
		return 0;
	printf("kernel_stack_check: regs=%x, pc=%x, sp=%x, kernel stack=%x\n",
		regs, regs->pc, kstk, base);
	no_kernel_stack_check++;
#if	MACH_KDB
	gimmeabreak();
#endif	MACH_KDB
	return 1;
}

#if 0	/* this code has been moved to user.s, rtn: copy{in/out}() */
/*
 * set thread recover address
 *
 *	During copyin() the error recovery rtn address is stashed in
 *	current_thread()->recover. If a fault is taken during copyin() then
 *	return to the caller of copyin() with an error code of -1. See
 *	user.s, copyin().
 */
set_thread_recover()
{
	register thread_t	th;
	extern void		FAULT_ERROR();

	if ( (th=current_thread()) != THREAD_NULL ) {
		th->recover = (vm_offset_t)FAULT_ERROR;
	}
}

clear_thread_recover()
{
	register thread_t	th;

	if ( (th=current_thread()) != THREAD_NULL ) {
		th->recover = 0;
	}
}
#endif
