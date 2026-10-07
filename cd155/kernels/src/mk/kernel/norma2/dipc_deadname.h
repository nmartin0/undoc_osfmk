/*
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
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
 * $Id: dipc_deadname.h,v 1.3 1994/11/18 20:57:40 mtm Exp $
 * 
 * HISTORY:
 * $log$
 */

#ifndef	_DIPC_DEADNAME_H
#define	_DIPC_DEADNAME_H

#define deadname_log1	NORMA_LOG1
#define deadname_log2	NORMA_LOG2
#define deadname_log3	NORMA_LOG3
#define deadname_log4	NORMA_LOG4
#define deadname_log5	NORMA_LOG5
#define deadname_log6	NORMA_LOG6

#define deadname_entry0(func) \
	NORMA_ENTRY0(DEAD_NAME_LOG_ID, func)
#define deadname_entry1(func, a)      \
	NORMA_ENTRY1(DEAD_NAME_LOG_ID, func, a)
#define deadname_entry2(func, a, b) \
	NORMA_ENTRY2(DEAD_NAME_LOG_ID, func, a, b)
#define deadname_entry3(func, a, b, c) \
	NORMA_ENTRY3(DEAD_NAME_LOG_ID, func, a, b, c)
#define deadname_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(DEAD_NAME_LOG_ID, func, a, b, c, d)
#define deadname_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(DEAD_NAME_LOG_ID, func, a, b, c, d, e)
#define deadname_entry6(func, a, b, c, d, e, f) \
	NORMA_ENTRY6(DEAD_NAME_LOG_ID, func, a, b, c, d, e, f)

#endif /* _DIPC_DEADNAME_H */
