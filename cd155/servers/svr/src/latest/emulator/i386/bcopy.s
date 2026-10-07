/*
 *
 *$Copyright
 *Copyright 1993, 1994 , 1995 Intel Corporation
 *INTEL CONFIDENTIAL
 *The technical data and computer software contained herein are subject
 *to the copyright notices; trademarks; and use and disclosure
 *restrictions identified in the file located in /etc/copyright on
 *this system.
 *Copyright$
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
 * $Log: bcopy.s,v $
 *Revision 1.5  1994/11/18  20:25:14  mtm
 *Copyright additions/changes
 *
 *Revision 1.4  1993/07/14  17:32:00  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 *Revision 1.1.1.3  1993/07/01  18:24:58  cfj
 *Adding new code from vendor
 *
 *Revision 1.3  1993/05/06  18:58:58  cfj
 *ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:18:42  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:09:30  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:29:58  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/03  23:58:15  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/12/16  12:43:22  roy
 * 	91/11/07  15:14:50  condict
 * 	Replaced the C version with this assembler version for > 2X speedup.
 *
 * 	91/02/05  17:10:49  mrt
 * 	Changed to new Mach copyright
 *
 * 	90/11/05  14:26:58  rpd
 * 	Introduce bcopy16.  For 16bit copies to bus memory
 *
 * 	90/05/03  15:24:53  dbg
 * 	From Bob Baron.
 *
 * 
 */

#include <i386/asm.h>

/ bcopy(from, to, bcount)

ENTRY(bcopy)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	movl	B_ARG0,%esi
	movl	B_ARG1,%edi
bcopy_common:
	movl	B_ARG2,%edx
	cld
/ move longs
	movl	%edx,%ecx
	sarl	$2,%ecx
	rep
	movsl
/ move bytes
	movl	%edx,%ecx
	andl	$3,%ecx
	rep
	movsb
	popl	%esi
	popl	%edi
	leave
	ret


/ memcpy(to, from, count)

ENTRY(memcpy)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	movl	B_ARG0,%edi
	movl	B_ARG1,%esi
	jmp	bcopy_common

/ bcopy16(from, to, bcount) using word moves

ENTRY(bcopy16)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
/	movl	8+12(%esp),%edx		/  8 for the two pushes above
/	movl	8+ 8(%esp),%edi
/	movl	8+ 4(%esp),%esi
	movl	B_ARG2, %edx
	movl	B_ARG1, %edi
	movl	B_ARG0, %esi
/ move words
0:	cld
	movl	%edx,%ecx
	sarl	$1,%ecx
	rep
	movsw
/ move bytes
	movl	%edx,%ecx
	andl	$1,%ecx
	rep
	movsb
	popl	%esi
	popl	%edi
	leave
	ret	

