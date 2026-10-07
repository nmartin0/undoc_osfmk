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
 * $Log: tnc_ipc.c,v $
 * Revision 1.12  1995/02/01  21:55:08  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/11/18  20:44:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/06/11  01:08:51  yazz
 *  Reviewer: none
 *  Risk: very lo
 *  Benefit or PTS #: augment panic msg to help with #9739
 *  Testing: all builds & boots multiuser
 *  Module(s): server/tnc/tnc_ipc.c
 *
 * Added extra info to panic message.
 *
 * Revision 1.9  1993/10/21  23:36:47  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.8  1993/07/14  18:34:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:48:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/18  03:20:27  cfj
 * Complete MI Driver merge.
 *
 * Revision 1.6  1993/05/06  19:26:03  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:25  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/03/29  18:26:15  cfj
 * Merge with T9.
 *
 * Revision 1.2.8.2  1993/03/29  18:16:58  cfj
 * More ux_server_thread_block/unblocking from Locus.
 *
 * Revision 1.4  1993/03/24  20:14:18  cfj
 * Merge with T9.
 *
 * Revision 1.2.8.1  1993/03/24  20:11:54  cfj
 * Change calls to cthread_yield() to thread_yield().
 *
 * Revision 1.3  1993/03/03  13:58:42  stefan
 * Node number translation is now done in get_tnc-port() instead of
 * bsd_get_tnc-port() (file tnc_server_side.c).
 *
 * Revision 1.2  1992/11/30  22:48:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:46:54  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.13  93/09/16  09:29:48  chrisp
 * [SPE 0030] Generic Spanning Trees: ops tables are now 3-way vectors.
 * 
 * Revision 3.12  93/06/19  15:30:10  yazz
 * [ ad1.04 merge ]
 * 	Respect the new polymorphism of send right parameter to 
 * 	netname_check_in_poly().
 * 
 * Revision 3.11  93/05/05  16:36:39  bolsen
 * [Bug 239] changed call to is_node_valid() to is_tnc_node_valid().
 * 
 * Revision 3.10  93/03/27  17:04:06  yazz
 * Added some ux_server_thread_blocking/unblocking() calls.
 * 
 * Revision 3.9  93/03/22  21:15:06  yazz
 * OSF lock changes.  Change cthread_yield() calls to thread_yield().
 * 
 * Revision 3.8  92/09/29  08:21:18  roman
 * Get rid of unused routine svr_tnc_node_present().
 * Add the concept of a per-node table of private virtual process system
 * 	operations (pvpsops).
 * 
 * Revision 3.7  92/08/25  14:51:17  chrisp
 * Fix for bug #49: sleep 2 seconds between attempts to contact the root fs node
 * when registering the TNC server. This prevents node 0 being swamped with
 * norma messages and so failing to initialize.
 * 
 * Revision 3.6  92/07/28  12:56:17  chrisp
 * Correct tnc_get_port() to re-use slots referencing dead names.
 * 
 * Revision 3.5  92/07/09  11:36:57  roman
 * Add routine tnc_exception_setup() which is called on all nodes that do
 * not run the init process. This routine queries the node that does run
 * the init process and set up the emulator exception addresses with
 * the same values that are set up on the node that runs the init process.
 * 
 * Revision 3.4  92/07/08  09:13:37  roman
 * Change strategy of setting up TNC server ports to use the local name
 * 	server to register and obtain the server port, rather than 
 * 	using a NORMA special port.
 * Change node numbers to type node_t.
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.3  92/05/06  13:58:56  roman
 * Change get_tnc_port() and set_tnc_port() to actually match the
 * 	newly written man pages with respect to behavior on
 * 	boundary and error conditions.
 * 
 * Revision 3.2  92/02/26  08:20:16  roman
 * Add support for system calls get_tnc_port() and set_tnc_port().
 * Add the server code that is used to support the get_tnc_port() system call.
 * Use the routine is_node_valid() when trying to obtain the server
 * port for a remote node.
 * 
 * Revision 3.1  91/11/12  14:30:51  roman
 * Fix minor typo in call to mach_port_insert_right().
 * 
 * Revision 3.0  91/10/21  13:20:29  roman
 * Deal with inter-node inter-server Mach IPC. Initial submission.
 * 
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/user.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <sys/ux_exception.h>
#include <uxkern/import_mach.h>
#include <mach/norma_special_ports.h>
#include "../user/include/servers/netname_defs.h"

extern mach_port_t privileged_host_port;
extern node_t	   this_node;

