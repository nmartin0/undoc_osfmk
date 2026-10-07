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
 * HISTORY
 * $Log: init_main.c,v $
 * Revision 1.30  1995/02/28  00:50:35  jlitvin
 * We only need to create one credentials zone.  This was wasting about a
 * page of memory per node.
 *
 *  Reviewer: yazz
 *  Risk: low
 *  Benefit or PTS #: 12540
 *  Testing: VSX
 *  Module(s): server/bsd/init_main.c
 *
 * Revision 1.29  1995/02/10  23:31:21  stans
 *  'lint' picking with typedefs for a clean compile.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing:WW50 sats
 *
 * Revision 1.28  1995/02/01  21:25:48  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.27  1994/12/29  16:11:02  jlitvin
 * In the checkin for PTS #10356, part of the instrumentation checkin for
 * PTS #9421 was removed, but not enough so that the assertful servers
 * would link.
 *
 *  Reviewer: nina@locus.com
 *  Risk: low
 *  Benefit or PTS #: 12031
 *  Testing: DEBUG servers build
 *  Module(s): server/bsd/init_main.c
 *
 * Revision 1.26  1994/11/18  20:26:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.25  1994/10/25  22:48:48  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: High -- many lines changed in many files
 *  Benefit or PTS #: 9853
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/bsd/init_main.c
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_fork.c
 * 	server/bsd/kern_prot.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/sys/user.h
 * 	server/tnc/chkpnt_pproc.c
 * 	server/tnc/rvp_subr.c
 * 	server/tnc/tnc_svipc.c
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/syscall_subr.c
 * Side-thread changes.  Init new p_side_ref_count.  Renamed p_sigref to the
 * more general p_exit_hold_count.  Initted u.u_procp at the correct time to
 * respect more thorough assertions in several thread setup/teardown routines.
 *
 * Revision 1.24  1994/09/20  18:24:23  yazz
 *  Author: Mike Leibensperger
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: Instrumentation for #9421
 *  Testing: Sched EAT, misc networking testing also
 *  Module(s): server/uxkern/const_region.h, server/bsd/init_main.c,
 * 	server/bsd/uipc_proto.c, server/netinet/in_proto.c,
 * 	server/tnc/dvp_init.c
 *
 * Implement a mechanism in assertful servers whereby areas of the data
 * segment known to be constant can be vm_protect()'d against unintended
 * overwriting.  Corrupted jump tables are known to cause #9421.
 *
 * Revision 1.23  1994/08/31  22:46:25  mtm
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
 * Revision 1.21.2.1  1994/08/10  18:30:43  rlg
 * The queue limit for the server's root vnode port was increased to
 * MACH_PORT_QLIMIT_MAX.  This change, coupled with the changes to the
 * emulator for the reply port, work around a deadlock problem with the
 * new NORMA implementation.
 *
 * Committing in R1.3 WW33 to fix PTS #10409
 *
 * Modified Files:   server/bsd/init_main.c
 *                   emulator/emul_stack.h
 *                   emulator/emul_stack_alloc.c
 *                   emulator/pfs2_user_side.c
 *
 * Revision 1.21  1994/06/01  20:33:58  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): server/sys/errno.h, server/sys/version.h server/nx/nx.c
 * 	    server/nx/nx.defs server/nx/nx_svr.defs server/nx/nx_types.defs,
 * 	    server/nx/nx_types.h, server/nx/nx_create_attr.c (new)
 * 	    emulator/i860/emul_machdep.c
 *  Related: libnx, allocator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.20  1994/01/14  20:38:48  cfj
 * Merge from R1_2.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.19  1994/01/13  21:07:04  cfj
 * Merge R1_2 bug fix.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.18  1994/01/13  17:54:32  jlitvin
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
 * Revision 1.17.2.2  1994/01/14  20:32:11  cfj
 * Use ROOT_FS_NODE instead of BOOT_FIRST_NODE as the node number to look up
 * the bootmesh "sync" port.
 *
 *  Reviewer:nandy, jlitvin
 *  Risk:L
 *  Benefit or PTS #:(PTS # 7805) Increase productivity by getting DEBUG_NODE working.
 *  Testing:Boot the DEBUG_NODE
 *  Module(s):server/bsd/init_main.c
 *
 * Revision 1.17.2.1  1994/01/13  21:03:27  cfj
 * Cause the svr_exception_thread to be created even in the
 * non-MAPPED_FILE case.
 *
 *  Reviewer:jlitvin
 *  Risk:L
 *  Benefit or PTS #:7782.  When a server gets an exception on a compute node,
 * 		  a dead body will be left to examine.
 *  Testing:
 *  Module(s):server/bsd/init_main.c
 * 	   server/uxkern/ux_exception.c
 *
 * Revision 1.17  1993/10/11  17:03:51  cfj
 * Remove extern declaration of root_fs_node in nx_boot_init() so that the module will compile with 4.5 PGI compilers.
 *
 * Revision 1.16  1993/10/05  00:35:55  jlitvin
 * Fix for PTS #6083.  The server and init programs are created by hand.
 * Therefore, they need their u_area start date hand-crafted, also.
 *
 * Revision 1.15  1993/08/18  16:37:56  cfj
 * Fix for PTS bug #6098.  Turn on selected code which previously was turned off if FULLSERVER
 * was not defined to allow pipes to work with the lite server in compute paritions.
 *
 * Revision 1.14  1993/07/19  22:58:39  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.13  1993/07/14  17:46:42  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:45:19  cfj
 * Adding new code from vendor
 *
 * Revision 1.12  1993/06/01  16:00:16  stefan
 * Renamed sll/sll.h to sll/sll_types.h.
 *
 * Revision 1.11  1993/05/20  17:49:21  cfj
 * Add #include <pty.h>
 *
 * Revision 1.10  1993/05/18  18:49:38  cfj
 * Put #if NPTY > 0 around call to pty_initialization.
 *
 * Revision 1.9  1993/05/13  09:13:33  stefan
 * Fixed merge problem where nx_boot_init() was called twice.
 * Integrated static load leveling support.
 *
 * Revision 1.8  1993/05/11  15:00:59  cfj
 * Add the check for Paragon vs ipsc860 for the utsname.
 *
 * Revision 1.7  1993/05/06  19:01:54  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:23:14  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.23  93/07/13  15:55:54  slively
 * Added the #if's required for LITE server builds.
 * Calls to netinit and devsw_init are allowed in a LITE server as well as
 * in a FULLSERVER - The call to devsw_init could use some further
 * investigation to determine if it should be taken out of a LITE server.
 *      Revision 2.29  93/07/02  15:35:09  rabii
 *      Put in include pty.h to get NPTY right (rabii)
 *
 * Revision 2.22  93/06/25  11:18:00  slively
 * Back out the LITE server changes.
 * #if FILESERVER stuff mostly.
 * 
 * Revision 2.21  93/06/22  19:53:24  slively
 * Lite server changes, #if FILESERVER.
 * 
 * Revision 2.20  93/06/09  17:14:46  yazz
 * [ Bug #0283 ] Do not call pty_initialization() in bsmall configuration.
 * 
 * Revision 2.19  93/06/02  09:48:27  yazz
 * For Sys V IPC under TNC initialize svipc mechanism if this is the svipc node.
 * 
 * 	Revision 2.25  93/04/08  11:19:11  loverso
 * 		Print size of server after initialization (cfj@intel)
 *
 * 	Revision 2.24  93/03/24  15:37:26  loverso
 * 		Don't explicitly guess what networking to call; let netinit() have
 * 		knowledge.  We give it a boolean indicating if we are a "full" node
 * 		or not.  [Fixes uninitialized domain lock].  (loverso)
 *
 * 	Revision 2.23  93/02/26  11:12:22  rabii
 * 		Added nx_boot_init routine (i860 only)
 *
 * 		Moved advertisment of FILESERVER to after the appropriate
 * 		initializations (rabii)
 *
 * 	Revision 2.21  93/01/14  13:11:12  loverso
 * 		Only call procman_setup on root_fs_node (plus cleanup).
 *
 * Revision 2.17  93/02/26  19:51:58  yazz
 * Guard with #ifdef i860 the Intel NX bootmesh code.  (This code is
 * harmless on non-Paragon machines, but for the space it takes up,
 * but there's no reason to compile it into non i860 architectures.)
 * 
 * Revision 2.16  93/02/22  13:36:14  yazz
 * Add nx_boot_init() code from Intel, for Paragon.  This code applies only
 * to Paragon systems, but it generates no problems on non-Paragon systems.
 * (OSF is likely to #ifdef this code, if only to save space on other
 * architectures.)
 * 
 * Revision 2.15  92/12/29  12:40:44  chrisp
 * Remove cinit() - this is now in tty_subr.c. Change in support of dynamic
 * 	allocation of cblock lists.
 * 
 * Revision 2.14  92/12/01  10:38:32  chrisp
 * For TNC, don't register nodes for shutdown.
 * 
 * Revision 2.13  92/11/23  15:54:07  klh
 * 	Revision 2.19  92/11/17  19:47:01  loverso
 * 		Create a thread to catch server exceptions.  (mmp)
 * 
 * 	Revision 2.18  92/11/11  10:38:14  rabii
 * 		Remove sanity check for root_device_node which is now done is devstat
 * 
 * 	Revision 2.17  92/11/09  15:37:46  rabii
 * 		[92/10/06  12:08:20  roman]
 * 		Fix RCS comments.
 * 
 * 	Revision 2.16  92/11/03  11:54:09  mmp
 * 		For shutdown: initialize node existence list, register with
 * 		root_fs_node.  (mmp)
 * 
 * Revision 2.12  92/10/06  12:08:20  roman
 * Fix RCS comments.
 * 
 * Revision 2.11  92/10/05  13:37:08  klh
 * 	Revision 2.15  92/10/05  12:06:07  rabii
 * 		Changed some printfs to printf_debug (roy)
 * 
 * 	Revision 2.14  92/09/25  12:36:45  rabii
 * 		ifdefed out sanity check for root node until next release 
 *		(rabii)
 * 
 * 	Revision 2.13  92/09/24  17:07:37  rabii
 * 		Removed extra printf (rabii)
 * 
 * 	Revision 2.12  92/09/24  16:49:30  rabii
 * 		Added sanity check to make sure root_device_node passed in from 
 *		boot variables matches what is in the inode. (rabii)
 * 
 * 	Revision 2.11  92/09/11  09:26:31  rabii
 * 		Added code to only register for rproc if accept_procs is TRUE 
 *		(rabii)
 * 
 * 	Revision 2.10  92/08/13  19:17:06  rabii
 * 		[92/08/06  16:37:22  roman]
 * 		Make changes to allow builds without REMOTE_PROC to work.
 * 
 * Revision 2.10  92/08/06  16:37:22  roman
 * Make changes to allow builds without REMOTE_PROC to work.
 * 
 * Revision 2.9  92/08/06  13:30:25  klh
 * 	Revision 2.9  92/07/29  08:58:43  rabii
 * 		Fixed RCS log
 * 
 * 	Revision 2.8  92/07/28  20:02:19  rabii
 * 		Added thread to sync. pager initialization as well as rproc 
 * 		registeration (rabii)
 * 
 * 	Revision 2.7  92/07/14  14:58:02  rabii
 * 		Be sure to enable uipc on the fs nodes, so that FIFOs work. 
 *		(loverso)
 * 
 * Revision 2.8  92/07/10  08:44:04  chrisp
 * For TNC, intialize streams only on root fs node.
 * 
 * Revision 2.7  92/07/07  15:06:42  roman
 * Add additional print_debug() statements during initialization.
 * 
 * Revision 2.6  92/07/07  13:05:03  klh
 * 	Revision 2.6  92/06/30  22:45:30  loverso
 * 		Pass in ruid and rgid to credentials_allocate (rabii)
 * 
 * 		Turn "on" vnode_pager before emul_shared_init (rabii)
 * 
 * Revision 2.5  92/06/09  16:39:32  pjg
 * 	Call mbinit for all fileservers.
 * 
 * Revision 2.4  92/06/08  18:18:26  pjg
 * 	Do not start network threads in fileservers other than the root (pjg).
 * 
 * 	Moved getting the default pager to before turning on the shared part of
 * 	emulator for MAP_UAREA so the default pager gets set right (rabii)
 * 
 * Revision 2.3  92/05/31  18:58:01  loverso
 * 	Add include of mapped_files.h.
 * 	[92/05/29            roy]
 * 
 * Revision 2.2  92/05/24  19:52:02  pjg
 * 	Adapted to OSF/1 AD.
 * 
 * Revision 3.6  92/04/03  14:13:19  condict
 * Include nfs.h so nfs initialization gets turned on.
 * Add call to sched_init (part of the sched_prim.c rewrite).
 * Execute OSF1_main with master lock held, in uniproc configuration.
 * 
 * Revision 3.5  92/03/31  15:39:53  emcmanus
 * Catch exceptions for Unix tasks, but not for the server itself.
 * 
 * Revision 3.4  92/03/24  21:02:51  barbou
 * Fix for bug #124: initialize the exception port before creating the
 * init task.
 * 
 * Revision 3.3  92/03/23  18:01:48  condict
 * Rewrote the list of includes to be more like the integrated kernel.  Also,
 * rewrote some code and moved some to uxkern/server_init.c for same reason.
 * 
 * Revision 3.2  92/03/23  10:28:34  sp
 * Initialize the vm_mmap subsystem and set up the vm_mmap for proc[0]
 * for newproc to copy.
 * 
 * Revision 3.1  92/03/20  15:08:03  jose
 * Removed async_thread.
 * 
 * Revision 3.0  92/03/13  15:17:57  condict
 * Adapted from the OSF/1 integrated kernel, to replace bsd/mach_init.c.
 * (See also uxkern/server_init.c)

 * Revision 2.30  92/05/24  14:17:09  pjg
 * 	Call ux_handler_init (formerly ux_handler_setup) only once.
 * 	Renamed root_node to root_device_node.
 * 
 * Revision 2.29  92/05/18  12:29:00  roy
 * 	Revision 2.24.1.1  92/04/22  09:54:24  roy
 * 	Call mf_init() for MAPPED_FILES.
 * 	Implemented the ability to import/export the vnode_pager to
 * 	be used by remote nodes as the default pager (rabii)
 * 
 * Revision 2.28  92/05/01  16:02:24  rabii
 * 	Removed extra task_suspend and fixed some NULL refs.
 * 
 * Revision 2.27  92/05/01  15:48:21  rabii
 * 	Fixed string_to_args so that argc is set to the correct value
 * 
 * Revision 2.26  92/05/01  10:21:36  rabii
 * 	added support for find_string to parse node specific config values.
 * 	(durriya)
 * 
 * 	Added virtual socket initialization and NETSERVER and netserver_node
 * 	initialization (bhk)
 * 
 * 	Setup server's exception port for all nodes (including non-root 
 *	fs nodes).  (roman)
 * 
 * 	Implemented functionality to allow passing of command line
 * 	args through boot magic (rabii)
 * 
 * Revision 2.25  92/04/05  16:47:43  pjg
 * 	Changed initialization of vnode proxies (pjg)
 * 
 * 	In setup() save return status from various kernel calls in a variable
 * 	and print it in panic messages (durriya)
 * 
 * 	Remove the dynamic default of root_fs_node and default)pager_node to
 * 	this_node for TNC. (chrisp)
 * 
 * 	Get rid of task parameter from credentials_allocate(). Add task 
 * 	parameter to call to credentials_set_state(). (roman)
 * 
 * 	Add support for a new type (a series of ranges of positive integers) 
 * 	to the find_string() routine.
 * 	Add support for a new boot configuration parameter BOOT_NODE_LIST,
 * 	which has the list of all the nodes in the cluster.
 * 	Remove call to tnc_init().
 * 	For TNC, and as a temporary measure, default root_fs_node and
 * 		default_pager_node to this_node. (roman)
 * 
 * Revision 2.24  92/03/15  14:38:46  roy
 * 	Remove extra print_debug statements (roy).
 * 	Replaced allocate_credentials with new interfaces (rabii)
 * 	Added support for the BOOT_EMULATOR_NAME boot magic variable.
 * 	(rabii for cfj@ssd.intel.com)
 * 	Retry if fsvr_get_root_port returns an error. Fix initialization
 * 	of u.u_cdir and u.u_rdir (pjg).
 * 	Add remote_vnode_pager_node as boot time configuration 
 * 	variable (durriya).
 * 
 * Revision 2.23  92/03/09  18:16:08  durriya
 * 	set server_build string to "". Do not use sys/server_include.h yet 
 * 
 * Revision 2.22  92/03/09  14:26:19  durriya
 * 	Revision 3.47  92/02/27  21:08:15  jose
 * 	Activated the async thread: NOT TAKEN IN OSF/1 AD (pjg).
 * 	Print memory used by buffer cache.
 * 
 * 	Revision 3.46  92/02/25  17:48:07  condict
 * 	Change all calls to cthread_wire to ux_thread_wire, so ux_server_loop
 * 	can correctly compute required number of Mach kernel threads.
 * 
 * 	Revision 3.45  92/02/19  16:22:39  emcmanus
 * 	*** empty log message ***
 * 
 * 	Revision 3.44  92/02/18  18:56:57  jose
 * 	Added hooks for the async_io thread (not yet activated).
 * 
 * 	Revision 3.43  92/01/20  10:45:46  bernadat
 * 	Added call to xprbootstrap().
 * 
 * 	Revision 3.42  92/01/07  23:33:12  condict
 * 	Get rid of mutex_init on a simple lock.  Use simple_lock_init instead.
 * 	Also, fix reference to NFS configuration option.
 * 
 * 	Revision 3.41  92/01/02  15:20:32  david
 * 	allow for NFS deconfiguration
 * 
 * 	Revision 3.40  91/12/20  17:56:29  barbou
 * 	Include and use the new "server_build.h" header file.
 * 
 * 	Revision 3.39  91/12/18  17:15:12  sp
 * 	Only syspend the server (with -h) if we are a second server
 * 
 * Revision 2.21  92/03/01  18:43:20  pjg
 * 	1992/03/01  pjg
 * 	Initialize vnode proxies for the cwd and root directories.
 * 	Don't call newproc for proc 1 in fileservers other than the
 * 	root.
 * 	Print the message "waiting for..." before sleeping.
 * 	Some more ifdef cleanup.
 * 
 * 	1992/03/01  loverso
 * 	Removed call to server_port_table_init.
 * 
 * Revision 2.20  92/02/21  21:33:46  durriya
 * 	fix a bug in retry loop for nameserver port of root fs node
 * 
 * Revision 2.19  92/02/21  16:12:22  durriya
 * 	define root_fs_node for non NORMA_IPC also, so that we can get 
 * 	rid of NORMA_IPC conditionals in other places. Dont allocate 
 * 	fsproc and servermsg structures statically - they are allocated
 * 	in fsproc_init now. Define mach_init_sleep and use it for timeout
 * 	cleaned up race conditions in netname_look_up loops
 * 
 * Revision 2.18  92/02/11  22:15:46  pjg
 * 	Always print a message in the end of server initialization.
 * 
 * 	Fix arg declaration in find_string() (rabii).
 * 
 * 	Fix default setup of root node to work with nd regardless of
 * 	microkernel version. Fixes so that code continues to work for 
 * 	microkernels not supporting host_get_boot_info() (roman@locus).
 * 
 * Revision 2.17  92/01/17  17:24:26  roy
 * 	Clean up and comment NORMA_IPC code.
 * 
 * Revision 2.16  92/01/16  16:21:34  roy
 * 	Add support for remote pager support.  Start local node nameserver
 * 	from system_setup. (durriya)
 * 
 * Revision 2.15  92/01/14  17:43:34  roy
 * 	Remove extraneous norma_ipc ifdefs.
 * 
 * Revision 2.14  92/01/14  11:03:20  roy
 * 	Start credentials service later to prevent race (pjg).
 * 	Changed initialization of vproc0 and vproc1 (roy).
 * 	Added get_boot_config() support (durriya).
 * 	Added remote_proc support (sjs).
 * 
 * Revision 2.13  92/01/05  18:21:39  roy
 * 	1992/01/03  22:13:33  noemi
 * 	Removed bogus setup of uarea credentials in system_setup.  Re-organized
 * 	system_setup so that all fileservice initialization is completed before
 * 	execing init and loading the emualtor.
 * 	1991/12/31  00:00:38  noemi
 * 	CHPC merge with ad7.4
 * 
 * Revision 2.12  92/01/02  18:53:46  roy
 * 	91/12/19  17:45:08  pjg
 * 	Put the hacks to support local root and local console on the second
 * 	file-server under conditionals (local_root and local_console). They
 * 	are disabled by default. To enable them, give the flag -c for local
 * 	console and -r for local root in the command line.
 * 
 * 	1991/11/30  19:36:04  noemi
 * 	Added declaration of rootnode.  Changed parameters in call to bdevvp.
 * 	Added bootstrap code for fileserver.
 * 
 * 	1991/10/14  20:05:49  noemi
 * 	Added a call to server_port_table_init and allocation of servermsg
 * 	structures.  Removed obsolete setting of msh_kind in the message header.
 * 
 * Revision 2.11  91/12/18  18:26:33  roy
 * 	ux_server_thread_max is now ux_server_receive_max.
 * 
 * Revision 2.10  91/12/17  16:08:42  roy
 * 	91/12/13  13:02:58  sp
 * 	Remove CMUCS conditionals.
 * 
 * 	91/12/09  18:49:29  emcmanus
 * 	Removed the debugger_thread stuff because nobody uses it.
 * 	Move the initialisation of server profiling.
 * 
 * 	91/11/07  15:15:07  condict
 * 	Added runtime flag for turning on the wired_threads option.
 * 
 * 	91/10/30  17:42:03  bernadat
 * 	In case of serialized file system with SER_COMPAT option
 * 	release master lock before invoking vnode pager.
 * 
 * 	91/10/29  16:11:15  barbou
 * 	Fix for bug #24: initialize the utsname structure.
 * 	Print PID when booting as a second server.
 * 
 * 	91/10/28  11:09:55  condict
 * 	Remove unused time_p variable.
 * 
 * 	91/10/24  09:04:11  sp
 * 	register the emulator as a mapped region that can't go away
 * 
 * 	91/10/17  18:32:52  barbou
 * 	Deleted include of "cmucs.h" (defunct option).
 * 	A second server now catches signals and enters the debugger.
 * 	LVM initialization.
 * 
 * 	91/10/17  15:44:47  condict
 * 	Create the specified minimum number of server threads on bootstrap, 
 * 	instead of just one.
 * 
 * 	91/10/15  12:16:13  sp
 * 	make the emulator a special keep on exec region
 * 
 * 	91/09/27  11:56:55  emcmanus
 * 	Initialize profiling data and devsw structures.
 * 
 * 	91/09/20  19:15:46  barbou
 * 	Dynamic computation of the number of pages shared with the emulator.
 * 
 * 	91/09/16  15:45:46  condict
 * 	Add missing calls to init functions (msginit, seminit and flckinit).
 * 
 * 	Revision 3.23  91/09/13  12:48:55  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * 	91/09/10  11:57:50  barbou
 * 	Automatic setting of second_server.
 * 	Print the MK banner and check that it is up to date with this server.
 * 	Use uarea_terminate() to decrement the reference count on the 
 * 	credentials.
 * 
 * Revision 2.9  91/12/13  10:07:01  roy
 * 	91/11/19  10:02:45  roy
 * 	Integrate new vm_set_page_size logic.
 * 
 * Revision 2.8  91/12/08  09:58:16  rabii
 * 	Added code to turn on the credentials service
 * 
 * Revision 2.7  91/11/22  14:54:36  rabii
 * 	Locus Merge
 * 	Merged LCC and new OSF versions.
 * 	a) Revision 3.26  91/09/27  11:56:55  emcmanus
 * 	   Initialize profiling data and devsw structures.
 * 
 * 	b)  Revision 3.25  91/09/20  19:15:46  barbou
 * 	    Dynamic computation of the number of pages shared with the emulator.
 * 
 * 	c)  Revision 3.24  91/09/16  15:45:46  condict
 * 	    Add missing calls to init functions (msginit, seminit and flckinit).
 * 
 * 	d)  Revision 3.23  91/09/13  12:48:55  sp
 * 	    include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * 	e)  Revision 3.22  91/09/10  11:57:50  barbou
 * 	    Automatic setting of second_server.
 * 	    Print the MK banner and check that it is up to date with this server
 * 	    Use uarea_terminate() to decrement the reference count on the 
 * 	    credentials. 
 * 	    (mbarnett)
 * 
 * 	Change code to not depend on fact that thate fsproc array immediately
 * 	follow proc array. (roman)
 * 	Allow initialization of the server to either support TNC or not support
 * 	TNC. (roman)
 * 	Add call to initialize TNC. (roman)
 * 	newproc() has 2 correctly has 2 arguments. (chrisp)
 *
 * Revision 2.6  91/10/14  12:08:36  sjs
 * 	Revision 3.26  91/09/27  11:56:55  emcmanus
 * 	Initialize profiling data and devsw structures.
 * 
 * 	Revision 3.25  91/09/20  19:15:46  barbou
 * 	Dynamic computation of the number of pages shared with the emulator.
 * 
 * 	Revision 3.24  91/09/16  15:45:46  condict
 * 	Add missing calls to init functions (msginit, seminit and flckinit).
 * 
 * 	Revision 3.23  91/09/13  12:48:55  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * 	Revision 3.22  91/09/10  11:57:50  barbou
 * 	Automatic setting of second_server.
 * 	Print the MK banner and check that it is up to date with this server.
 * 	Use uarea_terminate() to decrement the reference count on the 
 *	credentials.
 * 
 * Revision 2.5  91/10/04  14:48:51  chrisp
 * Get rid of extraneous $Log. Get rid of reference to msgh_kind field
 * (it is now a sequence number).
 * 
 * Revision 2.4  91/09/16  15:39:40  rabii
 * 	Merge of V2.0 and Locus (locus check-in by roman)
 * 	Initialize the dummy processes used by the file and vnode server
 * 	routines. Remove all process group initialization and put into
 * 	vproc and pvproc structures. Put in initialization of vproc
 * 	and pvproc directories.
 * 
 * Revision 2.2  91/08/31  13:22:46  rabii
 * 	Initial V2.0 Checkin
 * 
 * 
 * Revision 1.18  90/10/31  13:48:27  devrcs
 * 	Replace all references of NZERO with PRIZERO
 * 	[90/10/25  12:44:46  sp]
 * 
 * 	Added missing uarea_lock_init() for the initial task.
 * 	[90/10/21  17:53:54  jeffc]
 * 
 * 	Renamed MMAX_TEST_CONF for MST_DYNAMIC
 * 	[90/10/11  14:08:58  knight]
 * 
 * 	Call softclock_init in main() - see bsd/kern_clock.c for why.
 * 	[90/10/09  18:14:31  tmt]
 * 
 * 	For the 386, initialize the cxenix field in the proc structure
 * 	of init to NULL.
 * 	[90/10/08  13:20:34  swallace]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:54:14  gm]
 * 
 * 	Additional lock debugging:  track lock counts.
 * 	(Currently disabled.)
 * 	[90/09/28  12:36:45  nags]
 * 
 * 	Changed the group management to separate cr_gid from the cr_groups
 * 	 array.
 * 	[90/09/21  11:05:42  collins]
 * 
 * 	Changed devsw_configure to devsw_init
 * 	Moved devsw_init to be called earlier
 * 	[90/09/17  14:52:02  knight]
 * 
 * Revision 1.16  90/09/23  15:42:41  devrcs
 * 	Eliminate init_buf_done anachronism
 * 	[90/09/10  17:25:43  jeffc]
 * 
 * 	Made the number of pages per thread allocated to the vnode_pager
 * 	map a symbolic constant.
 * 	[90/09/11  06:53:16  ers]
 * 
 * Revision 1.15  90/08/24  11:15:18  devrcs
 * 	Removed references to u.u_ap and u.u_arg.
 * 	[90/08/08  14:46:49  gmf]
 * 
 * 	Hooks for dynamic loading of NFS.
 * 	Eliminate old, useless QUOTA code.
 * 	[90/08/18  03:40:02  nags]
 * 
 * Revision 1.14  90/07/27  08:43:28  devrcs
 * 	Change float dk_mspw to long dk_wpms.
 * 	[90/07/18  11:47:40  brezak]
 * 
 * 	Check the return value from thread_create in setup_main()
 * 	and main() and panic if a kernel thread can not be allocated.
 * 	[90/07/19  11:53:30  jvs]
 * 
 * 	New mmax isr scheme (bolinger@encore.com).
 * 
 * 	Condensed Recent History:
 * 	Initialize ISR threads.					bolinger@encore
 * 	Unnecessary log message (recheck-in after merge).	coren@osf.org
 * 	Scale kernel_pageable_map on numcpus; parallel execs.	nags@osf.org
 * 	Panic on failure to create initial task.		coren@osf.org
 * 	Gave rootvp a mount structure and condensed history	nags@osf.org
 * 	Added missing semicolon.				seiden@osf.org
 * 	nags merge						nags@encore.com
 * 	Changes from SecureWare					seiden@osf.org
 * 	Remove pageable flag from zinit argument list.		jvs@osf.org
 * 	Removed defunct network dependencies, clean up history. tmt@osf.org
 * 	Changes for gcc				      brezak and duthie@osf.org
 * 	Changed inode pager names to vnode pager names.		collins@osf.org
 * 	Allocate zone space based on number of inode pagers.	collins@osf.org
 * 	Added device switch initialization			knight@osf.org
 * 	Removed sysv_ipc conditional.				bet@osf.org
 * 	Added global_lock_initialization(); removed RFS code	nags@osf.org
 * 	Added parameter to ndinit() 				gmf@osf.org
 * 	Initialization of STREAMS must be conditional.		nolting@osf.org
 * 	Added msginit() and seminit() to main() (SVIPC)		bet@osf.org
 * 	added flckinit() (initialize POSIX record/file locking) ers@osf.org
 * 	Merged Robert Coren's and Rich Morris's changes		ers@osf.org
 * 	Removed include of <sys/dir.h>.				@osf.org
 * 	Changes for BSD4.4 process structures.			coren@osf.org
 * 	Integrated 4.4BSD file system changes as of 1/5/90	noemi@osf.org
 * 	Moved network init code out of here.			tmt@osf.org
 * 	Fixes for quotas					noemi@osf.org
 * 	moved start of netisr_thread 				gmf@osf.org
 * 	added nfsinit() call; moved from nfs_init()		gmf@osf.org
 * 	Initial Revision from Zero.2.Build with Loader fixes
 * 	Changes for 4.4BSD vnodes, cred structure.		ers@osf.org
 * 
 * 	Condensed relevant ancient history (reverse chronology)
 * 	Mount lock & hostname lock record statistics.	alan@encore.com
 * 	Merged Mach 2.5 & Encore BSD parallelization.	alan@encore.com
 * 	Startup of multiple netisr threads.		boykin@encore.com
 * 	Add action and sched threads 			dlb@cmu.edu
 * 	Add kernel monitor support			dlb@cmu.edu
 * 	Allocations in the kernel_pageable_map		mwyoung@cmu.edu
 * 	Added call to kern_prot_init for identity zone	jsb@cmu.edu
 * 	No floats in Mips kernels.			af@cmu.edu
 * 	Remove handling of default memory manager.	mwyoung@cmu.edu
 * 	Added definitions of rootdev, dumpdev, dumplo	rpd@cmu.edu
 * 	I386: exl change to buffer initialization	rvb@cmu.edu
 * 	Code reorg for inode pager, netisr thread	jsb@cmu.edu
 * 	Use simple_lock_addr when calling thread_sleep.	rpd@cmu.edu
 * 	Use object_copyout instead of port_copyout.	rpd@cmu.edu
 * 	Add task_name()					mwyoung@cmu.edu
 * 	Split up XPR initialization into two parts.	mwyoung@cmu.edu
 * 	Only set printf_cpu_number if NCPU > 1 		rvb@cmu.edu
 * 	Added socket-lock zone initialization.		boykin@encore.com
 * 	Added INIFADDR_LOCK initialization.		boykin@encore.com
 * 	Have init_task turn on printf_cpu_number.	rvb@cmu.edu
 * 	Eliminated use of kern/mach_ipc_defs.h.		rpd@cmu.edu
 * 	Added ROUTE_LOCK initialization.		boykin@encore.com
 * 	Added TIME_LOCK initialization.			boykin@encore.com
 * 	Pty, select, and socket initialization 		alan@encore.com
 * 	Use new memory object types.			mwyoung@encore.com
 * 	Added mbuf allocation threads, hostname lock.	alan@encore.com
 * 	Added MACH_EXCEPTION, device_pager_init call.	mwyoung@encore.com
 * 	Changed includes				mja@cmu.edu
 * 	Init code for parallel BSD filesystem.		alan@encore.com
 * 	[90/06/18  16:58:03  nags]
 * 
 * $EndLog
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
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)init_main.c	7.1 (Berkeley) 6/5/86
 */

