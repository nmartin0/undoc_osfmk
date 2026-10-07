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
 * $Id: pcb.c,v 2.34 1995/04/04 21:22:46 lenb Exp $
 */

#include <cpus.h>
#include <mach_kdb.h>
#include <mach_assert.h>

#include <mach/thread_status.h>
#include <mach/kern_return.h>
#include <mach/boolean.h>

#include <kern/counters.h>
#include <kern/thread.h>
#include <kern/mach_param.h>
#include <mach/vm_param.h>
#include <i860/thread.h>
#include <i860/psl.h>
#include <i860/setjmp.h>
#include <i860/cpu_master.h>

#include <i860/fpe/fpe.h>

#if	MACH_KDB
#include <ddb/db_sym.h>
#endif	MACH_KDB

#if	PARAGON860
#include <i860paragon/baton.h>
#endif	PARAGON860

/* This is the number of bytes above an initial kernel stack  */
#define I860_STACK_COOKIES	(sizeof(struct i860_exception_link) + \
			sizeof(struct i860_kernel_state))

/* keep the pcb's aligned on this boundary */
#define ALIGNMENT	16

/* top of the kernel stack */
#define TOPOF(base)	(((base)+KERNEL_STACK_SIZE - I860_STACK_COOKIES) & ~0xf)

/* cache debugging */
#if	NOCACHE
#if     PARAGON860
int     flush_on_syscall_return = 1;
int     flush_on_exception_return = 1;
int     flush_on_bootstrap_return = 1;
#else   PARAGON860
int	flush_on_syscall_return = 0;
int	flush_on_exception_return = 0;
int	flush_on_bootstrap_return = 0;
#endif  PARAGON860
#else	NOCACHE
#if     PARAGON860
int     flush_on_syscall_return = 0;
int     flush_on_exception_return = 0;
int     flush_on_bootstrap_return = 0;
#else   PARAGON860
int	flush_on_syscall_return = 0;
int	flush_on_exception_return = 0;
int	flush_on_bootstrap_return = 0;
#endif  PARAGON860
#endif	NOCACHE

/*
 *	Default policy for misaligned accesses.
 */
int	i860_misalignment_policy = 1;	/* 1=signal, 0=handle */

void	change_address_space();	/* forward... */

vm_offset_t	kernel_stack[NCPUS];
pcb_t		current_pcb[NCPUS];

#define	PARANOIDPCB	0	/* track thread in ks->ks_r0 */
#define	PREFAULT_PC	0	/* XXX copyin(*(fir-4)...) workaround XXX */

zone_t pcb_zone;

/* install thread "th" the current thread */

#if	ASMP
#define INSTALL_THREAD(th)	\
	{ register int cpu = cpu_number(), s = sploff(); \
	active_threads[cpu]->baton_level = kernel_baton.level;	\
	active_threads[cpu] = (th);			\
	kernel_baton.level = (th)->baton_level;		\
	active_stacks[cpu] = (th)->kernel_stack;	\
	current_pcb[cpu] = (th)->pcb;			\
	kernel_stack[cpu] = TOPOF((th)->kernel_stack);	\
	splon(s);					\
	}
#else	/* !ASMP */
#define INSTALL_THREAD(th)	\
	{ register int cpu = cpu_number(), s = sploff(); \
	active_threads[cpu] = (th);			\
	active_stacks[cpu] = (th)->kernel_stack;	\
	current_pcb[cpu] = (th)->pcb;			\
	kernel_stack[cpu] = TOPOF((th)->kernel_stack);	\
	splon(s);					\
	}
#endif	/* !ASMP */


#if	MACH_KDB && MACH_ASSERT && DB_MACHINE_COMMANDS

#define	SWITCH_HISTORY	1
#if	SWITCH_HISTORY

#define	SWITCH_HISTLEN	32

