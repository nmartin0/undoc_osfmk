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
 * $Log: ipc_port.c,v $
 * Revision 1.16  1994/11/18  20:49:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/08/31  21:24:35  mtm
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
 * Revision 1.13  1994/07/29  21:50:08  rkl
 *  Fixed spelling errors (Principle to Principal).
 *
 * Revision 1.12  1994/07/15  16:17:02  stans
 *  Reduce norma log noise in dipc_port_log(), removed exported flag in favor
 *  of saving UID after it's been removed.
 *
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #: debug
 *  Testing: developer
 *
 * Revision 1.11  1994/07/13  01:22:14  andyp
 * Late changes from the NORMA2 branch.
 *
 * Revision 1.10  1994/07/12  19:22:15  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9  1994/06/27  23:22:55  stans
 *   In function db_port_walk(), more separation between active/inactive port
 *   reference count buckets.
 *  Reviewer:self
 *  Risk: low
 *  Benefit or PTS #: make db_port_walk() port reference counts (active/inactive)
 * 		   correct.
 *  Testing: developer
 *
 * Revision 1.8  1994/06/27  22:33:06  stans
 *  In funtion db_port_walk(), make port reference count overflow active/inactive
 *  buckets reflect active/inactive port status instead of ignoring port state.
 *
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #: make debug fields useful
 *  Testing: developer
 *
 * Revision 1.7  1994/05/18  21:08:22  flb
 *  Reviewer: Self
 *  Risk: low
 *  Benefit or PTS #: 8130, 8281 & 8509
 *  Testing: sats, eats
 *  Module(s): ipc_port.c
 *
 *  Description: if a thread is holding a kmsg, we now wake up the thread instead
 * 			   of attempting to destroy the kmsg.
 *
 * Revision 1.6.4.42  1994/07/13  00:48:47  rkl
 *  Use new name in ipc_port_t
 *
 * Revision 1.6.4.41  1994/07/05  17:53:24  rkl
 *  Handle releasing net_kmsg's from destroyed port.
 *
 * Revision 1.6.4.40  1994/07/01  17:14:38  stans
 *   Be aware of known IPC spaces in ipc_port_print()
 *   Enhance 'tracer' as a filtering mechanism.
 *
 * Revision 1.6.4.39  1994/06/27  23:17:23  stans
 *   db_port_walk(), more separation of active/inactive reference_count buckets.
 *
 * Revision 1.6.4.38  1994/06/27  22:55:41  andyp
 * Delete the log. Fix for PTS 8509 (old NORMA): avoid duplicate kmsg
 * destruction when destroying a port holding queued messages (leading
 * to corrupted zones).
 *
 * Revision 1.6.4.37  1994/06/27  22:03:36  stans
 *   In db_port_walk(), make active and inactive reference_count overflow
 *   buckets reflect active/inactive instead of both.
 *
 * Revision 1.6.4.36  1994/06/22  00:44:46  rkl
 *  Ability to 'mark' a port and find it thru dp_port_walk()
 *
 * Revision 1.6.4.35  1994/06/20  17:13:55  rkl
 *  Removed BORG capability.
 *
 * Revision 1.6.4.34  1994/06/10  15:13:53  rkl
 *  Added db_find_rcvr() debug routine to print all port/psets a thread is
 *  currently receiving on.
 *
 * Revision 1.6.4.33  1994/06/09  18:52:08  stans
 *   db_port_walk(): both versions of NORMA use
 * 	ipc_space_remote, remove '#ifdef NORMA_IPC'.
 *   added db_ref() which calls db_port_walk() with correct args.
 *
 * Revision 1.6.4.32  1994/06/07  05:07:31  rkl
 *  Initialize spare bits to 0 and printed some of the spare bit vaules.
 *
 * Revision 1.6.4.31  1994/05/26  22:58:46  stans
 * debug output reformat
 *
 * Revision 1.6.4.30  1994/05/25  16:51:23  stans
 *   Do not initialize elements that are no longer in the structure.
 *
 * Revision 1.6.4.29  1994/05/25  16:39:56  stans
 *   dipc port leak debug rtn 'db_ports()'
 *
 * Revision 1.6.4.28  1994/05/25  06:54:20  andyp
 * Added fields for remote kobj server support.  Removed the overloading
 * of the remote enqueue status with the blocked senders fields.
 * Added some comments and debug support.
 *
 * Revision 1.6.4.27  1994/05/18  22:40:50  stans
 * NORMA_IPC==0 cleanup
 *
 * Revision 1.6.4.26  1994/05/18  17:00:24  stans
 *   Handle '-norma_ipc', Deadname request/init conditionals
 *
 * Revision 1.6.4.25  1994/05/13  00:09:09  stans
 *   show port has kserver active on separate line. No output if kserver is
 *   not active.
 *
 * Revision 1.6.4.24  1994/05/11  01:09:21  stans
 *   Name change: 'kserver_id' --> 'dipc_ksvr_id'
 *
 * Revision 1.6.4.23  1994/05/07  00:32:26  stans
 *   Be tidy: in ipc_port_destroy() clear deadname request pointer after the
 *   memory has been released; leave no doubts.
 *
 * Revision 1.6.4.22  1994/05/05  02:29:19  stans
 *   Support has migrated and has been exported in show port cmd.
 *
 * Revision 1.6.4.21  1994/05/05  02:04:43  stans
 *   new show port fields: kserver_id, has migrated
 *
 * Revision 1.6.4.20  1994/05/04  19:38:47  rkl
 *  Added DIPC remote_sorights to show port command.
 *
 * Revision 1.6.4.19  1994/04/26  16:02:49  stans
 *   Cleanup "show port" output.
 *
 * Revision 1.6.4.18  1994/04/21  20:22:21  andyp
 * Added dipc_borg switch.
 *
 * Revision 1.6.4.17  1994/04/20  00:32:10  stans
 *   Support new dipc_do_nms bit
 *
 * Revision 1.6.4.16  1994/04/19  23:35:04  andyp
 * norma_log ports going back into the zone.
 *
 * Revision 1.6.4.15  1994/04/19  20:28:58  rkl
 *  Changed macro DIPC_IS_NORMA2() to DIPC_IS_PROXY() for speed and less
 *  noisy log file.
 *
 * Revision 1.6.4.14  1994/04/19  01:39:13  stans
 *   In ipc_port_destroy()
 * 	decrement ip_msgcount for each kmsg destroyed; house-keeping.
 * 	decrement port reference to each queued kmsg which is destroyed.
 *
 * Revision 1.6.4.13  1994/04/15  21:16:41  rkl
 *  db_port_walk() now displays the types of ports.
 *
 * Revision 1.6.4.12  1994/04/14  00:41:40  stans
 *   In 'ipc_port_destroy()' dequeue/delete different types of queued kmsgs.
 *
 * Revision 1.6.4.11  1994/04/13  20:04:28  andyp
 * Added DIPC deadnames notification.  Added the option of deallocating
 * sent messages from an ast rather than waking a thread.
 *
 * Revision 1.6.4.10  1994/04/06  00:33:26  stans
 * more !NORMA_IPC
 *
 * Revision 1.6.4.9  1994/03/23  17:29:40  stans
 * added dipc_port_destroy_state()
 *
 * Revision 1.6.4.8  1994/03/18  20:57:55  stans
 * show port debug cleanup
 *
 * Revision 1.6.4.7  1994/03/16  21:44:07  rkl
 *  Added debug printing of blocked senders fields
 *
 * Revision 1.6.4.6  1994/03/15  23:26:24  andyp
 * Added fields for NORMA2 remote blocked senders.
 *
 * Revision 1.6.4.5  1994/03/11  18:47:11  stans
 * show kmsg + NORMA2
 *
 * Revision 1.6.4.4  1994/03/11  17:04:36  rkl
 *  Added debug port printing support for NORMA2 fields.
 *
 * Revision 1.6.4.3  1994/03/10  07:14:05  rkl
 *  Added call to dipc_port_destroy() in ipc_port_destroy()
 *
 * Revision 1.6.4.2  1994/03/10  02:42:59  stans
 * corrected spelling on dipc_migragte_state
 *
 * Revision 1.6.4.1  1994/03/04  01:14:08  rkl
 *  Added initialization of NORMA2 fields in the port.
 *
 * Revision 1.6  1993/09/28  18:25:14  andyp
 * Update for 1.2 release.
 *
 *
 *	Under MACH_ASSERT, track port allocation and deallocation.
 *	[alanl@osf.org, andyp@ssd.intel.com]
 *	Fix annoying port display.  [alanl@osf.org]
 *
 *	Add port counting code, currently disabled, for future debugging
 *	of port leaks.  [alanl@osf.org]
 *
 * Revision 1.5  1993/06/30  22:42:46  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:36:58  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:33:25  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * END SSD HISTORY
 */
 
