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
 * $Log: rtask_cli_vproc.c,v $
 * Revision 1.26  1995/04/08  00:05:39  yazz
 *  Reviewer: Ray Shapouri, Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 12711 (improvement over earlier checkin)
 *  Testing: EATs controlc, sched, os_interfaces, MUNOPS SAT runs
 *  Module(s): server/tnc/rtask_cli_vproc.c
 * Use new vm_allocate/deallocate_strict() routines instead of asserts,
 * to panic on VM error conditions.
 *
 * Revision 1.25  1995/03/21  22:46:52  yazz
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Med due to # of lines
 *  Benefit or PTS #: 12711, a derivitive of mandatory 11238
 *  Testing: MUNOPS
 *  Module(s): server/tnc/rtask_cli_vproc.c
 * Fixed over a dozen VM leaks of OOL memory from nx_get_info() type calls.
 *
 * Revision 1.24  1995/02/01  21:48:22  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.23  1995/01/20  20:44:02  nandy
 * Moved pproc_assert_sleep_wait() to the top of the loop in waitmulti()
 *
 *  Reviewer: Chris Peak
 *  Risk: L
 *  Benefit or PTS #: 11752
 *  Testing: nx_loadve9_hang test
 *  Module(s): server/tnc/rtask_cli_vproc.c
 *
 * Revision 1.22  1994/12/16  18:19:54  raysx
 * Added code so that when kalloc fails on the second memmory
 * allocation, the first successfully allocated memmory is
 * deallocated.
 *
 * Reviewer(s): yazz@locus.com
 * 	     suri@ssdintel.intel.com
 * Risk: Low
 * Benefit or PTS #: 11925
 * Testing: By inspection.
 * Module(s): tnc/rtask_cli_vproc.c
 *
 * Revision 1.21  1994/11/18  20:43:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.20  1994/08/31  22:47:16  mtm
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
 * Revision 1.19.2.1  1994/08/10  15:53:37  nandy
 * rforkmulti() needs to inherit PV_NX_ATH_PGRP along with other flags.
 *
 *  Reviewer: John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 10559
 *  Testing: IPD EATs
 *  Module(s): rtask_cli_vproc.c
 *
 * Revision 1.19  1994/07/27  16:30:54  johannes
 * In rfork() the information is got from the utask by extending
 * rfork_pproc_load_msg() call and then passed to the remote node
 * by extending cli_rfork() call.
 *
 * In rforkmulti() the information is got from the utask by extending
 * rfork_pproc_load_msg() call and then passed to the remote node
 * by extending PVPSOP_RFORKMULTI() call. Finally the ports have to
 * be deallocated.
 *
 * In migrate() the information is got from the utask by extending
 * migrate_pproc_load_msg() call and then passed to the remote node
 * by extending cli_migrate() call.
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
 * Revision 1.18  1994/07/26  14:27:15  johannes
 * waitmulti(): changed incorrect debug_string for VPROC_LOCK_EXCL
 *
 * Revision 1.17  1994/06/21  17:07:41  nandy
 * waitmulti() now cosiders option WUNTRACED. Stopped processes
 * are not reaped if WUNTRACED is not set.
 *
 * Revision 1.16  1994/06/18  00:46:40  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.15  1994/06/03  15:21:38  chrisp
 * Corrected re-definition of macro __PVPSOP__ for the non-debug case
 * (when MACH_ASSERT=0).
 *
 *  Reviewer:
 *  Risk: L
 *  Benefit or PTS #:
 *  Testing: Compilation successful without MACH_ASSERT
 *  Module(s): rtask_cli_vproc.c rtask_svr_vproc.c
 *
 * Revision 1.14  1994/06/02  22:29:08  chrisp
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
 * Revision 1.13  1994/03/14  02:05:48  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.12  1993/11/03  19:17:17  yazz
 * Establish a sequence number mechanism whereby process group signals, such as
 * those generated by CTRL/C, are guaranteed to be delivered to child processes
 * of a reproducing (fork(), rfork(), rforkmulti(), etc.) task.  Many unixes
 * have a timing window where new child procs can miss out on a pgrp-style signal.
 *
 * Revision 1.11  1993/10/21  23:35:35  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.10  1993/07/14  18:33:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.7  1993/07/01  20:46:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.9  1993/06/09  00:09:53  cfj
 * Change occurances of #include <i860ipsc/mcmsg> to #include <i860paragon/mcmsg>
 *
 * Revision 1.8  1993/05/06  19:23:11  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.5  1993/05/03  17:46:00  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.7  1993/04/03  03:09:00  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.6  1993/03/27  18:18:22  cfj
 * Merge with T9.
 *
 * Revision 1.5.6.1  1993/03/26  23:41:51  cfj
 * Put ux_server_thread_blocking/unblocking around off node RPCs.
 *
 * Revision 1.1.2.4.2.2  1992/12/16  06:02:26  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.4.2.1  1992/12/14  23:21:37  brad
 * Merged tip of old NX branch with PFS branch.
 * Revision 1.4  1992/11/30  22:47:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.5  1992/12/11  03:01:55  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.4  1992/11/30  22:47:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.5  1992/11/17  20:22:25  cfj
 * Made our sources match Roman's for the crash in rforkmulti().
 * In table_node() map the node number from logical to physical.
 *
 * Revision 1.1.2.4  1992/11/16  02:30:28  cfj
 * Fix the case where the number of nodes to rforkmulti() is greater than
 * MAX_MULTI_LIST_SIZE.  Do not do the bopy into ch_vproc_port_array, and
 * use new_ch_vproc_port_array in the loop doing the tnc_install_vproc_port().
 *
 * Revision 1.1.2.3  1992/11/09  18:00:57  cfj
 * Conflict resolution of 11/05/92 bug fix drop from Locus.
 *
 * Revision 1.1.2.2  1992/11/06  20:31:32  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:45:56  dleslie
 * Local changes for NX through noon, November 5, 1992.
 * 
 * Revision 3.44  93/09/16  09:07:24  chrisp
 * [SPE 0030] Generic Spanning Trees: rpvproc_ops_table is now a 3-way vector.
 * 
 * Revision 3.43  93/06/19  15:26:35  yazz
 * [ ad1.04 merge ]
 * 	Replace MAX_MMAP_REGIONS with the new TNC_MAX_MMAP_REGIONS in
 * 	remote tasking operations.
 * 
 * Revision 3.42  93/04/23  13:54:27  bolsen
 * [Bug 212-4754] released extra send right for rdir_port and cdir_port due
 * 	to COPY_SEND in cli_rforkmulti(_long) call.
 * 
 * Revision 3.41  93/03/23  19:51:34  yazz
 * Added ux_server_thread_blocking/unblocking() calls around remote task
 * RPCs.
 * 
 * Revision 3.40  92/11/16  09:27:47  roman
 * [Bug 104] rforkmulti() with > MAX_MULTI_LIST_SIZE nodes causes overrun
 * 	of stack array ch_vproc_port_array. Changed to keep
 * 	server-allocated array until the routine is done.
 * Fix pointer types in rforkmulti() to correctly reflect underlying types.
 * 
 * Revision 3.39  92/11/10  12:51:08  chrisp
 * Remove dependency on tnc/tnc_types_gen.h.
 * 
 * Revision 3.38  92/11/05  15:34:57  klh
 * Fix Bug #92, initialize cred_cache_size prior to calling *_pproc_load_msg()
 * (klh for chrisp)
 * 
 * Revision 3.37  92/11/02  11:44:16  roman
 * Add support for out-of-line-memory versions of MiG calls for
 * 	migrate, rexecve, and rforkmulti.
 * 
 * Revision 3.36  92/10/28  15:27:57  roman
 * Change types for cleaner compilation.
 * 
 * Revision 3.35  92/10/16  11:18:50  chrisp
 * [Bug #66] Use rtask_pproc_unquiesce() in error cases before returning to
 * 	the emulator.
 * [Bug #72] Cancel deadname notification for task port before migrating
 * 	to avoid executing irrelevent exit logic in the notification
 * 	routine should the old task be destroyed (by the new emulator)
 * 	before server rights are given up.
 * Now call rtask_pproc_remove () here rather than from bsd_server_side.c.
 * 
 * Revision 3.34  92/10/08  17:05:16  chrisp
 * Change names of operations tables for consistency.
 * 
 * Revision 3.33  92/10/08  11:31:50  roman
 * Call rtask_pproc_quiesce() in migrate and rexec so that all threads
 * 	in user space can reach a "safe" point prior to the migration.
 * 
 * Revision 3.32  92/10/01  10:30:21  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.31  92/09/29  08:09:08  roman
 * Change references from "site" to "node".
 * Change pvproc field names to new (better, more consistant) versions.
 * Add concept of foster children lists that must be moved for rexec
 * 	and migrate.
 * Change the migrate lock to movement lock (name change only). Also recognize
 * 	that the migrate lock structure is now separate from the pvproc
 * 	structure.
 * 
 * Revision 3.30  92/07/30  16:11:20  chrisp
 * If migrate() or rexec() fails remotely, ensure that port sequence counts
 * 	are correctly reset.
 * 
 * Revision 3.29  92/07/17  11:23:17  roman
 * Fix error in rforkmulti() where a sequence of bad node numbers caused
 * 	a server panic.
 * 
 * Revision 3.28  92/07/10  09:02:02  chrisp
 * Reset port sequence count (to -1) if rexec fails and ports have to be
 * 	re-established on the client side. Include assertion check that
 * 	mach resets its message counts to 0 in this case.
 * 
 * Revision 3.27  92/07/08  09:08:58  roman
 * Change node numbers to type node_t.
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.26  92/06/26  14:00:32  chrisp
 * [Bug #27] Job control handling fixed for migrated/rexec'd processes - pvproc
 * 	field pvp_jobc added to MIG.
 * 
 * Revision 3.25  92/06/05  15:38:53  roman
 * Correct code for mmape'ed files following v0.89 integration.
 * 
 * Revision 3.24  92/05/06  13:34:06  chrisp
 * [Bug #9] Transfer pvproc field pvp_cttynode on rexec()/migrate().
 * 
 * Revision 3.23  92/05/06  09:22:54  chrisp
 * Add calls to VPROC_[UN]LOCK_FLAG macros to guard updates to pvp_flag.
 * 
 * Revision 3.22  92/05/01  09:58:52  roman
 * Fix ordering of the command name and login name for rfork and
 * migrate to correspond with the underlying pproc layer and with the
 * MiG interfaces.
 * 
 * Revision 3.21  92/04/22  12:36:24  chrisp
 * Alter existence rules for session leader vproc, mark:
 * 	- on node of each pgrp leader in session;
 * 	- on node of session leader for each pgrp in session list.
 * In rfork() and rforkmulti(), don't pass session vproc port to remote server.
 * In migrate() and rexec(), pass session vproc port to server-side only
 * 	for pgrp leaders.
 * 
 * Revision 3.20  92/04/14  10:32:17  roman
 * Correct logic to build up pgrp and session lists. Correct reference counting
 * 	decrements for migration of pgrp and session leaders.
 * 
 * Revision 3.19  92/04/06  16:04:28  chrisp
 * Take note of return value from tnc_get_server_port() and don't assume
 * 	parameters return sensible values in the error case.
 * 
 * Revision 3.18  92/04/01  16:19:34  roman
 * Add support for client portion of rforkmulti() system call.
 * 
 * Revision 3.17  92/03/27  11:29:24  roman
 * Changes to incorporate the new OSF/1 AD v0.8.5.1 handling of root and
 * current working directory ports.
 * 
 * Revision 3.16  92/03/18  12:14:35  roman
 * Add locking around migrate() and rexecve() to prevent these from
 * interracting with pvproc ops and vproc ops (to support mult-threaded
 * interruptible server).
 * 
 * Revision 3.15  92/03/12  15:27:46  roman
 * Initialize the new pvproc fields that track a parent's pgrp and session id.
 * 
 * Revision 3.14  92/02/20  09:24:27  roman
 * Add new parameter to tnc_install_vproc_port() and tnc_bestow_vproc_port().
 * No longer hold pvproc lock across calls to cli_migrate() and
 * cli_rexecve(). This was a bad idea anyway. Instead, use the
 * new interfaces to tnc_install_vproc_port() and tnc_bestow_vproc_port()
 * to do the work.
 * 
 * Revision 3.13  92/02/14  08:50:46  roman
 * Turn PV_IS_LOCAL pvproc flag on and off as necessary, using
 * appropriate mutexes.
 * Have server-to-server rfork/rexec/migrate use a separate parameter for
 * returning errors (rather than just the return value) so that
 * output parameters can be passed back on errors.
 * Change size of vproc lists for pgrp, sessions, and parent-child-sibling
 * lists to use symbolic constant for size of list.
 * 
 * Revision 3.12  92/01/28  15:52:02  roman
 * Add new parameters of current and root directory ports to rexec.
 * 
 * Revision 3.11  92/01/17  11:08:48  chrisp
 * Remove superfluous "session leader on member node" reference counting.
 * 
 * Revision 3.10  92/01/16  11:43:25  chrisp
 * Reference count decrements added for parent/pgrp/session.
 * 
 * Revision 3.9  92/01/15  16:46:16  roman
 * Simplify many variable names to get rid of rf_, mi_, and re_ prefixes
 * where this added no information.
 * Got rid of extraneous output error parameter for server-to-server MiG
 * routines.
 * Got rid of extraneous printf's on failure of rfork()/rexec()/migrate().
 * Change calls to reinstall_cred_port() to instead call
 * reinstall_process_ports().
 * Much fixing of indentation to be a single consistent style throughout
 * the file.
 * 
 * Revision 3.8  92/01/15  10:28:15  chrisp
 * Uncomment VPROC_RELEASE(active) for rexecve and migrate.
 * 
 * Revision 3.7  92/01/14  13:20:45  yazz
 * Properly renamed vproc port names now used when migrate/rexec fails.
 * Renames bypassed if port already has desired name.
 * Panic messages now contain more info, and corrected routine names.
 * 
 * Revision 3.6  92/01/10  14:58:52  yazz
 * Credentials (.74 version) merge.  Some error messages that had mentioned
 * incorrect routine names corrected.  On rfork(), send right to the child
 * vproc are returned to the parent with a MAKE_SEND in the MiG spec instead
 * of being explicitly inserted and then moved with MOVE_SEND (easier cleanup
 * on error).  Remote task failures now cleaned up after and task on original
 * node is better re-instated, including reinstalled Rcv rts for vproc, cred
 * and proc and their return to listening port-sets.
 * 
 * Revision 3.5  92/01/07  18:22:31  roman
 * Declare and pass the mmap structures from the client to the server
 * so that mapped files can be later supported.
 * 
 * Revision 3.4  91/12/24  10:30:29  roman
 * Change strategy so that routines that are giving away send rights
 * to vprocs always use tnc_bestow_vproc_port() to give away the
 * rights. For rfork, give vproc port an extra send right so that
 * the right can be passed back to emulator via MOVE_SEND.
 * 
 * Revision 3.3  91/12/19  11:09:05  chrisp
 * Transfer zombie and stopped status for migrate and rexec.
 * 
 * Revision 3.2  91/12/18  16:46:50  roman
 * Remove unused debugging routine.
 * 
 * Revision 3.1  91/12/16  09:31:34  roman
 * Changes to get rexec to work for the first time.
 * 
 * Revision 3.0  91/12/13  09:14:00  roman
 * Initial submission. This file contains the vproc code for the client
 * side of rfork/rexec/migrate.
 * 
 */

#include <sys/errno.h>
#include <sys/wait.h>
#include <tnc/rtask.h>
#include <uxkern/bsd_types_gen.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <sys/syscall.h>
#include <uxkern/proc_to_task.h>
#include <tnc/tnc.h>
#include <uxkern/syscall_subr.h>

#ifdef NX
#include <i860paragon/mcmsg/mcmsg_info.h>

#if __STDC__ == 1

extern int nx_map_node(struct pvproc *pvp,
                       int            dest_node,
                       APPLINFO_T    *applinfo_p,
                       LP_MAP_T      *nodelist_p,
                       int           *nodelistcnt_p);


extern int nx_map_nodelist(struct pvproc *pvp,
                           int            node_array[],
                           int            node_count,
                           APPLINFO_T    *applinfo_p,
                           LP_MAP_T      *nodelist_p,
                           int           *nodelistcnt_p);

extern int nx_get_info(struct pvproc *pvp,
                       APPLINFO_T *applinfo,
                       LP_MAP_T *nodelist,
                       int      *nodelistcnt,
                       int       flag,
                       uid_t     euid,
                       int      *err);
#else

extern int nx_map_node();
extern int nx_map_nodelist();
extern int nx_get_info();

#endif /* __STDC__ */

#endif /* NX */

extern struct pvproc_ops_vector rpvproc_ops_table;
extern mach_port_t tnc_bestow_vproc_port(struct vproc *v, int flag);
extern int dvp_child_join_pgrp_setup(struct vproc *vp, int *seqnop);

int
rfork(
	struct vproc		*vp,
	node_t			ch_node,
	thread_state_t		ch_state,
	unsigned int		ch_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name,
	task_t			*ch_taskp,		/* OUT */
	thread_t		*ch_threadp,		/* OUT */
	pid_t			*ch_pidp)		/* OUT */
{
	register struct vproc	*vc;
	register struct pvproc	*pvp;
	mach_port_t		server_port;
	mach_port_t		ch_vproc_port;
	struct rf_data		rf_pp_data;
	task_t			pp_task;
	char			pp_comm[MAXCOMLEN + 1];
	char			pp_logname[MAXLOGNAME];
	int			error;
	mach_port_t		mmap_pagers[TNC_MAX_MMAP_REGIONS];
	struct mmap_struct	mmap_structs[TNC_MAX_MMAP_REGIONS];
	unsigned int		mmap_count;
	mach_port_t		rdir_port;
	mach_port_t		cdir_port;
	kern_return_t		ret;
	int			parent_seqno;
#ifdef NX
        int                     nodelistcnt = 0;
        LP_MAP_T                nodelist = (LP_MAP_T)NULL;
        APPLINFO_T              applinfo;
#endif /* NX */
#ifdef PARACORE
	mach_port_t		exec_rdir_port;
	mach_port_t		exec_cdir_port;
	char			exec_prg_name[PATH_MAX];
#endif /* PARACORE */
		

	pvp = PVP(vp);
#ifdef NX
        /*
         * Check if this is a NX application and a parition has
         * been allocated to it.  If so, convert the node number from
         * a logical node number to a physical node number and pass it
         * on.
         */
        rf_pp_data.nx_flags = 0;
        if (nx_in_partition(vp)) {
            rf_pp_data.nx_flags = pvp->pvp_flag &
                                 (PV_NX_PARTITIONED | 
				  PV_NX_APPLICATION |
				  PV_NX_NO_SIGCHLD |
				  PV_NX_ATH_PGRP);

            ch_node = nx_map_node(pvp, ch_node, &applinfo,
                                  &nodelist, &nodelistcnt);
					/* allocates new pages of OOL VM */
            if (ch_node == -1) {
                return(EINVAL);
            }
        }
#endif /* NX */

	/*
	 * Doing an rfork to ourselves is disallowed, at least for now.
	 */
	if (ch_node == this_node) {
		error = EINVAL;
		goto out2;
	}

	/*
	 * Seize the vproc during the fork operation.  We must prevent
	 * anyone from changing the parent's process group id during
	 * the fork; otherwise the new child may not wind up in the same
	 * process group as its parent.  We don't usually like to hold
	 * this lock while an RPC is occurring, but signal delivery is
	 * not blocked by this lock; only exit's, setpgid's and setsid's
	 * are prevented.
	 */
	VPROC_LOCK_EXCL(vp, "rfork: parent");

	/*
	 * When creating new child processes, the management of that new
	 * child's joining its parent's process group requires special
	 * handling to prevent the child from missing signals directed
	 * at the entire process group.  Routines dvp_child_join_pgrp_setup()
	 * and dvp_child_join_pgrp_end() perform this special handling.
	 */
	error = dvp_child_join_pgrp_setup(vp, &parent_seqno);
	if (error != ESUCCESS) {
		goto out;		/* ERESTART a distinct possiblilty */
	}

	/*
	 * Get the port to the remote server.
	 */
	error = tnc_get_server_port(ch_node, &server_port);
	if (error != ESUCCESS) {
		error = EINVAL;
		goto out;
	}

	/*
	 * Load parent task's info into the structure (which contains
	 * all ints) and put the character data (2 items) into places
	 * of their own -- all for passing via MiG.
	 */
	rfork_pproc_load_msg(pvp->pvp_pproc, &rf_pp_data, &pp_task,
			      mmap_structs, mmap_pagers, &mmap_count,
			      &rdir_port, &cdir_port,
			      pp_comm, pp_logname
#ifdef PARACORE
			      , &exec_rdir_port, &exec_cdir_port,
			      exec_prg_name 
#endif /* PARACORE */
			      );
	rf_pp_data.rf_p_pid = vp->vp_pid;	/* parent's pid */
	rf_pp_data.rf_p_pgid = pvp->pvp_pgid;	/* parent's gid */
	rf_pp_data.rf_p_sid = pvp->pvp_sid;	/* parent's sess id */
	rf_pp_data.rf_p_pgrp_mem_seqno = parent_seqno;	/* parent's seqno */

	/*
	 * Perform the rfork to the remote node.
	 */
	ux_server_thread_blocking();
	ret = cli_rfork(server_port, 
			&error,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
			    ? tnc_bestow_vproc_port(VPROCPTR(pvp->pvp_pgid), 0)
			    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			ch_taskp,
			ch_threadp,
			&ch_vproc_port, 
			ch_pidp,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname
#ifdef NX
,                       applinfo,
                        nodelist, nodelistcnt
#endif /* NX */
#ifdef PARACORE
			, exec_rdir_port, exec_cdir_port,
			exec_prg_name, strlen(exec_prg_name) + 1
#endif /* PARACORE */
			);
	ux_server_thread_unblocking();

	if (ret != KERN_SUCCESS)
		panic("rfork: cli_rfork RPCfail ret=0x%x", ret);

	if (error != ESUCCESS) {
		goto out;
	}

	/*
	 * Get a vproc for the newly created child, and put it on its
	 * parent's vproc list.
	 *
	 * Normally we should now do a VPROC_HOLD(s) to keep a 
	 * reference.  Instead we just skip doing
	 * a VPROC_RELEASE(g) for the LOCATE_VPROC_PID() below.
	 */
	vc = LOCATE_VPROC_PID(*ch_pidp);
	if (vc == 0)
		panic("rfork: cannot find child vproc");
	tnc_install_vproc_port(vc, ch_vproc_port, 0);
	PVP(vc)->pvp_childl = pvp->pvp_head_childl;
	pvp->pvp_head_childl = vc;

	error = ESUCCESS;

out:
	VPROC_UNLOCK_EXCL(vp, "rfork: parent");

out2:
#ifdef NX
        if (nodelist != (LP_MAP_T)NULL) {	/* dealloc OOL mem if any */
        	vm_deallocate_strict(mach_task_self(), (vm_address_t)nodelist,
				nodelistcnt * sizeof(LP_MAP_ENTRY_T));
        }
#endif /* NX */

	return(error);
}


int
migrate(
	struct vproc		*vm,
	node_t			new_node,
	thread_state_t		cur_state,
	unsigned int		cur_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name,
	mach_port_t		old_task_name)
{
	register struct vproc	*w;
	register struct pvproc	*pvm;
	mach_port_t		server_port;
	struct mi_data		mi_data;
	task_t			old_task;
	mach_port_t		cur_proc_port;
	mach_port_t		cur_cred_port;
	char			cur_comm[MAXCOMLEN + 1];
	char			cur_logname[MAXLOGNAME];
	int			error;
	pid_t			child_pid[MAX_VPROC_LIST];
	mach_port_t		child_port[MAX_VPROC_LIST];
	int			child_stat[MAX_VPROC_LIST];
	unsigned int		child_cnt;
	pid_t			foster_child_pid[MAX_VPROC_LIST];
	mach_port_t		foster_child_port[MAX_VPROC_LIST];
	unsigned int		foster_child_cnt;
	pid_t			pgrp_member_pid[MAX_VPROC_LIST];
	mach_port_t		pgrp_member_port[MAX_VPROC_LIST];
	unsigned int		pgrp_member_cnt;
	pid_t			sess_member_pid[MAX_VPROC_LIST];
	mach_port_t		sess_member_port[MAX_VPROC_LIST];
	unsigned int		sess_member_cnt;
	mach_port_t		ret_vproc_port;
	mach_port_t		ret_cred_port;
	mach_port_t		ret_proc_port;
	mach_port_t		mmap_pagers[TNC_MAX_MMAP_REGIONS];
	struct mmap_struct	mmap_structs[TNC_MAX_MMAP_REGIONS];
	unsigned int		mmap_count;
	node_t			cred_cache[CREDENTIALS_CACHE_SIZE];
	unsigned int		cred_cache_size = CREDENTIALS_CACHE_SIZE;
	node_t			*cred_cachep = cred_cache;
	mach_port_t		rdir_port;
	mach_port_t		cdir_port;
	mach_port_t		task_deadname_notify_port;
	mach_port_t		dummy_port;
	kern_return_t		ret;
	boolean_t		longrpc;
	pid_t			*child_pidp = NULL;
	mach_port_t		*child_portp = NULL;
	int			*child_statp = NULL;
	pid_t			*foster_child_pidp = NULL;
	mach_port_t		*foster_child_portp = NULL;
	pid_t			*pgrp_member_pidp = NULL;
	mach_port_t		*pgrp_member_portp = NULL;
	pid_t			*sess_member_pidp = NULL;
	mach_port_t		*sess_member_portp = NULL;
	pid_t			*pidp;
	mach_port_t		*portp;
	int			*statp;
#ifdef NX
        int                     nodelistcnt = 0;
        LP_MAP_T                nodelist = (LP_MAP_T)NULL;
        APPLINFO_T              applinfo;
#endif /* NX */
#ifdef PARACORE
	mach_port_t		exec_rdir_port;
	mach_port_t		exec_cdir_port;
	char			exec_prg_name[PATH_MAX];
#endif /* PARACORE */

	pvm = PVP(vm);

#ifdef NX
        /*
         * Check if this is a NX application and a parition has
         * been allocated to it.  If so, convert the node number from
         * a logical node number to a physical node number and pass it
         * on.
         */
        mi_data.nx_flags = 0;
        if (nx_in_partition(vm)) {
            mi_data.nx_flags = pvm->pvp_flag &
                              (PV_NX_PARTITIONED | 
			       PV_NX_APPLICATION |
			       PV_NX_NO_SIGCHLD |
				PV_NX_ATH_PGRP);

            new_node = nx_map_node(pvm, new_node, &applinfo,
                                  &nodelist, &nodelistcnt);
					/* allocates new pages of OOL VM */
            if (new_node == -1) {
                return(EINVAL);
            }
        }
#endif /* NX */

	/*
	 * Doing a migrate to ourselves is disallowed, at least for now.
	 */
	if (new_node == this_node) {
		error = EINVAL;
		goto out2;
	}

	/*
	 * Get the port to the remote server.
	 */
	error = tnc_get_server_port(new_node, &server_port);
	if (error != ESUCCESS) {
		error = EINVAL;
		goto out2;
	}

	/*
	 * Bring the migrating task into a quiescent and safe state.
	 */
	error = rtask_pproc_quiesce(pvm->pvp_pproc);
	if (error != ESUCCESS) {
		goto out2;
	}

	/*
	 * Lock the vproc up for the duration of the migration.
	 */
	VPROC_START_MOVEMENT(vm, "migrate");
	VPROC_LOCK_EXCL(vm, "migrate");
	VPROC_LOCK_PGRP_LIST_EXCL(vm, "migrate");
	VPROC_LOCK_SESSION_LIST_EXCL(vm, "migrate");
	VPROC_LOCK_FOSTER_LIST_EXCL(vm, "migrate");

	/*
	 * Load migrating task's info into the structure (which contains
	 * all ints) and put the character data (2 items) into places
	 * of their own -- all for passing via MiG.
	 */
	error = migrate_pproc_load_msg(pvm->pvp_pproc, &mi_data,
			       	       &old_task, &cur_proc_port,
			       	       &cur_cred_port, 
				       &cred_cachep, &cred_cache_size,
			       	       mmap_structs, mmap_pagers, &mmap_count,
			       	       &rdir_port, &cdir_port,
			       	       cur_comm, cur_logname
#ifdef PARACORE
				       , &exec_rdir_port, &exec_cdir_port,
				       exec_prg_name 
#endif /* PARACORE */
			       	       );
	if (error != ESUCCESS) {
		goto out;
	}

	mi_data.mi_p_pid = vm->vp_pid;		/* current pid */
	mi_data.mi_p_ppid = pvm->pvp_ppid;	/* parent's pid */
	mi_data.mi_p_foster_ppid = pvm->pvp_foster_ppid; /* foster parent's */
	mi_data.mi_p_pgid = pvm->pvp_pgid;	/* current gid */
	mi_data.mi_p_sid = pvm->pvp_sid;	/* current sess id */
	mi_data.mi_p_pgrp_mem_seqno = pvm->pvp_pgrp_mem_seqno;
						/* current pgrp sig mem seqno */
	mi_data.mi_p_pgrp_ldr_seqno = pvm->pvp_pgrp_ldr_seqno;
						/* current pgrp sig ldr seqno */
	mi_data.mi_pp_pgid = pvm->pvp_pp_pgid;	/* parent's gid */
	mi_data.mi_pp_sid = pvm->pvp_pp_sid;	/* parent's sess id */

	/*
	 * Figure out if we're doing a long-form rpc or not.
	 */
	vproc_list_counts(vm, 
			  &child_cnt, &foster_child_cnt,
			  &pgrp_member_cnt, &sess_member_cnt);
	longrpc =  (cred_cache_size > CREDENTIALS_CACHE_SIZE ||
			child_cnt > MAX_VPROC_LIST ||
			foster_child_cnt > MAX_VPROC_LIST ||
			pgrp_member_cnt > MAX_VPROC_LIST ||
			sess_member_cnt > MAX_VPROC_LIST);

	/*
	 * Create an array with all the child vproc information in it.
	 */
	if (child_cnt > MAX_VPROC_LIST) {
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &child_pidp,
				child_cnt * sizeof(pid_t),
				TRUE);
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &child_portp,
				child_cnt * sizeof(mach_port_t),
				TRUE);
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &child_statp,
				child_cnt * sizeof(int),
				TRUE);
	} else {
		child_pidp = child_pid;
		child_portp = child_port;
		child_statp = child_stat;
	}
	for (w = pvm->pvp_head_childl, pidp = child_pidp, 
				       portp = child_portp,
				       statp = child_statp;
			w != NULL;
			w = PVP(w)->pvp_childl, pidp++, portp++, statp++) {
		*pidp = w->vp_pid;
		*portp = tnc_bestow_vproc_port(w, 0);
		*statp = PVP(w)->pvp_flag & (PV_SZOMB|PV_SSTOP);
	}

	/*
	 * Create an array with all the foster child vproc information in it.
	 */
	if (foster_child_cnt > MAX_VPROC_LIST) {
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &foster_child_pidp,
				foster_child_cnt * sizeof(pid_t),
				TRUE);
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &foster_child_portp,
				foster_child_cnt * sizeof(mach_port_t),
				TRUE);
	} else {
		foster_child_pidp = foster_child_pid;
		foster_child_portp = foster_child_port;
	}
	for (w = pvm->pvp_head_foster_childl, pidp = foster_child_pidp, 
				              portp = foster_child_portp;
			w != NULL;
			w = PVP(w)->pvp_foster_childl, pidp++, portp++) {
		*pidp = w->vp_pid;
		*portp = tnc_bestow_vproc_port(w, 0);
	}


	/*
	 * Create an array with all the pgrp member information in it
	 * (if the process is a pgrp leader).
	 */
	if (pvm->pvp_flag & PV_PGRPLEADER) {
		if (pgrp_member_cnt > MAX_VPROC_LIST) {
			vm_allocate_strict(mach_task_self(),
					(vm_address_t *) &pgrp_member_pidp,
					pgrp_member_cnt * sizeof(pid_t),
					TRUE);
			vm_allocate_strict(mach_task_self(),
					(vm_address_t *) &pgrp_member_portp,
					pgrp_member_cnt*sizeof(mach_port_t),
					TRUE);
		} else {
			pgrp_member_pidp = pgrp_member_pid;
			pgrp_member_portp = pgrp_member_port;
		}
		for (w = pvm->pvp_head_pgrpl, pidp = pgrp_member_pidp, 
					      portp = pgrp_member_portp;
				w != NULL;
				w = PVP(w)->pvp_pgrpl, pidp++, portp++) {
			*pidp = w->vp_pid;
			*portp = tnc_bestow_vproc_port(w, 0);
		}
	}

	/*
	 * Create an array with all the session member information in it
	 * (if the process is a session leader).
	 */
	if (pvm->pvp_flag & PV_SESSIONLEADER) {
		if (sess_member_cnt > MAX_VPROC_LIST) {
			vm_allocate_strict(mach_task_self(),
					(vm_address_t *) &sess_member_pidp,
					sess_member_cnt * sizeof(pid_t),
					TRUE);
			vm_allocate_strict(mach_task_self(),
					(vm_address_t *) &sess_member_portp,
					sess_member_cnt * sizeof(mach_port_t),
					TRUE);
		} else {
			sess_member_pidp = sess_member_pid;
			sess_member_portp = sess_member_port;
		}
		for (w = pvm->pvp_sessionl, pidp = sess_member_pidp, 
					    portp = sess_member_portp;
				w != NULL;
				w = PVP(w)->pvp_sessionl, pidp++, portp++) {
			*pidp = w->vp_pid;
			*portp = tnc_bestow_vproc_port(w, 0);
		}
	}

	/*
	 * Before going remote to terminate the old local task
	 * (from the new emulator), cancel its deadname notification.
	 */
	ret = mach_port_request_notification(mach_task_self(),
					     old_task,
					     MACH_NOTIFY_DEAD_NAME,
					     0,
					     MACH_PORT_NULL,
					     MACH_MSG_TYPE_MOVE_SEND_ONCE,
					     &task_deadname_notify_port);
	ASSERT(ret == KERN_SUCCESS);

	/*
	 * Perform the migrate to the remote node. If the long form of
	 * RPC is used, then make sure to deallocate the necessary
	 * memory.
	 */
	if (!longrpc) {
		ux_server_thread_blocking();
		ret = cli_migrate(server_port, 
			&error,
			vm->vp_pid,
			(pvm->pvp_flag & PV_PGRPLEADER) != 0,
			tnc_bestow_vproc_port(vm, BESTOW_RECEIVE_RT), 
			vproc_port_name,
			cur_proc_port,
			cur_cred_port, cred_port_name, 
			cred_cache, cred_cache_size,
			tnc_bestow_vproc_port(VPROCPTR(pvm->pvp_ppid), 0),
			(pvm->pvp_foster_ppid && 
				    pvm->pvp_foster_ppid != pvm->pvp_ppid)
			    ? tnc_bestow_vproc_port(
				    VPROCPTR(pvm->pvp_foster_ppid),0)
			    : MACH_PORT_NULL,
			pvm->pvp_pgid
			    ? tnc_bestow_vproc_port(VPROCPTR(pvm->pvp_pgid),0)
			    : MACH_PORT_NULL,
			(pvm->pvp_flag & PV_PGRPLEADER) && pvm->pvp_sid
			    ? tnc_bestow_vproc_port(VPROCPTR(pvm->pvp_sid),0)
			    : MACH_PORT_NULL,
			child_pid, child_cnt,
			child_port, child_cnt,
			child_stat, child_cnt,
			foster_child_pid, foster_child_cnt,
			foster_child_port, foster_child_cnt,
			pgrp_member_pid, pgrp_member_cnt,
			pgrp_member_port, pgrp_member_cnt,
			sess_member_pid, sess_member_cnt,
			sess_member_port, sess_member_cnt,
			pvm->pvp_jobc,
			pvm->pvp_cttynode,
			rdir_port, cdir_port,
			old_task, old_task_name,
			cur_state, cur_state_count,
			&mi_data,
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			cur_comm, cur_logname,
			&ret_vproc_port, &ret_cred_port, &ret_proc_port
#ifdef NX
			,applinfo,
                         nodelist, nodelistcnt
#endif /* NX */
#ifdef PARACORE
			, exec_rdir_port, exec_cdir_port,
			exec_prg_name, strlen(exec_prg_name) + 1
#endif /* PARACORE */
			 );
		ux_server_thread_unblocking();
		if (ret != KERN_SUCCESS)
			panic("migrate: cli_migrate failure ret=0x%x", ret);
	} else {
		ux_server_thread_blocking();
		ret = cli_migrate_long(server_port, 
			&error,
			vm->vp_pid,
			(pvm->pvp_flag & PV_PGRPLEADER) != 0,
			tnc_bestow_vproc_port(vm, BESTOW_RECEIVE_RT), 
			vproc_port_name,
			cur_proc_port,
			cur_cred_port, cred_port_name, 
			cred_cachep, cred_cache_size,
			tnc_bestow_vproc_port(VPROCPTR(pvm->pvp_ppid), 0),
			(pvm->pvp_foster_ppid && 
				    pvm->pvp_foster_ppid != pvm->pvp_ppid)
			    ? tnc_bestow_vproc_port(
				    VPROCPTR(pvm->pvp_foster_ppid),0)
			    : MACH_PORT_NULL,
			pvm->pvp_pgid
			    ? tnc_bestow_vproc_port(VPROCPTR(pvm->pvp_pgid),0)
			    : MACH_PORT_NULL,
			(pvm->pvp_flag & PV_PGRPLEADER) && pvm->pvp_sid
			    ? tnc_bestow_vproc_port(VPROCPTR(pvm->pvp_sid),0)
			    : MACH_PORT_NULL,
			child_pidp, child_cnt,
			child_portp, child_cnt,
			child_statp, child_cnt,
			foster_child_pidp, foster_child_cnt,
			foster_child_portp, foster_child_cnt,
			pgrp_member_pidp, pgrp_member_cnt,
			pgrp_member_portp, pgrp_member_cnt,
			sess_member_pidp, sess_member_cnt,
			sess_member_portp, sess_member_cnt,
			pvm->pvp_jobc,
			pvm->pvp_cttynode,
			rdir_port, cdir_port,
			old_task, old_task_name,
			cur_state, cur_state_count,
			&mi_data,
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			cur_comm, cur_logname,
			&ret_vproc_port, &ret_cred_port, &ret_proc_port
#ifdef NX
                       ,applinfo,
                        nodelist, nodelistcnt
#endif /* NX */
#ifdef PARACORE
			, exec_rdir_port, exec_cdir_port,
			exec_prg_name, strlen(exec_prg_name) + 1
#endif /* PARACORE */
			);
		ux_server_thread_unblocking();
		if (ret != KERN_SUCCESS)
			panic("migrate: cli_migrate_long RPCfail ret=0x%x",ret);
	}
	if (error != ESUCCESS) {
		int	dummy;

		/* 
		 * If the migration call failed for some reason, put all
		 * the ports back to the correct names and make sure they
		 * are in the relevent port set.
		 */
		ASSERT(ret_vproc_port == vproc_to_port_lookup(vm));
		tnc_install_vproc_port(vm, ret_vproc_port, INSTALL_RECEIVE_RT);
		pvm->pvp_movement_lock.ml_vproc_sequence = -1;
		pvm->pvp_movement_lock.ml_proc_sequence = 0;
		{
			mach_port_status_t	proc_rstat;
			mach_port_status_t	vproc_rstat;
			kern_return_t		ret;
	
			ret = mach_port_get_receive_status(mach_task_self(),
							   ret_proc_port,
							   &proc_rstat);
			if (ret != KERN_SUCCESS)
				panic("mach_port_get_receive_status for "
				      "proc port returned 0x%x",ret);
			ret = mach_port_get_receive_status(mach_task_self(),
							   (mach_port_t) vm,
							   &vproc_rstat);
			if (ret != KERN_SUCCESS)
				panic("mach_port_get_receive_status for "
				      "vproc port returned 0x%x",ret);
	
			ASSERT((proc_rstat.mps_seqno+vproc_rstat.mps_seqno)==0);
		}
		ux_server_add_port(vproc_to_port_lookup(vm));
		reinstall_process_ports(ret_proc_port,
					ret_cred_port,
					cred_cache,cred_cache_size);
		/*
		 * Re-install the task port deadname notification.
		 */
		ret = mach_port_request_notification(mach_task_self(),
						old_task,
						MACH_NOTIFY_DEAD_NAME,
						1,
						task_deadname_notify_port,
						MACH_MSG_TYPE_MOVE_SEND_ONCE,
						&dummy_port);
		ASSERT(ret == KERN_SUCCESS);

		VPROC_UNLOCK_FOSTER_LIST_EXCL(vm, "migrate");
		VPROC_UNLOCK_SESSION_LIST_EXCL(vm, "migrate");
		VPROC_UNLOCK_PGRP_LIST_EXCL(vm, "migrate");
		VPROC_UNLOCK_EXCL(vm, "migrate");
		VPROC_END_MOVEMENT(vm, "migrate");
		(void) PVPSOP_REMOTE_VPROC_FREE(new_node, vm);

		/* Let the calling task continue */
		(void) rtask_pproc_unquiesce(pvm->pvp_pproc);

		goto out2;
	}

	/* Throw away the deadname notification port right */
	ret = mach_port_deallocate(mach_task_self(),task_deadname_notify_port);
	ASSERT(ret == KERN_SUCCESS);

	/* No longer local */
	VPROC_LOCK_FLAG(vm, "migrate");
	pvm->pvp_flag &= ~PV_IS_LOCAL;
	VPROC_UNLOCK_FLAG(vm, "migrate");

	/*
	 * Adjust reference counts for parent, pgrp and session leader of
	 * the migrated process.
	 */
	VPROC_RELEASE(VPROCPTR(pvm->pvp_ppid),
		      "migrate (parent on child node)");
	if (pvm->pvp_foster_ppid != 0 && pvm->pvp_foster_ppid != pvm->pvp_ppid)
		VPROC_RELEASE(VPROCPTR(pvm->pvp_foster_ppid),
			      "migrate (foster parent on foster child node)");
	if (pvm->pvp_pgid != 0)
		VPROC_RELEASE(VPROCPTR(pvm->pvp_pgid),
			      "migrate (pgrp leader on member node)");

	/*
	 * The parent-child-sibling list is now on the remote node.  Clean
	 * up the list on this node.
	 */
	while (pvm->pvp_head_childl != NULL) {
		register struct vproc *vc = pvm->pvp_head_childl;
		pvm->pvp_head_childl = PVP(vc)->pvp_childl;
		PVP(vc)->pvp_childl = NULL;
		VPROC_RELEASE(vc, "migrate (child on parent node)");
	}

	/*
	 * The foster child list is now on the remote node.  Clean
	 * up the list on this node.
	 */
	while (pvm->pvp_head_foster_childl != NULL) {
		register struct vproc *vc = pvm->pvp_head_foster_childl;
		pvm->pvp_head_foster_childl = PVP(vc)->pvp_foster_childl;
		PVP(vc)->pvp_foster_childl = NULL;
		VPROC_RELEASE(vc, "migrate (foster child on parent node)");
	}

	/*
	 * The pgrp list is now on the remote node.  Clean
	 * up the list on this node.
	 */
	if (pvm->pvp_flag & PV_PGRPLEADER) {
		VPROC_LOCK_FLAG(vm, "migrate");
		pvm->pvp_flag &= ~PV_PGRPLEADER;
		VPROC_UNLOCK_FLAG(vm, "migrate");
		VPROC_RELEASE(VPROCPTR(pvm->pvp_sid),
			      "migrate(session leader on member node)");
		while (pvm->pvp_head_pgrpl != NULL) {
			register struct vproc *vg = pvm->pvp_head_pgrpl;
			pvm->pvp_head_pgrpl = PVP(vg)->pvp_pgrpl;
			PVP(vg)->pvp_pgrpl = NULL;
			VPROC_RELEASE(vg,"migrate(pgrp member on leader node)");
			VPROC_RELEASE(vm,"migrate(pgrp leader on leader node)");
		}
	}

	/*
	 * The session list is now on the remote node.  Clean
	 * up the list on this node.
	 */
	if (pvm->pvp_flag & PV_SESSIONLEADER) {
		VPROC_LOCK_FLAG(vm, "migrate");
		pvm->pvp_flag &= ~PV_SESSIONLEADER;
		VPROC_UNLOCK_FLAG(vm, "migrate");
		while (pvm->pvp_sessionl != NULL) {
			register struct vproc *vs = pvm->pvp_sessionl;
			pvm->pvp_sessionl = PVP(vs)->pvp_sessionl;
			PVP(vs)->pvp_sessionl = NULL;
			VPROC_RELEASE(vs, "migrate(sess member on ldr node)");
			VPROC_RELEASE(vm, "migrate(sess leader on ldr node)");
		}
	}

	/*
	 * Now switch the pvproc ops to the remote version, since we've
	 * successfully migrated.
	 */
	pvm->pvp_ops = &rpvproc_ops_table;

	/*
	 * Clean up the local process
	 * (the process is already running on the remote node).
	 */
	rtask_pproc_remove(pvm->pvp_pproc);


	/*
	 * Undo all the locks at the beginning of the routine.
	 */