struct switch_history {
	unsigned int	sh_flag;	/* what kind of record */
	unsigned long	sh_nth;		/* nth call to switch_context() */
	unsigned long	sh_trapn;	/* nth ctrap */
	unsigned long	sh_astn;	/* nth ast */
	thread_t	sh_curth;	/* current_thread() */
	vm_offset_t	sh_cursp;	/* current_stack() */
	thread_t	sh_old;		/* old thread */
	int		sh_old_state;	/* old thread's state */
	vm_offset_t	sh_old_kstack;	/* old thread's kernel stack */
	void		(*sh_cont)();	/* continuation for old thread */
	thread_t	sh_new;		/* new thread */
	int		sh_new_state;	/* new thread's state */
	vm_offset_t	sh_new_kstack;	/* new thread's kernel stack */
	unsigned long	sh_r1;		/* new thread's kernel regs */
	unsigned long	sh_sp;		/* new thread's kernel regs */
	unsigned long	sh_fp;		/* new thread's kernel regs */
};
unsigned long switch_context_total;
int	switch_history_next;
int	switch_history_enabled = 0;
static struct switch_history switch_history[SWITCH_HISTLEN];

#define	LOG_SWITCH	0
#define	LOG_HANDOFF	1
#define LOG_ATTACH	2
#define LOG_DETACH	3
#define LOG_CONTINUE	4
#define LOG_SYSCALLRET	5
#define LOG_EXCEPTRET	6
#define LOG_BOOTRET	7

/*
 *	runtime patchable...
 */
#define switch_log(meth,old,cont,new,newks)	\
	if (switch_history_enabled) _switch_log(meth,old,cont,new,newks)

static void _switch_log(method, old, continuation, new, newks)
int		method;
thread_t	old;
void		(*continuation)();
thread_t	new;
struct i860_kernel_state *newks;
{
	struct switch_history *sh;

	extern unsigned long	taken_ast_count;

	sh = &switch_history[switch_history_next++];
	if (switch_history_next >= SWITCH_HISTLEN)
		switch_history_next = 0;

	sh->sh_flag = method;
	sh->sh_nth = switch_context_total++;
 
	sh->sh_trapn = 0;	/* recorded per-cpu in rpmsoft, see rpm.h */
	sh->sh_astn = taken_ast_count;
	sh->sh_curth = current_thread();
#if 0	/* code from sjs */
	sh->sh_cursp = new ?  USER_REGS(new)->sp : current_stack ();
	if (new && sh->sh_cursp == 0)   /* only happens if sp unassigned */
		sh->sh_cursp = (vm_offset_t)USER_REGS(new);
#else
	sh->sh_cursp = current_stack();
#endif
	sh->sh_old = old;
	sh->sh_old_state = old->state;
	sh->sh_old_kstack = old->kernel_stack;
	sh->sh_new = new;

	switch (method) {
	case LOG_SWITCH:
		sh->sh_new_state = new->state;
		sh->sh_new_kstack = new->kernel_stack;
		sh->sh_cont = continuation;
		sh->sh_r1 = newks->ks_r1;
		sh->sh_sp = newks->ks_sp;
		sh->sh_fp = newks->ks_fp;
		break;

	case LOG_HANDOFF:
		sh->sh_new_state = new->state;
		sh->sh_new_kstack = new->kernel_stack;
		break;

	case LOG_ATTACH:
	case LOG_CONTINUE:
		sh->sh_cont = continuation;
	case LOG_DETACH:
	case LOG_SYSCALLRET:
	case LOG_EXCEPTRET:
	case LOG_BOOTRET:
		break;
	}
}
#endif	/* SWITCH_HISTORY */

#else	/* MACH_KDB && MACH_ASSERT && DB_MACHINE_COMMANDS */

#define	switch_log(method, thread, continuation, a, b)

#endif	/* MACH_KDB && MACH_ASSERT && DB_MACHINE_COMMANDS */


/*
 *	From: Andy Pfiffer, Intel SSD
 *	Big-time overhaul for i860 stack handoff support.
 *
 *	Block comments above some of the functions were copied
 *	from the descriptions given in:
 *
 *		"Using Continuations to Implement Thread Management
 *		 and Communication in Operating Systems",
 *		R. Draves, B. Bershad, R. Rashid, R. Dean.
 */

