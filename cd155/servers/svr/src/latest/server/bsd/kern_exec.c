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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This file was extended and modified by the Center for High Performance
 * Computing of Worcester Polytechnic Institute on behalf of OSF.
 */
/* 
 * HISTORY
 * $Log: kern_exec.c,v $
 * Revision 1.17  1995/02/01  21:26:27  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.16  1994/12/20  22:52:06  suri
 *  Reviewer: jlitvin
 *  Risk: Low
 *  Benefit or PTS #: 11640
 *  Testing: Specific Testcase
 *  Module(s): Backing out the fix for PTS-11270/11317/10593 (vm_allocate/sbrk
 *  changes), as it was causing excessive wiring down of memory on the compute
 *  nodes under some specific cases (the testcase for PTS-11640 is one of them).
 *  The vm_allocate/sbrk changes have to be reimplemented, perhaps using
 *  vm_reserve().
 *
 * Revision 1.15  1994/11/18  20:26:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/10/25  23:42:44  suri
 *  Reviewer: jlitvin
 *  Risk: M
 *  Benefit or PTS #: 11317
 *  Testing: Specific testcase, fileio, pthreads, xtrnl, NQS/MACs EATs
 *  Module(s): obreak() in server/bsd/kern_mman.c
 *            setrlimit() in server/bsd/kern_resource.c
 *            coff_getxfile() in server/bsd/kern_exec.c
 *            user struct in server/sys/user.h
 *            rf_data struct in server/tnc/rtask.h
 *            rfork_pproc_load_msg() in server/tnc/rtask_cli_pproc.c
 *            rfork_pproc_unload_msg() in server/tnc/rtask_svr_pproc.c
 *
 * Revision 1.13  1994/10/11  18:36:29  suri
 *  Reviewer: cfj
 *  Risk: H
 *  Benefit or PTS #: 10593
 *  Testing: Specific testcase;  fileio, message_passing, xtrnl,
 *  	  PFS and pthreads EATs
 *  Module(s): e_obreak() in emulator/emul_mapped.c
 *             coff_getxfile() in server/bsd/kern_exec.c
 *             obreak() in server/bsd/kern_mman.c
 *             setrlimit() in server/bsd/kern_resource.c
 *
 * Revision 1.12  1994/07/27  16:22:25  johannes
 * In execve_prim() the new utask fields are filled and the old dir ports
 * inherited by fork() are released.
 *
 *  Reviewer: Nandini
 *  Risk: H
 *  Benefit or PTS #: information for absolute exec path in core files
 *  Testing: developer
 *  Module(s): server/sys: user.h
 *             server/bsd: kern_exec.c, kern_exit.c, kern_fork.c
 *             server/tnc: pvps.ops, tnc.defs, rtask_server.c
 *                         rtask_cli_pproc.c, rtask_cli_vproc.c
 *                         rtask_svr_pproc.c, rtask_svr_vproc.c
 *                         chkpnt_vproc.c
 *             server/paracore: core.c
 *
 * Revision 1.11  1994/01/13  17:54:07  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.10  1993/10/06  22:44:36  jlitvin
 * Make sure that ux_server_thread_unblocking() is called for BSD a.out
 * exec's (even though I don't think this code is ever called).  Why do
 * we have this dead code hogging up space?!?!
 *
 * Revision 1.9  1993/09/29  21:34:32  cfj
 * Removed the check which was treating 386 and 860 executables as the
 * same.  It was just cruft from when we did not have a 860 specific version
 * of ld.
 *
 * Revision 1.8  1993/07/14  17:47:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:46:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/07  19:09:49  nandy
 * Fixed a merge conflict
 *
 * Revision 1.6  1993/05/06  19:02:40  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.5  1993/04/03  03:03:47  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4  1993/03/31  21:33:50  shala
 * Fix in order to load non pagable file. Fixed by robboy.
 *
 * Revision 1.1.2.1.2.1  1993/01/09  00:03:16  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.3  1992/12/15  17:40:16  cfj
 * Put a trunc_page() around the result in PAGI_DATA_OFFSET for the i860.
 *
 * Revision 1.2  1992/11/30  22:15:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:05:40  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:06:15  cfj
 * Bump major revision number.
 *
 * Revision 2.29  1993/04/29  13:57:36  klh
 * 	Revision 2.28  93/02/26  11:12:39  rabii
 * 		Redefine PAGI_DATA_OFFSET for i860 to work for PARAGON
 *
 * 	Revision 2.27  93/01/07  11:16:48  condict
 * 		Set emulator_text_size, and initialise profiling if configured.
 * 		[92/09/24  16:49:36  emcmanus]
 *
 * Revision 2.28  93/02/26  19:52:58  yazz
 * Fix RCS comments and remove extraneous comment block.
 * 
 * Revision 2.27  93/02/22  13:38:53  yazz
 * Code update from Intel for the Paragon.
 * 	Revision 1.3  1992/12/15  17:40:16  cfj
 * 	Put a trunc_page() around the result in PAGI_DATA_OFFSET for the i860.
 *
 * 	Revision 1.2  1992/11/30  22:15:23  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.1  1992/11/06  00:05:40  dleslie
 * 	Local changes for NX through noon, November 5, 1992.
 *
 * 	Revision 4.1  1992/11/04  00:06:15  cfj
 * 	Bump major revision number.
 *
 * Revision 2.26  1992/09/24  16:49:36  rabii
 * 	or traced processes, execve_prim() is too early to SIGTRAP;
 * 	his replaces 2.25 with the more complete Grenoble fix.  (dwm; #376)
 *
 * Revision 2.25  92/09/17  13:41:51  rabii
 * 	[1992/09/11  17:28:05  cfj]
 * 	Moved the check for the trace flag (STRC) and the call to psignal() from
 * 	execve_prim() to exec_args_set() in ldr_exec.c.  This way the
 * 	arguments are available to a debugger at the time of the exec.
 * 
 * Revision 2.24  92/08/13  19:17:14  rabii
 * 	Added VM_PROT_READ to the maximum page_0 protection so that it can be
 * 	upgraded in case of 0 page reference (see mach_signal.c) (rabii)
 * 
 * Revision 2.23  92/06/10  16:43:03  pjg
 * 	Make sure remote_exec_check_vnode is called and read_access is
 * 	initialized in all code paths in getxfile and coff_getxfile.
 * 
 * Revision 2.22  92/05/26  12:03:49  pjg
 * 	Don't call ux_server_thread_[un]blocking around remote_inode_pager_setup.
 * 	Changed the comments of these calls.
 * 
 * Revision 2.21  92/05/24  14:13:22  pjg
 * 	Revision 3.16  92/03/24  21:02:58  barbou
 * 	Fix for bug #119: null pointer in execve_prim().
 * 	Allow page 0 protection to be readable on demand, for backward
 * 	compatibility.
 * 
 * 	Revision 3.15  92/03/23  18:02:01  condict
 * 	Add ux_server_thread_blocking/unblocking calls around each vm operation,
 * 	since they send messages to the vnode_pager, which can be blocked at any
 * 	time.
 * 
 * 	Revision 3.14  92/03/13  15:18:12  condict
 * 	Read the maximum of exec header size and MAXINTERP, in order to ensure
 * 	that we have enough bytes for either shell script or binary program.
 * 
 * 	Revision 3.13  92/03/05  12:52:08  sp
 * 	Tidy up courtesy of pjg
 * 
 * Revision 2.20  92/05/18  12:28:48  roy
 * 	Revision 2.16.1.1  92/04/22  09:54:04  roy
 * 	Removed use of vm_map_no_mo.
 * 	[92/03/23            roy]
 * 
 * Revision 2.19  92/05/01  15:55:06  rabii
 * 	Removed setting of HASPATHBUF. (pjg)
 * 
 * Revision 2.18  92/05/01  10:16:38  rabii
 * 	Set option HASPATHBUF in ni_nameiop (pjg).
 * 
 * 	Picked up fix for page 0 protection from jose.
 * 
 * Revision 2.17  92/04/05  16:46:10  pjg
 * 	Pass ndp as argument to remote_exec_lookup.
 * 
 * Revision 2.16  92/03/09  14:03:09  durriya
 * 	Revision 3.12  91/12/19  15:58:44  bernadat
 * 	Allow BSD a.out programs built with -T options to load. This
 * 	is a hack and should be removed once the binaries will be in rose
 * 	format. (This was necessary for the dos server)
 * 
 * Revision 2.15  92/03/03  13:56:03  pjg
 * 	Changed calls to remote_exec_read to include the 'flag' parameter.
 * 
 * Revision 2.14  92/03/01  18:40:50  pjg
 * 	Use vnode proxies to interface the FS. Change the names of the FS
 * 	support routines to remote_*.
 * 
 * Revision 2.13  92/02/11  22:11:37  pjg
 * 	Change the vm_map code for funky protection mapping of page 0 to
 * 	use VM_INHERIT_COPY rather than VM_INHERIT_SHARE until
 * 	NORMA3 microkernels become prevelant (TNC only) (roman@locus).
 * 
 * Revision 2.12  92/01/17  19:47:00  roy
 * 	Remove interrupt arg from exec_lookup (pjg).
 * 
 * Revision 2.11  92/01/14  17:01:54  roy
 * 	Changed include of norma_ipc.h to remote_proc.h.
 * 
 * Revision 2.10  92/01/14  11:13:44  roy
 * 	Significant changes to exec to support exec'ing remote files (pjg).
 * 
 * Revision 2.9  92/01/02  18:51:08  roy
 * 	91/12/16  19:42:24  pjg
 * 	Limited test of DFS support for remote exec(). All the changes are
 * 	ifdef'ed OSF1_ADFS.
 * 
 * 	91/09/22  15:57:20  noemi
 * 	OSF1/ADFS V2.0 update
 * 
 * Revision 2.8  91/12/17  13:18:57  roy
 * 	91/10/30  17:41:43  bernadat
 * 	In case of serialized file system with SER_COMPAT option
 * 	release master lock before invoking vnode pager.
 * 
 * 	91/10/16  12:15:01  sp
 * 	put macho_uses_loader back to 0
 * 
 * 	91/10/15  12:14:20  sp
 * 	move the vm_exec function to uxkern/vm_unix.c rewriting it on the way
 * 
 * 	91/10/09  18:34:17  emcmanus
 * 	Include "profiling.h" so #if PROFILING conditionals are reasonable.
 * 
 * Revision 2.7  91/11/13  12:50:16  rabii
 * 	Added i860 COFF support
 * 
 *
 * [Mon Oct 21 10:26:11 PDT 1991] cfj@ssd.intel.com
 *      Added i860 coff support.
 *
 * Revision 2.5  91/10/14  11:51:36  sjs
 *   	91/10/04  16:35:57  sp
 * 	Add support for auxiliary vector
 * 	Fix exec_load_loader
 * 
 * 	91/09/27  11:55:10  emcmanus
 * 	Conditionalize reference to profiling element of u.
 * 
 * 	91/09/10  11:56:30  barbou
 * 	Hack to bypass a vm_map() bug when called with MEMORY_OBJECT_NULL
 * 	(fixed in MK59).
 * 
 * Revision 2.4  91/10/04  14:43:58  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:32:04  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	File descriptors now handled in emulator. Remove handling of
 * 	close-on-exec files from here.
 * 
 * Revision 2.2  91/08/31  13:21:29  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/05  11:58:13  jose
 * Added code for ROSE support
 * 
 * Revision 3.3  91/07/26  15:48:03  jose
 *  Adapted from 1.0.2 to OSF/1s environment
 * Turned on COFF support for i386
 * 
 * Revision 1.17.9.2  91/06/19  08:42:55  lwa
 * 	In secure systems, get privileges from the ORIGINAL file passed
 * 	in to exec() in exec_get_effective_ids(), and establish the
 * 	privileges in exec_setuid().  Move the exec_setuid() calls OUT
 * 	of the getxfile() routines into common code.
 * 	[91/06/17  08:16:38  lwa]
 * 
 * Revision 1.17.6.2  91/03/19  08:57:18  bilbo
 * 	Call clear_p_realtimer from execv to clear p_realtimer on successful exec,
 * 	if the timer is a nano_timer rather than normal setitimer. (bug#0732).
 * 	[91/03/19  08:43:57  bilbo]
 * 
 * Revision 1.17  90/10/31  13:48:45  devrcs
 * 	Replace an = by == so that u_text and u_tsize get set up properly
 * 	[90/10/25  17:05:20  sp]
 * 
 * 	This fixes bug #1520.  The prior fix to bug #0098 was somewhat
 * 	of an overkill in that it mapped all error returns from
 * 	exec_load_loader() to ENOEXEC, under the assumption that there was
 * 	some failure in trying to load the user-space loader.  However
 * 	E2BIG should be passed through as is.
 * 	[90/10/23  13:07:51  kwallace]
 * 
 * Revision 1.16  90/10/07  13:16:59  devrcs
 * 	Corrected C programming bug of using && instead of & in
 * 	o_mach_o_getxfile() where region protections are computed.
 * 	[90/10/04  12:02:44  kwallace]
 * 
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:50:43  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:55:01  gm]
 * 
 * 	Use consistent credentials macros.
 * 	[90/08/18  23:55:16  nags]
 * 
 * 	Remove dead 386 code
 * 	[90/08/17  16:55:02  kevins]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:35:18  mcg]
 * 
 * 	Remove unexecuted audit code, the result of a bad merge.
 * 	[90/08/13  12:15:43  seiden]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:35:18  mcg]
 * 
 * Revision 1.15  90/09/13  11:41:52  devrcs
 * 	Fix broken calling sequence for exec_load_loader.  Also,
 * 	restore missing return lost in next change.
 * 	[90/08/30  09:18:56  lwa]
 * 
 * Revision 1.14  90/08/24  11:15:50  devrcs
 * 	removed u.u_error references
 * 	[90/08/20  12:33:51  gmf]
 * 
 * 	Changes for new system call interface.
 * 	[90/08/19  16:40:11  gmf]
 * 
 * 	Changes for new system call interface (gmf).
 * 	Changes for u_file_state (sue).
 * 
 * Revision 1.13  90/07/27  08:43:39  devrcs
 * 	Fix credentials leak.
 * 	[90/07/20  13:48:59  nags]
 * 
 * 	Changed Mach-O support for revised object file format -- the region
 * 	load command changed and the header is now in canonical format.
 * 	[90/07/17  16:25:49  melanie]
 * 
 * Revision 1.12  90/07/17  11:18:21  devrcs
 * 	Changes for least-privilege checking
 * 	[90/07/10  21:51:22  seiden]
 * 
 * 	Replaced execve_uses_loader switch with coff_uses_loader and
 * 	macho_uses_loader, and slightly changed the flow of control using them.
 * 	Deleted a few Mach-O sanity checks (that didn't affect the kernel).
 * 	Rewrote algorithm to calculate u-area stuff for MachO object files.
 * 	[90/07/09  11:38:31  melanie]
 * 
 * 	Changed default Multimax MOD start for OSF/Mach-O from 0x20 to 0x2000.
 * 	[90/07/04  12:29:48  kwallace]
 * 
 * Revision 1.11  90/06/22  20:05:40  devrcs
 * 	Fixed use of undeclared variable bsd_a_out.
 * 	[90/06/18  10:47:00  melanie]
 * 
 * 	nags merge
 * 	[90/06/12  19:05:33  nags]
 * 
 * 	Added changes from SecureWare.
 * 	[90/06/10  04:34:56  seiden]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:39:20  seiden]
 * 
 * 	Changed load_init_program() effectively moved init from /etc to
 * 	/sbin.  Actually, made changes such that load_init_program()
 * 	attempts to load one of the following programs, in the following
 * 	order.
 * 
 * 		/sbin/mach_init
 * 		/sbin/init
 * 		/etc/mach_init
 * 		/etc/init
 * 
 * 	The first one successfully loaded is the init program.
 * 	[90/06/09  17:17:43  kwallace]
 * 
 * 	Added support for the OSF/Mach-O object file format.
 * 	[90/06/08  12:14:12  melanie]
 * 
 * 	Change to use new vm_exec() function.
 * 	[90/06/04  15:58:17  lwa]
 * 
 * Revision 1.10  90/05/24  22:59:54  devrcs
 * 	Fixed bug with shell script processing where shell_name and
 * 	shell_arg were being overwritten by the object file header of
 * 	the shell.  As it turns out, the auxiliary vector entry of
 * 	type AT_EXEC_FILENAME, when executing a #! shell script, was
 * 	never correctly set-up.  It should have contained the pathname
 * 	of the specified shell, but that string was being overwritten by
 * 	the object file header of the shell.
 * 	[90/05/16  12:11:00  kwallace]
 * 
 * 	Added to fix to bug #0098, such that if exec_load_loader() returns
 * 	an error, such as ENOENT because the loader couldn't be loaded, we
 * 	map it to ENOEXEC.
 * 	[90/05/11  08:21:59  kwallace]
 * 
 * 	Removed use of obscure variable names such as indir, dirp, cfname
 * 	and cfarg, and replaced them with more mnemonic names such as
 * 	shell_name, shell_name_tail and shell_arg.
 * 	[90/05/10  18:33:49  kwallace]
 * 
 * 	Fixed bug #0098, where calling execve() on shell scripts of length
 * 	greater than MAXINTERP bytes, that did not begin with a #!, failed
 * 	with ENOENT, instead of ENOEXEC.
 * 	[90/05/02  17:21:16  kwallace]
 * 
 * 	Condensed history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	change shell scrip error of ENOENT to ENOEXEC	kwallace@osf.org
 * 	Massive changes for support of exec_with_loader	kwallace@osf.org
 * 	Check vp->v_wrcnt rather than walk file table.	noemi@osf.org
 * 	Set SEXEC flag in xgetfile routines.		coren@osf.org
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Saved {g,u}ids, VOP_LOCK to fix pager race.	gm@osf.org
 * 	Use new kmem_alloc that handles wait/wakeup.	mwyoung@cmu.edu
 * 	i386: Locking around suiword was incorrect.	rvb@cmu.edu
 * 	Fixes to coff_getxfile().			af@cmu.edu
 * 	Fix execve to allow SYSV_COFF and BSD_A_OUT	rvb@cmu.edu
 * 	Fixed OMAGIC images for mips.			af@cmu.edu
 * 	Fixes for i386 coff.				rvb@cmu.edu
 * 	Changed execve() for sparc. 			jjc@cmu.edu
 * 	Common program loader for vax/sun/ibmrt		gm0w@cmu.edu
 * 	LOADER_PAGE_SIZE defined as SECTALIGN for COFF	rvb@cmu.edu
 * 	Remerge and cleanup multimax and i386 code.	dlb@cmu.edu
 * 	Add i386 into mmax coff processing		rvb,dlb@cmu.edu
 * 	Correct zeroing of front of bss 		dlb@cmu.edu
 * 	mmax_getxfile:  check aouthdr for page size	dlb@cmu.edu
 * 	mmax_getxfile: fix bss base calculation.	dlb@cmu.edu
 * 	Use copyout to zero partial page at end of data mwyoung@cmu.edu
 * 	Unlock inode after calling inode_pager_setup()	mwyoung@cmu.edu
 * 	Watch out for zero return from kmem_alloc_wait.	mwyoung@cmu.edu
 * 	Changed ibmrt program_loader to use USRTEXT	mrt@cmu.edu
 * 	Removed the reserving of segment 0 on the RT	rpd@cmu.edu
 * 	Fix up arguments to vm_protect.			mwyoung@cmu.edu
 * 	Use vm_map.  Release inode_pager upon errors.	mwyoung@cmu.edu
 * 	inode_pager_release() relinquishes port rights	mwyoung@cmu.edu
 * 	Use new memory object types.			mwyoung@cmu.edu
 * 	Record stack start, end, direction of growth.	mwyoung@cmu.edu
 * 	Handle zero-size data regions.			mwyoung@cmu.edu
 * 	Changed for updated sysV header files. 		dlb@cmu.edu
 * 	sun3: Force page 0 to be VM_PROTECT_NONE	rvb@cmu.edu
 * 	Changed inode_pager_setup() calling sequence.	mwyoung@cmu.edu
 * 	Changed multimax to copyout an array of zeroes	dlb@cmu.edu
 * 	Check kernel port security for setuid/gid	mwyoung@cmu.edu
 * 	load_init_program runs on master.		dlb@cmu.edu
 * 	roundup text size separate from data+bss (vax)	dbg@cmu.edu
 * 	Added "load_init_program()" 			mwyoung@cmu.edu
 * 	Merged in changes for Sun			jjc@cmu.edu
 * 	Merged in Multimax changes; uses coff format.	dlb@cmu.edu
 * 	ibmrt: Added exect() call for adb on the RT.	bolosky@cmu.edu
 * 	Remember text, data+bss end on old boundaries	dbg@cmu.edu
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
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
 *	@(#)kern_exec.c	7.19 (Berkeley) 1/4/90
 */
