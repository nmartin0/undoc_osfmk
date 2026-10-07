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
 * $Log: vs_init.c,v $
 * Revision 1.15  1995/02/01  23:24:14  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.14  1994/11/18  20:52:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/05/04  22:18:54  mjl
 * Merge revision 1.10.4.3 from R1_2 branch into main trunk.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.10.4.3  1994/04/27  23:06:02  yazz
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
 * TNC select rewrite.  Turn off the questionable and confusing
 * vs_optimize_for_local_ns patch variable.  Replace various init macros.
 *
 * Revision 1.10.4.2  1993/12/21  22:45:15  dbm
 *  Reviewer: Nina Lepak, B Olsen (Locus), Dave Minturn
 *  Risk:Medium
 *  Benefit or PTS #:7424
 *  Testing: Tested for specific test case under 1.2.  This
 * 	  is the 1.2 branch checkin for the mainline 1.12 revision.
 *
 *  Module(s):
 * 	vs_init.c
 *
 * Revision 1.12  1993/12/10  22:05:36  nina
 * Fixed bugs that prevented Paragons from being used
 * as NFS clients if the boot node is not a network
 * server node.  See #6831, #6719, #7421, #7422, #7423
 * #7424 and #7426. If the clearinghouse node was configured
 * to be a node other than the bootnode, the system would
 * hang during boot.  This was due to the fact that
 * vs_init() running on the boot node attempted a
 * NORMA call against a node that wasn't up yet.
 * vs_init() and clearinghouse_init() were restructured
 * and the function find_clearinghouse() was added to get
 * around this problem.
 *
 *
 *  Reviewer:bolsen@locus.com, dbm@ssd.intel.com
 *  Risk:Medium
 *  Benefit or PTS #:#7424
 *  Testing:Lachman NFS main suite, various configs, Locus network testing
 *  Module(s):./server/vsocket/vs_init.c
 *
 * Revision 1.11  1993/12/07  17:45:16  mjl
 * Renamed USER_DRP_TO_CBK_* hash table macros to be UDRP_TO_VS_* .
 *
 *
 *  Reviewer: cfj@ssd.intel.com, bhk@locus.com
 *  Risk: low
 *  Benefit or PTS #: 7272
 *  Testing: Locus network tests
 *  Module(s): server/vsocket/sys_vsocket.c, server/vsocket/vs_init.c,
 * 	server/vsocket/vs_subr.c, server/vsocket/vs_subr.h
 *
 * Revision 1.10  1993/09/01  01:40:59  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.9  1993/07/14  18:49:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  21:13:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/05/18  03:21:15  cfj
 * Complete MI Driver merge.
 *
 * Revision 1.7  1993/05/06  19:33:31  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 3.20  93/08/23  00:16:46  mjl
 * Add virtual socket statistics structure.
 * 
 * Revision 3.19  93/08/03  17:11:17  mjl
 * Use macros in vs_subr.h to declare and initialize select-related port
 * hash tables.
 * 
 * Revision 3.18  93/07/27  11:43:28  mjl
 * Cosmetic programming style changes.  Some improved debug printfs.
 * 
 * Revision 3.17  93/06/19  15:43:52  yazz
 * [ ad1.04 merge ]
 * 	Respect the new polymorphism of a netname_check_in_poly() parameter.
 * 
 * Revision 3.16  93/05/05  22:44:47  mjl
 * MIv3 doesn't require ARP registry, so don't init it here.
 * 
 * Revision 3.15  93/04/03  11:53:38  klh
 * Allow local optimization to be enabled at runtime.
 * (klh for bhk)
 * 
 * Revision 3.14  93/03/22  21:21:25  yazz
 * OSF lock changes.  Cthread_yield() calls become thread_yield().
 * 
 * Revision 3.13  93/03/19  17:33:07  bhk
 * added initialization of the delay hash table
 * 
 * Revision 3.12  93/02/23  17:49:54  mjl
 * Enclose MI-specific code in #if NMI > 0 .
 * 
 * Revision 3.11  93/02/22  17:44:04  mjl
 * Initialize MI address resolution clearinghouse.
 * 
 * Revision 3.10  93/02/10  21:04:05  klh
 * Extra checkin to sync up RCS comments correctly
 * 
 * Revision 3.9  93/02/09  18:15:45  mjl
 * Initialize clearinghouse data structures used by virtual socket subsystems.
 * 
 * Revision 3.9  93/02/08  16:55:02  bhk
 * declared and initialized the hash tables needed to keep track of selected
 * vsockets to insure that the file ports send rights are properly decremented
 * 
 * Revision 3.8  92/10/27  17:52:24  bhk
 * Removed the debug thread
 * 
 * Revision 3.7  92/09/24  17:14:36  bhk
 * moved clearinghouse intialization into the general initialization
 * 
 * Revision 3.6  92/08/25  14:56:08  chrisp
 * Fix for bug #49: change cthread_yield() to a sleep of 2 seconds to prevent
 * the root fs node being overloaded with norma messages and failing to
 * initialize.
 * 
 * Revision 3.5  92/07/26  17:53:06  bhk
 * cleaned up debug
 * 
 * Revision 3.4  92/07/07  15:21:30  roman
 * Format corrections for consistent look in the code.
 * Fix minor typo in printf.
 * 
 * Revision 3.3  92/06/16  18:46:38  bhk
 * added error code to panic
 * 
 * Revision 3.2  92/04/20  17:30:52  bhk
 * Added clearinghouse initialization
 * 
 * Revision 3.1  92/03/20  17:08:28  bhk
 * moved vsocket.h to vsocket directory
 * 
 * Revision 3.0  92/03/04  14:56:44  bhk
 * Genesis	bhk
 * 
 *
 * Genisis 12/5/91 bhk
 *
 */

