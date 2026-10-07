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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 */

/*
 * HISTORY
 * $Log: pfs_vfsops.c,v $
 * Revision 1.8  1995/03/08  21:42:10  jlitvin
 * We don't really want to call ufs_init() for PFS.  Create a dummy
 * pfs_init() instead.  I also removed pfs_statfs() since it is no longer
 * used.  This was wasting time and memory on heavy servers.
 *
 *  Reviewer: brad
 *  Risk: very low
 *  Benefit or PTS #: 12639
 *  Testing: VSX
 *  Module(s): server/pfs/pfs_vfsops.c
 *
 * Revision 1.7  1995/03/02  19:18:45  stans
 *  Vnode caching support: pfs_host_init() syscall
 *
 *  Reviewer:jlitvin,suri,cfj
 *  Risk:medium
 *  Benefit or PTS #:8129
 *  Testing:WW07 sats
 *
 * Revision 1.6  1994/11/18  20:38:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/12/23  01:47:27  brad
 * Fixed various compilers warnings, lint errors, and lint warnings.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: None.
 *  Testing: Booted and ran minimal PFS tests.
 *  Module(s): emulator/emul_stack_alloc.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_user_side.c
 *             emulator/pfs2_user_side.c
 *             server/pfs/pfs_vfsops.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/fsvr_server_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr_types.h
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.4  1993/05/10  21:19:34  brad
 * Updated pfs_mountfs to comform to ad1.0.3 mount changes.
 *
 * Revision 1.3  1993/04/03  03:07:02  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.4  1993/03/30  02:32:56  brad
 * Added support for forwarding PFS mount operations when the disk partition
 * to be mounted is remote.
 *
 * Revision 1.1.2.1.2.3  1993/02/23  04:36:07  brad
 * Unlock mount table and unmount on error in pfs_mount().
 *
 * Revision 1.1.2.1.2.2  1992/12/18  18:19:29  brad
 * Added p_magic to statpfs structure, for PFS file header validation.
 *
 * Revision 1.2  1992/11/30  22:33:44  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:11:03  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/10  15:21:58  cfj
 * Put into NX branch.
 *
 * Revision 1.1  1992/11/05  22:35:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1  1992/10/22  15:24:09  dbm
 * Added for PFS functionality.
 *
 *
 *
 */

#if	MACH
#include <quota.h>
#if       BUFCACHE_STATS
#include <bufcache_stats.h>
#endif
#ifdef	i386
#include <cputypes.h>
#endif
#endif

#include <mapped_files.h>
#include <sys/secdefines.h>
#include <norma_ipc.h>
#if NORMA_IPC
#include <mach/norma_special_ports.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/namei.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <sys/biostats.h>
#include <sys/ucred.h>
#include <sys/file.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#if	MACH
#include <kern/zalloc.h>
#include <kern/kalloc.h>
#else
#include <sys/malloc.h>
#endif
#include <ufs/quota.h>
#if	QUOTA
#include <sys/proc.h>
#endif
#include <ufs/fs.h>
#include <ufs/ufsmount.h>
#include <ufs/inode.h>
#include <sys/lock_types.h>
#if	MACH
#include <sys/user.h>
#include <kern/mfs.h>
#endif
#ifdef	OSF1_ADFS
#include <uxkern/syscall_subr.h>
#include <uxkern/sthread.h>
#endif
#if	EXL
#include <machine/open.h>
#endif
#include <uxkern/mf.h>

#if	MACH
zone_t	superblock_zone;
#endif

#ifdef	PFS
/* #define DEBUG_PFS */


/*
 * Extern declarations.
 */
int		pfs_mount();
extern int	ufs_mount();
extern int	ufs_start();
int		pfs_unmount();
extern int	ufs_unmount();
extern int	ufs_root();
extern int	ufs_quotactl();
extern int	ufs_statfs();
extern int	ufs_sync();
extern int	ufs_fhtovp();
extern int	ufs_vptofh();
int		pfs_init();

/*
 * VFS operations for PFS file systems.
 */
struct vfsops pfs_vfsops = {
	pfs_mount,
	ufs_start,
	pfs_unmount,
	ufs_root,
	ufs_quotactl,
	ufs_statfs,
	ufs_sync,
	ufs_fhtovp,
	ufs_vptofh,
	pfs_init
};


/*
 * VFS Operations.
 */

/*
 * PFS mount.
 *
 * Executed on the node owning the mounted on directory ... so the ufs_mount()
 * operation called here may forward the operation to another node if the disk 
 * partition to be mounted is remote.
 */
