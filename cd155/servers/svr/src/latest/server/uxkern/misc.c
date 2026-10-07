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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: misc.c,v $
 * Revision 1.21  1995/03/09  17:51:16  stans
 *  Reduce the number of times we check for dirty disk buffers. We now have
 *  a working DELAY() macro, wait only 3 minutes instead of days.
 *
 *  Reviewer:lenb,jlitvin
 *  Risk:low
 *  Benefit or PTS #: 12563
 *  Testing: WW07 sats + developer shutdown tests.
 *
 * Revision 1.20  1995/02/11  00:01:21  stans
 *  'lint' picking with typedefs for aclean compile.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.19  1995/02/01  22:27:12  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.18  1994/11/18  20:48:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/09/06  19:21:39  jlitvin
 * Don't just wait a short time before giving up writing dirty buffers to
 * disk when shutting the system down.  Wait a very long time and panic
 * if we are still not done.
 *
 *  Reviewer: dbm & rlg
 *  Risk: low
 *  Benefit or PTS #: 10749
 *  Testing: developer
 *  Module(s): server/uxkern/misc.c
 *
 * Revision 1.16  1994/07/26  00:08:06  jlitvin
 * The server should enter the debugger on an assertion failure, like it
 * does for a server panic.
 *
 *  Reviewer: chrisp
 *  Risk: low
 *  Benefit or PTS #: 10365
 *  Testing: developer
 *  Module(s): server/uxkern/misc.c
 *
 * Revision 1.15  1994/06/18  00:20:40  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.14  1994/04/27  17:36:36  jlitvin
 * Replace mismatched unix_release() call with missing mutex_unlock() call.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 8520
 *  Testing: VSX & customer test case
 *  Module(s): server/uxkern/misc.c
 *
 * Revision 1.13  1994/04/05  15:54:30  jlitvin
 * Add OSF changes for real-time support.
 *
 *  Risk: low
 *  Benefit or PTS #: 8207
 *  Module(s): bsd/kern_resource.c & uxkern/misc.c
 *
 * Revision 1.12  1994/01/26  12:41:11  paul
 *  Reviewer: none (cfg for changes to 1.2 tree)
 *  Risk: low
 *  Benefit or PTS #: 7698
 *  Testing:
 *  Module(s):
 * 	uxkern/misc.c
 * 	uxkern/rpm_clock.c
 * 	bsd/kern_time.c
 * 	bsd/kern_utctime.c
 * 	afs/afs_osi.c
 *
 *
 * Merge of bug fixes for RPM from 1.2 tree - Moved the actual sending of the
 * RPM offset outside of a TIME lock.
 *
 * Revision 1.9.2.2  1994/01/18  20:24:20  paul
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 7698
 *  Testing:
 *  Module(s):
 *      uxkern/misc.c
 *       uxkern/rpm_clock.c
 *       bsd/kern_time.c
 *       afs/afs_osi.c
 *
 * Changes to fix bug 7698 - Time Lock hang
 *    Moved the RPM's offsett transmittal RPC outside of the TIME lock to
 *    prevent deadlocks.
 *
 * misc.c:
 *         Added a comment to resettodr()
 *
 * Revision 1.9.2.1  1993/12/07  16:20:02  cfj
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *
 * Revision 1.11  1994/01/12  17:46:11  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.10  1993/12/03  19:24:24  paul
 * Various fixes to RPM support, plus support for global setting of Timezone
 *
 * Moved the VPSOP_RPMOFFSET() call into rpm_set_time() (which is in
 * rpm_clock.c) to hide some of the details of the rpm offset stuff from
 * resettodr..
 *
 *
 *  Reviewer: John Litvin (jlitvin@ssd.intel.com) Brent Olsen (bolsen@locus.com)
 *  Risk: Moderate
 *  Benefit or PTS #:Fixes bug #s 3503 5303 6029 7299
 *  Testing: functionality checked on olympus.sd.locus.com w/RPM support
 *  Module(s): server/uxkern/misc.c
 *
 * Revision 1.9  1993/10/29  11:54:19  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.8  1993/07/14  18:43:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:04:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/07  18:48:38  nandy
 * Resolved a merge conflict
 *
 * Revision 1.6  1993/05/06  19:31:15  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.5  1993/04/03  03:12:23  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  06:05:39  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.1.2.1  1992/12/14  23:24:35  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.4  1992/12/11  18:28:31  cfj
 * Change mach_sample_thread() to thread_sample() yet again.
 *
 * Revision 1.3  1992/12/11  03:06:38  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:55:25  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/24  19:10:34  cfj
 * Modified Debugger() so that it checks for second_server and only
 * prints the pid of the server if second_server is TRUE.
 *
 * Revision 1.1.2.2  1992/11/23  18:35:18  nandy
 * Added proper ifdefs. This should allow us to build a server with 
 * STD+WS-second.
 *
 * Revision 1.1.2.1  1992/11/05  23:44:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:54:14  cfj
 * Bump major revision number.
 *
 * Revision 2.18  1992/11/02  21:50:55  cfj
 * Final integration and testing of IPD modifications
 *
 * Revision 2.17  1992/10/23  02:00:45  cfj
 * T6 merge.
 *
 * Revision 2.16  1992/10/16  18:17:54  cfj
 * Modification so that NX programs can run.
 *
 * Revision 2.18  1992/11/02  21:50:55  cfj
 * Final integration and testing of IPD modifications
 *
 * Revision 2.17  92/10/23  02:00:45  cfj
 * T6 merge.
 * 
 * Revision 2.16  1992/10/16  18:17:54  cfj
 * Modification so that NX programs can run.
 *
 * Revision 2.14  1992/10/10  01:26:12  cfj
 * Modified profil_get() and profil_set() to use thread_sample().
 * Also fix eat_buffers() so that the message size is reset before each
 * call to mach_msg_receive().
 *
 * Revision 2.15  92/12/01  11:55:11  chrisp
 * Split boot() into pps_bootsync() and pps_boot(). The former is always
 * 	executed on the root fs node to flush and sync filesystems, and
 * 	the latter halts or reboots a node. For non-TNC, rebooting
 * 	the root fs node involving rebooting all server nodes within
 * 	pps_boot() explicitly.
 * 
 * Revision 2.14  92/11/23  15:57:10  klh
 * 	Revision 2.14  92/11/03  13:12:01  mmp
 * 		Shutdown changes: on a normal (non-panic) reboot, root_fs_node will
 * 		call sync and vflushall, then send an async halt message to all other
 * 		nodes.  Also added code for the other server nodes to register when
 * 		they start up.  (mmp)
 * 
 * Revision 2.13  92/10/06  12:22:46  roman
 * Revision 1.1.1.1  1993/05/03  17:53:01  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.16  1993/04/24  18:45:26  klh
 *      Revision 2.19  94/02/28  11:11:32  rabii
 * 	        Added real-time support
 * 
 * 	Revision 2.18  93/04/08  11:41:55  loverso
 * 		Include synch.h to get null spl macro defs and avoid function calls.
 * 		[1992/06/04  14:18:28  condict]
 *
 * 		ux server threads are wired by default. (loverso)
 *
 * 	Revision 2.17  93/02/25  12:51:13  loverso
 * 		Don't have Debugger() call second_getpid() if not SECOND_SERVER.
 * 		(condict)
 *
 * 	Revision 2.16  93/01/07  11:15:05  condict
 * 		Return error value from profil_get() if copyout() fails.
 * 		[1992/09/25  16:52:03  emcmanus]
 *
 * 		Updated to new MK profiling interface.  Request and deal with a
 * 		no-more-senders notification on the samples reply port.  Use new
 * 		prof_server() to do most of the work.
 * 		[1992/09/24  19:54:21  emcmanus]
 *
 * Revision 2.15  92/12/01  11:55:11  chrisp
 * Split boot() into pps_bootsync() and pps_boot(). The former is always
 * 	executed on the root fs node to flush and sync filesystems, and
 * 	the latter halts or reboots a node. For non-TNC, rebooting
 * 	the root fs node involving rebooting all server nodes within
 * 	pps_boot() explicitly.
 * 
 * Revision 2.14  92/11/23  15:57:10  klh
 * 	Revision 2.14  92/11/03  13:12:01  mmp
 *              Shutdown changes: on a normal (non-panic) reboot,
 *		root_fs_node will call sync and vflushall, then
 *		send an async halt message to all other nodes.
 *		Also added code for the other server nodes to
 *		register when they start up.  (mmp)
 * 
 * Revision 2.13  92/10/06  12:22:46  roman
 * Fix RCS comments.
 * 
 * Revision 2.12  92/10/05  13:38:25  klh
 * 	Revision 2.13  92/09/29  16:49:03  rabii
 * 		Sync the disks when rebooting.  Also call vflushall (new 
 *		routine) before that to flush out mapped files.  (mmp)
 * 
 * 	Revision 2.12  92/09/11  09:29:46  rabii
 * 		Fix to share_lock_solid from Gernoble (rabii)
 * 
 * 	Revision 2.11  92/08/26  12:14:16  loverso
 * 		Indicate errors if kernel does not include profiling.  (loverso)
 * 
 * 	Revision 2.10  92/08/13  19:21:35  rabii
 * 		Removed "if (second_server)" from panic routine (rabii)
 * 
 * Revision 2.11  92/08/06  13:41:15  klh
 * 	Revision 2.9  92/07/28  20:00:16  rabii
 * 		Modified Debugger to call task_suspend instead of reboot (rabii)
 * 
 * Revision 2.10  92/07/17  10:07:38  chrisp
 * Correct placement of parentheses in call to shared_lock_solid() in
 * 	routine share_lock().
 * 
 * Revision 2.9  92/07/10  09:13:37  chrisp
 * For MAP_UAREA, change the shared locking routines not to use the master
 * 	mutex but a new per-process mutex instead. Also reformat some
 * 	code in this area.
 * 
 * Revision 2.8  92/05/24  13:59:28  pjg
 * 	Revision 3.27  92/03/31  15:41:14  emcmanus
 * 	Name the eat_buffers thread.
 * 
 * 	Revision 3.26  92/03/23  18:04:31  condict
 * 	Remove bogus include of <mach.h>.
 * 
 * Revision 2.7  92/03/09  13:37:46  durriya
 * 	92/02/28  00:11:41  condict
 * 	Move get_thread_pc here from the machine-dependent file.  It is now
 * 	machine-independent (rewritten to use get_thread_state_pc.
 * 
 * 	92/02/26  13:22:16  sp
 * 	Upgrade to 1.0.4
 * 
 * 	92/02/25  17:48:42  condict
 * 	Change all calls to cthread_wire to ux_thread_wire, so ux_server_loop
 * 	can correctly compute required number of Mach kernel threads.
 * 	Also, fix bug in share_lock_solid: parens were wrong in check for 
 * 	whether the who field of the lock is a valid proc ptr.
 * 
 * 	92/01/07  23:35:36  condict
 * 	Fix share_lock_solid so it doesn't panic when lock times out.  Instead,
 * 	awaken all waiters and reinitialize lock properly.
 * 
 * 	91/12/11  15:12:40  condict
 * 	Change the mpsleep on on a share lock into a condition wait.  Fixes bug
 * 	whereby mplseep could be called recursively.
 * 
 * 	91/12/09  18:49:52  emcmanus
 * 	Removed the ux_thread_unwire code when the profile sample thread exits,
 * 	since it never does exit.
 * 	Removed the just_return test, since gdb's return command can be used.
 * 
 * Revision 2.6  91/12/17  10:54:58  roy
 * 	91/12/11  15:12:40  condict
 * 	Change the mpsleep on on a share lock into a condition wait.  Fixes 
 * 	bug whereby mplseep could be called recursively.
 * 
 * 	91/12/09  18:49:52  emcmanus
 * 	Removed the cthread_unwire code when the profile sample thread exits,
 * 	since it never does exit.
 * 	Removed the just_return test, since gdb's return command can be used.
 * 
 * 	91/11/13  14:56:04  barbou
 * 	Added code to restore the console state when halting a second server.
 * 
 * 	91/10/23  16:39:07  condict
 * 	Add microtime function and get_time function (the latter needed only if
 * 	not using mapped time).
 * 
 * 	91/10/18  17:16:04  jose
 * 	Removed the swapon stub.
 * 
 * 	91/10/17  18:34:01  barbou
 * 	Debugger() prints the pid of the (ssecond) server: this is the pid to be
 * 	attached by gdb.
 * 
 * 	91/10/17  15:45:35  condict
 * 	Add definition of resettodr, to improve code re-use.
 * 
 * 	91/10/10  15:57:22  barbou
 * 	Enter the debugger after panic as a second server. Also wait for the 
 * 	debugger (suspend) instead of trapping: the debugger may not be 
 * 	attached.   
 * 
 * 	91/10/09  18:40:45  emcmanus
 * 	profil() should be nosys() not a panic when profiling is not in the 
 * 	server.
 * 
 * Revision 2.5  91/12/13  10:23:26  roy
 * 	91/11/18  14:11:37  roy
 * 	Remove swapon() stub.
 * 
 * Revision 2.4  91/11/05  12:15:20  sjs
 * Fixed RCS comments
 * 
 * Revision 2.3  91/10/14  13:24:02  sjs
 * 	Revision 3.11  91/10/01  14:12:24  condict
 * 	Remove ifndef OSF1_SERVER.
 * 
 * 	Revision 3.10  91/09/27  12:02:52  emcmanus
 * 	Profiling support.
 * 
 * 	Revision 3.9  91/09/18  17:47:47  condict
 * 	If boot is called because we paniced, go into Debugger instead 
 *	of exiting.
 * 
 * Revision 2.2  91/08/31  14:26:56  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.8  91/08/27  15:38:44  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.7  91/07/29  16:32:40  barbou
 * Modified to decide at run-time if running as a second server.
 * 
 * Revision 3.6  91/07/18  15:47:32  benjamin
 * Added hi-resolution clock functions (only with compile
 * flag MP_SLAVE_AS_CLK = config option HI_RES_CLOCK):
 * 
 *  - init_hi_res_clock() maps mach 3.0 kernel's high-resolution
 *    clock into the osf1 single server's virtual address space.
 *    Address of clock is stored in hi_res_clock_time.
 * 
 *  - mmap_hi_res_clock(&ptr) implements system call #188 for
 *    i386 systems by mapping the mach 3.0 kernel's high-res
 *    clock into the virtual address space of the caller.
 * 
 *  - term_hi_res_clock() destroys the device pager managing
 *    the clock page, making it unavailable to osf1_ss and
 *    to osf1_ss users.  Not called in current implementation.
 * 
 * Revision 3.5  91/06/25  17:13:12  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.4  91/06/05  16:20:46  condict
 * Remove #ifndef OSF1_SERVER code.
 * 
 * Revision 3.3  91/05/07  15:56:33  condict
 * Change bad (old-interface) call to tsleep.
 * 
 * Revision 3.2  91/03/08  16:06:30  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/02/27  16:26:31  condict
 * Change Debugger to do breakpoint trap (i386) or task_suspend, for gdb.
 * 
 * Revision 3.0  91/01/17  12:06:10  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.12  91/08/12  22:38:21  rvb
 * 	Fixed share_unlock to not take unix_master.
 * 	[91/04/08            rpd]
 * 
 * Revision 2.11  90/09/09  14:13:54  rpd
 * 	In init_mapped_time, map the time device with VM_INHERIT_NONE
 * 	instead of VM_INHERIT_SHARE.
 * 	[90/09/09            rpd]
 * 
 * Revision 2.10  90/08/06  15:35:03  rwd
 * 	When using ptrace() there appears to be a way to get garbled
 * 	share locks.  Allow for this by cleaning when noticed.
 * 	[90/07/24            rwd]
 * 	Added share lock panic.
 * 	[90/07/05            rwd]
 * 	Added share_lock and share_unlock routines.
 * 	[90/06/08            rwd]
 * 
 * Revision 2.9  90/06/19  23:16:02  rpd
 * 	Added share_try_lock.
 * 	[90/06/09            rpd]
 * 
 * Revision 2.8  90/06/02  15:28:01  rpd
 * 	Must be on master before tsleep.
 * 	[90/04/04            rwd]
 * 	Removed mips conditionals in init_mapped_time.
 * 	[90/04/24            rpd]
 * 
 * 	Rewrote set_thread_priority using new priority calls.
 * 	[90/04/23            rpd]
 * 	Converted to new IPC.
 * 	Fixed bug in boot: don't try to use root if it isn't mounted.
 * 	[90/03/26  20:19:52  rpd]
 * 
 * Revision 2.7  90/05/29  20:25:16  rwd
 * 	Remove debugging printfs.
 * 	[90/05/10            rwd]
 * 	Must be on master before tsleep.
 * 	[90/04/04            rwd]
 * 
 * Revision 2.6  90/03/14  21:30:58  rwd
 * 	Add share_lock_solid and share_unlock_solid.
 * 	[90/02/16            rwd]
 * 	Include default_pager_object_user here.
 * 	[90/01/22            rwd]
 * 
 * Revision 2.5  89/11/29  15:31:32  af
 * 	No execute permission for mips, if one can avoid it.
 * 	[89/11/26  11:36:05  af]
 * 
 * Revision 2.4  89/09/26  10:30:27  rwd
 * 	Added Debugger
 * 	[89/09/20            rwd]
 * 
 * Revision 2.3  89/09/15  15:29:39  rwd
 * 	Make mapped time dependant on MAP_TIME
 * 	[89/09/13            rwd]
 * 	Change includes
 * 	[89/09/11            rwd]
 * 
 * Revision 2.2  89/08/09  14:46:09  rwd
 * 	Use sizeof(time_value_t) instead of NBPG since device_map and
 * 	vm_map are rounding up for us.
 * 	[89/08/09            rwd]
 * 	Fixed microtime to get time from mtime.
 * 	[89/08/08            rwd]
 * 	Added copyright to file by dbg.  Changed get_time to use mapped
 * 	time and be macro in sys/kernel.h.
 * 	[89/08/03            rwd]
 * 
 * $EndLog$
 */
/*
 * Miscellaneous routines.
 */
#include <map_time.h>
#include <map_uarea.h>
#include <mapped_files.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/buf.h>
#include <sys/mount.h>
#include <ufs/fs.h>
#include <sys/time.h>
#include <sys/reboot.h>
#include <sys/kernel.h>
#include <sys/synch.h>			/* for the spl macros */

#include <sys/vproc.h>

#include <uxkern/device.h>
#include <mach/port.h>
#include <mach/vm_prot.h>
#include <mach/vm_inherit.h>

#include <uxkern/device_utils.h>

extern mach_port_t	privileged_host_port;
extern mach_port_t	default_processor_set;

#ifdef	SECOND_SERVER
extern int	second_server;
#endif	/* SECOND_SERVER */

int real_time = 0;

#include "profiling.h"

#if PROFILING

#include <sys/proc.h>
#include <sys/user.h>
#include <mach/mach_port.h>
#include <mach/message.h>
#include <mach/prof_types.h>
#include <uxkern/proc_to_port.h>
#include <cthreads.h>
#include <machine/vmparam.h>

/*
 * Sysmap and Syssize must be declared
 * in the Server to ensure BSD kgmon
 * compatibility.
 */
int Sysmap;
int Syssize;

/*
 * Real-time support, when non zero will expand pri space
 */

extern	void 		ux_create_thread();
extern	struct proc 	*task_to_proc_lookup();

extern mach_port_t 	ux_profil_port_set;
extern mach_port_t      server_prof_port;
mach_port_t 		reply_port = MACH_PORT_NULL;
extern int		emulator_text_size;

#ifdef GPROF
extern struct uuprof server_prof_struct;
extern struct uuprof emulator_prof_struct;
#endif


/*
 ******************************************************************
 *
 *	C_addupc  :  performs updating of the PC samples collected
 *			from the micro-kernel according to the
 *			Unix standard algorithm required by gprof.
 *		pc    --- a PC sample (virtual address)
 *		uprof --- a per process structure containing all
 *				profiling informations
 *
 ******************************************************************
 */
#ifdef __GNUC__
__inline__
#endif
void	C_addupc(pc, uprof)
	unsigned int	pc;
	struct uuprof 	*uprof;
{
	unsigned int    pchigh, pclow;
	short		*addr;
/*---------------------------------------------------------*/
/*
 * NOTE :	All PCs out of range could be processed and
 *		stored apart, rather than discarded.
 *		This would be useful for library profiling.
 */
/*---------------------------------------------------------*/

#ifdef GPROF
	if (pc - EMULATOR_BASE < (unsigned) emulator_text_size)
		uprof = &emulator_prof_struct;
#endif
	if (uprof->pr_scale == 0)
	   return;

	pc -= uprof->pr_off;
	if (pc < 0) {
	   /* Underflow */
	   return;
	}

	pclow  = pc & 0xffff;
	pchigh = (pc >> 16) & 0xffff;

	pchigh *= uprof->pr_scale;
	pclow  *= uprof->pr_scale;
	pc = ((pclow >> 16) & 0xffff) + pchigh;

	if (pc >= uprof->pr_size) {
	   /* Overflow */
	   return;
	}

	addr = (uprof->pr_base + pc/sizeof(short));
	(*addr)++;
}


/*
 *************************************************************************
 *
 *	eat_buffers : 	performs the reception of all messages containing
 *			   PC samples sent from the microkernel. It then
 *			   finds out the identity of the Unix process on
 *			   whose account the samples were collected,
 *			   transforms the samples according to C_addupc
 *			   algorithm and stores the data in the process
 *			   buffer area (see the user and proc structures).
 *			Implemented by a Cthread.
 *
 *		arg --- meaningless, only for CThread Library compatibility
 *			reasons.
 *
 *************************************************************************
 */

void
eat_buffers(arg)
any_t 	arg;
{
    struct message {
	mach_msg_header_t	head;
	mach_msg_type_t 	type;
	sample_array_t		arg;
    } msg, outmsg;

    mach_msg_return_t  mr;

    /* wire it to make sure it is always here */
    cthread_set_name(cthread_self(), "eat_buffers");

    while (1) {
	/*
	 * Forever waiting for a message to receive and process.
	 */
	msg.head.msgh_local_port = ux_profil_port_set;
	msg.head.msgh_size = sizeof(struct message);

	mr = mach_msg_receive(&msg.head);
	if (mr != MACH_MSG_SUCCESS) {
	    printf("eat_buffers: msg_receive -> %d\n", mr);
	    break;
	}

	if (!ux_notify_server(&msg.head, &outmsg.head) &&
	    !prof_server(&msg.head, &outmsg.head)) {
	    printf("eat_buffers: unknown message type\n");
	    break;
	}
    }
}


kern_return_t
receive_samples(port, samples, sample_count)
mach_port_t port;
sample_array_t samples;
mach_msg_type_number_t sample_count;
{
    struct proc *p;
    int i;
    struct uuprof *uprof;
#ifdef GPROF
    if (port == server_prof_port) {
	/* Server execution samples */
	p = NULL;
	uprof = &server_prof_struct;
    }
    else
#endif
    {
	p = pport_to_proc_lookup(port);
	if (p == NULL)
	    panic("samples for unknown process");	/* DELETE PANIC? */
	uprof = &(p->p_utask.uu_prof);
    }

    /* If the scale is set to 0, the profiling status must be
     * disabled (thus we reject the samples), according to
     * the BSD profil() semantics.
     */
    if (uprof->pr_scale ||
	(p != NULL && (p->p_utask.uu_prof_on || p->p_taskprofed)))
	for (i = 0; i < sample_count; i++)
	    C_addupc(samples[i], uprof);
}


int
sample_done(port)
mach_port_t port;
{
    if (port == server_prof_port)
	return 1;
    else return pport_to_proc_wakeup(port);
}


/*
 ********************************************************************
 *
 *	profil_set :	Enables the profiling service for a Unix
 *		 	   user process. All initialization stuff
 *			   is done here.
 *			It is called by bsd_profil_set stub routine
 *			   therefore, all parameters are accessed via
 *			   the uthread structure.
 *
 ********************************************************************
 */

profil_set(p, args, retval)
struct proc *p;
void *args;
void *retval;
{
    uthread_t 	uth = &u;
    int	        *arg=args;
    task_t		cur_task;
    thread_t	cur_thread;

    kern_return_t	kr;
    int	i;


    if (uth->u_prof_on == FALSE) {
	/* Enable profiling of the process. */
	uth->u_prof_on = TRUE;

	/*
	 * Store user address and other necessary information.
	 */
	uth->u_prof_buf     = (short *)  arg[0];
	uth->u_prof.pr_size = (unsigned) arg[1];
	uth->u_prof.pr_off  = (unsigned) arg[2];
	uth->u_prof.pr_scale= (unsigned) arg[3];


	/*
	 * Allocate per process the Server internal
	 *    profiling buffer.
	 */
	if ((uth->u_prof.pr_base = (short *) malloc(uth->u_prof.pr_size))
	    == (short *) NULL) {
	    printf("profil_set: cannot allocate internal buffer\n");
	    uth->u_prof_on = FALSE;
	    return ENOBUFS;	/* XXX: more meaningful?  and cleanup. */
	}


	/* Initialize counters to zero */

	{
	    int j= uth->u_prof.pr_size/sizeof(short);
	    short *ptr= (short *)uth->u_prof.pr_base;
	    for(i=0; i<j;ptr[i++]=0) ;
	}

	/*
	 * Allocate the profiling port and register
	 * the proc structure of the user process.
	 * If we are already profiling another thread,
	 * or we are doing emulator profiling, we may
	 * already have a port.
	 */
	if (p->p_profport != MACH_PORT_NULL)
	    reply_port = p->p_profport;
	else {
	    reply_port = pport_to_proc_enter(p);
	    p->p_profport = reply_port;
	}

	/*
	 * Find out the identities of the thread and task
	 * which implement the current process.
	 */
	cur_task = p->p_task;
	cur_thread = p->p_thread;


	/*
	 * Enable the sampling service at micro-kernel level.
	 */
	kr = thread_sample((mach_port_t) cur_thread, reply_port);
	if (kr != KERN_SUCCESS) {
	    if (kr == MIG_BAD_ID)
		printf("profil: microkernel does not support profiling\n");
	    else printf("profil: thread_sample -> %d\n", kr);
	} else {
	    mach_port_t old_notify_port;
	    kr = mach_port_request_notification(mach_task_self(),
		   reply_port, MACH_NOTIFY_NO_SENDERS,
		   (mach_port_mscount_t) 0, reply_port,
		   MACH_MSG_TYPE_MAKE_SEND_ONCE, &old_notify_port);
	    if (kr != KERN_SUCCESS)
		printf("profil: request_notify -> %d\n", kr);
	}
	if (kr != KERN_SUCCESS) {
	    free(uth->u_prof.pr_base, uth->u_prof.pr_size);
	    uth->u_prof_on = FALSE;
	    /* Reply port will be removed on process exit. */
	    nosys();
	}
    }
    /* else ignore it */
    return 0;
}

/*
 ********************************************************************
 *
 *	profil_get :	Disables the profiling service for a Unix
 *		 	   user process. Cleaning up is done here.
 *			The buffer containing data related to the
 *			   profiled execution is dumped out in the
 * 			   user process virtual address space.
 *			It is called by bsd_profil_get stub routine
 *			   therefore, all parameters are accessed via
 *			   the uthread structure.
 *
 ********************************************************************
 */
profil_get(p, args, retval)
	struct proc *p;
	void *args;
	void *retval;
{
	uthread_t uth = &u;
	task_t		cur_task;
	thread_t	cur_thread;
	char		*out_addr;
	int		out_count;
	int		error = 0;


	if (uth->u_prof_on == TRUE) {
	   /* Disable profiling of the process */
	   uth->u_prof_on = FALSE;

           /* Make a syscall to disable the sampling service in
	    * the micro-kernel.
            * The value of the 'reply_port' parameter is set to
	    * MACH_PORT_NULL to indicate that sampling must be
	    * disabled.
            */
	   cur_task = p->p_task;
	   cur_thread = p->p_thread;

	   /*
	    *	Disable the sampling service at micro-kernel level.
	    */
  	   thread_sample((mach_port_t) cur_thread, MACH_PORT_NULL);

           /* This call will tell the micro-kernel to send the last buffer.
            * As soon as the receiver (eat_buffer) receives it and
            * makes the last updates to the internal buffer, it will
            * wake us up.
	    * Data will be ready to be copied out.
            */

	   pport_to_proc_sleep(p->p_profport);


	   /* This disables the profiling of the process:
	    * samples collected are dumped.
	    */

	   out_addr = *(char **)args;
	   out_count = uth->u_prof.pr_size;   /* size in chars */

	   error = copyout( (caddr_t) uth->u_prof.pr_base,
			    (caddr_t) out_addr,
			    out_count);
	   if (error)
	      printf("Profil: can't write address %x kr=%d\n",
						(int)out_count,error);

           /* Free the internal buffers here */
           free( uth->u_prof.pr_base, uth->u_prof.pr_size );

	}
	/* else ignore it */
	return error;
}

#else	/* PROFILING */

profil_set()
{
	nosys();
}

profil_get()
{
	nosys();
}

#endif	/* PROFILING */

/* Defining profil() seems to be necessary because of the way tables are
   generated from syscalls.master. */
profil()
{
	nosys();
}

/*
 * Missing system calls
 */

resuba()
{
}

/*
 * New kernel interfaces.
 */

#if MAP_TIME
time_value_t *mtime = NULL;

init_mapped_time()
{
	kern_return_t rc;
	mach_port_t device_port, pager = MACH_PORT_NULL;

	rc = device_open(device_server_port,0,"time",&device_port);
	if (rc != D_SUCCESS) panic("unable to open device time");

	rc = device_map(device_port, VM_PROT_READ,
			0, sizeof(time_value_t), &pager, 0);
	if (rc != D_SUCCESS) panic("unable to map device time");
	if (pager == MACH_PORT_NULL) panic("unable to map device time");
	
	rc = vm_map(mach_task_self(), (vm_address_t *)&mtime,
		    (vm_size_t)sizeof(time_value_t), 0, TRUE,
		    pager, 0, 0, VM_PROT_READ, 
		    VM_PROT_READ, VM_INHERIT_NONE);
	if (rc != D_SUCCESS) panic("unable to vm_map device time");

	rc = mach_port_deallocate(mach_task_self(), pager);
	if (rc != KERN_SUCCESS) panic("unable to deallocate pager");
}

#else MAP_TIME
init_mapped_time(){}

/*
 * Note: caller must hold TIME LOCKS:
 */
get_mach_time()
{
	time_value_t	time_value;

	kern_timestamp(&time_value);
	time->tv_sec = time_value.seconds;
	time->tv_usec = time_value.microseconds;
}

#endif MAP_TIME

#ifdef MP_SLAVE_AS_CLK

/*
 *
 *  In init_hi_res_clock(), a pseudo-device is created to permit the
 *  high_resolution clock to be mapped into the single server's address
 *  space.  Rather than deallocating the pseudo-device's pager once the
 *  single server can access the clock, retain the pager, and use it
 *  to service requests from other tasks to access the clock.  Add a
 *  call mmap_hi_res_clock(&clock_pointer) to permit other tasks
 *  to receive the clock, and term_hi_res_clock() to deallocate the
 *  pseudo-device pager.
 *
 */

int *hi_res_clock_time = NULL;

mach_port_t     hi_res_clk_device_port, hi_res_clk_pager = MACH_PORT_NULL;

init_hi_res_clock()
{
    kern_return_t   rc;
    int             unmap_unused;

    rc = device_open(device_server_port,0, "hi_res_clock",&hi_res_clk_device_port);
    switch (rc) {
    case D_SUCCESS:
         break;
    case D_INVALID_OPERATION:
         panic("init_hi_res_clock: device_server_port is not the master device port.");
    case D_WOULD_BLOCK:
         panic("init_hi_res_clock: hi_res_clock is busy, but D_NOWAIT was specified in mode 0.");
    case D_ALREADY_OPEN:
         panic("init_hi_res_clock: hi_res_clock is already open in a manner incompatible with mode 0.");
    case D_NO_SUCH_DEVICE:
         panic("init_hi_res_clock: hi_res_clock is not the name of any known device.");
    case D_DEVICE_DOWN:
         panic("init_hi_res_clock: hi_res_clock has been shut down.");
    default:
         panic("init_hi_res_clock: unable to open device hi_res_clock, device_open ret val = %d",rc);
    }

    rc = device_map(hi_res_clk_device_port, VM_PROT_READ,
                    0, sizeof(int), &hi_res_clk_pager, unmap_unused);
    switch (rc) {
    case KERN_SUCCESS:
         break;
    case D_NO_SUCH_DEVICE:
         panic("init_hi_res_clock: hi_res_clock is not open or operational.");
    default:
         panic("init_hi_res_clock: unable to mmap device hi_res_clock, device_map ret val = %d",rc);
    }
    if (hi_res_clk_pager == MACH_PORT_NULL) panic("init_hi_res_clock: device_map returned null pager port!");

    rc = vm_map(mach_task_self(), &hi_res_clock_time, sizeof(int), 0, TRUE,
                hi_res_clk_pager, 0, 0, VM_PROT_READ,
                VM_PROT_READ, VM_INHERIT_NONE);
    switch (rc) {
    case KERN_SUCCESS:
         break;
    case KERN_NO_SPACE:
         panic("init_hi_res_clock: not enough space in the server's VAS for vm_map to alloc new region.");
    case KERN_INVALID_ARGUMENT:
         panic("init_hi_res_clock: an illegal arguement supplied to vm_map.");
    default:
         panic("init_hi_res_clock: unable to mmap device hi_res_clock, vm_map ret val = %d",rc);
    }

    return(0);
}

/*
 *
 *  return location of high-resolution clock in caller's address space [z1000, BSD SS version]
 *
 */
#include <sys/user.h>
#include <sys/proc.h>

mmap_hi_res_clock(p, args, retval)
         struct proc *p;
         void *args;
         int  *retval;

{
    vm_task_t       client_task;
    kern_return_t   rc;
    vm_address_t    client_offset;

    if (hi_res_clk_pager == MACH_PORT_NULL) {
       *retval = -1;
       return (-1);
    }

    else {

       /* according to Randy, the send rights to the user's task are already
          held in the single server at u.u_procp->p_task.  There is no need
          for the user to have to pass this information back to the single server */

       client_task = u.u_procp->p_task;

       rc = vm_map(client_task , &client_offset, sizeof(int), 0, TRUE,
                           hi_res_clk_pager, 0, 0, VM_PROT_READ,
                           VM_PROT_READ, VM_INHERIT_NONE);

       if (rc == KERN_SUCCESS)
           *retval = client_offset;
       else
           *retval = rc;
    }

    return (0);
}

term_hi_res_clock()

{
    kern_return_t   rc;

    if (hi_res_clk_pager == MACH_PORT_NULL)
       return (-1);

    rc = mach_port_deallocate(mach_task_self(), hi_res_clk_pager);
    switch (rc) {
    case KERN_SUCCESS:
         break;
    case KERN_INVALID_TASK:
         panic("term_hi_res_clock: could not deallocate pager: task parameter invalid.");
    case KERN_INVALID_NAME:
         panic("term_hi_res_clock: could not deallocate pager: name did not denote a right.");
    case KERN_INVALID_RIGHT:
         panic("term_hi_res_clock: could not deallocate pager: name denoted an invalid right.");
    default:
         panic("term_hi_res_clock: could not deallocate pager: mach_port_deallocate rc= %d",rc);
    }
}




#endif MP_SLAVE_AS_CLK


set_time(tvp)
	struct timeval *tvp;
{
	time_value_t	time_value;

	time_value.seconds = tvp->tv_sec;
	time_value.microseconds = tvp->tv_usec;

	(void) host_set_time(privileged_host_port, time_value);
}

/* Must be called from inside of a TIME_LOCK: */
resettodr()
{

#if defined(__i860__) && defined(TNC)

/*
 * If there is an RPM, we calculate the offset for the new time of day, but
 * we can't attempt to send it to other nodes yet, because we don't want to
 * RPC while we hold the time lock. Therefore, after we return to our caller
 * and the lock is released, rpm_transmitt_offset has to be called to send
 * the new offset to all the nodes in the cluster.
 */
	if(check_for_rpm()){
		rpm_set_time(&time);
	}
#endif /* defined(__i860__) && defined(TNC) */

	set_time(&time);

}

/*
 * Return the best possible estimate of the time in the timeval
 * to which tvp points.  We do this by reading the interval count
 * register to determine the time remaining to the next clock tick.
 * We must compensate for wraparound which is not yet reflected in the time
 * (which happens when the ICR hits 0 and wraps after the splhigh(),
 * but before the mfpr(ICR)).  Also check that this time is no less than
 * any previously-reported time, which could happen around the time
 * of a clock adjustment.  Just for fun, we guarantee that the time
 * will be greater than the value obtained by a previous call.
 */
microtime(tvp)
	register struct timeval *tvp;
{
	int s = splhigh();
	static struct timeval lasttime;

	TIME_READ_LOCK();
	*tvp = time;
	if (tvp->tv_sec == lasttime.tv_sec &&
	    tvp->tv_usec <= lasttime.tv_usec &&
	    (tvp->tv_usec = lasttime.tv_usec + 1) >= 1000000) {
		tvp->tv_sec++;
		tvp->tv_usec -= 1000000;
	}
	lasttime = *tvp;
	TIME_READ_UNLOCK();
	splx(s);
}

/*
 * This is the microtime routine without guaranteed unique timestamps.
 * It's used where we simply want the time of day, or where the usecs
 * are discarded.
 */
raw_microtime(tvp)
	register struct timeval *tvp;
{
	int s = splhigh();
	TIME_READ_LOCK();
	*tvp = time;
	TIME_READ_UNLOCK();
	splx(s);
}

struct node_exist_list_entry {
	node_t				node;
	struct node_exist_list_entry	*next;
};

struct 	node_exist_list_entry	*node_exist_list_ptr;
zone_t	node_exist_list_zone;
struct	mutex			node_exist_list_lock;

node_exist_list_setup()
{
	struct 	node_exist_list_entry	*nep;
	/* initialize list and mutex */
	mutex_init(&node_exist_list_lock);
	node_exist_list_zone = zinit(sizeof(struct node_exist_list_entry),
                                vm_page_size, 0, "node_exist_list_entry");
	ZALLOC(node_exist_list_zone, nep, struct node_exist_list_entry *);
	if (nep == NULL)
		panic("node_exist_list_setup: No more entries\n");
	nep->node = this_node;
	nep->next = NULL;
	node_exist_list_ptr = nep;
}

server_register(node)
	node_t	node;
{
	struct	node_exist_list_entry        *nep;

	mutex_lock(&node_exist_list_lock);
	ZALLOC(node_exist_list_zone, nep, struct node_exist_list_entry *);
        if (nep == NULL)
                panic("server_register: No more entries\n");
	
	nep->node = node;
	nep->next = node_exist_list_ptr->next;
	node_exist_list_ptr->next = nep;
	mutex_unlock(&node_exist_list_lock);
}

int	waittime = -1;

Debugger()
{
#ifdef	SECOND_SERVER
	if (second_server) {
		printf("Debugger (suspending server pid=%d)\n",
			second_getpid());
		task_suspend(mach_task_self());	/* wait for debugger */
		return;
	}
#endif	/* SECOND_SERVER */
#if defined(__i860__)
	pps_boot(RB_DEBUGGER, RB_NOSYNC | RB_DEBUGGER);
#else
	printf("Debugger (suspending server)\n");
	task_suspend(mach_task_self());		/* wait for kernel debugger */
	return;
#endif

}

extern node_t this_node, root_fs_node;

pps_bootsync(paniced, flags)
	int	paniced;
	int	flags;
{

	printf("Reboot()ing.\n");
	if ((this_node == root_fs_node) && (paniced != RB_PANIC)) {
#if MAPPED_FILES
		if ((flags & RB_NOSYNC) == 0) {
			/*
			 * calls mntflushbuf and vflush for each mounted file
			 * system to get mapped files sync'ed out to disk
			 */
			vflushall();
		}
#endif
		if (((flags & RB_NOSYNC) == 0) && (waittime < 0) &&
		    (bfreelist[0].b_forw != 0)) {
		    int syncarg = -1;

		    waittime = 0;
		    (void) splnet();
		    printf("syncing disks... ");

		    sync(NULL, &syncarg, NULL);

		    {
			register struct buf	*bp;
			int	iter, nbusy, obusy;

			obusy = 0;
#if	__i860__
			/*
			 * DELAY(1) == approx. 4 microseconds.
			 * DELAY(40000) == approx. 0.16 seconds
			 * timings done using the Paragon dclock() function.
			 * 50 iterations == approx. 3 mins 16 secs.
			 */
#endif	/* __i860__ */
			for (iter = 0; iter < 50; iter++) {
			    nbusy = 0;
			    for (bp = &buf[nbuf]; --bp >= buf; )
				if ((bp->b_flags & (B_BUSY|B_INVAL)) == B_BUSY)
				    nbusy++;

			    if (nbusy == 0) {
		    		printf("0 done.\n");
				return;
			    }
			    printf("%d ", nbusy);

			    /*
			     * Same number of buffers busy?
			     * If not then reset the wait count to wait at
			     * least one wait-period.
			     */
			    if (nbusy != obusy)
				iter = 1;

			    obusy = nbusy;

			    DELAY(40000 * iter);
			}
                	panic("pps_bootsync: Buffers stuck in memory\n");
		    }
		}
	}
}

pps_boot(paniced, flags)
	int	paniced;
	int	flags;
{
#ifndef	TNC
	struct node_exist_list_entry *nep;
#endif

#ifdef	TNC
	if ((this_node != root_fs_node) && (paniced != RB_PANIC))
		printf("Rebooting at request of root_fs_node\n");
#else	/* TNC */
	if ((this_node == root_fs_node) && (paniced != RB_PANIC)) {
		/* halt other nodes */
		mutex_lock(&node_exist_list_lock);
		nep = node_exist_list_ptr;
		while (nep != NULL) {
			if (nep->node != this_node)
				remote_server_halt(nep->node, flags);
			nep = nep->next;
		}
		mutex_unlock(&node_exist_list_lock);
	}
#endif	/* TNC */

#ifdef	SECOND_SERVER
	if (second_server) {
		if (paniced == RB_BOOT) {
			second_cons_restore();
			second__exit();
		} else {
			Debugger();
		}
		return;
	}
#endif	/* SECOND_SERVER */

	(void) host_reboot(privileged_host_port, flags);
}

thread_read_times(thread, utv, stv)
	thread_t	thread;
	time_value_t	*utv;
	time_value_t	*stv;
{
	struct thread_basic_info	bi;
	unsigned int			bi_count;

	bi_count = THREAD_BASIC_INFO_COUNT;
	(void) thread_info(thread,
			   THREAD_BASIC_INFO,
			   (thread_info_t)&bi,
			   &bi_count);

	*utv = bi.user_time;
	*stv = bi.system_time;
}

/*
 *	Priorities run from 0 (high) to 31 (low).
 *	The user base priority is 12.
 *	priority = 12 + nice / 2.
 */

set_thread_priority(thread, pri)
	thread_t	thread;
	int		pri;
{
	if (real_time) {
		pri += 54;
	}
	(void) thread_max_priority(thread, default_processor_set, pri);
	(void) thread_priority(thread, pri, FALSE);
}

/*
 * Get the PC register for the specified thread, using a machine-independent
 * interface:
 */
kern_return_t
get_thread_pc(thread, pc)
	thread_t	thread;
	int 		*pc;
{
	thread_state_data_t	thread_state;

	return get_thread_state_pc(thread, pc, (thread_state_t)&thread_state);
}

#if MAP_UAREA
#include <sys/user.h>
#include <kern/parallel.h>
#define BACKOFF_SECS 5
#define SHARED_PRIORITY PSPECL

extern int hz;

boolean_t 
share_try_lock(p, lock)
	register struct proc *p;
	register struct shared_lock *lock;
{
	if (spin_try_lock(&lock->lock)) {
		if (p->p_shared_rw->us_inuse)
			lock->who = (int)p | KERNEL_USER;
		u.uu_share_lock_count++;
		return TRUE;
	} else
		return FALSE;
}

void 
share_lock(x, p)
	register struct shared_lock *x;
	register struct proc *p;
{
	if (p->p_shared_rw->us_inuse) {
		while (!spin_try_lock(&(x)->lock) && !share_lock_solid(x,p));
		(x)->who = (int)(p) | KERNEL_USER;
	} else {
		spin_lock(&(x)->lock);
	}
	u.uu_share_lock_count++;
}

void
share_unlock(x, p)
	register struct shared_lock *x;
	register struct proc *p;
{
	if (p->p_shared_rw->us_inuse) {
		(x)->who = 0;
		spin_unlock(&(x)->lock);
		if ((x)->need_wakeup) {
			mutex_lock(&p->p_shared_mutex);
			x->timed_out = 0;
			condition_signal(&x->lock_is_free);
			mutex_unlock(&p->p_shared_mutex);
		}
	} else {
		spin_unlock(&(x)->lock);
	}
	if (u.uu_share_lock_count-- == 0) {
		panic("share_unlock < 0");
		u.uu_share_lock_count = 0;
	}
}

/* Lock held too long: wake up all the threads sleeping in share_lock_solid: */
int
share_lock_timeout(x, p)
	register struct shared_lock *x;
	register struct proc *p;
{
	mutex_lock(&p->p_shared_mutex);
	x->timed_out = 1;
	condition_broadcast(&x->lock_is_free);
	mutex_unlock(&p->p_shared_mutex);
}

int
share_lock_solid(x, p)
	register struct shared_lock *x;
	register struct proc *p;
{
	int timeout_id;
	mutex_lock(&p->p_shared_mutex);

	if (!x->timed_out) {
		if (spin_try_lock(&x->lock)) {
			mutex_unlock(&p->p_shared_mutex);
			return 1;
		}
/*		printf("[%x]Share_lock_solid %x\n",(int)x, x->who);*/
		x->timed_out = 0;	/* Will be set by share_lock_timeout */

		/* Set timeout for the condition_wait: */
		timeout_id = timeout_special(share_lock_timeout,
						x, BACKOFF_SECS * hz, 1);
		x->need_wakeup++;
		/*
		 * Avoid a race condition with the emulator's
		 * share_unlock(): the emulator might have already
		 * released the lock before we incremented need_wakeup,
		 * so don't condition_wait() in this case: the emulator
		 * hasn't called bsd_share_wakeup().
		 */
		if (spin_try_lock(&x->lock)) {
			/* printf("shared_lock_solid(0x%x): avoided race condition with emulator.\n", (int) x); */
			x->need_wakeup--;
			untimeout_special(timeout_id);
			mutex_unlock(&p->p_shared_mutex);
			return 1;
		}
		condition_wait(&x->lock_is_free, &p->p_shared_mutex);
		x->need_wakeup--;
		untimeout_special(timeout_id);
	}

	if (x->timed_out) {
		if ((x->who & ~KERNEL_USER) >= (int)proc &&
				    (x->who & ~KERNEL_USER) <= (int)procNPROC)
		    printf("[%x]Taking timed out share lock from %x\n",
								(int)x, x->who);
		else
		    printf("[%x]Taking scribbled share_lock (who=%x)\n",
								(int)x, x->who);

		/* Wake up anyone else sleeping on the lock: */
		condition_broadcast(&x->lock_is_free);

		/* Reset it to a valid state: */
		share_lock_init(x);
	}

/*	printf("[%x]Share_lock_solid\n",(int)x);*/
	mutex_unlock(&p->p_shared_mutex);
	return 0;
}

#endif MAP_UAREA

#include <mach/default_pager_object_user.c>
