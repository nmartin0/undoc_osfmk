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
 * $Log: xpr.h,v $
 * Revision 1.5  1994/11/18  20:32:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:02:30  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:22:16  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:17:38  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:07  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:37  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:31  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1992/03/09  16:07:21  durriya
 * 	initial check-in from grV3.5
 *
 *
 */
/*
 * Include file for xpr circular buffer silent tracing.  
 *
 */

/*
 * If the kernel flag XPRDEBUG is set, the XPR macro is enabled.  The 
 * macro should be invoked something like the following:
 *	XPR(XPR_SYSCALLS, ("syscall: %d, 0x%x\n", syscallno, arg1);
 * which will expand into the following code:
 *	if (xprflags & XPR_SYSCALLS)
 *		xpr("syscall: %d, 0x%x\n", syscallno, arg1);
 * Xpr will log the pointer to the printf string and up to 6 arguements,
 * along with a timestamp and cpuinfo (for multi-processor systems), into
 * a circular buffer.  The actual printf processing is delayed until after
 * the buffer has been collected.  It is assumed that the text/data segments
 * of the kernel can easily be reconstructed in a post-processor which
 * performs the printf processing.
 *
 * If the XPRDEBUG compilation switch is not set, the XPR macro expands 
 * to nothing.
 */

#ifndef	_KERN_XPR_H_
#define _KERN_XPR_H_

#include <sys/synch.h>
#include <xpr_debug.h>


#if	XPR_DEBUG

#ifndef	ASSEMBLER

extern unsigned int xprflags;
#define XPR(flags,xprargs) if(xprflags&flags) xpr xprargs

#endif	ASSEMBLER

/*
 * flags for message types.
 */
#define XPR_SYSCALLS		(1 << 0)
#define XPR_TRAPS		(1 << 1)
#define XPR_SCHED		(1 << 2)
#define XPR_NPTCP		(1 << 3)
#define XPR_NP			(1 << 4)
#define XPR_TCP			(1 << 5)

#define XPR_VM_OBJECT		(1 << 8)
#define XPR_VM_OBJECT_CACHE	(1 << 9)
#define XPR_VM_PAGE		(1 << 10)
#define XPR_VM_PAGEOUT		(1 << 11)
#define XPR_MEMORY_OBJECT	(1 << 12)
#define XPR_VM_FAULT		(1 << 13)
#define XPR_INODE_PAGER		(1 << 14)
#define XPR_INODE_PAGER_DATA	(1 << 15)

#else	XPR_DEBUG
#define XPR(flags,xprargs)
#endif	XPR_DEBUG

#ifndef	ASSEMBLER
struct xprbuf {
	char 	*msg;
	int	arg1,arg2,arg3,arg4,arg5;
	int	thread;
};
#endif	ASSEMBLER
#endif	_KERN_XPR_H_
