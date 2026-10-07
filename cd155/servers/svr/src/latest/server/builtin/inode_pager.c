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
 * $Log: inode_pager.c,v $
 * Revision 1.24  1995/03/28  16:30:52  cfleck
 *  Reviewer: stans, andyp, terry, (server staff) johannes, jlitvin
 *  Risk: medium
 *  Benefit or PTS #: 12280
 *  Testing: johannes core dump test code, fritz's svm test, parallel SATs
 *  Module(s): kernel/norma2/norma2_init.c, kernel/norma2/norma_transport.h,
 *            kernel/norma2/dipc_special.c, server/builtin/inode_pager.c
 *  Description:  Added interface, norma_enable_vnode_pager, to register with
 * 		the kernel on startup.  This allows a special set of
 * 		NORMA RDMA resources to be assigned to the vnode pager
 * 		threads.  This keeps them from competing for these resources
 * 		under heavy loads.  This reduces the chances of blocking
 * 		waiting on these resources..
 *
 * Revision 1.23  1995/01/05  10:10:47  johannes
 * vs_page_write_complete(): If there was an error in vs_page_write() to a
 *                           paging file do a panic() instead of a printf().
 *                           If there was an error in vs_page_write() to a
 *                           regular file do a uprintf() instead of a printf().
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: low
 *  Benefit or PTS #: 11853
 *  Testing: developer testing
 *  Module(s): server/builtin/inode_pager.c
 *
 * Revision 1.22  1995/01/05  10:01:29  johannes
 * Forward the memory_object_terminate request and the vnode_pager_no_senders
 * call to the alternate thread to ensure handling the corresponding messages
 * in the right order.
 *
 *  Reviewer: Nandini
 *  Risk: medium
 *  Benefit or PTS #: 11816 (regression caused by fix of 11105)
 *  Testing: developer testing, VSE test suite (on MP and GP)
 *  Module(s): server/builtin/alt_memory_object.defs
 *             server/builtin/inode_pager.c
 *
 * Revision 1.21  1994/12/04  17:57:27  suri
 *  Reviewer: jlitvin
 *  Risk: L
 *  Benefit or PTS #: 11714
 *  Testing: Specific testcase
 *  Module(s): pf_file_init()
 *  Solution: For remote paging nodes, server assigns a new device vnode
 *  during paging file initialization, which is later is used in
 *  searching through the paging_file linked list to identify if the
 *  device has already been paging. The search was failing because
 *  pf_file_init() was using this newly generated device vnode, as
 *  opposed the actual one. The code has been modified to use the device
 *  node number, major and minor numbers as the search criteria instead.
 *
 * Revision 1.20  1994/11/21  18:41:42  johannes
 * A second ("alternate") thread per port set has been introduced.
 * The possibly blocking memory object requests init, data_request and
 * data_unlock are forwarded to the alternate thread by using local Mach
 * message passing. Thus, the other requests, especially "page-out", can
 * be handled always by the "normal" thread.
 *
 *  Reviewer: Nandini
 *  Risk: High
 *  Benefit or PTS #: 11105
 *  Testing: developer testing, IPI-3 EATs with a memory leaking server
 *  Module(s): server/builtin/inode_pager.c
 * 	    server/conf/Makefile.template
 * 	    server/builtin/alt_memory_object.defs
 *
 * Revision 1.19  1994/11/18  20:28:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1994/11/15  19:45:49  jlitvin
 * #10589 was still occurring with the previous change.  Keep nandy's
 * priority setting and dbm's ordering of the crhold() and crfree().  The
 * function (inode_pager_setup()) is now the same as dbm's used for TCAT.
 *
 *  Reviewer: nandy
 *  Risk: low
 *  Benefit or PTS #: 10589
 *  Testing: Concur EAT on meyers (which occasionally shows the bug)
 *  Module(s): server/builtin/inode_pager.c
 *
 * Revision 1.17  1994/10/25  00:26:15  jlitvin
 * Reconsider the idea behind the fix for PTS #5580.  The credentials
 * reference count has to be decremented in inode_pager_setup() since it
 * won't be done anywhere else.
 *
 *  Reviewer: nandy
 *  Risk: low
 *  Benefit or PTS #: 10589
 *  Testing: PTS #5580 test case (67 continuous hours!) and TCAT testing
 *  Module(s): server/builtin/inode_pager.c
 *
 * Revision 1.16  1994/06/28  23:00:20  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.15  1994/06/18  00:06:45  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.14  1993/10/28  03:09:05  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.13  1993/09/29  01:20:25  brad
 * Bug fix from Paul Roy at OSF for bug #5463: don't allow a pageout to
 * occur after itrunc() free's the disk blocks.
 *
 * Revision 1.12  1993/09/27  04:27:58  robboy
 * New ifdefs for FULLSERVER
 *
 * Revision 1.11  1993/08/19  00:44:04  cfj
 * Map MACH_SEND_INVALID_DEST to KERN_SUCCESS if returned by memory_object_lock_request().
 * This fixed PTS bugs #6094 & #6191.
 *
 * Revision 1.10  1993/07/19  23:04:51  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.9  1993/07/16  16:45:26  nandy
 * Don't free the credential in inode_pager_setup till the last thread is done.
 *
 * Revision 1.8  1993/07/14  17:52:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:09:48  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/11  23:59:35  brad
 * Fixed ad1.0.3 merge problem in swapon.
 *
 * Revision 1.6  1993/05/06  19:11:57  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:26:19  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/09  23:16:14  cfj
 * Merge with T9.5.  Adds paging flow control mods.
 *
 * Revision 1.3.6.1  1993/04/09  16:37:36  cfj
 * Paging flow control modifications.
 *
 *     Paging flow control (NORMA_VM).  Pagers must handshake
 *     with kernels using memory_object_data_write_completed
 *     and a new memory_object_set_attributes.  Both interfaces
 *     come from OSF/1R1.1.
 *     Also:  release vs_lock before calling m_o_lock_request
 *     and before calling m_o_change_attributes; otherwise,
 *     pager can deadlock.  This fix is temporary and eventually
 *     will be superseded by revamped locking.  [alanl, sjs]
 *
 * Revision 1.4  1993/04/03  03:05:11  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/03/17  17:35:11  dbm
 * Added check to swapon to give error if PFS file system was selected.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:59:20  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:55:23  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:18:11  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:17:13  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.54  93/10/20  15:26:20  dnoveck
 *      DEV_BSIZE elimination: change dbtob and dgtob.
 *
 * Revision 2.50  93/07/13  15:57:56  slively
 *
 *      Revision 2.49  93/06/29  16:19:26  rabii
 *      Remove some code with FULLSERVER conditional (rabii)
 *
 * Revision 2.49  93/06/25  11:19:51  slively
 * Backout the LITE server changes, #if UFS.
 * 
 * Revision 2.48  93/06/22  19:57:55  slively
 * Changes for LITE server support.  #if UFS and stubs (vnode_pager and such).
 * All stubs can be found in the #else UFS section.
 * 
 * Revision 2.47  93/06/16  15:26:54  klh
 * 	Revision 2.47  93/06/02  17:17:24  rabii
 * 		Don't pass iov_base to vs_page_write_complete in vs_page_write.
 * 		[93/05/19            roy]
 * 
 * 	Revision 2.46  93/05/18  21:47:32  loverso
 * 		Enable paging flow control (requires NMK13.24 kernel or later)
 * 
 * 	Revision 2.45  93/05/18  14:59:46  loverso
 * 		Don't provide iov_len to vs_page_write_complete.
 * 		[93/05/18            roy]
 * 
 * 	Revision 2.44  93/05/13  16:51:14  roy
 * 		Set paging file iomode to VIO_PAGING, and use VIO_IS_PAGING macro
 * 		when appropriate.  vop_pageout no longer has 'paging' arg.
 * 		[93/05/05            roy]
 * 
 * Revision 2.46  93/05/25  19:48:27  yazz
 * Changes from OSF via Intel to allow paging to partitions, in a no
 * mapped files configuration.
 * 
 * Revision 2.45  93/05/06  13:18:33  yazz
 * Cause NORMA_VM (server flow control ifdef) to be 0 for the i386.
 * 
 * Revision 2.44  93/05/05  21:01:24  yazz
 * Automatically turn on NORMA flow-control for Paragon builds but
 * not for HyperCube builds.  (When the HyperCube microkernel and
 * associated include files are updated this change will be obsoleted.)
 * 
 * Revision 2.43  93/04/08  11:29:22  loverso
 * 	ux server threads are wired by default. (loverso)
 *
 * Revision 2.42  93/03/30  16:12:35  roy
 * 	Support for fast path io files.
 * 	inode_pager_setup now asserts that vp arg is non-null.
 * 	Move code from MAPPED_FILES ifdef to OSF1_ADFS ifdef.
 * 	[93/03/18            roy]
 * 
 * Revision 2.41  93/03/25  17:27:36  roy
 * 	Paging flow control (NORMA_VM).  Pagers must handshake
 * 	with kernels using memory_object_data_write_completed
 * 	and a new memory_object_set_attributes.  Both interfaces
 * 	come from OSF/1R1.1.
 * 	Also:  release vs_lock BEFORE calling m_o_lock_request;
 * 	otherwise, pager can deadlock.  This fix is temporary
 * 	and will be superseded by revamped locking.  [alanl, sjs]
 * 
 * 	Moved all calls to m_o_d_w_completed into vs_page_write_completed.
 * 	Changed m_o_change_attributes a la m_o_lock_request above.  [roy]
 * 
 * Revision 2.40  93/03/25  09:55:53  durriya
 * 	add vnode_pager_info to return vnode pager statistics. Also 
 * 	remove duplicate pass in pf_find.                  (durriya)
 * 
 * Revision 2.39  93/03/12  13:21:36  rabii
 * 	Removed extra printfs (rabii)
 * 
 * Revision 2.38  93/03/12  12:56:14  rabii
 * 	Removed extra printfs (rabii)
 * 
 * Revision 2.37  93/02/18  12:28:19  rabii
 * 	Replaced ifdef MACH_AFS by if MACH_AFS (rabii)
 * 
 * Revision 2.36  93/02/18  11:50:03  rabii
 * 	Added new routine swapon which now allows swapons to remote
 * 	block devices (rabii)
 * 
 * 	Added extra threads for AFS files to avoid deadlocks (rabii)
 * 
 * Revision 2.35  93/01/08  14:41:28  durriya
 * 	additional node # as arg to BDEVSW_PSIZE macro.            durriya
 * 
 * Revision 2.34  92/11/12  22:22:12  loverso
 * 	Objects created via memory_object_create are torn down when they
 * 	are terminated.  This fixes a leak of paging space and vstructs
 * 	when the vnode pager is providing default paging services.
 * 	[92/11/11            roy]
 * 
 * Revision 2.32  92/11/05  17:29:04  roy
 * 	Implement memory_object_data_return.
 * 	[92/10/26            roy]
 * 
 * Revision 2.31  92/10/05  12:07:49  rabii
 * 	Added an extra flag to VOP_PAGEOUT telling the underlying operation
 * 	if the file is a paging file. (roy)
 * 
 * Revision 2.30  92/09/20  11:26:46  roy
 * 	vnode_pager_flush will avoid cleaning for temporary objects.
 * 	[92/09/17            roy]
 * 
 * Revision 2.29  92/09/11  09:27:06  rabii
 * 	Return an error if swapon of VBLK device attempted in OSF1_ADFS 
 * 	(not yet supported).
 * 	[92/09/04            roy]
 * 
 * Revision 2.28  92/08/27  09:52:13  loverso
 * 	Print message when pageouts fail.
 * 	[92/08/26            roy]
 * 
 * Revision 2.27  92/08/26  12:10:30  loverso
 *	Also call VOP_ALLOC if vs_page_read is successful but resid != 0.
 * 	Fixed so that vs->pager only used #if MACH_ASSERT.
 * 	[92/08/09            roy]
 * 
 * Revision 2.26  92/07/29  09:03:36  rabii
 * 	Fixed RCS log
 * 
 * Revision 2.25  92/07/29  08:30:37  rabii
 * 	Don't attempt to clear VTEXT flag for paging files.
 * 	Fix up vs_page_write for case where there's no paging file.
 * 	Added vnode_pager_is_set and removed cheking in of pager port (rabii).
 * 	Add count return arg to VOP_ALLOC calls.
 * 	If VOP_ALLOC fails, use m_o_data_error appropriately.
 * 	[92/07/23            roy]
 * 
 * Revision 2.24  92/07/14  14:55:27  rabii
 * 	Move clearing of vnode's VTEXT flag from the no-senders code
 * 	to the memory_object_terminate code.
 * 	[92/07/13            roy]
 * 
 * Revision 2.23  92/06/30  22:46:09  loverso
 * 	Make pf_pager_create use the ucred from proc 0 instead of the one
 * 	from current process. This fixes the call from emul_shared_init.
 * 	(rabii)
 * 
 * Revision 2.22  92/05/31  18:58:32  loverso
 * 	Doesn't need to include cpus.h anymore (pjg).
 * 	VOP_ALLOC now takes a cred arg. (roy)
 * 
 * Revision 2.21  92/05/27  20:08:08  pjg
 * 	Use master_lock and master_unlock instead of manipulating the lock
 * 	directly to allow redefinition of the master lock.
 * 	Include cpus.h and mach_ldebug.g (needed in parallel.h).
 * 
 * Revision 2.19  92/05/26  11:26:45  pjg
 * 	Added call to memory_object_default_server in the vnode pager to
 * 	evaluate memory_object_create calls originating from remote
 * 	kernels.
 * 	[92/05/26            sjs]
 * 
 * Revision 2.18  92/05/24  14:21:04  pjg
 * 	Fix case of vstruct being left locked in vnode_pager_change_attributes.
 * 	Remove server_thread_register call from vnode_pager_slave.	
 * 		Add unix_master/release calls for NCPUS==1 (condict).
 * 	[92/05/19            roy]
 * 
 * Revision 2.17  92/05/18  12:32:05  roy
 * 	Revision 2.14.2.9  92/05/08  12:17:42  roy
 * 	Restrict vop_pageout amount in vs_page_write, if nec.
 * 	Fix assert in m_o_data_unlock.
 * 	Modified to export vnode pager port if export_paging is on (rabii)
 * 	[92/04/28             roy]
 * 
 * 	Revision 2.14.2.8  92/04/22  09:53:25  roy
 * 	Remove renaming of memory_object_* routines.
 * 	[92/03/29            roy]
 * 
 * 	Simplify logic in m_o_data_request by using new vs_page_read interface.
 * 	[92/03/29            roy]
 * 
 * 	Revision 2.14.2.7  92/03/27  10:24:35  roy
 * 	Set default copy strategy in vstruct for paging files.
 * 	[92/03/27            roy]
 * 
 * 	Revision 2.14.2.6  92/03/25  19:15:41  roy
 * 	Vstruct's vs_inited flag no longer needed; mem_obj_control != NULL
 * 	tells us when the object is active.  Use consistent naming
 * 	convention for externally callable routines:  vnode_pager_*().
 * 	[92/03/25            roy]
 * 
 * 	Revision 2.14.2.5  92/03/25  18:07:03  roy
 * 	Removed dead code and comments.
 * 	[92/03/25            roy]
 * 
 * 	Revision 2.14.2.4  92/03/25  08:41:34  roy
 * 	Change over to using memory_object_change_attributes.
 * 	[92/03/25            roy]
 * 
 * 	Revision 2.14.2.3  92/03/24  11:26:45  roy
 * 	Major rework of initialization, termination, setup, and no-senders
 * 	logic.  Added reference counting to vstructs.  Removed duplicate
 * 	information between v_vm_info and vstruct.  Paging file logic
 * 	is basically unaffected.
 * 	[92/03/24            roy]
 * 
 * 	Revision 2.14.2.2  92/03/18  11:44:02  roy
 * 	Pager ports named with address of vstruct is standard.  Removed
 * 	PAGER_PORT_ALIAS ifdefs.
 * 	[92/03/18            roy]
 * 
 * 	Revision 2.14.2.1  92/03/17  19:23:49  roy
 * 	Fixed vs_page_write for paging files.
 * 	Undid change whereby vrele() was being called from terminate.
 * 	Modified args to memory_object_lock_request (jeffrey).
 * 	[92/03/17            roy]
 * 
 * Revision 2.16  92/05/01  10:22:17  rabii
 * 	Put in fixes for page 0 proection bug from jose, also
 * 	set vnode_copy_strategy to MEMORY_OBJECT_COPY_TEMPORARY
 * 	for the pagingfile.
 * 
 * Revision 2.15  92/03/23  13:56:37  pjg
 * 	Fixed vs_page_write for paging files.
 * 	Undid change whereby vrele() was being called from terminate.
 * 	Modified args to memory_object_lock_request (jeffrey).
 * 	[92/03/17            roy]
 * 
 * Revision 2.14  92/03/15  14:42:30  roy
 * 	92/03/09  10:35:21  roy
 * 	Use new VOP_PAGEIN and VOP_PAGEOUT interfaces, for mappable files.
 * 	Change interface of vnode_flush_object to use a memory_object_t instead 
 * 	of vnode (durriya).
 * 
 * 	92/03/03  17:08:12  roy
 * 	Added vnode_uncache_object.  Multi-page writes supported .
 * 	for MAPPED_FILES.
 * 
 * Revision 2.13  92/03/09  11:58:13  durriya
 * 	92/02/25  17:48:20  condict
 * 	Change all calls to cthread_wire to ux_thread_wire, so ux_server_loop
 * 	can correctly compute required number of Mach kernel threads.
 * 
 * 	92/02/18  18:57:09  jose
 * 	Allowed anonymous object creation before swapon by
 * 	lazy-allocating paging file (bug 94).
 * 	Removed unix_master in memory_object_lock_completed (bug 96).
 * 
 * 	91/12/20  14:32:44  jose
 * 	Added port name aliasing for performance (port_name == &vstruct)
 * 	Changed returned value of vnode_pager_get for no paging file
 * 
 * 	91/12/17  17:18:56  jose
 * 	Updated to 1.0.3.
 * 
 * Revision 2.12  92/03/01  18:47:47  pjg
 * 	Removed unix_master in memory_object_lock_completed because
 * 	they cause deadlock in msync() (durriya).
 * 
 * Revision 2.11  92/02/11  22:24:41  pjg
 * 	Call vrele on the vnode in vnode_pager_no_senders() and not in
 * 	memory_object_terminate() (see comment in vnode_pager_no_senders()).
 * 
 * Revision 2.10  92/01/05  19:29:43  roy
 * 	91/12/30  16:21:47  roy
 * 	Call server_thread_register from vnode_pager_slave.
 * 
 * Revision 2.9  91/12/18  16:35:22  roy
 * 	Remove memory_objectPoly arg to memory_object_change_completed.
 * 
 * Revision 2.8  91/12/16  20:33:44  roy
 * 	91/12/03  17:41:37  jose
 * 	Fixed the a.out busy bug (bug 15).
 * 	Initialized the paging field in vs_create.
 * 
 * Revision 2.7  91/12/13  10:07:39  roy
 * 	91/10/14  20:53:54  roy
 * 	Convert to use m_o_data_supply.
 * 
 * 	91/12/04  16:43:41  roy
 * 	File size handling on pageout now handled below VOP interface.
 * 
 * 	91/11/19  09:59:32  roy
 * 	Merge latest changes from 1.0.2.
 * 
 * 	91/10/30  17:42:33  bernadat
 * 	In case of serialized file system with SER_COMPAT option
 * 	release master lock before invoking vnode pager.
 * 
 * 	91/10/24  16:43:15  jose
 * 	Removed ifdef OSF1_SERVER around old code
 * 
 * 	91/10/24  09:04:23  sp
 * 	in vnode_flush_object, only block the caller when data is written back 
 * 	to the pager.
 * 
 * 	91/10/23  16:38:14  condict
 * 	Add TIME LOCKS around use of the time var.
 * 
 * 	91/10/21  18:47:25  emcmanus
 * 	Conditionalise asserts that won't compile for the server.
 * 	Initialisation fixes from Jose.
 * 
 * 	91/10/18  17:15:37  jose
 * 	Added anonimous memory backup from OSF/1.
 * 	Also added one default port per vnode set to be able to flush
 * 	incoming messages correctly in a multithreaded environment.
 * 	Corrected several other minor bugs.
 * 
 * 	91/10/16  12:15:16  sp
 * 	don't hold locks while sending messages to the vnode pager. This causes
 * 	mount/umount to hang
 * 
 * 	91/10/15  12:16:45  sp
 * 	add locking to inode_uncache and inode_uncache_try
 * 
 * Revision 2.6  91/11/13  13:08:45  rabii
 * 	Added ANSI prototype definitions for some routines
 * 
 * Revision 2.5  91/10/14  20:36:38  roy
 * 	Fix revision history comments.
 * 
 * Revision 2.4  91/10/14  12:10:36  sjs
 * 	91/10/04  16:37:40  sp
 * 	Fix bug where objects were being uncached with 
 *	memory_object_set_attributes even if the object was not cached. 
 *	This caused an obscure thread hang in he mach kernel.
 * 
 * 	91/09/13  12:49:26  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE and stop using vm_page_size
 * 
 * 	91/09/10  12:01:04  barbou
 * 	Hack to bypass a bug in vm_map() when called with MEMORY_OBJECT_NULL.
 * 
 * 	91/09/06  13:48:56  condict
 * 	Add stub function memory_object_change_completed as req'd by MK60.	
 * 
 * 
 * Revision 2.3  91/10/04  14:55:26  chrisp
 * Get rid of references to msgh_kind field (this is now a sequence number).
 * This includes removal of the assert for MACH_MSGH_KIND_NOTIFICATION.
 * 
 * Revision 2.2  91/08/30  16:28:25  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.4  91/08/27  15:27:36  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.3  91/08/27  14:39:03  sp
 * Include kalloc.h for vm_pageable hack
 * 
 * Revision 3.2  91/06/27  15:48:34  sp
 * use OSF/1 zinit interface
 * 
 * Revision 3.1  91/06/25  17:03:49  condict
 * Turn of vnode pager debug messages.
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.0  91/06/25  14:40:55  jose
 *  Adapted from the original OSF/1 vnode pager
 * 
 * Revision 1.19.2.3  90/12/20  12:12:12  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.19.2.2  90/11/14  14:16:39  dlb
 * 	Fix a couple of locking glitches in previous fix.
 * 	[90/10/30  08:44:02  dlb]
 * 	Always lock vnode when manipulating vm_info components. [gmf]
 * 	Detect and back out of setup/setup race. [gmf]
 * 	Rewrite no_senders logic to eliminate setup/terminate race. [dlb]
 * 	[90/10/29  08:39:04  dlb]
 * 
 * Revision 1.18.1.4  90/10/25  13:28:30  dlb
 * 	Pass correct flags to spec_open.  Close resulting vnode if pager
 * 	file init fails for some other reason.
 * 	[90/10/24  17:10:44  dlb]
 * 
 * Revision 1.18.1.3  90/10/12  12:03:59  dlb
 * 	Resize pending write zone to 5000 entries.
 * 	[90/10/12  11:53:04  dlb]
 * 
 * Revision 1.18.1.2  90/10/09  11:59:41  dlb
 * 	Remove assertion from vnode_pager_allow_pageins, with explanation
 * 	(data_write_hook not called on object being terminated).
 * 	[90/10/09  11:47:08  dlb]
 * 
 * Revision 1.18  90/10/07  13:21:23  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:04:45  gm]
 * 
 * 	Remove two assertions that are no longer true for System V
 *      shared memory.
 * 	[90/09/28  14:35:33  dlb]
 * 	Rework short circuit logic to track pending pageouts here, instead of
 * 	leaving fictitious pages in the objects.  (1152)
 * 	Get System V shared memory support right. (1196)
 * 	[90/09/28  13:06:47  dlb]
 * 
 * 	This isn't exactly a fix to bug #372.  The bug was not repeatable.
 * 	Instead, it isolates any possible future occurances by using a
 * 	seperate zone for indirect blocks.
 * 	[90/09/21  11:03:44  jvs]
 * 
 * Revision 1.17  90/09/23  15:44:03  devrcs
 * 	Use singly linked list of bufs for swbuf and local simple_lock to
 * 	remove dependencies on struct buf and UNIX_LOCKS. Move wakeup and
 * 	enqueue code here from spec_vnops.c. Add lock on error to mpsleep.
 * 	Wrap all simple_locks in splbio().
 * 	[90/09/15  14:23:56  tmt]
 * 
 * 	Call spec_open() in pf_file_init() to get error checking at
 * 	swapon time when adding a device as a paging file.  Also,
 * 	added support to allow pageouts to a device to be asynchronous.
 * 	[90/09/11  06:57:26  ers]
 * 
 * 	In memory_object_create, make sure port is in hash table before
 * 	adding it to port set.
 * 	[90/09/05  14:56:16  ers]
 * 
 * Revision 1.16  90/09/13  11:42:58  devrcs
 * 	Fix swapon to use copyinstr() as it should.
 * 	[90/08/31  09:04:21  tmt]
 * 
 * 	Fix errnos for swapon syscall. Fill in pf_name to record file name.
 * 	[90/08/25  10:13:08  brezak]
 * 
 * 	Change mach_swapon to swapon. Turn off paging printf unless
 * 	vnode_pager_debug is on.
 * 	[90/08/22  18:14:23  brezak]
 * 
 * Revision 1.15  90/08/24  11:19:51  devrcs
 * 	Changed mach_swapon to use new system call interface.
 * 	Removed include of syscontext.h
 * 	[90/08/17  17:40:40  nags]
 * 
 * 	Race when activating pager could leak credentials.
 * 	[90/08/18  23:44:02  nags]
 * 
 * 	Fixes for IPC short-circuiting and paging to raw partitions.
 * 	[90/08/18  15:42:57  ers]
 * 
 * Revision 1.14  90/08/09  13:15:58  devrcs
 * 	Removed assert(!vs->paging) and printf from inode_pager_release().
 * 	[90/08/03  16:40:35  bet]
 * 
 * 	In memory_object_data_write (short-circuit case), it's not an error
 * 	if the vm_object associated with a memory object has disappeared.
 * 	[90/07/26  12:06:05  ers]
 * 
 * 	Added the msync system call.
 * 	[90/07/24  15:17:21  havens]
 * 
 * 	Don't increment errors on pagein (failed experiment).
 * 	[90/07/24  10:05:34  tmt]
 * 
 * Revision 1.13  90/07/27  08:45:30  devrcs
 * 	Added vnode_pager_data_request_direct for bypassing IPC on pagein.
 * 	[90/07/23  14:37:46  ers]
 * 
 * Revision 1.12  90/07/17  11:20:25  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:53:29  seiden]
 * 
 * 	Two bugs in pf_deallocate_page causing paging files to grow:
 * 	flipped sign in comparison and '=' instead of '=='.  Also,
 * 	unsigned comparison bug against hipage causes hipage not to
 * 	be maintained, so paging file is never truncated.
 * 	[90/07/03  10:01:19  lwa]
 * 
 * 	Fixed some bad tests in pf_deallocate_page.
 * 	[90/07/02  19:11:03  gm]
 * 
 * Revision 1.10  90/06/22  20:08:10  devrcs
 * 	nags merge
 * 
 * 	Condensed relevant history:
 * 	Parallelized for OSF/1					nags@encore.com
 * 	Secureware: least privilege, MAC, DAC,auditing		seiden@osf.org
 * 	Remove pageable flag from zinit argument list		jvs@osf.org
 *	Put back pageable flag in zinit argument list		jose@gr.osf.org
 * 	Increment vs->errros on pagein problems			tmt@osf.org
 * 	Increment vs->errors if pf_bmap() fails			ers@osf.org
 * 	place inode_uncache BEFORE mach_user_internal.h include	collins@osf.org
 * 	Added vnode_pager_get() for SysV shared memory.		bet@osf.org
 * 	Removed panic if (vs->paging) in memory_object_init()   bet@osf.org
 * 	Set u.u_error on errors from mach_swapon.		collins@osf.org
 * 	Changed syscall interface to inode pager.		jeffc@osf.org
 * 	New version of the inode pager.				collins@osf.org
 * 	    Put the default paging file support back in.
 * 	    Converted mach_swapon to use our new file system functions for
 * 	     name lookup and super-user check.
 * 	    Added pf_hint to speed the search of a paging file block.
 * 	    Added the function pf_map_extend to extend a vnode paging file
 * 	     map - this used to be done in-line in pf_bmap.
 * 	    Added support for the file system flags M_SWAP_PREFER and
 * 	     M_SWAP_NEVER.
 * 	    Restructured the file into a more logical order.  This included
 * 	     adding function headings and section headings.
 * 	    Added the vnode_port_hash_lock.  All of the vnode_port_hash
 * 	     functions are now multithread safe.
 * 	    Added the pager_file_lock.  All of the pf_* functions are now
 * 	     multithread safe.
 * 	    Changed names of functions and data structures from inode_xxx to
 * 	     vnode_xxx.
 * 	    Added missing <mach/vm_param.h> include.
 * 
 * 	    From rich.draves @ cmu.cs.edu
 * 
 * 	    Purged inode_pager_active, inode_pager_shutdown.
 * 
 * 	    Updated to new SERVER_LOOP interface.
 * 	    Updated to new vm_map_pageable interface.
 * 	    Fixed various unix_master/unix_release bugs.
 * 	    Added support for multiple threads.
 * 
 * 	    Revamped and simplified in the following ways:
 * 	    1)  Purged non-MACH_XP, non-MACH_VFS code.
 * 	    2)  Purged anonymous inode code; revised mach_swapon call.
 * 	    3)  Purged internal queue of data requests.
 * 	    4)  Simplified the no-senders detection.
 * 	    5)  Dropped support for multiple client kernels.
 * 
 * 	Condensed ancient history:
 * 	Picked up pager-file support (Peter King at NeXT).  (rfr)
 * 	Replaced VOP_PAGE_INIT/VOP_RDWR with VOP_PAGE_WRITE.  (jsb)
 * 	Use vm_set_default_memory_manager.  (mwyoung)
 * 	Fake no-senders detections with a "dead" ref count.  (mwyoung)
 * 	Allow multiple clients of a memory object.  (mwyoung)
 * 	Use memory_object_data_error.  (mwyoung)
 * 	Remember errors on a per-object basis. (mwyoung)
 * 	Added inode_pager_release(). (mwyoung)
 * 	Try to avoid deadlock when allocating data structures  (avie, mwyoung).
 * 	Try to printf rather than panic when faced with errors (avie).
 * 	"No buffer code" enhancements. (avie)
 * 	External paging version. (mwyoung, bolosky)
 * 	Allow pageout to ask whether a page has been written out.  (dbg)
 * 	Keep only a pool of in-core inodes.  (avie)
 * 	Use readahead when able. (avie)
 * 	Require that inode operations occur
 * 	 on the master processor (avie, rvb, dbg).
 * 	Combine both "program text" and "normal file" handling
 * 	 into one. (avie, mwyoung)
 * 	Allocate paging inodes on mounted filesystems (mja);
 * 	 allow preferences to be supplied (mwyoung).
 * 	[90/02/09  12:24:41  collins]
 * 
 * Revision 0.0  86/03/12            dbg
 * 	Created.
 * 	[86/03/12            dbg]
 * 
 * 	$EndLog$
 * 
 * $EndLog$
 */
