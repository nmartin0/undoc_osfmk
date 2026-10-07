/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
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
 * OSF/1 Release 1.0
 */
/*
 * timer.c
 *
 *
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: timer.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:12:22 $";
#endif

/*
 * Timer package.
 */


#define TIMER_DEBUG	0
/*
#define DEBUGOFF
*/

#include <pthread.h>
#include <sys/errno.h>
#include <mach.h>
#include <mach/message.h>

#include "debug.h"
#include "lock_queue.h"
#include "lock_queue_macros.h"
#include "ls_defs.h"
#include "mem.h"
#include "timer.h"
#include "netmsg.h"
#include "trace.h"

#ifdef	vax
#undef	VAX_FAST_LOCK
#define	VAX_FAST_LOCK	1
#else	vax
#undef	VAX_FAST_LOCK
#define	VAX_FAST_LOCK	0
#endif	vax
#if	VAX_FAST_LOCK
#include "vax_fast_lock.h"
#endif	VAX_FAST_LOCK


#define TIMER_QUANTUM	param.timer_quantum


static struct lock_queue	timer_queue;		/* Structure used to queue timers. */
static  pthread_mutex_t		timer_lock;		/* Lock for timer global values. */
static port_t			timer_port;		/* Port used to wait for messages. */
static long			timer_time;		/* The time that timer_run uses. */

/*
 * Memory management definitions.
 */
PUBLIC mem_objrec_t	MEM_TIMER;



/*
 * GET_ABSOLUTE_TIME
 *	Adds now to the interval of timer t and assigns the result to the deadline of t.
 */
#if	0
#define GET_ABSOLUTE_TIME(now, t) {							\
	t->deadline.tv_sec = now.tv_sec + t->interval.tv_sec;				\
	if ((t->deadline.tv_usec = now.tv_usec + t->interval.tv_usec) >= 1000000) {	\
		t->deadline.tv_usec -= 1000000;						\
		t->deadline.tv_sec ++;							\
	}										\
}
#endif	0
#define GET_ABSOLUTE_TIME(t) (t)->deadline.tv_sec = timer_time + ((t)->interval.tv_sec << 1)



/*
 * timer_run
 *	main loop of timer package.
 *
 * Design:
 *	Loops doing the following:
 *		Tries to get the first item on the timer queue;
 *		If there is nothing there then sleeps for TIMER_QUANTUM using msg_receive
 *		else if the first item is expired then do it.
 *
 */
timer_run()
BEGIN("timer_run")
	register timer_act_t	first_timer;
	boolean_t		removed;
	register msg_header_t	*timer_msg_ptr;
	msg_header_t		timer_msg;

#if	LOCK_THREADS
	pthread_mutex_lock(&thread_lock);
#endif	LOCK_THREADS

	timer_msg_ptr = &timer_msg;

	while (1) {
		pthread_mutex_lock(&timer_queue.lock);
		first_timer = (timer_act_t)timer_queue.head;
		pthread_mutex_unlock(&timer_queue.lock);
		if ((first_timer) && (timer_time >= first_timer->deadline.tv_sec)) {
			lq_remove_macro(&timer_queue, first_timer, removed);
			if (removed) {
				DEBUG1(TIMER_DEBUG, 1, 1026, (long)first_timer);
				(first_timer->action)(first_timer);
			}
		}
		else {
			/*
			 * Sleep for TIMER_INTERVAL.
			 */
			timer_msg_ptr->msg_size = sizeof(msg_header_t);
			timer_msg_ptr->msg_local_port = timer_port;
			DEBUG2(TIMER_DEBUG, 2, 1027, TIMER_QUANTUM, timer_time)
			(void)msg_receive(timer_msg_ptr, RCV_TIMEOUT, TIMER_QUANTUM);
			pthread_mutex_lock(&timer_lock);
			timer_time++;
			pthread_mutex_unlock(&timer_lock);
		}
	}

END



/*
 * timer_wake_up
 *	Sends a wake-up message to the timer thread.
 *
 */
void timer_wake_up()
BEGIN("timer_wake_up")
	kern_return_t	kr;
	msg_header_t	wake_up_msg;

	wake_up_msg.msg_simple = TRUE;
	wake_up_msg.msg_type = MSG_TYPE_NORMAL;
	wake_up_msg.msg_size = sizeof(msg_header_t);
	wake_up_msg.msg_remote_port = timer_port;
	wake_up_msg.msg_local_port = PORT_NULL;
	wake_up_msg.msg_id = 1111;

	kr = msg_send(&wake_up_msg, SEND_TIMEOUT, 0);
	if (kr != KERN_SUCCESS) {
	    if (kr == SEND_TIMED_OUT) {
		ERROR((msg, "timer_wake_up.msg_send timed out."));
		LOG0(TIMER_DEBUG, 0, 1024);
	    }
	    else {
		ERROR((msg, "timer_wake_up.msg_send fails, kr = %d.", kr));
	    }
	}

	RET;
END



/*
 * insert_timer_test
 *	compares two timers - returns TRUE if t has a closer deadline than cur.
 */
#define insert_timer_test(cur,t,args) \
	(((timer_act_t)(t))->deadline.tv_sec < ((timer_act_t)(cur))->deadline.tv_sec)


