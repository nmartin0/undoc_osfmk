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
 *	$Id: rpc.c,v 1.9 1995/01/31 19:43:13 andyp Exp $
 */

/*
 * SSD HISTORY
 * $Log: rpc.c,v $
 * Revision 1.9  1995/01/31  19:43:13  andyp
 * PTS #: 12233
 * Mandatory?:  No
 * Description: 3 different data structure initialization errors (two are
 *         quite similar) are corrected by these changes.  In RPC and RDMA,
 *         the last handle initialized does not have the next field
 *         correctly set to {RPC,RDMA}_GROUP_EMPTY.  No error would occur
 *         unless an attempt was made to allocate more handles into a group
 *         than exist (we don't do this, so it was never seen).
 *         Additionally, the rpc_rearm field needs to be initialized to 0.
 * Reviewer(s): stephan
 * Risk:        very low
 * Testing:     minimal
 * Module(s):
 *         M i860paragon/mcmsg/mcmsg_rpc.c
 *         M rpc_rdma/rdma.c
 *         M rpc_rdma/rpc.c
 *
 * Revision 1.8  1994/11/18  20:59:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/09/09  17:53:08  andyp
 * PTS #: 10760, 10618, 10660
 * Mandatory?:  10760==yes, others==no.
 * Description: The vnode pager needed to have access to a separate pool
 * 	of RDMA handles.  RPC/RDMA handle allocation has been
 * 	modified to preserve allocation request order.
 * 	Corrected a typo in kern/ast.h.
 * 	Reduced the overhead for page thawing (and removed old code).
 * 	Set ice_cube_pages=2 by default.
 * Reviewer(s): rkl
 * Risk:	     Medium
 * Testing:     byte sats, test case for 10660
 * Module(s):
 * 	M intel/pmap.c
 * 	M kern/ast.h
 * 	M norma2/dipc_client.h
 * 	M norma2/dipc_mqueue.c
 * 	M norma2/dipc_mqueue.h
 * 	M norma2/dipc_server.c
 * 	M norma2/dipc_server.h
 * 	M norma2/norma2_init.c
 * 	M norma2/norma_transport.h
 * 	M norma2/recv_engine.c
 * 	M rpc_rdma/rdma.c
 * 	M rpc_rdma/rpc.c
 * 	M vm/vm_user.c
 *
 * Revision 1.6  1994/08/31  21:25:43  mtm
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
 * Revision 1.4.2.1  1994/08/17  06:41:39  andyp
 * Merged in from the mainline the fixes for PTS #10438.
 *
 * Revision 1.5  1994/08/17  05:32:41  andyp
 * Made rpc_send_reply_recv() a single request to the MCP rather
 * than two with a spin loop in the middle.  This change also improves
 * inline Mach message latency by about 8%.
 *
 * Revision 1.4  1994/07/29  21:52:31  rkl
 *  Added function pointer as parameter to db_rpc_is_interesting() so DIPC
 *  could have the contents of the payload bay dumped.
 *
 * Revision 1.3  1994/07/22  16:37:16  andyp
 * Added support for displaying "interesting" RPC handles from !db_sys().
 *
 * Revision 1.2  1994/07/12  21:31:11  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.11  1994/03/24  00:15:36  andyp
 * Moved a sploff() up to cover the first access to the free list.
 *
 * Revision 1.1.2.10  1994/03/24  00:13:22  andyp
 * Added sploff()/splon() pairs during RPC handle free list operations
 * to permit disposal of RPC handles from interrupt level.
 *
 * Revision 1.1.2.9  1994/03/09  23:42:06  rkl
 * Non-blocking handle allocation wasn't fully initializing the handle
 * item data structures.
 *
 * Revision 1.1.2.8  1994/03/08  22:09:57  andyp
 * Added routines for installing an RPC callback after a request had
 * been posted.  The MCMSG engine already supported this -- I just
 * exported an interface.
 *
 * Revision 1.1.2.7  1994/02/24  20:12:48  stans
 *   rpc_request_arrival() had "assert(rpc_state_receiving(handle));" assert
 *   removed.
 *
 * Revision 1.1.2.6  1994/02/24  00:26:10  rkl
 *  Resolved problems (and bugs) reported by lint(1).
 *
 * Revision 1.1.2.5  1994/02/15  18:27:37  stans
 * added xxx_machine_init()
 *
 * Revision 1.1.2.4  1994/02/09  21:40:59  rkl
 * Polling routine to see if an RPC reply had departed wasn't
 * quite correct.  [andyp]
 *
 * Revision 1.1.2.3  1994/02/04  07:56:40  andyp
 * RPC flow control now works; RDMA is still under construction.
 *
 * Revision 1.1.2.2  1994/01/27  19:14:47  andyp
 * Changed RPC_SUCCESS to RPC_OK.  Moved rpc_engine.h into a machine-
 * specific rpc.h.  Added some user-mode accessible test points to
 * RPC (they'll be removed after testing).
 *
 * Revision 1.1.2.1  1994/01/26  18:38:38  andyp
 * Added some debugging routines and added external definitions
 * to rdma.h to conform with the spec.
 *
 * Revision 1.1  1994/01/25  00:51:02  andyp
 * First checkin of conforming RPC implementation for the NORMA rewrite.
 *
 *
 * END SSD HISTORY
 */

