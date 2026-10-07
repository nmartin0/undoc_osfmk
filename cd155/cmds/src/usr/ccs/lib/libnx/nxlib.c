/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *			  INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	  Copyright 1992  Intel Corporation.
 *
 * $Id: nxlib.c,v 1.110 1995/03/10 23:11:38 joel Exp $
 *
 * HISTORY
 * $Log: nxlib.c,v $
 * Revision 1.110  1995/03/10  23:11:38  joel
 *  Reviewer: 	sean, tnt
 *  Risk:		low
 *  Benefit or PTS #: 12646  message passing in hrecv() handler with -plk
 * 		     	  fails with "Not enough space"
 * 		the library was trying to wire down all memory between
 * 		the main thread stack and handler thread stack due to
 *    		the local variables in the handler, fixed by adding locking
 * 		code for the handler stack.
 *  Testing:	developer test case, handlerproc test, message EATs,
 * 		parallel SATs
 *  Module(s):	nxlib.c and nx_port.c
 *
 * Revision 1.109  1995/01/27  21:32:38  terry
 * This fixes the problem in _csend() where during broadcasts, there is
 * potential for the routine to fail and csend() to think it completed correctly.
 *
 *
 *  Reviewer: tnt,sean
 *  Risk: Low
 *  Benefit or PTS #: 12270
 *  Testing: ran test case for gcolx() which detected the failure in the first
 * 		  place.
 *  Module(s): nxlib.c
 *
 * Revision 1.108  1995/01/20  18:25:10  joel
 *  Reviewer:      sean
 *  Risk:          low
 *  Benefit or PTS #: 11968 sender exits before recv_continue (new error message
 * 		    defined here).
 *  Testing:       testcase, message EATs
 *  Module(s): msgp_inq.c msgp_nxdat.c msgp_atdt.c mcmsg_nx.h nxlib.c
 *             libc/errlst.c  server/sys/errno.h
 *
 * Revision 1.107  1994/11/29  19:54:37  tnt
 * PTS #: 11639
 * Mandatory?:  No (H1)
 * Description: During a fork() the memory allocated for the stacks
 *              of the NX threads was not freed.  This memory space is
 *              now reclaimed during the start-up of the forked child.
 *              Two versions of forknx.o now exist, one for libnx.a and
 *              one for libc.a.  The libc.a version is as it was in R1.2.
 * Reviewer(s): Shane Story
 * Risk:        Medium
 * Testing:     Developer tests, Message Passing, Controlc, Rmcall,
 *              and Misc. EATs.  Also, built libc.a and tested running
 *              non-NX applications on service node; booted from ramdisk
 *              and ran non-NX applications on boot node.
 * Module(s):   usr/ccs/lib/common_nx_c/forknx.c
 *              usr/ccs/lib/libnx/nxlib.c
 *              usr/ccs/lib/libnx/Makefile
 *
 * Revision 1.106  1994/11/19  02:32:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.105  1994/11/08  22:17:38  joel
 *  Reviewer: tnt
 *  Risk: low
 *  Benefit or PTS #: changes in _msgcancel to fix PTS 11484, changes in
 * 	csend, crecv, and crecvx to fix PTS 10555
 *  Testing: PTS test cases, lat, message EATs
 *  Module(s): nxlib.c
 *
 * Revision 1.104  1994/10/18  00:22:53  joel
 *  Reviewer: emeron
 *  Risk: low
 *  Benefit or PTS #: 11289
 *  Testing: recvcancel EAT
 *  Module(s): msgp_nxdat.c nxlib.c
 *
 * Revision 1.103  1994/10/05  18:35:55  joel
 *  Reviewer: tnt
 *  Risk: low
 *  Benefit or PTS #: 3715
 *  Testing: bug test case, developer tests, TCAT, Message EATs
 *  Module(s): nxlib.c msgp_nxdat.c
 *         This fix makes msgcancel() remove send requests from the
 *         mt->send_wait_unk list so canceled message no longer block
 *         PNQ/PAK protocol.
 *
 * cVS: ----------------------------------------------------------------------
 *
 * Revision 1.102  1994/08/31  20:22:41  bradf
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
 * Revision 1.100.2.1  1994/08/03  17:21:45  tnt
 *    PTS #: 9901
 *    Mandatory?:  Yes
 *    Description: Fix problem with original changes.  Variables were declared from
 *                 within libnx/nxlib.c.  Now they are declared in
 *                 common_nx_c/forknx.c so that they are resolved for apps that
 *                 don't link with libnx.a.
 *    Reviewer(s): None
 *    Risk:        Low
 *    Module(s):   usr/ccs/lib/common_nx_c/forknx.c, libnx/nxlib.c
 *    Testing:     Message EATs.  Developer tests linked with libc.a, but not
 *                 libnx.a
 *
 * Revision 1.100  1994/07/14  21:46:03  shala
 *  Reviewer: NONE
 *  Risk: Low
 *  Benefit or PTS #: Extra file
 *  Testing: Build nxlib.o object.
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnx/nxlib.c
 *             cmds_libs/src/usr/ccs/lib/libnx/Makefile
 *
 * Removed nxconcur.o since R5.0 compiler does not need it anymore.
 * _mp_init() is handled differently.
 *
 * Revision 1.99  1994/06/21  22:59:58  tnt
 *  Reviewers: Terry Prickett, Greg Regnier
 *  Risk:  Low
 *  Benefit or PTS #: 7605; Consolidated the pthread specific code in nx_port.c
 *                    so that only the nx_port.o object need be included in
 *                    libc_r.a for pthread nx threads.
 *
 *  Testing: Message Passing EATs, TAMU ipx code.
 *  Module(s): nxlib.c, nx_port.c, libc_r/Makefile, libpthreads/pthread.c
 *
 * Moved masktrap() into nx_port.c.
 *
 * Revision 1.98  1994/06/16  17:54:58  tnt
 *  Reviewers: Greg Regnier, Terry Prickett
 *  Risk:  Low
 *  Benefit or PTS #: 7605; Create the hrecv/hsend and vm paging/receive-continue
 *                    threads as pthreads in a pthread application.  When linking
 *                    with libpthreads and libc_r, the nxlib.o and nx_port.o
 *                    objects will be retrieved from libc_r.  The libnx.a objects
 *                    are unchanged and still create mach threads.
 *  Testing: Message Passing EATs, TAMU ipx code, lat, lath.
 *  Module(s): nxlib.c, nx_port.c
 *
 * Thread id checks now done for mach threads or pthreads, depending on
 * object version.
 *
 * Revision 1.97  1994/06/10  17:08:08  terry
 * When msgmerge() is called with mid's or merged mid`s with the same
 * mid on two list and error is now return.
 *
 *  Reviewer: joel, regnier
 *  Risk: low
 *  Benefit or PTS #: 7647
 *  Testing: ran test case
 *  Module(s): nxlib.c
 *
 * Revision 1.96  1994/06/08  15:29:29  ellend
 *  Reviewer: Terry
 *  Risk: Low
 *  Benefit or PTS #: Fixed type definitions to match manpages and calling
 * 				   routines in various recv calls. Also fixes 7517
 *  Testing: Message passing, miscellaneous EATs
 *  Module(s): ../usr/ccs/lib/libnx.c
 *
 * Revision 1.95  1994/06/06  21:10:01  regnier
 * Fine tune use of cmid, rearrange for better i-cache behavior. Clean up
 * old receive continue code. Fix potential bug where info was being
 * copied for a csend.
 *
 *  Reviewer: tnt, terry
 *  Risk: Low
 *  Benefit: Latency tuning.
 *  Testing: Message EATs, irxnode, lat
 *  Module(s): nxlib.c
 *
 * Revision 1.94  1994/06/03  00:24:20  sean
 *  Reviewer:  shala
 *  Risk: 	low
 *  Benefit or PTS #:
 * 	setptype() now can be compiled to call _mp_init() which is
 * 	in the libmp3.  If nxlib.c is compiled with -DCONCUR, then
 * 	that version can be put in libmp3 by the compiler people
 * 	and linked so that the version of setptype() with _mp_init()
 * 	supercedes the other version.  This is how the application
 * 	gives the runtime library a chance to create enough threads
 * 	to make all of the processors run.
 *
 *  Testing:  gregt convinced himself that it does what it should.
 *  Module(s):
 * 	cmds_libs/src/usr/ccs/lib/libnx/nxlib.c
 *
 * Revision 1.93  1994/05/05  23:18:20  terry
 * This fixes a problem with global send and csend within an hrecv handler.
 *
 *  Reviewer: tnt and regnier
 *  Risk: medium
 *  Benefit or PTS #: 9225
 *  Testing: Parallel sats, misc and message passing eats and testcase
 *  Module(s): nxlib.c
 *
 * Revision 1.92  1994/02/17  00:52:04  tnt
 *  Reviewers: Greg Regnier, Terry Prickett
 *  Risk: Low
 *  Benefit or PTS #: 7736; Disable ability to reset a process' ptype.
 *  Testing: Message Passing EATs, Rmcall EATs, developer testing.
 *  Modules: nxlib.c, nx_nfork.c
 *
 * Since mcmsg_ptype can only be set once, the value is initialized to
 * INVALID_PTYPE in a forked child, and then checked below in _setptype().  The
 * mcmsg_initialized global has been removed, as it's only purpose was to
 * prevent re-executing the initialization code in _setptype(), when
 * resetting the ptype was a valid operation.
 * The code that handled the case of resetting the ptype has been
 * ifdef'd out for now.
 *
 * Revision 1.88.2.3  1994/02/02  19:19:53  tnt
 *  Reviewers: Greg Regnier, Terry Prickett
 *  Risk: Medium
 *  Benefit or PTS #: 7785,7241; Main thread no longer suspended by hrecv thread.
 *                    The two threads now run independently, except for
 *                    synchronization via the nx and masktrap locks.
 *  Testing: Message Passing, Pthreads, Controlc, Unix, and Fileio EATs,
 *           Parallel SATs (except pfs, slab, cg), irxnode, TAMU.
 *  Modules: nx_port.c, nxlib.c
 *
 * In masktrap(), check to see if caller is the hrecv thread.  If it is
 * don't attempt to grab the lock, since the hrecv thread will already
 * possess the lock.
 *
 * Revision 1.88.2.2  1993/12/22  00:32:24  tnt
 * Reviewer: Tim Teitenberg, fix by Paul Pierce
 * Risk: Medium
 * Benefit or PTS #: 7574. Might also fix app. slowdowns with -plk.
 * Testing: irxnode, Message Passing EATs
 * Module: nxlib.c
 *
 * An xmsg may be attached to a request when it completes normally.
 * Therefore, it's necessary to be able to throw away an xmsg
 * both in _probe_nxreq and in _fulfill_nx_xmsg. I split out the code for
 * tossing an xmsg from _fulfill_nx_xmsg and put it in (new) _nx_free, then
 * added a call to it in _probe_nxreq. To tell whether there is an xmsg, it
 * checks nxreq->xmsg which must be zero by default.  As far as I could tell
 * that was true already as long as it gets cleared again when the xmsg is
 * freed.
 *
 * Revision 1.88.2.1  1993/12/17  23:18:44  tnt
 *  Reviewer: Terry Prickett, Greg Regnier
 *  Risk: Low
 *  Benefit or PTS #: 7519; Created unique spin lock for masktrap so that the
 *                    instrumentation tools may call masktrap within the NX library.
 *  Testing: Message Passing EATs
 *  Modules: nxlib.c, nx_lock.c, nx_port.c, mcmsg_lib.h
 *
 * Created a separate lock for the critical sections within _masktrap().  This
 * was necessary because the tools, when used to instrument the nx library
 * functions, call _masktrap() after they have the nx_lock.
 *
 * Revision 1.88  1993/11/18  19:25:17  dleslie
 *  Reviewer: shala
 *  Risk: low
 *  Benefit or PTS #: get nx and mcmsg headers out of export tree, not obj
 * 	tree.  This allows users to build without having an obj tree fully
 * 	populated with headers
 *  Testing: built libnx
 *  Module(s):
 *     Makefile _gcol.c _gcolx.c _gops.c _gsync.c _load.c allocUser.c
 *     allocsys.c allocsys_.c autoinit.c bitmap.c bitmap2.c create.c
 *     nodeparser.c nx_initve.c nx_load.c nx_load_.c nx_loadve.c
 *     nx_loadve_.c nx_lock.c nx_part_ops.c nx_part_ops_.c nx_port.c
 *     nx_pri.c nxlib.c parsepart.c parsesched.c partlock.c
 *     partprint.c partutils.c rkassert.c rklib.c rkmem.c utils.c
 *     writepart.c
 *
 *
 * VS:    writepart.c
 *
 * Revision 1.87  1993/11/04  17:47:23  shala
 *  Reviewer: regnier
 *  Risk: low
 *  Benefit or PTS #: Bug #7049.
 *  Testing: Build nxlib.c and setiomode.c with no errors.
 *  Module(s): nxlib.c, nx.h, setiomode.c.
 *
 * Made message passing calls definitions in nx.h to match the man pages
 * for those calls. In order to compile nxlib.c, remove include file
 * "nx.h" from this file and defined expected definitions locally.
 *
 * Revision 1.86  1993/10/26  16:40:34  shala
 * Fixed problem with redeclarations of some of the functions in order
 * to build with R4.5 compiler. Here is the list of those functions:
 * _crecvx(), _crecv(), _irecv(), _irecvx(), _masktrap(), _hrecvx(), _hrecv(),
 * _msgdone(), _msgcancel(). Also many other function declarations got fixed in
 * usr/include/nx.h.
 *
 * Revision 1.85  1993/10/14  17:39:12  tnt
 * Changes to support the new receive continue functionality.
 *
 * Revision 1.84  1993/09/29  17:28:13  shala
 * Removed forknx(). This now a file in common_nx_c/forknx.c. This is done
 * to make libc.a and libnx.a share the same fork() routnine.
 *
 * Revision 1.83  1993/09/24  23:01:48  regnier
 * Tuning. Keep the application processor off the bus while polling for
 * completion of csend, crecv and crecvx. The same change needs to be
 * incorporated for isend/irecv.
 *
 * Revision 1.82  1993/09/24  21:15:34  ellend
 * Fix bug 6188 by ANDing off the GLOBAL_BIT in infoptype.
 *
 * Revision 1.81  1993/09/13  20:07:13  andrews
 * Now allow split xmsg buffers to have xmsg buffer fragments of size 0,
 * rather than FREEing size 0 fragments.  This would result in subsequent
 * failure if the kernel's xmsg_rdy was pointing to the size 0 buffer fragment
 * which was FREE'd.
 *
 * Revision 1.80  1993/09/13  17:22:07  regnier
 * Tuning...replace info bcopy()s with assignments.
 *
 * Revision 1.79  1993/09/09  21:14:27  andrews
 * Fix for previous checkin; wrong source version was checked in.
 *
 * Revision 1.78  1993/09/09  20:21:00  andrews
 * Fixed PTS bug #6497 (info*() calls returned incorrect values if placed
 * between irecv / isend calls).  The "copyinfo" parameter from _probe_nxreq()
 * is now passed down to _fulfill_nx_xmsg().
 *
 * Revision 1.77  1993/08/27  18:00:42  terry
 * this fixes two bugs: # 6325 and 6379
 * also 4485 which a duplicate of 6379.  The first 6325 is where the performance
 * improvement to csend, crecv etc. caused a problem of not keeping accurate
 * state of the mid.  The second was a problem when setptype was not called
 * and message passing was still attempted.
 *
 * Revision 1.76  1993/08/12  21:27:59  regnier
 * Add optimization so that thread 0 does not have to allocate and free
 * a mid.
 *
 * Revision 1.75  1993/08/04  16:14:13  andrews
 * Fixed ipd_msg_info initialization problem.
 *
 * Revision 1.74  1993/07/29  21:44:22  tnt
 * Changed locations of the nx_spin_lock code to protect the necessary
 * critical sections.  Changed calls to setup_recv within the hrecv functions
 * to pass a NULL pointer as the msginfo pointer, to solve the problem with
 * hrecv's altering the msginfo values.
 *
 * Revision 1.73  1993/07/23  22:39:47  andrews
 * Fixed PTS bug 5880 (child processes of nx_nfork() could not be debugged
 * with IPD; nx_ptypes_by_pid() would fail).
 *
 * Revision 1.72  1993/07/09  22:53:45  joel
 * Moved #define NUM_NXREQ to kernel's mcmsg_nx.h so _gcolx.c can use it too.
 *
 * Revision 1.71  1993/07/08  18:03:42  andrews
 * Fixed infonode() for global sends.
 *
 * Revision 1.70  1993/07/06  23:57:45  andrews
 * Added xmsg buffer coalescing.
 *
 * Revision 1.69  1993/07/06  21:52:17  prp
 * Change IPD info about buffers.
 *
 * Revision 1.68  1993/07/01  18:00:57  regnier
 * Add infocopy parameter to _probe_nxreq() to fix multiple receive
 * events for PERFMON support. Also set info pointer to NULL for hrecv
 * and hrecvx hopefully to fix Texas A&M problem.
 *
 * Revision 1.67  1993/06/29  00:54:14  prp
 * Make probe basically work again.
 *
 * Revision 1.66  1993/06/22  22:08:42  prp
 * Move all buffered message matching to the kernel.
 *
 * Revision 1.65  1993/06/11  23:14:53  shala
 * Fixed the xmsg address parameter in two calls to _xmsg_bcopy().
 * Added additional debug information to nrq structure in _setup_send().
 * Fixed by andrews.
 *
 * Revision 1.64  1993/06/10  16:25:11  andrews
 * Added split xmsg buffer support.
 *
 * Revision 1.63  1993/06/09  17:43:15  shala
 * Fixed unresolved spin_lock and spin_unlock, by changing the names to
 * nx_spin_lock and nx_spin_unlock.
 *
 * Revision 1.62  1993/06/09  00:30:30  dbm
 * Added spin_unlock() and spin_lock() calls around the calls to check_gsend
 * to allow simple broadcast to work.  THIS NEEDS TO BE LOOKED AT BY THE
 * MESSAGE PASSING PEOPLE TO PUT CORRECT FIX IN !!
 *
 * Revision 1.61  1993/06/07  22:49:19  terry
 * change spin_lock_init, spin_lock and spin_unlock to nx_spin_lock_init,
 * nx_spin_lock and nx_spin_unlock respectively.  Also change nx_spin_lock and
 * nx_spin_unlock to _nx_spin_lock and _nx_spin_unlock respectively to avoid
 * a conflict with names in cthreads.
 *
 * Revision 1.60  1993/05/24  18:21:23  terry
 * added locking code to support hrecv and hsend and make message passing thread
 * safe.
 *
 * Revision 1.59  1993/05/23  00:52:24  prp
 * Message processor interface, moved NX calls, flick fix, library cleanup.
 *
 * Revision 1.58  1993/05/21  17:32:32  regnier
 * Small optimization by calculating mid address in critical sections.
 * Fixed compiler warning with typecast.
 *
 * Revision 1.57  1993/05/21  00:00:17  regnier
 * Merged R1.0 fixes that were not in main trunk:
 * 1.48.2.5: Fix to avoid receiving messages of system reserved types with
 *           a type selector of -1. Fixed by regnier.
 * 1.48.2.4: Fixed bug #4311 (hrecv) by regnier.
 *
 * Revision 1.56  1993/05/19  16:06:51  regnier
 * Merged back bug fixes that were lost from R1.0.
 *
 * Revision 1.55  1993/05/18  21:15:30  regnier
 * Added _flick() calls in blocking calls for T10.
 *
 * Revision 1.54  1993/05/04  16:58:55  shala
 * Define _nx_perfmon_msginfo and _nx_perfmon_xmsgt.
 *
 * Revision 1.53  1993/04/27  15:42:16  ellend
 * fix global send problem sending extra messages, bug #4942
 *
 * Revision 1.52  1993/04/02  23:33:01  top
 * fixed a bug that caused global send from the host node to fail.
 *
 * Revision 1.51  1993/03/23  19:45:45  hays
 * Modified nx_port.c, nxlib.c, and rklib.c for performance monitoring.
 * the macros are in the kernel header file mcmsg_xmsg.h
 *
 * Revision 1.50  1993/02/23  02:37:06  ellend
 * Parallel global send capability added
 *
 * Revision 1.49  1993/02/22  15:39:39  joel
 * Added forknx() to change the ptype to INVALID_PTYPE when fork() is called.
 * In conjunction with ceating a new version of libc/fork.s in IPSC860/fork.s
 * There is some PTS associated with this, but I don't know how to find out
 * which one it is.
 *
 * Revision 1.48  1993/02/04  18:48:03  shala
 * Call del_irecv_mid() in msgignore in addition to msgdone routines. This
 * fix should have been done in conjunction with Rev 1.44.
 * Fixed by joel.
 *
 * Revision 1.47  1993/01/25  21:26:18  joel
 * Changes to fix problem with globsend2 introduced by version 1.44.  If msg
 * is completed before msgdone is called (by provoke code in crecv & csend)
 * the info stuff was lost.
 *
 * Revision 1.46  1993/01/22  23:45:52  regnier
 * Fix race condition causing one request to eat two messages.
 * Problem caused the Latency & Bandwidth SAT (broadcast) to hang.
 *
 * Revision 1.45  1993/01/20  00:57:52  andrews
 * Fixed problem wherein _msgdone() returned without specifying a return value.
 *
 * Revision 1.44  1993/01/15  22:03:40  joel
 * Fix 2dfft bug.  Deadlock occurred of two nodes simultaneously did irecv
 * and then csend.  Fix allows csend to provoke a nx_recv_continue for
 * outstanding irecvs.  PTS 3564
 *
 * Revision 1.42  1993/01/14  19:48:13  andrews
 * Fixed bug #3566 (MPP Linpack).
 *
 * Revision 1.41  1992/12/24  00:18:02  ellend
 * Add check for ignored mids in msgdone
 *
 * Revision 1.40  1992/12/23  23:33:40  andrews
 * Added cross-application message passing support back into library.
 *
 * Revision 1.39  1992/12/23  22:43:40  andrews
 * Fixed back-to-back broadcast problem.
 *
 * Revision 1.38  1992/12/16  18:13:06  ellend
 * repair damage done by last update
 *
 * Revision 1.37  1992/12/16  02:01:19  regnier
 * Changes to _fulfill_nx_xmsg() for second round of VM support.
 *
 * Revision 1.36  1992/12/16  01:37:58  ellend
 * remove debug printf in hrecv, again
 *
 * Revision 1.35  1992/12/15  18:35:09  andrews
 * Bug fixes 3813, 3814.  Added cross-application message passing support.
 *
 * Revision 1.34  1992/12/14  23:56:50  ellend
 * Remove debug printf in hrecv
 *
 * Revision 1.33  1992/12/11  00:18:32  ellend
 * Fix bugs 3547, 3578 and 3580
 *
 * Revision 1.32  1992/11/17  18:53:10  dleslie
 * This version was used in transmittal 6.  The 'ifdef' around put_imptype_t
 * wasn't checked into CVS.  Joel Clark told me about this.
 *
 * Revision 1.31  1992/11/04  17:43:47  andrews
 * Moved IPD initialization code to a point earlier in application initialization
 * sequence.
 *
 * Revision 1.30  1992/10/31  17:50:02  joel
 *  Fixed PTS 2722 and 3151.  Now mcmsg_init returns -2 if it cannot wire
 * down the user memory without leaving too few pages for the system to continue
 * to operate.
 *
 * Revision 1.29  1992/10/16  18:18:33  andrews
 * Fixed include file problem.
 *
 * Revision 1.28  1992/10/15  23:01:11  andrews
 * Added IPD support.
 *
 * Revision 1.27  1992/10/09  23:06:04  regnier
 * Use symbol INVALID_PTYPE (defined in nx.h) instead of constant.
 *
 * Revision 1.26  1992/10/02  23:25:39  joel
 * Fix PTS 3078.  FORCE type message sent by a process to itself with no
 * receive posted where not being dropped.  Now if no receive is posted the
 * nxreq.state is marked NX_COMPLETE, the send returns success and the message
 * is dropped.
 *
 * Revision 1.25  1992/10/02  22:42:18  regnier
 * Set mcmsg_initialized to getpid() instead of 1 for the case of
 * forked processes.
 *
 * Revision 1.24  1992/09/08  14:39:24  regnier
 * Added function _init_mcmsg_intf to initialize the mcmsg_intf structure
 * without calling setptype.
 *
 * Revision 1.23  92/09/03  07:56:08  shala
 * Used the temp names for rk interface until they are available to users
 * 
 * Revision 1.22  92/08/03  16:54:41  regnier
 * Change interface to mcmsg_nx_recv. The system call now returns
 * status rather than an xmsg.
 * 
 * Add the _masktrap call.
 * 
 * Revision 1.21  92/07/30  15:40:57  regnier
 * Fixed count parameter for handler of hsend calls.
 * 
 * Revision 1.20  92/07/28  13:24:19  regnier
 * Add _hsend and _hsendx calls.
 * 
 * Revision 1.19  92/07/27  17:56:10  regnier
 * Add nx_port_init() call to _setptype to setup NX kernel port.
 * 
 * Revision 1.18  92/07/27  11:29:52  regnier
 * Add _hrecv() and _hrecvx() calls.
 * 
 * Revision 1.17  92/07/06  19:58:05  prp
 * Bug 2900
 * Check for c_nxreq special case
 * 
 * Bug 2901
 * Add type check
 * 
 * Bug 2902
 * Add ptype check
 * 
 * Revision 1.16  92/06/25  12:50:32  prp
 * Bug 2847
 * Set errno when _setptype fails
 * Note - we need to change "pid" to "ptype" in all the EQ errors
 * 
 * Revision 1.15  92/06/23  18:33:31  prp
 * Bugs 2818 and 2821
 * 
 * Revision 1.14  92/06/17  17:35:13  prp
 * Bug 2682, 2nd try
 * 
 * Revision 1.13  92/06/17  15:18:27  prp
 * Buffer management fixes
 * 
 * Revision 1.12  92/06/15  12:39:25  prp
 * Bug 2715
 * Bug 2716
 * 
 * Revision 1.11  92/06/12  21:17:00  prp
 * Bug 2723
 * Also an important fix to _lock_buffer for stack based buffers
 * 
 * Revision 1.10  92/06/09  21:17:52  stans
 * added ID and ident strings
 * 
 *
 */
