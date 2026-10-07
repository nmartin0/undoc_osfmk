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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: uipc_socket.c,v $
 * Revision 1.10  1995/02/01  21:30:22  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:28:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/08/31  22:46:37  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.7.2.1  1994/08/22  21:52:27  nina
 *  Reviewer:yazz, hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7618, 10383. See also 7155 and 10114.
 *  Testing:NFS EATS. TCP EATS
 *  Module(s):
 * 	server/sys/socketvar.h
 * 	server/bsd/uipc_socket.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 *
 * 	cmds_libs/usr/src/sbin/portmap/Makefile
 * 	cmds_libs/usr/src/sbin/portmap/portmap.c
 *
 * 	cmds_libs/usr/src/sbin/mountd/Makefile
 * 	cmds_libs/usr/src/sbin/mountd/mountd.c
 *
 * 	cmds_libs/usr/src/sbin/nfsd/Makefile
 * 	cmds_libs/usr/src/sbin/nfsd/nfsd.c
 *
 * 	cmds_libs/usr/src/sbin/nfsiod/Makefile
 * 	cmds_libs/usr/src/sbin/nfsiod/nfsiod.c
 *
 * 	cmds_libs/usr/src/ccs/lib/libnx/Makefile
 * 	cmds_libs/usr/src/ccs/lib/libnx/tnc_subs.c (NEW)
 *
 * 	cmds_libs/sbin/init.d/nfs
 *
 *
 * Modifications have been made to the server, libnx, the NFS
 * daemons and nfs start/stop script so that each configured
 * network server node runs its own copy of the NFS daemons.
 *
 * Added the VS_BINDLOCAL option to sogetopt() and sosetopt().
 *
 * Revision 1.7  1994/05/12  17:57:53  nina
 *  Reviewer:mjl
 *  Risk:Low
 *  Benefit or PTS #:6660
 *  Testing:Test case provided with bug report
 *  Module(s):./server/bsd/uipc_socket.c
 *
 *  Add SO_ACCEPTCONN to the sogetopt() case statement.
 *
 * Revision 1.6  1994/03/14  02:00:55  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.5  1993/07/14  17:50:53  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  18:51:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:07:52  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1992/11/30  22:17:40  dleslie
 * Copy of NX branch back into main trunk
 * Revision 1.1.1.2  1993/05/03  17:25:55  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.2  1992/11/06  18:21:58  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/06  00:07:50  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.18  92/10/27  17:29:15  bhk
 * Made getsocketname and getpeername use the sogetaddr socket function
 * Virualized the sogetaddr function.
 * 
 * Revision 2.19  1993/04/24  18:48:01  klh
 * 	Revision 2.12  93/04/08  11:20:38  loverso
 * 		Fix possible uninitialized variable (Karl Kleinpaste @ CMU)
 *
 * Revision 2.18  92/10/27  17:29:15  bhk
 * Made getsocketname and getpeername use the sogetaddr socket function
 * Virualized the sogetaddr function.
 * 
 * Revision 2.17  92/08/17  12:50:05  mjl
 * Got rid of extraneous arg to VSOP_SLEEP().
 * 
 * Revision 2.15  92/07/07  15:12:59  roman
 * Minor format corrections to match OSF code exactly.
 * 
 * Revision 2.14  92/05/06  13:09:05  bhk
 * added VSOP_SLEEP vsocket operation
 * 
 * Revision 2.13  92/04/20  17:24:45  bhk
 * Deleted spurious definition of vsocket_ops
 * 
 * Revision 2.12  92/04/14  10:07:07  roman
 * Correct typo where VPOP_SIGPGRP() should be VPOP_SIGPROC().
 * 
 * Revision 2.11  92/03/28  10:49:04  chrisp
 * allow non-TNC build without Virtual Sockets
 * 
 * Revision 2.10  92/03/27  11:09:51  roman
 * Add support for vsockets for TNC.
 * 
 * Revision 2.9  92/03/24  10:19:46  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.6  92/03/09  14:34:09  durriya
 * 	Revision 3.2  91/12/18  17:15:49  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.5  91/11/22  14:55:00  rabii
 * 	VPOP_SIGPGRP now has additional parameter. (chrisp)
 * 
 * 	Allow LOCATE_VPROC_PID() to return a null pointer, which must be
 * 	checked for and recovered from. (roman)
 * 
 * 	Changes to arguments required by VPOP_SIG* operations. (chrisp)
 * 
 * Revision 2.4  91/10/04  14:54:21  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:51:04  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Change to use process group ids in place of pgrp structures.
 * 
 * Revision 2.2  91/08/31  13:24:33  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:31:57  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16.8.2  91/04/29  12:10:04  tmt
 * 	Unlock socket in sodisconnect(). Tune sosend() cluster allocations.
 * 	[91/04/10  09:49:10  tmt]
 * 
 * Revision 1.16.6.2  91/03/15  17:46:09  tmt
 * 	Rearrange sodisconnect() to provide exportable interface.
 * 	[91/03/13  19:00:02  tmt]
 * 
 * Revision 1.16  90/10/31  13:50:14  devrcs
 * 	Check so_dqlen before dequeueing from so_dq in sofree.
 * 	[90/10/13  09:48:00  tmt]
 * 
 * 	Arbitrate races in soclose, sofree (from isr soabort) and sodequeue
 * 	by using new so_dq, so_dqlen and SP_CLOSING. Modify code in sodequeue,
 * 	soaccept, soclose, sofree to conform. Move socketvar.h comment here.
 * 	[90/10/12  17:41:08  tmt]
 * 
 * Revision 1.15  90/10/07  13:20:45  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:03:35  gm]
 * 
 * 	Look at flags for MSG_NONBLOCK in sosend/soreceive.
 * 	Remove include of mach_ldebug.h, done in net_globals.h.
 * 	[90/09/29  18:05:57  tmt]
 * 
 * 	For tracking lock counts, solockpair should unlock the
 * 	socklocks before freeing.
 * 	[90/09/28  12:36:58  nags]
 * 
 * 	Fix sodequeue from returning unitialized value on success.
 * 	[90/09/22  19:36:11  tmt]
 * 
 * 	Move SS_LOCKABLE and SS_FREEABLE to SP* in so_special.
 * 	[90/09/20  18:43:49  tmt]
 * 
 * Revision 1.14  90/09/13  11:42:41  devrcs
 * 	Add PCATCH to sosleep priority. Use sockaddr_old converter.
 * 	[90/08/28  11:50:59  tmt]
 * 
 * 	Allow types of header and data in soreceive processing.
 * 	Remove sockbuf upcall functions on NOFDREF detach.
 * 	[90/08/24  12:39:10  tmt]
 * 
 * Revision 1.13  90/08/09  13:15:49  devrcs
 * 	Fix pipe atomicity from returning EMSGSIZ when full.
 * 	[90/07/26  10:36:55  tmt]
 * 
 * Revision 1.12  90/07/27  08:45:07  devrcs
 * 	Update to BSD Reno release.
 * 	Modify sosend/soreceive calling, add timeout to sosleep.
 * 	Delete soXXsend/soXXreceive.
 * 	Use "sowatomic" macro for pipe atomicity in sosend.
 * 	[90/07/20  12:44:45  tmt]
 * 
 * Revision 1.11  90/07/17  11:20:12  devrcs
 * 	Use symbolic in SP_* calls
 * 	[90/07/10  21:53:11  seiden]
 * 
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	Fix bugs in sogetaddr(). Change soshutdown to match syscall args.
 * 	[90/07/03  18:42:40  tmt]
 * 
 * Revision 1.9  90/06/22  20:07:48  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:54:33  seiden]
 * 
 * 	Rearrange code and #ifdefs slightly. Don't do security if
 * 	socket has no associated u-area (e.g. XTI), invalid!
 * 	[90/06/11  16:34:29  tmt]
 * 
 * 	nags merge
 * 	[90/06/12  21:18:00  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:41:29  seiden]
 * 
 * 	Zero sockbuf lock pointers before lock_done. Change an #ifdef.
 * 	Remove MMAX_MP tokens and replace with PARALLEL_SELECT.
 * 	Fix locking problem in soabort. Make pipe writes atomic.
 * 	[90/06/06  14:10:00  tmt]
 * 
 * Revision 1.8  90/05/13  18:34:41  devrcs
 * 	Add and use SS_NOUAREA bit. Fix locks after failed sosblock and
 * 	sosbunlock. Add temporary send/receive interfaces for XTI.
 * 	[90/05/04  14:41:05  tmt]
 * 
 * Revision 1.7  90/04/27  18:53:38  devrcs
 * 	Modify callers of sosleep, sosbwait, sosblock because these routines
 * 	no longer longjmp on interrupted sleeps. Add two routines for XTI.
 * 	[90/04/20  12:16:43  tmt]
 * 
 * Revision 1.6  90/04/14  00:30:29  devrcs
 * 	Do dynamic domain referencing. Strengthen declarations (void).
 * 	[90/04/10  14:53:45  tmt]
 * 
 * Revision 1.5  90/02/05  15:47:53  robert
 * 	Use more macros for locks. Move lock pair code here.
 * 	[90/01/19  14:10:46  tmt]
 * 
 * Revision 1.4  90/01/18  08:42:32  gm
 * 	Do sopriv() to refresh SS_PRIV bit at appropriate points.
 * 	Change #if !MMAX_MP to #if MACH && NETNCPUS > 1 for
 * 	signals, etc to more closely agree with need.
 * 	Do SPL and take socket locks in {set,get}sockopt.
 * 	Expand m_copy macro to direct m_copym.
 * 	[90/01/08  15:39:19  tmt]
 * 
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/03  11:52:20  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:28:10  gm]
 * 
 * Revision 1.2  89/12/26  09:24:03  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)uipc_socket.c	2.1 16:10:40 4/20/90 SecureWare, Inc. */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986, 1988, 1990 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	%W% (Berkeley) %G%
 *	Merged: uipc_socket.c	7.23 (Berkeley) 6/29/90
 */

