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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_inq.c,v 1.8 1994/11/18 20:41:30 mtm Exp $
 */

/*
 * mcmsg_inq.c
 *
 * Find other processes in an application
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>
#include <i860ipsc/mcmsg/mcmsg_nx.h>

mcmsg_inquire(si)
	select_item_t	*si;
{
	select_item_t	*sh;
	select_item_t	*st;

	si->method = SENDMETH_PNQ;
	st = mcmsg_task->send_wait_unk;
	mcmsg_task->send_wait_unk = si;
	if (st == 0) {
		si->link = si;
		mcmsg_send(SENDMETH_PNQ, si, 0);
	} else {
		sh = st->link;
		assert(sh != 0 && sh->method != 0xdead);
		si->link = sh;
		st->link = si;
	}
}

#define PNABUF_SIZE 600

struct pnabuf {
	unsigned long	physnode;
	long		source_pid;
} mcmsg_pnabuf[PNABUF_SIZE];

unsigned long mcmsg_pnabuf_in;
unsigned long mcmsg_pnabuf_out;

mcmsg_delay_pna(physnode, source_pid)
	register unsigned long	physnode;
	register long		source_pid;
{
	register unsigned long	new_in;

	new_in = mcmsg_pnabuf_in + 1;
	if (new_in == PNABUF_SIZE) {
		new_in = 0;
	}
	assert(new_in != mcmsg_pnabuf_out);
	mcmsg_pnabuf[mcmsg_pnabuf_in].physnode = physnode;
	mcmsg_pnabuf[mcmsg_pnabuf_in].source_pid = source_pid;
	mcmsg_pnabuf_in = new_in;
}

mcmsg_flush_pna()
{

	mcmsg_send(SENDMETH_PNA, 0, 0);
}

mcmsg_send_pnq(si, dummy)
	select_item_t	*si;
	register unsigned long	dummy;
{
	register unsigned long	node;
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	route;
	extern int ipsc_physnode;

	node = si->nxrq.dest_node;
	hdr1 = MCTRL_PNQ | (ipsc_physnode << 16);
	hdr2 = 24;
	hdr3 = mcmsg_task->pid;
	route = calculate_route(
		 mcmsg_task->task_ptype_list->phys_node_list[node]);
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2, node, si);
	assert(si->mcmsg_task == mcmsg_task);
	send2(route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, mcmsg_task->node);
	send2((si->nxrq.source_ptype & ~GLOBAL_BIT), mcmsg_task->applinfo.app);
	send2eod(node, si->nxrq.dest_ptype);
mcmsg_trace_debug("pnq method", 1, si->method, 0, 0, 0);
	return 0;
}

mcmsg_send_pna(dummy1, dummy2)
	register dummy1;
	register dummy2;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	new_out;

	new_out = mcmsg_pnabuf_out;
	assert(new_out < PNABUF_SIZE);
	assert(mcmsg_pnabuf_in < PNABUF_SIZE);
	if (new_out != mcmsg_pnabuf_in) {
		hdr1 = MCTRL_PNA;
		hdr2 = 8;
		hdr3 = 0;
		mcmsg_trace_send(hdr1, hdr2, hdr3, 2,
				 mcmsg_pnabuf[new_out].physnode,
				 mcmsg_pnabuf[new_out].source_pid);
		send2(calculate_route(mcmsg_pnabuf[new_out].physnode), 0);
		send2(hdr1, hdr2);
		send2eod(hdr3, mcmsg_pnabuf[new_out].source_pid);
		new_out++;
		if (new_out == PNABUF_SIZE) {
			new_out = 0;
		}
		mcmsg_pnabuf_out = new_out;
		if (new_out != mcmsg_pnabuf_in) {
			return SENDMETH_PNA;
		}
	}
	return 0;
}

mcmsg_send_pak(pid_si, ptype)
	register select_item_t	*pid_si;
	register long		ptype;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	avail;
	register mcmsg_task_t	*mt;

	mt = pid_si->mcmsg_task;
	if (pid_si->ppid.recv_target == 0) {
		avail = mt->applinfo.memory_each;
		pid_si->ppid.recv_target = avail;
		mt->assign_target += avail;
		if (avail > mt->provided - mt->assigned) {
			avail = mt->provided - mt->assigned;
		}
		mt->assigned += avail;
		pid_si->ppid.recv_total = avail;
	} else {
		avail = pid_si->ppid.recv_give;
		pid_si->ppid.recv_give = 0;
	}

	if (mcmsg_task->applinfo.process_lock) {
		hdr1 = MCTRL_PAKL | (avail << 16);
	} else {
		hdr1 = MCTRL_PAK | (avail << 16);
	}
	hdr2 = 16;
	hdr3 = pid_si->mcmsg_task->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2,
		pid_si->ppid.node, pid_si->ppid.recv_total);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, pid_si->value);
	send2eod(pid_si->mcmsg_task->node, ptype);
	return 0;
}

mcmsg_recv_pnq(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	dest_app;
	register long 		dest_node;
	register long 		dest_ptype;
	register unsigned long	source_node;
	register long 		source_ptype;
	register long 		msg_type;
	register unsigned long	msg_length;
	register unsigned long	msg_size;
	register mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register int		status;

	recv2(source_pid, source_node);
	recv2(source_ptype, dest_app);
	recv2(dest_node, dest_ptype);

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, msg_length, msg_type);

	status = mcmsg_find_local(dest_app, dest_node, dest_ptype);
	if (status != 0) {
		/* mcmsg_find_local already called mcmsg_trace_drop */
		if (status == 1) {
			mcmsg_delay_pna(hdr1 >> 16, source_pid);
		}
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

	pid_si = mcmsg_install_remote(source_pid, source_node, source_ptype);
	assert(pid_si != 0);

	assert(mt == pid_si->mcmsg_task); /* XXX is this always true? */
	if (!pid_si->ppid.sent_pak) {
		pid_si->ppid.sent_pak = 1;
		mcmsg_send(SENDMETH_PAK, pid_si, dest_ptype);
	}
}

