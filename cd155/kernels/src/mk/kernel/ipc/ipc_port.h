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
 * HISTORY
 * $Log: ipc_port.h,v $
 * Revision 1.12  1994/11/18  20:49:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/08/31  21:24:37  mtm
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
 * Revision 1.9  1994/07/16  00:41:20  stans
 *  Fixed ip_reference() & ip_release() macros to reflect new norma logging
 *  calling conventions.
 *
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #: debug
 *  Testing: developer
 *
 * Revision 1.8  1994/07/13  01:22:18  andyp
 * Late changes from the NORMA2 branch.
 *
 * Revision 1.7  1994/07/12  19:22:19  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.6.4.16  1994/07/13  00:48:03  rkl
 *  Renamed some fields in ipc_port_t
 *
 * Revision 1.6.4.15  1994/06/22  00:42:03  rkl
 *  Added verbose ip_reference logging through VERBOSE_REF_LOGGING bootmagic.
 *
 * Revision 1.6.4.14  1994/06/20  17:13:10  rkl
 *  Removed BORG capability.
 *
 * Revision 1.6.4.13  1994/05/25  16:42:13  stans
 *   Short to save principle's (soon to be proxy) queue limit.
 *
 * Revision 1.6.4.12  1994/05/25  06:54:26  andyp
 * Added fields for remote kobj server support.  Removed the overloading
 * of the remote enqueue status with the blocked senders fields.
 * Added some comments and debug support.
 *
 * Revision 1.6.4.11  1994/05/11  01:22:34  stans
 *   Name change: kserver_id --> dipc_ksvr_id
 *
 * Revision 1.6.4.10  1994/05/05  02:09:07  stans
 *   kserver_id, exported and has migrated.
 *
 * Revision 1.6.4.9  1994/04/20  00:30:42  stans
 *   Added dipc_do_nms boolean for doing no-more senders.
 *   NORMA2 + NORMA_IPC
 *   Reduce filename output in reference logging.
 *
 * Revision 1.6.4.8  1994/04/14  16:02:57  rkl
 *  Added port refererence count logging.
 *
 * Revision 1.6.4.7  1994/04/13  20:04:32  andyp
 * Added DIPC deadnames notification.  Added the option of deallocating
 * sent messages from an ast rather than waking a thread.
 *
 * Revision 1.6.4.6  1994/04/05  23:51:00  stans
 *   dipc_migrate_state reduced to a single bit instead of three.
 *
 * Revision 1.6.4.5  1994/04/05  23:29:50  stans
 * !NORMA_IPC
 *
 * Revision 1.6.4.4  1994/03/15  23:26:29  andyp
 * Added fields for NORMA2 remote blocked senders.
 *
 * Revision 1.6.4.3  1994/03/10  02:55:35  stans
 *   Corrected spelling on dipc_migrate_state
 *
 * Revision 1.6.4.2  1994/03/02  19:32:21  stans
 * added DIPC_IS_NORMA2()
 *
 * Revision 1.6.4.1  1994/03/02  18:38:00  rkl
 *  Added more fields for NORMA2 in ipc_port.  Added DIPC_IS_PROXY() macro.
 *
 * Revision 1.6  1993/09/28  18:25:18  andyp
 * Update for 1.2 release.
 *
 *
 *	Add additional state to the port structure purely for
 *	debugging purposes.  Each port gets linked into a global
 *	list when allocated from the port zone and removed from
 *	the list when freed.  At creation time, we also record
 *	the identity of the creator (thread context) and the
 *	kernel function call stack.  Leave some room for additional
 *	debugging variables without rebuilding the world.
 *	[alanl@osf.org, andyp@ssd.intel.com]
 *
 * Revision 1.5  1993/06/30  22:42:48  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:37:01  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:33:28  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:33:13  dleslie
 * First R1_0 release
 *
 * Revision 2.12.2.4  92/05/28  18:14:17  jeffreyh
 * 	Change value of IP_NORMA_FAKE_DNREQUEST to 
 * 	not get confused with MACH_IPC_COMPAT
 * 
 * Revision 2.12.2.3  92/05/26  18:54:02  jeffreyh
 * 	Add dead name proxy definitions
 * 	[92/05/25            dlb]
 * 
 * 	Added ip_norma_sync to allow thread synchronization on ports.
 * 	[92/05/19            sjs]
 * 
 * 	Added norma_ip_forward to indicate a proxy cannot be thrown away
 * 	until the root node has been updated to point at the new node.
 * 	[92/05/18            sjs]
 * 
 * Revision 2.12.2.2.1.1  92/05/06  17:47:13  jeffreyh
 * 	Add ip_norma_atrium_waiter for atrium message syncronization.
 * 	[92/05/05            dlb]
 * 
 * Revision 2.12.2.2  92/02/21  11:23:37  jsb
 * 	Removed ip_norma_queued. Added ip_norma_spare[1234].
 * 	[92/02/18  07:44:11  jsb]
 * 
 * 	Added ip_norma_xmm_object_refs.
 * 	[92/02/16  16:12:50  jsb]
 * 
 * 	Added ip_norma_xmm_object.
 * 	[92/02/09  14:42:47  jsb]
 * 
 * Revision 2.12.2.1  92/01/03  16:35:50  jsb
 * 	Renamed IP_NORMA_REQUEST macros to ip_nsproxy{,m,p}.
 * 	They now look like Rich's ip_pdrequest macros.
 * 	[91/12/30  07:53:29  jsb]
 * 
 * 	Added IP_NORMA_NSREQUEST macros for no-senders notification support.
 * 	[91/12/28  17:03:42  jsb]
 * 
 * 	Added ip_norma_{queued,queue_next} for new norma_ipc_send
 * 	implementation that maintains a list of ports with unsent
 * 	remote messages. (The old implementation kept a single
 * 	list of unsent messages for all ports.)
 * 	[91/12/28  08:44:40  jsb]
 * 
 * 	Added ip_norma_atrium.
 * 	[91/12/26  20:03:40  jsb]
 * 
 * 	Added ip_norma_sotransit. Removed ip_norma_{wanted,migrating}.
 * 	Made ip_norma_dest_node unsigned.
 * 	[91/12/25  16:44:29  jsb]
 * 
 * 	NORMA_IPC: removed unused fields from struct ipc_port. Corrected log.
 * 	[91/12/24  14:16:33  jsb]
 * 
 * Revision 2.12  91/12/14  14:28:26  jsb
 * 	NORMA_IPC: replaced dummy port struct fields with real names.
 * 
 * Revision 2.11  91/11/14  16:56:20  rpd
 * 	Added ipc_fields.h hack, with fields in struct ipc_port to match.
 *	Added IP_NORMA_IS_PROXY macro.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.10  91/10/09  16:10:01  af
 * 	Added (unconditional) ipc_port_print declaration.
 * 	[91/09/02            rpd]
 * 
 * Revision 2.9  91/08/28  11:13:50  jsb
 * 	Added ip_seqno and ipc_port_set_seqno.
 * 	[91/08/09            rpd]
 * 	Renamed clport (now ip_norma) fields in struct ipc_port.
 * 	[91/08/14  19:31:55  jsb]
 * 
 * Revision 2.8  91/08/03  18:18:37  jsb
 * 	Fixed include. Added clport fields directly to struct ipc_port.
 * 	[91/07/17  14:06:25  jsb]
 * 
 * Revision 2.7  91/06/17  15:46:26  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:44:06  jsb]
 * 
 * Revision 2.6  91/05/14  16:35:34  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:23:10  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:50:04  mrt]
 * 
 * Revision 2.4  90/11/05  14:29:39  rpd
 * 	Added ipc_port_reference, ipc_port_release.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.3  90/09/28  16:55:18  jsb
 * 	Added NORMA_IPC support.
 * 	[90/09/28  14:03:58  jsb]
 * 
 * Revision 2.2  90/06/02  14:51:13  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:01:25  rpd]
 * 
 */
