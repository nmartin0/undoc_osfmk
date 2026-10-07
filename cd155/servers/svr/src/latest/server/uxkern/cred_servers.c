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
 * $Log: cred_servers.c,v $
 * Revision 1.5  1994/11/18  20:47:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/25  22:35:24  yazz
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
 * Revision 1.3  1993/07/14  18:40:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:59:24  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:53:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:17  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:50:44  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1992/04/05  17:07:42  pjg
 * 	New file containing only the credentials stubs (rabii)
 *
 */

#include <sys/unix_defs.h>
#undef	KERNEL
#include <uxkern/cred_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/cred_user.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/credcache_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/credcache_user.c>