#include "net/net_globals.h"
#if	MACH
#include <sys/secdefines.h>
#endif

#include "sys/param.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/vproc.h"
#include "sys/file.h"
#include "sys/kernel.h"
#include "sys/time.h"

#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#include "sys/mbuf.h"
#include "sys/socket.h"
#ifdef TNC
#include "vsocket/vsocket.h"
#else
#include "sys/so_defs.h"
#endif
#include "sys/socketvar.h"
#include "sys/domain.h"
#include "sys/protosw.h"

#include "net/net_malloc.h"

#if	MACH
#include "kern/parallel.h"

#if	SEC_BASE
#include <sys/security.h>
#endif
#if	SEC_ILB
#include <sys/secpolicy.h>
#endif
#endif

LOCK_ASSERTL_DECL

/*
 * Socket operation routines.
 * These routines are called by the routines in
 * sys_socket.c or from a system process, and
 * implement the semantics of socket operations by
 * switching out to the protocol specific routines.
 *
 * TODO:
 *	test socketpair
 *	clean up async
 *	out-of-band is a kludge
 */
/*ARGSUSED*/
socreate(dom, aso, type, proto)
	struct socket **aso;
	register int type;
	int proto;
{
	register struct protosw *prp;
	register struct socket *so;
	register int error;
	DOMAIN_FUNNEL_DECL(f)

	if (proto)
		prp = pffindproto(dom, proto, type);
	else
		prp = pffindtype(dom, type);
	if (prp == 0)
		return (EPROTONOSUPPORT);
	if (prp->pr_type != type) {
		DOMAINRC_UNREF(prp->pr_domain);
		return (EPROTOTYPE);
	}
	NET_MALLOC(so, struct socket *, sizeof(*so), M_SOCKET, M_WAITOK);
	bzero((caddr_t)so, sizeof(*so));
	so->so_type = type;
	so->so_proto = prp;
	if (prp->pr_domain->dom_funnel == 0)
		so->so_special |= SP_LOCKABLE;
#if	SEC_ARCH
	SP_OBJECT_CREATE(SIP->si_tag, so->so_tag, (tag_t *) 0, SEC_OBJECT);
	audstub_levels(so->so_tag);
#if	SEC_ILB
	if (sp_alloc_object(so->so_tag)) {	/* XXX should wait! */
		NET_FREE(so, M_SOCKET);
		return ENOMEM;
	}
	SP_EMPTY_OBJECT(so->so_tag);
#endif	/* SEC_ILB */
#endif	/* SEC_ARCH */
#if	NETSYNC_LOCK
	{
	struct socklocks *lp;
	NET_MALLOC(lp, struct socklocks *, sizeof(*lp), M_SOCKET, M_WAITOK);
	SOCKET_LOCKINIT(so, lp);
	++lp->refcnt;
	}
#endif
#if	PARALLEL_SELECT
	queue_init(&so->so_snd.sb_selq);
	queue_init(&so->so_rcv.sb_selq);
#endif
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	sopriv(so);	/* Set the SS_PRIV bit if create user is privileged. */
	error =
	    (*prp->pr_usrreq)(so, PRU_ATTACH,
		(struct mbuf *)0, (struct mbuf *)proto, (struct mbuf *)0);
	if (error) {
		so->so_state |= SS_NOFDREF;
		sofree(so);
	} else
		*aso = so;
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

sobind(so, nam)
	struct socket *so;
	struct mbuf *nam;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	sopriv(so);
	error =
	    (*so->so_proto->pr_usrreq)(so, PRU_BIND,
		(struct mbuf *)0, nam, (struct mbuf *)0);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

solisten(so, backlog)
	register struct socket *so;
	int backlog;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	sopriv(so);
	error =
	    (*so->so_proto->pr_usrreq)(so, PRU_LISTEN,
		(struct mbuf *)0, (struct mbuf *)0, (struct mbuf *)0);
	if (error == 0) {
		if (so->so_q == 0)
			so->so_options |= SO_ACCEPTCONN;
		if (backlog < 0)
			backlog = 0;
		so->so_qlimit = min(backlog, SOMAXCONN);
	}
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

static void
sodqfree(so)
	register struct socket *so;
{
	register struct socket *soq;

	LOCK_ASSERT("sodqfree", SOCKET_ISLOCKED(so));
	if (so->so_dqlen <= 0) {
		while (soq = so->so_dq) {
			SOCKET_LOCK(soq);
			sofree(soq);
			SOCKET_UNLOCK(soq);
		}
	}
}

void
sofree(so)
	register struct socket *so;
{
	struct socket *head = so->so_head;

	LOCK_ASSERT("sofree", SOCKET_ISLOCKED(so));
	if (so->so_pcb || (so->so_state & SS_NOFDREF) == 0)
		return;
	if (head) {
		/*
		 * See associated code in soclose and sodequeue.
		 */
		if (!(so->so_special & SP_CLOSING)) {
			SOCKET_LOCK(head);
			if (soqremque(so, 0) || soqremque(so, 1)) {
				if (head->so_dqlen) {
					so->so_special |= SP_CLOSING;
					soqinsque(head, so, -1);
				} else
					so->so_head = 0;
			} else
				panic("sofree dq");
			SOCKET_UNLOCK(head);
		} else {
			LOCK_ASSERT("sofree head", SOCKET_ISLOCKED(head));
			if (head->so_dqlen > 0)
				return;
			else if (soqremque(so, -1))
				so->so_head = 0;
			else
				panic("sofree dq2");
		}
	}
	if (so->so_head)	/* race in progress - loser frees */
		return;
	sbrelease(&so->so_snd);
	sorflush(so);
	DOMAINRC_UNREF(so->so_proto->pr_domain);
#if	!NETSYNC_LOCK
#if	SEC_ILB
	sp_free_object(so->so_tag);
#endif
	NET_FREE(so, M_SOCKET);
#else	/* NETSYNC_LOCK */
	so->so_special |= SP_FREEABLE;
#endif
}

#if	NETSYNC_LOCK
/*
 * The socklocks structure contains the locks and reference count
 *  for each socket.  When a socket is paired (ala unp_connect2),
 *  only one such structure is used for multiple sockets.
 * A note on reference counts. The socket is not explicitly reference
 *  counted due to higher level mechanisms such as the file descriptor
 *  (embodied in SS_NOFDREF). We use this to advantage since it saves a
 *  lot of bookkeeping. The socklocks structure for network-connected
 *  sockets always has a refcnt of 1, however, when two unix domain
 *  sockets are connected, they reference the same socklocks struct,
 *  and in this case refcnt grows to >= 2.
 * The sockbuf locks are currently redundant and are no-oped in a
 *  non-debug kernel (they are never taken without the socket lock).
 *  A higher degree of parallelism may be obtained with them in future.
 */

void
solockpair(so, so2)
	register struct socket *so, *so2;
{
	/*
	 * The lock structure for paired sockets must be the same lock
	 * in order to prevent race conditions.
	 */
	LOCK_ASSERT("solockpair so1", SOCKET_ISLOCKED(so));
	LOCK_ASSERT("solockpair so2", SOCKET_ISLOCKED(so2));

	if (so->so_lock != so2->so_lock) {
		struct socklocks *lp;
		int r1 = so->so_lock->refcnt;
		int r2 = so2->so_lock->refcnt;
		LOCK_ASSERT("solockpair refcnt", (r1 == 1 || r2 == 1));
		if (r1 > r2) {
			lp = so2->so_lock;
			so2->so_lock = so->so_lock;
			so2->so_rcv.sb_lock = so->so_rcv.sb_lock;
			so2->so_snd.sb_lock = so->so_snd.sb_lock;
			so->so_lock->refcnt++;
		} else {
			lp = so->so_lock;
			so->so_lock = so2->so_lock;
			so->so_rcv.sb_lock = so2->so_rcv.sb_lock;
			so->so_snd.sb_lock = so2->so_snd.sb_lock;
			so2->so_lock->refcnt++;
		}
#if     MACH_LDEBUG
		lock_done(lp);
#endif
		NET_FREE(lp, M_SOCKET);
	}
}

sounlock(so)
	struct socket *so;
{
	if (so->so_special & SP_FREEABLE) {
		/*
		 * The socket lock implicitly protects the socklocks refcnt.
		 * Make sure we don't drop the count without the lock!
		 */
		struct socklocks *lp = so->so_lock;

		LOCK_ASSERT("sounlock lp", (lp != 0));
		if (lp) {
			int cnt = --lp->refcnt;
			so->so_lock = 0;
			so->so_rcv.sb_lock = so->so_snd.sb_lock = 0;
			lock_done(&lp->sock_lock);
			if (cnt <= 0)
				NET_FREE(lp, M_SOCKET);
		}
#if	SEC_ILB
		sp_free_object(so->so_tag);
#endif
		NET_FREE(so, M_SOCKET);
		return 1;
	}
	lock_done(&so->so_lock->sock_lock);
	return 0;
}
#endif	/* NETSYNC_LOCK */

static int
sodisconn(so)
	struct socket *so;
{
	int error;

	LOCK_ASSERT("sodisconn", SOCKET_ISLOCKED(so));
	if ((so->so_state & SS_ISCONNECTED) == 0) {
		error = ENOTCONN;
		goto bad;
	}
	if (so->so_state & SS_ISDISCONNECTING) {
		error = EALREADY;
		goto bad;
	}
	error = (*so->so_proto->pr_usrreq)(so, PRU_DISCONNECT,
	    (struct mbuf *)0, (struct mbuf *)0, (struct mbuf *)0);
bad:
	return (error);
}

/*
 * Close a socket on last file table reference removal.
 * Initiate disconnect if connected.
 * Free socket when disconnect complete.
 */
soclose(so)
	register struct socket *so;
{
	int error = 0;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	if (so->so_options & SO_ACCEPTCONN) {
		register struct socket *soq;
		/*
		 * A lock hierarchy problem appears here when racing
		 * soabort by a netisr thread: so_dqlen arbitrates.
		 * Setting SP_CLOSING prevents any new accepts, and
		 * so_dqlen pushes all sofree's to so_dq, where we
		 * safely clean them up.
		 */
		so->so_special |= SP_CLOSING;
		++so->so_dqlen;
		while ((soq = so->so_q0) || (soq = so->so_q)) {
			SOCKET_UNLOCK(so);
			(void) soabort(soq);
			SOCKET_LOCK(so);
		}
		if (--so->so_dqlen <= 0 && so->so_dq)
			sodqfree(so);
	}
	if (so->so_pcb == 0)
		goto discard;
	if (so->so_state & SS_ISCONNECTED) {
		if ((so->so_state & SS_ISDISCONNECTING) == 0) {
			error = sodisconn(so);
			if (error)
				goto drop;
		}
		if (so->so_options & SO_LINGER) {
			if ((so->so_state & SS_ISDISCONNECTING) &&
			    (so->so_state & SS_NBIO))
				goto drop;
			while (so->so_state & SS_ISCONNECTED)
				if (error = VSOP_SLEEP(so,
						(caddr_t)&so->so_timeo,
						(PZERO+1) | PCATCH,
						so->so_linger))
					break;
		}
	}
drop:
	if (so->so_pcb) {
		int error2 =
		    (*so->so_proto->pr_usrreq)(so, PRU_DETACH,
			(struct mbuf *)0, (struct mbuf *)0, (struct mbuf *)0);
		if (error == 0)
			error = error2;
	}
discard:
	if (so->so_state & SS_NOFDREF)
		panic("soclose: NOFDREF");
	so->so_snd.sb_wakeup = so->so_rcv.sb_wakeup = 0;
	so->so_snd.sb_wakearg = so->so_rcv.sb_wakearg = 0;
	so->so_state |= SS_NOFDREF;
	sofree(so);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

soabort(so)
	struct socket *so;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	error =
	    (*so->so_proto->pr_usrreq)(so, PRU_ABORT,
		(struct mbuf *)0, (struct mbuf *)0, (struct mbuf *)0);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

soaccept(so, nam)
	register struct socket *so;
	struct mbuf *nam;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	/* Test used to be reversed. Now the responsibility of caller. */
	if (so->so_state & SS_NOFDREF)
		panic("soaccept: NOFDREF");
	sopriv(so);
	error = (*so->so_proto->pr_usrreq)(so, PRU_ACCEPT,
	    (struct mbuf *)0, nam, (struct mbuf *)0);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

soconnect(so, nam)
	register struct socket *so;
	struct mbuf *nam;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	if (so->so_options & SO_ACCEPTCONN)
		error = EOPNOTSUPP;
	else {
		sopriv(so);
		/*
		 * If protocol is connection-based, can only connect once.
		 * Otherwise, if connected, try to disconnect first.
		 * This allows user to disconnect by connecting to, e.g.,
		 * a null address.
		 */
		if (so->so_state & (SS_ISCONNECTED|SS_ISCONNECTING) &&
		    ((so->so_proto->pr_flags & PR_CONNREQUIRED) ||
		    (error = sodisconn(so))))
			error = EISCONN;
		else
			error = (*so->so_proto->pr_usrreq)(so, PRU_CONNECT,
			    (struct mbuf *)0, nam, (struct mbuf *)0);
	}
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

soconnect2(so1, so2)
	register struct socket *so1;
	struct socket *so2;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so1), f);
	SOCKET_LOCK2(so1, so2);
	sopriv(so1); sopriv(so2);
	error = (*so1->so_proto->pr_usrreq)(so1, PRU_CONNECT2,
	    (struct mbuf *)0, (struct mbuf *)so2, (struct mbuf *)0);
	SOCKET_UNLOCK2(so1, so2);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

