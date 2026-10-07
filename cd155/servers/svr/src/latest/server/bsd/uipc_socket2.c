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
 * $Log: uipc_socket2.c,v $
 * Revision 1.9  1995/02/01  21:30:37  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:28:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/03/14  02:01:02  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.6  1993/09/01  01:34:57  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.5  1993/07/14  17:51:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  18:51:42  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:08:06  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:26:01  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.2  1992/11/06  18:22:04  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/06  00:07:54  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.15  93/08/06  17:52:44  bhk
 * Removed an extranious VSOP_SLEEP call
 * 
 * Revision 2.14  92/10/29  19:32:06  bhk
 * Unvirtualized a sosleep call in sosbwait that was causing crashes
 * 
 * Revision 2.13  92/08/17  12:50:37  mjl
 * Got rid of extraneous arg to VSOP_SLEEP().
 * 
 * Revision 2.12  92/08/08  01:36:23  jdh
 * modified sosleep() to VSOP_SLEEP() with a flags parameter
 * XXX don't need -- remove extra field -- jdh
 * 
 * Revision 2.11  92/06/05  13:56:41  klh
 * 	Revision 2.8  92/05/18  12:30:26  roy
 * 		Incorporate socket select changes from 1.1.
 * 		Only select_wakeup when a socket buffer had a select outstanding,
 * 		and as long as the (single) select queue is not empty.
 * 		sbselqueue/sbseldequeue are no longer used, but remain for compat.
 * 		(loverso)
 * 
 * Revision 2.10  92/04/14  10:08:20  roman
 * Replace missing master lock and correct VPOP_SIGPROC() call.
 * 
 * Revision 2.9  92/04/06  10:44:28  mjl
 * Add TNC hooks to sosleep().  If socket is relocating, return ERESTART to
 * any blocked socket system calls.
 * 
 * Revision 2.8  92/03/24  08:59:37  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.6  92/03/09  14:34:37  durriya
 * 	Revision 3.3  92/01/30  16:05:05  sp
 * 	Remove MACH_NO_KERNEL conditionals
 * 
 * 	Revision 3.2  91/12/18  17:15:56  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.5  91/11/22  14:55:04  rabii
 * 	VPOP_SIGPGRP has additional parameter. (chrisp)
 * 
 * 	Allow LOCATE_VPROC_PID() to return a null pointer, which must be
 * 	checked for and recovered from. (roman)
 * 
 * 	Changes to arguments required by VPOP_SIG* operations. (chrisp)
 * 
 * Revision 2.4  91/10/04  14:54:32  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:51:52  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Change to use process group ids in place of pgrp structures.
 * 
 * Revision 2.2  91/08/31  13:24:39  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:32:05  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.15.8.2  91/04/29  12:10:14  tmt
 * 	Enhance sb_lowat computation in soreserve().
 * 	[91/04/10  09:49:40  tmt]
 * 
 * Revision 1.15.6.2  91/03/15  17:46:20  tmt
 * 	Fix unlock and wakeup WAKEONE handling.
 * 	[91/03/13  19:00:43  tmt]
 * 
 * Revision 1.15  90/10/31  13:50:19  devrcs
 * 	Rearrange soqinsque, soqremque.
 * 	[90/10/13  09:48:25  tmt]
 * 
 * 	Don't do sonewsock or soisconnected so_q ops when listener is
 * 	closing. Deal with new so_dq in soqremque and soqinsque. Copy
 * 	only the SP_INHERIT bits in sonewsock. Fix unlock_socheck.
 * 	[90/10/12  17:45:00  tmt]
 * 
 * Revision 1.14  90/10/07  13:20:52  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:03:47  gm]
 * 
 * 	Remove include of sys/buf.h
 * 	[90/09/29  18:06:17  tmt]
 * 
 * 	Move SS_FREEABLE and SS_LOCKABLE to SP* in so_special.
 * 	Add SE_POLL bit to sbwakeup operation, always call wakeup.
 * 	[90/09/20  18:44:53  tmt]
 * 
 * Revision 1.13  90/09/13  11:42:47  devrcs
 * 	Sosleep() now uses tsleep()! Modify callers.
 * 	Note use of "force unfunnel" in sosleep().
 * 	[90/08/28  11:52:07  tmt]
 * 
 * 	Handle timeouts in sosleep. Use tsleep if !MACH.
 * 	[90/08/24  12:45:44  tmt]
 * 
 * Revision 1.12  90/07/27  08:45:13  devrcs
 * 	Update to BSD Reno release.
 * 	Fix several locking problems.
 * 	Pass timeout to sosleep (unused yet).
 * 	[90/07/19  15:39:14  tmt]
 * 
 * Revision 1.11  90/07/05  23:08:24  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	[90/07/03  18:43:22  tmt]
 * 
 * Revision 1.10  90/06/22  20:07:53  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:54:53  seiden]
 * 
 * 	Change sleep priority calculation. Fix sbwakeup typos.
 * 	[90/06/16  20:14:19  tmt]
 * 
 * 	Use SOHASUAREA macro. Rearrange code in sosleep and be a
 * 	bit more paranoid about procp. Add illuminating comment :-).
 * 	Rearrange code and #ifdefs slightly.
 * 	[90/06/12  18:05:45  tmt]
 * 
 * 	nags merge
 * 	[90/06/12  21:18:19  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:41:37  seiden]
 * 
 * 	Tune lock debugging printfs. Add string for recursive testing.
 * 	Remove MMAX_MP tokens and replace with PARALLEL_SELECT.
 * 	Modify sbpoll for XTI.
 * 	[90/06/06  14:11:06  tmt]
 * 
 * Revision 1.9  90/05/13  18:34:46  devrcs
 * 	Add and use SS_NOUAREA bit. Do XTI socket wakeup routines.
 * 	Tune out sockbuf assertions in certain cases.
 * 	[90/05/05  15:47:04  tmt]
 * 
 * Revision 1.8  90/04/27  18:53:43  devrcs
 * 	No more longjmp's from sleepers. Flags in sockbuf also indicate
 * 	interruptibility (for NFS, e.g.). Add wakeup_one for nfsd's.
 * 	Add sb_wakeup call for XTI (not quite done).
 * 	[90/04/20  12:19:24  tmt]
 * 
 * Revision 1.7  90/04/14  00:30:33  devrcs
 * 	Add domain refcnt'ing for dynamic domain framework.
 * 	Add lotsa void's.  Refine lock debugging a little.
 * 	[90/04/09  18:29:39  tmt]
 * 
 * Revision 1.6  90/03/27  13:15:19  gm
 * 	Modify sosleep() to set so_error and return synchronously
 * 	if called by other than sbwait and sblock.
 * 	[90/03/09  12:41:54  tmt]
 * 
 * Revision 1.5  90/02/05  15:47:58  robert
 * 	Use more macros for locks.
 * 	[90/01/19  14:11:45  tmt]
 * 
 * Revision 1.4  90/01/18  08:42:39  gm
 * 	Remove spurious spl0 in sosleep(), use SPL macros in case !NETSYNC_SPL.
 * 	[90/01/12  17:15:22  tmt]
 * 
 * 	Restore soqremque and do head==NULL check in caller.
 * 	Add assertions to sblock/unlock.
 * 	Fix spl stuff in sosleep (spl0 is a void under (at least) Unix!).
 * 	Expand m_copy macro to m_copym.
 * 	[90/01/08  15:39:44  tmt]
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
 * Revision 1.3  90/01/03  11:52:24  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:28:13  gm]
 * 
 * Revision 1.2  89/12/26  09:24:14  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)uipc_socket2.c	2.1 16:10:48 4/20/90 SecureWare */
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
 *	Merged: uipc_socket2.c	7.15 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"
