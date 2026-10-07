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
 * $Log: tcp_timer.c,v $
 * Revision 1.4  1994/11/18  20:35:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:27:22  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:24  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:08  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/10/22  23:18:30  hobbes
 * Added the RFC_1323 extensions.
 *
 * Revision 2.2  1991/08/31  13:44:44  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:37:14  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10.4.2  91/03/15  17:49:43  tmt
 * 	Drop bogus extra argument to tcp_usrreq().
 * 	[91/03/13  19:12:37  tmt]
 * 
 * Revision 1.10  90/10/07  14:36:07  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:16:50  gm]
 * 
 * Revision 1.9  90/07/27  09:02:14  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:41:17  tmt]
 * 
 * Revision 1.8  90/07/05  23:13:25  devrcs
 * 	Fix deadlock problem in tcp_fasttimo (tcb head locked before sockets
 * 	and inpcb's). Remove SPL's, done in netisr.c and inet_funnel for us.
 * 	[90/07/03  18:55:37  tmt]
 * 
 * Revision 1.7  90/06/22  20:39:30  devrcs
 * 	ASSERT macro (LOCK_ASSERT) cannot wrap lines.
 * 	[90/06/09  17:58:17  tmt]
 * 
 * 	Parallelization repairs. Socket locks, then inpcb locks.
 * 	Take inpcbhead locks directly. Do refcounting right.
 * 	Resync correctly if lost chain in tcp_slowtimo().
 * 	[90/06/07  16:14:49  tmt]
 * 
 * 	Fix unlock problem in tcp_fasttimo.
 * 	[90/05/18  11:38:16  tmt]
 * 
 * Revision 1.6  90/04/27  19:19:19  devrcs
 * 	Tickle a stat when we lose the tcb chain - just temporary.
 * 	[90/04/20  13:04:58  tmt]
 * 
 * Revision 1.5  90/04/14  00:33:39  devrcs
 * 	Add void's.
 * 	[90/04/09  16:35:34  tmt]
 * 
 * Revision 1.4  90/01/18  08:48:03  gm
 * 	No global lock in INPCB_LOCK macro.
 * 	Add another struct mbuf *0 to usrreq args.
 * 	[90/01/08  16:13:25  tmt]
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
 * Revision 1.3  90/01/03  12:42:06  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:23  gm]
 * 
 * Revision 1.2  89/12/26  10:16:04  gm
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
 *	Base:	tcp_timer.c	7.15 (Berkeley) 4/8/89
 *	Merged:	tcp_timer.c	7.18 (Berkeley) 6/28/90
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
#include "netinet/ip_var.h"
#include "netinet/tcp.h"
#include "netinet/tcpip.h"
#include "netinet/tcp_fsm.h"
#include "netinet/tcp_seq.h"
#include "netinet/tcp_timer.h"
#include "netinet/tcp_var.h"

#include "net/net_malloc.h"

LOCK_ASSERTL_DECL

int	tcp_keepidle = TCPTV_KEEP_IDLE;
int	tcp_keepintvl = TCPTV_KEEPINTVL;
int	tcp_maxidle;

/*
 * Fast timeout routine for processing delayed acks
 */
#if	NETSYNC_LOCK
/*
 * Because the fasttimo only sends delayed ack segments and thus the
 * connections do not vanish in mid-operation, we don't have to worry
 * as much about losing the chain as slowtimo. So, if the next inpcb
 * goes away when we're processing the current, we just go around
 * again (since we turn off DELACK on send). An alternative would be
 * to return and go around in 200ms.
 */