/*
 *	File:	inode_pager.c
 *
 *	"Swap" pager that pages to/from Unix vnodes.  Also handles demand
 *	paging from files.
 */

#define VNODE_PAGER_ALT_THREADS

#include <fullserver.h>
#include <mach_nbc.h>
#include <mach_afs.h>
#include <mach_assert.h>

#include <sys/secdefines.h>

#include <mach_ldebug.h>
#include <kern/parallel.h>
#include <kern/zalloc.h>
#include <kern/queue.h>
#include <kern/mfs.h>

#include <sys/time.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <sys/mount.h>

#include <sys/buf.h>
#include <sys/errno.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/conf.h>
#include <sys/fcntl.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/table.h>

#include <ufs/inode.h>
#include <ufs/fs.h>
#include <nfs/nfsnode.h>
#if	MACH_AFS
#include <afs/osi.h>
#endif	/* MACH_AFS */

#include <builtin/inode_pager.h>
#ifdef VNODE_PAGER_ALT_THREADS
#include <builtin/alt_memory_object.h>
#endif /* VNODE_PAGER_ALT_THREADS */
#if SEC_BASE
#include <sys/security.h>
#endif

#include <uxkern/vm_param.h>
#include <builtin/ux_exception.h>

#include <cthreads.h>

extern mach_port_t privileged_host_port;

#ifndef	private
#define private
#endif

/*
 * NORMA systems have problems controlling paging flow among nodes.
 * Previously, NORMA systems regarded a pageout as complete when
 * a page left the original node.  However, a node providing paging
 * service could then be swamped trying to keep up with the pages
 * arriving from its clients.  We have introduced the OSF/1R1.1
 * interfaces (data_write_completed and set_attributes) and forced
 * all pagers to supply completion messages on pageout.  That way,
 * a kernel cleaning its memory can know when a pageout has finished
 * before inserting new pageouts into the paging tree.
 */
#if defined(i860) && !defined(OLD_SMALL_I860_THREAD_STATE)
#define	NORMA_VM	1
#else
#define	NORMA_VM	0
#endif


/*
 * Notes:
 *	If there is not enough space available in a paging file, we will
 * SILENTLY not write a page out.  This is due to the following
 * implementations:
 *
 *	pf_alloc - Does not guarantee that there is enough space available in
 *		   the allocated paging file for the memory object being
 *		   backed by that file.
 *
 *	memory_object_data_write - Does not check return value from
 *				   vs_page_write.
 *
 *	vs_page_write - Does not return an error indication if pf_bmap failed.*
 *	pf_bmap - Can fail because pf_alloc makes no guarantee that the paging
 *		  file will be large enough for the memory object.  An error
 *		  is returned from this level atleast...
 *
 * How is the vnode pager structure protected?  It seems that there can be
 * only one reference to each vstruct at a time.
 */

/*
 * Macros and static data related to paging files.
 */
#define PAGEMAP_THRESHOLD	512 
#define	PAGEMAP_ENTRIES		(PAGEMAP_THRESHOLD/sizeof(vm_offset_t))
#define	PAGEMAP_SIZE(npgs)	(npgs*sizeof(long))

#define	INDIRECT_PAGEMAP_ENTRIES(npgs) (((npgs-1)/PAGEMAP_ENTRIES) + 1)
#define INDIRECT_PAGEMAP_SIZE(npgs) (INDIRECT_PAGEMAP_ENTRIES(npgs) * sizeof(caddr_t))
#define INDIRECT_PAGEMAP(size)	(PAGEMAP_SIZE(size) > PAGEMAP_THRESHOLD)

#define RMAPSIZE(blocks) 	(howmany(blocks,NBBY))

lock_data_t	pager_file_lock;		/* Global lock on queues.   */
queue_head_t	pager_files;			/* Pager file queue.	    */
int		pager_file_count;		/* Number of active files.  */
						/*	initialized?	     */

/*
 * Vnode Pager Structure:  This is the structure used to manage vnode pager
 *	objects.  It is setup by vnode_pager_setup() and torn down by
 *      vnode_pager_no_senders().  It is reference counted, thus allowing
 *      callers into the vnode pager to access the structure without fear
 *      that it will be torn down out from under them.
 */
typedef struct vstruct {
	mach_port_t	mem_obj_control;/* Memory object's control port.     */
        int             refcnt;         /* Reference count.                  */
	lock_data_t	vs_lock;	/* Lock on this data structure	     */
	unsigned int	cacheable:1,	/* Cacheable after termination?	     */
			paging:1,	/* Is this a paging file?	     */
	                using_nms:1; 	/* Using no-senders for teardown?    */
        memory_object_copy_strategy_t
                        copy_strategy;  /* Copy strategy for this object.    */
	int		set;		/* Pager set number.		     */
	int             errors;         /* Pageout error count.              */
	int		vs_size;	/* Size of this chunk in pages.	     */
	pager_file_t	vs_pf;		/* Pager file this uses.	     */
	vm_offset_t	**vs_pmap;	/* Map of pages into paging file.    */
	struct vnode	*vp;		/* In memory vnode for this object.  */
	struct ucred	*cred;		/* Credentials.			     */
	mach_port_urefs_t urefs;	/* Number of urefs for request port  */
#if     MACH_ASSERT
	mach_port_t	pager;		/* Pager port.                       */
#endif
} *vnode_pager_t;

#define	VNODE_PAGER_NULL	((vnode_pager_t) 0)
zone_t		vstruct_zone;		/* The vstructure allocation zone.   */
zone_t		vindirect_zone;		/* The indirect block zone.          */


/*
 * Vnode Pager Set Data Structure:  This data structure is used to keep track
 *	of port sets that have been allocated for the Vnode Pager.  Memory
 *	object paging ports are randomly assigned to these port sets.
 *	Multiple port sets, then allows multiple concurrent IPCs to the vnode
 *	pager.  This allows a level of multithreading.  There are some
 *	problems with this type of multithreading - in particular the
 *	allocation of ports to port sets is static and thus set utilization is
 *	not necessarily balanced.
 *
 * NOTE:
 *	The ipset_count is manipulated without locks held.  This is not a
 *	problem for now as it is never used.  In otherwords, it may become
 *	incorrect, but we don't care.
 */
typedef struct vnode_pager_set {
	mach_port_t	ipset_set;	/* Port set name. */
	mach_port_t	ipset_port;	/* Default port for this set. */
	int		ipset_count;	/* Number of objects in the set. */
	cthread_t	ipset_thread;	/* Thread servicing the set. */
	vm_offset_t	ipset_buffer;	/* Input buffer. */
#ifdef VNODE_PAGER_ALT_THREADS
	mach_port_t	ipset_alt_port;	/* Alternate port for this set. */
	cthread_t	ipset_alt_thread;	
			/* Alternate thread servicing the alternate port. */
#endif /* VNODE_PAGER_ALT_THREADS */
} *vnode_pager_set_t;

struct	mutex	pager_set_lock;
vnode_pager_set_t vnode_pager_sets;
int		vnode_pager_num_sets = 5;	/* Level of multithreading */
int		max_pager_threads;		/* Total vnode pager threads */
int		vnode_pager_max_urefs = 10000;
#if	MACH_AFS
int		afs_pager_num_sets = 3;		/* afs multithreading */
#endif	/* MACH_AFS */


/*
 * Declarations for internal support routines.
 */
boolean_t       vnode_pager_notify_server();
any_t		vnode_pager_server_loop();
cthread_t	vnode_pager_start_slave();
void		vnode_pager_slave();
void            vnode_pager_bootstrap();
void            vnode_pager_add();
#ifdef VNODE_PAGER_ALT_THREADS
void		vnode_pager_alt_slave();
#endif /* VNODE_PAGER_ALT_THREADS */

/*
 * Declarations for routines supporting paging files.
 */
vnode_pager_t	pf_alloc();
vnode_pager_t	pf_pager_create();
pager_file_t	pf_lookup();
void		pf_dealloc();
vm_offset_t	pf_bmap();
pager_file_t	pf_find();
vm_offset_t	pf_allocate_page();
void		pf_deallocate_page();
int		pf_file_init();
int		pf_map_extend();

/*
 * Declarations for internal routines operating on vstructs.
 */
vnode_pager_t	vs_create();
void    	vs_ref();
void            vs_unref();
boolean_t	vs_page_check();
kern_return_t	vs_page_read();
void		vs_page_write();
void		vs_page_write_complete();
void		vs_page_wait();

/*
 * Declarations for externally callable routines.
 */
memory_object_t vnode_pager_ref();
void            vnode_pager_unref();
kern_return_t   vnode_pager_flush();
kern_return_t   vnode_pager_change_attributes();
memory_object_t inode_pager_setup();    
void            inode_pager_release();    
void            vnode_pager_no_senders();
memory_object_t vnode_pager_get();
void            inode_uncache();        
boolean_t       inode_uncache_try();    

/*
 * Static variables.
 */
boolean_t	vnode_pager_debug = FALSE;
task_t		vnode_pager_self;		/* My task ID.		     */
int		vnode_pager_pagein_count = 0;	/* Number of pagein requests */
int		vnode_pager_pageout_count = 0;  /*   "    " pageout   "      */
mach_port_t	vnode_pager_default = MACH_PORT_NULL;
int		vnode_pager_is_set = FALSE;	/* has a swapon been done? */

/*
 * Macros to convert from pager (memory object) ports to vstruct data
 * structures, and vice versa.
 */
#define PAGER_TO_VSTRUCT(mem_obj, vs)                                     \
        vs = (vnode_pager_t) (mem_obj);                                   \
        assert(vs && vs->pager == (mem_obj));        

#define VSTRUCT_TO_PAGER(vs)    ((memory_object_t) vs)

#if	NBBY == 8
#define BYTEMASK 0xff
#else
Define a byte mask for this machine.
#endif



/*
 * Acquire a reference to a pager object (i.e., vstruct) to guarantee that it 
 * won't disappear out from under a caller.  
 */
memory_object_t
vnode_pager_ref(vp)
        struct vnode    *vp;
{
	vnode_pager_t   vs;

	VN_LOCK(vp);
        if (vp->v_vm_info->pager == MACH_PORT_NULL) {
                VN_UNLOCK(vp);
                return (MEMORY_OBJECT_NULL);
        }

        PAGER_TO_VSTRUCT(vp->v_vm_info->pager, vs);
        lock_write(&vs->vs_lock);
        VN_UNLOCK(vp);
        vs_ref(vs);
	lock_done(&vs->vs_lock);
        return(VSTRUCT_TO_PAGER(vs));
}

/*
 * Release a reference to a pager object (i.e., vstruct).
 */
void
vnode_pager_unref(object)
        memory_object_t object;
{
	vnode_pager_t   vs;

        PAGER_TO_VSTRUCT(object, vs);
        vs_unref(vs);
}

/*
 * This routine affects data stored in the kernel's memory object.
 * A caller may cause memory object data to be flushed (discarded),
 * cleaned (dirty pages written back), or both.
 *
 * If the object is marked temporary, no cleaning will be performed
 * even if the should_clean arg is true.
 *
 * NOTE: This routine may only be invoked by a caller that guarantees the
 * vnode pager knows about the pager object (i.e., has a vstruct).  
 * A caller can guarantee this by first obtaining a reference to the 
 * object via vnode_pager_ref(), or by holding a send right to the 
 * object (e.g., a send right returned by vnode_pager_setup).
 */
