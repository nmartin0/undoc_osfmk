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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: sys_vsocket.c,v $
 * Revision 1.36  1995/02/01  23:19:00  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.35  1994/11/18  20:51:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.34  1994/10/07  19:08:05  yazz
 * (Corrected CVS comments.)
 *
 * Revision 1.33  1994/10/07  18:30:29  yazz
 * Locked the socket structure for all references and traversals of that
 * socket's rvs chain.
 *
 *  Authors of fix: Nina Lepak, Jerry Toman
 *  Reviewer: hobbes, yazz, nina
 *  Risk: medium
 *  Benefit or PTS #: 10967 (more locking needed in vsocket code)
 *  Testing: TCP/IP EAT testing in several configurations
 *  Module(s): server/vsocket/sys_vsocket.c
 * 	    server/vsocket/vs_netops.c
 * 	    server/vsocket/vs_subr.c
 *
 * Revision 1.32  94/05/25  18:28:13  mjl
 * Call to vs_fionbio() in vsoo_ioctl() must have SOCKET_LOCK held.
 * 
 *  Reviewer: Nina Lepak <nina@locus.com>
 *  Risk: Low
 *  Benefit or PTS #: 9591
 *  Testing: NFS mount on non-netserver node worked
 *  Module(s): server/vsocket/sys_vsocket.c
 * 
 * Revision 1.31  1994/05/09  23:38:28  slk
 * Added new function vs_fionbio() to avoid duplication of code
 * in vsoo_ioctl() and vslo_ioctl().  Used vs_fionbio() in
 * vsoo_ioctl() to change the SS_NBIO flag in the socket state
 * if there is no local secondary on the primary network server
 * node.  This allows fcntl() to change a socket to nonblocking
 * on primary sockets with no local secondary socket.
 *
 *  Reviewer: Nina Lepak, Mike Leibensperger, John Litvin
 *  Risk: Medium, one file greater than 10 lines.
 *  Benefit or PTS #: #5950, accept() blocks on nonblocking socket.
 *  Testing: NONBLOCK test cases /home/sigeval/Bugs/noblock/jlitvin
 *  Module(s):
 *
 * Revision 1.30  1994/05/04  22:10:35  mjl
 * TNC select rewrite.  The vsoo_select() routine in particular has been
 * radically rewritten.  Highlights:
 *  - Unique integer select id identifies select call to remote secondaries,
 *     avoiding drp deadname tracking problems.
 *  - The "selid/drp map entry" (SDM) data structure localizes info about
 *     a particular select call.
 *  - File port svrref logic centralized in vsoo_select() enqueue and
 *     dequeue loops.
 *  - Fields in rvs chain items now begin with "rvs_".
 *  - ANSI prototypes throughout.
 *
 *  Reviewer: Charlie Johnson (Intel), Bob Yasi (Locus)
 *  Risk: Medium
 *  Benefit or PTS #: #7537 + select rewrite
 *  Testing: VSX, EATS, bobtest, Eval
 *  Module(s):
 * 	server/bsd/subr_select.c
 * 	server/sys/select.h
 * 	server/sys/socketvar.h
 * 	server/sys/user.h
 * 	server/tnc/un_debug.c
 * 	server/tnc/un_debug.h
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/bsd_server_side.c
 * 	server/uxkern/fsvr.defs
 * 	server/uxkern/fsvr2_server_side.c
 * 	server/uxkern/fsvr_port.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/port_hash.c
 * 	server/uxkern/port_hash.h
 * 	server/vsocket/mi_config.c
 * 	server/vsocket/sys_vsocket.c
 * 	server/vsocket/two_way_hash.h
 * 	server/vsocket/vs.defs
 * 	server/vsocket/vs_chouse.c
 * 	server/vsocket/vs_debug.c
 * 	server/vsocket/vs_init.c
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_subr.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vsocket.h
 *
 * Revision 1.29  1994/04/05  14:42:24  cfj
 * Merge revision 1.22.2.7 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.28  1994/03/14  16:43:40  cfj
 * Merged revision 1.22.2.6 from R1_2 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.27  1994/03/13  17:14:12  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7294, 6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_ipc.c,
 * 	vs_netops.c, vs_subr.c, sys_vsocket.c
 *
 * Revision 1.22.2.7  1994/04/05  14:37:14  cfj
 * Call ux_server_add_port() to put the sdrp into the ux_server_loop port set
 * so that send-once notifications get received and handled.
 *
 *  Reviewer:yazz@locus.com,jlitvin
 *  Risk:L
 *  Benefit or PTS #:7616
 *  Testing:testcase, VSX EAT
 *  Module(s):	server/vsocket/sys_vsocket.c
 * 		server/uxkern/fsvr_port.c
 *
 * Revision 1.22.2.6  1994/03/14  16:35:30  cfj
 * If def out the error printf when deallocating the deadname since it
 * is possible that another piece of code could allocate it before
 * vs_select_deadname() gets to it.
 *
 *  Reviewer:shala,yazz@locus.com
 *  Risk:L
 *  Benefit or PTS #:8506
 *  Testing:
 *  Module(s):server/vsocket/sys_vsocket.c
 *
 * Revision 1.22.2.4  1994/03/07  23:03:58  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:#7294/#6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_ipc.c,
 * 	sys_vsocket.c, vs_netops.c, vs_subr.c
 *
 * Deleted an obsolete comment. Checked that rvs is
 * marked VS_USE before using it in remote_if_ioctl().
 *
 * Revision 1.26  1994/03/09  01:14:30  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: lo
 *  Benefit or PTS #: #7052
 *  Testing: pvm runs for days; "bobtest" stress test runs also
 *  Module(s): server/vsocket/sys_vsocket.c
 *
 * In the case where a select() call with a timeout involved going off-node,
 * one of the deadname references was not deallocated, producing a prodigious
 * portleak.  This circumstance is now recognized and the extra ref deallocated.
 *
 * Revision 1.25  1994/03/03  19:26:39  slk
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 7016 merge from R1.2
 *  Testing: build and boot
 *  Module(s):
 *
 * Revision 1.22.2.3  1994/03/01  02:39:13  yazz
 *  Reviewer: Bernie Keany
 *  Risk: lo
 *  Benefit or PTS #: #7016
 *  Testing: extensive
 *  Module(s): server/vsocket/sys_vsocket.c
 *
 * Handle the SIOCSIFFLAGS ioctl command by informing the clearinghouse.
 *
 * Revision 1.24  1993/12/10  21:55:16  nina
 *  Reviewer:bolsen@locus.com, dbm@ssd.intel.com
 *  Risk:Medium
 *  Benefit or PTS #:#7424
 *  Testing:Lachman NFS main suite, various configurations
 *  Module(s):./server/vsocket/sys_vsocket.c
 *
 * Fixed bugs that prevented Paragons from being used
 * as NFS clients if the boot node is not a network
 * server node.  See #6831, #6719, #7421, #7422, #7423
 * #7424 and #7426.  If the clearinghouse node was
 * configured to be a node other than the bootnode,
 * the system would hang during system startup. This
 * was because a NORMA call was made to a node that
 * wasn't up yet.  sys_vsocket.c was changed to
 * use the function find_clearinghouse() before
 * making clearinghouse requests.
 *
 * Revision 1.23  1993/12/07  17:44:12  mjl
 *  Reviewer: cfj@ssd.intel.com, bhk@locus.com
 *  Risk: low
 *  Benefit or PTS #: 7272
 *  Testing: Locus network tests
 *  Module(s): server/vsocket/sys_vsocket.c, server/vsocket/vs_init.c,
 * 	server/vsocket/vs_subr.c, server/vsocket/vs_subr.h
 *
 * In vs_select_deadname(), fixed bogus lookup in the sdrp-soright/udrp-name
 * two-way hash table.  Lookup should be surrogate drp send-once right to
 * user drp name, not vice versa.  Also, renamed USER_DRP_TO_CBK_* hash
 * table macros to be UDRP_TO_VS_* and cleaned up some panic() messages.
 *
 * Revision 1.22  1993/10/30  15:19:12  nandy
 * Changed INCREMENT/DECREMENT_VS_REF_CNT to INCREMENT/DECREMENT_VSNET_REF_CNT
 * PTS : 6097
 * Reviewer: bhk@locus, cfj
 * Risk :  Low
 * Testing : Done
 *
 * Revision 1.21  1993/09/14  15:13:21  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.20.2.1  1993/09/14  15:11:43  cfj
 * Added a panic if the surrogate delay port is lost. (bhk@locus.com)
 * Part of fix for PTS bug #6097.
 *
 * Revision 1.20  1993/09/01  01:40:17  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.19  1993/08/09  15:54:23  nandy
 * "lost delay port" panic removed.
 *
 * Revision 1.18  1993/08/04  03:55:20  cfj
 * 08-03-93 Code drop from Locus.
 *
 * Revision 1.17  1993/07/30  15:40:00  cfj
 * Back out the modification that bhk made where the socket lock was
 * held across calls to r_vs_select_check() and r_vs_select_enqueue().
 *
 * Revision 1.16  1993/07/29  21:55:07  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server
 * slowdown.
 *
 * Revision 1.15  1993/07/28  21:23:36  cfj
 * Partial select() fix for PTS #5079.
 *
 * Revision 1.14  1993/07/16  20:47:15  hobbes
 * Added the HIPPI address resolution calls to the global_ioctl
 * logic .. enabling SSI in hippi_setmap.
 *
 * Revision 1.13  1993/07/14  18:48:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/09  15:07:19  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.5  1993/07/01  21:13:19  cfj
 * Adding new code from vendor
 *
 * Revision 1.12  1993/05/20  16:04:22  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 3.46  93/08/31  09:29:17  mjl
 * Tell which remote_vs_ioctl() failed in panic printf.
 * 
 * Revision 3.45  93/08/26  17:03:13  mjl
 * [LCCbug #0378] Eliminate length argument to remote_vs_ioctl() routine.
 * 
 * Revision 3.44  93/08/26  11:00:51  mjl
 * [LCCbug #0374, #0377; #0376 no longer reproducible; maybe fixes #0372]
 * Replace r_vs_ioctl() RPC's fixed length INOUT array argument with
 * two variable length array arguments, one IN and one OUT.  Prevents
 * server addressing exceptions that occured when all N bytes of fixed
 * arg weren't allocated in server's address space.  Also, when starting
 * readability-checking threads after setting FIONBIO, clear possible
 * EWOULDBLOCK error set as side effect of vs_soreadable().
 * 
 * Revision 3.43  93/08/23  02:41:49  bhk
 * Added recount protection when dequeueing selects. Fixes [#373]
 * 
 * Revision 3.42  93/08/23  00:15:59  mjl
 * [LCCbug #0370]  If socket now has default vsocket ops (vs->vs_data
 * is NULL), have vsoo_select() just call soo_select() to avoid leaking
 * option history chain entries.  Move creation of these entries to a
 * common routine.  In vs_select_deadname(), take a virtual socket ref
 * to prevent the socket from closing out from under us (NOTE: still
 * some problems with this!).
 * 
 * Revision 3.41  93/08/22  09:34:01  bhk
 * Closed an mbuf leak [#358]
 * Primed the readable cache for non-blocking I/O to prevent
 * hangs when garbage flags are non-blocking I/O is specified
 * by MSG_NONBLOCK in the flags field of a receive [#360]
 *  
 * Revision 3.40  93/08/19  15:07:33  bhk
 * Merged HIPPI support
 * Unified the create of a remote virtual socket [#349]
 * 
 * Revision 3.39  93/08/17  19:28:19  mjl
 * Add prototyped forward decls to nuke i860 compiler warnings.
 * The r_vs_socreate() RPC now has an OUT errno argument. Added
 * additional VSDEBERROR debug printfs.
 * 
 * Revision 3.38  93/08/16  18:52:42  bhk
 * Cleaned up comments.
 * Made getstate an asyncronous call [ # 346]
 * Cache readable state on getstate callback.
 * 
 * Revision 3.37  93/08/11  15:01:50  mjl
 * Tweek some debug printfs.
 * 
 * Revision 3.36  93/08/11  09:01:30  bhk
 * Checked for a surrogate deadname expiration before select_deadname does.
 * removes uselss code path in select.
 * Bug #336
 * 
 * Revision 3.35  93/08/09  15:52:14  bhk
 * Passed the callback port on remote socket creation
 * 
 * Revision 3.34  93/08/06  17:55:34  bhk
 * Fix to a possible lost delay panic
 * cleanup of warnings
 * rename of state flags
 * 
 * Revision 3.33  93/08/03  17:10:18  mjl
 * (a) Code cleanup, added comments.
 * (b) Replace direct calls to port hashing routines with macros from
 *	vs_subr.h.
 * (c) Use more general print_port_info() routine for port debug messages.
 * (d) Destroy the surrogate drp's receive right prior to calling remote
 *       or local select_dequeue() operations.  These operations will in
 *       turn wait for the sdrp to actually become dead.  This is a work
 *       around for serious tardiness in delivery of d-n notifications.
 * 
 * Revision 3.32  93/08/02  03:17:37  bhk
 * Multiple file servers now keep a server file reference per
 * secondary virtual socket to avoid race conditions with dead-names
 * 
 * Revision 3.31  93/08/01  22:45:04  bhk
 * Removed hashed delay port before the dequeue to close deadname window
 * Fixed problem with deadname notifications where the deadname was not
 * arriving at the target.
 * slowed the leak in the select queues
 * 
 * Revision 3.30  93/07/30  14:10:46  yazz
 * No longer unlock the secondary sockets (added #ifdef notdef). (yazz for bhk)
 * 
 * Revision 3.29  93/07/29  10:43:56  bhk
 * Closed a port leak from the previous checkin,
 * slowed  the expansion of select queue due to  lost enqueues
 * 
 * Revision 3.28  93/07/28  15:02:11  bhk
 * Allow select to requeue selects when select_wakeup wakes up a
 * socket when there is nothing to do.  Closed a window between the
 * time a socket was checked for events and the select was enqueued
 * where an event could happen and not be detected
 * 
 * Revision 3.27  93/07/07  10:35:37  mjl
 * [LCC #0314] Call net_threadstart_deferred() to start deferred network
 * threads when a network interface has been succesfully configured.
 * 
 * Revision 3.26  93/06/29  15:43:29  bhk
 * Upgrade to the new scheme of holding a file reference when select is called
 * This fixes Bugs 233 and 264
 * 
 * Revision 3.25  93/05/07  19:14:00  mjl
 * Don't start protocol timeout processing threads until a network interface
 * is successfully configured.  Also, preliminary work for LCC bug #0256.
 * 
 * Revision 3.24  93/05/07  15:19:19  nina
 * global_ioctl() was modified to use the new function
 * find_network_servers().  Emulate single system semantics
 * for SIOCDARP ahd SIOCSARP.  
 * 
 * Revision 3.23  93/05/05  22:33:33  mjl
 * Change vslo_ioctl() for MIv3 style of adding clearinghouse info, i.e. first
 * reserve a clearinghouse entry, then do the ifioctl(), then confirm the
 * new clearinghouse entry or else reset it.
 * 
 * Revision 3.22  93/05/04  16:46:19  bhk
 * Fixed RCS comment
 * 
 * Revision 3.21  93/05/03  14:46:48  bhk
 * Fixed select enqueue bug(241), multiserver bind/connect bug(242),
 * workaround for delayed deadname notifications of canceled selects.
 * 
 * Revision 3.20  93/04/16  14:43:19  mjl
 * Start pffasttimo and pfslowtimo threads prior to configuring an interface.
 * Add ux_server_thread_{,un}blocking() around if_check_in() RPC.
 * 
 * Revision 3.19  93/04/12  15:51:36  nina
 * Made changes to support single system semantics for SIOCGIFCONF,
 * SIOCADDRT and SIOCDELRT.
 * 
 * Revision 3.18  93/04/03  11:52:31  klh
 * Split select into three parts (check, enqueue, dequeue).
 * Enhance state transition detection. (klh for bhk)
 * 
 * Revision 3.17  93/03/24  14:39:08  bhk
 * added ux_server_thread_blocking to getstate remote operation
 * 
 * Revision 3.16  93/03/19  19:51:20  bhk
 * Fixed the deadlock bug (#206) added release and recapture of the
 * master lock to the macro which is called be fore a remote operation.
 * 
 * Revision 3.15  93/03/19  17:28:58  bhk
 * The Select Fix.  major changes to the way select operates
 * on remote network servers, Select now checks before
 * enqueing outstanding requests to see if the request needs to
 * be enqueued or is just a dummy attempt to see if the request
 * was satisfied
 * 
 * Revision 3.14  93/03/03  17:18:28  mjl
 * Remove bogus panic in remote_if_ioctl() --- should just pass back errno.
 * Also, add extern decl for net_interface_name() to please picky compilers.
 * 
 * Revision 3.13  93/03/02  14:07:34  bhk
 * Added framework for the SIOCGIFCONF ioctl
 * 
 * Revision 3.12  93/02/25  17:53:36  nina
 * Function-ship all interface ioctls to the network server node specified in
 * the interface name prefix.  (mjl)
 * 
 * Revision 3.11  93/02/10  17:08:25  klh
 * Fix RCS comments
 * 
 * Revision 3.10  93/02/08  16:52:59  bhk
 * Fixed problem where selects where incrementing the file port when they were
 * canceled.
 * 
 * Revision 3.9  93/01/04  20:41:17  bhk
 * added transaction ID to remote IPC socket calls
 * 
 * Revision 3.8  92/10/27  17:43:32  bhk
 * Returned the sense command to an indirection of the protosw field in the
 * socket.
 * 
 * Revision 3.7  92/09/24  17:09:11  bhk
 * fixed select timeout problem
 * 
 * Revision 3.6  92/07/26  17:47:25  bhk
 * Cleaned up debug
 * Added support for select on remote sockets
 * Added code to remove the requirement to modify protocol
 * stacks.  (Stacks can be dropped in unchanged)
 * 
 * Revision 3.5  92/06/23  15:32:58  chrisp
 * [Bug #32] Fix for select() looping.
 * 
 * Revision 3.4  92/06/22  12:13:32  bhk
 * Fixed ioctl for multiserver configurations
 * got select working remotely
 * 
 * Revision 3.3  92/06/16  18:43:59  bhk
 * fixed return values, queue handling in vs_ioctl
 * 
 * Revision 3.2  92/04/20  17:29:44  bhk
 * Added remote virtual socket code for select and ioctl
 * 
 * Revision 3.1  92/03/20  17:07:59  bhk
 * moved vsocket.h to vsocket directory
 * 
 * Revision 3.0  92/03/04  14:56:22  bhk
 * Genesis	bhk
 * 
 * Revision 2.2  91/08/31  13:23:29  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:55:32  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.14  90/10/31  13:49:49  devrcs
 * 	Rearrange soo_write so it builds with SEC_ARCH on.
 * 	[90/10/10  12:00:38  tmt]
 * 
 * 	Separate NDELAY and NONBLOCK.
 * 	[90/10/12  10:16:53  jvs]
 * 
 * 	fix recent regression that made B1SECURITY
 * 	version of this code not compile
 * 	[90/10/11  22:21:01  hosking]
 * 
 * Revision 1.13  90/10/07  13:19:35  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:51:54  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:00:33  gm]
 * 
 * 	Pass nonblock flag to sosend/soreceive from soo_write/soo_read.
 * 	Don't set SS_NBIO on pipes, use above instead.
 * 	[90/09/29  17:09:41  tmt]
 * 
 * Revision 1.12  90/09/13  11:42:26  devrcs
 * 	Remove issig() glue, now sosleep handles it.
 * 	[90/08/28  11:49:12  tmt]
 * 
 * Revision 1.11  90/08/24  11:19:01  devrcs
 * 	Must call issig() unconditionally if sosleep indicates EINTR.
 * 	[90/08/20  10:10:04  tmt]
 * 
 * 	Make soo_read/write do issig without longjmp.
 * 	[90/08/20  07:14:34  gmf]
 * 
 * 	(It would be desirable for the networking to use tsleep)
 * 	[90/08/20  03:38:00  gmf]
 * 
 * Revision 1.10  90/07/27  08:44:45  devrcs
 * 	Update to BSD Reno release.
 * 	Modify soo_read and soo_write to conform.
 * 	[90/07/20  12:44:16  tmt]
 * 
 * Revision 1.9  90/07/05  23:08:03  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	[90/07/03  18:39:41  tmt]
 * 
 * Revision 1.8  90/06/22  20:07:14  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:54:10  seiden]
 * 
 * 	Call issig() before longjmp'ing to rwuio, since sosleep
 * 	cannot (does not) do so. Pipe reads (e.g.) were not
 * 	restarting because of this bug.
 * 	[90/06/12  18:01:41  tmt]
 * 
 * 	Rearrange sec_sobufcount w/locks to avoid panic.
 * 	[90/06/11  16:32:49  tmt]
 * 
 * 	Use IOCGROUP macro. Remove unneded #includes. Take FP_LOCK in close.
 * 	[90/06/09  17:57:27  tmt]
 * 
 * 	nags merge
 * 	[90/06/12  21:16:56  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:41:01  seiden]
 * 
 * 	Remove MMAX_MP tokens and replace with PARALLEL_SELECT.
 * 	Restore old code in select, add unix_master around signals.
 * 	[90/06/06  14:06:58  tmt]
 * 
 * Revision 1.7  90/04/27  18:53:03  devrcs
 * 	Check error return of sosend/soreceive, they no longer longjmp.
 * 	[90/04/20  12:10:22  tmt]
 * 
 * Revision 1.6  90/04/14  00:30:03  devrcs
 * 	Add Robert Coren's poll interface, with #ifdef for !MACH.
 * 	De-lint and rearrange two things too.
 * 	[90/04/10  13:31:58  tmt]
 * 
 * Revision 1.5  90/03/27  13:14:56  gm
 * 	Filesystem parallelization changes [noemi]
 * 
 * Revision 1.4  90/01/18  08:42:11  gm
 * 	Do sopriv before ioctl to refresh SS_PRIV bit
 * 	[89/01/08  15:39:05  tmt]
 * 
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/03  11:51:00  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:26:55  gm]
 * 
 * Revision 1.2  89/12/26  09:21:51  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)sys_socket.c	2.1 16:10:28 4/20/90 SecureWare, Inc. */
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
 * Copyright (c) 1982, 1986, 1990 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	sys_socket.c	7.5 (Berkeley) 5/9/89
 *	Merged: sys_socket.c	7.8 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"
#if	MACH
#include <sys/secdefines.h>
#endif

#include "sys/param.h"
#include "sys/systm.h"
#include "sys/ioctl.h"
#include "sys/user.h"
#include "sys/file.h"
#include "sys/uio.h"
#include "sys/stat.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "vsocket/vsocket.h"
#include "sys/socketvar.h"
#include "sys/domain.h"
#include "sys/protosw.h"

#include "net/if.h"
#include "net/if_llc.h"
#include "net/if_hippi.h"
#include "net/route.h"
#include "vsocket/vs_types.h"
#include "vsocket/vs_types_gen.h"
#include "vsocket/vs_chouse.h"
#include "vsocket/vs_mig.h"
#include "vsocket/vs_subr.h"
#include "uxkern/bsd_types.h"
#include "uxkern/port_hash.h"
#include "uxkern/syscall_subr.h"
#include <mach/kern_return.h>

LOCK_ASSERTL_DECL

extern node_t		this_node;
extern mach_port_t	clearinghouse_port;
extern mach_port_t	vsfindserver();

#define Forward		extern

Forward int vsoo_read	(struct file *, struct uio *, struct ucred *);
Forward int vsoo_write	(struct file *, struct uio *, struct ucred *);
Forward int vsoo_ioctl	(struct file *, int, caddr_t);
Forward int vsoo_select	(struct file *, short *, short *, int);
Forward int vsoo_close	(struct file *);

Forward int remote_if_ioctl	(node_t, struct socket *, int, caddr_t);
Forward int global_ioctl	(struct socket *, int, caddr_t);
Forward int vslo_ioctl	(struct socket *, int, caddr_t, boolean_t);
Forward void vs_fionbio	(struct socket *, boolean_t);

extern int vs_optimize_for_local_ns;

CONST struct	fileops vsocketops =
    { vsoo_read, vsoo_write, vsoo_ioctl, vsoo_select, vsoo_close };


/* ARGSUSED */
int
vsoo_read(
	struct file	*fp,
	struct uio	*uio,
	struct ucred	*cred)
{
	int flags = 0;
	struct socket *so;

	VSDEBUG(VSDEBENTRY,("vsoo_read: &fp 0x%x &uio 0x%x &cread 0x%x\n",
			fp,uio,cred));

	BM(FP_LOCK(fp));
	if (fp->f_flag & (FNDELAY|FNONBLOCK))
		flags = MSG_NONBLOCK;
	BM(FP_UNLOCK(fp));

	so = (struct socket *)fp->f_data;
	return VSOP_RECEIVE(so,(struct mbuf **)0, uio, 
		(struct mbuf **)0, (struct mbuf **)0, &flags);
}

/* ARGSUSED */
int
vsoo_write(
	struct file	*fp,
	struct uio	*uio,
	struct ucred	*cred)
{
#if	SEC_ARCH
	struct mbuf *control = 0;
	int error;
#endif
	struct socket *so;
	int flags = 0;

	VSDEBUG(VSDEBENTRY,("vsoo_write: &fp 0x%x &uio 0x%x &cread 0x%x\n",
			fp,uio,cred));

	BM(FP_LOCK(fp));
	if (fp->f_flag & (FNDELAY|FNONBLOCK))
		flags = MSG_NONBLOCK;
	BM(FP_UNLOCK(fp));

	so = (struct socket *)fp->f_data;
#if	!SEC_ARCH
	return VSOP_SEND((struct socket *)fp->f_data, (struct mbuf *)0,
		uio, (struct mbuf *)0, (struct mbuf *)0, flags);
#else	/* SEC_ARCH */
	if (error = sec_internalize_rights(&control))
		return error;
	return VSOP_SEND(so, (struct mbuf*)0, uio, (struct mbuf *)0,
		control , flags);
#endif
}


int
vsoo_ioctl(
	struct file	*fp,
	int		cmd,
	register caddr_t data)
{
	register struct socket *so = (struct socket *)fp->f_data;
	int	rval;
	int error = 0;
	int tmperror = 0;
	register vs_socket_t 	*rvs;
	struct uthread		*uth = &u;
	struct ifreq		*ifr = (struct ifreq *)data;
	struct server_oip *oipp = &u.uu_oip;
	node_t			ns_node;
	node_t			*np;
	int			i;
	boolean_t		has_local_secondary = FALSE;
	extern char		*net_interface_name();

	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vsoo_ioctl: &fp 0x%x cmd 0x%x &data 0x%x\n",
			fp,cmd,data));

	/*
	 *  If no TNC data is attached, someone must have called
	 *  set_default_vsops() on this socket, so use non-TNC fileops
	 *  too.
	 */

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	if (so->vs_data == NULL) {
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		return (soo_ioctl(fp, cmd, data));
	}

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);

	switch ( IOCGROUP(cmd) ) {
	case 'i':
		/*
		 * If this is a GIFCONF, we want to enforce single
		 * system semantics
		 */
		switch(cmd) {
	
		case SIOCGIFCONF:
#ifdef COMPAT_43
		case OSIOCGIFCONF:
#endif
		case SIOCDARP:
		case SIOCSARP:
		case SIOCSHART:
		case SIOCDHART:
			return(global_ioctl(so, cmd, data));
			/*NOTREACHED*/

		default:
			/*
			 * Other interface commands must be function shipped
			 * to the specified network server node.
			 */
			if(net_interface_name(ifr->ifr_name,
					      NULL,&ns_node) == NULL) {
				VSDEBUG(VSDEBERROR,
					("vsoo_ioctl: bad interface %s\n",
					 ifr->ifr_name));
				return (ENXIO);
			}
			if (ns_node == this_node) {
				error = vslo_ioctl(so, cmd, data, FALSE);
				return error;
			} else {
				error = remote_if_ioctl(ns_node, so, cmd, data);
				return error;
			}
			/*NOTREACHED*/
		}

	case 'r':
		/*
		 * If this is a route command, enforce single
		 * system semantics.
		 */
		error = global_ioctl(so, cmd, data);
		return error;
		/*NOTREACHED*/
	}

	/*
	 * If we get here, this is not an 'i' or 'r' group ioctl.
	 */

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	/* Remember the local single server optimization */
	if(!(rvs=(vs_socket_t *)so->vs_data) || !(so->vs_flags & VS_ISBOUND)) {
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		return vslo_ioctl(so,cmd,data,TRUE);
	}

	for ( ; rvs ; rvs = rvs->rvs_next ) {
		if(!(rvs->rvs_flags & VS_USE))
			continue;
		if(rvs->rvs_server_port == MACH_PORT_NULL) {
			has_local_secondary = TRUE;
		} else {
			VSOP_SET_FORW(oipp, rvs->rvs_server_port);
			tmperror = remote_vs_ioctl(rvs->rvs_server_port,
						   uth->uu_procp->p_cred,
						   uth->uu_oip.oip_transid,
						   cmd, data, &rval);
			VSOP_END_FORW(oipp);
			if (tmperror == ESUCCESS)
				tmperror = rval;
		}
		if(tmperror)
			error = tmperror;
	}

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);

	if (has_local_secondary == TRUE) {
		tmperror = vslo_ioctl(so,cmd,data,TRUE);
		if(tmperror)
			error = tmperror;
	}

	switch (cmd) {
		case FIONBIO:
			if (has_local_secondary == FALSE) {
				DOMAIN_FUNNEL(sodomain(so), f);
				SOCKET_LOCK(so);
				vs_fionbio(so, *(int *)data);
				SOCKET_UNLOCK(so);
				DOMAIN_UNFUNNEL(f);
			}
			break;
	}

	return error;
}


int
vslo_ioctl(
	struct socket		*vs,
	int			cmd,
	register caddr_t	data,
	boolean_t		queue_opts)
{
	int			s, error = 0;
	mach_port_t		netserv_port = MACH_PORT_NULL;
	struct ifreq		*ifr;
	struct ifnet		*ifp;
	kern_return_t		kr;
	iface_info_t		ifinfo;
	chouse_key_t		chskey;
	int			want_threads = 0;
	int			found;

	DOMAIN_FUNNEL_DECL(f)
	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);

	switch (cmd) {

	case FIONBIO:
		vs_fionbio(vs, *(int *)data);
		break;

	case FIOASYNC:
		SOCKBUF_LOCK(&vs->so_rcv);
		SOCKBUF_LOCK(&vs->so_snd);
		if (*(int *)data) {
			vs->so_state |= SS_ASYNC;
			vs->so_rcv.sb_flags |= SB_ASYNC;
			vs->so_snd.sb_flags |= SB_ASYNC;
		} else {
			vs->so_state &= ~SS_ASYNC;
			vs->so_rcv.sb_flags &= ~SB_ASYNC;
			vs->so_snd.sb_flags &= ~SB_ASYNC;
		}
		SOCKBUF_UNLOCK(&vs->so_snd);
		SOCKBUF_UNLOCK(&vs->so_rcv);
		break;

	case FIONREAD:
#if	SEC_ARCH
		*(int *)data = sec_sobufcount(&vs->so_rcv, vs);
#else
		SOCKBUF_LOCK(&vs->so_rcv);
		*(int *)data = vs->so_rcv.sb_cc;
		SOCKBUF_UNLOCK(&vs->so_rcv);
#endif
		break;

	case SIOCSPGRP:
		vs->so_pgid = *(int *)data;
		break;

	case SIOCGPGRP:
		*(int *)data = vs->so_pgid;
		break;

	case SIOCATMARK:
		*(int *)data = (vs->so_state&SS_RCVATMARK) != 0;
		break;

	case SIOCSIFADDR:
	case SIOCAIFADDR:
		/*
		 *  Maybe registering a new network server with the
		 *  clearinghouse.  If we succeed, we'll want to start
		 *  the timeout processing threads.
		 */
		want_threads++;
		netserv_port = vsfindserver(sodomain(vs)->dom_family);
		/*FALLTHRU*/

	case SIOCSIFNETMASK:
	case SIOCSIFDSTADDR:
	case SIOCSIFBRDADDR:
#ifdef	NOTYET
	case SIOCDIFADDR:
#endif
	case SIOCSIFFLAGS:
		/*
		 *  The clearinghouse must be informed of all ioctls
		 *  that add, modify, or delete interface addressing
		 *  info.  We must do this prior to calling ifioctl()
		 *  to make sure that no two interfaces are configured
		 *  with the same address.
		 */
		ifr = (struct ifreq *)data;
		ifp = ifunit(ifr->ifr_name);
		if (ifp == NULL) {
			error = ENXIO;
			break;
		}

		/* Clearinghouse lookup key. */
		chskey.ck_id = (int)ifp;
		chskey.ck_af = ifr->ifr_addr.sa_family;
		chskey.ck_node = this_node;

		s = find_clearinghouse();
		if (s != ESUCCESS) {
			VSDEBUG(VSDEBERROR,
			("vslo_ioctl: find_clearinghouse: error %d\n",s));
			return(s);
		}			
		ux_server_thread_blocking();
		kr = if_addr_update(clearinghouse_port,
				    (char_p_t)&chskey,
				    ifp->if_flags,
				    cmd,
				    (char_p_t)ifr,
				    netserv_port,
				    (char_p_t)&ifinfo,
				    &error);
		ux_server_thread_unblocking();
		if (kr != KERN_SUCCESS)
			panic("vslo_ioctl: if_addr_update: kr 0x%x\n",
			      kr);
		if (error == ESUCCESS) {
			/* Found a clearinghouse entry. */
			found = 1;
		} else if (error == ENOENT) {
			/* No clearinghouse entry. */
			found = 0;
		} else
			break;

		queue_opts = FALSE;
		sopriv(vs);
		error = ifioctl(vs, cmd, data);
		if (error == ESUCCESS && found) {
			/*
			 *  All went well with the ioctl, so confirm
			 *  the interface configuration change.  (This
			 *  propagates the change to corresponding MI
			 *  interfaces on other network server nodes.)
			 */
		   
			ux_server_thread_blocking();
			kr = if_confirm_update(clearinghouse_port,
					       (char_p_t)&chskey,
					       cmd,
					       (char_p_t)ifr,
					       &s);
			ux_server_thread_unblocking();
			if (kr != KERN_SUCCESS)
			    panic("vslo_ioctl: if_confirm_update: kr 0x%x\n",
				  kr);
			if (s != ESUCCESS)
			    printf("vslo_ioctl: if_confirm_update: error %d\n",
				   s);
			if (want_threads) {
				/*
				 *  This is now a network server node, so
				 *  start the deferred network threads.
				 */
				s = splimp();
				net_threadstart_deferred();
				splx(s);
			}
		} else if (found) {
			/*
			 *  Error doing the ioctl, so restore clearinghouse
			 *  to its previous state.
			 */
			ux_server_thread_blocking();
			kr = if_addr_reset(clearinghouse_port,
					   (char_p_t)&chskey,
					   (char_p_t)&ifinfo,
					   &s);
			ux_server_thread_unblocking();
			if (kr != KERN_SUCCESS)
				panic("vslo_ioctl: if_addr_reset: kr 0x%x\n",
				      kr);
			if (s != ESUCCESS)
				printf("vslo_ioctl: if_addr_reset: error %d\n",
				       s);
		}
		break;

	default:
		/*
		 * Interface/routing/protocol specific ioctls:
		 * interface and routing ioctls should have a
		 * different entry since a socket's unnecessary
		 * However, socket SS_PRIV bit serves as auth.
		 */
		sopriv(vs);
		if (IOCGROUP(cmd) == 'i') {
			error = ifioctl(vs, cmd, data);
			queue_opts = FALSE;
		} else if (IOCGROUP(cmd) == 'r') {
			error = rtioctl(vs, cmd, data);
			queue_opts = FALSE;
		} else
			error = ((*vs->so_proto->pr_usrreq)(vs, PRU_CONTROL, 
				(struct mbuf *)cmd, 
				(struct mbuf *)data, (struct mbuf *)0));
		break;
	}
	if (error == ESUCCESS && queue_opts) {
		error = create_vs_opts(vs, VS_OPT_IOCTL, 0, 
				       cmd, IOCPARM_LEN(cmd), data);
	}
	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	return (error);
}


#include "sys/poll.h"

/* Note: must not set revents when scanning == 0 */
int
vsoo_select(
	struct file		*fp,
	short			*events,
	short			*revents,
	int			scanning)
{
	register struct socket	*so = (struct socket *)fp->f_data;
	struct uthread		*uth = current_thread();
	struct server_oip	*oipp = &uth->uu_oip;
	int			error = ESUCCESS;
	kern_return_t		kr;
	mach_port_t		server_port;
	short			myevents;
	mach_port_t		delay_port = MACH_PORT_NULL;
	vs_socket_t		*rvs;
	int			rvs_count;
	int			refs_taken;
	int			need_a_ref;
	int			enqueued_count;
#define dn_cancelled		enqueued_count
	int			isreadable;
	selid_drp_map_t		*sdm = NULL;
	DOMAIN_FUNNEL_DECL(f)
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	if ((rvs=(vs_socket_t *)so->vs_data) == NULL ||
	    (vs_optimize_for_local_ns && !(so->vs_flags & VS_IS_REMOTE))) {
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		VSDEBUG(VSDEBSELECT, ("vsoo_select: short circuited!\n"));
		return  soo_select(fp,events,revents,scanning);
	}

	VSDEBUGX(VSDEBFILEPORT,
		 print_port_info(uth->uu_sel_file_port, "File_port"));
	VSDEBUGX(VSDEBPORT,
		 (print_port_info(uth->uu_sel_delay_port, "Delay_port"),
		  print_port_info(uth->uu_procp->p_cred, "Credentials")));

	/* We are always a primary socket here. */
	ASSERT((so->vs_flags & VS_IS_SHADOW) == 0);

	if(!scanning) 
		goto notscanning;

	ASSERT((mach_port_t)fp == uth->uu_sel_file_port);

	VSDEBUG(VSDEBSELECT,
		("-> vsoo_select(0x%x) %sscan selflg=0x%x sid=0x%x drp=0x%x "
		 "evt=0x%x revt=0x%x\n\tidx=%d vs_flag=0x%x\n",
		 fp, (uth->uu_sel_again ? "re" : ""),
		 uth->uu_sel_flags, uth->uu_sel_id, uth->uu_sel_delay_port,
		 *events, *revents, uth->uu_sel_index, so->vs_flags));

	/*
	 *  Set the uarea select flags to inform all subsequent
	 *  select activity in this thread that we are dealing
	 *  with distributed vsocket selects and that
	 *  uu_sel_delay_port is the real drp.
	 */
	uth->uu_sel_flags |= (SQ_VSOCK|SQ_DRP);

	/* 
	 * for each of the secondary virtual sockets, query for a response
	 * and queue the request if none
	 */
	for (rvs = (vs_socket_t *)so->vs_data; rvs; rvs = rvs->rvs_next) {
		if ((rvs->rvs_flags & VS_USE) == 0)
			continue;

		myevents = 0;
		if(rvs->rvs_server_port == MACH_PORT_NULL) {
			vs_select_check(so,*events,&myevents);
		} else {
			VSOP_SET_FORW(oipp, rvs->rvs_server_port);
			kr = r_vs_select_check(rvs->rvs_server_port,
				uth->uu_procp->p_cred,
				uth->uu_oip.oip_transid,
				*events,
				&myevents,
				&rvs->rvs_state,
				&rvs->rvs_soerror,
				&rvs->rvs_qlen,
				&isreadable);
			VSOP_END_FORW(oipp);
			if (kr != KERN_SUCCESS) {
				error = vs_map_error(kr);
			} else if (isreadable) {
				rvs->rvs_flags |= VS_READABLE;
			}
		}
		*revents |= myevents;
	}
	vs_collapse_state(so);	/* combine new state information */
#ifdef	FUTURE_ERROR_HANDLING_FIXES_IN_PLACE
	if ( !error && (error = so->so_error) != ESUCCESS ) {
		VSDEBUG(VSDEBERROR,
			("vsoo_select: scan posted errno %d on so 0x%x\n",
			 error, so));
	}
#endif

	/*
	 *  If a selected event or an error occurred, or if we are
	 *  responding to a select that has otherwise been satisfied
	 *  (i.e. this is an "immediate" select), then we are done.
	 */
	if ( *revents || error || uth->uu_sel_index == -1 ) {
		VSDEBUGX(VSDEBSELFREF, vs_frefs(fp, "   !Q'd"));
		VSDEBUG(VSDEBSELECT,
		    ("<- vsoo_select(0x%x) !Q'd, revts=0x%x err=%d idx=%d\n",
		     fp, *revents, error, uth->uu_sel_index));
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		return error;
	}

	/*
	 *  Otherwise we will need to enqueue select entries.
	 *  Remember the delay port, since OSF base code may
	 *  clear it from the uth area.
	 */
	delay_port = uth->uu_sel_delay_port;

	ASSERT(uth->uu_sel_id != SEL_ID_NULL || uth->uu_sel_again == FALSE);

	if (uth->uu_sel_again == FALSE) {
		/*
		 *  This is a new select call.  Create a new entry in
		 *  the selid/drp map for it.  This will return a
		 *  pointer to the map entry, which contains the
		 *  unique select id assigned to this distributed
		 *  select call.  The map entry exists for the
		 *  duration of this select.  It holds both a ref on
		 *  the primary vsocket, and a file port
		 *  svrref+sright.  After calling this macro the map
		 *  entry itself has two refs; when we are done with
		 *  it here we must unref it once via
		 *  SELID_DRP_LOOKUP_DONE().
		 */
		SELID_DRP_INSERT(fp, delay_port, sdm);
		uth->uu_sel_id = sdm->sdm_selid;
		VSDEBUG(VSDEBSELECT,
		    ("   vsoo_select(0x%x) assigned sid 0x%x, sdm 0x%x\n",
		     fp, uth->uu_sel_id, sdm));
		VSDEBUGX(VSDEBSELFREF, vs_frefs(fp, "   1st Q"));
	} else {
		/*
		 *  This is not the first call to FOP_SELECT(), i.e.
		 *  we are called from the sbsd_sel_poll_reply() RPC
		 *  after a select_wakeup() has happened.  Locate the
		 *  selid/drp map entry; if it is gone, another thread
		 *  must have cleaned up so we are done.
		 */
		ASSERT(uth->uu_sel_id != SEL_ID_NULL);
		SELID_DRP_LOOKUP(uth->uu_sel_id, sdm);
		if (sdm != NULL) {
			delay_port = sdm->sdm_drp;
			ASSERT(delay_port == uth->uu_sel_delay_port);
		} else {
			/* 
			 *  Can't find the drp for this select id, so
			 *  someone must have already cleaned up.
			 */
			VSDEBUG(VSDEBSELECT,
			    ("<- vsoo_select(0x%x) selid 0x%x not mapped\n",
			     fp, uth->uu_sel_id));
			SOCKET_UNLOCK(so);
			DOMAIN_UNFUNNEL(f);
			return ESUCCESS;
		}
		VSDEBUGX(VSDEBSELFREF, vs_frefs(fp, "   re-Q"));
	}


	/*
	 *  Let's enqueue some selects.
	 *
	 *  One fp->f_svrsend ref (svrref) is needed for each of the N
	 *  secondary sockets, plus one to store in the selid/drp map.
	 *  The ..._delay() RPC created on svrref at the beginning of
	 *  the select, and this one will be put in the map.  If this
	 *  is a requeue (again == TRUE), we already have one file
	 *  port svrref that came to us via _reply(), so only N-1 are
	 *  needed.
	 */
	need_a_ref = !uth->uu_sel_again;
	refs_taken = 0;
	enqueued_count = 0;
	for ( rvs = (vs_socket_t *)so->vs_data, rvs_count = 0;
	      rvs && (uth->uu_sel_flags & SQ_DEAD) == 0; /* quit if drp dead */
	      rvs = rvs->rvs_next ) {

		if ((rvs->rvs_flags & VS_USE) == 0)
			continue;
		rvs_count++;	/* Count useable rvs entries. */

		/*
		 *  Take a svrref for this secondary if we need one.
		 */
		if ( need_a_ref ) {
			kr = mach_port_insert_right(mach_task_self(),
						    uth->uu_sel_file_port,
						    uth->uu_sel_file_port,
						    MACH_MSG_TYPE_MAKE_SEND);
			if (kr != KERN_SUCCESS) {
				VSDEBUG((VSDEBERROR|VSDEBSELECT),
					("vsoo_select: m_p_insert_right: "
					 "fp 0x%x, kr 0x%x\n",
					 uth->uu_sel_file_port, kr));
				SOCKET_UNLOCK(so);
				DOMAIN_UNFUNNEL(f);
				return vs_map_error(kr);
			}
			fp_ref_port_svr(fp);
			refs_taken++;
		}

		if (rvs->rvs_server_port == MACH_PORT_NULL) {
			/*
			 *  If primary socket is also a secondary,
			 *  enqueue a select queue entry there.
			 */
			ASSERT((caddr_t)rvs == so->vs_data);
			select_enqueue(&so->so_rcv.sb_selq);
			if (uth->uu_sel_delay_port == MACH_PORT_NULL) {
				enqueued_count++;
				need_a_ref = TRUE;
			} else {
				need_a_ref = FALSE;
			}
		} else {
			need_a_ref = remote_vs_select_enqueue(rvs,
							      *events,
							      enqueued_count);
			if ( need_a_ref )
				enqueued_count++;
		}
	}

	/*
	 *  Now every secondary has a file port svrref!
	 *
	 *  If we don't need a ref at the end of the enqueue loop, and
	 *  we made some here, then we made one too many.  (If we
	 *  didn't take any additional svrrefs here, ..._reply() or
	 *  ..._delay() will want the one we've got, so we don't clean
	 *  it up.)
	 */
	if ( need_a_ref == FALSE && refs_taken ) {
		VSDEBUG(VSDEBSELECT, ("   fp=0x%x sid=0x%x too many svrrefs\n",
				      fp, uth->uu_sel_id));
		fp_unref_port(fp, -1);
		refs_taken--;
	}

#if	MACH_ASSERT
	if (uth->uu_sel_again == FALSE) {
		/* From sbsd_sel_poll_delay()... */
		ASSERT(enqueued_count == refs_taken);
		ASSERT(enqueued_count == rvs_count);
	}
#endif

	/*
	 *  Post-enqueue processing.
	 */
	VSDEBUGX(VSDEBSELFREF, vs_frefs(fp, "   postQ"));
	VSDEBUG(VSDEBSELECT,
	  ("<- vsoo_select(0x%x) %sscan drp=0x%x %sdead, %d queued, sdm=0x%x\n",
	   fp, (uth->uu_sel_again ? "re" : ""), uth->uu_sel_delay_port,
	     ((uth->uu_sel_flags & SQ_DEAD) ? "" : "not "),
	     enqueued_count, sdm));
	if (enqueued_count != 0) {
		/* Tell caller we enqueued something. */
		uth->uu_sel_delay_port = MACH_PORT_NULL;
	}
	if (uth->uu_sel_flags & SQ_DEAD) {
		/* Force a subsequent FOP_SELECT() cleanup call. */
		if (error == ESUCCESS)
			error = EBADF;
	} else if (enqueued_count == 0) {
		/*
		 *  This clause is possibly redundant, since the only
		 *  circumstance where nothing would be enqueued is when
		 *  the drp has gone dead.  XXX Maybe should be a panic?
		 */
		printf("vsoo_select/scan: nothing enqueued but drp not dead, "
		       "uu_selid/drp %x/%x, sdm 0x%x\n",
		       uth->uu_sel_id, delay_port, sdm);
		ASSERT(uth->uu_sel_delay_port != MACH_PORT_NULL);
		SDM_SCRUB(sdm);	/* Next SDM_UNREF call scrubs the map entry. */
	}
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	SELID_DRP_LOOKUP_DONE(sdm);

	return error;

notscanning:

	/* 
	 *  We get here when we need to cleanup any select queues.
	 *  We won't be called if:
	 *	- this is an immediate select (index == -1),
	 *  	- an error occured on the scanning part of the select, or
	 *  	- the scanning part of the select found an event.
	 */

	VSDEBUG(VSDEBSELECT,
		("-> vsoo_select(0x%x) clnup selflg=0x%x sid=0x%x drp=0x%x "
		 "evt=0x%x revt=0x%x\n\tidx=%d vs_flag=0x%x\n",
		 fp, uth->uu_sel_flags, uth->uu_sel_id, uth->uu_sel_delay_port,
		 *events, *revents, uth->uu_sel_index, so->vs_flags));

	ASSERT((uth->uu_sel_flags & (SQ_VSOCK|SQ_DRP)) == (SQ_VSOCK|SQ_DRP));

	/*
	 *  If no selid assigned, then the select must have been
	 *  immediately satisfied.  If there is a selid but no
	 *  selid/drp map entry for it, some other thread must
	 *  have cleaned up.  Either way there's no work for us.
	 */
	if (uth->uu_sel_id != SEL_ID_NULL) {
		SELID_DRP_LOOKUP(uth->uu_sel_id, sdm);
		uth->uu_sel_delay_port = delay_port =
			(sdm ? sdm->sdm_drp : MACH_PORT_NULL);
	}
	if (uth->uu_sel_id == SEL_ID_NULL || delay_port == MACH_PORT_NULL) {
		VSDEBUG(VSDEBSELECT,
		    ("<- vsoo_select(0x%x) no-op, sid=%x drp=0x%x sdm=0x%x\n",
		     fp, uth->uu_sel_id, delay_port, sdm));
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		return ESUCCESS;
	}

	ASSERT(uth->uu_sel_index != -1);
	VSDEBUGX(VSDEBSELFREF, vs_frefs(fp, "   DQing"));

	/*
	 *  A select_wakeup() may have discovered that the drp went
	 *  dead and left a bit set in the selid/drp map to tell us so.
	 */
	if (sdm->sdm_flags & SDM_DEAD) {
		VSDEBUG(VSDEBSELECT, ("   vsoo_select(0x%x) SDM_DEAD\n", fp));
		uth->uu_sel_flags |= SQ_DEAD;
	}

	/*
	 *  Now remove all entries for this select id from the select
	 *  queues of the secondary sockets.
	 */
	dn_cancelled = FALSE;
	uth->uu_sel_flags &= ~SQ_DEQUEUED;
	for (rvs = (vs_socket_t *)so->vs_data; rvs; rvs = rvs->rvs_next) {
		if (!(rvs->rvs_flags & VS_USE)) 
			continue;

		if (rvs->rvs_server_port == MACH_PORT_NULL) {

			/* Gets rid of local file svrref iff drp is dead. */
			select_dequeue(&so->so_rcv.sb_selq);

			/*
			 *  We can assume that d-n notification was cancelled
			 *  even if SQ_DEQUEUED is not set, because we *did*
			 *  queue something here before, and if we did not
			 *  just dequeue something then a wakeup must have
			 *  occurred.  Either way d-n msgs are cancelled.
			 */
			dn_cancelled = TRUE;

			/*
			 *  If the drp was *not* dead and we are called from
			 *  a remote reply, we have to get rid of the svrref
			 *  that the local selq held.  This ensures that
			 *  ..._reply() will have one and only one svrref to
			 *  get rid of, as it expects.
			 */
			if ((uth->uu_sel_flags &
			     (SQ_DEAD|SQ_REMOTE_REPLY|SQ_DEQUEUED))
			    == (SQ_REMOTE_REPLY|SQ_DEQUEUED)) {
				VSDEBUG(VSDEBSELECT,
				    ("   fp=0x%x sid=0x%x clnup local svrref\n",
				     fp, uth->uu_sel_id));
				fp_unref_port(fp, -1);
			}
		} else {
			/* Always gets rid of remote file svrref. */
			remote_vs_select_dequeue(rvs, &dn_cancelled);
		}
	}
	vs_collapse_state(so);
#ifdef	FUTURE_ERROR_HANDLING_FIXES_IN_PLACE
	if ( (error = so->so_error) != ESUCCESS ) {
		VSDEBUG(VSDEBERROR,
			("vsoo_select: dequeue posted errno %d on so 0x%x\n",
			 error, so));
	}
#else
	error = ESUCCESS;
#endif

	/*
	 *  The select call is over, so clean up the selid/drp map.  A
	 *  scrubbed entry can't be seen by lookups, and will go away
	 *  when we call SELID_DRP_LOOKUP_DONE().
	 */
	SDM_SCRUB(sdm);

	/*
	 *  If the delay port has become a dead name and all secondary
	 *  sockets are remote, we have to deallocate it here because 
	 *  no local call to select_dequeue() was made (which would
	 *  deallocate the dead drp in a non-TNC system).
	 */
	if (uth->uu_sel_flags & SQ_DEAD) {
		if (((vs_socket_t *)so->vs_data)->rvs_server_port) {
			VSDEBUG(VSDEBSELECT,
				("   sid=0x%x: dealloc drp=%x, no local rvs\n",
				 uth->uu_sel_id, uth->uu_sel_delay_port));
			kr = mach_port_deallocate(mach_task_self(),
						  uth->uu_sel_delay_port);
			if (kr != KERN_SUCCESS) {
				VSDEBUG((VSDEBERROR|VSDEBSELECT),
					("vsoo_select/clnup: drp dealloc: "
					 "drp 0x%x selid 0x%x kr 0x%x\n",
					 uth->uu_sel_delay_port,
					 uth->uu_sel_id, kr));
			}
		}
	}

	VSDEBUGX(VSDEBSELFREF, vs_frefs(fp, "   postDQ"));
	VSDEBUG(VSDEBSELECT,
		("<- vsoo_select(0x%x) clnup, %sdead\n",
		 fp, ((uth->uu_sel_flags & SQ_DEAD) ? "was " : "")));

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	SELID_DRP_LOOKUP_DONE(sdm);

	return error;
#undef	dn_cancelled
}


vsoo_stat(so, ub)
	register struct socket *so;
	register struct stat *ub;
{
	int error = 0;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vsoo_stat: &so 0x%x &ub 0x%x\n", so,ub));

	bzero((caddr_t)ub, sizeof (*ub));
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	error = ((*so->so_proto->pr_usrreq)(so, PRU_SENSE,
	    (struct mbuf *)ub, (struct mbuf *)0, 
	    (struct mbuf *)0));
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}


int
vsoo_close(
	struct file		*fp)
{
	register struct socket	*so;
	int			error = ESUCCESS;

	VSDEBUG(VSDEBENTRY,("vsoo_close: &fp 0x%x\n",fp));
	/*
	 * We don't need to lock fp here because soo_close is only
	 * called on the last close of the socket.
	 */
	if (so = (struct socket *)fp->f_data)
		error = VSOP_CLOSE(so);
	FP_LOCK(fp);
	fp->f_data = 0;
	FP_UNLOCK(fp);
	VSDEBUGX(VSDEBFILEPORT, print_port_info(fp, "closing::"));
	return (error);
}


/*
 *  This routine function ships an interface ioctl to a particular
 *  network server node.  We use the remove virtual socket (rvs) chain
 *  to keep track of which nodes we already have a remote socket on;
 *  if we haven't yet got one on the nsnode, we create one.
 */
int
remote_if_ioctl(
	node_t			ns_node,
	struct socket		*so,
	int			cmd,
	caddr_t			data)
{
	vs_socket_t		*rvs;
	mach_port_t		ns_port;
	mach_port_t		remote_port;
	struct socket		*remote_addr;
	mach_port_t		vs_port;
	int			rc;
	kern_return_t		kr;
	struct uthread		*uth = current_thread();
	struct server_oip	*oipp = &uth->uu_oip;
	struct ifconf		*ifc;
	mach_msg_type_name_t	used;
	caddr_t			datap;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	/*
	 *  Find any existing remote socket for ns_node.
	 */
	for ( rvs = (vs_socket_t *)so->vs_data; rvs; rvs = rvs->rvs_next ) {
		if ( !(rvs->rvs_flags & VS_USE) )
			continue;
		if (ns_node == vs_rvs_node_number(rvs))
			break;
	}
	if ( rvs == NULL ) {
		/*
		 *  No remote virtual socket on ns_node, so make one.
		 */
		rvs = vs_get_new_rvs(so);
		if (rvs == NULL) {
			rc = ENOMEM;
			goto out;
		}
		rc = tnc_get_server_port(ns_node, &ns_port);
		if (rc != ESUCCESS)
			goto out;
		VSOCK_TO_PORT_LOOKUP(so,vs_port);
		VSOP_SET_FORW(oipp, ns_port);
		kr = r_vs_socreate(ns_port,
				   uth->uu_procp->p_cred,
				   vs_port,
				   uth->uu_oip.oip_transid,
				   sodomain(so)->dom_family,
				   &remote_port,
				   so->so_type,
				   so->so_proto->pr_protocol,
				   (int *) &remote_addr,
				   &rc);
		VSOP_END_FORW(oipp);
		if (kr != ESUCCESS) {
			VSDEBUG(VSDEBERROR,
				("remote_if_ioctl: r_vs_socreate: kr 0x%x\n",
				 kr));
			rc = vs_map_error(kr);
		}
		if (rc != ESUCCESS) {
			VSDEBUG(VSDEBERROR,
				("remote_if_ioctl: r_vs_socreate: rc %d\n",
				 rc));
			VS_FREE(rvs, VSM_RVS);
			goto out;
		}
		ASSERT(remote_port != MACH_PORT_NULL);
		rvs->rvs_server_port	= remote_port;
		rvs->rvs_server_node	= ns_node;
		rvs->rvs_remote_so	= remote_addr;
		rvs->rvs_state		= so->so_state;
		rvs->rvs_qlen		= so->so_qlen;
		rvs->rvs_soerror	= so->so_error;
		/* XXX Should I call replay_vs_opts(so, rvs) here? -mjl */
	}

	/*
	 *  Now we have the rvs that we need to function ship
	 *  the ioctl.
	 */

	switch(cmd) {

	case SIOCGIFCONF:
#ifdef COMPAT_43
	case OSIOCGIFCONF:
#endif
		ifc = (struct ifconf *)data;
		used = ifc->ifc_len;
		VSOP_SET_FORW(oipp, rvs->rvs_server_port);
		kr = r_vs_global_ioctl(rvs->rvs_server_port,
				uth->uu_procp->p_cred,
				uth->uu_oip.oip_transid,
				cmd,
				ifc->ifc_len,
				&datap,
				&used,
				&rc);
		VSOP_END_FORW(oipp);

		if (kr != KERN_SUCCESS)
		    panic("remote_if_ioctl: r_vs_global_ioctl: kr %d (0x%x)\n",
			  kr, kr);
		if (!rc) {
			copyout(datap, ifc->ifc_buf, used);
			vm_deallocate(mach_task_self(),
					(vm_address_t)datap,
					used);
			ifc->ifc_len = used;
		}
		break;

	default:
		VSOP_SET_FORW(oipp, rvs->rvs_server_port);
		kr = remote_vs_ioctl(rvs->rvs_server_port,
				     uth->uu_procp->p_cred,
				     uth->uu_oip.oip_transid,
				     cmd, data, &rc);
		VSOP_END_FORW(oipp);
		if (kr != KERN_SUCCESS)
			panic("remote_if_ioctl: remote_vs_ioctl: cmd=0x%x, "
			      "kr=0x%x\n", cmd, kr);
		break;
	}
 out:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (rc);
}


/*
 * Name:
 *	global_ioctl()
 *
 * Function:
 *	Emulate single system semantics for SIOCGIFCONF, OSIOCGIFCONF,
 *	SIOCSARP, SIOCDARP, and 'r' group ioctls
 */
int
global_ioctl(
	struct socket	*so,
	int		cmd,
	caddr_t		data)
{
	node_t		*np;
	int             nsnodes;	/*number of network server nodes*/
	int		nodes_returned; 
	int		node_num;
	node_t		nodes[MAX_SERVERS];
	mach_port_t	ports[MAX_SERVERS];
	int		i, rc;
	int		cnt;	
	int		used;
	kern_return_t	kr;
	char		*datap;
	struct ifconf	*ifc = (struct ifconf *)data;
	/*
	 * Get a list of all the network servers
	 */
	rc = find_network_servers(MAX_SERVERS,
				  &nsnodes,
				  &nodes_returned,
				  nodes,
				  ports);

	/*
	 * If nsnodes == 0, there are no network servers
	 * configured.  If there are not network servers
	 * configured, there are no network interfaces,
	 * no routes, etc.
	 */
	if(nsnodes == 0) {
		return(0);
	}
			
	/*
	 * Do the right thing, depending on which ioctl we're
	 * processing.
	 */
	switch(cmd) {
		
	case SIOCGIFCONF:
#ifdef COMPAT_43
	case OSIOCGIFCONF:		
#endif
		/* the number of bytes user handed over */
		cnt = ifc->ifc_len;

		/* save the original pointer to our data buffer */
		datap = ifc->ifc_buf;

		/* used to keep track of how many bytes we use */
		used = 0;

		/* get the ifnet blocks */
		for(i=0, np = nodes;
		    (i < nodes_returned) && (cnt >= sizeof(struct ifreq));
		    i++, np++){
			if(*np == INVALID_NODE)
				break;

			if(*np == this_node)
				rc = vslo_ioctl(so,cmd,data,FALSE);
			else
				rc = remote_if_ioctl(*np,so,cmd,data);

			if(rc != ESUCCESS)
				continue;

			/*
			 * The function ifconf modifies the ifc_len field. On
			 * return, it contains the number of bytes that were
			 * used to return a particular node's ifnet blocks.
			 * Before we make the next remote ioctl call, we need
			 * to adjust ifc_buf pointer and the ifc_len field so
			 * as not to write over the info we've already fetched.
			 */
			ifc->ifc_buf += ifc->ifc_len;
			used += ifc->ifc_len;
			ifc->ifc_len = cnt - used;
		}

		/*
		 * The user expects the total number of bytes consumed to
		 * be in ifc_len.  The user also doesn't expect to have
		 * the ifc_buf pointer altered, so ...
		 */
		ifc->ifc_len = used;
		ifc->ifc_buf = datap;
		break;
	
	
	case SIOCADDRT:
	case SIOCDELRT:
	case SIOCSARP:
	case SIOCDARP:
	case SIOCSHART:
	case SIOCDHART:
		/* ship the cmd to all network nodes */
		for(i=0, np = nodes; (i < nodes_returned); i++, np++) {
			if(*np == INVALID_NODE)
				break;

			if(*np == this_node)
				rc = vslo_ioctl(so,cmd,data,FALSE);
			else
				rc = remote_if_ioctl(*np,so,cmd,data);

			if(rc != ESUCCESS)
				continue;
		}
		break;

	default:
		rc = EINVAL;
		break;

	}

	return(rc);
}

/* 
 * Set or clear the SS_NBIO flag in the socket state.
 * The SOCKET_LOCK must be held, because vs_soreadable() requires it.
 */
void
vs_fionbio(
	struct socket	*vs,
	boolean_t	nbio)
{
	LOCK_ASSERT("vs_fionbio", SOCKET_ISLOCKED(vs));

	if (vs->so_special & SP_PIPE) {
		/* Handled by soo_read/write */
		VSDEBUG(VSDEBMISC, 
			("vs_fionbio: Handled by read/write.\n"));
		return;
	}
	if (!nbio) {
		VSDEBUG(VSDEBMISC, 
			("vs_fionbio: unset SS_NBIO.\n"));
		vs->so_state &= ~SS_NBIO;
		return;
	}

	VSDEBUG(VSDEBMISC, ("vs_fionbio: set SS_NBIO.\n"));
	vs->so_state |= SS_NBIO;

	/*
	 *  If there are multiple secondaries, do a
	 *  readability check to set VS_READABLE bits
	 *  in the rvs chain if any of the secondaries
	 *  are readable.  Priming the cache, so to speak.
	 *  (If no secondary is readable, we must be sure
	 *  to clear the EWOULDBLOCK error, otherwise
	 *  subsequent send()'s will fail!)
	 */
	if (vs->vs_flags & VS_IS_MULTI) {
		(void)vs_soreadable(vs, TRUE);
		switch (vs->so_error) {
			case EWOULDBLOCK:
				vs->so_error = ESUCCESS;
				/* FALL THROUGH */
			case ESUCCESS:
				break;
			default:
				VSDEBUG(VSDEBERROR,
				("vs_fionbio: FIONBIO: vs_soreadable errno=%d\n",
					vs->so_error));
				break;
		}
	} 
}