mcmsg_recv_pna(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register long		dummy;
	register unsigned long	dest_pid;
	register mcmsg_task_t	*mt;
	register select_item_t	*st;
	register select_item_t	*sh;
	register select_item_t	*sn;
	register unsigned long	t;

	recv2(dummy, dest_pid);

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, dummy, 0, 0, 0);
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

	mcmsg_trace_recv(hdr1, hdr2, dummy, 1, mcmsg_task, 0);

	st = mt->send_wait_unk;
	if (st == 0) {
		mcmsg_trace_drop("no unk send waiting", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	sh = st->link;
	assert(sh != 0);
	sh->method = SENDMETH_PNQ;
	mcmsg_send(SENDMETH_PNQ, sh, 0);
}

mcmsg_recv_pak(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register long		source_pid;
	register unsigned long	dest_pid;
	register unsigned long	source_node;
	register long		source_ptype;
	register mcmsg_task_t	*mt;
	register select_item_t	*ppid_si;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register select_item_t	*sn;
	register select_item_t	*st;
	register long		inq;
	register unsigned long	t;

	recv2(source_pid, dest_pid);
	recv2(source_node, source_ptype);

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("dest pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

#if	MACH_ASSERT
	ppid_si = mcmsg_lookup_remote(source_pid);
#endif	MACH_ASSERT
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, mcmsg_task, ppid_si);

	pid_si = mcmsg_install_remote(source_pid, source_node, source_ptype);
	assert(pid_si != 0);
	assert(ppid_si == 0 || ppid_si == pid_si);

	if ((hdr1 & 0xFFFF) == MCTRL_PAKL) {
		pid_si->ppid.process_lock = 1;
	}

	assert(pid_si->ppid.send_avail == 0);
	pid_si->ppid.send_ready = 1;
	pid_si->ppid.send_avail = hdr1 >> 16;

	st = mt->send_wait_unk;
	if (st != 0) {
		sn = st->link;
		mcmsg_trace_debug("pak unk", 1, sn, 0, 0, 0);

		inq = sn->nxrq.dest_node == source_node &&
		      sn->nxrq.dest_ptype == source_ptype;
		if (!pid_si->ppid.sent_pak) {
			assert(inq);
			pid_si->ppid.sent_pak = 1;
			mcmsg_send(SENDMETH_PAK, pid_si, sn->nxrq.source_ptype);
		}

		assert((t = MAXLOOP) != 0);
		while (sn != 0) {
			register long pid;

			pid = mcmsg_remote_pid(sn->nxrq.dest_node,
						 sn->nxrq.dest_ptype);
			if (pid == -1) {
				break;
			}
			pid_si = mcmsg_lookup_remote(pid);
			assert(pid_si != 0);
			sn->nxrq.pid_si = pid_si;
			if (!pid_si->ppid.send_ready) {
				break;
			}

			si = sn;
			if (sn == st) {
				mt->send_wait_unk = sn = 0;
			} else {
				if (mt->send_wait_unk == sn) {
					mt->send_wait_unk = st;
				}
				st->link = sn = sn->link;
			}
mcmsg_trace_debug("pak scan", 4, si, sn, st, mt->send_wait_unk);
			assert(si->method == SENDMETH_PNQ);
			mcmsg_schedule_send(si);
			assert(t-- != 0);
		}
		if (inq && sn != 0 && sn->method != 0) {
			mcmsg_send(sn->method, sn, sn->value);
		}
	} else {
		assert(ppid_si != 0);
	}
}

/*
 *	Routine:
 *		mcmsg_send_prm
 *
 *	Purpose:
 *		Inform remote process that this process is terminating.
 *
 *	Returns:
 *		None.
 */
mcmsg_send_prm(pid_si, dummy)
	register select_item_t *pid_si;
	register dummy;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	route;

	route = pid_si->ppid.route;
	hdr1 = MCTRL_PRM;
	hdr2 = 8;
	hdr3 = mcmsg_task->pid;	/* src pid */
	mcmsg_trace_send(hdr1, hdr2, hdr3, 1, pid_si->value, 0);

	send2(route, 0);
	send2(hdr1, hdr2);
	send2eod(hdr3, pid_si->value);

	return 0;
}