#include "sys/param.h"
#include "sys/types.h"
#include "uxkern/import_mach.h"
#include "sys/mbuf.h"
#include "sys/uio.h"
#include "sys/socket.h"
#include "vsocket/vsocket.h"
#include "vsocket/vs_chouse.h"
#include <mach/norma_special_ports.h>
#include "../user/include/servers/netname_defs.h"
#include "vsocket/vs_subr.h"
#include "sys/errno.h"

extern node_t	this_node;
extern mach_port_t	nameserver_port;
extern mach_port_t	privileged_host_port;

void clearinghouse_init();

/*
 * netserver_node is set equal to the bootmagic variable
 * NETSERVER. If a user doesn't specify a node number 
 * when ifconfiging network interface hardware, this value 
 * is used as the default device node number and is stored 
 * in the dv_node field of the device's struct ifconfig.
 */
node_t			netserver_node = -1;

/*
 * The bootmagic CLEARINGHOUSE can be used to specify
 * which node should maintain the clearinghouse database.
 * If it is not specified, it defaults to NETSERVER. The
 * variable cl_nameserver_port contains the MACH port of 
 * the name server on the clearinghouse node. NOTE: the
 * clearinghouse node MUST be a network server node.
 */
#define	CLEARINGHOUSE	"CLEARINGHOUSE"
mach_port_t		clearinghouse_port = MACH_PORT_NULL;
mach_port_t		chs_nameserver_port = MACH_PORT_NULL;
node_t			clearinghouse_node = -1;

mach_port_t		vs_server_port = MACH_PORT_NULL;
int			vs_optimize_for_local_ns = FALSE;

struct vs_stats		vsstats;	/* virtual socket statistics */

/* Select id mapping tables for select(2), see <vsocket/vs_subr.h> */
VS_SECNDRY_LOCK_DECL;
SELID_DRP_DECL;
SELID_TO_SVS_DECL;


void
vs_init()
{
	kern_return_t	kr;

	VSDEBUG(VSDEBENTRY,("vs_init\n"));

	if(netserver_node == -1) {
		panic("vs_init:No netserver node\n");
		return;
	}

	clearinghouse_init();
	chouse_init(&netserv_chouse, "netsrv");

	if(mach_port_allocate(mach_task_self(),
			      MACH_PORT_RIGHT_RECEIVE,
			      &vs_server_port) != KERN_SUCCESS)
		panic("vsinit: unable to allocate vs_server_port\n");
	if(mach_port_insert_right(mach_task_self(),
				  vs_server_port,
				  vs_server_port,
				  MACH_MSG_TYPE_MAKE_SEND) != KERN_SUCCESS)
		panic("vs_init: vs_server_port unable to set rights\n");

	/* Various lookup tables for TNC select(2) support, see vs_subr.h . */
	VS_SECNDRY_LOCK_INIT;
	SELID_DRP_INIT;
	SELID_TO_SVS_INIT;

	ux_server_add_port(vs_server_port);

#include "inet.h"
#if INET > 0
	inetsrv_init();
#endif
}


