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
 * $Log: in_pcb.c,v $
 * Revision 1.6  1995/02/01  21:34:43  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:34:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/01  01:35:30  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 2.4  93/08/20  18:55:06  mjl
 * [LCCbug #0355] When connecting via MI using unbound socket (INADDR_ANY),
 * choose a non-MI interface to use as the packet source address.
 * 
 * Revision 2.3  93/08/19  12:57:21  mjl
 * [LCCbug #0351, #0352] In in_pcbconnect(), ignore ifaddrs of
 * MI-specific routes.
 * 
 * Revision 1.3  1993/05/06  20:26:08  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:35  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:40  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:02  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:42  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:43:01  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:35:21  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12.5.2  91/03/15  17:48:57  tmt
 * 	Add Ansi prototypes for in_pcb{notify,lookup,match}.
 * 	[91/03/13  19:09:15  tmt]
 * 
 * Revision 1.12  90/10/07  14:34:23  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:14:04  gm]
 * 
 * Revision 1.11  90/08/09  13:26:19  devrcs
 * 	Add "last pcb cache" to in_pcblookup and in_pcbdetach.
 * 	[90/07/25  07:49:21  tmt]
 * 
 * Revision 1.10  90/07/27  09:00:49  devrcs
 * 	Update to BSD Reno release.
 * 	Rearrange or remove route locks - route.c handles.
 * 	[90/07/19  17:29:16  tmt]
 * 
 * Revision 1.9  90/07/05  23:13:07  devrcs
 * 	Remove lostchain temp variable.
 * 	[90/07/03  18:51:48  tmt]
 * 
 * Revision 1.8  90/06/22  20:38:58  devrcs
 * 	Parallelization repairs. Socket locks, then inpcb locks.
 * 	Take inpcbhead locks directly. Do refcounting right.
 * 	[90/06/07  16:09:53  tmt]
 * 
 * Revision 1.7  90/04/27  19:15:30  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:50:26  tmt]
 * 
 * Revision 1.6  90/04/14  00:33:05  devrcs
 * 	Add void's.
 * 	[90/04/09  16:28:44  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:24  robert
 * 	Use macros for inpcb, socket _islocked.
 * 	[90/01/19  15:26:13  tmt]
 * 
 * Revision 1.4  90/01/18  08:45:05  gm
 * 	Placed in_losing_lock under NETSYNC_LOCK conditional.
 * 	[90/01/16  18:58:52  tmt]
 * 
 * 	New convention: the global lock chain is the inp_lock of the inp_head.
 * 	This means we don't have to pass in the global lock pointer to notify,
 * 	detach, etc and makes the code more general. Also makes in_losing_lock
 * 	possible, which is used by NFS for now.
 * 	Lose old mbuf code for inpcb's and stick with MALLOC.
 * 	[90/01/08  16:03:34  tmt]
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
 * Revision 1.3  90/01/03  12:41:31  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:46  gm]
 * 
 * Revision 1.2  89/12/26  10:10:45  gm
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
 * Copyright (c) 1982, 1986 Regents of the University of California.
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
 *	Merged:	in_pcb.c	7.13 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/socketvar.h"
#include "sys/protosw.h"

#include "net/if.h"
#include "net/route.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/in_pcb.h"
#include "netinet/in_var.h"
#include "netinet/ip_var.h"

#include "net/net_malloc.h"

LOCK_ASSERTL_DECL

CONST struct	in_addr zeroin_addr;

in_pcballoc(so, head)
	struct socket *so;
	struct inpcb *head;
{
	register struct inpcb *inp;

	NET_MALLOC(inp, struct inpcb *, sizeof *inp, M_PCB, M_NOWAIT);
	if (inp == NULL)
		return (ENOBUFS);
	bzero((caddr_t)inp, sizeof *inp);
	inp->inp_head = head;
	inp->inp_socket = so;
	INPCB_LOCKINIT(inp);
	INPCBRC_LOCKINIT(inp);
	inp->inp_refcnt = 1;
	INHEAD_WRITE_LOCK(head);
	insque(inp, head);
	so->so_pcb = (caddr_t)inp;
	INHEAD_WRITE_UNLOCK(head);
	return (0);
}
	
