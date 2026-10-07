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
 * nm_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/nm_defs.h,v 1.2 1994/11/19 03:11:35 mtm Exp $
 *
 */

/*
 * Random definitions for the network service that everyone needs!
 */


#ifndef	_NM_DEFS_
#define	_NM_DEFS_

/*
 * netaddr_t is declared with the kernel files,
 * in sys/mach_ipc_netport.h.
 */
#if	NeXT
#include	<sys/mach_ipc_netport.h>
#else	NeXT
#include	<mach/netport.h>
#endif	NeXT

#ifdef	notdef
typedef unsigned long	netaddr_t;
#endif	notdef

typedef union {
    struct {
	unsigned char ia_net_owner;
	unsigned char ia_net_node_type;
	unsigned char ia_host_high;
	unsigned char ia_host_low;
    } ia_bytes;
    netaddr_t ia_netaddr;
} ip_addr_t;

extern short 			conf_own_format;

#endif	_NM_DEFS_
