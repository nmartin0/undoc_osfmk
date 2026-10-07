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
 * Copyright 1992 by Intel Corporation,
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
 * Paragon multi-processor routines.
 *
 * utilized when NCPUS > 1, and/or when the MCP is enabled (MCMSG)
 *
 * $Id: mp.c,v 1.36 1995/03/21 21:01:45 lenb Exp $
 *
 */

#include <cpus.h>

#include <mach_kdb.h>
#include <mach_assert.h>

#include <mach/kern_return.h>
#include <mach/processor_info.h>

#include <mach/boolean.h>
#include <mach/machine.h>
#include <mach/vm_param.h>

#include <vm/vm_kern.h>
#include <vm/pmap.h>

#include <kern/lock.h>
#include <kern/cpu_number.h>
#include <kern/processor.h>
#include <kern/sched_prim.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/thread_swap.h>

#include <sys/reboot.h>

#include <i860paragon/mp.h>
#include <i860paragon/dp.h>

#include <i860/cpu_master.h>
#include <i860/psl.h>		/* PSR_IM */

#define	MP_DEBUG MACH_ASSERT

#define	MAX_NCPUS 3
#if	NCPUS > MAX_NCPUS
FORCE SYNTAX #error
#endif

#if	MP_DEBUG
int cross_xmit[MAX_NCPUS];
#endif

extern unsigned long dpset[NDPS];
extern unsigned long dpclr[NDPS];

/*
 * each CPU owns a word in the vector (index by cpu_number()).
 * bit positions represent the reason(s) for why the interprocessor interrupt
 * occured (doorbell was rung). 
 */
int     cpu_int_word[NCPUS];

mp_panic( int dst_cpu )
{
	CPU_INT_REASON_SET(MP_PANIC, dst_cpu);
	cpu_interrupt(dst_cpu);
}


#if	NCPUS > 1

decl_simple_lock_data(, freeze_cpu_lock);

/*
 * slave stack pointers, top of the kernel stack. Indexed by cpu_number() with
 * no stack allocated for the maste_cpu as it uses it's own startup stack
 * "_kint_stack_hi".
 */
int	slave_stacks[NCPUS];

/*
 * These constants define how the CPU behaves. The mode of the CPU is not
 * allowed to change for the life of the node.
 */
/* XXX duplicate from i860paragon/model_dep.c */
#define CPU_OFF			'o'
#define CPU_MCPMODE		'm'
#define CPU_APPMODE		'a'

#if	SLAVE_CLOCK

send_slave_clock(regs)
	struct i860_saved_state *regs;
{
	register int	cpu;

	for(cpu=0; cpu < NCPUS; cpu++ ) {
		if ( cpu == master_cpu )
			continue;
#if 0
		if ( machine_slot[cpu].running != TRUE)
			continue;
#endif
		CPU_INT_REASON_SET(MP_CLOCK,cpu);
		cpu_interrupt(cpu);
	}
}
#endif



mp_halt( int dst_cpu )
{
	CPU_INT_REASON_SET(MP_HALT,dst_cpu);
	cpu_interrupt(dst_cpu);
}

/*
 * invoke kdb on slave processors 
 */

remote_kdb() {
	int		my_cpu = cpu_number();
	register	i;

	for (i = 0; i < NCPUS; i++)
		if ( i != my_cpu && machine_slot[i].running == TRUE) {
			CPU_INT_REASON_SET(MP_KDB,i);
			cpu_interrupt(i);
		}
}

mp_doorbell( int dst_cpu )
{
	CPU_INT_REASON_SET(MP_NOP,dst_cpu);
	cpu_interrupt(dst_cpu);
}

/*
 * Send cross cpu interrupt for pmap update. Called from pmap.c
 */

interrupt_processor(int dest_cpu)
{
	CPU_INT_REASON_SET(MP_TLB_FLUSH,dest_cpu);
        cpu_interrupt(dest_cpu);
}

/*
 * XXX
 */

kern_return_t
cpu_control(slot_num, info, count)
        int                     slot_num;
        processor_info_t        info;
        long                    *count;
{
	return (KERN_FAILURE);
}


/*
 * panic all other CPU's
 */
