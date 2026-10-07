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
 * $Log: if_ether.c,v $
 * Revision 1.6  1995/02/11  00:21:50  stans
 *  'lint' picking with typedefs for a clean compile.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.5  1994/11/18  20:34:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:11:12  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:04:49  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.3  1993/05/06  20:25:29  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:22  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:15  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:26:39  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:14  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/10/17  18:26:01  cfj
 * IPD & Allocator Support.
 *
 * Revision 2.5  93/07/07  10:34:06  mjl
 * [LCC #0314] Use NET_THREADSTART() macro for deferred startup of net threads.
 * 
 * Revision 2.4  1992/05/24  14:35:56  pjg
 * 	92/03/31  15:40:47  emcmanus
 * 	Name the arptimer thread (if used).
 * 	[92/05/18            srl]
 *
 * Revision 2.3  92/03/09  14:42:14  durriya
 * 	91/12/18  17:16:41  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.2  91/08/31  13:42:21  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:34:42  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.4.3  91/03/15  17:48:39  tmt
 * 	Cast arguments to arpoutput. Add Ansi declaration for same.
 * 	[91/03/13  19:08:20  tmt]
 * 
 * Revision 1.13.4.2  91/02/19  15:24:22  tmt
 * 	Set pkthdr before replying in in_arpinput().
 * 	[91/02/16  15:50:19  tmt]
 * 
 * Revision 1.13  90/10/07  14:33:41  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:13:07  gm]
 * 
 * 	Move arpresolve() stub to net/if_ethersubr.c.
 * 	[90/09/29  19:47:15  tmt]
 * 
 * Revision 1.12  90/09/23  15:55:43  devrcs
 * 	Add rtentry parameter to looutput.
 * 	[90/09/15  15:36:06  tmt]
 * 
 * Revision 1.11  90/08/24  12:14:21  devrcs
 * 	Set arpwhohas and arpioctl vectors for dynamic attach.
 * 	[90/08/17  18:07:51  tmt]
 * 
 * Revision 1.10  90/07/27  09:00:18  devrcs
 * 	Update to BSD Reno release.
 * 	Reset at_sent on reply so stale refresh will occur later.
 * 	[90/07/19  17:24:39  tmt]
 * 
 * Revision 1.9  90/07/05  23:12:58  devrcs
 * 	Uniprocessor and spl compat. Merge splnet and ARP_LOCK.
 * 	[90/07/03  18:50:47  tmt]
 * 
 * Revision 1.8  90/06/22  20:38:51  devrcs
 * 	Enhanced ARP:
 * 		Single shared table for all interfaces
 * 		Hardware address independent
 * 		Stale detection with unicast refresh
 * 		Anti-flooding of resolves with backoff
 * 		Early freeing of unresolved mbufs
 * 		Tunable defaults
 * 	[90/06/15  08:14:44  tmt]
 * 
 * Revision 1.7  90/04/27  19:14:36  devrcs
 * 	Run at splnet. Finally get COMPAT_43 right.
 * 	Fix arptab_bsiz to match netstat's idea.
 * 	[90/04/20  12:45:19  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:55  devrcs
 * 	Add arpintr() and arpintrq. Use netisr_add to install in softnet_intr.
 * 	Add void's. Always copyout old-style sockaddr's at ioctl's.
 * 	[90/04/09  16:26:18  tmt]
 * 
 * Revision 1.5  90/03/27  13:21:51  gm
 * 	Change args to lock_init2() [gmf]
 * 
 * Revision 1.4  90/01/18  08:44:42  gm
 * 	Fix struct mbuf *m hiding previous declaration.
 * 	[90/01/08  16:00:35  tmt]
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
 * Revision 1.3  90/01/03  12:41:17  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:30  gm]
 * 
 * Revision 1.2  89/12/26  09:49:09  gm
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
 *	Base:	if_ether.c	7.10 (Berkeley) 4/22/89
 *	Merged:	if_ether.c	7.12 (Berkeley) 6/28/90
 */

