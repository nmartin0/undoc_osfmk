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
 * $Id: recv_engine.h,v 1.3 1994/11/18 20:58:54 mtm Exp $
 *
 * HISTORY:
 *
 */

#ifndef	_RX_ENGINE_H_
#define	_RX_ENGINE_H_ 1

/*
 *  Prototypes
 */
extern	void		dipc_recv_engine_init( );

extern	kern_return_t	copyout_kmsg(		ipc_kmsg_t,
						char* );

extern	void		flush_outstanding_data(	rdma_handle_t,
						vm_size_t,
						vm_map_t );

extern	void		recv_callback(		rdma_handle_t,
						rdma_notify_t );

extern	void		recv_rdma_data(		rdma_handle_t,
						vm_offset_t,
						vm_size_t,
						vm_map_t );

extern	kern_return_t	recv_ool_data(		mach_msg_type_long_t*,
						vm_size_t,
						vm_offset_t*,
						rdma_handle_t );

extern	kern_return_t	recv_inline_port(	mach_msg_type_long_t*,
						vm_size_t,
						vm_offset_t*,
						int );

extern	kern_return_t	recv_ool_port(		mach_msg_type_long_t*,
						vm_size_t,
						vm_offset_t*,
						rdma_handle_t );

extern	kern_return_t	process_complex_mesg_body( ipc_kmsg_t,
						rdma_handle_t,
						kmsg_parse_tbl_t* );

extern	kern_return_t	recv_kmsg( ipc_kmsg_t, unsigned, rdma_handle_t );

extern	ipc_kmsg_t	process_meta_kmsg( meta_kmsg_t );

extern	ipc_kmsg_t	convert_meta_kmsg_2_kmsg( meta_kmsg_t, boolean_t );

/*
 * logging macros
 */

#define rx_log1 NORMA_LOG1
#define rx_log2 NORMA_LOG2
#define rx_log3 NORMA_LOG3
#define rx_log4 NORMA_LOG4
#define rx_log5 NORMA_LOG5
#define rx_log6 NORMA_LOG6
#define rx_log7 NORMA_LOG7
#define rx_log8 NORMA_LOG8
#define rx_log9 NORMA_LOG9

#define rx_entry0(func) \
        NORMA_ENTRY0(RX_ENGINE_LOG_ID, func)
#define rx_entry1(func, a)      \
        NORMA_ENTRY1(RX_ENGINE_LOG_ID, func, a)
#define rx_entry2(func, a, b) \
        NORMA_ENTRY2(RX_ENGINE_LOG_ID, func, a, b)
#define rx_entry3(func, a, b, c) \
        NORMA_ENTRY3(RX_ENGINE_LOG_ID, func, a, b, c)
#define rx_entry4(func, a, b, c, d) \
        NORMA_ENTRY4(RX_ENGINE_LOG_ID, func, a, b, c, d)
#define rx_entry5(func, a, b, c, d, e) \
        NORMA_ENTRY5(RX_ENGINE_LOG_ID, func, a, b, c, d, e)

#endif	/* _RX_ENGINE_H_ */
