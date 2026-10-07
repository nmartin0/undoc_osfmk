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
 *	$Id: rdma.c,v 1.5 1995/01/31 19:42:54 andyp Exp $
 */

/*
 * SSD HISTORY
 * $Log: rdma.c,v $
 * Revision 1.5  1995/01/31  19:42:54  andyp
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
 * Revision 1.4  1994/11/18  20:59:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/09/09  17:53:03  andyp
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
 * Revision 1.2  1994/07/12  21:31:06  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.14  1994/07/06  20:20:59  andyp
 * Forgot a state in case for rdma_undo().
 *
 * Revision 1.1.2.13  1994/07/06  20:16:06  andyp
 * Prototyped (and left disabled) a new RDMA function rdma_undo()
 * that will be used by the fast-path enqueue.
 *
 * Revision 1.1.2.12  1994/05/25  07:37:16  andyp
 * Make configuration of RDMA resources with bootmagic a
 * little more flexible for all users of RDMA.
 *
 * Revision 1.1.2.11  1994/04/01  01:05:56  andyp
 * Protect RDMA handle group free lists from corruption by interrupt-level
 * handle disposal.
 *
 * Revision 1.1.2.10  1994/03/09  23:42:29  rkl
 * Non-blocking handle allocation wasn't fully initializing the handle
 * item data structures.
 *
 * Revision 1.1.2.9  1994/03/02  18:43:50  andyp
 * Added additional user-mode bindings for RDMA testing.
 *
 * Revision 1.1.2.8  1994/02/24  00:26:05  rkl
 *  Resolved problems (and bugs) reported by lint(1).
 *
 * Revision 1.1.2.7  1994/02/18  22:13:10  rkl
 *  Added missing rdma_send_complete() and rdma_recv_complete() calls.
 *
 * Revision 1.1.2.6  1994/02/18  01:54:05  andyp
 * Added rdma_seqid_t's for two new polling routines.
 *
 * Revision 1.1.2.5  1994/02/15  18:27:49  stans
 * added xxx_machine_init()
 *
 * Revision 1.1.2.4  1994/02/07  23:56:28  andyp
 * Fixed an assert that checked for state violations -- its okay to
 * free an RDMA handle when accepting, connecting, flushing, *or*
 * disconnecting.
 *
 * Revision 1.1.2.3  1994/02/07  20:20:29  andyp
 * Rounded out RDMA interfaces and added user-mode bindings for testing.
 *
 * Revision 1.1.2.2  1994/02/04  07:56:33  andyp
 * RPC flow control now works; RDMA is still under construction.
 *
 * Revision 1.1.2.1  1994/01/27  19:12:54  andyp
 * Correct some typos and some cleanup.
 *
 * Revision 1.1  1994/01/26  20:09:15  andyp
 * Initial revision
 *
 *
 * END SSD HISTORY
 */

#include <mach_kdb.h>
#include <mach/boolean.h>
#include <mach/machine/kern_return.h>
#include <mach/machine/vm_types.h>
#include <vm/vm_map.h>
#include <kern/assert.h>
#include <kern/lock.h>
#include <kern/kalloc.h>
#include <kern/thread.h>
#include <ipc/ipc_thread.h>
#include <rpc_rdma/rdma.h>


int	rdma_max_handles, rdma_max_groups;

#define	rdma_range_check(x, min, max)	(((x) >= (min)) && ((x) < (max)))
#define	rdma_valid_handle(h)	rdma_range_check(h, 0, rdma_max_handles)
#define	rdma_valid_group(g)	rdma_range_check(g, 0, rdma_max_groups)

#define RDMA_UNGROUPED	((rdma_group_t) -1)


/*
 *	An internal rdma_handle_item (indexed by an rdma_handle_t)
 */
typedef struct rdma_handle_item {
	rdma_handle_t	rdma_next;		/* free or group "link" */
	rdma_group_t	rdma_group_id;		/* allocation group */
	rdma_state_t	rdma_state;		/* current state */
	rdma_notify_t	rdma_send_callarg;	/* callback argument */
	void		(*rdma_send_callback) ( rdma_handle_t, rdma_notify_t );
	rdma_notify_t	rdma_recv_callarg;	/* callback argument */
	void		(*rdma_recv_callback) ( rdma_handle_t, rdma_notify_t );
} *rdma_handle_item_t;

