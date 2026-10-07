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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_inq.c,v 1.30 1995/01/30 21:42:54 joel Exp $
 */

/*
 * mcmsg_inq.c
 *
 * Find other processes in an application
 */

#define	MCMSG_MODULE	MCMSG_MODULE_NX
#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>

mcmsg_task_t *mcmsg_find_local();

mcmsg_inquire(mt, si)
	mcmsg_task_t	*mt;
	select_item_t	*si;
{
	select_item_t	*sh;
	select_item_t	*st;
	int i;
	mcmsg_task_t *inq_mt;
	select_item_t	*pid_si;

	si->method = MCTRL_PNQ;
	mcmsg_trace_debug("inquire si mtype dnode", 3, 
		si, si->nxrq.msg_type,si->nxrq.dest_node,0);

	/* look for matching local process to short circuit inquire protocol */
	if (mt->node == si->nxrq.dest_node) {
		i = mcmsg_ptype_select(mt->task_ptype_list,si->nxrq.dest_ptype);
	} else { 
		i = -1;
	}
	if (i < 0 || 
	    ((inq_mt = (mcmsg_task_t *)(mt->task_ptype_list->ptype_entry[i].item)) == 0) ||
            (inq_mt != mt)){
		/* local proc not found send inquiry */
		st = mt->send_wait_unk;
		mt->send_wait_unk = si;
		if (st == 0) {
			si->link = si;
			mcmsg_send(mt, MCTRL_PNQ, si, 0);
		} else {
			sh = st->link;
			assert(sh != 0 && sh->method != 0xdead);
			si->link = sh;
			st->link = si;
		}
		return;
	} else {
		/* found local match, install its pid */
		pid_si = mcmsg_install_remote(mt,inq_mt->pid, 
				inq_mt->node, si->nxrq.dest_ptype);
		assert(pid_si != 0);
		si->nxrq.pid_si = pid_si;
		mcmsg_trace_debug("inquire found local proc ",2,si,pid_si,0,0);
		mcmsg_inq_switch[si->value](mt, si);
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

mcmsg_flush_pna(mt)
	mcmsg_task_t	*mt;
{

	mcmsg_send(mt, MCTRL_PNA, 0, 0);
}

mcmsg_inq_null()
{

	assert(0);
}

mcmsg_send_pnq(mt, ctl, si)
	mcmsg_task_t	*mt;
	select_item_t	*si;
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
	hdr3 = mt->pid;
	route = calculate_route(
		 mt->task_ptype_list->phys_node_list[node]);
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2, node, si);
	assert(si->mcmsg_task == mt);
	send2(route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, mt->node);
	send2((si->nxrq.source_ptype & ~GLOBAL_BIT), mt->applinfo.app);
	send2eod(node, si->nxrq.dest_ptype);
	mcmsg_trace_debug("send_pnq si ptype", 2, si, si->nxrq.dest_ptype,0,0);
	return;
}

mcmsg_send_pna(mt, ctl)
	mcmsg_task_t	*mt;
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
			mcmsg_send_tail(mt, MCTRL_PNA);
			return;
		}
	}
	return;
}

