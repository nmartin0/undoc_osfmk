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
 * $Id: dipc_mqueue.c,v 1.12 1995/03/31 18:00:02 andyp Exp $
 *
 * HISTORY:
 */

#include <mach_assert.h>

#include <mach/norma_special_ports.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <rpc_rdma/rdma.h>
#include <norma2/meta_kmsg.h>
#include <norma2/dipc_uid.h>
#include <norma2/norma_transport.h>
#include <norma2/tx_engine.h>
#include <norma2/dipc_transit.h>
#include <norma2/dipc_port.h>
#include <norma2/dipc_client.h>
#include <norma2/kmsg_parser.h>
#include <norma2/norma_log.h>
#include <norma2/dipc_mqueue.h>

dipc_mqueue_stats_t	dipc_mqueue_stats;

/*
 *  Name:	dipc_kmsg_destroy
 *
 *  Input:	Pointer to kmsg being destroyed
 *
 *  Output:	Resources associated with the kmsg are freed.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Depending on the kmsg type, resources associated
 *			with the kmsg and the kmsg itself are freed.
 */
void
dipc_kmsg_destroy(kmsg)
	ipc_kmsg_t	kmsg;
{
	mqueue_entry1(dipc_kmsg_destroy, kmsg);

	switch( kmsg->ikm_kmsg_type ) {

	  case IKM_KMSG_TYPE_LOCAL:
		ipc_kmsg_destroy(kmsg);
		break;

	  case IKM_KMSG_TYPE_NET:
		dipc_kmsg_dealloc(kmsg);
		break;

	  case IKM_KMSG_TYPE_META:
		rdma_flush_endpoint( ((meta_kmsg_t)kmsg)->mkm_rdma_token );
		(void)meta_kmsg_free( (meta_kmsg_t)kmsg );
		break;

	  default:
		panic("dipc_kmsg_destroy: unknown kmsg type %d\n",
			kmsg->ikm_kmsg_type);
	}
}

/*
 *  Name:	dipc_convert_kmsg_header
 *
 *  Input:	kmsg pointer
 *
 *  Output:	Destination and reply ports are converted to UID's and transits
 *		are allocated if needed.
 *
 *  Returns:		DIPC_SUCCESS if all went well.
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Describe function.
 *
 */
dipc_return_t
dipc_convert_kmsg_header(kmsg)
	ipc_kmsg_t	kmsg;
{
	ipc_port_t	port;
	dipc_uid_t	dest;
	dipc_uid_t	reply;
	mach_msg_bits_t	bits = kmsg->ikm_header.msgh_bits;

	mqueue_entry1(dipc_convert_kmsg_header, kmsg);

	assert(kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL);

	/*
	 *  Convert destination port.
	 */
	port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;

	assert(port != IP_NULL);
	assert(DIPC_IS_PROXY(port));
	assert(DIPC_UID_VALID(port));

	dest = port->dipc_uid;

	/*
	 * dipc_mqueue_send code will check if the dest port needs to be
	 * destroyed.
	 */ 
	mqueue_log5(3, "%s: dest %x uid %x hbits 0x%x\n",
			__FUNC__,
			port,
			port->dipc_uid,
			MACH_MSGH_BITS_REMOTE(bits));
	dipc_port_log(__FUNC__,"entry",port);

	if (MACH_MSGH_BITS_REMOTE(bits) == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		/*
		 *  Consume send-once right and it's port reference
		 */
		assert( port->ip_sorights > 0 );
		port->ip_sorights--; 
		ip_release(port);
		mqueue_log4(3,"    remote port 0x%x sorights %d refs %d\n",
				port,
				port->ip_sorights,
				port->ip_references);
	} else {
		/*
		 *  MACH_MSG_TYPE_PORT_SEND:  Consume a send right and it's
		 *  port reference.
		 */
		assert( port->ip_srights > 0 );
		port->ip_srights--;
		ip_release(port);
	}

	/*
	 *  Convert the reply port.
	 */
	if ( (port=(ipc_port_t)kmsg->ikm_header.msgh_local_port) != IP_NULL ) {
		mqueue_log6(3,
			"    repl %x hbits %x srights %d sorights %d refs %d\n",
			port,
			MACH_MSGH_BITS_LOCAL(bits),
			port->ip_srights,
			port->ip_sorights,
			port->ip_references);

		reply = dipc_send_port( port, ipc_object_copyin_type(
						MACH_MSGH_BITS_LOCAL(bits)),
					FALSE);
	}
	else
		reply = (dipc_uid_t)port;

	mqueue_log3(3, "    repl %x uid %x\n", port, reply);

	/*
	 *  Everything went well, commit the convertion.
	 */
	kmsg->ikm_header.msgh_remote_port = dest;
	kmsg->ikm_header.msgh_local_port  = reply;
	kmsg->ikm_kmsg_type = IKM_KMSG_TYPE_NET;

	mqueue_log4(3, "%s exit: dest uid %x msgh_id=%d\n",
			__FUNC__,
			kmsg->ikm_header.msgh_remote_port,
			kmsg->ikm_header.msgh_id);

	return (DIPC_SUCCESS);
}