rdma_handle_t		rdma_item_float;	/* initial free list */
rdma_handle_item_t	rdma_item_vec;		/* vector of RDMA resources */

#define rdma_handle_item_of(handle)	(&rdma_item_vec[(handle)])
#define rdma_handle_for_item(item)	((item) - rdma_item_vec)
#define	rdma_group_of(handle)		(rdma_handle_item_of(handle)->rdma_group_id)

/*
 *	RDMA states
 */
#define	RDMA_HSTATE_FLOATING		0
#define	RDMA_HSTATE_IDLE		1
#define	RDMA_HSTATE_READY		2
#define	RDMA_HSTATE_ACCEPTING		3
#define	RDMA_HSTATE_CONNECTING		4
#define	RDMA_HSTATE_FLUSHING		5
#define	RDMA_HSTATE_UNCONNECTING	6
#define	RDMA_HSTATE_MAX			7


/*
 *	RDMA state query macros
 */
#define	rdma_get_handle_state(h)	(rdma_handle_item_of((h))->rdma_state)
#define	rdma_state_floating(h)		\
		(rdma_get_handle_state(h) == RDMA_HSTATE_FLOATING)
#define	rdma_state_idle(h)		\
		(rdma_get_handle_state(h) == RDMA_HSTATE_IDLE)
#define	rdma_state_ready(h)		\
		(rdma_get_handle_state(h) == RDMA_HSTATE_READY)
#define	rdma_state_accepting(h)		\
		(rdma_get_handle_state(h) == RDMA_HSTATE_ACCEPTING)
#define	rdma_state_connecting(h)	\
		(rdma_get_handle_state(h) == RDMA_HSTATE_CONNECTING)
#define	rdma_state_flushing(h)		\
		(rdma_get_handle_state(h) == RDMA_HSTATE_FLUSHING)
#define	rdma_state_unconnecting(h)	\
		(rdma_get_handle_state(h) == RDMA_HSTATE_UNCONNECTING)


/*
 *	Each RDMA group id is an index into a vector of lists of
 *	free handles.
 */
typedef struct rdma_group_item {
	rdma_handle_t		grp_free_list;		/* free handles */
	int			grp_free_count;		/* no. free handles */
	int			grp_free_wanted;	/* handles wanted */
	int			grp_total;		/* total handles */
	struct ipc_thread_queue	grp_blocked;		/* blocked threads */
	decl_simple_lock_data(,	grp_lock)		/* MP locking */
} *rdma_group_item_t;

rdma_group_item_t	rdma_group_vec;	/* vector of RDMA handle lists */
#define	rdma_group_item_of(group)	(&rdma_group_vec[(group)])


/*
 *	Internal initialization routines
 */
static rdma_return_t	rdma_init_groups( int );
static rdma_return_t	rdma_init_handles( int );


/*
 *	static rdma_return_t rdma_init_groups(ngroups)
 *
 *	PURPOSE
 *
 *	Initialize the RDMA group vector.
 *
 *	RETURNS
 *
 *	RDMA_SUCCESS		if resources could be allocated.
 *	RDMA_SHORTAGE		if too many resources are needed.
 */
static rdma_return_t rdma_init_groups( int ngroups )
{
	vm_size_t		size;
	rdma_group_item_t	g;
	int			i;

	size = ngroups * sizeof(struct rdma_group_item);
	if ((g = (rdma_group_item_t) kalloc(size)) == 0) {
		return RDMA_SHORTAGE;
	}

	rdma_group_vec = g;
	for (i = 0; i < ngroups; i++, g++) {
		g->grp_free_list = RDMA_GROUP_EMPTY;
		g->grp_free_count = 0;
		g->grp_free_wanted = 0;
		g->grp_total = 0;
		ipc_thread_queue_init(&g->grp_blocked);
		simple_lock_init(&g->grp_lock);
	}

	return RDMA_SUCCESS;
}


/*
 *	static void rdma_init_handle_item(h, next)
 *
 *	PURPOSE
 *
 *	Internal routine to initialize an RDMA handle item.
 */
