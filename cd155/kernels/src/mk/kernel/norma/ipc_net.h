/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */

/*
 * SSD HISTORY
 * $Log: ipc_net.h,v $
 * Revision 1.9  1994/11/18  20:55:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/10/20  16:01:55  rkl
 * Added NORMA Fast OOL capability.  When a single OOL type that is larger
 * than a VM page and <= a page list is sent, it will attempt to use the
 * Fast OOL transfer method.  This method allows all pages except for the
 * last to be sent without waiting for an ACK from the receiver.  If the
 * receiver does not have the resources to support the transfer, it will
 * be done in the regular way.
 *
 * Revision 1.7  1993/07/28  16:11:47  stans
 *    Enable dynamic allocation of NORMA PCS (Protocol Control Structures).
 *
 * Revision 1.6  1993/07/22  02:20:32  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.5  1993/06/30  22:50:35  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/04/27  20:45:49  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_net.h,v
 * Revision 1.1.5.4  1993/04/15  22:45:02  alanl
 * 	Paging flow control (NORMA_VM).  Move recognition of
 * 	critical NORMA messages back into ipc_unreliable.c.  [alanl]
 * 	[1993/04/15  22:10:36  alanl]
 *
 * Revision 1.1.5.3  1993/03/03  21:20:03  dwm
 * 	Add pg_ool_bytes for norma fastpath. (alanl)
 * 	[1993/03/03  21:16:33  dwm]
 * 
 * Revision 1.1.5.2  1993/02/02  13:39:55  dwm
 * 	Add NETIPC_TYPE_MASK and NETIPC_TYPE_MAX
 * 	to bound types.  Provide a single definition
 * 	for NORMA_IPC_PULL_RECEIVE_MSG. (alanl)
 * 	[1993/02/01  22:36:03  dwm]
 * 
 * Revision 1.1  1992/09/30  02:34:04  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.2.2.3  92/09/15  17:34:06  jeffreyh
 * 	With jeffreyh:  stole bits from the message type field
 * 	for use in suspend/resume.  Defined NETIPC_PAGE_TRACE,
 * 	debugging code for tracing page flows between nodes.
 * 	[92/08/20            alanl]
 * 
 * 	Changed pg_norma_uid to pg_seqid; with recent changes,
 * 	it is possible to have more than one kmsg on the incomplete
 * 	list that came from the same port/node.  The norma_uid is
 * 	insufficient to distinguish this case, whereas the sequence
 * 	id will do just fine.  [With jeffreyh.]
 * 
 * Revision 2.2.2.2  92/06/24  18:01:20  jeffreyh
 * 	Added pg_norma_uid to pginfo structure.
 * 	[92/06/10            jeffreyh]
 * 
 * Revision 2.2.2.1  92/01/21  21:51:40  jsb
 * 	Added file/author/date comment.
 * 	[92/01/21  19:44:13  jsb]
 * 
 * 	Conditionalized NETIPC_CHECKSUM on NORMA_ETHER. Removed MAXVEC
 * 	definition. Removed lint-inspired includes of ipc_{pset,space}.h.
 * 	[92/01/16  22:09:32  jsb]
 * 
 * 	Added NETIPC_CHECKSUM conditional and netipc_hdr checksum field.
 * 	[92/01/14  21:32:29  jsb]
 * 
 * 	Changed ctl_status type.
 * 	[92/01/13  19:34:52  jsb]
 * 
 * 	De-linted.
 * 	[92/01/13  10:14:47  jsb]
 * 
 * 	Moved protocol dependent definitions into norma/ipc_unreliable.c.
 * 	Added ctl_status to netipc_hdr (which shouldn't be exported anyway).
 * 	[92/01/11  17:07:26  jsb]
 * 
 * 	Old contents moved to norma/ipc_netvec.h.
 * 	Now contains definitions shared by files split from norma/ipc_net.c.
 * 	[92/01/10  20:38:53  jsb]
 * 
 */ 
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1992 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	norma/ipc_net.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for reliable delivery and flow control for NORMA_IPC.
 */

#include <norma_ether.h>

#include <machine/machparam.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <mach/vm_param.h>
#include <kern/assert.h>
#include <kern/lock.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_kmsg.h>
#include <norma/ipc_node.h>
#include <norma/ipc_netvec.h>
#include <sys/varargs.h>

#if	NORMA_ETHER
#define	NETIPC_CHECKSUM	1
#else
/*
 * XXX
 * The checksumming code is not likely to work on the ipsc until
 * the netipc_recv routines for i386 and i860 are changed to set
 * the vector sizes to reflect the size of the incoming data.
 */
#define	NETIPC_CHECKSUM	0
#endif

#define	NETIPC_TYPE_INVALID	0x00000000L
#define	NETIPC_TYPE_KMSG	0xabcd0f00L
#define	NETIPC_TYPE_PAGE	0xabcd0f01L
#define	NETIPC_TYPE_CTL		0xabcd0f02L
#define	NETIPC_TYPE_MASK	0x000000ffL
#define NETIPC_TYPE_MAX		3L

