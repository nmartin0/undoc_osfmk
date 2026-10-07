/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * SSD HISTORY
 * $Log: vm_pageout.c,v $
 * Revision 1.22  1995/03/28  16:24:06  cfleck
 *   Description: added comment to 1.21 revision
 *
 * Revision 1.21  1995/03/28  15:45:26  cfleck
 *  Reviewer: stans, andyp, terry
 *  Risk: medium
 *  Benefit or PTS #: 12280, 12102
 *  Testing: johannes core dump test code, fritz's svm test (caused denied
 *           counts to sky rocket, and parallel SATs
 *  Module(s): kernel/vm_pageout.c
 *  Description: Fixed problem caused by 8754. Objects marked external, but
 * 		 generated locally were not getting serviced properly.
 * 		 An attempt was made to take a local reservation, but
 *		 it was denied (causing the denied counter to be bumped)
 *		 because local reservations were zero'd out...
 *		 To fix this, a need_reservation flag was added.
 *		 In this case, no reservation was needed.
 * 		 This allows those pages to processed properly.
 *  
 *
 * Revision 1.20  1995/02/04  01:52:51  stans
 *  Remove dead/unreferenced norma version 1 code.
 *
 *  Reviewer:self
 *  Risk:low
 *  Benefit or PTS #:5249
 *  Testing:WW50 sats
 *  Module(s):
 *
 * Revision 1.19  1994/11/18  21:04:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1994/10/11  17:36:10  tnt
 *  PTS #: 8754
 *  Mandatory?:  Yes
 *  Description: Ignore unexpected data-write-completed notifications rather
 *               than panicking.  No longer make pageout reservations for
 *               local pageout requests.
 *  Reviewer(s): Stefan Tritscher
 *  Risk:        Medium
 *  Testing:     Developer tests, Message passing EATs, Parallel SATs.
 *  Module(s):   kernel/vm/vm_object.c
 *               kernel/vm/vm_pageout.c
 *               kernel/vm/memory_object.c
 *               kernel/norma/xmm_server.c
 *
 * Left in VM_PAGEOUT_LOCAL_RESERVATION_MAX for now, but set value to
 * zero to reflect that local requests don't use reservations.  Removed
 * vm_pageout_reserve_terminate.
 *
 * Revision 1.17  1994/08/31  21:26:08  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.15.2.1  1994/08/23  01:48:49  rkl
 *  Fixed and indefinite loop bug in vm_pageout_scan().
 *
 *  Reviewer: stans
 *  Risk: low
 *  Benefit or PTS #: 10678
 *  Testing: Ran PFS r/w test
 *  Module(s): kernel/vm/vm_pageout.c
 *
 * Revision 1.15  1994/07/18  21:12:04  regnier
 * Add VCF message passing protocol. No VCF code is compiled in unless
 * the '+vcf' option is used to build the kernel.
 *
 *  Reviewer: wms
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: EATS, developer
 *  Module(s): vm/vm_pageout.c  kern/syscall_sw.c   conf/MASTER.i860
 *             conf/files.i860  i860paragon/mcmsg/mcmsg_config.h
 *             i860paragon/mcmsg/mcmsg_config_vcf.h
 *             i860paragon/msgp/msgp_mp.c
 *             i860paragon/vcf/*
 *
 * Revision 1.14  1994/07/12  19:27:01  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.13  1994/03/30  01:15:04  joel
 *  Reviewer: cfj, terry
 *  Risk:low
 *  Benefit or PTS #:7923
 *  Testing:MCAT for a month
 *  Module(s): kernel/vm/vm_pageout.c
 *
 * Revision 1.12  1994/03/06  22:26:24  sean
 *  Reviewer:  steved
 *  Risk:  low
 *
 * Checking in these files which represent Steved's fixes for problems
 * in the following following areas:
 *
 * -fscan
 * -vdp clock skew
 * -bootmagic for processor mode
 *
 * ./ddb/db_command.c
 * ./ddb/db_print.c
 * ./i860/hardclock.c
 * ./i860paragon/mp.h
 * ./i860paragon/fscan.c
 * ./i860paragon/fscan.h
 * ./i860paragon/model_dep.c
 * ./i860paragon/mp_start.s
 * ./i860paragon/rpm.c
 * ./i860paragon/mp.c
 * ./i860paragon/interrupt.c
 * ./intel/pmap.c
 * ./ipsc/bootenv.c
 * ./kern/sched_prim.c
 * ./kern/mach_clock.c
 * ./vm/vm_pageout.c
 *
 * Revision 1.11.2.12  1994/06/27  23:04:00  andyp
 * Make the pageout thread fixpri 9.
 *
 *	Revision 1.10.2.2  1994/03/30  01:09:15  joel
 *	 Reviewer: cfj,terry
 *	 Risk:low
 *	 Benefit or PTS #: 7923
 *	 Testing: MCAT scripts for 1 month
 *	 Module(s): vm/vm_pageout.c
 *
 *	Revision 1.10.2.1  1993/12/15  01:24:38  joel
 *	 Reviewer: andyp,cfleck
 *	 Risk:low
 *	 Benefit or PTS #: 7375
 *	 Testing: MCAT .compute and .service scripts
 *	 Module(s):vm_pageout.c
 *	 R1.2 RTI approved.
 *
 * Revision 1.11.2.11  1994/06/20  17:16:22  rkl
 *  Removed BORG capability.
 *
 * Revision 1.11.2.10  1994/05/26  21:29:33  andyp
 * Reformatted the pageout pending printout.
 *
 * Revision 1.11.2.9  1994/05/25  07:45:05  andyp
 * Give the pageout thread "transmission" privileges (it can use
 * a private map and a private RDMA group) for ool data.
 *
 * Revision 1.11.2.8  1994/05/14  00:21:00  stans
 * -NORMA_IPC
 *
 * Revision 1.11.2.7  1994/05/12  18:58:09  andyp
 * Don't try to print the uid of the pager port when printing the
 * pageout reservation table -- just the port is fine.
 *
 * Revision 1.11.2.6  1994/05/11  15:05:26  rkl
 *  !db_vm will not report the DIPC uid when appropriate.
 *
 * Revision 1.11.2.5  1994/04/22  18:48:02  andyp
 * Added NORMA2 DIPC_IS_PROXY() checks.
 *
 * Revision 1.11.2.4  1994/04/05  23:25:48  stans
 *   !NORMA_IPC, compile with NORMA_IPC == 0, NORMA_{DEVICE,VM} == 1
 *
 * Revision 1.11.2.3  1994/03/10  07:24:05  rkl
 *  Fixed PGI warning for assert(!"some string here") style of assert.
 *
 * Revision 1.11.2.2  1994/03/02  18:42:09  andyp
 * Added some missing #ifdef's for compiling without NORMA.
 *
 * Revision 1.11.2.1  1994/01/26  02:10:10  stans
 *   Frozen page support: pageout daemon will move these pages to the end of
 *   the active queue, nothing more.
 *
 * Revision 1.11  1993/12/15  19:22:27  joel
 *  Reviewer: andyp, cfleck
 *  Risk:low
 *  Benefit or PTS #: 7375
 *  Testing: ORNL MCAT .compute and .service tests
 *  Module(s): vm_pageout.c
 *  Main line commit.
 *
 * Revision 1.10  1993/10/15  18:42:45  stans
 *    vm_pageout_reserve_terminate() target object is already locked remove
 *    explicit vm_lock/unlock calls around decrement of completions_pending.
 *
 * Revision 1.9  1993/10/01  19:59:38  andyp
 * Removed a "\t" from the !db_vm output.
 *
 * Revision 1.8  1993/09/28  18:09:36  andyp
 * Update for 1.2 release.
 *
 *
 *	27-Sep-93 [andyp@ssd.intel.com]
 *	Fully synchronous object termination can deadlock
 *	for the local case, so terminate pageout reservations
 *	only in the local case (semi-synchronous), leaving
 *	NORMA cases fully synchronous. (hints from alanl@osf.org)
 *
 *	- Turn on code in vm_pageout_completed to wake up
 *	anyone waiting for pageouts to complete.
 * 	- Added PENDING_TRACKING fields to follow
 *	data_write_completed activity through VM.
 *	- In db_vm, display first few entries in use in the
 *	vm_pageout_pending table.
 *	[alanl@osf.org]
 *
 * Revision 1.7  1993/07/22  02:28:15  andyp
 * Recovered OSF's log.  Updated to match OSF's version.
 *
 * Revision 1.6  1993/07/01  15:33:07  terry
 * source sync with nmk13.25
 *
 * Revision 1.5  1993/06/30  23:02:10  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:43:11  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:56:15  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:21:19  dleslie
 * Patch release of April 23
 *
 * Revision 1.4  1993/04/16  05:24:55  SSD
 * third code drop for page flow control.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: vm_pageout.c,v
 * Revision 1.2.4.11  1993/07/06  15:40:39  rwd
 * 	Rename netipc_able_continue_recv() to vm_netipc_able_continue_recv()
 * 	so we can also make some norma based decisions before resuming
 * 	receiving.
 * 	[93/07/02            rwd]
 *
 * Revision 1.2.4.10  1993/06/02  13:15:38  dwm
 * 	Fix bad assertion in vm_pageout_completed(); bug 670.
 * 	[1993/06/02  13:14:45  dwm]
 * 
 * Revision 1.2.4.9  1993/05/26  18:57:52  alanl
 * 	Split external reservations into two categories:  local
 * 	and remote.  This distinction is necessary to eliminate
 * 	a deadlock wherein reservations pending for a suffering
 * 	local vnode pager can prevent remote paging.  Update comments.
 * 	[1993/05/26  18:56:46  alanl]
 * 
 * Revision 1.2.4.8  1993/05/04  20:01:51  alanl
 * 	Fix the stupid force printf to line up.
 * 	[1993/05/04  20:01:30  alanl]
 * 
 * Revision 1.2.4.7  1993/05/03  19:13:33  alanl
 * 	1.  Remove offending assertion that assumes all pagers
 * 	support flow control.
 * 	2.  When sleeping because no reservations were available,
 * 	ensure the sleep time is at least vm_pageout_empty_wait.
 * 	3.  Clean up paging loop a bit.
 * 	[1993/05/03  18:50:45  alanl]
 * 
 * Revision 1.2.4.6  1993/04/28  15:35:47  alanl
 * 	Support older pagers by always granting reservations
 * 	for pages sent to them.  Defeats paging flow control,
 * 	so the assumption is that we will never page heavily
 * 	to an older pager.  For the intended purpose, supporting
 * 	a cluster running a BSD first server, this is a safe
 * 	assumption.
 * 	Also:  fix annoying #ifdef and assertion compilation errors.
 * 	[1993/04/28  15:35:16  alanl]
 * 
 * Revision 1.2.4.5  1993/04/27  20:11:26  alanl
 * 	Paging flow control (NORMA_VM).  Fix pageout overflow
 * 	reservations to allow many more overflows for the
 * 	external reservation category.  Allow per-category
 * 	overflow counts.  [dlb@osf.org]
 * 	Fix pageout scan loop to wait while either internal
 * 	or external reservations are pending.  [dlb@osf.org]
 * 	Add comment explaining overflow reservations (which was
 * 	part of the original flow control check-in).  [alanl@osf.org]
 * 	[1993/04/27  18:55:17  alanl]
 * 
 * Revision 1.2.4.4  1993/04/22  20:24:28  bernard
 * 	Removed write complete asserts (added by write complete changes)
 * 	 so older servers can run.
 * 	[93/04/22            bernard]
 * 
 * Revision 1.2.4.3  1993/04/15  22:49:28  alanl
 * 	Paging flow control (NORMA_VM).  New pageout reservation
 * 	code.  Take a reservation from the pageout scan loop and
 * 	handle failure to acquire a reservation.  Verify that all
 * 	pageouts go to objects that return data_write_completed
 * 	messages.  [alanl, sjs]
 * 	Fixes and tuning.  [alanl]
 * 	[1993/04/15  22:14:52  alanl]
 * 
 * Revision 1.2.4.2  1993/01/20  02:48:31  dwm
 * 	Drop reserved pool to something reasonable: 3*burst_max for starters.
 * 	[1993/01/20  02:44:33  dwm]
 * 
 * Revision 1.2  1992/11/25  01:20:24  robert
 * 	integrate changes below for norma_14
 * 
 * 	Dave Mitchell (dwm) at Open Software Foundation
 * 	Fix single '&' botch, in vm_pageout_scan().
 * 	[1992/11/13  19:44:51  robert]
 * 
 * 	fix history
 * 	[1992/11/09  22:43:42  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:55:27  robert]
 * 
 * Revision 0.0  92/10/21            alan
 * 	The reserved page pool computation incorrectly accounted
 * 	for the number of pages required for pull_receive messages.
 * 	Fix the computation to account for one pull_receive per
 * 	page-out message.  To Be Done:  verify in detail every
 * 	source of VM demands under paging load.  The pool size
 * 	may still be too small.
 * 
 * 	Revision 1.1  1992/11/05  21:01:38  robert
 * 	Initial revision
 * 	[92/10/21            alan]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.21.5.3  92/09/15  17:37:26  jeffreyh
 * 	Add a few more pages to the reserved pool.
 * 	[92/08/28            jeffreyh]
 * 
 * 	Discard error pages if encountered (just like absent).
 * 	[92/08/21            dlb]
 * 
 * 	With Jeffreyh:  Surgery on the suspend/resume protocol demands
 * 	that we eliminate older wakeups of the kserver_pageout_support
 * 	thread.
 * 	[92/08/20            alanl]
 * 
 * Revision 2.21.5.2  92/06/24  18:04:11  jeffreyh
 * 	Wakeup the pagout_support_thread when there is enough memory
 * 	available to continue receiving kmsgs without filtering.
 * 	[92/06/10            jeffreyh]
 * 
 * 	Added netipc_able_continue_mgs() under NORMA_VM conditional. It
 * 	 is used to judge if a suspended norma message can be restarted.
 * 	[92/06/03            jeffreyh]
 * 
 * Revision 2.21.5.1  92/05/29  11:49:15  jeffreyh
 * 	NORMA only:  drastically increase the size
 * 	of the reserved VM pool to account for
 * 	deficiencies in the NORMA pageout path.
 * 	[92/05/07            alanl]
 * 
 * Revision 2.21  91/12/11  08:44:16  jsb
 * 	Added vm_pageout_active, vm_pageout_inactive,
 * 	and other measurement counters.  Fixed the log.
 * 	[91/11/24            rpd]
 * 
 * Revision 2.20  91/10/09  16:20:36  af
 * 	Added vm_pageout_pause_count, vm_pageout_pause_max technology
 * 	so that vm_pageout_burst_wait can decrease as well as increase.
 * 	[91/10/04            rpd]
 * 
 * Revision 2.19  91/08/28  11:18:54  jsb
 * 	Fixed vm_pageout_scan to send pages to the default pager
 * 	when memory gets very tight.  This is the same idea as the old
 * 	vm_pageout_reserved_external and vm_pageout_reserved_internal,
 * 	but with a different implementation that forcibly double-pages.
 * 	[91/08/07            rpd]
 * 	Precious page support: return precious pages on pageout, use
 * 	memory_object_data_return instead of memory_object_data_write
 * 	when appropriate,
 * 	[91/07/03  14:20:57  dlb]
 * 
 * Revision 2.18  91/07/01  08:28:13  jsb
 * 	Add missing includes of vm/vm_map.h and kern/thread.h.
 * 	[91/06/29  16:53:36  jsb]
 * 
 * Revision 2.17  91/06/17  15:49:37  jsb
 * 	NORMA_VM: declare definitions for memory_object_data_{initialize,write}
 * 	since they won't be provided by interposed-on memory_object_user.h.
 * 	[91/06/17  11:13:22  jsb]
 * 
 * Revision 2.16  91/05/18  14:41:49  rpd
 * 	Fixed vm_pageout_continue to always call vm_pageout_scan.
 * 	Revamped vm_pageout_scan.  Now it recalculates vm_page_inactive_target,
 * 	gradually moves pages from the active list to the inactive list,
 * 	looks at vm_page_free_wanted, handles absent and fictitious pages,
 * 	and blocks with a continuation (vm_pageout_scan_continue), which
 * 	uses vm_page_laundry_count to adjust vm_pageout_burst_wait.
 * 	[91/04/06            rpd]
 * 
 * 	Changed vm_page_free_wanted to unsigned int.
 * 	[91/04/05            rpd]
 * 	Added vm_page_grab_fictitious.
 * 	[91/03/29            rpd]
 * 	Changed vm_page_init.
 * 	[91/03/24            rpd]
 * 
 * Revision 2.15  91/05/14  17:50:59  mrt
 * 	Correcting copyright
 * 
 * Revision 2.14  91/03/16  15:06:50  rpd
 * 	Modified vm_pageout_scan for further vm_page_deactivate changes.
 * 	Also changed it to ignore pages in dead objects.
 * 	[91/03/11            rpd]
 * 	Added vm_pageout_continue.
 * 	[91/01/20            rpd]
 * 
 * Revision 2.13  91/02/05  17:59:57  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:34:17  mrt]
 * 
 * Revision 2.12  91/01/08  16:45:57  rpd
 * 	Added net_kmsg_collect.
 * 	[91/01/05            rpd]
 * 	Added consider_task_collect, consider_thread_collect.
 * 	[91/01/03            rpd]
 * 
 * 	Added stack_collect.
 * 	[90/12/31            rpd]
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * 	Ensure that vm_page_free_target is at least five pages
 * 	larger than vm_page_free_min, to avoid vm_page_wait deadlock.
 * 	[90/11/19            rpd]
 * 
 * 	Replaced swapout_threads with consider_zone_gc.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.11  90/11/05  14:35:03  rpd
 * 	Modified vm_pageout_scan for new vm_page_deactivate protocol.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.10  90/10/12  13:06:53  rpd
 * 	Fixed vm_pageout_page to take busy pages.
 * 	[90/10/09            rpd]
 * 
 * 	In vm_pageout_scan, check for new software reference bit
 * 	in addition to using pmap_is_referenced.  Remove busy pages
 * 	found on the active and inactive queues.
 * 	[90/10/08            rpd]
 * 
 * Revision 2.9  90/08/27  22:16:02  dbg
 * 	Fix error in initial assumptions: vm_pageout_setup must take a
 * 	BUSY page, to prevent the page from being scrambled by pagein.
 * 	[90/07/26            dbg]
 * 
 * Revision 2.8  90/06/19  23:03:22  rpd
 * 	Locking fix for vm_pageout_page from dlb/dbg.
 * 	[90/06/11            rpd]
 * 
 * 	Correct initial condition in vm_pageout_page (page is NOT busy).
 * 	Fix documentation for vm_pageout_page and vm_pageout_setup.
 * 	[90/06/05            dbg]
 * 
 * 	Fixed vm_object_unlock type in vm_pageout_page.
 * 	[90/06/04            rpd]
 * 
 * Revision 2.7  90/06/02  15:11:56  rpd
 * 	Removed pageout_task and references to kernel_vm_space.
 * 	[90/04/29            rpd]
 * 
 * 	Made vm_pageout_burst_max, vm_pageout_burst_wait tunable.
 * 	[90/04/18            rpd]
 * 	Converted to new IPC and vm_map_copyin_object.
 * 	[90/03/26  23:18:10  rpd]
 * 
 * Revision 2.6  90/05/29  18:39:52  rwd
 * 	Picked up new vm_pageout_page from dbg.
 * 	[90/05/17            rwd]
 * 	Rfr change to send multiple pages to pager at once.
 * 	[90/04/12  13:49:13  rwd]
 * 
 * Revision 2.5  90/05/03  15:53:21  dbg
 * 	vm_pageout_page flushes page only if asked; otherwise, it copies
 * 	the page.
 * 	[90/03/28            dbg]
 * 
 * 	If an object's pager is not initialized, don't page out to it.
 * 	[90/03/21            dbg]
 * 
 * Revision 2.4  90/02/22  20:06:48  dbg
 * 		PAGE_WAKEUP --> PAGE_WAKEUP_DONE to reflect the fact that it
 * 		clears the busy flag.
 * 		[89/12/13            dlb]
 * 
 * Revision 2.3  90/01/11  11:48:27  dbg
 * 	Pick up recent changes from mainline:
 * 
 * 		Eliminate page lock when writing back a page.
 * 		[89/11/09            mwyoung]
 * 
 * 		Account for paging_offset when setting external page state.
 * 		[89/10/16  15:29:08  af]
 * 
 * 		Improve reserve tuning... it was a little too low.
 * 
 * 		Remove laundry count computations, as the count is never used.
 * 		[89/10/10            mwyoung]
 * 
 * 		Only attempt to collapse if a memory object has not
 * 		been initialized.  Don't bother to PAGE_WAKEUP in
 * 		vm_pageout_scan() before writing back a page -- it
 * 		gets done in vm_pageout_page().
 * 		[89/10/10            mwyoung]
 * 
 * 		Don't reactivate a page when creating a new memory
 * 		object... continue on to page it out immediately.
 * 		[89/09/20            mwyoung]
 * 
 * 		Reverse the sensing of the desperately-short-on-pages tests.
 * 		[89/09/19            mwyoung]
 * 		Check for absent pages before busy pages in vm_pageout_page().
 * 		[89/07/10  00:01:22  mwyoung]
 * 
 * 		Allow dirty pages to be reactivated.
 * 		[89/04/22            mwyoung]
 * 
 * 		Don't clean pages that are absent, in error, or not dirty in
 * 		vm_pageout_page().  These checks were previously issued
 * 		elsewhere.
 * 		[89/04/22            mwyoung]
 * 
 * Revision 2.2  89/09/08  11:28:55  dbg
 * 	Reverse test for internal_only pages.  Old sense left pageout
 * 	daemon spinning.
 * 	[89/08/15            dbg]
 * 
 * 18-Jul-89  David Golub (dbg) at Carnegie-Mellon University
 *	Changes for MACH_KERNEL:
 *	. Removed non-XP code.
 *	Count page wiring when sending page to default pager.
 *	Increase reserved page count.
 *	Make 'internal-only' count LARGER than reserved page count.
 *
 * Revision 2.18  89/06/12  14:53:05  jsb
 * 	Picked up bug fix (missing splimps) from Sequent via dlb.
 * 	[89/06/12  14:39:28  jsb]
 * 
 * Revision 2.17  89/04/18  21:27:08  mwyoung
 * 	Recent history [mwyoung]:
 * 		Keep hint when pages are written out (call
 * 		 vm_external_state_set).
 * 		Use wired-down fictitious page data structure for "holding_page".
 * 	History condensation:
 * 		Avoid flooding memory managers by using timing [mwyoung].
 * 		New pageout daemon for external memory management
 * 		 system [mwyoung].
 * 	[89/04/18            mwyoung]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	vm/vm_pageout.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	The proverbial page-out daemon.
 */