pfs_mount(mp, path, data, mount_ndp)
	register struct mount	*mp;
	char			*path;
	caddr_t 		data;
	struct nameidata 	*mount_ndp;
{
	int 				error;

#ifdef	DEBUG_PFS
	register struct pfs_args 	*args  = (struct pfs_args *)data;

	printf("pfs_mount: mp = 0x%x, path = %s, data = 0x%x, mount_ndp = 0x%x\n",
	       mp, path, data, mount_ndp);
	dump_pfsargs(args);
#endif	DEBUG_PFS

	/*
	 * Mount the underlying "physical" UFS file system that will contain
	 * PFS stripe file headers, etc.
	 */
	error = ufs_mount(mp, path, data, mount_ndp);
	if (error)
		goto out;

#ifdef	DEBUG_PFS
	printf("pfs_mount: mp->m_stat.f_type = %d\n", mp->m_stat.f_type);
	if (mp->m_statpfs == NULL) {
		printf("pfs_mount: mp->m_statpfs is NULL, must be remote\n");
	} else {
		printf("pfs_mount: mp->m_statpfs is not NULL, must be local\n");
		dump_pfsattr(mp->m_statpfs);
	}
#endif	DEBUG_PFS

out:
	return(error);
}


/*
 * PFS unmount.  
 *
 * Executed on the node owning the disk partition that is mounted.
 *
 * Synchronization assumptions:
 *	-- other unmounts are locked out.
 *	-- mp is still on global list, and is accessible.
 */
pfs_unmount(mp, mntflags)
	struct mount *mp;
	int mntflags;
{
	int	error;

#ifdef	DEBUG_PFS
	printf("pfs_unmount: mp = 0x%x, mp->m_stat.f_type = %d, mntflags = 0x%x\n",
	       mp, mp->m_stat.f_type, mntflags);
#endif	DEBUG_PFS

#ifdef	DEBUG_PFS
	printf("pfs_mount: mp->m_stat.f_type = %d\n", mp->m_stat.f_type);
	if (mp->m_statpfs == NULL) {
		printf("pfs_mount: mp->m_statpfs is NULL, must be remote\n");
	} else {
		printf("pfs_mount: mp->m_statpfs is not NULL, must be local\n");
		dump_pfsattr(mp->m_statpfs);
	}
#endif	DEBUG_PFS

	error = ufs_unmount(mp, mntflags);
	if (error)
		return(error);

	/*
	 * Deallocation of statpfs can safely be done here, since new pathname
	 * translations (e.g. resulting from a statpfs()) were already disabled
	 * in the VFS dounmount() function.
	 */
	MOUNT_LOCK(mp);
	ASSERT(mp->m_statpfs != NULL);
	kfree(mp->m_statpfs, mp->m_statpfs->p_reclen);
	mp->m_statpfs = NULL;
	MOUNT_UNLOCK(mp);

	return(error);
}


/*
 * Handle PFS mount requests from remote nodes.  This routine cloned from
 * ufs_mountfs().
 */