#include <fullserver.h>
#include <map_uarea.h>
#include <mapped_files.h>
#include <mach_device.h>
#include <mach_host.h>
#include <mach_km.h>
#include <mach_net.h>
#include <mach_xp.h>
#include <xpr_debug.h>
#include <cpus.h>
#include <mach_emulation.h>
#include <streams.h>
#include <nfs.h>
#include <pty.h>

#include <cputypes.h>
#include <sys/secdefines.h>

#include <sys/unix_defs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/map.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/conf.h>
#include <sys/buf.h>
#include <sys/clist.h>
#include <sys/dk.h>
#include <sys/table.h>
#include <sys/lock_types.h>
#include <sys/kernel.h>
#include <machine/reg.h>
#include <machine/cpu.h>

#include <mach/machine.h>
#include <sys/version.h>
#include <uxkern/vm_param.h>
#include <mach/boolean.h>
#include <kern/sched_prim.h>
#include <kern/zalloc.h>
#include <builtin/inode_pager.h>
#include <mach/memory_object.h>
#include <mach/task_special_ports.h>
#include <builtin/ux_exception.h>
#include <sys/utsname.h>

#ifdef	multimax
#include <mst.h>
#if	NMST
#include <mstopt.h>
#endif
#endif

#ifdef  OSF1_ADFS
#include <uxkern/sthread.h>
#include <norma_ipc.h>
#include <remote_proc.h>
#if NORMA_IPC
#include <mach/norma_special_ports.h>
#endif
#endif