/*
 *  Name:	dipc_reconstitute_header
 *
 *  Input:	Pointer to the kmsg and the destination port.
 *
 *  Output:	Undo the effects of converting a local kmsg header to a
 *		net kmsg header.
 *
 *  Returns:		DIPC_SUCCESS if all went well.
 *
 *  MP and locking
 *  consideration:	Destination port is unlocked.
 *
 *  Description:	Describe function.
 *
 */
dipc_reconstitute_header(kmsg, rem_port)
	ipc_kmsg_t	kmsg;
	ipc_port_t	rem_port;
{
	ipc_port_t	loc_port;
	mach_msg_bits_t	bits = kmsg->ikm_header.msgh_bits;

	mqueue_entry1(dipc_reconstitute_header, kmsg);

	assert(DIPC_IS_PROXY(rem_port));
	assert(kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_NET);

	/*
	 *  Because the port could have been declared dead and the
	 *  DIPC state desrtoyed through dead name notification, the
	 *  destination port is passed in and protected by an ip_reference
	 *  in dipc_mqueue_send().  This makes looking up the port with
	 *  the UID unnecessary.
	 */
	assert(rem_port->ip_references > 0);

	ip_lock(rem_port);

	if (MACH_MSGH_BITS_REMOTE(bits)==MACH_MSG_TYPE_PORT_SEND_ONCE) {
		/*
		 *  Give back send-once righ.
		 */
		rem_port->ip_sorights++; 
	} else {
		/*
		 *  MACH_MSG_TYPE_PORT_SEND:  Give back the send right.
		 */
		rem_port->ip_srights++;
	}

	/*
	 *  Give back the reference.
	 */
	ip_reference(rem_port);
	ip_unlock(rem_port);

	/*
	 *  Convert the reply port.
	 */
	loc_port = dipc_port_lookup(kmsg->ikm_header.msgh_local_port);
	if (loc_port != IP_NULL ) {
		ip_lock(loc_port);

		switch(ipc_object_copyin_type(MACH_MSGH_BITS_LOCAL(bits))) {

		case MACH_MSG_TYPE_PORT_SEND_ONCE:
			if (DIPC_IS_PROXY(loc_port)) {
				loc_port->ip_sorights++;
				ip_reference(loc_port);
			} else {
				/*
				 *  Leave ip_sorights and ip_references alone.
				 *  They will be taken care of in standard IPC
				 *  code. We only need to take back the remote
				 *  send right we gave in dipc_send_soright().
				 */
				loc_port->dipc_remote_sorights--;
			}

			break;

		case MACH_MSG_TYPE_PORT_SEND:
			break;

		default:
		      panic("dipc_reconstitute_header: Invalid name in header");
		}

		ip_unlock(loc_port);
	}

	/*
	 *  Everything went well, commit the convertion.
	 */
	kmsg->ikm_header.msgh_remote_port = (mach_port_t) rem_port;
	kmsg->ikm_header.msgh_local_port  = (mach_port_t) loc_port;
	kmsg->ikm_kmsg_type = IKM_KMSG_TYPE_LOCAL;

	return (DIPC_SUCCESS);
}