/*
 *	File:	ipc/ipc_port.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Definitions for ports.
 */

#ifndef	_IPC_IPC_PORT_H_
#define _IPC_IPC_PORT_H_

#include <mach_ipc_compat.h>
#include <norma_ipc.h>
#include <mach_assert.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <kern/lock.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_table.h>
#include <ipc/ipc_thread.h>
#if	MACH_ASSERT
#include <kern/queue.h>
#endif

/*
 *  A receive right (port) can be in four states:
 *	1) dead (not active, ip_timestamp has death time)
 *	2) in a space (ip_receiver_name != 0, ip_receiver points
 *	to the space but doesn't hold a ref for it)
 *	3) in transit (ip_receiver_name == 0, ip_destination points
 *	to the destination port and holds a ref for it)
 *	4) in limbo (ip_receiver_name == 0, ip_destination == IP_NULL)
 *
 *  If the port is active, and ip_receiver points to some space,
 *  then ip_receiver_name != 0, and that space holds receive rights.
 *  If the port is not active, then ip_timestamp contains a timestamp
 *  taken when the port was destroyed.
 */

typedef unsigned int ipc_port_timestamp_t;

#if	MACH_ASSERT 
#define	IP_CALLSTACK_MAX	10
#define	IP_MISC_MAX		20
#endif

