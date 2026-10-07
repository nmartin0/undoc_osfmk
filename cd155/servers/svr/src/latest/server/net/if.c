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
 * $Log: if.c,v $
 * Revision 1.16  1995/02/01  21:32:33  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.15  1994/11/18  20:33:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/10/26  18:35:56  hobbes
 * 	Moved the declaration of ifdvnode back as it wasn't building
 * 	the heavy server the way I had changed it.
 *
 * Revision 1.13  1994/10/26  17:22:48  hobbes
 *  Reviewer: John Litvin
 *  Risk: fix build problem
 *  Benefit or PTS #: fix build problem
 *  Testing: fix build problem
 *  Module(s): net/if.c
 *
 * Revision 1.12  1994/10/21  17:39:09  nina
 *  Reviewer:yazz, hobbes
 *  Risk:Low
 *  Benefit or PTS #:11218
 *  Testing:Run ifconfig up command multiple times
 * 	on different nodes and then exercise
 * 	network
 *  Module(s):./server/net/if.c
 *
 *         For MI associations (net interface name has "-" as the first
 *         character), in the ifnet chain,extract the node number embedded
 * 	in the association's name and use that to match with the node
 * 	number embedded in the input parameter instead of the contents
 *         of ifp->if_dvnode.
 *
 * Revision 1.11  1994/03/03  19:27:55  slk
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 7016 merge from R1.2
 *  Testing: Build and Boot
 *  Module(s):
 *
 * Revision 1.10.4.1  1994/03/01  02:31:10  yazz
 *  Reviewer: Bernie Keany
 *  Risk: lo
 *  Benefit or PTS #: #7016
 *  Testing: extensive
 *  Module(s): server/net/if.c
 *
 *
 * Handle mi server entries in the ifnet list correctly, so as not to create
 * bogus entries in response to an ifconfig down command.
 *
 * Revision 1.10  1993/07/29  21:59:48  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server slowdown.
 *
 * Revision 1.9  1993/07/14  18:06:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:04:05  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.3  1993/07/01  19:27:20  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/06/08  21:57:27  hobbes
 * added RFC_1323 ifdefs to support the ramdisk server.
 *
 * Revision 1.7  1993/05/27  22:28:50  hobbes
 * Added ioctl handling for setting and deleting HIPPI
 * address resolution entries.
 *
 * Revision 1.6  1993/05/17  19:06:20  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.5  1993/05/06  18:59:24  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/14  15:13:17  cfj
 * Merge with T9.5
 *
 * Revision 1.2.8.2  1993/04/14  15:03:13  cfj
 * Remove defined(__i860__) where not needed.
 *
 * Revision 1.3  1993/03/25  21:29:49  cfj
 * T9 Merge.
 *
 * Revision 1.2.8.1  1993/03/24  23:40:08  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.1.1.1  1993/05/03  17:32:21  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.14  93/07/29  11:34:31  mjl
 * [Bug #0327] In ifconf(), when an IFF_INVISIBLE interface is seen, continue
 * on down the ifnet chain (don't break out of the loop!).
 * 
 * Revision 2.13  93/07/07  10:31:09  mjl
 * [LCC #0314] Use NET_THREADSTART() macro for deferred startup of net threads.
 * 
 * Revision 2.12  93/06/29  09:34:54  nina
 * Fix link errors in +tnc build. [LCC #297]
 * 
 * Revision 2.11  93/05/07  15:01:02  nina
 * Modified to hide MI ifnet blocks from GIFCONF unless
 * debugging the MI driver.
 * 
 * Revision 2.10  1993/04/16  14:12:04  nina
 * According to INTEL engineering, #if defined(__i860__) || defined(TNC)
 * should be #if defined(TNC)
 *
 * Revision 2.9  93/04/12  15:42:40  nina
 * Reconstruct interface name prefix #ifdef TNC. Don't bother
 * if only #ifdef __i860__
 * 
 * Revision 2.8  93/03/22  19:42:13  nina
 * Fixed i860 compilation errors. Do not return network serverse
 * node in reconstructed interface name string if it equals
 * device node.
 * 
 * Revision 2.7  93/02/25  17:47:11  nina
 * Modified to support new network interface naming scheme.
 * 
 * Revision 2.6  92/09/11  09:27:28  rabii
 * 	Added Intel's changes to deal with <node>cnp0 (rabii)
 * 
 * Revision 2.5  92/05/24  14:34:04  pjg
 * 	92/03/31  15:40:37  emcmanus
 * 	Name the if_slowtimo thread (if used).
 * 	[92/05/18            srl]
 * 
 * Revision 2.4  92/03/09  14:38:41  durriya
 * 	92/01/30  16:05:34  sp
 * 	Remove MACH_NO_KERNEL conditionals
 * 
 * 	91/12/18  17:16:15  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.3  91/10/14  12:35:24  sjs
 * 	91/10/01  14:09:55  condict
 * 	Protect against multiple calls of ifinit.
 * 
 * Revision 2.2  91/08/31  13:38:33  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:32:35  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10  90/10/07  14:31:23  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:09:30  gm]
 * 
 * Revision 1.9  90/08/24  12:13:28  devrcs
 * 	Allow dynamic attach of ARP (arpioctl).
 * 	[90/08/17  18:04:10  tmt]
 * 
 * Revision 1.8  90/07/27  08:58:32  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:23:10  tmt]
 * 
 * Revision 1.7  90/04/27  19:09:15  devrcs
 * 	Finally get COMPAT_43 right.
 * 	[90/04/20  12:22:53  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:08  devrcs
 * 	Move declarations here, strengthen types (void).
 * 	SIO/OSIO ioctl's always copy out old-style sockaddrs.
 * 	Allow two-digit interface names (xx0 -> xx99).
 * 	[90/04/09  16:05:33  tmt]
 * 
 * Revision 1.5  90/02/05  15:49:42  robert
 * 	Use macro for socket _islocked.
 * 	[90/01/19  14:21:42  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:04  gm
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
 * Revision 1.3  90/01/03  12:40:37  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:37:46  gm]
 * 
 * Revision 1.2  89/12/26  09:43:30  gm
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
 *	Base:	if.c	7.8 (Berkeley) 5/5/89
 *	Merged: if.c	7.13 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/kernel.h"
#include "sys/ioctl.h"
#include "sys/errno.h"

#ifdef OSF1_SERVER
#include <sys/synch.h>
#endif

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/socketvar.h"
#include "sys/protosw.h"

#include "net/if.h"
#include "net/if_dl.h"
#include "net/if_types.h"

#if RFC_1323
#include "net/if_llc.h"
#include "net/if_hippi.h"
#endif

#include "net/route.h"

#include "net/net_malloc.h"

#if defined(TNC)
#include "netinet/in.h"
#include "lnsvr/netname_defs.h"
#include "mi.h"
#include "vsocket/mi_var.h"
#endif

LOCK_ASSERTL_DECL

int	ifqmaxlen = IFQ_MAXLEN;

/*
 * Network interface utility routines.
 *
 * Routines with ifa_ifwith* names take sockaddr *'s as
 * parameters.
 */

struct	ifnet *ifnet;
extern	struct ifnet loif;
int	if_index, if_indexlim = 8;
struct	ifaddr **ifnet_addrs;

void
ifinit()
{
	register struct ifnet *ifp;

	loinit();		/* Be sure to init loopback */

	for (ifp = ifnet; ifp; ifp = ifp->if_next) {
		if (ifp->if_snd.ifq_maxlen == 0)
			ifp->if_snd.ifq_maxlen = ifqmaxlen;
		if (ifp->if_hdrlen > max_linkhdr)
			max_linkhdr = ifp->if_hdrlen;
	}

#if	!NETISR_THREAD
	if_slowtimo();
#else
	{	/* Protect against multiple calls: */
		static int first_time = 1;
		if (!first_time)
			return;
		first_time = 0;
	}
	NET_THREADSTART(if_slowtimo, -1, "if_slowtimo");
#endif
}

#ifdef	vax
void
ifubareset(uban)
{
	ifreset(uban);
}
#endif

/*
 * Call each interface on a bus reset.
 */
void
ifreset(n)
	int n;
{
	register struct ifnet *ifp;

	for (ifp = ifnet; ifp; ifp = ifp->if_next)
		if (ifp->if_reset)
			(*ifp->if_reset)(ifp->if_unit, n);
}

static void
sprint_d(cp, n)
	register char *cp;
	u_short n;
{
	register int q, m;
	do {
	    if (n >= 10000) m = 10000;
		else if (n >= 1000) m = 1000;
		else if (n >= 100) m = 100;
		else if (n >= 10) m = 10;
		else m = 1;
	    q = n / m;
	    n -= m * q;
	    if (q > 9) q = 10; /* For crays with more than 100K interfaces */
	    *cp++ = "0123456789Z"[q];
	} while (n > 0);
	*cp++ = 0;
}

/*
 * Attach an interface to the
 * list of "active" interfaces.
 */
void
if_attach(ifp)
	struct ifnet *ifp;
{
	unsigned socksize, ifasize;
	int namelen, unitlen;
	char workbuf[16];
	register struct ifnet **p = &ifnet;
	register struct sockaddr_dl *sdl;
	register struct ifaddr *ifa;

	if (ifp != &loif) {
		IFQ_LOCKINIT(&(ifp->if_snd));
		NETSTAT_LOCKINIT(&(ifp->if_slock));
	}
	while (*p)
		p = &((*p)->if_next);
	*p = ifp;

/* Parallelize me! */
	ifp->if_index = ++if_index;
	if (ifnet_addrs == 0 || if_index >= if_indexlim) {
		unsigned n = (if_indexlim <<= 1) * sizeof(ifa);
		struct ifaddr **q;
		NET_MALLOC(q, struct ifaddr **, n, M_IFADDR, M_WAITOK);
		if (ifnet_addrs) {
			bcopy((caddr_t)ifnet_addrs, (caddr_t)q, n/2);
			NET_FREE(ifnet_addrs, M_IFADDR);
		}
		ifnet_addrs = q;
	}
	/*
	 * create a Link Level name for this device
	 */
	sprint_d(workbuf, ifp->if_unit);
	namelen = strlen(ifp->if_name);

	unitlen = strlen(workbuf);
#define _offsetof(t, m) ((int)((caddr_t)&((t *)0)->m))
	socksize = _offsetof(struct sockaddr_dl, sdl_data[0]) +
				unitlen + namelen + ifp->if_addrlen;
	socksize = (socksize + (sizeof(long)-1)) & ~(sizeof(long)-1);
	if (socksize < sizeof(*sdl))
		socksize = sizeof(*sdl);
	ifasize = sizeof(*ifa) + 2 * socksize;
	NET_MALLOC(ifa, struct ifaddr *, ifasize, M_IFADDR, M_WAITOK);
	ifnet_addrs[if_index - 1] = ifa;
	bzero((caddr_t)ifa, ifasize);
	sdl = (struct sockaddr_dl *)(ifa + 1);
	ifa->ifa_addr = (struct sockaddr *)sdl;
	ifa->ifa_ifp = ifp;
	sdl->sdl_len = socksize;
	sdl->sdl_family = AF_LINK;
	bcopy(ifp->if_name, sdl->sdl_data, namelen);
	bcopy((caddr_t)workbuf, namelen + (caddr_t)sdl->sdl_data, unitlen);
	sdl->sdl_nlen = (namelen += unitlen);
	sdl->sdl_index = ifp->if_index;
	sdl = (struct sockaddr_dl *)(socksize + (caddr_t)sdl);
	ifa->ifa_netmask = (struct sockaddr *)sdl;
	sdl->sdl_len = socksize - ifp->if_addrlen;
	while (namelen != 0)
		sdl->sdl_data[--namelen] = 0xff;
	ifa->ifa_next = ifp->if_addrlist;
	ifa->ifa_rtrequest = link_rtrequest;
	ifp->if_addrlist = ifa;
}

/*
 * Locate an interface based on a complete address.
 */
/*ARGSUSED*/
struct ifaddr *
ifa_ifwithaddr(addr)
	register struct sockaddr *addr;
{
	register struct ifnet *ifp;
	register struct ifaddr *ifa;

#define	equal(a1, a2) \
  (bcmp((caddr_t)(a1), (caddr_t)(a2), ((struct sockaddr *)(a1))->sa_len) == 0)
	for (ifp = ifnet; ifp; ifp = ifp->if_next)
	    for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family != addr->sa_family)
			continue;
		if (equal(addr, ifa->ifa_addr))
			return (ifa);
		if ((ifp->if_flags & IFF_BROADCAST) && ifa->ifa_broadaddr &&
		    equal(ifa->ifa_broadaddr, addr))
			return (ifa);
	}
	return ((struct ifaddr *)0);
}
/*
 * Locate the point to point interface with a given destination address.
 */
/*ARGSUSED*/
struct ifaddr *
ifa_ifwithdstaddr(addr)
	register struct sockaddr *addr;
{
	register struct ifnet *ifp;
	register struct ifaddr *ifa;

	for (ifp = ifnet; ifp; ifp = ifp->if_next) 
	    if (ifp->if_flags & IFF_POINTOPOINT)
		for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr->sa_family != addr->sa_family)
				continue;
			if (equal(addr, ifa->ifa_dstaddr))
				return (ifa);
	}
	return ((struct ifaddr *)0);
}

