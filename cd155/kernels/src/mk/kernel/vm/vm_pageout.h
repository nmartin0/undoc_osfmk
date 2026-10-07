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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Paging flow control (NORMA_VM).  Define pageout
 * reservation interfaces.  [alanl, sjs]
 * $Log: vm_pageout.h,v $
 * Revision 1.6  1994/11/18  21:04:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  23:02:13  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:43:16  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:56:18  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:21:22  dleslie
 * Patch release of April 23
 *
 * Revision 1.1  1992/09/30  02:37:29  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.4  91/05/14  17:51:11  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  18:00:18  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:34:35  mrt]
 * 
 * Revision 2.2  90/06/02  15:12:02  rpd
 * 	Added declaration of vm_pageout_setup.
 * 	[90/05/31            rpd]
 * 
 * 	Changes for MACH_KERNEL:
 * 	. Remove non-XP declarations.
 * 	[89/04/28            dbg]
 * 
 * Revision 2.1  89/08/03  16:45:59  rwd
 * Created.
 * 
 * Revision 2.9  89/04/18  21:28:02  mwyoung
 * 	No relevant history.
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 *	File:	vm/vm_pageout.h
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Declarations for the pageout daemon interface.
 */

#ifndef	_VM_VM_PAGEOUT_H_
#define _VM_VM_PAGEOUT_H_

#include <norma_vm.h>
#include <vm/vm_page.h>

/*
 *	Exported routines.
 */

extern vm_page_t vm_pageout_setup();
extern void vm_pageout_page();

#if	NORMA_VM
/*
 *	In NORMA systems, it is necessary to throttle
 *	pageouts between nodes.  Moreover, lock_request
 *	activity also falls in this category.
 */
#define	PAGEOUT_RESERVE_INTERNAL	(0)
#define	PAGEOUT_RESERVE_EXTERNAL	(1)
#define	PAGEOUT_RESERVE_LOCK_REQUEST	(2)
#define PAGEOUT_RESERVE_LOCAL		(3)
#define	PAGEOUT_RESERVE_MAX		(4)

extern boolean_t			vm_pageout_reserve();
#endif

#endif	_VM_VM_PAGEOUT_H_
