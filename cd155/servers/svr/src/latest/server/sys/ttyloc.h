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
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: ttyloc.h,v $
 * Revision 1.5  1994/11/18  20:41:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:30:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:09  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:20:43  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:45:06  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:45  cfj
 * Bump major revision number.
 * Revision 1.1.1.1  1993/05/03  17:43:26  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  14:15:37  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:46:44  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:55:53  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:47:53  gm]
 * 
 * Revision 1.2  90/01/02  20:16:11  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:45  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.3  89/03/09  22:09:20  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:57:39  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 0.0  86/08/15            mja
 * 	Changed TLC_MYHOST to expand to the host ID.
 * 	[86/08/15            mja]
 * 
 * Revision 0.0  85/05/15            mja
 * 	Made conditional on _TTYLOC_ symbol to permit recursive
 * 	includes.
 * 	[V1(1)]
 * 	[85/05/15            mja]
 * 
 * Revision 0.0  83/09/09            mja
 * 	Added TLC_TACTTY and TLC_RANDOMTTY definitions.
 * 	[83/09/09            mja]
 * 
 * Revision 0.0  83/03/28            mja
 * 	Created (V3.06h).
 * 	[83/03/28            mja]
 * 
 * $EndLog$
 */

#ifndef	_SYS_TTYLOC_H_
#define _SYS_TTYLOC_H_

struct ttyloc
{
    long tlc_hostid;		/* host identifier of location (on Internet) */
    long tlc_ttyid;		/* terminal identifier of location (on host) */
};

/*
 *  Pseudo host location of Front Ends
 */
#define TLC_FEHOST	((128<<24)|(2<<16)|(254<<8)|255)
#define TLC_SPECHOST	((0<<24)|(0<<16)|(0<<8)|0)

/*
 *  Pseudo terminal index of console
 */
#define TLC_CONSOLE	((unsigned short)0177777)
#define TLC_DISPLAY	((unsigned short)0177776)

/*
 *  Constants
 */
#define TLC_UNKHOST	((long)(0))
#define TLC_UNKTTY	((long)(-1))
#define TLC_DETACH	((long)(-2))
#define TLC_TACTTY	((long)(-3))		/* unused by kernel */
#define TLC_RANDOMTTY	((long)(-4))		/* unused by kernel */

#ifdef	KERNEL
#include <sys/kernel.h>		/* for hostid */

/*
 *  IP address of local host (as determined by the network module)
 */
#define TLC_MYHOST	(hostid)

#endif	KERNEL
#endif	_SYS_TTYLOC_H_