typedef struct ipc_port {
	struct ipc_object ip_object;

	union {
		struct ipc_space *receiver;
		struct ipc_port *destination;
		ipc_port_timestamp_t timestamp;
	} data;
	mach_port_t ip_receiver_name;

	ipc_kobject_t ip_kobject;

	mach_port_mscount_t ip_mscount;
	mach_port_rights_t ip_srights;
	mach_port_rights_t ip_sorights;

	struct ipc_port *ip_nsrequest;
	struct ipc_port *ip_pdrequest;
	struct ipc_port_request *ip_dnrequests;

	struct ipc_pset *ip_pset;
	mach_port_seqno_t ip_seqno;		/* locked by message queue */
	unsigned short ip_msgcount;		/* mach_port_msgcount_t */
	unsigned short ip_qlimit;		/* mach_port_msgcount_t */
	struct ipc_mqueue ip_messages;
	struct ipc_thread_queue ip_blocked;

#if	NORMA2
	unsigned long	dipc_uid;
	unsigned long	dipc_exported_uid;	/* audit trail after death */
	struct ipc_port	*dipc_uid_next;
	unsigned long	dipc_is_proxy:		1,
			dipc_is_special:	1,
			dipc_forward:		1,
			dipc_migrate_state:	1,
			dipc_do_nms:		1,
			dipc_migrated:		1,
			dipc_serialize_lock:	1,

			dipc_spare_bit_7:	1,
			dipc_spare_bit_8:	1,
			dipc_spare_bit_9:	1,
			dipc_spare_bit_10:	1,
			dipc_spare_bit_11:	1,
			dipc_spare_bit_12:	1,
			dipc_spare_bit_13:	1,
			dipc_spare_bit_14:	1,
			dipc_spare_bit_15:	1,

			dipc_spare_16_bits:	16;

	unsigned long	dipc_serialize_waiters;
	unsigned short	dipc_qlimit,
			dipc_tracer;
	unsigned long	dipc_node;
	long		dipc_transit;
	long		dipc_remote_sorights;

	/*
	 *	Remote ipc_kobject_server() support
	 */
	struct ipc_port	*dipc_kobj_active_list;
	ipc_thread_t	dipc_ksvr_id;			/* %%% for debug */

	/*
	 *	Remote blocked senders
	 */
	unsigned long	*dipc_blocked_senders;		/* ptr to bit vector */
	unsigned long	dipc_blocked_sender_next;	/* next node to try */
	int		dipc_blocked_sender_count;	/* no. blocked sends */

	/*
	 *	Remote enqueue
	 */
	unsigned long	dipc_enqueue_in_flight;
	unsigned long	dipc_early_queue_avail;

	long		ip_norma_xmm_object_refs;
	struct ipc_port	*ip_norma_xmm_object;

#endif	/* NORMA2 */

#if	NORMA_IPC
	unsigned long ip_norma_uid;
	unsigned long ip_norma_dest_node;
	long ip_norma_stransit;
	long ip_norma_sotransit;
	long ip_norma_xmm_object_refs;
	unsigned int
	/* boolean_t */	ip_norma_is_proxy: 1,	   /* proxy for remote port */
			ip_norma_is_special: 1,	   /* norma special port */
			ip_norma_atrium_waiter: 1, /* sync for migration */
			ip_norma_forward: 1,       /* proxy is placeholder */
			ip_norma_sync: 1,          /* thread synchronize */
			ip_norma_is_zombie: 1,	   /* port to be destroyed */
			ip_norma_kserver_active: 1,/* kserver msg in process */
			ip_norma_suspended: 1,     /* port is suspended */
	  		:0;			   /* (align to boundary) */
	struct ipc_port *ip_norma_atrium;
	struct ipc_port *ip_norma_queue_next;
	struct ipc_port *ip_norma_xmm_object;
	struct ipc_port *ip_norma_next;
#if	MACH_ASSERT
	long ip_norma_spare1;
	long ip_norma_spare2;
	long ip_norma_spare3;
#endif
#endif	/* NORMA_IPC */
#if	MACH_ASSERT
	queue_chain_t	ip_port_links;	/* all allocated ports */
	unsigned long	ip_thread;	/* who made me?  thread context */
	unsigned long	ip_timetrack;	/* give an idea of "when" created */
	unsigned long	ip_callstack[IP_CALLSTACK_MAX]; /* stack trace */
	unsigned long	ip_misc[IP_MISC_MAX]; /* handy debugging spares */
#endif
} *ipc_port_t;