kern_return_t 
vnode_pager_flush(object, offset, size, wait, should_clean, should_flush,
                   lock_value)
        memory_object_t object;
	vm_offset_t	offset;
	vm_size_t	size;
	int		wait;
	boolean_t	should_clean, should_flush;
	vm_prot_t	lock_value;
{
	int flag;
	int error;
	vnode_pager_t vs;
	boolean_t should_wait;
	mach_port_t mem_obj_control;
	memory_object_return_t	what_to_return;
	
	PAGER_TO_VSTRUCT(object, vs);
	lock_write(&vs->vs_lock);
	
        if (vs->mem_obj_control == MACH_PORT_NULL) {
                /* object not active */
		lock_done(&vs->vs_lock);
		return KERN_SUCCESS;
	}

	/*
	 * Don't clean if the object is marked temporary.
	 */
	if (should_clean && vs->copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
		if (!should_flush) {
			lock_done(&vs->vs_lock);
			return KERN_SUCCESS;	/* nothing to do */
		}
		should_clean = FALSE;
	}
		
	if (should_clean) {
                assert(vs->vp);
		what_to_return = MEMORY_OBJECT_RETURN_DIRTY;
	} else
		what_to_return = MEMORY_OBJECT_RETURN_NONE;

	should_wait = (should_clean || wait == 1) ? TRUE : FALSE;
	if (should_wait) {
		assert_wait((int) vs, FALSE);
#if	NCPUS == 1
		/*
		 * Unlock if we'll block on a vnode pager
		 * thread (because it needs the lock).
		 */
		if (u.uu_master_lock) 
			master_unlock();
#endif	
	}

        /*
         * It's possible the memory object is in the process of being
         * terminated, meaning the recipient of this message may
         * be the pager itself (who won't handle it and hence won't do
         * a thread_wakeup).  This is solved by having m_o_terminate 
         * do a thread_wakeup.
	 *
	 * XXX Holding the vs_lock while calling m_o_lock_request can
	 * deadlock the system becasue m_o_data_write also wants to
	 * get the vs_lock.  This should be fixed instead by using
	 * a separate lock on the paging path.
         */
	mem_obj_control = vs->mem_obj_control;
        lock_done(&vs->vs_lock);
	error = memory_object_lock_request(vs->mem_obj_control, 
					   offset, size, what_to_return, 
					   should_flush, lock_value,
					   should_wait ? object : 
					   MACH_PORT_NULL);

	if (error) {
		if (should_wait)
			clear_wait(current_thread(), 0, FALSE);
	        if (error == MACH_SEND_INVALID_DEST)
	                /*
                         * Possible that memory object was terminated
                         * after lock_done call above.
	                 */
	                return(KERN_SUCCESS);
                else {
                        printf("m_o_lock_request error=0x%x\n", error);
			return(error);
		}
	}

	if (should_wait) {
		thread_block();
#if	NCPUS == 1
		if (u.uu_master_lock)
			master_lock();
#endif
	}

	if (should_clean) {
                /* Now flush the data to disk. */
                flag = wait ? MNT_WAIT : 0;
                VOP_FSYNC(vs->vp, FWRITE, vs->cred, flag, error);
                if (error) {
			printf("VOP_FSYNC error=0x%x\n", error);
                        return KERN_MEMORY_ERROR;
		}
        }

	return KERN_SUCCESS;
}

/*
 * Change the attributes of a memory object.  
 * 
 * The caller may choose to wait until the operation is known to have
 * completed.  In the case of setting cacheable to FALSE, specifying 'wait'
 * guarantee that the kernel has relinquished its data if it could 
 * (it's possible it couldn't if the data is currently accessible 
 * - i.e., mapped).
 *
 * NOTE: This routine may only be invoked by a caller that guarantees the
 * vnode pager knows about the pager object (i.e., has a vstruct).  
 * A caller can guarantee this by first obtaining a reference to the 
 * object via vnode_pager_ref(), or by holding a send right to the 
 * object (e.g., a send right returned by vnode_pager_setup).
 *
 * XXX this is currently not the case with irefresh in vfs_vnops.c and
 * vflush in vfs_subr.c.  They do not prevent vnode recycling.
 */
kern_return_t
vnode_pager_change_attributes(object, cacheable, temporary, wait)
        memory_object_t         object;
        boolean_t               *cacheable;
        boolean_t               *temporary;
	boolean_t		wait;
{
	vnode_pager_t	vs;
        boolean_t       cur_cacheable, new_cacheable;
	kern_return_t	err;
	mach_port_t 	mem_obj_control;
        memory_object_copy_strategy_t   new_copy_strat;

	PAGER_TO_VSTRUCT(object, vs);
	lock_write(&vs->vs_lock);

        cur_cacheable = vs->cacheable ? TRUE : FALSE;
        if (cacheable) 
                new_cacheable = *cacheable;
        else
                new_cacheable = cur_cacheable;

        if (temporary) 
                new_copy_strat = *temporary ? MEMORY_OBJECT_COPY_TEMPORARY :
                        MEMORY_OBJECT_COPY_DELAY;
        else
                new_copy_strat = vs->copy_strategy;

        if (new_cacheable != cur_cacheable || 
            new_copy_strat != vs->copy_strategy) {
                /*
                 * Attribute change required.  If the object is not 
                 * active then just record the info in the vstruct.
                 * Otherwise, tell the kernel as well.  
                 */
                vs->cacheable = new_cacheable ? 1 : 0;
                vs->copy_strategy = new_copy_strat;

                if (vs->mem_obj_control != MACH_PORT_NULL) {
                        if (wait) {
                                assert_wait((int) vs, FALSE);
#if	NCPUS == 1
				/*
				 * Unlock if we'll block on a vnode pager
				 * thread (because it needs the lock).
				 */
				if (u.uu_master_lock) 
					master_unlock();
#endif	
			}

                        /*
                         * It's possible the memory object is in the process of 
                         * being terminated, meaning the recipient of this 
                         * message may be the pager itself (who won't handle 
                         * it and hence won't do a thread_wakeup).  This is 
                         * solved by  having m_o_terminate do a thread_wakeup.
			 *
			 * XXX Holding the vs_lock while calling 
			 * m_o_change_attributes can deadlock the system 
			 * becasue m_o_data_write also wants to get the
			 * vs_lock.  This should be fixed instead by using
			 * a separate lock on the paging path.
                         */
			mem_obj_control = vs->mem_obj_control;
                        lock_done(&vs->vs_lock);
			err = memory_object_change_attributes(
					      mem_obj_control,
                                              new_cacheable,
                                              new_copy_strat,
                                              wait ? object : 
                                              MACH_PORT_NULL);

                        if (err) {
                                printf("Change_attributes error=0x%x\n",err);
                                if (wait) {
                                        clear_wait(current_thread(), 0, FALSE);
#if	NCPUS == 1
					if (u.uu_master_lock)
						master_lock();
#endif
				}
                                return(err);
                        }
                        if (wait) {
                                thread_block();
#if	NCPUS == 1
				if (u.uu_master_lock)
					master_lock);
#endif
			}
			return KERN_SUCCESS;
                }

	}

        lock_done(&vs->vs_lock);
        return KERN_SUCCESS;
}

int     inode_uncache_cnt = 0;
int     inode_uncache_try_cnt = 0;

void
inode_uncache(vp)
	struct vnode	*vp;
{
        memory_object_t         object;
        boolean_t               cacheable = FALSE;

        inode_uncache_cnt++;

        /*
         * Get a reference to the object to ensure it doesn't disappear
         * out from under us.
	 */
        if ((object = vnode_pager_ref(vp)) != MEMORY_OBJECT_NULL) {
                (void) vnode_pager_change_attributes(object, &cacheable, 
                                                     NULL, FALSE);
                vnode_pager_unref(object);
        }
}

boolean_t
inode_uncache_try(vp)
	struct vnode	*vp;
{
        memory_object_t         object;
        boolean_t               cacheable = FALSE;

        inode_uncache_try_cnt++;

        /*
         * Get a reference to the object to ensure it doesn't disappear
         * out from under us.
	 */
        if ((object = vnode_pager_ref(vp)) != MEMORY_OBJECT_NULL) {
                (void) vnode_pager_change_attributes(object, &cacheable, 
                                                     NULL, TRUE);
                vnode_pager_unref(object);
        }

	/*
	 * Now we can check if the object has been terminated.
	 */
	VN_LOCK(vp);
        if (vp->v_vm_info->pager == MACH_PORT_NULL) {
                VN_UNLOCK(vp);
                return (TRUE);
        } else {
                VN_UNLOCK(vp);
                return (FALSE);
        } 
}

/*
 *	Routine:	vnode_pager_setup
 *	Function:
 *		Allocate and initialize a memory object for the specified
 *		vnode.  This object may be used in subsequent vm_map calls.
 *
 *
 *	Arguments:
 *		vp - Pointer to the vnode to create a memory object for.
 *		is_text - An indication that the vnode is a text image.
 *		can_cache - An indication that the kernel can cache data from
 *			    this memory object.
 *
 *	Note:
 *		When the memory object returned by this call is no longer
 *		needed (e.g., it has been mapped into the desired address
 *		space), it should be deallocated using vnode_pager_release.
 *
 *		This call does not run in the context of the vnode_pager task,
 *		and therefore must translate the ports it gets itself.
 *
 *		Synchronization (MP):  The vnode lock protects the vm_info
 *		structure.  If both the vnode and port locks must be taken,
 *		the order is vnode lock, then port lock.
 */
memory_object_t
inode_pager_setup(vp, is_text, can_cache)
	struct vnode	*vp;
	boolean_t	is_text;
	boolean_t	can_cache;
{
#if	FULLSERVER
	vnode_pager_t	vs;
	kern_return_t	ret;
        mach_port_t     previous;
        memory_object_t pager;

	ASSERT(vp != NULLVP);
	VN_LOCK(vp);
	if (is_text)
		vp->v_flag |= VTEXT;

loop:
	/*
	 * If the vnode does not already have a memory object, then allocate
	 * one.  Otherwise, just allocate a new send right.
	 */
	if (vp->v_vm_info->pager == MEMORY_OBJECT_NULL) {
                VN_UNLOCK(vp);

                /* Check to make sure this isn't in use as a pager file. */
                if (pf_lookup(vp) != PAGER_FILE_NULL) {
                        uprintf("Can't page directly to a paging file.\n");
                        return(MEMORY_OBJECT_NULL);
                }

                /*
                 * Allocate a vstruct data structure to handle the EMMI.
                 * Note that this takes a vref on the vnode.
                 */
                vs = vs_create(vp);
                pager = VSTRUCT_TO_PAGER(vs);

                /*
                 * Allocate a port with the name chosen by vs_create.
                 */
                if ((ret = mach_port_allocate_name(vnode_pager_self, 
                                            MACH_PORT_RIGHT_RECEIVE,
                                            pager)) != KERN_SUCCESS)
			panic("vnode_pager_setup: can't alloc port name=0x%x "
					"ret=0x%x", pager, ret);

                if (mach_port_request_notification(vnode_pager_self, pager,
                                                   MACH_NOTIFY_NO_SENDERS, 1,
                                                   pager,
                                                   MACH_MSG_TYPE_MAKE_SEND_ONCE,
                                                   &previous) != KERN_SUCCESS)
                        panic("vnode_pager_setup: request_notify");

                /*
                 * Allocate a send right.
                 */
                if (mach_port_insert_right(mach_task_self(),
                                           pager, pager, 
                                           MACH_MSG_TYPE_MAKE_SEND)
                        != KERN_SUCCESS)
                        panic("vnode_pager_setup: can't allocate send right");

                /*
                 * Add the vstruct to a port set. 
                 */
                vnode_pager_add(vs);

                /*
                 * Check to see if we raced with another thread in this code.
                 */
                VN_LOCK(vp);
                if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL) {
                        /*
                         * Someone beat us to it!  Clean up and try again.
                         * Unref the vstruct and destroy the port (all rights).
                         * We don't need to do anything to undo the 
                         * vnode_pager_add.
                         */
                        VN_UNLOCK(vp);
                        mach_port_destroy(mach_task_self(), pager);
                        vs_unref(vs);
                        VN_LOCK(vp);
                        goto loop;
                }

                /*
                 * Setup v_vm_info and vstruct structures.
                 */
                vp->v_vm_info->pager = pager;
                vs->cacheable = can_cache ? 1 : 0;
		vs->using_nms = 1;	/* using no-senders for teardown */

                /* XXX Consider passing a 'temporary' arg to this routine. */
                vs->copy_strategy = MEMORY_OBJECT_COPY_DELAY;  
                vp->v_vm_info->mscount = 1;		
                VN_UNLOCK(vp);

	} else {
                /*
                 * If there is already a memory object for this vnode, create
                 * an additional send right to be returned to the caller.
                 * Increment the make-send count to allow vnode_pager_no_senders
                 * to handle the case where a no-more-senders message has 
                 * already been generated.
                 *
                 * We hold the vnode lock throughout to prevent vstruct
                 * tear down (see vnode_pager_no_senders).
                 */
                pager = vp->v_vm_info->pager;
                if ((mach_port_insert_right(mach_task_self(), pager, pager,
                                            MACH_MSG_TYPE_MAKE_SEND)) 
                        != KERN_SUCCESS)
                        panic("vnode_pager_setup: can't acquire send rights");

                vp->v_vm_info->mscount++;		
                VN_UNLOCK(vp);

                /*
                 * The vstruct's creds field points to the creds of the
                 * last thread to call inode_pager_setup for a vnode 
                 * (i.e., a crock).
                 * The creds are only needed for the write path so that
                 * it can clear setuid and setgid bits if the writer isn't  
                 * privileged.
                 */
                PAGER_TO_VSTRUCT(pager, vs);
                lock_write(&vs->vs_lock);
		/*
		 * Free up the previous credentials.
		 */
                if (vs->cred != NULL)
			crfree(vs->cred);
		crhold(u.u_cred);
		vs->cred = u.u_cred;
                lock_done(&vs->vs_lock);
	}

	return(pager);
#else	/* FULLSERVER */
	panic("inode_pager_setup");
#endif	/* FULLSERVER */
}

/*
 *	Routine:	vnode_pager_release
 *	Purpose:
 *		Relinquish any references or rights that were associated with
 *		the result of a call to vnode_pager_setup.
 *
 *	Arguments:
 *		object - The memory object to release.
 *
 *	Note:
 *		This call, like vnode_pager_setup, does not run	in the context
 *		of the vnode_pager. 
 */
void
inode_pager_release(object)
	memory_object_t	object;
{
        (void) mach_port_deallocate(vnode_pager_self, object);
}

/*
 *	Routine:	vnode_pager_no_senders
 */
void 
vnode_pager_no_senders(pager, mscount)
	memory_object_t	pager;
	mach_port_mscount_t mscount;
{
#if	FULLSERVER
	register vnode_pager_t	vs;
	register struct vnode	*vp;
	mach_port_mscount_t 	ourcount;
        mach_port_type_t        ptype;
        mach_port_t             previous;

        /*
         * Only one no-more-senders can be in progress at a time for an object.
         * => vstruct cannot be torn down out from under us
         */
	PAGER_TO_VSTRUCT(pager, vs);

        /* object must not be active */
        assert(vs->mem_obj_control == MACH_PORT_NULL);   

	/* object must be using no-more-senders msgs for teardown */
	assert(vs->using_nms);

        if (vs->paging) {
                (void) mach_port_mod_refs(vnode_pager_self, pager,
                                          MACH_PORT_RIGHT_RECEIVE, -1);
                assert(mach_port_type(vnode_pager_self, pager, &ptype) ==
                       KERN_INVALID_NAME);
		pf_dealloc(vs);
                return;
        }

        vp = vs->vp;
        assert(vp);

	/*
	 * We might not have processed the no-senders notification
	 * in time to keep inode_pager_setup from creating more
	 * send rights for this memory_object.  Hence, use the mscount
	 * to determine if we should just register for another
	 * no-senders notification.
	 */
        VN_LOCK(vp);
	ourcount = vp->v_vm_info->mscount;
	if (mscount < ourcount) {
                VN_UNLOCK(vp);
		if ((mach_port_request_notification(
				vnode_pager_self, pager,
				MACH_NOTIFY_NO_SENDERS, ourcount,
				pager, MACH_MSG_TYPE_MAKE_SEND_ONCE,
				&previous) != KERN_SUCCESS) ||
		    (previous != MACH_PORT_NULL))
			panic("vnode_pager_no_senders: request_notify");
		return;
	} else if (mscount > ourcount) {
		printf("ourcount=%d mscount=%d ", ourcount, mscount);
		panic("vnode_pager_no_senders: mscount too big");
	}

        /*
         * Clear the pager port to prevent others (such as vnode_pager_flush)
         * from grabbing it.  
         */
	vp->v_vm_info->pager = MEMORY_OBJECT_NULL;
        VN_UNLOCK(vp);

	/*
	 * Destroy the pager port.
	 */
	(void) mach_port_mod_refs(vnode_pager_self, pager,
				  MACH_PORT_RIGHT_RECEIVE, -1);
        assert(mach_port_type(vnode_pager_self, pager, &ptype) == 
               KERN_INVALID_NAME);

        vs_unref(vs);
#else	/* FULLSERVER */
	panic("vnode_pager_no_senders");
#endif	/* FULLSERVER */
}

#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t
alt_vnode_pager_no_senders(mach_port_t alt_port,
		           memory_object_t pager, 
		           mach_port_mscount_t mscount)
#else
kern_return_t
alt_vnode_pager_no_senders(alt_port, pager, mscount)
	memory_object_t	pager;
	mach_port_mscount_t mscount;
#endif /* __STDC__ */
{
	vnode_pager_no_senders(pager, mscount);
	
	return KERN_SUCCESS;
}
#endif /* VNODE_PAGER_ALT_THREADS */

/*
 *	Routine:	vnode_pager_get
 *	Function:
 *		Allocate a paging file and initialize a memory object for it.
 *		This object may be used in subsequent vm_map calls.
 *
 *
 *	Arguments:
 *		can_cache - An indication that the kernel can cache data from
 *			    this memory object.
 *		size - The size of the memory object to allocate paging space
 *		       for.
 *	Note:
 *		When the memory object returned by this call is no longer
 *		needed (e.g., it has been mapped into the desired address
 *		space), it should be deallocated using vnode_pager_release.
 *
 *		This call does not run in the context of the vnode_pager task,
 *		and therefore must translate the ports it gets itself.
 *		(In the context of the OSF/1 server this comment is no longer
 *		valid).
 *
 */
#if	FULLSERVER
memory_object_t
vnode_pager_get(can_cache, size)
	boolean_t	can_cache;
	vm_size_t	size;
{
	vnode_pager_t	vs;
        memory_object_t pager;
	mach_port_t	previous;
	kern_return_t	ret;

	/*
	 * Allocate a paging file.
	 * Report any problems to the console.
	 */
	vs = pf_alloc(size);
	if (vs == VNODE_PAGER_NULL) {
		printf("(vnode_pager)get: unable to allocate");
		printf(" vnode_pager structure [SUGGEST RUN swapon]\n");
		return(MEMORY_OBJECT_NULL);
	}		
	vs->cacheable = can_cache ? 1 : 0;
	vs->using_nms = 1;	/* using no-senders for teardown */

        /* XXX Consider passing a 'temporary' arg to this routine. */
        vs->copy_strategy = MEMORY_OBJECT_COPY_TEMPORARY;  

        pager = VSTRUCT_TO_PAGER(vs);

        /*
         * Allocate a port with the name chosen by pf_alloc.
         */
	if ((ret = mach_port_allocate_name(vnode_pager_self,
			MACH_PORT_RIGHT_RECEIVE, pager)) != KERN_SUCCESS)
		panic("vnode_pager_get: can't alloc port name=0x%x ret=0x%x",
				pager, ret);

	if (mach_port_request_notification(vnode_pager_self, pager,
					   MACH_NOTIFY_NO_SENDERS, 1,
					   pager,
					   MACH_MSG_TYPE_MAKE_SEND_ONCE,
					   &previous) != KERN_SUCCESS)
		panic("vnode_pager_get: request_notify");

        /*
         * Allocate a send right for our caller.
         */
	if (mach_port_insert_right(vnode_pager_self, pager, pager,
				   MACH_MSG_TYPE_MAKE_SEND)
	    != KERN_SUCCESS)
	    panic("vnode_pager_get: can't acquire send rights");

        /*
         * Add the vstruct to a port set. 
         */
        vnode_pager_add(vs);

	return((memory_object_t) pager);
}



/*
 *	Routine:	memory_object_init (vnode_pager_init)
 *	Function:
 *		Initialize the specified memory object.  The object has been
 *		previously created and has an associated vnode pager structure.
 *		This function serves as a notification that the object is now
 *		in use.  In addition a cache management request port is
 *		specified.
 *
 *	Arguments:
 *		mem_obj - The port that represents the memory object data.
 *		mem_obj_control - The port that the memory manager uses to
 *			control the use of its data by the kernel.
 *		mem_obj_name - The port that the kernel will use to identify
 *			this memory object to other tasks.
 *		page_size - The kernel's page size.
 */