out:
	VPROC_UNLOCK_FOSTER_LIST_EXCL(vm, "migrate");
	VPROC_UNLOCK_SESSION_LIST_EXCL(vm, "migrate");
	VPROC_UNLOCK_PGRP_LIST_EXCL(vm, "migrate");
	VPROC_UNLOCK_EXCL(vm, "migrate");
	VPROC_END_MOVEMENT(vm, "migrate");

out2:
	if (cred_cache_size > CREDENTIALS_CACHE_SIZE && *cred_cachep != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) cred_cachep,
				cred_cache_size * sizeof(node_t));
	}
	if (child_cnt > MAX_VPROC_LIST && *child_pidp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) child_pidp,
				child_cnt * sizeof(pid_t));
	}
	if (child_cnt > MAX_VPROC_LIST && *child_portp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) child_portp,
				child_cnt * sizeof(mach_port_t));
	}
	if (child_cnt > MAX_VPROC_LIST && *child_statp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) child_statp,
				child_cnt * sizeof(int));
	}
	if (foster_child_cnt > MAX_VPROC_LIST && *foster_child_pidp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) foster_child_pidp,
				foster_child_cnt * sizeof(pid_t));
	}
	if (foster_child_cnt > MAX_VPROC_LIST && *foster_child_portp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) foster_child_portp,
				foster_child_cnt * sizeof(mach_port_t));
	}
	if (pgrp_member_cnt > MAX_VPROC_LIST && *pgrp_member_pidp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) pgrp_member_pidp,
				pgrp_member_cnt * sizeof(pid_t));
	}
	if (pgrp_member_cnt > MAX_VPROC_LIST && *pgrp_member_portp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) pgrp_member_portp,
				pgrp_member_cnt * sizeof(mach_port_t));
	}
	if (sess_member_cnt > MAX_VPROC_LIST && *sess_member_pidp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) sess_member_pidp,
				sess_member_cnt * sizeof(pid_t));
	}
	if (sess_member_cnt > MAX_VPROC_LIST && *sess_member_portp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) sess_member_portp,
				sess_member_cnt * sizeof(mach_port_t));
	}

