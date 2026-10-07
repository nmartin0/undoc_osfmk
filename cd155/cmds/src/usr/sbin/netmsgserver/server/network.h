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
 * network.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/network.h,v 1.2 1994/11/19 03:11:34 mtm Exp $
 *
 */

/*
 * Random network-level definitions and externs.
 */


#ifndef	_NETWORK_
#define	_NETWORK_

#include <mach/boolean.h>
#include "nm_defs.h"

#define HOST_NAME_SIZE	40
extern char			my_host_name[HOST_NAME_SIZE];
extern netaddr_t		my_host_id;
extern netaddr_t		broadcast_address;
extern short			last_ip_id;

extern boolean_t network_init();

extern int udp_checksum();

#endif	_NETWORK_
