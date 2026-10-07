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
 * $Log: tcp_var.h,v $
 * Revision 1.4  1994/11/18  20:35:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:27:30  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:29  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:30:03  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:11  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:18  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/10/22  23:02:39  hobbes
 * Added the RFC_1323 extensions.
 *
 * Revision 2.2  1991/08/31  13:44:57  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:37:30  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11  90/10/07  14:36:24  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:17:11  gm]
 * 
 * Revision 1.10  90/08/09  13:26:50  devrcs
 * 	Finish Reno update - REASS_MBUF.
 * 	[90/07/25  11:11:16  tmt]
 * 
 * Revision 1.9  90/07/27  09:02:26  devrcs
 * 	Update to BSD Reno release.
 * 	Move t_template to tcpcb instead of allocating it.
 * 	Keep maxrcvd, rearrange tcpcb.
 * 	[90/07/19  17:44:14  tmt]
 * 
 * Revision 1.8  90/06/22  20:39:37  devrcs
 * 	Parallelization repairs. Socket locks, then inpcb locks.
 * 	Take inpcbhead locks directly. Do refcounting right.
 * 	Add t_timestamp for slowtimo resync.
 * 	[90/06/07  16:16:32  tmt]
 * 
 * Revision 1.7  90/04/27  19:19:36  devrcs
 * 	Use _KERNEL. Remove data, function declarations.
 * 	[90/04/20  13:06:45  tmt]
 * 
 * Revision 1.6  90/03/27  13:22:51  gm
 * 	Change args to lock_init2() [gmf]
 * 
 * Revision 1.4  90/01/18  08:48:16  gm
 * 	Put statistics lock under KERNEL && LOCK_NETSTATS.
 * 	[90/01/12  17:07:07  tmt]
 * 
 * 	Global tcp lock is now in tcb.
 * 	[90/01/08  16:14:28  tmt]
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
 * Revision 1.3  90/01/03  12:42:11  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:30  gm]
 * 
 * Revision 1.2  89/12/26  10:16:44  gm
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
 *	Base:	tcp_var.h	7.8 (Berkeley) 6/29/88
 *	Merged:	tcp_var.h	7.10 (Berkeley) 6/28/90
 */

/*
 * Kernel variables for tcp.
 */

/*
 * Tcp control block, one per tcp; fields:
 */
struct tcpcb {
	struct	tcpiphdr *seg_next;	/* sequencing queue */
	struct	tcpiphdr *seg_prev;
	short	t_state;		/* state of this connection */
	short	t_softerror;		/* possible error not yet reported */
	short	t_timer[TCPT_NTIMERS];	/* tcp timers */
	short	t_rxtshift;		/* log(2) of rexmt exp. backoff */
	short	t_rxtcur;		/* current retransmit value */
	short	t_dupacks;		/* consecutive dup acks recd */
	u_short	t_maxseg;		/* maximum segment size */
	char	t_force;		/* 1 if forcing out a byte */
#ifdef RFC_1323
 	u_short	t_flags;
#else
	u_char  t_flags;
#endif
#define	TF_ACKNOW	0x0001		/* ack peer immediately */
#define	TF_DELACK	0x0002		/* ack, but try to delay it */
#define	TF_NODELAY	0x0004		/* don't delay packets to coalesce */
#define	TF_NOOPT	0x0008		/* don't use tcp options */
#define	TF_SENTFIN	0x0010		/* have sent FIN */
#ifdef RFC_1323
#define TF_REQ_SCALE	0x0020		/* have/will request window scaling */
#define TF_RCVD_SCALE	0x0040		/* other side has requested scaling */
#define TF_REQ_TSTMP	0x0080		/* have/will request time-stamps */
#define TF_RCVD_TSTMP	0x0100		/* a time-stamp was received in SYN */
#define TF_SACK_PERMIT	0x0200		/* other side said I could SACK */
#endif /* RFC_1323 */

/* out-of-band data */
	char	t_oobflags;		/* have some */
	char	t_iobc;			/* input character */
#define	TCPOOB_HAVEDATA	0x01
#define	TCPOOB_HADDATA	0x02

