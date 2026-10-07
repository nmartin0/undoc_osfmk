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
 * Simple lock primitives
 *
 * HISTORY
 * $Log: slock.s,v $
 *Revision 1.7  1994/11/18  20:32:08  mtm
 *Copyright additions/changes
 *
 *Revision 1.6  1994/10/25  22:24:04  yazz
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
 *Added tiny asm routine to return the current value of the frame pointer.
 *This is used to trace back call chains.
 *
 *Revision 1.5  1993/07/14  17:59:57  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 *Revision 1.1.1.3  1993/07/01  19:18:49  cfj
 *Adding new code from vendor
 *
 *Revision 1.4  1993/05/12  19:02:42  cfj
 *Switch around the 2 instructions in current_pc() so that there is not
 *an empty delay slot.
 *
 *Revision 1.3  1993/05/06  19:10:30  cfj
 *ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:07  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:15  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:22  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:14:57  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/06/05  13:58:12  klh
 * 	Revision 2.4  92/05/12  16:16:13  loverso
 * 		Just keep current_pc(), and fix assembler syntax
 *
 * Revision 2.4  92/04/06  19:08:02  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.3  92/04/05  16:53:47  pjg
 * 	Add current_pc() (sjs/pjg)
 * 
 * 	Get rid of $Header to allow o2install to work correctly. (roman)
 * 
 * 
 * Revision 2.2  91/11/17  14:50:32  rabii
 * 	Initial check-in
 */
	.text

#include <i860/asm.h>

/*
 * current_pc()
 *	returns the program counter of the caller
 */
ENTRY(current_pc)
	bri	r1
	mov	r1,r16

/*
 * current_fp()
 *	returns the frame pointer of the caller
 */
ENTRY(current_fp)
	bri	r1
	mov	fp,r16
