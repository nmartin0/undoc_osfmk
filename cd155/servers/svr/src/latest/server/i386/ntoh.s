/*
 *
 *$Copyright
 *Copyright 1993, 1994 , 1995 Intel Corporation
 *INTEL CONFIDENTIAL
 *The technical data and computer software contained herein are subject
 *to the copyright notices; trademarks; and use and disclosure
 *restrictions identified in the file located in /etc/copyright on
 *this system.
 *Copyright$
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
 * $Log: ntoh.s,v $
 *Revision 1.4  1994/11/18  20:31:06  mtm
 *Copyright additions/changes
 *
 *Revision 1.3  1993/07/14  17:56:25  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:49  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:26  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:35:25  rabii
 * 	Initial V2.0 Checkin
 *
Revision 3.1  91/08/05  13:57:58  sp
Upgrade to 1.0.2

 * Revision 1.4  90/10/07  13:42:35  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:36:58  gm]
 * 
 * Revision 1.3  90/07/17  11:31:25  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:24:46  kevins]
 * 
 * 	Fixed for Profiling.  (You can only have one ENTRY() in a list
 * 	of Entry() points.  The .align's get you.)
 * 	[90/02/26            rvb]
 * 
 * 	New a.out and coff compatible .s files.
 * 	[89/10/16            rvb]
 * 
 * Revision 1.3  89/02/26  12:35:37  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 0.0  89/02/16            rvb
 * 	Created.
 * 	[89/02/16            rvb]
 * 
 * $EndLog$
 */

#include <i386/asm.h>

Entry(ntohl)
ENTRY(htonl)
	movl	4(%esp), %eax
	rorw	$8, %ax
	ror	$16,%eax
	rorw	$8, %ax
	ret


Entry(ntohs)
ENTRY(htons)
	movzwl	4(%esp), %eax
	rorw	$8, %ax
	ret
