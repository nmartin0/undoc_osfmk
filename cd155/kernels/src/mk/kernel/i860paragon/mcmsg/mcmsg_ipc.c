/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
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
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_ipc.c,v 1.11 1995/03/21 21:02:52 lenb Exp $
 */

/*
 * mcmsg_ipc.c
 *
 * Mach IPC interface
 */

#define	MCMSG_MODULE	MCMSG_MODULE_IPC

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <mach_assert.h>
#include <i860paragon/mcmsg/mcmsg_ipc.h>

#ifdef DYNAMIC_NUM_NODES
struct ipc_sender_iterm *alloc_ipc_rts_table();
#endif DYNAMIC_NUM_NODES

/*
 *	Initialization for NORMA IPC to mcmsg interfaces.
 */

netipc_network_init()
{

	ipcreq_bootmagic_init();
	mcmsg_mp_init();
	ipcreq_receiving = 0;
	ipcreq_recv_count = 0;
	ipcreq_sending = 0;
	ipcreq_send_count = 0;
	ipcreq_recv_node = -1;
	ipcreq_rtsqueue_init();
#if	IPC_COPY_SEND
	ipcreq_copy_send_init();
#endif	IPC_COPY_SEND
}

/*
 *	Override compile-time settings with bootmagic.
 */

ipcreq_bootmagic_init()
{

	/*
	 *	packet size?
	 */

	ipcreq_packet_size = getbootint("IPC_PACKET_SIZE", 1728);

	/*
	 *	copy what NORMA wants transmitted to a private buffer
	 *	and then transmit the copy?
	 */

	ipcreq_copy_send_enabled = getbootint("IPC_COPY_SEND", 0);

	/*
	 *	use the 3-trip protocol?  (probably an overall lose
	 *	on small systems, perhaps a small win on large systems)
	 */

	ipcreq_rts_cts = getbootint("IPC_RTS_CTS", 1);

#if	IPC_CSUM

	/*
	 *	ignore bad checksums?
	 */

	ipcreq_ignore_bad_checksums = getbootint("IPC_IGNORE_BAD_CHECKSUMS", 0);

	/*
	 *	fail an assertion if there is a bad checksum?
	 */

	ipcreq_stop_on_csum_failure = getbootint("IPC_CSUM_ASSERTS", 1);
#endif	IPC_CSUM

}

ipcreq_rtsqueue_init()
{
	struct ipc_sender_item	*rts;
	int	i,table_size;

	/*
	 *	initialize the free list
	 */
	ipc_rts_free = 0;
#ifdef DYNAMIC_NUM_NODES
        table_size=(sizeof(struct ipc_sender_item) * max_rts_reqs);
        rts=ipc_rts_cts_table
          =(struct ipc_sender_item *)alloc_ipc_rts_table(table_size);
#else DYNAMIC_NUM_NODES
	rts = &ipc_rts_cts_table[0];
#endif DYNAMIC_NUM_NODES
	for (i = 0; i < MAX_RTS_REQS; i++) {
		rts->ipc_next = ipc_rts_free;
		ipc_rts_free = rts++;
		ipc_rts_free_count++;
	}
}


#if	IPC_COPY_SEND
/*
 *	copy NORMA's data out of the way before
 *	we transmit and transmit the secret copy.
 *
 *	XXX ONLY CORRECT FOR 4K AND 8K VM PAGE SIZES
 */
#define	MAX_PAGE_SIZE	8192
#define	MAX_ADDITIONAL	sizeof(struct netipc_hdr)
#define	MAX_INSURANCE	128				/* LTU alignment */
#define	MAX_IPC_SEND	(MAX_PAGE_SIZE + MAX_ADDITIONAL + MAX_INSURANCE)

static char ipcreq_copy_send_buffer_space[MAX_IPC_SEND + INTEL_PGBYTES];
static char *ipcreq_copy_send_buffer;

#define	ALIGN32(p) (((unsigned long) (p) + 32) & ~(32 - 1))
#define	ALIGNPG(p) (((unsigned long) (p) + INTEL_PGBYTES) & ~(INTEL_PGBYTES - 1))

ipcreq_copy_send_init()
{

	ipcreq_copy_send_buffer = (char *)
				  ALIGNPG(ipcreq_copy_send_buffer_space);
}

/*
 *	this routine has an interesting side effect in that it
 *	re-packetizes the original request, and LTU-favorable
 *	alignment falls out for free...
 */

