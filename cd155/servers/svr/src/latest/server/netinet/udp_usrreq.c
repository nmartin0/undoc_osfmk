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
 * $Log: udp_usrreq.c,v $
 * Revision 1.4  1994/11/18  20:35:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:27:35  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:34  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:30:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:17  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:28  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/03/09  14:45:11  durriya
 * 	92/02/13  17:09:25  david
 * 	re-enabled socket locked ASSERT turned off by condict
 *
 * Revision 2.3  91/12/16  19:20:45  roy
 * 	91/12/11  15:12:13  condict
 * 	Turn off the ASSERT for unlocked sockets -- AFS doesn't lock sockets.
 * 
 * Revision 2.2  91/08/31  13:45:11  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:37:43  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.14  90/10/07  14:36:38  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:17:32  gm]
 * 
 * Revision 1.13  90/09/23  15:56:02  devrcs
 * 	Fix icmp_error call arguments.
 * 	[90/09/05  17:32:03  tmt]
 * 
 * Revision 1.12  90/08/09  13:26:54  devrcs
 * 	Do last pcb cache with help from in_pcblookup.
 * 	[90/07/24  14:15:14  tmt]
 * 
 * Revision 1.11  90/07/27  09:02:38  devrcs
 * 	Update to BSD Reno release.
 * 	Rearrange udp_output, etc.
 * 	[90/07/19  17:46:28  tmt]
 * 
 * Revision 1.10  90/07/05  23:13:33  devrcs
 * 	Change SPL macros to NETSPL.
 * 	[90/07/03  18:56:49  tmt]
 * 
 * Revision 1.9  90/06/29  13:37:15  devrcs
 * 	Make security contingent on MACH for compat.
 * 	[90/06/26  20:02:34  tmt]
 * 
 * Revision 1.8  90/06/22  20:39:40  devrcs
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:45:10  seiden]
 * 
 * 	Parallelization repairs. Socket locks, then inpcb locks.
 * 	Take inpcbhead locks directly. Do refcounting right.
 * 	[90/06/07  16:17:14  tmt]
 * 
 * Revision 1.7  90/04/27  19:20:06  devrcs
 * 	Change code to handle new _NO_BITFIELDS ip header.
 * 	[90/04/20  13:08:06  tmt]
 * 
 * Revision 1.6  90/04/14  00:33:42  devrcs
 * 	Add void's, delete a function decl.
 * 	[90/04/09  16:36:04  tmt]
 * 
 * Revision 1.5  90/02/05  15:51:01  robert
 * 	Use macros for _islocked.
 * 	[90/01/19  15:09:55  tmt]
 * 
 * Revision 1.4  90/01/18  08:48:27  gm
 * 	Update INPCB_LOCK macro arguments.
 * 	Do UDP_READ_UNLOCK in usrreq if no inp!
 * 	Update in_pcbnotify calls (back to original thanks to in_pcb.c and locks).
 * 	Don't SOCKET_LOCK after in_pcbdetach (old code).
 * 	[90/01/08  16:15:27  tmt]
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
 * Revision 1.3  90/01/03  12:42:14  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:33  gm]
 * 
 * Revision 1.2  89/12/26  10:17:28  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)udp_usrreq.c	2.1 16:12:29 4/20/90 SecureWare */
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
 * Copyright (c) 1982, 1986, 1988, 1990 Regents of the University of California.
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
 *	Merged:	udp_usrreq.c	7.17 (Berkeley) 7/1/90
 */

#include "net/net_globals.h"
#if	MACH
#include <sys/secdefines.h>
#endif

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"
#include "sys/stat.h"

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
#include "netinet/ip_var.h"
#include "netinet/ip_icmp.h"
#include "netinet/udp.h"
#include "netinet/udp_var.h"

#include "net/net_malloc.h"

LOCK_ASSERTL_DECL

struct	inpcb udb;
struct	udpstat udpstat;

/*
 * UDP protocol implementation.
 * Per RFC 768, August, 1980.
 */