#ifdef SLL
#include <sll/sll_types.h>
#endif /* SLL */

#if MACH_ASSERT
#include <uxkern/const_region.h>
#include <sys/protosw.h>

extern struct protosw unixsw[], inetsw[];
extern struct vproc_ops dvproc_ops_table;
extern char e_unixsw[], e_dvpop[];
#endif

extern mach_port_t	ux_handler_init();
#if	!REMOTE_PROC
any_t			init_default_pager();
#else	/* REMOTE_PROC */
any_t			init_default_pager_rproc();
#endif	/* REMOTE_PROC */

/* Bootstrap debug messages: */
int	print_debug_messages = 0;
#define print_debug(x) \
	if (print_debug_messages) printf x; else 0

long	cp_time[CPUSTATES];

long	tk_nin;
long	tk_nout;

dev_t	rootdev;		/* device of the root */
char	*rootdev_name;		/* name of the root device */
struct vnode *rootvp;		/* vnode of root filesystem */
dev_t	dumpdev;		/* device to take dumps on */
long	dumplo;			/* offset into dumpdev */
int	show_space;
long	hostid;
char	hostname[MAXHOSTNAMELEN];
int	hostnamelen;
char	domainname[MAXDOMNAMELEN];
int	domainnamelen;

struct	timeval boottime;
struct	timeval time;
struct	timezone tz;			/* XXX */
int	hz;
int	phz;				/* alternate clock's frequency */
int	tick;
time_t	lbolt;				/* awoken once a second */

