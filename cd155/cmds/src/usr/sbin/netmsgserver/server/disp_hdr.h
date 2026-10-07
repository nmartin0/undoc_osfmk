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
 * disp_hdr.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/disp_hdr.h,v 1.2 1994/11/19 03:10:50 mtm Exp $
 *
 */

/*
 * Definitions for the header common to all network server messages.
 */


#ifndef	_DISP_HDR_
#define	_DISP_HDR_

#include <mach/boolean.h>
#include "config.h"


typedef struct {
    unsigned short	disp_type;	/* type of message (network format) */
    unsigned short	src_format;	/* format for all following data */
} disp_hdr_t, *disp_hdr_ptr_t;


/*
 * Version number for messages.
 */
#define	DISPATCHER_VERSION	(100 * 1)


/*
 * Values for disp_type. DISPE_* represents the index of the
 * entry in dispatcher_switch.
 */
#define DISPE_IPC_MSG		1
#define DISPE_IPC_UNBLOCK	2
#define DISPE_PORTCHECK		3
#define DISPE_NETNAME		4
#define DISPE_PO_RO_HINT	5
#define DISPE_PO_RO_XFER	6
#define DISPE_PO_TOKEN		7
#define DISPE_PO_DEATH		8
#define DISPE_PORTSEARCH	9
#define DISPE_PS_AUTH		10
#define DISPE_STARTUP		11
#define	DISPE_IPC_ABORT		12

#define DISP_TYPE_MAX		(12 + 1)

#define DISP_IPC_MSG		(DISPE_IPC_MSG		+ DISPATCHER_VERSION)
#define DISP_IPC_UNBLOCK	(DISPE_IPC_UNBLOCK	+ DISPATCHER_VERSION)
#define DISP_PORTCHECK		(DISPE_PORTCHECK	+ DISPATCHER_VERSION)
#define DISP_NETNAME		(DISPE_NETNAME		+ DISPATCHER_VERSION)
#define DISP_PO_RO_HINT		(DISPE_PO_RO_HINT	+ DISPATCHER_VERSION)
#define DISP_PO_RO_XFER		(DISPE_PO_RO_XFER	+ DISPATCHER_VERSION)
#define DISP_PO_TOKEN		(DISPE_PO_TOKEN		+ DISPATCHER_VERSION)
#define DISP_PO_DEATH		(DISPE_PO_DEATH		+ DISPATCHER_VERSION)
#define DISP_PORTSEARCH		(DISPE_PORTSEARCH	+ DISPATCHER_VERSION)
#define DISP_PS_AUTH		(DISPE_PS_AUTH		+ DISPATCHER_VERSION)
#define DISP_STARTUP		(DISPE_STARTUP		+ DISPATCHER_VERSION)
#define	DISP_IPC_ABORT		(DISPE_IPC_ABORT	+ DISPATCHER_VERSION)

/*
 * Table for byte-swapping requirements 
 */
extern boolean_t disp_swap_table[DISP_FMT_MAX][DISP_FMT_MAX];

/*
 * Check if byte-swapping is needed 
 */
#define	DISP_CHECK_SWAP(sf) (disp_swap_table[sf][CONF_OWN_FORMAT])

/*
 * Dispatcher return codes. These codes are in the same
 * space as the TR_* and IPC_* codes.
 */
#define	DISP_WILL_REPLY	0
#define DISP_FAILURE	-1
#define DISP_SUCCESS	-2
#define DISP_IGNORE	-3

#endif	_DISP_HDR_
