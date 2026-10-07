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
 * $Log: vs_ipc.c,v $
 * Revision 1.39  1995/03/28  19:28:45  nina
 *  Reviewer:hobbes
 *  Risk:Lo
 *  Benefit or PTS #:12794
 *  Testing:traceroute to various hosts
 *  Module(s):server/vsocket/vs_ipc.c
 *
 * Modify nsrv_if_assign_port_number() to use zero for the port number for
 * IPPROTO_ICMP and IPPROTO_RAW sockets.
 *
 * Revision 1.38  1995/03/03  19:08:23  toman
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
 * Revision 1.37  1995/02/11  00:04:56  stans
 *  'lint' picking with typedefs for a clean compile
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.36  1995/02/01  23:25:40  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.35  1994/12/23  01:29:44  nina
 *  Move the functions vs_bind_local() and vs_bind_remote()
 *  to vs_subr.c.
 *
 * Revision 1.34  1994/12/22  02:17:46  nina
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
 *   The was only seen in systems with multiple network configurations
 *   when a user attempted to bind to INADDR_ANY/port 0. In this
 *   situation, the system is supposed to pick the port number for
 *   the caller. The problem occurred because the VSOCKET code
 *   didn't ensure that the primary socket and each secondary socket used
 *   the same port number.
 *
 *   Now, a bind to port 0 results in an RPC to the clearinghouse to
 *   pick a port number, based on the domain and protocol of the socket.
 *
 * Revision 1.33  1994/11/18  20:52:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.32  1994/09/20  18:30:51  nina
 *  Reviewer:nina, hobbes
 *  Risk:Medium
 *  Benefit or PTS #:10818
 *  Testing:TCP/IP eats and PTS test case
 *  Module(s):
 * 	./server/vsocket/vs_ipc.c
 * 	./server/vsocket/vs_netops.c
 *
 *  Fix VSDEBUG code.
 *
 * Revision 1.31  1994/08/31  22:48:03  mtm
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
 * Revision 1.29.2.1  1994/08/10  18:16:19  yazz
 * PTS #: #9221
 * Mandatory: Technically NO, but Suri's scrub says it's about to become mandatory.
 * Description: Routine nsrv_r_vs_sorecv now deallocates VM it had leaked.
 *              (Read for 300Kb; 100Kb is in fact read; leftover 200Kb
 *               of buffer space is now deallocated instead of being leaked.)
 * Reviewer(s): John Litvin
 * Risk: Low.  Few lines in one routine in one source file.
 * Module(s): server/vsocket/vs_ipc.c
 * Testing: The "haupt.c" reproducible testcase, with the memsnoop program.
 *
 * Revision 1.29  1994/07/28  22:24:26  yazz
 *  Reviewer: Mike Leibensperger
 *  Risk: Lo
 *  Benefit or PTS #: 10421
 *  Testing: EATs tcp-ip
 *  Module(s): server/vsocket/vs.defs, .../vs_subr.c, .../vs_ipc.c,
 *             .../vsocket.h
 * Made all calls to start_vsockserver_op() check for error returns.  Removed
 * stale code.  Added some VSDEBUG statements.
 *
 * Revision 1.28  1994/07/22  01:51:09  yazz
 *  Reviewer: slk@locus.com
 *  Risk: lo
 *  Benefit or PTS #: #10347
 *  Testing: EATs tcp-ip
 *  Module(s): server/vsocket/vs_ipc.c
 *
 * When the start_vsockserver_op() routine returns a failure (rare), *never*
 * call end_vsockserver_op().
 *
 * Revision 1.27  1994/06/15  17:26:42  mjl
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
 * Revision 1.26  1994/05/23  14:19:06  cfj
 * In nsrv_vs_dequeue(), deallocate the callbackport in the error cases.  Plugs
 * a port leak.
 *
 *  Reviewer:mjl@locus.com
 *  Risk:L
 *  Benefit or PTS #:9449
 *  Testing:Tensor application, tcp-ip EAT
 *  Module(s):server/vsocket/vs_ipc.c
 *
 * Revision 1.25  1994/05/21  01:14:38  yazz
 *  Reviewer: Chris Peak, Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: fixes a memory leak.  *May* turn out to fix #9221 and #9497.
 *  Module(s): server/tnc/vs_ipc.c
 *
 * Fix a memory leak in nsrv_r_vs_sosend_long().  The SERVER_DEALLOC macro
 * will not release the OOL memory if an error occurs, and this memory must
 * be released in either case.
 *
 * Revision 1.24  1994/05/04  22:20:33  mjl
 * Merge revision 1.21.2.3 from R1_2 branch into main trunk.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.21.2.3  1994/04/27  23:06:54  yazz
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
 * TNC select rewrite.  Combine r_vs_select_dequeue() and
 * ..._getstate_immed() RPCs.  Support for select id scheme; elimination
 * of "surrogate drp" scheme.  New r_vs_scrub_remote_selects() RPC is
 * called from "bottom half" (e.g. select_wakeup() call from Netintr())
 * if the drp has gone dead to ensure a cleanup FOP_SELECT() call is
 * made.  Use new VS_MALLOC macros.  Also many small fixes.  (Note:
 * nsrv_r_vs_fp_unref_svr() is unused.)
 *
 * Revision 1.23  1994/04/16  21:16:38  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: #8968.  Random pages of memory won't be deallocated
 * 			   out from under the server by mistake anymore.
 *  Testing: EATs tcpip
 *  Module(s): server/vsocket/vs_ipc.c
 *
 * Initialize MIG OUT params for deallocated out-of-line memory to null values,
 * so that in case of error, random stack garbage does not specify a page of
 * memory to be deallocated by mistake.
 *
 * Revision 1.22  1994/03/13  17:09:47  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7294, 6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_ipc.c,
 * 	vs_subr.c, vs_netops.c, sys_vsocket.c
 *
 * Merge changes from R1.2
 *
 * Revision 1.21.2.1  1994/03/07  23:07:36  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7294/6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_ipc.c,
 * 	sys_vsocket.c, vs_netops.c, vs_subr.c
 *
 * Make sure that rvs selected in nsrv_r_m_vs_wakeup()
 * is marked VS_USE.
 *
 * Revision 1.21  1993/09/20  23:58:44  cfj
 * Merge R1.1 bug fixes into main stem.
 *
 * Revision 1.20  1993/09/14  15:17:00  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.19.2.2  1993/09/20  23:49:49  cfj
 * Bug fix for PTS #6663.  The correct netserver is now chosen.
 *
 * Revision 1.19.2.1  1993/09/14  15:15:37  cfj
 * Split the hold/release routines into network oriented and AF_UNIX
 * oriented (These should only be virtual socket operations.).  Only
 * increment the virtual socket reference count on the select enqueue if
 * it is the first time through. (again == FALSE)
 * Part of fix for PTS bug #6097.
 *
 * Revision 1.19  1993/09/01  01:41:11  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.18  1993/08/04  03:55:25  cfj
 * 08-03-93 Code drop from Locus.
 *
 * Revision 1.17  1993/07/30  15:40:53  cfj
 * Back out the modification that bhk made where the socket lock was
 * held across calls to r_vs_select_check() and r_vs_select_enqueue().
 *
 * Revision 1.16  1993/07/29  21:55:26  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network
 * server slowdown.
 *
 * Revision 1.15  1993/07/14  18:49:09  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  21:14:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.14  1993/05/20  16:04:36  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 3.44  93/08/26  11:04:47  mjl
 * [LCCbug #0374; #0376 no longer reproducible; maybe fixes #0372]
 * Replace r_vs_ioctl() RPC's fixed length INOUT array argument with
 * two variable length array arguments, one IN and one OUT.  Prevents
 * server addressing exceptions that occured when all N bytes of fixed
 * arg weren't allocated in server's address space.  Also, in
 * r_m_vs_wakeup(), don't assert that rvs must be found---if rvs can't
 * be found, socket must have been closed.
 * 
 * Revision 3.43  93/08/22  09:36:25  bhk
 * Fixed the crash when the asynchronous vs_select_dequeue would
 * arrive after the close [#361]
 * 
 * Revision 3.42  93/08/17  19:35:59  mjl
 * [Bugs #0349, #0350] Pass client_node argument to if_list() so clearinghouse
 * can determine if there is a client-local network server.  Return errno as
 * an out argument from r_vs_socreate().
 * 
 * Revision 3.41  93/08/16  18:58:04  bhk
 * Removed remote sleep routines. [# 346]
 * split remote getstate into synchronouse and asynchronous calls [#346]
 * Added creds port to close to avoid crash[ #347]
 * 
 * Revision 3.40  93/08/13  14:29:44  bhk
 * Fixed bogus assert
 * 
 * Revision 3.39  93/08/11  15:08:01  mjl
 * [Bug #0340] In nsrv_r_vs_soclose(), awaken any sleeping r_vs_soreadable()
 * threads before closing.  In nsrv_r_vs_soreadable(), take a vsocket ref
 * and release it when done to avoid lossage when racing against a close.
 * Use ANSI prototypes throughout.
 * 
 * Revision 3.38  93/08/11  09:08:05  bhk
 * Changed the way readability is determined by having the primary
 * virtual socket check for the ability of the secondary to perform read
 * requests by sending simple routines to the secondaries which
 * whill later send simple wakeup routines back
 * 
 * Revision 3.37  93/08/09  18:57:01  bhk
 * Passed the target port over as a port name in r_m_vs_sosleep to
 * allow the primary virtual socket to identify the secondary which
 * woke up.  Removed the parent port from r_m_vs_sosleep and used the
 * cached callback port.  fixes bug 323
 * 
 * Revision 3.36  93/08/09  15:53:32  bhk
 * added callback port to to remote socket creation
 * Fixed typo in remote sleep
 * 
 * Revision 3.35  93/08/06  17:57:59  bhk
 * Stopped virtual sockets with multiple active secondary sockets
 * from consuming the entire transport bandwith waiting for something to read
 * 
 * Revision 3.34  93/08/03  17:15:51  mjl
 * Fix some debug printfs.  Set VS_IS_SHADOW flag for remote secondary
 * sockets.  Make nsrv_r_m_vs_sosleep() return an error code.
 * 
 * Revision 3.33  93/08/01  22:48:58  bhk
 * Fixed close by geting a deadname notification on the secondaries
 * delay port.  This notification is ignored but it is necessary to
 * force the server reference count in the file table to be
 * decremented.
 * 
 * Revision 3.32  93/07/30  14:11:55  yazz
 * Restore SOCKET_LOCK and SOCKET_UNLOCK and remove a now meaningless
 * assertion in r_vs_select_enqueue().  (yazz for bhk)
 * 
 * Revision 3.31  93/07/29  11:37:39  mjl
 * [Bug #0326] Fix typo in nsrv_r_m_vs_wakeup() that caused infinite loop
 * in multiple net server case.  Also, when one secondary socket awakens
 * from a remote sleep, cancel the sleeps on the other secondaries.
 * 
 * Revision 3.30  93/07/28  15:04:06  bhk
 * closed a window where a socket could be checked for and event and an event
 * could occur before the select was enqueued thus losing the event.
 * 
 * Revision 3.29  93/05/13  11:41:20  mjl
 * Remove stale code.
 * 
 * Revision 3.28  93/05/10  14:43:50  bhk
 * Synchronized state after connection request, set synchronized state
 * 
 * Revision 3.27  93/05/07  15:30:51  nina
 * Remove outdated code.  Interfaces to chouse_find_nearest(),
 * and chouse_list() were changed. 
 * 
 * Revision 3.26  93/05/05  22:45:42  mjl
 * Add server stubs for new MIv3 clearinghouse RPCs.  Rename ...get_nearest...
 * for consistency.
 * 
 * Revision 3.25  93/05/04  16:47:09  bhk
 * Fixed RCS comment
 * 
 * Revision 3.24  93/05/03  14:49:00  bhk
 *  Workaround for delayed deadname notifications of canceled selects.
 * 
 * Revision 3.23  93/04/16  11:26:18  bhk
 * Added stronger checking to the select dequeue virtual socket port to
 * detect a deadname
 * 
 * Revision 3.22  93/04/12  15:59:28  nina
 * Added two RPCs to support single system semantics for SIOCGIFCONF,
 * SIOCADDRT and SIOCDELRT.
 * 
 * Revision 3.21  93/04/09  11:06:30  bolsen
 * removed extra m_freem() call in nsrv_r_vs_sorecv().
 * fixes panic: freeing free mbuf
 * 
 * Revision 3.20  93/04/03  12:05:49  klh
 * Split select into three parts (check, enqueue, dequeue).
 * Plug the mbuf leak (#193).
 * 
 * Revision 3.19  93/03/19  17:53:58  bhk
 * Fixed select. Major rewrite of select code not to use OSF select.
 * Fixed the Assertion SOCKET_ISLOCKED by allowing r_vs_getstate to 
 * either lock, unlock, lock and unlock or do nothing to the socket 
 * lock when it is called
 * 
 * Revision 3.18  93/03/03  17:20:03  mjl
 * Check error return from vsfindmatch() in nsrv_if_lookup() [LCC bug #0174].
 * 
 * Revision 3.17  93/02/19  15:39:58  bhk
 * Fixed return of nsrv_r_vs_sogetopt to call end_vsockserver_op
 * to cleanup correctly
 * 
 * Revision 3.16  93/02/10  21:10:34  klh
 * Additional checkin to sync up RCS comments with Configuration file
 * 
 * Revision 3.15  93/02/09  18:17:22  mjl
 * Move all clearinghouse management code to vs_chouse.c , and call as needed.
 * 
 * Revision 3.15  93/02/08  16:55:59  bhk
 * destroyed the file ports send right if the select operation was successful
 * to keep the send right count correct
 * 
 * Revision 3.14  93/01/26  10:51:21  klh
 * Closed memory leak for non-local clients. Closes bug 150
 * 
 * Revision 3.13  93/01/11  19:17:46  bhk
 * Added family to if_lookup to allow passing a NULL socket address.
 * 
 * Revision 3.12  93/01/08  15:39:48  mjl
 * Fix sense of test prior to call to sosetopt().
 * 
 * Revision 3.11  93/01/05  14:38:44  bhk
 * Rearrange code for ease of debugging.
 * 
 * Revision 3.10  93/01/04  20:43:12  bhk
 * Added transaction IDs to remote vsocket calls to allow signals
 * to be processed correctly.
 * Added checks for local endpoints in the clearing house.  This is
 * need for routing endpoints
 * 
 * Revision 3.9  92/10/29  19:33:35  bhk
 * Cleaned up and unified debug messages`
 * 
 * Revision 3.8  92/10/27  17:54:17  bhk
 * Fixed bug in socket creation for connects.
 * Added remote soreserve server code.
 * replaced usrreq with getaddr to harmonize with the new virtual socket
 * operations callout tabled.
 * 
 * Revision 3.7  92/10/07  16:20:06  bhk
 * Allow if_lookup to return MACH_PORT_NULL as the local netserver.
 * cleaned up code to reduce compiler warning messages
 * 
 * Revision 3.6  92/09/24  17:17:16  bhk
 * added not to interface registstation,
 * code cleanup,
 * stoped storing ioctl and setsockopts playback data on
 * secondary sockets,
 * allow if_lookup to return MACH_PORT_NULL as local host
 * and MACH_PORT_DEAD as not server not found
 * 
 * Revision 3.5  92/07/26  17:54:37  bhk
 * cleaned up debug
 * added support for getpeername
 * 
 * Revision 3.4  92/06/22  12:16:07  bhk
 * removed panics and returned ENOMEM
 * Fixed clearinghouse queueing.
 * 
 * Revision 3.3  92/06/16  18:47:18  bhk
 * fixed return values added remote sleep/wakeup and shutdown
 * 
 * Revision 3.2  92/05/06  16:14:45  bhk
 * destroyed port on close
 * send now returns amount of data written
 * added ipc's to support sleep virtual socket operation
 * 
 * Revision 3.1  92/04/22  17:30:23  bhk
 * cleanup printfs,  added support for getsockname.
 * 
 * Revision 3.0  92/04/20  17:22:28  bhk
 * Genesis  Internet Remote Virtual socket server side code
 * 
 * 
 */

#include "net/net_globals.h"
#include "sys/param.h"
#include "sys/errno.h"
#include "uxkern/import_mach.h"
#include "uxkern/syscall_subr.h" 

#include "../user/include/servers/netname_defs.h"
#include "sys/mbuf.h"
#include "sys/uio.h"
#include "sys/file.h"
#include "sys/ioctl.h"
#include "vsocket/vsocket.h"
#include "vsocket/vs_types.h"
#include "vsocket/vs_subr.h"
#include "sys/socket.h"
#include "sys/socketvar.h"
#include "sys/protosw.h"
#include "sys/domain.h"
#include "net/if.h"
#include "net/route.h"

#include "netinet/in.h"
#include "vsocket/vs_chouse.h"
#include "vsocket/vs_mig.h"
#include "uxkern/port_hash.h"

#define Register	register /* for easy toggling of register decls */

struct vsocket_ops	*vsfindvsops();
netserv_data_t		*netserv_list = 0;
struct mutex		netserv_lock = MUTEX_INITIALIZER;
chouse_t		netserv_chouse;

extern match_func_t	vsfindmatch();
extern node_t		this_node;

LOCK_ASSERTL_DECL

#define VSDEBTEMP 0x40000
/*
 *  All ioctls that modify interface addresses, netmasks, or broadcast
 *  addresses call this to inform the clearinghouse of the change.
 */
kern_return_t
nsrv_if_addr_update(
	mach_port_t		server_port,
	chouse_key_t		*ckp,
	int			iface_flags,		    
	int			cmd,
	ifreq_t			*ifr,
	mach_port_t		netserv_port,
	iface_info_t		*iip,
	int			*rval)
{
	*rval = chouse_addr_update(&netserv_chouse,
				   ckp,
				   iface_flags,
				   cmd,
				   ifr,
				   netserv_port,
				   iip);
	return (KERN_SUCCESS);
}


kern_return_t
nsrv_if_confirm_update(
	mach_port_t	server_port,
	chouse_key_t	*ckp,			/* chouse key of updated i/f */
	int		cmd,			/* ioctl causing update */
	ifreq_t		*ifr,			/* ioctl data */
	int		*rval)
{
#include "mi.h"
#if	NMI > 0
	extern int	mi_autoconfig();

	*rval = mi_autoconfig(&netserv_chouse, ckp, cmd, ifr);
#else
	*rval = ESUCCESS;
#endif
	return (KERN_SUCCESS);
}


kern_return_t
nsrv_if_addr_reset(
	mach_port_t	server_port,
	chouse_key_t	*ckp,		/* key of i/f entry to reset */
	iface_info_t	*iip,		/* previous state to restore */
	int		*rval)
{
	VSDEBUG(VSDEBENTRY|VSDEBCLRHOUSE,
		("nsrv_if_addr_reset: ifp 0x%x on node %d being reset\n",
		 ckp->ck_id, ckp->ck_node));

	*rval = chouse_addr_reset(&netserv_chouse, ckp, iip);

	return (KERN_SUCCESS);
}


/*
 * nsrv_if_check_out - remove a server/address pair from the clearinghouse 
 *
 * unimplemented
 */
kern_return_t
nsrv_if_check_out(server_port,family,address,addrlen,rval)
	mach_port_t	server_port;
	int		family;
	struct sockaddr	*address;
	int		addrlen;
	int		*rval;
{
	VSDEBUG((VSDEBENTRY|VSDEBCLRHOUSE),
		("if_check_out: family %d address %d.%d.%d.%d\n",
			family,address->sa_data[0],
			address->sa_data[1],address->sa_data[2],
			address->sa_data[3]));
#ifdef	NOTDEF
	*rval = chouse_check_out(&netserv_chouse,
				 family,
				 address);
#endif

	*rval = EIO;
	return KERN_SUCCESS;
}


/*
 * Name:
 *	nsrv_if_list()
 *
 * Function:
 *	return a list of network server node numbers &
 *	a list of corresponding mach_ports
 *
 * Inputs:
 *	name	-	name of clearinghouse that we're interested in
 *	nel	-	number of elements in nodes & ports array(s)
 *	client_node -	node requesting this information
 *	
 * Outputs:
 *	numns	-	number of network server nodes configured
 *	rel	-	number of output elements used
 *	nodes	-	filled in with network server node numbers.
 *	node_num -	required by MIG.
 *	ports	-	filled in with control ports of network servers
 *	port_num -	required by MIG
 *	rval	-	return code
 *
 * Return codes:
 *	Success or MIG errors
 */	
kern_return_t
nsrv_if_list(
	mach_port_t	server_port,
	node_t		client_node,
	int		nel,
	int		*numns,
	int		*rel,
	node_t		nodes[],
	int		*node_num,
	port_list_t	ports,
	int		*port_num,
	kern_return_t	*rval)
{
	/* We have a clearinghouse. Return desired info. */
	*rval = chouse_list(&netserv_chouse,
			    client_node, nel, numns, rel, nodes, ports);

	if (*rval == ESUCCESS) {
		*node_num = *rel;
		*port_num = *rel;
	} else {
		*node_num = 0;
		*port_num = 0;
	}

	return(KERN_SUCCESS);		     
}


/* 
 * nsrv_if_lookup - find a network server given an address
 *
 * This routine loops through the list of servers checking for an address
 * match.  It returns an array of network server ports.
 *
 * Here we pass "family" seperately from the sockaddr, since it's
 * possible to bind to a NULL address (and have the system pick an
 * address for you), and we need the family in this case too.
 */ 
kern_return_t
nsrv_if_lookup(
	mach_port_t	server_port,
	node_t		node,
	int		family,
	struct sockaddr	*address,
	int		addrlen,
	server_list_t	netserv_port,		/* out */
	int		*netserv_port_num,	/* out */
	kern_return_t	*rval)			/* out */
{
	match_func_t	match;

	if ( (match = vsfindmatch(family)) == NULL ) {
		*rval = EPFNOSUPPORT;
		return (KERN_SUCCESS);
	}

#ifdef	OBSOLETE
	if (addrlen != 0 && (*match)(address, NULL) ) {
		/*
		 *  Matching an address against NULL means checking
		 *  to see if it is one of the "special" addresses
		 *  that should be forced to a local socket rather
		 *  than be shadowed by a secondary socket on a
		 *  remote network server.  Pass back MACH_PORT_NULL
		 *  to indicate this.
		 */
		netserv_port[0] = MACH_PORT_NULL;
		*netserv_port_num = 1;
		*rval = ESUCCESS;
		return (KERN_SUCCESS);
	}
#endif	/* OBSOLETE */

	*rval = chouse_lookup(&netserv_chouse, node, family, address, addrlen,
			      netserv_port, netserv_port_num);
	return (KERN_SUCCESS);
}


/*
 * nsrv_if_get_nearest_server - get the "nearest" network server
 */
kern_return_t
nsrv_if_get_nearest_server(
	mach_port_t	server_port,
	int		family,
	node_t		node,
	node_t		*host_node,
	mach_port_t	*host_port)
{
	VSDEBUG(VSDEBENTRY, ("nsrv_if_get_nearest_server: fam %d node %d\n",
			     family, node));
	*host_port = chouse_find_nearest(&netserv_chouse,
					 family, node, host_node);
	return (KERN_SUCCESS);
}


/*
 *  nsrv_if_get_server_by_route - get the correct network server
 *  to use for sending packets to the target address
 */
kern_return_t
nsrv_if_get_server_by_route(
	mach_port_t	server_port,
	node_t		pvs_node,	/* primary virtual socket's node */
	struct sockaddr	*sa,		/* the target socket address */
	int		salen,		/* length of target sockaddr */
	node_t		*ret_node,	/* OUT - node of netserver */
	mach_port_t	*ret_port)	/* OUT - port of netserver */
{
	ASSERT(salen == sa->sa_len && salen != 0);
	chouse_get_server_by_route(&netserv_chouse,
				   pvs_node,
				   sa,
				   ret_node,
				   ret_port);
	return (KERN_SUCCESS);
}


/*
 * Remote socket creation routine
 *
 * Create a socket then allocate a port and place it in the
 * server loop port set
 */
kern_return_t
nsrv_r_vs_socreate(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	mach_port_t	callback_port,
	transaction_id_t transid,
	int		domain,
	mach_port_t	*socket_port,
	int		so_type,
	int		protocol,
	struct socket	**socket_addr,
	int		*rval)
{
	struct socket	*so;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("r_vs_socreate 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_socreate,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}
	
	*rval = SOCREATE(domain, &so, so_type, protocol);
	if (*rval == ESUCCESS) {
		VSOCK_TO_PORT_LOOKUP(so, *socket_port);
		*socket_addr = so;
		so->vs_flags |= (VS_ISBOUND|VS_IS_SHADOW);
		so->vs_callback = callback_port;
	} else {
		mach_port_deallocate(mach_task_self(), callback_port);
		*socket_port = MACH_PORT_NULL;
		*socket_addr = NULL;
	}

	end_vsockserver_op(cred_port, *rval, 0);
	return KERN_SUCCESS;
}


/*
 *  Close a remote secondary socket.
 */
kern_return_t
nsrv_r_vs_soclose(
	mach_port_t		server_port,
	mach_port_t		cred_port,
	transaction_id_t	transid,
	kern_return_t		*error)
{
#if	MACH_ASSERT
#define	ASSERTFUL(s)	s
#else
#define ASSERTFUL(s)
#endif
	struct socket		*so;
	kern_return_t		kr;
	ASSERTFUL(int		refs = 1;)
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("r_vs_soclose 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port, cred_port, transid,
					SYS_VSOCK_soclose, 0);
	if (kr != KERN_SUCCESS) {
		VSDEBUG(VSDEBERROR,
		    ("nsrv_r_vs_close: start_vsockserver_op: kr 0x%x\n", kr));
		return(kr);
	}

	PORT_TO_VSOCK_LOOKUP(server_port, so);
	DOMAIN_FUNNEL(sodomain(so), f);

	/* Sets VS_CLOSING flag; may sleep. */
	MARK_VSOCK_CLOSING(so);

	SOCKET_LOCK(so);

	/*
	 *  Destroy primary socket callback port and accept no
	 *  further messages from the primary.  The server_port
	 *  itself will be deallocated by the VSOP_CLOSE() call
	 *  below.
	 */
	kr = mach_port_deallocate(mach_task_self(), so->vs_callback);
	if (kr != KERN_SUCCESS)
		panic("nsrv_r_vs_soclose: m_p_deallocate: "
		      "so 0x%x, port 0x%x, kr 0x%x\n",
		      so, so->vs_callback, kr);

	/*
	 *  Wake any outstanding r_vs_soreadable() readability checks
	 *  and r_vs_getstate_delay() state checks.
	 */
	if (so->vs_sleep_cmd & VS_SLEEP_RECV) {
		sowakeup(so,&so->so_rcv);
		ASSERTFUL(refs++);
	}
	if (so->vs_sleep_cmd & VS_SLEEP_TIMEO) {
		wakeup((caddr_t)&so->so_timeo);
		ASSERTFUL(refs++);
	}

	/*
	 *  Should have one ref each for:
	 *  - existence
	 *  - readability thread if any
	 *  - sleep thread if any
	 */
	ASSERT(refs == so->vs_refcnt);

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);

	*error = VSOP_CLOSE(so);

	return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
#undef	ASSERTFUL
}


kern_return_t
nsrv_r_vs_soconnect(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t transid,
	sockaddr_t	address,
	int		addrlen,
	int		*state,
	int		*rval)
{
	struct socket	*so;
	struct mbuf	*nam;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_soconnect 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_soconnect,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	*rval = ESUCCESS;
	PORT_TO_VSOCK_LOOKUP(server_port,so);
	if(!(*rval = sockargs(&nam, (caddr_t)address, address->sa_len, 
		MT_SONAME)))  {
		*rval = soconnect(so,nam);
		m_freem(nam);
	}
	*state = so->so_state;
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


nsrv_r_vs_sosetopt(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t transid,
	int		level,
	int		name,
	caddr_t		value,
	int		value_len,
	kern_return_t	*rval)
{
	struct socket	*so;
	struct mbuf	*m = NULL;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_sosetopt 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sosetopt,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	*rval = ESUCCESS;

	PORT_TO_VSOCK_LOOKUP(server_port,so);
	if(value_len) {
		if((m = m_get(M_WAIT, MT_SOOPTS)) == NULL) 
			*rval = ENOBUFS;
		else {
			bcopy(value,mtod(m,caddr_t),value_len);
			m->m_len = value_len;
		}
	}

	if(*rval == ESUCCESS)
		*rval = sosetopt(so,level,name,m);

	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


nsrv_r_vs_sogetopt(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t transid,
	int		level,
	int		name,
	int		*type,
	caddr_t		addr,
	int		*addr_len,
	kern_return_t	*rval)
{
	struct socket	*so;
	struct mbuf	*m = NULL;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_sogetopt 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sogetopt,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	*rval = ESUCCESS;

	PORT_TO_VSOCK_LOOKUP(server_port,so);
	*rval = sogetopt(so,level,name,&m);

	if(m) {
		*type = m->m_type;
		bcopy(mtod(m,caddr_t),addr,m->m_len);
		*addr_len = m->m_len;
		m_free(m);
	} else {
		*type = MT_SOOPTS;
		*addr_len = 0;
	}
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


#include "sys/poll.h"

nsrv_r_vs_select_check(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t	transid,
	short		events,
	short		*revents,
	int		*state,
	int		*soerror,
	int		*qlen,
	int		*isreadable)
{

	struct file	fp;
	struct socket	*so;
	kern_return_t	error;
	struct uthread	*uth = &u;
	kern_return_t	kr;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBSELECT|VSDEBENTRY,
			("nsrv_r_vs_select_check 0x%x ", server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sel_check,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}
	
	PORT_TO_VSOCK_LOOKUP(server_port,so);

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	vs_select_check(so,events,revents);
	*state = so->so_state;
	*soerror = so->so_error;
	*qlen = so->so_qlen;
	*isreadable = soreadable(so);
	VSDEBUG(VSDEBSELECT,
		("revents 0x%x qlen 0x%x state 0x%x, soerror 0x%x\n",
		*revents, *qlen, *state, *soerror));
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


kern_return_t
nsrv_r_vs_select_enqueue(
	mach_port_t		server_port,
	mach_port_t		cred_port,
	mach_port_t		file_port,
	sel_id_t		sel_id,
	transaction_id_t	transid,
	int			index,
	int			events,
	int			again,
	mach_port_t		*ret_file_port)
{
	struct uthread		*uth = current_thread();
	struct socket		*so;
	kern_return_t		kr;
	short			revents;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBSELECT|VSDEBENTRY,
			("nsrv_r_vs_select_enqueue 0x%x ",server_port));

	kr = start_vsockserver_op(server_port, cred_port, transid,
					SYS_VSOCK_sel_enqueue, 0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	ASSERT(sel_id != SEL_ID_NULL);

	PORT_TO_VSOCK_LOOKUP(server_port,so);
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	/*
	 *  If this is the first time through, we need to take a ref
	 *  on this secondary socket to prevent its closing out from
	 *  under the select(2).  XXX/zzz Is this needed now that a
	 *  svrref is held in the selid/drp map entry?
	 */
	if (again == FALSE) {
		INCREMENT_VSNET_REFCNT(so,"nsrv_r_vs_select_enqueue");
	}

	/*
	 *  Set up the user area to make select_enqueue() happy.
	 *  Since this is a remote secondary socket, there is no
	 *  delayed reply port, so don't set SQ_DRP and put the
	 *  (guaranteed non-zero) select id in uu_sel_delay_port.
	 */
	uth->uu_sel_file_port = file_port;
	uth->uu_sel_delay_port = sel_id;
	uth->uu_sel_index = index;
	uth->uu_sel_events = events;
	uth->uu_sel_again = again;
	uth->uu_sel_id = sel_id;
	uth->uu_sel_flags = SQ_VSOCK;
	
	select_enqueue(&so->so_rcv.sb_selq);

	/*
	 *  If select_enqueue() didn't clear uu_sel_delay_port then
	 *  the enqueue attempt failed, so we must return the file
	 *  port send right.  Back on the primary, it and the
	 *  corresponding svrref will be released.
	 */
	if (uth->uu_sel_delay_port != MACH_PORT_NULL) {
		*ret_file_port = file_port;
		goto done;
	}
	*ret_file_port = MACH_PORT_NULL;

	/*
	 *  If the event being selected on has already occurred,
	 *  wake up right away.
	 */
	revents = 0;
	vs_select_check(so, events, &revents);
	if (revents) {
		VSDEBUG(VSDEBSELECT,("nsrv_r_vs_sel_enq: immed wakeup, "
				     "svs 0x%x, selid 0x%x, revents 0x%x\n",
				     so, sel_id, revents));
		select_wakeup(&so->so_rcv.sb_selq);
	}

done:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


/*
 *  Perform a select dequeue on a remote secondary socket, at the
 *  same time returning the secondary's state information.
 *
 *  Notes:
 *    - The select_dequeue() expects uu_sel_delay_port to be non-zero.
 *	Since we don't have a delay port on this remote secondary, we
 *	use the (guaranteed non-zero) select id instead.
 */
kern_return_t
nsrv_r_vs_select_dequeue_getstate(
	mach_port_t		server_port,
	mach_port_t		cred_port,
	sel_id_t		sel_id,
	int			sel_flags,
	int			index,
	int			*state,
	int			*soerror,
	int			*qlen,
	int			*isreadable)
{
	struct uthread		*uth = current_thread();
	struct socket		*so;
	kern_return_t		kr;
	mach_port_type_t	type;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBSELECT|VSDEBENTRY,
		("nsrv_vs_select_dequeue: server port 0x%x selid 0x%x",
		 server_port, sel_id));

	kr = start_vsockserver_op(server_port, cred_port, 0/*transid*/,
					SYS_VSOCK_sel_dequeue, 0);
	if (kr != KERN_SUCCESS) {
		VSDEBUG(VSDEBERROR,
		    ("nsrv_vs_select_dequeue: start_vsockserver_op: kr 0x%x\n",
		     kr));
		return(kr);
	}

	PORT_TO_VSOCK_LOOKUP(server_port, so);

	ASSERT(sel_id != SEL_ID_NULL);

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

#if	MACH_ASSERT
	/*
	 *  If this select id is in the selid/svs map, it better map to
	 *  the proper secondary socket!
	 */
	{ struct socket *dummy;
	  VS_SECNDRY_READ_LOCK;
	  SELID_TO_SVS_LOOKUP(sel_id, dummy);
	  if (dummy && (dummy != so)) {
		  panic("nsrv_r_vs_select_dequeue: selid reuse badness, "
			"selid/svs 0x%x/0x%x, enqueue id on so=0x%x\n",
			sel_id, dummy, so);
	  }
	  VS_SECNDRY_UNLOCK;
	}
#endif

	uth->uu_sel_index = index;
	uth->uu_sel_id = sel_id;
	uth->uu_sel_delay_port = sel_id;
	uth->uu_sel_flags = sel_flags &~ SQ_DRP;
	select_dequeue(&so->so_rcv.sb_selq);

	/* Return the secondary socket's state information. */
	*state = so->so_state;
	*soerror = so->so_error;
	*qlen = so->so_qlen;
	*isreadable = soreadable(so);

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);

	/* The corresponding increment is in nsrv_r_vs_select_enqueue() above */
	DECREMENT_VSNET_REFCNT(so, "nsrv_r_vs_select_dequeue");

	return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
}


/*
 *  This RPC simpleroutine is called when select_wakeup() discovers
 *  that the drp has gone dead (in select_cleanup(), which see).  In
 *  this case, no cleanup call to FOP_SELECT() is made because the
 *  base code thinks it is about to do all the cleanup necessary right
 *  there in select_cleanup().  In TNC, however, there are possibly
 *  many remote secondary sockets holding file ports etc. that need to
 *  be cleaned up in this case.
 *
 *  This routine bears a certain similarity to select_deadname().
 *
 *  XXX NB that when we get here the drp has already been deallocated
 *  in select_cleanup(), and its name is available for re-use by Mach.
 *  This creates a window during which the name is still in the
 *  selid/drp map, and SELID_DRP_INSERT() may panic if it is reused.
 *  However, this window exists anyway whenever vsoo_select()/cleanup
 *  is called and the drp has gone dead.  The real solution is to
 *  associate the drp with the selid/drp map entry itself, and have it
 *  be deallocated only when the map entry is scrubbed.  In the meantime,
 *  rather than implement a partial fix for a problem that might not
 *  arise in practice, I leave this mungo comment here as a guide.
 *  (A partial fix would consist of deferring the drp deallocation,
 *  and doing it when SDM_DEAD is detected in the vsoo_select() cleanup,
 *  rather than when SDM_DEAD is set in select_cleanup().)
 */
/*simpleroutine*/
kern_return_t
nsrv_r_vs_scrub_remote_selects(
	mach_port_t	socket_port,
	mach_port_t	cred_port,
	mach_port_t	dead_delay_port_name)
{
	selid_drp_map_t	*sdm;
	kern_return_t	kr;
	struct uthread	*uth = current_thread();
	short		dummy;
	short		events
			    = POLLIN|POLLOUT|POLLERR|POLLHUP|POLLNVAL|POLLPRI;

	VSDEBUG(VSDEBENTRY,
		("nsrv_vs_scrub_remote_selects: args 0x%x 0x%x 0x%x\n",
		socket_port, cred_port, dead_delay_port_name));
	kr = start_vsockserver_op(socket_port, cred_port, 0/*transid*/,
					SYS_VSOCK_scrub_rem_sel, 0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	/*
	 *  Get a ref on the selid/drp map entry for the select we are
	 *  cleaning up.
	 *
	 *  If we can't find it, the select_deadname() thread managed
	 *  to map the drp name to a file port before SEL_PORT_REMOVE
	 *  was called.  We lost a race with select_deadname(), which
	 *  has already called FOP_SELECT() to clean up.
	 */
	DRP_SELID_LOOKUP(dead_delay_port_name, sdm);
	if (sdm == NULL) {
		return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
	}

	ASSERT(sdm->sdm_flags & SDM_DEAD);
	ASSERT(sdm->sdm_so == (struct socket *)socket_port);
	ASSERT(sdm->sdm_fp->f_data == (caddr_t)sdm->sdm_so);

	/*
	 *  Make a FOP_SELECT() cleanup call to clean up remote secondaries.
	 */
	uth->uu_sel_id = sdm->sdm_selid;
	uth->uu_sel_flags = (SQ_VSOCK|SQ_DEAD|SQ_DRP);
	uth->uu_sel_delay_port = sdm->sdm_drp;
	uth->uu_sel_file_port = (mach_port_t) sdm->sdm_fp;
	uth->uu_sel_index = 0;
	(void) vsoo_select(sdm->sdm_fp, &events, &dummy, 0);

	/*
	 *  This should get rid of the selid/drp map entry, which was
	 *  scrubbed by FOP_SELECT/cleanup.
	 */
	SELID_DRP_LOOKUP_DONE(sdm);

	return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
}


kern_return_t
nsrv_r_vs_sobind(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t transid,
	sockaddr_t	address,
	int		addrlen,
	int		*rval)
{

	struct socket *so;
	struct mbuf	*nam;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_sobind 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sobind,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}
	
	PORT_TO_VSOCK_LOOKUP(server_port,so);
	*rval = ESUCCESS;
	if(addrlen)
		*rval = sockargs(&nam, (caddr_t)address, address->sa_len,
			MT_SONAME);
	else
		nam = (struct mbuf *)0;
	if(!*rval)
		*rval = sobind(so,nam);
	if(nam)
		m_freem(nam);
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


nsrv_r_vs_sosend_short(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t transid,
	int		vssend_flags,
	sockaddr_t	address,
	int		addrlen,
	int		flags,
	small_char_array data,
	int		data_len,
	int		*sent,
	int		*rval)
{


	struct socket	*so;
	struct	iovec	aiov;
	struct	uio	auio;
	struct mbuf	*nam = (struct mbuf *)0;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_send_short 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sosend_short,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	*rval = ESUCCESS;
	PORT_TO_VSOCK_LOOKUP(server_port,so);

	aiov.iov_base = (caddr_t)data;
	aiov.iov_len = data_len;

	auio.uio_iov = &aiov;

	auio.uio_iovcnt = 1;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_WRITE;
	auio.uio_offset = 0;			
	auio.uio_resid = data_len;
	if(vssend_flags & VS_SEND_ADDR)
		*rval = sockargs(&nam, (caddr_t)address, address->sa_len,
			MT_SONAME) ;
	if(!*rval) {
		*rval = sosend(so,nam,&auio,(struct mbuf *)0,
			(struct mbuf *)0,flags);
	}
	*sent = auio.uio_offset;
	if(nam)
		m_freem(nam);
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


nsrv_r_vs_sosend_long(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t	transid,
	int		vssend_flags,
	sockaddr_t	address,
	int		addrlen,
	int		flags,
	char_array	data,
	int		data_len,
	int		*sent,
	int		*rval)
{
	struct socket	*so;
	struct	iovec	aiov;
	struct	uio	auio;
	struct mbuf	*nam = (struct mbuf *)0;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_send_long 0x%x\n", server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sosend_long,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	*rval = ESUCCESS;
	PORT_TO_VSOCK_LOOKUP(server_port,so);

	aiov.iov_base = (caddr_t)data;
	aiov.iov_len = data_len;

	auio.uio_iov = &aiov;

	auio.uio_iovcnt = 1;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_WRITE;
	auio.uio_offset = 0;			
	auio.uio_resid = data_len;
	if(vssend_flags & VS_SEND_ADDR)
		*rval = sockargs(&nam, (caddr_t)address, address->sa_len,
			MT_SONAME) ;
	if(!*rval) {
		*rval = sosend(so,nam,&auio,(struct mbuf *)0,
			(struct mbuf *)0,flags);
	}
	*sent = auio.uio_offset;

	/* always dealloc OOL memory whether send worked or not */
	kr = vm_deallocate(mach_task_self(),
				(vm_address_t)data,
				(vm_size_t)data_len);
	if (kr != KERN_SUCCESS) {
		panic("nsrv_r_vs_sosend_long: OOL mem dealloc kr=0x%x\n", kr);
	}
	if(nam)
		m_freem(nam);
	
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


int
nsrv_r_vs_sorecv(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t	transid,
	int		len,
	sockaddr_t	address,
	int		*addrlenp,
	int		*flagsp,
	caddr_t		*datap,		/* deallocated, out-of-line memory */
	int 		*data_lenp,
	int		*rvalp)
{
	struct socket	*so;
	struct	iovec	aiov;
	struct	uio	auio;
	struct mbuf	*nam;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_sorecv 0x%x\n", server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sorecv,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	/*
	 * Initialize values for OUT parameters in case of error.  (The
	 * pointer to out-of-line memory is irrelevant if its len is 0.)
	 */
	*datap = 0;
	*data_lenp = 0;
	*addrlenp = 0;

	PORT_TO_VSOCK_LOOKUP(server_port,so);

	if (vm_allocate(mach_task_self(),(vm_address_t *)datap,
			len, TRUE) != KERN_SUCCESS) {
		*rvalp = ENOBUFS;
		goto out;
	}

	aiov.iov_base = (caddr_t)*datap;
	aiov.iov_len = len;

	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_offset = 0;			
	auio.uio_resid = len;

	*rvalp = soreceive(so, &nam, &auio, (struct mbuf **)0,
			  (struct mbuf **)0, flagsp);
	if (*rvalp == ESUCCESS) {
		*data_lenp = auio.uio_offset;
		if (nam) {
			*addrlenp = nam->m_len;
			bcopy(mtod(nam,caddr_t), address, *addrlenp);
		}
	}
	if (nam)
		m_freem(nam);

out:
	if (*datap) {		/* we did vm_alloc some mem above */
		if (*rvalp) {	/* the call failed, so dealloc *all* of it */
			kr = vm_deallocate(mach_task_self(), 
					     (vm_address_t) *datap, len);
			ASSERT(kr == KERN_SUCCESS);
			*datap = 0;
			*data_lenp = 0;	/* say nothing to pass back & dealloc */
		} else {	/* call was OK -- dealloc any excess *only* */
			len -= round_page(*data_lenp);
			if (len > 0) {	/* req len was bigger than recv'd len */
				vm_address_t	start;
				start = (vm_address_t) 
					(*datap + round_page(*data_lenp));
				(void) vm_deallocate(mach_task_self(), 
						     start, len);
			}
		}
	}		
	return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
}


kern_return_t
nsrv_r_vs_solisten(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t	transid,
	int		backlog,
	boolean_t	*accept_ok,
	int		*rval)
{
	struct socket 	*so;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_solisten 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_solisten,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	PORT_TO_VSOCK_LOOKUP(server_port,so);
	*rval = solisten(so,backlog);
	*accept_ok = (so->so_options & SO_ACCEPTCONN)?TRUE:FALSE;
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


kern_return_t
nsrv_r_vs_sodequeue(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	mach_port_t	callback_port,
	mach_port_t	*socket_port,
	transaction_id_t transid,
	sockaddr_t	address,
	int		*addrlen,
	int		compat_43,
	int		*rval)
{
	struct socket	*head;
	struct socket	*so;
	struct mbuf	*nam = 0;
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_dequeue 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sodequeue,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	PORT_TO_VSOCK_LOOKUP(server_port, head);
	
	*rval = sodequeue(head,&so,&nam,compat_43);
	if (*rval != ESUCCESS) {
		VSDEBUG(VSDEBMISC,
			("nsrv_r_vs_sodequeue: sodeque(0x%x, ...) errno %d\n",
			 head, *rval));
		goto bad_dequeue;
	}
	so->vs_ops = vsfindvsops(so);
	if ((*rval = VSOP_CREATE(so)) != ESUCCESS) {
		VSDEBUG(VSDEBMISC,
			("nsrv_r_vs_sodequeue: VSOP_CREATE(0x%x): errno %d\n",
			 so, *rval));
		soclose(so);
		goto bad_dequeue;
	}
	so->vs_callback = callback_port;
	so->vs_flags |= (VS_ISBOUND|VS_IS_SHADOW);

	VSOCK_TO_PORT_LOOKUP(so,*socket_port);
	*addrlen = nam->m_len;
	bcopy(mtod(nam,caddr_t),address,*addrlen);
	if(nam)
		m_freem(nam);
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);

bad_dequeue:
	/*
	 *  We lost somehow, so don't return any OUT parameters.
	 */
	*socket_port = MACH_PORT_NULL;
	*addrlen = 0;
	if(nam)
		m_freem(nam);
	/*
	 *  Deallocate the callback port send right.
	 */
	(void) mach_port_deallocate(mach_task_self(), callback_port);

	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


/*ARGSUSED*/
kern_return_t
nsrv_r_vs_ioctl(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t transid,
	int		cmd,
	char 		*in_data,
	int		in_len,
	char		*out_data,
	int		*out_len,
	int		*rval)
{
	struct socket	*svs;	/* secondary virtual socket */
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY, ("nsrv_r_vs_ioctl 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port, cred_port, transid,
					SYS_VSOCK_ioctl, 0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	PORT_TO_VSOCK_LOOKUP(server_port, svs);

	/* If ioctl has IN parm, it is IOCPARM_LEN(cmd) bytes long. */
	ASSERT(((cmd & IOC_IN) && IOCPARM_LEN(cmd) != in_len) == 0);

	*rval = vslo_ioctl(svs, cmd, in_data, FALSE);
	*out_len = 0;
	if (*rval == ESUCCESS && ((cmd & IOC_OUT) != 0) ) {
		*out_len = IOCPARM_LEN(cmd);
		bcopy(in_data, out_data, *out_len);
	}

	return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
}


kern_return_t
nsrv_r_vs_global_ioctl(
	mach_port_t		server_port,
	mach_port_t		cred_port,
	transaction_id_t	transid,
	int			cmd,
	int			len,
	caddr_t			*data,	/* deallocated, out-of-line memory */
	int			*datalen,
	int			*rval)
{
	struct socket		*so;
	struct ifconf		*ifc;
	kern_return_t		kr;

	VSDEBUG(VSDEBENTRY, ("nsrv_r_vs_global_ioctl 0x%x\n", server_port));

	kr = start_vsockserver_op(server_port, cred_port, transid,
					SYS_VSOCK_global_ioctl, 0);
	if (kr != KERN_SUCCESS)
		return(kr);

	/*
	 * Initialize values for OUT parameters in case of error.  (The
	 * pointer to out-of-line memory is irrelevant if its len is 0.)
	 */
	*datalen = 0;

	PORT_TO_VSOCK_LOOKUP(server_port, so);

	VS_MALLOC(ifc, struct ifconf *, sizeof(struct ifconf), VSM_IFCONF);
	if (ifc == NULL) {
		*rval = ENOMEM;
		return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
	}

	kr = vm_allocate(mach_task_self(), (vm_address_t *)data, len, TRUE);
	if (kr != KERN_SUCCESS) {
		VSDEBUG(VSDEBERROR,
			("nsrv_r_vs_global_ioctl: vm_alloc: len %d, kr 0x%x\n",
			 len, kr));
		VS_FREE(ifc, VSM_IFCONF);
		*rval = ENOBUFS;
		return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
	}

	ifc->ifc_len = len;
	ifc->ifc_buf = (caddr_t)*data;

	*rval = vslo_ioctl(so, cmd, (caddr_t)ifc, FALSE);
	if (*rval == ESUCCESS)
		*datalen = ifc->ifc_len;

	VS_FREE(ifc, VSM_IFCONF);
	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}


kern_return_t
nsrv_r_vs_sogetaddr(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	transaction_id_t transid,
	int		which,
	int		compat_43,
	sockaddr_t	address,
	int		*addrlen,
	kern_return_t	*rval)
{
	struct socket *so;
	struct mbuf *m;
	kern_return_t	kr;
	DOMAIN_FUNNEL_DECL(f);

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_sogetaddr 0x%x\n",server_port));

	kr = start_vsockserver_op(server_port,cred_port,transid,
					SYS_VSOCK_sogetaddr,0);
	if (kr != KERN_SUCCESS) {
		return(kr);
	}

	*rval = KERN_SUCCESS;
	PORT_TO_VSOCK_LOOKUP(server_port,so);

	if(!(*rval = sogetaddr(so, &m, which, compat_43))) {
		*addrlen = m->m_len;
		bcopy(mtod(m,caddr_t),address,*addrlen);
	} else
		*addrlen = 0;	
	if(m)
		m_free(m);

	return end_vsockserver_op(cred_port,KERN_SUCCESS,0);
}
			
/*
 * Wakeup is called from a remote seconcdary virtual socket to the
 * call back port of the primary virtual socket in response
 * to a status change on that secondary.  Both vs_getstate_delay()
 * and vs_soreadable can call this routine.
 */
/*simpleroutine*/
kern_return_t
nsrv_r_m_vs_wakeup(
	mach_port_t	server_port,
	mach_port_t	target_port,
	int		index,
	int		cmd,
	int		state,
	int		soerror,
	int		qlen,
	int		isreadable)
{
	struct socket	*so;
	vs_socket_t	*rvs;
	int		tmpindex;
	DOMAIN_FUNNEL_DECL(f)

	/* zzz Why don't we need {start,end}_vsockserver_op() calls here? */

	VSDEBUG(VSDEBENTRY,("nsrv_r_m_vs_wakeup 0x%x index %d\n",
		server_port,index));

	if(!(MACH_PORT_VALID(server_port))) 
		return KERN_SUCCESS;
	PORT_TO_VSOCK_LOOKUP(server_port,so);

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	for ( rvs = (vs_socket_t *)so->vs_data; rvs ; rvs = rvs->rvs_next ) {
		if (!(rvs->rvs_flags & VS_USE))
			continue;
		if (target_port == rvs->rvs_server_port)
			break;
	}
	if (rvs == NULL) {
		/* Looks like we lost a race against vs_netsoclose(). */
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		return KERN_SUCCESS;
	}

	switch(cmd) {
	case  VS_SLEEP_TIMEO:
		tmpindex = rvs->rvs_state_index;
		break;
	case VS_SLEEP_RECV:
		tmpindex  = rvs->rvs_read_index;
		break;
	default:
		panic("nsrv_r_m_vs_wakeup: Unknown Wakeup call\n");
		break;
	}
	if (index == tmpindex) {

		rvs->rvs_qlen = qlen;
		rvs->rvs_state = state;
		rvs->rvs_soerror = soerror;
		if(isreadable)
			rvs->rvs_flags |= VS_READABLE;
		vs_collapse_state(so);


		switch (cmd) {
		case  VS_SLEEP_TIMEO:
			/*
			 * Clear the check flag to allow state check requests
			 * to be re-issued
			 */
			rvs->rvs_flags &= ~VS_STATE_CHECK;
			wakeup((caddr_t)&so->so_timeo);
			break;
		case VS_SLEEP_RECV:
			/*
			 * Clear the check flag to allow read check requests
			 * to be re-issued
			 */
			rvs->rvs_flags &= ~VS_READ_CHECK;
			sowakeup(so,&so->so_rcv);
			break;
		}
	} else {
		VSDEBUG(VSDEBMISC,("index mismatch, vs_index %d index %d\n",
			tmpindex,index));
	}

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return KERN_SUCCESS;
}


#ifdef	STALE_CODE
kern_return_t
nsrv_r_vs_getstate_immed(
	mach_port_t		server_port,
	mach_port_t		cred_port,
	transaction_id_t	transid,
	int			*state,
	int			*soerror,
	int			*qlen,
	int			*isreadable)
{
	kern_return_t		kr;
	struct socket		*so;
	DOMAIN_FUNNEL_DECL(f)

	kr = start_vsockserver_op(server_port, cred_port, transid, 
					SYS_VSOCK_getstate_imm, 0);
	if (kr != KERN_SUCCESS)
		return(kr);

	/*
	 * Retrieve the secondary socket's state.
	 */
	PORT_TO_VSOCK_LOOKUP(server_port, so);
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	*state = so->so_state;
	*soerror = so->so_error;
	*qlen = so->so_qlen;
	*isreadable = soreadable(so);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);

	return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
}
#endif	/* STALE_CODE */


/*simpleroutine*/
kern_return_t
nsrv_r_vs_getstate_delay(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	mach_port_t	target_port,	/* MACH_MSG_TYPE_PORT_NAME */
	int		index,
	int		state,
	int		soerror,
	int		qlen)
{
	struct socket	*so;
	int		error;
	kern_return_t	kr;
	vs_socket_t	*rvs;
	DOMAIN_FUNNEL_DECL(f)

	  
	VSDEBUG(VSDEBTEMP,("nsrv_r_vs_getstate_delay: port 0x%x\n", 
			server_port));

	kr = start_vsockserver_op(server_port, cred_port, 0, 
					SYS_VSOCK_getstate_del, 0);
	if (kr != KERN_SUCCESS) {
		VSDEBUG(VSDEBERROR, ("nsrv_r_vs_getstate_delay: start_vsockserver_op failed. kr %d\n", kr) );
		/* ux_server_loop/mach_msg_destroy nukes creds port */
		return kr;
	}

	PORT_TO_VSOCK_LOOKUP_ASYNC(server_port, so);
	VSDEBUG(VSDEBTEMP, ("nsrv_r_vs_getstate_delay: so 0x%x\n", so));
	if (so == NULL) {
		VSDEBUG(VSDEBTEMP,("nsrv_r_vs_getstate_delay: so == NULL") );
		return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
	}
	LOCK_ASSERT("nsrv_r_vs_getstate_delay", SOCKET_ISLOCKED(so));

	DOMAIN_FUNNEL(sodomain(so), f);

	/*
	 *  Sanity check: did this socket close and reopen before
	 *  this asynchronous RPC could arrive?
	 */
	rvs = (vs_socket_t *)so->vs_data;
	ASSERT(rvs != NULL);
	if (rvs->rvs_state_index != (index - 1)) {
		VSDEBUG(VSDEBTEMP, ("nsrv_r_vs_getstate_delay: index mismatch, "
				    "so 0x%x idx %d\n",
				    so, index));
		DOMAIN_UNFUNNEL(f);
		SOCKET_UNLOCK(so);
		return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
	} else {
		rvs->rvs_state_index = index;
	}

	/*
	 *  We must take an additional vsocket reference here, so that
	 *  nsrv_r_vs_soclose() won't destroy the socket out from
	 *  under us while we sleep.  That routine will awaken any
	 *  threads sleeping here; the last dereference will close the
	 *  socket.
	 */
	INCREMENT_VSNET_REFCNT(so, "enter nsrv_r_vs_getstate_delay:");

	/*
	 *  Ours should be the only thread waiting for this secondary
	 *  socket to become readable.  This is guaranteed by setting
	 *  the VS_STATE_CHECK flag in the rvs at the primary socket.
	 *  If some other thread already set VS_SLEEP_TIMEO, we got
	 *  trouble.
	 */
	ASSERT((so->vs_sleep_cmd & VS_SLEEP_TIMEO) == 0);

	/*
	 *  If the state of this secondary socket agrees with that
	 *  recorded in the corresponding rvs at the primary, and we
	 *  are not closing, go to sleep waiting for an event.
	 */
	if ((state == so->so_state) && (soerror == so->so_error) && 
	    (qlen == so->so_qlen) && ((so->vs_flags & VS_CLOSING)== 0)) {
		so->vs_sleep_cmd |= VS_SLEEP_TIMEO;
		VSDEBUG(VSDEBTEMP,("nsrv_r_vs_getstate_delay: go to sleep. so 0x%x\n",so));
		error = sosleep(so,
				(caddr_t)&so->so_timeo,
				(PZERO+1) | PCATCH,
				0 /* no timeout */);
		VSDEBUG(VSDEBTEMP,("nsrv_r_vs_getstate_delay: awake. so 0x%x\n", so));
		so->vs_sleep_cmd &= ~VS_SLEEP_TIMEO;
		if(error)
			so->so_error = error;
	}

	/*
	 *  Either our state is different than the primary's rvs,
	 *  or an event has happened and wakeup was called.
	 */
	if ((so->vs_flags & VS_CLOSING) == 0) {
		kern_return_t	kr;
		VSDEBUG(VSDEBTEMP,("nsrv_r_vs_getstate_delay: wakeup call. so 0x%x\n", so));
		kr = r_m_vs_wakeup(so->vs_callback, target_port, index, 
			   VS_SLEEP_TIMEO,
			   so->so_state, so->so_error, so->so_qlen,
			   soreadable(so));
		if (kr != KERN_SUCCESS) {
			VSDEBUG(VSDEBERROR,
		    ("nsrv_r_m_vs_sosleep: r_m_vs_wakeup: kr 0x%x port 0x%x\n",
			     kr,so->vs_callback));
		}
	}

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	DECREMENT_VSNET_REFCNT(so, "leave nsrv_r_vs_getstate_delay:");
	VSDEBUG(VSDEBTEMP, ("nsrv_r_vs_getstate_delay: returning. so 0x%x\n",so) );
	return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
}


int
nsrv_r_vs_soshutdown(
	mach_port_t	server_port,
	int		how)
{
	struct socket	*so;
	
	VSDEBUG(VSDEBENTRY,("r_vs_soshutdown 0x%x\n",server_port));

	PORT_TO_VSOCK_LOOKUP(server_port,so);
	return soshutdown(so,how);
}


/*
 * This checks for the readability of secondary sockets.
 * Only called if a socket is bound to multiple network servers.
 *
#ifdef	notyet
 * XXX To avoid the thread_yield in vs_soreadable() for the nbio case,
 * this RPC could be made synchronous with semantics like
 *	if (soreadable(2ndary)) {
 *		return TRUE
 *	} else {
 *		r_vs_soreadable_async(2ndary)
 *		return FALSE
 *	}
 * Thus after the calling this synchronous RPC for all secondaries,
 * nbio would know immediately whether to return EWOULDBLOCK and
 * there would be no need to yield.
#endif
 */
/*simpleroutine*/
int
nsrv_r_vs_soreadable(
	mach_port_t	server_port,
	mach_port_t	cred_port,
	mach_port_t	target_port,	/* MACH_MSG_TYPE_PORT_NAME */
	int		index)
{
	struct socket	*so;
	kern_return_t	kr;
	vs_socket_t	*rvs;
	DOMAIN_FUNNEL_DECL(f)

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_soreadable 0x%x\n", server_port));

	kr = start_vsockserver_op(server_port, cred_port, 0, 
					SYS_VSOCK_soreadable, 0);
	if (kr != KERN_SUCCESS) {
		/* ux_server_loop/mach_msg_destroy nukes creds port */
		return kr;
	}

	PORT_TO_VSOCK_LOOKUP_ASYNC(server_port, so);
	if (so == NULL)
		return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
	LOCK_ASSERT("nsrv_r_vs_soreadable", SOCKET_ISLOCKED(so));

	DOMAIN_FUNNEL(sodomain(so), f);

	/*
	 *  Sanity check: did this socket close and reopen before
	 *  this asynchronous RPC could arrive?
	 */
	rvs = (vs_socket_t *)so->vs_data;
	ASSERT(rvs != NULL);
	if (rvs->rvs_read_index != (index - 1)) {
		VSDEBUG((VSDEBSOREADABLE|VSDEBMISC),
		    ("nsrv_r_vs_soreadable: index mismatch, so 0x%x idx %d\n",
		     so, index));
		DOMAIN_UNFUNNEL(f);
		SOCKET_UNLOCK(so);
		return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
	} else {
		rvs->rvs_read_index = index;
	}

	/*
	 *  We must take an additional vsocket reference here, so that
	 *  nsrv_r_vs_soclose() won't destroy the socket out from
	 *  under us while we sleep.  That routine will awaken any
	 *  threads sleeping here; the last dereference will close the
	 *  socket.
	 */
	INCREMENT_VSNET_REFCNT(so, "enter nsrv_r_vs_soreadable:");

	/*
	 *  Ours should be the only thread waiting for this secondary
	 *  socket to become readable.  This is guaranteed by setting
	 *  the VS_READ_CHECK flag in the rvs at the primary socket.
	 *  If some other thread already set VS_SLEEP_RECV, we got
	 *  trouble.
	 */
	ASSERT((so->vs_sleep_cmd & VS_SLEEP_RECV) == 0);

	/*
	 *  Sleep waiting for this secondary to become readable.
	 */
	while ( !soreadable(so) && !(so->vs_flags & VS_CLOSING) ) {
		so->vs_sleep_cmd |= VS_SLEEP_RECV;
		sosbwait(&so->so_rcv,so);
		so->vs_sleep_cmd &= ~VS_SLEEP_RECV;
	}

	if ((so->vs_flags & VS_CLOSING) == 0) {
		/*
		 *  This secondary socket is now readable; notify the
		 *  primary socket via its callback port.
		 */
		kr = r_m_vs_wakeup(so->vs_callback, target_port, index,
				   VS_SLEEP_RECV, so->so_state, so->so_error,
				   so->so_qlen, TRUE);
		if (kr != KERN_SUCCESS) {
			VSDEBUG(VSDEBERROR,
		    ("nsrv_r_m_vs_sosleep: r_m_vs_wakeup: kr 0x%x port 0x%x\n",
				 kr,so->vs_callback));
			VSDEBUGX(VSDEBMISC,
				 print_port_info(so->vs_callback,"Parent"));
		}
	}

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	DECREMENT_VSNET_REFCNT(so, "leave nsrv_r_vs_soreadable:");
	return end_vsockserver_op(cred_port, KERN_SUCCESS, 0);
}

	
nsrv_r_vs_soreserve(
	mach_port_t	server_port,
	int		sendcc,
	int		recvcc,
	int		*result)
{
	struct socket *so;

	VSDEBUG(VSDEBENTRY,("nsrv_r_vs_soreserve 0x%x\n",server_port));

	PORT_TO_VSOCK_LOOKUP(server_port,so);

	*result = soreserve(so,sendcc,recvcc);

	return KERN_SUCCESS;
}


nsrv_r_vs_so_unlock(
	mach_port_t	server_port)
{
	struct socket	*so;
	DOMAIN_FUNNEL_DECL(f)

	PORT_TO_VSOCK_LOOKUP(server_port,so);

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return KERN_SUCCESS;
}

kern_return_t
nsrv_if_assign_port_number(
	mach_port_t		server_port,
	int			domain,
	short			protocol,
	ushort			*port,
	int			*rval)
{
	chouse_t		*chp;
	portmap_t		*pmp;
	protocol_portns_t	*pnsp;

	VSDEBUG(VSDEBENTRY, ("entered nsrv_if_assign_port_number\n"));

	/*
	 * These protocols don't allow anything for a port #
	 * except 0
	 */
	if (protocol == IPPROTO_RAW || protocol == IPPROTO_ICMP) {
		*port = 0;
		return ESUCCESS;
	}

	/* Get address of clearinghouse */
	chp =  &netserv_chouse;

	/* Loop through list of domains looking for a match */
	for (pmp = chp->ch_portmap; pmp; pmp = pmp->pm_next) {
		if (pmp->pm_domain == domain) {
			VSDEBUG(VSDEBNODEBIND,		  
				("nsrv_if_assign_port_number: chp 0x%x "
				"pmp 0x%x pm_domain %d\n", 
				pmp, chp, pmp->pm_domain));
			break;				
		}
	}
	if (pmp == NULL) {
		panic("nsrv_if_assign_port_number: no such domain %d\n",
			domain);
	}


	/* Loop through list of protocols looking for a match */
	for (pnsp = pmp->pm_proto; pnsp; pnsp = pnsp->pns_next) {
		if (pnsp->pns_protocol == protocol)
			break;
	}
	if (pnsp == NULL) {
		panic("nsrv_if_assign_port_number: no such protocol %d\n",
			protocol);
	}

	/* Lock the match */
	mutex_lock(&pnsp->pns_lock);
	
	/* 
	 * Assign number.  This code closely resembles code in
	 * in_pcbbind().
	 */
	if (pnsp->pns_port++ < IPPORT_RESERVED ||
	    pnsp->pns_port > IPPORT_USERRESERVED)
		pnsp->pns_port = IPPORT_RESERVED;
		
	*port = htons(pnsp->pns_port);

	/* unlock */
	mutex_unlock(&pnsp->pns_lock);

	return ESUCCESS;
}


