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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: mfs.h,v $
 * Revision 1.8  1994/11/18  20:32:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/15  18:05:02  brad
 * Added two fields to vm_info struct for PFS.  These fields are used by
 * system calls that require synchronization because some remote stripefile
 * operations are in progess.
 *
 * Revision 1.6  1993/07/14  18:01:48  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:16:49  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:06:07  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  22:57:51  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:00:07  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:56:55  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:23:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:10  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.10  1992/11/17  19:52:08  loverso
 * 	Added temporary and cacheable flags to vm_info struct.
 * 	[92/11/15            roy]
 *
 * Revision 2.9  92/09/20  11:27:00  roy
 * 	No more temporary/valid_size flags in vm_info struct.
 * 	[92/09/17            roy]
 * 
 * Revision 2.8  92/08/26  12:11:50  loverso
 * 	No more accessed/modified flags in vm_info struct.
 * 	[92/08/19            roy]
 * 
 * Revision 2.7  92/07/14  15:04:14  rabii
 * 	Added 'temporary' flag to vm_info struct for MAPPED_FILES.
 * 	[92/07/13            roy]
 * 
 * Revision 2.6  92/06/08  18:20:56  pjg
 * 	Added 'accessed' and 'must_clean' flags to vm_info struct 
 * 	for MAPPED_FILES. (roy)
 * 
 * Revision 2.5  92/05/18  12:32:23  roy
 * 	Revision 2.4.1.1  92/04/22  09:51:59  roy
 * 	Added mscount, wait_cnt to vm_info structure.  Removed pager_request.
 * 	[92/03/18            roy]
 * 
 * Revision 2.4  92/03/15  14:42:46  roy
 * 	92/03/03  16:52:08  roy
 * 	Got rid of modified_size flag.
 * 
 * Revision 2.3  91/12/13  10:11:11  roy
 * 	91/11/19  20:15:52  roy
 * 	Added VM_INFO lock macros and some fields to struct vm_info in
 * 	support of mapped files.
 * 
 * 	91/11/19  10:04:05  roy
 * 	Added initial MAPPED_FILES support.  Fixed this file to
 * 	accurately reflect reality.
 * 
 * Revision 2.2  91/08/31  13:37:54  rabii
 * 	Initial V2.0 Checkin
 * 
 * $EndLog$
 */
#ifndef	_KERN_MFS_H_
#define _KERN_MFS_H_

#include <mapped_files.h>

#include <uxkern/import_mach.h>
#include <mach/memory_object.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/zalloc.h>
#include <sys/types.h>
#if     MAPPED_FILES | PFS
#include <uxkern/mf.h>
#endif
#if	PFS
#include <sys/estat.h>
#endif

extern zone_t		vm_info_zone;

#define VM_INFO_NULL	((struct vm_info *) 0)


/*
 * Auxiliary mapping information associated with vnodes.
 */
struct vm_info {
	memory_object_t	pager;		/* memory object */
	mach_port_mscount_t mscount;	/* Number of send rights created     */
	vm_size_t	vnode_size;	/* vnode size (not reflected in vp) */
	struct ucred	*cred;		/* vnode credentials */
	int		error;		/* holds error codes */
#if	MAPPED_FILES | PFS
	struct mutex    vm_lock;     	/* lock protecting vm info */
	int		temporary:1,	/* is the file temporary? */
                        cacheable:1,	/* is the file cacheable? */
        		must_clean:1,	/* does the file need cleaning? */
	                ra_granted:1,   /* some client have read access? */
	                wa_granted:1,   /* some client have write access? */
                        acquiring:1;    /* someone acquiring the token? */
	int		wait_cnt;	/* num threads waiting to acquire */
	vm_offset_t	min_offset;     /* min and max VA's used for cleaning */
	vm_offset_t	max_offset;
        struct condition may_acquire;   /* signal to allow token acquisition */
        struct condition tok_released;  /* signal to wakeup an acquirer */
        token_info_t    *wtoken;        /* pointer to write token struct */
        queue_head_t    rtoken_list;    /* list of read token structs */
#endif	
#ifdef PFS
	struct mutex    pfs_lock;     	/* lock protecting pfs_deleting */
	boolean_t	pfs_deleting;	/* stripe data marked for deletion? */
	esize_t		pfs_length;	/* cached pfs file length */
#endif
};

#ifdef	PFS
#define PFS_LOCK(vp)		mutex_lock(&(vp)->v_vm_info->pfs_lock)
#define PFS_UNLOCK(vp)		mutex_unlock(&(vp)->v_vm_info->pfs_lock)
#endif

#endif	/* _KERN_MFS_H_ */
