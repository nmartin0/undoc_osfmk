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
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

/*
 * HISTORY:
 * $Log: rdma_eng.c,v $
 * Revision 1.3  1994/11/18  20:59:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:31:20  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.2  1994/02/23  23:05:10  rkl
 *  Fixed stuff reported by lint.
 *
 * Revision 1.1.2.1  1994/02/18  22:13:47  rkl
 *  Added support for rdma_seqid_t's
 *
 * Revision 1.1  1994/02/16  18:10:16  rkl
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
#include <vm/vm_map.h>
#include <kern/assert.h>
#include <kern/kalloc.h>
#include <rpc_rdma/rdma.h>
#include <rpc_rdma/loopback/rdma.h>

/*
 *	RDMA engine type definitions
 */
typedef struct rdma_lb_engine_req {
	vm_map_t	map;		/* used only when faulting */
	vm_offset_t	buf;		/* buffer pointer */
	vm_size_t	count;		/* byte count remaining */
	unsigned int	notify : 1,	/* flag for notification */
	                active : 1;	/* indicates buffer is `armed' */
} rdma_lb_engine_req_t;

typedef struct rdma_lb_engine {
	int			rdma_state;
	int			rdma_index;

	struct rdma_lb_engine	*rdma_endpoint;

	unsigned short		rdma_send_head;
	unsigned short		rdma_send_tail;
	rdma_seqid_t		rdma_send_in;
	rdma_seqid_t		rdma_send_out;
	rdma_lb_engine_req_t	rdma_send[RDMA_MAXREQ];

	unsigned short		rdma_recv_head;
	unsigned short		rdma_recv_tail;
	rdma_seqid_t		rdma_recv_in;
	rdma_seqid_t		rdma_recv_out;
	rdma_lb_engine_req_t	rdma_recv[RDMA_MAXREQ];

} rdma_lb_engine_t;

#define	RDMA_LOOPBACK_ENG_IDLE		0
#define	RDMA_LOOPBACK_ENG_ACCEPTING	1
#define	RDMA_LOOPBACK_ENG_CONNECTED	2
#define	RDMA_LOOPBACK_ENG_FLUSHING	3

/*
 *  Macros
 */
#define	handle_is_valid(h)	((h >= 0) && (h < rdma_lb_handles))

#define	engine_is_idle(h)	\
  ((! rdma_lb_engine[h].rdma_send[rdma_lb_engine[h].rdma_send_head].active) && \
   (! rdma_lb_engine[h].rdma_recv[rdma_lb_engine[h].rdma_recv_head].active))

#define	get_engine(h) 		(&rdma_lb_engine[h])

#define	engine_is_valid(e)	\
	((e >= rdma_lb_engine) && (e < (rdma_lb_engine + rdma_lb_handles)))

#define	is_send_queue_empty(e)	 (e->rdma_send_head == e->rdma_send_tail)
#define	is_recv_queue_empty(e)	 (e->rdma_recv_head == e->rdma_recv_tail)
	
#if	MACH_ASSERT
#define	TRACE	if (rdma_loopback_trace) printf
#else
#define	TRACE
#endif

/*
 *	RDMA engine globals
 */
rdma_lb_engine_t	*rdma_lb_engine;
int			rdma_lb_handles;

/*
 *  Debug
 */
int	rdma_loopback_trace = 0;

/*
 *	RDMA engine interfaces
 */
