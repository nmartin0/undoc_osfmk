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
 * $Log: nfssys.h,v $
 * Revision 1.5  1994/11/18  20:37:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:17:03  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:39:21  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:54  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:36:07  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:58  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:17  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:54:25  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:41:12  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:40:11  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:23:02  gm]
 * 
 * Revision 1.2  90/08/24  12:16:22  devrcs
 * 	Lock definitions for dynamic NFS loading.
 * 	[90/08/18  03:49:03  nags]
 * 
 * $EndLog$
 */
/*
 * Common functions for loadable NFS system calls.  Temporary
 * until we have general loadable system calls.
 */

#ifndef	_NFS_NFSSYS_H_
#define	_NFS_NFSSYS_H_


#define NULL_FUNC		(int (*)(int))0

udecl_simple_lock_data(,nfs_sys_lock)	/* for dynamic loading of nfs */
#define	NFS_SYS_LOCK()		usimple_lock(&nfs_sys_lock)
#define	NFS_SYS_UNLOCK()	usimple_unlock(&nfs_sys_lock)
#define	NFS_SYS_LOCK_INIT()	usimple_lock_init(&nfs_sys_lock)

lock_data_t	nfs_daemon_lock;	/* for dynamic loading/unloading */
#define	NFSD_LOCK()		lock_write(&nfs_daemon_lock)
#define	NFSD_UNLOCK()		lock_write_done(&nfs_daemon_lock)
#define	NFSD_LOCK_INIT()	lock_init2(&nfs_daemon_lock, TRUE, LTYPE_NFSD)

#endif	/* _NFS_NFSSYS_H_ */
