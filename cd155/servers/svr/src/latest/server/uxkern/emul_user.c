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
 * HISTORY
 * $Log: emul_user.c,v $
 * Revision 1.5  1994/11/18  20:47:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/25  22:34:57  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: Lo
 *  Benefit or PTS #: 11128
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/i386/bsd_machdep.c
 * 	server/i386/slock.s
 * 	server/i860/bsd_machdep.c
 * 	server/i860/slock.s
 * 	server/kern/parallel.h
 * 	server/kern/sched_prim.c
 * 	server/sys/unix_defs.h
 * 	server/tnc/rvp_subr.c
 * 	server/uxkern/cred_servers.c
 * 	server/uxkern/emul_user.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/ux_server_loop.c
 * Include <sys/unix.defs> so that MACH_ASSERT and MACH_LDEBUG are properly
 * respected in master locking-related includes.
 *
 * Revision 1.3  1993/07/14  18:41:10  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:00:50  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:54:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:50  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:51:50  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1992/01/17  17:26:43  roy
 * 	Inital checkin (loverso).
 *
 *
 */
#include <sys/unix_defs.h>
#undef KERNEL
#include <uxkern/emul_call_user.c>