/*
 * Ethernet address resolution protocol.
 * TODO:
 *	link entries onto hash chains, keep free list
 *	add "inuse/lock" bit (or ref. count) along with valid bit
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/kernel.h"
#include "sys/errno.h"
#include "sys/ioctl.h"
#include "sys/syslog.h"

#ifdef	OSF1_SERVER
#include <sys/synch.h>
#endif

#include "sys/mbuf.h"
#include "sys/socket.h"

#include "net/if.h"
#include "net/route.h"
#include "net/netisr.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#ifdef NX 
#include "machine/endian.h"
#endif

LOCK_ASSERTL_DECL

struct	arptab *arptab;
int	arptab_size;				/* count of arptab structs */
int	arptab_bsiz = 16, arptab_nb = 37;	/* tabs per bucket, # buckets */
static	char anypublished;

static	struct arptab arptabXXX[37*16];	/* To avoid malloc, for now */

struct	ifqueue arpintrq;
int	arpqmaxlen = IFQ_MAXLEN;
int	useloopback = 1;	/* use loopback interface for local traffic */


#if	NETSYNC_LOCK
lock_data_t	global_arp_lock;
#define ARP_LOCKINIT()	lock_init2(&global_arp_lock, TRUE, LTYPE_ARP)
#define ARP_LOCK()	{ NETSPL(s,net); lock_write(&global_arp_lock); }
#define ARP_UNLOCK()	{ lock_done(&global_arp_lock); NETSPLX(s); }
#else
#define ARP_LOCKINIT()
#define ARP_LOCK()	NETSPL(s,net)
#define ARP_UNLOCK()	NETSPLX(s)
#endif

/*
 * ARP trailer negotiation.  Trailer protocol is not IP specific,
 * but ARP request/response use IP addresses.
 */
#define ETHERTYPE_IPTRAILERS ETHERTYPE_TRAIL

#define	ARPTAB_HASH(a) \
	((u_long)(a) % arptab_nb)

#define	ARPTAB_LOOK(at,addr,ifp) { \
	register short n; \
	at = &arptab[ARPTAB_HASH(addr) * arptab_bsiz]; \
	for (n = 0 ; n < arptab_bsiz; n++,at++) \
		if (at->at_if == (ifp) && at->at_iaddr.s_addr == (addr)) \
			break; \
	if (n >= arptab_bsiz) \
		at = 0; \
}

/*
 * ARP (RFC826) protocol.
 *
 * Enhancements over traditional BSD:
 *	Single shared table for all if's
 *	Hardware (interface) independent
 *	Stale detection with unicast refresh
 *	Anti-flooding of resolves with backoff
 *	Early freeing of unresolved mbufs
 *
 * Primary limitation: per-interface output routine required (arpoutput()).
 */

/* timer values */
#ifdef NX

#define	ARPT_AGE	(1*10)	/* aging timer, 10 secs. */
/* Settable values (times ARPT_AGE seconds) used by arptimer() */
int	arpkillc   = 20*6;	/* kill completed entry in 20 mins. */
int	arpkilli   = 3*6;	/* kill incomplete entry in 3 minutes */
int	arprefresh = 10*6;	/* time to refresh entry */
int	arphold	   = 1;		/* time to keep at_hold packet */

#else
#define	ARPT_AGE	1	/* aging timer, 1 sec. */

/* Settable values (in SECONDS) used by arptimer() */
int	arpkillc   = 20*60;	/* kill completed entry in 20 mins. */
int	arpkilli   = 3*60;	/* kill incomplete entry in 3 minutes */
int	arprefresh = 2*60;	/* time to refresh entry */
int	arphold	   = 5;		/* time to keep at_hold packet */
#endif /* NX */
/* Settable counts used by arpresolve() */
int	arplost	   = 3;		/* retry threshold for broadcast */
int	arpdead	   = 6;		/* retry threshold for backoff */

extern struct ifnet loif;

/*
 * Timeout routine.  Update arp_tab entries once a second.
 */
arptimer()
{
	register struct arptab *at;
	register i;
	NETSPL_DECL(s)

	ARP_LOCK();
	at = &arptab[0];
	for (i = 0; i < arptab_size; i++, at++) {
		if (at->at_flags == 0)
			continue;
		/* release held packet after hold time */
		if (++at->at_timer >= arphold && at->at_hold) {
			m_freem(at->at_hold);
			at->at_hold = 0;
		}
		/* no further action if perm */
		if (at->at_flags & ATF_PERM)
			continue;
		/* set stale bit if need refresh */
		if (++at->at_valid >= arprefresh)
			at->at_flags |= ATF_STALE;
		/* check expiration time */
		if (at->at_timer < ((at->at_flags&ATF_COM)?arpkillc:arpkilli))
			continue;
		/* timer has expired, clear entry */
		arptfree(at);
	}
	ARP_UNLOCK();
#if	!NETISR_THREAD
	timeout(arptimer, (caddr_t)0, ARPT_AGE * hz);
#else
	return (ARPT_AGE * hz);
#endif
}