#include <mach_kdb.h>
#include <mach/boolean.h>
#include <mach/machine/kern_return.h>
#include <mach/machine/vm_types.h>
#include <kern/assert.h>
#include <kern/lock.h>
#include <kern/kalloc.h>
#include <kern/thread.h>
#include <ipc/ipc_thread.h>
#include <rpc_rdma/rpc.h>


int	rpc_max_handles, rpc_max_groups, rpc_max_classes;

#define	rpc_range_check(x, min, max)	(((x) >= (min)) && ((x) < (max)))
#define	rpc_valid_handle(h)	rpc_range_check(h, 0, rpc_max_handles)
#define	rpc_valid_group(g)	rpc_range_check(g, 0, rpc_max_groups)
#define	rpc_valid_class(c)	rpc_range_check(c, 0, rpc_max_classes)

#define RPC_UNGROUPED	((rpc_group_t) -1)


/*
 *	An internal rpc_handle_item (indexed by an rpc_handle_t)
 */
typedef struct rpc_handle_item {
	rpc_handle_t		rpc_next;	/* free or group "link" */
	rpc_group_t		rpc_group_id;	/* allocation group */
	rpc_state_t		rpc_state;	/* current state */
	void			*rpc_args;	/* pointer to payload bay */
	rpc_notify_t		rpc_callarg;	/* callback argument */
	void			(*rpc_callback)( rpc_handle_t, rpc_notify_t );
	int			_rpc_pad[2];
} *rpc_handle_item_t;

rpc_handle_t		rpc_item_float;	/* initial free list */
rpc_handle_item_t	rpc_item_vec;	/* vector of RPC resources */

#define rpc_handle_item_of(handle)	(&rpc_item_vec[(handle)])
#define rpc_handle_for_item(item)	((item) - rpc_item_vec)
#define	rpc_group_of(handle)		(rpc_handle_item_of(handle)->rpc_group_id)

/*
 *	RPC states
 */
#define	RPC_STATE_FLOATING	0
#define	RPC_STATE_IDLE		1
#define	RPC_STATE_READY		2
#define	RPC_STATE_RECEIVING	3
#define	RPC_STATE_REQUESTING	4
#define	RPC_STATE_SERVICING	5
#define	RPC_STATE_REPLYING	6

#define	RPC_STATE_MAX		7


/*
 *	RPC state query macros
 */
#define	rpc_get_handle_state(h)	(rpc_handle_item_of((h))->rpc_state)
#define	rpc_state_floating(h)	(rpc_get_handle_state(h) == RPC_STATE_FLOATING)
#define	rpc_state_idle(h)	(rpc_get_handle_state(h) == RPC_STATE_IDLE)
#define	rpc_state_ready(h)	(rpc_get_handle_state(h) == RPC_STATE_READY)
#define	rpc_state_receiving(h)	(rpc_get_handle_state(h) == RPC_STATE_RECEIVING)
#define	rpc_state_requesting(h)	(rpc_get_handle_state(h) == RPC_STATE_REQUESTING)
#define	rpc_state_servicing(h)	(rpc_get_handle_state(h) == RPC_STATE_SERVICING)
#define	rpc_state_replying(h)	(rpc_get_handle_state(h) == RPC_STATE_REPLYING)


/*
 *	Each RPC group id is an index into a vector of lists of
 *	free handles.
 */
typedef struct rpc_group_item {
	rpc_handle_t		grp_free_list;		/* free handles */
	int			grp_free_count;		/* no. free handles */
	int			grp_free_wanted;	/* handles wanted */
	int			grp_total;		/* total handles */
	struct ipc_thread_queue	grp_blocked;		/* blocked threads */
	decl_simple_lock_data(,	grp_lock)		/* MP locking */
} *rpc_group_item_t;

rpc_group_item_t	rpc_group_vec;	/* vector of RPC handle lists */
#define	rpc_group_item_of(group)	(&rpc_group_vec[(group)])


/*
 *	Internal initialization routines
 */
static rpc_return_t	rpc_init_groups( int );
static rpc_return_t	rpc_init_handles( int );


/*
 *	static rpc_return_t rpc_init_groups(ngroups)
 *
 *	PURPOSE
 *
 *	Initialize the RPC group vector.
 *
 *	RETURNS
 *
 *	RPC_OK		if resources could be allocated.
 *	RPC_SHORTAGE		if too many resources are needed.
 */
static rpc_return_t rpc_init_groups( int ngroups )
{
	vm_size_t		size;
	rpc_group_item_t	g;
	int			i;

	size = ngroups * sizeof(struct rpc_group_item);
	if ((g = (rpc_group_item_t) kalloc(size)) == 0) {
		return RPC_SHORTAGE;
	}

	rpc_group_vec = g;
	for (i = 0; i < ngroups; i++, g++) {
		g->grp_free_list = RPC_GROUP_EMPTY;
		g->grp_free_count = 0;
		g->grp_free_wanted = 0;
		g->grp_total = 0;
		ipc_thread_queue_init(&g->grp_blocked);
		simple_lock_init(&g->grp_lock);
	}

	return RPC_OK;
}


/*
 *	static void rpc_init_handle_item(h, next)
 *
 *	PURPOSE
 *
 *	Internal routine to initialize an RPC handle item.
 */
static void rpc_init_handle_item(rpc_handle_item_t h, rpc_handle_t next)
{
	h->rpc_next = next;
	h->rpc_group_id = RPC_UNGROUPED;
	h->rpc_state = RPC_STATE_FLOATING;
	h->rpc_args = 0;
	h->rpc_callback = 0;
	h->rpc_callarg = 0;
}


