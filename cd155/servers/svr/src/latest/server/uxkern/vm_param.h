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
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: vm_param.h,v $
 * Revision 1.5  1994/11/18  20:49:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:45:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:07:21  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:34:29  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:16  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1991/12/13  10:24:10  roy
 * 	91/11/30  17:03:34  roy
 * 	Include import_mach.h at the top.
 *
 * 	91/11/19  10:07:32  roy
 * 	Integrated atop/ptoa and different page_mask/page_shift logic.
 *
 * Revision 2.3  91/10/15  10:10:12  roy
 * Fix revision history comments.
 * 
 * Revision 2.2  91/10/14  13:25:05  sjs
 * 	Initial revision.
 * 
 * Revision 3.0  91/09/13  12:47:57  sp
 * new file to hold all things formally kept in mach/vmparam.h
 * 
 * $EndLog$
 */

#ifndef	_UXKERN_VM_PARAM_H_
#define _UXKERN_VM_PARAM_H_

#include <machine/vmparam.h>
#include <uxkern/import_mach.h>

#define PAGE_SIZE	vm_page_size	/* size of page in addressible units */

extern	vm_size_t	page_mask;
extern	int		page_shift;

/*
 *	Convert addresses to pages and vice versa.
 *	No rounding is used.
 */

#define atop(x)		(((unsigned)(x)) >> page_shift)
#define ptoa(x)		((vm_offset_t)((x) << page_shift))

#define page_aligned(x)	((((vm_offset_t) (x)) & page_mask) == 0)

/*
 * The following stuff provides compatability for people using vm_pageable
 * after CMU so considerately removed it.
 */
extern mach_port_t privileged_host_port;

#define	vm_pageable(task, address, size, access)	vm_wire(privileged_host_port, task, address, size, access)

#endif	_UXKERN_VM_PARAM_H_
