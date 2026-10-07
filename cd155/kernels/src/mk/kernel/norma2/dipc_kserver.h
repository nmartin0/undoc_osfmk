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
 *  DIPC (Distributed IPC) server
 *
 * $Id: dipc_kserver.h,v 1.3 1994/11/18 20:57:50 mtm Exp $
 *
 * HISTORY:
 */
#ifndef	_DIPC_KSERVER_H_
#define	_DIPC_KSERVER_H_ 1

/*
 * kernel object port.
 *	all kernel obj to kernel obj IPC's are sent to and received from this
 *	port.  In essence this port represents the local kernel.
 */

extern ipc_port_t	dipc_kernel_port;

/*
 * rkl logging macros
 */
#define ksvr_log1 NORMA_LOG1
#define ksvr_log2 NORMA_LOG2
#define ksvr_log3 NORMA_LOG3
#define ksvr_log4 NORMA_LOG4
#define ksvr_log5 NORMA_LOG5
#define ksvr_log6 NORMA_LOG6
#define ksvr_log7 NORMA_LOG7
#define ksvr_log8 NORMA_LOG8
#define ksvr_log9 NORMA_LOG9

#define ksvr_entry0(func) \
        NORMA_ENTRY0(KSERVER_LOG_ID, func)
#define ksvr_entry1(func, a)      \
        NORMA_ENTRY1(KSERVER_LOG_ID, func, a)
#define ksvr_entry2(func, a, b) \
        NORMA_ENTRY2(KSERVER_LOG_ID, func, a, b)
#define ksvr_entry3(func, a, b, c) \
        NORMA_ENTRY3(KSERVER_LOG_ID, func, a, b, c)
#define ksvr_entry4(func, a, b, c, d) \
        NORMA_ENTRY4(KSERVER_LOG_ID, func, a, b, c, d)
#define ksvr_entry5(func, a, b, c, d, e) \
        NORMA_ENTRY5(KSERVER_LOG_ID, func, a, b, c, d, e)

#endif	/* _DIPC_KSERVER_H_ */
