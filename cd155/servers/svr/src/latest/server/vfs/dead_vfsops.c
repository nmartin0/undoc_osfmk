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
 * $Log: dead_vfsops.c,v $
 * Revision 1.5  1994/11/18  20:50:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:45:38  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:08:13  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:31:30  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:20  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:57:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:45:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:56:47  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:29:47  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/01  17:02:02  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:59:58  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:54:14  gm]
 * 
 * Revision 1.3  90/08/24  12:29:48  devrcs
 * 	Added dead_quotactl to dead_vfsops.
 * 	[90/08/18  03:53:22  nags]
 * 
 * Revision 1.2  90/07/27  09:09:23  devrcs
 * 	Dead mount structures.
 * 	[90/07/20  17:08:24  nags]
 * 
 * $EndLog$
 */

#include <sys/param.h>
#include <sys/user.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/errno.h>

/*
 * dead vfs operations.
 */
int dead_badvfsop();
int dead_root();
int dead_quotactl();
int dead_statfs();
int dead_sync();
int dead_fhtovp();
int dead_vptofh();

struct vfsops dead_vfsops = {
	dead_badvfsop,	/* vfs_mount */
	dead_badvfsop,	/* vfs_start */
	dead_badvfsop,	/* vfs_unmount */
	dead_root,
	dead_quotactl,
	dead_statfs,
	dead_sync,
	dead_fhtovp,
	dead_vptofh,
	dead_badvfsop,	/* vfs_init */
};

/*
 * The dead mount structure
 */
struct mount dead_mount;

/*
 * VFS Operations.
 */
/* ARGSUSED */
dead_badvfsop()
{
#if	MACH_ASSERT
	printf("Dead_badvfsop called\n");
#endif
	panic("dead_badvfsop called");
}

/*
 * Return root of a filesystem
 */
dead_root(mp, vpp)
	struct mount *mp;
	struct vnode **vpp;
{
	
#if	MACH_ASSERT
	printf("Dead_root called\n");
#endif
	return (ENODEV);
}

/*
 * Quota controls
 */
dead_quotactl(mp, cmds, uid, arg)
	struct mount *mp;
	int cmds;
	uid_t uid;
	caddr_t arg;
{
#if	MACH_ASSERT
	printf("Dead_quotactl called\n");
#endif
	return (EOPNOTSUPP);
}

/*
 * File system status
 */
dead_statfs(mp)
	struct mount *mp;
{
	
#if	MACH_ASSERT
	printf("Dead_statfs called\n");
#endif
	return (ENODEV);
}

/*
 * Flush out the buffer cache
 */
/* ARGSUSED */
dead_sync(mp, waitfor)
	struct mount *mp;
	int waitfor;
{

#if	MACH_ASSERT
	printf("Dead_sync called\n");
#endif
	return (ENODEV);
}

/*
 * File handle to vnode pointer translation.
 */
/* ARGSUSED */
dead_fhtovp(mp, fhp, vpp)
	struct mount *mp;
	struct fid *fhp;
	struct vnode **vpp;
{

#if	MACH_ASSERT
	printf("Dead_fhtovp called\n");
#endif
	return (ENODEV);
}

/*
 * Vnode pointer to File handle.
 */
/* ARGSUSED */
dead_vptofh(mp, fhp, vpp)
	struct mount *mp;
	struct fid *fhp;
	struct vnode **vpp;
{

#if	MACH_ASSERT
	printf("Dead_vptofh called\n");
#endif
	return (ENODEV);
}