/*
 * Send an ARP packet, asking who has addr on interface ac.
 */
static void
arprequest(ac, addr, dest)
	register struct arpcom *ac;
	struct in_addr *addr;
	u_char *dest;		/* Dest hwaddr - bcast/etc */
{
	register struct mbuf *m;
	register struct arphdr *ah;
	int hln, pln;

	hln = ac->ac_if.if_addrlen;
	pln = sizeof (*addr);
if (hln <= 0 || sizeof (*ah) + (2*hln) + (2*pln) > MHLEN) return;
	if ((m = m_gethdr(M_DONTWAIT, MT_DATA)) == NULL)
		return;
	m->m_pkthdr.len = m->m_len = sizeof(*ah) + (2*hln) + (2*pln);
	m->m_pkthdr.rcvif = 0;
	MH_ALIGN(m, m->m_len);
	ah = mtod(m, struct arphdr *);
	ah->ar_pro = htons(ETHERTYPE_IP);
	ah->ar_hln = hln;	/* hardware address length */
	ah->ar_pln = pln;	/* protocol address length */
	ah->ar_op = htons(ARPOP_REQUEST);
	bcopy((caddr_t)ac->ac_hwaddr, (caddr_t)AR_SHA(ah), hln);
	bcopy((caddr_t)&ac->ac_ipaddr, (caddr_t)AR_SPA(ah), pln);
	bzero((caddr_t)AR_THA(ah), hln);
	bcopy((caddr_t)addr, (caddr_t)AR_TPA(ah), pln);
	arpoutput(ac, m, dest, ac->ac_arphrd);
}

/*
 * Resolve an IP address into a hardware address.  If success, 
 * desten is filled in.  If there is no entry in arptab,
 * set one up and broadcast a request for the IP address.
 * Hold onto this mbuf and resend it once the address
 * is finally resolved.  A return value of 1 indicates
 * that desten has been filled in and the packet should be sent
 * normally; a 0 return indicates that the packet has been
 * taken over here, either now or for later transmission.
 *
 * We now raise interrupts only to splnet, since arpinput no
 * longer runs as a packet interrupt routine.
 */