int	cmask = CMASK;
extern int (*mountroot)();
extern any_t	ux_server_loop();

extern mach_port_t	privileged_host_port;
extern mach_port_t	default_pager_port;

extern void	svr_exception_thread();

#ifdef  OSF1_ADFS
extern mach_port_t	nameserver_port;
extern mach_port_t	fileserver_port;
extern mach_port_t	root_ns_port;
extern mach_port_t	root_fs_port;
extern mach_port_t	root_vnode_port;

extern node_t  this_node;
extern node_t  root_fs_node;
extern node_t  root_device_node;

#if     NORMA_IPC
extern int     import_paging;
extern int     export_paging;
extern node_t  pager_node;
extern void     local_node_nameserver();
#if     TNC
extern          node_t  netserver_node;
extern          node_t  svipc_node;
#endif  /* TNC */
#endif  /* NORMA_IPC */
#endif  /* OSF1_ADFS */


/*
 *	Defaults for all processes
 */
int maxuprc = MAXUPRC;		/* default maximum proccesses per user */
#if	UNIX_LOCKS
#include <sys/time.h>
#include <kern/lock.h>
#include <sys/file.h>
#include <sys/select.h>
#endif

/*
 * Initialization code.
 * Called from server main routine as
 * soon as a few initializations
 * have been established.
 * Functions:
 *	clear and free user core
 *	turn on clock
 *	hand craft 0th process
 *	call all initialization routines
 *	fork - process 0 to schedule
 *	     - process 1 execute bootstrap
 *	     - process 2 to page out
 */
