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
 * $Log: ipc_sched.h,v $
 * Revision 1.6  1994/11/18  20:52:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/27  21:30:23  andyp
 * Setting timeouts of somewhere over 11 hours when HZ==100 demonstrated
 * a 32-bit arithmetic problem with the milliseconds-to-ticks macro.
 * Rearranged the log to match OSF's.
 *
 * Revision 1.4  1993/06/30  22:44:49  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:35:44  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:35:49  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_sched.h,v
 * Revision 1.2  1992/11/25  01:10:38  robert
 * 	integrate changes below for norma_14
 *
 * 	Philippe Bernadat (bernadat) at gr.osf.org
 * 	Added convert_ipc_timeout_to_ticks from MK73
 * 	[1992/11/13  19:33:04  robert]
 *
 * Revision 1.1  1992/09/30  02:29:42  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.6  91/05/14  16:42:41  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/03/16  14:50:17  rpd
 * 	Replaced ipc_thread_switch with thread_handoff.
 * 	Renamed ipc_thread_{go,will_wait,will_wait_with_timeout}
 * 	to thread_{go,will_wait,will_wait_with_timeout}.
 * 	Removed ipc_thread_block.
 * 	[91/02/17            rpd]
 * 
 * Revision 2.4  91/02/05  17:26:58  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:13:32  mrt]
 * 
 * Revision 2.3  91/01/08  15:15:58  rpd
 * 	Removed ipc_thread_go_and_block.
 * 	Added ipc_thread_switch.
 * 	Added continuation argument to ipc_thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.2  90/06/02  14:54:27  rpd
 * 	Created for new IPC.
 * 	[90/03/26  23:46:16  rpd]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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

#ifndef	_KERN_IPC_SCHED_H_
#define	_KERN_IPC_SCHED_H_

#include <mach/boolean.h>
#include <kern/macro_help.h>

extern void
thread_go(/* thread_t */);

extern void
thread_will_wait(/* thread_t */);

extern void
thread_will_wait_with_timeout(/* thread_t, mach_msg_timeout_t */);

extern boolean_t
thread_handoff(/* thread_t, void (*)(), thread_t */);

/*
 *	Convert a timeout in milliseconds (mach_msg_timeout_t)
 *	to a timeout in ticks (for use by set_timeout).
 *	This conversion rounds UP so that small timeouts
 *	at least wait for one tick instead of not waiting at all.
 */

#define convert_ipc_timeout_to_ticks(millis)	\
		(((millis) + ((1000 / hz) - 1)) / (1000 / hz))

#endif	_KERN_IPC_SCHED_H_
