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
 * $Log: tcp_output.c,v $
 * Revision 1.4  1994/11/18  20:35:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:27:10  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:18  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:41  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:21:56  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/10/22  23:18:27  hobbes
 * Added the RFC_1323 extensions.
 *
 * Revision 2.3  1992/03/09  14:43:26  durriya
 * 	91/12/18  17:17:07  sp
 * 	Include sys/synch.h to get spl macros
 *
 * Revision 2.2  91/08/31  13:44:29  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:36:56  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11.4.2  91/04/29  12:10:29  tmt
 * 	Avoid roundoff error in peer send buffer calculation.
 * 	[91/04/10  09:54:21  tmt]
 * 
 * Revision 1.11  90/10/07  14:35:52  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:16:28  gm]
 * 
 * Revision 1.10  90/09/13  11:48:20  devrcs
 * 	Fix unsigned comparison problem in window update decision.
 * 	[90/08/21  16:26:14  tmt]
 * 
 * Revision 1.9  90/07/27  09:02:01  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:38:59  tmt]
 * 
 * Revision 1.8  90/05/13  18:45:01  devrcs
 * 	Add test code to handle small peer send buf/large local recv buf.
 * 	[90/05/05  18:06:26  tmt]
 * 
 * Revision 1.7  90/04/27  19:19:08  devrcs
 * 	Change code to handle new _NO_BITFIELDS tcp header.
 * 	[90/04/20  13:03:09  tmt]
 * 
 * Revision 1.6  90/04/14  00:33:33  devrcs
 * 	Add a void.
 * 	[90/04/09  16:34:46  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:52  robert
 * 	Use macros for _islocked.
 * 	[90/01/19  15:06:00  tmt]
 * 
 * Revision 1.4  90/01/18  08:47:53  gm
 * 	Evaluate tcp_mss only once in options.
 * 	[90/01/08  16:12:29  tmt]
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
 * Revision 1.3  90/01/03  12:42:00  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:16  gm]
 * 
 * Revision 1.2  89/12/26  10:15:21  gm
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
 *	Base:	tcp_output.c	7.18 (Berkeley) 4/8/89
 *	Merged:	tcp_output.c	7.21 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/socketvar.h"
#include "sys/protosw.h"
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#include "net/route.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/in_pcb.h"
#include "netinet/ip_var.h"
#include "netinet/tcp.h"
#include "netinet/tcpip.h"
#define	TCPOUTFLAGS
#include "netinet/tcp_fsm.h"
#include "netinet/tcp_seq.h"
#include "netinet/tcp_timer.h"
#include "netinet/tcp_var.h"
#include "netinet/tcp_debug.h"

LOCK_ASSERTL_DECL

#if	RFC_1323
#define TCP_MAX_OPTIONSPC	32	/* most bytes we'll put in options */
#endif

/*
 * Tcp output routine: figure out what should be sent and send it.
 */
