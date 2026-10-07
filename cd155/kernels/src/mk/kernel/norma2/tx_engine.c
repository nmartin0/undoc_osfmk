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
 *
 * $Id: tx_engine.c,v 1.10 1994/11/18 20:58:56 mtm Exp $
 *
 * HISTORY:
 */

#include <mach_assert.h>

#include <mach/message.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_kmsg.h>
#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>
#include <norma2/dipc_uid.h>
#include <norma2/kmsg_parser.h>
#include <norma2/tx_engine.h>
#include <norma2/dipc_transit.h>
#include <norma2/norma_log.h>
#include <norma2/norma_transport.h>

/*
 * transmit engine stats
 */
#define	FASTPATH_STATS 0

typedef struct {
	unsigned long	page_faults;

	unsigned long	ool_data;
	unsigned long	ool_ports;
	unsigned long	inline_ports;

	unsigned long	simple_messages;
	unsigned long	complex_messages;

	unsigned long	dealloc_ast_deferred;
	unsigned long	dealloc_ast_seen;

	unsigned long	multipart_sends;
#if	FASTPATH_STATS
        unsigned long	complex_inline_32;
	unsigned long	complex_inline_64;
	unsigned long	complex_inline_96;
	unsigned long	complex_inline_128;
	unsigned long	complex_inline_160;
	unsigned long	complex_inline_192;
	unsigned long	complex_inline_huge;

        unsigned long	non_complex_32;
	unsigned long	non_complex_64;
	unsigned long	non_complex_96;
	unsigned long	non_complex_128;
	unsigned long	non_complex_160;
	unsigned long	non_complex_192;
	unsigned long	non_complex_huge;
#endif

} dipc_tx_stats_t;

#define	DIPC_TX_STATS(a) dipc_tx_engine_stats.a
dipc_tx_stats_t        dipc_tx_engine_stats;

/*
 *  Parsing function table for ipc_kmsg to net_kmsg conversion
 *  of message body.
 */
kmsg_parse_tbl_t dipc_cnvrt = {
	dipc_cnvrt_inline_port,		/* inline port */
	dipc_cnvrt_ool_port,		/* ool port    */
	0,				/* inline data */
	dipc_cnvrt_ool_data,		/* ool data    */
};

/*
 *  Parsing function table for transmission of out-of-line data
 */
kmsg_parse_tbl_t dipc_ool_send = {
	0,				/* inline port */
	dipc_send_ool_ports,		/* ool port    */
	0,				/* inline data */
	dipc_send_ool_data,		/* ool data    */
};

/*
 *  Parsing function table for net_kmsg resource disposal.
 */
kmsg_parse_tbl_t dipc_kmsg_dispose = {
	0,				/* inline port */
	dipc_free_ool_ports,		/* ool port    */
	0,				/* inline data */
	dipc_free_ool_data,		/* ool data    */
};

/*
 *  Callback queue structures.
 */
dipc_rdma_fault_queue_t		dipc_rdma_fault_queue;
dipc_kmsg_disposal_t		dipc_kmsg_disposal_queue;
dipc_kmsg_disposal_t		dipc_pageout_kmsg_disposal_queue;


boolean_t	dipc_private_pmaps = TRUE;

vm_map_t dipc_ool_map_create()
{
	vm_offset_t	min, max;
	vm_map_t	map;
	pmap_t		pmap;

	min = round_page(VM_MIN_ADDRESS) + PAGE_SIZE;
	max = trunc_page(VM_MAX_ADDRESS);
	map = vm_map_create(pmap_create(0), min, max, TRUE);
	assert(map != VM_MAP_NULL);
	map->wait_for_space = TRUE;
	map->no_coalesce = TRUE;

	pmap = vm_map_pmap(map);
	pmap->private_pmap = dipc_private_pmaps;

	return map;
}


vm_map_t	dipc_pageout_map;

void dipc_pageout_priv(thread_t thread)
{
	vm_map_t	map;

	map = dipc_ool_map_create();
	dipc_pageout_map = map;

	/*
	 *	To avoid a pageout deadlock (pageout thread sends
	 *	a message with ool data, vm_map_pmap_copy_range()
	 *	can't map it (no page table), sending begins and
	 *	triggers a send fault, the tx fault thread is dispatched
	 *	and can't get a page for the page table -- it needs to
	 *	pmap_expand() -- and it blocks waiting for a page and
	 *	holds the lock on the pageout thread's ool tx map),
	 *	we pre-allocate one page table.
	 */
	pmap_expand(map->pmap, round_page(VM_MIN_ADDRESS) + PAGE_SIZE);

	thread->dipc_ool_tx_map = map;
	thread->dipc_rdma_tx_group = NORMA_RDMA_GROUP_PAGEOUT;
}


/*
 *  Name:	rdma_send_fault_intr()
 *
 *  Input:	RDMA handle for associated with a transmit page not present.
 *
 *  Output:	The handle is placed in a queue serviced by the
 *		dipc_rdma_vm_fault_thread() thread.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	Interrupt context of callback is always run on the
 *			same CPU.  More than one thread may run this code
 *			requiring the page queue to employ a simple lock.
 *
 *  Description:	Put the handle for the transfer causing the fault
 *			in the dipc_rdma_vm_fault_thread() service queue.
 *			and call vm_fault to bring them in.
 */

volatile boolean_t dipc_rdma_vm_fault_thread_reawaken;
volatile boolean_t dipc_rdma_vm_fault_thread_awake;


void
dipc_rdma_vm_fault_thread_wakeup(event)
	int	event;
{
	tx_entry1(dipc_rdma_vm_fault_thread_wakeup, event);

	if (dipc_rdma_vm_fault_thread_awake) {
		dipc_rdma_vm_fault_thread_reawaken = TRUE;
	} else {
		dipc_rdma_vm_fault_thread_awake = TRUE;
		thread_wakeup_one(event);
	}
	DIPC_TX_STATS(page_faults++);
}

