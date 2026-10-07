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
 * $Log: ip_input.c,v $
 * Revision 1.9  1995/02/01  21:35:01  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:35:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/09/01  01:35:39  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 2.7  93/08/20  18:57:10  mjl
 * [LCCbug #0355] In ipintr(), if incoming packet's destination is an MI
 * interface, forward the packet (it will go over MI to its real dest).
 * 
 * Revision 2.6  93/08/19  13:11:09  mjl
 * [LCCbug #0354] If a new ipforward_rt is allocated in ip_forward(), set
 * the register variable "rt" to point to the new route, or server crashes.
 * 
 * Revision 2.5  93/05/25  10:34:25  mjl
 * Check mbuf packet header to see if packet forwarding should be done in spite
 * of whether ipforwarding or ipgateway are set or not.
 * 
 * Revision 1.6  1993/07/16  15:05:09  cfj
 * Fixed merge error in ipintr().
 *
 * Revision 1.5  1993/05/06  20:26:45  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:53  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:06:42  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:04:14  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/21  18:06:00  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 2.4  93/01/12  13:51:32  mjl
 * Fix typo in debug printf.
 * 
 * Revision 1.1.2.1  1992/11/05  23:27:24  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  92/03/09  14:42:43  durriya
 * 	91/12/18  17:16:49  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.2  91/08/31  13:43:42  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:36:06  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.4.2  91/02/19  15:24:40  tmt
 * 	Fix timing hole in freeing overlapped fragment.
 * 	[91/02/16  15:51:54  tmt]
 * 
 * Revision 1.13  90/10/07  14:35:03  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:15:10  gm]
 * 
 * 	Include port.h in MACH_NET code, for PORT_NULL.
 * 	[90/09/27  08:34:01  tmt]
 * 
 * Revision 1.12  90/09/23  15:55:54  devrcs
 * 	Fix ip_forward, icmp_error call arguments.
 * 	[90/09/05  17:30:54  tmt]
 * 
 * Revision 1.11  90/08/24  12:14:38  devrcs
 * 	Add routing netmask subroutine (see net/route.c).
 * 	[90/08/17  18:08:45  tmt]
 * 
 * Revision 1.10  90/07/27  09:01:20  devrcs
 * 	Update to BSD Reno release.
 * 	Move rtinithead() to ip_init().
 * 	Remove or rearrange route locks - route.c handles.
 * 	[90/07/19  17:33:35  tmt]
 * 
 * Revision 1.9  90/07/05  23:13:18  devrcs
 * 	Remove unneeded spl's.
 * 	[90/07/03  18:52:53  tmt]
 * 
 * Revision 1.8  90/06/22  20:39:09  devrcs
 * 	Add "ipsrcroute" variable to DISABLE source routing.
 * 	[90/06/11  11:21:17  tmt]
 * 
 * Revision 1.7  90/04/27  19:16:10  devrcs
 * 	Change code to handle new _NO_BITFIELDS ip header. Move data
 * 	declarations here. Put MACH_NET support back in - untested.
 * 	[90/04/20  12:56:27  tmt]
 * 
 * Revision 1.6  90/04/14  00:33:15  devrcs
 * 	Add netisr_add() call for dynamic netisr install. Add void's.
 * 	Do in_cksum inline for (hopefully always) better speed.
 * 	Check version of incoming packets!
 * 	[90/04/09  16:32:06  tmt]
 * 
 * Revision 1.4  90/01/18  08:47:09  gm
 * 	Apply the idea from one of Van Jabobson's speedups to ip frag
 * 	reassembly, i.e. do pullup only if queuing a fragment. It may
 * 	be worth fixing it completely in future (struct mbuf *next, *prev).
 * 	Rearrange M_WCARD set/clear for efficiency.
 * 	[90/01/08  16:07:32  tmt]
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
 * Revision 1.3  90/01/03  12:41:43  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:58  gm]
 * 
 * Revision 1.2  89/12/26  10:12:51  gm
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
 *	Base:	ip_input.c	7.14 (Berkeley) 9/20/89
 *	Merged:	ip_input.c	7.16 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"
#if	MACH
#include <mach_net.h>
#endif

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"

#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/domain.h"
#include "sys/protosw.h"

#include "net/if.h"
#include "net/route.h"
#include "net/netisr.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/in_pcb.h"
#include "netinet/in_var.h"
#include "netinet/ip_var.h"
#include "netinet/ip_icmp.h"
#include "netinet/icmp_var.h"

#include "net/net_malloc.h"

LOCK_ASSERTL_DECL

int	ipgateway = IPGATEWAY;			/* Are configured as gateway */
int	ipforwarding = IPFORWARDING;		/* Are acting as gateway */
extern	int in_interfaces;			/* Have multiple interfaces */
int	ipsendredirects = IPSENDREDIRECTS;
int	ipdirected_broadcast = IPDIRECTED_BROADCAST;
int	ipsrcroute = 1;		/* Dangerous security hole, but popular option*/

u_char	ip_protox[IPPROTO_MAX];
int	ipqmaxlen = IFQ_MAXLEN;
struct	in_ifaddr *in_ifaddr;			/* first inet address */
struct	ifqueue	ipintrq;			/* ip packet input queue */

struct	ipstat	ipstat;
struct	ipq	ipq;				/* ip reass. queue */
u_short	ip_id;					/* ip packet ctr, for ids */

#if	NETSYNC_LOCK
lock_data_t		ip_frag_lock;
simple_lock_data_t	ip_misc_lock;
simple_lock_data_t	inifaddr_lock;
#endif

/*
 * We need to save the IP options in case a protocol wants to respond
 * to an incoming packet over the same route if the packet got here
 * using IP source routing.  This allows connection establishment and
 * maintenance when the remote end is on a network that is not known
 * to us.
#if	NETISR_THREAD
 * Fix me fix me fix me. ip_stripoptions() is structured to do what
 * we need: extract the options if desired and so they can be passed
 * back down. The global save area is not workable.
#endif
 */
static int ip_nhops;
static struct ip_srcrt ip_srcrt;