#include <mach_pagemap.h>
#include <norma_vm.h>
#include <norma_ipc.h>
#include <mach_kdb.h>

#include <mach/mach_types.h>
#include <mach/memory_object.h>
#include <mach/memory_object_default.h>
#include <mach/memory_object_user.h>
#include <mach/vm_param.h>
#include <mach/vm_statistics.h>
#include <kern/counters.h>
#include <kern/thread.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <machine/vm_tuning.h>
#include <machine/thread.h>		/* for KEEP_STACKS */
#if	NORMA_VM
#include <kern/ipc_sched.h>		/* for convert_ipc_timeout_to_ticks */
#endif

#ifndef	VM_PAGEOUT_BURST_MAX
#define	VM_PAGEOUT_BURST_MAX	10		/* number of pages */
#endif	VM_PAGEOUT_BURST_MAX

#if    VCF
extern	int	*vcf_dirbase;
#define	VCF_SWAP	105
#endif VCF


#if	NORMA_VM
/*
 *	Number of pageouts to insert into a paging
 *	tree at any one time.
 */
#ifndef	VM_PAGEOUT_INTERNAL_RESERVATION_MAX
#define	VM_PAGEOUT_INTERNAL_RESERVATION_MAX	10
#endif

#ifndef	VM_PAGEOUT_EXTERNAL_RESERVATION_MAX
#define	VM_PAGEOUT_EXTERNAL_RESERVATION_MAX	10
#endif