/*
 * Find an interface on a specific network.  If many, choice
 * is first found.
 */
struct ifaddr *
ifa_ifwithnet(addr)
	struct sockaddr *addr;
{
	register struct ifnet *ifp;
	register struct ifaddr *ifa;
	u_int af = addr->sa_family;

	if (af >= AF_MAX)
		return (0);
	if (af == AF_LINK) {
		register struct sockaddr_dl *sdl = (struct sockaddr_dl *)addr;
		if (sdl->sdl_index && sdl->sdl_index <= if_index)
			return (ifnet_addrs[sdl->sdl_index - 1]);
	}
	for (ifp = ifnet; ifp; ifp = ifp->if_next)
	    for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next) {
		register char *cp, *cp2, *cp3;
		register char *cplim;
		if (ifa->ifa_addr->sa_family != af || ifa->ifa_netmask == 0)
			continue;
		cp = addr->sa_data;
		cp2 = ifa->ifa_addr->sa_data;
		cp3 = ifa->ifa_netmask->sa_data;
		cplim = ifa->ifa_netmask->sa_len + (char *)ifa->ifa_netmask;
		for (; cp3 < cplim; cp3++)
			if ((*cp++ ^ *cp2++) & *cp3)
				break;
		if (cp3 == cplim)
			return (ifa);
	    }
	return ((struct ifaddr *)0);
}