#ifdef NX
	if (nodelist != (LP_MAP_T)NULL) {
		vm_deallocate_strict(mach_task_self(), (vm_address_t)nodelist,
				nodelistcnt*sizeof(LP_MAP_ENTRY_T));
	}
#endif /* NX */

	/*
	 * Process is no longer resident here, so decrement its use count, if
	 * there was no error.  This is done late in this routine on purpose.
	 */
	if (error == ESUCCESS) {
		VPROC_RELEASE(vm, "migrate (active)");
	}

	return(error);
}


int
rexecve(
	struct vproc		*ve,
	char			*fname,
	unsigned int		fname_count,
	node_t			new_node,
	thread_state_t		cur_state,
	unsigned int		cur_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name,
	mach_port_t		old_task_name,
	vm_address_t		arg_addr,
	vm_size_t		arg_size,
	int			arg_count,
	int			env_count,
	unsigned int		char_count)
{
	register struct vproc	*w;
	register struct pvproc	*pve;
	mach_port_t		server_port;
	struct re_data		re_data;
	task_t			old_task;
	mach_port_t		cur_proc_port;
	mach_port_t		cur_cred_port;
	char			cur_logname[MAXLOGNAME + 1];
	int			error;
	kern_return_t		ret;
	pid_t			child_pid[MAX_VPROC_LIST];
	mach_port_t		child_port[MAX_VPROC_LIST];
	int			child_stat[MAX_VPROC_LIST];
	unsigned int		child_cnt;
	pid_t			foster_child_pid[MAX_VPROC_LIST];
	mach_port_t		foster_child_port[MAX_VPROC_LIST];
	unsigned int		foster_child_cnt;
	pid_t			pgrp_member_pid[MAX_VPROC_LIST];
	mach_port_t		pgrp_member_port[MAX_VPROC_LIST];
	unsigned int		pgrp_member_cnt;
	pid_t			sess_member_pid[MAX_VPROC_LIST];
	mach_port_t		sess_member_port[MAX_VPROC_LIST];
	unsigned int		sess_member_cnt;
	mach_port_t		ret_vproc_port;
	mach_port_t		ret_cred_port;
	mach_port_t		ret_proc_port;
	mach_port_t		mmap_pagers[TNC_MAX_MMAP_REGIONS];
	struct mmap_struct	mmap_structs[TNC_MAX_MMAP_REGIONS];
	unsigned int		mmap_count;
	node_t			cred_cache[CREDENTIALS_CACHE_SIZE];
	unsigned int		cred_cache_size = CREDENTIALS_CACHE_SIZE;
	node_t			*cred_cachep = cred_cache;
	mach_port_t		rdir_port;
	mach_port_t		cdir_port;
	mach_port_t		task_deadname_notify_port;
	mach_port_t		dummy_port;
	boolean_t		longrpc;
	pid_t			*child_pidp = NULL;
	mach_port_t		*child_portp = NULL;
	int			*child_statp = NULL;
	pid_t			*foster_child_pidp = NULL;
	mach_port_t		*foster_child_portp = NULL;
	pid_t			*pgrp_member_pidp = NULL;
	mach_port_t		*pgrp_member_portp = NULL;
	pid_t			*sess_member_pidp = NULL;
	mach_port_t		*sess_member_portp = NULL;
	pid_t			*pidp;
	mach_port_t		*portp;
	int			*statp;
#ifdef NX
        int                     nodelistcnt = 0;
        LP_MAP_T                nodelist = (LP_MAP_T)NULL;
        APPLINFO_T              applinfo;
#endif /* NX */

	pve = PVP(ve);
#ifdef NX
        /*
         * Check if this is a NX application and a parition has
         * been allocated to it.  If so, convert the node number from
         * a logical node number to a physical node number and pass it
         * on.
         */
        re_data.nx_flags = 0;
        if (nx_in_partition(ve)) {
            re_data.nx_flags = pve->pvp_flag &
                              (PV_NX_PARTITIONED | 
			       PV_NX_APPLICATION |
			       PV_NX_NO_SIGCHLD |
				PV_NX_ATH_PGRP	);

            new_node = nx_map_node(pve, new_node, &applinfo,
                                  &nodelist, &nodelistcnt);
					/* allocates new pages of OOL VM */
            if (new_node == -1) {
                return(EINVAL);
            }
        }
#endif /* NX */

	/*
	 * Doing an rexecve to ourselves is disallowed, at least for now.
	 */
	if (new_node == this_node) {
		error = EINVAL;
		goto out;
	}

	/*
	 * Get the port to the remote server.
	 */
	error = tnc_get_server_port(new_node, &server_port);
	if (error != ESUCCESS) {
		error = EINVAL;
		goto out;
	}

	/*
	 * Bring the migrating task into a quiescent and safe state.
	 */
	error = rtask_pproc_quiesce(pve->pvp_pproc);
	if (error != ESUCCESS) {
		return(error);
	}

	/*
	 * Lock the vproc up for the duration of the rexec.
	 */
	VPROC_START_MOVEMENT(ve, "rexecve");
	VPROC_LOCK_EXCL(ve, "rexecve");
	VPROC_LOCK_PGRP_LIST_EXCL(ve, "rexecve");
	VPROC_LOCK_SESSION_LIST_EXCL(ve, "rexecve");
	VPROC_LOCK_FOSTER_LIST_EXCL(ve, "rexecve");

	/*
	 * Load rexecing task's info into the structure (which contains
	 * all ints) and put the character data (2 items) into places
	 * of their own -- all for passing via MiG.
	 */
	error = rexecve_pproc_load_msg(pve->pvp_pproc, &re_data,
			       	       &old_task, &cur_proc_port,
			       	       &cur_cred_port, 
				       &cred_cachep, &cred_cache_size,
			       	       mmap_structs, mmap_pagers, &mmap_count,
			       	       &rdir_port, &cdir_port,
			       	       cur_logname);
	if (error != ESUCCESS) {
		goto out2;
	}
	re_data.re_p_pid = ve->vp_pid;		/* current pid */
	re_data.re_p_ppid = pve->pvp_ppid;	/* parent's pid */
	re_data.re_p_foster_ppid = pve->pvp_foster_ppid; /* foster parent's */
	re_data.re_p_pgid = pve->pvp_pgid;	/* current gid */
	re_data.re_p_sid = pve->pvp_sid;	/* current sess id */
	re_data.re_p_pgrp_mem_seqno = pve->pvp_pgrp_mem_seqno;
						/* current pgrp sig mem seqno */
	re_data.re_p_pgrp_ldr_seqno = pve->pvp_pgrp_ldr_seqno;
						/* current pgrp sig ldr seqno */
	re_data.re_pp_pgid = pve->pvp_pp_pgid;	/* parent's gid */
	re_data.re_pp_sid = pve->pvp_pp_sid;	/* parent's sess id */

	/*
	 * Figure out if we're doing a long-form rpc or not.
	 */
	vproc_list_counts(ve, 
			  &child_cnt, &foster_child_cnt,
			  &pgrp_member_cnt, &sess_member_cnt);
	longrpc =  (cred_cache_size > CREDENTIALS_CACHE_SIZE ||
			child_cnt > MAX_VPROC_LIST ||
			foster_child_cnt > MAX_VPROC_LIST ||
			pgrp_member_cnt > MAX_VPROC_LIST ||
			sess_member_cnt > MAX_VPROC_LIST);

	/*
	 * Create an array with all the child vproc information in it.
	 */
	if (child_cnt > MAX_VPROC_LIST) {
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &child_pidp,
				child_cnt * sizeof(pid_t),
				 TRUE);
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &child_portp,
				child_cnt * sizeof(mach_port_t),
				TRUE);
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &child_statp,
				child_cnt * sizeof(int),
				TRUE);
	} else {
		child_pidp = child_pid;
		child_portp = child_port;
		child_statp = child_stat;
	}
	for (w = pve->pvp_head_childl, pidp = child_pidp, 
				       portp = child_portp,
				       statp = child_statp;
			w != NULL;
			w = PVP(w)->pvp_childl, pidp++, portp++, statp++) {
		*pidp = w->vp_pid;
		*portp = tnc_bestow_vproc_port(w, 0);
		*statp = PVP(w)->pvp_flag & (PV_SZOMB|PV_SSTOP);
	}

	/*
	 * Create an array with all the foster child vproc information in it.
	 */
	if (foster_child_cnt > MAX_VPROC_LIST) {
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &foster_child_pidp,
				foster_child_cnt * sizeof(pid_t),
				TRUE);
		vm_allocate_strict(mach_task_self(),
				(vm_address_t *) &foster_child_portp,
				foster_child_cnt * sizeof(mach_port_t),
				TRUE);
	} else {
		foster_child_pidp = foster_child_pid;
		foster_child_portp = foster_child_port;
	}
	for (w = pve->pvp_head_foster_childl, pidp = foster_child_pidp, 
				              portp = foster_child_portp;
			w != NULL;
			w = PVP(w)->pvp_foster_childl, pidp++, portp++) {
		*pidp = w->vp_pid;
		*portp = tnc_bestow_vproc_port(w, 0);
	}


	/*
	 * Create an array with all the pgrp member information in it
	 * (if the process is a pgrp leader).
	 */
	if (pve->pvp_flag & PV_PGRPLEADER) {
		if (pgrp_member_cnt > MAX_VPROC_LIST) {
			vm_allocate_strict(mach_task_self(),
					(vm_address_t *) &pgrp_member_pidp,
					pgrp_member_cnt * sizeof(pid_t),
					TRUE);
			vm_allocate_strict(mach_task_self(),
					(vm_address_t *) &pgrp_member_portp,
					pgrp_member_cnt * sizeof(mach_port_t),
					TRUE);
		} else {
			pgrp_member_pidp = pgrp_member_pid;
			pgrp_member_portp = pgrp_member_port;
		}
		for (w = pve->pvp_head_pgrpl, pidp = pgrp_member_pidp, 
					      portp = pgrp_member_portp;
				w != NULL;
				w = PVP(w)->pvp_pgrpl, pidp++, portp++) {
			*pidp = w->vp_pid;
			*portp = tnc_bestow_vproc_port(w, 0);
		}
	}

	/*
	 * Create an array with all the session member information in it
	 * (if the process is a session leader).
	 */
	if (pve->pvp_flag & PV_SESSIONLEADER) {
		if (sess_member_cnt > MAX_VPROC_LIST) {
			vm_allocate_strict(mach_task_self(),
					  (vm_address_t *) &sess_member_pidp,
					  sess_member_cnt * sizeof(pid_t),
					  TRUE);
			vm_allocate_strict(mach_task_self(),
					  (vm_address_t *) &sess_member_portp,
					  sess_member_cnt * sizeof(mach_port_t),
					  TRUE);
		} else {
			sess_member_pidp = sess_member_pid;
			sess_member_portp = sess_member_port;
		}
		for (w = pve->pvp_sessionl, pidp = sess_member_pidp, 
					    portp = sess_member_portp;
				w != NULL;
				w = PVP(w)->pvp_sessionl, pidp++, portp++) {
			*pidp = w->vp_pid;
			*portp = tnc_bestow_vproc_port(w, 0);
		}
	}


	/*
	 * Before going remote to terminate the old local task
	 * (from the new emulator), cancel its deadname notification.
	 */
	ret = mach_port_request_notification(mach_task_self(),
					     old_task,
					     MACH_NOTIFY_DEAD_NAME,
					     0,
					     MACH_PORT_NULL,
					     MACH_MSG_TYPE_MOVE_SEND_ONCE,
					     &task_deadname_notify_port);
	ASSERT(ret == KERN_SUCCESS);

	/*
	 * Perform the rexecve to the remote node. If the long form of
	 * RPC is used, then make sure to deallocate the necessary
	 * memory.
	 */
	if (!longrpc) {
		ux_server_thread_blocking();
		ret = cli_rexecve(server_port, 
			&error,
			fname, fname_count,
			arg_addr, arg_size, 
			arg_count, env_count, char_count,
			ve->vp_pid,
			(pve->pvp_flag & PV_PGRPLEADER) != 0,
			tnc_bestow_vproc_port(ve, BESTOW_RECEIVE_RT), 
			vproc_port_name,
			cur_proc_port,
			cur_cred_port, cred_port_name,
			cred_cache, cred_cache_size,
			tnc_bestow_vproc_port(VPROCPTR(pve->pvp_ppid), 0),
			(pve->pvp_foster_ppid && 
					pve->pvp_foster_ppid != pve->pvp_ppid)
			    ? tnc_bestow_vproc_port(
					VPROCPTR(pve->pvp_foster_ppid),0)
			    : MACH_PORT_NULL,
			pve->pvp_pgid
			    ? tnc_bestow_vproc_port(VPROCPTR(pve->pvp_pgid),0)
			    : MACH_PORT_NULL,
			(pve->pvp_flag & PV_PGRPLEADER) && pve->pvp_sid
			    ? tnc_bestow_vproc_port(VPROCPTR(pve->pvp_sid),0)
			    : MACH_PORT_NULL,
			child_pid, child_cnt,
			child_port, child_cnt,
			child_stat, child_cnt,
			foster_child_pid, foster_child_cnt,
			foster_child_port, foster_child_cnt,
			pgrp_member_pid, pgrp_member_cnt,
			pgrp_member_port, pgrp_member_cnt,
			sess_member_pid, sess_member_cnt,
			sess_member_port, sess_member_cnt,
			pve->pvp_jobc,
			pve->pvp_cttynode,
			rdir_port, cdir_port,
			old_task, old_task_name,
			cur_state, cur_state_count,
			&re_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			cur_logname,
			&ret_vproc_port, &ret_cred_port, &ret_proc_port
#ifdef NX
                        ,applinfo,
                         nodelist, nodelistcnt
#endif /* NX */
			);
		ux_server_thread_unblocking();
		if (ret != KERN_SUCCESS)
			panic("rexec: cli_rexecve RPCfail ret=0x%x", ret);
	} else {
		ux_server_thread_blocking();
		ret = cli_rexecve_long(server_port, 
			&error,
			fname, fname_count,
			arg_addr, arg_size, 
			arg_count, env_count, char_count,
			ve->vp_pid,
			(pve->pvp_flag & PV_PGRPLEADER) != 0,
			tnc_bestow_vproc_port(ve, BESTOW_RECEIVE_RT), 
			vproc_port_name,
			cur_proc_port,
			cur_cred_port, cred_port_name,
			cred_cachep, cred_cache_size,
			tnc_bestow_vproc_port(VPROCPTR(pve->pvp_ppid), 0),
			(pve->pvp_foster_ppid && 
					pve->pvp_foster_ppid != pve->pvp_ppid)
			    ? tnc_bestow_vproc_port(
					VPROCPTR(pve->pvp_foster_ppid),0)
			    : MACH_PORT_NULL,
			pve->pvp_pgid
			    ? tnc_bestow_vproc_port(VPROCPTR(pve->pvp_pgid),0)
			    : MACH_PORT_NULL,
			(pve->pvp_flag & PV_PGRPLEADER) && pve->pvp_sid
			    ? tnc_bestow_vproc_port(VPROCPTR(pve->pvp_sid),0)
			    : MACH_PORT_NULL,
			child_pidp, child_cnt,
			child_portp, child_cnt,
			child_statp, child_cnt,
			foster_child_pidp, foster_child_cnt,
			foster_child_portp, foster_child_cnt,
			pgrp_member_pidp, pgrp_member_cnt,
			pgrp_member_portp, pgrp_member_cnt,
			sess_member_pidp, sess_member_cnt,
			sess_member_portp, sess_member_cnt,
			pve->pvp_jobc,
			pve->pvp_cttynode,
			rdir_port, cdir_port,
			old_task, old_task_name,
			cur_state, cur_state_count,
			&re_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			cur_logname,
			&ret_vproc_port, &ret_cred_port, &ret_proc_port
#ifdef NX
                        ,applinfo,
                         nodelist, nodelistcnt
#endif /* NX */
			);
		ux_server_thread_unblocking();
		if (ret != KERN_SUCCESS)
			panic("rexec: cli_rexecve_long RPCfail ret=0x%x", ret);
	}
	if (error != ESUCCESS) {
		int	dummy;

		/* 
		 * If the migration call failed for some reason, put all
		 * the ports back to the correct names and make sure they
		 * are in the relevent port set.
		 */
		ASSERT(ret_vproc_port == vproc_to_port_lookup(ve));
		tnc_install_vproc_port(ve, ret_vproc_port, INSTALL_RECEIVE_RT);
		pve->pvp_movement_lock.ml_vproc_sequence = -1;
		pve->pvp_movement_lock.ml_proc_sequence = 0;
		{
			mach_port_status_t	proc_rstat;
			mach_port_status_t	vproc_rstat;
			kern_return_t		ret;
	
			ret = mach_port_get_receive_status(mach_task_self(),
							   ret_proc_port,
							   &proc_rstat);
			if (ret != KERN_SUCCESS)
				panic("mach_port_get_receive_status for "
				      "proc port returned 0x%x",ret);
			ret = mach_port_get_receive_status(mach_task_self(),
							   (mach_port_t) ve,
							   &vproc_rstat);
			if (ret != KERN_SUCCESS)
				panic("mach_port_get_receive_status for "
				       "vproc port returned 0x%x",ret);
	
			ASSERT((proc_rstat.mps_seqno+vproc_rstat.mps_seqno)==0);
		}
		ux_server_add_port(vproc_to_port_lookup(ve));
		reinstall_process_ports(ret_proc_port,
					ret_cred_port,
					cred_cache, cred_cache_size);
		/*
		 * Re-install the task port deadname notification.
		 */
		ret = mach_port_request_notification(mach_task_self(),
						     old_task,
						     MACH_NOTIFY_DEAD_NAME,
						     1,
						     task_deadname_notify_port,
						     MACH_MSG_TYPE_MOVE_SEND_ONCE,
						     &dummy_port);
		ASSERT(ret == KERN_SUCCESS);

		VPROC_UNLOCK_FOSTER_LIST_EXCL(ve, "rexecve");
		VPROC_UNLOCK_SESSION_LIST_EXCL(ve, "rexecve");
		VPROC_UNLOCK_PGRP_LIST_EXCL(ve, "rexecve");
		VPROC_UNLOCK_EXCL(ve, "rexecve");
		VPROC_END_MOVEMENT(ve, "rexecve");
		(void) PVPSOP_REMOTE_VPROC_FREE(new_node, ve);

		/* Let the calling task continue */
		(void) rtask_pproc_unquiesce(pve->pvp_pproc);

		goto out;
	}

	/* Throw away the deadname notification port right */
	(void) mach_port_deallocate(mach_task_self(),task_deadname_notify_port);

	/* No longer local */
	VPROC_LOCK_FLAG(ve, "rexecve");
	pve->pvp_flag &= ~PV_IS_LOCAL;
	VPROC_UNLOCK_FLAG(ve, "rexecve");

	/*
	 * Adjust reference counts for parent, pgrp and session leader of
	 * the migrated process.
	 */
	VPROC_RELEASE(VPROCPTR(pve->pvp_ppid),
		      "rexecve (parent on child node)");
	if (pve->pvp_foster_ppid != 0 && pve->pvp_foster_ppid != pve->pvp_ppid)
		VPROC_RELEASE(VPROCPTR(pve->pvp_foster_ppid),
			      "rexecve (foster parent on foster child node)");
	if (pve->pvp_pgid != 0)
		VPROC_RELEASE(VPROCPTR(pve->pvp_pgid),
			      "rexecve (pgrp leader on member node)");

	/*
	 * The parent-child-sibling list is now on the remote node.  Clean
	 * up the list on this node.
	 */
	while (pve->pvp_head_childl != NULL) {
		register struct vproc *vc = pve->pvp_head_childl;
		pve->pvp_head_childl = PVP(vc)->pvp_childl;
		PVP(vc)->pvp_childl = NULL;
		VPROC_RELEASE(vc, "rexecve (child on parent node)");
	}

	/*
	 * The foster child list is now on the remote node.  Clean
	 * up the list on this node.
	 */
	while (pve->pvp_head_foster_childl != NULL) {
		register struct vproc *vc = pve->pvp_head_foster_childl;
		pve->pvp_head_foster_childl = PVP(vc)->pvp_foster_childl;
		PVP(vc)->pvp_foster_childl = NULL;
		VPROC_RELEASE(vc, "rexecve (foster child on parent node)");
	}

	/*
	 * The pgrp list is now on the remote node.  Clean
	 * up the list on this node.
	 */
	if (pve->pvp_flag & PV_PGRPLEADER) {
		VPROC_LOCK_FLAG(ve, "rexecve");
		pve->pvp_flag &= ~PV_PGRPLEADER;
		VPROC_UNLOCK_FLAG(ve, "rexecve");
		VPROC_RELEASE(VPROCPTR(pve->pvp_sid),
			      "rexecve (session leader on member node)");
		while (pve->pvp_head_pgrpl != NULL) {
			register struct vproc *vg = pve->pvp_head_pgrpl;
			pve->pvp_head_pgrpl = PVP(vg)->pvp_pgrpl;
			PVP(vg)->pvp_pgrpl = NULL;
			VPROC_RELEASE(vg, "rexecve (pgrp member on ldr node)");
			VPROC_RELEASE(ve, "rexecve (pgrp leader on ldr node)");
		}
	}

	/*
	 * The session list is now on the remote node.  Clean
	 * up the list on this node.
	 */
	if (pve->pvp_flag & PV_SESSIONLEADER) {
		VPROC_LOCK_FLAG(ve, "rexecve");
		pve->pvp_flag &= ~PV_SESSIONLEADER;
		VPROC_UNLOCK_FLAG(ve, "rexecve");
		while (pve->pvp_sessionl != NULL) {
			register struct vproc *vs = pve->pvp_sessionl;
			pve->pvp_sessionl = PVP(vs)->pvp_sessionl;
			PVP(vs)->pvp_sessionl = NULL;
			VPROC_RELEASE(vs, "rexecve (sess member on ldr node)");
			VPROC_RELEASE(ve, "rexecve (sess leader on ldr node)");
		}
	}

	/*
	 * Now switch the pvproc ops to the remote version, since we've
	 * successfully rexec'd.
	 */
	pve->pvp_ops = &rpvproc_ops_table;

	/*
	 * Clean up the local process
	 * (the process is already running on the remote node).
	 */
	rtask_pproc_remove(pve->pvp_pproc);