#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t
orig_memory_object_init(mach_port_t mem_obj, 
		        mach_port_t mem_obj_control, 
		        mach_port_t mem_obj_name, 
		        vm_size_t page_size)
#else
kern_return_t
orig_memory_object_init(mem_obj, mem_obj_control, mem_obj_name, page_size)
	mach_port_t		mem_obj;
	mach_port_t		mem_obj_control;
	mach_port_t		mem_obj_name;
	vm_size_t		page_size;
#endif /* __STDC__ */
#else /* VNODE_PAGER_ALT_THREADS */
#if __STDC__ == 1
kern_return_t
memory_object_init(mach_port_t mem_obj, 
		   mach_port_t mem_obj_control, 
		   mach_port_t mem_obj_name, 
		   vm_size_t page_size)
#else
kern_return_t
memory_object_init(mem_obj, mem_obj_control, mem_obj_name, page_size)
	mach_port_t		mem_obj;
	mach_port_t		mem_obj_control;
	mach_port_t		mem_obj_name;
	vm_size_t		page_size;
#endif /* __STDC__ */
#endif /* VNODE_PAGER_ALT_THREADS */
{
	vnode_pager_t	vs;

	if (vnode_pager_debug)
		printf("(vnode_pager)init: mem_obj=%d, request=%d, name=%d\n",
		       mem_obj, mem_obj_control, mem_obj_name);

	assert(mem_obj_control != MACH_PORT_NULL);
	assert(mem_obj_name != MACH_PORT_NULL);
	assert(page_size == PAGE_SIZE);

	/*
	 * In Mach 3.0, we don't need to worry about init/terminate races.
	 */
	PAGER_TO_VSTRUCT(mem_obj, vs);
	assert(vs->mem_obj_control == MACH_PORT_NULL);
	assert(vs->urefs == 0);

	lock_write(&vs->vs_lock);
	vs->mem_obj_control = mem_obj_control;
	vs->urefs = 1;

	/*
	 * Reply to the kernel:  the memory object is ready.
         * Hold the vstruct lock to guarantee that the kernel and
         * server have the same notion of cacheability and copy strategy.
	 */
#if	!NORMA_VM
        if (memory_object_set_attributes(mem_obj_control, TRUE, vs->cacheable, 
                                         vs->copy_strategy)
						!= KERN_SUCCESS)
		panic("(vnode_pager)init: set_attributes");
#else
	/*
	 *	Pagers must handshake using m_o_d_write_completed;
	 *	thus, two of the OSF/1R1.1 VM interfaces
	 *	(data_write_completed and set_attributes) have found
	 *	their way into OSF/1 AD.
	 */
        if (memory_object_set_attributes(mem_obj_control, TRUE, vs->cacheable, 
					 TRUE, vs->copy_strategy, page_size)
	    != KERN_SUCCESS)
		panic("(vnode_pager)init: set_attributes");
#endif

	lock_done(&vs->vs_lock);

	(void) mach_port_deallocate(vnode_pager_self, mem_obj_name);

	return(KERN_SUCCESS);
}

#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t
memory_object_init(mach_port_t mem_obj, 
		   mach_port_t mem_obj_control, 
		   mach_port_t mem_obj_name, 
		   vm_size_t page_size)
#else
kern_return_t
memory_object_init(mem_obj, mem_obj_control, mem_obj_name, page_size)
	mach_port_t		mem_obj;
	mach_port_t		mem_obj_control;
	mach_port_t		mem_obj_name;
	vm_size_t		page_size;
#endif /* __STDC__ */
{
	kern_return_t ret;
	
	ret = do_alt_memory_object_init((mach_port_t) cthread_self(), 
					 mem_obj, mem_obj_control,
					 mem_obj_name, page_size);
	if (ret != KERN_SUCCESS)
		panic("memory_object_init: do_alt_memory_object_init failed(0x%x)\n", ret);
	
	return KERN_SUCCESS;
}

#if __STDC__ == 1
kern_return_t
alt_memory_object_init(mach_port_t alt_port,
		       mach_port_t mem_obj, 
		       mach_port_t mem_obj_control, 
		       mach_port_t mem_obj_name, 
		       vm_size_t page_size)
#else
kern_return_t
alt_memory_object_init(alt_port, mem_obj, mem_obj_control, 
		       mem_obj_name, page_size)
	mach_port_t		alt_port;
	mach_port_t		mem_obj;
	mach_port_t		mem_obj_control;
	mach_port_t		mem_obj_name;
	vm_size_t		page_size;
#endif /* __STDC__ */
{
	return orig_memory_object_init(mem_obj, mem_obj_control,
				       mem_obj_name, page_size);
}
#endif /* VNODE_PAGER_ALT_THREADS */

/*
 *	Routine:	memory_object_terminate (vnode_pager_terminate)
 *	Function:
 *		This function is called when all address space mappings of a
 *		particular memory object are deallocated.  The receive rights
 *		to the control and name ports are included in this call.  This
 *		allows the memory manager to destroy those ports.  When the
 *		kernel calls this function, it also releases its send rights
 *		to the three ports.
 *
 *	Arguments:
 *		mem_obj - The port that represents the memory object data.
 *		mem_obj_control - The port that the memory manager uses to
 *			control the use of its data by the kernel.
 *		mem_obj_name - The port that the kernel will use to identify
 *			this memory object to other tasks.
 */
#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t
orig_memory_object_terminate(mach_port_t mem_obj, 
		             mach_port_t mem_obj_control, 
		             mach_port_t mem_obj_name)
#else
kern_return_t
orig_memory_object_terminate(mem_obj, mem_obj_control, mem_obj_name, page_size)
	mach_port_t		mem_obj;
	mach_port_t		mem_obj_control;
	mach_port_t		mem_obj_name;
#endif /* __STDC__ */
#else /* VNODE_PAGER_ALT_THREADS */
#if __STDC__ == 1
kern_return_t
memory_object_terminate(mach_port_t mem_obj, 
		        mach_port_t mem_obj_control, 
		        mach_port_t mem_obj_name)
#else
kern_return_t
memory_object_terminate(mem_obj, mem_obj_control, mem_obj_name)
	mach_port_t		mem_obj;
	mach_port_t		mem_obj_control;
	mach_port_t		mem_obj_name;
#endif /* __STDC__ */
#endif /* VNODE_PAGER_ALT_THREADS */
{
        struct vnode    	*vp;
	vnode_pager_t		vs;
        mach_port_type_t        ptype;
        
	if (vnode_pager_debug)
		printf("(vnode_pager)terminate: mem_obj=%d, request=%d, name=%d\n",
		       mem_obj, mem_obj_control, mem_obj_name);

	assert(mem_obj_control != MACH_PORT_NULL);
	assert(mem_obj_name != MACH_PORT_NULL);

	/*
	 * In Mach 3.0, we don't need to worry about init/terminate races.
	 */
	PAGER_TO_VSTRUCT(mem_obj, vs);
	assert(vs->mem_obj_control == mem_obj_control);

	if (!vs->paging) {
		vp = vs->vp;
		VN_LOCK(vp);
		/*
		 * When the object is terminated, no more mappings exist and we
		 * we can clear the text flag.  Unfortunately, sometimes this 
		 * flag won't get cleared even if the file's not being executed 
		 * (consider an appl. that has the file mmap'd).  Also, it
		 * doesn't handle the race with exec having done a 
		 * vnode_pager_setup but not yet having mapped the file.  
		 * And, it doesn't handle executable code that has been 
		 * mmap'd rather than exec'd (e.g., in the case of 
		 * dynamically linked libraries).
		 * 
		 * By the way, we clear the text flag here rather than in 
		 * vnode_pager_no_senders because a no-senders message isn't 
		 * guaranteed to arrive before a 
		 * memory_object_change_completion message (which comes in 
		 * on behalf of vnode_pager_change_attributes code).
		 */
		vp->v_flag &= ~VTEXT;	     
		VN_UNLOCK(vp);
	}

	lock_write(&vs->vs_lock);

	vs->mem_obj_control = MACH_PORT_NULL;
	vs->urefs = 0;
	thread_wakeup((int) vs);        /* in case someone is waiting */
	lock_done(&vs->vs_lock);

	/*
	 * If not using no-senders for vstruct tear down, then we must
	 * do it now.
	 */
        if (!vs->using_nms) {
		assert(vs->paging);
                (void) mach_port_mod_refs(vnode_pager_self, mem_obj,
                                          MACH_PORT_RIGHT_RECEIVE, -1);
                assert(mach_port_type(vnode_pager_self, mem_obj, &ptype) ==
                       KERN_INVALID_NAME);
		pf_dealloc(vs);
        }

        if (mach_port_destroy(vnode_pager_self, mem_obj_control))
                panic("m_o_terminate.destroy_cntl");

        if (mach_port_destroy(vnode_pager_self, mem_obj_name))
                panic("m_o_terminate.destroy_name");

	return KERN_SUCCESS;
}

#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t
memory_object_terminate(mach_port_t mem_obj, 
		        mach_port_t mem_obj_control, 
		        mach_port_t mem_obj_name)
#else
kern_return_t
memory_object_terminate(mem_obj, mem_obj_control, mem_obj_name)
	mach_port_t		mem_obj;
	mach_port_t		mem_obj_control;
	mach_port_t		mem_obj_name;
#endif /* __STDC__ */
{
	kern_return_t ret;
	
	ret = do_alt_memory_object_terminate((mach_port_t) cthread_self(), 
					     mem_obj, mem_obj_control,
					     mem_obj_name);
	if (ret != KERN_SUCCESS)
		panic("memory_object_terminate: do_alt_memory_object_terminate failed(0x%x)\n", ret);
	
	return KERN_SUCCESS;
}

#if __STDC__ == 1
kern_return_t
alt_memory_object_terminate(mach_port_t alt_port,
		            mach_port_t mem_obj, 
		            mach_port_t mem_obj_control, 
		            mach_port_t mem_obj_name)
#else
kern_return_t
alt_memory_object_terminate(alt_port, mem_obj, mem_obj_control, mem_obj_name)
	mach_port_t		alt_port;
	mach_port_t		mem_obj;
	mach_port_t		mem_obj_control;
	mach_port_t		mem_obj_name;
#endif /* __STDC__ */
{
	return orig_memory_object_terminate(mem_obj, mem_obj_control, mem_obj_name);
}
#endif /* VNODE_PAGER_ALT_THREADS */


/*
 *	vnode_pager_check_request:
 *
 *	Called by those functions (data_request, data_write, etc)
 *	which are receiving send rights for the mem_obj_control port.
 */

vnode_pager_t
vnode_pager_check_request(mem_obj, mem_obj_control)
	mach_port_t	mem_obj;
	mach_port_t	mem_obj_control;
{
	register vnode_pager_t vs;

	PAGER_TO_VSTRUCT(mem_obj, vs);
	assert(vs->mem_obj_control == mem_obj_control);
	assert(vs->urefs > 0);

        /*
	 * XXX shouldn't be needed because object requests are 
	 *  single-threaded 
	 */
	lock_write(&vs->vs_lock);       

	if (++vs->urefs > vnode_pager_max_urefs) {
		/*
		 *	Deallocate excess user references.
		 */

		(void) mach_port_mod_refs(vnode_pager_self, mem_obj_control,
					  MACH_PORT_RIGHT_SEND, -vs->urefs+1);
		vs->urefs = 1;
	}

	lock_done(&vs->vs_lock);
	return vs;
}


/*
 *	Routine:	memory_object_data_request (vnode_pager_data_request)
 *	Function:
 *		Read the requested data from the backing file and return the
 *		data to the caller.
 *
 *	Arguments:
 *		mem_obj - The port that represents the memory object data.
 *		mem_obj_control - The port that the memory manager uses to
 *			control the use of its data by the kernel.
 *		offset - The offset of the data in the memory object.
 *		length - The size of the data in bytes.
 *		desired_access - The memory access modes requested for the
 *                      cached data.
 */
#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t	
orig_memory_object_data_request(memory_object_t mem_obj, 
			        mach_port_t mem_obj_control,
			        vm_offset_t offset, 
			        vm_size_t length, 
			        vm_prot_t desired_access)
#else
kern_return_t	
orig_memory_object_data_request(mem_obj, mem_obj_control,
			        offset, length, desired_access)
	memory_object_t	mem_obj;
	mach_port_t	mem_obj_control;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
#endif /* __STDC__ */
#else /* VNODE_PAGER_ALT_THREADS */
#if __STDC__ == 1
kern_return_t	
memory_object_data_request(memory_object_t mem_obj, 
			   mach_port_t mem_obj_control,
			   vm_offset_t offset, 
			   vm_size_t length, 
			   vm_prot_t desired_access)
#else
kern_return_t	
memory_object_data_request(mem_obj, mem_obj_control,
			   offset, length, desired_access)
	memory_object_t	mem_obj;
	mach_port_t	mem_obj_control;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
#endif /* __STDC__ */
#endif /* VNODE_PAGER_ALT_THREADS */
{
	register vnode_pager_t	vs;
	int			ret;
        vm_prot_t               lock_value;
	vm_offset_t		buffer;
	int			count, resid;

	if (vnode_pager_debug)
		printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
			"(vnode_pager)data_request", mem_obj, offset, length);

	/* Currently this only support single page transfers. */
	if (length != PAGE_SIZE)
		panic("(vnode_pager)data_request: bad length");

	/*
	 * Look up the vnode paging structure, it must be init'ed and the
	 * control port must be the same as specified in the call.
	 */
	vs = vnode_pager_check_request(mem_obj, mem_obj_control);

	/*
	 * If there have been any previous errors, we bag out of the call.
	 */
	if (vs->errors) {
		if (vs->vs_pf) {
			simple_lock(&vs->vs_pf->pf_stat_lock);
			vs->vs_pf->pf_pagein_fail++;
			simple_unlock(&vs->vs_pf->pf_stat_lock);
		}

		printf("(vnode_pager)data_request:");
		printf(" dropping request (previous errors)\n");

		(void) memory_object_data_error(mem_obj_control, offset,
						PAGE_SIZE, KERN_FAILURE);
		return KERN_SUCCESS;
	}

#ifdef	OSF1_ADFS
	/*
	 * We don't allow unaligned paging requests.
	 */
	if ((offset & page_mask) != 0) {
		printf("memory_object_data_request unaligned offset=0x%x",
		       offset);
		(void) memory_object_data_error(mem_obj_control, offset,
						PAGE_SIZE, KERN_FAILURE);
		return KERN_SUCCESS;
	}
#endif
		
	/*
	 * Read the requested page.  Upon return, the data may exist in the
         * per-port set buffer or may be a newly allocated buffer.  In the
         * latter case, it's our responsibility to deallocate the new buffer.
         * It's also possible vs_page_read() returns EINVAL which instructs
         * us to do a zero-fill.
	 */
	buffer = vnode_pager_sets[vs->set].ipset_buffer;
	ret = vs_page_read(vs, offset, &buffer, &resid);

	switch (ret) {
	case ESUCCESS:
		if (buffer == vnode_pager_sets[vs->set].ipset_buffer) {
			/*
			 * The ipset_buffer was filled (can't deallocate).
			 */
			if (vnode_pager_debug)
			    printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
			       "(vnode_pager)data_provided", mem_obj, offset,
			       length);
			(void) memory_object_data_provided(mem_obj_control, 
							   offset, buffer, 
							   PAGE_SIZE,
							   VM_PROT_NONE);
		} else {
			/*
			 * A newly allocated buffer was returned.
			 */
                        assert(buffer != NULL);
#ifdef 	OSF1_ADFS
			/*
			 * If some bytes didn't exist, and a write is being
			 * done, then we must allocate disk space for them.
			 */
			if (resid && !vs->paging && VIO_IS_MAPPED(vs->vp) &&
			    (desired_access & VM_PROT_WRITE)) {
				VOP_ALLOC(vs->vp, offset+PAGE_SIZE-resid, 
					  resid, &count, vs->cred, ret);
                                if (ret) {
					if (ret == ENOSPC)
						ret = EXC_UNIX_ENOSPC;
					else
					      panic("vop_alloc bad ret 0x%x\n",
						      ret);
					(void) memory_object_data_error(
						    mem_obj_control, offset,
						    PAGE_SIZE, ret);
					vm_deallocate(mach_task_self(), buffer,
						      PAGE_SIZE-resid);
				        break;
				}
			}
#endif
			if (vnode_pager_debug)
			    printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
			       "(vnode_pager)data_supply", mem_obj, offset,
			       length);
			(void) memory_object_data_supply(mem_obj_control, 
							 offset, buffer, 
							 PAGE_SIZE, TRUE, 
							 VM_PROT_NONE, FALSE,
							 MACH_PORT_NULL);
                }
		break;

	case EINVAL:
		if (vnode_pager_debug)
			printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
			       "(vnode_pager)data_unavailable", mem_obj,
			       offset, length);
                /*
                 * Backing storage does not exist for this data range.
                 * For mapped files, allocate it now if write access is 
                 * desired, else protect the page so that a write will 
                 * result in a m_o_data_unlock (at which time we can 
                 * allocate backing storage).
                 */
#ifdef 	OSF1_ADFS
                if (!vs->paging && VIO_IS_MAPPED(vs->vp)) 
                        if (desired_access & VM_PROT_WRITE) {
                                VOP_ALLOC(vs->vp, offset, PAGE_SIZE, &count, 
					  vs->cred, ret);
                                if (ret) {
					if (ret == ENOSPC)
						ret = EXC_UNIX_ENOSPC;
					else
					      panic("vop_alloc bad ret 0x%x\n",
						      ret);
					(void) memory_object_data_error(
						    mem_obj_control, offset,
						    PAGE_SIZE, ret);
				} else
					/* zero fill */
					(void) memory_object_data_unavailable(
							 mem_obj_control,
                                                         offset,
                                                         PAGE_SIZE);
                        } else {
                                /*
                                 * Unfortunately, m_o_data_unavailable does not
                                 * support a lock_value arg.
                                 */
                                buffer = vnode_pager_sets[vs->set].ipset_buffer;
                                bzero((char *)buffer, PAGE_SIZE);
                                (void) memory_object_data_supply(
						      mem_obj_control,
                                                      offset, buffer,
                                                      PAGE_SIZE, 
                                                      FALSE, 
                                                      VM_PROT_WRITE, 
                                                      FALSE,
                                                      MACH_PORT_NULL);
                        }
                else
#endif
                        (void) memory_object_data_unavailable(mem_obj_control, 
                                                              offset, 
							      PAGE_SIZE);
		break;

	case EIO:
		if (vnode_pager_debug)
			printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
			       "(vnode_pager)data_error", mem_obj, offset,
			       length);
		(void) memory_object_data_error(mem_obj_control, offset,
						PAGE_SIZE, KERN_FAILURE);
		break;

	default:
		panic("(vnode_pager)data_request: vs_page_read");
	}

	return KERN_SUCCESS;
}

#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t	
memory_object_data_request(memory_object_t mem_obj, 
			   mach_port_t mem_obj_control,
			   vm_offset_t offset, 
			   vm_size_t length, 
			   vm_prot_t desired_access)
#else
kern_return_t	
memory_object_data_request(mem_obj, mem_obj_control,
			   offset, length, desired_access)
	memory_object_t	mem_obj;
	mach_port_t	mem_obj_control;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
#endif /* __STDC__ */
{
	kern_return_t ret;
	
	ret = do_alt_memory_object_data_request((mach_port_t) cthread_self(), 
						mem_obj, mem_obj_control,
					        offset, length, desired_access);
	if (ret != KERN_SUCCESS)
		panic("memory_object_data_request: do_alt_memory_object_data_request failed(0x%x)\n", ret);
	
	return KERN_SUCCESS;
}

#if __STDC__ == 1
kern_return_t	
alt_memory_object_data_request(mach_port_t alt_port, 
			       memory_object_t mem_obj,
			       mach_port_t mem_obj_control,
			       vm_offset_t offset, 
			       vm_size_t length, 
			       vm_prot_t desired_access)
#else
kern_return_t	
alt_memory_object_data_request(alt_port, mem_obj, mem_obj_control,
			       offset, length, desired_access)
	mach_port_t	alt_port;
	memory_object_t	mem_obj;
	mach_port_t	mem_obj_control;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
#endif /* __STDC__ */
{
	return orig_memory_object_data_request(mem_obj, mem_obj_control,
					       offset, length, desired_access);
}
#endif /* VNODE_PAGER_ALT_THREADS */

/*
 * 	Routine:	memory_object_data_write (vnode_pager_data_write)
 *	Function:
 *		Write the specified data back to its backing vnode.
 *
 *	Arguments:
 *		mem_obj - The port that represents the memory object data.
 *		mem_obj_control - The port that the memory manager uses to
 *			control the use of its data by the kernel.
 *		offset - The offset of the data in the memory object.
 *		length - The size of the data in bytes.
 */
#if __STDC__ == 1
kern_return_t	
memory_object_data_write(memory_object_t mem_obj, 
			 mach_port_t mem_obj_control, 
			 vm_offset_t offset, 
			 pointer_t data, 
			 vm_size_t length)
