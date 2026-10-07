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
 * $Log: ipc.h,v $
 * Revision 1.6  1995/02/01  21:36:22  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:40:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:25:14  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:19:22  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:42:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:45  cfj
 * Bump major revision number.
 * Revision 1.1.1.1  1993/05/03  17:41:38  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  93/06/02  09:54:21  yazz
 * For Sys V IPC under TNC define the svipc structure used to keep track of
 * svipc operations on a system-wide basis.
 * 
 * Revision 2.2  91/08/31  14:07:09  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:43:14  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:51:08  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:40:31  gm]
 * 
 * Revision 1.5  90/08/09  13:28:44  devrcs
 * 	Move key_t to types.h, as X/OPEN requires.
 * 	[90/08/03  16:28:29  bet]
 * 
 * Revision 1.4  90/06/22  20:53:08  devrcs
 * 	Changed uid, gid, and mode fields of struct ipc_perm to ushort,
 * 	as per SVID.
 * 	[90/06/06  13:04:56  ers]
 * 
 * Revision 1.3  90/04/27  19:25:56  devrcs
 * 	Integrated with AIX header.
 * 	[90/04/20  10:50:14  bet]
 * 
 * Revision 1.2  90/02/23  00:32:45  devrcs
 * 	Initial version of System V IPC common header file.
 * 	[90/02/20  11:19:57  bet]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)ipc.h	1.9  com/inc/sys,3.1,8943 9/7/89 08:16:38 */
#ifndef _SYS_IPC_H_
#define _SYS_IPC_H_

#include	<standards.h>

#ifdef _XOPEN_SOURCE

#include <sys/types.h>

typedef long    mtyp_t;		/* ipc message type */

/* Common IPC Structures */
struct ipc_perm {
	ushort		uid;		/* owner's user id	*/
	ushort		gid;		/* owner's group id	*/
	ushort		cuid;		/* creator's user id	*/
	ushort		cgid;		/* creator's group id	*/
	ushort		mode;		/* access modes		*/
	u_short		seq;		/* slot usage sequence number */
	key_t		key;		/* key			*/
};

/* common IPC operation flag definitions */
#define	IPC_CREAT	0001000		/* create entry if key doesn't exist */
#define	IPC_EXCL	0002000		/* fail if key exists */
#define	IPC_NOWAIT	0004000		/* error if request must wait */

/* Keys. */
#define	IPC_PRIVATE	(key_t)0	/* private key */

/* Control Commands. */
#define	IPC_RMID	0	/* remove identifier */
#define	IPC_SET		1	/* set options */
#define	IPC_STAT	2	/* get options */

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE

/* Common ipc_perm mode Definitions. */
#define	IPC_ALLOC	0100000		/* entry currently allocated        */
#define	IPC_R		0000400		/* read or receive permission       */
#define	IPC_W		0000200		/* write or send permission	    */

#endif /* _OSF_SOURCE */

#if defined(TNC) && defined(_KERNEL)
#include <uxkern/import_mach.h>
/*
 * Structure used by the System V IPC server under TNC.
 */
#define	SV_MAGIC	0x050578
struct svipc {
	long			sv_magic;
	struct svipc		*sv_forw;	/* doubly linked list */
	struct svipc		*sv_back;
	pid_t			sv_pid;
	struct mutex		sv_lock;
	struct sem_undo		*sv_semundo;
	u_short			sv_shmsegs;
	u_short			sv_refcnt;
};
#endif /* TNC */

#endif /* _SYS_IPC_H_ */