extern struct pvps_ops_vector rpvps_ops_table;
extern struct pvps_ops_vector dpvps_ops_table;
/*
 * Table of ports used for inter-node communication.
 */
#define		TNC_SERVER		"TNC_SERVER"
#define 	TNC_SERVER_TABLE_SIZE	4096
mach_port_t	tnc_server_port_table[TNC_SERVER_TABLE_SIZE];
struct pvps_ops_vector	*tnc_pvps_ops_table[TNC_SERVER_TABLE_SIZE];
mach_port_t	this_node_tnc_server_port = MACH_PORT_NULL;/* for this server */

/*
 * This routine is used to initialize the table used for inter-node
 * communication. It uses a "lazy" evaluation scheme, filling in
 * table entries only on initial request for a server port to a node.
 * Additionally, the port used to receive inter-node server requests
 * is initialized by this routine.
 */
void
tnc_ipc_init()
{
	kern_return_t	ret;
	register int	i;
	mach_port_t	nameserver_port;

	/*
	 * Initialize the table to all empty slots.
	 */
	for(i = 0; i < TNC_SERVER_TABLE_SIZE; i++) {
		tnc_server_port_table[i] = MACH_PORT_NULL;
		tnc_pvps_ops_table[i] = &rpvps_ops_table;
	}

	/*
	 * Find the nameserver port for the local node.
	 */
	ret = norma_get_nameserver_port(privileged_host_port,
					this_node,
					&nameserver_port);
	if (ret != KERN_SUCCESS)
		panic("tnc_ipc_init: norma_get_nameserver_port failure");

	/*
	 * Allocate a port for the server to use for responding to
	 * requests from other servers for TNC service. The port also
	 * needs a send right for the norma_set_special_port() to work.
	 */
	ret = mach_port_allocate(mach_task_self(), 
				 MACH_PORT_RIGHT_RECEIVE,
				 &this_node_tnc_server_port);
	if (ret != KERN_SUCCESS)
		panic("tnc_ipc_init: mach_port_allocate failure");
	ret = mach_port_insert_right(mach_task_self(), 
				     this_node_tnc_server_port,
				     this_node_tnc_server_port,
				     MACH_MSG_TYPE_MAKE_SEND);
	if (ret != KERN_SUCCESS)
		panic("tnc_ipc_init: mach_port_insert_right failure");

	/*
	 * Set up the server node port allocated above to be the TNC server 
	 * port for inter-node communication. We do this by registering
	 * the port with the name server.
	 */
	ret = netname_check_in_poly(nameserver_port,
				    TNC_SERVER,
				    mach_task_self(),
				    MACH_MSG_TYPE_COPY_SEND,
				    this_node_tnc_server_port,
				    MACH_MSG_TYPE_COPY_SEND);
	if (ret != KERN_SUCCESS) {
		panic("tnc_ipc_init: netname_check_in_poly failure");
	}

	/*
	 * Add the server node port for the current node to the list of
	 * ports being handled by the server.
	 */
	ux_server_add_port(this_node_tnc_server_port);

	/*
	 * Put the server port in the server-wide table.
	 */
	if (this_node < 0 || this_node >= TNC_SERVER_TABLE_SIZE)
		panic("tnc_ipc_init: tnc_mynode invalid");
	tnc_server_port_table[this_node] = this_node_tnc_server_port;

	/*
	 * Set up the local pvps operations table.
	 */
	tnc_pvps_ops_table[this_node] = &dpvps_ops_table;

	/*
	 * No longer require the netserver_port.
	 */
	(void) mach_port_deallocate(mach_task_self(), nameserver_port);
}

/*
 * This routine is used to obtain send rights to the TNC server on
 * specific node (typically not the current node).
 *
 * The routine ensures that communication with the TNC server on the
 * node either is currently possible or was possible at some time
 * in the past (i.e. the node number repesents a node that has/had
 * a TNC server running).
 */