static void rdma_init_handle_item(rdma_handle_item_t h, rdma_handle_t next)
{
	h->rdma_next = next;
	h->rdma_group_id = RDMA_UNGROUPED;
	h->rdma_state = RDMA_HSTATE_FLOATING;
	h->rdma_send_callarg = 0;
	h->rdma_send_callback = 0;
	h->rdma_recv_callarg = 0;
	h->rdma_recv_callback = 0;
}


/*
 *	static rdma_return_t rdma_init_handles(nhandles)
 *
 *	PURPOSE
 *
 *	Initialize the RDMA handles.
 *
 *	RETURNS
 *
 *	RDMA_SUCCESS		if resources could be allocated.
 *	RDMA_SHORTAGE		if too many resources are needed.
 */
static rdma_return_t rdma_init_handles( int nhandles )
{
	rdma_handle_item_t	h;
	int			i;
	vm_size_t		size;
	rdma_handle_t		next;

	size = nhandles * sizeof(struct rdma_handle_item);
	if ((h = (rdma_handle_item_t) kalloc(size)) == 0)
		return RDMA_SHORTAGE;

	rdma_item_vec = h;
	next = 1;
	for (i = 0; i < nhandles - 1; i++, h++, next++)
		rdma_init_handle_item(h, next);
	rdma_init_handle_item(h, RDMA_GROUP_EMPTY);
	rdma_item_float = 0;

	return RDMA_SUCCESS;
}


/*
 *	rdma_return_t rdma_init(nhandles, ngroups)
 *
 *	PURPOSE
 *
 *	Initialize and configure the RDMA facilities to use the supplied
 *	numbers of RDMA handles and groups.
 *
 *	NOTES
 *
 *	Must be called exactly once during system initialization.
 *
 *	RETURNS
 *
 *	RDMA_SUCCESS		if resources could be allocated.
 *	RDMA_SHORTAGE		if too many resources are needed.
 */
rdma_return_t rdma_init( int nhandles, int ngroups )
{
	rdma_return_t	kr;

	if ((kr = rdma_init_groups(ngroups)) != RDMA_SUCCESS)
		return kr;

	if ((kr = rdma_init_handles(nhandles)) != RDMA_SUCCESS)
		return kr;

	if ((kr = rdma_init_engine(nhandles, ngroups)) != RDMA_SUCCESS)
		return kr;

	rdma_max_handles = nhandles;
	rdma_max_groups = ngroups;

	return RDMA_SUCCESS;
}


/*
 *	rdma_return_t rdma_group_alloc(group, nhandles)
 *
 *	PURPOSE
 *
 *	Associate the supplied number of RDMA handles with an RDMA group.
 *
 *	NOTES
 *
 *	Must be called exactly once during system initialization.
 *
 *	RETURNS
 *
 *	RDMA_SUCCESS		if the handles are placed within the group
 *	RDMA_INVALID_GROUP	if the group is invalid
 *	RDMA_HANDLE_SHORTAGE	if not enough handles to place into the group
 */
rdma_return_t rdma_group_alloc( rdma_group_t group, int nhandles )
{
	rdma_handle_t		handle;
	rdma_group_item_t	g;
	rdma_handle_item_t	h;
	int			i;

	if ((group < 0) || (group >= rdma_max_groups))
		return RDMA_INVALID_GROUP;

	g = rdma_group_item_of(group);
	simple_lock(&g->grp_lock);

	for (i = 0; i < nhandles; i++) {

		/*
		 *	if no more handles, quit.
		 */
		if ((handle = rdma_item_float) == RDMA_GROUP_EMPTY) {
			simple_unlock(&g->grp_lock);
			return RDMA_HANDLE_SHORTAGE;
		}

		/*
		 *	remove the handle from the floating list,
		 *	make it a member of the group,
		 *	and mark it <idle>.
		 */
		h = rdma_handle_item_of(handle);
		rdma_item_float = h->rdma_next;

		h->rdma_state = RDMA_HSTATE_IDLE;
		h->rdma_group_id = group;
		h->rdma_next = g->grp_free_list;
		g->grp_free_list = handle;
		g->grp_free_count++;
		g->grp_total++;
	}

	simple_unlock(&g->grp_lock);

	return RDMA_SUCCESS;
}


