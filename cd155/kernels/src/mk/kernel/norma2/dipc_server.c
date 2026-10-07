/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */

/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1994  Intel Corporation.
 *
 *	$Id: dipc_server.c,v 1.9 1995/01/24 17:16:21 cfleck Exp $
 */

#include "cpus.h"
#include "mach_assert.h"

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <mach/message.h>

#include <vm/vm_map.h>

#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>

#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>

#include <norma2/meta_kmsg.h>
#include <norma2/dipc_uid.h>
#include <norma2/dipc_migrate.h>
#include <norma2/dipc_server.h>
#include <norma2/norma_transport.h>
#include <norma2/dipc_blocked.h>
#include <norma2/dipc_client.h>

extern void		panic( char * );
extern void		bcopy( char *, char *, int );
extern kern_return_t	thread_policy( thread_t, int, int );
extern int		splsched();
extern void		splx( int );

extern void		dipc_unblock_one_sender( ipc_port_t );

extern ipc_port_t	dipc_kernel_port;
extern ipc_kmsg_t	dipc_kmsg_grab( vm_size_t );

extern ipc_port_t	dipc_get_port_migration_state( 
				ipc_port_t,
				dipc_port_migration_state_t,
				dipc_node_t);

extern	int		rpc_engine_payload_size;

extern vm_map_t dipc_pageout_map;

static boolean_t	dipc_op_demux(
	rpc_handle_t,				/* rpc handle */
	boolean_t,				/* canwait */
	rpc_class_t,				/* rpc service class */
	void (*)( rpc_handle_t, rpc_notify_t ),	/* callback function */
	rpc_notify_t );				/* callback arg */

static boolean_t	dipc_op_enqueue( dipc_enqueue_op_t, boolean_t );

static boolean_t	dipc_op_buy_transits( dipc_transit_op_t, boolean_t );

static boolean_t	dipc_op_sell_transits( dipc_transit_op_t, boolean_t );

static boolean_t	dipc_op_queue_avail( dipc_queue_avail_op_t, boolean_t );

static boolean_t	dipc_op_polymorph( dipc_polymorph_op_t, boolean_t );

static boolean_t	dipc_op_migration( dipc_migration_op_t, boolean_t );

static boolean_t	dipc_op_dn_request( dipc_request_dead_op_t, boolean_t );

static boolean_t	dipc_op_dn_notify( dipc_notify_dead_op_t, boolean_t );


/*
 *	The enqueue thread services only enqueue requests,
 *	and can block allocating a meta_kmsg.
 */
static void	dipc_enqueue_thread();
int		dipc_enqueue_thread_pri = 0;
int		dipc_enqueue_thread_policy = POLICY_FIXEDPRI;
int		dipc_enqueue_thread_quantum = 1;
rpc_handle_t   *dipc_enqueue_ring;
volatile int	dipc_enqueue_ring_in;
volatile int	dipc_enqueue_ring_out;
int		dipc_enqueue_ring_max;
volatile boolean_t	dipc_enqueue_thread_continue;
volatile boolean_t	dipc_enqueue_thread_running;

/*
 *	The control thread services transit operations,
 *	and processes message-queue-no-longer-full notifications.
 *
 *	It blocks only when waiting for the next request.
 */
static void	dipc_control_thread();
int		dipc_control_thread_pri;
int		dipc_control_thread_policy = POLICY_FIXEDPRI;
int		dipc_control_thread_quantum = 1;
rpc_handle_t   *dipc_control_ring;
volatile int	dipc_control_ring_in;
volatile int	dipc_control_ring_out;
int		dipc_control_ring_max;
volatile boolean_t	dipc_control_thread_continue;
volatile boolean_t	dipc_control_thread_running;

#include <norma2/norma_log.h>

typedef struct {
	unsigned long	coherency_hack_invoked;
	unsigned long	coherency_hack_unstuck;

	unsigned long	emmi_reply_priv_enqueue;
	unsigned long	emmi_reply_enqueue;
	unsigned long	kobj_enqueue;
	unsigned long	pset_enqueue;
	unsigned long	port_enqueue;
	unsigned long	waiting_receiver;

	unsigned long	buy_transits;
	unsigned long	sell_transits;

	unsigned long	early_queue_avail;
	unsigned long	no_blocked_sender;
	unsigned long	awaken_blocked_sender;

	unsigned long	migration;
	unsigned long	polymorph;

	unsigned long	deadname_req;
	unsigned long	deadname_notify;

	unsigned long	fastpath_hits;
	unsigned long	fastpath_busy;
	unsigned long	fastpath_no_kmsg;
} dipc_server_stats_t;

dipc_server_stats_t	dipc_server_stats;
#define	SERVER_STATS(a)	dipc_server_stats.a;

/*
 *	Initialize a ring buffer used to record which RPC handles
 *	have requests ready for servicing.
 */
static void dipc_ring_buffer_init(rpc_handle_t **ring, int max)
{
	vm_offset_t	addr;
	vm_size_t	size;

	server_entry2(dipc_ring_buffer_init, ring, max);

	size = max * sizeof(rpc_handle_t);
	if ((addr = kalloc(size)) == 0)
		panic("dipc_ring_buffer_init");
	*ring = (rpc_handle_t *) addr;
}

#define	RPC_COHERENCY_HACK	1
#if	RPC_COHERENCY_HACK

/*
 *	The code is compiled in by default, but the workaround is
 *	now disabled by default.  Set RPC_COHERENCY_HACK_TICKS=1000
 *	in bootmagic to scan for stuck RPC engines.
 */
