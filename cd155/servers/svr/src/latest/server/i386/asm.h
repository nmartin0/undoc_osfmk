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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: asm.h,v $
 * Revision 1.4  1994/11/18  20:30:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:55:12  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:14:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:19  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:12:35  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:32:56  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/08/27  15:29:45  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.1  91/08/05  13:56:36  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  13:36:48  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:29:41  gm]
 * 
 * Revision 1.4  90/07/17  11:27:21  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:16:45  kevins]
 * 
 * 	Typo on ENTRY if gprof
 * 	[90/03/29            rvb]
 * 
 * 	fix SVC for "ifdef wheeze" [kupfer]
 * 	[90/02/28  15:47:31  rvb]
 * 
 * 	Fix the GPROF definitions.
 * 	 ENTRY(x) gets profiled iffdef GPROF.
 * 	 Entry(x) (and DATA(x)) is NEVER profiled.
 * 	 MCOUNT can be used by asm that intends to build a frame,
 * 	 after the frame is built.
 * 	[90/02/26            rvb]
 * 
 * 	Add #define addr16 .byte 0x67
 * 	[90/02/09            rvb]
 * 
 * 	Added LBi, SVC and ENTRY
 * 	[89/11/10  09:51:33  rvb]
 * 
 * 	New a.out and coff compatible .s files.
 * 	[89/10/16            rvb]
 * 
 * $EndLog$
 */


#define S_ARG0	 4(%esp)
#define S_ARG1	 8(%esp)
#define S_ARG2	12(%esp)
#define S_ARG3	16(%esp)

#define FRAME	pushl %ebp; movl %esp, %ebp
#define EMARF	leave

#define B_ARG0	 8(%ebp)
#define B_ARG1	12(%ebp)
#define B_ARG2	16(%ebp)
#define B_ARG3	20(%ebp)

#ifdef	wheeze

#define ALIGN 4
#define EXT(x) x
#define LEXT(x) x:
#define LCL(x) ./**/x

#define LB(x,n) ./**/x
#define LBb(x,n) ./**/x
#define LBf(x,n) ./**/x

#define	SVC lcall $7,$0

#define String .string
#define Value  .value
#define Times(a,b) [a\*b]
#define Divide(a,b) [a\\b]

#define INB	inb	(%dx)
#define OUTB	outb	(%dx)
#define INL	inl	(%dx)
#define OUTL	outl	(%dx)

#else	/* wheeze */
#define ALIGN
#define	LCL(x)	x

#define LB(x,n) n
#ifdef	__STDC__
#define EXT(x) _ ## x
#define LEXT(x) _ ## x ## :

#define LBb(x,n) n ## b
#define LBf(x,n) n ## f
#else	/* __STDC__ */
#define EXT(x) _/**/x
#define LEXT(x) _/**/x/**/:
#define LBb(x,n) n/**/b
#define LBf(x,n) n/**/f
#endif	/* __STDC__ */

#define SVC .byte 0x9a; .long 0; .word 0x7

#define String	.ascii
#define Value	.word
#define Times(a,b) (a*b)
#define Divide(a,b) (a/b)

#define INB	inb	%dx, %al
#define OUTB	outb	%al, %dx
#define INL	inl	%dx, %eax
#define OUTL	outl	%eax, %dx

#endif	/* wheeze */

#define data16	.byte 0x66
#define addr16	.byte 0x67



#ifdef GPROF
#ifdef	__STDC__

#define MCOUNT		.data; LB(x, 9); .long 0; \
			.text; lea LBb(x, 9),%edx; call mcount
#define	ENTRY(x)	.globl EXT(x); .align ALIGN; LEXT(x) ; \
			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;
#define	ASENTRY(x) 	.globl x; .align ALIGN; x ## : ; \
  			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;
#else	/* __STDC__ */

#define MCOUNT		.data; LB(x, 9): .long 0; \
			.text; lea LBb(x, 9),%edx; call mcount
#define	ENTRY(x)	.globl EXT(x); .align ALIGN; LEXT(x) ; \
			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;
#define	ASENTRY(x) 	.globl x; .align ALIGN; x: ; \
			pushl %ebp; movl %esp, %ebp; MCOUNT; popl %ebp;
#endif	/* __STDC__ */

#else	/* GPROF */
#ifdef	__STDC__

#define MCOUNT
#define	ENTRY(x)	.globl EXT(x); .align ALIGN; LEXT(x)
#define	ASENTRY(x)	.globl x; .align ALIGN; x ## :

#else	/* __STDC__ */

#define MCOUNT
#define	ENTRY(x)	.globl EXT(x); .align ALIGN; LEXT(x)
#define	ASENTRY(x)	.globl x; .align ALIGN; x:

#endif	/* __STDC__ */
#endif	/* GPROF */

#define	Entry(x)	.globl EXT(x); .align ALIGN; LEXT(x)
#define	DATA(x)		.globl EXT(x); .align ALIGN; LEXT(x)
