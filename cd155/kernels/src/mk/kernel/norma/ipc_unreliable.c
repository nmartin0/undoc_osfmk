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
 * $Log: ipc_unreliable.c,v $
 * Revision 1.20  1994/11/18  20:56:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.19  1994/03/30  01:20:09  joel
 *  Reviewer: terry
 *  Risk: low
 *  Benefit or PTS #: 7434
 *  Testing: mcat for two weeks
 *  Module(s): norma/ipc_unreliable.c
 *
 * Revision 1.15.2.4  1994/03/30  01:18:01  joel
 *  Reviewer: terry
 *  Risk: low
 *  Benefit or PTS #: 7434
 *  Testing: MCAT for two weeks
 *  Module(s): kernel/norma/ipc_unreliable.c
 *
 * Revision 1.15.2.3  1994/01/12  20:31:05  rkl
 *  Put in a *working* algorithm to determine if the overallocated FOOL page
 *  should be returned.
 *
 *  Reviewer: andyp
 *  Risk: low
 *  Benefit or PTS #: 7743
 *  Testing: Custom test with varing offest and transfer sizes
 *  Module(s): norma/ipc_input.c norma/ipc_unreliable.c
 *
 * Revision 1.15.2.2  1993/12/15  01:41:36  joel
 *  Reviewer: andyp, cfleck
 *  Risk:medium
 *  Benefit or PTS #: 7434
 *  Testing: ORNL MCAT .compute and .service test scripts
 *  Module(s): ipc_unreliable.c
 *  R1.2 RTI approved.
 *
 * Revision 1.15.2.1  1993/12/08  18:19:57  rkl
 *  Removed the ability to "lock on" to a node using a filter value.
 *
 *  Reviewer: Joel
 *  Risk: moderate
 *  Benefit or PTS #: 7317
 *  Testing: ORNL PFS acceptance tests
 *  Module(s): msgp_ipc.c mcmsg_ipc.c mcmsg_ipc.h ipc_net.c ipc_unreliable.c
 *
 * Revision 1.15  1993/11/18  23:49:09  rkl
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
 * Revision 1.14  1993/10/20  16:02:35  rkl
 * Added NORMA Fast OOL capability.  When a single OOL type that is larger
 * than a VM page and <= a page list is sent, it will attempt to use the
 * Fast OOL transfer method.  This method allows all pages except for the
 * last to be sent without waiting for an ACK from the receiver.  If the
 * receiver does not have the resources to support the transfer, it will
 * be done in the regular way.
 *
 * Revision 1.13  1993/09/28  18:04:23  andyp
 * Update for the 1.2 release.
 *
 *
 *	- Pass an additional parameter to netipc_page_put
 *	identifying the caller, for debugging purposes.
 *
 *	- Add node numbers to pcs queue print-outs.
 *
 *	- Revamp norma_critical_message to distinguish between
 *	critical (special action required by receiver) and
 *	super-critical (special action required by sender
 *	as well as receiver) messages.  Define the NORMA
 *	norma_ipc_node_set send-side message as super-critical.
 *
 *	- Change callers to norma_critical_message.
 *	[alanl@osf.org]
 *
 * Revision 1.12  1993/07/28  16:12:46  stans
 *    Enable dynamic allocation of NORMA pcs structures.
 *
 * Revision 1.11  1993/07/22  02:20:53  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.10  1993/06/30  22:50:57  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.9  1993/06/25  22:29:23  andyp
 * Merged up to the latest from OSF's tree.  Recovered OSF's log messages
 * where needed.  Added the fixes for CPU idle times.  Added some Paragon
 * instrumentation for billing time spent in ipc_wait.c:norma_ipc_kmsg_accept()
 * as idle time.
 *
 * Revision 1.8  1993/06/22  17:38:45  andyp
 * Workaround the backoff problems by setting the defaults for Paragon
 * to pre-backoff code settings per rwd@osf.org.
 *
 * Revision 1.7  1993/06/21  23:57:33  andyp
 * New backoff logic from OSF to avoid Langermania!  Merged in our
 * local changes.  Recovered OSF's log.
 *
 * Revision 1.6  1993/06/09  01:40:33  terry
 * source sync with OSF
 *
 * Revision 1.5  1993/05/11  17:31:01  andyp
 * Dropped MAX_NUM_NODES down from 2048 to 1024 (we need the memory).
 * Added PARAGON860 to a #if to workaround a panic condition that
 * occurs when netipc_packet_allocate() stumbles into an empty list.
 * The workaround is reckless -- it might call zalloc() from interrupt
 * level.  See PTS #5093 for more information.
 *
 * Revision 1.4  1993/04/27  20:46:07  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.3  1993/04/27  00:19:46  dleslie
 * Patch release of April 23
 *
 * Revision 1.6  1993/04/16  05:11:21  SSD
 * third code drop for page flow control code.
 *
 * Revision 1.4  1993/04/12  23:57:58  andyp
 * Rather than failing an assertion in netipc_recv_resume() when
 * receiving a RESUME_QUERY from a node we don't think is suspended,
 * issue a warning message and bump a counter.  Also, don't send
 * the useless 4-byte data field when sending from netvec_a -- it
 * can lead to poor alignment conditions within the low-level
 * Paragon transport (which leads to poor performance).
 *
 * Revision 1.3  1993/04/12  03:24:05  SSD
 * second code drop for pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_unreliable.c,v
 * Revision 1.3.4.14  1993/06/10  19:20:34  alanl
 * 	Make the maximum backoff ticks a boot-time settable parameter,
 * 	and lower the default value.  [sjs, alanl]
 * 	Also:  abstract backoff computations; detect outgoing CTL
 * 	collisions and prevent backoff from increasing when CTL_SYNC
 * 	and/or CTL_RESUME are dropped due to such a collision.  [alanl]
 * 	Add netipc_pretend_no_ack so that a suspend that can't be
 * 	handled due to low memory can be backed out of the system.  [alanl]
 * 	[1993/06/10  19:19:08  alanl]
 *
 * Revision 1.3.4.13  1993/06/02  20:08:01  alanl
 * 	Added PROXY_DATA_WRITE_COMPLETED to critical messages on a port
 * 	for message reordering.  [sjs]
 * 	[1993/06/02  20:07:28  alanl]
 * 
 * Revision 1.3.4.12  1993/05/26  18:57:43  alanl
 * 	Expand debugging output slightly.
 * 	[1993/05/26  18:06:47  alanl]
 * 
 * Revision 1.3.4.11  1993/05/26  14:59:29  rwd
 * 	Backoff that scales to anysize for RESUME/QUERY and SYNC.
 * 	[93/05/21            rwd]
 * 
 * 	Added exponential backoff to SYNC and RESUME_QUERY
 * 	packet sending.
 * 	[93/05/10            rwd]
 * 
 * Revision 1.3.4.10  1993/05/05  12:14:50  dwm
 * 	Remove newly bogus assertion from norma_ipc_ack().
 * 	[1993/05/05  12:11:09  dwm]
 * 
 * Revision 1.3.4.9  1993/05/03  19:13:24  alanl
 * 	Drop NETIPC_TYPE_INVALID packets.  Add assertion verifying
 * 	state of netipc_recv vector.
 * 	[1993/05/03  19:06:39  alanl]
 * 
 * Revision 1.3.4.8  1993/04/26  18:55:21  alanl
 * 	Under one circumstance, netipc_queue_ctl can in fact be called
 * 	with CTL_NONE as its message.  Fix the assertion.
 * 	[1993/04/26  18:54:53  alanl]
 * 
 * Revision 1.3.4.7  1993/04/15  22:45:20  alanl
 * 	Paging flow control (NORMA_VM).  NORMA IPC must provide
 * 	priority handling for memory_object_data_write_completed.
 * 	This message must be accepted even when memory is low to
 * 	allow pageout to complete.
 * 	Also:  termination can cause norma_ipc_node_set messages
 * 	in the critical paging path (fix is needed even without
 * 	the addition of paging flow control).
 * 	Also:  migrate details of recognizing special case NORMA
 * 	IPC messages completely into this module.
 * 	Also:  fix netipc_queue_ctl to trace node number.  [alanl]
 * 	Dump NORMA state from debugger.  [stans@ssd.intel.com, alanl]
 * 	[1993/04/15  22:11:02  alanl]
 * 
 * Revision 1.3.4.6  1993/04/08  22:09:35  dwm
 * 	Pick up latest Intel changes.  Raise MAX_NUM_NODES.
 * 	[1993/04/08  18:35:12  dwm]
 * 
 * Revision 1.3.4.5  1993/03/03  21:20:17  dwm
 * 	Correct fastpath handling in _netipc_recv_intr(). (alanl)
 * 	[1993/03/03  21:16:42  dwm]
 * 
 * Revision 1.3.4.4  1993/02/02  13:39:47  dwm
 * 	Add vm_page_gobble() calls where needed. (dwm bug #542)
 * 
 * 	Bounded CTL types with CTL_MAX.
 * 	Imported NORMA14 packet statistics, replacing
 * 	older counters (still crude).
 * 	Extracted NORMA_IPC_PULL_RECEIVE in favor of
 * 	a common definition in norma/ipc_net.h.
 * 	Added synchronization note for netipc_suspended.
 * 	Revamped RESUME_QUERY timeout/generation.
 * 	Fixed suspend/resume corner case -- missed
 * 	checking for a suspended connection when
 * 	acknowledging a duplicate packet.
 * 	Removed unneeded mipscache_Iflush. (alanl)
 * 	[1993/02/01  22:36:18  dwm]
 * 
 * Revision 1.3.4.3  1993/01/20  02:48:20  dwm
 * 	Add some counters for statistics gathering,
 * 	re: redesign of norma.  [ alanl ]
 * 	[1993/01/20  02:44:25  dwm]
 * 
 * Revision 1.3.4.2  1993/01/06  16:39:15  dwm
 * 	Add a bit of range checking to _netipc_recv_intr() (bug #529).
 * 	[1993/01/06  16:38:50  dwm]
 * 
 * Revision 1.3  1992/12/01  00:07:25  robert
 * 	integrate changes for norma_14/norma_13_16
 * 
 * 	Dave Mitchell (dwm) at Open Software Foundation 23-Nov-92
 * 	In _netipc_recv_intr, use tr for checksum mismatch, not printf.
 * 	Add netipc_checksum_counter.
 * 	[1992/11/30  17:42:07  robert]
 * 
 * Revision 1.2  1992/11/25  01:15:01  robert
 * 	integrate changes below for norma_14
 * 
 * 	Alan Langerman (alan) at Open Software Foundation 17-Nov-92
 * 	Missed one place where the receiver must save the sequence
 * 	number of a suspended packet.  If a connection was already
 * 	suspended, a newly dropped non-special packet resulting in
 * 	another suspend operation on the same connection could
 * 	erroneously be acked if the suspend message was lost and the
 * 	sender timed out and sent a SYNC.  Fixed by moving the code
 * 	to save the sequence number into pcs_suspend_source().
 * 	Also:  slightly improve netipc_pcs_print formatting and
 * 	remove irrelevant CTL_QUENCH code.
 * 
 * 	Alan Langerman (alan) at Open Software Foundation 17-Nov-92
 * 	Remove unneeded trace code from norma_ipc_ack().
 * 
 * 	Dave Mitchell (dwm) at Open Software Foundation 11-Nov-92
 * 	1.  In _netipc_recv_intr, the tr5 for CTL messages had
 * 	an incorrect format string.  2.  Trace netipc_drain_unacks.
 * 	3.  Additional formatting for trs about CTL messages.
 * 	[1992/11/18  23:30:42  robert]
 * 
 * 	integrate changes below for norma_14
 * 
 * 	Alan Langerman (alan) at Open Software Foundation
 * 	The previous suspend/sync fix opened up a new problem.
 * 	The receiver could enter a suspended state and later
 * 	encounter a SYNC for a dropped packet.  By always
 * 	responding with a SYNC, the receiver forced the sender
 * 	to requeue the packet for transmission later.  But if
 * 	the packet in question was special, the receiver may
 * 	actually have accepted it -- just not quickly enough
 * 	to generate the ACK.  The receiver will eventually
 * 	wind up with TWO copies of the packet when the sender
 * 	finally resumes the connection.  The solution is to
 * 	remember the ID of the last suspended packet.  On a
 * 	SYNC, if the IDs match then an ACK is needed because
 * 	the packet was dropped by the receiver.  Otherwise,
 * 	the packet was accepted.
 * 	[1992/11/13  19:39:26  robert]
 * 
 * 	fix history
 * 	[1992/11/09  21:57:26  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:47:33  robert]
 * 
 * Revision 0.0  92/10/21            alan
 * 	Suspend/sync race.  The sender can miss a suspend message
 * 	and send a sync.  The receiver then replies with an ack
 * 	because the pcs state indicates that the last_received packet
 * 	was accepted.  In fact, the receiver dropped that packet when
 * 	it sent the suspend.  Check for this case rather than always
 * 	responding to a sync with an ack.  Also update tr's.
 * 	[92/10/21            alan]
 * 
 * Revision 0.0  92/09/11            jsb
 * 	Added printf7 for short printfs of received message ids.
 * 	Fixed mips phystokv definitions.
 * 	Clarified netipc_ticks initialization. Added mipscache_Iflush
 * 	to _netipc_recv_intr. Added norma msg profiling support,
 * 	under NORMA_MSG_TRACE (tied to MACH_ASSERT for now).
 * 
 * 	Revision 1.1  1992/11/05  21:00:02  robert
 * 	Initial revision
 * 	[92/09/11            jsb]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.8  92/09/15  17:34:44  jeffreyh
 * 	Converted debug logging from ntr to tr.
 * 	[92/09/10            alanl]
 * 
 * 	Deadlock prevention.  Paging messages can become stuck
 * 	behind otherwise-useless transmissions on the netipc_sends
 * 	queue.  Rotate the queue a little bit each time we go
 * 	through netipc_send_intr.  [With Alanl.]
 * 	More ntr's
 * 	[92/08/28            jeffreyh]
 * 
 * 	Surgery on the suspend/resume protocol.  Extensive revisions
 * 	by Jeffrey Heller and Alan Langerman.
 * 	[92/08/20            alanl]
 * 
 * 	Added debugging printf
 * 	[92/07/02            jeffreyh]
 * 
 * 	Use pageable copy objects for large NORMA messages.
 * 	Small messages continue to be handled using page list
 * 	technology at interrupt level -- in theory, minimizing
 * 	latency.  Pages belonging to messages larger than a
 * 	cutoff limit are sent to the netipc_thread for reassembly.
 * 	Choosing the method is done by _netipc_recv_intr.  Call
 * 	the function netipc_input_init to initialize input-side
 * 	data structures.  Interfaces for norma_remote_to_kmsg,
 * 	norma_deliver_kmsg, and norma_deliver_page all change.
 * 	Eliminate defines for netipc_special_kmsg packet filter
 * 	cases that no longer exist.
 * 	[92/06/02            alanl]
 * 
 * 	Alter netipc_special_kmsg packet filter to pass only messages
 * 	inbound to the default pager when memory runs low.
 * 	[92/05/26            alanl]
 * 
 * Revision 2.1.2.7  92/06/24  18:01:59  jeffreyh
 * 	Changes to allow suspending of in transit kmsgs in 
 * 	low memory situations.
 * 		1) Added pcs_suspend queue field to the pcs
 * 		2) Added CTL_RESUME control message
 * 		3) Added  netipc_recv_resume to handle 
 * 		  the CTL_RESUME messages
 * 		4) Added netipc_send_resume to send a CTL_RESUME
 * 		  to all waiting nodes
 * 
 * 	Big changes to the norma_special_kmsg routine. It now
 * 	only allows packets bound for the default pager, and
 * 	pull_receive messages.
 * 	[92/06/10            jeffreyh]
 * 
 * Revision 2.1.2.6  92/05/27  00:53:30  jeffreyh
 * 	Enhance netipc_special_kmsg to deal with pages. A better version 
 * 	should be on the way that checks destination instead of 
 * 	message type.
 * 
 * Revision 2.1.2.5.2.2  92/05/08  10:50:43  jeffreyh
 * 	Use the new netipc_page_list reservation system to
 * 	coordinate the use of memory between _netipc_recv_intr
 * 	and norma_ipc_ack.
 * 	[92/05/07            alanl]
 * 
 * Revision 2.1.2.5.2.1  92/04/28  16:01:41  jeffreyh
 * 	Return after doing a retransmit. 
 * 	[92/04/13            dlb]
 * 
 * Revision 2.1.2.5  92/04/08  15:46:27  jeffreyh
 * 	Removed i860 debugging stuff
 * 	[92/04/08            andyp]
 * 
 * 	Revert buffer allocation logic in norma_ipc_ack().
 * 	[92/04/06            dlb]
 * 
 * 	Fix timer queue scrambling bug by having both _netipc_timeout_intr
 * 	and _netipc_recv_nack check that the timer is queued before dequeueing.
 * 	[92/04/03            dlb]
 * 
 * 	Bodies of queue_iterate macros were deleting current queue
 * 	element in _netipc_timeout_intr, netipc_self_unstop, and
 * 	_netipc_send_intr.  Replaced with C code that works right.
 * 	Also reformatted _netipc_timeout_intr and added comments.
 * 	[92/04/03            dlb]
 * 
 * Revision 2.1.2.4  92/03/28  10:12:20  jeffreyh
 * 	Always allocate new buffer in norma_ipc_ack().  Error
 * 	cases now keep the buffer for later cleanup.
 * 	[92/03/20  14:12:57  dlb]
 * 
 * Revision 2.1.2.3  92/02/21  11:25:08  jsb
 * 	Reduced netipc_ticks to something more reasonable.
 * 	[92/02/20  14:01:35  jsb]
 * 
 * Revision 2.1.2.2  92/02/18  19:16:47  jeffreyh
 * 	[intel] added callhere debugging stuff for iPSC.
 * 	[92/02/13  13:07:35  jeffreyh]
 * 
 * Revision 2.1.2.1  92/01/21  21:53:02  jsb
 * 	Use softclock timeouts instead of call from clock_interrupt.
 * 	[92/01/17  12:20:09  jsb]
 * 
 * 	Added netipc_checksum flag for use under NETIPC_CHECKSUM conditional.
 * 	Never panic on checksum failure. Enter send queue when remembering
 * 	nack when netipc_sending is true. Forget nack when receiving ack.
 * 	Added netipc_receiving flag.
 * 	[92/01/16  22:27:47  jsb]
 * 
 * 	Added conditionalized checksumming code. Eliminated redundant and
 * 	unused pcs_last_unacked field. Added pcs_nacked field to avoid
 * 	throwing away nacks when netipc_sending is true. Removed panics
 * 	on obsolete seqids. Eliminated ancient obsolete comments.
 * 	Added netipc_start_receive call, which takes account of ipc_ether.c
 * 	now modifying vec[i].size to indicate actual received amounts.
 * 	[92/01/14  21:53:20  jsb]
 * 
 * 	Removed netipc_packet definitions and references. Changes for new
 * 	interface with norma/ipc_output.c (look there for explanation).
 * 	[92/01/13  20:21:54  jsb]
 * 
 * 	Changed to use pcs structures and queue macros.
 * 	Added netipc_pcs_print. De-linted.
 * 	[92/01/13  10:19:19  jsb]
 * 
 * 	First checkin. Simple protocol for unreliable networks.
 * 	[92/01/11  17:39:37  jsb]
 * 
 * 	First checkin. Contains functions moved from norma/ipc_net.c.
 * 	[92/01/10  20:47:49  jsb]
 * 
 */