/*
 * Find an interface using a specific address family
 */
struct ifaddr *
ifa_ifwithaf(af)
	register int af;
{
	register struct ifnet *ifp;
	register struct ifaddr *ifa;

	for (ifp = ifnet; ifp; ifp = ifp->if_next)
	    for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next)
		if (ifa->ifa_addr->sa_family == af)
			return (ifa);
	return ((struct ifaddr *)0);
}

/*
 * Find an interface address specific to an interface best matching
 * a given address.
 */
struct ifaddr *
ifaof_ifpforaddr(addr, ifp)
	struct sockaddr *addr;
	register struct ifnet *ifp;
{
	register struct ifaddr *ifa;
	register char *cp, *cp2, *cp3;
	register char *cplim;
	struct ifaddr *ifa_maybe = 0;
	u_int af = addr->sa_family;

	if (af >= AF_MAX)
		return (0);
	for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family != af)
			continue;
		ifa_maybe = ifa;
		if (ifa->ifa_netmask == 0) {
			if (equal(addr, ifa->ifa_addr) ||
			    (ifa->ifa_dstaddr && equal(addr, ifa->ifa_dstaddr)))
				return (ifa);
			continue;
		}
		cp = addr->sa_data;
		cp2 = ifa->ifa_addr->sa_data;
		cp3 = ifa->ifa_netmask->sa_data;
		cplim = ifa->ifa_netmask->sa_len + (char *)ifa->ifa_netmask;
		for (; cp3 < cplim; cp3++)
			if ((*cp++ ^ *cp2++) & *cp3)
				break;
		if (cp3 == cplim)
			return (ifa);
	}
	return (ifa_maybe);
}