static
arpresolve_local(ac, m, destip, desten, usetrailers)
	register struct arpcom *ac;
	struct mbuf *m;
	register struct in_addr *destip;
	register u_char *desten;
	int *usetrailers;
{
	register struct arptab *at;
	struct sockaddr_in sin;
	register struct in_ifaddr *ia;
	int retval = 0;
	NETSPL_DECL(s)

/* XXX Compat for non-ported drivers XXX check IFT_ETHER */
if (ac->ac_if.if_addrlen == 0)
	ac->ac_if.if_addrlen = 6;
if (ac->ac_if.if_addrlen == 6 && !ac->ac_bcastaddr)
	ac->ac_bcastaddr = (u_char *)etherbroadcastaddr;
if (ac->ac_if.if_addrlen == 6 && ac->ac_arphrd == 0)
	ac->ac_arphrd = ARPHRD_ETHER;

	*usetrailers = 0;
	if (m->m_flags & M_BCAST) {	/* broadcast */
		if (!ac->ac_bcastaddr) {
			m_freem(m);
			return (0);
		}
		bcopy(ac->ac_bcastaddr, (caddr_t)desten,
		    (int)ac->ac_if.if_addrlen);
		return (1);
	}
	/* if for us, use software loopback driver if up */
	for (ia = in_ifaddr; ia; ia = ia->ia_next)
	    if ((ia->ia_ifp == &ac->ac_if) &&
		(destip->s_addr == ia->ia_addr.sin_addr.s_addr)) {
		/*
		 * This test used to be
		 *	if (loif.if_flags & IFF_UP)
		 * It allowed local traffic to be forced
		 * through the hardware by configuring the loopback down.
		 * However, it causes problems during network configuration
		 * for boards that can't receive packets they send.
		 * It is now necessary to clear "useloopback"
		 * to force traffic out to the hardware.
		 */
		if (useloopback) {
			sin.sin_family = AF_INET;
			sin.sin_len = sizeof(sin);
			sin.sin_addr = *destip;
			(void) looutput(&loif, m, (struct sockaddr *)&sin,						(struct rtentry *)0);
			/*
			 * The packet has already been sent and freed.
			 */
			return (0);
		} else {
			bcopy((caddr_t)ac->ac_hwaddr, (caddr_t)desten,
			    (int)ac->ac_if.if_addrlen);
			return (1);
		}
	}
	ARP_LOCK();
	ARPTAB_LOOK(at, destip->s_addr, &ac->ac_if);
	if (at == 0) {			/* not found */
		if (ac->ac_if.if_flags & IFF_NOARP) {
			u_long lna; int len;
			ARP_UNLOCK();
			lna = in_lnaof(*destip);
			len = ac->ac_if.if_addrlen;
			bcopy((caddr_t)ac->ac_hwaddr, (caddr_t)desten, len);
			desten[--len] = lna & 0xff;
			if (len > 0) desten[--len] = (lna >> 8) & 0xff;
			if (len > 0) desten[--len] = (lna >> 16) & 0x7f;
			return (1);
		}
		at = arptnew(destip, &ac->ac_if);
		if (at == 0)
			panic("arpresolve: no free entry");
		at->at_hold = m;
		ARP_UNLOCK();
		arprequest(ac, destip, ac->ac_bcastaddr);
		return (0);
	}
	at->at_timer = 0;		/* restart the timer */
	if (at->at_flags & ATF_COM) {	/* entry IS complete */
		bcopy((caddr_t)at->at_hwaddr, (caddr_t)desten,
		    (int)at->at_if->if_addrlen);
		if (at->at_flags & ATF_USETRAILERS)
			*usetrailers = 1;
		retval = 1;
	} else {
		/*
		 * There is an arptab entry, but no hardware address
		 * response yet.  Replace the held mbuf with this
		 * latest one.
		 */
		if (at->at_hold)
			m_freem(at->at_hold);
		at->at_hold = m;
	}
	/* Prevent ARP flooding of unresolved entries. ARP depends on
	 * transport level sends to retry, but backs off independently */
	if ((!retval || (at->at_flags & ATF_STALE)) &&
	    at->at_sent <= at->at_valid) {
		u_char hwaddr[sizeof at->at_hwaddr];
		if (!(at->at_flags & ATF_DEAD) && ++at->at_retry >= arpdead)
			at->at_flags |= ATF_DEAD;	/* no further backoff */
		/* Backoff 2 sec, 4 sec, 8 sec ... (64 sec default) */
		at->at_sent = at->at_valid + (1 << at->at_retry);
		/* Unicast if known and < 3 tries, else broadcast */
		/* Copy out known address so can unlock for send */
		if (retval && at->at_retry < arplost)
		    bcopy(at->at_hwaddr, hwaddr, (int)ac->ac_if.if_addrlen);
		else if (ac->ac_bcastaddr)
		    bcopy(ac->ac_bcastaddr, hwaddr, (int)ac->ac_if.if_addrlen);
		else
		    goto out;
		/* Time to refresh the modify bits, too */
		at->at_flags &= ~(ATF_USETRAILERS|ATF_USE802);
		ARP_UNLOCK();
		arprequest(ac, destip, hwaddr);
		return retval;
	}
out:
	ARP_UNLOCK();
	return retval;
}

/* Used in logmsg below */
static char *
arp_sprintf(a, hwaddr, len)
	register char *a;
	register u_char *hwaddr;
	register int len;
{
	register int i, j;
	CONST static char tohex[] = "0123456789ABCDEF";

	for (i = j = 0; i < len; ++i) {
		a[j++] = tohex[hwaddr[i] >> 4];
		a[j++] = tohex[hwaddr[i] & 0x0f];
		a[j++] = ':';
	}
	if (i == 0) {
		a[0] = '?'; a[1] = '?'; a[2] = '?';
		j = 4;
	}
	a[j-1] = '\0';
	return a;
}

/*
 * Called as softnet_intr routine when ARP packets received.
 * Common length and type checks are done here,
 * then the protocol-specific routine is called.
 */
