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
 * $Log: vfont.h,v $
 * Revision 1.3  1994/11/18  21:11:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:53  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:48  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:22:03  nandy
 * Initial revision
 *
 * Revision 2.3  92/04/13  14:40:25  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.3  90/10/07  20:39:10  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:02:57  gm]
 * 
 * Revision 2.2  90/01/02  19:15:56  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)vfont.h	5.1 (Berkeley) 5/30/85
 */

/*
 * The structures header and dispatch define the format of a font file.
 *
 * See vfont(5) for more details.
 */
struct header {
	short magic;
	unsigned short size;
	short maxx;
	short maxy;
	short xtend;
}; 

struct dispatch {
	unsigned short addr;
	short nbytes;
	char up,down,left,right;
	short width;
};