#else
kern_return_t	
memory_object_data_write(mem_obj, mem_obj_control, offset, data, length)
	memory_object_t		mem_obj;
	mach_port_t		mem_obj_control;
	register vm_offset_t	offset;
	register pointer_t	data;
	vm_size_t		length;
#endif /* __STDC__ */
{
	vnode_pager_t	vs;

	if (vnode_pager_debug)
		printf("%s: mem_obj=%d, offset=0x%x, length=0x%x\n",
		       "(vnode_pager)data_write", mem_obj, offset, length);

	/*
	 * This memory manager can only handle sizes that are a multiple of
	 * the page size.
	 */
	ASSERT((length % PAGE_SIZE) == 0);

	/*
	 * Look up the vnode paging structure.  It must be init'ed and its
	 * control port must match the one specified in the call.
	 */
	vs = vnode_pager_check_request(mem_obj, mem_obj_control);

	/*
	 * Write the data.
	 */
	vs_page_write(vs, offset, data, length);

	return(KERN_SUCCESS);
}

/*
 *	Routine: memory_object_data_initialize (vnode_pager_data_initialize)
 *	Function:
 *		Write the initial data for a kernel-created memory object.  If
 *		the data has already been supplied, then this function does
 *		nothing.  In all other respects, this function behaves exactly
 *		as memory_object_data_write.  This call will only be made on
 *		memory object created by the kernel.
 *
 *	Arguments:
 *		mem_obj - The port that represents the memory object data.
 *		mem_obj_control - The port that the memory manager uses to
 *			control the use of its data by the kernel.
 *		offset - The offset into the memory object to initialize.
 *		addr   - The address of the data to initialize the object to.
 *		data_cnt - The number of bytes of data.
 *
 *	Note:
 *		The implementation is far from optimized, and also assumes
 *		that the vnode pager is single-threaded.  JAC is this still
 *		true???
 *		This does the initialization one page at a time, it seems as
 *		though we should be able to cluster this operation.
 */

kern_return_t
memory_object_data_initialize(mem_obj, mem_obj_control, offset, addr, data_cnt)
	memory_object_t	mem_obj;
	mach_port_t	mem_obj_control;
	register
	vm_offset_t	offset;
	register
	pointer_t	addr;
	vm_size_t	data_cnt;
{
	vnode_pager_t	vs;
	vm_offset_t	data_sent;

	if (vnode_pager_debug)
		printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
		       "(vnode_pager)data_initialize",
		       mem_obj, offset, data_cnt);

	if ((data_cnt % PAGE_SIZE) != 0)
		panic("(vnode_pager)data_initialize: data_cnt");

	/*
	 * Look up the memory object's vnode pager structure.  The count must
	 * be one and the request port must match.
	 */
	vs = vnode_pager_check_request(mem_obj, mem_obj_control);

	/*
	 * Initialize the pager file to the specified data.
	 */
	for (data_sent = 0; data_sent < data_cnt; data_sent += PAGE_SIZE) {
		if (!vs_page_check(vs, offset + data_sent, addr + data_sent)) {
			vs_page_write(vs, offset + data_sent, 
				      (char *)addr + data_sent, PAGE_SIZE);
			if (vs->vs_pf) {
				simple_lock(&vs->vs_pf->pf_stat_lock);
				vs->vs_pf->pf_pageinit_write++;
				simple_unlock(&vs->vs_pf->pf_stat_lock);
			}
		} else {
		       vs_page_write_complete(vs, offset + data_sent, addr, 
					      PAGE_SIZE, TRUE, ESUCCESS);
		}
		if (vs->vs_pf) {
			simple_lock(&vs->vs_pf->pf_stat_lock);
			vs->vs_pf->pf_pageinit_count++;
			simple_unlock(&vs->vs_pf->pf_stat_lock);
		}
	}


	return(KERN_SUCCESS);
}

#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t	
orig_memory_object_data_unlock(memory_object_t pager, 
			       mach_port_t pager_request,
			       vm_offset_t offset, 
			       vm_size_t length, 
			       vm_prot_t desired_access)
#else
kern_return_t	
orig_memory_object_data_unlock(pager, pager_request,
			       offset, length, desired_access)
	memory_object_t	pager;
	mach_port_t	pager_request;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
#endif /* __STDC__ */
#else /* VNODE_PAGER_ALT_THREADS */
#if __STDC__ == 1
kern_return_t	
memory_object_data_unlock(memory_object_t pager, 
			  mach_port_t pager_request,
			  vm_offset_t offset, 
			  vm_size_t length, 
			  vm_prot_t desired_access)
#else
kern_return_t	
memory_object_data_unlock(pager, pager_request,
			  offset, length, desired_access)
	memory_object_t	pager;
	mach_port_t	pager_request;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
#endif /* __STDC__ */
#endif /* VNODE_PAGER_ALT_THREADS */
{
	vnode_pager_t	vs;
	boolean_t	should_flush;
        int             count, error;

#ifdef	lint
	pager++; pager_request++;
	offset++; length++;
	desired_access++;
#endif

#ifdef	OSF1_ADFS
	vs = vnode_pager_check_request(pager, pager_request);
        assert(!vs->paging);
        assert(VIO_IS_MAPPED(vs->vp));

        /* 
	 * We support lock values of read or read/write.  Hence this
	 * m_o_data_unlock must be requesting write access.
	 */
        assert((desired_access & VM_PROT_WRITE));

	/*
	 * Make sure backing storage is allocated for this range.
	 */
        VOP_ALLOC(vs->vp, offset, length, &count, vs->cred, error);
        if (error) {
		/*
		 * Can't respond with m_o_data_error so flush the page
		 * and let the kernel request again with m_o_data_request.
		 */
		should_flush = TRUE;
	} else
		should_flush = FALSE;

	/*
	 * We are guaranteed the vstruct won't be torn down out from
         * under us.  The reason is that vstructs are only torn down via
         * a no-more-senders, and there's no way such a message could be
         * queued ahead of the m_o_data_unlock (because it's targeted
         * at the memory object port).
	 */
        (void) memory_object_lock_request(vs->mem_obj_control, offset, length,
                                          MEMORY_OBJECT_RETURN_NONE, 
					  should_flush, VM_PROT_NONE, 
					  MACH_PORT_NULL);

	return KERN_SUCCESS;
#else
	panic("(vnode_pager)data_unlock: called");
	return KERN_FAILURE;
#endif
}

#ifdef VNODE_PAGER_ALT_THREADS
#if __STDC__ == 1
kern_return_t	
memory_object_data_unlock(memory_object_t pager, 
			  mach_port_t pager_request,
			  vm_offset_t offset, 
			  vm_size_t length, 
			  vm_prot_t desired_access)
#else
kern_return_t	
memory_object_data_unlock(pager, pager_request,
			  offset, length, desired_access)
	memory_object_t	pager;
	mach_port_t	pager_request;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
#endif /* __STDC__ */
{
	kern_return_t ret;

	ret = do_alt_memory_object_data_unlock((mach_port_t) cthread_self(), 
					       pager, pager_request,
					       offset, length, desired_access);
	if (ret != KERN_SUCCESS)
		panic("memory_object_data_unlock: do_alt_memory_object_data_unlock failed(0x%x)\n", ret);
	
	return KERN_SUCCESS;
}

#if __STDC__ == 1
kern_return_t	
alt_memory_object_data_unlock(mach_port_t alt_port, 
			      memory_object_t pager,
			      mach_port_t pager_request,
			      vm_offset_t offset, 
			      vm_size_t length, 
			      vm_prot_t desired_access)
#else
kern_return_t	
alt_memory_object_data_unlock(alt_port, pager, pager_request,
			      offset, length, desired_access)
	mach_port_t	alt_port;
	memory_object_t	pager;
	mach_port_t	pager_request;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	desired_access;
#endif /* __STDC__ */
{
	return orig_memory_object_data_unlock(pager, pager_request,
		                              offset, length, desired_access);
}
#endif /* VNODE_PAGER_ALT_THREADS */

#if __STDC__ == 1
kern_return_t
memory_object_lock_completed(memory_object_t pager, 
			     mach_port_t pager_request, 
			     vm_offset_t offset, 
			     vm_size_t length)
#else
kern_return_t
memory_object_lock_completed(pager, pager_request, offset, length)
	memory_object_t	pager;
	mach_port_t	pager_request;
	vm_offset_t	offset;
	vm_size_t	length;
#endif /* __STDC__ */
{
	vnode_pager_t	vs;

#ifdef	lint
	offset++; length++;
#endif

	vs = vnode_pager_check_request(pager, pager_request);
	thread_wakeup((int) vs);
	return KERN_SUCCESS;
}

#if __STDC__ == 1
kern_return_t
memory_object_copy(memory_object_t old_memory_object, 
		   memory_object_control_t old_memory_control,
		   vm_offset_t offset, 
		   vm_size_t length, 
		   memory_object_t new_memory_object)
#else
kern_return_t
memory_object_copy(old_memory_object, old_memory_control,
		   offset, length, new_memory_object)
	memory_object_t		old_memory_object;
	memory_object_control_t	old_memory_control;
	vm_offset_t		offset;
	vm_size_t		length;
	memory_object_t		new_memory_object;
#endif /* __STDC__ */
{
#ifdef	lint
	old_memory_object++; old_memory_control++;
	offset++; length++; new_memory_object++;
#endif

	panic("(vnode_pager)copy: called");
	return KERN_FAILURE;
}

/*
 *	Routine:	memory_object_create (vnode_pager_create)
 *	Purpose:
 *		Create a memory object for temporary kernel virtual memory.
 *
 *	Arguments:
 *		def_port - The default memory manager port.
 *		mem_obj - The port that represents the memory object data.
 *		size - The size of the temporary object.
 *		mem_obj_control - The port that the memory manager uses to
 *			control the use of its data by the kernel.
 *		mem_obj_name - The port that the kernel will use to identify
 *			this memory object to other tasks.
 *		page_size - The kernel's page size.
 */

kern_return_t
memory_object_create(def_port, mem_obj, size, mem_obj_control, 
		     mem_obj_name, page_size)
	mach_port_t	def_port;
	mach_port_t	mem_obj;
	vm_size_t	size;
	mach_port_t	mem_obj_control;
	mach_port_t	mem_obj_name;
	vm_size_t	page_size;
{
	vnode_pager_t	vs;
        memory_object_t pager;
	kern_return_t	ret;

	if (vnode_pager_debug)
		printf("%s: mem_obj=%d, new_request=%d, new_name=%d\n",
		       "(vnode_pager)create", mem_obj, mem_obj_control,
		       mem_obj_name);

        assert(def_port == vnode_pager_default);
        assert(mem_obj != MACH_PORT_NULL);
	assert(mem_obj_control != MACH_PORT_NULL);
	assert(mem_obj_name != MACH_PORT_NULL);
	assert(page_size == PAGE_SIZE);

	/*
	 * Allocate a paging file.
	 * Report any problems to the console.
	 */
	vs = pf_alloc(size);
	if (vs == VNODE_PAGER_NULL) {
		printf("(vnode_pager)create: unable to allocate");
		printf(" vnode_pager structure [REBOOT SUGGESTED]\n");
		return(KERN_RESOURCE_SHORTAGE);
	}

        pager = VSTRUCT_TO_PAGER(vs);
	assert(vs->mem_obj_control == MACH_PORT_NULL);
	assert(vs->urefs == 0);

	vs->mem_obj_control = mem_obj_control;
	vs->urefs = 1;

	/*
	 * Kernel-created memory objects are well-behaved in that only
	 * the kernel holds send rights to the memory object port.
	 * This right is relinquished by the kernel when the memory
	 * object is terminated.  Hence, there's no need to use NMS
	 * messages, but rather the vstruct may be torn down when
	 * the object is terminated.
	 */
	vs->using_nms = 0;

        /*
         * Kernel-created memory objects are different beasts.  We don't
         * ever attempt to set/change their attributes, so we don't
         * set the cacheable or copy_strategy fields in the vstruct.
         * They are implemented as non-cacheable, internal objects
         * (internal implies that all pages will be discarded at
         * termination time).
         */

        /*
         * Rename the port with the name chosen by pf_alloc.
         */
	if ((ret = mach_port_rename(vnode_pager_self, mem_obj, pager))
	    != KERN_SUCCESS) {
		panic("vnode_pager_create: port rename fail oldname=0x%x "
				"newname=0x%x ret=0x%x", mem_obj, pager, ret);
	}

        /*
         * Add the vstruct to a port set. 
         */
        vnode_pager_add(vs);

	return(KERN_SUCCESS);
}

#if __STDC__ == 1
kern_return_t
memory_object_data_return(memory_object_t mem_obj, 
			  memory_object_control_t mem_obj_control, 
			  vm_offset_t offset, 
			  vm_offset_t data, 
			  vm_size_t length,
			  boolean_t dirty, 
			  boolean_t kernel_copy)
#else
kern_return_t
memory_object_data_return(mem_obj, mem_obj_control, offset, data, length,
			  dirty, kernel_copy)
	memory_object_t	mem_obj;
	memory_object_control_t
			mem_obj_control;
	vm_offset_t	offset;
	vm_offset_t	data;
	vm_size_t	length;
	boolean_t	dirty;
	boolean_t	kernel_copy;
#endif /* __STDC__ */
{
	vnode_pager_t	vs;

#ifdef	lint
	kernel_copy++;
#endif	lint
	if (vnode_pager_debug)
		printf("%s: mem_obj=%d, offset=0x%x, length=0x%x\n",
		       "(vnode_pager)data_return", mem_obj, offset, length);

	/*
	 * This memory manager can only handle sizes that are a multiple of
	 * the page size.
	 */
	ASSERT((length % PAGE_SIZE) == 0);
	ASSERT(dirty == TRUE);

	/*
	 * Look up the vnode paging structure.  It must be init'ed and its
	 * control port must match the one specified in the call.
	 */
	vs = vnode_pager_check_request(mem_obj, mem_obj_control);

	/*
	 * Write the data.
	 */
	vs_page_write(vs, offset, data, length);

	return(KERN_SUCCESS);
}

#if __STDC__ == 1
kern_return_t
memory_object_supply_completed(memory_object_t memory_object, 
			       memory_object_control_t memory_control, 
			       vm_offset_t offset, 
			       vm_size_t length,
			       kern_return_t result, 
			       vm_offset_t error_offset)
#else
kern_return_t
memory_object_supply_completed(memory_object, memory_control, offset, length,
			       result, error_offset)
	memory_object_t	memory_object;
	memory_object_control_t
			memory_control;
	vm_offset_t	offset;
	vm_size_t	length;
	kern_return_t	result;
	vm_offset_t	error_offset;
#endif /* __STDC__ */
{
#ifdef	lint
	memory_object++; memory_control++; offset++;
	length++; result++; error_offset++;
#endif	lint
	panic("(vnode_pager)supply_completed: called");
}

#if __STDC__ == 1
kern_return_t
memory_object_change_completed(memory_object_t memory_object, 
			       boolean_t may_cache, 
			       memory_object_copy_strategy_t copy_strategy)
#else
kern_return_t
memory_object_change_completed(memory_object, may_cache, copy_strategy)
	memory_object_t	memory_object;
        boolean_t may_cache;
        memory_object_copy_strategy_t copy_strategy;
#endif /* __STDC__ */
{
	vnode_pager_t	vs;
#ifdef	lint
	may_cache++; copy_strategy++;
#endif	lint

	/*
	 * We are guaranteed the vstruct won't be torn down out from
         * under us.  The reason is that vstructs are only torn down via
         * a no-more-senders, and there's no way such a message could be
         * queued ahead of the m_o_change_completed (because it's targeted
         * at the memory object port).
	 */
	PAGER_TO_VSTRUCT(memory_object, vs);

	thread_wakeup((int) vs);

	return KERN_SUCCESS;
}
#endif  /* FULLSERVER */


/*
 *			Vnode Struct Functions
 *
 * Routines:
 *	vs_create          - Create a vstruct data structure.
 *      vs_ref             - Reference a vstruct data structure.
 *      vs_unref           - Unreference a vstruct data structure.
 *	vs_page_check	   - Check for existence of a page in the paging file.
 *	vs_page_read       - Read a page from a vnode paging file.
 *	vs_page_write      - Write a page to a vnode paging file.
 *	vs_page_write_complete - Callback when write is done.
 */

/*
 *	Routine:	vs_create
 *	Function:
 *		Create an vstruct corresponding to the given vp.
 *
 *	Notes:
 *		This can still livelock -- if the pageout daemon needs an
 *		vnode_pager record it won't get one until someone else refills
 *		the zone.
 */

private vnode_pager_t
vs_create(vp)
	register struct vnode	*vp;
{
	register vnode_pager_t	vs;

	vs = (struct vstruct *) zalloc(vstruct_zone);
	assert(vs != VNODE_PAGER_NULL);

#if     MACH_ASSERT
	vs->pager = VSTRUCT_TO_PAGER(vs);	
#endif
	vs->mem_obj_control = MACH_PORT_NULL;
	vs->paging = 0;
	lock_init(&vs->vs_lock, TRUE);
	vs->vp = vp;
	vs->vs_pf = NULL;
	crhold(u.u_cred);
	vs->cred = u.u_cred;
	vs->errors = 0;
	vs->urefs = 0;
        vs->refcnt = 1;
	VREF(vp);

	return(vs);
}

/*
 *	Routine:	vs_ref
 *	Function:
 *		Reference a vstruct.
 *              Called with vstruct lock held.
 */
void
vs_ref(vs)
        register vnode_pager_t	vs;
{
        assert(!vs->paging);
        vs->refcnt++;
}

/*
 *	Routine:	vs_unref
 *	Function:
 *		Unreference a vstruct.
 *              vstruct lock should not be held.
 */
void
vs_unref(vs)
        register vnode_pager_t	vs;
{
	lock_write(&vs->vs_lock);       
        assert(!vs->paging);
        if (--vs->refcnt == 0) {
                assert(vs->vp);
                vrele(vs->vp);      
                crfree(vs->cred);		
                zfree(vstruct_zone, (vm_offset_t) vs);
        } else
                lock_done(&vs->vs_lock);       
}

/*
 *	Routine:	vs_page_check
 *	Function:
 *		Check if the specified page exists in the paging file.
 *	Returns:
 *		TRUE if the page exists, FALSE is it does not.
 */
#if	FULLSERVER
boolean_t
vs_page_check(vs, offset, addr)
	vnode_pager_t	vs;
	vm_offset_t	offset;
	vm_offset_t	addr;
{
	if (!vs->paging)
		panic("vs_page_check");

	if (pf_bmap(vs, offset, B_READ) != (vm_offset_t)-1)
		return(TRUE);
	else
		return(FALSE);
}


/*
 *	Routine:	vs_page_read
 *	Function:
 *		Read a page from a vnode paging file.  This could either be a
 *		paging file or a "regular" file on disk.
 *
 *	Arguments:
 *		vs     - The vnode paging structure to read from.
 *		offset - The offset within the structure to read from.
 *		buffer - Pointer to an address of a buffer to fill.  
 *                       If, on return, the buffer address is different
 *                       then the caller is responsible for deallocating
 *                       the new buffer.
 *		resid  - [out] amount of bytes not read.
 *	Return:
 *		errno - EINVAL if the offset is illegal.  Tells the caller
 *                      to provide zero-filled data.
 */

kern_return_t
vs_page_read(vs, offset, buffer, resid)
	vnode_pager_t	vs;
	vm_offset_t	offset;
	vm_offset_t	*buffer;    /* in/out */        
	int		*resid;	    /* out */
{
	vm_offset_t	actual_offset;
	vm_offset_t	kva;
	struct uio	uio;
	struct iovec	iov;
	kern_return_t	ret;

	if (vs->paging) {
		actual_offset = pf_bmap(vs, offset, B_READ);
	} else {
		actual_offset = offset;
	}

	if (actual_offset == (vm_offset_t)-1)
		/* NOT a failure - zero fill on demand page */
		return (EINVAL);

	if (vs->vs_pf) {
		simple_lock(&vs->vs_pf->pf_stat_lock);
		vs->vs_pf->pf_pagein_count++;
		simple_unlock(&vs->vs_pf->pf_stat_lock);
	}

	if (vnode_pager_debug)
		printf("%s: vp = 0x%x, addr = 0x%x, offset = 0x%x\n",
		  "(vnode_pager)vs_page_read", vs->vp, *buffer, actual_offset);

	uio.uio_segflg = UIO_SYSSPACE;
	iov.iov_len = PAGE_SIZE;
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	uio.uio_offset = actual_offset;
	uio.uio_rw = UIO_READ;
	uio.uio_resid = PAGE_SIZE;

	/*
	 * Note that paging partitions are still being handled by the
	 * pgrd/prwr interfaces.  The proper way to rectify this is 
	 * convert them to use the datain/dataout interfaces and
	 * implement spec_datain/spec_dataout routines that use the
	 * vfs VIO module to perform synchronization in the face of
	 * asynchronous writes.
	 */
#ifdef	OSF1_ADFS
	if (VIO_IS_MAPPED(vs->vp) || 
	    (VIO_IS_PAGING(vs->vp) && vs->vp->v_type == VREG)) {
		/*
		 * Pass address of address so that a buffer may be returned.
		 */
		assert(vs->vp->v_type == VREG);
		iov.iov_base = (caddr_t)buffer;
		VOP_PAGEIN(vs->vp, &uio, vs->cred, ret);
	} else if (VIO_IS_FASTPATH(vs->vp)) {
		/*
		 * Pass NULL so that a buffer may be returned.
		 */
		assert(vs->vp->v_type == VREG);
		iov.iov_base = (caddr_t)NULL;
		VOP_DATAIN(vs->vp, &uio, 0, vs->cred, ret);
		*buffer = (vm_offset_t)iov.iov_base;
        } else
#endif
        {
                /*
                 * Pass address of buffer to be filled.
                 */
		iov.iov_base = (caddr_t)*buffer;
		VOP_PGRD(vs->vp, &uio, vs->cred, ret);
	}

	if (vnode_pager_debug)
		printf("%s: vp = 0x%x, addr = 0x%x, offset = 0x%x\n",
		       "(vnode_pager)vs_page_read done", vs->vp, *buffer, 
		       actual_offset);
	vnode_pager_pagein_count++;	/* Not Locked: may be inconsistent. */

	if (ret && vs->vs_pf) {
		simple_lock(&vs->vs_pf->pf_stat_lock);
		vs->vs_pf->pf_pagein_fail++;
		simple_unlock(&vs->vs_pf->pf_stat_lock);
	}
	
	*resid = uio.uio_resid;
	return (ret);
}

