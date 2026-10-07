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
 * $Log: ipc_net.c,v $
 * Revision 1.12  1994/11/18  20:55:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/07/12  19:24:29  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.10  1994/03/19  01:29:12  lenb
 * Benefit: use positive logic on locking assertions
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 1.9.2.1  1994/03/11  23:01:47  andyp
 * Corrected the use of varargs to be more portable.
 *
 * Revision 1.9  1993/12/08  18:12:07  rkl
 *  Removed the ability to "lock on" to a node using a filter value.
 *
 *  Reviewer: Joel
 *  Risk: moderate
 *  Benefit or PTS #: 7317
 *  Testing: ORNL PFS acceptance tests
 *  Module(s): msgp_ipc.c mcmsg_ipc.c mcmsg_ipc.h ipc_net.c ipc_unreliable.c
 *
 * Revision 1.8  1993/10/20  16:02:02  rkl
 * Added NORMA Fast OOL capability.  When a single OOL type that is larger
 * than a VM page and <= a page list is sent, it will attempt to use the
 * Fast OOL transfer method.  This method allows all pages except for the
 * last to be sent without waiting for an ACK from the receiver.  If the
 * receiver does not have the resources to support the transfer, it will
 * be done in the regular way.
 *
 * Revision 1.7  1993/10/01  19:35:05  andyp
 * Boost the page list refill parameter above the high water mark
 * to buy some headroom.
 *
 * Revision 1.6  1993/09/28  18:03:42  andyp
 * Update for the 1.2 release.
 *
 *
 *	Pass an additional parameter to netipc_page_put identifying
 *	the caller, for debugging purposes.
 *	The netipc page pool refill mark may not be lower than
 *	the low-water mark.  Otherwise, NORMA IPC reception may
 *	grind to a halt without causing the pool to be refilled.
 *	Also:  additional tr debugging output.
 *	[alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:20:26  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:50:31  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:45:47  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:19:31  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/16  05:13:02  SSD
 * third code drop for page flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_net.c,v
 * Revision 1.2.4.6  1993/07/06  15:40:28  rwd
 * 	netipc_able_continue_recv moved here and looks at netipc_page_list
 * 	AND calls the vm check.
 * 	[1993/07/02  21:18:59  rwd]
 *
 * Revision 1.2.4.5  1993/06/30  16:16:36  rwd
 * 	NORMA hasn't used unacks for a long time.  [rwd, alanl]
 * 	[1993/06/29  13:05:37  rwd]
 * 
 * Revision 1.2.4.4  1993/04/27  17:43:24  alanl
 * 	Added new refill threshold logic for netipc page pool.  (dlb)
 * 	[1993/04/27  17:41:10  alanl]
 * 
 * Revision 1.2.4.3  1993/02/02  13:39:43  dwm
 * 	Add vm_page_gobble() calls where needed. (dwm bug #542)
 * 
 * 	Added netipc_intr_lock() and netipc_intr_unlock()
 * 	for setting netipc_lock_owner state.  Switch to new
 * 	kserver_pageout_support thread synchronization. (alanl)
 * 	[1993/02/01  22:35:58  dwm]
 * 
 * Revision 1.2.4.2  1993/01/20  02:48:02  dwm
 * 	Added some counters to netipc code: grabs, ungrabs, replenishments.
 * 	[1993/01/20  02:43:08  dwm]
 * 
 * Revision 1.2.1.2  1993/01/19  18:56:34  dwm
 * 	Added some counters to netipc code: grabs, ungrabs, replenishments.
 * 
 * Revision 1.2  1992/11/25  01:14:11  robert
 * 	integrate changes below for norma_14
 * 
 * 	Dave Mitchell (dwm) at Open Software Foundation 11-Nov-92
 * 	Count up reservation denials when memory is already low.
 * 	Edit netipc_reserve_page tr.
 * 	[1992/11/18  23:30:31  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/13  19:38:26  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:45:16  robert]
 * 
 * 	integrate any changes for norma 14 below
 * 	[1992/11/05  19:04:18  robert]
 * 
 * Revision 0.0  92/10/26            alan
 * 	Track page reservations with tr's.
 * 	[92/10/26            alan]
 * 
 * Revision 0.0  92/08/05            jsb
 * 	Added printf7.
 * 
 * 	Revision 1.1  1992/11/05  20:59:37  robert
 * 	Initial revision
 * 	[92/08/05            jsb]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 *
 * Revision 2.10.2.11  92/09/15  17:33:53  jeffreyh
 * 	Add debugging assert
 * 	[92/07/02            jeffreyh]
 * 
 * 	Use pageable copy objects for large NORMA messages.
 * 	Small messages continue to be handled using page list
 * 	technology at interrupt level -- in theory, minimizing
 * 	latency.  Pages belonging to messages larger than a
 * 	cutoff limit are sent to the netipc_thread for reassembly.
 * 	[92/06/02            alanl]
 * 
 * Revision 2.10.2.10  92/06/24  18:01:04  jeffreyh
 * 	Changed netipc_reserve_page to turn down packets from all
 * 	if memory gets short.
 * 
 * 	Moved check for vm_privilege to top of netipc_replenish to stop
 * 	netipc_output_replenish from being run by the default pager. It
 * 	has caused deadlocks.
 * 	[92/06/10            jeffreyh]
 * 
 * Revision 2.10.2.9  92/05/27  00:49:26  jeffreyh
 * 	Fixed argument to thread_wakeup_one in netipc_page_grab
 * 	[92/05/11            jeffreyh]
 * 
 * Revision 2.10.2.8.1.1  92/05/08  10:43:50  jeffreyh
 * 	Implement reservation system for netipc_page_list memory.
 * 	Users first reserve a page then allocate the page using
 * 	netipc_page_grab.  Reservations can be cancelled.  If a
 * 	reservation fails, special-case allocation can be done
 * 	using a forcible reservation.  If memory runs low, additional
 * 	pages can be stolen from the VM system's reserved page pool.
 * 	When returning memory to the netipc_page_list, excess pages
 * 	will be returned to the VM system.
 * 	[92/05/07            alanl]
 * 
 * Revision 2.10.2.8  92/04/09  14:24:17  jeffreyh
 * 	Do not set ch_timestamp on machines other then iPSC860
 * 
 * Revision 2.10.2.7  92/04/08  15:45:45  jeffreyh
 * 	Removed i860 debugging ifdefs.  Cleaned up history buffers.
 * 	[92/04/08            andyp]
 * 
 * Revision 2.10.2.6  92/03/28  10:11:43  jeffreyh
 * 	Have netipc thread clean up undeliverable kmsgs.
 * 	[92/03/20  14:12:41  jsb]
 * 
 * Revision 2.10.2.5  92/02/21  11:24:34  jsb
 * 	Moved spl{on,off} definitions earlier in the file (before all uses).
 * 	[92/02/18  08:03:01  jsb]
 * 
 * 	Removed accidently included lint declarations of panic, etc.
 * 	[92/02/16  11:17:48  jsb]
 * 
 * 	Eliminated netipc_thread_wakeup/netipc_replenish race.
 * 	[92/02/09  14:16:24  jsb]
 * 
 * Revision 2.10.2.4  92/02/18  19:14:43  jeffreyh
 * 	[intel] added support for callhere debug option of iPSC.
 * 	[92/02/13  13:02:21  jeffreyh]
 * 
 * Revision 2.10.2.3  92/01/21  21:51:30  jsb
 * 	From sjs@osf.org: moved node_incarnation declaration here from
 * 	ipc_ether.c.
 * 	[92/01/17  14:37:03  jsb]
 * 
 * 	New implementation of netipc lock routines which uses sploff/splon
 * 	and which releases spl before calling interrupt handlers (but after
 * 	taking netipc lock).
 * 	[92/01/16  22:20:30  jsb]
 * 
 * 	Removed panic in netipc_copy_grab: callers can now deal with failure.
 * 	[92/01/14  21:59:10  jsb]
 * 
 * 	In netipc_drain_intr_request, decrement request counts before calling
 * 	interrupt routines, not after. This preserves assertion that there is
 * 	at most one outstanding send or receive interrupt.
 * 	[92/01/13  20:17:18  jsb]
 * 
 * 	De-linted.
 * 	[92/01/13  10:15:50  jsb]
 * 
 * 	Now contains locking, allocation, and debugging printf routines.
 * 	Purged log.
 * 	[92/01/11  17:38:28  jsb]
 * 
 */ 
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1992 Carnegie Mellon University
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
 *	File:	norma/ipc_net.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Routines for reliable delivery and flow control for NORMA_IPC.
 */

#include <norma/ipc_net.h>
#include <norma/tr.h>

#if	i386 || i860
#else
#define	sploff()	splhigh()
#define	splon(s)	splx(s)
#endif

/*
 * Not proven to be multiprocessor-safe
 */

unsigned long node_incarnation = 1;		/* should never be zero */

decl_simple_lock_data(,netipc_lock)

thread_t netipc_lock_owner = THREAD_NULL;
#define	THREAD_INTR	((thread_t) 1)

int send_intr_request = 0;
int recv_intr_request = 0;
int timeout_intr_request = 0;

#if	iPSC386 || iPSC860
#if	iPSC860
#include <i860ipsc/nodehw.h>
#endif	iPSC860
extern void	netipc_called_here();
#endif	iPSC386 || iPSC860


/*
 * Called with interrupts not explicitly disabled but with lock held.
 * Returns with interrupts as they were and with lock released.
 */
int
netipc_drain_intr_request()
{
	int s;

	s = sploff();
	assert(netipc_lock_owner != THREAD_NULL);
	while (send_intr_request > 0 ||
	       recv_intr_request > 0 ||
	       timeout_intr_request > 0) {
		/*
		 * Send and receive interrupts are counting interrupts.
		 * Many timeout interrupts map into one.
		 */
		netipc_lock_owner = THREAD_INTR;
		if (send_intr_request > 0) {
			send_intr_request--;
			splon(s);
			_netipc_send_intr();
			s = sploff();
		} else if (recv_intr_request > 0) {
			recv_intr_request--;
			splon(s);
			_netipc_recv_intr();
			s = sploff();
		} else {
			assert(timeout_intr_request > 0);
			timeout_intr_request = 0;
			splon(s);
			_netipc_timeout_intr();
			s = sploff();
		}
	}
	netipc_lock_owner = THREAD_NULL;
	splon(s);
}

/*
 * XXX
 * These testing functions should have spls.
 */

boolean_t
netipc_thread_locked()
{
	return (netipc_lock_owner == current_thread());
}

boolean_t
netipc_thread_not_locked()
{
	return (netipc_lock_owner != current_thread());
}

boolean_t
netipc_intr_locked()
{
	return (netipc_lock_owner == THREAD_INTR);
}

boolean_t
netipc_locked()
{
	return (netipc_lock_owner != THREAD_NULL);
}

boolean_t
netipc_unlocked()
{
	return (netipc_lock_owner == THREAD_NULL);
}

netipc_intr_lock()
{
	assert(netipc_unlocked());
	netipc_lock_owner = THREAD_INTR;
}

void
netipc_thread_lock()
{
	int s;

	/*
	 * Threads fight among themselves.
	 */
	simple_lock(&netipc_lock);

	/*
	 * A single thread fights against interrupt handler.
	 */
	s = sploff();
	assert(netipc_unlocked());
	netipc_lock_owner = current_thread();
	splon(s);
}

void
netipc_intr_unlock()
{
	assert(netipc_intr_locked());
	netipc_lock_owner = THREAD_NULL;
}

void
netipc_thread_unlock()
{
	assert(netipc_thread_locked());

	/*
	 * Process queued interrupts, and release simple lock.
	 */
	netipc_drain_intr_request();
	simple_unlock(&netipc_lock);
}

netipc_send_intr()
{
	int s;

	s = sploff();
	if (netipc_lock_owner == THREAD_NULL) {
		netipc_lock_owner = THREAD_INTR;
		splon(s);
		_netipc_send_intr();
		netipc_drain_intr_request();
	} else {
		assert(send_intr_request == 0);
		send_intr_request++;
		splon(s);
	}
}

netipc_recv_intr()
{
	int s;

	s = sploff();
	if (netipc_lock_owner == THREAD_NULL) {
		netipc_lock_owner = THREAD_INTR;
		splon(s);
		_netipc_recv_intr();
		netipc_drain_intr_request();
	} else {
		assert(recv_intr_request == 0);
		recv_intr_request++;
		splon(s);
	}
}

netipc_timeout_intr()
{
	int s;

	s = sploff();
	if (netipc_lock_owner == THREAD_NULL) {
		netipc_lock_owner = THREAD_INTR;
		splon(s);
		_netipc_timeout_intr();
		netipc_drain_intr_request();
	} else {
		timeout_intr_request = 1;
		splon(s);
	}
}

boolean_t	netipc_thread_awake = FALSE;
boolean_t	netipc_thread_reawaken = FALSE;
int		netipc_thread_awaken = 0;

/*
 *	The netipc_page_list contains pages set aside
 *	for the netipc system.  By using a separate list,
 *	pages can be allocated at interrupt level.
 *
 *	The list has three important levels:
 *		list_high	maximum number of pages allowed on list
 *		list_low	the system attempts to keep at least
 *				this many pages on the list
 *		list_fallback_limit
 *				minimum number of pages kept around
 *				to satisfy special requests
 *
 *	These counters, adjusted dynamically, are also important:
 *		list_avail	number of pages not reserved
 *		list_count	number of pages on the list
 *
 *	In normal operation, pages are "reserved" from the
 *	list and then actually allocated from the list at a
 *	future time.  (Alternately, the reservation may be
 *	cancelled.)  A normal reservation will be denied
 *	if there is no page available to satisfy it.  A forced
 *	reservation causes the system to take the fallback
 *	pool into account before denying a request.
 *
 *	The fallback pool exists for handling special messages.
 *	These messages are critical to the pageout paths both
 *	for the local node and for remote nodes.  See the function
 *	ipc_unreliable.c:netipc_special_kmsg() for details.
 *
 *	In normal operation, the list is replenished with pages
 *	returned from previous allocation requests or by pages
 *	fed onto the list by the netipc_thread.  When memory is
 *	low, we use the kserver_pageout_support_thread to trickle
 *	enough pages back onto the list so that fallback requests
 *	can be granted.
 *
 *	The values for list_low and list_high should take
 *	into account the number of pages set aside for the
 *	fallback pool.
 *
 *	The entire set of list-related variables is protected by
 *	a single simple lock, the netipc_page_list_lock.  This lock
 *	must always be taken with interrupts disabled.  I have
 *	chosen sploff (splhigh) but only because the rest of the netipc
 *	system seems to run this way; I would have preferred splnet.
 *
 *	netipc_page_list_{low,high,refill} will be bootime adjusted if the
 *	current node is the root of a paging tree.
 */

vm_page_t		netipc_page_list = VM_PAGE_NULL;
int			netipc_page_list_count = 0;
int			netipc_page_list_avail = 0;
#if	NORMA_FAST_OOL
#define	PAGE_LIST_HIGH		( VM_MAP_COPY_PAGE_LIST_MAX + \
				 (VM_MAP_COPY_PAGE_LIST_MAX / 2))
#define	PAGE_LIST_LOW		((PAGE_LIST_HIGH / 3) * 2)
#define	PAGE_LIST_REFILL	(PAGE_LIST_LOW + \
				 (PAGE_LIST_HIGH - PAGE_LIST_LOW) / 2)
