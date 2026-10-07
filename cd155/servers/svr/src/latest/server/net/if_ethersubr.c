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
 * $Log: if_ethersubr.c,v $
 * Revision 1.5  1994/11/18  20:33:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:07:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:27:38  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:31  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:30  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:18:46  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:28  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1992/03/09  14:39:09  durriya
 * 	91/12/18  17:16:23  sp
 * 	Include sys/synch.h to get spl macros
 *
 * Revision 2.2  91/08/31  13:39:35  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:33:05  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12.4.2  91/03/15  17:47:36  tmt
 * 	Add fourth parm to if_output call.
 * 	[91/03/13  19:05:00  tmt]
 * 
 * Revision 1.12  90/10/07  14:31:41  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:09:58  gm]
 * 
 * 	Patch in arpresolve and ns_thishost for dynamic inet, xns.
 * 	[90/09/29  19:44:58  tmt]
 * 
 * Revision 1.11  90/09/23  15:55:17  devrcs
 * 	Add rtentry parameter to looutput call.
 * 	[90/09/15  15:33:29  tmt]
 * 
 * Revision 1.10  90/09/13  11:47:58  devrcs
 * 	Fix NS configuration test.
 * 	[90/08/28  11:20:29  tmt]
 * 
 * Revision 1.9  90/08/24  12:13:36  devrcs
 * 	Allow dynamic attach of ARP (arpwhohas, arpioctl).
 * 	Test INET, NS options from file, not make line.
 * 	[90/08/19  14:22:40  tmt]
 * 
 * Revision 1.8  90/07/27  08:58:47  devrcs
 * 	Update to BSD Reno release.
 * 	Handle XNS output.
 * 	[90/07/19  16:33:03  tmt]
 * 
 * Revision 1.7  90/04/27  19:11:56  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:23:47  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:14  devrcs
 * 	Use new netisr_input framework. Add arp isr.
 * 	[90/04/09  16:07:15  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:21  gm
 * 	Add RFC1042 SNAP SAP support, both in and out. Incoming SNAP and
 * 	ether framing always supported, outgoing framing selected by
 * 	IFF_SNAP flag (set in ifconfig).
 * 	[90/01/09  12:31:40  tmt]
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
 * Revision 1.3  90/01/02  20:10:48  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:44:05  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1989 Regents of the University of California.
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
 *	Base:	if_ethersubr.c	7.5 (Berkeley) 9/20/89
 *	Merged:	if_ethersubr.c	7.10 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"

#ifdef	OSF1_SERVER
#include <sys/synch.h>
#endif

#include "sys/mbuf.h"
#include "sys/socket.h"

#include "net/if.h"
#include "net/if_llc.h"
#include "net/netisr.h"
#include "net/route.h"

/* Domain stuff will go away with dynamic protocols */
#include "netinet/in.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#include "netns/ns.h"
#include "netns/ns_if.h"

LOCK_ASSERTL_DECL

CONST u_char	etherbroadcastaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
union ns_host ns_thishost;	/* XXX */
extern	struct ifnet loif;

/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Use trailer local net encapsulation if enough data in first
 * packet leaves a multiple of 512 bytes of data in remainder.
 * Assumes that ifp is actually pointer to arpcom structure.
 */
ether_output(ifp, m0, dst, rt)
	register struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
	struct rtentry *rt;
{
	short type;
	int s, error = 0;
 	u_char edst[6];
	struct in_addr idst;
	register struct mbuf *m = m0;
	struct mbuf *mcopy = (struct mbuf *)0;
	register struct ether_header *eh;
	int usetrailers, off, len = m->m_pkthdr.len;
#define	ac ((struct arpcom *)ifp)

	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING)) {
		error = ENETDOWN;
		goto bad;
	}
	switch (dst->sa_family) {

	case AF_INET:
		idst = ((struct sockaddr_in *)dst)->sin_addr;
 		if (!arpresolve(ac, m, &idst, edst, &usetrailers))
			return (0);	/* if not yet resolved */
		if ((ifp->if_flags & IFF_SIMPLEX) && (*edst & 1))
			mcopy = m_copym(m, 0, (int)M_COPYALL, M_DONTWAIT);
		off = m->m_pkthdr.len - m->m_len;
		if (usetrailers && off > 0 && (off & 0x1ff) == 0 &&
		    (m->m_flags & M_EXT) == 0 &&
		    m->m_data >= m->m_pktdat + 2 * sizeof (u_short)) {
			type = ETHERTYPE_TRAIL + (off>>9);
			m->m_data -= 2 * sizeof (u_short);
			m->m_len += 2 * sizeof (u_short);
			len += 2 * sizeof (u_short);
			*mtod(m, u_short *) = htons((u_short)ETHERTYPE_IP);
			*(mtod(m, u_short *) + 1) = htons((u_short)m->m_len);
			goto gottrailertype;
		}
		type = ETHERTYPE_IP;
		goto gottype;

	case AF_NS:
		type = ETHERTYPE_NS;
		bcopy((caddr_t)&(((struct sockaddr_ns *)dst)->sns_addr.x_host),
		    (caddr_t)edst, sizeof (edst));
		if (!bcmp((caddr_t)edst, (caddr_t)&ns_thishost, sizeof(edst)))
			return (looutput(&loif, m, dst, rt));
		if ((ifp->if_flags & IFF_SIMPLEX) && (*edst & 1))
			mcopy = m_copym(m, 0, (int)M_COPYALL, M_DONTWAIT);
		goto gottype;

	case AF_UNSPEC:
		eh = (struct ether_header *)dst->sa_data;
 		bcopy((caddr_t)eh->ether_dhost, (caddr_t)edst, sizeof (edst));
		type = eh->ether_type;
		goto gottype;

	default:
		printf("%s%d: can't handle af%d\n", ifp->if_name, ifp->if_unit,
			dst->sa_family);
		error = EAFNOSUPPORT;
		goto bad;
	}