void
arpintr()
{
	register struct mbuf *m;
	int s;

	for (;;) {
		s = splimp();
		IF_DEQUEUE(&arpintrq, m);
		splx(s);
		if (m == 0)
			return;
if ((m->m_flags & M_PKTHDR) == 0)
panic("arpintr no HDR");
		arpinput((struct arpcom *)m->m_pkthdr.rcvif, m);
	}
}

void
arpinput(ac, m)
	struct arpcom *ac;
	struct mbuf *m;
{
	register struct arphdr *ar;

	if (ac->ac_if.if_flags & IFF_NOARP)
		goto out;
	if (m->m_len < sizeof(struct arphdr))
		goto out;
	ar = mtod(m, struct arphdr *);
	if (m->m_len < sizeof(struct arphdr) + 2 * ar->ar_hln + 2 * ar->ar_pln)
		goto out;

	switch (ntohs(ar->ar_pro)) {

	case ETHERTYPE_IPTRAILERS:
		/* Ignore trailers in BOTH directions if disabled */
		if (ac->ac_if.if_flags & IFF_NOTRAILERS)
			break;
		/* FALL THROUGH */
	case ETHERTYPE_IP:
		in_arpinput(ac, m);
		return;

	default:
		break;
	}
out:
	m_freem(m);
}

/*
 * ARP for Internet protocols on (e.g. 10 Mb/s Ethernet).
 * Algorithm is that given in RFC 826.
 * In addition, a sanity check is performed on the sender
 * protocol address, to catch impersonators.
 * We also handle negotiations for use of trailer protocol:
 * ARP replies for protocol type ETHERTYPE_TRAIL are sent
 * along with IP replies if we want trailers sent to us,
 * and also send them in response to IP replies.
 * This allows either end to announce the desire to receive
 * trailer packets.
 * We reply to requests for ETHERTYPE_TRAIL protocol as well,
 * but don't normally send requests.
 */
void
in_arpinput(ac, m)
	register struct arpcom *ac;
	struct mbuf *m;
{
	register struct arphdr *ah;
	register struct arptab *at;
	register struct in_ifaddr *ia;
	struct in_ifaddr *maybe_ia = 0;
	struct mbuf *mcopy = 0;
	struct in_addr isaddr, itaddr, myaddr;
	int proto, op, completed = 0;
	NETSPL_DECL(s)

