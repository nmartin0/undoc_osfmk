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
 * $Log: ipc_kmsg.h,v $
 * Revision 1.11  1994/11/18  20:49:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/07/13  01:22:09  andyp
 * Late changes from the NORMA2 branch.
 *
 * Revision 1.9  1994/07/12  19:22:02  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.8.4.10  1994/07/12  21:02:31  stans
 *   NORMA_VM --> NORMA_TASK for includes of "vm_map.h"; prep for ASVM.
 *
 * Revision 1.8.4.9  1994/06/20  17:13:37  rkl
 *  Removed BORG capability.
 *
 * Revision 1.8.4.8  1994/05/25  19:06:20  stans
 *   Since the kmsg now has a vm_map_t we need vm_map.h, NORMA_IPC --> NORMA_VM
 *
 * Revision 1.8.4.7  1994/05/25  06:50:06  andyp
 * Carry along the map holding any ool data for transmission.
 *
 * Revision 1.8.4.6  1994/04/05  23:29:44  stans
 * !NORMA_IPC
 *
 * Revision 1.8.4.5  1994/03/10  02:54:54  stans
 *   Set and assert() kmsg default type is local.
 *
 * Revision 1.8.4.4  1994/02/04  07:51:34  andyp
 * norma2 work broke regular NORMA kmsg deallocation making NORMA
 * nearly useless on a multinode Paragon.
 *
 * Revision 1.8.4.3  1994/01/28  19:31:08  rkl
 *  Changed the #defines for IKM_SIZE_NORMA and IKM_SIZE_NETWORK so networking
 *  will work again.  Needed because of new ikm_size as bit field.
 *
 * Revision 1.8.4.2  1994/01/27  16:57:03  stans
 *  Support NORMA2 IPC
 *
 * Revision 1.8.4.1  1994/01/26  01:49:55  stans
 *   kmsg size field broken (ala union and macros) into size and type fields.
 *
 * Revision 1.8  1993/10/20  15:53:59  rkl
 * Added "ikmus_fast_ool" field to kmsg structure to support NORMA Fast OOL
 * capability.  This bit is used to indicate to the receiver that the sender
 * wishes to send an OOL message using the Fast OOL method.
 *
 * Revision 1.7  1993/06/30  22:42:26  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.6  1993/06/21  23:53:27  andyp
 * Picked up the santioned versions of ipc_kmsg.h (bitfield unions)
 * and the hack for the vm_map_copyout_page_list() deadlock in ipc_kmsg.c
 * from dlb.  Picked up mods from OSF's tree for ipc_mqueue.c which contains
 * another counter for suspending the sending thread when transmitting.
 *
 * Revision 1.5  1993/06/19  01:20:53  andyp
 * Bitfield definitions used by NORMA were wrong; this version is from
 * OSF's tree.
 *
 * Revision 1.4  1993/06/09  01:36:48  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:33:05  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:32:43  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_kmsg.h,v
 * Revision 1.1.7.4  1993/06/16  19:48:27  alanl
 * 	Fix bitfield definition in kmsg header; some compilers
 * 	would treat the union in such a way that the bitfields
 * 	overlapped rather than being allocated contiguously
 * 	and then overlapping with the next unsigned int.
 * 	At the same time, combined the previous bitfield union
 * 	into the larger bitfield, saving some more space.
 * 	[1993/06/16  19:47:26  alanl]
 *
 * Revision 1.1.7.3  1993/04/22  19:38:29  rod
 * 	Add send state info for NORMA to struct kmsg.	[mmp@osf.org]
 * 	[1993/04/20  17:54:08  rod]
 * 
 * Revision 1.1.7.2  1993/03/03  21:19:58  dwm
 * 	Added ikm_ool_bytes to track sizeof ool data for fastpath (alanl).
 * 	[1993/03/03  21:16:22  dwm]
 * 
 * Revision 1.1  1992/09/30  02:28:52  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.11.2.4  92/05/28  18:14:33  jeffreyh
 * 	Add ikm_has_dest_right boolean.  Put in union with ikm_norma_acked.
 * 	[92/05/28            dlb]
 * 
 * Revision 2.11.2.3  92/05/26  18:53:43  jeffreyh
 * 	Add ikm_norma_acked field for norma message processing.
 * 	[92/05/05            dlb]
 * 
 * Revision 2.11.2.2  92/04/08  15:44:33  jeffreyh
 * 	Temporary debugging logic.
 * 	[92/04/06            dlb]
 * 
 * Revision 2.11.2.1  92/01/03  16:35:19  jsb
 * 	Added ikm_source_node to support norma_ipc_receive_rright.
 * 	[91/12/28  08:38:53  jsb]
 * 
 * Revision 2.11  91/12/14  14:26:54  jsb
 * 	NORMA_IPC: added ikm_copy to struct kmsg.
 * 
 * Revision 2.10  91/08/28  11:13:31  jsb
 * 	Renamed IKM_SIZE_CLPORT to IKM_SIZE_NORMA.
 * 	[91/08/15  08:12:02  jsb]
 * 
 * Revision 2.9  91/08/03  18:18:24  jsb
 * 	NORMA_IPC: added ikm_page field to struct ipc_kmsg.
 * 	[91/07/17  14:01:38  jsb]
 * 
 * Revision 2.8  91/06/17  15:46:15  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:46:12  jsb]
 * 
 * Revision 2.7  91/05/14  16:33:21  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/03/16  14:48:10  rpd
 * 	Replaced ith_saved with ipc_kmsg_cache.
 * 	[91/02/16            rpd]
 * 
 * Revision 2.5  91/02/05  17:22:08  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:45:52  mrt]
 * 
 * Revision 2.4  91/01/08  15:14:04  rpd
 * 	Added ipc_kmsg_free.  Generalized the notion of special message sizes.
 * 	[91/01/05            rpd]
 * 	Added declarations of ipc_kmsg_copyout_object, ipc_kmsg_copyout_body.
 * 	[90/12/21            rpd]
 * 
 * Revision 2.3  90/09/28  16:54:48  jsb
 * 	Added NORMA_IPC support (hack in ikm_free).
 * 	[90/09/28  14:03:06  jsb]
 * 
 * Revision 2.2  90/06/02  14:50:24  rpd
 * 	Increased IKM_SAVED_KMSG_SIZE from 128 to 256.
 * 	[90/04/23            rpd]
 * 	Created for new IPC.
 * 	[90/03/26  20:56:16  rpd]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 *	File:	ipc/ipc_kmsg.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Definitions for kernel messages.
 */

