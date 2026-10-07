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
 * $Id: dipc_transit.h,v 1.3 1994/11/18 20:58:21 mtm Exp $
 *
 * HISTORY
 * $Log: dipc_transit.h,v $
 * Revision 1.3  1994/11/18  20:58:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:29:50  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.9  1994/07/11  16:49:32  rkl
 *  New and improved logging.
 *
 * Revision 1.1.2.8  1994/06/28  17:15:36  stans
 *   Reduce compiler warnings:
 *     added dipc_port_lookup() extern
 *     removed dipc_port_destroy() extern
 *
 * Revision 1.1.2.7  1994/06/24  17:39:23  rkl
 *  Added extended statitistic gathering and reporting.
 *
 * Revision 1.1.2.6  1994/05/05  00:52:18  stans
 * more logging args
 *
 * Revision 1.1.2.5  1994/04/15  21:19:14  rkl
 *  Added prototypes
 *
 * Revision 1.1.2.4  1994/03/27  03:31:09  stans
 * missing transit_log6
 *
 * Revision 1.1.2.3  1994/03/23  20:28:56  rkl
 *  Attempt to make lint(1) happy.
 *
 * Revision 1.1.2.2  1994/03/15  00:34:06  rkl
 *  Made changes to support new norma logging capability.
 *
 * Revision 1.1.2.1  1994/03/11  01:04:54  andyp
 * Split the dipc server into two threads and two classes that share
 * large amounts of code.  Added definitions for group and class
 * allocations.  Added transit request processing to the dipc control
 * server thread.  Moved the old fake stuff out of the way and into it's
 * own group and class.
 *
 * Revision 1.1  1994/03/10  07:44:01  rkl
 * Initial revision
 *
 */

#ifndef	_DIPC_TRANSIT_H_
#define	_DIPC_TRANSIT_H_

/*
 *  Stats structure.
 */
#define TRANIST_STATS(a) dipc_transit_stats.a

typedef struct {
	int	foo;
} dipc_transit_stats_t;

#define transit_log1	NORMA_LOG1
#define transit_log2	NORMA_LOG2
#define transit_log3	NORMA_LOG3
#define transit_log4	NORMA_LOG4
#define transit_log5	NORMA_LOG5
#define transit_log6	NORMA_LOG6
#define transit_log7	NORMA_LOG7
#define transit_log8	NORMA_LOG8
#define transit_log9	NORMA_LOG9

#define transit_entry0(func) \
	NORMA_ENTRY0(TRANSIT_LOG_ID, func)
#define transit_entry1(func, a)      \
	NORMA_ENTRY1(TRANSIT_LOG_ID, func, a)
#define transit_entry2(func, a, b) \
	NORMA_ENTRY2(TRANSIT_LOG_ID, func, a, b)
#define transit_entry3(func, a, b, c) \
	NORMA_ENTRY3(TRANSIT_LOG_ID, func, a, b, c)
#define transit_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(TRANSIT_LOG_ID, func, a, b, c, d)
#define transit_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(TRANSIT_LOG_ID, func, a, b, c, d, e)
#define transit_entry6(func, a, b, c, d, e, f) \
	NORMA_ENTRY6(TRANSIT_LOG_ID, func, a, b, c, d, e, f)

#if __STDC__

extern	void		dipc_get_transits( ipc_port_t );
extern	void		dipc_give_transits( ipc_port_t, int );
extern	void		dipc_return_transits( ipc_port_t );
extern	void		dipc_reclaim_transits( ipc_port_t, int );
extern	dipc_uid_t	dipc_send_sright( ipc_port_t, boolean_t );
extern	dipc_uid_t	dipc_send_soright( ipc_port_t, boolean_t );
extern	dipc_uid_t	dipc_send_rright( ipc_port_t );
extern	dipc_uid_t	dipc_send_port( ipc_port_t,
					mach_msg_type_name_t,
					boolean_t );
extern	ipc_port_t	dipc_port_lookup( dipc_uid_t );
#else

extern	void		dipc_get_transits();
extern	void		dipc_give_transits();
extern	void		dipc_return_transits();
extern	void		dipc_reclaim_transits();
extern	dipc_uid_t	dipc_send_sright();
extern	dipc_uid_t	dipc_send_soright();
extern	dipc_uid_t	dipc_send_rright();
extern	dipc_uid_t	dipc_send_port();
extern	ipc_port_t	dipc_port_lookup();

#endif 	/* __STDC__ */
#endif	/* _DIPC_TRANSIT_H_ */