gottrailertype:
	/*
	 * Packet to be sent as trailer: move first packet
	 * (control information) to end of chain.
	 */
	while (m->m_next)
		m = m->m_next;
	m->m_next = m0;
	m = m0->m_next;
	m0->m_next = 0;

gottype:
	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */
#define SNAP_LEN 8			/* dsap + ssap + snap */
	if (ifp->if_flags & IFF_SNAP) {
		M_PREPEND(m, sizeof (struct ether_header)+SNAP_LEN, M_DONTWAIT);
	} else {
		M_PREPEND(m, sizeof (struct ether_header), M_DONTWAIT);
	}
	if (m == 0) {
		error = ENOBUFS;
		goto bad;
	}
	eh = mtod(m, struct ether_header *);
	type = htons((u_short)type);
	if (ifp->if_flags & IFF_SNAP) {
		struct llc *llc = (struct llc *)(eh + 1);
		llc->llc_dsap = llc->llc_ssap = LLC_SNAP_LSAP;
		llc->llc_snap.control = LLC_UI;
		llc->llc_snap.org_code[0] =
			llc->llc_snap.org_code[1] =
				llc->llc_snap.org_code[2] = 0;
		bcopy((caddr_t)&type, (caddr_t)&llc->llc_snap.ether_type,
			sizeof(llc->llc_snap.ether_type));
		type = (len += SNAP_LEN);
		type = htons((u_short)type);
	}
	bcopy((caddr_t)&type,(caddr_t)&eh->ether_type,
		sizeof(eh->ether_type));
 	bcopy((caddr_t)edst, (caddr_t)eh->ether_dhost, sizeof (edst));
 	bcopy((caddr_t)ac->ac_enaddr, (caddr_t)eh->ether_shost,
	    sizeof(eh->ether_shost));
	/*
	 * Queue message on interface, and start output if interface
	 * not yet active.
	 */
	s = splimp();
	IFQ_LOCK(&ifp->if_snd);
	if (IF_QFULL(&ifp->if_snd)) {
		IF_DROP(&ifp->if_snd);
		IFQ_UNLOCK(&ifp->if_snd);
		splx(s);
		error = ENOBUFS;
		goto bad;
	}
	IF_ENQUEUE_NOLOCK(&ifp->if_snd, m);
	IFQ_UNLOCK(&ifp->if_snd);
	NETSTAT_LOCK(&ifp->if_slock);
	microtime(&ifp->if_lastchange);
	ifp->if_obytes += len + sizeof (struct ether_header);
	if (edst[0] & 1)
		ifp->if_omcasts++;
	NETSTAT_UNLOCK(&ifp->if_slock);
	if ((ifp->if_flags & IFF_OACTIVE) == 0)
		(*ifp->if_start)(ifp);
	splx(s);
	if (mcopy)
		(void) looutput(&loif, mcopy, dst, rt);
	return (error);

bad:
	if (mcopy)
		m_freem(mcopy);
	if (m)
		m_freem(m);
	return (error);
}

/*
 * Process a received Ethernet packet;
 * the packet is in the mbuf chain m without
 * the ether header, which is provided separately.
 */
void
ether_input(ifp, eh, m)
	struct ifnet *ifp;
	register struct ether_header *eh;
	struct mbuf *m;
{
	register struct llc *l;
	int s, isr = -1;