#if	MACH
#include <sys/secdefines.h>
#endif

#include "sys/param.h"
#include "sys/systm.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/vproc.h"
#include "sys/file.h"

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
#include "kern/queue.h"

#if	SEC_ARCH
#include <sys/security.h>
#endif
#if	SEC_ILB
#include <sys/secpolicy.h>
#endif
#endif

LOCK_ASSERTL_DECL

/*
 * Primitive routines for operating on sockets and socket buffers
 */

u_long	sb_max = SB_MAX;		/* patchable */

/*
 * Procedures to manipulate state flags of socket
 * and do appropriate wakeups.  Normal sequence from the
 * active (originating) side is that soisconnecting() is
 * called during processing of connect() call,
 * resulting in an eventual call to soisconnected() if/when the
 * connection is established.  When the connection is torn down
 * soisdisconnecting() is called during processing of disconnect() call,
 * and soisdisconnected() is called when the connection to the peer
 * is totally severed.  The semantics of these routines are such that
 * connectionless protocols can call soisconnected() and soisdisconnected()
 * only, bypassing the in-progress calls when setting up a ``connection''
 * takes no time.
 *
 * From the passive side, a socket is created with
 * two queues of sockets: so_q0 for connections in progress
 * and so_q for connections already made and awaiting user acceptance.
 * As a protocol is preparing incoming connections, it creates a socket
 * structure queued on so_q0 by calling sonewconn().  When the connection
 * is established, soisconnected() is called, and transfers the
 * socket structure to so_q, making it available to accept().
 * 
 * If a socket is closed with sockets on either
 * so_q0 or so_q, these sockets are dropped.
 *
 * If higher level protocols are implemented in
 * the kernel, the wakeups done here will sometimes
 * cause software-interrupt process scheduling.
 */

void
soisconnecting(so)
	register struct socket *so;
{

	LOCK_ASSERT("soisconnecting", SOCKET_ISLOCKED(so));
	so->so_state &= ~(SS_ISCONNECTED|SS_ISDISCONNECTING);
	so->so_state |= SS_ISCONNECTING;
}

void
soisconnected(so)
	register struct socket *so;
{
	register struct socket *head = so->so_head;

	LOCK_ASSERT("soisconnected", SOCKET_ISLOCKED(so));
	so->so_state &= ~(SS_ISCONNECTING|SS_ISDISCONNECTING|SS_ISCONFIRMING);
	so->so_state |= SS_ISCONNECTED;
	if (head) {
		SOCKET_LOCK(head);
		/*
		 * So_dqlen is safe here because both locks are held, but
		 * we don't move things around while soclose is active.
		 */
		if (!(head->so_special & SP_CLOSING) && soqremque(so, 0)) {
			soqinsque(head, so, 1);
			sorwakeup(head);
			wakeup((caddr_t)&head->so_timeo);
		}
		SOCKET_UNLOCK(head);
	} else {
		wakeup((caddr_t)&so->so_timeo);
		sorwakeup(so);
		sowwakeup(so);
	}
}

void
soisdisconnecting(so)
	register struct socket *so;
{

	LOCK_ASSERT("soisdisconnecting",SOCKET_ISLOCKED(so));
	so->so_state &= ~SS_ISCONNECTING;
	so->so_state |= (SS_ISDISCONNECTING|SS_CANTRCVMORE|SS_CANTSENDMORE);
	wakeup((caddr_t)&so->so_timeo);
	sowwakeup(so);
	sorwakeup(so);
}

void
soisdisconnected(so)
	register struct socket *so;
{

	LOCK_ASSERT("soisdisconnected", SOCKET_ISLOCKED(so));
	so->so_state &= ~(SS_ISCONNECTING|SS_ISCONNECTED|SS_ISDISCONNECTING);
	so->so_state |= (SS_CANTRCVMORE|SS_CANTSENDMORE);
	wakeup((caddr_t)&so->so_timeo);
	sowwakeup(so);
	sorwakeup(so);
}

