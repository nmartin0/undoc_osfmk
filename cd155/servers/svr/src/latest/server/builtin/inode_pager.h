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
 * $Log: inode_pager.h,v $
 * Revision 1.5  1994/11/18  20:28:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:52:27  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:10:11  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:12:17  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:26:31  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:18:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:17:21  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1992/03/09  11:58:37  durriya
 * 	91/12/17  17:19:03  jose
 * 	Unmodified from 1.0.3
 * 	Rearrange pager_file and pager_stat so that the unsupported utility
 * 	that peeks at these through /dev/kmem will still work.
 * 	[91/08/24  20:31:52  jeffc]
 *
 * 	Add per-paging file statistics, add pager_file definitions so that
 * 	table(PGINFO) can access them.
 * 	[91/03/05  15:51:34  jeffc]
 *
 * Revision 2.2  91/08/31  13:25:30  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 1.6  90/10/07  13:21:36  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:05:04  gm]
 * 
 * Revision 1.5  90/09/23  15:44:15  devrcs
 * 	Added #define for amount of memory allocated to vnode_pager map.
 * 	[90/09/11  06:53:24  ers]
 * 
 * Revision 1.4  90/09/13  11:43:09  devrcs
 * 	Add exported structures.
 * 	[90/08/25  10:13:25  brezak]
 * 
 * 	Change mach_swapon to swapon.
 * 	[90/08/22  18:14:39  brezak]
 * 
 * Revision 1.3  90/03/27  13:16:23  gm
 * 	Purged non-XP code.
 * 	Changed forward function definitions to vnode_* from inode_*.
 * 	Added mach_swapon.
 * 	[90/02/06            collins]
 * 
 * Revision 1.2  90/01/02  20:00:53  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:28:28  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:04:44  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.8  89/03/09  19:34:55  rpd
 * 	More cleanup.
 * 
 * Revision 2.7  89/02/25  15:00:57  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.6  89/02/07  01:00:49  mwyoung
 * 	Relocated from vm/inode_pager.h
 * 
 * Revision 2.5  89/01/15  16:40:17  rpd
 * 	Use decl_simple_lock_data.
 * 	[89/01/15  15:28:44  rpd]
 * 
 * Revision 2.4  88/10/18  03:42:57  mwyoung
 * 	Added inode_uncache_try(), so that all caching knowledge is
 * 	buried in the inode_pager itself.
 * 	[88/09/18            mwyoung]
 * 
 * Revision 2.3  88/08/25  18:25:30  mwyoung
 * 	Corrected include file references.
 * 	[88/08/22            mwyoung]
 * 
 * 	Add declaration for inode_pager_release().
 * 	[88/08/11  18:52:57  mwyoung]
 * 
 * Revision 0.0  87/12/06            mwyoung
 * 	Removed inode_pager_t structure declaration from this file...
 * 	it should not be exported from the inode_pager implementation.
 * 	[87/12/06            mwyoung]
 * 
 * $EndLog$
 */

#ifndef	_BUILTIN_INODE_PAGER_H_
#define _BUILTIN_INODE_PAGER_H_

#include <uxkern/import_mach.h>

#define MS_NONE		0x0000
#define MS_PREFER	0x0001		/* Prefer this file. */
/*
 * Number of pages of vnode_pager address space allocated per
 * vnode_pager thread
 */
#define VNODE_PAGER_ALLOC	10

/*
 * Miscellaneous pager stats that can be returned per swap file.
 */
#define PNAMELEN 90
struct pager_stat {
	long		ps_npgs;	/* Size of file in pages.	     */
	long		ps_pfree;	/* Number of unused pages.	     */
	long		ps_lowat;	/* Low water page.		     */
	boolean_t	ps_prefer;	/* Is this a prefered paging file?   */
	char		ps_name[PNAMELEN];	/* Filename of this file.    */
	ulong_t		ps_pagein_count;/* Number of pageins		     */
	ulong_t		ps_pageout_count;/* Number of pageouts		     */
	ulong_t		ps_pagein_fail;	/* Number of pagein failures	     */
	ulong_t		ps_pageout_fail;/* Number of pageout failures	     */
	ulong_t		ps_pageinit_count;/* Number of page initializations  */
	ulong_t		ps_pageinit_write;/* Number of " " actually written  */
};

/*
 * Pager File Structure:  This structure is used to manage the pager files
 *	that back temporary memory objects.
 */

typedef struct pager_file {
	queue_chain_t	pf_chain;	/* link to other paging files.	     */
	struct	vnode	*pf_vp;		/* vnode of paging file.	     */
	u_int		pf_count;	/* Number of vstruct using this file */
	u_char		*pf_bmap; 	/* Map of used blocks.		     */
	long		pf_hint;	/* Hint of where to look next.	     */
	vm_offset_t	pf_hipage;	/* Highest page allocated only used  */
					/*    when lowat != 0.		     */
        struct pager_stat pf_stat;
	decl_simple_lock_data(,pf_stat_lock)	/* lock for statistics	     */
	lock_data_t	pf_lock;	/* Lock for alloc and dealloc.	     */
} *pager_file_t;

#define pf_npgs	pf_stat.ps_npgs
#define pf_pfree pf_stat.ps_pfree
#define pf_lowat pf_stat.ps_lowat
#define pf_name pf_stat.ps_name
#define pf_prefer pf_stat.ps_prefer
#define pf_pagein_count pf_stat.ps_pagein_count
#define pf_pageout_count pf_stat.ps_pageout_count
#define pf_pagein_fail pf_stat.ps_pagein_fail
#define pf_pageout_fail pf_stat.ps_pageout_fail
#define pf_pageinit_count pf_stat.ps_pageinit_count
#define pf_pageinit_write pf_stat.ps_pageinit_write

#define	PAGER_FILE_NULL		(pager_file_t) 0
#ifdef	_KERNEL
#include <mach_xp.h>

#include <mach/boolean.h>
#include <mach/memory_object.h>
#include <kern/queue.h>

extern void		vnode_pager_bootstrap();

extern memory_object_t	inode_pager_setup();
extern void		inode_pager_release();
extern void		vnode_uncache();
extern boolean_t	vnode_uncache_try();

#include <kern/lock.h>

decl_simple_lock_data(extern,inode_pager_init_lock)

extern void		vnode_pager();

extern lock_data_t	pager_file_lock;	/* Global lock on queues.   */
extern queue_head_t	pager_files;		/* Pager file queue.	    */
extern int		pager_file_count;	/* Number of active files.  */

#endif	/* _KERNEL */
#endif	/* _BUILTIN_INODE_PAGER_H_ */