/*
 *  Name:	dipc_reconstitute_ool_data
 *
 *  Input:	Pointer to OOL data currently in the transmit map and length
 *		and of data.  The type and argument are unused.
 *
 *  Output:	vm_copy_t is recreated and OOL data in the xmit map deallocated.
 *
 *  Returns:	Return status from vm_map_copyin().
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Convert the virtural address in the NORMA map back
 *			into a vm_copy_t.
 */
/*ARGSUSED*/
kern_return_t
dipc_reconstitute_ool_data(type, length, vaddr, args)
	mach_msg_type_long_t	*type;			/* unused */
	vm_size_t		length;
	vm_offset_t		*vaddr;
	dipc_parse_args_t	*args;
{
	kern_return_t	kr;
	vm_map_copy_t	copy = VM_MAP_COPY_NULL;

	mqueue_entry4(dipc_reconstitute_ool_data ,type, length, *vaddr,args);
	
	/*
	 *  Don't waste time converting zero bytes.
	 */
	if (length == 0) {
		mqueue_log2(1, "%s: zero length OOL request\n", __FUNC__);
		return (KERN_SUCCESS);
	}

	/*
	 *  Convert transmit_map virtual address space back into a vm_copy_t and
	 *  deallocate the memory.
	 */
	kr = vm_map_copyin(args->map, *vaddr, length, TRUE, &copy);
	*vaddr = (vm_offset_t) copy;

	return (kr);
}

/*
 *  Name:	dipc_reconstitute_port_array
 *
 *  Input:	Pointer and number of UID's to convert.
 *
 *  Output:	All UID's are converted back to ipc_port_t's.
 *
 *  Returns:	KERN_SUCCESS
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Convert UID's back to ipc_port_t's.
 */
kern_return_t
dipc_reconstitute_port_array(uids, type, num)
	dipc_uid_t		*uids;
	mach_msg_type_long_t	*type;
	int			num;
{
	mach_msg_type_name_t	name;

	mqueue_entry3(dipc_reconstitute_port_array ,uids, type, num);

	if (type->msgtl_header.msgt_longform)
		name = type->msgtl_name;
	else
		name = type->msgtl_header.msgt_name;
	
	/*
	 *  Convert UID's back into ipc_port_t's.
	 */
	while (num-- > 0) {
		*uids = (dipc_uid_t) dipc_unsend_port(*uids, name, TRUE);
		uids++;
	}

	return (KERN_SUCCESS);
}

/*
 *  Name:	dipc_reconstitute_inline_ports
 *
 *  Input:	Pointer to UID's to convert back into ipc_port_t's and length
 *		of UID array in bytes.  The type and args argument are unused
 *		in the function.
 *
 *  Output:	All UID's are converted back into ipc_port_t's.
 *
 *  Returns:	Return status from dipc_reconstitute_port_array()
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Convert in-line UID's to ipc_port_t's.
 */
/*ARGSUSED*/
kern_return_t
dipc_reconstitute_inline_ports(type, length, uids, args)
	mach_msg_type_long_t	*type;
	vm_size_t		length;
	dipc_uid_t		*uids;
	dipc_parse_args_t	*args;			/* unused */
{
	mqueue_entry4(dipc_reconstitute_inline_ports ,type,length,uids,args);
	return dipc_reconstitute_port_array( uids, type,
					     length / sizeof(dipc_uid_t));
}