void
udp_init()
{

	udb.inp_next = udb.inp_prev = &udb;
	INPCBRC_LOCKINIT(&udb);		/* in_pcbnotify frobs */
	udb.inp_refcnt = 2;
	INHEAD_LOCKINIT(&udb);
	NETSTAT_LOCKINIT(&udpstat.udps_lock);
}

int	udpcksum = 1;
int	udp_ttl = UDP_TTL;

#if	!NETISR_THREAD
struct	sockaddr_in udp_in = { sizeof(udp_in), AF_INET };
#endif

void
udp_input(m, iphlen)
	register struct mbuf *m;
	int iphlen;
{
	register struct ip *ip;
	register struct udphdr *uh;
	register struct inpcb *inp;
	register struct socket *so;
	struct mbuf *opts = 0;
	int len;
	struct ip save_ip;
#if	NETISR_THREAD
	extern CONST struct sockaddr_in in_zeroaddr;
	struct sockaddr_in udp_in;
#endif

	NETSTAT_LOCK(&udpstat.udps_lock);
	udpstat.udps_ipackets++;
	NETSTAT_UNLOCK(&udpstat.udps_lock);
	/*
	 * Strip IP options, if any; should skip this,
	 * make available to user, and use on returned packets,
	 * but we don't yet have a way to check the checksum
	 * with options still present.
	 */
	if (iphlen > sizeof (struct ip)) {
		ip_stripoptions(m, (struct mbuf *)0);
		iphlen = sizeof(struct ip);
	}

	/*
	 * Get IP and UDP header together in first mbuf.
	 */
	ip = mtod(m, struct ip *);
	if (m->m_len < iphlen + sizeof(struct udphdr)) {
		if ((m = m_pullup(m, iphlen + sizeof(struct udphdr))) == 0) {
			NETSTAT_LOCK(&udpstat.udps_lock);
			udpstat.udps_hdrops++;
			NETSTAT_UNLOCK(&udpstat.udps_lock);
			return;
		}
		ip = mtod(m, struct ip *);
	}
	uh = (struct udphdr *)((caddr_t)ip + iphlen);

	/*
	 * Make mbuf data length reflect UDP length.
	 * If not enough data to reflect UDP length, drop.
	 */
	len = ntohs((u_short)uh->uh_ulen);
	if (ip->ip_len != len) {
		if (len > (int)ip->ip_len) {
			NETSTAT_LOCK(&udpstat.udps_lock);
			udpstat.udps_badlen++;
			NETSTAT_UNLOCK(&udpstat.udps_lock);
			goto bad;
		}
		m_adj(m, len - (int)ip->ip_len);
		/* ip->ip_len = len; */
	}
	/*
	 * Save a copy of the IP header in case we want to restore it
	 * for sending an ICMP error message in response.
	 */
	save_ip = *ip;

	/*
	 * Checksum extended UDP header and data.
	 */
	if (udpcksum && uh->uh_sum) {
		((struct ipovly *)ip)->ih_next = 0;
		((struct ipovly *)ip)->ih_prev = 0;
		((struct ipovly *)ip)->ih_x1 = 0;
		((struct ipovly *)ip)->ih_len = uh->uh_ulen;
		if (uh->uh_sum = in_cksum(m, len + sizeof (struct ip))) {
			NETSTAT_LOCK(&udpstat.udps_lock);
			udpstat.udps_badsum++;
			NETSTAT_UNLOCK(&udpstat.udps_lock);
			goto bad;
		}
	}

	/*
	 * Locate pcb for datagram.
	 *
	 * BSD "Reno" caches the previous pcb looked up here. This is
	 * problematic with parallelization and thread safety, due to
	 * the synchronization required, and taking/releasing refcounts,
	 * so we push it down to in_pcblookup (where it belongs?).
	 */
	inp = in_pcblookup(&udb, ip->ip_src, uh->uh_sport,
	    ip->ip_dst, uh->uh_dport, INPLOOKUP_WILDCARD|INPLOOKUP_USECACHE);
	if (inp == 0) {
		/* don't send ICMP response for broadcast packet */
		NETSTAT_LOCK(&udpstat.udps_lock);
		udpstat.udps_noport++;
		NETSTAT_UNLOCK(&udpstat.udps_lock);
		if (m_broadcast(m)) {
			NETSTAT_LOCK(&udpstat.udps_lock);
			udpstat.udps_noportbcast++;
			NETSTAT_UNLOCK(&udpstat.udps_lock);
			goto bad;
		}
		*ip = save_ip;
		ip->ip_len += iphlen;
		icmp_error(m, ICMP_UNREACH, ICMP_UNREACH_PORT, zeroin_addr);
		return;
	}

	/*
	 * Construct sockaddr format source address.
	 * Stuff source address and datagram in user buffer.
	 */
#if	NETISR_THREAD
	udp_in = in_zeroaddr;
#endif
	udp_in.sin_port = uh->uh_sport;
	udp_in.sin_addr = ip->ip_src;
	if (inp->inp_flags & INP_CONTROLOPTS) {
		struct mbuf **mp = &opts;

		if (inp->inp_flags & INP_RECVDSTADDR) {
			*mp = udp_saveopt((caddr_t) &ip->ip_dst,
			    sizeof(struct in_addr), IP_RECVDSTADDR);
			if (*mp)
				mp = &(*mp)->m_next;
		}
#ifdef notyet
		/* options were tossed above */
		if (inp->inp_flags & INP_RECVOPTS) {
			*mp = udp_saveopt((caddr_t) opts_deleted_above,
			    sizeof(struct in_addr), IP_RECVOPTS);
			if (*mp)
				mp = &(*mp)->m_next;
		}
		/* ip_srcroute doesn't do what we want here, need to fix */
		if (inp->inp_flags & INP_RECVRETOPTS) {
			*mp = udp_saveopt((caddr_t) ip_srcroute(),
			    sizeof(struct in_addr), IP_RECVRETOPTS);
			if (*mp)
				mp = &(*mp)->m_next;
		}
#endif
	}
	iphlen += sizeof(struct udphdr);
	m->m_len -= iphlen;
	m->m_pkthdr.len -= iphlen;
	m->m_data += iphlen;
	so = inp->inp_socket;
	SOCKET_LOCK(so);
	INPCBRC_UNREF(inp);	/* Not necessary to INPCB_LOCK(inp); */
	SOCKBUF_LOCK(&so->so_rcv);
	if (sbappendaddr(&so->so_rcv, (struct sockaddr *)&udp_in,
	    m, opts) == 0) {
		SOCKBUF_UNLOCK(&so->so_rcv);
		SOCKET_UNLOCK(so);
		NETSTAT_LOCK(&udpstat.udps_lock);
		udpstat.udps_fullsock++;
		NETSTAT_UNLOCK(&udpstat.udps_lock);
		goto bad;
	}
	SOCKBUF_UNLOCK(&so->so_rcv);
	NETSTAT_LOCK(&udpstat.udps_lock);
	udpstat.udps_ipackets++;
	NETSTAT_UNLOCK(&udpstat.udps_lock);
	sorwakeup(so);
	SOCKET_UNLOCK(so);
	return;
bad:
	m_freem(m);
	if (opts)
		m_freem(opts);
}

