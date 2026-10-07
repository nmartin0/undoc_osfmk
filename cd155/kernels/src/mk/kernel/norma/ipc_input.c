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
 * $Log: ipc_input.c,v $
 * Revision 1.17  1994/11/18  20:55:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.16  1994/05/03  22:20:42  tnt
 *  Same as revision 1.15.  Corrected typo.
 *
 * Revision 1.15  1994/05/03  18:51:08  tnt
 * Reviewers: Stan Smith, Len Brown
 * Risk: Low
 * Benefit or PTS #: 9257; Unprotected ast_on()/ast_off() macros in the norma
 *                   code were clearing AST bits set from interrupt level
 *                   handlers.  They are now protected by spl calls.
 * Testing: Message Passing EATs; pfs, mxm SATs
 * Modules: ipc_input.c
 *
 * Revision 1.14  1994/03/19  01:28:44  lenb
 * Benefit: FORCE_MASTER cleanup
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 1.13  1994/01/12  20:36:57  rkl
 *  Put in a *working* algorithm to determine if the overallocated FOOL page
 *  should be returned.
 *
 *  Reviewer: andyp
 *  Risk: low
 *  Benefit or PTS #: 7743
 *  Testing: Custom test with varing offest and transfer sizes
 *  Module(s): norma/ipc_input.c norma/ipc_unreliable.c
 *
 * Revision 1.10.2.2  1994/01/03  22:05:10  rkl
 *  Fixed botched logic for page allocation when OOL transfer starts on a
 *  non-zero offset.  We now always over allocate on the initial request and
 *  return the extra page if it's not needed.
 *
 *  Reviewer: Terry Prickett
 *  Risk: Low
 *  Benefit or PTS #: 7550 and 7613
 *  Testing: Ran SLAB SAT and gprof EAT
 *  Module(s): kernel/norma/ipc_input.c
 *
 * Revision 1.10.2.1  1993/12/15  01:23:27  joel
 *  Reviewer: andyp,cfleck
 *  Risk: low
 *  Benefit or PTS #: 7502
 *  Testing: booted and checked new default with kernel debugger
 *  Module(s): ipc_input.c model_dep.c
 *  R1.2 RTI approved
 *
 * Revision 1.10  1993/11/18  23:48:29  rkl
 *  Reviewer: Joel
 *  Risk: Moderate
 *  Benefit or PTS #: 7198
 *  Testing: PFS SATS (at least to the point were the problem occured)
 *  Module(s): ipc_input.c ipc_unreliable.c
 *
 *  Fixed NORMA fast OOL bug that reserved the wrong number of pages before
 *  the transfer.  It did not take into account non-aligned pages in the
 *  vm_page_list.
 *
 * Revision 1.9  1993/10/20  16:01:41  rkl
 * Added NORMA Fast OOL capability.  When a single OOL type that is larger
 * than a VM page and <= a page list is sent, it will attempt to use the
 * Fast OOL transfer method.  This method allows all pages except for the
 * last to be sent without waiting for an ACK from the receiver.  If the
 * receiver does not have the resources to support the transfer, it will
 * be done in the regular way.
 *
 * Revision 1.8  1993/09/28  18:02:53  andyp
 * Update for the 1.2 release.
 *
 *
 *	Track length of port queues.  [alanl@osf.org]
 *
 *	Pass an additional parameter to netipc_page_put identifying
 *	the caller, for debugging purposes.  [alanl@osf.org]
 *
 * Revision 1.7  1993/09/27  15:59:29  stans
 * MACH_ASSERT NORMA mumble() debug call
 *
 * Revision 1.6  1993/06/30  22:50:19  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/06/25  22:28:33  andyp
 * Merged up to the latest from OSF's tree.  Recovered OSF's log messages
 * where needed.  Added the fixes for CPU idle times.  Added some Paragon
 * instrumentation for billing time spent in ipc_wait.c:norma_ipc_kmsg_accept()
 * as idle time.
 *
 * Revision 1.4  1993/06/09  01:40:09  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:45:36  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2  1993/04/12  17:21:42  SSD
 * page flow control fixes.
 *
 * Revision 1.1.10.3  1993/04/27  00:19:17  dleslie
 * Patch release of April 23
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_input.c,v 
 * Revision 1.2.5.3  1993/04/06  17:43:30  mmp
 * 	 The use of the norma_ipc_handoff_mqueue optimization could
 * 	 lead to out-of-order message delivery:  a message not able
 * 	 to take the optimized path would be queued up and lose out
 * 	 to a following message that could take advantage of the
 * 	 optimization.  Fix by serializing message delivery through
 * 	 a common path.  (alanl)
 * 	[1993/04/06  17:42:55  mmp]
 *
 * Revision 1.2.5.2  1993/03/03  21:20:10  dwm
 * 	Enable fastpath delivery (alanl).
 * 	[1993/03/03  21:16:29  dwm]
 * 
 * Revision 1.2  1992/11/25  01:13:52  robert
 * 	integrate changes below for norma_14
 * 	[1992/11/13  19:37:56  robert]
 * 
 * 	fix history
 * 	[1992/11/09  21:38:54  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:44:25  robert]
 * 
 * Revision 0.0  92/10/21            alan
 * 		Update tr's.
 * 	[92/10/21            alan]
 * 
 * Revision 0.0  92/10/14            alan
 * 		Fix netipc_ast to traverse norma_kmsg_incomplete_queue
 * 		correctly even when deleting elements from the queue.
 * 	[92/10/14            alan]
 * 
 * Revision 0.0  92/09/29            dlb
 * 		Delete bad assert in netipc_ast.
 * 	[92/09/29            dlb]
 * 
 * Revision 0.0  92/09/27            dlb
 * 		For slow delivery, ack of last page in message must be delayed until
 * 		actual delivery.  Premature ack can violate message ordering.
 * 	[92/09/27            dlb]
 * 
 * Revision 0.0  92/09/11            jsb
 * 		Added copy_length parameter to copy_to_kalloc to ensure that we kalloc
 * 		the same amount of memory as we will be freeing later.
 * 
 * 	Revision 1.1  1992/11/05  20:59:28  robert
 * 	Initial revision
 * 	[92/09/11            jsb]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 *
 * Revision 2.11.2.10  92/09/16  14:56:17  jeffreyh
 * 	Disable fast-path page delivery until better tested.
 * 
 * Revision 2.11.2.9  92/09/15  17:33:26  jeffreyh
 * 	Converted logging from ntr to tr.  Moved tr routines
 * 	to new file.
 * 	[92/09/10            alanl]
 * 
 * 	Enable fast-path page delivery (really original delivery
 * 	path), limited to a single page-list of pages.  Longer
 * 	messages will be delivered through object-flavor copy
 * 	objects.  Fix page-tracking assertions in norma_deliver_page
 * 	to handle fast-path and use only one bit instead of four.
 * 	[92/09/09            alanl]
 * 
 * 	It is possible for a port to become dead between the time
 * 	the initial kmsg is received and the time subsequent pages
 * 	belonging to the kmsg are received.  The assertion in
 * 	norma_deliver_page must be smarter to handle this case.
 * 	[92/09/09            alanl]
 * 	Use pageable copy objects for large NORMA messages.
 * 	Small messages continue to be handled using page list
 * 	technology at interrupt level -- in theory, minimizing
 * 	latency.  Pages belonging to messages larger than a
 * 	cutoff limit are sent to the netipc_thread for reassembly,
 * 	using a pageable, object flavor copy object.  Introducing:
 * 	netipc_slow_pages, associated queue, zone and thread.  A
 * 	mode bit causes norma_deliver_page to handle both large and
 * 	small message cases.  The function netipc_input_init handles
 * 	initialization for input-side data structures.  Revamped
 * 	norma_kmsg_incomplete_queue and added netipc_assembly_wrappers.
 * 	With jeffreyh:  added page tracing code to verify that
 * 	all pages sent arrive in order and completely on the
 * 	remote node.  Added ntr(), norma trace routine, that
 * 	records recent events in a circular buffer.  And more
 * 	counters and debug code than you can shake a stick at.
 * 	[92/06/02            alanl]
 * 
 * Revision 2.11.2.8  92/06/24  18:00:34  jeffreyh
 * 	Changed norma_remote_to_kmsg to match on the norma uid as well as
 * 	the node.
 * 	[92/06/10            jeffreyh]
 * 
 * 	Rewrite bogus incomplete kmsg scanning code in netipc_ast().
 * 	[92/06/08            dlb]
 * 
 * Revision 2.11.2.7  92/05/27  00:57:50  jeffreyh
 * 	Interface to netipc_page_put changes to indicate
 * 	whether page being put was borrowed from reserved
 * 	VM pool.
 * 	[92/05/07            alanl]
 * 	Move code that receives destination out of interrupt level.
 * 	It's in a new routine, norma_kmsg_receive_dest_and_ack that
 * 	is only called at AST level.  This results in some acks being
 * 	delayed until an AST can occur; this is necessary, and should
 * 	be compensated for by enlarging protocol windows.
 * 	[92/05/05            dlb]
 * 
 * 	Clear MIGRATED bit when queueing migrated message on atrium.
 * 	Do a wakeup if someone is waiting for the arrival of messages
 * 	on this atrium.
 * 	[92/04/29            dlb]
 * 	Fixed race condition in norma_handoff_kmsg().
 * 	[92/04/16            sjs]
 * 
 * Revision 2.11.2.6  92/04/08  15:45:25  jeffreyh
 * 	Temporary debugging logic.
 * 	[92/04/06            dlb]
 * 
 * 	Don't call norma_ipc_error_receiving from here.  That cleanup is
 * 	the sender's responsibility.
 * 	[92/04/06            dlb]
 * 
 * Revision 2.11.2.5  92/03/28  10:11:26  jeffreyh
 * 	In norma_deliver_kmsg(), if there's an error receiving,
 * 	let norma_ipc_receive_*_dest routine decide whether to
 * 	clear port field, or leave it to be garbage collected.
 * 	Remove IKM_SIZE_NORMA setting; caller does that.
 * 	[92/03/25            dlb]
 * 	In norma_handoff_message we must check for migrating kmsgs before
 * 	kmsgs heading for kernel objects, else the norma uid may get
 * 	dereferenced as a port structure.
 * 	[92/03/20  14:12:33  jsb]
 * 
 * 	If delivery fails (can't get destination), pass kmsg
 * 	to norma_ipc_error_receiving() for cleanup.
 * 	[92/03/20  14:12:33  dlb]
 * 	In norma_handoff_message we must check for migrating kmsgs before
 * 	kmsgs heading for kernel objects, else the norma uid may get
 * 	dereferenced as a port structure.
 * 	[92/03/20            jsb]
 * 
 * Revision 2.11.2.4  92/02/21  11:24:15  jsb
 * 	Store msgh_id in global variable in norma_ipc_finish_receiving,
 * 	for debugging purposes.
 * 	[92/02/20  10:32:54  jsb]
 * 
 * Revision 2.11.2.3  92/01/21  21:51:09  jsb
 * 	Removed global_msgh_id.
 * 	[92/01/17  14:35:50  jsb]
 * 
 * 	More de-linting.
 * 	[92/01/17  11:39:24  jsb]
 * 
 * 	More de-linting.
 * 	[92/01/16  22:10:41  jsb]
 * 
 * 	De-linted.
 * 	[92/01/13  10:15:12  jsb]
 * 
 * 	Fix from dlb to increment receiver->ip_seqno in thread_go case.
 * 	[92/01/11  17:41:46  jsb]
 * 
 * 	Moved netipc_ack status demultiplexing here.
 * 	[92/01/11  17:08:19  jsb]
 * 
 * Revision 2.11.2.2  92/01/09  18:45:18  jsb
 * 	Turned off copy object continuation debugging.
 * 	[92/01/09  15:37:46  jsb]
 * 
 * 	Added support for copy object continuations.
 * 	[92/01/09  13:18:50  jsb]
 * 
 * 	Replaced spls with netipc_thread_{lock,unlock}.
 * 	[92/01/08  10:14:14  jsb]
 * 
 * 	Made out-of-line ports work.
 * 	[92/01/05  17:51:28  jsb]
 * 
 * 	Parameter copy_npages replaced by page_last in norma_deliver_page.
 * 	Removed continuation panic since continuations are coming soon.
 * 	[92/01/05  15:58:34  jsb]
 * 
 * Revision 2.11.2.1  92/01/03  16:37:18  jsb
 * 	Replaced norma_ipc_ack_failure with norma_ipc_ack_{dead,not_found}.
 * 	[91/12/29  16:01:41  jsb]
 * 
 * 	Added type parameter to norma_ipc_receive_migrating_dest.
 * 	Added debugging code to remember msgh_id when creating proxies.
 * 	[91/12/28  18:07:18  jsb]
 * 
 * 	Pass remote node via kmsg->ikm_source_node to norma_ipc_receive_port
 * 	on its way to norma_ipc_receive_rright. Now that we have a real
 * 	ikm_source_node kmsg field, we can get rid of the ikm_remote hack.
 * 	[91/12/27  21:37:36  jsb]
 * 
 * 	Removed unused msgid (not msgh_id) parameters.
 * 	[91/12/27  17:08:39  jsb]
 * 
 * 	Queue migrated messages on atrium port.
 * 	[91/12/26  20:37:49  jsb]
 * 
 * 	Moved translation of local port to norma_receive_complex_ports.
 * 	Moved norma_receive_complex_ports call to norma_ipc_finish_receiving.
 * 	Added code for MACH_MSGH_BITS_MIGRATED, including call to new routine
 * 	norma_ipc_receive_migrating_dest. 
 * 	[91/12/25  16:54:50  jsb]
 * 
 * 	Made large kmsgs work correctly. Corrected log.
 * 	Added check for null local port in norma_deliver_kmsg.
 * 	[91/12/24  14:33:18  jsb]
 * 
 * Revision 2.11  91/12/15  17:31:06  jsb
 * 	Almost made large kmsgs work... now it leaks but does not crash.
 * 	Changed debugging printfs.
 * 
 * Revision 2.10  91/12/15  10:47:09  jsb
 * 	Added norma_ipc_finish_receiving to support large in-line msgs.
 * 	Small clean-up of norma_deliver_page.
 * 
 * Revision 2.9  91/12/14  14:34:11  jsb
 * 	Removed private assert definition.
 * 
 * Revision 2.8  91/12/13  13:55:01  jsb
 * 	Fixed check for end of last copy object in norma_deliver_page.
 * 	Moved norma_ipc_ack_xxx calls to safer places.
 * 
 * Revision 2.7  91/12/10  13:26:00  jsb
 * 	Added support for moving receive rights.
 * 	Use norma_ipc_ack_* upcalls (downcalls?) instead of return values
 * 	from norma_deliver_kmsg and _page.
 * 	Merged dlb check for continuation-needing copy objects in
 * 	norma_deliver_page.
 * 	Added (untested) support for multiple copy objects per message.
 * 	[91/12/10  11:26:32  jsb]
 * 
 * Revision 2.6  91/11/19  09:40:50  rvb
 *	Added new_remote argument to norma_deliver_kmsg to support
 *	migrating receive rights.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.5  91/11/14  16:51:39  rpd
 * 	Replaced norma_ipc_get_proxy with norma_ipc_receive_{port,dest}.
 *	Added check that destination port can accept message.
 * 	Added checks on type of received rights.
 * 	[91/09/19  13:51:21  jsb]
 * 
 * Revision 2.4  91/08/28  11:16:00  jsb
 * 	Mark received pages as dirty and not busy.
 * 	Initialize copy->cpy_cont and copy->cpy_cont_args.
 * 	[91/08/16  10:44:19  jsb]
 * 
 * 	Fixed reference to norma_ipc_kobject_send.
 * 	[91/08/15  08:42:23  jsb]
 * 
 * 	Renamed clport things to norma things.
 * 	[91/08/14  21:34:13  jsb]
 * 
 * 	Fixed norma_ipc_handoff code.
 * 	Added splon/sploff redefinition hack.
 * 	[91/08/14  19:11:07  jsb]
 * 
 * Revision 2.3  91/08/03  18:19:19  jsb
 * 	Use MACH_MSGH_BITS_COMPLEX_DATA instead of null msgid to determine
 * 	whether data follows kmsg.
 * 	[91/08/01  21:57:37  jsb]
 * 
 * 	Eliminated remaining old-style page list code.
 * 	Cleaned up and corrected clport_deliver_page.
 * 	[91/07/27  18:47:08  jsb]
 * 
 * 	Moved MACH_MSGH_BITS_COMPLEX_{PORTS,DATA} to mach/message.h.
 * 	[91/07/04  13:10:48  jsb]
 * 
 * 	Use vm_map_copy_t's instead of old style page_lists.
 * 	Still need to eliminate local conversion between formats.
 * 	[91/07/04  10:18:11  jsb]
 * 
 * Revision 2.2  91/06/17  15:47:41  jsb
 * 	Moved here from ipc/ipc_clinput.c.
 * 	[91/06/17  11:02:28  jsb]
 * 
 * Revision 2.2  91/06/06  17:05:18  jsb
 * 	Fixed copyright.
 * 	[91/05/24  13:18:23  jsb]
 * 
 * 	First checkin.
 * 	[91/05/14  13:29:10  jsb]
 * 
 */
/* CMU_ENDHIST */
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
 *	File:	norma/ipc_input.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Functions to support ipc between nodes in a single Mach cluster.
 */

#include <mach_assert.h>

#include <machine/machparam.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <mach/vm_param.h>
#include <mach/port.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/host.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <kern/zalloc.h>
#include <kern/queue.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_marequest.h>
#include <norma/ipc_net.h>
#include <norma/tr.h>
#ifdef	PARAGON860
#include <machine/cpu_master.h>
#endif	PARAGON860

extern zone_t vm_map_copy_zone;

extern vm_map_copy_t	netipc_copy_grab();
extern void		norma_ipc_kobject_send();
extern ipc_port_t	norma_ipc_receive_port();
extern void		netipc_page_put();
boolean_t		norma_remote_to_kmsg();

extern ipc_mqueue_t	norma_ipc_handoff_mqueue;
extern ipc_kmsg_t	norma_ipc_handoff_msg;
extern mach_msg_size_t	norma_ipc_handoff_max_size;
extern mach_msg_size_t	norma_ipc_handoff_msg_size;

/*
 *	These two queues are for messages on their
 *	way into the rest of the ipc system.  These messages
 *	may be complete (all data received) or incomplete
 *	(waiting for more stuff).  In addition, each kmsg
 *	(header) may or may not have been acked; both queues contain
 *	both types of messages; the ikm_norma_acked field in the
 *	header indicates whether the ack has been sent.
 */

ipc_kmsg_t	norma_kmsg_complete;
mpqueue_head_t	norma_kmsg_incomplete_queue_head;
unsigned int	c_norma_kmsg_complete_max_length = 0;
#define	norma_kmsg_incomplete_queue ((mpqueue_head_t *)		\
				     &norma_kmsg_incomplete_queue_head)