out2:
	/*
	 * Undo all the locks at the beginning of the routine.
	 */
	VPROC_UNLOCK_FOSTER_LIST_EXCL(ve, "rexecve");
	VPROC_UNLOCK_SESSION_LIST_EXCL(ve, "rexecve");
	VPROC_UNLOCK_PGRP_LIST_EXCL(ve, "rexecve");
	VPROC_UNLOCK_EXCL(ve, "rexecve");
	VPROC_END_MOVEMENT(ve, "rexecve");


out:
	if (cred_cache_size > CREDENTIALS_CACHE_SIZE && *cred_cachep != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) cred_cachep,
				cred_cache_size * sizeof(node_t));
	}
	if (child_cnt > MAX_VPROC_LIST && *child_pidp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) child_pidp,
				child_cnt * sizeof(pid_t));
	}
	if (child_cnt > MAX_VPROC_LIST && *child_portp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) child_portp,
				child_cnt * sizeof(mach_port_t));
	}
	if (child_cnt > MAX_VPROC_LIST && *child_statp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) child_statp,
				child_cnt * sizeof(int));
	}
	if (foster_child_cnt > MAX_VPROC_LIST && *foster_child_pidp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) foster_child_pidp,
				foster_child_cnt * sizeof(pid_t));
	}
	if (foster_child_cnt > MAX_VPROC_LIST && *foster_child_portp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) foster_child_portp,
				foster_child_cnt * sizeof(mach_port_t));
	}
	if (pgrp_member_cnt > MAX_VPROC_LIST && *pgrp_member_pidp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) pgrp_member_pidp,
				pgrp_member_cnt * sizeof(pid_t));
	}
	if (pgrp_member_cnt > MAX_VPROC_LIST && *pgrp_member_portp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) pgrp_member_portp,
				pgrp_member_cnt * sizeof(mach_port_t));
	}
	if (sess_member_cnt > MAX_VPROC_LIST && *sess_member_pidp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) sess_member_pidp,
				sess_member_cnt * sizeof(pid_t));
	}
	if (sess_member_cnt > MAX_VPROC_LIST && *sess_member_portp != NULL) {
		vm_deallocate_strict(mach_task_self(),
				(vm_address_t) sess_member_portp,
				sess_member_cnt * sizeof(mach_port_t));
	}
