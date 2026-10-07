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
 * $Log: ip_output.c,v $
 * Revision 1.6  1995/02/01  21:35:16  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:35:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/01  01:35:48  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 2.5  93/08/27  16:28:12  nina
 * [LCC 375]. ip_ctloutput() gave a lock assertion failure on an
 * inpcb structure.  This was due to the fact that tcp_ctloutput()
 * and ip_ctloutput() were not properly coordinating their access
 * to the lock.  In addition, ip_ctloutuput(), returned an incorrect
 * error code if the level parameter was invalid.
 * 
 * ip_ctloutput() was modified to return ENOPROTOOPT for an invalid
 * level parameter.  ip_ctloutput() now locks the inpcb on entry
 * and unlocks it on exit.
 * 
 * Revision 2.4  93/08/20  18:59:15  mjl
 * [LCCbug #0355] If packet src addr is not yet known and packet is to go out
 * over an MI interface, choose a real local interface as its src addr.
 * 
 * Revision 1.3  1993/05/06  20:26:48  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:56  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:10  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:32  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:21:18  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/03/09  14:43:01  durriya
 * 	91/12/18  17:16:57  sp
 * 	Include sys/synch.h to get spl macros
 *
 * Revision 2.2  91/08/31  13:43:47  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:36:11  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11.4.3  91/03/15  17:49:06  tmt
 * 	Drop bogus 1st argument to in_pcbopts() call.
 * 	[91/03/13  19:10:48  tmt]
 * 
 * Revision 1.11.4.2  91/02/19  15:24:49  tmt
 * 	Set ip_ttl, not tos, in ip_ctloutput(IP_TTL).
 * 	[91/02/16  15:52:56  tmt]
 * 
 * Revision 1.11  90/10/07  14:35:09  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:15:25  gm]
 * 
 * Revision 1.10  90/07/27  09:01:25  devrcs
 * 	Update to BSD Reno release.
 * 	Remove or rearrange route locks - route.c handles.
 * 	[90/07/19  17:34:27  tmt]
 * 
 * Revision 1.9  90/06/29  13:37:04  devrcs
 * 	Remove machine dependent #ifdef.
 * 	[90/06/26  19:57:23  tmt]
 * 
 * Revision 1.8  90/06/22  20:39:13  devrcs
 * 	Fix leak in fragment output.
 * 	[90/06/11  11:21:44  tmt]
 * 
 * Revision 1.7  90/04/27  19:16:15  devrcs
 * 	Change code to handle new _NO_BITFIELDS ip header.
 * 	[90/04/20  12:57:21  tmt]
 * 
 * Revision 1.6  90/03/27  13:22:34  gm
 * 	Fix bug where last fragment got m_pkthdr, not first.
 * 	[90/03/07  12:19:43  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:37  robert
 * 	Use macro for inpcb_islocked.
 * 	[90/01/19  15:00:50  tmt]
 * 
 * Revision 1.4  90/01/18  08:47:14  gm
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
 * Revision 1.3  90/01/03  12:41:47  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:02  gm]
 * 
 * Revision 1.2  89/12/26  10:13:05  gm
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
 * Copyright (c) 1982, 1986, 1988 Regents of the University of California.
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
 *	Base:	ip_output.c	7.16 (Berkeley) 9/20/89
 *	Merged:	ip_output.c	7.21 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"

#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

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

LOCK_ASSERTL_DECL

/*
 * IP output.  The packet in mbuf chain m contains a skeletal IP
 * header (with len, off, ttl, proto, tos, src, dst).
 * The mbuf chain containing the packet will be freed.
 * The mbuf opt, if present, will not be freed.
 */