/*
 *	static rpc_return_t rpc_init_handles(nhandles)
 *
 *	PURPOSE
 *
 *	Initialize the RPC handles.
 *
 *	RETURNS
 *
 *	RPC_OK		if resources could be allocated.
 *	RPC_SHORTAGE		if too many resources are needed.
 */
static rpc_return_t rpc_init_handles( int nhandles )
{
	rpc_handle_item_t	h;
	int			i;
	vm_size_t		size;
	rpc_handle_t		next;

	size = nhandles * sizeof(struct rpc_handle_item);
	if ((h = (rpc_handle_item_t) kalloc(size)) == 0)
		return RPC_SHORTAGE;

	rpc_item_vec = h;
	next = 1;
	for (i = 0; i < nhandles - 1; i++, h++, next++)
		rpc_init_handle_item(h, next);
	rpc_init_handle_item(h, RPC_GROUP_EMPTY);
	rpc_item_float = 0;

	return RPC_OK;
}


/*
 *	rpc_return_t rpc_init(nhandles, ngroups, nclasses)
 *
 *	PURPOSE
 *
 *	Initialize and configure the RPC facilities to use the supplied
 *	numbers of RPC handles, groups, and classes.
 *
 *	NOTES
 *
 *	Must be called exactly once during system initialization.
 *
 *	RETURNS
 *
 *	RPC_OK		if resources could be allocated.
 *	RPC_SHORTAGE		if too many resources are needed.
 */
rpc_return_t rpc_init( int nhandles, int ngroups, int nclasses )
{
	kern_return_t	kr;

	if ((kr = rpc_init_groups(ngroups)) != RPC_OK)
		return kr;

	if ((kr = rpc_init_handles(nhandles)) != RPC_OK)
		return kr;

	if ((kr = rpc_init_engine(nhandles, ngroups, nclasses)) != RPC_OK)
		return kr;

	rpc_max_handles = nhandles;
	rpc_max_groups = ngroups;
	rpc_max_classes = nclasses;

	return RPC_OK;
}


/*
 *	rpc_return_t rpc_group_alloc(group, nhandles)
 *
 *	PURPOSE
 *
 *	Associate the supplied number of RPC handles with an RPC group.
 *
 *	NOTES
 *
 *	Must be called exactly once during system initialization.
 *
 *	RETURNS
 *
 *	RPC_OK		if the handles are placed within the group
 *	RPC_INVALID_GROUP	if the group is invalid
 *	RPC_HANDLE_SHORTAGE	if not enough handles to place into the group
 */
rpc_return_t rpc_group_alloc( rpc_group_t group, int nhandles )
{
	rpc_handle_t		handle;
	rpc_group_item_t	g;
	rpc_handle_item_t	h;
	int			i;

	if ((group < 0) || (group >= rpc_max_groups))
		return RPC_INVALID_GROUP;

	g = rpc_group_item_of(group);
	simple_lock(&g->grp_lock);

	for (i = 0; i < nhandles; i++) {

		/*
		 *	if no more handles, quit.
		 */
		if ((handle = rpc_item_float) == RPC_GROUP_EMPTY) {
			simple_unlock(&g->grp_lock);
			return RPC_HANDLE_SHORTAGE;
		}

		/*
		 *	remove the handle from the floating list,
		 *	make it a member of the group,
		 *	and mark it <idle>.
		 */
		h = rpc_handle_item_of(handle);
		rpc_item_float = h->rpc_next;

		h->rpc_state = RPC_STATE_IDLE;
		h->rpc_group_id = group;
		h->rpc_next = g->grp_free_list;
		g->grp_free_list = handle;
		g->grp_free_count++;
		g->grp_total++;
	}

	simple_unlock(&g->grp_lock);

	return RPC_OK;
}


/*
 *	static void rpc_handle_grab(group, argsize, reserve)
 *
 *	PURPOSE
 *
 *	Internal routine to attempt to allocate an RPC
 *	handle from a group.
 *
 *	RETURNS
 *
 *	an RPC handle		if a handle has been allocated
 *	RPC_GROUP_EMPTY		if wait is FALSE and the group is empty.
 */
static rpc_handle_t rpc_handle_grab(
	rpc_group_t	group,
	int		argsize,
	boolean_t	reserve )
{
	rpc_group_item_t	g;
	rpc_handle_item_t	h;
	rpc_handle_t		handle;
	thread_t		self;
	int			s;

	g = rpc_group_item_of(group);

	simple_lock(&g->grp_lock);

	s = sploff();
	if ((handle = g->grp_free_list) != RPC_GROUP_EMPTY) {

		assert(rpc_valid_handle(handle));
		assert(rpc_state_idle(handle));
		assert(rpc_valid_group(rpc_group_of(handle)));
		assert(rpc_group_of(handle) == group);

		h = rpc_handle_item_of(handle);
		g->grp_free_list = h->rpc_next;
		g->grp_free_count--;

		splon(s);

		h->rpc_next = RPC_GROUP_EMPTY;
		h->rpc_state = RPC_STATE_READY;
		h->rpc_args = rpc_engine_alloc_payload(handle, argsize);
	} else {
		if (reserve) {
			self = current_thread();
			g->grp_free_wanted++;
			ipc_thread_enqueue_macro(&g->grp_blocked, self);
			thread_will_wait(self);
		}
		splon(s);
	}

	simple_unlock(&g->grp_lock);

	return handle;

}