#define	norma_kmsg_incomplete_queue_lock()			\
	simple_lock(&norma_kmsg_incomplete_queue->lock)
#define	norma_kmsg_incomplete_queue_unlock()			\
	simple_unlock(&norma_kmsg_incomplete_queue->lock)

unsigned int	c_norma_kmsg_incomplete_queue_count = 0;
unsigned int	c_norma_kmsg_incomplete_queue_inputs = 0;
unsigned int	c_norma_kmsg_incomplete_queue_outputs = 0;

int jc_handoff_fasthits = 0;
int jc_handoff_hits = 0;
int jc_handoff_misses = 0;
int jc_handoff_m2 = 0;		/* XXX very rare (0.1 %) */
int jc_handoff_m3 = 0;
int jc_handoff_m4 = 0;
int jc_netipc_ast = 0;

#if	NORMA_FAST_OOL
int	c_netipc_fast_ool_accept = 0;
int	c_netipc_fast_ool_reject = 0;
#endif	NORMA_FAST_OOL

#define	MACH_MSGH_BITS_COMPLEX_ANYTHING	\
	(MACH_MSGH_BITS_COMPLEX_DATA | MACH_MSGH_BITS_COMPLEX_PORTS)

/*
 *	When delivering large messages, the netipc system must
 *	choose between a fast path that strains memory and a
 *	slow path that is much more friendly.  Fast path messages
 *	must be reassembled entirely with wired-down pages; slow
 *	path messages will be reassembled using pageable memory.
 */