in_pcbbind(inp, nam)
	register struct inpcb *inp;
	struct mbuf *nam;
{
	register struct socket *so = inp->inp_socket;
	register struct inpcb *head = inp->inp_head;
	register struct sockaddr_in *sin;
	int error = 0;
	u_short lport = 0;
#define	ret(err)	{ error = (err); goto out; }

	LOCK_ASSERT("in_pcbbind", INPCB_ISLOCKED(inp));
	if (in_ifaddr == 0)
		return (EADDRNOTAVAIL);
	if (inp->inp_lport || inp->inp_laddr.s_addr != INADDR_ANY)
		return (EINVAL);
	if (nam == 0) {
		INHEAD_WRITE_LOCK(head);
		goto noname;
	}
	sin = mtod(nam, struct sockaddr_in *);
	if (nam->m_len != sizeof (*sin))
		return (EINVAL);
	if (sin->sin_addr.s_addr != INADDR_ANY) {
		lport = sin->sin_port;
		sin->sin_port = 0;		/* yech... */
		if (ifa_ifwithaddr((struct sockaddr *)sin) == 0)
			return (EADDRNOTAVAIL);
		sin->sin_port = lport;
	} else
		lport = sin->sin_port;
	INHEAD_WRITE_LOCK(head);
	if (lport) {
		u_short aport = ntohs(lport);
		int wild = 0;
		struct inpcb *inp2;

		/* GROSS */
		if (aport < IPPORT_RESERVED && !(so->so_state & SS_PRIV))
			ret(EACCES);
		/* even GROSSER, but this is the Internet */
		if ((so->so_options & SO_REUSEADDR) == 0 &&
		    ((so->so_proto->pr_flags & PR_CONNREQUIRED) == 0 ||
		     (so->so_options & SO_ACCEPTCONN) == 0))
			wild = INPLOOKUP_WILDCARD;
		inp2 = in_pcblookup(head,
		    zeroin_addr, 0, sin->sin_addr, lport, wild);
		if (inp2) {
			INPCBRC_UNREF(inp2);
			ret(EADDRINUSE);
		}
	}
	inp->inp_laddr = sin->sin_addr;
noname:
	if (lport == 0) {
		u_short firstport = 0;
		do {
			if (head->inp_lport++ < IPPORT_RESERVED ||
			    head->inp_lport > IPPORT_USERRESERVED)
				head->inp_lport = IPPORT_RESERVED;
			if (firstport == 0)
				firstport = head->inp_lport;
			else if (firstport == head->inp_lport)
				ret(EADDRINUSE);
			lport = htons(head->inp_lport);
		} while (in_pcbmatch(head,
			    zeroin_addr, 0, inp->inp_laddr, lport));
	}
	inp->inp_lport = lport;
out:
	INHEAD_WRITE_UNLOCK(head);
	return (error);
}

/*
 * Connect from a socket to a specified address.
 * Both address and port must be specified in argument sin.
 * If don't have a local address for this socket yet,
 * then pick one.
 */
in_pcbconnect(inp, nam)
	register struct inpcb *inp;
	struct mbuf *nam;
{
	struct in_ifaddr *ia;
	struct sockaddr_in *ifaddr;
	register struct sockaddr_in *sin = mtod(nam, struct sockaddr_in *);