void
rdma_send_fault_intr(handle)
	rdma_handle_t handle;
{
	dipc_rdma_fault_queue_t	*p = &dipc_rdma_fault_queue;

	tx_entry1(rdma_send_fault_intr, handle);

	*(p->put++) = handle;
	if (p->put == p->bottom)
		p->put = p->top;
	assert(p->put != p->take);
	dipc_rdma_vm_fault_thread_wakeup((int) p);
}

/*
 *  Name:	dipc_rdma_vm_fault_thread()
 *
 *  Input:	None
 *
 *  Output:	Non-present page is faulted in.
 *
 *  Returns:		Never
 *
 *  MP and locking
 *  consideration:	Interrupt context of callback is always run on the
 *			same CPU.  More than one thread may run this code
 *			requiring the page queue to employ a simple lock.
 *
 *  Description:	Sleep waiting for the RDMA page-not-present callback
 *			to place the address of the missing page in the page
 *			fault queue.  On wakeup, remove all page addresses
 *			and call vm_fault to bring them in.
 */
void
dipc_rdma_vm_fault_thread()
{
	dipc_rdma_fault_queue_t	*p = &dipc_rdma_fault_queue;
	kern_return_t			kr;
	int				s;
	boolean_t			again;

	tx_entry0(dipc_rdma_vm_fault_thread);

	thread_set_own_priority(DIPC_RDMA_FAULT_THREAD_PRI);

	simple_lock_init(&p->take_ptr_lock);

	for (;;) {

		dipc_rdma_vm_fault_thread_reawaken = FALSE;

		/*
		 *  Service all available handles.
		 */
		for (;;) {
			rdma_handle_t		handle;
			rdma_fault_info_t	info;

			/*
			 *  Get the lock
			 */
			simple_lock(&p->take_ptr_lock);
		
			/*
			 *  If we're done, go back to sleep.
			 */
			if (p->put == p->take) {
				simple_unlock(&p->take_ptr_lock);
				break;
			}

			/*
			 *  Get and update the take pointer.
			 */
			handle = *(p->take++);
			if (p->take == p->bottom)
				p->take = p->top;
			simple_unlock(&p->take_ptr_lock);

			/*
			 *  Using the handle, get the VM info we need to
			 *  to satisfy the fault and fault the page(s) in.
			 */
			rdma_send_fault_info(handle, &info);

			if (!pmap_page_is_present(info.map->pmap, info.addr)) {
				tx_log5(1,
				"%s: page not present pmap=%x addr=%x cnt=%x\n",
					__FUNC__,
					info.map->pmap,
					info.addr,
					info.count);

				kr = vm_fault(info.map, trunc_page(info.addr),
					VM_PROT_READ,
					FALSE,
					FALSE,
					(void (*)()) 0);

				if (kr != KERN_SUCCESS) {
					tx_log3(0,
					    "%s: vm_fault() returned 0x%x\n",
						__FUNC__, kr);
					printf("vm_fault returned 0x%x ", kr);
					panic("fatal tx fault (1)");
				}
			}

			if ((round_page(info.addr) - info.addr) < info.count) {
				kr = vm_fault(	info.map, round_page(info.addr),
						VM_PROT_READ, FALSE, FALSE,
						(void (*)()) 0);
				if (kr != KERN_SUCCESS) {
					tx_log3(0,
					    "%s: vm_fault() returned 0x%x\n",
						__FUNC__, kr);
					printf("vm_fault returned 0x%x ", kr);
					panic("fatal tx fault (2)");
				}
			}

			/*
			 *  Have RDMA try again.
			 */
			rdma_resume_send(handle);
		}

		/*
		 *	Quick check -- keep going if additional transmission
		 *	faults have occurred.
		 */
		if (dipc_rdma_vm_fault_thread_reawaken == TRUE)
			continue;

		/*
		 *	Transmission fault queue looks empty; defer
		 *	callbacks and check for sure.
		 */
		s = splsched();
		if ((again = dipc_rdma_vm_fault_thread_reawaken) == FALSE) {
			assert_wait(p, FALSE);
			dipc_rdma_vm_fault_thread_awake = FALSE;
		}
		splx(s);

		if (again == FALSE)
			thread_block((void (*)()) 0);

	}
	/*NOTREACHED*/
	panic("dipc_rdma_vm_fault_thread thread exiting");
}

/*
 *  Name:	dipc_free_ool_data
 *
 *  Input:	Pointer to data to free and length of data. The type argument
 *		and opaque argument are not used in this function.
 *
 *  Output:	Out-of-line data item is freed.
 *
 *  Returns:		KERN_SUCCESS
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Free the out-of-line data item that is currently in
 *			the norma_map.
 */
/*ARGSUSED*/
kern_return_t
dipc_free_ool_data(type, length, data, map)
	mach_msg_type_t		*type;			/* unused */
	vm_size_t		length;
	vm_offset_t		*data;
	vm_map_t		map;
{
	tx_entry4(dipc_free_ool_data, type, length, *data, map);

	if (length)
		return (vm_deallocate(map, *data, length));
	else
		return (KERN_SUCCESS);
}

/*
 *  Name:	dipc_free_ool_ports
 *
 *  Input:	Pointer to ports to free and length of data. The type argument
 *		and opaque argument are not used in this function.
 *
 *  Output:	Out-of-line port memory is freed.
 *
 *  Returns:		KERN_SUCCESS
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Free the out-of-line port memory referenced.
 *
 */
/*ARGSUSED*/
kern_return_t
dipc_free_ool_ports(type, length, data, map)
	mach_msg_type_t		*type;			/* unused */
	vm_size_t		length;
	vm_offset_t		*data;
	vm_map_t		map;			/* unused */
{
	tx_entry4(dipc_free_ool_ports, type, length, *data, map);

	if (length)
		kfree(*data, length);
	return (KERN_SUCCESS);
}