unsigned long	rpc_coherency_hack_ticks = 0;	/* 10*100 == 10 seconds */


rpc_coherency_hack_intr()
{
	extern int	rpc_engine_unstick();

	/*server_entry(rpc_coherency_hack_intr);*/

	SERVER_STATS(coherency_hack_invoked++);
	SERVER_STATS(coherency_hack_unstuck += rpc_engine_unstick());
	rpc_coherency_hack_set_timeout();
}

rpc_coherency_hack_set_timeout()
{
	if (rpc_coherency_hack_ticks > 0) {
		timeout(rpc_coherency_hack_intr, 0, rpc_coherency_hack_ticks);
	}
}
#endif	/* RPC_COHERENCY_HACK */


/*
 *	Initialize and start the NORMA2 enqueue server thread.
 */
void dipc_enqueue_server_init(int nhandles)
{
	server_entry1(dipc_enqueue_server_init, nhandles);

	dipc_ring_buffer_init(&dipc_enqueue_ring, nhandles + 1);
	dipc_enqueue_ring_max = nhandles + 1;

	(void) kernel_thread(kernel_task, dipc_enqueue_thread, 0);

#if	RPC_COHERENCY_HACK
	/*
	 *	It appears that there is some coherency problem
	 *	that may lead to a "stuck" RPC handle for
	 *	after-the-fact callback installation -- the MCP
	 *	doesn't generate notification, and the main CPU
	 *	believes that the event has yet to complete.
	 *	Until there is a firm answer as to what is wrong,
	 *	and what the real fix might cost, this brute force
	 *	approach will get the ball rolling again.
	 *
	 *	The erroneous situation is easy to detect and
	 *	easy to kick start.
	 */
	rpc_coherency_hack_ticks = getbootint("RPC_COHERENCY_HACK_TICKS",
					rpc_coherency_hack_ticks);
	rpc_coherency_hack_set_timeout();
#endif	/* RPC_COHERENCY_HACK */
}


/*
 *	Initialize and start the NORMA2 port control thread.
 */
void dipc_control_server_init(int nhandles)
{
	server_entry1(dipc_control_server_init, nhandles);

	dipc_ring_buffer_init(&dipc_control_ring, nhandles + 1);
	dipc_control_ring_max = nhandles + 1;

	(void) kernel_thread(kernel_task, dipc_control_thread, 0);
}


/*
 *	Common routine to wakeup a dipc server thread (enqueue or control).
 *
 *	A wakeup is posted only if the thread is not currently running.
 *
 *	Called from interrupt level.
 */
static void dipc_thread_wakeup_common(
	int			event,
	volatile boolean_t	*cont,
	volatile boolean_t	*running )
{
	server_entry3(dipc_thread_wakeup_common, event, cont, running);

	if (*running) {
		*cont = TRUE;
	} else {
		*running = TRUE;
		thread_wakeup(event);
	}
}


/*
 *	Wake the enqueue server thread.
 */
static void dipc_enqueue_thread_wakeup(int event)
{
	server_entry1(dipc_enqueue_thread_wakeup, event);

	dipc_thread_wakeup_common(event,
		&dipc_enqueue_thread_continue, &dipc_enqueue_thread_running);
}


/*
 *	Wake the control server thread.
 */
static void dipc_control_thread_wakeup(int event)
{
	server_entry1(dipc_control_thread_wakeup, event);

	dipc_thread_wakeup_common(event,
		&dipc_control_thread_continue, &dipc_control_thread_running);
}


/*
 *	An enqueue request has arrived.
 *
 *	If the enqueue request could not be handled from interrupt
 *	level, add the now active RPC handle to the ring buffer,
 *	and inform the enqueue server thread.
 */
static void dipc_enqueue_request_arrival(rpc_handle_t rpc, rpc_notify_t notify)
{
	boolean_t	serviced;
	int		in;

	server_entry2(dipc_enqueue_request_arrival, rpc, notify);

	serviced = dipc_op_demux(rpc, FALSE, NORMA_RPC_CLASS_ENQUEUE,
			dipc_enqueue_request_arrival, notify);

	if (serviced == FALSE) {
		dipc_enqueue_ring[(in = dipc_enqueue_ring_in)] = rpc;
		if (++in == dipc_enqueue_ring_max)
			in = 0;
		dipc_enqueue_ring_in = in;
		dipc_enqueue_thread_wakeup((int) notify);
	}
}


/*
 *	A control request (transits, etc.) has arrived.
 *
 *	If the request could not be handled from interrupt level,
 *	add the now active RPC handle to the ring buffer,
 *	and inform the control server thread.
 */
static void dipc_control_request_arrival(rpc_handle_t rpc, rpc_notify_t notify)
{
	boolean_t	serviced;
	int		in;

	server_entry2(dipc_control_request_arrival, rpc, notify);

	serviced = dipc_op_demux(rpc, FALSE, NORMA_RPC_CLASS_CONTROL,
			dipc_control_request_arrival, notify);

	if (serviced == FALSE) {
		dipc_control_ring[(in = dipc_control_ring_in)] = rpc;
		if (++in == dipc_control_ring_max)
			in = 0;
		dipc_control_ring_in = in;
		dipc_control_thread_wakeup((int) notify);
	}
}


/*
 *	During initialization, post receives in a class for all RPC
 *	handles within a group.
 */
