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
 * $Id: ipc_wait.c,v 1.13 1995/03/14 08:02:31 lenb Exp $
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
 */
/*
 *	File:	norma/ipc_wait.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 */

#include <cpus.h>	/* NCPUS */
#include <mach_host.h>

#include <mach/port.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/counters.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_marequest.h>
#if	PARAGON860
#include <i860paragon/baton.h>
#endif	PARAGON860

extern void netipc_thread_lock();
extern void netipc_thread_unlock();

/*
 * XXX Needs locking to be multiprocessor safe.
 * XXX We probably might also want per-processor spinning,
 * XXX although this will complicate the sending code.
 *
 * We signal that we are waiting by setting handoff_mqueue nonzero.
 * Our sender specifies that something has changed by setting msg nonzero.
 * We then signal that we are releasing this module by setting msg zero.
 *
 * YYY We've added locking so some of the comments above are out of data.
 *
 * XXX Should try having this loop handle asts?
 */
ipc_mqueue_t	norma_ipc_handoff_mqueue;
ipc_kmsg_t	norma_ipc_handoff_msg;
mach_msg_size_t	norma_ipc_handoff_max_size;
mach_msg_size_t	norma_ipc_handoff_msg_size;

#if	iPSC386 || iPSC860 || PARAGON860
#if	NCPUS > 1
int	norma_ipc_kmsg_accept_disabled = 1;
#else	/* NCPUS > 1 */
int	norma_ipc_kmsg_accept_disabled = 0;
#endif	/* NCPUS > 1 */
#endif	iPSC386 || iPSC860 || PARAGON860

#if	PARAGON860
/*
 * Four out of five dentists surveyed claim that this routine does not make
 * anything of significance go faster.  As the benefit/complication
 * ratio is near zero -- permanently disabled it.
 */

/*ARGSUSED*/
ipc_kmsg_t
norma_ipc_kmsg_accept(mqueue, max_size, msg_size)
	register volatile ipc_mqueue_t mqueue;
	mach_msg_size_t max_size;
	mach_msg_size_t *msg_size;
{
	return (IKM_NULL);
}

#else	/* PARAGON860 */

int c_break_reset = 0;
int c_break_handoff = 0;
int c_break_thread = 0;
int c_break_gcount = 0;
int c_break_lcount = 0;
int c_break_ast = 0;
int c_break_ast_terminate = 0;
int c_break_ast_halt = 0;
int c_break_ast_block = 0;
int c_break_ast_network = 0;
int c_break_ast_netipc = 0;

/*
 * Spin until something else is runnable or until a kmsg shows up.
 *
 * Called holding netipc_thread_lock.  Will release it while waiting
 * and reacquire before returning.
 */