volatile boolean_t dipc_kmsg_dealloc_thread_awake;
volatile boolean_t dipc_kmsg_dealloc_thread_reawaken;

volatile boolean_t dipc_pageout_kmsg_dealloc_thread_awake;
volatile boolean_t dipc_pageout_kmsg_dealloc_thread_reawaken;



void
dipc_kmsg_dealloc(kmsg)
	ipc_kmsg_t kmsg;
{
	vm_size_t	size;
	int		count;
	kern_return_t	kr = KERN_SUCCESS;

	tx_entry1(dipc_kmsg_dealloc, kmsg);

	assert(kmsg->ikm_transfer_index == 0);

	/*
	 *  If this is a complex message free any out-of-line
	 *  resources.
	 */
	if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX) {

		assert(kmsg->ikm_dipc_map != VM_MAP_NULL);
		tx_log2(3,"freeing complex data for %x\n",kmsg);
		size  = DIPC_MSG_BODY_SIZE(kmsg);
		count = -1;
		(void) norma_parse_kmsg(&count,
			(mach_msg_type_long_t*)(kmsg + 1),
					&size,
					&dipc_kmsg_dispose,
					&kr, kmsg->ikm_dipc_map);

		assert(kr == KERN_SUCCESS);
	}

	/*kmsg->ikm_dipc_map = VM_MAP_NULL;*/

	/*
	 *  Free the kmsg
	 */
	tx_log2(3,"freeing net_kmsg %x\n",kmsg);
	ipc_kmsg_free(kmsg);
}


void
dipc_kmsg_dealloc_loop(dispose)
	dipc_kmsg_disposal_t	*dispose;
{
	ipc_kmsg_t		kmsg;
	rdma_handle_t		handle;
	int			s;

	tx_entry1(dipc_kmsg_dealloc_loop, dispose);

	s = sploff();
	simple_lock(&dispose->take_ptr_lock);

	while (dispose->put != dispose->take) {

		kmsg = *(dispose->take++);
		if (dispose->take == dispose->bottom)
			dispose->take = dispose->top;

		simple_unlock(&dispose->take_ptr_lock);
		splon(s);

		/*
		 *	This is only a little sleazy --
		 *	when the send callback fired, the
		 *	RDMA handle gets jammed into
		 *	the ikm_prev field of the kmsg.
		 */
		handle = (rdma_handle_t) kmsg->ikm_prev;

		dipc_kmsg_dealloc(kmsg);

		/*
		 *  Disconnect and free the RDMA handle.
		 */
		rdma_disconnect(handle);
		rdma_handle_free(handle);

		s = sploff();
		simple_lock(&dispose->take_ptr_lock);

	}

	simple_unlock(&dispose->take_ptr_lock);
	splon(s);
}


void
dipc_dealloc_ast()
{
	thread_t	thread;
	int		s;

	tx_entry0(dipc_dealloc_ast);

	DIPC_TX_STATS(dealloc_ast_seen++);

	s = splsched();
	ast_off(cpu_number(), AST_DIPC_DEALLOC);
	splx(s);

	/*
	 *	We don't want some kinds of threads to deallocated
	 *	kmsgs from an AST (eg, they may block acquiring
	 *	write access to a transmission map to deallocate
	 *	address space).
	 *
	 *	Any idle thread is rejected, as is any thread
	 *	with vm_privilege (on the assumption that
	 *	vm_privilege implies a "special' thread that
	 *	should block in only well-understodd cases).
	 *
	 */
	thread = current_thread();
	if (((thread->state & TH_IDLE) == TH_IDLE) ||
	     (thread->vm_privilege == TRUE)) {

		DIPC_TX_STATS(dealloc_ast_deferred++);

		if (dipc_kmsg_dealloc_thread_awake) {
			dipc_kmsg_dealloc_thread_reawaken = TRUE;
		} else {
			dipc_kmsg_dealloc_thread_awake = TRUE;
			thread_wakeup_one((int) &dipc_kmsg_disposal_queue);
		}

		return;
	}

	dipc_kmsg_dealloc_loop(&dipc_kmsg_disposal_queue);
}


/*
 *  Name:	dipc_kmsg_dealloc_thread_continue
 *
 *  Input:	pointer to disposal queue, awake and reawake semaphores
 *
 *  Output:	All resources associated with a net_kmsg are deallocated
 *
 *  Returns:		Never
 *
 *  MP and locking
 *  consideration:	Interrupt context of callback is always run on the
 *			same CPU.  Only one thread services the disposal queue.
 *
 *  Description:	Sleep waiting for callback routine to place net_kmsg
 *			in disposal queue.  On wakeup, remove all net_kmsg's
 *			in the queue and free all associated resources.
 */
void
dipc_kmsg_dealloc_thread_continue(p, awake, reawake)
	dipc_kmsg_disposal_t	*p;
	volatile boolean_t	*awake;
	volatile boolean_t	*reawake;
{
	ipc_kmsg_t		kmsg;
	int			s;
	boolean_t		again;
	rdma_handle_t		handle;

	tx_entry3(dipc_kmsg_dealloc_thread_continue, p, awake, reawake);

	for (;;) {

		*reawake = FALSE;

		/*
		 *	Deallocate all net_kmsg's in the queue.
		 */
		dipc_kmsg_dealloc_loop(p);

		/*
		 *	Quick check -- keep going if additional disposal
		 *	interrupts have fired.
		 */
		if (*reawake == TRUE)
			continue;

		/*
		 *	Disposal queue looks empty; defer callbacks and
		 *	check for sure.
		 */
		s = splsched();
		if ((again = *reawake) == FALSE) {
			assert_wait((int) p, FALSE);
			*awake = FALSE;
		}
		splx(s);

		if (again == FALSE)
			thread_block((void (*)()) 0);
	}
	/*NOTREACHED*/
	panic("dipc_kmsg_dealloc_thread_continue thread exiting");
}


