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
 * HISTORY
 * $Log: vs_netops.c,v $
 * Revision 1.45  1995/02/11  00:05:58  stans
 *  'lint' picking with typedefs for a clean compile.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.44  1995/02/01  23:29:01  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.43  1994/12/23  01:46:50  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:10356
 *  Testing:Test case and TCP/IP EATS with various network configs
 *  Module(s):
 * 	net/rtsock.c
 * 	netinet/in_proto.c
 * 	vsocket/vs_chouse.h
 * 	vsocket/vs_chouse.c
 * 	vsocket/vs.defs
 * 	vsocket/vs_ipc.c
 * 	vsocket/vs_subr.c
 * 	vsocket/vs_netops.c
 *
 *   The problem was only seen in systems with multiple network configurations
 *   when a user attempted to bind to INADDR_ANY/port 0. In this
 *   situation, the system is supposed to pick the port number for
 *   the caller. The problem occurred because the VSOCKET code
 *   didn't ensure that the primary socket and each secondary socket used
 *   the same port number.
 *
 *   Now, a bind to port 0 results in an RPC to the clearinghouse to
 *   pick a port number, based on the domain and protocol of the socket.
 *
 * Revision 1.42  1994/11/18  20:52:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.41  1994/10/07  19:09:06  yazz
 * (Corrected CVS comments.)
 *
 * Revision 1.40  1994/10/07  18:31:41  yazz
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
 * Revision 1.39  1994/09/20  18:34:32  nina
 *  Reviewer:nina, hobbes
 *  Risk:Medium
 *  Benefit or PTS #:10818
 *  Testing:TCP/IP eats and PTS test case
 *  Module(s):
 * 	./server/vsocket/vs_ipc.c
 * 	./server/vsocket/vs_netops.c
 *
 *  Update and check the aggregate socket state before
 *  deciding whether or not to sleep waiting for an event.
 *
 * Revision 1.38  1994/09/02  20:13:09  nina
 *  Reviewer:mjl, hobbes
 *  Risk:Low
 *  Benefit or PTS #:7412
 *  Testing:TCP/IP EATS
 *  Module(s):vs_netops.c
 *
 * listen() didn't work on an unbound socket. Do an
 * implicit bind() in vs_netsolisten() if necessary.
 * Fix socket locking in vs_netsolisten().
 *
 * Revision 1.37  1994/08/31  22:48:09  mtm
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
 * Revision 1.36  1994/08/26  21:16:55  yazz
 * Merged R1_3 revisions 1.34.2.2 & 1.34.2.3 into main stem.  (for nina)
 *
 * Revision 1.34.2.3  1994/08/24  00:06:06  yazz
 *  Corrected build problem; SOCK_LOCK => SOCKET_LOCK.
 *
 * Revision 1.34.2.2  1994/08/23  15:08:50  nina
 *  Reviewer:yazz, hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7618, 10383. See also 7155 and 10114.
 *  Testing:NFS EATS, TCP/IP EATS
 *  Module(s):
 * 	server/sys/socketvar.h
 * 	server/bsd/uipc_socket.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 *
 * 	cmds_libs/usr/src/sbin/portmap/Makefile
 * 	cmds_libs/usr/src/sbin/portmap/portmap.c
 *
 * 	cmds_libs/usr/src/sbin/mountd/Makefile
 * 	cmds_libs/usr/src/sbin/mountd/mountd.c
 *
 * 	cmds_libs/usr/src/sbin/nfsd/Makefile
 * 	cmds_libs/usr/src/sbin/nfsd/nfsd.c
 *
 * 	cmds_libs/usr/src/sbin/nfsiod/Makefile
 * 	cmds_libs/usr/src/sbin/nfsiod/nfsiod.c
 *
 * 	cmds_libs/usr/src/ccs/lib/libnx/Makefile
 * 	cmds_libs/usr/src/ccs/lib/libnx/tnc_subs.c (NEW)
 *
 * 	cmds_libs/sbin/init.d/nfs
 *
 *
 * Modifications have been made to the server, libnx, the NFS
 * daemons and nfs start/stop script so that each configured
 * network server node runs its own copy of the NFS daemons.
 *
 * Added support for the VS_BINDLOCAL option. Fixed some socket
 * locking problems.
 *
 * Revision 1.35  1994/08/19  18:52:35  mjl
 * Merge revision 1.34.2.1 into main trunk.
 *
 * Revision 1.34.2.1  1994/08/19  17:29:02  mjl
 * Remove bogus VSDEBUG() macro from vs_netsoreceive().
 *
 *  Reviewer: Bob Yasi <yazz@locus.com>
 *  Risk: Low
 *  Benefit or PTS #: 10658
 *  Testing: None
 *  Module(s): server/vsocket/vs_netops.c
 *
 * Revision 1.34  1994/07/22  00:27:55  yazz
 *  Author of fix: Mike Leibensperger (mjl)
 *  Reviewer: Bob Yasi
 *  Risk: lo
 *  Benefit or PTS #: 9420 C-1
 *  Testing: Developed testcase now passes; experimental also passes at cust site.
 *  Module(s): server/vsocket/vs_netops.c
 *
 *
 * In vs_netsodequeue() retain a ptr to head of rvs chain so head entry's
 * so_qlen value can be zeroed when an ENOTCONN error is encountered.
 *
 * Revision 1.33  1994/06/15  17:27:17  mjl
 * Break race between close of remote virtual socket (MARK_VSOCK_CLOSING() macro)
 * and asynchronous status checking RPCs (PORT_TO_VSOCK_LOOKUP_ASYNC() macro).
 *
 *  Reviewer: Bob Yasi <yazz@locus.com>, Charlie Johnson <cfj@ssd.intel.com>
 *  Risk: Medium
 *  Benefit or PTS #: 9024
 *  Testing: NFS mounts on HiPPI configurations now succeed.
 *  Module(s):
 * 	server/sys/socketvar.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/sys_vsocket.c
 *
 * Revision 1.32  1994/05/27  18:04:16  mjl
 * In vs_netsorecv(), test SO_ACCEPTCONN to see if listen(2) has been called
 * on the socket (and therefore reads should faile w/ ENOTCONN).  Previously
 * the SS_ISCONNECTED/ISCONNECTING flags were used [PTS 8789]; this doesn't
 * work because these bits might not be valid on non-co-located primaries.
 *
 *  Reviewer: Nina Lepak <nina@locus.com>
 *  Risk: Low
 *  Benefit or PTS #: 9630
 *  Testing: Run the "haupt" program on a non-netserver node.
 *  Module(s): server/vsocket/vs_netops.c
 *
 * Revision 1.31  1994/05/12  17:18:07  nina
 *  Reviewer:mjl
 *  Risk:Low
 *  Benefit or PTS #:8789, 9404
 *  Testing:Test cases specified for #8789
 *  Module(s):./server/vsocket/vs_netops.c
 *
 *  For 8789, return ENOTCONN if a recv() is attempted
 *  on an unconnected socket that requires a connection.
 *  For 9404, return EAFNOSUPPORT if a connect() is attempted
 *  and the specified family in the socket addr is not AF_INET.
 *
 * Revision 1.30  1994/05/04  22:22:34  mjl
 * Merge revision 1.24.2.4 from R1_2 branch into main trunk.
 *
 * Revision 1.29  1994/04/29  23:20:36  yazz
 *  Reviewer: Bob Yasi
 *  Risk: Lo
 *  Benefit or PTS #: #8733
 *  Testing: Testcase now passes
 *  Module(s): server/tnc/vs_netops.c
 *
 * The variable rval was being incorrectly checked in vs_netsoconnect().
 * Now only error is checked.  Also fixed some routine names in dbg msgs.
 *
 * Revision 1.28  1994/04/13  19:05:51  chrisp
 * Remove blank line introduced in RCS history following rev 1.25 check-in.
 *
 *  Reviewer: None
 *  Risk: Very low
 *  Benefit or PTS #: 8959
 *  Testing: Compiles
 *  Module(s):
 *
 * Revision 1.27  1994/03/17  18:51:41  nina
 *  Reviewer:hobbes
 *  Risk:Low
 *  Benefit or PTS #:8441
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_subr.c,
 * 	vs_netops.c
 *  Merge changes for #8441 from R1.2 in to R1.3
 *
 * Revision 1.24.2.4  1994/04/27  23:08:05  yazz
 *  Reviewer: Charlie Johnson, Bob Yasi
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
 * TNC select rewrite.  Make vs_netsoclose() obey vsocket ref counting.
 * Introduce netsocreate1(), used by both vs_netsocreate() and
 * vs_netsodequeue().  Rework vs_netsodequeue().  Get rid of too-loud
 * debug printfs.  Add domain funnel to vs_netsobind().  Use ANSI
 * prototypes.  Other code cleanup.
 *
 * Revision 1.24.2.3  1994/03/16  18:46:13  nina
 *  Reviewer:hobbes
 *  Risk:Low
 *  Benefit or PTS #:8441
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_subr.c,
 * 	vs_netops.c
 *
 *  The function vs_netsorecv() calls vs_soreadable() twice. The
 *  purpose of the second call is to start a thread on a remote
 *  network server to wait for more data and notify the server
 *  if and when any data arrives. In this capacity, vs_soreadable()
 *  will set so_error to EWOULDBLOCK. Clear so_error to prevent
 *  subsequent socket calls from failing.
 *
 * Revision 1.26  1994/03/13  17:23:33  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7294, 6927, 8440, 8442
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_ipc.c,
 * 	vs_netops.c, vs_subr.c, sys_vsocket.c
 *
 *  Merge changes from R1.2
 *
 *
 * Revision 1.24.2.2  1994/03/07  23:11:51  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7294/6927, 8440, 8442
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_ipc.c,
 * 	sys_vsocket.c, vs_netops.c, vs_ipc.c
 *
 * 1) Added debug code
 *
 * 2) vs_netsoreserve() was not returning error
 * code correctly. This was fixed previously, but
 * appears to have been inadvertently dropped.
 *
 * 3) Made numerous modifications to fix #7294,
 * #6927, #8440 and #8442. Made sure that the
 * following fields in members of rvs chains
 * were corrected set/checked: vs_flags, server_port,
 * server_node. Changed vs_netsodequeue() to
 * call vs_collapse_state() if necessary (#8440).
 * If a non NULL nam parameter is passed to
 * vs_netsosend(), the user has attempted a
 * sendto() operation. It is therefore
 * necessary to call find_one_server() to
 * verify which network server should be
 * used to send the packet (#8442).
 *
 * Revision 1.25  1994/02/02  17:28:01  slk
 *   Bind would fail on a compute node due to unset clearinghouse_port.
 *   Use the function find_clearinghouse() before making clearinghouse
 *   requests (if_lookup).
 *
 *  Reviewer:  Charlie Johnson, John Litvin, Nina Lepak
 *  Risk: Low
 *  Benefit or PTS #: 7890
 *  Testing: Run EATS getsockname by hand on compute, service & boot nodes.
 *  Module(s):
 *
 * Revision 1.24.2.1  1994/02/02  17:37:05  slk
 *   Bind would fail on a compute node due to unset clearinghouse_port.
 *   Use the function find_clearinghouse() before making clearinghouse
 *   requests (if_lookup).
 *
 *  Reviewer:  Charlie Johnson, John Litvin, Nina Lepak
 *  Risk: Low
 *  Benefit or PTS #: 7890
 *  Testing: Run EATS getsockname by hand on compute, service & boot nodes.
 *  Module(s):
 *
 * Revision 1.24  1993/09/20  23:58:53  cfj
 * Merge R1.1 bug fixes into main stem.
 *
 * Revision 1.23  1993/09/18  00:27:59  cfj
 * Merge in R1.1 bug fix.
 *
 * Revision 1.22  1993/09/16  14:25:17  cfj
 * Fixed the LOCK_ASSERT in vs_nethold() to pass vs to SOCKET_ISLOCKED()
 * macro instead of so.
 *
 * Revision 1.21  1993/09/14  15:21:27  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.20.2.3  1993/09/20  23:50:06  cfj
 * Bug fix for PTS #6663.  The correct netserver is now chosen.
 *
 * Revision 1.20.2.2  1993/09/18  00:22:45  cfj
 * Fix for PTS bug #6676.  Modified the function vs_netsosleep() so that
 * the return value from the call to sosleep() is assigned to the
 * variable "error".
 *
 * Revision 1.20.2.1  1993/09/14  15:19:41  cfj
 * Split the hold/release routines into network oriented and AF_UNIX
 * oriented. (These should be virtual socket operations.)  We now
 * destroy the virtual sockets port on the last decrement of the reference
 * count.  Part of the bug fix for PTS #6097.
 *
 * Revision 1.20  1993/09/01  01:41:33  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.19  1993/07/14  18:49:27  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  21:14:24  cfj
 * Adding new code from vendor
 *
 * Revision 1.18  1993/05/20  16:04:44  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 3.51  93/08/26  11:06:15  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.  Option history
 * should be replayed with SOCKET_LOCK() held, just in case.
 * 
 * Revision 3.50  93/08/23  00:21:09  mjl
 * [LCCbug #0370] Move creation of history chain elements to common subroutine.
 * Return correct mbuf length in vs_netsogetopt() and vs_netsogetaddr().
 * 
 * Revision 3.49  93/08/22  09:40:27  bhk
 * Closed an mbuf leak [#358]
 * Primed the readable cache for non-blocking I/O to prevent
 * hangs when garbage flags are non-blocking I/O is specified
 * by MSG_NONBLOCK in the flags field of a receive [#360]
 * 
 * Revision 3.48  93/08/19  15:09:01  bhk
 * Unified the creation of remote virtual sockets [#349]
 * 
 * Revision 3.47  93/08/17  19:59:17  mjl
 * [Bug #0349, #0350] Rewrote vs_netsobind() so that any error gotten while
 * creating or binding a remote secondary aborts the entire bind operation.
 * This prevents contention between processes trying to bind to the same
 * port number using INADDR_ANY.  NB some form of back-off is probably needed.
 * Also: (a) replay option history when creating AF_ROUTE secondaries (a good
 * reason to keep the history chain until close), (b) move clearing of
 * SS_ISCONNECTED flag to vs_close_remotes() routine, (c) prototyped forward
 * decls to get rid of i860 warnings, (d) stale code removal, comment fixes
 * and additional VSDEBERROR printfs, (e) r_vs_socreate() has errno out arg.
 * 
 * Revision 3.46  93/08/16  19:03:32  bhk
 * Removed remote sleep routines. [# 346]
 * split remote getstate into synchronouse and asynchronous calls [#346]
 * Added creds port to close to avoid crash[ #347]
 * Fixed bind problem w/ mutliple network servers where
 * a partial bind was causing future binds to fail [#345]
 * 
 * Revision 3.45  93/08/11  15:09:28  mjl
 * Various style improvements and debug printf tweeks.
 * 
 * Revision 3.44  93/08/11  09:11:03  bhk
 * Moved the vs_index field from the primary virtual socket to the
 * remote virtual socket chain.  This allows the primary
 * to check secondaries when needed instead of all at once.
 * 
 * Revision 3.43  93/08/09  19:09:55  bhk
 * Passed the target port over as a port name in r_m_vs_sosleep to
 * allow the primary virtual socket to identify the secondary which
 * woke up.  Removed the parent port from r_m_vs_sosleep and used the
 * cached callback port.  fixes bug 323
 * 
 * Revision 3.42  93/08/09  15:55:22  bhk
 * Fixed freeing a free mbuf
 * added callback port to remote socket creation
 * 
 * Revision 3.41  93/08/06  17:59:03  bhk
 * Stopped virtual sockets with multiple active secondary sockets from
 * consuming the entire transport bandwith waiting for something to
 * read Alligned the lock state flag names to better reflect their
 * meaning.
 * 
 * Revision 3.40  93/08/03  17:17:30  mjl
 * Don't do OIP forwarding around r_vs_soreadable(), as no transid is being
 * passed (and in any case the server side never blocks).
 * 
 * Revision 3.39  93/06/29  15:44:43  bhk
 * Upgrade to the new V1.05b1 OSF/1 AD which holds a file reference
 * when doing a select.  Fixes bugs 264 and 244
 * 
 * Revision 3.38  93/06/24  13:33:18  mjl
 * [LCC bug 0301] Add missing ux_server_thread_{un,}blocking() calls around
 *     if_lookup() RPC.  Add debug code to show which nodes a virtual socket
 *     becomes bound to.
 * 
 * Revision 3.37  93/05/10  14:44:47  bhk
 * Synchronized state after connection request
 * 
 * Revision 3.36  93/05/10  11:55:47  bhk
 * Fixed the getpeername bug (#258) by correctly zeroing the pointer
 * to the mbuf Only free the mbuf if a remote error occured.
 * 
 * Revision 3.35  93/05/07  15:34:24  nina
 * Added vsocket_routeops and vs_route_sosend() to support
 * single system semantics for writes on AF_ROUTE sockets 
 * 
 * Revision 3.34  93/05/05  22:47:57  mjl
 * Select queue calculation corrections.  (mjl for bhk)
 * 
 * Revision 3.33  93/05/04  16:48:28  bhk
 * Fixed RCS Comment
 * 
 * Revision 3.32  93/05/03  14:50:05  bhk
 * Fixed select enqueue bug(241), multiserver bind/connect bug(242),
 * Fixed udpsoak(228), qlen mismatches(243)
 * Added correct detection of multiple network servers.
 * 
 * Revision 3.31  93/04/16  11:29:05  bhk
 * cleaned up the comments and added select fixes done at Intel
 * 
 * Revision 3.30  93/04/03  12:08:31  klh
 * Split select into three parts (check, enqueue, dequeue)
 * Fixed locking problem (211).  (klh for bhk)
 * 
 * Revision 3.29  93/03/25  18:01:27  bhk
 * Fixed a problem with the ux_server_thread_blocking/unblocking in
 * vs_netsoclose where ux_server_thread_unblocking would always be called.
 * 
 * Revision 3.28  93/03/24  14:39:56  bhk
 * added ux_server_thread_blocking and ux_server_thread_unblocking
 * to remote operations
 * 
 * Revision 3.27  93/03/19  19:52:59  bhk
 * Fixed the deadlock bug (#206) added release and recapture of the
 * master lock to the macro which is called be fore a remote operation.
 * 
 * Revision 3.26  93/03/19  18:12:15  bhk
 * Fixed a port leak for virtual sockets.
 * code cleanup for return values,
 * Stopped getstate from unlocking a socket it did not have the lock to.
 * Fixed the mbuf leak for sockets on a network server node.
 * 
 * Revision 3.25  93/03/17  19:16:32  klh
 * In vs_netsosend() transfer count information was not being updated
 * 	in certain error conditions.
 * In vs_netsocreate() fixed intialization problem in socket creation
 * 	to allow operations on upbound sockets (klh for bhk)
 * 
 * Revision 3.24  93/03/06  15:24:43  klh
 * Previous checkin was incorrect.  Change len to len1.
 * 
 * Revision 3.23  93/03/06  14:30:25  klh
 * Bug #194 (Intel 4161): Free receive buffers for network receives
 * (klh for bhk).
 * 
 * Revision 3.22  93/03/03  17:20:48  mjl
 * Definition of INVALID_NODE was moved to <sys/types.h>.
 * 
 * Revision 3.21  93/03/02  14:09:25  bhk
 * Two small bugs. One was a number of parameters mismatch in connect
 * and the other was a problem with recieve and multiple network servers.
 * 
 * Revision 3.20  93/02/25  17:55:48  nina
 * Initialize node number in the remote virtual socket chain.  Currently only 
 * remote_if_ioctl() uses this field (see server/vsocket/sys_vsocket.c).  (mjl)
 * 
 * Revision 3.19  93/02/22  17:48:34  mjl
 * No-op check in, o2install has a bug....
 * 
 * Revision 3.18  93/02/19  15:40:49  bhk
 * Cleaned up remote getsockopt and setsockopt code.
 * 
 * Revision 3.17  93/02/10  16:16:14  bhk
 * Return error or success for vs_netsogetopt
 * 
 * Revision 3.16  93/02/09  18:19:17  mjl
 * Code cleanup, plus fix for Bug#0159: don't have an assertion failure
 * when no appropriate network servers are registered for a socket address.
 * 
 * Revision 3.15  93/01/11  20:40:23  bhk
 * Fixed the optimization which left a receive right on a local virtual socket.
 * 
 * Revision 3.14  93/01/11  19:21:06  bhk
 * Added family to if_lookup to allow passing a NULL sockaddr
 * 
 * Revision 3.13  93/01/08  15:41:37  mjl
 * Rewrote vs_netsobind() to replay saved option history correctly for remote
 * secondary sockets.  Set VS_IS_PORT when primary's callback port is created.
 * Sporadic code cleanup.
 * 
 * Revision 3.12  93/01/06  16:41:48  mjl
 * Fix two potential server segmentation violations in RPC arg lists.
 * Since sockaddr_old() and sockaddr_new() are not strictly inverse, let the
 * client side of an RPC to a remote net server perform the compat_43 work
 * (otherwise subsequent calls to sockargs() lose).  Also, minor cleanup.
 * 
 * Revision 3.11  93/01/05  14:41:10  bhk
 * Fix call to soqremque() -- use queue 0, not queue 1.  Decrement queue len
 * on head socket to prevent RPC storm on accept().  Rearrange code to reduce
 * indentation and confusing nesting.
 * 
 * Revision 3.10  93/01/04  20:44:09  bhk
 * Added support for signals for remote vsocket operations
 * 
 * Revision 3.9  92/12/11  15:01:21  mjl
 * No more VSOP_VSOCK{CREATE,DESTROY}() virtual socket operations.
 * 
 * Revision 3.8  92/10/29  19:34:17  bhk
 * Cleaned up debug code
 * 
 * Revision 3.7  92/10/27  17:55:25  bhk
 * Removed all protocol dependancies from network socket virutal operations
 * files.  Added network virtual operations for getaddr and reserve, replaced
 * usrreq with getaddr
 * 
 * Revision 3.6  92/10/07  16:20:42  bhk
 * Cleaned up code to reduce compiler warning messages
 * 
 * Revision 3.5  92/09/24  17:17:44  bhk
 * fixed select timeout
 * 
 * Revision 3.4  92/07/26  17:56:20  bhk
 * cleaned up debug
 * added support for get peer address
 * fixed bug in accept
 * 
 * Revision 3.3  92/07/06  09:09:20  chrisp
 * Correction to previous checkin: call to soreceive() missing its first
 * 	parameter (vs) and does not require rval.
 * 
 * Revision 3.2  92/06/26  14:08:21  chrisp
 * Add missing last parameter in calls to soreceive() and r_vs_soclose()
 * 	- if only to to keep the ANSI compiler quiet.
 * 
 * Revision 3.1  92/06/22  12:17:53  bhk
 * fixed vs_netdequeue and vs_netsosleep to work in
 * multi-server configurations
 * 
 * Revision 3.0  92/06/16  18:32:29  bhk
 * Genesis
 * 
 * Revision 3.2  92/05/06  16:12:57  bhk
 * got inet dequeueing working, added vs_netsosleep for
 * virtual socke sleep operation in the AF_INET domain
 * 
 * Revision 3.1  92/04/22  17:19:23  bhk
 * added getsockname call, missing vsocket operations ,
 * Also changed panics to printfs for unsupported functions
 * adn returned EOPNOTSUP error codes
 * 
 * Revision 3.0  92/04/20  17:22:04  bhk
 * Genesis Internet Remote Virtual socket user side code
 * 
 * Revision 3.0  92/03/04  14:56:52  bhk
 * Genesis	bhk
 * 
 *
 * Genisis 12/5/91 bhk
 *
 */

#include "net/net_globals.h"
#include "sys/param.h"
#include <mach/boolean.h>
#include "sys/types.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/mbuf.h"
#include "sys/uio.h"
#include "sys/socket.h"
#include "vsocket/vsocket.h"
#include "sys/socketvar.h"
#include "netinet/in.h"
#include "sys/protosw.h"
#include "sys/domain.h"
#include "vsocket/vs_types.h"
#include "vsocket/vs_chouse.h"
#include "vsocket/vs_mig.h"
#include "vsocket/vs_subr.h"

#include "sys/errno.h"
#include "uxkern/bsd_types.h"
#include "vsocket/vs_chouse.h"

extern node_t	this_node;
extern struct vsocket_ops default_vsops;
extern mach_port_t	clearinghouse_port;
extern int	clearinghouse_node;
extern int	vs_optimize_for_local_ns;

struct vsocket_ops *vsfindvsops();

extern int	vs_netrelease(struct socket *);
extern int	vs_nethold(struct socket *);
extern void	vs_make_callback_port(struct socket *);

#define VSDEBTEMP 0x40000

/*
 *  Virtual socket operation tables for networking and routing protocol
 *  families.
 */

#define Forward	extern

Forward int vs_netsocreate	(struct socket *);
Forward int vs_netsoclose	(struct socket *);
Forward int vs_netsosend	(struct socket *, struct mbuf *, struct uio *,
				 struct mbuf *, struct mbuf *, int);
Forward int vs_netsorecv	(struct socket *, struct mbuf **, struct uio *,
				 struct mbuf **, struct mbuf **, int *);
Forward int vs_netsodequeue	(struct socket *, struct socket **,
				 struct mbuf **, int);
Forward int vs_netsolisten	(struct socket *, int);
Forward int vs_netsoconnect	(struct socket *, struct mbuf *);
Forward int vs_netsoconnect2	(struct socket *, struct socket *);
Forward int vs_netsobind	(struct socket *, struct mbuf *);
Forward int vs_netsoshutdown	(struct socket *, int);
Forward int vs_netsosetopt	(struct socket *, int, int, struct mbuf *);
Forward int vs_netsogetopt	(struct socket *, int, int, struct mbuf **);
Forward int vs_netnotify	(struct socket *, pid_t, int);
Forward int vs_netrelocate	(struct socket *);
Forward int vs_netsosleep	(struct socket *, caddr_t, int, int);
Forward int vs_netsoreserve	(struct socket *, u_long, u_long);
Forward int vs_netsogetaddr	(struct socket *, struct mbuf **, int, int);
Forward int vs_route_sosend	(struct socket *, struct mbuf *, struct uio *,
				 struct mbuf *, struct mbuf *, int);

extern int vs_badop();

struct vsocket_ops vsocket_netops = {
	vs_netsocreate,
	vs_netsoclose,
	vs_netsosend,
	vs_netsorecv,
	vs_netsodequeue,
	vs_netsolisten,
	vs_netsoconnect,
	vs_netsoconnect2,
	vs_netsobind,
	vs_netsoshutdown,
	vs_netsosetopt,
	vs_netsogetopt,
	vs_netnotify,
	vs_netrelocate,
	vs_netsogetaddr,
	vs_netsosleep,
	vs_badop,		/* VSOP_VSOCKCONNECT */
	vs_netsoreserve
};


struct vsocket_ops vsocket_routeops = {
	vs_netsocreate,
	vs_netsoclose,
	vs_route_sosend,
	vs_netsorecv,
	vs_netsodequeue,
	vs_netsolisten,
	vs_netsoconnect,
	vs_netsoconnect2,
	vs_netsobind,
	vs_netsoshutdown,
	vs_netsosetopt,
	vs_netsogetopt,
	vs_netnotify,
	vs_netrelocate,
	vs_netsogetaddr,
	vs_netsosleep,
	vs_badop,		/* VSOP_VSOCKCONNECT */
	vs_netsoreserve
};


/*
 *  Virtual socket networking operations.
 */


/*
 * create a virtual socket.
 * The creation of a virtual socket should
 * be generic enough to use for all network types
 */

Forward int netsocreate1(struct socket *, int);

int 
vs_netsocreate(
	struct socket		*vs)
{
	VSDEBUG(VSDEBENTRY, ("vs_netsocreate(0x%x)\n", vs));
	return netsocreate1(vs, 0/*not locked*/);
}

int
netsocreate1(
	struct socket		*vs,
	int			islocked)
{
	register vs_socket_t	*rvs;
	extern vs_socket_t	*vs_get_new_rvs();
	DOMAIN_FUNNEL_DECL(f)

	if (!islocked) {
		DOMAIN_FUNNEL(sodomain(vs), f);
		SOCKET_LOCK(vs);
	}

	INCREMENT_VSNET_REFCNT(vs, "netsocreate1:");
	vs->vs_opts = 0;
	vs->vs_flags = 0;
	vs->vs_sleep_cmd = 0;
	vs->vs_callback = MACH_PORT_NULL;
	rvs = vs_get_new_rvs(vs);
	if (rvs == NULL) {
		if (!islocked) {
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
		}
		DECREMENT_VSNET_REFCNT(vs,"netsocreate1:");
		VSDEBUG(VSDEBERROR,
		       ("netsocreate1: vs_alloc_rvs failed, vs=0x%x\n", vs));
		return ENOMEM;
	}

	/*
	 *  Initialize condition variable and mutex used to break
	 *  ``async RPC vs. close RPC'' races at remote secondaries.
	 */
	mutex_init(&vs->vs_close_mutex);
	condition_init(&vs->vs_can_close);

	/*
	 *  Finally, create a port for the primary socket itself,
	 *  with one send right.  A copy of the send right
	 *  is generated each time a remote secondary socket is
	 *  created with r_vs_socreate() or r_vs_sodequeue().
	 */
	vs_make_callback_port(vs);

	if (!islocked) {
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
	}
	return ESUCCESS;
}


int
vs_netsoclose(
	struct socket	*vs)		/* vsocket to close */
{
	int		error = ESUCCESS;
	int		tmperror = ESUCCESS;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_netsoclose 0x%x\n",vs));

	/*
	 *  When this refcount goes to zero, soclose() is called
	 *  via vs_netrelease().
	 */
	error = DECREMENT_VSNET_REFCNT(vs,"vs_netsoclose:");
	if(!error)
		error = tmperror;

	return error;
}


int
vs_netsosend(
	struct socket	*vs,		/* vsocket to send on */
	struct mbuf	*nam,		/* name to send to */
	struct uio	*uio,		/* data to send */
	struct mbuf	*top,		/* top of mbuf chain (no uio) */
	struct mbuf	*control,	/* control message */
	int		flags)
{
	struct sockaddr		*sa = (struct sockaddr *)0;
	struct uthread		*uth = &u;
	vs_socket_t		*rvs;
	int			count;
	int			error;
	kern_return_t		kret;
	int			vssend_flag = 0;
	int			sent;
	int			salen = 0;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_netsosend 0x%x\n",vs));

	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);

	rvs = (vs_socket_t *)vs->vs_data;
	/*
	 * if the socket is not bound to a network server 
	 * find one and bind it
	 */
	if (!(vs->vs_flags & VS_ISBOUND) || nam ) {
		struct sockaddr sadd;

		if (nam) {
			sa = mtod(nam, struct sockaddr *);
			vssend_flag |= VS_SEND_ADDR;
			salen = sa->sa_len;
		} else {
			sa = &sadd;
			bzero(sa,sizeof(struct sockaddr));
			sa->sa_len = sizeof(struct sockaddr);
		}
		
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
		
		error = find_one_server(vs, sa, &rvs);
		if (error != ESUCCESS) {
			VSDEBUG(VSDEBERROR,
				("vs_netsosend: find_one_server: error %d\n",
				 error));
			goto bad_send;
		}
		ASSERT(rvs != NULL && rvs->rvs_server_node != INVALID_NODE);

		if (rvs->rvs_server_port != MACH_PORT_NULL) {
			DOMAIN_FUNNEL(sodomain(vs), f);		  
			SOCKET_LOCK(vs);
			error = replay_vs_opts(vs, rvs);
			if(error) {
				VSDEBUG(VSDEBERROR,
					("vs_netsosend: replay_vs_opts: "
					 "error %d\n", error));
				goto bad_send;
			}
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
		}
	} else {
		for (rvs = (vs_socket_t *)vs->vs_data;
		     rvs; 
		     rvs = rvs->rvs_next) {
			VSDEBUG(VSDEBMISC,
				("vs_netsosend: rvs %x vs_flags %x\n",
				rvs, rvs->rvs_flags));
			if (rvs->rvs_flags & VS_USE)
				break;			  
		}
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
	}

	ASSERT(rvs != NULL);

	if(rvs->rvs_server_port != MACH_PORT_NULL) {
		struct server_oip *oipp = &u.uu_oip;

		if(uio) {
			vssend_flag |= VS_SEND_DATA;
		}
		if(nam) {
			sa = mtod(nam, struct sockaddr *);
			vssend_flag |= VS_SEND_ADDR;
			salen = sa->sa_len;
		}
		/*
		 * Currently the top and control mbuf chains are not
		 * supported for remote operation
		 */

		if(top) {
			vssend_flag |= VS_SEND_TOP;
			panic("vs_netsosend: not ready for top 0x%x\n",top);
		}
		if(control) {
			vssend_flag |= VS_SEND_CONTROL;
			panic("vs_netsosend: not ready for control 0x%x\n",
				control);
		}
		VSOP_SET_FORW(oipp, rvs->rvs_server_port);
		if(uio->uio_iov->iov_len > SMALL_ARRAY_LIMIT) {
			if(kret = r_vs_sosend_long(rvs->rvs_server_port,
				uth->uu_procp->p_cred,
				uth->uu_oip.oip_transid,
				vssend_flag,
				(char *)sa,
				salen,
				flags,
				uio->uio_iov->iov_base,
				uio->uio_iov->iov_len,
				&sent,
				&error))
				error = vs_map_error(kret);
		} else {
			if( kret = r_vs_sosend_short(rvs->rvs_server_port,
				uth->uu_procp->p_cred,
				uth->uu_oip.oip_transid,
				vssend_flag,
				(char *)sa,
				salen,
				flags,
				uio->uio_iov->iov_base,
				uio->uio_iov->iov_len,
				&sent,
				&error))
				error = vs_map_error(kret);
		}
		VSOP_END_FORW(oipp);
		/*
		 * Certain errors transmit data so update the counts
		 */
		switch (error) {
			case ESUCCESS:
			case ERESTART:
			case EWOULDBLOCK:
			case EINTR:
				uio->uio_resid -= sent;
				uio->uio_offset += sent;
		}
	} else {
		error = sosend(vs,nam,uio,top,control,flags);
	}

bad_send:
	return(error);
}


/*
 *  VSOP_SEND() operation for AF_ROUTE sockets only!!!
 */
int
vs_route_sosend(
	struct socket	*vs,		/* vsocket to send on */
	struct mbuf	*nam,		/* name to send to */
	struct uio	*uio,		/* data to send */
	struct mbuf	*top,		/* top of mbuf chain (no uio) */
	struct mbuf	*control,	/* control message */
	int		flags)		/* flags */
{
	struct sockaddr		*sa = (struct sockaddr *)0;
	vs_socket_t		*rvs;
	int			error;
	kern_return_t		kret;
	int			vssend_flag = 0;
	int			sent;
	int			salen = 0;
	mach_port_t		ns_port;
	mach_port_t		remote_port;
	struct socket		*remote_addr;
	mach_port_t		vs_port;
	node_t			*np;
	node_t			nodes[MAX_SERVERS];
	mach_port_t		ports[MAX_SERVERS];
	int			nodes_returned;
	int			nsnodes;
	int			node_num;
	struct uthread		*uth = current_thread();
	struct server_oip 	*oipp = &uth->uu_oip;
	int			i;
	struct uio		auio;	
	struct iovec		aiov;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_route_sosend 0x%x\n",vs));

	error = find_network_servers(MAX_SERVERS, &nsnodes,
				     &nodes_returned, nodes, ports);
	if (error != ESUCCESS)
		return(error);

	/*
	 * If nsnodes == 0, there are no network servers
	 * configured.  If there are not network servers
	 * configured, there are no network interfaces,
	 * no routes, etc.
	 */
	if(nsnodes == 0) {
		return(ESUCCESS);
	}
	/*
	 * If we get this far, we have an array that contains
	 * a list of all currently configured network server
	 * nodes.  Go thru the list, repeating the send function
	 * for each entry in the list.
	 */
	for(i = 0, np = nodes; i < nodes_returned; i++, np++) {
                
		/* Assert we aren't using old end-of-table guardian scheme. */
		ASSERT(*np != INVALID_NODE);

		/*
		 * We want to send the same message to each
		 * network server node.  Therefore, we need a
		 * fresh copy of the original uio and uiov for
		 * each send.
		 */
		auio.uio_iov = &aiov;
		auio.uio_iovcnt = uio->uio_iovcnt;
		auio.uio_offset = 0;
		auio.uio_resid = uio->uio_resid;
		auio.uio_segflg = uio->uio_segflg;
		auio.uio_rw = uio->uio_rw;
		auio.uio_iov->iov_base = uio->uio_iov->iov_base;
		auio.uio_iov->iov_len = uio->uio_iov->iov_len;
		sent = 0;

		/*
		 * This loop ensures that we send the whole
		 * message each time.
		 */
		while(auio.uio_resid != 0) {			

			if(*np == this_node) {
				/*
				 *  we want to send on this node
				 */
				error = sosend(vs,nam,&auio,top,control,flags);
			} else {
				/*
				 *  we want to send to a remote node
				 */
				VSOCK_TO_PORT_LOOKUP(vs,vs_port);
				DOMAIN_FUNNEL(sodomain(vs), f);
				SOCKET_LOCK(vs);
				for(rvs = (vs_socket_t *)vs->vs_data;
				    rvs; 
				    rvs = rvs->rvs_next ) {
					if (*np == vs_rvs_node_number(rvs))
						break;
				}

				if ( rvs == NULL ) {
					/*
					 *  No remote virtual socket
					 *  on ns_node, so make one.
					 */
					rvs = vs_get_new_rvs(vs);
					if (rvs == NULL) {
						SOCKET_UNLOCK(vs);
						DOMAIN_UNFUNNEL(f);
						return (ENOMEM);
					}
					error = tnc_get_server_port(*np,
								    &ns_port);
					if (error != ESUCCESS) {
						auio.uio_resid = 0;
						SOCKET_UNLOCK(vs);
						DOMAIN_UNFUNNEL(f);
						continue;
					}

					VSOP_SET_FORW(oipp, ns_port);
					kret = r_vs_socreate(ns_port,
						uth->uu_procp->p_cred,
						vs_port,
						uth->uu_oip.oip_transid,
						sodomain(vs)->dom_family,
						&remote_port,
						vs->so_type,
						vs->so_proto->pr_protocol,
						(int *) &remote_addr,
						&error);
					VSOP_END_FORW(oipp);
					if (kret != ESUCCESS)
						error = vs_map_error(kret);
					if (error != ESUCCESS) {
						VSDEBUG(VSDEBERROR,
				    ("vs_route_sosend: r_vs_socreate: rc %d\n",
							 error));
						VS_FREE(rvs, VSM_RVS);
						auio.uio_resid = 0;
						SOCKET_UNLOCK(vs);
						DOMAIN_UNFUNNEL(f);
						continue;
					}
					ASSERT(remote_port != MACH_PORT_NULL);
					rvs->rvs_server_port = remote_port;
					rvs->rvs_server_node = *np;
					rvs->rvs_remote_so = remote_addr;
					rvs->rvs_state	= vs->so_state;
					rvs->rvs_qlen	= vs->so_qlen;
					rvs->rvs_soerror = vs->so_error;
					replay_vs_opts(vs, rvs);
					SOCKET_UNLOCK(vs);
					DOMAIN_UNFUNNEL(f);
				} /* end of make remote virtual socket */

				if(uio) {
					vssend_flag |= VS_SEND_DATA;
				}
				if(nam) {
					sa = mtod(nam, struct sockaddr *);
					vssend_flag |= VS_SEND_ADDR;
					salen = sa->sa_len;
				}
				/*
				 * Currently the top and control mbuf chains 
				 * are not supported for remote operation
				 */

				if(top) {
					vssend_flag |= VS_SEND_TOP;
					panic("vs_route_sosend: not ready for top 0x%x\n",top);
				}
				if(control) {
					vssend_flag |= VS_SEND_CONTROL;
					panic("vs_route_sosend: not ready for control 0x%x\n",control);
				}
				if(auio.uio_iov->iov_len > SMALL_ARRAY_LIMIT) {
					VSOP_SET_FORW(oipp, rvs->rvs_server_port);
					kret=r_vs_sosend_long(rvs->rvs_server_port,
						uth->uu_procp->p_cred,
						uth->uu_oip.oip_transid,
						vssend_flag,
						(char *)sa,
						salen,
							flags,
						auio.uio_iov->iov_base+auio.uio_offset,
						auio.uio_resid,
						&sent,
						&error);
					VSOP_END_FORW(oipp);
					if(kret != KERN_SUCCESS)
						error = vs_map_error(kret);
				} else {
					VSOP_SET_FORW(oipp, rvs->rvs_server_port);
					kret=r_vs_sosend_short(rvs->rvs_server_port,
						uth->uu_procp->p_cred,
						uth->uu_oip.oip_transid,
						vssend_flag,
						(char *)sa,
						salen,
						flags,
						auio.uio_iov->iov_base+auio.uio_offset,
						auio.uio_resid,
						&sent,
						&error);
					VSOP_END_FORW(oipp);
					if(kret != KERN_SUCCESS)
						error = vs_map_error(kret);
				}

			} /* end send to remote virtual socket */

			/*
			 * Certain errors transmit some data, so
			 * update the counts
			 */
			switch (error) {
				case ESUCCESS:
				case ERESTART:
				case EWOULDBLOCK:
				case EINTR:
					auio.uio_resid -= sent;
					auio.uio_offset += sent;
				default:
					/* prevent infinite loops */
					auio.uio_resid = 0;
			} 

		} /* end while loop */		
			
	} /* end for loop on list of network server nodes */
	uio->uio_resid = auio.uio_resid;
	return(error);
}


int
vs_netsorecv(
	struct socket	*vs,		/* vsocket to receive on */
	struct mbuf	**nam,		/* name received from  */
	struct uio	*uio,		/* data to send */
	struct mbuf	**topp,		/* top of mbuf chain (no uio) */
	struct mbuf	**controlp,	/* control message */
	int	*flagsp)		/* flags */
{
	int	error;
	kern_return_t	kret;
	sockarg_t	sa;
	mach_msg_type_number_t		addr_len;
	mach_msg_type_number_t		len;
	mach_msg_type_number_t		len1;
	caddr_t		*data;
	struct uthread	*uth = &u;
	vs_socket_t	*rvs;
	char 		*data_addr;
	struct mbuf	*m = NULL;
	boolean_t	result;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_netsorecv 0x%x\n",vs));
	if (nam) 
		*nam = NULL;
	if (controlp)
		*controlp = NULL;

	/* No reads allowed on the rendezvous (listening) socket! */
	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	if (vs->so_options & SO_ACCEPTCONN) {
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
		return ENOTCONN;
	}
	
	addr_len = (nam)?sizeof(struct sockaddr):0;
	len1 = len = uio->uio_iov->iov_len;
	rvs = (vs_socket_t *)vs->vs_data;
	if (!(vs->vs_flags & VS_IS_MULTI)) {	/* if there's only one */
		/* Locate the useable rvs... */
		for ( ; rvs ; rvs = rvs->rvs_next )
			if (rvs->rvs_flags & VS_USE)
				break;
		if (rvs == NULL)
			panic("vs_netsorecv: no useable rvs on vs 0x%x\n", vs);

		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);

		if (rvs->rvs_server_port != MACH_PORT_NULL) {
			struct server_oip *oipp = &u.uu_oip;
			if (nam) {
				m = m_getclr(M_WAIT, MT_SONAME);
			}
			VSOP_SET_FORW(oipp, rvs->rvs_server_port);
			if (kret = r_vs_sorecv(rvs->rvs_server_port,
				uth->uu_procp->p_cred,
				uth->uu_oip.oip_transid,
				len,
				(nam ? mtod(m,char *) : NULL),
				&addr_len,
				flagsp,
				(char_array *)&data_addr, 
				&len,
				&error))
				error = vs_map_error(kret);
			VSOP_END_FORW(oipp);
			if (!error) {
				error=uiomove(data_addr,len,uio);
				vm_deallocate(mach_task_self(),
					(vm_address_t) data_addr, len);
				if (nam)
					m->m_len = addr_len;
			} else {
				m_freem(m);
				m = NULL;
			}
		} else {
			error = soreceive(vs,nam,uio,topp,controlp,flagsp);
		}
	} else {
		struct server_oip *oipp = &u.uu_oip;
		int	flags;

		flags = flagsp?*flagsp:0;
		
		/*
		 * If we're non-blocking
		 * don't get trapped by the vs_soreadable() call
		 */
		VSDEBUG(VSDEBSOREADABLE,
			("vs_soreadable: so_state && SS_NBIO 0x%x "
			 "flags & MSG_NONBLOCK 0x%x\n",
			 (vs->so_state & SS_NBIO),
			 (flags & MSG_NONBLOCK) ));
		rvs = vs_soreadable(vs,
		    ((vs->so_state & SS_NBIO) || (flags & MSG_NONBLOCK)));
		VSDEBUG(VSDEBSOREADABLE,
			("vs_soreadable: back from vs_soreadable #1\n"));
		if(!rvs) {
			if(vs->so_error)
				error = vs->so_error;
			else
				error = EWOULDBLOCK;
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
		} else if(rvs->rvs_server_port == MACH_PORT_NULL) {
			/* 
			 * turn off the readability hint
			 */
			rvs->rvs_flags &= ~VS_READABLE;
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
			error = soreceive(vs,nam,uio,topp,controlp,flagsp);
		} else {
			if(nam && !m)
				m = m_getclr(M_WAIT, MT_SONAME);
			rvs->rvs_flags &= ~VS_READABLE;
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
			VSOP_SET_FORW(oipp, rvs->rvs_server_port);
			if(kret = r_vs_sorecv(rvs->rvs_server_port,
				uth->uu_procp->p_cred,
				uth->uu_oip.oip_transid,
				len,
				(nam ? mtod(m,char *) : NULL),
				&addr_len,
				flagsp,
				&data_addr, 
				&len1,
				&error)) 
				error = vs_map_error(kret);
			VSOP_END_FORW(oipp);
			if (!error) {
				error = uiomove(data_addr,len1,uio);
				vm_deallocate(mach_task_self(),
					      (vm_address_t) data_addr, len1);
				if(nam)
					m->m_len = addr_len;
			} else if (m) {
				m_freem(m);
				m = NULL;
			}
			/*
			 * The following call to vs_soreadable() is
			 * used to start a thread on the remote network
			 * server that we've just received data from.
			 * The thread's purpose is to wait for more 
			 * data and inform us when it arrives. Since
			 * we are calling vs_soreadable() to start the
			 * thread and are not actually interested in 
			 * receiving data at this time, we can ignore 
			 * any EWOULDBLOCK errors that vs_soreadable()
			 * returns to us.
			 */
			DOMAIN_FUNNEL(sodomain(vs), f);
			SOCKET_LOCK(vs);
			VSDEBUG(VSDEBSOREADABLE,
				("vs_soreadable: before vs_soreadable #2 "
				 "so_error %d\n", vs->so_error));
			(void) vs_soreadable(vs, TRUE);
			VSDEBUG(VSDEBSOREADABLE,
				("vs_soreadable: back from vs_soreadable #2 "
				 "so_error %d\n", vs->so_error));
			if (vs->so_error == EWOULDBLOCK)
				vs->so_error = ESUCCESS;		  
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
		}
	}

	if (m)
		*nam = m;
	return error;
}


/*
 * Here we dequeue a socket from one of possibly many network servers.
 * by the time we get here someone must have a connection pending.
 */
int
vs_netsodequeue(
	struct socket		*head, 
	struct socket		**aso,
	struct mbuf		**nam,
	int			compat_43)
{
	int			error = ENOTCONN;
	kern_return_t		kr;
	struct uthread		*uth = &u;
	sockarg_t		sa;
	struct sockaddr 	*sap;	
	mach_msg_type_number_t	addr_len;
	vs_socket_t		*head_rvs, *rvs;
	struct socket 		*vs;
	boolean_t		nbio = FALSE;
	struct server_oip	*oipp = &u.uu_oip;
	node_t			remote_dequeue_node;
	mach_port_t		remote_dequeue_port;
	DOMAIN_FUNNEL_DECL(f)
	
	VSDEBUG(VSDEBENTRY, ("vs_netsodequeue(0x%x)\n", head));

	DOMAIN_FUNNEL(sodomain(head), f);
	SOCKET_LOCK(head);

	/*
	 *  Even when this optimization is on, we should never get here
	 *  for sockets without an rvs chain.
	 */
	ASSERT( !(vs_optimize_for_local_ns && vs->vs_data == NULL) );

	/*
	 *  Scan the secondary socket info looking for someone with a
	 *  connection to dequeue.  Note that we could get here from
	 *  accept1() without sleeping, and therefore without having
	 *  done a vs_collapse_state(), so we do so here.
	 */
	vs_collapse_state(head);
	for (head_rvs = (vs_socket_t *)head->vs_data ;
	     head_rvs && (((head_rvs->rvs_flags & VS_USE) == 0) ||
			  head_rvs->rvs_qlen == 0) ;
	     head_rvs = head_rvs->rvs_next)
		;

	if (head_rvs == NULL) {
		VSDEBUG(VSDEBERROR,
			("vs_netsodequeue(0x%x): ENOTCONN!!!\n", head));
		SOCKET_UNLOCK(head);
		DOMAIN_UNFUNNEL(f);
		return ENOTCONN;
	}
	ASSERT(head_rvs->rvs_qlen != 0);
	SOCKET_UNLOCK(head);
	DOMAIN_UNFUNNEL(f);
	remote_dequeue_node = head_rvs->rvs_server_node;
	remote_dequeue_port = head_rvs->rvs_server_port;

	/*
	 *  First handle the local case.
	 */
	if (head_rvs->rvs_server_port == MACH_PORT_NULL) {
		/* Dequeue locally. */
		error =  sodequeue(head,aso,nam,compat_43);
		if (error) {
			VSDEBUG(VSDEBERROR,
			    ("vs_netsodequeue(0x%x): sodequeue: error %d\n", 
			     head, error));
			return error;
		}

		/*
		 * We have to decrement our view of how many
		 * connections were queued on the local socket
		 * so that the so_qlen can hold the total of all
		 * connections queued
		 */
		DOMAIN_FUNNEL(sodomain(head), f);
		SOCKET_LOCK(head);
		head_rvs->rvs_qlen--;
		head->vs_oldqlen--;

		/*
		 *  Transform new socket into a TNC virtual socket.
		 */
		error = netsocreate1(*aso, 0/*not locked*/);
		if (error) {
		    VSDEBUG(VSDEBERROR,
			("vs_netsodequeue(0x%x): netsocreate1(0x%x): err %d\n",
			 head, *aso, error));
			SOCKET_UNLOCK(head);
			DOMAIN_UNFUNNEL(f);		    
			return error;
		}
		if (vs_optimize_for_local_ns) {
			set_default_vsops(*aso);
		} else {
			(*aso)->vs_ops = vsfindvsops(head);
		}
		(*aso)->vs_flags |= VS_ISBOUND;
		
		SOCKET_UNLOCK(head);
		DOMAIN_UNFUNNEL(f);
		return ESUCCESS;
	}

	/*
	 *  Dequeue the remote connection.
	 */

	DOMAIN_FUNNEL(sodomain(head), f);
	SOCKET_LOCK(head);

	/*
	 *  In anticipation of a successful remote dequeue, fix up the
	 *  "composite" qlen as above.
	 */
	head->so_qlen--;
	head->vs_oldqlen--;

	/*
	 *  Simulate the dequeueing of a connected socket on the local
	 *  side by calling newsock().  Then undo the unwanted side effect
	 *  of a successful call: we don't want the new socket on the
	 *  head socket's queue any longer.
	 */
	if ((vs = sonewsock(head, 0)) == NULL) {
		/* head returned unlocked if error */	  
		DOMAIN_UNFUNNEL(f);
		return ECONNREFUSED;
	}
	SOCKET_LOCK(head);	/* Re-lock! */
	soqremque(vs, 0);

	/*
	 *  Make it a TNC network virtual socket!
	 */
	if ((error = netsocreate1(vs, 1/*islocked*/)) != ESUCCESS) {
		SOCKET_UNLOCK(head);
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
		return error;
	}
	rvs = (vs_socket_t *)vs->vs_data;
	/* The rvs_server_port is filled in by the RPC below. */
	rvs->rvs_server_node = remote_dequeue_node;
	rvs->rvs_state = vs->so_state;
	rvs->rvs_qlen = vs->so_qlen;
	rvs->rvs_soerror = vs->so_error;
	vs->vs_ops = vsfindvsops(head);
	vs->vs_flags |=  (VS_ISBOUND|VS_IS_REMOTE);
	vs->so_state &= ~SS_NOFDREF;

	/*
	 *  Create the corresponding remote secondary socket.
	 *
	 *  XXX Note we don't pass compat_43 to the network server
	 *  here.  If we let the server side of the RPC do the
	 *  compat_43 work, it will confuse the call to sockargs()
	 *  below.  Eventually the passing of compat_43 to the network
	 *  server should be abolished in all cases.
	 */
	sap = (struct sockaddr *)sa;
	addr_len = sizeof(struct sockaddr);
	VSOP_SET_FORW(oipp, remote_dequeue_port);
	kr = r_vs_sodequeue(remote_dequeue_port,
			    uth->uu_procp->p_cred,
			    (mach_port_t)vs,
			    &rvs->rvs_server_port,
			    uth->uu_oip.oip_transid,
			    sa, &addr_len, 0 /*compat_43*/,
			    &error);
	VSOP_END_FORW(oipp);
	if (kr != KERN_SUCCESS)
		panic("vs_netsodequeue(0x%x): r_vs_sodequeue(0x%x, ...): "
		      "kr 0x%x\n",
		      head, remote_dequeue_port, kr);
	if (error != ESUCCESS) {
		VSDEBUG(VSDEBERROR,
			("vs_netsodequeue(0x%x): remote dequeue errno=%d "
			 "rvs=0x%x rvs_qlen=%d rvs_state=0x%x rvs_err=%d\n",
			 head, error, rvs, rvs->rvs_qlen, rvs->rvs_state,
			 rvs->rvs_soerror));
		/*
		 *  Update rvs state to reflect a connection that was
		 *  dropped after it got enqueued at the listen socket.
		 */
		if (error == ENOTCONN)
			head_rvs->rvs_qlen = 0;
		SOCKET_UNLOCK(vs);
		(void) VSOP_CLOSE(vs);
		SOCKET_UNLOCK(head);
		DOMAIN_UNFUNNEL(f);
		return error;
	}

	/*
	 *  Fix up the new primary socket's bound address info.
	 */
	if (nam) {
		/*
		 * We have to use sizeof sockaddr since
		 * compat_43 mode blows the length field
		 * away.
		 */
		error = sockargs(nam, (caddr_t)sap,
				 sap->sa_len, MT_SONAME);
		if(error == ESUCCESS && compat_43)
			sockaddr_old(*nam);
	}

	/*
	 *  Return the newly dequeued primary socket.
	 */
	*aso = vs;
	SOCKET_UNLOCK(vs);
	SOCKET_UNLOCK(head);
	DOMAIN_UNFUNNEL(f);
	return error;
}


/*
 * listen 
 * loop through all the virtual sockets enableing listen on all of them,
 * sockets who's  virtual socket contains MACH_PORT_NULL should have
 * only one vitual socket on the list.
 */ 
int
vs_netsolisten(
	struct socket	*vs,		/* vsocket to listen on */
	int		backlog)	/* how many connections to queue */
{
	struct uthread	*uth = &u;
	vs_socket_t	*rvs;
	int		error = ESUCCESS;
	kern_return_t	kr;
	struct server_oip *oipp = &uth->uu_oip;
	boolean_t	accept_ok, local_listen;
	struct sockaddr *saddr;
	struct mbuf	*mp;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_netsolisten 0x%x\n",vs));
	local_listen = FALSE;

	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);

	/*
	 * If we have not bound to a network server yet, do
	 * so now.
	 */
	if ( !(vs->vs_flags & VS_ISBOUND) ) {
		/* This is a rip-off of sockargs */
		mp = m_getclr(M_WAIT, MT_SONAME);
		if (mp == NULL) {
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
			return (ENOBUFS);
		}
		mp->m_len = sizeof(struct sockaddr);
		saddr = mtod(mp, struct sockaddr *);
		saddr->sa_family = vs->so_proto->pr_domain->dom_family;
		sockaddr_new(mp);
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
		error = vs_netsobind(vs, mp);
		m_freem(mp);
		if (error)
			return error;
		DOMAIN_FUNNEL(sodomain(vs), f);
		SOCKET_LOCK(vs);
	}

	for (rvs = (vs_socket_t *)vs->vs_data; rvs; rvs = rvs->rvs_next) {
		if ((rvs->rvs_flags & VS_USE) == 0)
			continue;
		if (rvs->rvs_server_port == MACH_PORT_NULL) {
			local_listen = TRUE;
		} else {
			VSOP_SET_FORW(oipp, rvs->rvs_server_port);
			kr = r_vs_solisten(rvs->rvs_server_port,
					   uth->uu_procp->p_cred,
					   uth->uu_oip.oip_transid,
					   backlog, &accept_ok, &error);
			VSOP_END_FORW(oipp);
			if (kr != KERN_SUCCESS)
			    panic("vs_netsolisten: r_vs_solisten: kr 0x%x\n",
				  kr);
			if (error != ESUCCESS) {
				SOCKET_UNLOCK(vs);
				DOMAIN_UNFUNNEL(f);
				return error;
			}
			if (accept_ok) {
				if (vs->so_q == 0)
					vs->so_options |= SO_ACCEPTCONN;
				if (backlog < 0)
					backlog = 0;
				vs->so_qlimit = min(backlog, SOMAXCONN);
			}
		}
	}

	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);

	if (local_listen) {
		error = solisten(vs, backlog);
		if (error != ESUCCESS)
			return error;
	}
	return (ESUCCESS);
}


/*
 * vs netconnect2 make a local connection. This call is rarely
 * if every used for network sockets.
 * We the network stacks are removed from non-network servers
 * we will have work to do here.
 */
int
vs_netsoconnect2(
	struct socket	*vs1,		/* half or socket pair to connect */
	struct socket	*vs2)		/* other half of socket pair */
{

	VSDEBUG(VSDEBENTRY,("vs_netsoconnect2 0x%x 0x%x\n",vs1,vs2));

	return(soconnect2(vs1,vs2));
}


int
vs_netsoshutdown(
	struct socket	*vs,		/* vsocket to shut down */
	int		how)		/* how to shut it down */
{
	vs_socket_t	*rvs;
	int		error = ESUCCESS;
	int		tmperror = ESUCCESS;
	kern_return_t	kr;
	boolean_t	local_shutdown = FALSE;

	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_netsoshutdown 0x%x\n",vs));

	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	for(rvs=(vs_socket_t *)vs->vs_data;rvs;rvs = rvs->rvs_next)  {
		if(rvs->rvs_server_port == MACH_PORT_NULL)
			local_shutdown = TRUE;
		else {
			ux_server_thread_blocking();
			if(kr = r_vs_soshutdown(rvs->rvs_server_port,how))
				tmperror = vs_map_error(kr);
			ux_server_thread_unblocking();
		}
		if(tmperror)
			error = tmperror;
	}

	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	if (local_shutdown) {
		tmperror = soshutdown(vs, how);
		if (tmperror)
			error = tmperror;
	}
	return error;
}


int
vs_netsosetopt(
	struct socket	*vs,		/* vsocket to set option on */
	int		level,		/* protocol for option */
	int		optname,	/* name of option */
	struct mbuf	*m0)		/* value to set option to */
{
	int			error = ESUCCESS;
	kern_return_t		kret;
	struct mbuf		*m1 = (struct mbuf *)0;
	int			rval = ESUCCESS;
	vs_socket_t		*rvs;
	mach_msg_type_number_t	len = 0;
	struct uthread		*uth = &u;
	struct server_oip	*oipp = &uth->uu_oip;
	char			*addr = 0;
	boolean_t		local_setopt = FALSE;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_netsosetopt 0x%x\n",vs));

	/*
	 * make a copy of the mbuf to allow replay of the
	 * operation when we bind to a remote network server
	 */
	if(m0) {
		len = m0->m_len;
		addr = mtod(m0,char *);
		if(!(m1 = m_copym(m0,0,m0->m_len,M_WAIT)))
			return ENOBUFS;
	}

	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	for(rvs=(vs_socket_t *)vs->vs_data;rvs;rvs = rvs->rvs_next) {
		if(!(rvs->rvs_flags & VS_USE))
			continue;
		/*
		 *  If it's a local or unbound socket then set the
		 *  option locally, otherwise go remote.
		 */
		if((rvs->rvs_server_port == MACH_PORT_NULL)  ||
			(rvs->rvs_server_port == MACH_PORT_DEAD)) {
			local_setopt = TRUE;
		}else {
			VSOP_SET_FORW(oipp, rvs->rvs_server_port);
			kret = r_vs_sosetopt(rvs->rvs_server_port,
				uth->uu_procp->p_cred,
				uth->uu_oip.oip_transid,
				level,
				optname,
				addr,
				len,
				&rval);
			VSOP_END_FORW(oipp);
			if(kret)
				rval = vs_map_error(kret);
		}
		if(rval)
			error = rval;
	}

	if (local_setopt) {
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
		rval = sosetopt(vs, level, optname, m1);
		DOMAIN_FUNNEL(sodomain(vs), f);
		SOCKET_LOCK(vs);
		m1 = (struct mbuf *)0;
		if (rval)
			error = rval;
	}

	/*
	 *  We must keep an option history for unbound sockets, so that
	 *  when they are bound the options can be replayed for the new
	 *  remote secondary sockets.
	 */
	if (error == ESUCCESS) {
 		error = create_vs_opts(vs, VS_OPT_OPT, 
 				       level, optname, m0->m_len,
 				       mtod(m0,caddr_t));
	}
	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	if(m0)
		m_free(m0);
	if(m1)
		m_free(m1);
	return error;
}


int
vs_netsogetopt(
	struct socket	*vs,		/* vsocket to get option on */
	int		level,		/* protocol for option */
	int		optname,	/* name of option */
	struct mbuf	**m0)		/* value of option */
{
	int		error;
	kern_return_t	kret;
	mach_msg_type_number_t		len = 0;
	struct uthread	*uth = &u;
	caddr_t		addr;
	vs_socket_t	*rvs;
	int		type;
	int		rval;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_netsogetopt 0x%x\n",vs));

	/*
	 *  If it's a local or unbound socket then get the option
	 *  locally, otherwise get the option from the first remote
	 *  secondary on the chain (all secondaries should have the
	 *  same options set).
	 */
	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	rvs = (vs_socket_t *)vs->vs_data;
	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	if (rvs->rvs_server_port == MACH_PORT_NULL ||
	    rvs->rvs_server_port == MACH_PORT_DEAD) {
		error = sogetopt(vs,level,optname,m0);
	} else {
		struct server_oip *oipp = &uth->uu_oip;

		*m0 = m_get(M_WAIT, MT_SOOPTS);
		addr = mtod(*m0, caddr_t);
		len = MLEN;	/* MIG wants to see max length for OUT args */

		VSOP_SET_FORW(oipp, rvs->rvs_server_port);
		kret = r_vs_sogetopt(rvs->rvs_server_port,
			uth->uu_procp->p_cred,
			uth->uu_oip.oip_transid,
			level,
			optname,
			&type,
			addr,
			&len,
			&rval);
		VSOP_END_FORW(oipp);
		if(kret) {
			error = vs_map_error(kret);
		} else {
			error = rval;
			(*m0)->m_type = type;
			(*m0)->m_len = len;
		}
	}

	return error;
}


/*ARGSUSED*/
int
vs_netnotify(
	struct socket	*so,
	pid_t		pid,
	int		new_node)
{
	VSDEBUG( VSDEBENTRY,("vs_netnotify\n"));
	return 0;
}


/*ARGSUSED*/
int
vs_netrelocate(
	struct socket	*so)
{
	VSDEBUG(VSDEBENTRY,("vs_netrelocate\n"));
	return 0;
}


int
vs_netsosleep(
	struct socket	*vs,
	caddr_t		addr,
	int		pri,
	int		tmo)
{
	struct uthread 	*uth = &u;
	struct server_oip *oipp = &uth->uu_oip;
	vs_socket_t	*rvs;
	int		error = 0;
	kern_return_t	kr;
	int		saved_state;
	int		saved_qlen;
	int		soerror = 0;
	int		rval;
	mach_port_t	parent_port;
	int		cmd;

	VSDEBUG(VSDEBENTRY,("vs_netsosleep 0x%x\n",vs));
	LOCK_ASSERT("vs_netsosleep", SOCKET_ISLOCKED(vs)); 

	/*
	 *  If the aggregate socket state was invalid (i.e. if it 
	 *  doesn't match before and after a vs_collapse_state() 
	 *  call), then we don't really want to sleep here.
	 */
	saved_qlen = vs->so_qlen;
	saved_state = vs->so_state;
	vs_collapse_state(vs);
	if ((saved_qlen != vs->so_qlen) ||
	    (saved_state != vs->so_state))
		return ESUCCESS;

	/* 
	 * Send out any necessary state transition checks
	 */
	for (rvs = (vs_socket_t *)vs->vs_data; rvs; rvs = rvs->rvs_next) {
		if (!(rvs->rvs_flags & VS_USE))
			continue;
		if (rvs->rvs_server_port == MACH_PORT_NULL) {
			continue;
		}

		if(!(rvs->rvs_flags & VS_STATE_CHECK)) {
			rvs->rvs_flags |= VS_STATE_CHECK;
			/* MiG simpleroutine, this thread not blocking. */
			VSDEBUG(VSDEBTEMP, ("vs_netsosleep: calling "
				"r_vs_getstate_delay. so 0x%x port 0x%x\n", 
					    vs, rvs->rvs_server_port));
			kr = r_vs_getstate_delay(rvs->rvs_server_port,
				    uth->uu_procp->p_cred,
				    rvs->rvs_server_port,
				    ++rvs->rvs_state_index,
				    rvs->rvs_state,
				    rvs->rvs_soerror,
				    rvs->rvs_qlen);
			if(kr != KERN_SUCCESS) {
				VSDEBUG(VSDEBERROR, 
					("r_vs_getstate_delay: kr %d\n",kr));
				rvs->rvs_soerror =  vs_map_error(kr);
			}
		}
	}

	/*
	 * Sleep locally
	 * If a local event occurs the wakeup is a usual,
	 * if it is remote then the callback thread will 
	 * wake us up.
	 * This really should be integrated with the token
	 * management piece of TNC
	 */
	error = sosleep(vs,addr,pri,tmo);

	/*
	 * collapse the state of all the secondaries into
	 * the primary
	 */
	vs_collapse_state(vs);

	return error;
}


int
vs_netsoconnect(
	struct socket	*vs,		/* vsocket to connect */
	struct mbuf	*nam)		/* who to connect to */
{
	mach_port_t	server;
	mach_port_t	server_list[MAX_SERVERS];
	kern_return_t	kr = KERN_SUCCESS;
	int		error;
	boolean_t	islocal = FALSE;
	vs_socket_t	*rvs, *this_rvs;
	struct socket	*so;
	struct sockaddr	*saddr;
	struct uthread	*uth = &u;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("vs_netsoconnect 0x%x\n",vs));

	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	rvs = (vs_socket_t *)vs->vs_data;
	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	saddr = mtod(nam, struct sockaddr *);
	if((saddr) && (!saddr->sa_family))
		saddr->sa_family = vs->so_proto->pr_domain->dom_family;
	if(saddr->sa_family != AF_INET)
		return(EAFNOSUPPORT);	  

	/*
	 * A user does not have to call bind() before calling
	 * connect(). It is therefore possible to arrive in
	 * vs_netsoconnect() with a bound or an unbound socket.
	 * It is also possible that a user could call connect()
	 * for a SOCK_DGRAM or SOCK_RAW.
	 * 
	 * No matter which case, we must select the appropriate
	 * network server to handle the traffic between the two
	 * sockets. 
	 */
	error = find_one_server(vs, saddr, &rvs);
	if (error != ESUCCESS) {
		VSDEBUG((VSDEBERROR|VSDEBFOS),
			("netsoconnect: find_one_server: errno %d\n", error));
		return(error);
	}
	ASSERT(rvs != NULL && rvs->rvs_server_node != INVALID_NODE);

	VSDEBUG(VSDEBFOS, 	  
		("vs_netsoconnect: find_one_server: rvs 0x%x, server_port 0x%x,"
		 " server_node %d\n",
		 rvs, rvs->rvs_server_port, rvs->rvs_server_node));

	/*
	 * If we return successfully from find_one_server(),
	 * we have a pointer to an rvs. 
	 */
	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	switch (rvs->rvs_server_port) {
	case MACH_PORT_NULL:
		/*
		 * We are the preferred network server.
		 */
		if (vs_optimize_for_local_ns) {
			/*
			 * set a flag saying we are local since
			 * setting default virtual socket operations
			 * clears the remote virtual socket chain
			 */
			islocal = TRUE;
			set_default_vsops(vs);
		} else {
			vs->vs_flags |= VS_ISBOUND;
		}
		VSDEBUG(VSDEBMISC,
			("vs_netsoconnect: We are the network server\n"));
		break;

	default:
		/* 
		 * We have a remote virtual socket
		 * We will have to set options here
		 */
		error = replay_vs_opts(vs, rvs);
		break;
	}

	/*
	 *  We are connecting via _one_ network server, so this socket
	 *  should no longer have multiple active secondaries.  When
	 *  the connection is complete, we'll mark the rest of the
	 *  remote virtual sockets as down (VS_USE cleared).
	 */	   
	vs->vs_flags &= ~VS_IS_MULTI;
	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	if (!islocal && (rvs->rvs_server_port != MACH_PORT_NULL)) {
		struct server_oip *oipp = &uth->uu_oip;
		VSOP_SET_FORW(oipp, rvs->rvs_server_port);
		kr = r_vs_soconnect(rvs->rvs_server_port,
				       uth->uu_procp->p_cred,
				       uth->uu_oip.oip_transid,
				       (char *)saddr,
				       saddr->sa_len,
				       &rvs->rvs_state,
				       &error);
		VSOP_END_FORW(oipp);
		if (kr != KERN_SUCCESS)
		    panic("vs_netsoconnect: r_vs_soconnect rvs=0x%x kr=0x%x\n",
			  rvs, kr);
	} else {
		error = soconnect(vs,nam); 
	}			
	if (error != ESUCCESS) 
		return (error);

	/*
	 *  We may have attached to multiple network servers via a
	 *  bind call.  Mark the extras as not useable, but only
	 *  if this socket is being used for Virtual Circuit model
	 *  communications. If it is being used for Datagram model
	 *  communications, we could have a connection to one socket
	 *  and still be doing sendto() recvfrom() calls on one or
	 *  more other sockets.
	 */
	if ( !islocal ) {
		DOMAIN_FUNNEL(sodomain(vs), f);
		SOCKET_LOCK(vs);
		for (this_rvs = (vs_socket_t *)vs->vs_data;
		     this_rvs;
		     this_rvs = this_rvs->rvs_next) {
			/* Disable VS_USE on rvs's we won't be using. */
			if (this_rvs != rvs) {
				this_rvs->rvs_flags &= ~VS_USE;
				VSDEBUG(VSDEBMISC,
					("vs_netsoconnect: ~VS_USE for "
					 "0x%x\n", this_rvs));
			}
		}
		vs_collapse_state(vs);
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
	}
	return error;
}

int
vs_netsobind(
        struct socket   *vs,            /* vsocket to bind */
        struct mbuf     *nam)           /* name to bind to */
{
        mach_port_t             server_list[MAX_SERVERS];
        mach_msg_type_number_t  numnetservers = MAX_SERVERS;
        kern_return_t           kr = KERN_SUCCESS;
        int                     error = ESUCCESS;
        mach_port_t             rport;
        struct socket           *raddr;
        struct sockaddr         *saddr;
        register vs_socket_t    *rvs;
        mach_port_t             vs_port;
        int                     i;
        int                     old_flags;
	boolean_t		cont;
	ushort			specified_port, first_port, port;
        DOMAIN_FUNNEL_DECL(f)

        VSDEBUG(VSDEBENTRY, ("vs_netsobind(vs=0x%x)\n", vs));

        DOMAIN_FUNNEL(sodomain(vs), f);
        SOCKET_LOCK(vs);

        /*
         *  We don't allow bound sockets to be rebound.  Perhaps
         *  some future protocol family will allow this, but for
         *  now we go with the 4.3 bind() man page....
         *
         *  But whether rebinds are legal is really protocol
         *  dependent, so the best thing to do is:
         *    1. destroy the current rvs chain,
         *    2. proceed with the rebind,
         *    3. be careful about local calls to sobind().
         *       Make sure they are done after every remote
         *       secondary has had a chance to succeed
         */
        if (vs->vs_flags & VS_ISBOUND) {
                SOCKET_UNLOCK(vs);
                DOMAIN_UNFUNNEL(f);
                return (EINVAL);
        }
        old_flags = vs->vs_flags;       /* May need to restore these later. */
        SOCKET_UNLOCK(vs);
        DOMAIN_UNFUNNEL(f);

        /*
         *  We have to find the correct network server(s).
         */
        saddr = (nam ? mtod(nam, struct sockaddr *) : NULL);
        if((saddr) && (!saddr->sa_family))
                saddr->sa_family = vs->so_proto->pr_domain->dom_family;

        /*
         * Find the clearinghouse before we try to use it
         */
        error = find_clearinghouse();
        if(error != ESUCCESS) {
                VSDEBUG(VSDEBERROR,
                ("vs_netsobind: find_clearinghouse: error %d\n",error));
                return(error);
        }

        ux_server_thread_blocking();
        kr = if_lookup(clearinghouse_port,
                          this_node,
                          vs->so_proto->pr_domain->dom_family,
                          (char *)saddr,
                          (mach_msg_type_number_t)(saddr?saddr->sa_len:0),
                          server_list,
                          &numnetservers,
                          &error);
        ux_server_thread_unblocking();
        if (kr != KERN_SUCCESS) {
                VSDEBUG(VSDEBERROR,
                        ("vs_netsobind: if_lookup: kr 0x%x\n", kr));
                return vs_map_error(kr);
        } else if (error != ESUCCESS) {
                VSDEBUG(VSDEBERROR,
                        ("vs_netsobind: if_lookup: error 0x%x\n", error));
                return vs_map_error(error);
        }

        VSDEBUG(VSDEBCONTROL|VSDEBNODEBIND,
                ("vs_netsobind: %d servers for so=0x%x\n", numnetservers, vs));

        DOMAIN_FUNNEL(sodomain(vs), f);
        SOCKET_LOCK(vs);
        if (numnetservers > 1) {
                /*
                 * If the VS_BINDLOCAL option is set, we only want to bind
                 * to the local network server. If this_node is not a network
                 * server node, return an error to the user.
                 */
                if (vs->vs_flags & VS_BINDLOCAL) {
                        if (server_list[0] != MACH_PORT_NULL) {
                                SOCKET_UNLOCK(vs);
                                DOMAIN_UNFUNNEL(f);
                                return(EADDRNOTAVAIL);
                        }
                        /*bindlocal = TRUE;*/
                }
                else
                        vs->vs_flags |= VS_IS_MULTI;
        }
        else if (numnetservers == 0) {
                SOCKET_UNLOCK(vs);
                DOMAIN_UNFUNNEL(f);
                VSDEBUG(VSDEBERROR,
                        ("vs_netsobind: no network servers\n"));
                return (EADDRNOTAVAIL);
        }

        /*
         *  Create a secondary socket on each network server node.
         *
         *  NB if any r_vs_sobind() call fails with EADDRINUSE, the
         *  whole bind operation fails.  This is necessary since
         *  processes running on different nodes may try to bind to
         *  the same IP port number.  For example, process P may bind
         *  to port X on node A, while process Q may also bind to port
         *  X on node B.  When P goes to B to bind with the same port
         *  number, it will fail---likewise when Q goes to A.  (The
         *  rshd can do this.)  For this reason we always bind any
         *  local secondary socket last, so that if we get any
         *  EADDRINUSE error we can destroy the remote secondaries
         *  without having made the primary socket unbindable (recall
         *  that the primary also serves as a secondary if the
         *  primary's node is a network server).
 	 *
	 *  If specified_port != 0, and we get any sort of error,
	 *  fail and return to the user.
	 * 
 	 *  If specified_port == 0, the user wants the system to 
	 *  pick the port number. In the case where multiple network
	 *  interfaces are configured, each of the secondary sockets
	 *  must have the same port number. Therefore, if any of
	 *  the bind operations fails with EADDRINUSE, tear everything
	 *  down and try again with a different port number. If
	 *  some other error is encountered, tear everything down
	 *  and return the error to the user.
         */
        ASSERT(vs->vs_data != NULL);

	specified_port = ((struct sockaddr_in*)saddr)->sin_port;	
	if (specified_port) {
		error = vs_bind_remotes(vs, numnetservers, 
				&server_list[0], nam);
		if (error == ESUCCESS)
			error = vs_bind_local(vs, numnetservers, nam, &f);
		if (error != ESUCCESS) {
			(void)vs_close_remotes(vs, 1/*keep local rvs*/);
			vs->vs_flags = old_flags;
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
			return(error);
		}
	}

	/*
	 * specified_port == 0, i.e. the user wishes the system to
	 * pick a port number. 
	 */
	else {
		cont = TRUE;
		first_port = 0;
		while (cont) {
			/*
			 * Contact the clearinghouse and get a
			 * port number.
			 */
			kr = if_assign_port_number(clearinghouse_port,
				sodomain(vs)->dom_family,
				vs->so_proto->pr_protocol,
				(short *)&port,
				&error);
			if  (kr != ESUCCESS) {
				VSDEBUG(VSDEBERROR,
					("vsnetsobind: if_assign_port_number:"
					"kr 0x%x\n", kr));
				error = vs_map_error(kr);
				cont = FALSE;
			}
			/*
			 * if_assign_port_number() returns ESUCCESS or
			 * dies with an ASSERTION failure.
			 */

			/* Detect cycling thru entire port space */
			if (first_port == 0)
				first_port = port;
			else if (first_port == port) {
				error = EADDRINUSE;
				cont = FALSE;
			}
			((struct sockaddr_in*)saddr)->sin_port = port;

			error = vs_bind_remotes(vs, numnetservers,
					&server_list[0], nam);
			if (error == ESUCCESS)
				error = vs_bind_local(vs, numnetservers,
						nam, &f);
			if (error == EADDRINUSE) {
				(void)vs_close_remotes(vs, 1);
				vs->vs_flags = old_flags;
			}
			else
				cont = FALSE;
		}

		/* 
		 * Make sure that saddr has exactly
		 * what the user sent to us before
		 * we return
		 */
		((struct sockaddr_in*)saddr)->sin_port = specified_port;

		if (error != ESUCCESS) {
			(void) vs_close_remotes(vs, 1/*keep local rvs*/);
			vs->vs_flags = old_flags;
			SOCKET_UNLOCK(vs);
			DOMAIN_UNFUNNEL(f);
			return error;
		}
	} 

        /*
         *  If we have multiple network servers, and we anticipate doing
         *  non-blocking I/O, then prime the readability checking threads.
         */
        if((vs->so_state & SS_NBIO) && (vs->vs_flags & VS_IS_MULTI)) {
                (void) vs_soreadable(vs,TRUE);
        }
        SOCKET_UNLOCK(vs);
        DOMAIN_UNFUNNEL(f);
        return (ESUCCESS);
} /* end of vs_netsobind */


int
vs_netsoreserve(
	struct socket	*vs,
	u_long		sendcc,
	u_long		recvcc)
{
	vs_socket_t	*rvs;
	int	error = ESUCCESS;
	int	tmperror;
	kern_return_t	kret;
	DOMAIN_FUNNEL_DECL(f);

	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	for(rvs=(vs_socket_t *)vs->vs_data;rvs;rvs = rvs->rvs_next) {
		if(!(rvs->rvs_flags & VS_USE))
			continue;
		
		if(rvs->rvs_server_port == MACH_PORT_NULL)
			tmperror = soreserve(vs,sendcc, recvcc);
		else {
			ux_server_thread_blocking();
			if(kret =  r_vs_soreserve(rvs->rvs_server_port,
				sendcc, recvcc, 
				&tmperror)) 
				tmperror = vs_map_error(kret);
			ux_server_thread_unblocking();
		}
		if(tmperror)
			error = tmperror;
	}
	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	return(error);
}


int
vs_netsogetaddr(
	struct socket	*vs,
	struct mbuf	**nam,
	int		which,
	int		compat_43)
{
	vs_socket_t	*rvs;
	int	error = ESUCCESS;
	struct mbuf	*m;
	sockarg_t	sa;
	kern_return_t	tmperror;
	struct server_oip *oipp = &u.uu_oip;
	DOMAIN_FUNNEL_DECL(f);

	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	rvs=(vs_socket_t *)vs->vs_data;
	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	if(rvs->rvs_server_port == MACH_PORT_NULL) {
		error = sogetaddr(vs,nam,which,compat_43);
	} else {
		struct uthread		*uth = &u;
		mach_msg_type_number_t	len;

		*nam = 0;
		if(!(m = m_getclr(M_WAIT, MT_SONAME)))
			return ENOBUFS;
		/*
		 * since we only have enough room for 1 mbuf's worth of data
		 */
		len = MLEN;

		VSOP_SET_FORW(oipp, rvs->rvs_server_port);
		if(tmperror = r_vs_sogetaddr(rvs->rvs_server_port,
			uth->uu_procp->p_cred,
			uth->uu_oip.oip_transid,
			which,
			compat_43,
			mtod((m),char *),
			&len,
			&error)) 
			error = vs_map_error(tmperror);
		VSOP_END_FORW(oipp);
		/*
		 * if the remote failed free the mbuf. otherwise
		 * the caller is responsible for freeing it.
		 */
		if(error)
			m_freem(m);
		else {
			m->m_len = len;
			*nam = m;
		}
	}
	return error;
}
