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
 * $Id: dipc_port.h,v 1.3 1994/11/18 20:58:06 mtm Exp $
 *
 * HISTORY
 */

#ifndef	_DIPC_PORT_H_
#define	_DIPC_PORT_H_

typedef ipc_port_t	dipc_port_t;
typedef dipc_port_t	dipc_proxy_t;

#define	port_log1	NORMA_LOG1
#define	port_log2	NORMA_LOG2
#define	port_log3	NORMA_LOG3
#define	port_log4	NORMA_LOG4
#define	port_log5	NORMA_LOG5
#define	port_log6	NORMA_LOG6
#define	port_log7	NORMA_LOG7
#define	port_log8	NORMA_LOG8
#define	port_log9	NORMA_LOG9
#define	port_log10	NORMA_LOG10

#define	port_entry0(func) \
	NORMA_ENTRY0(PORT_LOG_ID, func)
#define	port_entry1(func, a) \
	NORMA_ENTRY1(PORT_LOG_ID, func, a)
#define	port_entry2(func, a, b) \
	NORMA_ENTRY2(PORT_LOG_ID, func, a, b)
#define	port_entry3(func, a, b, c) \
	NORMA_ENTRY3(PORT_LOG_ID, func, a, b, c)
#define	port_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(PORT_LOG_ID, func, a, b, c, d)
#define	port_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(PORT_LOG_ID, func, a, b, c, d, e)

extern	ipc_port_t	dipc_proxy_create( dipc_uid_t );
extern	void		dipc_proxy_destroy( ipc_port_t );
extern	void		dipc_port_remove( ipc_port_t );
extern	void		dipc_port_remove_try( ipc_port_t );
extern	void		dipc_port_destroy( ipc_port_t );
extern	kern_return_t	dipc_receive_dest( ipc_kmsg_t );
extern	ipc_port_t	dipc_receive_soright( dipc_uid_t );
extern	ipc_port_t	dipc_receive_sright( dipc_uid_t );
extern	ipc_port_t	dipc_receive_port(dipc_uid_t, mach_msg_type_name_t );
#endif	/* _DIPC_PORT_H_ */
