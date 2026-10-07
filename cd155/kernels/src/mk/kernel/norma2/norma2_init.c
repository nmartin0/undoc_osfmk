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
 * $Id: norma2_init.c,v 1.8 1995/04/07 17:28:45 andyp Exp $
 *
 * HISTORY:
 */

/*
 * initialize the NORMA2 IPC subsystem
 */

#include <cpus.h>
#include <mach_assert.h>
#include <mach_kdb.h>
#include <norma_ipc.h>

#include <mach/mach_types.h>
#include <mach/boolean.h>

#include <vm/vm_kern.h>
#include <vm/vm_map.h>

#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>
#include <norma2/meta_kmsg.h>
#include <norma2/norma_transport.h>
#include <norma2/dipc_uid.h>
#include <norma2/dipc_migrate.h>
#include <norma2/dipc_server.h>

vm_map_t	norma_map;
vm_map_t	dipc_kernel_recv_ool_map;
int		dipc_rdma_tx_group_size;

extern int	ice_cube_pages;

#if	MACH_ASSERT
extern int	trace_uid;
#endif

int		dipc_serialize;
int		dipc_fastpath_max;

/*
 * Initialization required before any threads run or RPC/RDMA initialization.
 */
void
dipc_early_init()
{
	extern	int	rpc_engine_payload_size;

	int	tmp_max;
	int	tmp_payload;

	/*
	 *  Determine the size of a DIPC payload.
	 */
	if ((tmp_max = getbootint("DIPC_FASTPATH_MAX", 96)) == 0)
		tmp_payload = sizeof(struct dipc_enqueue_request);
	else
		tmp_payload = sizeof(struct dipc_enqueue_request) +
			      sizeof(mach_msg_header_t) + tmp_max;

	/*
	 *  Make sure is will hold the largest DIPC RPC.
	 */
	if (tmp_payload < sizeof(dipc_request_union_t))
		tmp_payload = sizeof(dipc_request_union_t);

	/*
	 *  Make sure the payload is cache line aligned.
	 */
	tmp_payload = (tmp_payload + 31) & ~31;
	
	/*
	 *  Now give any extra created after alignment to the fastpath size.
	 *
	 *  N.B. Even though the bootmagic input is based on message BODY
	 *       size, we set dipc_fastpath_max to include the message header
	 *	 size for quicker compares when it's time to decide if a
	 *	 message can go fastpath or not.
	 */
	if (tmp_max)
		tmp_max = tmp_payload - sizeof(struct dipc_enqueue_request);

	dipc_fastpath_max       = tmp_max;
	rpc_engine_payload_size = tmp_payload;

	printf("Maximum RPC payload = %d   Maximum fastpath RPC = %d\n",
			rpc_engine_payload_size,
			dipc_fastpath_max == 0 ? 0 :
			dipc_fastpath_max - sizeof(mach_msg_header_t));
}

/*
 * NORMA2 initialization
 *
 * implicit inputs:
 *
 *	RPC and RDMA engines have been initialized, see kern/startup.c
 */

