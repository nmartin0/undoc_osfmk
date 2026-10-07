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
 * $Log: route.c,v $
 * Revision 1.9  1995/02/01  21:34:14  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:34:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/11/03  01:30:23  slk
 *  Change Description: This is a two part fix.
 *  1)  In server/vsocket/if_mi.c remove stale #ifdef NOTDEF surrounding
 *  a counter decrement.
 *  2)  In server/net/route.c force the proper address to be placed in the
 *  ifaddr function pointer by calling ifa_withroute after the rn_delete.
 *  This causes the code to follow the path to decrementing the counter.
 *
 *  Reviewer(s): Nina Lepak, Mike Leibensperger, Surender Brahmaroutu
 *  Risk: Med
 *  Benefit or PTS #: 10950 H-0
 *       changing or deleting routes corrupts the routing table displayed
 *       by netstat -r
 *  Testing: Configured with one network server on and off the boot node,
 * 	two network servers on same and different networks.  For each
 * 	of those configurations performed a ping, telnet and rsh into
 * 	and out of the paragon, on network servers and non-network server
 * 	nodes.
 * 	EATS: Lachman, TCP/IP, NFS, EAT, controlc, message, ipd,
 * 	os_interfaces, xtrnl (VSX), misc, rmcall.
 *  Module(s): server/net/route.c
 * 	   server/vsocket/if_mi.c
 *
 * Revision 1.6  1993/07/14  18:09:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:29:07  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:00:22  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:06:28  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:00:25  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:57:33  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:27:19  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1992/11/11  18:34:41  loverso
 * 	Delete obs rthost/rtnet (see 1.1 or BNR2)
 *
 * Revision 2.2  91/08/31  13:41:36  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:34:16  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12  90/10/07  14:33:03  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:12:04  gm]
 * 
 * Revision 1.11  90/08/24  12:14:13  devrcs
 * 	Make rtioctl netmask dynamic with table driven function call.
 * 	Remove domain-specific includes.
 * 	[90/08/19  14:15:52  tmt]
 * 
 * Revision 1.10  90/07/27  08:59:54  devrcs
 * 	Update to BSD Reno release.
 * 	Move locks into rtfree/rtalloc/etc, using new macros in route.h.
 * 	[90/07/19  16:57:12  tmt]
 * 
 * Revision 1.9  90/07/05  23:12:51  devrcs
 * 	Change SPL macros to NETSPL... evaluate need.
 * 	[90/07/03  18:49:04  tmt]
 * 
 * Revision 1.8  90/05/13  18:44:54  devrcs
 * 	Provide radixnode heads for all address families.
 * 	[90/04/30  10:30:40  tmt]
 * 
 * Revision 1.7  90/04/27  19:14:08  devrcs
 * 	Change rtinitheads to just do one at a time. Change name.
 * 	[90/04/20  12:34:17  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:46  devrcs
 * 	Move declarations here, strengthen types (void).
 * 	[90/04/09  16:20:02  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:11  robert
 * 	Use macro for socket islocked.
 * 	[90/01/19  14:45:44  tmt]
 * 
 * Revision 1.4  90/01/18  08:44:25  gm
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
 * Revision 1.3  90/01/03  12:41:05  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:16  gm]
 * 
 * Revision 1.2  89/12/26  09:47:45  gm
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
 *	Base:	route.c	7.12 (Berkeley) 5/4/89
 *	Merged:	route.c	7.17 (Berkeley) 6/28/90
 */
 
#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"
#include "sys/ioctl.h"

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

#include "net/rtsock.c"

#define	SA(p)	((struct sockaddr *)(p))

int	rttrash;		/* routes not in table but not freed */
int	rthashsize = RTHASHSIZ;	/* for netstat, etc. */

struct	radix_node_head *rnheads[AF_MAX];
static	void (*setroutemask[AF_MAX])();	/* See rtioctl() */

struct	route_cb route_cb;
struct	rtstat	rtstat;
#if	NETSYNC_LOCK
lock_data_t	route_lock;
#endif

void
rtinithead(family, count, fcn)
	void (*fcn)();
{
	register struct radix_node_head *rnh;
	ROUTE_LOCK_DECL()

	if ((unsigned)family >= AF_MAX)
		panic("rtinithead");
	ROUTE_WRITE_LOCK();
	for (rnh = radix_node_head; rnh && (family != rnh->rnh_af); )
		rnh = rnh->rnh_next;
	if (rnh == 0) {
		rn_inithead(&rnheads[family], count, family);
		setroutemask[family] = fcn;
	}
	ROUTE_WRITE_UNLOCK();
}

/*
 * Packet routing routines.
 */
void
rtalloc(ro)
	register struct route *ro;
{
	ROUTE_LOCK_DECL()

	ROUTE_WRITE_LOCK();
	if (ro->ro_rt && ro->ro_rt->rt_ifp && (ro->ro_rt->rt_flags & RTF_UP))
		return;				 /* XXX */
	ro->ro_rt = rtalloc1(&ro->ro_dst, 1);
	ROUTE_WRITE_UNLOCK();
}

struct rtentry *
rtalloc1(dst, report)
	register struct sockaddr *dst;
	int  report;
{
	register struct radix_node_head *rnh;
	register struct rtentry *rt;
	register struct radix_node *rn;
	struct rtentry *newrt = 0;
	int err = 0;
	ROUTE_LOCK_DECL()

	ROUTE_WRITE_LOCK();
	for (rnh = radix_node_head; rnh && (dst->sa_family != rnh->rnh_af); )
		rnh = rnh->rnh_next;
	if (rnh && rnh->rnh_treetop &&
	    (rn = rn_match((caddr_t)dst, rnh->rnh_treetop)) &&
	    ((rn->rn_flags & RNF_ROOT) == 0)) {
		newrt = rt = (struct rtentry *)rn;
		if (report && (rt->rt_flags & RTF_CLONING)) {
			if (err = rtrequest(RTM_RESOLVE, dst, SA(0),
						SA(0), 0, &newrt))
				goto miss;
		} else
			rt->rt_refcnt++;
	} else {
		NETSTAT_LOCK(&rtstat.rts_lock);
		rtstat.rts_unreach++;
		NETSTAT_UNLOCK(&rtstat.rts_lock);
	miss:	if (report)
			rt_missmsg(RTM_MISS, dst, SA(0), SA(0), SA(0), 0, err);
	}
	ROUTE_WRITE_UNLOCK();
	return (newrt);
}

void
rtfree(rt)
	register struct rtentry *rt;
{
	ROUTE_LOCK_DECL()

	ROUTE_WRITE_LOCK();
	if (rt == 0)
		panic("rtfree");
	rt->rt_refcnt--;
	if (rt->rt_refcnt <= 0 && (rt->rt_flags & RTF_UP) == 0) {
		rttrash--;
		if (rt->rt_nodes->rn_flags & (RNF_ACTIVE | RNF_ROOT))
			panic ("rtfree 2");
		NET_FREE(rt, M_RTABLE);
	}
	ROUTE_WRITE_UNLOCK();
}

/*
 * Force a routing table entry to the specified
 * destination to go through the given gateway.
 * Normally called as a result of a routing redirect
 * message from the network layer.
 */
void
rtredirect(dst, gateway, netmask, flags, src, rtp)
	struct sockaddr *dst, *gateway, *netmask, *src;
	int flags;
	struct rtentry **rtp;
{
	register struct rtentry *rt;
	int error = 0;
	short *stat = 0;
	ROUTE_LOCK_DECL()

	ROUTE_WRITE_LOCK();
	/* verify the gateway is directly reachable */
	if (ifa_ifwithnet(gateway) == 0) {
		error = ENETUNREACH;
		goto done;
	}
	rt = rtalloc1(dst, 0);
	/*
	 * If the redirect isn't from our current router for this dst,
	 * it's either old or wrong.  If it redirects us to ourselves,
	 * we have a routing loop, perhaps as a result of an interface
	 * going down recently.
	 */
#define	equal(a1, a2) (bcmp((caddr_t)(a1), (caddr_t)(a2), (a1)->sa_len) == 0)
	if (!(flags & RTF_DONE) && rt && !equal(src, rt->rt_gateway))
		error = EINVAL;
	else if (ifa_ifwithaddr(gateway))
		error = EHOSTUNREACH;
	if (error)
		goto done;
	/*
	 * Create a new entry if we just got back a wildcard entry
	 * or the the lookup failed.  This is necessary for hosts
	 * which use routing redirects generated by smart gateways
	 * to dynamically build the routing tables.
	 */
	if ((rt == 0) || (rt_mask(rt) && rt_mask(rt)->sa_len < 2))
		goto create;
	/*
	 * Don't listen to the redirect if it's
	 * for a route to an interface. 
	 */
	if (rt->rt_flags & RTF_GATEWAY) {
		if (((rt->rt_flags & RTF_HOST) == 0) && (flags & RTF_HOST)) {
			/*
			 * Changing from route to net => route to host.
			 * Create new route, rather than smashing route to net.
			 */
		create:
			flags |=  RTF_GATEWAY | RTF_DYNAMIC;
			error = rtrequest((int)RTM_ADD, dst, gateway,
				    SA(0), flags, (struct rtentry **)0);
			stat = &rtstat.rts_dynamic;
		} else {
			/*
			 * Smash the current notion of the gateway to
			 * this destination.  Should check about netmask!!!
			 */
			if (gateway->sa_len <= rt->rt_gateway->sa_len) {
				Bcopy(gateway, rt->rt_gateway, gateway->sa_len);
				rt->rt_flags |= RTF_MODIFIED;
				flags |= RTF_MODIFIED;
				stat = &rtstat.rts_newgateway;
			} else
				error = ENOSPC;
		}
	} else
		error = EHOSTUNREACH;
done:
	if (rt) {
		if (rtp && !error)
			*rtp = rt;
		else
			rtfree(rt);
	}
	ROUTE_WRITE_UNLOCK();
	NETSTAT_LOCK(&rtstat.rts_lock);
	if (error)
		rtstat.rts_badredirect++;
	else
		(stat && (*stat)++);
	NETSTAT_UNLOCK(&rtstat.rts_lock);
	rt_missmsg(RTM_REDIRECT, dst, gateway, netmask, src, flags, error);
}

/*
* Routing table ioctl interface.
*/
rtioctl(so, req, data)
	struct socket *so;
	int req;
	caddr_t data;
{
#ifndef COMPAT_43
	return (EOPNOTSUPP);
#else
	register struct ortentry *entry = (struct ortentry *)data;
	int error;
	struct sockaddr *netmask = 0, protomask;

	if (req == SIOCADDRT)
		req = RTM_ADD;
	else if (req == SIOCDELRT)
		req = RTM_DELETE;
	else
		return (EINVAL);

	LOCK_ASSERT("rtioctl", SOCKET_ISLOCKED(so));

	if (!(so->so_state & SS_PRIV))
		return (EACCES);
#if BYTE_ORDER != BIG_ENDIAN
	if (entry->rt_dst.sa_family == 0 && entry->rt_dst.sa_len < 16) {
		entry->rt_dst.sa_family = entry->rt_dst.sa_len;
		entry->rt_dst.sa_len = 16;
	}
	if (entry->rt_gateway.sa_family == 0 && entry->rt_gateway.sa_len < 16) {
		entry->rt_gateway.sa_family = entry->rt_gateway.sa_len;
		entry->rt_gateway.sa_len = 16;
	}
#else
	if (entry->rt_dst.sa_len == 0)
		entry->rt_dst.sa_len = 16;
	if (entry->rt_gateway.sa_len == 0)
		entry->rt_gateway.sa_len = 16;
#endif
	if ((entry->rt_flags & RTF_HOST) == 0 &&
	    (unsigned)(entry->rt_dst.sa_family) < AF_MAX &&
	    setroutemask[entry->rt_dst.sa_family]) {
		bzero((caddr_t)&protomask, sizeof protomask);
		(*setroutemask[entry->rt_dst.sa_family])
			(&entry->rt_dst, &protomask);
		netmask = &protomask;
	}
	error = rtrequest(req, &(entry->rt_dst), &(entry->rt_gateway), netmask,
				entry->rt_flags, (struct rtentry **)0);
	rt_missmsg((req == RTM_ADD ? RTM_OLDADD : RTM_OLDDEL),
		   &(entry->rt_dst), &(entry->rt_gateway),
		   netmask, SA(0), entry->rt_flags, error);
	return (error);
#endif
}

struct ifaddr *
ifa_ifwithroute(flags, dst, gateway)
	int	flags;
	struct sockaddr	*dst, *gateway;
{
	struct ifaddr *ifa;
	if ((flags & RTF_GATEWAY) == 0) {
		/*
		 * If we are adding a route to an interface,
		 * and the interface is a pt to pt link
		 * we should search for the destination
		 * as our clue to the interface.  Otherwise
		 * we can use the local address.
		 */
		ifa = 0;
		if (flags & RTF_HOST) 
			ifa = ifa_ifwithdstaddr(dst);
		if (ifa == 0)
			ifa = ifa_ifwithaddr(gateway);
	} else {
		/*
		 * If we are adding a route to a remote net
		 * or host, the gateway may still be on the
		 * other end of a pt to pt link.
		 */
		ifa = ifa_ifwithdstaddr(gateway);
	}
	if (ifa == 0)
		ifa = ifa_ifwithnet(gateway);
	return (ifa);
}

rtrequest(req, dst, gateway, netmask, flags, ret_nrt)
	int req, flags;
	struct sockaddr *dst, *gateway, *netmask;
	struct rtentry **ret_nrt;
{
	int len, error = 0;
	register struct rtentry *rt;
	register struct radix_node *rn;
	register struct radix_node_head *rnh;
	struct ifaddr *ifa;
	struct sockaddr *ndst;
	u_char af = dst->sa_family;
	ROUTE_LOCK_DECL()
#undef	senderr
#define	senderr(x) { error = (x); goto bad; }

	ROUTE_WRITE_LOCK();
	for (rnh = radix_node_head; rnh && (af != rnh->rnh_af); )
		rnh = rnh->rnh_next;
	if (rnh == 0)
		senderr(ESRCH);
	if (flags & RTF_HOST)
		netmask = 0;
	switch (req) {
	case RTM_DELETE:
		if (ret_nrt && (rt = *ret_nrt)) {
			RTFREE(rt);
			*ret_nrt = 0;
		}
		if ((rn = rn_delete((caddr_t)dst, (caddr_t)netmask, 
					rnh->rnh_treetop)) == 0)
			senderr(ESRCH);

		if (rn->rn_flags & (RNF_ACTIVE | RNF_ROOT))
			panic ("rtrequest delete");
		rt = (struct rtentry *)rn;
		rt->rt_flags &= ~RTF_UP;

#ifdef	TNC
		/* XXX - Why is the ifaddr function pointer not
		 * filled in for the MI radix nodes?
		 */
		if ( (ifa = ifa_ifwithroute(flags, dst, gateway)) &&
			ifa->ifa_rtrequest )
			ifa->ifa_rtrequest(RTM_DELETE, rt, SA(0));
#else
		if ((ifa = rt->rt_ifa) && ifa->ifa_rtrequest)
			ifa->ifa_rtrequest(RTM_DELETE, rt, SA(0));
#endif	/* TNC */
		
		rttrash++;
		if (rt->rt_refcnt <= 0)
			rtfree(rt);
		break;

	case RTM_RESOLVE:
		if (ret_nrt == 0 || (rt = *ret_nrt) == 0)
			senderr(EINVAL);
		ifa = rt->rt_ifa;
		flags = rt->rt_flags & ~RTF_CLONING;
		gateway = rt->rt_gateway;
		if ((netmask = rt->rt_genmask) == 0)
			flags |= RTF_HOST;
		goto makeroute;

	case RTM_ADD:
		if ((ifa = ifa_ifwithroute(flags, dst, gateway)) == 0)
			senderr(ENETUNREACH);
	makeroute:
		len = sizeof (*rt) + RT_ROUNDUP(gateway) + RT_ROUNDUP(dst);
		R_Malloc(rt, struct rtentry *, len);
		if (rt == 0)
			senderr(ENOBUFS);
		Bzero(rt, len);
		ndst = (struct sockaddr *)(rt + 1);
		if (netmask) {
			rt_maskedcopy(dst, ndst, netmask);
		} else
			Bcopy(dst, ndst, dst->sa_len);
		rn = rn_addroute((caddr_t)ndst, (caddr_t)netmask,
					rnh->rnh_treetop, rt->rt_nodes);
		if (rn == 0) {
			NET_FREE(rt, M_RTABLE);
			senderr(EEXIST);
		} 

		rt->rt_ifa = ifa;
		rt->rt_ifp = ifa->ifa_ifp;
		rt->rt_flags = RTF_UP | flags;
		rn->rn_key = (caddr_t) ndst; /* == rt_dst */
		rt->rt_gateway = (struct sockaddr *)
					(rn->rn_key + RT_ROUNDUP(dst));
		Bcopy(gateway, rt->rt_gateway, gateway->sa_len);
		if (req == RTM_RESOLVE)
			rt->rt_rmx = (*ret_nrt)->rt_rmx; /* copy metrics */
		if (ifa->ifa_rtrequest)
			ifa->ifa_rtrequest(req, rt, SA(ret_nrt ? *ret_nrt : 0));
		if (ret_nrt) {
			*ret_nrt = rt;
			rt->rt_refcnt++;
		}
		break;
	}
bad:
	ROUTE_WRITE_UNLOCK();
	return (error);
}

void
rt_maskedcopy(src, dst, netmask)
	struct sockaddr *src, *dst, *netmask;
{
	register u_char *cp1 = (u_char *)src;
	register u_char *cp2 = (u_char *)dst;
	register u_char *cp3 = (u_char *)netmask;
	u_char *cplim = cp2 + *cp3;
	u_char *cplim2 = cp2 + *cp1;

	*cp2++ = *cp1++; *cp2++ = *cp1++; /* copies sa_len & sa_family */
	cp3 += 2;
	if (cplim > cplim2)
		cplim = cplim2;
	while (cp2 < cplim)
		*cp2++ = *cp1++ & *cp3++;
	if (cp2 < cplim2)
		bzero((caddr_t)cp2, (unsigned)(cplim2 - cp2));
}
/*
 * Set up a routing table entry, normally
 * for an interface.
 */
rtinit(ifa, cmd, flags)
	register struct ifaddr *ifa;
	int cmd, flags;
{
	return rtrequest(cmd, ifa->ifa_dstaddr, ifa->ifa_addr,
		    ifa->ifa_netmask, flags | ifa->ifa_flags, &ifa->ifa_rt);
}

#include "net/radix.c"
