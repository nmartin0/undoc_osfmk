/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_nxport.c,v 1.13 1994/11/18 20:41:48 mtm Exp $
 */

/*
 * mcmsg_port.c
 *
 * NX Kernel port functions.
 *
 *	The NX kernel port is a mechanism by which the kernel can
 *	inform the associated user task of:
 *
 *		- completion of hsend/hrecv
 *		- a non-resident (bad) page in a user message buffer
 *
 */

#define MCMSG_NXPORT

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_nx.h>
#include <ipc/ipc_space.h>
#include <kern/ast.h>


/*
 * The current nxport request.
 */
#define MAX_NXPORT_REQS	128

int					nxport_req_index = 0;				/* current req index */
mcmsg_nxport_req_t	nxport_req_list[MAX_NXPORT_REQS];	/* list of requests */

/*
 *	Routine:
 *		syscall_mcmsg_nxport_setup [kernel trap]
 *
 *	Purpose:
 *		Attach the NX special port to the callers task.
 */
syscall_mcmsg_nxport_setup(portname)
mach_port_t portname;
{
	register int x;
	task_t	task;
	mcmsg_nxport_req_t req;

	mcmsg_trace_debug("nxport setup", 1, portname, 0, 0, 0);

	x = spldcm();

	/* Insert nx portname into calling task */

	task = current_task();
	assert (task->mcmsg_task != 0);
	task->mcmsg_task->nxport = portname;

	splx(x);
	return KERN_SUCCESS;
}

/*
 *	Routine:
 *		mcmsg_send_nxport
 *
 *	Purpose:
 *		Send a message to the NX Special Port.
 */
mcmsg_send_nxport(task, req)
task_t		task;
mcmsg_nxport_req_t *req;
{
	struct {
		mach_msg_header_t hdr;
		mach_msg_type_t   type;
		mcmsg_nxport_req_t req;
	} msg;
	mach_port_t	destport;
	mach_msg_return_t result;
	mach_msg_option_t option;
	mach_msg_timeout_t timeout;

	mcmsg_trace_debug("nxport send", 2, task, req, 0, 0);

	assert(task != 0);
	assert(task->mcmsg_task != 0);

	/*
	 * Extract nxport from task.
	 */
	destport = task->mcmsg_task->nxport;
	assert(MACH_PORT_VALID(destport));

	/*
	 * fill in message
	 */
	msg.hdr.msgh_size = sizeof(msg);
	msg.hdr.msgh_remote_port = destport;
	msg.hdr.msgh_local_port = MACH_PORT_NULL;
	msg.hdr.msgh_id = 0;
	msg.hdr.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);

	msg.type.msgt_name     = MACH_MSG_TYPE_INTEGER_32;
	msg.type.msgt_size     = 32;
	msg.type.msgt_number   = sizeof(mcmsg_nxport_req_t) / sizeof(long);
	msg.type.msgt_inline   = 1;
	msg.type.msgt_longform = 0;
	msg.type.msgt_deallocate = 0;
	msg.type.msgt_unused   = 0;

	msg.req = *req;

	/*
	 * Send.
	 */

	timeout = 0;
	option = MACH_SEND_MSG | MACH_SEND_ALWAYS;
	result = mcmsg_mach_msg_send(task, &msg, sizeof(msg), option);

	assert (result == MACH_MSG_SUCCESS);
	return KERN_SUCCESS;
}


/*
 *	Routine:
 *		mcmsg_mach_msg_send
 *
 *	Purpose:
 *		Just like mach_msg() except:
 *
 *			1) Only does send.
 *			2) Handles send Options.
 *			3) Deals with arbitrary task.
 *
 *	Returns:
 *		
 */