mcmsg_send_pak(mt, ctl, pid_si, ptype)
	register mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register long		ptype;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	avail;

	assert(mt == pid_si->mcmsg_task);

	if (mt->applinfo.process_lock) {
		hdr1 = MCTRL_PAKL;
	} else {
		hdr1 = MCTRL_PAK;
	}
	hdr2 = 16;
	hdr3 = mt->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2,
		pid_si->ppid.node, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, pid_si->value);
	send2eod(mt->node, ptype);
	return;
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

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, source_node, source_ptype);

	mt = mcmsg_find_local(dest_app, dest_node, dest_ptype, hdr1 >> 16, source_pid);
	if (mt == 0) {
		return;
	}

	pid_si = mcmsg_install_remote(mt,source_pid, source_node, source_ptype);
	assert(pid_si != 0);

	assert(mt == pid_si->mcmsg_task); /* XXX is this always true? */
	if (!pid_si->ppid.sent_pak) {
		pid_si->ppid.sent_pak = 1;
		mcmsg_send(mt, MCTRL_PAK, pid_si, dest_ptype);
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

	if ((st = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, dummy, 0, 0, 0);
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mt = st->mcmsg_task;

	mcmsg_trace_recv(hdr1, hdr2, dummy, 1, mt, 0);

	st = mt->send_wait_unk;
	if (st == 0) {
		mcmsg_trace_drop("no unk send waiting", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	sh = st->link;
	assert(sh != 0);
	sh->method = MCTRL_PNQ;
	mcmsg_send(mt, MCTRL_PNQ, sh, 0);
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

	if ((st = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("dest pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mt = st->mcmsg_task;

#if	MACH_ASSERT
	ppid_si = mcmsg_lookup_remote(mt, source_pid);
#endif	MACH_ASSERT
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, source_node, ppid_si);

	pid_si = mcmsg_install_remote(mt, source_pid, source_node, source_ptype);
	assert(pid_si != 0);
	assert(ppid_si == 0 || ppid_si == pid_si);

	pid_si->ppid.recvd_pak = 1;
	if ((hdr1 & 0xFFFF) == MCTRL_PAKL) {
		pid_si->ppid.process_lock = 1;
	}

	st = si = mt->send_wait_unk;
	if (st != 0) {
		sn = si->link;
		mcmsg_trace_debug("pak unk", 1, sn, 0, 0, 0);

		inq = sn->nxrq.dest_node == source_node &&
		      sn->nxrq.dest_ptype == source_ptype;
                if ((!pid_si->ppid.sent_pak) && (inq)) {
			pid_si->ppid.sent_pak = 1;
			mcmsg_trace_debug("reply pak", 1, sn, 0, 0, 0);
			mcmsg_send(mt, MCTRL_PAK, pid_si, sn->nxrq.source_ptype);
		}

		assert((t = MAXLOOP) != 0);
		do {
	/* In this loop we remove all select_items for the process that 
	   just sent us a PAK as well as any other process that initiated
	   and completed the PNQ/PAK protocol while our message to it 
	   was sitting on this list.
	 */
			register long pid;

			assert(t-- != 0);
			sn = si->link;

			if ((pid = mcmsg_remote_pid(mt,
					       sn->nxrq.dest_node,
					       sn->nxrq.dest_ptype)) < 0) {
				si = si->link;
				continue;
			}
			if ((pid_si = mcmsg_lookup_remote(mt, pid)) == 0) {
				si = si->link;
				continue;
			}
			sn->nxrq.pid_si = pid_si;
			if ((!pid_si->ppid.sent_pak) ||
			    (!pid_si->ppid.recvd_pak)) {
				si = si->link;
				continue;
			}

			if (sn == sn->link) {
				mt->send_wait_unk = 0;
			} else {
				if (st == sn) {
					mt->send_wait_unk = si;
				}
				si->link  = sn->link;
			}
			sn->link = 0;
			mcmsg_trace_debug("pak scan found msg",
				4,si,sn,st,mt->send_wait_unk);
			assert(sn->method == MCTRL_PNQ);
			mcmsg_inq_switch[sn->value](mt, sn);
		} while (sn != st) ;

		if (inq && mt->send_wait_unk &&
		    (sn = mt->send_wait_unk->link) != 0 && sn->method != 0) {
			mcmsg_trace_debug("recv_pak send next pnq",1,sn,0,0,0);
			mcmsg_send(mt, sn->method, sn, sn->value);
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
mcmsg_send_prm(mt, ctl, route, src_pid, dst_pid)
	mcmsg_task_t	*mt;
	register unsigned long	route;
	register unsigned long	src_pid;
	register unsigned long	dst_pid;
{
	mcmsg_trace_send(MCTRL_PRM, 8, dst_pid, 2, src_pid, dst_pid);

	send2(route, 0);
	send2(MCTRL_PRM, 8);
	send2eod(src_pid, dst_pid);

	return;
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
	register select_item_t *si;
	register select_item_t *pid_si;
	register select_item_t *pid_last;
	register mcmsg_task_t	*mt;
	register unsigned long rtotal;
	register ptype_list_t	*node_ptype_list;
	register node_list_t	*node_list;
	register int	i, remote_node, pid_index;
        unsigned long t;


	recv2(source_pid, dest_pid);

	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		/*
		 * The destination pid does not exist
		 */
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		return;
	}
	mt = si->mcmsg_task;
	
	/* 
	 * lookup the remote pid
	 */
	pid_si = mcmsg_lookup_remote(mt, source_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, 
		pid_si, pid_si ? pid_si->ppid.recv_total : 0);

	if (pid_si != 0) {
		/*
		 * release message buffer resource
		 */
		mt = pid_si->mcmsg_task;
		remote_node = pid_si->ppid.node;
		assert(mt != 0);

		/*
		 * Remove from avail_need list if present
		 */

		if ((mt->avail_need) && (pid_si->ppid.avail_link)) {
		    pid_last = mt->avail_need;

		    assert((t = MAXLOOP) != 0);
		    for(;;) {
			assert(pid_last->method == SELMETH_PID);
			if (pid_si == pid_last->ppid.avail_link) {
				assert(pid_si->ppid.avail_link != 0);
				if (pid_si == pid_last) {
		       			assert(mt->avail_need == pid_si);
					mt->avail_need = 0;
				} else {
					pid_last->ppid.avail_link =
					 pid_si->ppid.avail_link;
					if (mt->avail_need == pid_si) {
					    mt->avail_need = pid_last;
					}
				}
				pid_si->ppid.avail_link = 0;
				break;   
			}
			pid_last = pid_last->ppid.avail_link;
			if (pid_last == mt->avail_need) 
				break;
			assert(t-- != 0);
		    }
		}

		/*
		 * Remove from attach_pending list if present
		 */

		if ((mt->attach_pend) && (pid_si->ppid.att_pend_link)) {
		    pid_last = mt->attach_pend;
		    assert((t = MAXLOOP) != 0);
		    for(;;) {
			assert(pid_last->method == SELMETH_PID);
			if (pid_si == pid_last->ppid.att_pend_link) {
				assert(pid_si->ppid.att_pend_link != 0);
				if (pid_si == pid_last) {
		       			assert(mt->attach_pend == pid_si);
					mt->attach_pend = 0;
				} else {
					pid_last->ppid.att_pend_link =
					 pid_si->ppid.att_pend_link;
					if (mt->attach_pend == pid_si) {
					    mt->attach_pend = pid_last;
					}
				}
				pid_si->ppid.att_pend_link = 0;
				break;   
			}
			pid_last = pid_last->ppid.att_pend_link;
			if (pid_last == mt->attach_pend) 
				break;
			assert(t-- != 0);
		    }
		}

		/*
		 * Clean up waiting sends 
		 */

		if (pid_si->ppid.send_wait) {
		    si = pid_si->ppid.send_wait;
		    assert((t = MAXLOOP) != 0);
		    for(;;) {
			register nxreq_t        *req;
			register select_item_t  *sl;

			req = (long *)mcmsg_validate_long(mt, si->nxrq.request);
			if (req != 0) {
				req->state = NX_COMPLETE;
			}
			sl = si->link;
			if (!(si->nxrq.vm_ast_pending)) {
				mcmsg_free_select_item(si);
			}
		  	si->nxrq.pid_si = (select_item_t *) 0xdead;
			if (sl == pid_si->ppid.send_wait) {
				break;
			} else {
				si = sl;
			}
			assert(t-- != 0);
		    }
		    pid_si->ppid.send_wait = 0;
		}

		/* Clean up any pending recvs for messages sent from this pid.
		 */

		si = mcmsg_selector_lookup_si(&mcmsg_seq_sel, source_pid);
		if (si != 0) {
			assert((t = MAXLOOP) != 0);
			while (si != 0) {
				/* save the UNIX pid so recv_continue can
				   call mcmsg_remove_sequence on this si
				 */	
				si->value = si->nxrq.pid_si->value;
				si->nxrq.pid_si = (select_item_t *) 0xdead;
				si = si->nxrq.seq_link;
				assert(t-- != 0);
			}
		}
		/*
		 * Detach and free the item representing the remote pid.
		 */
		mcmsg_remove_remote(mt, source_pid);

		/*
		 * Remove the pid from the ptype list[s].
		 */
		node_ptype_list = mt->node_ptype_list;
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

mcmsg_remote_pid(mt, node, ptype)
	mcmsg_task_t	*mt;
	register unsigned long	node;
	register unsigned long	ptype;
{
	register ptype_list_t	*node_ptype_list;
	register int		i;
	register node_list_t	*node_list;

	if (node > mt->numnodes) {
		return -1;
	}
	node_ptype_list = mt->node_ptype_list;
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

mcmsg_task_t *
mcmsg_find_local(app, node, ptype, physnode, source_pid)
	register unsigned long	app;
	register unsigned long	node;
	register unsigned long	ptype;
	register unsigned long	physnode;
	register unsigned long	source_pid;
{
	register select_item_t	*si;
	register ptype_list_t	*task_ptype_list;
	register mcmsg_task_t	*mt;
	register int		i;

	si = mcmsg_selector_lookup_si(&mcmsg_app_sel, app);
	if (si == 0) {
		mcmsg_delay_pna(physnode, source_pid);
		return 0;
	}
	task_ptype_list = (ptype_list_t *)(si->item);
	if (task_ptype_list == 0) {
		mcmsg_trace_debug("dest app not found", 3, app, node, ptype, 0);
		mcmsg_delay_pna(physnode, source_pid);
		return 0;
	}

	i = mcmsg_ptype_select(task_ptype_list, ptype);
	if (i < 0 ||
	 (mt = (mcmsg_task_t *)(task_ptype_list->ptype_entry[i].item)) == 0) {
		mcmsg_trace_debug("dest ptype not found", 3,
				  app, node, ptype, 0);
		mcmsg_delay_pna(physnode, source_pid);
		return 0;
	}

	if (mt->node != node) {
		mcmsg_trace_drop("wrong node", node);
		mcmsg_msg_drop++;
		return 0;
	}
	return mt;
}

/*
 *	Hash on node number (pid >> 16) while value is (pid).
 *	This is done for performance reasons as this allows hashing
 *	to distribute more evenly for the SPMD (usual) case. (gjr)
 *	See PTS#9755
 */

#define PID_HASH(p) (p >> 16)

/*
 *	Routine(s):
 *		mcmsg_install_remote()
 *		mcmsg_lookup_remote()
 *		mcmsg_remove_remote()
 *
 *	Are used solely to install, lookup, and remove remote pid
 *	items from the remote pid selector (mt->pid_sel).
 *
 */
select_item_t *
mcmsg_install_remote(mt, pid, node, ptype)
	register mcmsg_task_t	*mt;
	register long		pid;
	register unsigned long	node;
	register long		ptype;
{
	register select_item_t	*si;
	register int		i;
	register ptype_list_t	*node_ptype_list;
	register node_list_t	*node_list;

	si = (select_item_t *)mcmsg_lookup_remote(mt, pid);

	if (si == 0) {
		assert(mt != 0);
		assert(node <= mt->numnodes);

		node_ptype_list = mt->node_ptype_list;
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
				  pid, si, mt, mt->pid_sel);
		si->ppid.node = node;
		si->ppid.process_lock = 0;
		si->ppid.send_ready = 0;
		si->ppid.sent_pak = 0;
		si->ppid.recvd_pak = 0;
#if BIGPKTS
		si->ppid.sent_nat = 0;
#endif BIGPKTS
		si->ppid.in_rel_send_wait = 0;
		si->ppid.route = calculate_route(
			mt->task_ptype_list->phys_node_list[node]);
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
		si->ppid.attach_link = 0;
		si->ppid.att_pend_link = 0;
		si->ppid.sub_pid_si = 0;
		si->ppid.when_recvd = 0;
		/*
		 * Install remote pids based on node (PTS#9755)
		 */
		mcmsg_selector_install_si(mt,
					  mt->pid_sel,
					  si,
					  PID_HASH(pid),
					  SELMETH_PID);
		si->value = pid;
	}
	assert(	si->ppid.avail_link == 0 ||
		mt->avail_need != 0);
	return si;
}

#if HANDCODE
/* see msgp_sel.s */
#else HANDCODE
select_item_t *
mcmsg_lookup_remote(mt, pid)
	mcmsg_task_t		*mt;
	register long		pid;
{
	register select_item_t	*si;
	register select_item_t	*sitail;	/* Tail item */
	register unsigned long	hi;		/* Hash index */
	unsigned long t;

	/*
	 * Look up hash entry
	 */
	if (PID_HASH(pid) == 0) {
		sitail = mt->pid_sel->zero;
	} else {
		hi = SELECT_HASH_FUN(PID_HASH(pid));

		assert(hi < SELECT_HASH_LEN);
		sitail = mt->pid_sel->hash[hi];
	}
	if (sitail == 0) {
		return 0;
	}

	/*
	 * Search the linked list from head to tail
	 * for the desired value
	 */

	assert((t = MAXLOOP) != 0);
	for (si = sitail->link; ; si = si->link) {
		assert(si != 0);
		if (si->value == pid)
			break;
		if (si == sitail) {
			return 0;
		}
		assert(t-- != 0);
	}

	assert( si == 0 || si->mcmsg_task == mt);
	assert(	si == 0 ||
		si->ppid.avail_link == 0 ||
		mt->avail_need != 0);
	return si;
}
#endif HANDCODE

mcmsg_remove_remote(mt, pid)
	mcmsg_task_t *mt;
	register unsigned long	pid;
{
	register select_item_t	*si;	/* Select item */
	register select_item_t	*siprev;	/* Item before si on the list */
	register select_item_t	*sitail;	/* Tail item */
	register void		**seltail;	/* Pointer to head entry */
	register unsigned long	hi;		/* Hash index */

	/*
	 * Find and deinstall the select_item
	 */

	if (PID_HASH(pid) == 0) {
		seltail = &mt->pid_sel->zero;
	} else {
		hi = SELECT_HASH_FUN(PID_HASH(pid));
		assert(hi < SELECT_HASH_LEN);
		seltail = &mt->pid_sel->hash[hi];
	}

	/*
	 * Get tail item
	 */

	sitail = *seltail;
	if (sitail == 0) {
		return;
	}

	/*
	 * Search list from head to tail with si
	 * siprev follows si
	 */

	siprev = sitail;
	si = sitail->link;
	if (si == sitail) {
		/* Special case for one item in list */
		if (si->value != pid)
			return;
		*seltail = 0;
	} else {
		unsigned long t;

		/*
		 * Search the list
		 */

		assert((t = MAXLOOP) != 0);
		for (;;) {
			if (si->value == pid)
				break;
			if (si == sitail)
				return;
			siprev = si;
			si = si->link;
			assert(t-- != 0);
		}

		/*
		 * Relink si out of the list
		 */

		siprev->link = si->link;
		if (si == sitail) {
			*seltail = siprev;
		}
	}
	/* Clear the link when asserts enabled */
	assert((si->link = 0) == 0);

	/*
	 * Free it
	 */
	mcmsg_free_select_item(si);
}
