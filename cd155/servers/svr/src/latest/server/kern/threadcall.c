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
 * HISTORY
 * $Log: threadcall.c,v $
 * Revision 1.10  1995/02/10  23:55:17  stans
 *  'lint' picking: call_thread() changed from type void to any_t. return value
 *  added to coninside with 'any_t'.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing:WW05 sats
 *
 * Revision 1.9  1994/11/18  20:32:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/04/28  19:06:14  chrisp
 * Suppress multiple warnings if fixed priority scheduling request fails
 * on i386 hosts.
 *
 *  Reviewer: cfj
 *  Risk: L
 *  Benefit or PTS #: 9188
 *  Testing: On i386 platform and code inspection.
 *  Module(s): threadcall.c
 *
 * Revision 1.7  1994/03/29  23:51:29  cfj
 * Merge revision 1.6.4.1 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.6.4.1  1994/03/29  23:45:27  cfj
 * Make any created server threads scheduling policy POLICY_FIXEDPRI.
 *
 *  Reviewer:jlitvin
 *  Risk:Low
 *  Benefit or PTS #:9723
 *  Testing:pccm2
 *  Module(s):server/uxkern/ux_server_loop.c
 * 	   server/kern/threadcall.c
 *
 * Revision 1.6  1993/07/14  18:02:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:21:53  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:17:24  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:06:16  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:00:16  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:57:01  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:23:29  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:22  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.4  1992/11/11  17:27:07  condict
 * 	Since only ux_server_loop threads may be unwired, panic in
 * 	thread_callq_t if wired_threads not turned on.
 * 	[92/11/11            condict]
 *
 * Revision 2.3  92/03/09  14:38:04  durriya
 * 	[Revision 3.2  91/12/18  17:16:08  sp]
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.2  91/12/16  13:10:12  roy
 * 	Initial check-in.
 * 
 * Revision 3.1  91/10/29  16:11:38  barbou
 * Restored unexpanded copyright and revision history.
 * 
 * Revision 1.3  90/10/07  13:57:18  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  10:00:02  gm]
 * 
 * Revision 1.2  90/08/24  12:04:42  devrcs
 * 	Initial version.
 * 	[90/08/19  20:10:04  jeffc]
 * 
 * $EndLog$
 */
/*
 * threadcall.c: Routines to manage callout threads.
 *
 * The callout threads are used to perform operations that need
 * to be initiated in response to I/O completions, but cannot be
 * performed directly from interrupt context. Also used to limit
 * stack growth caused by explicit or implicit recursion.
 */
#include <sys/types.h>
#include <kern/lock.h>
#include <sys/synch.h>
#include <kern/zalloc.h>
#ifdef	OSF1_SERVER
#include <sys/user.h>
#else	/* OSF1_SERVER */
#include <kern/thread.h>
#include <vm/vm_kern.h>
#include <mach/vm_param.h>
#endif	/* OSF1_SERVER */

#include <kern/threadcall.h>

/*
 * Function:
 * boolean_t thread_call(thread_callq_t *, void (*)(void *), void *)
 *
 * Arguments:
 *	tcq	pointer to thread_callq structure [initialized by
 *		thread_call_create()], indicating the class of thread
 *		this request should be routed to.
 *	func	pointer to function to be invoked.
 *	arg	argument to called function.
 *
 * Invoke a function in thread context.
 *
 * Return value:
 *	TRUE	request successfully queued.
 *	FALSE	insufficient memory to queue request.
 */
boolean_t
thread_call(tcq, func, arg)
register thread_callq_t *tcq;
register void (*func)();
register void *arg;
{
	register struct thread_call *tc;
	register int s = splhigh();
	/*
	 * Since the whole purpose is to make interrupt things
	 * turn into thread things, we must splhigh() around
	 * the zget/zfree calls.
	 */

#ifdef	OSF1_SERVER
	simple_lock(&tcq->tcq_lock);
#endif	/* OSF1_SERVER */
	if ((tc = (thread_call_t *)zget(tcq->tcq_zone)) != NULL) {
		tc->tc_next = NULL;
		tc->tc_func = func;
		tc->tc_arg = arg;
		
#ifdef	OSF1_SERVER
#else	/* OSF1_SERVER */
		simple_lock(&tcq->tcq_lock);
#endif	/* OSF1_SERVER */

		if (tcq->tcq_head == NULL)  {
			tcq->tcq_head = tc;
			/* thread_wakeup_one is not sufficient here */
			thread_wakeup((int)tcq);
		} else {
			tcq->tcq_tail->tc_next = tc;
		}
		tcq->tcq_tail = tc;

#ifdef	OSF1_SERVER
#else	/* OSF1_SERVER */
		simple_unlock(&tcq->tcq_lock);
#endif	/* OSF1_SERVER */
	}
#ifdef	OSF1_SERVER
	simple_unlock(&tcq->tcq_lock);
#endif	/* OSF1_SERVER */
	splx(s);
	return((tc!=NULL)?TRUE:FALSE);
}

