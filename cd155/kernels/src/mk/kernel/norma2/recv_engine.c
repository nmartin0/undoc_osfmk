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
 *	$Id: recv_engine.c,v 1.9 1995/03/17 22:30:38 terry Exp $
 */

#include <cpus.h>
#include <norma_ipc.h>
#include <mach_kdb.h>
#include <mach_assert.h>

#include <mach/mach_types.h>
#include <mach/boolean.h>
#include <kern/queue.h>
#include <mach/vm_param.h>
#include <mach/kern_return.h>

#include <kern/kalloc.h>
#include <kern/task.h>

#include <ipc/ipc_kmsg.h>

#include <vm/vm_map.h>

#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>

#include <norma2/meta_kmsg.h>
#include <norma2/kmsg_parser.h>
#include <norma2/norma_transport.h>
#include <norma2/norma_log.h>
#include <norma2/recv_engine.h>

#if MACH_ASSERT
#include <device/net_status.h>
#endif

extern vm_map_t	kernel_map;
extern vm_map_t	dipc_kernel_recv_ool_map;

/*
 * Receive engine stats
 */
typedef struct {
	unsigned long	kmem_recv_posts;
	unsigned long	kmem_recv_blocks;

	unsigned long	ool_recv_posts;
	unsigned long	ool_recv_blocks;

	unsigned long	ool_ports;

	unsigned long	simple_messages;
	unsigned long	complex_messages;
} dipc_recv_stats_t;

#define	DIPC_RX_STATS(a) dipc_recv_engine_stats.a
dipc_recv_stats_t        dipc_recv_engine_stats;

/*
 * Initialize the receive engine.
 */

void
dipc_recv_engine_init()
{
	/*
	 * init receive engine stat counters
	 */
	bzero( (char *)&dipc_recv_engine_stats, sizeof(dipc_recv_stats_t) );
}


/*
 * RDMA recv callback fuction. generate a wakeup for the specified event
 *
 * inputs:
 *	handle		RDMA handle_t
 *	sleeping_on	(rdma_notify_t) bit pattern we are sleeping on.
 *
 * outputs:
 *	none.
 */

/* ARGSUSED */
void
recv_callback( handle, sleeping_on )
	rdma_handle_t	handle;		/* unused */
	rdma_notify_t	sleeping_on;
{
	thread_wakeup( sleeping_on );
}

/*
 * Receive data from the RDMA interface, waiting until we have it all.
 *
 * inputs:
 *	handle	RDMA handle
 *	addr	address where data goes
 *	size	byte size of data to be recv'ed
 *	map	pointer to map for VM faults.
 *
 * outputs:
 *	none.
 */

#define	RDMA_KMEM_SPIN_DELAY		10

#if 0
#define	RDMA_KMEM_SPIN_BUCKET_MAX	50
#endif

#if	RDMA_KMEM_SPIN_BUCKET_MAX
unsigned long	recv_rdma_kmem_spin_histogram_overflows;
unsigned long	recv_rdma_kmem_spin_histogram[ RDMA_KMEM_SPIN_BUCKET_MAX ];

#if	RDMA_KMEM_SPIN_BUCKET_MAX
int db_spin_buckets()
{
	int	i, total = 0;

	db_printf(" us\t count (overflow=%u)\n",
		recv_rdma_kmem_spin_histogram_overflows);
	for (i = 0; i < RDMA_KMEM_SPIN_BUCKET_MAX; i++) {
		total += recv_rdma_kmem_spin_histogram[i];
		db_printf("%3d\t%6u\n",
			i * RDMA_KMEM_SPIN_DELAY,
			recv_rdma_kmem_spin_histogram[i]);
	}

	return total;
}

int db_reset_spin_buckets()
{
	int	i;

	recv_rdma_kmem_spin_histogram_overflows = 0;
	for (i = 0; i < RDMA_KMEM_SPIN_BUCKET_MAX; i++)
		recv_rdma_kmem_spin_histogram[i] = 0;

	return 0;
}
#endif	MACH_KDB