/*
 * When an attempt at a new connection is noted on a socket
 * which accepts connections, sonewconn is called.  If the
 * connection is possible (subject to space constraints, etc.)
 * then we allocate a new structure, properly linked into the
 * data structure of the original socket, and return this.
 * Connstatus may be 0, or SO_ISCONFIRMING, or SO_ISCONNECTED.
 *
 * Currently, sonewconn() is defined as sonewsock() in socketvar.h
 * to catch calls that are missing the (new) second parameter.
 */
struct socket *
sonewsock(head, connstatus)
	register struct socket *head;
	int connstatus;
{
	register struct socket *so;
	int soqueue = connstatus ? 1 : 0;

	LOCK_ASSERT("sonewsock", SOCKET_ISLOCKED(head));
	if (head->so_special & SP_CLOSING)
		goto bad;
	if (head->so_qlen + head->so_q0len > 3 * head->so_qlimit / 2)
		goto bad;
	NET_MALLOC(so, struct socket *, sizeof(*so), M_SOCKET, M_NOWAIT);
	if (so == NULL)
		goto bad;
	bzero((caddr_t)so, sizeof(*so));
	so->so_type = head->so_type;
	so->so_options = head->so_options &~ SO_ACCEPTCONN;
	so->so_linger = head->so_linger;
	so->so_state = (head->so_state | SS_NOFDREF) & ~SS_PRIV;
	so->so_special = head->so_special & SP_INHERIT;
	so->so_proto = head->so_proto;
	so->so_timeo = head->so_timeo;
	so->so_pgid = head->so_pgid;
	so->so_rcv.sb_flags = head->so_rcv.sb_flags & SB_INHERIT;
	so->so_snd.sb_flags = head->so_snd.sb_flags & SB_INHERIT;
#if	SEC_ARCH
	bcopy(head->so_tag, so->so_tag, sizeof so->so_tag);
#if	SEC_ILB
	if (sp_alloc_object(so->so_tag)) {
		NET_FREE(so, M_SOCKET);
		goto bad;
	}
	SP_EMPTY_OBJECT(so->so_tag);
#endif	/* SEC_ILB */
#endif 	/* SEC_ARCH */
#if	NETSYNC_LOCK
	{
	struct socklocks *lp;
	NET_MALLOC(lp, struct socklocks *, sizeof (*lp), M_SOCKET, M_NOWAIT);
	if (lp == NULL) {
		NET_FREE(so, M_SOCKET);
		goto bad;
	}
	SOCKET_LOCKINIT(so, lp);
	++lp->refcnt;
	}
#endif
#if	PARALLEL_SELECT
	queue_init(&so->so_snd.sb_selq);
	queue_init(&so->so_rcv.sb_selq);
#endif
	/* Since the refcnt is !0 due to head, it's not
	 * necessary to lock the domain list for the ++refcnt. */
	DOMAINRC_REF(sodomain(so));
	SOCKET_LOCK(so);
	(void) soreserve(so, head->so_snd.sb_hiwat, head->so_rcv.sb_hiwat);
	soqinsque(head, so, soqueue);
	if ((*so->so_proto->pr_usrreq)(so, PRU_ATTACH,
	    (struct mbuf *)0, (struct mbuf *)0, (struct mbuf *)0)) {
		if (so->so_head) {
			if (so->so_head != head)	/* ??? */
				SOCKET_LOCK(so->so_head);
			(void) soqremque(so, soqueue);
			if (so->so_head != head)	/* ??? */
				SOCKET_UNLOCK(so->so_head);
		}
		SOCKET_UNLOCK(so);
#if	NETSYNC_LOCK
		NET_FREE(so->so_lock, M_SOCKET);
#endif
		DOMAINRC_UNREF(sodomain(so));
		NET_FREE(so, M_SOCKET);
		goto bad;
	}
	if (connstatus) {
		sorwakeup(head);
		wakeup((caddr_t)&head->so_timeo);
		so->so_state |= connstatus;
	}
	SOCKET_UNLOCK(head);
	return (so);

bad:
	SOCKET_UNLOCK(head);
	return (struct socket *)0;
}

void
soqinsque(head, so, q)
	register struct socket *head, *so;
	int q;
{
	register struct socket **prev;

	LOCK_ASSERT("soqinsque head", SOCKET_ISLOCKED(head));
	LOCK_ASSERT("soqinsque so", SOCKET_ISLOCKED(so));
	so->so_head = head;
	if (q == 0) {
		head->so_q0len++;
		so->so_q0 = 0;
		for (prev = &(head->so_q0); *prev; )
			prev = &((*prev)->so_q0);
	} else if (q > 0) {
		head->so_qlen++;
		so->so_q = 0;
		for (prev = &(head->so_q); *prev; )
			prev = &((*prev)->so_q);
	} else {
		/* so_dqlen means something else */
		so->so_dq = 0;
		for (prev = &(head->so_dq); *prev; )
			prev = &((*prev)->so_dq);
	}
	*prev = so;
}

soqremque(so, q)
	register struct socket *so;
	int q;
{
	register struct socket *head, *prev, *next;

	head = so->so_head;
	LOCK_ASSERT("soqremque head", SOCKET_ISLOCKED(head));
	LOCK_ASSERT("soqremque so", SOCKET_ISLOCKED(so));
	prev = head;
	for (;;) {
		if (q == 0)
			next = prev->so_q0;
		else if (q > 0)
			next = prev->so_q;
		else
			next = prev->so_dq;
		if (next == so)
			break;
		if (next == 0)
			return (0);
		prev = next;
	}
	if (q == 0) {
		prev->so_q0 = next->so_q0;
		head->so_q0len--;
	} else if (q > 0) {
		prev->so_q = next->so_q;
		head->so_qlen--;
	} else {
		prev->so_dq = next->so_dq;
	}
	next->so_q0 = next->so_q = next->so_dq = 0;
	next->so_head = 0;
	return (1);
}