/*
 * @OSF_FREE_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 *	File:	ipc/ipc_port.c
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Functions to manipulate IPC ports.
 */

#include <mach_ipc_compat.h>
#include <mach_kdb.h>
#include <norma_ipc.h>

#include <mach/port.h>
#include <mach/kern_return.h>
#include <kern/lock.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_notify.h>

#if	NORMA_IPC
#include <norma/ipc_node.h>
#endif	NORMA_IPC

#if	NORMA2
#include <rpc_rdma/rdma.h>
#include <norma2/meta_kmsg.h>
#include <norma2/dipc_uid.h>
#include <ipc/ipc_kmsg.h>

unsigned short	dipc_tracer;	/* for debug */

#endif	/* NORMA2 */

decl_simple_lock_data(, ipc_port_multiple_lock_data)

decl_simple_lock_data(, ipc_port_timestamp_lock_data)
ipc_port_timestamp_t ipc_port_timestamp_data;

unsigned	c_ipc_port_destroy_wakeup = 0;

/*
 *	Routine:	ipc_port_timestamp
 *	Purpose:
 *		Retrieve a timestamp value.
 */

ipc_port_timestamp_t
ipc_port_timestamp()
{
	ipc_port_timestamp_t timestamp;

	ipc_port_timestamp_lock();
	timestamp = ipc_port_timestamp_data++;
	ipc_port_timestamp_unlock();

	return timestamp;
}

/*
 *	Routine:	ipc_port_dnrequest
 *	Purpose:
 *		Try to allocate a dead-name request slot.
 *		If successful, returns the request index.
 *		Otherwise returns zero.
 *	Conditions:
 *		The port is locked and active.
 *	Returns:
 *		KERN_SUCCESS		A request index was found.
 *		KERN_NO_SPACE		No index allocated.
 */

kern_return_t
ipc_port_dnrequest(port, name, soright, indexp)
	ipc_port_t port;
	mach_port_t name;
	ipc_port_t soright;
	ipc_port_request_index_t *indexp;
{
	ipc_port_request_t ipr, table;
	ipc_port_request_index_t index;

	assert(ip_active(port));
	assert(name != MACH_PORT_NULL);
	assert(soright != IP_NULL);

	table = port->ip_dnrequests;
	if (table == IPR_NULL)
		return KERN_NO_SPACE;

	index = table->ipr_next;
	if (index == 0)
		return KERN_NO_SPACE;

	ipr = &table[index];
	assert(ipr->ipr_name == MACH_PORT_NULL);

	table->ipr_next = ipr->ipr_next;
	ipr->ipr_name = name;
	ipr->ipr_soright = soright;

	*indexp = index;
	return KERN_SUCCESS;
}

/*
 *	Routine:	ipc_port_dngrow
 *	Purpose:
 *		Grow a port's table of dead-name requests.
 *	Conditions:
 *		The port must be locked and active.
 *		Nothing else locked; will allocate memory.
 *		Upon return the port is unlocked.
 *	Returns:
 *		KERN_SUCCESS		Grew the table.
 *		KERN_SUCCESS		Somebody else grew the table.
 *		KERN_SUCCESS		The port died.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate new table.
 */

kern_return_t
ipc_port_dngrow(port)
	ipc_port_t port;
{
	ipc_table_size_t its;
	ipc_port_request_t otable, ntable;

	assert(ip_active(port));

	otable = port->ip_dnrequests;
	if (otable == IPR_NULL)
		its = &ipc_table_dnrequests[0];
	else
		its = otable->ipr_size + 1;

	ip_reference(port);
	ip_unlock(port);

	if ((its->its_size == 0) ||
	    ((ntable = it_dnrequests_alloc(its)) == IPR_NULL)) {
		ipc_port_release(port);
		return KERN_RESOURCE_SHORTAGE;
	}

	ip_lock(port);
	ip_release(port);

	/*
	 *	Check that port is still active and that nobody else
	 *	has slipped in and grown the table on us.  Note that
	 *	just checking port->ip_dnrequests == otable isn't
	 *	sufficient; must check ipr_size.
	 */

	if (ip_active(port) &&
	    (port->ip_dnrequests == otable) &&
	    ((otable == IPR_NULL) || (otable->ipr_size+1 == its))) {
		ipc_table_size_t oits;
		ipc_table_elems_t osize, nsize;
		ipc_port_request_index_t free, i;

		/* copy old table to new table */

		if (otable != IPR_NULL) {
			oits = otable->ipr_size;
			osize = oits->its_size;
			free = otable->ipr_next;

			bcopy((char *)(otable + 1), (char *)(ntable + 1),
			      (osize - 1) * sizeof(struct ipc_port_request));
		} else {
			osize = 1;
			free = 0;
		}

		nsize = its->its_size;
		assert(nsize > osize);

		/* add new elements to the new table's free list */

		for (i = osize; i < nsize; i++) {
			ipc_port_request_t ipr = &ntable[i];

			ipr->ipr_name = MACH_PORT_NULL;
			ipr->ipr_next = free;
			free = i;
		}

		ntable->ipr_next = free;
		ntable->ipr_size = its;
		port->ip_dnrequests = ntable;
		ip_unlock(port);

		if (otable != IPR_NULL) {
			it_dnrequests_free(oits, otable);
	        }
#if	NORMA2
	        else if ( DIPC_IS_PROXY(port) ) {
			dipc_dead_name_request_init(port);
		}
#endif	NORMA2
#if	NORMA_IPC
	        else
			norma_ipc_dnrequest_init(port);
#endif	NORMA_IPC
	} else {
		ip_check_unlock(port);
		it_dnrequests_free(its, ntable);
	}

	return KERN_SUCCESS;
}
 
/*
 *	Routine:	ipc_port_dncancel
 *	Purpose:
 *		Cancel a dead-name request and return the send-once right.
 *	Conditions:
 *		The port must locked and active.
 */

ipc_port_t
ipc_port_dncancel(port, name, index)
	ipc_port_t port;
	mach_port_t name;
	ipc_port_request_index_t index;
{
	ipc_port_request_t ipr, table;
	ipc_port_t dnrequest;

	assert(ip_active(port));
	assert(name != MACH_PORT_NULL);
	assert(index != 0);

	table = port->ip_dnrequests;
	assert(table != IPR_NULL);

	ipr = &table[index];
	dnrequest = ipr->ipr_soright;
	assert(ipr->ipr_name == name);

	/* return ipr to the free list inside the table */

	ipr->ipr_name = MACH_PORT_NULL;
	ipr->ipr_next = table->ipr_next;
	table->ipr_next = index;

	return dnrequest;
}

/*
 *	Routine:	ipc_port_pdrequest
 *	Purpose:
 *		Make a port-deleted request, returning the
 *		previously registered send-once right.
 *		Just cancels the previous request if notify is IP_NULL.
 *	Conditions:
 *		The port is locked and active.  It is unlocked.
 *		Consumes a ref for notify (if non-null), and
 *		returns previous with a ref (if non-null).
 */

void
ipc_port_pdrequest(port, notify, previousp)
	ipc_port_t port;
	ipc_port_t notify;
	ipc_port_t *previousp;
{
	ipc_port_t previous;

	assert(ip_active(port));

	previous = port->ip_pdrequest;
	port->ip_pdrequest = notify;
	ip_unlock(port);

	*previousp = previous;
}

/*
 *	Routine:	ipc_port_nsrequest
 *	Purpose:
 *		Make a no-senders request, returning the
 *		previously registered send-once right.
 *		Just cancels the previous request if notify is IP_NULL.
 *	Conditions:
 *		The port is locked and active.  It is unlocked.
 *		Consumes a ref for notify (if non-null), and
 *		returns previous with a ref (if non-null).
 */

void
ipc_port_nsrequest(port, sync, notify, previousp)
	ipc_port_t port;
	mach_port_mscount_t sync;
	ipc_port_t notify;
	ipc_port_t *previousp;
{
	ipc_port_t previous;
	mach_port_mscount_t mscount;

	assert(ip_active(port));

	previous = port->ip_nsrequest;
	mscount = port->ip_mscount;

	if ((port->ip_srights == 0) &&
	    (sync <= mscount) &&
	    (notify != IP_NULL)) {
		port->ip_nsrequest = IP_NULL;
		ip_unlock(port);
		ipc_notify_no_senders(notify, mscount);
	} else {
		port->ip_nsrequest = notify;
		ip_unlock(port);
	}

	*previousp = previous;
}

/*
 *	Routine:	ipc_port_set_qlimit
 *	Purpose:
 *		Changes a port's queue limit; the maximum number
 *		of messages which may be queued to the port.
 *	Conditions:
 *		The port is locked and active.
 */

