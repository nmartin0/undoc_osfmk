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
 * $Id: tx_engine.h,v 1.3 1994/11/18 20:59:00 mtm Exp $
 *
 * HISTORY:
 */

#ifndef	_TX_ENGINE_H_
#define	_TX_ENGINE_H_

/*
 *  Transmit thread priorities.
 */
#define	DIPC_KMSG_DEALLOC_THREAD_PRI		10
#define	DIPC_PAGEOUT_KMSG_DEALLOC_THREAD_PRI	 5	/* BASEPRI_SYSTEM - 1 */
#define	DIPC_RDMA_FAULT_THREAD_PRI		 0

/*
 *  Determine message body size from ipc_kmsg size.
 */
#define	DIPC_MSG_BODY_SIZE(k) \
	(k->ikm_header.msgh_size - sizeof(mach_msg_header_t))

/*
 *  Interface queue structure between RDMA send callback and kmsg disposal.
 */
typedef	struct dipc_kmsg_disposal {
	ipc_kmsg_t	*put;
	ipc_kmsg_t	*take;
	ipc_kmsg_t	*top;
	ipc_kmsg_t	*bottom;
	decl_simple_lock_data(, take_ptr_lock)
} dipc_kmsg_disposal_t;

/*
 *  Interface queue structure between RDMA page-not-present callback and
 *  thread responsible for faulting in the page.
 *
 *  N.B.  As long as there is only one possible interrupt context
 *	  (i.e. only one CPU services callback interrupts) only the
 *	  `take' pointer requires a lock to be used between threads.
 */
typedef	struct dipc_rdma_fault_queue {
	rdma_handle_t	*put;
	rdma_handle_t	*take;
	rdma_handle_t	*top;
	rdma_handle_t	*bottom;
	decl_simple_lock_data(, take_ptr_lock)
} dipc_rdma_fault_queue_t;

/*
 *  DIPC parsing function arguments
 */
typedef struct dipc_parse_args {
	rdma_handle_t	handle;
	int		to_send;
	vm_map_t	map;
} dipc_parse_args_t;

/*
 *  NORMA map
 */
extern	vm_map_t	norma_map;

/*
 *  tx_engine prototypes
 */
extern	void		dipc_tx_engine_init( );
extern	void		dipc_rdma_vm_fault_thread( );
extern	void		dipc_kmsg_dealloc_thread( );
extern	void		dipc_kmsg_dealloc( ipc_kmsg_t );
extern	kern_return_t	dipc_cnvrt_port_array( ipc_port_t*,
						mach_msg_type_long_t*,
						int );
extern	kern_return_t	dipc_free_ool_data(	mach_msg_type_t*,
						vm_size_t,
						vm_offset_t*,
						vm_map_t );
extern	kern_return_t	dipc_free_ool_ports(	mach_msg_type_t*,
						vm_size_t,
						vm_offset_t*,
						vm_map_t );
extern	kern_return_t	dipc_send_ool_data(	mach_msg_type_t*,
						vm_size_t,
						vm_offset_t*,
						dipc_parse_args_t* );
extern	kern_return_t	dipc_send_ool_ports(	mach_msg_type_t*,
						vm_size_t,
						vm_offset_t*,
						dipc_parse_args_t* );
extern	kern_return_t	dipc_cnvrt_inline_port(mach_msg_type_long_t*,
						vm_size_t,
						ipc_port_t*,
						dipc_parse_args_t* );
extern	kern_return_t	dipc_cnvrt_ool_data(	mach_msg_type_t*,
						vm_size_t,
						vm_map_copy_t*,
						dipc_parse_args_t* );
extern	kern_return_t	dipc_cnvrt_ool_port(	mach_msg_type_long_t*,
						vm_size_t,
						ipc_port_t**,
						dipc_parse_args_t* );
extern	void		dipc_send_kmsg (	ipc_kmsg_t,
						rdma_handle_t );

extern	void		dipc_rdma_send_callback (	rdma_handle_t,
							ipc_kmsg_t );

#define tx_log1	NORMA_LOG1
#define tx_log2	NORMA_LOG2
#define tx_log3	NORMA_LOG3
#define tx_log4	NORMA_LOG4
#define tx_log5	NORMA_LOG5
#define tx_log6	NORMA_LOG6

#define tx_entry0(func) \
	NORMA_ENTRY0(TX_ENGINE_LOG_ID, func)
#define tx_entry1(func, a)      \
	NORMA_ENTRY1(TX_ENGINE_LOG_ID, func, a)
#define tx_entry2(func, a, b) \
	NORMA_ENTRY2(TX_ENGINE_LOG_ID, func, a, b)
#define tx_entry3(func, a, b, c) \
	NORMA_ENTRY3(TX_ENGINE_LOG_ID, func, a, b, c)
#define tx_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(TX_ENGINE_LOG_ID, func, a, b, c, d)
#define tx_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(TX_ENGINE_LOG_ID, func, a, b, c, d, e)

#endif	/* _TX_ENGINE_H_ */