ip_output(m0, opt, ro, flags)
	struct mbuf *m0;
	struct mbuf *opt;
	struct route *ro;
	int flags;
{
	register struct ip *ip, *mhip;
	register struct ifnet *ifp;
	register struct mbuf *m = m0;
	register int hlen = sizeof (struct ip);
	int len, off, error = 0;
	struct route iproute;
	struct sockaddr_in *dst;
	struct in_ifaddr *ia;
	ROUTE_LOCK_DECL()

if ((m->m_flags & M_PKTHDR) == 0)
panic("ip_output no HDR");
	if (opt) {
		m = ip_insertoptions(m, opt, &len);
		hlen = len;
	}
	ip = mtod(m, struct ip *);
	/*
	 * Fill in IP header.
	 */
	if ((flags & IP_FORWARDING) == 0) {
		ip->ip_vhl = (IPVERSION << 4) | (hlen >> 2);
		ip->ip_off &= IP_DF;
		IPMISC_LOCK();
		ip->ip_id = htons(ip_id++);
		IPMISC_UNLOCK();
	} else {
		hlen = (ip->ip_vhl & 0x0f) << 2;
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_localout++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
	}
	/*
	 * Route packet.
	 */
	if (ro == 0) {
		ro = &iproute;
		bzero((caddr_t)ro, sizeof (*ro));
	}
	dst = (struct sockaddr_in *)&ro->ro_dst;
	/*
	 * If there is a cached route,
	 * check that it is to the same destination
	 * and is still up.  If not, free it and try again.
	 */
	ROUTE_WRITE_LOCK();
	if (ro->ro_rt && ((ro->ro_rt->rt_flags & RTF_UP) == 0 ||
	   dst->sin_addr.s_addr != ip->ip_dst.s_addr)) {
		RTFREE(ro->ro_rt);
		ro->ro_rt = (struct rtentry *)0;
	}
	if (ro->ro_rt == 0) {
		dst->sin_family = AF_INET;
		dst->sin_len = sizeof(*dst);
		dst->sin_addr = ip->ip_dst;
	}
	/*
	 * If routing to interface only,
	 * short circuit routing lookup.
	 */
	if (flags & IP_ROUTETOIF) {
		ROUTE_WRITE_UNLOCK();
		ia = (struct in_ifaddr *)ifa_ifwithdstaddr((struct sockaddr *)dst);
		if (ia == 0)
			ia = in_iaonnetof(in_netof(ip->ip_dst));
		if (ia == 0) {
			error = ENETUNREACH;
			goto bad;
		}
		ifp = ia->ia_ifp;
	} else {
		if (ro->ro_rt == 0)
			rtalloc(ro);
		if (ro->ro_rt == 0) {
			ROUTE_WRITE_UNLOCK();
			error = EHOSTUNREACH;
			goto bad;
		}
		ia = (struct in_ifaddr *)ro->ro_rt->rt_ifa;
		ifp = ro->ro_rt->rt_ifp;
		ro->ro_rt->rt_use++;
		if (ro->ro_rt->rt_flags & RTF_GATEWAY)
			dst = (struct sockaddr_in *)ro->ro_rt->rt_gateway;
		ROUTE_WRITE_UNLOCK();
	}
	/*
	 * If source address not specified yet, use address
	 * of outgoing interface.
	 */
	if (ip->ip_src.s_addr == INADDR_ANY) {
#include "mi.h"
#if defined(TNC) && NMI > 0
#include <net/if_types.h>
		/* Use real local i/f as src for outbound MI packets */
		if (ifp->if_type == IFT_MI) {
			extern struct in_ifaddr *ifa_real_with_af();
			register struct in_ifaddr *ia2;
			ia2 = ifa_real_with_af(AF_INET);
			ip->ip_src = ia2->ia_addr.sin_addr;
		} else
#endif
		ip->ip_src = IA_SIN(ia)->sin_addr;
	}
	/*
	 * Look for broadcast address and
	 * verify user is allowed to send
	 * such a packet.
	 */
	if (in_broadcast(dst->sin_addr)) {
		if ((ifp->if_flags & IFF_BROADCAST) == 0) {
			error = EADDRNOTAVAIL;
			goto bad;
		}
		if ((flags & IP_ALLOWBROADCAST) == 0) {
			error = EACCES;
			goto bad;
		}
		/* don't allow broadcast messages to be fragmented */
		if ((u_short)ip->ip_len > ifp->if_mtu) {
			error = EMSGSIZE;
			goto bad;
		}
		m->m_flags |= M_BCAST;
	}

	/*
	 * If small enough for interface, can just send directly.
	 */
	if ((u_short)ip->ip_len <= ifp->if_mtu) {
		ip->ip_len = htons((u_short)ip->ip_len);
		ip->ip_off = htons((u_short)ip->ip_off);
		ip->ip_sum = 0;
		ip->ip_sum = in_cksum(m, hlen);
		error = (*ifp->if_output)(ifp, m,
				(struct sockaddr *)dst, ro->ro_rt);
		goto done;
	}
	NETSTAT_LOCK(&ipstat.ips_lock);
	ipstat.ips_fragmented++;
	NETSTAT_UNLOCK(&ipstat.ips_lock);
	/*
	 * Too large for interface; fragment if possible.
	 * Must be able to put at least 8 bytes per fragment.
	 */
	if (ip->ip_off & IP_DF) {
		error = EMSGSIZE;
		goto bad;
	}
	len = (ifp->if_mtu - hlen) &~ 7;
	if (len < 8) {
		error = EMSGSIZE;
		goto bad;
	}

    {
	int mhlen, firstlen = len;
	struct mbuf **mnext = &m->m_nextpkt;

	/*
	 * Loop through length of segment after first fragment,
	 * make new header and copy data of each part and link onto chain.
	 */
	m0 = m;
	mhlen = sizeof (struct ip);
	for (off = hlen + len; off < (u_short)ip->ip_len; off += len) {
		MGETHDR(m, M_DONTWAIT, MT_HEADER);
		if (m == 0) {
			error = ENOBUFS;
			goto sendorfree;
		}
		m->m_data += max_linkhdr;
		mhip = mtod(m, struct ip *);
		*mhip = *ip;
		if (hlen > sizeof (struct ip)) {
			mhlen = ip_optcopy(ip, mhip) + sizeof (struct ip);
			mhip->ip_vhl = (IPVERSION << 4) | (mhlen >> 2);
		}
		m->m_len = mhlen;
		mhip->ip_off = ((off - hlen) >> 3) + (ip->ip_off & ~IP_MF);
		if (ip->ip_off & IP_MF)
			mhip->ip_off |= IP_MF;
		if (off + len >= (u_short)ip->ip_len)
			len = (u_short)ip->ip_len - off;
		else
			mhip->ip_off |= IP_MF;
		mhip->ip_len = htons((u_short)(len + mhlen));
		m->m_next = m_copym(m0, off, len, M_DONTWAIT);
		if (m->m_next == 0) {
			m_freem(m);
			error = ENOBUFS;	/* ??? */
			goto sendorfree;
		}
		m->m_pkthdr.len = mhlen + len;
		m->m_pkthdr.rcvif = (struct ifnet *)0;
		mhip->ip_off = htons((u_short)mhip->ip_off);
		mhip->ip_sum = 0;
		mhip->ip_sum = in_cksum(m, mhlen);
		*mnext = m;
		mnext = &m->m_nextpkt;
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_ofragments++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
	}
	/*
	 * Update first fragment by trimming what's been copied out
	 * and updating header, then send each fragment (in order).
	 */
	m = m0;
	m_adj(m, hlen + firstlen - (u_short)ip->ip_len);
	m->m_pkthdr.len = hlen + firstlen;
	ip->ip_len = htons((u_short)m->m_pkthdr.len);
	ip->ip_off = htons((u_short)(ip->ip_off | IP_MF));
	ip->ip_sum = 0;
	ip->ip_sum = in_cksum(m, hlen);
sendorfree:
	for (m = m0; m; m = m0) {
		m0 = m->m_nextpkt;
		m->m_nextpkt = 0;
		if (error == 0)
			error = (*ifp->if_output)(ifp, m,
			    (struct sockaddr *)dst, ro->ro_rt);
		else
			m_freem(m);
	}
    }
done:
	if (ro == &iproute && (flags & IP_ROUTETOIF) == 0 && ro->ro_rt)
		RTFREE(ro->ro_rt);
	return (error);
bad:
	m_freem(m0);
	goto done;
}