static int dipc_post_receives(
	rpc_group_t	group,
	rpc_class_t	class,
	int		size,
	void		(*callback)(rpc_handle_t, rpc_notify_t),
	rpc_notify_t	callarg)
{
	rpc_handle_t	rpc;
	int		cnt;

	server_entry5(dipc_post_receives, group, class, size, callback,callarg);

	cnt = 0;
	rpc = rpc_handle_alloc(group, FALSE, size);
	while (rpc != RPC_GROUP_EMPTY) {
		rpc_recv_request(class, rpc, callback, callarg);
		cnt++;
		rpc = rpc_handle_alloc(group, FALSE, size);
	}

	return cnt;
}


/*
 *	Post receives on all of the enqueue server thread's handles.
 */
static int dipc_enqueue_server_post_receives()
{
	server_entry0(dipc_enqueue_server_post_receives);

	return dipc_post_receives(NORMA_RPC_GROUP_ENQUEUE,
			NORMA_RPC_CLASS_ENQUEUE,
			rpc_engine_payload_size,
			dipc_enqueue_request_arrival,
			(rpc_notify_t) &dipc_enqueue_thread_continue );
}


/*
 *	Post receives on all of the control server thread's handles.
 */
int dipc_control_server_post_receives()
{
	server_entry0(dipc_control_server_post_receives);

	return dipc_post_receives(NORMA_RPC_GROUP_CONTROL,
			NORMA_RPC_CLASS_CONTROL,
			rpc_engine_payload_size,
			dipc_control_request_arrival,
			(rpc_notify_t) &dipc_control_thread_continue );
}


/*
 *	Configure priorities and thread scheduling policy.
 */
void dipc_enqueue_thread_configure()
{
	server_entry0(dipc_enqueue_thread_configure);

	thread_set_own_priority(dipc_enqueue_thread_pri);
	(void) thread_policy(current_thread(),
		dipc_enqueue_thread_policy, dipc_enqueue_thread_quantum);
}

void dipc_control_thread_configure()
{
	thread_t	thread;

	server_entry0(dipc_control_thread_configure);

	/*
	 *  The control thread is given VM privilege because it may need
	 *  to allocate memory (ie map entry for DIPC map) when migrating
	 *  messages.  If ALL the pageout reservations are for the port
	 *  being migrated (no data write completed message are coming
	 *  until this port is migrated) and the map entries zone is empty
	 *  when the OOL data is mapped in to the DIPC transmit map we
	 *  would hang in a deadlock.
	 *
	 *  We also need to use the pageout path (transmit maps and
	 *  RDMA group) because the "regular" deallocation thread
	 *  can write-lock the transmit map, deallocate address space
	 *  which triggers a vm_object_terminate() on an object that
	 *  has just begun migrating, which will deadlock the control
	 *  thread and the deallocation thread -- the deallocation
	 *  thread is holding the lock on the transmit map waiting
	 *  for the object to terminate, the control thread is
	 *  waiting for the lock on the transmit map and can't finish
	 *  the "pageout" of the messages under migration...
	 */
	thread = current_thread();
	thread->vm_privilege = TRUE;
	thread->dipc_ool_tx_map = dipc_pageout_map;
        thread->dipc_rdma_tx_group = NORMA_RDMA_GROUP_PAGEOUT;
	thread_set_own_priority(dipc_control_thread_pri);
	(void) thread_policy(current_thread(),
		dipc_control_thread_policy, dipc_control_thread_quantum);
}


/*
 *	Service requests for enqueue.
 */
static void dipc_enqueue_thread()
{
	rpc_handle_t	rpc;
	int		s, out, max = dipc_enqueue_ring_max;
	boolean_t	more, serviced;

	server_entry0(dipc_enqueue_thread);

	/*
	 *	Set any thread priorities, policies, etc.
	 */
	dipc_enqueue_thread_configure();


	/*
	 *	Hang a sign out front and go into business.
	 */
	dipc_enqueue_server_post_receives();
	for (;;) {

		dipc_enqueue_thread_continue = FALSE;

		/*
		 *	Drain the ring buffer of active RPC handles.
		 *
		 *	Service each request.
		 */
		while (dipc_enqueue_ring_in != (out = dipc_enqueue_ring_out)) {

			rpc = dipc_enqueue_ring[out];
			if (++out == max)
				out = 0;
			dipc_enqueue_ring_out = out;

			serviced = dipc_op_demux(rpc, TRUE,
				NORMA_RPC_CLASS_ENQUEUE,
				dipc_enqueue_request_arrival,
				(rpc_notify_t) &dipc_enqueue_thread_continue );

			if (serviced == FALSE)
				panic("dipc_enqueue_thread: not serviced\n");
		}

		/*
		 *	If one or more handles have since become active,
		 *	keep running.  Make a safe, quick check without
		 *	blocking interrupts.
		 */
		if (dipc_enqueue_thread_continue == TRUE)
			continue;

		/*
		 *	Looks like the request well has run dry...if
		 *	that is the case, block until something else
		 *	comes in.
		 */
		s = splsched();
		if ((more = dipc_enqueue_thread_continue) == FALSE) {
			dipc_enqueue_thread_running = FALSE;
			assert_wait((int) &dipc_enqueue_thread_continue, FALSE);
		}
		splx(s);

		if (more == FALSE)
			thread_block((void (*)()) 0);
	}
}



/*
 *	Service transit and queue available requests.
 */