#define	PAGE_LIST_FALLBACKS	((PAGE_LIST_HIGH - PAGE_LIST_LOW) / 2)

int			netipc_page_list_fallbacks = PAGE_LIST_FALLBACKS;
int			netipc_page_list_refill    = PAGE_LIST_REFILL;
int			netipc_page_list_owed      = 0;
int			netipc_page_list_low       = PAGE_LIST_LOW;
int			netipc_page_list_high      = PAGE_LIST_HIGH;
#else
int			netipc_page_list_fallbacks = 3;
int			netipc_page_list_refill = 28;
int			netipc_page_list_owed = 0;
int			netipc_page_list_low = 23;
int			netipc_page_list_high = 33;
#endif	NORMA_FAST_OOL
boolean_t		netipc_page_special_only = FALSE;
decl_simple_lock_data(static,netipc_page_list_lock)

/*
 *	Counters for netipc page allocation subsystem.
 *	A denied request is one rejected while there were
 *	still pages left in the fallback pool.  The impossible
 *	counter tallies attempts that failed because no pages
 *	at all were left on the list.
 */
int			c_netipc_reserve_denied = 0;
int			c_netipc_reserve_impossible = 0;
int			c_netipc_page_list_borrowed = 0;
int			c_netipc_page_list_returned = 0;
int			c_netipc_replenishments = 0;
int			c_netipc_grabs = 0;
int			c_netipc_ungrabs = 0;
int			c_netipc_replenish_fallbacks = 0;