/*
 * Insert IP options into preformed packet.
 * Adjust IP destination as required for IP source routing,
 * as indicated by a non-zero in_addr at the start of the options.
 */
struct mbuf *
ip_insertoptions(m, opt, phlen)
	register struct mbuf *m;
	struct mbuf *opt;
	int *phlen;
{
	register struct ipoption *p = mtod(opt, struct ipoption *);
	struct mbuf *n;
	register struct ip *ip = mtod(m, struct ip *);
	unsigned optlen;

	optlen = opt->m_len - sizeof(p->ipopt_dst);
	if (optlen + (u_short)ip->ip_len > IP_MAXPACKET)
		return (m);		/* XXX should fail */
	if (p->ipopt_dst.s_addr)
		ip->ip_dst = p->ipopt_dst;
	if (m->m_flags & M_EXT || m->m_data - optlen < m->m_pktdat) {
		MGETHDR(n, M_DONTWAIT, MT_HEADER);
		if (n == 0)
			return (m);
		n->m_pkthdr.len = m->m_pkthdr.len + optlen;
		m->m_len -= sizeof(struct ip);
		m->m_data += sizeof(struct ip);
		n->m_next = m;
		m = n;
		m->m_len = optlen + sizeof(struct ip);
		m->m_data += max_linkhdr;
		bcopy((caddr_t)ip, mtod(m, caddr_t), sizeof(struct ip));
	} else {
		m->m_data -= optlen;
		m->m_len += optlen;
		m->m_pkthdr.len += optlen;
		ovbcopy((caddr_t)ip, mtod(m, caddr_t), sizeof(struct ip));
	}
	ip = mtod(m, struct ip *);
	bcopy((caddr_t)p->ipopt_list, (caddr_t)(ip + 1), (unsigned)optlen);
	*phlen = sizeof(struct ip) + optlen;
	ip->ip_len += optlen;
	return (m);
}