/*
 * Default action when installing a route with a Link Level gateway.
 * Lookup an appropriate real ifa to point to.
 * This should be moved to /sys/net/link.c eventually.
 */
void
link_rtrequest(cmd, rt, sa)
register struct rtentry *rt;
struct sockaddr *sa;
{
	register struct ifaddr *ifa;
	struct sockaddr *dst;
	struct ifnet *ifp, *oldifnet = ifnet;

	if (cmd != RTM_ADD || ((ifa = rt->rt_ifa) == 0) ||
	    ((ifp = ifa->ifa_ifp) == 0) || ((dst = rt_key(rt)) == 0))
		return;
	if (ifa = ifaof_ifpforaddr(dst, ifp)) {
		rt->rt_ifa = ifa;
		if (ifa->ifa_rtrequest && ifa->ifa_rtrequest != link_rtrequest)
			ifa->ifa_rtrequest(cmd, rt, sa);
	}
}
/*
 * Mark an interface down and notify protocols of
 * the transition.
 * NOTE: must be called at splnet or eqivalent.
 */
void
if_down(ifp)
	register struct ifnet *ifp;
{
	register struct ifaddr *ifa;

	ifp->if_flags &= ~IFF_UP;
	for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next)
		pfctlinput(PRC_IFDOWN, ifa->ifa_addr);
	if_qflush(&ifp->if_snd);
}