sodisconnect(so)
	register struct socket *so;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	error = sodisconn(so);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

/*
 * Send on a socket.
 * If send must go all at once and message is larger than
 * send buffering, then hard error.
 * Lock against other senders.
 * If must go all at once and not enough room now, then
 * inform user that this would block and do nothing.
 * Otherwise, if nonblocking, send as much as possible.
 * The data to be sent is described by "uio" if nonzero,
 * otherwise by the mbuf chain "top" (which must be null
 * if uio is not).  Data provided in mbuf chain must be small
 * enough to send all at once.
 *
 * Returns nonzero on error, timeout or signal; callers
 * must check for short counts if EINTR/ERESTART are returned.
 * Data and control buffers are freed on return.
 */
sosend(so, addr, uio, top, control, flags)
	register struct socket *so;
	struct mbuf *addr;
	struct uio *uio;
	struct mbuf *top;
	struct mbuf *control;
	int flags;
{
	struct mbuf **mp;
	register struct mbuf *m;
	register long space, len, resid;
	int clen = 0, error, dontroute, mlen, atomic;
	DOMAIN_FUNNEL_DECL(f)

if (top && (uio || !(top->m_flags & M_PKTHDR)))
panic("sosend 1");
	if (uio)
		resid = uio->uio_resid;
	else
		resid = top->m_pkthdr.len;

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	atomic = sosendallatonce(so) || top;
	if (!atomic && sowatomic(so, resid)) atomic = -1;
	dontroute =
	    (flags & MSG_DONTROUTE) && (so->so_options & SO_DONTROUTE) == 0 &&
	    (so->so_proto->pr_flags & PR_ATOMIC);
	if (SOHASUAREA(so))
		u.u_ru.ru_msgsnd++;
	if (control)
		clen = control->m_len;
#define	snderr(errno)	{ error = errno; goto release; }

restart:
	if (error = sosblock(&so->so_snd, so))
		goto out;
	do {
		if (so->so_state & SS_CANTSENDMORE)
			snderr(EPIPE);
		if (so->so_error)
			snderr(so->so_error);
		if ((so->so_state & SS_ISCONNECTED) == 0) {
			if (so->so_proto->pr_flags & PR_CONNREQUIRED) {
				if ((so->so_state & SS_ISCONFIRMING) == 0)
					snderr(ENOTCONN);
			} else if (addr == 0)
				snderr(EDESTADDRREQ);
		}
		space = sbspace(&so->so_snd);
		if (flags & MSG_OOB)
			space += 1024;
		if (space < resid + clen &&
		    (atomic || space < so->so_snd.sb_lowat || space < clen)) {
			if ((atomic > 0 && resid > so->so_snd.sb_hiwat) ||
			    clen > so->so_snd.sb_hiwat)
				snderr(EMSGSIZE);
			if ((so->so_state & SS_NBIO) || (flags & MSG_NONBLOCK))
				snderr(EWOULDBLOCK);
			if (error = sosbwait(&so->so_snd, so))
				goto out;
			goto restart;
		}
		SOCKBUF_UNLOCK(&so->so_snd);
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		mp = &top;
		space -= clen;
		do {
		    if (uio == NULL) {
			/*
			 * Data is prepackaged in "top".
			 */
			resid = 0;
			if (flags & MSG_EOR)
				top->m_flags |= M_EOR;
		    } else do {
			if (top == 0) {
				MGETHDR(m, M_WAIT, MT_DATA);
				mlen = MHLEN;
				m->m_pkthdr.len = 0;
				m->m_pkthdr.rcvif = (struct ifnet *)0;
			} else {
				MGET(m, M_WAIT, MT_DATA);
				mlen = MLEN;
			}
			if (resid >= MINCLSIZE) {
				MCLGET(m, M_WAIT);
				if ((m->m_flags & M_EXT) == 0)
					goto nopages;
				mlen = MCLBYTES;
#ifdef	MAPPED_MBUFS
				len = min(MCLBYTES, resid);
#else
				if (atomic > 0 && top == 0) {
					len = min(MCLBYTES - max_hdr, resid);
					m->m_data += max_hdr;
				} else
					len = min(MCLBYTES, resid);
#endif
			} else {
nopages:
				len = min(min(mlen, resid), space);
				/*
				 * For datagram protocols, leave room
				 * for protocol headers in first mbuf.
				 */
				if (atomic > 0 && top == 0 && len < mlen)
					MH_ALIGN(m, len);
			}
			space -= len;
			error = uiomove(mtod(m, caddr_t), len, uio);
			resid = uio->uio_resid;
			m->m_len = len;
			*mp = m;
			top->m_pkthdr.len += len;
			if (error) {
				DOMAIN_FUNNEL(sodomain(so), f);
				SOCKET_LOCK(so);
				SOCKBUF_LOCK(&so->so_snd);
				goto release;
			}
			mp = &m->m_next;
			if (resid <= 0) {
				if (flags & MSG_EOR)
					top->m_flags |= M_EOR;
				break;
			}
		    } while (space > 0 && atomic);
		    DOMAIN_FUNNEL(sodomain(so), f);
		    SOCKET_LOCK(so);
		    if (dontroute)
			    so->so_options |= SO_DONTROUTE;
		    error = (*so->so_proto->pr_usrreq)(so,
			(flags & MSG_OOB) ? PRU_SENDOOB : PRU_SEND,
			top, addr, control);
		    if (dontroute)
			    so->so_options &= ~SO_DONTROUTE;
		    SOCKET_UNLOCK(so);
		    DOMAIN_UNFUNNEL(f);
		    clen = 0;
		    control = 0;
		    top = 0;
		    mp = &top;
		    if (error) {
			DOMAIN_FUNNEL(sodomain(so), f);
			SOCKET_LOCK(so);
			SOCKBUF_LOCK(&so->so_snd);
			goto release;
		    }
		} while (resid && space > 0);
		DOMAIN_FUNNEL(sodomain(so), f);
		SOCKET_LOCK(so);
		SOCKBUF_LOCK(&so->so_snd);
	} while (resid);

release:
	sbunlock(&so->so_snd);
out:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	if (top)
		m_freem(top);
	if (control)
		m_freem(control);
	return (error);
}

