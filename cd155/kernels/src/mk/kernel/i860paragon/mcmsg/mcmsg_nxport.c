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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_nxport.c,v 1.18 1994/11/18 20:44:29 mtm Exp $
 */

/*
 * mcmsg_port.c
 *
 * NX Kernel ports' functions.
 *
 *	The two NX kernel ports are a mechanism by which the kernel can
 *	inform the associated user task of:
 *
 *		- completion of hsend/hrecv
 *		- a non-resident (bad) page in a user message buffer (on a send)
 *		- a buffered received xmsg which may be copied to the user's buffer and
 *		  provoke the sender to send more data if the message is not complete.
 *
 */

#define MCMSG_NXPORT

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#include <ipc/ipc_space.h>
#include <kern/ast.h>

extern mcmsg_task_t	*mcmsg_user_mcmsg_task[];
extern int		mcmsg_user_post_tasks;

#define ALLOC_AST_SLOT(reqp) \
	reqp = &(mt->nxport_req_list[mt->nxport_req_in]); \
	mt->nxport_req_in = ++mt->nxport_req_in & (MAX_NXPORT_REQS-1)

/*
 *	Routine:
 *		syscall_mcmsg_nxport_setup [kernel trap]
 *
 *	Purpose:
 *		Attach the given NX special port to the callers task. The
 *		first call sets the hrecv port, the second call sets the vm
 *		paging and receive continue port.
 */
syscall_mcmsg_nxport_setup(portname)
mach_port_t portname;
{
	register int x;
	task_t	task;

	x = spldcm();

	/* Insert nx portname into calling task */

	task = current_task();
	assert (task->mcmsg_task != 0);
	if (task->mcmsg_task->nxport[0] == 0)
		/* hrecv port */
		task->mcmsg_task->nxport[0] = portname;
	else
		/* vm paging and receive continue port */
		task->mcmsg_task->nxport[1] = portname;

	splx(x);
	return KERN_SUCCESS;
}

/*
 *	Routine:
 *		mcmsg_send_nxport
 *
 *	Purpose:
 *		Send a message to one of the NX Special Ports.
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
	int slot;

	assert(task != 0);
	assert(task->mcmsg_task != 0);

	/*
	 * Extract nxport from task - determined by operation.
	 */
	switch(req->op) {
		case NXPORT_OP_MISSING_PAGE:
		case NXPORT_OP_RECV_CONTINUE:
			slot = 1;
			break;
		case NXPORT_OP_INVOKE_HANDLER:
		default:
			slot = 0;
			break;
	}
	destport = task->mcmsg_task->nxport[slot];
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
 *		mcmsg_pack_ast_buffer
 *
 *	Purpose:
 *		Pack any requests not delivered into contiguous entries.  The
 *		entries are packed up to the entry prior to where
 *		nxport_req_out now sits, which now points to the location
 *		where the next AST request will be placed during the door-bell
 *      interrupt processing.
 *		When we're done the nxport_req_out ring pointer will be set
 *		back to the oldest entry in the ring buffer.  The order of
 *		the entries, oldest to most recent, is preserved.
 *
 */

mcmsg_pack_ast_buffer(nxport_req_list, first_entry, last_entry, list_length)
register mcmsg_nxport_req_t *nxport_req_list;
register int first_entry;
register int last_entry;
register int list_length;
{
	register int i;

	while (last_entry != first_entry) {
		while ((nxport_req_list[last_entry].task != 0) &&
				(last_entry != first_entry)) {
			last_entry = --last_entry & (list_length-1);
		}
		if (last_entry == first_entry)
			break;
		i = (last_entry-1) & (list_length-1);
		while (nxport_req_list[i].task == 0) {
			i = --i & (list_length-1);
		}
		nxport_req_list[last_entry] = nxport_req_list[i];
		nxport_req_list[i].task = 0;
		if (i == first_entry)
			break;
	}
	return last_entry;		/* Return the new nxport_req_out value */
}

/*
 *	Routine:
 *		mcmsg_ast
 *
 *	Purpose:
 *		Called by ast_taken() to send request to user via one of the
 *		NX kernel ports.
 *
 *	Returns:
 *		None.
 *
 */