	LOCK_ASSERT("in_pcbconnect", INPCB_ISLOCKED(inp));
	if (nam->m_len != sizeof (*sin))
		return (EINVAL);
	if (sin->sin_family != AF_INET)
		return (EAFNOSUPPORT);
	if (sin->sin_port == 0)
		return (EADDRNOTAVAIL);
	if (in_ifaddr) {
		/*
		 * If the destination address is INADDR_ANY,
		 * use the primary local address.
		 * If the supplied address is INADDR_BROADCAST,
		 * and the primary interface supports broadcast,
		 * choose the broadcast address for that interface.
		 */
#define	satosin(sa)	((struct sockaddr_in *)(sa))
		if (sin->sin_addr.s_addr == INADDR_ANY)
		    sin->sin_addr = IA_SIN(in_ifaddr)->sin_addr;
		else if (sin->sin_addr.s_addr == (u_long)INADDR_BROADCAST &&
		  (in_ifaddr->ia_ifp->if_flags & IFF_BROADCAST))
		    sin->sin_addr = satosin(&in_ifaddr->ia_broadaddr)->sin_addr;
	}
	if (inp->inp_laddr.s_addr == INADDR_ANY) {
		register struct route *ro;
		struct ifnet *ifp;
		ROUTE_LOCK_DECL()

		ia = (struct in_ifaddr *)0;
		/* 
		 * If route is known or can be allocated now,
		 * our src addr is taken from the i/f, else punt.
		 */
		ROUTE_WRITE_LOCK();
		ro = &inp->inp_route;
		if (ro->ro_rt &&
		    (satosin(&ro->ro_dst)->sin_addr.s_addr !=
			sin->sin_addr.s_addr || 
		    inp->inp_socket->so_options & SO_DONTROUTE)) {
			RTFREE(ro->ro_rt);
			ro->ro_rt = (struct rtentry *)0;
		}
		if ((inp->inp_socket->so_options & SO_DONTROUTE) == 0 && /*XXX*/
		    (ro->ro_rt == (struct rtentry *)0 ||
		    ro->ro_rt->rt_ifp == (struct ifnet *)0)) {
			/* No route yet, so try to acquire one */
			ro->ro_dst.sa_family = AF_INET;
			ro->ro_dst.sa_len = sizeof(struct sockaddr_in);
			((struct sockaddr_in *) &ro->ro_dst)->sin_addr =
				sin->sin_addr;
			rtalloc(ro);
		}
		ROUTE_WRITE_UNLOCK();
		/*
		 * If we found a route, use the address
		 * corresponding to the outgoing interface
		 * unless it is the loopback (in case a route
		 * to our address on another net goes to loopback).
		 */
		if (ro->ro_rt && (ifp = ro->ro_rt->rt_ifp) &&
		    (ifp->if_flags & IFF_LOOPBACK) == 0)
			for (ia = in_ifaddr; ia; ia = ia->ia_next) {
#include "mi.h"
#if defined(TNC) && NMI > 0
#include <net/if_types.h>
				/* Use a real local i/f to connect via MI */
				extern struct in_ifaddr *ifa_real_with_af();
				if (ia->ia_ifp == ifp &&
				    ifp->if_type == IFT_MI) {
					ia = ifa_real_with_af(AF_INET);
					break;
				}
#endif
				if (ia->ia_ifp == ifp)
					break;
			}
		if (ia == 0) {
			u_short fport = sin->sin_port;

			sin->sin_port = 0;
			ia = (struct in_ifaddr *)
			    ifa_ifwithdstaddr((struct sockaddr *)sin);
			sin->sin_port = fport;
			if (ia == 0)
				ia = in_iaonnetof(in_netof(sin->sin_addr));
			if (ia == 0)
				ia = in_ifaddr;
			if (ia == 0)
				return (EADDRNOTAVAIL);
		}
		ifaddr = (struct sockaddr_in *)&ia->ia_addr;
	}
	INHEAD_WRITE_LOCK(inp->inp_head);
	if (in_pcbmatch(inp->inp_head,
	    sin->sin_addr,
	    sin->sin_port,
	    inp->inp_laddr.s_addr ? inp->inp_laddr : ifaddr->sin_addr,
	    inp->inp_lport)) {
		INHEAD_WRITE_UNLOCK(inp->inp_head);
		return (EADDRINUSE);
	}
	if (inp->inp_laddr.s_addr == INADDR_ANY) {
		if (inp->inp_lport == 0)
			(void)in_pcbbind(inp, (struct mbuf *)0);
		inp->inp_laddr = ifaddr->sin_addr;
	}
	inp->inp_faddr = sin->sin_addr;
	inp->inp_fport = sin->sin_port;
	INHEAD_WRITE_UNLOCK(inp->inp_head);
	return (0);
}