/* CMU_ENDHIST */
/*
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
 *	File:	norma/ipc_send.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1990
 *
 *	Functions to support ipc between nodes in a single Mach cluster.
 */

#include <kern/macro_help.h>
#include <kern/queue.h>
#include <kern/time_out.h>
#include <norma/ipc_net.h>
#include <norma/tr.h>
#include <machine/mach_param.h>	/* HZ */

/*
 *	Memory shortages and flow control.
 *
 *	When memory is low, the receiving node can return
 *	KERN_RESOURCE_SHORTAGE to the sending node.  The
 *	receiving node is still obligated to accept a special
 *	class of paging-related messages (see netipc_special_kmsg()
 *	in this file).
 *
 *	The sending node is responsible for suspending the
 *	current transmission and starting a new transmission
 *	from a new port if one is available (see ipc_output.c).
 *	The sending node can't shut up completely because special-
 *	case paging messages may be pending on ports queued behind
 *	the original port that encountered the KERN_RESOURCE_SHORTAGE.
 *
 *	We define the following, separate finite state machines for
 *	the input and output cases.  We intend to define a robust
 *	protocol that eliminates the need for complex handshaking.
 *
 *	XXX To be filled in, but probably not before checking it in
 *	and leaving anyone else trying to understand this code
 *	as confused as we were. XXX
 */

#define	CTL_NONE		0L
#define	CTL_ACK			1L
#define	CTL_NACK		2L
#define	CTL_SYNC		3L
#define	CTL_RESUME		4L
#define	CTL_NEW_INCARNATION	5L
#define	CTL_MAX			6L

#define	RESUME_ACK		77L
#define	RESUME_QUERY		99L

/*
 * MAX_BACKOFF_TICKS is the absolute highest number of backoffs that can
 * be set.  max_backoff_ticks (the lower case variable) is the maximum
 * number used by the system and is a tunable parameter.
 */
#if	PARAGON860
#define	MAX_BACKOFF_TICKS_INIT	8
#else	PARAGON860
#define	MAX_BACKOFF_TICKS_INIT	6	/* 2^6 ==> 640 milliseconds */
#endif	PARAGON860

#define MAX_BACKOFF_TICKS	20
int max_backoff_ticks = MAX_BACKOFF_TICKS_INIT;	
int c_netipc_syncb[MAX_BACKOFF_TICKS];
int c_netipc_resumeb[MAX_BACKOFF_TICKS];

/*
 * The following parameters control the SYNC and RESUME/QUERY timeouts
 * and their backoff.  NETIPC_TICKS and NETIPC_RESUME_TIMEOUT_TICKS
 * determine the resolution of the timeout.  A value of 1 translates
 * to 10msec, the system clock tick time.  This is the value of the
 * smallest timeout and the separation that all nodes will have
 * from one another if they are all retrying and sufficient backoff
 * has occured.  If it is determined that two nodes sending messages
 * 10msec appart is not a sufficient time interval, increase this
 * paramater.
 *
 * The backoff for retrying SYNC and RESUME/QUERY is at the power
 * of 2.  Each backoff will also divide the number of nodes falling on
 * a single tick by 2.  With NETIPC_SYNC_BACKOFF and NETIPC_RESUME_BACKOFF
 * set at (1), each node will be sending on a different tick in
 * LOGbase2(NODES) backoffs.  If this is found not to be quick enough,
 * the _BACKOFF parameter can be increased to have a constant effect
 * on how many backoffs are required.  If this parameter is changed to
 * C, then the time to unique packets per tick becomes LOGbase2(NODES/C)
 * or LOGbase2(NODES) - LOGbase2(C).
 */

#define NETIPC_CONGESTION_FACTOR (16)
int netipc_congestion_factor = NETIPC_CONGESTION_FACTOR;
int netipc_congestion_counter = NETIPC_CONGESTION_FACTOR;

#define NETIPC_SYNC_BACKOFF (1)
int netipc_sync_backoff = NETIPC_SYNC_BACKOFF;
#define netipc_sync_backoffc \
  netipc_sync_backoff * netipc_congestion()

#define NETIPC_RESUME_BACKOFF (1)
int netipc_resume_backoff = NETIPC_RESUME_BACKOFF;

#define	NETIPC_TICKS	(1)
int netipc_ticks = NETIPC_TICKS;

#define	NETIPC_RESUME_TIMEOUT_TICKS	(1)
int		netipc_resume_timeout_ticks = NETIPC_RESUME_TIMEOUT_TICKS;

#if	DYNAMIC_NUM_NODES
	/* see norma/ipc_net.h */
#else	/* DYNAMIC_NUM_NODES */

#if	PARAGON860
#if	MCMSG
#define	MAX_NUM_NODES		1024	/* XXX */
#else	MCMSG
#define	MAX_NUM_NODES		1	/* XXX */
#endif	MCMSG
#else	PARAGON860
#define	MAX_NUM_NODES		256	/* XXX */
#endif	PARAGON860

#endif	/* DYNAMIC_NUM_NODES */

extern unsigned long	netipc_page_delivery_limit;
struct netipc_hdr	netipc_recv_hdr;

extern int Noise6;
extern int Noise7;

extern void		netipc_recv_ack_with_status();
extern ipc_port_t	norma_port_lookup_locked();
extern void		norma_deliver_page();
extern void		norma_deliver_page_slowly();

boolean_t		netipc_receiving = FALSE;
struct netvec		netvec_r[2];

vm_page_t		netipc_recv_page;
extern vm_page_t	netipc_page_grab();
extern boolean_t	netipc_reserve_page();
extern boolean_t	netipc_reserve_page_force();
extern void		netipc_reserve_cancel();
extern void		netipc_page_init();

unsigned int netipc_in_ctl_stats[CTL_MAX];
unsigned int netipc_in_type_stats[NETIPC_TYPE_MAX];
unsigned int netipc_out_ctl_stats[CTL_MAX];
unsigned int c_netipc_ctl_collisions[CTL_MAX];

#if	TRACE_BUFFER || MACH_KDB || 1
static char		*ctl_to_ascii();
#endif

/* XXX must make sure seqids never wrap to 0 */

extern unsigned long	node_incarnation;

#if	DYNAMIC_NUM_NODES
	/* moved to ipc_net.h */
#else	/* DYNAMIC_NUM_NODES */

typedef struct retry {
	int	ticks;
	int	backoff;
} retry_t;

#define	MAX_WINDOW_SIZE	1

#define	WX		(MAX_WINDOW_SIZE + 1)

/*
 * Protocol control structure
 * (struct pcb is already in use)
 */
typedef struct pcs	*pcs_t;
#define	PCS_NULL	((pcs_t) 0)
struct pcs {
	unsigned long	pcs_remote;
	unsigned long	pcs_last_received;
	unsigned long	pcs_last_sent;
	unsigned long	pcs_nacked;
	unsigned long	pcs_unacked_packetid[WX];
	unsigned long	pcs_incarnation;
	unsigned long	pcs_new_incarnation;
	unsigned long	pcs_ctl;
	unsigned long	pcs_ctl_seqid;
	kern_return_t	pcs_ctl_status;
	unsigned long	pcs_ctl_data;
	boolean_t	pcs_suspend_sent;	/* out:  suspend was sent */
#if	NORMA_FAST_OOL
	boolean_t	pcs_fast_ool;		/* out:  fast ool in progress */
#endif	NORMA_FAST_OOL
	unsigned long	pcs_suspend_seqid;	/* suspended packet id */
	retry_t		pcs_resume;
	retry_t		pcs_ticks;
	queue_chain_t	pcs_timer;
	queue_chain_t	pcs_send;
	queue_chain_t	pcs_suspend;		/* in:  remote suspended us  */
};
#endif  /* DYNAMIC_NUM_NODES */

queue_head_t netipc_timers;
queue_head_t netipc_sends;
queue_head_t netipc_suspended;
/*
 *	Synchronization note on netipc_suspended:
 *	manipulations of this list must be protected
 *	by splsched or higher, just as with
 *	netipc_timers.  This is a HACK to allow
 *	softclock-time processing of the suspended
 *	list to send RESUME_QUERYs.
 */

#if	DYNAMIC_NUM_NODES

