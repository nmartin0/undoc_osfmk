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
 * $Log: import_mach.h,v $
 * Revision 1.4  1994/11/18  20:48:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:42:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:03:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:54:58  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:41  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:53:31  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:26:22  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/01/17  12:06:06  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/06/02  15:27:47  rpd
 * 	Added <mach/notify.h>.
 * 	[90/03/28            rpd]
 * 	Removed mach/host.h.
 * 	[90/03/26  20:16:50  rpd]
 * 
 */

/*
 * MACH interface definitions and data for UX out-of-kernel kernel.
 */

/*
 * <mach/mach.h> must be included with 'KERNEL' off
 */
#ifdef	KERNEL
#define	KERNEL__
#undef	KERNEL
#endif	KERNEL

#include <mach/mach.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <mach/mig_errors.h>
#include <cthreads.h>

#ifdef	KERNEL__
#undef	KERNEL__
#define	KERNEL	1
#endif	KERNEL__
