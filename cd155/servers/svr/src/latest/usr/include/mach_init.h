/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: mach_init.h,v $
 * Revision 1.3  1994/11/18  21:10:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:15:27  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:49  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:27  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:36:55  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:30:43  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:50:16  gm]
 * 
 * Revision 1.2  90/01/02  19:16:22  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/12/26  10:10:30  gm
 * 	Current version from CMU.
 * 	[89/12/23            gm]
 * 
 * Revision 1.3  89/06/13  16:45:00  mrt
 * 	Defined macros for thread_reply and made task_data be another
 * 	name for thread_reply, as task_data() is no longer exported from
 * 	the kernel.
 * 	[89/05/28            mrt]
 * 
 * 	Moved definitions of round_page and trunc_page to
 * 	here from mach/vm_param.h
 * 	[89/05/18            mrt]
 * 
 * Revision 1.2  89/05/05  18:45:39  mrt
 * 	Cleanup and change includes for Mach 2.5
 * 	[89/04/28            mrt]
 * 
 * $EndLog$
 */
/*
 *	Items provided by the Mach environment initialization.
 */

#ifndef	_MACH_INIT_H_
#define	_MACH_INIT_H_	1

#include <mach/mach_types.h>

/*
 *	Kernel-related ports; how a task/thread controls itself
 */

extern	port_t	task_self_;
extern	port_t	task_notify_;
extern  port_t	thread_reply_;
#define task_data_	thread_reply_

#define	task_self()	task_self_
#define	task_data()	thread_reply_
#define	thread_reply()	thread_reply_
#define	task_notify()	task_notify_

#define	current_task()	task_self()

/*
 *	Other important ports in the Mach user environment
 */

#define	NameServerPort	name_server_port	/* compatibility */

extern	port_t	name_server_port;
extern	port_t	environment_port;
extern	port_t	service_port;

/*
 *	Where these ports occur in the "mach_ports_register"
 *	collection... only servers or the runtime library need know.
 */

#if	MACH_INIT_SLOTS
#define	NAME_SERVER_SLOT	0
#define	ENVIRONMENT_SLOT	1
#define SERVICE_SLOT		2

#define	MACH_PORTS_SLOTS_USED	3

extern	port_array_t	mach_init_ports;
extern	unsigned int	mach_init_ports_count;
#endif	MACH_INIT_SLOTS

/*
 *	Globally interesting numbers
 */

extern	vm_size_t	vm_page_size;

#define round_page(x)	((((vm_offset_t)(x) + (vm_page_size - 1)) / vm_page_size) * vm_page_size)
#define trunc_page(x)	((((vm_offset_t)(x)) / vm_page_size) * vm_page_size)

#endif	_MACH_INIT_H_
