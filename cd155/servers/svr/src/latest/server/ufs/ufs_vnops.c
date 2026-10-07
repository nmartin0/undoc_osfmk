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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: ufs_vnops.c,v $
 * Revision 1.31  1995/03/07  23:02:07  stans
 *  Make MAX_DEVICE_REQUEST a variable and settable via bootmagic
 *  'FS_MAX_DEVICE_REQUEST'; PFS device I/O support.
 *
 *  Reviewer:rlg
 *  Risk:low
 *  Benefit or PTS #:11397
 *  Testing:WW09 sats
 *  Module(s):
 * 	ufs/ufs_vnops.c
 * 	uxkern/boot_config.c
 *
 * Revision 1.30  1995/03/04  23:15:39  sean
 * PTS #:		9441  (multiple test cases cause server panic at bcopy()+70)
 * Description:	The code path for the ufs S_fsvr_write_at_offset, can sometimes
 * 		deallocate the write buffer twice.  In when that happens
 * 		another thread which allocates memory between the two
 * 		deallocation is vunerable to panic.
 * Reviewer(s):	Bob Y., John L.
 * Risk:		Med
 * Testing:	vsx eats (full suite of 12 hours!)
 * Fix:		vio_wip_done deallocates the buffer only if there was
 * 		not an error, or if there is no way to report the error
 * 		back to ux_server_loop().  If ux_server_loop() is
 * 		informed of an error, then deallocation occurs there.
 *  Module(s):	server/ufs/ufs_vnops.c
 * 		server/vfs/vfs_vio.c
 *
 * Revision 1.29  1994/11/23  22:13:53  raysx
 * panic: data_read: sec bounding on HiPPI node while testing
 * 	IPI-3 mounted as UFS
 *
 *  Reviewer: bolsen@locus.com
 * 	   jlitvin@ssd.intel.com
 *  Risk: Low
 *  Benefit or PTS #: 10650
 *  Module(s): server/ufs/ufs_vnops.c
 * 	    server/vfs/vfs_vnops.c
 *
 * -----------------------------------------------------------------------
 *
 * Revision 1.28  1994/11/18  20:46:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.27  1994/10/25  19:04:58  arlin
 *  fixed memory leak in ufs_datain()
 *
 *  Reviewer: Jerrie Coffman, Bob Godley
 *  Risk: low
 *  Benefit or PTS #: 11136
 *  Testing: IPI-3/PFS EATs
 *  Module(s): ufs_vnops.c
 *
 * Revision 1.26  1994/06/28  23:15:18  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.25  1994/02/17  16:58:12  brad
 * Merged revision 1.22.2.2 from the R1.2 branch.
 *
 * Revision 1.22.2.2  1994/02/16  04:19:57  brad
 * Fixed flawed implementation of disk block preallocation.  Only preallocate
 * full file system blocks for simplicity.  Handle i_resfrags field in
 * the inode correctly.  Several errors in ufs_prealloc() fixed.
 *  Reviewer: Bob Godley
 *  Risk: Med
 *  Benefit or PTS #: 6318
 *  Testing: Ran PTS test.  Ran ORNL climate modelling code from bug #7266
 *     and verified lsize working now.  Ran PFS EATs and fileio EATs on
 *     64 nodes.  unmounted and force-ran fsck many times to ensure file
 *     systems clean.
 *  Module(s): server/ufs/{ufs_alloc,ufs_bmap,ufs_inode,ufs_vnops}.c
 *             server/sys/buf.h
 *
 * Revision 1.24  1994/02/16  19:17:43  rlg
 * ufs_dataout() modified to set the ICHG and IUPD bits in the i_flag field
 * if the file has been modified.  The previous code only set these flags if
 * the file was extended.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  low
 *  Benefit or PTS #: 7906
 *  Testing:  fileio and pfs EATs; failing test case
 *  Module(s):  server/ufs_vnops.c {ufs_dataout()}
 *
 * Revision 1.23  1994/01/14  01:18:06  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	ufs/ufs_vnops.c, ufs/ufs_vfsops.c, ufs/ufs_lookup.c
 * 	ufs/ufs_inode.c, ufs/ufs_cache.c, ufs/ufs_alloc.c
 * 	ufs/mfs_vnops.c, ufs/mfs_vfsops.c
 *
 * Revision 1.22  1993/11/30  01:02:46  brad
 * Fixed problem with improper write conflict detection being done on Fast
 * Path writes that are not sector-aligned.  This problem was causing PFS
 * data corruption.
 *
 *  Reviewer: Dave Minturn, Paul Roy (OSF)
 *  Risk: Medium
 *  Benefit or PTS #: 7239
 *  Testing: Ran ProSolver test from David Scott that originally (but very
 *     infrequently) reproduced the problem.  Also ran an engineering test
 *     created to reproduce the problem ... both ran many times successfully,
 *     where before they had failed.  Also ran PFS EATs.
 *  Module(s): server/ufs/ufs_vnops.c, server/vfs/vfs_vio.c
 *
 * Revision 1.21  1993/10/08  01:21:39  cfj
 * Rework any lines where a cast was on the left side of the equals
 * sign so that the PGI 4.5 compiler could compile the module.
 *
 * Revision 1.20  1993/09/30  13:59:06  nandy
 * Fix from OSF for bug #6633 ( setuid/sgid bug).
 *
 * Revision 1.19  1993/09/29  00:15:24  dbm
 * Added fix for bug #6777, this was due to the i_writesize going to a
 * big number (0x80000000) and the code treating it as a negative instead
 * of positive number.  This was causing i_size to never get updated in
 * iupdsiz().
 *
 * Revision 1.18  1993/07/21  18:31:15  wunder
 * Modified ufs_prealloc to set i_writesize to match preallocated range.
 *
 * Revision 1.17  1993/07/14  18:38:51  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.16  1993/07/09  20:18:31  brad
 * Fixed Fast Path read coalescing bug in ufs_datain() ... fix is from
 * Paul Roy at OSF.  Also bumped Fast Path MAX_DEVICE_REQUEST up to 512K.
 *
 * Revision 1.1.1.4  1993/07/01  20:54:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.15  1993/07/03  00:51:18  wunder
 * Fixed bug 5602, ufs_prealloc was not setting i_truesize of inode
 * for mapped files.
 *
 * Revision 1.14  1993/06/23  16:36:40  wunder
 * Added fix to ufs_prealloc routine to clear ENOSPC error.
 *
 * Revision 1.13  1993/06/04  15:47:46  wunder
 * Added missing parameter to balloc_nbc call in ufs_prealloc.
 *
 * Revision 1.12  1993/05/27  03:09:31  wunder
 * Modified ufs_prealloc to perform actual disk block preallocation without
 * performing writes to file, instead using block reservation routines.
 *
 * Revision 1.11  1993/05/14  23:18:38  cfj
 * Use PUT_NODE_IN_SPECINFO macro to validate the node number to
 * specalloc().
 *
 * Revision 1.10  1993/05/06  20:31:13  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:50:04  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.33  1993/04/29  14:02:26  klh
 * 	Revision 2.37  93/04/13  18:09:50  roy
 * 		Added block coalescing logic to ufs_datain.
 * 		[93/04/13            roy]
 *
 * 	Revision 2.36  93/04/06  11:57:26  rabii
 * 		fix race in rename with respect to name caching
 *
 * 		Add the missing iput() in ufs_rename that caused incorrectly
 * 		referenced files.
 *
 * 		Vrele source vnode correctly after checkdir()
 *
 * 	Revision 2.35  93/03/30  16:10:50  roy
 * 		Fix bug in ufs_pagein() whereby the -1 returned from bmap()
 * 		could be lost.
 * 		[93/03/30            roy]
 *
 * 		No need to call itimes() from ufs_update because it'll be done
 * 		when the file is closed.  Can make this change because files
 * 		are now closed synchronously.
 * 		[93/03/26            roy]
 *
 * 		Implement ufs_datain/ufs_dataout and put them (and other code) under
 * 			the UFS_NBC ifdef.  ufs_read/ufs_write will call these routines
 * 			if the the file is VIO_IS_FASTPATH.
 * 		[93/03/08            roy]
 *
 * 	Revision 2.34  93/03/22  23:58:37  condict
 * 		Changed cthread_yield to thread_yield.  (See kern/sched_prim.c)
 *
 * 	Revision 2.33  93/02/26  11:11:52  rabii
 * 		Fix maknode to properly set i_node for old style mknods (rabii)
 *
 * 	Revision 2.32  93/01/11  14:37:45  mmp
 * 		In ufs_remove, call mf_temporary with wait==TRUE so that unlink
 * 		is synchronous.  (mmp)
 *
 * 	Revision 2.31  93/01/08  14:33:34  durriya
 * 		set b_dev in buf sttruct in ufs_strategy
 *
 * Revision 2.32  93/03/22  21:15:31  yazz
 * OSF lock changes.  Change cthread_yield() calls to thread_yield().
 * 
 * Revision 2.31  93/03/10  14:19:27  yazz
 * Synchronous close merge from Intel.
 * 
 * 	Revision 1.4.6.2  1993/02/24  19:44:51  cfj
 * 	DEV_TAB <0>0 bug fix from OSF.
 *
 * 	Revision 1.4.6.1  1993/02/16  20:38:38  cfj
 * 	Synchronous close from OSF.
 *
 * 	Revision 2.32  93/01/11  14:37:45  mmp
 * 	 In ufs_remove, call mf_temporary with wait==TRUE so that unlink
 * 	 is synchronous.  (mmp)
 * 
 * 	Revision 1.4  1992/12/02  19:09:53  brad
 * 	Fixed lsize() bug in Mapped File case.
 *
 * 	Revision 1.3  1992/11/30  22:51:36  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.3  1992/11/25  02:59:09  dbm
 * 	Added fix for new parameters in order to support mapped files with the
 * 	PFS LSEEK command.
 *
 * 	Revision 1.1.2.2  1992/11/06  20:33:06  dleslie
 * 	Merged bug drop from Locus November 3, 1992, with NX development
 *
 * 	Revision 1.1.2.1  1992/11/05  23:40:06  dleslie
 * 	Local changes for NX through noon, November 5, 1992.
 *
 * 	Revision 2.30  1992/10/22  15:42:16  dbm
 * 	Updated for PFS functionality.
 *
 * Revision 1.9  1993/04/03  03:10:52  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.8  1993/04/01  20:02:27  cfj
 * Merge with T9.
 *
 * Revision 1.4.6.4  1993/04/01  19:58:23  cfj
 * Fix a bug when reading data from a sparce file. (roy@osf.org)
 *
 * Revision 1.7  1993/03/24  19:53:37  cfj
 * Merge with T9.
 *
 * Revision 1.4.6.3  1993/03/24  19:48:28  cfj
 * Change calls to cthread_yield() to thread_yield().
 *
 * Revision 1.6  1993/02/24  19:48:51  cfj
 * DEV_TAB <0>0 bug fix from OSF.
 *
 * Revision 1.4.6.1  1993/02/16  20:38:38  cfj
 * Synchronous close from OSF.
 *
 * Revision 2.32  93/01/11  14:37:45  mmp
 * 	In ufs_remove, call mf_temporary with wait==TRUE so that unlink
 * 	is synchronous.  (mmp)
 * 
 * Revision 1.4  1992/12/02  19:09:53  brad
 * Fixed lsize() bug in Mapped File case.
 *
 * Revision 1.1.2.2.2.4  1993/03/10  05:24:46  brad
 * Fixed ufs_prealloc() so an actual is returned if not all requested
 * space can be allocated.
 *
 * Revision 1.1.2.2.2.3  1993/01/12  05:04:12  brad
 * Added MF_MAPPABLE() check to ufs_prealloc() to double-check mapped file case.
 *
 * Revision 1.1.2.2.2.2  1992/12/16  06:04:12  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.2.2.1  1992/11/25  23:14:40  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.3  1992/11/30  22:51:36  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/25  02:59:09  dbm
 * Added fix for new parameters in order to support mapped files with the
 * PFS LSEEK command.
 *
 * Revision 1.1.2.2  1992/11/06  20:33:06  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:40:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.30  1992/10/22  15:42:16  dbm
 * Updated for PFS functionality.
 *
 * Revision 2.45  94/02/03  11:02:08  dnoveck
 *      Changes for per-node buffer-cache block size:
 *           Do IO in units smaller than logical block size.
 *           Changes to balloc interface.
 *      Added coalescing to ufs_dataout.
 *
 *
 * Revision 2.42  93/10/20  15:31:33  dnoveck
 *      DEV_BSIZE elimination: Change use of DEV_BSIZE-based defines
 *      to their DISK_GRANULE-based corelates.  Change interface to
 *      {vio,data}_{read,write} to be in terms of disk granules.  Change
 *      secround{up,down} to mrecround{up,down}.
 *
 * Revision 2.39  93/05/18  14:41:52  loverso
 * 	Put correct node number in vnode. (cfj@ssd.intel.com)
 *
 * Revision 2.38  93/05/13  16:46:28  roy
 * 	Size handling modifications for mapped files:  ufs_update, 
 * 	ufs_getsize, ufs_pagein, ufs_pageout, ufs_getattr.
 * 	[93/05/05            roy]
 * 
 * Revision 2.37  93/04/13  18:09:50  roy
 * 	Added block coalescing logic to ufs_datain.
 * 	[93/04/13            roy]
 *
 * Revision 2.36  93/04/06  11:57:26  rabii
 * 	fix race in rename with respect to name caching
 * 
 * 	Add the missing iput() in ufs_rename that caused incorrectly 
 * 	referenced files.
 * 
 * 	Vrele source vnode correctly after checkdir()
 * 
 * Revision 2.35  93/03/30  16:10:50  roy
 * 	Fix bug in ufs_pagein() whereby the -1 returned from bmap()
 * 	could be lost.
 * 	[93/03/30            roy]
 * 
 * 	No need to call itimes() from ufs_update because it'll be done
 * 	when the file is closed.  Can make this change because files
 * 	are now closed synchronously.
 * 	[93/03/26            roy]
 * 
 * 	Implement ufs_datain/ufs_dataout and put them (and other code) under
 * 	the UFS_NBC ifdef.  ufs_read/ufs_write will call these routines
 * 	if the the file is VIO_IS_FASTPATH.
 * 	[93/03/08            roy]
 * 
 * Revision 2.34  93/03/22  23:58:37  condict
 * 	Changed cthread_yield to thread_yield.  (See kern/sched_prim.c)
 * 
 * Revision 2.33  93/02/26  11:11:52  rabii
 * 	Fix maknode to properly set i_node for old style mknods (rabii)
 * 
 * Revision 2.32  93/01/11  14:37:45  mmp
 * 	In ufs_remove, call mf_temporary with wait==TRUE so that unlink
 * 	is synchronous.  (mmp)
 * 
 * Revision 2.31  93/01/08  14:33:34  durriya
 * 	set b_dev in buf sttruct in ufs_strategy
 * 
 * Revision 2.30  92/11/02  16:08:58  mmp
 * 	In calc_data_write_buf(), bzero() the right part of the buffer.  (mmp)
 * 
 * Revision 2.29  92/10/05  12:08:31  rabii
 * 	Add paging arg to ufs_pageout.
 * 	[92/10/02            roy]
 * 
 * Revision 2.28  92/09/24  16:50:42  rabii
 * 	In ufs_pageout, fix '>' to '>=' in deciding whether to drop data.
 * 	[92/09/24            roy]
 * 
 * Revision 2.27  92/09/20  11:25:26  roy
 * 	Added ufs_setsize.  Call iinit_mf from maknode.  ufs_pageout now
 * 	trims back a request based on the true file size.
 * 	[92/09/15            roy]
 * 
 * Revision 2.26  92/09/11  09:28:17  rabii
 * 	Fix setting of iomode.  Add asserts to ufs_page_read/ufs_page_write.
 * 	[92/09/09            roy]
 * 
 * Revision 2.25  92/08/27  09:52:49  loverso
 * 	Don't print error message in ufs_pageout.
 * 	[92/08/26            roy]
 * 
 * Revision 2.24  92/08/26  12:12:32  loverso
 * 	Re-did updating of information from the mf module to the ufs.
 * 	mf_update is called which in turn calls a new vop: ufs_update.
 * 	Also, modified calling sequence to remote_getinfo for devices.
 * 	[92/08/18            roy]
 * 
 * 	Implement ufs_alloc for real this time.  Added UFS_NBC_DEBUG.
 * 	Call VIO_SETMODE from maknode to set the vnode's iomode.
 *	Checks for RLIMIT_FSIZE under MAPPED_FILES option are not done
 *	at this layer.  
 * 	[92/07/22            roy]
 * 
 * Revision 2.23  92/08/14  14:18:12  rabii
 * 	Properly initialize mod flags in ufs_getattr.
 * 
 * Revision 2.22  92/08/14  10:57:57  rabii
 * 	Modify ufs_getattr to handle access times of remote devices 
 * 	by contacting the node servicing the device if needed.
 * 
 * Revision 2.21  92/08/13  19:20:03  rabii
 * 	Set vp->v_iomode in maknode().
 * 
 * 	Added the new routine ufs_setinfo which will set the inode access
 * 	flags according to its parameters. (rabii)
 * 	[92/08/13            roy]
 * 
 * Revision 2.20  92/07/29  08:27:36  rabii
 * 	ufs_read and ufs_write now do the job of calling into the mapped
 * 	files module for mappable files.
 * 	[92/07/28            roy]
 * 
 * Revision 2.19  92/07/16  10:02:39  rabii
 * 	Temporary fix to ufs_alloc to not fill files with zeros (roy)
 * 
 * Revision 2.18  92/07/14  14:54:15  rabii
 * 	Modified ufs_pagein_nbc/ufs_pageout_nbc to use numsecs, secroundup, 
 * 	etc. macros.  Calling sequence to data_read/data_write changed.
 * 	Call mf_temporary() from ufs_unlink and ufs_rename.
 * 	[92/07/10            roy]
 * 
 * Revision 2.17  92/06/08  18:23:43  pjg
 * 	Call mf_get_info from ufs_close and ufs_getattr for MAPPED_FILES. (roy)
 * 
 * Revision 2.16  92/05/31  18:59:19  loverso
 * 	Implemented ufs_alloc.  Removed ufs_setsize.
 * 	[92/05/27            roy]
 * 
 * Revision 2.15  92/05/18  12:30:38  roy
 * 	Revision 2.11.1.2  92/05/08  12:15:32  roy
 * 	ufs_fsync doesn't call vflushbuf for MAPPED_FILES.
 * 	[92/04/28            roy]
 * 
 * 	Revision 2.11.1.1  92/04/22  09:54:56  roy
 * 	ufs_pagein_nbc returns EINVAL when reading from a hole.
 * 	[92/03/29            roy]
 * 
 * 	Comment out psignal for OSF1_ADFS (temporary).
 * 	[92/03/17            roy]
 * 
 * Revision 2.14  92/05/12  00:07:23  loverso
 * 	Undid the previous change. namei() is called without HASPATHBUF by
 * 	ufs_rename() (the pathnames are guaranteed to be local), but 
 * 	rename() makes sure any pathname buffer is deallocated before 
 * 	calling VOP_RENAME (pjg).
 * 
 * Revision 2.13  92/05/01  10:23:06  rabii
 * 	Set option HASPATHBUF in ni_nameiop and initialize the other
 * 	fields of ndp in calls to namei (ADFS only) (pjg).
 * 
 * Revision 2.12  92/03/20  11:50:56  pjg
 * 	Comment out psignal for OSF1_ADFS (temporary).
 * 	[92/03/17            roy]
 * 
 * Revision 2.11  92/03/15  14:41:00  roy
 * 	92/03/09  10:23:30  roy
 * 	Added ufs_pagein, ufs_pageout, ufs_setsize, ufs_alloc for MAPPED_FILES.
 * 
 * 	92/03/03  17:03:46  roy
 * 	Changes for MAPPED_FILES and UFS_NBC.
 * 
 * 	92/02/21  17:50:02  roy
 * 	Bug fixes in ufs_getattr and ufs_page_write_nbc.
 * 
 * 	92/02/19  10:46:20  roy
 * 	In ufs_page_write_nbc, handle case of frag extension in place.
 * 
 * Revision 2.10  92/03/09  12:51:03  durriya
 * 	Remove some OSF1_SERVER ifdefs. (jose)
 * 
 * Revision 2.9  92/02/21  16:41:17  durriya
 * 	move GET(PUT)NODE_FROM_INODE to ufs/inode.h (srl)
 * 
 * Revision 2.8  92/01/16  16:16:30  roy
 * 	define and use PUTNODE_IN_INODE & GETNODE_FROM_INODE to add & extract 
 * 	the node number to/from the inode
 * 
 * Revision 2.7  92/01/05  19:19:15  roy
 * 	91/11/12  19:42:53  noemi
 * 	Changes to use va_node field in vattr structure and changes to 
 * 	parameters in call to specalloc.
 * 
 * Revision 2.6  91/12/17  08:45:45  roy
 * 	91/10/23  16:38:38  condict
 * 	Remove unnecessary get_time calls.  The global time var now works 
 * 	correctly.
 * 
 * Revision 2.5  91/12/13  10:16:31  roy
 * 	91/12/04  16:42:17  roy
 * 	Added mapped files support.
 * 
 * 	91/09/23  10:15:17  roy
 * 	Added ufs_page_read_nbc/ufs_page_write_nbc.
 * 
 * Revision 2.4  91/11/26  13:36:30  rabii
 * 	Removed rmknod from tables and remove ufs_rmknod. Extend ufs_mknod
 * 	to do take a node argument.
 * 
 * Revision 2.3  91/11/25  11:28:18  rabii
 * 	Added rmknod to tables, also imlemented ufs_rmknod for remote devices
 * 
 * Revision 2.2  91/08/31  14:21:09  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/01  17:01:50  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.18.11.3  91/07/17  13:48:00  garyf
 * 	fix to ufs_link & secure filesystem
 * 	[91/07/17  13:22:05  garyf]
 * 
 * Revision 1.18.11.2  91/06/10  15:12:30  coren
 * 	Changed "appropriate privilege" for clearing of ISGID/ISUID on write from
 * 	"setprocident" to "owner+chmodsugid"
 * 	[91/06/07  16:58:05  coren]
 * 
 * 	Make clearing of ISGID & ISUID on write correct for both secured
 * 	and unsecured versions (bug 2126):
 * 	     don't clear bits if it's a directory;
 * 	     don't clear bits if user has appropriate privilege (or is suser);
 * 	     clear ISUID in all other cases.
 * 	[91/06/07  15:21:54  coren]
 * 
 * Revision 1.18.6.4  91/02/26  12:23:35  gmf
 * 	Fix from Berkeley.  If source file of ufs_rename is unlinked while
 * 	rename is in progress, the ni_dvp from the namei() call will
 * 	not be valid, and should not be referenced.
 * 	[91/02/04  13:49:05  gmf]
 * 
 * 	1.  Fixed problems in ufs_close and ufsspec_close, where
 * 	    a bad inode could be used (bug 1789).
 * 	2.  Fixed ufs_mkdir to properly clean up on EMLINK (#1788)
 * 	3.  Changed checks for LINK_MAX to use unsigned arithmetic
 * 	    to ensure correctness in case of wrap in ufs_rename.
 * 	[91/02/01  13:08:00  gmf]
 * 
 * Revision 1.18.6.3  91/02/21  12:29:15  seiden
 * 	Clear ISUID, ISGID, ISVTX only if record locking not being used.
 * 	[91/02/21  12:23:31  seiden]
 * 
 * Revision 1.18.6.2  91/02/01  10:46:13  gmf
 * 	Don't clear enforcement mode lock bits from ufs_setattr (called
 * 	from *truncate).
 * 	[90/12/05  16:59:22  gmf]
 * 
 * 	Don't clear ISGID bit if the file has bits set for
 * 	mandatory file locking (ISGID and not group execute).
 * 	[90/11/13  14:39:40  gmf]
 * 
 * Revision 1.18.3.4  91/02/01  10:41:40  gmf
 * 	Don't clear enforcement mode lock bits from ufs_setattr (called
 * 	from *truncate).
 * 	[90/12/05  16:59:22  gmf]
 * 
 * 	Don't clear ISGID bit if the file has bits set for
 * 	mandatory file locking (ISGID and not group execute).
 * 	[90/11/13  14:39:40  gmf]
 * 
 * Revision 1.18.3.3  91/01/22  14:13:56  morris
 * 	Change to allow up to LINK_MAX links (was LINK_MAX - 1).
 * 	[91/01/22  13:55:41  morris]
 * 
 * 	Added checks to ufs_mkdir, ufs_link, and ufs_rename to make sure
 * 	that the new link count does not exceed LINK_MAX.
 * 	[91/01/08  17:16:47  morris]
 * 
 * Revision 1.18  90/10/31  14:08:03  devrcs
 * 	Cause ufs_rename to call inode_uncache if
 * 	it needs to.
 * 	[90/10/25  13:50:45  gmf]
 * 
 * 	Fixes for the following rename bugs:
 * 	- Race between two threads renaming the same file.
 * 	- Race between ufs_rename and ufs_rmdir.
 * 	- Fix bad parent directory link count when renaming
 * 	  a directory to one that already exists in the
 * 	  same directory.
 * 	Also moved setting of ICHG flag in chown1 fix to
 * 	after all inode fields have been updated.
 * 	[90/10/24  14:49:59  nags]
 * 
 * 	setting of inode ICHG bit in chown1() moved to make it be
 * 	done for systems with security as well as without
 * 	[90/10/17  18:47:53  hosking]
 * 
 * 	Correct the correction to the EFBIG check; the old code was assuming
 * 	sequential access would hit the limit.  Also carefully reworked the
 * 	check to use unsigned arithmetic for (potentially large) file offsets.
 * 	[90/10/16  09:08:31  dlb]
 * 
 * 	Pass STRIPSLASH operation to namei from ufs_rename when
 * 	doing directory rename.
 * 	[90/10/13  12:48:02  gmf]
 * 
 * 	Correct the check for max file size. Only write up to the max file size
 * 	and return the resid. If we can't write anything then return EFBIG.
 * 	[90/10/11  09:45:34  sp]
 * 
 * 	Added ufs_seek to handle seeks on regular files, call spec_seek on
 * 	devices.
 * 	[90/10/08  17:12:18  collins]
 * 
 * 	Change BM lock to real lock for changing i_mode
 * 	in ufs_setattr.
 * 	[90/10/09  15:08:22  gmf]
 * 
 * Revision 1.17  90/10/07  14:59:46  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:53:58  gm]
 * 
 * 	Fix typo in previous submit.
 * 	[90/09/27  14:19:01  tmt]
 * 
 * 	Call iaccess to determine whether access and modification
 * 	times can be changed.
 * 	[90/09/27  14:04:25  morris]
 * 
 * 	Allow group members to update access times.  Also clear ISUID
 * 	and ISGID following a successful chown.
 * 	[90/09/26  14:51:12  morris]
 * 
 * 	Update to new BUF_LOCK protocols.
 * 	[90/09/25  19:04:06  jeffc]
 * 
 * Revision 1.16  90/09/23  16:01:07  devrcs
 * 	Changed ufs_setattr to clear ISUID and ISGID bits following
 * 	a successful truncation.
 * 	[90/09/12  21:55:29  morris]
 * 
 * 	Always bump the inode's generation number when its link count is
 * 	decremented to 0.
 * 	[90/09/12  15:25:52  noemi]
 * 
 * 	Eliminated init argument to page_write. Added pager and offset args.
 * 	Also eliminated ufs_page_exists.
 * 	[90/09/11  06:54:25  ers]
 * 
 * 	A call to quota_chown was not under #if QUOTA.
 * 	[90/09/09  14:53:04  nags]
 * 
 * 	Added quota comments.
 * 	[90/09/08  19:04:44  nags]
 * 
 * 	Add fifo_ioctl vector.
 * 	[90/09/05  17:35:55  tmt]
 * 
 * 	Quota sources based on 4.3BSD-Reno.
 * 	Modifications for OSF/1 quota locking.
 * 	[90/09/03  22:37:38  nags]
 * 
 * Revision 1.15  90/09/13  11:51:43  devrcs
 * 	Changed regular file select routine to be seltrue().
 * 	[90/08/28  17:04:37  coren]
 * 
 * Revision 1.14  90/08/24  12:29:33  devrcs
 * 	removed u.u_error references
 * 	[90/08/20  12:35:29  gmf]
 * 
 * 	Removed references to u.u_error from non-SecureWare code.
 * 	[90/08/17  17:48:47  nags]
 * 
 * 	changes related to MP support
 * 	[90/08/14  18:12:21  hosking]
 * 
 * Revision 1.13  90/08/09  13:29:47  devrcs
 * 	Don't update i_size in ufs_mkdir until after the write, and also
 * 	set the ICHG flag so it gets flushed to disk.
 * 	[90/08/02  13:06:58  gmf]
 * 
 * 	Do brelse if error in uiomove in ufs_write.
 * 	[90/07/27  09:46:43  nags]
 * 
 * 	Deal with error from uiomove in ufs_write;
 * 	Set i_size BEFORE writing directory to disk in ufs_mkdir.
 * 	[90/07/25  16:22:39  nags]
 * 
 * Revision 1.12  90/07/27  09:09:16  devrcs
 * 	Changed ufs_page_read and ufs_page_write to take a uio instead
 * 	of individual arguments.  Modified page_read to be able to copy
 * 	data directly into a physical page.
 * 	[90/07/23  14:40:50  ers]
 * 
 * 	VOP_OPEN changes for clones, fix directory updates
 * 	[90/07/20  17:08:08  nags]
 * 
 * 	Changed initialization order of directory inode in ufs_mkdir.
 * 	This prevents directories from being created with size 24.
 * 	[90/07/17  09:21:25  jeffc]
 * 
 * 	Prohibit hard links on directories.
 * 	[90/07/17  08:52:11  nags]
 * 
 * Revision 1.11  90/07/17  11:43:24  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  22:04:27  seiden]
 * 
 * 	More changes for gcc.
 * 	[90/07/07  22:37:05  gm]
 * 
 * 	Condensed relevant history, reverse chronology:
 * 	Removed paranoia assertion.			nags@encore.com
 * 	Removed use of CMUCS ifdef			nags@encore.com
 * 	Security fixes					seiden@osf.org
 * 	Removed an extra IN_WRITE_UNLOCK in ufs_rename	nags@encore.com
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Secureware: MAC, DAC, audit, least priviledge	seiden@osf.org
 * 	Fix up iftovt_tab and vttoif_tab.		gmf@osf.org
 * 	eof fix for readdir (add parm to ufs_readdir)	gmf@osf.org
 * 	Added ufs_islocked, mostly fixed ufs_abortop	gmf@osf.org
 * 	Fixed ufs_gettattr to uses va_size0, vasize1	noemi@osf.org
 * 	Support for FIFOs (named pipes)			ers@osf.org
 * 	Removed a couple of parameters from bmap	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Integrated Mach changes, fast symlink support.	gm@osf.org
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)ufs_vnops.c	7.27 (Berkeley) 1/13/90
 */