/*
 *	Routine:	vs_page_write
 *	Function:
 *		Write a page to a vnode paging file.  This could either be a
 *		paging file or a "regular" file on disk.
 *
 *	Arguments:
 *		vs     - The vnode paging structure to write to.
 *		offset - The offset within the structure to write to.
 *		addr   - The address to read the data from.
 *
 *	Return:
 *		errno - EINVAL if the offset is illegal.
 *			ENOSPC if the pager file was full, so the page
 *			       was not written.
 */

void
vs_page_write(vs, offset, addr, size)
	vnode_pager_t	vs;
	vm_offset_t	offset;
	vm_offset_t	addr;
	vm_size_t	size;
{
	vm_size_t	data_sent;
	struct uio	uio;
	struct iovec	iov;
	vm_offset_t	cur_offset;
	boolean_t	must_dealloc;
	int		num_pages, error, xerror = 0;

#ifdef 	OSF1_ADFS
	if (!vs->paging && (VIO_IS_MAPPED(vs->vp) || VIO_IS_FASTPATH(vs->vp))) {
		/*
		 * Multi-page writes are supported.
		 */
		iov.iov_base = (caddr_t)addr;;
		iov.iov_len = size;
		uio.uio_iov = &iov;
		uio.uio_iovcnt = 1;
		uio.uio_offset = offset;
		uio.uio_segflg = UIO_SYSSPACE;
		uio.uio_rw = UIO_WRITE;
		uio.uio_resid = size;
		if (VIO_IS_MAPPED(vs->vp))
			VOP_PAGEOUT(vs->vp, &uio, vs->cred, error);
		else
			VOP_DATAOUT(vs->vp, &uio, IO_CONSUME, vs->cred, error);

		vs_page_write_complete(vs, offset, addr, size, FALSE, error);

	} else
#endif
	{
		/*
		 * Multi-page writes are not supported.
		 */
		uio.uio_rw = UIO_WRITE;
		uio.uio_segflg = UIO_SYSSPACE;
		for (data_sent = 0; data_sent < size; data_sent += PAGE_SIZE) {
			cur_offset = offset + data_sent;
                        uio.uio_iov = &iov;
                        uio.uio_iovcnt = 1;
			addr = (vm_offset_t) ((char *)addr + data_sent);
			iov.iov_base = (caddr_t) addr;
			uio.uio_resid = PAGE_SIZE;
			iov.iov_len = PAGE_SIZE;
			if (vs->paging)
				uio.uio_offset = pf_bmap(vs, cur_offset,
							 B_WRITE);
			else 
				uio.uio_offset = cur_offset;

			if (uio.uio_offset == (vm_offset_t) -1) 
				/* no place to write to */
				vs_page_write_complete(vs, cur_offset,
						       addr, 
						       PAGE_SIZE, 
						       TRUE, ENOSPC); 
			else {
				assert(vs->vp);	       
#ifdef 	OSF1_ADFS
				if (VIO_IS_MAPPED(vs->vp) || 
				    (VIO_IS_PAGING(vs->vp) && 
				     vs->vp->v_type == VREG)) {
					assert(vs->paging);
					VOP_PAGEOUT(vs->vp, &uio, vs->cred, 
						    error);
					vs_page_write_complete(vs, cur_offset,
							       addr, 
							       PAGE_SIZE, 
							       FALSE, error); 
				} else 
#endif
				{
					VOP_PGWR(vs->vp, &uio, vs->cred, 
						 vs->vp->v_vm_info->pager, 
						 cur_offset, error);
					vs_page_write_complete(vs, cur_offset,
							       addr, 
							       PAGE_SIZE, 
							       TRUE, error); 
				}
			}

		}  /* for */
	}

	num_pages = size / PAGE_SIZE;		
	if (vs->paging && vs->vs_pf) {
		simple_lock(&vs->vs_pf->pf_stat_lock);
		vs->vs_pf->pf_pageout_count += num_pages;
		simple_unlock(&vs->vs_pf->pf_stat_lock);
	}

	/* Not Locked: may be inconsistent. */
	vnode_pager_pageout_count += num_pages;	
}

/*
 *	Routine:	vs_page_write_complete
 *	Function:
 *		Completion function called when a page write to a paging
 *		file has completed.
 *
 *	Arguments:
 *		vs     - The vnode paging structure written to.
 *		addr   - The address the data was written from.
 *		size   - The size of the data.
 * 		must_dealloc - should the data be deallocated?
 *		error  - result code.
 *
 *	Return:
 *		none.
 */
void
vs_page_write_complete(vs, offset, addr, size, must_dealloc, error)
	vnode_pager_t	vs;
	vm_offset_t	offset;
	vm_offset_t	addr;
	vm_size_t	size;
	boolean_t	must_dealloc;
	int		error;
{
	if (error) {
		/*
		 * The vs->error field is not properly interlocked here. We
		 * make the rash assumption that since it is never examined
		 * except for equality to zero, and once non-zero, it is never
		 * reset to zero while the vstruct is active, that the 
		 * increment below always works. (i.e., at least sets the field
		 * to a non-zero value.)
		 */
		if (vs->paging)
			panic("Pageout failure to a paging file:  Error=%d\n", error);
		else
			uprintf("Pageout failure to a regular file:  Error=%d\n", error);
		vs->errors++;
		if (vs->vs_pf) {
			simple_lock(&vs->vs_pf->pf_stat_lock);
			vs->vs_pf->pf_pageout_fail++;
			simple_unlock(&vs->vs_pf->pf_stat_lock);
		}
	}

#if	NORMA_VM
	/*
	 * Acknowledge completion of the pageout, allowing the
	 * sending kernel to free up resources.  
	 *
	 * vs->mem_obj_control can be used safely because of the 
	 * guarantee that m_o_terminate won't race in ahead of other 
	 * paging operations in progress.
	 */
	{
		kern_return_t	kr;

		kr = memory_object_data_write_completed(vs->mem_obj_control,
							offset, size);
		if (kr != KERN_SUCCESS)
			printf("vnode_pager: completion failure 0x%x\n", kr);
	}
#endif

	if (must_dealloc)
		/*
		 * Deallocate the memory that was written.
		 */
		if (vm_deallocate(vnode_pager_self, addr, size) != KERN_SUCCESS)
			panic("(vnode_pager)write_complete: deallocate failed");
}
#endif	/* FULLSERVER */


/*
 *			Paging File Support
 *
 * System Calls:
 *	swapon (mach_swapon) - Enable a paging file.
 *
 * Routines:
 *	pf_alloc           - Allocate a paging file.
 *	pf_lookup	   - Lookup a pager file given a vnode pointer.
 *	pf_default_init    - Initialize the default paging file.
 *	pf_file_init       - Initialize the specified vnode as a paging file.
 *	pf_find	           - Find the most preferred paging file.
 *	pf_pager_create    - Create a vnode pager structure.
 *	pf_dealloc         - Deallocate a vnode pager structure.
 *	pf_bmap            - Find the pager file offset given an object offset.
 *	pf_map_extend      - Extend the page map of a vnode paging structure.
 *	pf_allocate_page   - Allocate a page in a paging file.
 *	pf_deallocate_page - Deallocate a page in a paging file.
 *
 * Notes:
 *	These functions are multithread safe.
 *
 *	The field in the pager file structure pf_count is NOT maintained under
 *	lock.  Currently, this field is never read - hence we don't care if it
 *	is accurate.  If, in the future, we start using this field, the we
 *	will have to lock it.
 */

/*
 *	Routine:	swapon
 *	Function:
 *		Syscall interface to swapon.  This system call allows the
 *		caller to add paging files and devices.
 *
 *	Arguments:
 *		filename     - The file name of the new paging
 *		               file or raw device.
 *		flags        - One or more of the following bits:
 *			       MS_PREFER - This is a preferred paging file.
 *		lowat, hiwat - The low water and high water marks that the
 *			       size of the pager file will float between.  If
 *			       the low water mark is zero, then the file will
 *			       not shrink after paging space is freed.  If the
 *			       high water mark is zero, the file will grow
 *			       without bounds.
 *
 */