/*
 *  Name:	dipc_kmsg_dealloc_thread
 *
 *  Input:	None
 *
 *  Output:	All resources associated with a net_kmsg are deallocated
 *
 *  Returns:		Never
 *
 *  MP and locking
 *  consideration:	Interrupt context of callback is always run on the
 *			same CPU.  Only one thread services the disposal queue.
 *
 *  Description:	Sleep waiting for callback routine to place net_kmsg
 *			in disposal queue.  On wakeup, remove all net_kmsg's
 *			in the queue and free all associated resources.
 */
void
dipc_kmsg_dealloc_thread()
{
	tx_entry0(dipc_kmsg_dealloc_thread);

	thread_set_own_priority(DIPC_KMSG_DEALLOC_THREAD_PRI);

	/*
	 *  Give the deallocation thread VM privilege to prevent deadlock
	 *  with the pageout thread due to RDMA handle shortage.
	 */
	current_thread()->vm_privilege = TRUE;

	/*
	 *  Make it fixed priority.
	 */
	(void) thread_policy(current_thread(), POLICY_FIXEDPRI, 1);

	dipc_kmsg_dealloc_thread_continue(&dipc_kmsg_disposal_queue,
					  &dipc_kmsg_dealloc_thread_awake,
					  &dipc_kmsg_dealloc_thread_reawaken);
}

/*
 *  Name:	dipc_pageout_kmsg_dealloc_thread
 *
 *  Input:	None
 *
 *  Output:	All resources associated with a net_kmsg are deallocated
 *
 *  Returns:		Never
 *
 *  MP and locking
 *  consideration:	Interrupt context of callback is always run on the
 *			same CPU.  Only one thread services the disposal queue.
 *
 *  Description:	Handle all deallocations associated with paging thread.
 *			Sleep waiting for callback routine to place net_kmsg
 *			in disposal queue.  On wakeup, remove all net_kmsg's
 *			in the queue and free all associated resources.
 */
void
dipc_pageout_kmsg_dealloc_thread()
{
	tx_entry0(dipc_pageout_kmsg_dealloc_thread);

	thread_set_own_priority(DIPC_PAGEOUT_KMSG_DEALLOC_THREAD_PRI);

	/*
	 *  Anything related to the pageout thread must have VM privilege.
	 */
	current_thread()->vm_privilege = TRUE;

	/*
	 *  Make it fixed priority.
	 */
	(void) thread_policy(current_thread(), POLICY_FIXEDPRI, 1);

	dipc_kmsg_dealloc_thread_continue(
			&dipc_pageout_kmsg_disposal_queue,
			&dipc_pageout_kmsg_dealloc_thread_awake,
			&dipc_pageout_kmsg_dealloc_thread_reawaken
					 );
}

/*
 *  Name:	dipc_rdma_send_callback
 *
 *  Input:	RDMA handle associated with the RDMA notification
 *		pointer the ipc_kmsg associated with the RDMA notification.
 *
 *  Output:	ipc_kmsg pointer for completed transfers placed in disposal
 *		queue or additional requests posted to the RDMA transport.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	Interrupt context of callback is always run on the
 *			same CPU.  Only one thread services the disposal queue.
 *
 *  Description:	Makes additional RDMA transfer requests if the kmsg
 *			contains unsent out-of-line data.
 *
 *			Places completed kmsg's on the kmsg disposal queue if
 *			all out-of-line transfers are complete.
 */
#define	INDEX_TO_MSG_TYPE(k) \
    (mach_msg_type_long_t*)(((int)k) + (k->ikm_size - k->ikm_transfer_index))

int	dipc_dealloc_ast_enabled = 1;

void
dipc_rdma_send_callback(handle, kmsg)
	rdma_handle_t	handle;
	ipc_kmsg_t	kmsg;
{
	dipc_kmsg_disposal_t	*p;
	volatile boolean_t	*awake;
	volatile boolean_t	*reawake;
	boolean_t		special;
	int			s;

	tx_entry2(dipc_rdma_send_callback, handle, kmsg);

	/*
	 *  If there is possibly more to send, do it.
	 */
	if (kmsg->ikm_transfer_index) {
		dipc_parse_args_t	args;
		mach_msg_type_long_t	*type = INDEX_TO_MSG_TYPE(kmsg);
		int			count = RDMA_MAXREQ;
		kern_return_t		kr    = KERN_SUCCESS;

		DIPC_TX_STATS(multipart_sends++);

		args.to_send = count;
		args.handle  = handle;
		args.map     = kmsg->ikm_dipc_map;

		tx_log1(1,
		   "dipc_rdma_send_callback: Sending data from tx thread\n");

		(void) norma_parse_kmsg(&count, type, &kmsg->ikm_transfer_index,
					&dipc_ool_send, &kr, &args);
		assert(kr == KERN_SUCCESS);

		/*
		 *  If we found any work to do get out and wait for another
		 *  callback.  Otherwise, fall through to dispose of the spent
		 *  net_kmsg.
		 */
		if (count != RDMA_MAXREQ) {
			/*
			 *  We need to send a "chaser" to cause a callback
			 *  if we didn't fill the request queue.
			 */
			if (count != 0)
				(void)rdma_send(handle,
						(vm_offset_t)0,
						(vm_size_t)0,
						TRUE, args.map);
			return;
		}
	}

	/*
	 *  Put the spent net_kmsg in the disposal thread's queue.
	 * 
	 *  N.B.  The following queue interface assumes that in an MP system,
	 *	  a dedicated CPU services all interrupt for RDMA callbacks
	 *	  and a single disposal thread service the queue.  If round
	 *	  robin interrupt sharing is performed or more than one
	 *	  disposal thread is started, the appropriate locks would
	 *	  need to be added.
	 *
	 * Also:  Jam the RDMA handle that has finished into the
	 *	  kmsg->ikm_prev field for the deallocation mechanism
	 *	  to prevent handle allocations from racing ahead of
	 *	  kmsg deallocation.
	 */
	if (kmsg->ikm_dipc_map == dipc_pageout_map) {
		p       = &dipc_pageout_kmsg_disposal_queue;
		awake   = &dipc_pageout_kmsg_dealloc_thread_awake;
		reawake = &dipc_pageout_kmsg_dealloc_thread_reawaken;
		special = TRUE;
	} else {
		p       = &dipc_kmsg_disposal_queue;
		awake   = &dipc_kmsg_dealloc_thread_awake;
		reawake = &dipc_kmsg_dealloc_thread_reawaken;
		special = FALSE;
	}

	kmsg->ikm_prev = (ipc_kmsg_t) handle;

	simple_lock(&dispose->take_ptr_lock);

	*(p->put++) = kmsg;
	if (p->put == p->bottom)
		p->put = p->top;
	assert(p->put != p->take);

	simple_unlock(&dispose->take_ptr_lock);


	/*
	 *	If AST deallocation is enabled and the kmsg
	 *	is not receiving special attention, post an AST.
	 *
	 *	Otherwise, wakeup (or reawaken) a thread to
	 *	dispose of the kmsg.
	 */
	if (dipc_dealloc_ast_enabled && !special) {
		s = splsched();
		ast_on(cpu_number(), AST_DIPC_DEALLOC);
		splx(s);
	} else if (*awake) {
		*reawake = TRUE;
	} else {
		*awake = TRUE;
		thread_wakeup_one((int) p);
	}
}

