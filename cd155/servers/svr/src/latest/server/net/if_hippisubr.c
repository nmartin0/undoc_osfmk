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
 * HISTORY
 * $Log: if_hippisubr.c,v $
 * Revision 1.5  1994/11/18  20:33:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/05/06  20:34:07  hobbes
 * Merged over the final R1.2 fix .. fixing the error message
 * printed when a bogus packet arrives on HIPPI TCP/IP and doing
 * the appropriate things to stats counters.
 *
 * Revision 1.2.2.1  1994/04/28  18:10:38  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 9168
 *  Testing: HIPPI Eats and tests to force the error case
 *  Module(s): server/net/if_hippisubr.c
 *
 *  This fix will improve the usability of the error printf
 *  which comes out when a bogus packet arrives at the
 *  hippi_input function.
 *
 * Revision 1.3  1993/12/20  19:07:05  dleslie
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: don't declare variable whose address the code takes
 * 	as 'register', so we pass 'lint'
 *  Testing: builds
 *  Module(s):
 *
 * Revision 1.2  1993/11/23  00:46:50  hobbes
 *  Reviewer: bernie keany, arlin davis
 *  Risk: low
 *  Benefit or PTS #: debugging info always checked, slower HIPPI TCP/IP processing
 *  Testing: built R1.2 server .. tested on a WW45 system.
 *  Module(s): net/if_hippi.h net/if_hippisubr.c
 *
 * made the debugging logic be a compile time feature
 *
 * Revision 1.1  1993/05/27  22:31:01  hobbes
 * Initial checkin of HIPPI interface specific routines.
 *
 */
/*
 * File:    if_hippisubr.c
 *
 * Abstract:
 *	HIPPI interface specific routines for sending and
 *	receiving packets from the Device Driver.
 *	Structure mercilessly borrowed from the if_ethersubr.c
 *	module delivered by OSF1/AD.
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
#include "net/if_hippi.h"

/* Domain stuff will go away with dynamic protocols */
#include "netinet/in.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#include "netns/ns.h"
#include "netns/ns_if.h"

LOCK_ASSERTL_DECL

union ns_host ns_thishost;	/* XXX */
extern	struct ifnet loif;

#if HIPPIPRINTFS
extern int hippiprintfs;
#endif

/*
 * HIPPI output routine.
 * Encapsulate a packet of type family for the local net.
 * Assumes that ifp is actually pointer to arpcom structure.
 */
hippi_output(ifp, m0, dst, rt)
	register struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
	struct rtentry *rt;
{
	short type;
	int s, error = 0;
 	u_char dest_ieee[6];
	u_long I; /* Ifield */
	struct in_addr idst;
	register struct mbuf *m = m0;
	struct mbuf *mcopy = (struct mbuf *)0;
	register struct hippi_header *hh;
	int usetrailers, off, len = m->m_pkthdr.len;
#define	ac ((struct arpcom *)ifp)

	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING)) {
		error = ENETDOWN;
		goto bad;
	}
	switch (dst->sa_family) {

	case AF_INET:
		idst = ((struct sockaddr_in *)dst)->sin_addr;

		/* printf("about to call hippi_artpresolve\n"); */
		/* printf("ip address passed in was %s\n", inet_ntoa(idst)); */

 		if ( hippi_artpresolve(dst, dest_ieee, &I) < 0)
 			return(0);

		/* printf("ifield pulled from the table was %x\n", I); */

		type = ETHERTYPE_IP;
		/* printf("about to branch to gottype\n"); */
		goto gottype;

	case AF_UNSPEC:
/**** not sure what to do with this type *****
		eh = (struct ether_header *)dst->sa_data;
 		bcopy((caddr_t)eh->ether_dhost, (caddr_t)edst, sizeof (edst));
		type = eh->ether_type;
****/
		goto gottype;

	default:
		printf("%s%d: can't handle af%d\n", ifp->if_name, ifp->if_unit,
			dst->sa_family);
		error = EAFNOSUPPORT;
		goto bad;
	}

gottype:
	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */

	/* printf("here at gottype\n"); */
#if HIPPIPRINTFS
	if (hippiprintfs & HIPPIOUT)
		printf("hippi_output: len is %d\n", len);
#endif

	M_PREPEND(m, sizeof (struct hippi_header), M_DONTWAIT);
	if (m == 0) {
		error = ENOBUFS;
		goto bad;
	}

	hh = mtod(m, struct hippi_header *);
	type = htons((u_short)type);

	bzero((caddr_t)hh, sizeof(struct hippi_header));
	hh->cci.Ifield = htonl(I);
	hh->fp.words.w1 = htonl(0x04800018);  /* FP-header,ulp=4,d1size=3 */
	hh->fp.fields.d2_size = htonl(len + sizeof(struct llc));

	hh->llc.llc_dsap = hh->llc.llc_ssap = LLC_SNAP_LSAP;
	hh->llc.llc_snap.control = LLC_UI;
	bcopy((caddr_t)&type, (caddr_t)&hh->llc.llc_snap.ether_type,
		sizeof(hh->llc.llc_snap.ether_type));

	bcopy(dest_ieee, hh->le.dest_ieee_addr, sizeof(dest_ieee));