char _nxlib_c_id[]="$Id: nxlib.c,v 1.110 1995/03/10 23:11:38 joel Exp $";

/*
 * nxlib.c
 *
 * NX interface library
 */

#include <stdio.h>
#include <errno.h>
#include <mcmsg/mcmsg_appl.h>
#include <mcmsg/mcmsg_intf.h>
#include <mcmsg/mcmsg_xmsg.h>
#include <nx/mcmsg_lib.h>
#include <mcmsg/mcmsg_nx.h>
#include <nx/assert.h>
#include <nx/debugxmsg.h>
#include <sys/types.h>
#include <dbglib.h>
#include <sys/table.h>
#include <mach/port.h>
#include <mach/thread_switch.h>

long _isend();
long _msgwait();
long _msgignore();
long _msgmerge();

void ipd_ptype_init();

#ifdef VERIFY_XMSG
#define xmsg_verify(xmsg) _xmsg_verify(xmsg)
#else
#define xmsg_verify(xmsg)
#endif

#ifndef NO_PERFMON
unsigned long *_nx_perfmon_msginfo;
xmsg_t        *_nx_perfmon_xmsgt;
#endif

typedef
struct cache_line {
	double	x[4];
} cache_line_t;

cache_line_t		nxreq_space[ (NUM_NXREQ*sizeof(nxreq_t) +
				      sizeof(cache_line_t) - 1) /
				     sizeof(cache_line_t) + 1];