/*
 *  Name:	dipc_send_ool_data
 *
 *  Input:	Pointer to data to send, length of data, and pointer to
 *		dipc_parse_args containing the RDMA handle to use and
 *		count of remaing RDMA requests to send.  The type argument
 *		is unused in the function.
 *
 *  Output:	One RDMA request is made and the count value in the
 *		dipc_parse_args is decremented.
 *
 *  Returns:		KERN_SUCCESS
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Post an rdma_send() for one out-of-line data type.
 *			If this is the last request to be made at this time,
 *			as indicated by a count of 1 in the dipc_parse_args
 *			structure, ask for a callback with the request.
 */
/*ARGSUSED*/
kern_return_t
dipc_send_ool_data(type, length, data, args)
	mach_msg_type_t		*type;			/* unused */
	vm_size_t		length;
	vm_offset_t		*data;
	dipc_parse_args_t	*args;
{
	tx_entry4(dipc_send_ool_data, type, length, *data, args->to_send);

	assert(args->to_send != 0);

	/*
	 *  N.B.  This could be a zero length send.
	 */
	(void)rdma_send(args->handle,
			RDMA_TRANSFER_ALIGNMENT(*data),
			RDMA_TRANSFER_LENGTH(*data, length),
			(args->to_send == 1 ? TRUE : FALSE),
			args->map);
	args->to_send--;

	DIPC_TX_STATS(ool_data++);

	return (KERN_SUCCESS);
}

/*
 *  Name:	dipc_send_ool_ports
 *
 *  Input:	Pointer to ports to send, length of data, and pointer to
 *		dipc_parse_args containing the RDMA handle to use and
 *		count of remaing RDMA requests to send.  The type argument
 *		is unused in the function.
 *
 *  Output:	One RDMA request is made and the count value in the
 *		dipc_parse_args is decremented.
 *
 *  Returns:		KERN_SUCCESS
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Post an rdma_send() for one out-of-line data type.
 *			If this is the last request to be made at this time,
 *			as indicated by a count of 1 in the dipc_parse_args
 *			structure, ask for a callback with the request.
 */
/*ARGSUSED*/
kern_return_t
dipc_send_ool_ports(type, length, data, args)
	mach_msg_type_t		*type;			/* unused */
	vm_size_t		length;
	vm_offset_t		*data;
	dipc_parse_args_t	*args;
{
	tx_entry4(dipc_send_ool_ports, type, length, *data, args->to_send);

	assert(args->to_send != 0);

	(void)rdma_send(args->handle,
			RDMA_TRANSFER_ALIGNMENT(*data),
			RDMA_TRANSFER_LENGTH(*data, length),
			(args->to_send == 1 ? TRUE : FALSE),
			kernel_map);
	args->to_send--;

	DIPC_TX_STATS(ool_ports++);

	return (KERN_SUCCESS);
}

/*
 *  Name:	dipc_cnvrt_port_array
 *
 *  Input:	Pointer to ports to convert and number of ports.
 *
 *  Output:	All ports are converted to UID's and the apporopriate
 *		number of transits are allocated.
 *
 *  Returns:	KERN_SUCCESS
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Convert ports to UID's and allocate transits were
 *			needed.
 */
kern_return_t
dipc_cnvrt_port_array(ports, type, num)
	ipc_port_t		*ports;
	mach_msg_type_long_t	*type;
	int			num;
{
	mach_msg_type_name_t	name;

	tx_entry3(dipc_cnvrt_port_array, ports, type, num);

	if (type->msgtl_header.msgt_longform)
		name = type->msgtl_name;
	else
		name = type->msgtl_header.msgt_name;

	/*
	 *  Convert local IPC ports (ipc_port_t) to UID's
	 */
	while ( num-- > 0 ) {
		*ports = (ipc_port_t) dipc_send_port(*ports, name, TRUE);
		ports++;
	}

	return (KERN_SUCCESS);
}

/*
 *  Name:	dipc_cnvrt_inline_port
 *
 *  Input:	Pointer to ports to convert and length of ports in bytes.
 *		The type and args argument is unused in the function.
 *
 *  Output:	All ports are converted to UID's and the apporopriate
 *		number of transits are alloated.
 *
 *  Returns:	Return status from dipc_cnvrt_port_array()
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Convert in-line ports to UID's
 */
