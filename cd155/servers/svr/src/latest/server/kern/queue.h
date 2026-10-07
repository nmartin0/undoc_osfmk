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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: queue.h,v $
 * Revision 1.9  1994/11/18  20:32:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/02/01  21:20:56  jlitvin
 * Whoops!  I accidently added the lint warning I removed to the log message!
 *
 * Revision 1.7  1994/02/01  00:14:00  jlitvin
 * Get rid of obnoxious lint warning, "\* encountered inside a comment."
 *
 * Revision 1.6  1993/07/14  18:01:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:21:03  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:16:59  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:50  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/03/19  14:58:16  cfj
 * Merged from T9.
 *
 * Revision 1.2.8.2  1993/03/19  01:23:50  cfj
 * Fix to new locking code from OSF.
 *
 * Revision 2.5  1993/03/22  21:12:02  yazz
 * OSF lock changes.  Ordering of #include directives changed.
 *
 * Revision 2.4  92/08/13  19:18:46  rabii
 * 	Redefined the macro queue_init as a workaround for the i860's
 * 	optimizer so it does not get confused. (rabii)
 * 
 * Revision 2.3  92/05/24  14:31:24  pjg
 * 	92/03/23  10:28:53  sp
 * 	added queue_insert_after() and queue_insert_before() to support sorted
 * 	lists
 * 	[92/05/19            srl]
 * 
 * Revision 2.2  91/08/31  13:38:06  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/07  17:00:18  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/12/06  14:02:43  devrcs
 * 	Cleanup copyright and history log comments.
 * 	[90/11/15  11:06:44  gm]
 * 
 * Revision 1.5  90/10/07  13:55:33  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:57:21  gm]
 * 
 * Revision 1.4  90/08/24  12:03:49  devrcs
 * 	Add fast GNU-C inline functions.
 * 	[90/08/14  21:24:33  brezak]
 * 
 * Revision 1.3  90/06/22  20:14:47  devrcs
 * 	Removed UNIX_LOCKS dependencies.
 * 	[90/06/18  17:08:20  gmf]
 * 
 * 	nags merge
 * 	[90/06/12  21:28:08  gmf]
 * 
 * Revision 1.2  90/01/03  11:53:56  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:29:36  gm]
 * 
 * Revision 1.1  89/10/16  19:36:09  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:22:05  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.5  89/05/22  21:52:37  boykin
 * 	Merged MMAX_MP with Release 2.5.
 * 	Encore contributors to date:
 * 		Alan Langerman		(alan@encore.com)
 * 
 * Revision 2.7  89/03/09  20:15:03  rpd
 * 	More cleanup.
 * 
 * Revision 2.6  89/02/25  18:07:48  gm0w
 * 	Kernel code cleanup.
 * 	Put all the macros under #ifdef KERNEL
 * 	[89/02/15            mrt]
 * 
 * Revision 2.5  89/02/07  01:03:50  mwyoung
 * 	Relocated from sys/queue.h
 * 
 * Revision 2.4  88/12/19  02:51:30  mwyoung
 * 	Eliminate round_queue.
 * 	[88/11/22  01:22:22  mwyoung]
 * 
 * 	Add queue_remove_last.
 * 	[88/11/18            mwyoung]
 * 
 * Revision 2.3  88/08/24  02:40:43  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:20:58  mwyoung]
 * 
 * Revision 0.0  88/01/17            dpj
 * 	Added queue_enter_first, queue_last and queue_prev for use by
 * 	the TCP netport code.
 * 	[88/01/17            dpj]
 * 
 * Revision 0.0  87/03/17            dbg
 * 	Made queue package return queue_entry_t instead of vm_offset_t.
 * 	[87/03/17            dbg]
 * 
 * Revision 0.0  87/02/27            dbg
 * 	Made remqueue a real routine on all machines.  Defined old
 * 	Queue routines as macros that invoke new queue routines.
 * 	[87/02/27            dbg]
 * 
 * Revision 0.0  87/02/25            avie
 * 	Fixed non-KERNEL compilation.
 * 	[87/02/25            avie]
 * 
 * Revision 0.0  87/02/24            dlb
 * 	MULTIMAX: remqueue is now a real routine, removed define.
 * 	This is done to mask a compiler bug.
 * 	[87/02/24            dlb]
 * 
 * Revision 0.0  85/06/12            avie
 * 	Created.
 * 	[85/06/12            avie]
 * 
 * Revision 0.0  85/05/21            mja
 * 	Upgraded to 4.2BSD.  Changed to enQueue()/deQueue() names
 * 	to avoid conflicts with 4.2 dequeue().  [V1(1)]
 * 	[85/05/21            mja]
 * 
 * Revision 0.0  80/03/06            rfr
 * 	Created (V1.05).
 * 	[80/03/06            rfr]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 *	File:	queue.h
 *
 *	Type definitions for generic queues.
 */