/*
 * Function:
 * boolean_t thread_call_one(thread_callq_t *, void (*)(void *), void *)
 *
 * Ensures that at least one occurrence of the specified func/arg pair
 * appears on the given thread call queue. If the specified entry already
 * exists, the function simply returns success (TRUE). If the specified
 * entry does not exist, a new call is allocated and added to the list.
 */
boolean_t
thread_call_one(tcq, func, arg)
register thread_callq_t *tcq;
register void (*func)();
register void *arg;
{
	register struct thread_call *tc;
	register int s = splhigh();

	simple_lock(&tcq->tcq_lock);
	/*
	 * search for matching request on the existing thread_call list
	 */
	for (tc = tcq->tcq_head; tc; tc = tc->tc_next) {
		if ((tc->tc_func == func) && (tc->tc_arg == arg)) {
			simple_unlock(&tcq->tcq_lock);
			splx(s);
			return(TRUE);
		}
	}
	/*
	 * No such request found, allocate and add to the list.
	 */
	if ((tc = (struct thread_call *)zget(tcq->tcq_zone)) != NULL) {
		tc->tc_next = NULL;
		tc->tc_func = func;
		tc->tc_arg = arg;
		
		if (tcq->tcq_head == NULL)  {
			tcq->tcq_head = tc;
			/* thread_wakeup_one is not sufficient here */
			thread_wakeup((int)tcq);
		} else {
			tcq->tcq_tail->tc_next = tc;
		}
		tcq->tcq_tail = tc;
	}
	simple_unlock(&tcq->tcq_lock);
	splx(s);
	return((tc!=NULL)?TRUE:FALSE);
}

/*
 * Function:
 * void thread_call_create(thread_callq_t *, int)
 *
 * Create a new class of callout threads.
 * It is the caller's responsibility to populate the thread_callq zone
 * with sufficient thread_call structures by calling thread_call_alloc().
 */
void
thread_call_create(tcq, ncallthreads)
thread_callq_t *tcq;
int ncallthreads;
{
	zone_t zone;

	zone = zinit(sizeof(thread_call_t), 0, 0, "thread_call zone");
	zchange(zone, FALSE, FALSE, FALSE, FALSE);

	simple_lock_init(&tcq->tcq_lock);
	tcq->tcq_head = NULL;
	tcq->tcq_tail = NULL;
	tcq->tcq_zone = zone;
	tcq->tcq_zone_size = 0;
	tcq->tcq_threadcall_size = 0;
	
	thread_call_add(tcq, ncallthreads);
}

/*
 * Function:
 * void thread_call_add(thread_callq_t *, int)
 *
 * Add new threads to a thread_call class. This is useful on a multiprocessor
 * to allow multiple parallel execution of a class of thread_calls. Note
 * that this depends on the degree of lock contention (if any) for the
 * work that these threads will be carrying out.
 */
void
thread_call_add(tcq, ncallthreads)
thread_callq_t *tcq;
int ncallthreads;
{
#ifdef	OSF1_SERVER
	extern boolean_t wired_threads;
#endif	/* OSF1_SERVER */
	extern task_t first_task;
	thread_t thread;
	extern any_t call_thread();

	while (ncallthreads--) {
#ifdef	OSF1_SERVER
		if (wired_threads)
			cthread_detach( cthread_fork(call_thread, (any_t)tcq) );
		else
			panic("thread_call_add not allowed with wired threads");
#else	/* OSF1_SERVER */
		thread = kernel_thread_w_arg(first_task, call_thread, tcq);
#endif	/* OSF1_SERVER */
	}
}