#endif	/* RDMA_KMEM_SPIN_BUCKET_MAX */

void
recv_rdma_kmem_data( handle, addr, size, map, max_wait )
	rdma_handle_t	handle;
	vm_offset_t	addr;
	vm_size_t	size;
	vm_map_t	map;
	int		max_wait;
{
	boolean_t	done;
#if	RDMA_KMEM_SPIN_BUCKET_MAX
	int		mics = 0;
#endif	/* RDMA_KMEM_SPIN_BUCKET_MAX */

	/*
	 * post the recv request and spin.
	 */
	(void)rdma_recv(handle, addr, size, FALSE, map);
	DIPC_RX_STATS(kmem_recv_posts++);

	for (max_wait /= RDMA_KMEM_SPIN_DELAY; max_wait; max_wait--) {
		if (done = rdma_recv_done( handle ))
			break;

#if	RDMA_KMEM_SPIN_BUCKET_MAX
		mics += RDMA_KMEM_SPIN_DELAY;
#endif	/* RDMA_KMEM_SPIN_BUCKET_MAX */
		delay(RDMA_KMEM_SPIN_DELAY);
	}

#if	RDMA_KMEM_SPIN_BUCKET_MAX
	{
		if ((mics/10) > RDMA_KMEM_SPIN_BUCKET_MAX)
			recv_rdma_kmem_spin_histogram_overflows++;
		else
			recv_rdma_kmem_spin_histogram[mics / 10]++;
	}
#endif	/* RDMA_KMEM_SPIN_BUCKET_MAX */

	/*
	 * If spinning did not get us a completion, do the big sleep
	 * with a null rdma_recv() callback chaser to wake us.
	 */
	if ( ! done ) {
		rdma_set_recv_callback( handle, recv_callback,
					(rdma_notify_t)&handle );

		assert_wait( &handle, FALSE );
		(void)rdma_recv(handle, 0, 0, TRUE, map);
		thread_block( (void (*)()) 0 );

		DIPC_RX_STATS(kmem_recv_blocks++);
	}
}

int	ice_cube_pages = 2;

#define	CUBE_SIZE			(ice_cube_pages << PAGE_SHIFT)
#define	BYTE_OFFSET(addr)		((addr) & page_mask)
#define	BEST_FREEZE_RANGE(addr, offset)	(CUBE_SIZE - (offset))
#define	PAGE_COUNT(offset, size)	(round_page(size+offset) >> PAGE_SHIFT)

vm_size_t
ice_it(addr, size, map)
	vm_offset_t	addr;
	vm_size_t	size;
	vm_map_t	map;
{
	kern_return_t	kr;
	vm_size_t	fsize, offset;

	/*
	 *  Determine optimal freeze range.
	 */
	offset = BYTE_OFFSET(addr);
	fsize = BEST_FREEZE_RANGE(addr, offset);
	if ( size < fsize )
		fsize = size;
	/*
	 *  Put it on ice.
	 */
	kr = vm_page_freeze_range(map, addr, PAGE_COUNT(offset, fsize), TRUE);
	if (kr != KERN_SUCCESS) {
		printf("vm_page_freeze_range() kr=%d\n",kr);
		panic("ice_it()");
	}
	return fsize;
}

/*
 * copy OOL data into the user's address space. We create & freeze VM pages
 * in the user's address space. Pages are thawed after OOL data is copied in.
 * Note in the type descriptor for the OOL data wheather data was put in
 * the user's address space (msgtl_header.msgt_unused, 1 imples data was
 * placed in the user's address space, 0 == not). `msgtl_header.msgt_unused'
 * is observed in ipc_kmsg.c, rtn:ipc_kmsg_copyout_body().
 *
 * logic here needs to be filled in so multiple RDMA recv requests can be
 * outstanding.
 *
 * inputs:
 *	bsize	actual # of bytes of OOL data.
 *	srcAddr	pointer to where the OOL data is.
 *
 * outputs:
 *	KERN_SUCCESS
 *	otherwise kern_return error
 */