struct pcs *netipc_pcs;

#else	/* DYNAMIC_NUM_NODES */

struct pcs netipc_pcs[MAX_NUM_NODES];

#endif	/* DYNAMIC_NUM_NODES */

/*
 *	These macros enqueue and dequeue a pcs w.r.t.
 *	the netipc_suspended list.  Note that they check
 *	whether the pcs is already enqueued.
 */
#define	pcs_unsuspend_self(pcs)						\
MACRO_BEGIN								\
	if (queue_tabled(&(pcs)->pcs_suspend))				\
		queue_untable(&netipc_suspended, (pcs),			\
			      pcs_t, pcs_suspend);			\
MACRO_END
#define	pcs_suspended(pcs)						\
	(queue_tabled(&(pcs)->pcs_suspend))

/*
 *	Send SUSPEND and RESUME messages to a source node.
 */
#define	pcs_suspend_source(pcs, seqid)					\
MACRO_BEGIN								\
	pcs->pcs_suspend_sent = TRUE;					\
	pcs->pcs_suspend_seqid = seqid;					\
	pcs->pcs_last_received = seqid;					\
	netipc_queue_ctl((pcs), CTL_ACK, (seqid),			\
			 KERN_RESOURCE_SHORTAGE, node_self());		\
MACRO_END

#define	pcs_resume_source(pcs, data)					\
MACRO_BEGIN								\
	netipc_queue_ctl((pcs), CTL_RESUME, (pcs)->pcs_last_received,	\
			 KERN_SUCCESS, data);				\
MACRO_END


/*
 * Counters and such for debugging
 */
int c_netipc_timeout	= 0;
int c_netipc_retry_k	= 0;
int c_netipc_retry_m	= 0;
int c_netipc_retry_p	= 0;
int c_netipc_retry_o	= 0;

#if	PARAGON860
/*
 *	andyp@ssd.intel.com
 *
 *	Don't send the "send_data_a" from netvec_a on a Paragon; it's
 *	completely unused by everything and can lead to triggering poor
 *	alignment (and poor performance) in the low-level transport.
 */
#define	NETVEC_A_SIZE	1
#else	PARAGON860
#define	NETVEC_A_SIZE	2
#endif	PARAGON860

struct netvec		netvec_a[NETVEC_A_SIZE];
struct netipc_hdr	send_hdr_a;
#if	NETVEC_A_SIZE > 1
unsigned long		send_data_a = 0xabcd9876;
#endif	NETVEC_A_SIZE > 1

boolean_t		netipc_sending;

#if	MACH_ASSERT
#define	NORMA_MSG_TRACE	1	/* Turn on kmsg profiling, at bottom */
#endif

int calc_backoff(int backoff, int ticks)
{
	return (-(backoff << ticks) -
		(node_self() % (backoff << ticks)));
}


void init_retry(retry_t *r, int backoff)
{
	r->backoff = 0;
	r->ticks = calc_backoff(backoff, r->backoff);
}


void inc_retry(retry_t *r, int backoff)
{
	if (++(r->backoff) >= max_backoff_ticks)
		r->backoff = max_backoff_ticks - 1;
	r->ticks = calc_backoff(backoff, r->backoff);
}


void dec_retry(retry_t *r, int backoff)
{
	if (r->backoff > 0)
		r->backoff--;
	r->ticks = calc_backoff(backoff, r->backoff);
}


int netipc_congestion()
{
	if (netipc_congestion_factor)
		return (netipc_congestion_counter / netipc_congestion_factor);
	else
		return 1;
}

#if	NORMA_FAST_OOL
/*
 *  Mark fast OOL flag in PCS true.
 */
void
netipc_fast_ool_start(remote)
unsigned long	remote;
{
	netipc_pcs[remote].pcs_fast_ool = TRUE;
}

/*
 *  Called when the last page of an OOL message has been received.
 *  This routine can and will be called even when we aren't doing a
 *  fast OOL transfer.  To make it fast and simple, we unconditionally
 *  clear the pcs_fast_ool flag.
 */
void
netipc_fast_ool_done(remote)
unsigned long	remote;
{
	netipc_pcs[remote].pcs_fast_ool = FALSE;
}

/*
 *  Return the sending node number.
 */
unsigned long
norma_get_remote()
{
	/*
	 *  This seems a little cheesy to me.
	 */
	return netipc_recv_hdr.remote;
}

/*
 *  Return fast_ool boolean
 */
boolean_t
is_fast_ool(remote)
unsigned long	remote;
{
	return (netipc_pcs[remote].pcs_fast_ool);
}

/*
 *  Clear the pcs_unacked_packetid as part of doing a pseudo ACK.
 */
pcs_clear_unacked(remote, seqid)
unsigned long	remote;
unsigned long	seqid;
{
	pcs_t pcs = &netipc_pcs[remote];

	pcs->pcs_unacked_packetid[ seqid % WX ] = 0;
	if (pcs->pcs_nacked == seqid) {
		pcs->pcs_nacked = 0L;
	}
}

/*
 *  XXX Currently assumes that the ool data will all be from one ool
 *  XXX data type.
 */
compute_pages(ool_bytes)
vm_size_t	ool_bytes;
{
	return (atop(ool_bytes + (PAGE_SIZE-1)));
}

#define	INVALID_FAST_OOL_PACKETID  (~0)
unsigned long	netipc_fast_ool_packetid = INVALID_FAST_OOL_PACKETID;

#endif	NORMA_FAST_OOL


#if	NETIPC_CHECKSUM
int netipc_checksum = 1;
int netipc_checksum_print = 1;

#define	longword_aligned(x) ((((unsigned long)(x)) & (sizeof(long)-1)) == 0)

unsigned long
netipc_compute_checksum(vec, count)
	register struct netvec *vec;
	unsigned int count;
{
	int i;
	register unsigned long checksum = 0;
	register unsigned long *data;
	register int j;
	
	if (! netipc_checksum) {
		return 0;
	}
	for (i = 0; i < count; i++) {
		data = (unsigned long *) DEVTOKV(vec[i].addr);
		assert(longword_aligned((unsigned long) data));
		assert(longword_aligned(vec[i].size));
		for (j = vec[i].size / sizeof(*data) - 1; j >= 0; j--) {
			checksum += data[j];
		}
	}
	return checksum;
}
#endif	NETIPC_CHECKSUM

#if	mips
#include <mips/mips_cpu.h>
vm_offset_t
phystokv(phys)
	vm_offset_t phys;
{
	return PHYS_TO_K0SEG(phys);	/* cached */
}
#endif

/*
 * Use queue_chain_t to record whether we are already on the queue.
 * Use bogus field that should cause a page fault if we are not.
 */

#define BOGUS_QUEUE_ELEMENT	((queue_entry_t) 0xffffff00)

#define queue_init_untabled(q)			\
MACRO_BEGIN					\
	(q)->prev = BOGUS_QUEUE_ELEMENT;	\
	(q)->next = BOGUS_QUEUE_ELEMENT;	\
MACRO_END
	

#define queue_untable(head, elt, type, field)	\
MACRO_BEGIN					\
	queue_remove(head, elt, type, field);	\
	queue_init_untabled(&(elt)->field);	\
MACRO_END

#define queue_tabled(q)		((q)->prev != BOGUS_QUEUE_ELEMENT)

netipc_cleanup_send_state(pcs)
	pcs_t pcs;
{
	int i;

	/*
	 * Clean up connection state.
	 */
	pcs->pcs_last_sent = 0;
	for (i = 0; i < WX; i++) {
		pcs->pcs_unacked_packetid[i] = 0;
	}
	if (queue_tabled(&pcs->pcs_timer)) {
		queue_untable(&netipc_timers, pcs, pcs_t, pcs_timer);
	}
	if (queue_tabled(&pcs->pcs_send)) {
		queue_untable(&netipc_sends, pcs, pcs_t, pcs_send);
	}
	pcs_unsuspend_self(pcs);
	pcs->pcs_suspend_sent = FALSE;

	/*
	 * XXX
	 * This should be called by norma ipc layer
	 */
	netipc_cleanup_incarnation_complete(pcs->pcs_remote);
}

/*
 * A timer for a remote node gets set to 1 when a message is sent
 * to that node. Every so many milliseconds, the timer value is
 * incremented. When it reaches a certain value (currently 2),
 * a sync message is sent to see whether we should retransmit.
 *
 * The timer SHOULD be set to 0 when the message is acknowledged.
 */

int
netipc_unacked_seqid(pcs)
	register pcs_t pcs;
{
	register unsigned long seqid;

	seqid = pcs->pcs_last_sent - (MAX_WINDOW_SIZE - 1);
	if ((long) seqid < 0) {
		seqid = 0;
	}
	for (; seqid <= pcs->pcs_last_sent; seqid++) {
		if (pcs->pcs_unacked_packetid[seqid % WX]) {
			return seqid;
		}
	}
	return 0;
}

extern int netipc_timeout_intr();

struct timer_elt netipc_timer_elt;

netipc_set_timeout()
{
	netipc_timer_elt.fcn = netipc_timeout_intr;
	netipc_timer_elt.param = 0;
	set_timeout(&netipc_timer_elt, netipc_ticks);
}

#if	NORMA_FAST_OOL
int	c_fast_ool_timeouts = 0;
#endif	NORMA_FAST_OOL

_netipc_timeout_intr()
{
	register pcs_t pcs, next_pcs;
	register unsigned long seqid;

	pcs = (pcs_t) queue_first(&netipc_timers);
	while(!queue_end(&netipc_timers, (queue_entry_t) pcs)) {
	    next_pcs = (pcs_t) queue_next(&pcs->pcs_timer);

#if	NORMA_FAST_OOL
	    /*
	     *  Fast OOL transfers can take longer than a timeout and can't
	     *  deal with contorl messages during a transfer so if this PCS
	     *  is in the middle of a fast OOL transfer, skip it.
	     */
	    if (pcs->pcs_fast_ool == TRUE)
		c_fast_ool_timeouts++;
	    else
#endif	NORMA_FAST_OOL
	    {
	    /*
	     *	If pcs_ticks is currently 0, this is the first
	     *	timeout interrupt since queueing the packet -- ignore it.
	     *  If it is less then zero, then we have failed previously
             *  and it is not yet time to retry.
	     */	
	    if (pcs->pcs_ticks.ticks++ > 0) {

		/*
		 *	Look for an unacked sequence id.  If we can't find
		 *	one, then nothing has timed out (common case).
		 */
		assert(pcs->pcs_nacked == 0L);
		seqid = netipc_unacked_seqid(pcs);
		if (seqid == 0) {
		    if (queue_tabled(&pcs->pcs_timer)) {
			queue_untable(&netipc_timers, pcs, pcs_t, pcs_timer);
		    }
		}
		else {

		    /*
		     * Something has timed out. Send a sync.
		     * XXX (for just first seqid? How about a bitmask?)
		     */
		    c_netipc_timeout++;
		    c_netipc_syncb[pcs->pcs_ticks.backoff]++;
		    inc_retry(&pcs->pcs_ticks, netipc_sync_backoffc);
		    printf2("timeout %d\n", pcs->pcs_remote);
		    netipc_queue_ctl(pcs, CTL_SYNC, seqid, KERN_SUCCESS, 0L);
		}
	      }
	    }	/* !pcs_fast_ool */
	    pcs = next_pcs;
	}

	netipc_set_timeout();
}

/*
 * Should not panic, since a bad seqid is the sender's fault.
 */
boolean_t
netipc_obsolete_seqid(pcs, seqid)
	register pcs_t pcs;
	unsigned long seqid;
{
	if (seqid > pcs->pcs_last_sent) {
		printf("premature seqid %d > %d\n", seqid, pcs->pcs_last_sent);
		panic(" This should not happen!\n");
		return TRUE;
	}
	if (seqid <= pcs->pcs_last_sent - MAX_WINDOW_SIZE) {
		printf5("obsolete seqid %d <= %d\n",
			seqid, pcs->pcs_last_sent - MAX_WINDOW_SIZE);
		return TRUE;
	}
	if (pcs->pcs_unacked_packetid[seqid % WX] == 0) {
		printf5("seqid %d already acked\n", seqid);
		return TRUE;
	}
	return FALSE;
}

/*
 * Called with interrupts blocked, from pcs->pcs_recv_intr,
 * when a nack is received.
 * We will be stopped waiting for an ack; resending does not change this.
 */
netipc_recv_nack(pcs, seqid)
	register pcs_t pcs;
	unsigned long seqid;
{
	assert(netipc_locked());

	/*
	 * Ignore obsolete nacks.
	 */
	if (netipc_obsolete_seqid(pcs, seqid)) {
		return;
	}

	/*
	 *	Always remove the pcs from the timer queue.
	 *	We do this because:
	 *		1.  The protocol is known to be stop-and-wait.
	 *		2.  There can be no other packet outstanding.
	 *		3.  We must clean up the timer queue so that
	 *		    the backoff code will work correctly.
	 */
	if (queue_tabled(&pcs->pcs_timer))
		queue_untable(&netipc_timers, pcs, pcs_t, pcs_timer);

	/*
	 * Even if we cannot retransmit right away, remember the nack
	 * so that we don't send another sync. I have seen sync-nack
	 * loops under certain conditions when nacks are simply dropped.
	 * We also remove ourselves from the timeout queue so that
	 * the timeout routine doesn't have to check for already-nacked
	 * packets. We have to add ourselves to the send queue because
	 * it is send_intr who is now responsible for sending the nack.
	 */
	if (netipc_sending) {
		pcs->pcs_nacked = seqid;
		if (! queue_tabled(&pcs->pcs_send)) {
			queue_enter(&netipc_sends, pcs, pcs_t, pcs_send);
		}
		return;
	}

	/*
	 * We can retransmit now, so do so.
	 */
	assert(pcs->pcs_unacked_packetid[seqid % WX]);
	netipc_send_old(pcs->pcs_unacked_packetid[seqid % WX], seqid);
}