#include <ufs_nbc.h>
#include <ufs_nbc_debug.h>
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


/*
 * Global vfs data structures for ufs
 */

int	ufs_lookup(),
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
	ufs_page_write();
#ifdef	PFS
int	ufs_prealloc();
#endif
#ifdef	OSF1_ADFS
int	ufs_pagein(),
	ufs_pageout(),
	ufs_alloc(),
	ufs_update(),
	ufs_getsize(),
	ufs_datain(),
	ufs_dataout();
#endif

struct vnodeops ufs_vnodeops = {
	ufs_lookup,		/* lookup */
	ufs_create,		/* create */
	ufs_mknod,		/* mknod */
	ufs_open,		/* open */
	ufs_close,		/* close */
	ufs_access,		/* access */
	ufs_getattr,		/* getattr */
	ufs_setattr,		/* setattr */
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
#ifdef	PFS
	ufs_prealloc,		/* preallocate and set size */
#endif	PFS
#ifdef	OSF1_ADFS
	ufs_pagein,		/* pagein */
	ufs_pageout,		/* pageout */
	ufs_alloc,		/* alloc */
	ufs_update,		/* update */
	ufs_getsize,		/* getsize */
	ufs_datain,		/* datain */
	ufs_dataout,		/* dataout */
#endif
};

int	spec_lookup(),
	spec_open(),
	ufsspec_read(),
	ufsspec_write(),
	spec_strategy(),
	spec_bmap(),
	spec_ioctl(),
	spec_select(),
	spec_seek(),
	ufsspec_close(),
	ufsspec_reclaim(),
	spec_page_read(),
	spec_page_write(),
	spec_badop(),
	spec_nullop();

struct vnodeops spec_inodeops = {
	spec_lookup,		/* lookup */
	spec_badop,		/* create */
	spec_badop,		/* mknod */
	spec_open,		/* open */
	ufsspec_close,		/* close */
	ufs_access,		/* access */
	ufs_getattr,		/* getattr */
	ufs_setattr,		/* setattr */
	ufsspec_read,		/* read */
	ufsspec_write,		/* write */
	spec_ioctl,		/* ioctl */
	spec_select,		/* select */
	spec_badop,		/* mmap */
	spec_nullop,		/* fsync */
	spec_seek,		/* seek */
	spec_badop,		/* remove */
	spec_badop,		/* link */
	spec_badop,		/* rename */
	spec_badop,		/* mkdir */
	spec_badop,		/* rmdir */
	spec_badop,		/* symlink */
	spec_badop,		/* readdir */
	spec_badop,		/* readlink */
	spec_badop,		/* abortop */
	ufs_inactive,		/* inactive */
	ufsspec_reclaim,	/* reclaim */
	spec_bmap,		/* bmap */
	spec_strategy,		/* strategy */
	ufs_print,		/* print */
	spec_page_read,		/* page_read */
	spec_page_write,	/* page_write */
#ifdef	PFS
	spec_badop,		/* preallocate and set size */
#endif	PFS
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	ufs_update,		/* update */
	spec_badop,		/* getsize */
	spec_badop,		/* datain */
	spec_badop,		/* dataout */
#endif
};

int	fifo_open(),
	ufsfifo_close(),
	ufsfifo_read(),
	ufsfifo_write(),
	fifo_ioctl(),
	ufsfifo_getattr(),
	fifo_select();

struct vnodeops fifo_inodeops = {
	spec_lookup,		/* lookup */
	spec_badop,		/* create */
	spec_badop,		/* mknod */
	fifo_open,		/* open */
	ufsfifo_close,		/* close */
	ufs_access,		/* access */
	ufsfifo_getattr,	/* getattr */
	ufs_setattr,		/* setattr */
	ufsfifo_read,		/* read */
	ufsfifo_write,		/* write */
	fifo_ioctl,		/* ioctl */
	fifo_select,		/* select */
	spec_badop,		/* mmap */
	spec_nullop,		/* fsync */
	spec_seek,		/* seek */
	spec_badop,		/* remove */
	spec_badop,		/* link */
	spec_badop,		/* rename */
	spec_badop,		/* mkdir */
	spec_badop,		/* rmdir */
	spec_badop,		/* symlink */
	spec_badop,		/* readdir */
	spec_badop,		/* readlink */
	spec_badop,		/* abortop */
	ufs_inactive,		/* inactive */
	ufs_reclaim,		/* reclaim */
	spec_bmap,		/* bmap */
	spec_badop,		/* strategy */
	ufs_print,		/* print */
	spec_badop,		/* page_read */
	spec_badop,		/* page_write */
#ifdef  PFS
	spec_badop,		/* preallocate and set size */
#endif	PFS
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	spec_badop,		/* update */
	spec_badop,		/* getsize */
	spec_badop,		/* datain */
	spec_badop,		/* dataout */
#endif
};

enum vtype iftovt_tab[16] = {
	VNON, VFIFO, VCHR, VNON, VDIR, VNON, VBLK, VNON,
	VREG, VNON, VLNK, VNON, VSOCK, VNON, VNON, VBAD,
};
int	vttoif_tab[9] = {
	0, IFREG, IFDIR, IFBLK, IFCHR, IFLNK, IFSOCK, IFIFO, IFMT,
};


/*
 * Create a regular file
 */
ufs_create(ndp, vap)
	struct nameidata *ndp;
	struct vattr *vap;
{
	struct inode *ip;
	int error;

	if (error = maknode(vap, ndp, &ip))
		return (error);
	ndp->ni_vp = ITOV(ip);
	return (0);
}

/*
 * Mknod vnode call
 */
/* ARGSUSED */
ufs_mknod(ndp, vap, cred, node)
	struct nameidata *ndp;
	struct ucred *cred;
	register struct vattr *vap;
	int	node;
{
	struct inode *ip;
	int error;

	if (vap->va_rdev != VNOVAL) {
		if (vap->va_type != VBLK && vap->va_type != VCHR)
			return (EINVAL);
	}
	if (error = maknode(vap, ndp, &ip))
		return (error);
	iput(ip);
	return (0);
}

#ifdef	PFS
extern bipi_open();

ufs_chkipi(vp)
	struct vnode *vp;
{
	register struct inode *ip;
	dev_t	dev;

	BM(VN_LOCK(vp));
 	ip = VTOI(vp);
	IN_LOCK(ip);
	dev = ip->i_dev;
	IN_UNLOCK(ip);

	/*
	 * if we are using an IPI-3 device, allow only PFS activity on 
	 * an ufs mounted partition.
	 */
	if (bdevsw[major(dev)].d_open == bipi_open) {
		/*
		 * if we are not using the device for PFS exit with error
		 */
		if (!VIO_IS_FASTPATH(vp) && !VIO_IS_BUF(vp)) { 
			printf("Error: Cannot use ufs on IPI-3 device if not PFS.\n");
			BM(VN_UNLOCK(vp));
			return (EFSNOTSUPP);
		}
	}
	BM(VN_UNLOCK(vp));
	return(0);
}
#endif

/*
 * Open called.
 *
 * Nothing to do.
 */
/* ARGSUSED */
ufs_open(vpp, mode, cred)
	struct vnode **vpp;
	int mode;
	struct ucred *cred;
{
	int error = 0;

#ifdef	PFS
	error = ufs_chkipi((*vpp));
#endif
	return (error);

}

/*
 * Close called
 *
 * Update the times on the inode.
 */
/* ARGSUSED */
ufs_close(vp, fflag, cred)
	struct vnode *vp;
	int fflag;
	struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);

	BM(VN_LOCK(vp));
	if (vp->v_usecount > 1) {
		u_long	flag;

		BM(VN_UNLOCK(vp));
		IN_LOCK(ip);
		flag = ip->i_flag & IREADERROR;
		IN_UNLOCK(ip);
		/*
		 * If there was an error initializing the inode, we don't
		 * want to do anything.  We will follow this path
		 * because vclean will bump the reference count.
		 */
		if (!flag) {
			ITIMES(ip, &time, &time);
		}
	} else 
		BM(VN_UNLOCK(vp));

	return (0);
}

ufs_access(vp, mode, cred)
	struct vnode *vp;
	int mode;
	struct ucred *cred;
{
	struct inode *ip = VTOI(vp);
#if	QUOTA
	if (mode & VWRITE) {
		int error;
		switch (vp->v_type) {
		case VREG: case VDIR: case VLNK:
			if (error = getinoquota(ip))
				return (error);
		}
	}
#endif
	return (iaccess(ip, mode, cred));
}

/* ARGSUSED */
ufs_getattr(vp, vap, cred)
	struct vnode *vp;
	register struct vattr *vap;
	struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);
	enum vtype type;

	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
#if SEC_ARCH
	/*
	* Collect the file's labels for auditing
	*/
	audstub_levels(ip->i_tag);
#endif

#if	MAPPED_FILES
	/*
	 * Some attribute information (such as accessed/modified flags 
	 * and the true file size) may be cached in the mf module.
	 * Hence, cause it to be written back to the ufs.
	 */
	if (VIO_IS_MAPPED(vp))
		mf_update(vp);
#endif

#ifdef	OSF1_ADFS
	/*
	 * For a remote device where the inode is on this node but
	 * the device is being serviced by a remote node, we must
	 * make a call to find out the modification status.
	 */
	if (type == VCHR || type == VBLK){
		boolean_t	remote = FALSE;
		struct specinfo	*si;
		mach_port_t	rvp;

		BM(VN_LOCK(vp));
		si = vp->v_specinfo;
		BM(VN_UNLOCK(vp));
		BM(SI_LOCK(si));
		if (si->si_flag & SI_RMTDEV) {
			remote = TRUE;
			rvp = si->si_specport;
		}
		BM(SI_UNLOCK(si));
		if (remote == TRUE) 
			/*
			 * Cause cached state to be written back to the inode.
			 * In particular, we are interested in the accessed
			 * and modified information.
			 */
			remote_getinfo(vp, rvp);
	}
#endif

	ITIMES(ip, &time, &time);
	/*
	 * Copy from inode table
	 */
	IN_LOCK(ip);
	vap->va_fsid = ip->i_dev;
#if SEC_MAC
	if ((ip->i_type_flags & SEC_I_MLDCHILD) &&
	    !privileged(SEC_MULTILEVELDIR, 0))
	    vap->va_fileid = ip->i_parent;
	else
		vap->va_fileid = ip->i_number;
#else
	vap->va_fileid = ip->i_number;
#endif
	vap->va_mode = ip->i_mode & ~IFMT;
	vap->va_nlink = ip->i_nlink;
	vap->va_uid = ip->i_uid;
	vap->va_gid = ip->i_gid;
	vap->va_rdev = (dev_t)ip->i_rdev;
#ifdef	OSF1_ADFS
	vap->va_node = GETNODE_FROM_INODE(ip);
#endif
	vap->va_qsize.val[0] = ip->i_din.di_qsize.val[0];
	vap->va_qsize.val[1] = ip->i_din.di_qsize.val[1];
#if	MAPPED_FILES
	/*
	 * Only need to worry about 32 bits of size info because that's
	 * all that's really valid in the inode. 
	 */
	if (VIO_IS_MAPPED(vp))
		vap->va_size = ip->i_truesize;
#endif
	vap->va_atime.tv_sec = ip->i_atime;
	vap->va_atime.tv_usec = 0;
	vap->va_mtime.tv_sec = ip->i_mtime;
	vap->va_mtime.tv_usec = 0;
	vap->va_ctime.tv_sec = ip->i_ctime;
	vap->va_ctime.tv_usec = 0;
	vap->va_flags = ip->i_flags;
	vap->va_gen = ip->i_gen;
	/* this doesn't belong here */
	if (type == VBLK)
		vap->va_blocksize = BLKDEV_IOSIZE;
	else if (type == VCHR)
		vap->va_blocksize = MAXBSIZE;
	else
		vap->va_blocksize = ip->i_fs->fs_bsize;
	vap->va_bytes = dgtob(ip->i_blocks);
	IN_UNLOCK(ip);
	vap->va_bytes_rsv = -1;
	vap->va_type = type;
	return (0);
}

/*
 * Set attribute vnode op. called from several syscalls
 */
ufs_setattr(vp, vap, cred)
	register struct vnode *vp;
	register struct vattr *vap;
	register struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);
	int error = 0;
	uid_t iuid;

#if SEC_ARCH
	/*
	 * Collect the file's labels for auditing
  	 */
	audstub_levels(ip->i_tag);
