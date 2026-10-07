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
 * $Log: nx.c,v $
 * Revision 1.90  1995/04/04  21:37:40  suri
 *  Reviewer: jlitvin, yazz@locus.com
 *  Risk: M
 *  Benefit or PTS #: 12357
 *  Testing: Specific testcase, os_interfaces, controlc, rmcmd and rmcall EATs
 *  Module(s): nx_tam_wait()
 *  Description: During a brief window in nx_tam_wait() when it unlocks the
 *  vproc list while trying to PVPOP_REAP() and PVPOP_RMV_CHILD_FROM_PARENT(),
 *  the vproc can get reaped asynchronously by dpvpop_reap_multi(). In order to
 *  handle this situation, we now check for the return value from PVPOP_REAP()
 *  and if it is ESRCH, set the retval to 0 and return. The code will now be
 *  consistent with the case when the pgrp id is passed as a parameter.
 *
 * Revision 1.89  1995/03/29  21:58:00  suri
 *  Reviewer: sdh
 *  Risk: L
 *  Benefit or PTS #: 10374
 *  Testing: Specific testcase; controlc, os_interfaces, rmcall, rmcmd
 *           and sched EATs
 *  Module(s): nx_init_ports()
 *  Description: When a port name is checked into the local nameserver,
 *    it sets up a notify port "lnsvr_notify_port" to handle all the
 *    MACH_NOTIFY_DEAD_NAME instances. In an event of a deadname, the
 *    local nameserver deallocates the port and removes the port-to-name
 *    mapping from its list. However, the allocator port is currently
 *    handled in a different way leading to a server panic. The notify
 *    port for deadname notifications has been incorrectly reset to
 *    "nx_notify_port", which deallocates the port but and doesn't remove
 *    the port-name mapping in the local nameserver's list, causing all
 *    subsequent netname lookups to use incorrect and stale information.
 *
 *    The code has been modified so that the deadname notifications for
 *    the allocator port will now be serviced by lnsvr_notify_port, and
 *    therefore enabling proper cleanup action.
 *
 * Revision 1.88  1995/02/10  23:53:25  stans
 *  'lint' picking with typedefs for a clean compile.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing:WW05 sats
 *
 * Revision 1.87  1995/01/30  19:05:22  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: Lo
 *  Benefit or PTS #: 12267
 *  Testing: EATs os_interfeaces, controlc
 *  Module(s): server/nx/nx.c
 *
 * Revision 1.86  1995/01/27  18:42:12  raysx
 *  Changed the default case in nx_tam_ptrace() to cause a call to
 *  VPROC_RELEASE() before returning.
 *
 *  Reviewer: yazz@locus.com
 *  Risk: Low
 *  Benefit or PTS #: 12254
 *  Testing: By Inspection.
 *  Module(s): server/nx/nx.c
 *
 * Revision 1.85  1995/01/24  17:39:23  raysx
 *  Added calls to VPROC_RELEASE() in nx_attach_pgrp() and nx_tam_ptrace()
 *  when exiting.
 *
 *  Reviewer: suri@ssdintel.intel.com yazz@locus.com
 *  Risk: Low
 *  Benefit or PTS #: 12226
 *  Testing: By inspection.
 *  Module(s): server/nx.c
 *
 * Revision 1.84  1995/01/13  18:24:34  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: Lo
 *  Benefit or PTS #: 12143
 *  Testing: By inspection, plus EATs controlc, sched & os_interfaces
 *  Module(s): server/nx/nx.c
 *
 * Revision 1.83  1994/12/19  15:48:28  johannes
 * Wrapper for MIG call from server to allocator to inform about core dump.
 *
 *  Reviewer: Scott Hahn
 *  Risk: High (several components involved)
 *  Benefit or PTS #: 11577
 *  Testing: developer testing, special testing by Simon Tsang,
 *           corefile EAT, sched EAT, rmcmd EAT, controlc EAT
 *  Module(s): svr/server/paracore/dvp_pvpcore.c
 *             svr/server/nx/nx.c, nx.defs
 *             usr/sbin/allocator/alloc.defs, misc_rpcs.c, init_appl.c,
 *                                allocator.c, pspart.c
 *             usr/include/nx/schedule.h
 *             usr/include/allocsys.h
 *             usr/bin/pspart
 *
 * Revision 1.82  1994/11/18  20:37:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.81  1994/11/15  20:14:04  nandy
 * Changed nx_wait() to return 0 if reap fails and option is WNOHANG
 *
 *  Reviewer: suri
 *  Risk: L
 *  Benefit or PTS #: 11390
 *  Testing: IPD eats
 *  Module(s): nx/nx.c
 *
 * Revision 1.80  1994/08/31  22:46:44  mtm
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
 * Revision 1.77.2.2  1994/08/04  23:19:37  jlitvin
 * Fix bad merge by previous checkin.
 *
 * Revision 1.77.2.1  1994/08/03  15:52:54  nandy
 * Merged from the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.78  1994/08/03  15:41:47  nandy
 * Several changes to nx_tam_wait(), nx_signal_tam() to make tracing work
 * for processes that have not called nx_initve()
 *
 *  Reviewer: jlitvin
 *  Risk: M
 *  Benefit or PTS #: 10047
 *  Testing: PTS test case
 *  Module(s): bsd/kern-sig.c
 * 	bsd/kern_exit.c
 * 	tnc/dpvproc.h
 * 	tnc/dvp_vpops.c
 *
 * Revision 1.77  1994/07/26  18:29:00  jlitvin
 * After the allocator was restarted, the server would not always use the
 * correct allocator_port.  Instead of returning an internal error to the
 * user, the server will now use the correct allocator_port (like the
 * other system calls in nx.c).
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 10122 & 10371
 *  Testing: developer
 *  Module(s): server/nx/nx.c
 *
 * Revision 1.76  1994/07/06  18:10:54  nandy
 * nx_tam_wait() now calls PVPOP_RMV_PGRP_LIST() if the parent and the
 * process group are the same.
 *
 *  Reviewer: Chris Peak
 *  Risk: L
 *  Benefit or PTS #: 10040
 *  Testing: IPD EATS
 *  Module(s): nx.c
 *
 * Revision 1.75  1994/06/29  17:31:48  johannes
 * nx_app_part(): remove printf used for debugging
 *
 * Revision 1.74  1994/06/29  16:26:40  johannes
 * _nx_init(): calling init_pvp_core_data()
 * new function nx_app_part() for getting the complete allocator information
 * needed for allocinfo file
 *
 *  Initial check-in of parallel core dumping
 *  Reviewer: stefan, jlitvin
 *  Risk: Medium
 *  Benefit or PTS #: OS support for Postmortem Debugging
 *  Testing: developer tests
 *  Module(s):
 * 	svr/server/conf/MASTER
 * 	svr/server/conf/MASTER.i860
 * 	svr/server/conf/files.i860
 * 	svr/server/paracore/core_types.h
 * 	svr/server/paracore/allocinfo.c
 * 	svr/server/paracore/core.c
 * 	svr/server/paracore/dump.c
 * 	svr/server/paracore/dvp_pvpcore.c
 * 	svr/server/sys/allocinfo.h
 * 	svr/server/sys/core.h
 * 	svr/server/sys/user.h
 * 	svr/server/nx/nx.defs
 * 	svr/server/nx/nx.c
 * 	svr/server/bsd/kern_exit.c
 * 	svr/server/bsd/kern_fork.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/tnc/dpvproc.h
 * 	svr/server/tnc/dvp_init.c
 * 	svr/server/tnc/dvp_pvpops.c
 * 	svr/server/tnc/pvp.ops
 * 	svr/server/uxkern/fsvr_msg.c
 * 	cmds_libs/src/usr/sbin/allocator/alloc.defs
 * 	cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 * 	cmds_libs/src/usr/sbin/allocator/Makefile
 * 	cmds_libs/src/usr/include/README.locate
 * 	cmds_libs/src/usr/include/sys/Makefile
 *
 * Revision 1.73  1994/06/15  14:50:30  nandy
 * Added variable state to nx_tam_wait() to keep in sync with
 * dpvpop_reap.
 *
 * Revision 1.72  1994/06/02  22:21:21  chrisp
 * Parameter pgid of PVPOP_REAP() operation now INOUT, so new stack
 * variable introduced. Also NULL state parameter added to this operation.
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): nx.c
 *
 * Revision 1.71  1994/06/01  20:34:37  mag
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
 * Revision 1.70  1994/05/12  22:58:41  jlitvin
 * The function nx_mypart() could return a random error code if the
 * function nx_task_get_info() failed for some reason.  We weren`t saving
 * and returning the error from nx_task_get_info().
 *
 * Revision 1.69  1994/05/07  20:32:10  yazz
 * Merge R1.2 revision 1.56.2.12 into the main stem.
 *
 * Revision 1.56.2.12  1994/04/27  22:03:09  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: GUBT
 *  Testing: VSX, EATS
 *  Module(s): server/nx/nx.c
 *
 * Init MIG OUT params for deallocatable Out-Of-Line (OOL) memory to null,
 * lest randomly specified pages of VM be accidentally transmitted in the
 * reply and unintentionally deallocated out from under the server.
 *
 * Revision 1.56.2.11  1994/04/22  18:35:35  dbm
 * Added check for return of VPOP_GET_ATTR()
 *  Reviewer: Charlie Johnson
 *  Risk: Low
 *  Benefit or PTS #: 8729
 *  Testing: Specific test case, VSX Eats.
 *  Module(s):
 *         bsd/tty.c
 *         tnc/dvp_pvpops.c
 *         tnc/dvp_vpops.c
 *         nx/nx.c
 *
 * Revision 1.56.2.10  1994/03/30  01:08:52  cfj
 * In nx_tam_wait() for the case where it is waiting for a specific processes,
 * check the return result of VPROC_LOCATE_PID() more invoking the PVP()
 * macro.  Return ECHILD if the return result in NULL.
 *
 *  Reviewer: jlitvin
 *  Risk: Low
 *  Benefit or PTS #:8748
 *  Testing: test case.
 *  Module(s):server/nx/nx.c
 *
 * Revision 1.56.2.9  1994/03/09  16:15:54  nandy
 * Changed nx_proxy() to rely on the pvp_flag obtained form the
 * node where the process is running.
 *
 *  Reviewer: jlitvin
 *  Risk: Low
 *  Benefit or PTS #: 8178
 *  Testing: PTS test case
 *  Module(s): server/nx/nx.c
 *
 * Revision 1.63  1994/03/02  16:07:43  nandy
 * Mergerd from the R1_2 branch.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.56.2.8  1994/03/02  15:57:03  nandy
 * Make a default size service partition if any line in
 * the .partinfo file is empty.
 *
 *  Reviewer: cfj
 *  Risk: LOW
 *  Benefit or PTS #: 7932
 *  Testing: PTS test, Developer testing
 *  Module(s): server/nx/nx.c
 *
 * Revision 1.62  1994/02/16  00:14:13  cfj
 * Merged revisions 1.56.2.5 through 1.56.2.7 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.61  1994/02/01  15:32:32  nandy
 * Merged fron R1_2 branch.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.60  1994/01/14  18:21:50  cfj
 * Merged from R1_2.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.59  1994/01/12  23:25:23  cfj
 * Merged bug fix from R1_2.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.58  1994/01/12  13:56:26  nandy
 * Merged from R1.2 branch .
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.56.2.7  1994/02/16  00:07:24  cfj
 * In nx_signal_tam(), check to make sure the TAM has not been reaped
 * before calling pproc_wakeup().
 *
 *  Reviewer:nandy, jlitvin
 *  Risk: L
 *  Benefit or PTS #:Fix PTS-8054.
 *  Testing:os_interfaces EAT, prof EAT
 *  Module(s):server/nx/nx.c
 *
 * Revision 1.56.2.6  1994/02/08  19:58:36  nandy
 * Moved the call to REPORT_STATE() from dvpop_exit() to nx_tam_wait(). This
 * will allow tam to reap a process before parent does.
 *
 *  Reviewer: Chris Peak
 *  Risk: Medium
 *  Benefit or PTS #: 8084
 *  Testing: OS interface and IPD eats on 16 nodes
 *  Module(s): nx.c dvp_vpops.c
 *
 * Revision 1.56.2.5  1994/02/01  15:26:42  nandy
 * Changed nx_proxy() to return FALSE if the process is exiting.
 *
 *  Reviewer: cfj
 *  Risk: LOW
 *  Benefit or PTS #: 7855
 *  Testing: PTS test, VSX EATS, control-C EATS
 *  Module(s): nx.c
 *
 * Revision 1.56.2.4  1994/01/14  18:17:02  cfj
 * In nx_tam_waitk(), check if vc->vp_pid == -1 and skip the vproc if so because
 * it is being deallocated.
 *
 *  Reviewer:nandy
 *  Risk:M
 *  Benefit or PTS #:7776
 *  Testing:
 *  Module(s):
 *
 * Revision 1.56.2.3  1994/01/12  22:47:46  cfj
 * Partial fix for PTS #7776.  Fixed nx_tam_wait() so that it no longer
 * dereferences a NULL pointer.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.56.2.2  1994/01/12  13:54:08  nandy
 * Two changes. _nx_init() does not mark a process to be NX application if
 * init_appl() fails. nx_join() checks to see if the pgrp passed in is an
 * NX application
 *
 *  Reviewer: raya, carbajal
 *  Risk: LOW
 *  Benefit or PTS #: 7475
 *  Testing: PTS test case
 *  Module(s): nx.c
 *
 * Revision 1.56.2.1  1993/12/20  21:42:32  carbajal
 * vm_dealloc() OOL memory passed in for allocator system calls.
 * Tell MIG to dealloc this memory in nx_svr.defs
 *  Reviewer: cfj,nandy
 *  Risk: Low
 *  Benefit or PTS #: 7257
 *  Testing: Ran against bug report, MUNOPS, EATs
 *  Module(s): nx.c, nx_svr.defs
 *
 * Revision 1.56  1993/11/30  23:36:20  carbajal
 * Added nx_pspart_rpc, nx_alloc_exit calls
 * Use effective ids when necessary
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.55  1993/11/17  02:29:13  carbajal
 * R1.2 User model allocator system calls
 *  Reviewer: cfj
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model system calls
 *  Testing:
 *  Module(s):
 *
 * Revision 1.54  1993/11/16  15:26:08  cfj
 * nx_join() now return EQNODE if the process calling nx_join() is not on
 * a node of the partition.
 *
 *  Reviewer:nandy, jlitvin
 *  Risk:med
 *  Benefit or PTS #:Bug fix for #7151
 *  Testing:Verified that I can still use ipd.
 *  Module(s):server/nx/nx.c
 * 	   server/conf/syscall.master
 * 	   emulator/i860/emul_machdep.c
 *
 * Revision 1.53  1993/11/10  16:38:20  cfj
 * Fix an error in nx_check_allocator_response() which I forgot to remove
 * before.
 *
 *  Reviewer:None
 *  Risk:
 *  Benefit or PTS #: Will not loose errors returned by the allocator.
 *  Testing:
 *  Module(s):
 *
 * Revision 1.52  1993/11/09  19:14:22  cfj
 * Do deadname notification correctly for the allocator port.
 *
 *  Reviewer:nandy
 *  Risk:M
 *  Benefit or PTS #:Be able to kill and restart the allocator and still have
 * 		  parallel applications function.
 *  Testing:Manually killed and restarted the allocator and attempted parititon
 * 	 management commands and starting applications in between and after.
 *  Module(s):server/nx/nx.c & server/uxkern/ux_server_loop.c
 *
 * Revision 1.51  1993/11/03  15:56:52  nandy
 * iPVPOP_REAP() now returns EAGAIN if the process it is trying to reap is
 * already stopeed and is in the middle of exiting. Changed nx_tam_wait to take care
 * of this error condition.
 *
 * Revision 1.50  1993/11/02  23:48:46  cfj
 * Add a retry count on the number of attempt to send an RPC to the allocator.  If the
 * retry count decrements to zero, return EPALLOCERR.  This will allow the allocator to be
 * killed and user applications to receive an error instead of hanging.
 *
 * Risk:  Low
 * Reviewers:  carbajal, jlitvin
 * Benefit:  Kill the allocator will not hang user applications with no error
 *           returned.
 *
 * Revision 1.49  1993/11/02  15:51:54  cfj
 * Clean up the error checking for allocator RPCs and only retry the RPC if the RPC returns MACH_SEND_INVALID_DEST.
 *
 * Revision 1.48  1993/10/27  01:23:32  carbajal
 * Changed parameters to _nx_init to be more flexible
 * Added new system call nx_app_node_list()
 *
 * Revision 1.47  1993/10/21  23:24:39  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.46  1993/10/21  02:01:19  cfj
 * Fix for PTS #3377.  Have nx_map_nodelist(), nx_map_node(), nx_map_node_proc() and
 * nx_map_node_pid() check for negative node numbers.  Also fix the call
 * to VPOP_GET_ATTR() so that the correct number of parameters are passed.
 *
 * Revision 1.45  1993/10/11  13:06:07  nandy
 * Permission checking in nx_pri() enabled.
 *
 * Revision 1.44  1993/10/08  01:17:10  cfj
 * Remove allocator_blocking_port and add ANSI prototypes for some functions.
 *
 * Revision 1.43  1993/10/07  00:25:04  jlitvin
 * 3 minor things to make GCC happy (2 valid, 1 not): this_node is
 * actually a node_t, not int, you can't take an address of a register
 * variable and a character string pointer named string was causing it
 * fits.
 *
 * Revision 1.42  1993/10/06  22:11:31  cfj
 * Fix typo in nx_init_ports().
 *
 * Revision 1.41  1993/10/06  21:43:35  cfj
 * Add dead name notification for allocator port.
 *
 * Revision 1.40  1993/10/04  19:17:35  stans
 *    pps_reset_boot_node_list() now informs the MK of the new boot node list.
 *    Mach IPC sends/receives to a node which is down return an error instead
 *    of hanging the system.
 *
 * Revision 1.39  1993/09/09  16:04:57  cfj
 * Part of the fix for PTS bug #6449.  Remove the nx_sameuser() function which
 * is now not necessary.
 *
 * Revision 1.38  1993/08/28  00:47:37  carbajal
 * Added register_daemon() syscall PTS #6141
 *
 * Revision 1.37  1993/08/17  21:32:25  cfj
 * Missed a VPROC_RELEASE() in nx_pri(). (PTS bug #6189)
 *
 * Revision 1.36  1993/08/17  20:20:33  cfj
 * Add missing VPROC_RELEASE() calls that were necessary. (PTS bug #6189)
 * Also modfied nx_in_partition() and nx_map_node_pid() to use the PVPOP macros
 * so that logical to physical node number translation works properly when the
 * pid is not on the same node. (PTS bug #6191)
 *
 * Revision 1.35  1993/07/28  21:20:48  nandy
 * Changed the timeout default to 24hrs.
 *
 * Revision 1.34  1993/07/22  21:53:36  nandy
 * Added spanning tree code.
 *
 * Revision 1.33  1993/07/21  21:27:31  nandy
 * Workaround for timeout bug in the kernel. Change timeout to 4 hrs. if the
 * bootmagic is set to 0.
 *
 * Revision 1.32  1993/07/20  21:39:15  nandy
 * Added nx_print_fatal_message() to print timeout messages.
 *
 * Revision 1.31  1993/07/18  19:39:22  carbajal
 * Added nx_chpart_rpc system call. Cleanup up parameters to START_VPROCSERVER
 * for mkpart_rpc and rmpart_rpc
 *
 * Revision 1.30  1993/07/14  21:06:32  carbajal
 * Added support for allocator system calls
 *
 * Revision 1.29  1993/07/06  21:40:43  nandy
 * Two changes to nx_tam_wait. Set found_a_process only if a reapable process
 * is found.
 * Mark a process reaped only if the process is not marked a zombie.
 *
 * Revision 1.28  1993/06/10  23:32:23  nandy
 * In nx_signal_tam(), don't send signal if the parent and the tam are the same.
 *
 * Revision 1.27  1993/06/09  00:08:59  cfj
 * Change occurances of #include <i860ipsc/mcmsg/*.h> to #include <i860paragon/mcmsg/*.h>
 *
 * Revision 1.26  1993/06/01  23:29:49  nandy
 * Added bootmagic RPC_WAITTIME to specify RPC timmeout values for TNC calls.
 * nx_check_rpc_waiitime() validates the valuse passed thru the bootmagic.
 *
 * Revision 1.25  1993/05/11  20:55:30  nandy
 * nx_tam_wait() now ignores processes that are already reaped.
 *
 * Revision 1.24  1993/04/27  15:48:22  nandy
 * Use pvpop operation nx_get_p_flag() to go remtote and get the p_flag.
 *
 * Revision 1.23  1993/04/12  16:48:49  cfj
 * Added join_root_part() syscall.
 *
 * Revision 1.22  1993/04/05  15:09:19  cfj
 * Merge with T9.
 *
 * Revision 1.15.2.10  1993/04/05  15:03:53  cfj
 * Add magic_num parameter back into _nx_init().
 *
 * Revision 1.21  1993/04/03  22:34:43  cfj
 * Turn off DEBUG.
 *
 * Revision 1.20  1993/04/03  18:00:28  cfj
 * Merge with T9.
 *
 * Revision 1.15.2.6  1993/04/03  17:56:46  cfj
 * Removed some extraneous ux_server_thread_blocking/unblocking.
 *
 * Revision 1.15.2.5  1993/03/26  23:42:49  cfj
 * Put ux_server_thread_blocking/unblocking around off node RPCs.
 *
 * Revision 1.19  1993/04/03  03:06:53  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.18  1993/03/24  19:50:59  cfj
 * Merge with T9.
 *
 * Revision 1.15.2.4  1993/03/24  19:47:53  cfj
 * Change calls to cthread_yield() to thread_yield().
 *
 * Revision 1.17  1993/03/09  00:44:36  nandy
 * nx_proxy() goes remote to get the pvp_flags.
 *
 * Revision 1.15.2.3  1993/03/03  00:24:58  nandy
 * Temporarily commented out permission checking in nx_pri().
 *
 * Revision 1.15.2.2  1993/02/24  18:34:28  nandy
 * nx_pri() now uses LOCATE_VPROC_PID() to find the process group leader.
 *
 * Revision 1.15.2.1  1993/02/22  17:33:40  nandy
 * _nx_init() now takes an extra argument.
 *
 * Revision 1.1.2.4.2.4  1993/02/16  20:04:22  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.15  1993/02/08  23:23:32  nandy
 * nx_tam_wait() changed to mark the process "waited".
 *
 * Revision 1.14  1993/02/03  00:58:20  nandy
 * nx_tam_wait() changed to look thru the dynamic vproc table
 *
 * Revision 1.13  1993/02/01  20:05:45  nandy
 * Added nx_proxy_proc() to check if the process is a part of the nx 
 * application.
 *
 * Revision 1.12  1993/01/28  18:02:20  nandy
 * Added util nx_gang_check(). Returns TRUE if the application is gang stopped.
 *
 * Revision 1.11  1993/01/28  16:09:23  cfj
 * Require ROOT_FS_NODE to be in the service partition but no longer
 * require it to be logical node zero.
 *
 * Revision 1.10  1993/01/28  00:50:37  nandy
 * nx_report_allocator() now uses a proc structure.
 *
 * Revision 1.9  1993/01/26  17:28:53  cfj
 * Validate service partition in init_service_partition.
 *
 * Revision 1.8  1993/01/14  23:51:40  nandy
 * Added pproc_wakeup() to nx_signal_tam().
 *
 * Revision 1.1.2.4.2.3  1993/01/09  00:04:30  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.7  1993/01/05  20:41:27  shala
 * nx_report_allocator() now depends on the vproc for the pid.
 *
 * Revision 1.6  1993/01/05  19:52:32  shala
 * Added system call nx_pri() and also added support to make an exiting process
 * update the allocator.
 *
 * Revision 1.5  1992/12/31  17:19:31  nandy
 * nx_tam_wait() does not wait for any process not belonging to the pgroup.
 * nx_proxy() fails if the process is traced. This is a workaround to prevent
 * tam from death when the application exits.
 *
 * Revision 1.4  1992/12/18  17:23:51  nandy
 * nx_tam_wait() works more like waitpid(). The first argument could be a pid
 * or a process group.
 *
 * Revision 1.1.2.4.2.2  1992/12/16  06:00:45  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.4.2.1  1992/12/14  23:16:09  brad
 * Merged tip of old NX branch with PFS branch.
 * 
 * Revision 1.3  1992/12/14  17:42:26  cfj
 * If no service partition, create a 1 node service partition by default.
 *
 * Revision 1.2  1992/11/30  22:33:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.5  1992/11/24  21:39:23  cfj
 * Turn off debug.
 *
 * Revision 1.1.2.4  1992/11/13  18:51:58  cfj
 * Added reset_boot_node_list() system call.
 *
 * Revision 1.1.2.3  1992/11/11  16:00:00  joel
 * Fix PTS 3505.  If a NX application is put in the background, and then killed
 * with "kill <proxy_pid>" we really want the whole application to die, not just
 * the proxy_pid.  This change in kill3() does that.
 *
 * Revision 1.10  1992/11/02  21:49:18  cfj
 * Final integration and testing of IPD modifications
 *
 * Revision 1.9  1992/10/17  17:29:31  cfj
 * IPD & Allocator changes.
 *
 * Revision 1.8  1992/10/16  18:15:54  cfj
 * Modification so that NX programs can run.
 *
 * Revision 1.7  1992/10/14  17:17:39  cfj
 * NX integration.
 *
 * Revision 1.6  1992/09/04  16:49:54  nandy
 * Support for boot magic ALLOCATOR_NODE
 *
 * Revision 1.5  92/09/02  09:51:23  cfj
 * Fix a potential problem in nx_tam_wait() where the loop search through the
 * vproc table may exit too soon without finding a valid processes even though
 * there is one in the system.
 * 
 * Revision 1.4  92/08/26  15:35:18  nandy
 * migrate now calls end_vproc_port_op(). 
 * nx_traced_add_mem() takes IN/OUT parameter.
 * 
 * Revision 1.3  92/08/24  09:16:33  nandy
 * No need to test stopsig in nx_no_signal().
 * 
 * Revision 1.2  92/07/28  09:49:31  nandy
 * NX integration
 * 
 * Revision 1.20  92/07/02  10:28:46  cfj
 * Added nx_tam_wait(), nx_tam_ptrace() and bug fixes.
 * 
 * Revision 1.19  92/06/24  09:27:36  cfj
 * Added nx_map_node_proc() which can be called with a proc pointer.
 * 
 * Revision 1.18  92/06/22  15:32:15  cfj
 * nx_join() now migrates the requesting process into the partition.
 * 
 * Revision 1.17  92/06/18  14:11:30  cfj
 * Use lp_map_count - 1 as index when assigning the host node number.
 * 
 * Revision 1.16  92/06/11  16:02:00  cfj
 * Added nx_join,nx_attach_pgrp and nx_detach_pgrp.
 * 
 * Revision 1.15  92/06/10  07:57:44  cfj
 * Added support for setting up and initializing the service partition.
 * 
 * Revision 1.14  92/05/20  08:38:42  cfj
 * Turn off DEBUG and convert nx_init() to use vproc.
 * 
 * Revision 1.13  92/05/14  16:44:50  cfj
 * nx_init() now returns the size of the allocated parition.
 * 
 * Revision 1.12  92/05/11  13:32:18  cfj
 * Removed definition of DEBUG.
 * 
 * Revision 1.11  92/05/08  13:25:41  cfj
 * Changed _nx_init() from begin type MSG to type STUB.
 * 
 * Revision 1.10  92/05/04  17:02:54  cfj
 * Fix up include for mcmsg_info.h
 * 
 * Revision 1.9  92/05/04  10:01:33  cfj
 * Added pid to nx_task_put_info() and check if allocator port
 * has been deallocated and then look it up again.
 * 
 * Revision 1.8  92/04/29  09:11:16  cfj
 * Change _init to _nx_init
 * 
 * Revision 1.7  92/04/29  08:16:26  cfj
 * Change name of nx_init() to _init() and pass the pid to nx_task_put_info().
 * 
 * Revision 1.6  92/04/07  12:26:13  cfj
 * Turn off DEBUG.
 * 
 * Revision 1.5  92/04/07  12:20:08  cfj
 * Changed around nx_task_put_info() again.
 * 
 * Revision 1.4  92/04/03  08:57:56  cfj
 * Changed the parameters around in nx_task_put_info().
 * 
 * Revision 1.3  92/04/01  08:26:39  cfj
 * Forgot to deallocate the node list array returned by init_appl().
 * 
 * Revision 1.2  92/03/31  17:05:23  cfj
 * Fixed the copyright notice to say 1992.
 * 
 * Revision 1.1  92/03/31  17:03:14  cfj
 * Initial revision
 * 
 */
/**********************************************************************
 *
 *  Unix system calls which support NX message passing.
 *
 *  The calls are:
 *
 *    numnodes = _nx_init(nx_info, appl_info, magic_num);
 *
 *   NX_INIT_T      nx_info      IN: Allocator specific options
 *
 *   APPLINFO_T     appl_info    IN: Application specific message passing
 *                                   information.
 *          int     magic_num;   IN: Used for version control to ensure
 *                                   OS and binaries are compatible
 *
 *          int     numnodes    OUT: The actual number of nodes allocated.
 *
 *          _nx_init() calls the partition allocation server and then
 *          calls the kernel to store the returned node list with the
 *          per task information in the kernel.
 *
 *
 *    loc = nx_allocator_node();
 *
 *          int     loc;              The node number the local
 *                                    name server where the allocator
 *                                    port is checked in.
 *
 *    numnodes = nx_join( pgrp );
 *
 *               pid_t  pgrp;         The process group id of the NX
 *                                    application to join.
 *
 *               int    numnodes;     The number of nodes in the partition
 *                                    where the NX application is running.
 *
 *     Make the calling process part of the NX application indicated by
 *     the supplied process group.  
 *
 *
 *    err = nx_attach_pgrp( pgrp );
 *
 *               pid_t  pgrp;         The process group id of the NX
 *                                    application.
 *
 *               int    err;          0 = success, -1 = error.
 *
 *      Associate the indicated process group to the calling TAM.  
 *      This causes the TAM to be sent a SIGCHLD whenever a new
 *      process is created within the NX application.  Other standard
 *      debug events (breakpoints etc.) will also be fielded by
 *      the TAM.  This system call in intended to be used by 
 *      debuggers of parallel applications.  
 *
 *
 *    err = nx_detach_pgrp( pgrp );
 *
 *               pid_t  pgrp;         The process group id of the NX
 *                                    application.
 *
 *               int    err;          0 = success, -1 = error.
 *
 *       Disassociate the indicated process group from the calling TAM.
 *
 *    
 *    pid = nx_tam_wait( pgrp, (int *)status, options );
 *
 *               pid_t pid;          The pid or process group interested in.
 *               int   *status        Status of the process under debug.
 *               int   options        Either WNOHANG, WUNTRACED or WNOWAIT.
 *               pid_t pid;           -1 = error, otherwise pid of 
 *                                    a process under debug which caused
 *                                    a SIGCHLD.
 *
 *       Return the pid of a process in an NX application which is a
 *       member of the designated process group and who is stopped.  
 *       If there are no processes which qualify,
 *       then a pid of zero is returned.  This call should only be called
 *       when a TAM receives a SIGCHLD.
 *    
 *    ret = nx_tam_ptrace( req, pid, addr, data );
 *
 *               int    req;          ptrace request
 *               pid_t  pid;          pid to trace.
 *               int   *addr;         Address
 *               int    data;         Data 
 *
 *               int    ret           -1 on error, 0 otherwise.
 *
 *       The function of nx_tam_ptrace() mirrors that of ptrace() EXCEPT
 *       that it allows the tam to trace any process that is in the
 *       process group passed in when the tam called nx_attach_pgrp().
 *       This function can be called by TAM only and will return an
 *       EQTAM error if the process making the call had not previously
 *       called nx_attach_pgrp().
 *
 *      int
 *      nx_mypart(vp, rows, cols)
 *      struct vproc    *vp;
 *      long            *rows;
 *      long            *cols;
 *
 *      int     ret     -1 on error, o otherwise
 *
 *      This function returns the rectangular dimensions of the node set
 *      belonging to the calling NX application.
 *
 *
 *	int
 *	nx_app_info(vp,pgroup,appinfo)
 *	struct vproc    *vp;
 *	pid_t           pgroup;
 *	nx_app_info_t   *appinfo;
 *
 *	int 	ret	-1 on error, 0 otherwise
 *
 *	This function returns the NX application information maintained
 *	by the allocator.
 *
 *	int
 *	nx_app_node_list(vp,pgroup,node_list,node_list_size)
 *	struct vproc    *vp;
 *	pid_t           pgroup;
 *	LP_MAP_T        *node_list;
 *	int             *node_list_size;
 *
 *	int	ret	-1 on error, 0 otherwise
 *
 *	This function returns the node list used by the application specified
 *	by pgroup.
 *
 *	int	ret	-1 on error, 0 otherwise
 *
 *	int
 *	nx_free_node_list(vp,inode,bitmap,bitmap_size)
 *	struct vproc    *vp;
 *	ino_t           inode;
 *	MIG_BITMAP_T    *bitmap;
 *	int             *bitmap_size;
 *
 *	int	ret	-1 on error, 0 otherwise
 *
 *	This function returns the list of unallocated nodes in the 
 *	partition specified by inode.
 *
 *	int
 *	nx_pspart_rpc(vp,inode,buffer,buf_len)
 *	mach_port_t	vproc_port;
 *	boolean_t	*interrupt;
 *	ino_t		inode;
 *	char		*buffer;
 *	int		*buf_len;
 *
 *	int	ret	-1 on error, 0 otherwise
 *
 *	This functions returns the list of applications and active
 *	partitions in the specified partition.
 *
 *	int
 *	nx_alloc_exit_rpc(vp,pgroup)
 *	struct vproc	*vp;
 *	pid_t		pgroup;
 * 
 *	int	ret	-1 on error, 0 otherwise
 *
 *	This function tells the allocator to treat this application as
 *	is it has exited from the system. This is used by SPV.
 ***********************************************************************/

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/vproc.h>
#include <sys/proc.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <tnc/dpvproc.h>
#include <uxkern/import_mach.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/syscall_subr.h>
#include <norma_ipc.h>
#include <mach/norma_special_ports.h>
#include <i860paragon/mcmsg/mcmsg_info.h>
#include <nx/nx_user.h>
#include <nx/nx_struct.h>
#include <nx/nx_types.h>
#include <norma/node_status.h>

/*#define DEBUG 1 */
#ifdef DEBUG
int nx_debug = 0;
#define DEBUGOUT(a) if (nx_debug) printf a
#else
#define DEBUGOUT(a)
#endif /* DEBUG */

extern mach_port_t	privileged_host_port;
extern node_t this_node;
extern  mach_port_t     root_fs_port;
extern mach_msg_timeout_t rpc_waittime;

mach_port_t     allocator_port = MACH_PORT_NULL;
mach_port_t     nx_notify_port = MACH_PORT_NULL;
extern node_t allocator_node;

TAM_TABLE_T tam_table = (TAM_TABLE_T)NULL;
struct mutex tam_table_lock;

#define TAM_TABLE_LOCK   mutex_lock(&tam_table_lock)
#define TAM_TABLE_UNLOCK mutex_unlock(&tam_table_lock)

#define MAX_RPC_RETRIES 500

/* Forward References */
pid_t nx_lookup_tam(pid_t);
long minimum(long, long);
int nx_check_allocator_response( kern_return_t, int, int );


/*-------------------------- System Calls ----------------------------------*/

int
_nx_init(
	struct vproc	*vp,
	NX_INIT_T 	nx_info,
	int		*numnodes,
	APPLINFO_T	appl_info,
	int		selector_cnt,
	char		*selector_buf,
	int		selector_buflen)
{
    LP_MAP_T		lp_map;
    int			lp_map_count;
    kern_return_t	retcode;
    int			rc, ok;
    task_t		ptask;
    uid_t		puid;
    gid_t		pgid;
    pid_t		ppid;
    struct pvproc	*pvp;
    unsigned long	rows, cols;
    int			max_retries = MAX_RPC_RETRIES;


#ifndef NX
	return (EOPNOTSUPP);
#endif

	pvp = PVP(vp);

    DEBUGOUT(("_nx_init: inode=%d size=%d\n", nx_info.inode, nx_info.size));
    DEBUGOUT(("          appl_info.app = %d\n", appl_info.app));
#ifdef	CHKPNT
    DEBUGOUT(("          nx_info.ck_interval = %d\n", nx_info.ck_interval));
    DEBUGOUT(("          nx_info.no_restart = %d\n", nx_info.no_restart));
#endif	/* CHKPNT */
    DEBUGOUT(("                   .process_lock = %d\n", appl_info.process_lock));
    DEBUGOUT(("                   .pkt_size = %d\n", appl_info.pkt_size));
    DEBUGOUT(("                   .memory_buffer = %d\n", appl_info.memory_buffer));
    DEBUGOUT(("                   .memory_export = %d\n", appl_info.memory_export));
    DEBUGOUT(("                   .memory_each = %d\n", appl_info.memory_each));
    DEBUGOUT(("                   .send_threshold = %d\n", appl_info.send_threshold));
    DEBUGOUT(("                   .send_count = %d\n", appl_info.send_count));
    DEBUGOUT(("                   .give_threshold = %d\n", appl_info.give_threshold));

    *numnodes = 0;
    if (nx_init_ports() == -1) {
	return(EPALLOCERR);
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    ptask =  pvp->pvp_pproc->p_task;
    puid  =  pvp->pvp_pproc->p_ruid;
    pgid  =  pvp->pvp_pproc->p_rgid;
    ppid  =  pvp->pvp_pproc->p_pid;

#ifdef	CHKPNT
    DEBUGOUT(("init_appl(0x%08x, %d, %d, %d, %d, %d, %d, %d, 0x%08x, 0x%08x, 0x%08x)\n", allocator_port, puid, pgid, nx_info.inode, nx_info.size, ppid, nx_info.ck_interval, nx_info.no_restart, &lp_map, &lp_map_count, &rc));
#else	/* !CHKPNT */
    DEBUGOUT(("init_appl(0x%08x, %d, %d, %d, %d, %d, 0x%08x, 0x%08x, 0x%08x)\n", allocator_port, puid, pgid, nx_info.inode, nx_info.size, ppid, &lp_map, &lp_map_count, &rc));
#endif	/* CHKPNT */


    do {
	rows = 0;
	cols = 0;
	ux_server_thread_blocking();
		retcode = init_appl(allocator_port, 
				puid, u.u_cred->cr_uid,
				pgid, u.u_cred->cr_gid,
				(int)u.u_cred->cr_acctid,
				nx_info,
				(int *) &rows,
				(int *) &cols,
				ppid,
				selector_cnt,
				(MIG_BITMAP_T)selector_buf,
				(mach_msg_type_number_t)selector_buflen,
				(LP_MAP_T *) &lp_map,
				(mach_msg_type_number_t *) &lp_map_count,
				(int *) &rc);

        ux_server_thread_unblocking();

    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

    if (ok != ESUCCESS)
	return ok;


    DEBUGOUT(("rc = %d 0x%08x retcode=%d 0x%08x\n", rc, rc, retcode, retcode));


    if (retcode == MACH_MSG_SUCCESS && rc == ESUCCESS) {

	DEBUGOUT(("nx_task_put_info(0x%08x, 0x%08x, 0x%08x, 0x%08x, %d, %d)\n",	       privileged_host_port, lp_map, lp_map_count, &appl_info, ptask, ppid));


	/* Set the dimensions of the node set for use by mypart() */
	appl_info.rows = rows;
	appl_info.columns = cols;
	lp_map[lp_map_count - 1] = this_node;

	*numnodes = lp_map_count - 1; /* Do not count the host process */
	retcode = nx_task_put_info(privileged_host_port,
				   lp_map, 
				   lp_map_count,
				   appl_info,
				   ptask,
				   ppid);

	vm_deallocate(mach_task_self(), (vm_address_t)lp_map, 
		      (vm_size_t)(lp_map_count * sizeof(LP_MAP_ENTRY_T)));
    }


    DEBUGOUT(("retcode=%d 0x%08x rc=%d 0x%08x\n", retcode, retcode, rc, rc));

    if (retcode == MACH_MSG_SUCCESS) {
	/* 
	 * If process is already an NX application, then turn off
	 * the trace flag since it is now a NEW application and
	 * we only can debug ONE application per tam.
	 */
	if (pvp->pvp_flag & PV_NX_APPLICATION) {
	    pvp->pvp_pproc->p_flag &= ~STRC;
	}
	if ( rc == ESUCCESS ) {
		VPROC_LOCK_FLAG(vp, "_nx_init");
		pvp->pvp_flag |= PV_NX_APPLICATION | PV_NX_PARTITIONED;
		VPROC_UNLOCK_FLAG(vp, "_nx_init");
#ifdef PARACORE
		if ( pvp->pvp_flag & PV_PGRPLEADER ) {
			/*
			 * This is the proxy - initialize
			 * pvp_core_data structure.
			 */
			pvp->pvp_core_data = (pvp_core_data_t *) init_pvp_core_data(vp);
		}
#endif /* PARACORE */
	}
	return(rc);
    } else {
	return(retcode);
    }
}

int
nx_report_allocator(p)
        pid_t   p;
{
        int             rc, retcode;

        if (nx_init_ports() == -1) {
                return(EPALLOCERR);
        }

	ux_server_thread_blocking();
        retcode = remove_nx_appl(allocator_port, p, &rc);
	ux_server_thread_unblocking();
        if ( retcode != MACH_MSG_SUCCESS ) {
                return(retcode);
        }
        return(rc);
}

#ifdef PARACORE
int
nx_report_allocator_paracore(p)
        pid_t   p;
{
        int             rc, retcode;

        if (nx_init_ports() == -1) {
                return(EPALLOCERR);
        }

	ux_server_thread_blocking();
        retcode = nx_appl_dumping_core(allocator_port, p, &rc);
	ux_server_thread_unblocking();
        if ( retcode != MACH_MSG_SUCCESS ) {
                return(retcode);
        }
        return(rc);
}
#endif /* PARACORE */

	
int
NX__nx_init(
	mach_port_t	vproc_port,
	boolean_t	*interrupt,
	NX_INIT_T	nx_info,
        int            	*numnodes,
        APPLINFO_T     	appl_info,
	int            	selector_cnt,
	char          	*selector_buf,
	int		selector_buflen)
{
	START_VPROCSERVER(SYS__nx_init, 3)

	error = _nx_init( (struct vproc *)vproc_port,
				nx_info,
				numnodes,
				appl_info,
				selector_cnt,
				selector_buf,
				selector_buflen);

	END_VPROCSERVER(SYS__nx_init)
}

int
NX_nx_pri(vproc_port, interrupt, pid, pri)
	mach_port_t	vproc_port;
	boolean_t	*interrupt;
	pid_t		pid;
        int             pri;
{
	START_VPROCSERVER(SYS_nx_pri, 2)

	error = nx_pri(vproc_port, pid, pri );

	END_VPROCSERVER(SYS_nx_pri)
}


int
nx_pri(vp, pid, pri)
	struct vproc *vp;
	pid_t		pid;
	int		pri;
{
	int error, retcode, rc, ok;
	struct pvproc   *pvp;
	struct pvproc   *pvp_pgrp;
	struct vproc    *vp_pgrp;
	struct proc 	*p;
	uid_t		pgrp_uid;
	int		max_retries = MAX_RPC_RETRIES;

	pvp = PVP(vp);

	if((vp_pgrp = LOCATE_VPROC_PID(pid)) ==  (struct vproc *)0) {
			return(EPERM);
	}

	
	if((error = VPOP_GET_ATTR(vp_pgrp, 0, 0, 0, &pgrp_uid, 0, 0, FALSE)) 			!= ESUCCESS) {
		VPROC_RELEASE(vp_pgrp, "nx_pri");
		return(error);
	}

	if((pgrp_uid != u.u_cred->cr_uid ) && 
			(error = suser(u.u_cred, &u.u_acflag))) {
		VPROC_RELEASE(vp_pgrp, "nx_pri");
		return(error?error:EPERM);
	}
	
	
        if (nx_init_ports() == -1) {
		VPROC_RELEASE(vp_pgrp, "nx_pri");
                return(EPALLOCERR);
        }

	do {
	ux_server_thread_blocking();
        retcode = set_nx_pri(allocator_port, pid, pri, &rc);
	ux_server_thread_unblocking();
	} while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

	if (ok != ESUCCESS)
	    rc = ok;

	VPROC_RELEASE(vp_pgrp, "nx_pri");
        return(rc);
}

int
NX_nx_part_attributes(vproc_port,interrupt,inode,partinfo)
mach_port_t	vproc_port;
boolean_t	*interrupt;
ino_t		inode;
PARTREQ_T	*partinfo;
{
	START_VPROCSERVER(SYS_nx_part_attributes,2)
	error = nx_part_attributes(vproc_port, inode,partinfo);
	END_VPROCSERVER(SYS_nx_part_attributes)
}

int
nx_part_attributes(vp,inode,partinfo)
struct vproc 	*vp;
ino_t		inode;
PARTREQ_T	*partinfo;
{
    int         error,retcode, rc, ok;
    uid_t       puid;
    gid_t       pgid;
    struct pvproc *pvp;
    int         max_retries = MAX_RPC_RETRIES;

#ifndef NX
	return (EOPNOTSUPP);
#endif

	error = 0;
    if (nx_init_ports() == -1){
	error = EPALLOCERR;
	goto nx_part_attr_error;
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    pvp = PVP(vp);
    /* grab effective ids */
    puid  =  u.u_cred->cr_uid;
    pgid  =  u.u_cred->cr_gid;

    do {
	ux_server_thread_blocking();
	retcode = get_part_attributes(allocator_port,inode,puid,pgid,
				      partinfo,&rc);
	ux_server_thread_unblocking();

    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

    if (ok != ESUCCESS)
	error = ok;

nx_part_attr_error:
	return(error);
}

int
NX_nx_node_list(vproc_port,interrupt,inode,node_list,node_list_size)
mach_port_t	vproc_port;
boolean_t	*interrupt;
ino_t		inode;
LP_MAP_T	*node_list;
int		*node_list_size;
{
	START_VPROCSERVER(SYS_nx_node_list,3)      
	error = nx_node_list(vproc_port, inode,node_list,node_list_size);
	END_VPROCSERVER(SYS_nx_node_list)
}

int
nx_node_list(vp,inode,node_list,node_list_size)
struct vproc 	*vp;
ino_t		inode;
LP_MAP_T	*node_list;		/* OUT, dealloc (ool memory) */
int		*node_list_size;	/* size of the above */
{
    int         error,retcode, rc, ok;
    uid_t       puid;
    gid_t       pgid;
    struct pvproc *pvp;
    int         max_retries = MAX_RPC_RETRIES;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*node_list = 0;
	*node_list_size = 0;

#ifndef NX
	return (EOPNOTSUPP);
#endif

	error = 0;
    if (nx_init_ports() == -1){
	error = EPALLOCERR;
	goto nx_node_list_error;
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    pvp = PVP(vp);
    /* grab effective ids */
    puid  =  u.u_cred->cr_uid;
    pgid  =  u.u_cred->cr_gid;


    do {
	ux_server_thread_blocking();
	retcode = (int) get_part_node_list(allocator_port,inode,puid,pgid,
					   node_list,
					   (mach_msg_type_number_t *)node_list_size,
					   &rc);
	ux_server_thread_unblocking();
    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

    if (ok != ESUCCESS)
	error = ok;

nx_node_list_error:
	return(error);
}

int
NX_nx_part_bitmap(vproc_port,interrupt,inode,bitmap,bitmap_size)
mach_port_t	vproc_port;
boolean_t	*interrupt;
ino_t		inode;
MIG_BITMAP_T	*bitmap;
int		*bitmap_size;
{
	START_VPROCSERVER(SYS_nx_part_bitmap,3)      
	error = nx_part_bitmap(vproc_port, inode,bitmap,bitmap_size);
	END_VPROCSERVER(SYS_nx_part_bitmap)
}

int
nx_part_bitmap(vp,inode,bitmap,bitmap_size)
struct vproc 	*vp;
ino_t		inode;
MIG_BITMAP_T	*bitmap;		/* OUT, dealloc (ool memory) */
int		*bitmap_size;		/* size of the above */
{
    int         error,retcode, rc, ok;
    uid_t       puid;
    gid_t       pgid;
    struct pvproc *pvp;
    int         max_retries = MAX_RPC_RETRIES;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*bitmap = 0;
	*bitmap_size = 0;

#ifndef NX
	return (EOPNOTSUPP);
#endif

	error = 0;
    if (nx_init_ports() == -1){
	error = EPALLOCERR;
	goto nx_part_bitmap_error;
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    pvp   = PVP(vp);
    /* grab effective ids */
    puid  =  u.u_cred->cr_uid;
    pgid  =  u.u_cred->cr_gid;

    do {
	ux_server_thread_blocking();
	retcode = (int)get_part_bitmap(allocator_port,inode,puid,pgid,
				       bitmap,
				       (mach_msg_type_number_t *)bitmap_size,&rc);
	ux_server_thread_unblocking();
    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

    if (ok != ESUCCESS)
	error = ok;

nx_part_bitmap_error:
	return(error);
}

int
NX_nx_matching_bitmap(vproc_port,interrupt,bitmap,bitmap_size,
  selector_buf, selector_buflen, exact)
mach_port_t	vproc_port;
boolean_t	*interrupt;
MIG_BITMAP_T	*bitmap;
int		*bitmap_size;
char*		selector_buf;
int		selector_buflen;
int		exact;
{
	START_VPROCSERVER(SYS_nx_part_bitmap,3)      
	error = nx_matching_bitmap(vproc_port,bitmap,bitmap_size,
	  selector_buf,selector_buflen,exact);
	END_VPROCSERVER(SYS_nx_part_bitmap)
}

int
nx_matching_bitmap(vp,bitmap,bitmap_size,
  selector_buf,selector_buflen,exact)
struct vproc 	*vp;
MIG_BITMAP_T	*bitmap;		/* OUT, dealloc (ool memory) */
int		*bitmap_size;		/* size of the above */
char*		selector_buf;
int		selector_buflen;
int		exact;
{
    int         error,retcode, rc, ok;
    uid_t       puid;
    gid_t       pgid;
    struct pvproc *pvp;
    int         max_retries = MAX_RPC_RETRIES;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*bitmap = 0;
	*bitmap_size = 0;

#ifndef NX
	return (EOPNOTSUPP);
#endif

	error = 0;
    if (nx_init_ports() == -1){
	error = EPALLOCERR;
	goto nx_matching_bitmap_error;
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    pvp   = PVP(vp);
    /* grab effective ids */
    puid  =  u.u_cred->cr_uid;
    pgid  =  u.u_cred->cr_gid;

    do {
	ux_server_thread_blocking();
	retcode = (int)get_matching_bitmap(allocator_port,puid,pgid,
				       bitmap,
				       (mach_msg_type_number_t *)bitmap_size,
				       selector_buf, selector_buflen,exact,&rc);
	ux_server_thread_unblocking();
    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

    if (ok != ESUCCESS)
	error = ok;

nx_matching_bitmap_error:
	return(error);
}

int
NX_nx_mypart(vproc_port, interrupt, rows, cols)
        mach_port_t     vproc_port;
        boolean_t       *interrupt;
        long            *rows;
        long            *cols;
{
        START_VPROCSERVER(SYS_nx_mypart, 2)

        error = nx_mypart(vproc_port, rows, cols);

        END_VPROCSERVER(SYS_nx_mypart)
}

int
nx_mypart(vp, rows, cols)
        struct vproc    *vp;
        long            *rows;
        long            *cols;
{
        APPLINFO_T    appl_info;
        LP_MAP_T      lp_map;
        struct pvproc *pvp;
        long          lp_map_count;
        kern_return_t retcode;
        int             error;

        pvp = PVP(vp);

        if (pvp->pvp_flag & PV_NX_PARTITIONED) {
                error = 0;
                ux_server_thread_blocking();
                retcode = nx_task_get_info(privileged_host_port,
                             pvp->pvp_pproc->p_task,
                             &appl_info,
                             &lp_map,
                             (mach_msg_type_number_t *)&lp_map_count);
                ux_server_thread_unblocking();
                if ( retcode != KERN_SUCCESS) {
                                error = retcode;
                                goto nx_mypart_error;
                        }
                *rows = appl_info.rows;
                *cols = appl_info.columns;
		vm_deallocate(mach_task_self(), (vm_address_t)lp_map,
                      (lp_map_count * sizeof(LP_MAP_ENTRY_T)));
        }
        else
                error = EANOEXIST;

nx_mypart_error:
    return(error);
}

int
NX_nx_app_info(vproc_port,interrupt,pgroup,appinfo)
mach_port_t	vproc_port;
boolean_t	*interrupt;
pid_t		pgroup;
nx_app_info_t	*appinfo;
{
	START_VPROCSERVER(SYS_nx_app_info,2)
	error = nx_app_info(vproc_port, pgroup,appinfo);
	END_VPROCSERVER(SYS_nx_app_info)
}

int
nx_app_info(vp,pgroup,appinfo)
struct vproc 	*vp;
pid_t		pgroup;
nx_app_info_t	*appinfo;
{
    int         ok,error,retcode, rc;
    uid_t       puid;
    gid_t       pgid;
    struct pvproc *pvp;
    int         max_retries = MAX_RPC_RETRIES;

#ifndef NX
	return (EOPNOTSUPP);
#endif

	error = 0;
    if (nx_init_ports() == -1){
	error = EPALLOCERR;
	goto nx_app_info_error;
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    pvp = PVP(vp);
    /* grab effective ids */
    puid  =  u.u_cred->cr_uid;
    pgid  =  u.u_cred->cr_gid;

    do {
	ux_server_thread_blocking();
	retcode = get_nx_app_info(allocator_port,pgroup,puid,pgid,
				      appinfo,&rc);
	ux_server_thread_unblocking();

    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);
    if (rc != ESUCCESS)
	error = rc;

nx_app_info_error:
	return(error);
}

#ifdef PARACORE
int
nx_app_part(vp,pgroup,appinfo,partinfo)
struct vproc 	*vp;
pid_t		pgroup;
nx_app_info_t	*appinfo;
PARTREQ_T	*partinfo;
{
    int         ok,error,retcode, rc;
    uid_t       puid;
    gid_t       pgid;
    struct pvproc *pvp;
    int         max_retries = MAX_RPC_RETRIES;

#ifndef NX
	return (EOPNOTSUPP);
#endif

	error = 0;
    if (nx_init_ports() == -1){
	error = EPALLOCERR;
	goto nx_app_part_error;
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    pvp = PVP(vp);
    /* grab effective ids */
    puid  =  u.u_cred->cr_uid;
    pgid  =  u.u_cred->cr_gid;

    do {
	ux_server_thread_blocking();
	retcode = get_nx_app_part(allocator_port,pgroup,puid,pgid,
				      appinfo,partinfo,&rc);
	ux_server_thread_unblocking();

    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);
    if (rc != ESUCCESS)
	error = rc;

nx_app_part_error:
	return(error);
}
#endif /* PARACORE */

int
NX_nx_app_node_list(vproc_port,interrupt,pgroup,node_list,node_list_size)
mach_port_t	vproc_port;
boolean_t	*interrupt;
pid_t		pgroup;
LP_MAP_T	*node_list;
int		*node_list_size;
{
	START_VPROCSERVER(SYS_nx_app_node_list,3)      
	error = nx_app_node_list(vproc_port, pgroup,node_list,node_list_size);
	END_VPROCSERVER(SYS_nx_app_node_list)
}

int
nx_app_node_list(vp,pgroup,node_list,node_list_size)
struct vproc 	*vp;
pid_t		pgroup;
LP_MAP_T	*node_list;		/* OUT, dealloc (ool memory) */
int		*node_list_size;	/* size of the above */
{
    int         error,retcode, rc, ok;
    uid_t       puid;
    gid_t       pgid;
    struct pvproc *pvp;
    int         max_retries = MAX_RPC_RETRIES;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*node_list = 0;
	*node_list_size = 0;

#ifndef NX
	return (EOPNOTSUPP);
#endif

	error = 0;
    if (nx_init_ports() == -1){
	error = EPALLOCERR;
	goto nx_app_node_list_error;
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    pvp = PVP(vp);
    /* grab effective ids */
    puid  =  u.u_cred->cr_uid;
    pgid  =  u.u_cred->cr_gid;


    do {
	ux_server_thread_blocking();
	retcode = (int) get_app_node_list(allocator_port,pgroup,puid,pgid,
					   node_list,
					   (mach_msg_type_number_t *)node_list_size,
					   &rc);
	ux_server_thread_unblocking();
    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);
    if (rc != ESUCCESS)
	error = rc;

nx_app_node_list_error:
	return(error);
}

int
NX_nx_free_node_list(vproc_port,interrupt,inode,bitmap,bitmap_size)
mach_port_t	vproc_port;
boolean_t	*interrupt;
ino_t		inode;
MIG_BITMAP_T	*bitmap;
int		*bitmap_size;
{
	START_VPROCSERVER(SYS_nx_free_node_list,3)      
	error = nx_free_node_list(vproc_port, inode,bitmap,bitmap_size);
	END_VPROCSERVER(SYS_nx_free_node_list)
}

int
nx_free_node_list(vp,inode,bitmap,bitmap_size)
struct vproc 	*vp;
ino_t		inode;
MIG_BITMAP_T	*bitmap;		/* OUT, dealloc (ool memory) */
int		*bitmap_size;		/* size of the above */
{
    int         ok,error,retcode, rc;
    uid_t       puid;
    gid_t       pgid;
    struct pvproc *pvp;
    int         max_retries = MAX_RPC_RETRIES;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*bitmap = 0;
	*bitmap_size = 0;

#ifndef NX
	return (EOPNOTSUPP);
#endif

	error = 0;
    if (nx_init_ports() == -1){
	error = EPALLOCERR;
	goto nx_free_nodes_error;
    }

    /*
     *  Get the task, uid and gid out of the proc structure
     */
    pvp   = PVP(vp);

    /* grab effective ids */
    puid  =  u.u_cred->cr_uid;
    pgid  =  u.u_cred->cr_gid;


    do {
	ux_server_thread_blocking();
	retcode = (int)get_part_free_nodes(allocator_port,inode,
				       puid,pgid,
				       bitmap,
				       (mach_msg_type_number_t *)bitmap_size,&rc);
	ux_server_thread_unblocking();
    } while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

    if (ok != ESUCCESS)
	error = ok;

nx_free_nodes_error:
	return(error);
}

int
NX_nx_mkpart_rpc(vproc_port,interrupt,buffer,buf_len,num_nodes)
mach_port_t	vproc_port;
boolean_t	*interrupt;
MIG_BITMAP_T	buffer;
int		buf_len;
int		*num_nodes;
{
	START_VPROCSERVER(SYS_nx_mkpart_rpc,3)
	error = nx_mkpart_rpc(vproc_port,buffer,buf_len,num_nodes);
	END_VPROCSERVER(SYS_nx_mkpart_rpc)
}

int
nx_mkpart_rpc(vp,buffer,buf_len,num_nodes)
struct vproc 	*vp;
MIG_BITMAP_T    buffer;
int             buf_len;
int		*num_nodes;
{
	int		error,retcode, rc, ok;
	uid_t		euid,puid;
	gid_t		egid,pgid;
	struct pvproc	*pvp;
        int             max_retries = MAX_RPC_RETRIES;

#ifndef NX
        return (EOPNOTSUPP);
#endif

	error = 0;
	if (nx_init_ports() == -1){
		error = EPALLOCERR;
		goto nx_mkpart_rpc_error;
	}

	/*
	*  Get the task, uid and gid out of the proc structure
	*/
	pvp = PVP(vp);
	puid  =  pvp->pvp_pproc->p_ruid;
	pgid  =  pvp->pvp_pproc->p_rgid;
        /* grab effective ids */
        euid  =  u.u_cred->cr_uid;
        egid  =  u.u_cred->cr_gid;

	do {
	    ux_server_thread_blocking();
	    retcode = allocator_mkpart(allocator_port,puid,pgid,
				       buffer,buf_len,num_nodes,&rc);
	    ux_server_thread_unblocking();
	} while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

	if (ok != ESUCCESS)
	    error = ok;

nx_mkpart_rpc_error:
	vm_deallocate(mach_task_self(), (vm_address_t )buffer,
		 (vm_size_t)(buf_len));

	return(error);

}

int
NX_nx_chpart_rpc(vproc_port,interrupt,buffer,buf_len)
mach_port_t	vproc_port;
boolean_t	*interrupt;
char		*buffer;
int		buf_len;
{
	START_VPROCSERVER(SYS_nx_chpart_rpc,2)
	error = nx_chpart_rpc(vproc_port,buffer,buf_len);
	END_VPROCSERVER(SYS_nx_chpart_rpc)
}

int
nx_chpart_rpc(vp,buffer,buf_len)
struct vproc	*vp;
char		*buffer;
int		buf_len;
{
	int		error,retcode, rc, ok;
	uid_t		euid,puid;
	gid_t		egid,pgid;
	struct pvproc	*pvp;
        int             max_retries = MAX_RPC_RETRIES;

#ifndef NX
        return (EOPNOTSUPP);
#endif

	error = 0;
	if (nx_init_ports() == -1){
		error = EPALLOCERR;
		goto nx_chpart_rpc_error;
	}

	/*
	*  Get the task, uid and gid out of the proc structure
	*/
	pvp = PVP(vp);
	puid  =  pvp->pvp_pproc->p_ruid;
	pgid  =  pvp->pvp_pproc->p_rgid;
        /* grab effective ids */
        euid  =  u.u_cred->cr_uid;
        egid  =  u.u_cred->cr_gid;

	do {
	    ux_server_thread_blocking();
	    retcode = chpart(allocator_port,buffer,buf_len,puid,pgid,&rc);
	    ux_server_thread_unblocking();
	} while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

	if (ok != ESUCCESS)
	    error = ok;

nx_chpart_rpc_error:
	vm_deallocate(mach_task_self(), (vm_address_t)buffer,
		 (vm_size_t)(buf_len));
	return(error);
}


int
NX_nx_rmpart_rpc(vproc_port,interrupt,pathname,pathlen,inode,flags)
mach_port_t	vproc_port;
boolean_t	*interrupt;
char		*pathname;
int		pathlen;
ino_t		inode;
int		flags;
{
	START_VPROCSERVER(SYS_nx_rmpart_rpc,4)
	error = nx_rmpart_rpc(vproc_port,pathname,pathlen,inode,flags);
	END_VPROCSERVER(SYS_nx_rmpart_rpc)
}

int
nx_rmpart_rpc(vp,pathname,pathlen,inode,flags)
struct vproc	*vp;
char		*pathname;
int		pathlen;
ino_t		inode;
int		flags;
{
	int		error,retcode, rc, ok;
	uid_t		puid;
	gid_t		pgid;
	struct pvproc	*pvp;
        int             max_retries = MAX_RPC_RETRIES;

#ifndef NX
        return (EOPNOTSUPP);
#endif

	error = 0;
	if (nx_init_ports() == -1){
		error = EPALLOCERR;
		goto nx_rmpart_rpc_error;
	}

	/*
	*  Get the task, uid and gid out of the proc structure
	*/
	pvp = PVP(vp);
        /* grab effective ids */
        puid  =  u.u_cred->cr_uid;
        pgid  =  u.u_cred->cr_gid;

	do {
	    ux_server_thread_blocking();
	    retcode = part_remove(allocator_port,pathname,pathlen,inode,
				  puid,pgid,flags,&rc);
	    ux_server_thread_unblocking();
	} while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

	if (ok != ESUCCESS)
	    error = ok;

nx_rmpart_rpc_error:
	vm_deallocate(mach_task_self(), (vm_address_t)pathname,
		 (vm_size_t)(pathlen));
	return(error);
}

int
NX_nx_pspart_rpc(vproc_port,interrupt,inode,buffer,buf_len)
mach_port_t	vproc_port;
boolean_t	*interrupt;
ino_t		inode;
char		*buffer;
int		*buf_len;
{
	START_VPROCSERVER(SYS_nx_pspart_rpc,3)
	error = nx_pspart_rpc(vproc_port,inode,buffer,buf_len);
	END_VPROCSERVER(SYS_nx_pspart_rpc)
}

int
nx_pspart_rpc( vp, inode, buffer, buf_len )
	struct vproc		*vp;
	ino_t			inode;
	MIG_BITMAP_T		*buffer;	/* OUT, dealloc (ool memory) */
	mach_msg_type_number_t	*buf_len;	/* size of the above */
{
	int		error,retcode, rc, ok;
	uid_t		euid,puid;
	gid_t		egid,pgid;
	struct pvproc	*pvp;
        int             max_retries = MAX_RPC_RETRIES;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*buffer = 0;
	*buf_len = 0;

#ifndef NX
        return (EOPNOTSUPP);
#endif

	error = 0;
	if (nx_init_ports() == -1){
		error = EPALLOCERR;
		goto nx_pspart_rpc_error;
	}

	/*
	*  Get the task, uid and gid out of the proc structure
	*/
	pvp = PVP(vp);
	puid  =  pvp->pvp_pproc->p_ruid;
	pgid  =  pvp->pvp_pproc->p_rgid;
        /* grab effective ids */
        euid  =  u.u_cred->cr_uid;
        egid  =  u.u_cred->cr_gid;

	do {
	    ux_server_thread_blocking();
	    retcode = pspart(allocator_port,inode,euid,egid,buffer,buf_len,&rc);
	    ux_server_thread_unblocking();
	} while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

	if (ok != ESUCCESS)
	    error = ok;

nx_pspart_rpc_error:
	return(error);
}

int
NX_nx_alloc_exit_rpc(vproc_port,interrupt,pgroup)
mach_port_t	vproc_port;
boolean_t	*interrupt;
pid_t		pgroup;
{
	START_VPROCSERVER(SYS_nx_alloc_exit_rpc,1)
	error = nx_alloc_exit_rpc(vproc_port,pgroup);
	END_VPROCSERVER(SYS_nx_alloc_exit_rpc)
}

int
nx_alloc_exit_rpc(vp,pgroup)
struct vproc	*vp;
pid_t		pgroup;
{
	int		error,retcode, rc, ok;
        int             max_retries = MAX_RPC_RETRIES;

	/* Must be root to make this call */
	if (error = suser(u.u_cred, &u.u_acflag))
        	return (EACCES);

	do {
	    ux_server_thread_blocking();
	    retcode = remove_nx_appl(allocator_port,pgroup,&rc);
	    ux_server_thread_unblocking();
	} while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

	if (ok != ESUCCESS)
	    error = ok;

	return(error);

}


int
NX_nx_register_daemon(vproc_port,interrupt,daemon_id,socket_addr,sock_addr_len)
mach_port_t	vproc_port;
boolean_t	*interrupt;
int		daemon_id;
u_char		*socket_addr;
int		sock_addr_len;
{
	START_VPROCSERVER(SYS_nx_register_daemon,3)
	error = nx_register_daemon(vproc_port,daemon_id,socket_addr,sock_addr_len);
	END_VPROCSERVER(SYS_nx_register_daemon);
}

int
nx_register_daemon(vp,daemon_id,socket_addr,sock_addr_len)
struct vproc	*vp;
int		daemon_id;
u_char		*socket_addr;
int		sock_addr_len;
{
	int		error,retcode, rc, ok;
	uid_t		puid;
        int             max_retries = MAX_RPC_RETRIES;

	/* Must be root to make this call */
	if (error = suser(u.u_cred, &u.u_acflag))
        	return (EACCES);

	do {
	    ux_server_thread_blocking();
	    retcode = register_daemon_allocator(allocator_port,daemon_id,socket_addr,
						sock_addr_len,&rc);
	    ux_server_thread_unblocking();
	} while ((ok=nx_check_allocator_response(retcode, rc, max_retries--)) == EAGAIN);

	if (ok != ESUCCESS)
	    error = ok;

	return(error);

}

int
nx_allocator_node(p, args, retval)
    struct proc *p;
    void *args;
    int *retval;
{


#ifndef NX
	return (EOPNOTSUPP);
#endif


    DEBUGOUT(("nx_allocator_node:%d\n", allocator_node));

    *retval = allocator_node;
    retval[1] = 0;
    return(0);
}

int
nx_attach_pgrp(vp, pgrp)
struct vproc *vp;
pid_t         pgrp;
{
int error, i;
struct	      vproc *vs;
struct        pvproc *pvp, *pvs;


    pvp = PVP(vp);
    vs = LOCATE_VPROC_PID(pgrp);
    if ( vs == NULL ) {
	return(ESRCH);
    }
    pvs = PVP(vs);
    VPROC_LOCK_FLAG(vs, "nx_attach_pgrp");
    pvs->pvp_flag |= PV_NX_ATH_PGRP;
    VPROC_UNLOCK_FLAG(vs, "nx_attach_pgrp");

    if (tam_table == (TAM_TABLE_T)NULL) {
	error = nx_allocate_tam_table();
	if (error != ESUCCESS) {
		VPROC_RELEASE(vs, "nx_attach_pgrp");
		return (error);
	}
    }

    TAM_TABLE_LOCK;
    for(i=0;i<TAM_TABLE_SIZE;i++) {
	if (tam_table[i].pgrp == 0) {
	    tam_table[i].pgrp = pgrp;
	    tam_table[i].tam_pid = pvp->pvp_pproc->p_pid;
	    DEBUGOUT(("nx_attach_pgrp(tam_pid=%d, pgrp=%d)\n", 
		       pvp->pvp_pproc->p_pid, pgrp));
	    TAM_TABLE_UNLOCK;
	    VPROC_RELEASE(vs, "nx_attach_pgrp");
	    return (ESUCCESS);
	}
    }
    TAM_TABLE_UNLOCK;
    VPROC_RELEASE(vs, "nx_attach_pgrp");
    return (EQTAM);
}

int
NX_attach_pgrp(vproc_port, interrupt, pgrp)
	mach_port_t	vproc_port;
	boolean_t	*interrupt;
	pid_t            pgrp;
{

	START_VPROCSERVER(SYS_nx_attach_pgrp, 1)

	error = nx_attach_pgrp(vproc_port, pgrp);

	END_VPROCSERVER(SYS_nx_attach_pgrp)
}
int
nx_detach_pgrp(vp, pgrp)
struct vproc *vp;
pid_t         pgrp;
{
int i;
struct        pvproc *pvp;


    pvp = PVP(vp);

    TAM_TABLE_LOCK;
    for(i=0;i<TAM_TABLE_SIZE;i++) {
	if (tam_table[i].pgrp == pgrp && 
	    tam_table[i].tam_pid ==  pvp->pvp_pproc->p_pid) {
	    tam_table[i].pgrp = 0;
	    tam_table[i].tam_pid = 0;
	    TAM_TABLE_UNLOCK;
	    return (ESUCCESS);
	}
    }
    TAM_TABLE_UNLOCK;
    return (EQPGRP);
}

int
NX_detach_pgrp(vproc_port, interrupt, pgrp)
	mach_port_t	vproc_port;
	boolean_t	*interrupt;
	pid_t            pgrp;
{

	START_VPROCSERVER(SYS_nx_detach_pgrp, 1)

        error = nx_detach_pgrp(vproc_port, pgrp);

	END_VPROCSERVER(SYS_nx_detach_pgrp)
}
int
nx_join(vp, pgrp, numnodes)
struct vproc *vp;
pid_t         pgrp;
int          *numnodes;
{
int i;
struct        pvproc *pvp;
struct        vproc  *vp_pgrp;
pid_t          pid;
APPLINFO_T     applinfo;
LP_MAP_T       nodelist;
int            nodelistcnt;
int            error = ESUCCESS;
int            has_priv;
uid_t          uid;
int            ret;
unsigned long	pgrp_pvp_flag;

    

    *numnodes = 0;
    pvp = PVP(vp);

    (void) pproc_get_attr(0,&pid,0,0,0,&has_priv,&uid,0,0,0);

    /* Locate the vproc for the process group leader. */
    vp_pgrp = LOCATE_VPROC_PID(pgrp);
    if (vp_pgrp == (struct vproc *)NULL) {
	return(EQPGRP);
    }

    /* Make sure the process group is an NX application */
    PVPOP_NX_GET_FLAG(vp_pgrp, &pgrp_pvp_flag);
    if( !(pgrp_pvp_flag & PV_NX_APPLICATION)) {
	error = EQPGRP;
	goto out;
    }

    /* Get the nodelist and applinfo for the process */
    if (PVPOP_NX_GET_INFO(vp_pgrp, 
			  &applinfo, 
			  &nodelist, 
			  &nodelistcnt, 
			  has_priv ? VPROC_HAS_PRIV : 0,
			  uid, &error) !=
	KERN_SUCCESS) {
	error = EQPGRP;
	goto out;
    }
    if (error != ESUCCESS) 
	goto out;

    /*
     *  Process calling nx_join() MUST be on a node in the
     *  partition.
     */
    error = EQNODE;
    for(i=0;i<nodelistcnt;i++) {
	if (this_node == nodelist[i]) {
	    error = ESUCCESS;
	    break;
	}
    }
    if (error != ESUCCESS)
	goto dealloc;

    applinfo.app = -1;
    if ((ret=nx_put_info(pvp,
		    nodelist,
		    nodelistcnt,
		   &applinfo,
		    pid)) != KERN_SUCCESS) {
	error = EQPGRP;
    } else {
	*numnodes = nodelistcnt - 1;

	VPROC_LOCK_FLAG(vp, "nx_join");
	pvp->pvp_flag |= PV_NX_PARTITIONED;
	VPROC_UNLOCK_FLAG(vp, "nx_join");
    }

 dealloc:
    vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
		  (vm_size_t)(nodelistcnt * sizeof(LP_MAP_ENTRY_T)));

 out:
    VPROC_RELEASE(vp_pgrp, "nx_join");
    return (error);
}

int
NX_join(vproc_port, interrupt, pgrp, numnodes)
	mach_port_t	vproc_port;
	boolean_t	*interrupt;
	pid_t            pgrp;
        int             *numnodes;
{
	START_VPROCSERVER(SYS_nx_join, 1)

        error = nx_join(vproc_port, pgrp, numnodes);

	END_VPROCSERVER(SYS_nx_join)
}

/* Return the PID of process who sent SIGCHLD to TAM */
nx_tam_wait(vp, args, retval)
struct   vproc *vp;
void    *args;
int     *retval;
{
struct args {
	int	pgrp;
	int    *status;
	int     option;
} *uap = (struct args *) args;

pid_t               tam_pid;
boolean_t           tam_has_priv, found_a_process = FALSE;
int                 tam_uid, uid;
int                 error = ESUCCESS;
struct vproc       *vc;    /* Potentially interesting vproc.     */
struct pvproc      *pvc;   /* Aboves physical vproc              */
struct pvproc      *pvp;   /* Physical vproc of calling process. */
int                 status;
struct rusage_dev   ru;
extern struct       pvproc_ops dpvproc_ops_table;
extern struct mutex vproc_list_lock;
pid_t	pid = 0;    /* pid of interest. */
pid_t   pgid = 0;   /* group id. */
pid_t   ppid = 0;   /* pid of parent. */
pid_t   pvc_pgid;   /* group id of pid of interest. */
struct	vproc *vpp;	/* parents vproc */
unsigned long pvp_flag; /* Local copy of pvp_flag */
int state;



    *retval = 0;
    if (uap->pgrp == WAIT_ANY)
		return(EINVAL);
    else if (uap->pgrp > 0)
                pid = uap->pgrp;
    else if (uap->pgrp  == 0)
                return(EINVAL);
    else if (uap->pgrp < 0)
                 pgid = -(uap->pgrp);

    pvp = PVP(vp);

    if( pid ) {

		vc = LOCATE_VPROC_PID(pid);	

    		if( vc == (struct vproc *)NULL) {
			DEBUGOUT(("nx_tam_wait: pid %d not found\n",pid));
			return(ECHILD);
    		}

		pvc = PVP(vc);

		/* Get attributes about the pid of interest. */
       		error =  PVPOP_GET_ATTR(vc,
				      (pid_t *)&ppid,
				      (pid_t *)&pvc_pgid,
				      (pid_t *)0,
				      (uid_t *)&uid,
				      (uid_t *)0,
				      (boolean_t *)0,
				      (boolean_t *)0,
				      FALSE);
		if (error) {
		        VPROC_RELEASE(vc, "nx_tam_wait");
			return(error);
		}

    		tam_pid = nx_lookup_tam( pvc_pgid );
    		if (tam_pid == 0) {
		        VPROC_RELEASE(vc, "nx_tam_wait");
			return(EQTAM);
		}
    		
		if (tam_pid != vp->vp_pid) {
		        VPROC_RELEASE(vc, "nx_tam_wait");
			return(EPERM);
		}

    		(void) pproc_get_attr(0,0,0,0,0,(int *)&tam_has_priv,
			  (uid_t *)&tam_uid,0,0,0);


		VPROC_LOCK_FLAG(vc, "nx_tam_wait");
		pvp_flag = pvc->pvp_flag;
		VPROC_UNLOCK_FLAG(vc, "nx_tam_wait");

        	if ((pvp_flag & PV_NX_ATH_PGRP) &&
            			(tam_uid == uid || tam_has_priv)) {
                	DEBUGOUT(("nx_tam_wait:returning pid:%d\n", vc->vp_pid));
                	DEBUGOUT(("nx_tam_wait:tam_pid:%d tam_uid:%d uid=%d priv:%d\n",
                       		tam_pid, tam_uid, uid, tam_has_priv));

                	DEBUGOUT(("nx_tam_wait:pvp_flag:0x%x p_stat:0x%x p_flag:0x%x\n",
                       			pvc->pvp_flag,
                       			pvc->pvp_pproc->p_stat,
                       			pvc->pvp_pproc->p_flag));

                	if (pvp_flag & PV_SZOMB || pvp_flag & PV_SSTOP) {
				pid_t	pgid = WAIT_ANY;

				vpp = LOCATE_VPROC_PID(ppid);

                    		error = PVPOP_REAP(vc, &pgid,
                                       uap->option, &state, &status, &ru);
				if(( error != ESUCCESS) && 
					(uap->option & WNOHANG)) {
					VPROC_RELEASE(vpp, "nx_tam_wait");
					VPROC_RELEASE(vc, "nx_tam_wait");
					*retval = 0;
					return (ESUCCESS);
				}
				
				
				if(!( uap->option & WNOWAIT ) && 
						!(pvp_flag & PV_SZOMB)) {
					
						pvc->pvp_pproc->p_flag |= SWTED;
				}
				if(!( uap->option & WNOWAIT ) &&
						( pvp_flag & PV_SZOMB )) {
                                        error = PVPOP_RMV_CHILD_FROM_PARENT( vpp, vc );
					if(pvc->pvp_ppid == pgid) {
						(void) PVPOP_RMV_PGRP_LIST(vpp, vc, 0);
					}
                                }


                    		DEBUGOUT(("reap returns %d\n", error));

                    		if (uap->status && error == ESUCCESS) {
                        		error = copyout((caddr_t)&status,
                                        (caddr_t)uap->status,
                                        sizeof(status));
				}
                        	*retval = vc->vp_pid;
                                if( pvc->pvp_flag & PV_SZOMB ) {
                                        (void) PVPOP_REPORT_STATE(
                                                VPROCPTR(pvc->pvp_ppid),
                                                vc, VPROC_ZOMBIE);
                                }

				VPROC_RELEASE(vpp, "nx_tam_wait");
				VPROC_RELEASE(vc, "nx_tam_wait");
				return(ESUCCESS);
			
			}
    			if (!(uap->option & WNOHANG)) {
				DEBUGOUT(("(%d)tam_wait Blocking\n", vp->vp_pid));
				if (!(error = pproc_sleep_waiting(pvp->pvp_pproc))) {
	    				DEBUGOUT(("(%d):TAM woke up!!\n", tam_pid));
				}
			}
			VPROC_RELEASE(vc, "nx_tam_wait");
			if(error != ESUCCESS)
	    			return (error);

			return(ESUCCESS);
		}
		VPROC_RELEASE(vc, "nx_tam_wait");
    		return(ECHILD);
	}

    tam_pid = nx_lookup_tam( pgid );
    if (tam_pid == 0)
	return(EQTAM);

    if (tam_pid != vp->vp_pid)
	return(EQTAM);

    (void) pproc_get_attr(0,0,0,0,0,(int *)&tam_has_priv,
			  (uid_t *)&tam_uid,0,0,0);

    DEBUGOUT(("nx_tam_wait:pgrp=%d\n", pgid));

    /* Scan the PGRP list for eligible processes */
 loop:
    VPROC_LIST_LOCK();
    for (vc = vproc; vc != vprocNVPROC, vc != NULL; vc = vc->vp_next) {


	/* Skip the tam and vproc which are going away. */
	if (vc->vp_pid == vp->vp_pid || vc->vp_pid == -1)
	    continue;

	 /* Skip if remote */
	pvc = PVP(vc);

	VPROC_LOCK_FLAG(vc, "nx_tam_wait");
	pvp_flag = pvc->pvp_flag;
	VPROC_UNLOCK_FLAG(vc, "nx_tam_wait");

	if (!(pvp_flag & PV_IS_LOCAL) ||
	    pvc->pvp_pproc == (struct proc *)NULL)
	    continue;

	/* Get attributes about the pid of interest. */
	error =  PVPOP_GET_ATTR(vc,
			      (pid_t *)&ppid,
			      (pid_t *)&pvc_pgid,
			      (pid_t *)0,
			      (uid_t *)&uid,
			      (uid_t *)0,
			      (boolean_t *)0,
			      (boolean_t *)0,
			      FALSE);

	/* Skip if error on this pid */
	if (error) {
    	DEBUGOUT(("nx_tam_wait:pid=%d PVPOP_GET_ATTR error\n",vc->vp_pid ,error));
                continue;
	}

       /* Skip if the process does not belong to the pgroup */
       if( pvc_pgid != pgid) {
    		DEBUGOUT(("nx_tam_wait:pid=%d didn't match pgrp=%d\n",vc->vp_pid ,pgid));
                continue;
	}

        /* Skip if already reaped */
        if((pvc->pvp_pproc->p_flag & SWTED)) {
                continue;
        }
	

	DEBUGOUT(("nx_tam_wait: checking pid:%d\n", vc->vp_pid));


	if ((pvp_flag & PV_NX_ATH_PGRP) &&
	    (tam_uid == uid || tam_has_priv)) {
	        VPROC_HOLD(vc, "nx_tam_wait");
	        DEBUGOUT(("nx_tam_wait:returning pid:%d\n", vc->vp_pid));
		DEBUGOUT(("nx_tam_wait:tam_pid:%d tam_uid:%d uid=%d priv:%d\n",
		       tam_pid, tam_uid, uid, tam_has_priv));

		DEBUGOUT(("nx_tam_wait:pvp_flag:0x%x p_stat:0x%x p_flag:0x%x\n",
		       pvc->pvp_flag,
		       pvc->pvp_pproc->p_stat,
		       pvc->pvp_pproc->p_flag));
		
		if (pvp_flag & PV_SZOMB || pvp_flag & PV_SSTOP) {
		    pid_t	pgid = WAIT_ANY;
		    VPROC_LIST_UNLOCK();

		    vpp = LOCATE_VPROC_PID(ppid);

		    error = PVPOP_REAP(vc, &pgid,
				       uap->option, &state, &status, &ru);
			if ( error == EAGAIN ) {
				VPROC_RELEASE(vpp, "nx_tam_wait9");
				continue;
			}
			if(( error == ESRCH) && 
				(uap->option & WNOHANG)) {
				VPROC_RELEASE(vpp, "nx_tam_wait");
				*retval = 0;
				return (ESUCCESS);
			}
			if(!( pvp_flag & PV_SZOMB )) {
				pvc->pvp_pproc->p_flag |= SWTED;
			}
			if(!( uap->option & WNOWAIT ) && 
					!(pvp_flag & PV_SZOMB)) {
				
					pvc->pvp_pproc->p_flag |= SWTED;
			}

			if(!( uap->option & WNOWAIT ) &&
					( pvp_flag & PV_SZOMB )) {
                                        error = PVPOP_RMV_CHILD_FROM_PARENT( vpp, vc );
					if(pvc->pvp_ppid == pgid) {
						(void) PVPOP_RMV_PGRP_LIST(vpp, vc, 0);
                                }
                        }
		    VPROC_LIST_LOCK();

		    DEBUGOUT(("reap returns %d\n", error));

		    if (uap->status && error == ESUCCESS) {
			error = copyout((caddr_t)&status,
					(caddr_t)uap->status,
					sizeof(status));
		    }
		    if (error == ESUCCESS) {
	        	found_a_process = TRUE;
			*retval = vc->vp_pid;
			break;
		    }
		}
	    }
    }
    VPROC_LIST_UNLOCK();
    if (!found_a_process) {
                if (uap->option & WNOHANG) {
                        *retval = 0;
                        return(ESUCCESS);
                }
	return(ECHILD);
    } else {
                        if( pvc->pvp_flag & PV_SZOMB ) {
                                (void) PVPOP_REPORT_STATE(
                                        VPROCPTR(pvc->pvp_ppid),
                                        vc, VPROC_ZOMBIE);
                        }
   }


    DEBUGOUT(("nx_tam_wait:option:%d pvp_flag:0x%x found:%d\n",
	   uap->option, pvc->pvp_flag, found_a_process));


    if (((pvp_flag & PV_SSTOP) || (pvp_flag & PV_SZOMB)) 
	&& error == ESUCCESS) {
	VPROC_RELEASE(vc, "nx_tam_wait");
	return(ESUCCESS);
    }

    VPROC_RELEASE(vc, "nx_tam_wait");
    if (!(uap->option & WNOHANG)) {
	DEBUGOUT(("(%d)tam_wait Blocking\n", vp->vp_pid));
	if (!(error = pproc_sleep_waiting(pvp->pvp_pproc))) {
	    DEBUGOUT(("(%d):TAM woke up!!\n", tam_pid));
	    goto loop;
	}

	if (error != ESUCCESS)
	    return (error);
    }
    return(error);
}

/* trace an NX process. */
nx_tam_ptrace(vp, args, retval)
struct vproc *vp;
void *args;
int *retval;
{
register struct args {
	int	req;
	int	pid;
	int	*addr;
	int	data;
} *uap = (struct args *) args;
pid_t  tam_pid;
struct vproc  *v;
int    error = ESUCCESS;
int    request;
extern struct pvproc_ops dpvproc_ops_table;



    v = LOCATE_VPROC_PID(uap->pid);
    if (v == (struct vproc *)NULL)
	return(EINVAL);

    if(uap->req == PT_TRACE_ME) {
	error = pproc_ptrace(PVP(v)->pvp_pproc,PT_TRACE_ME,
			uap->addr,uap->data,retval);
	goto out;
    }


    tam_pid = nx_lookup_tam( PVP(v)->pvp_pgid );
    if (tam_pid == 0) {
	DEBUGOUT(("PTRACE:nx_tam lookup failed for pgroup = %d\n",PVP(v)->pvp_pgid ));
	error = EQTAM;
	goto out;
    }


    /* Skip if remote */
    if ((struct pvproc_ops *)PVP(v)->pvp_ops != (struct pvproc_ops *)&dpvproc_ops_table) {
	DEBUGOUT(("PTRACE:process is remote\n"));
	error = EINVAL;
	goto out;
    }

    if (!(PVP(v)->pvp_flag & PV_NX_ATH_PGRP)) {
	DEBUGOUT(("PTRACE:Not attached to Tam \n"));
	error = EINVAL;
	goto out;
    }

    switch (uap->req) {
    case PT_READ_I:		/* read word in child's I space */
	request = VPROC_PT_READ_I; break;
    case PT_READ_D:		/* read word in child's D space */
	request = VPROC_PT_READ_D; break;
    case PT_READ_U:		/* read word in child's user structure */
	request = VPROC_PT_READ_U; break;
    case PT_WRITE_I:	/* write word in child's I space */
	request = VPROC_PT_WRITE_I; break;
    case PT_WRITE_D:	/* write word in child's D space */
	request = VPROC_PT_WRITE_D; break;
    case PT_WRITE_U:	/* write word in child's user structure */
	request = VPROC_PT_WRITE_U; break;
    case PT_CONTINUE:	/* continue the child */
	request = VPROC_PT_CONTINUE; break;
    case PT_KILL:		/* kill the child process */
	request = VPROC_PT_KILL; break;
    case PT_STEP:		/* single step the child */
	request = VPROC_PT_STEP; break;
    default:
	error = EINVAL;
	goto out;
    }

    error = PVPOP_PTRACE(v, request, uap->addr, uap->data, retval);
 out:
    VPROC_RELEASE(v, "nx_tam_ptrace");
    return(error);
}

/* Turn off partitioning for this node. */
int
join_root_part(vp)
struct   vproc *vp;
{
    struct pvproc *pvp;
    int    error;


    if (error = suser(u.u_cred, &u.u_acflag))
	return (error);

    pvp = PVP(vp);

    if (pvp->pvp_flag & PV_NX_APPLICATION)
	return (EINVAL);

    VPROC_LOCK_FLAG(vp, "join_root_part");
    pvp->pvp_flag &= ~PV_NX_PARTITIONED;
    VPROC_UNLOCK_FLAG(vp, "join_root_part");
    return(ESUCCESS);
}
	
int
NX_join_root_part(vproc_port, interrupt)
	mach_port_t	vproc_port;
	boolean_t	*interrupt;
{
	START_VPROCSERVER(SYS_join_root_part, 1)

	error = join_root_part(vproc_port);

	END_VPROCSERVER(SYS_join_root_part);
}

/*-------------------------- RPCs ----------------------------------*/

int
NX_node_self(vproc_port, mynode)
	mach_port_t	vproc_port;
        int            *mynode;
{
int ii;
struct vproc *vprocp;
APPLINFO_T    appl_info;
LP_MAP_T      lp_map;
int           lp_map_count;
struct vproc *vp;
struct        pvproc *pvp;


    *mynode = -1;
    vp = port_to_vproc_lookup(vproc_port);

    pvp = PVP(vp);

    if (pvp->pvp_flag & PV_NX_PARTITIONED) {
	if (nx_task_get_info(privileged_host_port, 
			     pvp->pvp_pproc->p_task,
			     &appl_info,
			     &lp_map,
			     (mach_msg_type_number_t *)&lp_map_count) 
	    != KERN_SUCCESS) {
	    return(0);
	}

	for(ii=0;ii<lp_map_count;ii++) {
	    if (lp_map[ii] == this_node) {
		*mynode = ii;
	    }
	}
	vm_deallocate(mach_task_self(), (vm_address_t)lp_map,
		      (lp_map_count * sizeof(LP_MAP_ENTRY_T)));
    }
    vproc_end_vp_port_op(vp, "end_vprocserver_op");
    return(0); 
}

/* ARGSUSED */
reset_boot_node_list(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
	    char	  *nodelist;
	    unsigned int   nodelistlen;
	    boolean_t      local_node_only;
	} *uap = (struct args *) args;

	char        *nodelist;
	unsigned int nodelistlen = 0;
	int          error = ESUCCESS;

#if	SEC_BASE
	if (!privileged(SEC_SYSATTR, EPERM))
		return (EPERM);
#else   
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif

	if (uap->nodelistlen == 0 && uap->local_node_only)
	    return(EINVAL);

	if (uap->nodelistlen > 0) {
	    nodelistlen = uap->nodelistlen + 2;
	    error = vm_allocate(mach_task_self(), 
				(vm_address_t *)&nodelist, 
				(vm_size_t)nodelistlen,
				TRUE);
	    if (error != KERN_SUCCESS)
		return(EINVAL);

	    error = copyin((caddr_t)uap->nodelist, nodelist, 
			   uap->nodelistlen+1);

	    /*
	     * Need to put a line feed at the end of the string
	     * so that find_string() will think this is a bootmagic.
	     */
	    nodelist[uap->nodelistlen]   = '\n';
	    nodelist[uap->nodelistlen+1] = '\000';
	} else {
	    nodelist = (char *)NULL;
	}


	if (!error)
	    error = VPSOP_RESET_BOOT_NODE_LIST(nodelist, nodelistlen,
					       uap->local_node_only);

	if (uap->nodelistlen > 0)
	    vm_deallocate(mach_task_self(), (vm_address_t)nodelist, 
			  (vm_size_t)nodelistlen);

	return (ESUCCESS);
}

/*
 * convert an ASCII string (decimal radix) to an integer, side-effects returns
 * a pointer to the numeric scan's terminating char.
 *
 * inputs:
 *	p	string pointer.
 *	t	char **, return a pointer to the character which terminated
 *		the numeric string scan.
 * outputs:
 *	decimal integer value of the numeric string.
 *
 * side effect:
 *	pointer to terminating character written to (*t).
 */

atoi_term(p,t)
	register char *p;	/* IN */
	register char **t;	/* OUT */
{
        register int n;
        register int f;

        n = 0;
        f = 0;
        for(;;p++) {
                switch(*p) {
                case ' ':
                case '\t':
                        continue;
                case '-':
                        f++;
                case '+':
                        p++;
                }
                break;
        }
        while(*p >= '0' && *p <= '9')
                n = n*10 + *p++ - '0';

        /* return pointer to terminating character */
        if ( t )
                *t = p;

        return(f? -n: n);
}

/*
 * convert an ASCII string representation of the "BOOT_NODE_LIST" to a
 * bitvector. BNL is `\n` <newline> terminated.
 *
 * inputs:
 *	bnl	(char *)BOOT_NODE_LIST
 *	ns	(node_status_t)node status bitvector
 *
 * outputs:
 *	TRUE	success
 *	FALSE	errors.
 *
 * side-effects:
 *	bitvector 'ns' modified: bit-number according to node number from bnl
 *	is set to indicate.
 */

convert_bnl_to_node_status_bitvector(bnl, ns)
	char		*bnl;	/* IN */
	node_status_t	ns;	/* OUT */
{
	char		*term;
	register boolean_t	done;

	if ( bnl == (char *)0 )
		return FALSE;

	/* clean slate */
	bzero( ns, sizeof(node_status_t) );

	/*
	 * scan the BNL converting the node numbers into integers and then
	 * setting the corresponding bit in the node status bitvector.
	 */
	for( term=bnl,done=FALSE; !done; term++ ) {
		register int		node;
		register unsigned	bits, *status;

		node = atoi_term(term, &term);
		if ( *term == '\n' || *term == '\0' )
			done = TRUE;
		/*
		 * process a node range, example: "4..9"
		 */
		if ( *term == '.' ) {
			register int	enode;

			if ( *(term+1) != '.' ) {
				term++;	/* bad syntax */
				continue;
			}
			term += 2;	/* skipover ".." */
			enode = atoi_term(term, &term);	/* get ending node # */
			if ( *term == '\0' )
				done = TRUE;
			/*
			 * mark the nodes in the range as alive.
			 * do start thru end-1; fall into single-node code
			 * for the range-end node.
			 */
			for( ; node < enode; node++ ) {
				bits = (1 << (node % 32));
				status = &ns[ ( node / 32) ];
				*status |= bits;
			}
		}
		bits = (1 << (node % 32));
		status = &ns[ ( node / 32) ];
		*status |= bits;
	}
	return TRUE;
}

#if 0
dbg_nodes_up( ns )
        node_status_t   ns;
{
        int     kr;

        printf("SVR: node %d belives online: ",this_node);
        for(kr=0; kr < 128; kr++) {
                register unsigned j, sb, node;

                sb = ns[kr];
                for(j=0,node=1; j < 32; j++) {
                        if ( sb & node )
                                printf("%d ", j+(kr*32));
                        node = node << 1;
                }
        }
        printf("\n");
}
#endif

/*
 * reset the local micro-kernel's idea of which nodes are online.
 *
 * inputs:
 *	priv_host_port	this hosts's priviledged host port.
 *	nodelist	BOOT_NODE_LIST <newline> terminated.
 *
 * outputs:
 *	none.
 *
 * side effects:
 *	MK's idea of online nodes is altered.
 */

reset_mk_node_list( priv_host_port, nodelist )
	host_priv_t	priv_host_port;
	char		*nodelist;
{
	kern_return_t  kr;
	node_status_t	ns;

	if ( nodelist == (char *)0 ) {
		printf("reset_mk_node_list() BOOT_NODE_LIST == <NULL>?\n");
	}

	/*
	 * convert the bootmagic string "BOOT_NODE_LIST" to a bitvector
	 * representing online nodes.
	 */
	convert_bnl_to_node_status_bitvector(nodelist, ns);

	/*
	 * inform the local MK of the new online node list.
	 */
	if ((kr=norma_node_set_status( priv_host_port, ns )) != KERN_SUCCESS) {
		printf("norma_node_put_status() failure, kr %d\n",kr);
	}
}

/*
 * reset the local MK's and server's idea of the new bootNode list.
 *
 * inputs:
 *	nodelist	<newline> terminated bootmagic "BOOT_NODE_LIST=nodes".
 *	nodelistlen	string length.
 *
 * outputs:
 *	none.
 *
 * side effects:
 *	MK online node status reset, servers's BOOT_NODE_LIST string rewritten.
 */

pps_reset_boot_node_list(nodelist, nodelistlen)
    char *nodelist;
    int   nodelistlen;
{
	/*
	 * Inform local micro-kernel about the new bootnode list.
	 */
	reset_mk_node_list( privileged_host_port, nodelist );

	/*
	 * *** warning ***
	 *	reset_boot_magic_var() trashes the nodeList string; see
	 *	uxkern/boot_config.c.
	 */
	reset_boot_magic_var(nodelist, nodelistlen);
}

/*------------------------- Utility Functions -------------------------------*/

boolean_t
nx_application( p )
struct proc *p;
{
struct vproc *vp;
struct        pvproc *pvp;

	vp = p->p_vproc;
	pvp = PVP(vp);
	return (pvp->pvp_flag & PV_NX_APPLICATION);
}


boolean_t
nx_tam_attached( p )
struct proc *p;
{
struct vproc *vp;
struct        pvproc *pvp;

	vp = p->p_vproc;
	pvp = PVP(vp);
	return (pvp->pvp_flag & PV_NX_ATH_PGRP);
}

boolean_t
nx_application_vproc( v )
struct vproc *v;
{
	return (PVP(v)->pvp_flag & PV_NX_APPLICATION);
}

boolean_t
nx_gang_state( p )
struct proc *p;
{
struct vproc *vp;
struct        pvproc *pvp;

        vp = p->p_vproc;
        pvp = PVP(vp);
        return (pvp->pvp_flag & PV_NX_GANG_STOP);

}


boolean_t
nx_proxy( vp )
struct vproc *vp;
{
struct        pvproc *pvp;
unsigned long pvpflag;
unsigned long pflag;


	/* 
	 *  Make sure the process is not exiting
	 */


	PVPOP_NX_GET_FLAG(vp, &pvpflag);

	if(pvpflag & PV_SZOMB ) {
		return(FALSE);
	}

	PVPOP_NX_GET_P_FLAG(vp, &pflag);
	
	if ((pvpflag & PV_NX_APPLICATION) &&
	    (pvpflag & PV_PGRPLEADER) && !(pflag & STRC )) {
		return(TRUE);
	} else {
		return(FALSE);
	}
}


boolean_t
nx_no_signal( p )
struct proc *p;
{
struct        pvproc *pvp;
struct        vproc *vp;

	vp = LOCATE_VPROC_PID(p->p_pid);
	pvp = PVP(vp);
	if ((pvp->pvp_flag & PV_NX_APPLICATION) != PV_NX_APPLICATION){
	    VPROC_RELEASE(vp, "nx_no_signal");
	    return FALSE;
	}

        if (p->p_cursig == SIGUSR2) {
	    p->p_cursig = 0;
	    VPROC_RELEASE(vp, "nx_no_signal");
	    return TRUE;
	}

        VPROC_RELEASE(vp, "nx_no_signal");

        return FALSE;
}


int
init_service_partition( init_proc )
struct proc *init_proc;
{
APPLINFO_T    appl_info;
LP_MAP_T      lp_map;
int           lp_map_count, num_entries;
struct vproc *vp;
struct        pvproc *pvp;
LP_MAP_T      get_node_list(struct proc *init_proc, int *lp_map_count);
int ii = 0;
boolean_t     bad_lp_map = FALSE, root_fs_in_list = FALSE;


    lp_map = get_node_list(init_proc, &lp_map_count);
    if (lp_map == (LP_MAP_T)NULL) {
	return(0);
    }
    appl_info.app = -1;

    num_entries = lp_map_count;

    DEBUGOUT(("lp_map_count:%d\n", lp_map_count));
    while(!bad_lp_map && ii < lp_map_count) {
	/*
	 *  Enforce that the ROOT_FS_NODE must be 
	 *  in the service partition.  Paragon's will not
	 *  boot otherwise.
	 */
	if (lp_map[ii] == root_fs_node) {
	    root_fs_in_list = TRUE;
	}
	/*
	 *  Verify that the nodes in the service partition are a
         *  subset of BOOT_NODE_LIST.
	 */
	bad_lp_map = !is_node_valid(lp_map[ii]);

	DEBUGOUT(("lp_map[%d]:%d\n",ii, lp_map[ii]));
	ii++;
    }

    if (!root_fs_in_list) bad_lp_map = TRUE;

    /*
     *  If we have a bad node list, then give them a single
     *  node service partition containing only the ROOT_FS_NODE.
     */
    if (bad_lp_map) {
	printf("INVALID SERVICE PARTITION:");
	printf("Reverting to single node service partition.\n");
	lp_map[0] = root_fs_node;
	lp_map_count = 1;
    }

    DEBUGOUT(("task:0x%08x pid:%d\n", init_proc->p_task, init_proc->p_pid));
     
    if (nx_task_put_info(privileged_host_port,
			 lp_map, 
			 (mach_msg_type_number_t)lp_map_count,
			 appl_info,
			 init_proc->p_task,
			 init_proc->p_pid) == KERN_SUCCESS) {

	vp = init_proc->p_vproc;
	pvp = PVP(vp);
	VPROC_LOCK_FLAG(vp, "init_service_partition");
	pvp->pvp_flag |= PV_NX_PARTITIONED;
	VPROC_UNLOCK_FLAG(vp, "init_service_partition");
    }

    vm_deallocate(mach_task_self(), (vm_address_t)lp_map,
		 (num_entries * sizeof(LP_MAP_ENTRY_T)));
    return 0;
}

#define SERVICE_PARTINFO "/etc/nx/service/.partinfo"
#define MAX_LINE_SIZE 8192
#define PART_SIZE 5
#define NODE_LIST 11
#define NUM_LINES 14
#define LF 0xa


int
atoi( str )
char *str;
{
int  retval = 0;

  while (*str) {
      if (*str == ',' || *str == LF) {
	  return retval;
      }
      if (*str >= '0' && *str <= '9') {
	  retval = retval*10 + (*str - '0');
      }
      str++;
  }
  return retval;
}

char
*skip_to_comma( str )
char *str;
{

    while (*str) {
	if (*str == ',' || *str == LF) {
	    return str;
	}
	str++;
    }
    return (char *)NULL;
}  

LP_MAP_T
get_node_list(struct proc *init_proc, int *lp_map_count_p)
{
LP_MAP_T            lp_map = (LP_MAP_T)NULL;
struct nameidata   *ndp = &u.u_nd;
struct proc         *save_proc = u.u_procp;
struct vnode_proxy *vp = (struct vnode_proxy *) NULL;
int    error, line, ret, i;
boolean_t finished;
char  *start, *end;
char   buf[MAX_LINE_SIZE];
extern mach_port_t root_vnode_port;
char  *read_part_line(struct vnode_proxy *vp, char *buf, int bufsize);

	*lp_map_count_p = 0;
	unix_master();
        u.u_procp = init_proc;

	ndp->ni_cdirport = root_vnode_port;
        ndp->ni_rdirport = root_vnode_port;
	ndp->ni_dirp = SERVICE_PARTINFO;
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg =  UIO_SYSSPACE;	

	error = remote_exec_lookup(ndp, &vp);
	if (error)
	    goto out;

	for(line=1;line<=NUM_LINES;line++) {
	    start = read_part_line(vp, (char *)buf, sizeof(buf));
            if ( start == (char *)NULL ) {
                goto out;
            }

	    if (line == PART_SIZE) {
		*lp_map_count_p = atoi(start);
	    }
 
	    if (line == NODE_LIST && *lp_map_count_p > 0) {
		ret = vm_allocate(mach_task_self(), 
				 (vm_address_t *)&lp_map, 
				  (vm_size_t)(*lp_map_count_p 
					      * sizeof(LP_MAP_ENTRY_T)), 
				  TRUE);
		if (ret != KERN_SUCCESS)
		    goto out;

		for (i = 0; (end = skip_to_comma(start)); i++) {
		    finished = (*end == LF);
		    *end = '\0';
		    if (i < *lp_map_count_p)
			lp_map[i] = atoi(start);
		    if (finished)
			break;
		    start = end + 1;
		}
		break;
	    }
	}
		

    out:
	if (vp) {
	    remote_vfree(vp);
	}

        u.u_procp = save_proc;

        unix_release();

        /* 
	 * Is no service partition, then define a partition of size 1
	 * which is the root_fs_node.
	 */
        if (lp_map == (LP_MAP_T)NULL) {
	    *lp_map_count_p = 1;
	    ret = vm_allocate(mach_task_self(), 
			      (vm_address_t *)&lp_map, 
			      (vm_size_t)(*lp_map_count_p 
					  * sizeof(LP_MAP_ENTRY_T)), 
			      TRUE);
	    if (ret != KERN_SUCCESS) {
		*lp_map_count_p = 0;
		lp_map = (LP_MAP_T)NULL;
	    } else {
		lp_map[0] = root_fs_node;
	    }
	}

	return lp_map;
}

static int use_buf = FALSE;
static int cur_file_offset = 0;
static int cur_buf_offset = 0;

char 
*read_part_line(struct vnode_proxy *vp, char *buf, int bufsize)
{
int   error;
int   actual, ii, resid;
char *retval;



    if (!use_buf) {
	error= remote_exec_read(vp, (caddr_t)buf, bufsize, 
				(off_t)cur_file_offset, IO_UNIT, &resid);
	if (error)
	    return (char *)NULL;
	
	actual = bufsize - resid;
	cur_file_offset = cur_file_offset + actual;
	cur_buf_offset = 0;
	use_buf = TRUE;
    }

    retval = NULL;
    for(ii = cur_buf_offset;ii<bufsize;ii++) {
	if (buf[ii] == LF) {
	    retval = &buf[cur_buf_offset];
	    cur_buf_offset = ii + 1;
	    break;
	}
    }
    if (cur_buf_offset >= bufsize) {
	use_buf = FALSE;
    }
    return retval;
}

int
nx_init_ports()
{
    kern_return_t kr;
    mach_port_t   remote_nameserver_port;
    int rc;
    int retry_count = 0;
    int retry_max = 10;


    if (allocator_port != MACH_PORT_NULL) return 0;

    /* loop until the remote lnsvr is up */
 get_remote_ns_port:
    ux_server_thread_blocking();
    if ((kr = norma_get_nameserver_port(privileged_host_port, 
					allocator_node,
					&remote_nameserver_port))
	!= KERN_SUCCESS)
	panic("Unable to get nameserver port.");

    ux_server_thread_unblocking();
    if (remote_nameserver_port == MACH_PORT_NULL || 
	remote_nameserver_port == MACH_PORT_DEAD) {
	thread_yield();
	goto get_remote_ns_port;
    }

    if (allocator_port == MACH_PORT_NULL)
    do {
	ux_server_thread_blocking();
	rc = netname_look_up(remote_nameserver_port, "",
			     "ALLOCATOR", 
			     &allocator_port);
	ux_server_thread_unblocking();
	if (rc != KERN_SUCCESS && retry_count > retry_max) {

	    DEBUGOUT(("Unable to get allocator port\n"));

	    return -1;
	}
	retry_count++;
	if (rc != KERN_SUCCESS)
	    thread_yield();
    } while(rc != KERN_SUCCESS);

    DEBUGOUT(("Lookup of alloctor ports successful!!\n"));
    return 0;
}


int
nx_allocate_tam_table()
{
int ret, i;

    mutex_init(&tam_table_lock);
    ret = vm_allocate(mach_task_self(), 
		     (vm_address_t *)&tam_table,
		     (vm_size_t)(TAM_TABLE_SIZE * sizeof(TAM_TABLE_ENTRY)), 
		      TRUE);
    if (ret != KERN_SUCCESS) {
	tam_table = (TAM_TABLE_T)NULL;
	return (ENOMEM);
    }
    return (ESUCCESS);
}



int
nx_map_node(pvp, dest_node, applinfo_p, nodelist_p, nodelistcnt_p)
struct pvproc *pvp;
int            dest_node;
APPLINFO_T    *applinfo_p;
LP_MAP_T      *nodelist_p;
int           *nodelistcnt_p;
{
int      mapped_node;
LP_MAP_T tmp_nodelist;
int      tmp_nodelistcnt;
int      rc;

    ux_server_thread_blocking();
    rc = nx_task_get_info(privileged_host_port, 
			 pvp->pvp_pproc->p_task,
			 applinfo_p,
			&tmp_nodelist,
			 (mach_msg_type_number_t *)&tmp_nodelistcnt);
    ux_server_thread_unblocking();
    if (rc == KERN_SUCCESS) {
	if (dest_node >= tmp_nodelistcnt || dest_node < 0) {
	    vm_deallocate(mach_task_self(), (vm_address_t)tmp_nodelist, 
			  (tmp_nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
	    return(-1);
	}
	/* Convert logical to physical node number */
	mapped_node = tmp_nodelist[dest_node];
    } else {
	*nodelist_p = (LP_MAP_T)NULL;
	 return dest_node;;
    }
    *nodelist_p = tmp_nodelist;
    *nodelistcnt_p = tmp_nodelistcnt;
    return mapped_node;
}


int
nx_map_nodelist(pvp, node_array, node_count,
		applinfo_p, nodelist_p, nodelistcnt_p)
struct pvproc *pvp;
int            node_array[];
int            node_count;
APPLINFO_T    *applinfo_p;
LP_MAP_T      *nodelist_p;
int           *nodelistcnt_p;
{
LP_MAP_T tmp_nodelist;
int      tmp_nodelistcnt;
int      startidx, startcnt;
int      rc;


    ux_server_thread_blocking();
    rc = nx_task_get_info(privileged_host_port, 
			  pvp->pvp_pproc->p_task,
			  applinfo_p,
			 &tmp_nodelist,
			 (mach_msg_type_number_t *)&tmp_nodelistcnt);
    ux_server_thread_unblocking();
    if (rc == KERN_SUCCESS) {
	startidx = 0;
	startcnt = node_count;
	do {
	    if (node_array[startidx] >= tmp_nodelistcnt ||
		node_array[startidx] < 0) {
		vm_deallocate(mach_task_self(), (vm_address_t)tmp_nodelist, 
			      (tmp_nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
		return(-1);
	    }
		
	    node_array[startidx] = tmp_nodelist[node_array[startidx]];
	    startidx++;
	    startcnt--;
	} while (startcnt != 0);
    } else {
	*nodelist_p = (LP_MAP_T)NULL;
	return 0;
    }
    *nodelist_p = tmp_nodelist;
    *nodelistcnt_p = tmp_nodelistcnt;
    return 0;
}


int
nx_map_node_proc(p, dest_node)
struct proc *p;
int            dest_node;
{
int        mapped_node;
LP_MAP_T   tmp_nodelist;
int        tmp_nodelistcnt;
int        rc;
APPLINFO_T applinfo;
struct vproc *vp;
boolean_t nx_in_partition(); /* Forward */


    
    vp = LOCATE_VPROC_PID(p->p_pid);
    if (!nx_in_partition(vp)) {
	VPROC_RELEASE(vp, "nx_map_node_proc");
	return dest_node;
    }
    ux_server_thread_blocking();
    rc = nx_task_get_info(privileged_host_port, 
		  	  p->p_task,
			 &applinfo,
			 &tmp_nodelist,
			 (mach_msg_type_number_t *)&tmp_nodelistcnt);
    ux_server_thread_unblocking();
    if (rc == KERN_SUCCESS) {
	if (dest_node >= tmp_nodelistcnt || dest_node < 0) {
	    vm_deallocate(mach_task_self(), (vm_address_t)tmp_nodelist, 
			  (tmp_nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
	    VPROC_RELEASE(vp, "nx_map_node_proc");
	    return(-1);
	}
	/* Convert logical to physical node number */
	mapped_node = tmp_nodelist[dest_node];
	vm_deallocate(mach_task_self(), (vm_address_t)tmp_nodelist, 
		      (tmp_nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
	VPROC_RELEASE(vp, "nx_map_node_proc");
	return mapped_node;
    }
    VPROC_RELEASE(vp, "nx_map_node_proc");
    return dest_node;;
}

int
nx_map_node_pid(pid, dest_node)
pid_t         pid;
int            dest_node;
{
int        mapped_node;
LP_MAP_T   tmp_nodelist;
int        tmp_nodelistcnt;
int        rc, error;
APPLINFO_T applinfo;
struct vproc *vp;
boolean_t nx_in_partition(); /* Forward */


    
    vp = LOCATE_VPROC_PID(pid);
    if (vp == (struct vproc *)NULL) {
	return(-1);
    }
    if (!nx_in_partition(vp)) {
	VPROC_RELEASE(vp, "nx_map_node_pid");
	return dest_node;
    }
    rc = PVPOP_NX_GET_INFO(vp,
			   &applinfo, 
			   &tmp_nodelist, 
			   &tmp_nodelistcnt, 
			    VPROC_HAS_PRIV,
			    0, &error);
    if (rc == KERN_SUCCESS) {
	if (dest_node >= tmp_nodelistcnt || dest_node < 0) {
	    vm_deallocate(mach_task_self(), (vm_address_t)tmp_nodelist, 
			  (tmp_nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
	    VPROC_RELEASE(vp, "nx_map_node_pid");
	    return(-1);
	}
	/* Convert logical to physical node number */
	mapped_node = tmp_nodelist[dest_node];
	vm_deallocate(mach_task_self(), (vm_address_t)tmp_nodelist, 
		      (tmp_nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
	VPROC_RELEASE(vp, "nx_map_node_pid");
	return mapped_node;
    }
    VPROC_RELEASE(vp, "nx_map_node_pid");
    return dest_node;
}

int
nx_get_info(pvp, applinfo_p, nodelist_p, nodelistcnt_p, has_priv, euid, err)
struct pvproc *pvp;
APPLINFO_T    *applinfo_p;
LP_MAP_T      *nodelist_p;
int           *nodelistcnt_p;
int            has_priv;
int            euid;
int           *err;
{
int  uid;
int  rc;
    /*
     * Always init MIG's OUT params for ports and ool memory to null,
     * lest random values be accidentally transmitted in the reply.
     */
    *nodelist_p = 0;
    *nodelistcnt_p = 0;

    (void) pproc_get_attr(pvp->pvp_pproc,0,0,0,0,0,(uid_t *)&uid,0,0,0);

    if ((euid == uid) || has_priv) {
	*err = ESUCCESS;
    
	ux_server_thread_blocking();
	rc = nx_task_get_info(privileged_host_port, 
			      pvp->pvp_pproc->p_task,
			      applinfo_p,
			      nodelist_p,
			      (mach_msg_type_number_t *)nodelistcnt_p);
	ux_server_thread_unblocking();
	return (rc);
    }
    *err = EACCES;
    return (KERN_SUCCESS);
}


int
nx_put_info(pvp, nodelist, nodelistcnt, applinfo_p, pid)
struct pvproc *pvp;
LP_MAP_T       nodelist;
int            nodelistcnt;
APPLINFO_T    *applinfo_p;
pid_t          pid;
{
int ret;
    
    ux_server_thread_blocking();
    ret = nx_task_put_info(privileged_host_port,
			   nodelist, 
			   (mach_msg_type_number_t)nodelistcnt,
			   *applinfo_p,
			   pvp->pvp_pproc->p_task,
			   pid);
    ux_server_thread_unblocking();
    return ret;
}


boolean_t
nx_in_partition( vp )
struct vproc *vp;
{
long pvp_flag;

	PVPOP_NX_GET_FLAG(vp, &pvp_flag);
	return (pvp_flag & PV_NX_PARTITIONED);
}

pid_t
nx_lookup_tam(pid_t pgrp )
{
int i;
pid_t tam_pid = 0;



    if (tam_table == (TAM_TABLE_T)NULL)
	return 0;

    TAM_TABLE_LOCK;
    for(i=0;i<TAM_TABLE_SIZE;i++) {
	if (tam_table[i].pgrp == pgrp) {
	    tam_pid = tam_table[i].tam_pid;
	    break;
	}
    }
    TAM_TABLE_UNLOCK;
    return tam_pid;
}

boolean_t
nx_signal_tam( vc )
struct vproc *vc;
{
struct vproc  *vt;
struct pvproc *pvp;
pid_t         pgrp;
pid_t         tam_pid;
int           error;



    pvp = PVP(vc);

    if (!(pvp->pvp_flag & PV_NX_ATH_PGRP)) {
	DEBUGOUT(("nx_signal_tam: (%d) Not attached to the tam\n", vc->vp_pid));
	return FALSE;
    }


    if ((pvp->pvp_flag & PV_SSTOP) || (pvp->pvp_flag & PV_SZOMB)) {

	pgrp = pvp->pvp_pgid;
	DEBUGOUT(("lookup tam for pgrp: %d\n", pgrp));
	tam_pid = nx_lookup_tam(pgrp);
	if (tam_pid == 0) {
	    DEBUGOUT(("TAM not found\n"));
	    return FALSE;
	}

	/* Do not redirect the signal for the tam itself. */
	if (tam_pid == vc->vp_pid)
	    return FALSE;

	/* Do not send signal to the tam if the parent and the tam pid are the same */

/*
	if( tam_pid  ==  pvp->pvp_ppid) {
		return FALSE;
	}
*/

	DEBUGOUT(("TAM pid:%d\n", tam_pid));
	vt = LOCATE_VPROC_PID(tam_pid);
	if (vt == (struct vproc *)NULL)
	    return FALSE;

	DEBUGOUT(("SIGCHLD to TAM. pid:%d dbg pid:%d\n", 
		  vt->vp_pid, vc->vp_pid));

	error = VPOP_SIGPROC(vt, SIGCHLD, 0, 
			     VPROC_HAS_PRIV | VPROC_CHILD_STOP);

	/* Check again if the TAM has not been reaped, just in case. */
	if (PVP(vt)->pvp_pproc != NULL)
	    (void) pproc_wakeup(PVP(vt)->pvp_pproc);
    
	VPROC_RELEASE(vt, "nx_signal_tam");
	return TRUE;
    }
    return FALSE;
}

boolean_t
nx_trace_inherit( vc, pp_p_flag )
struct vproc *vc;
int    pp_p_flag;
{
struct pvproc *pvp;
int    error;



    pvp = PVP(vc);


    /* Only allow STRC flag inheritance if an NX application. */
    if (!(pvp->pvp_flag & PV_NX_ATH_PGRP)) 
	return FALSE;

    /* Only allow STRC flag inheritance of a TAM has attached to the pgrp. */
    if (nx_lookup_tam(pvp->pvp_pgid) == 0)
	return FALSE;

    if (pp_p_flag & STRC) {

	pvp->pvp_pproc->p_flag |= STRC;
	return TRUE;
    }
    return FALSE;
}

int
nx_get_trace_pflags( p )
struct proc *p;
{

    return(p->p_flag);
}

/*
 *  Call from dvpop_exit() to remove the tam from the tam table.
 */
int
nx_cleanup_tams(vp)
struct vproc *vp;
{
struct pvproc *pvp;
pid_t          pgrp;
pid_t          tam_pid;



    
    pvp = PVP(vp);
    pgrp = pvp->pvp_pgid;
    tam_pid = nx_lookup_tam(pgrp);
    if (tam_pid == 0)
	return;

    if (tam_pid != vp->vp_pid)
	return;

    nx_detach_pgrp(vp, pgrp);
}

int 
nx_traced_add_mem(ve, args, retval)
struct vproc *ve;
void	*args;
int	*retval;
{

	register struct args {
		pid_t	pid;
		int	*addr;
		size_t	length;
	} *uap = (struct args *) args;
	vm_offset_t     addr;
	vm_offset_t     start_addr;
	struct	proc	*p;
	kern_return_t	ret;

	/*
         * Get the proc structure from pid
         */
	
	if(( p = pfind(uap->pid)) == (struct proc *)0 ) {
                return(EPERM);
        }

        /*
         * Check if the process is stopped and traced
         */

        if (p->p_stat != SSTOP || !(p->p_flag & STRC)) {
                return (EPERM);
        }

        /*
         * Allocate the space
         */


	if (copyin((caddr_t)uap->addr, (caddr_t)&addr,
                    sizeof(addr))) {
                        *retval = KERN_INVALID_ADDRESS;
                        return(0);
        }
	start_addr = round_page(addr);

        ret = vm_allocate(p->p_task, (vm_address_t *)&start_addr,
					(vm_size_t)uap->length,
                                FALSE);
        if (ret != KERN_SUCCESS) {
                if (ret == KERN_INVALID_ADDRESS ) {
                        return(EFAULT);
                } else if ( ret == KERN_NO_SPACE ) {
                        return(ENOMEM);
                } else {
                        return(EINVAL);
                }
        }

	ret = vm_protect(p->p_task, start_addr, (vm_size_t)uap->length, 
						FALSE, VM_PROT_ALL);
        if (ret != KERN_SUCCESS) {
                if (ret == KERN_INVALID_ADDRESS ) {
                        return(EFAULT);
                } else if ( ret == KERN_NO_SPACE ) {
                        return(ENOMEM);
                } else {
                        return(EINVAL);
                }
        }
	if (copyout((caddr_t)&start_addr, (caddr_t)uap->addr,
                    sizeof(addr))) {
                        *retval = KERN_INVALID_ADDRESS;
                        return(0);
        }


	return(ESUCCESS);
}

int
nx_traced_delete_mem(ve, args, retval)
struct vproc *ve;
void    *args;
int     *retval;
{

        register struct args {
                pid_t   pid;
                int     addr;
		int	length;
        } *uap = (struct args *) args;
        struct  proc    *p;
        kern_return_t   ret;

        /*
         * Get the proc structure from pid
         */

        if(( p = pfind(uap->pid)) == (struct proc *)0 ) {
                return(EPERM);
        }

        /*
         * Check if the process is stopped and traced
         */

        if (p->p_stat != SSTOP || !(p->p_flag & STRC)) {
                return (EPERM);
        }


	ret = vm_deallocate( p->p_task, (vm_address_t)uap->addr, 
				uap->length); 
        if (ret != KERN_SUCCESS) {
                if (ret == KERN_INVALID_ADDRESS ) {
                        return(EFAULT);
                } else if ( ret == KERN_NO_SPACE ) {
                        return(ENOMEM);
                } else {
                        return(EINVAL);
                }
        }
	return(ESUCCESS);
}

int
nx_check_rpc_waittime(log_node, rpc_waittime_addr)
	node_t  *log_node;
	long    *rpc_waittime_addr;
{

	rpc_waittime = *rpc_waittime_addr;

        if (rpc_waittime == 0) {
                rpc_waittime = 0x5265c00;
		return(0);
        } else if ( rpc_waittime < 240000 ) {
                rpc_waittime = 0x5265c00;
        }
        return(0);
}

#define      BSD_PRINT       0x01

void
nx_print_fatal_message(node, text)
        node_t  node;
        char    *text;
{

        char buf[SMALL_ARRAY_LIMIT];
        char    *bp;

        bzero((caddr_t)buf, SMALL_ARRAY_LIMIT);
        sprintf(buf,"FATAL ERROR: %s : Node %d Failed. Please Reboot Immediately \n",text,
 node);
        if (this_node != root_fs_node) {
                ux_server_thread_blocking();
                (void)ubsd_remote_print(root_fs_port, BSD_PRINT,
                                0, 0, 0, buf, strlen(buf));
                ux_server_thread_unblocking();
        } else {
                printf(buf);
        }
}

/*
 *  Function:  nx_check_allocator_response()
 *
 *  Return Values:
 *                 ESUCCESS               RPC was sucessful.
 *                 EAGAIN                 Retry the RPC.
 *                 EPALLOCERR             Return this error to the user.
 *                 <Other Error>          The RPC was sucessful and the allocator itself
 *                                        returned an error.
 *
 *  Description:
 *      Check the return code from the RPC to the allocator
 *      and reinitialize the allocator port.  If the error from the
 *      allocator RPC was other than MACH_SEND_INVALID_DEST, then
 *      return EPAALLOCERR.
 */
int
nx_check_allocator_response( kern_return_t retcode, int alloc_rc, int retries)
{

    if (retries == 0 && retcode != MACH_MSG_SUCCESS)
	return(EPALLOCERR);

    if (retcode == MACH_SEND_INVALID_DEST ||
	retcode == MIG_SERVER_DIED) {
	if (allocator_port != MACH_PORT_NULL) {
	    mach_port_destroy(mach_task_self(), allocator_port);
	    allocator_port = MACH_PORT_NULL;
	}
	if (nx_init_ports() == -1) {
	    return(EPALLOCERR);
	}
	return(EAGAIN);
    }

    if (retcode != MACH_MSG_SUCCESS)
	return(retcode);

    return(alloc_rc);
}



/*
 *  Set up a notify server for detecting deadname notification of the 
 *  allocator port.  Right now, only worry about deadnames.
 */
#define do_mach_notify_port_deleted	nx_do_mach_notify_port_deleted
#define do_mach_notify_msg_accepted	nx_do_mach_notify_msg_accepted
#define do_mach_notify_port_destroyed	nx_do_mach_notify_port_destroyed
#define do_mach_notify_no_senders	nx_do_mach_notify_no_senders
#define do_mach_notify_send_once	nx_do_mach_notify_send_once
#define do_mach_notify_dead_name	nx_do_mach_notify_dead_name
#define notify_server			nx_notify_server
#define notify_server_routine 		nx_notify_server_routine
#include <mach/notify_server.c>

/*
 * Dead name notification.
 */
kern_return_t
do_mach_notify_dead_name(notify, name)
	mach_port_t	notify;
	mach_port_t	name;
{
extern void ux_server_remove_port();


        (void) mach_port_deallocate(mach_task_self(), name);
	return KERN_SUCCESS;
}

/*
 * Port deleted notification.
 */
kern_return_t
do_mach_notify_port_deleted(notify, name)
	mach_port_t	notify;
	mach_port_t	name;
{
#ifdef DEBUG
	printf("do_mach_notify_port_deleted: name=0x%x ???\n", name);
#endif
	(void) mach_port_deallocate(mach_task_self(), name);
	return KERN_FAILURE;
}

/*
 * Message accepted notification.
 */
kern_return_t
do_mach_notify_msg_accepted(notify, name)
	mach_port_t	notify;
	mach_port_t	name;
{
#ifdef DEBUG
	printf("do_mach_notify_msg_accepted: name=0x%x ???\n", name);
#endif
	(void) mach_port_deallocate(mach_task_self(), name);
	return KERN_FAILURE;
}

/*
 * No senders notification.
 */
kern_return_t
do_mach_notify_no_senders(notify, mscount)
	mach_port_t		notify;
	mach_port_mscount_t	mscount;
{
#if PROFILING
	if (sample_done(notify))
		return KERN_SUCCESS;
#endif
#ifdef DEBUG
	printf("do_mach_notify_no_senders: mscount=0x%x ???\n", mscount);
#endif
	return KERN_FAILURE;
}

/*
 * Port destroyed notification.
 */
kern_return_t
do_mach_notify_port_destroyed(notify, name)
	mach_port_t	notify;
	mach_port_t	name;
{
#ifdef DEBUG
	printf("do_mach_notify_port_destroyed: name=0x%x ???\n", name);
#endif
	(void) mach_port_deallocate(mach_task_self(), name);
	return KERN_FAILURE;
}

/*
 * Send once notification.
 */
kern_return_t
do_mach_notify_send_once(notify)
	mach_port_t	notify;
{
#ifdef	DEBUG
	printf("do_mach_notify_send_once: ???\n");
#endif
	return KERN_FAILURE;
}