#if 1
#define	NETIPC_PAGE_DELIVERY_LIMIT	(VM_MAP_COPY_PAGE_LIST_MAX * PAGE_SIZE)
#else
#define	NETIPC_PAGE_DELIVERY_LIMIT	(0 * PAGE_SIZE)
#endif
unsigned long				netipc_page_delivery_limit;

/*
 *	The netipc_assembly_wrapper structure contains the data
 *	necessary for deferring the reassembly of an incoming
 *	kmsg or page, possibly from interrupt level to thread context.
 *
 *	These structures are enqueued from interrupt level and
 *	may be further manipulated at interrupt level or dequeued
 *	and handled by a thread.
 *
 *	N.B.  You must know in advance whether the wrapper contains
 *	a kmsg or a page; there is no way to distinguish the two
 *	by examining the wrapper.
 */
typedef struct netipc_assembly_wrapper {
	queue_chain_t		naw_links;	/* queue links */
	unsigned long		naw_remote;	/* remote node number */
	union {
		ipc_kmsg_t	nawu_kmsg;	/* kmsg for reassembly */
		vm_page_t	nawu_page;	/* page being deferred */
	} nawu1;
	union {
		unsigned long	nawu_seqid;	/* kmsgs:  sequence # */
		struct pginfo	nawu_pginfo;	/* pages:  description */
	} nawu2;
} netipc_assembly_wrapper, *netipc_assembly_wrapper_t;

#define	naw_kmsg	nawu1.nawu_kmsg
#define	naw_page	nawu1.nawu_page
#define	naw_seqid	nawu2.nawu_seqid
#define	naw_pginfo	nawu2.nawu_pginfo

#define	NETIPC_ASSEMBLY_WRAPPER_NULL	((netipc_assembly_wrapper_t) 0)

/*
 *	The netipc_assembly_wrapper_zone provides the
 *	wrapper elements needed for other queues.  This
 *	zone may only be manipulated at sploff because
 *	it is shared between thread-context and interrupt-
 *	level routines.
 */
zone_t					netipc_assembly_wrapper_zone;
unsigned long netipc_assembly_wrapper_max = 1190;

/*
 *	Principal data structures for deferred page and kmsg
 *	reassembly:  a zone to provide netipc_assembly_wrappers;
 *	an interlocked queue to pass pages from interrupt level
 *	to thread context; and a queue to hold incomplete kmsgs
 *	(norma_kmsg_incomplete_queue, defined above).
 *
 *	N.B.  The zone, netipc_slow_page_queue_head and and the
 *	norma_kmsg_incomplete_queue_head should only be manipulated
 *	at sploff.
 */
mpqueue_head_t			netipc_slow_page_queue_head;
#define	netipc_slow_page_queue	((mpqueue_head_t*)&netipc_slow_page_queue_head)

#define	NDP_DEBUGGING	MACH_ASSERT

/*
 *	These counts are not MP-safe.
 */
unsigned int			c_netipc_slow_page_queue_count = 0;
unsigned int			c_netipc_slow_page_queue_inputs = 0;
unsigned int			c_netipc_slow_page_queue_outputs = 0;



/*
 * Called from a thread context, by the receiving thread.
 * May replace kmsg with new kmsg.
 *
 * (What if the message stays on the queue forever, hogging resources?)
 *
 * The only places Rich and I can think of where messages are received are:
 *	after calling ipc_mqueue_receive
 *	in exception handling path
 *	in kobject server
 */
int input_msgh_id = 0;
norma_ipc_finish_receiving(kmsgp)
	ipc_kmsg_t *kmsgp;
{
	mach_msg_header_t *msgh;
	mach_msg_bits_t mbits;

	/*
	 * Common case: not a norma message.
	 */
	if ((*kmsgp)->ikm_size != IKM_SIZE_NORMA) {
		return;
	}

	/*
	 * Translate local port, if one exists.
	 */
	msgh = &(*kmsgp)->ikm_header;
	input_msgh_id = msgh->msgh_id;
	mbits = msgh->msgh_bits;
	if (msgh->msgh_local_port) {
		/*
		 * We could call the correct form directly,
		 * eliminating the need to pass ikm_source_node.
		 */
		assert(MACH_MSGH_BITS_LOCAL(mbits) !=
		       MACH_MSG_TYPE_PORT_RECEIVE);
		msgh->msgh_local_port = (mach_port_t)
		    norma_ipc_receive_port((unsigned long)
					   msgh->msgh_local_port,
					   MACH_MSGH_BITS_LOCAL(mbits),
					   (*kmsgp)->ikm_source_node);
	}

	/*
	 * Common case: nothing left to do.
	 */
	if ((mbits & MACH_MSGH_BITS_COMPLEX_ANYTHING) == 0) {
		return;
	}

	/*
	 * Do we need to assemble a large message?
	 */
	if (mbits & MACH_MSGH_BITS_COMPLEX_DATA) {
		norma_ipc_receive_complex_data(kmsgp);
	}

	/*
	 * Do we need to process some ports?
	 *
	 * XXX local port handling should always be done here
	 */
	if (mbits & MACH_MSGH_BITS_COMPLEX_PORTS) {
		norma_ipc_receive_complex_ports(*kmsgp);
	}
}

/*
 * Replace fragmented kmsg with contiguous kmsg.
 */
norma_ipc_receive_complex_data(kmsgp)
	ipc_kmsg_t *kmsgp;
{
	ipc_kmsg_t old_kmsg = *kmsgp, kmsg;
	vm_map_copy_t copy;
	int i;
	boolean_t	was_page_list = TRUE;
	kern_return_t	kr;

	/*
	 * Assemble kmsg pages into one large kmsg.
	 *
	 * XXX
	 * For now, we do so by copying the pages.
	 * We could remap the kmsg instead.
	 */
	kmsg = ikm_alloc(old_kmsg->ikm_header.msgh_size);
	if (kmsg == IKM_NULL) {
		panic("norma_ipc_finish_receiving: ikm_alloc\n");
		return;
	}

	/*
	 * Copy and deallocate the first page.
	 */
	assert(old_kmsg->ikm_size == IKM_SIZE_NORMA);
	assert(old_kmsg->ikm_header.msgh_size + IKM_OVERHEAD > PAGE_SIZE);
	bcopy((char *) old_kmsg, (char *) kmsg, (int) PAGE_SIZE);
	norma_kmsg_put(old_kmsg);
	ikm_init(kmsg, kmsg->ikm_header.msgh_size);
	kmsg->ikm_header.msgh_bits &= ~MACH_MSGH_BITS_COMPLEX_DATA;

	/*
	 * Copy the other pages.
	 */
	copy = kmsg->ikm_copy;
	if (copy->type != VM_MAP_COPY_PAGE_LIST) {
		was_page_list = FALSE;
		kr = vm_map_convert_to_page_list(&copy);
		if (kr != KERN_SUCCESS)
			panic("copy_to_kalloc");
		kmsg->ikm_copy = copy;
	}

	assert(copy->type == VM_MAP_COPY_PAGE_LIST);
	for (i = 0; i < copy->cpy_npages; i++) {
		int length;
		vm_page_t m;
		char *page;

		m = copy->cpy_page_list[i];
		if (i == copy->cpy_npages - 1) {
			length = copy->size - i * PAGE_SIZE;
		} else {
			length = PAGE_SIZE;
		}
		assert(length <= PAGE_SIZE);
		assert((i+1) * PAGE_SIZE + length <=
		       ikm_plus_overhead(kmsg->ikm_header.msgh_size));
		page = (char *) phystokv(m->phys_addr);
		bcopy((char *) page, (char *) kmsg + (i+1) * PAGE_SIZE,
		      (int) length);
	}

	/*
	 * Deallocate pages; release copy object.
	 */

	if (was_page_list == TRUE) {
		netipc_thread_lock();
		for (i = 0; i < copy->cpy_npages; i++)
			netipc_page_put(copy->cpy_page_list[i], FALSE,
					"recv_complex_data");
		netipc_copy_ungrab(copy);
		netipc_thread_unlock();
	} else
		vm_map_copy_discard(copy);
	/*
	 * Return kmsg.
	 */
	*kmsgp = kmsg;
}


