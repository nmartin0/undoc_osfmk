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
 * nm_extra.c
 *
 *
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: nm_extra.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:11:36 $";
#endif

/*
 * Things that do not belong anywhere else!
 */


#define NM_EXTRA_DEBUG	0

#include <mach.h>
#include <stdio.h>
#include <mach/message.h>

#include "config.h"
#include "debug.h"
#include "netmsg.h"
#include "nm_defs.h"
#include <pthread.h>
#include <sys/errno.h>


/*
 * Tracing values.
 */
int		trace_recursion_level = 0;



/*
 * ipaddr_to_string
 *	Place an IP address into a string.
 *
 * Parameters:
 *	output_string	: the string to use
 *	input_address	: the address to used
 *
 */
EXPORT void ipaddr_to_string(output_string, input_address)
char		*output_string;
netaddr_t	input_address;
BEGIN("ipaddr_to_string")
    ip_addr_t	ip_address;

    ip_address.ia_netaddr = input_address;
    (void)sprintf(output_string, "%d.%d.%d.%d",
		ip_address.ia_bytes.ia_net_owner,
		ip_address.ia_bytes.ia_net_node_type,
		ip_address.ia_bytes.ia_host_high,
		ip_address.ia_bytes.ia_host_low);
    RET;

END



#if	LOCK_THREADS
/*
 * netmsg_receive_locked
 *	Does a non-blocking and unlocked receive on the local port contained in the message header.
 *
 * Parameters:
 *	msg_ptr		: pointer to a message buffer for the receive operation.
 *
 * Results:
 *	Whatever msg_receive returns.
 *
 * Note:
 *	Should only be called if lock_threads is turned on.
 *
 */
EXPORT netmsg_receive_locked(msg_ptr)
msg_header_t		*msg_ptr;
BEGIN("netmsg_receive_locked")
	register kern_return_t	mr;

	pthread_mutex_unlock(&thread_lock);
	mr = msg_receive(msg_ptr, MSG_OPTION_NONE, 0);
	pthread_mutex_lock(&thread_lock);
	RETURN(mr);

END
#endif	LOCK_THREADS