/*
 * Socantsendmore indicates that no more data will be sent on the
 * socket; it would normally be applied to a socket when the user
 * informs the system that no more data is to be sent, by the protocol
 * code (in case PRU_SHUTDOWN).  Socantrcvmore indicates that no more data
 * will be received, and will normally be applied to the socket by a
 * protocol when it detects that the peer will send no more data.
 * Data queued for reading in the socket may yet be read.
 */

void
socantsendmore(so)
	struct socket *so;
{

	LOCK_ASSERT("socantsendmore", SOCKET_ISLOCKED(so));
	so->so_state |= SS_CANTSENDMORE;
	sowwakeup(so);
}

void
socantrcvmore(so)
	struct socket *so;
{

	LOCK_ASSERT("socantrecvmore", SOCKET_ISLOCKED(so));
	so->so_state |= SS_CANTRCVMORE;
	sorwakeup(so);
}

/*
 * Socket select/wakeup routines.
 */

/*
 * Queue a process for a select on a socket buffer.
#if	PARALLEL_SELECT
 * In the parallel environment we can't simply store the thread pointer
 * due to the race between setting it and the select sleep(). So we
 * use events. In the uniprocessor MACH environment we use the
 * current thread structure instead of the proc.
#endif
 */

#if	MACH
#if	OSF1_SERVER
#include <uxkern/import_mach.h>
#else	/* OSF1_SERVER */
#include "kern/processor.h"
#include "kern/thread.h"
#include "kern/sched_prim.h"
#endif	/* OSF1_SERVER */
#endif

void
sbselqueue(sb)
	struct sockbuf *sb;
{
#if	!MACH
	struct proc *p;
#endif

	LOCK_ASSERT("sbselqueue", SOCKBUF_ISLOCKED(sb));
#if	PARALLEL_SELECT
	select_enqueue(&sb->sb_selq);
#else
#if	MACH
	if (sb->sb_sel && ((thread_t)(sb->sb_sel))->wait_event == (int)&selwait)
		sb->sb_flags |= SB_COLL;
	else
		sb->sb_sel = (struct proc *) current_thread();
#else	/* UNIX */
	if ((p = sb->sb_sel) && p->p_wchan == (caddr_t)&selwait)
		sb->sb_flags |= SB_COLL;
	else
		sb->sb_sel = u.u_procp;
#endif
#endif
	sb->sb_flags |= SB_SEL;
}

#if	PARALLEL_SELECT
void
sbseldequeue(sb)
	struct sockbuf *sb;
{
	LOCK_ASSERT("sbseldequeue", SOCKBUF_ISLOCKED(sb));

	select_dequeue(&sb->sb_selq);
}
#endif

/*
 * Sockbuf lock/unlock.
 */

sosblock(sb, so)
	register struct sockbuf *sb;
	struct socket *so;
{
	int error = 0;

	LOCK_ASSERT("sosblock", SOCKET_ISLOCKED(so));

	SOCKBUF_LOCK(sb);
	while (sb->sb_flags & SB_LOCK) {
		sb->sb_flags |= SB_WANT;
		if (!SOHASUAREA(so)) {	/* After SB_WANT for wakeup later */
			SOCKBUF_UNLOCK(sb);
			return EWOULDBLOCK;
		}
		assert_wait((caddr_t)&sb->sb_flags, !(sb->sb_flags&SB_NOINTR));
		SOCKBUF_UNLOCK(sb);
#if	MACH
		error = sosleep(so, (caddr_t)0, (sb->sb_flags & SB_NOINTR) ?
				(PZERO+1) : (PZERO+1) | PCATCH, 0);
#else
		error = sosleep(so, (caddr_t)&sb->sb_flags,
				(sb->sb_flags & SB_NOINTR) ?
				(PZERO+1) : (PZERO+1) | PCATCH, 0);
#endif
		if (error)
			return error;
		SOCKBUF_LOCK(sb);
	}
	sb->sb_flags |= SB_LOCK;
	return 0;
}

void
sbunlock(sb)
	register struct sockbuf *sb;
{
	LOCK_ASSERT("sbunlock", SOCKBUF_ISLOCKED(sb));

	sb->sb_flags &= ~SB_LOCK;
	if (sb->sb_flags & (SB_WANT|SB_WAKEONE)) {
		sb->sb_flags &= ~SB_WANT;
#if	MACH
		if (sb->sb_flags & SB_WAKEONE)
			wakeup_one((caddr_t)&sb->sb_flags);
		else
#endif
		wakeup((caddr_t)&sb->sb_flags);
		if (sb->sb_wakeup)
			(void) sbwakeup((struct socket *)0, sb, 1);
	}
	SOCKBUF_UNLOCK(sb);
}

/*
 * Wait for data to arrive at/drain from a socket buffer.
 * Note: returns an unlocked sockbuf.
 */
sosbwait(sb, so)
	struct sockbuf *sb;
	struct socket *so;
{
	LOCK_ASSERT("sosbwait", SOCKET_ISLOCKED(so));

	if (!SOHASUAREA(so))
		return EWOULDBLOCK;
	sb->sb_flags |= SB_WAIT;
	assert_wait((caddr_t)&sb->sb_cc, !(sb->sb_flags & SB_NOINTR));
	sbunlock(sb);
#if	MACH
	return sosleep(so, (caddr_t)0, (sb->sb_flags & SB_NOINTR) ?
			(PZERO+1) : (PZERO+1) | PCATCH, so->so_timeo);
#else
	return sosleep(so, (caddr_t)&sb->sb_cc, (sb->sb_flags & SB_NOINTR) ?
			(PZERO+1) : (PZERO+1) | PCATCH, so->so_timeo);
#endif
}