int c_netipc_obsolete_acks = 0;

netipc_recv_ack(pcs, seqid, status, data)
	register pcs_t pcs;
	unsigned long seqid;
	kern_return_t status;
	unsigned long data;
{
	unsigned long packetid;

	assert(netipc_locked());
	/*
	 * Ignore obsolete acks.
	 */
	if (netipc_obsolete_seqid(pcs, seqid)) {
	    c_netipc_obsolete_acks++;
	    netipc_congestion_counter += netipc_congestion_factor + 1;
	    return;
	}

	if (netipc_congestion_counter > netipc_congestion_factor)
	    netipc_congestion_counter--;
	/*
	 * Acknowledge the seqid, possibly freeing the kmsg.
	 * Forget any recorded nack.
	 *
	 * XXX
	 * Should we cancel the timer? It's not really necessary.
	 * If we do, we should check to see whether there are
	 * any other unacked packets, and only cancel the timer
	 * if there aren't.
	 */
	packetid = pcs->pcs_unacked_packetid[seqid % WX];
	pcs->pcs_unacked_packetid[seqid % WX] = 0;
	if (pcs->pcs_nacked == seqid) {
		pcs->pcs_nacked = 0L;
	}
	if (queue_tabled(&pcs->pcs_timer))
		queue_untable(&netipc_timers, pcs, pcs_t, pcs_timer);

	/*
	 * Pass acknowledgement to upper level.
	 */
	netipc_recv_ack_with_status(packetid, seqid, status, data);
}


/*
 *	In the suspend/resume protocol, if we can't handle the
 *	suspend immediately we must fix up enough state so that
 *	a future suspend message (acting on the same packet) will
 *	be handled correctly.  Basically, we must pretend that the
 *	suspend message was never seen.  It doesn't matter that we
 *	don't fix up the pcs_nacked field because eventually we
 *	will time out and send a SYNC for this packet.
 */
unsigned int	c_netipc_pretend_no_ack = 0;
void
netipc_pretend_no_ack(remote, packetid, seqid)
unsigned long	packetid;
unsigned long seqid;
{
	pcs_t	pcs;

	++c_netipc_pretend_no_ack;
	pcs = &netipc_pcs[remote];
	assert(pcs->pcs_unacked_packetid[seqid % WX] == 0);
	pcs->pcs_unacked_packetid[seqid % WX] = packetid;
}


unsigned int	c_netipc_recv_resume_query = 0;
unsigned int	c_netipc_recv_resume_ack = 0;
unsigned int	c_netipc_send_resume_ack = 0;
#if	PARAGON860
unsigned int	c_netipc_recv_curious_resume_query = 0;
#endif	PARAGON860

/*
 *	Called on sending and receiving sides.
 *	XXX fix this comment
 */
netipc_recv_resume(pcs, seqid, status, data)
register pcs_t	pcs;
unsigned long	seqid;
kern_return_t	status;
unsigned long	data;
{
	unsigned long	packetid;
	TR_DECL("netipc_recv_resume");

	tr5("remote 0x%x seqid 0x%x status 0x%x data 0x%x",
	    pcs->pcs_remote, seqid, status, data);
	assert(netipc_locked());
	assert(data == RESUME_QUERY || data == RESUME_ACK);

	if (data == RESUME_QUERY) {
		/*
		 *	If memory is available, send the
		 *	source confirmation that it may
		 *	resume its transmission.
		 */
#if	PARAGON860
		++c_netipc_recv_resume_query;
		if (pcs->pcs_suspend_sent == TRUE) {
			if (netipc_able_continue_recv()) {
				++c_netipc_send_resume_ack;
				pcs_resume_source(pcs, RESUME_ACK);
			}
		} else {
			++c_netipc_recv_curious_resume_query;
			printf("mk warning: netipc_recv_resume: pcs=0x%x, rnode=%d, suspended=%d\n",
				pcs,
				pcs->pcs_remote,
				pcs->pcs_suspend_sent);
			/*
			 *	XXX should probably answer with another
			 *	XXX RESUME_ACK, but that may not be
			 *	XXX the Right Thing To Do.
			 */
		}
#else	PARAGON860
		assert(pcs->pcs_suspend_sent == TRUE);
		++c_netipc_recv_resume_query;
		if (netipc_able_continue_recv()) {
			++c_netipc_send_resume_ack;
			pcs_resume_source(pcs, RESUME_ACK);
		}
#endif	PARAGON860
	} else {
		/*
		 * Pass notification to upper level.
		 */
		++c_netipc_recv_resume_ack;
		pcs_unsuspend_self(pcs);
		netipc_recv_resume_node(pcs->pcs_remote);
	}
}


/* 
 *	This routine queries nodes that have pending
 *	transmissions to determine whether transmissions
 *	may resume.  It is not MP-safe.
 */

boolean_t	netipc_resume_timeout_posted = FALSE;
unsigned int	c_netipc_resume_timeout_called = 0;
unsigned int	c_netipc_resume_timeout_posts = 0;
unsigned int	c_netipc_resume_timeout_activates = 0;
unsigned int	c_netipc_send_resume = 0;
unsigned int	c_netipc_resume_sync_failure = 0;
void		netipc_resume_timeout_activate();


/*
 *	Post RESUME_QUERYs for suspended pcs's.
 *	Contort in disgusting ways to obey the
 *	existing warped synchronization.  If we
 *	can't traverse the suspended list due to
 *	a synchronization failure, post another
 *	timeout to try again later.
 *
 *	As long as the suspend list is non-empty,
 *	continue to post timeouts.
 */
netipc_resume_timeout()
{
	register pcs_t	pcs;
	extern thread_t	netipc_lock_owner; /* gross! */
	register int	found = 0;
	int		s;
	int		invocation;
	TR_DECL("netipc_resume_timeout");

	invocation = ++c_netipc_resume_timeout_called;
	assert(netipc_resume_timeout_posted == TRUE);
	s = sploff();
	if (!netipc_locked()) {
		netipc_intr_lock();
		queue_iterate(&netipc_suspended, pcs, pcs_t, pcs_suspend) {
		    if (pcs->pcs_resume.ticks++ > 0) {
			++c_netipc_send_resume;
			c_netipc_resumeb[pcs->pcs_resume.backoff]++;
			inc_retry(&pcs->pcs_resume, netipc_resume_backoff);
			pcs_resume_source(pcs, RESUME_QUERY);
		    }
		    ++found;
		}
		netipc_intr_unlock();
		tr3("invocation 0x%x sent 0x%x resumes", invocation, found);
	} else {
		tr2("invocation 0x%x missed netipc_lock", invocation);
		++c_netipc_resume_sync_failure;
		found = -1;
	}
	netipc_resume_timeout_posted = FALSE;
	if (found != 0) {
		tr2("invocation 0x%x setting timeout", invocation);
		netipc_resume_timeout_activate();
	}
	splon(s);
}


/*
 *	Kick the netipc_resume_timeout thread into action.
 *	Not MP safe.
 */
struct timer_elt netipc_resume_timer_elt;
void
netipc_resume_timeout_activate()
{
	int	s;

	++c_netipc_resume_timeout_activates;
	s = splhigh();
	if (netipc_resume_timeout_posted == FALSE) {
		netipc_resume_timeout_posted = TRUE;
		++c_netipc_resume_timeout_posts;
		netipc_resume_timer_elt.fcn = netipc_resume_timeout;
		netipc_resume_timer_elt.param = 0;
		set_timeout(&netipc_resume_timer_elt,
			    netipc_resume_timeout_ticks);
	}
	splx(s);
}


void
pcs_suspend_node(remote)
unsigned long	remote;
{
	pcs_t	pcs;

	assert(netipc_locked());
	pcs = &netipc_pcs[remote];
	/*
	 *	No matter what anyone may say, this is a
	 *	stop and wait protocol.
	 */
	assert(!queue_tabled(&pcs->pcs_timer));
	init_retry(&pcs->pcs_resume, netipc_resume_backoff);
	if (!queue_tabled(&(pcs)->pcs_suspend))
		queue_enter(&netipc_suspended, (pcs),
			    pcs_t, pcs_suspend);
	netipc_resume_timeout_activate();
}


/*
 * Called by upper level to indicate that it has something to send,
 * and that we should make an upcall when we can perform that send.
 */
void
netipc_start(remote)
	unsigned long remote;
{
	register pcs_t pcs = &netipc_pcs[remote];

	assert(netipc_locked());
	if (! netipc_sending) {
		boolean_t sending;
		sending = netipc_send_new(remote, pcs->pcs_last_sent + 1);
		assert(sending);
	} else if (! queue_tabled(&pcs->pcs_send)) {
		queue_enter(&netipc_sends, pcs, pcs_t, pcs_send);
	}
}

Xnetipc_next_seqid(remote)
	unsigned long remote;
{
	pcs_t pcs = &netipc_pcs[remote];

	return pcs->pcs_last_sent + 1;
}

/*
 * XXX Still need to worry about running out of copy objects
 */

norma_kmsg_put(kmsg)
	ipc_kmsg_t kmsg;
{
	netipc_thread_lock();
	netipc_page_put(kmsg->ikm_page, FALSE, "norma_kmsg_put");
	netipc_thread_unlock();
}

netipc_cleanup_receive_state(pcs)
	register pcs_t pcs;
{
	pcs->pcs_last_received = 0;
}


/*
 * The pageout paths require special handling.  Thus, we detect
 * any messages bound for the default pager and give them priority.
 * We need to give pagein requests as well as pageout requests
 * this special treatment because pageout requests may be queued
 * behind pagein requests on the default_pager_internal_set.
 */
extern ipc_port_t norma_port_lookup_locked();

extern struct ipc_space	*default_pager_space;
unsigned int	c_netipc_special_kmsg = 0;
unsigned int	c_netipc_special_page = 0;
unsigned int	c_critical_pull_recv = 0;
unsigned int	c_critical_pdwc = 0;
unsigned int	c_critical_modwc = 0;
unsigned int	c_critical_m_o_d_unavailable = 0;
unsigned int	c_critical_m_o_d_provide = 0;
unsigned int	c_critical_node_set = 0;
unsigned int	c_critical_node_set_reply = 0;

/*
 *	NORMA must recognize special case messages:
 *		norma_ipc_pull_receive
 *		memory_object_data_write_completed
 *		proxy_data_write_completed
 *		norma_ipc_node_set (and its reply)
 *	that are critical to the pageout path.  Such messages receive
 *	priority handling.  This special-case handling is a BIG HACK
 *	and hard-coding the message ids is ANOTHER UGLY HACK.  XXX
 *
 *	There is now even a special case of critical messages.  Such
 *	super-critical messages require occasional special-case treatment
 *	by the SENDER as well as by the receiver.
 */
#define	NORMA_IPC_PULL_RECEIVE_MSG		(556004)
#define	MEMORY_OBJECT_DATA_WRITE_COMPLETED_MSG	(2042)
#define	MEMORY_OBJECT_DATA_PROVIDED		(2038)
#define	MEMORY_OBJECT_DATA_UNAVAILABLE		(2039)
#define	PROXY_DATA_WRITE_COMPLETED		(923016)
#define	NORMA_IPC_NODE_SET_MSG			(556011)
#define	NORMA_IPC_NODE_SET_MSG_REPLY		(556111)


boolean_t
norma_critical_message(id, super_only)
mach_msg_id_t	id;
boolean_t	super_only;
{
	boolean_t	super_critical;

	super_critical = FALSE;
	switch (id) {
	    case PROXY_DATA_WRITE_COMPLETED:
		++c_critical_pdwc;
		super_critical = TRUE;
		break;
	    case MEMORY_OBJECT_DATA_WRITE_COMPLETED_MSG:
		++c_critical_modwc;
		super_critical = TRUE;
		break;
	    case NORMA_IPC_NODE_SET_MSG:
		++c_critical_node_set;
		super_critical = TRUE;
		break;
	/* Added DATA_PROVIDED for PTS 7434 */
	    case MEMORY_OBJECT_DATA_UNAVAILABLE:
		++c_critical_m_o_d_unavailable;
		break;
	    case MEMORY_OBJECT_DATA_PROVIDED:
		++c_critical_m_o_d_provide;
		break;
	    case NORMA_IPC_PULL_RECEIVE_MSG:
		++c_critical_pull_recv;
		break;
	    case NORMA_IPC_NODE_SET_MSG_REPLY:
		++c_critical_node_set_reply;
		break;
	    default:
		return FALSE;
	}
	return (!super_only || super_critical);
}


/*
 *	Determine whether a special-case paging message
 *	is available on this port.  For one extra-special
 *	case, memory_object_data_write_completed, reorder
 *	the message queue to move these messages to the front.
 *	This routine could be implemented much more efficiently
 *	but it is an embarrassingly large hack anyway.
 */
unsigned int	c_norma_critical_port_calls = 0;
unsigned int	c_norma_critical_port_criticals = 0;
unsigned int	c_norma_critical_port_normals = 0;
unsigned int	c_norma_critical_port_saved = 0;