#include <remote_proc.h>
#include <sys/secdefines.h>
#if	SEC_BASE
#include <sys/security.h>
#endif
#if	SEC_ARCH
#include <sys/secpolicy.h>
#endif
#include <sys/exec_incl.h>

#include "profiling.h"

#ifdef __hp_osf
int aout_uses_loader = 1;
int coff_uses_loader = 1;
#else
int aout_uses_loader = 0;
int coff_uses_loader = 0;
#endif
int macho_uses_loader = 0;

#ifdef OSF1_SERVER
int emulator_text_size;
#endif

#if	OSF_MACH_O
extern int decode_mach_o_hdr(void *, size_t, unsigned long, mo_header_t *);
#endif


#ifdef	OSF1_SERVER
#include <bsd/exec.h>
#else
/*
 * exec system call, with and without environments.
 */
struct execa {
	char	*fname;
	char	**argp;
	char	**envp;
};

struct execr {				/* exec return arguments */
	char		*cfname;	/* shell file name */
	char		*cfarg;		/* shell args */
	int		*entry;		/* pointer to pc entry points */
	unsigned int	*entry_count;	/* number of entries */
};
#endif

execv(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	((struct execa *)args)->envp = NULL;
	return (execve(p, args, retval));
}

#ifdef	ibmrt
/* New RXTUnix system call for execve with single step active */
exect(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	int error;

	error = execve(p, args, retval);
	if (!error)
		u.u_ar0[ICSCS] |= ICSCS_INSTSTEP;
	return (error);
}
#endif

execve(p, args, retval)
	struct proc *p;
	void *args;
	void *retval;
{
	return execve_prim(p, args, retval, FALSE);
}