#ifndef	_IPC_IPC_KMSG_H_
#define _IPC_IPC_KMSG_H_

#include <cpus.h>
#include <mach_ipc_compat.h>
#include <norma_ipc.h>
#include <norma_task.h>

#include <mach/machine/vm_types.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/cpu_number.h>
#include <kern/macro_help.h>
#include <kern/kalloc.h>
#include <ipc/ipc_marequest.h>
#if	NORMA_TASK
#include <vm/vm_page.h>
#include <vm/vm_map.h>
#endif	NORMA_TASK

/*
 *	This structure is only the header for a kmsg buffer;
 *	the actual buffer is normally larger.  The rest of the buffer
 *	holds the body of the message.
 *
 *	In a kmsg, the port fields hold pointers to ports instead
 *	of port names.  These pointers hold references.
 *
 *	The ikm_header.msgh_remote_port field is the destination
 *	of the message.
 */

typedef struct ipc_kmsg {
	struct ipc_kmsg *ikm_next, *ikm_prev;
#if	NORMA2

	union {
		struct {
			unsigned int
				ikmus_size:30,		/* kmsg size */
				ikmus_kmsg_type:2;
							/* 0 == local kmsg
							 * 1 == meta kmsg
							 * 2 == net kmsg
							 * 3 == unused.
							 */
		} ikmu_sz_st;
		vm_size_t ikmu_size;
	} ikm_sz_u;

#define	ikm_kmsg_type	ikm_sz_u.ikmu_sz_st.ikmus_kmsg_type
#define	ikm_size	ikm_sz_u.ikmu_sz_st.ikmus_size
#define	ikm_init_size	ikm_sz_u.ikmu_size

#define IKM_KMSG_TYPE_LOCAL	0
#define IKM_KMSG_TYPE_META	1
#define IKM_KMSG_TYPE_NET	2

#else	/* NORMA2 */
	vm_size_t ikm_size;
#endif	/* NORMA2 */
	ipc_marequest_t ikm_marequest;
#if	NORMA_IPC
	vm_page_t	ikm_page;
	vm_map_copy_t	ikm_copy;
	unsigned long	ikm_source_node;
	vm_size_t	ikm_ool_bytes;		/* total size of ool data */
	union {
	    struct {
	       unsigned int
			ikmus_norma_acked:1,	/* RCV: Ack sent? */
			ikmus_has_dest_right:1,	/* SND: Holding dest right? */
		        ikmus_wait_for_send:1,	/* SND: waiting for send */
			ikmus_send_completed:1,	/* SND: kmsg completely sent */
			ikmus_no_wait:1,	/* SND: kernel: do not wait */
#if	NORMA_FAST_OOL
			ikmus_fast_ool:1,	/* SND: fast ool deliver */
#endif	NORMA_FAST_OOL
			ikmus_used_copyin:1;	/* SND: used vm_map_copyin */
            } ikmus;
	    unsigned int initbits;		/* to initialize above bits */
	} ikmu;
#endif	/* NORMA_IPC */
#if	NORMA2
	vm_map_t	ikm_dipc_map;		/* map used for ool sends   */
	vm_offset_t	ikm_transfer_index;	/* tracks ool rmda progress */
#endif	/* NORMA2 */
	mach_msg_header_t ikm_header;
} *ipc_kmsg_t;

