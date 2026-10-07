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
 * $Id: dipc_migrate.h,v 1.4 1994/11/18 20:57:54 mtm Exp $
 *
 * HISTORY
 */

#ifndef	_DIPC_MIGRATE_H
#define	_DIPC_MIGRATE_H

/*
 * dipc port migrate states: 'dipc_migrate_state'
 */
#define DIPC_MS_AT_REST		0
#define DIPC_MS_MIGRATING	1

/*
 * IPC port state which is migrated from across nodes.
 */
typedef struct migrate_port_state {
        int     seqno;		/* current message sequence number */
        int     qlimit;		/* current message queue limit */
        int     transit;	/* transit count */
	int	send_once;	/* count of send-once rights */
	int	resident_node;	/* where the old principal current resides */
	int	msgcount;	/* current # of queued messages */
} dipc_port_migration_state;

typedef dipc_port_migration_state *dipc_port_migration_state_t;

/*
 * prototypes
 */

extern ipc_port_t	dipc_receive_rright( dipc_uid_t );

extern kern_return_t	dipc_receive_migrating_message(	dipc_uid_t,
      	         					mach_msg_type_name_t,
       	        					mach_port_t * );
/*
 * logging macros
 */
#define migrate_log1 NORMA_LOG1
#define migrate_log2 NORMA_LOG2
#define migrate_log3 NORMA_LOG3
#define migrate_log4 NORMA_LOG4
#define migrate_log5 NORMA_LOG5
#define migrate_log6 NORMA_LOG6
#define migrate_log7 NORMA_LOG7
#define migrate_log8 NORMA_LOG8
#define migrate_log9 NORMA_LOG9
#define migrate_log10 NORMA_LOG10

#define migrate_entry0(func) \
        NORMA_ENTRY0(MIGRATE_LOG_ID, func)
#define migrate_entry1(func, a)      \
        NORMA_ENTRY1(MIGRATE_LOG_ID, func, a)
#define migrate_entry2(func, a, b) \
        NORMA_ENTRY2(MIGRATE_LOG_ID, func, a, b)
#define migrate_entry3(func, a, b, c) \
        NORMA_ENTRY3(MIGRATE_LOG_ID, func, a, b, c)
#define migrate_entry4(func, a, b, c, d) \
        NORMA_ENTRY4(MIGRATE_LOG_ID, func, a, b, c, d)
#define migrate_entry5(func, a, b, c, d, e) \
        NORMA_ENTRY5(MIGRATE_LOG_ID, func, a, b, c, d, e)

#endif	/* _DIPC_MIGRATE_H */
