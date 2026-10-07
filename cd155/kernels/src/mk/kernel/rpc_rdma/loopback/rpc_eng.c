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
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 */

/*
 * HISTORY
 * $Log: rpc_eng.c,v $
 * Revision 1.3  1994/11/18  20:59:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:31:24  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.2  1994/03/10  07:25:28  rkl
 *  Added rpc_engine_set_notify() call.
 *
 * Revision 1.1.2.1  1994/02/23  23:05:19  rkl
 *  Fixed stuff reported by lint.
 *
 * Revision 1.1  1994/02/16  18:10:23  rkl
 * Initial revision
 *
 *
 *
 */

#include <mach_kdb.h>
#include <mach_assert.h>
#include <mach/boolean.h>
#include <mach/machine/kern_return.h>
#include <mach/machine/vm_types.h>
#include <kern/assert.h>
#include <kern/kalloc.h>
#include <rpc_rdma/rpc.h>

/*
 *	Each RPC engine has some status bits and a pointer (set during
 *	intialization) to the corresponding payload bay.
 */
typedef struct rpc_lb_engine {
	struct rpc_lb_engine		*rpc_prev;
	struct rpc_lb_engine		*rpc_next;
	struct rpc_lb_engine		*rpc_endpoint;
	vm_offset_t			 rpc_payload;
	vm_size_t			 rpc_payload_size;
	int				 rpc_class;
	int				 rpc_state;
	int				 rpc_notify;
	int				 rpc_index;
} rpc_lb_engine_t;

/*
 *  Loopback states
 */
#define	RPC_LOOPBACK_ENG_IDLE			0
#define	RPC_LOOPBACK_ENG_WAIT_FOR_REPLY		1
#define	RPC_LOOPBACK_ENG_REPLY			2
#define	RPC_LOOPBACK_ENG_WAIT_FOR_RECEIVER	4
#define	RPC_LOOPBACK_ENG_WAIT_FOR_SENDER	5

/*
 *  Macros
 */
#define	handle_is_valid(h)	((h >= 0) && (h < rpc_lb_nengines))
#define	class_is_valid(c)	((c >= 0) && (c < rpc_lb_nclasses))

#define	rpc_lb_class_alloc(n) \
	((rpc_lb_engine_t**)kalloc(n * sizeof(rpc_lb_engine_t*)));

#define	rpc_lb_engine_alloc(n) \
	((rpc_lb_engine_t*)kalloc(n * sizeof(rpc_lb_engine_t)));

#if	MACH_ASSERT
#define	TRACE	if (rpc_loopback_trace) printf
#else
#define	TRACE
#endif

/*
 *  RPC loopback engine globals
 */
rpc_lb_engine_t		 *rpc_lb_engine;
rpc_lb_engine_t		**rpc_lb_class;

int			rpc_lb_nengines;
int			rpc_lb_nclasses;

#if	 ! DYNAMIC_RPC_LOOPBACK_RPC_ALLOCATION
int			rpc_engine_payload_size = 64;
#endif

/*
 *  Debug
 */
int	rpc_loopback_trace = 0;

/*
 *	RPC engine initialization
 */
/*ARGSUSED*/
rpc_return_t
rpc_init_engine( nhandles, ngroups, nclasses )
	int	nhandles;
	int	ngroups;
	int	nclasses;
{
	rpc_lb_engine_t  *eng;
	rpc_lb_engine_t **class;
	int		   i;

	TRACE("rpc_init_engine(nhandles=%d ngroups=%d nclasses=%d)\n",
						nhandles, ngroups, nclasses);
	/*
	 *  Allocate and initalize class vector.
	 */
	rpc_lb_nclasses = nclasses;
	rpc_lb_class = rpc_lb_class_alloc(nclasses);
	if (rpc_lb_class == 0)
		return (RPC_SHORTAGE);

	for (class = rpc_lb_class; nclasses--; class++)
		*class = (rpc_lb_engine_t*) 0;

	/*
	 *  Allocate the RPC engines
	 */
	rpc_lb_nengines  = nhandles;
	rpc_lb_engine = rpc_lb_engine_alloc(nhandles);
	if (rpc_lb_engine == 0)
		return (RPC_SHORTAGE);

	/*
	 *  Allocate space for all RPC payload bays and initialize each
	 *  RPC engine state.
	 */
	for (eng = rpc_lb_engine, i = 0; i < nhandles; i++, eng++) {
#if	DYNAMIC_RPC_LOOPBACK_RPC_ALLOCATION
		eng->rpc_payload = 0;
		eng->rpc_payload_size = 0;
#else
		if ((eng->rpc_payload = kalloc(rpc_engine_payload_size)) == 0)
			return (RPC_SHORTAGE);
		eng->rpc_payload_size = rpc_engine_payload_size;
#endif
		eng->rpc_state = RPC_LOOPBACK_ENG_IDLE;
		eng->rpc_prev = 0;
		eng->rpc_next = 0;
		eng->rpc_class = -1;
		eng->rpc_index = i;
	}

	return (RPC_OK);
}