/*
 * TIMER_START:
 *	If timer t is not already on the timer queue
 *	then the absolute deadline of t is computed and t is inserted in the timer queue.
 *
 * Note:
 *	Assumes that timer is NOT on the queue - timer_restart should be used if it is.
 */
void timer_start(t)
	register timer_act_t	t;
BEGIN("timer_start")

	pthread_mutex_lock(&timer_lock);
	GET_ABSOLUTE_TIME(t);
	pthread_mutex_unlock(&timer_lock);
	lq_insert_macro(&timer_queue, insert_timer_test, (pthread_queue_item_t)t, 0);
	DEBUG3(TIMER_DEBUG, 0, 1025, (long)t, t->deadline.tv_sec, t->interval.tv_sec);
	RET;

END



/*
 * TIMER_STOP:
 *	If timer t is present on the queue then it is removed from the queue.
 *	Returns whether there was a timer to be removed or not.
 */
boolean_t timer_stop(t)
	register timer_act_t	t;
BEGIN("timer_stop")
	boolean_t	ret;

	lq_remove_macro(&timer_queue, (pthread_queue_item_t)t, ret);
	if (ret) {
		DEBUG1(TIMER_DEBUG, 0, 1020, (long)t);
		RETURN(TRUE);
	} 
	else {
		DEBUG1(TIMER_DEBUG, 0, 1021, (long)t);
		RETURN(FALSE);
	}

END



/*
 * TIMER_RESTART:
 *	If timer t is present on the queue then it is removed.
 *	Queues timer t up as in timer_start.
 */
void timer_restart(t)
	register timer_act_t	t;
BEGIN("timer_restart")
	boolean_t	ret;

	lq_remove_macro(&timer_queue, (pthread_queue_item_t)t, ret);
	pthread_mutex_lock(&timer_lock);
	GET_ABSOLUTE_TIME(t);
	pthread_mutex_unlock(&timer_lock);
	lq_insert_macro(&timer_queue, insert_timer_test, (pthread_queue_item_t)t, 0);
	DEBUG1(TIMER_DEBUG, 0, 1028, (long)t);

	RET;

END




/*
 * TIMER_ALLOC:
 *	allocates space for a timer and initialises it.
 */
timer_act_t timer_alloc()
BEGIN("timer_alloc")
	register timer_act_t	new_timer;

	MEM_ALLOCOBJ(new_timer,timer_act_t,MEM_TIMER);
	DEBUG1(TIMER_DEBUG, 0, 1022, (long)new_timer);
	new_timer->link = (struct timer *)0;
	new_timer->interval.tv_sec = 0;
	new_timer->interval.tv_usec = 0;
	new_timer->action = (int (*)())0;
	new_timer->info = (char *)0;
	new_timer->deadline.tv_sec = 0;
	new_timer->deadline.tv_usec = 0;
	RETURN(new_timer);
END



/*
 * TIMER_INIT:
 *	Initilizes the timer package.
 *	Allocates the port used for wake-up messages.
 *	Creates a thread which waits for timers to expire.
 */
boolean_t timer_init()
BEGIN("timer_init")
	kern_return_t	kr;
	pthread_t	new_thread;

	/*
	 * Initialize the memory management facilities.
	 */
	mem_initobj(&MEM_TIMER,"Timer record",sizeof(struct timer),
								FALSE,140,50);


	timer_time = 0;
	lq_init(&timer_queue);
	pthread_mutex_init(&timer_lock, pthread_mutexattr_default);

	if ((kr = port_allocate(task_self(), &timer_port)) != KERN_SUCCESS) {
		ERROR((msg, "timer_init.port_allocate fails, kr = %s.\n", kr));
		RETURN(FALSE);
	}
	(void)port_disable(task_self(), timer_port);

	if( pthread_create(&new_thread, pthread_attr_default, (pthread_func_t)timer_run, (any_t)0) != 0)
		printf("timer_run: error\n");;
	pthread_setname_np(new_thread, "timer_run");
	pthread_detach(new_thread);

	RETURN(TRUE);
END



/*
 * timer_cthread_exit
 *	just calls cthread_exit.
 *
 */
timer_cthread_exit() {

    pthread_exit((any_t)0);
}


/*
 * timer_always_true
 *	used when calling lq_find_in_queue.
 *
 */
#define timer_always_true(item,args) TRUE


/*
 * TIMER_KILL:
 *	Terminates the background timer thread by tricking it into suicide.
 *	Note that the timer thread may not be terminated upon return --
 *	the purpose of this routine is to clean up when a program is
 *	ready to terminate so that the threads package will not dump core.
 */
void timer_kill()
BEGIN("timer_kill")
	static struct timer	suicide;
	cthread_queue_item_t		ret;

	do {
		pthread_yield();
		lq_find_macro(&timer_queue, timer_always_true, 0, ret);
	} while (ret);

	suicide.interval.tv_sec  =    0L;   /* An arbitrary time which is */
	suicide.interval.tv_usec = 1000L;   /* instantaneous to humans... */
	suicide.action = (int (*)())timer_cthread_exit;
	DEBUG0(TIMER_DEBUG, 0, 1023);
	timer_start(&suicide);
	RET;
END