ipcreq_copy_send(ipcreq)
	register ipcreq_t	*ipcreq;
{
	register char	*bufp;
	register unsigned long	size, addr;
	register struct netvec	*nv;
	register int	vi, vc;
	register char	*head;

	assert(ipcreq_sending == 0);

	vc = ipcreq->count;
	nv = ipcreq->bv;
	bufp = ipcreq_copy_send_buffer;
	head = bufp;

	for (vi = 0; vi < vc; vi++) {
		addr = nv[vi].addr;
		size = nv[vi].size;
		IPC_TRACE_DEBUG("ipc bcopy", 3, addr, bufp, size, 0);
		if ((size >= 4096) &&
		    (((addr | (unsigned long) bufp) & 0xf) == 0)) {
			do {
				/*
				 *	use the slightly lower-cost page copy
				 */
				piped_page_copy((char *) addr, bufp);
				size -= 4096;
				addr += 4096;
				bufp += 4096;
			} while (size >= 4096);
		}
		if (size > 0) {
			bcopy((char *) addr, bufp, size);
			bufp += size;
		}
	}
	assert((bufp - head) < MAX_IPC_SEND);


	/*
	 *	re-write the request as just 1 big netvec and
	 *	handle the packetizing logic in icpreq_send().
	 */
	ipcreq->count = 1;
	ipcreq->bv[0].addr = (unsigned long) head;
	ipcreq->bv[0].size = ipcreq->total;
	IPC_TRACE_DEBUG("ipc bcopy done", 2, head, ipcreq->total, 0, 0);
}
#endif	IPC_COPY_SEND

/*
 *	record NORMA's netvec in our request structure.
 */

ipcreq_record_send(ipcreq, node, vec, count)
	ipcreq_t	*ipcreq;
	int		node;
	struct netvec	*vec;
	int		count;
{
	int	i, vi;
	unsigned long	bytes;

	bytes = 0;
	ipcreq->node = node;
	for (i = 0, vi = 0; i < count; i++) {
		/*
		 *	sometimes NORMA hands us a 0 length
		 *	vector; detect and avert it here.
		 */
		if (vec[i].size > 0) {
			ipcreq->bv[vi].addr = vec[i].addr;
			ipcreq->bv[vi].size = vec[i].size;
			bytes += vec[i].size;
			IPC_TRACE_DEBUG("send setup",
				4, node, vec[i].addr, vec[i].size, i);
			vi++;
		}
	}
	assert(bytes > 0);
	ipcreq->count = vi;
	ipcreq->total = bytes;

#if	IPC_COPY_SEND
	/*
	 *	XXX COPY DATA TO A PRIVATE BUFFER BEFORE NORMA
	 *	XXX GIVES SOMETHING ELSE A CHANCE TO SCRIBBLE
	 *	XXX ON THE PAGE WE'RE TRYING TO TRANSMIT.
	 */
	if (ipcreq_copy_send_enabled) {
		ipcreq_copy_send(ipcreq);
	}
#endif	IPC_COPY_SEND

	ipcreq_send_node = node;
	ipcreq_send_route = calculate_route(node);
	ipcreq_sending = bytes;
	IPC_TRACE_DEBUG("ipc record", 3,
		node, ipcreq_send_route, ipcreq_sending, 0);
}

/*
 *	NORMA IPC interface.
 */

netipc_send(remote, vec, count)
	int	remote;				/* Destination node number */
	register struct netvec *vec;		/* Buffer vector */
	int	count;				/* Buffer vector length */
{
	ipcreq_t	*ipcreq;
	int		method;
	unsigned long	item;

	IPC_TRACE_DEBUG("netipc_send", 3, remote, vec, count, 0);

	assert(ipcreq_sending == 0);

	ipcreq = &ipcreq_send_request;

	/*
	 *	store the netvecs of this request.
	 */
	ipcreq_record_send(ipcreq, remote, vec, count);

	/*
	 * If not for the possibility of copying data above, the message
	 * processor could handle all of netipc_send. However, data
	 * structures are robust enough to split it here and leave the
	 * copying on the compute processor.
	 * XXX When copying no longer necessary make this like netipc_recv.
	 */

	/*
	 * Dispatch the rest of send startup to the message processor
	 * if enabled.
	 */
	mcmsg_post(POST_IPCSEND);
}


/*
 *	NORMA IPC interface.
 */

netipc_recv(vec, count)
	register struct netvec *vec;		/* Provided buffer vector */
	int count;				/* Buffer vector length */
{

	/*
	 * Hand it to the message processor
	 */
	mcmsg_post(POST_IPCRECV, vec, count);
}

#if     DYNAMIC_NUM_NODES

/*
 * dynamic rts_cts_table
 *
 * Dynamically allocate size of rts table by utilizing information
 * received in model_dep.c ..  This is important for large systems..
 * See i860paragon/model_dep.c
 *
 */

struct ipc_sender_iterm *alloc_ipc_rts_table(table_size)
int                     table_size;
{
        char                    *cp;
        kern_return_t           kr;

        /* ask for wired kernel memory */

        if ((kr = kmem_alloc_wired(kernel_map,&cp,table_size)) != KERN_SUCCESS)
                panic("alloc_ipc_rts_table() kmem_alloc_wired() failed.");
        bzero(cp, table_size);
        return((struct ipc_sender_iterm *) cp);
}

#endif  /* DYNAMIC_NUM_NODES */