extern zone_t		vm_map_copy_zone;
vm_map_copy_t		netipc_vm_map_copy_list = VM_MAP_COPY_NULL;
int			netipc_vm_map_copy_count = 0;

extern boolean_t	netipc_able_continue_recv();

#if	MACH_ASSERT
boolean_t		netipc_page_list_state_valid();
#endif

/*
 *	Add or return a page to the netipc_page_list pool.
 *
 *	This function also handles the case when someone attempts
 *	to add a page to the list and exceed the highwater mark.
 *	This problem can arise on multiprocessors, when the thread
 *	refilling the list races with other processors that may be
 *	returning previously allocated pages to the list.
 *	Four choices:
 *		- force all callers to do grody things like check
 *		the list variables under lock (which has to be dropped
 *		across calls into the VM system!), and maybe return
 *		pages to the VM system if the variables change
 *		- have netipc_page_put reject pages that exceed the
 *		highwater mark (caller responsible for freeing page)
 *		- have netipc_page_put return excess pages to the VM system
 *		- have netipc_page_put adjust the highwater mark
 *
 *	We implement the third option, returning excess pages to the
 *	VM system.  Also, netipc_page_put counts how many fallback pages
 *	are added to the netipc_page_list by borrowing from the reserved
 *	VM pool.  When the fallback pool has been repopulated, additional
 *	pages are returned to the VM system.
 */
