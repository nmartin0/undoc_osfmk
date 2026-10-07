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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: nfs_vfsops.c,v $
 * Revision 1.8  1995/02/01  21:35:51  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/11/18  20:37:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/28  23:06:12  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.5  1993/12/10  22:27:07  nina
 * Fixed bugs that prevented Paragons from being used
 * as NFS clients if the boot node is not a network
 * server node.  See #6831, #6719, #7421, #7422, #7423
 * #7424 and #7426.  nfs_mount() was modified to
 * contact the clearinghouse and find the "best"
 * network interface to use to contact the specified
 * NFS server and then forward the mount request to
 * the network server.  In addition, mountnfs() was
 * modified to use DEV_BSIZE instead of hard coded
 * constants when processing the user specified mount
 * options rsize and wsize.
 *
 *
 *  Reviewer:bolsen@locus.com, dbm@intel.com
 *  Risk:Medium
 *  Benefit or PTS #:6831, 6719, 5336
 *  Testing:Lachman NFS main suite, various configurations
 *  Module(s):./server/nfs/nfs_vfsops.c
 *
 * Revision 1.4  1993/07/14  18:16:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:38:38  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:35  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:55  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.7  1992/12/08  10:45:03  durriya
 * 	1.1 unmount sync changes - s/MOUNT_LOOKUP_DONE/UNMOUNT_READ_UNLOCK
 * 	and s/MOUNT_LOOKUP_LOCK_INIT/UNMOUNT_LOCK_INIT           (durriya)
 *
 * Revision 1.2  1992/11/30  22:32:38  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:48  cfj
 * Bump major revision number.
 *
 * Revision 2.8  94/02/03  11:01:18  dnoveck
 *      Changes for per-node buffer-cache block size: use bcace_maxbsize
 *      instead of MAXBSIZE.
 *
 * Revision 2.6  1992/04/05  16:58:34  pjg
 * 	Set mountid in the mount structure in mountnfs.     (durriya)
 * 
 * Revision 2.5  92/03/01  18:55:25  pjg
 * 	92/02/28  16:49:46  noemi
 * 	Changed NFS mount code to use mount structure ports instead of covered
 * 	vnode and file system root vnode ports.
 * 
 * Revision 2.4  92/02/21  16:22:25  durriya
 * 	get rid of NORMA_IPC conditionals
 * 
 * Revision 2.3  92/01/16  16:10:49  roy
 * 	92/01/16  13:32:40  pjg
 * 	Added support for NFS client. It now works with OSF1_ADFS.
 * 
 * Revision 2.2  91/08/31  13:52:18  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:40:37  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.6.3  91/06/20  14:58:49  tmt
 * 	Clip rw size to MAXBSIZE to avoid panics in getblk().
 * 	[91/06/20  08:54:29  tmt]
 * 
 * Revision 1.13.6.2  91/04/29  12:10:56  tmt
 * 	Clip [rw]size to transfer size advertised in fsstat.
 * 	[91/04/10  09:56:40  tmt]
 * 
 * Revision 1.13  90/10/31  14:02:07  devrcs
 * 	don't overwrite existing flags in nfs_root
 * 	[90/10/23  10:34:30  gmf]
 * 
 * Revision 1.12  90/10/07  14:39:36  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:22:09  gm]
 * 
 * 	Fix mountnfs() to eliminate remote root vnode in case
 * 	of a failure part way through the mount.
 * 	[90/09/24  14:31:49  gmf]
 * 
 * Revision 1.11  90/09/23  15:56:26  devrcs
 * 	Don't allow forcible unmount
 * 	[90/09/21  15:54:26  gmf]
 * 
 * 	New flags for vflush.
 * 	[90/09/03  22:35:32  nags]
 * 
 * Revision 1.10  90/08/24  12:16:07  devrcs
 * 	Move zones to nfs_hooks.c for dynamic NFS loading.
 * 	Add nfs_quotactl to nfs_vfsops.
 * 	[90/08/19  01:12:21  nags]
 * 
 * Revision 1.9  90/07/27  09:04:44  devrcs
 * 	NFS parallelization.
 * 	[90/07/20  17:04:02  nags]
 * 
 * 	nags merge
 * 	[90/06/12  21:36:17  nags]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:45:38  seiden]
 * 
 * 	Compressed history (reverse chronology):
 * 	Serialized for OSF/1.					nags@encore.com
 * 	Fsid change.  tmt's m_freem move in mountnfs.		gmf@osf.org
 * 	"Mount hang fix":  call nfs_statfs during mount.	gmf@osf.org
 * 	Fixed mount compatibility code; skip root vnode.	gmf@osf.org
 * 	Hold root vnode in mountnfs, release in nfs_unmount.	gmf@osf.org
 * 	Eliminate #ifdef VOP_PRINT (backward compatibility).	gmf@osf.org
 * 	Rearrange nfs_connect - see nfs_socket.c		tmt@osf.org
 * 	Improve transport independence.				tmt@osf.org
 * 	OSF/1 "one" snapshot revision.				tmt@osf.org
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	Fixes for first snapshot.				gm@osf.org
 * 	New networking code from BSD.				gm@osf.org
 * 	[90/06/10  02:03:10  seiden]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
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
 *	@(#)nfs_vfsops.c	7.10 (Berkeley) 11/30/89
 */

#include <sys/param.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/ucred.h>
#include <ufs/dir.h>
#include <sys/namei.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/errno.h>
#if	!MACH
#include <malloc.h>
#endif
#include <sys/buf.h>
#include <sys/mbuf.h>
#undef	m_data
#ifdef	OSF1_ADFS
#undef  m_next
#endif

#include <sys/socket.h>
#include <sys/systm.h>
#include <nfs/nfsv2.h>
#include <nfs/nfsnode.h>
#include <nfs/nfsmount.h>
#include <nfs/nfs.h>

#ifndef shouldbe
#include <sys/conf.h>
#endif

/*
 * nfs vfs operations.
 */
int nfs_mount();
int nfs_start();
int nfs_unmount();
int nfs_root();
int nfs_quotactl();
int nfs_statfs();
int nfs_sync();
int nfs_fhtovp();
int nfs_vptofh();
int nfs_init();

struct vfsops nfs_vfsops = {
	nfs_mount,
	nfs_start,
	nfs_unmount,
	nfs_root,
	nfs_quotactl,
	nfs_statfs,
	nfs_sync,
	nfs_fhtovp,
	nfs_vptofh,
	nfs_init,
};

static u_char nfs_mntid;
udecl_simple_lock_data(, nfsmntid_lock)
#define	NFSMNTID_LOCK()		usimple_lock(&nfsmntid_lock)
#define	NFSMNTID_UNLOCK()	usimple_unlock(&nfsmntid_lock)
#define	NFSMNTID_LOCK_INIT()	usimple_lock_init(&nfsmntid_lock)

/*
 * Called by vfs_mountroot when nfs is going to be mounted as root
 * Not Yet (By a LONG shot)
 */
nfs_mountroot()
{
	return (ENODEV);
}

/*
 * VFS Operations.
 *
 * mount system call
 * It seems a bit dumb to copyinstr() the host and path here and then
 * bcopy() them in mountnfs(), but I wanted to detect errors before
 * doing the sockargs() call because sockargs() allocates an mbuf and
 * an error after that means that I have to release the mbuf.
 */
/* ARGSUSED */
nfs_mount(mp, path, data, ndp)
	struct mount *mp;
	char *path;
	caddr_t data;
	struct nameidata *ndp;
{
	int error;
#ifndef	OSF1_ADFS
	struct nfs_args args;
#endif
	struct nfs_args *argsp;
	struct mbuf *saddr;
	char pth[MNAMELEN], hst[MNAMELEN];
	int len, flag;
	nfsv2fh_t nfh;

#ifdef OSF1_ADFS
	extern node_t this_node, root_fs_node;
	extern mach_port_t root_fs_port;
        mach_port_t mountport = MACH_PORT_NULL;
        mach_port_t remoteport = MACH_PORT_NULL;
        mach_port_t netserverport = MACH_PORT_NULL;

#ifdef TNC
	extern mach_port_t	clearinghouse_port;
	extern			find_clearinghouse();
	kern_return_t		kr;
        node_t      		netservernode = INVALID_NODE;
	struct sockaddr		*sa;
	int			rc;
#endif	/*TNC */
#endif	/* OSF1_ADFS */

	BM(MOUNT_LOCK(mp));
	flag = mp->m_flag;
	BM(MOUNT_UNLOCK(mp));
	if (flag & M_UPDATE)
		return (0);
#if SEC_FSCHANGE
	if (flag & M_SECURE)
		return EINVAL;
#endif

#ifdef	OSF1_ADFS
	argsp = (struct nfs_args *)data;
	bcopy((caddr_t)argsp->fh, (caddr_t)&nfh, sizeof(nfsv2fh_t));
	copystr(path, pth, MNAMELEN-1, &len);
	bzero(&pth[len], MNAMELEN-len);
	copystr(argsp->hostname, hst, MNAMELEN-1, &len);
	bzero(&hst[len], MNAMELEN-len);
#else
	if (error = copyin(data, (caddr_t)&args, sizeof (struct nfs_args)))
		return (error);
	if (error=copyin((caddr_t)argsp->fh, (caddr_t)&nfh, sizeof(nfsv2fh_t)))
		return (error);
	if (error = copyinstr(path, pth, MNAMELEN-1, &len))
		return (error);
	bzero(&pth[len], MNAMELEN-len);
	if (error = copyinstr(argsp->hostname, hst, MNAMELEN-1, &len))
		return (error);
	bzero(&hst[len], MNAMELEN-len);
	argsp = &args;
#endif /* OSF1_ADFS */

#ifdef	OSF1_ADFS
/*
 * This code assumes that if TNC is defined, OSF1_ADFS will also
 * be defined.  
 */

#ifdef TNC

	/*
	 * argsp points to the address of the NFS server. Call
	 * the clearinghouse to get the node  and port number
	 * of the "best" network server to communicate with
	 * the NFS server.  If netservernode != this_node,
	 * forward the mount request to netservernode.
	 * Before we query the clearinghouse, check the
	 * format of the sockaddr passed in by the mount cmd.
	 */

	/*
	 * Many commands are built with COMPAT_43 or equivalent
	 * define.  This causes them to pass "old" style sockaddr
	 * structs to the server. The server can also be built with
	 * COMPAT_43. However, the define _KERNEL causes the server to
	 * use the new sockaddr format.  The code that follows converts
	 * "old" style sockaddr formats so that the clearinghouse
	 * functions properly.
	 */
	sa = (struct sockaddr*)argsp->addr;
#if defined(COMPAT_43) && BYTE_ORDER != BIG_ENDIAN
        if (sa->sa_family == 0 && sa->sa_len < sizeof(struct sockaddr)) 
                sa->sa_family = sa->sa_len;
#endif
	sa->sa_len = sizeof(struct sockaddr);

	/* locate the clearinghouse before query*/
	rc = find_clearinghouse();
	if(rc != ESUCCESS){
		printf("nfs_mount: find_clearinghouse: rc %d\n",rc);
		return(rc);
	}		
	ux_server_thread_blocking();
	kr = if_get_server_by_route(clearinghouse_port,
				    this_node,
				    (char*)sa,
				    sa->sa_len,
				    &netservernode,
				    &netserverport);
	ux_server_thread_unblocking();

	if(kr != KERN_SUCCESS){
		panic("nfs_mount: by route failed");
		return(kr);
	}		

	/*
	 * If netservernode == INVALID_NODE, there is
	 * a configuration error, the clearinghouse is
	 * not working, or there is no way to reach
	 * the desired NFS server.
	 */
	if (netservernode == INVALID_NODE) 
		return(EIO);

	if (this_node != netservernode) {
#else	/* !TNC */
	if (this_node != root_fs_node) {
		netserverport = root_fs_port;
#endif	/* TNC */
		/*
		 * The network gateway is remote. Send remote_nfs_mount
		 * to continue the mount remotely
		 */

		/*
		 * Allocate a mount port for this mount structure and
		 * let the remote node complete the mount.
		 */
		mount_port_allocate(mp, &mountport);
		error = remote_nfs_mountfs(netserverport, mountport,
					   pth, flag, argsp, &remoteport);

		/* deallocate netserverport if necessary (not yet) */
		if (error) {
			/*
			 * Deallocate the mount structure port.
			 */
			mount_port_deallocate(mp);
			return (error);
		}
		ASSERT(remoteport != MACH_PORT_NULL);

		MOUNT_LOCK(mp);
		mp->m_flag |= M_REMOTE_FS;
		mp->m_remoteport = remoteport;
#if	MACH
		/*
		 * this is temporary, until the inode pager can handle nfs
		 * swapping.
		 */
		mp->m_flag |= M_SWAP_NEVER;
#endif
		MOUNT_UNLOCK(mp);
		/*
		 * Partially set up statfs structure in mount structure
		 * so getfsstat works right.
		 */
		mp->m_stat.f_type = MOUNT_NFS;
		mp->m_stat.f_flags = mp->m_flag;
		(void) bcopy(pth, (caddr_t)mp->m_stat.f_mntonname, MNAMELEN);
		(void) bcopy(hst, mp->m_stat.f_mntfromname, MNAMELEN);
		return(0);
	}
#endif	/* OSF1_ADFS */

	/* sockargs() call must be after above copyin() calls */
	if (error = sockargs(&saddr, (caddr_t)argsp->addr,
			     sizeof (struct sockaddr), MT_SONAME))
		return (error);
	argsp->fh = &nfh;
	error = mountnfs(argsp, mp, saddr, pth, hst);
#if	MACH
	/*
	 * this is temporary, until the inode pager can handle nfs
	 * swapping.
	 */
	MOUNT_LOCK(mp);
	mp->m_flag |= M_SWAP_NEVER;
	MOUNT_UNLOCK(mp);
#endif
	return (error);
}

#ifdef	OSF1_ADFS
/*
 * This routine handles remote mount requests (client NFS)
 */
nfs_mountfs(remoteport, pathname, flag, args, mountportp)
	mach_port_t remoteport;
	char *pathname;
	int flag;
	struct nfs_args *args;
	mach_port_t *mountportp;
{
	int error;
	struct mbuf *saddr;
	struct mount *mp;
	struct vnode *fsrootvpp;

	/*
	 * Check the arguments first, to avoid having to release the
	 * mount structure in case of error.
	 */
	if (error = sockargs(&saddr, (caddr_t)args->addr,
		sizeof (struct sockaddr), MT_SONAME))
		return (error);

	/*
	 * Allocate and initialize a mount structure.
	 */
	MOUNT_ALLOCATE(mp);
	mp->m_op = vfssw[MOUNT_NFS];
	mp->m_flag = (flag | M_REMOTE_DIR);
	mp->m_exroot = 0;
	mp->m_mounth = (struct vnode *)0;
	mp->m_remoteport = remoteport;
	mp->m_vnodecovered = (struct vnode *)0;
#if     SER_COMPAT
	mp->m_funnel = !FUNNEL_NULL;    /* XXX */
#endif
	UNMOUNT_LOCK_INIT(mp);
	MOUNT_VLIST_LOCK_INIT(mp);
	MOUNT_LOCK_INIT(mp);

	if ((error = mountnfs(args, mp, saddr, pathname, args->hostname))) {
		MOUNT_DEALLOCATE(mp);
		return(error);
	}
#if     MACH
	/*
	 * this is temporary, until the inode pager can handle nfs
	 * swapping.
	 */
	MOUNT_LOCK(mp);
	mp->m_flag |= M_SWAP_NEVER;
	MOUNT_UNLOCK(mp);
#endif
	/*
	 * Allocate a port for the mount structure.
	 */
	mount_port_allocate(mp, mountportp);

	/*
	 * Add the new mount structure to the mount list.
	 */
	MOUNTLIST_LOCK();
	mp->m_next = mounth.m_next;
	mp->m_prev = (struct mount *) &mounth;
	mounth.m_next = mp;
	mp->m_next->m_prev = mp;
	MOUNTLIST_UNLOCK();
 	return(0);
}
#endif	/* OSF1_ADFS */

/*
 * Common code for mount and mountroot
 */
mountnfs(argp, mp, saddr, pth, hst)
	register struct nfs_args *argp;
	register struct mount *mp;
	register struct mbuf *saddr;
	char *pth, *hst;
{
	register struct nfsmount *nmp;
	struct nfsnode *np;
	int error;
	fsid_t tfsid;
	struct mount *xmp;

#if	MACH
	if ((nmp = (struct nfsmount *)zalloc(nfsmount_zone)) == 
		(struct nfsmount *) 0)
		return (ENOMEM);
#else
	MALLOC(nmp, struct nfsmount *, sizeof *nmp, M_NFSMNT, M_WAITOK);
#endif
	bzero((caddr_t)nmp, sizeof *nmp);
	mp->m_data = (qaddr_t)nmp;
	/*
	 * Generate a unique nfs mount id. The problem is that a dev number
	 * is not unique across multiple systems. The techique is as follows:
	 * 1) Set to nblkdev,0 which will never be used otherwise
	 * 2) Generate a first guess as nblkdev,nfs_mntid where nfs_mntid is
	 *	NOT 0
	 * 3) Loop searching the mount list for another one with same id
	 *	If a match, increment val[0] and try again
	 * NB: I increment val[0] { a long } instead of nfs_mntid { a u_char }
	 *	so that nfs is not limited to 255 mount points
	 *     Incrementing the high order bits does no real harm, since it
	 *     simply makes the major dev number tick up. The upper bound is
	 *     set to major dev 127 to avoid any sign extention problems
	 */
	mp->m_stat.f_fsid.val[0] = makedev(nblkdev, 0);
	mp->m_stat.f_fsid.val[1] = MOUNT_NFS;
	NFSMNTID_LOCK();
	if (++nfs_mntid == 0)
		++nfs_mntid;
	NFSMNTID_UNLOCK();
	BM(NFSMNTID_LOCK());
	tfsid.val[0] = makedev(nblkdev, nfs_mntid);
	BM(NFSMNTID_UNLOCK());
	tfsid.val[1] = MOUNT_NFS;
	while (xmp = getvfs(&tfsid)) {
		UNMOUNT_READ_UNLOCK(xmp);
		tfsid.val[0]++;
		NFSMNTID_LOCK();
		nfs_mntid++;
		NFSMNTID_UNLOCK();
	}
	if (major(tfsid.val[0]) > 127) {
		error = ENOENT;
		m_freem(saddr);
		goto bad;
	}
	mp->m_stat.f_fsid.val[0] = tfsid.val[0];

#ifdef OSF1_ADFS
        SET_MOUNTID(mp);
#endif
#if	SER_COMPAT
	mp->m_funnel = FUNNEL_NULL;
#endif
	nmp->nm_mountp = mp;
	nmp->nm_flag = argp->flags;
	nmp->nm_rto = NFS_TIMEO;
	nmp->nm_rtt = -1;
	nmp->nm_rttvar = nmp->nm_rto << 1;
	nmp->nm_retry = NFS_RETRANS;
	nmp->nm_wsize = NFS_WSIZE;
	nmp->nm_rsize = NFS_RSIZE;
	bcopy((caddr_t)argp->fh, (caddr_t)&nmp->nm_fh, sizeof(nfsv2fh_t));
	bcopy(hst, mp->m_stat.f_mntfromname, MNAMELEN);
	bcopy(pth, mp->m_stat.f_mntonname, MNAMELEN);

	if ((argp->flags & NFSMNT_TIMEO) && argp->timeo > 0) {
		nmp->nm_rto = argp->timeo;
		/* NFS timeouts are specified in 1/10 sec. */
		nmp->nm_rto = (nmp->nm_rto * 10) / NFS_HZ;
		if (nmp->nm_rto < NFS_MINTIMEO)
			nmp->nm_rto = NFS_MINTIMEO;
		else if (nmp->nm_rto > NFS_MAXTIMEO)
			nmp->nm_rto = NFS_MAXTIMEO;
		nmp->nm_rttvar = nmp->nm_rto << 1;
	}

	if ((argp->flags & NFSMNT_RETRANS) && argp->retrans >= 0) {
		nmp->nm_retry = argp->retrans;
		if (nmp->nm_retry > NFS_MAXREXMIT)
			nmp->nm_retry = NFS_MAXREXMIT;
	}

	if ((argp->flags & NFSMNT_WSIZE) && argp->wsize > 0) {
		nmp->nm_wsize = argp->wsize;
		/* Round down to multiple of disk_granule */
		nmp->nm_wsize &= ~(DISK_GRANULE - 1);
		if (nmp->nm_wsize <= 0)
			nmp->nm_wsize = DISK_GRANULE;
		else if (nmp->nm_wsize > NFS_MAXDATA)
			nmp->nm_wsize = NFS_MAXDATA;
	}

	if ((argp->flags & NFSMNT_RSIZE) && argp->rsize > 0) {
		nmp->nm_rsize = argp->rsize;
		/* Round down to multiple of blocksize */
		nmp->nm_rsize &= ~(DISK_GRANULE - 1);
		if (nmp->nm_rsize <= 0)
			nmp->nm_rsize = DISK_GRANULE;
		else if (nmp->nm_rsize > NFS_MAXDATA)
			nmp->nm_rsize = NFS_MAXDATA;
	}
	/* Prevent panics in getblk(). */
	if (nmp->nm_rsize > bcache_maxbsize)
		nmp->nm_rsize = bcache_maxbsize;
	if (nmp->nm_wsize > bcache_maxbsize)
		nmp->nm_wsize = bcache_maxbsize;

	/* Set up the sockets and per-host congestion */
	if (error = nfs_connect(nmp, saddr))
		goto bad;

	/*
	 * A reference count is needed on the nfsnode representing the
	 * remote root.  If this object is not persistent, then backward
	 * traversals of the mount point (i.e. "..") will not work if
	 * the nfsnode gets flushed out of the cache.  Ufs doesn't have
	 * this problem, because one can identify root inodes by their
	 * number == ROOTINO (2).
	 */
	if (error = nfs_nget(mp, &nmp->nm_fh, &np))
		goto bad;

	/*
	 * do nfs_statfs call to prime the mount point's cache
	 */
	if (error = nfs_statfs(mp)) {
		struct vnode *vp = NFSTOV(np);
		/*
		 * Nobody knows about this vnode yet (mount structure
		 * isn't connected yet), so we can safely nuke it.
		 */
		VN_LOCK(vp);
		vgone(vp, VX_NOSLEEP, 0);
		VN_UNLOCK(vp);
		vrele(vp);
	} else {
		if (mp->m_stat.f_bsize > 0) {
			if (nmp->nm_wsize > mp->m_stat.f_bsize)
				nmp->nm_wsize = mp->m_stat.f_bsize;
			if (nmp->nm_rsize > mp->m_stat.f_bsize)
				nmp->nm_rsize = mp->m_stat.f_bsize;
		}
		return (0);
	}
bad:
	nfs_disconnect(nmp);
#if	MACH
	ZFREE(nfsmount_zone, nmp);
#else
	FREE(nmp, M_NFSMNT);
#endif
	return (error);
}

/*
 * unmount system call
 */
nfs_unmount(mp, mntflags)
	struct mount *mp;
	int mntflags;
{
	register struct nfsmount *nmp;
	struct nfsnode *np;
	struct vnode *vp;
	int flags = 0;
	int error;

	if (mntflags & MNT_FORCE)
		return (EINVAL);
	if (mntflags & MNT_FORCE)
		flags |= FORCECLOSE;
	nmp = vfs_to_nfs(mp);
	/*
	 * Clear out the buffer cache
	 */
	mntflushbuf(mp, 0);
	if (mntinvalbuf(mp))
		return (EBUSY);
	/*
	 * Goes something like this..
	 * - Check for activity on the root vnode (other than ourselves).
	 * - Call vflush() to clear out vnodes for this file system,
	 *   except for the root vnode.
	 * - Decrement reference on the vnode representing remote root.
	 * - Close the socket
	 * - Free up the data structures
	 */
	/*
	 * We need to decrement the ref. count on the nfsnode representing the
	 * remote root.  See comment in mountnfs().  The VFS unmount() has
	 * done vput on this vnode, otherwise we'd get deadlock!
	 */
	if (error = nfs_nget(mp, &nmp->nm_fh, &np))
		return(error);
	vp = NFSTOV(np);
	BM(VN_LOCK(vp));
	if (vp->v_usecount > 2) {
		BM(VN_UNLOCK(vp));
		vrele(vp);
		return (EBUSY);
	}
	BM(VN_UNLOCK(vp));
	if (error = vflush(mp, vp, flags)) {
		vrele(vp);
		return (error);
	}
	/*
	 * Get rid of TWO reference counts.
	 * Must vgone the vnode as well.  It points to the old mount
	 * structure, which is going to be a bad pointer very soon.
	 */
	vrele(vp);
	VN_LOCK(vp);
	vgone(vp, VX_NOSLEEP, 0);
	VN_UNLOCK(vp);
	vrele(vp);

	nfs_disconnect(nmp);
#if	MACH
	ZFREE(nfsmount_zone, nmp);
#else
	FREE(nmp, M_NFSMNT);
#endif
	return (0);
}

/*
 * Return root of a filesystem
 */
nfs_root(mp, vpp)
	struct mount *mp;
	struct vnode **vpp;
{
	register struct vnode *vp;
	struct nfsmount *nmp;
	struct nfsnode *np;
	int error;

	nmp = vfs_to_nfs(mp);
	if (error = nfs_nget(mp, &nmp->nm_fh, &np))
		return (error);
	vp = NFSTOV(np);
	VN_LOCK(vp);
	vp->v_type = VDIR;
	vp->v_flag |= VROOT;
	VN_UNLOCK(vp);
	*vpp = vp;
	return (0);
}

extern int syncprt;

/*
 * Flush out the buffer cache
 */
/* ARGSUSED */
nfs_sync(mp, waitfor)
	struct mount *mp;
	int waitfor;
{
	if (syncprt)
		bufstats();
	/*
	 * Force stale buffer cache information to be flushed.
	 */
	mntflushbuf(mp, waitfor == MNT_WAIT ? B_SYNC : 0);
	return (0);
}

/*
 * At this point, this should never happen
 */
/* ARGSUSED */
nfs_fhtovp(mp, fhp, vpp)
	struct mount *mp;
	struct fid *fhp;
	struct vnode **vpp;
{

	return (EINVAL);
}

/*
 * Vnode pointer to File handle, should never happen either
 */
/* ARGSUSED */
nfs_vptofh(mp, fhp, vpp)
	struct mount *mp;
	struct fid *fhp;
	struct vnode **vpp;
{

	return (EINVAL);
}

/*
 * Vfs start routine, a no-op.
 */
/* ARGSUSED */
nfs_start(mp, flags)
	struct mount *mp;
	int flags;
{

	return (0);
}

/*
 * Do operations associated with quotas, not supported
 */
nfs_quotactl(mp, cmd, uid, arg)
	struct mount *mp;
	int cmd;
	uid_t uid;
	caddr_t arg;
{
#ifdef lint
	mp = mp; cmd = cmd; uid = uid; arg = arg;
#endif /* lint */
	return (EOPNOTSUPP);
}

nfs_mntidinit()
{
	NFSMNTID_LOCK_INIT();
}
