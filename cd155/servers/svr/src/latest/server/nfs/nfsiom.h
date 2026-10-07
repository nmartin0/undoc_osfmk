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
 * HISTORY
 * $Log: nfsiom.h,v $
 * Revision 1.5  1994/11/18  20:37:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:16:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:38:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:44  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:48  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:40  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:00  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:53:38  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:40:51  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:39:49  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:22:30  gm]
 * 
 * Revision 1.3  90/01/02  20:22:37  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:20:56  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)nfsiom.h	7.1 (Berkeley) 8/30/89
 */

/*
 * Size of the resource map and system page table pte's that are mapped
 * by nfs for i/o much like a bus adapter
 */
#define	NFS_MAPREG	512	/* Num. of kernel pte's for i/o mapping */
				/* Must be >= MAXPHYS/NBPG */
#define	NFS_MSIZ	100	/* Size of alloc. map for pte's */

/*
 * Size of the bio buffers, must be a power of 2 <= MAX_BSIZE
 */
#define	NFS_BIOSIZE	8192
#define	NFS_BIOSHIFT	13	/* Bits for NFS_BIOSIZE */
