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
 * ipc_hdr.h 
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/ipc_hdr.h,v 1.2 1994/11/19 03:11:03 mtm Exp $ 
 *
 */

/*
 * Definitions for the network headers used by the IPC module. 
 */


#ifndef	_IPC_HDR_
#define	_IPC_HDR_

#include	"disp_hdr.h"
#include	"port_defs.h"

/*
 * Header for network IPC messages 
 */
typedef struct {
	disp_hdr_t      disp_hdr;	/* dispatcher header */
	network_port_t  local_port;
	network_port_t  remote_port;
	unsigned long   info;		/* info bits */
	unsigned long   npd_size;	/* size of Network Port Dictionary */
	unsigned long   inline_size;	/* size of inline part of message */
	unsigned long   ool_size;	/* size of ool part of message */
	unsigned long   ool_num;	/* number of ool sections (for assembly) */
	unsigned long	ipc_seq_no;	/* the IPC sequence number of this message */
}               ipc_netmsg_hdr_t;

/*
 * Bits for info field 
 */
#define IPC_INFO_SIMPLE		0x1
#define	IPC_INFO_RPC		0x2

#endif	_IPC_HDR_
