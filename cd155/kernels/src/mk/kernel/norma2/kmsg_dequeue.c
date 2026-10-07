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
 * Meta/k message dequeue
 *
 * $Id: kmsg_dequeue.c,v 1.4 1995/02/28 01:33:08 sean Exp $
 *
 * HISTORY:
 */

#include <cpus.h>
#include <norma_ipc.h>
#include <mach_kdb.h>
#include <mach_assert.h>

#include <mach/mach_types.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/vm_param.h>

#include <kern/queue.h>
#include <kern/kalloc.h>
#include <kern/thread.h>

#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_space.h>

#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>

#include <norma2/kmsg_parser.h>
#include <norma2/norma_transport.h>
#include <norma2/dipc_uid.h>
#include <norma2/meta_kmsg.h>
#include <norma2/kmsg_convert.h>
#include <norma2/recv_engine.h>
#include <norma2/norma_log.h>
#include <norma2/kmsg_dequeue.h>

/*
 * dequeue a meta kmsg from the specified Mach message queue.  'kmsg_q' can
 * be null (IKMQ_NULL) in the case when a message arrives on a port with a
 * blocked receiver, the kmsg pointers are stored in the blocked thread's
 * structure 'ith_kmsg' and never actually enqueued on the port's message queue.
 *
 * implicit inputs:
 *	message queue is locked.
 *	meta-kmsg (be it meta or real kmsg) is at the head of the Mach message
 *	queue.
 *
 * inputs:
 *	meta	meta kmsg pointer
 *	kmsg_q	message queue pointer.
 *
 * Outputs:
 *	MACH_MSG_SUCCESS
 *	MACH_RCV_PORT_DIED
 *
 * Side Effects:
 *	*kmsg_p = IKM_NULL or a valid kmsg.
 */

mach_msg_return_t
dipc_kmsg_dequeue(
		meta_kmsg_t 		meta,
		ipc_kmsg_queue_t	kmsg_q,
		ipc_kmsg_t		*kmsg_p )
{
	kern_return_t		kr;
	ipc_kmsg_t		kmsg;
	unsigned		kmsg_type = meta->mkm_kmsg_type;

	dequeue_entry3(dipc_kmsg_dequeue, meta, kmsg_q, kmsg_p );

	/*
	 * process the message
	 */
	switch ( kmsg_type ) {

	  case IKM_KMSG_TYPE_LOCAL:
		kmsg = (ipc_kmsg_t)meta;

		printf(
			"%s: LOCAL KMSG? Kmsg sz %d msg sz %d\n",
			"dipc_kmsg_dequeue",
			kmsg->ikm_size,
			kmsg->ikm_header.msgh_size
		);
		dequeue_log4(
			0,
			"%s: LOCAL KMSG? Kmsg sz %d msg sz %d\n",
			"dipc_kmsg_dequeue",
			kmsg->ikm_size,
			kmsg->ikm_header.msgh_size
		);
		/*
		 * return the kmsg pointer, nothing else to do.
		 */
		*kmsg_p = kmsg;

		return MACH_MSG_SUCCESS;
		break;

	  case IKM_KMSG_TYPE_NET:
		/*
		 * This is really is a kmsg which was stuffed into an RPC
		 * payload bay.  Remove the network kmsg from the message
		 * queue to be absolutely sure no other thread can  mess with
		 * it while we are converting it to a local format kmsg.  The
		 * kmsg queue pointer can be <null> in the case where we were
		 * blocked waiting for kmsg arrival.  The enqueue thread has
		 * wakened us to receive the kmsg but does not actually
		 * enqueue the kmsg, but sets pointers in the blocked thread's
		 * state. 
		 */
		kmsg = (ipc_kmsg_t)meta;

		if ( kmsg_q != IKMQ_NULL )
			ipc_kmsg_rmqueue( kmsg_q, kmsg );

		assert(kmsg->ikm_header.msgh_remote_port != DIPC_UID_NULL );

		dequeue_log6(
			3,
			"%s: NET kmsg %x uid %x kmsg sz %d msg sz %d\n",
			"dipc_kmsg_dequeue",
			kmsg,
			kmsg->ikm_header.msgh_remote_port,
			kmsg->ikm_size,
			kmsg->ikm_header.msgh_size
		);

		break;

	  case IKM_KMSG_TYPE_META:
		/*
		 * a real live meta kmsg
		 */
		assert( meta->mkm_dest_port != IP_NULL );

		/*
		 * remove the meta-kmsg from the message queue.
		 * the kmsg queue pointer can be <null> in the case where we
		 * were blocked waiting for kmsg arrival. Enqueue thread has
		 * wakened us to receive the kmsg but does not actually enqueue
		 * the kmsg but sets pointers in the blocked thread's state.
		 */
		if ( kmsg_q != IKMQ_NULL )
			ipc_kmsg_rmqueue( kmsg_q, meta );

		/*
		 * Process the META Kmsg, convert it to a NET kmsg by receiving
		 * the NET kmsg from the remote node).  The NET kmsg will be
		 * converted from network format to local format in
		 * convert_net_kmsg_2_kmsg() once we exit this switch
		 */
		kmsg = convert_meta_kmsg_2_kmsg( meta, FALSE );

		if (kmsg == IKM_NULL)
			panic("dipc_kmsg_dequeue: Err converting meta_kmsg\n");

		/*
		 * release/free the meta-kmsg
		 */

		(void)meta_kmsg_free( meta );

		break;

	  default:
		printf(
			"%s() bad kmsg type (0x%x)\n",
			"dipc_kmsg_dequeue",
			meta->mkm_kmsg_type
		);
		panic("dipc_kmsg_dequeue() bad kmsg?");
		break;
	}

	/*
	 * Convert network format (ports == uids) to local IPC format
	 * (ports == ipc_port_t's).  Convert ports in the message header
	 * and body. 
	 */
	if ( (kr = convert_net_kmsg_2_kmsg( kmsg )) != KERN_SUCCESS ) {
		printf(
			"%s: invalid kmsg 0x%x, kr %d\n",
			"dipc_kmsg_dequeue",
			kmsg,
			kr
		);

		ipc_kmsg_destroy(kmsg);

		*kmsg_p = IKM_NULL;

		return MACH_RCV_PORT_DIED;
	}

	/*
	 * if this was a meta-kmsg insert the real kmsg into the front of the
	 * message queue.
	 */
	if ( kmsg_q != IKMQ_NULL ) {
		/*
		 * Insert a REAL kmsg at the HEAD of the message
		 * queue as that's what higher layers of Mach IPC
		 * expect "ipc_mqueue_receive()".
		 */
		ipc_kmsg_enqueue_first(kmsg_q, kmsg);

		/* XXX debug
		 *
		 * sanity check: kmsg should be queued at the head
		 * of the message queue.
		 */
		{
			ipc_kmsg_t	first;

			first = ipc_kmsg_queue_first(kmsg_q);
			assert( first == kmsg );
		}
	}

	/*
	 * return the kmsg pointer.
	 */
	*kmsg_p = kmsg;

	return MACH_MSG_SUCCESS;
}
