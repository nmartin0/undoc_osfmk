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
 * $Log: tcp_usrreq.c,v $
 * Revision 1.6  1994/11/18  20:35:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/01  01:35:57  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 2.3  93/08/27  16:39:51  nina
 * [LCC 375] ip_ctloutput() was getting a lock assertion failure
 * due to the fact that it was not coordinating properly with
 * tcp_ctloutput().  Modify tcp_ctloutput() to work properly
 * with ip_ctloutput() with regard to the struct inpcb lock.
 * 
 * Revision 1.4  1993/05/27  23:57:04  hobbes
 * Initialized the tcpcb reference tp to NULL
 * without this the server will break when running
 * INETPRINTFS.
 *
 * Revision 1.3  1993/05/06  20:27:28  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:30:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:09  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:15  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/10/22  23:18:34  hobbes
 * Added the RFC_1323 extensions.
 *
 * Revision 2.2  1991/08/31  13:44:52  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:37:26  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13  90/10/07  14:36:18  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:17:04  gm]
 * 
 * Revision 1.12  90/08/24  12:14:54  devrcs
 * 	Free control mbufs in tcp_usrreq().
 * 	[90/08/09  13:38:35  tmt]
 * 
 * Revision 1.11  90/08/09  13:26:45  devrcs
 * 	Lock sockbuf (again) in SEND_OOB.
 * 	[90/07/24  14:14:41  tmt]
 * 
 * Revision 1.10  90/07/27  09:02:22  devrcs
 * 	Update to BSD Reno release.
 * 	Add an assertion. Don't re-check sbspace on OOB send.
 * 	[90/07/19  17:42:56  tmt]
 * 
 * Revision 1.9  90/07/05  23:13:29  devrcs
 * 	Remove SPL's, done at socket to inet layer.
 * 	[90/07/03  18:56:12  tmt]
 * 
 * Revision 1.8  90/06/29  13:37:08  devrcs
 * 	Make security contingent on MACH for compat.
 * 	[90/06/26  19:58:10  tmt]
 * 
 * Revision 1.7  90/06/22  20:39:34  devrcs
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:45:04  seiden]
 * 
 * 	Parallelization repairs. Socket locks, then inpcb locks.
 * 	Take inpcbhead locks directly. Do refcounting right.
 * 	Handle urgent data properly if !tcp_compat_42.
 * 	[90/06/07  16:15:56  tmt]
 * 
 * 	Fix lock problem in tcp_ctloutput.
 * 	[90/05/18  11:38:39  tmt]
 * 
 * Revision 1.6  90/04/27  19:19:28  devrcs
 * 	Checkpoint.
 * 	[90/04/20  13:06:09  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:57  robert
 * 	Use macro for socket_islocked.
 * 	[90/01/19  15:08:27  tmt]
 * 
 * Revision 1.4  90/01/18  08:48:11  gm
 * 	Update INPCB_LOCK macro arguments.
 * 	Don't SOCKET_LOCK after in_pcbdetach (old code).
 * 	[90/01/08  16:14:11  tmt]
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
 * Revision 1.3  90/01/03  12:42:09  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:27  gm]
 * 
 * Revision 1.2  89/12/26  10:16:28  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)tcp_usrreq.c	2.1 16:12:24 4/20/90 SecureWare */
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
 *	Base:	tcp_usrreq.c	7.12 (Berkeley) 4/8/89
 *	Merged:	tcp_usrreq.c	7.15 (Berkeley) 6/28/90
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
#include "netinet/tcp.h"
#include "netinet/tcpip.h"
#include "netinet/tcp_fsm.h"
#include "netinet/tcp_seq.h"
#include "netinet/tcp_timer.h"
#include "netinet/tcp_var.h"
#include "netinet/tcp_debug.h"

#include "net/net_malloc.h"

LOCK_ASSERTL_DECL

/*
 * TCP protocol interface to socket abstraction.
 */

/*
 * Process a TCP user request for TCP tb.  If this is a send request
 * then m is the mbuf chain of send data.  If this is a timer expiration
 * (called from the software clock routine), then timertype tells which timer.
 */
/*ARGSUSED*/
tcp_usrreq(so, req, m, nam, control)
	struct socket *so;
	int req;
	struct mbuf *m, *nam, *control;
{
	register struct inpcb *inp;
	register struct tcpcb *tp=NULL;
	int error = 0;
	int ostate;

	LOCK_ASSERT("tcp_usrreq", SOCKET_ISLOCKED(so));