#ifndef	VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX
#define	VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX	10
#endif

#ifndef	VM_PAGEOUT_LOCAL_RESERVATION_MAX
#define	VM_PAGEOUT_LOCAL_RESERVATION_MAX	0
#endif
#endif	/* NORMA_VM */

#ifndef	VM_PAGEOUT_BURST_MIN
#define	VM_PAGEOUT_BURST_MIN	5		/* number of pages */
#endif	VM_PAGEOUT_BURST_MIN

#ifndef	VM_PAGEOUT_BURST_WAIT
#define	VM_PAGEOUT_BURST_WAIT	30		/* milliseconds per page */
#endif	VM_PAGEOUT_BURST_WAIT

#ifndef	VM_PAGEOUT_EMPTY_WAIT
#define VM_PAGEOUT_EMPTY_WAIT	200		/* milliseconds */
#endif	VM_PAGEOUT_EMPTY_WAIT

#ifndef	VM_PAGEOUT_PAUSE_MAX
#define	VM_PAGEOUT_PAUSE_MAX	10		/* number of pauses */
#endif	VM_PAGEOUT_PAUSE_MAX

/*
 *	To obtain a reasonable LRU approximation, the inactive queue
 *	needs to be large enough to give pages on it a chance to be
 *	referenced a second time.  This macro defines the fraction
 *	of active+inactive pages that should be inactive.
 *	The pageout daemon uses it to update vm_page_inactive_target.
 *
 *	If vm_page_free_count falls below vm_page_free_target and
 *	vm_page_inactive_count is below vm_page_inactive_target,
 *	then the pageout daemon starts running.
 */

#ifndef	VM_PAGE_INACTIVE_TARGET
#define	VM_PAGE_INACTIVE_TARGET(avail)	((avail) * 2 / 3)
#endif	VM_PAGE_INACTIVE_TARGET

/*
 *	Once the pageout daemon starts running, it keeps going
 *	until vm_page_free_count meets or exceeds vm_page_free_target.
 */

#ifndef	VM_PAGE_FREE_TARGET
#define	VM_PAGE_FREE_TARGET(free)	(15 + (free) / 80)
#endif	VM_PAGE_FREE_TARGET

/*
 *	The pageout daemon always starts running once vm_page_free_count
 *	falls below vm_page_free_min.
 */

#ifndef	VM_PAGE_FREE_MIN
#define	VM_PAGE_FREE_MIN(free)	(10 + (free) / 100)
#endif	VM_PAGE_FREE_MIN

/*
 *	When vm_page_free_count falls below vm_page_free_reserved,
 *	only vm-privileged threads can allocate pages.  vm-privilege
 *	allows the pageout daemon and default pager (and any other
 *	associated threads needed for default pageout) to continue
 *	operation by dipping into the reserved pool of pages.
 */

#ifndef	VM_PAGE_FREE_RESERVED
#if	NORMA_VM
/*
 *	Once this formula is understood/corrected, COMMENT IT. XXXX
 */
#define	VM_PAGE_FREE_RESERVED	(vm_pageout_burst_max*3)
#else	/* NORMA_VM */
#define	VM_PAGE_FREE_RESERVED	15
#endif	/* NORMA_VM */
#endif	VM_PAGE_FREE_RESERVED

/*
 *	When vm_page_free_count falls below vm_pageout_reserved_internal,
 *	the pageout daemon no longer trusts external pagers to clean pages.
 *	External pagers are probably all wedged waiting for a free page.
 *	It forcibly double-pages dirty pages belonging to external objects,
 *	getting the pages to the default pager to clean.
 */

#ifndef	VM_PAGEOUT_RESERVED_INTERNAL
#define	VM_PAGEOUT_RESERVED_INTERNAL(reserve)	((reserve) - 5)
#endif	VM_PAGEOUT_RESERVED_INTERNAL

/*
 *	When vm_page_free_count falls below vm_pageout_reserved_really,
 *	the pageout daemon stops work entirely to let the default pager
 *	catch up (assuming the default pager has pages to clean).
 *	Beyond this point, it is too dangerous to consume memory
 *	even for memory_object_data_write messages to the default pager.
 */

#ifndef	VM_PAGEOUT_RESERVED_REALLY
#define	VM_PAGEOUT_RESERVED_REALLY(reserve)	((reserve) - 10)
#endif	VM_PAGEOUT_RESERVED_REALLY