/*
 * Create a "control" mbuf containing the specified data
 * with the specified type for presentation with a datagram.
 */
struct mbuf *
udp_saveopt(p, size, type)
	caddr_t p;
	register int size;
	int type;
{
	register struct cmsghdr *cp;
	struct mbuf *m;

	if ((m = m_get(M_DONTWAIT, MT_CONTROL)) == NULL)
		return ((struct mbuf *) NULL);
	cp = (struct cmsghdr *) mtod(m, struct cmsghdr *);
	bcopy(p, (caddr_t)cp + size, size);
	size += sizeof(*cp);
	m->m_len = size;
	cp->cmsg_len = size;
	cp->cmsg_level = IPPROTO_IP;
	cp->cmsg_type = type;
	return (m);
}

/*
 * Notify a udp user of an asynchronous error;
 * just wake up so that it can collect error status.
 */
void
udp_notify(inp, errno)
	struct inpcb *inp;
{
	register struct socket *so = inp->inp_socket;

	LOCK_ASSERT("udp_notify so", SOCKET_ISLOCKED(so));
	so->so_error = errno;
	sorwakeup(so);
	sowwakeup(so);
}

void
udp_ctlinput(cmd, sa, ip)
	int cmd;
	struct sockaddr *sa;
	register struct ip *ip;
{
	register struct udphdr *uh;

	if ((unsigned)cmd > PRC_NCMDS || inetctlerrmap[cmd] == 0)
		return;
	if (ip) {
		uh = (struct udphdr *)((caddr_t)ip + ((ip->ip_vhl&0x0f) << 2));
		in_pcbnotify(&udb, sa, uh->uh_dport, ip->ip_src, uh->uh_sport,
			cmd, udp_notify);
	} else
		in_pcbnotify(&udb, sa, 0, zeroin_addr, 0, cmd, udp_notify);
}