#define	ip_references		ip_object.io_references
#define	ip_bits			ip_object.io_bits
#define	ip_receiver		data.receiver
#define	ip_destination		data.destination
#define	ip_timestamp		data.timestamp

#define	IP_NULL			((ipc_port_t) IO_NULL)
#define	IP_DEAD			((ipc_port_t) IO_DEAD)

#define	IP_VALID(port)		IO_VALID(&(port)->ip_object)

#define	ip_active(port)		io_active(&(port)->ip_object)
#define	ip_lock_init(port)	io_lock_init(&(port)->ip_object)
#define	ip_lock(port)		io_lock(&(port)->ip_object)
#define	ip_lock_try(port)	io_lock_try(&(port)->ip_object)
#define	ip_unlock(port)		io_unlock(&(port)->ip_object)
#define	ip_check_unlock(port)	io_check_unlock(&(port)->ip_object)

#if	MACH_ASSERT && NORMA2
#define	ip_reference(port)						\
	{								\
	extern	void		(*norma_log)();				\
	extern	boolean_t	dipc_reference_logging;			\
	extern	boolean_t	log_all_refs;				\
	io_reference(&(port)->ip_object);				\
	if (dipc_reference_logging && (port->dipc_uid || log_all_refs))	\
		(*norma_log)("ip_reference: %x refs=%d +%d %s\n",	\
				port, port->ip_references, 		\
				__LINE__, &__FILE__[19]);		\
	}
#define	ip_release(port)						\
	{								\
	extern	void		(*norma_log)();				\
	extern	boolean_t	dipc_reference_logging;			\
	extern	boolean_t	log_all_refs;				\
	io_release(&(port)->ip_object);					\
	if (dipc_reference_logging && (port->dipc_uid || log_all_refs))	\
		(*norma_log)("ip_release: %x refs=%d +%d %s\n",	\
				port, port->ip_references,		\
				__LINE__, &__FILE__[19]);		\
	}

#else	/* MACH_ASSERT && NORMA2 */

#define	ip_reference(port)	io_reference(&(port)->ip_object)
#define	ip_release(port)	io_release(&(port)->ip_object)

#endif	/* MACH_ASSERT && NORMA2 */

#define	ip_alloc()		((ipc_port_t) io_alloc(IOT_PORT))
#define	ip_free(port)		io_free(IOT_PORT, &(port)->ip_object)

#define	ip_kotype(port)		io_kotype(&(port)->ip_object)

typedef ipc_table_index_t ipc_port_request_index_t;

typedef struct ipc_port_request {
	union {
		struct ipc_port *port;
		ipc_port_request_index_t index;
	} notify;

	union {
		mach_port_t name;
		struct ipc_table_size *size;
	} name;
} *ipc_port_request_t;

#define	ipr_next		notify.index
#define	ipr_size		name.size

#define	ipr_soright		notify.port
#define	ipr_name		name.name

#define	IPR_NULL		((ipc_port_request_t) 0)

#if	MACH_IPC_COMPAT
/*
 *	For backwards compatibility, the ip_pdrequest field can hold a
 *	send right instead of a send-once right.  This is indicated by
 *	the low bit of the pointer.  This works because the zone package
 *	guarantees that the two low bits of port pointers are zero.
 */

#define	ip_pdsendp(soright)	((unsigned int)(soright) & 1)
#define ip_pdsend(soright)	((ipc_port_t)((unsigned int)(soright) &~ 1))
#define	ip_pdsendm(sright)	((ipc_port_t)((unsigned int)(sright) | 1))

/*
 *	For backwards compatibility, the ipr_soright field can hold
 *	a space pointer.  This is indicated by the low bit of the pointer.
 *	This works because the zone package guarantees that the two low
 *	bits of port and space pointers are zero.
 */

#define	ipr_spacep(soright)	((unsigned int)(soright) & 1)
#define ipr_space(soright)	((ipc_space_t)((unsigned int)(soright) &~ 1))
#define	ipr_spacem(space)	((ipc_port_t)((unsigned int)(space) | 1))
#endif	MACH_IPC_COMPAT

