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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: queue.c,v $
 * Revision 1.6  1995/03/28  00:45:16  hfcheng
 *  Reviewer: Suri Brahmaroutu
 *
 *  Risk: Low
 *
 *  Benefit or PTS #: PTS 7927 "MPBETA/TCAT: server exception at
 *                    remque()+18, in nfs code"
 *
 *                    Before the time of failure, the vnode hash list is empty.
 *                    As a result, the server calls vnode_new() to allocate a
 *                    new vnode for a nfsnode.  Since it is newly allocated,
 *                    the nfsnode forward and backward pointers are NULL.
 *                    Before this nfsnode is put into the nfs hash list,
 *                    the server finds that another thread has a nfsnode for the
 *                    same file already.  As a result, the vnode is put back to
 *                    the vnode list without being used and the nfsnode's forward
 *                    and backward pointers are still NULL.  Then the next nfs
 *                    operation gets this vnode from the vnode hash list and tries
 *                    to clean up the node.  Since the vnode is setup for the
 *                    use of nfsnode before, it assumes that the nfsnode is in
 *                    nfs hash list and tries to derefence the forward and
 *                    backward pointers which are NULL.  This can also happen if
 *                    the vnode is used for ufs.  (PTS 10646)
 *
 *                    The solution is to check if the forward and backward
 *                    pointers are NULL before dereferencing them.
 *
 *
 *  Testing: 1. I have created a testcase for this bug and
 *              will use it to verify the fix.
 *           2. Tensor has run a R1.2 server with the same fix in it.
 *              (At that time, this is a workaround because we don't
 *               know the cause.  But it turns out that this is a good
 *               fix for the problem.)  They report that the fix works.
 *
 *  Module(s): server/i860/queue.c
 *
 * In remque(), check if the forward and backward pointers are null
 * before dereferencing them.
 *
 * Revision 1.5  1994/11/18  20:32:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:59:44  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:18:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:10:18  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:04  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:10  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/11/13  12:18:36  rabii
 * 	Initial checkin
 *
 * Revision 2.2  91/09/03  11:12:52  jsb
 * 	First checkin. Derived from i386 sources by Intel SSD.
 * 	[91/09/03  10:20:16  jsb]
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
	if (elt->next != (queue_entry_t)0) {
		(elt->next)->prev = elt->prev;
	}
	if (elt->prev != (queue_entry_t)0) {
		(elt->prev)->next = elt->next;
	}
	return((int)elt);
}