/*
 *	This routine can be invoked in thread context
 *	when delivering many pages or it can be invoked
 *	from interrupt context.  We attempt to figure
 *	out how we are invoked based on the type of the
 *	incoming object:  if the object is a page list,
 *	then we assume interrupt context, otherwise we
 *	assume thread context.
 *
 *	The question becomes important when disposing of
 *	the object:  we can't dispose of object allocated
 *	at interrupt level using routines safe only in
 *	thread context.  For !page_list objects, we are
 *	certain that they were allocated in thread context.
 *	Page list objects, on the other hand, are allocated
 *	both ways so we will always free them using the
 *	interrupt-safe routines.  For the freed pages,
 *	netipc_page_put will return excess pages to the
 *	general VM pool, so no worries.  However, the
 *	netipc_copy_ungrab routine potentially can accumulate
 *	excess copy objects without returning them to VM.
 *
 *	We should mark objects so we can dispose of them properly.  XXX
 */
vm_offset_t
copy_to_kalloc(copy, copy_length)
	vm_map_copy_t copy;
	vm_size_t copy_length;
{
	vm_offset_t	k;
	int		i;
	kern_return_t	kr;
	boolean_t	was_page_list;

	assert(copy != VM_MAP_COPY_NULL);
	assert(copy_length <= copy->size);
	k = kalloc(copy_length);
	assert(k);

	was_page_list = TRUE;
	if (copy->type != VM_MAP_COPY_PAGE_LIST) {
		was_page_list = FALSE;
		kr = vm_map_convert_to_page_list(&copy);
		if (kr != KERN_SUCCESS)
			panic("copy_to_kalloc");
	}
	assert(copy->type == VM_MAP_COPY_PAGE_LIST);

	if (copy->size > VM_MAP_COPY_PAGE_LIST_MAX * PAGE_SIZE)
		panic("copy_to_kalloc:  too much");

	/*
	 * Copy the other pages.
	 */
	for (i = 0; i < copy->cpy_npages; i++) {
		vm_size_t length;
		vm_page_t m;
		char *page;

		m = copy->cpy_page_list[i];
		if (i == copy->cpy_npages - 1) {
			length = copy->size - i * PAGE_SIZE;
		} else {
			length = PAGE_SIZE;
		}
		assert(length <= PAGE_SIZE);
		page = (char *) phystokv(m->phys_addr);
		bcopy((char *) page, (char *) k + i * PAGE_SIZE, (int) length);
	}

	/*
	 * Deallocate pages; release copy object.
	 */
	if (was_page_list == TRUE) {
		netipc_thread_lock();
		for (i = 0; i < copy->cpy_npages; i++)
			netipc_page_put(copy->cpy_page_list[i], FALSE,
					"copy_to_kalloc");
		netipc_copy_ungrab(copy);
		netipc_thread_unlock();
	} else
		vm_map_copy_discard(copy);

	return k;
}

/*
 * Translate ports. Don't do anything with data.
 */
norma_ipc_receive_complex_ports(kmsg)
	ipc_kmsg_t kmsg;
{
	mach_msg_header_t *msgh = &kmsg->ikm_header;
	vm_offset_t saddr = (vm_offset_t) (msgh + 1);
	vm_offset_t eaddr = (vm_offset_t) msgh + msgh->msgh_size;

	msgh->msgh_bits &= ~MACH_MSGH_BITS_COMPLEX_PORTS;
	while (saddr < eaddr) {
		mach_msg_type_long_t *type;
		mach_msg_type_size_t size;
		mach_msg_type_number_t number;
		boolean_t is_inline, longform;
		mach_msg_type_name_t type_name;
		vm_size_t length;

		type = (mach_msg_type_long_t *) saddr;
		is_inline = type->msgtl_header.msgt_inline;
		longform = type->msgtl_header.msgt_longform;
		if (longform) {
			type_name = type->msgtl_name;
			size = type->msgtl_size;
			number = type->msgtl_number;
			saddr += sizeof(mach_msg_type_long_t);
		} else {
			type_name = type->msgtl_header.msgt_name;
			size = type->msgtl_header.msgt_size;
			number = type->msgtl_header.msgt_number;
			saddr += sizeof(mach_msg_type_t);
		}

		/* calculate length of data in bytes, rounding up */
		length = ((number * size) + 7) >> 3;

		if (MACH_MSG_TYPE_PORT_ANY(type_name)) {
			ipc_port_t *ports;
			mach_msg_type_number_t i;

			if (is_inline) {
				ports = (ipc_port_t *) saddr;
			} else if (number > 0) {
				vm_map_copy_t copy = * (vm_map_copy_t *) saddr;
				* (vm_offset_t *) saddr =
					copy_to_kalloc(copy, length);
				ports = (ipc_port_t *) * (vm_offset_t *) saddr;
			}
			for (i = 0; i < number; i++) {
				if (type_name == MACH_MSG_TYPE_PORT_RECEIVE) {
#if     MACH_ASSERT
					mumble("rright 0x%x\n", ports[i]);
#endif
				}
				ports[i] = (ipc_port_t)
				    norma_ipc_receive_port((unsigned long)
							   ports[i],
							   type_name,
							   kmsg->
							   ikm_source_node);
			}
		}

		if (is_inline) {
			saddr += (length + 3) &~ 3;
		} else {
			saddr += sizeof(vm_offset_t);
		}
	}
}

/*
 * New routine to handle destination translation and sending
 * of corresponding ack.
 */
kern_return_t
norma_kmsg_receive_dest_and_ack(kmsg)
ipc_kmsg_t	kmsg;
{
	register mach_msg_header_t *msgh;
	unsigned long	ack_error_data = 0;
	kern_return_t kr;

	/*
	 *	Translate destination.
	 */

	msgh = (mach_msg_header_t *) &kmsg->ikm_header;
	if (msgh->msgh_bits & MACH_MSGH_BITS_MIGRATED) {
		kr = norma_ipc_receive_migrating_dest(
			(unsigned long) msgh->msgh_remote_port,
			MACH_MSGH_BITS_REMOTE(msgh->msgh_bits),
			(ipc_port_t *) &msgh->msgh_remote_port,
			&ack_error_data);
	} else {
		kr = norma_ipc_receive_dest(
			(unsigned long) msgh->msgh_remote_port,
			MACH_MSGH_BITS_REMOTE(msgh->msgh_bits),
			(ipc_port_t *) &msgh->msgh_remote_port,
			 &ack_error_data);
	}


#if	NORMA_FAST_OOL
	/*
	 *  See if the other side is trying to send a fast OOL message
	 *  and if so, try to reserve the memory.  Note that we will ask
	 *  for one more page than the byte count would indicate to take
	 *  care of the case where the sender is transmitting from a non-page
	 *  aligned buffer.  This could cause the data to spill over into
	 *  an `extra' for page.  We will give the page back when we receive
	 *  the first page of the copy object if our fears were unfounded.
	 *  At that time we can look at the offset and copy_size to make the
	 *  proper determination.
	 */
	if (kr == KERN_SUCCESS && kmsg->ikm_fast_ool == TRUE) {
		if (netipc_reserve_page(compute_pages(kmsg->ikm_ool_bytes)+1)) {
			netipc_fast_ool_start(norma_get_remote());
			assert(!kmsg->ikm_norma_acked);
			kmsg->ikm_norma_acked = TRUE;
			norma_ipc_ack_fast_ool(kr, ack_error_data);
			c_netipc_fast_ool_accept++;
			return(kr);
		} else {
			ack_error_data = KERN_NO_SPACE;
			c_netipc_fast_ool_reject++;
		}
	}
#endif	NORMA_FAST_OOL
	/*
	 *	Now ack the message.  Note that norma_ipc_ack
	 *	consumes the message if kr is not KERN_SUCCESS.
	 */

	assert(!kmsg->ikm_norma_acked);
	kmsg->ikm_norma_acked = TRUE;
	norma_ipc_ack(kr, ack_error_data);
	return(kr);
}


/*
 * Called in ast-mode, where it is safe to execute ipc code but not to block.
 * (This can actually be in an ast, or from an interrupt handler when the
 * processor was in the idle thread or spinning on norma_ipc_handoff_mqueue.)
 *
 * xxx verify port locking
 */
int norma_atrium_wakeups = 0;

