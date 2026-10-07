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
 * Copyright (c) 1991 Carnegie Mellon University
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
 * $Log: mig_allocate.c,v $
 * Revision 1.5  1994/11/18  21:05:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:03:39  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:57:59  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:03:53  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.1  92/03/28  10:28:17  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:31:45  jeffreyh]
 * 
 * Revision 2.2  92/01/03  20:35:42  dbg
 * 	Created.
 * 	[91/09/21            dbg]
 * 
 */

/*
 * Memory allocation routine for MiG interfaces.
 */
#include <mach.h>

void
mig_allocate(addr_p, size)
	vm_address_t	*addr_p;
	vm_size_t	size;
{
	if (vm_allocate(mach_task_self(),
			addr_p,
			size,
			TRUE)
	    != KERN_SUCCESS)
		*addr_p = 0;
}
