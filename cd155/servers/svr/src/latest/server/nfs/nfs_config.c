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
 * $Log: nfs_config.c,v $
 * Revision 1.5  1994/11/18  20:36:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:15:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:37:51  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:05  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:29  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:09  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:18  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:50:56  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:48  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/31  14:01:59  devrcs
 * 	Normalized filesys config
 * 	[90/10/07  09:12:37  knight]
 * 
 * Revision 1.3  90/10/07  14:38:51  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:21:05  gm]
 * 
 * 	Changed configure_nfs() -> nfs_configure().
 * 	Subsystem configure entry points need the form: {subsystem}_configure.
 * 	[90/09/27  16:50:49  knight]
 * 
 * Revision 1.2  90/08/24  12:15:27  devrcs
 * 	Created:  NFS dynamic configuration routines.
 * 	[90/08/18  03:45:31  nags]
 * 
 * $EndLog$
 */
/*
 * Routines for dynamically loading and unloading
 * NFS functionality and system calls from the kernel.
 */

#include <sys/sysconfig.h>
#include <sys/mount.h>
#include <sys/errno.h>
#include <nfs/nfssys.h>

extern int     (*nfssvc_func)();
extern int     (*async_daemon_func)();

extern struct vfsops nfs_vfsops;
extern nfs_nfssvc(), nfs_async_daemon();

int
nfs_configure(op, indata, indatalen, outdata, outdatalen)
	sysconfig_op_t		op;
	filesys_config_t *	indata;
	size_t	 		indatalen;
        filesys_config_t * 	outdata;
        size_t          	outdatalen;
{
	int	ret;

	switch (op) {
		case SYSCONFIG_CONFIGURE:
			if ((ret = vfssw_add(MOUNT_NFS, &nfs_vfsops)) != 0)
				return(ret);
			break;
		case SYSCONFIG_UNCONFIGURE:
			if ((ret = nfs_unconfig()) != 0)
				return(ret);
			break;
		default:
			return EINVAL;
			break;
	}
        if (outdata != NULL && outdatalen == sizeof(filesys_config_t)) {
                outdata->fc_version = OSF_FILESYS_CONFIG_10;
                outdata->fc_type = MOUNT_NFS;
        }
	return 0;
}


nfs_syscall_config()
{
	/*
	 * Configure system calls
	 */
	NFS_SYS_LOCK();
	nfssvc_func = nfs_nfssvc;
	async_daemon_func = nfs_async_daemon;
	NFS_SYS_UNLOCK();
	return(0);
}


nfs_unconfig()
{
	int error;
	extern int nfs_nfsds, nfs_asyncdaemons;

	NFSD_LOCK();
	if (nfs_nfsds != 0 || nfs_asyncdaemons != 0) {
		NFSD_UNLOCK();
		return(EBUSY);
	}
	if (error = vfssw_del(MOUNT_NFS)) {
		NFSD_UNLOCK();
		return(error);
	}
	NFS_SYS_LOCK();
	nfssvc_func = NULL_FUNC;
	async_daemon_func = NULL_FUNC;
	NFS_SYS_UNLOCK();
	NFSD_UNLOCK();
}
