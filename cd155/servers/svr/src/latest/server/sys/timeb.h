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
 * $Log: timeb.h,v $
 * Revision 1.4  1994/11/18  20:41:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:29:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:45  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:24  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:23  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:14:37  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:46:11  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:55:17  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:46:58  gm]
 * 
 * Revision 1.2  90/01/02  20:16:06  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:39  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.3  89/03/09  22:08:39  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:57:07  gm0w
 * 	Changes for cleanup.
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)timeb.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_TIMEB_H_
#define _SYS_TIMEB_H_

#include <sys/types.h>

/*
 * Structure returned by ftime system call
 */
struct timeb
{
	time_t	time;
	unsigned short millitm;
	short	timezone;
	short	dstflag;
};

#endif	_SYS_TIMEB_H_