/*
 * Function:
 * void thread_call_alloc(thread_callq_t *, int)
 *
 * Allocate space for a certain number of thread_calls. Users of
 * thread_calls are responsible for allocating an appropriate number
 * of thread_calls for the maximum number of thread_calls that will
 * be used simultaneously. It is an error to use thread_call without
 * allocating appropriate space, or being prepared for thread_call to
 * fail.
 *
 * nthreadcalls may be negative to deallocate space. It is an error
 * to deallocate more space than was allocated.
 */
void
thread_call_alloc(tcq, nthreadcalls)
thread_callq_t *tcq;
int nthreadcalls;
{
	zone_t zone;
	vm_offset_t addr;
	int size;
	int s;

	/*
	 * Compute the number of new 'thread_call' structs we actually
	 * have to allocate. Might be negative, if we already have more
	 * in the zone than we currently need.
	 */
	simple_lock(&tcq->tcq_lock);

	tcq->tcq_threadcall_size += nthreadcalls;

	ASSERT (tcq->tcq_threadcall_size >= 0);

	/* Compute new number of thread_call_t's to allocate */
	nthreadcalls = (tcq->tcq_threadcall_size - tcq->tcq_zone_size);

	if (nthreadcalls > 0) {
		size = round_page(sizeof(thread_call_t) * nthreadcalls);
		zone = tcq->tcq_zone;
#ifdef	OSF1_SERVER
		if (vm_allocate(mach_task_self(), &addr, size, TRUE) 
		    != KERN_SUCCESS) {
			panic("thread_call_alloc: vm_allocate() failed");
		}
#else	/* OSF1_SERVER */
		addr = kmem_alloc(kernel_map, size);
#endif	/* OSF1_SERVER */
		tcq->tcq_zone_size += size/sizeof(thread_call_t);

		simple_unlock(&tcq->tcq_lock);

		s = splhigh();
		zcram(zone, addr, size);
		splx(s);
	} else {
		simple_unlock(&tcq->tcq_lock);
	}
}

/*
 * Callout thread: general-purpose thread to execute functions on
 * behalf of other code; for example, if an interrupt needs to perform
 * operations that can only be performed in thread context, it can
 * send a call to this (these) threads to perform the work.
 */
any_t
#ifdef	OSF1_SERVER
call_thread(arg)
	any_t arg;
#else	/* OSF1_SERVER */
call_thread()
#endif	/* OSF1_SERVER */
{
#ifdef	OSF1_SERVER
	struct uthread	uthread;
	register int calc;
#else	/* OSF1_SERVER */
	register thread_t thread;
#endif	/* OSF1_SERVER */
	register thread_callq_t *tcq;

#ifdef	OSF1_SERVER
	extern int		ux_min_quantum;

	(void) thread_policy(mach_thread_self(), 
			     POLICY_FIXEDPRI, 
			     ux_min_quantum);

	calc = (int)&uthread - (int)&(ur_cthread_self());
	if (u_offset) {
		if (u_offset != calc) {
			panic("call_thread: different u_offsets");
		}
	} else {
		u_offset = calc;
	}

	bzero((char *)&uthread, sizeof(struct uthread));
	mutex_init(&uthread.uu_lock);
	condition_init(&uthread.uu_condition);

	/* Collect argument */
	tcq = (thread_callq_t *)arg;
#else	/* OSF1_SERVER */
	thread = current_thread();
	thread->priority = thread->sched_pri = 0;
	thread_swappable(thread, FALSE);

	/* Collect argument left by kernel_thread_w_arg() */
	tcq = (thread_callq_t *)thread->reply_port;
	thread->reply_port = PORT_NULL;
#endif	/* OSF1_SERVER */

	for(;;) {
		register struct thread_call *tc;
		void (*func)();
		void *arg;
		int s = splhigh();

		simple_lock(&tcq->tcq_lock);

		while ((tc = tcq->tcq_head) == NULL) {
			assert_wait((int)tcq, FALSE);
			simple_unlock(&tcq->tcq_lock);
			splx(s);
			thread_block();
			s = splhigh();
			simple_lock(&tcq->tcq_lock);
		}
		tcq->tcq_head = tc->tc_next; tc->tc_next = NULL;

		simple_unlock(&tcq->tcq_lock);

		func = tc->tc_func; tc->tc_func = NULL;
		arg = tc->tc_arg; tc->tc_arg = NULL;

		zfree(tcq->tcq_zone, tc);

		splx(s);

		(*func)(arg);
	}
	return 0;
}