task_t		first_task;
thread_t	first_thread;

thread_t setup_main()
/*
 *	first_addr contains the first available physical address
 *	running in virtual memory on the interrupt stack
 *
 */
{
	extern char	version_version[], version_release[];
	static struct ucred first_creds;

	print_debug((
	"--- setup_main() ---------------------------------------------\n"));

	rqinit();
	sched_init();

#if	XPR_DEBUG
	xprbootstrap();
#endif

	startup();

	kern_prot_init();
#if     FULLSERVER
	vnode_pager_bootstrap();
#endif  /* FULLSERVER */

	/*
	 *	initialize static version info in utsname struct
	 */

	bcopy(version_version, utsname.version, sizeof(utsname.version));
        utsname.version[sizeof(utsname.version)-1] = '\0';
        bcopy(version_release, utsname.release, sizeof(utsname.release));
        utsname.release[sizeof(utsname.release)-1] = '\0';

#ifdef __i860__
	{
	char    *machine_version = MACHINE;
	int      cpu_type;
        extern   int _i860_cpu_type();


	cpu_type = _i860_cpu_type();
	if (cpu_type == i860_XR)
	    machine_version = MACHINE_IPSC860;
	else if (cpu_type == i860_XP)
	    machine_version = MACHINE_PARAGON;
	bcopy(machine_version, utsname.machine, sizeof(utsname.machine));
        utsname.machine[sizeof(utsname.machine)-1] = '\0';
        }
#endif /* __i860__ */

	print_debug((
	"--- setup_main(setting up proc 0) ---------------------------\n"));

	/*
	 *	Create proc[0]'s u area.
	 */

	first_task = mach_task_self();

	/*
	 * The first thread and task need some credentials,
	 * appropriate pointers, miscellaneous locks initialized (sigh).
	 * N.B.  The first_creds structure has an arbitrarily high
	 * reference count to prevent it from being released back into
	 * the creds zone, from whence it never came.
	 */
	proc[0].p_utask.uu_procp = &proc[0];
	proc[0].p_rcred = &first_creds;
	first_creds.cr_ref = 100;
	first_creds.cr_uid = 0;
	first_creds.cr_gid = 0;
	first_creds.cr_ngroups = 1;
	first_creds.cr_groups[0] = 0;
	CR_LOCK_INIT(&first_creds);
	PROC_LOCK_INIT(&proc[0]);

	first_thread = mach_thread_self();
	proc[0].p_task = first_task;
	proc[0].p_thread = first_thread;

	/*
	 *	Return to assembly code to start the first process.
	 */

	return(first_thread);
}

/*
 *	Sets the name for the given task.
 */
void task_name(s)
	char		*s;
{
	int		length = strlen(s);

	bcopy(s, u.u_comm,
		length >= sizeof(u.u_comm) ? sizeof(u.u_comm) :
			length + 1);
}

/* To allow these values to be patched, they're globals here */
struct rlimit vm_initial_limit_stack = { DFLSSIZ, MAXSSIZ };
struct rlimit vm_initial_limit_data = { DFLDSIZ, MAXDSIZ };
struct rlimit vm_initial_limit_core = { RLIM_INFINITY, RLIM_INFINITY };