/*
 * Copy options from ip to jp,
 * omitting those not copied during fragmentation.
 */
ip_optcopy(ip, jp)
	struct ip *ip, *jp;
{
	register u_char *cp, *dp;
	int opt, optlen, cnt;

	cp = (u_char *)(ip + 1);
	dp = (u_char *)(jp + 1);
	cnt = ((ip->ip_vhl & 0x0f) << 2) - sizeof (struct ip);
	for (; cnt > 0; cnt -= optlen, cp += optlen) {
		opt = cp[0];
		if (opt == IPOPT_EOL)
			break;
		if (opt == IPOPT_NOP)
			optlen = 1;
		else
			optlen = cp[IPOPT_OLEN];
		/* bogus lengths should have been caught by ip_dooptions */
		if (optlen > cnt)
			optlen = cnt;
		if (IPOPT_COPIED(opt)) {
			bcopy((caddr_t)cp, (caddr_t)dp, (unsigned)optlen);
			dp += optlen;
		}
	}
	for (optlen = dp - (u_char *)(jp+1); optlen & 0x3; optlen++)
		*dp++ = IPOPT_EOL;
	return (optlen);
}

/*
 * IP socket option processing.
 */
ip_ctloutput(op, so, level, optname, mp)
	int op;
	struct socket *so;
	int level, optname;
	struct mbuf **mp;
{
	register struct inpcb *inp = sotoinpcb(so);
	register struct mbuf *m = *mp;
	register int optval;
	int error = 0;

	LOCK_ASSERT("ip_ctloutput", SOCKET_ISLOCKED(so));
	INPCB_LOCK(inp);

	if (level != IPPROTO_IP)
		error = ENOPROTOOPT;
	else switch (op) {

	case PRCO_SETOPT:
		switch (optname) {
		case IP_OPTIONS:
		case IP_RETOPTS:
			error = ip_pcbopts(&inp->inp_options, m);
			INPCB_UNLOCK(inp);
			return(error);

		case IP_TOS:
		case IP_TTL:
		case IP_RECVOPTS:
		case IP_RECVRETOPTS:
		case IP_RECVDSTADDR:
			if (m->m_len != sizeof(int))
				error = EINVAL;
			else {
				optval = *mtod(m, int *);
				switch (op) {

				case IP_TOS:
					inp->inp_ip.ip_tos = optval;
					break;

				case IP_TTL:
					inp->inp_ip.ip_ttl = optval;
					break;
#define	OPTSET(bit) \
	if (optval) \
		inp->inp_flags |= bit; \
	else \
		inp->inp_flags &= ~bit;

				case IP_RECVOPTS:
					OPTSET(INP_RECVOPTS);
					break;

				case IP_RECVRETOPTS:
					OPTSET(INP_RECVRETOPTS);
					break;

				case IP_RECVDSTADDR:
					OPTSET(INP_RECVDSTADDR);
					break;
				}
			}
			break;
#undef OPTSET

		default:
			error = EINVAL;
			break;
		}
		if (m)
			(void)m_free(m);
		break;

	case PRCO_GETOPT:
		switch (optname) {
		case IP_OPTIONS:
			*mp = m = m_get(M_WAIT, MT_SOOPTS);
			if (inp->inp_options) {
				m->m_len = inp->inp_options->m_len;
				bcopy(mtod(inp->inp_options, caddr_t),
				    mtod(m, caddr_t), (unsigned)m->m_len);
			} else
				m->m_len = 0;
			break;

		case IP_TOS:
		case IP_TTL:
		case IP_RECVOPTS:
		case IP_RECVRETOPTS:
		case IP_RECVDSTADDR:
			*mp = m = m_get(M_WAIT, MT_SOOPTS);
			m->m_len = sizeof(int);
			switch (op) {

			case IP_TOS:
				optval = inp->inp_ip.ip_tos;
				break;

			case IP_TTL:
				optval = inp->inp_ip.ip_ttl;
				break;

#define	OPTBIT(bit)	(inp->inp_flags & bit ? 1 : 0)

			case IP_RECVOPTS:
				optval = OPTBIT(INP_RECVOPTS);
				break;

			case IP_RECVRETOPTS:
				optval = OPTBIT(INP_RECVRETOPTS);
				break;

			case IP_RECVDSTADDR:
				optval = OPTBIT(INP_RECVDSTADDR);
				break;
			}
			*mtod(m, int *) = optval;
			break;

		default:
			error = EINVAL;
			break;
		}
		break;
	}
	INPCB_UNLOCK(inp);
	return (error);
}