extern void vm_pageout_continue();
extern void vm_pageout_scan_continue();

unsigned int vm_pageout_reserved_internal = 0;
unsigned int vm_pageout_reserved_really = 0;

unsigned int vm_pageout_burst_max = 0;
unsigned int vm_pageout_burst_min = 0;
unsigned int vm_pageout_burst_wait = 0;		/* milliseconds per page */
unsigned int vm_pageout_empty_wait = 0;		/* milliseconds */
unsigned int vm_pageout_pause_count = 0;
unsigned int vm_pageout_pause_max = 0;

#if	NORMA_VM
/*
 *	Track pageout reservations.   In a NORMA system,
 *	the vm_page_laundry_count mechanism does not
 *	suffice to throttle pageout traffic.
 *
 *	In the first place, NORMA IPC frees up pages as
 *	soon as they move off-node but long before the
 *	target pager receives (much less handles) the
 *	pageout messages.  Thus, it is possible for one
 *	or more clients to swamp a common vnode pager
 *	with pageout requests.  We remedy this problem
 *	by mandating a data_write_completed message in
 *	response to every data_write, data_initialize
 *	or data_return.  In theory, we could then alter
 *	NORMA IPC to defer freeing pageout messages and
 *	let data_write_completed do that job, thus once
 *	again taking advantage of the laundry count scheme.
 *	However, the laundry count scheme does not take
 *	into account pageouts generated by lock_request
 *	and pageouts sent to an external pager that do
 *	not involve double-paging.
 *
 *	The pageout reservation system is a way to extend
 *	the laundry count mechanism for use by clients
 *	other than the pageout scan path.  Moreover,
 *	the pageout reservation system could easily be
 *	generalized to support per-node or per-pager
 *	reservation quotas.
 *
 *	N.B.  The reservation variables are simple integers
 *	to catch underflow problems.
 *
 *	N.B.  As of this date, 8 April 1993, this code has
 *	not been tested on a shared-memory multiprocessor.
 */
#define PAGEOUT_RESERVE_FREE	(-1)
#define	RESERVATIONS(type)						\
	vm_pageout_reservations[(type)]
#define	RESERVATION_AVAILABLE(type)					\
	(vm_pageout_reservations[(type)] <				\
	 vm_pageout_reservation_max[(type)])
#define	vm_pageout_reservation_wait(type)				\
	assert_wait((int)&vm_pageout_reservation_needed[(type)], FALSE);

decl_simple_lock_data(,vm_pageout_reservation_lock)
#define	RESERVATION_LOCK	simple_lock(&vm_pageout_reservation_lock)
#define	RESERVATION_UNLOCK	simple_unlock(&vm_pageout_reservation_lock)

int	vm_pageout_reservation_max[PAGEOUT_RESERVE_MAX];
int	vm_pageout_reservations[PAGEOUT_RESERVE_MAX];
int	vm_pageout_reservation_needed[PAGEOUT_RESERVE_MAX];

#define	RESERVATION_DEBUG	MACH_ASSERT
#if	!RESERVATION_DEBUG
#define	vm_pageout_reservation_assertions(line_num)
#endif

int     vm_pageout_reserve_local_extobj=0;
int     vm_pageout_pending_notfound=0;

#define	RESERVATION_COUNTERS	1
#if	RESERVATION_COUNTERS
int	c_vm_pageout_reserve_granted[PAGEOUT_RESERVE_MAX];
int	c_vm_pageout_reserve_denied[PAGEOUT_RESERVE_MAX];
int	c_vm_pageout_reserve_noflow[PAGEOUT_RESERVE_MAX];
int	c_vm_pageout_completed[PAGEOUT_RESERVE_MAX];
int	c_vm_pageout_needed_max[PAGEOUT_RESERVE_MAX];
int	c_vm_pageout_wakeups[PAGEOUT_RESERVE_MAX];
int	c_vm_pageout_big_wakeups[PAGEOUT_RESERVE_MAX];
int	c_vm_pageout_terminated[PAGEOUT_RESERVE_MAX];
int	c_vm_pageout_reservation_total = 0;
int	c_vm_pageout_total_completed = 0;
#define	RC(op)	op
#else
#define	RC(op)
#endif	/* RESERVATION_COUNTERS */
#endif	/* NORMA_VM */

/*
 *	These variables record the pageout daemon's actions:
 *	how many pages it looks at and what happens to those pages.
 *	No locking needed because only one thread modifies the variables.
 */

unsigned int vm_pageout_active = 0;		/* debugging */
unsigned int vm_pageout_inactive = 0;		/* debugging */
unsigned int vm_pageout_inactive_nolock = 0;	/* debugging */
unsigned int vm_pageout_inactive_busy = 0;	/* debugging */
unsigned int vm_pageout_inactive_absent = 0;	/* debugging */
unsigned int vm_pageout_inactive_used = 0;	/* debugging */
unsigned int vm_pageout_inactive_clean = 0;	/* debugging */
unsigned int vm_pageout_inactive_dirty = 0;	/* debugging */
unsigned int vm_pageout_inactive_double = 0;	/* debugging */
#if	NORMA_VM
unsigned int vm_pageout_reservation_miss = 0;	/* debugging */
#endif

#if	NORMA_VM
/*
 * Define them here, since they won't be defined by memory_object_user.h.
 */
extern kern_return_t memory_object_data_initialize();
extern kern_return_t memory_object_data_write();

#define	PENDING_TRACKING	(MACH_ASSERT || MACH_KDB)

typedef struct pending_record {
	ipc_port_t	pager;
	vm_offset_t	offset;
	int		type;
#if	PENDING_TRACKING
	vm_object_t	object;
#endif
} pending_record;

#define	VM_PAGEOUT_PENDING_MAX	(VM_PAGEOUT_INTERNAL_RESERVATION_MAX +	   \
				 VM_PAGEOUT_EXTERNAL_RESERVATION_MAX +	   \
				 VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX + \
				 VM_PAGEOUT_LOCAL_RESERVATION_MAX)

pending_record	vm_pageout_pending[VM_PAGEOUT_PENDING_MAX];

int	vm_pageout_pending_max = VM_PAGEOUT_PENDING_MAX;

/*
 *	Attempt to reserve a pageout slot for
 *	a range of pages.
 */
boolean_t
vm_pageout_reserve(object, offset, size, type, force)
vm_object_t	object;
vm_offset_t	offset;
vm_size_t	size;
int		type;
boolean_t	force;
{
	pending_record	*p;

	/*
	 *	The current system is limited to pageouts
	 *	of one page at a time.  Perhaps someday
	 *	we will make this function smart enough to
	 *	take into account what pager is being targeted
	 *	and (for NORMA) what node.
	 */
	assert(size == page_size);
	assert(ip_kotype(object->pager) == IKOT_PAGER ||
	       ip_kotype(object->pager) == IKOT_PAGER_TERMINATING);
	assert(force == FALSE);		/* XXX alan XXX */

	RESERVATION_LOCK;
	assert(vm_pageout_reservations[type] >= 0);
	assert(vm_pageout_reservations[type] <=
	       vm_pageout_reservation_max[type]);

	assert(type != PAGEOUT_RESERVE_LOCAL);

	/*
	 *	Risky:  support older pagers by always
	 *	allowing pageouts to them.  The intended
	 *	purpose is to allow clusters to continue
	 *	operating with a first BSD server and
	 *	a second AD server.  In general, of course,
	 *	this check defeats the purpose of paging
	 *	flow control.
	 */
	if (object->pager_modwc == FALSE) {
		RC(++c_vm_pageout_reserve_noflow[type]);
		return TRUE;
	}

	/*
	 *	Deny a reservation attempt if too many reservations
	 *	are already pending.
	 */
	if (!RESERVATION_AVAILABLE(type)) {
		RESERVATION_UNLOCK;
		RC(++c_vm_pageout_reserve_denied[type]);
		return FALSE;
 	}

	++vm_pageout_reservations[type];
	++object->completions_pending;
	RC(++c_vm_pageout_reservation_total);
	RC(++c_vm_pageout_reserve_granted[type]);

	for (p = vm_pageout_pending;
	     p < vm_pageout_pending + vm_pageout_pending_max;
	     ++p) {
		if (p->type != PAGEOUT_RESERVE_FREE)
			continue;
		p->pager = object->pager;
		p->offset = offset;
		p->type = type;
#if	PENDING_TRACKING
		p->object = object;
#endif
		RESERVATION_UNLOCK;
		return TRUE;
	}
	panic("vm_pageout_reserve");
}


/*
 *	Update pageout reservation data to
 *	account for the completion of paging
 *	on a range.
 */