#ifndef	_KERN_QUEUE_H_
#define _KERN_QUEUE_H_

#include <mach/machine/vm_types.h>
#include <kern/macro_help.h>

/*
 *	Queue of abstract objects.  Queue is maintained
 *	within that object.
 *
 *	Supports fast removal from within the queue.
 *
 *	How to declare a queue of elements of type "foo_t":
 *		In the "*foo_t" type, you must have a field of
 *		type "queue_chain_t" to hold together this queue.
 *		There may be more than one chain through a
 *		"foo_t", for use by different queues.
 *
 *		Declare the queue as a "queue_t" type.
 *
 *		Elements of the queue (of type "foo_t", that is)
 *		are referred to by reference, and cast to type
 *		"queue_entry_t" within this module.
 */

/*
 *	A generic doubly-linked list (queue).
 */

struct queue_entry {
	struct queue_entry	*next;		/* next element */
	struct queue_entry	*prev;		/* previous element */
};

typedef struct queue_entry	*queue_t;
typedef	struct queue_entry	queue_head_t;
typedef	struct queue_entry	queue_chain_t;
typedef	struct queue_entry	*queue_entry_t;

/*
 *	enqueue puts "elt" on the "queue".
 *	dequeue returns the first element in the "queue".
 *	remqueue removes the specified "elt" from the specified "queue".
 */

#define enqueue(queue,elt)	enqueue_tail(queue, elt)
#define dequeue(queue)		dequeue_head(queue)

extern void		enqueue_head();
extern void		enqueue_tail();
extern queue_entry_t	dequeue_head();
extern queue_entry_t	dequeue_tail();
extern void		remqueue();

/*
 *	Macro:		queue_init
 *	Function:
 *		Initialize the given queue.
 *	Header:
 *		void queue_init(q)
 *			queue_t		q;	\* MODIFIED *\
 */
#define queue_init(q)                           \
MACRO_BEGIN                                     \
        {(q)->next = (q); (q)->prev = (q);}     \
MACRO_END

/*
 *	Macro:		queue_first
 *	Function:
 *		Returns the first entry in the queue,
 *	Header:
 *		queue_entry_t queue_first(q)
 *			queue_t	q;		\* IN *\
 */
#define queue_first(q)	((q)->next)

/*
 *	Macro:		queue_next
 *	Header:
 *		queue_entry_t queue_next(qc)
 *			queue_t qc;
 */
#define queue_next(qc)	((qc)->next)

/*
 *	Macro:		queue_end
 *	Header:
 *		boolean_t queue_end(q, qe)
 *			queue_t q;
 *			queue_entry_t qe;
 */
#define queue_end(q, qe)	((q) == (qe))

#define queue_empty(q)		queue_end((q), queue_first(q))

/*
 *	Macro:		queue_enter
 *	Header:
 *		void queue_enter(q, elt, type, field)
 *			queue_t q;
 *			<type> elt;
 *			<type> is what's in our queue
 *			<field> is the chain field in (*<type>)
 */
#define queue_enter(head, elt, type, field)			\
MACRO_BEGIN							\
	if (queue_empty((head))) {				\
		(head)->next = (queue_entry_t) elt;		\
		(head)->prev = (queue_entry_t) elt;		\
		(elt)->field.next = head;			\
		(elt)->field.prev = head;			\
	}							\
	else {							\
		register queue_entry_t prev;			\
								\
		prev = (head)->prev;				\
		(elt)->field.prev = prev;			\
		(elt)->field.next = head;			\
		(head)->prev = (queue_entry_t)(elt);		\
		((type)prev)->field.next = (queue_entry_t)(elt);\
	}							\
MACRO_END

#define queue_insert_after(head, cur, elt, type, field)		\
MACRO_BEGIN							\
	register queue_entry_t next;				\
								\
	next = (cur)->field.next;				\
	if ((head) == next)					\
		(head)->prev = (queue_entry_t)(elt);		\
	else							\
		((type)next)->field.prev = (queue_entry_t)(elt);\
	(elt)->field.next = next;				\
	(elt)->field.prev = (queue_entry_t)(cur);		\
	(cur)->field.next = (queue_entry_t)(elt);		\