	ah = mtod(m, struct arphdr *);
	if (ah->ar_hln != ac->ac_if.if_addrlen ||
	    ah->ar_pln != sizeof (itaddr) ||
	    ntohs(ah->ar_hrd) != ac->ac_arphrd) /* XXX allow a list */
		goto out;
	proto = ntohs(ah->ar_pro);
	op = ntohs(ah->ar_op);
	bcopy((caddr_t)AR_SPA(ah), (caddr_t)&isaddr, sizeof (isaddr));
	bcopy((caddr_t)AR_TPA(ah), (caddr_t)&itaddr, sizeof (itaddr));
	for (ia = in_ifaddr; ia; ia = ia->ia_next)
		if (ia->ia_ifp == &ac->ac_if) {
			maybe_ia = ia;
			if ((itaddr.s_addr == ia->ia_addr.sin_addr.s_addr) ||
			     (isaddr.s_addr == ia->ia_addr.sin_addr.s_addr))
				break;
		}
	if (maybe_ia == 0)
		goto out;
	myaddr = ia ? ia->ia_addr.sin_addr : maybe_ia->ia_addr.sin_addr;
	if (!bcmp((caddr_t)AR_SHA(ah), (caddr_t)ac->ac_hwaddr, (int)ah->ar_hln))
		goto out;	/* it's from me, or published, ignore it. */
	/* Reject reply and log message if hwaddr == broadcast */
	if (ac->ac_bcastaddr &&
	    !bcmp((caddr_t)AR_SHA(ah), ac->ac_bcastaddr, (int)ah->ar_hln)) {
		log(LOG_ERR,
		    "arp: hardware address is broadcast for IP address 0x%x!\n",
			    ntohl(isaddr.s_addr));
		goto out;
	}
	/* Log a message if someone else reponds with our IP address */
	if (isaddr.s_addr == myaddr.s_addr) {
		char a[2 * sizeof ac->ac_hwaddr + sizeof ac->ac_hwaddr + 1];
		log(LOG_ERR,
		   "arp: local IP address 0x%x in use by hardware address %s\n",
			ntohl(isaddr.s_addr),
			arp_sprintf(a, AR_SHA(ah), (int)ah->ar_hln));
		itaddr = myaddr;
		if (op == ARPOP_REQUEST)
			goto reply;
		goto out;
	}
	ARP_LOCK();
	ARPTAB_LOOK(at, isaddr.s_addr, &ac->ac_if);
	if (at) {
		bcopy((caddr_t)AR_SHA(ah), (caddr_t)at->at_hwaddr,
		    (int)ah->ar_hln);
		if ((at->at_flags & ATF_COM) == 0)
			completed = 1;
		at->at_flags |= ATF_COM;
		at->at_flags &= ~(ATF_STALE|ATF_DEAD);
		at->at_retry = at->at_valid = at->at_sent = 0;
		if (at->at_hold) {
			struct sockaddr_in sin;
			struct mbuf *om = at->at_hold;
			at->at_hold = 0;
			ARP_UNLOCK();
			sin.sin_family = AF_INET;
			sin.sin_len = sizeof(sin);
			sin.sin_addr = isaddr;
			(*ac->ac_if.if_output)(&ac->ac_if, 
			    om, (struct sockaddr *)&sin, (struct rtentry *)0);
			goto reply;
		}
	} else if (itaddr.s_addr == myaddr.s_addr) {
		/* ensure we have a table entry */
		if (at = arptnew(&isaddr, &ac->ac_if)) {
			bcopy((caddr_t)AR_SHA(ah), (caddr_t)at->at_hwaddr,
			    (int)ah->ar_hln);
			completed = 1;
			at->at_flags |= ATF_COM;
		}
	}
	ARP_UNLOCK();
reply:
	switch (proto) {

	case ETHERTYPE_IPTRAILERS:
		/* partner says trailers are OK */
		if (at) {
			ARP_LOCK();
			/* Must recheck in case table changed */
			if (at->at_if == &ac->ac_if &&
			    at->at_iaddr.s_addr == isaddr.s_addr)
				at->at_flags |= ATF_USETRAILERS;
			ARP_UNLOCK();
		}
		/*
		 * Reply to request iff we want trailers.
		 */
		if (op != ARPOP_REQUEST || ac->ac_if.if_flags & IFF_NOTRAILERS)
			goto out;
		break;

	case ETHERTYPE_IP:
		/*
		 * Reply if this is an IP request,
		 * or if we want to send a trailer response.
		 * Send the latter only to the IP response
		 * that completes the current ARP entry.
		 */
		if (op != ARPOP_REQUEST &&
		    (completed == 0 || ac->ac_if.if_flags & IFF_NOTRAILERS))
			goto out;
	}
	if (itaddr.s_addr == myaddr.s_addr) {
		/* I am the target */
		bcopy((caddr_t)AR_SHA(ah), (caddr_t)AR_THA(ah),
		    (int)ah->ar_hln);
		bcopy((caddr_t)ac->ac_hwaddr, (caddr_t)AR_SHA(ah),
		    (int)ah->ar_hln);
	} else if (anypublished) {
		ARP_LOCK();
		ARPTAB_LOOK(at, itaddr.s_addr, &ac->ac_if);
		if (at == NULL || (at->at_flags & ATF_PUBL) == 0) {
			ARP_UNLOCK();
			goto out;
		}
		bcopy((caddr_t)AR_SHA(ah), (caddr_t)AR_THA(ah),
		    (int)ah->ar_hln);
		bcopy((caddr_t)at->at_hwaddr, (caddr_t)AR_SHA(ah),
		    (int)ah->ar_hln);
		ARP_UNLOCK();
	} else
		goto out;