/*
 * Flush an interface queue.
 */
void
if_qflush(ifq)
	register struct ifqueue *ifq;
{
	register struct mbuf *m, *n;
	int s;

	s = splimp();
	IFQ_LOCK(ifq);
	n = ifq->ifq_head;
	while (m = n) {
		n = m->m_act;
		m_freem(m);
	}
	ifq->ifq_head = 0;
	ifq->ifq_tail = 0;
	ifq->ifq_len = 0;
	IFQ_UNLOCK(ifq);
	splx(s);
}

/*
 * Handle interface watchdog timer routines.  Called
 * from softclock, we decrement timers (if set) and
 * call the appropriate interface routine on expiration.
 */
if_slowtimo()
{
	register struct ifnet *ifp;
	int s = splimp();

	for (ifp = ifnet; ifp; ifp = ifp->if_next) {
		if (ifp->if_timer == 0 || --ifp->if_timer)
			continue;
		if (ifp->if_watchdog)
			(*ifp->if_watchdog)(ifp->if_unit);
	}
	splx(s);
#if	!NETISR_THREAD
	timeout(if_slowtimo, (caddr_t)0, hz / IFNET_SLOWHZ);
#else
	return (hz/IFNET_SLOWHZ);
#endif
}

/*
 * Map interface name to
 * interface structure pointer.
 */
struct ifnet *
ifunit(name)
	register char *name;
{
	register char *cp;
	register struct ifnet *ifp;
	int unit;
	unsigned len;
	char *ep, c;
	char *sh_name;
#ifdef  OSF1_SERVER
	char name2[IFNAMSIZ];
	node_t  dv_node, ifdvnode;
	extern char *net_interface_name();

	bcopy(name,name2,IFNAMSIZ);
#if defined (TNC)
        /* convert possible "<131>cnp0" --> "cnp0" */
        name = net_interface_name( name, &dv_node, NULL );
#endif
#endif

        for (cp = name; cp < name + IFNAMSIZ && *cp; cp++)
                if (*cp >= '0' && *cp <= '9')
                        break;
	if (*cp == '\0' || cp == name + IFNAMSIZ)
		return ((struct ifnet *)0);
	/*
	 * Save first char of unit, and pointer to it,
	 * so we can put a null there to avoid matching
	 * initial substrings of interface names.
	 */
	len = cp - name + 1;
	c = *cp;
	ep = cp;
	for (unit = 0; *cp >= '0' && *cp <= '9'; )
		unit = unit * 10 + *cp++ - '0';
	*ep = 0;
	for (ifp = ifnet; ifp; ifp = ifp->if_next) {
#if defined(TNC)
		ifdvnode = ifp->if_dvnode;	  
#endif
		if ( ifp->if_name[0] == '_' ) {	/* get stripped version, */
						/* without touching original */
			sh_name = ifp->if_name;
			sh_name++;
#if defined(TNC)
			sh_name = net_interface_name(sh_name, &ifdvnode, NULL);
#else
			sh_name = net_interface_name(sh_name, NULL, NULL);
#endif
		}
		else sh_name = ifp->if_name; 	/* interface on this_node */
		
		if (bcmp(sh_name, name, len)) 	/* attempt to match name */
			continue;
		if (unit == ifp->if_unit
#if defined(TNC)
 		  && ifdvnode == dv_node
#endif
		)
			break;
	}

#ifdef  OSF1_SERVER
        {
            extern struct ifnet *iface_find();
            if (ifp == 0)
                ifp = iface_find(name2);
        }
#endif

	*ep = c;
	return (ifp);
}