/*
 *	Routine:
 *		mcmsg_recv_prm
 *
 *	Purpose:
 *		Release resources for remote pid.
 *
 *	Returns:
 *		None.
 */
mcmsg_recv_prm(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long source_pid, dest_pid;
	register select_item_t *pid_si;
	register mcmsg_task_t	*mt;
	register unsigned long rtotal;
	register ptype_list_t	*node_ptype_list;
	register node_list_t	*node_list;
	register int	i, remote_node, pid_index;

	recv2(source_pid, dest_pid);

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		/*
		 * The destination pid does not exist
		 */
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		return;
	}
	
	/* 
	 * lookup the remote pid
	 */
	pid_si = mcmsg_lookup_remote(source_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, 
		pid_si, pid_si ? pid_si->ppid.recv_total : 0);

	if (pid_si != 0) {
		/*
		 * release message buffer resource
		 */
		mt = pid_si->mcmsg_task;
		remote_node = pid_si->ppid.node;
		assert(mt != 0);
		rtotal = pid_si->ppid.recv_total;
		mt->assign_target -= rtotal;
		mt->assigned -= rtotal;

		/*
		 * Detach and free the item representing the remote pid.
		 */
		mcmsg_selector_remove(mt->pid_sel, source_pid);

		/*
		 * Remove the pid from the ptype list[s].
		 */
		node_ptype_list = mt->task_ptype_list->node_ptype_list;
		for (i=0; i<node_ptype_list->last_entry; i++) {
			node_list = 
		    	(node_list_t *)(node_ptype_list->ptype_entry[i].item);
			if (node_list != 0) {
				if (node_list[remote_node] == source_pid) {
					node_list[remote_node] = -1;
				}
			}
		}
	}
	return;
}

mcmsg_remote_pid(node, ptype)
	register unsigned long	node;
	register unsigned long	ptype;
{
	register ptype_list_t	*node_ptype_list;
	register int		i;
	register node_list_t	*node_list;

	if (node > mcmsg_task->numnodes) {
		return -1;
	}
	node_ptype_list = mcmsg_task->task_ptype_list->node_ptype_list;
	i = mcmsg_ptype_select(node_ptype_list, ptype);
	if (i < 0) {
		return -1;
	}
	node_list = (node_list_t *)(node_ptype_list->ptype_entry[i].item);
	if (node_list == 0) {
		return -1;
	}
	return node_list[node];
}