/*
 * Sleep on an address within a socket. Executes tsleep()
 * after first (maybe) releasing socket lock and unfunnelling.
 * Restores these conditions and returns tsleep() value.
 */
sosleep(so, addr, pri, tmo)
	struct socket *so;
	caddr_t addr;
	int pri, tmo;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	if (!SOHASUAREA(so))	/* Insurance - no PCATCH or interruptible */
		pri = PZERO;
#if	MACH
	if (addr) {
		assert_wait(addr, (pri & PCATCH) != 0);
		addr = 0;
	}
#endif
	SOCKET_UNLOCK(so);	/* Unlock and unfunnel before sleep */
	DOMAIN_UNFUNNEL_FORCE(sodomain(so), f);

	error = tsleep(addr, pri, "network", tmo);

	DOMAIN_UNFUNNEL(f);	/* (actually, refunnel) */
	SOCKET_LOCK(so);
#ifdef  TNC
        if ( !error )
                error = TNC_SOSLEEP_HOOK(so);
#endif
	return error;
}

/*
 * Wakeup processes waiting on a socket buffer.
 * Do asynchronous notification via SIGIO
 * if the socket has the SS_ASYNC flag set.
 */
void
sowakeup(so, sb)
	register struct socket *so;
	register struct sockbuf *sb;
{
	LOCK_ASSERT("sowakeup", SOCKET_ISLOCKED(so));
	SOCKBUF_LOCK(sb);
#if	PARALLEL_SELECT
	if ((sb->sb_flags & SB_SEL) && !queue_empty(&so->so_rcv.sb_selq)) {
		sb->sb_flags &= ~SB_SEL;
		select_wakeup(&so->so_rcv.sb_selq);
	}
#else
	if (sb->sb_sel) {
		selwakeup(sb->sb_sel, sb->sb_flags & SB_COLL);
		sb->sb_sel = 0;
		sb->sb_flags &= ~(SB_SEL|SB_COLL);
	}
#endif
	if (sb->sb_flags & (SB_WAIT|SB_WAKEONE)) {
		sb->sb_flags &= ~SB_WAIT;
#if	MACH
		if (sb->sb_flags & SB_WAKEONE)
			wakeup_one((caddr_t)&sb->sb_cc);
		else
#endif
		wakeup((caddr_t)&sb->sb_cc);
	}
	if (sb->sb_wakeup)
		(void) sbwakeup(so, sb, 1);
	SOCKBUF_UNLOCK(sb);
	if (so->so_state & SS_ASYNC) {
		unix_master();
		if (so->so_pgid < 0) {
			register struct vproc *g;
			g = LOCATE_VPROC_PID(-so->so_pgid);
			if (g != 0) {
				(void)VPOP_SIGPGRP(g, SIGIO, 0, NULL, VPROC_HAS_PRIV);
				VPROC_RELEASE(g,"sowakeup()");
			}
		}
		else if (so->so_pgid > 0) {
			register struct vproc *v;
			v = LOCATE_VPROC_PID(so->so_pgid);
			if (v != 0) {
				(void)VPOP_SIGPROC(v, SIGIO, 0, VPROC_HAS_PRIV);
				VPROC_RELEASE(v,"sowakeup()");
			}
		}
		unix_release();
	}
}

/*
 * Notify alternate wakeup routine of new state. The bits are
 * for XTI at the moment and encode the following:
 *	disconn ordrel conn connconfirm data oobdata
 * When any of these are valid the high bit is set, if the
 * word is all 0, a previous failed lock attempt may be retried.
 *
 * Note: while the socket and sockbuf may be accessed from this
 * upcall (e.g. for determining sbspace, state, etc), it is not
 * possible to perform an action such as sending or receiving,
 * or to modify socket values. That must be done later from a
 * safe context.
 */
int
sbwakeup(so, sb, what)
	struct socket *so;
	struct sockbuf *sb;
	int what;
{
	int state;

	LOCK_ASSERT("sbwakeup sb", SOCKBUF_ISLOCKED(sb));

	/* Encode state */
	if (so) {
		LOCK_ASSERT("sbwakeup so", SOCKET_ISLOCKED(so));
		state = SE_STATUS;
		if (what == 0)
			state |= SE_POLL;
		if (so->so_error)
			state |= SE_ERROR;
		if (sb->sb_cc)
			state |= SE_HAVEDATA;
		if (so->so_oobmark)
			state |= SE_HAVEOOB;
		if (sbspace(sb) <= 0)
			state |= SE_DATAFULL;
		if (so->so_state & (SS_ISCONNECTED|SS_ISCONFIRMING))
			state |= SE_CONNOUT;
		if (so->so_qlen)
			state |= SE_CONNIN;
		if (so->so_state & SS_ISCONNECTED) {
			if (!(so->so_state & SS_CANTSENDMORE))
				state |= SE_SENDCONN;
			if (!(so->so_state & SS_CANTRCVMORE))
				state |= SE_RECVCONN;
		}
	} else
		state = 0;
	if (sb->sb_wakeup)
		(*sb->sb_wakeup)(sb->sb_wakearg, state);
	return state;
}

int
sbpoll(so, sb)
	struct socket *so;
	struct sockbuf *sb;
{
	int state;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	SOCKBUF_LOCK(sb);
	state = sbwakeup(so, sb, 0);
	SOCKBUF_UNLOCK(sb);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return state;
}