mcmsg_ast()
{
	register task_t task;
	register mcmsg_task_t *mt;
	register mcmsg_nxport_req_t *nxport_req_list;
	register int first_entry, last_entry;
	register int i;
	unsigned long	op;

	for (i=0; i<mcmsg_user_post_tasks; i++) {
		mt = mcmsg_user_mcmsg_task[i];
		nxport_req_list = mt->nxport_req_list;

		/*
		 * Send the requests. 
		 * If a task associated with a HANDLER request has
		 * masktrap set, save the request instead of sending it.
		 */

		first_entry = -1;
		while ((task = (task_t)nxport_req_list[mt->nxport_req_out].task) != 0) {
			op   = nxport_req_list[mt->nxport_req_out].op;

			if (op == NXPORT_OP_INVOKE_HANDLER &&
			    task->mcmsg_task->masktrap) {
				if (first_entry == -1) {
					first_entry = mt->nxport_req_out;
				}
			} else {
				mcmsg_send_nxport(task, &nxport_req_list[mt->nxport_req_out]);
				nxport_req_list[mt->nxport_req_out].task = 0;
			}
			last_entry = mt->nxport_req_out;
			mt->nxport_req_out = ++mt->nxport_req_out & (MAX_NXPORT_REQS-1);
		}

		if (first_entry != -1) {
			mt->nxport_req_out = mcmsg_pack_ast_buffer(nxport_req_list,
									first_entry, last_entry, MAX_NXPORT_REQS);
		}
	}
}

/*
 *	Routine:
 *		mcmsg_hreq_ast
 *
 *	Purpose:
 *		Called by message processor thread when
 *		hsend/hrecv request has completed. It:
 *
 *		  1) fills in the current request.  Task field must be set last.
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
	register mcmsg_nxport_req_t	*reqp;
	register mcmsg_task_t	*mt = task->mcmsg_task;

	ALLOC_AST_SLOT(reqp);
#if MACH_ASSERT
	if (reqp->task != 0) {
		mcmsg_trace_drop("hreq AST task", task);
		return -1;
	}
#endif MACH_ASSERT

	reqp->op   = NXPORT_OP_INVOKE_HANDLER;
	reqp->nxreq = nxreq;
	reqp->task = (unsigned long)task;

	/*
	 * Message processor must set AST for other cpu, then wake up other cpu.
	 * If no message processor, just set AST and go on.
	 */

	if (mcmsg_mp_enable) {
		mp_ast(0);
	} else {
		ast_on(cpu_number(), AST_MCMSG);
	}
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
 *		  1) fills in the current request. Task field must be set last.
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
	register mcmsg_nxport_req_t	*reqp;
	register mcmsg_task_t	*mt = task->mcmsg_task;

	ALLOC_AST_SLOT(reqp);
#if MACH_ASSERT
	if (reqp->task != 0) {
		mcmsg_trace_drop("vm AST task", task);
		return -1;
	}
#endif MACH_ASSERT

	reqp->op   = NXPORT_OP_MISSING_PAGE;
	reqp->nxreq = nxreq;
	reqp->fill1 = send_item;	/* XXX rename fill1 */
	reqp->task = (unsigned long)task;

	/*
	 * Message processor must set AST for other cpu, then wake up other cpu.
	 * If no message processor, just set AST and go on.
	 */

	if (mcmsg_mp_enable) {
		mp_ast(0);
	} else {
		ast_on(cpu_number(), AST_MCMSG);
	}
	return;
}


/*
 *	Routine:
 *		mcmsg_recv_cont_ast
 *
 *	Purpose:
 *		Called by message processor thread when an xmsg buffer can be
 *		copied to a user's buffer and receive continue is performed if
 *		the message is not complete.
 *
 *		  1) fills in the current request. Task field must be set last.
 *		  2) sets the MCMSG ast (AST_MCMSG)
 *
 *	Returns:
 *		None.
 *
 */


mcmsg_recv_cont_ast (task, nxreq)
task_t  task;
nxreq_t *nxreq;
{
	register mcmsg_nxport_req_t	*reqp;
	register mcmsg_task_t	*mt = task->mcmsg_task;

	ALLOC_AST_SLOT(reqp);
#if MACH_ASSERT
	if (reqp->task != 0) {
		mcmsg_trace_drop("recv cont AST task", task);
		return -1;
	}
#endif MACH_ASSERT

	reqp->op   = NXPORT_OP_RECV_CONTINUE;
	reqp->nxreq = nxreq;
	reqp->task = (unsigned long)task;

	/*
	 * Message processor must set AST for other cpu, then wake up other cpu.
	 * If no message processor, just set AST and go on.
	 */

	if (mcmsg_mp_enable) {
		mp_ast(0);
	} else {
		ast_on(cpu_number(), AST_MCMSG);
	}
	return;
}