void
in_pcbdisconnect(inp)
	struct inpcb *inp;
{
	LOCK_ASSERT("in_pcbdisconnect", INPCB_ISLOCKED(inp));
	INHEAD_WRITE_LOCK(inp->inp_head);
	inp->inp_faddr.s_addr = INADDR_ANY;
	inp->inp_fport = 0;
	if (inp->inp_socket->so_state & SS_NOFDREF)
		in_pcbdetach(inp);
	INHEAD_WRITE_UNLOCK(inp->inp_head);
}

void
in_pcbdetach(inp)
	struct inpcb *inp;
{
	LOCK_ASSERT("in_pcbdetach", INPCB_ISLOCKED(inp));
	LOCK_ASSERT("in_pcbdetach so", SOCKET_ISLOCKED(inp->inp_socket));

	INHEAD_WRITE_LOCK(inp->inp_head);
	if (inp == inp->inp_head->inp_head)	/* Check cached pcb */
		inp->inp_head->inp_head = 0;
	remque(inp);
	inp->inp_next = inp->inp_prev = 0;
	INHEAD_WRITE_UNLOCK(inp->inp_head);
	INPCB_UNLOCK(inp);
	INPCBRC_UNREF(inp);		/* Does in_pcbfree if unref */
}

void
in_pcbfree(inp)
	struct inpcb *inp;
{
	int refcnt;

	LOCK_ASSERT("in_pcbfree so", SOCKET_ISLOCKED(inp->inp_socket));

	INHEAD_WRITE_LOCK(inp->inp_head);
	if (inp->inp_next)
		panic("in_pcbfree");
	INPCBRC_LOCK(inp);
	refcnt = inp->inp_refcnt;
	INPCBRC_UNLOCK(inp);
	INHEAD_WRITE_UNLOCK(inp->inp_head);
	if (refcnt <= 1) {			/* Check for race */
		inp->inp_socket->so_pcb = 0;
		sofree(inp->inp_socket);
		if (inp->inp_options)
			(void)m_free(inp->inp_options);
		if (inp->inp_route.ro_rt)
			rtfree(inp->inp_route.ro_rt);
		NET_FREE(inp, M_PCB);
	}
}

void
in_setsockaddr(inp, nam)
	register struct inpcb *inp;
	struct mbuf *nam;
{
	register struct sockaddr_in *sin;
	
	LOCK_ASSERT("in_setsockaddr", INPCB_ISLOCKED(inp));
	nam->m_len = sizeof (*sin);
	sin = mtod(nam, struct sockaddr_in *);
	bzero((caddr_t)sin, sizeof (*sin));
	sin->sin_family = AF_INET;
	sin->sin_len = sizeof(*sin);
	sin->sin_port = inp->inp_lport;
	sin->sin_addr = inp->inp_laddr;
}

void
in_setpeeraddr(inp, nam)
	struct inpcb *inp;
	struct mbuf *nam;
{
	register struct sockaddr_in *sin;
	
	LOCK_ASSERT("in_setpeeraddr", INPCB_ISLOCKED(inp));
	nam->m_len = sizeof (*sin);
	sin = mtod(nam, struct sockaddr_in *);
	bzero((caddr_t)sin, sizeof (*sin));
	sin->sin_family = AF_INET;
	sin->sin_len = sizeof(*sin);
	sin->sin_port = inp->inp_fport;
	sin->sin_addr = inp->inp_faddr;
}

/*
 * Pass some notification to all connections of a protocol
 * associated with address dst.  The local address and/or port numbers
 * may be specified to limit the search.  The "usual action" will be
 * taken, depending on the ctlinput cmd.  The caller must filter any
 * cmds that are uninteresting (e.g., no error in the map).
 * Call the protocol specific routine (if any) to report
 * any errors for each matching socket.
 */
void
#ifndef _NO_PROTO
in_pcbnotify(
	struct inpcb *head,
	struct sockaddr *dst,
	u_short fport,
	struct in_addr laddr,
	u_short lport,
	int cmd,
	void (*notify)())