#define	ikm_norma_acked		ikmu.ikmus.ikmus_norma_acked
#define	ikm_has_dest_right	ikmu.ikmus.ikmus_has_dest_right
#define ikm_wait_for_send	ikmu.ikmus.ikmus_wait_for_send
#define ikm_send_completed	ikmu.ikmus.ikmus_send_completed
#define ikm_no_wait		ikmu.ikmus.ikmus_no_wait
#define ikm_used_copyin		ikmu.ikmus.ikmus_used_copyin
#if	NORMA_FAST_OOL
#define	ikm_fast_ool		ikmu.ikmus.ikmus_fast_ool
#endif	NORMA_FAST_OOL

#define	IKM_NULL		((ipc_kmsg_t) 0)

#define	IKM_OVERHEAD							\
		(sizeof(struct ipc_kmsg) - sizeof(mach_msg_header_t))

#define	ikm_plus_overhead(size)	((vm_size_t)((size) + IKM_OVERHEAD))
#define	ikm_less_overhead(size)	((mach_msg_size_t)((size) - IKM_OVERHEAD))

/*
 * XXX	For debugging.
 */
#define IKM_BOGUS		((ipc_kmsg_t) 0xffffff10)

/*
 *	We keep a per-processor cache of kernel message buffers.
 *	The cache saves the overhead/locking of using kalloc/kfree.
 *	The per-processor cache seems to miss less than a per-thread cache,
 *	and it also uses less memory.  Access to the cache doesn't
 *	require locking.
 */

extern ipc_kmsg_t ipc_kmsg_cache[NCPUS];

#define ikm_cache()	ipc_kmsg_cache[cpu_number()]

/*
 *	The size of the kernel message buffers that will be cached.
 *	IKM_SAVED_KMSG_SIZE includes overhead; IKM_SAVED_MSG_SIZE doesn't.
 */

#define	IKM_SAVED_KMSG_SIZE	((vm_size_t) 256)
#define	IKM_SAVED_MSG_SIZE	ikm_less_overhead(IKM_SAVED_KMSG_SIZE)

#define	ikm_alloc(size)							\
		((ipc_kmsg_t) kalloc(ikm_plus_overhead(size)))

#define	ikm_init(kmsg, size)						\
MACRO_BEGIN								\
	ikm_init_special((kmsg), ikm_plus_overhead(size));		\
