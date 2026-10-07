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
 * HISTORY
 * $Log: raw_usrreq.c,v $
 * Revision 1.5  1994/11/18  20:34:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:09:02  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:29:03  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:00:16  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:27:16  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:54  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:37  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:01  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:41:29  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:34:11  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12  90/10/07  14:32:59  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:11:56  gm]
 * 
 * Revision 1.11  90/08/24  12:14:06  devrcs
 * 	Free control mbufs in raw_usrreq().
 * 	[90/08/09  13:37:52  tmt]
 * 
 * Revision 1.10  90/07/27  08:59:50  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:56:01  tmt]
 * 
 * Revision 1.9  90/06/29  13:36:59  devrcs
 * 	Make security contingent on MACH for compat.
 * 	[90/06/26  20:00:53  tmt]
 * 
 * Revision 1.8  90/06/22  20:38:44  devrcs
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:44:58  seiden]
 * 
 * Revision 1.7  90/04/27  19:13:58  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:33:13  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:43  devrcs
 * 	Add void's.
 * 	[90/04/09  16:19:26  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:08  robert
 * 	Remove reference to rawintrq (unused). Use lock macro on socket.
 * 	[90/01/19  14:44:29  tmt]
 * 
 * Revision 1.4  90/01/18  08:44:22  gm
 * 	Expand m_copy macro to m_copym.
 * 	[90/01/08  15:56:32  tmt]
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
 * Revision 1.3  90/01/03  12:41:02  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:14  gm]
 * 
 * Revision 1.2  89/12/26  09:47:35  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)raw_usrreq.c	1.2 14:09:34 5/15/90 SecureWare */
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
 * Copyright (c) 1980, 1986 Regents of the University of California.
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
 *	Base:	raw_usrreq.c	7.7 (Berkeley) 9/20/89
 *	Merged:	raw_usrreq.c	7.9 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"
#if	MACH
#include "sys/secdefines.h"
#endif

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/socketvar.h"
#include "sys/domain.h"
#include "sys/protosw.h"

#include "net/if.h"
#include "net/route.h"
#include "net/raw_cb.h"

LOCK_ASSERTL_DECL

/*
 * Initialize raw connection block q.
 */
void
raw_init()
{

	RAW_LOCKINIT();
	ROUTE_LOCKINIT();
	NETSTAT_LOCKINIT(&rtstat.rts_lock);
	rawcb.rcb_next = rawcb.rcb_prev = &rawcb;
}


/*
 * Raw protocol input routine.  Find the socket
 * associated with the packet(s) and move them over.  If
 * nothing exists for this packet, drop it.
 */
/*
 * Raw protocol interface.
 */
raw_input(m0, proto, src, dst)
	struct mbuf *m0;
	register struct sockproto *proto;
	struct sockaddr *src, *dst;
{
	register struct rawcb *rp;
	register struct mbuf *m = m0;
	register int sockets = 0;
	struct socket *last;

	last = 0;
	RAW_LOCK();
	for (rp = rawcb.rcb_next; rp != &rawcb; rp = rp->rcb_next) {
		if (rp->rcb_proto.sp_family != proto->sp_family)
			continue;
		if (rp->rcb_proto.sp_protocol  &&
		    rp->rcb_proto.sp_protocol != proto->sp_protocol)
			continue;
		/*
		 * We assume the lower level routines have
		 * placed the address in a canonical format
		 * suitable for a structure comparison.
		 *
		 * Note that if the lengths are not the same
		 * the comparison will fail at the first byte.
		 */
#define	equal(a1, a2) \
  (bcmp((caddr_t)(a1), (caddr_t)(a2), a1->sa_len) == 0)
		if (rp->rcb_laddr && !equal(rp->rcb_laddr, dst))
			continue;
		if (rp->rcb_faddr && !equal(rp->rcb_faddr, src))
			continue;
		if (last) {
			struct mbuf *n;
			if (n = m_copym(m, 0, (int)M_COPYALL, M_DONTWAIT)) {
				SOCKET_LOCK(last);
				SOCKBUF_LOCK(&last->so_rcv);
				if (sbappendaddr(&last->so_rcv, src,
				    n, (struct mbuf *)0) == 0) {
					SOCKBUF_UNLOCK(&last->so_rcv);
					/* should notify about lost packet */
					m_freem(n);
				} else {
					SOCKBUF_UNLOCK(&last->so_rcv);
					sorwakeup(last);
					sockets++;
				}
				SOCKET_UNLOCK(last);
			}
		}
		last = rp->rcb_socket;
	}
	if (last) {
		SOCKET_LOCK(last);
		SOCKBUF_LOCK(&last->so_rcv);
		if (sbappendaddr(&last->so_rcv, src,
		    m, (struct mbuf *)0) == 0) {
			SOCKBUF_UNLOCK(&last->so_rcv);
			m_freem(m);
		} else {
			SOCKBUF_UNLOCK(&last->so_rcv);
			sorwakeup(last);
			sockets++;
		}
		SOCKET_UNLOCK(last);
	} else
		m_freem(m);
	RAW_UNLOCK();
	return (sockets);
}