#endif

	/*
	 * Check for unsetable attributes.
	 */
	if ((vap->va_type != VNON) || (vap->va_nlink != VNOVAL) ||
	    (vap->va_fsid != VNOVAL) || (vap->va_fileid != VNOVAL) ||
	    (vap->va_blocksize != VNOVAL) || (vap->va_rdev != VNOVAL) ||
#ifdef	OSF1_ADFS
	    (vap->va_node != VNOVAL) ||
#endif
	    ((int)vap->va_bytes != VNOVAL) || (vap->va_gen != VNOVAL)) {
		return (EINVAL);
	}
	/*
	 * Go through the fields and update iff not VNOVAL.
	 */
	if (vap->va_uid != (uid_t) VNOVAL || vap->va_gid != (gid_t) VNOVAL) {

#if SEC_BASE
		/* XXX Should do better locking here for audit accuracy */
	  	BM(IN_LOCK(ip));
		audstub_dac(0, ip->i_uid, ip->i_gid, ip->i_mode);
	  	BM(IN_UNLOCK(ip));
		error = chown1(ip, vap->va_uid, vap->va_gid, cred);
	  	BM(IN_LOCK(ip));
		audstub_dac(1, ip->i_uid, ip->i_gid, ip->i_mode);
	  	BM(IN_UNLOCK(ip));
		if (error)
			return (error);
#else
		if (error = chown1(ip, vap->va_uid, vap->va_gid, cred))
			return (error);
#endif
	}
	if (vap->va_size != VNOVAL) {
		u_short mask = ISUID;
		BM(VN_LOCK(vp));
		if (vp->v_type == VDIR) {
			BM(VN_UNLOCK(vp));
			return (EISDIR);
		}
		BM(VN_UNLOCK(vp));
		if (error = itrunc(ip, vap->va_size, 0)) /* XXX IO_SYNC? */
			return (error);
		IN_LOCK(ip);
		/*
		 * Don't clear enforcement mode lock bits, 
		 * indicated by setgid bit, but no group execute.
		 */
		if (!(ip->i_mode & ISGID) || (ip->i_mode & S_IXGRP))
			mask |= ISGID;
		ip->i_mode &= ~mask;
		IN_UNLOCK(ip);
	}
	BM(IN_LOCK(ip));
	iuid = ip->i_uid;
	BM(IN_UNLOCK(ip));
	if (vap->va_atime.tv_sec != VNOVAL || vap->va_mtime.tv_sec != VNOVAL) {
#if SEC_ARCH
		if (error = iaccess(ip, SP_SETATTRACC, cred))
			return error;
#endif
#if SEC_BASE
		if (cred->cr_uid != iuid && !privileged(SEC_OWNER, EPERM))
			return EPERM;
#else
		if (cred->cr_uid != iuid && 
		   (error = iaccess(ip, IWRITE, cred))) {
		        if (error == EACCES) error = EPERM;
			return(error);
	        }
#endif
		IN_LOCK(ip);
		if (vap->va_atime.tv_sec != VNOVAL)
			ip->i_flag |= IACC;
		if (vap->va_mtime.tv_sec != VNOVAL)
			ip->i_flag |= IUPD;
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
		if (error = iupdat(ip, &vap->va_atime, &vap->va_mtime, 1))
			return (error);
	}
	if (vap->va_mode != (u_short) VNOVAL) {
#if SEC_BASE
		/* XXX Should do better locking here for audit accuracy */
	  	BM(IN_LOCK(ip));
		audstub_dac(0, ip->i_uid, ip->i_gid, ip->i_mode);
	  	BM(IN_UNLOCK(ip));
		error = chmod1(ip, (int)vap->va_mode, cred);
	  	BM(IN_LOCK(ip));
		audstub_dac(1, ip->i_uid, ip->i_gid, ip->i_mode);
	  	BM(IN_UNLOCK(ip));
#else
		error = chmod1(ip, (int)vap->va_mode, cred);
#endif
	}
	if (vap->va_flags != VNOVAL) {
#if SEC_ARCH
		if (error = iaccess(ip, SP_SETATTRACC, cred))
			return error;
#endif
#if SEC_BASE
		if (cred->cr_uid != iuid && !privileged(SEC_OWNER, EPERM))
			return EPERM;
		IN_LOCK(ip);
		if (privileged(SEC_FILESYS, 0)) {
			ip->i_flags = vap->va_flags;
		} else {
													ip->i_flags &= 0xffff0000;
													ip->i_flags |= (vap->va_flags & 0xffff);
												}
#else /* SEC_BASE */
		if (cred->cr_uid != iuid &&
		    (error = suser(cred, &u.u_acflag))) 	/* XXX */
			return (error);
		IN_LOCK(ip);
		if (cred->cr_uid == 0) {
			ip->i_flags = vap->va_flags;
		} else {
			ip->i_flags &= 0xffff0000;
			ip->i_flags |= (vap->va_flags & 0xffff);
		}
#endif	/* SEC_BASE */
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
	}
	return (error);
}


/*
 * Change the mode on a file.
 */
chmod1(ip, mode, cred)
	register struct inode *ip;
	register int mode;
	struct ucred *cred;
{
	int error;
	struct vnode *vp = ITOV(ip);;
#if SEC_ARCH
	int ret;
	dac_t dac;
#endif
#if     MAPPED_FILES
        /*
         * If setting the suid or sgid mode bits, first synchronize
         * with any writes that have taken place.  Writes can cause
         * these bits to be cleared, but it is done lazily when a token
         * is released (via a call to ufs_update).  mf_update causes
         * tokens to be released, and hence guarantees that the lazy
         * clearing of these bits is actually performed prior to them
         * being set here.
         */
        if (((mode & ISUID) || (mode & ISGID)) && VIO_IS_MAPPED(vp))
                mf_update(vp);
#endif
	/*
	 * We need to synchronize with other threads doing chmods and
	 * chowns so we keep the inode locked for a while.
	 */
	IN_LOCK(ip);
#if SEC_BASE
	if (cred->cr_uid != ip->i_uid && !privileged(SEC_OWNER, EPERM) ||
	    !sec_mode_change_permitted(mode)) {
		IN_UNLOCK(ip);
		return u.u_error;
	}
#else
	if (cred->cr_uid != ip->i_uid && (error = suser(cred, &u.u_acflag))) {
		IN_UNLOCK(ip);
		return (error);
	}
#endif
#if SEC_ARCH
	if (error = iaccess(ip, SP_SETATTRACC, cred)) {
		IN_UNLOCK(ip);
		return error;
	}
#endif
	ip->i_mode &= ~07777;
#if	SEC_BASE
/* XXX -- the inode is locked!!! check it out */
	if ((ip->i_mode & IFMT) != IFDIR && (mode & ISVTX) &&
	    !privileged(SEC_LOCK, 0))
		mode &= ~ISVTX;
	if (!groupmember(ip->i_gid, cred) && (mode & ISGID) &&
	    !privileged(SEC_SETPROCIDENT, 0))
		mode &= ~ISGID;
	/*
	 * If we are setting the SUID bit of a file owned by root, and
	 * the file resides on a secure filesystem, and we have the
	 * supropagate privilege, add the sucompat privilege to the
	 * file's potential and granted privilege sets.
	 * MP note: inode lock depended upon to make ADDBIT atomic
	 */
	if ((mode & ISUID) && ip->i_uid == 0 && VSECURE(ITOV(ip)) &&
	    privileged(SEC_SUPROPAGATE, 0)) {
		ADDBIT(ip->i_ppriv, SEC_SUCOMPAT);
		ADDBIT(ip->i_gpriv, SEC_SUCOMPAT);
	}
#else /* SEC_BASE */
	if (cred->cr_uid) {
		if ((ip->i_mode & IFMT) != IFDIR)
			mode &= ~ISVTX;
		if (!groupmember(ip->i_gid, cred))
			mode &= ~ISGID;
	}
#endif /* SEC_BASE */
	ip->i_mode |= mode & 07777;
	ip->i_flag |= ICHG;
	IN_UNLOCK(ip);
#if	SEC_ARCH
	IN_LOCK(ip);
	dac.uid = ip->i_uid;
	dac.gid = ip->i_gid;
	dac.mode = ip->i_mode & 0777;
	IN_UNLOCK(ip);
	ret = SP_CHANGE_OBJECT(ip->i_tag, &dac, SEC_NEW_MODE);
	if (ret) {
		IN_LOCK(ip);
		if (ret & SEC_NEW_UID)
			ip->i_uid = dac.uid;
		if (ret & SEC_NEW_GID)
			ip->i_gid = dac.gid;
		if (ret & SEC_NEW_MODE)
			ip->i_mode = (ip->i_mode & ~0777) | (dac.mode & 0777);
		IN_UNLOCK(ip);
	}
#endif /* SEC_ARCH */
#if	MACH
	/* Mach VM system pays no attention to ISVTX bit. */
#else
	if ((vp->v_flag & VTEXT) && (ip->i_mode & ISVTX) == 0)
		xrele(vp);
#endif
	return (0);
}

/*
 * Perform chown operation on inode ip.
 *
 * Take the inode I/O lock for writing to
 * break chown1 races.  While ugly, doing so
 * considerably simplifies chown1, simplifies
 * other code (such as quotas) that depends
 * heavily on uids, and has little impact
 * on performance.  (To be honest, quotas
 * provided the strongest motivation; dealing
 * with all the possible chown1/chown1,
 * chown1/getinoquota, etc. races was just
 * too hairy.)
 *
 * The inode uid and gid are still altered under
 * inode incore lock, preserving locking assumptions
 * for other users of the inode uid and gid.
 */
chown1(ip, uid, gid, cred)
	register struct inode *ip;
	uid_t uid;
	gid_t gid;
	struct ucred *cred;
{
	int error;
#if SEC_ARCH
	int ret;
	dac_t dac;
#endif

	/*
	 * We need to synchronize with other threads racing to do
	 * chowns and chmods.  So we keep the inode locked for a while.
	 * We cheat by using the inode I/O lock for an extended period.
	 * We don't also need to take the incore lock when *examining*
	 * the inode's uid and gid because once the inode has been
	 * created this is the only code that modifies those fields.
	 * However, we must take the incore lock when *modifying* those
	 * fields, for the benefit of other code examining the fields
	 * without holding the inode I/O lock.
	 */
	IN_WRITE_LOCK(ip);
	if (uid == (uid_t) VNOVAL)
		uid = ip->i_uid;
	if (gid == (gid_t) VNOVAL)
		gid = ip->i_gid;
#if	SEC_BASE
	/*
	 * Must own the file or have OWNER privilege to change
	 * its owner or group.  Also, if we are changing the
	 * file's owner or are changing its gid to a group we
	 * don't belong to, must have CHOWN privilege.
	 *
	 * XXX -- inode locked!!!
	 */
	if (!sec_owner(ip->i_uid, ip->i_uid) ||
	    !sec_owner_change_permitted(ip->i_uid, ip->i_gid, uid, gid)) {
		IN_WRITE_UNLOCK(ip);
		 return u.u_error;
	}
#else
	/*
	 * If we don't own the file, are trying to change the owner
	 * of the file, or are not a member of the target group,
	 * the caller must be superuser or the call fails.
	 */
	if ((cred->cr_uid != ip->i_uid || uid != ip->i_uid ||
	    !groupmember((gid_t)gid, cred)) &&
	    (error = suser(cred, &u.u_acflag))) {
		IN_WRITE_UNLOCK(ip);
		return (error);
	}
#endif
#if	SEC_ARCH
	if (error = iaccess(ip, SP_SETATTRACC, cred)) {
		IN_WRITE_UNLOCK(ip);
		return error;
	}
#endif
#if	QUOTA
	if ((error = getinoquota(ip)) ||
	    (error = quota_chown(ip, uid, gid, 0, cred))) {
		IN_WRITE_UNLOCK(ip);
		return (error);
	}
#endif
	IN_LOCK(ip);
	ip->i_uid = uid;
	ip->i_gid = gid;
#if SEC_BASE
	ip->i_mode &= ~ISUID;
	ip->i_mode &= ~ISGID;
#else /* SEC_BASE */
	if (cred->cr_uid != 0) {
		ip->i_mode &= ~ISUID;
		ip->i_mode &= ~ISGID;
	}
#endif /* SEC_BASE */
	ip->i_flag |= ICHG;
#if SEC_ARCH
	/* XXX inode locking !!! */
	/*
	 * If the user chowns the security policy daemon, and
	 * the daemon must fault a page from its binary, deadlock
	 * results on the daemon's inode's I/O lock.
	 *
	 * If the policy daemon indicates that the inode receives
	 * a new uid or gid, we blindly force the quota system to
	 * give the file's blocks to the new uid/gid.  Doing otherwise
	 * allows the quota system to reject our change, which means
	 * informing the policy daemon, which may then mean calling
	 * the quota system again.
	 */
	dac.uid = ip->i_uid;
	dac.gid = ip->i_gid;
	dac.mode = ip->i_mode & 0777;
	IN_UNLOCK(ip);
	ret = SP_CHANGE_OBJECT(ip->i_tag, &dac, SEC_NEW_UID|SEC_NEW_GID);
	if (ret) {
#if	QUOTA
		if (ret & (SEC_NEW_UID|SEC_NEW_GID))
			if (quota_chown(ip, dac.uid, dac.gid, FORCE, cred))
				panic("chown1:  security/quota botch");
#endif
		IN_LOCK(ip);
		if (ret & SEC_NEW_UID)
			ip->i_uid = dac.uid;
		if (ret & SEC_NEW_GID)
			ip->i_gid = dac.gid;
		if (ret & SEC_NEW_MODE)
			ip->i_mode = (ip->i_mode & ~0777) | (dac.mode & 0777);
	} else
		IN_LOCK(ip);		/* just for the IN_UNLOCK, ugh */
#endif /* SEC_ARCH */
	IN_UNLOCK(ip);
	IN_WRITE_UNLOCK(ip);
	return (0);
}

/*
 * Vnode op for reading.
 */
/* ARGSUSED */
ufs_read(vp, uio, ioflag, cred)
	struct vnode *vp;
	register struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct inode *ip;
	register struct fs *fs;
	struct buf *bp;
        daddr_t bn, ios, raios;
	int size, diff, error;
	long n, iosoff, type;
	u_long isize;

#if	MAPPED_FILES
	if (VIO_IS_MAPPED(vp))
		return(mf_read(vp, uio, ioflag, cred));
#endif
#if	UFS_NBC
	if (VIO_IS_FASTPATH(vp)) 
		return(ufs_datain(vp, uio, ioflag, cred));
#endif

	if (uio->uio_rw != UIO_READ)
		panic("ufs_read mode");
	if (uio->uio_resid == 0)
		return (0);
	if (uio->uio_offset < 0)
		return (EINVAL);

	error = 0;
	ip = VTOI(vp);

	IN_READ_LOCK(ip);
	IN_LOCK(ip);
	type = ip->i_mode & IFMT;
	if (type != IFDIR && type != IFREG && type != IFLNK)
		panic("ufs_read type");

	ip->i_flag |= IACC;
	fs = ip->i_fs;
	isize = ip->i_size;
	IN_UNLOCK(ip);
	do {
		ios = iosection(fs, uio->uio_offset);
		iosoff = iosecoff(fs, uio->uio_offset);
		n = MIN((unsigned)(ioseclen(fs) - iosoff), uio->uio_resid);
		diff = isize - uio->uio_offset;
		if (diff <= 0) {
			IN_READ_UNLOCK(ip);
			return (0);
		}
		if (diff < n)
			n = diff;
		size = iosecsize(fs, ip, ios);
		raios = ios + 1;
		VN_LOCK(vp);
 		if (vp->v_lastr + 1 == ios &&
		    iosectosize(fs, raios) < isize) {
			VN_UNLOCK(vp);
			error = breada(ITOV(ip), ios, size, raios,
				       iosecsize(fs, ip, raios), NOCRED, &bp);
		} else {
			VN_UNLOCK(vp);
			error = bread(ITOV(ip), ios, size, NOCRED, &bp);
		}
		LASSERT(BUF_LOCK_HOLDER(bp));
		ASSERT(bp->b_resid >= 0);
		VN_LOCK(vp);
		vp->v_lastr = ios;
		VN_UNLOCK(vp);
		n = MIN(n, size - bp->b_resid);
		if (error) {
			brelse(bp);
			IN_READ_UNLOCK(ip);
			return (error);
		}
		error = uiomove(bp->b_un.b_addr + iosoff, (int)n, uio);
		if (n + iosoff == ioseclen(fs) || uio->uio_offset == isize)
			bp->b_flags |= B_AGE;
		brelse(bp);
	} while (error == 0 && uio->uio_resid > 0 && n != 0);
	IN_READ_UNLOCK(ip);
	return (error);
}

/*
 * Vnode op for writing.
 */
ufs_write(vp, uio, ioflag, cred)
	register struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct inode *ip;
	register struct fs *fs;
	struct buf *bp;
        daddr_t ios, bn;
	u_long osize, isize;
	int n, iosoff, flags;
	unsigned efbig;
#if	MACH
	int size, resid, error;
#else
	int count, size, resid, error;
#endif
	enum vtype	type;

#if	MAPPED_FILES
	if (VIO_IS_MAPPED(vp)) 
		return(mf_write(vp, uio, ioflag, cred));
#endif
#if	UFS_NBC
	if (VIO_IS_FASTPATH(vp)) 
		return(ufs_dataout(vp, uio, ioflag, cred));
#endif
	
	if (uio->uio_rw != UIO_WRITE)
		panic("ufs_write mode");
	if (uio->uio_resid == 0)
		return (0);
	if (uio->uio_offset < 0)
		return (EINVAL);

	error = 0;	
	ip = VTOI(vp);	       

	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	IN_WRITE_LOCK(ip);
	BM(IN_LOCK(ip));
	isize = ip->i_size;
	BM(IN_UNLOCK(ip));
	switch (type) {
	case VREG:
		if (ioflag & IO_APPEND)
			uio->uio_offset = isize;
		/* fall through */
	case VLNK:
		break;

	case VDIR:
		if ((ioflag & IO_SYNC) == 0)
			panic("ufs_write nonsync dir write");
		break;

	default:
		panic("ufs_write type");
	}
	if (type == VREG) {
		register unsigned file_limit;

		file_limit =  u.u_rlimit[RLIMIT_FSIZE].rlim_cur;
		if (uio->uio_offset >= file_limit) {
			IN_WRITE_UNLOCK(ip);
#ifndef	OSF1_ADFS
			unix_master();
			psignal(u.u_procp, SIGXFSZ);
			unix_release();
#endif
			return (EFBIG);
		}
		efbig = uio->uio_offset + uio->uio_resid;
		if (efbig > file_limit) {
			efbig -= file_limit;
			uio->uio_resid -= efbig;
		}
		else
			efbig = 0;
	} else
		efbig = 0;
       
	resid = uio->uio_resid;
	osize = isize;
	fs = ip->i_fs;
	flags = 0;
	if (ioflag & IO_SYNC)
		flags = B_SYNC;
	do {
		ios = iosection(fs, uio->uio_offset);
		iosoff = iosecoff(fs, uio->uio_offset);
		n = MIN((unsigned)(ioseclen(fs) - iosoff), uio->uio_resid);
		if (n < ioseclen(fs))
			flags |= B_CLRBUF;
		else
			flags &= ~B_CLRBUF;
		if (error = balloc(ip, iosecblock(fs, ios), iosecnum(fs, ios),
				   (int)(iosoff + n), &bp, flags))
			break;
		LASSERT(BUF_LOCK_HOLDER(bp));
		bn = bp->b_blkno;
		if (uio->uio_offset + n > isize) {
			IN_LOCK(ip);
			ip->i_size = uio->uio_offset + n;
			IN_UNLOCK(ip);
		}
                size = iosecsize(fs, ip, ios);
#if	MACH 
		VN_LOCK(vp);
		if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL) {
			VN_UNLOCK(vp);
			inode_uncache(ITOV(ip));
		} else
			VN_UNLOCK(vp);
#else
		count = howmany(size, CLBYTES);
		for (i = 0; i < count; i++)
			munhash(ip->i_devvp, bn + i * CLBYTES / DEV_BSIZE);
#endif
		n = MIN(n, size - bp->b_resid);
		error = uiomove(bp->b_un.b_addr + iosoff, n, uio);
		if (error) {
			brelse(bp);
			break;
		}	
		if (ioflag & IO_SYNC)
			(void) bwrite(bp);
		else if (n + iosoff == ioseclen(fs)) {
			bp->b_flags |= B_AGE;
			bawrite(bp);
		} else
			bdwrite(bp, bp->b_vp);
		IN_LOCK(ip);

		ip->i_flag |= IUPD|ICHG;

		if ((ip->i_mode & IFMT) != IFDIR) {
			/*
			 * clear setuid & setgid bits on regular files
			 * (unless privileged)
			 */
#if SEC_BASE
			if (!privileged(SEC_OWNER, 0) || !privileged(SEC_CHMODSUGID,0)) {
#else
			if (cred->cr_uid != 0) {
#endif
				u_short mask = ISUID;
				/*
				 * Don't clear enforcement mode lock bits, 
				 * indicated by setgid bit, but no group execute.
				 */
				if (!(ip->i_mode & ISGID) || (ip->i_mode & S_IXGRP))
					mask |= ISGID;
				ip->i_mode &= ~mask;
			 }
#if SEC_PRIV
			if (!privileged(SEC_CHPRIV, 0)) {
				bzero(ip->i_gpriv, sizeof ip->i_gpriv);
				bzero(ip->i_ppriv, sizeof ip->i_ppriv);
			}
#endif
		}
		IN_UNLOCK(ip);
	} while (error == 0 && uio->uio_resid > 0 && n != 0);
	IN_WRITE_UNLOCK(ip);
	if (efbig > 0)
		uio->uio_resid += efbig;
	if (error && (ioflag & IO_UNIT)) {
		(void) itrunc(ip, osize, ioflag & IO_SYNC);
		uio->uio_offset -= resid - uio->uio_resid;
		uio->uio_resid = resid;
	}
	if (!error && (ioflag & IO_SYNC)) {
		error = iupdat(ip, &time, &time, 1);
	}
	return (error);
}

