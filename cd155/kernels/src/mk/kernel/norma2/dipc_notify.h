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
 * $Id: dipc_notify.h,v 1.3 1994/11/18 20:57:59 mtm Exp $
 * 
 * HISTORY:
 * $Log: dipc_notify.h,v $
 * Revision 1.3  1994/11/18  20:57:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:29:27  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.2  1994/07/11  16:48:08  rkl
 *  New and improved logging.
 *
 * Revision 1.1.2.1  1994/05/05  00:31:59  stans
 * more logging args
 *
 * Revision 1.1  1994/04/08  15:32:03  rkl
 * Initial revision
 *
 *
 */

#ifndef	_DIPC_NOTIFY_H_
#define	_DIPC_NOTIFY_H_

#define notify_log1	NORMA_LOG1
#define notify_log2	NORMA_LOG2
#define notify_log3	NORMA_LOG3
#define notify_log4	NORMA_LOG4
#define notify_log5	NORMA_LOG5
#define notify_log6	NORMA_LOG6
#define notify_log7	NORMA_LOG7
#define notify_log8	NORMA_LOG8

#define notify_entry0(func) \
	NORMA_ENTRY0(NOTIFY_LOG_ID, func)
#define notify_entry1(func, a)      \
	NORMA_ENTRY1(NOTIFY_LOG_ID, func, a)
#define notify_entry2(func, a, b) \
	NORMA_ENTRY2(NOTIFY_LOG_ID, func, a, b)
#define notify_entry3(func, a, b, c) \
	NORMA_ENTRY3(NOTIFY_LOG_ID, func, a, b, c)
#define notify_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(NOTIFY_LOG_ID, func, a, b, c, d)
#define notify_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(NOTIFY_LOG_ID, func, a, b, c, d, e)
#define notify_entry6(func, a, b, c, d, e, f) \
	NORMA_ENTRY6(NOTIFY_LOG_ID, func, a, b, c, d, e, f)

#endif /* _DIPC_NOTIFY_H_ */