	struct	tcpiphdr t_template;	/* skeletal packet for transmit
					 * (used to be mbuf)
					 */
	struct	inpcb *t_inpcb;		/* back pointer to internet pcb */
	tcp_seq	t_timestamp;		/* used by slowtimo */
/*
 * The following fields are used as in the protocol specification.
 * See RFC783, Dec. 1981, page 21.
 */
/* send sequence variables */
	tcp_seq	snd_una;		/* send unacknowledged */
	tcp_seq	snd_nxt;		/* send next */
	tcp_seq	snd_up;			/* send urgent pointer */
	tcp_seq	snd_wl1;		/* window update seg seq number */
	tcp_seq	snd_wl2;		/* window update seg ack number */
	tcp_seq	iss;			/* initial send sequence number */
#ifdef RFC_1323
 	u_long	snd_wnd;		/* send window */
#else
 	u_short	snd_wnd;		/* send window */
#endif

/* receive sequence variables */
#ifdef RFC_1323
	u_long	rcv_wnd;		/* receive window */
#else
	u_short	rcv_wnd;		/* receive window */
#endif
	tcp_seq	rcv_nxt;		/* receive next */
	tcp_seq	rcv_up;			/* receive urgent pointer */
	tcp_seq	irs;			/* initial receive sequence number */
/*
 * Additional variables for this implementation.
 */
/* receive variables */
	tcp_seq	rcv_adv;		/* advertised window */
/* retransmit variables */
	tcp_seq	snd_max;		/* highest sequence number sent;
					 * used to recognize retransmits
					 */
/* congestion control (for slow start, source quench, retransmit after loss) */
#ifdef RFC_1323
	u_long	snd_cwnd;		/* congestion-controlled window */
	u_long	snd_ssthresh;		/* snd_cwnd size threshhold for
					 * slow start exponential to
					 * linear switch
					 */
#else
	u_short	snd_cwnd;		/* congestion-controlled window */
	u_short	snd_ssthresh;		/* snd_cwnd size threshhold for
					 * slow start exponential to
					 * linear switch
					 */
#endif
/*
 * transmit timing stuff.  See below for scale of srtt and rttvar.
 * "Variance" is actually smoothed difference.
 */
	short	t_idle;			/* inactivity time */
	short	t_rtt;			/* round trip time */
	tcp_seq	t_rtseq;		/* sequence number being timed */
	short	t_srtt;			/* smoothed round-trip time */
	short	t_rttvar;		/* variance in round-trip time */
	u_short	t_rttmin;		/* minimum rtt allowed */
#ifdef RFC_1323
	u_long	max_rcvd;		/* most peer has sent into window */
	u_long	max_sndwnd;		/* largest window peer has offered */

/* RFC 1323 stuff */
	u_char	snd_scale;		/* window scaling for send window */
	u_char	rcv_scale;		/* window scaling for recv window */
	u_char	request_r_scale;	/* pending window scaling */
	u_char	requested_s_scale;
	u_long	ts_recent;		/* time-stamp echo data */
	u_long	ts_recent_age;		/* how long since last updated */
	tcp_seq	last_ack_sent;
#else
	u_short	max_rcvd;		/* most peer has sent into window */
	u_short	max_sndwnd;		/* largest window peer has offered */
#endif
};

#define	intotcpcb(ip)	((struct tcpcb *)(ip)->inp_ppcb)
#define	sototcpcb(so)	(intotcpcb(sotoinpcb(so)))

/*
 * The smoothed round-trip time and estimated variance
 * are stored as fixed point numbers scaled by the values below.
 * For convenience, these scales are also used in smoothing the average
 * (smoothed = (1/scale)sample + ((scale-1)/scale)smoothed).
 * With these scales, srtt has 3 bits to the right of the binary point,
 * and thus an "ALPHA" of 0.875.  rttvar has 2 bits to the right of the
 * binary point, and is smoothed with an ALPHA of 0.75.
 */
#define	TCP_RTT_SCALE		8	/* multiplier for srtt; 3 bits frac. */
#define	TCP_RTT_SHIFT		3	/* shift for srtt; 3 bits frac. */
#define	TCP_RTTVAR_SCALE	4	/* multiplier for rttvar; 2 bits */
#define	TCP_RTTVAR_SHIFT	2	/* multiplier for rttvar; 2 bits */

/*
 * The initial retransmission should happen at rtt + 4 * rttvar.
 * Because of the way we do the smoothing, srtt and rttvar
 * will each average +1/2 tick of bias.  When we compute
 * the retransmit timer, we want 1/2 tick of rounding and
 * 1 extra tick because of +-1/2 tick uncertainty in the
 * firing of the timer.  The bias will give us exactly the
 * 1.5 tick we need.  But, because the bias is
 * statistical, we have to test that we don't drop below
 * the minimum feasible timer (which is 2 ticks).
 * This macro assumes that the value of TCP_RTTVAR_SCALE
 * is the same as the multiplier for rttvar.
 */
#define	TCP_REXMTVAL(tp) \
	(((tp)->t_srtt >> TCP_RTT_SHIFT) + (tp)->t_rttvar)

/* XXX
 * We want to avoid doing m_pullup on incoming packets but that
 * means avoiding dtom on the tcp reassembly code.  That in turn means
 * keeping an mbuf pointer in the reassembly queue (since we might
 * have a cluster).  As a quick hack, the source & destination
 * port numbers (which are no longer needed once we've located the
 * tcpcb) are overlayed with an mbuf pointer.
 */