	if (req == PRU_CONTROL)
		return (in_control(so, (int)m, (caddr_t)nam,
			(struct ifnet *)control));
#if	!SEC_ARCH
	if (control && control->m_len) {
		m_freem(control);
		if (m)
			m_freem(m);
		return (EINVAL);
	}
#endif

	inp = sotoinpcb(so);
	/*
	 * When a TCP is attached to a socket, then there will be
	 * a (struct inpcb) pointed at by the socket, and this
	 * structure will point at a subsidary (struct tcpcb).
	 */
	if (inp) {
		if (req != PRU_SLOWTIMO)
			INPCB_LOCK(inp);
		tp = intotcpcb(inp);
		/* If tp is 0, it's been closed but not yet freed */
		if (tp == 0) {
			if (req != PRU_SLOWTIMO)
				INPCB_UNLOCK(inp);
			return (EINVAL);		/* XXX */
		}
#ifdef KPROF
		tcp_acounts[tp->t_state][req]++;
#endif
		ostate = tp->t_state;
	} else {
		if (req != PRU_ATTACH)
			return (EINVAL);		/* XXX */
		ostate = 0;
	}

	switch (req) {

	/*
	 * TCP attaches to socket via PRU_ATTACH, reserving space,
	 * and an internet control block.
	 */
	case PRU_ATTACH:
		if (inp) {
			error = EISCONN;
			break;
		}
		error = tcp_attach(so);
		if (error)
			break;
		inp = sotoinpcb(so);
		if ((so->so_options & SO_LINGER) && so->so_linger == 0)
			so->so_linger = TCP_LINGERTIME;
		tp = sototcpcb(so);
		break;

	/*
	 * PRU_DETACH detaches the TCP protocol from the socket.
	 * If the protocol state is non-embryonic, then can't
	 * do this directly: have to initiate a PRU_DISCONNECT,
	 * which may finish later; embryonic TCB's can just
	 * be discarded here.
	 */
	case PRU_DETACH:
		if (tp->t_state > TCPS_LISTEN)
			tp = tcp_disconnect(tp);
		else
			tp = tcp_close(tp);
		break;

	/*
	 * Give the socket an address.
	 */
	case PRU_BIND:
		error = in_pcbbind(inp, nam);
		if (error)
			break;
		break;

	/*
	 * Prepare to accept connections.
	 */
	case PRU_LISTEN:
		if (inp->inp_lport == 0)
			error = in_pcbbind(inp, (struct mbuf *)0);
		if (error == 0)
			tp->t_state = TCPS_LISTEN;
		break;

	/*
	 * Initiate connection to peer.
	 * Create a template for use in transmissions on this connection.
	 * Enter SYN_SENT state, and mark socket as connecting.
	 * Start keep-alive timer, and seed output sequence space.
	 * Send initial segment on connection.
	 */
	case PRU_CONNECT:
		if (inp->inp_lport == 0) {
			error = in_pcbbind(inp, (struct mbuf *)0);
			if (error)
				break;
		}
		error = in_pcbconnect(inp, nam);
		if (error)
			break;
		(void) tcp_template(tp);
#ifdef RFC_1323
		/* Compute window scaling to request.
		 */
		while ( tp->request_r_scale < TCP_MAX_WINSHIFT &&
		    TCP_MAXWIN<<tp->request_r_scale < so->so_rcv.sb_hiwat )
			tp->request_r_scale++;
#endif
		soisconnecting(so);
		NETSTAT_LOCK(&tcpstat.tcps_lock);
		tcpstat.tcps_connattempt++;
		NETSTAT_UNLOCK(&tcpstat.tcps_lock);
		tp->t_state = TCPS_SYN_SENT;
		tp->t_timer[TCPT_KEEP] = TCPTV_KEEP_INIT;
		TCPMISC_LOCK();
		tp->iss = tcp_iss; tcp_iss += TCP_ISSINCR/2;
		TCPMISC_UNLOCK();
		tcp_sendseqinit(tp);
		error = tcp_output(tp);
		break;

	/*
	 * Create a TCP connection between two sockets.
	 */
	case PRU_CONNECT2:
		error = EOPNOTSUPP;
		break;

	/*
	 * Initiate disconnect from peer.
	 * If connection never passed embryonic stage, just drop;
	 * else if don't need to let data drain, then can just drop anyways,
	 * else have to begin TCP shutdown process: mark socket disconnecting,
	 * drain unread data, state switch to reflect user close, and
	 * send segment (e.g. FIN) to peer.  Socket will be really disconnected
	 * when peer sends FIN and acks ours.
	 *
	 * SHOULD IMPLEMENT LATER PRU_CONNECT VIA REALLOC TCPCB.
	 */
	case PRU_DISCONNECT:
		tp = tcp_disconnect(tp);
		break;

	/*
	 * Accept a connection.  Essentially all the work is
	 * done at higher levels; just return the address
	 * of the peer, storing through addr.
	 */
	case PRU_ACCEPT: {
		struct sockaddr_in *sin = mtod(nam, struct sockaddr_in *);

		nam->m_len = sizeof (struct sockaddr_in);
		sin->sin_family = AF_INET;
		sin->sin_len = sizeof(*sin);
		sin->sin_port = inp->inp_fport;
		sin->sin_addr = inp->inp_faddr;
		break;
		}

	/*
	 * Mark the connection as being incapable of further output.
	 */
	case PRU_SHUTDOWN:
		socantsendmore(so);
		tp = tcp_usrclosed(tp);
		if (tp)
			error = tcp_output(tp);
		break;

	/*
	 * After a receive, possibly send window update to peer.
	 */
	case PRU_RCVD:
		(void) tcp_output(tp);
		break;

	/*
	 * Do a send by putting data in output queue and updating urgent
	 * marker if URG set.  Possibly send more data.
	 */
	case PRU_SEND:
		SOCKBUF_LOCK(&so->so_snd);
		sbappend(&so->so_snd, m);
		SOCKBUF_UNLOCK(&so->so_snd);
		error = tcp_output(tp);
		break;

	/*
	 * Abort the TCP.
	 */
	case PRU_ABORT:
		tp = tcp_drop(tp, ECONNABORTED);
		break;

	case PRU_SENSE:
		INPCB_UNLOCK(inp);
		((struct stat *) m)->st_blksize = so->so_snd.sb_hiwat;
		return (0);

	case PRU_RCVOOB:
		if ((so->so_oobmark == 0 &&
		    (so->so_state & SS_RCVATMARK) == 0) ||
		    so->so_options & SO_OOBINLINE ||
		    tp->t_oobflags & TCPOOB_HADDATA) {
			error = EINVAL;
			break;
		}
		if ((tp->t_oobflags & TCPOOB_HAVEDATA) == 0) {
			error = EWOULDBLOCK;
			break;
		}
		m->m_len = 1;
		*mtod(m, caddr_t) = tp->t_iobc;
		if (((int)nam & MSG_PEEK) == 0)
			tp->t_oobflags ^= (TCPOOB_HAVEDATA | TCPOOB_HADDATA);
		break;

	case PRU_SENDOOB:
		SOCKBUF_LOCK(&so->so_snd);
		/* sosend checks this...
		if (sbspace(&so->so_snd) < -512) {
			SOCKBUF_UNLOCK(&so->so_snd);
			m_freem(m);
			error = ENOBUFS;
			break;
		}
		*/
		/*
		 * ... If tcp_compat_42:
		 * According to RFC961 (Assigned Protocols),
		 * the urgent pointer points to the last octet
		 * of urgent data.  We continue, however,
		 * to consider it to indicate the first octet
		 * of data past the urgent section.
		 * Otherwise, snd_up should be one lower.
		 */
		sbappend(&so->so_snd, m);
		tp->snd_up = tp->snd_una + so->so_snd.sb_cc - (!tcp_compat_42);
		SOCKBUF_UNLOCK(&so->so_snd);
		tp->t_force = 1;
		error = tcp_output(tp);
		tp->t_force = 0;
		break;

	case PRU_SOCKADDR:
		in_setsockaddr(inp, nam);
		break;

	case PRU_PEERADDR:
		in_setpeeraddr(inp, nam);
		break;

	/*
	 * TCP slow timer went off; going through this
	 * routine for tracing's sake.
	 */
	case PRU_SLOWTIMO:
		tp = tcp_timers(tp, (int)nam);
		req |= (int)nam << 8;		/* for debug's sake */
		break;

	default:
		panic("tcp_usrreq");
	}
	if (tp && (so->so_options & SO_DEBUG))
		tcp_trace(TA_USER, ostate, tp, (struct tcpiphdr *)0, req);