/*ARGSUSED*/
kern_return_t
dipc_cnvrt_inline_port(type, length, ports, args)
	mach_msg_type_long_t	*type;
	vm_size_t		length;
	ipc_port_t		*ports;
	dipc_parse_args_t	*args;			/* unused */
{
	tx_entry4(dipc_cnvrt_inline_port, type, length, ports, args);
	DIPC_TX_STATS(inline_ports++);
	return dipc_cnvrt_port_array(ports, type, length / sizeof(ipc_port_t));
}

/*
 *  Name:	dipc_cnvrt_ool_data
 *
 *  Input:	Pointer to out-of-line data to convert, length, and a pointer
 *		to an out-of-line type counter.  The type argument is unused
 *		in the function.
 *
 *  Output:	The out-of-line copy object is installed into the NORMA map
 *		and the out-of-line counter is incremented.
 *
 *  Returns:	Return status from vm_map_copyout()
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Convert ports to UID's and allocate transits were
 *			needed.
 */
/*ARGSUSED*/
dipc_cnvrt_ool_data(type, length, copy, args)
	mach_msg_type_t		*type;			/* unused */
	vm_size_t		length;
	vm_map_copy_t		*copy;
	dipc_parse_args_t	*args;
{
	extern	int	vm_map_aggressive_enter_max;

	kern_return_t	kr;
	vm_offset_t	vaddr;
	vm_map_t	map;
	vm_offset_t	src_va;
	vm_size_t	size;
	int		copy_type;
	int		old_aggressive;

	tx_entry4(dipc_cnvrt_ool_data, type, length, *copy, args->to_send);

	/*
	 *  Increment the out-of-line type counter.
	 */
	args->to_send++;

	/*
	 *  Don't waste time converting zero bytes.  The RDMA engine will
	 *  do a NOP.
	 */
	if (length == 0) {
		tx_log1(1, "dipc_cnvrt_ool_data: zero length request\n");
		return (KERN_SUCCESS);
	}

	/*
	 *  Install the copy object in NORMA's map.
	 *  cache copy_object's start_VA and byte size as vm_map_copyout() can
	 *  remove the copy_object thus invalidating it's fields.
	 */
	assert( (*copy) != VM_MAP_COPY_NULL );
	/* see vm_map_copyin() | vm_map_copyin_page_list for offset == src_va */
	src_va = (*copy)->offset;
	size = (*copy)->size;
	copy_type = (*copy)->type;
	map = args->map;

#if	0
	/* As of WW40, this seems to break the EATS so I've backed it out. */
	old_aggressive = vm_map_aggressive_enter_max;
	vm_map_aggressive_enter_max = 0;
#endif
	kr = vm_map_copyout(map, &vaddr, *copy);
#if	0
	vm_map_aggressive_enter_max = old_aggressive;
#endif

	if ( kr != KERN_SUCCESS ) {
		tx_log2(0,
	 	    "dipc_cnvrt_ool_data: vm_map_copyout() returned %d\n",kr);
		/* rkl - need to delete the copy object */
		assert(kr == KERN_SUCCESS);
	} else {
		if (copy_type == VM_MAP_COPY_OBJECT) {
			/*
			 *  It appears that map copy objects of flavor
			 *  VM_MAP_COPY_OBJECT always have an offset of
			 *  0, and they are always the result of pageouts
			 *  of some form (lock request, data return, etc.).
			 *
			 *  Because there is "no way back" to a source
			 *  virtual address, the ...pmap_copy_range()
			 *  cannot duplicate a source pte, we know,
			 *  <for certain>, that sending this will trigger
			 *  a transmission fault later.  So let's do it
			 *  now instead of later.
			 */
			tx_log4(3, "%s: VM_MAP_COPY_OBJECT src_va=%x size=%d\n",
					__FUNC__, src_va, size);
			kr = vm_fault(	map,
					vaddr,
					VM_PROT_READ, FALSE, FALSE,
					(void (*)()) 0);
		} else {
			/*
			 *  Copy the physical map pointers (pte's) since
			 *  vm_map_copyout() lazy evaluates the address range
			 *  (i.e., no pmap_enters()).
			 *
			 *  This chunk of code exploits the fact that
			 *  ...ENTRY_LIST and ...PAGE_LIST copy objects
			 *  have an offset that reflects the original
			 *  virtual address of the source map.
			 */
			vm_map_pmap_copy_range(current_task()->map,
						src_va,
						round_page( src_va + size ),
						map,
						vaddr );
		}

		/*
		 *  Replace copy_obj pointer in kmsg with the new VA (from
		 *  the norma_map).
		 */
		*copy = (vm_map_copy_t)vaddr;
	}

	return (kr);
}

/*
 *  Name:	dipc_cnvrt_ool_port
 *
 *  Input:	Pointer to out-of-line ports to convert, length, and a pointer
 *		to an out-of-line type counter.  The type argument is unused
 *		in the function.
 *
 *  Output:	The out-of-line ports are converted to UID's and if needed,
 *		transits are obtained for send rights.
 *
 *  Returns:	Return status from dipc_cnvrt_port_array()
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Convert out-of-line ports to UID's.
 */
/*ARGSUSED*/
dipc_cnvrt_ool_port(type, length, ports, args)
	mach_msg_type_long_t	*type;
	vm_size_t		length;
	ipc_port_t		**ports;
	dipc_parse_args_t	*args;
{
	tx_entry4(dipc_cnvrt_ool_port, type, length, *ports, args->to_send);

	/*
	 *  Increment the out-of-line type counter and do the conversion.
	 */
	args->to_send++;
	return dipc_cnvrt_port_array(*ports, type, length/sizeof(ipc_port_t));
}