/*
 * Socket buffer (struct sockbuf) utility routines.
 *
 * Each socket contains two socket buffers: one for sending data and
 * one for receiving data.  Each buffer contains a queue of mbufs,
 * information about the number of mbufs and amount of data in the
 * queue, and other fields allowing select() statements and notification
 * on data availability to be implemented.
 *
 * Data stored in a socket buffer is maintained as a list of records.
 * Each record is a list of mbufs chained together with the m_next
 * field.  Records are chained together with the m_nextpkt field. The upper
 * level routine soreceive() expects the following conventions to be
 * observed when placing information in the receive buffer:
 *
 * 1. If the protocol requires each message be preceded by the sender's
 *    name, then a record containing that name must be present before
 *    any associated data (mbuf's must be of type MT_SONAME).
 * 2. If the protocol supports the exchange of ``access rights'' (really
 *    just additional data associated with the message), and there are
 *    ``rights'' to be received, then a record containing this data
 *    should be present (mbuf's must be of type MT_RIGHTS).
 * 3. If a name or rights record exists, then it must be followed by
 *    a data record, perhaps of zero length.
 *
 * Before using a new socket structure it is first necessary to reserve
 * buffer space to the socket, by calling sbreserve().  This should commit
 * some of the available buffer space in the system buffer pool for the
 * socket (currently, it does nothing but enforce limits).  The space
 * should be released by calling sbrelease() when the socket is destroyed.
 */

soreserve(so, sndcc, rcvcc)
	register struct socket *so;
	u_long sndcc, rcvcc;
{
	int error = 0;

/* tmt - Do not assert this... evaluate
 *	LOCK_ASSERT("soreserve so", SOCKET_ISLOCKED(so)); */
	SOCKBUF_LOCK(&so->so_snd);
	SOCKBUF_LOCK(&so->so_rcv);
	if (sbreserve(&so->so_snd, sndcc) == 0)
		error = ENOBUFS;
	else if (sbreserve(&so->so_rcv, rcvcc) == 0) {
		sbrelease(&so->so_snd);
		error = ENOBUFS;
	} else {
		if (so->so_rcv.sb_lowat == 0)
			so->so_rcv.sb_lowat = 1;
		if (so->so_snd.sb_lowat == 0) {
			if (so->so_snd.sb_hiwat >= MCLBYTES * 2)
				so->so_snd.sb_lowat = MCLBYTES;
			else
				so->so_snd.sb_lowat = so->so_snd.sb_hiwat / 2;
		} else if (so->so_snd.sb_lowat > so->so_snd.sb_hiwat)
			so->so_snd.sb_lowat = so->so_snd.sb_hiwat;
	}
	SOCKBUF_UNLOCK(&so->so_rcv);
	SOCKBUF_UNLOCK(&so->so_snd);
	return (error);
}

/*
 * Allot mbufs to a sockbuf.
 * Attempt to scale mbmax so that mbcnt doesn't become limiting
 * if buffering efficiency is near the normal case.
 */
sbreserve(sb, cc)
	struct sockbuf *sb;
	u_long cc;
{

	LOCK_ASSERT("sbreserve", SOCKBUF_ISLOCKED(sb));
	if (cc > sb_max * MCLBYTES / (MSIZE + MCLBYTES))
		return (0);
	sb->sb_hiwat = cc;
	sb->sb_mbmax = min(cc * 2, sb_max);
	if (sb->sb_lowat > sb->sb_hiwat)
		sb->sb_lowat = sb->sb_hiwat;
	return (1);
}

/*
 * Free mbufs held by a socket, and reserved mbuf space.
 */
void
sbrelease(sb)
	struct sockbuf *sb;
{

	sbflush(sb);
	sb->sb_hiwat = sb->sb_mbmax = 0;
}

/*
 * Routines to add and remove
 * data from an mbuf queue.
 *
 * The routines sbappend() or sbappendrecord() are normally called to
 * append new mbufs to a socket buffer, after checking that adequate
 * space is available, comparing the function sbspace() with the amount
 * of data to be added.  sbappendrecord() differs from sbappend() in
 * that data supplied is treated as the beginning of a new record.
 * To place a sender's address, optional access rights, and data in a
 * socket receive buffer, sbappendaddr() should be used.  To place
 * access rights and data in a socket receive buffer, sbappendrights()
 * should be used.  In either case, the new data begins a new record.
 * Note that unlike sbappend() and sbappendrecord(), these routines check
 * for the caller that there will be enough space to store the data.
 * Each fails if there is not enough space, or if it cannot find mbufs
 * to store additional information in.
 *
 * Reliable protocols may use the socket send buffer to hold data
 * awaiting acknowledgement.  Data is normally copied from a socket
 * send buffer in a protocol with m_copy for output to a peer,
 * and then removing the data from the socket buffer with sbdrop()
 * or sbdroprecord() when the data is acknowledged by the peer.
 */

/*
 * Append mbuf chain m to the last record in the
 * socket buffer sb.  The additional space associated
 * the mbuf chain is recorded in sb.  Empty mbufs are
 * discarded and mbufs are compacted where possible.
 */
void
sbappend(sb, m)
	struct sockbuf *sb;
	struct mbuf *m;
{
	register struct mbuf *n;

	LOCK_ASSERT("sbappend", SOCKBUF_ISLOCKED(sb));
	if (m == 0)
		return;
	if (n = sb->sb_mb) {
		while (n->m_nextpkt)
			n = n->m_nextpkt;
		while (n->m_next)
			if (n->m_flags & M_EOR) {
				sbappendrecord(sb, m); /* XXXXXX!!!! */
				return;
			} else
				n = n->m_next;
	}
	sbcompress(sb, m, n);
}

/*
 * As above, except the mbuf chain
 * begins a new record.
 */
