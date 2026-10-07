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
 *	$Id: kmsg_convert.h,v 1.4 1995/02/24 17:56:11 andyp Exp $
 */
#ifndef	_KMSG_CONVERT_H_
#define	_KMSG_CONVERT_H_ 1


/*
 *  Stats structure.
 */
#define CONVERT_NKMSG_STATS(a) dipc_nkmsg_convert_stats.a

typedef struct {
	unsigned long	inline_ports;
	unsigned long	ool_ports;
	unsigned long	uids;
	unsigned long	uid_null;
	unsigned long	uid_dead;
	unsigned long	nkmsg;
	unsigned long	dest_dead;
	unsigned long	reply;
	unsigned long	complex;
} dipc_nkmsg_convert_stats_t;


/*
 *  Kmsg conversion prototypes
 */

extern kern_return_t	convert_net_kmsg_2_kmsg( ipc_kmsg_t );

/*
 * rkl logging macros
 */

#define kcnvrt_log1 NORMA_LOG1
#define kcnvrt_log2 NORMA_LOG2
#define kcnvrt_log3 NORMA_LOG3
#define kcnvrt_log4 NORMA_LOG4
#define kcnvrt_log5 NORMA_LOG5
#define kcnvrt_log6 NORMA_LOG6

#define kcnvrt_entry0(func) \
        NORMA_ENTRY0(KMSG_CONVERT_LOG_ID, func)
#define kcnvrt_entry1(func, a)      \
        NORMA_ENTRY1(KMSG_CONVERT_LOG_ID, func, a)
#define kcnvrt_entry2(func, a, b) \
        NORMA_ENTRY2(KMSG_CONVERT_LOG_ID, func, a, b)
#define kcnvrt_entry3(func, a, b, c) \
        NORMA_ENTRY3(KMSG_CONVERT_LOG_ID, func, a, b, c)
#define kcnvrt_entry4(func, a, b, c, d) \
        NORMA_ENTRY4(KMSG_CONVERT_LOG_ID, func, a, b, c, d)
#define kcnvrt_entry5(func, a, b, c, d, e) \
        NORMA_ENTRY5(KMSG_CONVERT_LOG_ID, func, a, b, c, d, e)

#endif	/* _KMSG_CONVERT_H_ */