panic_others()
{
        register int i, me = cpu_number();

        for (i = 0; i < NCPUS; i++) {
                if (i == me)
                        continue;
                if (machine_slot[i].is_cpu && machine_slot[i].running) {
			mp_panic(i);
                }
        }
}

/*
 * Called from master_cpu (0).  Start up the CPU_APPMODE processors.
 * The MCP will be enabled later according to boot_msg_proc.
 * XXX For simplicity, we'll proably want to start the MCP here too.
 */
void
start_other_cpus()
{
	int		i;
	extern void	cpu_start(int cpunum);
	extern char	boot_cpu_mode[];
	int		cpus_started = 0;
	extern		int getbootint();

	for (i = 1; i < NCPUS; i++)
	{
		if (machine_slot[i].is_cpu
			&& !machine_slot[i].running
			&& (boot_cpu_mode[i] == CPU_APPMODE))
		{
			cpu_start(i);
			cpus_started++;
	    	}
	}

#if	ASMP
	/*
	 * If we're in uni-processor mode, disable the baton --
	 * unless DISABLE_BATON=0
	 */
	if ((cpus_started == 0) && getbootint("DISABLE_BATON", 1))
	{
		int s = sploff();
		baton_disable();
		splon(s);
	}
#endif	ASMP
}

/*
 * Slave comes up here to initialize the hardware machine..
 * called from kern/startup.c:slave_main()
 *
 * implicit inputs:
 *	VM mapping turned OFF.
 *	interrupts are disabled.
 *	we have a valid startup stack.
 * outputs:
 *	none.
 *
 * side effects:
 *	mach_slot[my_cpu] initialized.
 *	interrupts enabled.
 */


slave_machine_init()
{
	int		my_cpu;
	thread_t	th;
	extern int	printf_cpu_number_prefix; /* see kern/printf.c */
#if	MP_DEBUG
	extern int	slave_noise;
#endif

	/*
	 * enable VM mapping.
	 */
	set_dirbase(kernel_pmap->dirbase);

	my_cpu = cpu_number();

#if	MP_DEBUG
	if (slave_noise) {
		/* eliminate cpu prefix for now */
		printf_cpu_number_prefix = FALSE;
		print_chip_type();	/* see i86paragon/model_dep.c */
	}
#endif

	/* indicate that this cpu is UP */
        machine_slot[my_cpu].running = TRUE;

	printf_cpu_number_prefix = TRUE;

	splhigh();
	splon( PSR_IM );
}

#define LOCAL_SSTACK 1	/* one advantage here is we can be < 1 vm_page_size
			 * using LOCAL_SSTACK 0 breaks on startup of 2nd CPU.
			 */
#define SS_SIZE 4096	/* startup stack size */

/*
 * Allocate slave initial (startup) stacks
 */
void
slave_stack_alloc()
{
	register int		i;
	vm_offset_t		stack_start=0;
#if LOCAL_SSTACK
	static char		sstack[SS_SIZE * (NCPUS-1)];
#endif
	/*
	 * Allocate a stack for each CPU except for
	 * the master CPU (which uses a bootstrap stack)
	 *
	 *      Kernel stacks should be naturally aligned,
	 *      so that it is easy to find the starting/ending
	 *      addresses of a stack given an address in the middle.
	 */
#if LOCAL_SSTACK
	stack_start = (vm_offset_t)sstack;
#else
	if ( kmem_alloc_wired(kernel_map,&stack_start,SS_SIZE*(NCPUS-1))
	     != KERN_SUCCESS)
		panic("slave_stack_alloc() kmem_alloc_wired() failed.");
#endif

#if MP_DEBUG
	bzero( stack_start, SS_SIZE * (NCPUS-1) );
#endif
	/*
	 * Set up pointers to the top of the slave stacks.
	 */
	stack_start += SS_SIZE;
	for (i = 0; i < NCPUS; i++) {
		if (i == master_cpu) 
			continue;
		slave_stacks[i] = stack_start;
		stack_start += SS_SIZE;
	}
}

#ifdef	FORCE_MASTER
/*
 * force the current thread to run on the master_cpu "CPU0" to protect the
 * MP unsafe kernel.
 */
typedef struct i860_saved_state	exception_frame_t;