/*
 * Set up IP options in pcb for insertion in output packets.
 * Store in mbuf with pointer in pcbopt, adding pseudo-option
 * with destination address if source routed.
 */
ip_pcbopts(pcbopt, m)
	struct mbuf **pcbopt;
	register struct mbuf *m;
{
	register cnt, optlen;
	register u_char *cp;
	u_char opt;

	/* turn off any old options */
	if (*pcbopt)
		(void)m_free(*pcbopt);
	*pcbopt = 0;
	if (m == (struct mbuf *)0 || m->m_len == 0) {
		/*
		 * Only turning off any previous options.
		 */
		if (m)
			(void)m_free(m);
		return (0);
	}

	/* This code used to allow it #if vax */
	if (m->m_len % sizeof(long))
		goto bad;

	/*
	 * IP first-hop destination address will be stored before
	 * actual options; move other options back
	 * and clear it when none present.
	 */
	if (m->m_data + m->m_len + sizeof(struct in_addr) >= &m->m_dat[MLEN])
		goto bad;
	cnt = m->m_len;
	m->m_len += sizeof(struct in_addr);
	cp = mtod(m, u_char *) + sizeof(struct in_addr);
	ovbcopy(mtod(m, caddr_t), (caddr_t)cp, (unsigned)cnt);
	bzero(mtod(m, caddr_t), sizeof(struct in_addr));

	for (; cnt > 0; cnt -= optlen, cp += optlen) {
		opt = cp[IPOPT_OPTVAL];
		if (opt == IPOPT_EOL)
			break;
		if (opt == IPOPT_NOP)
			optlen = 1;
		else {
			optlen = cp[IPOPT_OLEN];
			if (optlen <= IPOPT_OLEN || optlen > cnt)
				goto bad;
		}
		switch (opt) {

		default:
			break;

		case IPOPT_LSRR:
		case IPOPT_SSRR:
			/*
			 * user process specifies route as:
			 *	->A->B->C->D
			 * D must be our final destination (but we can't
			 * check that since we may not have connected yet).
			 * A is first hop destination, which doesn't appear in
			 * actual IP option, but is stored before the options.
			 */
			if (optlen < IPOPT_MINOFF - 1 + sizeof(struct in_addr))
				goto bad;
			m->m_len -= sizeof(struct in_addr);
			cnt -= sizeof(struct in_addr);
			optlen -= sizeof(struct in_addr);
			cp[IPOPT_OLEN] = optlen;
			/*
			 * Move first hop before start of options.
			 */
			bcopy((caddr_t)&cp[IPOPT_OFFSET+1], mtod(m, caddr_t),
			    sizeof(struct in_addr));
			/*
			 * Then copy rest of options back
			 * to close up the deleted entry.
			 */
			ovbcopy((caddr_t)(&cp[IPOPT_OFFSET+1] +
			    sizeof(struct in_addr)),
			    (caddr_t)&cp[IPOPT_OFFSET+1],
			    (unsigned)cnt + sizeof(struct in_addr));
			break;
		}
	}
	if (m->m_len > MAX_IPOPTLEN + sizeof(struct in_addr))
		goto bad;
	*pcbopt = m;
	return (0);

bad:
	(void)m_free(m);
	return (EINVAL);
}
