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
 * This source file was  modified by the Center for High Performance 
 * Computing ("CHPC") on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: lock_types.h,v $
 * Revision 1.5  1994/11/18  20:40:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:25:38  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:19:27  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:42:34  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:09  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:05  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:41:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1992/12/08  10:45:16  durriya
 * 	1.1 unmount sync changes - s/LTYPE_MOUNT_LOOKUP/LTYPE_UNMOUNT
 *
 * Revision 2.3  92/01/05  20:07:42  roy
 * 	1991/11/12  17:26:16  noemi
 * 	Added LTYPE_SPEC.
 * 
 * Revision 2.2  91/08/31  14:07:57  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:43:31  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  14:51:52  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:41:41  gm]
 * 
 * Revision 1.8  90/09/23  15:59:51  devrcs
 * 	Quota locks.
 * 	[90/09/03  22:35:47  nags]
 * 
 * Revision 1.7  90/08/24  12:26:24  devrcs
 * 	Added NFS dynamic loading lock.
 * 	[90/08/18  03:49:35  nags]
 * 
 * Revision 1.6  90/07/27  09:07:15  devrcs
 * 	Device switch locks, cleanup.
 * 	[90/07/20  17:06:27  nags]
 * 
 * 	New mmax isr scheme (bolinger@encore.com), NFS locks (nags).
 * 	[90/07/17  07:39:28  nags]
 * 
 * 	nags merge
 * 
 * 	Condensed history:
 * 	Added LTYPE_NFSREQ for nfs client requests.	tmt@osf.org
 * 	Added LTYPE_DOMAIN for net domain lock.		tmt@osf.org
 * 	Network category and lock types.		boykin@encore.com
 * 	Created [88/08/25].				alan@encore.com
 * 	[90/06/12  21:38:40  nags]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.
 * All Rights Reserved.
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */

/*
 * lock_types.h:  distinguishing values for various locks.
 */

#ifndef	_SYS_LOCK_TYPES_H_
#define	_SYS_LOCK_TYPES_H_

#define	LTYPE_FS_START		0
#define	LTYPE_DEFAULT_UNI	(LTYPE_FS_START + 1)
#define	LTYPE_FILE_IO		(LTYPE_FS_START + 2)
#define	LTYPE_SPECH		(LTYPE_FS_START + 3)
#define	LTYPE_UNMOUNT   	(LTYPE_FS_START + 4)
#define	LTYPE_BUF		(LTYPE_FS_START + 5)
#define	LTYPE_INODE_IO		(LTYPE_FS_START + 6)
#define	LTYPE_HOSTNAME		(LTYPE_FS_START + 7)
#define	LTYPE_PTY		(LTYPE_FS_START + 8)
#define	LTYPE_SCC_TTY		(LTYPE_FS_START + 10)
#define	LTYPE_FICHAIN		(LTYPE_FS_START + 11)
#define	LTYPE_VFSSW		(LTYPE_FS_START + 12)
#define	LTYPE_NFSNODE_IO	(LTYPE_FS_START + 13)
#define	LTYPE_CDEVSW		(LTYPE_FS_START + 14)
#define	LTYPE_BDEVSW		(LTYPE_FS_START + 15)
#define	LTYPE_NFSD		(LTYPE_FS_START + 16)
#define	LTYPE_DQUOT		(LTYPE_FS_START + 17)
#define	LTYPE_UMPQ		(LTYPE_FS_START + 18)
#define	LTYPE_TTY		(LTYPE_FS_START + 19)
#define	LTYPE_SPEC		(LTYPE_FS_START + 20)
#define	LTYPE_FS_LAST		(LTYPE_FS_START + 20)

#define	LTYPE_INTR_START	(LTYPE_FS_LAST + 0)
#define	LTYPE_SWINTR		(LTYPE_INTR_START + 1)

/*
 * Network
 */
#define	LTYPE_NET_START		128
#define	LTYPE_TCP		(LTYPE_NET_START+0)
#define	LTYPE_INPCB		(LTYPE_NET_START+1)
#define	LTYPE_SOCKET		(LTYPE_NET_START+2)
#define	LTYPE_UDP		(LTYPE_NET_START+3)
#define	LTYPE_IP		(LTYPE_NET_START+4)
#define	LTYPE_RAW		(LTYPE_NET_START+5)
#define	LTYPE_ROUTE		(LTYPE_NET_START+6)
#define	LTYPE_ARP		(LTYPE_NET_START+7)
#define	LTYPE_SOCKBUF		(LTYPE_NET_START+8)
#define	LTYPE_DOMAIN		(LTYPE_NET_START+9)
#define	LTYPE_NFSREQ		(LTYPE_NET_START+10)
#define	LTYPE_NET_LAST		(LTYPE_NET_START+10)

#endif	/* _SYS_LOCK_TYPES_H_ */
