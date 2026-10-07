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
 * $Log: uipc_usrreq.c,v $
 * Revision 1.12  1995/02/24  22:25:59  stans
 *  lint picking.
 *
 *  Reviewer:self
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW07 sats
 *
 * Revision 1.11  1995/02/01  21:31:15  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:28:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/04/13  21:09:52  chrisp
 * For non-TNC, remove incorrect ASSERT(unp->unp_vnodeport) in
 * unp_destroy_binding() and replace with guard that the vnodeport
 * is not MACH_PORT_NULL before attempting port deallocation.
 *
 *  Reviewer: None.
 *  Risk: Low
 *  Benefit or PTS #: 9006
 *  Testing: Networking operates correctly for non-TNC build.
 *  Module(s):
 *
 * Revision 1.8  1994/01/13  17:51:57  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.7  1993/09/01  01:35:19  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.6  1993/07/14  17:51:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:52:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:08:32  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:26:12  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:03:25  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/15  01:47:24  cfj
 * Multiple service partition fixes from Locus.
 * 
 * Revision 1.1.2.1  1992/11/06  00:08:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.19  93/08/26  10:43:42  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.
 * 
 * Revision 2.18  93/08/23  00:04:57  mjl
 * Fix GCC-isms that i860 compiler doesn't grok.
 * 
 * Revision 2.17  93/08/22  20:05:14  nina
 * [LCCbug #0359] Call unp_destroy_binding() even if there is no vnode
 * port, since AF_UNIX datagram sockets have bind ports but no vnode
 * ports.  Modify unp_destroy_binding() to correctly tear down bind
 * ports for datagram sockets.
 * 
 * Revision 2.16  93/08/11  14:59:37  mjl
 * Add forward declaration to eliminate ANSI compiler warning.
 * 
 * Revision 2.15  93/04/24  18:48:09  klh
 * 	Revision 2.14  93/02/02  15:22:41  rabii
 * 		We now go through a linked list for looking at file
 *		structures (rabii)
 * 
 * Revision 2.14  92/12/11  14:55:09  mjl
 * Got rid of VSOP_VSOCKDESTROY() virtual socket operation; instead use
 * unp_destroy_binding() (formerly un_vsockdestroy() ), which is now properly
 * #ifdef'ed for non-TNC builds.
 * 
 * Revision 2.13  92/12/10  17:04:04  mjl
 * Changes in support of TNC datagram sockets.  For PRU_SEND requests, use
 * one of {remote,local}_append_dgram() routines.  No more VSOP_VSOCKCREATE()
 * virtual socket op.
 *
 * Revision 2.12  92/10/05  13:37:35  klh
 * 	Revision 2.12  92/08/26  12:10:25  loverso
 * 		Revision 1.19.2.6  1992/03/17  16:20:05  tmt
 * 		Release vnode and zap v_socket pointer instead of panic().
 * 
 * Revision 2.11  92/08/08  01:38:26  jdh
 * added support for setting up and tearing down the relationship
 * between sockets and the vnodes their bound to (new vsops calls) -- jdh
 * 
 * Revision 2.10  92/05/12  17:53:32  loverso
 * 	Bandaid: use so_spare to prevent using a deallocated spocket. (rabii)
 * 
 * Revision 2.9  92/05/12  00:07:10  loverso
 * 	Get rid of ni_vp1, use ni_cdir instead (pjg).
 * 
 * Revision 2.8  92/03/01  18:43:33  pjg
 * 	Release the reference aquired by VOP_CREATE in unp_vsockcreate.
 * 	The only reference(s) on the vnode in the one obtained with 
 * 	get_vnode_port.
 * 
 * Revision 2.7  92/01/14  11:04:27  roy
 * 	Fix up Unix domain socket ifdefs.
 * 
 * Revision 2.6  92/01/10  14:19:13  rabii
 * 	Added missing arguments to mach_port_deallocate calls
 * 
 * Revision 2.5  92/01/09  16:18:20  roy
 * 	Unix domain socket support for OSF1_ADFS (loverso).
 * 
 * Revision 2.4  91/10/04  14:55:01  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:53:37  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	ifdef out the code that passes file descriptors in socket messages.
 * 	This code now will not work correctly. It looks like it didn't work
 * 	before anyway.
 * 
 * Revision 2.2  91/08/31  13:25:05  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:32:16  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.14.5.2  91/03/15  17:46:30  tmt
 * 	Cast sockaddr of sender in usrreq.
 * 	[91/03/13  19:01:22  tmt]
 * 
 * Revision 1.14  90/10/07  13:21:06  devrcs
 * 	Cleanup u_file_state and fix Unix build.
 * 	[90/10/01  17:02:02  tmt]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:04:11  gm]
 * 
 * 	Restore Reno namei and VOP code under ifdef.
 * 	Hide MACH file array changes from Unix build.
 * 	[90/09/29  18:08:53  tmt]
 * 
 * 	Set times on both sides of pipe on unp_connect2.
 * 	[90/09/27  15:23:46  tmt]
 * 
 * 	Posix/AES: Set pipe creation times in unp_connect2, not unp_attach.
 * 	Merge read and write stat values so pipe appears as one entity.
 * 	[90/09/27  09:39:13  tmt]
 * 
 * 	Add access, modify, change times to unpcb for Posix/AES
 * 	compliance on pipe operations. Sigh.
 * 	[90/09/20  18:46:54  tmt]
 * 
 * Revision 1.13  90/09/23  15:43:54  devrcs
 * 	Unp_scan arg is void. Make unp_bind ASSERT a panic.
 * 	[90/09/05  17:27:07  tmt]
 * 
 * Revision 1.12  90/08/24  11:19:42  devrcs
 * 	Changes for u_file_state.
 * 	[90/08/17  17:40:28  nags]
 * 
 * Revision 1.11  90/07/27  08:45:24  devrcs
 * 	Update to BSD Reno release.
 * 	Fix security interfaces to match.
 * 	Adjust pipe size to avoid problems.
 * 	[90/07/19  15:44:11  tmt]
 * 
 * Revision 1.10  90/06/22  20:08:02  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:55:09  seiden]
 * 
 * 	Merge 1.9.6.2 and 1.9.4.4
 * 	[90/06/13  09:03:28  tmt]
 * 
 * 	Merge with parallelized vnode and file layers. Take socket lock
 * 	on SIOCGETPEERPRIV. Rearrange code and #ifdefs slightly.
 * 	[90/06/11  16:38:33  tmt]
 * 
 * 	nags merge
 * 	[90/06/12  21:18:40  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:41:51  seiden]
 * 
 * 	Increase PIPSIZ to 8192 to give 4096 atomicity.
 * 	Remove spurious extra lock in unp_connect2.
 * 	Join LOCK_ASSERT lines, ASSERT macro can't handle them.
 * 	[90/06/06  14:16:14  tmt]
 * 
 * Revision 1.9  90/04/27  18:53:54  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:22:01  tmt]
 * 
 * Revision 1.8  90/04/14  00:30:42  devrcs
 * 	Rearrange code from include files, strengthen types (void).
 * 	[90/04/09  15:40:20  tmt]
 * 
 * Revision 1.7  90/03/27  13:15:42  gm
 * 	Filesystem parallelization changes [noemi]
 * 
 * Revision 1.6  90/02/23  00:22:11  devrcs
 * 	PRU_SENSE now sets size field of stat structure.  This is needed
 * 	by FIFOs.
 * 	[90/02/18  13:49:56  ers]
 * 
 * Revision 1.5  90/02/05  15:48:02  robert
 * 	Move socket lock pair code out to uipc_socket2.c.
 * 	Use more macros for locks.
 * 	[90/01/19  14:14:37  tmt]
 * 
 * Revision 1.4  90/01/18  08:42:48  gm
 * 	Pass back unnamed AF_UNIX peeraddr if connected to unnamed peer.
 * 	Don't allow bind to non-AF_UNIX family sockaddr.
 * 	Fix locking in unp_connect2.
 * 	Expand m_copy macros to m_copym.
 * 	[90/01/08  15:40:29  tmt]
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
 * Revision 1.3  90/01/03  11:52:31  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:28:21  gm]
 * 
 * Revision 1.2  89/12/26  09:24:35  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)uipc_usrreq.c	2.1 16:10:44 4/20/90 SecureWare, Inc. */
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
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
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
 *	Base:	uipc_usrreq.c	7.13 (Berkeley) 10/19/89
 * 	Merged: uipc_usrreq.c	7.20 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"