void
netipc_page_put(m, borrowed_reserve_vm, caller)
vm_page_t 	m;
boolean_t	borrowed_reserve_vm;
char		*caller;
{
	boolean_t	resume_connections;
	int		s;
	TR_DECL("netipc_page_put");

	assert(netipc_locked());

	s = sploff();
	simple_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	if (borrowed_reserve_vm == TRUE) {
		tr5("BORROWED count 0x%x owed 0x%x avail 0x%x caller %s",
		    netipc_page_list_count, netipc_page_list_owed,
		    netipc_page_list_avail, caller);
	} else {
		tr5("count 0x%x owed 0x%x avail 0x%x caller %s",
		    netipc_page_list_count, netipc_page_list_owed,
		    netipc_page_list_avail, caller);
	}

	/*
	 *	1.  Account for fallback pages being added to the list.
	 *	These pages were acquired by kserver_pageout_support_thread.
	 */
	if (borrowed_reserve_vm == TRUE) {
		++netipc_page_list_owed;
		++c_netipc_page_list_borrowed;
	}

	/*
	 *	2.  If we borrowed from the reserved VM pool, or if somehow
	 *	we are trying to stuff pages onto the netipc_page_list beyond
	 *	the highwater mark, return the page to the VM system.
	 *	We will return borrowed pages to the system in preference to
	 *	adding pages beyond the fallback limit.  Thus, memory in the
	 *	netipc_page_list will remain tight (and we will continue
	 *	rejecting all but special-case messages) until the overall VM
	 *	situation improves.
	 */
	if ((netipc_page_list_owed > 0 &&
	    netipc_page_list_count >= netipc_page_list_fallbacks)
	    || netipc_page_list_count >= netipc_page_list_high) {
		if (netipc_page_list_owed)
			--netipc_page_list_owed;
		simple_unlock(&netipc_page_list_lock);
		splon(s);
		vm_page_free(m);
		++c_netipc_page_list_returned;
		return;
	}

	/*
	 *	3.  The page really should be saved.
	 */
	* (vm_page_t *) &m->pageq.next = netipc_page_list;
	netipc_page_list = m;

	++netipc_page_list_avail;
	++netipc_page_list_count;

	assert(netipc_page_list_state_valid());
	simple_unlock(&netipc_page_list_lock);
	splon(s);
}


