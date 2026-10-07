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
 * $Log: fork.cs,v $
 * Revision 1.5  1994/11/18  21:06:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:05:46  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:59:55  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:06:03  dleslie
 * First R1_0 release
 *
 * Revision 2.1.2.1  92/03/28  10:23:32  jeffreyh
 * 	Correcting a typo
 * 	[92/03/26            andyp]
 * 	Changes from MK71
 * 	[92/03/11  18:50:01  jeffreyh]
 * 
 *
 */
/*
 * fork() library call.
 *
 * 92/03/09  Steven J. Sears, Open Software Foundation
 */

#include "SYS.h"

#define SYS_fork	2

SYSCALL(fork)
	bte	r17,r0,0f	// BRIF parent (r17 == 0)
	addu	-16,sp,sp
	st.l	r1,0(sp)	// push return address
	call	_mach_init
	st.l	r17,4(sp)	// push return code

	ld.l	0(sp),r1	// pop r1
	ld.l	4(sp),r17	// pop r17
	addu	16,sp,sp	// reset stack
	or	r0,r0,r16	// return 0
0:
	RET			// pid = fork()
