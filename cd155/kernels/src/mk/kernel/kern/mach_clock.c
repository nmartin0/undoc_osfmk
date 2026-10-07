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
 * SSD HISTORY
 * $Log: mach_clock.c,v $
 * Revision 1.12  1994/11/18  20:52:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/08/31  21:24:43  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.10.2.1  1994/08/11  22:23:29  regnier
 * PTS #: 10585
 * Mandatory: No
 * Description: Add MCP idle instrumentation for SPV (debug kernel only).
 * Reviewer(s): wds
 * Risk: Low
 * Module(s): kern/mach_clock.c  i860paragon/msgp/msgp_mp.c
 * Testing: Parallel SATs, developer tests.
 *
 * Revision 1.10  1994/07/12  19:23:24  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9  1994/05/06  21:27:50  lenb
 *         db_printf cleanup
 *
 * Revision 1.8  1994/03/19  01:23:46  lenb
 * Benefit: MACH_DEBUG changes
 * 	db_show_timeout(), db_show_all_timeouts(), db_show_all_softclocks()
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 1.7  1994/03/06  22:26:03  sean
 *  Reviewer:  steved
 *  Risk:  low
 *
 * Checking in these files which represent Steved's fixes for problems
 * in the following following areas:
 *
 * -fscan
 * -vdp clock skew
 * -bootmagic for processor mode
 *
 * ./ddb/db_command.c
 * ./ddb/db_print.c
 * ./i860/hardclock.c
 * ./i860paragon/mp.h
 * ./i860paragon/fscan.c
 * ./i860paragon/fscan.h
 * ./i860paragon/model_dep.c
 * ./i860paragon/mp_start.s
 * ./i860paragon/rpm.c
 * ./i860paragon/mp.c
 * ./i860paragon/interrupt.c
 * ./intel/pmap.c
 * ./ipsc/bootenv.c
 * ./kern/sched_prim.c
 * ./kern/mach_clock.c
 * ./vm/vm_pageout.c
 *
 * Revision 1.6.8.1  1994/05/18  22:30:27  stans
 * NORMA_IPC --> NORMA_VM
 *
 * Revision 1.6  1993/08/02  17:53:15  stans
 *    Throttle back softclock queue warnings; boost warning threshold.
 *
 * Revision 1.5  1993/06/30  22:45:06  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/25  22:21:25  andyp
 * Fixes from OSF's kernel tree for incorrect idle times.  Recovered OSF's
 * log entries.
 *
 * Revision 1.3  1993/04/27  20:36:02  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:36:19  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: mach_clock.c,v
 * Revision 1.2.2.3  1993/06/08  15:40:31  condict
 * 	Add VIDLE processor state, for user threads that go into an idle loop.
 * 	Fixes the computation of load factor, cpu utilization and profiling.
 * 	[1993/06/08  14:38:06  condict]
 *
 * Revision 1.2.2.2  1993/04/08  22:09:09  dwm
 * 	Pick up changes from Intel latest drop.
 * 	in-line expansion of get_softclock_processor macro.
 * 	[1993/04/08  18:34:21  dwm]
 * 
 * Revision 1.2  1992/11/25  01:11:13  robert
 * 	integrate changes below for norma_14
 * 
 * 	Philippe Bernadat (bernadat) at gr.osf.org
 * 	Propagate softclock handling on other cpus for MPs.
 * 	(See comment for softclock())
 * 	[1992/11/13  19:34:06  robert]
 * 
 * Revision 1.1  1992/09/30  02:09:31  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.16.4.5  92/09/15  17:21:33  jeffreyh
 * 	Added kernel task & threads profiling.
 * 	[92/07/17            bernadat]
 * 
 * Revision 2.16.4.4  92/04/30  11:59:08  bernadat
 * 	Consider user pc when profiling a user thread in kernel mode
 * 	[92/04/14            bernadat]
 * 
 * Revision 2.16.4.3  92/02/21  14:23:10  jsb
 * 	Removed include of norma_ipc.h (again).
 * 
 * Revision 2.16.4.2  92/02/18  19:09:02  jeffreyh
 * 	Added call to profile in clock_interrupt()
 * 	(Bernard Tabib & Andrei Danes @ gr.osf.org)
 * 	[92/01/02            bernadat]
 * 
 * Revision 2.16.4.1  92/01/21  21:50:49  jsb
 * 	Removed NORMA_IPC code.
 * 	[92/01/17  11:38:55  jsb]
 * 
 * Revision 2.16  91/08/03  18:18:56  jsb
 * 	NORMA_IPC: added call to netipc_timeout in hardclock.
 * 	[91/07/24  22:30:22  jsb]
 * 
 * Revision 2.15  91/07/31  17:45:57  dbg
 * 	Fixed timeout race.  Implemented host_adjust_time.
 * 	[91/07/30  17:03:54  dbg]
 * 
 * Revision 2.14  91/05/18  14:32:29  rpd
 * 	Fixed timeout/untimeout to use a fixed-size array of timers
 * 	instead of a zone.
 * 	[91/03/31            rpd]
 * 	Fixed host_set_time to update the mapped time value.
 * 	Changed the mapped time value to include a check field.
 * 	[91/03/19            rpd]
 * 
 * Revision 2.13  91/05/14  16:44:06  mrt
 * 	Correcting copyright
 * 
 * Revision 2.12  91/03/16  14:50:45  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 	Use counter macros to track thread and stack usage.
 * 	[91/03/01  17:43:15  rpd]
 * 
 * Revision 2.11  91/02/05  17:27:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:47  mrt]
 * 
 * Revision 2.10  91/01/08  15:16:22  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.9  90/11/05  14:31:27  rpd
 * 	Unified untimeout and untimeout_try.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.8  90/10/12  18:07:29  rpd
 * 	Fixed calls to thread_bind in host_set_time.
 * 	Fix from Philippe Bernadat.
 * 	[90/10/10            rpd]
 * 
 * Revision 2.7  90/09/09  14:32:18  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.6  90/08/27  22:02:48  dbg
 * 	Add untimeout_try for multiprocessors.  Reduce lint.
 * 	[90/07/17            dbg]
 * 
 * Revision 2.5  90/06/02  14:55:04  rpd
 * 	Converted to new IPC and new host port technology.
 * 	[90/03/26  22:10:04  rpd]
 * 
 * Revision 2.4  90/01/11  11:43:31  dbg
 * 	Switch to master CPU in host_set_time.
 * 	[90/01/03            dbg]
 * 
 * Revision 2.3  89/08/09  14:33:09  rwd
 * 	Include mach/vm_param.h and use PAGE_SIZE instead of NBPG.
 * 	[89/08/08            rwd]
 * 	Removed timemmap to machine/model_dep.c
 * 	[89/08/08            rwd]
 * 
 * Revision 2.2  89/08/05  16:07:11  rwd
 * 	Added mappable time code.
 * 	[89/08/02            rwd]
 * 
 * 14-Jan-89  David Golub (dbg) at Carnegie-Mellon University
 *	Split into two new files: mach_clock (for timing) and priority
 *	(for priority calculation).
 *
 *  8-Dec-88  David Golub (dbg) at Carnegie-Mellon University
 *	Use sentinel for root of timer queue, to speed up search loops.
 *
 * 30-Jun-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */ 
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 */
/*
 *	File:	clock_prim.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Clock primitives.
 */