/*
 *	Reserve a page for future use.  The request
 *	will be granted if the number of pages available
 *	exceeds the number of pages reserved for special-
 *	case situations.  The reservation must be balanced
 *	later by a call to netipc_page_grab, to actually
 *	allocate the page from the list, or by a call to
 *	netipc_reserve_cancel, to give up the reservation.
 */
boolean_t
#if	NORMA_FAST_OOL
netipc_reserve_page(pages)
int	pages;
#else
netipc_reserve_page()
#endif	NORMA_FAST_OOL
{
	int	success;
	int	s;
	TR_DECL("netipc_reserve_page");

	s = sploff();
	simple_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	tr5((netipc_page_special_only ? 
	     "SPEC %s avail 0x%x vmfree 0x%x wire 0x%x" :
	     "!spec %s avail 0x%x vmfree 0x%x wire 0x%x"),
	    netipc_able_continue_recv() ? "able" : "!ABLE",
	    netipc_page_list_avail, vm_page_free_count, vm_page_wire_count);

	/*
	 *	1.  Assume failure.
	 */
	success = FALSE;

	/*
	 *	2.  In low-memory situations, we reject all
	 *	incoming pages until memory has been freed.
	 *	Check for plentiful memory.
	 */
	if (netipc_page_special_only && netipc_able_continue_recv())
			netipc_page_special_only = FALSE;

	/*
	 *	3.  If memory seems to be generally available,
	 *	check for pages in the netipc page pool.  If we
	 *	can't find any pages in the pool, declare a
	 *	memory shortage and give up.
	 *
	 *	N.B.  The pool availability check, below, is duplicated
	 *	in netipc_page_put.
	 */
	if (netipc_page_special_only == FALSE) {
#if	NORMA_FAST_OOL
		tr2("reserving %d pages", pages);
		if ((netipc_page_list_avail - (pages - 1)) >
		    netipc_page_list_fallbacks) {
			netipc_page_list_avail -= pages;
			success = TRUE;
		} else {
			++c_netipc_reserve_denied;
			/*
			 *  If this was not a fast OOL query, (i.e. pages = 1)
			 *  declare the shortage.
			 */
			if (pages == 1)
				netipc_page_special_only = TRUE;
		}
#else
		if (netipc_page_list_avail > netipc_page_list_fallbacks) {
			--netipc_page_list_avail;
			success = TRUE;
		} else {
			++c_netipc_reserve_denied;
			netipc_page_special_only = TRUE;
		}
#endif	NORMA_FAST_OOL
	} else {
		++c_netipc_reserve_denied;
	}

	simple_unlock(&netipc_page_list_lock);
	splon(s);
	return success;
}