void
OSF1_main(arg)
	any_t	arg;
{
	register int i;
	register struct proc *p;
	int ret;
	struct proc *init_proc;
	extern void	file_table_init();
#if	UNIX_LOCKS
#ifdef	multimax
	extern void	slcintr_thread();
#endif
	extern void	acctwatch_thread();
#endif
	extern struct proc *newproc();
	void		global_lock_initialization();
#ifdef	multimax
	void		mmax_initialization();
#endif
#if 	NORMA_IPC
        int 		max_retry = 75;   /* try 75 times for server contact */
        int 		print_count = 10; /* print a message every 10 retries */
        int 		sleep_secs = 2;   /* sleep 2 secs between each retry */
	kern_return_t kr;
#endif

	print_debug((
	"--- OSF1_main() ---------------------------------------------\n"));

#if	!NORMA_IPC
	/* 
	 * If not using distributed operation, just get the local default
	 * pager port.
	 */
        kr = vm_set_default_memory_manager(privileged_host_port, 
                                           &default_pager_port);
        if (kr != KERN_SUCCESS) 
            panic("Unable to get default pager port\n");

        print_debug(("system_setup : got default pager port\n"));

#else	 NORMA_IPC
	print_debug(("system_setup: node number  %d -----\n", this_node));

#if	REMOTE_PROC
	/*
	 * Enable remote forks.
	 */
	if (this_node == root_fs_node)
		procman_setup();
#endif

	/* set up list of existing nodes, used for shutdown */
	node_exist_list_setup();

	/* 
	 * Start up a a local node nameserver.
	 */
	ux_create_thread(local_node_nameserver);

	/* 
	 * We keep looping until the local_node_nameserver registers with the 
	 * kernel to provide nameserver service at which point we get back
	 * a good nameserver_port.
	 */
	for (i=0; i<max_retry; i++) {
		kr = norma_get_nameserver_port(privileged_host_port,
						this_node, &nameserver_port);
		if (kr != KERN_SUCCESS)
			panic("can't get nameserver port: ret=0x%x", kr);

		if (MACH_PORT_VALID(nameserver_port))
			break;

		if (!(i % print_count)) 
			printf("Waiting for nameserver on local node...\n");

		mach_init_sleep(sleep_secs);
	}
	
	if (!MACH_PORT_VALID(nameserver_port)) {
		panic("Unable to contact local node nameserver\n"); 
	}

	print_debug(("system_setup: got local nameserver port  -----\n"));
#endif  /* NORMA_IPC */

	/*
	 * If paging services are being imported, then spawn a thread
	 * which will retry till it can get/set remote pager port, and
	 * will also register with the PM node for rproc if REMOTE_PROC
	 * is "on". Otherwise just set the default pager and also register
	 * with the PM node for rproc if REMOTE_PROC is "on".
	 */
#if	!REMOTE_PROC
	/*
	 * Spawn a thread to synchronize the inititalization of the default
	 * pager. 
	 */
	if (this_node != root_fs_node) {
		ux_create_thread(init_default_pager);
	}
#else	/* REMOTE_PROC */
	/*
	 * Spawn a thread to synchronize the inititalization of the default
	 * pager. And to register with the PM node for rproc
	 */
	if (this_node != root_fs_node) {
		ux_create_thread(init_default_pager_rproc);
	}
#endif	/* REMOTE_PROC */

	/*
	 * We must get the default pager port here since it is used in
	 * emul_init when MAP_UAREA is turned on. If left in the above 
	 * thread, it may not get set on time.
	 */
	if (!import_paging) {

		kr = vm_set_default_memory_manager(privileged_host_port,
						&default_pager_port);
		if (kr != KERN_SUCCESS)
			panic("Unable to get default pager port:0x%x\n",kr);
	}

	print_debug(("system_setup : got default pager port\n"));

#if     FULLSERVER
	vnode_pager(); 	/* Turn on the vnode pager, potentially exporting it */
#endif  /* FULLSERVER */
#if	MAP_UAREA
	/*
	 * Set up shared areas between server and client procs:
	 */
	emul_shared_init();
#endif	MAP_UAREA

#if     FULLSERVER
#if	MAPPED_FILES
	mf_init();		/* initialize mapped files subsystem */
	printf("Mapped files enabled.\n");
#endif
#endif  /* FULLSERVER */

	/*
	 * set up system process 0 (dummy)
	 */
	p = &proc[0];
	p->p_stat = SRUN;
	p->p_flag |= SLOAD|SSYS;
	p->p_nice = PRIZERO;
	simple_lock_init(&p->siglock);
	p->sigwait = FALSE;
	p->exit_thread = MACH_PORT_NULL;

	p->p_ref_count = 1;
	queue_init(&p->p_servers);
	p->p_side_ref_count = 0;
	p->p_exit_hold_count = 0;

	/*
	 * Register us as a server thread.  Must be called before the
	 * u_procp is initialized, below:
	 */
	server_thread_register(&u, p);

	print_debug((
	"--- OSF1_main(setting uarea) --------------------------------\n"));
	uarea_init(current_thread(), p);
	uarea_lock_init(&p->p_utask);
	u.u_procp = p;

#if	NCPUS == 1
	/* In uniproc configuration, we need to serialize everything with
	 * master lock:
	 */
	unix_master();
#endif

	u.u_cmask = cmask;

	for (i = 0; i < sizeof(u.u_rlimit)/sizeof(u.u_rlimit[0]); i++)
		u.u_rlimit[i].rlim_cur = u.u_rlimit[i].rlim_max = 
		    RLIM_INFINITY;
	u.u_maxuprc = maxuprc;

	u.u_rlimit[RLIMIT_STACK] = vm_initial_limit_stack;
	u.u_rlimit[RLIMIT_DATA] = vm_initial_limit_data;
	u.u_rlimit[RLIMIT_CORE] = vm_initial_limit_core;

	/* ??? This isn't in integrated kernel.  Why not? */
	for (i = 1; i < NGROUPS; i++)
	    u.u_groups[i] = NOGROUP;

        /*
         * Record its start time:
         */
        TIME_READ_LOCK();
        u.u_start = time;
        TIME_READ_UNLOCK();

#if     FULLSERVER
	mfs_init();
#endif  /* FULLSERVER */
	kallocinit();

#ifdef	OSF1_SERVER
	vm_mmap_init(); /* processes address map initialization */
#endif

	/*
	 * Initialize device switch tables.
	 */
	devsw_init();

#if	NFS
	/*
	 * Get vnodes for swapdev, argdev, and rootdev.
	 *
	 *
	 * We initialize the nfs hooks in case vfsinit calls nfs_init.
	 */
	nfs_hooks_init();
#endif /* NFS */
	vfsinit();
#if     FULLSERVER

	/*
	 * Check and report major and minor device num of the root:
	 */
	if (rootdev == (int)(-1)) {
		printf("Bad root device name: %s\n", rootdev_name);
		panic("Bad root device");
	}
	printf("Root device %s: major=%d minor=%d\n",
		rootdev_name, major(rootdev), minor(rootdev));
#endif  /* FULLSERVER */

#ifdef  OSF1_ADFS
#if     FULLSERVER
	print_debug((
	"--- system_setup(setting devvp for rootdev: %x/%x, root_device_node: %x --\n",
		major(rootdev), minor(rootdev), root_device_node));
	if (bdevvp(rootdev, root_device_node, VBLK, &rootvp))
		panic("can't setup bdevvp of rootdev");
#endif  /* FULLSERVER */
#else
	print_debug((
	"--- system_setup(setting devvp for rootdev = %x/%x ---\n",
		major(rootdev), minor(rootdev)));
	if (bdevvp(rootdev, &rootvp))
		panic("can't setup bdevvp of rootdev");
#endif

#if	SEC_BASE
	sec_init();
#endif
	/*
	 * Setup credentials
	 */
	p->p_rcred = crget();
	p->p_rcred->cr_ngroups = 1;

	softclock_init();	/* bsd/mach_clock.c */

#ifdef	OSF1_ADFS
	/*
	 * Turn on credentials service in support of fileserver
	 * functionality disjoint from process management functionality.
	 */
	cm_init();
	cc_init();
#endif

	/*
	 * Initialize tables, protocols, and set up well-known inodes.
	 */
	cinit();

#ifdef	TNC
	netinit(TRUE);
#else	/* TNC */
	netinit(this_node == root_fs_node);
#endif	/* TNC */

#if     FULLSERVER
	if (this_node == root_fs_node) {
#if	STREAMS
		pse_init();
#endif	/* STREAMS */
	}
#endif  /* FULLSERVER */

	pqinit();
	print_debug((
	"--- OSF1_main(before file_table_init) -----------------------\n"));
	file_table_init();
	print_debug((
	"--- OSF1_main(after file_table_init) ------------------------\n"));
	global_lock_initialization();
#if	multimax
	mmax_initialization();
#endif
	/*
	 * Initialize the file system server dummy processes
	 */
	print_debug((
	"--- system_setup(before fsproc_init) ---------------------------\n"));
	fsproc_init();
	print_debug((
	"--- system_setup(after fsproc_init)-----------------------------\n"));

#ifdef	TNC
	/*
	 * Initialize the System V IPC stuff if we are the SVIPC server.
	 */
	print_debug((
	"--- system_setup(before svipc_init) ---------------------------\n"));
	if (this_node == svipc_node) {
		svipc_init();
	}
	print_debug((
	"--- system_setup(after svipc_init)-----------------------------\n"));
#endif	/* TNC */

	/*
	 * Initialize vproc table.
	 */
	print_debug((
	"--- system_setup(before vproc_init) ----------------------------\n"));
	vproc_init();
	print_debug((
	"--- system_setup(after vproc_init) -----------------------------\n"));

	/*
	 * Initialize the vproc/pvproc for proc 0 after allocating credentials.
	 */
	p = &proc[0];
	p->p_cred = credentials_allocate(p->p_pid, p->p_pgid, p->p_sid, 
			p->p_rcred, p->p_utask.uu_cmask,
			p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_cur,
			p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_max,
			p->p_ruid, p->p_rgid,
			p->p_task);

#ifdef  TNC
	print_debug((
	"--- system_setup(before vs_init) -------------------------------\n"));
	vs_init();
	print_debug((
	"--- system_setup(after vs_init) --------------------------------\n"));
	print_debug((
	"--- system_setup(before dpvproc_init0) -------------------------\n"));
	dpvproc_init0(p);
	print_debug((
	"--- system_setup(after dpvproc_init0) --------------------------\n"));
#else
	print_debug((
	"--- system_setup(before bpvproc_init0) -------------------------\n"));
	bpvproc_init0(p);
	print_debug((
	"--- system_setup(after bpvproc_init0) --------------------------\n"));
#endif

#if     FULLSERVER
	/*
	 * Initialize block IO system.
	 */
	print_debug((
	"--- OSF1_main(before bio_init) ------------------------------\n"));
	bio_init();
	print_debug((
	"--- OSF1_main(after bio_init) -------------------------------\n"));
#endif  /* FULLSERVER */

#if	SEC_BASE
	audinit();
#if	SEC_ARCH
	spdinit();
#endif
#endif

#if	NORMA_IPC
	/*
	 * Allocate a file service port with a receive right and a
	 * send right.
	 */
	kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				&fileserver_port);
	if (kr != KERN_SUCCESS)
		panic("can't allocate fileserver port:0x%x\n",kr);

	kr = mach_port_insert_right(mach_task_self(), fileserver_port,
				fileserver_port, MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
		panic("can't create send right: ret=0x%x\n", kr);

	/*
	 * Tell the server to service the port
	 */
	ux_server_add_port(fileserver_port);

	/*
	 * Register the fileserver port with the name server.
	 */
	if ((kr = netname_check_in(nameserver_port, "FILESERVER", 
				   mach_task_self(), fileserver_port))
							!= KERN_SUCCESS)
		panic("can't register fileserver port: ret=0x%x",kr);
	print_debug((
	"--- OSF1_main(after file service registration) --------------\n"));
#endif  /* NORMA_IPC */

#ifdef	i860
	nx_boot_init();
#endif	/* i860 */

#ifdef SLL
	/*
	 * Initialize static load leveling.
	 */
	sll_init();
#endif /* SLL */

	/*
	 * Open the console.
	 */
	(void) cons_open(makedev(0,0), FREAD|FWRITE);

#if     FULLSERVER
	/*
	 * Set up the root file system.
	 * Only the root fileserver mounts the root filesystem.
	 */
	if (this_node == root_fs_node) {
#ifdef	OSF1_ADFS
		struct uthread *uth = &u;
#endif

		if ((*mountroot)())
			panic("cannot mount root");

		rootvp->v_mount = rootfs; /* set up root device's mount ptr */
		/*
		 * Get vnode for '/'.
		 * Setup rootdir and u.u_cdir to point to it.
		 */
		VFS_ROOT(rootfs, &rootdir, ret);
		if (ret)
			panic("cannot find root vnode");
#ifdef	OSF1_ADFS
		uth->u_cdirproxy.vpx_usecount = 0;
		uth->u_cdir = rootdir;
		uth->u_cdirport = MACH_PORT_NULL;
		remote_vref(&uth->u_cdirproxy);

		uth->u_rdirproxy.vpx_usecount = 0;
		uth->u_rdir = NULLVP;
		uth->u_rdirport = MACH_PORT_NULL;

#else
		u.u_cdir = rootdir;
		VREF(rootdir);
		u.u_rdir = NULL;
#endif
		print_debug((
	"--- system_setup(sucessfully mounted the root !) ---------------\n"));
	} else
		print_debug((
		"--- system_setup(non-root fileserver !) ---------------\n"));
#else   /* FULLSERVER */
                print_debug((
                "--- system_setup(non-root non-fileserver !) ------------\n"));
#endif  /* FULLSERVER */



/*	vnode_pager_default_init(); */
	boottime = time;

	/* SystemV ipc */
	msginit();
	seminit();

	/*
	 * Init record/file locking
	 */
	flckinit();

	/*
	 * make init process
	 */

	siginit(&proc[0]);

#if	XPR_DEBUG
	xprinit();
#endif

	/*
	 * Initialize the proc[0] vm_mmap structures so newproc can copy them
	 */
	vm_mmap_proc_init(u.u_procp);

#if	!NORMA_IPC
	/*
	 * Setup root vnode port.
	 */
	get_vnode_port(rootdir, &root_vnode_port);

#else	!NORMA_IPC

	/*
	 * Set up fileserver ports.  The root fileserver simply allocates
	 * a port for the root vnode.  Other fileservers must contact
	 * their local nameserver to obtain a send right to the root
	 * fileserver's service port.  They then send a message to that
	 * port to acquire a send right to the root vnode port.
	 */
	if (this_node == root_fs_node) {
		struct uthread *uth = &u;
		/*
		 * If this is the root fileserver node, setup root
		 * fileserver ports.
		 */
		get_vnode_port(rootdir, &root_vnode_port);

		/* boost root vnode port queue limit */
		kr = mach_port_set_qlimit(mach_task_self(),
					  root_vnode_port,
					  MACH_PORT_QLIMIT_MAX);
		if (kr != KERN_SUCCESS)
			panic("root_vnode_port: set_qlimit failed");

		root_fs_port = fileserver_port;
		root_ns_port = nameserver_port;

		uth->u_rdirproxy.vpx_usecount = 0;
		uth->u_rdir = rootdir;
		uth->u_rdirport = root_vnode_port;
		remote_vref(&uth->u_rdirproxy);
	} else {
		node_t  tmp_node;
		/*
		 * Get the nameserver port on the root fileserver node.
		 * Loop until the nameserver is up.
		 */
		for (i=0; i<max_retry; i++) {

			kr = norma_get_nameserver_port(privileged_host_port,
						root_fs_node, &root_ns_port);
			if (kr != KERN_SUCCESS)
				panic("can't get root nameserver port: %x", kr);
			
			if (MACH_PORT_VALID(root_ns_port))
				break;

			if (!(i % print_count)) 
				printf("Waiting for nameserver on node %d...\n",
					root_fs_node);
			mach_init_sleep(sleep_secs);
		}

		if (!MACH_PORT_VALID(root_ns_port))
			panic("Unable to get ns port on root fileserver node");

		kr = norma_port_location_hint(mach_task_self(),
						root_ns_port, &tmp_node);
		if (kr != KERN_SUCCESS)
			panic("can't get node # for root nameserver port: 0x%x",
				kr);

		if (tmp_node == this_node)
			panic("root nameserver port is local!!!");

		/*
		 * Lookup the root fileserver's service port.
		 */
		for (i=0; i< max_retry; i++) {
			
			kr = netname_look_up(root_ns_port, "FILESERVER",
						"FILESERVER", &root_fs_port);
			if (kr == KERN_SUCCESS)
				break;
			if (!(i % print_count))
				printf("Waiting for root fileserver on node %d...\n",
					root_fs_node);
			mach_init_sleep(sleep_secs);
		}
		if (kr != KERN_SUCCESS) 
			panic("unable to look up the root file svr port: ret=0x%x", kr);

		kr = norma_port_location_hint(mach_task_self(),
						root_fs_port, &tmp_node);
		if (kr != KERN_SUCCESS)
			panic("can't get node # for root fileserver port: ret=%x",
				kr);

		if (tmp_node == this_node)
			panic("root fileserver port is local!!");

		/*
		 * Send a message to the root fileserver's service port
		 * requesting a send right to the root vnode port.
		 */
		for (i=0; i< max_retry; i++) {
			kr = fsvr_get_root_port(root_fs_port, &root_vnode_port);
			if (kr == KERN_SUCCESS)
				break;
			if (!(i % print_count))
				printf("Waiting for root fileserver port on node %d...\n",
					root_fs_node);
			mach_init_sleep(sleep_secs);
		}
		if (kr != KERN_SUCCESS) 
			panic("can't get root fileserver port");

#ifndef	TNC
		/* now register this node's existence for shutdown purposes */
		kr = remote_server_register(this_node);
		if (kr != KERN_SUCCESS)
			panic("unable to register server node: ret=0x%x", kr);
#endif	/* !TNC */
	}

#endif  !NORMA_IPC

	print_debug(("system_setup: root vnode port set up -----\n"));

	/* start up a thread to catch the server's exceptions */
	ux_create_thread(svr_exception_thread);

	/*
	 * Establish the exception port for the local server
	 */
	ux_exception_port = ux_handler_init();

	if (this_node == root_fs_node) {
		/*
		 * Make init process on master node.  It MUST have pid 1
		 * and be &proc[1].
		 * not load the emulator or resume the init process thread.
		 */

		init_proc = newproc(1,TRUE);
		print_debug((
	"--- system_setup(after newproc) --------------------------------\n"));

		/*
		 * Initialize the vproc/pvproc for proc 1 after allocating 
		 * credentials.
		 */
		p = &proc[1];
		p->p_cred = credentials_allocate(p->p_pid, p->p_pgid, p->p_sid, 
				p->p_rcred, p->p_utask.uu_cmask,
				p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_cur,
				p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_max,
				p->p_ruid, p->p_rgid,
				p->p_task);
#ifdef	TNC
		(void) dpvproc_init1(p);
#else
		(void) bpvproc_init1(p);
#endif

		/* exception handler */
		kr = task_set_exception_port(init_proc->p_task,
					ux_exception_port);
		if (kr != KERN_SUCCESS)
			panic("system_setup: can't set exception port");
	}

	if (this_node == root_fs_node) {
		/* LOAD THE EMULATION LIBRARY ON THE MASTER NODE */

		/*
		 * although the emulator is not loaded yet, we do this to stop
		 * vm_exec from blowing it away when it is.
		 */
		vm_mmap_add(init_proc, EMULATOR_BASE, EMULATOR_SIZE,
			    (memory_object_t)NULL, this_node, (vm_offset_t)0,
			    VM_MMAP_FORCE_KEEP_ON_EXEC);

		/*
		 * Load the emulator library
		 */
		ret = load_emulator(init_proc);
		if (ret)
			panic("load_emulator x%x", ret);
		print_debug((
	"--- system_setup(after load_emulator) --------------------------\n"));
#ifdef NX
                print_debug((
        "--- system_setup(before init_service_partition) ----------------\n"));
                init_service_partition(init_proc);
                print_debug((
        "--- system_setup(after init_service_partition) ----------------\n"));
#endif /* NX */


		/*
		 * Start the first task!
		 */
		(void) thread_resume(init_proc->p_thread);
	}


#if	UNIX_LOCKS
	ux_create_thread(acctwatch_thread);
#endif

	/*
	 * Protect the following areas of the data segment against
	 * scribbling by marking them read-only using vm_protect().
	 * The areas are declared with an extra page of unused memory
	 * before and after them, since protection is done on a page
	 * by page basis.
	 */
#if MACH_ASSERT
	PROTECT_CONST_REGION(&unixsw[0],	e_unixsw, "unixsw");
	PROTECT_CONST_REGION(&dvproc_ops_table,	e_dvpop,  "dvpop");
#endif
	/*
	 * Let ps know what we are
	 */
	task_name("OSF/1 server");

	/*
	 * Become another server thread
	 */
	uarea_terminate(&u);
	server_thread_deregister(&u, &proc[0]);

	printf("Server initialization finished.\n");

	{
	struct task_basic_info	task_info_data;
	mach_msg_type_number_t	task_info_size = TASK_BASIC_INFO_COUNT;
	kern_return_t		rc;

	rc = task_info(	mach_task_self(),
			TASK_BASIC_INFO,
			(task_info_t)&task_info_data,
			(mach_msg_type_number_t *)&task_info_size );

	printf("Virtual Size: %d\n", task_info_data.virtual_size);
	printf("Resident Size: %d\n", task_info_data.resident_size);
	}

#if	NCPUS == 1
	unix_release();
#endif

	ux_server_loop();
	/*NOTREACHED*/
}