void
vm_pageout_completed(object, offset, size)
vm_object_t	object;
vm_offset_t	offset;
vm_size_t	size;
{
	pending_record	*p;
	boolean_t	found;
	int 		found_type;

	/*
	 *	Should receive a completion length
	 *	that is a multiple of our page size;
	 *	and in the current system, we are
	 *	limited to one page at a time.
	 */
	assert(size % page_size == 0);
	assert(size == page_size);
	assert(object != VM_OBJECT_NULL);

	RC(++c_vm_pageout_total_completed);
	RESERVATION_LOCK;
	vm_pageout_reservation_assertions(__LINE__);

	found = FALSE;
	for (p = vm_pageout_pending;
	     p < vm_pageout_pending + vm_pageout_pending_max;
	     ++p) {
		if (p->type == PAGEOUT_RESERVE_FREE ||
		    p->pager != object->pager || p->offset != offset)
			continue;
		found = TRUE;
		assert(vm_pageout_reservations[p->type] > 0);
		--vm_pageout_reservations[p->type];
		RC(++c_vm_pageout_completed[p->type]);
		vm_object_lock(object);
		object->completions_pending--;
		if (object->completions_pending == 0)
			vm_object_wakeup(object, VM_OBJECT_EVENT_COMPLETION);
		vm_object_unlock(object);
		found_type = p->type;
		p->type = PAGEOUT_RESERVE_FREE;
		break;
	}

	if (found == FALSE) {
		vm_pageout_pending_notfound++;
		RESERVATION_UNLOCK;
		return;
        }
         	

	/*
	 *	It may be a better idea to only wake
	 *	up one waiter at a time. XXX
	 */
	if (vm_pageout_reservation_needed[found_type] > 0) {
#if	RESERVATION_COUNTERS
		++c_vm_pageout_wakeups[found_type];
		if (vm_pageout_reservation_needed[found_type] > 1)
			++c_vm_pageout_big_wakeups[found_type];
#endif
		thread_wakeup((int)&vm_pageout_reservation_needed[found_type]);
	}

	RESERVATION_UNLOCK;
}


void
vm_pageout_await_completed(type)
	int	type;
{
	RESERVATION_LOCK;
	assert(type > PAGEOUT_RESERVE_FREE && type < PAGEOUT_RESERVE_MAX);
	assert(vm_pageout_reservations[type] >= 0);
	vm_pageout_reservation_assertions(__LINE__);
	while (vm_pageout_reservations[type] >=
	       vm_pageout_reservation_max[type]) {
		assert(vm_pageout_reservation_needed[type] >= 0);
		++vm_pageout_reservation_needed[type];
#if	RESERVATION_COUNTERS
		if (vm_pageout_reservation_needed[type] >
		    c_vm_pageout_needed_max[type])
			c_vm_pageout_needed_max[type] =
				vm_pageout_reservation_needed[type];
#endif
		vm_pageout_reservation_wait(type);
		RESERVATION_UNLOCK;
		thread_block((void (*)()) 0);
		RESERVATION_LOCK;
		--vm_pageout_reservation_needed[type];
		assert(vm_pageout_reservation_needed[type] >= 0);
	}
	RESERVATION_UNLOCK;
}


#if	RESERVATION_DEBUG
/*
 *	Check the state of the page reservation system.
 */
vm_pageout_reservation_assertions(line)
int	line;
{
	int	type;

	assert(vm_pageout_reservations[0] > 0 ||
	       vm_pageout_reservations[1] > 0 ||
	       vm_pageout_reservations[2] > 0 ||
	       vm_pageout_reservations[3] > 0);
	for (type = 0; type < PAGEOUT_RESERVE_MAX; ++type)
		assert(vm_pageout_reservations[type] <=
		       vm_pageout_reservation_max[type]);
}
#endif	/* RESERVATION_DEBUG */
#endif	/* NORMA_VM */


/*
 *	Routine:	vm_pageout_setup
 *	Purpose:
 *		Set up a page for pageout.
 *
 *		Move or copy the page to a new object, as part
 *		of which it will be sent to its memory manager
 *		in a memory_object_data_write or memory_object_initialize
 *		message.
 *
 *		The "paging_offset" argument specifies the offset
 *		of the page within its external memory object.
 *
 *		The "new_object" and "new_offset" arguments
 *		indicate where the page should be moved.
 *
 *		The "flush" argument specifies whether the page
 *		should be flushed from its object.  If not, a
 *		copy of the page is moved to the new object.
 *
 *	In/Out conditions:
 *		The page in question must not be on any pageout queues,
 *		and must be busy.  The object to which it belongs
 *		must be unlocked, and the caller must hold a paging
 *		reference to it.  The new_object must not be locked.
 *
 *		If the page is flushed from its original object,
 *		this routine returns a pointer to a place-holder page,
 *		inserted at the same offset, to block out-of-order
 *		requests for the page.  The place-holder page must
 *		be freed after the data_write or initialize message
 *		has been sent.  If the page is copied,
 *		the holding page is VM_PAGE_NULL.
 *
 *		The original page is put on a paging queue and marked
 *		not busy on exit.
 */
vm_page_t
vm_pageout_setup(m, paging_offset, new_object, new_offset, flush)
	register vm_page_t	m;
	vm_offset_t		paging_offset;
	register vm_object_t	new_object;
	vm_offset_t		new_offset;
	boolean_t		flush;
{
	register vm_object_t	old_object = m->object;
	kern_return_t	rc;
	register vm_page_t	holding_page;
	register vm_page_t	new_m;

	assert(m->busy && !m->absent && !m->fictitious);
#if	NORMA2
	assert( pmap_is_vm_page_frozen(m->phys_addr) == FALSE );
#endif

	/*
	 *	If we are not flushing the page, allocate a
	 *	page in the object.  If we cannot get the
	 *	page, flush instead.
	 */
	if (!flush) {
		vm_object_lock(new_object);
		new_m = vm_page_alloc(new_object, new_offset);
		if (new_m == VM_PAGE_NULL)
			flush = TRUE;
		vm_object_unlock(new_object);
	}

	if (flush) {
		/*
		 *	Create a place-holder page where the old one was,
		 *	to prevent anyone from attempting to page in this
		 *	page while we`re unlocked.
		 */
		while ((holding_page = vm_page_grab_fictitious())
							== VM_PAGE_NULL)
			vm_page_more_fictitious();

		vm_object_lock(old_object);
		vm_page_lock_queues();
		vm_page_remove(m);
		vm_page_unlock_queues();
		PAGE_WAKEUP_DONE(m);

		vm_page_lock_queues();
		vm_page_insert(holding_page, old_object, m->offset);
		vm_page_unlock_queues();

		/*
		 *	Record that this page has been written out
		 */
#if	MACH_PAGEMAP
		vm_external_state_set(old_object->existence_info,
					paging_offset,
					VM_EXTERNAL_STATE_EXISTS);
#endif	MACH_PAGEMAP

		vm_object_unlock(old_object);

		vm_object_lock(new_object);

		/*
		 *	Move this page into the new object
		 */

		vm_page_lock_queues();
		vm_page_insert(m, new_object, new_offset);
		vm_page_unlock_queues();

		m->dirty = TRUE;
		m->precious = FALSE;
		m->page_lock = VM_PROT_NONE;
		m->unlock_request = VM_PROT_NONE;
	}
	else {
		/*
		 *	Copy the data into the new page,
		 *	and mark the new page as clean.
		 */
		vm_page_copy(m, new_m);

		vm_object_lock(old_object);
		m->dirty = FALSE;
		pmap_clear_modify(m->phys_addr);

		/*
		 *	Deactivate old page.
		 */
		vm_page_lock_queues();
		vm_page_deactivate(m);
		vm_page_unlock_queues();

		PAGE_WAKEUP_DONE(m);

		/*
		 *	Record that this page has been written out
		 */

#if	MACH_PAGEMAP
		vm_external_state_set(old_object->existence_info,
					paging_offset,
					VM_EXTERNAL_STATE_EXISTS);
#endif	MACH_PAGEMAP

		vm_object_unlock(old_object);

		vm_object_lock(new_object);

		/*
		 *	Use the new page below.
		 */
		m = new_m;
		m->dirty = TRUE;
		assert(!m->precious);
		PAGE_WAKEUP_DONE(m);
	}

	/*
	 *	Make the old page eligible for replacement again; if a
	 *	user-supplied memory manager fails to release the page,
	 *	it will be paged out again to the default memory manager.
	 *
	 *	Note that pages written to the default memory manager
	 *	must be wired down -- in return, it guarantees to free
	 *	this page, rather than reusing it.
	 */

	vm_page_lock_queues();
	vm_stat.pageouts++;
	if (m->laundry) {
		/*
		 *	vm_pageout_scan is telling us to put this page
		 *	at the front of the inactive queue, so it will
		 *	be immediately paged out to the default pager.
		 */

		assert(!old_object->internal);
		m->laundry = FALSE;

		queue_enter_first(&vm_page_queue_inactive, m,
				  vm_page_t, pageq);
		m->inactive = TRUE;
		vm_page_inactive_count++;
	} else if (old_object->internal) {
		m->laundry = TRUE;

		vm_page_laundry_count++;

		vm_page_wire(m);
	} else
		vm_page_activate(m);
	vm_page_unlock_queues();

	/*
	 *	Since IPC operations may block, we drop locks now.
	 *	[The placeholder page is busy, and we still have
	 *	paging_in_progress incremented.]
	 */

	vm_object_unlock(new_object);

	/*
	 *	Return the placeholder page to simplify cleanup.
	 */

	return (flush ? holding_page : VM_PAGE_NULL);
}

/*
 *	Routine:	vm_pageout_page
 *	Purpose:
 *		Causes the specified page to be written back to
 *		the appropriate memory object.
 *
 *		The "initial" argument specifies whether this
 *		data is an initialization only, and should use
 *		memory_object_data_initialize instead of
 *		memory_object_data_write.
 *
 *		The "flush" argument specifies whether the page
 *		should be flushed from the object.  If not, a
 *		copy of the data is sent to the memory object.
 *
 *	In/out conditions:
 *		The page in question must not be on any pageout queues.
 *		The object to which it belongs must be locked.
 *	Implementation:
 *		Move this page to a completely new object, if flushing;
 *		copy to a new page in a new object, if not.
 */