boolean_t
norma_critical_port(port)
ipc_port_t	port;
{
	ipc_kmsg_t		kmsg;
	struct ipc_kmsg_queue	critical_messages;
	struct ipc_kmsg_queue	normal_messages;
	boolean_t		found_critical;

	++c_norma_critical_port_calls;
	ipc_kmsg_queue_init(&critical_messages);
	ipc_kmsg_queue_init(&normal_messages);
	found_critical = FALSE;
	ip_lock(port);

	/*
	 *	Sort wheat from chaff:  one pile of critical
	 *	messages and one pile of regular messages.
	 *	Note that order is maintained within each pile.
	 */
	while ((kmsg = ipc_kmsg_dequeue(&port->ip_messages.imq_messages))
	       != IKM_NULL) {
		if (norma_critical_message(kmsg->ikm_header.msgh_id, TRUE)) {
			++c_norma_critical_port_criticals;
			ipc_kmsg_enqueue(&critical_messages, kmsg);
			found_critical = TRUE;
		} else {
			++c_norma_critical_port_normals;
			ipc_kmsg_enqueue(&normal_messages, kmsg);
		}
	}
	assert(ipc_kmsg_queue_empty(&port->ip_messages.imq_messages));

	/*
	 *	Reassemble the port's message queue.  Add back critical
	 *	messages first.
	 */
	while ((kmsg = ipc_kmsg_dequeue(&critical_messages, kmsg)) != IKM_NULL)
		ipc_kmsg_enqueue(&port->ip_messages.imq_messages, kmsg);
	assert(ipc_kmsg_queue_empty(&critical_messages));

	/*
	 *	Now restore the normal messages, after the critical ones.
	 */
	while ((kmsg = ipc_kmsg_dequeue(&normal_messages, kmsg)) != IKM_NULL)
		ipc_kmsg_enqueue(&port->ip_messages.imq_messages, kmsg);
	assert(ipc_kmsg_queue_empty(&normal_messages));

	ip_unlock(port);
	if (found_critical == TRUE)
		++c_norma_critical_port_saved;
	return found_critical;
}


static boolean_t
netipc_special_kmsg(p, hdr, type)
vm_page_t		p;
struct netipc_hdr	*hdr;
unsigned long		type;
{
	ipc_kmsg_t	kmsg;
	ipc_port_t	port;

	assert(netipc_locked());

	/*
	 * Translate incoming page into a kmsg.  For an
	 * incoming page that contains a page (as opposed
	 * to containing a kmsg), find the kmsg belonging
	 * to the page.  The lookup of kmsg from page is
	 * relatively slow (linked list search) but we wind
	 * up in this routine relatively infrequently.
	 */
	if (type == NETIPC_TYPE_KMSG) {
		kmsg = (ipc_kmsg_t) VPTOKV(netipc_recv_page);
		/*
		 * Translate norma uid to a real port, then examine
		 * its ipc_space to see whether it's part of the
		 * default pager's ipc_space.
		 */
		port = norma_port_lookup_locked((unsigned long)
					(kmsg->ikm_header.msgh_remote_port));

	} else if (type == NETIPC_TYPE_PAGE) {
		(void) norma_remote_to_kmsg(hdr->remote,
					    hdr->pg.pg_first_seqid, TRUE,
					    &kmsg);
		port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
	} else
		return FALSE;

	if ((port != IP_NULL && port->ip_receiver == default_pager_space) ||
	    norma_critical_message(kmsg->ikm_header.msgh_id, FALSE)) {
		if (type == NETIPC_TYPE_KMSG)
			++c_netipc_special_kmsg;
		else
			++c_netipc_special_page;
		return TRUE;
	}

	return FALSE;
}

int netipc_drop_freq = 0;
int netipc_drop_counter = 0;
int netipc_checksum_counter = 0;

/*
 * A general comment about acknowledgements and seqids, since
 * I've screwed this up in the past...
 *
 * If seqid > pcs->pcs_last_received + 1:
 *	Something is very wrong, since we are using stop-and-wait.
 *	The sender sent a packet before receiving an ack for the
 *	preceeding packet.
 * 
 * If seqid = pcs->pcs_last_received + 1:
 *	This is a packet we've not processed before.
 *	Pass it up to the ipc code, and ack it (now or later).
 * 
 * If seqid = pcs->pcs_last_received:
 *	We've seen this before and acked it, but the ack
 *	might have been lost. Ack it again.
 * 
 * If seqid < pcs->pcs_last_received:
 *	We don't need to ack this, because we know (from having
 *	received later packets) that the sender has already
 *	received an ack for this packet.
 *
 * Again... this code needs to be modified to deal with window sizes > 1.
 */

unsigned int	c_netipc_page_delivery_count = 0;
unsigned int	c_netipc_recv_short_drop = 0;
unsigned int	c_netipc_recv_send_resume = 0;
unsigned int	c_netipc_recv_normal = 0;
unsigned int	c_netipc_recv_lowmem_special = 0;
unsigned int	c_netipc_recv_lowmem_normal = 0;
unsigned int	c_netipc_recv_packet_drops = 0;
unsigned int	c_netipc_recv_special_no_resume = 0;
unsigned int	c_netipc_old_recv = 0;
unsigned int	c_netipc_recv_inc_plr = 0;
unsigned int	c_netipc_suspend_syncs = 0;
unsigned int	c_netipc_suspend_acks = 0;
unsigned int	c_netipc_resuspends = 0;
unsigned int	c_netipc_suspends = 0;
unsigned int	c_netipc_recv_bogons = 0;
unsigned int	c_netipc_suspend_rxmits = 0;
unsigned int	c_netipc_recv_invalid = 0;
unsigned int	c_netipc_recv_node_self = 0;
unsigned int	c_netipc_zero_ool_bytes = 0;
unsigned int	c_netipc_unseen_seqid_nack_queued = 0;