/*ARGSUSED*/
rdma_return_t
rdma_init_engine( nhandles, ngroups )
	int	nhandles;
	int	ngroups;
{
	rdma_lb_engine_t *eng;
	int		  i;

	TRACE("rdma_init_engine(nhandles=%d ngroups=%d)\n", nhandles, ngroups);

	/*
	 *  Allocate RDMA engines
	 */
	rdma_lb_handles = nhandles;
	rdma_lb_engine = (rdma_lb_engine_t*)
				kalloc(nhandles * sizeof(rdma_lb_engine_t));
	if (rdma_lb_engine == 0)
		return (RDMA_SHORTAGE);
	/*
	 *  Initialize each RDMA engine.
	 */
	for (eng = rdma_lb_engine, i = 0; i < nhandles; i++, eng++) {
		rdma_lb_engine_req_t	*req;
		int			x;

		eng->rdma_state     = RDMA_LOOPBACK_ENG_IDLE;
		eng->rdma_endpoint  = 0;
		eng->rdma_send_head = 0;
		eng->rdma_send_tail = 0;
		eng->rdma_recv_head = 0;
		eng->rdma_recv_tail = 0;
		eng->rdma_index	    = i;
		eng->rdma_send_in   = 0;
		eng->rdma_send_in   = 0;
		eng->rdma_recv_out  = 0;
		eng->rdma_recv_out  = 0;

		for (req = eng->rdma_send, x = 0; x < RDMA_MAXREQ; x++, req++) {
			req->map    = (vm_map_t) 0;
			req->buf    = 0;
			req->count  = 0;
			req->notify = 0;
			req->active = 0;
		}
		for (req = eng->rdma_recv, x = 0; x < RDMA_MAXREQ; x++, req++) {
			req->map    = (vm_map_t) 0;
			req->buf    = 0;
			req->count  = 0;
			req->notify = 0;
			req->active = 0;
		}
	}

	return (RDMA_SUCCESS);
}

rdma_token_t
rdma_engine_token( handle )
	rdma_handle_t	handle;
{
	TRACE("rdma_engine_token(handle=%d)\n", handle);
	assert(handle_is_valid(handle));
	return (handle);
}

rdma_node_t
rdma_engine_crack_token( handle )
	rdma_handle_t	handle;
{
	TRACE("rdma_engine_crack_token(handle=%d)\n", handle);
	assert(handle_is_valid(handle));
	return (0);
}


vm_size_t
rdma_lb_xfer(s_handle, s_req,  r_handle, r_req, count)
	rdma_handle_t		s_handle;
	rdma_lb_engine_req_t	*s_req;
	rdma_handle_t		r_handle;
	rdma_lb_engine_req_t	*r_req;
	vm_size_t		count;
{
	TRACE("rdma_lb_xfer(s_handle=%d s_req=%x r_handle=%d r_req=%x count=%d)\n",
				s_handle, s_req, r_handle, r_req, count);

#if	PARAGON860
	while (count) {
		extern	vm_offset_t	mcmsg_validate1();

		vm_size_t	s_best;
		vm_size_t	r_best;
		vm_offset_t	s_paddr;
		vm_offset_t	r_paddr;

		/*
	 	 *  Make sure send and receive pages are present.
		 */
		if ((r_paddr = mcmsg_validate1(r_req->buf, 1,
					       r_req->map->pmap->dirbase))==0) {
			TRACE("	receiving page %x not present\n", r_req->buf);
			rdma_recv_fault_intr(r_handle);
			return (count);
		}

		if ((s_paddr = mcmsg_validate1(s_req->buf, 1,
					       s_req->map->pmap->dirbase))==0) {
			TRACE("	sending page %x not present\n", r_req->buf);
			rdma_send_fault_intr(s_handle);
			return (count);
		}

		/*
		 *  Figure how much we can move without crossing a page
		 *  boundary in either buffer.
		 */
		r_best = PAGE_SIZE - (r_req->buf & page_mask);
		s_best = PAGE_SIZE - (s_req->buf & page_mask);

		if (r_best > s_best)
			r_best = s_best;
		if (r_best > count)
			r_best = count;
		
		TRACE("	xfering %d bytes\n", r_best);

		/*
		 *  Make the transfer and update pointers/counters.
		 *  N.B.  For Paragon, physical memory is always in
		 *        the kernel map.
		 */
		bcopy(s_paddr, r_paddr, r_best);

		r_req->count -= r_best;
		r_req->buf   += r_best;

		s_req->count -= r_best;
		s_req->buf   += r_best;

		count -= r_best;
	}
#else
	bcopy(s_req->buf, r_req->buf, count);
#endif
	return (0);
}

