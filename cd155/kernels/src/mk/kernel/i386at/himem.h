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
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * $Log: himem.h,v $
 * Revision 1.5  1994/11/18  20:35:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:26:42  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:25:32  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:25:17  dleslie
 * First R1_0 release
 *

/*
 * Copyright 1992 by Open Software Foundation,
 * Grenoble, FRANCE
 *
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OSF or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 *   OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _I386AT_HIMEM_H_
#define _I386AT_HIMEM_H_

/*
 * support of memory above 16 Megs for DMA limited to memory
 * below 16 Megs.
 */

#include <platforms.h>
#if	CBUS
#include <cbus/cbus.h>
#endif	CBUS

#define HIMEM_STATS 0

#if	HIMEM_STATS
extern int himem_request;
extern int himem_used;
#endif	HIMEM_STATS

struct himem_link {
	struct himem_link *next;
	vm_offset_t	high_addr;	/* physical address */
	vm_offset_t	low_page;	/* physical page */
	vm_size_t	length;
};
 
typedef struct himem_link *hil_t;	

#if	CBUS

	/* 
	 * For CBUS, consider the CBUS address space corresponding
	 * to CBUS_START+bios as high memory, since we cant DMA
	 * to it as well
	 */

#define HIGH_MEM	((vm_offset_t) 0xf00000 + CBUS_START)
#define	BIOS_START	((vm_offset_t) 0x0a0000 + CBUS_START)
#define	BIOS_END	((vm_offset_t) 0x100000 + CBUS_START)

#define _high_mem_page(x)		       \
	 ((vm_offset_t)(x) >= HIGH_MEM ||      \
	  ((vm_offset_t)(x) >= BIOS_START &&   \
	  (vm_offset_t)(x) < BIOS_END))
#else	CBUS

#define HIGH_MEM		((vm_offset_t) 0xf00000)

#define _high_mem_page(x)	((vm_offset_t)(x) >= HIGH_MEM)

#endif	CBUS

#if	HIMEM_STATS
#define high_mem_page(x) \
	(++himem_request && _high_mem_page(x) && ++himem_used)

#else	HIMEM_STATS
#define high_mem_page(x) 	_high_mem_page(x)
#endif	HIMEM_STATS



#endif _I386AT_HIMEM_H_



