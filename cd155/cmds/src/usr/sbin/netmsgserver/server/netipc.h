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
 * netipc.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/netipc.h,v 1.2 1994/11/19 03:11:30 mtm Exp $
 *
 */

/*
 * Definitions for IPC interface to the network.
 */


#ifndef	_NETIPC_
#define	_NETIPC_

#include <mach/message.h>
#include <machine/endian.h>
#include <sys/types.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include "crypt.h"

typedef struct {
    msg_header_t	nih_msg_header;
    struct ip		nih_ip_header;
    struct udphdr	nih_udp_header;
    crypt_header_t	nih_crypt_header;
} netipc_header_t, *netipc_header_ptr_t;

#define NETIPC_MAX_PACKET_SIZE		(1500)	/* Should be ETHERMTU???? */
#define NETIPC_PACKET_HEADER_SIZE	(sizeof(struct ip) + sizeof(struct udphdr) + CRYPT_HEADER_SIZE)
#define NETIPC_MAX_DATA_SIZE		(NETIPC_MAX_PACKET_SIZE - NETIPC_PACKET_HEADER_SIZE)
#define NETIPC_SWAPPED_HEADER_SIZE	(sizeof(struct udphdr) + CRYPT_HEADER_SIZE)
#define NETIPC_MAX_MSG_SIZE		(NETIPC_MAX_PACKET_SIZE + sizeof(msg_header_t) + 8)

typedef struct {
    netipc_header_t	ni_header;
    char		ni_data[NETIPC_MAX_DATA_SIZE];
} netipc_t, *netipc_ptr_t;

#define NETIPC_MSG_ID	1959

/*
 * Functions exported by netipc.c
 */

extern netipc_receive();
/*
netipc_header_ptr_t	pkt_ptr;
*/


extern netipc_send();
/*
netipc_header_ptr_t	pkt_ptr;
*/

#define NETIPC_BAD_UDP_CHECKSUM	-1

#endif	_NETIPC_