_netipc_recv_intr()
{
	register struct netipc_hdr	*hdr = &netipc_recv_hdr;
	register pcs_t			pcs;
	register unsigned long		seqid = hdr->seqid;
	register unsigned long		type;
	register unsigned long 		node_resumed;
	register unsigned long 		incarnation = hdr->incarnation;
	register unsigned long		ctl = hdr->ctl;
	register unsigned long		ctl_seqid = hdr->ctl_seqid;
	register unsigned long		remote = hdr->remote;
	boolean_t			accept_packet, mem_avail, special;
	unsigned long			rtype;
#if	NETIPC_CHECKSUM
	unsigned long			checksum;
#endif	/* NETIPC_CHECKSUM */
	TR_DECL("_netipc_recv_intr");

	assert(netipc_locked());
	assert(netipc_receiving);

	netipc_receiving = FALSE;
	type = hdr->type & NODE_MASK;
	pcs = &netipc_pcs[remote];

	if (type == NETIPC_TYPE_KMSG)
		tr4("entry, KMSG remote 0x%x seqid 0x%x msgid %d",
		    remote, seqid, 
		  ((ipc_kmsg_t)VPTOKV(netipc_recv_page))->ikm_header.msgh_id);
	else
		tr5("entry, %s remote 0x%x %sseqid 0x%x",
		    (type == NETIPC_TYPE_INVALID ? "INVALID" :
		     type == NETIPC_TYPE_KMSG ? "KMSG" :
		     type == NETIPC_TYPE_PAGE ? "PAGE" :
		     type == NETIPC_TYPE_CTL ? "CTL" : "UNKNOWN"),
		    remote,
		    type == NETIPC_TYPE_CTL ? "ctl_" : "",
		    type == NETIPC_TYPE_CTL ? ctl_seqid : seqid);

#if	NORMA_FAST_OOL
	/*
	 *  Fast OOL is reliable and can't drop packets
	 */
#else
	/*
	 * Netipc_drop_freq > 0 enables debugging code.
	 */
	if (netipc_drop_freq) {
		if (++netipc_drop_counter >= netipc_drop_freq) {
			/*
			 * Reset counter, drop packet, and rearm.
			 */
			netipc_drop_counter = 0;
			netipc_start_receive();
			return;
		}
	}
#endif	NORMA_FAST_OOL

#if	NETIPC_CHECKSUM
	checksum = hdr->checksum;
	hdr->checksum = 0;
	hdr->checksum = netipc_compute_checksum(netvec_r, 2);
	if (hdr->checksum != checksum) {
		netipc_checksum_counter++;
		tr5("Checksum mismatch 0x%x/%x data=%d kr=%d",
			       hdr->checksum, checksum,
			       hdr->ctl_data, hdr->ctl_status);
#if	NORMA_FAST_OOL
		/*
		 *  Fast OOL transfer can not tolerate dropping a frame.
		 *  It depends on a reliable transport.  (Ya, I know, what's
		 *  it doing in a file called ipc_unreliable.c?)
		 */
		if (pcs->pcs_fast_ool)
			panic("checksum error doing fast OOL transfer\n");
#endif	NORMA_FAST_OOL
		netipc_start_receive();
		return;
	}
#endif	/* NETIPC_CHECKSUM */

	/*
	 * A little crude range checking: "remote" may still be invalid.
	 */
	if ((type != NETIPC_TYPE_INVALID && type != NETIPC_TYPE_KMSG &&
	     type != NETIPC_TYPE_PAGE && type != NETIPC_TYPE_CTL)
	    || remote >= MAX_NUM_NODES) {
		    /* Just drop it and jump-start */
		    c_netipc_recv_bogons++;
#if	NORMA_FAST_OOL
		    /*
		     *  Fast OOL transfer can not tolerate dropping a frame.
		     *  It depends on a reliable transport.  (Ya, I know, what's
		     *  it doing in a file called ipc_unreliable.c?)
		     */
		    if (pcs->pcs_fast_ool)
			panic("Bad type (0x%x) or node (0x%x) doing fast OOL transfer\n", type, remote);
#endif	NORMA_FAST_OOL
		    netipc_start_receive();
		    return;
	}

	if (type == NETIPC_TYPE_INVALID) {
		tr1("dropping INVALID packet");
		++c_netipc_recv_invalid;
#if	NORMA_FAST_OOL
		/*
		 *  Fast OOL transfer can not tolerate dropping a frame.
		 *  It depends on a reliable transport.  (Ya, I know, what's
		 *  it doing in a file called ipc_unreliable.c?)
		 */
		if (pcs->pcs_fast_ool)
			panic("NETIPC_TYPE_INVALID during fast OOL transfer\n");
#endif	NORMA_FAST_OOL
		netipc_start_receive();
		return;
	}

	if (remote == node_self()) {
		tr1("message from NODE SELF?");
		printf("message from NODE SELF?");
		++c_netipc_recv_node_self;
#if	NORMA_FAST_OOL
		/*
		 *  Fast OOL transfer can not tolerate dropping a frame.
		 *  It depends on a reliable transport.  (Ya, I know, what's
		 *  it doing in a file called ipc_unreliable.c?)
		 */
		if (pcs->pcs_fast_ool)
			panic("Fast OOL transfer from NODE SELF!\n");
#endif	NORMA_FAST_OOL
		netipc_start_receive();
		return;
	}

	/*
	 *	Crude packet statistics.  This could be implemented
	 *	more efficiently by adding code to existing cases,
	 *	below, but then there would have to be more counter
	 *	manipulations inserted for the various places packets
	 *	can end up (header incarnation logic, etc.).  The only
	 *	drawback here is that NETIPC_TYPE_INVALID is counted as
	 *	NETIPC_TYPE_KMSG in netipc_incoming_stats.
	 */
	rtype = type & NETIPC_TYPE_MASK;
	if (rtype >= 0 && rtype < NETIPC_TYPE_MAX)
		netipc_in_type_stats[rtype]++;
	if (type == NETIPC_TYPE_CTL &&
	    ctl > CTL_NONE && ctl < CTL_MAX)
		netipc_in_ctl_stats[ctl]++;

	/*
	 * Print packet if so desired.
	 */
	netipc_print('r', type, remote, ctl, seqid, ctl_seqid,
		     hdr->ctl_status, hdr->ctl_data, incarnation);

	/*
	 * Check incarnation of sender with what we thought it was.
	 * This lets us detect old packets as well as newly rebooted senders.
	 * The incarnation value always increases with each reboot.
	 */
	if (hdr->incarnation != pcs->pcs_incarnation) {
		if (hdr->incarnation < pcs->pcs_incarnation) {
			/*
			 * This is an old packet from a previous incarnation.
			 * We should ignore it.
			 */
#if	NORMA_FAST_OOL
			/*
			 *  Fast OOL transfer can not tolerate dropping a frame.
			 *  It depends on a reliable transport.  (Ya, I know,
			 *  what's it doing in a file called ipc_unreliable.c?)
			 */
			if (pcs->pcs_fast_ool)
				panic("New incarnation during fast OOL\n");
#endif	NORMA_FAST_OOL
			netipc_start_receive();
			return;
		} else if (pcs->pcs_incarnation == 0L) {
			/*
			 * This is the first packet we have ever received
			 * from this node. If it looks like a first packet
			 * (an implicit connection open), then remember
			 * incarnation number; otherwise, tell sender our
			 * new incarnation number, which should force him
			 * to do a netipc_cleanup_incarnation.
			 *
			 * Valid first packets are:
			 *	non-control messages with seqid = 1
			 *	control messages with ctl_seqid = 1
			 * Any others???
			 */
			assert(pcs->pcs_last_received == 0L);
			if ((type == NETIPC_TYPE_KMSG ||
			     type == NETIPC_TYPE_PAGE) && seqid == 1L ||
			    type == NETIPC_TYPE_CTL && ctl_seqid == 1L) {
				/*
				 * A valid first packet.
				 */
				pcs->pcs_incarnation = incarnation;
			} else {
				/*
				 * Probably left over from a previous
				 * incarnation.  Tell sender our new
				 * incarnation.
				 */
				netipc_queue_ctl(pcs, CTL_NEW_INCARNATION,
						 seqid, KERN_SUCCESS, 0L);
				netipc_start_receive();
				return;
			}
		} else {
			/*
			 * This is a packet from a new incarnation.
			 * We don't change incarnation number or process
			 * any packets until we have finished cleaning up
			 * anything that depended on previous incarnation.
			 */
			assert(incarnation > pcs->pcs_incarnation);
			if (pcs->pcs_new_incarnation == 0L) {
				pcs->pcs_new_incarnation = incarnation;
				netipc_cleanup_incarnation(pcs);
			}
			netipc_start_receive();
			return;
		}
	}
	assert(incarnation == pcs->pcs_incarnation);

	/*
	 * Check type of packet.
	 * Discard or acknowledge old packets.
	 */
	if (type == NETIPC_TYPE_KMSG || type == NETIPC_TYPE_PAGE) {
		/*
		 * If this is an old packet, then discard or acknowledge.
		 */
		if (seqid <= pcs->pcs_last_received) {
#if	NORMA_FAST_OOL
			/*
			 *  Fast OOL transfer can not tolerate dropping a frame.
			 *  It depends on a reliable transport.  (Ya, I know,
			 *  what's it doing in a file called ipc_unreliable.c?)
			 */
			if (pcs->pcs_fast_ool)
				panic("Bad seqid for type 0x%x in fast OOL transfer\n", type);
#endif	NORMA_FAST_OOL
			/*
			 * We have seen this packet before...
			 * but we might still need to reack it.
			 */
			c_netipc_old_recv++;
			if (seqid == pcs->pcs_last_received) {
				if (pcs->pcs_suspend_sent == TRUE &&
				    pcs->pcs_suspend_seqid == seqid) {
					++c_netipc_suspend_rxmits;
					pcs_suspend_source(pcs, seqid);
				} else {
					/*
					 * The sender may not yet have received
					 * an ack.  Send the ack immediately.
					 * Should we use ackdelay logic here?
					 */
					netipc_queue_ctl(pcs, CTL_ACK, seqid,
							 KERN_SUCCESS, 0L);
				}
			}
			/*
			 * Rearm with same buffer, and return.
			 */
			netipc_start_receive();
			return;
		}
	} else if (type != NETIPC_TYPE_CTL) {
		printf("netipc_recv_intr: bad type 0x%x\n", type);
#if	NORMA_FAST_OOL
		/*
		 *  Fast OOL transfer can not tolerate dropping a frame.
		 *  It depends on a reliable transport.  (Ya, I know,
		 *  what's it doing in a file called ipc_unreliable.c?)
		 */
		if (pcs->pcs_fast_ool)
			panic("Bad type 0x%x in fast OOL transfer\n", type);
#endif	NORMA_FAST_OOL
		netipc_start_receive();
		return;
	}

#if	TRACE_BUFFER
	if (ctl != CTL_NONE && ctl != CTL_SYNC) {
		tr5("CTL_%s seqid 0x%x status 0x%x data 0x%x",
		    ctl_to_ascii(ctl), ctl_seqid, hdr->ctl_status,
		    hdr->ctl_data);
	}
#endif

	/*
	 * Process incoming ctl, if any.
	 */
	if (ctl == CTL_NONE) {
		/*
		 *	Presumably a NETIPC_TYPE_KMSG or NETIPC_TYPE_PAGE.
		 *	Do nothing here.
		 */
	} else if (ctl == CTL_SYNC) {
		printf2("synch %d\n", ctl_seqid);
		if (ctl_seqid < pcs->pcs_last_received) {
			/* already acked, since sender sent newer packet. */
		} else if (ctl_seqid > pcs->pcs_last_received) {
			/* not yet seen; nack it. */
			/* use pending unacks? */
			assert(ctl_seqid == pcs->pcs_last_received + 1);
			if (ctl_seqid != pcs->pcs_last_received + 1) {
				printf("premature SYNC, remote %d: %d != %d\n",
				       remote, ctl_seqid,
				       pcs->pcs_last_received + 1);
			}
			++c_netipc_unseen_seqid_nack_queued;
			netipc_queue_ctl(pcs, CTL_NACK, ctl_seqid,
					KERN_SUCCESS, 0L);
		} else {
			/* may not be acked; ack it. */
			/* use ackdelay? */
			assert(ctl_seqid == pcs->pcs_last_received);
			/*
			 *	Courtesy of the suspend/resume protocol,
			 *	we must check whether a suspend is
			 *	outstanding on this connection.  If so,
			 *	then we must also check the ID of the
			 *	last suspended packet against the SYNC
			 *	sequence ID.  It is possible to accept
			 *	packets on a "suspended" connection if
			 *	the packets are paging-critical.  On the
			 *	other hand, if the SYNC request is for a
			 *	suspended packet, we dropped the packet
			 *	and the sender has not yet received the
			 *	suspend message.  Send another suspend.
			 *	If the packet in question was special
			 *	but we had to drop it anyway, we will
			 *	respond with a NACK (above).
			 */
			if (pcs->pcs_suspend_sent == TRUE &&
			    pcs->pcs_suspend_seqid == ctl_seqid) {
				++c_netipc_suspend_syncs;
				pcs_suspend_source(pcs, ctl_seqid);
			} else {
				if (pcs->pcs_suspend_sent == TRUE)
					++c_netipc_suspend_acks;
				netipc_queue_ctl(pcs, CTL_ACK, ctl_seqid,
						 KERN_SUCCESS, 0L);
			}
		}
	} else if (ctl == CTL_ACK) {
		netipc_recv_ack(pcs, ctl_seqid, hdr->ctl_status,
				hdr->ctl_data);
	} else if (ctl == CTL_NACK) {
		netipc_recv_nack(pcs, ctl_seqid);
	} else if (ctl == CTL_RESUME) {
		netipc_recv_resume(pcs, ctl_seqid, hdr->ctl_status,
				   hdr->ctl_data);
	} else if (ctl == CTL_NEW_INCARNATION) {
		/*
		 *	Nothing to do for this case; incarnation
		 *	update happens implicitly.
		 */
	} else {
		printf("%d: ctl?%d %d\n", node_self(), ctl, remote);
	}

	/*
	 * If this was just a ctl, then restart receive in same buffer.
	 */
	if (type == NETIPC_TYPE_CTL) {
		tr1("processed ctl, restart receive");
		netipc_start_receive();
		return;
	}

	/*
	 *	Can't check node_resumed at top of _netipc_recv_intr
	 *	because these bits aren't guaranteed to be valid for
	 *	control messages.
	 */
	node_resumed = hdr->type & ~NODE_MASK;
	tr4("node_resumed 0x%x %s msgh_id %d", node_resumed,
	    type == NETIPC_TYPE_KMSG ? "kmsg" : "page",
	    type != NETIPC_TYPE_KMSG ? 0 :
	    ((ipc_kmsg_t) VPTOKV(netipc_recv_page))->ikm_header.msgh_id);
	assert(node_resumed == NODE_OK || node_resumed == NODE_RESUMED);

	if (node_resumed == NODE_RESUMED) {
		assert(pcs->pcs_suspend_sent == TRUE);
		pcs->pcs_suspend_sent = FALSE;
		tr1("NODE_RESUMED ==> pcs_suspend_sent = FALSE");
	}

	accept_packet = FALSE;
	special = netipc_special_kmsg(netipc_recv_page, hdr, type);
	if (special || pcs->pcs_fast_ool) {
		if (netipc_reserve_page_force() == TRUE)
			accept_packet = TRUE;
		tr2("SPECIAL packet, %s reservation",
		    accept_packet == TRUE ? "FORCED" : "**NO**");
	} else {
		tr2("ordinary packet, pcs_suspend_sent 0x%x",
		    pcs->pcs_suspend_sent);
		if (pcs->pcs_suspend_sent) {
			++c_netipc_resuspends;
			pcs_suspend_source(pcs, seqid);
		} else {
#if	NORMA_FAST_OOL
			if (netipc_reserve_page( 1 ))
#else
			if (netipc_reserve_page())
#endif	NORMA_FAST_OOL
				accept_packet = TRUE;
			else {
				++c_netipc_suspends;
				pcs_suspend_source(pcs, seqid);
			}
		}
	}

	if (accept_packet == FALSE) {
		tr3("DROP seqid 0x%x node 0x%x", seqid, pcs->pcs_remote);
		++c_netipc_recv_packet_drops;
#if	NORMA_FAST_OOL
		/*
		 *  Fast OOL transfer can not tolerate dropping a frame.
		 *  It depends on a reliable transport.  (Ya, I know, what's
		 *  it doing in a file called ipc_unreliable.c?)
		 */
		if (pcs->pcs_fast_ool)
			panic("Fast OOL packetd not accepted\n");
#endif	NORMA_FAST_OOL
		netipc_start_receive();
		return;
	}

	/*
	 * At this point:
	 *	This is a previously unseen packet
	 *	We have room to keep it
	 *	It is either a kmsg or a page
	 *
	 * Deliver message according to its type.
	 */
	tr1("delivering kmsg/packet");
	assert(pcs->pcs_last_received == seqid - 1);
	assert(type == NETIPC_TYPE_KMSG || type == NETIPC_TYPE_PAGE);
	if (type == NETIPC_TYPE_KMSG) {
		register ipc_kmsg_t kmsg;
		
		/*
		 * This is a kmsg. Kmsgs are word aligned,
		 * and contain their own length.
		 */
		kmsg = (ipc_kmsg_t) VPTOKV(netipc_recv_page);
		kmsg->ikm_size = IKM_SIZE_NORMA;
		kmsg->ikm_marequest = IMAR_NULL;
		kmsg->ikm_page = netipc_recv_page;
		printf6("deliver kmsg: remote=%d msgh_id=%d dest=%x\n",
			remote,
			kmsg->ikm_header.msgh_id,
			kmsg->ikm_header.msgh_remote_port);
		printf7("r%d ", kmsg->ikm_header.msgh_id);
#ifdef NORMA_MSG_TRACE
		norma_msgid_count(kmsg->ikm_header.msgh_id);
#endif /* NORMA_MSG_TRACE */
		norma_deliver_kmsg(kmsg, remote, seqid);
	} else {
		/*
		 * This is out-of-line data, or out-of-line ports,
		 * or more of a bigger-than-page-size kmsg.
		 *
		 * In the slow path, the page is shuttled off onto
		 * a queue, from which it will be taken later by a
		 * kernel thread and reassembled onto the kmsg.
		 *
		 * In the fast path, we reassemble the page onto
		 * the kmsg now, still at interrupt level.  That
		 * path can fail, so the delivery routine has
		 * responsibility for sending an ack, if any.
		 *
		 * N.B.  When the ack is performed, netipc_recv_page
		 * is ripped loose and replaced with a new page.
		 */
		++c_netipc_page_delivery_count;
		if (hdr->pg.pg_ool_bytes == 0)
			++c_netipc_zero_ool_bytes;
#if	0
		assert(hdr->pg.pg_ool_bytes > 0);
#endif	0
		if (hdr->pg.pg_ool_bytes > netipc_page_delivery_limit) {
			printf6("deliver_page_slowly: remote=%d\n",
				remote);
			norma_deliver_page_slowly(netipc_recv_page, hdr);
		} else {
			printf6("deliver_page: remote=%d\n", remote);
			norma_deliver_page(netipc_recv_page, &hdr->pg,
					   remote, FALSE, FALSE);
		}
	}
	tr1("exit");
}

norma_ipc_ack(status, data)
	kern_return_t status;
	unsigned long data;
{
	register int remote = netipc_recv_hdr.remote;
	register unsigned long seqid = netipc_recv_hdr.seqid;
	register pcs_t pcs = &netipc_pcs[remote];

	TR_DECL("norma_ipc_ack");

#if	NORMA_FAST_OOL
	tr5("status 0x%x remote 0x%x seq 0x%x fool %d",
	    status, remote, seqid, pcs->pcs_fast_ool);
#else
	tr5("status 0x%x data 0x%x remote 0x%x seq 0x%x",
	    status, data, remote, seqid);
#endif	NORMA_FAST_OOL
	assert(netipc_locked());

	/*
	 * Send acknowledgements.
	 * Eventually, we should wait and try to piggyback these ctls.
	 * This could lead to deadlock if we aren't tricky.
	 * We should for example send acks as soon as we are idle.
	 */
	pcs->pcs_last_received = seqid;

#if	NORMA_FAST_OOL
	/*
	 *  If this is a fast ool transfer, don't ACK but re-arm the 
	 *  receiver for the node sending us fast OOL data.  We also
	 *  need to give back the page reserved in _netipc_recv_intr()
	 *  since we made our fast OOL reservation when the kmsg making
	 *  the request first arrived.
	 */
	if (pcs->pcs_fast_ool == TRUE) {
		assert(status == KERN_SUCCESS);
		netipc_reserve_cancel();
	} else
#endif	NORMA_FAST_OOL
		netipc_queue_ctl(pcs, CTL_ACK, seqid, status, data);

	/*
	 * Start a new receive.  Allocate a new buffer if prior receive
	 * succeeded (that buffer is now a kmsg).
	 * 
	 * XXX should we have done this before the deliver calls above?
	 */
	if (status == KERN_SUCCESS) {
		netipc_recv_hdr.type = NETIPC_TYPE_INVALID;
		/*
		 * This page_grab can't fail because previously
		 * we reserved a page (see _netipc_recv_intr).
		 */
		netipc_recv_page = netipc_page_grab();
		assert(netipc_recv_page != VM_PAGE_NULL);
		netvec_r[1].addr = VPTODEV(netipc_recv_page);
	} else {
		/*
		 * Give back the page reservation acquired by
		 * _netipc_recv_intr.
		 */
		netipc_reserve_cancel();
	}
	netipc_start_receive();
	tr1("exit");
}

#if	NORMA_FAST_OOL
/*
 *  This is very similar to norma_ipc_ack() but will send an ACK when the
 *  pcs_fast_ool flag is set.
 */
norma_ipc_ack_fast_ool(status, data)
	kern_return_t status;
	unsigned long data;
{
	register int remote = netipc_recv_hdr.remote;
	register pcs_t pcs = &netipc_pcs[remote];
	register unsigned long seqid = netipc_recv_hdr.seqid;

	assert(pcs->pcs_fast_ool == TRUE);
	assert(netipc_locked());
	assert(status == KERN_SUCCESS);

	/*
	 * Send acknowledgement for fast OOL request.
	 */
	pcs->pcs_last_received = seqid;
	netipc_queue_ctl(pcs, CTL_ACK, seqid, status, data);

	/*
	 *  We have already reserved all the pages coming in this fast OOL
	 *  transfer so give back the one allocated in _netipc_recv_intr().
	 */
	netipc_reserve_cancel();

	/*
	 * This page_grab can't fail because previously
	 * we reserved a page (see _netipc_recv_intr).
	 */
	netipc_recv_hdr.type = NETIPC_TYPE_INVALID;
	netipc_recv_page = netipc_page_grab();

	assert(netipc_recv_page != VM_PAGE_NULL);

	netvec_r[1].addr = VPTODEV(netipc_recv_page);
	netipc_start_receive();
}
#endif	NORMA_FAST_OOL