void
vm_pageout_page(m, initial, flush)
	register vm_page_t	m;
	boolean_t		initial;
	boolean_t		flush;
{
	vm_map_copy_t		copy;
	register vm_object_t	old_object;
	register vm_object_t	new_object;
	register vm_page_t	holding_page;
	vm_offset_t		paging_offset;
	kern_return_t		rc;
	boolean_t		precious_clean;

	assert(m->busy);

#if	NORMA2
assert( pmap_is_vm_page_frozen(m->phys_addr) == FALSE );
#endif

	/*
	 *	Cleaning but not flushing a clean precious page is a
	 *	no-op.  Remember whether page is clean and precious now
	 *	because vm_pageout_setup will mark it dirty and not precious.
	 *
	 * XXX Check if precious_clean && !flush can really happen.
	 */
	precious_clean = (!m->dirty) && m->precious;
	if (precious_clean && !flush) {
		PAGE_WAKEUP_DONE(m);
		assert(0 /*!"reservation without pageout?"*/); /* alan */
		return;
	}

	/*
	 *	Verify that we really want to clean this page.
	 */
	if (m->absent || m->error || (!m->dirty && !m->precious)) {
		VM_PAGE_FREE(m);
		assert(0 /*!"reservation without pageout?"*/); /* alan */
		return;
	}

	/*
	 *	Create a paging reference to let us play with the object.
	 */
	old_object = m->object;
	paging_offset = m->offset + old_object->paging_offset;
	vm_object_paging_begin(old_object);
	vm_object_unlock(old_object);

	/*
	 *	Allocate a new object into which we can put the page.
	 */
	new_object = vm_object_allocate(PAGE_SIZE);

	/*
	 *	Move the page into the new object.
	 */
	holding_page = vm_pageout_setup(m,
				paging_offset,
				new_object,
				0,		/* new offset */
				flush);		/* flush */

	rc = vm_map_copyin_object(new_object, 0, PAGE_SIZE, &copy);
	assert(rc == KERN_SUCCESS);

	if (initial || old_object->use_old_pageout) {
		rc = (*(initial ? memory_object_data_initialize
			     : memory_object_data_write))
			(old_object->pager,
			 old_object->pager_request,
			 paging_offset, (pointer_t) copy, PAGE_SIZE);
	}
	else {
		rc = memory_object_data_return(
			 old_object->pager,
			 old_object->pager_request,
			 paging_offset, (pointer_t) copy, PAGE_SIZE,
			 !precious_clean, !flush);
	}

	if (rc != KERN_SUCCESS)
		vm_map_copy_discard(copy);

	/*
	 *	Clean up.
	 */
	vm_object_lock(old_object);
	if (holding_page != VM_PAGE_NULL)
	    VM_PAGE_FREE(holding_page);
	vm_object_paging_end(old_object);
}

/*
 *	vm_pageout_scan does the dirty work for the pageout daemon.
 *	It returns with vm_page_queue_free_lock held and
 *	vm_page_free_wanted == 0.
 */

int vm_pageout_scan_msec;	/* Debug */
int vm_pageout_waiting=0;
int vm_pageout_scan_blocks=0;
int vm_pageout_scan_sleeping;
thread_t vm_pageout_scan_thread;


void vm_show_pageout_thread(thread) 
thread_t thread;
{
	printf("Thread ID = %X\n",thread);
	printf("State = %X\n",thread->state);
	printf("Timer = %X\n",&thread->timer);
	printf("        Function: %X\n",thread->timer.fcn);
	printf("        Param   : %X\n",thread->timer.param);
	printf("        Ticks   : %X\n",thread->timer.ticks);
#if NCPUS >1
	printf("        Bound   : %X\n",thread->timer.bound_processor);
#endif
	printf("Wait Event = %X\n",thread->wait_event);
	printf("Wait Result = %X\n",thread->wait_result);
	printf("Suspend Count = %X\n",thread->suspend_count);
	printf("Bound Processor = %X\n",thread->bound_processor);
#if NCPUS >1
	printf("Last Processor = %X\n",thread->last_processor);
#endif
}

