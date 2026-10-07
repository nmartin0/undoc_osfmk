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
 *	Machine-dependent definitions for cpu identification.
 *
 */
#ifndef	_I860_CPU_NUMBER_H_
#define	_I860_CPU_NUMBER_H_

#include <platforms.h>

#ifdef	ASSEMBLER

#define	CPU_REG p0

/*
 * read node hardware status register and return cpu number, side effect is to
 * store mycpu_number in the privileged reg 'CPU_REG'.
 *
 * macro input:
 *	'a'	register to return cpu number in, no other registers are
 *	destroyed!
 * 	
 * Node status register (32-bits):
 *	bits 21 & 20 identify the CPU number
 *	if bit 20 set then cpu 0
 *	if bit 21 set then cpu 1
 * 	bit 21, if == 1 then CPU1, bit 21 == 0 if bit 20 == 1 for CPU0
 */

#define	HDWR_CPU_NUMBER(a)						\
	orh	h%LB_NODE_STATUS,r0,a;					\
	or	l%LB_NODE_STATUS,a,a;					\
	ldio.l  a,a;			/* read node status reg */	\
	shr	20,a,a;			/* bit 20 & 21 are cpu 0 & 1 */	\
	and	3,a,a;							\
	bc	cpunum_fail;		/* must be cpu1 or cpu0 */	\
	shr	1,a,a			/* bit 21 == 0 for cpu 0 & 1 for cpu1 */

/*
 * set my CPU number in a processor protected register. Required for
 *  cpu_number() to correctly operate!!
 *
 * inputs:
 *	mycpunumber	guess what.
 *
 * outputs:
 *	none.
 *
 * side effects:
 *	CPU_REG = cpu number.
 */

#define SET_MY_CPU_NUMBER(a) st.c a,CPU_REG	/* save for fast access */


/*
 * return MY cpu number
 *
 *	load CPU the fast way (out of processor priv reg)
 *
 * macro inputs:
 *	'a'	general purpose processor register to return cpu number in
 *
 * n.b. cpu_number() assumes that FAST_CPU_NUMBER() macro is 1 instruction
 */

#define FAST_CPU_NUMBER(a) ld.c CPU_REG,a

#else	/* !ASSEMBLER */

extern int	cpu_number();
#define master_cpu	(0)

#endif	/* !ASSEMBLER */

#endif	/* _I860_CPU_NUMBER_H_ */