/*
 *	static void rpc_handle_wait(group, argsize)
 *
 *	PURPOSE
 *
 *	Internal routine to block the calling thread and wait
 *	for a free RPC handle within a group.
 *
 *	RETURNS
 *
 *	An RPC handle ready for use.
 */
static rpc_handle_t  rpc_handle_wait( rpc_group_t group, int argsize )
{
	rpc_handle_t		h;

	while ((h = rpc_handle_grab(group, argsize, TRUE)) == RPC_GROUP_EMPTY) {
		thread_block(0);
	}

	return h;
}


/*
 *	rpc_handle_t rpc_handle_alloc(group, wait, argsize)
 *
 *	PURPOSE
 *
 *	Allocate an RPC handle from an RPC handle group.
 *
 *	NOTES
 *
 *	If wait is true and the group is empty, the caller will be blocked
 *	until a handle within the group becomes available.
 *
 *	argsize specifies the maximum number of bytes to be used in any RPC
 *	(request or reply) that will be processed on the handle returned.
 *
 *	RETURNS
 *
 *	an RPC handle		if a handle has been allocated
 *	RPC_GROUP_EMPTY		if wait is FALSE and the group is empty.
 */
rpc_handle_t rpc_handle_alloc( rpc_group_t group, boolean_t wait, int argsize )
{
	if (wait == FALSE)
		return rpc_handle_grab(group, argsize, FALSE);

	return rpc_handle_wait(group, argsize);
}


/*
 *	void rpc_handle_free(handle)
 *
 *	PURPOSE
 *
 *	Return an RPC handle to the group of RPC handles from which
 *	it was allocated.
 *
 *	NOTES
 *
 *	This call should not be made while any RPC operations associated with
 *	the supplied handle are still in progress.
 *
 */
void rpc_handle_free( rpc_handle_t handle )
{
	rpc_group_item_t	g;
	rpc_handle_item_t	h;
	boolean_t		w;
	int			s;
	thread_t		th;

	assert(rpc_valid_handle(handle));
	assert(rpc_state_ready(handle));
	assert(rpc_valid_group(rpc_group_of(handle)));

	h = rpc_handle_item_of(handle);
	g = rpc_group_item_of(h->rpc_group_id);

	simple_lock(&g->grp_lock);
	{
		/*
		 *	mark the handle as <idle> and
		 *	prepend it to the group's free list.
		 */
		h->rpc_args = rpc_engine_free_payload(handle);
		h->rpc_state = RPC_STATE_IDLE;
		s = sploff();
		h->rpc_next = g->grp_free_list;
		g->grp_free_list = handle;
		g->grp_free_count++;

		/*
		 *	if at least one thread is waiting for a
		 *	handle within this group, wake one.
		 */
		if ((w = (g->grp_free_wanted > 0))) {
			g->grp_free_wanted--;
			th = ipc_thread_queue_first(&g->grp_blocked);
			assert(th != ITH_NULL);
			ipc_thread_rmqueue_first_macro(&g->grp_blocked, th);
		}
		splon(s);
	}
	simple_unlock(&g->grp_lock);

	if (w) thread_go(th);
}


/*
 *	rpc_args_t *rpc_arguments(handle)
 *
 *	PURPOSE
 *
 *	Return a pointer to a buffer associated with the supplied RPC handle.
 *
 *	NOTES
 *
 *	RETURNS
 *
 *	A pointer to a buffer.
 */
rpc_args_t rpc_arguments( rpc_handle_t handle )
{
	assert(rpc_valid_handle(handle));
	assert(!rpc_state_idle(handle));

	return (rpc_args_t) (rpc_handle_item_of(handle)->rpc_args);
}


/*
 *	void rpc_send_request(node, class, handle, callback, callarg)
 *
 *	PURPOSE
 *
 *	Initiate an RPC request to the class of receivers on the
 *	specified node.
 *
 *	NOTES
 *
 *	If callback is non-null, the callback function will be invoked
 *	when the corresponding reply arrives.
 *
 */
void rpc_send_request(
	rpc_node_t	node,
	rpc_class_t	class,
	rpc_handle_t	handle,
	void		(*callback)( rpc_handle_t, rpc_notify_t ),
	rpc_notify_t	callarg)
{
	rpc_handle_item_t	h;

	assert(rpc_valid_handle(handle));
	assert(rpc_valid_class(class));
	assert(rpc_state_ready(handle));

	h = rpc_handle_item_of(handle);

	h->rpc_callback = callback;
	h->rpc_callarg = callarg;
	h->rpc_state = RPC_STATE_REQUESTING;
	rpc_engine_send_request((int) node, (int) class,
				(int) handle, callback != 0);
}


/*
 *	void rpc_recv_request(class, handle, callback, callarg)
 *
 *	PURPOSE
 *
 *	Initiate an RPC receive within the supplied class.
 *
 *	NOTES
 *
 *	If callback is non-null, the callback function will be invoked
 *	when an RPC request arrives.
 *
 */