execve_prim(p, args, retval, special)
	struct proc *p;
	void *args;
	int *retval;
	boolean_t special;	/* TRUE means it is the emulator program */
{
	register struct execa *uap;
	register struct execr *rtp;
	int na, ne, nc;
	struct exec_privs privs;
	char *shell_name, *shell_arg;
	char shell_name_tail[MAXCOMLEN + 1];
#ifdef	OSF1_ADFS
	struct vnode_proxy *vp;
#else
	struct vnode *vp;
	struct vattr vattr;
	vm_offset_t exec_args = 0;
#endif
	register struct nameidata *ndp = &u.u_nd;
	char line[MAXINTERP];
	int pflag;
	union {
		char	ex_shell[MAXINTERP];	/* #! and interpreter name */
#if	SYSV_COFF
		/* 
		 * Coff fileheader structures.
		 */
		struct {
			struct filehdr fhdr;
			struct aouthdr ohdr;
#define ahdr exdata.coff.ohdr
		} coff;
		struct {
			short	magic;
		} coff_hdr;
#endif
#if	OSF_MACH_O
		long	omo_magic;
#endif
#if	BSD_A_OUT
		struct	exec ex_exec;
#endif
	} exdata;
	int resid, error;
#if     BSD_A_OUT
	int 	do_bsd_a_out = 0;
#endif 
#if	SYSV_COFF
	int	docoff = 0;
	int	aouthdr_offset;
#endif
#if	OSF_MACH_O
	int 		do_o_mach_o = 0;
	long		entry_addr = 0;
	int		conversion_error;
        char		mo_header_buf[MO_SIZEOF_RAW_HDR];
	                              /* buffer for raw canonical version */
        mo_header_t	mo_header;    /* translated version of the header */
#endif

	privs.creds = NULL;
	privs.is_priv = FALSE;
	uap = (struct execa *) args;
	rtp = (struct execr *) retval;

#ifdef	OSF1_SERVER
	/*
	 * Return command file name and argument as null strings
	 * by default.
	 */
	rtp->cfname[0] = '\0';
	rtp->cfarg[0] = '\0';
#endif	/* OSF1_SERVER */

	ndp->ni_dirp = uap->fname;
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg =  UIO_SYSSPACE;	/* we are not using the generic interface */
#ifdef OSF1_ADFS
	error = remote_exec_lookup(ndp, &vp);
	if (error)
		goto bad;
	ASSERT(vp != 0);
#else	OSF1_ADFS
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
#endif	OSF1_ADFS

#ifdef OSF1_ADFS
	{
	uid_t new_uid;
	gid_t new_gid;
	boolean_t new_is_priv;

	error = remote_exec_setup_suid(vp, p->p_flag, TRUE,
				       &new_uid, &new_gid, &new_is_priv);
	if (error)
		goto bad;
	/*
	 * XXX Does not support security option SEC_BASE
	 */
	privs.creds = crdup(u.u_cred);
	privs.creds->cr_uid = new_uid;
	privs.creds->cr_gid = new_gid;
	privs.is_priv = new_is_priv;
	}	
#else
	VOP_GETATTR(vp, &vattr, u.u_cred, error);
	if (error)
		goto bad;
	if (error = exec_get_effective_ids(vp, &vattr, &privs))
		goto bad;
	if (error = exec_check_access(p, vp, &vattr))
		goto bad;

#if	SEC_BASE
	/*
	 * If not on NOSUID filesystem, check for EXECSUID privilege
	 * for SUID programs.  Audit change of IDs.
	 */
	{
		int mflag;
		MOUNT_LOCK(vp->v_mount);
		mflag = vp->v_mount->m_flag;
		MOUNT_UNLOCK(vp->v_mount);
		if (!(mflag & M_NOSUID) && !exec_allowed(&vattr)) {
			error = u.u_error;    /* XXX - change exec_allowed! */
                	goto bad;
		}
	}
#endif
#endif /* OSF1_ADFS */

#ifdef PARACORE
	/*
	 * Fill utask fields for getting absolute pathname of execed file.
	 */
	
	/*
	 * Set exec program name.
	 */
	strncpy((caddr_t)p->p_utask.uu_exec_prg_name,
		(caddr_t)uap->fname, 
	        PATH_MAX);
	p->p_utask.uu_exec_prg_name[PATH_MAX - 1] = '\0';

#ifdef OSF1_ADFS
	/*
	 * Release references to old exec directory proxies.
	 */
	remote_vrele(&p->p_utask.uu_exec_utnd.utnd_cdir);	
	remote_vrele(&p->p_utask.uu_exec_utnd.utnd_rdir);
#endif	OSF1_ADFS
	
	/*
	 * Copy actual directory proxies.
	 */
	p->p_utask.uu_exec_utnd = p->p_utask.uu_utnd;
	
#ifdef OSF1_ADFS
	/*
	 * Init references to new exec directory proxies.
	 */
	vnode_proxy_init(&p->p_utask.uu_exec_utnd.utnd_cdir);
	vnode_proxy_init(&p->p_utask.uu_exec_utnd.utnd_rdir);
#endif	OSF1_ADFS
#endif /* PARACORE */
	
	/*
	 *	Read in the header to get magic number.
	 *	This magic number is architecture-dependent.
	 */
	exdata.ex_shell[0] = '\0';	/* for zero length files */
#ifdef OSF1_ADFS
	error = remote_exec_read(vp, (caddr_t)&exdata, sizeof(exdata),
				 (off_t)0, IO_UNIT, &resid);
#else
	error = vn_rdwr(UIO_READ, vp, (caddr_t)&exdata, sizeof(exdata),
	    (off_t)0, UIO_SYSSPACE, IO_UNIT, u.u_cred, &resid);
#endif
	if (error)
		goto bad;
	if (resid && exdata.ex_shell[0] != '#') {
		error = ENOEXEC;
		goto bad;
	}
	if (error = exec_get_shell(exdata.ex_shell, &shell_name, &shell_arg))
		goto bad;

	if (shell_name) {

		/*
		 * Save the shell_name and shell_arg because we are about
		 * to overwrite exdata.ex_shell.  Also adjust shell_name
		 * and shell_arg pointers to point to the new copies of
		 * the strings.
		 */
		bcopy((caddr_t)exdata.ex_shell, (caddr_t)line, MAXINTERP);
		shell_name = line + (shell_name - exdata.ex_shell);
		if (shell_arg)
			shell_arg = line + (shell_arg - exdata.ex_shell);

		/*
		 * Switch over to loading the shell.  Finished with vnode
		 * for the script.  Get a vnode for the shell.  Make sure
		 * the shell has the proper access permissions.  Read in
		 * the header.  The file must be at least MAXINTERP byte
		 * long.  If not, just as above, we assume it must be shell
		 * script.  However, at this point, we won't allow shell
		 * that indirectly refer to other shell scripts.  Save the
		 * directory entry name as returned by namei() to use later
		 * as argv[0].  Lastly, we do not allow shell scripts to be
		 * setuid or setgid.
		 */
#ifdef	OSF1_ADFS
		remote_vfree(vp);
#else
		vrele(vp);
#endif
		ndp->ni_nameiop = LOOKUP | FOLLOW; 
		ndp->ni_segflg = UIO_SYSSPACE;
		ndp->ni_dirp = shell_name;
#if	SEC_BASE
		audstub_exec_indir();
		audstub_path2();
#endif
#ifdef	OSF1_ADFS
		error = remote_exec_lookup(ndp, &vp);
		if (error)
			goto bad;
		ASSERT(vp != 0);
		{
			int arg0, arg1, arg2;	/* temp var for mig */
			error = remote_exec_setup_suid(vp, p->p_flag, FALSE,
						&arg0, &arg1, &arg2);
		}
		if (error)
			goto bad;
#else	OSF1_ADFS
		if (error = namei(ndp))
			return (error);
		vp = ndp->ni_vp;
		VOP_GETATTR(vp, &vattr, u.u_cred, error);
		if (error)
			goto bad;
		if (exec_check_access(p, vp, &vattr) == -1)
			goto bad;
#endif	OSF1_ADFS
#ifdef OSF1_ADFS
		error = remote_exec_read(vp, (caddr_t)&exdata, MAXINTERP, 
				  (off_t)0, IO_UNIT, &resid);
#else
		error = vn_rdwr(UIO_READ, vp, (caddr_t)&exdata, MAXINTERP,
		    (off_t)0, UIO_SYSSPACE, IO_UNIT, u.u_cred, &resid);
#endif
		if (error)
			goto bad;
		if (resid) {
			error = ENOEXEC;
			goto bad;
		}
		bcopy((caddr_t)ndp->ni_dent.d_name, (caddr_t)shell_name_tail,
		    MAXCOMLEN);
		shell_name_tail[MAXCOMLEN] = '\0';
#ifdef	OSF1_SERVER	/* this has to be sent back to the emulator */
		bcopy((caddr_t)ndp->ni_dent.d_name, (caddr_t)rtp->cfname,
		    MAXCOMLEN);
		rtp->cfname[MAXCOMLEN] = '\0';
		if (shell_arg) {
			bcopy(shell_arg, (caddr_t)rtp->cfarg, MAXINTERP);
		} else {
			rtp->cfarg[0] = '\0';
		}
#endif	/* OSF1_SERVER */
		reset_effective_ids(&privs);	/* shell scripts can't be setid */
	}

	/* See if we can recognize the file format.  Those that we
	 * recognize but don't know how to load will be sent off to
	 * the user space loader.
	 */

#if	BSD_A_OUT
	/*
	 * Read in first few bytes of file for segment sizes, magic number:
	 *	OMAGIC = plain executable
	 *	NMAGIC = RO text
	 *	ZMAGIC = demand paged RO text
	 * Also an ASCII line beginning with #! is
	 * the file name of a ``shell'' and arguments may be prepended
	 * to the argument list if given here.
	 *
	 * SHELL NAMES ARE LIMITED IN LENGTH.
	 *
	 * ONLY ONE ARGUMENT MAY BE PASSED TO THE SHELL FROM
	 * THE ASCII LINE.
	 */
	if ((exdata.ex_exec.a_magic&0xffff) == ZMAGIC ||
	    (exdata.ex_exec.a_magic&0xffff) == NMAGIC) {
		if (exdata.ex_exec.a_text == 0) {
			error = ENOEXEC;
			goto bad;
		}
		do_bsd_a_out= 1;
		goto gotobject;
	} else
	if ((exdata.ex_exec.a_magic&0xffff) == OMAGIC) {
		exdata.ex_exec.a_data += exdata.ex_exec.a_text;
		exdata.ex_exec.a_text = 0;
		do_bsd_a_out = 1;
		goto gotobject;
	} else
#ifdef	balance
	if ((exdata.ex_exec.a_magic&0xffff) == 0x10ea) {       /* ZMAGIC: 0@0 */
						/* no XMAGIC yet */
		int flags = exdata.ex_exec.a_magic & 0xffff0000;

		exdata.ex_exec.a_magic = ZMAGIC;	/* make other code easier */
		exdata.ex_exec.a_magic |= flags; 	/* XXX - keep flags ? */
		if (exdata.ex_exec.a_text == 0) {
			error = ENOEXEC;
			goto bad;
		}
		do_bsd_a_out = 1;
		goto gotobject;
	} else
#endif
#endif	/* BSD_A_OUT */
#if	OSF_MACH_O
	if (exdata.omo_magic == OUR_MOH_MAGIC) {
		if (macho_uses_loader) goto call_exec_loader;

		/*
		 * Now read in the complete file header, 
		 * starting from the beginning.
		 */
#ifdef OSF1_ADFS
		error = remote_exec_read(vp, (caddr_t)&mo_header_buf, 
				  sizeof(mo_header_buf), (off_t)0, IO_UNIT, &resid);
#else
		error = vn_rdwr (UIO_READ, vp, (caddr_t)&mo_header_buf,
			    sizeof(mo_header_buf), (off_t)0, UIO_SYSSPACE,
			    IO_UNIT, u.u_cred, &resid);
#endif
		if (error)
			goto bad;

		/* Convert the canonical version of the header so we can
		 * read it.  If we can't convert it here, send the file to
		 * the user space loader -- maybe it can load the file.
		 */

		conversion_error = decode_mach_o_hdr ((void *)&mo_header_buf,
				     (size_t)sizeof(mo_header_buf),
				     (unsigned long)MOH_HEADER_VERSION,
				     &mo_header);
		if (conversion_error != MO_HDR_CONV_SUCCESS) 
			goto call_exec_loader;

		/* Now we have valid header information; see if we 
		 * can load it here.
		 */
		if ((!(mo_header.moh_flags & MOH_EXECABLE_F))
		    || (mo_header.moh_flags & MOH_RELOCATABLE_F) /* needs relocation */
		    || (mo_header.moh_flags & MOH_UNRESOLVED_F)) {
			goto call_exec_loader;
		}
		/*
		 * The following checks are for whether the file
		 * can be loaded/executed on this system.
		 * We only have to worry about the 
		 * version number (and vendor_type) when
		 * there is a compatibility problem, 
		 * e.g. check that version >= N, where
		 * versions < N are not supported or are
		 * supported differently.  Then this 
		 * program only has to be updated for
		 * those changes it would find incompatible.
		 */
		if ((mo_header.moh_byte_order == OUR_BYTE_ORDER)
		    && (mo_header.moh_data_rep_id == OUR_DATA_REP_ID)
		    && (mo_header.moh_cpu_type == OUR_CPU_TYPE)
		    && (mo_header.moh_max_page_size==PAGE_SIZE)
		    && (mo_header.moh_sizeofcmds > 0)
		    && (mo_header.moh_load_map_cmd_off >= 
			mo_header.moh_first_cmd_off)
		    && (mo_header.moh_load_map_cmd_off < 
			(mo_header.moh_first_cmd_off +
			 mo_header.moh_sizeofcmds))) {
			        do_o_mach_o = 1;
				goto gotobject;
		} else {
			error = ENOEXEC;
			goto bad;
		}
	} else
#endif	/* OSF-MACH-O */
#if	SYSV_COFF
#ifdef	i860
	if (exdata.coff_hdr.magic == I860MAGIC) {
		aouthdr_offset = sizeof(struct filehdr);
		goto gotcoff;
	} else
#endif	/* i860 */
#ifdef	i386
	if (exdata.coff_hdr.magic == I386MAGIC) {
		aouthdr_offset = sizeof(struct filehdr);
		goto gotcoff;
	} else
#endif
#ifdef	multimax
	if (exdata.coff_hdr.magic == N16WRMAGIC ||
	    exdata.coff_hdr.magic == N16ROMAGIC) {
		aouthdr_offset = N16FILHSZ;
		goto gotcoff;
	} else if (exdata.coff_hdr.magic == NS32GMAGIC ||
		   exdata.coff_hdr.magic == NS32SMAGIC) {
		aouthdr_offset = FILHSZ;
		goto gotcoff;
	} else
#endif
#ifdef	mips
	if (exdata.coff_hdr.magic == MIPSMAGIC) {
		aouthdr_offset = FILHSZ;
		goto gotcoff;
	} else
#endif
#ifdef	__mc68000
	if (exdata.coff_hdr.magic == MC68MAGIC) {
		aouthdr_offset = FILHSZ;
		goto gotcoff;
	} else
#endif
#endif	/* SYSV_COFF */

	{
		/* not recognized; may be ascii */
		error = ENOEXEC;
		goto bad;
	}

	/* we only came here via explicit goto; if the magic number was
	 * recognized, there was always a goto
	 */
call_exec_loader:
		/*
		 * Either one of the debugging flags was on, instructing
		 * us to use the user-space loader, or the file was too
		 * complicated to load in the kernel.  Sigh, must finish
		 * with vnode and eventually re-do all the 
		 * processing we've done so far, until we get a 
		 * better interface into the internals of
		 * exec_load_loader().  It does everything, so 
		 * simply return when it completes.
		 */
			    
#ifdef	OSF1_ADFS
		remote_vfree(vp);
#else
		vrele(vp);
#endif

		if (privs.creds) {
			crfree(privs.creds);
			privs.creds = NULL;
		}
	
                error = exec_load_loader(p, 0, (char *)0, uap->fname,
#ifndef	OSF1_SERVER
			uap->argp, uap->envp);
#else
			uap->argp, uap->envp, retval);
#endif

		/* map exec_load_loader() errors */
		switch (error) {

		default:
		case ENOENT:
			error = ENOEXEC;
			break;

		case ESUCCESS:
			if (p->p_realtimer_coe)	/* ITIMER_REAL_COE HACK XXXX */
				(void)clear_p_realtimer(p);
			/* fall through... */
		case E2BIG:
			/* leave error as is */
			break;

		}

		return(error);

#if	SYSV_COFF
gotcoff:
	if (coff_uses_loader) goto call_exec_loader;
	/*
	 * Now read in the second (a.out) header for segment sizes
	 * and magic number:
	 *	OMAGIC = plain executable
	 *	NMAGIC = RO text
	 *	ZMAGIC = demand paged RO text
	 *
	 * XXX On some machines, OMAGIC here does not
	 * XXX mean what OMAGIC means under BSD_A_OUT.  This code
	 * XXX may need to be fixed for those machines.
	 */
	docoff = 1;
#ifdef OSF1_ADFS
	error = remote_exec_read(vp, (caddr_t)&ahdr, sizeof(struct aouthdr),
			  aouthdr_offset, IO_UNIT, &resid);
#else
	error = vn_rdwr(UIO_READ, vp, (caddr_t)&ahdr,
		sizeof(struct aouthdr), aouthdr_offset,
		UIO_SYSSPACE, IO_UNIT, u.u_cred, &resid);
#endif
	if (error)
		goto bad;

#if	defined(mips) || defined(__hp_osf)
	/*
	 * check for unaligned entry point
	 */
	if (ahdr.entry & (sizeof(int)-1)) {
		error = ENOEXEC;
		goto bad;
	}
#endif

	switch (ahdr.magic) {
	    case OMAGIC: /* XXX */
#if	defined(mips) || defined(__hp_osf)
		/* We do it right:
		 * Do not make text read-only, e.g. put it in the data section
		 * Note that by definition text and data are contiguous both
		 * in the file and in memory.
		 */
		ahdr.data_start = ahdr.text_start;
		ahdr.dsize += ahdr.tsize;
		ahdr.tsize = 0;
		break;
#endif
	    case NMAGIC:
	    case ZMAGIC:
		if (ahdr.tsize == 0) {
			error = ENOEXEC;
			goto bad;
		}
		break;

	    default:
		error = ENOEXEC;
		goto bad;
	}
#ifdef mips
	/*
	 * Enforce (artificial) addressability limit: this covers
	 * a chip bug.
	 */
	if ((ahdr.text_start < VM_MIN_ADDRESS) || 
	      (ahdr.data_start < VM_MIN_ADDRESS)) {
		error = ENOEXEC;
		goto bad;
	}
#endif
#ifdef	multimax
	/*
	 *	XXX Alignment flags don't get set in N16 fileheaders.
	 */
	if (aouthdr_offset == N16FILHSZ)
		ahdr.flags |= U_AL_1024;
#endif
#endif	/* SYSV_COFF */

gotobject:

#ifndef	OSF1_SERVER	/* This is done in the emulator */
	/*
	 * Collect arguments and lock vnode/inode.
	 */
	if (error = exec_args_collect((char *)0, UIO_SYSSPACE, 
		uap->fname, uap->argp, uap->envp, shell_name, shell_name_tail, 
		shell_arg, &exec_args, &na, &ne, &nc))
			goto bad;
#else	/* OSF1_SERVER */
	na = ne = nc = 0;		/* Is that allright ?? */
#endif	/* OSF1_SERVER */

#ifdef	sun
	/*
	 *	Save a.out header for Sun debuggers
	 */
	current_thread()->pcb->pcb_exec = exdata.ex_exec;
#endif
#if	SYSV_COFF
	if (docoff)
		error = coff_getxfile(p, vp, &exdata.coff.fhdr, 
		        &ahdr, nc + (na+4)*NBPW, privs.is_priv, special);
#endif

#if	OSF_MACH_O
	if (do_o_mach_o) 
  		error = o_mach_o_getxfile(p, vp, &mo_header, &entry_addr,
			nc + (na+4)*NBPW, privs.is_priv, special);
#endif

#if	BSD_A_OUT
	if (do_bsd_a_out)
#ifdef	sparc
	/*
	 * Make sure user register windows are empty before attempting to
	 * make a new stack.
	 */
		{
			flush_user_windows();
			error = getxfile(p, vp, &exdata.ex_exec,
				SA(nc + (na+4)*NBPW + sizeof(struct rwindow)),
					 privs.is_priv);
		}
#else
		error = getxfile(p, vp, &exdata.ex_exec, nc + (na+4)*NBPW,
				 privs.is_priv, special);

#endif	/* sparc */
#endif	/* BSD_A_OUT */

	if (error)
		goto bad;

	/*
	 * set SUID/SGID protections, if no tracing
	 */
	BM(PROC_LOCK(p));
	pflag = p->p_flag;
	BM(PROC_UNLOCK(p));
	if (pflag&STRC) {

		/* Tracing; don't actually set uids or privileges */

		reset_effective_ids(&privs);
#ifdef	mips
		current_thread()->pcb->trapcause = CAUSEEXEC;
#endif
#ifdef  OSF1_SERVER
                /*
		 * it's too early to do this: the emulator will do it later
		 */
#else   /* OSF1_SERVER */
                unix_master();
                psignal(p, SIGTRAP);
                unix_release();
#endif  /* OSF1_SERVER */
	}

	substitute_real_creds(p, NOCRED, privs.creds->cr_uid, NOCRED,
			      privs.creds->cr_gid, privs.creds);

	/* substitute_real_creds consumes our
	 * reference on the credentials, so
	 * arrange to avoid freeing them later.
	 */
	privs.creds = NULL;

#if	SEC_ARCH
	/* Inform security policy layer of change of ID if any */
	
	SP_CHANGE_SUBJECT();
#endif	/* SEC_ARCH */