#if HIPPIPRINTFS
	if (hippiprintfs & HIPPIOUT) {
		printf("prepended hippi_header size = %d llc size = %d\n",
			sizeof(struct hippi_header), sizeof(struct llc));
	}
#endif
	
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

	/* printf("enqueued the message on the hippi interface\n"); */

	IF_ENQUEUE_NOLOCK(&ifp->if_snd, m);
	IFQ_UNLOCK(&ifp->if_snd);
	NETSTAT_LOCK(&ifp->if_slock);
	microtime(&ifp->if_lastchange);
	ifp->if_obytes += len + sizeof (struct hippi_header);
	if (dest_ieee[0] & 1)
		ifp->if_omcasts++;
	NETSTAT_UNLOCK(&ifp->if_slock);

	/* printf("about to call the start routine\n"); */

	if ((ifp->if_flags & IFF_OACTIVE) == 0) {
		/* printf("calling the start routine\n"); */
		(*ifp->if_start)(ifp);
	}
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
 * Process a received HIPPI packet;
 * the packet is in the mbuf chain m.
 * There is no data in the header, unlike ethernet.
 */
void
hippi_input(ifp, m)
	struct ifnet *ifp;
	struct mbuf *m;
{
	register struct llc *l;
	int s, isr = -1;
	struct ether_header eh;
	register struct inbound_hippi_header *hh;

#if HIPPIPRINTFS
	if (hippiprintfs & HIPPIIN)
		printf("made it to hippi_input\n");
#endif

	s = splimp();
	NETSTAT_LOCK(&ifp->if_slock);
	microtime(&ifp->if_lastchange);
	ifp->if_ibytes += m->m_pkthdr.len;
	if (m->m_flags & (M_BCAST|M_MCAST))
		ifp->if_imcasts++;
	NETSTAT_UNLOCK(&ifp->if_slock);
	splx(s);

	/* map a hippi_header to the mbuf */
	hh = mtod(m, struct inbound_hippi_header *);

#if HIPPIPRINTFS
	if (hippiprintfs & HIPPIIN) {
		hh->fp.words.w1 = ntohl(hh->fp.words.w1);
		printf("HIPPI packet arrived decoding headers ---- \n");
		printf("\tfp header\n");
		printf("\t\tulp=%d d1_size=%d d2_size=%d\n", hh->fp.fields.ulp,							hh->fp.fields.d1_size, ntohl(hh->fp.fields.d2_size));
		printf("\tLLC header\n");
		printf("\t\tDSAP=%d SSAP=%d SNAP=%d\n", hh->llc.llc_dsap,
				hh->llc.llc_ssap, hh->llc.llc_snap.control);
		printf("\t\t ether_type = %x\n", ntohs(hh->llc.llc_snap.ether_type));
	}
#endif

	switch (ntohs(hh->llc.llc_snap.ether_type)) {

	case ETHERTYPE_IP:
		/* adjust the packet to point at the start of IP */

		/* printf("identified a HIPPI packet .. adjusting packet\n"); */

		m->m_data   += sizeof(struct inbound_hippi_header);
		m->m_len    -= sizeof(struct inbound_hippi_header);
		m->m_pkthdr.len -= sizeof(struct inbound_hippi_header);

#if HIPPIPRINTFS
		if (hippiprintfs & HIPPIIN)
			printf("packet size being passed up %d\n", m->m_len);
#endif

		/* I know netisr_input doesn't use eh but just to be sure ... */
		eh.ether_type = hh->llc.llc_snap.ether_type;
		isr = NETISR_IP;
		break;

	case ETHERTYPE_ARP:
		isr = NETISR_ARP;
		break;

	default:
		printf("dropping invalid HIPPI-LE packet type = 0x%x\n",
			ntohs(hh->llc.llc_snap.ether_type));
		m_freem(m);
		goto noproto;
	}
	if (netisr_input(isr, m, (caddr_t)&eh, sizeof(struct ether_header) )) {
noproto:
		s = splimp();
		NETSTAT_LOCK(&ifp->if_slock);
		ifp->if_noproto++;
		ifp->if_ierrors++;
		/*
		 * this is truly gross but we incremented it in
		 * the net_input_thread so we need to decrement
		 * it here
		 */
		ifp->if_ipackets--;
		NETSTAT_UNLOCK(&ifp->if_slock);
		splx(s);
	}
}
#undef	ac