void vm_pageout_scan()
{
	unsigned int burst_count;
	unsigned int laundry_pages;
#if	NORMA_VM
	unsigned int reserve_try;
	unsigned int obj_type;
	unsigned int need_reservation;
#endif

	/*
	 *	We want to gradually dribble pages from the active queue
	 *	to the inactive queue.  If we let the inactive queue get
	 *	very small, and then suddenly dump many pages into it,
	 *	those pages won't get a sufficient chance to be referenced
	 *	before we start taking them from the inactive queue.
	 *
	 *	We must limit the rate at which we send pages to the pagers.
	 *	data_write messages consume memory, for message buffers and
	 *	for map-copy objects.  If we get too far ahead of the pagers,
	 *	we can potentially run out of memory.
	 *
	 *	We can use the laundry count to limit directly the number
	 *	of pages outstanding to the default pager.  A similar
	 *	strategy for external pagers doesn't work, because
	 *	external pagers don't have to deallocate the pages sent them,
	 *	and because we might have to send pages to external pagers
	 *	even if they aren't processing writes.  So we also
	 *	use a burst count to limit writes to external pagers.
	 *
	 *	When memory is very tight, we can't rely on external pagers to
	 *	clean pages.  They probably aren't running, because they
	 *	aren't vm-privileged.  If we kept sending dirty pages to them,
	 *	we could exhaust the free list.  However, we can't just ignore
	 *	pages belonging to external objects, because there might be no
	 *	pages belonging to internal objects.  Hence, we get the page
	 *	into an internal object and then immediately double-page it,
	 *	sending it to the default pager.
	 *
	 *	consider_zone_gc should be last, because the other operations
	 *	might return memory to zones.  When we pause we use
	 *	vm_pageout_scan_continue as our continuation, so we will
	 *	reenter vm_pageout_scan periodically and attempt to reclaim
	 *	internal memory even if we never reach vm_page_free_target.
	 */

#ifdef	KEEP_STACKS
    Restart:
#endif	KEEP_STACKS
	stack_collect();
	net_kmsg_collect();
	consider_task_collect();
	consider_thread_collect();
	consider_zone_gc();

#if	NORMA_VM
	/*
	 *	Set reserve_try to be reasonable number of pages that
	 *	can be denied before we go to sleep.
	 */
	reserve_try = vm_page_inactive_count;
#endif
	for (burst_count = 0;;) {
		register vm_page_t m;
		register vm_object_t object;
		unsigned int free_count;
		unsigned long active_list_length;

		/*
		 *	Recalculate vm_page_inactivate_target.
		 */

		vm_page_lock_queues();
		vm_page_inactive_target =
			VM_PAGE_INACTIVE_TARGET(vm_page_active_count +
						vm_page_inactive_count);

		/*
		 *	Move pages from active to inactive.
		 */
		active_list_length = vm_page_active_count;
		while ((vm_page_inactive_count < vm_page_inactive_target) &&
		       !queue_empty(&vm_page_queue_active) &&
		       (active_list_length != 0)) {
			register vm_object_t object;

			active_list_length--;
			vm_pageout_active++;
			m = (vm_page_t) queue_first(&vm_page_queue_active);
			assert(m->active && !m->inactive);
#if	NORMA2
			if ( pmap_is_vm_page_frozen(m->phys_addr) ) {
				/*
				 * Move page to Q end and continue.
				 */
				queue_remove(&vm_page_queue_active, m,
					     vm_page_t, pageq);
				queue_enter(&vm_page_queue_active, m,
					    vm_page_t, pageq);
				vm_page_unlock_queues();
				vm_page_lock_queues();
				continue;
			}
#endif	/* NORMA2 */
			object = m->object;
			if (!vm_object_lock_try(object)) {
				/*
				 *	Move page to end and continue.
				 */

				queue_remove(&vm_page_queue_active, m,
					     vm_page_t, pageq);
				queue_enter(&vm_page_queue_active, m,
					    vm_page_t, pageq);
				vm_page_unlock_queues();
				vm_page_lock_queues();
				continue;
			}

			/*
			 *	If the page is busy, then we pull it
			 *	off the active queue and leave it alone.
			 */

			if (m->busy) {
				vm_object_unlock(object);
				queue_remove(&vm_page_queue_active, m,
					     vm_page_t, pageq);
				m->active = FALSE;
				vm_page_active_count--;
				continue;
			}

			/*
			 *	Deactivate the page while holding the object
			 *	locked, so we know the page is still not busy.
			 *	This should prevent races between pmap_enter
			 *	and pmap_clear_reference.  The page might be
			 *	absent or fictitious, but vm_page_deactivate
			 *	can handle that.
			 */

#if	NORMA2
			assert( pmap_is_vm_page_frozen(m->phys_addr) == FALSE );
#endif
			vm_page_deactivate(m);
			vm_object_unlock(object);
		}

		/*
		 *	We are done if we have met our target *and*
		 *	nobody is still waiting for a page.
		 */

		simple_lock(&vm_page_queue_free_lock);
		free_count = vm_page_free_count;
		if ((free_count >= vm_page_free_target) &&
		    (vm_page_free_wanted == 0)) {
			vm_page_unlock_queues();
			break;
		}
		simple_unlock(&vm_page_queue_free_lock);

		/*
		 * Sometimes we have to pause:
		 *	1) No inactive pages - nothing to do.
		 *	2) Flow control - wait for pagers to catch up.
		 *	3) Extremely low memory - sending out dirty pages
		 *	consumes memory.  We don't take the risk of doing
		 *	this if the default pager already has work to do.
#if	NORMA_VM
		 *	Attempt to formulate conditions equivalent to
		 *	cases 1-3 for NORMA_VM.
#endif
		 */

		if (queue_empty(&vm_page_queue_inactive) ||
		    (burst_count >= vm_pageout_burst_max) ||
#if	NORMA_VM
		    (reserve_try == 0) ||
		    (!RESERVATION_AVAILABLE(PAGEOUT_RESERVE_INTERNAL)) ||
		    ((free_count < vm_pageout_reserved_really) &&
		     (RESERVATIONS(PAGEOUT_RESERVE_INTERNAL) > 0))
#else
		    (vm_page_laundry_count >= vm_pageout_burst_max) ||
		    ((free_count < vm_pageout_reserved_really) &&
		     (vm_page_laundry_count > 0))
#endif
		    ) {
			unsigned int pages, msecs;

			/*
			 *	vm_pageout_burst_wait is msecs/page.
			 *	If there is nothing for us to do, we wait
			 *	at least vm_pageout_empty_wait msecs.
			 */

#if	NORMA_VM
			RESERVATION_LOCK;
			laundry_pages =
				RESERVATIONS(PAGEOUT_RESERVE_INTERNAL);
			RESERVATION_UNLOCK;
#else
			laundry_pages = vm_page_laundry_count;
#endif
			if (laundry_pages > burst_count)
				pages = laundry_pages;
			else
				pages = burst_count;
			msecs = pages * vm_pageout_burst_wait;

#if	NORMA_VM
			if (reserve_try == 0 && msecs < vm_pageout_empty_wait)
				msecs = vm_pageout_empty_wait;
#endif

			if (queue_empty(&vm_page_queue_inactive) &&
			    (msecs < vm_pageout_empty_wait))
				msecs = vm_pageout_empty_wait;
			vm_page_unlock_queues();

#if	NORMA_VM
			assert(msecs > 0);
#endif
			vm_pageout_scan_msec=msecs;	/* Debug */
			thread_will_wait_with_timeout(current_thread(), msecs);
			counter(c_vm_pageout_scan_block++);
			vm_pageout_waiting=1;
			assert(cpu_number() == 0);
			vm_pageout_scan_blocks++;
			
			vm_pageout_scan_sleeping=1;
			vm_pageout_scan_thread=current_thread();
			thread_block(vm_pageout_scan_continue);
			vm_pageout_waiting=0;
#ifdef	KEEP_STACKS
			/*
			 *	Unfortunately, we don't have call_continuation
			 *	so we can't rely on tail-recursion.
			 */

			vm_pageout_scan_continue();
			goto Restart;
#else	KEEP_STACKS
			call_continuation(vm_pageout_scan_continue);
			/*NOTREACHED*/
#endif	KEEP_STACKS
		}

		vm_pageout_inactive++;
		m = (vm_page_t) queue_first(&vm_page_queue_inactive);
		assert(!m->active && m->inactive);

#if	NORMA2
		/* FLW: page should never be on the inactive list */
		assert( !pmap_is_vm_page_frozen(m->phys_addr) );
#endif	/* NORMA2 */

		object = m->object;

		/*
		 *	Try to lock object; since we've got the
		 *	page queues lock, we can only try for this one.
		 */

		if (!vm_object_lock_try(object)) {
			/*
			 *	Move page to end and continue.
			 */

			queue_remove(&vm_page_queue_inactive, m,
				     vm_page_t, pageq);
			queue_enter(&vm_page_queue_inactive, m,
				    vm_page_t, pageq);
			vm_page_unlock_queues();
			vm_pageout_inactive_nolock++;
			continue;
		}

		/*
		 *	Remove the page from the inactive list.
		 */

		queue_remove(&vm_page_queue_inactive, m, vm_page_t, pageq);
		vm_page_inactive_count--;
		m->inactive = FALSE;

		if (m->busy || !object->alive) {
			/*
			 *	Somebody is already playing with this page.
			 *	Leave it off the pageout queues.
			 */

			vm_page_unlock_queues();
			vm_object_unlock(object);
			vm_pageout_inactive_busy++;
			continue;
		}

		/*
		 *	If it's absent or in error, we can reclaim the page.
		 */

		if (m->absent || m->error) {
			vm_pageout_inactive_absent++;
		    reclaim_page:
			vm_page_free(m);
			vm_page_unlock_queues();
			vm_object_unlock(object);
			continue;
		}

		/*
		 *	If it's being used, reactivate.
		 *	(Fictitious pages are either busy or absent.)
		 */

		assert(!m->fictitious);
		if (m->reference || pmap_is_referenced(m->phys_addr)
#if    VCF
		  || ((vcf_dirbase != (int *)0) && mcmsg_call(VCF_SWAP, m->phys_addr))
#endif VCF
		) {
			vm_object_unlock(object);
			vm_page_activate(m);
			vm_stat.reactivations++;
			vm_page_unlock_queues();
			vm_pageout_inactive_used++;
			continue;
		}

		/*
		 *	Eliminate all mappings.
		 */

		m->busy = TRUE;
		pmap_page_protect(m->phys_addr, VM_PROT_NONE);
		if (!m->dirty)
			m->dirty = pmap_is_modified(m->phys_addr);

		/*
		 *	If it's clean and not precious, we can free the page.
		 */

		if (!m->dirty && !m->precious) {
			vm_pageout_inactive_clean++;
			goto reclaim_page;
		}

		/*
		 *	If we are very low on memory, then we can't
		 *	rely on an external pager to clean a dirty page,
		 *	because external pagers are not vm-privileged.
		 *
		 *	The laundry bit tells vm_pageout_setup to
		 *	put the page back at the front of the inactive
		 *	queue instead of activating the page.  Hence,
		 *	we will pick the page up again immediately and
		 *	resend it to the default pager.
		 */

		assert(!m->laundry);
		if ((free_count < vm_pageout_reserved_internal) &&
		    !object->internal) {
			m->laundry = TRUE;
			vm_pageout_inactive_double++;
		}
		vm_page_unlock_queues();

		/*
		 *	If there is no memory object for the page, create
		 *	one and hand it to the default pager.
		 *	[First try to collapse, so we don't create
		 *	one unnecessarily.]
		 */

		if (!object->pager_initialized)
			vm_object_collapse(object);
		if (!object->pager_initialized)
			vm_object_pager_create(object);
		if (!object->pager_initialized)
			panic("vm_pageout_scan");

#if	NORMA_VM
		/*
		 *	Get a pageout reservation now.  If there are
		 *	none available, fix up the page's state and restore
		 *	it to the inactive queue.  Go look for a new victim.
		 *
		 *	XXX When things are bad, we may miss on many
		 *	reservations.  In this case, we may be destroying
		 *	the LRU properties of the inactive queue.  XXX
		 */
		if (object->internal) {
			obj_type = PAGEOUT_RESERVE_INTERNAL;
			need_reservation = TRUE;
#if	NORMA_IPC
		} else if (IP_NORMA_IS_PROXY(object->pager)) {
#endif	/* NORMA_IPC */

#if	NORMA2
		} else if (DIPC_IS_PROXY(object->pager)) {
#endif	/* NORMA2 */
			obj_type = PAGEOUT_RESERVE_EXTERNAL;
			need_reservation = TRUE;
		} else {
			obj_type = PAGEOUT_RESERVE_LOCAL;
			need_reservation = FALSE;
		 	++vm_pageout_reserve_local_extobj;
		}	

		if (need_reservation) {
			if (vm_pageout_reserve(object,
				       m->offset + object->paging_offset,
				       page_size,
				       obj_type,
				       FALSE) == FALSE) {
				vm_page_lock_queues();
				m->laundry = FALSE;
				m->inactive = TRUE;
				m->busy = FALSE;
				vm_page_inactive_count++;
				queue_enter(&vm_page_queue_inactive, m,
				    	    vm_page_t, pageq);
				vm_page_unlock_queues();
				vm_object_unlock(object);
				++vm_pageout_reservation_miss;
				--reserve_try;
				continue;
			}
		}
#endif	/* NORMA_VM */

		vm_pageout_inactive_dirty++;
		vm_pageout_page(m, FALSE, TRUE);	/* flush it */
		vm_object_unlock(object);
		burst_count++;
	}
}


#if	RESERVATION_COUNTERS
unsigned int	c_vm_pageout_scan_continue = 0;
unsigned int	c_vm_pageout_scan_continue_external = 0;
unsigned int	c_vm_pageout_scan_continue_internal = 0;
#endif
void vm_pageout_scan_continue()
{
	/*
	 *	We just paused to let the pagers catch up.
	 *	If vm_page_laundry_count is still high,
	 *	then we aren't waiting long enough.
	 *	If we have paused some vm_pageout_pause_max times without
	 *	adjusting vm_pageout_burst_wait, it might be too big,
	 *	so we decrease it.
	 */

	vm_page_lock_queues();
#if	NORMA_VM
#if	RESERVATION_COUNTERS
	RC(++c_vm_pageout_scan_continue);
	if (RESERVATION_AVAILABLE(PAGEOUT_RESERVE_EXTERNAL))
		RC(++c_vm_pageout_scan_continue_external);
	if (RESERVATION_AVAILABLE(PAGEOUT_RESERVE_INTERNAL))
		RC(++c_vm_pageout_scan_continue_internal);
#endif	/* RESERVATION_COUNTERS */
	/*
	 *	Take RESERVATION_LOCK here?  Doesn't seem worth it.  XXX
	 */
	if (RESERVATIONS(PAGEOUT_RESERVE_INTERNAL) > vm_pageout_burst_min)
#else	/* NORMA_VM */
	if (vm_page_laundry_count > vm_pageout_burst_min)
#endif	/* NORMA_VM */
	{
		vm_pageout_burst_wait++;
		vm_pageout_pause_count = 0;
	} else if (++vm_pageout_pause_count > vm_pageout_pause_max) {
		vm_pageout_burst_wait = (vm_pageout_burst_wait * 3) / 4;
		if (vm_pageout_burst_wait < 1)
			vm_pageout_burst_wait = 1;
		vm_pageout_pause_count = 0;
	}
	vm_page_unlock_queues();

#ifndef	KEEP_STACKS
	vm_pageout_continue();
	/*NOTREACHED*/
#endif	KEEP_STACKS
}