nxreq_t			*nxreq;
volatile long			nxreq_free;
volatile long			nxreq_ignore;
volatile unsigned long		nxreq_active;

/* 
 *	mid reserved for thread zero's _csend(),_crecv()
 */
int		cmid;

/*
 *	Values for infocopy parameter to _probe_nxreq:
 */
#define	NO_INFOCOPY	0
#define	INFOCOPY		1

long mcmsg_ptype_range_low;
long mcmsg_ptype_range_high;
ipd_msg_info_t ipd_msg_info;
ipd_msg_info_t *ipd_msg_info_p = NULL;
ipd_msg_aux_t ipd_msg_aux[NUM_NXREQ];
long	ipd_call_type = IPD_UNKNOWN_TYPE;
ipd_ptype_list_t	ipd_plist;

#define PAGEADDR(x,d) ( ( ((unsigned long)(x)) + (d) ) & 0xFFFFF000 )

extern unsigned long data_lock_end;
extern unsigned long first_stack_locked;
extern unsigned long handler_stack_locked;
extern unsigned long handler_stack_top;

_nx_lib_init()
{
	int	i;
	long	status;

	nxreq_free = -1;
	nxreq_ignore = -1;
	nxreq_active = 0;

	if (sizeof(nxreq_t) % sizeof(cache_line_t) != 0) {
		printf("bad nxreq size: %d, should be multiple of %d\n",
			sizeof(nxreq_t), sizeof(cache_line_t));
	}

	/* Align to cache-line boundary */
	nxreq = (nxreq_t *)
		(((unsigned long)nxreq_space + sizeof(cache_line_t) - 1) &
		~(sizeof(cache_line_t) - 1));

	for (i = NUM_NXREQ-1; i >= 0; i--) {
		nxreq[i].state = NX_FREE;
		nxreq[i].monitored = 0;
		/* Make sure they don't straddle page boundary */
		if (PAGEADDR(&nxreq[i], 0) ==
		    PAGEADDR(&nxreq[i], sizeof(nxreq_t)-1)) {
			nxreq[i].link = nxreq_free;
			nxreq_free = i;
		}
	}
	/*
	 * Reserve a mid for _csend, _crecv
	 */
	cmid = _alloc_mid();
	nxreq[cmid].monitored = 1;
	nxreq[cmid].state = NX_FREE;

	return 0;
}


_lock_buffer(buf, count)
	register unsigned long	buf;
	register unsigned long	count;
{
	register unsigned long bufend = buf + count;
	register int		st = 0;

	/* handler_stack_top may be 0 if nx_port_init not done yet */
	while (!handler_stack_top) {
		flick();
	}
	if (buf > handler_stack_top && buf < first_stack_locked) {
		st = mcmsg_wire(buf, first_stack_locked - buf);
		first_stack_locked = buf;
	} else if (bufend > data_lock_end && buf < handler_stack_locked) {
		if ((bufend - data_lock_end) < handler_stack_locked - buf) {
			st = mcmsg_wire(data_lock_end, bufend - data_lock_end);
			data_lock_end = bufend;
		} else {
			st = mcmsg_wire(buf, handler_stack_locked - buf);
			handler_stack_locked = buf;
		}
	}
	if (st) {
		if (st == -1) {
			errno = EQNOACT;
			return -1;
		}
		if (st == -2) {
			errno = ENOMEM;
			return -1;
		}
	}
	return 0;
}

_alloc_mid()
{
	long	mid;

	if(nxreq == (nxreq_t *)0) {
		errno = EQNOSET;
		return -1;
	}
	nx_spin_lock();
	mid = nxreq_free;
	if (mid != -1) {
		nxreq_free = nxreq[mid].link;
		nxreq[mid].link = -1;
		nxreq_active++;
		ipd_msg_aux[mid].call_type = ipd_call_type;
		ipd_msg_aux[mid].ptype = mcmsg_ptype;
		nx_spin_unlock();
		return mid;
	}
	mid = nxreq_ignore;
	if (mid != -1) {
		long	next;
		long	prev;

		prev = mid;
		next = nxreq[mid].link;
		for (;;) {
			if (_probe_nxreq(&nxreq[next], INFOCOPY, 0) != 0) {
				if (prev == next) {
					nxreq_ignore = -1;
				} else {
					nxreq[prev].link = nxreq[next].link;
					if (nxreq_ignore == next) {
						nxreq_ignore = prev;
					}
				}
				nxreq[next].state = NX_FREE;
				nxreq[next].monitored = 0;
				nxreq[next].link = -1;
				ipd_msg_aux[next].call_type = ipd_call_type;
				ipd_msg_aux[next].ptype = mcmsg_ptype;
				nx_spin_unlock();
				return next;
			}
			if (next == mid) {
				break;
			}
			prev = next;
			next = nxreq[next].link;
		}
	}
	errno = EQNOMID;
	nx_spin_unlock();
	return -1;
}
/*
 * Routine:	free_mid(mid)
 *
 * Description: Returns a mid to the free pool
 *
 * Returns: none
 */

_free_mid(mid)
	long mid;
{
	nx_spin_lock();
	nxreq[mid].state = NX_FREE;
	nxreq[mid].monitored = 0;
	nxreq[mid].link = nxreq_free;
	nxreq_free = mid;
	nxreq_active--;
	nx_spin_unlock();
}