void
ipc_port_set_qlimit(port, qlimit)
	ipc_port_t port;
	mach_port_msgcount_t qlimit;
{
	assert(ip_active(port));
	assert(qlimit <= MACH_PORT_QLIMIT_MAX);

	/* wake up senders allowed by the new qlimit */

	if (qlimit > port->ip_qlimit) {
		mach_port_msgcount_t i, wakeup;

		/* caution: wakeup, qlimit are unsigned */

		wakeup = qlimit - port->ip_qlimit;

		for (i = 0; i < wakeup; i++) {
			ipc_thread_t th;

			th = ipc_thread_dequeue(&port->ip_blocked);
			if (th == ITH_NULL)
				break;

			th->ith_state = MACH_MSG_SUCCESS;
			thread_go(th);
		}
	}

	port->ip_qlimit = qlimit;
}

/*
 *	Routine:	ipc_port_set_seqno
 *	Purpose:
 *		Changes a port's sequence number.
 *	Conditions:
 *		The port is locked and active.
 */

void
ipc_port_set_seqno(port, seqno)
	ipc_port_t port;
	mach_port_seqno_t seqno;
{
	if (port->ip_pset != IPS_NULL) {
		ipc_pset_t pset = port->ip_pset;

		ips_lock(pset);
		if (!ips_active(pset)) {
			ipc_pset_remove(pset, port);
			ips_check_unlock(pset);
			goto no_port_set;
		} else {
			imq_lock(&pset->ips_messages);
			port->ip_seqno = seqno;
			imq_unlock(&pset->ips_messages);
		}
	} else {
	    no_port_set:
		imq_lock(&port->ip_messages);
		port->ip_seqno = seqno;
		imq_unlock(&port->ip_messages);
	}
}

/*
 *	Routine:	ipc_port_clear_receiver
 *	Purpose:
 *		Prepares a receive right for transmission/destruction.
 *	Conditions:
 *		The port is locked and active.
 */

void
ipc_port_clear_receiver(port)
	ipc_port_t port;
{
	ipc_pset_t pset;

	assert(ip_active(port));

	pset = port->ip_pset;
	if (pset != IPS_NULL) {
		/* No threads receiving from port, but must remove from set. */

		ips_lock(pset);
		ipc_pset_remove(pset, port);
		ips_check_unlock(pset);
	} else {
		/* Else, wake up all receivers, indicating why. */

		imq_lock(&port->ip_messages);
		ipc_mqueue_changed(&port->ip_messages, MACH_RCV_PORT_DIED);
		imq_unlock(&port->ip_messages);
	}

	ipc_port_set_mscount(port, 0);
	imq_lock(&port->ip_messages);
	port->ip_seqno = 0;
	imq_unlock(&port->ip_messages);
}

/*
 *	Routine:	ipc_port_init
 *	Purpose:
 *		Initializes a newly-allocated port.
 *		Doesn't touch the ip_object fields.
 */

void
ipc_port_init(port, space, name)
	ipc_port_t port;
	ipc_space_t space;
	mach_port_t name;
{
	/* port->ip_kobject doesn't have to be initialized */

	port->ip_receiver = space;
	port->ip_receiver_name = name;

	port->ip_mscount = 0;
	port->ip_srights = 0;
	port->ip_sorights = 0;

	port->ip_nsrequest = IP_NULL;
	port->ip_pdrequest = IP_NULL;
	port->ip_dnrequests = IPR_NULL;

	port->ip_pset = IPS_NULL;
	port->ip_seqno = 0;
	port->ip_msgcount = 0;
	port->ip_qlimit = MACH_PORT_QLIMIT_DEFAULT;

#if	NORMA2
	port->dipc_uid = 0;
	port->dipc_exported_uid = 0;
	port->dipc_uid_next = IP_NULL;
	port->dipc_is_proxy = FALSE;
	port->dipc_is_special = FALSE;
	port->dipc_forward = FALSE;
	port->dipc_migrate_state = FALSE;
	port->dipc_do_nms = TRUE;
	port->dipc_migrated = FALSE;
	port->dipc_serialize_lock = FALSE;
	port->dipc_serialize_waiters = 0;
	port->dipc_node = 0;
	port->dipc_transit = 0;
	port->dipc_remote_sorights = 0;

	port->dipc_spare_bit_7 = 0;
	port->dipc_spare_bit_8 = 0;
	port->dipc_spare_bit_9 = 0;
	port->dipc_spare_bit_10 = 0;
	port->dipc_spare_bit_11 = 0;
	port->dipc_spare_bit_12 = 0;
	port->dipc_spare_bit_13 = 0;
	port->dipc_spare_bit_14 = 0;
	port->dipc_spare_bit_15 = 0;

	port->dipc_spare_16_bits = 0;

	port->dipc_blocked_senders = 0;
	port->dipc_blocked_sender_count = 0;
	port->dipc_blocked_sender_next = 0;

	port->dipc_ksvr_id = ITH_NULL;
	port->dipc_kobj_active_list = IP_NULL;

	port->dipc_enqueue_in_flight = 0;
	port->dipc_early_queue_avail = 0;
	port->ip_norma_xmm_object_refs = 0;
	port->ip_norma_xmm_object = IP_NULL;
	port->dipc_tracer = dipc_tracer;	/* for debug */

#endif /* NORMA2 */

#if	NORMA_IPC
	port->ip_norma_uid = 0;
	port->ip_norma_dest_node = 0;
	port->ip_norma_stransit = 0;
	port->ip_norma_sotransit = 0;
	port->ip_norma_xmm_object_refs = 0;
	port->ip_norma_is_proxy = FALSE;
	port->ip_norma_is_special = FALSE;
	port->ip_norma_atrium_waiter = FALSE;
	port->ip_norma_forward = FALSE;
	port->ip_norma_sync = FALSE;
	port->ip_norma_is_zombie = FALSE;
	port->ip_norma_kserver_active = FALSE;
	port->ip_norma_suspended = FALSE;
	port->ip_norma_atrium = IP_NULL;
	port->ip_norma_queue_next = port;
	port->ip_norma_xmm_object = IP_NULL;
	port->ip_norma_next = port;
#if	MACH_ASSERT
	port->ip_norma_spare1 = 0L;
	port->ip_norma_spare2 = 0L;
	port->ip_norma_spare3 = 0L;
#endif
#endif	/* NORMA_IPC */
#if	MACH_ASSERT
	/*
	 *	Don't initialize other port debugging
	 *	fields here because they are initialized
	 *	in port_track_alloc.
	 */
#endif
	ipc_mqueue_init(&port->ip_messages);
	ipc_thread_queue_init(&port->ip_blocked);
}

/*
 *	Routine:	ipc_port_alloc
 *	Purpose:
 *		Allocate a port.
 *	Conditions:
 *		Nothing locked.  If successful, the port is returned
 *		locked.  (The caller doesn't have a reference.)
 *	Returns:
 *		KERN_SUCCESS		The port is allocated.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_NO_SPACE		No room for an entry in the space.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
ipc_port_alloc(space, namep, portp)
	ipc_space_t space;
	mach_port_t *namep;
	ipc_port_t *portp;
{
	ipc_port_t port;
	mach_port_t name;
	kern_return_t kr;
	
	kr = ipc_object_alloc(space, IOT_PORT,
			      MACH_PORT_TYPE_RECEIVE, 0,
			      &name, (ipc_object_t *) &port);
	if (kr != KERN_SUCCESS)
		return kr;
	/* port is locked */

	ipc_port_init(port, space, name);

	*namep = name;
	*portp = port;
	return KERN_SUCCESS;
}

/*
 *	Routine:	ipc_port_alloc_name
 *	Purpose:
 *		Allocate a port, with a specific name.
 *	Conditions:
 *		Nothing locked.  If successful, the port is returned
 *		locked.  (The caller doesn't have a reference.)
 *	Returns:
 *		KERN_SUCCESS		The port is allocated.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_NAME_EXISTS	The name already denotes a right.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
ipc_port_alloc_name(space, name, portp)
	ipc_space_t space;
	mach_port_t name;
	ipc_port_t *portp;
{
	ipc_port_t port;
	kern_return_t kr;

	kr = ipc_object_alloc_name(space, IOT_PORT,
				   MACH_PORT_TYPE_RECEIVE, 0,
				   name, (ipc_object_t *) &port);
	if (kr != KERN_SUCCESS)
		return kr;
	/* port is locked */

	ipc_port_init(port, space, name);

	*portp = port;
	return KERN_SUCCESS;
}

#if	MACH_IPC_COMPAT
/*
 *	Routine:	ipc_port_delete_compat
 *	Purpose:
 *		Find and destroy a compat entry for a dead port.
 *		If successful, generate a port-deleted notification.
 *	Conditions:
 *		Nothing locked; the port is dead.
 *		Frees a ref for the space.
 */

