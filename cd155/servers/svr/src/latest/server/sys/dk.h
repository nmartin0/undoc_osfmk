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
 * $Log: dk.h,v $
 * Revision 1.4  1994/11/18  20:40:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:24:09  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:39  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:20  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:03  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:03:29  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:42:17  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:49:46  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:08:11  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:38:10  gm]
 * 
 * 	Posix tty support.
 * 	[90/02/06  11:59:32  morris]
 * 
 * Revision 1.5  90/07/27  09:07:00  devrcs
 * 	Change dk_mspw to dk_wpms and made it a long for all architectures.
 * 	[90/07/17  22:16:47  brezak]
 * 
 * Revision 1.4  90/05/13  20:33:32  devrcs
 * 	Changes for gcc
 * 	[90/05/01  22:12:37  brezak]
 * 
 * Revision 1.3  90/02/16  16:52:10  devrcs
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:04:06  ers]
 * 
 * Revision 1.2  90/01/02  20:15:42  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:01  gm
 * 	     Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.4  89/05/30  10:42:51  rvb
 * 	No floats for mips kernels.
 * 	[89/04/20            af]
 * 
 * Revision 2.3  89/03/09  22:03:17  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:52:44  gm0w
 * 	Changes for cleanup.
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)dk.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_DK_H_
#define _SYS_DK_H_

/*
 * Instrumentation
 */

#define CPUSTATES	4

#define CP_USER		0
#define CP_NICE		1
#define CP_SYS		2
#define CP_IDLE		3

#define DK_NDRIVE	8

#ifdef	KERNEL
extern long	cp_time[CPUSTATES];

extern int	dk_ndrive;
extern int	dk_busy;
extern long	dk_time[DK_NDRIVE];
extern long	dk_seek[DK_NDRIVE];
extern long	dk_xfer[DK_NDRIVE];
extern long	dk_wds[DK_NDRIVE];
extern long	dk_wpms[DK_NDRIVE];

extern long	tk_nin;
extern long	tk_nout;
       long     tk_cancc;
       long     tk_rawcc;
#endif	KERNEL
#endif	_SYS_DK_H_