void
sbappendrecord(sb, m0)
	register struct sockbuf *sb;
	register struct mbuf *m0;
{
	register struct mbuf *m;

	LOCK_ASSERT("sbappendrecord", SOCKBUF_ISLOCKED(sb));
	if (m0 == 0)
		return;
	if (m = sb->sb_mb)
		while (m->m_nextpkt)
			m = m->m_nextpkt;
	/*
	 * Put the first mbuf on the queue.
	 * Note this permits zero length records.
	 */
	sballoc(sb, m0);
	if (m)
		m->m_nextpkt = m0;
	else
		sb->sb_mb = m0;
	m = m0->m_next;
	m0->m_next = 0;
	if (m && (m0->m_flags & M_EOR)) {
		m0->m_flags &= ~M_EOR;
		m->m_flags |= M_EOR;
	}
	sbcompress(sb, m, m0);
}

/*
 * As above except that OOB data
 * is inserted at the beginning of the sockbuf,
 * but after any other OOB data.
 */
void
sbinsertoob(sb, m0)
	register struct sockbuf *sb;
	register struct mbuf *m0;
{
	register struct mbuf *m;
	register struct mbuf **mp;

	LOCK_ASSERT("sbinsertoob", SOCKBUF_ISLOCKED(sb));
	if (m0 == 0)
		return;
	for (mp = &sb->sb_mb; m = *mp; mp = &((*mp)->m_nextpkt)) {
	    again:
		switch (m->m_type) {

		case MT_OOBDATA:
			continue;		/* WANT next train */

		case MT_CONTROL:
			if (m = m->m_next)
				goto again;	/* inspect THIS train further */
		}
		break;
	}
	/*
	 * Put the first mbuf on the queue.
	 * Note this permits zero length records.
	 */
	sballoc(sb, m0);
	m0->m_nextpkt = *mp;
	*mp = m0;
	m = m0->m_next;
	m0->m_next = 0;
	if (m && (m0->m_flags & M_EOR)) {
		m0->m_flags &= ~M_EOR;
		m->m_flags |= M_EOR;
	}
	sbcompress(sb, m, m0);
}

/*
 * Append address and data, and optionally, control (ancillary) data
 * to the receive queue of a socket.  If present,
 * m0 must include a packet header with total length.
 * Returns 0 if no space in sockbuf or insufficient mbufs.
 */
sbappendaddr(sb, asa, m0, control)
	register struct sockbuf *sb;
	struct sockaddr *asa;
	struct mbuf *m0, *control;
{
	register struct mbuf *m, *n;
	int space = asa->sa_len;

	LOCK_ASSERT("sbappendaddr", SOCKBUF_ISLOCKED(sb));
if (m0 && (m0->m_flags & M_PKTHDR) == 0)
panic("sbappendaddr");
	if (m0)
		space += m0->m_pkthdr.len;
	for (n = control; n; n = n->m_next) {
		space += n->m_len;
		if (n->m_next == 0)	/* keep pointer to last control buf */
			break;
	}
	if (space > sbspace(sb))
		return (0);
	if (asa->sa_len > MLEN)
		return (0);
	MGET(m, M_DONTWAIT, MT_SONAME);
	if (m == 0)
		return (0);
	m->m_len = asa->sa_len;
	bcopy((caddr_t)asa, mtod(m, caddr_t), asa->sa_len);
	if (n)
		n->m_next = m0;		/* concatenate data to control */
	else
		control = m0;
	m->m_next = control;
	for (n = m; n; n = n->m_next)
		sballoc(sb, n);
	if (n = sb->sb_mb) {
		while (n->m_nextpkt)
			n = n->m_nextpkt;
		n->m_nextpkt = m;
	} else
		sb->sb_mb = m;
	return (1);
}

sbappendcontrol(sb, m0, control)
	struct sockbuf *sb;
	struct mbuf *control, *m0;
{
	register struct mbuf *m, *n;
	int space = 0;

	LOCK_ASSERT("sbappendcontrol", SOCKBUF_ISLOCKED(sb));
	if (control == 0)
		panic("sbappendcontrol");
	for (m = control; ; m = m->m_next) {
		space += m->m_len;
		if (m->m_next == 0)
			break;
	}
	n = m;			/* save pointer to last control buffer */
	for (m = m0; m; m = m->m_next)
		space += m->m_len;
	if (space > sbspace(sb))
		return (0);
	n->m_next = m0;			/* concatenate data to control */
	for (m = control; m; m = m->m_next)
		sballoc(sb, m);
	if (n = sb->sb_mb) {
		while (n->m_nextpkt)
			n = n->m_nextpkt;
		n->m_nextpkt = control;
	} else
		sb->sb_mb = control;
	return (1);
}

/*
 * Compress mbuf chain m into the socket
 * buffer sb following mbuf n.  If n
 * is null, the buffer is presumed empty.
 */
void
sbcompress(sb, m, n)
	register struct sockbuf *sb;
	register struct mbuf *m, *n;
{
	register int eor = 0;

	LOCK_ASSERT("sbcompress", SOCKBUF_ISLOCKED(sb));
	while (m) {
		eor |= m->m_flags & M_EOR;
		if (m->m_len == 0) {
			m = m_free(m);
			continue;
		}
		if (n && (n->m_flags & (M_EXT | M_EOR)) == 0 &&
		    (n->m_data + n->m_len + m->m_len) < &n->m_dat[MLEN] &&
		    n->m_type == m->m_type) {
			bcopy(mtod(m, caddr_t), mtod(n, caddr_t) + n->m_len,
			    (unsigned)m->m_len);
			n->m_len += m->m_len;
			sb->sb_cc += m->m_len;
			m = m_free(m);
			continue;
		}
		if (n)
			n->m_next = m;
		else
			sb->sb_mb = m;
		sballoc(sb, m);
		n = m;
		m->m_flags &= ~M_EOR;
		m = m->m_next;
		n->m_next = 0;
	}
	if (n)
		n->m_flags |= eor;
}