MACRO_END

#define queue_insert_before(head, cur, elt, type, field)	\
MACRO_BEGIN							\
	register queue_entry_t prev;				\
								\
	prev = (cur)->field.prev;				\
	if ((head) == prev)					\
		(head)->next = (queue_entry_t)(elt);		\
	else							\
		((type)prev)->field.next = (queue_entry_t)(elt);\
	(elt)->field.next = (queue_entry_t)(cur);		\
	(elt)->field.prev = prev;				\
	(cur)->field.prev = (queue_entry_t)(elt);		\
MACRO_END

/*
 *	Macro:		queue_field [internal use only]
 *	Function:
 *		Find the queue_chain_t (or queue_t) for the
 *		given element (thing) in the given queue (head)
 */
#define queue_field(head, thing, type, field)			\
		(((head) == (thing)) ? (head) : &((type)(thing))->field)

/*
 *	Macro:		queue_remove
 *	Header:
 *		void queue_remove(q, qe, type, field)
 *			arguments as in queue_enter
 */
#define queue_remove(head, elt, type, field)			\
MACRO_BEGIN							\
	register queue_entry_t	next, prev;			\
								\
	next = (elt)->field.next;				\
	prev = (elt)->field.prev;				\
								\
	queue_field((head), next, type, field)->prev = prev;	\
	queue_field((head), prev, type, field)->next = next;	\
MACRO_END

/*
 *	Macro:		queue_assign
 */
#define queue_assign(to, from, type, field)			\
MACRO_BEGIN							\
	((type)((from)->prev))->field.next = (to);		\
	((type)((from)->next))->field.prev = (to);		\
	*to = *from;						\
MACRO_END

#define queue_remove_first(h, e, t, f)				\
MACRO_BEGIN							\
	e = (t) queue_first((h));				\
	queue_remove((h), (e), t, f);				\
MACRO_END

#define queue_remove_last(h, e, t, f)				\
MACRO_BEGIN							\
	e = (t) queue_last((h));				\
	queue_remove((h), (e), t, f);				\
MACRO_END

/*
 *	Macro:		queue_enter_first
 *	Header:
 *		void queue_enter_first(q, elt, type, field)
 *			queue_t q;
 *			<type> elt;
 *			<type> is what's in our queue
 *			<field> is the chain field in (*<type>)
 */
#define queue_enter_first(head, elt, type, field)		\
MACRO_BEGIN							\
	if (queue_empty((head))) {				\
		(head)->next = (queue_entry_t) elt;		\
		(head)->prev = (queue_entry_t) elt;		\
		(elt)->field.next = head;			\
		(elt)->field.prev = head;			\
	}							\
	else {							\
		register queue_entry_t next;			\
								\
		next = (head)->next;				\
		(elt)->field.prev = head;			\
		(elt)->field.next = next;			\
		(head)->next = (queue_entry_t)(elt);		\
		((type)next)->field.prev = (queue_entry_t)(elt);\
	}							\
MACRO_END

/*
 *	Macro:		queue_last
 *	Function:
 *		Returns the last entry in the queue,
 *	Header:
 *		queue_entry_t queue_last(q)
 *			queue_t	q;		\* IN *\
 */
#define queue_last(q)	((q)->prev)

/*
 *	Macro:		queue_prev
 *	Header:
 *		queue_entry_t queue_prev(qc)
 *			queue_t qc;
 */
#define queue_prev(qc)	((qc)->prev)

/* Following include cannot be done until here, since lock.h also includes
 * queue.h and needs some of the declarations that are above this point:
 */
#include <kern/lock.h>

/*
 *	Define macros for queues with locks.
 */
struct mpqueue_head {
	struct queue_entry	head;		/* header for queue */
	decl_simple_lock_data(,lock)		/* lock for queue */
};

typedef struct mpqueue_head	mpqueue_head_t;

#define round_mpq(size)		(size)

#define mpqueue_init(q)						\
MACRO_BEGIN							\
	queue_init(&(q)->head);					\
	simple_lock_init(&(q)->lock);				\
MACRO_END

#define mpenqueue_tail(q, elt)					\
MACRO_BEGIN							\
	simple_lock(&(q)->lock);				\
	enqueue_tail(&(q)->head, elt);				\
	simple_unlock(&(q)->lock);				\
MACRO_END