/*
 * This function used to be just the vm_netipc_able_continue_recv
 * part.  This is not sufficient.  It is necessary to also check that
 * we have pages in the netipc_page_list or else we will just immediately
 * suspend again.
 */

boolean_t
netipc_able_continue_recv()
{
    extern boolean_t vm_netipc_able_continue_recv();
    return ((netipc_page_list_avail >= netipc_page_list_low) &&
	    vm_netipc_able_continue_recv());
}
/*
 *	Force a page reservation if at all possible.
 *	This call should be used only for reserving
 *	memory to permit processing messages of
 *	unusual priority.  In other words, messages
 *	critical to keeping the system running.
 */
boolean_t
netipc_reserve_page_force()
{
	int	success;
	int	s;
	TR_DECL("netipc_reserve_page_force");

	s = sploff();
	simple_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	tr3("avail 0x%x vmp_free 0x%x", netipc_page_list_avail,
	    vm_page_free_count);

	if (netipc_page_list_avail > 0) {
		--netipc_page_list_avail;
		success = TRUE;
	} else {
		++c_netipc_reserve_impossible;
		success = FALSE;
	}

	simple_unlock(&netipc_page_list_lock);
	splon(s);
	return success;
}


/*
 *	Cancel a previously reserved page.
 */
void
netipc_reserve_cancel()
{
	int	s;
	TR_DECL("netipc_reserve_cancel");

	s = sploff();
	simple_lock(&netipc_page_list_lock);
	tr2("avail 0x%x", netipc_page_list_avail);

	++netipc_page_list_avail;
	assert(netipc_page_list_state_valid());

	simple_unlock(&netipc_page_list_lock);
	splon(s);
	return;
}


#if	MACH_ASSERT
/*
 *	Debugging routine.  Check various pointers
 *	and counters to validate state of the
 *	netipc page subsystem.  Must be called
 *	while holding netipc_page_list_lock.
 */
static boolean_t
netipc_page_list_state_valid()
{
	assert(netipc_page_list_fallbacks > 0);
	assert(netipc_page_list_avail >= 0);
	assert(netipc_page_list_avail <= netipc_page_list_count);
	assert(netipc_page_list_count >= 0);
	assert(netipc_page_list_count <= netipc_page_list_high);
	return TRUE;
}
#endif


/*
 *	Initialize state of netipc page subsystem.
 *	Note that there is no need to allocate pages
 *	for the subsystem here; the first invocation
 *	of netipc_relenish will do that.
 */
void
netipc_page_init()
{
	simple_lock_init(&netipc_page_list_lock);
	if (netipc_page_list_refill < netipc_page_list_low) {
		printf("netipc_page_init warning:  refill %d is lower",
		       netipc_page_list_refill);
		printf(" than low-water mark %d\n", netipc_page_list_low);
	}
}


vm_map_copy_t
netipc_copy_grab()
{
	vm_map_copy_t copy;

	assert(netipc_locked());
	copy = netipc_vm_map_copy_list;
	if (copy != VM_MAP_COPY_NULL) {
		netipc_vm_map_copy_list = (vm_map_copy_t) copy->type;
		netipc_vm_map_copy_count--;
		c_netipc_grabs++;
		copy->type = VM_MAP_COPY_PAGE_LIST;
	}
	return copy;
}

void
netipc_copy_ungrab(copy)
	vm_map_copy_t copy;
{
	assert(netipc_locked());
	copy->type = (int) netipc_vm_map_copy_list;
	netipc_vm_map_copy_list = copy;
	netipc_vm_map_copy_count++;
	c_netipc_ungrabs++;
}

netipc_thread_wakeup()
{
	assert(netipc_locked());
	if (netipc_thread_awake) {
		netipc_thread_reawaken = TRUE;
	} else {
		thread_wakeup((int) &netipc_thread_awake);
	}
}

/*
 * XXX
 * The wakeup protocol for this loop is not quite correct...
 *
 * XXX
 * We should move the lists out all at once, not one elt at a time.
 *
 * XXX
 * The locking here is farcical.
 */