#if	MACH
#include <sys/secdefines.h>
#endif

#include "sys/param.h"
#include "sys/user.h"
#include "sys/vnode.h"
#include "sys/mount.h"
#include "sys/file.h"
#include "sys/stat.h"
#include "sys/time.h"

#include "sys/mbuf.h"	/* XXX must appear after mount.h */
#include "sys/socket.h"
#ifdef TNC
#include <vsocket/vsocket.h>
#include <vsocket/vs_types.h>
#include <tnc/un.h>	/* debugging variables and info in here */
#else
#include "sys/so_defs.h"
#endif
#include "sys/socketvar.h"
#include "sys/domain.h"
#include "sys/protosw.h"
#include "sys/unpcb.h"
#include "sys/un.h"

#include "net/net_malloc.h"

#if	MACH
#if	SEC_ARCH
#include <sys/security.h>
#include <sys/secpolicy.h>
#include <sys/ioctl.h>

extern caddr_t findrights();
#endif
#endif

LOCK_ASSERTL_DECL

/*
 * Unix communications domain.
 *
 * TODO:
 *	SEQPACKET, RDM
 *	rethink name space problems
 *	need a proper out-of-band
 */
CONST struct	sockaddr sun_noname = { sizeof(sun_noname), AF_UNIX };
ino_t	unp_vno;			/* prototype for fake vnode numbers */
#if	NETSYNC_LOCK
simple_lock_data_t	global_unpconn_lock;
simple_lock_data_t	unp_misc_lock;
#endif


void
uipc_init()
{
	UNPCONN_LOCKINIT();
	UNPMISC_LOCKINIT();
}

/*ARGSUSED*/
uipc_usrreq(so, req, m, nam, control)
	struct socket *so;
	int req;
	struct mbuf *m, *nam, *control;
{
	struct unpcb *unp = sotounpcb(so);
	register struct socket *so2;
	register int error = 0;

	if (req == PRU_CONTROL) {
#if	SEC_ARCH
		/* XXX m == scalar, nam == int * */
		if ((int) m == SIOCGPEERPRIV) {
			if (unp->unp_conn == (struct unpcb *) 0)
				return ENOTCONN;
			so2 = unp->unp_conn->unp_socket;
			/* Don't lock so2 for this */
			*(int *) nam = (so2->so_state & SS_PRIV) != 0;
			return 0;
		}
#endif
		return (EOPNOTSUPP);
	}
	if (req != PRU_SEND && control && control->m_len) {
		error = EOPNOTSUPP;
		goto release;
	}
	if (unp == 0 && req != PRU_ATTACH) {
		error = EINVAL;
		goto release;
	}

	LOCK_ASSERT("uipc_usrreq", SOCKET_ISLOCKED(so));