#include <cpus.h>
#include <stat_time.h>
#include <mach_prof.h>
#include <norma_ipc.h>
#include <norma_vm.h>


#include <mach/boolean.h>
#include <mach/machine.h>
#include <mach/time_value.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <kern/counters.h>
#include <kern/cpu_number.h>
#include <kern/host.h>
#include <kern/lock.h>
#include <kern/mach_param.h>
#include <kern/processor.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/thread.h>
#include <kern/time_out.h>
#include <kern/time_stamp.h>
#include <vm/vm_kern.h>
#include <sys/time.h>
#include <machine/mach_param.h>	/* HZ */
#include <machine/machparam.h>

extern void	thread_quantum_update();

extern int clock_hz;
void softclock();		/* forward */

int		hz, tick, tickadj;
int		bigadj = 1000000;	/* adjust 10*tickadj if adjustment */
int		timedelta = 0;
int		tickdelta = 0;
time_value_t	time = { 0, 0 };	/* time since bootup (uncorrected) */
unsigned long	elapsed_ticks = 0;	/* ticks elapsed since bootup */

/*
 *	This update protocol, with a check value, allows
 *		do {
 *			secs = mtime->seconds;
 *			usecs = mtime->microseconds;
 *		} while (secs != mtime->check_seconds);
 *	to read the time correctly.  (On a multiprocessor this assumes
 *	that processors see each other's writes in the correct order.
 *	We may have to insert fence operations.)
 */