void
ipc_port_delete_compat(port, space, name)
	ipc_port_t port;
	ipc_space_t space;
	mach_port_t name;
{
	ipc_entry_t entry;
	kern_return_t kr;

	assert(!ip_active(port));

	kr = ipc_right_lookup_write(space, name, &entry);
	if (kr == KERN_SUCCESS) {
		ipc_port_t sright;

		/* space is write-locked and active */

		if ((ipc_port_t) entry->ie_object == port) {
			assert(entry->ie_bits & IE_BITS_COMPAT);

			sright = ipc_space_make_notify(space);

			kr = ipc_right_destroy(space, name, entry);
			/* space is unlocked */
			assert(kr == KERN_INVALID_NAME);
		} else {
			is_write_unlock(space);
			sright = IP_NULL;
		}

		if (IP_VALID(sright))
			ipc_notify_port_deleted_compat(sright, name);
	}

	is_release(space);
}
#endif	MACH_IPC_COMPAT

/*
 *	Routine:	ipc_port_destroy
 *	Purpose:
 *		Destroys a port.  Cleans up queued messages.
 *
 *		If the port has a backup, it doesn't get destroyed,
 *		but is sent in a port-destroyed notification to the backup.
 *	Conditions:
 *		The port is locked and alive; nothing else locked.
 *		The caller has a reference, which is consumed.
 *		Afterwards, the port is unlocked and dead.
 */

void
ipc_port_destroy(port)
	ipc_port_t port;
{
	ipc_port_t pdrequest, nsrequest;
	ipc_mqueue_t mqueue;
	ipc_kmsg_queue_t kmqueue;
	ipc_kmsg_t kmsg;
	ipc_thread_t sender;
	ipc_port_request_t dnrequests;

	assert(ip_active(port));
	/* port->ip_receiver_name is garbage */
	/* port->ip_receiver/port->ip_destination is garbage */
	assert(port->ip_pset == IPS_NULL);
	assert(port->ip_mscount == 0);
	assert(port->ip_seqno == 0);

	/* first check for a backup port */

	pdrequest = port->ip_pdrequest;
	if (pdrequest != IP_NULL) {
		/* we assume the ref for pdrequest */
		port->ip_pdrequest = IP_NULL;

		/* make port be in limbo */
		port->ip_receiver_name = MACH_PORT_NULL;
		port->ip_destination = IP_NULL;
		ip_unlock(port);

#if	MACH_IPC_COMPAT
		/*
		 *	pdrequest might actually be a send right instead
		 *	of a send-once right, indicated by the low bit
		 *	of the pointer value.  If this is the case,
		 *	we must use ipc_notify_port_destroyed_compat.
		 */

		if (ip_pdsendp(pdrequest)) {
			ipc_port_t sright = ip_pdsend(pdrequest);

			if (!ipc_port_check_circularity(port, sright)) {
				/* consumes our refs for port and sright */
				ipc_notify_port_destroyed_compat(sright, port);
				return;
			} else {
				/* consume sright and destroy port */
				ipc_port_release_send(sright);
			}
		} else
#endif	MACH_IPC_COMPAT

		if (!ipc_port_check_circularity(port, pdrequest)) {
			/* consumes our refs for port and pdrequest */
			ipc_notify_port_destroyed(pdrequest, port);
			return;
		} else {
			/* consume pdrequest and destroy port */
			ipc_port_release_sonce(pdrequest);
		}

		ip_lock(port);
		assert(ip_active(port));
		assert(port->ip_pset == IPS_NULL);
		assert(port->ip_mscount == 0);
		assert(port->ip_seqno == 0);
		assert(port->ip_pdrequest == IP_NULL);
		assert(port->ip_receiver_name == MACH_PORT_NULL);
		assert(port->ip_destination == IP_NULL);

		/* fall through and destroy the port */
	}

	/*
	 *	rouse all blocked senders
	 *
	 *	This must be done with the port locked, because
	 *	ipc_mqueue_send can play with the ip_blocked queue
	 *	of a dead port.
	 */

	while ((sender = ipc_thread_dequeue(&port->ip_blocked)) != ITH_NULL) {
		sender->ith_state = MACH_MSG_SUCCESS;
		thread_go(sender);
	}

	/* once port is dead, we don't need to keep it locked */

	port->ip_object.io_bits &= ~IO_BITS_ACTIVE;
	port->ip_timestamp = ipc_port_timestamp();
#if	NORMA_IPC
	/*
	 *	destroy any NORMA_IPC state associated with port
	 */
	norma_ipc_port_destroy(port);
#endif	NORMA_IPC
	ip_unlock(port);

	/* throw away no-senders request */

	nsrequest = port->ip_nsrequest;
	if (nsrequest != IP_NULL)
		ipc_notify_send_once(nsrequest); /* consumes ref */

	/* destroy any queued messages */

	mqueue = &port->ip_messages;
	imq_lock(mqueue);
	assert(ipc_thread_queue_empty(&mqueue->imq_threads));
	kmqueue = &mqueue->imq_messages;

	while ((kmsg = ipc_kmsg_dequeue(kmqueue)) != IKM_NULL) {

		imq_unlock(mqueue);
#if	NORMA2
		switch( kmsg->ikm_kmsg_type ) {

		  case IKM_KMSG_TYPE_META:
		  case IKM_KMSG_TYPE_NET:
			ip_release(port);
			dipc_kmsg_destroy(kmsg);
			break;
			
		  case IKM_KMSG_TYPE_LOCAL:
#endif	/* NORMA2 */
			assert(kmsg->ikm_header.msgh_remote_port ==
							 (mach_port_t) port);

			ipc_port_release(port);
			kmsg->ikm_header.msgh_remote_port = MACH_PORT_NULL;

#if	NORMA_IPC
			/*
			 *	This message might be in network format
			 */
			norma_ipc_finish_receiving(&kmsg);

			/*
			 *	If ikm_wait_for_send is set, then some thread is
			 *	waiting for this kmsg and will free it itself.
			 *	Mark the kmsg completed and wake that thread.
			 *	Otherwise, free the kmsg here.
			 */
			if (kmsg->ikm_wait_for_send) {
				kmsg->ikm_send_completed = TRUE;
				++c_ipc_port_destroy_wakeup;
				thread_wakeup((int)kmsg);
			}
			else {
				ipc_kmsg_destroy(kmsg);
			}
#else	NORMA_IPC
			ipc_kmsg_destroy(kmsg);
#endif	/* NORMA_IPC */

#if	NORMA2
			break;

		  default:
			panic("ipc_port_destroy: unknow kmsg type 0x%x",
				kmsg->ikm_kmsg_type);
		}
#endif	/* NORMA2 */
		port->ip_msgcount--;
		imq_lock(mqueue);
	}

	imq_unlock(mqueue);

	/* generate dead-name notifications */

	dnrequests = port->ip_dnrequests;
	if (dnrequests != IPR_NULL) {
		ipc_table_size_t its = dnrequests->ipr_size;
		ipc_table_elems_t size = its->its_size;
		ipc_port_request_index_t index;

		for (index = 1; index < size; index++) {
			ipc_port_request_t ipr = &dnrequests[index];
			mach_port_t name = ipr->ipr_name;
			ipc_port_t soright;

			if (name == MACH_PORT_NULL)
				continue;

			soright = ipr->ipr_soright;
			assert(soright != IP_NULL);

#if	MACH_IPC_COMPAT
			if (ipr_spacep(soright)) {
				ipc_port_delete_compat(port,
						ipr_space(soright), name);
				continue;
			}
#endif	MACH_IPC_COMPAT

#if	NORMA_IPC
			if (soright == IP_NORMA_FAKE_DNREQUEST)
				norma_ipc_notify_dead_name(port, name);
			else
#endif	/* NORMA_IPC */
#if	NORMA2
			if (soright == DIPC_IP_DNREQUEST_MAGIC_COOKIE)
				dipc_notify_dead_name(port, name);
			else
#endif	/* NORMA2 */
			ipc_notify_dead_name(soright, name);
		}

		it_dnrequests_free(its, dnrequests);
        	port->ip_dnrequests = IPR_NULL;	/* leave no doubts... */
	}

#if	NORMA2
	/*
	 *	destroy any NORMA2 state associated with the port.
	 */
        if ( DIPC_UID_VALID(port) )
		dipc_port_destroy_state(port);
#endif	/* NORMA2 */

	if (ip_kotype(port) != IKOT_NONE)
		ipc_kobject_destroy(port);

	ipc_port_release(port); /* consume caller's ref */
}

/*
 *	Routine:	ipc_port_check_circularity
 *	Purpose:
 *		Check if queueing "port" in a message for "dest"
 *		would create a circular group of ports and messages.
 *
 *		If no circularity (FALSE returned), then "port"
 *		is changed from "in limbo" to "in transit".
 *
 *		That is, we want to set port->ip_destination == dest,
 *		but guaranteeing that this doesn't create a circle
 *		port->ip_destination->ip_destination->... == port
 *	Conditions:
 *		No ports locked.  References held for "port" and "dest".
 */