#ifndef OSF1_ADFS                
int
swapon(p, args, retval)
	void *p;
	void *args;
	int *retval;
{
	struct	args {
		char 	*filename;
		int	flags;
		long	lowat;
		long	hiwat;
	} *uap = (struct args *) args;
	pager_file_t		pf;
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp = NULLVP;
	int 			error = 0;

	/*
	 * Only the super-user can turn on a swapping file.
	 */
#if     SEC_BASE
	if (!privileged(SEC_MOUNT, EPERM))
		return (EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = uap->filename;

	/*
	 * Loop up the specified paging file, if it is found then attempt to
	 * initialize it.  If any of this doesn't work, return a helpful
	 * error. 
	 */
#if SEC_BASE
	/*
	 * Don't collect any audit data from the following namei.
	 */
	audstub_nopath();
#endif
	if (!(error = namei(ndp))) {
		vp = ndp->ni_vp;
		lock_write(&pager_file_lock);
		if (!(error = pf_file_init(&pf, vp, uap->lowat, 
                                           uap->hiwat))) {
			pf->pf_prefer = ((uap->flags & MS_PREFER) != 0);
			(void) copyinstr((caddr_t)uap->filename,
			    (caddr_t)pf->pf_name, sizeof pf->pf_name, (int *)0);
                }
		lock_done(&pager_file_lock);
	}

	/*
	 * Release the reference to the vnode taken by namei, and unlock it.
	 * There is still a reference to the vnode that was taken by
	 * pf_file_init.
	 */
	if (vp)
		vrele(vp);
	if (!error) 
		vnode_pager_is_set = TRUE;  /* indicate swapon has occurred */

	return(error);
}
#else	/* OSF1_ADFS */
int
swapon(p, args, retval)
	void *p;
	void *args;
	int *retval;
{
	struct	args {
		char 	*filename;
		int	flags;
		long	lowat;
		long	hiwat;
	} *uap = (struct args *) args;
	pager_file_t		pf;
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp = NULLVP;
	int 			error = 0;

	/*
	 * Only the super-user can turn on a swapping file.
	 */
#if     SEC_BASE
	if (!privileged(SEC_MOUNT, EPERM))
		return (EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = uap->filename;

	/*
	 * Loop up the specified paging file, if it is found then attempt to
	 * initialize it.  If any of this doesn't work, return a helpful
	 * error. 
	 */
#if SEC_BASE
	/*
	 * Don't collect any audit data from the following namei.
	 */
	audstub_nopath();
#endif
	if (!(error = namei(ndp))) {
		int		dev;
		char		name[PNAMELEN];
		int		prefer;

		prefer = ((uap->flags & MS_PREFER) != 0);
		(void) copyinstr((caddr_t)uap->filename,
			    (caddr_t)name, sizeof name, (int *)0);
		vp = ndp->ni_vp;
#ifdef PFS
		/*
		 * Disallow swapping into a PFS file system.
		 */
		if (VIO_IS_PFS(vp)) {
			vrele(vp);
			return(EFSNOTSUPP);
                }
#endif
		error = remote_pf_file_init(vp, uap->lowat, uap->hiwat, 
							prefer, name);
	}

	/*
	 * Release the reference to the vnode taken by namei, and unlock it.
	 * There is still a reference to the vnode that was taken by
	 * pf_file_init.
	 */
	if (vp)
		vrele(vp);

	return(error);
}
internal_pf_file_init(vp, lowat, hiwat, name, prefer)
	struct vnode	*vp;
	int		lowat;
	int		hiwat;
	int		prefer;
	char		*name;
{
#if	FULLSERVER
	pager_file_t	pf;
	int 		error = 0;

	lock_write(&pager_file_lock);
	if (!(error = pf_file_init(&pf, vp, lowat, hiwat))) {
		pf->pf_prefer = prefer;
		strcpy(pf->pf_name, name);
		vnode_pager_is_set = TRUE;  /* swapon has occurred */
	}
	lock_done(&pager_file_lock);
	return(error);
#else	/* FULLSERVER */
	panic("internal_pf_file_init");
#endif	/* FULLSERVER */
}
#endif	/* OSF1_ADFS */

#if	FULLSERVER
/*
 *	Routine:	pf_alloc
 *	Function:
 *		Allocate a paging file to page out a kernel-created memory
 *		object.
 *
 *	Arguments:
 *		size - The size of the memory object we are allocating paging
 *		       space for.
 *
 *	Notes:
 *		The size argument doesn't get passed through to pf_find, how
 *		do we know that the paging file found has enough space?
 */

vnode_pager_t
pf_alloc(size)
	vm_size_t	size;
{
	pager_file_t	pf;
	vnode_pager_t	vs;

	if (vnode_pager_debug)
		printf("pf_alloc called with size = %d pages\n",atop(size));

	/*
	 * Get a pager_file, then turn it into a paging space.
	 */
	lock_read(&pager_file_lock);
	pf = pf_find();
	lock_done(&pager_file_lock);

	if ((vs = pf_pager_create(pf, size)) == (vnode_pager_t)NULL)
		panic("pf_alloc: can't alloc vstruct");
	return(vs);
}

/*
 *	Routine:	pf_lookup
 *	Function:
 *		Lookup the pager file that correspondes with the specified
 *		vnode.
 *
 *	Arguments:
 *		vp - The vnode to look up.
 *
 *	Return:
 *		pager_file_t:  The pager file found.  If none is found, then
 *			       return PAGER_FILE_NULL.
 */

pager_file_t
pf_lookup(vp)
	struct vnode	*vp;
{
	register pager_file_t	pf;
	register pager_file_t	pf_tmp = PAGER_FILE_NULL;

	lock_read(&pager_file_lock);
	for (pf = (pager_file_t) queue_first(&pager_files);
	     !queue_end(&pager_files, &pf->pf_chain);
	     pf = (pager_file_t) queue_next(&pf->pf_chain)) {
		if (pf->pf_vp == vp) {
			pf_tmp = pf;
			break;
		}
	}
	lock_done(&pager_file_lock);
	return(pf_tmp);
}

/*
 * 	Routine:	pf_file_init
 *	Function:
 *		Create a pager_file structure for a new pager file.
 *
 *	Arguments:
 *		This file in question is specified by vnode pointer.
 *		lowat and hiwat are the low water and high water marks
 *		that the size of pager file will float between.  If
 *		the low water mark is zero, then the file will not
 *		shrink after paging space is freed.  If the high water
 *		mark is zero, the file will grow without bounds.
 *
 *	Return:
 *		kern_return_t - Zero if successful, otherwise an error number.
 *		pfp - pass by reference - Pointer to the allocated pager file.
 *
 *	Locking:
 *		This assumes that the pager_file_lock is held for write access.
 */
int
pf_file_init(pfp, vp, lowat, hiwat)
	pager_file_t	*pfp;
	struct vnode 	*vp;
	long		lowat;
	long		hiwat;
{
	struct	vattr		vattr;
	register pager_file_t	pf;
	int			error;
	int			mflag;
	register struct mount	*mp;
	long			i;
	long			size;
	int			type;
	int			dev;
	struct ucred		*ucred;

	*pfp = PAGER_FILE_NULL;
	error = 0;
	ucred = u.u_cred;

	/*
	 * Is some other object paging to this file?
	 */
	VN_LOCK(vp);
#ifdef	OSF1_ADFS
	if (vp->v_vm_info->pager) {
		VN_UNLOCK(vp);
		return(EBUSY);
	}
#endif
	mp = vp->v_mount;
	VN_UNLOCK(vp);

	/*
	 * Paging is permitted to regular files and block devices.
	 * We will not swap to file systems that have the M_SWAP_NEVER
	 * flag set.
	 */
	MOUNT_LOCK(mp);
	mflag = mp->m_flag;
	MOUNT_UNLOCK(mp);
	VN_LOCK(vp);
	type = vp->v_type;
	if (type == VBLK)
		dev = vp->v_specinfo->si_rdev;
	VN_UNLOCK(vp);
	if (type == VREG) {
		if (mflag & M_SWAP_NEVER)
			error = EINVAL;
		else if (mflag & M_RDONLY)
			error = EROFS;
	}
	else if (type == VBLK) {
		/*
		 * Do an open on the device to get error
		 * checking to happen, including a check
		 * for whether a file system is mounted
		 * on this device.
		 */
		error = spec_open(&vp, FREAD|FWRITE, ucred);
        }
	else
		error = EINVAL;
	if (error)
		return(error);

	/*
	 * Look to see if we are already paging to this file.
	 */
	for (pf = (pager_file_t) queue_first(&pager_files);
	     !queue_end(&pager_files, &pf->pf_chain);
	     pf = (pager_file_t) queue_next(&pf->pf_chain)) {
		if (major(pf->pf_vp->v_rdev) == major(dev) &&
 		    minor(pf->pf_vp->v_rdev) == minor(dev) &&
 		    pf->pf_vp->v_devnode == vp->v_devnode)
			break;
	}
	if (!queue_end(&pager_files, &pf->pf_chain)) {
		if (type == VBLK)
			spec_close(vp, 0, ucred);
		return(EBUSY);
	}

	/*
	 * Clean up the file blocks on an pager file by truncating to length
	 * "lowat".
	 */
	if (type == VREG) {
		VOP_GETATTR(vp, &vattr, ucred, error);
		size = vattr.va_size;
		if (size > lowat) {
			vattr_null(&vattr);
			vattr.va_size = size = lowat;
			VOP_SETATTR(vp, &vattr, ucred, error);
			if (error) {
				printf("pf_file_init: truncate failed, error = %d\n",
				       error);
				return(error);
			}
		}
	}
	else {
#ifdef OSF1_ADFS                
		BDEVSW_PSIZE(major(dev), dev, vp->v_devnode, size);
#else
		BDEVSW_PSIZE(major(dev), dev, size);
#endif
		if (size < 0) {
			spec_close(vp, 0, ucred);
			return(EINVAL);
		}
		hiwat = size = dgtob(size);
		lowat = 0;
                if (vnode_pager_debug)
                        printf("Paging partition (%d, %d) size %d\n", major(dev),
                               minor(dev), size);
	}

	/*
	 * Initialize the vnode_size field.  Allocate and initialize the pager
	 * file structure.
	 */
	pf = (pager_file_t) kalloc(sizeof(struct pager_file));
	VREF(vp);
	pf->pf_vp = vp;
	crhold(ucred);
	VN_LOCK(vp);
	vp->v_vm_info->vnode_size = size;
	vp->v_vm_info->cred = ucred;
#ifdef	OSF1_ADFS
	/*
	 * Set the I/O mode to indicate it's a paging file.
	 */
	vp->v_iomode = VIO_PAGING;
#endif
	VN_UNLOCK(vp);
	pf->pf_count = 0;
	pf->pf_lowat = atop(round_page(lowat));

	/*
	 * If no maximum space is specified, then we should make a map that
	 * can cover the entire disk, otherwise the block map need only
	 * cover the maximum space allowed.
	 */
	if (!hiwat && type == VREG) {
		/*
		 * these fields are immutable, so no locking required.
		 */
		hiwat = mp->m_stat.f_blocks * mp->m_stat.f_fsize;
	}

	if (vnode_pager_debug)
		printf("Paging file %x size = %d\n", vp, size);

	pf->pf_pfree = pf->pf_npgs = atop(hiwat);
	pf->pf_bmap = (u_char *) kalloc(RMAPSIZE(pf->pf_npgs));
	for (i = 0; i < pf->pf_npgs; i++) {
		clrbit(pf->pf_bmap, i);
	}
	pf->pf_hipage = 0;
	pf->pf_hint = 0;
	pf->pf_prefer = FALSE;
	pf->pf_pagein_count = 0;
	pf->pf_pagein_fail = 0;
	pf->pf_pageout_count = 0;
	pf->pf_pageout_fail = 0;
	pf->pf_pageinit_count = 0;
	pf->pf_pageinit_write = 0;
	simple_lock_init(&pf->pf_stat_lock);
	lock_init(&pf->pf_lock, TRUE);

	if (type == VBLK) {
		int i;
		/*
		 * Step over enough space to allow for a bootstrap and/or label
		 * on this device. This is really only necessary if the
		 * partition starts at sector zero, but we do it for all
		 * devices rather than worry about a page or so. We use the
		 * UFS superblock offset as an indication of how much space
		 * UFS would have reserved on this device.
		 */
		for (i = 0; i < (SBOFF + PAGE_SIZE - 1)/PAGE_SIZE; i++) {
			if (pf_allocate_page(pf) != i)
				panic("swapon: bad page reservation");
		}
	}

	/*
	 * Put the new pager file in the list.
	 */
	queue_enter(&pager_files, pf, pager_file_t, pf_chain);
	pager_file_count++;
	*pfp = pf;
	return(0);
}

/*
 *	Routine:	pf_find
 *	Function:
 *		Find the best place to page out a kernel-created memory
 *		object.
 *
 *	Implementation: 
 *		This looks for the "best" paging file.  Files are chosen in
 *		the following order:
 *
 *		1. Prefered file on preferred file system.
 *		2. Prefered file on any file system.
 *		3. Any file on preferred file system.
 *		4. Any file.
 *
 *		Within each category, the file with the most space is chosen
 *		first.
 *
 *	Locking:
 *		This assumes that the pager_file_lock is held for read access.
 */

pager_file_t
pf_find()
{
	int 		mostspace;
	pager_file_t	pf, mostpf;
	int		mflag;

	if (pager_file_count == 0) {
		return(PAGER_FILE_NULL);
	}

	/*
	 * It is a pretty easy search when there is only one paging file...
	 */
	if (pager_file_count == 1) {
		return((pager_file_t) queue_first(&pager_files));
	}

	mostpf = PAGER_FILE_NULL;
	mostspace = 0;

	/*
	 * First pass we look for preferred files on preferred file systems.
	 */
	for (pf = (pager_file_t)queue_first(&pager_files);
	     !queue_end(&pager_files, &pf->pf_chain);
	     pf = (pager_file_t)queue_next(&pf->pf_chain)) {
		MOUNT_LOCK(pf->pf_vp->v_mount);
		mflag = pf->pf_vp->v_mount->m_flag;
		MOUNT_UNLOCK(pf->pf_vp->v_mount);
		if (!pf->pf_prefer ||
		    !(mflag & M_SWAP_PREFER))
			continue;
		if (pf->pf_pfree > mostspace) {
			mostspace = pf->pf_pfree;
			mostpf = pf;
		}
	}
	if (mostpf != PAGER_FILE_NULL)
		return(mostpf);

	/*
	 * Second pass we look for preferred files on any file systems.
	 */
	for (pf = (pager_file_t)queue_first(&pager_files);
	     !queue_end(&pager_files, &pf->pf_chain);
	     pf = (pager_file_t)queue_next(&pf->pf_chain)) {
		if (!pf->pf_prefer)
			continue;
		if (pf->pf_pfree > mostspace) {
			mostspace = pf->pf_pfree;
			mostpf = pf;
		}
	}
	if (mostpf != PAGER_FILE_NULL)
		return(mostpf);

	/*
	 * Third pass we look for any files on preferred file systems.
	 */
	for (pf = (pager_file_t)queue_first(&pager_files);
	     !queue_end(&pager_files, &pf->pf_chain);
	     pf = (pager_file_t)queue_next(&pf->pf_chain)) {
		MOUNT_LOCK(pf->pf_vp->v_mount);
		mflag = pf->pf_vp->v_mount->m_flag;
		MOUNT_UNLOCK(pf->pf_vp->v_mount);
		if (!(mflag & M_SWAP_PREFER))
			continue;
		if (pf->pf_pfree > mostspace) {
			mostspace = pf->pf_pfree;
			mostpf = pf;
		}
	}
	if (mostpf != PAGER_FILE_NULL)
		return(mostpf);

	/*
	 * Fourth pass we look for any files.
	 */
	for (pf = (pager_file_t)queue_first(&pager_files);
	     !queue_end(&pager_files, &pf->pf_chain);
	     pf = (pager_file_t)queue_next(&pf->pf_chain)) {
		if (pf->pf_pfree > mostspace) {
			mostspace = pf->pf_pfree;
			mostpf = pf;
		}
	}
	return(mostpf);
}


/*
 *	Routine:	pf_pager_create
 *	Function:
 *		Create a vnode pager structure cooresponding to the given
 *		paging file.
 *
 *	Arguments:
 *		pf   - The paging file that backs the memory object.
 *		size - The size of the memory object to allocate paging space
 *		       for.
 *
 *	Notes:
 *		This can still "livelock" -- if the pageout daemon needs an
 *		vnode_pager record it may have to block until someone else
 *		refills the zone.
 *
 *	Locking:
 *		No locks need be held before taken this function.
 */

private
vnode_pager_t
pf_pager_create(pf, size)
	register pager_file_t	pf;
	vm_size_t		size;
{
	register vnode_pager_t	vs;
	register int		i;

	/*
	 * Get a vnode_pager_t from it zone.  This will block until one is
	 * available.
	 */
	vs = (struct vstruct *) zalloc(vstruct_zone);
	assert(vs != VNODE_PAGER_NULL);

#if	MACH_ASSERT
	vs->pager = VSTRUCT_TO_PAGER(vs);
#endif

	vs->mem_obj_control = MACH_PORT_NULL;
	vs->vs_size = atop(round_page(size));

	/*
	 * Allocate the pmap, either PAGEMAP_SIZE or INDIRECT_PAGEMAP_SIZE
	 * depending on the size of the memory object.
	 */
	if (INDIRECT_PAGEMAP(vs->vs_size)) {
		vs->vs_pmap = (vm_offset_t **)
			kalloc(INDIRECT_PAGEMAP_SIZE(vs->vs_size));
	} else {
		vs->vs_pmap = (vm_offset_t **)
			kalloc(PAGEMAP_SIZE(vs->vs_size));
	}

	/*
	 * Check to see that we got the space.  If we didn't then we can't
	 * sleep waiting for free pages (it could cause a deadlock), so we
	 * just return nothing.
	 */
	if (vs->vs_pmap == (vm_offset_t **) 0) {
		zfree(vstruct_zone, (vm_offset_t) vs);
		return(VNODE_PAGER_NULL);
	}

	/*
	 * Zero the indirect pointers, or set the direct pointers to -1.
	 */
	if (INDIRECT_PAGEMAP(vs->vs_size)) {
		bzero((caddr_t)vs->vs_pmap,
		      INDIRECT_PAGEMAP_SIZE(vs->vs_size));
	} else {
	       	for (i = 0; i < vs->vs_size; i++)
		     vs->vs_pmap[i] = (vm_offset_t *) -1;
	}

	/*
	 * Complete the initializaton and return the allocate vnode paging
	 * structure.
	 */
	vs->paging = 1;
	lock_init(&vs->vs_lock, TRUE);
	{
		struct proc *p;

		p = &proc[0];
		crhold(p->p_rcred);
		vs->cred = p->p_rcred;
	}
	vs->errors = 0;
	vs->vs_pf = pf;
	if (pf != PAGER_FILE_NULL) {
		vs->vp = pf->pf_vp;
		pf->pf_count++;
	} else {
		vs->vp = NULLVP;
	}

	vs->urefs = 0;

	return(vs);
}

/*
 *	Routine:	pf_dealloc
 *	Function:
 *		Deallocate the specified vnode paging structure and all
 *		resources associated with that structure.
 *		
 *	Arguments:
 *		vs - Vnode paging structure to deallocate.
 *
 *	Locking:
 *		No locks must be taken before calling this function.
 */

void
pf_dealloc(vs)
	vnode_pager_t	vs;
{
	pager_file_t		pf;
	int			i,j;

	if (vs->paging == 0)
		panic("pf_dealloc: not a paging file");
	pf = vs->vs_pf;

	/*
	 * If this is an indirect structure, then we walk through the valid
	 * (non-zero) indirect pointers and deallocate the valid (non-negative
	 * one) pages.  When all of the pages in an indirect page have been
	 * deallocated, we deallocate the page.  When all of the indirect
	 * pages have been deallocated we deallocate the indirect pointers.
	 */
	if (INDIRECT_PAGEMAP(vs->vs_size)) {
		for (i = 0; i < INDIRECT_PAGEMAP_ENTRIES(vs->vs_size); i++) {
			if (vs->vs_pmap[i] != NULL) {
				for(j = 0; j < PAGEMAP_ENTRIES; j++) {
					if (vs->vs_pmap[i][j] != -1) {
						if (pf == PAGER_FILE_NULL)
							panic("pf_dealloc: NULL pf");
						pf_deallocate_page(pf, 
							    vs->vs_pmap[i][j]);
						vs->vs_pmap[i][j] = -1;
					}
				}
				zfree(vindirect_zone, vs->vs_pmap[i]);
				vs->vs_pmap[i] = NULL;
			}
		}
		kfree(vs->vs_pmap, INDIRECT_PAGEMAP_SIZE(vs->vs_size));
	}

	/*
	 * If this is a direct structure, then simply free the valid
	 * (non-negative one) pages.  When all of the pages have been freed,
	 * free the direct page map.
	 */
	else {
		for (i = 0; i < vs->vs_size; i++) {
			if (vs->vs_pmap[i] != (vm_offset_t *)-1) {
				if (pf == PAGER_FILE_NULL)
					panic("pf_dealloc: NULL pf");
				pf_deallocate_page(pf, (daddr_t)vs->vs_pmap[i]);
				vs->vs_pmap[i] = (vm_offset_t *)-1;
			}
		}
		kfree(vs->vs_pmap, PAGEMAP_SIZE(vs->vs_size));
	}
	vs->vs_pmap = NULL;

	/*
	 * Decrement our reference to the paging file, free the credentials
	 * and deallocate the zone.
	 */
	if (pf != PAGER_FILE_NULL) 
		pf->pf_count--;
        crfree(vs->cred);		
	zfree(vstruct_zone, (vm_offset_t) vs);
}

/*
 *	Routine:	pf_bmap
 *	Function:
 *		Return the file offset into a pager file for a given offset
 *		into an object backed by the pager file.
 *
 *		This function will expand the vnode paging structure's block
 *		map if the object has grown since the pf_alloc or the last
 *		pf_bmap.
 *
 *	Arguments:
 *		vs       - The vnode structure for the paging file.
 *		f_offset - The offset (in bytes) into the object.
 *		flag     - Currently one of:
 *			   B_READ - We are performing a read, only look up the
 *				    page.  Do not do any allocations.
 *
 *	Returns:
 *		vm_offset_t - The page offset into the pager file that backs
 *		up the specified offset into the object.  If the page is not
 *		in the pager file or there is no room left in the file, then
 *		-1 is returned.
 *
 *	Locking:
 *		No locks need to be taken before calling this function.
 */

vm_offset_t
pf_bmap(vs, f_off, flag)
	vnode_pager_t 	vs;
	vm_offset_t 	f_off;
	int 		flag;
{
	vm_offset_t 	f_page;		/* The page number of f_off.	*/
	vm_offset_t 	newpage;	/* The new page allocated.	*/
	vm_offset_t 	newoffset;	/* The paging file byte offset.	*/
	register int 	i;
	pager_file_t	pf;

	f_page = atop(f_off);

	if (vnode_pager_debug)
		printf("pf_bmap called with %x (%d)\n", f_off, f_page);

	/*
	 * If the object has no paging file associated, go for it
	 */
	if (vs->vs_pf == PAGER_FILE_NULL) {
		if (flag == B_READ)
			return ((vm_offset_t)-1);
		lock_read(&pager_file_lock);
		if ((pf = pf_find()) == PAGER_FILE_NULL) {
		   lock_done(&pager_file_lock);
		   printf("vnode pager: no space in available paging files;\n");
		   printf("             swapon suggested\n");
  		   return((vm_offset_t)-1);
		}
		lock_done(&pager_file_lock);
		vs->vs_pf = pf;
		vs->vp = pf->pf_vp;
		pf->pf_count++;
	}
	/*
	 * If the object has grown, extend the page map.
	 */
	if (f_page >= vs->vs_size) {
		if (pf_map_extend(vs, f_page + 1)) {
			return((vm_offset_t)-1);
		}
	}

	/*
	 * Look for the desired page.  If the map is indirect, then we have a
	 * two level lookup.  First find the indirect block, then find the
	 * actual page.  If the indirect block has not yet been allocated,
	 * then do so.  If the page has not yet been allocated, then do so.
	 * If any of the allocations fail, then return an error.
	 */
	if (INDIRECT_PAGEMAP(vs->vs_size)) {
		long	ind_block = f_page/PAGEMAP_ENTRIES;
		long	ind_offset = f_page%PAGEMAP_ENTRIES;

		/* Is the indirect block allocated? */
		if (vs->vs_pmap[ind_block] == NULL) {

			/* Do not allocate for a read, this is an error */
			if (flag == B_READ) {
				return ((vm_offset_t)-1);
			}

			/* Allocate the indirect block */
			if ((vs->vs_pmap[ind_block] =
			    (vm_offset_t *) zalloc(vindirect_zone)) == NULL) {
				return((vm_offset_t)-1);
			}

			/* Initialize the page numbers to invalid */
			for (i = 0; i < PAGEMAP_ENTRIES;
			     vs->vs_pmap[ind_block][i++] = (vm_offset_t)-1);
		}

		/* Is the page allocated? */
		if (vs->vs_pmap[ind_block][ind_offset] == -1) {

			/* Do not allocate for a read, this is an error */
			if (flag == B_READ) {
				return((vm_offset_t)-1);
			}

			/* Allocate a page from the paging file. */
			if ((newpage = pf_allocate_page(vs->vs_pf)) ==  -1) {
				return((vm_offset_t)-1);
			}
			vs->vs_pmap[ind_block][ind_offset] = newpage;
		}
		/* Calculate the byte offset in the paging file */
		newoffset = (f_off&page_mask) +
			ptoa(vs->vs_pmap[ind_block][ind_offset]);
	}

	/*
	 * Direct mapped structure.  Simply look in the map for the paging
	 * page, if it is not yet allocated, then allocate it.
	 */
	else {
		if (vs->vs_pmap[f_page] == (vm_offset_t *) -1) {

			/* Do not allocate for a read, this is an error */
			if (flag == B_READ) {
				return((vm_offset_t)-1);
			}

			/* Attempt to allocat a page from the paging file */
			if ((newpage = pf_allocate_page(vs->vs_pf)) == -1) {
				return((vm_offset_t)-1);
			}
			vs->vs_pmap[f_page] = (vm_offset_t *)newpage;
		}
		/* Calculate the byte offset in the paging file */
		newoffset = (f_off&page_mask) +
			ptoa((vm_offset_t)vs->vs_pmap[f_page]);
	}

	if (vnode_pager_debug)
		printf("pf_bmap returning %x\n", newoffset);
	return(newoffset);
}

/*
 *	Routine:	pf_map_extend
 *	Function:
 *		Extend the page map of a vnode paging structure.
 *
 *	Arguments:
 *		vs       - The vnode paging structure.
 *		new_size - The new size of the paging structure.
 *
 *	Return:
 *		int - Zero if the extension was successful, -1 if there was a
 *		problem.
 *
 *	Notes:
 *		This function can not sleep.  If we have a problem allocating
 *		space, we simply return with an error indication.
 *
 *	Locking:
 *		No locks need to be taken before calling this function.
 */

int
pf_map_extend(vs, new_size)
	vnode_pager_t	vs;
	int		new_size;
{
	vm_offset_t	**new_pmap;
	int		i;

	/*
	 * If the new size extends into the indirect range, then we have one
	 * of two cases: we are going from indirect to indirect, or we are
	 * going from direct to indirect.  If we are going from indirect to
	 * indirect, then it is possible that the new size will fit in the old
	 * indirect map.  If this is the case, then just reset the size of the
	 * vnode paging structure and we are done.  If the new size will not
	 * fit into the old indirect map, then we have to allocate a new
	 * indirect map and copy the old map pointers into this new map.
	 *
	 * If we are going from direct to indirect, then we have to allocate a
	 * new indirect map and copy the old direct pages into the first
	 * indirect page of the new map.
	 */
	if (INDIRECT_PAGEMAP(new_size)) {
		int new_map_size = INDIRECT_PAGEMAP_SIZE(new_size);

		if (INDIRECT_PAGEMAP(vs->vs_size)) {
			int old_map_size = INDIRECT_PAGEMAP_SIZE(vs->vs_size);

			/* New size fit into old map? */
			if (new_map_size == old_map_size) {
				vs->vs_size = new_size;
				return(0);
			}
				
			/* Get a new indirect map */
			if ((new_pmap = (vm_offset_t **)
			     kalloc(new_map_size)) == NULL) {
				return (-1);
			}
			bzero((caddr_t)new_pmap, new_map_size);

			/* Copy old entries into new map */
			for (i = 0; i < (old_map_size/sizeof (caddr_t)); i++) {
				new_pmap[i] = vs->vs_pmap[i];
			}

			/* And free the old map */
			kfree((vm_offset_t)vs->vs_pmap, old_map_size);
		}
		else {	/* Old map was a direct map. */

			/* Get a new indirect map */
			if ((new_pmap = (vm_offset_t **)
			     kalloc(new_map_size)) == NULL) {
				return (-1);
			}
			bzero((caddr_t)new_pmap, new_map_size);

			/* Allocate an indirect page */
			if ((new_pmap[0] = (vm_offset_t *)
			     zalloc(vindirect_zone)) == NULL) {
				kfree((vm_offset_t)new_pmap, new_map_size);
				return (-1);
			}

			/* Copy old map into first indirect block. */
			for (i = 0; i < vs->vs_size; i++) {
				new_pmap[0][i] = (vm_offset_t) vs->vs_pmap[i];
			}

			/* Initialize the remainder of the block. */
			for (i = vs->vs_size; i < PAGEMAP_ENTRIES;
			     new_pmap[0][i++] = (vm_offset_t)-1);

			/* And free the old map */
			kfree((vm_offset_t)vs->vs_pmap,
			      PAGEMAP_SIZE(vs->vs_size));
		}
	}

	/*
	 * If the new map is a direct map, then the old map must also have
	 * been a direct map.  All we have to do is to allocate a new direct
	 * map, copy the old entries into it and free the old map.
	 */
	else {
		/* Allocate the new map */
		if ((new_pmap = (vm_offset_t **)
		     kalloc(PAGEMAP_SIZE(new_size))) == NULL) {
			return (-1);
		}

		/* Copy info from the old map into the new map */
		for (i = 0; i < vs->vs_size; i++) {
			new_pmap[i] = vs->vs_pmap[i];
		}

		/* Initialize the rest of the new map */
		for (i = vs->vs_size; i < new_size;
		     new_pmap[i++] = (vm_offset_t *) -1);

		/* Free the old map */
		kfree((vm_offset_t)vs->vs_pmap, PAGEMAP_SIZE(vs->vs_size));
	}
	vs->vs_pmap = new_pmap;
	vs->vs_size = new_size;
	return(0);
}

/*
 * 	Routine:	pf_allocate_page
 *	Function:
 *		Allocate a page in the specified paging file.
 *
 *	Arguments:
 *		pf - The paging file to allocate a page from.
 *
 *	Return:
 *		vm_offset_t - The file offset within the paging file of the
 *		allocated page.
 *
 *	Notes:
 *		The pager file lock is only ever taken for write access, why
 *		is it a read/write lock?
 *
 *	Locking:
 *		No locks need to be taken before calling this function.
 */

vm_offset_t
pf_allocate_page(pf)
	register struct pager_file *pf;
{
	int		byte_num;		/* byte counter */
	int 		bit_num;		/* bit counter */
	vm_offset_t	page;			/* page number */

	/*
	 * Lock access to the paging file and check if there is space
	 * available - if there is none, then unlock and return an error.
	 */
	lock_write(&pf->pf_lock);
	if (pf->pf_pfree == 0) {
		lock_done(&pf->pf_lock);
		return(-1);
	}

	/*
	 * Look for an available paging block.  At the end of the loop,
	 * byte_num is the byte offset and bit_num is the bit offset of the
	 * first zero bit in the pager file bitmap.
	 */
	byte_num = pf->pf_hint;
	for ( ; byte_num < howmany(pf->pf_npgs, NBBY); byte_num++) {
		if (*(pf->pf_bmap + byte_num) != BYTEMASK) {
			for (bit_num = 0; bit_num < NBBY; bit_num++) {
				if (isclr((pf->pf_bmap + byte_num), bit_num))
					break;
			}
			assert(bit_num != NBBY);
			break;
		}
	}
	pf->pf_hint = byte_num;
	page = (byte_num*NBBY) + bit_num;

	/* This should not happen as we checked for pfree of zero above. */
	if (page >= pf->pf_npgs) {
		panic("pf_allocate_page: invalid page number");
	}

	/*
	 * If there is a low water mark, we have to maintain the highest page
	 * allocated so that we know when we can truncate the file down to the
	 * low water mark.
	 */
	if (pf->pf_lowat && page > pf->pf_hipage) {
		pf->pf_hipage = page;
	}
	setbit(pf->pf_bmap,page);
	if (--pf->pf_pfree == 0)
		printf("pf_allocate_page: pager file is full.\n");

	lock_done(&pf->pf_lock);

	if (vnode_pager_debug)
		printf("pf_allocate_page returning: %d\n", page);
	return(page);
}

/*
 *	Routine:	pf_deallocate_page
 *	Function:
 *		Deallocate the specified page from the specified paging file.
 *
 *	Arguments:
 *		pf   - The paging file to deallocate the page from.
 *		page - The page to deallocate.
 *
 *	Locking:
 *		All necessary locking is done in this func
 */

void
pf_deallocate_page(pf, page)
     register struct pager_file *pf;
     daddr_t 			page;
{
	struct vnode	*vp = pf->pf_vp;
	struct ucred	*cred;
	struct vattr	vattr;
	int		error;
	long		tpage;
	int		i;


	if (page >= (daddr_t) pf->pf_npgs)
		panic("pf_deallocate_page: Invalid page number");

	/*
	 * Lock the paging file, clear the page's bitmap and increment the
	 * number of free pages.
	 */
	lock_write(&pf->pf_lock);
	clrbit(pf->pf_bmap, page);
	if (++pf->pf_pfree == 1)
		printf("pf_deallocate_page: pager file is available again.\n");

	/*
	 * Move the hint down to the freed page if it is less than the current
	 * hint.
	 */
	if ((page/NBBY) < pf->pf_hint) {
		pf->pf_hint = (page/NBBY);
	}

	/*
	 * If there is no low water mark, we do not need to maintain the
	 * hipage.
	 */
	if (pf->pf_lowat == 0 || page != pf->pf_hipage) {
		lock_done(&pf->pf_lock);
		return;
	}

	/*
	 * We just deallocated the hipage and we need to maintain it.  Look
	 * for the new hipage.
	 */
	for (i = page - 1; i >= 0; i--) {
		if (isset(pf->pf_bmap, i)) {
			pf->pf_hipage = i;
			break;
		}
	}

	/*
	 * If the new hipage is past the low water mark, then truncate the
	 * file.
	 */
	tpage = pf->pf_hipage + 1;
	VN_LOCK(vp);
	cred = vp->v_vm_info->cred;
	if (vp->v_type == VREG && tpage >= pf->pf_lowat && vp->v_vm_info->vnode_size > ptoa(tpage)) {
		VN_UNLOCK(vp);
		vattr_null(&vattr);
		vattr.va_size = ptoa(tpage);
		assert((int) vattr.va_size >= 0);
		VOP_SETATTR(vp, &vattr, cred, error);
		if (error) {
			printf("pf_deallocate_page: error truncating pager file, error = %d\n", error);
		}
	} else
		VN_UNLOCK(vp);
	lock_done(&pf->pf_lock);
}
#endif	/* FULLSERVER */

#ifdef OSF1_ADFS
extern node_t this_node;

/*
 * int
 * vnode_pager_info.
 * 
 * Returns information about the vnode pager, specifically about
 * paging files and statistics.  Currently the only caller is table().
 * This function could be extended to get/set more vnode pager
 * information.
 * 
 * For each paging file/partition an entry is filled in the 'pginfo' array
 * 'pgsize' indicates the number of entries for which space has been allocated
 * in 'pginfo'
 * The actual num of entries filled is returned in 'retsize'
 * 
 * If 'pgsize' is 0 we only return the # of paging files/partitions. 
 */
int
vnode_pager_info(pginfo, pgsize, retsize)
	register struct tbl_pginfo_10 	*pginfo;  /* array of pginfo struct */
        int                             pgsize;   /* # of ele in pginfo */
        int                             *retsize; /* # of entries returned */
{
	register pager_file_t pf = PAGER_FILE_NULL;
	register struct tbl_pagerglobal_info *pggp;

        *retsize = 0;
        lock_read(&pager_file_lock);
	if (pgsize == 0) {  /* just checking for # of entries */
		*retsize = pager_file_count;
                lock_done(&pager_file_lock);
		return (0);
	} 

	if (pager_file_count == 0) {
                *retsize = 0;
                lock_done(&pager_file_lock);
                return(0);
	}

	/*
	 * loop throught the list of paging files 
	 */
        
        for (pf = (pager_file_t) queue_first(&pager_files);
             !queue_end(&pager_files, &pf->pf_chain);
             pf = (pager_file_t) queue_next(&pf->pf_chain)) {

                simple_lock(&pf->pf_stat_lock);
		pginfo->pg_prefer = pf->pf_prefer;
		pginfo->pg_npgs = pf->pf_npgs;
		pginfo->pg_free = pf->pf_pfree;
		pginfo->pg_pagein_count = pf->pf_pagein_count;
		pginfo->pg_pagein_fail = pf->pf_pagein_fail;
		pginfo->pg_pageout_count = pf->pf_pageout_count;
		pginfo->pg_pageout_fail = pf->pf_pageout_fail;
                pginfo->pg_pageinit_count = pf->pf_pageinit_count;
                pginfo->pg_pageinit_write = pf->pf_pageinit_write;
                pginfo->pg_hipage = pf->pf_hipage;
                pginfo->pg_node = this_node;
                simple_unlock(&pf->pf_stat_lock);
                pginfo->pg_type = (pf->pf_vp->v_type == VREG) ? PG_VNODE_FILE : 
                  PG_VNODE_RAWPART;
		copystr(pf->pf_name, pginfo->pg_name, min(PATH_MAX, PNAMELEN),
                        0);
                if (++(*retsize) >= pgsize)
                        break;
                pginfo++;       /* next entry */
        }
        lock_done(&pager_file_lock);
	return (0);
}
#endif

#if     FULLSERVER
boolean_t
vnode_pager_notify_server(in, out)
	mach_msg_header_t *in, *out;
{
	register mach_no_senders_notification_t *n =
			(mach_no_senders_notification_t *) in;

	/*
	 *	The only send-once rights we create are for
	 *	receiving no-more-senders notifications.
	 *	Hence, if we receive a message directed to
	 *	a send-once right, we can assume it is
	 *	a genuine no-senders notification from the kernel.
	 */

	if ((n->not_header.msgh_bits !=
			MACH_MSGH_BITS(0, MACH_MSG_TYPE_PORT_SEND_ONCE)) ||
	    (n->not_header.msgh_id != MACH_NOTIFY_NO_SENDERS))
		return FALSE;

	assert(n->not_header.msgh_size == sizeof *n);
	assert(n->not_header.msgh_remote_port == MACH_PORT_NULL);

	assert(n->not_type.msgt_name == MACH_MSG_TYPE_INTEGER_32);
	assert(n->not_type.msgt_size == 32);
	assert(n->not_type.msgt_number == 1);
	assert(n->not_type.msgt_inline);
	assert(! n->not_type.msgt_longform);

#ifdef VNODE_PAGER_ALT_THREADS
	{
	kern_return_t ret;
	
	ret = do_alt_vnode_pager_no_senders((mach_port_t) cthread_self(), 
					    n->not_header.msgh_local_port, 
					    n->not_count);
	if (ret != KERN_SUCCESS)
		panic("vnode_pager_no_senders: do_alt_vnode_pager_no_senders failed(0x%x)\n", ret);
	}
#else /* VNODE_PAGER_ALT_THREADS */
	vnode_pager_no_senders(n->not_header.msgh_local_port, n->not_count);
#endif /* VNODE_PAGER_ALT_THREADS */

	out->msgh_remote_port = MACH_PORT_NULL;
	return TRUE;
}

#ifdef VNODE_PAGER_ALT_THREADS
#if     FULLSERVER
#include <mach/memory_object_server.c>
#include <mach/memory_object_default_server.c>
#include <builtin/alt_memory_object_server.c>
#include <builtin/alt_memory_object_user.c>
#endif  /* FULLSERVER */
#endif /* VNODE_PAGER_ALT_THREADS */

#ifdef VNODE_PAGER_ALT_THREADS
any_t
vnode_pager_server_loop(port_set, alt_port)
	mach_port_t port_set;
	mach_port_t alt_port;
#else /* VNODE_PAGER_ALT_THREADS */
any_t
vnode_pager_server_loop(port_set)
	mach_port_t port_set;
#endif /* VNODE_PAGER_ALT_THREADS */
{
	mach_msg_return_t	r;
	mach_port_t	my_self;
	vm_offset_t	messages;
	register mach_msg_header_t *in_msg;
	register mach_msg_header_t *out_msg;
#ifdef VNODE_PAGER_ALT_THREADS
	register int is_alt_thread = (port_set == alt_port);
#endif /* VNODE_PAGER_ALT_THREADS */

	my_self = mach_task_self();

	if (vm_allocate(my_self, &messages, 2 * 8192, TRUE) != KERN_SUCCESS)
	    panic("inode_pager: can't allocate message buffers");

	in_msg  = (mach_msg_header_t *)messages;
	out_msg = (mach_msg_header_t *)(messages + 8192);

	for (;;) {
	    r = mach_msg(in_msg, MACH_RCV_MSG,
			 0, 8192, port_set,
			 MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	    if (r != MACH_MSG_SUCCESS) {
		printf("inode_pager: receive failed, %d\n", r);
		continue;
	    }

#if	NCPUS == 1
	    unix_master();
#endif	
#ifdef VNODE_PAGER_ALT_THREADS
	    if (is_alt_thread)
	        (void) alt_memory_object_server(in_msg, out_msg);
	    else
#endif /* VNODE_PAGER_ALT_THREADS */
	    (void) (memory_object_server(in_msg, out_msg) ||
		    memory_object_default_server(in_msg, out_msg) ||
		    vnode_pager_notify_server(in_msg, out_msg));
#if	NCPUS == 1
	    unix_release();
#endif	

	    if (MACH_PORT_VALID(out_msg->msgh_remote_port)) {
		(void) mach_msg(out_msg, MACH_SEND_MSG,
				out_msg->msgh_size, 0, MACH_PORT_NULL,
				MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	    }
	}
}

/*
 *	Routine:	vnode_pager
 *	Function:
 *		Entry point of the vnode_pager thread.  This is called at
 *		system startup (out of init_main), after the vnode pager
 *		task has been created.  This will perform some initialization,
 *		create vnode pager default ports and create the multiple vnode
 *		pager threads.
 */

void
vnode_pager()
{
	extern void 		task_name();
	memory_object_t		t;
	int 			i;

	vnode_pager_self = mach_task_self();

#if	MACH_AFS
	max_pager_threads = vnode_pager_num_sets + afs_pager_num_sets;
#else	/* MACH_AFS */
	max_pager_threads = vnode_pager_num_sets;
#endif	/* MACH_AFS */

	/*
	 * Check that we have atleast one pager.  Allocate the vnode pager
	 * sets.
	 */
	if (vnode_pager_num_sets < 1)
		panic("vnode_pager: number of sets less than one");
	vnode_pager_sets = (vnode_pager_set_t)
		kalloc((max_pager_threads * sizeof (*vnode_pager_sets)));
	if (vnode_pager_sets == 0)
		panic("inode_pager: can't allocate sets");

	/*
	 * Allocate the vnode pager sets & default ports. These will each be 
	 * assigned to a different thread.  This allows a degree of 
	 * multithreading.
	 */
	for (i = 0; i < max_pager_threads; i++) {
		vnode_pager_set_t ipset = &vnode_pager_sets[i];
		mach_port_t set, port;
		vm_offset_t buffer;

		/*
		 * Create a default port for this vnode set.
		 */
		if (mach_port_allocate(vnode_pager_self, 
				       MACH_PORT_RIGHT_RECEIVE,
				       &port) != KERN_SUCCESS)
			panic("inode_pager: can't allocate default port");
		
		/*
		 * Create a port set for this vnode set
		 */
		if (mach_port_allocate(vnode_pager_self, 
				       MACH_PORT_RIGHT_PORT_SET,
				       &set) != KERN_SUCCESS)
			panic("inode_pager: cannot create port set");

		/*
		 * Associate this default port with the pager set.
		 */
		if (mach_port_move_member(vnode_pager_self, port, set)
		    != KERN_SUCCESS)
			panic("inode_pager: cannot enable default port");

		if (vm_allocate(vnode_pager_self, &buffer, PAGE_SIZE, TRUE)
							!= KERN_SUCCESS)
			panic("inode_pager: can't allocate a buffer");
		
		ipset->ipset_set = set;
		ipset->ipset_port = port;
		ipset->ipset_count = 0;
		ipset->ipset_thread = NO_CTHREAD;
		ipset->ipset_buffer = buffer;
	}

	/*
	 * The default vnode pager port will be the first set default port
	 */
	vnode_pager_default = vnode_pager_sets[0].ipset_port;

	/*
	 * Allocate threads for the rest of the vnode pager sets.
	 */

	/* protects vnode_pager_sets at thread creation time */
	mutex_init(&pager_set_lock);
	for (i = 0; i < max_pager_threads; i++) {
		vnode_pager_set_t ipset = &vnode_pager_sets[i];
#ifdef VNODE_PAGER_ALT_THREADS
		mach_port_t port;
#endif /* VNODE_PAGER_ALT_THREADS */

		mutex_lock(&pager_set_lock);
		ipset->ipset_thread = 
			vnode_pager_start_slave(vnode_pager_slave);
		mutex_unlock(&pager_set_lock);

		if (vnode_pager_debug)
			printf("vnode pager: created thread %d: %x\n",
			       i, ipset->ipset_thread);

#ifdef VNODE_PAGER_ALT_THREADS
		/*
		 * Create a alternate port for this vnode set.
		 */
		port = (mach_port_t) ipset->ipset_thread;
		if (mach_port_allocate_name(vnode_pager_self, 
				            MACH_PORT_RIGHT_RECEIVE,
				            port) != KERN_SUCCESS)
			panic("inode_pager: can't allocate alternate port");
		
		/*
		 * Increase queue limit as workaround for PTS-#11705.
		 */
		if (mach_port_set_qlimit(vnode_pager_self, 
				         port, 256) != KERN_SUCCESS)
			panic("inode_pager: can't set qlimit of alternate port");
		
		/*
		 * Insert send right to alternate port for this vnode set.
		 */
		if (mach_port_insert_right(vnode_pager_self, port, port,
				           MACH_MSG_TYPE_MAKE_SEND) != KERN_SUCCESS)
			panic("inode_pager: can't insert send right to alternate port");
		
		ipset->ipset_alt_port = port;
		
		mutex_lock(&pager_set_lock);
		ipset->ipset_alt_thread = 
			vnode_pager_start_slave(vnode_pager_alt_slave);
		mutex_unlock(&pager_set_lock);

		if (vnode_pager_debug)
			printf("vnode pager: created alternate thread %d: %x\n",
			       i, ipset->ipset_alt_thread);
#endif /* VNODE_PAGER_ALT_THREADS */
	}
}

/*
 *	Routine:	vnode_pager_slave
 *	Function:
 *		Entry point for the vnode pager slaves.  Start the slave's
 *		message server loop.
 *
 *	Note:
 *		The slave thread's number is communicated to it in the
 *		reply_port field of its thread structure.  No problem, as long
 *		as we remember to clear this field before Mig operations try
 *		to use it.
 */

void
vnode_pager_slave(arg)
    any_t arg;
{
	int num;
	cthread_t th = cthread_self();
	struct uthread *uth = &u;

	set_thread_priority( mach_thread_self(), 2);


	/* use special set of NORMA resources */

        norma_enable_vnode_pager(privileged_host_port,mach_thread_self());

	mutex_lock(&pager_set_lock);
	for (num = 0; num < max_pager_threads; num++) {
		if (vnode_pager_sets[num].ipset_thread == th)
			break;
	}
	mutex_unlock(&pager_set_lock);
	if (num == max_pager_threads)
		panic("vnode_pager_slave.get_num");

	if (num == 0) {
		cthread_set_name(th, "vnpgr_master");
		if (vnode_pager_debug)
			printf("vnode_pager: Master thread(%d) %x started\n", 
			       num, th);
	} else {
		cthread_set_name(th, "vnpgr_slave");
		if (vnode_pager_debug)
			printf("vnode_pager: slave thread(%d) %x started\n", 
			       num, th);
	}
	uarea_init(uth, &proc[0]);
#ifdef VNODE_PAGER_ALT_THREADS
	vnode_pager_server_loop(vnode_pager_sets[num].ipset_set,
	                        vnode_pager_sets[num].ipset_alt_port);
#else /* VNODE_PAGER_ALT_THREADS */
	vnode_pager_server_loop(vnode_pager_sets[num].ipset_set);
#endif /* VNODE_PAGER_ALT_THREADS */
}

#ifdef VNODE_PAGER_ALT_THREADS
/*
 *	Routine:	vnode_pager_alt_slave
 *	Function:
 *		Entry point for the vnode pager alternate slaves.  
 *		Start the alternate slave's message server loop.
 */

void
vnode_pager_alt_slave(arg)
    any_t arg;
{
	int num;
	cthread_t th = cthread_self();
	struct uthread *uth = &u;

	set_thread_priority( mach_thread_self(), 2);

	/* use special set of NORMA resources */

        norma_enable_vnode_pager(privileged_host_port,mach_thread_self());

	mutex_lock(&pager_set_lock);
	for (num = 0; num < max_pager_threads; num++) {
		if (vnode_pager_sets[num].ipset_alt_thread == th)
			break;
	}
	mutex_unlock(&pager_set_lock);
	if (num == max_pager_threads)
		panic("vnode_pager_slave.get_num");

	cthread_set_name(th, "vnpgr_alt_slave");
	if (vnode_pager_debug)
		printf("vnode_pager: alternate slave thread(%d) %x started\n", 
		       num, th);

	uarea_init(uth, &proc[0]);
	vnode_pager_server_loop(vnode_pager_sets[num].ipset_alt_port,
				vnode_pager_sets[num].ipset_alt_port);
}
#endif /* VNODE_PAGER_ALT_THREADS */


/*
 *	Routine:	vnode_pager_bootstrap
 *	Function:
 *		Initialize the vnode pager data structures.
 *
 *	Assumptions:
 *		Called at system boot time.
 */

void
vnode_pager_bootstrap()
{
	register vm_size_t	size;

	/*
	 *	Initialize zone of paging structures.
	 */
	size = (vm_size_t) sizeof(struct vstruct);
	vstruct_zone = zinit(size,
			     (vm_size_t) 10000*size,	/* XXX */
			     PAGE_SIZE, "vnode pager structures");
	/*
	 *      Initialize zone of indirect blocks.
	 */
	vindirect_zone = zinit(PAGEMAP_THRESHOLD,
			     (vm_size_t) 512*PAGEMAP_THRESHOLD, /* XXX */
			     PAGE_SIZE, "vnode indirect blocks");

	queue_init(&pager_files);
	lock_init(&pager_file_lock, TRUE);
	pager_file_count = 0;

}

/*
 *	Routine:	vnode_pager_start_slave
 *	Function:
 *		Start a slave thread to handle a port set.
 *
 *	Arguments:
 *		num - The slave and port set number.
 *
 *	Return:
 *		thread_t - The created thread.
 *
 *	Notes:
 *		The mach_user_internal.h interface contains thread_create and
 *		thread_resume definitions that we don't want.
 */

cthread_t
vnode_pager_start_slave(func)
	cthread_fn_t(*func);
{
	cthread_t       slave;
        extern any_t	ux_thread_bootstrap();

	slave = cthread_fork(ux_thread_bootstrap, (any_t)func);
	cthread_detach(slave);

	return(slave);
}
#endif  /* FULLSERVER */

/*
 *	Routine:	vnode_pager_add
 *	Purpose:
 *		Add a vnode structure to a port set.  We use a number of port
 *		sets in a feeble attempt to get some multithreading.
 *
 *	Arguments:
 *		vs - The vnode structure to add.
 *
 *	Notes:
 *		The ipset_count is not maintained in an MP safe manner.
 *		Currently this does not matter as no one uses the number.
 */

void
vnode_pager_add(vs)
	register vnode_pager_t	vs;
{
	extern struct timeval time;
	mach_port_t name;
	vnode_pager_set_t set;
	int i;
	kern_return_t ret;

	/*
	 * Allocate a random port set.
	 */
	name = VSTRUCT_TO_PAGER(vs);
	TIME_READ_LOCK();
	i = time.tv_sec + time.tv_usec + name;
#if	MACH_AFS
	/* 
	 * Make sure a different set of vnode pager threads handle
	 * all AFS related paging to avoid deadlocks.
	 */
	if (IsAfsVnode(vs->vp)) {
	     i = (i % (unsigned int) (afs_pager_num_sets - 1)) \
			+ vnode_pager_num_sets + 1;
	} else 
#endif	/* MACH_AFS */
	     i = (i % (unsigned int) (vnode_pager_num_sets - 1)) + 1;
	TIME_READ_UNLOCK();
	assert((0 <= i) && (i < max_pager_threads));
	set = &vnode_pager_sets[i];

	/*
	 * Add this vnode structure's pager port to the random port set.
	 */
	if ((ret = mach_port_move_member(vnode_pager_self, name, 
				     set->ipset_set)) != KERN_SUCCESS) {
		printf("ret = %x\n",ret);
		panic("vnode_pager_add: can't put object into set");
	}

	set->ipset_count++;		/* NO LOCKING: may be inconsistent. */
	vs->set = i;
}

#ifndef VNODE_PAGER_ALT_THREADS
#if     FULLSERVER
#include <mach/memory_object_server.c>
#include <mach/memory_object_default_server.c>
#endif  /* FULLSERVER */
#endif /* VNODE_PAGER_ALT_THREADS */