#ifdef NX
	if (nodelist != (LP_MAP_T)NULL) {
		vm_deallocate_strict(mach_task_self(), (vm_address_t)nodelist,
				nodelistcnt*sizeof(LP_MAP_ENTRY_T));
	}
#endif /* NX */

	/*
	 * Process is no longer resident here, so decrement its use count, if
	 * there was no error.  This is done late in this routine on purpose.
	 */
	if (error == ESUCCESS) {
		VPROC_RELEASE(ve, "rexecve (active)");
	}

	return(error);
}

/*
 * As an expediency to avoid wholesale re-organization of code:
 * override the PVPSOP dispatch table to force remote operation
 * even to the local node. This is necessary to keep
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

int
rforkmulti(
	struct vproc		*vp,
	boolean_t		forkfamily,
	int			np_array[],
	int			proc_count,
	int			rval_array[],		/* OUT */
	int			*rval_count,
	pid_t			ch_pid_array[],		/* OUT */
	int			*ch_pid_count,
	mach_port_t		file_port_name[],
	unsigned int		file_port_name_count,
	mach_port_t		file_port_right[],
	unsigned int		file_port_right_count,
	thread_state_t		ch_state,
	unsigned int		ch_state_count,
	mach_port_t		vproc_port_name,
	mach_port_t		cred_port_name)
{
	register struct vproc	*vc;
	register struct pvproc	*pvp;
	int			i;
	unsigned int		cnt;
	mach_port_t		server_port;
	mach_port_t		ch_vproc_port;
	struct rf_data		rf_pp_data;
	task_t			pp_task;
	char			pp_comm[MAXCOMLEN + 1];
	char			pp_logname[MAXLOGNAME];
	int			error;
	mach_port_t		mmap_pagers[TNC_MAX_MMAP_REGIONS];
	struct mmap_struct	mmap_structs[TNC_MAX_MMAP_REGIONS];
	int			mmap_count;
	mach_port_t		rdir_port;
	mach_port_t		cdir_port;
	mach_port_t		ch_vproc_port_array_st[MAX_MULTI_LIST_SIZE];
	mach_port_t		*ch_vproc_port_array = ch_vproc_port_array_st;
	kern_return_t		ret;
	int			parent_seqno;
#ifdef NX
        int                     nodelistcnt = 0;
        LP_MAP_T                nodelist = (LP_MAP_T)NULL;
        APPLINFO_T              applinfo;
#endif /* NX */
#ifdef PARACORE
	mach_port_t		exec_rdir_port;
	mach_port_t		exec_cdir_port;
	char			exec_prg_name[PATH_MAX];
#endif /* PARACORE */


	pvp = PVP(vp);

#ifdef NX
        /*
         * Check if this is a NX application and a partition has
         * been allocated to it.  If so, convert the node number from
         * a logical node number to a physical node number and pass it
         * on.
         */
        rf_pp_data.nx_flags = 0;
        if (nx_in_partition(vp)) {
            rf_pp_data.nx_flags = pvp->pvp_flag &
                                 (PV_NX_PARTITIONED | 
				  PV_NX_APPLICATION |
				  PV_NX_NO_SIGCHLD |
				  PV_NX_ATH_PGRP);
	    if (forkfamily) {
		/*
		 * In the case of forkfamily we don't want node number
		 * translation, so we just need to get NX info.
		 */
		int	err;
		if (nx_get_info(pvp,
				&applinfo,
                                &nodelist, &nodelistcnt,
				TRUE, 0,
				&err) != ESUCCESS)
					/* allocates new pages of OOL VM */
	        panic("rforkmulti: nx_get_info failed, err=%d", err);
	    } else if (nx_map_nodelist(pvp, np_array, 
				proc_count, 
				&applinfo,
                                &nodelist, &nodelistcnt) == -1) {
					/* allocates new pages of OOL VM */
                return (EINVAL);
            }
        }
#endif /* NX */

	/*
	 * Seize the vproc during the fork operation.  We must prevent
	 * anyone from changing the parent's process group id during
	 * the fork; otherwise the new child may not wind up in the same
	 * process group as its parent.  We don't usually like to hold
	 * this lock while an RPC is occurring, but signal delivery is
	 * not blocked by this lock; only exit's, setpgid's and setsid's
	 * are prevented.
	 */
	VPROC_LOCK_EXCL(vp, "rforkmulti: parent");

	/*
	 * When creating new child processes, the management of that new
	 * child's joining its parent's process group requires special
	 * handling to prevent the child from missing signals directed
	 * at the entire process group.  Routines dvp_child_join_pgrp_setup()
	 * and dvp_child_join_pgrp_end() perform this special handling.
	 */
	error = dvp_child_join_pgrp_setup(vp, &parent_seqno);
	if (error) {
		goto out;		/* ERESTART a distinct possiblilty */
	}

	/*
	 * Don't bother to check the array sizes for validity
	 * - this is done higher up.
	 */

	/*
	 * Be pessimistic, assume the worst and prepare to return
	 * bad news all round. This is more likely in the forkfamily
	 * case when either we can't fork locally because the required
	 * pid is already allocated.
	 */
	for (i = 0; i < proc_count; i++) {
		if (is_tnc_node_valid(forkfamily ? VPROCNODE(np_array[i])
						 : np_array[i]))
			rval_array[i] = ECHILD;
		else {
			rval_array[i] = EINVAL;
			error = EINVAL;
		}
		ch_pid_array[i] = -1;
	}
	if (error) {
		*rval_count = proc_count;
		*ch_pid_count = proc_count;
		error = ESUCCESS;
		goto out;
	}

	/*
	 * Load parent task's info into the structure (which contains
	 * all ints) and put the character data (2 items) into places
	 * of their own -- all for passing via MiG.
	 */
	rfork_pproc_load_msg(pvp->pvp_pproc, &rf_pp_data, &pp_task,
			      mmap_structs, mmap_pagers, &mmap_count,
			      &rdir_port, &cdir_port,
			      pp_comm, pp_logname
#ifdef PARACORE
			      , &exec_rdir_port, &exec_cdir_port,
			      exec_prg_name 
#endif /* PARACORE */
			      );
	
	rf_pp_data.rf_p_pid = vp->vp_pid;	/* parent's pid */
	rf_pp_data.rf_p_pgid = pvp->pvp_pgid;	/* parent's gid */
	rf_pp_data.rf_p_sid = pvp->pvp_sid;	/* parent's sess id */
	rf_pp_data.rf_p_pgrp_mem_seqno = parent_seqno;	/* parent's seqno */

	cnt = proc_count;

	/*
	 * Call either the short version of rforkmulti or the
	 * long version of rforkmulti, depending upon the number
	 * of nodes. If the long version is called, perform
	 * appropriate memory allocation and deallocation.
	 */
	if (cnt <= MAX_MULTI_LIST_SIZE &&
	    file_port_name_count <= MAX_MULTI_LIST_SIZE) {
#ifdef NX
#ifdef PARACORE
		ret = PVPSOP_RFORKMULTI(
			forkfamily ? VPROCNODE(np_array[0]) : np_array[0], 
			forkfamily,
			np_array, cnt,
			0,
			0,
			proc_count,
			rval_array, &cnt,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
		    	    ? tnc_bestow_vproc_port(
					VPROCPTR(pvp->pvp_pgid), 0)
		    	    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			ch_vproc_port_array, &cnt, 
			ch_pid_array, &cnt,
			file_port_name, file_port_name_count,
			file_port_right, file_port_right_count,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname,
                        applinfo,
                        nodelist, nodelistcnt,
			exec_rdir_port, exec_cdir_port,
			exec_prg_name, strlen(exec_prg_name) + 1,
			NULL
			);
#else /* PARACORE */
		ret = PVPSOP_RFORKMULTI(
			forkfamily ? VPROCNODE(np_array[0]) : np_array[0], 
			forkfamily,
			np_array, cnt,
			0,
			0,
			proc_count,
			rval_array, &cnt,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
		    	    ? tnc_bestow_vproc_port(
					VPROCPTR(pvp->pvp_pgid), 0)
		    	    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			ch_vproc_port_array, &cnt, 
			ch_pid_array, &cnt,
			file_port_name, file_port_name_count,
			file_port_right, file_port_right_count,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname,
                        applinfo,
                        nodelist, nodelistcnt,
			NULL
			);
#endif /* PARACORE */
#else /* NX */
#ifdef PARACORE
		ret = PVPSOP_RFORKMULTI(
			forkfamily ? VPROCNODE(np_array[0]) : np_array[0], 
			forkfamily,
			np_array, cnt,
			0,
			0,
			proc_count,
			rval_array, &cnt,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
		    	    ? tnc_bestow_vproc_port(
					VPROCPTR(pvp->pvp_pgid), 0)
		    	    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			ch_vproc_port_array, &cnt, 
			ch_pid_array, &cnt,
			file_port_name, file_port_name_count,
			file_port_right, file_port_right_count,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname,
			exec_rdir_port, exec_cdir_port,
			exec_prg_name, strlen(exec_prg_name) + 1,
			NULL
			);
#else /* PARACORE */
		ret = PVPSOP_RFORKMULTI(
			forkfamily ? VPROCNODE(np_array[0]) : np_array[0], 
			forkfamily,
			np_array, cnt,
			0,
			0,
			proc_count,
			rval_array, &cnt,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
		    	    ? tnc_bestow_vproc_port(
					VPROCPTR(pvp->pvp_pgid), 0)
		    	    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			ch_vproc_port_array, &cnt, 
			ch_pid_array, &cnt,
			file_port_name, file_port_name_count,
			file_port_right, file_port_right_count,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname,
			NULL
			);
#endif /* PARACORE */
#endif /* NX */
	} else {
		int		*new_rval_array = NULL;
		mach_port_t	*new_ch_vproc_port_array = NULL;
		pid_t		*new_ch_pid_array = NULL;
		unsigned int	new_rval_cnt = 0;
		unsigned int	new_ch_vproc_port_cnt = 0;
		unsigned int	new_ch_pid_cnt = 0;
#ifdef NX
#ifdef PARACORE
		ret = PVPSOP_RFORKMULTI_LONG(
			forkfamily ? VPROCNODE(np_array[0]) : np_array[0], 
			forkfamily,
			np_array, cnt,
			0,
			0,
			proc_count,
			&new_rval_array, &new_rval_cnt,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
		    	    ? tnc_bestow_vproc_port(
					VPROCPTR(pvp->pvp_pgid), 0)
		    	    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			&new_ch_vproc_port_array,&new_ch_vproc_port_cnt,
			&new_ch_pid_array, &new_ch_pid_cnt,
			file_port_name, file_port_name_count,
			file_port_right, file_port_right_count,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname,
                        applinfo,
                        nodelist, nodelistcnt,
			exec_rdir_port, exec_cdir_port,
			exec_prg_name, strlen(exec_prg_name) + 1,
			NULL
			);
#else /* PARACORE */
		ret = PVPSOP_RFORKMULTI_LONG(
			forkfamily ? VPROCNODE(np_array[0]) : np_array[0], 
			forkfamily,
			np_array, cnt,
			0,
			0,
			proc_count,
			&new_rval_array, &new_rval_cnt,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
		    	    ? tnc_bestow_vproc_port(
					VPROCPTR(pvp->pvp_pgid), 0)
		    	    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			&new_ch_vproc_port_array,&new_ch_vproc_port_cnt,
			&new_ch_pid_array, &new_ch_pid_cnt,
			file_port_name, file_port_name_count,
			file_port_right, file_port_right_count,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname,
                        applinfo,
                        nodelist, nodelistcnt,
			NULL
			);
#endif /* PARACORE */
#else /* NX */
#ifdef PARACORE
		ret = PVPSOP_RFORKMULTI_LONG(
			forkfamily ? VPROCNODE(np_array[0]) : np_array[0], 
			forkfamily,
			np_array, cnt,
			0,
			0,
			proc_count,
			&new_rval_array, &new_rval_cnt,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
		    	    ? tnc_bestow_vproc_port(
					VPROCPTR(pvp->pvp_pgid), 0)
		    	    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			&new_ch_vproc_port_array,&new_ch_vproc_port_cnt,
			&new_ch_pid_array, &new_ch_pid_cnt,
			file_port_name, file_port_name_count,
			file_port_right, file_port_right_count,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname,
			exec_rdir_port, exec_cdir_port,
			exec_prg_name, strlen(exec_prg_name) + 1,
			NULL
			);
#else /* PARACORE */
		ret = PVPSOP_RFORKMULTI_LONG(
			forkfamily ? VPROCNODE(np_array[0]) : np_array[0], 
			forkfamily,
			np_array, cnt,
			0,
			0,
			proc_count,
			&new_rval_array, &new_rval_cnt,
			tnc_bestow_vproc_port(vp, 0),
			pvp->pvp_pgid
		    	    ? tnc_bestow_vproc_port(
					VPROCPTR(pvp->pvp_pgid), 0)
		    	    : MACH_PORT_NULL,
			rdir_port, cdir_port,
			pp_task,
			&new_ch_vproc_port_array,&new_ch_vproc_port_cnt,
			&new_ch_pid_array, &new_ch_pid_cnt,
			file_port_name, file_port_name_count,
			file_port_right, file_port_right_count,
			ch_state, ch_state_count,
			vproc_port_name,
			cred_port_name,
			&rf_pp_data, 
			mmap_structs, mmap_count,
			mmap_pagers, mmap_count,
			pp_comm, pp_logname,
			NULL
			);
#endif /* PARACORE */
#endif /* NX */
		bcopy(new_rval_array, rval_array, cnt * sizeof(int));
		vm_deallocate_strict(mach_task_self(),
				     (vm_address_t) new_rval_array,
				     cnt * sizeof(int));
		bcopy(new_ch_pid_array, ch_pid_array, cnt * sizeof(pid_t));
		vm_deallocate_strict(mach_task_self(),
				     (vm_address_t) new_ch_pid_array,
				     cnt * sizeof(pid_t));
		ch_vproc_port_array = new_ch_vproc_port_array;
	}

	/*
	 * Because we used COPY_SEND in the cli_rforkmulti(_long) call
	 * there is an extra send right for rdir_port and cdir_port that
	 * must be deallocated.
	 */
	ret = mach_port_deallocate(mach_task_self(), rdir_port);
	ASSERT(ret == KERN_SUCCESS);

	ret = mach_port_deallocate(mach_task_self(), cdir_port);
	ASSERT(ret == KERN_SUCCESS);

#ifdef PARACORE
	/*
	 * The same is true for exec_rdir_port and exec_cdir_port.
	 */
	ret = mach_port_deallocate(mach_task_self(), exec_rdir_port);
	if (ret != KERN_SUCCESS) {
		printf("rforkmulti: unable to deallocate exec_rdir_port(0x%x), 0x%x\n",
				exec_rdir_port, ret);
	}

	ret = mach_port_deallocate(mach_task_self(), exec_cdir_port);
	if (ret != KERN_SUCCESS) {
		printf("rforkmulti: unable to deallocate exec_cdir_port(0x%x), 0x%x\n",
				exec_cdir_port, ret);
	}
#endif /* PARACORE */

	/*
	 * Same applies to all emulator file ports.
	 */
	for (i=0; i<file_port_right_count; i++) {
		ret = mach_port_deallocate(mach_task_self(),
					   file_port_right[i]);
		if (ret != KERN_SUCCESS) {
			bootnode_printf("rforkmulti: "
			       "unable to deallocate file port(0x%x), 0x%x",
					file_port_right[i], ret);
		}
	}

	/*
	 * Get a vproc for the newly created child, and put it on its
	 * parent's vproc list.
	 *
	 * Normally we should now do a VPROC_HOLD(s) to keep a 
	 * reference.  Instead we just skip doing
	 * a VPROC_RELEASE(g) for the LOCATE_VPROC_PID() below.
	 * Note also for the benefit of the waitmulti() system call
	 * we scan the children in reverse order so that the child
	 * list is created in node order. 
	 */
	for (i = proc_count - 1; i >= 0; i--) {
		if (rval_array[i] != ESUCCESS)
			continue;
		vc = LOCATE_VPROC_PID(ch_pid_array[i]);
		if (vc == 0)
			panic("rforkmulti: cannot find child vproc");
		tnc_install_vproc_port(vc, ch_vproc_port_array[i], 0);
		PVP(vc)->pvp_childl = pvp->pvp_head_childl;
		pvp->pvp_head_childl = vc;
	}

	/*
	 * If we have an explicitly allocated vproc port array, we get
	 * rid of it now (we could not do so until the install_vproc_port()
	 * calls above were done).
	 */
	if (cnt > MAX_MULTI_LIST_SIZE) {
		vm_deallocate_strict(mach_task_self(),
				     (vm_address_t) ch_vproc_port_array,
				     cnt * sizeof(mach_port_t));
	}

	/*
	 * Set up the output parameter array sizes appropriately.
	 */
	*rval_count = proc_count;
	*ch_pid_count = proc_count;

	error = ESUCCESS;

out:
	VPROC_UNLOCK_EXCL(vp, "rforkmulti: parent");
	return(error);
}