#else
in_pcbnotify(head, dst, fport, laddr, lport, cmd, notify)
	struct inpcb *head;
	struct sockaddr *dst;
	u_short fport, lport;
	struct in_addr laddr;
	int cmd;
	void (*notify)();
#endif
{
	register struct inpcb *inp, *inpnxt;
	register struct socket *so;
	struct in_addr faddr;
	int errno;

	if ((unsigned)cmd > PRC_NCMDS || dst->sa_family != AF_INET)
		return;
	faddr = ((struct sockaddr_in *)dst)->sin_addr;
	if (faddr.s_addr == INADDR_ANY)
		return;

	/*
	 * Redirects go to all references to the destination,
	 * and use in_rtchange to invalidate the route cache.
	 * Dead host indications: notify all references to the destination.
	 * Otherwise, if we have knowledge of the local port and address,
	 * deliver only to that socket.
	 */
	if (PRC_IS_REDIRECT(cmd) || cmd == PRC_HOSTDEAD) {
		fport = 0;
		lport = 0;
		laddr.s_addr = 0;
		if (cmd != PRC_HOSTDEAD)
			notify = in_rtchange;
	}
	if (notify == 0)
		return;
	errno = inetctlerrmap[cmd];

resync:	/* XXX */
	INHEAD_READ_LOCK(head);
	if (inp = head->inp_next) {
		INPCBRC_REF(inp);
		for(; inp != head; inp = inpnxt) {
			inpnxt = inp->inp_next;
			INPCBRC_REF(inpnxt);
			if (inp->inp_faddr.s_addr != faddr.s_addr ||
			    inp->inp_socket == 0 ||
			    (lport && inp->inp_lport != lport) ||
			    (laddr.s_addr && inp->inp_laddr.s_addr != laddr.s_addr) ||
			    (fport && inp->inp_fport != fport)) {
				INPCBRC_UNREF(inp);
				continue;
			}
			INHEAD_READ_UNLOCK(head);
			so = inp->inp_socket;
			SOCKET_LOCK(so);
			INPCB_LOCK(inp);
			(*notify)(inp, errno);	/* <- */
			INPCB_UNLOCK(inp);
			INPCBRC_UNREF(inp);
			SOCKET_UNLOCK(so);
			INHEAD_READ_LOCK(head);
			/* We only resync if we lose our next inpcb */
			if (inpnxt->inp_next == 0) {
				INHEAD_READ_UNLOCK(head);
				so = inpnxt->inp_socket;
				SOCKET_LOCK(so);
				INPCBRC_UNREF(inpnxt);
				SOCKET_UNLOCK(so);
				goto resync;
			}
		}
		INPCBRC_UNREF(inp);	/* inp == head here */
	}
	INHEAD_READ_UNLOCK(head);
}

/*
 * Check for alternatives when higher level complains
 * about service problems.  For now, invalidate cached
 * routing information.  If the route was created dynamically
 * (by a redirect), time to try a default gateway again.
 */
void
in_losing(inp)
	struct inpcb *inp;
{
	register struct rtentry *rt;

	LOCK_ASSERT("in_losing", INPCB_ISLOCKED(inp));
	if ((rt = inp->inp_route.ro_rt)) {
		rt_missmsg(RTM_LOSING, &inp->inp_route.ro_dst,
			    rt->rt_gateway, (struct sockaddr *)rt_mask(rt),
			    (struct sockaddr *)0, rt->rt_flags, 0);
		if (rt->rt_flags & RTF_DYNAMIC)
			(void) rtrequest(RTM_DELETE, rt_key(rt),
				rt->rt_gateway, rt_mask(rt), rt->rt_flags, 
				(struct rtentry **)0);
		inp->inp_route.ro_rt = 0;
		rtfree(rt);
		/*
		 * A new route can be allocated
		 * the next time output is attempted.
		 */
	}
}

#if	NETSYNC_LOCK
void
in_losing_lock(inp)
	struct inpcb *inp;
{
	INPCB_LOCK(inp);
	in_losing(inp);
	INPCB_UNLOCK(inp);
}
#endif

/*
 * After a routing change, flush old routing
 * and allocate a (hopefully) better one.
 */
