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
 * $Log: tcp_debug.c,v $
 * Revision 1.5  1994/11/18  20:35:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/05/27  23:53:33  hobbes
 * Removed a duplicate #include of protosw.h
 * that caused the code not to compile when INETPRINTFS
 * were TRUE.
 *
 * Revision 1.3  1993/05/06  20:26:57  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:06  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:26  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:43  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:21:37  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:44:11  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:36:36  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9.4.2  91/04/29  12:10:22  tmt
 * 	Add protosw.h include to quiet compiler warning.
 * 	[91/04/10  09:53:55  tmt]
 * 
 * Revision 1.9  90/10/07  14:35:32  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:15:59  gm]
 * 
 * Revision 1.8  90/07/27  09:01:44  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:36:21  tmt]
 * 
 * Revision 1.7  90/06/22  20:39:19  devrcs
 * 	ANSI-ize the pf() macro.
 * 	[90/06/06  16:55:33  tmt]
 * 
 * Revision 1.6  90/04/27  19:18:54  devrcs
 * 	Move debug data declarations here.
 * 	[90/04/20  12:59:53  tmt]
 * 
 * Revision 1.5  90/04/14  00:33:24  devrcs
 * 	Add a void.
 * 	[90/04/09  16:33:41  tmt]
 * 
 * Revision 1.4  90/01/18  08:47:31  gm
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
 * Revision 1.3  90/01/02  20:11:34  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:14:20  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
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
 *	Base:	tcp_debug.c	7.3 (Berkeley) 6/29/88
 *	Merged:	tcp_debug.c	7.6 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/errno.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/socketvar.h"

#include "net/if.h"
#include "net/route.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/in_pcb.h"

#if	INETPRINTFS
/* load symbolic names */
#define PRUREQUESTS
#define TCPSTATES
#define	TCPTIMERS
#define	TANAMES
#endif

#include "sys/protosw.h"

#include "netinet/ip_var.h"
#include "netinet/tcp.h"
#include "netinet/tcpip.h"
#include "netinet/tcp_fsm.h"
#include "netinet/tcp_seq.h"
#include "netinet/tcp_timer.h"
#include "netinet/tcp_var.h"
#include "netinet/tcp_debug.h"

LOCK_ASSERTL_DECL

struct	tcp_debug tcp_debug[TCP_NDEBUG];
int	tcp_debx;
int	tcpconsdebug = 0;

/*
 * Tcp debug routines
 */
void
tcp_trace(act, ostate, tp, ti, req)
	short act, ostate;
	struct tcpcb *tp;
	struct tcpiphdr *ti;
	int req;
{
	tcp_seq seq, ack;
	int len, flags;
	struct tcp_debug *td;

	TCPMISC_LOCK();
	td = &tcp_debug[tcp_debx++];
	if (tcp_debx == TCP_NDEBUG)
		tcp_debx = 0;
	TCPMISC_UNLOCK();
	td->td_time = iptime();
	td->td_act = act;
	td->td_ostate = ostate;
	td->td_tcb = (caddr_t)tp;
	if (tp)
		td->td_cb = *tp;
	else
		bzero((caddr_t)&td->td_cb, sizeof (*tp));
	if (ti)
		td->td_ti = *ti;
	else
		bzero((caddr_t)&td->td_ti, sizeof (*ti));
	td->td_req = req;
	if (tcpconsdebug == 0)
		return;
#if	INETPRINTFS
	if (tp)
		printf("%x %s:", tp, tcpstates[ostate]);
	else
		printf("???????? ");
	printf("%s ", tanames[act]);
	switch (act) {

	case TA_INPUT:
	case TA_OUTPUT:
	case TA_DROP:
		if (ti == 0)
			break;
		seq = ti->ti_seq;
		ack = ti->ti_ack;
		len = ti->ti_len;
		if (act == TA_OUTPUT) {
			seq = ntohl(seq);
			ack = ntohl(ack);
			len = ntohs((u_short)len);
		}
		if (act == TA_OUTPUT)
			len -= sizeof (struct tcphdr);
		if (len)
			printf("[%x..%x)", seq, seq+len);
		else
			printf("%x", seq);
		printf("@%x, urp=%x", ack, ti->ti_urp);
		flags = ti->ti_flags;
		if (flags) {
#ifndef lint
			char *cp = "<";
#ifdef	__STDC__
#define pf(f) { if (ti->ti_flags&TH_##f) { printf("%s%s", cp, #f); cp = ","; } }
#else
#define pf(f) { if (ti->ti_flags&TH_/**/f) { printf("%s%s", cp, "f"); cp = ","; } }
#endif
			pf(SYN); pf(ACK); pf(FIN); pf(RST); pf(PUSH); pf(URG);
#endif
			printf(">");
		}
		break;

	case TA_USER:
		printf("%s", prurequests[req&0xff]);
		if ((req & 0xff) == PRU_SLOWTIMO)
			printf("<%s>", tcptimers[req>>8]);
		break;
	}
	if (tp)
		printf(" -> %s", tcpstates[tp->t_state]);
	/* print out internal state of tp !?! */
	printf("\n");
	if (tp == 0)
		return;
	printf("\trcv_(nxt,wnd,up) (%x,%x,%x) snd_(una,nxt,max) (%x,%x,%x)\n",
	    tp->rcv_nxt, tp->rcv_wnd, tp->rcv_up, tp->snd_una, tp->snd_nxt,
	    tp->snd_max);
	printf("\tsnd_(wl1,wl2,wnd) (%x,%x,%x)\n",
	    tp->snd_wl1, tp->snd_wl2, tp->snd_wnd);
#endif
}