udp_output(inp, m, addr, control)
	register struct inpcb *inp;
	register struct mbuf *m;
	struct mbuf *addr, *control;
{
	register struct udpiphdr *ui;
	register int len = m->m_pkthdr.len;
	struct in_addr laddr;
	int error = 0;
	NETSPL_DECL(s)

	LOCK_ASSERT("udp_output", INPCB_ISLOCKED(inp));
	if (control)
		m_freem(control);		/* XXX */

	if (addr) {
		laddr = inp->inp_laddr;
		if (inp->inp_faddr.s_addr != INADDR_ANY) {
			error = EISCONN;
			goto release;
		}
		/*
		 * Must block input while temporarily connected.
		 */
		NETSPL(s,net);
		INHEAD_WRITE_LOCK(&udb);
		error = in_pcbconnect(inp, addr);
		if (error) {
			m_freem(m);
			goto nosend;
		}
	} else {
		if (inp->inp_faddr.s_addr == INADDR_ANY) {
			error = ENOTCONN;
			goto release;
		}
	}
	/*
	 * Calculate data length and get a mbuf
	 * for UDP and IP headers.
	 */
	M_PREPEND(m, sizeof(struct udpiphdr), M_WAIT);

	/*
	 * Fill in mbuf with extended UDP header
	 * and addresses and length put into network format.
	 */
	ui = mtod(m, struct udpiphdr *);
	ui->ui_next = ui->ui_prev = 0;
	ui->ui_x1 = 0;
	ui->ui_pr = IPPROTO_UDP;
	ui->ui_len = htons((u_short)len + sizeof (struct udphdr));
	ui->ui_src = inp->inp_laddr;
	ui->ui_dst = inp->inp_faddr;
	ui->ui_sport = inp->inp_lport;
	ui->ui_dport = inp->inp_fport;
	ui->ui_ulen = ui->ui_len;

	/*
	 * Stuff checksum and output datagram.
	 */
	ui->ui_sum = 0;
	if (udpcksum) {
	    if ((ui->ui_sum = in_cksum(m, sizeof (struct udpiphdr) + len)) == 0)
		ui->ui_sum = 0xffff;
	}
	((struct ip *)ui)->ip_len = sizeof (struct udpiphdr) + len;
	((struct ip *)ui)->ip_ttl = inp->inp_ip.ip_ttl;	/* XXX */
	((struct ip *)ui)->ip_tos = inp->inp_ip.ip_tos;	/* XXX */
	NETSTAT_LOCK(&udpstat.udps_lock);
	udpstat.udps_opackets++;
	NETSTAT_UNLOCK(&udpstat.udps_lock);
	error = ip_output(m, inp->inp_options, &inp->inp_route,
	    inp->inp_socket->so_options & (SO_DONTROUTE | SO_BROADCAST));