mach_msg_return_t
mcmsg_mach_msg_send(task, msg, send_size, option)
	task_t task;
	mach_msg_header_t *msg;
	mach_msg_option_t option;
	mach_msg_size_t send_size;
{
	ipc_space_t space = task->itk_space;
	vm_map_t map = task->map;
	ipc_kmsg_t kmsg;
	mach_port_seqno_t seqno;
	mach_msg_return_t mr;


	mr = ipc_kmsg_get_from_kernel(msg, send_size, &kmsg);
	if (mr != MACH_MSG_SUCCESS) {
		panic("mcmsg_mach_msg_send");
	}

	mr = ipc_kmsg_copyin(kmsg, space, map, MACH_PORT_NULL);
	if (mr != MACH_MSG_SUCCESS) {
		ikm_free(kmsg);
		return mr;
	}

	do {
		mr = ipc_mqueue_send(kmsg, option, MACH_MSG_TIMEOUT_NONE);
	} while (mr == MACH_SEND_INTERRUPTED);

	return mr;
}

/*
 *	Routine:
 *		mcmsg_ast
 *
 *	Purpose:
 *		Called by ast_taken() to send request to user via the
 *		NX kernel port.
 *
 *	Returns:
 *		None.
 *
 */
mcmsg_ast()
{
	register int i, x;
	register int save_index;
	task_t task;
	unsigned long	op;

	/* no message interrupts */
	x = spldcm();

	mcmsg_trace_debug("mcmsg_ast", 1, nxport_req_index, 0, 0, 0);

	/*
	 * Send the requests. 
	 * If a task associated with a HANDLER request has
	 * masktrap set, save the request instead of sending it.
	 */
	save_index = 0;
	for (i=0; i<nxport_req_index; i++) {

		task = (task_t)nxport_req_list[i].task;
		op   = nxport_req_list[i].op;

		if (op == NXPORT_OP_INVOKE_HANDLER &&
		    task->mcmsg_task->masktrap) {
			if (i != save_index) {
				nxport_req_list[save_index] = nxport_req_list[i];
			}
			save_index++;
		} else {
			mcmsg_send_nxport(task, &nxport_req_list[i]);
		}
	}

	/* reset the index */
	nxport_req_index = save_index;

	/* Turn the ast OFF if no more requests */
	if (nxport_req_index == 0) {
		ast_off(cpu_number(), AST_MCMSG);
	}

	mcmsg_trace_debug("mcmsg_ast done", 1, nxport_req_index, 0, 0, 0);

	/* restore spl */
	splx(x);
}

/*
 *	Routine:
 *		mcmsg_hreq_ast
 *
 *	Purpose:
 *		Called by message processor thread when
 *		hsend/hrecv request has completed. It:
 *
 *		  1) fills in the current request.
 *		  2) sets the MCMSG ast (AST_MCMSG)
 *
 *	Returns:
 *		None.
 *
 */

mcmsg_hreq_ast (task, nxreq)
task_t  task;
nxreq_t *nxreq;
{
	mcmsg_nxport_req_t	*reqp;

	reqp = &nxport_req_list[nxport_req_index++];
	assert (nxport_req_index < MAX_NXPORT_REQS);

	reqp->task = (unsigned long)task;
	reqp->op   = NXPORT_OP_INVOKE_HANDLER;
	reqp->nxreq = nxreq;

	ast_on(cpu_number(), AST_MCMSG);
	return;
}

/*
 *	Routine:
 *		mcmsg_vm_ast
 *
 *	Purpose:
 *		Called by message processor thread when it 
 *		has a vm request to send to the user process. It:
 *
 *		  1) fills in the current request.
 *		  2) sets the MCMSG ast (AST_MCMSG)
 *
 *	Returns:
 *		None.
 *
 */

mcmsg_vm_ast (task, nxreq, send_item)
task_t  task;
nxreq_t *nxreq;
unsigned long send_item;	/* Actually pointer to send select item */
{
	mcmsg_nxport_req_t	*reqp;

	reqp = &nxport_req_list[nxport_req_index++];
	assert (nxport_req_index < MAX_NXPORT_REQS);

	reqp->task = (unsigned long)task;
	reqp->op   = NXPORT_OP_MISSING_PAGE;
	reqp->nxreq = nxreq;
	reqp->fill1 = send_item;	/* XXX rename fill1 */

	ast_on(cpu_number(), AST_MCMSG);
	return;
}