#if	SEC_BASE
	compute_subject_privileges(&privs.vsattr);
#endif	/*SEC_BASE*/

#ifdef	OSF1_ADFS
	remote_vfree(vp);
#else
	vrele(vp);
#endif
	vp = NULL;

	/*
	 * Copy back arglist.
	 */
#ifndef	OSF1_SERVER
	exec_args_copyback(0, (char *)0, exec_args, na, ne, nc);
#else
	if (!special)
		exec_auxv_copyback(0, (char *)0, uap->fname, shell_name);
#endif

	unix_master();
	execsigs(u.uu_procp);
	unix_release();

#if	SYSV_COFF
	if (docoff)
#ifdef	multimax
		setregs(ahdr.entry, ahdr.mod_start);
#endif
#if	defined(i386) || defined(mips) || defined (__hp_osf) || defined(i860)
#ifdef	OSF1_SERVER
		set_entry_address(ahdr.entry, rtp->entry, rtp->entry_count);
#else	/* OSF1_SERVER */
		setregs(ahdr.entry);
#endif	/* OSF1_SERVER */
#endif

#endif	/* SYSV_COFF */

#if	OSF_MACH_O
	if (do_o_mach_o)
#ifdef	multimax
#define	MMAX_MOD_START	0x2000
		setregs(entry_addr, MMAX_MOD_START);
#else
#ifdef	OSF1_SERVER
		set_entry_address(entry_addr, rtp->entry, rtp->entry_count);
#else	/* OSF1_SERVER */
		setregs(entry_addr);
#endif	/* OSF1_SERVER */
#endif
#endif	/* OSF_MACH_O */

#if	BSD_A_OUT
	if (do_bsd_a_out)
#ifdef	OSF1_SERVER
		set_entry_address(exdata.ex_exec.a_entry, rtp->entry, rtp->entry_count);
#else	/* OSF1_SERVER */
	        setregs(exdata.ex_exec.a_entry);
#endif	/* OSF1_SERVER */
#endif

#ifdef	vax
	{
		/*
		 *	This belongs in vax.setregs()
		 */
		extern int nsigcode[5];

		bcopy((caddr_t)nsigcode,
		      (caddr_t)(VM_MAX_ADDRESS - sizeof(nsigcode)),
		      sizeof(nsigcode));
	}
#endif
#ifdef	ibmrt
	{
	    	/*
		 *	sigcode[] must agree with declaration in pcb.h
		 *
		 *	sigcode goes at the bottom of the user_stack,
		 *	where, of course, the user's stack can grow
		 *	down on top of it, but this seems unlikely.
		 *	Putting it at the top makes ps(1) unhappy.
		 */
		extern int sigcode[3];
		bcopy((caddr_t)sigcode,
		      (caddr_t)SIGCODE_ADDRESS,
		      sizeof(sigcode));
	}
#endif

	/*
	 * Remember file name for accounting.
	 */
	u.u_acflag.fi_flag &= ~AFORK;
	if (shell_name)
		bcopy((caddr_t)shell_name_tail, (caddr_t)u.u_comm, MAXCOMLEN);
	else {
		if (ndp->ni_dent.d_namlen > MAXCOMLEN)
			ndp->ni_dent.d_namlen = MAXCOMLEN;
		bcopy((caddr_t)ndp->ni_dent.d_name, (caddr_t)u.u_comm,
		    (unsigned)(ndp->ni_dent.d_namlen + 1));
	}
	if (p->p_realtimer_coe)		/* ITIMER_REAL_COE HACK XXXXX */
		(void)clear_p_realtimer(p);
bad:
#ifndef	OSF1_SERVER
	if (exec_args)
		exec_args_free(exec_args);
#endif	/* OSF1_SERVER */
	if (vp) {
#ifdef	OSF1_ADFS
		remote_vfree(vp);
#else
		vrele(vp);
#endif	OSF1_ADFS
	}
	if (privs.creds) {
		crfree(privs.creds);
		privs.creds = NULL;
	}
	if (error == EGETXFILE) {
		struct proc	*p;
		/* 
		 *	getxfile failed, kill the current process.
		 *	Send SIGKILL, blow away other pending signals.
		 */
		p = u.uu_procp;
		unix_master();
		p->p_sig = sigmask(SIGKILL);
		p->p_cursig = SIGKILL;
		u.u_sig = 0;
		u.u_cursig = 0;
		psig();		/* Bye */
		unix_release();
		return (error);
	}
	return (error);
}

#if	BSD_A_OUT
/*
 * Read in and set up memory for executed file.
 */
getxfile(p, vp, ep, nargc, is_priv, special)
	register struct proc *p;
	register struct vnode_proxy *vp;
	struct exec *ep;
	int nargc;
	boolean_t is_priv;
	boolean_t special;	/* TRUE means it is the emulator program */
{
	size_t ts, ds, ss;
	int pagi;
	vm_size_t	text_size, data_size;
	int		busy, error;
	int		read_access = -1;

	if ((ep->a_magic&0xffff) == ZMAGIC)
		pagi = SPAGV;
	else
		pagi = 0;

	/*
	 *	The vm system handles text that is modified
	 *	for tracing - we don't have to worry about it.
	 *	Disallow the exec if the vnode has a reference
	 * 	count > 1 or if the is open for writing.
	 */

#ifdef OSF1_ADFS
	busy = ((ep->a_magic&0xffff) != OMAGIC && 
		remote_exec_check_vnode (vp, &read_access));
#else OSF1_ADFS
	VN_LOCK(vp);
	busy = ((ep->a_magic&0xffff) != OMAGIC && (vp->v_flag & VTEXT) == 0 &&
	    vp->v_usecount != 1 && vp->v_wrcnt);
	VN_UNLOCK(vp);
#endif OSF1_ADFS
	if (busy) {
		error = ETXTBSY;
		goto bad;
	}

	/*
	 * Compute text and data sizes and make sure not too large.
	 * NB - Check data and bss separately as they may overflow 
	 * when summed together.
	 */
	text_size = loader_round_page(ep->a_text);	/* bytes */
	ts = btoc(text_size);				/* machine pages */
#ifdef __hp_osf
        /* This fixes a bug.  However, I know of no other machine where
        the loader page size is larger than the VM page size.  So, I can't
        properly test this.  So, for now the bug is fixed only on HP */
	data_size = round_page(ep->a_data + ep->a_bss);
#else
	data_size = loader_round_page(ep->a_data + ep->a_bss);
#endif

							/* bytes */
	ds = btoc(data_size);				/* machine pages */
	ss = SSIZE + btoc(loader_round_page(nargc));
	PROC_LOCK(p);
	p->p_flag &= ~(SPAGV|SSEQL|SUANOM|SOUSIG|SXONLY);
	PROC_UNLOCK(p);
#ifdef	OSF1_ADFS
	if (read_access == -1) {
		remote_exec_check_vnode (vp, &read_access);
	}
#else
	VOP_ACCESS(vp, VREAD, u.u_cred, read_access);
#endif
	PROC_LOCK(p);
	if (read_access) {
		p->p_flag |= SXONLY;
		p->p_flag &= ~STRC;
		error = 0;
	}
#if	SEC_BASE
	else if (is_priv)
		p->p_flag |= SXONLY;	/* prevent core dumps of priv pgms */
#endif	/* SEC_BASE */
	p->p_flag |= pagi | SEXEC;
	PROC_UNLOCK(p);
#define unix_stack_size	(u.u_rlimit[RLIMIT_STACK].rlim_cur)

	(void) vm_exec(is_priv);

	error = program_loader(vp, ep, pagi, special);
	U_HANDY_LOCK();
	u.u_tsize = ts;
	u.u_dsize = ds;
	u.u_ssize = ss;
#if PROFILING
	u.u_prof.pr_scale = 0;
#endif
	U_HANDY_UNLOCK();
bad:
	return (error);
}
#endif	/* BSD_A_OUT */

#if	defined(sun) || defined(i386) || defined(i860)
#define	FILE_OFFSET	0	/* beware - not LOADER_PAGE_SIZE on SUN */
extern vm_prot_t	page_0_protection;
#endif

extern vm_prot_t	page_0_protection;

#if	SYSV_COFF
/*
 * Version of getxfile for machines that use the Common Object File Format.
 * Some vendors have their own idea of what COFF is supposed to be, so
 * we have still machine-dependencies here.
 */
coff_getxfile(p, vp, fhd, ap, nargc, is_priv, special)
	register struct proc *p;
	register struct vnode_proxy *vp;
	struct filehdr *fhd;
	struct aouthdr *ap;
	int nargc;
	boolean_t is_priv;
	boolean_t special;	/* TRUE means it is the emulator program */
{
	size_t 		ts, ds, ss;
	int 		pagi, busy;
	vm_offset_t	addr;
	vm_size_t	size;
#ifdef	OSF1_SERVER
	task_t		my_task;
#else	/* OSF1_SERVER */
	vm_map_t	my_map;
#endif	/* OSF1_SERVER */
	vm_offset_t	vm_text_start, vm_text_end;
	vm_offset_t	vm_data_start, vm_data_end;
	vm_offset_t	vm_end;
	int		error;
	int		read_access = -1;

#ifdef	multimax
	vm_offset_t	sectalign;

	/*
	 *	Page size used by loader is encoded in flags field.
	 *	Only 1024 and 4096 are supported currently.
	 */
	if (ap->flags & U_AL_1024)
		sectalign = 1024;
	else if (ap->flags & U_AL_4096)
		sectalign = 4096;
	else {
		error = ENOEXEC;
		goto bad;
	}

#define SECTALIGN		sectalign
#define NOPAGI_TEXT_OFFSET	SECTALIGN
#define NOPAGI_DATA_OFFSET	(SECTALIGN + ap->tsize)
#define PAGI_TEXT_OFFSET	SECTALIGN
#define PAGI_DATA_OFFSET	(SECTALIGN + loader_round_page(ap->tsize))
#endif	/* multimax */

#ifdef	i386
#define NOPAGI_TEXT_OFFSET	((fhd->f_nscns * sizeof(struct scnhdr)) + \
				 sizeof(struct filehdr) + fhd->f_opthdr)
#define NOPAGI_DATA_OFFSET	(NOPAGI_TEXT_OFFSET + ap->tsize)
#define PAGI_TEXT_OFFSET	0
#define PAGI_DATA_OFFSET	trunc_page(ap->text_start + ap->tsize)
#endif	/* i386 */

#ifdef i860
#define NOPAGI_TEXT_OFFSET	((fhd->f_nscns * sizeof(struct scnhdr)) + \
				 sizeof(struct filehdr) + fhd->f_opthdr)+0x10
#define NOPAGI_DATA_OFFSET	(NOPAGI_TEXT_OFFSET + ap->tsize)
#define PAGI_TEXT_OFFSET	0
#define PAGI_DATA_OFFSET	trunc_page((NOPAGI_TEXT_OFFSET+ap->tsize) - (ap->data_start - vm_data_start))

#endif	/* i860 */

#ifdef	__hp_osf
#define NOPAGI_TEXT_OFFSET	((fhd->f_nscns * sizeof(struct scnhdr)) + \
				 sizeof(struct filehdr) + fhd->f_opthdr)
#define NOPAGI_DATA_OFFSET	(NOPAGI_TEXT_OFFSET + ap->tsize)
#define PAGI_TEXT_OFFSET	SECTALIGN
#define PAGI_DATA_OFFSET	(PAGI_TEXT_OFFSET + ap->tsize)
#endif	/* __hp_osf */

#ifdef	mips
#define SECTALIGN		((vm_offset_t)4096)
#define NOPAGI_TEXT_OFFSET	N_TXTOFF(*fhd, *ap)
#define NOPAGI_DATA_OFFSET	(N_TXTOFF(*fhd, *ap) + ap->tsize)
#define PAGI_TEXT_OFFSET	NOPAGI_TEXT_OFFSET
#define PAGI_DATA_OFFSET	NOPAGI_DATA_OFFSET
#endif	/* mips */

	/*
	 *	Check pageability.
	 */
	if (ap->magic == ZMAGIC)
		pagi = SPAGV;
	else
		pagi = 0;

	/*
	 *	The vm system handles text that is modified
	 *	for tracing - we do not have to worry about it.
	 *	Disallow the exec if the vnode has a reference
	 * 	count > 1 or if the is open for writing.
	 */

#ifdef OSF1_ADFS
	busy = (ap->magic != OMAGIC &&
		remote_exec_check_vnode (vp, &read_access));
#else OSF1_ADFS
	VN_LOCK(vp);
	busy = (ap->magic != OMAGIC && (vp->v_flag&VTEXT) == 0 &&
	    vp->v_usecount != 1 && vp->v_wrcnt);
	VN_UNLOCK(vp);
#endif OSF1_ADFS
	if (busy) {
		error = ETXTBSY;
		goto bad;
	}

	/*
	 * Compute text, data and stack sizes.
	 */
#if 0
printf("Coff file: magic=%x tsize=%x dsize=%x bsize=%x entry=%x t_start=%x d_start=%x\n",
       ap->magic,ap->tsize, ap->dsize, ap->bsize, ap->entry, ap->text_start, ap->data_start); 
#endif
	ts = btoc(loader_round_page(ap->tsize));
#ifdef __hp_osf
        /* This fixes a bug.  However, I know of no other machine where
        the loader page size is larger than the VM page size.  So, I can't
        properly test this.  So, for now the bug is fixed only on HP */
	ds = btoc(round_page(ap->bsize + ap->dsize));
#else
	ds = btoc(loader_round_page(ap->bsize + ap->dsize));
#endif
	ss = SSIZE + btoc(loader_round_page(nargc));

	PROC_LOCK(p);
	p->p_flag &= ~(SPAGV|SSEQL|SUANOM|SOUSIG|SXONLY);
	PROC_UNLOCK(p);
#ifdef	OSF1_ADFS
	if (read_access == -1) {
		remote_exec_check_vnode (vp, &read_access);
	}
#else
	VOP_ACCESS(vp, VREAD, u.u_cred, read_access);
#endif
	PROC_LOCK(p);
	if (read_access) {
		p->p_flag |= SXONLY;
		p->p_flag &= ~STRC;
		error = 0;
	}
#if	SEC_BASE
	else if (is_priv)
		p->p_flag |= SXONLY;	/* prevent core dumps of priv pgms */
#endif	/* SEC_BASE */
	p->p_flag |= pagi | SEXEC;
	PROC_UNLOCK(p);

#define unix_stack_size	(u.u_rlimit[RLIMIT_STACK].rlim_cur)

#ifdef	OSF1_SERVER
	my_task = u.uu_procp->p_task;
#define	my_map	my_task		/* not a map but avoids changing code */
#else	/* OSF1_SERVER */
	my_map = current_task()->map;