/*
 *	vm_pageout is the high level pageout daemon.
 */

void vm_pageout_continue()
{
	/*
	 *	The pageout daemon is never done, so loop forever.
	 *	We should call vm_pageout_scan at least once each
	 *	time we are woken, even if vm_page_free_wanted is
	 *	zero, to check vm_page_free_target and
	 *	vm_page_inactive_target.
	 */

	for (;;) {
		vm_pageout_scan();
		/* we hold vm_page_queue_free_lock now */
		assert(vm_page_free_wanted == 0);
		assert_wait((int) &vm_page_free_wanted, FALSE);
		simple_unlock(&vm_page_queue_free_lock);
		counter(c_vm_pageout_block++);
		thread_block(vm_pageout_continue);
	}
}

void vm_pageout()
{
	int		free_after_reserve;
#if	NORMA_VM
	pending_record	*p;
#endif	/* NORMA_VM */

	/* 
	 * Set thread priority to FIXEDPRI so it won't get too low
	 * to be useful. PTS 7375
	 */

	thread_set_own_priority(9);
	(void) thread_policy(current_thread(), POLICY_FIXEDPRI, 1);

	current_thread()->vm_privilege = TRUE;
	stack_privilege(current_thread());

#if	NORMA2
	/*
	 *	The pageout thread needs its own transmission map
	 *	and RDMA group to avoid deadlocks caused by the VM locking
	 *	hierarchy (write lock a map, allocate an vm_map_entry_t,
	 *	pool is full -- block w/ the norma_map locked).
	 */
	dipc_pageout_priv(current_thread());
#endif	/* NORMA2 */

	/*
	 *	Initialize some paging parameters.
	 */

	if (vm_pageout_burst_max == 0)
		vm_pageout_burst_max = VM_PAGEOUT_BURST_MAX;

#if	NORMA_VM
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_INTERNAL] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_INTERNAL] =
			VM_PAGEOUT_INTERNAL_RESERVATION_MAX;
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_EXTERNAL] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_EXTERNAL] =
			VM_PAGEOUT_EXTERNAL_RESERVATION_MAX;
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_LOCK_REQUEST] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_LOCK_REQUEST] =
			VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX;
	if (vm_pageout_reservation_max[PAGEOUT_RESERVE_LOCAL] == 0)
		vm_pageout_reservation_max[PAGEOUT_RESERVE_LOCAL] =
			VM_PAGEOUT_LOCAL_RESERVATION_MAX;

	for (p = vm_pageout_pending;
	     p < vm_pageout_pending + vm_pageout_pending_max;
	     ++p)
       		p->type = PAGEOUT_RESERVE_FREE;
#endif	/* NORMA_VM */

	if (vm_pageout_burst_min == 0)
		vm_pageout_burst_min = VM_PAGEOUT_BURST_MIN;

	if (vm_pageout_burst_wait == 0)
		vm_pageout_burst_wait = VM_PAGEOUT_BURST_WAIT;

	if (vm_pageout_empty_wait == 0)
		vm_pageout_empty_wait = VM_PAGEOUT_EMPTY_WAIT;

	if (vm_page_free_reserved == 0)
		vm_page_free_reserved = VM_PAGE_FREE_RESERVED;

	if (vm_pageout_pause_max == 0)
		vm_pageout_pause_max = VM_PAGEOUT_PAUSE_MAX;

	if (vm_pageout_reserved_internal == 0)
		vm_pageout_reserved_internal =
			VM_PAGEOUT_RESERVED_INTERNAL(vm_page_free_reserved);

	if (vm_pageout_reserved_really == 0)
		vm_pageout_reserved_really =
			VM_PAGEOUT_RESERVED_REALLY(vm_page_free_reserved);

	free_after_reserve = vm_page_free_count - vm_page_free_reserved;

	if (vm_page_free_min == 0)
		vm_page_free_min = vm_page_free_reserved +
			VM_PAGE_FREE_MIN(free_after_reserve);

	if (vm_page_free_target == 0)
		vm_page_free_target = vm_page_free_reserved +
			VM_PAGE_FREE_TARGET(free_after_reserve);

	if (vm_page_free_target < vm_page_free_min + 5)
		vm_page_free_target = vm_page_free_min + 5;

	/*
	 *	vm_pageout_scan will set vm_page_inactive_target.
	 */

	vm_pageout_continue();
	/*NOTREACHED*/
}


#if	NORMA_VM && NORMA_IPC
/* DEAD-CODE:
 * This routine is used by the old norma code to decide if it can tell a node
 * to continue accepting messages, not just vm priority ones.  It NEEDS
 * to be tuned.  Little thought as to what is really the right thing to do
 * has been done.
 */

boolean_t
vm_netipc_able_continue_recv()
{
	return (vm_page_free_count > 
		(vm_page_free_target -
		((vm_page_free_target - vm_page_free_reserved)/2)));
}

#endif	/* NORMA_VM && NORMA_IPC */


#if	MACH_KDB
#if	NORMA_VM
void
db_rc_dump(prompt, carray)
char		*prompt;
unsigned int	carray[];
{
	int	x;

	db_printf(prompt);
	for (x = 0; x < PAGEOUT_RESERVE_MAX; ++x)
		db_printf("\t%8d", carray[x]);
}


#define	DB_PENDING_PAGEOUT_MAX	10
int db_pending_pageout_max = DB_PENDING_PAGEOUT_MAX;

char *
pageout_type_str(type)
int	type;
{
	char	*name;

	switch (type) {
	    case PAGEOUT_RESERVE_INTERNAL:
		name = "Internal";
		break;
	    case PAGEOUT_RESERVE_EXTERNAL:
		name = "External";
		break;
	    case PAGEOUT_RESERVE_LOCK_REQUEST:
		name = "Lock Request";
		break;
	    case PAGEOUT_RESERVE_LOCAL:
		name = "Local";
		break;
	    default:
		name = "UNKNOWN";
		break;
	}
	return name;
}
#endif	NORMA_VM


db_vm()
{
#if	NORMA_VM
	pending_record	*p;
	int		type, count;
#endif
	extern int vm_page_gobble_count;

	db_printf("VM Statistics:\n");
	db_printf("    pages:\n");
	db_printf("\tactiv %5d  inact %5d  free  %5d",
		  vm_page_active_count, vm_page_inactive_count,
		  vm_page_free_count);
	db_printf("\twire  %5d  gobbl %5d\n",
		  vm_page_wire_count, vm_page_gobble_count);
	db_printf("\tlaund %5d\n", vm_page_laundry_count);

	db_printf("   target:\n");
	db_printf("\tmin   %5d  inact %5d  free   %5d",
		vm_page_free_min, vm_page_inactive_target,
		vm_page_free_target);
	db_printf("\tresrv %5d\n", vm_page_free_reserved);

	db_printf("    burst:\n");
	db_printf("\tmax   %5d  min   %5d  wait  %5d   empty %5d\n",
		  vm_pageout_burst_max, vm_pageout_burst_min,
		  vm_pageout_burst_wait, vm_pageout_empty_wait);

	db_printf("    pause:\n");
	db_printf("\tcount %5d  max   %5d\n",
		  vm_pageout_pause_count, vm_pageout_pause_max);
#if	NORMA_VM
	db_printf("reservations:\n");
	db_printf("\t\tInternal\tExternal\t    Lock\t   Local\n");
	db_rc_dump("    current", vm_pageout_reservations);
	db_rc_dump("\n    maximum", vm_pageout_reservation_max);
	db_rc_dump("\n    needed", vm_pageout_reservation_needed);
	db_printf("\n");
#if	RESERVATION_COUNTERS
	db_rc_dump("    granted", c_vm_pageout_reserve_granted);
	db_rc_dump("\n    completed", c_vm_pageout_completed);
	db_rc_dump("\n    denied", c_vm_pageout_reserve_denied);
	db_rc_dump("\n    terminated", c_vm_pageout_terminated);
#endif	/* RESERVATION_COUNTERS */
	db_printf("\n");
	for (type = count = 0; type < PAGEOUT_RESERVE_MAX; ++type)
		count += vm_pageout_reservations[type];
	if (count > 0) {
		if (count > db_pending_pageout_max)
			count = db_pending_pageout_max;
		db_printf("Pending Pageouts:\n");
		db_printf("          Port Kmsgs     Object     Offset Type\n");
		for (p = vm_pageout_pending;
		     p < vm_pageout_pending + vm_pageout_pending_max &&
		     count > 0;
		     ++p) {
			if (p->type != PAGEOUT_RESERVE_FREE) {
				db_printf("    0x%08x %5d 0x%08x 0x%08x %s\n",
					p->pager,
					db_port_kmsg_count(p->pager),
					p->object,
					p->offset,
					pageout_type_str(p->type));
				--count;
			}
		}
	}
#if	RESERVATION_COUNTERS
	db_printf("scan_continue\tint %8d\text %8d\tcall %8d\n",
		  c_vm_pageout_scan_continue_internal,
		  c_vm_pageout_scan_continue_external,
		  c_vm_pageout_scan_continue);
#endif	/* RESERVATION_COUNTERS */
#endif	/* NORMA_VM */
}
#endif	/* MACH_KDB */