void
rdma_lb_attempt_transfer( s_eng, r_eng )
	rdma_lb_engine_t  *s_eng;
	rdma_lb_engine_t  *r_eng;
{
	rdma_lb_engine_req_t	*s_req;
	rdma_lb_engine_req_t	*r_req;
	int			notify;

	TRACE("rdma_lb_attempt_transfer(s_eng=%x r_eng=%x)\n", s_eng, r_eng);

	assert(engine_is_valid(s_eng));
	assert(engine_is_valid(r_eng));

	s_req = &s_eng->rdma_send[ s_eng->rdma_send_head ];
	r_req = &r_eng->rdma_recv[ r_eng->rdma_recv_head ];
	
	/*
	 *  Deliver data while both sides have active buffers.
	 */
	while (s_req->active && r_req->active) {
		vm_size_t	count;
			
		/*
		 *  Set the count to the smaller buffer and make the copy.
		 */
		count = r_req->count;
		if (count > s_req->count)
			count = s_req->count;

		/*
		 *  Make the transfer.  Return is transfer was not complete.
		 *  (Page not present.)
		 */
		if (rdma_lb_xfer(s_eng->rdma_index, s_req,
				 r_eng->rdma_index, r_req, count)) {
			return;
		}

		/*
		 *  While construct takes care of 0 length chasers.
		 */
		while (r_req->active && (r_req->count == 0)) {
			TRACE("	receive request %x going inactive\n", r_req);

			r_req->active = 0;
			r_req->buf = 0;		/* for easy debug */

			notify = r_req->notify;	/* retain before increment */

			r_eng->rdma_recv_out++;
			r_eng->rdma_recv_head++;
			if (r_eng->rdma_recv_head == RDMA_MAXREQ) {
				r_eng->rdma_recv_head = 0;
				r_req = r_eng->rdma_recv;
			} else {
				r_req++;
			}

			if (notify) {
				TRACE("	giving RECEIVE notification for handle %d\n",
							r_eng->rdma_index);
				rdma_recv_intr(r_eng->rdma_index);
			}
		}

		/*
		 *  While construct takes care of 0 length chasers.
		 */
		while (s_req->active && (s_req->count == 0)) {
			TRACE("	send request %x going inactive\n", s_req);

			s_req->active = 0;
			s_req->buf = 0;		/* for easy debug */

			notify = s_req->notify;	/* retain before increment */

			s_eng->rdma_send_out++;
			s_eng->rdma_send_head++;
			if (s_eng->rdma_send_head == RDMA_MAXREQ) {
				s_eng->rdma_send_head = 0;
				s_req = s_eng->rdma_send;
			} else {
				s_req++;
			}

			if (notify) {
			TRACE("	giving SEND notification for handle %d\n",
							s_eng->rdma_index);
				rdma_send_intr(s_eng->rdma_index);
			}
		}
	}
}

void
rdma_lb_attempt_send( s_eng )
	rdma_lb_engine_t  *s_eng;
{
	rdma_lb_engine_t	*r_eng;

	TRACE("rdma_lb_attempt_send(s_eng=%x)\n", s_eng);

	assert(engine_is_valid(s_eng));

	if (s_eng->rdma_state == RDMA_LOOPBACK_ENG_CONNECTED) {
		r_eng = s_eng->rdma_endpoint;

		assert(engine_is_valid(r_eng));

		rdma_lb_attempt_transfer(s_eng, r_eng);
	}
}

void
rdma_lb_attempt_recv( r_eng )
	rdma_lb_engine_t  *r_eng;
{
	rdma_lb_engine_t	*s_eng;

	TRACE("rdma_lb_attempt_recv(r_eng=%x)\n", r_eng);

	assert(engine_is_valid(r_eng));

	s_eng = r_eng->rdma_endpoint;
	assert(engine_is_valid(s_eng));

	rdma_lb_attempt_transfer(s_eng, r_eng);
}

