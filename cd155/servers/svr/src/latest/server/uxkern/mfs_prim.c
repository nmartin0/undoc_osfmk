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
 * $Log: mfs_prim.c,v $
 * Revision 1.10  1994/11/18  20:48:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/03/03  18:10:26  brad
 * Merged revision 1.8.4.1 from the R1.2 branch into R1.3.
 *
 * Revision 1.8.4.1  1994/03/03  01:17:13  brad
 * Added ASSERT in support of bug fix for PTS 6779.
 *
 *  Reviewer: Bob Godley
 *  Risk: Low
 *  Benefit or PTS #: 6779
 *  Testing: Ran PTS test case on 8, 16, and 64 nodes multiple times with
 *             no failures.  Ran PFS EATs on 64 nodes.
 *  Module(s): server/vfs/vfs_syscalls.c, server/uxkern/mfs_prim.c
 *
 * Revision 1.8  1993/07/15  18:07:20  brad
 * Added initialization of new PFS fields in vm_info struct.  These fields
 * are used by system calls that require synchronization because some remote
 * stripefile operations are in progess.
 *
 * Revision 1.7  1993/07/14  18:43:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:04:17  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/06/21  22:51:28  dbm
 * Added PFS length initialization to vm_info_reinit().
 *
 * Revision 1.5  1993/05/06  19:31:06  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:12:21  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.3  1993/03/11  00:55:17  dbm
 * Changed PFS lengths to use VNOVAL instead of -1 for uninitialized file
 * length values.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  22:53:16  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:05:35  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:06:35  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:55:21  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.11  92/11/17  19:53:36  loverso
 * 	Added temporary and cacheable flags to vm_info struct.
 * 	[92/11/15            roy]
 * 
 * Revision 2.11  1992/11/17  19:53:36  loverso
 * 	Added temporary and cacheable flags to vm_info struct.
 * 	[92/11/15            roy]
 *
 * Revision 2.10  92/09/20  11:24:53  roy
 * 	No more temporary/valid_size flags in vm_info struct.
 * 	[92/09/17            roy]
 * 
 * Revision 2.9  92/08/26  12:14:12  loverso
 * 	No more accessed/modified flags in vm_info struct.
 * 	[92/08/19            roy]
 * 
 * Revision 2.8  92/07/14  14:52:00  rabii
 * 	Initialize 'must_clean' and 'temporary' flags.
 * 	[92/07/13            roy]
 * 
 * Revision 2.7  92/05/18  12:27:55  roy
 * 	Revision 2.6.1.2  92/05/08  12:08:31  roy
 * 	Added vm_info_reinit() and vm_info_check().
 * 	[92/05/01            roy]
 * 
 * 	Revision 2.6.1.1  92/04/22  10:05:18  roy
 * 	Initialize more fields for mapped_files.
 * 	[92/03/31            roy]
 * 
 * Revision 2.6  92/03/15  14:30:15  roy
 * 	92/03/03  16:51:14  roy
 * 	Initialize min_offset to INT_MAX.
 * 
 * Revision 2.5  91/12/13  10:23:07  roy
 * 	91/11/19  10:09:01  roy
 * 	This file now only contains mfs_init() and vm_info_init().
 * 
 * 	91/11/15  11:41:44  roy
 * 	Overhaul by upgrading to 1.0.2.
 * 
 * 
 * $EndLog$
 */

#include <mapped_files.h>
#include <sys/kernel.h>
#include <kern/mfs.h>
#include <kern/zalloc.h>
#include <sys/types.h>
#include <sys/vnode.h>
#include <sys/resource.h>
#ifdef PFS
#include <sys/estat.h>
#endif

zone_t 		vm_info_zone;
boolean_t	mfs_initialized = FALSE;

/*
 *	mfs_init:
 *
 *	Initialize the mfs module.
 */
mfs_init()
{
	int			i;

	if (mfs_initialized) return;
	mfs_initialized = TRUE;

	i = (vm_size_t) sizeof (struct vm_info);
	vm_info_zone = zinit (i, 10000*i, 8192, "vm_info zone");
}

/*
 *	vm_info_init:
 *
 *	Initialize a vm_info structure for an vnode.
 */
vm_info_init(vp)
	struct vnode *vp;
{
	register struct vm_info	*vmp;

	/* XXX should have system startup call mfs_init() instead */
	if (!mfs_initialized) mfs_init();

	vmp = vp->v_vm_info;
	if (vmp == VM_INFO_NULL)
		vmp = (struct vm_info *) zalloc(vm_info_zone);
	vmp->cred = (struct ucred *) NULL;
	vmp->error = 0;
	vmp->vnode_size = (vm_size_t) 0;
#if	MAPPED_FILES | PFS
	vmp->temporary = 0;
	vmp->cacheable = 1;
	vmp->must_clean = 0;
	vmp->ra_granted = vmp->wa_granted = 0;
        vmp->acquiring = 0;
	vmp->wait_cnt = 0;
	vmp->min_offset = (vm_offset_t) INT_MAX;
	vmp->max_offset = (vm_offset_t) 0;
	mutex_init(&vmp->vm_lock);
        condition_init(&vmp->may_acquire);
        condition_init(&vmp->tok_released);
        vmp->wtoken = NULL;
        queue_init(&vmp->rtoken_list);
#endif
#if 	PFS
	mutex_init(&vmp->pfs_lock);
	vmp->pfs_deleting = FALSE;
	vmp->pfs_length.shigh = VNOVAL;   	/* Initialize PFS length */
	vmp->pfs_length.slow = VNOVAL;		/* to a known state. 	 */
#endif
	vp->v_vm_info = vmp;
}

#if	MAPPED_FILES | PFS
/*
 *	vm_info_reinit:
 *
 *	Reinitialize a vm_info structure when it's being reused.
 */
vm_info_reinit(vp)
	struct vnode *vp;
{
	register struct vm_info	*vmp = vp->v_vm_info;
	
	ASSERT(!vmp->acquiring);
	ASSERT(!vmp->ra_granted);
	ASSERT(!vmp->wa_granted);
	ASSERT(vmp->wtoken == NULL);
	ASSERT(queue_empty(&vmp->rtoken_list));
	ASSERT(vmp->wait_cnt == 0);
	vmp->temporary = 0;
	vmp->cacheable = 1;
	vmp->must_clean = 0;
	vmp->min_offset = (vm_offset_t) INT_MAX;
	vmp->max_offset = (vm_offset_t) 0;
#if 	PFS
	ASSERT(vmp->pfs_lock.held == 0);
	vmp->pfs_deleting = FALSE;
	vmp->pfs_length.shigh = VNOVAL;   	/* Initialize PFS length */
	vmp->pfs_length.slow = VNOVAL;		/* to a known state. 	 */
#endif
}	

#if	MACH_ASSERT
vm_info_check(vp)
	struct vnode *vp;
{
	register struct vm_info	*vmp = vp->v_vm_info;
	
	ASSERT(!vmp->acquiring);
	ASSERT(!vmp->ra_granted);
	ASSERT(!vmp->wa_granted);
	ASSERT(vmp->wtoken == NULL);
	ASSERT(queue_empty(&vmp->rtoken_list));
	ASSERT(vmp->wait_cnt == 0);
}	
#endif	/* MACH_ASSERT */

#endif	/* MAPPED_FILES | PFS */

