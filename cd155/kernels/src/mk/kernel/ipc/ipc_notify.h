/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: ipc_notify.h,v $
 * Revision 1.5  1994/11/18  20:49:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:42:38  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:33:18  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:33:02  dleslie
 * First R1_0 release
 *
 * Revision 2.4  91/05/14  16:34:40  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:22:39  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:48:56  mrt]
 * 
 * Revision 2.2  90/06/02  14:50:55  rpd
 * 	Created for new IPC.
 * 	[90/03/26  20:58:15  rpd]
 * 
 */
/*
 *	File:	ipc/ipc_notify.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Declarations of notification-sending functions.
 */

#ifndef	_IPC_IPC_NOTIFY_H_
#define _IPC_IPC_NOTIFY_H_

#include <mach_ipc_compat.h>

extern void
ipc_notify_init();

extern void
ipc_notify_port_deleted(/* ipc_port_t, mach_port_t */);

extern void
ipc_notify_msg_accepted(/* ipc_port_t, mach_port_t */);

extern void
ipc_notify_port_destroyed(/* ipc_port_t, ipc_port_t */);

extern void
ipc_notify_no_senders(/* ipc_port_t, mach_port_mscount_t */);

extern void
ipc_notify_send_once(/* ipc_port_t */);

extern void
ipc_notify_dead_name(/* ipc_port_t, mach_port_t */);

#if	MACH_IPC_COMPAT

extern void
ipc_notify_port_deleted_compat(/* ipc_port_t, mach_port_t */);

extern void
ipc_notify_msg_accepted_compat(/* ipc_port_t, mach_port_t */);

extern void
ipc_notify_port_destroyed_compat(/* ipc_port_t, ipc_port_t */);

#endif	MACH_IPC_COMPAT
#endif	_IPC_IPC_NOTIFY_H_