ipc_kmsg_t
norma_ipc_kmsg_accept(mqueue, max_size, msg_size)
	register volatile ipc_mqueue_t mqueue;
	mach_msg_size_t max_size;
	mach_msg_size_t *msg_size;
{
	register processor_t myprocessor;
	register volatile thread_t *threadp;
	register volatile int *gcount;
	register volatile int *lcount;
	int mycpu;

#if	iPSC386 || iPSC860 || PARAGON860
	if (norma_ipc_kmsg_accept_disabled) {
		return IKM_NULL;
	}
#endif	iPSC386 || iPSC860 || PARAGON860
#if	1
	if (c_break_reset) {
		c_break_reset = 0;
		c_break_handoff = 0;
		c_break_thread = 0;
		c_break_gcount = 0;
		c_break_lcount = 0;
		c_break_ast = 0;
		c_break_ast_halt = 0;
		c_break_ast_terminate = 0;
		c_break_ast_block = 0;
		c_break_ast_network = 0;
		c_break_ast_netipc = 0;
	}
#endif

	mycpu = cpu_number();
	myprocessor = current_processor();
	threadp = (volatile thread_t *) &myprocessor->next_thread;
	lcount = (volatile int *) &myprocessor->runq.count;


#if	PARAGON860
	paragon_mark_cpu_idle(cpu_number());
#endif	PARAGON860

#if	MACH_HOST
	gcount = (volatile int *) &myprocessor->processor_set->runq.count;
#else	MACH_HOST
	gcount = (volatile int *) &default_pset.runq.count;
#endif	MACH_HOST

	/*
	 * Indicate that we are spinning on this queue.
	 *
	 * Nonzero norma_ipc_handoff_mqueue keeps other receivers away.
	 * Nonzero norma_ipc_handoff_msg keeps other senders away.
	 */
	if (norma_ipc_handoff_mqueue != IMQ_NULL) {
		return IKM_NULL;
	}
	
	/*
	 * Don't mark cpu idle, we still like our pmap.  Use VIDLE instead,
	 * which is same as RUNNING, except for CPU statistics.
	 *
	 * XXX Will myprocessor->next_thread ever get set?
	 */
	assert(myprocessor->state == PROCESSOR_RUNNING);
	myprocessor->state = PROCESSOR_VIDLE;

	/*
	 * While we were on our way in, a kmsg could have been queued for
	 * this port.  Bail out in this case.
	 * XXX AST_NETIPC check is overly general.
	 */
	if (need_ast[mycpu] & AST_NETIPC) {
	        c_break_ast_netipc++;
	}

	assert(norma_ipc_handoff_msg == 0);
	norma_ipc_handoff_max_size = max_size;
	norma_ipc_handoff_msg_size = 0;
	norma_ipc_handoff_mqueue = mqueue;
	assert(ipc_kmsg_queue_first(&mqueue->imq_messages) == IKM_NULL);

	netipc_thread_unlock();
#if	ASMP
	baton_exit();
	ASSERT_BATON_NOT_OWNER();
#endif	/* ASMP */
	/*
	 * Spin until reschedule or kmsg handoff. Do asts in the meantime.
	 */

	for (;;) {

		if (norma_ipc_handoff_msg != IKM_NULL) {
			c_break_handoff++;
			break;
		}
		if (need_ast[mycpu]) {
			if (need_ast[mycpu] & AST_HALT) {
				c_break_ast_halt++;
			}
			if (need_ast[mycpu] & AST_TERMINATE) {
				c_break_ast_terminate++;
			}
			if (need_ast[mycpu] & AST_BLOCK) {
				c_break_ast_block++;
			}
			if (need_ast[mycpu] & AST_NETWORK) {
				c_break_ast_network++;
			}
			if (need_ast[mycpu] & AST_NETIPC) {
				c_break_ast_netipc++;
			}
			c_break_ast++;
			break;
		}
		if (*threadp != (volatile thread_t) THREAD_NULL) {
			c_break_thread++;
			break;
		}
		if (*gcount != 0) {
			c_break_gcount++;
			break;
		}
		if (*lcount != 0) {
			c_break_lcount++;
			break;
		}
	}

	/* We're not idle anymore: */
	myprocessor->state = PROCESSOR_RUNNING;

#if	PARAGON860
	paragon_mark_cpu_active(cpu_number());
#endif	PARAGON860

#if	ASMP
	ASSERT_BATON_NOT_OWNER();
	baton_enter();
#endif	/* ASMP */
	/*
	 * Before we release mqueue, we must check for a delivered message.
	 */
	netipc_thread_lock();
	if (norma_ipc_handoff_msg != IKM_NULL) {
		/*
		 * Someone left us a message,
		 * or an indication of a message that was too large.
		 */
		if (norma_ipc_handoff_msg_size) {
			*msg_size = norma_ipc_handoff_msg_size;
			norma_ipc_handoff_mqueue = IMQ_NULL;
			norma_ipc_handoff_msg = IKM_NULL;
			netipc_thread_unlock();
			return IKM_NULL;
		} else {
			register ipc_kmsg_t kmsg;
			kmsg = norma_ipc_handoff_msg;
			norma_ipc_handoff_mqueue = IMQ_NULL;
			norma_ipc_handoff_msg = IKM_NULL;
			netipc_thread_unlock();
			return kmsg;
		}
	}
	norma_ipc_handoff_mqueue = IMQ_NULL;
	norma_ipc_handoff_msg = IKM_NULL;
	assert(ipc_kmsg_queue_first(&mqueue->imq_messages) == IKM_NULL);
	return IKM_NULL;
}
#endif	/* !PARAGON860 */
