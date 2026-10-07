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
 * $Log: icmp_var.h,v $
 * Revision 1.4  1994/11/18  20:34:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:25:24  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:21  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:26:36  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:11  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:42:16  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:34:37  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:33:37  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:12:59  gm]
 * 
 * Revision 1.7  90/07/27  09:00:15  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:23:42  tmt]
 * 
 * Revision 1.6  90/04/27  19:14:32  devrcs
 * 	Use _KERNEL. Move declaration of icmpstat out.
 * 	[90/04/20  12:44:12  tmt]
 * 
 * Revision 1.4  90/01/18  08:44:38  gm
 * 	#ifdefs for user code compatability.
 * 	[90/01/16  13:42:15  tmt]
 * 
 * 	Make structure overlay constant regardless of config options.
 * 	[90/01/08  15:59:47  tmt]
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
 * Revision 1.3  90/01/03  12:41:14  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:27  gm]
 * 
 * Revision 1.2  89/12/26  09:49:00  gm
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
 *	Base:	icmp_var.h	7.4 (Berkeley) 6/29/88
 *	Merged:	icmp_var.h	7.5 (Berkeley) 6/28/90
 */

/*
 * Variables related to this implementation
 * of the internet control message protocol.
 */
struct	icmpstat {
/* statistics related to icmp packets generated */
	int	icps_error;		/* # of calls to icmp_error */
	int	icps_oldshort;		/* no error 'cuz old ip too short */
	int	icps_oldicmp;		/* no error 'cuz old was icmp */
	int	icps_outhist[ICMP_MAXTYPE + 1];
/* statistics related to input messages processed */
 	int	icps_badcode;		/* icmp_code out of range */
	int	icps_tooshort;		/* packet < ICMP_MINLEN */
	int	icps_checksum;		/* bad checksum */
	int	icps_badlen;		/* calculated bound mismatch */
	int	icps_reflect;		/* number of responses */
	int	icps_inhist[ICMP_MAXTYPE + 1];
#if	defined(_KERNEL) && LOCK_NETSTATS
	simple_lock_data_t	icps_lock;	/* statistics lock */
#endif
};

#ifdef _KERNEL
extern struct	icmpstat icmpstat;
#endif
