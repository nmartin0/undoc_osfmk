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
 
#include 	<stdio.h>
#include 	<string.h>
#include 	<mach/port.h>
#include 	<mach/error.h>
#include 	<mach/message.h>
#include 	<mach/norma_special_ports.h>
#include	"bootmesh.h"

/*
** Routines which communicate directly with microkernel services.
**	poll_setup()
**	poll_nodes()
**	wait_for_server()
**	get_boot_magic()
*/

/*
 * Create a famous port and wait for all booted server to register
 * their presence.
 */
#define	SYNC_PORT_NAME		"SYNCSERVERINIT"
#define POLL_TIMEDOUT		-1
#define POLL_ERR		-2
#define POLL_RESOURCE_ERR	-3
#define POLL_TIMEOUT_VAL	(1 * 90 * 1000)		/* ninety seconds */
#define POLL_FIRST_TIMEOUT	(1 * 1000)		/* one sec per node */
#define POLL_NEXT_TIMEOUT	(300)			/* 300 msec per node */


static mach_port_t	SyncServerPort;

/* message expected from servers */
struct {
	mach_msg_header_t head;
	mach_msg_type_t nodeType;
	long  node;
	char node_attributes[1024];
} syncsvr;


poll_setup()
{
	mach_port_t     privileged_host_port;
	mach_port_t     name_server_port;
	kern_return_t   retcode;
 
#ifdef DEBUG
DBG("poll_setup: \n");
#endif DEBUG
	if (control(SKIP_POLLING))
		return(0);

	/*
	 * allocate the server synchronization port
	 */
	retcode = mach_port_allocate(
			mach_task_self(),
			MACH_PORT_RIGHT_RECEIVE, 
			&SyncServerPort );
	if (retcode != KERN_SUCCESS) {
		sprintf(err,"Unable to allocate port, mach err %d", retcode);
		ERR(err);
	}
#ifdef DEBUG
DBG("poll_setup: port allocated \n");
#endif DEBUG

#ifdef notneeded
	retcode = mach_port_insert_right(
			mach_task_self(),
			SyncServerPort,
			SyncServerPort,
			MACH_MSG_TYPE_MAKE_SEND );
	if (retcode != KERN_SUCCESS) {
		sprintf(err,"Can't create send right, mach err %d", retcode);
		ERR(err);
	}
#endif notneeded

	/*
	 * catalogue port with bootnode's name server 
	 */
	privileged_host_port = task_by_pid(-1);
	retcode = norma_get_nameserver_port(
			privileged_host_port,
			my_physical_node(), 	/* boot node */
			&name_server_port );
	if (retcode != KERN_SUCCESS) {
		sprintf(err,"Unable to get nameserver port, mach err %d",
			retcode);
		ERR(err);
	}
#ifdef DEBUG
DBG("poll_setup: nameserver found \n");
#endif DEBUG

       	retcode = netname_check_in(
			name_server_port,
			SYNC_PORT_NAME,
			MACH_PORT_NULL,
			SyncServerPort );
	if (retcode != KERN_SUCCESS) {
	        sprintf(err,"Port checkin failed, mach err %d", retcode);
	        ERR(err);
	}
#ifdef DEBUG
DBG("poll_setup: SyncServerPort registered with nameserver \n");
#endif DEBUG
	return(0);

}