void rpc_recv_request(
	rpc_class_t	class,
	rpc_handle_t	handle,
	void		(*callback)( rpc_handle_t, rpc_notify_t ),
	rpc_notify_t 	callarg)
{
	rpc_handle_item_t	h;

	assert(rpc_valid_handle(handle));
	assert(rpc_valid_class(class));
	assert(rpc_state_replying(handle) || rpc_state_ready(handle));

	h = rpc_handle_item_of(handle);

	h->rpc_callback = callback;
	h->rpc_callarg = callarg;
	h->rpc_state = RPC_STATE_RECEIVING;
	rpc_engine_recv_request((int) class, (int) handle, callback != 0);
}


/*
 *	void rpc_send_reply(handle, callback, callarg)
 *
 *	PURPOSE
 *
 *	Initiate an RPC reply.
 *
 *	NOTES
 *
 *	If callback is non-null, the callback function will be invoked when
 *	the RPC reply has been sent.
 *
 */
void rpc_send_reply(
	rpc_handle_t	handle,
	void		(*callback)( rpc_handle_t, rpc_notify_t ),
	rpc_notify_t	callarg)
{
	rpc_handle_item_t	h;

	assert(rpc_valid_handle(handle));
	assert(rpc_state_servicing(handle));

	h = rpc_handle_item_of(handle);

	h->rpc_callback = callback;
	h->rpc_callarg = callarg;
	h->rpc_state = RPC_STATE_REPLYING;
	rpc_engine_send_reply((int) handle, callback != 0);
}


/*
 *	void rpc_send_reply_recv(class, handle, callback, callarg)
 *
 *	PURPOSE
 *
 *	Initiate an RPC reply and an RPC receive request.
 *
 *	NOTES
 *
 *	RPC reply departure callbacks are not available with this
 *	interface.  The callback function used here, if invoked,
 *	will be called as in rpc_recv_request().
 *
 *	{ This will become one operation eventually }
 *
 */
void rpc_send_reply_recv(
	rpc_class_t	class,
	rpc_handle_t	handle,
	void		(*callback)( rpc_handle_t, rpc_notify_t ),
	rpc_notify_t 	callarg)
{
	rpc_handle_item_t	h;

	assert(rpc_valid_handle(handle));
	assert(rpc_valid_class(class));
	assert(rpc_state_servicing(handle));

	h = rpc_handle_item_of(handle);

	h->rpc_callback = callback;
	h->rpc_callarg = callarg;
	h->rpc_state = RPC_STATE_RECEIVING;
	rpc_engine_send_reply_recv((int) class, (int) handle, callback != 0);
}


/*
 *	boolean_t rpc_send_ready(handle)
 *
 *	PURPOSE
 *
 *	Poll local state to see if the reply from an RPC request has arrived.
 *
 *	NOTES
 *
 *	If a reply arrival callback notification had been requested,
 *	this routine will return TRUE if called from inside the
 *	callback function.
 *
 *	RETURNS
 *
 *	TRUE		if an RPC reply to a request has arrived.
 *	FALSE		if the reply has yet to arrive.
 *
 */
boolean_t rpc_send_ready( rpc_handle_t handle )
{
	rpc_handle_item_t	h;

	assert(rpc_valid_handle(handle));
	assert(!rpc_state_idle(handle));
	assert(!rpc_state_receiving(handle));
	assert(!rpc_state_servicing(handle));
	assert(!rpc_state_replying(handle));

	h = rpc_handle_item_of(handle);

	/*
	 *	if it's already marked <ready>, say so.
	 */
	if (h->rpc_state == RPC_STATE_READY)
		return TRUE;

	/*
	 *	ask the engine (and update)
	 */
	if (rpc_engine_status(handle) == RPC_ENGINE_IDLE) {
		h->rpc_state = RPC_STATE_READY;
		return TRUE;
	}

	return FALSE;

}


/*
 *	boolean_t rpc_recv_ready(handle)
 *
 *	PURPOSE
 *
 *	Poll local state to see if an RPC request has arrived.
 *
 *	NOTES
 *
 *	If a request arrival callback notification had been requested,
 *	this routine will return TRUE if called from inside the
 *	callback function.
 *
 *	RETURNS
 *
 *	TRUE		if an RPC request has arrived.
 *	FALSE		if a request yet to arrive.
 *
 */
boolean_t rpc_recv_ready( rpc_handle_t handle )
{
	rpc_handle_item_t	h;

	assert(rpc_valid_handle(handle));
	assert(!rpc_state_idle(handle));
	assert(!rpc_state_ready(handle));
	assert(!rpc_state_requesting(handle));
	assert(!rpc_state_replying(handle));

	h = rpc_handle_item_of(handle);

	/*
	 *	if it's already marked <servicing>, say so.
	 */
	if (h->rpc_state == RPC_STATE_SERVICING)
		return TRUE;

	/*
	 *	ask the engine (and update)
	 */
	if (rpc_engine_status(handle) == RPC_ENGINE_IDLE) {
		h->rpc_state = RPC_STATE_SERVICING;
		return TRUE;
	}

	return FALSE;

}


/*
 *	boolean_t rpc_reply_ready(handle)
 *
 *	PURPOSE
 *
 *	Poll local state to see if an RPC reply has been sent.
 *
 *	NOTES
 *
 *	If a reply sent callback notification had been requested,
 *	this routine will return TRUE if called from inside the
 *	callback function.
 *
 *	RETURNS
 *
 *	TRUE		if the RPC reply has been sent.
 *	FALSE		if the reply has not yet been sent.
 *
 */