/*
 *	stack_attach:
 *
 *	Transforms a machine-independent continuation into a machine-
 *	dependent kernel stack, attaching the kernel stack to the thread
 *	and initializing the stack so that when switch_context() resumes
 *	the thread, control transfers to the supplied continuation function
 *	with the previously running thread as an argument.
 *
 */
void stack_attach(thread, stack, continuation)
	thread_t	thread;
	vm_offset_t	stack;
	void		(*continuation)();
{
	register struct i860_kernel_state *ks;

	counter(if (++c_stacks_current > c_stacks_max)
			c_stacks_max = c_stacks_current);

	assert(stack != (vm_offset_t) 0);
	assert(thread->kernel_stack == (vm_offset_t) 0);
	assert(continuation != (void (*)()) 0);
	assert(thread != current_thread());

#if	MACH_ASSERT
	{
		int		dummy;
		static int	inrange();

		if (current_stack() && !inrange(&dummy, current_stack())) {
			printf("stack_attach(th=0x%x,stack=0x%x,cont=0x%x):\n",
				thread, stack, continuation);
			printf("&dummy = 0x%x\n", &dummy);
			printf("[0x%x to 0x%x]\n",
				current_stack(),
				current_stack() + KERNEL_STACK_SIZE);
			assert(0);
		}
	}
#endif	MACH_ASSERT

	thread->kernel_stack = stack;

	ks = STACK_I860KS(stack);

#if	PARANOIDPCB
	/*
	 *	XXX the following assertion can falsely trigger
	 *	XXX when given a brand-new stack when
	 *	XXX stack_alloc() calls kmem_alloc_aligned()
	 *	XXX or when stack_check_usage == TRUE.
	 */
	/*assert(ks->ks_r0 == 0);*/
	if (ks->ks_r0 != 0) {
#if	0
		printf("stack_attach(th=0x%x,st=0x%x,cont=0x%x):\n",
			thread, stack, continuation);
		printf("    ks=0x%x, ks->ks_r0 == 0x%x\n", ks, ks->ks_r0);
#endif	0
	}
	ks->ks_r0 = (unsigned) thread;
#endif	PARANOIDPCB

	ks->ks_r1 = (unsigned) continuation;
	ks->ks_sp = (unsigned) TOPOF(stack);
	ks->ks_fp = 0;

	STACK_I860EL(stack)->eframe = USER_REGS(thread);

	switch_log(LOG_ATTACH, thread, continuation, 0, 0);
}


/*
 *	stack_detach:
 *
 *	Detaches and returns the thread's kernel stack.
 *
 */
vm_offset_t stack_detach(thread)
	register thread_t	thread;
{
	register vm_offset_t	stack;

	counter(if (--c_stacks_current < c_stacks_min)
			c_stacks_min = c_stacks_current);

	assert(thread != current_thread());
	assert(thread->kernel_stack != current_stack());
	assert(thread->kernel_stack != (vm_offset_t) 0);

	stack = thread->kernel_stack;
	thread->kernel_stack = 0;

#if	PARANOIDPCB
	{
		register struct i860_kernel_state *ks;

		ks = STACK_I860KS(stack);
		assert(ks->ks_r0 == (unsigned) thread);
		ks->ks_r0 = 0;
	}
#endif	PARANOIDPCB

	switch_log(LOG_DETACH, thread, 0, 0, 0);
	return stack;
}


/*
 *	stack_handoff:
 *
 *	Does a stack handoff, moving the current kernel stack from the
 *	current thread to the new thread.  stack_handoff() changes
 *	address spaces if necessary.  stack_handoff() returns as the
 *	new thread.
 */
