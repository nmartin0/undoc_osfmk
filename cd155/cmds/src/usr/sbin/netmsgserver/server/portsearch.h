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
 * portsearch.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/portsearch.h,v 1.2 1994/11/19 03:12:02 mtm Exp $
 *
 */

/*
 * External definitions for the port search module
 */


#ifndef	_PORTSEARCH_
#define	_PORTSEARCH_

#include <mach/boolean.h>


extern void ps_do_port_search();
/*
port_rec_ptr_t		port_rec_ptr;
boolean_t		new_information;
network_port_ptr_t	new_nport_ptr;
int			(*retry)();
*/

extern boolean_t ps_init();
/*
*/

#endif	_PORTSEARCH_