	/* Only unlock if we still have a connection. */
	if (tp && (req & 0xff) != PRU_SLOWTIMO)
		INPCB_UNLOCK(inp);
	if (control)
		m_freem(control);
	return (error);
}

tcp_ctloutput(op, so, level, optname, mp)
	int op;
	struct socket *so;
	int level, optname;
	struct mbuf **mp;
{
	int error = 0;
	struct inpcb *inp = sotoinpcb(so);
	register struct tcpcb *tp = intotcpcb(inp);
	register struct mbuf *m;

	LOCK_ASSERT("tcp_ctloutput", SOCKET_ISLOCKED(so));

	if (level != IPPROTO_TCP)
		error = ip_ctloutput(op, so, level, optname, mp);

	else switch (op) {

	INPCB_LOCK(inp);
		
	case PRCO_SETOPT:
		m = *mp;
		switch (optname) {

		case TCP_NODELAY:
			if (m == NULL || m->m_len < sizeof (int))
				error = EINVAL;
			else if (*mtod(m, int *))
				tp->t_flags |= TF_NODELAY;
			else
				tp->t_flags &= ~TF_NODELAY;
			break;

		case TCP_MAXSEG:	/* not yet */
		default:
			error = EINVAL;
			break;
		}
		if (m)
			(void) m_free(m);
		break;

	case PRCO_GETOPT:
		*mp = m = m_get(M_WAIT, MT_SOOPTS);
		m->m_len = sizeof(int);

		switch (optname) {
		case TCP_NODELAY:
			*mtod(m, int *) = tp->t_flags & TF_NODELAY;
			break;
		case TCP_MAXSEG:
			*mtod(m, int *) = tp->t_maxseg;
			break;
		default:
			error = EINVAL;
			break;
		}
		break;

	INPCB_UNLOCK(inp);

	}