/*
 *	static void rdma_handle_grab(group, hint, reserve)
 *
 *	PURPOSE
 *
 *	Internal routine to attempt to allocate an RDMA
 *	handle from a group.
 *
 *	RETURNS
 *
 *	an RDMA handle		if a handle has been allocated
 *	RDMA_GROUP_EMPTY	if wait is FALSE and the group is empty.
 */
/*ARGSUSED*/
static rdma_handle_t rdma_handle_grab(
	rdma_group_t	group,
	rdma_node_t	unused,
	boolean_t	reserve )
{
	rdma_group_item_t	g;
	rdma_handle_item_t	h;
	rdma_handle_t		handle;
	thread_t		self;
	int			s;

	g = rdma_group_item_of(group);

	simple_lock(&g->grp_lock);

	s = sploff();
	if ((handle = g->grp_free_list) != RDMA_GROUP_EMPTY) {

		assert(rdma_valid_handle(handle));
		assert(rdma_state_idle(handle));
		assert(rdma_valid_group(rdma_group_of(handle)));
		assert(rdma_group_of(handle) == group);

		h = rdma_handle_item_of(handle);
		g->grp_free_list = h->rdma_next;
		g->grp_free_count--;

		splon(s);

		h->rdma_state = RDMA_HSTATE_READY;
		h->rdma_next = RDMA_GROUP_EMPTY;
		h->rdma_send_callback = 0;
		h->rdma_recv_callback = 0;

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
 *	static rdma_handle_t rdma_handle_wait(group, hint)
 *
 *	PURPOSE
 *
 *	Internal routine to block the calling thread and wait
 *	for a free RDMA handle within a group.
 *
 *	RETURNS
 *
 *	An RDMA handle ready for use.
 */
static rdma_handle_t rdma_handle_wait( rdma_group_t group, rdma_node_t hint )
{
	rdma_handle_t		h;

	while ((h = rdma_handle_grab(group, hint, TRUE)) == RDMA_GROUP_EMPTY) {
		thread_block(0);
	}

	return h;
}


/*
 *	rdma_handle_t rdma_handle_alloc(group, wait, hint)
 *
 *	PURPOSE
 *
 *	Allocate an RDMA handle from an RDMA handle group.
 *
 *	NOTES
 *
 *	If wait is true and the group is empty, the caller will be blocked
 *	until a handle within the group becomes available.
 *
 *	hint specifies the node expected to become the other endpoint.
 *	{ hint is ignored in this implementation }
 *
 *	RETURNS
 *
 *	an RDMA handle		if a handle has been allocated
 *	RDMA_GROUP_EMPTY	if wait is FALSE and the group is empty.
 */
rdma_handle_t rdma_handle_alloc(
	rdma_group_t	group,
	boolean_t	wait,
	rdma_node_t	hint )
{
	if (wait == FALSE)
		return rdma_handle_grab(group, hint, FALSE);

	return rdma_handle_wait(group, hint);
}


/*
 *	void rdma_handle_free(handle)
 *
 *	PURPOSE
 *
 *	Return an RDMA handle to the group of RDMA handles from which
 *	it was allocated.
 *
 *	NOTES
 *
 *	This call should not be made while any RDMA operations associated with
 *	the supplied handle are still in progress.
 *
 */
void rdma_handle_free( rdma_handle_t handle )
{
	rdma_group_item_t	g;
	rdma_handle_item_t	h;
	boolean_t		w;
	int			s;
	thread_t		th;

	assert(rdma_valid_handle(handle));
	assert(!rdma_state_floating(handle));
	assert(!rdma_state_idle(handle));
	assert(!rdma_state_ready(handle));

	assert(rdma_valid_group(rdma_group_of(handle)));

	h = rdma_handle_item_of(handle);
	g = rdma_group_item_of(h->rdma_group_id);

	simple_lock(&g->grp_lock);
	{
		/*
		 *	mark the handle as <idle> and
		 *	prepend it to the group's free list.
		 */
		h->rdma_send_callback = 0;
		h->rdma_send_callarg = 0;
		h->rdma_recv_callback = 0;
		h->rdma_recv_callarg = 0;
		h->rdma_state = RDMA_HSTATE_IDLE;
		s = sploff();
		h->rdma_next = g->grp_free_list;
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
 *	rdma_token_t rdma_token(handle)
 *
 *	PURPOSE
 *
 *	Fabricate a token representing a [node,handle] tuple.
 *
 *	RETURNS
 *
 *	An RDMA token.
 */
rdma_token_t rdma_token( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));

	return rdma_engine_token(handle);
}


/*
 *	rdma_node_t rdma_endpointof(token)
 *
 *	PURPOSE
 *
 *	Crack an RDMA token for the node id within a token.
 *
 *	RETURNS
 *
 *	An RDMA node id.
 */
rdma_node_t rdma_endpointof( rdma_token_t token )
{
	return rdma_engine_crack_token(token);
}


/*
 *	void rdma_accept(handle)
 *
 *	PURPOSE
 *
 *	Place the local handle into <accepting> state.
 *
 */
void rdma_accept( rdma_handle_t handle )
{
	rdma_handle_item_t	h;

	assert(rdma_valid_handle(handle));
	assert(rdma_state_ready(handle));

	h = rdma_handle_item_of(handle);
	h->rdma_state = RDMA_HSTATE_ACCEPTING;
	rdma_engine_accept(handle);

}


/*
 *	void rdma_connect(token, handle)
 *
 *	PURPOSE
 *
 *	Place the local handle into the <connecting> state,
 *	and associate it with the endpoint named by the token.
 *
 */
void rdma_connect( rdma_token_t token, rdma_handle_t handle )
{
	rdma_handle_item_t	h;

	assert(rdma_valid_handle(handle));
	assert(rdma_state_ready(handle));

	h = rdma_handle_item_of(handle);
	h->rdma_state = RDMA_HSTATE_CONNECTING;
	rdma_engine_connect(token, handle);
}


/*
 *	void rdma_disconnect(handle)
 *
 *	PURPOSE
 *
 *	Disconnect the local endpoint.
 */
void rdma_disconnect( rdma_handle_t handle )
{
	rdma_handle_item_t	h;

	assert(rdma_valid_handle(handle));
	assert(!rdma_state_floating(handle));
	assert(!rdma_state_idle(handle));
	assert(!rdma_state_unconnecting(handle));

	h = rdma_handle_item_of(handle);
	h->rdma_state = RDMA_HSTATE_UNCONNECTING;
	rdma_engine_disconnect(handle);
}


/*
 *	rdma_return_t rdma_flush(handle)
 *
 *	PURPOSE
 *
 *	Conditonally abort posted transfers.  If no connection has yet
 *	to be made, flush the local engine.
 *
 *	RETURNS
 *
 *	RDMA_SUCCESS		if the flush was successful.
 *	RDMA_CONNECTED		if a connection has already been established.
 */
rdma_return_t rdma_flush( rdma_handle_t handle)
{
	rdma_handle_item_t	h;

	assert(rdma_valid_handle(handle));
	assert(!rdma_state_floating(handle));
	assert(!rdma_state_idle(handle));
	assert(!rdma_state_ready(handle));
	assert(!rdma_state_flushing(handle));
	assert(!rdma_state_unconnecting(handle));

	if (rdma_engine_flush(handle) == 0) {
		h = rdma_handle_item_of(handle);
		h->rdma_state = RDMA_HSTATE_FLUSHING;
		return RDMA_SUCCESS;
	}

	return RDMA_CONNECTED;
}


/*
 *	void rdma_flush_endpoint(token)
 *
 *	PURPOSE
 *
 *	Remotely flush the RDMA endpoint named by the token.
 */
void rdma_flush_endpoint(rdma_token_t token)
{
	rdma_engine_flush_endpoint(token);
}


/*
 *	void rdma_undo(handle)
 *
 *	PURPOSE
 *
 *	A special-use routine that:
 *
 *	- silently flushes any posted operations (ie, no
 *	  callbacks will be generated).
 *
 *	- disconnects the RDMA handle if connected.
 *
 *	- frees the RDMA handle if allocated.
 *
 *	WARNING:
 *
 *	The side-effects of calling this routine are undefined
 *	if a connection from this handle to another endpoint has
 *	already been established.
 */
void rdma_undo(rdma_handle_t handle)
{
	assert(rdma_valid_handle(handle));

	switch (rdma_get_handle_state(handle)) {
	case RDMA_HSTATE_FLOATING:
	case RDMA_HSTATE_IDLE:
		/*
		 *	ungrouped handle or unallocated handle.
		 *	nothing to do.  (although someone might
		 *	be interested in knowing about this case).
		 */
		/*assert(0);*/
		return;

	case RDMA_HSTATE_READY:
		/*
		 *	allocated, but nothing posted.
		 *	just free it and return.
		 */
		rdma_handle_free(handle);
		return;

	case RDMA_HSTATE_CONNECTING:
	case RDMA_HSTATE_FLUSHING:
	case RDMA_HSTATE_UNCONNECTING:
		/*
		 *	these are strange states to be undoing...
		 *	someone might be interested it knowing
		 *	about it.
		 */
		assert(0);
		/* fall through... */
	case RDMA_HSTATE_ACCEPTING:
		/*
		 *	might have some operations posted.
		 *	dump them, and disconnect.
		 */
		rdma_engine_undo(handle);
		rdma_handle_free(handle);
		return;
	}

	assert(0);
}


/*
 *	void rdma_set_send_callback(handle, callback, callarg)
 *
 *	PURPOSE
 *
 *	Install a callback function to be called when a tagged
 *	send operation is processed by the engine.
 */
void rdma_set_send_callback(
	rdma_handle_t	handle,
	void		(*callback)( rdma_handle_t, rdma_notify_t ),
	rdma_notify_t	callarg)
{
	rdma_handle_item_t	h;

	assert(rdma_valid_handle(handle));
	assert(!rdma_state_idle(handle));

	h = rdma_handle_item_of(handle);
	h->rdma_send_callback = callback;
	h->rdma_send_callarg = callarg;
}


/*
 *	void rdma_set_recv_callback(handle, callback, callarg)
 *
 *	PURPOSE
 *
 *	Install a callback function to be called when a tagged
 *	receive operation is processed by the engine.
 */
void rdma_set_recv_callback(
	rdma_handle_t	handle,
	void		(*callback)( rdma_handle_t, rdma_notify_t ),
	rdma_notify_t	callarg)
{
	rdma_handle_item_t	h;

	assert(rdma_valid_handle(handle));
	assert(!rdma_state_idle(handle));

	h = rdma_handle_item_of(handle);
	h->rdma_recv_callback = callback;
	h->rdma_recv_callarg = callarg;
}


/*
 *	rdma_seqid_t rdma_send(handle, buf, count, notify, map)
 *
 *	PURPOSE
 *
 *	Schedule "count" bytes from "buf" to be sent on "handle."
 *
 *	If "notify" is TRUE, the send callback installed at the time the
 *	operation completes will be invoked.
 *
 *	"map" will be used to silently resolve any page-not-present faults
 *	that may occur.
 */
rdma_seqid_t rdma_send(
	rdma_handle_t	handle,
	vm_offset_t	buf,
	vm_size_t	count,
	boolean_t	notify,
	vm_map_t	map)
{
	assert(rdma_valid_handle(handle));
	assert(!rdma_state_floating(handle));
	assert(!rdma_state_idle(handle));
	assert(!rdma_state_ready(handle));
	assert(!rdma_state_flushing(handle));
	assert(!rdma_state_unconnecting(handle));

	return rdma_engine_send(handle, buf, count, notify, map);

}


/*
 *	rdma_seqid_t rdma_recv(handle, buf, count, notify, map)
 *
 *	PURPOSE
 *
 *	Schedule the reception of "count" bytes into "buf" on "handle."
 *
 *	If "notify" is TRUE, the receive callback installed at the time the
 *	operation completes will be invoked.
 *
 *	"map" will be used to silently resolve any page-not-present faults
 *	that may occur.
 */
rdma_seqid_t rdma_recv(
	rdma_handle_t	handle,
	vm_offset_t	buf,
	vm_size_t	count,
	boolean_t	notify,
	vm_map_t	map)
{
	assert(rdma_valid_handle(handle));
	assert(!rdma_state_floating(handle));
	assert(!rdma_state_idle(handle));
	assert(!rdma_state_ready(handle));
	assert(!rdma_state_flushing(handle));
	assert(!rdma_state_unconnecting(handle));

	return rdma_engine_recv(handle, buf, count, notify, map);

}


boolean_t rdma_send_busy( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_engine_send_busy(handle);
}


boolean_t rdma_send_ready( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_engine_send_ready(handle);
}


boolean_t rdma_send_done( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_engine_send_done(handle);
}


rdma_seqid_t rdma_send_complete( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_engine_send_complete(handle);
}


boolean_t rdma_recv_busy( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_engine_recv_busy(handle);
}


boolean_t rdma_recv_ready( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_engine_recv_ready(handle);
}


boolean_t rdma_recv_done( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_engine_recv_done(handle);
}


rdma_seqid_t rdma_recv_complete( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_engine_recv_complete(handle);
}


void rdma_send_intr( rdma_handle_t handle )
{
	rdma_handle_item_t	h;
	rdma_notify_t		a;
	void			(*f)( rdma_handle_t, rdma_notify_t );

	assert(rdma_valid_handle(handle));

	h = rdma_handle_item_of(handle);
	f = h->rdma_send_callback;
	a = h->rdma_send_callarg;
	assert(f != 0);

	(*f)(handle, a);
}


void rdma_recv_intr( rdma_handle_t handle )
{
	rdma_handle_item_t	h;
	rdma_notify_t		a;
	void			(*f)( rdma_handle_t, rdma_notify_t );

	assert(rdma_valid_handle(handle));

	h = rdma_handle_item_of(handle);
	f = h->rdma_recv_callback;
	a = h->rdma_recv_callarg;
	assert(f != 0);

	(*f)(handle, a);
}


#define	RDMA_TEST	1
#if	RDMA_TEST
/*
 *	syscall bindings for user-mode testing.
 */
int syscall_rdma_alloc( rdma_group_t group, boolean_t wait, rdma_node_t hint )
{
	assert(rdma_valid_group(group));
	return (int) rdma_handle_alloc(group, wait, hint);
}


int syscall_rdma_free( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	rdma_handle_free(handle);
	return 0;
}


int syscall_rdma_token( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return (int) rdma_token(handle);
}


int syscall_rdma_accept( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	rdma_accept(handle);
	return 0;
}


int syscall_rdma_connect( rdma_token_t token, rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	rdma_connect(token, handle);
	return 0;
}


rdma_seqid_t syscall_rdma_send(rdma_handle_t handle, vm_offset_t buf, vm_size_t cnt)
{
	assert(rdma_valid_handle(handle));
	return (rdma_send(handle, buf, cnt, 0, current_map()));
}


rdma_seqid_t syscall_rdma_recv(rdma_handle_t handle, vm_offset_t buf, vm_size_t cnt)
{
	assert(rdma_valid_handle(handle));
	return (rdma_recv(handle, buf, cnt, 0, current_map()));
}


int syscall_rdma_disconnect( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	rdma_disconnect(handle);
	return 0;
}


int syscall_rdma_send_busy( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_send_busy(handle);
}


int syscall_rdma_send_ready( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_send_ready(handle);
}


int syscall_rdma_send_done( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_send_done(handle);
}


int syscall_rdma_send_complete( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_send_complete(handle);
}


int syscall_rdma_recv_busy( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_recv_busy(handle);
}


int syscall_rdma_recv_ready( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_recv_ready(handle);
}


int syscall_rdma_recv_done( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_recv_done(handle);
}


int syscall_rdma_recv_complete( rdma_handle_t handle )
{
	assert(rdma_valid_handle(handle));
	return rdma_recv_complete(handle);
}
#endif	/* RDMA_TEST */



#if	MACH_KDB

/*
 *	Debugging support.
 */

static char *rdma_handle_state_ascii[] = {
	"<ungrouped>",
	"<idle>",
	"<ready>",
	"<accepting>",
	"<connecting>",
	"<flushing>",
	"<unconnecting>"
};


/*
 *	Pretty print an "rdma_handle_item", then ask the
 *	RDMA engine to print out what it knows about
 *	the supplied handle.
 */
int rdma_print_handle( rdma_handle_t handle )
{
	rdma_handle_item_t	h;
	char			*s;
	extern int		indent;
	extern int		rdma_print_engine( int );

	if (!rdma_valid_handle(handle)) {
		db_printf("rdma_print_handle: rdma_max_handles=%d\n",
			rdma_max_handles);
		return -1;
	}

	h = rdma_handle_item_of(handle);

	iprintf("rdma handle=%d (0x%x), group=%d, state=%d ",
		handle, h,
		h->rdma_group_id,
		h->rdma_state);

	if ((h->rdma_state < 0) || (h->rdma_state >= RDMA_HSTATE_MAX))
		s = "<huh?>";
	else
		s = rdma_handle_state_ascii[h->rdma_state];

	iprintf("%s\n", s);

	indent += 2;
	iprintf("next=%d (0x%x)\n", h->rdma_next);
	iprintf("send_callback=0x%x, send_callarg=0x%x\n",
		h->rdma_send_callback,
		h->rdma_send_callarg);
	iprintf("recv_callback=0x%x, recv_callarg=0x%x\n",
		h->rdma_recv_callback,
		h->rdma_recv_callarg);

	(void) rdma_print_engine((int) handle);
	indent -= 2;

	return handle;
}


/*
 *	Pretty print *all* RDMA handles.
 */
int rdma_print_all_handles()
{
	rdma_handle_t	handle;

	for (handle = 0; handle < rdma_max_handles; handle++)
		(void) rdma_print_handle(handle);

	return rdma_max_handles;
}


/*
 *	Pretty print an RDMA handle group.
 */
int rdma_print_group( rdma_group_t group )
{
	rdma_group_item_t	g;
	rdma_handle_item_t	h;
	rdma_handle_t		handle;
	thread_t		th, first;
	task_t			task;
	int			i, task_id, thread_id;
	char			*s;
	int			allocated, free;
	extern int		indent;

	if (!rdma_valid_group(group)) {
		db_printf("rdma_print_group: rdma_max_groups=%d\n",
			rdma_max_groups);
		return -1;
	}

	g = rdma_group_item_of(group);

	iprintf("rdma group=%d (0x%x), total=%d, free=%d, wanted=%d\n",
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

	iprintf("allocated:\n");
	indent += 2;
	allocated = 0;
	for (handle = 0; handle < rdma_max_handles; handle++) {
		h = rdma_handle_item_of(handle);
		if (h->rdma_group_id != group)
			continue;
		if (h->rdma_state == RDMA_HSTATE_IDLE)
			continue;
		if ((h->rdma_state < 0) || (h->rdma_state > RDMA_HSTATE_MAX))
			s = "<huh?>";
		else
			s = rdma_handle_state_ascii[h->rdma_state];
		iprintf("handle=%3d state=%s\n", handle, s);
		allocated++;
	}
	iprintf("allocated=%d (0x%x)\n", allocated, allocated);
	indent -= 2;

	iprintf("free:\n");
	indent += 2;
	handle = g->grp_free_list;
	free = 0;
	while (handle != RDMA_GROUP_EMPTY) {
		h = rdma_handle_item_of(handle);
		iprintf("handle=%d (0x%x), next=%d\n", handle, h, h->rdma_next);
		handle = h->rdma_next;
		free++;
	}
	iprintf("free=%d (0x%x)\n", free, free);
	indent -= 2;

	indent -= 2;
	return group;
}


/*
 *	Pretty print *all* RDMA groups.
 */
int rdma_print_all_groups()
{
	rdma_group_t	group;

	for (group = 0; group < rdma_max_groups; group++)
		(void) rdma_print_group(group);

	return rdma_max_groups;
}


/*
 *	Pretty print everything about RDMA.
 */
void db_rdma()
{
	(void) rdma_print_all_groups();
	(void) rdma_print_all_handles();
}

#endif	/* MACH_KDB */

/*
 * initialize the RDMA machinery; called from kern/startup.c
 */

void rdma_machine_init()
{
	int	handles, groups;

	handles = 0;
	groups = 0;

	/*
	 *	RDMA users must add in resource requirements here.
	 */
#if	NORMA2
	handles += dipc_define_rdma_handles();
	groups += dipc_define_rdma_groups();
#endif	/* NORMA2 */

	if ((handles > 0) && (groups > 0)) {
		if (rdma_init(handles, groups) != RDMA_SUCCESS)
			panic("rdma_machine_init");
	}
}