#define mpdequeue_head(q, elt)					\
MACRO_BEGIN							\
	simple_lock(&(q)->lock);				\
	if (queue_empty(&(q)->head))				\
		*(elt) = 0;					\
	else							\
		*(elt) = dequeue_head(&(q)->head);		\
	simple_unlock(&(q)->lock);				\
MACRO_END

#define QWAIT	1
#define QNOWAIT	0
#define mpdequeue1(q, elt, wait)				\
MACRO_BEGIN							\
	for (;;) {						\
		register int s = splhigh();			\
		simple_lock(&(q)->lock);			\
		if (queue_empty(&(q)->head))			\
			if (wait) {				\
				assert_wait((int) (q), FALSE);	\
				simple_unlock(&(q)->lock);	\
				splx(s);			\
				thread_block();			\
				continue;			\
			} else					\
				*(elt) = 0;			\
		else						\
			*((caddr_t *)elt) = (caddr_t)dequeue_head(&(q)->head); \
		simple_unlock(&(q)->lock);			\
		splx(s);					\
		break;						\
	}							\
MACRO_END

#define mpenqueue1(q, elt)					\
MACRO_BEGIN							\
	register int empty = 0;					\
	register int s = splhigh();				\
	simple_lock (&(q)->lock);				\
	if (queue_empty(&(q)->head)) ++empty;			\
	enqueue_tail(&(q)->head, (elt));			\
	simple_unlock (&(q)->lock);				\
	splx(s);						\
	if (empty) thread_wakeup ((int) (q));			\
MACRO_END

/*
 *	Old queue stuff, will go away soon.
 */

/*
 * General purpose structure to define circular queues.
 *  Both the queue header and the queue elements have this
 *  structure.
 */

struct Queue
{
    struct Queue * F;
    struct Queue * B;
};

#define initQueue(q)	(queue_init((queue_t)(q)))
#define enQueue(q,elt)	(enqueue_tail((queue_t)(q),(queue_entry_t)(elt)))
#define deQueue(q)	((struct Queue *)dequeue_head((queue_t)(q)))
#define remQueue(q,elt)	(remqueue((queue_t)(q),(queue_entry_t)(elt)))
#define Queueempty(q)	(queue_empty((queue_t)(q)))


#if	__GNUC__ && !_NO_INLINE_QUEUE
/* Define fast inline functions for queues */
/*
 *	Insert element at head of queue.
 */
extern void __inline__ enqueue_head(que, elt)
	register queue_t	que;
	register queue_entry_t	elt;
{
	elt->next = que->next;
	elt->prev = que;
	elt->next->prev = elt;
	que->next = elt;
}

/*
 *	Insert element at tail of queue.
 */
extern void __inline__ enqueue_tail(que,elt)
	register queue_t	que;
	register queue_entry_t	elt;
{
	elt->next = que;
	elt->prev = que->prev;
	elt->prev->next = elt;
	que->prev = elt;
}

/*
 *	Remove and return element at head of queue.
 */
extern queue_entry_t __inline__ dequeue_head(que)
	register queue_t	que;
{
	register queue_entry_t	elt;

	if (que->next == que)
		return((queue_entry_t)0);

	elt = que->next;
	elt->next->prev = que;
	que->next = elt->next;
	return(elt);
}

/*
 *	Remove and return element at tail of queue.
 */
extern queue_entry_t __inline__ dequeue_tail(que)
	register queue_t	que;
{
	register queue_entry_t	elt;

	if (que->prev == que)
		return((queue_entry_t)0);

	elt = que->prev;
	elt->prev->next = que;
	que->prev = elt->prev;
	return(elt);
}

/*
 *	Remove arbitrary element from queue.
 *	Does not check whether element is on queue - the world
 *	will go haywire if it isn't.
 */

/*ARGSUSED*/
extern void __inline__ remqueue(que, elt)
	queue_t			que;
	register queue_entry_t	elt;
{
	elt->next->prev = elt->prev;
	elt->prev->next = elt->next;
}

#if	!defined(vax)
extern __inline__ insque(entry, pred)
	register struct queue_entry *entry, *pred;
{
	entry->next = pred->next;
	entry->prev = pred;
	(pred->next)->prev = entry;
	pred->next = entry;
}

extern __inline__ remque(elt)
	register struct queue_entry *elt;
{
	(elt->next)->prev = elt->prev;
	(elt->prev)->next = elt->next;
	return((int)elt);
}
#endif	/* vax */

#endif	/* __GNUC__ && !_NO_INLINE_QUEUE */

#endif	/* _KERN_QUEUE_H_ */