#define OOL_RECV_MAX_WAIT	150	/* in microseconds */
#define	OOL_DELAY_TIME		10	/* in microseconds */

extern int coalesce_recv = 0;

/* ARGSUSED */
kern_return_t
recv_ool_data( type, bsize, srcAddr, handle )
	mach_msg_type_long_t	*type;		/* unused */
	vm_size_t		bsize;
	vm_offset_t		*srcAddr;	/* IN/OUT */
	rdma_handle_t		handle;
{
	kern_return_t	kr;
	vm_map_t	users_map;
	vm_offset_t	dest, offset, data_start;
	vm_size_t	alloc_size;
	vm_offset_t	cur_addr;
	vm_size_t	cur_size;
	vm_offset_t	last_addr;
	vm_size_t	last_size;
	rdma_seqid_t	last_id;
	int		max_wait;
	boolean_t	isKernelTask = FALSE;

	rx_entry4(recv_ool_data, type, bsize, srcAddr, handle );

	/*
	 * is this a kernel task?
	 */
	if ( vm_map_pmap(current_task()->map) == kernel_pmap ) {
		isKernelTask = TRUE;
		assert(current_task()->map == kernel_map);
		users_map = dipc_kernel_recv_ool_map;
		rx_log2(3, "%s: (kernel task) recv OOL data.\n", __FUNC__);
	} else {
		users_map = current_task()->map;
	}

	/*
	 * indicate we have not put any OOL data in the user's address space.
	 */
	type->msgtl_header.msgt_unused = 0;

	/*
	 * protect aginst bad OOL size. OOL VA is NOT valid on this node,
	 * it was on the sending node during the transmission of this kmsg. 
	 */
	assert((int) bsize >= 0);

	if ( (int)bsize == 0 )
		return KERN_SUCCESS;

	/*
	 * allocate VM in the user's task address space
	 */
	offset = ((*srcAddr) & page_mask);
	alloc_size = round_page(bsize + offset);
	dest = 0;
	/*
	 * see if coalescing of objects is wanted on recieves
	 */
	if(!isKernelTask && coalesce_recv == 0) {
		kr = vm_allocate_coalesce(users_map, &dest, alloc_size, TRUE, FALSE);
	} else {
		kr = vm_allocate(users_map, &dest, alloc_size, TRUE, TRUE);
	}


	/*
	 * if unable to create VM space in user land then bitbucket the RDMA
	 * data and return.
	 */
	if ( kr != KERN_SUCCESS ) {
		rdma_flush(handle);
		return kr;
	}

	/*
	 * we must preserve the offset of the sender.
	 */
	data_start = dest+offset;
	cur_addr = RDMA_TRANSFER_ALIGNMENT(data_start);
	cur_size = RDMA_TRANSFER_LENGTH(data_start,bsize);

	/*
	 *  Freeze and post 1st range of memory.
	 */
resync:
	last_addr = cur_addr;
	last_size = ice_it(cur_addr, cur_size, users_map);
	last_id = rdma_recv(handle, last_addr, last_size, FALSE, users_map);
	DIPC_RX_STATS(ool_recv_posts++);

	cur_addr += last_size;
	cur_size -= last_size;

	while( cur_size ) {
		vm_offset_t	next_addr;
		vm_size_t	next_size;
		rdma_seqid_t	next_id;

		next_addr = cur_addr;
		next_size = ice_it(cur_addr, cur_size, users_map);
		next_id = rdma_recv(handle, next_addr, next_size,
							FALSE, users_map);
		DIPC_RX_STATS(ool_recv_posts++);

		cur_size -= next_size;
		cur_addr += next_size;

		max_wait = OOL_RECV_MAX_WAIT / OOL_DELAY_TIME;
		while ( max_wait && (rdma_recv_complete(handle) < last_id )) {
			max_wait--;
			delay(OOL_DELAY_TIME);
		}

		/*
		 *  If data hasn't arrived by now, set a callback and wait
		 *  for it.
		 */
		if (max_wait == 0) {
			rdma_set_recv_callback( handle, recv_callback,
						(rdma_notify_t)&handle );
			/*
			 *  Post a zero length receive to generate callback.
			 */
			assert_wait( &handle, FALSE );
			(void)rdma_recv(handle, 0, 0, TRUE, users_map);
			thread_block( (void (*)()) 0 );

			DIPC_RX_STATS(ool_recv_blocks++);

			pmap_thaw_quickly(vm_map_pmap(users_map), last_addr,
			   PAGE_COUNT(BYTE_OFFSET(last_addr), last_size));
			pmap_thaw_quickly(vm_map_pmap(users_map), next_addr,
			   PAGE_COUNT(BYTE_OFFSET(next_addr), next_size));

			if (cur_size)
				goto resync;
			else
				goto done;
		}

		/*
		 * Thaw the received range
		 */
		pmap_thaw_quickly(vm_map_pmap(users_map), last_addr,
			PAGE_COUNT(BYTE_OFFSET(last_addr), last_size));

		last_size = next_size;
		last_addr = next_addr;
		last_id   = next_id;
	}

	max_wait = OOL_RECV_MAX_WAIT / OOL_DELAY_TIME;
	while ( max_wait && !rdma_recv_done( handle ) ) {
		max_wait--;
		delay(OOL_DELAY_TIME);
	}

	/*
	 *  If the remaining data hasn't arrived by now, set a callback and wait
	 *  for it.
	 */
	if (max_wait == 0) {
		rdma_set_recv_callback( handle, recv_callback,
					(rdma_notify_t)&handle );
		/*
		 *  Post a zero length receive to generate callback.
		 */
		assert_wait( &handle, FALSE );
		(void)rdma_recv(handle, 0, 0, TRUE, users_map);
		thread_block( (void (*)()) 0 );
	}

	/*
	 * Thaw the received range
	 */
	pmap_thaw_quickly(vm_map_pmap(users_map), last_addr,
		PAGE_COUNT(BYTE_OFFSET(last_addr), last_size));

done:
	/*
	 * If the receiving task is a 'kernel' task/thread (kserver thread)
	 * then the address placed in the kmsg is a page-list copy-object
	 * pointer an not the VA address of the data. Copy-objects are expected
	 * by kernel routines (aka kserver threads).
	 * 
	 * Otherwise, it's a user VA where the data starts.
	 */

	if ( isKernelTask ) {
		kern_return_t	rc;
		vm_map_copy_t	copy_obj=VM_MAP_COPY_NULL;

		/*
		 * Produce a page_list copy object.
		 */
		rc = vm_map_copyin_page_list(
				users_map,
				data_start,
				bsize,
				TRUE,		/* destroy src */
				TRUE,		/* steal pages */
				&copy_obj,
				FALSE,		/* ! a continuation */
				VM_PROT_WRITE,	/* protection */
				FALSE );	/* do zero-fill */

		if ( rc != KERN_SUCCESS ) {
			rx_log7(0,
			  "%s map %x VA %x sz %d cp %x kr %d\n", __FUNC__,
				users_map,data_start,alloc_size,copy_obj,kr);
			panic("recv_ool_data: vm_map_copyin_page_list()");
		}
		*srcAddr = (vm_offset_t) copy_obj; /* set the copy object */
	}
	else {
		/*
		 * Reset the OOL data pointer in the USER's message to point
		 * where we stored the data (in the User's address space),
		 * indicate we have done this. 'msgtl_header.msgt_unused' is
		 * observed by the message body copyout routine to skip the
		 * OOL data copyout as we have already done it here.
		 */

		*srcAddr = data_start;
		type->msgtl_header.msgt_unused = 1;
	}

	return kr;
}