/*
 * Interface ioctls.
 */
ifioctl(so, cmd, data)
	struct socket *so;
	int cmd;
	caddr_t data;
{
	register struct ifnet *ifp;
	register struct ifreq *ifr;
	int error;

	LOCK_ASSERT("ifioctl", SOCKET_ISLOCKED(so));

	switch (cmd) {

	case SIOCGIFCONF:
#ifdef	OSIOCGIFCONF
	case OSIOCGIFCONF:
#endif
		return (ifconf(cmd, data));

#if RFC_1323
	case SIOCSHART:
	case SIOCDHART:
		if (!(so->so_state & SS_PRIV))
			 return (EACCES);
		return (hippi_arpioctl(cmd, data));
#endif

	case SIOCSARP:
	case SIOCDARP:
		if (!(so->so_state & SS_PRIV))
			return (EACCES);
		/* FALL THROUGH */
	case SIOCGARP:
#ifdef	OSIOCGARP
	case OSIOCGARP:
#endif
		return (arpioctl(cmd, data));
	}
	ifr = (struct ifreq *)data;
	ifp = ifunit(ifr->ifr_name);
	if (ifp == 0)
		return (ENXIO);
	switch (cmd) {

	case SIOCGIFFLAGS:
		ifr->ifr_flags = ifp->if_flags;
		break;

	case SIOCGIFMETRIC:
		ifr->ifr_metric = ifp->if_metric;
		break;

	case SIOCSIFFLAGS:
		if (!(so->so_state & SS_PRIV))
			return (EACCES);
		if ((ifp->if_flags&IFF_UP) && (ifr->ifr_flags&IFF_UP) == 0) {
			int s = splimp();
			if_down(ifp);
			splx(s);
		}
		ifp->if_flags = (ifp->if_flags & IFF_CANTCHANGE) |
			(ifr->ifr_flags &~ IFF_CANTCHANGE);
		if (ifp->if_ioctl) 
			(void) (*ifp->if_ioctl)(ifp, cmd, data);
		break;

	case SIOCSIFMETRIC:
		if (!(so->so_state & SS_PRIV))
			return (EACCES);
		ifp->if_metric = ifr->ifr_metric;
		break;

	default:
		if (so->so_proto == 0)
			return (EOPNOTSUPP);
#ifndef COMPAT_43
		return ((*so->so_proto->pr_usrreq)(so, PRU_CONTROL,
			cmd, data, ifp));
#else
	    {
		int ocmd = cmd;

		switch (cmd) {

		case SIOCSIFDSTADDR:
		case SIOCSIFADDR:
		case SIOCSIFBRDADDR:
		case SIOCSIFNETMASK:
#if BYTE_ORDER != BIG_ENDIAN
			if (ifr->ifr_addr.sa_family == 0 &&
			    ifr->ifr_addr.sa_len < 16) {
				ifr->ifr_addr.sa_family = ifr->ifr_addr.sa_len;
				ifr->ifr_addr.sa_len = 16;
			}
#else
			if (ifr->ifr_addr.sa_len == 0)
				ifr->ifr_addr.sa_len = 16;
#endif
			break;

#ifdef	OSIOCGIFADDR
		case OSIOCGIFADDR:
			cmd = SIOCGIFADDR;
			break;

		case OSIOCGIFDSTADDR:
			cmd = SIOCGIFDSTADDR;
			break;

		case OSIOCGIFBRDADDR:
			cmd = SIOCGIFBRDADDR;
			break;

		case OSIOCGIFNETMASK:
			cmd = SIOCGIFNETMASK;
#endif
		}
		error =  ((*so->so_proto->pr_usrreq)(so, PRU_CONTROL,
							    cmd, data, ifp));
#ifdef	OSIOCGIFADDR
		switch (ocmd) {

		case OSIOCGIFADDR:
		case OSIOCGIFDSTADDR:
		case OSIOCGIFBRDADDR:
		case OSIOCGIFNETMASK:
			((struct osockaddr *)&ifr->ifr_addr)->sa_family =
							ifr->ifr_addr.sa_family;
		}
#endif
		return (error);

	    }
#endif
	}
	return (0);
}

