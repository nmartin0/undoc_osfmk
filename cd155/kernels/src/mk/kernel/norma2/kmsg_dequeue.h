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
 * $Id: kmsg_dequeue.h,v 1.3 1994/11/18 20:58:34 mtm Exp $
 *
 * HISTORY:
 */

#ifndef	_KMSG_DEQUEUE_H_
#define	_KMSG_DEQUEUE_H_ 1

/*
 *  Kmsg dequeue prototypes
 */

mach_msg_return_t
	 dipc_kmsg_dequeue(meta_kmsg_t, ipc_kmsg_queue_t, ipc_kmsg_t *);

/*
 * logging macros
 */

#define dequeue_log1 NORMA_LOG1
#define dequeue_log2 NORMA_LOG2
#define dequeue_log3 NORMA_LOG3
#define dequeue_log4 NORMA_LOG4
#define dequeue_log5 NORMA_LOG5
#define dequeue_log6 NORMA_LOG6

#define dequeue_entry0(func) \
        NORMA_ENTRY0(KMSG_DEQUEUE_LOG_ID, func)
#define dequeue_entry1(func, a)      \
        NORMA_ENTRY1(KMSG_DEQUEUE_LOG_ID, func, a)
#define dequeue_entry2(func, a, b) \
        NORMA_ENTRY2(KMSG_DEQUEUE_LOG_ID, func, a, b)
#define dequeue_entry3(func, a, b, c) \
        NORMA_ENTRY3(KMSG_DEQUEUE_LOG_ID, func, a, b, c)
#define dequeue_entry4(func, a, b, c, d) \
        NORMA_ENTRY4(KMSG_DEQUEUE_LOG_ID, func, a, b, c, d)
#define dequeue_entry5(func, a, b, c, d, e) \
        NORMA_ENTRY5(KMSG_DEQUEUE_LOG_ID, func, a, b, c, d, e)

#endif	/* _KMSG_DEQUEUE_H_ */
