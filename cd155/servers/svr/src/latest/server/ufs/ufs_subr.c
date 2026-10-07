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
 * $Log: ufs_subr.c,v $
 * Revision 1.5  1994/11/18  20:46:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:38:31  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:54:03  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:31:00  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:47  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:51:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:39:53  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:48:39  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:20:41  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/01  17:01:16  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:59:31  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:53:34  gm]
 * 
 * Revision 1.6  90/09/23  16:01:00  devrcs
 * 	Must include ufsmount.h for VFSTOUFS macro.
 * 	[90/09/08  19:31:15  nags]
 * 	Moved the fake_inode_init routine here from ufs_vfsops.c.
 * 	[90/09/08  19:04:23  nags]
 * 
 * Revision 1.5  90/06/22  20:55:53  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Updated for OSF/1 parallelization.		nags@encore.com
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	BSD4.4 Changes.					gm@osf.org
 * 	[90/06/12  21:42:07  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.
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
 *	@(#)ufs_subr.c	7.11 (Berkeley) 12/30/89
 */

#ifdef	_KERNEL
#if	MACH
#include <cputypes.h>
#endif
#endif

#include <sys/param.h>
#include <sys/time.h>
#include <sys/lock_types.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <ufs/fs.h>
#include <ufs/inode.h>
#include <ufs/ufsmount.h>

extern	int around[9];
extern	int inside[9];
extern	u_char *fragtbl[];

/*
 * Update the frsum fields to reflect addition or deletion
 * of some frags.
 */
fragacct(fs, fragmap, fraglist, cnt)
	struct fs *fs;
	int fragmap;
	long fraglist[];
	int cnt;
{
	int inblk;
	register int field, subfield;
	register int siz, pos;

	inblk = (int)(fragtbl[fs->fs_frag][fragmap]) << 1;
	fragmap <<= 1;
	for (siz = 1; siz < fs->fs_frag; siz++) {
		if ((inblk & (1 << (siz + (fs->fs_frag % NBBY)))) == 0)
			continue;
		field = around[siz];
		subfield = inside[siz];
		for (pos = siz; pos <= fs->fs_frag; pos++) {
			if ((fragmap & field) == subfield) {
				fraglist[siz] += cnt;
				pos += siz;
				field <<= siz;
				subfield <<= siz;
			}
			field <<= 1;
			subfield <<= 1;
		}
	}
}

/*
 * block operations
 *
 * check if a block is available
 */
isblock(fs, cp, h)
	struct fs *fs;
	unsigned char *cp;
	daddr_t h;
{
	unsigned char mask;

	switch ((int)fs->fs_frag) {
	case 8:
		return (cp[h] == 0xff);
	case 4:
		mask = 0x0f << ((h & 0x1) << 2);
		return ((cp[h >> 1] & mask) == mask);
	case 2:
		mask = 0x03 << ((h & 0x3) << 1);
		return ((cp[h >> 2] & mask) == mask);
	case 1:
		mask = 0x01 << (h & 0x7);
		return ((cp[h >> 3] & mask) == mask);
	default:
		panic("isblock");
		return (NULL);
	}
}

/*
 * take a block out of the map
 */
clrblock(fs, cp, h)
	struct fs *fs;
	u_char *cp;
	daddr_t h;
{

	switch ((int)fs->fs_frag) {
	case 8:
		cp[h] = 0;
		return;
	case 4:
		cp[h >> 1] &= ~(0x0f << ((h & 0x1) << 2));
		return;
	case 2:
		cp[h >> 2] &= ~(0x03 << ((h & 0x3) << 1));
		return;
	case 1:
		cp[h >> 3] &= ~(0x01 << (h & 0x7));
		return;
	default:
		panic("clrblock");
	}
}

/*
 * put a block into the map
 */
setblock(fs, cp, h)
	struct fs *fs;
	unsigned char *cp;
	daddr_t h;
{

	switch ((int)fs->fs_frag) {

	case 8:
		cp[h] = 0xff;
		return;
	case 4:
		cp[h >> 1] |= (0x0f << ((h & 0x1) << 2));
		return;
	case 2:
		cp[h >> 2] |= (0x03 << ((h & 0x3) << 1));
		return;
	case 1:
		cp[h >> 3] |= (0x01 << (h & 0x7));
		return;
	default:
		panic("setblock");
	}
}

#if (!defined(vax) && !defined(tahoe)) || defined(VAX630) || defined(VAX650)
/*
 * C definitions of special instructions.
 * Normally expanded with inline.
 */
#if	defined(sun) || defined(mips)
#else
scanc(size, cp, table, mask)
	u_int size;
	register u_char *cp, table[];
	register u_char mask;
{
	register u_char *end = &cp[size];

	while (cp < end && (table[*cp] & mask) == 0)
		cp++;
	return (end - cp);
}
#endif

#endif /* (!defined(vax)&&!defined(tahoe))||defined(VAX630)||defined(VAX650) */

#if !defined(vax) && !defined(tahoe)
skpc(mask, size, cp)
	register u_char mask;
	u_int size;
	register u_char *cp;
{
	register u_char *end = &cp[size];

	while (cp < end && *cp == mask)
		cp++;
	return (end - cp);
}

locc(mask, size, cp)
	register u_char mask;
	u_int size;
	register u_char *cp;
{
	register u_char *end = &cp[size];

	while (cp < end && *cp != mask)
		cp++;
	return (end - cp);
}
#endif


/*
 * Initialize just the few fields necessary for
 * limited use of a fake inode.
 */
fake_inode_init(tvp, mp)
struct vnode *tvp;
struct mount *mp;
{
	struct inode	*ip;
	struct ufsmount	*ump;

	tvp->v_mount = mp;
	VN_LOCK_INIT(tvp);
	ip = VTOI(tvp);
	ip->i_vnode = tvp;
	ump = VFSTOUFS(mp);
	ip->i_dev = ump->um_dev;
	ip->i_fs = ump->um_fs;
	ip->i_devvp = ump->um_devvp;
	IN_LOCK_INIT(ip);
	IN_IO_LOCK_INIT(ip);
}