netipc_replenish(always)
	boolean_t always;
{
	vm_page_t	m;

	/*
	 *	Netipc_replenish could inadvertantly allocate
	 *	memory from the reserved VM pool if the function
	 *	is called by a vm-privileged thread.
	 */
	if (current_thread()->vm_privilege) {
		return;
	}

	assert(netipc_unlocked());
	netipc_output_replenish();	/* XXX move somewhere else */
	while (netipc_vm_map_copy_count < 300) {
		vm_map_copy_t copy;

		c_netipc_replenishments++;
		copy = (vm_map_copy_t) zalloc(vm_map_copy_zone);
		netipc_thread_lock();
		copy->type = (int) netipc_vm_map_copy_list;
		netipc_vm_map_copy_list = copy;
		netipc_vm_map_copy_count++;
		netipc_thread_unlock();
	}
	
	/*
	 *	Cut corners here:  examine the list_count and list_high
	 *	variables without taking any locks.  Assumes that the
	 *	memory subsystem is well-behaved.  On a uniprocessor,
	 *	this is not a problem.  On a multiprocessor, even if we
	 *	did the right locking here we could still wind up
	 *	blowing extra pages into the pool while we raced other
	 *	processors returning previously allocated pages to the pool.
	 *	netipc_page_put handles this problem.
	 */
	while (netipc_page_list_count < netipc_page_list_high) {
		m = vm_page_grab();
		if (m == VM_PAGE_NULL) {
			break;
		}
		m->tabled = FALSE;
		vm_page_init(m, m->phys_addr);
		vm_page_gobble(m); /* mark as consumed internally */
		/*
		 *	Take netipc_thread_lock for the sake of
		 *	those routine(s) called by netipc_page_put
		 *	that still expect this synchronization.
		 */
		netipc_thread_lock();
		netipc_page_put(m, FALSE, "netipc_replenish");
		netipc_thread_unlock();
	}
	while (always && netipc_page_list_count < netipc_page_list_low) {
		while ((m = vm_page_grab()) == VM_PAGE_NULL) {
			vm_page_wait(0);
		}
		m->tabled = FALSE;
		vm_page_init(m, m->phys_addr);
		vm_page_gobble(m); /* mark as consumed internally */
		/*
		 *	Take netipc_thread_lock for the sake of
		 *	those routine(s) called by netipc_page_put
		 *	that still expect this synchronization.
		 */
		netipc_thread_lock();
		netipc_page_put(m, FALSE, "netipc_replenish2");
		netipc_thread_unlock();
	}
}


/*
 *	This routine is called from the kserver_pageout_support_thread
 *	when the netipc_page_list is running so low it has even cut
 *	into the fallback page pool.  Because this routine is called
 *	from a thread with vm_privilege, be careful to only allocate
 *	as many pages as necessary to replenish the fallback pool.
 *
 *	XXX Concern:  what happens if pages get trapped in the pool and
 *	don't make it back to vm?
 */
void
netipc_replenish_fallbacks()
{
	vm_page_t	m;

	assert(current_thread()->vm_privilege);

	/*
	 * Allocate pages until the fallback pool has been restored
	 * to health.
	 *
	 * Cut corners:  don't bother locking around loop test.
	 */
	while (netipc_page_list_count < netipc_page_list_fallbacks) {
		++c_netipc_replenish_fallbacks;
		m = vm_page_grab();
		assert(m != VM_PAGE_NULL);
		m->tabled = FALSE;
		vm_page_init(m, m->phys_addr);
		vm_page_gobble(m); /* mark as consumed internally */
		/*
		 *	Take netipc_thread_lock for the sake of
		 *	those routine(s) called by netipc_page_put
		 *	that still expect this synchronization.
		 */
		netipc_thread_lock();
		netipc_page_put(m, TRUE, "netipc_replenish_fallbacks");
		netipc_thread_unlock();
	}
}


/*
 *	Grab a vm_page_t at interrupt level.
 *	Callers MUST have previously reserved a page!
 *	Given these rules, this routine should never fail.
 *
 *	If only fallback pages are left on the list, awaken
 *	the netipc_thread to replenish the list.  If even
 *	the fallback pages have vanished, kick-start the
 *	kserver_pageout_support_thread to replenish the list.
 *	The kserver_p_s_thread has vm_privilege.
 *
 *	XXX Waiting until the fallback pool is empty may
 *	not be a good idea.  On the other hand, allocating
 *	new fallback pages as soon as any of them disappear
 *	may not be a good idea, either.
 *
 *	Changed to add new refill threshold.
 */
vm_page_t
netipc_page_grab()
{
	vm_page_t	m;
	int		awaken_netipc_thread;
	int		awaken_kserver_thread;
	int		s;

	assert(netipc_locked());

	s = sploff();
	simple_lock(&netipc_page_list_lock);
	assert(netipc_page_list_state_valid());
	assert(netipc_page_list_count > 0);
	assert(netipc_page_list != VM_PAGE_NULL);

	m = netipc_page_list;
	netipc_page_list = (vm_page_t) m->pageq.next;
	--netipc_page_list_count;

	awaken_kserver_thread = (netipc_page_list_count == 0);
	awaken_netipc_thread = (netipc_page_list_count <=
				netipc_page_list_refill);

	simple_unlock(&netipc_page_list_lock);
	splon(s);
	if (awaken_kserver_thread)
		kserver_pageout_support_wakeup();
	if (awaken_netipc_thread)
		netipc_thread_wakeup();

	return m;
}