/*
 *  Name:	dipc_reconstitute_ool_ports
 *
 *  Input:	Pointer to OOL-pointer to UID's to convert back into
 *		ipc_port_t's and length of UID array in bytes.  The type
 *		and args argument are unused in the function.
 *
 *  Output:	All UID's are converted back into ipc_port_t's.
 *
 *  Returns:	Return status from dipc_reconstitute_port_array()
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:	Convert ool UID's to ipc_port_t's.
 */
/*ARGSUSED*/
kern_return_t
dipc_reconstitute_ool_ports(type, length, uids, args)
	mach_msg_type_long_t	*type;
	vm_size_t		length;
	dipc_uid_t		**uids;
	dipc_parse_args_t	*args;			/* unused */
{
	mqueue_entry4(dipc_reconstitute_ool_ports ,type, length, uids, args);
	return dipc_reconstitute_port_array( *uids, type,
					     length / sizeof(dipc_uid_t));
}

/*
 *  Name:	dipc_reconstitute_kmsg
 *
 *  Input:	Pointer to kmsg and proxy port.
 *
 *  Output:	UIDs and OOL pointers are reconstitued back to ipc_port_t's
 *		and vm_copy_t's.
 *
 *  Returns:		Void
 *
 *  MP and locking
 *  consideration:	Describe locking and MP considerations.
 *
 *  Description:	This routine is called when an enqueue has vaild.
 *			Before returning, the kmsg must be converted back
 *			to local format.
 */
kmsg_parse_tbl_t dipc_reconstitute_tbl = {
	dipc_reconstitute_inline_ports,		/* inline port */
	dipc_reconstitute_ool_ports,		/* ool port    */
	0,					/* inline data */
	dipc_reconstitute_ool_data,		/* ool data    */
};

void
dipc_reconstitute_kmsg(kmsg, port)
	ipc_kmsg_t	kmsg;
	ipc_port_t	port;
{
	mach_msg_type_long_t	*type = (mach_msg_type_long_t*)(kmsg+1);
	int			count = -1;
	vm_size_t		size = DIPC_MSG_BODY_SIZE(kmsg);
	kern_return_t		kr;

	mqueue_entry1(dipc_reconstitute_kmsg, kmsg);

	MQUEUE_STATS(unsend++);

	/*
	 *  Reconstitute the header.
	 */
	dipc_reconstitute_header(kmsg, port);

	/*
	 *  Reconstitute the body if it contains complex types.
	 */
	if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX) {
		dipc_parse_args_t	pargs;

		assert( kmsg->ikm_dipc_map != VM_MAP_NULL );
		pargs.map = kmsg->ikm_dipc_map;	/* for OOL data */
		(void) norma_parse_kmsg(&count,
					type,
					&size,
					&dipc_reconstitute_tbl,
					&kr,
					&pargs);
	}
}

/*
 *  Name:	dipc_mqueue_send
 *
 *  Input:	Pointer to proxy port, kmsg, options, and timeout value.
 *
 *  Output:	Item represenging the kmsg (meta-kmsg) or actual kmsg enqueued
 *		on remote port if port is valid.  Kmsg body and OOL data and
 *		ports posted to RDMA engine.
 *
 *  Returns:		MACH_MSG_SUCCESS if all goes well - kmsg destroyed.
 *			MACH_SEND_TIMED_OUT if we waited for `timeout' seconds
 *			on a full queue - kmsg NOT destroyed.
 *			MACH_SEND_INTERRUPTED if enqueue was interrupted - kmsg
 *			NOT destroyed.
 *  MP and locking
 *  consideration:	Describe locking and MP considerations.
 *
 *  Description:	All remote enqueues pass throught here.  This includes
 *			messages being migrated from a port.  For this reason,
 *			all three possible kmsg types (local, net, meta) are
 *			handled.
 *
 */

#if MACH_ASSERT
#define	ENQUEUE_ACTIVE_CHECK(p, r)					\
	if (!ip_active(p))						\
		mqueue_log4(1, "%s: enqueued on dead proxy %x kr=%d\n", \
				__FUNC__, port, kr);
