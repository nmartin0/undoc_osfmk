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
 * ipc.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/ipc.h,v 1.2 1994/11/19 03:11:01 mtm Exp $
 *
 */

/*
 * External definitions for the IPC module.
 */


#ifndef	_IPC_
#define	_IPC_

#include <mach/boolean.h>

extern boolean_t ipc_init();
/*
*/


extern void ipc_msg_accepted();
/*
port_rec_ptr_t	port_rec_ptr;
*/


extern void ipc_port_dead();
/*
port_rec_ptr_t	port_rec_ptr;
*/


#if	RPCMOD
extern void ipc_port_moved();
/*
port_rec_ptr_t	port_rec_ptr;
*/
#endif	RPCMOD


extern ipc_retry();
/*
port_rec_ptr_t	port_rec_ptr;
*/


extern void ipc_freeze();
/*
port_rec_ptr_t	port_rec_ptr;
*/


#if	NETPORT
/*
 * Macros to manipulate the kernel NETPORT (MACH_NP) tables.
 */

#define	ipc_netport_enter(netport,localport,local) {			\
	if (param.conf_netport) {					\
		netport_enter(task_self(),(netport),(localport),(local)); \
	}								\
}

#define	ipc_netport_remove(netport) {					\
	if (param.conf_netport) {					\
		netport_remove(task_self(),(netport));			\
	}								\
}
#endif	NETPORT

#endif	_IPC_