boolean_t rpc_reply_ready( rpc_handle_t handle )
{
	rpc_handle_item_t	h;

	assert(rpc_valid_handle(handle));
	assert(!rpc_state_idle(handle));
	assert(!rpc_state_requesting(handle));
	assert(!rpc_state_receiving(handle));
	assert(!rpc_state_servicing(handle));

	h = rpc_handle_item_of(handle);

	/*
	 *	if it's already marked <ready>, say so.
	 */
	if (h->rpc_state == RPC_STATE_READY)
		return TRUE;

	/*
	 *	ask the engine (and update)
	 */
	if (rpc_engine_status(handle) == RPC_ENGINE_IDLE) {
		h->rpc_state = RPC_STATE_READY;
		return TRUE;
	}

	return FALSE;
}


/*
 *	static void rpc_invoke_callback(handle, state)
 *
 *	PURPOSE
 *
 *	Internal common routine to invoke an RPC callback
 *	and switch to a new state.
 *
 *	NOTES
 *
 *	This routine is called from interrupt level.
 */
static void rpc_invoke_callback( rpc_handle_t handle, rpc_state_t newstate )
{
	rpc_handle_item_t	h;
	rpc_notify_t		a;
	void			(*f)( rpc_handle_t, rpc_notify_t );

	h = rpc_handle_item_of(handle);
	f = h->rpc_callback;
	a = h->rpc_callarg;
	assert(f != 0);

	h->rpc_callback = 0;
	h->rpc_callarg = 0;
	h->rpc_state = newstate;
	(*f)(handle, a);
}


/*
 *	void rpc_request_arrival(handle)
 *
 *	PURPOSE
 *
 *	Called from underlying mechanisms when an RPC request
 *	arrival notification was requested.
 *
 *	NOTES
 *
 *	This routine is called from interrupt level.
 */
void rpc_request_arrival( rpc_handle_t handle )
{
	assert(rpc_valid_handle(handle));
	rpc_invoke_callback(handle, RPC_STATE_SERVICING);
}


/*
 *	void rpc_reply_arrival(handle)
 *
 *	PURPOSE
 *
 *	Called from underlying mechanisms when an RPC reply
 *	arrival notification was requested.
 *
 *	NOTES
 *
 *	This routine is called from interrupt level.
 */
void rpc_reply_arrival( rpc_handle_t handle )
{
	assert(rpc_valid_handle(handle));
	assert(rpc_state_requesting(handle));

	rpc_invoke_callback(handle, RPC_STATE_READY);
}


/*
 *	void rpc_reply_depart(handle)
 *
 *	PURPOSE
 *
 *	Called from underlying mechanisms when an RPC reply
 *	departure notification has been requested.
 *
 *	NOTES
 *
 *	This routine is called from interrupt level.
 */
void rpc_reply_depart( rpc_handle_t handle )
{
	assert(rpc_valid_handle(handle));
	assert(rpc_state_replying(handle));

	rpc_invoke_callback(handle, RPC_STATE_READY);
}


boolean_t rpc_set_callback(
	rpc_handle_t	handle,
	void		(*callback)( rpc_handle_t, rpc_notify_t ),
	rpc_notify_t 	callarg)
{
	rpc_handle_item_t	h;
	boolean_t	already;
	rpc_notify_t	olda;
	void		(*oldf)( rpc_handle_t, rpc_notify_t );

	assert(rpc_valid_handle(handle));
	assert(!rpc_state_idle(handle));

	h = rpc_handle_item_of(handle);
	oldf = h->rpc_callback;
	olda = h->rpc_callarg;
	h->rpc_callback = callback;
	h->rpc_callarg = callarg;

	if ((already = rpc_engine_set_notify(handle)) == TRUE) {
		h->rpc_callback = oldf;
		h->rpc_callarg = olda;
	}

	return already;
}


#define	RPC_TEST	1
#if	RPC_TEST
/*
 *	Dangerous user-mode accessible RPC test points
 *	that *will* go away.
 */
int syscall_rpc_alloc( rpc_group_t group, boolean_t wait, vm_size_t size )
{
	assert(rpc_valid_group(group));
	return (int) rpc_handle_alloc(group, wait, size);
}


int syscall_rpc_free( rpc_handle_t handle )
{
	assert(rpc_valid_handle(handle));
	rpc_handle_free(handle);
	return 0;
}


/*ARGSUSED*/
static void syscall_rpc_generic_wakeup(rpc_handle_t handle, rpc_notify_t arg)
{
	thread_wakeup((int) arg);
}


int syscall_rpc_recv( rpc_class_t class, rpc_handle_t handle, vm_offset_t payload )
{
	rpc_notify_t	arg;
	extern int	rpc_engine_payload_size;	/* <<< */

	assert(rpc_valid_class(class));
	assert(rpc_valid_handle(handle));

	arg = (rpc_notify_t) &arg;	/* nifty */

	assert_wait((int) arg, FALSE);
	rpc_recv_request(class, handle, syscall_rpc_generic_wakeup, arg);
	thread_block(0);

	copyout(rpc_arguments(handle), payload, rpc_engine_payload_size);
	return 0;
}


int syscall_rpc_reply( rpc_handle_t handle, vm_offset_t payload )
{
	rpc_notify_t	arg;
	extern int	rpc_engine_payload_size;	/* <<< */

	assert(rpc_valid_handle(handle));

	arg = (rpc_notify_t) &arg;

	copyin(payload, rpc_arguments(handle), rpc_engine_payload_size);

	assert_wait((int) arg, FALSE);
	rpc_send_reply(handle, syscall_rpc_generic_wakeup, arg);
	thread_block(0);

	return 0;
}