void
norma2_init()
{
	extern	vm_map_t		dipc_ool_map_create();
	extern	mach_msg_option_t	old_style_enqueue;
	extern	int	coalesce_recv;

	rdma_return_t	rc;
	int		meta_kmsg_max, tx, rx, emmi, pageout, txpriv, pagein;
	int             vnode_page_rx,vnode_page_tx;

	/*
	 *  See if we should serialize on enqueue
	 */
	dipc_serialize = getbootint("DIPC_SERIALIZE", 1);

	/*
	 *  See if they want old syle enqueuing behavior.
	 */
	if (getbootint("OLD_STYLE_ENQUEUE", 0))
		old_style_enqueue = MACH_SEND_ALWAYS;

	/*
	 *	See if Coalescing of objects on receives is wanted.
	 */
	coalesce_recv = getbootint("COALESCE_RECV", 0);

	/*
	 *  Initialize norma debugging.
	 */
	(void)norma_debug_init();

	/*
	 *	Create a 'pageable VM' NORMA transmit map.
	 *	The first logical page of address space is skipped
	 *	to avoid generating a VA of 0x00000000, primarily
	 *	for paranoid reasons.
	 */
	norma_map = dipc_ool_map_create();
	assert(norma_map != VM_MAP_NULL);

	/*
	 *	Create a map for receiving ool data destined
	 *	for dipc_kobj_server threads (rather than
	 *	using the kernel_map).
	 */
	dipc_kernel_recv_ool_map = dipc_ool_map_create();
	assert(dipc_kernel_recv_ool_map != VM_MAP_NULL);

	/*
	 * allocate RPC groups.
	 */
	rc = rpc_group_alloc( NORMA_RPC_GROUP_INITIATOR,
				NORMA_RPC_GROUP_INITIATOR_ALLOC);
	if ( rc != RPC_OK ) {
		panic("dipc rpc initiator group alloc failure");
	}

	rc = rpc_group_alloc( NORMA_RPC_GROUP_ENQUEUE,
				NORMA_RPC_GROUP_ENQUEUE_ALLOC);
	if ( rc != RPC_OK ) {
		panic("dipc rpc server enqueue group alloc failure");
	}

	rc = rpc_group_alloc( NORMA_RPC_GROUP_CONTROL,
				NORMA_RPC_GROUP_CONTROL_ALLOC);
	if ( rc != RPC_OK ) {
		panic("dipc rpc server control group alloc failure");
	}

	/*
	 * allocate RDMA groups.
	 */
	rx = getbootint("NORMA_RDMA_GROUP_RECV_ALLOC",
			 NORMA_RDMA_GROUP_RECV_ALLOC);

	tx = getbootint("NORMA_RDMA_GROUP_XMIT_ALLOC",
			 NORMA_RDMA_GROUP_XMIT_ALLOC);

	pageout = getbootint("NORMA_RDMA_GROUP_PAGEOUT_ALLOC",
			      NORMA_RDMA_GROUP_PAGEOUT_ALLOC);

	emmi = getbootint("NORMA_RDMA_GROUP_EMMI_PRIV_ALLOC",
			   NORMA_RDMA_GROUP_EMMI_PRIV_ALLOC);

	txpriv = getbootint("NORMA_RDMA_GROUP_XMIT_PRIV_ALLOC",
			     NORMA_RDMA_GROUP_XMIT_PRIV_ALLOC);

	pagein = getbootint("NORMA_RDMA_GROUP_PAGEIN_ALLOC",
			     NORMA_RDMA_GROUP_PAGEIN_ALLOC);

	vnode_page_rx = getbootint("NORMA_RDMA_GROUP_VNODE_PAGER_RX_ALLOC",
			     NORMA_RDMA_GROUP_VNODE_PAGER_RX_ALLOC);

	vnode_page_tx = getbootint("NORMA_RDMA_GROUP_VNODE_PAGER_TX_ALLOC",
			     NORMA_RDMA_GROUP_VNODE_PAGER_TX_ALLOC);

	assert(rx > 0);
	assert(tx > 0);
	assert(pageout > 0);
	assert(emmi > 0);
	assert(txpriv > 0);
	assert(pagein > 0);
	assert(vnode_page_rx > 0);
	assert(vnode_page_tx > 0);

	/*
	 *	Count the number of rdma handles that might need
	 *	slots in the kmsg disposal and faulting ring
	 *	buffers.  See tx_engine.c:dipc_tx_engine_init()
	 *	for the initialization of the 3 ring buffers.
	 *
	 *	The following rdma groups do not need slots in the
	 *	ring buffer because they are used for receives only
	 *	(which cannot fault and kmsg/rdma handle disposal for
	 *	a receive is entirely different from a send):
	 *
	 *		NORMA_RDMA_GROUP_RECV
	 *		NORMA_RDMA_GROUP_VNODE_PAGER_RX
	 *		NORMA_RDMA_GROUP_EMMI_REPLY
	 *
	 *	These rdma groups potentially require slots in the fault
	 *	and disposal ring buffers:
	 *
	 *		NORMA_RDMA_GROUP_PAGEOUT
	 *		NORMA_RDMA_GROUP_XMIT
	 *		NORMA_RDMA_GROUP_XMIT_PRIV
	 *		NORMA_RDMA_GROUP_XMIT_PAGEIN
	 *		NORMA_RDMA_GROUP_VNODE_PAGER_TX
	 */
	dipc_rdma_tx_group_size = pageout	/* pageout requests */
				+ tx		/* general sends */
				+ txpriv	/* m_o_d_write_completed */
				+ pagein	/* m_o_d_supply */
				+ vnode_page_tx;/* general sends, vnode pager */

	rc = rdma_group_alloc( NORMA_RDMA_GROUP_XMIT, tx );
	if ( rc != RDMA_SUCCESS ) {
		panic("dipc rdma xmit group alloc failure");
	}

	rc = rdma_group_alloc( NORMA_RDMA_GROUP_RECV, rx );
	if ( rc != RDMA_SUCCESS ) {
		panic("dipc rdma recv group alloc failure");
	}

	rc = rdma_group_alloc( NORMA_RDMA_GROUP_PAGEOUT, pageout );
	if ( rc != RDMA_SUCCESS ) {
		panic("dipc rdma pageout group alloc failure");
	}

	rc = rdma_group_alloc( NORMA_RDMA_GROUP_EMMI_REPLY, emmi);
	if ( rc != RDMA_SUCCESS ) {
		panic("dipc rdma emmi reply group alloc failure");
	}

	rc = rdma_group_alloc( NORMA_RDMA_GROUP_XMIT_PRIV, txpriv);
	if ( rc != RDMA_SUCCESS ) {
		panic("dipc rdma emmi tx priv group alloc failure");
	}

	rc = rdma_group_alloc( NORMA_RDMA_GROUP_XMIT_PAGEIN, pagein);
	if ( rc != RDMA_SUCCESS ) {
		panic("dipc rdma emmi tx pagein group alloc failure");
	}

	rc = rdma_group_alloc( NORMA_RDMA_GROUP_VNODE_PAGER_RX,vnode_page_rx);
	if ( rc != RDMA_SUCCESS ) {
		panic("dipc rdma vnode_page_rx group alloc failure");
	}

	rc = rdma_group_alloc( NORMA_RDMA_GROUP_VNODE_PAGER_TX,vnode_page_tx);
	if ( rc != RDMA_SUCCESS ) {
		panic("dipc rdma vnode_page_tx group alloc failure");
	}

	/*
	 * Initialize......
	 */

	/*
	 *	allocate meta-kmsgs
	 */
	meta_kmsg_max = getbootint("META_KMSG_MAX", 4096);
	if (meta_kmsg_init(meta_kmsg_max) != KERN_SUCCESS) {
		panic("norma2_init: meta_kmsg_init, meta_kmsg_max=%d",
			meta_kmsg_max);
	}

	/*
	 *	allocate fastpath kmsg cache
	 */
	dipc_kmsg_init(getbootint("FASTPATH_KMSG_MAX", 64));

	/*
	 *	number of pages to freeze on receive
	 */
	ice_cube_pages = getbootint("ICE_CUBE_PAGES", ice_cube_pages);
	if (ice_cube_pages <= 0) {
		panic("norma2_init: strangely sized ice cubes (%d)\n",
			ice_cube_pages);
	}

	/*
	 *	UID tables
	 */
	dipc_uid_init();

	/*
	 *	Receive Engine
	 */
	dipc_recv_engine_init();

	/*
	 * 	Transmit Engine
	 */
	dipc_tx_engine_init();

        /*
         * init/start enqeue service threads and control service threads.
         */
        dipc_enqueue_server_init(NORMA_RPC_GROUP_ENQUEUE_ALLOC);
        dipc_control_server_init(NORMA_RPC_GROUP_CONTROL_ALLOC);

        /*
         * 	DIPC (Distributed IPC) Kobj server
         */
        dipc_kobj_server_init();
}

