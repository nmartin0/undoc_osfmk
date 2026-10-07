/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */

/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1994  Intel Corporation.
 *
 *	$Id: dipc_debug.c,v 1.10 1994/11/18 20:57:42 mtm Exp $
 */

#include <mach_assert.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/machine/vm_types.h>
#include <mach/vm_param.h>
#include <mach/task_info.h>
#include <mach/task_special_ports.h>
#include <mach/port.h>
#include <kern/queue.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/assert.h>
#include <kern/processor.h>
#include <ipc/ipc_space.h>
#include <ipc/port.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_right.h>
#include <ipc/ipc_splay.h>
#include <ipc/ipc_entry.h>
#include <ipc/ipc_pset.h>
#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>
#include <norma2/norma_transport.h>
#include <norma2/dipc_uid.h>
#include <norma2/dipc_migrate.h>
#include <norma2/dipc_server.h>


db_dipc_stats()
{
	db_server_stats();
	db_dipc_client_stats();
	db_mqueue_stats();
	db_dipc_port_stats();
	db_dipc_recv_stats();
	db_dipc_tx_stats();
	db_uid_stats();
	
	return 0;
}

void db_dipc(int verbose)
{
	db_dipc_stats();
}

static char	*dipc_status_strs[] = {
	"DIPC_OK",
	"DIPC_DEST_INVALID",
	"DIPC_DEST_DEAD",
	"DIPC_DEST_FULL",
	"DIPC_DEST_MOVED",
	"DIPC_BOGUS_REQUEST"
};

static char	*dipc_type_strs[] = {
	"IKM_KMSG_TYPE_LOCAL",
	"IKM_KMSG_TYPE_META",
	"IKM_KMSG_TYPE_NET",
	"BOGUS",
};

static char	*dipc_options_strs[] = {
	"MACH_SEND_MSG",
	"MACH_RCV_MSG",
	"SEND_INTERRUPT",
	0,

	"MACH_SEND_TIMEOUT",
	"MACH_SEND_NOTIFY",
	"MACH_SEND_INTERRUPT",
	"MACH_SEND_CANCEL",

	"MACH_RCV_TIMEOUT",
	"MACH_RCV_NOTIFY",
	"MACH_RCV_INTERRUPT",
	"MACH_RCV_LARGE",

	0,
	0,
	0,
	0,

	"MACH_SEND_ALWAYS",
	0,
	0,
	0,

	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,

	0,
	"DIPC_EMMI_REPLY_PRIV",
	"DIPC_EMMI_REPLY",
	0,
};

void
db_dipc_req_header(h, str)
	dipc_header_t	*h;
	char		*str;
{
	char	*status;

	if (h->status == 0xffff)
		status = "UNINITIALIZED";
	else if (h->status < (sizeof(dipc_status_strs) / 4))
		status = dipc_status_strs[ h->status ];
	else
		status = "BOGUS";

	iprintf("%s:\n", str);
	iprintf("status = %s destination node = %d uid = 0x%x\n",
					status, h->node, h->uid);
}