/* ARGSUSED */
kern_return_t
dealloc_ool_data( type, bsize, srcAddr, handle )
	mach_msg_type_long_t	*type;		/* unused */
	vm_size_t		bsize;
	vm_offset_t		*srcAddr;	/* IN */
	rdma_handle_t		handle;
{
	vm_map_t	users_map;
	vm_offset_t	offset;
	vm_size_t	alloc_size;

	/*
	 * protect aginst bad OOL size and data pointer.
	 */
	if ( ((int)bsize <= 0) || (*srcAddr == 0) )
		return KERN_SUCCESS;

	users_map = current_task()->map;

	/*
	 * deallocate VM in the user's task address space
	 */
	offset = ((*srcAddr) & page_mask);
	alloc_size = round_page(bsize + offset);

	vm_deallocate(users_map, *srcAddr, alloc_size);

	return KERN_SUCCESS;
}

/* ARGSUSED */
kern_return_t
dealloc_ool_port( type, data_size, addr, handle )
	mach_msg_type_long_t	*type;		/* unused */
	vm_size_t		data_size;
	vm_offset_t		*addr;
	rdma_handle_t		handle;
{
	/*
	 * protect aginst bad OOL size and data pointer.
	 */
	if ( ((int)data_size <= 0) || (*addr == 0) )
		return KERN_SUCCESS;

	/* release kernel memory */
	kfree( *addr, data_size );

	/* any port functions which need to be handled? */

	return KERN_SUCCESS;
}