/* ARGSUSED */
ufs_ioctl(vp, com, data, fflag, cred)
	struct vnode *vp;
	int com;
	caddr_t data;
	int fflag;
	struct ucred *cred;
{

	return (ENOTTY);
}

/*
 * Mmap a file
 *
 * NB Currently unsupported.
 */
/* ARGSUSED */
ufs_mmap(vp, fflags, cred)
	struct vnode *vp;
	int fflags;
	struct ucred *cred;
{

	return (EINVAL);
}

/*
 * Synch an open file.
 */
/* ARGSUSED */
ufs_fsync(vp, fflags, cred, waitfor)
	struct vnode *vp;
	int fflags;
	struct ucred *cred;
	int waitfor;
{
	struct inode *ip = VTOI(vp);

	if (fflags&FWRITE) {
		IN_LOCK(ip);
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
	}
#ifdef	OSF1_ADFS
	/*
	 * Sync'ing files in OSF/1 AD adds a different twist because 
	 * dirty file data for mapped files must be written-back from 
	 * main memory before it's written to disk.  The proper place
	 * to initiate such write-backs is in this routine because it
	 * really is a file system-specific action.  Unfortunately, this
	 * is made difficult because:
	 * - ufs_fsync is often bypassed in favor of direct calls to
	 *   vflushbuf (see ufs_sync, mntflushbuf)
	 * - in one case we don't want this routine to initiate write-backs 
	 *   (i.e., when the vnode pager calls it) to avoid recursive deadlock.
	 *
	 * In addition, explicit checks of vp->v_dirtyblkhd in random code
	 * to determine if a file has dirty data is incorrect for mapped
	 * files, requiring yet more hacks (e.g., see ufs_sync).
	 *
	 * A correct, clean solution to this problem would involve the
	 * following:
	 * - encapsulate all file cleaning in the file system-dependent
	 *   routine, VOP_FSYNC
	 * - the file system-dependent VOP_SYNC routine calls VOP_FSYNC
	 *   for every file on the mount structure's vnode list.  Better
	 *   yet, the sync() code in vfs_sycalls.c should be able to do
	 *   this, relying on VOP_SYNC to only write out meta data, etc.
	 * - the special case code for mapped files in vflushbuf and fsync()
	 *   is removed
	 * - the VOP_FSYNC 'waitfor' arg is generalized to be a flags word,
	 *   thus allowing an additional flag specifying not to initiate
	 *   main memory cleaning.
	 *
	 * An additional note for stackable file system folks:  
	 *   The act of initiating main memory cleaning could be implemented
	 *   in a vnode stacked on top of the standard ufs vnode.  This
	 *   could be a clean solution to the recursive deadlock situation
	 *   mentioned above in that the vnode pager could "hook into"
	 *   the standard ufs vnode thus bypassing the vnode that initiates
	 *   write-backs.
	 */
#endif  

#if	MAPPED_FILES
	/*
	 * Cleaning vnode data for mappable files is done in the fsync() code.
	 */
	if (!VIO_IS_MAPPED(vp))
#endif
		vflushbuf(vp, waitfor == MNT_WAIT ? B_SYNC : 0);

	return (iupdat(ip, &time, &time, waitfor == MNT_WAIT));
}

/*
 * Seek on a file
 *
 * Negative offsets are invalid.
 */
/* ARGSUSED */
ufs_seek(vp, oldoff, newoff, cred)
	struct vnode *vp;
	off_t oldoff, newoff;
	struct ucred *cred;
{
	if ((int) newoff < 0)
		return(EINVAL);
	else
		return(0);
}

/*
 * ufs remove
 * Hard to avoid races here, especially
 * in unlinking directories.
 */
ufs_remove(ndp)
	struct nameidata *ndp;
{
	register struct inode *ip, *dp;
	register struct vnode *vp;
	int error;

	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	ASSERT(vp->v_type != VDIR);
	BM(VN_UNLOCK(vp));
	ip = VTOI(ndp->ni_vp);
	dp = VTOI(ndp->ni_dvp);
	if ((ip->i_mode&IFMT) == IFDIR)
		return(EISDIR);
	/*
	 * We must ensure that the vnode describing this file is
	 * inaccessible after we remove the file from the directory
	 * so we purge its name cache entry.  If the link count on
	 * the inode drops to 0, we must also prevent NFS clients
	 * from using almost stale file handles for this inode so
	 * we increment the generation number.  The inode will be
	 * inacessible from both UFS and NFS file systems when the
	 * link count is 0.
	 */
	IN_WRITE_LOCK(dp);
	cache_purge(ITOV(ip));
	error = dirremove(ndp);
	if (!error) {
		IN_LOCK(ip);
		if (--ip->i_nlink == 0) {
			ip->i_gen = get_nextgen();
			ip->i_flag |= ICHG;
			IN_UNLOCK(ip);
#if     MACH
#if	MAPPED_FILES
			if (VIO_IS_MAPPED(vp))
				/*
				 * Tell the mapped file module that the file
				 * is "temporary."  This means the file is 
				 * uncacheable in the VM system, dirty data
				 * won't be paged out when the VM object is
				 * deactivated, and sync() won't operate on
				 * this file.
				 */
				mf_temporary(vp, TRUE);
			else
#endif
			{
				BM(VN_LOCK(vp));
				if (vp->v_vm_info->pager!=MEMORY_OBJECT_NULL) {
					BM(VN_UNLOCK(vp));
					inode_uncache(vp);
				} else
					BM(VN_UNLOCK(vp));
			}
#endif 
		} else {
			ip->i_flag |= ICHG;
			IN_UNLOCK(ip);
		}
	}
	cache_purge(ITOV(ip));		/* Just in case... - XXX */
	IN_WRITE_UNLOCK(dp);
	iput(ip);
	iput(dp);
	return (error);
}

/*
 * link vnode call
 */
ufs_link(vp, ndp)
	register struct vnode *vp;
	register struct nameidata *ndp;
{
	register struct inode *ip = VTOI(vp);
	register struct inode *dp = VTOI(ndp->ni_dvp);
	int error;
	int decr_link = 0;

#if SEC_ARCH
	/* XXX inode locking !!! */
	if (SP_ACCESS(ip->i_tag, VTOI(ndp->ni_dvp)->i_tag, SP_LINKACC, NULL)) {
		error = u.u_error;
		goto out;
	}
#endif
	IN_LOCK(ip);
	if ((ushort_t) ip->i_nlink >= LINK_MAX) {
		IN_UNLOCK(ip);
		error = EMLINK;
		goto out;
	}
	ip->i_nlink++;
	ip->i_flag |= ICHG;
	IN_UNLOCK(ip);
	decr_link = 1;
	error = iupdat(ip, &time, &time, 1);
	if (!error) {
		IN_WRITE_LOCK(dp);
		error = direnter(ip, ndp);
		IN_WRITE_UNLOCK(dp);
	}
out:
	iput(dp);
	if (error && decr_link) {
		IN_LOCK(ip);
		if (--ip->i_nlink == 0)
			ip->i_gen = get_nextgen();
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
	}
	return (error);
}

/*
 * Rename system call.
 * 	rename("foo", "bar");
 * is essentially
 *	unlink("bar");
 *	link("foo", "bar");
 *	unlink("foo");
 * but ``atomically''.  Can't do full commit without saving state in the
 * inode on disk which isn't feasible at this time.  Best we can do is
 * always guarantee the target exists.
 *
 * Basic algorithm is:
 *
 * 1) Bump link count on source while we're linking it to the
 *    target.  This also ensure the inode won't be deleted out
 *    from underneath us while we work (it may be truncated by
 *    a concurrent `trunc' or `open' for creation).
 * 2) Link source to destination.  If destination already exists,
 *    delete it first.
 * 3) Unlink source reference to inode if still around. If a
 *    directory was moved and the parent of the destination
 *    is different from the source, patch the ".." entry in the
 *    directory.
 */
