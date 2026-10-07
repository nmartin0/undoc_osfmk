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
 * Copyright 1990 by Open Software Foundation,
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

/*
 * HISTORY
 * 28-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Moved DMA defs to i386at/eisa.h
 *
 * $Log: mbus.h,v $
 * Revision 1.5  1994/11/18  20:55:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:49:28  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:44:42  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:46:31  dleslie
 * First R1_0 release
 *
 * Revision 2.1.3.2  92/06/24  18:00:13  jeffreyh
 * 	Fix to be able to compile STD+COMPAQ (Single cpu)
 * 	[92/06/09            bernadat]
 * 
 * 	Changes for new at386_io_lock() interface
 * 	[92/06/03            bernadat]
 * 
 * Revision 2.1.3.1  92/04/30  11:59:56  bernadat
 * 	Code for MBUS based machines: SystemPro
 * 	[92/04/08            bernadat]
 * 
*/

#ifndef _MBUS_MBUS_H_
#define _MBUS_MBUS_H_

#include <cpus.h>
#include <i386at/mp/mp.h>

/* I/O ports */

#define	P1_ID	0x0c82		/* ID word, processor type */
#define P2_ID	0xfc82

#define	MBUS_386 	0x50	/* Processor type */
#define MBUS_486	0x59

#define P1_CTRL	0x0c6a		/* Control word */
#define P2_CTRL 0xfc6a

#define P2_VECT	0xfc68

/* Control word bits */

#define MBUS_INT_DIS	0x80	/* disable interrupts */
#define MBUS_INT_SET	0x40	/* Raise inter cpu interrupt */
#define	MBUS_387_ERR	0x20	/* floating point error */
#define	MBUS_FLUSH	0x10	/* Flush cache */
#define	MBUS_STOP	0x08	/* disable mbus access */
#define MBUS_CACHE_EN	0x04	/* Enable cache */
#define MBUS_387_INST	0x02	/* 387 installed  Read only */
#define MBUS_RESET	0x01	/* resets P2 */

#define	P2_RESET_VECT	0x467	/* Contains vector where P2 jumps at reset */

#define	MBUS_BIOS_REMAP_START	0x0fe0000	/* BIOS ROM is remapped here */
#define	MBUS_BIOS_REMAP_END	0x1000000	/* must not use this space   */

/*
 * Cpu number, no local memory on CPUS, this is a hack, limited to 8 cpus,
 * on the 486, bits 0, 1, 2 and 5, 6, 7, 8, 9, 10, 11 are free. We use bits
 * 0, 1 and 2 If we want to use more cpus, we will have to use bits 5-11
 * with shifts.
 */

#define	CPU_NUMBER(r) \
	movl	%cr3, r ; \
	andl	$0x7, r

#define	MBUS_IPL			SPL6	/* software interrupt level */

#ifndef	ASSEMBLER

#if	NCPUS > 1

/*
 * Structures to implement device_locks
 * for device drivers. See at386_io_lock, at386_io_unlock.
 */

#include <kern/thread.h>

struct mp_dev_lock {
	struct slock	lock;
	boolean_t	pending_ops;	/* pending dev ops */
	int		(*op[MP_DEV_OP_MAX])();	
					/* device op routines */
	int		unit;		/* needed as an argument to func */
	thread_t	holder;		/* thread holding the device */
	unsigned int	count;		/* this is a recursive lock */
};

#define at386_io_lock_state()

#endif	NCPUS > 1

#endif	ASSEMBLER
#endif _MBUS_MBUS_H_