static void dipc_control_thread()
{
	rpc_handle_t	rpc;
	int		s, out, max = dipc_control_ring_max;
	boolean_t	more, serviced;

	server_entry0(dipc_control_thread);

	dipc_control_thread_configure();
	dipc_control_server_post_receives();
	for (;;) {

		dipc_control_thread_continue = FALSE;

		/*
		 *	Drain the ring buffer of active RPC handles.
		 *
		 *	Service each request.
		 */
		while (dipc_control_ring_in != (out = dipc_control_ring_out)) {

			rpc = dipc_control_ring[out];
			if (++out == max)
				out = 0;
			dipc_control_ring_out = out;

			serviced = dipc_op_demux(rpc, TRUE,
				NORMA_RPC_CLASS_CONTROL,
				dipc_control_request_arrival,
				(rpc_notify_t) &dipc_control_thread_continue );

			if (serviced == FALSE)
				panic("dipc_control_thread: not serviced\n");
		}

		/*
		 *	If one or more handles have since become active,
		 *	keep running.  Make a safe, quick check without
		 *	blocking interrupts.
		 */
		if (dipc_control_thread_continue == TRUE)
			continue;

		/*
		 *	Looks like the request well has run dry...if
		 *	that is the case, block until something else
		 *	comes in.
		 */
		s = splsched();
		if ((more = dipc_control_thread_continue) == FALSE) {
			dipc_control_thread_running = FALSE;
			assert_wait((int) &dipc_control_thread_continue, FALSE);
		}
		splx(s);

		if (more == FALSE)
			thread_block((void (*)()) 0);
	}
}


/*
 *	Decode and service requests.
 *
 *	If called from interrupt level, the value for the
 *	canwait parameter must be set to FALSE.
 *
 *	Returns TRUE if the request was serviced, FALSE if it should
 *	be deferred to thread context.
 *
 *	It is an error to defer service when called with
 *	canwait == TRUE.
 */

#define SVR_OP(op) \
	server_log6((canwait ? 2 : 3), "%s: %s uid %x wait %d  hdl 0x%x\n",\
		__FUNC__, op, req->uid, canwait, rpc)

static boolean_t dipc_op_demux(
	rpc_handle_t	rpc,
	boolean_t	canwait,
	rpc_class_t	class,
	void		(*callback)( rpc_handle_t, rpc_notify_t ),
	rpc_notify_t	notify )
{
	dipc_header_t	*req;
	boolean_t	reply;

	server_entry5(dipc_op_demux, rpc, canwait, class, callback, notify);

	req = (dipc_header_t *) rpc_arguments(rpc);

	switch (req->type) {
	case DIPC_REQUEST_NOP:
		SVR_OP("DIPC_REQUEST_NOP");
		req->status = DIPC_OK;
		reply = TRUE;
		break;

	case DIPC_REQUEST_ENQUEUE:
		SVR_OP("DIPC_REQUEST_ENQUEUE");
		reply = dipc_op_enqueue((dipc_enqueue_op_t) req, canwait);
		break;

	case DIPC_REQUEST_BUY_TRANSITS:
		SVR_OP("DIPC_REQUEST_BUY_TRANSITS");
		reply = dipc_op_buy_transits((dipc_transit_op_t) req, canwait);
		break;

	case DIPC_REQUEST_SELL_TRANSITS:
		SVR_OP("DIPC_REQUEST_SELL_TRANSITS");
		reply = dipc_op_sell_transits((dipc_transit_op_t) req, canwait);
		break;

	case DIPC_REQUEST_MIGRATION:
		SVR_OP("DIPC_REQUEST_MIGRATION");
		reply = dipc_op_migration((dipc_migration_op_t) req, canwait);
		break;

	case DIPC_REQUEST_DEAD_NAME:
		SVR_OP("DIPC_REQUEST_DEAD_NAME");
		reply = dipc_op_dn_request((dipc_request_dead_op_t) req, canwait);
		break;

	case DIPC_NOTIFY_QUEUE_AVAIL:
		SVR_OP("DIPC_NOTIFY_QUEUE_AVAIL");
		reply = dipc_op_queue_avail(
				(dipc_queue_avail_op_t) req, canwait);
		break;

	case DIPC_NOTIFY_POLYMORPH:
		SVR_OP("DIPC_NOTIFY_POLYMORPH");
		reply = dipc_op_polymorph((dipc_polymorph_op_t) req, canwait);
		break;

	case DIPC_NOTIFY_DEAD_NAME:
		SVR_OP("DIPC_NOTIFY_DEAD_NAME");
		reply = dipc_op_dn_notify((dipc_notify_dead_op_t) req, canwait);
		break;

	default:
		server_log3(0, "%s: Unknown type: 0x%x\n", __FUNC__, req->type);
		req->status = DIPC_BOGUS_REQUEST;
		reply = TRUE;
		break;
	}

	if (reply) {
		rpc_send_reply_recv(class, rpc, callback, notify);
	} else {
		assert(canwait == FALSE);
	}

	return reply;
}


/*
 *	Enqueue a meta_kmsg on a port.
 *
 * implicit inputs:
 *	port is locked and referenced.
 *
 * implicit outputs:
 *	port is locked and referenced.
 *
 *	(Note that net and local kmsgs may also be supplied as an argument
 *	to this routine.)
 */