/*
 *  Name:	dipc_send_kmsg
 *
 *  Input:	Pointer an ipc_kmsg that has already had the mach_msg_header
 *		converted to network format and the RDMA handle associated
 *		RDMA token sent in the enqueue RPC.
 *
 *  Output:	The body of the kmsg is converted to network format and the
 *		net_kmsg posted for transmission with up to RDMA_MAXREQ - 1
 *		out-of-line buffers. When the transmission has completed the
 *		net-kmsg will be placed on the kmsg disposal thread work queue.
 *		Eventually the disposal thread will delete the kmsg.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Converts port rights and out-of-line data items to
 *			network format.  Initiates the transfer of the net_kmsg
 *			and up to RDMA_MAXREQ - 1 out-of-line data items.
 */
void
dipc_send_kmsg(kmsg, handle)
	ipc_kmsg_t	kmsg;
	rdma_handle_t	handle;
{
	thread_t	th;

	tx_entry2(dipc_send_kmsg, kmsg, handle);

	/*
	 *  Activate the handle and set a callback for when the RDMA request
	 *  queue is empty.
	 */
	rdma_accept(handle);
	rdma_set_send_callback(handle, (void(*)())dipc_rdma_send_callback,
							(rdma_notify_t)kmsg);
	/*
	 *  Assume it will go in one shot.
	 */
	kmsg->ikm_transfer_index = 0;

	/*
	 *  Set the ool transmission map to use.
	 */
	th = current_thread();
	if ((kmsg->ikm_dipc_map = th->dipc_ool_tx_map) == VM_MAP_NULL)
		kmsg->ikm_dipc_map = norma_map;

	/*
	 *  Deal with complex message.
	 *
	 *  N.B.  We'll walk the message body twice.  If this proves to
	 *	  be too slow, we'll build a buffer list while we parse.
	 */
	if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX) {
		mach_msg_type_long_t	*type = (mach_msg_type_long_t*)(kmsg+1);
		vm_size_t		size  =  DIPC_MSG_BODY_SIZE(kmsg);
		vm_size_t		size2 = size;
		int			parse_count = -1;
		int			ool_data;
		dipc_parse_args_t	args;
		kern_return_t		kr = KERN_SUCCESS;
		
		tx_log3(3, "converting kmsg 0x%x of size %d for transfer\n",\
					kmsg, kmsg->ikm_header.msgh_size );
		
		args.to_send = 0;
		args.handle  = handle;
		args.map     = kmsg->ikm_dipc_map;

		/*
		 *  Convert the ipc_kmsg body.
		 */
		(void) norma_parse_kmsg(&parse_count, type, &size,
					&dipc_cnvrt, &kr, &args);
		assert(kr == KERN_SUCCESS);

		ool_data = args.to_send;

		/*
		 *  If no out-of-line data was found, queue the net_kmsg
		 *  for transmission with a callback.
		 */
		if (ool_data == 0) {
#if	FASTPATH_STATS
			int	body_size = DIPC_MSG_BODY_SIZE(kmsg);

			if (body_size <= 32)
				DIPC_TX_STATS(complex_inline_32++);
			else if (body_size <= 64)
				DIPC_TX_STATS(complex_inline_64++);
			else if (body_size <= 96)
				DIPC_TX_STATS(complex_inline_96++);
			else if (body_size <= 128)
				DIPC_TX_STATS(complex_inline_128++);
			else if (body_size <= 160)
				DIPC_TX_STATS(complex_inline_160++);
			else if (body_size <= 192)
				DIPC_TX_STATS(complex_inline_192++);
			else
				DIPC_TX_STATS(complex_inline_huge++);
#endif
			tx_log2(3,"no OOL data in complex net_kmsg %x\n", kmsg);

			(void)rdma_send(handle,
				(vm_offset_t)kmsg,
				ikm_plus_overhead(kmsg->ikm_header.msgh_size),
				TRUE, kernel_map);
		} else {
			boolean_t		more_to_send;

			tx_log3(3,"sending complex kmsg %x of size %d\n", kmsg,
				ikm_plus_overhead(kmsg->ikm_header.msgh_size));

			/*
			 *  Send the net_kmsg without a callback.
			 */
			(void)rdma_send(handle,
				(vm_offset_t)kmsg,
				ikm_plus_overhead(kmsg->ikm_header.msgh_size),
				FALSE, kernel_map);

			/*
			 *  Make sure we don't overrun the request queue.
			 */
			if (ool_data > (RDMA_MAXREQ - 1)) {
				ool_data = RDMA_MAXREQ - 1;
				more_to_send = TRUE;
				tx_log3(1,"over %d OOL entires in net_ksmg %x\n",
							RDMA_MAXREQ - 1, kmsg);
			} else {
				more_to_send = FALSE;
			}

			/*
			 *  Have the parser send out-of-line data type.  The
			 *  last one out will ask for a callback.
			 */
			args.to_send = ool_data;
			(void) norma_parse_kmsg(&ool_data, type, &size2,
						&dipc_ool_send, &kr, &args);
			assert(kr == KERN_SUCCESS);

			/*
			 *  If we couldn't do it all in one pass, set the
			 *  remaining number of bytes to process.
			 */
			if (more_to_send)
				kmsg->ikm_transfer_index = size2;
		}

		DIPC_TX_STATS(complex_messages++);
	}

	/*
	 *  The non-complex case is a snap.
	 */
	else {
#if	FASTPATH_STATS
		int	body_size = DIPC_MSG_BODY_SIZE(kmsg);

		if (body_size <= 32)
			DIPC_TX_STATS(non_complex_32++);
		else if (body_size <= 64)
			DIPC_TX_STATS(non_complex_64++);
		else if (body_size <= 96)
			DIPC_TX_STATS(non_complex_96++);
		else if (body_size <= 128)
			DIPC_TX_STATS(non_complex_128++);
		else if (body_size <= 160)
			DIPC_TX_STATS(non_complex_160++);
		else if (body_size <= 192)
			DIPC_TX_STATS(non_complex_192++);
		else
			DIPC_TX_STATS(non_complex_huge++);
#endif

		tx_log3(3, "sending simple net_kmsg %x of size %d\n", kmsg,
			ikm_plus_overhead(kmsg->ikm_header.msgh_size));

		(void)rdma_send(handle,
				(vm_offset_t)kmsg,
				ikm_plus_overhead(kmsg->ikm_header.msgh_size),
				TRUE, kernel_map);

		DIPC_TX_STATS(simple_messages++);
	}
}