#ifdef	WAITMULTI_TIMING
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
 * NAME:	waitmulti
 *
 * FUNCTION:	Wait for stopped or exited rforkmulti'ed child processes.
 *
 * RETURNS:	ECHILD, if no suitable child found.
 *		EAGAIN, if insufficient resources exit.
 *		ESUCCESS on completion.
 *		When called with a count less than the current number of
 *		children, the system retval is returned containing the
 *		actual number and neither pid nor status is returned.
 */
int
waitmulti(
	struct vproc	*vp,
	void		*args,
	int		*retval)
{
	struct args { 
		int	*count;
		pid_t	*pid_array;
		int	*status_array;
		int	options;
	} *uap = (struct args *) args;
        register struct pvproc	*pvp = PVP(vp);
        register struct vproc	*vc;
        register struct pvproc	*pvc;
        register struct vproc	*vo;
        register struct vproc	*vn;
        register	f;
	rusage_dev_t	ru_loc;
        int		error = ESUCCESS;
        int		wait_count;
        int		user_count = 0;
	int		i;
	int		*status_array = NULL;	/* temp copy of proc stats */
	pid_t		*pid_array = NULL;	/* temp copy of child pids */
	pid_t		*child_list = NULL;	/* full list of child pids */
	u_int		child_list_len;
	rstat_t		*rstat = NULL;
	u_int		rstat_len;
	rstat_t		*rstatp = NULL;
        int		ret;
	boolean_t	reaping = !(uap->options & VPROC_WNOWAIT);
	boolean_t	untraced = uap->options & WUNTRACED;

	error = copyin(uap->count, &user_count, sizeof(int));
	if (error)
		return(error);

	/*
	 * Allocate memory for pid and status arrays to be returned to the
	 * user.
	 */
	pid_array    = (pid_t *) kalloc(user_count*sizeof(pid_t));
	if (pid_array == NULL)
		return(EAGAIN);
	status_array = (int *)   kalloc(user_count*sizeof(int));
	if (status_array == NULL) {
		kfree(pid_array, user_count * sizeof(pid_t));
		return(EAGAIN);
	}

	/*
	 * What follows is an augmented wait loop...
	 * Note, however, that state info saved in vproc's on the parent's
	 * child list is not used is assess child state. To avoid races,
	 * only state returned from reap_multi() is used.
	 */
	TIMER_START();
loop:
	VPROC_LOCK_EXCL(vp, "waitmulti");
	(void) pproc_assert_sleep_wait(pvp->pvp_pproc);
	wait_count = 0;
	/*
	 * Check for incoming count <= num children.
	 */
	for (i = 0, vo = NULL, vc = pvp->pvp_head_childl;
				vc != NULL;
				vo = vc, vc = PVP(vc)->pvp_childl)
		i++;
	if (i == 0) {
		VPROC_UNLOCK_EXCL(vp, "waitmulti");
		error = ECHILD;
		goto out;
	} else if (i > user_count) {
		VPROC_UNLOCK_EXCL(vp, "waitmulti");
		*retval = i;
		error = ESUCCESS;
		goto out;
	}

	/*
	 * Allocate dynamic memory for the list of child pids and
	 * populate the pid list from the child list. This is necessary
	 * only if here for the first time.
	 * Note: vm_allocate() for alignment; the list is used as out of line
	 * data in subsequent RPCs.
	 */
	if (child_list == NULL) {
		ret = vm_allocate(mach_task_self(),
				  (vm_address_t *) &child_list,
				  i * sizeof(pid_t), TRUE);
		if (ret != KERN_SUCCESS) {
			VPROC_UNLOCK_EXCL(vp, "waitmulti");
			error = EAGAIN;
			goto out;
		}
		for (i = 0, vo = NULL, vc = pvp->pvp_head_childl;
				vc != NULL;
				vo = vc, vc = PVP(vc)->pvp_childl, i++) {
			child_list[i] = vc->vp_pid;
		}
		child_list_len = i;
	}

	/*
	 * Perform a REAP_MULTI() to collect state for all children.
	 * If called with the WNOWAIT option, we're not reaping and so we can
	 * safely arm elder reporting now. Otherwise, this is deferred until
	 * we know there is no status to return. Note that we can't arm
	 * elder reporting and reap children in one operation since we might
	 * reap elders! 
	 */
	TIMER_MARK(("REAP_MULTI() for %d children", child_list_len));
	error = PVPOP_REAP_MULTI(pvp->pvp_head_childl,
				 reaping ? NULL : pvp->pvp_head_childl,
				 WAIT_ANY, uap->options,
				 &rstat, &rstat_len,
				 &ru_loc,
				 0,
				 child_list, child_list_len,
				 0,
				 child_list_len,
				 NULL);
	if (error != ESUCCESS)
		panic("waitmulti(): REAP_MULTI returns %d", error);

	/* Accumulate all reaped zombie children's stats in parent */
	if (reaping)
		pproc_add_rusage(PVP(vp)->pvp_pproc, ru_loc);

	f = 0;
	rstatp = rstat;
	vo = NULL, vc = pvp->pvp_head_childl;
	while (vc != NULL) {

		pvc = PVP(vc);
		vn = pvc->pvp_childl;

		if (rstatp->pgid == 0) {
			/*
			 * If pgid = 0, the child's pgid did not match.
			 */
			rstatp++;
			vo = vc, vc = vn;
			continue;
		}
		f++;

		if (rstatp->state & PV_SZOMB || ((rstatp->state & PV_SSTOP)
						&& untraced)) {
			/* return child's ID and wstat */
			pid_array[wait_count] = vc->vp_pid; 
			status_array[wait_count] = rstatp->wstat;
			wait_count++;

			/*
			 * If the process is a zombie, then remove it
			 * from the parent-child-sibling list.
			 */
			if ((rstatp->state & PV_SZOMB) && reaping) {
				if (vo == NULL)
				    pvp->pvp_head_childl = vn;
				else
				    PVP(vo)->pvp_childl = vn;

				/*
				 * If the child was in the parent's pgrp
				 * then the remote reap will have left the
				 * pgrp list removal to us as an optimization.
				 */
				if (rstatp->pgid == vp->vp_pid)
					(void) PVPOP_RMV_PGRP_LIST(vp, vc, 0);

				/*
				 * Now release the child vproc on the node of
				 * the parent. Wait till after all references
				 * to "vc" are done.
				 */
				VPROC_RELEASE(vc, "wait(child)");

				vc = vn;
				rstatp++;
				continue;
			}
		}
		vo = vc, vc = vn;
		rstatp++;
	}
	VPROC_UNLOCK_EXCL(vp, "waitmulti");
	/*
	 * Free the memory returned by the reap_multi.
	 */
	vm_deallocate_strict(mach_task_self(),
			     (vm_address_t) rstat,
			     rstat_len*sizeof(rstat_t));
	if (wait_count != 0) {
		*retval = wait_count;
		error = ESUCCESS;
	} else if (f == 0) {
		error = ECHILD;
	} else if (uap->options & VPROC_WNOHANG) {
		*retval = wait_count;
		error = ESUCCESS;
	} else {
		/*
		 * Here if nothing to return.
		 * If we're reaping, we didn't arm the elder reporting tree
		 * earlier. So, arm this now. This is accompanied by a no_wait
		 * reap and a scan to make sure there's no process we just
		 * missed.
		 */
		if (reaping) {
			error = PVPOP_REAP_MULTI(pvp->pvp_head_childl,
						 pvp->pvp_head_childl,
						 WAIT_ANY, VPROC_WNOWAIT,
						 &rstat, &rstat_len,
						 &ru_loc,
						 0,
						 child_list, child_list_len,
						 0,
						 child_list_len,
						 NULL);
			if (error != ESUCCESS)
				panic("waitmulti: REAP_MULTI returned %d", error);
			for (i=0; i<child_list_len; i++)
				if (rstat[i].state & (PV_SZOMB|PV_SSTOP)) {
					vm_deallocate_strict(mach_task_self(),
						(vm_address_t) rstat,
						rstat_len*sizeof(rstat_t));
					TIMER_MARK(("REAP_MULTI(,arm_elder,) has status"));
					goto loop;
			}
		}
		if (!(error = pproc_sleep_waiting(pvp->pvp_pproc)))
			goto loop;
	}

out:
	/*
	 * Pack up and ship out.
	 */
	(void) copyout(&wait_count, uap->count, sizeof(int)); 
	if (wait_count > 0) {
		error = copyout(pid_array, uap->pid_array,
				wait_count*sizeof(pid_t));
		if (!error)
			error = copyout(status_array, uap->status_array,
					wait_count*sizeof(int));
	}
	kfree(pid_array, user_count * sizeof(pid_t));
	kfree(status_array, user_count * sizeof(int));
	if (child_list != NULL) {
		vm_deallocate_strict(mach_task_self(),
				     (vm_address_t) child_list,
				     child_list_len*sizeof(pid_t));
	}
	TIMER_MARK(("waitmulti() returning %d, count %d", *retval, wait_count));
	return (error);
}
