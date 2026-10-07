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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: queue.c,v $
 * Revision 1.4  1994/11/18  20:31:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:56:51  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:59  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:55  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:35  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:35:39  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/06/25  17:07:13  condict
 * Moved sys/*.h files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.0  91/01/17  12:10:09  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/05/21  13:53:49  dbg
 * 	Taken from kern/queue.c. The vax does this inline.
 * 	[89/06/12            rwd]
 * 
 * Revision 2.1  89/08/04  15:13:40  rwd
 * Created.
 * 
 * Revision 2.3  89/02/25  18:07:42  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.2  89/01/31  01:21:11  rpd
 * 	Multimax inline handles all queue routines now.
 * 	[88/05/19            dlb]
 * 
 * 17-Mar-87  David Golub (dbg) at Carnegie-Mellon University
 *	Created from routines written by David L. Black.
 *
 */ 

/*
 *	Routines to implement queue package.
 */

#include <kern/queue.h>

/*
 *	Insert element at head of queue.
 */
void enqueue_head(que, elt)
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
void enqueue_tail(que,elt)
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
queue_entry_t dequeue_head(que)
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
queue_entry_t dequeue_tail(que)
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
void remqueue(que, elt)
	queue_t			que;
	register queue_entry_t	elt;
{
	elt->next->prev = elt->prev;
	elt->prev->next = elt->next;
}

insque(entry, pred)
	register struct queue_entry *entry, *pred;
{
	entry->next = pred->next;
	entry->prev = pred;
	(pred->next)->prev = entry;
	pred->next = entry;
}

remque(elt)
	register struct queue_entry *elt;
{
	(elt->next)->prev = elt->prev;
	(elt->prev)->next = elt->next;
	return((int)elt);
}
