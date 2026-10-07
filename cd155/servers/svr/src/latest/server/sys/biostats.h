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
 * Buffer cache statistics.
 */
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: biostats.h,v $
 * Revision 1.4  1994/11/18  20:40:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:23:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:13  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:38  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:02:28  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:41:36  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:49:00  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:36:59  gm]
 * 
 * Revision 1.3  90/07/27  09:06:47  devrcs
 * 	Use BUFCACHE_STATS instead of BIO_STATISTICS.
 * 	[90/07/20  17:05:34  nags]
 * 
 * Revision 1.2  90/06/22  20:52:30  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Created.					nags@encore.com
 * 	[90/06/12  21:37:37  gmf]
 * 
 * $EndLog$
 */

#ifndef	_SYS_BIOSTATS_H_
#define _SYS_BIOSTATS_H_

#if	BUFCACHE_STATS
struct bio_stats {
	int	getblk_hits;
	int	getblk_misses;
	int	getblk_research;
	int	getblk_dupbuf;
	int	getnewbuf_calls;
	int	getnewbuf_buflocked;
	int	vflushbuf_lockskips;
	int	mntflushbuf_misses;
	int	mntinvalbuf_misses;
	int	vinvalbuf_misses;
	int	allocbuf_buflocked;
	int	ufssync_misses;
};

#ifdef	_KERNEL
extern struct bio_stats 	bio_stats;
#define	BUF_STATS(clause)	STATS_ACTION(&bio_stats_lock, (clause))
vdecl_simple_lock_data(,bio_stats_lock)
#endif

#else	/* BUFCACHE_STATS */

#ifdef	_KERNEL
#define	BUF_STATS(clause)
#endif

#endif	/* BUFCACHE_STATS */
#endif	/* _SYS_BIOSTATS_H_ */
