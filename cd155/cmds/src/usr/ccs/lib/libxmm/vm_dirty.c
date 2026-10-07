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
 * @OSF_FREE_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: vm_dirty.c,v $
 * Revision 1.2  1994/11/19  02:38:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/23  01:03:34  stans
 * Initial revision
 *
 * Revision 1.2  1992/05/12  14:49:46  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/04  08:02:10  condict]
 *
 * Revision 2.3  92/01/22  22:53:57  rpd
 * 	Fixed includes to use "" when appropriate.
 * 	[92/01/18            rpd]
 * 
 * Revision 2.2  91/07/06  15:11:26  jsb
 * 	First checkin.
 * 
 * $EndLog$
 */
/*
 *	File:	vm_dirty.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Routines for vm allocation and deallocation.
 */

#include <mach.h>
#include "vm_dirty.h"

#if 0
static vm_offset_t dirty_page = 0;

vm_allocate_dirty(page)
	vm_offset_t *page;
{
	if (dirty_page) {
		*page = dirty_page;
		dirty_page = 0;
		return KERN_SUCCESS;
	} else {
		return vm_allocate(mach_task_self(), page, vm_page_size, TRUE);
	}
}

vm_deallocate_dirty(page)
	vm_offset_t page;
{
	if (dirty_page == 0) {
		dirty_page = page;
		return KERN_SUCCESS;
	} else {
		return vm_deallocate(mach_task_self(), page, vm_page_size);
	}
}
#else
vm_allocate_dirty(page)
	vm_offset_t *page;
{
	kern_return_t kr;

	kr = vm_allocate(mach_task_self(), page, vm_page_size, TRUE);
	if (kr != KERN_SUCCESS) {
		panic("vm_allocate_dirty: %x/%d", kr, kr);
	}
	return KERN_SUCCESS;
}

vm_deallocate_dirty(page)
	vm_offset_t page;
{
	kern_return_t kr;

	kr = vm_deallocate(mach_task_self(), page, vm_page_size);
	if (kr != KERN_SUCCESS) {
		panic("vm_deallocate_dirty: %x/%d", kr, kr);
	}
	return KERN_SUCCESS;
}
#endif