void stack_handoff(old, new)
	register thread_t	old;
	register thread_t	new;
{
	register vm_offset_t	stack;

	assert(get_spl() > 0);
	assert(old == current_thread());
	assert(old->kernel_stack == current_stack());
	assert(old->kernel_stack != (vm_offset_t) 0);
	assert(USER_REGS(old) == (STACK_I860EL(old->kernel_stack)->eframe));
	assert(new->kernel_stack == (vm_offset_t) 0);

	stack = old->kernel_stack;
	new->kernel_stack = stack;
	old->kernel_stack = (vm_offset_t) 0;

#if	PARANOIDPCB
	{
		register struct i860_kernel_state *ks;

		ks = STACK_I860KS(stack);
		assert(ks->ks_r0 == (unsigned) old);
		ks->ks_r0 = (unsigned) new;
	}
#endif	PARANOIDPCB

	if (old->task != new->task)
		change_address_space(old, new);

	INSTALL_THREAD(new);

	STACK_I860EL(stack)->eframe = USER_REGS(new);

	switch_log(LOG_HANDOFF, old, 0, new, 0);
}

void load_context(new)
	thread_t	new;
{
	INSTALL_THREAD(new);
	i860_context_load(STACK_I860KS(new->kernel_stack), 0);
	/*NOTREACHED*/
}


#if	MACH_ASSERT
static int inrange(addr, base)
vm_offset_t	addr, base;
{
	return ((addr > base) && (addr < (base + KERNEL_STACK_SIZE)));
}
#endif	MACH_ASSERT


/*
 *	export a routine that can be called to range check the
 *	the current stack pointer against current_thread()->kernel_stack.
 */
int	stack_assert_red = 1024;
boolean_t stack_assert_bounds()
{
	vm_offset_t	stack;
	int		insp;

	if ((stack = current_stack()) != 0) {
		assert((vm_offset_t) &insp > (stack + stack_assert_red));
		assert((vm_offset_t) &insp < (stack + KERNEL_STACK_SIZE));
		return  ((vm_offset_t) &insp > (stack + stack_assert_red)) &&
			((vm_offset_t) &insp < (stack + KERNEL_STACK_SIZE));
	}
	return TRUE;
}



#if	PREFAULT_PC
/*
 *	XXX WORKAROUND:
 *	XXX
 *	XXX Read the instruction at (fir - 4) to prefault
 *	XXX in the text if needed.
 */
void i860_prefault_pc(regs)
	register struct i860_saved_state *regs;
{
	unsigned int	instruction;

	copyin(regs->pc - 4, &instruction, sizeof(unsigned int));
}
#endif	/* PREFAULT_PC */


/*
 *	switch_context:
 *
 *	Resumes the new thread on its preserved kernel stack.
 *	This call changes address spaces if necessary.  If a
 *	continuation for the thread is supplied, then switch_context()
 *	does not save registers and does not return.  Otherwise,
 *	switch_context() saves the current thread's register state
 *	and kernel stack and returns when the calling thread is
 *	rescheduled, returning the previously running thread.
 */
thread_t switch_context(old, continuation, new)
	register thread_t	old;
	void (*continuation)();
	register thread_t	new;
{
	register struct i860_kernel_state *newks, *oldks;
	extern thread_t	i860_context_switch();

	assert(get_spl() > 0);

	assert(new->kernel_stack != (vm_offset_t) 0);
	assert(old->kernel_stack != (vm_offset_t) 0);

	assert(old == current_thread());
	assert(old->kernel_stack == current_stack());

#if	MACH_ASSERT
	{
		int	dummy;

		if (!inrange(&dummy, current_stack())) {
			printf("switch_context(old=0x%x,cont=0x%x,new=0x%x):\n",
				old, continuation, new);
			printf("&dummy = 0x%x\n", &dummy);
			printf("[0x%x to 0x%x]\n",
				current_stack(),
				current_stack() + KERNEL_STACK_SIZE);
			assert(0);
		}
	}
#endif	MACH_ASSERT

	oldks = STACK_I860KS(old->kernel_stack);
	newks = STACK_I860KS(new->kernel_stack);
#if	PARANOIDPCB
	assert(oldks->ks_r0 == (unsigned) old);
	assert(newks->ks_r0 == (unsigned) new);
#endif	PARANOIDPCB
	assert(inrange(newks->ks_sp, new->kernel_stack));

	old->swap_func = continuation;

	if (old->task != new->task)
		change_address_space(old, new);

	INSTALL_THREAD(new);

	switch_log(LOG_SWITCH, old, continuation, new, newks);

	return i860_context_switch(newks, continuation, oldks, old);
}