/* Gateway */
extern	int if_index;
u_long	*ip_ifmatrix;

static void
setiproutemask(dst, mask)
	struct sockaddr_in *dst, *mask;
{
	in_sockmaskof(dst->sin_addr, mask);
}

/*
 * IP initialization: fill in IP protocol switch table.
 * All protocols not implemented in kernel go to raw IP protocol handler.
 */
void
ip_init()
{
	register struct protosw *pr;
	register int i;

	pr = pffindproto(PF_INET, IPPROTO_RAW, SOCK_RAW);
	if (pr == 0)
		panic("ip_init");
	for (i = 0; i < IPPROTO_MAX; i++)
		ip_protox[i] = pr - inetsw;
	for (pr = inetdomain.dom_protosw;
	    pr < inetdomain.dom_protoswNPROTOSW; pr++)
		if (pr->pr_domain->dom_family == PF_INET &&
		    pr->pr_protocol && pr->pr_protocol != IPPROTO_RAW)
			ip_protox[pr->pr_protocol] = pr - inetsw;
	ipq.next = ipq.prev = &ipq;
	ip_id = ntohl(iptime());
	in_ifaddr = NULL;
	in_interfaces = 0;
	INIFADDR_LOCKINIT();
	IFQ_LOCKINIT(&ipintrq);
	ipintrq.ifq_maxlen = ipqmaxlen;
	IPFRAG_LOCKINIT();
	IPMISC_LOCKINIT();
	NETSTAT_LOCKINIT(&ipstat.ips_lock);
	NETSTAT_LOCKINIT(&icmpstat.icps_lock);
/* Problems w/dynamic and ||ization! */
	/* Init ip_ifmatrix in case we become a gateway */
	i = (if_index + 1) * (if_index + 1) * sizeof (u_long);
	NET_MALLOC(ip_ifmatrix, u_long *, i, M_RTABLE, M_WAITOK);
	if (ip_ifmatrix = 0)
		panic("no memory for ip_ifmatrix");
#if	MACH_NET
	mach_net_ipinit();
#endif

	rtinithead(AF_INET, 32, setiproutemask);
	(void) netisr_add(NETISR_IP, ipintr, &ipintrq, &inetdomain);

	/* For now init ARP here... XXX */
	arpinit();
}

#if	!NETISR_THREAD
struct	sockaddr_in ipaddr = { sizeof(ipaddr), AF_INET };
#else
/* Following struct protected by route lock, sort of. */
#endif
struct	route ipforward_rt;

/*
 * Ip input routine.  Checksum and byte swap header.  If fragmented
 * try to reassemble.  Process options.  Pass to next level.
 */