ufs_rename(fndp, tndp)
	register struct nameidata *fndp, *tndp;
{
	register struct inode *ip, *xp, *dp;
	struct dirtemplate dirbuf;
	int doingdirectory = 0, oldparent = 0, newparent = 0;
	int stripslash = 0;
	int error = 0;
	int dummy;

	dp = VTOI(fndp->ni_dvp);
	ip = VTOI(fndp->ni_vp);
#if SEC_ARCH
	/*
	 * Check link access between existing file and target directory.
	 * If target file already exists, check process's delete access.
	 */
	if (SP_ACCESS(ip->i_tag, VTOI(tndp->ni_dvp)->i_tag, SP_LINKACC, NULL)) {
		error = u.u_error;
	} else if (tndp->ni_vp)
		error = iaccess(VTOI(tndp->ni_vp), SP_DELETEACC, tndp->ni_cred);
	if (error)
		goto abort;
#endif /* SEC_ARCH */
#if SEC_BASE
	/*
	 * If we end up calling namei below, we don't want it to
	 * collect any audit data since both pathnames have already
	 * been audited when they were translated by the top level
	 * system call function.
	 */
	audstub_nopath();
#endif
	IN_WRITE_LOCK(dp);
	IN_LOCK(ip);
	if (ip->i_flag & IRENAME) {
		error = EINVAL;
		goto abort2;
	}
	ip->i_flag |= IRENAME;
	if ((ip->i_mode&IFMT) == IFDIR) {
		register struct dirent *d = &fndp->ni_dent;

		/*
		 * Avoid ".", "..", and aliases of "." for obvious reasons.
		 */
		if ((d->d_namlen == 1 && d->d_name[0] == '.') || dp == ip ||
#if SEC_MAC
		    /* Prevent renaming of mld subdirectories */
		    (ip->i_type_flags & SEC_I_MLDCHILD) ||
#endif
		    fndp->ni_isdotdot) {
			ip->i_flag &= ~IRENAME;
			error = EINVAL;
			goto abort2;
		}
		IN_UNLOCK(ip);
		oldparent = dp->i_number;
		doingdirectory++;
		stripslash = STRIPSLASH;
		cache_purge(ITOV(ip));
	} else
		IN_UNLOCK(ip);
	/*
	 * Check that the source has not been removed.  
	 */
	if (error = checkdir(fndp, DEL)) {
		IN_LOCK(ip);
		ip->i_flag &= ~IRENAME;
		goto abort2;
	}
	/*
	 * 1) Bump link count while we're moving stuff
	 *    around.  If we crash somewhere before
	 *    completing our work, the link count
	 *    may be wrong, but correctable.
	 */
	IN_LOCK(ip);
	ip->i_nlink++;
	ip->i_flag |= ICHG;
	IN_UNLOCK(ip);
	IN_WRITE_UNLOCK(dp);
	error = iupdat(ip, &time, &time, 1);
again:
	dp = VTOI(tndp->ni_dvp);
	xp = NULL;
	if (tndp->ni_vp)
		xp = VTOI(tndp->ni_vp);
	/*
	 * If ".." must be changed (ie the directory gets a new
	 * parent) then the source directory must not be in the
	 * directory heirarchy above the target, as this would
	 * orphan everything below the source directory. Also
	 * the user must have write permission in the source so
	 * as to be able to change "..". We must repeat the call
	 * to namei, as the parent directory is iput by the call
	 * to checkpath().
	 */
	if (oldparent != dp->i_number)
		newparent = dp->i_number;
	if (doingdirectory && newparent) {
		if (error = iaccess(ip, IWRITE, tndp->ni_cred))
			goto bad;
		tndp->ni_nameiop = RENAME | WANTPARENT | stripslash;
		do {
			dp = VTOI(tndp->ni_dvp);
			if (xp != NULL)
				iput(xp);
			if (error = checkpath(ip, dp, tndp->ni_cred))
				goto out;
			if (error = namei(tndp))
				goto out;
			xp = NULL;
			if (tndp->ni_vp)
				xp = VTOI(tndp->ni_vp);
		} while (dp != VTOI(tndp->ni_dvp));
	}
	IN_WRITE_LOCK(dp);
	if (xp == NULL) {
		/*
		 * If the target didn't exist, check that it hasn't
		 * been created.
		 */
		if (error = checkdir(tndp, ADD)) {
			if (error == EEXIST) {
				IN_WRITE_UNLOCK(dp);
				iput(dp);
				tndp->ni_nameiop = 
				    RENAME | WANTPARENT | stripslash;
				if (error = namei(tndp))
					goto out;
				goto again;
			} else
				goto bad2;
		}
	} else {
		/*
		 * Check that the target hasn't been removed.
		 */
		 if (error = checkdir(tndp, DEL)) {
			if (error == ENOENT) {
				iput(xp);
				xp = NULL;
			} else
				goto bad2;
		}
	}
	if (xp && doingdirectory)
		IN_READ_LOCK(xp);
	/*
	 * 2) If target doesn't exist, link the target
	 *    to the source and unlink the source.
	 *    Otherwise, rewrite the target directory
	 *    entry to reference the source inode and
	 *    expunge the original entry's existence.
	 */
	if (xp == NULL) {
#ifdef OSF1_ADFS
		if ((dp->i_dev != ip->i_dev) || 
                    (dp->i_devvp->v_devnode != ip->i_devvp->v_devnode))
#else
		if ((dp->i_dev != ip->i_dev)
#endif
			panic("rename: EXDEV");
		/*
		 * Account for ".." in new directory.
		 * When source and destination have the same
		 * parent we don't fool with the link count.
		 */
		if (doingdirectory && newparent) {
			IN_LOCK(dp);
			if ((ushort_t) dp->i_nlink >= LINK_MAX) {
			        IN_UNLOCK(dp);
			        error = EMLINK;
			        goto bad2;
			}
			dp->i_nlink++;
			dp->i_flag |= ICHG;
			IN_UNLOCK(dp);
			error = iupdat(dp, &time, &time, 1);
		}
		if (error = direnter(ip, tndp)) {
			if (doingdirectory && newparent) {
				IN_LOCK(dp);
				dp->i_nlink--;
				dp->i_flag |= ICHG;
				IN_UNLOCK(dp);
			}
			goto bad2;
		}
	} else {
		struct vnode *vp;

#ifdef OSF1_ADFS
		if (xp->i_dev != dp->i_dev || xp->i_dev != ip->i_dev ||
                    (xp->i_devvp->v_devnode != dp->i_devvp->v_devnode) ||
                    (xp->i_devvp->v_devnode != ip->i_devvp->v_devnode))
#else
		if (xp->i_dev != dp->i_dev || xp->i_dev != ip->i_dev)
#endif
			panic("rename: EXDEV");
		/*
		 * Short circuit rename(foo, foo).
		 */
		if (xp->i_number == ip->i_number)
			panic("rename: same file");
		/*
		 * If the parent directory is "sticky", then the user must
		 * own the parent directory, or the destination of the rename,
		 * otherwise the destination may not be changed (except by
		 * root). This implements append-only directories.
		 */
		BM(IN_LOCK(dp));
#if SEC_BASE
		if ((dp->i_mode & ISVTX) && 
		    tndp->ni_cred->cr_uid != dp->i_uid) {
			BM(IN_UNLOCK(dp));
			BM(IN_LOCK(xp));
			if (xp->i_uid != tndp->ni_cred->cr_uid) {
				BM(IN_UNLOCK(xp));
				if (!privileged(SEC_OWNER, EPERM)) {
					error = EPERM;
					goto bad3;
				}
			} else
				BM(IN_UNLOCK(xp));
		} else
			BM(IN_UNLOCK(dp));
#else
		if ((dp->i_mode & ISVTX) && tndp->ni_cred->cr_uid != 0 &&
		    tndp->ni_cred->cr_uid != dp->i_uid) {
			BM(IN_UNLOCK(dp));
			BM(IN_LOCK(xp));
			if (xp->i_uid != tndp->ni_cred->cr_uid) {
				BM(IN_UNLOCK(xp));
				error = EPERM;
				goto bad3;
			} else
				BM(IN_UNLOCK(xp));
		} else
			BM(IN_UNLOCK(dp));
#endif
		/*
		 * Target must be empty if a directory
		 * and have no links to it.
		 * Also, insure source and target are
		 * compatible (both directories, or both
		 * not directories).
		 */
		if ((xp->i_mode&IFMT) == IFDIR) {
			if (xp->i_nlink != 2 ||
			    !dirempty(xp, dp->i_number, tndp->ni_cred)) {
				error = ENOTEMPTY;
				goto bad3;
			} 
			if (!doingdirectory) {
				error = ENOTDIR;
				goto bad3;
			}
			cache_purge(ITOV(dp));
			cache_purge(ITOV(xp));
		} else if (doingdirectory) {
			error = EISDIR;
			goto bad3;
		}
		if (error = dirrewrite(dp, ip, tndp))
			goto bad3;
		/*
		 * Adjust the link count of the target to
		 * reflect the dirrewrite above.  If this
		 * is a directory, it is empty, so we can
		 * squash the inode and any space associated
		 * with it. We disallowed renaming over top
		 * of a directory with links to it above, as
		 * the remaining link would point to a directory
		 * without "." or ".." entries.
		 */
		IN_LOCK(xp);
		if (--xp->i_nlink == 0)
			cache_purge(ITOV(xp));
		if (doingdirectory) {
			if (--xp->i_nlink != 0)
				panic("rename: linked directory");
			xp->i_gen = get_nextgen();
			IN_UNLOCK(xp);
			/*
			* If the target directory is in the same
			* directory as the source directory,
			* decrement the link count on the parent
			* of the target directory.
			*/
			if (!newparent) {
				IN_LOCK(dp);
				dp->i_nlink--;
				dp->i_flag |= ICHG;
				IN_UNLOCK(dp);
			}
			IN_READ_UNLOCK(xp);
			error = itrunc(xp, (u_long)0, IO_SYNC);
			IN_LOCK(xp);
		} else if (xp->i_nlink == 0)
			xp->i_gen = get_nextgen();
		xp->i_flag |= ICHG;
#if     MACH
		/*
		 * Uncache any VM information associated with
		 * unlinked file (a la ufs_remove).
		 */
		if (xp->i_nlink == 0) {
			IN_UNLOCK(xp);
			vp = ITOV(xp);
#if	MAPPED_FILES
			if (VIO_IS_MAPPED(vp))
				/*
				 * Tell the mapped file module that the file
				 * is "temporary."  This means the file is 
				 * uncacheable in the VM system, dirty data
				 * won't be paged out when the VM object is
				 * deactivated, and sync() won't operate on
				 * this file.
				 */
				mf_temporary(vp, FALSE);
			else
#endif
			{
				BM(VN_LOCK(vp));
				if (vp->v_vm_info->pager!=MEMORY_OBJECT_NULL) {
					BM(VN_UNLOCK(vp));
					inode_uncache(vp);
				} else
					BM(VN_UNLOCK(vp));
			}
		} else
			IN_UNLOCK(xp);

#else  /* MACH */
		IN_UNLOCK(xp);
#endif 
		iput(xp);
		xp = NULL;
	}
	IN_WRITE_UNLOCK(VTOI(tndp->ni_dvp));

	/*
	 * 3) Unlink the source.
	 */
	iput(VTOI(fndp->ni_dvp));
	fndp->ni_nameiop = DELETE | WANTPARENT | stripslash;
	(void)namei(fndp);
	if (fndp->ni_vp != NULL) {
		xp = VTOI(fndp->ni_vp);
		dp = VTOI(fndp->ni_dvp);
	} else {
		/*
		 * If ni_vp is NULL, then source is already gone, and
		 * ni_dvp is undefined, and not referenced.
		 * Must fail check below (xp != ip) to clean up properly.
		 */
		ASSERT(ip != (struct inode *)NULL);
		ASSERT(error == 0);
		xp = NULL;
		dp = NULL;
	}
	/*
	 * Ensure that the directory entry still exists and has not
	 * changed while the new name has been entered. If the source is
	 * a file then the entry may have been unlinked or renamed. In
	 * either case there is no further work to be done. If the source
	 * is a directory then it cannot have been rmdir'ed; its link
	 * count of three would cause a rmdir to fail with ENOTEMPTY.
	 * The IRENAME flag ensures that it cannot be moved by another
	 * rename.
	 */
	if (xp != ip) {
		if (doingdirectory)
			panic("rename: lost dir entry");
	} else {
		/*
		 * If the source is a directory with a
		 * new parent, the link count of the old
		 * parent directory must be decremented
		 * and ".." set to point to the new parent.
		 */
		if (doingdirectory && newparent) {
			IN_LOCK(dp);
			dp->i_nlink--;
			dp->i_flag |= ICHG;
			IN_UNLOCK(dp);
			error = vn_rdwr(UIO_READ, ITOV(xp), (caddr_t)&dirbuf,
				sizeof (struct dirtemplate), (off_t)0,
				UIO_SYSSPACE, 0, tndp->ni_cred, (int *)0);
			if (error == 0) {
				if (dirbuf.dotdot_namlen != 2 ||
				    dirbuf.dotdot_name[0] != '.' ||
				    dirbuf.dotdot_name[1] != '.') {
					dirbad(xp, 12, "rename: mangled dir");
				} else {
					dirbuf.dotdot_ino = newparent;
					(void) vn_rdwr(UIO_WRITE, ITOV(xp),
					    (caddr_t)&dirbuf,
					    sizeof (struct dirtemplate),
					    (off_t)0, UIO_SYSSPACE,
					    IO_SYNC, tndp->ni_cred, (int *)0);
					cache_purge(ITOV(dp));
				}
			}
		}
		IN_WRITE_LOCK(dp);
		error = dirremove(fndp);
		IN_WRITE_UNLOCK(dp);
		IN_LOCK(xp);
		if (!error) {
			if (--xp->i_nlink == 0)
				xp->i_gen = get_nextgen();
			xp->i_flag = (xp->i_flag | ICHG) & ~IRENAME;
		} else
			xp->i_flag &= ~IRENAME;
		IN_UNLOCK(xp);
	}
	iput(VTOI(tndp->ni_dvp)); 	/* parent of target */
	if (dp)
		iput(dp);	/* parent of source */
	if (xp)
		iput(xp);	/* source */
	iput(ip);		/* source */
	return (error);

bad3:
	if (xp && doingdirectory)	/* target */
		IN_READ_UNLOCK(xp);
bad2:
	IN_WRITE_UNLOCK(VTOI(tndp->ni_dvp));
bad:
	if (xp)				/* target */
		iput(xp);
	iput(dp);			/* parent of target */
out:
	iput(VTOI(fndp->ni_dvp));	/* parent of source */
	IN_LOCK(ip);
	if (--ip->i_nlink == 0)
		ip->i_gen = get_nextgen();
	ip->i_flag = (ip->i_flag | ICHG) & ~IRENAME;
	IN_UNLOCK(ip);
	iput(ip);			/* source */
	return (error);
abort2:
	IN_UNLOCK(ip);
	IN_WRITE_UNLOCK(dp);
	iput(ip);
	fndp->ni_vp = NULLVP;   /* to prevent multiple vrele */
abort:
	VOP_ABORTOP(tndp, dummy);
	vrele(tndp->ni_dvp);
	if (tndp->ni_vp)
		vrele(tndp->ni_vp);
	VOP_ABORTOP(fndp, dummy);
	vrele(fndp->ni_dvp);
	/* could have been modified by checkdir() */
	if (fndp->ni_vp)
		vrele(fndp->ni_vp);
	return (error);
}

/*
 * A virgin directory (no blushing please).
 */
struct dirtemplate mastertemplate = {
	0, 12, 1, ".",
	0, DIRBLKSIZ - 12, 2, ".."
};

/*
 * Mkdir system call
 */
ufs_mkdir(ndp, vap)
	struct nameidata *ndp;
	struct vattr *vap;
{
	register struct inode *ip, *dp;
	struct inode *tip;
	struct vnode *dvp;
	struct dirtemplate dirtemplate;
	int error;
	int dmode;
	struct vnode *vp;

	dvp = ndp->ni_dvp;
	dp = VTOI(dvp);
#if SEC_MAC
	/*
	 * When making a mld subdirectory we are called with NULL vap.
	 */
	if (vap == NULL) {
		/*
		 * Ensure that the process dominates the mld.
		 */
		if (!mld_dominate(dp->i_tag)) {
			iput(dp);
			return EACCES;
		}
		BM(IN_LOCK(dp));
    		dmode = dp->i_mode & 07777;
		BM(IN_UNLOCK(dp));
	} else
#endif /* SEC_MAC */
	dmode = vap->va_mode&0777;
	dmode |= IFDIR;
	/*
	 * Must simulate part of maknode here
	 * in order to acquire the inode, but
	 * not have it entered in the parent
	 * directory.  The entry is made later
	 * after writing "." and ".." entries out.
	 */
	error = ialloc(dp, dirpref(dp->i_fs), dmode, &tip);
	if (error) {
		iput(dp);
		return (error);
	}
	ip = tip;
	IN_LOCK(ip);
	ip->i_uid = ndp->ni_cred->cr_uid;
	ip->i_gid = dp->i_gid;
#if SEC_MAC
	/*
	 * If making a mld subdirectory, inherit owner and group
	 * from the mld.
	 */
	if (vap == NULL) {
		ip->i_uid = dp->i_uid;
		ip->i_gid = dp->i_gid;
		ip->i_parent = dp->i_number;
		ip->i_type_flags = SEC_I_MLDCHILD;
	}
#endif /* SEC_MAC */
	IN_UNLOCK(ip);
#if	QUOTA
	/*
	 * No need to hold inode I/O write lock across
	 * this chkiq call because no one else knows
	 * about this inode.
	 */
	if ((error = getinoquota(ip)) ||
	    (error = chkiq(ip, 1, ndp->ni_cred, 0))) {
		ifree(ip, ip->i_number, dmode);
		iput(ip);
		iput(dp);
		return (error);
	}
#endif
	IN_LOCK(ip);
	ip->i_nlink = 2;
	ip->i_flag |= IACC|IUPD|ICHG;
	ip->i_mode = dmode;
	IN_UNLOCK(ip);
	vp = ITOV(ip);
	BM(VN_LOCK(vp));	/* Vnode should be unknown now anyway */
	vp->v_type = VDIR;	/* Rest init'd in iget() */
	BM(VN_UNLOCK(vp));

	error = iupdat(ip, &time, &time, 1);
	/*
	 * Bump link count in parent directory
	 * to reflect work done below.  Should
	 * be done before reference is created
	 * so reparation is possible if we crash.
	 */
	IN_LOCK(dp);
	if ((ushort_t) dp->i_nlink >= LINK_MAX) {
		IN_UNLOCK(dp);
		error = EMLINK;
		goto out;
	}
	dp->i_nlink++;
	dp->i_flag |= ICHG;
	IN_UNLOCK(dp);
	error = iupdat(dp, &time, &time, 1);

	/*
	 * Initialize directory with "."
	 * and ".." from static template.
	 */
	dirtemplate = mastertemplate;
	dirtemplate.dot_ino = ip->i_number;
	dirtemplate.dotdot_ino = dp->i_number;
	error = vn_rdwr(UIO_WRITE, ITOV(ip), (caddr_t)&dirtemplate,
		sizeof(dirtemplate), (off_t)0, UIO_SYSSPACE,
		IO_SYNC, ndp->ni_cred, (int *)0);
	if (error)
		goto out;

	if (DIRBLKSIZ > dp->i_fs->fs_fsize)
		panic("mkdir: blksize");     /* XXX - should grow w/balloc() */
	else {
		IN_WRITE_LOCK(ip);
		IN_LOCK(ip);
		ip->i_size = DIRBLKSIZ;
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
		IN_WRITE_UNLOCK(ip);
	}

	/*
	 * Directory all set up, now
	 * install the entry for it in
	 * the parent directory.
	 */
	IN_WRITE_LOCK(dp);
	error = direnter(ip, ndp);
	IN_WRITE_UNLOCK(dp);
out:
	if (error) {
		if (error != EMLINK) {
			IN_LOCK(dp);
			dp->i_nlink--;
			dp->i_flag |= ICHG;
			IN_UNLOCK(dp);
		}
		IN_LOCK(ip);
		ip->i_nlink = 0;
		ip->i_gen = get_nextgen();
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
		/*
		 * No need to do an explicit itrunc here,
		 * iput will do this for us because we set
		 * the link count to 0.
		 */
		iput(ip);
	} else
		ndp->ni_vp = ITOV(ip);
	iput(dp);
	return(error);
}

/*
 * Rmdir system call.
 */
ufs_rmdir(ndp)
	register struct nameidata *ndp;
{
	register struct inode *ip, *dp;
	int error = 0;

	ip = VTOI(ndp->ni_vp);
	dp = VTOI(ndp->ni_dvp);
	/*
	 * No rmdir "." please.
	 */
	if (dp == ip) {
		iput(dp);
		iput(ip);
		return (EINVAL);
	}
	/*
	 * We hold the directory read locked until we remove it
	 * in dirremove.  This prevents any files or directories
	 * from being created in the directory throughout this
	 * process.
	 * 
	 * We hold the parent directory write locked until after
	 * we remove the directory from it.  We need it to remove
	 * the directory from the parent.  We must take the write
	 * lock on the parent directory before we can take the read
	 * lock on the child directory.  This forces us to hold the
	 * write lock until after dirremove has done its work.
	 */
	IN_WRITE_LOCK(dp);
	IN_READ_LOCK(ip);
	/*
	 * Verify the directory is empty (and valid).
	 * (Rmdir ".." won't be valid since
	 *  ".." will contain a reference to
	 *  the current directory and thus be
	 *  non-empty.)
	 */
	if (ip->i_nlink != 2 || !dirempty(ip, dp->i_number, ndp->ni_cred)) {
		error = ENOTEMPTY;
		goto bad;
	}
	/*
	 * We remove the directory from the cache here because we
	 * don't want anyone to find it in the cache.  If dirremove
	 * fails, which it rarely does, the entry may have to be
	 * entered into the cache again later. 
	 */
	cache_purge(ITOV(ip));
	if (error = dirremove(ndp))
		goto bad;
	cache_purge(ITOV(dp));
	IN_LOCK(dp);
	dp->i_nlink--;
	dp->i_flag |= ICHG;
	IN_UNLOCK(dp);
	IN_WRITE_UNLOCK(dp);
	iput(dp);
	ndp->ni_dvp = NULL;
	/*
	 * Truncate inode.  The only stuff left
	 * in the directory is "." and "..".  The
	 * "." reference is inconsequential since
	 * we're squashing it.  The ".." reference
	 * has already been adjusted above.
	 * If we have a hard link to this directory,
	 * we only want to decrement by one.
	 */
	IN_LOCK(ip);
	ip->i_nlink -= 2;
	ASSERT(ip->i_nlink == 0);
	ip->i_gen = get_nextgen();
	IN_UNLOCK(ip);
	IN_READ_UNLOCK(ip);
	error = itrunc(ip, (u_long)0, IO_SYNC);
	cache_purge(ITOV(ip));		/* Just in case... - XXX */
	iput(ip);
	return (error);
bad:
	IN_READ_UNLOCK(ip);
	IN_WRITE_UNLOCK(dp);
	iput(dp);
	iput(ip);
	return (error);
}

/*
 * symlink -- make a symbolic link
 */
ufs_symlink(ndp, vap, target)
	struct nameidata *ndp;
	struct vattr *vap;
	char *target;
{
	struct inode *ip;
	int error;

	/*
	 * Since the new inode must be consistent when made visible,
	 * maknode does the work to write the symlink to disk.
	 */
	vap->va_symlink = target;
	if (error = maknode(vap, ndp, &ip))
		return (error);
	iput(ip);
	return (error);
}

/*
 * Vnode op for read and write
 */
ufs_readdir(vp, uio, cred, eofflagp)
	struct vnode *vp;
	register struct uio *uio;
	struct ucred *cred;
	int *eofflagp;
{
	int count, lost, error;
	register struct inode *ip = VTOI(vp);

	count = uio->uio_resid;
	count &= ~(DIRBLKSIZ - 1);
	lost = uio->uio_resid - count;
	if (count < DIRBLKSIZ || (uio->uio_offset & (DIRBLKSIZ -1)))
		return (EINVAL);
	uio->uio_resid = count;
	uio->uio_iov->iov_len = count;
	error = ufs_read(vp, uio, 0, cred);
	uio->uio_resid += lost;
	BM(IN_LOCK(ip));
	if ((ip->i_size - uio->uio_offset) <= 0)
		*eofflagp = 1;
	else
		*eofflagp = 0;
	BM(IN_UNLOCK(ip));
	return (error);
}

/*
 * Return target name of a symbolic link
 */
ufs_readlink(vp, uiop, cred)
	struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{

	register struct inode *ip = VTOI(vp);
	int error;
	u_long isize;

	/*
	 *	Encore fast symbolic link support
	 */
	BM(IN_LOCK(ip));
	if (ip->i_size > uiop->uio_resid)
	        return(ERANGE);
	if (ip->i_flags & IC_FASTLINK) {
		isize = ip->i_size;
		BM(IN_UNLOCK(ip));
		error = uiomove(ip->i_symlink, isize, uiop);
		IN_LOCK(ip);
		ip->i_flag |= IACC;
		IN_UNLOCK(ip);
		return(error);
	}
	BM(IN_UNLOCK(ip));
	return (ufs_read(vp, uiop, 0, cred));
}

/*
 * Ufs abort op, called after namei() when a CREATE/DELETE isn't actually
 * done. Iff ni_vp/ni_dvp not null and locked, unlock.
 */
ufs_abortop(ndp)
	register struct nameidata *ndp;
{
	return;
}

/*
 * Get access to bmap
 */
ufs_bmap(vp, bn, vpp, bnp)
	struct vnode *vp;
	daddr_t bn;
	struct vnode **vpp;
	daddr_t *bnp;
{
	struct inode *ip = VTOI(vp);

	if (vpp != NULL)
		*vpp = ip->i_devvp;
	if (bnp == NULL)
		return (0);
	return (bmap(ip, bn, bnp));
}

/*
 * Just call the device strategy routine
 */
ufs_strategy(bp)
	register struct buf *bp;
{
	register struct inode *ip = VTOI(bp->b_vp);
	struct vnode *vp;
	int error;

#if MACH_LDEBUG
	if (bp->b_flags & B_ASYNC) {
		LASSERT(BUF_IS_LOCKED(bp));
	} else {
		LASSERT(BUF_LOCK_HOLDER(bp));
	}
#endif
	if (bp->b_vp->v_type == VBLK || bp->b_vp->v_type == VCHR)
		panic("ufs_strategy: spec");
	if (bp->b_blkno == bp->b_lblkno) {
		if (error = bmap(ip, iosecblock(ip->i_fs, bp->b_lblkno),
				 &bp->b_blkno))
 		if ((long)bp->b_blkno == -1)
  			clrbuf(bp);
		else
			bp->b_blkno += iosecdisp(ip->i_fs, bp->b_lblkno);
  	}
 	if ((long)bp->b_blkno == -1) {
 		biodone(bp);
  		return (0);
 	}
	vp = ip->i_devvp;
	bp->b_dev = vp->v_rdev;
#ifdef OSF1_ADFS
        bp->b_devnode = vp->v_devnode;
#endif
	(*(vp->v_op->vn_strategy))(bp);
	return (0);
}

/*
 * Print out the contents of an inode.
 */
ufs_print(vp)
	struct vnode *vp;
{
	register struct inode *ip = VTOI(vp);

	printf("tag VT_UFS, ino %d, on dev %d, %d\n", ip->i_number,
		major(ip->i_dev), minor(ip->i_dev));
}

/*
 * Read wrapper for special devices.
 */
ufsspec_read(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{

	register struct inode *ip = VTOI(vp);;
	/*
	 * Set access flag.
	 */
	IN_LOCK(ip);
	ip->i_flag |= IACC;
	IN_UNLOCK(ip);
	return (spec_read(vp, uio, ioflag, cred));
}

/*
 * Write wrapper for special devices.
 */
ufsspec_write(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);

	/*
	 * Set update and change flags.
	 */
	IN_LOCK(ip);
	ip->i_flag |= IUPD|ICHG;
	IN_UNLOCK(ip);
	return (spec_write(vp, uio, ioflag, cred));
}

/*
 * Close wrapper for special devices.
 *
 * Update the times on the inode then do device close.
 */
ufsspec_close(vp, fflag, cred)
	struct vnode *vp;
	int fflag;
	struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);

	BM(VN_LOCK(vp));
	if (vp->v_usecount > 1) {
		u_long	flag;

		BM(VN_UNLOCK(vp));
		IN_LOCK(ip);
		flag = ip->i_flag & IREADERROR;
		IN_UNLOCK(ip);
		/*
		 * If there was an error initializing the inode, we don't
		 * want to do anything.  We will follow this path
		 * because vclean will bump the reference count.
		 */
		if (flag)
			return(0);
		ITIMES(ip, &time, &time);
	} else
		BM(VN_UNLOCK(vp));
	return (spec_close(vp, fflag, cred));
}

/*
 * Read wrapper for fifos.
 */
ufsfifo_read(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);
	/*
	 * Set access flag.
	 */
	IN_LOCK(ip);
	ip->i_flag |= IACC;
	IN_UNLOCK(ip);
	return (fifo_read(vp, uio, ioflag, cred));
}

/*
 * Write wrapper for fifos.
 */
ufsfifo_write(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);
	/*
	 * Set update and change flags.
	 */
	IN_LOCK(ip);
	ip->i_flag |= IUPD|ICHG;
	IN_UNLOCK(ip);
	return (fifo_write(vp, uio, ioflag, cred));
}

/*
 * Close wrapper for fifos.
 *
 * Update the times on the inode then do device close.
 */
ufsfifo_close(vp, fflag, cred)
	struct vnode *vp;
	int fflag;
	struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);

	VN_LOCK(vp);
	if (vp->v_usecount > 1) {
		VN_UNLOCK(vp);
		ITIMES(ip, &time, &time);
	} else
		VN_UNLOCK(vp);
	return (fifo_close(vp, fflag, cred));
}

/*
 * getattr wrapper for fifos.
 */
ufsfifo_getattr(vp, vap, cred)
	struct vnode *vp;
	register struct vattr *vap;
	struct ucred *cred;
{
	int error;

	/*
	 * Get most attributes from the inode, rest
	 * from the fifo.
	 */
	if (error = ufs_getattr(vp, vap, cred))
		return (error);
	return (fifo_getattr(vp, vap, cred));
}

/*
 * Make a new file.
 */
maknode(vap, ndp, ipp)
	register struct vattr *vap;
	register struct nameidata *ndp;
	struct inode **ipp;
{
	register struct inode *ip;
	struct inode *tip;
	register struct inode *pdir = VTOI(ndp->ni_dvp);
	ino_t ipref;
	register struct vnode *vp;
	enum vtype type;
	int mode;
	int error, updated = 0;

	*ipp = 0;
	mode = MAKEIMODE(vap->va_type, vap->va_mode);
	if ((mode & IFMT) == IFDIR)
		ipref = dirpref(pdir->i_fs);
	else
		ipref = pdir->i_number;
	error = ialloc(pdir, ipref, mode, &tip);
	if (error) {
		iput(pdir);
		return (error);
	}
	ip = tip;
	IN_LOCK(ip);
	ip->i_uid = ndp->ni_cred->cr_uid;
	ip->i_gid = pdir->i_gid;
#if	SEC_BASE
	/* XXX inode locked !!! */
	if ((ip->i_mode & ISGID) && !groupmember(ip->i_gid, ndp->ni_cred) &&
	    !privileged(SEC_SETPROCIDENT, 0))
#else
	if ((ip->i_mode & ISGID) && !groupmember(ip->i_gid, ndp->ni_cred) &&
	    suser(ndp->ni_cred, NULL))
#endif
		ip->i_mode &= ~ISGID;
	IN_UNLOCK(ip);
#if	QUOTA
	/*
	 * No need to hold inode I/O write lock across
	 * this chkiq call because no one else knows
	 * about this inode.
	 */
	if ((error = getinoquota(ip)) ||
	    (error = chkiq(ip, 1, ndp->ni_cred, 0))) {
		ifree(ip, ip->i_number, mode);
		iput(ip);
		iput(pdir);
		return (error);
	}
#endif
	IN_LOCK(ip);
	ip->i_flag |= IACC|IUPD|ICHG;
	if ((mode & IFMT) == 0)
		mode |= IFREG;
	ip->i_mode = mode;
	ip->i_nlink = 1;
	IN_UNLOCK(ip);
	vp = ITOV(ip);
	VN_LOCK(vp);
	vp->v_type = IFTOVT(mode);	/* Rest init'd in iget() */
	type = vp->v_type;
#ifdef	OSF1_ADFS
	/*
	 * For regular files, get the iomode from the mount structure.
	 * Otherwise, the default iomode is to use the buffer cache.
	 */
	vp->v_iomode = vp->v_type == VREG ? vp->v_mount->m_iomode : VIO_BUF;
#endif
	VN_UNLOCK(vp);

#if	UFS_NBC
	iinit_extra(ip);	/* init some extra fields in the inode */
#endif

	/*
	 * If the inode was recycled from another type, we need to
	 * reset its v_ops.  Start out assuming regular file; it will
	 * be changed as required below.
	 */
	/*
	 * Handle atomic special cases:
	 *	-- special files
	 *	-- symlinks
	 *	-- sockets
	 * Some of this could possibly be avoided by vgone'ing the
	 * newly created vnodes, so they get set up properly in iget,
	 * but there are some races involved.
	 */
	if (vap->va_rdev != VNOVAL) {
		ASSERT(type == VBLK || type == VCHR);
#ifdef	OSF1_ADFS
		if (error = specalloc(vp, vap->va_rdev, 
				PUT_NODE_IN_SPECINFO(vap->va_node)))
#else
		if (error = specalloc(vp, vap->va_rdev))
#endif
		{
			VN_LOCK(vp);
			vp->v_type = VNON;
			VN_UNLOCK(vp);
			goto bad;
		}
		IN_LOCK(ip);
		ip->i_rdev = vap->va_rdev;
#ifdef	OSF1_ADFS
		if (vap->va_node != NONODE) {
			PUTNODE_IN_INODE(ip, vap->va_node);
		} else {
			ip->i_node = 0;
		}
#endif
		IN_UNLOCK(ip);
		VN_LOCK(vp);
		vp->v_op = &spec_inodeops;
		VN_UNLOCK(vp);
	} else if (type == VLNK) {
		/*
	 	 * Handle symlinks, making a fast link if name fits.
	 	 */
		register int len = strlen(vap->va_symlink);
		if (len < MAX_FASTLINK_SIZE && create_fastlinks) {
			bcopy(vap->va_symlink, ip->i_symlink, len);
			IN_WRITE_LOCK(ip);
			IN_LOCK(ip);
			ip->i_size = len;
			/*
			 * Strictly speaking, these three statements
			 * need not be placed under inode I/O lock
			 * but doing so saves releasing, reacquiring
			 * and re-releasing the incore lock... on a
			 * file that won't have any other users yet!
			 */
			ip->i_symlink[len] = '\0';
			ip->i_flags |= IC_FASTLINK;
			ip->i_flag |= IACC|IUPD|ICHG;
			IN_UNLOCK(ip);
			IN_WRITE_UNLOCK(ip);
		} else {
			IN_WRITE_LOCK(pdir);
			error = iupdat(ip, &time, &time, 1);
			IN_WRITE_UNLOCK(pdir);
			updated = 1;
			if (error)
				goto bad;
			error = vn_rdwr(UIO_WRITE, ITOV(ip), vap->va_symlink, 
					len, (off_t)0, UIO_SYSSPACE,
					0, ndp->ni_cred, (int *)0);
			if (error)
				goto bad;
		}
	} else if (type == VSOCK)
		vp->v_socket = (struct socket *) vap->va_socket;
	else if (type == VFIFO)
		vp->v_op = &fifo_inodeops;
	/*
	 * Make sure inode goes to disk before directory entry.
	 */
	IN_WRITE_LOCK(pdir);
	if (!updated) {
		error = iupdat(ip, &time, &time, 1);
	}
	if (!error)
	    error = direnter(ip, ndp);
	IN_WRITE_UNLOCK(pdir);
	if (!error) {
		iput(pdir);
		*ipp = ip;
		return (0);
	}
bad:
	iput(pdir);
	/*
	 * Write error occurred trying to update the inode
	 * or the directory so must deallocate the inode.
	 */
	IN_LOCK(ip);
	ip->i_nlink = 0;
	ip->i_gen = get_nextgen();
	ip->i_flag |= ICHG;
	IN_UNLOCK(ip);
	iput(ip);
	return (error);
}