MACRO_END

#if	NORMA2

#define	ikm_init_special(kmsg, size)					\
MACRO_BEGIN								\
	(kmsg)->ikm_size = (size);					\
	(kmsg)->ikm_kmsg_type = IKM_KMSG_TYPE_LOCAL;			\
	(kmsg)->ikm_marequest = IMAR_NULL;				\
MACRO_END

#elif	NORMA_IPC

#define	ikm_init_special(kmsg, size)					\
MACRO_BEGIN								\
	(kmsg)->ikm_size = (size);					\
	(kmsg)->ikm_marequest = IMAR_NULL;				\
	(kmsg)->ikmu.initbits = 0;					\
MACRO_END

#else	/* NORMA_IPC == 0 */

#define	ikm_init_special(kmsg, size)					\
MACRO_BEGIN								\
	(kmsg)->ikm_size = (size);					\
	(kmsg)->ikm_marequest = IMAR_NULL;				\
MACRO_END

#endif	/* !NORMA_IPC */

#if	NORMA2

#define	ikm_check_initialized(kmsg, size)				\
MACRO_BEGIN								\
	assert((kmsg)->ikm_size == (size));				\
	assert((kmsg)->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL);		\
	assert((kmsg)->ikm_marequest == IMAR_NULL);			\
MACRO_END

#elif	NORMA_IPC

#define	ikm_check_initialized(kmsg, size)				\
MACRO_BEGIN								\
	assert((kmsg)->ikm_size == (size));				\
	assert((kmsg)->ikm_marequest == IMAR_NULL);			\
MACRO_END

#endif	/* NORMA_IPC */

/*
 *	Non-positive message sizes are special.  They indicate that
 *	the message buffer doesn't come from ikm_alloc and
 *	requires some special handling to free.
 *
 *	ipc_kmsg_free is the non-macro form of ikm_free.
 *	It frees kmsgs of all varieties.
 */

#if	NORMA2
#define	IKM_SIZE_NETWORK	0
#define ikm_free(kmsg)		ipc_kmsg_free(kmsg)
#elif	NORMA_IPC
#define	IKM_SIZE_NORMA		0
#define	IKM_SIZE_NETWORK	-1

#define	ikm_free(kmsg)							\
MACRO_BEGIN								\
	register vm_size_t _size = (kmsg)->ikm_size;			\
									\
	if ((int)_size > 0)						\
		kfree((vm_offset_t) (kmsg), _size);			\
	else								\
		ipc_kmsg_free(kmsg);					\
MACRO_END
#endif	/* NORMA_IPC */

/*
 *	struct ipc_kmsg_queue is defined in kern/thread.h instead of here,
 *	so that kern/thread.h doesn't have to include ipc/ipc_kmsg.h.
 */

#include <kern/thread.h>	/* for struct ipc_kmsg_queue */

typedef struct ipc_kmsg_queue *ipc_kmsg_queue_t;

#define	IKMQ_NULL		((ipc_kmsg_queue_t) 0)


#define	ipc_kmsg_queue_init(queue)		\
MACRO_BEGIN					\
	(queue)->ikmq_base = IKM_NULL;		\
MACRO_END

#define	ipc_kmsg_queue_empty(queue)	((queue)->ikmq_base == IKM_NULL)

extern void
ipc_kmsg_enqueue(/* ipc_kmsg_queue_t, ipc_kmsg_t */);

extern ipc_kmsg_t
ipc_kmsg_dequeue(/* ipc_kmsg_queue_t */);

extern void
ipc_kmsg_rmqueue(/* ipc_kmsg_queue_t, ipc_kmsg_t */);

#define	ipc_kmsg_queue_first(queue)		((queue)->ikmq_base)

extern ipc_kmsg_t
ipc_kmsg_queue_next(/* ipc_kmsg_queue_t, ipc_kmsg_t */);