/*
 *	Taking the ipc_port_multiple lock grants the privilege
 *	to lock multiple ports at once.  No ports must locked
 *	when it is taken.
 */

decl_simple_lock_data(extern, ipc_port_multiple_lock_data)

#define	ipc_port_multiple_lock_init()					\
		simple_lock_init(&ipc_port_multiple_lock_data)

#define	ipc_port_multiple_lock()					\
		simple_lock(&ipc_port_multiple_lock_data)

#define	ipc_port_multiple_unlock()					\
		simple_unlock(&ipc_port_multiple_lock_data)

/*
 *	The port timestamp facility provides timestamps
 *	for port destruction.  It is used to serialize
 *	mach_port_names with port death.
 */

decl_simple_lock_data(extern, ipc_port_timestamp_lock_data)
extern ipc_port_timestamp_t ipc_port_timestamp_data;

#define	ipc_port_timestamp_lock_init()					\
		simple_lock_init(&ipc_port_timestamp_lock_data)

#define	ipc_port_timestamp_lock()					\
		simple_lock(&ipc_port_timestamp_lock_data)

#define	ipc_port_timestamp_unlock()					\
		simple_unlock(&ipc_port_timestamp_lock_data)

extern ipc_port_timestamp_t
ipc_port_timestamp();

/*
 *	Compares two timestamps, and returns TRUE if one
 *	happened before two.  Note that this formulation
 *	works when the timestamp wraps around at 2^32,
 *	as long as one and two aren't too far apart.
 */

#define	IP_TIMESTAMP_ORDER(one, two)	((int) ((one) - (two)) < 0)

#define	ipc_port_translate_receive(space, name, portp)			\
		ipc_object_translate((space), (name),			\
				     MACH_PORT_RIGHT_RECEIVE,		\
				     (ipc_object_t *) (portp))

#define	ipc_port_translate_send(space, name, portp)			\
		ipc_object_translate((space), (name),			\
				     MACH_PORT_RIGHT_SEND,		\
				     (ipc_object_t *) (portp))

extern kern_return_t
ipc_port_dnrequest(/* ipc_port_t, mach_port_t, ipc_port_t,
		      ipc_port_request_index_t * */);

extern kern_return_t
ipc_port_dngrow(/* ipc_port_t */);

extern ipc_port_t
ipc_port_dncancel(/* ipc_port_t, mach_port_t, ipc_port_request_index_t */);

#define	ipc_port_dnrename(port, index, oname, nname)			\
MACRO_BEGIN								\
	ipc_port_request_t ipr, table;					\
									\
	assert(ip_active(port));					\
									\
	table = port->ip_dnrequests;					\
	assert(table != IPR_NULL);					\
									\
	ipr = &table[index];						\
	assert(ipr->ipr_name == oname);					\
									\
	ipr->ipr_name = nname;						\
MACRO_END

extern void
ipc_port_pdrequest(/* ipc_port_t, ipc_port_t, ipc_port_t * */);

extern void
ipc_port_nsrequest(/* ipc_port_t, mach_port_mscount_t,
		      ipc_port_t, ipc_port_t * */);

extern void
ipc_port_set_qlimit(/* ipc_port_t, mach_port_msgcount_t */);

#define	ipc_port_set_mscount(port, mscount)				\
MACRO_BEGIN								\
	assert(ip_active(port));					\
									\
	(port)->ip_mscount = (mscount);					\
MACRO_END

extern void
ipc_port_set_seqno(/* ipc_port_t, mach_port_seqno_t */);

extern void
ipc_port_clear_receiver(/* ipc_port_t */);

extern void
ipc_port_init(/* ipc_port_t, ipc_space_t, mach_port_t */);

extern kern_return_t
ipc_port_alloc(/* ipc_space_t, mach_port_t *, ipc_port_t * */);

extern kern_return_t
ipc_port_alloc_name(/* ipc_space_t, mach_port_t, ipc_port_t * */);

extern void
ipc_port_destroy(/* ipc_port_t */);

extern boolean_t
ipc_port_check_circularity(/* ipc_port_t, ipc_port_t */);

extern ipc_port_t
ipc_port_lookup_notify(/* ipc_space_t, mach_port_t */);

extern ipc_port_t
ipc_port_make_send(/* ipc_port_t */);

extern ipc_port_t
ipc_port_copy_send(/* ipc_port_t */);

extern mach_port_t
ipc_port_copyout_send(/* ipc_port_t, ipc_space_t */);

