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
 * Copyright (c) 1991 Carnegie Mellon University
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
 * $Log: norma_special_ports.h,v $
 * Revision 1.7  1995/03/28  02:59:47  stans
 *  MAX_SPECIAL_ID reduced from 40 --> 15, must matche dipc_uid.h'.
 *
 *  Reviewer: lenb
 *  Risk: medium-high
 *  Benefit or PTS #: 12734
 *  Testing:
 *         Installed & booted kernel on systems: 'a5', XPS150 and testjig.
 *         WW10 sats
 *         Developer tests:
 *                 Verified console logging works for kernel printf()'s at
 *                 interrupt and non-interrupt levels.
 *
 * Revision 1.6  1995/02/04  01:03:28  stans
 *  Added norma_{s/g}et_conslog_port() to support remote MK console logging.
 *
 *  Reviewer:lenb/tnt
 *  Risk:low
 *  Benefit or PTS #:5249
 *  Testing:WW50 sat
 *  Module(s):
 *
 * Revision 1.5  1994/11/18  20:53:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:47:25  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:38:42  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:39:58  dleslie
 * First R1_0 release
 *
 * Revision 2.3.2.2  92/06/24  18:05:35  jeffreyh
 * 	Added NORMA_HOST_PAGING_PORT and related macros. 
 * 	[92/06/17            jeffreyh]
 * 
 * Revision 2.3.2.1  92/01/09  18:44:29  jsb
 * 	Define MAX_SPECIAL_KERNEL_ID.
 * 	[92/01/04  18:16:44  jsb]
 * 
 * Revision 2.3  91/12/13  13:47:36  jsb
 * 	Moved MAX_SPECIAL_ID here from ipc/ipc_node.h.
 * 
 * Revision 2.2  91/08/03  18:19:11  jsb
 * 	First checkin.
 * 	[91/07/25  07:52:36  jsb]
 * 
 */
/*
 *	File:	mach/norma_special_ports.h
 *
 *	Defines codes for remote access to special ports.  These are NOT
 *	port identifiers - they are only used for the norma_get_special_port
 *	and norma_set_special_port routines.
 */

#ifndef	_MACH_NORMA_SPECIAL_PORTS_H_
#define _MACH_NORMA_SPECIAL_PORTS_H_

#define	MAX_SPECIAL_KERNEL_ID	10
#define	MAX_SPECIAL_ID		15	/* must match norma2/dipc_uid.h */

/*
 * Provided by kernel
 */
#define NORMA_DEVICE_PORT	1
#define NORMA_HOST_PORT		2
#define NORMA_HOST_PRIV_PORT	3
#define NORMA_HOST_PAGING_PORT	4

/*
 * Not provided by kernel
 */
#define NORMA_NAMESERVER_PORT	(1 + MAX_SPECIAL_KERNEL_ID)
#define NORMA_CONSOLE_LOG_PORT	(2 + MAX_SPECIAL_KERNEL_ID)

/*
 * Definitions for ease of use.
 *
 * In the get call, the host parameter can be any host, but will generally
 * be the local node host port. In the set call, the host must the per-node
 * host port for the node being affected.
 */

#define norma_get_device_port(host, node, port)	\
	(norma_get_special_port((host), (node), NORMA_DEVICE_PORT, (port)))

#define norma_set_device_port(host, port)	\
	(norma_set_special_port((host), NORMA_DEVICE_PORT, (port)))

#define norma_get_host_port(host, node, port)	\
	(norma_get_special_port((host), (node), NORMA_HOST_PORT, (port)))

#define norma_set_host_port(host, port)	\
	(norma_set_special_port((host), NORMA_HOST_PORT, (port)))

#define norma_get_host_priv_port(host, node, port)	\
	(norma_get_special_port((host), (node), NORMA_HOST_PRIV_PORT, (port)))

#define norma_set_host_priv_port(host, port)	\
	(norma_set_special_port((host), NORMA_HOST_PRIV_PORT, (port)))

#define norma_get_host_paging_port(host, node, port)	\
	(norma_get_special_port((host), (node), NORMA_HOST_PAGING_PORT, (port)))

#define norma_set_host_paging_port(host, port)	\
	(norma_set_special_port((host), NORMA_HOST_PAGING_PORT, (port)))

#define norma_get_nameserver_port(host, node, port)	\
	(norma_get_special_port((host), (node), NORMA_NAMESERVER_PORT, (port)))

#define norma_set_nameserver_port(host, port)	\
	(norma_set_special_port((host), NORMA_NAMESERVER_PORT, (port)))

#define norma_get_conslog_port(host, node, port)	\
	(norma_get_special_port((host), (node), NORMA_CONSOLE_LOG_PORT, (port)))

#define norma_set_conslog_port(host, port)	\
	(norma_set_special_port((host), NORMA_CONSOLE_LOG_PORT, (port)))

#endif	_MACH_NORMA_SPECIAL_PORTS_H_