/*
 *  Name:	dipc_tx_engine_init
 *
 *  Input:	None
 *
 *  Output:	Resources to operation the transmit machine are allocated and
 *		the kmsg deallocation and VM fault threads are created.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Allocate the resources needed to operate the transmit
 *			engine.  This is primarily the callback queues and
 *			queue service threads.
 */
void
dipc_tx_engine_init()
{
	extern	int	dipc_rdma_tx_group_size;

	rdma_handle_t	*q1;
	ipc_kmsg_t	*q2;
	int		slots = dipc_rdma_tx_group_size + 1;

	tx_entry0(dipc_tx_engine_init);

	/*
	 *  Create and initialze VM fault callback queue along the the
	 *  thread that will service it.
	 */
	q1 = (rdma_handle_t*) kalloc(sizeof(vm_offset_t) * slots);
	dipc_rdma_fault_queue.top    = q1;
	dipc_rdma_fault_queue.put    = q1;
	dipc_rdma_fault_queue.take   = q1;
	dipc_rdma_fault_queue.bottom = q1 + slots;
	simple_lock_init(&dipc_rdma_fault_queue.take_ptr_lock);

	(void) kernel_thread(kernel_task, dipc_rdma_vm_fault_thread, (char*)0);

	/*
	 *  Create and initialze pageout deallocation thread and queues.
	 */
	q2 = (ipc_kmsg_t*) kalloc(sizeof(vm_offset_t) * slots);
	dipc_pageout_kmsg_disposal_queue.top    = q2;
	dipc_pageout_kmsg_disposal_queue.put    = q2;
	dipc_pageout_kmsg_disposal_queue.take   = q2;
	dipc_pageout_kmsg_disposal_queue.bottom = q2 + slots;
	simple_lock_init(&dipc_pageout_kmsg_disposal_queue.take_ptr_lock);

	(void) kernel_thread(kernel_task, dipc_pageout_kmsg_dealloc_thread,
								(char*)0);
	/*
	 *  Create and initialze common deallocation thread and queues.
	 */
	q2 = (ipc_kmsg_t*) kalloc(sizeof(vm_offset_t) * slots);
	dipc_kmsg_disposal_queue.top    = q2;
	dipc_kmsg_disposal_queue.put    = q2;
	dipc_kmsg_disposal_queue.take   = q2;
	dipc_kmsg_disposal_queue.bottom = q2 + slots;
	simple_lock_init(&dipc_kmsg_disposal_queue.take_ptr_lock);

	(void) kernel_thread(kernel_task, dipc_kmsg_dealloc_thread, (char*)0);

	/*
	 * init stat counters.
	 */
	bzero( (char *)&dipc_tx_engine_stats, sizeof(dipc_tx_stats_t) );
}

void
db_dipc_tx_stats()
{
        register dipc_tx_stats_t    *s=&dipc_tx_engine_stats;

        db_printf("Transmit engine stats:\n");
        db_printf("  page faults            %8d",   s->page_faults);
        db_printf("  inline ports           %8d\n", s->inline_ports);

        db_printf("  ool ports              %8d",   s->ool_ports);
        db_printf("  ool data               %8d\n", s->ool_data);

        db_printf("  simple messages        %8d",   s->simple_messages);
        db_printf("  complex messages       %8d\n", s->complex_messages);

        db_printf("  AST deallocs seen      %8d",   s->dealloc_ast_seen);
        db_printf("  deferred AST dealloc   %8d\n", s->dealloc_ast_deferred);

        db_printf("  multi-part sends       %8d\n", s->multipart_sends);

#if	FASTPATH_STATS
	db_printf("Complex inline stats\n");
        db_printf("  size_32  %6d",   s->complex_inline_32);
        db_printf("  size_96  %6d",   s->complex_inline_96);
        db_printf("  size_160 %6d",   s->complex_inline_160);
        db_printf("  huge     %6d\n", s->complex_inline_huge);
        db_printf("  size_64  %6d",   s->complex_inline_64);
        db_printf("  size_128 %6d",   s->complex_inline_128);
        db_printf("  size_192 %6d",   s->complex_inline_192);
        db_printf("  total    %6d\n", s->complex_inline_32  +
				      s->complex_inline_64  +
				      s->complex_inline_96  +
				      s->complex_inline_128 +
				      s->complex_inline_160 +
				      s->complex_inline_192 +
				      s->complex_inline_huge);

	db_printf("Non-Complex inline stats\n");
        db_printf("  size_32  %6d",   s->non_complex_32);
        db_printf("  size_96  %6d",   s->non_complex_96);
        db_printf("  size_160 %6d",   s->non_complex_160);
        db_printf("  huge     %6d\n", s->non_complex_huge);
        db_printf("  size_64  %6d",   s->non_complex_64);
        db_printf("  size_128 %6d",   s->non_complex_128);
        db_printf("  size_192 %6d",   s->non_complex_192);
        db_printf("  total    %6d\n", s->non_complex_32  +
				      s->non_complex_64  +
				      s->non_complex_96  +
				      s->non_complex_128 +
				      s->non_complex_160 +
				      s->non_complex_192 +
				      s->non_complex_huge);

#endif
}

#include<ipc/ipc_entry.h>
struct ipc_object*
db_port_by_name(task, name)
	task_t		task;
	mach_port_t	name;
{
	return ((ipc_entry_lookup(task->itk_space, name))->ie_object);
}