static void dipc_enqueue_meta_kmsg(
	ipc_port_t		port,
	meta_kmsg_t		mkm,
	mach_msg_option_t	options)
{
	ipc_mqueue_t		mqueue;
	ipc_pset_t		pset;
	ipc_thread_t		receiver;
	ipc_thread_queue_t	receivers;
	mach_msg_size_t		msize;
	int			event;
	extern ipc_port_t	dipc_emmi_reply_priv_port;
	extern ipc_port_t	dipc_emmi_reply_port;
	extern int		dipc_kobj_port_activate();

	server_entry3(dipc_enqueue_meta_kmsg, port, mkm, options);

	/*
	 *	Bump the message count on the destination port.
	 */
	port->ip_msgcount++;

	/*
	 *	Select the message queue for delivery based upon
	 *	the enqueue option bits (that are set by the sender by
	 *	looking at the message id), and attributes associated
	 *	with the destination port.
	 *
	 *	If the destination port is owned (and serviced)
	 *	by the kernel, choose among three delivery
	 *	methods:
	 *
	 *		1. a VM priv path that will be guaranteed to free
	 *		   up memory (m_o_d_write_completed, etc.).
	 *
	 *		2. satisfy a fault taken by a previous thread
	 *		   handling a regular kernel request (m_o_d_provided).
	 *
	 *		3. everything else (eg, r_device_open()).
	 *
	 *	-else-
	 *
	 *	If the destination is a port set, deliver to the port set's
	 *	message queue,
	 *
	 *	-else-
	 *
	 *	Deliver to the destination port.
	 */
	event = 0;
	if (port->ip_receiver == ipc_space_kernel) {
		if ( options & DIPC_EMMI_REPLY_PRIV ) {
			mqueue = &dipc_emmi_reply_priv_port->ip_messages;
			SERVER_STATS(emmi_reply_priv_enqueue++);
		} else if ( options & DIPC_EMMI_REPLY ) {
			mqueue = &dipc_emmi_reply_port->ip_messages;
			SERVER_STATS(emmi_reply_enqueue++);
		} else {
			event = dipc_kobj_port_activate(port);
			mqueue = &port->ip_messages;
			SERVER_STATS(kobj_enqueue++);
		}
	} else if ((pset = port->ip_pset) != IPS_NULL) {
		mqueue = &pset->ips_messages;
		SERVER_STATS(pset_enqueue++);
	} else {
		mqueue = &port->ip_messages;
		SERVER_STATS(port_enqueue++);
	}
	imq_lock(mqueue);

	/*
	 * attempt to find a compatible receiver.
	 */
	receivers = &mqueue->imq_threads;

	if (mkm->mkm_kmsg_type == IKM_KMSG_TYPE_NET) {
		msize = ((ipc_kmsg_t)mkm)->ikm_header.msgh_size;
	} else {
		msize = mkm->mkm_size - sizeof(struct ipc_kmsg)
			+ sizeof(mach_msg_header_t);
	}

	/*
	 *	Non-DIPC_EMMI_REPLY[_PRIV] kernel ports will not
	 *	have waiting receivers.  All other deliveries might.
	 */
	receiver = ipc_thread_queue_first(receivers);

	while ( receiver != ITH_NULL ) {

		ipc_thread_rmqueue_first_macro(receivers, receiver);
		assert(ipc_kmsg_queue_empty(&mqueue->imq_messages));

		if (msize > receiver->ith_msize) {
			/*
			 *	receiver waiting, but the receive
			 *	buffer is too small.
			 */
			server_log4(0,
				"%s: waiting receiver size too small %d > %d\n",
				__FUNC__, msize, receiver->ith_msize);

			receiver->ith_state = MACH_RCV_TOO_LARGE;
			receiver->ith_msize = msize;
			thread_go(receiver);
			receiver = ipc_thread_queue_first(receivers);
			continue;
		}

		/*
		 *	found a compatible receiver.
		 */
		server_log4(3, "%s: enqueue mkm=%x waiting-receiver=%x\n",
		  	__FUNC__, mkm, receiver);
		receiver->ith_state = MACH_MSG_SUCCESS;
		receiver->ith_kmsg = (ipc_kmsg_t) mkm;
		receiver->ith_seqno = port->ip_seqno++;
		imq_unlock(mqueue);
		thread_go(receiver);
		SERVER_STATS(waiting_receiver++);
		return;
	}

	/*
	 *	no receivers found, enqueue the message.
	 */
	server_log5(3, "%s: enqueue mkm=%x port=%x mqueue=%x\n",
			__FUNC__, mkm, port, mqueue);
	ipc_kmsg_enqueue_macro(&mqueue->imq_messages, (ipc_kmsg_t ) mkm);
	imq_unlock(mqueue);

	if (event) {
		thread_wakeup_one(event);
	}
}


/*
 *	In the common case, given a uid, find the port pointer.
 *
 *	Returns
 *	TRUE			if an error reply should be sent now.
 *					port is unlocked and no references.
 *
 *	FALSE			if the uid was validated,
 *					port is referenced and locked.
 *
 *	Replies
 *	DIPC_DEST_INVALID	if the uid is unknown.
 *	DIPC_DEST_DEAD		if the port is dead.
 *	DIPC_DEST_MOVED		if the port has been moved.
 */