boolean_t
ipc_port_check_circularity(port, dest)
	ipc_port_t port, dest;
{
	ipc_port_t base;

	assert(port != IP_NULL);
	assert(dest != IP_NULL);

	if (port == dest)
		return TRUE;
	base = dest;

	/*
	 *	First try a quick check that can run in parallel.
	 *	No circularity if dest is not in transit.
	 */

	ip_lock(port);
	if (ip_lock_try(dest)) {
		if (!ip_active(dest) ||
		    (dest->ip_receiver_name != MACH_PORT_NULL) ||
		    (dest->ip_destination == IP_NULL))
			goto not_circular;

		/* dest is in transit; further checking necessary */

		ip_unlock(dest);
	}
	ip_unlock(port);

	ipc_port_multiple_lock(); /* massive serialization */

	/*
	 *	Search for the end of the chain (a port not in transit),
	 *	acquiring locks along the way.
	 */

	for (;;) {
		ip_lock(base);

		if (!ip_active(base) ||
		    (base->ip_receiver_name != MACH_PORT_NULL) ||
		    (base->ip_destination == IP_NULL))
			break;

		base = base->ip_destination;
	}

	/* all ports in chain from dest to base, inclusive, are locked */

	if (port == base) {
		/* circularity detected! */

		ipc_port_multiple_unlock();

		/* port (== base) is in limbo */

		assert(ip_active(port));
		assert(port->ip_receiver_name == MACH_PORT_NULL);
		assert(port->ip_destination == IP_NULL);

		while (dest != IP_NULL) {
			ipc_port_t next;

			/* dest is in transit or in limbo */

			assert(ip_active(dest));
			assert(dest->ip_receiver_name == MACH_PORT_NULL);

			next = dest->ip_destination;
			ip_unlock(dest);
			dest = next;
		}

		return TRUE;
	}

	/*
	 *	The guarantee:  lock port while the entire chain is locked.
	 *	Once port is locked, we can take a reference to dest,
	 *	add port to the chain, and unlock everything.
	 */

	ip_lock(port);
	ipc_port_multiple_unlock();

    not_circular:

	/* port is in limbo */

	assert(ip_active(port));
	assert(port->ip_receiver_name == MACH_PORT_NULL);
	assert(port->ip_destination == IP_NULL);

	ip_reference(dest);
	port->ip_destination = dest;

	/* now unlock chain */

	while (port != base) {
		ipc_port_t next;

		/* port is in transit */

		assert(ip_active(port));
		assert(port->ip_receiver_name == MACH_PORT_NULL);
		assert(port->ip_destination != IP_NULL);

		next = port->ip_destination;
		ip_unlock(port);
		port = next;
	}

	/* base is not in transit */

	assert(!ip_active(base) ||
	       (base->ip_receiver_name != MACH_PORT_NULL) ||
	       (base->ip_destination == IP_NULL));
	ip_unlock(base);

	return FALSE;
}

/*
 *	Routine:	ipc_port_lookup_notify
 *	Purpose:
 *		Make a send-once notify port from a receive right.
 *		Returns IP_NULL if name doesn't denote a receive right.
 *	Conditions:
 *		The space must be locked (read or write) and active.
 */

ipc_port_t
ipc_port_lookup_notify(space, name)
	ipc_space_t space;
	mach_port_t name;
{
	ipc_port_t port;
	ipc_entry_t entry;

	assert(space->is_active);

	entry = ipc_entry_lookup(space, name);
	if (entry == IE_NULL)
		return IP_NULL;

	if ((entry->ie_bits & MACH_PORT_TYPE_RECEIVE) == 0)
		return IP_NULL;

	port = (ipc_port_t) entry->ie_object;
	assert(port != IP_NULL);

	ip_lock(port);
	assert(ip_active(port));
	assert(port->ip_receiver_name == name);
	assert(port->ip_receiver == space);

	ip_reference(port);
	port->ip_sorights++;
	ip_unlock(port);

	return port;
}

/*
 *	Routine:	ipc_port_make_send
 *	Purpose:
 *		Make a naked send right from a receive right.
 *	Conditions:
 *		The port is not locked but it is active.
 */

ipc_port_t
ipc_port_make_send(port)
	ipc_port_t port;
{
	assert(IP_VALID(port));

	ip_lock(port);
	assert(ip_active(port));
	port->ip_mscount++;
	port->ip_srights++;
	ip_reference(port);
	ip_unlock(port);

	return port;
}

/*
 *	Routine:	ipc_port_copy_send
 *	Purpose:
 *		Make a naked send right from another naked send right.
 *			IP_NULL		-> IP_NULL
 *			IP_DEAD		-> IP_DEAD
 *			dead port	-> IP_DEAD
 *			live port	-> port + ref
 *	Conditions:
 *		Nothing locked except possibly a space.
 */

ipc_port_t
ipc_port_copy_send(port)
	ipc_port_t port;
{
	ipc_port_t sright;

	if (!IP_VALID(port))
		return port;

	ip_lock(port);
	if (ip_active(port)) {
		assert(port->ip_srights > 0);

		ip_reference(port);
		port->ip_srights++;
		sright = port;
	} else
		sright = IP_DEAD;
	ip_unlock(port);

	return sright;
}

/*
 *	Routine:	ipc_port_copyout_send
 *	Purpose:
 *		Copyout a naked send right (possibly null/dead),
 *		or if that fails, destroy the right.
 *	Conditions:
 *		Nothing locked.
 */

mach_port_t
ipc_port_copyout_send(sright, space)
	ipc_port_t sright;
	ipc_space_t space;
{
	mach_port_t name;

	if (IP_VALID(sright)) {
		kern_return_t kr;

		kr = ipc_object_copyout(space, (ipc_object_t) sright,
					MACH_MSG_TYPE_PORT_SEND, TRUE, &name);
		if (kr != KERN_SUCCESS) {
			ipc_port_release_send(sright);

			if (kr == KERN_INVALID_CAPABILITY)
				name = MACH_PORT_DEAD;
			else
				name = MACH_PORT_NULL;
		}
	} else
		name = (mach_port_t) sright;

	return name;
}

/*
 *	Routine:	ipc_port_release_send
 *	Purpose:
 *		Release a (valid) naked send right.
 *		Consumes a ref for the port.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_port_release_send(port)
	ipc_port_t port;
{
	ipc_port_t nsrequest = IP_NULL;
	mach_port_mscount_t mscount;

	assert(IP_VALID(port));

	ip_lock(port);
	ip_release(port);

	if (!ip_active(port)) {
		ip_check_unlock(port);
		return;
	}

	assert(port->ip_srights > 0);

	if (--port->ip_srights == 0) {
#if	NORMA_IPC
		assert((port->ip_norma_stransit == 0) ||
			(IP_NORMA_IS_PROXY(port)));
#endif	
		nsrequest = port->ip_nsrequest;
		if (nsrequest != IP_NULL) {
			port->ip_nsrequest = IP_NULL;
			mscount = port->ip_mscount;
		}
	}

	ip_unlock(port);

	if (nsrequest != IP_NULL)
		ipc_notify_no_senders(nsrequest, mscount);
}

/*
 *	Routine:	ipc_port_make_sonce
 *	Purpose:
 *		Make a naked send-once right from a receive right.
 *	Conditions:
 *		The port is not locked but it is active.
 */

ipc_port_t
ipc_port_make_sonce(port)
	ipc_port_t port;
{
	assert(IP_VALID(port));

	ip_lock(port);
	assert(ip_active(port));
	port->ip_sorights++;
	ip_reference(port);
	ip_unlock(port);

	return port;
}

/*
 *	Routine:	ipc_port_release_sonce
 *	Purpose:
 *		Release a naked send-once right.
 *		Consumes a ref for the port.
 *
 *		In normal situations, this is never used.
 *		Send-once rights are only consumed when
 *		a message (possibly a send-once notification)
 *		is sent to them.
 *	Conditions:
 *		Nothing locked except possibly a space.
 */

void
ipc_port_release_sonce(port)
	ipc_port_t port;
{
	assert(IP_VALID(port));

	ip_lock(port);

	assert(port->ip_sorights > 0);

	port->ip_sorights--;

#if 	NORMA2
	if (DIPC_IS_PROXY(port)) {
		dipc_port_remove_try(port);
	}
#endif	/* NORMA2 */

#if	NORMA_IPC
	if (IP_NORMA_IS_PROXY(port)) {
		port->ip_norma_sotransit--;
		norma_port_remove_try(port);
	}
#endif	NORMA_IPC

	ip_release(port);

	if (!ip_active(port)) {
		ip_check_unlock(port);
		return;
	}

	ip_unlock(port);
}