void
rdma_engine_accept( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t  *us;
	rdma_lb_engine_t  *them;

	TRACE("rdma_engine_accept(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	us = get_engine(handle);
	if (us->rdma_endpoint) {
		assert(engine_is_valid(us->rdma_endpoint));

		them = us->rdma_endpoint;

		assert(them->rdma_state == RDMA_LOOPBACK_ENG_CONNECTED);

		us->rdma_state = RDMA_LOOPBACK_ENG_CONNECTED;

		/*
		 *  Move data if there are posted sends/receives.
		 */
		rdma_lb_attempt_send(us);
		rdma_lb_attempt_send(them);
	} else {
		us->rdma_state = RDMA_LOOPBACK_ENG_ACCEPTING;
	}
}

void
rdma_engine_connect( token, handle )
	rdma_token_t	token;
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*us;
	rdma_lb_engine_t	*them;

	TRACE("rdma_engine_connect(token=%d handle=%d)\n", token, handle);

	assert(handle_is_valid(handle));
	assert(handle_is_valid((int)token));

	us   = get_engine(handle);
	us->rdma_state = RDMA_LOOPBACK_ENG_CONNECTED;

	them = get_engine(token);
	them->rdma_endpoint = us;
	us->rdma_endpoint = them;

	if (them->rdma_state != RDMA_LOOPBACK_ENG_ACCEPTING)
		return;

	them->rdma_state = RDMA_LOOPBACK_ENG_CONNECTED;

	/*
	 *  Move data if there are posted sends/receives.
	 */
	rdma_lb_attempt_send(us);
	rdma_lb_attempt_send(them);
}

void
rdma_engine_disconnect( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*eng;

	TRACE("rdma_engine_disconnect(handle=%d)\n", handle);

	assert(handle_is_valid(handle));
	assert(engine_is_idle(handle));

	eng = get_engine(handle);
	eng->rdma_state     = RDMA_LOOPBACK_ENG_IDLE;
	eng->rdma_endpoint  = 0;
	eng->rdma_send_head = 0;
	eng->rdma_send_tail = 0;
	eng->rdma_recv_head = 0;
	eng->rdma_recv_tail = 0;
	eng->rdma_send_in   = 0;
	eng->rdma_send_out  = 0;
	eng->rdma_recv_in   = 0;
	eng->rdma_recv_out  = 0;
}