/*
 * Drop the packet; pretend that we never saw it.
 * Release page reservation acquired by _netipc_recv_intr.
 * Start a new receive.
 *
 * In a reliable interconnect module, we would register a nack here.
 */
norma_ipc_drop()
{
	netipc_reserve_cancel();
	netipc_start_receive();
}

#if MACH_ASSERT
int netipc_panic_on_collisions = 0;
#endif

/*
 * Called with interrupts blocked, from netipc_drain_acks and
 * netipc_recv_intr (and other places).  Ctl may be either CTL_ACK or
 * CTL_NACK.  If we are currently sending, use netipc_pending_ctl to
 * have netipc_send_intr do the send when the current send completes.
 *
 * The netipc_pending_ctl mechanism should be generalized to allow for
 * arbitrary pending send operations, so that no one needs to spin on
 * netipc_sending.
 */
netipc_queue_ctl(pcs, ctl, ctl_seqid, ctl_status, ctl_data)
	register pcs_t pcs;
	unsigned long ctl;
	unsigned long ctl_seqid;
	kern_return_t ctl_status;
	unsigned long ctl_data;
{
	TR_DECL("netipc_queue_ctl");

	assert(netipc_locked());
	assert(ctl > CTL_NONE && ctl < CTL_MAX);

	netipc_out_ctl_stats[ctl]++;
	tr5("%s seq 0x%x node/status 0x%x data 0x%x",
	    ctl_to_ascii(ctl), ctl_seqid,
	    (pcs->pcs_remote<<16)|ctl_status, ctl_data);
	/*
	 * If net is busy sending, let netipc_send_intr send the ctl.
	 */
	if (netipc_sending) {
		/*
		 * We may blow away a preceeding ctl, which is unfortunate
		 * but not fatal.  On the other hand, it's damned annoying.
		 */
		if (pcs->pcs_ctl != CTL_NONE) {
			++c_netipc_ctl_collisions[pcs->pcs_ctl];
			assert(!netipc_panic_on_collisions);
			if (pcs->pcs_ctl == CTL_SYNC)
				dec_retry(&pcs->pcs_ticks,
					netipc_sync_backoffc);
			else if (pcs->pcs_ctl == CTL_RESUME &&
				 pcs->pcs_ctl_data == RESUME_QUERY)
				dec_retry(&pcs->pcs_resume,
					netipc_resume_backoff);
			if (pcs->pcs_ctl == CTL_ACK) {
			    if (ctl == CTL_ACK) {
			        if (ctl_seqid <= pcs->pcs_ctl_seqid)
				    /* No need to resend old ACK */
				    return;
			    } else
			        /* We do not want to overwrite ACKs */
				return;
			    }
			}
		pcs->pcs_ctl = ctl;
		pcs->pcs_ctl_seqid = ctl_seqid;
		pcs->pcs_ctl_status = ctl_status;
		pcs->pcs_ctl_data = ctl_data;
		if (! queue_tabled(&pcs->pcs_send)) {
			queue_enter(&netipc_sends, pcs, pcs_t, pcs_send);
		}
		return;
	}

	/*
	 * Fill in send_hdr_a.
	 */
	send_hdr_a.ctl = ctl;
	send_hdr_a.ctl_seqid = ctl_seqid;
	send_hdr_a.ctl_status = ctl_status;
	send_hdr_a.ctl_data = ctl_data;

	/*
	 * Start the send.
	 */
	netipc_send_ctl(pcs->pcs_remote);
}

/*
 * Net send interrupt routine: called when a send completes.
 * If there is something else to send (currently, only ctls),
 * send it; otherwise, set netipc_sending FALSE.
 */
_netipc_send_intr()
{
	register pcs_t pcs, next_pcs;

	TR_DECL("_netipc_send_intr");

	tr1("Send interrupt fielded");

	assert(netipc_locked());
	assert(netipc_sending);
	netipc_sending = FALSE;

#if	NORMA_FAST_OOL
	/*
	 *  If we just completed sending a fast OOL page, let's try for
	 *  another.  If this was the last page, clear the packetid.
	 */
	if (netipc_fast_ool_packetid != INVALID_FAST_OOL_PACKETID) {
		if (netipc_next_fast_ool(netipc_fast_ool_packetid))
			return;
	}
#endif	NORMA_FAST_OOL

	/*
	 * Scan the pending list, looking for something to send.
	 * If something is on the list but doesn't belong, remove it.
	 */
	pcs = (pcs_t) queue_first(&netipc_sends);
	while (!queue_end(&netipc_sends, (queue_entry_t) pcs)) {
		next_pcs = (pcs_t) queue_next(&pcs->pcs_send);

		if (pcs->pcs_ctl != CTL_NONE) {
			/*
			 * There is a ctl to send; send it.
			 */
			send_hdr_a.ctl = pcs->pcs_ctl;
			send_hdr_a.ctl_seqid = pcs->pcs_ctl_seqid;
			send_hdr_a.ctl_status = pcs->pcs_ctl_status;
			send_hdr_a.ctl_data = pcs->pcs_ctl_data;
			pcs->pcs_ctl = CTL_NONE;
			netipc_send_ctl(pcs->pcs_remote);
			return;
		}

		/*
		 * There may be something to retransmit.
		 */
		if (pcs->pcs_nacked != 0L) {
			register unsigned long seqid = pcs->pcs_nacked;
			pcs->pcs_nacked = 0L;
			assert(pcs->pcs_unacked_packetid[seqid % WX]);
			netipc_send_old(pcs->pcs_unacked_packetid[seqid % WX],
					seqid);
			return;
		}

		/*
		 * There may be something new to send.
		 */
		if (netipc_send_new(pcs->pcs_remote, pcs->pcs_last_sent + 1)) {
			/*
			 * It is possible for an important paging reply
			 * to be stuck behind an otherwise-useless
			 * transmission that is making no progress.
			 * Thus, we rotate the queue a bit each time
			 * we exit this routine, so that the next time
			 * we enter we can make progress on a different
			 * transmission.
			 */
			queue_untable(&netipc_sends, pcs, pcs_t, pcs_send);
			queue_enter(&netipc_sends, pcs, pcs_t, pcs_send);
			return;
		}

		/*
		 * Nothing to send -- remove from queue.
		 */
		queue_untable(&netipc_sends, pcs, pcs_t, pcs_send);
		pcs = next_pcs;
	}
}

netipc_protocol_init()
{
	register unsigned long remote;
	register int i;
	register pcs_t pcs;

	for (i = 0; i < max_backoff_ticks; i++) {
	    c_netipc_syncb[i]=0;
	    c_netipc_resumeb[i]=0;
	}

	/*
	 * Initialize pcs structs, queues, and enable timer.
	 */
	for (remote = 0; remote < MAX_NUM_NODES; remote++) {
		pcs = &netipc_pcs[remote];
		pcs->pcs_remote = remote;
		queue_init_untabled(&pcs->pcs_timer);
		queue_init_untabled(&pcs->pcs_send);
		queue_init_untabled(&pcs->pcs_suspend);
		pcs->pcs_suspend_sent = FALSE;
		pcs->pcs_suspend_seqid = 0;
	}
	queue_init(&netipc_timers);
	queue_init(&netipc_sends);
	queue_init(&netipc_suspended);
	netipc_set_timeout();

	/*
	 *	Resume query processing.
	 */
	netipc_resume_timeout_posted = FALSE;

	/*
	 * Initialize netipc_recv_hdr and netvec_r
	 */
	netipc_recv_hdr.type = NETIPC_TYPE_INVALID;
	netvec_r[0].addr = KVTODEV(&netipc_recv_hdr);
	netvec_r[0].size = sizeof(struct netipc_hdr);
	netvec_r[1].size = PAGE_SIZE;

	/*
	 * Initialize send_hdr_a and netvec_a
	 */
	send_hdr_a.type = NETIPC_TYPE_CTL;
	send_hdr_a.remote = node_self();
	netvec_a[0].addr = KVTODEV(&send_hdr_a);
	netvec_a[0].size = sizeof(struct netipc_hdr);
#if	NETVEC_A_SIZE > 1
	netvec_a[1].addr = KVTODEV(&send_data_a);
	netvec_a[1].size = sizeof(unsigned long);
#endif	NETVEC_A_SIZE > 1

	/*
	 * Start receiving.
	 */
	netipc_recv_page = vm_page_grab();
	vm_page_gobble(netipc_recv_page);
	assert(netipc_recv_page != VM_PAGE_NULL);
	netvec_r[1].addr = VPTODEV(netipc_recv_page);
	assert(netvec_r[1].size == PAGE_SIZE);
	netipc_start_receive();
}

netipc_start_receive()
{
	assert(! netipc_receiving);
	netipc_receiving = TRUE;
	netvec_r[0].size = sizeof(struct netipc_hdr);
	netvec_r[1].size = PAGE_SIZE;
	netipc_recv(netvec_r, 2);
}

netipc_init()
{
	extern void	netipc_input_init();

	netipc_network_init();
	netipc_input_init();
	netipc_output_init();
	netipc_protocol_init();
	netipc_page_init();
}

netipc_cleanup_incarnation(pcs)
	register pcs_t pcs;
{
	printf1("netipc_cleanup_incarnation(%d)\n", pcs->pcs_remote);

	/*
	 * Clean up connection state and higher-level ipc state.
	 */
	netipc_cleanup_send_state(pcs);
	netipc_cleanup_receive_state(pcs);
	netipc_cleanup_ipc_state(pcs->pcs_remote);
}

#if 666
/*
 * This be in the norma ipc layer and should do something real
 */
netipc_cleanup_ipc_state(remote)
	unsigned long remote;
{
	/* XXX */
	netipc_cleanup_incarnation_complete(remote);
}
#endif

netipc_cleanup_incarnation_complete(remote)
	unsigned long remote;
{
	register pcs_t pcs = &netipc_pcs[remote];

	pcs->pcs_incarnation = pcs->pcs_new_incarnation;
	pcs->pcs_new_incarnation = 0;
}

netipc_send_ctl(remote)
	unsigned long remote;
{
	register struct netipc_hdr *hdr = &send_hdr_a;

	assert(netipc_locked());
	assert(! netipc_sending);
	netipc_sending = TRUE;

	hdr->incarnation = node_incarnation;
	if (remote == node_self()) {
		panic("netipc_send_ctl: sending to node_self!\n");
	}
	netipc_print('s', NETIPC_TYPE_CTL, remote, hdr->ctl, 0L,
		     hdr->ctl_seqid, hdr->ctl_status, hdr->ctl_data,
		     node_incarnation);
#if	NETIPC_CHECKSUM
	hdr->checksum = 0;
	hdr->checksum = netipc_compute_checksum(netvec_a, NETVEC_A_SIZE);
#endif	NETIPC_CHECKSUM
	netipc_send(remote, netvec_a, NETVEC_A_SIZE);
}

netipc_send_with_timeout(remote, vec, count, packetid, seqid)
	unsigned long remote;
	register struct netvec *vec;
	unsigned int count;
	unsigned long packetid;
	unsigned long seqid;
{
	struct netipc_hdr *hdr = (struct netipc_hdr *) DEVTOKV(vec[0].addr);
/*	register unsigned long seqid = hdr->seqid;*/
	register unsigned long type = hdr->type;
	register unsigned long ctl;
	register unsigned long ctl_seqid = hdr->ctl_seqid;
	register kern_return_t ctl_status = hdr->ctl_status;
	register unsigned long ctl_data = hdr->ctl_data;
	register pcs_t pcs = &netipc_pcs[remote];

	static boolean_t	timer_being_queued = FALSE;

	TR_DECL("netipc_send_with_timeout");

	tr3("Sending with timeout to remote=%d seqid=%d", remote, seqid);

	assert(netipc_locked());
	assert(! netipc_sending);
	assert(seqid = hdr->seqid);
	netipc_sending = TRUE;

	ctl = hdr->ctl = CTL_NONE;

	hdr->incarnation = node_incarnation;
	while (pcs->pcs_new_incarnation != 0L) {
		/*
		 * Shouldn't get this far!
		 */
		panic("netipc_send_with_timeout: incarnation clean!!!\n");
	}
	if (pcs->pcs_remote == node_self()) {
		panic("netipc_send_with_timeout: sending to node_self!\n");
	}
	netipc_print('s', type, pcs->pcs_remote, ctl, seqid, ctl_seqid,
		     ctl_status, ctl_data, node_incarnation);
	if (pcs->pcs_unacked_packetid[seqid % WX] == 0) {
		pcs->pcs_unacked_packetid[seqid % WX] = packetid;
	}
	if (seqid > pcs->pcs_last_sent) {
		pcs->pcs_last_sent = seqid;
	}
	assert(!timer_being_queued);
	timer_being_queued = TRUE;
	assert(!queue_tabled(&pcs->pcs_timer));
	init_retry(&pcs->pcs_ticks, netipc_sync_backoffc);
	queue_enter(&netipc_timers, pcs, pcs_t, pcs_timer);
	timer_being_queued = FALSE;
#if	NETIPC_CHECKSUM
	hdr->checksum = 0;
	hdr->checksum = netipc_compute_checksum(vec, count);
#endif	NETIPC_CHECKSUM
	netipc_send(pcs->pcs_remote, vec, count);
}

#if	NORMA_FAST_OOL
/*
 *  This is very similar to netipc_send_with_timeout() except it doesn't
 *  set a retry timer or check for new/wrong incarnations.
 */