norma_deliver_message(kmsg)
	ipc_kmsg_t kmsg;
{
	ipc_port_t port;
	ipc_mqueue_t mqueue;
	ipc_pset_t pset;
	ipc_thread_t receiver;
	ipc_thread_queue_t receivers;
	
	jc_handoff_fasthits++;

	/*
	 * Change meaning of complex_data bits to mean a kmsg that
	 * must be made contiguous.
	 */
	if (kmsg->ikm_copy == VM_MAP_COPY_NULL) {
		kmsg->ikm_header.msgh_bits &= ~MACH_MSGH_BITS_COMPLEX_DATA;
	} else {
		kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_COMPLEX_DATA;
	}

	/*
	 * If this is a migrating message, then just stick it
	 * directly on the queue, and return.
	 */
	port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
#if	MACH_ASSERT
	port_queue_length_check(port);
#endif	/* MACH_ASSERT */
	if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_MIGRATED) {
		port = port->ip_norma_atrium;
		assert(port != IP_NULL);
		port->ip_msgcount++;
		kmsg->ikm_header.msgh_bits &= ~MACH_MSGH_BITS_MIGRATED;
		ipc_kmsg_enqueue_macro(&port->ip_messages.imq_messages, kmsg);
		if (port->ip_norma_atrium_waiter) {
			port->ip_norma_atrium_waiter = FALSE;
			thread_wakeup(&port->ip_norma_atrium);
			norma_atrium_wakeups++;
		}
		return;
	}

	/*
	 * We must check to see whether this message is destined for a
	 * kernel object. If it is, and if we were to call ipc_mqueue_send,
	 * we would execute the kernel operation, possibly blocking,
	 * which would be bad. Instead, we hand the kmsg off to a kserver
	 * thread which does the delivery and associated kernel operation.
	 */
	assert(IP_VALID(port));
	if (port->ip_receiver == ipc_space_kernel) {
		norma_ipc_kobject_send(kmsg);
		return;
	}

	/*
	 * If there is no one spinning waiting for a message,
	 * then queue this kmsg via the normal mqueue path.
	 *
	 * We don't have to check queue length here (or in mqueue_send)
	 * because we already checked it in receive_dest_*.
	 */
	if (norma_ipc_handoff_mqueue == IMQ_NULL) {
		ipc_mqueue_send_always(kmsg);
		return;
	}

	/*
	 * Find the queue associated with this port.
	 */
	ip_lock(port);
	port->ip_msgcount++;
	assert(port->ip_msgcount > 0);
	pset = port->ip_pset;
	if (pset == IPS_NULL) {
		mqueue = &port->ip_messages;
	} else {
		mqueue = &pset->ips_messages;
	}

	/*
	 * If someone is spinning on this queue, we must release them.
	 * However, if the message is too large for them to successfully
	 * receive it, we continue below to find a receiver.
	 *
	 * Check norma_iopc_handoff_msg to make sure we don't overwrite
	 * a kmsg that is in progress (a nasty race condition); if it is
	 * set, drop through and either give the new kmsg to a waiting
	 * thread or queue it on the port.
	 */
	if (mqueue == norma_ipc_handoff_mqueue && norma_ipc_handoff_msg == 0) {
		norma_ipc_handoff_msg = kmsg;
		if (kmsg->ikm_header.msgh_size <= norma_ipc_handoff_max_size) {
			ip_unlock(port);
			return;
		}
		norma_ipc_handoff_msg_size = kmsg->ikm_header.msgh_size;
	}
	
	imq_lock(mqueue);
	receivers = &mqueue->imq_threads;
	ip_unlock(port);
	
	for (;;) {
		receiver = ipc_thread_queue_first(receivers);
		if (receiver == ITH_NULL) {
			/* no receivers; queue kmsg */
			
			ipc_kmsg_enqueue_macro(&mqueue->imq_messages, kmsg);
			imq_unlock(mqueue);
			return;
		}
		
		ipc_thread_rmqueue_first_macro(receivers, receiver);
		assert(ipc_kmsg_queue_empty(&mqueue->imq_messages));
		
		if (kmsg->ikm_header.msgh_size <= receiver->ith_msize) {
			/* got a successful receiver */
			
			receiver->ith_state = MACH_MSG_SUCCESS;
			receiver->ith_kmsg = kmsg;
			receiver->ith_seqno = port->ip_seqno++;
			imq_unlock(mqueue);
			
			thread_go(receiver);
			return;
		}
		
		receiver->ith_state = MACH_RCV_TOO_LARGE;
		receiver->ith_msize = kmsg->ikm_header.msgh_size;
		thread_go(receiver);
	}
}


/*
 *	Enqueue a message for delivery into local Mach IPC.
 *	When possible, notice a pending receiver to allow
 *	direct delivery.
 */
void
norma_enqueue_message(kmsg)
ipc_kmsg_t	kmsg;
{
	register ipc_kmsg_t	*kmsgp;
	unsigned int		c_queued_messages;
	int	s;
	TR_DECL("norma_enqueue_message");

	assert(netipc_locked());
	tr2("enqueuing kmsg 0x%x", kmsg);

	/*
	 *	Blow away kmsg links.  This many not
	 *	be necessary when delivering the kmsg
	 *	directly but it costs little.
	 */
	kmsg->ikm_next = IKM_NULL;
	kmsg->ikm_prev = IKM_BOGUS;	/* XXX */

	/*
	 *	First, "enqueue" the incoming message.
	 */
	if (norma_kmsg_complete != IKM_NULL) {
		c_queued_messages = 0;
		for (kmsgp = &norma_kmsg_complete;
		     (*kmsgp)->ikm_next;
		     kmsgp = &(*kmsgp)->ikm_next) {
			++c_queued_messages;
			continue;
		}
		(*kmsgp)->ikm_next = kmsg;
		if (c_queued_messages > c_norma_kmsg_complete_max_length)
			c_norma_kmsg_complete_max_length = c_queued_messages;
	} else {
		norma_kmsg_complete = kmsg;
	}

	/*
	 *	There is a thread spinning awaiting a message,
	 *	so the message at the head of the list may be
	 *	delivered now.  The kmsg might not yet have
	 *	been acked, so do that now if needed.  If the
	 *	ack fails, the message is destroyed so we
	 *	should try again.  Otherwise, deliver the
	 *	message and return.
	 *
	 * 	XXX This comment preserved from original norma_handoff_kmsg.
	 *	1. should be conditionalized on whether we really
	 *		are called at interrupt level
	 *	2. should check flag set by *all* idle loops
	 *	3. this comment applies as well to deliver_page
	 */
	while (norma_ipc_handoff_mqueue != IMQ_NULL &&
	       norma_kmsg_complete != IKM_NULL) {
		kmsg = norma_kmsg_complete;
		norma_kmsg_complete = kmsg->ikm_next;
		kmsg->ikm_next = IKM_BOGUS; /* XXX */
		tr2("attempt handoff of kmsg 0x%x", kmsg);
		if (!kmsg->ikm_norma_acked &&
		    norma_kmsg_receive_dest_and_ack(kmsg) != KERN_SUCCESS)
			continue;
		tr2("delivering kmsg 0x%x", kmsg);
		norma_deliver_message(kmsg);
		return;
	}

	/*
	 *	Can't deliver any message right now.  Post the AST
	 *	so messages will be delivered later.
	 */
	tr1("message deferred for AST processing");
	jc_handoff_misses++;
	s = sploff();
	ast_on(cpu_number(), AST_NETIPC);
	splon(s);
}


/*
 * Called from a thread context where it's okay to lock but not to block.
 */
void netipc_ast()
{
	ipc_kmsg_t			kmsg;
	netipc_assembly_wrapper_t	nawp;
	struct queue_entry		*next;
	kern_return_t			kr;
	int				s;
	TR_DECL("netipc_ast");

	tr1("entry");

#ifdef	FORCE_MASTER
	MUST_BE_MASTER("netipc_ast()", my_caller());
#endif	/* FORCE_MASTER */

	netipc_thread_lock();

	/*
	 *	Handle complete kmsgs.
	 */
	while (kmsg = norma_kmsg_complete) {
		norma_kmsg_complete = kmsg->ikm_next;
		kmsg->ikm_next = IKM_BOGUS; /* XXX */
		tr2("found complete kmsg 0x%x", kmsg);
		if (!kmsg->ikm_norma_acked &&
		    norma_kmsg_receive_dest_and_ack(kmsg) != KERN_SUCCESS)
			continue;
		tr2("delivering complete kmsg 0x%x\n", kmsg);
		norma_deliver_message(kmsg);
	}

	/*
	 *	Ack any incomplete kmsgs that need it.  This is more
	 *	complex because we're walking a list and choosing
	 *	whether to consume elements instead of always consuming
	 *	them (a return other than KERN_SUCCESS from
	 *	norma_kmsg_receive_dest_and_ack indicates that the message
	 *	has been consumed).
	 *
	 *	N.B.  This loop assumes (a) sploff and (b) uniprocessor.
	 *	Because elements can be removed from the queue by this
	 *	loop, it is necessary to save a pointer to the following
	 *	element before processing the current one.  This traversal
	 *	may not be safe in an MP environment.  XXX
	 */
	printf6("netipc_ast:  traversing incomplete queue\n");
	s = sploff();			/* for incomplete queue lock */
	norma_kmsg_incomplete_queue_lock();
	tr3("incomplete kmsg queue {next=0x%x,prev=0x%x}",
	    norma_kmsg_incomplete_queue_head.head.next,
	    norma_kmsg_incomplete_queue_head.head.prev);
	next = queue_first(&norma_kmsg_incomplete_queue->head);
	while (!queue_end(&norma_kmsg_incomplete_queue->head, next)) {
		nawp = (netipc_assembly_wrapper_t) next;
		next = queue_next(&nawp->naw_links);
		tr2("incomplete queue nawp 0x%x", nawp);
		kmsg = nawp->naw_kmsg;
		assert(kmsg != 0);
		assert(!queue_empty(&norma_kmsg_incomplete_queue->head));
		if (!kmsg->ikm_norma_acked) {
			norma_kmsg_incomplete_queue_unlock();
			tr2("nawp has unacked kmsg 0x%x", kmsg);
			kr = norma_kmsg_receive_dest_and_ack(kmsg);
			norma_kmsg_incomplete_queue_lock();
			if (kr != KERN_SUCCESS) {
				tr3("ack fails:  kmsg 0x%x status 0x%x",
				    kmsg, kr);
				remqueue(&norma_kmsg_incomplete_queue->head,
					 (queue_entry_t) nawp);
				++c_norma_kmsg_incomplete_queue_outputs;
				--c_norma_kmsg_incomplete_queue_count;
				norma_kmsg_incomplete_queue_unlock();
				zfree(netipc_assembly_wrapper_zone,
				      (vm_offset_t) nawp);
				norma_kmsg_incomplete_queue_lock();
			}
		} else {
			tr2("nawp has acked kmsg 0x%x", kmsg);
		}
	}
	norma_kmsg_incomplete_queue_unlock();
	splon(s);
	printf6("netipc_ast:  done traversing incomplete queue\n");
	netipc_thread_unlock();
	tr1("exit");
}