	switch (req) {

	case PRU_ATTACH:
		if (unp) {
			error = EISCONN;
			break;
		}
		error = unp_attach(so);
		break;

	case PRU_DETACH:
		unp_detach(unp);
		break;

	case PRU_BIND:
		SOCKET_UNLOCK(so);		/* namei may be interrupted */
		error = unp_bind(unp, nam);
		SOCKET_LOCK(so);
		break;

	case PRU_LISTEN:
#ifdef OSF1_ADFS
		if (unp->unp_vnodeport == 0)
#else
		if (unp->unp_vnode == 0)
#endif
			error = EINVAL;
		break;

	case PRU_CONNECT:
		error = unp_connect(so, nam);
		break;

	case PRU_CONNECT2:
		error = unp_connect2(so, (struct socket *)nam);
		break;

	case PRU_DISCONNECT:
		unp_disconnect(unp);
		break;

	case PRU_ACCEPT:
		/*
		 * Pass back name of connected socket,
		 * if it was bound and we are still connected
		 * (our peer may have closed already!).
		 */
		if (unp->unp_conn && unp->unp_conn->unp_addr) {
			nam->m_len = unp->unp_conn->unp_addr->m_len;
			bcopy(mtod(unp->unp_conn->unp_addr, caddr_t),
			    mtod(nam, caddr_t), (unsigned)nam->m_len);
		} else {
			nam->m_len = sizeof(sun_noname);
			*(mtod(nam, struct sockaddr *)) = sun_noname;
		}
		break;

	case PRU_SHUTDOWN:
		socantsendmore(so);
		unp_usrclosed(unp);
		break;

	case PRU_RCVD:
		switch (so->so_type) {

		case SOCK_DGRAM:
			panic("uipc 1");
			/*NOTREACHED*/

		case SOCK_STREAM:
#define	rcv (&so->so_rcv)
#define snd (&so2->so_snd)
			if (unp->unp_conn == 0)
				break;
			so2 = unp->unp_conn->unp_socket;
			LOCK_ASSERT("uipc_usrreq PRU_RCVD STREAM so2notso", (so->so_lock == so2->so_lock));
			SOCKBUF_LOCK(rcv);
			SOCKBUF_LOCK(snd);
			/*
			 * Adjust backpressure on sender
			 * and wakeup any waiting to write.
			 */
			snd->sb_mbmax += unp->unp_mbcnt - rcv->sb_mbcnt;
			unp->unp_mbcnt = rcv->sb_mbcnt;
			snd->sb_hiwat += unp->unp_cc - rcv->sb_cc;
			unp->unp_cc = rcv->sb_cc;
			if (so->so_special & SP_PIPE) {	/* Posix/AES */
				struct timeval now;
				microtime(&now);
				unp->unp_atime = now.tv_sec;
			}
			SOCKBUF_UNLOCK(snd);
			sowwakeup(so2);
			SOCKBUF_UNLOCK(rcv);
#undef snd
#undef rcv
			break;

		default:
			panic("uipc 2");
		}
		break;

	case PRU_SEND:
		if (control && (error = unp_internalize(control)))
			break;
		switch (so->so_type) {

		case SOCK_DGRAM: {
			if (nam) {
				if (unp->unp_conn) {
					error = EISCONN;
					break;
				}
				error = unp_connect(so, nam);
				LOCK_ASSERT("uipc_usrreq PRU_SEND DGRAM so", SOCKET_ISLOCKED(so));
				if (error)
					break;
			} else {
				if (unp->unp_conn == 0) {
					error = ENOTCONN;
					break;
				}
			}
#ifdef	TNC
			error = remote_append_dgram(so, m, control);
#else
			so2 = unp->unp_conn->unp_socket;
			error = local_append_dgram(so, so2, m, control);
#endif
			/* If we succeeded, don't release mbufs. */
			if (error == ESUCCESS) {
				m = 0;
				control = 0;
			}
			if (nam)
				unp_disconnect(unp);
			break;
		}

		case SOCK_STREAM:
#define	rcv (&so2->so_rcv)
#define	snd (&so->so_snd)
			if (so->so_state & SS_CANTSENDMORE) {
				error = EPIPE;
				break;
			}
			if (unp->unp_conn == 0)
				panic("uipc 3");
			so2 = unp->unp_conn->unp_socket;
			LOCK_ASSERT("uipc_usrreq PRU_SEND STREAM so2notso", (so2->so_lock == so->so_lock));
			SOCKBUF_LOCK(snd);
			SOCKBUF_LOCK(rcv);
			/*
			 * Send to paired receive port, and then reduce
			 * send buffer hiwater marks to maintain backpressure.
			 * Wake up readers.
			 */
			if (control) {
				(void)sbappendcontrol(rcv, m, control);
				control = 0;
			} else
				sbappend(rcv, m);
			snd->sb_mbmax -=
			    rcv->sb_mbcnt - unp->unp_conn->unp_mbcnt;
			unp->unp_conn->unp_mbcnt = rcv->sb_mbcnt;
			snd->sb_hiwat -= rcv->sb_cc - unp->unp_conn->unp_cc;
			unp->unp_conn->unp_cc = rcv->sb_cc;
			if (so->so_special & SP_PIPE) {	/* Posix/AES */
				struct timeval now;
				microtime(&now);
				unp->unp_ctime = unp->unp_mtime = now.tv_sec;
			}
			SOCKBUF_UNLOCK(rcv);
			sorwakeup(so2);
			SOCKBUF_UNLOCK(snd);
			m = 0;
#undef snd
#undef rcv
			break;

		default:
			panic("uipc 4");
		}
		break;

	case PRU_ABORT:
		unp_drop(unp, ECONNABORTED);
		break;

	case PRU_SENSE:
		if (so->so_type == SOCK_STREAM && unp->unp_conn != 0) {
			so2 = unp->unp_conn->unp_socket;
			/* Pipe stat's must behave per Posix/AES */
			if (so->so_special & SP_PIPE) {
				struct socket *rso, *wso;
				struct unpcb *runp, *wunp;
				/* Make socket pair appear as one entity */
				if (so->so_special & SP_WATOMIC) { /* write */
					wso = so; rso = so2;
					wunp = unp; runp = unp->unp_conn;
				} else {			   /* read */
					wso = so2; rso = so;
					wunp = unp->unp_conn; runp = unp;
				}
				((struct stat *) m)->st_atime = runp->unp_atime;
				((struct stat *) m)->st_mtime = wunp->unp_mtime;
				((struct stat *) m)->st_ctime = wunp->unp_ctime;
				((struct stat *) m)->st_blksize =
					wso->so_snd.sb_hiwat+rso->so_rcv.sb_cc;
				((struct stat *) m)->st_size =
					rso->so_rcv.sb_cc;
				if (unp->unp_vno == 0)
					unp->unp_vno = unp->unp_conn->unp_vno;
			/* Else traditional socket behavior */
			} else {
				((struct stat *) m)->st_blksize =
					so->so_snd.sb_hiwat+so2->so_rcv.sb_cc;
				((struct stat *) m)->st_size =
					so2->so_rcv.sb_cc;
			}
		} else
			((struct stat *) m)->st_blksize = so->so_snd.sb_hiwat;
		((struct stat *) m)->st_dev = NODEV;
		if (unp->unp_vno == 0) {
			UNPMISC_LOCK();
			unp->unp_vno = unp_vno++;
			UNPMISC_UNLOCK();
		}
		((struct stat *) m)->st_ino = unp->unp_vno;
		return (0);

	case PRU_RCVOOB:
		return (EOPNOTSUPP);

	case PRU_SENDOOB:
		error = EOPNOTSUPP;
		break;

	case PRU_SOCKADDR:
		if (unp->unp_addr) {
			nam->m_len = unp->unp_addr->m_len;
			bcopy(mtod(unp->unp_addr, caddr_t),
			    mtod(nam, caddr_t), (unsigned)nam->m_len);
		} else
			nam->m_len = 0;
		break;

	case PRU_PEERADDR:
		if (unp->unp_conn && unp->unp_conn->unp_addr) {
			nam->m_len = unp->unp_conn->unp_addr->m_len;
			bcopy(mtod(unp->unp_conn->unp_addr, caddr_t),
			    mtod(nam, caddr_t), (unsigned)nam->m_len);
		} else if (unp->unp_conn) {
			nam->m_len = sizeof(sun_noname);
			*(mtod(nam, struct sockaddr *)) = sun_noname;
		} else
			nam->m_len = 0;
		break;

	case PRU_SLOWTIMO:
		break;

	default:
		panic("piusrreq");
	}
release:
	if (control)
		m_freem(control);
	if (m)
		m_freem(m);
	return (error);
}


/*
 *  This was formerly part of the PRU_SEND/SOCK_DGRAM case in
 *  uipc_usrreq().  Moved it here so TNC code can call it directly.
 */
int
local_append_dgram(
	struct socket	*src,
	struct socket	*dest,
	struct mbuf	*m,
	struct mbuf	*control)
{
	struct unpcb		*unp = sotounpcb(src);
	CONST struct sockaddr	*from;
	int			error	= ESUCCESS;

#ifndef	TNC
	LOCK_ASSERT("uipc_usrreq PRU_SEND DGRAM so2notso",
		    (dest->so_lock == src->so_lock));
#else	/* TNC */
	/*
	 *  If this routine is called from un_dg_pkt_arrival(), the source
	 *  socket will be a stack temporary and have no socklocks.
	 */
	LOCK_ASSERT("local_append_dgram",
		    (dest->so_lock &&
		     (src->so_lock == NULL ||
		      (src->so_lock == dest->so_lock))));
	if (src->so_lock)
#endif
	SOCKBUF_LOCK(&src->so_snd);
	SOCKBUF_LOCK(&dest->so_rcv);
	if (unp->unp_addr)
		from = mtod(unp->unp_addr, CONST struct sockaddr *);
	else
		from = &sun_noname;
	if ( sbappendaddr((struct sockbuf *)&dest->so_rcv,
			  (struct sockaddr *)from, m, control) )
	{
		SOCKBUF_UNLOCK(&dest->so_rcv);
		sorwakeup(dest);
	} else {
		SOCKBUF_UNLOCK(&dest->so_rcv);
		error = ENOBUFS;
	}
#ifdef	TNC
	if (src->so_lock)
#endif
	SOCKBUF_UNLOCK(&src->so_snd);
	return(error);
}