_setup_send(type, buf, count, node, ptype, handler, hparam,
            nrq, originating_node,source_ptype)
	long	type;
	char	*buf;
	long	count;
	long	node;
	long	ptype;
	long	handler;
	long	hparam;
	nxreq_t	*nrq;
	unsigned long originating_node;
	long	source_ptype;
{
	int	st;

	if (type < 0) {
		errno = EQTYPE;
		return -1;
	}
	if (ptype < 0) {
		errno = EQPID;
		return -1;
	}
	if (count < 0) {
		errno = EQLEN;
		return -1;
	}

	nrq->state = NX_ACTIVE;
	nrq->err = NX_ERR_NONE;
	nrq->req = NX_SEND_REQ;
	nrq->type = type;
	nrq->node = node;
	nrq->ptype = ptype;
	nrq->bsize = count;
	nrq->bcount = 1;
	nrq->handler = handler;
	nrq->hparam = hparam;
	nrq->xmsg = 0;
	if (nrq->handler) {
		nrq->localinfo[0] = type;
		nrq->localinfo[1] = count;
		nrq->localinfo[2] = node;
		nrq->localinfo[3] = ptype;
	}
	nrq->info = 0;
	if (mcmsg_process_lock) {
		if (_lock_buffer(buf, count) == -1) {
			return -1;
		}
	}
	mcmsg_nx_send(type, buf, count, node, ptype, source_ptype,
	              nrq, originating_node);
	return 0;
}

_setup_recv(type, buf, count, node, ptype, info, handler, hparam, nxreq)
	long		type;
	unsigned long	buf;
	long		count;
	long		node;
	long		ptype;
	unsigned long	*info;
	unsigned long	handler;
	unsigned long	hparam;
	nxreq_t		*nxreq;
{
	int			result;

	nxreq->state = NX_ACTIVE;
	nxreq->err = NX_ERR_NONE;
	nxreq->info = info;
	nxreq->req = NX_RECV_REQ;
	nxreq->type = type;
	nxreq->node = node;
	nxreq->ptype = ptype;
	nxreq->buf = buf;
	nxreq->bsize = count;
	nxreq->bcount = 1;
	nxreq->boffset = 0;
	nxreq->handler = handler;
	nxreq->hparam = hparam;
	nxreq->xmsg = (xmsg_t *) 0;


	mcmsg_nx_recvx(	type, buf, count, node, ptype,
			mcmsg_ptype, nxreq);
	return 0;
}

#ifdef VERIFY_XMSG
xmsg_t	*start_xmsg;
xmsg_t	*curr_xmsg;
int	msg_count;

_xmsg_verify(xmsg)
xmsg_t	*xmsg;
{
unsigned char	chain_number;
char		first;
char		all_free;

	start_xmsg = xmsg;
	msg_count = 0;
	all_free = 1;

	while (xmsg != 0) {
		if ((xmsg->state == XMSG_EMPTY) ||
		    (xmsg->state == XMSG_BUSY))
			return;
		msg_count++;
		curr_xmsg = xmsg;
		assert(xmsg->state != XMSG_SPLIT);
		assert((xmsg->chain_number == 0) ||
		       (xmsg->chain_number == 1) ||
		       (xmsg->chain_number == 2)); 
		chain_number = xmsg->chain_number;
		first = 1;
		if (xmsg->state != XMSG_FREE)
			all_free = 0;

		while ((xmsg != 0) && (xmsg->chain_number != 0)) {
			if (!first)
				assert(xmsg->state == XMSG_SPLIT);
			if (!first || (xmsg->state == XMSG_FREE)) {
				assert(xmsg->backlink != 0);
			}
			if (xmsg->backlink != 0)
				assert(xmsg->backlink->link == xmsg);
			if (xmsg->link != 0)
				assert(xmsg->link->backlink == xmsg);
			if ((xmsg->prev_adjacent != 0) &&
			    (xmsg->prev_adjacent->state != XMSG_EMPTY) &&
			    (xmsg->prev_adjacent->state != XMSG_BUSY))
				assert(xmsg->prev_adjacent->next_adjacent ==
				       xmsg);
			if (xmsg->next_adjacent != 0) {
				if ((xmsg->next_adjacent->state !=
				     XMSG_EMPTY) &&
				    (xmsg->next_adjacent->state !=
				     XMSG_BUSY))
					assert(xmsg->next_adjacent->prev_adjacent ==
			 		       xmsg);
				assert(xmsg->next_adjacent ==
				       (xmsg_t *)((unsigned long)xmsg +
				                   xmsg->size +
				                   sizeof(xmsg_t)));
			}
			xmsg = xmsg->link;
			if (xmsg->state != XMSG_FREE)
				all_free = 0;
			if (xmsg != 0) {
				chain_number++;
				assert((xmsg->chain_number == chain_number) ||
			               (xmsg->chain_number == 0));
			}
			first = 0;	
		}	

		if (xmsg->backlink != 0)
			assert(xmsg->backlink->link == xmsg);
		if (xmsg->link != 0)
			assert(xmsg->link->backlink == xmsg);
		if ((xmsg->prev_adjacent != 0) &&
		    (xmsg->prev_adjacent->state != XMSG_EMPTY) &&
		    (xmsg->prev_adjacent->state != XMSG_BUSY))
			assert(xmsg->prev_adjacent->next_adjacent == xmsg);
		if (xmsg->next_adjacent != 0) {
			if ((xmsg->next_adjacent->state != XMSG_EMPTY) &&
			    (xmsg->next_adjacent->state != XMSG_BUSY))
				assert(xmsg->next_adjacent->prev_adjacent ==
			 	       xmsg);
			assert(xmsg->next_adjacent ==
				       (xmsg_t *)((unsigned long)xmsg +
				                   xmsg->size +
				                   sizeof(xmsg_t)));
		}

		if (!first && (xmsg != 0))
			assert(xmsg->state == XMSG_SPLIT);

		if (xmsg != 0)
			xmsg = xmsg->link;

		if (xmsg == start_xmsg) {
			assert(all_free);
			break;
		}
	}
}
#endif VERIFY_XMSG

mcmsg_set_errno(nxreq)
	nxreq_t	*nxreq;
{

	switch (nxreq->err) {

	case NX_ERR_NODE:
		errno = EQNODE;
		break;
	case NX_ERR_LEN:
		errno = EQMSGLONG;
		break;
	case NX_ERR_NOMEM:
		errno = ENOMEM;
		break;
	case NX_ERR_NOSENDER:
		errno = EQSENDABORT;
		break;

	default:
		errno = EINVAL;
	}
}

_xmsg_bcopy(xmsg, buf, size)
xmsg_t		*xmsg;
unsigned long	*buf;
unsigned long	size;
{
	unsigned long	need;
	unsigned long	offset;
	unsigned long	buf_size;

	xmsg_verify(xmsg);
	need = size;
	offset = 0;
	while (need) {
		if (need > xmsg->size)
			buf_size = xmsg->size;
		else
			buf_size = need;
		if (buf_size > 0) {
			bcopy(xmsg+1, ((unsigned long)buf) + offset, buf_size);
		}
		need -= buf_size;
		if (need > 0) {
			assert(xmsg->chain_number != 0);
			offset += buf_size;
			xmsg = xmsg->link;
		}
	}
}

/*
 *	_fulfill_nx_xmsg
 *
 *	Return 1 if message complete
 *	       0 if message not complete
 *	      -1 if error
 */
_fulfill_nx_xmsg(nxreq, xmsg, copyinfo)
	nxreq_t		*nxreq;
	xmsg_t		*xmsg;
	int		copyinfo;
{

	xmsg_verify(xmsg);

	if (xmsg->length > nxreq->bsize * nxreq->bcount) {
		errno = EQMSGLONG;
		nxreq->info = 0;
		nxreq->state = NX_COMPLETE;
		return -1;
	}

	if (xmsg->state == XMSG_FULL) {
		nxreq->localinfo[0] = xmsg->msg_type;
		nxreq->localinfo[1] = xmsg->length;
		nxreq->localinfo[2] = xmsg->originating_node;
		nxreq->localinfo[3] = xmsg->source_ptype;
		nxreq->localinfo[4] = xmsg->source_node;
		if (xmsg->length <= xmsg->totalsize) {
			_xmsg_bcopy(xmsg, nxreq->buf, xmsg->length);
		} else {
			_xmsg_bcopy(xmsg, nxreq->buf, xmsg->totalsize);
			xmsg->length -= xmsg->totalsize;
			xmsg->xmsg_data = 0;
			xmsg->xmsg_offset = xmsg->totalsize;
			xmsg->state = XMSG_CONT;
			nxreq->state = NX_ACTIVE;
			mcmsg_nx_recv_continue(xmsg->si);
			xmsg_verify(xmsg);
			return 0;
		}
	} else {
		/*
		 * xmsg->state == XMSG_STOP
		 */
		if (xmsg->length <= xmsg->xmsg_stop) {

			_xmsg_bcopy(xmsg,
			   nxreq->buf + xmsg->xmsg_offset,
			   xmsg->length);

		} else {

			_xmsg_bcopy(xmsg,
			   nxreq->buf + xmsg->xmsg_offset,
			   xmsg->xmsg_stop);

			xmsg->length      -= xmsg->xmsg_stop;
			xmsg->xmsg_offset += xmsg->xmsg_stop;
			xmsg->xmsg_data = 0;
			xmsg->state = XMSG_CONT;
			nxreq->state = NX_ACTIVE;
			mcmsg_nx_recv_continue(xmsg->si);
			xmsg_verify(xmsg);
			return 0;
		}
	}

	_nx_free(xmsg);
	nxreq->xmsg = 0;

	if (nxreq->err != NX_ERR_NONE) {
		mcmsg_set_errno(nxreq);
		nxreq->state = NX_COMPLETE;
		return -1;
	}
	if (copyinfo) {
		if (nxreq->info != 0) {
			nxreq->info[0] = nxreq->localinfo[0];
			nxreq->info[1] = nxreq->localinfo[1];
			nxreq->info[2] = nxreq->localinfo[2];
			nxreq->info[3] = nxreq->localinfo[3] & (~GLOBAL_BIT);
			nxreq->info[4] = nxreq->localinfo[4];
			PERFMON_RECV_INFO (_fulfill_nx_xmsg, nxreq->info);
			nxreq->info = 0;
		}
	}
	nxreq->state = NX_COMPLETE;
	return 1;
}

_nx_free(xmsg)
	xmsg_t	*xmsg;
{
	xmsg_t          *xxmsg;

	xxmsg = xmsg;
	for (;;) {
		mcmsg_provide_bytes -= xxmsg->size + sizeof(xmsg_t);
		if (xxmsg->chain_number == 0)
			break;
		xxmsg = xxmsg->link;
	}

	_xfree(xmsg);
	_xprovide();
}

