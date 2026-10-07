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
 * $Log: cpu.h,v $
 * Revision 1.5  1994/11/18  20:31:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:58:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:17:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:09:08  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:29:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:21:41  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:50  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/11/13  12:21:00  rabii
 * 	Initial checkin
 *
 * Revision 2.2  91/08/31  13:33:12  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:56:42  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  13:37:22  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:30:33  gm]
 * 
 * Revision 1.3  90/07/17  11:27:49  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:17:39  kevins]
 * 
 * Revision 1.3  89/02/26  12:27:09  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 0.0  89/01/02            rvb
 * 	This all seems necessary.
 * 	[89/01/02            rvb]
 * 
 * $EndLog$
 */

#define cpu_number() (0)
#define set_cpu_number()

int cpu;

int master_cpu;
