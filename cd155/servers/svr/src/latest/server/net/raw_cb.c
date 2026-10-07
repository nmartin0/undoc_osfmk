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
 * $Log: raw_cb.c,v $
 * Revision 1.5  1994/11/18  20:34:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:08:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:28:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:00:11  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:27:11  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:51  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:32  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:59  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:41:19  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:34:02  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:32:50  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:11:43  gm]
 * 
 * Revision 1.7  90/07/27  08:59:43  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:51:01  tmt]
 * 
 * Revision 1.6  90/04/27  19:13:41  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:32:33  tmt]
 * 
 * Revision 1.5  90/04/14  00:32:38  devrcs
 * 	Move declarations here, strengthen types (void).
 * 	[90/04/09  16:18:35  tmt]
 * 
 * Revision 1.4  90/01/18  08:44:16  gm
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
 * Revision 1.3  90/01/03  12:40:57  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:07  gm]
 * 
 * Revision 1.2  89/12/26  09:47:09  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
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
 *	Base:	raw_cb.c	7.9 (Berkeley) 4/25/89
 *	Merged:	raw_cb.c	7.11 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

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

#include "net/net_malloc.h"

LOCK_ASSERTL_DECL

/*
 * Routines to manage the raw protocol control blocks. 
 *
 * TODO:
 *	hash lookups by protocol family/protocol + address family
 *	take care of unique address problems per AF?
 *	redo address binding to allow wildcards
 */

u_long	raw_sendspace = RAWSNDQ;
u_long	raw_recvspace = RAWRCVQ;

struct rawcb rawcb;		/* head of list */
#if	NETSYNC_LOCK
lock_data_t global_raw_lock;
#endif

/*
 * Allocate a control block and a nominal amount
 * of buffer space for the socket.
 */
raw_attach(so, proto)
	register struct socket *so;
	int proto;
{
	register struct rawcb *rp = sotorawcb(so);
	int error;

	/*
	 * It is assumed that raw_attach is called
	 * after space has been allocated for the
	 * rawcb.
	 */
	if (rp == 0)
		return (ENOBUFS);
	if (error = soreserve(so, raw_sendspace, raw_recvspace))
		return (error);
	rp->rcb_socket = so;
	rp->rcb_proto.sp_family = so->so_proto->pr_domain->dom_family;
	rp->rcb_proto.sp_protocol = proto;
	insque(rp, &rawcb);	/* raw_usrreq provides the lock */
	return error;
}

/*
 * Detach the raw connection block and discard
 * socket resources.
 */
void
raw_detach(rp)
	register struct rawcb *rp;
{
	struct socket *so = rp->rcb_socket;

	so->so_pcb = 0;
	sofree(so);
	remque(rp);	/* raw_usrreq provides the lock */
#ifdef notdef
	if (rp->rcb_laddr)
		m_freem(dtom(rp->rcb_laddr));
	rp->rcb_laddr = 0;
#endif
	NET_FREE(rp, M_PCB);
}

/*
 * Disconnect and possibly release resources.
 */
void
raw_disconnect(rp)
	struct rawcb *rp;
{

#ifdef notdef
	if (rp->rcb_faddr)
		m_freem(dtom(rp->rcb_faddr));
	rp->rcb_faddr = 0;
#endif
	if (rp->rcb_socket->so_state & SS_NOFDREF)
		raw_detach(rp);
}

#ifdef notdef
raw_bind(so, nam)
	register struct socket *so;
	struct mbuf *nam;
{
	struct sockaddr *addr = mtod(nam, struct sockaddr *);
	register struct rawcb *rp;

	if (ifnet == 0)
		return (EADDRNOTAVAIL);
	rp = sotorawcb(so);
	nam = m_copym(nam, 0, M_COPYALL, M_WAIT);
	rp->rcb_laddr = mtod(nam, struct sockaddr *);
	return (0);
}
#endif
