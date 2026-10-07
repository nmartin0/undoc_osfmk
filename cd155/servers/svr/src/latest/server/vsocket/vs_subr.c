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
 * $Log: vs_subr.c,v $
 * Revision 1.37  1995/03/03  19:10:17  toman
 *  Reviewer: Bob Yasi, John Litvin
 *  Risk: Medium (due to number of lines changed)
 *  Benefit or PTS #: 12508
 *  Testing: Tested syscall tracing with multiple netservers
 *  Module(s): server/uxkern/fsvr_subr.c
 *             server/vsocket/vs_ipc.c
 *             server/vsocket/vs_subr.c
 *             server/vsocket/vsocket.h
 *  Made new vsocket syscodes and passed them as parameters to
 *  start_vsockserver_op().  Also added corresponding names to a
 *  list of names in fsvr_thread_initialize(), which are displayed
 *  for extra syscodes (>= 2000) when syscall tracing is enabled.
 *  Also added parameter "serial" to start_vsockserver_op() and
 *  end_vsockserver_op(), but all vsocket operations currently pass
 *  0 for this param.
 *
 * Revision 1.36  1995/02/11  00:04:01  stans
 *  'lint' picking :
 * 	rvs = (struct vs_socket_t *)vs->vs_data;
 *   becomes
 * 	rvs = (vs_socket_t *)vs->vs_data;
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing:WW05 sats
 *
 * Revision 1.35  1995/02/01  23:32:13  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.34  1994/12/23  01:43:08  nina
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
 * Revision 1.33  1994/11/18  20:52:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.32  1994/10/07  19:09:43  yazz
 * (Corrected CVS comments.)
 *
 * Revision 1.31  1994/10/07  18:32:30  yazz
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
 * Revision 1.30  1994/08/31  22:48:15  mtm
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
 * Revision 1.28.2.2  1994/08/23  15:02:45  nina
 *  Reviewer:yazz
 *  Risk:Lo
 *  Benefit or PTS #:Fix error in debug stmt. introduced in previous checkin.
 *  Testing:
 *  Module(s):vs_subr.c
 *
 * Revision 1.28.2.1  1994/08/22  22:42:31  nina
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
 * Fixed locking problems in find_one_server() that were
 * discovered while testing fix for #7618.
 *
 * Revision 1.28  1994/07/28  22:23:06  yazz
 *  Reviewer: Mike Leibensperger
 *  Risk: Lo
 *  Benefit or PTS #: 10421
 *  Testing: EATs tcp-ip
 *  Module(s): server/vsocket/vs.defs, .../vs_subr.c, .../vs_ipc.c,
 *             .../vsocket.h
 * Removed stale code.  Added debug code for rare start_vsockserver_op()
 * failure returns -- rare but serious when they occur.
 *
 * Revision 1.27  1994/06/15  17:27:47  mjl
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
 * Revision 1.26  1994/05/12  14:41:07  chrisp
 * Re-instate fix for PTS #8959 lost in preceding merge from R1.2.
 *
 * Revision 1.25  1994/05/04  22:25:21  mjl
 * Merge revision 1.18.2.5 from R1_2 branch into main trunk.
 *
 * Revision 1.24  1994/04/13  19:18:43  chrisp
 * Correct initialization loop for server_list[] to stop overstepping
 * the end of the array. This later results in an exception for i386.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 8959
 *  Testing: Problem correction verified.
 *  Module(s):
 *
 * Revision 1.23  1994/03/17  18:53:14  nina
 *  Reviewer:hobbes
 *  Risk:Low
 *  Benefit or PTS #:8441
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_subr.c,
 * 	vs_netops.c
 *  Merge changes for #8441 from R1.2 in to R1.3
 *
 * Revision 1.18.2.6  1994/04/27  23:09:15  yazz
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
 * TNC select rewrite.  Add "wrapper" routines around r_vs_select_enqueue()
 * and ..._dequeue() RPCs to do primary-side tasks when secondaries are
 * remote.  Always create S+R rights when making socket callback ports.
 * Code to generate multicomputer-unique select ids and store entries in
 * the select id/drp map.  Use new VS_MALLOC macros.  Get rid of too-loud
 * debug printfs.  Other code cleanup.  Unused vs_reap_file_ports()
 * routine is kept, "just in case".
 *
 * Revision 1.18.2.5  1994/03/16  18:36:18  nina
 *  Reviewer:hobbes
 *  Risk:Low
 *  Benefit or PTS #:8441
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_subr.c
 * 	vs_netops.c
 *  Added debug code to vs_soreadable(). Changed comment at
 *  head of find_one_server().
 *
 * Revision 1.22  1994/03/13  17:18:56  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7294, 6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_ipc.c,
 * 	vs_netops.c, vs_subr.c, sys_vsocket.c
 *
 *  Merge changes from R1.2
 *
 * Revision 1.18.2.4  1994/03/07  23:16:19  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7294/6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_ipc.c,
 * 	sys_vsocket.c, vs_netops.c, vs_subr.c
 *
 * Rework find_one_server() to use if_get_server_by_route()
 * to locate the appropriate network server to handle traffic
 * to a given address.
 *
 * Revision 1.21  1994/01/14  18:05:29  slk
 * Merge change to allow debug server to compile from the Locus Branch.
 *
 *  Reviewer: Nina Lepak
 *  Risk: Low
 *  Benefit or PTS #: Allow debug server to compile
 *  Testing: compile with +DBG
 *  Module(s): vs_subr.c
 *
 * Revision 1.18.2.3  1994/01/14  19:31:33  slk
 *  Reviewer: Nina Lepak
 *  Risk: Low
 *  Benefit or PTS #: Allow debug servers to compile, #7795
 *  Testing: Build with +DBG
 *  Module(s): vsocket/vs_subr.c
 *
 * Revision 1.20  1993/12/10  21:59:50  nina
 * Fixed bugs that prevented Paragons from being used
 * as NFS clients if the boot node is not a network
 * server node.  See #6831, #6719, #7421, #7422, #7423
 * #7424 and #7426.  If the clearinghouse node was
 * configured to be a node other than the bootnode,
 * the system would hang during system startup. This
 * was because a NORMA call was made to a node that
 * wasn't up yet.  vs_subr.c was changed to
 * use the function find_clearinghouse() before
 * making clearinghouse requests.
 *
 *
 *  Reviewer:bolsen@locus.com, dbm@ssd.intel.com
 *  Risk:Medium
 *  Benefit or PTS #:7424
 *  Testing:Lachman NFS main suite, various configurations
 *  Module(s):./server/vsocket/vs_subr.c
 *
 * Revision 1.19  1993/12/07  17:45:59  mjl
 * In vs_select_enqueue(), rename a local variable to reflect that it is
 * the surrogate delayed reply port we are manipulating.
 *
 *
 *  Reviewer: cfj@ssd.intel.com, bhk@locus.com
 *  Risk: low
 *  Benefit or PTS #: 7272
 *  Testing: Locus network tests
 *  Module(s): server/vsocket/sys_vsocket.c, server/vsocket/vs_init.c,
 * 	server/vsocket/vs_subr.c, server/vsocket/vs_subr.h
 *
 * Revision 1.18  1993/11/30  18:27:42  jlitvin
 * Missing break statement caused all EADDRNOTAVAIL errors to be treated
 * as EIO instead.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 5974
 *  Testing: /home/sigeval/Bugs/bind
 *  Module(s): server/vsocket/vs_subr.c
 *
 * Revision 1.17  1993/10/28  03:17:50  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.16  1993/09/20  23:59:04  cfj
 * Merge R1.1 bug fixes into main stem.
 *
 * Revision 1.15.2.1  1993/09/20  23:50:18  cfj
 * Bug fix for PTS #6663.  The correct netserver is now chosen.
 *
 * Revision 1.15  1993/09/01  01:41:54  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.14  1993/07/14  18:49:44  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  21:14:51  cfj
 * Adding new code from vendor
 *
 * Revision 1.13  1993/06/03  20:11:20  cfj
 * Fix for inbound connections with multiple network servers.
 *
 * Revision 1.12  1993/05/20  16:04:50  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 3.51  93/08/26  17:03:47  mjl
 * [LCCbug #0378] Eliminate length argument to remote_vs_ioctl() routine.
 * 
 * Revision 3.50  93/08/26  11:10:16  mjl
 * [LCCbug #0374; #0376 no longer reproducible; maybe fixes #0372]
 * Replace r_vs_ioctl() RPC's fixed length INOUT array argument with
 * two variable length array arguments, one IN and one OUT.  Prevents
 * server addressing exceptions that occured when all N bytes of fixed
 * arg weren't allocated in server's address space.  Also: (a) better
 * field names for history option entries, (b) got rid of i860 warning,
 * (c) remove bogus debug printf.
 * 
 * Revision 3.49  93/08/23  00:26:26  mjl
 * [LCCbug #0370] Add create_vs_opts() routine for creating new option
 * history entries.  Hold the SOCKET_LOCK() while transforming a virtual
 * socket to a regular one in set_default_vsops().  Remove bogus calls
 * to SOCKET_UNLOCK/DOMAIN_UNFUNNEL in vs_soreadable().
 * 
 * Revision 3.48  93/08/22  09:41:06  bhk
 * Closed an mbuf leak [#358]
 * Primed the readable cache for non-blocking I/O to prevent
 * hangs when garbage flags are non-blocking I/O is specified
 * by MSG_NONBLOCK in the flags field of a receive [#360]
 * 
 * Revision 3.47  93/08/20  12:42:38  bhk
 * Fixed typo in vs_rvs_node_number[#349]
 * 
 * Revision 3.46  93/08/19  15:26:39  bhk
 * Unified the creation of remote virtual sockets [#349]
 * Added vs_rvs_node_number to return the node number of a remote node [#349]
 * 
 * Revision 3.45  93/08/17  20:40:10  mjl
 * [Bugs #0349, #0350] Add common vs_get_new_rvs() subroutine used in
 * vs_netsocreate() and vs_close_remotes() when a freshly minted rvs
 * struct is needed.  Make find_one_server() pass this_node so
 * clearinghouse can know if a network server is local to this node.
 * Add "keep" arg to vs_close_remotes()--if TRUE, be sure to leave a
 * single rvs on the chain, so that the chain looks just like it did
 * after vs_netsocreate().  This is used when unwinding from an error
 * in vs_netsobind().  Also stale code removal and new debug printfs.
 * 
 * Revision 3.44  93/08/16  19:06:04  bhk
 * Corrected debugging text.
 * split getstate into synchronous and asynchrous routines [#346]
 * Added vs_remote_close to close all secondaries [#347]
 * 
 * Revision 3.43  93/08/11  15:10:49  mjl
 * [Bug #0340] Fix vsocket_hold() and vsocket_release() to deal with
 * multiple vsocket references being taken.
 * 
 * Revision 3.42  93/08/11  09:13:27  bhk
 * Fixed the telnet hang with multiple servers #335
 * Changed the readable check to a simple routine to cut down on intra-cluster
 * traffic.
 * 
 * Revision 3.41  93/08/09  19:10:25  bhk
 * Passed the target port over as a port name in r_m_vs_sosleep to
 * allow the primary virtual socket to identify the secondary which
 * woke up.  Removed the parent port from r_m_vs_sosleep and used the
 * cached callback port.
 * Fixes bug 323
 * 
 * Revision 3.40  93/08/09  15:55:59  bhk
 * added callback port to remote socket creation
 * 
 * Revision 3.39  93/08/06  17:59:36  bhk
 * Stopped virtual sockets with multiple active secondary sockets
 * from consuming the entire transport bandwith waiting for something to read
 * 
 * Revision 3.38  93/08/03  17:21:28  mjl
 * Some code cleanup.  Use macros in vs_subr.h for port hashing.
 * In vs_select_dequeue(), wait for the surrogate drp send-once
 * right to actually go dead before calling select_dequeue().
 * (This is a work around for slow d-n notification problem.)
 * 
 * Revision 3.37  93/06/24  13:34:11  mjl
 * [LCC bug 0301] Add missing ux_server_thread_{un,}blocking() calls around
 *     call to if_lookup().  Fixed some vertical spacing.
 * 
 * Revision 3.36  93/06/03  07:57:11  bhk
 * Fixed bug 0274 (no inbound connections with multiple network servers)
 * 
 * Revision 3.35  93/05/10  14:45:37  bhk
 * Synchronized state after connection request, set synchronized state
 * 
 * Revision 3.34  93/05/07  15:36:25  nina
 * Added the function find_network_servers().  Extended
 * the proto_to_vsop[] table to support AF_ROUTE sockets.
 * 
 * Revision 3.33  93/05/06  17:37:38  mjl
 * Revised vs_collapse_state() logic.  Fixes BSD ``r'' commands.  (mjl for bhk)
 * 
 * Revision 3.32  93/05/05  22:49:46  mjl
 * Rename vs_get_nearest_server() for consistency.  Also, (mjl for bhk),
 * corrections to vs_collapse_state() for tracking socket state on secondaries.
 * 
 * Revision 3.31  93/05/04  16:49:07  bhk
 * Fixed RCS Comments
 * 
 * Revision 3.30  93/05/03  14:50:41  bhk
 * Revision 3.32  93/05/03  14:50:05  bhk
 * Fixed connect bug (244), state syncronization bug (245),
 * and multiserver bind/connect bug(242)
 * 
 * Revision 3.29  93/04/16  11:33:55  bhk
 * Fixed a possbile freeing of a null mbuf
 * 
 * Revision 3.28  93/04/03  12:13:35  klh
 * Split select into three parts (check, enqueue, dequeue).
 * Enhance detection of state transition in the virtual socket layer.
 * (klh for bhk)
 * 
 * Revision 3.27  93/03/19  18:13:35  bhk
 * Added vs_make_callback_port and vs_clear_callback_port routines
 * to make sure every thing is done when a primary virtual socket
 * is created, destroyed, split or collapsed.
 * 
 * Revision 3.26  93/03/03  17:22:53  mjl
 * In vsfindmatch(), if no match routine is found return NULL rather than a
 * panic() stub.  Callers should catch the error.  [LCC bug #0174]
 * 
 * Revision 3.25  93/02/11  15:20:12  bhk
 * find_one_server now correctly sets the flags on whether the network server
 * is local or remote.
 * 
 * Revision 3.24  93/02/10  21:09:17  klh
 * Additional checkin to sync up RCS comments
 * 
 * Revision 3.23  93/02/09  18:21:23  mjl
 * Code cleanup, plus fix for Bug#0159: don't have an assertion failure
 * when no appropriate network servers are registered for a socket address.
 * Also fix vsdef_match() calling sequence to match that of vsinet_match().
 * 
 * Revision 3.23  93/02/08  16:57:04  bhk
 * added the cred_port option to end_vsockserver.  This currently does
 * nothing but it will correctly decrement the credentials send right count.
 * 
 * Revision 3.22  93/01/11  19:21:51  bhk
 * added family to if_lookup to allow passing a NULL sockaddr
 * 
 * Revision 3.21  93/01/08  15:42:27  mjl
 * Broke replaying and clearing of saved socket option history into
 * two routines.
 * 
 * Revision 3.20  93/01/04  20:44:57  bhk
 * Added support for correct signal delivery in remote vsocket operations.
 * 
 * Revision 3.19  92/12/10  17:38:49  mjl
 * Add Unix datagram virtual socket ops to table.
 * 
 * Revision 3.18  92/10/27  17:57:40  bhk
 * Removed protocol dependancies from the virtual socket operations callout
 * table.
 * Replaced old table with the new network table for the inet world.
 * 
 * Revision 3.17  92/10/07  16:21:15  bhk
 * Cleaned up code to reduce complier warning messages
 * 
 * Revision 3.16  92/09/28  13:39:14  klh
 * FIFOs use the default socket operations for manipulating their internal 
 * sockets. (klh for mjl).
 * 
 * Revision 3.15  92/09/24  17:18:27  bhk
 * Code cleanup to remove compiler warnings
 * 
 * Revision 3.14  92/08/08  01:58:16  jdh
 * modified unix domain protocol array; removed unused routine -- jdh
 * 
 * Revision 3.13  92/07/26  17:57:09  bhk
 * cleaned up debug
 * 
 * Revision 3.12  92/06/26  18:04:23  mjl
 * Use protocol number to select set of virtual socket ops for AF_UNIX.
 * 
 * Revision 3.11  92/06/22  12:18:40  bhk
 * fixed error mapping
 * 
 * Revision 3.10  92/06/16  18:48:46  bhk
 * added ioctl/sockopt queueing, error handling
 * 
 * Revision 3.9  92/06/15  17:40:11  mjl
 * #ifdef out the obsolete vsosetfp() routine.
 * 
 * Revision 3.8  92/06/11  16:32:27  mjl
 * Moved tnc_fsvr_start_op() and tnc_fsvr_end_op() to tnc/reloc_subr.c.
 * 
 * Revision 3.7  92/05/06  13:16:40  bhk
 * changed mutex lock to a socket lock to avoid deadlocks
 * 
 * Revision 3.6  92/04/22  17:34:09  bhk
 * Fixed virtual socket protocol/domain table for ansi C.
 * fixed panic when syscalltrace as enabled
 * 
 * Revision 3.5  92/04/20  17:32:15  bhk
 * added address match routines. Added server start and end routines to
 * set up the u area
 * 
 * Revision 3.4  92/04/06  11:14:16  mjl
 * Added routine to get microkernel's sequence number from a port.  Also,
 * vsosetfp(), called from SOSETFP() macro, stores file port in socket.
 * Added code for start- and end_fileserver_op() TNC hooks; start currently
 * does nothing, end counts IPC sequence numbers and possibly initiates
 * a relocation.
 * 
 * Revision 3.3  92/03/20  17:08:55  bhk
 * moved vsocket.h to vsocket directory
 * 
 * Revision 3.2  92/03/19  11:37:10  mjl
 * Added #ifdefs to allow clean non-TNC builds.
 * 
 * Revision 3.1  92/03/18  18:09:35  mjl
 * Added a table for finding vsocket ops tables.  This is in parallel to
 * the various protosw tables, and avoids TNC-related changes to them.
 * 
 * Revision 3.0  92/03/04  14:57:00  bhk
 * Genesis	bhk
 * 
 *
 */


#include "sys/param.h"
#include "sys/types.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/errno.h"
#include "sys/mbuf.h"
#include "sys/uio.h"
#include "sys/socket.h"
#include "vsocket/vsocket.h"
#include "sys/socketvar.h"
#include "sys/protosw.h"
#include "sys/domain.h"
#include "sys/file.h"
#include "sys/ioctl.h"
#include "uxkern/import_mach.h"
#include "vsocket/vs_types.h"
#include "inet.h"
#include "net/if.h"
#include "vsocket/vs_chouse.h"
#include "vsocket/vs_mig.h"
#include "vsocket/vs_subr.h"
#ifdef VSDEBUG
#include "netinet/in.h"
#endif

extern int vsdebug;
extern node_t	this_node;
extern mach_port_t	clearinghouse_port;

extern struct vsocket_ops default_vsops;
extern int	vsdef_match();

#ifdef TNC
extern struct vsocket_ops un_pp_vsops;
extern struct vsocket_ops un_st_vsops;
extern struct vsocket_ops un_dg_vsops;
extern struct vsocket_ops vsocket_netops;
extern struct vsocket_ops vsocket_routeops;

#if INET > 0
extern int	vsinet_match();
extern mach_port_t	inetserver_port;
#endif
#endif /* TNC */

#define	Forward		extern
Forward kern_return_t	remote_vs_ioctl(mach_port_t,
					mach_port_t,
					transaction_id_t,
					int, caddr_t, int *);
Forward int		vs_close_remotes(struct socket *, int);


/*
 *  This table maps a <family,type,protocol> triple to an appropriate
 *  set of virtual socket operations.  A zero entry in a key field is
 *  considered a wildcard.
 *
 *  This should be changed so each protocol stack
 *  installs its own entries.
 */
struct proto_to_vsop_entry {
	int	family;
	int	type;
	int	protocol;
	struct vsocket_ops *operations;
	match_func_t	match_address;
	mach_port_t	*local_server;
	char	*entry_name;
};
CONST struct proto_to_vsop_entry proto_to_vsop[] = {
#ifdef	TNC
    { AF_UNIX,	SOCK_STREAM,	VOS_PIPE,
	&un_pp_vsops,		vsdef_match,	0,	"pipe" },
    { AF_UNIX,	SOCK_STREAM,	VOS_SOCK,
	&un_st_vsops,		vsdef_match,	0,	"strm" },
    { AF_UNIX,	SOCK_STREAM,	VOS_FIFO,	/* use pipe ops */
	&un_pp_vsops,		vsdef_match,	0,	"fifo" },
    { AF_UNIX,	SOCK_DGRAM,	0,
	&un_dg_vsops,		vsdef_match,	0,	"dgram" },
#if INET > 0
    { AF_INET,  0,		0,
	&vsocket_netops,	vsinet_match,	&inetserver_port,     "inet"},
#endif
    { AF_ROUTE, SOCK_RAW,	0,
	&vsocket_routeops,	vsinet_match,	&inetserver_port,     "route"},
#endif
    { 0,	0,		0,
	&default_vsops,		vsdef_match,	0,	"dflt" },
    { -1,	-1,		-1,
	NULL,			vsdef_match,	0,	"NULL!" }
};


int
vsdef_match(addr, vsaddr)
struct sockaddr		*addr;
struct vs_sockaddr	*vsaddr;
{
	VSDEBUG(VSDEBERROR,("vsdef_match called\n"));
	return 0;
}


CONST struct proto_to_vsop_entry *
vsfindproto_to_vsop(dom,type,proto)
int	dom;
int	type;
int	proto;
{
	CONST struct proto_to_vsop_entry *p2v;
	for ( p2v = proto_to_vsop ; p2v->family != -1 ; p2v++ ) {
		if ( (!p2v->family || dom == p2v->family) &&
		     (!p2v->type || type == p2v->type) &&
		     (!p2v->protocol || proto == p2v->protocol) ) {

			VSDEBUG(VSDEBRET,("vsfindproto_to_vsop: <%d,%d,%d> ",
				   dom, type, proto));
			VSDEBUG(VSDEBRET,(" --> %s (0x%x)\n",
				   p2v->entry_name, p2v->operations));

			return p2v;
		}
	}
	return (struct proto_to_vsop_entry *)0;
}


/*
 * Return a pointer to the domain-specific address
 * matching routine, or NULL.
 */
match_func_t
vsfindmatch(family)
u_char	family;
{
	int	domain = (int)family;
	CONST struct proto_to_vsop_entry *p2v;
	
	if(p2v = vsfindproto_to_vsop(domain,0,0))
		return (p2v->match_address);
	else
		return (NULL);
}

	
mach_port_t
vsfindserver(family)
u_char	family;
{
	int	domain = (int)family;
	CONST struct proto_to_vsop_entry *p2v;
	
	if(p2v = vsfindproto_to_vsop(domain,0,0))
		return *p2v->local_server;
	else
		return (mach_port_t)0;

}
	

struct vsocket_ops *
vsfindvsops(vs)
struct socket *vs;
{
	CONST struct proto_to_vsop_entry *p2v;


	if(p2v = vsfindproto_to_vsop(vs->so_proto->pr_domain->dom_family,
		vs->so_proto->pr_type, vs->so_proto->pr_protocol))
		return(p2v->operations);

	VSDEBUG(VSDEBRET,("vsfindvsops: no match dom=%d, type=%d, proto=%d\n",
		       vs->so_proto->pr_domain->dom_family,
			vs->so_proto->pr_type,
			vs->so_proto->pr_protocol));

	return(&default_vsops);
}



int
vsocreate(dom, aso, type, proto)
	int dom;
	struct socket **aso;
	register int type;
	int proto;
{
	int	error = ESUCCESS;

	error = socreate(dom,aso,type,proto);
	if (error == ESUCCESS) {
		(*aso)->vs_ops = vsfindvsops(*aso);
		if (error = VSOP_CREATE(*aso)) {
			soclose(*aso);	/* XXX VSOP_CLOSE()? */
		}
	}
	return(error);
}


/*
 *  Extract the sequence number from a Rcv rt.
 *  If messages 1..N have been received on the port,
 *  then the sequence number returned here is N.
 *  This info isn't too useful unless you know that the
 *  Rcv rt is no longer in the server port set (since other messages
 *  may arrive, making the number returned here obsolete).
 */
mach_port_seqno_t
seqno_from_port(port)
	mach_port_t port;
{
	kern_return_t kr;
	mach_port_status_t mps;


	kr = mach_port_get_receive_status(mach_task_self(), port, &mps);
	if ( kr != KERN_SUCCESS ) {
		printf("seqno_from_port 0x%x failed, kr = %d\n", port, kr);
		return(NULL);
	}
	return( mps.mps_seqno );
}



kern_return_t
start_vsockserver_op(
	mach_port_t		port,
	mach_port_t		creds_port,
	transaction_id_t	transid,
	int			syscode,
	int			serial)
{
	struct uthread		*uth = &u;
	kern_return_t		error;

	/* initialize the server thread for this request */

	VSDEBUG(VSDEBCREDPORT,
		("Start:",vs_debug_port(creds_port,"Cred Port:")));

	uth->uu_syscode = syscode;
	error = fsvr_thread_initialize(uth, creds_port);
	if (error != KERN_SUCCESS) {
		/* things are pretty bad if this happens -- let folks know */
		printf("start_vsockserver_op FAILED 0x%x", error);
		return (error);
	}
	oip_register(uth, creds_port, transid);	/* a legit (void) routine */

	if (serial) {
		unix_master();
	}

	return (error);
}


kern_return_t
end_vsockserver_op(
	mach_port_t	cred_port,
	register int	error,
	register int	serial)
{
	kern_return_t	ret;
	struct uthread	*uth = &u;

	if (serial)
		unix_release();

	oip_deregister(uth);
	fsvr_thread_terminate(uth,error);
	VSDEBUG(VSDEBCREDPORT,
		("End:",vs_debug_port(cred_port,"Cred Port:")));
	return error;
}


/*
 *  Allocate a remote virtual socket entry to be placed on a primary
 *  socket's rvs chain.  Currently used only in vs_netsocreate() and
 *  (sometimes) in vs_close_remotes().
 */
vs_socket_t *
vs_get_new_rvs(
	struct socket	*vs)
{
	vs_socket_t	*rvs;
	vs_socket_t	*tmprvs;

	LOCK_ASSERT("vs_get_new_rvs", SOCKET_ISLOCKED(vs));

	VS_MALLOC(rvs, vs_socket_t *, sizeof(vs_socket_t), VSM_RVS);
	if (rvs == NULL)
		return (NULL);

	bzero((char *)rvs, sizeof(vs_socket_t));
	rvs->rvs_server_node = INVALID_NODE;
	rvs->rvs_flags = VS_USE;
	rvs->rvs_so = vs;

	if (vs != NULL) {
		rvs->rvs_state = vs->so_state;
		vs->vs_oldqlen = rvs->rvs_qlen = vs->so_qlen;
		rvs->rvs_soerror = vs->so_error;

		/*
		 *  Append the new remote virtual socket to the end
		 *  of the rvs chain.
		 */
		if (vs->vs_data) {
			tmprvs = (vs_socket_t *)vs->vs_data;
			while(tmprvs->rvs_next) {
				tmprvs = tmprvs->rvs_next;
			}
			tmprvs->rvs_next = rvs;
		} else {
			vs->vs_data = (caddr_t)rvs;
		}
	} else
		panic("vs_get_new_rvs: No primary?!\n");

	return (rvs);
}


kern_return_t
vs_map_error(
	kern_return_t	error)
{
	kern_return_t	newerr = error;

	/*
	 *  (We just picked a number for VS_NUM_ERRORS, since sys_nerr
	 *  doesn't exist...)
	 */
	if((error > VS_NUM_ERRORS) || (error < 0)) {
		switch (error) {
		case NSRV_ADDR_EXISTS:
		case NSRV_ADDR_NOT_FOUND:
			VSDEBUG(VSDEBERROR,
				("vs_map_error: addr lookup error %x\n",error));
			newerr = EADDRNOTAVAIL;
			break;
		default:
			newerr = EIO;	
		}
	} 
	VSDEBUG(VSDEBERROR, ("vs_map_error: mapped %d (0x%x) to %d (0x%x)\n",
			     error, error, newerr, newerr));
	return newerr;
}


void
enqueue_vs_opts(vs,opts)
	struct socket		*vs;
	vs_opts_t		*opts;
{
	register vs_opts_t	**rropts;

	LOCK_ASSERT("enqueue_vs_opts", SOCKET_ISLOCKED(vs));
	for ( rropts = (vs_opts_t **)&vs->vs_opts;
	      *rropts;
	      rropts = &(*rropts)->vo_next )
		;
	opts->vo_next = NULL;
	*rropts = opts;

	VSDEBUG(VSDEBCONTROL,
		("vs_opts = 0x%x rropts = 0x%x\n",vs->vs_opts,rropts));
}


#ifdef VSOCKET_DEBUG
int	vs_opts_tot_size = 0;
int	vs_opts_cur_size = 0;
int	vs_opts_cur_num = 0;
int	vs_opts_tot_num = 0;
#endif

int
create_vs_opts(
	struct socket	*vs,
	int		type,
	int		level,
	int		name,
	int		len,
	char		*data)
{
	vs_opts_t *	newopts;

	LOCK_ASSERT("create_vs_opts", SOCKET_ISLOCKED(vs));

	VS_MALLOC(newopts, vs_opts_t *, sizeof(vs_opts_t), VSM_OPT);
	if (newopts == NULL) {
		return ENOMEM;
	}
	newopts->vo_next = (vs_opts_t *)0;
	newopts->vo_value = NULL;
	newopts->vo_len = 0;
	if(data && len) {
		VS_MALLOC(newopts->vo_value, caddr_t, len, VSM_OPTVAL);
		if (newopts->vo_value == NULL) {
			VS_FREE(newopts, VSM_OPT);
			return ENOMEM;
		}
		bcopy(data,newopts->vo_value,len);
		newopts->vo_len = len;
	} 
	newopts->vo_type = type;
	newopts->vo_level = level;
	newopts->vo_optname = name;
	enqueue_vs_opts(vs, newopts);
	VSSTAT_LOCK;
	vsstats.vss_optsallocated++;
	VSSTAT_UNLOCK;
#ifdef VSOCKET_DEBUG
	vs_opts_tot_size += newopts->vo_len;
	vs_opts_cur_size += newopts->vo_len;
	vs_opts_cur_num++;
	vs_opts_tot_num++;
#endif
	return ESUCCESS;
}


kern_return_t
replay_vs_opts(vs, rvs)
	struct socket	*vs;
	vs_socket_t	*rvs;
{
	register struct vs_opts *rvsopt;
	struct vs_opts 		*tmpopt;
	struct uthread		*uth = &u;
	kern_return_t		error = KERN_SUCCESS;
	int			rval = KERN_SUCCESS;
	int			length;

	LOCK_ASSERT("replay_vs_opts", SOCKET_ISLOCKED(vs));

	/*
	 *  Don't need to replay options for the local socket,
	 *  since that was done in vs_netsosetopt().
	 */
	if (rvs->rvs_server_port == MACH_PORT_NULL) 
		return KERN_SUCCESS;


	/*
	 *  Replay vs's socket option history for the newly created
	 *  secondary socket represented by rvs.
	 */
	for (rvsopt = (vs_opts_t *)vs->vs_opts;
	     rvsopt;
	     rvsopt = rvsopt->vo_next) {

		switch(rvsopt->vo_type) {
		case VS_OPT_OPT:
			error = r_vs_sosetopt(rvs->rvs_server_port,
				      uth->uu_procp->p_cred,
				      uth->uu_oip.oip_transid,
				      rvsopt->vo_level,
				      rvsopt->vo_optname,
				      rvsopt->vo_value,
				      (mach_msg_type_number_t)
					      rvsopt->vo_len,
				      &rval);
			if (error != KERN_SUCCESS || rval != ESUCCESS) {
				VSDEBUG(VSDEBERROR,
	("replay_vs_opts(0x%x, 0x%x): r_vs_sosetopt: kr %d, errno %d\n",
					 vs, rvs, error, rval));
			}
			break;

		case VS_OPT_IOCTL:
			/*
			 *  Note: must use temporary for length argument,
			 *  since it will be set to zero for IOC_IN ioctls.
			 */
			error = remote_vs_ioctl(rvs->rvs_server_port,
				   uth->uu_procp->p_cred,
				   uth->uu_oip.oip_transid,
				   rvsopt->vo_optname,
				   rvsopt->vo_value,
				   &rval);
			if (error != KERN_SUCCESS || rval != ESUCCESS) {
				VSDEBUG(VSDEBERROR,
	("replay_vs_opts(0x%x, 0x%x): r_vs_sosetopt: kr %d, errno %d\n",
					 vs, rvs, error, rval));
			}
			break;
		}
	}

	/* XXX Fix error handling, or maybe panic on errors above? */
	return (KERN_SUCCESS);
}


void
free_vs_opts(vs)
	struct socket	*vs;
{
	register struct vs_opts *rvsopt;
	vs_socket_t		*rvs;
	struct vs_opts 		*tmpopt;

	LOCK_ASSERT("free_vs_opts", SOCKET_ISLOCKED(vs));

	/*
	 *  We no longer need the option history chain...
	 */
	rvsopt = (vs_opts_t *)vs->vs_opts;
	while ( rvsopt ) {
		if(rvsopt->vo_value)
			VS_FREE(rvsopt->vo_value, VSM_OPTVAL);
		tmpopt = rvsopt;
		rvsopt = rvsopt->vo_next;
		VS_FREE(tmpopt, VSM_OPT);
		VSSTAT_LOCK;
		vsstats.vss_optsfreed++;
		VSSTAT_UNLOCK;
	}
	vs->vs_opts = (caddr_t)0;
}


void
vs_make_callback_port(
	struct socket *vs)
{
	mach_port_t	socket_port;
	kern_return_t	kr;

	LOCK_ASSERT("vs_make_callback_port", SOCKET_ISLOCKED(vs));
	ASSERT((vs->vs_flags & VS_IS_PORT) == 0);

	/*
	 *  Create a port for the primary socket to receive messages
	 *  from remote secondaries.  Send rights will be given out to
	 *  secondary sockets later, on an "as needed" basis (e.g.
	 *  during select(), sleep(), etc.).
	 */
	VSOCK_TO_PORT_LOOKUP(vs, socket_port);
	kr = mach_port_allocate_name(mach_task_self(),
				     MACH_PORT_RIGHT_RECEIVE,
				     socket_port);
	if (kr != KERN_SUCCESS)
	    panic("vs_make_callback_port(0x%x): m_p_allocate_name: kr 0x%x\n",
		  vs, kr);

	/*
	 *  We create one send right here so that the default MiG
	 *  COPY_SEND semantics will work.
	 */
	kr = mach_port_insert_right(mach_task_self(),
				    socket_port, socket_port,
				    MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
	    panic("vs_make_callback_port(0x%x): m_p_insert_right: kr 0x%x\n",
		  vs, kr);

	vs->vs_flags |= VS_IS_PORT;
	vs->vs_magic = VS_MAGIC;
	ux_server_add_port(socket_port);
}


/*
 *  Deallocate a socket port.  This could be either a primary or
 *  secondary socket.
 */
void
vs_clear_callback_port(
	struct socket		*vs)
{
	mach_port_t		socket_port;
	kern_return_t		kr;

	LOCK_ASSERT("vs_clear_callback_port", SOCKET_ISLOCKED(vs));

	if (vs->vs_flags & VS_IS_PORT) {
		/*
		 *  Accept no more messages on the callback port.
		 */
		VSOCK_TO_PORT_LOOKUP(vs, socket_port);
		ux_server_remove_port(socket_port);

		/*
		 *  vs_make_callback_port() created both a send and
		 *  receive right, so we have to deallocate both.
		 */
		kr = mach_port_deallocate(mach_task_self(),
					  socket_port);
		if (kr != KERN_SUCCESS)
			panic("vs_clear_callback_port(0x%x): m_p_dealloc: "
			      "kr 0x%x\n", vs, kr);
		kr = mach_port_mod_refs(mach_task_self(),
					socket_port,
					MACH_PORT_RIGHT_RECEIVE,
					-1);
		if (kr != KERN_SUCCESS)
			panic("vs_clear_callback_port(0x%x): m_p_mod_refs: "
			      "kr 0x%x\n", vs, kr);

		/* No more callback port! */
		vs->vs_magic = 0;
		vs->vs_flags &= ~VS_IS_PORT;
	}
#if	MACH_ASSERT
	else {
		mach_port_type_t	type;

		print_port_info((mach_port_t)vs, "vs_clr_cbk_port");
		panic("vs_clear_callback_port(0x%x) VS_IS_PORT not set!\n", vs);
		/*NOTREACHED*/
		kr = mach_port_type(mach_task_self(), (mach_port_t)vs, &type);
		ASSERT(kr == KERN_INVALID_NAME);
	}
#endif	/* MACH_ASSERT */
}


set_default_vsops(vs)
struct socket *vs;
{
	register vs_socket_t *rvs;
	register vs_socket_t *tmprvs;

	/* Sorry, no turning back! zzz */
	panic("set_default_vsops");
	/*NOTREACHED*/

	SOCKET_LOCK(vs);
	rvs = (vs_socket_t*)vs->vs_data;
	vs->vs_flags |= VS_ISBOUND;
	vs->vs_ops = &default_vsops;
	vs_clear_callback_port(vs);
	free_vs_opts(vs);
	vs->vs_data = NULL;
	while(rvs) {
		tmprvs = rvs;
		rvs = rvs->rvs_next;
		VS_FREE(tmprvs, VSM_RVS);
	}
	SOCKET_UNLOCK(vs);
}


/*
 *  Reference counting for non-networking virtual sockets.
 *  XXX See vs_nethold() below...
 */
int total_vsockets = 0;

int
vsocket_hold(
	struct socket *so)
{
	LOCK_ASSERT("vsocket_hold", SOCKET_ISLOCKED(so));
	if (++so->vs_refcnt == 1)
		total_vsockets++;
	VSDEBUG(VSDEBREFCNT,("vsocket_hold: refcnt = %d\n",so->vs_refcnt));
	return(ESUCCESS);
}


int
vsocket_release(
	struct socket *so)
{
	int	error = 0;
	int	refs;

	SOCKET_LOCK(so);
	refs = --so->vs_refcnt;
	SOCKET_UNLOCK(so);

	if (refs == 0) {
		error = soclose(so);
		total_vsockets--;
	}
	return error;
}


/*
 *  Reference counting for networking virtual sockets.
 *
 *  XXX What we need is a VSOP_TEARDOWN() virtual operation
 *  so that the same reference counting routines can be used
 *  for all types of virtual sockets.
 */
int
vs_nethold(
	struct socket *vs)
{
	LOCK_ASSERT("vs_nethold", SOCKET_ISLOCKED(vs));
	if (++vs->vs_refcnt == 1)
		total_vsockets++;
	return(ESUCCESS);
}


int
vs_netrelease(
	struct socket *vs)
{
	int	error = 0;
	int	refs;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);

	refs = --vs->vs_refcnt;
	if (refs == 0) {
		/*
		 *  o Free the ioctl and setsockoption chain.
		 *  o Close any remote secondary sockets.
		 *  o Clear this socket's callback port.
		 *  o Restore the default virtual socket operations, making
		 *	this a "normal" socket once more.
		 */
		(void) free_vs_opts(vs);
		(void) vs_close_remotes(vs, 0/*don't keep local rvs*/);
		vs_clear_callback_port(vs);
		vs->vs_ops = &default_vsops;

		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);

		/* Close this garden variety socket... */
		error = soclose(vs);

		total_vsockets--;
	} else {
		SOCKET_UNLOCK(vs);
		DOMAIN_UNFUNNEL(f);
		VSDEBUG(VSDEBREFCNT,("vs_netrelease count = %d\n",refs));
	}
		
	return error;
}


#include "sys/poll.h"

/*
 * This is the base operation which select check performs.
#ifdef	NOTYET
 * Access to this function is via the private vsocket operations
 * r_pvs_select_check or l_pvs_select_check in the
 * private vsocket operations structure.
#endif
 */
void
vs_select_check(
	struct socket	*so,
	short		events,
	short 		*revents)
{
	VSDEBUG(VSDEBSELECT,("vs_select_check 0x%x ",so));
	if (events & (POLLNORM|POLLPRI)) {
		SOCKBUF_LOCK(&so->so_rcv);
		if (events & POLLNORM) {
			if (soreadable(so))
				*revents |= POLLNORM;
			else if (*revents == 0)
				so->so_rcv.sb_flags |= SB_SEL;
		}
		if (events & POLLPRI) {
			if (so->so_oobmark ||
			    (so->so_state & SS_RCVATMARK))
				*revents |= POLLPRI;
			else if (*revents == 0)
				so->so_rcv.sb_flags |= SB_SEL;
		}
		SOCKBUF_UNLOCK(&so->so_rcv);
	}
	if (events & POLLOUT) {
		SOCKBUF_LOCK(&so->so_snd);
		if (sowriteable(so))
			*revents |= POLLOUT;
		else if (*revents == 0)
			so->so_snd.sb_flags |= SB_SEL;
		SOCKBUF_UNLOCK(&so->so_snd);
	}
	VSDEBUG(VSDEBSELECT,("events 0x%x revents 0x%x\n",events, *revents));
}


sel_id_t	vs_select_id_counter;
#ifdef	VSOCKET_DEBUG
int	vs_select_id_wraps = 0;
#endif

/*
 *  This assigns a unique id to a distributed select(2) call, and stores
 *  it and its associated delayed reply port in the selid/drp map on the
 *  primary socket's node.  The map entry address is returned.
 *
 *  Logically this routine belongs with the SELID_DRP_* macros in
 *  vs_subr.h , but I felt this was too much code to put in a macro.
 *  Anyway: this is the only routine allowed to call TWH_* macros on
 *  ht_selid_drp directly without going thru the SELID_DRP_*
 *  macros.
 */
selid_drp_map_t *
do_selid_drp_insert(
	struct file	*fp,		/* file struct of primary socket */
	mach_port_t	drp)		/* delayed reply port name */
{
	int		idcount;
	sel_id_t	selid;
	selid_drp_map_t	*sdm;
	int		wrapped = 0;
	int		rc;
	struct socket	*so = (struct socket *)fp->f_data;

	TWH_WRITE_LOCK(ht_selid_drp);

	/*
	 *  Compose a select id.
	 */
	for (;;) {
		if ( (idcount = ++vs_select_id_counter) > LOCSELIDMAX ) {
			if (wrapped)
				panic("do_selid_drp_insert: double wrap\n");
			idcount = vs_select_id_counter = 1;
			VSDEBUGX(VSDEBALLBITS, vs_select_id_wraps++);
			wrapped++;
		}
		selid = (this_node << SELNODESHIFT) | idcount;

		/*
		 *  Make sure the select id is not in use.
		 *  Note this lookup can return a scrubbed entry;
		 *  that's OK because it is in use.
		 */
		TWH_LOOKUP_1ST_KEY(ht_selid_drp, selid, sdm, selid_drp_map_t *);
		if (sdm == NULL) {
			/* Not in use! */
			break;
		}
		/* Loop to pick another id... */
	}

	/*
	 *  Create a map entry with this selid.
	 */
	TWH_MALLOC(sdm, selid_drp_map_t *, sizeof(selid_drp_map_t));
	bzero((caddr_t)sdm, sizeof(selid_drp_map_t));
	sdm->sdm_selid = selid;
	sdm->sdm_drp = drp;
	sdm->sdm_flags = 0;
	sdm->sdm_refcnt = 2;
	SDM_LOCK_INIT(sdm);

	/*
	 *  Existence of a map entry holds a reference to the primary socket,
	 *  *and* a ref on the file structure.  Just making darn sure nothing
	 *  gets pulled out from under us!
	 */
	sdm->sdm_fp = fp;
	sdm->sdm_so = so;
	INCREMENT_VSNET_REFCNT(so, "do_selid_drp_insert");

	/*
	 *  Insert the entry into the map.
	 */
	TWH_ENTER(ht_selid_drp, sdm, rc);
	if (rc != TRUE) {
		selid_drp_map_t	*sdm2;
		TWH_LOOKUP_2ND_KEY(ht_selid_drp, drp, sdm2, selid_drp_map_t *);
		if (sdm2) {
			/* If this happens, see the XXX comment in
			 * nsrv_r_vs_scrub_remote_selects(). */
			panic("do_selid_drp_insert: drp 0x%x already mapped by "
			      "sdm 0x%x (new sdm 0x%x)\n", drp, sdm2, sdm);
		} else {
			/* If this happens, you're on your own, bucko! */
			panic("do_selid_drp_insert: sdm 0x%x\n", sdm);
		}
	}

	TWH_UNLOCK(ht_selid_drp);
	return sdm;
}


/*
 *  This is a wrapper around the r_vs_select_enqueue() RPC.
 *
 *  It is responsible for managing the OSF base code's
 *  delay-port-to-file-port hash table, only updating it if we succeed
 *  in enqueueing something here *and* it is the first successful
 *  enqueue.  (This is work normally done by select_enqueue(), but if
 *  that routine won't be called here on the primary socket node then
 *  we must do the work here.)  The "enqueued" argument tells us
 *  whether a previous call has already enqueued something (and
 *  therefore whether select_enqueue() was called on this node).
 *
 *  Return TRUE iff this call succeeds in enqueueing.
 */
int
remote_vs_select_enqueue(
	vs_socket_t		*rvs,
	short			events,
	int			enqueued)
{
	struct uthread		*uth = current_thread();
	struct server_oip	*oipp = &uth->uu_oip;
	mach_port_t		ret_file_port;
	int			rc = TRUE;
	kern_return_t		kr;

	/* We should never get here if the drp is already dead. */
	ASSERT((uth->uu_sel_flags & SQ_DEAD) == 0);

	/*
	 *  If nothing's been successfully enqueued so far, then it's
	 *  our job to update the base code SEL_PORT hash table and
	 *  turn on deadname notification.  (But if called from a
	 *  remote sbsd_sel_poll_reply(), there is already a SEL_PORT
	 *  entry and d-n notification is already enabled, so don't do
	 *  it twice.)
	 */
	if (enqueued == 0 && (uth->uu_sel_flags & SQ_REMOTE_REPLY) == 0) {
		ASSERT(uth->uu_sel_delay_port != MACH_PORT_NULL);
		if ( !select_request_deadname(uth->uu_sel_delay_port,
					      uth->uu_sel_file_port) ) {
			/* Don't enqueue anything if the drp is already dead! */
			uth->uu_sel_flags |= SQ_DEAD;
			return FALSE;
		}
	}

	/*
	 *  Enqueue a select entry on a remote secondary.
	 *  The uu_sel_file_port is sent with MOVE_SEND
	 *  semantics.
	 */
	VSOP_SET_FORW(oipp, rvs->rvs_server_port);
	kr = r_vs_select_enqueue(rvs->rvs_server_port,
				 uth->uu_procp->p_cred,
				 uth->uu_sel_file_port,
				 uth->uu_sel_id,
				 uth->uu_oip.oip_transid,
				 uth->uu_sel_index,
				 events,
				 uth->uu_sel_again,
				 &ret_file_port);
	VSOP_END_FORW(oipp);
	if (kr != KERN_SUCCESS) {
		panic("remote_vs_sel_enq: r_vs_sel_enq: kr 0x%x\n", kr);
		/*NOTREACHED*/
		rc = FALSE;
	} else if (ret_file_port != MACH_PORT_NULL) {
		VSDEBUG(VSDEBERROR,
		    ("remote_vs_sel_enq: ret_file_port=0x%x\n", ret_file_port));
		ASSERT(ret_file_port == uth->uu_sel_file_port);
		rc = FALSE;
	}

	/*
	 *  If we requested deadname notification above and the remote
	 *  enqueue operation failed, we need to back out our update
	 *  of the SEL_PORT hash table and cancel deadname
	 *  notification.
	 */
	if (enqueued == 0 && rc == FALSE) {
		ASSERT(uth->uu_sel_delay_port != MACH_PORT_NULL);
		if ( !select_sel_port_remove(uth->uu_sel_delay_port) )
			panic("remote_vs_sel_enq: sel_port_remove(0x%x)\n",
			      uth->uu_sel_delay_port);
		if ( !select_cancel_deadname(uth->uu_sel_delay_port) )
			uth->uu_sel_flags |= SQ_DEAD;
	}

	return (rc);
}


/*
 *  This is a wrapper around the r_vs_select_dequeue() RPC.
 *
 *  It is responsible for managing the OSF base code's
 *  delay-port-to-file-port hash table, only updating it if we
 *  succeed in enqueueing something here *and* it is the first
 *  successful enqueue.  The INOUT argument "dn_cancelled" tells
 *  us whether a previous call has already dequeued something,
 *  and therefore cancelled d-n notification, etc.
 *
 *  Note that an RPC failure can set rvs->rvs_soerror.
 */
void
remote_vs_select_dequeue(
	vs_socket_t		*rvs,
	int			*dn_cancelled)
{
	struct uthread		*uth = current_thread();
	struct server_oip	*oipp = &uth->uu_oip;
	mach_port_t		ret_file_port;
	int			isreadable = FALSE;
	kern_return_t		kr;

	if (*dn_cancelled == FALSE) {
		/*
		 *  If nothing's been successfully dequeued yet, then
		 *  it's our job to update the base code SEL_PORT hash
		 *  table and turn off deadname notification.
		 */
		if ( !select_sel_port_remove(uth->uu_sel_delay_port) )
			panic("remote_vs_sel_deq: sel_port_remove(0x%x)\n",
			      uth->uu_sel_delay_port);
		if ( !select_cancel_deadname(uth->uu_sel_delay_port) )
			uth->uu_sel_flags |= SQ_DEAD;

		/* Tell next call to this wrapper not to take this path. */
		*dn_cancelled = TRUE;
	}

	/*
	 *  Dequeue select entries on a remote secondary, and return
	 *  the secondary's state.
	 */
	VSOP_SET_FORW(oipp, rvs->rvs_server_port);
	kr = r_vs_select_dequeue_getstate(rvs->rvs_server_port,
					  uth->uu_procp->p_cred,
					  uth->uu_sel_id,
					  uth->uu_sel_flags,
					  uth->uu_sel_index,
					  &rvs->rvs_state,
					  &rvs->rvs_soerror,
					  &rvs->rvs_qlen,
					  &isreadable);
	VSOP_END_FORW(oipp);
	if (kr != KERN_SUCCESS) {
		VSDEBUG(VSDEBERROR,
		    ("remote_vs_select_dequeue: r_vs_select_dequeue_getstate: "
		     "selid=0x%x index=%d kr=0x%x\n",
		     uth->uu_sel_id, uth->uu_sel_index, kr));
		rvs->rvs_soerror = vs_map_error(kr);
		/*
		 *  Note we don't set *dequeued to FALSE here even though
		 *  we didn't dequeue, because we've already done the
		 *  necessary pre-dequeue processing.
		 */
	} else if (isreadable) {
		rvs->rvs_flags |= VS_READABLE;
	}
}


/*
 * This routine sets the local (if any) qlen and collapses all the
 * local and remote socket state into the primary socket.
 */
vs_collapse_state(vs)
struct socket *vs;
{

	vs_socket_t	*rvs;
	int	state = vs->so_state;
	int	qlen = 0;
	int	mask;			/* bits to set and clear */
	int	soerror = vs->so_error;;

	LOCK_ASSERT("vs_collapse_state", SOCKET_ISLOCKED(vs));	

	for(rvs = (vs_socket_t *)vs->vs_data; rvs; rvs = rvs->rvs_next) {
		if(!(rvs->rvs_flags & VS_USE)) 
			continue;
		if(rvs->rvs_server_port == MACH_PORT_NULL) {
			/*
			 * 1. Compute bits that changed.
			 * 2. AND with new state to get bits to set.
			 * 3. Set 'em.
			 */
			mask = (vs->so_state ^ vs->vs_oldstate);  /* 1. */
			mask &= vs->so_state;	/* 2. */
			state |= mask;		/* 3a. */
			rvs->rvs_state |= mask;	/* 3b. */
			/*
			 * 1. Recompute bits that changed.
			 * 2. AND with _old_ state to get bits to clear.
			 * 3. Clear 'em.
			 */
			mask = (vs->so_state ^ vs->vs_oldstate);  /* 1. */
			mask &= vs->vs_oldstate;	/* 2. */
			state &= ~mask;			/* 3a. */
			rvs->rvs_state &= ~mask;		/* 3b. */
			/*
			 * set the qlen for any new waiting connections
			 * We only increment the local qlen here, 
			 * the qlen is decremented in vs_netsodequeue
			 */
			ASSERT(vs->so_qlen >= vs->vs_oldqlen);
			VSDEBUG(VSDEBCST,
				("vs_collapse_state: vs 0x%x rvs 0x%x "
				"so_qlen %d rvs_qlen %d vs_oldqlen %d\n",
				 vs, rvs, vs->so_qlen, rvs->rvs_qlen,
				 vs->vs_oldqlen));

			rvs->rvs_qlen += vs->so_qlen - vs->vs_oldqlen;
			qlen += rvs->rvs_qlen;
		} else {
			/*
			 * Turn on the bits that have been turned on...
			 *
			 * (Note it's not as tricky here because the
			 * rvs state bits are known to be correct for
			 * the remote socket, while above we are dealing
			 * with the "composite" bits for *all* sockets.)
			 */
			state |= ((rvs->rvs_state ^ rvs->rvs_ostate) &
				  rvs->rvs_state);
			/*
			 * ...and turn off the bits that have been cleared.
			 */
			state &= ~((rvs->rvs_state ^ rvs->rvs_ostate) &
				   rvs->rvs_ostate);

			qlen += rvs->rvs_qlen;

			rvs->rvs_ostate = rvs->rvs_state;
			if(!soerror)
				soerror = rvs->rvs_soerror;
		}
	}
	vs->vs_oldqlen = vs->so_qlen = qlen;
	vs->vs_oldstate = vs->so_state;
	vs->so_state = state;
	vs->so_error = soerror;
}


/*
 *  This routine finds the best server to use for a particular
 *  target address by querying the clearinghouse.  It is called from
 *  send(2) or connect(2) virtual socket ops when the local socket
 *  has not yet been bound to a node.
 */
int
find_one_server(
	struct socket	*vs,		/* vsocket to connect */
	struct sockaddr *saddr,		/* target address */
	vs_socket_t	**rvsp)		/* rvs to update w/ netserver info */
{
	mach_port_t	server_list[1];
	mach_port_t	remote_port;
	node_t		remote_node;
	struct socket	*remote_addr;
	vs_socket_t	*rvs;
	mach_port_t	vs_port;
	int		i, rval;
	boolean_t	found = FALSE;
	mach_msg_type_number_t
			numnetservers = 1;
	kern_return_t	kr;
	node_t		host_node;
	struct uthread	*uth = &u;
	struct server_oip *oipp = &uth->uu_oip;
	DOMAIN_FUNNEL_DECL(f)

	ASSERT(rvsp != NULL);

	/* set the family (sometimes apps forget) */
	if ( !saddr->sa_family )
		saddr->sa_family = vs->so_proto->pr_domain->dom_family;

	ASSERT(saddr->sa_family != AF_UNSPEC);

	/*
	 * Locate the clearinghouse before we attempt to
	 * use it.
	 */
	rval = find_clearinghouse();
	if (rval != ESUCCESS) {
		VSDEBUG(VSDEBERROR,
		    ("find_one_server: find_clearinghouse: rval %d\n", rval));
		return rval;
	}
			
	/*
	 * Try to find the network interface that we
	 * need to reach saddr. First, initialize
	 * server_list.
	 */
	server_list[0] = MACH_PORT_DEAD;
	VSDEBUG(VSDEBFOS,
	    ("find_one_server: trying to find %s\n", sockaddr_to_string(saddr)));

	ux_server_thread_blocking();
	kr = if_get_server_by_route(clearinghouse_port,
				    this_node,
				    (char *)saddr,
				    saddr->sa_len,
				    &host_node,
				    &server_list[0]);
	ux_server_thread_unblocking();
	if (kr != KERN_SUCCESS) {
		VSDEBUG(VSDEBERROR,
			("find_one_server: if_get_server_by_route: "
			 "kr 0x%x, ret_node %d\n", kr, host_node));
		rval = vs_map_error(kr);
		return rval;
	}

	if (host_node == INVALID_NODE || numnetservers == 0) {
		/*
		 *  No network servers appropriate for this sockaddr
		 *  have been registered with the clearinghouse.
		 */
		VSDEBUG(VSDEBERROR,
			("find_one_server: if_get_server_by_route: "
			 "rval %d, host_node %d, numnetservers %d\n",
			 rval, host_node, numnetservers));
		return EADDRNOTAVAIL;
	} else {
		VSDEBUG(VSDEBFOS,
			("find_one_server: host_node %d\n", host_node));
	}

	/*
	 *  We now have the node number and network server port of the
	 *  node with the most appropriate interface. Loop thru the
	 *  rvs chain and see if we already have a socket on the
	 *  desired node.
	 *
	 *  If the VS_ISBOUND flag is set, we are already associated
	 *  with one or more network servers. If VS_ISBOUND is not
	 *  set, no such relationship has been established.
	 *
	 *  If there is no existing association, just use the rvs
	 *  entry that is already allocated.
	 *
	 *  If there are existing associations, check to see if we
	 *  are already bound to the desired network server. If 
	 *  not, allocate an rvs and create a remote secondary, i.e.
	 *  create a new association.
	 */
	DOMAIN_FUNNEL(sodomain(vs), f);
	SOCKET_LOCK(vs);
	if (vs->vs_flags & VS_ISBOUND) {
		/* Try to find an existing rvs for host_node. */
		for (rvs = (vs_socket_t*)vs->vs_data; rvs; 
		     rvs = rvs->rvs_next) {
			VSDEBUG(VSDEBFOS,
				("find_one_server: rvs 0x%x, server_port 0x%x, "
				 "server_node %d, rvs_flags %d\n",
				 rvs, rvs->rvs_server_port,
				 rvs->rvs_server_node, rvs->rvs_flags));
			if (rvs->rvs_server_node == host_node) {
				found = TRUE;
				break;
			}
		}
		if (rvs == NULL) {
			rvs = vs_get_new_rvs(vs);
			VSDEBUG(VSDEBFOS,
				("find_one_server: new rvs 0x%x\n", rvs));
			if (rvs == NULL) {
				VSDEBUG(VSDEBERROR,
				("find_one_server: vs_get_new_rvs: ENOMEM\n"));
				SOCKET_UNLOCK(vs);
				DOMAIN_UNFUNNEL(f);
				return ENOMEM;
			}
		}
	} else {
		/* Use the rvs created by VSOP_SOCREATE(). */
		rvs = (vs_socket_t *)vs->vs_data;
		VSDEBUG(VSDEBFOS, ("find_one_server: using original rvs\n"));
	}

	/*
	 * If server_list[0] == MACH_PORT_NULL, the local node is
	 * the network server to be used.  In this case it is not
	 * necessary to create a remote secondary socket.
	 *
	 * If server_list[0] != MACH_PORT_NULL, the appropriate
	 * network interface is on another node. If found is
	 * FALSE, there is no existing association with that
	 * network server, i.e. no remote secondary socket.
	 * In that case, we create one.
	 */
	if (found) {
		/* We already have a remote secondary and know our port name. */
		remote_port = rvs->rvs_server_port;
		ASSERT(host_node == rvs->rvs_server_node);
	} else if (server_list[0] == MACH_PORT_NULL) {
		/* The clearinghouse says we should use the local node. */
		remote_port = MACH_PORT_NULL;
		ASSERT(host_node == this_node);
	} else {
		/* We need to create a new remote secondary. */
		VSOCK_TO_PORT_LOOKUP(vs, vs_port);
		VSOP_SET_FORW(oipp, server_list[0]);
		kr = r_vs_socreate(server_list[0],
				      uth->uu_procp->p_cred,
				      vs_port,
				      oipp->oip_transid,
				      sodomain(vs)->dom_family,
				      &remote_port,
				      vs->so_type,
				      vs->so_proto->pr_protocol,
				      (int *)&remote_addr,
				      &rval);
		VSOP_END_FORW(oipp);
		if (kr != KERN_SUCCESS) {
			VSDEBUG(VSDEBERROR,
				("find_one_server: r_vs_socreate: kr 0x%x\n",
				 kr));
			rval = vs_map_error(kr);
		}
		if (rval != ESUCCESS) {
			VSDEBUG(VSDEBERROR,
			   ("find_one_server: r_vs_socreate: rval %d\n", rval));
			return rval;
		} else if (remote_port == MACH_PORT_NULL) {
			VSDEBUG(VSDEBERROR,
				("find_one_server: r_vs_socreate: "
				 "remote_port == MACH_PORT_NULL\n"));
			return EPROTONOSUPPORT;
		}
	}

	rvs->rvs_server_port = remote_port;
	rvs->rvs_server_node = host_node;
	rvs->rvs_remote_so = remote_addr;
	vs->vs_flags |= VS_ISBOUND;
	if (remote_port != MACH_PORT_NULL)
		vs->vs_flags |= VS_IS_REMOTE;
	
	VSDEBUG(VSDEBFOS, ("find_one_server: returning port 0x%x, "
			    "node %d, vs_flags 0x%x rvs flags 0x%x\n",
			    remote_port, host_node, vs->vs_flags,
			    rvs->rvs_flags));
	*rvsp = rvs;
	SOCKET_UNLOCK(vs);
	DOMAIN_UNFUNNEL(f);
	return ESUCCESS;
}


/*
 * Name:
 *	find_network_servers
 *
 * Function:
 *	Query the clearinghouse node.  Get a list of the
 *	configured network server nodes and their associated
 *	control ports.  Use RPC if necessary.
 *
 * Inputs:
 *	nodes	-	ptr to array of node_t
 *	ports	-	ptr to array of mach_port_t
 *	nel	-	number of elements in nodes and ports arrays
 *	
 * Outputs:
 *	numns	-	ptr to int. set to total # of network server
 *			nodes configured
 *	rel	-	number of returned elements, i.e. # of elements
 *			in nodes and ports array that contain interesting
 *			information.
 *
 * Returns:
 *	ESUCCESS
 *	EINVAL	-	invalid paramter
 */
int
find_network_servers(
	int 		nel,
	int 		*numns,
	int 		*rel,
	node_t 		*nodes,
	mach_port_t	*ports)
{
	kern_return_t	kr;
	int		i;
	int		rc;
	node_t		*np;
	mach_port_t	*mp;
	mach_msg_type_number_t		node_num;
	mach_msg_type_number_t		port_num;
	struct uthread	*uth = current_thread();
	struct server_oip *oipp = &uth->uu_oip;

	extern node_t		clearinghouse_node;
	extern mach_port_t	clearinghouse_port;
	
	/* verify our input parameters */
	if (numns == NULL || rel == NULL || nodes == NULL || ports == NULL)
		return(EINVAL);

	/* first initialize the buffers */
	for(i = 0, np = nodes, mp = ports; i < nel; i++, np++, mp++) {
		*np = INVALID_NODE;
		*mp = MACH_PORT_NULL;
	}
	
	/*
	 * If we are the clearinghouse node, just call chouse_list(),
	 * else we must do an rpc
	 */
	if (this_node == clearinghouse_node) {
		rc = chouse_list(&netserv_chouse, this_node, nel,
				 numns, rel, nodes, ports);
	} else {
		rc = find_clearinghouse(); 
		if (rc != ESUCCESS) {
		    VSDEBUG(VSDEBERROR,
			("find_network_servers: find_clearinghouse: rc=%d\n",
			 rc));
		    return (rc);
		}			
		node_num = nel;
		port_num = nel;
		VSOP_SET_FORW(oipp, clearinghouse_port);
		kr = if_list(clearinghouse_port,
			     this_node,
			     nel,
			     numns,
			     rel,
			     nodes,
			     &node_num,
			     ports,
			     &port_num,
			     &rc);
		VSOP_END_FORW(oipp);
	}

	return(rc);
}
	

int vsnb_yd = 0;	/* calls to thread_yield for same MJL */


vs_socket_t *
vs_soreadable(
	struct socket	*vs,
	boolean_t	nbio)
{

	vs_socket_t	*rvs;
	kern_return_t	kret;
	int		isreadable = 0;
	boolean_t	result;
	struct uthread 	*uth = &u;
	int		error = ESUCCESS;
	DOMAIN_FUNNEL_DECL(f)

	/*
	 * Determine if a virtual socket has any readable data
	 */
	ASSERT(vs->vs_flags & VS_IS_MULTI);
	LOCK_ASSERT("vs_soreadable", SOCKET_ISLOCKED(vs));

	for (rvs = (vs_socket_t *)vs->vs_data; rvs; rvs = rvs->rvs_next) {
		VSDEBUG(VSDEBSOREADABLE,
			("vs_soreadable: rvs 0x%x rvs_flags 0x%x\n",
			rvs, rvs->rvs_flags));
		if ((rvs->rvs_flags & VS_USE) == 0)
			continue;

		if (rvs->rvs_flags & VS_READABLE) {
			return rvs;
		}
	}

	/*
	 * Loop through the secondary sockets 
	 * We hold the lock to prevent events occuring in the window
	 * between the times we check readability 
	 * and the time that sleep is called.
	 */
restart:
	for (rvs = (vs_socket_t *)vs->vs_data; rvs; rvs = rvs->rvs_next) {
		if ((rvs->rvs_flags & VS_USE) == 0)
			continue;
		if (rvs->rvs_server_port == MACH_PORT_NULL) {
			if (soreadable(vs))  {
				VSDEBUG(VSDEBSOREADABLE,
					("vs_soreadable: vs 0x%x rvs 0x%x"
					 " sb_cc %d sb_lowat %d so_state %d"
					 " so_qlen %d so_error %d\n",
					 vs, rvs, vs->so_rcv.sb_cc,
					 vs->so_rcv.sb_lowat,
					 vs->so_state, vs->so_qlen,
					 vs->so_error));
				rvs->rvs_flags |= VS_READABLE;
				isreadable++;
			}
			else
			    VSDEBUG(VSDEBSOREADABLE,
				    ("vs_soreadable: local rvs unreadable\n"));
		} else {
			/*
			 * check to see if we have previously queued
			 * a check to see if the secndary is readable
			 * if not put the check in the mail
			 */
			if(!(rvs->rvs_flags & VS_READ_CHECK)) {
				rvs->rvs_flags |= VS_READ_CHECK;
				if (kret = r_vs_soreadable(rvs->rvs_server_port,
				    uth->uu_procp->p_cred,
				    rvs->rvs_server_port,
				    ++rvs->rvs_read_index)) {
					vs->so_error = vs_map_error(kret);
				}
			}
		}
	}
	if(!isreadable && !nbio ) {
		vs->so_error = sosbwait(&vs->so_rcv,vs);
		VSDEBUG(VSDEBSOREADABLE,
			("vs_soreadable: !isreadable && !nbio "
			"vs 0x%x so_error %d\n", vs, vs->so_error));
	}	
	for (rvs = (vs_socket_t *)vs->vs_data; rvs; rvs = rvs->rvs_next) {
		if ((rvs->rvs_flags & VS_USE) == 0)
			continue;
		if (rvs->rvs_flags & VS_READABLE) {
			return rvs;
		}
	}
	if(nbio) {
		vs->so_error = EWOULDBLOCK;
		VSDEBUG(VSDEBSOREADABLE,
			("vs_soreadable: set EWOULDBLOCK\n"));
	}		
	if((vs->so_error == ESUCCESS))
		goto restart;

	return (vs_socket_t *)0;
}


/*
 *  Routine to close the secondary sockets for a particular primary socket.
 *
 *  In some cases (i.e. vs_netsobind() ) we do not want to destroy the
 *  rvs structure corresponding to the local secondary.  (Indeed, we
 *  want to create one if none exists.)  The intent in such cases is
 *  that we are backing out from a failed attempt to bind to multiple
 *  network servers---we do not wish to disturb the state of the
 *  primary socket, we only want to get rid of the secondaries.
 *  These cases are indicated by keep == TRUE.
 *
 *  Returns the last errno encountered, or ESUCCESS.
 */
int
vs_close_remotes(
	struct socket	*vs,	/* primary socket with secondaries to close */
	int		keep)	/* TRUE => keep local secondary's rvs if any */
{
	vs_socket_t	*rvs, *rvs2;
	vs_socket_t	*local_rvs	= NULL;
	int		error		= ESUCCESS;
	int		tmperror	= ESUCCESS;
	kern_return_t	kr;
	struct uthread	*uth		= &u;
	struct server_oip *oipp		= &uth->uu_oip;

	LOCK_ASSERT("vs_close_remotes", SOCKET_ISLOCKED(vs));

	/* Take complete responsibility for this rvs chain. */
	rvs = (vs_socket_t*)vs->vs_data;
	vs->vs_data = NULL;

	while (rvs != NULL) {

		if (rvs->rvs_server_port == MACH_PORT_NULL) {
			/* Maybe save local secondary for later... */
			if (keep)
				local_rvs = rvs;
		} else {
			/*
			 *  Tear down a remote secondary socket.
			 */
			VSOP_SET_FORW(oipp, rvs->rvs_server_port);
			kr = r_vs_soclose(rvs->rvs_server_port,
					  uth->uu_procp->p_cred,
					  oipp->oip_transid,
					  &tmperror);
			VSOP_END_FORW(oipp);
			if (kr != KERN_SUCCESS) {
				VSDEBUG(VSDEBERROR,
			    ("vs_close_remotes(0x%x): r_vs_soclose: kr 0x%x\n",
					 vs, kr));
				error = vs_map_error(kr);
			} else if (tmperror) {
				VSDEBUG(VSDEBERROR,
			     ("vs_close_remotes(0x%x): r_vs_soclose: err %d\n",
					 tmperror));
				error = vs_map_error(tmperror);
			}

			/*
			 *  Clean up our port referrence to the remote
			 *  socket we just tore down.
			 */
			kr = mach_port_deallocate(mach_task_self(),
						  rvs->rvs_server_port);
			if (kr != KERN_SUCCESS)
			    VSDEBUG(VSDEBERROR,
				("vs_close_remotes(0x%x): m_p_dealloc 0x%x:"
				 " kr 0x%x\n", vs, rvs->rvs_server_port, kr));

			/*
			 * If the remote socket was in use, clear the
			 * SS_ISCONNECTED flag.  This is to keep the final
			 * local close from hanging waiting for the
			 * connection to close.  Clearing the flag only
			 * matters if SO_LINGER is set and if
			 * the lingering was done on the remote node.
			 */
			if (rvs->rvs_flags & VS_USE)
				vs->so_state &= ~SS_ISCONNECTED;
		}

		/*
		 *  If we don't want to keep it, free the rvs.
		 */
		if (rvs != local_rvs) {
			rvs2 = rvs;
			rvs = rvs->rvs_next;
			VS_FREE(rvs2, VSM_RVS);
		} else {
			ASSERT(keep);
			rvs = rvs->rvs_next;
		}
	}

	/*
	 *  All the remote secondaries are gone.  If there is a local
	 *  secondary that is to be kept, it is local_rvs.  If we are
	 *  keeping but there is no local_rvs, we need to make one!
	 */
	if (local_rvs != NULL) {
		local_rvs->rvs_next = NULL;
		vs->vs_data = (caddr_t) local_rvs;
	} else if (keep) {
		/* Since vs_get_new_rvs() appends, and only one rvs wanted... */
		ASSERT(vs->vs_data == NULL);
		rvs = vs_get_new_rvs(vs);
		if (rvs == NULL) {
			VSDEBUG(VSDEBERROR,
			      ("vs_close_remotes(0x%x): vs_alloc_rvs failed\n",
			       vs));
			return (ENOMEM);
		}
		ASSERT(vs->vs_data == (caddr_t) rvs);
	} else {
		vs->vs_data = NULL;
	}

	return error;
}


/*
 * Return the node number of the remote virtual socket.  If the node number
 * is not known locally, update it from the remote. 
 *
 *  XXX This is totally bogus---the r_vs_socreate() RPC should return
 *  a node number to be stored in the rvs, and that should be the end
 *  of it.   mjl
 */
node_t
vs_rvs_node_number(rvs)
vs_socket_t *rvs;
{
	node_t	remote_node;
	kern_return_t	kret;

	if(rvs->rvs_server_node == INVALID_NODE) {
		VSDEBUG(0/*VSDEBERROR*/,
			("vs_rvs_node_number(0x%x): rvs_server_node not set\n",
			 rvs));
		if(rvs->rvs_server_port == MACH_PORT_NULL) {
			remote_node = this_node;
		} else {
			kret = norma_port_location_hint(mach_task_self(),
							rvs->rvs_server_port,
							&remote_node);
			if(kret != KERN_SUCCESS) {
				VSDEBUG(VSDEBERROR,
				    ("vs_rvs_node_number: Hint failed %x\n",
				     kret));
				remote_node = INVALID_NODE;
			} 
		}
		rvs->rvs_server_node = remote_node;
	}
	return (rvs->rvs_server_node);
}
				
			
/*
 *  Wrapper for virtual socket remote ioctl() calls.  This is needed
 *  because MiG doesn't support variable length inout array arguments.
 */
kern_return_t
remote_vs_ioctl(
	mach_port_t	server_port,
	mach_port_t	creds_port,
	transaction_id_t transid,
	int		cmd,
	caddr_t		data,
	int		*rc)
{
	kern_return_t	kr;
	int		len = IOCPARM_LEN(cmd);

	ASSERT(len < VS_MAXIOCLEN);

	kr = r_vs_ioctl(server_port,
			creds_port,
			transid,
			cmd,
			data, (mach_msg_type_number_t)len,	/* in */
			data, (mach_msg_type_number_t *)&len,	/* out */
			rc);

	/*
	 *  If an IOC_OUT ioctl succeeded, the returned length must
	 *  be == IOCPARM_LEN(cmd).
	 */
	ASSERT(kr != KERN_SUCCESS || *rc != ESUCCESS ||
	       ((cmd & IOC_OUT) == 0) || len == IOCPARM_LEN(cmd));

	return (kr);
}


#if	MACH_ASSERT
/*
 *  Heap allocation statistics.
 */

int	vsm_count[VSM_MAX];
decl_simple_lock_data(,vsm_count_lock)

void
increment_vsm_count(
	int	type)
{
	ASSERT(type < VSM_MAX);
	simple_lock(&vsm_count_lock);
	vsm_count[type]++;
	simple_unlock(&vsm_count_lock);
}

void
decrement_vsm_count(
	int	type)
{
	ASSERT(type < VSM_MAX);
	simple_lock(&vsm_count_lock);
	vsm_count[type]--;
	simple_unlock(&vsm_count_lock);
}


#ifdef	STALE_DEBUG_CODE

/*
 *  Debug routines to print selid-to-drp two-way hash table.
 */

#define SEL2DRP_COLS	4

int
selid2drp_k2d(
	port_hash_entry_t	k2d,	/* key-to-data hash entry */
	int			arg)	/* addr of column counter */
{
	char			*val;
	char			ch, ch2;
	int			*colp = (int *)arg;
	kern_return_t		kr;
	mach_port_type_t	type;

	/* Reverse lookup up the data in the data-to-key hash table. */
	val = port_hash_lookup(ht_selid_drp.twh_data_to_key,
			       (mach_port_t) k2d->value);
	if ( val == NULL ) {
		ch = '*';	/* no reverse data-->key mapping! */
	} else if ( val == (char *)k2d->port ) {
		ch = '<';	/* reverse mapping exists, no problem. */
	} else {
		ch = '?';	/* data doesn't map back to key, ouch! */
	}

	/* Find status of this drp. */
	kr = mach_port_type(mach_task_self(), (mach_port_t) k2d->value, &type);
	if (kr == KERN_INVALID_NAME) {
		ch2 = '#';	/* no such right */
	} else if (kr != KERN_SUCCESS) {
		ch2 = 'E';	/* unexpected error */
	} else if (type & MACH_PORT_TYPE_DEAD_NAME) {
		ch2 = 'D';
	} else if (type & MACH_PORT_TYPE_SEND_ONCE) {
		ch2 = ' ';
	} else
		ch2 = 'B';	/* extremely bogus */

	/* Print mapping. */
	if ( (k2d->port >> SELNODESHIFT) != this_node ) {
		/* Non-local selids shouldn't be in the selid/drp table! */
		printf("\n*** node=%d : ", (k2d->port >> SELNODESHIFT));
		*colp = SEL2DRP_COLS - 1;
	}
	printf(" %04x%c->%08x%c ", (k2d->port & ((1<<SELNODESHIFT)-1)),
	       ch, k2d->value, ch2);

	/* Crlf every four mappings so it fits in 80 columns. */
	*colp += 1;
	if ( (*colp % SEL2DRP_COLS) == 0 )
		printf("\n");

	return 0;
}


int
selid2drp_d2k(
	port_hash_entry_t	d2k,	/* data-to-key hash entry */
	int			arg)	/* addr of column counter */
{
	char			*val;
	char			ch;
	int			*colp = (int *)arg;

	/* Reverse lookup up the key in the key-to-data hash table. */
	val = port_hash_lookup(ht_selid_drp.twh_key_to_data,
			       (mach_port_t) d2k->value);
	if ( val == NULL ) {
		ch = '*';	/* no reverse data-->key mapping! */
	} else if ( val == (char *)d2k->port ) {
		return 0;	/* this was already printed by selid2drp_k2d */
	} else {
		ch = '?';	/* data doesn't map back to key, ouch! */
	}

	/* Print mapping.  NB key always appears first! */
	if ( ((node_t)d2k->value >> SELNODESHIFT) != this_node ) {
		/* Non-local selids shouldn't be in the selid/drp table! */
		printf("\n*** node=%d : ",
		       ((node_t)d2k->value >> SELNODESHIFT));
		*colp = SEL2DRP_COLS - 1;
	}
	printf(" %04x%c->%08x", ((node_t)d2k->value & ((1<<SELNODESHIFT)-1)),
	       ch, d2k->port);

	/* Crlf every four mappings so it fits in 80 columns. */
	*colp += 1;
	if ( (*colp % SEL2DRP_COLS) == 0 )
		printf("\n");

	return 0;
}


void
selid2drp_print(
	two_way_hash_t		*twh)
{
	char			ch;
	caddr_t			val;
	int			columns;

	/* Not yet rewritten to deal with new two-way hash implementation. */
	panic("selid2drp_print not yet rewritten!\n");
	/*NOTREACHED*/

	printf("[key-->data for node %d]\n", this_node);
	columns = 0;
	(void) port_hash_walk(twh->twh_key_to_data,
			      selid2drp_k2d,
			      (int)&columns);
	if ( (columns % SEL2DRP_COLS) != 0 )
		printf("\n");

	printf("[orphaned data-->key for node %d]\n", this_node);
	columns = 0;
	(void) port_hash_walk(twh->twh_data_to_key,
			      selid2drp_d2k,
			      (int)&columns);
	if ( (columns % SEL2DRP_COLS) != 0 )
		printf("\n");
}


/*
 *  Check the selid/drp map for dead drps.
 */
int
check_drp(
	port_hash_entry_t	d2s,
	int			arg)
{
	int			*numdead = (int *)arg;
	selid_drp_map_t		*sdm = (selid_drp_map_t *)d2s->value;
	kern_return_t		kr;
	mach_port_type_t	type;

	kr = mach_port_type(mach_task_self(),
			    (mach_port_t)sdm->sdm_drp, &type);
	if (kr == KERN_INVALID_NAME) {
		printf("selid/drp %x/%x BAD DRP (sdm 0x%x)\n",
		       sdm->sdm_selid, sdm->sdm_drp, sdm);
		*numdead += 1;
	}
}


int
report_deaths(
	char	*str)
{
	int		dead_count = 0;
	struct uthread	*uth = current_thread();

	(void) port_hash_walk(ht_selid_drp.twh_data_to_key,
			      check_drp,
			      (int)&dead_count);
	if (dead_count) {
		printf("There are %d missing DRPs: %s\n", dead_count, str);
		printf("uth 0x%x uu_sel_id 0x%x uu_sel_flags 0x%x\n",
		       uth, uth->uu_sel_id, uth->uu_sel_flags);
	}
	return dead_count;
}

#endif	/* STALE_DEBUG_CODE */

#endif	/* MACH_ASSERT */

int
vs_bind_remotes(
	struct socket	*vs,
	int		numnetservers, 
	mach_port_t	*server_list,
        struct mbuf     *nam)           /* name to bind to */
{

	int			i;
	int			error = ESUCCESS;
	vs_socket_t		*rvs;
	struct uthread		*uth = &u;
	struct server_oip	*oipp = &uth->uu_oip;
	struct socket		*raddr;
        struct sockaddr         *saddr;
	mach_port_t		rport;
	mach_port_t		vs_port;
	kern_return_t		kr;
	u_short			port;

	extern int		vs_optimize_for_local_ns;

        saddr = (nam ? mtod(nam, struct sockaddr *) : NULL);
        for ( i = 0; i < numnetservers ; i++ ) {

                /*
                 * If it's the first one, rvs will point to the first
                 * remote virtual socket on the list which was created
                 * in vs_netsocreate().  Otherwise vs_get_new_rvs()
                 * appends the remote virtual socket to the end of the
                 * remote virtual socket chain.
                 */
                rvs = (i == 0
                       ? (vs_socket_t *)vs->vs_data
                       : vs_get_new_rvs(vs));
                if (rvs == NULL) {
                        VSDEBUG(VSDEBERROR,
                                ("vs_netsobind(0x%x): no memory\n", vs));
			return ENOMEM;
                }

                /*
                 *  One of the network servers may be the local node.
                 */
                if (server_list[i] == MACH_PORT_NULL) {
                        /*
                         *  Optimization: if there is only one
                         *  network server and it is the local
                         *  node, just use the default virtual
                         *  socket ops from here on.  NB when
                         *  primary sockets are made to follow
                         *  migrating processes, this won't be
                         *  possible anymore.  XXX
                         *  We have to set up the vs_socket_t structure
                         *  so that it can be torn down correctly
                         */


                         if (vs_optimize_for_local_ns && (numnetservers== 1)) {
                                set_default_vsops(vs);
                                return VSOP_BIND(vs,nam);
                         }

                        if (vs->vs_flags & VS_BINDLOCAL)
				break;
                        else
                                continue;
                }

                /*
                 *  We have a remote network server, so create a secondary
                 *  socket for it, bind the secondary, and add it to the
                 *  chain.
                 */
                VSOCK_TO_PORT_LOOKUP(vs, vs_port);
                VSOP_SET_FORW(oipp, server_list[i]);
                kr = r_vs_socreate(server_list[i],
                                   uth->uu_procp->p_cred,
                                   vs_port,
                                   uth->uu_oip.oip_transid,
                                   sodomain(vs)->dom_family,
                                   &rport,
                                   vs->so_type,
                                   vs->so_proto->pr_protocol,
                                   (int *) &raddr,
                                   &error);
                VSOP_END_FORW(oipp);
                if (kr != KERN_SUCCESS) {
                        VSDEBUG(VSDEBERROR,
                            ("vs_netsobind: r_vs_socreate: kr 0x%x\n", kr));
                        return (vs_map_error(kr));
                }
                if (error != ESUCCESS) {
                        VSDEBUG(VSDEBERROR,
                            ("vs_netsobind: r_vs_socreate: err %d\n", error));
                        return (error);
                }

                ASSERT(rport != MACH_PORT_NULL);
                rvs->rvs_server_port = rport;
                rvs->rvs_remote_so = raddr;
                kr = norma_port_location_hint(mach_task_self(),
                                              rport,
                                              &rvs->rvs_server_node);
                if (kr != KERN_SUCCESS)
                        panic("vs_netsobind:norma_port_location_hint(%x): "
                              "kr 0x%x\n", rport, kr);
                replay_vs_opts(vs, rvs);

                VSOP_SET_FORW(oipp, rvs->rvs_server_port);
                kr = r_vs_sobind(rvs->rvs_server_port,
                                    uth->uu_procp->p_cred,
                                    uth->uu_oip.oip_transid,
                                    (char *)saddr,
                                    (saddr ? saddr->sa_len : NULL),
                                    &error);
                VSOP_END_FORW(oipp);
                if (kr != KERN_SUCCESS) {
                        VSDEBUG(VSDEBERROR,
                                ("vs_netsobind: r_vs_sobind: kr %d\n",kr));
                        return (vs_map_error(kr));
                }
		if (error != ESUCCESS)
			return error;

		vs->vs_flags |= (VS_ISBOUND | VS_IS_REMOTE);
		VSDEBUGX(VSDEBNODEBIND, 
			vs_show_node_binding(vs, rvs->rvs_server_port));

        } /* end of for loop */

	return ESUCCESS;

} /* end of bind_remotes */

int
vs_bind_local(
	struct socket		*vs,
	int			numnetservers,
        struct mbuf     	*nam,           /* name to bind to */
	struct domain_funnel	*f)		/* for DOMAIN_FUNNEL, etc. */  
{	

	struct vs_socket	*local_rvs;
	int			error;	

	/*
	 * See if we have to do a local bind
	 */
	for (local_rvs = (vs_socket_t*)vs->vs_data; 
		local_rvs; 
		local_rvs = local_rvs->rvs_next) {
		if (local_rvs->rvs_server_port == MACH_PORT_NULL)
			break;
	}

        /*
         *  Bind any local secondary socket last.
         */
        if (local_rvs) {
                SOCKET_UNLOCK(vs);
                DOMAIN_UNFUNNEL(*f);
                error = sobind(vs, nam);
                DOMAIN_FUNNEL(sodomain(vs), *f);
                SOCKET_LOCK(vs);

                VSDEBUG(VSDEBNODEBIND,
                        ("vs_netsobind: vs 0x%x rvs 0x%x "
                         "local node %d binding\n",
                         vs, local_rvs, this_node));
		if (error != ESUCCESS)
			return error;
                vs->vs_flags |= VS_ISBOUND;
                local_rvs->rvs_server_node = this_node;
                local_rvs->rvs_server_port = MACH_PORT_NULL;
        }

        return ESUCCESS;
}