tcp_output(tp)
	register struct tcpcb *tp;
{
	register struct socket *so = tp->t_inpcb->inp_socket;
	register long len, win;
	int off, flags, error;
	register struct mbuf *m;
	register struct tcpiphdr *ti;
#if	RFC_1323
	u_char opt[ TCP_MAX_OPTIONSPC ];
#else
	u_char *opt;
#endif
	unsigned optlen, hdrlen;
	int idle, sendalot;
#if	RFC_1323
	extern int tcp_dont_sack;
#else
	struct { u_char opt[4]; } tcp_initopt;
		/* tcp_initopt = { TCPOPT_MAXSEG, 4, 0x0, 0x0, }; */
#endif
	LOCK_ASSERT("tcp_output inpcb", INPCB_ISLOCKED(tp->t_inpcb));
	LOCK_ASSERT("tcp_output so", SOCKET_ISLOCKED(so));
	/*
	 * Determine length of data that should be transmitted,
	 * and flags that will be used.
	 * If there is some data or critical controls (SYN, RST)
	 * to send, then transmit; otherwise, investigate further.
	 */
	idle = (tp->snd_max == tp->snd_una);
	if (idle && tp->t_idle >= tp->t_rxtcur)
		/*
		 * We have been idle for "a while" and no acks are
		 * expected to clock out any data we send --
		 * slow start to get ack "clock" running again.
		 */
		tp->snd_cwnd = tp->t_maxseg;
again:
	sendalot = 0;
	off = tp->snd_nxt - tp->snd_una;
	win = min(tp->snd_wnd, tp->snd_cwnd);

	/*
	 * If in persist timeout with window of 0, send 1 byte.
	 * Otherwise, if window is small but nonzero
	 * and timer expired, we will send what we can
	 * and go to transmit state.
	 */
	if (tp->t_force) {
		if (win == 0)
			win = 1;
		else {
			tp->t_timer[TCPT_PERSIST] = 0;
			tp->t_rxtshift = 0;
		}
	}

	SOCKBUF_LOCK(&so->so_snd);

	flags = tcp_outflags[tp->t_state];
	len = min(so->so_snd.sb_cc, win) - off;

	if (len < 0) {
		/*
		 * If FIN has been sent but not acked,
		 * but we haven't been called to retransmit,
		 * len will be -1.  Otherwise, window shrank
		 * after we sent into it.  If window shrank to 0,
		 * cancel pending retransmit and pull snd_nxt
		 * back to (closed) window.  We will enter persist
		 * state below.  If the window didn't close completely,
		 * just wait for an ACK.
		 */
		len = 0;
		if (win == 0) {
			tp->t_timer[TCPT_REXMT] = 0;
			tp->snd_nxt = tp->snd_una;
		}
	}
	if (len > tp->t_maxseg) {
		len = tp->t_maxseg;
		sendalot = 1;
	}
	if (SEQ_LT(tp->snd_nxt + len, tp->snd_una + so->so_snd.sb_cc))
		flags &= ~TH_FIN;

	win = sbspace(&so->so_rcv);

	/*
	 * Sender silly window avoidance.  If connection is idle
	 * and can send all data, a maximum segment,
	 * at least a maximum default-size segment do it,
	 * or are forced, do it; otherwise don't bother.
	 * If peer's buffer is tiny, then send
	 * when window is at least half open.
	 * If retransmitting (possibly after persist timer forced us
	 * to send into a small window), then must resend.
	 */
	if (len) {
		if (len == tp->t_maxseg)
			goto send;
		if ((idle || tp->t_flags & TF_NODELAY) &&
		    len + off >= so->so_snd.sb_cc)
			goto send;
		if (tp->t_force)
			goto send;
		if (len >= tp->max_sndwnd / 2)
			goto send;
		if (SEQ_LT(tp->snd_nxt, tp->snd_max))
			goto send;
	}

	/*
	 * Compare available window to amount of window
	 * known to peer (as advertised window less
	 * next expected input).  If the update might advance our
	 * peer's window, and the difference is at least two
	 * max size segments, or at least 50% of the maximum possible
	 * window, then want to send a window update to peer.
	 */
	if (win > 0) {
#if	RFC_1323
		/* adv is the amount we can increase the window,
		 * taking into account that we are limited by
		 * TCP_MAXWIN << tp->rcv_scale.
		 */
		long adv = min(win,(long)TCP_MAXWIN<<tp->rcv_scale) -
			(tp->rcv_adv - tp->rcv_nxt);
#else
		int adv = win - (tp->rcv_adv - tp->rcv_nxt);
#endif

		if (adv > 0) {	/* beware unsigned comparisons! */
			if (adv >= 2 * tp->t_maxseg)
				goto send;
			if (2 * adv >= so->so_rcv.sb_hiwat)
				goto send;
			/* If peer's send buffer is small, but not tiny! */
			if (tp->max_rcvd >= tp->t_maxseg &&
			    tp->max_rcvd < so->so_rcv.sb_hiwat &&
			    2 * adv >= tp->max_rcvd)
				goto send;
		}
	}

	/*
	 * Send if we owe peer an ACK.
	 */
	if (tp->t_flags & TF_ACKNOW)
		goto send;
	if (flags & (TH_SYN|TH_RST))
		goto send;
	if (SEQ_GT(tp->snd_up, tp->snd_una))
		goto send;
	/*
	 * If our state indicates that FIN should be sent
	 * and we have not yet done so, or we're retransmitting the FIN,
	 * then we need to send.
	 */
	if (flags & TH_FIN &&
	    ((tp->t_flags & TF_SENTFIN) == 0 || tp->snd_nxt == tp->snd_una))
		goto send;

	/*
	 * TCP window updates are not reliable, rather a polling protocol
	 * using ``persist'' packets is used to insure receipt of window
	 * updates.  The three ``states'' for the output side are:
	 *	idle			not doing retransmits or persists
	 *	persisting		to move a small or zero window
	 *	(re)transmitting	and thereby not persisting
	 *
	 * tp->t_timer[TCPT_PERSIST]
	 *	is set when we are in persist state.
	 * tp->t_force
	 *	is set when we are called to send a persist packet.
	 * tp->t_timer[TCPT_REXMT]
	 *	is set when we are retransmitting
	 * The output side is idle when both timers are zero.
	 *
	 * If send window is too small, there is data to transmit, and no
	 * retransmit or persist is pending, then go to persist state.
	 * If nothing happens soon, send when timer expires:
	 * if window is nonzero, transmit what we can,
	 * otherwise force out a byte.
	 */
	if (so->so_snd.sb_cc && tp->t_timer[TCPT_REXMT] == 0 &&
	    tp->t_timer[TCPT_PERSIST] == 0) {
		tp->t_rxtshift = 0;
		tcp_setpersist(tp);
	}

	SOCKBUF_UNLOCK(&so->so_snd);

	/*
	 * No reason to send a segment, just return.
	 */
	return (0);

send:
	/*
	 * Before ESTABLISHED, force sending of initial options
	 * unless TCP set not to do any options.
	 * NOTE: we assume that the IP/TCP header plus TCP options
	 * always fit in a single mbuf, leaving room for a maximum
	 * link header, i.e.
	 *	max_linkhdr + sizeof (struct tcpiphdr) + optlen <= MHLEN
	 */
	optlen = 0;
	hdrlen = sizeof (struct tcpiphdr);
	if (flags & TH_SYN && (tp->t_flags & TF_NOOPT) == 0) {
#if	!RFC_1323
		opt = tcp_initopt.opt;
		optlen = sizeof (tcp_initopt);
		hdrlen += sizeof (tcp_initopt);
#endif
		opt[0] = TCPOPT_MAXSEG;
		opt[1] = 4;
		*(u_short *)(opt + 2) = htons((u_short) tcp_mss(tp, 0));
#if	RFC_1323
		optlen = 4;

		if ( (tp->t_flags & TF_REQ_SCALE) &&
		    ( ! (flags & TH_ACK) || (tp->t_flags & TF_RCVD_SCALE) )) {
			*( (u_long *) (opt+optlen) ) = htonl(
				TCPOPT_NOP<<24 |
				TCPOPT_WINDOW<<16 |
				3<<8 |
				tp->request_r_scale );
			optlen += 4;
		}

		/* Send a SACK_PERMITTED option in the SYN segment.
		 */
		if ( ! tcp_dont_sack ) {
			*( (u_long *) (opt+optlen) ) = htonl(
				TCPOPT_NOP<<24 |
				TCPOPT_NOP<<16 |
				TCPOPT_SACK_PERMITTED<<8 |
				/* length */ 2 );
			optlen += 4;
		}
#endif /* RFC_1323 */
	}
#if	RFC_1323
	/* Send a time-stamp and echo-reply if:
	 * This is a SYN and our side wants to use time-stamps
	 * (TF_REQ_TSTMP is set) or both our side and our peer have sent
	 * time-stamps in our SYN's.
	 */
	if ( (tp->t_flags&(TF_REQ_TSTMP|TF_NOOPT)) == TF_REQ_TSTMP &&
	    ! (flags&TH_RST) &&
	    ((flags&(TH_SYN|TH_ACK))==TH_SYN || (tp->t_flags&TF_RCVD_TSTMP))){

		/* Form time-stamp option as shown in appendix A of RFC 1323.
		 */

		*( (u_long *) (opt+optlen) ) = htonl(
			TCPOPT_NOP<<24 |
			TCPOPT_NOP<<16 |
			TCPOPT_TIMESTAMP<<8 |
			10 );
		*( (u_long *) (opt+optlen+4) ) = htonl( tcp_now );
		*( (u_long *) (opt+optlen+8) ) = htonl( tp->ts_recent );
		optlen += 12;
	}


	/* Send SACK if needed.
	 * I won't tack a SACK onto a non-empty segment.
	 */
	if ( tp->seg_next != (struct tcpiphdr *) tp && len == 0 &&
		! tcp_dont_sack &&
		(tp->t_flags&(TF_SACK_PERMIT|TF_NOOPT))==TF_SACK_PERMIT) {

		register struct tcpiphdr *q = tp->seg_next;
		register u_long *optl = (u_long *)(opt+optlen)+1;
		tcp_seq	block_start;
		u_long	block_size;
		int	sack_len = 2;

		/* Use these to gather runs of received segments.
		 * The first segment in the queue becomes the initial run.
		 */
		block_start = q->ti_seq;
		block_size = q->ti_len;

		do {
			q = (struct tcpiphdr *) q->ti_next;

			if ( q == (struct tcpiphdr *) tp ||
				block_start+block_size != q->ti_seq ) {
				
				/* Stick the relative origin and block size
				 * in the SACK option.
				 */
				*optl++ = htonl( block_start-tp->rcv_nxt );
				*optl++ = htonl( block_size );
				sack_len += 8;

				/* If no more will fit into options, quit.
				 */
				if ( sack_len + optlen > TCP_MAX_OPTIONSPC-8 )
					break;

				if ( q != (struct tcpiphdr *) tp ) {
					block_start = q->ti_seq;
					block_size = q->ti_len;
				}
			}
			else
				/* This segment just accumulates into previous
				 * run.
				 */
				block_size += q->ti_len;

		} while ( q != (struct tcpiphdr *) tp );

		*( (u_long *) (opt+optlen) ) = htonl( TCPOPT_NOP << 24 |
			TCPOPT_NOP << 16 | TCPOPT_SACK << 8 | sack_len );
		optlen += sack_len+2;
	}

	hdrlen += optlen;
#endif

#ifdef DIAGNOSTIC
	if (max_linkhdr + hdrlen > MHLEN)
		panic("tcphdr too big");
#endif

	/*
	 * Grab a header mbuf, attaching a copy of data to
	 * be transmitted, and initialize the header from
	 * the template for sends on this connection.
	 */
	NETSTAT_LOCK(&tcpstat.tcps_lock);
	if (len) {
		if (tp->t_force && len == 1)
			tcpstat.tcps_sndprobe++;
		else if (SEQ_LT(tp->snd_nxt, tp->snd_max)) {
			tcpstat.tcps_sndrexmitpack++;
			tcpstat.tcps_sndrexmitbyte += len;
		} else {
			tcpstat.tcps_sndpack++;
			tcpstat.tcps_sndbyte += len;
		}
		NETSTAT_UNLOCK(&tcpstat.tcps_lock);
#ifdef	notyet
		if ((m = m_copypack(so->so_snd.sb_mb, off,
		    (int)len, max_linkhdr + hdrlen)) == 0) {
			error = ENOBUFS;
			goto out;
		}
		/*
		 * m_copypack left space for our hdr; use it.
		 */
		m->m_len += hdrlen;
		m->m_data -= hdrlen;
#else
		MGETHDR(m, M_DONTWAIT, MT_HEADER);
		if (m == NULL) {
			error = ENOBUFS;
			goto out;
		}
		m->m_data += max_linkhdr;
		m->m_len = hdrlen;
		if (len <= MHLEN - hdrlen - max_linkhdr) {
			m_copydata(so->so_snd.sb_mb, off, (int) len,
			    mtod(m, caddr_t) + hdrlen);
			m->m_len += len;
		} else {
			m->m_next = m_copym(so->so_snd.sb_mb,
						off, (int) len, M_DONTWAIT);
			if (m->m_next == 0)
				len = 0;
		}
		/*
		 * If we're sending everything we've got, set PUSH.
		 * (This will keep happy those implementations which only
		 * give data to the user when a buffer fills or
		 * a PUSH comes in.)
		 */
		if (off + len == so->so_snd.sb_cc)
			flags |= TH_PUSH;
#endif
	} else {
		if (tp->t_flags & TF_ACKNOW)
			tcpstat.tcps_sndacks++;
		else if (flags & (TH_SYN|TH_FIN|TH_RST))
			tcpstat.tcps_sndctrl++;
		else if (SEQ_GT(tp->snd_up, tp->snd_una))
			tcpstat.tcps_sndurg++;
		else
			tcpstat.tcps_sndwinup++;
		NETSTAT_UNLOCK(&tcpstat.tcps_lock);

		MGETHDR(m, M_DONTWAIT, MT_HEADER);
		if (m == NULL) {
			error = ENOBUFS;
			goto out;
		}
		m->m_data += max_linkhdr;
		m->m_len = hdrlen;
	}
	m->m_pkthdr.rcvif = (struct ifnet *)0;
	ti = mtod(m, struct tcpiphdr *);
	if (tp->t_template.ti_pr != IPPROTO_TCP)
		panic("tcp_output");
	bcopy((caddr_t)&tp->t_template, (caddr_t)ti, sizeof (struct tcpiphdr));

	/*
	 * Fill in fields, remembering maximum advertised
	 * window for use in delaying messages about window sizes.
	 * If resending a FIN, be sure not to use a new sequence number.
	 */
	if (flags & TH_FIN && tp->t_flags & TF_SENTFIN && 
	    tp->snd_nxt == tp->snd_max)
		tp->snd_nxt--;
	ti->ti_seq = htonl(tp->snd_nxt);
	ti->ti_ack = htonl(tp->rcv_nxt);
	if (optlen) {
		bcopy((caddr_t)opt, (caddr_t)(ti + 1), optlen);
		ti->ti_xoff = (sizeof (struct tcphdr) + optlen) << 2;
	}
	ti->ti_flags = flags;
	/*
	 * Calculate receive window.  Don't shrink window,
	 * but avoid silly window syndrome.
	 */
	if (win < (long)(so->so_rcv.sb_hiwat / 4) && win < (long)tp->t_maxseg)
		win = 0;
#if	RFC_1323
	if (win > (long)TCP_MAXWIN<<tp->rcv_scale)
		win = (long)TCP_MAXWIN<<tp->rcv_scale;
#else
	if (win > TCP_MAXWIN)
		win = TCP_MAXWIN;
#endif
	if (win < (long)(tp->rcv_adv - tp->rcv_nxt))
		win = (long)(tp->rcv_adv - tp->rcv_nxt);
#if	RFC_1323
	ti->ti_win = htons( (u_short) (win>>tp->rcv_scale) );
#else
	ti->ti_win = htons((u_short)win);
#endif
	if (SEQ_GT(tp->snd_up, tp->snd_nxt)) {
		ti->ti_urp = htons((u_short)(tp->snd_up - tp->snd_nxt));
		ti->ti_flags |= TH_URG;
	} else
		/*
		 * If no urgent pointer to send, then we pull
		 * the urgent pointer to the left edge of the send window
		 * so that it doesn't drift into the send window on sequence
		 * number wraparound.
		 */
		tp->snd_up = tp->snd_una;		/* drag it along */

	SOCKBUF_UNLOCK(&so->so_snd);

	/*
	 * Put TCP length in extended header, and then
	 * checksum extended header and data.
	 */
	if (len + optlen)
		ti->ti_len = htons((u_short)(sizeof (struct tcphdr) +
		    optlen + len));
	ti->ti_sum = in_cksum(m, (int)(hdrlen + len));

	/*
	 * In transmit state, time the transmission and arrange for
	 * the retransmit.  In persist state, just set snd_max.
	 */
	if (tp->t_force == 0 || tp->t_timer[TCPT_PERSIST] == 0) {
		tcp_seq startseq = tp->snd_nxt;

		/*
		 * Advance snd_nxt over sequence space of this segment.
		 */
		if (flags & (TH_SYN|TH_FIN)) {
			if (flags & TH_SYN)
				tp->snd_nxt++;
			if (flags & TH_FIN) {
				tp->snd_nxt++;
				tp->t_flags |= TF_SENTFIN;
			}
		}
		tp->snd_nxt += len;
		if (SEQ_GT(tp->snd_nxt, tp->snd_max)) {
			tp->snd_max = tp->snd_nxt;
			/*
			 * Time this transmission if not a retransmission and
			 * not currently timing anything.
			 */
			if (tp->t_rtt == 0) {
				tp->t_rtt = 1;
				tp->t_rtseq = startseq;
				NETSTAT_LOCK(&tcpstat.tcps_lock);
				tcpstat.tcps_segstimed++;
				NETSTAT_UNLOCK(&tcpstat.tcps_lock);
			}
		}

		/*
		 * Set retransmit timer if not currently set,
		 * and not doing an ack or a keep-alive probe.
		 * Initial value for retransmit timer is smoothed
		 * round-trip time + 2 * round-trip time variance.
		 * Initialize shift counter which is used for backoff
		 * of retransmit time.
		 */
		if (tp->t_timer[TCPT_REXMT] == 0 &&
		    tp->snd_nxt != tp->snd_una) {
			tp->t_timer[TCPT_REXMT] = tp->t_rxtcur;
			if (tp->t_timer[TCPT_PERSIST]) {
				tp->t_timer[TCPT_PERSIST] = 0;
				tp->t_rxtshift = 0;
			}
		}
	} else
		if (SEQ_GT(tp->snd_nxt + len, tp->snd_max))
			tp->snd_max = tp->snd_nxt + len;

	/*
	 * Trace.
	 */
	if (so->so_options & SO_DEBUG)
		tcp_trace(TA_OUTPUT, tp->t_state, tp, ti, 0);

	/*
	 * Fill in IP length and desired time to live and
	 * send to IP level.  There should be a better way
	 * to handle ttl and tos; we could keep them in
	 * the template, but need a way to checksum without them.
	 */
	m->m_pkthdr.len = hdrlen + len;
	((struct ip *)ti)->ip_len = m->m_pkthdr.len;
	((struct ip *)ti)->ip_ttl = tp->t_inpcb->inp_ip.ip_ttl;	/* XXX */
	((struct ip *)ti)->ip_tos = tp->t_inpcb->inp_ip.ip_tos;	/* XXX */
	error = ip_output(m, tp->t_inpcb->inp_options, &tp->t_inpcb->inp_route,
	    so->so_options & SO_DONTROUTE);
	if (error) {
out:
		if (error == ENOBUFS) {
			tcp_quench(tp->t_inpcb);
			return (0);
		}
		if ((error == EHOSTUNREACH || error == ENETDOWN)
		    && TCPS_HAVERCVDSYN(tp->t_state)) {
			tp->t_softerror = error;
			return (0);
		}
		return (error);
	}
	NETSTAT_LOCK(&tcpstat.tcps_lock);
	tcpstat.tcps_sndtotal++;
	NETSTAT_UNLOCK(&tcpstat.tcps_lock);

	/*
	 * Data sent (as far as we can tell).
	 * If this advertises a larger window than any other segment,
	 * then remember the size of the advertised window.
	 * Any pending ACK has now been sent.
	 */
	if (win > 0 && SEQ_GT(tp->rcv_nxt+win, tp->rcv_adv))
		tp->rcv_adv = tp->rcv_nxt + win;
#if	RFC_1323
	tp->last_ack_sent = tp->rcv_nxt;
#endif
	tp->t_flags &= ~(TF_ACKNOW|TF_DELACK);
	if (sendalot)
		goto again;
	return (0);
}

void
tcp_setpersist(tp)
	register struct tcpcb *tp;
{
	register t = ((tp->t_srtt >> 2) + tp->t_rttvar) >> 1;

	if (tp->t_timer[TCPT_REXMT])
		panic("tcp_output REXMT");
	/*
	 * Start/restart persistance timer.
	 */
	TCPT_RANGESET(tp->t_timer[TCPT_PERSIST],
	    t * tcp_backoff[tp->t_rxtshift],
	    TCPTV_PERSMIN, TCPTV_PERSMAX);
	if (tp->t_rxtshift < TCP_MAXRXTSHIFT)
		tp->t_rxtshift++;
}