#define	RECV_OOL_PORT_WAIT	200	/* microseconds */

/* ARGSUSED */
kern_return_t
recv_ool_port( type, data_size, addr, handle )
	mach_msg_type_long_t	*type;		/* unused */
	vm_size_t		data_size;
	vm_offset_t		*addr;
	rdma_handle_t		handle;
{
	vm_offset_t	cur_addr;
	vm_size_t	cur_size;

	/*
	 * protect aginst bad OOL size and data pointer.
	 */
	if ( ((int)data_size <= 0) || (*addr == 0) )
		return KERN_SUCCESS;

	if ( (*addr = kalloc(data_size)) == (vm_offset_t)0 )
		return KERN_RESOURCE_SHORTAGE;

	cur_addr = RDMA_TRANSFER_ALIGNMENT(*addr);
	cur_size = RDMA_TRANSFER_LENGTH(*addr, data_size);

	/*
	 *  No need to freeze/thaw kernel virtual memory (it's wired).
	 */
	recv_rdma_kmem_data( handle,
			     cur_addr,
			     cur_size,
			     kernel_map,
			     RECV_OOL_PORT_WAIT );

	/*
	 * port conversion handled in kmsg_convert.c
	 */
	DIPC_RX_STATS(ool_ports++);
	return KERN_SUCCESS;
}


/*
 * functions to be called when a specific message descriptor is encountered
 * during kmsg parsing.
 */
kmsg_parse_tbl_t	kmsg_recv_ptable = {
				0,			/* inline port(s) */
				recv_ool_port,		/* OOL port(s) */
				0,			/* inline data */
				recv_ool_data		/* OOL data */
			};

/*
 * functions to be called when a specific message descriptor is encountered
 * during kmsg error processing. 'kmsg' has had errors during processing, make
 * sure any previously allocated OOL data is deallocated.
 */
kmsg_parse_tbl_t	kmsg_error_ptable = {
				0,			/* inline port(s) */
				dealloc_ool_port,	/* OOL port(s) */
				0,			/* inline data */
				dealloc_ool_data	/* OOL data */
			};

/*
 * Process a complex message.
 *
 * inputs:
 *	kmsg		point to a valid kmsg
 *	rdma_handle	handle for RDMA data recv's
 *	parse_table	kmsg parser table pointer.
 *
 * outputs:
 *	KERN_SUCCESS, otherwise kern_return_t error from parsing routines.
 */