int
rdma_engine_flush( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*eng;
	rdma_lb_engine_req_t	*req;

	TRACE("rdma_engine_flush(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);
	if (eng->rdma_state == RDMA_LOOPBACK_ENG_CONNECTED)
		return (RDMA_CONNECTED);
	
	/*
	 *  Flush send side.
	 */
	req = &eng->rdma_send[ eng->rdma_send_head ];
	while (1) {
		boolean_t	notify;

		/*
		 *  If the request is not active, we're done.
		 */
		if ( ! req->active )
			break;

		/*
		 *  Spruce the request up.
		 */
		req->map    = (vm_map_t) 0;
		req->buf    = 0;
		req->count  = 0;
		req->active = 0;

		/*
		 *  Defer notification until all maintainence is done
		 *  to prevent race where additional sends are attempted
		 *  from the callback before we have a chance to update
		 *  our request pointers.
		 */
		notify = req->notify;
		req->notify = 0;
		req++;

		eng->rdma_send_out++;
	  	eng->rdma_send_head++;
		if (eng->rdma_send_head == RDMA_MAXREQ) {
			req = eng->rdma_send;
			eng->rdma_send_head = 0;
		}

		/*
		 *  Should be save to notify now.
		 */
		if (notify)
			rdma_send_intr(eng->rdma_index);
	}

	assert(eng->rdma_send_head == eng->rdma_send_tail);
	eng->rdma_send_head = 0;
	eng->rdma_send_tail = 0;

	/*
	 *  Flush receive side.
	 */
	req = &eng->rdma_recv[ eng->rdma_recv_head ];
	while (1) {
		boolean_t	notify;

		/*
		 *  If this request is empty, we're done.
		 */
		if ( ! req->active )
			break;

		/*
		 *  Spruce the request up.
		 */
		req->map    = (vm_map_t) 0;
		req->buf    = 0;
		req->count  = 0;
		req->active = 0;

		/*
		 *  Defer notification until all maintainence is done
		 *  to prevent race where additional sends are attempted
		 *  from the callback before we have a chance to update
		 *  our request pointers.
		 */
		notify = req->notify;
		req->notify = 0;
		req++;

		eng->rdma_recv_out++;
	  	eng->rdma_recv_head++;
		if (eng->rdma_recv_head == RDMA_MAXREQ) {
			req = eng->rdma_recv;
			eng->rdma_recv_head = 0;
		}

		/*
		 *  Should be save to notify now.
		 */
		if (notify)
			rdma_recv_intr(eng->rdma_index);
	}

	assert(eng->rdma_recv_head == eng->rdma_recv_tail);
	eng->rdma_recv_head = 0;
	eng->rdma_recv_tail = 0;

	/*
	 *  Make the flush `sticky'
	 */
	eng->rdma_state = RDMA_LOOPBACK_ENG_FLUSHING;
	return (RDMA_SUCCESS);
}

void
rdma_engine_flush_endpoint( handle )
	rdma_token_t	handle;
{
	TRACE("rdma_engine_flush_endpoint(handle=%d)\n", handle);
	assert(handle_is_valid((int)handle));
}

rdma_seqid_t	
rdma_engine_send( handle, buf, count, notify, map )
	rdma_handle_t	handle;
	vm_offset_t	buf;
	vm_size_t	count;
	boolean_t	notify;
	vm_map_t	map;
{
	rdma_lb_engine_t	*eng;
	rdma_lb_engine_req_t	*req;
	boolean_t		empty;

	TRACE("rdma_engine_send(handle=%d buf=%x count=%d notify=%d map=%x)\n",
					handle, buf, count, notify, map);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);
	assert(eng->rdma_state != RDMA_LOOPBACK_ENG_IDLE);

	/*
	 *  If we're flushing, drop the request and give notification
	 *  if requested.
	 */
	if (eng->rdma_state == RDMA_LOOPBACK_ENG_FLUSHING) {
		if (notify)
			rdma_send_intr(handle);

		eng->rdma_send_in++;
		eng->rdma_send_out++;
		return(eng->rdma_send_in);
	}

	/*
	 *  Place the request in the send queue.
	 */
	req = &eng->rdma_send[ eng->rdma_send_tail ];
	assert( ! req->active );

	/*
	 *  If queue is empty and posting 0 length send, give notification
	 *  if requested and return;
	 */
	empty = is_send_queue_empty(eng);

	if (empty && (count == 0)) {
		if (notify) {
			TRACE("	giving SEND1 notification for handle %d\n",
									handle);
			rdma_send_intr(handle);
		}
		eng->rdma_recv_in++;
		eng->rdma_recv_out++;
		return (eng->rdma_recv_in);
	}

	/*
	 *  Post the send.
	 */
	req->map    = map;
	req->buf    = buf;
	req->count  = count;
	req->active = 1;
	req->notify = notify;

	eng->rdma_send_in++;
	eng->rdma_send_tail++;
	if (eng->rdma_send_tail == RDMA_MAXREQ)
		eng->rdma_send_tail = 0;
	
	/*
	 *  Smoke'um if ya got'um
	 *  N.B.  Empty check is needed to stop recursion problems.
	 */
	if (!empty)
		rdma_lb_attempt_send(eng);
	return (eng->rdma_send_in);
}