void
netipc_thread_continue()
{
	netipc_thread_lock();
	for (;;) {
		/*
		 * Record that we are awake.
		 * Look out for new awaken requests while we are out working.
		 */
		netipc_thread_awaken++;
		netipc_thread_awake = TRUE;
		netipc_thread_reawaken = FALSE;

		/*
		 * Call netipc_replenish with netipc lock unlocked.
		 */
		netipc_thread_unlock();
		netipc_replenish(TRUE);

		/*
		 * Call norma_ipc_clean_receive_errors to clean up
		 * kmsgs that couldn't be delivered (interrupt and
		 * ast code can't do this).
		 */
		norma_ipc_clean_receive_errors();
		netipc_thread_lock();

		/*
		 *	If we don't yet have enough pages, or someone
		 *	came up with something new for us to do, then
		 *	do more work before going to sleep.
		 *
		 *	N.B.  Cutting corners:  not using a lock here.
		 *	Assumes memory architecture is kind.
		 */
		if (netipc_page_list_count < netipc_page_list_low ||
		    netipc_thread_reawaken) {
			continue;
		}

		/*
		 * Nothing left for us to do right now.  Go to sleep.
		 */
		netipc_thread_awake = FALSE;
		assert_wait((int) &netipc_thread_awake, FALSE);
		(void) netipc_thread_unlock();
		thread_block(netipc_thread_continue);
		netipc_thread_lock();
	}
}

void
netipc_thread()
{
	thread_set_own_priority(0);	/* high priority */
	netipc_thread_continue();
}

int Noise0 = 0;	/* print netipc packets */	
int Noise1 = 0;	/* notification and migration debugging */
int Noise2 = 0;	/* synch and timeout printfs */
int Noise3 = 0;	/* copy object continuation debugging */
int Noise4 = 0;	/* multiple out-of-line section debugging */
int Noise5 = 0;	/* obsolete acks */
int Noise6 = 0;	/* short print of rcvd packets, including msgh_id */
int Noise7 = 0; /* short print of rcvd packets, including msgh_id */

extern cnputc();

/* VARARGS */
printf1(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (Noise1) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS */
printf2(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (Noise2) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS */
printf3(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (Noise3) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS */
printf4(va_alist)
	va_dcl
{
	va_list	listp;
	char	 *fmt;

	if (Noise4) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS */
printf5(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (Noise5) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS */
printf6(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (Noise6) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

/* VARARGS */
printf7(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (Noise7) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

#if	iPSC386 || iPSC860
#define	MAX_CALLS	256

struct netipc_call_history {
	char		*ch_filename;
	char		*ch_notation;
	int		ch_lineno;
	unsigned long	ch_timestamp;
};
struct netipc_call_history netipc_call_history[MAX_CALLS];
int	called_here_next = 0;

void netipc_called_here(filename, line, notation)
	char	*filename, *notation;
	int	line;
{
	int	s;
	struct netipc_call_history *ch;

	s = sploff();
	ch = &netipc_call_history[called_here_next++];
	if (called_here_next >= MAX_CALLS) {
		called_here_next = 0;
	}
	ch->ch_filename = filename;
	ch->ch_notation = notation;
	ch->ch_lineno = line;
#if	iPSC860
        ch->ch_timestamp = (unsigned long) inb(COUNTER_PORT);
#endif
	netipc_call_history[called_here_next].ch_filename = 0;
	splon(s);
}


void db_show_netipc_called_here()
{
	int	i, j;
	char	*s, *slash;
	struct netipc_call_history *ch;

	db_printf(" #   Line             File     When Note\n");
	j = called_here_next - 1;
	for (i = 0; i < MAX_CALLS; i++) {
		if (j < 0) {
			j = MAX_CALLS - 1;
		}
		ch = &netipc_call_history[j];
		if (ch->ch_filename) {
			slash = 0;
			for (s = ch->ch_filename; *s; s++) {
				if (*s == '/') {
					slash = s + 1;
				}
			}
			db_printf("%3d %5d %16s %8u %s\n",
				j,
				ch->ch_lineno,
				slash ? slash : ch->ch_filename,
				ch->ch_timestamp,
				ch->ch_notation);
			j--;
		} else {
			return;
		}
	}
}

#endif	iPSC386 || iPSC860