mapped_time_value_t *mtime = 0;

#define update_mapped_time(time)				\
MACRO_BEGIN							\
	if (mtime != 0) {					\
		mtime->check_seconds = (time)->seconds;		\
		mtime->microseconds = (time)->microseconds;	\
		mtime->seconds = (time)->seconds;		\
	}							\
MACRO_END

decl_simple_lock_data(,	timer_lock)	/* lock for ... */
timer_elt_data_t	timer_head;	/* ordered list of timeouts */
					/* (doubles as end-of-list) */

#if	NCPUS > 1
queue_head_t	softclock_head;	/* ordered list of cpus */
				/* handling soft clocks */

#define get_softclock_processor() \
	((processor_t)queue_first(&softclock_head) == master_processor && \
	 ((queue_remove(&softclock_head, master_processor, \
			processor_t, softclock_queue)), TRUE))

#else	NCPUS > 1

#define	get_softclock_processor() TRUE

#endif	NCPUS


/*
 *	Handle clock interrupts.
 *
 *	The clock interrupt is assumed to be called at a (more or less)
 *	constant rate.  The rate must be identical on all CPUS (XXX - fix).
 *
 *	Usec is the number of microseconds that have elapsed since the
 *	last clock tick.  It may be constant or computed, depending on
 *	the accuracy of the hardware clock.
 *
 */
