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
 * This file was created by the Center for High Performance Computing of
 * of Worcester Polytechnic Institute (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: fsvr_rmtspec_ops.c,v $
 * Revision 1.8  1994/11/18  20:48:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/01/12  17:46:42  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.6  1993/09/27  04:38:17  robboy
 * Deleted "#if 1", which had previously been "#if FULLSERVER"
 *
 * Revision 1.5  1993/08/18  16:37:34  cfj
 * Fix for PTS bug #6098.  Turn on selected code which previously was turned off if FULLSERVER
 * was not defined to allow pipes to work with the lite server in compute paritions.
 *
 * Revision 1.4  1993/07/19  22:59:49  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.3  1993/07/14  18:41:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:02:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:54:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:21  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:52:49  cfj
 * Bump major revision number.
 *
 * Revision 2.9  1992/10/22  15:53:53  dbm
 * Added PFS functionality.
 *
 * Revision 2.11  93/07/13  16:12:23  slively
 *      Revision 2.9  93/06/29  16:17:24  rabii
 *      Lite server mods (rabii)
 *
 * Revision 2.10  93/06/25  11:24:48  slively
 * Backout the LITE server changes.  Remove #if UFS and include ufs.h
 * 
 * Revision 2.9  93/06/22  20:03:57  slively
 * Support for LITE server, #if UFS section.
 * 
 * Revision 2.8  92/09/20  11:24:44  roy
 * 	Another op for OSF1_ADFS.
 * 	[92/09/15            roy]
 *
 * Revision 2.7  92/08/26  12:13:53  loverso
 * 	Additional ops for OSF1_ADFS.
 * 	[92/08/19            roy]
 * 
 * Revision 2.6  92/08/13  19:21:26  rabii
 * 	Modified rmtspec_sync to not worry about access times. access times 
 * 	are now passed to the node holding the device special file by the  
 * 	routines rmtspec_getattr and rmtspec_close. Also implemented 
 * 	rmtspec_getinfo which will provide the caller with the access 
 * 	information of an rmtnode. Also removed rmttimes which is now
 * 	obsolete. (rabii)
 * 
 * Revision 2.5  92/06/08  18:30:07  pjg
 * 	Added new vector entry for getting remote device attributes (rabii)
 * 
 * Revision 2.4  92/03/16  18:28:37  pjg
 * 	92/03/16  20:22:31  noemi
 * 	Changed rmtspec_sync to get specinfo structure from remote vnode agent
 * 	and not from shadow vnode.
 * 
 * Revision 2.3  92/01/14  11:21:37  roy
 * 	92/01/10  22:07:08  noemi
 * 	Added rmtspec_sync.
 * 
 * Revision 2.2  92/01/05  20:20:47  roy
 * 	91/11/12  22:01:13  noemi
 * 	Initial revision
 * 
 */

#ifdef	OSF1_ADFS
#include <ufs.h>
#include <fullserver.h>
#include <uxkern/import_mach.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/uio.h>
#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <uxkern/rmtnode.h>
#include <kern/parallel.h>

#if     FULLSERVER
int 	spec_lookup(),
	spec_open(),
	rmtspec_close(),
	rmtspec_getattr(),
	rmtspec_read(),
	rmtspec_write(),
	spec_ioctl(),
	spec_badop(),
	spec_select(),
	spec_seek(),
	ufs_inactive(),
	spec_reclaim(),
	spec_bmap(),
	spec_strategy(),
	spec_page_read(),
	spec_page_write(),
	spec_nullop();
#else   /* FULLSERVER */
int     spec_badop();
#endif  /* FULLSERVER */

struct vnodeops spec_rmtnodeops = {
#if     FULLSERVER
        spec_lookup,            /* lookup */
        spec_badop,             /* create */
        spec_badop,             /* mknod */
        spec_open,              /* open */
        rmtspec_close,         	/* close */
        spec_badop,             /* access */
        rmtspec_getattr,        /* getattr */
        spec_badop,             /* setattr */
        rmtspec_read,           /* read */
        rmtspec_write,          /* write */
        spec_ioctl,             /* ioctl */
        spec_select,            /* select */
        spec_badop,             /* mmap */
        spec_nullop,            /* fsync */
        spec_seek,              /* seek */
        spec_badop,             /* remove */
        spec_badop,             /* link */
        spec_badop,             /* rename */
        spec_badop,             /* mkdir */
        spec_badop,             /* rmdir */
        spec_badop,             /* symlink */
        spec_badop,             /* readdir */
        spec_badop,             /* readlink */
        spec_badop,             /* abortop */
        ufs_inactive,           /* inactive */
        spec_reclaim,        	/* reclaim */
        spec_bmap,              /* bmap */
        spec_strategy,          /* strategy */
        spec_badop,             /* print */
        spec_page_read,         /* page_read */
        spec_page_write,        /* page_write */
#ifdef  PFS
	spec_badop,		/* preallocate and set size */
#endif
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	spec_badop,		/* update */
	spec_badop,		/* getsize */
#endif
#else   /* FULLSERVER */
        spec_badop,             /* lookup */
        spec_badop,             /* create */
        spec_badop,             /* mknod */
        spec_badop,             /* open */
        spec_badop,             /* close */
        spec_badop,             /* access */
        spec_badop,             /* getattr */
        spec_badop,             /* setattr */
        spec_badop,             /* read */
        spec_badop,             /* write */
        spec_badop,             /* ioctl */
        spec_badop,             /* select */
        spec_badop,             /* mmap */
        spec_badop,             /* fsync *
        spec_badop,             /* seek */
        spec_badop,             /* remove */
        spec_badop,             /* link */
        spec_badop,             /* rename */
        spec_badop,             /* mkdir */
        spec_badop,             /* rmdir */
        spec_badop,             /* symlink */
        spec_badop,             /* readdir */
        spec_badop,             /* readlink */
        spec_badop,             /* abortop */
        spec_badop,             /* inactive */
        spec_badop,             /* reclaim */
        spec_badop,             /* bmap */
        spec_badop,             /* strategy */
        spec_badop,             /* print */
        spec_badop,             /* page_read */
        spec_badop,             /* page_write */
	 /* preallocate and set size not included.  Assume that
		!FULLSERVER ==> !PFS */
        spec_badop,             /* pagein */
        spec_badop,             /* pageout */
        spec_badop,             /* alloc */
        spec_badop,             /* update */
        spec_badop,             /* getsize */
#endif  /* FULLSERVER */
};

int 		remote_vnode_agents = 0;
struct mutex 	rmt_agent_lock = MUTEX_INITIALIZER;


#if     FULLSERVER
/*
 * Read wrapper for special devices.
 */
rmtspec_read(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct rmtnode *rp = (struct rmtnode *)(vp->v_data);

	/*
	 * Set access flag.
	 */
	RMT_LOCK(rp);
	rp->r_flag |= RMTACC;
	RMT_UNLOCK(rp);
	return (spec_read(vp, uio, ioflag, cred));
}


/*
 * Write wrapper for special devices.
 */
rmtspec_write(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct rmtnode *rp = (struct rmtnode *)(vp->v_data);

	/*
	 * Set update and change flags.
	 */
	RMT_LOCK(rp);
	rp->r_flag |= RMTUPD|RMTCHG;
	RMT_UNLOCK(rp);
	return (spec_write(vp, uio, ioflag, cred));
}

/*
 * Close wrapper for special devices.
 */
rmtspec_close(vp, fflag, cred)
	register struct vnode *vp;
	int fflag;
	struct ucred *cred;
{
	register struct rmtnode *rp = (struct rmtnode *)(vp->v_data);
	struct specinfo *si;
	int	error;
	mach_port_t	specport;
	int		accessed = FALSE;
	int		modified = FALSE;
	struct	vattr	vat;

	si = vp->v_specinfo;
	SPEC_READ_LOCK(si);
	specport = si->si_specport;
	ASSERT(si && si->si_specport != MACH_PORT_NULL);
	RMT_LOCK(rp);
	if (rp->r_flag & (RMTUPD|RMTCHG)) {
		modified = TRUE;
	}
	if (rp->r_flag & RMTACC) {
		accessed = TRUE;
	}
	rp->r_flag &= ~(RMTACC|RMTUPD|RMTCHG);
	RMT_UNLOCK(rp);
	/*
	 * Now update the inode on node holding the real inode
	 */
	if (accessed == TRUE || modified == TRUE) {
		error = remote_updateinfo(specport, accessed, modified, &vat);
		if (error) {
			printf("remote_updateinfo: Unable to update 0x%x\n",
						error);
		}
	}
	SPEC_READ_UNLOCK(si);
	error = spec_close(vp, fflag, cred);
	return(error);
}
#endif  /* FULLSERVER */

/*
 * Sync any data associated with remote nodes
 */
rmtspec_sync()
{
#if     !FULLSERVER
        return(EINVAL);
#else   /* FULLSERVER */
	register struct vnode *vp, *nvp;
	register struct mount *mp = DEADMOUNT;

	MOUNT_VLIST_LOCK(mp);
	for (vp = mp->m_mounth; vp; vp = nvp) {
		VN_LOCK(vp);
		if (RMTNODE(vp)) {
			/*
			 * nvp will hold the vnode pointer to the vnode we're 
			 * flushing, which could be different from vp, the one
			 * that's on the mount vnode list, in the case of VBLK.
			 */
			if (vp->v_type == VBLK) {
				if ((nvp = shadowvnode(vp)) == NULLVP) {
					VN_UNLOCK(vp);
					nvp = vp->v_mountf;
					continue;
				}
			} else
				nvp = vp;
			VN_UNLOCK(vp);
			VN_LOCK(nvp);
			if (nvp->v_dirtyblkhd == NULL) {
				VN_UNLOCK(nvp);
				nvp = vp->v_mountf;
				continue;
			}
			/*
			 * Write out any modified blocks for the special
			 * file.  This mimics the UFS code.
			 */
			if (vget_nowait(nvp)) {
				VN_UNLOCK(nvp);
				nvp = vp->v_mountf;
				continue;
			}
			MOUNT_VLIST_UNLOCK(mp);

			if (nvp->v_dirtyblkhd) {
				VN_UNLOCK(nvp);
				vflushbuf(nvp, 0);
			} else
				VN_UNLOCK(nvp);

			VN_LOCK(vp);
			if (vget_nowait(vp)) {
				VN_UNLOCK(vp);
				MOUNT_VLIST_LOCK(mp);
				nvp = vp->v_mountf;
				continue;
			}
			VN_UNLOCK(vp);
			vrele(vp);
			vrele(nvp);
			MOUNT_VLIST_LOCK(mp);
		} else
			VN_UNLOCK(vp);
		nvp = (vp->v_mount == mp) ? vp->v_mountf : mp->m_mounth;
	}
	MOUNT_VLIST_UNLOCK(mp);

	/*
	 * Set up for another round.
	 */
	timeout(rmtspec_sync, (caddr_t)0, RMT_TIMEOUT * hz);
#endif  /* FULLSERVER */	  
}
#endif	/* OSF1_ADFS */

rmtspec_getattr(vp, vap, cred)
	struct vnode *vp;
	register struct vattr *vap;
	struct ucred *cred;
{
	struct specinfo *si;
	mach_port_t	specport;
	struct rmtnode *rp = (struct rmtnode *)(vp->v_data);
	int		error;
	int		accessed = FALSE;
	int		modified = FALSE;
	
	si = vp->v_specinfo;

	SPEC_READ_LOCK(si);
	specport = si->si_specport;
	ASSERT(si && si->si_specport != MACH_PORT_NULL);
	RMT_LOCK(rp);
	if (rp->r_flag & (RMTUPD|RMTCHG)) {
		modified = TRUE;
	}
	if (rp->r_flag & RMTACC) {
		accessed = TRUE;
	}
	rp->r_flag &= ~(RMTACC|RMTUPD|RMTCHG);
	RMT_UNLOCK(rp);

	/*
	 * Now do the actual getattr
	 */
	error = remote_updateinfo(specport, accessed, modified, vap);
	SPEC_READ_UNLOCK(si);
	return(error);
}

rmtspec_getinfo(vp, accessedp, modifiedp)
	struct vnode 	*vp;
	boolean_t	*accessedp;
	boolean_t	*modifiedp;
{
	struct rmtnode *rp = (struct rmtnode *)(vp->v_data);
	
	RMT_LOCK(rp);
	if (rp->r_flag & RMTACC) {
		*accessedp = TRUE;
	}
	if (rp->r_flag & (RMTUPD|RMTCHG)) {
		*modifiedp = TRUE;
	}
	rp->r_flag &= ~(RMTACC|RMTUPD|RMTCHG);
	RMT_UNLOCK(rp);
	return (0);
}