#endif	/* OSF1_SERVER */
	/*
	 *	Even if we are execing the same image (the RFS server
	 *	does this, for example), we do not have to unlock the
	 *	vnode; deallocating it does not require it to be locked.
	 */
	(void) vm_exec(is_priv);
	
	/*
	 *	Allocate low-memory stuff: text, data, bss.
	 *	Read text and data into lowest part, then make text read-only.
	 */

	/*
	 *	Remember where text and data start.
	 */
	U_HANDY_LOCK();
	u.u_text_start = (caddr_t) ap->text_start;
	u.u_data_start = (caddr_t) trunc_page(ap->data_start);
	U_HANDY_UNLOCK();

	/*
	 *	Note vm boundaries for data and text segments.  If data
	 *	and text overlap a page, that is considered data.
	 *
	 */
	vm_text_start = trunc_page(ap->text_start);
	vm_text_end = round_page(ap->text_start + ap->tsize);
	vm_data_start = trunc_page(ap->data_start);
	vm_data_end = round_page(ap->data_start + ap->dsize);
	vm_end = round_page(ap->data_start + ap->dsize + ap->bsize);
#if defined(i386) || defined(i860)
	ds = btoc(round_page(ap->data_start + ap->dsize + ap->bsize) - vm_data_start);
#endif /* defined(i386) || defined(i860) */


	if (vm_text_end > vm_data_start)
		vm_text_end = vm_data_start;

	error = 0;

#ifdef	OSF1_SERVER
	if (pagi == 0) {
		/*
		 * Not demand paged.
		 * In OMAGIC images the code is not read-protected.
		 */
		/*
		 * Allocate the data segment (OMAGIC & NMAGIC).
		 */
		if (ap->dsize > 0) {
			addr = vm_data_start;
			size = vm_end - vm_data_start;	/* include bss */
			if (vm_allocate(my_map, &addr, size, FALSE) != KERN_SUCCESS) {
				uprintf("%s: Data section too big.\n", u.u_comm);
				goto suicide;
			}
		}
		/* 
		 * Allocate the text segment (NMAGIC).
		 */
		if (ap->tsize > 0) {
			addr = vm_text_start;
			size = vm_text_end - vm_text_start;
			if (vm_allocate(my_map, &addr, size, FALSE) != KERN_SUCCESS) {
				uprintf("%s: Text section too big.\n", u.u_comm);
				goto suicide;
			}
		}
		/*
		 * Allocate temporary space for image
		 * in server's address space
		 */
		size = max(ap->tsize, ap->dsize);
		if (size > 0 &&
		    vm_allocate(mach_task_self(), &addr, size, TRUE)
		    !=  KERN_SUCCESS) {
			uprintf("Cannot find space for exec'ing.\n");
			goto suicide;
		}

		/*
		 *	Read in the data segment (OMAGIC & NMAGIC).  It goes on
		 *	the next loader_page boundary after the text.
		 */
		if (ap->dsize > 0) {
#ifdef OSF1_ADFS
			error = remote_exec_read(vp, (caddr_t) addr, 
					  (int) ap->dsize,
					  NOPAGI_DATA_OFFSET, IO_UNIT,
					  (int *)0);
#else
			error = vn_rdwr(UIO_READ, vp,
					(caddr_t) addr, (int) ap->dsize,
					NOPAGI_DATA_OFFSET, UIO_SYSSPACE, IO_UNIT,
					u.u_cred, (int *)0);
#endif
			if (error == 0) {
				ux_server_thread_blocking();
				error = vm_write(my_task, ap->data_start, addr, round_page(ap->dsize));
				ux_server_thread_unblocking();
			}
		}
		/*
		 *	Read in text segment, and read-protect it
		 *	if necessary (NMAGIC).
		 */
		if ((error == KERN_SUCCESS) && (ap->tsize > 0)) {
#ifdef OSF1_ADFS
			error = remote_exec_read(vp, (caddr_t) addr, 
					  (int) ap->tsize,
					  NOPAGI_TEXT_OFFSET, IO_UNIT,
					  (int *) 0);
#else
			error = vn_rdwr(UIO_READ, vp,
				(caddr_t) addr, (int) ap->tsize,
				NOPAGI_TEXT_OFFSET, UIO_SYSSPACE, IO_UNIT,
				u.u_cred, (int *) 0);
#endif
			ux_server_thread_blocking();
			if (error == 0) {
				error = vm_write(my_task, ap->text_start, addr, round_page(ap->tsize));
			}

			if ((error == KERN_SUCCESS) && (ap->magic != OMAGIC)) {
				(void) vm_protect(my_map,
					vm_text_start,
					vm_text_end - vm_text_start,
					FALSE,
					VM_PROT_READ|VM_PROT_EXECUTE);
			}
			ux_server_thread_unblocking();
		}
		if (size > 0)
			(void) vm_deallocate(mach_task_self(), addr, size);
	}
	else {
		memory_object_t	pager;

		/*
		 *	Allocate a region backed by the execed vnode.
		 */

#ifdef OSF1_ADFS
		pager = (memory_object_t)remote_inode_pager_setup(vp, TRUE,TRUE);
#else
		pager = (memory_object_t)inode_pager_setup(vp, TRUE, TRUE);
#endif
		/* XXX what if setup failed? */

		ux_server_thread_blocking();	/* May contact vnode pager */
		/*
		 *	Map the text segment.
		 */
		if (ap->tsize > 0) {
			if (vm_map(my_map, &vm_text_start,
				   vm_text_end - vm_text_start, 0, FALSE,
				   pager, PAGI_TEXT_OFFSET, TRUE,
				   VM_PROT_READ|VM_PROT_EXECUTE,
				   VM_PROT_ALL, VM_INHERIT_COPY)
			    != KERN_SUCCESS) {
				uprintf("%s: Cannot map text.\n",u.u_comm);
				inode_pager_release(pager);
				ux_server_thread_unblocking();
				goto suicide;
			}
		}
		/*
		 *	Map the data segment, if any.
		 */
		if (vm_data_end > vm_data_start) {
			if (vm_map(my_map, &vm_data_start,
			    vm_data_end - vm_data_start, 0, FALSE, pager,
			    PAGI_DATA_OFFSET,
			    TRUE, VM_PROT_READ|VM_PROT_WRITE,
			    VM_PROT_ALL, VM_INHERIT_COPY)
			      != KERN_SUCCESS) {
				uprintf("%s: Cannot map data.\n", u.u_comm);
				inode_pager_release(pager);
				ux_server_thread_unblocking();
				goto suicide;
			}
		}

		inode_pager_release(pager);

		/*
		 *	Allocate bss.  First check whether any more is needed.
		 */

		
		size =  vm_end - vm_data_end;
		if ( (long)size > 0 ) {			/* == missing     */
			addr = vm_data_end;
			if (vm_allocate(my_map, &addr, size, FALSE)
			    != KERN_SUCCESS) {
				    uprintf("%s: Cannot allocate space for bss.\n", u.u_comm);
				    ux_server_thread_unblocking();
				    goto suicide;
			}
			vm_protect(my_map, addr, size, FALSE, VM_PROT_READ|VM_PROT_WRITE);
		}

		/*
		 *	If the data segment does not end on a VM page
		 *	boundary, we have to clear the remainder of the VM
		 *	page it ends on so that the bss segment will
		 *	(correctly) be zero.
		 */
		addr = ap->data_start + ap->dsize;
		size = vm_data_end - addr;
		if (size > 0) {
			pointer_t	local_addr;
			vm_size_t	ret_size;
			vm_offset_t	addr_page = trunc_page(addr);
			kern_return_t	rt;

			rt = vm_read(my_task, addr_page, round_page(size),
				     &local_addr, &ret_size);
			if (rt == KERN_SUCCESS) {
				bzero((char *)local_addr + (addr - addr_page), size);
				rt = vm_write(my_task, addr_page, local_addr, ret_size);
				(void) vm_deallocate(mach_task_self(), local_addr, ret_size);
			}
			if (rt != KERN_SUCCESS) {
				uprintf("Cannot zero partial data page\n");
				ux_server_thread_unblocking();
				goto suicide;
			}
		}
		ux_server_thread_unblocking();
	}
#if	defined(sun) || defined(i386) || defined(i860)
	if (!special) {
	    addr = 0;
	    size = ap->text_start;
	    if (trunc_page(size) > 0) {
		/*
		 *	Text does not start in page 0 : protect
		 *	the region from 0 to the start of text.
		 */
		ux_server_thread_blocking();    /* May contact vnode pager */
		if (vm_map(my_map, &addr, vm_text_start, 0, FALSE,
			   MEMORY_OBJECT_NULL, (vm_offset_t) 0, FALSE,
			   page_0_protection, page_0_protection | VM_PROT_READ, 
#if	REMOTE_PROC | TNC
			   VM_INHERIT_COPY)
#else	/* REMOTE_PROC | TNC */
			   VM_INHERIT_SHARE)
#endif	/* REMOTE_PROC | TNC */
		    != KERN_SUCCESS) {
		    uprintf("Cannot reserve user page 0.\n");
		    ux_server_thread_unblocking();
		    goto suicide;
		}
	    	ux_server_thread_unblocking();
	    } else {
		pointer_t	local_addr;
		vm_size_t	ret_size;
		kern_return_t	rt;
		/*
		 *	Text starts in page 0 :
		 *	Zero location 0 so that stupid code that
		 *	checks for *0 == 0 will work
		 */
		ux_server_thread_blocking();    /* May contact vnode pager */
		if (vm_protect(my_task, 0, round_page(sizeof(int)),
			       FALSE, VM_PROT_ALL) != KERN_SUCCESS) {
		    uprintf("Cannot unprotect user page 0.\n");
		    ux_server_thread_unblocking();
		    goto suicide;
		}
		rt = vm_read(my_task, 0, round_page(sizeof(int)),
			     &local_addr, &ret_size);
		if (rt == KERN_SUCCESS) {
		    *((int *)local_addr) = 0;
		    rt = vm_write(my_task, 0, local_addr, ret_size);
		    (void) vm_deallocate(mach_task_self(), local_addr, ret_size);
		    if (rt != KERN_SUCCESS) {
			uprintf("Cannot zero user location 0\n");
			ux_server_thread_unblocking();
			goto suicide;
		    }
		}		
		if (vm_protect(my_task, 0, round_page(sizeof(int)),
			       FALSE, VM_PROT_READ|VM_PROT_EXECUTE) != KERN_SUCCESS) {
		    uprintf("Cannot reprotect user page 0.\n");
		    ux_server_thread_unblocking();
		    goto suicide;
		}
                ux_server_thread_unblocking();
	    }
	} /* !special */
#endif /* defined(sun) || defined(i386) || defined(i860) */

#else	/* OSF1_SERVER */
		/* This section is intentionally left blank (jqr) */
#endif	/* OSF1_SERVER */

	/*
	 *	Create the stack.  (Deallocate the old one and create a 
	 *	new one).
	 */

	size = round_page(unix_stack_size);
	U_HANDY_LOCK();
	u.u_stack_start = (caddr_t) (addr = trunc_page(VM_MAX_ADDRESS - size));
	u.u_stack_end = u.u_stack_start + size;
	u.u_stack_grows_up = FALSE;
	U_HANDY_UNLOCK();
#ifndef	OSF1_SERVER	/* This has already has been done in vm_exec (JQR) */
	(void) vm_deallocate(my_map, addr, size);
#endif	/* OSF1_SERVER */
	if (vm_allocate(my_map, &addr, size, FALSE) != KERN_SUCCESS) {
		uprintf("%s: Cannot find space for stack.\n", u.u_comm);
		goto suicide;
	}

	U_HANDY_LOCK();
	u.u_tsize = ts;
	u.u_dsize = ds;
	u.u_ssize = ss;
	u.u_prof.pr_scale = 0;
	U_HANDY_UNLOCK();
bad:
	return (error);
suicide:
	return (EGETXFILE);
}
#endif	/* SYSV_COFF */

#if	OSF_MACH_O
/* Version of getxfile for the OSF/mach-o object file format. */

