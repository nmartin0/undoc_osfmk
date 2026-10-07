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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: device.h,v $
 * Revision 1.4  1994/11/18  20:47:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:40:30  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:59:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:53:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:51:13  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:25:16  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/01/17  12:05:53  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  89/09/15  15:29:19  rwd
 * 	Created
 * 	[89/09/11            rwd]
 * 
 */
#ifdef	KERNEL
#define	KERNEL__
#undef	KERNEL
#endif	KERNEL

#include <device/device.h>

#ifdef	KERNEL__
#undef	KERNEL__
#define	KERNEL	1
#endif	KERNEL__