norma_debug_init()
{
#if	MACH_ASSERT
	norma_log_init();
#endif
	return (0);
}


void dipc_thread_template_init(thread_t thread)
{
	thread->dipc_ool_tx_map = VM_MAP_NULL;
	thread->dipc_ool_rx_map = VM_MAP_NULL;
	thread->dipc_rdma_tx_group = NORMA_RDMA_GROUP_XMIT;
	thread->dipc_rdma_rx_group = NORMA_RDMA_GROUP_RECV;
}


int dipc_define_rdma_handles()
{
	int	h = 0;

	h += getbootint("NORMA_RDMA_GROUP_XMIT_ALLOC",
			 NORMA_RDMA_GROUP_XMIT_ALLOC);

	h += getbootint("NORMA_RDMA_GROUP_RECV_ALLOC",
			 NORMA_RDMA_GROUP_RECV_ALLOC);

	h += getbootint("NORMA_RDMA_GROUP_EMMI_PRIV_ALLOC",
			 NORMA_RDMA_GROUP_EMMI_PRIV_ALLOC);

	h += getbootint("NORMA_RDMA_GROUP_PAGEOUT_ALLOC",
			 NORMA_RDMA_GROUP_PAGEOUT_ALLOC);

	h += getbootint("NORMA_RDMA_GROUP_XMIT_PRIV_ALLOC",
			 NORMA_RDMA_GROUP_XMIT_PRIV_ALLOC);

	h += getbootint("NORMA_RDMA_GROUP_PAGEIN_ALLOC",
			 NORMA_RDMA_GROUP_PAGEIN_ALLOC);

	h += getbootint("NORMA_RDMA_GROUP_VNODE_PAGER_RX_ALLOC",
			 NORMA_RDMA_GROUP_VNODE_PAGER_RX_ALLOC);

	h += getbootint("NORMA_RDMA_GROUP_VNODE_PAGER_TX_ALLOC",
			 NORMA_RDMA_GROUP_VNODE_PAGER_TX_ALLOC);

	return h;
}


int dipc_define_rdma_groups()
{
	return NORMA_RDMA_TOTAL_GROUPS;
}

