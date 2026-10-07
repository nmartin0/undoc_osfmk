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
 * HISTORY
 * $Log: slock.s,v $
 *Revision 1.5  1994/11/18  20:31:14  mtm
 *Copyright additions/changes
 *
 *Revision 1.4  1994/10/25  22:23:30  yazz
 * Reviewer: Nandini Ajmani
 * Risk: Lo
 * Benefit or PTS #: 11128
 * Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 * Module(s):
 *	server/i386/bsd_machdep.c
 *	server/i386/slock.s
 *	server/i860/bsd_machdep.c
 *	server/i860/slock.s
 *	server/kern/parallel.h
 *	server/kern/sched_prim.c
 *	server/sys/unix_defs.h
 *	server/tnc/rvp_subr.c
 *	server/uxkern/cred_servers.c
 *	server/uxkern/emul_user.c
 *	server/uxkern/fsvr_subr.c
 *	server/uxkern/ux_server_loop.c
 *
 *Added tiny new asm routine to return the current value of the frame pointer.
 *
 *Revision 1.3  1993/07/14  17:57:01  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:16:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:21:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:11  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:45  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1992/04/05  16:53:19  pjg
 * 	Initial revision
 *
 */

#include <i386/asm.h>


/ current_pc()

ENTRY(current_pc)
	movl	(%esp),%eax
	ret
/ current_fp()

ENTRY(current_fp)
	movl	%ebp,%eax
	ret
