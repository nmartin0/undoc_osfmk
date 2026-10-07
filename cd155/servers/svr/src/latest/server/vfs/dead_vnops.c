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
 * $Log: dead_vnops.c,v $
 * Revision 1.5  1994/11/18  20:50:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:45:44  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:08:17  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:31:32  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:21  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:57:21  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:45:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:56:51  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/10/22  16:06:12  dbm
 * Added PFS functionality.
 *
 * Revision 2.4  1992/09/20  11:26:16  roy
 * 	Another op for OSF1_ADFS.
 * 	[92/09/15            roy]
 *
 * Revision 2.3  92/08/26  12:14:24  loverso
 * 	Additional ops for OSF1_ADFS.
 * 	[92/08/19            roy]
 * 
 * Revision 2.2  91/08/31  14:29:54  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/01  17:02:11  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9.4.2  91/03/05  10:21:51  gmf
 * 	1.  Collapse several ops into one, which returns EIO.
 * 	2.  Remove calls to chkvnlock().  It doesn't matter
 * 	    if we're in the middle of vclean; we return
 * 	    errors unconditionally.
 * 	3.  Make dead_badop() a printf, not a panic.
 * 	[91/03/01  10:43:03  gmf]
 * 
 * Revision 1.9  90/10/07  15:00:02  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:54:21  gm]
 * 
 * Revision 1.8  90/07/27  09:09:27  devrcs
 * 	VOP_OPEN changes for clone driver support.
 * 	[90/07/20  17:08:32  nags]
 * 
 * 	Include mount.h for all cases (funnelling).
 * 	[90/07/17  07:40:57  nags]
 * 
 * 	Change print vop from badop to nullop.
 * 	[90/06/28  14:46:10  nags]
 * 
 * Revision 1.6  90/06/22  20:56:10  devrcs
 * 	nags merge
 * 
 * 	Condensed relevant ancient history, reverse chronology:
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	New 4.4BSD file system file as of 1/5/90	noemi@osf.org
 * 	[90/06/12  21:42:41  nags]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
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
 *	@(#)dead_vnops.c	7.7 (Berkeley) 1/2/90
 */

#include <sys/param.h>
#include <sys/time.h>
#include <sys/mount.h>			/* funnelling through mnt struct */
#include <sys/vnode.h>
#include <sys/errno.h>
#include <sys/namei.h>
#include <sys/buf.h>
#include <sys/poll.h>

int	dead_lookup(),
	dead_open(),
	dead_rdwr(),
	dead_strategy(),
	dead_ioctl(),
	dead_select(),
	dead_ebadf(),
	dead_badop(),
	dead_nullop();

struct vnodeops dead_vnodeops = {
	dead_lookup,	/* lookup */
	dead_badop,	/* create */
	dead_badop,	/* mknod */
	dead_open,	/* open */
	dead_nullop,	/* close */
	dead_ebadf,	/* access */
	dead_ebadf,	/* getattr */
	dead_ebadf,	/* setattr */
	dead_rdwr,	/* read */
	dead_rdwr,	/* write */
	dead_ioctl,	/* ioctl */
	dead_select,	/* select */
	dead_badop,	/* mmap */
	dead_nullop,	/* fsync */
	dead_nullop,	/* seek */
	dead_badop,	/* remove */
	dead_badop,	/* link */
	dead_badop,	/* rename */
	dead_badop,	/* mkdir */
	dead_badop,	/* rmdir */
	dead_badop,	/* symlink */
	dead_ebadf,	/* readdir */
	dead_ebadf,	/* readlink */
	dead_badop,	/* abortop */
	dead_nullop,	/* inactive */
	dead_nullop,	/* reclaim */
	dead_rdwr,	/* bmap */
	dead_strategy,	/* strategy */
	dead_nullop,	/* print */
#if	MACH
	dead_badop,	/* page_read */
	dead_badop,	/* page_write */
#endif	
#ifdef	PFS
	dead_ebadf,	/* preallocate and set size */
#endif
#ifdef	OSF1_ADFS
	dead_badop,		/* pagein */
	dead_badop,		/* pageout */
	dead_badop,		/* alloc */
	dead_badop,		/* update */
	dead_badop,		/* getsize */
#endif
};

/*
 * TODO:
 * 	Add statistics for these functions.
 */

/*
 * Trivial lookup routine that always fails.
 */
dead_lookup(vp, ndp)
	struct vnode *vp;
	struct nameidata *ndp;
{

	ndp->ni_dvp = vp;
	ndp->ni_vp = NULL;
	return (ENOTDIR);
}

/*
 * Open always fails as if device did not exist.
 */
/* ARGSUSED */
dead_open(vpp, mode, cred)
	struct vnode **vpp;
	int mode;
	struct ucred *cred;
{

	return (ENXIO);
}

/*
 * Vnode op for most dead vnop accesses via file descriptor
 */
/* ARGSUSED */
dead_rdwr(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{

	/*
	 * Always return EIO
	 */
	return (EIO);
}

/*
 * Always return EIO
 */
/* ARGSUSED */
dead_ioctl(vp, com, data, fflag, cred)
	struct vnode *vp;
	register int com;
	caddr_t data;
	int fflag;
	struct ucred *cred;
{

	/*
	 * Always return EIO
	 */
	return (EIO);
}


/* ARGSUSED */
dead_select(vp, events, revents, scanning, cred)
	struct vnode *vp;
	short *events, *revents;
        int scanning;
	struct ucred *cred;
{

	if (scanning)
		*revents |= POLLNVAL;
	return (0);
}

/*
 * Always return EIO
 */
dead_strategy(bp)
	register struct buf *bp;
{

	return (EIO);
}

/*
 * Empty vnode failed operation
 */
dead_ebadf()
{

	return (EBADF);
}

/*
 * Empty vnode bad operation
 * Given parallelization and vnode semantics in OSF/1, under
 * some circumstances (e.g. forcible unmount), it might be possible to
 * get to this operation.  Therefore, a panic is undesirable, but it
 * is rare enough to warrant a printf.  It would be useful to know.
 */
dead_badop()
{

	printf("dead_badop called\n");
}

/*
 * Empty vnode null operation
 */
dead_nullop()
{

	return (0);
}
