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
 * $Log: tcp_timer.h,v $
 * Revision 1.4  1994/11/18  20:35:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:27:26  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:25  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:57  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:12  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:44:48  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:37:19  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7.4.2  91/03/15  17:49:51  tmt
 * 	Update extern declaration for tcp_backoff.
 * 	[91/03/13  19:13:07  tmt]
 * 
 * Revision 1.7  90/10/07  14:36:12  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:16:57  gm]
 * 
 * Revision 1.6  90/07/27  09:02:18  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:42:03  tmt]
 * 
 * Revision 1.5  90/04/27  19:19:24  devrcs
 * 	Fix CONST.
 * 	[90/04/20  13:05:28  tmt]
 * 
 * Revision 1.4  90/01/18  08:48:07  gm
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
 * Revision 1.3  90/01/02  20:11:37  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:16:15  gm
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
 *	Base:	tcp_timer.h	7.6 (Berkeley) 6/29/88
 *	Merged:	tcp_timer.h	7.8 (Berkeley) 6/28/90
 */

/*
 * Definitions of the TCP timers.  These timers are counted
 * down PR_SLOWHZ times a second.
 */
#define	TCPT_NTIMERS	4

#define	TCPT_REXMT	0		/* retransmit */
#define	TCPT_PERSIST	1		/* retransmit persistance */
#define	TCPT_KEEP	2		/* keep alive */
#define	TCPT_2MSL	3		/* 2*msl quiet time timer */

/*
 * The TCPT_REXMT timer is used to force retransmissions.
 * The TCP has the TCPT_REXMT timer set whenever segments
 * have been sent for which ACKs are expected but not yet
 * received.  If an ACK is received which advances tp->snd_una,
 * then the retransmit timer is cleared (if there are no more
 * outstanding segments) or reset to the base value (if there
 * are more ACKs expected).  Whenever the retransmit timer goes off,
 * we retransmit one unacknowledged segment, and do a backoff
 * on the retransmit timer.
 *
 * The TCPT_PERSIST timer is used to keep window size information
 * flowing even if the window goes shut.  If all previous transmissions
 * have been acknowledged (so that there are no retransmissions in progress),
 * and the window is too small to bother sending anything, then we start
 * the TCPT_PERSIST timer.  When it expires, if the window is nonzero,
 * we go to transmit state.  Otherwise, at intervals send a single byte
 * into the peer's window to force him to update our window information.
 * We do this at most as often as TCPT_PERSMIN time intervals,
 * but no more frequently than the current estimate of round-trip
 * packet time.  The TCPT_PERSIST timer is cleared whenever we receive
 * a window update from the peer.
 *
 * The TCPT_KEEP timer is used to keep connections alive.  If an
 * connection is idle (no segments received) for TCPTV_KEEP_INIT amount of time,
 * but not yet established, then we drop the connection.  Once the connection
 * is established, if the connection is idle for TCPTV_KEEP_IDLE time
 * (and keepalives have been enabled on the socket), we begin to probe
 * the connection.  We force the peer to send us a segment by sending:
 *	<SEQ=SND.UNA-1><ACK=RCV.NXT><CTL=ACK>
 * This segment is (deliberately) outside the window, and should elicit
 * an ack segment in response from the peer.  If, despite the TCPT_KEEP
 * initiated segments we cannot elicit a response from a peer in TCPT_MAXIDLE
 * amount of time probing, then we drop the connection.
 */

#define	TCP_TTL		60		/* default time to live for TCP segs */
/*
 * Time constants.
 */
#define	TCPTV_MSL	( 30*PR_SLOWHZ)		/* max seg lifetime (hah!) */
#define	TCPTV_SRTTBASE	0			/* base roundtrip time;
						   if 0, no idea yet */
#define	TCPTV_SRTTDFLT	(  3*PR_SLOWHZ)		/* assumed RTT if no info */

#define	TCPTV_PERSMIN	(  5*PR_SLOWHZ)		/* retransmit persistance */
#define	TCPTV_PERSMAX	( 60*PR_SLOWHZ)		/* maximum persist interval */

#define	TCPTV_KEEP_INIT	( 75*PR_SLOWHZ)		/* initial connect keep alive */
#define	TCPTV_KEEP_IDLE	(120*60*PR_SLOWHZ)	/* dflt time before probing */
#define	TCPTV_KEEPINTVL	( 75*PR_SLOWHZ)		/* default probe interval */
#define	TCPTV_KEEPCNT	8			/* max probes before drop */

#define	TCPTV_MIN	(  1*PR_SLOWHZ)		/* minimum allowable value */
#define	TCPTV_REXMTMAX	( 64*PR_SLOWHZ)		/* max allowable REXMT value */

#define	TCP_LINGERTIME	120			/* linger at most 2 minutes */

#define	TCP_MAXRXTSHIFT	12			/* maximum retransmits */

#ifndef	CONST
#define	CONST
#endif

#ifdef	TCPTIMERS
CONST	char *tcptimers[] =
    { "REXMT", "PERSIST", "KEEP", "2MSL" };
#endif

/*
 * Force a time value to be in a certain range.
 */
#define	TCPT_RANGESET(tv, value, tvmin, tvmax) { \
	(tv) = (value); \
	if ((tv) < (tvmin)) \
		(tv) = (tvmin); \
	else if ((tv) > (tvmax)) \
		(tv) = (tvmax); \
}

#ifdef _KERNEL
extern int tcp_keepidle;		/* time before keepalive probes begin */
extern int tcp_keepintvl;		/* time between keepalive probes */
extern int tcp_maxidle;			/* time to drop after starting probes */
extern int tcp_ttl;			/* time to live for TCP segs */
extern CONST int tcp_backoff[];
#endif