int
tnc_get_server_port(
	node_t		node, 
	mach_port_t	*server_portp)
{
	kern_return_t	ret;
	mach_port_t	nameserver_port;

	/*
	 * First, we make a sanity check and make sure the specified node fits 
	 * into the local table.
	 */
	if (node < 0 || node >= TNC_SERVER_TABLE_SIZE)
		return(EINVAL);

	/*
	 * Check to see if the node is in the node table.
	 */
	if (!is_tnc_node_valid(node))
		return(EINVAL);

	/*
	 * Check if we have sent a message to the specified node at some time 
	 * in the past and the message failed. Currently, we assume that
	 * retrying is a waste of resources.
	 */
	if (tnc_server_port_table[node] == MACH_PORT_DEAD)
		return(EINVAL);

	/*
	 * Check if we have sent a message to the specified node at some time 
	 * in the past and the message succeeded. If so, we return the
	 * port that we stored.
	 */
	if (tnc_server_port_table[node] != MACH_PORT_NULL) {
		*server_portp = tnc_server_port_table[node];
		return(ESUCCESS);
	}

	/*
	 * If we get here, it's the first try at communication with
	 * the specified node.
	 *
	 * First we try to get the nameserver port for the node (we wait
	 * for the nameserver to come up). Then we get the TNC server
	 * for the node (we wait for the port to be registered).
	 */
	do {
		ret = norma_get_nameserver_port(privileged_host_port,
						node,
						&nameserver_port);
		if (ret != KERN_SUCCESS)
			panic("tnc_get_server_port: get_nameserver failure "
			 "ret=0x%x privport=0x%x", ret, privileged_host_port);
		if (nameserver_port == MACH_PORT_NULL || 
				nameserver_port == MACH_PORT_DEAD)
                mach_init_sleep(2);	/* wait 2 secs */
	} while (nameserver_port == MACH_PORT_NULL 
			|| nameserver_port == MACH_PORT_DEAD);

	do {
		ret = netname_look_up(nameserver_port,
				      TNC_SERVER,
				      TNC_SERVER,
				      server_portp);
		if (ret != NETNAME_SUCCESS) {
			if (ret != NETNAME_NOT_CHECKED_IN)
				panic("tnc_get_server_port: look_up failure "
				 "ret=0x%x ns_port=0x%x", ret, nameserver_port);
			thread_yield();
		}
	} while (ret != NETNAME_SUCCESS);

	tnc_server_port_table[node] = *server_portp;

	/*
	 * No longer require the netserver_port.
	 */
	(void) mach_port_deallocate(mach_task_self(), nameserver_port);

	return(ESUCCESS);
}


/*
 * Set up the emulator exception addresses for nodes that are not running
 * the init process. This routine is called early during the initialization
 * of all such nodes.
 *
 * Emulator exception addresses are addresses that are known to the server
 * to require special handling if SIGSEGV or SIGBUS violations occur.
 * When starting up the emulator for the init process, the emulator
 * informs the server of these addresses, but only on the node the
 * init process is on.
 */
void
tnc_exception_setup(
	node_t		init_node)
{
	int		error;
	kern_return_t	ret;
	mach_port_t	server_port;

	ASSERT(init_node != this_node);

	error = tnc_get_server_port(init_node, &server_port);
	if (error != ESUCCESS)
		panic("tnc_exception_setup: unable to get server port");

	ux_server_thread_blocking();
	ret = cli_tnc_get_exception_addr(server_port,
					 &emul_uacc_start,
					 &emul_uacc_end,
					 &emul_uacc_err);
	ux_server_thread_unblocking();
	if (ret != KERN_SUCCESS)
		panic("tnc_exception_setup: unable to get exception addresses");
	ASSERT(emul_uacc_start && emul_uacc_end && emul_uacc_err &&
	       emul_uacc_start < emul_uacc_end);
}


/*
 * Routine called by MiG on the node the init process runs on to fill in
 * the emulator exception addresses.
 */
kern_return_t
svr_tnc_get_exception_addr(
	mach_port_t	server_port,
	int		*uacc_start,
	int		*uacc_end,
	int		*uacc_err)
{
	/*
	 * Wait for the emulator to fill in the exception fields (if
	 * it has not already done so)
	 */
	while (!emul_uacc_start || !emul_uacc_end || !emul_uacc_err ||
			emul_uacc_start > emul_uacc_end) {
		thread_yield();
	}

	/*
	 * Fill in the parameters.
	 */
	*uacc_start = emul_uacc_start;
	*uacc_end = emul_uacc_end;
	*uacc_err = emul_uacc_err;

	return(KERN_SUCCESS);
}


/*
 * The array of special TNC ports that is manipulated by set_special_port()
 * and get_special_port().
 */
#define		SPECIAL_PORT_ARRAY_SIZE		10
mach_port_t	special_tnc_port[SPECIAL_PORT_ARRAY_SIZE];
int		special_tnc_port_id[SPECIAL_PORT_ARRAY_SIZE];
int		num_special_tnc_ports = 0;
 

/*
 * The set_tnc_port system call. Sets the special TNC port indexed by "id".
 */
