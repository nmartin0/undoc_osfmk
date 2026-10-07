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
 * $Log: tcp_debug.h,v $
 * Revision 1.5  1995/02/10  23:40:03  stans
 *  Guard aginst being included multiple times with a outside controlling ifndef.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing:WW05 sats
 *
 * Revision 1.4  1994/11/18  20:35:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:26:59  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:08  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:29  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:21:42  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:44:15  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:36:39  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:35:36  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:16:05  gm]
 * 
 * Revision 1.6  90/07/27  09:01:48  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:36:45  tmt]
 * 
 * Revision 1.5  90/04/27  19:18:56  devrcs
 * 	Remove data declarations. Fix CONST.
 * 	[90/04/20  13:00:37  tmt]
 * 
 * Revision 1.4  90/01/18  08:47:34  gm
 * 	Add a CONST.
 * 	[90/01/08  16:11:25  tmt]
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
 * Revision 1.3  90/01/02  20:11:35  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:14:32  gm
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
 *	Base:	tcp_debug.h	7.3 (Berkeley) 6/29/88
 *	Merged:	tcp_debug.h	7.4 (Berkeley) 6/28/90
 */

#ifndef	_NETINET_TCP_DEBUG_H_
#define	_NETINET_TCP_DEBUG_H_ 1

struct	tcp_debug {
	n_time	td_time;
	short	td_act;
	short	td_ostate;
	caddr_t	td_tcb;
	struct	tcpiphdr td_ti;
	short	td_req;
	struct	tcpcb td_cb;
};

#define	TA_INPUT 	0
#define	TA_OUTPUT	1
#define	TA_USER		2
#define	TA_RESPOND	3
#define	TA_DROP		4

#ifndef	CONST
#define	CONST
#endif

#ifdef TANAMES
CONST	char	*tanames[] =
    { "input", "output", "user", "respond", "drop" };
#endif

#define	TCP_NDEBUG 100
extern	struct	tcp_debug tcp_debug[TCP_NDEBUG];
extern	int	tcp_debx;

#endif	/* _NETINET_TCP_DEBUG_H_ */