/*
 * Return interface configuration
 * of system.  List may be used
 * in later ioctl's (above) to get
 * other information.
 *
 * This function reconstructs the
 * complete interface name from info
 * stored in the ifnet block. The name
 * is in the form:
 * 	<DV,NS>interfaceunit
 *
 *	DV = device node
 *	NS = network server node
 *	interface = acronym for interface type
 *	unit = unit number
 */
/*ARGSUSED*/
ifconf(cmd, data)
	int cmd;
	caddr_t data;
{
	register struct ifconf *ifc = (struct ifconf *)data;
	register struct ifnet *ifp = ifnet;
	register struct ifaddr *ifa;
	register char *cp;
	struct ifreq ifr, *ifrp;
	int space = ifc->ifc_len, error = 0;
#if defined(TNC)
	extern char* itoa();
	extern node_t this_node;
#endif
	int	len;	/* tmp to avoid doing multiple strlen */

	ifrp = ifc->ifc_req;
	for (; space > sizeof (ifr) && ifp; ifp = ifp->if_next) {
		cp = ifr.ifr_name;			/* destination */
#if defined(TNC)
#if NMI > 0 && defined(MI_DEBUG)
		/* ignore MI driver ifnet blocks unless debugging */
		if((ifp->if_flags & IFF_INVISIBLE) && (!(mi_debug & MD_INVF)))
			continue;
#else
		if(ifp->if_flags & IFF_INVISIBLE)
			continue;
#endif
		*cp++ = '<';				/* start prefix */
		cp = itoa(ifp->if_dvnode, cp);		/* <DV		*/
		cp--;					/* wrt over '\0' */

                if((this_node != ifp->if_dvnode) && (strcmp("lo",ifp->if_name) != 0)) {
                        *cp++ = ',';                    /* <DV,         */
                        cp = itoa(this_node, cp);       /* <DV,NS       */
                        cp--;
                }
		*cp++ = '>';				/* <DV,NS> */
#endif
		/* Get interface type from ifnet block */
		len = strlen(ifp->if_name);
		bcopy(ifp->if_name, cp, len);
		cp += len;

		/* Almost done.  Append unit number and '\0' */
		itoa(ifp->if_unit,cp);		/* itoa appends '\0' */
		if ((ifa = ifp->if_addrlist) == 0) {
			bzero((caddr_t)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
			error = copyout((caddr_t)&ifr, (caddr_t)ifrp, sizeof (ifr));
			if (error)
				break;
			space -= sizeof (ifr), ifrp++;
		} else 
		    for ( ; space > sizeof (ifr) && ifa; ifa = ifa->ifa_next) {
			register struct sockaddr *sa = ifa->ifa_addr;
#if	defined(COMPAT_43) && defined(OSIOCGIFCONF)
			if (cmd == OSIOCGIFCONF) {
				struct osockaddr *osa =
					 (struct osockaddr *)&ifr.ifr_addr;
				ifr.ifr_addr = *sa;
				osa->sa_family = sa->sa_family;
				error = copyout((caddr_t)&ifr, (caddr_t)ifrp,
						sizeof (ifr));
				ifrp++;
			} else
#endif
			if (sa->sa_len <= sizeof(*sa)) {
				ifr.ifr_addr = *sa;
				error = copyout((caddr_t)&ifr, (caddr_t)ifrp,
						sizeof (ifr));
				ifrp++;
			} else {
				space -= sa->sa_len - sizeof(*sa);
				if (space < sizeof (ifr))
					break;
				error = copyout((caddr_t)&ifr, (caddr_t)ifrp,
						sizeof (ifr.ifr_name));
				if (error == 0)
				    error = copyout((caddr_t)sa,
				      (caddr_t)&ifrp->ifr_addr, sa->sa_len);
				ifrp = (struct ifreq *)
					(sa->sa_len + (caddr_t)&ifrp->ifr_addr);
			}
			if (error)
				break;
			space -= sizeof (ifr);
		}
	}
	ifc->ifc_len -= space;
	return (error);
}
