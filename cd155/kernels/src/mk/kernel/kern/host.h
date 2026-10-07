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
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * $Log: host.h,v $
 * Revision 1.7  1994/11/18  20:52:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/07/13  01:21:51  andyp
 * Late changes from the NORMA2 branch.
 *
 * Revision 1.5  1994/07/12  19:23:12  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.4.8.2  1994/07/12  21:05:08  stans
 *   'host_paging_self' now under NORMA_TASK instead of NORMA_{VM/IPC}; ASVM prep.
 *
 * Revision 1.4.8.1  1994/05/13  22:59:13  stans
 *   Changed NORMA_IPC --> NORMA_VM
 *
 * Revision 1.4  1993/06/30  22:44:34  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:35:29  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:35:16  dleslie
 * First R1_0 release
 *
 * Revision 2.4.2.1  92/06/24  18:05:04  jeffreyh
 * 	Added host_paging_self field to host structure
 * 	[92/06/17            jeffreyh]
 * 
 * Revision 2.4  91/05/14  16:41:25  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:26:20  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:12:24  mrt]
 * 
 * Revision 2.2  90/06/02  14:53:53  rpd
 * 	Created for new host/processor technology.
 * 	[90/03/26  23:50:40  rpd]
 * 
 * 	Cleanup changes.
 * 	[89/08/02            dlb]
 * 	extern realhost declaration.
 * 	[89/02/03            dlb]
 * 
 * Revision 2.3  89/10/15  02:04:17  rpd
 * 	Minor cleanups.
 * 
 * Revision 2.2  89/10/11  14:05:08  dlb
 * 	Cleanup.
 * 	[89/08/02            dlb]
 * 
 */

/*
 *	kern/host.h
 *
 *	Definitions for host data structures.
 *
 */

#ifndef	_KERN_HOST_H_
#define _KERN_HOST_H_

#ifdef KERNEL
#include <norma_task.h>
#endif

struct	host {
    	struct ipc_port *host_self;
	struct ipc_port *host_priv_self;
#if NORMA_TASK
	struct ipc_port *host_paging_self;
#endif
};

typedef struct host	*host_t;
typedef struct host	host_data_t;

#define HOST_NULL	((host_t)0)

extern host_data_t	realhost;

#endif	_KERN_HOST_H_
