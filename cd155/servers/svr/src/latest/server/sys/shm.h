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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * HISTORY
 * $Log: shm.h,v $
 * Revision 1.5  1994/11/18  20:41:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/10  23:50:37  jlitvin
 * Fix PTS bug #5281.  shmctl() sometimes only requires 2 parameters, so
 * don't require 3 in the prototype.
 *
 * Revision 1.3  1993/07/14  18:27:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:20  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.6  93/06/02  09:55:57  yazz
 * For Sys V IPC under TNC change the pid fields from u_short to pid_t.
 * 
 * Revision 2.5  92/05/18  12:28:31  roy
 * 	Change svr_port to a node number in shmid_internal structure.
 * 	[92/05/17            roy]
 *
 * Revision 2.4  92/03/20  11:39:20  pjg
 * 	For OSF1_ADFS also add svr_port to shmid_internal       (durriya)
 * 
 * 	[Revision 3.3  91/12/17  17:19:07  jose]
 * 	Added the object internal name to shmid_internal
 * 	to deallocate correctly.
 * 
 * Revision 2.3  91/12/16  17:56:45  roy
 * 	91/10/31  12:27:28  david
 * 	added for server support
 * 
 * Revision 2.2  91/08/31  14:11:51  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:44:56  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7.5.2  91/03/05  13:39:54  lwa
 * 	If in the kernel, define SHMLBA as PAGE_SIZE.
 * 	[91/02/28  11:10:03  lwa]
 * 
 * Revision 1.7  90/10/07  14:53:52  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:44:42  gm]
 * 
 * Revision 1.6  90/08/09  13:29:03  devrcs
 * 	Removed  extern u_int shm_mark;
 * 	[90/07/31  10:48:54  bet]
 * 
 * Revision 1.5  90/07/27  09:07:48  devrcs
 * 	Changed SHMLBA definition to getpagesize().
 * 	Defined shminfo structure, #ifdef _KERNEL.
 * 	Added external declarations.
 * 	[90/07/23  12:52:38  bet]
 * 
 * Revision 1.4  90/06/22  20:53:57  devrcs
 * 	Conditionalize vm/vm_object.h upon _KERNEL.
 * 	[90/06/13  19:18:15  gm]
 * 
 * 	Changed type of shm_*pid fields to ushort, as per SVID.
 * 	[90/06/06  13:05:27  ers]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:46:17  seiden]
 * 
 * Revision 1.3  90/04/27  19:27:04  devrcs
 * 	Modified AIX header for SysV shared memory implementation.
 * 	[90/04/20  10:55:44  bet]
 * 
 * Revision 1.2  90/03/13  20:49:43  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/09  15:18:10  tom]
 * 
 * $EndLog$
 */
/* @(#)shm.h	1.14  com/inc/sys,3.1,8943 10/10/89 15:58:08 */
/*
 * COMPONENT_NAME: (SYSIPC) IPC Shared Memory Facility
 *
 * FUNCTIONS:
 *
 * ORIGINS: 27 3
 *
 * (C) COPYRIGHT International Business Machines Corp. 1987, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 */

#ifndef _SYS_SHM_H_
#define _SYS_SHM_H_

#include <standards.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <mach/machine/vm_types.h>
#ifdef _KERNEL
#ifndef OSF1_SERVER
#include <vm/vm_object.h>
#endif	/* OSF1_SERVER */
#endif

#ifdef _XOPEN_SOURCE

/*
 *	Implementation Constants.
 */

#ifndef _KERNEL
#define SHMLBA  getpagesize()
#else
#define SHMLBA  PAGE_SIZE
#endif /* _KERNEL */

/*
 *	Operation Flags.
 */

#define	SHM_RDONLY	010000	/* attach read-only (else read-write) */
#define	SHM_RND		020000	/* round attach address to SHMLBA */

/*
 *	Structure Definitions.
 */

/*
 *      There is a shared mem id data structure for each shared memory
 *      and mapped file segment in the system.
 */

struct shmid_ds {
	struct ipc_perm	shm_perm;	/* operation permission struct */
	int		shm_segsz;	/* size of segment in bytes */
	pid_t		shm_lpid;	/* pid of last shmop */
	pid_t		shm_cpid;	/* pid of creator */
	u_short		shm_nattch;	/* current # attached */
	time_t		shm_atime;	/* last shmat time */
	time_t		shm_dtime;	/* last shmdt time */
	time_t		shm_ctime;	/* last change time */
};

#ifdef _KERNEL
struct shmid_internal {
	struct shmid_ds	s;		/* shared mem id data structure */
	memory_object_t	shm_pager;	/* memory object for shared mem */
	boolean_t	shm_bumpref; 	/* TRUE if object->ref_count hasn't
					 * been incremented to prevent
					 * removal on last detach
					 */
#ifdef OSF1_SERVER
	mach_port_t	shm_obj_name;	/* memory object internal name */
	vm_address_t	shm_serv_addr;	/* region location in server address space */
#endif /* OSF1_SERVER */
#ifdef OSF1_ADFS
        node_t		shm_node;	/* node number of the vnode pager */
#endif
};
#endif /* _KERNEL */

#ifndef _KERNEL
#ifdef _NO_PROTO
extern caddr_t	shmat();
extern int	shmdt();
extern int	shmctl();
#else
extern caddr_t	shmat(int, caddr_t, int);
extern int 	shmdt(caddr_t);
extern int 	shmctl(int, int, ...);
#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE

/*
 *	ipc_perm Mode Definitions.
 */
#define	SHM_R		IPC_R	/* read permission */
#define	SHM_W		IPC_W	/* write permission */

#ifdef _KERNEL
struct	shminfo {
	int	shmmax,		/* max shared memory segment size */
		shmmin,		/* min shared memory segment size */
		shmmni,		/* number of shared memory identifiers */
		shmseg;		/* max attached shared memory segments per process */
};
#endif /* _KERNEL */

#endif /* _OSF_SOURCE */

extern struct shmid_internal shmem[];
extern struct shminfo shminfo;

#endif /* _SYS_SHM_H_ */