db_show_payload(p)
	dipc_request_union_t	*p;
{
	extern int	indent;

	if (p == 0)
		return 0;
	
	indent += 4;
	switch (p->nop.request.type) {
	case DIPC_REQUEST_NOP:
		db_dipc_req_header(p, "DIPC_REQUEST_NOP");
		break;

	case DIPC_REQUEST_ENQUEUE: {
		int 	i;
		int	options = p->en_queue.options;

		db_dipc_req_header(p, "DIPC_REQUEST_ENQUEUE");

		iprintf("options = 0x%x ", options);

		for (i = 0; i < (sizeof(dipc_options_strs)/4); i++)
			if ((1 << i) & options)
				db_printf("%s ", dipc_options_strs[ i ]);
		db_printf("\n");

		iprintf("RDMA token = 0x%x kmsg size = %d kmsg type = %s\n",
				p->en_queue.token, p->en_queue.kmsg_size,
				dipc_type_strs[ p->en_queue.kmsg_type ]);

		if (p->en_queue.kmsg_type == IKM_KMSG_TYPE_NET) 
			ipc_msg_print(&p->en_queue + 1);
		break;
	}

	case DIPC_REQUEST_BUY_TRANSITS:
		db_dipc_req_header(p, "DIPC_REQUEST_BUY_TRANSITS");
		iprintf("transits = %d\n", p->transit.transits);
		break;

	case DIPC_REQUEST_SELL_TRANSITS:
		db_dipc_req_header(p, "DIPC_REQUEST_SELL_TRANSITS");
		iprintf("transits = %d\n", p->transit.transits);
		break;

	case DIPC_REQUEST_MIGRATION:
		db_dipc_req_header(p, "DIPC_REQUEST_MIGRATION");
		iprintf(
"from node %d seqno=%d qlmit=%d transit=%d so=%d res_node=%d msgcnt=%d\n",
			p->migration.from,
			p->migration.state.seqno,
			p->migration.state.qlimit,
			p->migration.state.transit,
			p->migration.state.send_once,
			p->migration.state.resident_node,
			p->migration.state.msgcount);
		break;

	case DIPC_REQUEST_DEAD_NAME:
		db_dipc_req_header(p, "DIPC_REQUEST_DEAD_NAME");
		iprintf("from node %d\n", p->dead_name.from);
		break;

	case DIPC_NOTIFY_QUEUE_AVAIL:
		db_dipc_req_header(p, "DIPC_NOTIFY_QUEUE_AVAIL");
		break;

	case DIPC_NOTIFY_POLYMORPH:
		db_dipc_req_header(p, "DIPC_NOTIFY_POLYMORPH");
		iprintf("from node %d\n", p->polymorph.from);
		break;

	case DIPC_NOTIFY_DEAD_NAME:
		db_dipc_req_header(p, "DIPC_NOTIFY_DEAD_NAME");
		break;

	default:
		iprintf("Unknown DIPC request type 0x%x\n",
				p->nop.request.type);
	}
	indent -= 4;
	return 0;
}

void
db_dipc_payload(handle)
	rpc_handle_t handle;
{
	db_show_payload(rpc_arguments(handle));
}

db_sys_dipc(verbose)
	int	verbose;
{
	void	(*func)();

	if (verbose)
		func = db_dipc_payload;
	else
		func = 0;

	rpc_print_interesting_handles("Client Group",
		NORMA_RPC_GROUP_INITIATOR, FALSE, func);

	rpc_print_interesting_handles("Enqueue Group",
		NORMA_RPC_GROUP_ENQUEUE, FALSE, func);

	rpc_print_interesting_handles("Control Group",
		NORMA_RPC_GROUP_CONTROL, FALSE, func);
}

#if	MACH_ASSERT