void
in_rtchange(inp)
	register struct inpcb *inp;
{
	LOCK_ASSERT("in_rtchange", INPCB_ISLOCKED(inp));
	if (inp->inp_route.ro_rt) {
		rtfree(inp->inp_route.ro_rt);
		inp->inp_route.ro_rt = 0;
		/*
		 * A new route can be allocated the next time
		 * output is attempted.
		 */
	}
}

struct inpcb *
#ifndef _NO_PROTO
in_pcblookup(
	struct inpcb *head,
	struct in_addr faddr,
	u_short fport,
	struct in_addr laddr,
	u_short lport,
	int flags)
#else
in_pcblookup(head, faddr, fport, laddr, lport, flags)
	struct inpcb *head;
	struct in_addr faddr, laddr;
	u_short fport, lport;
	int flags;
#endif
{
	register struct inpcb *inp, *match = 0;
	int matchwild = 3, wildcard;

	INHEAD_READ_LOCK(head);
	if ((flags & INPLOOKUP_USECACHE) && (inp = head->inp_head) &&
	    inp->inp_lport == lport && inp->inp_fport == fport &&
	    inp->inp_faddr.s_addr == faddr.s_addr &&
	    inp->inp_laddr.s_addr == laddr.s_addr)
		match = inp;			/* Cache hit */
	else for (inp = head->inp_next; inp != head; inp = inp->inp_next) {
		if (inp->inp_lport != lport)
			continue;
		wildcard = 0;
		if (inp->inp_laddr.s_addr != INADDR_ANY) {
			if (laddr.s_addr == INADDR_ANY)
				wildcard++;
			else if (inp->inp_laddr.s_addr != laddr.s_addr)
				continue;
		} else {
			if (laddr.s_addr != INADDR_ANY)
				wildcard++;
		}
		if (inp->inp_faddr.s_addr != INADDR_ANY) {
			if (faddr.s_addr == INADDR_ANY)
				wildcard++;
			else if (inp->inp_faddr.s_addr != faddr.s_addr ||
			    inp->inp_fport != fport)
				continue;
		} else {
			if (faddr.s_addr != INADDR_ANY)
				wildcard++;
		}
		if (wildcard && (flags & INPLOOKUP_WILDCARD) == 0)
			continue;
		if (wildcard < matchwild) {
			match = inp;
			matchwild = wildcard;
			if (matchwild == 0)
				break;
		}
	}
	if (match) {
		INPCBRC_REF(match);
		if (flags & INPLOOKUP_USECACHE)
			head->inp_head = match;	/* save for next cache */
	}
	INHEAD_READ_UNLOCK(head);
	return (match);
}

/* This is just in_pcblookup with no wildcard flags. It's a bit faster. */
/* If NETSYNC_LOCK, it does not take locks or return a referenced INPCB. */
struct inpcb *
#ifndef _NO_PROTO
in_pcbmatch(
	struct inpcb *head,
	struct in_addr faddr,
	u_short fport,
	struct in_addr laddr,
	u_short lport)
#else
in_pcbmatch(head, faddr, fport, laddr, lport)
	struct inpcb *head;
	struct in_addr faddr, laddr;
	u_short fport, lport;
#endif
{
	register struct inpcb *inp;

	for (inp = head->inp_next; inp != head; inp = inp->inp_next) {
		if (inp->inp_lport != lport)
			continue;
		if (inp->inp_laddr.s_addr != INADDR_ANY) {
			if (laddr.s_addr == INADDR_ANY)
				continue;
			else if (inp->inp_laddr.s_addr != laddr.s_addr)
				continue;
		} else {
			if (laddr.s_addr != INADDR_ANY)
				continue;
		}
		if (inp->inp_faddr.s_addr != INADDR_ANY) {
			if (faddr.s_addr == INADDR_ANY)
				continue;
			else if (inp->inp_faddr.s_addr != faddr.s_addr ||
			    inp->inp_fport != fport)
				continue;
		} else {
			if (faddr.s_addr != INADDR_ANY)
				continue;
		}
		return inp;
	}
	return 0;
}