	return (error);
}

u_long	tcp_sendspace = 1024*4;
u_long	tcp_recvspace = 1024*4;

/*
 * Attach TCP protocol to socket, allocating
 * internet protocol control block, tcp control block,
 * buffer space, and entering LISTEN state if to accept connections.
 */
tcp_attach(so)
	struct socket *so;
{
	register struct tcpcb *tp;
	struct inpcb *inp;
	int error;

	if (so->so_snd.sb_hiwat == 0 || so->so_rcv.sb_hiwat == 0) {
		error = soreserve(so, tcp_sendspace, tcp_recvspace);
		if (error)
			return (error);
	}
	error = in_pcballoc(so, &tcb);
	if (error)
		return (error);
	inp = sotoinpcb(so);
	INPCB_LOCK(inp);
	tp = tcp_newtcpcb(inp);
	if (tp == 0) {
		int nofd = so->so_state & SS_NOFDREF;	/* XXX */

		so->so_state &= ~SS_NOFDREF;	/* don't free the socket yet */
		in_pcbdetach(inp);
		so->so_state |= nofd;
		return (ENOBUFS);
	}
	tp->t_state = TCPS_CLOSED;
	return (0);
}

/*
 * Initiate (or continue) disconnect.
 * If embryonic state, just send reset (once).
 * If in ``let data drain'' option and linger null, just drop.
 * Otherwise (hard), mark socket disconnecting and drop
 * current input data; switch states based on user close, and
 * send segment to peer (with FIN).
 */
struct tcpcb *
tcp_disconnect(tp)
	register struct tcpcb *tp;
{
	struct socket *so = tp->t_inpcb->inp_socket;

	if (tp->t_state < TCPS_ESTABLISHED)
		tp = tcp_close(tp);
	else if ((so->so_options & SO_LINGER) && so->so_linger == 0)
		tp = tcp_drop(tp, 0);
	else {
		soisdisconnecting(so);
		sbflush(&so->so_rcv);
		tp = tcp_usrclosed(tp);
		if (tp)
			(void) tcp_output(tp);
	}
	return (tp);
}

/*
 * User issued close, and wish to trail through shutdown states:
 * if never received SYN, just forget it.  If got a SYN from peer,
 * but haven't sent FIN, then go to FIN_WAIT_1 state to send peer a FIN.
 * If already got a FIN from peer, then almost done; go to LAST_ACK
 * state.  In all other cases, have already sent FIN to peer (e.g.
 * after PRU_SHUTDOWN), and just have to play tedious game waiting
 * for peer to send FIN or not respond to keep-alives, etc.
 * We can let the user exit from the close as soon as the FIN is acked.
 */
struct tcpcb *
tcp_usrclosed(tp)
	register struct tcpcb *tp;
{

	switch (tp->t_state) {

	case TCPS_CLOSED:
	case TCPS_LISTEN:
	case TCPS_SYN_SENT:
		tp->t_state = TCPS_CLOSED;
		tp = tcp_close(tp);
		break;

	case TCPS_SYN_RECEIVED:
	case TCPS_ESTABLISHED:
		tp->t_state = TCPS_FIN_WAIT_1;
		break;

	case TCPS_CLOSE_WAIT:
		tp->t_state = TCPS_LAST_ACK;
		break;
	}
	if (tp && tp->t_state >= TCPS_FIN_WAIT_2)
		soisdisconnected(tp->t_inpcb->inp_socket);
	return (tp);
}
