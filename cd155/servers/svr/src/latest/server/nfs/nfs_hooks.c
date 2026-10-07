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
 * $Log: nfs_hooks.c,v $
 * Revision 1.5  1994/11/18  20:36:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:15:50  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:37:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:09  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:31  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:02  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:20  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:51:03  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:53  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:38:56  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:21:11  gm]
 * 
 * Revision 1.3  90/09/13  11:48:32  devrcs
 * 	Also need to passthrough syscall args to nosys(),
 * 	and to return a value.
 * 	[90/08/30  16:40:55  tmt]
 * 
 * 	Add new syscall arguments to nfssvc and async_daemon, and
 * 	pass them on thru.
 * 	[90/08/30  15:36:42  sharpe]
 * 
 * Revision 1.2  90/08/24  12:15:33  devrcs
 * 	Created:  permanent hooks for dynamic NFS.
 * 	[90/08/18  03:45:35  nags]
 * 
 * $EndLog$
 */
/*
 * Permanently resident hooks for dynamically loading NFS.
 * Until system calls can be dynamically allocated in the
 * sysent[], and until zones can be dynamically destroyed,
 * this file contains the necessary stubs for the NFS
 * system calls and zones.
 */

#include <sys/sysconfig.h>
#include <sys/mount.h>
#include <sys/errno.h>
#include <sys/lock_types.h>
#include <kern/zalloc.h>
#include <nfs/nfssys.h>
#include <sys/lock_types.h>
#include <sys/proc.h>

/*
 * Zones used by the NFS code for dynamic data structures
 */
zone_t	nfsmount_zone = ZONE_NULL;
zone_t	sillyrename_zone = ZONE_NULL;
zone_t	nfsreq_zone = ZONE_NULL;
zone_t	nfsreaddir_zone = ZONE_NULL;

int     (*nfssvc_func)() = NULL_FUNC;
int     (*async_daemon_func)() = NULL_FUNC;

nfssvc(p, args, retval)
        struct proc *p;
        void *args;
        int *retval;
{
	int     (*func)();

	/*
	 * We take a blocking lock to ensure that NFS is not
	 * unloaded while we are starting a new daemon.
	 * nfs_nfssvc will unlock NFSD_LOCK for us.
	 */
	NFSD_LOCK();
	NFS_SYS_LOCK();
	func = nfssvc_func;
	NFS_SYS_UNLOCK();
	if (func == NULL_FUNC) {
		NFSD_UNLOCK();
		return nosys(p, args, retval);
	} else
		return (*func)(p, args, retval);
}

async_daemon(p, args, retval)
        struct proc *p;
        void *args;
        int *retval;
{

	int     (*func)();

	/*
	 * We take a blocking lock to ensure that NFS is not
	 * unloaded while we are starting a new daemon.
	 * nfs_async_daemon will unlock it for us.
	 */
	NFSD_LOCK();
	NFS_SYS_LOCK();
	func = async_daemon_func;
	NFS_SYS_UNLOCK();
	if (func == NULL_FUNC) {
		NFSD_UNLOCK();
		return nosys(p, args, retval);
	} else
		return (*func)(p, args, retval);
}


nfs_hooks_init()
{
	NFS_SYS_LOCK_INIT();
	NFSD_LOCK_INIT();
}