/*
 * Both send and receive buffers are allocated PIPSIZ bytes of buffering
 * for stream sockets, although the total for sender and receiver is
 * actually only PIPSIZ.
 * Datagram sockets really use the sendspace as the maximum datagram size,
 * and don't really want to reserve the sendspace.  Their recvspace should
 * be large enough for at least one max-size datagram plus address.
 */
#if	(PIPE_BUF * 2) < 4096		/* sys/syslimits.h */
/* If PIPSIZ is set to < 4096 experience shows that many applications
 * deadlock. Note that PIPE_BUF is the write atomicity limit. */
#define	PIPSIZ	4096
#else
#define	PIPSIZ	(PIPE_BUF * 2)
#endif
u_long	unpst_sendspace = PIPSIZ;
u_long	unpst_recvspace = PIPSIZ;
u_long	unpdg_sendspace = 2*1024;	/* really max datagram size */
u_long	unpdg_recvspace = 4*1024;

int	unp_rights;			/* file descriptors in flight */

unp_attach(so)
	struct socket *so;
{
	register struct unpcb *unp;
	int error;
	
	if (so->so_snd.sb_hiwat == 0 || so->so_rcv.sb_hiwat == 0) {
		switch (so->so_type) {

		case SOCK_STREAM:
			error = soreserve(so, unpst_sendspace, unpst_recvspace);
			break;

		case SOCK_DGRAM:
			error = soreserve(so, unpdg_sendspace, unpdg_recvspace);
			break;
		}
		if (error)
			return (error);
	}
	NET_MALLOC(unp, struct unpcb *, sizeof *unp, M_PCB, M_NOWAIT);
	if (unp == NULL)
		return (ENOBUFS);
	bzero((caddr_t)unp, sizeof *unp);
	so->so_pcb = (caddr_t)unp;
	unp->unp_socket = so;
	return (0);
}

#ifdef	OSF1_ADFS
void unp_destroy_binding(struct socket *);	/* forward */
#endif

void
unp_detach(unp)
	register struct unpcb *unp;
{
	
#ifdef OSF1_ADFS
	unp_destroy_binding(unp->unp_socket);
#else
	if (unp->unp_vnode) {
		VN_LOCK(unp->unp_vnode);
		unp->unp_vnode->v_socket = 0;
		VN_UNLOCK(unp->unp_vnode);
		vrele(unp->unp_vnode);
		unp->unp_vnode = 0;
	}
#endif
	if (unp->unp_conn)
		unp_disconnect(unp);
	while (unp->unp_refs)
		unp_drop(unp->unp_refs, ECONNRESET);
	soisdisconnected(unp->unp_socket);
	unp->unp_socket->so_pcb = 0;
	unp->unp_socket->so_spare = 1;		/*XXX*/
	m_freem(unp->unp_addr);
	NET_FREE(unp, M_PCB);
	UNPMISC_LOCK();
	if (unp_rights)
		unp_gc();
	UNPMISC_UNLOCK();
}


#ifdef OSF1_ADFS
/*
 * We are running in a fileserver context where u.u_nd is valid, *but*
 * we are on the node where the socket fd exists.  I.e., the pathname
 * we need to lookup is probably remote.  This can be detected by a
 * NULL vnode port (ni_cdir) for the starting point.
 *
 * The actual vnode operations have been extracted into unp_vsockcreate, which
 * returns a vnode port.  It will probably go remote to create the socket file.
 * It would have been nice to use just mknod to create the socket.  Much
 * ugliness could also have been avoided if we could have stashed away
 * pointers to an m_copy of nam and unp, so that this could return EREMOTE
 * to S_fsvr_uds_bind, who would forward.
 */
unp_bind(unp, nam)
	struct unpcb *unp;
	struct mbuf *nam;
{
	struct sockaddr_un *soun = mtod(nam, struct sockaddr_un *);
	register struct nameidata *ndp = &u.u_nd;
	int error;
#ifdef	TNC
	mach_port_t sockid;
#else
	caddr_t sockid;
#endif

	ndp->ni_dirp = soun->sun_path;
	if (unp->unp_vnodeport != NULL)
		return (EINVAL);
	if (soun->sun_family != AF_UNIX)
		return (EAFNOSUPPORT);
	if (nam->m_len == MLEN) {
		if (*(mtod(nam, caddr_t) + nam->m_len - 1) != 0)
			return (EINVAL);
	} else
		*(mtod(nam, caddr_t) + nam->m_len) = 0;
/* SHOULD BE ABLE TO ADOPT EXISTING AND wakeup() ALA FIFO's */
#if	SEC_BASE
	audstub_unp_bind();
#endif
	ndp->ni_pathlen = strlen(ndp->ni_dirp) + 1;

#ifdef	TNC
	create_socket_bind_port(unp->unp_socket, &sockid);
	error = unp_vsockcreate(sockid, &unp->unp_vnodeport);
	if (error) {
		destroy_socket_bind_port(unp->unp_socket);
		return error;
	}
#else	/* ! TNC */
	sockid = (caddr_t) unp->unp_socket;
	error = unp_vsockcreate(sockid, &unp->unp_vnodeport);
	if (error)
		return error;
#endif
#ifdef TNC
	/* There appears to be a bug in the socket code -- what if a
	 * connected accept socket makes a bind call?  It was
	 * already given a copy of the listening parent's address in
	 * unp->unp_addr, but it is not really bound to that
	 * address; a connect() call will find the parent.  The
	 * address is put in unp->unp_addr so that the socket
	 * connected to this accept socket can ask at any time what
	 * address is associated with this socket.  The bug is that
	 * if a bind() call on an accept socket succeeds, the new
	 * address to which the socket is bound will be stored in
	 * unp->unp_addr, overwriting the previous bind address. 
	 * There are two ways to fix this problem.  One option is to
	 * deallocate the original address given to an accept socket
	 * if a bind() call succeeds.  The other option is to return
	 * an error at the start of the bind operation if unp_addr
	 * is not null.  While I suspect that a bind() on an accept
	 * socket really shouldn't succeed, I have chosen the first
	 * fix, as it has a less noticeable impact on current socket
	 * operations. -- jdh
	 */
	m_freem(unp->unp_addr);
#endif	/* TNC */
	unp->unp_addr = m_copym(nam, 0, (int)M_COPYALL, M_DONTWAIT);
	return (0);
}

unp_vsockcreate(sockid, portp)
#ifdef TNC
	mach_port_t sockid;	/* IN - socket bind port to insert in vnode */
