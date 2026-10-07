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
 * $Log: ttydev.h,v $
 * Revision 1.5  1994/11/18  20:41:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:30:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:20:37  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:45:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:39  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:42  cfj
 * Bump major revision number.
 * Revision 1.1.1.1  1993/05/03  17:43:25  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  14:15:28  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:46:40  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:55:49  devrcs
 * 	changed USE_OLD_TTY to _USE_OLD_TTY, USE_NEW_TTY to _USE_NEW_TTY
 * 	[90/10/04  09:39:31  rabin]
 * 
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:09:17  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:47:46  gm]
 * 
 * Revision 1.4  90/03/13  20:50:18  mbrown
 * 	Added CBAUD mask for old tty support.
 * 	[90/03/08  13:27:24  tom]
 * 
 * 	AIX merge first cut.
 * 	[90/02/09  18:06:12  tom]
 * 
 * 	Posix tty support.
 * 	[90/02/06  13:34:57  morris]
 * 
 * Revision 1.3  90/02/16  16:52:58  devrcs
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:18:20  ers]
 * 
 * Revision 1.2  90/01/02  20:16:11  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:44  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.3  89/03/09  22:09:15  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:57:34  gm0w
 * 	Changes for cleanup.
 * 
 * $EndLog$
 */

/*
 *
 * <bsd/sys/ttydev.h - a la 4.xBSD for BSD to AIX porting tools
 *	derived from BSD <sys/ttydev.h>
 * COPYRIGHT 1987 IBM CORP.
 */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ttydev.h	7.6 (Berkeley) 11/20/89
 */

/*
 * COMPATABILITY HEADER FILE --
 */
/*
 * Terminal definitions related to underlying hardware.
 */

#ifndef _SYS_TTYDEV_H_
#define	_SYS_TTYDEV_H_

#ifdef _USE_OLD_TTY

/*
 * Mask to clear speed bits
 */

#define CBAUD	0x0000000f

/*
 * Speeds
 */

#define	B0	0x00000000
#define	B50	0x00000001
#define	B75	0x00000002
#define	B110	0x00000003
#define	B134	0x00000004
#define	B150	0x00000005
#define	B200	0x00000006
#define	B300	0x00000007
#define	B600	0x00000008
#define	B1200	0x00000009
#define	B1800	0x0000000a
#define	B2400	0x0000000b
#define	B4800	0x0000000c
#define	B9600	0x0000000d
#define	B19200	0x0000000e
#define	B38400	0x0000000f
#define EXTA    B19200
#define EXTB    B38400
#endif /* _USE_OLD_TTY */

#endif	/* _SYS_TTYDEV_H_ */
