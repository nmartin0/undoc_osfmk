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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: swapgeneric.c,v $
 * Revision 1.4  1994/11/18  20:31:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:57:08  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:16:17  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:21:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:16  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:50  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:35:58  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/05  13:58:18  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8.4.3  90/12/20  14:49:57  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.8.4.2  90/11/19  13:48:13  jeffc
 * 	Changed gets() to echo.  Eliminate gets() from fd_makedev.
 * 	Made boot -a[skname] work.
 * 
 * Revision 1.8  90/10/31  13:57:09  devrcs
 * 	Changes for AT386 dumps.
 * 	[90/10/25  19:31:56  jvs]
 * 
 * Revision 1.7  90/10/07  13:47:42  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:45:07  gm]
 * 
 * Revision 1.6  90/09/23  15:51:18  devrcs
 * 	Added RDB new IBM Debugger
 * 	[90/09/07  17:24:11  kevins]
 * 
 * 	Moved boottype and boothowto to this module.
 * 	[90/09/06  14:34:18  jd]
 * 
 * Revision 1.5  90/08/09  13:19:33  devrcs
 * 	Merge of 386 code
 * 	[90/07/27  08:41:33  kevins]
 * 
 * 	get rid if asm(hlt), and put the Root on hd0a for PS2
 * 	[90/07/26  14:08:04  jd]
 * 
 * Revision 1.4  90/07/17  11:32:27  devrcs
 * 	Changes for i386at to i386/AT386
 * 	[90/06/29  09:45:52  kevins]
 * 
 * 	Updates for SS4
 * 	[90/06/29  09:31:39  kevins]
 * 
 * 	Use floppy type passed in when booting from floppy.
 * 	[90/02/09            rvb]
 * 
 * 	Move i386at definition to file prologue.
 * 	[90/01/02  13:50:13  rvb]
 * 
 * 	Kill printout "booting ... "
 * 	[89/12/26            rvb]
 * 
 * 	5 1/4' as default
 * 	[89/12/21  17:49:23  rvb]
 * 
 * 	New ... whizzy ... setconf code to find root.
 * 	[89/10/17            rvb]
 * 
 * Revision 1.3  89/02/26  12:34:03  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 0.0  88/12/31            rvb
 * 	Derived from MACH2.0 vax release.
 * 	[88/12/31            rvb]
 * 
 * Revision 0.0  87/10/29            rvb
 * 	Allow root to be an arbitrary slice on the drive.
 * 	[87/10/29            rvb]
 * 
 * $EndLog$
 */
 
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)swapgeneric.c	7.1 (Berkeley) 6/6/86
 */
/*
 * Empty file to satisfy configuration.
 */