rdma_seqid_t
rdma_engine_recv( handle, buf, count, notify, map )
	rdma_handle_t	handle;
	vm_offset_t	buf;
	vm_size_t	count;
	boolean_t	notify;
	vm_map_t	map;
{
	rdma_lb_engine_t	*eng;
	rdma_lb_engine_req_t	*req;
	boolean_t		empty;

	TRACE("rdma_engine_recv(handle=%d buf=%x count=%d notify=%d map=%x)\n",
					handle, buf, count, notify, map);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);

	assert(eng->rdma_state != RDMA_LOOPBACK_ENG_IDLE);

	/*
	 *  If we're flushing, drop the request and give notification
	 *  if requested.
	 */
	if (eng->rdma_state == RDMA_LOOPBACK_ENG_FLUSHING) {
		if (notify)
			rdma_recv_intr(handle);
		eng->rdma_recv_in++;
		eng->rdma_recv_out++;
		return(eng->rdma_recv_in);
	}

	/*
	 *  Place the request in the send queue.
	 */
	req = &eng->rdma_recv[ eng->rdma_recv_tail ];
	assert( ! req->active );

	/*
	 *  If queue is empty and posting 0 length recv, give notification
	 *  if requested and return;
	 */
	empty = is_recv_queue_empty(eng);

	if (empty && (count == 0)) {
		if (notify) {
			TRACE("	giving RECEIVE1 notification for handle %d\n",
									handle);
			rdma_recv_intr(handle);
		}
		eng->rdma_recv_in++;
		eng->rdma_recv_out++;
		return (eng->rdma_recv_in);
	}

	/*
	 *  Post the receive.
	 */
	req->map    = map;
	req->buf    = buf;
	req->count  = count;
	req->active = 1;
	req->notify = notify;

	eng->rdma_recv_in++;
	eng->rdma_recv_tail++;
	if (eng->rdma_recv_tail == RDMA_MAXREQ)
		eng->rdma_recv_tail = 0;
	
	/*
	 *  Smoke'um if ya got'um
	 *  N.B.  Empty check is needed to stop recursion problems.
	 */
	if (!empty)
		rdma_lb_attempt_recv(eng);
	return (eng->rdma_recv_in);
}

/*
 *	RDMA engine polling interfaces
 */
