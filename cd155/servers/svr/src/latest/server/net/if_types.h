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
 * $Log: if_types.h,v $
 * Revision 1.6  1994/11/18  20:33:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  18:07:59  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:28:07  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  18:59:43  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1993/03/25  23:23:37  cfj
 * T9 Merge.
 *
 * Revision 1.2.8.1  1993/03/24  23:40:22  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.1.1.1  1993/05/03  17:32:38  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1993/02/22  17:09:30  mjl
 * Add interface type for MI interface.
 *
 * Revision 2.2  91/08/31  13:39:58  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:33:22  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:32:04  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:10:32  gm]
 * 
 * Revision 1.6  90/07/27  08:59:06  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:35:55  tmt]
 * 
 * Revision 1.5  90/04/27  19:12:28  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:27:07  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:39  gm
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
 * Revision 1.3  90/01/02  20:10:52  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:45:00  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 Regents of the University of California.
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
 *	Base:	if_types.h	7.2 (Berkeley) 9/20/89
 *	Merged:	if_types.h	7.3 (Berkeley) 6/28/90
 */


/* interface types for benefit of parsing media address headers */
#define IFT_OTHER	0x1		/* none of the following */
#define IFT_1822	0x2		/* old-style arpanet imp */
#define IFT_HDH1822	0x3		/* HDH arpanet imp */
#define IFT_X25DDN	0x4		/* x25 to imp */
#define IFT_X25		0x5		/* PDN X25 interface */
#define	IFT_ETHER	0x6		/* Ethernet I or II */
#define	IFT_ISO88023	0x7		/* CMSA CD */
#define	IFT_ISO88024	0x8		/* Token Bus */
#define	IFT_ISO88025	0x9		/* Token Ring */
#define	IFT_ISO88026	0xa		/* MAN */
#define	IFT_STARLAN	0xb
#define	IFT_P10		0xc		/* Proteon 10MBit ring */
#define	IFT_P80		0xd		/* Proteon 10MBit ring */
#define IFT_HY		0xe		/* Hyperchannel */
#define IFT_FDDI	0xf
#define IFT_LAPB	0x10
#define IFT_SDLC	0x11
#define IFT_T1		0x12
#define IFT_CEPT	0x13
#define IFT_ISDNBASIC	0x14
#define IFT_ISDNPRIMARY	0x15
#define IFT_PTPSERIAL	0x16
#define	IFT_LOOP	0x18		/* loopback */
#define IFT_EON		0x19		/* ISO over IP */
#define	IFT_XETHER	0x1a		/* obsolete 3MB experimental ethernet */
#define	IFT_NSIP	0x1b		/* XNS over IP */
#define	IFT_SLIP	0x1c		/* IP over generic TTY */
#define IFT_MI		0x1d		/* Mach IPC */
