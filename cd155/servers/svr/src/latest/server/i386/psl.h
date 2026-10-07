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
 * $Log: psl.h,v $
 * Revision 1.4  1994/11/18  20:31:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:56:38  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:53  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:32  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:35:35  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/05  13:58:09  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  13:43:28  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:38:16  gm]
 * 
 * Revision 1.3  90/07/17  11:32:11  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:31:03  kevins]
 * 
 * Revision 1.4  89/03/09  20:03:39  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:33:24  gm0w
 * 	Changes for cleanup.
 * 
 * $EndLog$
 */
 
/*
 *	80386 EFLAGS register definition
 */

#define EFL_CF		0x00000001	/* carry flag */
#define EFL_PF		0x00000004	/* parity flag */
#define EFL_AF		0x00000010	/* auxiliary carry */
#define EFL_ZF		0x00000040	/* zero flag */
#define EFL_SF		0x00000080	/* sign flag */
#define EFL_ALLCC	0x000000c5	/* all cc bits - unlikely */
#define EFL_TF		0x00000100	/* trap flag */
#define EFL_IF		0x00000200	/* interrupt enable flag */
#define EFL_DF		0x00000400	/* direction flag */
#define EFL_OF		0x00000800	/* overflow flag */
#define EFL_IOPL	0x00003000	/* i/o privilege level */
#define EFL_NT		0x00004000	/* nested task flag */
#define EFL_RF		0x00010000	/* resume flag */
#define EFL_VM		0x00020000	/* virtual 8086 mode */

#define EFL_USERSET	0
#define EFL_USERCLR	(EFL_IF | EFL_NT | EFL_IOPL | EFL_RF | EFL_VM)