#endif
void
tcp_fasttimo()
{
	register struct inpcb *inp, *inpnxt;
	register struct tcpcb *tp;
	register struct socket *so;

resync:
	INHEAD_READ_LOCK(&tcb);
	if (inp = tcb.inp_next) {
		INPCBRC_REF(inp);
		for (; inp != &tcb; inp = inpnxt) {
			so = inp->inp_socket;
			inpnxt = inp->inp_next;
			INPCBRC_REF(inpnxt);
			INHEAD_READ_UNLOCK(&tcb);
			SOCKET_LOCK(so);
			INPCB_LOCK(inp);
			if ((tp = (struct tcpcb *)inp->inp_ppcb) &&
			    (tp->t_flags & TF_DELACK)) {
				tp->t_flags &= ~TF_DELACK;
				tp->t_flags |= TF_ACKNOW;
				NETSTAT_LOCK(&tcpstat.tcps_lock);
				tcpstat.tcps_delack++;
				NETSTAT_UNLOCK(&tcpstat.tcps_lock);
				(void) tcp_output(tp);
			}
			INPCB_UNLOCK(inp);
			INPCBRC_UNREF(inp);
			SOCKET_UNLOCK(so);
			INHEAD_READ_LOCK(&tcb);
			if (inpnxt->inp_next == 0) {	/* lost our next */
				INHEAD_READ_UNLOCK(&tcb);
				so = inpnxt->inp_socket;
				SOCKET_LOCK(so);
				INPCBRC_UNREF(inpnxt);
				SOCKET_UNLOCK(so);
				goto resync;
			}
		}
		INPCBRC_UNREF(inp);	/* inp == head here */
	}
	INHEAD_READ_UNLOCK(&tcb);
}

/*
 * Tcp protocol timeout routine called every 500 ms.
 * Updates the timers in all active tcb's and
 * causes finite state machine actions if timers expire.
 */
#if	NETSYNC_LOCK
/*
 * The hard part in this routine in NETSYNC_LOCK is to make sure we're looking
 * at a valid INPCB chain. Since connections can come and go while we're in
 * the middle of tcp_usrreq, that isn't necessarily easy.  What we do know
 * is that the current connection won't go away while the INPCB refcnt is
 * held, and we must look for the 'next' CB while the global TCP chain lock
 * is held. We will need to resync if the 'next' INPCB goes away while we're
 * processing the current, but we have no problem with the current going away
 * thanks to the socket lock and INPCB reference.
 */
#endif

void
tcp_slowtimo()
{
	register struct inpcb *inp, *inpnxt;
	register struct tcpcb *tp;
	register int i;
	register tcp_seq timestamp;

	tcp_maxidle = TCPTV_KEEPCNT * tcp_keepintvl;
	timestamp = tcp_iss;

	/*
	 * Search through tcb's and update active timers.
	 */
resync:
	INHEAD_READ_LOCK(&tcb);
	if (inp = tcb.inp_next) {
		INPCBRC_REF(inp);
		for (; inp != &tcb; inp = inpnxt) {
			struct socket *so = inp->inp_socket;
			inpnxt = inp->inp_next;
			INPCBRC_REF(inpnxt);
			INHEAD_READ_UNLOCK(&tcb);
			SOCKET_LOCK(so);
			INPCB_LOCK(inp);
			tp = intotcpcb(inp);
			if (tp && timestamp != tp->t_timestamp) {
				tp->t_timestamp = timestamp;
				for (i = 0; i < TCPT_NTIMERS; i++) {
					if (tp->t_timer[i] &&
					    --tp->t_timer[i] == 0) {
						LOCK_ASSERT("tcp_slowtimo so", so == tp->t_inpcb->inp_socket);
						(void) tcp_usrreq(
							so,
							PRU_SLOWTIMO,
							(struct mbuf *)0,
							(struct mbuf *)i,
							(struct mbuf *)0);
						if (inp->inp_next == 0)
							goto tpgone;
					}
				}
				tp->t_idle++;
#ifdef RFC_1323
 				tp->ts_recent_age++;
#endif
				if (tp->t_rtt)
					tp->t_rtt++;
			}
			INPCB_UNLOCK(inp);
tpgone:			
			INPCBRC_UNREF(inp);
			SOCKET_UNLOCK(so);
			INHEAD_READ_LOCK(&tcb);
			if (inpnxt->inp_next == 0) {	/* lost our next */
				INHEAD_READ_UNLOCK(&tcb);
				so = inpnxt->inp_socket;
				SOCKET_LOCK(so);
				INPCBRC_UNREF(inpnxt);
				SOCKET_UNLOCK(so);
				goto resync;
			}
		}
		INPCBRC_UNREF(inp);	/* inp == head here */
	}
	INHEAD_READ_UNLOCK(&tcb);

	TCPMISC_LOCK();
	tcp_iss += TCP_ISSINCR/PR_SLOWHZ;		/* increment iss */
	if (tcp_compat_42 && (int)tcp_iss < 0)
		tcp_iss = 0;				/* XXX */
#ifdef RFC_1323
 	tcp_now++;					/* for time-stamps */
#endif
	TCPMISC_UNLOCK();
}