/*ARGSUSED*/
void
raw_ctlinput(cmd, arg)
	int cmd;
	struct sockaddr *arg;
{

	if (cmd < 0 || cmd > PRC_NCMDS)
		return;
	/* INCOMPLETE */
}

/*ARGSUSED*/
raw_usrreq(so, req, m, nam, control)
	struct socket *so;
	int req;
	struct mbuf *m, *nam, *control;
{
	register struct rawcb *rp = sotorawcb(so);
	register int error = 0;
	int len;

	if (req == PRU_CONTROL)
		return (EOPNOTSUPP);
#if	!SEC_ARCH
	if (control && control->m_len) {
		error = EOPNOTSUPP;
		goto release;
	}
#endif
	if (rp == 0) {
		error = EINVAL;
		goto release;
	}

	LOCK_ASSERT("raw_usrreq", SOCKET_ISLOCKED(so));
	RAW_LOCK();

	switch (req) {

	/*
	 * Allocate a raw control block and fill in the
	 * necessary info to allow packets to be routed to
	 * the appropriate raw interface routine.
	 */
	case PRU_ATTACH:
		if ((so->so_state & SS_PRIV) == 0) {
			error = EACCES;
			break;
		}
		error = raw_attach(so, (int)nam);
		break;

	/*
	 * Destroy state just before socket deallocation.
	 * Flush data or not depending on the options.
	 */
	case PRU_DETACH:
		if (rp == 0) {
			error = ENOTCONN;
			break;
		}
		raw_detach(rp);
		break;

#ifdef notdef
	/*
	 * If a socket isn't bound to a single address,
	 * the raw input routine will hand it anything
	 * within that protocol family (assuming there's
	 * nothing else around it should go to). 
	 */
	case PRU_CONNECT:
		if (rp->rcb_faddr) {
			error = EISCONN;
			break;
		}
		nam = m_copym(nam, 0, M_COPYALL, M_WAIT);
		rp->rcb_faddr = mtod(nam, struct sockaddr *);
		soisconnected(so);
		break;

	case PRU_BIND:
		if (rp->rcb_laddr) {
			error = EINVAL;			/* XXX */
			break;
		}
		error = raw_bind(so, nam);
		break;
#endif

	case PRU_CONNECT2:
		error = EOPNOTSUPP;
		break;		/* NOT goto release; */

	case PRU_DISCONNECT:
		if (rp->rcb_faddr == 0) {
			error = ENOTCONN;
			break;
		}
		raw_disconnect(rp);
		soisdisconnected(so);
		break;

	/*
	 * Mark the connection as being incapable of further input.
	 */
	case PRU_SHUTDOWN:
		socantsendmore(so);
		break;

	/*
	 * Ship a packet out.  The appropriate raw output
	 * routine handles any massaging necessary.
	 */
	case PRU_SEND:
		if (nam) {
			if (rp->rcb_faddr) {
				error = EISCONN;
				break;
			}
			rp->rcb_faddr = mtod(nam, struct sockaddr *);
		} else if (rp->rcb_faddr == 0) {
			error = ENOTCONN;
			break;
		}
		RAW_UNLOCK();
		error = (*so->so_proto->pr_output)(m, so);
		m = NULL;
		if (nam)
			rp->rcb_faddr = 0;
		goto release;		/* skip unlock */

	case PRU_ABORT:
		raw_disconnect(rp);
		soisdisconnected(so);
		sofree(so);
		break;

	case PRU_SENSE:
		/*
		 * stat: don't bother with a blocksize.
		 */
		RAW_UNLOCK();
		return (0);

	/*
	 * Not supported.
	 */
	case PRU_RCVOOB:
	case PRU_RCVD:
		RAW_UNLOCK();
		return(EOPNOTSUPP);

	case PRU_LISTEN:
	case PRU_ACCEPT:
	case PRU_SENDOOB:
		error = EOPNOTSUPP;
		break;

	case PRU_SOCKADDR:
		if (rp->rcb_laddr == 0) {
			error = EINVAL;
			break;
		}
		len = rp->rcb_laddr->sa_len;
		bcopy((caddr_t)rp->rcb_laddr, mtod(nam, caddr_t), (unsigned)len);
		nam->m_len = len;
		break;

	case PRU_PEERADDR:
		if (rp->rcb_faddr == 0) {
			error = ENOTCONN;
			break;
		}
		len = rp->rcb_faddr->sa_len;
		bcopy((caddr_t)rp->rcb_faddr, mtod(nam, caddr_t), (unsigned)len);
		nam->m_len = len;
		break;

	default:
		panic("raw_usrreq");
	}
	RAW_UNLOCK();
release:
	if (control != NULL)
		m_freem(control);
	if (m != NULL)
		m_freem(m);
	return (error);
}