/*
 *	RPC engine payload slots
 */
void*
rpc_engine_alloc_payload( handle, argsize )
	rpc_handle_t	handle;
	int		argsize;
{
	rpc_lb_engine_t  *eng;

	TRACE("rpc_engine_alloc_payload(handle=%d argsize=%d)\n",
							handle, argsize);

	assert(handle_is_valid(handle));

	eng = &rpc_lb_engine[ handle ];

#if	DYNAMIC_RPC_LOOPBACK_RPC_ALLOCATION
	assert(eng->rpc_payload == 0);
	eng->rpc_payload_size = argsize;
	eng->rpc_payload = kalloc(argsize);
	return((void*)eng->rpc_payload);
#else
	if (argsize > rpc_engine_payload_size)
		return ((void*) 0);
	else
		return ((void*) eng->rpc_payload);
#endif
}

void*
rpc_engine_free_payload( handle )
	rpc_handle_t	handle;
{
	TRACE("rpc_engine_free_payload(handle=%d)\n", handle);

#if	DYNAMIC_RPC_LOOPBACK_RPC_ALLOCATION
	rpc_lb_engine_t  *eng;

	assert(handle_is_valid(xhandle));
	eng = &rpc_lb_engine[ xhandle ];

	assert(eng->rpc_payload);
	kfree(eng->rpc_payload, eng->rpc_payload_size);
#endif
	return ((void*) 0);
}

rpc_lb_engine_t*
rpc_loopback_match(class, state)
	int	class;
	int	state;
{
	rpc_lb_engine_t  *eng;

	TRACE("rpc_loopback_match(class=%d state=%d)\n", class, state);

	assert(class_is_valid(class));

	for (eng = rpc_lb_class[ class ]; eng; eng = eng->rpc_next) {
		if (eng->rpc_state == state) {
			if (eng->rpc_prev)
				eng->rpc_prev->rpc_next = eng->rpc_next;
			if (eng->rpc_next)
				eng->rpc_next->rpc_prev = eng->rpc_prev;
			if (eng == rpc_lb_class[ class ])
				rpc_lb_class[ class ] = eng->rpc_next;
			return (eng);
		}
	}
	return ((rpc_lb_engine_t*)0);
}

void
rpc_loopback_enqueue_on_class(eng, class)
	rpc_lb_engine_t	*eng;
	int		class;
{
	TRACE("rpc_loopback_enqueue_on_class(eng=%x class=%d)\n", eng, class);

	assert(class_is_valid(class));

	eng->rpc_prev = 0;
	eng->rpc_next = rpc_lb_class[ class ];
	rpc_lb_class[ class ] = eng;
}

void
rpc_loopback_deliver_request(s, r)
	rpc_lb_engine_t  *s;
	rpc_lb_engine_t  *r;
{
	TRACE("rpc_loopback_deliver_request(s=%x r=%x)\n", s, r);

	/*
	 *  Deliver the contents of the payload.
	 */
	bcopy(s->rpc_payload, r->rpc_payload, r->rpc_payload_size);

	/*
	 *  Hook'um up
	 */
	r->rpc_endpoint = s;
	s->rpc_endpoint = r;

	/*
	 *  Set state
	 */
	r->rpc_state = RPC_LOOPBACK_ENG_REPLY;
	s->rpc_state = RPC_LOOPBACK_ENG_WAIT_FOR_REPLY;

	/*
	 *  Make receiver notification is requested.
	 */
	if (r->rpc_notify) {
		r->rpc_notify = FALSE;
		rpc_request_arrival(r->rpc_index);
	}
}

/*
 *	RPC engine operations
 */
void
rpc_engine_send_request( node, class, handle, notify )
	rpc_node_t	node;
	rpc_class_t	class;
	rpc_handle_t	handle;
	boolean_t	notify;
{
	rpc_lb_engine_t  *s;
	rpc_lb_engine_t  *r;

	TRACE("rpc_engine_send_request(node=%d class=%d handle=%d notify=%d)\n",
						node, class, handle, notify);

	assert(handle_is_valid(handle));
	assert(class_is_valid(class));

	s = &rpc_lb_engine[ handle ];

	assert(s->rpc_state == RPC_LOOPBACK_ENG_IDLE);
	assert(s->rpc_payload);

	/*
	 *  Set state
	 */
	s->rpc_notify = notify;
	s->rpc_state  = RPC_LOOPBACK_ENG_WAIT_FOR_RECEIVER;

	/*
	 *  See if we have a waiting receiver on this class.
	 */
	if(r = rpc_loopback_match(class, RPC_LOOPBACK_ENG_WAIT_FOR_SENDER))
		rpc_loopback_deliver_request(s, r);
	else
		rpc_loopback_enqueue_on_class(s, class);
}