netipc_send_fast_ool(remote, vec, count, packetid, seqid, last_page)
	unsigned long remote;
	register struct netvec *vec;
	unsigned int count;
	unsigned long packetid;
	unsigned long seqid;
	boolean_t last_page;
{
	struct netipc_hdr *hdr = (struct netipc_hdr *) DEVTOKV(vec[0].addr);
	register pcs_t pcs = &netipc_pcs[remote];

	TR_DECL("netipc_send_fast_ool");

	tr4("Sending FOOL to remote=%d seqid=%d last_page=%d",
			remote, seqid, last_page);

	assert(netipc_locked());
	assert(! netipc_sending);
	assert(seqid = hdr->seqid);

	netipc_sending = TRUE;
	if (last_page)
		netipc_fast_ool_packetid = INVALID_FAST_OOL_PACKETID;
	else
		netipc_fast_ool_packetid = packetid;

	hdr->ctl = CTL_NONE;
	hdr->incarnation = node_incarnation;

	while (pcs->pcs_new_incarnation != 0L) {
		/*
		 * Shouldn't get this far!
		 */
		panic("netipc_send_fast_ool: incarnation clean!!!\n");
	}
	if (pcs->pcs_remote == node_self()) {
		panic("netipc_send_fast_ool: sending to node_self!\n");
	}

	pcs->pcs_unacked_packetid[seqid % WX] = packetid;
	pcs->pcs_last_sent = seqid;

#if	NETIPC_CHECKSUM
	hdr->checksum = 0;
	hdr->checksum = netipc_compute_checksum(vec, count);
#endif	NETIPC_CHECKSUM
	netipc_send(pcs->pcs_remote, vec, count);
}
#endif	NORMA_FAST_OOL

#if	TRACE_BUFFER || MACH_KDB || 1
static char *
ctl_to_ascii(ctl)
unsigned long ctl;
{
	return(	ctl == CTL_NONE ? "NONE" :
		ctl == CTL_ACK ? "ACK" :
		ctl == CTL_NACK ? "NACK" :
		ctl == CTL_SYNC ? "SYNC" :
		ctl == CTL_RESUME ? "RESUME" :
		"UNKNOWN" );
}
#endif

netipc_print(c, type, remote, ctl, seqid, ctl_seqid, ctl_status, ctl_data,
	     incarnation)
	char c;
	unsigned long type;
	unsigned long remote;
	unsigned long ctl;
	unsigned long seqid;
	unsigned long ctl_seqid;
	kern_return_t ctl_status;
	unsigned long ctl_data;
	unsigned long incarnation;
{
	char *ts;
	char *cs;
	extern int Noise0;

	assert(netipc_locked());
	if (Noise0 == 2) {
		printf("%c", c);
		return;
	}
	if (Noise0 == 0) {
		return;
	}
	if (type == NETIPC_TYPE_KMSG) {
		ts = "kmsg";
	} else if (type == NETIPC_TYPE_PAGE) {
		ts = "page";
	} else if (type == NETIPC_TYPE_CTL) {
		ts = "ctrl";
		seqid = 0;
	} else {
		ts = "????";
	}
	if (ctl == CTL_NONE) {
		cs = "none";
		ctl_seqid = 0;
	} else if (ctl == CTL_ACK) {
		cs = "ack ";
	} else if (ctl == CTL_NACK) {
		cs = "nack";
	} else if (ctl == CTL_SYNC) {
		cs = "sync";
	} else if (ctl == CTL_RESUME) {
		cs = "resu";
	} else {
		cs = "????";
	}
	printf("%c remote=%d type=%s.%04d ctl=%s.%04d kr=%2d data=%2d %10d\n",
	       c, remote, ts, seqid, cs, ctl_seqid, ctl_status, ctl_data,
	       incarnation);
}

#ifdef	NORMA_MSG_TRACE
#define NORMA_MSG_BUFSZ	1024
struct norma_msgid {
	int msgid;
	int count;
} norma_msgid_buf[NORMA_MSG_BUFSZ];

struct norma_msgid *norma_msgid_first = &norma_msgid_buf[0];
struct norma_msgid *norma_msgid_last = &norma_msgid_buf[0];
struct norma_msgid *norma_msgid_end = &norma_msgid_buf[NORMA_MSG_BUFSZ];
int norma_msgid_overflow = 0;

norma_msgid_reset()
{
	norma_msgid_last = norma_msgid_first;
	norma_msgid_overflow = 0;
}

norma_msgid_count(msgid)
	int msgid;
{
	struct norma_msgid *ns;

	for (ns = norma_msgid_first; ns < norma_msgid_last; ns++) {
		if (ns->msgid == msgid) {
			ns->count++;
			return;
		}
	}
	if (ns == norma_msgid_end) {
		norma_msgid_overflow++;
		return;
	}
	ns->msgid = msgid;
	ns->count = 1;
	norma_msgid_last = ns + 1;
}

norma_msgid_print()
{
	struct norma_msgid *ns;

	for (ns = norma_msgid_first; ns < norma_msgid_last; ns++) {
		kdbprintf("%-8d %8d\n", ns->msgid, ns->count);
	}
	return (ns - norma_msgid_first);
}

norma_msgid_op(operation, buf, len)
	int operation;
	vm_offset_t buf;
	vm_offset_t len;
{
	vm_size_t size;

	switch(operation) {
	case 0:
		size = (vm_size_t) (sizeof(struct norma_msgid) *
				    (norma_msgid_last -
				     norma_msgid_first));
		
		(void) copyout((vm_offset_t) norma_msgid_buf,
			       (vm_offset_t) buf,
			       (vm_size_t) size);
		(void) copyout((vm_offset_t) &size,
			       (vm_offset_t) len,
			       (vm_size_t) sizeof(size));
		return TRUE;
		
	case 1:
		norma_msgid_reset();
		return TRUE;
		
	default:
		return FALSE;
	}
}
#endif	/* NORMA_MSG_TRACE */

#include <mach_kdb.h>
#if	MACH_KDB

#define	printf	kdbprintf

/*
 *	Routine:	netipc_pcs_print
 *	Purpose:
 *		Pretty-print a netipc protocol control structure for ddb.
 */

netipc_pcs_print(pcs)
	pcs_t pcs;
{
	extern int indent;
	int i;

	if ((unsigned int) pcs < MAX_NUM_NODES) {
		pcs = &netipc_pcs[(unsigned int) pcs];
	}

	printf("netipc pcs 0x%x\n", pcs);

	indent += 2;

	iprintf("remote=%d", pcs->pcs_remote);
	printf(", last_received=%d", pcs->pcs_last_received);
	printf(", last_sent=%d", pcs->pcs_last_sent);
#if	NORMA_FAST_OOL
	printf(", fast_ool=%d", pcs->pcs_fast_ool);
#endif	NORMA_FAST_OOL
	printf(", nacked=%d\n", pcs->pcs_nacked);

	iprintf("sent_packet[0..%d]={", WX - 1);
	for (i = 0; i < WX - 1; i++) {
		printf("0x%x, ", pcs->pcs_unacked_packetid[i]);
	}
	printf("0x%x}\n", pcs->pcs_unacked_packetid[WX - 1]);

	iprintf("incarnation=%d", pcs->pcs_incarnation);
	printf(", new_incarnation=%d", pcs->pcs_new_incarnation);
	printf(", suspend_sent=%d", pcs->pcs_suspend_sent);
	printf(", suspend_seqid=%d\n", pcs->pcs_suspend_seqid);

	iprintf("ctl=%d", pcs->pcs_ctl);
	printf("[%s]", ctl_to_ascii(pcs->pcs_ctl));
	printf(", ctl_seqid=%d", pcs->pcs_ctl_seqid);
	printf(", ctl_status=%ld", pcs->pcs_ctl_status);
	printf(", ctl_data=%ld\n", pcs->pcs_ctl_data);
	iprintf("ticks=%d", pcs->pcs_ticks.ticks);
	printf(", ticks_backoff=%d", pcs->pcs_ticks.backoff);
	printf(", resume_ticks=%d", pcs->pcs_resume.ticks);
	printf(", resume_backoff=%d\n", pcs->pcs_resume.backoff);

	iprintf("timer=[%x,%x t=%d]",
	       pcs->pcs_timer.prev,
	       pcs->pcs_timer.next,
	       queue_tabled(&pcs->pcs_timer));
	iprintf("send=[%x,%x t=%d]",
	       pcs->pcs_send.prev,
	       pcs->pcs_send.next,
	       queue_tabled(&pcs->pcs_send));
	printf(", suspend=[%x,%x t=%d]",
	       pcs->pcs_suspend.prev,
	       pcs->pcs_suspend.next,
	       queue_tabled(&pcs->pcs_suspend));
	printf("\n");

	indent -=2;
}


#define	db_pcs_queue(q, link, name)				\
        iprintf("%s:\t", (name));				\
        if (queue_empty((q)))					\
                printf("%s", empty);				\
	else							\
		queue_iterate((q), pcs, pcs_t, link)		\
                	printf("0x%x[%d] ",pcs,pcs->pcs_remote);\
	printf("\n");

extern int c_netipc_recv_ack_kernel_short;
extern int c_norma_ipc_send_criticals;
db_norma_ipc()
{
	register pcs_t  pcs;
	char		*empty="EMPTY";
	register int i;
	extern int indent;

	iprintf("norma port table statistics:\n");
	norma_port_table_statistics();

	iprintf("\nnetipc_sending %d, able_recv %d, send_criticals %d\n\n",
		netipc_sending, netipc_able_continue_recv(),
		c_norma_ipc_send_criticals);

	iprintf("netipc queues:\n");
	indent += 2;
	db_pcs_queue(&netipc_suspended, pcs_suspend, "suspends");
	db_pcs_queue(&netipc_sends, pcs_send, "sends");
	db_pcs_queue(&netipc_timers, pcs_timer, "timers");
	indent -= 2;

	iprintf("suspend/resume:\n");
	indent += 2;
	iprintf("sent:  suspends %7d resumes %7d queries  %7d\n",
		c_netipc_suspends,
		c_netipc_send_resume_ack,
		c_netipc_send_resume);
	iprintf("recv:  suspends %7d resumes %7d queries  %7d\n",
		c_netipc_recv_ack_kernel_short,
		c_netipc_recv_resume_ack,
		c_netipc_recv_resume_query);
#if	NORMA_FAST_OOL
	{
	extern	int	c_netipc_recv_ack_fast_ool_accepted,
			c_netipc_recv_ack_fast_ool_rejected,
			netipc_fast_ool,
			c_netipc_fast_ool_accept,
			c_netipc_fast_ool_reject;

	iprintf("fast OOL as initiator: accepted %7d rejected %7d total %7d\n",
		c_netipc_recv_ack_fast_ool_accepted,
		c_netipc_recv_ack_fast_ool_rejected,
		netipc_fast_ool);

	iprintf("fast OOL as responder: accepted %7d rejected %7d total %7d\n",
		c_netipc_fast_ool_accept, c_netipc_fast_ool_reject,
		c_netipc_fast_ool_accept + c_netipc_fast_ool_reject);
	}
#endif	NORMA_FAST_OOL

	iprintf("sync   backoff ticks = %d  backoff = %d  backoffc = %d:\n",
		netipc_ticks, netipc_sync_backoff, netipc_sync_backoffc);
	for (i = 0; i < max_backoff_ticks; i++) {
	   iprintf(" %8d",c_netipc_syncb[i]);
	   if ((i+1)%6==0) iprintf("\n");
	}
	if (i%6!=0) iprintf("\n");
	iprintf("rquery backoff ticks = %d  backoff = %d:\n",
		netipc_resume_timeout_ticks, netipc_resume_backoff);
	for (i = 0; i < max_backoff_ticks; i++) {
	   iprintf(" %8d",c_netipc_resumeb[i]);
	   if ((i+1)%6==0) iprintf("\n");
	}
	if (i%6!=0) iprintf("\n");
	indent -= 2;

	iprintf("obsolete acks: %7d\n", c_netipc_obsolete_acks);

	{
	extern int netipc_page_list_count,     netipc_page_list_avail,
		   netipc_page_list_fallbacks, netipc_page_list_refill,
		   netipc_page_list_owed,      netipc_page_list_low,
		   netipc_page_list_high;

	iprintf("netipc recource levels:\n");
	indent += 2;
	iprintf("netipc_page_list_count     %4d  netipc_page_list_avail  %4d\n",
		 netipc_page_list_count,         netipc_page_list_avail);
	iprintf("netipc_page_list_fallbacks %4d  netipc_page_list_refill %4d\n",
		 netipc_page_list_fallbacks,     netipc_page_list_refill);
	iprintf("netipc_page_list_owed      %4d  netipc_page_list_low    %4d\n",
		 netipc_page_list_owed,          netipc_page_list_low);
	iprintf("netipc_page_list_high      %4d\n",
		 netipc_page_list_high);
	indent -= 2;
	}

#if	0
netipc_resume_timeout_ticks,
netipc_resume_timeout_posted,
c_netipc_resume_timeout_called,
c_netipc_resume_timeout_posts,
c_netipc_resume_timeout_activates,
c_netipc_resume_sync_failure,
c_netipc_suspend_syncs,
c_netipc_suspend_acks,
c_netipc_resuspends,
c_netipc_recv_bogons,
c_netipc_suspend_rxmits,
c_netipc_recv_special_no_resume,
c_netipc_recv_send_resume,

c_netipc_special_kmsg,
c_netipc_special_page,
c_critical_pull_recv,
c_critical_pdwc,
c_critical_modwc,
c_critical_node_set,
c_critical_node_set_reply,

c_netipc_page_delivery_count,
c_netipc_recv_short_drop,
c_netipc_recv_normal,
c_netipc_recv_lowmem_special,
c_netipc_recv_lowmem_normal,
c_netipc_recv_packet_drops,
c_netipc_old_recv,
c_netipc_recv_inc_plr,
c_netipc_obsolete_acks,
#endif
}
#endif	/* MACH_KDB */
