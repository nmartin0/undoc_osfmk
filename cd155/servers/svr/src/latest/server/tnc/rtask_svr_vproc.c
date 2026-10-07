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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * $Log: rtask_svr_vproc.c,v $
 * Revision 1.27  1995/02/01  21:50:15  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.26  1994/11/18  20:43:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.25  1994/07/27  16:50:54  johannes
 * In rfork_server() the call of rfork_pproc_unload_msg() and
 * of rtask_pproc_fork() had to be extended.
 *
 * In rforkmulti_server() the call of rfork_pproc_unload_msg() and
 * of rtask_pproc_fork() had to be extended.
 *
 * In migrate_server() the call of migrate_pproc_unload_msg() and
 * of rtask_pproc_fork() had to be extended.
 *
 * In rexecve_server() the extended rtask_pproc_fork() is called with null
 * ports.
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
 * Revision 1.24  1994/07/21  14:30:41  johannes
 * added missing parameters to rtask_pproc_fork() call in rforkmulti_server()
 *
 *  Reviewer: chrisp
 *  Risk: L
 *  Benefit or PTS #: bug fix, neccessary for PARACORE extensions
 *  Testing: running a NX program with PARACORE extensions
 *  Module(s): tnc/rtask_svr_vproc.c
 *
 * Revision 1.23  1994/07/07  16:40:31  chrisp
 * Install emulator file ports before joining parent's process group
 * and obtaining updated signal state - which may lead to termination.
 *
 *  Reviewer: None
 *  Risk: None
 *  Benefit or PTS #: 9920
 *  Testing: Interrupting test program no longer panics server
 *  Module(s): rtask_svr_vproc.c
 *
 * Revision 1.22  1994/06/18  21:49:00  chrisp
 * In rforkmulti_server(), eliminate extra send rights on child task and
 * child emulator thread ports - these are no longer transferred to parent.
 * Move unlocking of child vproc until file ports have been inserted.
 * Remove duplicate line initializing child pid array - merge problem.
 * Add indication of error codes in all panic messages.
 *
 *  Reviewer: yazz@locus.com
 *  Risk: L
 *  Benefit or PTS #: 9859 and 9920
 *  Testing: TNC functional tests.
 *  Module(s): rtask_svr_vproc.c
 *
 * Revision 1.21  1994/06/14  21:13:23  cfj
 * In rforkmulti_server(), if the thread_resume() of the local child
 * fails, just set the vproc to NULL.
 *
 *  Reviewer:chrisp@locus.com
 *  Risk:L
 *  Benefit or PTS #:9822
 *  Testing: The following EATs:
 * 	  message, controlc, misc, rmcall, rmcmd
 *  Module(s):server/tnc/rtask_svr_vproc.c
 *
 * Revision 1.20  1994/06/03  15:21:59  chrisp
 * Corrected re-definition of macro __PVPSOP__ for the non-debug case
 * (when MACH_ASSERT=0).
 *
 *  Reviewer:
 *  Risk: L
 *  Benefit or PTS #:
 *  Testing: Compilation successful without MACH_ASSERT
 *  Module(s): rtask_cli_vproc.c rtask_svr_vproc.c
 *
 * Revision 1.19  1994/06/02  22:29:42  chrisp
 * In dpvpop_reap(), perform PVPOP_RMV_PGRP_LIST() for zombie child
 * only if child's pgrp leader is not its parent; return child pgid.
 * In dvpop_wait(), analyze pgid returned for zombie child and call
 * PVPOP_RMV_PGRP_LIST() if this is the parent pid.
 *
 * Support added for waitmulti() - viz: elder reporting and reap multi
 * operation (refer to rtask_cli_vproc.c).
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): dpvproc.h dvp_pvpops.c dvp_vpops.c pvp.ops pvps.ops rtask.h
 * 	    rtask_cli_vproc.c rtask_server.c rtask_svr_vproc.c
 * 	    spanning_tree.c tnc_async.defs tnc_server_side.c
 * 	    tnc_types.defs tnc_types.h tnc_types_gen.c
 *
 * Revision 1.18  1994/04/13  16:46:45  cfj
 * Merged revision 1.14.2.3 from R1_2.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.17  1994/03/14  02:06:25  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.16  1994/01/12  19:13:35  cfj
 * Merge bug fix from R1_2.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.15  1993/12/11  01:22:35  yazz
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 6916
 *  Testing: multitest.c now passes
 *  Module(s): server/tnc/rtask_svr_pproc.c + server/tnc/rtask_svr_vproc.c
 *
 * Have the rforkmulti vproc layer cope correctly with failures returned from
 * the physical (pproc) layer when creating new processes.  This includes
 * initializing port names, errno values and child pid values properly.
 *
 * Also, prevent the NX code from issuing vproc operations on a non-existant
 * vproc, when this same pproc layer failure occurs.
 *
 * Revision 1.14.2.3  1994/04/13  16:41:06  cfj
 * Modified rfork_server() to initialize all out parameters to
 * something rational.  Specifically, ports get MACH_PORT_NULL.
 *
 *  Reviewer:yazz@locus.com
 *  Risk:L
 *  Benefit or PTS #:8991
 *  Testing:kenbus
 *  Module(s):server/tnc/rtask_svr_vproc.c
 *
 * Revision 1.14.2.2  1994/01/12  19:07:53  cfj
 * Modified rfork_server() and rforkmulti_server() so that the call to
 * pproc_set_attr() is made before the call to dvp_child_join_pgrp_end() to
 * avail trying to use the p->p_vproc field before it was initialized.
 *
 *  Reviewer: yazz@locus.com
 *  Risk: L
 *  Benefit or PTS #: 7655
 *  Testing: Entered test case.
 *  Module(s): server/tnc/rtask_svr_vproc.c
 *             server/tnc/dvp_vpops.c
 *
 * Revision 1.14.2.1  1993/12/17  19:52:57  cfj
 * Merge bug fix for PTS #6916 from the main-stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.14  1993/11/03  19:17:41  yazz
 * Establish a sequence number mechanism whereby process group signals, such as
 * those generated by CTRL/C, are guaranteed to be delivered to child processes
 * of a reproducing (fork(), rfork(), rforkmulti(), etc.) task.  Many unixes
 * have a timing window where new child procs can miss out on a pgrp-style signal.
 * 
 * Revision 1.13  1993/10/21  23:36:03  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.12  1993/07/14  18:33:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.8  1993/07/01  20:46:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.11  1993/06/09  00:10:01  cfj
 * Change occurances of #include <i860ipsc/mcmsg/*.h> to #include <i860paragon/mcmsg/*.h>
 *
 * Revision 1.10  1993/05/06  19:24:10  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.6  1993/05/03  17:46:33  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.9  1993/04/13  23:55:30  cfj
 * Merge with T9.5.
 *
 * Revision 1.4.6.5  1993/04/13  23:46:22  cfj
 * Fix rforkmulti workaround to pass local_task.
 *
 * Revision 1.4.6.4  1993/04/13  22:50:58  cfj
 * Removed extra params from cli_rforkmulti_msg_receive.
 *
 * Revision 1.8  1993/04/03  03:09:16  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.7  1993/03/29  15:33:54  cfj
 * Merge with T9.
 *
 * Revision 1.4.6.2  1993/03/26  23:41:37  cfj
 * Put ux_server_thread_blocking/unblocking around off node RPCs.
 *     Revision 3.47  93/03/23  19:52:06  yazz
 *     Added ux_server_thread_blocking/unblocking() calls around remote tasking
 *     RPCs.
 * 
 * Revision 1.6  1993/03/11  16:17:24  cfj
 * Merged in final version of the new forkmulti() from Locus but kept the workaround.
 *
 * Revision 1.1.2.5.2.1  1992/12/16  06:02:47  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/11/30  22:48:13  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.5  1992/11/15  21:27:27  cfj
 * Put rforkmulti() receive workaround back in.
 *
 * Revision 1.1.2.4  1992/11/10  01:55:13  cfj
 * Delete duplicate declaration of error variable.
 *
 * Revision 1.1.2.3  1992/11/09  20:35:43  cfj
 * cli_rforkmulti_long_msg_receive was not being called in rforkmulti_server()
 *
 * Revision 1.1.2.2  1992/11/06  20:31:53  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:46:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.43  1992/11/02  21:50:30  cfj
 * Final integration and testing of IPD modifications
 *
 * Revision 3.42  1992/10/23  02:00:07  cfj
 * T6 merge.
 * 
 * Revision 3.49  93/09/16  09:09:23  chrisp
 * [SPE 0030] Generic Spanning Trees: dpvproc_ops_table now a 3-way vector.
 * [BUG 0387] Migration deadlock under stress: no need to lock child vprocs
 * 	of migrating parent arriving on a new node.
 * 
 * Revision 3.48  93/05/04  16:51:55  yazz
 * Added ux_server_thread_blocking/unblocking() surrounding
 * norma_task_create().
 * 
 * Revision 3.47  93/03/23  19:52:06  yazz
 * Added ux_server_thread_blocking/unblocking() calls around remote tasking
 * RPCs.
 * 
 * Revision 3.46  93/03/01  11:30:30  roman
 * [SPE #18] For a faster rforkmulti(), create a task on the local node, and
 * 	have this task used for virtual address space inheritance by
 * 	pseudo-children in the rforkmulti() spanning tree.
 * 
 * Revision 3.45  92/12/04  07:15:12  chrisp
 * Further corrections for bug #110: for error returns of migrate and
 * 	rexecve, the process' pid rather than its parent's was being
 * 	quoted in call to tnc_dispose_vproc_port() for parent's port.
 * 	This caused a remote vproc free panic subsequently.
 * 
 * Revision 3.44  92/12/01  11:27:12  chrisp
 * [Bug #110] In failure cases, call new routine tnc_dispose_vproc_port()
 * 	to safely dispose of vproc send rights (which may have been
 * 	concurrently renamed).
 * 
 * Revision 3.43  92/11/16  09:28:29  roman
 * Fix pointer types in rforkmulti_server() to correctly reflect 
 * 	underlying types.
 * 
 * Revision 3.42  92/11/09  11:07:33  roman
 * [Bug 0097] Call that should have been to  
 * 	cli_rforkmulti_long_msg_receive() was calling
 * 	cli_rforkmulti_msg_receive() instead.
 * 
 * Revision 3.41  92/11/02  12:14:14  roman
 * Change top-level routine names and interfaces slightly (getting rid
 * 	of some parameters) to allow for calls from rtask_server.c.
 * Restructure of rforkmulti() to use out-of-line memory when
 * 	necessary for long arrays of nodes.
 * 
 * Revision 3.40  92/10/08  17:05:40  chrisp
 * Change names of operations tables for consistency.
 * 
 * Revision 3.39  92/10/01  10:31:02  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.38  92/09/29  09:54:59  roman
 * Remove unnecessary clearing of ZOMBIE and STOP fields in pvprocs
 * 	of foster children for migrate and rexec.
 * 
 * Revision 3.37  92/09/29  08:14:47  roman
 * Change all references from "site" to node.
 * Make sure all locks use the new lock macros.
 * Change dpvproc_struct_init() to take a vproc parameter rather than
 * 	a pvproc parameter.
 * Add concept of foster children lists which can move over on
 * 	rexec and migrate.
 * Make assorted declarations of vprocs more local to { } for more readable
 * 	code.
 * 
 * Revision 3.36  92/07/30  16:11:52  chrisp
 * Reset both proc and vproc port sequence counts.
 * 
 * Revision 3.35  92/07/17  11:24:01  roman
 * Fix error in rforkmulti() where a sequence of bad node numbers caused
 * 	a server panic.
 * 
 * Revision 3.34  92/07/10  09:03:00  chrisp
 * Initialize port sequence count to 0 on arrival of migration or rexec.
 * 
 * Revision 3.33  92/07/08  09:09:38  roman
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.32  92/06/26  14:00:56  chrisp
 * [Bug #27] Job control handling fixed for migrated/rexec'd processes - pvproc
 * 	field pvp_jobc added to MIG.
 * 
 * Revision 3.31  92/06/05  15:39:45  roman
 * Fix code for mmap'ed files following v0.89 integration.
 * 
 * Revision 3.30  92/05/06  13:34:24  chrisp
 * [Bug #9] Transfer pvproc field pvp_cttynode on rexec()/migrate().
 * 
 * Revision 3.29  92/05/06  09:23:42  chrisp
 * Add calls to VPROC_[UN]LOCK_FLAG macros to guard updates to pvp_flag.
 * 
 * Revision 3.28  92/04/22  13:46:20  chrisp
 * Alter existence rules for session leader vproc, mark:
 * 	- on node of each pgrp leader in session;
 * 	- on node of session leader for each pgrp in session list.
 * In svr_rfork[multi](), session vproc port parameter now not required and
 * 	deleted.
 * In svr_migrate() and svr_rexecve(), keep reference to session vproc only
 * 	for pgrp leader processes.
 * Correct vproc locking in svr_rforkmulti().
 * 
 * Revision 3.27  92/04/22  08:53:47  chrisp
 * In rfork(), take child vproc lock and setup op tables before adding it to
 * 	hash list.
 * Add vproc locks throughout rfork()/migrate()/rexec() code.
 * Re-arrange clauses concerning pgrp leadership and session leadership in
 * 	migrate() and rexec() code to improve readability.
 * 
 * Revision 3.26  92/04/20  15:14:48  chrisp
 * Improve accuracy of comments.
 * 
 * Revision 3.25  92/04/14  14:29:24  roman
 * Get rid of extraneous root directory and current directory parameters to
 * 	rexecve_pproc_exec().
 * 
 * Revision 3.24  92/04/14  10:40:22  roman
 * Correct reference counting for pgrp and session lists when migrating or
 * 	rexec'ing pgrp/session leaders.
 * Add current and root directory ports in call to rtask_pproc_exec().
 * 
 * Revision 3.23  92/04/06  16:05:43  chrisp
 * Take note of return value from tnc_get_server_port() and don't assume
 * 	perameters return sensible values in the error case.
 * 
 * Revision 3.22  92/04/01  16:20:10  roman
 * Add support for server portion of rforkmulti() system call.
 * 
 * Revision 3.21  92/03/27  17:39:09  roman
 * Fix order of parameters in svr_rexecve() to match the .defs file (the
 * 	proc port was in the wrong location).
 * 
 * Revision 3.20  92/03/27  11:31:09  roman
 * Changes due to new OSF/1 AD v0.8.5.1 handling of root and current working
 * 	directory ports.
 * 
 * Revision 3.19  92/03/18  12:16:03  roman
 * Reorganize a bit of code at the end of svr_migrate() and svr_rexecve() to
 * allow for the fact that vproc ops and pvproc ops may happen at any time
 * due to the fact the server is multi-threaded and pre-emptable.
 * 
 * Revision 3.18  92/03/12  15:30:36  roman
 * Initialize the new pvproc fields that track a parent's session and pgrp id.
 * Change the sequence of operations in locating the vproc and calling
 * start_tncserver_op() to make recovery from a failed
 * start_tncserver_op() more simplistic.
 * Fix missing parameter to call to tnc_bestow_vproc_port().
 * 
 * Revision 3.17  92/02/20  09:26:38  roman
 * Add new parameter to tnc_install_vproc_port() and tnc_bestow_vproc_port().
 * No longer manipulate the PV_IS_LOCAL flag in this file, as this
 * is now done by tnc_install_vproc_port() and tnc_bestow_vproc_port().
 * Get rid of calls to tnc_uninstall_vproc_port() as the work it used to
 * do is now done by tnv_install_vproc_port() with the additional
 * parameter.
 * 
 * Revision 3.16  92/02/14  08:55:27  roman
 * Fix up passing back of output parameters on failure of rfork/rexec/migrate.
 * Particular attention paid to passing back receive right to vproc
 * port.
 * Change server interface to rfork/rexec/migrate to have error output
 * parameter rather than using return value, so that ports can
 * be passed back as output parameters for these routines.
 * 
 * Revision 3.15  92/01/28  16:24:27  roman
 * Add extra current working directory and root directory ports to interface
 * to rexecve(). Deallocate these ports when necessary.
 * 
 * Revision 3.14  92/01/17  11:09:03  chrisp
 * Remove superfluous "session leader on member node" reference counting.
 * 
 * Revision 3.13  92/01/16  11:46:29  chrisp
 * Add VPROC_HOLD for session leader on new node of member if the migrating/
 * rexec'ing process is a pgrp leader.
 * 
 * Revision 3.12  92/01/15  16:51:42  roman
 * Get rid of extraneous rval parameter for server-to-server MiG routines. The
 * return value of the routine is sufficient.
 * Numerous small formatting fixes to make the code consistent throughout the
 * file.
 * 
 * Revision 3.11  92/01/15  10:28:55  chrisp
 * Add extra increment for origin node vproc in rfork().
 * 
 * Revision 3.10  92/01/14  13:23:08  yazz
 * After tnc_install_vproc_port renames a port, use the new name, instead
 * of the old for insert-right operations on migrate and rexecve operations.
 * Include insert-right operation in rexecve where it had been missing.
 * 
 * Revision 3.9  92/01/10  16:14:38  yazz
 * Fixed typo.
 * 
 * Revision 3.8  92/01/10  15:51:56  yazz
 * Fixed typo.
 * 
 * Revision 3.7  92/01/10  15:11:49  yazz
 * Credentials (.74 version) merge.  Remote tasking operations can now fail
 * and have the passed Rcv rts returned to the originating node where they
 * are properly re-installed.  Fewer task-to-task and node-to-node port
 * right transfers take place, for efficiency.  Passing of port names
 * replaces passing of actual ports in some cases to achieve this.
 * 
 * Revision 3.6  92/01/10  10:14:57  roman
 * Get rid of insertion of task port into the new task that was created on
 * rfork/rexec/migrate. This was already being done in
 * rtask_svr_pproc.c.
 * Call rexecve_pproc_exec() (new routine) at appropriate time, and change
 * parameters in call to rexecve_pproc_fix().
 * Change code to set rval where appropriate, rather than setting the
 * error variable ("error" is used for Mach internal-type errors).
 * 
 * Revision 3.5  92/01/07  18:26:31  roman
 * Make sure that the mmap structures used by rfork/rexec/migrate are declared
 * and passed around correctly.
 * 
 * Revision 3.4  92/01/03  11:14:54  chrisp
 * Has controlling terminal flag bit set in pvp_flag after rexec/migrate.
 * 
 * Revision 3.3  91/12/24  10:32:15  roman
 * Initialize pvproc structure using dpvproc_struct_init() rather than bzero().
 * Give away send rights to vproc port during rfork() using
 * tnc_bestow_vproc_port().
 * 
 * Revision 3.2  91/12/19  11:10:51  chrisp
 * Set-up transferred zombie and stopped status for migrate and rexec.
 * 
 * Revision 3.1  91/12/16  09:32:28  roman
 * Chnages to get rexec to work for the first time.
 * 
 * Revision 3.0  91/12/13  09:15:24  roman
 * Initial submission. This file contains the vproc code for the server
 * side of rfork/rexec/migrate.
 * 
 *
 */

#include <sys/errno.h>
#include <sys/syscall.h>
#include <tnc/rtask.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <sys/syscall.h>
#include <uxkern/proc_to_task.h>

#include <mach/thread_status.h>

#ifdef NX
#include <i860paragon/mcmsg/mcmsg_info.h>


#if __STDC__ == 1

extern int nx_put_info(struct pvproc  *pvp,
                       LP_MAP_T       nodelist,
                       int            nodelistcnt,
                       APPLINFO_T    *applinfo_p,
                       pid_t    pid);
#else

extern int nx_put_info();

#endif /* __STDC__ */

#endif /* NX */

extern struct vproc_ops 	dvproc_ops_table;
extern struct pvproc_ops_vector	dpvproc_ops_table;
extern struct vproc 		*vproc_alloc();
extern void			dvp_child_join_pgrp_end();

int
rfork_server(
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		pp_task,		/* parent task */
	task_t		*ch_taskp,		/* OUT */
	thread_t	*ch_threadp,		/* OUT */
	mach_port_t	*ch_vproc_portp,	/* OUT */
	pid_t		*ch_pidp,		/* OUT */
	thread_state_t	ch_state,		/* state of parent */
	unsigned int	ch_state_count,
	mach_port_t	vproc_port_name,	/* name within emulator */
	mach_port_t	cred_port_name,		/* name within emulator */
	struct rf_data	*rf_data,		/* inherited data */
	struct mmap_struct mmap_structs[],	/* memory mapped file data */
	unsigned int	mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*command_name,		/* command name string */
	char		*logname		/* login name string */
#ifdef NX
,       APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
#ifdef PARACORE
,	mach_port_t	exec_rdir_port,		/* exec root directory */
	mach_port_t	exec_cdir_port,		/* exec current directory */
	char		*exec_prg_name		/* exec program name */
#endif /* PARACORE */
)
{
	struct vproc	*v;			/* very temporary vproc ptr */
	struct vproc	*vc;			/* ptr to child's vproc */
	struct vproc	*vp;			/* ptr to parent's vproc */
	struct pvproc	*pvc;			/* ptr to child's pvproc */
	struct proc	*ch_procp;
	int		hashidx;
	int		error;
	kern_return_t	ret;
#ifdef NX
	boolean_t       send_sigtrap;
#endif /* NX */

	/*
	 *  Initialize the out variables to something sane.
	 */
	*ch_taskp = (task_t) MACH_PORT_NULL;
	*ch_threadp = (thread_t) MACH_PORT_NULL;
	*ch_vproc_portp = MACH_PORT_NULL;
	*ch_pidp = (pid_t) 0;

	/*
	 * Allocate a vproc (and its associated pvproc) for the child.
	 * If there's not enough memory we better find out about it before
	 * we do much more.
	 */
	if ((vc = vproc_alloc()) == NULL) {
		error = ENOMEM;
		goto out;
	}

	*ch_pidp = dvp_pidgen();	/* generate the child's pid */

	/*
	 * Call the physical layer first, to fill in our own "dummy"
	 * proc and user structures with info from the real parent
	 * on another node.  We don't deal with the physical layer's
	 * data structures here (except for a single pointer to that
	 * layer's structures), just the data structures of the vproc +
	 * pvproc (virtual) layer.
	 */
        rfork_pproc_unload_msg(NULL, rf_data, pp_task,
			       mmap_structs, mmap_pagers, mmap_array_size,
			       command_name, logname
#ifdef PARACORE
			       , exec_prg_name
#endif /* PARACORE */
			       );

	/*
 	 * Now call the physical layer again to do the actual physical
	 * fork.  If that's successful then we can do the virtual layer
	 * of forking.  (If that's not successful, the physical layer
	 * undoes whatever it has done and returns failure.  In that
	 * case, we only have the vproc_alloc call above to undo.)
	 *
	 * Note that pointers to the child's task port and thread port
	 * are passed so that the physical layer can pass this information
	 * back to the client.
	 *
	 * A brand new credentials port is set up by the fork mechanism
	 * and a send right to it is inserted into the child task using
	 * the name we provide.  This is the same name that the parent
	 * process (whose memory is being inherited) had for the cred port.
	 */
	error = rtask_pproc_fork(*ch_pidp, &ch_procp,
				 ch_state, ch_state_count,
				 rdir_port, cdir_port,
				 ch_taskp, ch_threadp,
				 NULL, cred_port_name,
				 (int *)NULL, 0
#ifdef PARACORE
				 , exec_rdir_port, exec_cdir_port
#endif /* PARACORE */
				 );
	if (error != ESUCCESS) {
		vproc_dealloc(vc);	/* failure.  undo what we've done */
		goto out;		/* and return the cause */
	}

	pvc = PVP(vc);			/* get address of associated pvproc */
	dpvproc_struct_init(vc);	/* initialize it */
	pvc->pvp_flag |= PV_IS_LOCAL;
	pvc->pvp_flag |= PV_IS_ORIGIN;

	/*
	 * Continue to set up the data structures of the newly created
	 * child's vproc + pvproc.
	 */

	vc->vp_ops = &dvproc_ops_table;
	pvc->pvp_ops = &dpvproc_ops_table;

	/* put the vproc on the vproc hash chain */
	VPROC_LOCK_EXCL(vc,"rfork_server(child)");
	VPROC_LIST_LOCK();
	vc->vp_pid = *ch_pidp;
	v = vproc_hash[hashidx = VPROCPIDHASH(*ch_pidp)];
	vc->vp_hashbwd = NULL;
	vc->vp_hashfwd = v;
	vproc_hash[hashidx] = vc;
	if (v)
		v->vp_hashbwd = vc;
	VPROC_HOLD(vc, "rfork_server(active)");
	VPROC_HOLD(vc, "rfork_server(origin)");
	VPROC_LIST_UNLOCK();

	/*
	 * Allocate a Mach port for the new child's vproc.  We assign
	 * the name of the pointer (vc) that was allocated above.
	 */
	(void) vproc_port_allocate(vc);
	pvc->pvp_flag |= PV_HAS_PORT_RIGHT;
	*ch_vproc_portp = tnc_bestow_vproc_port(vc, 0);

	/*
	 * Insert a send right to the child's vproc port into the child
	 * task itself.  Assign it the same name as its parent (whose
	 * memory the child inherits) knew its own vproc port by.
	 */
	ret = mach_port_insert_right(*ch_taskp,
				     vproc_port_name,	/* name to assign */
				     *ch_vproc_portp,	/* name for Rcv rt */
				     MACH_MSG_TYPE_COPY_SEND);
	if (ret != KERN_SUCCESS) {
		panic("rfork_server: vproc insert_right failed");
	}

	/*
	 * Point the child's pvproc at its underlying pproc (proc.h)
	 * structure.  Then initialize some more fields of the pvproc
	 * structure.
	 */
	pvc->pvp_pproc = ch_procp;	/* as returned by the physical layer */

	/* sort out the immediate family relationships */
	pvc->pvp_head_childl = NULL;		/* child has no children */
	pvc->pvp_ppid = rf_data->rf_p_pid;	/* set child's parent id */
	pvc->pvp_foster_ppid = rf_data->rf_p_pid; /* set foster parent id */
	pvc->pvp_pgid = rf_data->rf_p_pgid;	/* child's pgrp is parent's */
	pvc->pvp_sid = rf_data->rf_p_sid;	/* child's sess is parent's */
	pvc->pvp_pp_sid = rf_data->rf_p_sid;	/* parent's sess is parent's */
	pvc->pvp_pgrp_mem_seqno = rf_data->rf_p_pgrp_mem_seqno;
						/* parent's pgrpsig mem seqno */
	pvc->pvp_pp_pgid = rf_data->rf_p_pgid;	/* parent's pgrp is parent's */

#ifdef NX
        if (rf_data->nx_flags & PV_NX_PARTITIONED) {
	    VPROC_LOCK_FLAG(vc,"svr_rfork(child)");
            pvc->pvp_flag |= rf_data->nx_flags;
	    VPROC_UNLOCK_FLAG(vc,"svr_rfork(child)");

            /*
             *  If we are an NX application then put the appl_info and
             *  the node list into the task.
             */
	    ux_server_thread_blocking();
            ret = nx_put_info(pvc,
                              nodelist,
                              nodelistcnt,
                             &applinfo,
                              *ch_pidp);
	    ux_server_thread_unblocking();
            if (ret != KERN_SUCCESS) {
                panic("rfork: svr_rfork failure nx_put_info ret=0x%x",
                      ret);
            }
            vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                          (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
            nodelist = (LP_MAP_T)NULL;
        }
#endif /* NX */

	/*
	 * Create parent vproc locally, out of passed parent vproc port.
	 * Normally we should now do a VPROC_HOLD(vp) to keep a 
	 * reference to the parent.  Instead we just skip doing
	 * a VPROC_RELEASE(vp) for the LOCATE_VPROC_PID() below.
	 */
	vp = LOCATE_VPROC_PID(rf_data->rf_p_pid);
	if (vp == 0)
		panic("rfork_server: cannot find parent");
	tnc_install_vproc_port(vp, pp_vproc_port, 0);


	/* initialize the process structure with values from the pvproc */
	pproc_set_attr(pvc->pvp_pproc, 
		       vc, 
		       &vc->vp_pid, 
		       &pvc->pvp_ppid, 
		       &pvc->pvp_pgid, 
		       &pvc->pvp_sid,
		       0);

	/*
	 * Have the child join the parent's pgrp.
	 * Normally we should now do a VPROC_HOLD(g) to keep a 
	 * reference to the pgrp leader.  Instead we just skip doing
	 * a VPROC_RELEASE(g) for the LOCATE_VPROC_PID() below.
	 */
	if (pvc->pvp_pgid != 0) {
		struct vproc *g;
		g = LOCATE_VPROC_PID(pvc->pvp_pgid);
		if (g == 0)
			panic("rfork_server: cannot find pgrp leader");
		tnc_install_vproc_port(g, pgrp_vproc_port, 0);
		
		dvp_child_join_pgrp_end(vp, vc, g);	/* join parent's pgrp */
	}
	/*
	 * Set pvproc flag field to indicate whether this process has a
	 * controlling terminal. This is derived from the physical proc.
	 */
	{
		int has_sctty;
		pproc_get_attr(pvc->pvp_pproc,0,0,0,0,0,0,0,&has_sctty,0);
		if (has_sctty) {
			VPROC_LOCK_FLAG(vc,"rfork_server(child)");
			pvc->pvp_flag |= PV_SCTTY;
			VPROC_UNLOCK_FLAG(vc,"rfork_server(child)");
		}
	}
#ifdef NX
        send_sigtrap = nx_trace_inherit(vc, rf_data->rf_p_flag);
#endif /* NX */

	VPROC_UNLOCK_EXCL(vc,"rfork_server(child)");
#ifdef NX
        if (nodelist != (LP_MAP_T)NULL) {
            vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                          (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
        }
	if (send_sigtrap) {
	    (void)VPOP_REPORT_STATE(vc, VPROC_NO_SIGCHLD);
	    error = VPOP_SIGPROC(vc, SIGTRAP, 0, 
				 VPROC_HAS_PRIV | VPROC_CHILD_STOP);
	}
#endif /* NX */
	return(ESUCCESS);

out:
	/*
	 * Here if there's been a failure and we must safely deallocate
	 * all the send rights to family members that we hold.
	 * Note: a check for non-null ports made by the disposal routine.
	 */
	tnc_dispose_vproc_port(rf_data->rf_p_pid, pp_vproc_port);
	tnc_dispose_vproc_port(rf_data->rf_p_pgid, pgrp_vproc_port);
	return(error);
}


int
migrate_server(
	pid_t		cur_pid,		/* pid of migrating process */
	int		is_pgrpleader,		/* proc is pgrp leader */
	mach_port_t	cur_vproc_port,		/* proc's vproc port (Rcv rt) */
	mach_port_t	vproc_port_name,
	mach_port_t	cur_proc_port,		/* proc's proc port (Rcv rt) */
						/* bootstrap pt needs no name */
	mach_port_t	cur_cred_port,		/* proc's cred port (Rcv rt) */
	mach_port_t	cred_port_name,
	int		cur_cred_cache[],	/* proc's cred cache */
	unsigned int	cur_cred_cache_size,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	fpp_vproc_port,		/* foster parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	sess_vproc_port,	/* session leader */
	pid_t		child_pid[],		/* pids of children */
	unsigned int	child_pid_cnt,
	mach_port_t	child_port[],		/* ports of childrens' vprocs */
	unsigned int	child_port_cnt,
	int		child_stat[],		/* stat (ZOMBIE or STOP) ... */
	unsigned int	child_stat_cnt,		/* ... of childrens' vprocs */
	pid_t		foster_child_pid[],	/* pids of children */
	unsigned int	foster_child_pid_cnt,
	mach_port_t	foster_child_port[],	/* ports of foster children */
	unsigned int	foster_child_port_cnt,
	pid_t		pgrp_member_pid[],	/* pids of pgrp members */
	unsigned int	pgrp_member_pid_cnt,
	mach_port_t	pgrp_member_port[],	/* ports of pgrp members' ... */
	unsigned int	pgrp_member_port_cnt,	/* ... vprocs */
	pid_t		sess_member_pid[],	/* pids of session members */
	unsigned int	sess_member_pid_cnt,
	mach_port_t	sess_member_port[],	/* ports of session ... */
	unsigned int	sess_member_port_cnt,	/* ... members' vprocs */
	int		pgrp_jobc,		/* job control count */
	int		cttynode,		/* node of controlling tty */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		old_task,		/* actual task port and... */
	task_t		old_task_name,		/* old emulator's name for it */
	thread_state_t	cur_state,		/* state of thread */
	unsigned int	cur_state_count,
	struct mi_data	*mi_data,		/* proc and u struct fields */
	struct mmap_struct mmap_structs[],	/* mmap areas of proc */
	unsigned int	mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*command_name,		/* command name string */
	char		*logname		/* login name string */
#ifdef NX
,       APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
#ifdef PARACORE
,	mach_port_t	exec_rdir_port,		/* exec root directory */
	mach_port_t	exec_cdir_port,		/* exec current directory */
	char		*exec_prg_name		/* exec program name */
#endif /* PARACORE */
)
{
	struct vproc	*vm;
	struct pvproc	*pvm;
	kern_return_t	ret;
	struct proc	*mi_procp;
	register int	i;
	task_t		new_task;
	mach_port_t	dummy_port;
	int		error;

	/*
	 * Create (if necessary) a vproc on this node for the migrating
	 * process, and install its port at the correct address.
	 */
	vm = LOCATE_VPROC_PID(cur_pid);
	if (vm == 0)
		panic("migrate_server: cannot find migrating vproc");
	pvm = PVP(vm);
	tnc_install_vproc_port(vm, cur_vproc_port, INSTALL_RECEIVE_RT);
	cur_vproc_port = vproc_to_port_lookup(vm); /* may have new name */

	/*
	 * Call the physical layer first, to fill in our own "dummy"
	 * proc and user structures with info from the original process that
	 * is on another node.  We don't deal with the physical layer's
	 * data structures here (except for a single pointer to that
	 * layer's structures), just the data structures of the vproc +
	 * pvproc (virtual) layer.
	 */
	migrate_pproc_unload_msg(NULL, mi_data, old_task, 
			         mmap_structs, mmap_pagers, mmap_array_size,
				 command_name, logname
#ifdef PARACORE
			         , exec_prg_name
#endif /* PARACORE */
				 );

 	/* Now call the physical layer again to do the actual physical
	 * fork.  If that's successful then we can do the virtual layer
	 * of forking.  (If that's not successful, the physical layer
	 * undoes whatever it has done and returns failure.  In that
	 * case, we only have the vproc_alloc call above to undo.)
	 *
	 * Note that pointers to the child's task port and thread port are
	 * passed so that the physical layer can return these port values.
	 * (For migrate, the value of the thread port is not needed.)
	 */
	error = rtask_pproc_fork(cur_pid, &mi_procp, 
				 cur_state, cur_state_count,
				 rdir_port, cdir_port,
				 &new_task, &dummy_port,
				 &cur_cred_port, cred_port_name,
				 cur_cred_cache, cur_cred_cache_size
#ifdef PARACORE
				 , exec_rdir_port, exec_cdir_port
#endif /* PARACORE */
				 );
	if (error != ESUCCESS) {
#ifdef NX
                if (nodelist != (LP_MAP_T)NULL) {
                    vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                                  (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                }
#endif /* NX */
		goto out;
	 }

	/*
	 * Insert a send right to the migrated proc's vproc port into the
	 * migrated task itself.  Assign it the same name as its parent
	 * (whose memory the new task inherits) knew its own vproc port by.
	 */
	ret = mach_port_insert_right(new_task,
				     vproc_port_name,	/* name to assign */
				     cur_vproc_port,	/* name for Rcv rt */
				     MACH_MSG_TYPE_MAKE_SEND);
	if (ret != KERN_SUCCESS)
		panic("migrate_server: vproc insert_right failed");

	VPROC_LOCK_EXCL(vm, "migrate_server");

	/*
	 * Point the child's pvproc at its underlying pproc (proc.h)
	 * structure.  Then initialize some more fields of the pvproc
	 * structure.
	 */
	pvm->pvp_pproc = mi_procp;	/* as returned by the physical layer */

	/*
	 * Initialize the port operation sequence counts.
	 */
	pvm->pvp_movement_lock.ml_proc_sequence = 0;
	pvm->pvp_movement_lock.ml_vproc_sequence = 0;

	/* sort out the immediate family relationships */
	pvm->pvp_head_childl = NULL;		/* ignore children for now */
	pvm->pvp_ppid = mi_data->mi_p_ppid;	/* set migratee's parent id */
	pvm->pvp_foster_ppid = mi_data->mi_p_foster_ppid; /* foster parent id */
	pvm->pvp_pgid = mi_data->mi_p_pgid;	/* set migratee's pgrp id */
	pvm->pvp_sid = mi_data->mi_p_sid;	/* set migratee's sess id */
	pvm->pvp_pgrp_mem_seqno = mi_data->mi_p_pgrp_mem_seqno;
						/* current pgrp sig mem seqno */
	pvm->pvp_pgrp_ldr_seqno = mi_data->mi_p_pgrp_ldr_seqno;
						/* current pgrp sig ldr seqno */
	pvm->pvp_pp_pgid = mi_data->mi_pp_pgid;	/* set migratee's par's pgid */
	pvm->pvp_pp_sid = mi_data->mi_pp_sid;	/* set migratee's par's sess */

#ifdef NX
        if (mi_data->nx_flags & PV_NX_PARTITIONED) {

            ret = nx_put_info(pvm,
                              nodelist,
                              nodelistcnt,
                             &applinfo,
                              cur_pid);
            if (ret != KERN_SUCCESS) {
                panic("migrate: svr_migrate failure nx_put_info ret=0x%x",
                      ret);
            } else {
		VPROC_LOCK_FLAG(vm,"svr_migrate(child)");
                pvm->pvp_flag |= mi_data->nx_flags;
		VPROC_UNLOCK_FLAG(vm,"svr_migrate(child)");
            }
            vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                          (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
            nodelist = (LP_MAP_T) NULL;
        }
#endif /* NX */

	/*
	 * Create migratee's parent vproc locally.
	 * Normally we should now do a VPROC_HOLD(vp) to keep a 
	 * reference to the parent.  Instead we just skip doing
	 * a VPROC_RELEASE(vp) for the LOCATE_VPROC_PID() below.
	 */
	{
		struct vproc *vp = LOCATE_VPROC_PID(pvm->pvp_ppid);
		if (vp == 0)
			panic("migrate_server: cannot find parent");
		tnc_install_vproc_port(vp, pp_vproc_port, 0);
	}

	/*
	 * Create migratee's foster parent vproc locally.
	 * Normally we should now do a VPROC_HOLD(vp) to keep a 
	 * reference to the foster parent.  Instead we just skip doing
	 * a VPROC_RELEASE(vp) for the LOCATE_VPROC_PID() below.
	 */
	if (pvm->pvp_foster_ppid != 0 && pvm->pvp_foster_ppid != pvm->pvp_ppid){
		struct vproc *fvp = LOCATE_VPROC_PID(pvm->pvp_foster_ppid);
		if (fvp == 0)
			panic("migrate_server: cannot find foster parent");
		tnc_install_vproc_port(fvp, fpp_vproc_port, 0);
	}

	/*
	 * Have the migrating process join its pgrp.
	 * Normally we should now do a VPROC_HOLD(g) to keep a 
	 * reference to the pgrp leader.  Instead we just skip doing
	 * a VPROC_RELEASE(g) for the LOCATE_VPROC_PID() below.
	 */
	if (pvm->pvp_pgid != 0) {
		struct vproc *g = LOCATE_VPROC_PID(pvm->pvp_pgid);
		if (g == 0)
			panic("migrate_server: cannot find pgrp leader");
		tnc_install_vproc_port(g, pgrp_vproc_port, 0);
	}

	/*
	 * If a pgrp leader, keep a reference to the session leader.
	 * Normally we should now do a VPROC_HOLD(s) to keep a 
	 * reference to the session leader.  Instead we just skip doing
	 * a VPROC_RELEASE(s) for the LOCATE_VPROC_PID() below.
	 */
	if (is_pgrpleader && pvm->pvp_sid != 0) {
		struct vproc *s = LOCATE_VPROC_PID(pvm->pvp_sid);
		if (s == 0)
			panic("migrate_server: cannot find session leader");
		tnc_install_vproc_port(s, sess_vproc_port, 0);
	}

	/*
	 * Establish the parent-child-sibling chain on this node, using
	 * the list of children that was passed in.
	 * Normally we should now do a VPROC_HOLD(vc) to keep a 
	 * reference to each child.  Instead we just skip doing
	 * a VPROC_RELEASE(vc) for the LOCATE_VPROC_PID() below.
	 */
	for (i = 0; i < child_pid_cnt; i++) {
		struct vproc *vc = LOCATE_VPROC_PID(child_pid[i]);
		if (vc == 0)
			panic("migrate_server: cannot find child");
		tnc_install_vproc_port(vc, child_port[i], 0);
		VPROC_LOCK_FLAG(vc,"migrate_server(child)");
		PVP(vc)->pvp_flag &= ~(PV_SZOMB|PV_SSTOP);
		PVP(vc)->pvp_flag |= child_stat[i];
		VPROC_UNLOCK_FLAG(vc,"migrate_server(child)");
		PVP(vc)->pvp_childl = pvm->pvp_head_childl;
		pvm->pvp_head_childl = vc;
	}

	/*
	 * Establish the foster children chain on this node, using
	 * the list of children that was passed in.
	 * Normally we should now do a VPROC_HOLD(vc) to keep a 
	 * reference to each child.  Instead we just skip doing
	 * a VPROC_RELEASE(vc) for the LOCATE_VPROC_PID() below.
	 */
	for (i = 0; i < foster_child_pid_cnt; i++) {
		struct vproc *vc = LOCATE_VPROC_PID(foster_child_pid[i]);
		if (vc == 0)
			panic("migrate_server: cannot find foster child");
		tnc_install_vproc_port(vc, foster_child_port[i], 0);
		VPROC_LOCK_EXCL(vc, "migrate_server(foster child)");
		PVP(vc)->pvp_foster_childl = pvm->pvp_head_foster_childl;
		pvm->pvp_head_foster_childl = vc;
		VPROC_UNLOCK_EXCL(vc, "migrate_server(foster child)");
	}

	/*
	 * Establish the pgrp list on this node, using
	 * the list of pgrp members that was passed in.
	 * Normally we should now do a VPROC_HOLD(vg) to keep a 
	 * reference to each pgrp member.  Instead we just skip doing
	 * a VPROC_RELEASE(vg) for the LOCATE_VPROC_PID() below.
	 */
	if (is_pgrpleader) {
		VPROC_LOCK_PGRP_LIST_EXCL(vm, "migrate_server");
		VPROC_LOCK_FLAG(vm,"migrate_server");
		pvm->pvp_flag |= PV_PGRPLEADER;
		VPROC_UNLOCK_FLAG(vm,"migrate_server");
		pvm->pvp_jobc = pgrp_jobc;
		for (i = 0; i < pgrp_member_pid_cnt; i++) {
			struct vproc *vg = LOCATE_VPROC_PID(pgrp_member_pid[i]);
			if (vg == 0)
				panic("migrate_server: cannot find pgrp memb");
			tnc_install_vproc_port(vg, pgrp_member_port[i], 0);
			PVP(vg)->pvp_pgrpl = pvm->pvp_head_pgrpl;
			pvm->pvp_head_pgrpl = vg;
			VPROC_HOLD(vm, "migrate_server(pgrp ldr on ldr node)");
		}
		VPROC_UNLOCK_PGRP_LIST_EXCL(vm, "migrate_server");
	}

	/*
	 * Establish the session list on this node, using
	 * the list of session members that was passed in.
	 * Normally we should now do a VPROC_HOLD(vs) to keep a 
	 * reference to each session member.  Instead we just skip doing
	 * a VPROC_RELEASE(vs) for the LOCATE_VPROC_PID() below.
	 */
	if (pvm->pvp_sid == vm->vp_pid) {
		VPROC_LOCK_SESSION_LIST_EXCL(vm, "migrate_server");
		VPROC_LOCK_FLAG(vm,"migrate_server");
		pvm->pvp_flag |= PV_SESSIONLEADER;
		VPROC_UNLOCK_FLAG(vm,"migrate_server");
		pvm->pvp_cttynode = cttynode;
		for (i = 0; i < sess_member_pid_cnt; i++) {
			struct vproc *vs = LOCATE_VPROC_PID(sess_member_pid[i]);
			if (vs == 0)
				panic("migrate_server: cannot find sess memb");
			tnc_install_vproc_port(vs, sess_member_port[i], 0);
			PVP(vs)->pvp_sessionl = pvm->pvp_sessionl;
			pvm->pvp_sessionl = vs;
			VPROC_HOLD(vm, "migrate_server(sess ldr on ldr node)");
		}
		VPROC_UNLOCK_SESSION_LIST_EXCL(vm, "migrate_server");
	}

	/* initialize the process structure with values from the pvproc */
	pproc_set_attr(pvm->pvp_pproc, 
		       vm, 
		       &vm->vp_pid, 
		       &pvm->pvp_ppid, 
		       &pvm->pvp_pgid, 
		       &pvm->pvp_sid,
		       0);

	/*
	 * Set pvproc flag field to indicate whether this process has a
	 * controlling terminal. This is derived from the physical proc.
	 */
	{
		int has_sctty;
		pproc_get_attr(pvm->pvp_pproc,0,0,0,0,0,0,0,&has_sctty,0);
		if (has_sctty) {
			VPROC_LOCK_FLAG(vm,"migrate_server");
			pvm->pvp_flag |= PV_SCTTY;
			VPROC_UNLOCK_FLAG(vm,"migrate_server");
		}
	}

	/*
	 * Now just fix up the process that was created above.  This involves
	 * initializing fields that were zeroed by pproc_fork() and setting
	 * up the process port correctly.  The fix routine also starts up the 
	 * emulator, so it should be called this late.
	 */
	migrate_pproc_fix(mi_procp, mi_data, 
			  old_task, old_task_name,
			  &cur_proc_port);

	/*
	 * Set up the current vproc as a vproc with local vproc ops, and
	 * allow them to be serviced. Do this absolutely last to
	 * prevent pvproc ops until the other setup is complete.
	 */
	VPROC_LOCK_FLAG(vm,"migrate_server");
	pvm->pvp_flag &= ~(PV_SZOMB | PV_SSTOP);
	pvm->pvp_flag |= PV_IS_LOCAL;
	VPROC_UNLOCK_FLAG(vm,"migrate_server");
	pvm->pvp_ops = &dpvproc_ops_table;
	ux_server_add_port(vproc_to_port_lookup(vm));

	VPROC_UNLOCK_EXCL(vm, "migrate_server");
#ifdef NX
        if (nodelist != (LP_MAP_T)NULL) {
            vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                          (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
        }
#endif /* NX */
	return(ESUCCESS);

out:
	/*
	 * Here if there's been a failure and we must safely deallocate
	 * all the send rights to family members that we hold.
	 * Note: a check for non-null ports made by the disposal routine.
	 */
	tnc_dispose_vproc_port(mi_data->mi_p_ppid, pp_vproc_port);
	tnc_dispose_vproc_port(mi_data->mi_p_pgid, pgrp_vproc_port);
	tnc_dispose_vproc_port(mi_data->mi_p_foster_ppid, fpp_vproc_port);
	tnc_dispose_vproc_port(mi_data->mi_p_sid, sess_vproc_port);
	tnc_dispose_vproc_port_array(child_pid, child_port, child_pid_cnt);
	tnc_dispose_vproc_port_array(foster_child_pid, foster_child_port,
				     foster_child_pid_cnt);
	tnc_dispose_vproc_port_array(pgrp_member_pid, pgrp_member_port,
				     pgrp_member_pid_cnt);
	tnc_dispose_vproc_port_array(sess_member_pid, sess_member_port,
				     sess_member_pid_cnt);
	return(error);
}


int
rexecve_server(
	char		*fname,			/* file we are exec-ing */
	unsigned int	fname_count,		/* size of above file name */
	vm_offset_t	arg_addr,		/* parameters for later use */
	vm_size_t	arg_size,
	int		arg_count,
	int		env_count,
	unsigned int	char_count,
	pid_t		cur_pid,		/* pid of migrating process */
	int		is_pgrpleader,		/* proc is pgrp leader */
	mach_port_t	cur_vproc_port,		/* process vproc port */
	mach_port_t	vproc_port_name,
	mach_port_t	cur_proc_port,		/* process port */
	mach_port_t	cur_cred_port,		/* process cred port */
	mach_port_t	cred_port_name,
	int		cur_cred_cache[],	/* proc's cred cache */
	unsigned int	cur_cred_cache_size,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	fpp_vproc_port,		/* foster parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	sess_vproc_port,	/* session leader */
	pid_t		child_pid[],		/* pids of children */
	unsigned int	child_pid_cnt,
	mach_port_t	child_port[],		/* ports of childrens' vprocs */
	unsigned int	child_port_cnt,
	int		child_stat[],		/* stat (ZOMBIE or STOP) ... */
	unsigned int	child_stat_cnt,		/* ... of childrens' vprocs */
	pid_t		foster_child_pid[],	/* pids of children */
	unsigned int	foster_child_pid_cnt,
	mach_port_t	foster_child_port[],	/* ports of foster children */
	unsigned int	foster_child_port_cnt,
	pid_t		pgrp_member_pid[],	/* pids of pgrp members */
	unsigned int	pgrp_member_pid_cnt,
	mach_port_t	pgrp_member_port[],	/* ports of pgrp members' ... */
	unsigned int	pgrp_member_port_cnt,	/* ... vprocs */
	pid_t		sess_member_pid[],	/* pids of session members */
	unsigned int	sess_member_pid_cnt,
	mach_port_t	sess_member_port[],	/* ports of session ... */
	unsigned int	sess_member_port_cnt,	/* ... members' vprocs */
	int		pgrp_jobc,		/* job control count */
	int		cttynode,		/* node of controlling tty */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		old_task,
	task_t		old_task_name,		/* just a name, not a right */
	thread_state_t	cur_state,		/* state of thread */
	unsigned int	cur_state_count,
	struct re_data	*re_data,		/* proc and u struct fields */
	struct mmap_struct mmap_structs[],	/* mmap areas of proc */
	unsigned int	mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*logname		/* login name string */
#ifdef NX
,       APPLINFO_T       applinfo,              /* Application info  */
        LP_MAP_T         nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
)
{
	struct vproc	*ve;
	struct pvproc	*pve;
	struct proc	*re_procp;
	register int	i;
	mach_port_t	dummy_port;
	task_t		new_task;
	kern_return_t	ret;
	int		error;

	/*
	 * Create (if necessary) a vproc on this node for the rexecing
	 * process, and install its port at the correct address.
	 */
	ve = LOCATE_VPROC_PID(cur_pid);
	if (ve == 0)
		panic("rexecve_server: cannot find rexec vproc");
	pve = PVP(ve);
	tnc_install_vproc_port(ve, cur_vproc_port, INSTALL_RECEIVE_RT);
	cur_vproc_port = vproc_to_port_lookup(ve); /* may have new name */

	/*
	 * Call the physical layer first, to fill in our own "dummy"
	 * proc and user structures with info from the original process that
	 * is on another node.  We don't deal with the physical layer's
	 * data structures here (except for a single pointer to that
	 * layer's structures), just the data structures of the vproc +
	 * pvproc (virtual) layer.
	 */
	rexecve_pproc_unload_msg(NULL, re_data, old_task, 
			         mmap_structs, mmap_pagers, mmap_array_size,
				 logname);

 	/* Now call the physical layer again to do the actual physical
	 * fork.  If that's successful then we can do the virtual layer
	 * of forking.  (If that's not successful, the physical layer
	 * undoes whatever it has done and returns failure.  In that
	 * case, we only have the vproc_alloc call above to undo.)
	 *
	 * Note that pointers to the child's task port and thread port
	 * are passed so that the physical layer can pass this information 
	 * back to the client.
	 */
	error = rtask_pproc_fork(cur_pid, &re_procp, 
				 cur_state, cur_state_count,
				 rdir_port, cdir_port,
				 &new_task, &dummy_port,
				 &cur_cred_port, cred_port_name,
				 cur_cred_cache, cur_cred_cache_size
#ifdef PARACORE
	/*
	 * uu_exec_utnd will be set later via execve().
	 */
				 , MACH_PORT_NULL, MACH_PORT_NULL
#endif /* PARACORE */
				 );
	if (error != ESUCCESS) {
#ifdef NX
                if (nodelist != (LP_MAP_T)NULL) {
                    vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                                  (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                }
#endif /* NX */
		goto out;
	}

	/*
	 * Insert a send right to the rexecve'd proc's vproc port into the
	 * newly-created task itself.  Assign it the same name as its parent
	 * (whose memory the new task inherits) knew its own vproc port by.
	 */
	ret = mach_port_insert_right(new_task,
				     vproc_port_name,
				     vproc_to_port_lookup(ve),
				     MACH_MSG_TYPE_MAKE_SEND);
	if (ret != KERN_SUCCESS)
		panic("rexecve_server: vproc insert_right failed");

	/*
	 * Now exec on top of the process that was created above.  This 
	 * involves initializing fields that were zeroed by pproc_fork() and 
	 * also setting up the process port correctly.  This also calls 
	 * execve to do the real exec work.
	 */
	error = rexecve_pproc_exec(re_procp, 
				   fname, fname_count,
				   arg_addr, arg_size,
				   arg_count, env_count, char_count,
				   re_data, 
				   old_task, old_task_name,
				   &cur_proc_port);
	if (error != ESUCCESS) {
		rtask_pproc_remove(re_procp);
		goto out;
	}

	VPROC_LOCK_EXCL(ve, "rexecve_server");

	/*
	 * Point the child's pvproc at its underlying pproc (proc.h)
	 * structure.  Then initialize some more fields of the pvproc
	 * structure.
	 */
	pve->pvp_pproc = re_procp;	/* as returned by the physical layer */

	/*
	 * Initialize the port operation sequence counts.
	 */
	pve->pvp_movement_lock.ml_proc_sequence = 0;
	pve->pvp_movement_lock.ml_vproc_sequence = 0;

	/* sort out the immediate family relationships */
	pve->pvp_head_childl = NULL;		/* ignore children for now */
	pve->pvp_ppid = re_data->re_p_ppid;	/* set rexecee's parent id */
	pve->pvp_foster_ppid = re_data->re_p_foster_ppid; /* foster parent id */
	pve->pvp_pgid = re_data->re_p_pgid;	/* set rexecee's pgrp id */
	pve->pvp_sid = re_data->re_p_sid;	/* set rexecee's sess id */
	pve->pvp_pgrp_mem_seqno = re_data->re_p_pgrp_mem_seqno;
						/* current pgrp sig mem seqno */
	pve->pvp_pgrp_ldr_seqno = re_data->re_p_pgrp_ldr_seqno;
						/* current pgrp sig ldr seqno */
	pve->pvp_pp_pgid = re_data->re_pp_pgid;	/* set rexecee's par's pgrp */
	pve->pvp_pp_sid = re_data->re_pp_sid;	/* set rexecee's par's sess */

#ifdef NX
        if (re_data->nx_flags & PV_NX_PARTITIONED) {
            /*
             *  If we are an NX application then put the appl_info and
             *  the node list into the task.
             */
            ret = nx_put_info(pve,
                              nodelist,
                              nodelistcnt,
                             &applinfo,
                              cur_pid);
            if (ret != KERN_SUCCESS) {
                panic("rexec: svr_rexecve failure nx_put_info ret=0x%x",
                      ret);
            } else {
		VPROC_LOCK_FLAG(ve,"svr_rexecve(child)");
                pve->pvp_flag |= re_data->nx_flags;
		VPROC_UNLOCK_FLAG(ve,"svr_rexecve(child)");
            }
            vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                          (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));

            nodelist = (LP_MAP_T) NULL;
        }
#endif /* NX */

	/*
	 * Create rexecee's parent vproc locally.
	 * Normally we should now do a VPROC_HOLD(vp) to keep a 
	 * reference to the parent.  Instead we just skip doing
	 * a VPROC_RELEASE(vp) for the LOCATE_VPROC_PID() below.
	 */
	{
		struct vproc *vp = LOCATE_VPROC_PID(pve->pvp_ppid);
		if (vp == 0)
			panic("rexecve_server: cannot find parent");
		tnc_install_vproc_port(vp, pp_vproc_port, 0);
	}

	/*
	 * Create rexecee's foster parent vproc locally.
	 * Normally we should now do a VPROC_HOLD(vp) to keep a 
	 * reference to the foster parent.  Instead we just skip doing
	 * a VPROC_RELEASE(vp) for the LOCATE_VPROC_PID() below.
	 */
	if (pve->pvp_foster_ppid != 0 && pve->pvp_foster_ppid != pve->pvp_ppid){
		struct vproc *fvp = LOCATE_VPROC_PID(pve->pvp_foster_ppid);
		if (fvp == 0)
			panic("rexecve_server: cannot find foster parent");
		tnc_install_vproc_port(fvp, fpp_vproc_port, 0);
	}

	/*
	 * Have the migrating process join its pgrp.
	 * Normally we should now do a VPROC_HOLD(g) to keep a 
	 * reference to the pgrp leader.  Instead we just skip doing
	 * a VPROC_RELEASE(g) for the LOCATE_VPROC_PID() below.
	 */
	if (pve->pvp_pgid != 0) {
		struct vproc *g = LOCATE_VPROC_PID(pve->pvp_pgid);
		if (g == 0)
			panic("rexecve_server: cannot find pgrp leader");
		tnc_install_vproc_port(g, pgrp_vproc_port, 0);
	}

	/*
	 * If a pgrp leader, keep a reference to the session leader.
	 * Normally we should now do a VPROC_HOLD(s) to keep a 
	 * reference to the session leader.  Instead we just skip doing
	 * a VPROC_RELEASE(s) for the LOCATE_VPROC_PID() below.
	 */
	if (is_pgrpleader && pve->pvp_sid != 0) {
		struct vproc *s = LOCATE_VPROC_PID(pve->pvp_sid);
		if (s == 0)
			panic("rexecve_server: cannot find session leader");
		tnc_install_vproc_port(s, sess_vproc_port, 0);
	}

	/*
	 * Establish the parent-child-sibling chain on this node, using
	 * the list of children that was passed in.
	 * Normally we should now do a VPROC_HOLD(vc) to keep a 
	 * reference to each child.  Instead we just skip doing
	 * a VPROC_RELEASE(vc) for the LOCATE_VPROC_PID() below.
	 */
	for (i = 0; i < child_pid_cnt; i++) {
		struct vproc *vc = LOCATE_VPROC_PID(child_pid[i]);
		if (vc == 0)
			panic("rexecve_server: cannot find child");
		tnc_install_vproc_port(vc, child_port[i], 0);
		VPROC_LOCK_FLAG(vc, "rexecve_server(child)");
		PVP(vc)->pvp_flag &= ~(PV_SZOMB|PV_SSTOP);
		PVP(vc)->pvp_flag |= child_stat[i];
		VPROC_UNLOCK_FLAG(vc, "rexecve_server(child)");
		PVP(vc)->pvp_childl = pve->pvp_head_childl;
		pve->pvp_head_childl = vc;
	}

	/*
	 * Establish the foster children chain on this node, using
	 * the list of children that was passed in.
	 * Normally we should now do a VPROC_HOLD(vc) to keep a 
	 * reference to each child.  Instead we just skip doing
	 * a VPROC_RELEASE(vc) for the LOCATE_VPROC_PID() below.
	 */
	for (i = 0; i < foster_child_pid_cnt; i++) {
		struct vproc *vc = LOCATE_VPROC_PID(foster_child_pid[i]);
		if (vc == 0)
			panic("rexecve_server: cannot find foster child");
		tnc_install_vproc_port(vc, foster_child_port[i], 0);
		VPROC_LOCK_EXCL(vc, "rexecve_server(foster child)");
		PVP(vc)->pvp_foster_childl = pve->pvp_head_foster_childl;
		pve->pvp_head_foster_childl = vc;
		VPROC_UNLOCK_EXCL(vc, "rexecve_server(foster child)");
	}

	/*
	 * Establish the pgrp list on this node, using
	 * the list of pgrp members that was passed in.
	 * Normally we should now do a VPROC_HOLD(vg) to keep a 
	 * reference to each pgrp member.  Instead we just skip doing
	 * a VPROC_RELEASE(vg) for the LOCATE_VPROC_PID() below.
	 */
	if (is_pgrpleader) {
		VPROC_LOCK_PGRP_LIST_EXCL(ve, "rexecve_server");
		VPROC_LOCK_FLAG(ve, "rexecve_server");
		pve->pvp_flag |= PV_PGRPLEADER;
		VPROC_UNLOCK_FLAG(ve, "rexecve_server");
		pve->pvp_jobc = pgrp_jobc;
		for (i = 0; i < pgrp_member_pid_cnt; i++) {
			struct vproc *vg = LOCATE_VPROC_PID(pgrp_member_pid[i]);
			if (vg == 0)
				panic("rexecve_server: cannot find pgrp memb");
			tnc_install_vproc_port(vg, pgrp_member_port[i], 0);
			PVP(vg)->pvp_pgrpl = pve->pvp_head_pgrpl;
			pve->pvp_head_pgrpl = vg;
			VPROC_HOLD(ve, "rexecve_server(pgrp ldr on ldr node)");
		}
		VPROC_UNLOCK_PGRP_LIST_EXCL(ve, "rexecve_server");
	}

	/*
	 * Establish the session list on this node, using
	 * the list of session members that was passed in.
	 * Normally we should now do a VPROC_HOLD(vs) to keep a 
	 * reference to each session member.  Instead we just skip doing
	 * a VPROC_RELEASE(vs) for the LOCATE_VPROC_PID() below.
	 */
	if (pve->pvp_sid == ve->vp_pid) {
		VPROC_LOCK_SESSION_LIST_EXCL(ve, "rexecve_server");
		VPROC_LOCK_FLAG(ve, "rexecve_server");
		pve->pvp_flag |= PV_SESSIONLEADER;
		VPROC_UNLOCK_FLAG(ve, "rexecve_server");
		pve->pvp_cttynode = cttynode;
		for (i = 0; i < sess_member_pid_cnt; i++) {
			struct vproc *vs = LOCATE_VPROC_PID(sess_member_pid[i]);
			if (vs == 0)
				panic("rexecve_server: cannot find sess memb");
			tnc_install_vproc_port(vs, sess_member_port[i], 0);
			PVP(vs)->pvp_sessionl = pve->pvp_sessionl;
			pve->pvp_sessionl = vs;
			VPROC_HOLD(ve, "rexecve_server(sess ldr on ldr node)");
		}
		VPROC_UNLOCK_SESSION_LIST_EXCL(ve, "rexecve_server");
	}

	/* initialize the process structure with values from the pvproc */
	pproc_set_attr(pve->pvp_pproc, 
		       ve, 
		       &ve->vp_pid, 
		       &pve->pvp_ppid, 
		       &pve->pvp_pgid, 
		       &pve->pvp_sid,
		       0);

	/*
	 * Set pvproc flag field to indicate whether this process has a
	 * controlling terminal. This is derived from the physical proc.
	 */
	{
		int has_sctty;
		pproc_get_attr(pve->pvp_pproc,0,0,0,0,0,0,0,&has_sctty,0);
		if (has_sctty) {
			VPROC_LOCK_FLAG(ve,"rexecve_server");
			pve->pvp_flag |= PV_SCTTY;
			VPROC_UNLOCK_FLAG(ve,"rexecve_server");
		}
	}

	/*
	 * Now do any final fixup on the exec'ed process. This probably
	 * involves starting the process up. We delay it until now to allow
	 * full setup of the vproc environment.
	 */
	rexecve_pproc_fix(re_procp);

	/*
	 * Set up the current vproc as a vproc with local vproc ops, and
	 * allow them to be serviced. Do this absolutely last to
	 * prevent pvproc ops until the other setup is complete.
	 */
	VPROC_LOCK_FLAG(ve,"rexecve_server");
	pve->pvp_flag &= ~(PV_SZOMB | PV_SSTOP);
	pve->pvp_flag |= PV_IS_LOCAL;
	VPROC_UNLOCK_FLAG(ve,"rexecve_server");
	pve->pvp_ops = &dpvproc_ops_table;
	ux_server_add_port(vproc_to_port_lookup(ve));

	VPROC_UNLOCK_EXCL(ve, "rexecve_server");
#ifdef NX
        if (nodelist != (LP_MAP_T)NULL) {
            vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                          (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
        }
#endif /* NX */
	return(ESUCCESS);

out:
	/*
	 * Here if there's been a failure and we must safely deallocate
	 * all the send rights to family members that we hold.
	 * Note: a check for non-null ports made by the disposal routine.
	 */
	tnc_dispose_vproc_port(re_data->re_p_ppid, pp_vproc_port);
	tnc_dispose_vproc_port(re_data->re_p_pgid, pgrp_vproc_port);
	tnc_dispose_vproc_port(re_data->re_p_foster_ppid, fpp_vproc_port);
	tnc_dispose_vproc_port(re_data->re_p_sid, sess_vproc_port);
	tnc_dispose_vproc_port_array(child_pid, child_port, child_pid_cnt);
	tnc_dispose_vproc_port_array(foster_child_pid, foster_child_port,
				     foster_child_pid_cnt);
	tnc_dispose_vproc_port_array(pgrp_member_pid, pgrp_member_port,
				     pgrp_member_pid_cnt);
	tnc_dispose_vproc_port_array(sess_member_pid, sess_member_port,
				     sess_member_pid_cnt);
	return(error);
}

/*
 * This underhand re-definition avoids wholesale re-organization
 * of code: override the PVPSOP dispatch table to force remote
 * operation even to the local node. This is required to keep
 * port rights logic and other stuff happy.
 */
#undef __PVPSOP__
extern struct pvps_ops_vector rpvps_ops_table;
#ifdef	PVPROC_DEBUG
#ifdef	__STDC__
#define __PVPSOP__(op,mode,node,args,argfmt) \
	(dpvps_db1(#op,argfmt), dpvps_db2 args, \
	 dpvps_db3((*rpvps_ops_table.mode->op) args))
#else	/* __STDC__ */
#define __PVPSOP__(op,mode,node,args,argfmt) \
	(dpvps_db1("op",argfmt), dpvps_db2 args, \
	 dpvps_db3((*rpvps_ops_table.mode->op) args))
#endif	/* __STDC__ */
#else	/* PVPROC_DEBUG */
#define __PVPSOP__(op,mode,node,args,argfmt) \
	(*rpvps_ops_table.mode->op) args
#endif	/* PVPROC_DEBUG */

#ifdef	RFORKMULTI_TIMING
#include <sys/time.h>
static struct timeval	start_time, time;
#define	TIMER_START()							\
	raw_microtime(&start_time)
#define TIMER_MARK(printf_str)						\
	raw_microtime(&time);						\
	time.tv_sec = time.tv_sec - start_time.tv_sec;			\
	time.tv_usec = time.tv_usec - start_time.tv_usec;		\
	if (time.tv_usec < 0) {						\
		time.tv_usec += 1000000;				\
		time.tv_sec -= 1;					\
	}								\
	printf("<%d> %4u.%06u ",					\
		this_node, time.tv_sec, time.tv_usec);			\
	printf printf_str;						\
	printf("\n")
#else
#define	TIMER_START()
#define TIMER_MARK(printf_str)
#endif

/*
 * The default tree algorithm to be used by rforkmulti() is binary.
 * This can be overriden by boot magic to be the optimal spanning
 * tree algorithm.
 */
boolean_t	tnc_rforkmulti_binary_tree = TRUE;

int
rforkmulti_server(
	boolean_t	forkfamily,
	int		np_array[],		/* pids/nodes in subtree */
	int		np_array_len,
	int		np_idx,			/* our index in array */
	int		base_idx,		/* base index for subtree */
	int		proc_count,		/* total number of procs */
	int		rval_array[],		/* OUT */
	int		*rval_count,
	mach_port_t	pp_vproc_port,		/* parent */
	mach_port_t	pgrp_vproc_port,	/* pgroup leader */
	mach_port_t	rdir_port,		/* root directory */
	mach_port_t	cdir_port,		/* current directory */
	task_t		pp_task,		/* parent task */
	mach_port_t	ch_vproc_port_array[],	/* OUT */
	int		*ch_vproc_count,
	pid_t		ch_pid_array[],		/* OUT */
	int		*ch_pid_count,
	mach_port_t	file_port_name[],
	unsigned int	file_port_name_count,
	mach_port_t	file_port_right[],
	unsigned int	file_port_right_count,
	thread_state_t	ch_state,		/* state of parent */
	unsigned int	ch_state_count,
	mach_port_t	vproc_port_name,	/* name within emulator */
	mach_port_t	cred_port_name,		/* name within emulator */
	struct rf_data	*rf_data,		/* inherited data */
	struct mmap_struct mmap_structs[],	/* memory mapped file data */
	int		mmap_struct_array_size,
	mach_port_t	mmap_pagers[],
	int		mmap_array_size,
	char		*command_name,		/* command name string */
	char		*logname		/* login name string */
#ifdef NX
,       APPLINFO_T      applinfo,              /* Application info  */
        LP_MAP_T        nodelist,              /* The node list     */
        int              nodelistcnt            /* Number of nodes   */
#endif /* NX */
#ifdef PARACORE
,	mach_port_t	exec_rdir_port,		/* exec root directory */
	mach_port_t	exec_cdir_port,		/* exec current directory */
	char		*exec_prg_name,		/* exec program name */
	unsigned int	exec_prg_name_len	/* size of above file name */
#endif /* PARACORE */
)
{
	struct vproc	*v;			/* very temporary vproc ptr */
	struct vproc	*vc;			/* ptr to child's vproc */
	struct vproc	*vp;			/* ptr to parent's vproc */
	struct pvproc	*pvc;			/* ptr to child's pvproc */
	register int	i, j;
	kern_return_t	ret;
	int		error = ESUCCESS;
	struct proc	*ch_procp;
	int		hashidx;
	task_t		local_task;
	task_t		ch_task;
	thread_t	ch_thread;
	msg_handle_t	reply[VP_STACK_ARRAY_SIZE];
	unsigned int	cnt[VP_STACK_ARRAY_SIZE];
	int		*new_rval_array[VP_STACK_ARRAY_SIZE];
	mach_port_t	*new_ch_vproc_port_array[VP_STACK_ARRAY_SIZE];
	pid_t		*new_ch_pid_array[VP_STACK_ARRAY_SIZE];
	unsigned int	new_rval_cnt[VP_STACK_ARRAY_SIZE];
	unsigned int	new_ch_vproc_port_cnt[VP_STACK_ARRAY_SIZE];
	unsigned int	new_ch_pid_cnt[VP_STACK_ARRAY_SIZE];
	int		subtree_idx[VP_STACK_ARRAY_SIZE];
	int		subtree_base[VP_STACK_ARRAY_SIZE];
	int		subtree_count = VP_STACK_ARRAY_SIZE;
	int		sbase;
#ifdef NX
	boolean_t       send_sigtrap;
#endif /* NX */

	ASSERT((forkfamily ? VPROCNODE(np_array[np_idx]) : np_array[np_idx])
		== this_node);

	/*
	 * Calculate the division of labor for the asynchronous portion of the
	 * rforkmulti().
	 */
	if (tnc_rforkmulti_binary_tree) {
		subtree_idx[0] = 1;
		cnt[0] = np_array_len >> 1;
		subtree_idx[1] = subtree_idx[0] + cnt[0];
		cnt[1] = np_array_len - cnt[0] - 1;
		if (cnt[0] == 0)
			subtree_count = 0;
		else if (cnt[1] == 0)
			subtree_count = 1;
		else
			subtree_count = 2;
	} else {
		/*
		 * Find the entries in the pid array that are below this entry
		 * in the spanning tree.
		 */
		error =  get_spanning_tree(proc_count, np_idx + base_idx,
					   subtree_idx, &subtree_count);
		if (error)
			panic("rforkmulti_server: get_spanning_tree failed, "
			      "return = 0x%x", error);
	}

	/*
	 * Set up the output parameter array sizes appropriately.
	 */
	*rval_count = np_array_len;
	*ch_vproc_count = np_array_len;
	*ch_pid_count = np_array_len;

	/*
	 * Be pessimistic, assume the worst and prepare to return
	 * bad news all round. This is more likely in the forkfamily
	 * case when either we can't fork locally because the required
	 * pid is already allocated.
	 */
	for (i = 0; i < np_array_len; i++) {
		rval_array[i] = ECHILD;
		ch_pid_array[i] = -1;
		ch_vproc_port_array[i] = MACH_PORT_NULL;
	}

	/*
	 * In the case of forkfamily, allocate the required pid/vproc
	 * now. If we can't we return immediately with an error.
	 */
	if (forkfamily) {
		if (VPROCNODE(np_array[np_idx]) != this_node)
			return(ECHILD);
		error = PVPSOP_REMOTE_VPROC_ALLOCATE(this_node,
						     np_array[np_idx]);
		if (error != ESUCCESS) {
			rval_array[0] = error;
			return(error);
		}
		vc = VPROCPTR(np_array[np_idx]);
		ch_pid_array[np_idx] = np_array[np_idx];
	}

	TIMER_START();

	/*
	 * In order to load-level the paging on Mach systems, make the
	 * spanning tree "pseudo-children" inherit their address spaces
	 * from the process/task that is created on this node. In order
	 * to maximize parallelism, we do not do a full fork here, but
	 * only create the Mach task, and make sure the fork uses the
	 * created task later.
	 */
	ux_server_thread_blocking();
	ret = norma_task_create(pp_task, TRUE, this_node, &local_task);
	ux_server_thread_unblocking();

	if (ret == KERN_SUCCESS) {
		u.uu_fork_task = local_task;
	} else {
		/*
		 * In case of failure, inherit from parent on other node,
		 * and try the task_create again later during pproc_fork().
		 */
		printf("rforkmulti_server: norma_task_create failure, ret=%d\n",
		       ret);
		local_task = pp_task;
	}
	TIMER_MARK(("after norma_task_create()"));

	/*
	 * Perform the asynchronous "send" of the message to all the
	 * processes that are below this entry in the spanning tree.
	 */
	for (i=0; i<subtree_count; i++) {
		node_t  subtree_node;
		/*
		 * Check remote node.
		 */
		subtree_idx[i] -= base_idx;
		subtree_node = forkfamily ?
				VPROCNODE(np_array[subtree_idx[i]]) :
				np_array[subtree_idx[i]];
		if (!is_tnc_node_valid(subtree_node)) {
			cnt[i] = 0;
			continue;
		}
		if (tnc_rforkmulti_binary_tree) {
			sbase = subtree_idx[i];
			subtree_base[i] = 0;
		} else {
			tnc_spanning_tree_info(proc_count,
					       subtree_idx[i] + base_idx,
					       &subtree_base[i], &cnt[i]);
			sbase = subtree_base[i] - base_idx;
		}
		TIMER_MARK(("before SEND(%d,..)", subtree_node));
		/*
		 * Call either the short version of rforkmulti or the
		 * long version of rforkmulti, depending upon the number
		 * of nodes. If the long version is called, perform
		 * appropriate memory allocation and deallocation.
		 */
		if (cnt[i] <= MAX_MULTI_LIST_SIZE &&
		    file_port_name_count <= MAX_MULTI_LIST_SIZE) {
#ifdef NX
#ifdef PARACORE
			ret = PVPSOP_RFORKMULTI_SEND(
				subtree_node,
				forkfamily,
				&np_array[sbase], cnt[i],
				subtree_idx[i]-sbase,
				subtree_base[i],
				proc_count,
				NULL, NULL,
				pp_vproc_port, 
				pgrp_vproc_port,
				rdir_port, cdir_port,
				local_task,
				NULL, NULL,
				NULL, NULL,
				file_port_name, file_port_name_count,
				file_port_right, file_port_right_count,
				(int *)ch_state, ch_state_count,
				vproc_port_name, cred_port_name,
				rf_data,
				mmap_structs, mmap_struct_array_size, 
				mmap_pagers, mmap_array_size, 
				command_name, logname,
                                applinfo,
                                nodelist, nodelistcnt,
				exec_rdir_port, exec_cdir_port,
                                exec_prg_name, exec_prg_name_len,
				&reply[i]
				);
#else /* PARACORE */
			ret = PVPSOP_RFORKMULTI_SEND(
				subtree_node,
				forkfamily,
				&np_array[sbase], cnt[i],
				subtree_idx[i]-sbase,
				subtree_base[i],
				proc_count,
				NULL, NULL,
				pp_vproc_port, 
				pgrp_vproc_port,
				rdir_port, cdir_port,
				local_task,
				NULL, NULL,
				NULL, NULL,
				file_port_name, file_port_name_count,
				file_port_right, file_port_right_count,
				(int *)ch_state, ch_state_count,
				vproc_port_name, cred_port_name,
				rf_data,
				mmap_structs, mmap_struct_array_size, 
				mmap_pagers, mmap_array_size, 
				command_name, logname,
                                applinfo,
                                nodelist, nodelistcnt,
				&reply[i]
				);
#endif /* PARACORE */
#else /* NX */
			ret = PVPSOP_RFORKMULTI_SEND(
#ifdef PARACORE
				subtree_node,
				forkfamily,
				&np_array[sbase], cnt[i],
				subtree_idx[i]-sbase,
				subtree_base[i],
				proc_count,
				NULL, NULL,
				pp_vproc_port, 
				pgrp_vproc_port,
				rdir_port, cdir_port,
				local_task,
				NULL, NULL,
				NULL, NULL,
				file_port_name, file_port_name_count,
				file_port_right, file_port_right_count,
				(int *)ch_state, ch_state_count,
				vproc_port_name, cred_port_name,
				rf_data,
				mmap_structs, mmap_struct_array_size, 
				mmap_pagers, mmap_array_size, 
				command_name, logname,
				exec_rdir_port, exec_cdir_port,
                                exec_prg_name, exec_prg_name_len,
				&reply[i]
				);
#else /* PARACORE */
			ret = PVPSOP_RFORKMULTI_SEND(
				subtree_node,
				forkfamily,
				&np_array[sbase], cnt[i],
				subtree_idx[i]-sbase,
				subtree_base[i],
				proc_count,
				NULL, NULL,
				pp_vproc_port, 
				pgrp_vproc_port,
				rdir_port, cdir_port,
				local_task,
				NULL, NULL,
				NULL, NULL,
				file_port_name, file_port_name_count,
				file_port_right, file_port_right_count,
				(int *)ch_state, ch_state_count,
				vproc_port_name, cred_port_name,
				rf_data,
				mmap_structs, mmap_struct_array_size, 
				mmap_pagers, mmap_array_size, 
				command_name, logname,
				&reply[i]
				);
#endif /* PARACORE */
#endif /* NX */
			if (ret != KERN_SUCCESS) {
				cnt[i] = 0;
			}
		} else {
			new_rval_array[i] = NULL;
			new_ch_vproc_port_array[i] = NULL;
			new_ch_pid_array[i] = NULL;
			new_rval_cnt[i] = 0;
			new_ch_vproc_port_cnt[i] = 0;
			new_ch_pid_cnt[i] = 0;
#ifdef NX
#ifdef PARACORE
			ret = PVPSOP_RFORKMULTI_LONG_SEND(
				subtree_node,
				forkfamily,
				&np_array[sbase], cnt[i],
				subtree_idx[i]-sbase,
				subtree_base[i],
				proc_count,
				NULL, NULL,
				pp_vproc_port, 
				pgrp_vproc_port,
				rdir_port, cdir_port,
				local_task,
				NULL, NULL,
				NULL, NULL,
				file_port_name, file_port_name_count,
				file_port_right, file_port_right_count,
				(int *)ch_state, ch_state_count,
				vproc_port_name, cred_port_name,
				rf_data,
				mmap_structs, mmap_struct_array_size, 
				mmap_pagers, mmap_array_size, 
				command_name, logname,
                                applinfo,
                                nodelist, nodelistcnt,
				exec_rdir_port, exec_cdir_port,
                                exec_prg_name, exec_prg_name_len,
				&reply[i]
			        );
#else /* PARACORE */
			ret = PVPSOP_RFORKMULTI_LONG_SEND(
				subtree_node,
				forkfamily,
				&np_array[sbase], cnt[i],
				subtree_idx[i]-sbase,
				subtree_base[i],
				proc_count,
				NULL, NULL,
				pp_vproc_port, 
				pgrp_vproc_port,
				rdir_port, cdir_port,
				local_task,
				NULL, NULL,
				NULL, NULL,
				file_port_name, file_port_name_count,
				file_port_right, file_port_right_count,
				(int *)ch_state, ch_state_count,
				vproc_port_name, cred_port_name,
				rf_data,
				mmap_structs, mmap_struct_array_size, 
				mmap_pagers, mmap_array_size, 
				command_name, logname,
                                applinfo,
                                nodelist, nodelistcnt,
				&reply[i]
			        );
#endif /* PARACORE */
#else /* NX */
#ifdef PARACORE
			ret = PVPSOP_RFORKMULTI_LONG_SEND(
				subtree_node,
				forkfamily,
				&np_array[sbase], cnt[i],
				subtree_idx[i]-sbase,
				subtree_base[i],
				proc_count,
				NULL, NULL,
				pp_vproc_port, 
				pgrp_vproc_port,
				rdir_port, cdir_port,
				local_task,
				NULL, NULL,
				NULL, NULL,
				file_port_name, file_port_name_count,
				file_port_right, file_port_right_count,
				(int *)ch_state, ch_state_count,
				vproc_port_name, cred_port_name,
				rf_data,
				mmap_structs, mmap_struct_array_size, 
				mmap_pagers, mmap_array_size, 
				command_name, logname,
				exec_rdir_port, exec_cdir_port,
                                exec_prg_name, exec_prg_name_len,
				&reply[i]
			        );
#else /* PARACORE */
			ret = PVPSOP_RFORKMULTI_LONG_SEND(
				subtree_node,
				forkfamily,
				&np_array[sbase], cnt[i],
				subtree_idx[i]-sbase,
				subtree_base[i],
				proc_count,
				NULL, NULL,
				pp_vproc_port, 
				pgrp_vproc_port,
				rdir_port, cdir_port,
				local_task,
				NULL, NULL,
				NULL, NULL,
				file_port_name, file_port_name_count,
				file_port_right, file_port_right_count,
				(int *)ch_state, ch_state_count,
				vproc_port_name, cred_port_name,
				rf_data,
				mmap_structs, mmap_struct_array_size, 
				mmap_pagers, mmap_array_size, 
				command_name, logname,
				&reply[i]
			        );
#endif /* PARACORE */
#endif /* NX */
			if (ret != KERN_SUCCESS) {
				cnt[i] = 0;
			}
		}
	}

	TIMER_MARK(("before local fork"));
	/*
	 * If this is the regular rforkmulti case...
	 * allocate a vproc (and its associated pvproc) for the child.
	 * If there's not enough memory we better find out about it before
	 * we do much more.
	 */
	if (!forkfamily) {
		if ((vc = vproc_alloc()) == NULL) {
			rval_array[np_idx] = ENOMEM;
			tnc_dispose_vproc_port(rf_data->rf_p_pid,
					       pp_vproc_port);
			tnc_dispose_vproc_port(rf_data->rf_p_pgid,
					       pgrp_vproc_port);
			goto out;
		}
		ch_pid_array[np_idx] = dvp_pidgen();	/* the child's pid */
	}

	/*
	 * Call the physical layer first, to fill in our own "dummy"
	 * proc and user structures with info from the real parent
	 * on another node.  We don't deal with the physical layer's
	 * data structures here (except for a single pointer to that
	 * layer's structures), just the data structures of the vproc +
	 * pvproc (virtual) layer.
	 */
	rfork_pproc_unload_msg(NULL, rf_data, pp_task, 
			       mmap_structs, mmap_pagers, mmap_array_size,
			       command_name, logname
#ifdef PARACORE
			       , exec_prg_name
#endif /* PARACORE */
			       );

	/*
 	 * Now call the physical layer again to do the actual physical
	 * fork.  If that's successful then we can do the virtual layer
	 * of forking.  (If that's not successful, the physical layer
	 * undoes whatever it has done and returns failure.  In that
	 * case, we only have the vproc_alloc call above to undo.)
	 *
	 * Note that pointers to the child's task port and thread port
	 * are passed so that the physical layer can pass this information
	 * back to the client.
	 *
	 * A brand new credentials port is set up by the fork mechanism
	 * and a send right to it is inserted into the child task using
	 * the name we provide.  This is the same name that the parent
	 * process (whose memory is being inherited) had for the cred port.
	 */
	rval_array[np_idx] = rtask_pproc_fork(ch_pid_array[np_idx],
						&ch_procp,
						ch_state, ch_state_count,
						rdir_port, cdir_port,
						&ch_task,
						&ch_thread,
						NULL, cred_port_name,
				 		(int *)NULL, 0
#ifdef PARACORE
				 		, exec_rdir_port, exec_cdir_port
#endif /* PARACORE */
						);
	if (rval_array[np_idx] != ESUCCESS) {
		if (forkfamily)
			VPROC_RELEASE(vc, "rforkmulti_server(origin)");
		ch_pid_array[np_idx] = -1;
		vproc_dealloc(vc);	/* failure.  undo what we've done */
		vc = NULL;		/* do nothing more with this ptr */
		goto out;		/* and return the cause */
	}

	pvc = PVP(vc);			/* get address of associated pvproc */
	dpvproc_struct_init(vc);	/* initialize it */
	pvc->pvp_flag |= PV_IS_LOCAL | PV_IS_ORIGIN;

	/*
	 * Continue to set up the data structures of the newly created
	 * child's vproc + pvproc.
	 */

	vc->vp_ops = &dvproc_ops_table;
	pvc->pvp_ops = &dpvproc_ops_table;

	VPROC_LOCK_EXCL(vc,"rforkmulti_server(child)");
	VPROC_HOLD(vc, "rforkmulti_server(active)");
	if (!forkfamily) {
		/* put the vproc on the vproc hash chain */
		VPROC_HOLD(vc, "rforkmulti_server(origin)");
		VPROC_LIST_LOCK();
		vc->vp_pid = ch_pid_array[np_idx];
		v = vproc_hash[hashidx = VPROCPIDHASH(ch_pid_array[np_idx])];
		vc->vp_hashbwd = NULL;
		vc->vp_hashfwd = v;
		vproc_hash[hashidx] = vc;
		if (v)
			v->vp_hashbwd = vc;
		VPROC_LIST_UNLOCK();
	}

	/*
	 * Allocate a Mach port for the new child's vproc.  We assign
	 * the name of the pointer (vc) that was allocated above.
	 */
	(void) vproc_port_allocate(vc);
	pvc->pvp_flag |= PV_HAS_PORT_RIGHT;
	ch_vproc_port_array[np_idx] = tnc_bestow_vproc_port(vc, 0);

	/*
	 * rforkmulti() differs from fork()/rfork() in that child task
	 * and child emulator threads ports are not passed back to the
	 * parent. The underlying physical code has bumped the send
	 * counts on these ports in preparation to thier MOVE. Therefore,
	 * we need to dereference them.
	 */
	(void) mach_port_deallocate(mach_task_self(), ch_task);
	(void) mach_port_deallocate(mach_task_self(), ch_thread);
	
	/*
	 * Insert a send right to the child's vproc port into the child
	 * task itself.  Assign it the same name as its parent (whose
	 * memory the child inherits) knew its own vproc port by.
	 */
	ret = mach_port_insert_right(ch_task,
				     vproc_port_name,
				     ch_vproc_port_array[np_idx],
				     MACH_MSG_TYPE_COPY_SEND);
	if (ret != KERN_SUCCESS) {
		panic("rforkmulti_server: vproc insert_right failed, "
		      "ret = 0x%x", ret);
	}

	/*
	 * Install file ports into the child task.
	 */
	for (i=0; i<file_port_right_count; i++) {
		ret = mach_port_insert_right(ch_task,
					     file_port_name[i],
					     file_port_right[i],
					     MACH_MSG_TYPE_MOVE_SEND);
		if (ret != KERN_SUCCESS) {
			panic("rforkmulti_server: file insert_right failed, "
			      "ret = 0x%x", ret);
		}
	}

	/*
	 * Increment the send right to the vproc port one more time.
	 * This is to make up for the fact that the send right will
	 * be sent back via MOVE_SEND as part of the vproc array
	 * (rather than the normal COPY_SEND used for rfork(), migrate(),
	 * and rexecve().
	 */
	ret = mach_port_mod_refs(mach_task_self(),
				 ch_vproc_port_array[np_idx],
				 MACH_PORT_RIGHT_SEND,
				 +1);
	if (ret != KERN_SUCCESS) {
		panic("rforkmulti_server: vproc mod_refs failed, "
		      "ret = 0x%x", ret);
	}

	/*
	 * Point the child's pvproc at its underlying pproc (proc.h)
	 * structure.  Then initialize some more fields of the pvproc
	 * structure.
	 */
	pvc->pvp_pproc = ch_procp;	/* as returned by the physical layer */

	/* sort out the immediate family relationships */
	pvc->pvp_head_childl = NULL;		/* child has no children */
	pvc->pvp_ppid = rf_data->rf_p_pid;	/* set child's parent id */
	pvc->pvp_foster_ppid = rf_data->rf_p_pid; /* child's foster parent id */
	pvc->pvp_pgid = rf_data->rf_p_pgid;	/* child's pgrp is parent's */
	pvc->pvp_sid = rf_data->rf_p_sid;	/* child's sess is parent's */
	pvc->pvp_pp_sid = rf_data->rf_p_sid;	/* parent's sess is parent's */
	pvc->pvp_pp_pgid = rf_data->rf_p_pgid;	/* parent's pgrp is parent's */


#ifdef NX
        if (rf_data->nx_flags & PV_NX_PARTITIONED) {
	    VPROC_LOCK_FLAG(vc,"svr_rforkmulti(child)");
            pvc->pvp_flag |= rf_data->nx_flags;
	    VPROC_UNLOCK_FLAG(vc,"svr_rforkmulti(child)");
            /*
             *  If we are an NX application then put the appl_info and
             *  the node list into the task.
             */
            ret = nx_put_info(pvc,
                              nodelist,
                              nodelistcnt,
                             &applinfo,
                              ch_pid_array[np_idx]);
            if (ret != KERN_SUCCESS) {
                panic("rforkmulti: svr_rforkmulti failure nx_task_put_info, "
		      "ret=0x%x", ret);
            }
            vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                          (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
            nodelist = (LP_MAP_T) NULL;
        }
#endif /* NX */

	/*
	 * Create parent vproc locally, out of passed parent vproc port.
	 * Normally we should now do a VPROC_HOLD(vp) to keep a 
	 * reference to the parent.  Instead we just skip doing
	 * a VPROC_RELEASE(vp) for the LOCATE_VPROC_PID() below.
	 */
	vp = LOCATE_VPROC_PID(rf_data->rf_p_pid);
	if (vp == 0)
		panic("rforkmulti_server: cannot find parent");
	tnc_install_vproc_port(vp, pp_vproc_port, 0);

	/* initialize the process structure with values from the pvproc */
	pproc_set_attr(pvc->pvp_pproc, 
		       vc, 
		       &vc->vp_pid, 
		       &pvc->pvp_ppid, 
		       &pvc->pvp_pgid, 
		       &pvc->pvp_sid,
		       0);

	/*
	 * Have the child join the parent's pgrp.
	 * Normally we should now do a VPROC_HOLD(g) to keep a 
	 * reference to the pgrp leader.  Instead we just skip doing
	 * a VPROC_RELEASE(g) for the LOCATE_VPROC_PID() below.
	 */
	if (pvc->pvp_pgid != 0) {
		struct vproc *g;
		g = LOCATE_VPROC_PID(pvc->pvp_pgid);
		if (g == 0)
			panic("rforkmulti_server: cannot find pgrp leader");
		tnc_install_vproc_port(g, pgrp_vproc_port, 0);

		dvp_child_join_pgrp_end(vp, vc, g);	/* join parent's pgrp */
	}

	/*
	 * Set pvproc flag field to indicate whether this process has a
	 * controlling terminal. This is derived from the physical proc.
	 */
	{
		int has_sctty;
		pproc_get_attr(pvc->pvp_pproc,0,0,0,0,0,0,0,&has_sctty,0);
		if (has_sctty) {
			VPROC_LOCK_FLAG(vc,"rforkmulti_server(child)");
			pvc->pvp_flag |= PV_SCTTY;
			VPROC_UNLOCK_FLAG(vc,"rforkmulti_server(child)");
		}
	}

#ifdef NX
        send_sigtrap = nx_trace_inherit(vc, rf_data->rf_p_flag);
#endif /* NX */

	VPROC_UNLOCK_EXCL(vc,"rforkmulti_server(child)");

out:
	/*
	 * Wait for the asynchronous portion of the rforkmulti() to complete.
	 */
	for (i=0; i<subtree_count; i++) {
		node_t	subtree_node;
		if (cnt[i] == 0)
			continue;
		subtree_node = forkfamily ?
				VPROCNODE(np_array[subtree_idx[i]]) :
				np_array[subtree_idx[i]];
		/*
		 * Call either the short version of rforkmulti or the
		 * long version of rforkmulti, depending upon the number
		 * of nodes. If the long version is called, perform
		 * appropriate memory allocation and deallocation.
		 */
		if (tnc_rforkmulti_binary_tree)
			sbase = subtree_idx[i];
		else
			sbase = subtree_base[i] - base_idx;
		TIMER_MARK(("before RECEIVE(%d,..)", subtree_node));
		if (cnt[i] <= MAX_MULTI_LIST_SIZE &&
		    file_port_name_count <= MAX_MULTI_LIST_SIZE) {
#ifdef NX
#ifdef PARACORE
			ret = PVPSOP_RFORKMULTI_RECEIVE(
					subtree_node,
					forkfamily,
					&np_array[sbase], cnt[i],
					subtree_idx[i]-sbase,
					subtree_base[i],
					proc_count,
					&rval_array[sbase], &cnt[i],
					pp_vproc_port, 
					pgrp_vproc_port,
					rdir_port, cdir_port,
					local_task,
					&ch_vproc_port_array[sbase], &cnt[i],
					&ch_pid_array[sbase], &cnt[i],
					file_port_name, file_port_name_count,
					file_port_right, file_port_right_count,
					(int *)ch_state, ch_state_count, 
					vproc_port_name, cred_port_name,
					rf_data,
					mmap_structs, mmap_struct_array_size,
					mmap_pagers, mmap_array_size,
					command_name, logname,
                                        applinfo,
                                        nodelist, nodelistcnt,
					exec_rdir_port, exec_cdir_port,
                                	exec_prg_name, exec_prg_name_len,
					&reply[i]
					);
#else /* PARACORE */
			ret = PVPSOP_RFORKMULTI_RECEIVE(
					subtree_node,
					forkfamily,
					&np_array[sbase], cnt[i],
					subtree_idx[i]-sbase,
					subtree_base[i],
					proc_count,
					&rval_array[sbase], &cnt[i],
					pp_vproc_port, 
					pgrp_vproc_port,
					rdir_port, cdir_port,
					local_task,
					&ch_vproc_port_array[sbase], &cnt[i],
					&ch_pid_array[sbase], &cnt[i],
					file_port_name, file_port_name_count,
					file_port_right, file_port_right_count,
					(int *)ch_state, ch_state_count, 
					vproc_port_name, cred_port_name,
					rf_data,
					mmap_structs, mmap_struct_array_size,
					mmap_pagers, mmap_array_size,
					command_name, logname,
                                        applinfo,
                                        nodelist, nodelistcnt,
					&reply[i]
					);
#endif /* PARACORE */
#else /* NX */
#ifdef PARACORE
			ret = PVPSOP_RFORKMULTI_RECEIVE(
					subtree_node,
					forkfamily,
					&np_array[sbase], cnt[i],
					subtree_idx[i]-sbase,
					subtree_base[i],
					proc_count,
					&rval_array[sbase], &cnt[i],
					pp_vproc_port, 
					pgrp_vproc_port,
					rdir_port, cdir_port,
					local_task,
					&ch_vproc_port_array[sbase], &cnt[i],
					&ch_pid_array[sbase], &cnt[i],
					file_port_name, file_port_name_count,
					file_port_right, file_port_right_count,
					(int *)ch_state, ch_state_count, 
					vproc_port_name, cred_port_name,
					rf_data,
					mmap_structs, mmap_struct_array_size,
					mmap_pagers, mmap_array_size,
					command_name, logname,
					exec_rdir_port, exec_cdir_port,
                                	exec_prg_name, exec_prg_name_len,
					&reply[i]
					);
#else /* PARACORE */
			ret = PVPSOP_RFORKMULTI_RECEIVE(
					subtree_node,
					forkfamily,
					&np_array[sbase], cnt[i],
					subtree_idx[i]-sbase,
					subtree_base[i],
					proc_count,
					&rval_array[sbase], &cnt[i],
					pp_vproc_port, 
					pgrp_vproc_port,
					rdir_port, cdir_port,
					local_task,
					&ch_vproc_port_array[sbase], &cnt[i],
					&ch_pid_array[sbase], &cnt[i],
					file_port_name, file_port_name_count,
					file_port_right, file_port_right_count,
					(int *)ch_state, ch_state_count, 
					vproc_port_name, cred_port_name,
					rf_data,
					mmap_structs, mmap_struct_array_size,
					mmap_pagers, mmap_array_size,
					command_name, logname,
					&reply[i]
					);
#endif /* PARACORE */
#endif /* NX */
		} else {
#ifdef NX
#ifdef PARACORE
			ret = PVPSOP_RFORKMULTI_LONG_RECEIVE(
					subtree_node,
					forkfamily,
					&np_array[sbase], cnt[i],
					subtree_idx[i]-sbase,
					subtree_base[i],
					proc_count,
					&new_rval_array[i], &new_rval_cnt[i],
					pp_vproc_port, 
					pgrp_vproc_port,
					rdir_port, cdir_port,
					local_task,
					&new_ch_vproc_port_array[i], 
						&new_ch_vproc_port_cnt[i],
					&new_ch_pid_array[i], 
						&new_ch_pid_cnt[i],
					file_port_name, file_port_name_count,
					file_port_right, file_port_right_count,
					(int *)ch_state, ch_state_count, 
					vproc_port_name, cred_port_name,
					rf_data,
					mmap_structs, mmap_struct_array_size,
					mmap_pagers, mmap_array_size,
					command_name, logname,
                                        applinfo,
                                        nodelist, nodelistcnt,
					exec_rdir_port, exec_cdir_port,
                                	exec_prg_name, exec_prg_name_len,
					&reply[i]
					);
#else /* PARACORE */
			ret = PVPSOP_RFORKMULTI_LONG_RECEIVE(
					subtree_node,
					forkfamily,
					&np_array[sbase], cnt[i],
					subtree_idx[i]-sbase,
					subtree_base[i],
					proc_count,
					&new_rval_array[i], &new_rval_cnt[i],
					pp_vproc_port, 
					pgrp_vproc_port,
					rdir_port, cdir_port,
					local_task,
					&new_ch_vproc_port_array[i], 
						&new_ch_vproc_port_cnt[i],
					&new_ch_pid_array[i], 
						&new_ch_pid_cnt[i],
					file_port_name, file_port_name_count,
					file_port_right, file_port_right_count,
					(int *)ch_state, ch_state_count, 
					vproc_port_name, cred_port_name,
					rf_data,
					mmap_structs, mmap_struct_array_size,
					mmap_pagers, mmap_array_size,
					command_name, logname,
                                        applinfo,
                                        nodelist, nodelistcnt,
					&reply[i]
					);
#endif /* PARACORE */
#else /* NX */
#ifdef PARACORE
			ret = PVPSOP_RFORKMULTI_LONG_RECEIVE(
					subtree_node,
					forkfamily,
					&np_array[sbase], cnt[i],
					subtree_idx[i]-sbase,
					subtree_base[i],
					proc_count,
					&new_rval_array[i], &new_rval_cnt[i],
					pp_vproc_port, 
					pgrp_vproc_port,
					rdir_port, cdir_port,
					local_task,
					&new_ch_vproc_port_array[i], 
						&new_ch_vproc_port_cnt[i],
					&new_ch_pid_array[i], 
						&new_ch_pid_cnt[i],
					file_port_name, file_port_name_count,
					file_port_right, file_port_right_count,
					(int *)ch_state, ch_state_count, 
					vproc_port_name, cred_port_name,
					rf_data,
					mmap_structs, mmap_struct_array_size,
					mmap_pagers, mmap_array_size,
					command_name, logname,
					exec_rdir_port, exec_cdir_port,
                                	exec_prg_name, exec_prg_name_len,
					&reply[i]
					);
#else /* PARACORE */
			ret = PVPSOP_RFORKMULTI_LONG_RECEIVE(
					subtree_node,
					forkfamily,
					&np_array[sbase], cnt[i],
					subtree_idx[i]-sbase,
					subtree_base[i],
					proc_count,
					&new_rval_array[i], &new_rval_cnt[i],
					pp_vproc_port, 
					pgrp_vproc_port,
					rdir_port, cdir_port,
					local_task,
					&new_ch_vproc_port_array[i], 
						&new_ch_vproc_port_cnt[i],
					&new_ch_pid_array[i], 
						&new_ch_pid_cnt[i],
					file_port_name, file_port_name_count,
					file_port_right, file_port_right_count,
					(int *)ch_state, ch_state_count, 
					vproc_port_name, cred_port_name,
					rf_data,
					mmap_structs, mmap_struct_array_size,
					mmap_pagers, mmap_array_size,
					command_name, logname,
					&reply[i]
					);
#endif /* PARACORE */
#endif /* NX */
			if (ret != KERN_SUCCESS) {
				continue;
			}
			bcopy(new_rval_array[i], &rval_array[sbase], 
			      cnt[i]*sizeof(int));
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) new_rval_array[i],
					     cnt[i]*sizeof(int));
			bcopy(new_ch_vproc_port_array[i], 
			      &ch_vproc_port_array[sbase], 
			      cnt[i]*sizeof(mach_port_t));
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) 
						    new_ch_vproc_port_array[i],
					     cnt[i]*sizeof(mach_port_t));
			bcopy(new_ch_pid_array[i], &ch_pid_array[sbase], 
			      cnt[i]*sizeof(pid_t));
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) new_ch_pid_array[i],
					     cnt[i]*sizeof(pid_t));
		}
	}

	/*
	 * We can now let the child go on its way. This is the earliest
	 * time that this can be done safely because siblings at the
	 * next level in the memory inheritance (paging) tree must
	 * have (norma_)created their tasks first.
	 */
	ret = thread_resume(ch_thread);
	if (ret != KERN_SUCCESS) {
		/*
		 * Child dead before birth - null out the vproc so that
		 * nothing further is done with this child. Let the
		 * probably successful rval_array value be returned to
		 * the parent, however.
		 */
		vc = NULL;
	}

#ifdef NX
	if (send_sigtrap && vc != NULL) {
	    (void)VPOP_REPORT_STATE(vc, VPROC_NO_SIGCHLD);
	    error = VPOP_SIGPROC(vc, SIGTRAP, 0, 
				 VPROC_HAS_PRIV | VPROC_CHILD_STOP);
	}
#endif /* NX */
	
	TIMER_MARK(("done"));
	return(ESUCCESS);
}