#define REASS_MBUF(ti) (*(struct mbuf **)&((ti)->ti_t))

/*
 * TCP statistics.
 * Many of these should be kept per connection,
 * but that's inconvenient at the moment.
 */
struct	tcpstat {
	u_long	tcps_connattempt;	/* connections initiated */
	u_long	tcps_accepts;		/* connections accepted */
	u_long	tcps_connects;		/* connections established */
	u_long	tcps_drops;		/* connections dropped */
	u_long	tcps_conndrops;		/* embryonic connections dropped */
	u_long	tcps_closed;		/* conn. closed (includes drops) */
	u_long	tcps_segstimed;		/* segs where we tried to get rtt */
	u_long	tcps_rttupdated;	/* times we succeeded */
	u_long	tcps_delack;		/* delayed acks sent */
	u_long	tcps_timeoutdrop;	/* conn. dropped in rxmt timeout */
	u_long	tcps_rexmttimeo;	/* retransmit timeouts */
	u_long	tcps_persisttimeo;	/* persist timeouts */
	u_long	tcps_keeptimeo;		/* keepalive timeouts */
	u_long	tcps_keepprobe;		/* keepalive probes sent */
	u_long	tcps_keepdrops;		/* connections dropped in keepalive */

	u_long	tcps_sndtotal;		/* total packets sent */
	u_long	tcps_sndpack;		/* data packets sent */
	u_long	tcps_sndbyte;		/* data bytes sent */
	u_long	tcps_sndrexmitpack;	/* data packets retransmitted */
	u_long	tcps_sndrexmitbyte;	/* data bytes retransmitted */
	u_long	tcps_sndacks;		/* ack-only packets sent */
	u_long	tcps_sndprobe;		/* window probes sent */
	u_long	tcps_sndurg;		/* packets sent with URG only */
	u_long	tcps_sndwinup;		/* window update-only packets sent */
	u_long	tcps_sndctrl;		/* control (SYN|FIN|RST) packets sent */

	u_long	tcps_rcvtotal;		/* total packets received */
	u_long	tcps_rcvpack;		/* packets received in sequence */
	u_long	tcps_rcvbyte;		/* bytes received in sequence */
	u_long	tcps_rcvbadsum;		/* packets received with ccksum errs */
	u_long	tcps_rcvbadoff;		/* packets received with bad offset */
	u_long	tcps_rcvshort;		/* packets received too short */
	u_long	tcps_rcvduppack;	/* duplicate-only packets received */
	u_long	tcps_rcvdupbyte;	/* duplicate-only bytes received */
	u_long	tcps_rcvpartduppack;	/* packets with some duplicate data */
	u_long	tcps_rcvpartdupbyte;	/* dup. bytes in part-dup. packets */
	u_long	tcps_rcvoopack;		/* out-of-order packets received */
	u_long	tcps_rcvoobyte;		/* out-of-order bytes received */
	u_long	tcps_rcvpackafterwin;	/* packets with data after window */
	u_long	tcps_rcvbyteafterwin;	/* bytes rcvd after window */
	u_long	tcps_rcvafterclose;	/* packets rcvd after "close" */
	u_long	tcps_rcvwinprobe;	/* rcvd window probe packets */
	u_long	tcps_rcvdupack;		/* rcvd duplicate acks */
	u_long	tcps_rcvacktoomuch;	/* rcvd acks for unsent data */
	u_long	tcps_rcvackpack;	/* rcvd ack packets */
	u_long	tcps_rcvackbyte;	/* bytes acked by rcvd acks */
	u_long	tcps_rcvwinupd;		/* rcvd window update packets */
#ifdef RFC_1323
 	u_long	tcps_pawsdrop;		/* segments dropped due to PAWS */
#endif
#if	defined(_KERNEL) && LOCK_NETSTATS
	simple_lock_data_t tcps_lock;	/* statistics lock */
#endif
};

#ifdef _KERNEL
#if	NETSYNC_LOCK
extern	simple_lock_data_t	misc_tcp_lock;
#define TCPMISC_LOCKINIT()	simple_lock_init(&misc_tcp_lock)
#define TCPMISC_LOCK()		simple_lock(&misc_tcp_lock)
#define TCPMISC_UNLOCK()	simple_unlock(&misc_tcp_lock)
#else	/* !NETSYNC_LOCK */
#define TCPMISC_LOCKINIT()
#define TCPMISC_LOCK()
#define TCPMISC_UNLOCK()
#endif

extern	int tcp_compat_42;
extern	struct	inpcb tcb;		/* head of queue of active tcpcb's */
extern	struct	tcpstat tcpstat;	/* tcp statistics */
#ifdef RFC_1323
u_long	tcp_now;		/* For rfc1072&1185 time-stamps */
#endif
#endif