#else
#define ENQUEUE_ACTIVE_CHECK(p, r)
#endif /* MACH_ASSEERT */

mach_msg_option_t	old_style_enqueue = 0;

#define	DIPC_MOD_OPTS_SEND_ALWAYS(k, opt) \
	((MACH_MSGH_BITS_REMOTE((k)->ikm_header.msgh_bits) == \
		MACH_MSG_TYPE_PORT_SEND_ONCE) ? (opt | MACH_SEND_ALWAYS) : \
						(opt | old_style_enqueue))

#define	DIPC_MOD_OPTS_ENQUEUE_EMMI(k, opt)	(msg_emmi_val((k)) | (opt))

#define	DIPC_MOD_OPTS(k, opt) \
	(DIPC_MOD_OPTS_SEND_ALWAYS(k, opt) | \
	 DIPC_MOD_OPTS_ENQUEUE_EMMI(k, opt))


kern_return_t
dipc_mqueue_send(port, kmsg, option, timeout)
	ipc_port_t		port;
	ipc_kmsg_t		kmsg;
	mach_msg_option_t	option;
	mach_msg_timeout_t	timeout;
{
	rdma_handle_t		rdma_handle;
	rdma_group_t		rdma_group;
	rdma_token_t		token;
	dipc_enqueue_id_t	enq_id;
	kern_return_t		kr;
	boolean_t		enqueued;
	boolean_t		send_kmsg;
	boolean_t		free_meta_kmsg;
	mach_msg_option_t	send_options;

	mqueue_entry4(dipc_mqueue_send, port, kmsg, option, timeout);

	assert(IP_VALID(port));
	assert(port->ip_receiver != ipc_space_kernel);
	assert(DIPC_IS_PROXY(port));
	assert(ip_active(port));
	assert( port->dipc_node != node_self() );

	ip_reference(port);	/* make sure the port does not go away on us */

	dipc_port_log(__FUNC__,"entry",port);
	mqueue_log5(3, "%s: port %x uid %x send-options 0x%x\n",
			__FUNC__,
			port,
			port->dipc_uid,
			option
	);

	switch (kmsg->ikm_kmsg_type) {

	/*
	 *  Local kmsg needs to have the header converted.
	 */
	case IKM_KMSG_TYPE_LOCAL:
		MQUEUE_STATS(enqueue_local++);
		if (dipc_convert_kmsg_header(kmsg) != DIPC_SUCCESS) {
			/*
			 *  Free kmsg and all associated resources
			 *  and treat this like an invalid port.
			 */
			printf(
		"dipc_mqueue_send: dipc_convert_kmsg_header() !DIPC_SUCCESS\n");
			mqueue_log2(0,
			       "%s: dipc_convert_kmsg_header() !DIPC_SUCCESS\n",
				__FUNC__);

			dipc_kmsg_destroy(kmsg);
			ip_release(port);
			return (MACH_MSG_SUCCESS);
		}
		free_meta_kmsg = FALSE;
		send_kmsg = TRUE;
		send_options = DIPC_MOD_OPTS(kmsg, option);
		break;

	/*
	 *  Meta_kmsg's already have an rdma_token.
	 */
	case IKM_KMSG_TYPE_META:
		MQUEUE_STATS(enqueue_meta++);
		mqueue_log2(3, "%s: enqueuing meta_kmsg\n", __FUNC__);
		token = ((meta_kmsg_t)kmsg)->mkm_rdma_token;
		free_meta_kmsg = TRUE;
		send_kmsg = FALSE;
		send_options = option;
		break;

	/*
	 *  Net_kmsg - nothing to do
	 */
	case IKM_KMSG_TYPE_NET:
		MQUEUE_STATS(enqueue_net++);
		mqueue_log2(3, "%s: enqueuing net_kmsg\n", __FUNC__);
		free_meta_kmsg = FALSE;
		send_kmsg = TRUE;
		send_options = DIPC_MOD_OPTS(kmsg, option);
		break;
	
	default:
		panic("dipc_mqueue_send: Unknown kmsg type - 0x%x\n",
							kmsg->ikm_kmsg_type);
	}

	/*
	 *  Get an RDMA handle and token to tranfer the kmsg and OOL data with.
	 */
	if (kmsg->ikm_kmsg_type != IKM_KMSG_TYPE_META) {
		if ((send_options & DIPC_EMMI_REPLY_PRIV) != 0) {
			rdma_group = NORMA_RDMA_GROUP_XMIT_PRIV;
			MQUEUE_STATS(send_priv++);
		} else if ((send_options & DIPC_EMMI_REPLY) != 0) {
			rdma_group = NORMA_RDMA_GROUP_XMIT_PAGEIN;
			MQUEUE_STATS(send_pagein++);
		} else {
			rdma_group = current_thread()->dipc_rdma_tx_group;
		}
		rdma_handle = rdma_handle_alloc( rdma_group, TRUE, 0);
		if (rdma_handle == RDMA_GROUP_EMPTY)
			panic("rdma_handle_alloc() XMIT group?");
		token = rdma_token(rdma_handle);
	}

	/*
	 *  Loop until the kmsg/meta_kmsg is enqueued or an error is returned.
	 *  We'll keep the port locked until we have a resolution.
	 *
	 *  XXX  Is keeping the port locked for the enqueue opertion
	 *  XXX  a good thing or even necessary???
	 */
	ip_lock(port);
	enqueued = FALSE;

	while ( ! enqueued ) {
		/*
		 *  Send the RPC_ENQUEUE_REQ now and overlap with data
		 *  conversion and RDMA posts.
		 */
		enq_id = dipc_kmsg_enqueue_request(
				port,
				kmsg,
				send_options,
				token);
		/*
		 *  Convert local kmsgs to network format and post initial
		 *  RDMA requests.
		 */
		if (send_kmsg) {
			/*
			 *	IMPORTANT SAFETY TIP:
			 *
			 *	Once dipc_send_kmsg() has been called,
			 *	it is an error to dereference fields
			 *	within the kmsg or net-kmsg structure
			 *	(it may have already been sent and
			 *	deallocated).
			 */
			dipc_send_kmsg(kmsg, rdma_handle);
			send_kmsg = FALSE;	/* only send it once */
		}

		/*
		 *  Collect the RPC_ENQUEUE reply
		 */
		switch (dipc_kmsg_enqueue_await(enq_id, port)) {

		/*
		 *  Meta_kmsg was enqued.  Transmit engine will send
		 *  and destroy kmsg and associated resources.
		 */
		case DIPC_QUEUE_MKMSG:
			if (free_meta_kmsg)
				(void)meta_kmsg_free((meta_kmsg_t)kmsg);

			enqueued = TRUE;
			kr = MACH_MSG_SUCCESS;
			break;

		/*
		 *  We took the optimized path that sent the kmsg in
		 *  the RPC request.  Use the rdma_undo() call to
		 *  flush the endpoint without incurring a callback
		 *  and return the rdma handle.  Destroy the enqueued
		 *  kmsg.
		 */
		case DIPC_QUEUE_KMSG:
			assert(kmsg->ikm_kmsg_type != IKM_KMSG_TYPE_META);
			MQUEUE_STATS(payload_kmsg++);
			mqueue_log2(3, "%s: payload bay kmsg\n", __FUNC__);
			enqueued = TRUE;
			rdma_undo(rdma_handle);
			dipc_kmsg_destroy(kmsg);
			kr = MACH_MSG_SUCCESS;
			break;
		/*
		 *  Remote queue is full.  Block waiting for the
		 *  queue to open.
		 */
		case DIPC_QUEUE_FULL: {
			ipc_thread_t self = current_thread();

			MQUEUE_STATS(queue_full++);
			mqueue_log2(1, "%s: DIPC_QUEUE_FULL\n", __FUNC__);

			/*
			 *  Setup for timeouts.
			 */
			if (option & MACH_SEND_TIMEOUT) {
				if (timeout == 0) {
					kr = MACH_SEND_TIMED_OUT;
					enqueued = TRUE;
					break;
				}
				thread_will_wait_with_timeout(self, timeout);
			} else {
				thread_will_wait(self);
			}

			/*
			 *  Put our thread on the blocked senders queue.
			 */
			ipc_thread_enqueue(&port->ip_blocked, self);
			self->ith_state = MACH_SEND_IN_PROGRESS;

			/*
			 *  Unlock the port and wait
			 */
			ip_unlock(port);
			thread_block((void (*)()) 0);
			ip_lock(port);

			/*
			 *  Check reason for wakeup
			 */
			if (self->ith_state != MACH_MSG_SUCCESS) {
				assert(self->ith_state==MACH_SEND_IN_PROGRESS);

				/*
				 *  Take ourselves off the blocked senders
				 *  queue and deal with the reason for the
				 *  interruption.
				 */
				ipc_thread_rmqueue(&port->ip_blocked, self);
				switch (self->ith_wait_result) {

				/*
				 *  Interrupted send.
				 */
				case THREAD_INTERRUPTED:
					kr = MACH_SEND_INTERRUPTED;
					break;
				/*
				 *  Timeout
				 */
				case THREAD_TIMED_OUT:
					assert(option & MACH_SEND_TIMEOUT);
					kr = MACH_SEND_TIMED_OUT;
					break;
				
				case THREAD_RESTART:
				default:
					assert(0);
					break;
				}
			}
			break;
		}

		/*
		 *  Remote port was bogus or dead.  Break out and return error
		 *  or MACH_MSG_SUCCESS if this came from a kernel thread.
		 */
		case DIPC_QUEUE_INVALID:
		case DIPC_QUEUE_DEAD:
			mqueue_log4(0, "%s: port 0x%x uid 0x%x INVALID/DEAD\n",
					__FUNC__,
					port,
					port->dipc_uid);
			enqueued = TRUE;

			/*
			 *  Kernel threads always send with MACH_SEND_ALWAYS.
			 *  They assume enqueues never fail so don't disappoint
			 *  them.  This is OK because it simulates the same
			 *  behaviour as if the port went away after making
			 *  a successful enqueue.
			 */
			if (option & MACH_SEND_ALWAYS) {
				kr = MACH_MSG_SUCCESS;
				mqueue_log3(0,
				   "%s: Forcing MACH_MSG_SUCCESS for UID %x\n",
					__FUNC__,
					port->dipc_uid);
				rdma_undo(rdma_handle);
				dipc_kmsg_destroy(kmsg);
			} else {
				kr = MACH_SEND_INVALID_DEST;
			}
			break;

		/*
		 *  The response was queue full, but queue-not-full
		 *  notification arrived and was processed before
		 *  the thread could block.  Just try again.
		 */
		case DIPC_QUEUE_WAS_FULL:
			MQUEUE_STATS(queue_was_full++);
			mqueue_log2(0, "%s: DIPC_QUEUE_WAS_FULL\n", __FUNC__);
			break;

		/*
		 * Something unexpected was returned.
		 */
		default:
			panic("bad return dipc_collect_enqueue_reply()");
		}
	}

	ENQUEUE_ACTIVE_CHECK(port, kr);

	ip_unlock(port);

	/*
	 *  If the return code != MACH_MSG_SUCCESS, the kmsg must be
	 *  converted back to local format.  We need to flush and
	 *  return the RDMA handle without causing a callback.
	 */
	if (kr != MACH_MSG_SUCCESS) {
		rdma_undo(rdma_handle);
		dipc_reconstitute_kmsg(kmsg, port);
	}

	/*
	 *  Release our protective reference on the port.  If the port has
	 *  died as a result of this send, we need to release the port too.
	 */
	if (ip_active(port)){
		ip_release(port);
		dipc_port_remove_try(port);
	} else {
		ipc_port_release(port);
	}

	return (kr);
}