void
clearinghouse_init()
{
	static int isinited = 0;
	kern_return_t	kr;

	/* there's no sense in being redundant */
	if (isinited++)
		return;

	/*
	 * If we did not configure CLEARINGHOUSE in the boot magic use the
	 * netserver node
	 */
	if (clearinghouse_node == -1)
		clearinghouse_node = netserver_node;

	if (clearinghouse_node  == this_node) { 

		/* set up name server port */
		chs_nameserver_port = nameserver_port;

		/* 
		 * Allocate a port for the clearinghouse.
		 * Make a send right and then register with
		 * the local name server.
		 */
		kr = mach_port_allocate(mach_task_self(),
					MACH_PORT_RIGHT_RECEIVE,
					&clearinghouse_port);

		if (kr != KERN_SUCCESS)
		    panic("vs_init: mach_port_allocate: kr 0x%x\n", kr);

		kr = mach_port_insert_right(mach_task_self(),
					    clearinghouse_port,
					    clearinghouse_port,
					    MACH_MSG_TYPE_MAKE_SEND);
		if (kr != KERN_SUCCESS)
		    panic("vs_init: mach_port_insert_right: kr0x%x\n", kr);

		kr = netname_check_in_poly(chs_nameserver_port,
					   CLEARINGHOUSE,
					   mach_task_self(),
					   MACH_MSG_TYPE_COPY_SEND,
					   clearinghouse_port,
					   MACH_MSG_TYPE_COPY_SEND);
		if (kr != KERN_SUCCESS)
			panic("vs_init: netname_check_in_poly: kr0x%x\n", kr);

		ux_server_add_port(clearinghouse_port);


	}
}


/* Name:	find_clearinghouse()
 *
 * Function:
 *		This function was originally written to get
 *		around a MACH bug.  Specifically if node A
 *		tried to contact node B before bootmesh()
 *		had run, the system would hang.  
 *		clearinghouse_init() was rewritten so that
 *		it basically did nothing if it wasn't running
 *		on the designated clearinghouse node. Everybody
 *		else delayed doing their initialization until
 *		just before making a clearinghouse request.
 *		This approach has the added benefit of providing
 *		a hook that can be used in the future to remove
 *		the clearinghouse as a single point of failure.
 *
 * Inputs:
 *		None
 *
 * Outputs:
 *		None
 *
 * Errors:
 *		panics if microkernel fails us
 *
 *		ENOENT - can't find the clearinghouse name server 
 *		ENOENT - clearinghouse not checked in with name server
 *		
 */ 
int
find_clearinghouse()
{
	kern_return_t	kr;

	/*
	 * If we did not configure CLEARINGHOUSE in the boot magic use the
	 * netserver node
	 */
	if (clearinghouse_node == -1) 
		clearinghouse_node = netserver_node;
	
	/*
	 * Get the name server port on the clearinghouse
	 * node via the NORMA call and then use that 
	 * port to ask the name server for the 
	 * clearinghouse port.
	 */
	if((chs_nameserver_port == MACH_PORT_NULL) ||
	      (chs_nameserver_port == MACH_PORT_DEAD)){
		
		VSDEBUG(VSDEBCONTROL,("getting chs nameserver port\n"));
		if(norma_get_nameserver_port(
			privileged_host_port,
			clearinghouse_node,
			&chs_nameserver_port) 
			!= KERN_SUCCESS)
				panic(
				 "vs_init: can't get clearinghouse node\n");

		if(chs_nameserver_port == MACH_PORT_NULL ||
			chs_nameserver_port == MACH_PORT_DEAD) {

			VSDEBUG((VSDEBCONTROL | VSDEBENTRY),
				("chs_nameserver_port bad 0x%x\n",\
					chs_nameserver_port));
			return(ENOENT);
		}
	}

	/* look up the clearinghouse port */
	if(clearinghouse_port == MACH_PORT_NULL) {
		kr = netname_look_up(chs_nameserver_port,
				     CLEARINGHOUSE,
				     CLEARINGHOUSE,
				     &clearinghouse_port);

		if (kr == NETNAME_NOT_CHECKED_IN) {
			printf(
			 "find_clearinghouse: clearinghouse not checked in\n");
			return(ENOENT);
		}		
		else if (kr != NETNAME_SUCCESS)
			panic("vs_init:unknown netname_lookup err 0x%x\n", kr);

	}
	return(ESUCCESS);
}
