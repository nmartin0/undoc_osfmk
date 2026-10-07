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
 * $Log: udp_var.h,v $
 * Revision 1.4  1994/11/18  20:35:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:27:37  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:36  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:30:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:20  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:32  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:45:16  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:37:48  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10  90/10/07  14:36:43  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:17:40  gm]
 * 
 * Revision 1.9  90/07/27  09:02:43  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:47:00  tmt]
 * 
 * Revision 1.8  90/06/22  20:39:44  devrcs
 * 	Parallelization repairs. Socket locks, then inpcb locks.
 * 	Take inpcbhead locks directly. Do refcounting right.
 * 	[90/06/07  16:17:34  tmt]
 * 
 * Revision 1.7  90/04/27  19:20:09  devrcs
 * 	Use _KERNEL. Move data declarations.
 * 	[90/04/20  13:08:42  tmt]
 * 
 * Revision 1.6  90/03/27  13:22:54  gm
 * 	Change args to lock_init2() [gmf]
 * 
 * Revision 1.4  90/01/18  08:48:31  gm
 * 	Put statistics lock under KERNEL && LOCK_NETSTATS.
 * 	[90/01/12  17:07:45  tmt]
 * 
 * 	Global udp lock is now in udb.
 * 	[90/01/08  16:16:01  tmt]
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
 * Revision 1.3  90/01/03  12:42:18  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:36  gm]
 * 
 * Revision 1.2  89/12/26  10:17:39  gm
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
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
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
 *	Base:	udp_var.h	7.5 (Berkeley) 9/20/89
 *	Merged:	udp_var.h	7.7 (Berkeley) 6/28/90
 */

/*
 * UDP kernel structures and variables.
 */
struct	udpiphdr {
	struct 	ipovly ui_i;		/* overlaid ip structure */
	struct	udphdr ui_u;		/* udp header */
};
#define	ui_next		ui_i.ih_next
#define	ui_prev		ui_i.ih_prev
#define	ui_x1		ui_i.ih_x1
#define	ui_pr		ui_i.ih_pr
#define	ui_len		ui_i.ih_len
#define	ui_src		ui_i.ih_src
#define	ui_dst		ui_i.ih_dst
#define	ui_sport	ui_u.uh_sport
#define	ui_dport	ui_u.uh_dport
#define	ui_ulen		ui_u.uh_ulen
#define	ui_sum		ui_u.uh_sum

struct	udpstat {
				/* input statistics: */
	int	udps_ipackets;		/* total input packets */
	int	udps_hdrops;		/* packet shorter than header */
	int	udps_badsum;		/* checksum error */
	int	udps_badlen;		/* data length larger than packet */
	int	udps_noport;		/* no socket on port */
	int	udps_noportbcast;	/* of above, arrived as broadcast */
	int	udps_fullsock;		/* not delivered, input socket full */
	int	udpps_pcbcachemiss;	/* input packets missing pcb cache */
				/* output statistics: */
	int	udps_opackets;		/* total output packets */
#if	defined(_KERNEL) && LOCK_NETSTATS
	simple_lock_data_t udps_lock;		/* statistics lock */
#endif
};

#define	UDP_TTL		30	/* default time to live for UDP packets */

#ifdef	_KERNEL
extern	struct	inpcb udb;
extern	struct	udpstat udpstat;
#endif