void clock_interrupt(usec, usermode, basepri, pc)
	register int	usec;		/* microseconds per tick */
	boolean_t	usermode;	/* executing user code */
	boolean_t	basepri;	/* at base priority */
{
	register int		my_cpu = cpu_number();
	register thread_t	thread = current_thread();

	counter(c_clock_ticks++);
	counter(c_threads_total += c_threads_current);
	counter(c_stacks_total += c_stacks_current);

#if	STAT_TIME
	/*
	 *	Increment the thread time, if using
	 *	statistical timing.
	 */
	if (usermode) {
	    timer_bump(&thread->user_timer, usec);
	}
	else if (processor_ptr[my_cpu]->state == PROCESSOR_VIDLE) {
	    /* If we are a user thread doing an idle loop (VIDLE state),
	     * donate our time to the idle thread for this processor:
	     */
	    timer_bump(&processor_ptr[my_cpu]->idle_thread->system_timer, usec);
	} else {
	    timer_bump(&thread->system_timer, usec);
	}
#endif	STAT_TIME

	/*
	 *	Increment the CPU time statistics.
	 */
	{
	    register int	state;

	    if (usermode)
		state = CPU_STATE_USER;
	    else if (processor_ptr[my_cpu]->state != PROCESSOR_IDLE &&
		     processor_ptr[my_cpu]->state != PROCESSOR_VIDLE)
		state = CPU_STATE_SYSTEM;
	    else
		state = CPU_STATE_IDLE;

	    machine_slot[my_cpu].cpu_ticks[state]++;

	    /*
	     *	Adjust the thread's priority and check for
	     *	quantum expiration.
	     */

	    thread_quantum_update(my_cpu, thread, 1, state);
	}

	/*
	 *	Time-of-day and time-out list are updated only
	 *	on the master CPU.
	 */
	if (my_cpu == master_cpu) {

	    register int s;
	    register timer_elt_t	telt;
	    boolean_t	needsoft = FALSE;
	    processor_t softclock_processor;

#if	(MCMSG && (NCPUS > 1))
	/*
	 * Update MCP statistics.
	 */
	{
		extern int mcmsg_mp_enable;
	
		if (mcmsg_mp_enable && 
		    processor_ptr[1]->state == PROCESSOR_IDLE) {
			machine_slot[1].cpu_ticks[CPU_STATE_IDLE]++;
		}
	}
#endif	(MCMSG && (NCPUS > 1))

#if	TS_FORMAT == 1
	    /*
	     *	Increment the tick count for the timestamping routine.
	     */
	    ts_tick_count++;
#endif	TS_FORMAT == 1

	    /*
	     *	Update the tick count since bootup, and handle
	     *	timeouts.
	     */

	    s = splsched();
	    simple_lock(&timer_lock);

	    elapsed_ticks++;

	    telt = (timer_elt_t)queue_first(&timer_head.chain);

#if	(NCPUS > 1) && defined(PARAGON860)
	    /*
	     * PGI "C" compiler will not digest the get_softclock_processor()
	     * macro. Here we expand it by hand....sigh.
	     */
	    if (telt->ticks <= elapsed_ticks) {
		if ((processor_t)(queue_first(&softclock_head)) == master_processor) {
			queue_remove(&softclock_head, master_processor,
					processor_t, softclock_queue);
			needsoft = TRUE;
                }
	    }
#else
	    if (telt->ticks <= elapsed_ticks && get_softclock_processor()) {
	    	    needsoft = TRUE;
	    }
#endif
	    simple_unlock(&timer_lock);
	    splx(s);

	    /*
	     *	Increment the time-of-day clock.
	     */
	    if (timedelta == 0) {
		time_value_add_usec(&time, usec);
	    }
	    else {
		register int	delta;

		if (timedelta < 0) {
		    delta = usec - tickdelta;
		    timedelta += tickdelta;
		}
		else {
		    delta = usec + tickdelta;
		    timedelta -= tickdelta;
		}
		time_value_add_usec(&time, delta);
	    }
	    update_mapped_time(&time);

	    /*
	     *	Schedule soft-interupt for timeout if needed
	     */
	    if (needsoft) {
		if (basepri) {
		    (void) splsoftclock();
		    softclock();
		}
		else {
		    setsoftclock(master_processor->slot_num);
		}
	    }
	}
#if	MACH_PROF
	if (thread->thread_profiled) {
		if (!usermode && thread->task->map != kernel_map) {
			/* 
			 * User thread, but Kernel mode.  If not VIDLE,
			 * register user pc (mach_msg, vm_allocate ...)
			 */
			if (processor_ptr[my_cpu]->state != PROCESSOR_VIDLE)
				profile(user_pc(thread), thread->profil_buffer);
		} else {
			/*
			 * User thread and user mode or
			 * kernel thread and kernel mode
			 * register interrupted pc
			 */
			if (processor_ptr[my_cpu]->state != PROCESSOR_VIDLE)
				profile(pc, thread->profil_buffer);
		}
	}
	if (kernel_task->task_profiled) {
		if (!usermode && thread->task->map != kernel_map)
		  	/*
			 * User thread not profiled in kernel mode,
			 * kernel task profiled, register kernel pc
			 * for kernel task
			 */
			if (processor_ptr[my_cpu]->state != PROCESSOR_VIDLE)
				profile(pc, kernel_task->profil_buffer);
	}

#endif	MACH_PROF
}


int softclock_active_count;	/* number of cpus handling softclocks */

/*
 * Insert a processor into the queue of processors
 * candidate for softclock handling
 */

#if NCPUS > 1
softclock_queue(processor)
processor_t processor;
{
       	simple_lock(&timer_lock);
	queue_enter(&softclock_head, processor, processor_t, 
			  softclock_queue);
	simple_unlock(&timer_lock);
}
	