void
rpc_engine_recv_request( class, handle, notify )
	rpc_class_t	class;
	rpc_handle_t	handle;
	boolean_t	notify;
{
	rpc_lb_engine_t  *r;
	rpc_lb_engine_t  *s;

	TRACE("rpc_engine_recv_request(class=%d handle=%d notify=%d)\n",
							class, handle, notify);

	assert(handle_is_valid(handle));
	assert(class_is_valid(class));

	r = &rpc_lb_engine[ handle ];

	assert(r->rpc_state == RPC_LOOPBACK_ENG_IDLE);
	assert(r->rpc_payload);

	/*
	 *  Set state
	 */
	r->rpc_notify = notify;
	r->rpc_state  = RPC_LOOPBACK_ENG_WAIT_FOR_SENDER;
	
	/*
	 *  See if we have a waiting sender on this class.
	 */
	if(s = rpc_loopback_match(class, RPC_LOOPBACK_ENG_WAIT_FOR_RECEIVER))
		rpc_loopback_deliver_request(s, r); 
	else
		rpc_loopback_enqueue_on_class(r, class);
}

void
rpc_engine_send_reply( handle, notify )
	rpc_handle_t	handle;
	boolean_t	notify;
{
	rpc_lb_engine_t  *r;
	rpc_lb_engine_t  *s;

	TRACE("rpc_engine_send_reply(handle=%d notify=%d)\n", handle, notify);

	assert(handle_is_valid(handle));

	r = &rpc_lb_engine[ handle ];

	assert(r->rpc_endpoint);

	s = r->rpc_endpoint;

	assert(r->rpc_state == RPC_LOOPBACK_ENG_REPLY);
	assert(s->rpc_state == RPC_LOOPBACK_ENG_WAIT_FOR_REPLY);

	/*
	 *  Deliver the reply
	 */
	bcopy(r->rpc_payload, s->rpc_payload, s->rpc_payload_size);

	/*
	 *  Change state
	 */
	r->rpc_state = RPC_LOOPBACK_ENG_IDLE;
	s->rpc_state = RPC_LOOPBACK_ENG_IDLE;

	/*
	 *  Give notification is requested.
	 */
	if (r->rpc_notify) {
		r->rpc_notify = FALSE;
		rpc_reply_depart(r->rpc_index);
	}

	if (s->rpc_notify) {
		s->rpc_notify = FALSE;
		rpc_reply_arrival(s->rpc_index);
	}
}

/*
 *	Ask for notification after the send/receive has already been made.
 */
boolean_t
rpc_engine_set_notify( handle )
	rpc_handle_t	handle;
{
	rpc_lb_engine_t  *eng;

	TRACE("rpc_engine_set_notify(handle=%d)\n", handle);

	eng = &rpc_lb_engine[ handle ];

	assert(handle_is_valid(handle));

	if (eng->rpc_state == RPC_LOOPBACK_ENG_IDLE) {
		return (TRUE);
	} else {
		eng->rpc_notify = TRUE;
		return (FALSE);
	}
}

/*
 *	RPC engine polling interface
 */
boolean_t
rpc_engine_status( handle )
	rpc_handle_t	handle;
{
	rpc_lb_engine_t  *eng;

	TRACE("rpc_engine_status(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	eng = &rpc_lb_engine[ handle ];

	switch (eng->rpc_state) {
		case RPC_LOOPBACK_ENG_IDLE:
		case RPC_LOOPBACK_ENG_REPLY:
			return (RPC_ENGINE_IDLE);
		default:
			return (~RPC_ENGINE_IDLE);
	}
}

#if	MACH_KDB

/*
 *	Pretty print an RPC engine.
 */
int rpc_print_engine( handle )
	rpc_handle_t	handle;
{
	rpc_lb_engine_t	*r;
	extern int	indent;

	if ( ! handle_is_valid(handle) ) {
		iprintf("warning: using ((rpc_lb_engine_t *) 0x%x)\n", handle);
		r = (rpc_lb_engine_t *) handle;
		handle = r - rpc_lb_engine;
	} else {
		r = &rpc_lb_engine[ handle ];
	}

	iprintf("rpc engine=%d (0x%x) {\n", handle, r);

	indent += 2;
	iprintf("prev=0x%x, next=0x%x,\n",
		r->rpc_prev,
		r->rpc_next);
	iprintf("payload=0x%x, payload size=0x%x, state=%d\n",
		r->rpc_payload,
		r->rpc_payload_size,
		r->rpc_state);
	indent -= 2;

	iprintf("}\n");

	return ( (int) handle );
}

int
rpc_print_engine_class( class )
	int	class;
{
	extern int		indent;

	indent += 2;
	iprintf("rpc class printing not supported\n");
	indent -= 2;
	
	return (class);
}

#endif	/* MACH_KDB */
