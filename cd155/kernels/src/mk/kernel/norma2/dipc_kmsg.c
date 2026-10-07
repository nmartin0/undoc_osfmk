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
 *	$Id: dipc_kmsg.c,v 1.3 1994/11/18 20:57:45 mtm Exp $
 */

#include <kern/assert.h>
#include <kern/lock.h>
#include <kern/kalloc.h>
#include <ipc/ipc_kmsg.h>
/*
#include <mach/machine/vm_types.h>
#include <mach/machine/kern_return.h>
#include <vm/vm_map.h>
#include <rpc_rdma/rdma.h>
#include <ipc/ipc_port.h>
#include <norma2/meta_kmsg.h>
*/

extern	int	dipc_fastpath_max;

decl_simple_lock_data(, dipc_kmsg_free_list_lock)

ipc_kmsg_t	dipc_kmsg_free_list;
int		dipc_kmsg_free_count;
int		dipc_kmsg_cache_size;

/*
 *	Allocate The initial fastpath kmsg cache
 *
 *	N.B.  This routine must be called after dipc_fastpath_max has been
 *	      initialized.  
 */
void dipc_kmsg_init(int max)
{
	ipc_kmsg_t	kmsg;
	vm_size_t	size;
	int		i;

	size = dipc_fastpath_max;
	dipc_kmsg_free_list = IKM_NULL;
	dipc_kmsg_free_count = 0;

	simple_lock_init(&dipc_kmsg_free_list_lock);

	for (i = 0; i < max; i++) {
		/*
		 *  N.B. ikm_alloc() will add kmsg overhead to passed size.
		 */
		if (kmsg = ikm_alloc(size)) {
			kmsg->ikm_size = ikm_plus_overhead(size);
			kmsg->ikm_next = dipc_kmsg_free_list;

			dipc_kmsg_free_list = kmsg;
			dipc_kmsg_free_count++;
		} else {
			printf("dipc_kmsg_init: ikm_alloc failed (%d / %d)\n",
								i, max);
			break;
		}
	}

	dipc_kmsg_cache_size = dipc_kmsg_free_count;
	return;
}

/*
 *	Try to allocate a fastpath DIPC kmsg.
 *	Returns either a ipc_kmsg_t, or IKM_NULL.
 */
ipc_kmsg_t dipc_kmsg_grab(vm_size_t size)
{
	ipc_kmsg_t	kmsg;
	int		s;

	assert(size <= ikm_plus_overhead(dipc_fastpath_max));

	simple_lock(&dipc_kmsg_free_list_lock);

	s = sploff();
	if ((kmsg = dipc_kmsg_free_list) != IKM_NULL) {
		assert(dipc_kmsg_free_count > 0);
		assert(kmsg->ikm_size == ikm_plus_overhead(dipc_fastpath_max));

		kmsg->ikm_kmsg_type = IKM_KMSG_TYPE_NET;
		kmsg->ikm_marequest = IMAR_NULL;
		kmsg->ikm_transfer_index = 0;
		kmsg->ikm_dipc_map = VM_MAP_NULL;

		dipc_kmsg_free_list = kmsg->ikm_next;
		dipc_kmsg_free_count--;

	}
	splon(s);

	simple_unlock(&dipc_kmsg_free_list_lock);

	return kmsg;
}

/*
 *	Attempt to keep fastpath kmsg cache full.
 *
 */
void dipc_kmsg_put(ipc_kmsg_t kmsg)
{
	int		s, free;
	boolean_t	w;

#define	IKM_SIZE_DIPC	(ikm_plus_overhead(dipc_fastpath_max))
	assert(kmsg != 0);
	assert(kmsg->ikm_size == IKM_SIZE_DIPC);
	assert(dipc_kmsg_free_count <= dipc_kmsg_cache_size);

	simple_lock(&dipc_kmsg_free_list_lock);

	s = sploff();

	if (dipc_kmsg_free_count < dipc_kmsg_cache_size) {
		kmsg->ikm_next = dipc_kmsg_free_list;
		dipc_kmsg_free_list = kmsg;
		dipc_kmsg_free_count++;
	} else {
		kfree((vm_offset_t) kmsg, kmsg->ikm_size);
	}

	splon(s);

	simple_unlock(&meta_kmsg_free_list_lock);

	return;
}