kern_return_t
process_complex_mesg_body( kmsg, rdma_handle, parse_table )
	ipc_kmsg_t	kmsg;
	rdma_handle_t	rdma_handle;
	kmsg_parse_tbl_t *parse_table;
{
	mach_msg_type_long_t	*mtype = (mach_msg_type_long_t *)(kmsg+1);
	vm_size_t		body_size;
	int			count;
	kern_return_t		func_kr = KERN_SUCCESS;

	rx_entry3(process_complex_mesg_body, kmsg, rdma_handle, parse_table );

	body_size = kmsg->ikm_header.msgh_size - sizeof(mach_msg_header_t);

	if ( (int)body_size <= 0 )
		panic("process_complex_mesg_body() zero length body?");

	count = -1;	/* process all types in the kmsg body */

	(void)norma_parse_kmsg( &count, mtype, &body_size, parse_table,
				&func_kr, (void*)rdma_handle);

	return func_kr;
}


/*
 * dequeue a kmsg off the send_queue, copying it to the supplied kmsg.
 * "kmsg's" are allocated out of wired kernel memory.
 *
 * Free send Q kmsg.
 *
 * implicit inputs:
 *	ONLY field set in the new kmsg is the kmsg->ikm_size, which was
 *	initialized from the meta-kmsg info. 
 *
 * inputs:
 *	kmsg	kmsg pointer.
 *	handle	RDMA handle
 *
 * outputs:
 *	kern_return_t's
 */

#define	RECV_KMSG_WAIT	150	/* microseconds */

kern_return_t
recv_kmsg( ipc_kmsg_t kmsg, unsigned net_size, rdma_handle_t handle )
{
	rx_entry3(recv_kmsg, kmsg, net_size, handle );

	/*
	 * get the kmsg data from the RDMA interface.
	 */
	recv_rdma_kmem_data( handle,
			     (vm_offset_t)kmsg,
			     net_size,
			     kernel_map,
			     RECV_KMSG_WAIT );

#if MACH_ASSERT
	/*
	 *  Verify what we were told about the kmsg size IS really true.
	 *
	 *  kmsg's from a network driver will have an ikm_size of 0
	 *  and that's OK.
	 */
	if ((kmsg->ikm_size == 0) &&
	    (kmsg->ikm_header.msgh_id == NET_RCV_MSG_ID) &&
	    (net_size >= sizeof(struct ipc_kmsg))) {
		;
	} else if (net_size > kmsg->ikm_size ||
		   net_size < sizeof(struct ipc_kmsg)) {
		rx_log6(0,
		"%s: ? size, kmsg %x ikm_size %d net-size %d, msgh sz %d\n",
			__FUNC__,
			kmsg,
			kmsg->ikm_size,
			net_size,
			kmsg->ikm_header.msgh_size);
		printf("recv_kmsg: ? size, kmsg->ikm_size %d, net_size %d\n",
			kmsg->ikm_size, net_size);
		assert(0);
	}
#endif

	return	KERN_SUCCESS;
}

/*
 * Given a meta-kmsg, receive the REAL kmsg with additional OOL data. Real
 * kmsg is received into a kernel kmsg, OOL data is received directly into the
 * receiver's address space. Mach message header and body are later copy from
 * kernel kmsg to user's receive buffer.
 *
 * inputs:
 *	mkm		meta-kmsg pointer
 *	free_meta_kmsg	do we or the caller release/delete the meta-kmsg?
 *
 * output:
 *	an "ipc_kmsg_t", IKM_NULL == Error or a valid kmsg pointer.
 */