int	inode_read_aheads = 0;
int	inode_read_individuals = 0;

#ifdef	OSF1_SERVER		/* to get rid of subsequent ifdefs */
#define pmap_zero_page(x)	0
#define	copy_to_phys(x,y,z)	0
#endif

ufs_page_read(vp, uio, cred)
	struct vnode *vp;
	struct uio *uio;
	struct ucred *cred;
{
	register struct inode *ip = VTOI(vp);
	struct fs *fs;		/* filesystem */
	int bsize;		/* size of logical blocks */
	int limit;		/* can't read more bytes than this */
	int amount;		/* bytes left to read */
	int sofar;		/* bytes read so far */
	int error;		/* our error return */
	int size;
	int phys = 0;
	u_long isize;

	ASSERT(!VIO_IS_MAPPED(vp));
	error = EINVAL;		/* initially, no data read */

	IN_LOCK(ip);
	ip->i_flag |= IACC;
	IN_UNLOCK(ip);

	/*
	 *	Check that we are trying to read data that
	 *	lies within the file.
	 */

	fs = ip->i_fs;
	bsize = fs->fs_bsize;
	sofar = 0;
	size = uio->uio_resid;
	if (uio->uio_segflg == UIO_PHYSSPACE)
		phys++;

	IN_READ_LOCK(ip);
	BM(IN_LOCK(ip));
	isize = ip->i_size;
	limit = isize - uio->uio_offset;
	BM(IN_UNLOCK(ip));
	if ((limit <= 0) || (size <= 0)) {
		IN_READ_UNLOCK(ip);
		return(error);
	}
	amount = (size <= limit) ? size : limit;
	if (phys && limit < size)
		pmap_zero_page(uio->uio_iov->iov_base);

	do {
		daddr_t lbn;	/* logical block where our data lies */
		int on;		/* byte offset within that logical block */
		int n;		/* the number of bytes read this time */
		daddr_t rablock;
		int fsize;	/* size of fragment/block */
		struct buf *bp;	/* our buffer with the data */

		/*
		 *	Find block and offset within it for our data.
		 */

		lbn = lblkno(fs, uio->uio_offset);
		on  = blkoff(fs, uio->uio_offset);

		/*
		 *	Don't read beyond the end of a logical block.
		 *	We handle logical blocks one at a time.
		 */

		n = MIN(bsize - on, amount);
		rablock = lbn + 1;

		/*
		 *	We might be reading from a fragment
		 *	instead of a full block.
		 */

		fsize = blksize(fs, ip, lbn);
		n = MIN(n, fsize);

		/*
		 *	If we're doing sequential IO, try read-ahead.
		 */

		VN_LOCK(vp);
		if (vp->v_lastr + 1 == lbn &&
		    lblktosize(fs, rablock) < isize) {
			VN_UNLOCK(vp);
			inode_read_aheads++;
				error = breada(vp, lbn, fsize, rablock,
					blksize(fs, ip, rablock), NOCRED, &bp);
		} else {
			VN_UNLOCK(vp);
			inode_read_individuals++;
			error = bread(vp, lbn, fsize, NOCRED, &bp);
		}
		VN_LOCK(vp);
		vp->v_lastr = lbn;
		VN_UNLOCK(vp);

		LASSERT(BUF_LOCK_HOLDER(bp));
		if (error) {
			VN_LOCK(vp);
			vp->v_vm_info->error = error;
			VN_UNLOCK(vp);
			brelse(bp);
			printf("error %d on pagein (bread)\n", error);
			error = EIO;
			IN_READ_UNLOCK(ip);
			return(error);
		}
		ASSERT(bp->b_resid == 0);
		ASSERT(bp->b_bcount == fsize);
		if (phys)
			copy_to_phys(bp->b_un.b_addr+on, uio->uio_iov->iov_base + sofar, n);
		else
			bcopy(bp->b_un.b_addr+on, uio->uio_iov->iov_base + sofar, n);
		bp->b_flags |= B_USELESS;
		brelse(bp);

		/*
		 *	If we finally put good data
		 *	into the buffer, we have to zero
		 *	the initial portion that we skipped.
		 */
		if (error != 0) {
/* NEED TO ZERO BEGINNING OF PAGE IF phys */
/* THIS CODE IS NEVER REACHED!! */
			if (!phys && sofar > 0)
				bzero(uio->uio_iov->iov_base, sofar);
			error = 0;
		}

		sofar += n;
		uio->uio_offset += n;
		uio->uio_resid -= n;
		amount -= n;
	} while (amount > 0);
	IN_READ_UNLOCK(ip);
	/*
	 *	If we are returning real data in the buffer,
	 *	and we ran up against the file size,
	 *	we must zero the remainder of the buffer.
	 */
	if (!phys && (error == 0) && (limit < size))
		bzero(uio->uio_iov->iov_base + limit, size - limit);

	return(error);
}

ufs_page_write(vp, uio, cred, pager, offset)
	struct vnode	*vp;
	struct uio 	*uio;
	struct ucred	*cred;
	memory_object_t pager;
	vm_offset_t	offset;
{
	register struct inode *ip = VTOI(vp);
	int		error;

	ASSERT(!VIO_IS_MAPPED(vp));
	error = ufs_write(vp, uio, 0, cred);
	if (error)
		printf("error %d on pageout (ufs_page_write)\n", error);
	return error;
}


#ifdef	OSF1_ADFS

/*
 * A note about handling the file size for Mapped Files:
 *
 * Historically, the ip->i_size field in inodes has really represented
 * two notions:
 * 1) the size of data that's been written with write()
 * 2) the size of data on the disk 
 * 
 * Because buffer caching has been intimately tied in with the UFS,
 * representing both these notions with a single i_size field has
 * not been problematic.
 *
 * With mapped files, however, the caching is done at a higher layer.
 * This has necessitated treating each notion of size independently.
 * Hence, ip->i_size continues to represent the size of data on the disk,
 * but a new field, ip->i_truesize (not in the on-disk inode) represents
 * the size of data that's been written with write().
 *
 * At any given moment, it may be the case that an inode has not had its
 * i_truesize field updated to reflect reality (because of caching at 
 * higher layers).  Hence, the mapped files module provides an interface,
 * mf_update, for causing the size to be written back to the UFS (called
 * by ufs_getattr).  Also, ufs_pageout uses the mf_get_size_for_pageout 
 * interface.
 *
 * Interfaces for getting/setting an inode's i_truesize are provided by 
 * ufs_getsize/ufs_update.  
 */


/*
 * Update the accessed/modified flags and file size associated with an inode.
 * Also clear the suid and sgid bits if an unprivileged user has modified
 * the file.
 *
 *
 * The file size reflects the amount of data that has been written via
 * write(), which may be greater than the amount that has been propogated
 * to disk.  Hence, it must be recorded in a different field than ip->i_size
 * (i.e., ip->i_truesize).
 */
ufs_update(vp, accessed, modified, size, flags, cred)
	register struct vnode 	*vp;
	int	 		accessed;
	int	 		modified;
	int			size;
	int			flags;
	struct ucred 		*cred;
{
	struct inode		*ip = VTOI(vp);

	IN_LOCK(ip);
	if (accessed)	
		ip->i_flag |= IACC;		
        if (modified) {
                ip->i_flag |= IUPD;
#if     MAPPED_FILES
                if (VIO_IS_MAPPED(vp)) {
                        /*
                         * If an unprivileged user modifies a file we must
                         * clear the suid and sgid bits.  Also see the
                         * code/comment in chmod1.
                         */
                        if (cred->cr_uid != 0) {
                                u_short mask = ISUID;
                                /*
                                 * Don't clear enforcement mode lock bits,
                                 * indicated by setgid bit, but no group
                                 * execute.
                                 */
                                if (!(ip->i_mode & ISGID) ||
                                    (ip->i_mode & S_IXGRP))
                                        mask |= ISGID;
                                ip->i_mode &= ~mask;
                        }
                }
#endif
        }
#if	MAPPED_FILES	
	/*
	 * For VIO_IS_MAPPED files, if the size is changing we must update 
	 * the i_truesize field.  A higher layer guarantees that callers of
	 * ufs_update and ufs_getsize are synchronized.
	 *
	 * If we're truncating (growing or shrinking), itrunc will update 
	 * i_truesize, otherwise do it here.
	 */
	if (size != -1) {
		ASSERT(VIO_IS_MAPPED(vp));
		if (flags & IO_TRUNC) {
			IN_UNLOCK(ip);
			return(itrunc(ip, (u_long) size, flags));
		} else {
			if (size < ip->i_truesize)
				printf("ufs_update: newsize=%d < cursize=%d.  Should be rare occurrence.\n", size, ip->i_truesize);
			else {
				ip->i_truesize = size;
				ip->i_flag |= ISIZ;  /* i_size needs updating */
			}
		}
	}
#endif
	IN_UNLOCK(ip);

	return(ESUCCESS);
}


#if	UFS_NBC

void dataout_ichange(struct inode *, u_long, struct ucred *);

#if	UFS_NBC_DEBUG
#define	debug_incr_counter(x)	(x)++
int			ufs_alloc_frag_move = 0;     /* stats */
int			ufs_alloc_frag_extend = 0;
#ifdef	PFS
int			ufs_prealloc_frag_move = 0;     /* stats */
int			ufs_prealloc_frag_extend = 0;
#endif
int			calc_alloc = 0;			
int			calc_alloc_move = 0;
int			calc_alloc_extend = 0;
int			calc_alloc_aligned = 0;
int			calc_alloc_unaligned = 0;
int			calc_no_alloc = 0;
int			calc_no_alloc_aligned = 0;
int			calc_no_alloc_unaligned = 0;
#else
#define	debug_incr_counter(x)	
#endif

/*
 * Allocate disk storage.  This will not extend the length (ip->i_size) 
 * of a file.
 */
ufs_alloc(vp, offset, length, count, cred)
	struct vnode 	*vp;
	off_t		offset;
	int  		length;
	int		*count;
	struct ucred 	*cred;
{
	register struct inode 	*ip = VTOI(vp);
	register struct fs 	*fs;
	daddr_t			lbn, bn;
	vm_address_t		buf;
	boolean_t		new_alloc;
	int 			on, n, resid, write_indx, write_size; 
	int			error = ESUCCESS;

	ASSERT(VIO_BLK_RESERVE(vp));
	IN_WRITE_LOCK(ip);
	fs = ip->i_fs;
	resid = length;
	on = blkoff(fs, offset);  	/* offset in logical block */
	while (resid) {
		lbn = lblkno(fs, offset);
		n = MIN((unsigned)(fs->fs_bsize - on), resid);
		/*
		 * Reserve physical blocks corresponding to logical block lbn.
		 * Tell balloc_nbc to synchronize with outstanding I/O's.
		 */
		if (error = balloc_nbc(ip, lbn, (int)(on + n), TRUE, 0,
				       &bn, &new_alloc, &write_indx,
				       &write_size, &buf, B_RESERVE))
			break;

		if (new_alloc && write_size > 0) {
			if (write_indx != 0)
				bn += btodg(write_indx);
			if (buf != NULL) {
				/*
				 * A frag is being extended and moved.
				 */
				debug_incr_counter(ufs_alloc_frag_move); 
			} else {
				/*
				 * A frag is only being extended.
				 */
				debug_incr_counter(ufs_alloc_frag_extend); 
				buf = balloc_get_zero_buf(write_size);
			}

			if (VIO_IS_FASTPATH(vp)) 
				error = vio_write(vp, fs->fs_devinfo, 
						  buf, write_size, lbn, 1, 
						  bn, btodg(write_size),
						  TRUE, FALSE);
			else
				error = data_write(fs->fs_devinfo, bn, 
						   buf, 
						   btodg(write_size),
						   FALSE);
			if (error) 
				break;
		}

		resid -= n;
		offset += n;
		on = 0;
	}  /* while */
		
	IN_WRITE_UNLOCK(ip);
	*count = length - resid;

	return(error);
}

/*
 * Get the size of a file.  It differs from ufs_getattr in that it
 * does not call mf_update to force information back from the mf module.
 * Hence, the caller must be sure that cached size information has
 * already been propogated back to the ufs.
 */
ufs_getsize(vp, sizep, flags, cred)
	struct vnode 	*vp;
	int		*sizep;
	int		flags;
	struct ucred 	*cred;
{
	struct inode	*ip = VTOI(vp);

	IN_LOCK(ip);
	*sizep = ip->i_truesize;
	IN_UNLOCK(ip);

	return(ESUCCESS);
}

/*
 * NOTE:  ufs_pageout and ufs_pagein will eventually be subsumed by the new
 * ufs_datain and ufs_dataout interfaces.  However, currently they are
 * still used by VIO_IS_MAPPED and paging files.
 */

/*
 * File data read routine that bypasses the traditional buffer cache.
 * The file offset to read from need not be page-aligned, and the requested 
 * length need not be a page size multiple.  The returned data is pointed
 * to directly by *uio->uio_iov->iov_base.  However, the actual buffer 
 * returned is page-aligned and a page size multiple (with the beginning 
 * and end of the buffer zero'd appropriately).
 *
 * XXX Currently only page-aligned requests, and sizes less than a file
 * system block size are supported.
 */
ufs_pagein(vp, uio, cred)
	struct vnode 		*vp;
	struct uio 		*uio;
	struct ucred 		*cred;
{
	register struct inode 	*ip = VTOI(vp);
	struct fs 		*fs;		
	vm_address_t 		data = NULL;  
	daddr_t 		lbn, bn, rablock, rabn;   
	int 			limit, read_len, sofar, on, n, ragrans;
	u_long 			isize;
	int			error;
	extern vm_size_t	page_mask;

	error = EINVAL;		/* initially, no data read */

	/*
	 * For mapped files, updates to i_flag are handled at a
	 * higher layer.
	 */
	if (!VIO_IS_MAPPED(vp)) {
		IN_LOCK(ip);
		ip->i_flag |= IACC;
		IN_UNLOCK(ip);
	}

	ASSERT(vp->v_type == VREG);

	IN_READ_LOCK(ip);
	BM(IN_LOCK(ip));
	isize = VIO_IS_PAGING(vp) ? ip->i_size : ip->i_writesize;
	limit = isize - uio->uio_offset;
	BM(IN_UNLOCK(ip));

	/*
	 * Check that we are trying to read data that lies within the file.
	 * Returning EINVAL here will cause a zero-fill to occur which is
	 * what we want.  
	 *
	 * XXX if limit <=0 could return a successful error code and set
	 * the 'data' return arg to NULL, as is done below.  This should
	 * be rectified when multi-page requests are supported.
	 */
	if ((limit <= 0) || (uio->uio_resid <= 0)) {
		IN_READ_UNLOCK(ip);
		return(error);
	}

	fs = ip->i_fs;

	/* Assumptions */
	ASSERT(vm_page_size <= fs->fs_bsize);
	ASSERT((fs->fs_bsize & page_mask) == 0);
	if ((uio->uio_offset & page_mask) != 0 || uio->uio_resid > fs->fs_bsize)
		panic("ufs_pagein.badassump");

	/* restrict amount to read by end-of-file */
	read_len = (limit <= uio->uio_resid) ? limit : uio->uio_resid;

	/* byte offset within first logical blk */
	on  = blkoff(fs, uio->uio_offset);  
	sofar = 0;
	while (read_len) {
		/*
		 * Amount to read on this disk op is restricted by the
		 * end of the file system block or the end of file.
		 */
		n = MIN(fs->fs_bsize - on, read_len);

		/*
		 * Obtain logical block, map it to a physical block, and
		 * increment it to the proper block (if reading from the 
		 * middle of a logical block).
		 */
		lbn = lblkno(fs, uio->uio_offset);
		if (error = bmap(ip, lbn, &bn)) 
			break;

		/*
		 * If we're doing sequential IO, try read-ahead.
		 */
		rablock = lbn + 1;
		VN_LOCK(vp);
		if (vp->v_lastr + 1 == lbn &&
		    lblktosize(fs, rablock) < isize) {
			VN_UNLOCK(vp);
			inode_read_aheads++;
			error = bmap(ip, rablock, &rabn);
			if (error || rabn < 0)
			      rabn = ragrans = 0;
			else
			      ragrans = btodg(blksize2(fs, isize, rablock));
		} else {
			VN_UNLOCK(vp);
			inode_read_individuals++;
			rabn = ragrans = 0;
		}

		/* rabn = ragrans = 0;  this turns off read-ahead */

		/*
		 * Get the data.  If we're trying to read from a hole in
		 * the file just return 0's.
		 */
		if ((long)bn == -1) {
			/*
			 * This will trigger the caller to do a zero-fill.
			 */
                        error = EINVAL;
			data = NULL;  
		} else {
			/* 
			 * 'data' returned is a page size multiple and
			 * is guaranteed to be zero'd beyond the length
			 * requested.  Note that invalid bytes at the 
			 * end of a record were zero'd when the sector
			 * was written.
			 */
			if (on != 0)
				bn += btodg(on);
			if (error = data_read(fs->fs_devinfo, bn, 
					      btodg(mrecroundup(fs, n)),
					      rabn, ragrans, &data))
				break;
		}
		VN_LOCK(vp);
		vp->v_lastr = lbn;
		VN_UNLOCK(vp);

		/*
		 * Set the data return address.
		 * For now, we only support one data region (enforced below).
		 * When we support multiple data regions, must adapt the
		 * zero-fill logic above.
		 */
		*(vm_address_t *)(uio->uio_iov->iov_base) = data;

		sofar += n;
		uio->uio_offset += n;
		read_len -= n;

		/* 'on' is only possibly non-zero the first time through */
		if ((on = blkoff(fs, uio->uio_offset)) != 0 && read_len != 0)
			panic("ufs_pagein.badon");

		/* 
		 * Only single I/O's supported at the moment.
		 */
		if (read_len != 0) {
			printf("ufs_page_read - amount=%d != 0. offset=%d\n",
			       read_len, uio->uio_offset+n);
			panic("ufs_pagein.multi");
		}
	}  /* while */

	uio->uio_resid -= sofar;

	if (error && error != EINVAL) {
		VN_LOCK(vp);
		vp->v_vm_info->error = error;
		VN_UNLOCK(vp);
		printf("error %d on pagein (bread)\n", error);
		error = EIO;
		IN_READ_UNLOCK(ip);
		return(error);
	}

	IN_READ_UNLOCK(ip);

	return(error);
}

/*
 * This routine is called by ufs_data_write_nbc to perform post-processing
 * of the information returned from balloc_nbc.  It is only called when new
 * disk space has been allocated.  In this case, we must ensure that the
 * new space (of size write_size) is written with either new data or zero's.  
 *
 * On entry, if buf is non-NULL then it represents data from a disk fragment 
 * that is being moved to a new location.  The buffer has write_size bytes 
 * of data (from the old fragment as well as zero'd padding at the end), 
 * but the actual size of the buffer is rounded up to a page size multiple.
 *
 * All args are IN except 'buf' which is IN/OUT.
 */
int
calc_data_write_buf(origbuf, on, n, write_indx, write_size, buf)      
	char		*origbuf;
	int		on;
	int		n;
	int		write_indx;
	int		write_size;
	char		**buf;		/* IN/OUT */
{	                                      
	int		amount;

	if (*buf != NULL) {					
		debug_incr_counter(calc_alloc_move);				
		bcopy(origbuf, *buf+on, n);	
		if (vm_deallocate(mach_task_self(),(vm_address_t)origbuf,n))
			panic("calc_data_write_buf.vm_dealloc1");
	} else if (on == write_indx && n == write_size) {	
		/*						
		 * Data to be written is on a record boundary 	
		 * and is a multiple of the record size.	
		 */						
		debug_incr_counter(calc_alloc_aligned);				
		*buf = origbuf;					
	} else {						
		/*			  
		 * Allocate a buf and fill with data or zero's. 
		 * Future optimization:	 If possible use the original 
		 * buf (which is page aligned) to do the zero'ing.
		 * Probably worthwhile since 'calc_alloc_aligned' counter
		 * has been observed to be large (probably as a result
		 * of growing files by non-aligned amounts).
		 */						
		debug_incr_counter(calc_alloc_unaligned);
		if (vm_allocate(mach_task_self(), (vm_address_t *) buf, 
				write_size, TRUE) != KERN_SUCCESS)
			return(ENOMEM);

		if (on < write_indx)
			panic("calc_data_write_buf");
		/* make 'on' relative to beg. of the buffer */
		on -= write_indx;  

		bcopy(origbuf, *buf+on, n);		/* the real data */
		amount = n;

		if (on) {		
			bzero(*buf, on);	
			amount += on;
		}

		if (write_size > amount)
			bzero(*buf+amount, write_size - amount);		

		if (vm_deallocate(mach_task_self(),(vm_address_t)origbuf,n))
			panic("calc_data_write_buf.vm_dealloc2");
	}

	return(ESUCCESS);
}					