#define	ipc_kmsg_rmqueue_first_macro(queue, kmsg)			\
MACRO_BEGIN								\
	register ipc_kmsg_t _next;					\
									\
	assert((queue)->ikmq_base == (kmsg));				\
									\
	_next = (kmsg)->ikm_next;					\
	if (_next == (kmsg)) {						\
		assert((kmsg)->ikm_prev == (kmsg));			\
		(queue)->ikmq_base = IKM_NULL;				\
	} else {							\
		register ipc_kmsg_t _prev = (kmsg)->ikm_prev;		\
									\
		(queue)->ikmq_base = _next;				\
		_next->ikm_prev = _prev;				\
		_prev->ikm_next = _next;				\
	}								\
  	/* XXX Debug paranoia */					\
  	kmsg->ikm_next = IKM_BOGUS;					\
  	kmsg->ikm_prev = IKM_BOGUS;					\
MACRO_END

#define	ipc_kmsg_enqueue_macro(queue, kmsg)				\
MACRO_BEGIN								\
	register ipc_kmsg_t _first = (queue)->ikmq_base;		\
									\
	if (_first == IKM_NULL) {					\
		(queue)->ikmq_base = (kmsg);				\
		(kmsg)->ikm_next = (kmsg);				\
		(kmsg)->ikm_prev = (kmsg);				\
	} else {							\
		register ipc_kmsg_t _last = _first->ikm_prev;		\
									\
		(kmsg)->ikm_next = _first;				\
		(kmsg)->ikm_prev = _last;				\
		_first->ikm_prev = (kmsg);				\
		_last->ikm_next = (kmsg);				\
	}								\
MACRO_END

extern void
ipc_kmsg_destroy(/* ipc_kmsg_t */);

extern void
ipc_kmsg_clean(/* ipc_kmsg_t */);

extern void
ipc_kmsg_free(/* ipc_kmsg_t */);

extern mach_msg_return_t
ipc_kmsg_get(/* mach_msg_header_t *, mach_msg_size_t, ipc_kmsg_t * */);

extern mach_msg_return_t
ipc_kmsg_get_from_kernel(/* mach_msg_header_t *, mach_msg_size_t,
			    ipc_kmsg_t * */);

extern mach_msg_return_t
ipc_kmsg_put(/* mach_msg_header_t *, ipc_kmsg_t, mach_msg_size_t */);

extern void
ipc_kmsg_put_to_kernel(/* mach_msg_header_t *, ipc_kmsg_t, mach_msg_size_t */);

extern mach_msg_return_t
ipc_kmsg_copyin_header(/* mach_msg_header_t *, ipc_space_t, mach_port_t */);

extern mach_msg_return_t
ipc_kmsg_copyin(/* ipc_kmsg_t, ipc_space_t, vm_map_t, mach_port_t */);

extern void
ipc_kmsg_copyin_from_kernel(/* ipc_kmsg_t */);

extern mach_msg_return_t
ipc_kmsg_copyout_header(/* mach_msg_header_t *, ipc_space_t, mach_port_t */);

extern mach_msg_return_t
ipc_kmsg_copyout_object(/* ipc_space_t, ipc_object_t,
			   mach_msg_type_name_t, mach_port_t * */);

extern mach_msg_return_t
ipc_kmsg_copyout_body(/* vm_offset_t, vm_offset_t, ipc_space_t, vm_map_t */);

extern mach_msg_return_t
ipc_kmsg_copyout(/* ipc_kmsg_t, ipc_space_t, vm_map_t, mach_port_t */);

extern mach_msg_return_t
ipc_kmsg_copyout_pseudo(/* ipc_kmsg_t, ipc_space_t, vm_map_t */);

extern void
ipc_kmsg_copyout_dest(/* ipc_kmsg_t, ipc_space_t */);

#if	MACH_IPC_COMPAT

extern mach_msg_return_t
ipc_kmsg_copyin_compat(/* ipc_kmsg_t, ipc_space_t, vm_map_t */);

extern mach_msg_return_t
ipc_kmsg_copyout_compat(/* ipc_kmsg_t, ipc_space_t, vm_map_t */);

#endif	MACH_IPC_COMPAT
#endif	_IPC_IPC_KMSG_H_