/*
 *	Routine:	ipc_port_release_receive
 *	Purpose:
 *		Release a naked (in limbo or in transit) receive right.
 *		Consumes a ref for the port; destroys the port.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_port_release_receive(port)
	ipc_port_t port;
{
	ipc_port_t dest;

	assert(IP_VALID(port));

	ip_lock(port);
	assert(ip_active(port));
	assert(port->ip_receiver_name == MACH_PORT_NULL);
	dest = port->ip_destination;

	ipc_port_destroy(port); /* consumes ref, unlocks */

	if (dest != IP_NULL)
		ipc_port_release(dest);
}

/*
 *	Routine:	ipc_port_alloc_special
 *	Purpose:
 *		Allocate a port in a special space.
 *		The new port is returned with one ref.
 *		If unsuccessful, IP_NULL is returned.
 *	Conditions:
 *		Nothing locked.
 */

ipc_port_t
ipc_port_alloc_special(space)
	ipc_space_t space;
{
#if	NORMA_IPC
#if	i386
	int ret = (&ret)[2];	/* where we were called from */
#else
	int ret = (int) ipc_port_alloc_special;
#endif
	extern int input_msgh_id;
#endif	NORMA_IPC
	ipc_port_t port;

	port = (ipc_port_t) io_alloc(IOT_PORT);
	if (port == IP_NULL)
		return IP_NULL;

	io_lock_init(&port->ip_object);
	port->ip_references = 1;
	port->ip_object.io_bits = io_makebits(TRUE, IOT_PORT, 0);

	/*
	 *	The actual values of ip_receiver_name aren't important,
	 *	as long as they are valid (not null/dead).
	 */

	ipc_port_init(port, space, 1);

#if	NORMA_IPC && MACH_ASSERT
	port->ip_norma_spare1 = ret;
	port->ip_norma_spare2 = input_msgh_id;
#endif	/* NORMA_IPC && MACH_ASSERT */
	return port;
}

/*
 *	Routine:	ipc_port_dealloc_special
 *	Purpose:
 *		Deallocate a port in a special space.
 *		Consumes one ref for the port.
 *	Conditions:
 *		Nothing locked.
 */

void
ipc_port_dealloc_special(port, space)
	ipc_port_t port;
	ipc_space_t space;
{
	ip_lock(port);
	assert(ip_active(port));
	assert(port->ip_receiver_name != MACH_PORT_NULL);
	assert(port->ip_receiver == space);

	/*
	 *	We clear ip_receiver_name and ip_receiver to simplify
	 *	the ipc_space_kernel check in ipc_mqueue_send.
	 */

	port->ip_receiver_name = MACH_PORT_NULL;
	port->ip_receiver = IS_NULL;

	/* relevant part of ipc_port_clear_receiver */
	ipc_port_set_mscount(port, 0);
	port->ip_seqno = 0;

	ipc_port_destroy(port);
}

#if	MACH_IPC_COMPAT

/*
 *	Routine:	ipc_port_alloc_compat
 *	Purpose:
 *		Allocate a port.
 *	Conditions:
 *		Nothing locked.  If successful, the port is returned
 *		locked.  (The caller doesn't have a reference.)
 *
 *		Like ipc_port_alloc, except that the new entry
 *		is IE_BITS_COMPAT.
 *	Returns:
 *		KERN_SUCCESS		The port is allocated.
 *		KERN_INVALID_TASK	The space is dead.
 *		KERN_NO_SPACE		No room for an entry in the space.
 *		KERN_RESOURCE_SHORTAGE	Couldn't allocate memory.
 */

kern_return_t
ipc_port_alloc_compat(space, namep, portp)
	ipc_space_t space;
	mach_port_t *namep;
	ipc_port_t *portp;
{
	ipc_port_t port;
	ipc_entry_t entry;
	mach_port_t name;
	ipc_table_size_t its;
	ipc_port_request_t table;
	ipc_table_elems_t size;
	ipc_port_request_index_t free, i;
	kern_return_t kr;

	port = ip_alloc();
	if (port == IP_NULL)
		return KERN_RESOURCE_SHORTAGE;

	its = &ipc_table_dnrequests[0];
	table = it_dnrequests_alloc(its);
	if (table == IPR_NULL) {
		ip_free(port);
		return KERN_RESOURCE_SHORTAGE;
	}

	kr = ipc_entry_alloc(space, &name, &entry);
	if (kr != KERN_SUCCESS) {
		ip_free(port);
		it_dnrequests_free(its, table);
		return kr;
	}
	/* space is write-locked */

	entry->ie_object = (ipc_object_t) port;
	entry->ie_request = 1;
	entry->ie_bits |= IE_BITS_COMPAT|MACH_PORT_TYPE_RECEIVE;

	ip_lock_init(port);
	ip_lock(port);
	is_write_unlock(space);

	port->ip_references = 1; /* for entry, not caller */
	port->ip_bits = io_makebits(TRUE, IOT_PORT, 0);

	ipc_port_init(port, space, name);

	size = its->its_size;
	assert(size > 1);
	free = 0;

	for (i = 2; i < size; i++) {
		ipc_port_request_t ipr = &table[i];

		ipr->ipr_name = MACH_PORT_NULL;
		ipr->ipr_next = free;
		free = i;
	}

	table->ipr_next = free;
	table->ipr_size = its;
	port->ip_dnrequests = table;

	table[1].ipr_name = name;
	table[1].ipr_soright = ipr_spacem(space);
	is_reference(space);

	*namep = name;
	*portp = port;
	return KERN_SUCCESS;
}

/*
 *	Routine:	ipc_port_copyout_send_compat
 *	Purpose:
 *		Copyout a naked send right (possibly null/dead),
 *		or if that fails, destroy the right.
 *		Like ipc_port_copyout_send, except that if a
 *		new translation is created it has the compat bit.
 *	Conditions:
 *		Nothing locked.
 */

mach_port_t
ipc_port_copyout_send_compat(sright, space)
	ipc_port_t sright;
	ipc_space_t space;
{
	mach_port_t name;

	if (IP_VALID(sright)) {
		kern_return_t kr;

		kr = ipc_object_copyout_compat(space, (ipc_object_t) sright,
					       MACH_MSG_TYPE_PORT_SEND, &name);
		if (kr != KERN_SUCCESS) {
			ipc_port_release_send(sright);
			name = MACH_PORT_NULL;
		}
	} else
		name = (mach_port_t) sright;

	return name;
}

/*
 *	Routine:	ipc_port_copyout_receiver
 *	Purpose:
 *		Copyout a port reference (possibly null)
 *		by giving the caller his name for the port,
 *		if he is the receiver.
 *	Conditions:
 *		Nothing locked.  Consumes a ref for the port.
 */

mach_port_t
ipc_port_copyout_receiver(port, space)
	ipc_port_t port;
	ipc_space_t space;
{
	mach_port_t name;

	if (!IP_VALID(port))
		return MACH_PORT_NULL;

	ip_lock(port);
	if (port->ip_receiver == space) {
		name = port->ip_receiver_name;
		assert(MACH_PORT_VALID(name));
	} else
		name = MACH_PORT_NULL;

	ip_release(port);
	ip_check_unlock(port);

	return name;
}

#endif	MACH_IPC_COMPAT


#if	MACH_ASSERT
#include <ddb/db_sym.h>
#include <ddb/db_access.h>

#define	FUNC_NULL	((void (*)) 0)
#define	MAX_REFS	5		/* bins for tracking ref counts */

queue_head_t	port_alloc_queue;
decl_simple_lock_data(,port_alloc_queue_lock)


typedef unsigned long	u_long;

typedef struct port_item {
	u_long		item;
	u_long		count;
} port_item;


#define	ITEM_MAX	400
typedef struct port_track {
	char		*name;
	u_long		max;
	u_long		warning;
	port_item	items[ITEM_MAX];
} port_track;

port_track	port_callers;		/* match against calling addresses */
port_track	port_threads;		/* match against allocating threads */
port_track	port_spaces;		/* match against ipc spaces */

u_long		port_count = 0;
u_long		port_count_warning = 20000;
u_long		port_timestamp = 0;


void
port_track_alloc(object)
ipc_object_t	object;
{
	ipc_port_t	port;
	unsigned int	i;

	port = (ipc_port_t) object;
	port->ip_thread = (u_long) current_thread();
	port->ip_timetrack = port_timestamp++;
	for (i = 0; i < IP_CALLSTACK_MAX; ++i)
		port->ip_callstack[i] = 0;
	for (i = 0; i < IP_MISC_MAX; ++i)
		port->ip_misc[i] = 0;
#if	PARAGON860
	/* XXX fix this interface XXX alanl */
	i860_callstack(&port->ip_callstack[0], IP_CALLSTACK_MAX);
#endif
	simple_lock(&port_alloc_queue_lock);
	assert(port_count >= 0);
	++port_count;
	if (port_count_warning > 0 && port_count >= port_count_warning)
		assert(port_count < port_count_warning);
	queue_enter(&port_alloc_queue, port, ipc_port_t, ip_port_links);
	simple_unlock(&port_alloc_queue_lock);
}


