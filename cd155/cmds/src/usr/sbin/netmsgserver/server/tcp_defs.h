/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * tcp_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/tcp_defs.h,v 1.2 1994/11/19 03:12:21 mtm Exp $
 *
 */

/*
 * Definitions for the TCP module.
 */


#ifndef	_TCP_DEFS_
#define	_TCP_DEFS_

#include	"mem.h"
#include	"sbuf.h"
#include	"sys_queue.h"

/*
 * Transaction records.
 */
typedef struct tcp_trans {
	int			state;	/* see defines below */
	unsigned long		trid;
	int			client_id;
	sbuf_ptr_t		data;
	int			crypt_level;
	int			(*reply_proc)();
	sys_queue_chain_t	transq;	/* list of pending/waiting transactions */
} tcp_trans_t, *tcp_trans_ptr_t;

#define	TCP_TR_INVALID	0
#define	TCP_TR_PENDING	1	/* awaiting a reply */
#define	TCP_TR_WAITING	2	/* awaiting transmission */


/*
 * Memory management definitions.
 */
extern mem_objrec_t		MEM_TCPTRANS;


#endif	_TCP_DEFS_