	if (eh->ether_dhost[0] & 1) {
		if (bcmp((caddr_t)etherbroadcastaddr, (caddr_t)eh->ether_dhost,
		    sizeof(etherbroadcastaddr)) == 0)
			m->m_flags |= M_BCAST;
		else
			m->m_flags |= M_MCAST;
	}
	s = splimp();
	NETSTAT_LOCK(&ifp->if_slock);
	microtime(&ifp->if_lastchange);
	ifp->if_ibytes += m->m_pkthdr.len + sizeof (*eh);
	if (m->m_flags & (M_BCAST|M_MCAST))
		ifp->if_imcasts++;
	NETSTAT_UNLOCK(&ifp->if_slock);
	splx(s);
again:
	switch (eh->ether_type) {

	case ETHERTYPE_IP:
		isr = NETISR_IP;
		break;

	case ETHERTYPE_ARP:
		isr = NETISR_ARP;
		break;

	default:
		if (eh->ether_type > ETHERMTU)
			break;
		if (eh->ether_type > m->m_pkthdr.len)
			break;
		l = mtod(m, struct llc *);
		switch (l->llc_control) {
		case LLC_UI:
		/* LLC_UI_P forbidden in class 1 service */
		    if (m->m_len < 2)
			goto dropanyway;
		    if ((l->llc_dsap == LLC_ISO_LSAP) &&
			(l->llc_ssap == LLC_ISO_LSAP)) {
				/* LSAP for ISO */
				isr = NETISR_ISO;
				break;
		    }
		    if ((l->llc_dsap == LLC_SNAP_LSAP) &&
			(l->llc_ssap == LLC_SNAP_LSAP)) {
			    if (m->m_len < SNAP_LEN)
				goto dropanyway;
			    if (l->llc_snap.control == LLC_UI &&
			        l->llc_snap.org_code[0] == 0 &&
			        l->llc_snap.org_code[1] == 0 &&
			        l->llc_snap.org_code[2] == 0) {
					/* RFC1042 IP SNAP SAP */
					eh->ether_type =
						ntohs(l->llc_snap.ether_type);
					/* Bump past SNAP header */
					m->m_data	+= SNAP_LEN;
					m->m_len	-= SNAP_LEN;
					m->m_pkthdr.len	-= SNAP_LEN;
					/* Technically the type should now be
					 * one of IP, ARP, RARP, (more?), but
					 * what the heck... */
					goto again;
			    }
			    break;
		    }
		    /* Other SNAP SAP code here */
		    goto dropanyway;

		case LLC_XID:
		case LLC_XID_P:
		    if (m->m_len < 6)
			goto dropanyway;
		    l->llc_window = 0;
		    l->llc_fid = 9;
		    l->llc_class = 1;
		    l->llc_dsap = l->llc_ssap = 0;
		    /* Fall through to */
		case LLC_TEST:
		case LLC_TEST_P:
		{
		    struct sockaddr sa;
		    register struct ether_header *eh2;
		    int i;
		    u_char c = l->llc_dsap;
		    l->llc_dsap = l->llc_ssap;
		    l->llc_ssap = c;
		    if (m->m_flags & (M_BCAST | M_MCAST))
			bcopy((caddr_t)ac->ac_enaddr,
			      (caddr_t)eh->ether_dhost, 6);
		    sa.sa_family = AF_UNSPEC;
		    sa.sa_len = sizeof(sa);
		    eh2 = (struct ether_header *)sa.sa_data;
		    for (i = 0; i < 6; i++) {
			eh2->ether_shost[i] = c = eh->ether_dhost[i];
			eh2->ether_dhost[i] = 
				eh->ether_dhost[i] = eh->ether_shost[i];
			eh->ether_shost[i] = c;
		    }
		    eh2->ether_type = eh->ether_type;
		    ifp->if_output(ifp, m, &sa, (struct rtentry *)0);
		    return;
		}
		dropanyway:
		default:
		    m_freem(m);
		    goto noproto;
	    }
	}
	if (netisr_input(isr, m, (caddr_t)eh, sizeof *eh)) {
noproto:
		s = splimp();
		NETSTAT_LOCK(&ifp->if_slock);
		ifp->if_noproto++;
		NETSTAT_UNLOCK(&ifp->if_slock);
		splx(s);
	}
}
#undef	ac

/*
 * Stubs for dynamic attach of ARP to drivers (XXX).
 */
void (*arpreq)();
int  (*arpres)();
int  (*arpctl)();

int
arpioctl(cmd, data)
	int cmd;
	caddr_t data;
{
	if (arpctl)
		return (*arpctl)(cmd, data);
	return EINVAL;
}

void
arpwhohas(ac, addr)
	struct arpcom *ac;
	struct in_addr *addr;
{
	if (arpreq)
		(*arpreq)(ac, addr, ac->ac_bcastaddr);
}

int
arpresolve(ac, m, destip, desten, usetrailers)
	register struct arpcom *ac;
	struct mbuf *m;
	register struct in_addr *destip;
	register u_char *desten;
	int *usetrailers;
{
	if (arpres)
		return (*arpres)(ac, m, destip, desten, usetrailers);
	return EINVAL;
}

/*
 * Convert Ethernet address to printable (loggable) representation.
#if	NETSYNC_LOCK
 * This is not currently protected by locks, but used only by if_qe.
 * If anyone else wants to use this, it would be best to pass in &etherbuf.
#endif
 */
CONST static char digits[] = "0123456789abcdef";
char *
ether_sprintf(ap)
	register u_char *ap;
{
	register i;
	static char etherbuf[18];
	register char *cp = etherbuf;

	for (i = 0; i < 6; i++) {
		*cp++ = digits[*ap >> 4];
		*cp++ = digits[*ap++ & 0xf];
		*cp++ = ':';
	}
	*--cp = 0;
	return (etherbuf);
}