extern void
ipc_port_release_send(/* ipc_port_t */);

extern ipc_port_t
ipc_port_make_sonce(/* ipc_port_t */);

extern void
ipc_port_release_sonce(/* ipc_port_t */);

extern void
ipc_port_release_receive(/* ipc_port_t */);

extern ipc_port_t
ipc_port_alloc_special(/* ipc_space_t */);

extern void
ipc_port_dealloc_special(/* ipc_port_t */);

#define	ipc_port_alloc_kernel()		\
		ipc_port_alloc_special(ipc_space_kernel)
#define	ipc_port_dealloc_kernel(port)	\
		ipc_port_dealloc_special((port), ipc_space_kernel)

#define	ipc_port_alloc_reply()		\
		ipc_port_alloc_special(ipc_space_reply)
#define	ipc_port_dealloc_reply(port)	\
		ipc_port_dealloc_special((port), ipc_space_reply)

#define	ipc_port_reference(port)	\
		ipc_object_reference(&(port)->ip_object)

#define	ipc_port_release(port)		\
		ipc_object_release(&(port)->ip_object)

#if	MACH_IPC_COMPAT

extern kern_return_t
ipc_port_alloc_compat(/* ipc_space_t, mach_port_t *, ipc_port_t * */);

extern mach_port_t
ipc_port_copyout_send_compat(/* ipc_port_t, ipc_space_t */);

extern mach_port_t
ipc_port_copyout_receiver(/* ipc_port_t, ipc_space_t */);

#if	NORMA2

#define	DIPC_IS_PROXY(port)	((port)->dipc_is_proxy)

#endif	/* NORMA2 */

#endif	MACH_IPC_COMPAT

extern void
ipc_port_print(/* ipc_port_t */);

#if	NORMA_IPC || NORMA2

#if	NORMA_IPC
#define	IP_NORMA_IS_PROXY(port)	((port)->ip_norma_is_proxy)
#endif

/*
 *	A proxy never has a real nsrequest, but is always has a fake
 *	nsrequest so that the norma ipc system is notified when there
 *	are no send rights for a proxy. A fake nsrequest is indicated by
 *	the low bit of the pointer.  This works because the zone package
 *	guarantees that the two low bits of port pointers are zero.
 */

#define	ip_nsproxyp(nsrequest)	((unsigned int)(nsrequest) & 1)
#define ip_nsproxy(nsrequest)	((ipc_port_t)((unsigned int)(nsrequest) &~ 1))
#define	ip_nsproxym(proxy)	((ipc_port_t)((unsigned int)(proxy) | 1))

#if	NORMA2
#define	DIPC_IP_DNREQUEST_MAGIC_COOKIE	((ipc_port_t) 0xdeadface)
#define DIPC_DNREQUEST_NODEOF(name)	((name) - 0x10000)
#define DIPC_DNREQUEST_NAMEOF(node)	((node) + 0x10000)
#endif	/* NORMA2 */

/*
 *	Dead name requests have to stay on the node that they are originally
 * 	made on (the right entry all but contains a pointer to the request
 *	structure).  If they are attached to a proxy port, the proxy port
 *	attaches a fake dnrequest to the real port.  When the real receive
 *	right is destroyed, the fake dnrequest invokes the norma system.
 *	The norma system notifies the remote node to destroy the proxy,
 *	setting off the real dead name notifications.  Fake dn requests
 *	are indicated by the magic constant (note that it's not aligned,
 *	hence can't be a port).  The use of 2 as the least significant
 *	digit prevents this from getting confused with a real dead name
 *	request (ports are 4-byte aligned).  Can't use 1 for this purpose
 *	because MACH_IPC_COMPAT is already using it (ipr_spacep).
 *
 *	Due to our use of existing dnrequest data structures, 0 can't
 *	be used as the name of a node (it looks like a cancelled dnrequest).
 *	The remapping here is somewhat arbitrary.
 */

#define	IP_NORMA_FAKE_DNREQUEST	(ipc_port_t)0x22222222

#define NODE_TO_DNREQUEST_NAME(node)	((node) + 0x10000)
#define DNREQUEST_NAME_TO_NODE(node)	((node) - 0x10000)

#define	port_wellness_check(p)	(assert ((p->ip_refcount - p->ip_srights - \
				 p->ip_sorights - p->ip_msgcount -	\
				 (norma_port_tabled(p) ? 1 : 0))) >= 0)
#endif	/* NORMA_IPC || NORMA2 */

#endif	_IPC_IPC_PORT_H_