void
db_space_ports(	ipc_space_t	space,
		ipc_port_t	port,
		task_t		task,
		int		ddb_task_num )
{
	register ipc_entry_num_t	size;
	register ipc_entry_t		table;
	register mach_port_index_t	index;
	register ipc_tree_entry_t	tentry;
	register ipc_entry_t		entry;
	register ipc_port_t		eport;
	register unsigned 		bits;
	char				*sname;
	char				spaceBuf[24];

	is_read_lock(space);

	if ( !space->is_active ) {
		is_read_unlock(space);
		return;
	}

	table = space->is_table;
	size = space->is_table_size;
	for(index=0; index < size; index++) {
		if ( (entry = &table[index]) != IE_NULL ) {
			if (entry->ie_bits & MACH_PORT_TYPE_PORT_RIGHTS) {
				eport = (ipc_port_t)entry->ie_object;
				if ( eport != IP_NULL &&
				     (eport == port || port == IP_NULL) ) {
					bits = entry->ie_bits;
					if ( space == ipc_space_kernel )
						sname = "[ipc_space_kernel]";
					else if ( space == ipc_space_remote )
						sname = "[ipc_space_remote]";
					else if ( space == ipc_space_reply )
						sname = "[ipc_space_reply]";
					else {
						sprintf(spaceBuf,"%x",space);
						sname = spaceBuf;
					}
					iprintf("$task%02d (*task %x) space %s entry %x [%s%s]\n",
						 ddb_task_num, task,sname,entry,
					 	((bits&MACH_PORT_TYPE_RECEIVE) ?
							"recv": "" ), 
					 	((bits & MACH_PORT_TYPE_SEND) ?
							" send":
						   ((bits &
						     MACH_PORT_TYPE_SEND_ONCE) ?
							" send-once" : "")));
				}
			}
		}
	}

	for (tentry = ipc_splay_traverse_start(&space->is_tree);
     	     tentry != ITE_NULL;
     	     tentry = ipc_splay_traverse_next(&space->is_tree, FALSE)) {

		entry = &tentry->ite_entry;
		if (entry->ie_bits & MACH_PORT_TYPE_PORT_RIGHTS) {
			eport = (ipc_port_t) entry->ie_object;
			if ( eport != IP_NULL &&
			     (eport == port || port == IP_NULL) ) {
				bits = entry->ie_bits;
				if ( space == ipc_space_kernel )
					sname = "[ipc_space_kernel]";
				else if ( space == ipc_space_remote )
					sname = "[ipc_space_remote]";
				else if ( space == ipc_space_reply )
					sname = "[ipc_space_reply]";
				else {
					sprintf(spaceBuf,"%x",space);
					sname = spaceBuf;
				}
				iprintf("$task%02d (*task %x) space %s entry %x [%s%s]\n",
				    ddb_task_num, task,sname,entry,
				    ((bits & MACH_PORT_TYPE_RECEIVE) ?
					"recv" : ""),
				    ((bits & MACH_PORT_TYPE_SEND) ?
							" send":
					((bits & MACH_PORT_TYPE_SEND_ONCE) ?
							" send-once" : "")));
			}
		}
	}
	ipc_splay_traverse_finish(&space->is_tree);

	is_read_unlock(space);
}

/*
 * check all task spaces and those known by the kernel to see if the specified
 * port is registered. Support "show prights port-addr". Answer the question:
 * who cares about port 'x'?.
 */

void
db_tasks_by_port(ipc_port_t port)
{
	processor_set_t	pset;
	task_t		task;
	int		ddb_task_num=0;
	extern int	indent;

	if ( !IP_VALID(port) ) {
		db_printf("invalid port %x\n",port);
		return;
	}

	db_printf("Tasks holding rights for %s %x\n",
		port->ip_pset ? "pset" : "port", port);
	indent += 3;	/* iprintf() indent by 3 spaces */

	/*
	 * Iterate over all processor sets, checking all tasks to find which
	 * tasks have right(s) for the specified port.
	 */
        queue_iterate(&all_psets, pset, processor_set_t, all_psets) {

#if 0
	  iprintf("pset %x: %d threads %d tasks %d processors\n",
			pset, pset->thread_count, pset->task_count,
			pset->processor_count);
#endif
	  /*
	   * iterate over all tasks in this processor set.
	   */
	  queue_iterate(&pset->tasks, task, task_t, pset_tasks) {

		task_reference(task);	/* make sure the task is valid */
		db_space_ports( task->itk_space, port, task, ddb_task_num );
		task_release(task);
		ddb_task_num++;
	   }
	}

	/*
	 * Check those IPC spaces associated with the kernel task
	 */
	db_space_ports( ipc_space_kernel, port, kernel_task, 0 );
	db_space_ports( ipc_space_remote, port, kernel_task, 0 );
	db_space_ports( ipc_space_reply, port, kernel_task, 0 );

	indent -= 3;
}
#endif	/* MACH_ASSERT */