static boolean_t dipc_op_validate_port(
	dipc_header_t	*req,
	ipc_port_t	*portp )
{
	ipc_port_t	port;

	server_entry2(dipc_op_validate_port, req, portp);

	/*
	 *	If this is a port that isn't in the uid table,
	 *	reply that it is invalid.
	 */
	if ((port = dipc_port_lookup(req->uid)) == IP_NULL) {
		server_log3(0,"%s: uid %x not found\n", __FUNC__, req->uid);
		req->status = DIPC_DEST_INVALID;
		return TRUE;
	}

	ip_lock(port);
	ip_reference(port);

	*portp = port;

	/*
	 *	If the port is a forwarding proxy, redirect the sender
	 *	to our best guess as to the node currently holding the
	 *	receive right. This occurs before the ip_active() check as
	 *	a forwarding proxy can be DEAD and still forwarding (uid is
	 *	valid).
	 */
	if ( port->dipc_forward ) {
		server_log5(0,"%s: proxy %x uid %x forwarding to node %d\n",
			__FUNC__,port,port->dipc_uid,port->dipc_node);
		req->status = DIPC_DEST_MOVED;
		req->node = port->dipc_node;
		ip_release(port);
		ip_unlock(port);
		return TRUE;
	}

	/*
	 *	If the port is dead, say so.
	 */
	if (!ip_active(port)) {
		server_log3(0, "%s: port %x is not active\n", __FUNC__, port);
		req->status = DIPC_DEST_DEAD;
		/*
		 *	probably need additional port right maintenance here.
		 */
		ip_release(port);
		ip_unlock(port);
		DIPC_PORT_LOG("dead-port",port);
		return TRUE;
	}

	req->status = DIPC_OK;

	/*
	 *	Okay, now we've got the local port.
	 *
	 *	we hold the port locked and a single port reference.
	 */

	return FALSE;
}

/*
 *	Service a "get transits" request.
 */
static boolean_t dipc_op_buy_transits(
	dipc_transit_op_t	req,
	boolean_t		canwait )
{
	ipc_port_t	port;

	server_entry2(dipc_op_buy_transits, req, canwait);

	if (canwait == FALSE)	/* not from interrupt level...yet */
		return FALSE;

	if (dipc_op_validate_port(&req->request, &port) == TRUE)
		return TRUE;

	dipc_give_transits(port, req->transits);

	ip_release(port);
	ip_unlock(port);

	req->request.status = DIPC_OK;
	SERVER_STATS(buy_transits++);
	return TRUE;
}


/*
 *	Service a "put transits" request.
 */
static boolean_t dipc_op_sell_transits(
	dipc_transit_op_t	req,
	boolean_t		canwait )
{
	ipc_port_t	port;

	server_entry2(dipc_op_sell_transits, req, canwait);

	if (canwait == FALSE)	/* not from interrupt level...yet */
		return FALSE;

	if (dipc_op_validate_port(&req->request, &port) == TRUE)
		return TRUE;

	dipc_reclaim_transits(port, req->transits);

	ip_release(port);
	ip_unlock(port);

	req->request.status = DIPC_OK;
	SERVER_STATS(sell_transits++);
	return TRUE;
}


/*
 *	Service an "enqueue message" request.
 *
 * side effects:
 *	on a successful enqueue a port reference is held.
 *
 */