norma_deliver_kmsg(kmsg, remote, seqid)
	ipc_kmsg_t kmsg;
	unsigned long remote;
	unsigned long seqid;
{
	register mach_msg_header_t	*msgh;
	netipc_assembly_wrapper_t	nawp;
	int				s;
	TR_DECL("norma_deliver_kmsg");

	tr4("kmsg 0x%x remote 0x%x seqid 0x%x", kmsg, remote, seqid);
	assert(netipc_intr_locked());

	/*
	 * Initialize norma fields in kmsg structure.  Remote port
	 * check has been moved to locations where it's known to be safe.
	 */
	kmsg->ikm_copy = VM_MAP_COPY_NULL;
	kmsg->ikm_source_node = remote;
	kmsg->ikm_norma_acked = FALSE;
	msgh = (mach_msg_header_t *) &kmsg->ikm_header;

	/*
	 * If the message is incomplete, put it on the incomplete list.
	 * Must check destination before ack'ing message; that requires
	 * an AST.
	 */
	if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX_DATA) {
		s = sploff();
		nawp = (netipc_assembly_wrapper_t)
			zget(netipc_assembly_wrapper_zone);
		splon(s);
		if (nawp == NETIPC_ASSEMBLY_WRAPPER_NULL)
			panic("norma_deliver_kmsg: netipc_assembly_wrapper");
		nawp->naw_remote = remote;
		nawp->naw_seqid = seqid;
		nawp->naw_kmsg = kmsg;
		kmsg->ikm_next = IKM_BOGUS; /* XXX */
		kmsg->ikm_prev = IKM_BOGUS; /* XXX */
		tr3("enqueue incomplete kmsg 0x%x nawp 0x%x", kmsg, nawp);
		s = sploff();
		mpenqueue_tail(norma_kmsg_incomplete_queue,
			       (queue_entry_t) &nawp->naw_links);
		++c_norma_kmsg_incomplete_queue_inputs;
		++c_norma_kmsg_incomplete_queue_count;
		ast_on(cpu_number(), AST_NETIPC);
		splon(s);
		return;
	}

	norma_enqueue_message(kmsg);
	tr1("exit");
}

kern_return_t
norma_deliver_page_continuation(cont_args, copy_result)
	char *cont_args;
	vm_map_copy_t *copy_result;
{
	boolean_t abort;

	abort = (copy_result == (vm_map_copy_t *) 0);
	if (abort) {
		/*
		 * XXX need to handle this
		 */
		panic("norma_deliver_page_continuation: abort!\n");
		return KERN_SUCCESS;
	} else {
		*copy_result = (vm_map_copy_t) cont_args;
		return KERN_SUCCESS;
	}
}


/*
 *	norma_deliver_page has two modes of operation.  When
 *	slowly == FALSE, the routine assembles incoming pages
 *	in a page-list copy object and expects to be called
 *	from interrupt level.  When slowly == TRUE, the routine
 *	uses an object-flavor copy object and expects to be
 *	called from within thread context.
 *
 *	The latter case exists to handle pages transferred as part
 *	of messages consisting of large numbers of pages.  We want
 *	to make sure that these pages can themselves be paged while
 *	the message is in transit.  We use a pageable memory object
 *	to hold the pages as we accumulate them.
 *
 *	Note that, for slowly == TRUE, this routine may block on
 *	locks but it will never block due to insufficient memory.
 *	We guarantee that any page reaching this routine will be
 *	inserted into a pageable object without waiting for
 *	additional free memory.
 *
 *	When called within thread context, whatever routine
 *	received the packet at interrupt level takes responsibility
 *	for acking the packet.  However, the caller assumes
 *	responsibility for netipc lock synchronization.  This routine
 *	must still disable interrupts at appropriate times when
 *	using data structures shared with interrupt level.
 *
 *	The thread_context parameter has been added to allow
 *	norma_deliver_page to reassemble pages using the fast page-
 *	list technology but from thread context.  This is primarily
 *	intended for use when debugging.  Slowly may only ever
 *	be TRUE when calling norma_deliver_page from thread context.
 */