o_mach_o_getxfile (p, vp, mhp, entryp, nargc, is_priv, special)
	register struct proc *p;
        register struct vnode_proxy	*vp;
        mo_header_t		*mhp;
        long			*entryp;
        int			nargc;
	boolean_t		is_priv;
	boolean_t special;	/* TRUE means it is the emulator program */
{
	caddr_t		lc_bufferp = NULL;
	load_cmd_map_command_t	*ld_map_cmdp;
	ldc_header_t	*lcp;
	entry_command_t *ent_lcp;
	long		entry_tmp=0;
	region_command_t  *reg_lcp;
	int		i;
	int		error=0;
	int		resid;
	int		nentries;
	int		last_region_id;
	int		nregions;
	int		bad_omo_format;
	int		saw_writeable_region;
	int		saw_nonwriteable_region;
	char		*p1;
	char		*p2;
	vm_offset_t	highest_addr = 0;
	vm_offset_t	lowest_addr = 0;
	vm_offset_t	lowest_w_addr = 0;
	vm_offset_t	highest_nonw_addr = 0;
	vm_prot_t	region_prot;
	vm_size_t	vm_region_size;
	vm_size_t	file_size;
	vm_size_t	vm_mapped_size;
	int		pagi, busy;
	vm_offset_t	addr;
	vm_size_t	size;
#ifdef	OSF1_SERVER
	task_t		my_task;
#else	/* OSF1_SERVER */
	vm_map_t	my_map;
#endif	/* OSF1_SERVER */
	vm_offset_t	vm_region_start;
	vm_offset_t	vm_region_end;
	memory_object_t	pager;
	struct vattr	vattr;

#define LOAD_CMD_P(cmd_off) (lc_bufferp - mhp->moh_first_cmd_off + cmd_off)


	/* 
	 * Check that the page size linked for will work here.
	 * Eventually, the linked page size must be an integral
	 * multiple of the current VM page size.
	 * However, for now they must be equal so that object files
	 * converted from other formats will be mapped correctly.
	 */

	if (mhp->moh_max_page_size != PAGE_SIZE) {
		error = ENOEXEC;
		goto out;
	}

	pagi = SPAGV;		/* assume all mach-o files can be mapped */

	/*
	 *	The vm system handles text that is modified
	 *	for tracing - we do not have to worry about it.
	 *	Disallow the exec if the vnode has a reference
	 * 	count > 1 or if the file is open for writing.
	 */

#ifdef OSF1_ADFS
	busy = remote_exec_check_vnode (vp, &error);
#else OSF1_ADFS
	busy = (((vp->v_flag&VTEXT)==0) && (vp->v_usecount !=1) && vp->v_wrcnt);
#endif OSF1_ADFS
	if (busy) {
		error = ETXTBSY;
		goto out;
	}

	PROC_LOCK(p);
	p->p_flag &= ~(SPAGV|SSEQL|SUANOM|SOUSIG|SXONLY);
	PROC_UNLOCK(p);
#ifndef	OSF1_ADFS
	VOP_ACCESS(vp, VREAD, u.u_cred, error);
#endif	OSF1_ADFS
	PROC_LOCK(p);
	if (error) {
		p->p_flag |= SXONLY;
		p->p_flag &= ~STRC;
		error = 0;
	}
#if	SEC_BASE
	else if (is_priv)
		p->p_flag |= SXONLY;	/* prevent core dumps of priv pgms */
#endif	/* SEC_BASE */
	p->p_flag |= pagi | SEXEC;
	PROC_UNLOCK(p);

#define unix_stack_size	(u.u_rlimit[RLIMIT_STACK].rlim_cur)

	/* read in load commands */

	lc_bufferp = kalloc((long)(mhp->moh_sizeofcmds));
	if (lc_bufferp == NULL) {
		error = ENOEXEC;
		goto out;
	}

#ifdef OSF1_ADFS
	error = remote_exec_read(vp, lc_bufferp, (long)(mhp->moh_sizeofcmds), 
			  (off_t)(mhp->moh_first_cmd_off), IO_UNIT, &resid);
#else
	error = vn_rdwr(UIO_READ, vp, lc_bufferp, 
			    (long)(mhp->moh_sizeofcmds), 
			    (off_t)(mhp->moh_first_cmd_off),
			    UIO_SYSSPACE, IO_UNIT, u.u_cred, &resid);
#endif
	if (error)
		goto out;
	if (resid) {
		error = ENOEXEC;
		goto out;
	}

	ld_map_cmdp = (load_cmd_map_command_t *)
		(LOAD_CMD_P(mhp->moh_load_map_cmd_off));

	/* Check that ld_map_cmdp is valid and that the load map command
	   has  reasonable values */

	if ((!VALID_LDC_HEADER_PTR(ld_map_cmdp))
	|| (ld_map_cmdp->ldc_cmd_size > (mhp->moh_sizeofcmds -
					     (mhp->moh_load_map_cmd_off -
					     mhp->moh_first_cmd_off)))) {
		error = ENOEXEC;
		goto clean_up_less;
	}

	/* check that nentries map entries fit in the space occupied by
	 * the load command map */

	nentries = ld_map_cmdp->lcm_nentries;
	p1 = (char *) ld_map_cmdp;
	p2 = p1 + ld_map_cmdp->ldc_cmd_size;

	if ((char *)(&ld_map_cmdp->lcm_map[nentries]) > p2) {
		error = ENOEXEC;
		goto clean_up_less;
	}

	/*
	 * Loop through the load commands looking for the entry point
	 * and the region commands.
	 *
	 * Look for an entry command.  If we dont find
	 * one, return ENOEXEC, since all execable files
	 * should have an entry.  If you want to change the code
	 * to call exec_load_loader instead, you should
	 * read in the load commands and check for the
	 * entry at the time you check for magic numbers,
	 * so you can call exec_load_loader BEFORE you
	 * collect the arguments.
	 *
	 * Find all the region load commands and check that we can load them
	 * all.  We need to check in a separate pass from mapping so that
	 * we can return ENOEXEC if there are format problems.  Once we
	 * have cleared our address space and begun mapping, the only error
	 * that should be returned is EGETXFILE.  In order to simplify
	 * the second pass, the first pass zeroes out all the load command
	 * map entries that don't correspond to region commands.
	 */

	/* Initialize stuff */
	/* already initialized entry_tmp and vm address range variables */

	bad_omo_format = FALSE;
	last_region_id = -1;	
	nregions = 0;
	saw_writeable_region = FALSE;
	saw_nonwriteable_region = FALSE;
#ifndef	OSF1_ADFS
	VOP_GETATTR(vp, &vattr, u.u_cred, error);	/* get file size */
	if (error != 0)
		 goto clean_up_less;
#endif	OSF1_ADFS

	for (i = 0; i < nentries; i++) {
		if (ld_map_cmdp->lcm_map[i] == 0) continue;
		lcp = (ldc_header_t *)(LOAD_CMD_P(ld_map_cmdp->lcm_map[i]));

		/* check the validity of the load command pointer */
		if ((ld_map_cmdp->lcm_map[i] >= (mhp->moh_sizeofcmds +
						 mhp->moh_first_cmd_off))
		    || (!VALID_LDC_HEADER_PTR(lcp))) {
			bad_omo_format = TRUE;
			break;
		}

		if (lcp->ldci_cmd_type == LDC_ENTRY) {
			ent_lcp = (entry_command_t *)lcp;
			if ((ent_lcp->entc_flags & ENT_VALID_ABSADDR_F)) {
				entry_tmp = (long)(ent_lcp->entc_absaddr);
			}
		}
		if (lcp->ldci_cmd_type != LDC_REGION) {
			ld_map_cmdp->lcm_map[i] = 0;
			continue;
		}
		reg_lcp = (region_command_t *)lcp;

		if (reg_lcp->regc_vm_size == 0) {   /* ignore null region */
			ld_map_cmdp->lcm_map[i]  = 0;
			continue;
		}

		/* do not bother to check whether region offset and size are
		 * within the file bounds -- if they aren't, they will
		 * only affect the process
		 */

		vm_region_start = (vm_offset_t)(reg_lcp->regc_vm_addr);
		vm_region_size = (vm_size_t)(round_page(reg_lcp->regc_vm_size));
		vm_region_end = vm_region_start + vm_region_size - 1;

		if ((!(reg_lcp->regc_flags & REG_ABS_ADDR_F))

			/* region MUST start on a page boundary */
		    || (vm_region_start != trunc_page(vm_region_start))

		    || (reg_lcp->regc_initprot == MO_PROT_NONE)

			/* must have read or write or execute */
		    || ((!(reg_lcp->regc_initprot & MO_PROT_READ)) &&
			(!(reg_lcp->regc_initprot & MO_PROT_WRITE)) &&
			(!(reg_lcp->regc_initprot & MO_PROT_EXECUTE)))) {
			    bad_omo_format = TRUE;
			    break;
		    }

		last_region_id = i;
		nregions++;

		 /* Now accumulate information for u.u_text and u.u_data. */

		if (nregions == 1) {
			lowest_addr = vm_region_start;
			highest_addr = vm_region_end;
		} else {if (vm_region_start < lowest_addr)
				lowest_addr = vm_region_start;
			if (vm_region_end > highest_addr)
				highest_addr = vm_region_end;
		}

		if ((reg_lcp->regc_initprot & MO_PROT_WRITE) != 0) {
			if (saw_writeable_region == FALSE) {
				lowest_w_addr = vm_region_start;
				saw_writeable_region = TRUE;
			}
			else { if (vm_region_start < lowest_w_addr)
				       lowest_w_addr = vm_region_start;
		       }
		}
		else {  /* have a non-writeable region */
			if (saw_nonwriteable_region == FALSE) {
				highest_nonw_addr = vm_region_end;
				saw_nonwriteable_region = TRUE;
			}
			else { if (vm_region_end > highest_nonw_addr)
				       highest_nonw_addr = vm_region_end;
		       }
		}

	}		/* end of first load command loop */


	if ((entry_tmp == 0) 
	    || (last_region_id < 0) 
	    || (bad_omo_format == TRUE)) {
		error = ENOEXEC;
		goto clean_up_less;
	}

	/* Now that we're done with the format checks, we can clear the
	 * address space and map the regions.
	 */

#ifdef	OSF1_SERVER
	my_task = u.uu_procp->p_task;
#undef	my_map
#define	my_map	my_task		/* not a map but avoids changing code */
#else	/* OSF1_SERVER */
	my_map = current_task()->map;
#endif	/* OSF1_SERVER */
	/*
	 *	Even if we are execing the same image (the RFS server
	 *	does this, for example), we do not have to unlock the
	 *	vnode; deallocating it does not require it to be locked.
	 */
	(void) vm_exec(is_priv);

	/* Set up pager.  Keep vnode unlocked while we're mucking with
	 * file and hope that someone doesn't zap us.
	 * Assume that ALL Mach-O objects are to be mapped and demand paged.
	 */

#ifdef OSF1_ADFS
	pager = (memory_object_t)remote_inode_pager_setup(vp, TRUE, TRUE);
#else
	pager = (memory_object_t)inode_pager_setup(vp, TRUE, TRUE);
#endif
	/* XXX what if it failed? */

#ifdef	OSF1_SERVER
	ux_server_thread_blocking();	/* May contact vnode pager */
#endif	OSF1_SERVER

	/* Loop through load commands again, mapping all the regions. */

	for (i = 0; i <= last_region_id; i++) {
		if (ld_map_cmdp->lcm_map[i] == 0)
			continue;	/* only regions are left in map */
		reg_lcp = (region_command_t *)
			(LOAD_CMD_P(ld_map_cmdp->lcm_map[i]));

		/*
		 * Calculate VM boundaries in case those specified are
		 * not on page boundaries.
		 */

		vm_region_start = (vm_offset_t)(reg_lcp->regc_vm_addr);
		file_size = (vm_size_t)(reg_lcp->ldc_header.ldci_section_len);
		vm_mapped_size = round_page(file_size);
		vm_region_size = (vm_size_t)(round_page(reg_lcp->regc_vm_size));

		/* Translate the protection attributes */

		region_prot = VM_PROT_NONE;
		if (reg_lcp->regc_initprot & MO_PROT_READ) 
			region_prot |= VM_PROT_READ;
		if (reg_lcp->regc_initprot & MO_PROT_WRITE) 
			region_prot |= VM_PROT_WRITE;
		if (reg_lcp->regc_initprot & MO_PROT_EXECUTE) 
			region_prot |= VM_PROT_EXECUTE;

		/* Map the file part of the region. */

		if (vm_mapped_size > 0) {
			if (vm_map(my_map, &vm_region_start, vm_mapped_size,
				   0, FALSE, pager, 
				   (vm_offset_t)(reg_lcp->ldc_header.ldci_section_off),
				   TRUE, region_prot, VM_PROT_ALL,
				   VM_INHERIT_COPY) 
				!= KERN_SUCCESS) {
					uprintf("%s: Cannot map region with load map id %d.\n", u.u_comm, i);
					error = EGETXFILE;
					goto clean_up_more;
			}

			/*
			 * Map the bss part of the region, if any.  If the
			 * bss part does not start on a page boundary, we
			 * first zero out the remainder of the last mapped
			 * page; this will cause a copy-on-write of that 
			 * page to be triggered.  Then any remaining bss is
			 * allocated from anonymous memory.
			 */
			if ((vm_size_t)(reg_lcp->regc_vm_size) > file_size) {			                               /* have trailing bss */
				size = vm_mapped_size - file_size;
				if (size > 0) {	    /* VOP is still unlocked */
#ifdef	OSF1_SERVER
					pointer_t	local_addr;
					vm_size_t	ret_size;
					vm_offset_t	addr_page;
					kern_return_t	rt;
					
					addr = vm_region_start + file_size; 
					addr_page  = trunc_page(addr);

					rt = vm_read(my_task, addr_page, round_page(size),
						     &local_addr, &ret_size);
					if (rt == KERN_SUCCESS) {
						bzero((char *)local_addr + (addr - addr_page), size);
						rt = vm_write(my_task, addr_page, local_addr, ret_size);
						(void) vm_deallocate(mach_task_self(), local_addr, ret_size);
					}
					if (rt != KERN_SUCCESS) {
						uprintf("%s: Cannot zero partial page of region with id %d.\n", u.u_comm, i);
						error = EGETXFILE;
						goto clean_up_more;
					}
#else	/* OSF1_SERVER */
					if (copyout(vm_kern_zero_page,
						    (caddr_t)(vm_region_start + file_size),
						    size)) {	/* no write access?? */
						uprintf("%s: Cannot zero partial page of region with id %d.\n", u.u_comm, i);
						error = EGETXFILE;
						goto clean_up_more;
					}
#endif OSF1_SERVER
				/* Leave VOP unlocked until done with regions. 	*/
				}	/* end of padding last mapped page with zeroes */
			}
		}			/* end of mapping from file */

		if (vm_region_size > vm_mapped_size) {
			addr = vm_region_start + vm_mapped_size;
			size = vm_region_size - vm_mapped_size;
			if (vm_allocate(my_map, &addr, size, FALSE)
			    != KERN_SUCCESS) {
				    uprintf ("%s: Cannot allocate space for bss in region with id %d.\n", u.u_comm, i);
                                    error = EGETXFILE;
                                    goto clean_up_more;
			    }
                        vm_protect(my_map, addr, size, FALSE, region_prot);
                 }		   	/* end of allocating bss */
        }     	                   	/* end of second load command loop */


        /* Fill in fields in u_area.
	 * The approach here is to consider data as everything
	 * between the lowest writeable address and the highest 
	 * address used, and to consider text as everything 
	 * non-writeable that is lower than data.
	 *
	 * This may result in one of several anomalies, including:
	 * - the real text section not showing up in
	 *   the u-area (if it is above a writeable region),
	 * - "data" being nonwriteable (if there are no writeable regions).
	 * These are OK.  The purpose of the u-area is no longer
	 * to accurately reflect object sections, but rather
	 * 1) to make malloc (sbrk) work, which uses dsize, and
	 * 2) to make core files work well enough to be useable
	 * (even is extra stuff is copied).
	 */

	if ((saw_nonwriteable_region == FALSE) 
	    || (lowest_addr == lowest_w_addr)) {	/* no "text" */
		u.u_text_start = (caddr_t) 0;
		u.u_tsize = (size_t)(btoc(0));
	} else {
		u.u_text_start = (caddr_t) lowest_addr;
		(highest_nonw_addr < lowest_w_addr) ?
			(u.u_tsize = (size_t)(btoc(highest_nonw_addr - lowest_addr + 1))) :
				(u.u_tsize = (size_t)(btoc(lowest_w_addr - lowest_addr)));
	}

	if (saw_writeable_region == FALSE)
		lowest_w_addr = lowest_addr;	/* set it so we can use it */
	u.u_data_start = (caddr_t) lowest_w_addr;
	u.u_dsize = (size_t)(btoc(highest_addr - lowest_w_addr + 1));
		
		/* ******** */
/*		uprintf("u t start = %lx, u t size = %ld (0x%lx)\n", (long)
			(u.u_text_start), (long)(u.u_tsize),(long)(u.u_tsize));
		uprintf("u d start = %lx, u d size = %ld (0x%lx)\n", (long)
			(u.u_data_start), (long)(u.u_dsize),(long)(u.u_dsize));
*/

        /* Free memory allocated for loading (not args). */

        inode_pager_release(pager);
#ifdef	OSF1_SERVER
	ux_server_thread_unblocking();
#endif	OSF1_SERVER
	kfree(lc_bufferp, (long)(mhp->moh_sizeofcmds));

	/*
	 *	Create the stack.  (Deallocate the old one and create a 
	 *	new one).
	 */

	size = round_page(unix_stack_size);
        addr = trunc_page(VM_MAX_ADDRESS - size);
	U_HANDY_LOCK();
	u.u_stack_start = (caddr_t) addr;
	u.u_stack_end = u.u_stack_start + size;
	u.u_stack_grows_up = FALSE;
	U_HANDY_UNLOCK();
#ifndef	OSF1_SERVER	/* This has already has been done in vm_exec (JQR) */
	(void) vm_deallocate(my_map, addr, size);
#endif	/* OSF1_SERVER */
	if (vm_allocate(my_map, &addr, size, FALSE) != KERN_SUCCESS) {
		uprintf("%s: Cannot find space for stack.\n", u.u_comm);
		error = EGETXFILE;
                goto out;
	}

	U_HANDY_LOCK();
	u.u_ssize = SSIZE + btoc(round_page(nargc));
	u.u_prof.pr_scale = 0;
	U_HANDY_UNLOCK();

        goto out;

clean_up_more:
        inode_pager_release(pager);
#ifdef	OSF1_SERVER
	ux_server_thread_unblocking();
#endif	OSF1_SERVER
clean_up_less:
	kfree(lc_bufferp, (long)(mhp->moh_sizeofcmds));
out:
	*entryp = entry_tmp;
        return (error);
}
#endif	/* OSF_MACH_O */