long
_iprobex(type, node, ptype, info)
	long	type;
	long	node;
	long	ptype;
	long	*info;
{
	int	st;
	int	mid;
	nxreq_t *nrq;
	xmsg_t	*xmsg;
	long	save_ipd_call_type;

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_IPROBEX_TYPE;

	mid = _alloc_mid();
	if (mid == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	nrq = &nxreq[mid];
	ipd_msg_aux[mid].call_type = ipd_call_type;
	ipd_msg_aux[mid].ptype = mcmsg_ptype;

	_setup_recv(type, 0, -1, node, ptype, 0, 0, 0, nrq);

	PERFMON_IDLE_START(_probex);
	for (;;) {

		if (nrq->state != NX_ACTIVE) {
			break;
		}

		_flick();
	}
	PERFMON_IDLE_END(_probex);

	ipd_call_type = save_ipd_call_type;

	st = 0;
	if (nrq->state == NX_BUFFERED) {
		if (info != 0) {
			xmsg = nrq->xmsg;
			info[0] = xmsg->msg_type;
			info[1] = xmsg->length;
			info[2] = xmsg->originating_node;
			info[3] = xmsg->source_ptype & ~GLOBAL_BIT;
			info[4] = xmsg->source_node;
		}
		st = 1;
	}
	_free_mid(mid);
	return st;
}

long
_iprobe(typesel)
	long	typesel;
{

	return _iprobex(typesel, -1, -1, msginfo);
}

long
_cprobex(typesel, nodesel, ptypesel, info)
	long	typesel;
	long	nodesel;
	long	ptypesel;
	long	*info;
{
	int	st;

	PERFMON_IDLE_START(_cprobex);
	for (;;) {
		st = _iprobex(typesel, nodesel, ptypesel, info);
		if (st != 0) {
		  break;
		}
		_flick();
	}
	PERFMON_IDLE_END(_cprobex);
	return st;
}

long
_cprobe(typesel)
	long	typesel;
{

	return _cprobex(typesel, -1, -1, msginfo);
}

_probe_nxreq(nxreq, copyinfo, lock)
	nxreq_t	*nxreq;
	int		copyinfo;	/* boolean */
	int		lock;	/* boolean */
{

	if (nxreq->state == NX_ACTIVE) {
		return 0;
	}

	if (lock) nx_spin_lock();

	if (nxreq->state == NX_BUFFERED) {
		xmsg_verify(nxreq->xmsg);
		if (nxreq->xmsg->state == XMSG_FULL ||
		    nxreq->xmsg->state == XMSG_STOP) {
			int status;
			status = _fulfill_nx_xmsg(nxreq, nxreq->xmsg, copyinfo);
			if (lock) nx_spin_unlock();
			return status;
		}
		if (lock) nx_spin_unlock();
		return 0;
	}
	if (nxreq->xmsg != 0) {
		xmsg_verify(nxreq->xmsg);
		_nx_free(nxreq->xmsg);
		nxreq->xmsg = 0;
	}
	if (nxreq->err != NX_ERR_NONE) {
		mcmsg_set_errno(nxreq);
		if (lock) nx_spin_unlock();
		return -1;
	}
	if (copyinfo && (nxreq->req == NX_RECV_REQ)) {
		if (nxreq->info != 0) {
			nxreq->info[0] = nxreq->localinfo[0];
			nxreq->info[1] = nxreq->localinfo[1];
			nxreq->info[2] = nxreq->localinfo[2];
			nxreq->info[3] = nxreq->localinfo[3] & (~GLOBAL_BIT);
			nxreq->info[4] = nxreq->localinfo[4];
			PERFMON_RECV_INFO(_probe_nxreq, nxreq->info);
		}
		nxreq->info = 0;
	}
	nxreq->state = NX_COMPLETE;
	xmsg_verify(nxreq->xmsg);
	if (lock) nx_spin_unlock();
	return 1;
}


long
_csend(type, buf, count, node, ptype)
	long	type;
	char	*buf;
	long	count;
	long	node;
	long	ptype;
{
	int	done, i, mid;
	nxreq_t	*midp;
	int thread0;	/* boolean */
	long	save_ipd_call_type;


	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_CSEND_TYPE;

	if (node == -1) {
		mid = _isend(type, buf, count, node, ptype);
		done = -1;
		if (mid != -1) {
			done = _msgwait(mid);
		}
		ipd_call_type = save_ipd_call_type;
		return done;
	}

	thread0 = _nx_main_thread();
	if (thread0) {
		mid = cmid;
		if(nxreq == (nxreq_t *)0) {
			errno = EQNOSET;
			return -1;
		}
	} else {
		mid = _alloc_mid();
		if (mid == -1) {
			ipd_call_type = save_ipd_call_type;
			return -1;
		}
	}
	midp = &nxreq[mid];

	ipd_msg_aux[mid].call_type = ipd_call_type;
	ipd_msg_aux[mid].ptype = mcmsg_ptype;

	/*
	 * Post Send
	 */
	done = _setup_send(type, buf, count, node, ptype, 0, 0, midp, mynode(),
			mcmsg_ptype);

	/*
	 * Block
	 */
	PERFMON_IDLE_START(_csend);
	while (!done) {
		if (midp->state != NX_ACTIVE) {
			done = _probe_nxreq(midp, NO_INFOCOPY, 1);
			if (done) break;
		}
		_flick();
	}
	PERFMON_IDLE_END(_csend);

	if ( !thread0 ) {
		_free_mid(mid);
	} else {
		midp->state = NX_FREE;
	}
	ipd_call_type = save_ipd_call_type;

	if (done == -1) {
		return -1;
	}
	return 0;
}

long
_crecv(type, buf, count)
	long	type;
	unsigned long	buf;
	long	count;
{
	int	done, i, st;
	int	mid;
	nxreq_t  *midp;
	int thread0;	/* boolean */
	long	save_ipd_call_type;

	if (count < 0) {
		errno = EQLEN;
		return -1;
	}
	if (mcmsg_process_lock) {
		if (_lock_buffer(buf, count) == -1) {
			return -1;
		}
	}

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_CRECV_TYPE;

	thread0 = _nx_main_thread();
	if (thread0) {
		mid = cmid;
		if(nxreq == (nxreq_t *)0) {
			errno = EQNOSET;
			return -1;
		}
	} else {
		mid = _alloc_mid();
		if (mid == -1) {
			ipd_call_type = save_ipd_call_type;
			return -1;
		}
	}
	midp = &nxreq[mid];

	ipd_msg_aux[mid].call_type = ipd_call_type;
	ipd_msg_aux[mid].ptype = mcmsg_ptype;

	/* This is a monitored receive */
	midp->monitored = 1;

	/*
	 * Post Receive
	 */
	_setup_recv(type, buf, count, -1, -1, msginfo, 0, 0, midp);

	/*
	 * Block
	 */
	PERFMON_IDLE_START(_crecv);
	for(;;) {

		if (midp->state != NX_ACTIVE) {
			done = _probe_nxreq(midp, INFOCOPY,1);
			if (done) break;
		}
		_flick();
	}
	PERFMON_IDLE_END(_crecv);

	ipd_call_type = save_ipd_call_type;
	if (done == -1) {
		return -1;
	}

	st = _check_gsend(midp);
	if (st == -1) {
		if ( !thread0 ) {
			_free_mid(mid);
		} else {
			midp->state = NX_FREE;
		}
		return -1;
	}

	if ( !thread0 ) {
		_free_mid(mid);
	} else {
		midp->state = NX_FREE;
	}
	return 0;
}

long
_crecvx(type, buf, count, node, ptype, info)
	long	type;
	unsigned long	buf;
	long	count;
	long	node;
	long	ptype;
	long	*info;
{
	int	done, i, st;
	int	mid;
	nxreq_t *midp;
	int thread0;
	long	save_ipd_call_type;

	if (count < 0) {
		errno = EQLEN;
		return -1;
	}
	if (mcmsg_process_lock) {
		if (_lock_buffer(buf, count) == -1) {
			return -1;
		}
	}

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_CRECVX_TYPE;

	thread0 = _nx_main_thread();
	if (thread0) {
		mid = cmid;
		if(nxreq == (nxreq_t *)0) {
			errno = EQNOSET;
			return -1;
		}
	} else {
		mid = _alloc_mid();
		if (mid == -1) {
			ipd_call_type = save_ipd_call_type;
			return -1;
		}
	}
	midp = &nxreq[mid];

	ipd_msg_aux[mid].call_type = ipd_call_type;
	ipd_msg_aux[mid].ptype = mcmsg_ptype;

	/* This is a monitored receive */
	midp->monitored = 1;

	/*
	 * Post Receive
	 */
	_setup_recv(type, buf, count, node, ptype, info, 0, 0, midp);

	/*
	 * Block
	 */
	PERFMON_IDLE_START(_crecvx);
	while (1) {

		if (midp->state != NX_ACTIVE) {
			done = _probe_nxreq(midp, INFOCOPY,1);
			if (done) break;
		}
		_flick();
	}
	PERFMON_IDLE_END(_crecvx);

	ipd_call_type = save_ipd_call_type;

	if (done == -1) {
		return -1;
	}

	st = _check_gsend(midp);
	if (st == -1) {
		if ( !thread0 ) {
			_free_mid(mid);
		} else {
			midp->state = NX_FREE;
		}
		return -1;
	}

	if ( !thread0 ) {
		_free_mid(mid);
	} else {
		midp->state = NX_FREE;
	}
	return 0;
}

_check_gsend(nxreq)
	nxreq_t *nxreq;
{
	int ncount;
	long nlist[MAX_GLOBAL];
	int i, st;
	int smid;
	long ptype;

	ptype = nxreq->localinfo[3];
	if (ptype & GLOBAL_BIT) {
		/*
		 * Global message, need to send it on
		 */

		ncount = tree_recv(nxreq->localinfo[4], nlist);
		if (ncount == 0) {
			return 0;
		}
		smid = -1;
		for (i = 0; i < ncount; i++) {
			st = _isend_forward(nxreq->localinfo[0],
			                   (char *)nxreq->buf, 
				           nxreq->localinfo[1], nlist[i],
			                   mcmsg_ptype,
			                   nxreq->localinfo[2],
					   mcmsg_ptype | GLOBAL_BIT);

			if (st == -1) {
				if (smid != -1) {
					_msgignore(smid);
				}
				return -1;
			}
			smid = _msgmerge(smid, st);
		}
		/*
		 * Wait for sends to complete
		 */
		_msgwait(smid);
	}
	return 0;
}

long
_isend(type, buf, count, node, ptype)
	long	type;
	char	*buf;
	long	count;
	long	node;
	long	ptype;
{
	return _isend_forward(type, buf, count, node, ptype, mynode(),
				mcmsg_ptype);
}

int
_isend_forward(type, buf, count, node, ptype, originating_node, source_ptype)
	long	type;
	char	*buf;
	long	count;
	long	node;
	long	ptype;
	unsigned long originating_node;
	long	source_ptype;
{
	int		mid;
	int		st;
	long		save_ipd_call_type;
	char		xapp;
	long		nodecount;
	long		nlist[MAX_GLOBAL];

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_ISEND_TYPE;

	if (node == -1) {
		long	i;

		nodecount = tree_send(nlist);
		xapp = FALSE;
		/*
		 * Set source ptype to indicate global send
		 */
		mid = -1;

		for (i = 0; i < nodecount; i++) {
			if (nlist[i] == mynode() && ptype == myptype() && !xapp) {
				continue;
			}
			st = _isend_forward(type, buf, count, nlist[i],
			                    ptype, mynode(),
					    mcmsg_ptype | GLOBAL_BIT);
			if (st == -1) {
				if (mid != -1) {
					_msgignore(mid);
				}
				ipd_call_type = save_ipd_call_type;
				return -1;
			}
			mid = _msgmerge(mid, st);
		}
		if (mid == -1) {	/* No nodes to send to, not an error */
			mid = _alloc_mid();
			if (mid != -1) {
				nxreq[mid].state = NX_COMPLETE;
				nxreq[mid].info = 0;
			}
		}
		ipd_call_type = save_ipd_call_type;
		return mid;
	}

	mid = _alloc_mid();
	if (mid == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	st = _setup_send(type, buf, count, node, ptype, 0, 0,
	                 &nxreq[mid], originating_node,source_ptype);
	ipd_call_type = save_ipd_call_type;
	if (st == -1) {
		return -1;
	}
	return mid;
}

long
_irecvx(type, buf, count, node, ptype, info)
	long	type;
	unsigned long	buf;
	long	count;
	long	node;
	long	ptype;
	long	*info;
{
	int		mid;
	int		st;
	long		save_ipd_call_type;

	if (count < 0) {
		errno = EQLEN;
		return -1;
	}
	if (mcmsg_process_lock) {
		if (_lock_buffer(buf, count) == -1) {
			return -1;
		}
	}

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_IRECVX_TYPE;

	mid = _alloc_mid();
	if (mid == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	_setup_recv(type, buf, count, node, ptype, info, 0, 0, &nxreq[mid]);
	ipd_call_type = save_ipd_call_type;
	return mid;
}

long
_irecv(type, buf, count)
	long	type;
	unsigned long	buf;
	long	count;
{
	int	mid;
	int	st;
	long	save_ipd_call_type;

	if (count < 0) {
		errno = EQLEN;
		return -1;
	}
	if (mcmsg_process_lock) {
		if (_lock_buffer(buf, count) == -1) {
			return -1;
		}
	}

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_IRECV_TYPE;

	mid = _alloc_mid();
	if (mid == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	_setup_recv(type, buf, count, -1, -1, msginfo, 0, 0, &nxreq[mid]);
	ipd_call_type = save_ipd_call_type;
	return mid;
}

/*
 *	  Routine:
 *		      _hsend_global
 *
 *	  Purpose:
 *		      Set up the global send for hsend calls
 *
 *	  Returns:
 *		      first mid in list if ok, -1 if error
 */

_hsend_global(type, buf, count, ptype, handler, hparam, save_ipd_call_type,
			  ncount, nlist)
	long	type;
	char	*buf;
	long	count;
	long	ptype;
	void	(*handler) ();
	long	hparam;
	long	save_ipd_call_type;
	int 	ncount;
	long	nlist[];
{
	long prev_mid, first_mid, mid;
	int node_count;
	int i;
	int st;

	prev_mid = -1;
	first_mid = -1;
	if (ncount == 0) {
		return 0;
	}
	for (i = 0; i < ncount; i++) {
		if ((nlist[i] == mynode()) && (ptype == myptype())) {
			continue;
		}
		mid = _alloc_mid();
		if (mid < 0) {
			ipd_call_type = save_ipd_call_type;
			return -1;
		}

		if (first_mid == -1) {
			first_mid = mid;
		}

		/*
		 * Connect mids in a circular list
		 */

		if (prev_mid != -1) {
			nxreq[mid].link = nxreq[prev_mid].link;
			nxreq[prev_mid].link = mid;
		} else {
			nxreq[mid].link = mid;
		}
		prev_mid = mid;
	}
	mid = first_mid;
	for (i = 0; i < ncount; i++) {
		if ((nlist[i] == mynode()) && (ptype == myptype())) {
			continue;
		}
		st = _setup_send(type, buf, count, nlist[i], ptype,
			 handler, hparam, &nxreq[mid], mynode(),
				mcmsg_ptype | GLOBAL_BIT);

		ipd_call_type = save_ipd_call_type;
		if (st == -1) {
			return -1;
		}
		mid = nxreq[mid].link;

	}
	return first_mid;
}

long
_hsend(type, buf, count, node, ptype, handler)
	long	type;
	char	*buf;
	long	count;
	long	node;
	long	ptype;
	void	(*handler) ();
{
	int	st;
	int mid;
	long	save_ipd_call_type;
	int	ncount;
	long nlist[MAX_GLOBAL];

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE) {
		ipd_call_type = IPD_HSEND_TYPE;
	}

	if (node == -1) {
		ncount = tree_send(nlist);
		if (_hsend_global(type, buf, count, ptype, handler, 0, 
				save_ipd_call_type, ncount, nlist) < 0) {
			return -1;
		} else {
			return 0;
		}
	}
	mid = _alloc_mid();

	if (mid == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	st = _setup_send(type, buf, count, node, ptype, 
			 handler, 0, &nxreq[mid], mynode(),mcmsg_ptype);
	ipd_call_type = save_ipd_call_type;
	if (st == -1) {
		return -1;
	}
	return 0;
}

long
_hsendx(type, buf, count, node, ptype, handler, hparam)
	long	type;
	char	*buf;
	long	count;
	long	node;
	long	ptype;
	void	(*handler) ();
	long	hparam;
{
	int	st;
	int mid;
	long	save_ipd_call_type;
	int	ncount;
	long nlist[MAX_GLOBAL];

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE) {
		ipd_call_type = IPD_HSENDX_TYPE;
	}
	if (node == -1) {
		ncount = tree_send(nlist);
		if (_hsend_global(type, buf, count, ptype,
				handler, hparam, save_ipd_call_type, ncount, nlist) < 0) {
			return -1;
		} else {
			return 0;
		}
	}
	mid = _alloc_mid();

	if (mid == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	st = _setup_send(type, buf, count, node, ptype, 
			 handler, hparam, &nxreq[mid], mynode(),mcmsg_ptype);
	ipd_call_type = save_ipd_call_type;
	if (st == -1) {
		return -1;
	}
	return 0;
}

long
_hrecvx(type, buf, count, node, ptype, handler, hparam)
	long	type;
	unsigned long	buf;
	long	count;
	long	node;
	long	ptype;
	void 	(*handler)();
	unsigned long	hparam;
{
	int	mid;
	int	st;
	long	save_ipd_call_type;

	if (count < 0) {
		errno = EQLEN;
		return -1;
	}
	if (mcmsg_process_lock) {
		if (_lock_buffer(buf, count) == -1) {
			return -1;
		}
	}

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_HRECVX_TYPE;

	mid = _alloc_mid();
	if (mid == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	_setup_recv(type, buf, count, node, ptype,
				 (void *)0, (long)handler, hparam, &nxreq[mid]);
	ipd_call_type = save_ipd_call_type;
	return 0;
}

long
_hrecv(type, buf, count, handler)
	long		type;
	unsigned long	buf;
	long	count;
	void (*handler)();
{
	int		mid;
	int		st;
	long		save_ipd_call_type;

	if (count < 0) {
		errno = EQLEN;
		return -1;
	}
	if (mcmsg_process_lock) {
		if (_lock_buffer(buf, count) == -1) {
			return -1;
		}
	}

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_HRECV_TYPE;

	mid = _alloc_mid();
	if (mid == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	_setup_recv(type, buf, count, -1, -1, 
				 (void *)0, (long)handler, 0, &nxreq[mid]);
	ipd_call_type = save_ipd_call_type;
	return 0;
}

long
_msgdone(amid)
	unsigned long	amid;
{
	long	mid;
	long	st;
	long	next;
	long	reqtype;

	if (amid >= NUM_NXREQ || nxreq[amid].state == NX_FREE) {
		errno = EQMID;
		return -1;
	}

	/*
	 * If the mid is in the ignore list, EQMID error
	 */

	if (nxreq_ignore != -1) {
		mid = nxreq_ignore;
		do {
			if (amid == mid) {
				errno = EQMID;
				return -1;
			}
			mid = nxreq[mid].link;
		} while (mid != nxreq_ignore);
	}

	mid = amid;
	for (;;) {
		st = _probe_nxreq(&nxreq[mid], INFOCOPY,1);
		if (st != 1) {
			return st;
		}
		if (nxreq[mid].req == NX_RECV_REQ) {
			st = _check_gsend(&nxreq[mid]);
			if (st == -1) {
				return -1;
			}
		}
		next = nxreq[mid].link;
		nxreq[amid].link = next;

		if (mid != amid) {
			_free_mid(mid);
		}

		if (next == -1) {
			break;
		}
		mid = next;
	}
	_free_mid(amid);
	return 1;
}

long
_msgwait(mid)
	long	mid;
{
	long	st;

	/* This is a monitored request */
	nxreq[mid].monitored = 1;

	PERFMON_IDLE_START(_msgwait);
	for (;;) {
		st = _msgdone(mid);
		if (st == -1) {
		  break;
		}
		if (st == 1) {
		  st = 0;
		  break;
		}
		_flick();
	}
	PERFMON_IDLE_END(_msgwait);
	return st;
}

long
_msgignore(mid)
	long	mid;
{
	long 	amid;

	if (mid < 0 || mid >= NUM_NXREQ || nxreq[mid].state == NX_FREE) {
		errno = EQMID;
		return -1;
	}
	/*
	 * If the mid is merged, then need to make sure that the
	 * entire mid list is added to the nxreq_ignore pool. The
	 * amid will be the last mid in the list.
	 */

	amid = mid;
	while (nxreq[amid].link != -1) {
		amid = nxreq[amid].link;
	}
	if (nxreq_ignore == -1) {
		nxreq[amid].link = mid;
	} else {
		nxreq[amid].link = nxreq[nxreq_ignore].link;
		nxreq[nxreq_ignore].link = mid;
	}
	nxreq_ignore = mid;
	return 0;
}

long
_msgmerge(mid1, mid2)
	long	mid1;
	long	mid2;
{
	long	next;
	long	i,j;

	if (mid1 == -1) {
		return mid2;
	}
	if (mid2 == -1) {
		return mid1;
	}
	if (mid1 < 0 || mid1 >= NUM_NXREQ || nxreq[mid1].state == NX_FREE) {
		errno = EQMID;
		return -1;
	}
	if (mid2 < 0 || mid2 >= NUM_NXREQ || nxreq[mid2].state == NX_FREE) {
		errno = EQMID;
		return -1;
	}
	/*
	 * now check to see if there are any duplicates on the 2 lists.
	 * if there are then this is an error.
	 */
	for(i=mid1;i != -1;i=nxreq[i].link) {
		for(j=mid2;j != -1;j=nxreq[j].link) {
			if(i == j) {
				errno = EQMID;
				return -1;
			}
		}
	}
	/*
	 * no duplicates so continue on
	 */
 
	if (nxreq[mid1].link == -1) {
		nxreq[mid1].link = mid2;
		return mid1;
	}
	if (nxreq[mid2].link == -1) {
		nxreq[mid2].link = mid1;
		return mid2;
	}
	for (next = nxreq[mid1].link;
	     nxreq[next].link != -1;
	     next = nxreq[next].link);
	nxreq[next].link = mid2;
	return mid1;
}

long
_msgcancel(amid)
	unsigned long	amid;
{
	int	st;
	int 	mid, next;

	if (amid >= NUM_NXREQ ||
	    nxreq[amid].state == NX_FREE) {
		errno = EQMID;
		return -1;
	}
	PERFMON_IDLE_START(_msg_cancel);
	mid = amid;
	for(;;) {
	    st = 0;
	    if (nxreq[mid].state == NX_ACTIVE) {
		/* This is a monitored request. */
		nxreq[mid].monitored = 1;
		st = NX_ACTIVE;
		mcmsg_nx_cancel(&nxreq[mid], &st);
		while (st == NX_ACTIVE) {
			flick();
		}
	    }
	    next = nxreq[mid].link;
	    if (next == -1) {
		break;
            }
	    mid = next;
	}
	mid = amid;
	if ((nxreq[mid].state == NX_ACTIVE) && (st != NX_COMPLETE)) {
		/* This is a monitored request. */
		nxreq[mid].monitored = 1;
		for (;;) {
			st = _msgdone(mid);
			if (st != 0) {
				break;
			}
			_flick();
		}
	}
	PERFMON_IDLE_END(_msg_cancel);
	return st;
}

/*
 *	Routine:
 *		init_mcmsg_intf()
 *
 *	Purpose:
 *		Initialize the mcmsg_intf structure which contains the
 *		message passing interface info between the library and 
 *		the kernel, including applinfo, numnodes and mynode.
 *
 *	Returns:
 *		0 OK
 *		-1 error
 */

nodeinfo_t _nx_nodeinfo;

_init_mcmsg_intf()
{
	long		st;

	/*
	 * Wire _nodeinfo for the kernel.
	 */
	if ((st = mcmsg_wire(&_nx_nodeinfo, sizeof(_nx_nodeinfo))) == -1) {
		errno = EACCES;
		return -1;
	}
	if (st == -2) {
		errno = ENOMEM;
		return -1;
	}
	/*
	 * Get nodeinfo from the kernel.
	 */
	if (mcmsg_nx_nodeinfo(&_nx_nodeinfo) == -1) {
		errno = EQNOACT;
		return -1;
	}
	/*
	 * Init mcmsg_intf from kernel info.
	 */
	mcmsg_mynode = _nx_nodeinfo.mynode;
	mcmsg_numnodes = _nx_nodeinfo.numnodes;
	mcmsg_app = _nx_nodeinfo.applinfo.app;
	mcmsg_pid = getpid();
	mcmsg_pkt_size = _nx_nodeinfo.applinfo.pkt_size;
	mcmsg_memory_each = _nx_nodeinfo.applinfo.memory_each;
	mcmsg_send_threshold = _nx_nodeinfo.applinfo.send_threshold;
	mcmsg_send_count = _nx_nodeinfo.applinfo.send_count;
	mcmsg_give_threshold = _nx_nodeinfo.applinfo.give_threshold;
	mcmsg_process_lock = _nx_nodeinfo.applinfo.process_lock;
	/*
	 * Set ptype to INVALID_PTYPE for now.
	 */
	mcmsg_ptype = INVALID_PTYPE;
	
	return 0;
}


ipd_ptype_select(ipd_plist, ptype)
ipd_ptype_list_t *ipd_plist;
long	ptype;
{
register long		i;
register long		j;
register long		m;
register long		n;
register long		p;
register unsigned long	t;

	m = 0;
	n = ipd_plist->last_entry;
	i = j = n >> 1;
	for (;;) {
		p = ipd_plist->ptype_entry[i].ptype;
		if (ptype == p) {
			return i;
		}
		if (ptype > p) {
			if (i == n) {
				return i;
			}
			m = i;
		} else {
			if (i == m) {
				return i-1;
			}
			n = i-1;
		}
		j >>= 1;
		if (j == 0 && m != n) {
			j = 1;
		}
		i = m + j;
	}
}


ipd_ptype_add(ipd_plist, ptype, item, offset)
ipd_ptype_list_t *ipd_plist;
long	ptype;
long	item;
long	offset;
{
register long		i;
register long		j;
register long		n;
register long		a;

	/* Special case if this is the first entry being added */
	if (ipd_plist->last_entry == 0) {
		ipd_plist->ptype_entry[0].ptype = ptype;
		ipd_plist->ptype_entry[0].item = item;
		ipd_plist->ptype_entry[0].offset = offset;
		ipd_plist->ptype_entry[1].ptype = ptype + 1;
		ipd_plist->ptype_entry[1].item = 0;
		ipd_plist->last_entry = 1;
		return 1;
	}		

	n = ipd_plist->last_entry;
	i = ipd_ptype_select(ipd_plist, ptype);
	if (i >= 0 && ipd_plist->ptype_entry[i].item == item) {
		return 0;	/* Already there, same task */
	}
	if (i >= 0 && ipd_plist->ptype_entry[i].item != 0) {
		return -1;	/* Already there, trying to change task */
	}
	if (i > 0 &&
	    ipd_plist->ptype_entry[i-1].item == item &&
	    ipd_plist->ptype_entry[i].ptype == ptype) {
		if (i < n &&
		    ipd_plist->ptype_entry[i+1].ptype == ptype + 1) {
			if (ipd_plist->ptype_entry[i+1].item == item) {
				a = 2;
			} else {
				a = 1;
			}
			ipd_plist->last_entry = n - a;
			for (j = i; j <= n - a; j++) {
				ipd_plist->ptype_entry[j] = ipd_plist->ptype_entry[j+a];
			}
		} else {
			ipd_plist->ptype_entry[i].ptype = ptype + 1;
		}
		return 1;	/* Expand existing range up */
	}
	if (i < n &&
	    ipd_plist->ptype_entry[i+1].item == item &&
	    ipd_plist->ptype_entry[i+1].ptype == ptype + 1) {
		ipd_plist->ptype_entry[i+1].ptype = ptype;
		if (i >= 0 &&
		    ipd_plist->ptype_entry[i].ptype == ptype) {
			ipd_plist->last_entry = n - 1;
			for (j = i; j <= n - 1; j++) {
				ipd_plist->ptype_entry[j] = ipd_plist->ptype_entry[j+1];
			}
		}
		return 1;	/* Expand existing range down */
	}
	if (i >= 0 &&
	    ipd_plist->ptype_entry[i].ptype == ptype) {
		if (i == n ||
		    ipd_plist->ptype_entry[i+1].ptype != ptype + 1) {
			if (n == PTYPE_ENTRIES-1) {
				return -1;	/* No room */
			}
			ipd_plist->last_entry = n+1;
			for (j = n; j >= i; j--) {
				ipd_plist->ptype_entry[j+1] = ipd_plist->ptype_entry[j];
			}
			ipd_plist->ptype_entry[i+1].ptype = ptype + 1;
		}
		ipd_plist->ptype_entry[i].item = item;
		ipd_plist->ptype_entry[i].offset = offset;
		return 1;	/* Add one entry at start of range */
	}
	if (i < n &&
	    ipd_plist->ptype_entry[i+1].ptype == ptype + 1) {
		if (n == PTYPE_ENTRIES-1) {
			return -1;	/* No room */
		}
		ipd_plist->last_entry = n+1;
		for (j = n; j > i; j--) {
			ipd_plist->ptype_entry[j+1] = ipd_plist->ptype_entry[j];
		}
		ipd_plist->ptype_entry[i+1].ptype = ptype;
		ipd_plist->ptype_entry[i+1].item = item;
		ipd_plist->ptype_entry[i+1].offset = offset;
		return 1;	/* Add one entry at end of range */
	}
	if (i < n &&
	    ipd_plist->ptype_entry[i+1].item == 0) {
		if (n == PTYPE_ENTRIES-1) {
			return -1;	/* No room */
		}
		ipd_plist->last_entry = n+1;
		for (j = n; j > i; j--) {
			ipd_plist->ptype_entry[j+1] = ipd_plist->ptype_entry[j];
		}
		ipd_plist->ptype_entry[i+2].ptype = ptype + 1;
		ipd_plist->ptype_entry[i+1].ptype = ptype;
		ipd_plist->ptype_entry[i+1].item = item;
		ipd_plist->ptype_entry[i+1].offset = offset;
		return 1;	/* Add one entry and extend upper range */
	}
	if (n >= PTYPE_ENTRIES-2) {
		return -1;	/* No room */
	}
	ipd_plist->last_entry = n+2;
	for (j = n; j > i; j--) {
		ipd_plist->ptype_entry[j+2] = ipd_plist->ptype_entry[j];
	}
	ipd_plist->ptype_entry[i+1].ptype = ptype;
	ipd_plist->ptype_entry[i+1].item = item;
	ipd_plist->ptype_entry[i+1].offset = offset;
	ipd_plist->ptype_entry[i+2].ptype = ptype + 1;
	ipd_plist->ptype_entry[i+2].item = 0;
	return 1;	/* Add two entries */
}


void ipd_ptype_init(ipd_plist)
ipd_ptype_list_t	*ipd_plist;
{
	ipd_plist->method = 0 /* SELMETH_PTYPELIST */;
	ipd_plist->last_entry = 0;
	ipd_plist->ptype_entry[0].ptype = 0;
	ipd_plist->ptype_entry[0].item = 0;
	ipd_plist->ptype_entry[0].offset = 0;
}


/*
 *	_setptype(ptype)
 *
 *	Set the ptype of the current process.
 *	If first time through, do some parallel initialization.
 *
 *	Returns:
 *		0  on success
 *		-1 on error	(errno is set)
 */
long
_setptype(ptype)
	long		ptype;
{
	nodeinfo_t	*nodeinfo;
	long		st;
	long		status;

	/*
	 * Remove check for >= GLOBAL_BIT when parallel global send
	 * implemented in kernel.
	 */

	if ((ptype < 0)  || (ptype >= GLOBAL_BIT)) {
		errno = EQPID;
		return -1;
	}

	if (mcmsg_ptype == INVALID_PTYPE) {
		/*
		 * First time _setptype called by this process.
		 */

		/* XXX this should be temp - use nxreq_space for nodeinfo */
		if ((st = mcmsg_wire(nxreq_space, sizeof(nxreq_space))) == -1) {
			errno = EACCES;
			return -1;
		}
		if (st == -2) {
			errno = ENOMEM;
			return -1;
		}
		nodeinfo = (nodeinfo_t *)nxreq_space;

		st = mcmsg_nx_nodeinfo(nodeinfo);
		if (st == -1) {
			errno = EQNOACT;
			return -1;
		}

		mcmsg_post_page_init();

		st = _xinit(nodeinfo);
		if (st == -1) {
			return st;
		}

		ipd_ptype_init(&ipd_plist);
		ipd_msg_info.buffer_p = mcmsg_buffer;
		ipd_msg_info.buffer_size = mcmsg_buffer_size;
		ipd_msg_info.nxreq = nxreq;
		ipd_msg_info.nxreq_size = NUM_NXREQ;
		ipd_msg_info.nodenum = _mynode();
		ipd_msg_info.ipd_msg_aux = ipd_msg_aux;
		ipd_msg_info.ipd_plist = &ipd_plist;
		ipd_msg_info.num_ptypes = 0;
		ipd_msg_info.current_ptype = &mcmsg_ptype;
		ipd_msg_info.app_id = &mcmsg_app;
		ipd_msg_info_p = &ipd_msg_info;
		status = table(TBL_IPD_MSG_INFO, getpid(), &ipd_msg_info_p,
		               -1, sizeof(ipd_msg_info_p));
		if (status != 1) {
			printf("libnx: table() initialization error (1) = %d\n",
			       status);
		}

		st = 0;
		mcmsg_nx_reserve_ptype(ptype, 1, &st);
		while (st == 0) {
			flick();
		}
		if (st == -1) {
			errno = EQUSEPID;
			return st;
		}
		if (ipd_ptype_add(&ipd_plist, ptype, 1, 0) == 1) {
			ipd_msg_info.num_ptypes++;
		}

		mcmsg_ptype_range_low = ptype;
		mcmsg_ptype_range_high = ptype;

		/* init NX kernel port */
		st = nx_port_init();
		if (st != 0) {
			errno = EQNOACT;
			return -1;
		}

	} else {
		errno = EQSET;
		return -1;
#if 0
		if (ptype > mcmsg_ptype_range_high ||
		   ptype < mcmsg_ptype_range_low) {
		st = 0;
		mcmsg_nx_reserve_ptype(ptype, 1, &st);
		while (st == 0) {
			flick();
		}
		if (st == -1) {
			errno = EQUSEPID;
			return st;
		}
		if (ipd_ptype_add(&ipd_plist, ptype, 1, 0) == 1) {
			ipd_msg_info.num_ptypes++;
		}
		if (mcmsg_ptype_range_high == ptype-1) {
			mcmsg_ptype_range_high = ptype;
		} else if (mcmsg_ptype_range_low == ptype+1) {
			mcmsg_ptype_range_low = ptype;
		}
		}
#endif
	}
	mcmsg_ptype = ptype;

	return 0;
}


ptype_present(ptype)
long	ptype;
{
long	i;

	i = ipd_ptype_select(&ipd_plist, ptype);
	if (i >= 0 && ((ipd_plist.ptype_entry[i].item == 1) ||
			       (ipd_plist.ptype_entry[i].item == 2)))
		return 1;	/* Already there */
	else
		return 0;	/* Not present */
}


long
_flushmsg(typesel, node, ptype)
	long	typesel;
	long	node;
	long	ptype;
{
	return 0;
}

/*
 * Routines to implement a parallel global send
 */

/*
 * Utility routines
 */

#define MASKSIZE 16

long
power_of_two(ppow, prem)
	int *ppow;
	int *prem;
{
	int num;
	unsigned size;

	num = mcmsg_numnodes;

	if(_mynode() == num) {
		num++;                  /* if host node is sender then special */
	}

	*ppow = 0;
	size = 1;

	while (size < num) {
		size = size << 1;
		(*ppow)++;
	}

	if (size == num) {
		*prem = 0;
		return 1;
	}

	(*ppow)--;

	*prem = num - (size >> 1);


	if (*prem > 0) {
		return 0;
	}
	return -1;
}

toggle(inbits, bitt, poutbits)
	unsigned int inbits;
	int bitt;
	unsigned int *poutbits;
{
	unsigned int bmask;

	bmask = 1 << bitt;
	if ((inbits & bmask) >> bitt) {
		*poutbits = inbits & ~bmask;
	} else {
		*poutbits = inbits | bmask;
	}
}

unsigned int
bintogray(n)
	unsigned int n;
{
	int temp;

	temp = n << 1;
	return ((temp ^ n) >> 1);
}

static unsigned int mask[MASKSIZE] =
	{ 0x0001, 0x0002, 0x0004, 0x0008,
	  0x0010, 0x0020, 0x0040, 0x0080,
	  0x0100, 0x0200, 0x0400, 0x0800,
	  0x1000, 0x2000, 0x4000, 0x8000 };

unsigned int
graytobin(g)
	unsigned int g;
{
	int i, n;

	n = mask[MASKSIZE-1] & g;
	for (i = MASKSIZE-2; i >= 0; --i) {
		n += (mask[i] & g) ^ ((mask[i+1] & n) >> 1);

	}
	return (n);
}
/*
 *  Calling Sequence:
 *	count = treesend(node_list);
 *
 *  Description:
 *	Calculate node list for originator of broadcast
 *	using spanning tree algorithm.
 *
 *  Parameters:
 *	int node_list()	logical node list
 *
 *  Returns:
 *	int count		number of nodes in node_listp
 */
int
tree_send(node_list)
	int node_list[];
{
	long i;
	int node_count;
	unsigned int gdestproc;
	unsigned int destproc;

	unsigned int gval;
	int me;
	long pt;
	int pow, rem;

	me = mcmsg_mynode;
	pt = power_of_two(&pow, &rem);
	gval = bintogray(me);

	node_count = 0;

	if (pt == 1) {

		/*
		 * is a power of 2
		 */

		if (pow > 0) {

			for (i = pow-1; i >= 0; i--) {
				toggle(gval, i, &gdestproc);
				node_list[node_count++] = graytobin(gdestproc);

			}
		}
	} else {

		/*
		 * is not a power of 2
		 */

		if (me >= (1 << pow)) {

			/*
			 * Upper half node
			 */

			toggle(gval, pow, &gdestproc);
			node_list[node_count++] = graytobin(gdestproc);

		} else {

			/*
			 * Lower half node
			 */

			if (pow > 0) {
				for (i = pow -1; i >= 0; i--) {
					toggle(gval, i, &gdestproc);
					node_list[node_count++] = graytobin(gdestproc);
				}
			}
			toggle(gval, pow, &gdestproc);
			destproc = graytobin(gdestproc);
			if (destproc < mcmsg_numnodes) {
				node_list[node_count++] = destproc;
			}
		}
	}
	return node_count;
}

/*
 *  Calling Sequence:
 *	node_count = tree_recv(from_node, node_list);
 *
 *  Description:
 *	Calculate node list for receiver of broadcast message
 *	using spanning tree algorithm.
 *
 *  Parameters:
 *	int from_node   the node the message came from
 *	int node_list[] list of logical nodes to forward message
 *
 *  Returns:
 *	int node_count  number of nodes in node_list
 */
int
tree_recv(from_node, node_list)
	int from_node;
	int node_list[];
{
	int node_count;
	int i, me, pow, rem;
	int destproc;
	int uphalf;
	unsigned int gval, gsval, tmask, bitset, gdestproc;
	long pt;

	me = mcmsg_mynode;
	pt = power_of_two(&pow, &rem);

	gval = bintogray(me);
	uphalf = 0;
	if (me >= (1<<pow)) {
		uphalf = 1;
	}

	node_count = 0;

	if (!uphalf) {

		/*
		 * Lower half node
		 */

		gsval = bintogray(from_node);
		tmask = gval ^ gsval;
		if (tmask != 0) {
			bitset = 0;
			while (tmask != 1) {
				tmask = tmask >> 1;
				bitset++;
			}
			for (i = bitset-1; i >= 0; i--) {
				toggle(gval, i, &gdestproc);
				node_list[node_count++] = graytobin(gdestproc);
			}
		}

		if (pt != 1) {

			/*
			 * Not a power of 2
			 */

			toggle(gval, pow, &gdestproc);
			destproc = graytobin(gdestproc);
			if ((destproc < mcmsg_numnodes) &&
				(destproc != from_node)) {
				node_list[node_count++] = destproc;
			}
		}
	}
	return node_count;
}

/*
 * Message processor interface initialization
 */

#define INTEL_PGBYTES	4096
#define POST_EMPTY	0

typedef void	mcmsg_task_t;

void *mcmsg_post_page;
int mcmsg_post_page_in;
int mcmsg_msgp = 0;


extern void	mcmsg_post1();
extern void	mcmsg_post2();
extern void	mcmsg_post3();
extern void	mcmsg_post4();
extern void	mcmsg_post5();
extern void	mcmsg_post6();
extern void	mcmsg_post7();
extern void	mcmsg_post8();
extern void	mcmsg_post9();
extern void	mcmsg_post10();
extern void	mcmsg_call();

void	(*mcmsg_intf_vec1)() = mcmsg_post1;
void	(*mcmsg_intf_vec2)() = mcmsg_post2;
void	(*mcmsg_intf_vec3)() = mcmsg_post3;
void	(*mcmsg_intf_vec4)() = mcmsg_post4;
void	(*mcmsg_intf_vec5)() = mcmsg_post5;
void	(*mcmsg_intf_vec6)() = mcmsg_post6;
void	(*mcmsg_intf_vec7)() = mcmsg_post7;
void	(*mcmsg_intf_vec8)() = mcmsg_post8;
void	(*mcmsg_intf_vec9)() = mcmsg_post9;
void	(*mcmsg_intf_vec10)() = mcmsg_post10;


mcmsg_post_page_init()
{
	unsigned long p;
	int	i;

	p = 0;
	if (mcmsg_post_page == 0) {
		p = (unsigned long)calloc(INTEL_PGBYTES*2, 1);
		if (p != 0) {
			mcmsg_post_page = (void *)((p+INTEL_PGBYTES-1) &
					  ~(INTEL_PGBYTES-1));
		}
	}
	i = -1;
	if (mcmsg_post_page != 0) {
		mcmsg_post_page_in = 0;
		i = mcmsg_mp_access(mcmsg_post_page);
	}
	if (i == 0) {
		mcmsg_msgp = 1;
	} else {
		mcmsg_msgp = 0;
		if (p != 0) {
			free(p);
			mcmsg_post_page = 0;
		}
		mcmsg_intf_vec1 = mcmsg_call;
		mcmsg_intf_vec2 = mcmsg_call;
		mcmsg_intf_vec3 = mcmsg_call;
		mcmsg_intf_vec4 = mcmsg_call;
		mcmsg_intf_vec5 = mcmsg_call;
		mcmsg_intf_vec6 = mcmsg_call;
		mcmsg_intf_vec7 = mcmsg_call;
		mcmsg_intf_vec8 = mcmsg_call;
		mcmsg_intf_vec9 = mcmsg_call;
		mcmsg_intf_vec10 = mcmsg_call;
	}
	return 0;
}

/*
 *	Routine:
 *		_nx_main_thread
 *
 *	Arguments:
 *		None.
 *
 *	Description:
 *		This routine quickly determines whether it is running
 *		in the context of Thread 0 (the main user thread).
 *		It does this by verifying that a stack variable
 *		on the stack is within the boundaries of the stack
 *		space that is allocated for the first thread of a
 *		mach task.
 *
 *		The following mach definitions are used:
 *
 *		  USRSTACK is the top of the user stack.
 *		  DFLSSIZ  is the default size of a thread's stack
 *
 *	Returns:
 *		1 if this is the main thread (thread 0)
 *		0 if not
 */
#include <mach/i860/vm_types.h>
#include <sys/vmparam.h>

int
_nx_main_thread()
{
	unsigned long p, var;

	p = (unsigned long)&var;
	if (p < (unsigned long)USRSTACK && p > (unsigned long)USRSTACK-DFLSSIZ) {
		return 1;
	}
	return 0;
}
