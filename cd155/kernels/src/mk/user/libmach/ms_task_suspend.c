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
 * $Log: ms_task_suspend.c,v $
 * Revision 1.5  1994/11/18  21:05:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:04:15  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:58:34  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:04:35  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.1  92/03/28  10:32:34  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:33:40  jeffreyh]
 * 
 * Revision 2.2  92/01/03  20:36:25  dbg
 * 	Split into individual files so that user can substitute for any
 * 	one routine (e.g. vm_allocate).
 * 	[91/09/05            dbg]
 * 
 * Revision 2.8  91/06/25  10:34:48  rpd
 * 	Check against MACH_SEND_INTERRUPTED instead of KERN_SUCCESS.
 * 	[91/05/20            rpd]
 * 
 * Revision 2.7  91/05/14  17:53:28  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/14  14:17:54  mrt
 * 	Added new Mach copyright
 * 	[91/02/13  12:44:29  mrt]
 * 
 * Revision 2.5  90/09/09  14:34:31  rpd
 * 	Added mach_port_allocate_name since it didn't exist because of
 * 	the makefile sed.
 * 	[90/09/03            rwd]
 * 
 * Revision 2.4  90/06/19  23:03:50  rpd
 * 	Added mach_port_allocate, mach_port_deallocate, mach_port_insert_right.
 * 	[90/06/02            rpd]
 * 
 * Revision 2.3  90/06/02  15:12:36  rpd
 * 	Try kernel call whenever syscall fails.
 * 	[90/05/31            rpd]
 * 
 * 	Updated for new IPC.
 * 	Removed vm_allocate_with_pager.
 * 	[90/05/31            rpd]
 * 
 * Revision 2.2  90/05/29  18:40:03  rwd
 * 	New file from rfr to try traps then mig.
 * 	[90/04/20            rwd]
 * 
 */

#include <mach/mach.h>
#include <mach/message.h>

kern_return_t task_suspend(target_task)
	task_t target_task;
{
	kern_return_t result;

	result = syscall_task_suspend(target_task);
	if (result == MACH_SEND_INTERRUPTED)
		result = mig_task_suspend(target_task);
	return(result);
}