/*
 *	thread_set_syscall_return:
 *
 *	Alter the thread's state so that a following thread_exception_return
 *	will make the thread return 'retval' from a syscall(r16).
 *	Utilized by mach/mach_msg.c; mach_msg_interrupt()
 */
void thread_set_syscall_return(thread, retval)
	thread_t	thread;
	kern_return_t	retval;
{
	register struct i860_saved_state *regs;

	assert(thread->pcb != 0);
	regs = USER_REGS(thread);
	regs->r16 = retval;
	regs->pc += 4;		/* step past the trap instruction */
}

#define	FIXED_PGC_COMPILER	0
#if	!FIXED_PGC_COMPILER
/*
 *	PGI's optimizer generates incorrect code when
 *	the macro's are expanded inline but works okay
 *	when they're inside a subroutine call.
 */
void change_pmaps(opmap, oth, npmap, nth)
	register pmap_t		opmap, npmap;	/* old, new */
	register thread_t	oth, nth;	/* old, new */
{
	register int	cpu = cpu_number();

	PMAP_DEACTIVATE_USER(opmap, oth, cpu);
	PMAP_ACTIVATE_USER(npmap, nth, cpu);

}
#endif	!FIXED_PGC_COMPILER


/*
 *	change_address_space:
 *
 *	Perform pmap invalidation if switching tasks.
 */

void change_address_space(old, new)
	register thread_t	old;
	register thread_t	new;
{
	task_t old_task, new_task;

	if ((old_task = old->task) != (new_task = new->task)) {

		/*
		 * if same physical map (pmap) then skip the needless change
		 */
		if ( vm_map_pmap(old_task->map) == vm_map_pmap(new_task->map) )
			return;

#if	FIXED_PGC_COMPILER

		{ register int	cpu = cpu_number();

		PMAP_DEACTIVATE_USER(vm_map_pmap(old_task->map), old, cpu);
		PMAP_ACTIVATE_USER(vm_map_pmap(new_task->map), new, cpu);
		}

#else	FIXED_PGC_COMPILER 

		change_pmaps( vm_map_pmap(old_task->map), old,
			      vm_map_pmap(new_task->map), new);

#endif	FIXED_PGC_COMPILER

	}
}


/*
 *	Initialize the pcb zone
 *
 *	We enforce a 16-byte alignment restriction on every pcb
 *	as that will permit fst.q and friends when saving and
 *	restoring context.
 */
void pcb_module_init()
{
	int	size;

	size = (sizeof(struct pcb) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);

	pcb_zone = zinit(size, THREAD_MAX * size, THREAD_CHUNK * size,
			FALSE, "i860 pcb state");
	zaligned(pcb_zone);
}


pcb_t pcb_free_list;		/* list of unused pcb structures */
unsigned int pcb_free_count;	/* size of the list, for debugging */

/*
 *	Allocate a pcb.
 *
 *	Try the free list first, then try the zone.  Always check
 *	the alignment...
 */
pcb_t pcb_alloc()
{
	register pcb_t	pcb;

	if ((pcb = pcb_free_list) != 0) {
		pcb_free_list = *((pcb_t *)pcb);
		pcb_free_count--;
		if ((int) pcb & (ALIGNMENT-1))
			panic("pcb_alloc: misaligned pcb on free list");
		return pcb;
	}

	pcb = (pcb_t) zalloc(pcb_zone);
	if (pcb == 0)
		panic("pcb_alloc");
	if ((int) pcb & (ALIGNMENT-1))
		panic("pcb_alloc: misaligned pcb returned from zalloc");

	return pcb;

}


