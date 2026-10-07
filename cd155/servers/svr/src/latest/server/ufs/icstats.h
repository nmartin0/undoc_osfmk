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
 * Inode cache statistics.
 */
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: icstats.h,v $
 * Revision 1.5  1994/11/18  20:45:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:37:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:52:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:30:24  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:58  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:50:21  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:38:50  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:47:21  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:18:12  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/01  16:59:39  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:58:06  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:51:24  gm]
 * 
 * Revision 1.3  90/07/27  09:08:44  devrcs
 * 	Use INOCACHE_STATS instead of ICACHE_STATISTICS.
 * 	[90/07/20  17:07:09  nags]
 * 
 * Revision 1.2  90/06/22  20:55:04  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Created.					nags@encore.com
 * 	[90/06/12  21:40:34  gmf]
 * 
 * $EndLog$
 */

#ifndef	_UFS_ICSTATS_H_

#if	INOCACHE_STATS
struct icache_stats {
	u_int	ic_iget_call;		/* times iget called */
	u_int	ic_iget_loop;		/* iget started over from scratch */
	u_int	ic_iget_research;	/* iget searched hash for duplicate */
	u_int	ic_iget_vget;		/* iget stumbled on funny vnode */
	u_int	ic_iget_hit;		/* cache hits */
	u_int	ic_iget_error;		/* error reading inode from disk */
	u_int	ic_iget_insert;		/* insertions into inode cache */
	u_int	ic_iget_reallocd;	/* freed, then reactivated */
};
extern struct icache_stats icache_stats;
#endif

#endif	/* _UFS_ICSTATS_H_ */