/*
 * Remove a processor from the queue of processors
 * candidate for softclock handling
 */

softclock_unqueue(processor)
processor_t processor;
{
	simple_lock(&timer_lock);
	queue_remove(&softclock_head, processor, processor_t, 
			  softclock_queue);
	simple_unlock(&timer_lock);
}

/*
 * Pop next processor from softclock queue.
 * Timer lock must be held
 */

processor_t
next_softclock_processor()
{
	processor_t	processor;

	processor = (processor_t) queue_next(&softclock_head);
	if (!queue_end(&softclock_head, (queue_t)processor)) {
		queue_remove(&softclock_head, processor, processor_t, 
			  softclock_queue);
		return(processor);
	}
	return(PROCESSOR_NULL);
}

/*
 * Register the fact that the current cpu is handling softclocks
 */

#define enter_softclock(ncalls) \
	if (!ncalls) \
		softclock_active_count++;


/*
 * Push back current processor from softclock queue.
 * Timer lock must be held
 */

release_softclock_processor()
{
  	processor_t processor = current_processor();

	if (processor == master_processor)
  		queue_enter_first(&softclock_head, processor,
				  processor_t, softclock_queue)
	else
	  	queue_enter(&softclock_head, processor,
			    processor_t, softclock_queue);
	softclock_active_count--;
}

/*
 * Propagate softclock on other cpus if needed.
 * Timer lock must be held
 */

/*
 * SOFT_CLOCK_LIMIT_SHIFT is used to decide if propagation of softclocks
 * should occur. The invariant is
 * (softclock_active_count << SOFT_CLOCK_LIMIT_SHIFT) < number_of_timeout_elts
 * || (softclock_active_count >= number of active cpus.
 * Do not propagate all cpus at once.
 */

#define SOFT_CLOCK_LIMIT_SHIFT 3 	/* 8 softclocks per CPU */

#define propagate_softclock(telt, ncalls) \
{ \
    processor_t next_processor; \
    if (!(ncalls & ((1 << SOFT_CLOCK_LIMIT_SHIFT)-1)) && \
	telt->count > (softclock_active_count << SOFT_CLOCK_LIMIT_SHIFT) \
        && (next_processor = next_softclock_processor()) != PROCESSOR_NULL) { \
	    setsoftclock(next_processor->slot_num); \
    } \
}

/*
 * XXX This code is not complete, if bound_processor
 * is not NULL and not master, there is no garanty that the
 * elt will be processed, need to activate this processor.
 * Master processor is always activated for softclocks
*/

#define check_processor(telt) \
	while (telt->bound_processor != PROCESSOR_NULL && \
	       telt->bound_processor != current_processor()) { \
		telt = (timer_elt_t)queue_next(&telt->chain); \
		if (telt->ticks > elapsed_ticks) \
			break; \
	}

#else	NCPUS > 1
#define enter_softclock(ncalls)
#define release_softclock_processor()
#define propagate_softclock(telt, ncalls)
#define check_processor(telt)
#endif	NCPUS > 1


