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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log: mach_msg.c,v $
 * Revision 1.5  1994/11/18  21:05:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:03:26  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:57:43  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:03:31  dleslie
 * First R1_0 release
 *
 * Revision 2.3.2.1  92/03/28  10:25:56  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:29:45  jeffreyh]
 * 
 * Revision 2.3  92/01/23  15:22:17  rpd
 * 	Fixed to not pass MACH_SEND_INTERRUPT and MACH_RCV_INTERRUPT
 * 	to the kernel.
 * 	[92/01/20            rpd]
 * 
 * Revision 2.2  92/01/15  17:17:13  rpd
 * 	Created from msg.c.
 * 	[92/01/15            rpd]
 * 
 */

#include <mach/port.h>
#include <mach/message.h>

#define LIBMACH_OPTIONS	(MACH_SEND_INTERRUPT|MACH_RCV_INTERRUPT)

mach_msg_return_t
mach_msg(msg, option, send_size, rcv_size, rcv_name, timeout, notify)
	mach_msg_header_t *msg;
	mach_msg_option_t option;
	mach_msg_size_t send_size;
	mach_msg_size_t rcv_size;
	mach_port_t rcv_name;
	mach_msg_timeout_t timeout;
	mach_port_t notify;
{
	mach_msg_return_t mr;

	/*
	 * Consider the following cases:
	 *	1) Errors in pseudo-receive (eg, MACH_SEND_INTERRUPTED
	 *	plus special bits).
	 *	2) Use of MACH_SEND_INTERRUPT/MACH_RCV_INTERRUPT options.
	 *	3) RPC calls with interruptions in one/both halves.
	 *
	 * We refrain from passing the option bits that we implement
	 * to the kernel.  This prevents their presence from inhibiting
	 * the kernel's fast paths (when it checks the option value).
	 */

	mr = mach_msg_trap(msg, option &~ LIBMACH_OPTIONS,
			   send_size, rcv_size, rcv_name,
			   timeout, notify);
	if (mr == MACH_MSG_SUCCESS)
		return MACH_MSG_SUCCESS;

	if ((option & MACH_SEND_INTERRUPT) == 0)
		while (mr == MACH_SEND_INTERRUPTED)
			mr = mach_msg_trap(msg,
				option &~ LIBMACH_OPTIONS,
				send_size, rcv_size, rcv_name,
				timeout, notify);

	if ((option & MACH_RCV_INTERRUPT) == 0)
		while (mr == MACH_RCV_INTERRUPTED)
			mr = mach_msg_trap(msg,
				option &~ (LIBMACH_OPTIONS|MACH_SEND_MSG),
				0, rcv_size, rcv_name,
				timeout, notify);

	return mr;
}
