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
 * nm_extra.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/nm_extra.h,v 1.2 1994/11/19 03:11:37 mtm Exp $
 *
 */

/*
 * External definitions of functions provided by nm_extra.c.
 */


#ifndef	_NM_EXTRA_
#define	_NM_EXTRA_

#include "config.h"
#include "debug.h"

#if	LOCK_THREADS
#define netmsg_receive(msg_ptr) netmsg_receive_locked(msg_ptr)
#else	LOCK_THREADS
#define netmsg_receive(msg_ptr) msg_receive(msg_ptr, MSG_OPTION_NONE, 0)
#endif	LOCK_THREADS

extern void ipaddr_to_string();
/*
char		*output_string;
netaddr_t	input_address;
*/

#endif	_NM_EXTRA_