void
norma_deliver_page(page, page_info, remote, slowly, thread_context)
vm_page_t	page;
struct pginfo	*page_info;
unsigned long	remote;
boolean_t	slowly;
boolean_t	thread_context;
{
	ipc_kmsg_t			kmsg, *kmsgp;
	vm_map_copy_t			copy, *copyp, new_copy;
	vm_object_t			object;
	vm_offset_t			offset;
	kern_return_t			kr;
	boolean_t			page_last = page_info->pg_page_last;
	boolean_t			copy_last = page_info->pg_copy_last;
	netipc_assembly_wrapper_t	nawp;
	int				s;
	TR_DECL("norma_deliver_page");

	assert(slowly == TRUE || netipc_intr_locked());
	assert(slowly == FALSE || netipc_thread_locked());
	assert(thread_context == TRUE || slowly == FALSE);
	tr3("entry, page 0x%x page_info 0x%x", page, page_info);

	/*
	 *	Find the kmsg under construction.  For the case
	 *	where norma_deliver_page is being called from
	 *	thread context, interrupts must be disabled while
	 *	traversing the norma_kmsg_incomplete list.
	 */
	if (thread_context == TRUE)
		s = sploff();
	(void) norma_remote_to_kmsg_internal(remote,
					     page_info->pg_first_seqid,
					     TRUE, &nawp);
	kmsg = nawp->naw_kmsg;
	if (thread_context == TRUE)
		splon(s);
	tr2("located kmsg 0x%x", kmsg);

	/*
	 * Find the pointer to the copy object within
	 * the kmsg.  A long but otherwise inline kmsg
	 * may in fact overflow into an additional page
	 * (page_info->pg_msgh_offset == 0).  Otherwise,
	 * we want to find the pointer within the kmsg
	 * for this copy object.
	 */
	if (page_info->pg_msgh_offset == 0)
		copyp = &kmsg->ikm_copy;
	else
		copyp = (vm_map_copy_t *) ((vm_offset_t) &kmsg->ikm_header +
					   page_info->pg_msgh_offset);

	/*
	 * If this is the first page, create a copy object.
	 */
	if (page_info->pg_page_first) {
		unsigned long copy_size = page_info->pg_copy_size;
		unsigned long copy_offset = page_info->pg_copy_offset;

#if	NORMA_FAST_OOL
		/*
		 *  See if we need to give back a reserved fast OOL page 
		 *  that was was allocated for the case where the transfer
		 *  started on a non-zero offset and spilled over into an
		 *  `extra' page.
		 */
		if (is_fast_ool(remote)) {
			if ((copy_offset != 0) &&
			    (compute_pages(copy_size) !=
			   	  compute_pages(copy_size + copy_offset)) ){
				tr1("kept overcommitted fast OOL page");
			} else {
				netipc_reserve_cancel();
				tr1("giving back overcommitted fast OOL page");
			}
		}

#endif	NORMA_FAST_OOL
			
		if (slowly == TRUE) {
			/*
			 *	Use a pageable object to store the
			 *	incoming message.  Note that neither
			 *	vm_object_allocate nor vm_map_copyin_object
			 *	will block -- their respective zones
			 *	are not pageable.  This path can't
			 *	block due to insufficient memory.
			 */
			netipc_thread_unlock();
			object = vm_object_allocate(round_page(copy_size));
			kr = vm_map_copyin_object(object, copy_offset,
						  copy_size, &copy);
			assert(kr == KERN_SUCCESS);
			assert(copy->type == VM_MAP_COPY_OBJECT);
			*copyp = copy;
			netipc_thread_lock();
#if	NDP_DEBUGGING
			/*
			 *	Steal the high bit from the sequence number
			 *	to declare that the first page of the message
			 *	has been seen.  This is not a generally safe
			 *	bit as sequence numbers certainly could use
			 *	the high bit of the longword, but it is safe
			 *	enough for these limited debugging purposes.
			 */
			kmsg->ikm_header.msgh_seqno |= 0x80000000;
#endif
			/*
			 *	Fall through to page assembly code.
			 */
		} else {
			copy = netipc_copy_grab();
			if (copy == VM_MAP_COPY_NULL) {
				if (thread_context == TRUE)
					netipc_page_put(page, FALSE,
							"norma_deliver_page");
				else
					norma_ipc_drop();
				return;
			}
			copy->type = VM_MAP_COPY_PAGE_LIST;
			copy->cpy_npages = 1;
			copy->offset = copy_offset;
			copy->size = copy_size;
			copy->cpy_page_list[0] = page;
			copy->cpy_cont = ((kern_return_t (*)()) 0);
			copy->cpy_cont_args = (char *)VM_MAP_COPYIN_ARGS_NULL;
			*copyp = copy;
#if	NDP_DEBUGGING
			kmsg->ikm_header.msgh_seqno |= 0x80000000;
#endif
			goto test_for_completion;
		}
	}

#if	NDP_DEBUGGING
	/*
	 *	At this step, we should have seen at least the
	 *	first page of the message (this in fact may be
	 *	the first page of the message).
	 */
	assert((kmsg->ikm_header.msgh_seqno & 0x80000000) != 0);
#endif

	copy = *copyp;

	if (slowly == TRUE) {
		/*
		 *	Insert the page into the object-flavor copy object.
		 *	Because there is no explicit operation on a copy
		 *	object for this case, we code this operation
		 *	directly on the copy object's underlying object.
		 *	We use copy_index as a running index into
		 *	the object.  Unpleasant assumptions:
		 *		- pages arrive in order
		 *		- page assembly is single-threaded
		 *	However, these assumptions are also true of the
		 *	page-list assembly code.
		 */
#if	NETIPC_PAGE_TRACE
		assert(thread_context == TRUE);
		if (kmsg->ikm_norma_acked) {
			ipc_port_t p;
			extern ipc_port_t norma_port_lookup_locked();
			p=norma_port_lookup_locked(page_info->pg_remote_port);
			assert(p == IP_NULL ||
			       (mach_port_t) p ==
			       kmsg->ikm_header.msgh_remote_port);
		} else
			assert((unsigned long)kmsg->ikm_header.msgh_remote_port
			       == page_info->pg_remote_port);
		assert(page_info->pg_copy_index*PAGE_SIZE == copy->cpy_index);
		assert(page_info->pg_msgh_id == kmsg->ikm_header.msgh_id);
#endif
		assert(copy->type == VM_MAP_COPY_OBJECT);
		object = copy->cpy_object;
		offset = copy->cpy_index;
		copy->cpy_index += PAGE_SIZE;

		vm_object_lock(object);
		vm_page_lock_queues();
		vm_page_insert(page, object, offset);
		page->dirty = TRUE;
		page->busy = page->precious = FALSE;
		page->page_lock = page->unlock_request = VM_PROT_NONE;
		vm_page_activate(page);
		vm_page_unlock_queues();
		vm_object_unlock(object);

		goto common_completion;
	} else {
		/*
		 * There is a preexisting copy object.
		 * If we are in the first page list, things are simple.
		 */
		assert(copy->type == VM_MAP_COPY_PAGE_LIST);
		if (copy->cpy_npages < VM_MAP_COPY_PAGE_LIST_MAX) {
			copy->cpy_page_list[copy->cpy_npages++] = page;
			goto test_for_completion;
		}

		/*
		 * We are beyond the first page list.
		 * Chase list of copy objects until we are in the last one.
		 */
		printf3("deliver_page: npages=%d\n", copy->cpy_npages);
		while (vm_map_copy_has_cont(copy)) {
			copy = (vm_map_copy_t) copy->cpy_cont_args;
		}

		/*
		 * Will we fit in this page list?
		 * Note: this may still be the first page list,
		 * but in that case the test will fail.
		 */
		if (copy->cpy_npages < VM_MAP_COPY_PAGE_LIST_MAX) {
			copy->cpy_page_list[copy->cpy_npages++] = page;
			(*copyp)->cpy_npages++;
			goto test_for_completion;
		}

		/*
		 * We won't fit; we have to create a continuation.
		 */
		printf3("deliver_page: new cont, copy=0x%x\n", copy);
		assert(copy->cpy_cont_args == (char *) 0);
		if (copy != *copyp) {
			/*
			 * Only first copy object has fake
			 * (grand total) npages.  Only first
			 * copy object has unalligned offset.
			 */
			assert(copy->cpy_npages == VM_MAP_COPY_PAGE_LIST_MAX);
			assert(copy->offset == 0);
		}
		new_copy = netipc_copy_grab();
		if (new_copy == VM_MAP_COPY_NULL) {
			if (thread_context == TRUE)
				netipc_page_put(page, FALSE,
						"norma_deliver_page2");
			else
				norma_ipc_drop();
			return;
		}
		new_copy->cpy_page_list[0] = page;
		new_copy->cpy_npages = 1;
		new_copy->cpy_cont = ((kern_return_t (*)()) 0);
		new_copy->cpy_cont_args = (char *) VM_MAP_COPYIN_ARGS_NULL;
		new_copy->size = copy->size -
			(PAGE_SIZE*VM_MAP_COPY_PAGE_LIST_MAX - copy->offset);
		assert(trunc_page(copy->offset) == 0);
		new_copy->offset = 0;
		copy->cpy_cont = norma_deliver_page_continuation;
		copy->cpy_cont_args = (char *) new_copy;
		(*copyp)->cpy_npages++;
	}

test_for_completion:
	/*
	 * Mark page dirty (why?) and not busy.
	 */
	assert(! page->tabled);
	page->busy = FALSE;
	page->dirty = TRUE;

#if	NORMA_FAST_OOL
	/*
	 *  If this is the last page in the last copy object, get out of
	 *  fast OOL state here so the ack will be sent.
	 */
	if (page_last && copy_last) {
		tr1("Last FOOL page has been received");
		netipc_fast_ool_done(remote);
	}
#endif	NORMA_FAST_OOL

        /*
	 *	We were able to put the page in a page list somewhere.
	 *	We therefore know at this point that this call will succeed,
	 *	so if we are delivering the page from interrupt context,
	 *	generate the acknowledgement now.  If we are handling the
	 *	page in thread context, the acknowledgement was generated
	 *	much earlier.
	 *
	 *	N.B.  We saved pg_page_last and pg_copy_last earlier
	 *	because the caller may pass a pointer into the global
	 *	netipc_recv_hdr when norma_deliver_page is invoked from
	 *	interrupt level.  The norma_ipc_ack will restart a
	 *	network operation, clobbering netipc_recv_hdr.
	 */
	if (thread_context == FALSE)
		norma_ipc_ack(KERN_SUCCESS, 0L);

common_completion:
	assert(thread_context == TRUE || slowly == FALSE);

	/*
	 * If this is not the last page, then the message
	 * is not yet complete, so return now.
	 */
	if (!page_last) {
		return;
	}

	/* 
	 *	This is the last page in the copy object.  For a page
	 *	list copy object, correct copy->cpy_npages.  For an
	 *	object flavor copy object, reset cpy_index to zero;
	 *	other routines assume that on first sight an object
	 *	flavor copy object will have zero in cpy_index.
	 */
#if	NETIPC_PAGE_TRACE
	/*
	 *	These assertions may only be checked for the case
	 *	where delivery is accomplished in thread context
	 *	because in interrupt context the norma_icp_ack
	 *	above will clobber the structure pointed to by page_info.
	 */
	if (thread_context == TRUE && slowly == TRUE) {
		assert(page_info->pg_copy_npages*PAGE_SIZE==copy->cpy_index);
	}
#endif
	if (slowly == TRUE) {
		assert(copy->type == VM_MAP_COPY_OBJECT);
		assert(*copyp == copy);
		copy->cpy_index = 0;
	} else {
		assert(copy->type == VM_MAP_COPY_PAGE_LIST);
		assert(!vm_map_copy_has_cont(*copyp) ||
		       vm_map_copy_cont_is_valid(*copyp));
		if ((*copyp)->cpy_npages > VM_MAP_COPY_PAGE_LIST_MAX)
			(*copyp)->cpy_npages = VM_MAP_COPY_PAGE_LIST_MAX;
	}

	/*
	 * If this is not the last copy object, then the message is
	 * not yet complete, so return.
	 */
	if (!copy_last) {
		return;
	}

#if	NDP_DEBUGGING
	/*
	 *	The tracking bit should be on, indicating that
	 *	we have seen (at least) the first page in this message.
	 *	Turn the bit back off so it won't disturb later processing.
	 */
	assert((kmsg->ikm_header.msgh_seqno & 0x80000000) != 0);
	kmsg->ikm_header.msgh_seqno &= ~0x80000000;
#endif

	/*
	 *	Remove the netipc_assembly_wrapper and its kmsg
	 *	from the norma_kmsg_incomplete_queue.  Because
	 *	the message is complete, there can be no other
	 *	thread that knows about the kmsg.  Also free up
	 *	the wrapper.
	 */
	tr3("for kmsg 0x%x shedding nawp 0x%x", kmsg, nawp);
	s = sploff();
	simple_lock(&norma_kmsg_incomplete_queue->lock);
	remqueue(&norma_kmsg_incomplete_queue->head, (queue_entry_t) nawp);
	++c_norma_kmsg_incomplete_queue_outputs;
	--c_norma_kmsg_incomplete_queue_count;
	simple_unlock(&norma_kmsg_incomplete_queue->lock);
	zfree(netipc_assembly_wrapper_zone, (vm_offset_t) nawp);

	/*
	 * If this was a slow delivery, ack the page.
	 * norma_deliver_page_slowly did not ack it
	 * because once the last page is acked, the next message
	 * can come in, and it might get ahead of this one.
	 */
	if (slowly) {
		assert(thread_context);
		norma_ipc_ack(KERN_SUCCESS, 0L);
	}

	/*
	 *	Pass the message on to the next higher layer.
	 */
	norma_enqueue_message(kmsg);
	tr1("exit");
	splon(s);
}


