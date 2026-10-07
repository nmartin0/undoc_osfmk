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
 *	$Id: meta_kmsg.c,v 1.3 1994/11/18 20:58:39 mtm Exp $
 */

#include <kern/assert.h>
#include <kern/lock.h>
#include <kern/kalloc.h>
#include <mach/machine/vm_types.h>
#include <mach/machine/kern_return.h>
#include <vm/vm_map.h>
#include <rpc_rdma/rdma.h>
#include <ipc/ipc_port.h>
#include <norma2/meta_kmsg.h>


decl_simple_lock_data(, meta_kmsg_free_list_lock)


meta_kmsg_t	meta_kmsg_free_list;
int		meta_kmsg_free_count;
int		meta_kmsg_free_wanted;


/*
 *	Allocate space for "max" meta_kmsgs.
 *
 *	Returns KERN_RESOURCE_SHORTAGE if it couldn't get the memory,
 *	otherwise returns KERN_SUCCESS.
 */
kern_return_t meta_kmsg_init(int max)
{
	meta_kmsg_t	mkm;
	vm_size_t	size;
	int		i;

	size = max * META_KMSG_SIZE;
	if ((mkm = (meta_kmsg_t) kalloc(size)) == 0)
		return KERN_RESOURCE_SHORTAGE;

	meta_kmsg_free_list = META_KMSG_NULL;
	meta_kmsg_free_count = 0;
	for (i = 0; i < max; i++) {
		mkm->mkm_next = meta_kmsg_free_list;
		meta_kmsg_free_list = mkm++;
		meta_kmsg_free_count++;
	}

	return KERN_SUCCESS;
}


/*
 *	Try to allocate a meta_kmsg of size "size" that will
 *	be connected to the RDMA endpoint specified by "rdma_token."
 *
 *	{Currently, the hints about size and endpoint are ignored.}
 *
 *	Returns either a meta_kmsg_t, or META_KMSG_NULL.
 */
meta_kmsg_t meta_kmsg_grab(rdma_token_t rdma_token, vm_size_t size)
{
	meta_kmsg_t	mkm;
	int		s;

	simple_lock(&meta_kmsg_free_list_lock);

	s = sploff();
	if ((mkm = meta_kmsg_free_list) != META_KMSG_NULL) {
		assert(meta_kmsg_free_count > 0);
		meta_kmsg_free_count--;
		meta_kmsg_free_list = mkm->mkm_next;
		mkm->mkm_rdma_token = rdma_token;
		mkm->mkm_size = size;
		mkm->mkm_kmsg_type = 1;
		mkm->mkm_dest_port = IP_NULL;
	}
	splon(s);

	simple_unlock(&meta_kmsg_free_list_lock);

	return mkm;
}


/*
 *	Block waiting for a meta_kmsg to become available.
 */
void meta_kmsg_wait()
{
	int		s;

	simple_lock(&meta_kmsg_free_list_lock);

	s = splsched();
	assert_wait((int) &meta_kmsg_free_wanted, FALSE);
	meta_kmsg_free_wanted++;
	splx(s);

	simple_unlock(&meta_kmsg_free_list_lock);

	thread_block(0);
}


/*
 *	Allocate a meta_kmsg.
 *
 *	If none are currently available, block the caller until
 *	one can be acquired.
 */
meta_kmsg_t meta_kmsg_alloc(
	rdma_token_t	rdma_token,
	vm_size_t	size,
	boolean_t	canwait)
{
	meta_kmsg_t	mkm;

	if (canwait == FALSE)
		return meta_kmsg_grab(rdma_token, size);

	while ((mkm = meta_kmsg_grab(rdma_token, size)) == META_KMSG_NULL)
		meta_kmsg_wait();

	return mkm;
}


/*
 *	Return a meta_kmsg to the pool of meta_kmsgs.
 *
 *	Awaken a thread if there is at least one waiting for
 *	a meta_kmsg.
 */
int meta_kmsg_free(meta_kmsg_t mkm)
{
	int		s, free;
	boolean_t	w;

	assert(mkm != 0);
	assert(mkm->mkm_kmsg_type == 1);

	simple_lock(&meta_kmsg_free_list_lock);

	s = sploff();
	mkm->mkm_next = meta_kmsg_free_list;
	meta_kmsg_free_list = mkm;
	free = meta_kmsg_free_count++;
	if ((w = (meta_kmsg_free_wanted > 0)))
		meta_kmsg_free_wanted--;
	splon(s);

	simple_unlock(&meta_kmsg_free_list_lock);

	if (w) thread_wakeup_one((int) &meta_kmsg_free_wanted);

	return free;
}