pfs_mountfs(devvp, flag, remoteport, dir, spec, stripe_args, mountportp)
	struct vnode 	*devvp;
	int 		flag;
	mach_port_t 	remoteport;
	char		*dir;
	char		*spec;
	struct statpfs	*stripe_args;
	mach_port_t 	*mountportp;
{
	struct mount		*mp;
	register struct fs	*fs;
	int			error;
	u_int 			size;
	struct statpfs		*stripe_attr;
	int			stripe_attr_len = stripe_args->p_reclen;
	struct uio		uio;
	struct iovec		iov;

#ifdef	DEBUG_PFS
	printf("pfs_mountfs: devvp=0x%x flag=0x%x remoteport=0x%x dir=%s spec=%s stripe_args=0x%x mountportp=0x%x\n", 
	       devvp, flag, remoteport, dir, spec, stripe_args, mountportp);
#endif
	ASSERT(devvp != NULLVP);
	ASSERT((flag & M_REMOTE_FS) == 0);

	/*
	 * Allocate and initialize a mount structure.
	 */
	MOUNT_ALLOCATE(mp);
	mp->m_op = vfssw[MOUNT_PFS];
	mp->m_flag = (flag | M_REMOTE_DIR);
	mp->m_exroot = 0;
	mp->m_stat.f_type = MOUNT_NONE;
	mp->m_mounth = (struct vnode *)0;
	mp->m_remoteport = remoteport;
	mp->m_vnodecovered = (struct vnode *)0;
#if     SER_COMPAT
	mp->m_funnel = !FUNNEL_NULL;    /* XXX */
#endif
	UNMOUNT_LOCK_INIT(mp);
	MOUNT_VLIST_LOCK_INIT(mp);
	MOUNT_LOCK_INIT(mp);

	/*
	 * Let mountfs do the bulk of the work.
	 */
	if ((error = mountfs(devvp, mp, 0)) != 0) {
		/*
		 * devvp is vrele'd by the marshalling code.
		 */
		MOUNT_DEALLOCATE(mp);
		return(error);
	}

	/*
	 * Allocate a port for the mount structure.
	 */
	mount_port_allocate(mp, mountportp);

	/*
	 * Set up the statfs structure in the mount structure.
	 */
	fs = (VFSTOUFS(mp))->um_fs;
	(void) copystr(dir, fs->fs_fsmnt, sizeof(fs->fs_fsmnt) - 1, &size);
	bzero(fs->fs_fsmnt + size, sizeof(fs->fs_fsmnt) - size);
	bcopy((caddr_t)fs->fs_fsmnt, (caddr_t)mp->m_stat.f_mntonname, MNAMELEN);
	(void) copystr(spec, mp->m_stat.f_mntfromname, MNAMELEN - 1, 
		&size);
	bzero(mp->m_stat.f_mntfromname + size, MNAMELEN - size);
	(void) ufs_statfs(mp);
	mp->m_stat.f_type = MOUNT_PFS;

	/*
	 * Reset the I/O mode (set by mountfs()) for this file system to 
	 * VIO_PFS.
	 */
	mp->m_iomode = VIO_PFS;

	/*
	 * Allocate and initialize the statpfs structure in the mount 
	 * structure.  Stripe attributes are copied in by forging a uio
	 * structure to hand to uiomove.  We can't just use bcopy() here,
	 * because we're copying out-of-line user data that could be backed
	 * by an untrusted pager.
	 */
	ASSERT(mp->m_statpfs == NULL);
	stripe_attr = (struct statpfs *)kalloc(stripe_attr_len);
	if (stripe_attr == NULL) {
		MOUNT_DEALLOCATE(mp);
		return(ENOMEM);
	}
	iov.iov_base = (caddr_t) stripe_args;
	iov.iov_len = stripe_attr_len;
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	uio.uio_offset = 0;
	uio.uio_resid = stripe_attr_len;
	uio.uio_segflg = UIO_SYSSPACE;
	uio.uio_rw = UIO_WRITE;
	error = uiomove((caddr_t)stripe_attr, stripe_attr_len, &uio);
	if (error) {
		MOUNT_DEALLOCATE(mp);
		kfree(stripe_attr, stripe_attr_len);
		return(error);
	}
	stripe_attr->p_magic = PFS_MAGIC;
	mp->m_statpfs = stripe_attr;
#ifdef	DEBUG_PFS
	dump_pfsattr(mp->m_statpfs);
#endif	DEBUG_PFS

	/*
	 * Add the new mount structure to the mount list.
	 */
	MOUNTLIST_LOCK();
	mp->m_next = mounth.m_next;
	mp->m_prev = (struct mount *)&mounth;
	mounth.m_next = mp;
	mp->m_next->m_prev = mp;
	MOUNTLIST_UNLOCK();

	/*
	 * Enable lookups on this filesystem.
	 */
	return(ESUCCESS);
}


/*
 * dummy PFS init.
 */
pfs_init()
{
	return (0);
}


/*
 * Pre-parse the PFS path.
 */
pfs_pre_parse(p, args, retval)
    struct proc *p;
    void *args;
    int *retval;
{
	struct args {
		char	*fname;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	int error;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = namei(ndp))
		return (error);
	return (0);
}


#ifdef	DEBUG_PFS
dump_pfsargs(pfs_argsp)
	struct pfs_args *pfs_argsp;
{
	struct ufs_args	*ufs_argsp = &pfs_argsp->fs_args;

	printf("pfs mount arguments: \n");
	printf("  special = %s\n", ufs_argsp->fspec);
	printf("  exflags = 0x%x\n", ufs_argsp->exflags);
	printf("  exroot = %d\n", ufs_argsp->exroot);

	dump_pfsattr(&pfs_argsp->stripe_attr);
}


dump_pfsattr(sattr)
	struct statpfs *sattr;
{
	pathname_t	*sdir = &sattr->p_sdirs;
	int 		i;

	printf("pfs stripe attributes: \n");
	printf("  record length = %d\n", sattr->p_reclen);
	printf("  stripe unit size = %d\n", sattr->p_sunitsize);
	printf("  stripe factor = %d\n", sattr->p_sfactor);

	for (i = 0; i < sattr->p_sfactor; i++) {
		printf("  stripedir %d, length = %d, dir = %s\n",
			i, sdir->namelen, sdir->name);
		sdir = (pathname_t *)((char *)sdir + PATHSIZE(sdir));
	}
}
#endif	DEBUG_PFS
#endif	PFS