/*
 * File data write routine that bypasses the traditional buffer cache.
 * It assumes that the buffer provided in the request is page-aligned and
 * has a length a multiple of the page size.  However, the range of valid
 * data within the buffer may be of arbitrary alignment and length.  The
 * beginning of this range is indicated by uio->uio_iov->iov_base adjusted
 * upwards by (uio->uio_offset % vm_page_size) bytes.
 */
ufs_pageout(vp, uio, cred)
	register struct vnode 	*vp;
	struct uio 		*uio;
	struct ucred 		*cred;
{
	register struct inode 	*ip = VTOI(vp);
	register struct fs 	*fs;
	daddr_t 		lbn, bn;
	u_long 			isize;
	int 			n, on, write_indx, write_size;
	int			bufoff, amount;
	vm_address_t 		origbuf, buf;
	boolean_t 		new_alloc;
	int			error = 0;
	extern vm_size_t	page_mask;
	u_long			efbig;

	if (uio->uio_rw != UIO_WRITE)
		panic("ufs_write mode");
	if (uio->uio_resid == 0)
		return (0);
	if (uio->uio_offset < 0)
		return (EINVAL);

	BM(VN_LOCK(vp));
	if (vp->v_type != VREG)
		panic("ufs_pageout not file");
	BM(VN_UNLOCK(vp));

	/*
	 * Note: this routine assumes that RLIMIT_FSIZE checks (so that
	 * EFBIG may be returned) are done at a higher layer.
	 */

	fs = ip->i_fs;

	/* Assumptions */
	ASSERT(vm_page_size <= fs->fs_bsize);
	ASSERT((fs->fs_bsize & page_mask) == 0);
	if ((uio->uio_offset & page_mask) != 0)
		panic("ufs_pageout.badassump");

	IN_WRITE_LOCK(ip);
	BM(IN_LOCK(ip));
	isize = VIO_IS_PAGING(vp) ? ip->i_size : ip->i_writesize;
	BM(IN_UNLOCK(ip));
	/*
	 * Make sure the file size doesn't exceed INT_MAX, the max. file size 
	 */
	efbig = uio->uio_offset + uio->uio_resid;
	if (efbig > INT_MAX) {
		efbig -= INT_MAX;
		uio->uio_resid -= efbig;
	} else 
		efbig = 0;
	
	bufoff = uio->uio_offset & page_mask;
	on = blkoff(fs, uio->uio_offset);  /* offset in logical block */
	while (uio->uio_resid) {
		lbn = lblkno(fs, uio->uio_offset);
		n = MIN((unsigned)(fs->fs_bsize - on), uio->uio_resid);
		origbuf = (vm_address_t) 
			  ((char *)(uio->uio_iov->iov_base)+bufoff);

		/* 
		 * balloc_nbc returns an identifier for a contiguous range of
		 * physical blocks (bn) representing a logical block (lbn).
		 * Other information returned is handled directly by
		 * calc_data_write_args.
		 */
		if (error = balloc_nbc(ip, lbn, (int)(on + n), FALSE, 0, &bn,
				       &new_alloc, &write_indx, &write_size,
				       &buf, 0))
			break;

		if (!new_alloc) {
			/*							
			 * No new disk space was allocated.  Must ensure that
			 * we write on record boundaries.
			 */							
			debug_incr_counter(calc_no_alloc);
			write_indx = mrecrounddown(fs, on);
			write_size = mrecroundup(fs, on + n) - write_indx;
			if (write_indx != 0)
				bn += btodg(write_indx);

			/*
			 * XXX Sometimes n < write_size because eof has
			 * been reached.  Don't want to read first in this case.
			 */
			if (on == write_indx && n == write_size) {		
				/*						
				 * Data to be written is on a record boundary 	
				 * and is a multiple of the record size.	
				 */						
				debug_incr_counter(calc_no_alloc_aligned);
				buf = origbuf;
			} else {						
				/*						
				 * Data to be written is not record-aligned
				 * Must read the records first and copy		
				 * the new data.				
				 */						
				debug_incr_counter(calc_no_alloc_unaligned);
				if (error = data_read(fs->fs_devinfo, bn, 
						      btodg(write_size),
						      0, 0, &buf))
					break;
				/* make 'on' relative to beg. of the buffer */
				on -= write_indx;  
				bcopy((char*)origbuf, (char *)buf+on, n);
				if (vm_deallocate(mach_task_self(), 
						     origbuf, n))
					panic("ufs_write_nbc.vm_dealloc1");
			}					   
		} else {
			/*
			 * New disk space was allocated.  Must ensure that
			 * all of it's written with real data or zero's.
			 *
			 * XXX Be more intelligent about eof. (that is, don't 
			 * need to ever write zero's beyond eof because reading 
			 * does the zeroing appropriately).
			 */
			/*
			 * Take care of the case where we did a frag
			 * extension in place (note: write_indx is the
			 * index of the new disk space allocated).
			 * XXX Could be more intelligent about not reading
			 * first if 'on' is record-aligned.
			 */
			debug_incr_counter(calc_alloc);
			if (buf == NULL && on < write_indx) {
				debug_incr_counter(calc_alloc_extend);
				write_indx = mrecrounddown(fs, on);
				write_size = mrecroundup(fs, on + n) - 
					write_indx;
				if (write_indx != 0)
					bn += btodg(write_indx);
				if (error = data_read(fs->fs_devinfo, bn, 
						      btodg(write_size),
						      0, 0, &buf))
					break;
				/* make 'on' relative to beg. of the buffer */
				on -= write_indx;  
				bcopy((char*)origbuf, (char *)buf+on, n);
				amount = on + n;
				if (write_size > amount)
					bzero((char *)buf+amount, 
					      write_size-amount);

				if (vm_deallocate(mach_task_self(),
						     (vm_address_t)origbuf, n))
					panic("ufs_write_nbc.vm_dealloc2");
			} else {
				if (error = calc_data_write_buf(origbuf, on, n,
						 write_indx, write_size, &buf))
					break;
				if (write_indx != 0)
					bn += btodg(write_indx);
			}
		}
		
		if ((uio->uio_offset + n) > isize) {
			IN_LOCK(ip);
			if (VIO_IS_PAGING(vp)) {
				ip->i_size = uio->uio_offset + n;
				ip->i_flag |= IUPD|ICHG;
			} else {
				ip->i_writesize = uio->uio_offset + n;
				/* Updates to flags done in ufs_update */
			}

			IN_UNLOCK(ip);
		}

		/*
		 * Write the data.
		 *
		 * data_write() consumes (deallocates) data on a page size
		 * granularity.  It's possible, however, that the valid data
		 * within the VM region, indicated by (buf, size), is not 
		 * page-aligned.  Note that this is an asynchronous write.
		 */
		if (error = data_write(fs->fs_devinfo, bn, buf, 
				       btodg(write_size), FALSE))
			break;

		bufoff += n;
		uio->uio_resid -= n;
		uio->uio_offset += n;
		/* 'on' is only possibly non-zero the first time through */
		if ((on = blkoff(fs, uio->uio_offset)) != 0 && 
		    uio->uio_resid != 0)
			panic("ufs_pageout.badon");
		
		IN_LOCK(ip);
			if (!VIO_IS_MAPPED(vp) && (ip->i_mode & IFMT) != IFDIR){
			/*
			 * clear setuid & setgid bits on regular files
			 * (unless privileged). Clearing the bits for
			 * mapped files occurs at ufs_update time.
			 */
#if SEC_BASE
			if (!privileged(SEC_OWNER, 0) || !privileged(SEC_CHMODSUGID,0)) {
#else
			if (cred->cr_uid != 0) {
#endif
				u_short mask = ISUID;
				/*
				 * Don't clear enforcement mode lock bits, 
				 * indicated by setgid bit, but no group execute.
				 */
				if (!(ip->i_mode & ISGID) || (ip->i_mode & S_IXGRP))
					mask |= ISGID;
				ip->i_mode &= ~mask;
			 }
#if SEC_PRIV
			if (!privileged(SEC_CHPRIV, 0)) {
				bzero(ip->i_gpriv, sizeof ip->i_gpriv);
				bzero(ip->i_ppriv, sizeof ip->i_ppriv);
			}
#endif
		}
		IN_UNLOCK(ip);

	}  /* while */

	IN_WRITE_UNLOCK(ip);
	if (efbig > 0) 
		uio->uio_resid += efbig;

	return (error);
}


#if	UFS_NBC_DEBUG
int			ufs_datain_success = 0;     /* stats */
int			ufs_datain_error = 0;
int			ufs_datain_io1optimal = 0;	
int			ufs_datain_io1 = 0;	
int			ufs_datain_io2 = 0;	
int			ufs_datain_io3 = 0;	
int			ufs_datain_io4 = 0;	
int			ufs_datain_io5 = 0;	
int			ufs_datain_move = 0;	
int			ufs_datain_copy = 0;	
int			ufs_datain_overwrite = 0;	

int			ufs_dataout_success = 0;    
int			ufs_dataout_error = 0;
int			ufs_dataout_io0 = 0;	
int			ufs_dataout_io1 = 0;	
int			ufs_dataout_io2 = 0;	
int			ufs_dataout_io3 = 0;	
int			ufs_dataout_io4 = 0;	
int			ufs_dataout_io5 = 0;	
int			ufs_dataout_partial = 0;
int			ufs_dataout_vmalloc = 0;
int			ufs_dataout_read = 0;
int			ufs_dataout_fragmove = 0;
#endif

#define	BLOCK_COALESCE	1

#if	BLOCK_COALESCE
/*
 * Unlike traditional Unix device drivers, the Mach device interface
 * has a limit on the amount of bytes that can be read/written
 * in a single request.  Therefore, we define a limit here.
 * This is basically a hack in lieu of either:
 * - the kernel not having any limit, or
 * - the kernel exporting a per-device limit through a well-defined interface
 * 
 * The 512k limit is arbitrary; reset via bootmagic 'FS_MAX_DEVICE_REQUEST'.
 */
#define MAX_DEVICE_REQUEST	fs_max_device_request
long fs_max_device_request = 512*1024;

/*
 * Run-time switch to enable/disable coalescing logic.
 */
int	ufs_coalesce = 1;

#endif 	/* BLOCK_COALESCE */

/*
 * File data read routine.  It utilizes the vnode layer's VIO module 
 * for synchronization and the buffer cache is not used to cache 
 * file data.
 *
 * The offset passed in has no alignment restrictions.
 * Nor is there a restriction on the length that may be read (other
 * than the max. file size restriction).
 *
 * If the uio->uio_iov->iov_base argument is non-NULL then the caller
 * wants a buffer filled.  Otherwise, the caller wants a new buffer
 * containing the requested data to be returned.  In this case, the
 * caller is responsible for deallocating the new buffer.
 */
ufs_datain(vp, uio, ioflag, cred)
	struct vnode 		*vp;
	struct uio 		*uio;
	int			ioflag;
	struct ucred 		*cred;
{
	register struct inode 	*ip = VTOI(vp);
	register struct fs 	*fs;		
	daddr_t 		lbn, bn, numblks;
	int 			limit, read_len, sofar, on, n, round_amount;
	int			extra_len;	/* extra data in buffer after read */
	int			numgran, io_cnt, buflen, error;
	vm_address_t		buf, newbuf;
	void			*tag;
	extern vm_size_t	page_mask;
#if	BLOCK_COALESCE
	daddr_t			bn2;
	int			n2, read_len2;
#endif

	fs = ip->i_fs;
	ASSERT(VIO_IS_FASTPATH(vp));
	ASSERT(vm_page_size <= fs->fs_bsize);
	ASSERT((fs->fs_bsize & page_mask) == 0);
	ASSERT(uio->uio_resid >= 0);
	if (uio->uio_iovcnt != 1)
		panic("ufs_datain: uio_iovcnt=%d != 1\n", uio->uio_iovcnt);

	/*
	 * Initialize some return args.
	 */
	error = ESUCCESS;
	
	/* 
	 * XXX Since synchronized access to fast_path files is provided
	 * at a higher layer, we shouldn't need to restrict concurrency
	 * at this layer (except around calls to bmap(), etc.).
	 */
	IN_READ_LOCK(ip);

	BM(IN_LOCK(ip));
	limit = ip->i_size - uio->uio_offset;
	BM(IN_UNLOCK(ip));

	if ((limit <= 0) || (uio->uio_resid == 0)) {
		IN_READ_UNLOCK(ip);
		return(error);
	}
	/* restrict amount to read by end-of-file */
	read_len = (limit <= uio->uio_resid) ? limit : uio->uio_resid;

	/*
	 * We handle the case where uio_offset is not record aligned by
	 * rounding it down and compensating later when the return buffer
	 * pointer is set. uio_offset can be larger than a page_size for 
	 * devices with block sizes larger than a page. 
	 */
	round_amount = uio->uio_offset - mrecrounddown(fs, uio->uio_offset);
	if (round_amount > 0) {
		uio->uio_offset -= round_amount;
		read_len += round_amount;
	}
		
	/*
	 * It is necessary to allocate a buffer to contain the data to be 
	 * read if one or more of the following is true:
	 * - the caller didn't provide a buffer 
	 * - the offset is not record aligned (round_amount > 0)
	 * - the length is not a multiple of the record size
	 *
	 * Actual allocation is deferred on the hope that only a single
	 * I/O will be necessary, in which case the device subsystem
	 * will allocate a buffer as part of the device operation.
	 */
	if (uio->uio_iov->iov_base == NULL ||
	    round_amount > 0 || 
	    mrecroundup(fs, read_len) != read_len) {
		buf = NULL;
	} else {
		buf = (vm_offset_t)uio->uio_iov->iov_base;
	}
	newbuf = NULL;	/* may point to a newly allocated buffer */

	if (!VIO_IS_MAPPED(vp)) {
		/* mapped file updates to i_flag are done at a higher layer */
		IN_LOCK(ip);
		ip->i_flag |= IACC;
		IN_UNLOCK(ip);
	}

	/*
	 * Synchronize with other operations based on the logical block
	 * number and length of the request.
	 */
	lbn = lblkno(fs, uio->uio_offset);
	numblks = lblkno(fs, uio->uio_offset+read_len-1) - lbn + 1;

 	tag = (void *) vio_read_setup(vp, fs->fs_devinfo, lbn, numblks);
	ASSERT(tag != NULL);

	on  = blkoff(fs, uio->uio_offset);  /* offset within logical block */
	sofar = 0;
	io_cnt = 0;
	extra_len = 0;
	buflen = mrecroundup(fs,read_len); /* must be multiple of device block */ 
	lbn = -1;
	while (read_len) {
		if (lbn == -1) {
			/*
			 * Amount to read on this disk op is restricted by the
			 * end of the file system block or the end of file.
			 */
			n = MIN(fs->fs_bsize - on, read_len);

			/*
			 * Obtain logical block, map it to a physical block.
			 */
			lbn = lblkno(fs, uio->uio_offset);
			if (error = bmap(ip, lbn, &bn)) 
				break;
		} 
#if	BLOCK_COALESCE
		else {
			/* bmap was already done */
			if (error)
				break;
			n = n2;
			bn = bn2;
		}
#endif
		lbn = -1;	/* reset */

		if ((long)bn == -1) {
			/*
			 * Zero-fill if reading from a hole.
			 */
			if (buf == NULL) {
				/* must allocate a buffer */
				if (error = vm_allocate(mach_task_self(), 
							&newbuf, 
							mrecroundup(fs,read_len), 
							TRUE))
					break;
				buf = newbuf;
			}
			bzero((char *) buf, n);

		} else {
			if (on != 0) {
				/*
				 * Increment record number if reading from
				 * the middle of a logical block.
				 */
				bn += btodg(on);
			}
			numgran = btodg(mrecroundup(fs, n));

#if	BLOCK_COALESCE
			/*
			 * Try to build up a larger request by coalescing
			 * continguous blocks, but limit the max size
			 * to MAX_DEVICE_REQUEST bytes.
			 */
			read_len2 = read_len - n;
			while (read_len2 && n < MAX_DEVICE_REQUEST &&
			       ufs_coalesce) {
				n2 = MIN(fs->fs_bsize, read_len2);
				lbn = lblkno(fs, uio->uio_offset + n);
				if (error = bmap(ip, lbn, &bn2)) 
					break;
				
				if (bn2 != bn + numgran)
					/*
					 * If we break out here then we
					 * won't need to repeat the bmap
					 * because lbn != -1.
					 */
					break;

				n += n2;
				read_len2 -= n2;
				numgran = btodg(mrecroundup(fs, n));
				lbn = -1;
			}
#endif
			/*
			 * Perform the disk read.  Optimize by detecting
			 * the case where the entire request can be
			 * satisfied by a single I/O.
			 */
			if (buf == NULL && n == read_len) {
				/* note that io_cnt is not incremented */
				debug_incr_counter(ufs_datain_io1optimal);
				if (error = vio_device_read_synchronous(tag, 
								       bn, 
								       numgran, 
								       &newbuf))
					break;
				buf = newbuf;
				/*
				 * Remove any extra data pages from newbuf:
				 * read_len includes round_amount.
				 */
				extra_len = dgtob(numgran); /* Total size of the buffer. */
				extra_len -= read_len;  /* extra data at end of buffer */
			} else {
				if (buf == NULL) {
				      /* must allocate a buffer */
				      if (error = vm_allocate(mach_task_self(),
							      &newbuf, 
							      mrecroundup(fs,read_len),
							      TRUE))
					      break;
				      buf = newbuf;
				}
				if (error = vio_device_read(tag, bn, numgran, 
							    buf))
					break;
				/*
				 * Remove any extra data pages from buf:
				 * n includes round_amount.
				 */
				extra_len = dgtob(numgran); /* size of this read buffer */
				extra_len -= n; 	/* extra data for this read */ 
				io_cnt++;	/* track ops initiated */
			}
		}

		sofar += n;
		uio->uio_offset += n;
		ASSERT(buf != NULL);
		buf = (vm_address_t)((char *) buf + n);
		read_len -= n;
		on = 0;
	}  /* while */

#if	UFS_NBC_DEBUG
	if (error)
		debug_incr_counter(ufs_datain_error);
	else
		debug_incr_counter(ufs_datain_success);
	if (io_cnt == 1)
		debug_incr_counter(ufs_datain_io1);
	else if (io_cnt == 2)
		debug_incr_counter(ufs_datain_io2);
	else if (io_cnt == 3)
		debug_incr_counter(ufs_datain_io3);
	else if (io_cnt == 4)
		debug_incr_counter(ufs_datain_io4);
	else if (io_cnt != 0)
		/* 5 or more io's */
		debug_incr_counter(ufs_datain_io5);
#endif
	/*
	 * If any errors occurred, act as if no reads succeeded.
	 * In either case, we must wait for pending io ops to complete.
	 * Note that vio_read_complete() can tolerate an io_cnt of zero.
	 */
	if (!error)
		error = vio_read_complete(tag, io_cnt);
	else
		(void) vio_read_complete(tag, io_cnt);
		
	if (!error) {	
		/*
		 * If iov_base is non-NULL and newbuf is NULL
		 * then we were able to read directly into the
		 * supplied buffer.
		 */
		if (uio->uio_iov->iov_base == NULL) {
			ASSERT(newbuf != NULL);
			debug_incr_counter(ufs_datain_move);
			uio->uio_iov->iov_base = 
				(char *)newbuf + round_amount;
			/*
			 * Need to remove the round_amount and extra_len:
			 */
			if ((round_amount >= vm_page_size) && (newbuf != NULL)) {
				/* 
				 * Deallocate extra pages at the beginning of the 
				 * allocated buffer. sofar includes any offset.
				 */
				if (vm_deallocate(mach_task_self(), 
					(vm_address_t)newbuf, 
					trunc_page(round_amount)))
						panic("ufs_datain.vm_dealloc.offset");
			}
			if ((extra_len >= vm_page_size) && (newbuf != NULL)) {
				/*
				 * Deallocate extra pages at the end of the 
				 * allocated buffer. sofar includes any offset.
				 */
				if (vm_deallocate(mach_task_self(), 
				  	(vm_address_t)(newbuf+round_page(sofar)), 
					trunc_page(extra_len)))
						panic("ufs_datain.vm_dealloc.extra");
			}
		} else if (newbuf != NULL) {
			debug_incr_counter(ufs_datain_copy);
			bcopy((char *)newbuf+round_amount, 
			      uio->uio_iov->iov_base,
			      sofar - round_amount);
			if (vm_deallocate(mach_task_self(), 
					  (vm_address_t)newbuf, 
					  buflen))
				panic("ufs_datain.vm_dealloc");
		} else
			debug_incr_counter(ufs_datain_overwrite);

		uio->uio_resid -= (sofar - round_amount);

	} else if (newbuf != NULL) {
		if (vm_deallocate(mach_task_self(), (vm_address_t)newbuf, 
				  buflen))
			panic("ufs_datain.vm_dealloc2");
	}