/*
 * Free all mbufs in a sockbuf.
 * Check that all resources are reclaimed.
 */
void
sbflush(sb)
	register struct sockbuf *sb;
{

	if (sb->sb_flags & SB_LOCK)
		panic("sbflush");
	SOCKBUF_LOCK(sb);
	while (sb->sb_mbcnt)
		sbdrop(sb, (int)sb->sb_cc);
	if (sb->sb_cc || sb->sb_mb)
		panic("sbflush 2");
	SOCKBUF_UNLOCK(sb);
}

/*
 * Drop data from (the front of) a sockbuf.
 */
void
sbdrop(sb, len)
	register struct sockbuf *sb;
	register int len;
{
	register struct mbuf *m, *mn;
	struct mbuf *next;

	LOCK_ASSERT("sbdrop", SOCKBUF_ISLOCKED(sb));
	next = (m = sb->sb_mb) ? m->m_nextpkt : 0;
	while (len > 0) {
		if (m == 0) {
			if (next == 0)
				panic("sbdrop");
			m = next;
			next = m->m_nextpkt;
			continue;
		}
		if (m->m_len > len) {
			m->m_len -= len;
			m->m_data += len;
			sb->sb_cc -= len;
			break;
		}
		len -= m->m_len;
		sbfree(sb, m);
		MFREE(m, mn);
		m = mn;
	}
	while (m && m->m_len == 0) {
		sbfree(sb, m);
		MFREE(m, mn);
		m = mn;
	}
	if (m) {
		sb->sb_mb = m;
		m->m_nextpkt = next;
	} else
		sb->sb_mb = next;
}

/*
 * Drop a record off the front of a sockbuf
 * and move the next record to the front.
 */
void
sbdroprecord(sb)
	register struct sockbuf *sb;
{
	register struct mbuf *m, *mn;

	LOCK_ASSERT("sbdroprecord", SOCKBUF_ISLOCKED(sb));
	m = sb->sb_mb;
	if (m) {
		sb->sb_mb = m->m_nextpkt;
		do {
			sbfree(sb, m);
			MFREE(m, mn);
		} while (m = mn);
	}
}

#if	NETSYNC_LOCK && NETSYNC_LOCKTEST
/* Debugging aids: don't belong here */

int socket_lockhang = 1;

lock_socheck(so)
struct socket *so;
{
	if (!so) {
		printf("\tLocking null so\n");
		return 0;
	}
	if (!so->so_lock) {
		printf("\tLocking socket 0x%x with null lock\n", so);
		return 0;
	}
	if (!so->so_snd.sb_lock || !so->so_rcv.sb_lock)
		printf("\tLocking socket 0x%x with null sockbuf lock(s)\n", so);
	if (so->so_special & SP_FREEABLE)
		printf("\tLocking freeable socket 0x%x!\n", so);
#if	NETSYNC_SPL
	if (SOCKET_ISLOCKED(so)) {
		if (so->so_special & SP_LOCKABLE)
			printf("\tLocking locked socket 0x%x\n", so);
		return socket_lockhang;
	}
#endif
	return 1;
}
unlock_socheck(so)
struct socket *so;
{
	if (!so) {
		printf("\tUnlocking null so\n");
		return 0;
	}
	if (!so->so_lock) {
		printf("\tUnlocking socket 0x%x with null lock(s)\n", so);
		return 0;
	}
	if (so->so_snd.sb_lock && so->so_rcv.sb_lock) {
#if	SOCKBUF_LOCKTEST
		/* sockbuf locks are null if not */
		int a = SOCKBUF_ISLOCKED(&so->so_snd);
		int b = SOCKBUF_ISLOCKED(&so->so_rcv);
		if (a || b) {
			char *snd, *plus, *rcv;
			snd = plus = rcv = "";
			if (a) snd = "snd";
			if (b) rcv = "rcv";
			if (a && b) plus = "+";
			printf("\tUnlocking socket 0x%x with locked so->so_%s%s%s\n",
				so, snd, plus, rcv);
		}
#endif
	} else
		printf("\tUnlocking socket 0x%x with null sockbuf lock(s)\n", so);
	if (!SOCKET_ISLOCKED(so)) {
		printf("\tUnlocking unlocked socket 0x%x\n", so);
		return 0;
	}
	return 1;
}
/* Use sb to back up to socket and check locks there? Later if so. */
lock_sbcheck(sb)
struct sockbuf *sb;
{
	if (!sb->sb_lock) {
		printf("\tLocking sockbuf 0x%x with null lock\n", sb);
		return 0;
	}
#if	NETSYNC_SPL
	if (SOCKBUF_ISLOCKED(sb)) {
		printf("\tLocking locked sockbuf 0x%x\n", sb);
		return socket_lockhang;
	}
#endif
	return 1;
}
unlock_sbcheck(sb)
struct sockbuf *sb;
{
	if (!sb->sb_lock) {
		printf("\tUnlocking sockbuf 0x%x with null lock\n", sb);
		return 0;
	}
	if (!SOCKBUF_ISLOCKED(sb)) {
		printf("\tUnlocking unlocked sockbuf 0x%x\n", sb);
		return 0;
	}
	return 1;
}

#ifdef	S_LCK
char _net_lock_format_[] = "\t%s %s %d\n";
char _net_simple_lock_[] = "simple_lock";
char _net_simple_unlock_[] = "simple_unlock";
char _net_lock_write_[] = "lock_write";
char _net_lock_read_[] = "lock_read";
char _net_lock_write_to_read_[] = "lock_write_to_read";
char _net_lock_done_[] = "lock_done";
char _net_lock_recursive_[] = "lock_recursive";
#endif
#endif