/*
 *	BSD_A_OUT program loaders follow.
 */

#if	BSD_A_OUT
#if	defined(vax) || defined(sun) || defined(ibmrt) || defined(i386) || defined(__hp_osf) || defined(i860)

/*
 *	FILE_OFFSET is how much of the file to skip when the page
 *	containing the a.out header is not part of the loaded image.
 *	This is not true on the SUN.
 */
#if	defined(vax) || defined(ibmrt) || defined(__hp_osf)
#define	FILE_OFFSET	LOADER_PAGE_SIZE
#endif

#if	defined(sun) || defined(i386)
#define	FILE_OFFSET	0	/* beware - not LOADER_PAGE_SIZE on SUN */
#endif

#ifdef	OSF1_SERVER
program_loader(vp, ep, pagi, special)
	struct vnode	*vp;
	struct exec	*ep;
	int		pagi;
	boolean_t	special;    /* TRUE means it is the emulator program */
#else	/* OSF1_SERVER */
program_loader(vp, ep, pagi)
	struct vnode_proxy	*vp;
	struct exec	*ep;
	int		pagi;
#endif	/* OSF1_SERVER */
{
	memory_object_t	pager;
#ifdef	OSF1_SERVER
	task_t		my_task;
#else	/* OSF1_SERVER */
	vm_map_t	my_map;
#endif	/* OSF1_SERVER */
	vm_offset_t	addr;
	vm_size_t	size;
	vm_offset_t	text_start;
	vm_offset_t	data_start;
	vm_size_t	loader_text_size;
	vm_size_t	loader_data_size;
	vm_size_t	text_size;
	vm_size_t	data_size;
	vm_size_t	data_bss_size;
	vm_size_t	real_text_size;
	vm_size_t	real_data_size;
	int		error = 0;

#ifdef	OSF1_SERVER
	my_task = u.uu_procp->p_task;
#undef	my_map
#define my_map my_task	/* to avoid changin code */
#else	/* OSF1_SERVER */
	my_map = current_task()->map;
#endif	/* OSF1_SERVER */

	/*
	 *	Make copy of exec header fields that we might want
	 *	to modify.
	 */
	real_text_size = ep->a_text;
	real_data_size = ep->a_data;
#ifdef	i386
	if (real_text_size)
		real_text_size += sizeof (struct exec);
#define	BELIEVE_A_OUT 1
#endif

	/*
	 *	On the VAX, the loader aligns the text and data so
	 *	that we cannot protect all of the text.  Therefore,
	 *	we adjust the text/data sizes so that the text size
	 *	corresponds to the part we can protect and the data
	 *	size includes the text we cannot protect.
	 */
#if	defined(vax)
	if (pagi) {
		size = loader_round_page(real_text_size)-
		    trunc_page(real_text_size);
		real_text_size -= size;
		real_data_size += size;
	}
#endif
#ifdef	BELIEVE_A_OUT
	loader_text_size = real_text_size;
	loader_data_size = real_data_size;
#else
	loader_text_size = loader_round_page(real_text_size);
	loader_data_size = loader_round_page(real_data_size);
#endif

	/*
	 *	Since the data starts immediately on the next loader
	 *	page boundary, we cannot round the text size up to
	 *	the next memory size on the VAX.
	 */
#ifdef	vax
	text_size = loader_text_size;
#endif
#ifdef	__hp_osf
	text_size = loader_text_size;
#endif
#if	defined(sun) || defined(ibmrt) || defined(i386)
	text_size = round_page(real_text_size);
#endif

	data_size = round_page(real_data_size);
	data_bss_size = round_page(real_data_size+ep->a_bss);

#ifdef	OSF1_SERVER
	if (!special) {
#define load_address
#ifdef	load_address
#define A_RELOC	0x01000000
		if (ep->a_magic & A_RELOC) {
			if (!ep->a_trsize) {
				uprintf("strange text relocation\n");
			}
			text_start = ep->a_trsize;
		} else
#endif	/* load_address */
			text_start = USRTEXT;
	} else {
		text_start = EMULATOR_BASE;
		emulator_text_size = ep->a_text;
#if GPROF
		emulator_prof_init();
#endif
	}
#else	/* OSF1_SERVER */
#ifdef	i386
#undef	USRTEXT
#define USRTEXT	0x10000
#endif
	text_start = USRTEXT;
#endif	/* OSF1_SERVER */

	if (pagi == 0 && ep->a_text == 0)
		data_start = text_start;
	else {
#ifdef	vax
		/* data immediately after text */
		data_start = text_size;
#endif
#ifdef	__hp_osf
		/* data immediately after text */
		data_start = text_size;
#endif
#ifdef	sun3
		/* data into next segment boundary after text */
		data_start = ((text_start+text_size+SGOFSET)&~SGOFSET);
#endif
#ifdef	sun4
		if (pagi == 0)
			data_start = ((text_start+text_size+SGOFSET)&~SGOFSET);
		else
			data_start = round_page(text_start + text_size);
#endif
#ifdef	ibmrt
		/* data into fixed data segment */
		data_start = 0x10000000;
#endif
#ifdef	i386
#ifdef	OSF1_SERVER
		data_start = i386_round_page(text_start + text_size);
#ifdef	load_address
		if ((ep->a_magic & A_RELOC) && ep->a_drsize)
			data_start = ep->a_drsize;
#endif 	/* load_address */
#else	/* OSF1_SERVER */
		data_start = ((text_start+text_size+(PAGE_SIZE-1))&~(PAGE_SIZE-1));
#endif	/* OSF1_SERVER */
#endif
	}

#ifdef	sun
	if (pagi && trunc_page(text_start) != text_start) {
		uprintf("text doesn't start on page boundary.\n");
		goto suicide;
	}
#endif
#ifdef	__hp_osf
	if (pagi && trunc_page(text_start) != text_start) {
		uprintf("text doesn't start on page boundary.\n");
		goto suicide;
	}
#endif

	/*
	 *	Remember text and data starting points.
	 */
	U_HANDY_LOCK();
	u.u_text_start = (caddr_t)text_start;
	u.u_data_start = (caddr_t)data_start;
	U_HANDY_UNLOCK();
#ifdef	vax
	if (pagi) u.u_data_start += size; /* adjust for change above */
#endif

	error = 0;

	if (pagi == 0) {
		/*
		 *	Not demand paged.
		 *
		 *	Allocate space for image in task's address space.
		 */
		if (text_size + data_bss_size > 0) {
			addr = trunc_page(data_start);
			size = data_bss_size + data_start - addr;
			if (size > 0 &&
			    vm_allocate(my_map, &addr, size, FALSE)
			    != KERN_SUCCESS) {
				uprintf("Cannot find space for data+bss.\n");
				goto suicide;
			}
			addr = text_start;
			size = trunc_page(text_size);
			if (size > 0 &&
			    vm_allocate(my_map, &addr, size, FALSE)
			    != KERN_SUCCESS) {
				uprintf("Cannot find space for text.\n");
				goto suicide;
			}
		}
#ifdef	OSF1_SERVER
		/*
		 * Allocate temporary space for image
		 * in server's address space
		 */
		size = max(real_text_size, real_data_size);
		if (size > 0 &&
		    vm_allocate(mach_task_self(), &addr, size, TRUE)
		    !=  KERN_SUCCESS) {
			uprintf("Cannot find space for exec'ing.\n");
			goto suicide;
		}
		/*
		 *	Read in the data segment (OMAGIC & NMAGIC).
		 */
		if (real_data_size) {
#ifdef OSF1_ADFS
			error = remote_exec_read(vp, (caddr_t)addr, 
					  (int)real_data_size,
					  (off_t)real_text_size, IO_UNIT,
					  (int *)0);
#else
			error = vn_rdwr(UIO_READ, vp,
				(caddr_t)addr, (int)real_data_size,
				(off_t)real_text_size,
				UIO_SYSSPACE, IO_UNIT, u.u_cred, (int *)0);
#endif
			if (error == 0)
				error = vm_write(my_task, data_start, addr, data_size);
		}
		/*
		 *	Read in text segment if necessary (NMAGIC),
		 *	and read-protect it.
		 */
		if ((error == KERN_SUCCESS) && (real_text_size > 0)) {
#ifdef OSF1_ADFS
			error = remote_exec_read(vp, (caddr_t)addr, 
					  (int)real_text_size,
					  (off_t)0, IO_UNIT, (int *) 0);
#else
			error = vn_rdwr(UIO_READ, vp,
				(caddr_t) addr, (int)real_text_size,
				(off_t)0, UIO_SYSSPACE, 
				IO_UNIT, u.u_cred, (int *) 0);
#endif
			if (error == 0) {
				error = vm_write(my_task, text_start, addr, text_size);
				if (error == KERN_SUCCESS && trunc_page(text_size) > 0) {
					(void) vm_protect(my_task,
							  text_start,
							  trunc_page(text_size),
							  FALSE,
							  VM_PROT_READ|VM_PROT_EXECUTE);
				}
			}
		}
		if (size > 0)
			(void) vm_deallocate(mach_task_self(), addr, size);
#else	/* OSF1_SERVER */
		/*
		 *	Read in the data segment (OMAGIC & NMAGIC).
		 */
		if (real_data_size) {
			error = vn_rdwr(UIO_READ, vp,
				(caddr_t)data_start, (int)real_data_size,
				(off_t)(sizeof(struct exec) + real_text_size),
				UIO_SYSSPACE, IO_UNIT, u.u_cred, (int *)0);
		}
		/*
		 *	Read in text segment if necessary (NMAGIC),
		 *	and read-protect it.
		 */
		if ((error == 0) && (real_text_size > 0)) {
			error = vn_rdwr(UIO_READ, vp,
				(caddr_t) text_start, (int)real_text_size,
				(off_t)sizeof(struct exec), UIO_USERISPACE, 
				IO_UNIT, u.u_cred, (int *) 0);
			if (error == 0 && trunc_page(text_size) > 0) {
				(void) vm_protect(my_map,
					 text_start,
					 trunc_page(text_size),
					 FALSE,
					 VM_PROT_READ|VM_PROT_EXECUTE);
			}
		}
#endif	/* OSF1_SERVER */
	}
	else {
		kern_return_t ret;
		/*
		 *	Allocate a region backed by the exec'ed vnode.
		 */
#ifdef OSF1_ADFS
		pager = (memory_object_t)remote_inode_pager_setup(vp, TRUE,TRUE);
#else
		pager = (memory_object_t)inode_pager_setup(vp, TRUE, TRUE);
#endif
#ifdef  OSF1_SERVER
		ux_server_thread_blocking();    /* May contact vnode pager */
#endif  OSF1_SERVER

		/*
		 *	Map the text segment.
		 */
		addr = text_start;
		if (text_size != 0) {
			if ((ret=vm_map(my_map, &addr, text_size, 0, FALSE,
			    pager, (vm_offset_t) FILE_OFFSET, TRUE,
			    VM_PROT_READ|VM_PROT_EXECUTE, VM_PROT_ALL, VM_INHERIT_COPY))
			    != KERN_SUCCESS) {
				printf("Cannot map text into user address space %x\n",ret);
				inode_pager_release(pager);
#ifdef	OSF1_SERVER
				ux_server_thread_unblocking();
#endif	OSF1_SERVER
				goto suicide;
			}
		}

		/*
		 *	Now map in the data segment.
		 */
		addr = data_start;
		if (data_size != 0) {
			if (vm_map(my_map, &addr, data_size, 0, FALSE,
				pager, (vm_offset_t) loader_text_size+FILE_OFFSET,
				TRUE, VM_PROT_ALL, VM_PROT_ALL, VM_INHERIT_COPY)
			     != KERN_SUCCESS) {
				inode_pager_release(pager);
#ifdef	OSF1_SERVER
				ux_server_thread_unblocking();
#endif	OSF1_SERVER
				uprintf("Cannot map data into user space.\n");
				goto suicide;
			}
			addr += data_size;
		}

		inode_pager_release(pager);

		/*
		 *	Allocate the remainder of the BSS segment.
		 */
		size = data_bss_size - data_size;
		if (size != 0 &&
		    vm_allocate(my_map, &addr, size, FALSE)!= KERN_SUCCESS) {
		    	uprintf("Cannot allocate BSS in user address space\n");
#ifdef	OSF1_SERVER
			ux_server_thread_unblocking();
#endif	OSF1_SERVER
			goto suicide;
		}

		/*
		 *	If the data segment does not end on a VM page
		 *	boundary, we have to clear the remainder of the VM
		 *	page it ends on so that the bss segment will
		 *	(correctly) be zero.
		 *	The loader has already guaranteed that the (text+data)
		 *	segment ends on a loader_page boundary.
		 */
		addr = data_start + loader_data_size;
		size = data_size - loader_data_size;
		if (size > 0) {
#ifdef	OSF1_SERVER
			pointer_t	local_addr;
			vm_size_t	ret_size;
			vm_offset_t	addr_page = trunc_page(addr);
			kern_return_t	rt;

			rt = vm_read(my_task, addr_page, round_page(size),
				     &local_addr, &ret_size);
			if (rt == KERN_SUCCESS) {
				bzero((char *)local_addr + (addr - addr_page), size);
				rt = vm_write(my_task, addr_page, local_addr, ret_size);
				(void) vm_deallocate(mach_task_self(), local_addr, ret_size);
			}
			if (rt != KERN_SUCCESS) {
				uprintf("Cannot zero partial data page\n");
				ux_server_thread_unblocking();
				goto suicide;
			}
#else	/* OSF1_SERVER */
			if (copyout(vm_kern_zero_page, (caddr_t)addr, size)) {
				uprintf("Cannot zero partial data page\n");
				goto suicide;
			}
#endif	/* OSF1_SERVER */
		}
	}

	/*
	 *	Protect the region from 0 to the start of text.
	 */
#if	defined(sun) || defined(i386) || defined(i860)
	addr = 0;
#ifdef  load_address
	if (text_start && !special)
#endif  /* load_address */
	{
		if (vm_map(my_map, &addr, text_start, 0, FALSE,
			   MEMORY_OBJECT_NULL, (vm_offset_t) 0, FALSE,
			   page_0_protection, page_0_protection | VM_PROT_READ,
#if	REMOTE_PROC | TNC
			   /* vm_inherit_share nyi across nodes */
			   VM_INHERIT_COPY)
#else
		           VM_INHERIT_SHARE)
#endif
			!= KERN_SUCCESS) {
		        uprintf("Cannot reserve user page 0.\n");
			ux_server_thread_unblocking();
			goto suicide;
		}
        }
