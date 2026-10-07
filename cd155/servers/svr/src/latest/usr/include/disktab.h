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
 * $Log: disktab.h,v $
 * Revision 1.3  1994/11/18  21:09:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:24  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:05  nandy
 * Initial revision
 *
 * Revision 2.4  92/04/13  14:35:07  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.4  90/10/07  20:27:00  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:44:45  gm]
 * 
 * Revision 2.3  90/01/02  19:15:12  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  10:09:56  gm
 * 	Added function prototype declaration.
 * 	[89/06/30  11:56:14  bww]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)disktab.h	5.2 (Berkeley) 10/1/85
 */

/*
 * Disk description table, see disktab(5)
 */
#define	DISKTAB		"/etc/disktab"

struct	disktab {
	char	*d_name;		/* drive name */
	char	*d_type;		/* drive type */
	int	d_secsize;		/* sector size in bytes */
	int	d_ntracks;		/* # tracks/cylinder */
	int	d_nsectors;		/* # sectors/track */
	int	d_ncylinders;		/* # cylinders */
	int	d_rpm;			/* revolutions/minute */
	int	d_badsectforw;		/* supports DEC bad144 std */
	int	d_sectoffset;		/* use sect rather than cyl offsets */
	struct	partition {
		int	p_size;		/* #sectors in partition */
		short	p_bsize;	/* block size in bytes */
		short	p_fsize;	/* frag size in bytes */
	} d_partitions[8];
};

#if __STDC__
extern struct disktab *getdiskbyname(char*);
#else
struct	disktab *getdiskbyname();
#endif