void
port_track_free(object)
ipc_object_t	object;
{
	ipc_port_t	port;

	port = (ipc_port_t) object;
	simple_lock(&port_alloc_queue_lock);
	assert(port_count > 0);
	--port_count;
	queue_remove(&port_alloc_queue, port, ipc_port_t, ip_port_links);
	simple_unlock(&port_alloc_queue_lock);
}


void
port_track_init(trackp, name)
port_track	*trackp;
char		*name;
{
	port_item	*i;

	trackp->max = trackp->warning = 0;
	trackp->name = name;
	for (i = trackp->items; i < trackp->items + ITEM_MAX; ++i)
		i->item = i->count = 0;
}


void
port_item_add(trackp, item)
port_track	*trackp;
u_long		item;
{
	port_item	*limit, *i;

	limit = trackp->items + trackp->max;
	for (i = trackp->items; i < limit; ++i)
		if (i->item == item) {
			i->count++;
			return;
		}
	if (trackp->max >= ITEM_MAX) {
		if (trackp->warning++ == 0)
			iprintf("%s:  no room\n", trackp->name);
		return;
	}
	i->item = item;
	i->count = 1;
	trackp->max++;
}


/*
 *	Simple (and slow) bubble sort.
 */
void
port_track_sort(trackp)
port_track	*trackp;
{
	port_item	*limit, *p;
	port_item	temp;
	boolean_t	unsorted;

	limit = trackp->items + trackp->max - 1;
	do {
		unsorted = FALSE;
		for (p = trackp->items; p < limit - 1; ++p) {
			if (p->count < (p+1)->count) {
				temp = *p;
				*p = *(p+1);
				*(p+1) = temp;
				unsorted = TRUE;
			}
		}
	} while (unsorted == TRUE);
}


void
port_track_print(trackp, func)
port_track	*trackp;
void		(*func)();
{
	port_item	*limit, *p;

	limit = trackp->items + trackp->max;
	iprintf("%s:\n", trackp->name);
	for (p = trackp->items; p < limit; ++p) {
		if (func != FUNC_NULL)
			(*func)(p);
		else
			iprintf("0x%x\t%8d\n", p->item, p->count);
	}
}


void
port_callers_print(p)
port_item	*p;
{
	db_printf("0x%x\t%8d\t", p->item, p->count);
	db_printsym(p->item, DB_STGY_PROC);
	db_printf("\n");
}


/*
 *	Translate port's cache of call stack pointers
 *	into symbolic names.
 */
void
port_trace(port)
ipc_port_t	port;
{
	unsigned int	i;

	for (i = 0; i < IP_CALLSTACK_MAX; ++i) {
		db_printf("[%d] 0x%x\t", i, port->ip_callstack[i]);
		if (port->ip_callstack[i] != 0 &&
		    DB_VALID_KERN_ADDR(port->ip_callstack[i]))
			db_printsym(port->ip_callstack[i], DB_STGY_PROC);
		db_printf("\n");
	}
}

db_ref( int refs )
{
	db_port_walk(1, 1, 1, refs, 0);
	return 0;
}

/*
 *	Examine all currently allocated ports.
 *	Options:
 *		verbose		display suspicious ports
 *		display		print out each port encountered
 *		ref_search	restrict examination to ports with
 *				a specified reference count
 *		ref_target	reference count for ref_search
 */
int
db_port_walk(verbose, display, ref_search, ref_target, tracer)
unsigned int	verbose;
unsigned int	display;
unsigned int	ref_search;
unsigned int	ref_target;
unsigned short	tracer;
{
	ipc_port_t	port;
	unsigned int	ref_overflow, refs, i, ref_inactive_overflow;
	unsigned int	no_receiver, no_match;
	unsigned int	ref_counts[MAX_REFS];
	unsigned int	inactive[MAX_REFS];
#if	NORMA_IPC
	unsigned int	norma_principals = 0;
	unsigned int	norma_proxies = 0;
#endif
#if	NORMA2
	unsigned int	dipc_principals = 0;
	unsigned int	dipc_proxies = 0;
#endif
	unsigned int	ipc_ports = 0;

	iprintf("Allocated port count is %d\n", port_count);
	no_receiver = no_match = ref_overflow = 0;
	ref_inactive_overflow = 0;
	for (i = 0; i < MAX_REFS; ++i) {
		ref_counts[i] = 0;
		inactive[i] = 0;
	}
	port_track_init(&port_callers, "port callers");
	port_track_init(&port_threads, "port threads");
	port_track_init(&port_spaces, "port spaces");
	if (ref_search) {
		if ( tracer )
			iprintf("Walking ports of ref_count=%d, tracer 0x%x.\n",
				 ref_target,tracer);
		else
			iprintf("Walking ports of ref_count=%d.\n", ref_target);
	}
	else {
		if ( tracer )
			iprintf("Walking all ports, tracer 0x%x.\n",tracer);
		else
			iprintf("Walking all ports.\n");
	}

	queue_iterate(&port_alloc_queue, port, ipc_port_t, ip_port_links) {
		char	*port_type;

#if NORMA2
		if ( tracer && (tracer != port->dipc_tracer) )
			continue;
#endif	/* NORMA2 */

#if	NORMA_IPC
		if (port->ip_norma_uid)
			if (IP_NORMA_IS_PROXY(port)) {
				port_type = "    NORMA PROXY";
				if (ip_active(port))
					norma_proxies++;
			} else {
				port_type = "NORMA PRINCIPAL";
				if (ip_active(port))
					norma_principals++;
			}
		else
#endif	/* NORMA_IPC */
#if	NORMA2
		if (port->dipc_uid)
			if (DIPC_IS_PROXY(port)) {
				port_type = "     DIPC PROXY";
				if (ip_active(port))
					dipc_proxies++;
			} else {
				port_type = " DIPC PRINCIPAL";
				if (ip_active(port))
					dipc_principals++;
			}
		else
#endif	/* NORMA2 */
			{
			port_type = "       IPC port";
			if (ip_active(port))
				ipc_ports++;
			}

		refs = port->ip_references;
		if (ref_search && refs != ref_target)
			continue;

		if (refs >= MAX_REFS) {
			if (ip_active(port))
				++ref_overflow;
			else
				++ref_inactive_overflow;
		} else if (refs < 0)
			iprintf("%s 0x%x has ref count %d!\n",
				port_type, port, port->ip_object.io_references);
		else {
			if (refs == 0 && verbose)
				iprintf("%s 0x%x has ref count of zero!\n",
					port_type, port);
			if (ip_active(port))
				ref_counts[refs]++;
			else
				inactive[refs]++;
		}
		port_item_add(&port_threads, (u_long) port->ip_thread);
		for (i = 0; i < IP_CALLSTACK_MAX; ++i) {
			if (port->ip_callstack[i] != 0 &&
			    DB_VALID_KERN_ADDR(port->ip_callstack[i]))
				port_item_add(&port_callers,
					      port->ip_callstack[i]);
		}
		if (!ip_active(port)) {
			if (verbose)
				iprintf("%s 0x%x, inactive, refcnt %d\n",
					port_type, port, refs);
			continue;
		}

		if (port->ip_receiver_name == MACH_PORT_NULL) {
			iprintf("%s  0x%x, no receiver, refcnt %d\n",
				port, refs);
			++no_receiver;
			continue;
		}
		if (port->ip_receiver == ipc_space_kernel ||
		    port->ip_receiver == ipc_space_reply ||
		    port->ip_receiver == ipc_space_remote ||
		    ipc_entry_lookup(port->ip_receiver,
				     port->ip_receiver_name) != IE_NULL) {
			port_item_add(&port_spaces, (u_long)port->ip_receiver);
			if (display) {
				iprintf( "%s 0x%x time 0x%x ref_cnt %d\n",
						port_type, port,
						port->ip_timetrack, refs);
			}
			continue;
		}
		iprintf("%s 0x%x, rcvr 0x%x, name 0x%x, ref %d, no match\n",
				port_type, port, port->ip_receiver,
				port->ip_receiver_name, refs);
		++no_match;
	}
	iprintf("Active port type summary:\n");
	iprintf("\t             IPC %6d \n", ipc_ports);
#if	NORMA_IPC
	iprintf("\tNORMA principals %6d NORMA proxies %6d\n",
					norma_principals, norma_proxies);
#endif
#if	NORMA2
	iprintf("\tDIPC  principals %6d DIPC  proxies %6d\n",
					dipc_principals, dipc_proxies);
#endif
	iprintf("summary:\tcallers %d threads %d spaces %d\n",
		port_callers.max, port_threads.max, port_spaces.max);

	/* XXX fix these printfs to use MAX_REFS */
	iprintf("\tref_counts:  [0]=%d [1]=%d [2]=%d [3]=%d [4]=%d [5+]=%d\n",
		ref_counts[0], ref_counts[1], ref_counts[2],
		ref_counts[3], ref_counts[4], ref_overflow);

	iprintf("\t%d ports w/o receivers, %d w/o matches\n",
		no_receiver, no_match);

	iprintf("\tinactives:");
	if ( ref_inactive_overflow || inactive[0] || inactive[1] ||
	     inactive[2] || inactive[3] || inactive[4] )
		printf(" [0]=%d [1]=%d [2]=%d [3]=%d [4]=%d [5+]=%d\n",
			inactive[0], inactive[1], inactive[2],
			inactive[3], inactive[4], ref_inactive_overflow);
	else
		printf(" No inactive ports.\n");

	port_track_sort(&port_spaces);
	port_track_print(&port_spaces, FUNC_NULL);
	port_track_sort(&port_threads);
	port_track_print(&port_threads, FUNC_NULL);
	port_track_sort(&port_callers);
	port_track_print(&port_callers, port_callers_print);
	return 0;
}