#define	NETIPC_PAGE_TRACE	MACH_ASSERT

struct pginfo {
	unsigned long	pg_msgh_offset;
	boolean_t	pg_page_first;
	boolean_t	pg_page_last;
	boolean_t	pg_copy_last;
	unsigned long	pg_copy_offset;
	unsigned long	pg_copy_size;
	unsigned long	pg_first_seqid;
	unsigned long   pg_ool_bytes;   /* total expected length of ool data */
					/* ...trade-off:  can send once with */
					/* kmsg, but then forced to look up */
					/* kmsg for each page... */
#if	NETIPC_PAGE_TRACE
	unsigned long	pg_remote_port;	/* from dp_remote_port */
	unsigned long	pg_copy_index;	/* from dp_copy_index */
	unsigned long	pg_copy_npages;	/* from dp_copy_npages */
	unsigned long	pg_msgh_id;	/* from ikm_header->msgh_id */
#endif
};

struct netipc_hdr {
#if	NETIPC_CHECKSUM
	unsigned long	checksum;
#endif	NETIPC_CHECKSUM
	unsigned long	type;
	unsigned long	seqid;
	struct pginfo	pg;
	unsigned long	remote;
	unsigned long	ctl;
	unsigned long	ctl_seqid;
	kern_return_t	ctl_status;
	unsigned long	ctl_data;
	unsigned long	incarnation;
};

/*
 *	The upper two bits of the low 16-bit word of the
 *	netipc_hdr.type field are stolen for flow control.
 *	Eventually, this will be a single boolean bit.
 */
#define	NODE_MASK		(0xffff0fff)
#define	NODE_OK			(0x00004000)
#define	NODE_RESUMED		(0x0000c000)

/*
 * Some devices want virtual addresses, others want physical addresses.
 *
 * KVTODEV:	Kernel virtual address to device address
 * DEVTOKV:	Device address to kernel virtual address
 * VPTODEV:	vm_page_t to device address
 *
 * XXX These should be defined somewhere else.
 */
#if	NORMA_ETHER	|| i860
/*
 * Device uses virtual addresses.
 */
#define	KVTODEV(addr)	((unsigned long) (addr))
#define	DEVTOKV(addr)	((unsigned long) (addr))
#define	VPTODEV(m)	(VPTOKV(m))
#else	/*NORMA_ETHER*/
/*
 * Device uses physical addresses.
 */
#define	KVTODEV(addr)	((unsigned long) kvtophys(addr))
#define	DEVTOKV(addr)	((unsigned long) phystokv(addr))
#define	VPTODEV(m)	((m)->phys_addr)
#endif	/*NORMA_ETHER*/

#define	VPTOKV(m)	phystokv((m)->phys_addr)

extern void netipc_thread_lock();
extern void netipc_thread_unlock();
extern void netipc_copy_ungrab();

#define	DYNAMIC_NUM_NODES 1	/* see also ipc_special.c */

#if	DYNAMIC_NUM_NODES

extern int	max_num_nodes;

#define MAX_WINDOW_SIZE 1

#define WX	(MAX_WINDOW_SIZE + 1)

typedef struct retry {
        int     ticks;
        int     backoff;
} retry_t;

/*
 * Protocol control structure
 * (struct pcb is already in use)
 */
typedef struct pcs	*pcs_t;
#define	PCS_NULL	((pcs_t) 0)
struct pcs {
	unsigned long	pcs_remote;
	unsigned long	pcs_last_received;
	unsigned long	pcs_last_sent;
	unsigned long	pcs_nacked;
	unsigned long	pcs_unacked_packetid[WX];
	unsigned long	pcs_incarnation;
	unsigned long	pcs_new_incarnation;
	unsigned long	pcs_ctl;
	unsigned long	pcs_ctl_seqid;
	kern_return_t	pcs_ctl_status;
	unsigned long	pcs_ctl_data;
	boolean_t	pcs_suspend_sent;	/* out:  suspend was sent */
#if	NORMA_FAST_OOL
	boolean_t	pcs_fast_ool;		/* out:  fast ool in progress */
#endif	NORMA_FAST_OOL
	unsigned long	pcs_suspend_seqid;	/* suspended packet id */
	retry_t		pcs_resume;
	retry_t		pcs_ticks;
	queue_chain_t	pcs_timer;
	queue_chain_t	pcs_send;
	queue_chain_t	pcs_suspend;		/* in:  remote suspended us  */
};

/*
 * see i860paragon/model_dep.c and norma/ipc_unreliable.c
 */
#define MAX_NUM_NODES	max_num_nodes

#endif	/* DYNAMIC_NUM_NODES */

#if	NORMA_FAST_OOL
#define	NORMA_ANY_NODE		(~0)
#endif	NORMA_FAST_OOL