void
ipintr()
{
	register struct ip *ip;
	register struct mbuf *m;
	register struct ipq *fp;
	register struct in_ifaddr *ia;
	register u_long sum;
	int hlen, s;

next:
	/*
	 * Get next datagram off input queue and get IP header
	 * in first mbuf.
	 */
	s = splimp();
	IF_DEQUEUE(&ipintrq, m);
	splx(s);
	if (m == 0)
		return;
if ((m->m_flags & M_PKTHDR) == 0)
panic("ipintr no HDR");
	/*
	 * If no IP addresses have been set yet but the interfaces
	 * are receiving, can't do anything with incoming packets yet.
	 */
	if (in_ifaddr == NULL)
		goto bad;
	NETSTAT_LOCK(&ipstat.ips_lock);
	ipstat.ips_total++;
	NETSTAT_UNLOCK(&ipstat.ips_lock);
	if (m->m_len < sizeof (struct ip) &&
	    (m = m_pullup(m, sizeof (struct ip))) == 0) {
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_toosmall++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		goto next;
	}
	ip = mtod(m, struct ip *);
	hlen = (ip->ip_vhl & 0x0f) << 2;
	if (hlen < sizeof(struct ip) ||	/* minimum header length */
	    (ip->ip_vhl & 0xf0) != IPVERSION << 4) {	/* wrong stat! */
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_badhlen++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		goto bad;
	}
	if (hlen > m->m_len) {
		if ((m = m_pullup(m, hlen)) == 0) {
			NETSTAT_LOCK(&ipstat.ips_lock);
			ipstat.ips_badhlen++;
			NETSTAT_UNLOCK(&ipstat.ips_lock);
			goto next;
		}
		ip = mtod(m, struct ip *);
	}

#if	0
	if (ip->ip_sum = in_cksum(m, hlen)) {
#else
	/* Inline in_cksum over IP header. */
#define	ADDSUM(n)	case n:	sum +=	*(u_short *)((caddr_t)ip + n - 2) + \
					*(u_short *)((caddr_t)ip + n - 4)
	sum = 0;
	switch (hlen) {
		ADDSUM(60); ADDSUM(56); ADDSUM(52); ADDSUM(48); ADDSUM(44);
		ADDSUM(40); ADDSUM(36); ADDSUM(32); ADDSUM(28); ADDSUM(24);
		ADDSUM(20); ADDSUM(16); ADDSUM(12); ADDSUM( 8); ADDSUM( 4);
	}
	sum = (sum & 0xffff) + (sum >> 16);
	sum += sum >> 16;
	if (~sum & 0xffff) {
#endif
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_badsum++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		goto bad;
	}

	/*
	 * Convert fields to host representation.
	 */
	ip->ip_len = ntohs((u_short)ip->ip_len);
	if (ip->ip_len < hlen) {
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_badlen++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		goto bad;
	}
	ip->ip_id = ntohs(ip->ip_id);
	ip->ip_off = ntohs((u_short)ip->ip_off);

	/*
	 * Check that the amount of data in the buffers
	 * is as at least much as the IP header would have us expect.
	 * Trim mbufs if longer than we expect.
	 * Drop packet if shorter than we expect.
	 */
	if (m->m_pkthdr.len < ip->ip_len) {
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_tooshort++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		goto bad;
	}
	if (m->m_pkthdr.len > ip->ip_len) {
		if (m->m_len == m->m_pkthdr.len) {
			m->m_len = ip->ip_len;
			m->m_pkthdr.len = ip->ip_len;
		} else
			m_adj(m, ip->ip_len - m->m_pkthdr.len);
	}

	/*
	 * Process options and, if not destined for us,
	 * ship it on.  ip_dooptions returns 1 when an
	 * error was detected (causing an icmp message
	 * to be sent and the original packet to be freed).
	 */
	ip_nhops = 0;		/* for source routed packets */
	if (hlen > sizeof (struct ip) && ip_dooptions(m))
		goto next;

	/*
	 * Check our list of addresses, to see if the packet is for us.
	 */
	m->m_flags |= M_WCARD;	/* Accepting a broadcast? */
	for (ia = in_ifaddr; ia; ia = ia->ia_next) {
#define	satosin(sa)	((struct sockaddr_in *)(sa))

		if (IA_SIN(ia)->sin_addr.s_addr == ip->ip_dst.s_addr) {
			m->m_flags &= ~M_WCARD;	/* A direct address */
#include "mi.h"
#if defined(TNC) && NMI > 0
			{
			  extern void mi_rtrequest();
			  if (ia->ia_ifa.ifa_rtrequest == mi_rtrequest) {
				  /* Forward MI reply packets. */
				  m->m_flags |= M_TNC_FORW;
				  ip_forward(m, 0);
				  goto next;
			  }
		  	}
#endif
			goto ours;
		}
		if ((!ipdirected_broadcast || ia->ia_ifp==m->m_pkthdr.rcvif) &&
		    (ia->ia_ifp->if_flags & IFF_BROADCAST)) {
			u_long t;

			if (satosin(&ia->ia_broadaddr)->sin_addr.s_addr ==
			    ip->ip_dst.s_addr)
				goto ours;
			if (ip->ip_dst.s_addr == ia->ia_netbroadcast.s_addr)
				goto ours;
			/*
			 * Look for all-0's host part (old broadcast addr),
			 * either for subnet or net.
			 */
			t = ntohl(ip->ip_dst.s_addr);
			if (t == ia->ia_subnet)
				goto ours;
			if (t == ia->ia_net)
				goto ours;
		}
	}
	if (ip->ip_dst.s_addr == (u_long)INADDR_BROADCAST)
		goto ours;
	if (ip->ip_dst.s_addr == INADDR_ANY)
		goto ours;

	/*
	 * Not for us; forward if possible and desirable.
	 */
	if ( (ipforwarding == 0 || ipgateway == 0) && ! M_TNC_FORWARD(m) ) {
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_cantforward++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		m_freem(m);
	} else
		ip_forward(m, 0);
	goto next;

ours:
	/*
	 * If offset or IP_MF are set, must reassemble.
	 * Otherwise, nothing need be done.
	 * (We could look in the reassembly queue to see
	 * if the packet was previously fragmented,
	 * but it's not worth the time; just let them time out.)
	 */
	if (ip->ip_off &~ IP_DF) {
		/*
		 * Look for queue of fragments
		 * of this datagram.
		 */
		IPFRAG_LOCK();
		for (fp = ipq.next; fp != &ipq; fp = fp->next)
			if (ip->ip_id == fp->ipq_id &&
			    ip->ip_src.s_addr == fp->ipq_src.s_addr &&
			    ip->ip_dst.s_addr == fp->ipq_dst.s_addr &&
			    ip->ip_p == fp->ipq_p)
				goto found;
		fp = 0;
found:
		/*
		 * Adjust ip_len to not reflect header,
		 * set ip_mff if more fragments are expected,
		 * convert offset of this to bytes.
		 */
		ip->ip_len -= hlen;
		((struct ipasfrag *)ip)->ipf_mff = 0;
		if (ip->ip_off & IP_MF)
			((struct ipasfrag *)ip)->ipf_mff = 1;
		ip->ip_off <<= 3;

		/*
		 * If datagram marked as having more fragments
		 * or if this is not the first fragment,
		 * attempt reassembly; if it succeeds, proceed.
		 */
		if (((struct ipasfrag *)ip)->ipf_mff || ip->ip_off) {
			NETSTAT_LOCK(&ipstat.ips_lock);
			ipstat.ips_fragments++;
			NETSTAT_UNLOCK(&ipstat.ips_lock);
			m = ip_reass(m, fp);
			IPFRAG_UNLOCK();
			if (m == 0)
				goto next;
			else {
				NETSTAT_LOCK(&ipstat.ips_lock);
				ipstat.ips_reassembled++;
				NETSTAT_UNLOCK(&ipstat.ips_lock);
			}
			ip = mtod(m, struct ip *);
		} else {
			if (fp)
				ip_freef(fp);
			IPFRAG_UNLOCK();
		}
	} else
		ip->ip_len -= hlen;

	/*
	 * Switch out to protocol's input routine.
	 */
	NETSTAT_LOCK(&ipstat.ips_lock);
	ipstat.ips_delivered++;
	NETSTAT_UNLOCK(&ipstat.ips_lock);
#if	MACH_NET
	m = mach_net_ipreceive(m, hlen);
	if (m == NULL)
		goto next;
#endif
	(*inetsw[ip_protox[ip->ip_p]].pr_input)(m, hlen);
	goto next;
bad:
	m_freem(m);
	goto next;
}

/*
 * Take incoming datagram fragment and try to
 * reassemble it into whole datagram.  If a chain for
 * reassembly of this datagram already exists, then it
 * is given as fp; otherwise have to make a chain.
 */
struct mbuf *
ip_reass(m, fp)
	register struct mbuf *m;
	register struct ipq *fp;
{
	register struct ipasfrag *ip, *q;
	struct mbuf *t;
	int hlen, i, next;

	LOCK_ASSERT("ip_reass", lock_islocked(&ip_frag_lock));

	/* Need to perform dtom() on fragq. Real fix later? */
	if (m->m_flags & M_EXT) {
		m = m_pullup(m, sizeof (struct ip));
		if (m == NULL)
			goto dropfrag1;
	}
	ip = mtod(m, struct ipasfrag *);
	hlen = (ip->ip_vhl & 0x0f) << 2;

	/*
	 * Presence of header sizes in mbufs
	 * would confuse code below.
	 */
	m->m_data += hlen;
	m->m_len -= hlen;

	/*
	 * If first fragment to arrive, create a reassembly queue.
	 */
	if (fp == 0) {
		if ((t = m_get(M_DONTWAIT, MT_FTABLE)) == NULL)
			goto dropfrag;
		fp = mtod(t, struct ipq *);
		fp->ipq_ttl = IPFRAGTTL;
		fp->ipq_p = ip->ip_p;
		fp->ipq_id = ip->ip_id;
		fp->ipq_next = fp->ipq_prev = (struct ipasfrag *)fp;
		fp->ipq_src = ((struct ip *)ip)->ip_src;
		fp->ipq_dst = ((struct ip *)ip)->ip_dst;
		q = (struct ipasfrag *)fp;
		insque(fp, &ipq);
		goto insert;
	}

	/*
	 * Find a segment which begins after this one does.
	 */
	for (q = fp->ipq_next; q != (struct ipasfrag *)fp; q = q->ipf_next)
		if (q->ip_off > ip->ip_off)
			break;

	/*
	 * If there is a preceding segment, it may provide some of
	 * our data already.  If so, drop the data from the incoming
	 * segment.  If it provides all of our data, drop us.
	 */
	if (q->ipf_prev != (struct ipasfrag *)fp) {
		i = q->ipf_prev->ip_off + q->ipf_prev->ip_len - ip->ip_off;
		if (i > 0) {
			if (i >= ip->ip_len)
				goto dropfrag;
			m_adj(m, i);
			ip->ip_off += i;
			ip->ip_len -= i;
		}
	}

	/*
	 * While we overlap succeeding segments trim them or,
	 * if they are completely covered, dequeue them.
	 */
	while (q != (struct ipasfrag *)fp && ip->ip_off + ip->ip_len > q->ip_off) {
		i = (ip->ip_off + ip->ip_len) - q->ip_off;
		if (i < q->ip_len) {
			q->ip_len -= i;
			q->ip_off += i;
			m_adj(dtom(q), i);
			break;
		}
		{
		struct ipasfrag *p = q;
		q = q->ipf_next;
		ip_deq(p);
		m_freem(dtom(p));
		}
	}

insert:
	/*
	 * Stick new segment in its place;
	 * check for complete reassembly.
	 */
	ip_enq(ip, q->ipf_prev);
	next = 0;
	for (q = fp->ipq_next; q != (struct ipasfrag *)fp; q = q->ipf_next) {
		if (q->ip_off != next)
			return (0);
		next += q->ip_len;
	}
	if (q->ipf_prev->ipf_mff)
		return (0);

	/*
	 * Reassembly is complete; concatenate fragments.
	 */
	q = fp->ipq_next;
	m = dtom(q);
	t = m->m_next;
	m->m_next = 0;
	m_cat(m, t);
	q = q->ipf_next;
	while (q != (struct ipasfrag *)fp) {
		t = dtom(q);
		q = q->ipf_next;
		m_cat(m, t);
	}

	/*
	 * Create header for new ip packet by
	 * modifying header of first packet;
	 * dequeue and discard fragment reassembly header.
	 * Make header visible.
	 */
	ip = fp->ipq_next;
	ip->ip_len = next;
	((struct ip *)ip)->ip_src = fp->ipq_src;
	((struct ip *)ip)->ip_dst = fp->ipq_dst;
	remque(fp);
	(void) m_free(dtom(fp));
	m = dtom(ip);
	hlen = ((ip->ip_vhl & 0x0f) << 2);
	m->m_len += hlen;
	m->m_data -= hlen;
	return m;

dropfrag:
	m_freem(m);
dropfrag1:
	NETSTAT_LOCK(&ipstat.ips_lock);
	ipstat.ips_fragdropped++;
	NETSTAT_UNLOCK(&ipstat.ips_lock);
	return (0);
}

/*
 * Free a fragment reassembly header and all
 * associated datagrams.
 */
void
ip_freef(fp)
	struct ipq *fp;
{
	register struct ipasfrag *q, *p;

	LOCK_ASSERT("ip_freef", lock_islocked(&ip_frag_lock));
	for (q = fp->ipq_next; q != (struct ipasfrag *)fp; q = p) {
		p = q->ipf_next;
		ip_deq(q);
		m_freem(dtom(q));
	}
	remque(fp);
	(void) m_free(dtom(fp));
}

/*
 * Put an ip fragment on a reassembly chain.
 * Like insque, but pointers in middle of structure.
 */
void
ip_enq(p, prev)
	register struct ipasfrag *p, *prev;
{

	p->ipf_prev = prev;
	p->ipf_next = prev->ipf_next;
	prev->ipf_next->ipf_prev = p;
	prev->ipf_next = p;
}

/*
 * To ip_enq as remque is to insque.
 */
void
ip_deq(p)
	register struct ipasfrag *p;
{

	p->ipf_prev->ipf_next = p->ipf_next;
	p->ipf_next->ipf_prev = p->ipf_prev;
}

/*
 * IP timer processing;
 * if a timer expires on a reassembly
 * queue, discard it.
 */
void
ip_slowtimo()
{
	register struct ipq *fp;

	IPFRAG_LOCK();
	fp = ipq.next;
	if (fp) while (fp != &ipq) {
		--fp->ipq_ttl;
		fp = fp->next;
		if (fp->prev->ipq_ttl == 0) {
			NETSTAT_LOCK(&ipstat.ips_lock);
			ipstat.ips_fragtimeout++;
			NETSTAT_UNLOCK(&ipstat.ips_lock);
			ip_freef(fp->prev);
		}
	}
	IPFRAG_UNLOCK();
}

/*
 * Drain off all datagram fragments.
 */
void
ip_drain()
{

	IPFRAG_LOCK();
	while (ipq.next != &ipq) {
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_fragdropped++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		ip_freef(ipq.next);
	}
	IPFRAG_UNLOCK();
}

/*
 * Do option processing on a datagram,
 * possibly discarding it if bad options are encountered,
 * or forwarding it if source-routed.
 * Returns 1 if packet has been forwarded/freed,
 * 0 if the packet should be processed further.
 */
ip_dooptions(m)
	struct mbuf *m;
{
	register struct ip *ip = mtod(m, struct ip *);
	register u_char *cp;
	register struct ip_timestamp *ipt;
	register struct in_ifaddr *ia;
	int opt, optlen, cnt, off, code, type = ICMP_PARAMPROB, forward = 0;
	struct in_addr *sin;
	n_time ntime;
#if	NETISR_THREAD
	extern CONST struct sockaddr_in in_zeroaddr;
	struct sockaddr_in ipaddr;

	ipaddr = in_zeroaddr;
#endif

	cp = (u_char *)(ip + 1);
	cnt = ((ip->ip_vhl & 0x0f) << 2) - sizeof (struct ip);
	for (; cnt > 0; cnt -= optlen, cp += optlen) {
		opt = cp[IPOPT_OPTVAL];
		if (opt == IPOPT_EOL)
			break;
		if (opt == IPOPT_NOP)
			optlen = 1;
		else {
			optlen = cp[IPOPT_OLEN];
			if (optlen <= 0 || optlen > cnt) {
				code = &cp[IPOPT_OLEN] - (u_char *)ip;
				goto bad;
			}
		}
		switch (opt) {

		default:
			break;

		/*
		 * Source routing with record.
		 * Find interface with current destination address.
		 * If none on this machine then drop if strictly routed,
		 * or do nothing if loosely routed.
		 * Record interface address and bring up next address
		 * component.  If strictly routed make sure next
		 * address is on directly accessible net.
		 */
		case IPOPT_LSRR:
		case IPOPT_SSRR:
			if ((off = cp[IPOPT_OFFSET]) < IPOPT_MINOFF) {
				code = &cp[IPOPT_OFFSET] - (u_char *)ip;
				goto bad;
			}
			ipaddr.sin_addr = ip->ip_dst;
			ia = (struct in_ifaddr *)
				ifa_ifwithaddr((struct sockaddr *)&ipaddr);
			if (ia == 0) {
				if (opt == IPOPT_SSRR) {
					type = ICMP_UNREACH;
					code = ICMP_UNREACH_SRCFAIL;
					goto bad;
				}
				/*
				 * Loose routing, and not at next destination
				 * yet; nothing to do except forward.
				 */
				break;
			}
			off--;			/* 0 origin */
			if (off > optlen - sizeof(struct in_addr)) {
				/*
				 * End of source route.  Should be for us.
				 */
				save_rte(cp, ip->ip_src);
				break;
			}
			/*
			 * locate outgoing interface
			 */
			bcopy((caddr_t)(cp + off), (caddr_t)&ipaddr.sin_addr,
			    sizeof(ipaddr.sin_addr));
			if (opt == IPOPT_SSRR) {
#define	INA	struct in_ifaddr *
#define	SA	struct sockaddr *
				if ((ia = (INA)ifa_ifwithdstaddr((SA)&ipaddr)) == 0)
			    		ia = in_iaonnetof(in_netof(ipaddr.sin_addr));
			} else
				ia = ip_rtaddr(ipaddr.sin_addr);
			if (ia == 0) {
				type = ICMP_UNREACH;
				code = ICMP_UNREACH_SRCFAIL;
				goto bad;
			}
			ip->ip_dst = ipaddr.sin_addr;
			bcopy((caddr_t)&(IA_SIN(ia)->sin_addr),
			    (caddr_t)(cp + off), sizeof(struct in_addr));
			cp[IPOPT_OFFSET] += sizeof(struct in_addr);
			forward = 1;
			break;

		case IPOPT_RR:
			if ((off = cp[IPOPT_OFFSET]) < IPOPT_MINOFF) {
				code = &cp[IPOPT_OFFSET] - (u_char *)ip;
				goto bad;
			}
			/*
			 * If no space remains, ignore.
			 */
			off--;			/* 0 origin */
			if (off > optlen - sizeof(struct in_addr))
				break;
			bcopy((caddr_t)(&ip->ip_dst), (caddr_t)&ipaddr.sin_addr,
			    sizeof(ipaddr.sin_addr));
			/*
			 * locate outgoing interface; if we're the destination,
			 * use the incoming interface (should be same).
			 */
			if ((ia = (INA)ifa_ifwithaddr((SA)&ipaddr)) == 0 &&
			    (ia = ip_rtaddr(ipaddr.sin_addr)) == 0) {
				type = ICMP_UNREACH;
				code = ICMP_UNREACH_HOST;
				goto bad;
			}
			bcopy((caddr_t)&(IA_SIN(ia)->sin_addr),
			    (caddr_t)(cp + off), sizeof(struct in_addr));
			cp[IPOPT_OFFSET] += sizeof(struct in_addr);
			break;

		case IPOPT_TS:
			code = cp - (u_char *)ip;
			ipt = (struct ip_timestamp *)cp;
			if (ipt->ipt_len < 5)
				goto bad;
			if (ipt->ipt_ptr > ipt->ipt_len - sizeof (long)) {
				u_char oflw = (ipt->ipt_oflg & 0xf0) + 0x10;
				ipt->ipt_oflg &= ~0xf0;
				ipt->ipt_oflg |= oflw;
				if (oflw == 0)
					goto bad;
				break;
			}
			sin = (struct in_addr *)(cp + ipt->ipt_ptr - 1);
			switch (ipt->ipt_oflg & 0x0f) {

			case IPOPT_TS_TSONLY:
				break;

			case IPOPT_TS_TSANDADDR:
				if (ipt->ipt_ptr + sizeof(n_time) +
				    sizeof(struct in_addr) > ipt->ipt_len)
					goto bad;
				ia = ifptoia(m->m_pkthdr.rcvif);
				bcopy((caddr_t)&IA_SIN(ia)->sin_addr,
				    (caddr_t)sin, sizeof(struct in_addr));
				ipt->ipt_ptr += sizeof(struct in_addr);
				break;

			case IPOPT_TS_PRESPEC:
				if (ipt->ipt_ptr + sizeof(n_time) +
				    sizeof(struct in_addr) > ipt->ipt_len)
					goto bad;
				bcopy((caddr_t)sin, (caddr_t)&ipaddr.sin_addr,
				    sizeof(struct in_addr));
				if (ifa_ifwithaddr((SA)&ipaddr) == 0)
					continue;
				ipt->ipt_ptr += sizeof(struct in_addr);
				break;

			default:
				goto bad;
			}
			ntime = iptime();
			bcopy((caddr_t)&ntime, (caddr_t)cp + ipt->ipt_ptr - 1,
			    sizeof(n_time));
			ipt->ipt_ptr += sizeof(n_time);
		}
	}
	if (forward) {
		ip_forward(m, 1);
		return (1);
	} else
		return (0);
bad:
	icmp_error(m, type, code, zeroin_addr);
	return (1);
}

/*
 * Given address of next destination (final or next hop),
 * return internet address info of interface to be used to get there.
 */
struct in_ifaddr *
ip_rtaddr(dst)
	 struct in_addr dst;
{
	register struct sockaddr_in *sin;
	ROUTE_LOCK_DECL()

	sin = (struct sockaddr_in *) &ipforward_rt.ro_dst;

	ROUTE_WRITE_LOCK();
	if (ipforward_rt.ro_rt == 0 || dst.s_addr != sin->sin_addr.s_addr) {
		if (ipforward_rt.ro_rt) {
			RTFREE(ipforward_rt.ro_rt);
			ipforward_rt.ro_rt = 0;
		}
		sin->sin_family = AF_INET;
		sin->sin_len = sizeof(*sin);
		sin->sin_addr = dst;

		rtalloc(&ipforward_rt);
	}
	ROUTE_WRITE_UNLOCK();
	if (ipforward_rt.ro_rt == 0)
		return ((struct in_ifaddr *)0);
	return ((struct in_ifaddr *) ipforward_rt.ro_rt->rt_ifa);
}

/*
 * Save incoming source route for use in replies,
 * to be picked up later by ip_srcroute if the receiver is interested.
 */
void
save_rte(option, dst)
	u_char *option;
	struct in_addr dst;
{
	unsigned olen;

	if (!ipsrcroute)
		return;
	olen = option[IPOPT_OLEN];
#if	INETPRINTFS
	if (inetprintfs > 1)
		printf("save_rte: olen %d\n", olen);
#endif
	if (olen > sizeof(ip_srcrt) - (1 + sizeof(dst)))
		return;
	bcopy((caddr_t)option, (caddr_t)ip_srcrt.srcopt, olen);
	ip_nhops = (olen - IPOPT_OFFSET - 1) / sizeof(struct in_addr);
	ip_srcrt.dst = dst;
}

/*
 * Retrieve incoming source route for use in replies,
 * in the same form used by setsockopt.
 * The first hop is placed before the options, will be removed later.
 */
struct mbuf *
ip_srcroute()
{
	register struct in_addr *p, *q;
	register struct mbuf *m = NULL;

	if (ip_nhops == 0)
		goto out;

	m = m_get(M_DONTWAIT, MT_SOOPTS);
	if (m == 0)
		goto out;

#define OPTSIZ	(sizeof(ip_srcrt.nop) + sizeof(ip_srcrt.srcopt))

	/* length is (nhops+1)*sizeof(addr) + sizeof(nop + srcrt header) */
	m->m_len = ip_nhops * sizeof(struct in_addr) + sizeof(struct in_addr) +
	    OPTSIZ;
#if	INETPRINTFS
	if (inetprintfs > 1)
		printf("ip_srcroute: nhops %d mlen %d", ip_nhops, m->m_len);
#endif

	/*
	 * First save first hop for return route
	 */
	p = &ip_srcrt.route[ip_nhops - 1];
	*(mtod(m, struct in_addr *)) = *p--;
#if	INETPRINTFS
	if (inetprintfs > 1)
		printf(" hops %X", ntohl(*mtod(m, struct in_addr *)));
#endif

	/*
	 * Copy option fields and padding (nop) to mbuf.
	 */
	ip_srcrt.nop = IPOPT_NOP;
	ip_srcrt.srcopt[IPOPT_OFFSET] = IPOPT_MINOFF;
	bcopy((caddr_t)&ip_srcrt.nop,
	    mtod(m, caddr_t) + sizeof(struct in_addr), OPTSIZ);
	q = (struct in_addr *)(mtod(m, caddr_t) +
	    sizeof(struct in_addr) + OPTSIZ);
#undef OPTSIZ
	/*
	 * Record return path as an IP source route,
	 * reversing the path (pointers are now aligned).
	 */
	while (p >= ip_srcrt.route) {
#if	INETPRINTFS
		if (inetprintfs > 1)
			printf(" %X", ntohl(*q));
#endif
		*q++ = *p--;
	}
	/*
	 * Last hop goes to final destination.
	 */
	*q = ip_srcrt.dst;
#if	INETPRINTFS
	if (inetprintfs > 1)
		printf(" %X\n", ntohl(*q));
#endif
out:
	return (m);
}

/*
 * Strip out IP options, at higher
 * level protocol in the kernel.
 * Second argument is buffer to which options
 * will be moved, and return value is their length.
 * XXX should be deleted; last arg currently ignored.
 */
void
ip_stripoptions(m, mopt)
	register struct mbuf *m;
	struct mbuf *mopt;
{
	register int i;
	struct ip *ip = mtod(m, struct ip *);
	register caddr_t opts;
	int olen;

	olen = ((ip->ip_vhl & 0x0f) << 2) - sizeof (struct ip);
	opts = (caddr_t)(ip + 1);
	i = m->m_len - (sizeof (struct ip) + olen);
	bcopy(opts  + olen, opts, (unsigned)i);
	m->m_len -= olen;
	if (m->m_flags & M_PKTHDR)
		m->m_pkthdr.len -= olen;
	ip->ip_vhl = (IPVERSION << 4) | sizeof(struct ip) >> 2;
}

CONST u_char inetctlerrmap[PRC_NCMDS] = {
	0,		0,		0,		0,
	0,		EMSGSIZE,	EHOSTDOWN,	EHOSTUNREACH,
	EHOSTUNREACH,	EHOSTUNREACH,	ECONNREFUSED,	ECONNREFUSED,
	EMSGSIZE,	EHOSTUNREACH,	0,		0,
	0,		0,		0,		0,
	ENOPROTOOPT
};

/*
 * Forward a packet.  If some error occurs return the sender
 * an icmp packet.  Note we can't always generate a meaningful
 * icmp message because icmp doesn't have a large enough repertoire
 * of codes and types.
 *
 * If not forwarding, just drop the packet.  This could be confusing
 * if ipforwarding was zero but some routing protocol was advancing
 * us as a gateway to somewhere.  However, we must let the routing
 * protocol deal with that.
 *
 * The srcrt parameter indicates whether the packet is being forwarded
 * via a source route.
 */
void
ip_forward(m, srcrt)
	struct mbuf *m;
	int srcrt;
{
	register struct ip *ip = mtod(m, struct ip *);
	register struct sockaddr_in *sin;
	register struct rtentry *rt;
	int error, type = 0, code;
	struct mbuf *mcopy;
	struct in_addr dest;
	ROUTE_LOCK_DECL()
#if	NETISR_THREAD
	struct route ipforward_tmp;
#endif

	dest.s_addr = 0;
#if	INETPRINTFS
	if (inetprintfs > 1)
		printf("forward: src %x dst %x ttl %x\n", ip->ip_src,
			ip->ip_dst, ip->ip_ttl);
#endif
	if (m_broadcast(m) || in_canforward(ip->ip_dst) == 0) {
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_cantforward++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		m_freem(m);
		return;
	}
	ip->ip_id = htons(ip->ip_id);
	if (ip->ip_ttl <= IPTTLDEC) {
		icmp_error(m, ICMP_TIMXCEED, ICMP_TIMXCEED_INTRANS, dest);
		return;
	}
	ip->ip_ttl -= IPTTLDEC;

	sin = (struct sockaddr_in *)&ipforward_rt.ro_dst;
	ROUTE_WRITE_LOCK();
	if ((rt = ipforward_rt.ro_rt) == 0 ||
	    ip->ip_dst.s_addr != sin->sin_addr.s_addr) {
		if (ipforward_rt.ro_rt) {
			RTFREE(ipforward_rt.ro_rt);
			ipforward_rt.ro_rt = 0;
		}
		sin->sin_family = AF_INET;
		sin->sin_len = sizeof(*sin);
		sin->sin_addr = ip->ip_dst;

		rtalloc(&ipforward_rt);
		if ((rt = ipforward_rt.ro_rt) == 0) {
			ROUTE_WRITE_UNLOCK();
			icmp_error(m, ICMP_UNREACH, ICMP_UNREACH_HOST, dest);
			return;
		}
	}
	ROUTE_WRITETOREAD_LOCK();

	/*
	 * Save the IP header plus 64 bits of the packet in case
	 * we need to generate an ICMP message to the src.
	 */
	code = ((ip->ip_vhl & 0x0f) << 2) + 8;	/* borrow code for length */
	if (code < (int)ip->ip_len) code = (int)ip->ip_len;
	mcopy = m_copym(m, 0, code, M_DONTWAIT);

	if (ipgateway)
		ip_ifmatrix[rt->rt_ifp->if_index +
			if_index * m->m_pkthdr.rcvif->if_index]++;

	/*
	 * If forwarding packet using same interface that it came in on,
	 * perhaps should send a redirect to sender to shortcut a hop.
	 * Only send redirect if source is sending directly to us,
	 * and if packet was not source routed (or has any options).
	 * Also, don't send redirect if forwarding using a default route
	 * or a route modified by a redirect.
	 */
#define	satosin(sa)	((struct sockaddr_in *)(sa))
	if (rt->rt_ifp == m->m_pkthdr.rcvif &&
	    (rt->rt_flags & (RTF_DYNAMIC|RTF_MODIFIED)) == 0 &&
	    satosin(rt_key(rt))->sin_addr.s_addr != 0 &&
	    ipsendredirects && !srcrt) {
		struct in_ifaddr *ia;
		u_long src = ntohl(ip->ip_src.s_addr);
		u_long dst = ntohl(ip->ip_dst.s_addr);

		if ((ia = ifptoia(m->m_pkthdr.rcvif)) &&
		   (src & ia->ia_subnetmask) == ia->ia_subnet) {
		    if (rt->rt_flags & RTF_GATEWAY)
			dest = satosin(rt->rt_gateway)->sin_addr;
		    else
			dest = ip->ip_dst;
		    /*
		     * If the destination is reached by a route to host,
		     * is on a subnet of a local net, or is directly
		     * on the attached net (!), use host redirect.
		     * (We may be the correct first hop for other subnets.)
		     */
#define	RTA(rt)	((struct in_ifaddr *)(rt->rt_ifa))
		    type = ICMP_REDIRECT;
		    if ((rt->rt_flags & RTF_HOST) ||
		        (rt->rt_flags & RTF_GATEWAY) == 0)
			    code = ICMP_REDIRECT_HOST;
		    else if (RTA(rt)->ia_subnetmask != RTA(rt)->ia_netmask &&
			(dst & RTA(rt)->ia_netmask) ==  RTA(rt)->ia_net)
			    code = ICMP_REDIRECT_HOST;
		    else
			    code = ICMP_REDIRECT_NET;
#if	INETPRINTFS
		    if (inetprintfs > 1)
		        printf("redirect (%d) to %x\n", code, dest.s_addr);
#endif
		}
	}

#if	NETISR_THREAD
	/*
	 * Sigh. Unfortunately, we can't rely on ipforward_rt staying around
	 * while we call ip_output(). We need to make a copy to save in case
	 * somebody else mungs it. However note #define hack.
	 */
	ipforward_tmp = ipforward_rt;
#define ipforward_rt	ipforward_tmp
#endif
	ROUTE_READ_UNLOCK();
	error = ip_output(m, (struct mbuf *)0, &ipforward_rt, IP_FORWARDING);
	NETSTAT_LOCK(&ipstat.ips_lock);
	if (error)
		ipstat.ips_cantforward++;
	else {
		ipstat.ips_forward++;
		if (type)
			ipstat.ips_redirectsent++;
		else {
			NETSTAT_UNLOCK(&ipstat.ips_lock);
			if (mcopy)
				m_freem(mcopy);
			return;
		}
	}
	NETSTAT_UNLOCK(&ipstat.ips_lock);
	if (mcopy == NULL)
		return;
	switch (error) {

	case 0:				/* forwarded, but need redirect */
		/* type, code set above */
		break;

	case ENETUNREACH:		/* shouldn't happen, checked above */
	case EHOSTUNREACH:
	case ENETDOWN:
	case EHOSTDOWN:
	default:
		type = ICMP_UNREACH;
		code = ICMP_UNREACH_HOST;
		break;

	case EMSGSIZE:
		type = ICMP_UNREACH;
		code = ICMP_UNREACH_NEEDFRAG;
		NETSTAT_LOCK(&ipstat.ips_lock);
		ipstat.ips_cantfrag++;
		NETSTAT_UNLOCK(&ipstat.ips_lock);
		break;

	case ENOBUFS:
		type = ICMP_SOURCEQUENCH;
		code = 0;
		break;
	}
	icmp_error(mcopy, type, code, dest);
}

#if	MACH_NET
/*
 * I can't believe I'm putting this support back in. But it's easier
 * to put it here than to fix the NetMsgServer to just use its nice
 * parallel connected transport socket to do the same thing in the
 * way it should. Cf kern/mach_net.c
 */

#include "mach/port.h"		/* For PORT_NULL */

extern int (*send_mach_net_datagram)();	/* Linkage point into IP */

void
mach_net_ipinit()
{
	send_mach_net_datagram = mach_net_ipsend;
}

void
mach_net_ipdone()
{
	send_mach_net_datagram = 0;
}

mach_net_ipsend(p, size)
	caddr_t p;
	register int size;
{
	struct mbuf		*top;
	register struct mbuf	**mp;
	register struct ip	*ip;
	ROUTE_LOCK_DECL()
	static struct route	cached_route;

	ip = (struct ip *)p;
	if (size != ip->ip_len || size <= sizeof (*ip))
		return(FALSE);

	top = 0;
	mp = &top;
	do {
		register struct mbuf *m;
		register unsigned int len;

		if (top == 0) {
			m = m_gethdr(M_WAIT, MT_DATA);
			m->m_pkthdr.len = size;
			m->m_pkthdr.rcvif = 0;
			len = MHLEN;
		} else {
			m = m_get(M_WAIT, MT_DATA);
			len = MLEN;
		}
		if (size > MINCLSIZE) {
			MCLGET(m, M_WAIT);
			if (m->m_flags & M_EXT)
				len = MCLBYTES;
		}
		m->m_len = len = MIN(size, len);
		bcopy(p, mtod(m, caddr_t), len);
		p += len;
		size -= len;
		*mp = m;
		mp = &m->m_next;
	} while (size > 0);

	/*
	 * Ugh. Use cached route if the destination address is the same.
	 * A connected socket would do this for us, and better.
	 */
	ROUTE_WRITE_LOCK();
	if (cached_route.ro_rt && (cached_route.ro_rt->rt_flags & RTF_UP)==0 ||
	    ((struct sockaddr_in *)&cached_route.ro_dst)->sin_addr.s_addr
	    != ip->ip_dst.s_addr) {
		if (cached_route.ro_rt)
			RTFREE(cached_route.ro_rt);
		cached_route.ro_rt = 0;
	}
	ROUTE_WRITE_UNLOCK();

	return ip_output(top, (struct mbuf *)0,
		      &cached_route, (IP_FORWARDING|IP_ALLOWBROADCAST));
}

/*
 *	Source and destination ports.
 *	Common to both UDP datagrams and TCP segments.
 */
struct ip_ports {
	/* struct ip	ip_header; + options */
	u_short		src_port;
	u_short		dst_port;
};

struct mbuf *
mach_net_ipreceive(m, iphlen)
	struct mbuf *m;
	int iphlen;
{
	register struct ip	*ip;
	struct ip_ports		pp;
	port_t			port;

	if (m->m_pkthdr.len < iphlen + sizeof pp)
		return m;
	if (m->m_len >= iphlen + sizeof pp)
		pp = *(struct ip_ports *)(mtod(m, caddr_t) + iphlen);
	else
		m_copydata(m, iphlen, sizeof pp, (caddr_t)&pp);
	/*
	 * The old kern/mach_net didn't swap the ports, but it seems
	 * it should... Either it never ran on little-endian machines
	 * or the server always used palindromes (e.g. 0).
	 */
	pp.src_port = ntohs(pp.src_port);
	pp.dst_port = ntohs(pp.dst_port);
	ip = mtod(m, struct ip *);
	port = find_listener(ip->ip_src.s_addr, pp.src_port,
				ip->ip_dst.s_addr, pp.dst_port, ip->ip_p);
	if (port != PORT_NULL) {
		/* Not sure if server needs this, but kern/mach_net did it */
		if (iphlen > sizeof (struct ip))
			ip_stripoptions(m, (struct mbuf *)0);
		/* Restore IP header, somewhat */
		ip->ip_len += ((ip->ip_vhl & 0x0f) << 2);
		ip->ip_off >>= 3;
		receive_mach_net_datagram(m);
		m = 0;
	}
	return m;
}
#endif