void
port_debug_init()
{
	queue_init(&port_alloc_queue);
	simple_lock_init(&port_alloc_queue_lock);
}
#endif	/* MACH_ASSERT */


#if	MACH_KDB
#define	printf	kdbprintf

/*
 *	Routine:	ipc_port_print
 *	Purpose:
 *		Pretty-print a port for kdb.
 */

void
ipc_port_print(port)
	ipc_port_t port;
{
	char	*empty="";
	extern int indent;

	printf("port 0x%x\n", port);

	indent += 2;

	ipc_object_print(&port->ip_object);
#if	NORMA2
	{
		char	*s = (char *)0;

		if( port->ip_receiver == ipc_space_kernel )
			s = "[ipc_space_kernel]";
		else if( port->ip_receiver == ipc_space_reply )
			s = "[ipc_space_reply]";
		else if( port->ip_receiver == ipc_space_remote )
			s = "[ipc_space_remote]";

		iprintf("receiver=");
		if ( s )
			printf("%s", s);
		else
			printf("0x%x", port->ip_receiver);
		printf(", receiver_name=0x%x\n", port->ip_receiver_name);
	}
#else
	iprintf("receiver=0x%x", port->ip_receiver);
	printf(", receiver_name=0x%x\n", port->ip_receiver_name);
#endif

	iprintf("mscount=%d", port->ip_mscount);
	printf(", srights=%d", port->ip_srights);
	printf(", sorights=%d\n", port->ip_sorights);

	iprintf("nsrequest=0x%x", port->ip_nsrequest);
	printf(", pdrequest=0x%x", port->ip_pdrequest);
	printf(", dnrequests=0x%x\n", port->ip_dnrequests);

	iprintf("pset=0x%x", port->ip_pset);
	printf(", seqno=%d", port->ip_seqno);
	printf(", msgcount=%d", port->ip_msgcount);
	printf(", qlimit=%d\n", port->ip_qlimit);

	iprintf("kmsgs=0x%x", port->ip_messages.imq_messages.ikmq_base);
	printf(", rcvrs=0x%x", port->ip_messages.imq_threads.ithq_base);
	printf(", sndrs=0x%x", port->ip_blocked.ithq_base);
	printf(", kobj=0x%x\n", port->ip_kobject);

#if	NORMA_IPC
	if ( port->ip_norma_is_proxy || port->ip_norma_uid  ||
	     port->ip_norma_is_special )
	{
		iprintf("\nNorma [%s%s%s] uid 0x%x\n",
			(port->ip_norma_is_proxy ?
				"Proxy" : "Principal"),
			(port->ip_norma_is_special ?
				" special" : ""),
			(port->ip_norma_forward ?
				" forwarding" : ""),
			port->ip_norma_uid);

		iprintf("dest_node=%d", port->ip_norma_dest_node);
		printf(", stransit=%d", port->ip_norma_stransit);
		printf(", xorefs=%d", port->ip_norma_xmm_object_refs);
		printf(", sync=%d", port->ip_norma_sync);
		printf(", sotransit=%d\n", port->ip_norma_sotransit);

		iprintf("is_zombie=%d", port->ip_norma_is_zombie);
		printf(", kserver_active=%d", port->ip_norma_kserver_active);
		printf(", norma_atrium=0x%x\n", port->ip_norma_atrium);

		iprintf("queue_next=0x%x", port->ip_norma_queue_next);
		printf(", xmm_object=0x%x", port->ip_norma_xmm_object);
		printf(", next=0x%x\n", port->ip_norma_next);

#if	MACH_ASSERT
		iprintf("norma_spare1=0x%x", port->ip_norma_spare1);
		printf(", norma_spare2=0x%x", port->ip_norma_spare2);
		printf(", norma_spare3=0x%x\n", port->ip_norma_spare3);
#endif
	}
#endif	/* NORMA_IPC */

#if	NORMA2
	iprintf("\nDIPC: [%s%s%s%s%s] uid 0x%x (0x%x) uid_next 0x%x\n", 
			(port->dipc_is_proxy	? "Proxy" : "Principal"),
			(port->dipc_is_special	? " special" : empty),
			(port->dipc_forward	? " forwarding" : empty),
			(port->dipc_migrate_state ? " migration-in-progress"
						: empty),
			(port->dipc_migrated	? " migrated" : empty),
			port->dipc_uid,
			port->dipc_exported_uid,
			port->dipc_uid_next);

	iprintf("node %d", port->dipc_node);
	printf(", transits %d", port->dipc_transit);
	printf(", remote_sorights %d", port->dipc_remote_sorights);
	printf(", xorefs %d", port->ip_norma_xmm_object_refs);
	printf(", xmm_obj 0x%x\n", port->ip_norma_xmm_object);

	iprintf("Remote Kobj Service [thread=0x%x, next=0x%x]\n",
			port->dipc_ksvr_id,
			port->dipc_kobj_active_list);

	iprintf("Remote Enqueue [flying=%d, early=%d]\n",
			port->dipc_enqueue_in_flight,
			port->dipc_early_queue_avail);

	iprintf("Remote Blocked Senders[0x%x, count=%d, next=%d]\n",
			port->dipc_blocked_senders,
			port->dipc_blocked_sender_count,
			port->dipc_blocked_sender_next);

	iprintf("NMS-notification [%s]\n", 
			(port->dipc_do_nms ? "default" : "on-request"));

	iprintf("dipc_serialize_lock=%d dipc_serialize_waiters=%d\n",
			port->dipc_serialize_lock,
			port->dipc_serialize_waiters);
#endif	/* NORMA2 */

#if	MACH_ASSERT
	iprintf("\nall_ports\tnext 0x%x prev 0x%x\n",
		port->ip_port_links.next, port->ip_port_links.prev);
	iprintf("thread 0x%x timestamp 0x%x\n", port->ip_thread,
		port->ip_timetrack);
	/* fix these printfs to use IP_CALLSTACK_MAX and IP_MISC_MAX XXX */
	iprintf("cl[0]=0x%x [1]=0x%x [2]=0x%x [3]=0x%x [4]=0x%x\n",
		port->ip_callstack[0], port->ip_callstack[1],
		port->ip_callstack[2], port->ip_callstack[3],
		port->ip_callstack[4]);
	iprintf("cl[5]=0x%x [6]=0x%x [7]=0x%x [8]=0x%x [9]=0x%x\n",
		port->ip_callstack[5], port->ip_callstack[6],
		port->ip_callstack[7], port->ip_callstack[8],
		port->ip_callstack[9]);
	iprintf("mi[0]=0x%x [1]=0x%x [2]=0x%x [3]=0x%x [4]=0x%x\n",
		port->ip_misc[0], port->ip_misc[1],
		port->ip_misc[2], port->ip_misc[3],
		port->ip_misc[4]);
	iprintf("mi[5]=0x%x [6]=0x%x [7]=0x%x [8]=0x%x [9]=0x%x\n",
		port->ip_misc[5], port->ip_misc[6],
		port->ip_misc[7], port->ip_misc[8],
		port->ip_misc[9]);
#endif	/* MACH_ASSERT */
	indent -=2;
}

#if	MACH_ASSERT

db_find_rcvr(thread)
	ipc_thread_t	thread;
{
	ipc_port_t		port;
	ipc_thread_queue_t	queue;
	ipc_thread_t		th, first;

	queue_iterate(&port_alloc_queue, port, ipc_port_t, ip_port_links) {
		if (port->ip_pset)
			queue = &port->ip_pset->ips_messages.imq_threads;
		else
			queue = &port->ip_messages.imq_threads;

		first = ipc_thread_queue_first(queue);
		if (first == ITH_NULL)
			continue;
		th = first;
		do {
			if (th == thread) {
				if (port->ip_pset)
					db_printf("pset=%x ", port->ip_pset);
				db_printf("port=%x\n", port);
			}
			th = th->ith_next;
		} while (th != first);
	}

	return 0;
}

#endif	MACH_ASSERT

#endif	MACH_KDB