	IN_READ_UNLOCK(ip);
	return(error);
}

/*
 * File data write routine.  It utilizes the vnode layer's VIO module 
 * for synchronization and the buffer cache is not used to cache 
 * file data.
 *
 * The buffer and offset passed in have no alignment restrictions.
 * Nor is there a restriction on the length that may be written (other
 * than the max. file size restriction).
 *
 * If the caller sets the IO_CONSUME flag in the ioflag arg then this
 * routine will consume (deallocate) the buffer being written.  This is
 * a performance optimization because it allows the I/O to be performed
 * asynchronously without copying the data.  If the caller does not set
 * the IO_CONSUME flag then the data is copied first, but the I/O is still 
 * performed asynchronously (unless IO_SYNC is set).
 *
 * XXX If ufs_dataout is modified to support VIO_IS_MAPPED files then
 * make sure that the suid and sgid bits aren't cleared for those files
 * (see below).
 */
ufs_dataout(vp, uio, ioflag, cred)
	register struct vnode 	*vp;
	struct uio 		*uio;
	int			ioflag;
	struct ucred 		*cred;
{
	register struct inode 	*ip = VTOI(vp);
	register struct fs 	*fs;
	daddr_t 		lbn, bn, numblks;
	u_long 			isize;
	int 			n, on, write_indx, write_size;
	vm_address_t 		buf, newbuf = NULL;
	boolean_t 		new_alloc;
	void			*tag;
	struct iovec		*iovp;
	int			io_cnt, amount, error = 0; 
	int			save_indx = 0, partial_overwrite;
	extern vm_size_t	page_mask;

#define DATAOUT_BUMP(n) {					\
	buf = (vm_address_t) ((char *)buf + (n));		\
	uio->uio_resid -= (n);					\
	uio->uio_offset += (n);					\
	on = 0;							\
}
#if BLOCK_COALESCE
	vm_address_t		bcoal_buf = NULL;
        u_long			bcoal_off;
        daddr_t                 bcoal_bn = 0;
        daddr_t                 bcoal_blks = 0;

#define DATAOUT_BCOAL_PURGE {					\
	if (bcoal_blks != 0) {					\
                error = vio_device_write(tag, bcoal_buf,	\
					 bcoal_bn, bcoal_blks); \
		bcoal_blks = 0;					\
		bcoal_buf = NULL;				\
		if (error)					\
		        goto finish;				\
		io_cnt++;					\
		dataout_ichange(ip, uio->uio_offset, cred);	\
	}							\
}
#else /* BLOCK_COALESCE */
#define DATAOUT_BCOAL_PURGE
#endif /* BLOCK_COALESCE */


	fs = ip->i_fs;
	ASSERT(VIO_IS_FASTPATH(vp));
	ASSERT(vm_page_size <= fs->fs_bsize);
	ASSERT((fs->fs_bsize & page_mask) == 0);
	ASSERT(uio->uio_resid >= 0);
	ASSERT(uio->uio_rw == UIO_WRITE);

	if (uio->uio_resid == 0)
		return (ESUCCESS);
	if (uio->uio_offset < 0) {
		if ((ioflag & IO_CONSUME) != 0)
			if ((error = vm_deallocate(mach_task_self(), 
					 (vm_address_t) uio->uio_iov->iov_base,
					 (vm_size_t) uio->uio_resid))
			    != KERN_SUCCESS) 
				panic("ufs_dataout.vm_deallocate 0x%x\n", 
				      error);
		return (EINVAL);
	}

	/* 
	 * Copy the incoming data if either the IO_CONSUME flag isn't 
	 * set or the uio_iovcnt > 1.
	 */
	if ((ioflag & IO_CONSUME) == 0 || uio->uio_iovcnt > 1) {
		error = vm_allocate(mach_task_self(), &buf, uio->uio_resid, 
				    TRUE);
		if (error)
			panic("ufs_dataout.vm_allocate: 0x%x\n", error);

		on = 0;
		for (n = 0, iovp = uio->uio_iov; n < uio->uio_iovcnt; 
		     n++, iovp++) {
			bcopy(iovp->iov_base, (char *)buf+on, iovp->iov_len);
			on += iovp->iov_len;
		}
	} else
		buf = (vm_address_t) uio->uio_iov->iov_base;


	error = ESUCCESS;

	/*
	 * Note: this routine assumes that RLIMIT_FSIZE checks (so that
	 * EFBIG may be returned) are done at a higher layer.
	 */

	IN_WRITE_LOCK(ip);
	BM(IN_LOCK(ip));
	isize = ip->i_size;
	BM(IN_UNLOCK(ip));

	/*
	 * Synchronize with other operations based on the logical block
	 * number and length of the request.
	 */
	lbn = lblkno(fs, uio->uio_offset);
	numblks = lblkno(fs, uio->uio_offset+uio->uio_resid-1) - lbn + 1;

 	tag = (void *) vio_write_setup(vp, fs->fs_devinfo, buf, uio->uio_resid,
				       lbn, numblks);
	ASSERT(tag != NULL);

	on = blkoff(fs, uio->uio_offset);  	/* offset in logical block */
	io_cnt = 0;
	while (uio->uio_resid) {
		lbn = lblkno(fs, uio->uio_offset);
		n = MIN((unsigned)(fs->fs_bsize - on), uio->uio_resid);

		/* 
		 * balloc_nbc returns an identifier for a contiguous range of
		 * physical blocks (bn) representing a logical block (lbn).
		 *
		 * Upon return, new_alloc indicates if new disk space was
		 * allocated.  If so, write_indx and write_size specify how
		 * much must be written to satisfy the requirement that 
		 * all newly allocated data be written with real data or
		 * with zero's.  And, when new_alloc is true, it's possible
		 * that buf is non-NULL which specifies a buffer that must
		 * be written to the new disk space.  This supports frag
		 * reallocation.  Note that the data in the buffer in this
		 * case (with amount indicated by write_size) will have all 
		 * the data from the original frags in addition to zero's for
		 * the new frags.
		 *
		 * Since synchronization with other I/O's was performed by
		 * vio_write_setup, the 'synchronize' arg to balloc_nbc 
		 * is FALSE.
		 */
		if (error = balloc_nbc(ip, lbn, (int)(on + n), FALSE, 
#if BLOCK_COALESCE
				       bcoal_blks == 0 ? 0 : uio->uio_offset,
#else
				       0,
#endif
				       &bn, &new_alloc, &write_indx, 
				       &write_size, &newbuf, 0))
			break;

		partial_overwrite = 0;	/* must reset each time through loop */
		if (!new_alloc) {
			/*
			 * Setup these values.  balloc_nbc() only does it
			 * when new_alloc is true.
			 */
			ASSERT(newbuf == NULL && on >= write_indx);
			write_indx = mrecrounddown(fs, on);
			write_size = mrecroundup(fs, on + n) - write_indx;
		} else if (on < write_indx) {
			/*
			 * When new_alloc is true it's also possible that 
			 * on < write_indx in which case we're partially 
			 * overwriting preexisting data.  Hence, adjust
			 * write_indx, write_size and set a flag to ensure
			 * that we read data first.
			 */
			ASSERT(newbuf == NULL);
			save_indx = write_indx;
			write_indx = mrecrounddown(fs, on);
			write_size += (save_indx - write_indx);
			partial_overwrite++;
			debug_incr_counter(ufs_dataout_partial);
		}

		if (write_indx != 0)
			bn += btodg(write_indx);

		/*
		 * If we could write the blocks directly from the buffer,
		 * (see immediately below for explanation of the conditions),
		 * see if we can tack the write onto a preceding or follow-
		 * ing one.
		 */
		if (on == write_indx && n == write_size && newbuf == NULL) {
			/*
			 * Write the disk blocks directly from the 
			 * supplied buffer.  The write is asynchronous.
			 *
			 * If new_alloc == TRUE, this can be done when:
			 * - a frag reallocation is not occurring, and
			 * - the aligment and amount of the data being 
			 *   written equals that of the newly allocated 
			 *   disk space.
			 *
			 * If new_alloc == FALSE, this can be done when:
			 * - the write is on a record boundary, and
			 * - the length of the write is a multiple of the
			 *   record size.
			 */
#if BLOCK_COALESCE
			if (bcoal_blks != 0 && 
			    bn == bcoal_bn + bcoal_blks &&
			    dgtob(bcoal_blks) + n <= MAX_DEVICE_REQUEST) {
				bcoal_blks += btodg(n);
				DATAOUT_BUMP(n);
				continue;
			}
			DATAOUT_BCOAL_PURGE;
			if (ufs_coalesce) {
				bcoal_buf = buf;
				bcoal_off = uio->uio_offset;
				bcoal_bn = bn;
				bcoal_blks = btodg(n);
				DATAOUT_BUMP(n);
				continue;
			}
#endif /* BLOCK_COALESCE */			
			error = vio_device_write(tag, buf, bn, 
					      btodg(write_size));
			if (error)
				break;
			io_cnt++;
		} else {
			/*
			 * Must write from a different buffer.
			 * If newbuf is non-NULL then frag movement is
			 * occurring and we already have a proper sized buffer.
			 */
			DATAOUT_BCOAL_PURGE;
			if (newbuf == NULL) {
				/*
				 * Either vm_allocate a new buffer or read
				 * existing data from disk.  If new_alloc is
				 * true then:
				 * - may have to zero the end of the buffer
				 * - may have to zero the beginning of the 
				 *   buffer but only if we're not partially
				 *   overwriting preexisting data.
				 */
				ASSERT(on >= write_indx);
				on -= write_indx;
				if (new_alloc && !partial_overwrite) {
					/*
					 * Allocate a new buffer.  
					 */
					debug_incr_counter(ufs_dataout_vmalloc);
					error = vm_allocate(mach_task_self(),
							    &newbuf, 
							    write_size, 
							    TRUE);
					if (error)
						break;
					if (on) 	
						bzero(newbuf, on);	
				} else {
					/*
					 * Read the exiting data from disk.
					 */
					debug_incr_counter(ufs_dataout_read);
					error = vio_read(vp, fs->fs_devinfo, 
							lbn, 1, bn,
							btodg(write_size),
							FALSE,
							&newbuf);
					if (error)
						break;
				}
				ASSERT(newbuf != NULL);
				if (new_alloc) {
					amount = on + n;
					if (write_size > amount)
						bzero((char *)newbuf+amount, 
						      write_size - amount);     
				}
			} else
				debug_incr_counter(ufs_dataout_fragmove);

			/*
			 * Now copy the data into the new buffer.
			 */
			bcopy((char*)buf, (char *)newbuf+on, n);

			/*
			 * Kick off the asynchronous write, but first
			 * associate the new buffer with the logical
			 * operation in progress.  The buffer is 
			 * consumed by the vio module.
			 */
			vio_assoc_write_buf(tag, newbuf, write_size);
			error = vio_device_write(tag, newbuf, bn, 
						 btodg(write_size));
			if (error)
				break;
			io_cnt++;
		}

		DATAOUT_BUMP(n);
		dataout_ichange(ip, uio->uio_offset, cred);
			
	}  /* while */
	DATAOUT_BCOAL_PURGE;

finish:
		
#if	UFS_NBC_DEBUG
	if (error)
		debug_incr_counter(ufs_dataout_error);
	else
		debug_incr_counter(ufs_dataout_success);
	if (io_cnt == 0)
		debug_incr_counter(ufs_dataout_io0);
	else if (io_cnt == 1)
		debug_incr_counter(ufs_dataout_io1);
	else if (io_cnt == 2)
		debug_incr_counter(ufs_dataout_io2);
	else if (io_cnt == 3)
		debug_incr_counter(ufs_dataout_io3);
	else if (io_cnt == 4)
		debug_incr_counter(ufs_dataout_io4);
	else
		/* 5 or more io's */
		debug_incr_counter(ufs_dataout_io5);
#endif

	/*
	 * Tell the VIO module how many device writes were initiated.
	 * Either wait for all writes to complete, or let the VIO module
	 * cleanup the request (including deallocating the buffer)
	 * when they're all done.  Note that these routines can tolerate
	 * an io_cnt of zero.
	 */
	if (error) {
		vio_write_wait_error(tag, io_cnt, error);
	} else if (!(ioflag & IO_SYNC)) {
		vio_device_writes_initiated(tag, io_cnt);
	} else {
		error = vio_write_wait(tag, io_cnt);
	}
		
	IN_WRITE_UNLOCK(ip);

	return (error);

#undef  DATAOUT_BUMP
#undef  DATAOUT_BCOAL_PURGE
}

void
dataout_ichange(
	struct inode *ip,
	u_long size,
	struct ucred *cred)
{
        IN_LOCK(ip);
        if (size > ip->i_size) {
		ip->i_size = size;
		ip->i_flag |= ICHG|IUPD;
	}
	if ((ip->i_mode & IFMT) != IFDIR) {
	        /*
		 * clear setuid & setgid bits on regular files
		 * (unless privileged)
		 */
#if SEC_BASE
	        if (!privileged(SEC_OWNER, 0) || !privileged(SEC_CHMODSUGID,0)) {
#else
		if (cred->cr_uid != 0) {
#endif
		        u_short mask = ISUID;
			/*
			 * Don't clear enforcement mode lock bits, 
			 * indicated by setgid bit, but no group execute.
			 */
			if (!(ip->i_mode & ISGID) || (ip->i_mode & S_IXGRP))
			        mask |= ISGID;
			ip->i_mode &= ~mask;
		}
#if SEC_PRIV
		if (!privileged(SEC_CHPRIV, 0)) {
		        bzero(ip->i_gpriv, sizeof ip->i_gpriv);
			bzero(ip->i_ppriv, sizeof ip->i_ppriv);
		}
#endif
       }
       IN_UNLOCK(ip);
}

#else	/* UFS_NBC */

ufs_pagein(vp, uio, cred)
	struct vnode 	*vp;
	struct uio 	*uio;
	struct ucred 	*cred;
{
	return(ENXIO);
}

ufs_pageout(vp, uio, cred)
	struct vnode	*vp;
	struct uio 	*uio;
	struct ucred	*cred;
{
	return(ENXIO);
}

ufs_alloc(vp, offset, length, count, cred)
	struct vnode 	*vp;
	off_t		offset;
	int  		length;
	int		*count;
	struct ucred 	*cred;
{
	return(ENXIO);
}

ufs_getsize(vp, sizep, flags, cred)
	struct vnode 	*vp;
	int		*sizep;
	int		flags;
	struct ucred 	*cred;
{
	return(ENXIO);
}

ufs_datain(vp, uio, ioflag, cred)
	struct vnode 	*vp;
	struct uio 	*uio;
	int		ioflag;
	struct ucred 	*cred;
{
	return(ENXIO);
}

ufs_dataout(vp, uio, ioflag, cred)
	struct vnode	*vp;
	struct uio 	*uio;
	int		ioflag;
	struct ucred	*cred;
{
	return(ENXIO);
}

#endif	/* UFS_NBC */

#endif	/* OSF1_ADFS */

#ifdef	PFS
/*
 * Increase the size of a file, preallocating disk storage if necessary (if the
 * size of the file is increasing).
 */
ufs_prealloc(vp, oldsize, newsize, cred)
	struct vnode 	*vp;
	size_t 		oldsize;
	size_t 		*newsize;
	struct ucred	*cred;
{
#if	MAPPED_FILES
	struct vm_info  *vmp = vp->v_vm_info;
#endif
	register struct inode	*ip = VTOI(vp);
	register struct fs	*fs;
	daddr_t			lbn, bn;
	vm_address_t		buf;
	off_t			offset = oldsize;
	register int		resid = *newsize - oldsize;
	boolean_t		new_alloc, iflags_modified = FALSE;
	int			on, n, write_indx, write_size;
	long			old_iflags;
	int			error = ESUCCESS;

	/*
	 * Ensure that the new file size is not too big to return in a long.
	 */
	if ((long) *newsize < 0)
		return(EINVAL);

	/*
	 * If the new file size is less than the current file size then
	 * just return indicating success.
	 */
	if (*newsize <= oldsize) {
		*newsize = oldsize;
		return(ESUCCESS);
	}

	/*
	 * We're increasing the size of the file.  Check that user's file
	 * size limit will not be exceeded.
	 */
	if (*newsize > u.u_rlimit[RLIMIT_FSIZE].rlim_cur)
		return(EFBIG);

#if	MAPPED_FILES
	if (MF_MAPPABLE(vp))
		/*
		 * Must have the data write token since the caller does not
		 * necessarily hold it.
		 */
		get_data_token(vmp, NULL, TOK_DATA_WRITE);
#endif

	/*
	 * Mark inode preallocated, so fsck knows that it's OK to have blocks
	 * that are allocated but not written.  Get the modified inode to
	 * disk now, else if a crash occurs before the inode gets to disk fsck
	 * will clean up any blocks allocated by this prealloc operation.
	 */
	IN_LOCK(ip);
	if (!(ip->i_flags & IC_PREALLOCATED)) {
		old_iflags = ip->i_flags;
		ip->i_flags |= IC_PREALLOCATED;
		ip->i_flag |= IUPD|ICHG;
		iflags_modified = TRUE;
		IN_UNLOCK(ip);
		if (error = iupdat(ip, &time, &time, 1))
			goto out;
	} else {
		IN_UNLOCK(ip);
	}

	/*
	 * Allocate disk blocks.
	 */
	IN_WRITE_LOCK(ip);
	fs = ip->i_fs;
	on = blkoff(fs, offset);	/* offset in logical block */
	while (resid) {
		lbn = lblkno(fs, offset);
		n = MIN((unsigned)(fs->fs_bsize - on), resid);

		/*
		 * Reserve physical blocks corresponding to logical block lbn.
		 * Synchronize with other processes that may be attempting to
		 * also extend the file.
		 *
		 * Note that only full file system blocks are preallocated.
		 * Due to the complexity of the block reservation code, it is
		 * much simpler to avoid trying to "preallocate" fragments,
		 * and instead just write zeroes.  Since fragments represent
		 * a small amount of disk space and are only present in direct
		 * blocks, this is not a performance issue.
		 */
		if (error = balloc_nbc(ip, lbn, (int)(on + n), TRUE, 0, &bn,
				       &new_alloc, &write_indx, &write_size,
				       &buf,
				       (((lbn < NDADDR) && (n < fs->fs_bsize))
					? 0 : B_PREALLOC)))
			break;
		if (new_alloc && write_size > 0) {
			if (write_indx != 0)
				bn += btodg(write_indx);
			if (buf != NULL) {
				/*
				 * A frag is being extended and moved.
				 */
				debug_incr_counter(ufs_prealloc_frag_move);
			} else {
				/*
				 * A frag is only being extended.
				 */
				debug_incr_counter(ufs_prealloc_frag_extend);
				buf = balloc_get_zero_buf(write_size);
			}

			if (VIO_IS_FASTPATH(vp)) 
				error = vio_write(vp, fs->fs_devinfo, buf,
						  write_size, lbn, 1, bn,
						  btodg(write_size),
						  TRUE, FALSE);
			else
				error = data_write(fs->fs_devinfo, bn, buf, 
						   btodg(write_size),
						   FALSE);
			if (error)
				break;
		}

		resid -= n;
		offset += n;
		on = 0;		/* can only be > 0 the first time through */

		/*
		 * balloc_nbc requires an up-to-date i_size.
		 */
		IN_LOCK(ip);
		ip->i_size = ip->i_truesize = ip->i_writesize = offset;
		ip->i_flag |= ICHG|IUPD;

		if ((ip->i_mode & IFMT) != IFDIR) {
			/*
			 * Clear setuid & setgid bits on regular files
			 * (unless privileged).
			 */
#if SEC_BASE
			if (!privileged(SEC_OWNER, 0) ||
			    !privileged(SEC_CHMODSUGID,0)) {
#else
			if (cred->cr_uid != 0) {
#endif
				u_short mask = ISUID;
				/*
				 * Don't clear enforcement mode lock bits, 
				 * indicated by setgid bit, but no group
				 * execute.
				 */
				if (!(ip->i_mode & ISGID) ||
				    (ip->i_mode & S_IXGRP))
					mask |= ISGID;
				ip->i_mode &= ~mask;
			 }
#if SEC_PRIV
			if (!privileged(SEC_CHPRIV, 0)) {
				bzero(ip->i_gpriv, sizeof ip->i_gpriv);
				bzero(ip->i_ppriv, sizeof ip->i_ppriv);
			}
#endif
		}
		IN_UNLOCK(ip);
			
	}  /* while */

	IN_WRITE_UNLOCK(ip);

	if (error == ENOSPC)
		error = ESUCCESS;

	*newsize = offset;

	/*
	 * Back out the change to the inode flags if no new disk space was
	 * allocated.
	 */
	if ((offset == oldsize) && (iflags_modified)) {
		IN_LOCK(ip);
		ip->i_flags = old_iflags;
		ip->i_flag |= IUPD|ICHG;
		IN_UNLOCK(ip);
		if (error = iupdat(ip, &time, &time, 1))
			goto out;
	}

out:
#if	MAPPED_FILES
	if (MF_MAPPABLE(vp))
		rel_data_token(vmp, TOK_DATA_WRITE);
#endif

	return(error);
}


/* DEBUG_PFS */
pfs_dump_inode(ip)
struct inode	*ip;
{
	int	i;

	printf("  ino=%d size=%d truesize=%d writesize=%d\n",
	       ip->i_number, ip->i_size, ip->i_truesize, ip->i_writesize,
	       ip->i_blocks, major(ip->i_dev), minor(ip->i_dev));
	printf("  flags=%d blocks=%d dev=%d,%d\n",
	       ip->i_flags, ip->i_blocks,
	       major(ip->i_dev), minor(ip->i_dev));
	printf("  resfrags=");
	for (i = 0; i < NDADDR; i++)
		printf(" %d", (int)ip->i_resfrags[i]);
	printf("\n");
}
#endif	PFS
