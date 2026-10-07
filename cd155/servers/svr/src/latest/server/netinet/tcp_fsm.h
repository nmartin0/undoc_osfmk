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
 * $Log: tcp_fsm.h,v $
 * Revision 1.5  1995/02/10  23:41:08  stans
 *  Guard aginst multiple includes with outside #ifndef
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.4  1994/11/18  20:35:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:27:00  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:10  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:31  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:47  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:21:45  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:44:19  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:36:43  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:35:40  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:16:12  gm]
 * 
 * Revision 1.6  90/07/27  09:01:51  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:37:01  tmt]
 * 
 * Revision 1.5  90/04/27  19:19:01  devrcs
 * 	Fix CONST, remove trailing commas.
 * 	[90/04/20  13:01:27  tmt]
 * 
 * Revision 1.4  90/01/18  08:47:37  gm
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
 * Revision 1.3  90/01/02  20:11:36  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:14:47  gm
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
 *	Base:	tcp_fsm.h	7.3 (Berkeley) 6/29/88
 *	Merged:	tcp_fsm.h	7.4 (Berkeley) 6/28/90
 */

#ifndef	_NETINET_TCP_FSM_H_
#define	_NETINET_TCP_FSM_H_ 1

/*
 * TCP FSM state definitions.
 * Per RFC793, September, 1981.
 */

#define	TCP_NSTATES	11

#define	TCPS_CLOSED		0	/* closed */
#define	TCPS_LISTEN		1	/* listening for connection */
#define	TCPS_SYN_SENT		2	/* active, have sent syn */
#define	TCPS_SYN_RECEIVED	3	/* have send and received syn */
/* states < TCPS_ESTABLISHED are those where connections not established */
#define	TCPS_ESTABLISHED	4	/* established */
#define	TCPS_CLOSE_WAIT		5	/* rcvd fin, waiting for close */
/* states > TCPS_CLOSE_WAIT are those where user has closed */
#define	TCPS_FIN_WAIT_1		6	/* have closed, sent fin */
#define	TCPS_CLOSING		7	/* closed xchd FIN; await FIN ACK */
#define	TCPS_LAST_ACK		8	/* had fin and close; await FIN ACK */
/* states > TCPS_CLOSE_WAIT && < TCPS_FIN_WAIT_2 await ACK of FIN */
#define	TCPS_FIN_WAIT_2		9	/* have closed, fin is acked */
#define	TCPS_TIME_WAIT		10	/* in 2*msl quiet wait after close */

#define	TCPS_HAVERCVDSYN(s)	((s) >= TCPS_SYN_RECEIVED)
#define	TCPS_HAVERCVDFIN(s)	((s) >= TCPS_TIME_WAIT)

#ifndef	CONST
#define	CONST
#endif

#ifdef	TCPOUTFLAGS
/*
 * Flags used when sending segments in tcp_output.
 * Basic flags (TH_RST,TH_ACK,TH_SYN,TH_FIN) are totally
 * determined by state, with the proviso that TH_FIN is sent only
 * if all data queued for output is included in the segment.
 */
CONST	u_char	tcp_outflags[TCP_NSTATES] = {
    TH_RST|TH_ACK, 0, TH_SYN, TH_SYN|TH_ACK,
    TH_ACK, TH_ACK,
    TH_FIN|TH_ACK, TH_FIN|TH_ACK, TH_FIN|TH_ACK, TH_ACK, TH_ACK
};
#endif

#ifdef KPROF
int	tcp_acounts[TCP_NSTATES][PRU_NREQ];
#endif

#ifdef	TCPSTATES
CONST	char *tcpstates[] = {
	"CLOSED",	"LISTEN",	"SYN_SENT",	"SYN_RCVD",
	"ESTABLISHED",	"CLOSE_WAIT",	"FIN_WAIT_1",	"CLOSING",
	"LAST_ACK",	"FIN_WAIT_2",	"TIME_WAIT"
};
#endif

#endif	/* _NETINET_TCP_FSM_H_ */