force_master_cpu()
{
	register exception_frame_t	*regs;
	register thread_t		th=current_thread();


	if ( th != THREAD_NULL ) {
		regs = USER_REGS(th);
		/* must not be a KERNEL mode thread */
		if ( regs->sp == 0 && regs->dirbase == 0 ) {
			if ( current_processor()->idle_thread != th ) {
				assert(th->bound_processor == master_processor);
			}
			return;	/* kernel thread */
		}

		if ( th->bound_processor == PROCESSOR_NULL ) {
			thread_bind( th, master_processor );
			/* XXX this IS required. otherwise the system runs so
			 * slow it hardly makes it to single-user shell prompt.
			 */
			thread_block((void (*)()) 0);
		} else {
			assert(th->bound_processor == master_processor);
		}
		assert( cpu_number() == 0 );
	}
}

release_master_cpu()
{
	register exception_frame_t	*regs;
	register thread_t		th;

	if ( (th=current_thread()) != THREAD_NULL ) { 
		regs = USER_REGS(th);
		/*
		 * must not release KERNEL threads, sad way to determine if a
		 * thread is kernel or not.
		 */
		if ( regs->sp == 0 && regs->dirbase == 0 ) {
#if 0 /* debug */
			if ( th->bound_processor == PROCESSOR_NULL ) {
				printf("kthread %x NOT bound to master\n",th);
				thread_bind( th, master_processor );
				thread_block((void (*)()) 0);
			}
#endif
			return;	/* kernel thread */
		}

		/* USER mode thread */
		if ( th->bound_processor != PROCESSOR_NULL ) {
        		thread_bind( th, PROCESSOR_NULL );
			thread_block((void (*)()) 0);
		}
	}
}


/* DEBUG */

must_be_master(s, caller)
	char	*s;
	int	caller;
{
	register thread_t	self;

        if ( (self=current_thread()) && cpu_number() != 0 ) {
		register exception_frame_t	*regs;

		regs = USER_REGS(self);
		printf("NOT on MASTER CPU, called from %s\n", s);
		printf("  prev caller 0x%x\n",caller);
                printf("th 0x%x bnd 0x%x proc 0x%x psr 0x%x sp 0x%x db 0x%x\n",
                                self,self->bound_processor,master_processor,
				regs->psr,regs->sp,regs->dirbase);
                gimmeabreak();
        }
}
#endif	/* FORCE_MASTER */

#ifdef	FREEZE_NOT_YET_READY
unsigned int cpu_is_frozen[NCPUS];

/*
 * freeze_other_cpus()
 * does not return until they've acknowledged they're frozen.
 */
void
freeze_other_cpus()
{
	int		my_cpu = cpu_number();
	register	i;
	extern	int	get_psr();

	assert (get_psr() & PSR_IM);	/* interrupts enabled */

	SIMPLE_LOCK(&freeze_cpu_lock);

	for (i = 0; i < NCPUS; i++)
		if ( i != my_cpu && machine_slot[i].running == TRUE) {
			CPU_INT_REASON_SET(MP_FREEZE, i);
			cpu_interrupt(i);
		}
	SIMPLE_UNLOCK(&freeze_cpu_lock);

	return;
}

/*
 * return 1 if all the other valid cpus are frozen.
 */
int
other_cpus_frozen()
{
	int		my_cpu = cpu_number();
	register	i;

	for (i = 0; i < NCPUS; i++)
	{
		if ( i != my_cpu && machine_slot[i].running == TRUE)
		{
			if (!cpu_is_frozen[i])
				return (0);
		}
	}
	return (1);
}

void
thaw_other_cpus()
{
}
#endif	FREEZE_NOT_YET_READY

#endif	/* NCPUS > 1 */

#if	MCMSG || (NCPUS > 1)

/*
 * Generate an interrupt at specified processor.
 *
 *	cross processor interrupt bit reason(s) have already been set in the
 *	destination cpu reason word.  legal to direct at self or others.
 */

cpu_interrupt( int dest_cpu )
{
	register unsigned	s, adrs, bits;

	assert( dest_cpu < NCPUS );

	if ( machine_slot[dest_cpu].running != TRUE)
		return;

#if	MP_DEBUG
	cross_xmit[cpu_number()]++;
#endif
	/*
	 * write DP ASIC control register
	 */
	outl(dpset[dest_cpu], (1 << DP_CTRL_ATTEN_OTHER));
}

#endif	/* MCMSG || (NCPUS > 1) */
