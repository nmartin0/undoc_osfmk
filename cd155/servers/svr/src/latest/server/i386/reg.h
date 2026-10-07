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
 * $Log: reg.h,v $
 * Revision 1.4  1994/11/18  20:31:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:56:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:16:03  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:58  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:08  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:37  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:35:43  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/05  13:58:13  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  13:46:13  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:42:45  gm]
 * 
 * Revision 1.3  90/07/17  11:32:14  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:31:09  kevins]
 * 
 * Revision 1.4  89/09/25  12:25:55  rvb
 * 	This is UNPUBLISHED PROPRIETARY SOURCE CODE of Prime Computer, Inc.
 * 	[89/09/23            rvb]
 * 
 * $EndLog$
 */

/*
**
**  Copyright (c) 1988 Prime Computer, Inc.  Natick, MA 01760
**  All Rights Reserved
**
**  This is UNPUBLISHED PROPRIETARY SOURCE CODE of Prime Computer, Inc.
**  Inclusion of the above copyright notice does not evidence any actual
**  or intended publication of such source code.
*/


#define SS	0x0012
#define UESP	0x0011
#define EFL	0x0010
#define CS	0x000F
#define EIP	0x000E
#define ERR	0x000D
#define TRAPNO	0x000C
#define EAX	0x000B
#define ECX	0x000A
#define EDX	0x0009
#define EBX	0x0008
#define ESP	0x0007
#define EBP	0x0006
#define ESI	0x0005
#define EDI	0x0004
#define DS	0x0003
#define ES	0x0002
#define FS	0x0001
#define GS	0x0000