static boolean_t dipc_op_enqueue(
	dipc_enqueue_op_t	enq,
	boolean_t		canwait )
{
	ipc_port_t		port;
	meta_kmsg_t		mkm;
	ipc_kmsg_t		kmsg;

	server_entry2(dipc_op_enqueue, enq, canwait);

	/*
	 *	can't enqueue from interrupt level...yet.
	 */
	if (canwait == FALSE) {
		return FALSE;
	}

	/*
	 *	Validate the port.  If the validation generated an error
	 *	reply, there is nothing more to do.
	 */
	if (dipc_op_validate_port(&enq->request, &port) == TRUE) {
		server_log3(0,"%s: uid %x invalid\n",__FUNC__,enq->request.uid);
		return TRUE;
	}


	/*
	 *	If the queue limit would be violated, and MACH_SEND_ALWAYS
	 *	is not requested, don't enqueue (and remember that the
	 *	sending node will probably have blocked senders).
	 */
	if ((port->ip_msgcount >= port->ip_qlimit) &&
	    ((enq->options & MACH_SEND_ALWAYS) == 0)) {
		if (canwait == FALSE) {
			ip_release(port);
			ip_unlock(port);
			return FALSE;
		}
		server_log3(1, "%s: port %x full\n", __FUNC__, port);

		dipc_blocked_sender_add(port, rdma_endpointof(enq->token));
		enq->request.status = DIPC_DEST_FULL;
		ip_release(port);
		ip_unlock(port);
		return TRUE;
	}


	/*
	 *	If the request contained a small kmsg in the cargo bay
	 *	of the request, try to allocate and enqueue it as
	 *	a kmsg.  If there are no kmsgs of the appropriate size
	 *	on hot standby, give up and try the meta-kmsg approach.
	 *
	 *	Note that this relies on kmsgs and meta-kmsgs having
	 *	a similar memory layout for the first few words.
	 *
	 *	It should be mentioned that when the optimization fails,
	 *	it fails hard; that is, the thread is never blocked
	 *	waiting for a kmsg just because there is a kmsg in the
	 *	payload bay.  And if an interrupt-level enqueue cannot
	 *	grab a kmsg, then the request is rewritten to ignore
	 *	the kmsg in the payload bay.
	 */
	if (enq->kmsg_type == IKM_KMSG_TYPE_NET) {
		server_log3(3, "%s: TYPE_NET on port %x\n", __FUNC__, port);

		if ((port->ip_msgcount == 0) &&
		    (kmsg = dipc_kmsg_grab(enq->kmsg_size)) != IKM_NULL) {
			bcopy(	(char *)(enq + 1),
				(char *)&kmsg->ikm_header,
				ikm_less_overhead(enq->kmsg_size));

			dipc_enqueue_meta_kmsg( port,
						(meta_kmsg_t) kmsg,
						enq->options);

			SERVER_STATS(fastpath_hits++);
			enq->request.status = DIPC_OK;

			/*
			 * hold a reference on the port to preserve the
			 * port and cover the send{-once}-right in the
			 * kmsg header 'msgh_remote_port' field.  Reference 
			 * is released during the kmsg dequeue process.
			 */
			ip_unlock(port);
			return TRUE;
		}

		/*
		 *	Modify the kmsg_type in place so that the reply
		 *	will indicate that the kmsg enqueue optimization
		 *	failed and was retried as a meta-kmsg.
		 */
		if (port->ip_msgcount != 0) {
			SERVER_STATS(fastpath_busy++);
			server_log3(3,
			      "%s: Fastpath attempt on port with %d messages\n",
						__FUNC__, port->ip_msgcount);
		} else {
			SERVER_STATS(fastpath_no_kmsg++);
			server_log2(1,
				"%s: Can't get kmsg for enqueue\n", __FUNC__);
		}
		enq->kmsg_type = IKM_KMSG_TYPE_META;
	}

	/*
	 *	Allocate and compose a meta-kmsg as a representation of
	 *	the kmsg residing on the sender's node.
	 *
	 *	If running in thread context, the call to meta_kmsg_alloc()
	 *	can block.
	 */
	if (enq->kmsg_type == IKM_KMSG_TYPE_META) {
		server_log3(3, "%s: TYPE_META on port %x\n", __FUNC__, port);

		mkm = meta_kmsg_alloc(enq->token, enq->kmsg_size, canwait);
		if (mkm == META_KMSG_NULL) {
			server_log2(0,"%s: Can't get meta_kmsg\n", __FUNC__);
			assert(canwait == FALSE);
			ip_release(port);
			ip_unlock(port);
			return FALSE;
		}

		mkm->mkm_size = enq->kmsg_size;
		mkm->mkm_kmsg_type = enq->kmsg_type;
		mkm->mkm_rdma_token = enq->token;
		mkm->mkm_dest_port = port;

		dipc_enqueue_meta_kmsg( port, mkm, enq->options );
		enq->request.status = DIPC_OK;

		/*
		 * hold a reference on the port to preserve the port and 
		 * cover the send{-once}-right in the kmsg header
		 * 'msgh_remote_port' field. Reference is released during the
		 * kmsg dequeue process.
		 */
		ip_unlock(port);

		return TRUE;
	}

	/*
	 *	Somehow, something sent a really bogus kmsg type...
	 */
	server_log3(0, "%s: BOGUS_REQUEST - %x\n", __FUNC__, enq->kmsg_type);

	assert(0);

	enq->request.status = DIPC_BOGUS_REQUEST;

	ip_release(port);
	ip_unlock(port);

	return TRUE;
}


/*
 *	Service a "queue available" request.
 */
static boolean_t dipc_op_queue_avail(
	dipc_queue_avail_op_t	req,
	boolean_t		canwait )
{
	ipc_port_t		port;
	ipc_thread_t		sender;

	server_entry2(dipc_op_queue_avail, req, canwait);

	if (canwait == FALSE) {
		return FALSE;
	}

	if (dipc_op_validate_port(&req->request, &port) == TRUE)
		return TRUE;

	req->request.status = DIPC_OK;

	/*
	 *	If there are any enqueue operations in progress,
	 *	record the arrival of the queue avail.
	 */
	if (port->dipc_enqueue_in_flight > 0) {
		server_log1(1, "dipc_op_queue_avail: Early queue avail\n");
		port->dipc_early_queue_avail++;
		SERVER_STATS(early_queue_avail++);
	}


	/*
	 *	Wakeup all blocked senders.
	 */
	while ((sender = ipc_thread_dequeue(&port->ip_blocked)) != ITH_NULL) {
		server_log2(1, "dipc_op_queue_avail: Waking %x\n", sender);
		sender->ith_state = MACH_MSG_SUCCESS;
		thread_go(sender);
		SERVER_STATS(awaken_blocked_sender++);
	}
	
	/*
	 *	Release the reference and lock taken in dipc_op_validate_port()
	 */
	ip_release(port);
	ip_unlock(port);

	return TRUE;
}


static boolean_t dipc_op_migration(
	dipc_migration_op_t	req,
	boolean_t		canwait )
{
	ipc_port_t	port;

	server_entry2(dipc_op_migration, req, canwait);

	if (canwait == FALSE) {
		return FALSE;
	}

	/*
	 * Is the port for this 'uid' here? Is this where the principal lives?
	 */
        if (dipc_op_validate_port(&req->request, &port) == TRUE) {
                server_log3(0,
                        "dipc_op_migration: bad validation uid %x status %d\n",
                        req->request.uid,
			req->request.status);
		/* req->request.status set by dipc_op_validate_port() */
                return TRUE;
        }

	/*
	 *retrieve the port migration state. any problems will panic below.
	 */
	 dipc_get_port_migration_state(	port,
					&req->state,
					req->from);

	/*
	 * port is locked and reference from dipc_op_validate_port()
	 */
	ip_release(port);
	ip_unlock(port);

	req->request.status = DIPC_OK;

	SERVER_STATS(migration++);
	return TRUE;
}