	if (addr) {
		in_pcbdisconnect(inp);
		inp->inp_laddr = laddr;
nosend:
		INHEAD_WRITE_UNLOCK(&udb);
		NETSPLX(s);
	}
	return (error);

release:
	m_freem(m);
	return (error);
}

u_long	udp_sendspace = 9216;		/* really max datagram size */
u_long	udp_recvspace = 40 * (1024 + sizeof(struct sockaddr_in));
					/* 40 1K datagrams */

/*ARGSUSED*/
udp_usrreq(so, req, m, addr, control)
	struct socket *so;
	int req;
	struct mbuf *m, *addr, *control;
{
	struct inpcb *inp = sotoinpcb(so);
	int error = 0;

	LOCK_ASSERT("udp_usrreq", SOCKET_ISLOCKED(so));
	if (req == PRU_CONTROL)
		return (in_control(so, (int)m, (caddr_t)addr,
			(struct ifnet *)control));
#if	!SEC_ARCH
	if (control && control->m_len) {
		error = EINVAL;
		goto release;
	}
#endif
	if (inp) {
		INPCB_LOCK(inp);
	} else if (req != PRU_ATTACH) {
		error = EINVAL;
		goto release;
	}

	switch (req) {

	case PRU_ATTACH:
		if (inp != NULL) {
			error = EINVAL;
			break;
		}
		error = in_pcballoc(so, &udb);
		if (error)
			break;
		error = soreserve(so, udp_sendspace, udp_recvspace);
		if (error)
			break;
		((struct inpcb *) so->so_pcb)->inp_ip.ip_ttl = udp_ttl;
		break;

	case PRU_DETACH:
		in_pcbdetach(inp);
		inp = 0;
		break;

	case PRU_BIND:
		error = in_pcbbind(inp, addr);
		break;

	case PRU_LISTEN:
		error = EOPNOTSUPP;
		break;

	case PRU_CONNECT:
		if (inp->inp_faddr.s_addr != INADDR_ANY) {
			error = EISCONN;
			break;
		}
		error = in_pcbconnect(inp, addr);
		if (error == 0)
			soisconnected(so);
		break;

	case PRU_CONNECT2:
		error = EOPNOTSUPP;
		break;

	case PRU_ACCEPT:
		error = EOPNOTSUPP;
		break;

	case PRU_DISCONNECT:
		if (inp->inp_faddr.s_addr == INADDR_ANY) {
			error = ENOTCONN;
			break;
		}
		in_pcbdisconnect(inp);
		inp->inp_laddr.s_addr = INADDR_ANY;
		so->so_state &= ~SS_ISCONNECTED;		/* XXX */
		break;

	case PRU_SHUTDOWN:
		socantsendmore(so);
		break;

	case PRU_SEND:
		error = udp_output(inp, m, addr, control);
		m = 0;
		control = 0;
		break;

	case PRU_ABORT:
		soisdisconnected(so);
		in_pcbdetach(inp);
		inp = 0;
		break;

	case PRU_SOCKADDR:
		in_setsockaddr(inp, addr);
		break;

	case PRU_PEERADDR:
		in_setpeeraddr(inp, addr);
		break;

	case PRU_SENSE:
		/*
		 * stat: don't bother with a blocksize.
		 */
		INPCB_UNLOCK(inp);
		return (0);

	case PRU_SENDOOB:
	case PRU_FASTTIMO:
	case PRU_SLOWTIMO:
	case PRU_PROTORCV:
	case PRU_PROTOSEND:
		error =  EOPNOTSUPP;
		break;

	case PRU_RCVD:
	case PRU_RCVOOB:
		INPCB_UNLOCK(inp);
		return (EOPNOTSUPP);	/* do not free mbuf's */

	default:
		panic("udp_usrreq");
	}
	if (inp)
		INPCB_UNLOCK(inp);
release:
	if (control != NULL)
		m_freem(control);
	if (m != NULL)
		m_freem(m);
	return (error);
}