void
global_lock_initialization()
{
	extern struct slock	accounting_lock;
	udecl_simple_lock_data(extern,cblock_freelist_lock)
	decl_simple_lock_data(extern,printf_lock)
	extern void		log_init();
#if	!defined (OSF1_SERVER) && SER_COMPAT
	/* from ../bsd */
	extern lock_data_t	default_uni_lock;
#endif
#if	MACH_LDEBUG
	extern int		check_locks;
	int	i;
#endif

	/* from ../bsd */
	TIME_LOCK_INIT();
	cons_initialization();
	tty_initialization();

#if     NPTY > 0
	pty_initialization();
#endif /* NPTY > 0 */

	log_init();
	usimple_lock_init(&accounting_lock);
#if	0
	lock_init2(&mount_lock, TRUE, LTYPE_MOUNT_TABLE);
#endif
	usimple_lock_init(&cblock_freelist_lock);
	lock_init2(&hostname_lock, TRUE, LTYPE_HOSTNAME);
	simple_lock_init(&printf_lock);		/* subr_prf.c */
	select_init();
#if	!defined (OSF1_SERVER) && SER_COMPAT
	lock_init2(&default_uni_lock, TRUE, LTYPE_DEFAULT_UNI);
#endif

#if	MACH_LDEBUG
	check_locks = 1;
	check_lock_counts = 0;
	for (i = 0; i < NCPUS; i++) {
		slck_dbg[i].count = 0;
		bzero(&slck_dbg[i].addr[0], sizeof(int)*MAX_LOCK);
	}
#endif

}