static boolean_t dipc_op_polymorph(
	dipc_polymorph_op_t	req,
	boolean_t		canwait )
{
	ipc_port_t	port;

	server_entry2(dipc_op_polymorph, req, canwait);

	if (canwait == FALSE) {
		return FALSE;
	}

	if (dipc_op_validate_port(&req->request, &port) == TRUE) {
		server_log2(0,
			"dipc_op_polymorph: bad validation (uid=%x)\n",
			req->request.uid);
		return TRUE;
	}

	ip_release(port);
	ip_unlock(port);

	dipc_port_migrate_messages(port);

	req->request.status = DIPC_OK;

	SERVER_STATS(polymorph++);
	return TRUE;
}


static boolean_t dipc_op_dn_request(
	dipc_request_dead_op_t	req,
	boolean_t		canwait )
{
	ipc_port_t	port;

	server_entry2(dipc_op_dn_request, req, canwait);

	if (canwait == FALSE) {
		return FALSE;
	}

	if (dipc_op_validate_port(&req->request, &port) == TRUE) {
		server_log2(0,
			"dipc_op_dn_request: bad validation (uid=%x)\n",
			req->request.uid);
		req->request.status = DIPC_DEST_INVALID;
		return TRUE;
	}

	(void) dipc_service_dead_name_request(req->from, port);

	ip_release(port);
	ip_unlock(port);

	req->request.status = DIPC_OK;

	SERVER_STATS(deadname_req++);
	return TRUE;

}


static boolean_t dipc_op_dn_notify(
	dipc_notify_dead_op_t	req,
	boolean_t		canwait )
{
	ipc_port_t	port;

	server_entry2(dipc_op_dn_notify, req, canwait);

	if (canwait == FALSE) {
		return FALSE;
	}

        (void) dipc_op_validate_port(&req->request, &port);

	server_log5(3, "%s: uid %x status 0x%x port %x\n",
			__FUNC__,
			req->request.uid,
			req->request.status,
			port);

	/*
	 * the port is locked and referenced ONLY in the case of 'DIPC_OK'.
	 */

	switch (req->request.status) {
	case DIPC_DEST_INVALID:
		server_log3(0,"%s: uid %x not found\n",
				__FUNC__,
				req->request.uid);
		break;

	case DIPC_DEST_DEAD:
		server_log3(0,"%s: port %x !ip_active\n", __FUNC__, port);
		break;

	case DIPC_OK:
		assert(DIPC_IS_PROXY(port));

		/* proxy lock & reference are consumed in the following call */
		dipc_service_dead_name_notify(port);

		/* port is now DEAD */
		break;

	case DIPC_DEST_MOVED:
		/*
		 * dipc_op_validate_port() releases the lock & reference in
		 * this case.
		 */ 
		ip_reference(port);
		ip_lock(port);

		/* port lock & reference are consumed in the following call */
		dipc_service_dead_name_notify(port);

		/* port is now DEAD */
		break;

	default:
		assert(0);
		panic("dipc_op_dn_notify: bogus return code");
		break;
	}

	SERVER_STATS(deadname_notify++);
	return TRUE;
}

/*
 * Peek at the ##!*# Mach message ID to determine if this message needs
 * special processing during the enqueue process. A thread will be assigned
 * to process this message ASAP.
 */

msg_emmi_val( ipc_kmsg_t kmsg )
{
	server_entry1(msg_emmi_val, kmsg);

	switch ( kmsg->ikm_header.msgh_id ) {
	case   2038:		/* memory_object_data_provided */
	case 923015:		/* proxy_data_supply */
		server_log3(3,"%s: EMMI_REPLY mid %d\n",
			__FUNC__,
			kmsg->ikm_header.msgh_id);
		return DIPC_EMMI_REPLY | MACH_SEND_ALWAYS;

	case   2042:		/* memory_object_data_write_completed */
	case 923016:		/* proxy_data_write_completed */
		server_log3(3, "%s: EMMI_REPLY_PRIV mid %d\n",
			__FUNC__,
			kmsg->ikm_header.msgh_id);
		return DIPC_EMMI_REPLY_PRIV | MACH_SEND_ALWAYS;

	  default:
		return DIPC_NOT_SPECIAL;
	}
}

db_server_stats()
{
	dipc_server_stats_t	*p = &dipc_server_stats;

        db_printf("dipc_server stats:\n");
	db_printf("  hack_invoked           %8d",   p->coherency_hack_invoked);
	db_printf("  hack_unstuck           %8d\n", p->coherency_hack_unstuck);
	
	db_printf("  emmi_reply_priv_enq    %8d",   p->emmi_reply_priv_enqueue);
	db_printf("  emmi_reply_enqueue     %8d\n", p->emmi_reply_enqueue);
	db_printf("  kobj_enqueue           %8d",   p->kobj_enqueue);
	db_printf("  pset_enqueue           %8d\n", p->pset_enqueue);
	db_printf("  port_enqueue           %8d",   p->port_enqueue);
	db_printf("  waiting_receiver       %8d\n", p->waiting_receiver);

	db_printf("  migration              %8d",   p->migration);
	db_printf("  polymorph              %8d\n", p->polymorph);

	db_printf("  deadname_req           %8d",   p->deadname_req);
	db_printf("  deadname_notify        %8d\n", p->deadname_notify);

	db_printf("  fastpath_hits          %8d",   p->fastpath_hits);
	db_printf("  fastpath_busy          %8d\n", p->fastpath_busy);
	db_printf("  fastpath_no_kms        %8d\n", p->fastpath_no_kmsg);

	return 0;
}