	m->m_pkthdr.len = m->m_len =
	    sizeof(*ah) + (2*(int)ah->ar_hln) + sizeof(isaddr) + sizeof(itaddr);
	bcopy((caddr_t)AR_SPA(ah), (caddr_t)AR_TPA(ah), sizeof(isaddr));
	bcopy((caddr_t)&itaddr, (caddr_t)AR_SPA(ah), sizeof(itaddr));
	ah->ar_op = htons(ARPOP_REPLY); 
	/*
	 * If incoming packet was an IP reply,
	 * we are sending a reply for type IPTRAILERS.
	 * If we are sending a reply for type IP
	 * and we want to receive trailers,
	 * send a trailer reply as well.
	 */
	if (op == ARPOP_REPLY)
		ah->ar_pro = htons(ETHERTYPE_IPTRAILERS);
	else if (proto == ETHERTYPE_IP &&
	    (ac->ac_if.if_flags & IFF_NOTRAILERS) == 0)
		mcopy = m_copym(m, 0, (int)M_COPYALL, M_DONTWAIT);
	arpoutput(ac, m, (u_char *)AR_THA(ah), ntohs(ah->ar_hrd));
	if (mcopy) {
		ah = mtod(mcopy, struct arphdr *);
		ah->ar_pro = htons(ETHERTYPE_IPTRAILERS);
		arpoutput(ac, mcopy, (u_char *)AR_THA(ah), ntohs(ah->ar_hrd));
	}
	return;

out:
	m_freem(m);
	return;
}

/*
 * ARP output. This code should be per-interface.
 */
void
#ifndef _NO_PROTO
arpoutput(
	struct arpcom *ac,
	struct mbuf *m,
	u_char *dest,
	u_short arphrd)
#else
arpoutput(ac, m, dest, arphrd)
	struct arpcom *ac;
	struct mbuf *m;
	u_char *dest;
	u_short arphrd;
#endif
{
	struct sockaddr sa;

	switch (arphrd) {
case 0:				/*XXX*/
arphrd = ARPHRD_ETHER;
	case ARPHRD_802:	/*XXX - needs per-host semantic*/
	case ARPHRD_ETHER: {
		struct ether_header *eh = (struct ether_header *)sa.sa_data;
		sa.sa_family = AF_UNSPEC;
		sa.sa_len = sizeof(sa);
if (dest == NULL) dest = (u_char *)etherbroadcastaddr; /* XXX */
		bzero((caddr_t)eh->ether_shost, sizeof (eh->ether_shost));
		bcopy(dest, (caddr_t)eh->ether_dhost, sizeof(eh->ether_dhost));
		eh->ether_type = ETHERTYPE_ARP;	/* ether_output will swap */
		break;
	}
	default:
		printf("arpoutput: can't handle type %d\n", (int)arphrd);
		m_freem(m);
		return;
	}
	mtod(m, struct arphdr *)->ar_hrd = htons(arphrd);
	(*ac->ac_if.if_output)(&ac->ac_if, m, &sa, (struct rtentry *)0);
}

/*
 * Free an arptab entry.
 * Must be called at splnet and/or ARP_LOCK held.
 */
void
arptfree(at)
	register struct arptab *at;
{
	if (at->at_hold)
		m_freem(at->at_hold);
	bzero((caddr_t)at, sizeof *at);
}

/*
 * Enter a new address in arptab, pushing out the oldest entry 
 * from the bucket if there is no room.
 * This always succeeds since no bucket can be completely filled
 * with permanent entries (except from arpioctl when testing whether
 * another permanent entry will fit).
 * MUST BE CALLED AT SPLNET (and/or) WITH ARP_LOCK HELD.
 */
struct arptab *
arptnew(addr, ifp)
	struct in_addr *addr;
	struct ifnet *ifp;
{
	register n;
	int oldest = -1;
	register struct arptab *at, *ato = NULL;

	at = &arptab[ARPTAB_HASH(addr->s_addr) * arptab_bsiz];
	for (n = 0; n < arptab_bsiz; n++,at++) {
		if (at->at_flags == 0)
			goto out;	 /* found an empty entry */
		if (at->at_flags & ATF_PERM)
			continue;
		if ((int) at->at_timer > oldest) {
			oldest = at->at_timer;
			ato = at;
		}
	}
	if (ato == NULL)
		return (NULL);
	at = ato;
	arptfree(at);
out:
	at->at_if = ifp;
	at->at_iaddr = *addr;
	at->at_flags = ATF_INUSE;
	return (at);
}