/*
 *	This thread is awakened whenever there are
 *	pages enqueued to the netipc_slow_page_queue.
 *	The netipc_slow_page_thread runs the queue,
 *	calling norma_deliver_page on each page.
 */
boolean_t	netipc_slow_page_switch = TRUE; /* XXX */
extern thread_t netipc_lock_owner;	/* XXX */
#define	THREAD_INTR	((thread_t) 1)	/* XXX */

void
netipc_slow_page_continuation()
{
	netipc_assembly_wrapper_t	nawp;
	int				s;

	netipc_thread_lock();
	for (;;) {
		s = sploff();
		mpdequeue_head(netipc_slow_page_queue,(queue_entry_t *)&nawp);
		if (nawp == NETIPC_ASSEMBLY_WRAPPER_NULL) {
			assert_wait((int) netipc_slow_page_continuation,
				    FALSE);
			(void) netipc_thread_unlock();
			splon(s);
			thread_block(netipc_slow_page_continuation);
			/* NOTREACHED */
		}
		assert(c_netipc_slow_page_queue_count > 0);
		--c_netipc_slow_page_queue_count;
		++c_netipc_slow_page_queue_outputs;
		splon(s);
		printf6("deferred page:  remote=%d\n",nawp->naw_remote);
		if (netipc_slow_page_switch == TRUE) {
			norma_deliver_page(nawp->naw_page, &nawp->naw_pginfo,
					   nawp->naw_remote, TRUE, TRUE);
		} else {
			s = sploff();
			netipc_lock_owner = THREAD_INTR;
			splon(s);
			norma_deliver_page(nawp->naw_page, &nawp->naw_pginfo,
					   nawp->naw_remote, FALSE, TRUE);
			s = sploff();
			netipc_lock_owner = current_thread();
			splon(s);
		}
		s = sploff();
		zfree(netipc_assembly_wrapper_zone, (vm_offset_t) nawp);
		splon(s);
	}
}


void
netipc_slow_page_thread()
{
	kern_return_t	kr;

	/*
	 *	Wire this thread to prevent deadlock that occurs when
	 *	we need to page to a remote machine and cannot start
	 *	a thread due to insufficient memory.
	 */
	kr = thread_wire (realhost.host_priv_self, current_thread(), TRUE);
	if (kr != KERN_SUCCESS)
		panic("netipc_slow_page_thread:  thread_wire fails");
	netipc_slow_page_continuation();
	/* NOTREACHED */
}


/*
 *	Enqueue all necessary information about a page to
 *	allow it to be processed by an auxiliary kernel thread.
 *	The message will then be reassembled using a pageable copy
 *	object rather than a (wired) page-list copy object.
 *
 *	At this point we have committed to taking the page so we
 *	we acknowledge it now.  The ack rips away netipc_recv_page,
 *	which is probably what we were called with, and replaces it
 *	with a new page.
 */
void
norma_deliver_page_slowly(page, hdr)
vm_page_t		page;
struct netipc_hdr	*hdr;
{
	netipc_assembly_wrapper_t	nawp;
	int				s;
	TR_DECL("norma_deliver_page_slowly");

	assert(netipc_intr_locked());
	tr3("entry, page 0x%x hdr 0x%x", page, hdr);

	s = sploff();
	nawp = (netipc_assembly_wrapper_t) zget(netipc_assembly_wrapper_zone);
	splon(s);
	if (nawp == NETIPC_ASSEMBLY_WRAPPER_NULL)
		panic("norma_deliver_page_slowly: no wrapper");
	tr2("allocated nawp 0x%x", nawp);

	nawp->naw_remote = hdr->remote;
	nawp->naw_pginfo = hdr->pg;
	nawp->naw_page = page;

	s = sploff();
	mpenqueue_tail(netipc_slow_page_queue,
		       (queue_entry_t) &nawp->naw_links);
	++c_netipc_slow_page_queue_count;
	++c_netipc_slow_page_queue_inputs;
	splon(s);

	/*
	 *	Sleep/wakeup of this thread is synchronized
	 *	through the netipc_thread_lock.
	 */
	thread_wakeup((int) netipc_slow_page_continuation);

	/*
	 *	If this is the last page in the message,
	 *	the ack must be delayed until actual delivery.
	 */
	if (!(hdr->pg.pg_page_last && hdr->pg.pg_copy_last)) {
		norma_ipc_ack(KERN_SUCCESS, 0L);
	}
#if	NORMA_FAST_OOL
	/*
	 *  This is the last page in a page list so do a blanket clear of
	 *  the fast OOL flag so if we were doing a fast OOL transfer the
	 *  last page will get acked.  If we weren't, no harm.
	 */
	else
		netipc_fast_ool_done(hdr->remote);
#endif	NORMA_FAST_OOL

	printf6("norma_deliver_page_slowly exit\n");
	tr1("exit");
}


/*
 * Attempt to find the kmsg currently in progress
 * for the given remote node.
 *
 * N.B.  Must be called at sploff!
 */
boolean_t
norma_remote_to_kmsg_internal(remote, first_seqid, must_find, result_nawp)
unsigned long			remote;
unsigned long			first_seqid;
boolean_t			must_find;
netipc_assembly_wrapper_t	*result_nawp;
{
	ipc_kmsg_t			*kmsgp, kmsg;
	ipc_port_t			port;
	unsigned long			uid;
	netipc_assembly_wrapper_t	nawp;
	TR_DECL("norma_remote_to_kmsg_internal");

	tr3("remote 0x%x first_seqid 0x%x", remote, first_seqid);

	/*
	 * Find appropriate kmsg.
	 * XXX consider making this an array?
	 */
#if	NETIPC_PAGE_TRACE
	*result_nawp = NETIPC_ASSEMBLY_WRAPPER_NULL;
#endif
	simple_lock(&norma_kmsg_incomplete_queue->lock);
	queue_iterate(&norma_kmsg_incomplete_queue->head, nawp,
		      netipc_assembly_wrapper_t, naw_links) {
		if (nawp->naw_remote == remote
		    && nawp->naw_seqid == first_seqid) {
#if	NETIPC_PAGE_TRACE
			if (*result_nawp != NETIPC_ASSEMBLY_WRAPPER_NULL)
				panic("norma_remote_to_kmsg:  duplicates!");
#endif
			*result_nawp = nawp;
#if	!NETIPC_PAGE_TRACE
			simple_unlock(&norma_kmsg_incomplete_queue->lock);
			return TRUE;
#endif
		}
	}
	simple_unlock(&norma_kmsg_incomplete_queue->lock);
#if	NETIPC_PAGE_TRACE
	if (*result_nawp != NETIPC_ASSEMBLY_WRAPPER_NULL)
		return TRUE;
#endif
	if (must_find == TRUE)
		panic("norma_remote_to_kmsg_internal:  kmsg not found");
	*result_nawp = NETIPC_ASSEMBLY_WRAPPER_NULL;
	return FALSE;
}


/*
 *	Hide internal details of netipc assembly wrappers
 *	from the rest of the world.
 */
boolean_t
norma_remote_to_kmsg(remote, first_seqid, must_find, result_kmsg)
unsigned long			remote;
unsigned long			first_seqid;
boolean_t			must_find;
ipc_kmsg_t			*result_kmsg;
{
	netipc_assembly_wrapper_t	nawp;
	boolean_t			status;

	status = norma_remote_to_kmsg_internal(remote, first_seqid,
					       must_find, &nawp);
	*result_kmsg = nawp->naw_kmsg;
	return status;
}


/*
 *	Initialization for the input side of netipc.
 */
void
netipc_input_init()
{
	netipc_assembly_wrapper_t	nawp;
	vm_size_t			memory_size;
	vm_offset_t			memory;
	kern_return_t			kr;

	netipc_page_delivery_limit = NETIPC_PAGE_DELIVERY_LIMIT;

	/*
	 *	Initialize zone of free netipc_slow_page structures
	 *	and queue for passing filled structures from interrupt
	 *	level to thread context.  Allocate wired memory and
	 *	cram it into the zone.
	 */
	mpqueue_init(norma_kmsg_incomplete_queue);
	mpqueue_init(netipc_slow_page_queue);
	memory_size = round_page(netipc_assembly_wrapper_max * sizeof(*nawp));
	netipc_assembly_wrapper_zone = zinit(sizeof(*nawp), memory_size,
					     PAGE_SIZE, FALSE,
					     "netipc assembly wrappers");

	kr = kmem_alloc_wired(kernel_map, &memory, memory_size);
	if (kr != KERN_SUCCESS)
		panic("netipc_input_init:  no wrapper memory");

	zcram(netipc_assembly_wrapper_zone, memory, memory_size);

	(void) kernel_thread(kernel_task, netipc_slow_page_thread, (char *)0);
}