int syscall_rpc_send(
	rpc_node_t	node,
	rpc_class_t	class,
	rpc_handle_t	handle,
	vm_offset_t	payload,
	boolean_t	spin)
{
	rpc_notify_t	arg;
	extern int	rpc_engine_payload_size;	/* <<< */

	assert(rpc_valid_class(class));
	assert(rpc_valid_handle(handle));

	copyin(payload, rpc_arguments(handle), rpc_engine_payload_size);

	if (spin) {
		rpc_send_request(node, class, handle, 0, 0);
		while (!rpc_send_ready(handle))
			;
	} else {
		arg = (rpc_notify_t) &arg;
		assert_wait((int) arg, FALSE);
		rpc_send_request(node, class, handle, syscall_rpc_generic_wakeup, arg);
		thread_block(0);
	}

	copyout(rpc_arguments(handle), payload, rpc_engine_payload_size);

	return 0;
}
#endif	/* RPC_TEST */


#if	MACH_KDB

/*
 *	Debugging support.
 */

static char *rpc_handle_state_ascii[] = {
	"<ungrouped>",
	"<free>",
	"<ready>",
	"<receiving>",
	"<requesting>",
	"<servicing>",
	"<replying>"
};


/*
 *	Pretty print an "rpc_handle_item", then ask the
 *	RPC engine to print out what it knows about
 *	the supplied handle.
 */
int rpc_print_handle( rpc_handle_t handle )
{
	rpc_handle_item_t	h;
	char			*s;
	extern int		indent;
	extern int		rpc_print_engine( int );

	if (!rpc_valid_handle(handle)) {
		db_printf("rpc_print_handle: rpc_max_handles=%d\n",
			rpc_max_handles);
		return -1;
	}

	h = rpc_handle_item_of(handle);

	iprintf("rpc handle=%d (0x%x), group=%d, state=%d ",
		handle, h,
		h->rpc_group_id,
		h->rpc_state);

	if ((h->rpc_state < 0) || (h->rpc_state > RPC_STATE_REPLYING))
		s = "<huh?>";
	else
		s = rpc_handle_state_ascii[h->rpc_state];

	iprintf("%s {\n", s);
	indent += 2;

	iprintf("next=%d (0x%x), args=0x%x, callback=0x%x, callarg=0x%x\n",
		h->rpc_next, h->rpc_next,
		h->rpc_args,
		h->rpc_callback,
		h->rpc_callarg);

	(void) rpc_print_engine((int) handle);

	indent -= 2;
	iprintf("}\n");

	return handle;
}


/*
 *	Pretty print *all* RPC handles.
 */
int rpc_print_all_handles()
{
	rpc_handle_t	handle;

	for (handle = 0; handle < rpc_max_handles; handle++)
		(void) rpc_print_handle(handle);

	return rpc_max_handles;
}


static boolean_t db_rpc_is_interesting(
	rpc_handle_t	handle,
	boolean_t	recvtoo )
{
	rpc_handle_item_t	h;
	int			s;

	h = rpc_handle_item_of(handle);

	switch (h->rpc_state) {
	case RPC_STATE_RECEIVING:
		/*
		 *	receives that have not yet been serviced
		 *	are interesting.
		 */
		s = rpc_engine_status(handle);
		if ((s == RPC_ENGINE_BUSY) && (recvtoo == FALSE))
			return FALSE;
		/* fall through */
	case RPC_STATE_REQUESTING:
	case RPC_STATE_SERVICING:
	case RPC_STATE_REPLYING:
		return TRUE;
	}

	return FALSE;
}


/*
 *	Pretty print all interesting RPC handles.
 */
int rpc_print_interesting_handles(
	char		*banner,
	rpc_group_t	group,
	boolean_t	recvtoo,
	void		(*func)())
{
	rpc_handle_t		handle;
	rpc_handle_item_t	h;
	rpc_group_item_t	g;
	int			i, any;
	int			states[RPC_STATE_MAX];
	char			*nl = "\n";

	for (i = 0; i < RPC_STATE_MAX; i++)
		states[i] = 0;

	any = 0;
	for (handle = 0; handle < rpc_max_handles; handle++) {
		h = rpc_handle_item_of(handle);
		if (h->rpc_group_id != group)
			continue;
		states[h->rpc_state]++;
		if (db_rpc_is_interesting(handle, recvtoo) == TRUE)
			any++;
	}

	g = rpc_group_item_of(group);
	db_printf("RPC \"%s\": [group=%d total=%d free=%d wanted=%d]\n",
		banner,
		group,
		g->grp_total,
		g->grp_free_count,
		g->grp_free_wanted);

	db_printf("summary:");
	for (i = 0; i < RPC_STATE_MAX; i++) {
		if (states[i] == 0)
			continue;
		db_printf(" %s=%d", rpc_handle_state_ascii[i], states[i]);
	}
	db_printf(nl);

	if (any == 0) {
		db_printf(nl);
		return 0;
	}


	db_printf("rpc grp        state node c b i r   prev     next   *payload callback callarg\n");
	for (handle = 0; handle < rpc_max_handles; handle++) {

		h = rpc_handle_item_of(handle);
		if (h->rpc_group_id != group)
			continue;

		if (db_rpc_is_interesting(handle, recvtoo) == FALSE)
			continue;

		db_printf(" %02x %3d %12s ",
			handle,
			h->rpc_group_id,
			rpc_handle_state_ascii[h->rpc_state]);

		(void) rpc_print_interesting_engine((int) handle);
		
		db_printf(" %08x %08x %08x\n",
			h->rpc_args,
			h->rpc_callback,
			h->rpc_callarg);

		if (func)
			(*func)(handle);
	}

	db_printf(nl);

	return any;
}