/*
 *	There is a nasty race between softclock and reset_timeout.
 *	For example, scheduling code looks at timer_set and calls
 *	reset_timeout, thinking the timer is set.  However, softclock
 *	has already removed the timer but hasn't called thread_timeout
 *	yet.
 *
 *	Interim solution:  We initialize timers after pulling
 *	them out of the queue, so a race with reset_timeout won't
 *	hurt.  The timeout functions (eg, thread_timeout,
 *	thread_depress_timeout) check timer_set/depress_priority
 *	to see if the timer has been cancelled and if so do nothing.
 *
 *	This still isn't correct.  For example, softclock pulls a
 *	timer off the queue, then thread_go resets timer_set (but
 *	reset_timeout does nothing), then thread_set_timeout puts the
 *	timer back on the queue and sets timer_set, then
 *	thread_timeout finally runs and clears timer_set, then
 *	thread_set_timeout tries to put the timer on the queue again
 *	and corrupts it.
 *
 *	Why do we need to propagate softclock on other cpus ?
 *	Consider the following scenario:
 *		The server has taken a lock.
 *		Then traps into the kernel to send/receive a message.
 *		The kernel locks the ipc_space.
 *		The kernel (still at level 0) is interrupted by softclock.
 *		For some reason there are many queued timeout 
 *			(this happens with the cthread locks, timeouting
 *			 with 10 ms values).
 *		The other cpus will continue calling thread_depress because
 *		of the lock held by the server. Even if this lock was held
 *		by another thread, the message signaling it cant reach the
 *		server because of the ipc space lock.
 *		Softclock will run forever processing thread_depress_timeouts.
 *
 *		Propagating softclocks on the other cpus has 2 advantages:
 *			Distribute the softclock load on all cpus.
 *			Make sure that at some point no cpus will be able
 *			to queue new timeouts.
 *		(this happened very often on the sequent)
 */

#if	NORMA_VM
int	ncalls_warn = 300;
int	ncalls_warnings=0;
#else	/* NORMA_VM */
int ncalls_warn = 100;
#endif	/* NORMA_VM */

void softclock()
{
	/*
	 *	Handle timeouts.
	 */
	int	s;
	register timer_elt_t	telt;
	register int	(*fcn)();
	register char	*param;
	int		ncalls = 0;

	while (TRUE) {
	    s = splsched();
	    simple_lock(&timer_lock);
	    enter_softclock(ncalls);
	    telt = (timer_elt_t) queue_first(&timer_head.chain);
	    check_processor(telt);
	    if (telt->ticks > elapsed_ticks) {
		release_softclock_processor();
		simple_unlock(&timer_lock);
		splx(s);
		break;
	    }

	    fcn = telt->fcn;
	    param = telt->param;

	    remqueue(&timer_head.chain, (queue_entry_t)telt);
	    telt->set = TELT_UNSET;
	    propagate_softclock(telt, ncalls);
	    simple_unlock(&timer_lock);
	    splx(s);

	    ncalls++;
	    assert(fcn != 0);
	    (*fcn)(param);
	}
#if	NORMA_VM
	if ( (ncalls > ncalls_warn) && (--ncalls_warnings <= 0) ) {
		printf("WARNING softclock on cpu %d called %d functions\n",
		       cpu_number(), ncalls);
		ncalls_warnings = 15;	/* throttle */
	}
#else	/* NORMA_VM */
	if (ncalls > ncalls_warn) {
		printf("WARNING softclock on cpu %d called %d functions\n",
		       cpu_number(), ncalls);
	}
#endif	/* NORMA_VM */
}

/*
 *	Set timeout.
 *
 *	Parameters:
 *		telt	 timer element.  Function and param are already set.
 *		interval time-out interval, in hz.
 */
void set_timeout(telt, interval)
	register timer_elt_t	telt;	/* already loaded */
	register unsigned int	interval;
{
	int	s;
	register timer_elt_t	next;

	s = splsched();
	simple_lock(&timer_lock);

	interval += elapsed_ticks;

	for (next = (timer_elt_t)queue_first(&timer_head.chain);
	     ;
	     next = (timer_elt_t)queue_next((queue_entry_t)next)) {

	    if (next->ticks > interval)
		break;
#if	NCPUS > 1
	    else if (next->ticks == interval)
		next->count++;
#endif	NCPUS > 1
	}
	telt->ticks = interval;
#if	NCPUS > 1
	telt->count = 0;
#endif	NCPUS > 1
	/*
	 * Insert new timer element before 'next'
	 * (after 'next'->prev)
	 */
	insque((queue_entry_t) telt, ((queue_entry_t)next)->prev);
	telt->set = TELT_SET;
	simple_unlock(&timer_lock);
	splx(s);
}