#ifdef  OSF1_SERVER
	ux_server_thread_unblocking();
#endif  /* OSF1_SERVER */
#endif /* defined(sun) || defined(i386) || defined(i860) */

#define unix_stack_size	(u.u_rlimit[RLIMIT_STACK].rlim_cur)
	/*
	 *	Create the stack.
	 *	(Deallocate the old one and create a new one).
	 */
	size = round_page(unix_stack_size);
	addr = trunc_page(VM_MAX_ADDRESS - size);
	U_HANDY_LOCK();
	u.u_stack_start = (caddr_t) addr;
	u.u_stack_end = (caddr_t) addr + size;
	u.u_stack_grows_up = FALSE;
	U_HANDY_UNLOCK();
#ifndef	OSF1_SERVER	/* This has already has been done in vm_exec (JQR) */
	(void) vm_deallocate(my_map, addr, size);
#endif	/* OSF1_SERVER */
	if (vm_allocate(my_map, &addr, size, FALSE) != KERN_SUCCESS) {
		uprintf("Cannot find space for stack.\n");
		goto suicide;
	}

	return (error);

suicide:
	return (EGETXFILE);
}
#endif	/* vax || sun || ibmrt || i386 || __hp_osf */

#ifdef	balance
/*
 * balance_getxfile()
 *	Loader for Sequent Balance (ns32000) object files.
 *
 * Code derived from VAX version in getxfile().
 *
 * Magic number ZMAGIC has already been converted to ZMAGIC.  No XMAGIC yet.
 * Assumes "pagi".
 *
 * Balance a.out's assume the code is loaded at 0x800, and text/data are
 * rounded to 2k boundaries.  The header is loaded at 0x800 as part of the
 * text, to avoid wasting space.  0 -> 0x7ff are filled with read-only
 * zero pages (ZMAGIC).  When/if there is XMAGIC, 0->0x7ff needs to
 * be totally invalid.
 *
 * ep->a_text includes the low 2k of address space; the file doesn't, however.
 */

#define LOADER_LOWBYTES	LOADER_PAGE_SIZE	/* start of address space not */
						/* backed by file */

program_loader(vp, ep, pagi)
	struct vnode	*vp;
	struct exec	*ep;
	int		pagi;
{
	register vm_map_t	my_map;
	register kern_return_t	ret;
	register vm_size_t	copy_size;
	register vm_offset_t	copy_end;
	register vm_offset_t	data_end;
	vm_size_t	bss_size;
	memory_object_t	pager;
	vm_offset_t	addr;
	vm_offset_t	low_delta;
	long		size;
	int		error = 0;

#ifdef	lint
	pagi++;
#endif

	/*
	 * Need to know how much larger MACH page is than LOADER_LOWBYTES.
	 */

	if (PAGE_SIZE > LOADER_LOWBYTES)
		low_delta = PAGE_SIZE - LOADER_LOWBYTES;
	else
		low_delta = 0;

	my_map = current_task()->map;

	/*
	 *	Remember text and data starting points
	 */
	U_HANDY_LOCK();
	u.u_text_start = USRTEXT;
	u.u_data_start = (caddr_t) loader_round_page(ep->a_text);
	U_HANDY_UNLOCK();

	error = 0;

	/*
	 * Allocate a region backed by the exec'ed vnode.
	 *
	 * copy_size is set to that part of the file that will be page-aligned
	 * in the addresss space (ie, after LOADER_LOWBYTES).  Thus, if
	 * LOADER_LOWBYTES < PAGE_SIZE, the beginning of the file is not
	 * part of the "copy" map.
	 */

	copy_size = round_page(ep->a_text + ep->a_data - PAGE_SIZE);
	if (ep->a_text+ep->a_data > PAGE_SIZE) {
		pager = (memory_object_t)inode_pager_setup(vp, TRUE, TRUE);
		addr = VM_MIN_ADDRESS + round_page(LOADER_LOWBYTES),
		ux_server_thread_blocking();	/* May contact vnode pager */
		ret = vm_map(my_map, &addr, copy_size, 0, FALSE,
			     pager, low_delta, TRUE,
			     VM_PROT_ALL, VM_PROT_ALL, VM_INHERIT_COPY);
		ux_server_thread_unblocking();

		inode_pager_release(pager);

		if (ret != KERN_SUCCESS) {
			uprintf("Unable to map text/data.\n");
			goto suicide;
		}
	}

	/*
	 *	Allocate the blank area preceding the text
	 */

	addr = VM_MIN_ADDRESS;
	if (vm_allocate(my_map, &addr, round_page(LOADER_LOWBYTES), FALSE)
	    != KERN_SUCCESS) {
	    	uprintf("Cannot allocate low bytes region\n");
		goto suicide;
	}

	/*
	 * If the loader page-size < PAGE_SIZE, need to read the
	 * first part of the file into place.  Do this before write-protext
	 * the text, since we must write on it.
	 */

	int	resid;
	if (low_delta) {
		ret = vn_rdwr(UIO_READ, vp,
				(caddr_t) LOADER_LOWBYTES, (int) low_delta,
				(off_t) 0, UIO_USERISPACE,
			        IO_UNIT, u.u_cred, &resid);
		if (ret != KERN_SUCCESS) {
			uprintf("Could not read first page of text.\n");
			goto suicide;
		}
	}

	/*
	 * Read-protect just the text region.  Do this before we zero
	 * the bss area, so that we have only one copy of the text.
	 */

	(void) vm_protect(my_map,
		 VM_MIN_ADDRESS,
		 trunc_page(ep->a_text),
		 FALSE,
		 VM_PROT_READ|VM_PROT_EXECUTE);

	/*
	 * If the data segment does not end on a VM page boundary,
	 * we have to clear the remainder of the VM page it ends
	 * on so that the bss segment will (correctly) be zero.
	 * The loader has already guaranteed that the (text+data)
	 * segment ends on a loader_page boundary.
	 */

	data_end = VM_MIN_ADDRESS + loader_round_page(ep->a_text + ep->a_data);
	copy_end = VM_MIN_ADDRESS + round_page(LOADER_LOWBYTES) + copy_size;
	if (copy_end > data_end) {
		if(copy_end-data_end > PAGE_SIZE) {
			uprintf("Cannot clear front of bss segment.\n");
			goto suicide;
		}
		if (copyout(vm_kern_zero_page, (caddr_t)data_end, copy_end - data_end)) {
			uprintf("Cannot zero partial data page\n");
			goto suicide;
		}
	}

	/*
	 *	Allocate the BSS region
	 */

	bss_size = round_page(ep->a_text + ep->a_data + ep->a_bss) - copy_end;
	addr = copy_end;
	if (bss_size != 0) {
		if (vm_allocate(my_map, &addr, bss_size, FALSE) != KERN_SUCCESS) {
			uprintf("Cannot allocate BSS region\n");
			goto suicide;
		}
	}

	/*
	 * Create the stack.  (Deallocate the old one and create a new one).
	 *
	 * Is it really necessary to deallocate the old stack?  The
	 * vm_map_remove() done above should have deleted the entire
	 * address space.  This one might make some data/bss dissappear,
	 * though.
	 */

	size = round_page(unix_stack_size);
	U_HANDY_LOCK();
	u.u_stack_start = (caddr_t) (addr = trunc_page(VM_MAX_ADDRESS - size));
	u.u_stack_end = u.u_stack_start + size;
	u.u_stack_grows_up = FALSE;
	U_HANDY_UNLOCK();
	(void) vm_deallocate(my_map, addr, size);
	if (vm_allocate(my_map, &addr, size, FALSE) != KERN_SUCCESS) {
		uprintf("Cannot create stack.\n");
		goto suicide;
	}
	return (error);

suicide:
	return (EGETXFILE);
}
#endif	/* balance */

#endif	/* BSD_A_OUT */

#include <sys/reboot.h>


/*
 * For loading the emulator: copies argv and env out to the user program:
 */
void
copy_out_args_and_stack(cp, nc, na, ne, indir, new_arg_addr)
	register char *	cp;	/* argument and environment strings */
	register int	nc;	/* total size of strings */
	register int	na;	/* number of arguments + number of env. */
	int		ne;	/* number of environment entries */
	char **		indir;	/* if not NULL, pointer to script args */
	vm_offset_t	*new_arg_addr;
				/* argument address in user code (OUT) */
{
	register char **k_ap;	/* kernel arglist address */
	register char *	u_cp;	/* user argument string address */
	register char *	k_cp;	/* kernel argument string address */
	vm_offset_t	u_arg_start;
				/* user start of argument list block */
	vm_offset_t	k_arg_start;
				/* kernel start of argument list block */
	vm_size_t	arg_size;
				/* size of argument list block */
	vm_offset_t	u_arg_page_start;
				/* user start of args, page-aligned */
	vm_size_t	arg_page_size;
				/* page_aligned size of args */
	vm_offset_t	k_arg_page_start;
				/* kernel start of args, page-aligned */

	/*
	 * Ask machine-dependent code for argument list address
	 */
	set_arg_addr(na*NBPW + 3*NBPW + nc + NBPW,
		     &u_arg_start,
		     &arg_size);

	/*
	 * Round to page boundaries, and allocate kernel copy.
	 */
	u_arg_page_start = trunc_page(u_arg_start);
	arg_page_size = (vm_size_t)(round_page(u_arg_start + arg_size)
					- u_arg_page_start);

	(void) vm_allocate(mach_task_self(),
			   &k_arg_page_start,
			   (vm_size_t)arg_page_size,
			   TRUE);

	k_arg_start = k_arg_page_start + (u_arg_start - u_arg_page_start);

	k_ap = (char **)k_arg_start;
	u_cp = (char *)u_arg_start + na*NBPW + 3*NBPW;
	k_cp = (char *)k_arg_start + na*NBPW + 3*NBPW;

	*k_ap++ = (char *)(na - ne);	/* set number of arguments */

	for (;;) {
	    int	len;

	    if (na == ne)
		*k_ap++ = 0;

	    if (--na < 0)
		break;

	    *k_ap++ = u_cp;
	    if (indir && *indir) {
		(void) copystr(*indir++, k_cp, (unsigned)nc, &len);
	    }
	    else {
		(void) copystr(cp, k_cp, (unsigned)nc, &len);
		cp += len;
	    }
	    u_cp += len;
	    k_cp += len;
	    nc -= len;
	}
	*k_ap = 0;

	/*
	 * Write the argument list out to user space.
	 */
	(void) vm_write(u.u_procp->p_task,
			u_arg_page_start,
			(pointer_t)k_arg_page_start,
			arg_page_size);
	(void) vm_deallocate(mach_task_self(),
			     k_arg_page_start,
			     arg_page_size);

	/*
	 * Return the arg-list address.
	 */
	*new_arg_addr = u_arg_start;
}


/*
 * Load the system call emulator, and tell it to exec() the first
 * program.
 */
#include <sys/reboot.h>

char		*emulator_name = "/mach_servers/emulator";
#if SEC_BASE
char		*init_program_name = "/tcb/bin/init";
#else
char		*init_program_name = "/mach_servers/mach_init";
#endif

char		default_arg_1[] = "-sa\0";
char		*(default_program_args[]) = {default_arg_1, 0};

				/* NOTE: Following may be changed by
				 *       starting server with excess args:
				 */
char		**init_program_args = default_program_args;

struct execa	init_exec_args;
int		init_attempts = 0;

load_emulator(p)
	struct proc *p;
{
	int		error = 0;
	vm_offset_t	new_arg_addr;

	struct execr	retval;		/* Following needed to call execve: */
	int		entry[4];
	unsigned int	entry_count;
	char		cfname[256];
	char		cfarg[256];
	struct nameidata *ndp = &u.u_nd;
	extern mach_port_t root_vnode_port;

        struct proc *save_proc = u.u_procp;

	unix_master();

        retval.cfname = cfname;
        retval.cfarg = cfarg;
        retval.entry = entry;
        retval.entry_count = &entry_count;

#ifndef	mips
	default_arg_1[1] = (boothowto & RB_SINGLE) ? 's' : 'x';
	default_arg_1[2] = (boothowto & RB_ASKNAME) ? 'a' : 'x';
#endif

        /*
         * Temporarily become the process and the calling thread.
         */
        u.u_procp = p;

        /*
         * Prepare for loading the emulator.
         */
        emul_init_process(p);

#if	MAP_UAREA
        /*
         * Remap shared area:
         */
	if ((error = mapin_user(p)) != KERN_SUCCESS) {
		printf("load_emulator: mapin_user %d\n", error);
		return error;
	}
#endif	MAP_UAREA

	do {
		if (init_attempts == 2)
			panic("Can't load emulator (%s)", emulator_name);
		init_attempts++;

		if (error) {
			printf("Load of %s failed, errno %d\n",
					emulator_name, error);
			error = 0;
			boothowto |= RB_INITNAME;
		}

		/*
		 *	Set up argument block for fake call to execve.
		 */

		init_exec_args.fname = emulator_name;
		init_exec_args.argp = 0;   /* Args are set after exec, below */
		init_exec_args.envp = 0;

		ASSERT(u.u_cred != NOCRED);
#if	SER_COMPAT
		unix_release();	/* in case pager is not parallelized */
#endif	SER_COMPAT
		ndp->ni_cdirport = ndp->ni_rdirport = root_vnode_port;
		error = execve_prim(u.u_procp, (void *) &init_exec_args,
				    &retval, TRUE);
#if	SER_COMPAT
		unix_master();
#endif	SER_COMPAT
        } while (error);

        /*
         * Copy out arglist
         */
        {
	    char	  arg_buf[256];
            register char *cp;
            register int  len;

            cp = arg_buf;
            len = strlen(init_program_name) + 1;
            bcopy(init_program_name, cp, len);
            cp += len;

            len = strlen(init_program_args[0]) + 1;
            bcopy(init_program_args[0], cp, len);
            cp += len;

            copy_out_args_and_stack(arg_buf,
                                    cp - arg_buf,
                                    2,
                                    0,
                                    (char **)0,
                                    &new_arg_addr);
        }


	/*
	 * Set initial user registers.  Must do all the stuff that
	 * exec in the emulator normally does.
	 */
		set_emulator_state(entry, entry_count, new_arg_addr);

        /*
         * Restore our process ID.
         */
        u.u_procp = save_proc;

        unix_release();

	return error;
}

