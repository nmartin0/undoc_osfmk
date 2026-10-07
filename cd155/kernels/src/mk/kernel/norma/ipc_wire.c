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
 * $Log: ipc_wire.c,v $
 * Revision 1.7  1994/11/18  20:56:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/22  02:21:02  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.5  1993/06/30  22:51:03  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:40:38  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:46:13  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:51:36  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_wire.c,v
 * Revision 1.1.4.2  1993/04/22  19:38:52  rod
 * 	vm_map_copyin_page_list() takes no_zero_fill arg.
 * 	[1993/04/12  17:43:11  rod]
 *
 * 	device_read_overwrite() support:
 * 		vm_map_copyin_page_list() takes protection argument.
 * 	[1993/02/19  11:53:07  rod]
 *
 * Revision 1.1  1992/09/30  02:13:35  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.1  92/01/21  21:53:23  jsb
 * 	First checkin. Contents moved here from norma/ipc_output.c.
 * 	[92/01/10  20:51:47  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
/*
 *	File:	norma/ipc_wire.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1992
 *
 *	Functions to wire and unwire pages for norma ipc.
 */

#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/vm_attributes.h>
#include <mach/vm_param.h>
#include <kern/assert.h>
#include <kern/zalloc.h>
#include <vm/vm_fault.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <ipc/ipc_port.h>

/*
 * For now, we just steal pages.
 * In the future, we should remember enough information for
 * norma_ipc_thread_return to either pmap_protect appropriate pages
 * or block the thread until the pages are released.
 *
 * I think we don't need a norma_ipc_copyin_page_list_cont; we should be
 * able to use vm_map_copyin_page_list_cont.
 */
kern_return_t
norma_ipc_copyin_page_list(map, addr, len, src_destroy, copy_result, is_cont)
	vm_map_t	map;
	vm_offset_t	addr;
	vm_size_t	len;
	boolean_t	src_destroy;
	vm_map_copy_t	*copy_result;	/* OUT */
	boolean_t	is_cont;
{
	return vm_map_copyin_page_list(map, addr, len, src_destroy, TRUE,
				       copy_result, is_cont, VM_PROT_READ,
				       FALSE);
}

/*
 * Called when a thread wants to return to user space after possibly having
 * performed a norma_ipc_copyin_page_list.
 *
 * Hooks need to be added somewhere around thread_syscall_return and/or
 * returns from mach_msg_trap, etc.
 *
 * For now, this routine can do nothing (and not be called) because
 * norma_ipc_copyin_page_list simply steals pages.
 */
norma_ipc_thread_return()
{
}
