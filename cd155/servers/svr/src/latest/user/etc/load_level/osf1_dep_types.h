/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
/*
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: osf1_dep_types.h,v $
 * Revision 1.8  1995/02/01  23:53:38  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/11/19  03:08:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/11/18  20:53:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/11/19  19:04:42  bolsen
 *  Reviewer: Mike Barnett and Stefan Tritscher
 *  Risk: low
 *  Benefit or PTS #: 5391 - dbl should be a "typedef double"
 *  Testing: LCC load_leveld tests
 *  Module(s): user/etc/load_level/osf1_dep_types.h
 *
 * Revision 1.4  1993/07/14  18:51:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:29:40  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:17:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:01:57  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:49:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:02:18  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:58:08  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.1  93/08/03  10:58:32  bolsen
 * Fixed the following bug:
 * [Bug 305/5391]  dbl should be a "typedef double" for proper alignment
 * 
 * Revision 3.0  92/05/13  08:23:11  mbarnett
 * initial checkin
 * 
 */

typedef int	node_array_t[100];
typedef double	dbl;
typedef	dbl	load_array_t[100];

