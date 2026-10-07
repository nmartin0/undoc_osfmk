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
 * $Log: pfs_vnops.c,v $
 * Revision 1.5  1994/11/18  20:38:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/05/27  01:55:17  brad
 * Added ufs_datain/ufs_dataout vnode ops (for Fast Path support).
 * Removed debug, and removed pfs_create() and pfs_open() which
 * are no longer used.
 *
 * Revision 1.3  1993/04/03  03:07:04  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  23:04:59  dbm
 * Added PFS token functionality.
 *
 * Revision 1.2  1992/11/30  22:33:46  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:11:07  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/10  15:22:00  cfj
 * Put into NX branch.
 *
 * Revision 1.1  1992/11/05  22:35:02  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1  1992/10/22  15:24:11  dbm
 * Added for PFS functionality.
 *
 *
 *
 */

#include <ufs_nbc.h>
#include <mapped_files.h>

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif
#if SEC_ARCH
#include <sys/secpolicy.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/buf.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/conf.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <ufs/quota.h>
#include <ufs/inode.h>
#include <ufs/fs.h>
#if	MACH
#include <sys/syslimits.h>
#include <mach/memory_object.h>
#include <builtin/inode_pager.h>
#include <kern/mfs.h>
#include <kern/assert.h>
#include <kern/parallel.h>
#endif
#include <uxkern/mf.h>

#ifdef	PFS

/*
 * Global vfs data structures for pfs.
 */

extern int
	ufs_lookup(),
	ufs_create(),
	ufs_mknod(),
	ufs_open(),
	ufs_close(),
	ufs_access(),
	ufs_getattr(),
	ufs_setattr(),
	ufs_read(),
	ufs_write(),
	ufs_ioctl(),
	seltrue(),
	ufs_mmap(),
	ufs_fsync(),
	ufs_seek(),
	ufs_remove(),
	ufs_link(),
	ufs_rename(),
	ufs_mkdir(),
	ufs_rmdir(),
	ufs_symlink(),
	ufs_readdir(),
	ufs_readlink(),
	ufs_abortop(),
	ufs_inactive(),
	ufs_reclaim(),
	ufs_bmap(),
	ufs_strategy(),
	ufs_print(),
	ufs_page_read(),
	ufs_page_write(),
	ufs_prealloc(),
	ufs_pagein(),
	ufs_pageout(),
	ufs_alloc(),
	ufs_getsize(),
	ufs_datain(),
	ufs_dataout();

int	pfs_setattr();
int	pfs_update();

struct vnodeops pfs_vnodeops = {
	ufs_lookup,		/* lookup */
	ufs_create,		/* create */
	ufs_mknod,		/* mknod */
	ufs_open,		/* open */
	ufs_close,		/* close */
	ufs_access,		/* access */
	ufs_getattr,		/* getattr */
	pfs_setattr,		/* setattr */
	ufs_read,		/* read */
	ufs_write,		/* write */
	ufs_ioctl,		/* ioctl */
	seltrue,		/* select */
	ufs_mmap,		/* mmap */
	ufs_fsync,		/* fsync */
	ufs_seek,		/* seek */
	ufs_remove,		/* remove */
	ufs_link,		/* link */
	ufs_rename,		/* rename */
	ufs_mkdir,		/* mkdir */
	ufs_rmdir,		/* rmdir */
	ufs_symlink,		/* symlink */
	ufs_readdir,		/* readdir */
	ufs_readlink,		/* readlink */
	ufs_abortop,		/* abortop */
	ufs_inactive,		/* inactive */
	ufs_reclaim,		/* reclaim */
	ufs_bmap,		/* bmap */
	ufs_strategy,		/* strategy */
	ufs_print,		/* print */
	ufs_page_read,		/* page_read */
	ufs_page_write,		/* page_write */
	ufs_prealloc,		/* prealloc */
#ifdef  OSF1_ADFS
        ufs_pagein,             /* pagein */
        ufs_pageout,            /* pageout */
        ufs_alloc,              /* alloc */
        pfs_update,             /* update */
        ufs_getsize,            /* getsize */
	ufs_datain,		/* datain */
	ufs_dataout,		/* dataout */
#endif
};


/*
 * Set attribute vnode op. called from several syscalls.  All we do here is
 * filter out requests to set the file size (e.g. when O_TRUNC specified on
 * open()).  These requests are invalid on a PFS header file, which contains
 * meta information that is maintained by the PFS file system.
 */
pfs_setattr(vp, vap, cred)
	register struct vnode *vp;
	register struct vattr *vap;
	register struct ucred *cred;
{
	vap->va_size = VNOVAL;

	return(ufs_setattr(vp, vap, cred));
}


pfs_update(vp, accessed, modified, size, flags, cred)
	register struct vnode 	*vp;
	int	 		accessed;
	int	 		modified;
	esize_t			*size;
	int			flags;
	struct ucred 		*cred;
{

	/*
	 * Update the PFS length:
	 */
	vp->v_vm_info->pfs_length.shigh = size->shigh;
	vp->v_vm_info->pfs_length.slow  = size->slow;

	return(ESUCCESS);
}
#endif	PFS