/*
 *	Return a pcb to the free list.
 */
void pcb_free(pcb)
	register pcb_t	pcb;
{
	pcb_free_count++;
	*((pcb_t *) pcb) = pcb_free_list;
	pcb_free_list = pcb;
}


/*
 *	Initialize a pcb for a thread.
 */
void pcb_init(thread)
	register thread_t	thread;
{
	register pcb_t		pcb;

	/*
	 *	Allocate a pcb.
	 */
	pcb = pcb_alloc();
	thread->pcb = pcb;

	counter(if (++c_threads_current > c_threads_max)
		c_threads_max = c_threads_current);

	/*
	 *	We can't let random values leak out to the user.
	 */
	bzero((char *) pcb, sizeof *pcb);

	/*
	 *	Make the thread run in user mode,
	 *	if it ever comes back out of the kernel.
	 */
	pcb->i860ss.psr = PSR_PU | PSR_PIM;
	pcb->i860ss.pc = 0;	/* set to 0 to stop stack backtrace */

	pcb->i860ss.fsr = FSR_FTE; /* enable floating Point traps */
	pcb->ieee_status = IEEE_MASKS;
	pcb->flags = PCB_IEEE_FP | PCB_SIGNAL_PFLDQ;

	/*
	 *	Default (initial) setting for misaligned accesses.
	 */
	if (i860_misalignment_policy)
		pcb->flags |= PCB_SIGNAL_MA;
	else
		pcb->flags &= ~PCB_SIGNAL_MA;

}


/*
 *	Terminate a thread's pcb.
 *
 *	Return the pcb to the free list, and unlink the
 *	pcb from the thread.
 */
void pcb_terminate(thread)
	register thread_t thread;
{
	register pcb_t pcb = thread->pcb;

	counter(if (--c_threads_current < c_threads_mint)
			c_threads_min = c_threads_current);

	/*
	 * kernel threads no longer have a pcb allocated.
	 */
	if ( pcb ) {
		pcb_free(pcb);
		thread->pcb = 0;
	}
}


/*
 *	pcb_collect:
 *
 *	Attempt to free excess pcb memory.
 */

void pcb_collect(thread)
	thread_t thread;
{
	register pcb_t	pcb;

	while (pcb_free_count > 0) {
		pcb = pcb_free_list;
		pcb_free_list = *((pcb_t *)pcb);
		pcb_free_count--;
		zfree(pcb_zone, (vm_offset_t) pcb);
	}
}


/*
 *	thread_setstatus:
 *
 *	Set the status of the given thread.
 */

