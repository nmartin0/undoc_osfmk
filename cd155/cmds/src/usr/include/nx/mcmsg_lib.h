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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  Intel Corporation.
 *
 *
 *  	$header$
 *
 */

/*
 * mcmsg_lib.h
 *
 * User library globals
 */

#ifndef MCMSG_LIB_H
#define MCMSG_LIB_H

#ifndef MCMSG_LIB_EXT
#define MCMSG_LIB_EXT		extern
#endif  MCMSG_LIB_EXT


MCMSG_LIB_EXT long		msginfo[8];

#define	msginfo_type		msginfo[0]
#define	msginfo_count		msginfo[1]
#define	msginfo_node		msginfo[2]
#define	msginfo_ptype		msginfo[3]

MCMSG_LIB_EXT volatile int	_nx_lock;
MCMSG_LIB_EXT volatile int	_masktrap_lock;

MCMSG_LIB_EXT int		mcmsg_msgp;

MCMSG_LIB_EXT unsigned long	mcmsg_mynode;
MCMSG_LIB_EXT unsigned long	mcmsg_numnodes;
MCMSG_LIB_EXT long		mcmsg_app;
MCMSG_LIB_EXT long		mcmsg_pid;
MCMSG_LIB_EXT long		mcmsg_ptype;

MCMSG_LIB_EXT unsigned long	mcmsg_pkt_size;
MCMSG_LIB_EXT unsigned long	mcmsg_memory_each;
MCMSG_LIB_EXT unsigned long	mcmsg_send_threshold;
MCMSG_LIB_EXT unsigned long	mcmsg_send_count;
MCMSG_LIB_EXT unsigned long	mcmsg_give_threshold;
MCMSG_LIB_EXT unsigned long	mcmsg_provide_threshold;
/* moved to nx_port.c MCMSG_LIB_EXT long	mcmsg_process_lock; */
extern long mcmsg_process_lock;

MCMSG_LIB_EXT unsigned long	mcmsg_addrspace;
MCMSG_LIB_EXT unsigned long	mcmsg_addrsize;

MCMSG_LIB_EXT xmsg_t		*mcmsg_buffer;
MCMSG_LIB_EXT xmsg_t		*mcmsg_buffer_end;
MCMSG_LIB_EXT unsigned long	mcmsg_buffer_size;
MCMSG_LIB_EXT unsigned long	mcmsg_provide_size;

extern xmsg_t			*mcmsg_free;

MCMSG_LIB_EXT unsigned long	mcmsg_provide_bytes;

MCMSG_LIB_EXT xmsg_t		*mcmsg_incoming_head;
MCMSG_LIB_EXT xmsg_t		*mcmsg_incoming_middle;
MCMSG_LIB_EXT xmsg_t		*mcmsg_outgoing_head;
MCMSG_LIB_EXT xmsg_t		*mcmsg_outgoing_tail;

#endif MCMSG_LIB_H