/*
 *	Routine:	dipc_mqueue_move
 *	Purpose:
 *		Move messages from one queue (source) to another (dest).
 *		Only moves messages sent to the specified port.
 *		Can handle meta_kmsgs as well as kmsgs and
 *		supercedes ipc/ipc_mqueue.c:ipc_mqueue_move().
 *	Conditions:
 *		Both queues must be locked.
 *		(This is sufficient to manipulate port->ip_seqno.)
 */

void
dipc_mqueue_move(dest, source, port)
	ipc_mqueue_t dest;
	ipc_mqueue_t source;
	ipc_port_t port;
{
	ipc_kmsg_queue_t oldq, newq;
	ipc_thread_queue_t blockedq;
	ipc_kmsg_t kmsg, next;
	ipc_thread_t th;
	ipc_port_t remote;
	mach_msg_size_t size;

	mqueue_entry3(dipc_mqueue_move, dest, source, port);

	oldq = &source->imq_messages;
	newq = &dest->imq_messages;
	blockedq = &dest->imq_threads;

	for (kmsg = ipc_kmsg_queue_first(oldq);
	     kmsg != IKM_NULL; kmsg = next) {
		next = ipc_kmsg_queue_next(oldq, kmsg);

		/* only move messages sent to port */
		switch( kmsg->ikm_kmsg_type ) {
		case IKM_KMSG_TYPE_LOCAL:
			remote = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
			size = kmsg->ikm_header.msgh_size;
			break;

		case IKM_KMSG_TYPE_META:
			remote = ((meta_kmsg_t) kmsg)->mkm_dest_port;
			size = ((meta_kmsg_t) kmsg)->mkm_size
					- sizeof(struct ipc_kmsg)
					+ sizeof(mach_msg_header_t);
			break;

		case IKM_KMSG_TYPE_NET:
			remote = dipc_port_lookup((dipc_uid_t)
					kmsg->ikm_header.msgh_remote_port);
			size = kmsg->ikm_header.msgh_size;
			assert(remote != IP_NULL);
			break;

		default:
			panic("dipc_mqueue_move: unknown kmsg type %d",
				kmsg->ikm_kmsg_type);
			remote = IP_NULL;
			size = 0;
			break;
		}

		if (remote != port)
			continue;

		ipc_kmsg_rmqueue(oldq, kmsg);

		/* before adding kmsg to newq, check for a blocked receiver */

		while ((th = ipc_thread_dequeue(blockedq)) != ITH_NULL) {
			assert(ipc_kmsg_queue_empty(newq));

			thread_go(th);

			/* check if the receiver can handle the message */

			if (size <= th->ith_msize) {
				th->ith_state = MACH_MSG_SUCCESS;
				th->ith_kmsg = kmsg;
				th->ith_seqno = port->ip_seqno++;

				goto next_kmsg;
			}

			th->ith_state = MACH_RCV_TOO_LARGE;
			th->ith_msize = size;
		}

		/* didn't find a receiver to handle the message */

		ipc_kmsg_enqueue(newq, kmsg);
	    next_kmsg:;
	}
}

db_mqueue_stats()
{
	dipc_mqueue_stats_t	*p = &dipc_mqueue_stats;

        db_printf("dipc_mqueue stats:\n");
	db_printf("  enqueue_local          %8d",   p->enqueue_local);
	db_printf("  enqueue_meta           %8d\n", p->enqueue_meta);
	db_printf("  enqueue_net            %8d",   p->enqueue_net);
	db_printf("  payload_kmsg           %8d\n", p->payload_kmsg);
	db_printf("  queue_full             %8d",   p->queue_full);
	db_printf("  queue_was_full         %8d\n", p->queue_was_full);
	db_printf("  unsend                 %8d",   p->unsend);
	db_printf("  send_priv              %8d\n", p->send_priv);
	db_printf("  send_pagein            %8d\n", p->send_pagein);

	return 0;
}