int
set_tnc_port(
	int		id,
	mach_port_t	port)
{
	int		error;
	int		found = FALSE;
	int		freeslot = num_special_tnc_ports;
	register int	i;
	mach_port_t	special_port;
	kern_return_t	ret;
	mach_msg_type_name_t type;
	mach_port_type_t ptype;

	/*
	 * Super-user only on this system call.
	 */
	if (error = suser(u.u_cred, &u.u_acflag))
		return(error);

	/*
	 * See if this ID is already in the array. Also mark the first free
	 * slot while we are passing through.
	 */
	for (i = num_special_tnc_ports-1; i >= 0; i--) {
		if (special_tnc_port[i] == MACH_PORT_NULL)
			freeslot = i;
		else if (special_tnc_port_id[i] == id) {
			found = TRUE;
			break;
		}
	}
	
	/*
	 * Either set up the port in the table (if the port passed in
	 * was not MACH_PORT_NULL), or get rid of a port in the table.
	 */
	if (port != MACH_PORT_NULL) {
		if (found) {
			ret = mach_port_type(mach_task_self(),
					     special_tnc_port[i],
					     &ptype);
			if (ret != KERN_SUCCESS)
				panic("set_tnc_port: port_type failure");
			if (ptype != MACH_PORT_TYPE_DEAD_NAME)
				return(EEXIST);
			(void) mach_port_deallocate(mach_task_self(),
						    special_tnc_port[i]);
			freeslot = i;
		}
		if (freeslot >= SPECIAL_PORT_ARRAY_SIZE)
			return(ENOMEM);
		ret = mach_port_extract_right(u.u_procp->p_task,
					      port,
					      MACH_MSG_TYPE_MAKE_SEND,
					      &special_port,
					      &type);
		if (ret != KERN_SUCCESS)
			if (ret == KERN_INVALID_NAME || 
					ret == KERN_INVALID_RIGHT)
				return(EINVAL);
			else
				panic("set_tnc_port: extract_right failure");
		special_tnc_port[freeslot] = special_port;
		special_tnc_port_id[freeslot] = id;
		if (freeslot == num_special_tnc_ports)
			num_special_tnc_ports++;
	} else {
		if (!found)
			return(ENOENT);
		(void) mach_port_deallocate(mach_task_self(),
					    special_tnc_port[i]);
		special_tnc_port[i] = MACH_PORT_NULL;
	}

	return(ESUCCESS);
}


/*
 * The get_tnc_port system call. Gets the special TNC port indexed by
 * "id" from the node "node".
 */
int
get_tnc_port(
	int		id,
	int		node,
	mach_port_t	*portp)
{
	int		error;
	mach_port_t	server_port;
	kern_return_t	ret;

#ifdef NX
	/*
	 * Map logical node number to physical node number.
	 */
	node = nx_map_node_proc(u.u_procp, node);
	if (node == -1) {
	    return EINVAL;
	}
#endif /* NX */

	/*
	 * Super-user only on this system call.
	 */
	if (error = suser(u.u_cred, &u.u_acflag))
		return(error);

	/*
	 * Try to get a send right to the server node.
	 */
	error = tnc_get_server_port(node, &server_port);
	if (error != ESUCCESS)
		return(EINVAL);

	/*
	 * Go to the remote node to get its special port.
	 */
	ux_server_thread_blocking();
	ret = cli_get_tnc_port(server_port, id, portp);
	ux_server_thread_unblocking();
	if (ret != KERN_SUCCESS)
		panic("get_tnc_port: cli_get_tnc_port failure");
	if (*portp == MACH_PORT_NULL)
		return(ENOENT);

	return(ESUCCESS);
}


/*
 * Server node routine that fetches the special port and returns it.
 */
kern_return_t
svr_get_tnc_port(
	mach_port_t	server_port,
	int		id,
	mach_port_t	*portp)
{
	register int	i;

	*portp = MACH_PORT_NULL;
	for (i = 0; i < num_special_tnc_ports; i++) {
		if (special_tnc_port_id[i] == id &&
				special_tnc_port[i] != MACH_PORT_NULL) {
			*portp = special_tnc_port[i];
			break;
		}
	}

	return(KERN_SUCCESS);
}


/*
 * Routine used to obtain the pvps ops table address for a node.
 */
struct pvps_ops_vector *
tnc_get_pvps_ops(
	node_t		node)
{
	ASSERT(node < TNC_SERVER_TABLE_SIZE);

	return(tnc_pvps_ops_table[node]);
}