boolean_t
rdma_engine_send_busy( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*eng;

	TRACE("rdma_engine_send_busy(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);
	return (eng->rdma_send[ eng->rdma_send_tail ].active);
}

boolean_t
rdma_engine_send_ready( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*eng;

	TRACE("rdma_engine_send_read(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);
	return ( ! eng->rdma_send[ eng->rdma_send_tail ].active );
}

boolean_t
rdma_engine_send_done( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*eng;

	TRACE("rdma_engine_send_more(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);
	return ( ! eng->rdma_send[ eng->rdma_send_head ].active );
}

boolean_t
rdma_engine_recv_busy( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*eng;

	TRACE("rdma_engine_recv_busy(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);
	return (eng->rdma_recv[ eng->rdma_recv_tail ].active);
}

boolean_t
rdma_engine_recv_ready( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*eng;

	TRACE("rdma_engine_recv_ready(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);
	return ( ! eng->rdma_recv[ eng->rdma_recv_tail ].active );
}

boolean_t
rdma_engine_recv_done( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*eng;

	TRACE("rdma_engine_recv_done(handle=%d)\n", handle);

	assert(handle_is_valid(handle));

	eng = get_engine(handle);
	return ( ! eng->rdma_recv[ eng->rdma_recv_head ].active );
}

rdma_seqid_t
rdma_engine_send_complete( handle )
	rdma_handle_t	handle;
{
	TRACE("rdma_engine_send_complete(handle=%d)\n", handle);

	assert(handle_is_valid(handle));
	return (get_engine(handle)->rdma_send_out);
}

rdma_seqid_t
rdma_engine_recv_complete( handle )
	rdma_handle_t	handle;
{
	TRACE("rdma_engine_recv_complete(handle=%d)\n", handle);

	assert(handle_is_valid(handle));
	return (get_engine(handle)->rdma_recv_out);
}

#if	PARAGON860

/*
 *	RDMA faulting interfaces
 */
void
rdma_recv_fault_info( handle, info )
	rdma_handle_t		handle;
	rdma_fault_info_t	*info;
{
	rdma_lb_engine_t	*eng;
	rdma_lb_engine_req_t	*req;

	TRACE("rdma_recv_fault_info(handle=%d info=%x)\n", handle, info);

	assert(handle_is_valid(handle));
	eng = get_engine(handle);

	req = &eng->rdma_recv[ eng->rdma_recv_head ];
	assert(req->active);

	info->map   = req->map;
	info->addr  = req->buf;
	info->count = req->count;
}

void
rdma_send_fault_info( handle, info )
	rdma_handle_t		handle;
	rdma_fault_info_t	*info;
{
	rdma_lb_engine_t	*eng;
	rdma_lb_engine_req_t	*req;

	TRACE("rdma_send_fault_info(handle=%d info=%x)\n", handle, info);

	assert(handle_is_valid(handle));
	eng = get_engine(handle);

	req = &eng->rdma_send[ eng->rdma_send_head ];
	assert(req->active);

	info->map   = req->map;
	info->addr  = req->buf;
	info->count = req->count;
}

void
rdma_resume_send( handle )
	rdma_handle_t	handle;
{
	TRACE("rdma_resume_send(handle=%d)\n", handle);

	assert(handle_is_valid(handle));
	rdma_lb_attempt_send(get_engine(handle));
}

void
rdma_resume_recv( handle )
	rdma_handle_t	handle;
{
	TRACE("rdma_resume_recv(handle=%d)\n", handle);

	assert(handle_is_valid(handle));
	rdma_lb_attempt_recv(get_engine(handle));
}

#endif

#if	MACH_KDB

/*
 *	Pretty-print an RDMA request.
 */
void
rdma_print_engine_request( req )
	rdma_lb_engine_req_t	*req;
{
	iprintf("buf=0x%x cnt=%d a=%d n=%d map=0x%x\n",
		req->buf,
		req->count,
		req->active,
		req->notify,
		req->map);
}


static char*
rdma_lb_engine_state_human( state )
	int	state;
{
	switch (state) {
	case RDMA_LOOPBACK_ENG_IDLE:
		return "idle";
	case RDMA_LOOPBACK_ENG_ACCEPTING:
		return "accepting";
	case RDMA_LOOPBACK_ENG_CONNECTED:
		return "connected";
	case RDMA_LOOPBACK_ENG_FLUSHING:
		return "flushing";
	}
	return "fobbed";
}


/*
 *	Pretty-print an RDMA engine.
 */
rdma_print_engine( handle )
	rdma_handle_t	handle;
{
	rdma_lb_engine_t	*rdma;
	int			h, t, count;
	extern int		indent;

	if ( ! handle_is_valid(handle)) {
		iprintf("warning: using ((rdma_lb_engine_t *) 0x%x)\n", handle);
		rdma = (rdma_lb_engine_t *) handle;
		handle = rdma - rdma_lb_engine;
	} else {
		rdma = &rdma_lb_engine[ handle ];
	}

	iprintf("rdma engine=%d (0x%x) {\n", handle, rdma);

	indent += 2;
	iprintf("state=0x%x [%s], handle=%d, endpoint=%d\n",
		rdma->rdma_state,
		rdma_lb_engine_state_human(rdma->rdma_state),
		rdma->rdma_index,
		rdma->rdma_endpoint ? rdma->rdma_endpoint->rdma_index : -1);

	iprintf("send_head=%d, send_tail=%d {\n",
		rdma->rdma_send_head,
		rdma->rdma_send_tail);
	indent += 2;
	count = 0;
	h = rdma->rdma_send_head;
	t = rdma->rdma_send_tail;
	while (h != t) {
		rdma_print_engine_request(&rdma->rdma_send[h]);
		if (++h == RDMA_MAXREQ)
			h = 0;
		count++;
	}
	indent -= 2;
	iprintf("} /* count=%d */,\n", count);

	iprintf("recv_head=%d, recv_tail=%d {\n",
		rdma->rdma_recv_head,
		rdma->rdma_recv_tail);
	indent += 2;
	count = 0;
	h = rdma->rdma_recv_head;
	t = rdma->rdma_recv_tail;
	while (h != t) {
		rdma_print_engine_request(&rdma->rdma_recv[h]);
		if (++h == RDMA_MAXREQ)
			h = 0;
		count++;
	}
	indent -= 2;
	iprintf("} /* count=%d */\n", count);

	indent -= 2;

	iprintf("}\n");
	return (int) handle;
}

#endif	/* MACH_KDB */