/*
 *	Pretty print an RPC handle group.
 */
int rpc_print_group( rpc_group_t group )
{
	rpc_group_item_t	g;
	rpc_handle_item_t	h;
	rpc_handle_t		handle;
	thread_t		th, first;
	task_t			task;
	int			i, task_id, thread_id;
	extern int		indent;
	char			*s;
	int			allocated, free;

	if (!rpc_valid_group(group)) {
		db_printf("rpc_print_group: rpc_max_groups=%d\n",
			rpc_max_groups);
		return -1;
	}

	g = rpc_group_item_of(group);

	iprintf("rpc group=%d (0x%x), total=%d, free=%d, wanted=%d {\n",
		group, g,
		g->grp_total,
		g->grp_free_count,
		g->grp_free_wanted);

	indent += 2;

	iprintf("threads blocked for handles:\n");
	first = g->grp_blocked.ithq_base;
	indent += 2;
	if (first != ITH_NULL) {
		th = first;
		i = 0;
		do {
			task = th->task;
			task_id = db_lookup_task(task);
			thread_id = db_lookup_thread(th);
			iprintf("%2d 0x%x [ $task%d.%d ]\n",
				i,
				th,
				task_id, thread_id);
			i++;
			th = th->ith_next;
		} while (th != first);
	} else {
		iprintf("[ none ]\n");
	}
	indent -= 2;

	iprintf("allocated {\n");
	indent += 2;
	allocated = 0;
	for (handle = 0; handle < rpc_max_handles; handle++) {
		h = rpc_handle_item_of(handle);
		if (h->rpc_group_id != group)
			continue;
		if (h->rpc_state == RPC_STATE_IDLE)
			continue;
		if ((h->rpc_state < 0) || (h->rpc_state > RPC_STATE_REPLYING))
			s = "<huh?>";
		else
			s = rpc_handle_state_ascii[h->rpc_state];
		iprintf("handle=%3d state=%s\n", handle, s);
		allocated++;
	}
	iprintf("allocated=%d (0x%x)\n", allocated, allocated);
	indent -= 2;
	iprintf("}\n");

	iprintf("free {\n");
	indent += 2;
	handle = g->grp_free_list;
	free = 0;
	while (handle != RPC_GROUP_EMPTY) {
		h = rpc_handle_item_of(handle);
		iprintf("handle=%d (0x%x), next=%d\n", handle, h, h->rpc_next);
		handle = h->rpc_next;
		free++;
	}
	iprintf("free=%d (0x%x)\n", free, free);
	indent -= 2;
	iprintf("}\n");

	indent -= 2;
	iprintf("}\n");

	return group;
}


/*
 *	Pretty print *all* RPC groups.
 */
int rpc_print_all_groups()
{
	rpc_group_t	group;

	for (group = 0; group < rpc_max_groups; group++)
		(void) rpc_print_group(group);

	return rpc_max_groups;
}


/*
 *	Pretty print an RPC class.
 */
int rpc_print_class( rpc_class_t class )
{
	extern int	rpc_print_engine_class( rpc_class_t );

	if (!rpc_valid_class(class)) {
		db_printf("rpc_print_class: rpc_max_classes=%d\n",
			rpc_max_classes);
		return -1;
	}

	return rpc_print_engine_class(class);
}


/*
 *	Pretty print *all* RPC classes.
 */
int rpc_print_all_classes()
{
	rpc_class_t	class;

	for (class = 0; class < rpc_max_classes; class++)
		(void) rpc_print_class(class);

	return rpc_max_classes;
}


/*
 *	Pretty print everything about RPC.
 */
void db_rpc()
{
	(void) rpc_print_all_groups();
	(void) rpc_print_all_handles();
	(void) rpc_print_all_classes();
}

#endif	/* MACH_KDB */


/*
 * initialize the RPC machinery; called  from kern/startup.c
 */

void rpc_machine_init()
{
	int	handles, groups, classes, n;

	handles = getbootint("RPC_HANDLES", 45);
	groups = getbootint("RPC_GROUPS", 5);
	classes = getbootint("RPC_CLASSES", 5);

	if (rpc_init(handles, groups, classes) != RPC_OK)
		panic("rpc_machine_init");

	/*
	 *	temporary grossness...
	 */
	if ((groups >= 1) && ((n = getbootint("RPC_GROUP_0", 0)) != 0))
		(void) rpc_group_alloc(0, n);
	if ((groups >= 2) && ((n = getbootint("RPC_GROUP_1", 0)) != 0))
		(void) rpc_group_alloc(1, n);
	if ((groups >= 3) && ((n = getbootint("RPC_GROUP_2", 0)) != 0))
		(void) rpc_group_alloc(2, n);
	if ((groups >= 4) && ((n = getbootint("RPC_GROUP_3", 0)) != 0))
		(void) rpc_group_alloc(3, n);

}
