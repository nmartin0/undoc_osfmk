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
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/*
 * HISTORY
 * $Log: second_syscalls.s,v $
 *Revision 1.5  1994/11/18  20:32:05  mtm
 *Copyright additions/changes
 *
 *Revision 1.4  1993/07/14  17:59:53  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 *Revision 1.1.1.3  1993/07/01  19:18:43  cfj
 *Adding new code from vendor
 *
 *Revision 1.3  1993/05/06  19:10:25  cfj
 *ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:06  cfj
 * Initial 1.0.3 code drop
 * Revision 1.2  1992/11/30  22:22:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:19  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1992/01/09  16:58:21  roy
 * 	91/12/20  12:13:51  stans
 * 	added syscalls getpid() & sigreturn()
 *
 * Revision 2.2  91/11/17  14:51:36  rabii
 * 	Initial check-in
 * 
 */

/*
 * System calls needed by second server to access the primary server 
 * via unix system calls.  We just rename them unix_foo instead of foo 
 * not to mix up with this server entries.
 */

#if defined(SYSLIBC_SCCS) && !defined(lint)
	.asciz "@(#)write.s	5.5 (Berkeley) 6/27/88"
#endif /* SYSLIBC_SCCS and not lint */

#include <i860/second_SYS.h>

SYSCALL(open)
	RET

SYSCALL(fstat)
	RET

SYSCALL(access)
	RET

SYSCALL(lseek)
	RET

SYSCALL(write)
	RET

SYSCALL(read)
	RET

SYSCALL(close)
	RET

SYSCALL(ioctl)
	RET

SYSCALL(sigvec)
	RET

SYSCALL(sigreturn)
        RET

SYSCALL(getpid)
        RET

PSEUDO(_exit,exit)

ASSIGN(second_errno)
	.long	0

	.text; .align ALIGN
	.globl  LCL(second_cerror)
LCL(second_cerror):
	mov	EXT(second_errno),r30
	st.l	r16,0(r30)
	mov	-1,r16
	RET