ipc_kmsg_t
convert_meta_kmsg_2_kmsg(
			meta_kmsg_t	mkm,
			boolean_t	free_meta_kmsg )
{
	kern_return_t	kr;
	ipc_kmsg_t	kmsg;
	rdma_handle_t	rdma_handle;
	unsigned	kmsg_alloc_size;
	unsigned	net_size = mkm->mkm_size;

	/*
	 * allocate an rdma_handle based on the rdma 'token' in the meta kmsg.
	 * We can wait for a handle.
	 */
	rdma_handle = rdma_handle_alloc(current_thread()->dipc_rdma_rx_group,
				TRUE, -1);

	/*
	 * connect an rdma_handle to the endpoint identified by the rdma
	 * 'token' from the meta kmsg.
	 */
	rdma_connect(mkm->mkm_rdma_token, rdma_handle);

	/*
	 * alloc a real kmsg.
	 * adjust size to compensate for funny ikm_xxx macros. XXX
	 * get the meta_kmsg back in the free pool now that we're done with it.
	 */
	kmsg = ikm_alloc(ikm_less_overhead(net_size));
	ikm_init( kmsg, ikm_less_overhead(net_size));

	/*
	 * Remember the allocated size of the kmsg. The Kmsg comming over the
	 * wire may have a different (<=) value.
	 * When (message(header+body) < IKM_SAVED_KMSG_SIZE) then
	 * ikm_size == 256 while the actual size of the Mach message can be
	 * less. The meta-kmsg size reflects the true size of the
	 * Mach message-body+ikm_overhead.
	 */
	kmsg_alloc_size = kmsg->ikm_size;

	/*
	 * do we free the meta kmsg or will someone else?
	 */
	if ( free_meta_kmsg )
		(void)meta_kmsg_free( mkm );

	/*
	 * receive the kmsg.
	 */
	if ( (kr=recv_kmsg( kmsg, net_size, rdma_handle )) != KERN_SUCCESS ) {
		kmsg->ikm_size = kmsg_alloc_size;
		ikm_free( kmsg );
		rdma_flush( rdma_handle );
		rdma_disconnect( rdma_handle );
		rdma_handle_free( rdma_handle );
		return IKM_NULL;
	}

	/* mark kmsg as a local kmsg */
	kmsg->ikm_kmsg_type = IKM_KMSG_TYPE_LOCAL;

	/*
	 * restore the allocated size. The mach message contained within the
	 * kmsg envelop may be equal to or smaller than the kmsg itself.
	 * Believe the message header size!!
	 */
	kmsg->ikm_size = kmsg_alloc_size;

	/*
	 * Is this a complex message: possible OOL data/ports?
	 * If so, then walk the message body, receiving OOL data directly into
	 * the user's address space.
	 */
	if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX) {

		kr = process_complex_mesg_body( kmsg,
						rdma_handle,
						&kmsg_recv_ptable );
		if ( kr != KERN_SUCCESS ) {
			/*
			 * Free any previously allocated OOL data before
			 * freeing kmsg.
			 */
			(void) process_complex_mesg_body( kmsg,
						   rdma_handle,
						   &kmsg_error_ptable );
			ikm_free( kmsg );
			rdma_flush( rdma_handle );
			rdma_disconnect( rdma_handle );
			rdma_handle_free( rdma_handle );
			return IKM_NULL;
		}
		DIPC_RX_STATS(complex_messages++);
	} else {
		DIPC_RX_STATS(simple_messages++);
	}

	/*
	 * cleanup and we're outta here... 
	 */
	rdma_disconnect( rdma_handle );
	rdma_handle_free( rdma_handle );

	return kmsg;
}

void
rdma_recv_fault_intr( rdma_handle_t handle )
{
	rdma_fault_info_t       info;

	printf("rdma_recv_fault_intr on handle %d\n",handle);

	rdma_recv_fault_info( handle, &info );

	printf("  task 0x%x VA 0x%x map 0x%x pmap 0x%x\n",
		current_task(),
		info.addr,
		info.map,
		info.map->pmap );

	panic("rdma_recv_fault_intr()?");
}

void
db_dipc_recv_stats()
{
	register dipc_recv_stats_t	*rs=&dipc_recv_engine_stats;

	db_printf("Receive engine stats:\n");
	db_printf("  kmem_recv_posts        %8d",   rs->kmem_recv_posts);
	db_printf("  kmem_recv_blocks       %8d\n", rs->kmem_recv_blocks);

	db_printf("  ool_recv_posts         %8d",   rs->ool_recv_posts);
	db_printf("  ool_recv_blocks        %8d\n", rs->ool_recv_blocks);

	db_printf("  simple_messages        %8d",   rs->simple_messages);
	db_printf("  complex_messages       %8d\n", rs->complex_messages);

	db_printf("  ool_ports              %8d\n",   rs->ool_ports);
}