#else	/* TNC */
	caddr_t sockid;
#endif	/* TNC */
	mach_port_t *portp;	/* OUT - port for socket's VSOCK vnode */
{
	register struct vnode *vp;
	register struct nameidata *ndp = &u.u_nd;
	struct vattr vattr;
	int error;

	ndp->ni_nameiop = CREATE | FOLLOW | WANTPARENT | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;

	/*
	 * If the pathname starts on this node, then call namei to perform
	 * (start) the pathname translation
	 */
	if (ndp->ni_cdir) {
		error = namei(ndp);
		if (error && error != EREMOTE)
			return (error);
	} else {
		/*
		 * The pathname begins on a remote node; set up to forward
		 */
		error = EREMOTE;
		ndp->ni_forwport = ndp->ni_cdirport;
	}
	if (error == EREMOTE) {
		/*
		 * This will complete the operation; this is only
		 * separate because we don't include uxkern/sthread.h
		 * (which is good, since it doesn't overtly pollute this file)
		 */
		return remote_unp_vsockcreate(sockid, portp);
	}

	/*
	 * To get here, we are running on the node where the VSOCK file
	 * is to be created.
	 */
	vp = ndp->ni_vp;
	if (vp != NULL) {
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
		vrele(vp);
		return (EADDRINUSE);
	}
	vattr_null(&vattr);
	vattr.va_type = VSOCK;
	vattr.va_mode = 0777;

	/*
	 * The creation of the socket must be "atomic," so the create
	 * operation needs additional information for the VSOCK case.
	 */
	vattr.va_socket = (char *) sockid;
	VOP_CREATE(ndp, &vattr, error);
	if (error)
		return (error);
	vp = ndp->ni_vp;
	
	/* filesystem sanity check */
#ifdef TNC
	/* It's a Mach port, not an address... */
	if ((mach_port_t)vp->v_socket != sockid) {
#else
	if ((caddr_t)vp->v_socket != sockid) {
#endif
		VN_LOCK(vp);
		vp->v_socket = 0;
		VN_UNLOCK(vp);
		vrele(vp);
		return (EADDRNOTAVAIL);
	}

	/*
	 * Get a port for this vnode, and return it.
	 */
	get_vnode_port(vp, portp);

	/*
	 * Release the reference aquired by VOP_CREATE. The only
	 * reference(s) on the vnode in the one obtained with get_vnode_port.
	 */
	vrele(vp);

	return 0;
}


/*
 *  Remove the vnode that represents the unix domain pathname address
 *  currently bound to this socket if it exists. Sockets created via
 *  socketpair() don't have afflicated vnodes. Break down the bind
 *  port.
 *  This virtual op is called from unp_detach().  It is assumed 
 *  that the socket is already locked.
 */
void
unp_destroy_binding(
	struct socket	*so)		/* socket to unbind from vnode */
{
	int		error;
	kern_return_t	kr;
	mach_port_t	vnport;
	struct unpcb	*unp	= sotounpcb(so);

	LOCK_ASSERT("unp_destroy_binding", SOCKET_ISLOCKED(so));

#ifndef	TNC
	/*
	 *  XXX If OSF1_ADFS is defined but TNC is not, I don't think
	 *  Unix datagram sockets work at all unless both sending and
	 *  receiving sockets are on the file system node where the
	 *  VSOCK vnode resides (since with OSF1_ADFS but not TNC,
	 *  "sockid" is just the address of a socket, and not a Mach
	 *  port).  Anyway, here we do just what the original code in
	 *  unp_detach() did prior to any TNC code.
	 */
	if (unp->unp_vnodeport != MACH_PORT_NULL) {
		(void) mach_port_deallocate(mach_task_self(),
					    unp->unp_vnodeport);
		unp->unp_vnodeport = MACH_PORT_NULL;
	}

#else	/* TNC */

	vnport = unp->unp_vnodeport;
	if(vnport) {

		ASSERT(so->vs_bindport != MACH_PORT_NULL);

		UNDEBUG(U_BINDPORT,
			("enter unp_destroy_binding(so=0x%x), vnport=0x%x\n",
			 so, vnport));

		/*
		 *  Ask the vnode to deallocate its send right to this
		 *  socket's bind port.
		 *
		 *  XXX Don't really know what to do if we get an error here.
		 */
		error = remote_unp_unbind(vnport);
		if (error != ESUCCESS)
			printf(
			"unp_destroy_binding: remote_unp_unbind error 0x%x\n",
			       error);

		/*
		 *  Now deallocate this socket's send right to the vnode port.
		 *  This port *should* represent the last referrence to the
		 *  vnode, and its deallocation should cause the vnode itself 
		 *  to be released.  Note that a socket *always* has a 
		 *  reference to a vnode port and never a direct reference 
		 *  to the vnode, even if the vnode is on the same node. 
		 *  (See unp_bind() comment.)
		 */
		kr = mach_port_deallocate(mach_task_self(), vnport);
		if (kr != KERN_SUCCESS)
			panic("unp_destroy_binding: m_p_deallocate: kr 0x%x\n",
			      kr);
		unp->unp_vnodeport = MACH_PORT_NULL;
	}	
		
	/*
	 *  Finally, get rid of this socket's bind port.  Any
	 *  connected sockets should get dead name errors next
	 *  time they try to send.
	 */
	if(so->vs_bindport != MACH_PORT_NULL) {
		ux_server_remove_port(so->vs_bindport);
		UNMAP_PORT_TO_SOCKET(so->vs_bindport, so);
		kr = mach_port_mod_refs(mach_task_self(), so->vs_bindport, 
					MACH_PORT_RIGHT_RECEIVE, -1);
		if (kr != KERN_SUCCESS)
			panic("unp_destroy_binding: m_p_modrefs: kr 0x%x\n",
			      kr);
		so->vs_bindport = MACH_PORT_NULL;
	}

#endif	/* TNC */
}

#else /* !OSF1_ADFS */

unp_bind(unp, nam)
	struct unpcb *unp;
	struct mbuf *nam;
{
	struct sockaddr_un *soun = mtod(nam, struct sockaddr_un *);
	register struct vnode *vp;
	register struct nameidata *ndp = &u.u_nd;
	struct vattr vattr;
	int error;

	ndp->ni_dirp = soun->sun_path;
	if (unp->unp_vnode != NULL)
		return (EINVAL);
	if (soun->sun_family != AF_UNIX)
		return (EAFNOSUPPORT);
	if (nam->m_len == MLEN) {
		if (*(mtod(nam, caddr_t) + nam->m_len - 1) != 0)
			return (EINVAL);
	} else
		*(mtod(nam, caddr_t) + nam->m_len) = 0;
/* SHOULD BE ABLE TO ADOPT EXISTING AND wakeup() ALA FIFO's */
#if	MACH
	ndp->ni_nameiop = CREATE | FOLLOW | WANTPARENT;
#else
	ndp->ni_nameiop = CREATE | FOLLOW | LOCKPARENT;
#endif
	ndp->ni_segflg = UIO_SYSSPACE;
#if	SEC_BASE
	audstub_unp_bind();
#endif

	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	if (vp != NULL) {
#if	MACH
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
#else
		VOP_ABORTOP(ndp);
		if (ndp->ni_dvp == vp)
			vrele(ndp->ni_dvp);
		else
			vput(ndp->ni_dvp);
#endif
		vrele(vp);
		return (EADDRINUSE);
	}
#if	MACH
	vattr_null(&vattr);
	vattr.va_type = VSOCK;
	vattr.va_mode = 0777;
	/*
	 * The creation of the socket must be "atomic," so the create
	 * operation needs additional information for the VSOCK case.
	 */
	vattr.va_socket = (char *) unp->unp_socket;
	VOP_CREATE(ndp, &vattr, error);
	if (error)
		return (error);
	vp = ndp->ni_vp;
	if (vp->v_socket != unp->unp_socket)	/* filesystem sanity check */
		panic("unp_bind");
	unp->unp_vnode = vp;
	unp->unp_addr = m_copym(nam, 0, (int)M_COPYALL, M_DONTWAIT);
#else
	VATTR_NULL(&vattr);
	vattr.va_type = VSOCK;
	vattr.va_mode = 0777;
	if (error = VOP_CREATE(ndp, &vattr))
		return (error);
	vp = ndp->ni_vp;
	vp->v_socket = unp->unp_socket;
	unp->unp_vnode = vp;
	unp->unp_addr = m_copym(nam, 0, (int)M_COPYALL, M_DONTWAIT);
	VOP_UNLOCK(vp);
#endif
	return (0);
}
#endif /* ! OSF1_ADFS */


unp_connect(so, nam)
	struct socket *so;
	struct mbuf *nam;
{
	register struct sockaddr_un *soun = mtod(nam, struct sockaddr_un *);
	register struct vnode *vp = NULL;
	register struct nameidata *ndp = &u.u_nd;
#ifdef OSF1_ADFS
	mach_port_t vnport = MACH_PORT_NULL;
#ifdef TNC
	mach_port_t sockid = MACH_PORT_NULL;
#else	/* TNC */
	caddr_t sockid;
#endif	/* TNC */
#endif
	int error;

	LOCK_ASSERT("unp_connect", SOCKET_ISLOCKED(so));
	if (soun->sun_family != AF_UNIX)
		return (EAFNOSUPPORT);
	ndp->ni_dirp = soun->sun_path;
	if (nam->m_data + nam->m_len == &nam->m_dat[MLEN]) {	/* XXX */
		if (*(mtod(nam, caddr_t) + nam->m_len - 1) != 0)
			return (EMSGSIZE);
	} else
		*(mtod(nam, caddr_t) + nam->m_len) = 0;
	SOCKET_UNLOCK(so);			/* namei may be interrupted */
#if	MACH
#ifdef OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
#endif
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
#endif
	ndp->ni_segflg = UIO_SYSSPACE;

#ifndef OSF1_ADFS
	if (error = namei(ndp)) {
		SOCKET_LOCK(so);
		return (error);
	}
#else
	/*
	 * If the pathname starts on this node, then call namei to perform
	 * (start) the pathname translation
	 */
	if (ndp->ni_cdir) {
		error = namei(ndp);
		if (error && error != EREMOTE) {
			SOCKET_LOCK(so);
			return (error);
		}
	} else {
		/*
		 * The pathname begins on a remote node; set up to forward
		 */
		error = EREMOTE;
		ndp->ni_forwport = ndp->ni_cdirport;
	}
	if (error == EREMOTE) {
		/*
		 * This will complete the lookup; this is only
		 * separate because we don't include uxkern/sthread.h
		 * (which is good, since it doesn't overtly pollute this file)
		 */
		error = remote_unp_vsockread(&sockid, &vnport);
		if( error )
			goto bad;
	} else {
		/*
		 * The vnode is local; read it directly.
		 */
#endif	/* OSF1_ADFS */
		vp = ndp->ni_vp;
		VN_LOCK(vp);
		if (vp->v_type != VSOCK) {
			VN_UNLOCK(vp);
			error = ENOTSOCK;
			goto bad;
		} else
			VN_UNLOCK(vp);
#if	MACH
		VOP_ACCESS(vp, VWRITE, ndp->ni_cred, error);
#else
		error = VOP_ACCESS(vp, VWRITE, ndp->ni_cred);
#endif
		if (error)
			goto bad;
#ifndef	TNC
		sockid = (caddr_t) vp->v_socket;
#else
		error = get_bind_port_from_vnode(vp, &sockid);
		if (error)
			goto bad;
#endif
#ifdef OSF1_ADFS
	}
#endif
	/*
	 * this routine returns with so locked
	 */
	error =  VSOP_VSOCKCONNECT(so, sockid);
	goto exit;

bad:
	SOCKET_LOCK(so);
exit:
#ifndef OSF1_ADFS
#if	MACH
	vrele(vp);
#else
	vput(vp);
#endif
#else
	if (vp)
		vrele(vp);
	if (vnport)
		mach_port_deallocate(mach_task_self(), vnport);
#endif
	return (error);
}


/*
 * In pre-TNC systems, the code in this routine appeared in in the body
 * of unp_connect(), where a call to VSOP_VSOCKCONNECT() now appears.
 * Now the VSOP_VSOCKCONNECT() virtual socket operation moves execution
 * to the multicomputer node where the connection should occur; this
 * routine is then called to make the local connection.
 *
 * It is assumed that both so and so2 are unlocked as we enter this
 * routine.  It is assumed that when we leave this routine, so is
 * locked regardless of whether an error has occured.  The accept 
 * socket (so3) is only locked if a connect succeeded.
 */
unp_local_connect(so, so2)
struct socket *so;
struct socket *so2;
{
	struct unpcb *unp2;
	struct mbuf *nam;
	struct socket *so3;
	int error;

#if	NETSYNC_LOCK
int so2notso;
#endif
	if (so2 == 0 || so2->so_spare == 1) {		/*XXX*/
		error = ECONNREFUSED;
		goto bad;
	}
	if (so->so_type != so2->so_type) {
		error = EPROTOTYPE;
		goto bad;
	}
	SOCKET_LOCK2(so, so2);
#if	NETSYNC_LOCK
	so2notso = (so2->so_lock != so->so_lock);
#endif
	if (so->so_proto->pr_flags & PR_CONNREQUIRED) {
		if ((so2->so_options & SO_ACCEPTCONN) == 0) {
			error = ECONNREFUSED;
#if	NETSYNC_LOCK
			if (so2notso)
				SOCKET_UNLOCK(so2);
#endif
			return(error);
		}
#if	SEC_ARCH
		bcopy(so->so_tag, so2->so_tag, SEC_NUM_TAGS * sizeof(tag_t));
#endif
		unp2 = sotounpcb(so2);	/* Get server name before unlock */
		if (unp2->unp_addr)
			nam = m_copym(unp2->unp_addr, 0, (int)M_COPYALL, M_DONTWAIT);
		else
			nam = 0;
		if ((so3 = sonewconn(so2, 0)) == 0) {
			error = ECONNREFUSED;
			if (nam) m_freem(nam);
#if	NETSYNC_LOCK
			/*
			 * sonewconn() unlocks so2. But if
			 * so and so2 shared the same lock...
			 */
			if (!so2notso)
				goto bad;
;
#endif
			return(error);
		}
#if	NETSYNC_LOCK
		/*
		 * Note that so3 is brand new and has no relation
		 * to so. But sonewconn unlocked the old so2, which
		 * may have shared a lock with so (or have been so
		 * itself!). We thus may need to relock so.
		 */
		if (!so2notso)
			SOCKET_LOCK(so);
#endif
		sotounpcb(so3)->unp_addr = nam;
		so2 = so3;
	}
	error = unp_connect2(so, so2);
	return(error);

bad:
	SOCKET_LOCK(so);
	return(error);
}

unp_connect2(so, so2)
	register struct socket *so;
	register struct socket *so2;
{
	register struct unpcb *unp = sotounpcb(so);
	register struct unpcb *unp2;

	LOCK_ASSERT("unp_connect2 so", SOCKET_ISLOCKED(so));
	LOCK_ASSERT("unp_connect2 so2", SOCKET_ISLOCKED(so2));
	if (so2->so_type != so->so_type)
		return (EPROTOTYPE);
#if	NETSYNC_LOCK
	solockpair(so, so2);
#endif
	ASSERT(so2->so_lock != NULL);
	ASSERT(so->so_lock != NULL);
#ifdef	TNC
	/*
	 *  For datagram sockets, the unp_*ref stuff is not used,
	 *  and unp_conn is a Mach port, not to be messed with!
	 *  We only need to call soisconnected().
	 */
	if (so->so_type == SOCK_DGRAM) {
		soisconnected(so);
		return (ESUCCESS);
	}
#endif
	unp2 = sotounpcb(so2);
	unp->unp_conn = unp2;
	switch (so->so_type) {

	case SOCK_DGRAM:
		UNPCONN_LOCK();
		unp->unp_nextref = unp2->unp_refs;
		unp2->unp_refs = unp;
		UNPCONN_UNLOCK();
		soisconnected(so);
		break;

	case SOCK_STREAM:
		unp2->unp_conn = unp;
		soisconnected(so);
		soisconnected(so2);
		if (so->so_special & SP_PIPE) {	/* Posix/AES */
			struct timeval now;
			microtime(&now);
			unp->unp_ctime = unp->unp_atime =
				unp->unp_mtime = now.tv_sec;
			unp2->unp_ctime = unp2->unp_atime =
				unp2->unp_mtime = now.tv_sec;
		}
		break;

	default:
		panic("unp_connect2");
	}
	return (0);
}

void
unp_disconnect(unp)
	struct unpcb *unp;
{
	register struct unpcb *unp2 = unp->unp_conn;

	if (unp2 == 0)
		return;
	LOCK_ASSERT("unp_disconnect", SOCKET_ISLOCKED(unp->unp_socket));
#if	defined(NETSYNC_LOCK) && !defined(TNC)
	/*
	 * unp->unp_socket is locked on entry... be sure unp2's is also.
	 */
	if (unp->unp_socket->so_lock != unp2->unp_socket->so_lock)
		SOCKET_LOCK(unp2->unp_socket);
#endif
	switch (unp->unp_socket->so_type) {

	case SOCK_DGRAM:
#ifdef	TNC
		un_dg_disconnect(unp);
#else
		UNPCONN_LOCK();
		if (unp2->unp_refs == unp)
			unp2->unp_refs = unp->unp_nextref;
		else {
			unp2 = unp2->unp_refs;
			for (;;) {
				if (unp2 == 0)
					panic("unp_disconnect");
				if (unp2->unp_nextref == unp)
					break;
				unp2 = unp2->unp_nextref;
			}
			unp2->unp_nextref = unp->unp_nextref;
		}
		unp->unp_nextref = 0;
		UNPCONN_UNLOCK();
#endif	/* ! TNC */
		unp->unp_socket->so_state &= ~SS_ISCONNECTED;
		break;

	case SOCK_STREAM:
		soisdisconnected(unp->unp_socket);
		unp2->unp_conn = 0;
		soisdisconnected(unp2->unp_socket);
		break;
	}
	unp->unp_conn = 0;
#if	defined(NETSYNC_LOCK) && !defined(TNC)
	if (unp->unp_socket->so_lock != unp2->unp_socket->so_lock)
		SOCKET_UNLOCK(unp2->unp_socket);
#endif
}

#ifdef notdef
void
unp_abort(unp)
	struct unpcb *unp;
{

	unp_detach(unp);
}
#endif

/*ARGSUSED*/
void
unp_usrclosed(unp)
	struct unpcb *unp;
{

}

void
unp_drop(unp, errno)
	struct unpcb *unp;
	int errno;
{
	struct socket *so = unp->unp_socket;

	LOCK_ASSERT("unp_drop", SOCKET_ISLOCKED(so));
	so->so_error = errno;
	unp_disconnect(unp);
	if (so->so_head) {
		so->so_pcb = (caddr_t) 0;
		m_freem(unp->unp_addr);
		NET_FREE(unp, M_PCB);
		sofree(so);
	}
}

#ifdef notdef
void
unp_drain()
{

}
#endif

unp_externalize(rights)
	struct mbuf *rights;
{
	panic("unp_externalize called \n");
#if	0
	register int i;
	register struct cmsghdr *cm = mtod(rights, struct cmsghdr *);
	register struct file **rp = (struct file **)(cm + 1);
	register struct file *fp;
	int newfds = (cm->cmsg_len - sizeof(*cm)) / sizeof (int);
	int f;

#if	SEC_ARCH
	rp = (struct file **) findrights((caddr_t) rp,
			cm->cmsg_len - sizeof(*cm), SEC_RIGHTS_FDS, &newfds);
	if (rp)
		newfds /= sizeof(int);
	else
		newfds = 0;
#endif	/* SEC_ARCH */
#if	MACH
	if (newfds > ufavail(&u.u_file_state)) {
#else
	if (newfds > ufavail()) {
#endif
		for (i = 0; i < newfds; i++) {
			fp = *rp;
			unp_discard(fp);
			*rp++ = 0;
		}
		return (EMSGSIZE);
	}
	for (i = 0; i < newfds; i++) {
#if	MACH
		if (ufalloc(0, &f, &u.u_file_state))
#else
		if (ufalloc(0, &f))
#endif
			panic("unp_externalize");
		fp = *rp;
		U_FD_SET(f, fp, &u.u_file_state);
		FP_LOCK(fp);
		fp->f_msgcount--;
		FP_UNLOCK(fp);
		UNPMISC_LOCK();
		unp_rights--;
		UNPMISC_UNLOCK();
		*(int *)rp++ = f;
	}
	return (0);
#endif	/* NEVER */
}

unp_internalize(control)
	struct mbuf *control;
{
	panic("unp_internalize called \n");
#if	0
	register struct cmsghdr *cm = mtod(control, struct cmsghdr *);
	register struct file **rp;
	register struct file *fp;
	register int i, fd, error = 0;
	int oldfds;

	if (cm->cmsg_type != SCM_RIGHTS || cm->cmsg_level != SOL_SOCKET ||
	    cm->cmsg_len != control->m_len)
		return (EINVAL);
#if	SEC_ARCH
	rp = (struct file **) findrights((caddr_t)(cm + 1),
			cm->cmsg_len - sizeof(*cm), SEC_RIGHTS_FDS, &oldfds);
	if (rp)
		oldfds /= sizeof(int);
	else
		oldfds = 0;
	U_FDTABLE_LOCK(&u.u_file_state);
	for (i = 0; i < oldfds; i++) {
		fd = *(int *)(rp + i);
		if ((unsigned)(fd) >= NOFILE ||
		    ((fp) = u.u_file_state.uf_ofile[(fd)]) == NULL ||
		    fp == U_FD_RESERVED) {
			error = EBADF;
			goto out;
		}
	}
#else 	/* !SEC_ARCH */
	oldfds = (cm->cmsg_len - sizeof (*cm)) / sizeof (int);
	rp = (struct file **)(cm + 1);
	U_FDTABLE_LOCK(&u.u_file_state);
	for (i = 0; i < oldfds; i++) {
		fd = *(int *)rp++;
		if ((unsigned)(fd) >= NOFILE ||
#if	MACH
		    ((fp) = u.u_file_state.uf_ofile[(fd)]) == NULL ||
		    fp == U_FD_RESERVED) {
#else
		    u.u_ofile[fd] == NULL) {
#endif
			error = EBADF;
			goto out;
		}
	}
	rp = (struct file **)(cm + 1);
#endif	/* !SEC_ARCH */
	for (i = 0; i < oldfds; i++) {
#if	MACH
		fp = u.u_file_state.uf_ofile[*(int *)rp];
#else
		fp = u.u_ofile[*(int *)rp];
#endif
		*rp++ = fp;
		FP_LOCK(fp);
		fp->f_count++;
		fp->f_msgcount++;
		FP_UNLOCK(fp);
		UNPMISC_LOCK();
		unp_rights++;
		UNPMISC_UNLOCK();
	}
out:
	U_FDTABLE_UNLOCK(&u.u_file_state);
	return (error);
#endif	/* NEVER */
}

#if	NETSYNC_LOCK
/*
 * Unix connection rights garbage collection. Enough races in here
 * to choke Secretariat! RIP. However, the globals here are protected
 * by UNPMISC_LOCK(), I think.
 *
 * UNTESTED BECAUSE NOBODY PASSES RIGHTS AROUND AND/OR IF THEY DO
 * THE RIGHTS DON'T GET LEFT TO OTHERS TO GET GC'D. Yecch.
 */
#endif
int	unp_defer, unp_gcing;
extern	struct domain unixdomain;

void
unp_gc()
{
	register struct file *fp;
	register struct socket *so;

	if (unp_gcing)
		return;
	unp_gcing = 1;
printf("unp_gc: Beware!\n");	/* Will certainly break in MP version... */
	UNPMISC_UNLOCK();		/* until done */
restart:
	unp_defer = 0;
	for (fp = flisthead; fp != (struct file *)NULL; fp = fp->f_filelist)
		fp->f_flag &= ~(FMARK|FDEFER);
	do {
		for (fp = flisthead; fp != (struct file *)NULL; 
						fp = fp->f_filelist) {
			if (fp->f_count == 0)
				continue;
			if (fp->f_flag & FDEFER) {
				fp->f_flag &= ~FDEFER;
				unp_defer--;
			} else {
				if (fp->f_flag & FMARK)
					continue;
				if (fp->f_count == fp->f_msgcount)
					continue;
				fp->f_flag |= FMARK;
			}
			if (fp->f_type != DTYPE_SOCKET ||
			    (so = (struct socket *)fp->f_data) == 0)
				continue;
			if (so->so_proto->pr_domain != &unixdomain ||
			    (so->so_proto->pr_flags&PR_RIGHTS) == 0)
				continue;
			SOCKET_LOCK(so);		/* ??? */
			SOCKBUF_LOCK(&so->so_rcv);	/* ??? */
			if (so->so_rcv.sb_flags & SB_LOCK) {
				sosbwait(&so->so_rcv, so);
				/* Locks are released by sosbwait */
				goto restart;
			}
			unp_scan(so->so_rcv.sb_mb, unp_mark);
			SOCKBUF_UNLOCK(&so->so_rcv);
			SOCKET_UNLOCK(so);
		}
	} while (unp_defer);
	for (fp = flisthead; fp != (struct file *)NULL; fp = fp->f_filelist) {
		if (fp->f_count == 0)
			continue;
		if (fp->f_count == fp->f_msgcount && (fp->f_flag & FMARK) == 0)
			while (fp->f_msgcount)
				unp_discard(fp);
	}
	UNPMISC_LOCK();		/* restore lock */
	unp_gcing = 0;
}

void
unp_dispose(m)
	struct mbuf *m;
{
	if (m)
		unp_scan(m, unp_discard);
}

void
unp_scan(m0, op)
	register struct mbuf *m0;
	void (*op)();
{
	register struct mbuf *m;
	register struct file **rp;
	register struct cmsghdr *cm;
	register int i;
	int qfds;

	while (m0) {
		for (m = m0; m; m = m->m_next)
			if (m->m_type == MT_CONTROL &&
			    m->m_len >= sizeof(*cm)) {
				cm = mtod(m, struct cmsghdr *);
				if (cm->cmsg_level != SOL_SOCKET ||
				    cm->cmsg_type != SCM_RIGHTS)
					continue;
#if	SEC_ARCH
				rp = (struct file **)
					findrights((caddr_t)(cm + 1),
						cm->cmsg_len - sizeof(*cm),
							SEC_RIGHTS_FDS, &qfds);
				if (rp)
					qfds /= sizeof(struct file *);
				else
					qfds = 0;
#else	/* !SEC_ARCH */
				qfds = (cm->cmsg_len - sizeof *cm)
						/ sizeof (struct file *);
				rp = (struct file **)(cm + 1);
#endif	/* !SEC_ARCH */
				for (i = 0; i < qfds; i++)
					(*op)(*rp++);
				break;		/* XXX, but saves time */
			}
		m0 = m0->m_act;
	}
}

void
unp_mark(fp)
	struct file *fp;
{

	FP_LOCK(fp);
	if (fp->f_flag & FMARK) {
		FP_UNLOCK(fp);
		return;
	}
	unp_defer++;
	fp->f_flag |= (FMARK|FDEFER);
	FP_UNLOCK(fp);
}

void
unp_discard(fp)
	struct file *fp;
{

	FP_LOCK(fp);
	fp->f_msgcount--;
	FP_UNLOCK(fp);
	UNPMISC_LOCK();
	unp_rights--;
	UNPMISC_UNLOCK();
	FP_UNREF(fp);
}