/*
 * Cancel all timers for TCP tp.
 */
void
tcp_canceltimers(tp)
	struct tcpcb *tp;
{
	register int i;

	for (i = 0; i < TCPT_NTIMERS; i++)
		tp->t_timer[i] = 0;
}

CONST int	tcp_backoff[TCP_MAXRXTSHIFT + 1] =
    { 1, 2, 4, 8, 16, 32, 64, 64, 64, 64, 64, 64, 64 };

/*
 * TCP timer processing.
 */
struct tcpcb *
tcp_timers(tp, timer)
	register struct tcpcb *tp;
	int timer;
{
	register int rexmt;

	switch (timer) {

	/*
	 * 2 MSL timeout in shutdown went off.  If we're closed but
	 * still waiting for peer to close and connection has been idle
	 * too long, or if 2MSL time is up from TIME_WAIT, delete connection
	 * control block.  Otherwise, check again in a bit.
	 */
	case TCPT_2MSL:
		if (tp->t_state != TCPS_TIME_WAIT &&
		    tp->t_idle <= tcp_maxidle)
			tp->t_timer[TCPT_2MSL] = tcp_keepintvl;
		else
			tp = tcp_close(tp);
		break;

	/*
	 * Retransmission timer went off.  Message has not
	 * been acked within retransmit interval.  Back off
	 * to a longer retransmit interval and retransmit one segment.
	 */
	case TCPT_REXMT:
		if (++tp->t_rxtshift > TCP_MAXRXTSHIFT) {
			tp->t_rxtshift = TCP_MAXRXTSHIFT;
			NETSTAT_LOCK(&tcpstat.tcps_lock);
			tcpstat.tcps_timeoutdrop++;
			NETSTAT_UNLOCK(&tcpstat.tcps_lock);
			tp = tcp_drop(tp, tp->t_softerror ?
			    tp->t_softerror : ETIMEDOUT);
			break;
		}
		NETSTAT_LOCK(&tcpstat.tcps_lock);
		tcpstat.tcps_rexmttimeo++;
		NETSTAT_UNLOCK(&tcpstat.tcps_lock);
		rexmt = TCP_REXMTVAL(tp) * tcp_backoff[tp->t_rxtshift];
		TCPT_RANGESET(tp->t_rxtcur, rexmt,
		    tp->t_rttmin, TCPTV_REXMTMAX);
		tp->t_timer[TCPT_REXMT] = tp->t_rxtcur;
		/*
		 * If losing, let the lower level know and try for
		 * a better route.  Also, if we backed off this far,
		 * our srtt estimate is probably bogus.  Clobber it
		 * so we'll take the next rtt measurement as our srtt;
		 * move the current srtt into rttvar to keep the current
		 * retransmit times until then.
		 */
		if (tp->t_rxtshift > TCP_MAXRXTSHIFT / 4) {
			in_losing(tp->t_inpcb);
			tp->t_rttvar += (tp->t_srtt >> TCP_RTT_SHIFT);
			tp->t_srtt = 0;
		}
		tp->snd_nxt = tp->snd_una;
		/*
		 * If timing a segment in this window, stop the timer.
		 */
		tp->t_rtt = 0;
		/*
		 * Close the congestion window down to one segment
		 * (we'll open it by one segment for each ack we get).
		 * Since we probably have a window's worth of unacked
		 * data accumulated, this "slow start" keeps us from
		 * dumping all that data as back-to-back packets (which
		 * might overwhelm an intermediate gateway).
		 *
		 * There are two phases to the opening: Initially we
		 * open by one mss on each ack.  This makes the window
		 * size increase exponentially with time.  If the
		 * window is larger than the path can handle, this
		 * exponential growth results in dropped packet(s)
		 * almost immediately.  To get more time between 
		 * drops but still "push" the network to take advantage
		 * of improving conditions, we switch from exponential
		 * to linear window opening at some threshhold size.
		 * For a threshhold, we use half the current window
		 * size, truncated to a multiple of the mss.
		 *
		 * (the minimum cwnd that will give us exponential
		 * growth is 2 mss.  We don't allow the threshhold
		 * to go below this.)
		 */
		{
		u_int win = min(tp->snd_wnd, tp->snd_cwnd) / 2 / tp->t_maxseg;
		if (win < 2)
			win = 2;
		tp->snd_cwnd = tp->t_maxseg;
		tp->snd_ssthresh = win * tp->t_maxseg;
		tp->t_dupacks = 0;
		}
		(void) tcp_output(tp);
		break;

	/*
	 * Persistance timer into zero window.
	 * Force a byte to be output, if possible.
	 */
	case TCPT_PERSIST:
		NETSTAT_LOCK(&tcpstat.tcps_lock);
		tcpstat.tcps_persisttimeo++;
		NETSTAT_UNLOCK(&tcpstat.tcps_lock);
		tcp_setpersist(tp);
		tp->t_force = 1;
		(void) tcp_output(tp);
		tp->t_force = 0;
		break;

	/*
	 * Keep-alive timer went off; send something
	 * or drop connection if idle for too long.
	 */
	case TCPT_KEEP:
		NETSTAT_LOCK(&tcpstat.tcps_lock);
		tcpstat.tcps_keeptimeo++;
		NETSTAT_UNLOCK(&tcpstat.tcps_lock);
		if (tp->t_state < TCPS_ESTABLISHED)
			goto dropit;
		if (tp->t_inpcb->inp_socket->so_options & SO_KEEPALIVE &&
		    tp->t_state <= TCPS_CLOSE_WAIT) {
		    	if (tp->t_idle >= tcp_keepidle + tcp_maxidle)
				goto dropit;
			/*
			 * Send a packet designed to force a response
			 * if the peer is up and reachable:
			 * either an ACK if the connection is still alive,
			 * or an RST if the peer has closed the connection
			 * due to timeout or reboot.
			 * Using sequence number tp->snd_una-1
			 * causes the transmitted zero-length segment
			 * to lie outside the receive window;
			 * by the protocol spec, this requires the
			 * correspondent TCP to respond.
			 */
			NETSTAT_LOCK(&tcpstat.tcps_lock);
			tcpstat.tcps_keepprobe++;
			NETSTAT_UNLOCK(&tcpstat.tcps_lock);
			if (tcp_compat_42)
				/*
				 * The keepalive packet must have nonzero
				 * length to get a 4.2 host to respond.
				 */
				tcp_respond(tp, &tp->t_template,
				    (struct mbuf *)NULL,
				    tp->rcv_nxt - 1, tp->snd_una - 1, 0);
			else
				tcp_respond(tp, &tp->t_template,
				    (struct mbuf *)NULL,
				    tp->rcv_nxt, tp->snd_una - 1, 0);
			tp->t_timer[TCPT_KEEP] = tcp_keepintvl;
		} else
			tp->t_timer[TCPT_KEEP] = tcp_keepidle;
		break;
	dropit:
		NETSTAT_LOCK(&tcpstat.tcps_lock);
		tcpstat.tcps_keepdrops++;
		NETSTAT_UNLOCK(&tcpstat.tcps_lock);
		tp = tcp_drop(tp, ETIMEDOUT);
		break;
	}
	return (tp);
}