#if	multimax
static void
mmax_initialization()
{
	extern void	ms_cmdinit();

	extern struct buf	sccbuf; /* XXX */
	extern struct slock	console_lock; /* XXX */
	extern lock_data_t init_bdl_lock;

	ms_cmdinit();
	lock_init2(&sccbuf.b_lock, TRUE, LTYPE_BUF);
	event_clear(&sccbuf.b_iocomplete);
	usimple_lock_init(&console_lock);
	simple_lock_init(&init_bdl_lock);

}
#endif

mach_port_t	pager_fs_port = MACH_PORT_NULL;

#if	REMOTE_PROC
extern	int	accept_procs;
#endif	/* REMOTE_PROC */

any_t
#if	!REMOTE_PROC
init_default_pager()
#else	/* REMOTE_PROC */
init_default_pager_rproc()
#endif	/* REMOTE_PROC */
{
	kern_return_t	kr;
	int		i;
	int		pager_is_ready = FALSE;
	int 		max_retry = 75;   /* try 75 times for server contact */
	int 		print_count = 10; /* print a message every 10 retries */
	int 		sleep_secs = 10;  /* sleep 10 secs between each retry */
	mach_port_t	remote_nameserver_port = MACH_PORT_NULL;
	mach_port_t	new_default_pager_port = MACH_PORT_NULL;

	/*
	 * This is to be done only when importing paging, otherwise 
	 * we already have a default pager, and if rproc is on, we
	 * will register with the PM, one the root_fs_node is set
	 * and if rproc is not on, we are done.
	 */
	if (import_paging) {
	    printf("Import paging from %d\n",pager_node);
	    /* loop until the remote lnsvr is up */
	    for (i=0; i<max_retry; i++) {
		if ((kr = norma_get_nameserver_port(privileged_host_port,
						      pager_node,
						      &remote_nameserver_port))
			    != KERN_SUCCESS)
		panic("Cannot get ns port for remote pager :0x%x",kr);

		if (remote_nameserver_port != MACH_PORT_NULL &&
			    remote_nameserver_port != MACH_PORT_DEAD)
			    break;

		if (!(i % print_count)) {
			    printf("Waiting for nameserver on node %d...\n", 
						pager_node);
		}
		mach_init_sleep(sleep_secs);
	    }
	    if (remote_nameserver_port == MACH_PORT_NULL || 
		remote_nameserver_port == MACH_PORT_DEAD)
		panic("Unable to get ns port on remote pager node \n");

	    print_debug(("Got nameserver port for pager node %d\n",pager_node));
	    for (i=0; i<max_retry; i++) {
		if ((kr = netname_look_up(remote_nameserver_port, "FILESERVER",
						"FILESERVER", 
						&pager_fs_port))
			    == KERN_SUCCESS)
		break;
		if (!(i % print_count)) 
			printf("Waiting for fileserver port on node %d...\n", 
						pager_node);
		print_debug(("Got fs port for pager node %d\n",pager_node));
		mach_init_sleep(sleep_secs);
	    }
		
	    mach_port_deallocate(mach_task_self(), remote_nameserver_port);

	    /*
	     * Call up the remote vnode pager to see if it is initialized
	     */
	    i = 0;
	    while (pager_is_ready != TRUE) {
		kr = remote_vnode_pager_get_state(&pager_is_ready, 
						&new_default_pager_port);
		if (kr != KERN_SUCCESS)
			panic("Unable to call remote pager:0x%x\n",kr);

		if (!(i % print_count)) 
			printf("Waiting for vnode pager on node %d...\n", 
						pager_node);
		mach_init_sleep(sleep_secs);
		i++;
	    }
	    if(new_default_pager_port == MACH_PORT_NULL)
		    panic("Default pager port is NULL");
	    print_debug(("Got pager port 0x%x\n", new_default_pager_port));
    	    /* 
	     * save our default pager port for use later. Since vm_set....
	     * swaps the default_pager_port parameter we have to use 
	     * 'new_default_pager_port'
	     */
	    default_pager_port = new_default_pager_port;
	    kr = vm_set_default_memory_manager_poly(privileged_host_port,
						&new_default_pager_port,
						MACH_MSG_TYPE_COPY_SEND);
	    if (kr != KERN_SUCCESS)
		panic("unable to set remote default pager port:0x%x\n",kr);
	}

#if	REMOTE_PROC
	if (accept_procs == TRUE) {
		/*
		 * Now register yourself with the PM but wait till root_fs_port 
		 * is set
		 */ 
		for (i=0; i<max_retry; i++) {
			if (root_fs_port != MACH_PORT_NULL) 
				break;
			if (!(i % print_count)) 
				printf("Waiting for root_fs_port on node %d\n", 
							root_fs_node);
			mach_init_sleep(sleep_secs);
		}
		kr = remote_rproc_server_register(this_node);
		if (kr != KERN_SUCCESS)
			panic("unable to register with PM: ret=0x%x",kr);
	}
#endif	/* REMOTE_PROC */

	/*
	 * Now go away
	 */
	return((any_t) 0);
}
#ifdef	i860
/*
 *  If we are not boot_first_node, then look up the SYNCSERVERINIT
 *  port and send a sync message to it.
 */
nx_boot_init()
{
    mach_port_t     sync_server_init_port;
    mach_port_t     remote_nameserver_port = MACH_PORT_NULL;
    kern_return_t   kr;
    typedef struct {
	mach_msg_header_t head;
	mach_msg_type_t nodeType;
	node_t  node;
	char node_attributes[1024];
    } syncsvr_t;
    syncsvr_t       syncsvrsend;


    if (this_node != root_fs_node) {
        /*
         * This is where we tell the bootmesh program that we are ready
         */


        if ((kr = norma_get_nameserver_port(privileged_host_port,
					    root_fs_node,
					    &remote_nameserver_port)) 
	    != KERN_SUCCESS)
	        panic("can't get nameserver port : 0x%x", kr);

	kr = netname_look_up(remote_nameserver_port, "SYNCSERVERINIT",
				 "SYNCSERVERINIT", &sync_server_init_port);

	if( kr == KERN_SUCCESS ) {
	    syncsvrsend.head.msgh_bits = MACH_MSGH_BITS(
		      MACH_MSG_TYPE_COPY_SEND,MACH_MSG_TYPE_MAKE_SEND_ONCE);
	    syncsvrsend.head.msgh_size = sizeof syncsvrsend;
	    syncsvrsend.head.msgh_remote_port = sync_server_init_port;
	    syncsvrsend.head.msgh_local_port = MACH_PORT_NULL;
	    syncsvrsend.head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	    syncsvrsend.head.msgh_id = 0;
	    syncsvrsend.head.msgh_seqno = 0;
	    syncsvrsend.node = this_node;

	    nx_create_node_attributes(privileged_host_port,
	      syncsvrsend.node_attributes,
	      sizeof(syncsvrsend.node_attributes));

	    syncsvrsend.nodeType.msgt_name = MACH_MSG_TYPE_INTEGER_32;
	    syncsvrsend.nodeType.msgt_size = 32;
	    syncsvrsend.nodeType.msgt_number = 1;
	    syncsvrsend.nodeType.msgt_inline = 1;
	    syncsvrsend.nodeType.msgt_longform = 0;
	    syncsvrsend.nodeType.msgt_deallocate = 0;
	    syncsvrsend.nodeType.msgt_unused   = 0;

	    if((kr = mach_msg_send(&(syncsvrsend.head))) != KERN_SUCCESS){
		panic("send failed : 0x%x", kr);
	    }
	}
    }
}
#endif	/* i860 */
