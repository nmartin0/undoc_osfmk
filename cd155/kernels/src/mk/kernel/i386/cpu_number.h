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
 * HISTORY
 * $Log: cpu_number.h,v $
 * Revision 1.5  1994/11/18  20:32:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:23:16  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:21:53  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:21:24  dleslie
 * First R1_0 release
 *
 * Revision 2.3.9.2  92/04/30  11:49:49  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.3.9.1  92/02/18  18:43:57  jeffreyh
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.3  91/05/14  16:04:27  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:30:54  dbg
 * 	Created.
 * 	[91/03/21            dbg]
 * 
 */

/*
 *	Machine-dependent definitions for cpu identification.
 *
 */
#ifndef	_I386_CPU_NUMBER_H_
#define	_I386_CPU_NUMBER_H_

#include <platforms.h>

#if	AT386
/*
 * At least one corollary cpu type does not have local memory at all.
 * The only way I found to store the cpu number was in some 386/486
 * system register. cr3 has bits 0, 1, 2 and 5, 6, 7, 8, 9, 10, 11
 * available. Right now we use 0, 1 and 2. So we are limited to 8 cpus.
 * For more cpus, we could use bits 5 - 11 with a shift.
 *
 * Even for other machines, like COMPAQ this is much faster the inb/outb
 * 4 cycles instead of 10 to 30.
 */
#if	defined(__GNUC__)
#define cpu_number() 			\
    ({	register int _ret;		\
	asm ("	movl %%cr3, %0;		\
		andl $0x7, %0"		\
		    : "=r" (_ret)	\
		    : "0" (_ret)	\
		    );			\
	_ret;				\
    })

#else	defined(__GNUC__)
extern	cpu_number();
#endif	defined(__GNUC__)
#else	AT386
extern int	cpu_number();
#endif	AT386

#endif	/* _I386_CPU_NUMBER_H_ */