static int
arpioctl_local(cmd, data)
	int cmd;
	caddr_t data;
{
	register struct arpreq *ar = (struct arpreq *)data;
	register struct arptab *at;
	register struct sockaddr_in *sin;
	struct ifaddr *ifa;
	NETSPL_DECL(s)

	sin = (struct sockaddr_in *)&ar->arp_ha;
#if defined(COMPAT_43) && BYTE_ORDER != BIG_ENDIAN
	if (sin->sin_family == 0 && sin->sin_len < 16)
		sin->sin_family = sin->sin_len;
#endif
	sin->sin_len = sizeof(ar->arp_ha);
	sin = (struct sockaddr_in *)&ar->arp_pa;
#if defined(COMPAT_43) && BYTE_ORDER != BIG_ENDIAN
	if (sin->sin_family == 0 && sin->sin_len < 16)
		sin->sin_family = sin->sin_len;
#endif
	sin->sin_len = sizeof(ar->arp_pa);
	if (ar->arp_pa.sa_family != AF_INET ||
	    ar->arp_ha.sa_family != AF_UNSPEC)
		return (EAFNOSUPPORT);
	ARP_LOCK();
	/* Tricky - don't know ifp, so match any - XXX use ifa_withnet? */
	ARPTAB_LOOK(at, sin->sin_addr.s_addr, at->at_if);
	if (at == NULL) {		/* not found */
		if (cmd != SIOCSARP) {
			ARP_UNLOCK();
			return (ENXIO);
		}
		if ((ifa = ifa_ifwithnet(&ar->arp_pa)) == NULL) {
			ARP_UNLOCK();
			return (ENETUNREACH);
		}
	}
	switch (cmd) {

	case SIOCSARP:		/* set entry */
		if (at == NULL) {
			at = arptnew(&sin->sin_addr, ifa->ifa_ifp);
			if (at == NULL) {
				ARP_UNLOCK();
				return (EADDRNOTAVAIL);
			}
			if (ar->arp_flags & ATF_PERM) {
			/* never make all entries in a bucket permanent */
				register struct arptab *tat;
				
				/* try to re-allocate */
				tat = arptnew(&sin->sin_addr, ifa->ifa_ifp);
				if (tat == NULL) {
					arptfree(at);
					ARP_UNLOCK();
					return (EADDRNOTAVAIL);
				}
				arptfree(tat);
			}
		}
		bcopy((caddr_t)ar->arp_ha.sa_data, (caddr_t)at->at_hwaddr,
		    sizeof (at->at_hwaddr));
		at->at_flags = ATF_COM | ATF_INUSE |
			(ar->arp_flags & ~ATF_CANTCHANGE);
		if (at->at_flags & ATF_PUBL)
			anypublished = 1;
		break;

	case SIOCDARP:		/* delete entry */
		arptfree(at);
		break;

	case SIOCGARP:		/* get entry */
#ifdef	OSIOCGARP
	case OSIOCGARP:
#endif
		bcopy((caddr_t)at->at_hwaddr, (caddr_t)ar->arp_ha.sa_data,
		    sizeof (ar->arp_ha.sa_data));
#if	defined(COMPAT_43) && defined(OSIOCGARP)
		if (cmd == OSIOCGARP)
			((struct osockaddr *)&ar->arp_ha)->sa_family =
							ar->arp_ha.sa_family;
#endif
		ar->arp_flags = at->at_flags;
		break;
	}
	ARP_UNLOCK();
	return (0);
}

void
arpinit()
{
	/* Stubs for dynamic attach - in net/if_ethersubr.c */
	extern void (*arpreq)();
	extern int  (*arpres)();
	extern int  (*arpctl)();

	ARP_LOCKINIT();
	if (!arptab)
		arptab = arptabXXX;
 	if (arptab_bsiz <= 0)
		arptab_bsiz = (ipgateway ? 16 : 9);
 	if (arptab_nb <= 0)
		arptab_nb = (ipgateway ? 37 : 19);
	arptab_size = arptab_bsiz * arptab_nb;
	if (arptab == arptabXXX &&
	    sizeof (struct arptab) * arptab_size > sizeof arptabXXX)
		panic("arpinit");
	bzero((caddr_t)arptab, sizeof (struct arptab) * arptab_size);
	IFQ_LOCKINIT(&arpintrq);
	arpintrq.ifq_maxlen = arpqmaxlen;
	arpctl = arpioctl_local;
	arpres = arpresolve_local;
	arpreq = arprequest;
	(void) netisr_add(NETISR_ARP, arpintr, &arpintrq, &inetdomain);
#if	!NETISR_THREAD
	arptimer();
#else
	NET_THREADSTART(arptimer, 0, "arptimer");
#endif
}