boolean_t reset_timeout(telt)
	register timer_elt_t	telt;
{
	int	s;

	s = splsched();
	simple_lock(&timer_lock);
	if (telt->set) {
	    remqueue(&timer_head.chain, (queue_entry_t)telt);
	    telt->set = TELT_UNSET;
	    simple_unlock(&timer_lock);
	    splx(s);
	    return TRUE;
	}
	else {
	    simple_unlock(&timer_lock);
	    splx(s);
	    return FALSE;
	}
}

void init_timeout()
{
	register i;

	simple_lock_init(&timer_lock);
	queue_init(&timer_head.chain);
#if	NCPUS > 1
	queue_init(&softclock_head);
#endif	NCPUS > 1
	timer_head.ticks = ~0;	/* MAXUINT - sentinel */

	hz = clock_hz;
	tick = (1000000 / clock_hz);
	if (hz > 100) {
		tickadj = 1;
	} else {
		tickadj = 100 / clock_hz;
	}

	elapsed_ticks = 0;
}

/*
 * Read the time.
 */
kern_return_t
host_get_time(host, current_time)
	host_t		host;
	time_value_t	*current_time;	/* OUT */
{
	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

	do {
		current_time->seconds = mtime->seconds;
		current_time->microseconds = mtime->microseconds;
	} while (current_time->seconds != mtime->check_seconds);

	return (KERN_SUCCESS);
}

/*
 * Set the time.  Only available to privileged users.
 */
kern_return_t
host_set_time(host, new_time)
	host_t		host;
	time_value_t	new_time;
{
	int	s;

	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

#if	NCPUS > 1
	/*
	 * Switch to the master CPU to synchronize correctly.
	 */
	thread_bind(current_thread(), master_processor);
	if (current_processor() != master_processor)
	    thread_block((void (*)) 0);
#endif	NCPUS > 1

	s = splhigh();
	time = new_time;
	update_mapped_time(&time);
	resettodr();
	splx(s);

#if	NCPUS > 1
	/*
	 * Switch off the master CPU.
	 */
	thread_bind(current_thread(), PROCESSOR_NULL);
#endif	NCPUS > 1

	return (KERN_SUCCESS);
}

/*
 * Adjust the time gradually.
 */
kern_return_t
host_adjust_time(host, new_adjustment, old_adjustment)
	host_t		host;
	time_value_t	new_adjustment;
	time_value_t	*old_adjustment;	/* OUT */
{
	time_value_t	oadj;
	unsigned int	ndelta;
	int		s;

	if (host == HOST_NULL)
		return (KERN_INVALID_HOST);

	ndelta = new_adjustment.seconds * 1000000
		+ new_adjustment.microseconds;

#if	NCPUS > 1
	thread_bind(current_thread(), master_processor);
	if (current_processor() != master_processor)
	    thread_block((void (*)) 0);
#endif	NCPUS > 1

	s = splclock();

	oadj.seconds = timedelta / 1000000;
	oadj.microseconds = timedelta % 1000000;

	if (timedelta == 0) {
	    if (ndelta > bigadj)
		tickdelta = 10 * tickadj;
	    else
		tickdelta = tickadj;
	}
	if (ndelta % tickdelta)
	    ndelta = ndelta / tickdelta * tickdelta;

	timedelta = ndelta;

	splx(s);
#if	NCPUS > 1
	thread_bind(current_thread(), PROCESSOR_NULL);
#endif	NCPUS > 1

	*old_adjustment = oadj;

	return (KERN_SUCCESS);
}

mapable_time_init()
{
	if (kmem_alloc_wired(kernel_map, (vm_offset_t *) &mtime, PAGE_SIZE)
						!= KERN_SUCCESS)
		panic("mapable_time_init");
	bzero((char *)mtime, PAGE_SIZE);
	update_mapped_time(&time);
}

timeopen()
{
	return(0);
}
timeclose()
{
	return(0);
}

/*
 *	Compatibility for device drivers.
 *	New code should use set_timeout/reset_timeout and private timers.
 *	These code can't use a zone to allocate timers, because
 *	it can be called from interrupt handlers.
 *	For MPs, bind timeouts to master for compatibility
 */