/*
 * Implement receive operations on a socket.
 * We depend on the way that records are added to the sockbuf
 * by sbappend*.  In particular, each record (mbufs linked through m_next)
 * must begin with an address if the protocol so specifies,
 * followed by an optional mbuf or mbufs containing ancillary data,
 * and then zero or more mbufs of data.
 * In order to avoid blocking network interrupts for the entire time here,
 * we splx() while doing the actual copy to user space.
 * Although the sockbuf is locked, new data may still be appended,
 * and thus we must maintain consistency of the sockbuf during that time.
#if	NETSYNC_LOCK
 * Note "sockbuf locked" means only SB_LOCK set to synchronize with other
 * processes. The actual SOCKBUF_LOCK is released.
#endif
 * 
 * The caller may receive the data as a single mbuf chain by supplying
 * an mbuf **mp0 for use in returning the chain.  The uio is then used
 * only for the count in uio_resid.
 */
soreceive(so, paddr, uio, mp0, controlp, flagsp)
	register struct socket *so;
	struct mbuf **paddr;
	struct uio *uio;
	struct mbuf **mp0;
	struct mbuf **controlp;
	int *flagsp;
{
	register struct mbuf *m, **mp;
	register int flags, len, error, offset;
	struct protosw *pr = so->so_proto;
	struct mbuf *nextrecord;
	int moff, type;
	DOMAIN_FUNNEL_DECL(f)
#if	SEC_ILB
	tag_t ntag;
	int some_moved = 0;
#endif

	mp = mp0;
	if (paddr)
		*paddr = 0;
	if (controlp)
		*controlp = 0;
	if (flagsp)
		flags = *flagsp &~ MSG_EOR;
	else 
		flags = 0;
	if (flags & MSG_OOB) {
		m = m_get(M_WAIT, MT_DATA);
		DOMAIN_FUNNEL(sodomain(so), f);
		SOCKET_LOCK(so);
		error = (*pr->pr_usrreq)(so, PRU_RCVOOB,
		    m, (struct mbuf *)(flags & MSG_PEEK), (struct mbuf *)0);
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		if (error)
			goto bad;
#if	SEC_ILB
		if (SOHASUAREA(so) &&
		    SP_CHECK_FLOAT(UIO_READ, so->so_tag, &ntag)) {
			error = u.u_error;		/* XXX */
			goto bad;
		}
#endif
		do {
			error = uiomove(mtod(m, caddr_t),
			    (int) min(uio->uio_resid, m->m_len), uio);
			m = m_free(m);
		} while (uio->uio_resid && error == 0 && m);
#if	SEC_ILB
		if (SOHASUAREA(so))
			SP_DO_FLOAT(UIO_READ, so->so_tag, &ntag);
#endif
bad:
		if (m)
			m_freem(m);
		return (error);
	}
	if (mp)
		*mp = (struct mbuf *)0;
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	if (so->so_state & SS_ISCONFIRMING && uio->uio_resid)
		(*pr->pr_usrreq)(so, PRU_RCVD, (struct mbuf *)0,
		    (struct mbuf *)0, (struct mbuf *)0);

restart:
	if (error = sosblock(&so->so_rcv, so))
		goto out;
	m = so->so_rcv.sb_mb;
	/*
	 * If we have less data than requested, block awaiting more
	 * (subject to any timeout) if:
	 *   1. the current count is less than the low water mark, or
	 *   2. MSG_WAITALL is set, and it is possible to do the entire
	 *	receive operation at once if we block (resid <= hiwat).
	 * If MSG_WAITALL is set but resid is larger than the receive buffer,
	 * we have to do the receive in sections, and thus risk returning
	 * a short count if a timeout or signal occurs after we start.
	 */
	if (m == 0 || so->so_rcv.sb_cc < uio->uio_resid && 
	    (so->so_rcv.sb_cc < so->so_rcv.sb_lowat ||
	    ((flags & MSG_WAITALL) && uio->uio_resid <= so->so_rcv.sb_hiwat))) {
		if (m == 0 && so->so_rcv.sb_cc)
			panic("receive 1");
		if (so->so_error) {
			error = so->so_error;
			so->so_error = 0;
			goto release;
		}
		if (so->so_state & SS_CANTRCVMORE)
			goto release;
		if ((so->so_state & (SS_ISCONNECTED|SS_ISCONNECTING)) == 0 &&
		    (so->so_proto->pr_flags & PR_CONNREQUIRED)) {
			error = ENOTCONN;
			goto release;
		}
		if (uio->uio_resid == 0)
			goto release;
		if ((so->so_state & SS_NBIO) || (flags & MSG_NONBLOCK)) {
			error = EWOULDBLOCK;
			goto release;
		}
		if (error = sosbwait(&so->so_rcv, so))
			goto out;
		goto restart;
	}
	if (SOHASUAREA(so))
		u.u_ru.ru_msgrcv++;
if (m->m_type == 0)
panic("receive 3a");
	nextrecord = m->m_nextpkt;
	if (pr->pr_flags & PR_ADDR) {
		if (m->m_type != MT_SONAME)
			panic("receive 1a");
		if (flags & MSG_PEEK) {
			if (paddr)
				*paddr = m_copym(m, 0, m->m_len, M_WAIT);
			m = m->m_next;
		} else {
			sbfree(&so->so_rcv, m);
			if (paddr) {
				*paddr = m;
				so->so_rcv.sb_mb = m->m_next;
				m->m_next = 0;
				m = so->so_rcv.sb_mb;
			} else {
				MFREE(m, so->so_rcv.sb_mb);
				m = so->so_rcv.sb_mb;
			}
		}
	}
	while (m && m->m_type == MT_CONTROL && error == 0) {
#if	SEC_ILB
		/*
		 * If the rights buffer associated with this record
		 * contains an information label tag, retrieve the tag
		 * and assign it to the socket.  The call to SP_DO_FLOAT
		 * takes care of adjusting the bitmaps so that the
		 * subsequent call to SP_CHECK_FLOAT will compute a new
		 * process IL tag.
		 */
		if (SOHASUAREA(so) && getILfromrights(m, ntag))
			SP_DO_FLOAT(UIO_WRITE, so->so_tag, &ntag);
#endif	/* SEC_ILB */
		if (flags & MSG_PEEK) {
			if (controlp)
				*controlp = m_copym(m, 0, m->m_len, M_WAIT);
			m = m->m_next;
		} else {
			sbfree(&so->so_rcv, m);
			if (controlp) {
				if (pr->pr_domain->dom_externalize &&
				    mtod(m, struct cmsghdr *)->cmsg_type ==
				    SCM_RIGHTS)
				   error = (*pr->pr_domain->dom_externalize)(m);
				*controlp = m;
				so->so_rcv.sb_mb = m->m_next;
				m->m_next = 0;
				m = so->so_rcv.sb_mb;
			} else {
				MFREE(m, so->so_rcv.sb_mb);
				m = so->so_rcv.sb_mb;
			}
		}
		if (controlp)
			controlp = &(*controlp)->m_next;
	}
	if (m) {
		if ((flags & MSG_PEEK) == 0)
			m->m_nextpkt = nextrecord;
		type = m->m_type;
	}
#if	SEC_ILB
	if (SOHASUAREA(so) && SP_CHECK_FLOAT(UIO_READ, so->so_tag, &ntag))
		error = u.u_error;			/* XXX */
#endif
	moff = 0;
	offset = 0;
	while (m && uio->uio_resid > 0 && error == 0) {
		if (m->m_type != type) {
			if (type != MT_DATA && type != MT_HEADER)
				break;
			if (m->m_type != MT_DATA && m->m_type != MT_HEADER)
				break;
			type = m->m_type;	/* MT_HEADER==MT_DATA */
		}
		if (m->m_type == MT_OOBDATA)
			flags |= MSG_OOB;
		else if (m->m_type != MT_DATA && m->m_type != MT_HEADER)
			panic("receive 3");
		type = m->m_type;
		so->so_state &= ~SS_RCVATMARK;
		len = uio->uio_resid;
		if (so->so_oobmark && len > so->so_oobmark - offset)
			len = so->so_oobmark - offset;
		if (len > m->m_len - moff)
			len = m->m_len - moff;
		/*
		 * If mp is set, just pass back the mbufs.
		 * Otherwise copy them out via the uio, then free.
		 * Sockbuf must be consistent here (points to current mbuf,
		 * it points to next record) when we drop priority;
		 * we must note any additions to the sockbuf when we
		 * block interrupts again.
		 */
#if	SEC_ILB
		some_moved = 1;
#endif
		if (mp == 0) {
			SOCKBUF_UNLOCK(&so->so_rcv);
			SOCKET_UNLOCK(so);
			DOMAIN_UNFUNNEL(f);
			error = uiomove(mtod(m, caddr_t) + moff, (int)len, uio);
			DOMAIN_FUNNEL(sodomain(so), f);
			SOCKET_LOCK(so);
			SOCKBUF_LOCK(&so->so_rcv);
		} else
			uio->uio_resid -= len;
		if (len == m->m_len - moff) {
			if (m->m_flags & M_EOR)
				flags |= MSG_EOR;
			if (flags & MSG_PEEK) {
				m = m->m_next;
				moff = 0;
			} else {
				nextrecord = m->m_nextpkt;
				sbfree(&so->so_rcv, m);
				if (mp) {
					*mp = m;
					mp = &m->m_next;
					so->so_rcv.sb_mb = m = m->m_next;
					*mp = (struct mbuf *)0;
				} else {
					MFREE(m, so->so_rcv.sb_mb);
					m = so->so_rcv.sb_mb;
				}
				if (m)
					m->m_nextpkt = nextrecord;
			}
		} else {
			if (flags & MSG_PEEK)
				moff += len;
			else {
				if (mp)
					*mp = m_copym(m, 0, len, M_WAIT);
				m->m_data += len;
				m->m_len -= len;
				so->so_rcv.sb_cc -= len;
			}
		}
		if (so->so_oobmark) {
			if ((flags & MSG_PEEK) == 0) {
				so->so_oobmark -= len;
				if (so->so_oobmark == 0) {
					so->so_state |= SS_RCVATMARK;
					break;
				}
			} else
				offset += len;
		}
		if (flags & MSG_EOR)
			break;
		/*
		 * If the MSG_WAITALL flag is set (for non-atomic socket),
		 * we must not quit until "uio->uio_resid == 0" or an error
		 * termination.  If a signal/timeout occurs, return
		 * with a short count but without error.
#if	FIXME
		 * Keep sockbuf locked against other readers.
#endif
		 */
		while (flags & MSG_WAITALL && m == 0 && uio->uio_resid > 0 &&
		    !sosendallatonce(so)) {
			if ((error = sosbwait(&so->so_rcv, so)) ||
			    (error = sosblock(&so->so_rcv, so))) {
				error = 0;
				goto out;
			}
			if (m = so->so_rcv.sb_mb)
				nextrecord = m->m_nextpkt;
			if (so->so_error || so->so_state & SS_CANTRCVMORE)
				break;
			continue;
		}
	}
	if ((flags & MSG_PEEK) == 0) {
		if (m == 0)
			so->so_rcv.sb_mb = nextrecord;
		else if (pr->pr_flags & PR_ATOMIC) {
			flags |= MSG_TRUNC;
			(void) sbdroprecord(&so->so_rcv);
		}
		if (pr->pr_flags & PR_WANTRCVD && so->so_pcb) {
			SOCKBUF_UNLOCK(&so->so_rcv);
			(*pr->pr_usrreq)(so, PRU_RCVD, (struct mbuf *)0,
			    (struct mbuf *)flags, (struct mbuf *)0,
			    (struct mbuf *)0);
			SOCKBUF_LOCK(&so->so_rcv);
		}
	}
	if (flagsp)
		*flagsp |= flags;
#if	SEC_ILB
	if (SOHASUAREA(so)) {
		if (some_moved)
			SP_DO_FLOAT(UIO_READ, so->so_tag, &ntag);
		if (so->so_rcv.sb_cc == 0)
			SP_EMPTY_OBJECT(so->so_tag);
	}
#endif	/* SEC_ILB */
release:
	sbunlock(&so->so_rcv);
out:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

soshutdown(so, how)
	register struct socket *so;
	register int how;
{
	int error = 0;
	DOMAIN_FUNNEL_DECL(f)

	switch (how++) {
	case 0: case 1: case 2:
		break;
	default:
		return (EINVAL);
	}
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	if (how & 1)
		sorflush(so);
	if (how & 2)
		error = (*so->so_proto->pr_usrreq)(so, PRU_SHUTDOWN,
		    (struct mbuf *)0, (struct mbuf *)0, (struct mbuf *)0);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

void
sorflush(so)
	register struct socket *so;
{
	register struct sockbuf *sb = &so->so_rcv;
	register struct protosw *pr = so->so_proto;
	struct sockbuf asb;
	DOMAIN_FUNNEL_DECL(f)

	LOCK_ASSERT("sorflush", SOCKET_ISLOCKED(so));

	DOMAIN_FUNNEL(sodomain(so), f);
	sb->sb_flags |= SB_NOINTR;
	(void) sosblock(sb, so);
	SOCKBUF_UNLOCK(sb);		/* want SB_LOCK but !locked */
	socantrcvmore(so);
	SOCKBUF_LOCK(sb);
	sbunlock(sb);
	asb = *sb;
#if	NETSYNC_LOCK || PARALLEL_SELECT
	/* We cannot just bzero the sockbuf, it would destroy our
	 * locks and/or select queue. So, we do it the silly way. */
	sb->sb_cc	= 0;
	sb->sb_hiwat	= 0;
	sb->sb_mbcnt	= 0;
	sb->sb_mbmax	= 0;
	sb->sb_lowat	= 0;
	sb->sb_mb	= 0;
	sb->sb_flags	= 0;
	sb->sb_timeo	= 0;
#else
	bzero((caddr_t)sb, sizeof (*sb));
#endif
	DOMAIN_UNFUNNEL(f);
	if (pr->pr_flags & PR_RIGHTS && pr->pr_domain->dom_dispose)
		(*pr->pr_domain->dom_dispose)(asb.sb_mb);
	sbrelease(&asb);
#if	SEC_ILB
	if (SOHASUAREA(so))
		SP_EMPTY_OBJECT(so->so_tag);
#endif
}

void
sopriv(so)
	struct socket *so;
{
	LOCK_ASSERT("sopriv", SOCKET_ISLOCKED(so));

	if (SOHASUAREA(so)) {
#if	SEC_BASE
		if (privileged(SEC_REMOTE, 0))
#else
		if (suser(u.u_cred, &u.u_acflag) == 0)
#endif
			so->so_state |= SS_PRIV;
		else
			so->so_state &= ~SS_PRIV;
	}
}

sosetopt(so, level, optname, m0)
	register struct socket *so;
	int level, optname;
	struct mbuf *m0;
{
	int error = 0;
	register struct mbuf *m = m0;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	sopriv(so);
	if (level != SOL_SOCKET) {
		if (so->so_proto && so->so_proto->pr_ctloutput) {
			error = ((*so->so_proto->pr_ctloutput)
				  (PRCO_SETOPT, so, level, optname, &m0));
			m = 0;
		} else
			error = ENOPROTOOPT;
	} else {
		switch (optname) {

		case SO_LINGER:
			if (m == NULL || m->m_len != sizeof (struct linger)) {
				error = EINVAL;
				goto bad;
			}
			so->so_linger = mtod(m, struct linger *)->l_linger;
			/* fall thru... */

		case SO_DEBUG:
		case SO_KEEPALIVE:
		case SO_DONTROUTE:
		case SO_USELOOPBACK:
		case SO_BROADCAST:
		case SO_REUSEADDR:
		case SO_OOBINLINE:
#if	SEC_ARCH
		case SO_EXPANDED_RIGHTS:
#endif
			if (m == NULL || m->m_len < sizeof (int)) {
				error = EINVAL;
				goto bad;
			}
			if (*mtod(m, int *))
				so->so_options |= optname;
			else
				so->so_options &= ~optname;
			break;

		case SO_SNDBUF:
		case SO_RCVBUF:
		case SO_SNDLOWAT:
		case SO_RCVLOWAT:
			if (m == NULL || m->m_len < sizeof (int)) {
				error = EINVAL;
				goto bad;
			}
			switch (optname) {

			case SO_SNDBUF:
				SOCKBUF_LOCK(&so->so_snd);
				if (sbreserve(&so->so_snd,
				    (u_long) *mtod(m, int *)) == 0)
					error = ENOBUFS;
				SOCKBUF_UNLOCK(&so->so_snd);
				if (error)
					goto bad;
				break;

			case SO_RCVBUF:
				SOCKBUF_LOCK(&so->so_rcv);
				if (sbreserve(&so->so_rcv,
				    (u_long) *mtod(m, int *)) == 0)
					error = ENOBUFS;
				SOCKBUF_UNLOCK(&so->so_rcv);
				if (error)
					goto bad;
				break;

			case SO_SNDLOWAT:
				so->so_snd.sb_lowat = *mtod(m, int *);
				break;
			case SO_RCVLOWAT:
				so->so_rcv.sb_lowat = *mtod(m, int *);
				break;
			}
			break;

		case SO_SNDTIMEO:
		case SO_RCVTIMEO:
		    {
			struct timeval *tv;
			short val;

			if (m == NULL || m->m_len < sizeof (*tv)) {
				error = EINVAL;
				goto bad;
			}
			tv = mtod(m, struct timeval *);
			if (tv->tv_sec > SHRT_MAX / hz - hz) {
				error = EDOM;
				goto bad;
			}
			val = tv->tv_sec * hz + tv->tv_usec / tick;

			switch (optname) {

			case SO_SNDTIMEO:
				so->so_snd.sb_timeo = val;
				break;
			case SO_RCVTIMEO:
				so->so_rcv.sb_timeo = val;
				break;
			}
			break;
		    }

#ifdef TNC
		case VS_BINDLOCAL:
			if (m == NULL || m->m_len < sizeof (int)) {
				error = EINVAL;
				goto bad;
			}
			if (*mtod(m, int *))
				so->vs_flags |= optname;
			else
				so->vs_flags &= ~optname;
			break;
#endif			
		default:
			error = ENOPROTOOPT;
			break;
		}
	}
bad:
	if (m)
		(void) m_free(m);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

sogetopt(so, level, optname, mp)
	register struct socket *so;
	int level, optname;
	struct mbuf **mp;
{
	register struct mbuf *m;
	int error = 0;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	sopriv(so);
	if (level != SOL_SOCKET) {
		if (so->so_proto && so->so_proto->pr_ctloutput)
			error = ((*so->so_proto->pr_ctloutput)
				  (PRCO_GETOPT, so, level, optname, mp));
		else
			error = ENOPROTOOPT;
	} else {
		m = m_get(M_WAIT, MT_SOOPTS);
		m->m_len = sizeof (int);

		switch (optname) {

		case SO_LINGER:
			m->m_len = sizeof (struct linger);
			mtod(m, struct linger *)->l_onoff =
				so->so_options & SO_LINGER;
			mtod(m, struct linger *)->l_linger = so->so_linger;
			break;

		case SO_USELOOPBACK:
		case SO_DONTROUTE:
		case SO_DEBUG:
		case SO_KEEPALIVE:
		case SO_REUSEADDR:
		case SO_BROADCAST:
		case SO_OOBINLINE:
		case SO_ACCEPTCONN:			
#if	SEC_ARCH
		case SO_EXPANDED_RIGHTS:
#endif
			*mtod(m, int *) = so->so_options & optname;
			break;

		case SO_TYPE:
			*mtod(m, int *) = so->so_type;
			break;

		case SO_ERROR:
			*mtod(m, int *) = so->so_error;
			so->so_error = 0;
			break;

		case SO_SNDBUF:
			*mtod(m, int *) = so->so_snd.sb_hiwat;
			break;

		case SO_RCVBUF:
			*mtod(m, int *) = so->so_rcv.sb_hiwat;
			break;

		case SO_SNDLOWAT:
			*mtod(m, int *) = so->so_snd.sb_lowat;
			break;

		case SO_RCVLOWAT:
			*mtod(m, int *) = so->so_rcv.sb_lowat;
			break;

		case SO_SNDTIMEO:
		case SO_RCVTIMEO:
		    {
			int val = (optname == SO_SNDTIMEO ?
			     so->so_snd.sb_timeo : so->so_rcv.sb_timeo);

			m->m_len = sizeof(struct timeval);
			mtod(m, struct timeval *)->tv_sec = val / hz;
			mtod(m, struct timeval *)->tv_usec =
			    (val % hz) / tick;
			break;
		    }

#ifdef TNC
		case VS_BINDLOCAL:
			*mtod(m, int *) = so->vs_flags & optname;
			break;
#endif
		default:
			(void)m_free(m);
			error = ENOPROTOOPT;
			goto bad;
		}
		*mp = m;
	}
bad:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

void
sohasoutofband(so)
	register struct socket *so;
{
	LOCK_ASSERT("sohasoutofband so", SOCKET_ISLOCKED(so));
	LOCK_ASSERT("sohasoutofband sb", SOCKBUF_ISLOCKED(&so->so_rcv));

	unix_master();
	if (so->so_pgid < 0) {
		register struct vproc *g;
		g = LOCATE_VPROC_PID(-so->so_pgid);
		if (g != 0) {
			(void) VPOP_SIGPGRP(g, SIGURG, 0, NULL, VPROC_HAS_PRIV);
			VPROC_RELEASE(g,"sohasoutofband()");
		}
	}
	else if (so->so_pgid > 0) {
		register struct vproc *v;
		v = LOCATE_VPROC_PID(so->so_pgid);
		if (v != 0) {
			(void) VPOP_SIGPROC(v, SIGURG, 0, VPROC_HAS_PRIV);
			VPROC_RELEASE(v,"sohasoutofband()");
		}
	}

#if	PARALLEL_SELECT
	unix_release();
	select_wakeup(&so->so_rcv.sb_selq);
#else
	if (so->so_rcv.sb_sel) {
		selwakeup(so->so_rcv.sb_sel, so->so_rcv.sb_flags & SB_COLL);
		so->so_rcv.sb_sel = 0;
		so->so_rcv.sb_flags &= ~SB_COLL;
	}
	unix_release();
#endif
}

/*
 * "Accept" the first queued connection.
 */
sodequeue(head, so, nam, compat_43)
	struct socket *head, **so;
	struct mbuf **nam;
{
	int error = 0;
	struct mbuf *m;
	struct socket *aso;
	DOMAIN_FUNNEL_DECL(f)

	if (nam)
		*nam = 0;
	*so = 0;
	DOMAIN_FUNNEL(sodomain(head), f);
	SOCKET_LOCK(head);
	if ((head->so_options & SO_ACCEPTCONN) == 0) {
		error = EINVAL;
		goto bad;
	}
again:
	if (head->so_qlen == 0) {
		error = ENOTCONN;
		goto bad;
	}
	if (head->so_error) {
		error = head->so_error;
		head->so_error = 0;
		goto bad;
	}
	/*
	 * Other threads may race this accept when we unlock "head" in
	 * order to follow proper lock hierarchy. We dequeue the _first_
	 * on so_q, and protect it (from sofree or other deq's) with
	 * head->so_dqlen. If we lose such a race, the thread that comes
	 * in last calls sofree(). Note sofree checks NOFDREF, etc. No
	 * race can occur if !NETSYNC_LOCK.
	 */
	if (head->so_special & SP_CLOSING) {
		error = ECONNABORTED;	/* paranoia */
		goto bad;
	}
	aso = head->so_q;
	++head->so_dqlen;
	SOCKET_UNLOCK(head);
	SOCKET_LOCK(aso);
	SOCKET_LOCK(head);
	if (aso != head->so_q) {		/* Didn't win race */
		SOCKET_UNLOCK(aso);
		aso = 0;
	}
	if (--head->so_dqlen <= 0 && head->so_dq) /* Last does any cleanup */
		sodqfree(head);
	if (aso == 0)				/* Back to starting block */
		goto again;

	if (soqremque(aso, 1) == 0)
		panic("sodequeue");
	aso->so_state &= ~SS_NOFDREF;
	SOCKET_UNLOCK(head);
	SOCKET_UNLOCK(aso);
	DOMAIN_UNFUNNEL(f);
	m = m_getclr(M_WAIT, MT_SONAME);
	(void) soaccept(aso, m);
	*so = aso;
	if (nam) {
		if (compat_43)
			sockaddr_old(m);
		*nam = m;
	} else
		m_freem(m);
	return error;

bad:
	SOCKET_UNLOCK(head);
	DOMAIN_UNFUNNEL(f);
	return error;
}

sogetaddr(so, nam, which, compat_43)
	struct socket *so;
	struct mbuf **nam;
{
	int error;
	struct mbuf *m = 0;
	DOMAIN_FUNNEL_DECL(f)

	*nam = 0;
	m = m_getclr(M_WAIT, MT_SONAME);
	if (m == NULL) 
		return ENOBUFS;

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	if (which && (so->so_state & (SS_ISCONNECTED|SS_ISCONFIRMING)) == 0) {
		error = ENOTCONN;
		goto bad;
	}
	error = (*so->so_proto->pr_usrreq)(so,
				which ? PRU_PEERADDR : PRU_SOCKADDR,
				(struct mbuf *)0, m, (struct mbuf *)0);
	if (error == 0) {
		if (compat_43)
			sockaddr_old(m);
		*nam = m;
	}
bad:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	if (error)
		m_freem(m);
	return error;
}