poll_nodes(list)
nodetype	*list;
{
	mach_msg_timeout_t      	timeout;
	int             i;
	nodetype	*np;
	int		op_nodes = 0;
	int		acked_nodes = 0;
	int 		up_nodes, id;
	char*		attribute;
 
#ifdef DEBUG
DBG("poll_nodes: \n");
#endif DEBUG
	if (control(SKIP_POLLING)) {
		PROGRESS("\n");
		return(0);
	}

	VERBOSE("Polling servers...\n");

	/*
 	 * count operational nodes
	 */
	for (op_nodes = 0, np = list;
		np < &list[maxnodes]; np++)
			op_nodes += np->n_operational;
#ifdef DEBUG
DBG("poll_nodes: %d nodes were booted\n",op_nodes);
#endif DEBUG

	/*
	 * wait for a message from each server on each operational
 	 * node as it completes initialization
	 */
	up_nodes = op_nodes;
	timeout = (arg_timeout >= 0) ? arg_timeout : POLL_TIMEOUT_VAL +
		POLL_FIRST_TIMEOUT * op_nodes;

	VERBOSE("op_nodes = %d  timeout = %d\n", op_nodes, timeout);

	if (debug) {
	    VERBOSE("POLL_TIMEOUT_VAL = %d  POLL_FIRST_TIMEOUT = %d  POLL_NEXT_TIMEOUT = %d\n", POLL_TIMEOUT_VAL, POLL_FIRST_TIMEOUT, POLL_NEXT_TIMEOUT);
	}

	VERBOSE("Waiting for server sync messages, received:\n");

	while (up_nodes--) {

		/* wait for a message from any server */
		if (debug) {
		    VERBOSE("Waiting for server sync message...\n");
		}
		id = wait_for_server(timeout, &attribute);

		if (id == POLL_ERR || id == POLL_RESOURCE_ERR)
			ERR("server polling fails");

		if (id == POLL_TIMEDOUT) {

			/* give 'em maximum chance */
			if (poll_all)
				continue;

			/* else assume all unreported are broken */
			break;			
		}

		if ((maxnodes > 1) && (id >= maxnodes)) {
			/* out of range node, serious problems */
			fprintf(stderr,
				"ping received from bogus node %d\n",id);
			continue;
		}

		if (maxnodes == 1)
			np = list;
		else
			np = &list[id];

		np->n_attrs = strdup(attribute);

		if (!np->n_operational) {
			fprintf(stderr,
				"non-operational node %d responds to poll\n",
				id);
			continue;
		} 

		np->n_acked = 1;
		acked_nodes++;
		timeout = (arg_timeout >= 0) ? arg_timeout : POLL_TIMEOUT_VAL +
				POLL_NEXT_TIMEOUT * up_nodes;

		VERBOSE("%6d  ", id);
		if (debug) {
		    VERBOSE("  up_nodes = %d  timeout = %d\n", up_nodes, timeout);
		}

	}


	/* 
	 * report failed nodes:
	 * report number to stderr,
	 * list of nodes (suitable for further consumption) to stdout
	 */
	if (acked_nodes < op_nodes) {
		VERBOSE("\nWait for server sync messages timed-out\n");
		PROGRESS("\n");
		fprintf(stderr,"%s: %d nodes failed to boot\n",
			progname,op_nodes-acked_nodes);
		for (np = list; np < &list[maxnodes]; np++)
			if (np->n_operational && !np->n_acked) {
				printf("%d\n", np->n_id);
				PROGRESS("node %4d failed to initialize\n", np->n_id);
			}
	}
	else {
		VERBOSE("\nAll Servers successfully initialized...\n");
		PROGRESS("initialization complete.\n");
	}

#ifdef DEBUG
DBG("poll_nodes: booted nodes = %d, acked nodes = %d\n",op_nodes,acked_nodes);
#endif DEBUG

	return(0);
}


/*
 * listen on synchronization port for  incoming messages;
 * return node id in message body to caller or notification of timeout/errors
 */
wait_for_server(timeout, attribute)
	mach_msg_timeout_t      	timeout;
	char**				attribute;
{

	mach_msg_size_t 		max_size;
	register mach_msg_return_t 	mr;

	syncsvr.head.msgh_size = max_size = sizeof(syncsvr);
	syncsvr.head.msgh_local_port = SyncServerPort;

	mr = mach_msg(  &syncsvr.head,
			MACH_RCV_MSG|MACH_RCV_TIMEOUT,
		      	0,
			max_size,
			SyncServerPort,
		      	timeout,
			MACH_PORT_NULL );


	if (mr == MACH_MSG_SUCCESS) {
#ifdef DEBUG
DBG("wait_for_server: message succeeds\n");
DBG("\tnode %d\n",syncsvr.node);
DBG("\thead.msgh_size %d\n",syncsvr.head.msgh_size);
DBG("\tnodeType.msgt_size %d\n",syncsvr.nodeType.msgt_size);
DBG("\tnodeType.msgt_number %d\n",syncsvr.nodeType.msgt_number);
DBG("\tnode_attributes %s\n",syncsvr.node_attributes);
#endif DEBUG
		/* return respondent's node id and attributes */
		*attribute = syncsvr.node_attributes;
		return(syncsvr.node);
	}

	if (mr == MACH_RCV_TIMED_OUT) {
#ifdef DEBUG
DBG("wait_for_server: message times out\n");
#endif DEBUG
		return(POLL_TIMEDOUT);
	}

#ifdef DEBUG
DBG("wait_for_server: message err %x \n",mr);
#endif DEBUG
	return(POLL_ERR);
}


#define	BOOTMAGIC_TRAP

#ifdef	BOOTMAGIC_TRAP	/* use BOOTMAGIC_TRAP in place of mach rpc */

int
bootmagic_trap(buffer)
    char    *buffer;
{
    asm("trap r0,r25,r0");
}

/*
 * Request bootmagic from microkernel
 */
get_boot_magic(bootmagic)
	char	*bootmagic;
{
    int 	boot_magic_len;

    boot_magic_len = bootmagic_trap(bootmagic);

    if (boot_magic_len > 0)
	return (0);
}


#else	/* use mach rpc in place of BOOTMAGIC_TRAP */

/*
 * Request bootmagic from microkernel
 */
get_boot_magic(bootmagic)
	char	*bootmagic;
{
	kern_return_t       kr;
	mach_port_t privileged_host_port;

	privileged_host_port = task_by_pid(-1);
	kr = host_get_boot_info(privileged_host_port,bootmagic);
	if (kr != KERN_SUCCESS)
	    return(-1);
	return(0);
}

#endif	/* use mach rpc in place of BOOTMAGIC_TRAP */