mcmsg_find_local(app, node, ptype)
	register unsigned long	app;
	register unsigned long	node;
	register unsigned long	ptype;
{
	register ptype_list_t	*task_ptype_list;
	register mcmsg_task_t	*mt;
	register int		i;

	task_ptype_list = (ptype_list_t *)
			mcmsg_selector_lookup(&mcmsg_app_sel, app);
	if (task_ptype_list == 0) {
		mcmsg_trace_debug("dest app not found", 3, app, node, ptype, 0);
		return 1;
	}

	i = mcmsg_ptype_select(task_ptype_list, ptype);
	if (i < 0 ||
	 (mt = (mcmsg_task_t *)(task_ptype_list->ptype_entry[i].item)) == 0) {
		mcmsg_trace_debug("dest ptype not found", 3,
				  app, node, ptype, 0);
		return 1;
	}

	if (mt->node != node) {
		mcmsg_trace_drop("wrong node", node);
		mcmsg_msg_drop++;
		return -1;
	}
	mcmsg_task = mt;
	return 0;
}

select_item_t *
mcmsg_install_remote(pid, node, ptype)
	register long		pid;
	register unsigned long	node;
	register long		ptype;
{
	register select_item_t	*si;
	register int		i;
	register ptype_list_t	*node_ptype_list;
	register node_list_t	*node_list;

	si = (select_item_t *)mcmsg_selector_lookup_si(mcmsg_task->pid_sel,
						       pid);
	if (si == 0) {
		assert(node <= mcmsg_task->numnodes);

		node_ptype_list = mcmsg_task->task_ptype_list->node_ptype_list;
		i = mcmsg_ptype_select(node_ptype_list, ptype);
		if (i < 0 ||
		 (node_list = (node_list_t *)
			      (node_ptype_list->ptype_entry[i].item)) == 0) {
			node_list = (node_list_t *)mcmsg_l2malloc(
					l2size((node_ptype_list->numnodes+1)*
					       sizeof(unsigned long)));
			assert(node_list != 0);
			for (i = 0; i <= node_ptype_list->numnodes; i++) {
				node_list[i] = -1;
			}
			i = mcmsg_ptype_add(node_ptype_list,
					    ptype,
					    (void *)node_list,
					    0);
			assert(i != -1);
		}
		node_list[node] = pid;

		si = mcmsg_alloc_select_item();
		assert(si != 0);
		mcmsg_trace_debug("install pid", 4,
				  pid, si, mcmsg_task, mcmsg_task->pid_sel);
		si->ppid.node = node;
		si->ppid.process_lock = 0;
		si->ppid.send_ready = 0;
		si->ppid.sent_pak = 0;
		si->ppid.route = calculate_route(
			mcmsg_task->task_ptype_list->phys_node_list[node]);
		si->ppid.send_avail = 0;
		si->ppid.recv_give = 0;
		si->ppid.recv_total = 0;
		si->ppid.recv_target = 0;
		si->ppid.recv_taken = 0;
		si->ppid.send_wait = 0;
		si->ppid.rk_recv_want = 0;
		si->ppid.rk_recv_seq = 0;
		si->ppid.rk_recv_pid = 0;
		si->ppid.rk_recv_type = 0;
		si->ppid.rk_recv_ptype = 0;
		si->ppid.rk_recv_link = 0;
		si->ppid.avail_link = 0;
		si->ppid.retire_seq_in = 0;
		si->ppid.retire_seq_out = 0;
		mcmsg_selector_install_si(mcmsg_task->pid_sel,
					  si,
					  pid,
					  SELMETH_PID);
	}
	assert(	si == 0 ||
		si->ppid.avail_link == 0 ||
		mcmsg_task->avail_need != 0);
	return si;
}

select_item_t *
mcmsg_lookup_remote(pid)
	register long		pid;
{
	register select_item_t	*si;

	si = (select_item_t *)mcmsg_selector_lookup_si(mcmsg_task->pid_sel,
						       pid);
	assert(	si == 0 ||
		si->ppid.avail_link == 0 ||
		mcmsg_task->avail_need != 0);
	return si;
}

