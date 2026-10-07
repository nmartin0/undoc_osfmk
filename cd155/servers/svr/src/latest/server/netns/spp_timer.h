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
 * $Log: spp_timer.h,v $
 * Revision 1.5  1994/11/18  20:36:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:14:31  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:36:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:02:49  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:31:33  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:28  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:51  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:18  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:49:45  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:21  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:38:26  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:20:17  gm]
 * 
 * Revision 1.4  90/07/27  09:03:59  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:19:34  tmt]
 * 
 * $EndLog$
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
 *	Base:	spp_timer.h	7.3 (Berkeley) 6/28/90
 */

/*
 * Definitions of the SPP timers.  These timers are counted
 * down PR_SLOWHZ times a second.
 */
#define	SPPT_NTIMERS	4

#define	SPPT_REXMT	0		/* retransmit */
#define	SPPT_PERSIST	1		/* retransmit persistance */
#define	SPPT_KEEP	2		/* keep alive */
#define	SPPT_2MSL	3		/* 2*msl quiet time timer */

/*
 * The SPPT_REXMT timer is used to force retransmissions.
 * The SPP has the SPPT_REXMT timer set whenever segments
 * have been sent for which ACKs are expected but not yet
 * received.  If an ACK is received which advances tp->snd_una,
 * then the retransmit timer is cleared (if there are no more
 * outstanding segments) or reset to the base value (if there
 * are more ACKs expected).  Whenever the retransmit timer goes off,
 * we retransmit one unacknowledged segment, and do a backoff
 * on the retransmit timer.
 *
 * The SPPT_PERSIST timer is used to keep window size information
 * flowing even if the window goes shut.  If all previous transmissions
 * have been acknowledged (so that there are no retransmissions in progress),
 * and the window is too small to bother sending anything, then we start
 * the SPPT_PERSIST timer.  When it expires, if the window is nonzero,
 * we go to transmit state.  Otherwise, at intervals send a single byte
 * into the peer's window to force him to update our window information.
 * We do this at most as often as SPPT_PERSMIN time intervals,
 * but no more frequently than the current estimate of round-trip
 * packet time.  The SPPT_PERSIST timer is cleared whenever we receive
 * a window update from the peer.
 *
 * The SPPT_KEEP timer is used to keep connections alive.  If an
 * connection is idle (no segments received) for SPPTV_KEEP amount of time,
 * but not yet established, then we drop the connection.  If the connection
 * is established, then we force the peer to send us a segment by sending:
 *	<SEQ=SND.UNA-1><ACK=RCV.NXT><CTL=ACK>
 * This segment is (deliberately) outside the window, and should elicit
 * an ack segment in response from the peer.  If, despite the SPPT_KEEP
 * initiated segments we cannot elicit a response from a peer in SPPT_MAXIDLE
 * amount of time, then we drop the connection.
 */

#define	SPP_TTL		30		/* default time to live for SPP segs */
/*
 * Time constants.
 */
#define	SPPTV_MSL	( 15*PR_SLOWHZ)		/* max seg lifetime */
#define	SPPTV_SRTTBASE	0			/* base roundtrip time;
						   if 0, no idea yet */
#define	SPPTV_SRTTDFLT	(  3*PR_SLOWHZ)		/* assumed RTT if no info */

#define	SPPTV_PERSMIN	(  5*PR_SLOWHZ)		/* retransmit persistance */
#define	SPPTV_PERSMAX	( 60*PR_SLOWHZ)		/* maximum persist interval */

#define	SPPTV_KEEP	( 75*PR_SLOWHZ)		/* keep alive - 75 secs */
#define	SPPTV_MAXIDLE	(  8*SPPTV_KEEP)	/* maximum allowable idle
						   time before drop conn */

#define	SPPTV_MIN	(  1*PR_SLOWHZ)		/* minimum allowable value */
#define	SPPTV_REXMTMAX	( 64*PR_SLOWHZ)		/* max allowable REXMT value */

#define	SPP_LINGERTIME	120			/* linger at most 2 minutes */

#define	SPP_MAXRXTSHIFT	12			/* maximum retransmits */

#ifndef	CONST
#define	CONST
#endif

#ifdef	SPPTIMERS
CONST	char *spptimers[] =
    { "REXMT", "PERSIST", "KEEP", "2MSL" };
#endif

/*
 * Force a time value to be in a certain range.
 */
#define	SPPT_RANGESET(tv, value, tvmin, tvmax) { \
	(tv) = (value); \
	if ((tv) < (tvmin)) \
		(tv) = (tvmin); \
	else if ((tv) > (tvmax)) \
		(tv) = (tvmax); \
}

#ifdef _KERNEL
extern int spp_backoff[];
#endif