kern_return_t thread_setstatus(thread, flavor, tstate, count)
	thread_t		thread;
	int			flavor;
	thread_state_t		tstate;
	unsigned int		count;
{
	switch (flavor) {
	case i860_THREAD_STATE: {
		register struct i860_saved_state *i860ss;
		register struct i860_thread_state *state;

		if (count != i860_THREAD_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		assert(thread->pcb != 0);
		i860ss = USER_REGS(thread);
		state = (struct i860_thread_state *) tstate;

		bcopy( &state->r0, &i860ss->r0, 64 * sizeof(int) );

		/*
		 * XXX bcopy() is used below because the alignment of
		 * XXX "state" may not be trusted (although it seems to
		 * XXX be 4-byte aligned...).
		 *
		 * XXX If i860/thread.h and mach/i860/thread_status.h
		 * XXX matched, none of this would be a problem.
		 */

		/* 3 stage load pipe */
		bcopy(&state->psv_l1[0], &i860ss->psv_l1[0], 6*sizeof(double));

		/* 3 stage adder pipe */
		bcopy(&state->psv_a1, &i860ss->psv_a1, 3 * sizeof(double));

		/* 3 stage multiplier pipe */
		bcopy(&state->psv_m1, &i860ss->psv_m1, 3 * sizeof(double));

		/* 1 stage interpolator(?) pipe */
		bcopy(&state->psv_i1, &i860ss->psv_i1, 1 * sizeof(double));

		/* T, KR, KI, merge */
		bcopy(&state->spc_t, &i860ss->spc_t, sizeof(double));
		bcopy(&state->spc_ki, &i860ss->spc_ki, sizeof(double));
		bcopy(&state->spc_kr, &i860ss->spc_kr, sizeof(double));
		bcopy(&state->spc_merge, &i860ss->spc_merge, sizeof(double));

		/* 3 stages of fsr */
		i860ss->fsr1 = state->fsr1;
		i860ss->fsr2 = state->fsr2;
		i860ss->fsr = state->fsr;

		/* other special registers of interest */
		i860ss->pc = state->pc;
		i860ss->psr = state->psr | (PSR_PU|PSR_PIM);
		i860ss->epsr = get_epsr();	/* no user access */
		i860ss->db = state->db;

		/* FPE handler values */
		i860ss->trapped_opcode = state->fpe_trapped_op;
		i860ss->rdest = state->fpe_rdest;
		i860ss->src1 = state->fpe_src1;
#if	FIXED_SERVER_IEEE_STATUS
		thread->pcb->ieee_status = state->fpe_ieee_status;
#endif	FIXED_SERVER_IEEE_STATUS
		bcopy(&state->fp1, &i860ss->fp1, 4 * sizeof(double));

		break;
	}

	default:
		return (KERN_INVALID_ARGUMENT);
	}

	return(KERN_SUCCESS);
}


/*
 *	thread_getstatus:
 *
 *	Get the status of the specified thread.
 */

kern_return_t thread_getstatus(thread, flavor, tstate, count)
	register thread_t	thread;
	int			flavor;
	thread_state_t		tstate;
	unsigned int		*count;
{
	switch (flavor) {
	case THREAD_STATE_FLAVOR_LIST:
		if (*count < 1)
			return (KERN_INVALID_ARGUMENT);

		tstate[0] = i860_THREAD_STATE;
		*count = 1;
		break;

	case i860_THREAD_STATE: {
		register struct i860_saved_state *i860ss;
		register struct i860_thread_state *state;

		if (*count < i860_THREAD_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		assert(thread->pcb != 0);
		i860ss = USER_REGS(thread);
		state = (struct i860_thread_state *) tstate;

		bcopy( &i860ss->r0, &state->r0, 64 * sizeof(int) );

		/*
		 * XXX bcopy() is used below because the alignment of
		 * XXX "state" may not be trusted (although it seems to
		 * XXX be 4-byte aligned...).
		 *
		 * XXX If i860/thread.h and mach/i860/thread_status.h
		 * XXX matched, none of this would be a problem.
		 */

		/* 3 stage load pipe */
		bcopy(&i860ss->psv_l1[0], &state->psv_l1[0], 6*sizeof(double));

		/* 3 stage adder pipe */
		bcopy(&i860ss->psv_a1, &state->psv_a1, 3 * sizeof(double));

		/* 3 stage multiplier pipe */
		bcopy(&i860ss->psv_m1, &state->psv_m1, 3 * sizeof(double));

		/* 1 stage interpolator(?) pipe */
		bcopy(&i860ss->psv_i1, &state->psv_i1, 1 * sizeof(double));

		/* T, KR, KI, merge */
		bcopy(&i860ss->spc_t, &state->spc_t, sizeof(double));
		bcopy(&i860ss->spc_ki, &state->spc_ki, sizeof(double));
		bcopy(&i860ss->spc_kr, &state->spc_kr, sizeof(double));
		bcopy(&i860ss->spc_merge, &state->spc_merge, sizeof(double));

		/* 3 stages of fsr */
		state->fsr1 = i860ss->fsr1;
		state->fsr2 = i860ss->fsr2;
		state->fsr = i860ss->fsr;

		/* other special registers of interest */
		state->pc = i860ss->pc;
		state->psr = i860ss->psr;
		state->epsr = i860ss->epsr;
		state->db = i860ss->db;

		/* FPE handler values */
		state->fpe_trapped_op = i860ss->trapped_opcode;
		state->fpe_rdest = i860ss->rdest;
		state->fpe_src1 = i860ss->src1;
		state->fpe_ieee_status = thread->pcb->ieee_status;
		bcopy(&i860ss->fp1, &state->fp1, 4 * sizeof(double));

		break;
	}

	default:
		return (KERN_INVALID_ARGUMENT);
	}

	return (KERN_SUCCESS);
}


#if	SWITCH_HISTORY
static char *stateof(s)
int	s;
{
	static char	buf[16];
	char	*p = buf;

	*p++ = (s & TH_WAIT)    ? 'w' : '.';
	*p++ = (s & TH_SUSP)    ? 's' : '.';
	*p++ = (s & TH_RUN)     ? 'r' : '.';
	*p++ = (s & TH_UNINT)   ? 'u' : '.';
	*p++ = (s & TH_HALTED)  ? 'h' : '.';
	*p++ = (s & TH_IDLE)    ? 'i' : '.';
	*p++ = (s & TH_SWAPPED) ? 'O' : '.';
	*p++ = (s & TH_SW_COMING_IN) ? 'I' : '.';

	return buf;
}


static char *logmethod(n)
int	n;
{
	switch (n) {
	case LOG_SWITCH:
		return "swit";

	case LOG_HANDOFF:
		return "hand";

	case LOG_ATTACH:
		return "atch";

	case LOG_CONTINUE:
		return "cont";

	case LOG_DETACH:
		return "dtch";

	case LOG_SYSCALLRET:
		return "usys";

	case LOG_EXCEPTRET:
		return "uexc";

	case LOG_BOOTRET:
		return "boot";
	}
	return "????";
}


void db_show_switch_hist()
{
	int	i, j;
	struct switch_history *sh;

	db_printf("  nth sw  nth trap   nth ast  what  cur thread   cur stack\n");

	j = switch_history_next - 1;
	for (i = 0; i < SWITCH_HISTLEN; i++) {

		if (j < 0)
			j = SWITCH_HISTLEN - 1;

		sh = &switch_history[j];

		db_printf("%8u  %8u  %8u  %4s  0x%08x  0x%08x\n",
			sh->sh_nth,
			sh->sh_trapn,
			sh->sh_astn,
			logmethod(sh->sh_flag),
			sh->sh_curth,
			sh->sh_cursp);

		switch (sh->sh_flag) {
		case LOG_HANDOFF:
		case LOG_ATTACH:
		case LOG_DETACH:
		case LOG_CONTINUE:
		case LOG_SWITCH:
			db_printf("\told=0x%x [kstak=0x%08x state=%s]\n",
				sh->sh_old,
				sh->sh_old_kstack,
				stateof(sh->sh_old_state));
			if ((sh->sh_flag != LOG_DETACH) &&
			    (sh->sh_flag != LOG_HANDOFF)) {
				db_printf("\tcont[0x%08x]=", sh->sh_cont);
				if (sh->sh_cont) {
					db_printsym(sh->sh_cont, DB_STGY_ANY);
				}
			}
			db_printf("\n");
			if (sh->sh_new) {
				db_printf("\t");
				db_printf("new=0x%x [kstak=0x%08x state=%s]\n",
					sh->sh_new,
					sh->sh_new_kstack,
					stateof(sh->sh_new_state));
				if ((sh->sh_flag == LOG_SWITCH) ||
				    (sh->sh_flag == LOG_ATTACH)) {
					db_printf("\t    [sp=0x%x fp=0x%x r1[0x%x]=",
						sh->sh_sp,
						sh->sh_fp,
						sh->sh_r1);
					db_printsym(sh->sh_r1, DB_STGY_ANY);
					db_printf("]\n");
				}
			}
			break;

		case LOG_SYSCALLRET:
		case LOG_EXCEPTRET:
		case LOG_BOOTRET:
			break;
		}
		j--;
		db_printf("\n");
	}
}
#endif	SWITCH_HISTORY
