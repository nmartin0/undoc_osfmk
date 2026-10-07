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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: msgbuf.h,v $
 * Revision 1.4  1994/11/18  20:40:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:26:11  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:57  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:28  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:09:01  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:43:58  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:52:29  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:42:38  gm]
 * 
 * Revision 1.2  90/01/02  20:15:59  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:22  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:20:28  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.5  89/05/30  10:43:37  rvb
 * 	Rephrased a comment.
 * 	[89/05/17            af]
 * 
 * Revision 2.4  89/03/09  22:05:57  rpd
 * 	More cleanup.
 * 
 * Revision 2.3  89/02/25  17:54:58  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.2  89/01/23  22:27:16  af
 * 	Hack for mips msgbuf allocation
 * 	[89/01/07            af]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)msgbuf.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_MSGBUF_H_
#define _SYS_MSGBUF_H_

#define MSG_MAGIC	0x063061
#define MSG_BSIZE	(4096 - 3 * sizeof (long))

struct	msgbuf {
	long	msg_magic;
	long	msg_bufx;
	long	msg_bufr;
	char	msg_bufc[MSG_BSIZE];
};

#ifdef	KERNEL
#ifdef	mips
/*
 * The message buffer lives at the end of physical address
 * space, so that we can recover it after a crash.
 */
extern struct	msgbuf *pmsgbuf;
#else
extern struct	msgbuf msgbuf;
#endif	mips
#endif	KERNEL
#endif	_SYS_MSGBUF_H_