#define NTIMERS		20

timer_elt_data_t timeout_timers[NTIMERS];

/*
 *	Set timeout.
 *
 *	fcn:		function to call
 *	param:		parameter to pass to function
 *	interval:	timeout interval, in hz.
 */
void timeout(fcn, param, interval)
	int	(*fcn)(/* char * param */);
	char *	param;
	int	interval;
{
	int	s;
	register timer_elt_t elt;

	s = splsched();
	simple_lock(&timer_lock);
	for (elt = &timeout_timers[0]; elt < &timeout_timers[NTIMERS]; elt++)
	    if (elt->set == TELT_UNSET)
		break;
	if (elt == &timeout_timers[NTIMERS])
	    panic("timeout");
	elt->fcn = fcn;
	elt->param = param;
	elt->set = TELT_ALLOC;
#if	NCPUS > 1
	elt->bound_processor = master_processor;
#endif	NCPUS > 1
	simple_unlock(&timer_lock);
	splx(s);

	set_timeout(elt, (unsigned int)interval);
}

/*
 * Returns a boolean indicating whether the timeout element was found
 * and removed.
 */
boolean_t untimeout(fcn, param)
	register int	(*fcn)();
	register char *	param;
{
	int	s;
	register timer_elt_t elt;

	s = splsched();
	simple_lock(&timer_lock);
	queue_iterate(&timer_head.chain, elt, timer_elt_t, chain) {

	    if ((fcn == elt->fcn) && (param == elt->param)) {
		/*
		 *	Found it.
		 */
		remqueue(&timer_head.chain, (queue_entry_t)elt);
		elt->set = TELT_UNSET;

		simple_unlock(&timer_lock);
		splx(s);
		return (TRUE);
	    }
	}
	simple_unlock(&timer_lock);
	splx(s);
	return (FALSE);
}

#include <mach_debug.h>
#if	MACH_DEBUG
#include <i860/db_machdep.h>	/* db_expr_t */
#include <ddb/db_sym.h>	/* DB_STGY_ANY */

void
db_show_timeout(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	register timer_elt_t elt;

	if (addr == (db_expr_t)0 || !have_addr) {
		db_error("No timeout\n");
		/*NOTREACHED*/
	}

	elt = ((timer_elt_t)addr);

	db_printf("ticks 0x%x", elt->ticks);
	db_printf(" set 0x%x", elt->set);
#if	NCPUS > 1
	db_printf(" cnt %d bnd 0x%x ",
		elt->count, elt->bound_processor);
#endif	NCPUS > 1
	db_printsym((db_addr_t)elt->fcn, DB_STGY_ANY);
	db_printf("(0x%x)\n", elt->param);
}

/*ARGSUSED*/
void
db_show_all_timeouts(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	register timer_elt_t elt;

	db_printf("ticks 0x%x elapsed\n", elapsed_ticks);
	queue_iterate(&timer_head.chain, elt, timer_elt_t, chain) {
		db_show_timeout((db_expr_t)elt, 1, count, modif);
	}
}
/*ARGSUSED*/
void
db_show_all_softclocks(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	processor_t	p;
	extern void	db_show_processor(db_expr_t, int, db_expr_t, char *);

#ifdef	PARAGON860
	int i;

	db_printf("spl_soft_clock_needed[]:");
	for (i = 0; i < NCPUS; ++i)
	{
		extern	int spl_soft_clock_needed[];	/* xxx mach-dependent */
		db_printf(" %d", spl_soft_clock_needed[i]);
	}
	db_printf("\n");
#endif	PARAGON860

#if	NCPUS > 1
	db_printf("softclock_active_count: %d\n", softclock_active_count);

	queue_iterate(&softclock_head, p, processor_t, softclock_queue) {
		db_show_processor((db_expr_t)p, 1, (db_expr_t)count, modif);
	}
#endif	/* NCPUS > 1 */
}

#endif	MACH_DEBUG

