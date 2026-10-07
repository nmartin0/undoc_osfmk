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
 * SSD HISTORY
 * $Log: ipc_node.h,v $
 * Revision 1.7  1994/11/18  20:55:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/09/27  16:00:15  stans
 * node_is_valid
 *
 * Revision 1.5  1993/07/22  02:20:38  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:50:40  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:45:53  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:51:15  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_node.h,v
 * Revision 1.2  1992/11/25  01:14:20  robert
 * 	fix history
 * 	[1992/11/09  21:45:24  robert]
 *
 * 	integrate changes below for norma_14
 *
 * 	15-Oct-92 Alan Langerman (alan) at Open Software Foundation
 * 	Add IP_NORMA_FAKE_NODE as a distinguished value indicating
 * 	an impossible node number.
 * 	[1992/11/09  16:45:45  robert]
 *
 * Revision 1.1  1992/11/05  20:59:39  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.3.2.4  92/09/15  17:34:09  jeffreyh
 * 	Moved norma_port_tabled macro here from ipc_list.c
 * 	[92/08/06            dlb]
 * 
 * 	Moved node_is_valid test here from ipc_special.c
 * 	[92/07/23            jeffreyh]
 * 
 * Revision 2.3.2.3  92/04/08  15:45:58  jeffreyh
 * 	Add IP_NORMA_MAX_NODE.
 * 	[92/04/07            dlb]
 * 
 * 	Define IP_NORMA_NULL and IP_NORMA_DEAD.  Latter steals one
 * 	value from IP_NORMA_MAX_LID.
 * 	[92/04/02            dlb]
 * 
 * Revision 2.3.2.2  92/01/09  18:45:45  jsb
 * 	Use MAX_SPECIAL_KERNEL_ID, not MAX_SPECIAL_ID, in IP_NORMA_SPECIAL.
 * 	[92/01/04  18:26:48  jsb]
 * 
 * Revision 2.3.2.1  92/01/03  16:37:54  jsb
 * 	Added IP_NORMA_{MAX_LID,SPECIAL} macros, and declaration
 * 	of host_special_port array.
 * 	[91/12/24  14:25:35  jsb]
 * 
 * Revision 2.3  91/12/13  14:00:49  jsb
 * 	Moved MAX_SPECIAL_ID to mach/norma_special_ports.h.
 * 
 * Revision 2.2  91/11/14  16:46:02  rpd
 * 	Created.
 * 
 */
/* CMU_ENDHIST */
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
 */
/*
 *	File:	norma/ipc_node.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for norma nodes.
 */

#ifndef	_NORMA_IPC_NODE_H_
#define	_NORMA_IPC_NODE_H_

#include <norma_ether.h>
#include <mach/norma_special_ports.h>

/*
 * A uid (unique id) = node + lid (local id)
 * We embed the node id in the uid because receive rights rarely move.
 */

/*
 * 12 bits for node		-> max 4096 nodes
 * 20 bits for local id		-> max 1,048,575 exported ports per node
 *
 * (Byte allignment and intuitive node:lid placement aids debugging)
 */
#define	IP_NORMA_NODE(uid)	(((unsigned long)(uid) >> 20) & 0x00000fff)
#define	IP_NORMA_LID(uid)	((unsigned long)(uid) & 0x000fffff)
#define	IP_NORMA_UID(node, lid)	(((node) << 20) | (lid))

#define	IP_NORMA_MAX_LID	IP_NORMA_LID(0xfffffffe)
#define	IP_NORMA_SPECIAL(uid)	(IP_NORMA_LID(uid) > 0 && \
				 IP_NORMA_LID(uid) <= MAX_SPECIAL_KERNEL_ID)

#define IP_NORMA_MAX_NODE	(0xfff)
#define IP_NORMA_FAKE_NODE	(0xffffffff)

extern ipc_port_t		host_special_port[MAX_SPECIAL_ID];

/*
 * Bounds checking macro for nodes.
 */
#ifdef	node_is_valid
#undef	node_is_valid
#endif

#if	NORMA_ETHER
#define node_is_valid(n)	(netipc_node_valid(n))
#endif

#ifdef  PARAGON860
#define node_is_valid(n)	(norma_node_status_op(0,n))
#endif

#ifndef node_is_valid
#define node_is_valid(n)	(n < IP_NORMA_MAX_NODE)
#endif

/*
 *	Reserved values for passing NULL and DEAD ports
 */

#define IP_NORMA_NULL		0
#define IP_NORMA_DEAD		IP_NORMA_LID(0xffffffff)

/*
 *	Macro to determine if port is in norma port table
 */
#define	norma_port_tabled(port)	((port)->ip_norma_next != (port))

#endif	_NORMA_IPC_NODE_H_
