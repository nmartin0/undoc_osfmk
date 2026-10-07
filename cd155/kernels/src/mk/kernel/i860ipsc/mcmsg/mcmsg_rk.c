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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_rk.c,v 1.29 1994/11/18 20:41:55 mtm Exp $
 */

/*
 * mcmsg_rk.c
 *
 * RK messages
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_nx.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>

syscall_mcmsg_xsend(xmsg, node, ptype)
	xmsg_t		*xmsg;
	unsigned	node;
	int		ptype;
{
	register task_t		task;
	register xmsg_t		*xp;
	register int		x;
	register long		i;
	register mcmsg_task_t	*mt;
	register unsigned long	sequence;
	register unsigned long	msg_size;
	register long		dest_pid;
	register select_item_t	*si;
	register select_item_t	*pid_si;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	task = current_task();
	mt = task->mcmsg_task;
	if (mt == 0) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}
	mcmsg_phys = 0;

	mcmsg_task = mt;
	xp = (xmsg_t *)mcmsg_validate_line(xmsg);
	if (xp == 0) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}
	sequence = mcmsg_send_sequence;
	mcmsg_send_sequence = sequence + 1;

	msg_size = ((xp->length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));

	dest_pid = mcmsg_remote_pid(node, ptype);
	if (dest_pid != -1) {
		pid_si = mcmsg_lookup_remote(dest_pid);
		if (pid_si == 0) {
			assert(mcmsg_reentry--);
			RED_OFF(RED_MSG);
			splx(x);
			return -1; /* XXX local node */
		}
	} else {
		pid_si = 0;
	}
	mcmsg_trace_debug("rk send", 4, node, msg_size, dest_pid, pid_si);

	si = mcmsg_alloc_select_item();
	assert(si != 0);
	si->nxrq.xmsg = xmsg;
	si->value = sequence;
	si->mcmsg_task = mt;
	si->nxrq.pid_si = pid_si;
	si->nxrq.dest_node = node;
	si->nxrq.dest_ptype = ptype;
	si->nxrq.buf = (unsigned long)(xmsg + 1);
	si->nxrq.count = xp->length;
	si->nxrq.sequence = sequence;
	si->nxrq.vm_ast_pending = 0;

	if (pid_si != 0 &&
	    pid_si->ppid.send_avail >= msg_size) {
		si->method = SENDMETH_RK1;
		si->nxrq.take = msg_size;
		pid_si->ppid.send_avail -= msg_size;
		mcmsg_send(SENDMETH_RK1, si, sequence);
	} else {
		select_item_t	*st;
		select_item_t	*sh;

		si->method = SENDMETH_RKS;
		si->nextmethod = SENDMETH_RK1A;

		if (pid_si != 0) {
			st = pid_si->ppid.send_wait;
			pid_si->ppid.send_wait = si;
			if (st == 0) {
				si->link = si;
				mcmsg_send(SENDMETH_RKS, si, sequence);
			} else {
				sh = st->link;
				si->link = sh;
				st->link = si;
			}
		} else {
			mcmsg_inquire(si);
		}
	}
	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

/*
 *	mcmsg_send_rka
 *
 *	RK give buffer space to node.
 */

mcmsg_send_rka(pid_si, mcmsg_task)
	register mcmsg_task_t	*mcmsg_task;
	register select_item_t	*pid_si;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;

	hdr1 = MCTRL_RKA | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = 8;
	hdr3 = mcmsg_task->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 1, pid_si->ppid.recv_total, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2eod(hdr3, pid_si->value);
	return 0;
}

/*
 *	mcmsg_send_rks
 */

mcmsg_send_rks(si, sequence)
	register select_item_t	*si;
	register unsigned long	sequence;
{
	register unsigned long	node;
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;
	register unsigned long	route;

	pid_si = si->nxrq.pid_si;
	if (pid_si != 0) {
		hdr1 = MCTRL_RKS | (pid_si->ppid.recv_give << 16);
		pid_si->ppid.recv_give = 0;
		route = pid_si->ppid.route;
	} else {
		hdr1 = MCTRL_RKS;
		route = calculate_route(
		 mcmsg_task->task_ptype_list->phys_node_list[si->nxrq.dest_node]);

mcmsg_trace_debug("rks route", 3, si->nxrq.dest_node,
 mcmsg_task->task_ptype_list->phys_node_list[si->nxrq.dest_node], route, 0);

	}
	hdr2 = 16 | (si->nxrq.sequence << 16);
	mcmsg_trace_send(hdr1, hdr2, mcmsg_task->pid, 1, si->nxrq.count, 0);
	send2(route, 0);
	send2(hdr1, hdr2);
	send2(mcmsg_task->pid, mcmsg_task->node);
	send2(mcmsg_task->applinfo.app, si->nxrq.dest_node);
	send2eod(si->nxrq.dest_ptype, si->nxrq.count);
	return 0;
}

/*
 *	mcmsg_send_rkr
 *
 *	Send RK flow control packet.
 */

mcmsg_send_rkr(pid_si, sequence)
	register select_item_t	*pid_si;
	register unsigned long	sequence;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register mcmsg_task_t	*mt;

	mt = mcmsg_task;

	hdr1 = MCTRL_RKR | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = 8 | (sequence << 16);
	mcmsg_trace_send(hdr1, hdr2, mt->pid, 1, pid_si, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2eod(mt->pid, pid_si->value);
	return 0;
}

mcmsg_send_rk1a(si, sequence)
	register select_item_t	*si;
	register unsigned long	sequence;
{
	register select_item_t	*pid_si;
	register unsigned long	length;
	register unsigned long	pkt;
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	bufp;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register xmsg_t		*xp;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);

	if (!mcmsg_validate_send_buffer(SENDMETH_RK1A, si)) {
		return 0;
	}
	pkt = mcmsg_task->applinfo.pkt_size;
	length = si->nxrq.count;
	bufp = si->nxrq.buf;
	if (length <= pkt) {
		pkt = (length + PKT_GRAN-1) & ~(PKT_GRAN-1);
		if (length > 0) {
			bp1 = mcmsg_validate_line(bufp);
			bp2 = mcmsg_validate(bufp + length-1);
			if (bp1 == 0 || bp2 == 0) {
				return 0;
			}
		}
	} else {
		bp1 = mcmsg_validate_line(bufp);
		bp2 = mcmsg_validate(bufp + pkt-1);
		if (bp1 == 0 || bp2 == 0) {
			mcmsg_trace_drop("rk1a send invalid buffer", bufp);
			mcmsg_msg_drop++;
			return 0;
		}
		si->nxrq.offset = pkt;
		si->nxrq.buf = bufp + pkt;
		si->nxrq.count = length - pkt;
	}
	hdr1 = MCTRL_RKN | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = (pkt + 8) | (si->nxrq.sequence << 16);
	mcmsg_trace_send(hdr1, hdr2, mcmsg_task->pid, 1, 0, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);

	if (length == 0) {
		send2eod(mcmsg_task->pid, 0);
	} else {
		send2(mcmsg_task->pid, 0);
		mcmsg_send_buf(bp1, bp2, pkt);
		if (length > pkt) {
			return SENDMETH_RKN;
		}
	}
	xp = (xmsg_t *)mcmsg_validate_line(si->nxrq.xmsg);
	if (xp != 0) {
		xp->state = XMSG_TRASH;
	}
	mcmsg_free_select_item(si);
	return 0;
}

/*
 *	mcmsg_send_rk1
 *
 *	RK send first packet.
 */

mcmsg_send_rk1(si, sequence)
	register select_item_t	*si;
	register unsigned long	sequence;
{
	register select_item_t	*pid_si;
	register unsigned long	length;
	register unsigned long	pkt;
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	bufp;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register xmsg_t		*xp;

	if (!mcmsg_validate_send_buffer(SENDMETH_RK1, si)) {
		return 0;
	}
	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);

	pkt = mcmsg_task->applinfo.pkt_size;
	length = si->nxrq.count;
	bufp = si->nxrq.buf;
	if (length <= pkt) {
		pkt = (length + PKT_GRAN-1) & ~(PKT_GRAN-1);
		if (length > 0) {
			bp1 = mcmsg_validate_line(bufp);
			bp2 = mcmsg_validate(bufp + length-1);
			if (bp1 == 0 || bp2 == 0) {
				return 0;
			}
		}
	} else {
		bp1 = mcmsg_validate_line(bufp);
		bp2 = mcmsg_validate(bufp + pkt-1);
		if (bp1 == 0 || bp2 == 0) {
			mcmsg_trace_drop("rk1 send invalid buffer", bufp);
			mcmsg_msg_drop++;
			return 0;
		}
		si->nxrq.offset = pkt;
		si->nxrq.buf = bufp + pkt;
		si->nxrq.count = length - pkt;
	}
	hdr1 = MCTRL_RK1 | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = (pkt + 16) | (si->nxrq.sequence << 16);
	mcmsg_trace_send(hdr1, hdr2, mcmsg_task, 1, length, 0);

	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(mcmsg_task->pid, pid_si->value);
	if (length == 0) {
		send2eod(si->nxrq.dest_ptype, 0);
	} else {
		send2(si->nxrq.dest_ptype, length);
		mcmsg_send_buf(bp1, bp2, pkt);
		if (length > pkt) {
			return SENDMETH_RKN;
		}
	}
	xp = (xmsg_t *)mcmsg_validate_line(si->nxrq.xmsg);
	if (xp != 0) {
		xp->state = XMSG_TRASH;
	}
	mcmsg_free_select_item(si);
	return 0;
}

/*
 *	mcmsg_send_rkn
 *
 *	RK send subsequent packet.
 */

mcmsg_send_rkn(si, sequence)
	register select_item_t	*si;
	register unsigned long	sequence;
{
	register select_item_t	*pid_si;
	register unsigned long	length;
	register unsigned long	pkt;
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	bufp;
	register unsigned long	offset;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register xmsg_t		*xp;

	if (!mcmsg_validate_send_buffer(SENDMETH_RKN, si)) {
		return 0;
	}
	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);

	pkt = mcmsg_task->applinfo.pkt_size;
	offset = si->nxrq.offset;
	length = si->nxrq.count;
	bufp = si->nxrq.buf;
	if (length <= pkt) {
		pkt = (length + PKT_GRAN-1) & ~(PKT_GRAN-1);
	}
	bp1 = mcmsg_validate(bufp);
	bp2 = mcmsg_validate(bufp + pkt - 1);
	if (bp1 == 0 || bp2 == 0) {
		mcmsg_trace_drop("rkn send invalid buffer", bufp);
		mcmsg_msg_drop++;
		return 0;
	}
	hdr1 = MCTRL_RKN | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = (pkt + 8) | (si->nxrq.sequence << 16);
	mcmsg_trace_send(hdr1, hdr2, mcmsg_task->pid, 1, offset, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(mcmsg_task->pid, offset);

	mcmsg_send_buf(bp1, bp2, pkt);
	if (length > pkt) {
		si->nxrq.offset = offset + pkt;
		si->nxrq.buf = bufp + pkt;
		si->nxrq.count = length - pkt;
		return SENDMETH_RKN;
	}
	xp = (xmsg_t *)mcmsg_validate_line(si->nxrq.xmsg);
	if (xp != 0) {
		xp->state = XMSG_TRASH;
	}
	mcmsg_free_select_item(si);
	return 0;
}

/*
 *	mcmsg_recv_rka
 *
 *	RK receive buffer space.
 */

mcmsg_recv_rka(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	dest_pid;
	register mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register unsigned int	give;

	recv2(source_pid, dest_pid);

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("dest pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

	pid_si = mcmsg_lookup_remote(source_pid);
	if (pid_si == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("source pid not found", source_pid);
		mcmsg_msg_drop++;
		return;
	}
	give = hdr1 >> 16;
	pid_si->ppid.send_avail += give;

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, pid_si->ppid.send_avail, 0);

	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(pid_si);
	}
}

mcmsg_recv_rks(hdr1, hdr2)
	register unsigned long hdr1;
	register unsigned long hdr2;
{
	register long	source_pid;
	register unsigned long	source_node;
	register long		dest_app;
	register unsigned long	dest_node;
	register long		dest_pid;
	register long 		dest_ptype;
	register unsigned long	msg_length;
	register unsigned long	msg_size;
	register long 		source_ptype;
	register unsigned long	sequence;
	register unsigned long	avail;
	register xmsg_t		*xmsg;
	register xmsg_t		*xp;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register select_item_t	*sh;
	mcmsg_task_t	*mt;

	recv2(source_pid, source_node);
	recv2(dest_app, dest_node);
	recv2(dest_ptype, msg_length);
	sequence = hdr2 >> 16;
	source_ptype = dest_ptype; /* XXX FIX THIS */

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, msg_length, 0);
	if (mcmsg_find_local(dest_app, dest_node, dest_ptype) != 0) {
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

	pid_si = mcmsg_install_remote(source_pid, source_node, source_ptype);
	pid_si->ppid.send_avail += hdr1 >> 16;

	assert(mt->provided >= mt->assigned);
	avail = mt->provided - mt->assigned;
	msg_size = ((msg_length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));

	if (avail >= msg_size &&
	    (xmsg = mcmsg_alloc_whole_xmsg(msg_length, avail)) != 0) {
		xp = (xmsg_t *)mcmsg_validate_line(xmsg);
		xp->length = msg_length;
		xp->source_node = pid_si->ppid.node;

		si = mcmsg_alloc_select_item();
		assert(si != 0);
		si->method = SELMETH_RECV_XMSG;
		si->nxrq.xmsg = xmsg;
		si->value = source_pid;
		si->nxrq.pid_si = pid_si;
		si->nxrq.sequence = sequence;
		si->nxrq.buf = (unsigned long)(xmsg + 1);
		si->nxrq.count = msg_length;
		si->nxrq.take = 0;
		si->nxrq.stop = msg_length + PKT_GRAN;
		si->nxrq.offset = 0;

		mcmsg_install_sequence(source_pid, si);

		mcmsg_send(SENDMETH_RKR, pid_si, sequence);

	} else {

		assert(pid_si->ppid.rk_recv_pid == 0);
		pid_si->ppid.rk_recv_want = msg_length;
		pid_si->ppid.rk_recv_seq = sequence;
		pid_si->ppid.rk_recv_pid = source_pid;
		pid_si->ppid.rk_recv_type = -1;
		pid_si->ppid.rk_recv_ptype = -1;
		if (pid_si->ppid.avail_link == 0) {
			if (mt->avail_need == 0) {
				pid_si->ppid.avail_link = pid_si;
			} else {
				register select_item_t *sh;
				register select_item_t *st;

				st = mt->avail_need;
				sh = st->ppid.avail_link;
				st->ppid.avail_link = pid_si;
				pid_si->ppid.avail_link = sh;
			}
			mt->avail_need = pid_si;
		}
	}
}

mcmsg_recv_rkr(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register long		source_pid;
	register unsigned long	dest_pid;
	register unsigned long	sequence;
	register unsigned long	give;
	register mcmsg_task_t	*mt;
	register select_item_t	*st;
	register select_item_t	*sh;
	register select_item_t	*si;
	register select_item_t	*sn;
	register unsigned long	t;
	register select_item_t	*pid_si;
	register long		pid;
	register unsigned char	s_out;

	recv2(source_pid, dest_pid);
	sequence = hdr2 >> 16;
	give = hdr1 >> 16;

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;
	pid_si = mcmsg_lookup_remote(source_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, mcmsg_task, pid_si);
	assert(pid_si != 0);

	assert(pid_si->ppid.send_ready);
	pid_si->ppid.send_avail += give;


	if (!mcmsg_release_send_seq(pid_si, sequence)) {

		/*
		 * Search queued sends
		 */

		s_out = mcmsg_send_store_out;
		assert((t = MAXLOOP) != 0);
		while (s_out != mcmsg_send_store_in) {
			st = (select_item_t *)mcmsg_send_store[s_out].item;
			if (st != 0 &&
			    (st->nextmethod == SENDMETH_NXN) &&
			    st->nxrq.pid_si == pid_si &&
			    st->nxrq.sequence == sequence) {
				st->nxrq.stop = st->nxrq.count;
				mcmsg_send_store[s_out].method = SENDMETH_NXN;
				mcmsg_trace_debug("rkr send", 1, st, 0, 0, 0);
				return;
			}
			s_out++;
			assert(t-- != 0);
		}
	}

	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(pid_si);
	}
}

mcmsg_recv_rk1(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register long		source_pid;
	register long		dest_pid;
	register long 		dest_ptype;
	register select_item_t	*pid_si;
	register unsigned long	msg_length;
	register int		pkt;
	register unsigned long	offset;
	register xmsg_t		*xmsg;
	register xmsg_t		*xp;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	node;
	register mcmsg_task_t	*mt;
	register unsigned long	msg_size;
	register select_item_t	*si;

	pkt = (hdr2 & 0xFFFF) - 16;
	assert(pkt >= 0);

	recv2(source_pid, dest_pid);
	recv2(dest_ptype, msg_length);

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, msg_length, 0);
	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush(pkt);
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

	pid_si = mcmsg_lookup_remote(source_pid);
	assert(pid_si != 0);
	pid_si->ppid.send_avail += hdr1 >> 16;

	msg_size = ((msg_length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));
	mcmsg_relinquish(mt, pid_si, msg_size);

	xmsg = mcmsg_alloc_whole_xmsg(msg_length, mt->provided - mt->assigned);
	if (xmsg == 0) {
		mcmsg_trace_drop("space not found", msg_length);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush(pkt);
		return;
	}
	xp = (xmsg_t *)mcmsg_validate_line(xmsg);
	xp->length = msg_length;
	xp->source_node = pid_si->ppid.node;
	if (pkt > 0) {
		bp1 = mcmsg_validate_line(xmsg + 1);
		bp2 = mcmsg_validate(((unsigned long)(xmsg + 1)) + pkt - 1);
		if (bp1 == 0 || bp2 == 0) {
			mcmsg_trace_drop("rk1 recv buffer invalid", xmsg + 1);
			mcmsg_msg_drop++;
			mcmsg_fifo_flush(pkt);
			return;
		}
		mcmsg_recv_buf(bp1, bp2, pkt);
	}
	if (msg_length <= pkt) {
		xp->state = XMSG_FULL;
		return;
	}

	si = mcmsg_alloc_select_item();
	assert(si != 0);
	si->method = SELMETH_RECV_XMSG;
	si->nxrq.xmsg = xmsg;
	si->value = source_pid;
	si->nxrq.pid_si = pid_si;
	si->nxrq.sequence = hdr2 >> 16;
	si->nxrq.buf = ((unsigned long)(xmsg + 1)) + pkt;
	si->nxrq.count = msg_length;
	si->nxrq.take = 0;
	si->nxrq.stop = msg_length + PKT_GRAN;
	si->nxrq.offset = pkt;

	mcmsg_install_sequence(source_pid, si);
}

/*
 *	mcmsg_recv_rkn
 *
 *	RK receive subsequent packet.
 */

mcmsg_recv_rkn(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register long		source_pid;
	register unsigned long	offset;
	register int		pkt;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register xmsg_t		*xmsg;
	register xmsg_t		*xp;

	pkt = (hdr2 & 0xFFFF) - 8;
	assert(pkt >= 0);

	recv2(source_pid, offset);


	assert((offset & (PKT_GRAN-1)) == 0);

	si = mcmsg_lookup_sequence(source_pid, hdr2 >> 16);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, offset, si);
	if (si == 0) {
		mcmsg_trace_drop("seq not found", hdr2 >> 16);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush(pkt);
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

	pid_si = mcmsg_lookup_remote(source_pid);
	assert(pid_si != 0);
	pid_si->ppid.send_avail += hdr1 >> 16;

	xmsg = si->nxrq.xmsg;

	if (pkt > 0) {
		bp1 = mcmsg_validate(si->nxrq.buf);
		bp2 = mcmsg_validate(si->nxrq.buf + pkt - 1);
		if (bp1 == 0 || bp2 == 0) {
			mcmsg_trace_drop("buffer not valid",
				((unsigned long)(xmsg + 1)) + offset);
			mcmsg_msg_drop++;
			mcmsg_fifo_flush(pkt);
			return;
		}
		mcmsg_recv_buf(bp1, bp2, pkt);
	}
	if (si->nxrq.count <= pkt) {
		xp = (xmsg_t *)mcmsg_validate_line(xmsg);
		if (xp != 0) {
			xp->state = XMSG_FULL;
		}
		mcmsg_remove_sequence(source_pid, si);
		return;
	} else {
		si->nxrq.buf += pkt;
		si->nxrq.count -= pkt;
	}
}

/*
 *	mcmsg_release_send_seq
 *
 *	Release a specific message from send_wait queue.
 *
 *	returns:
 *		0 if none sent
 *		1 if send started
 */
mcmsg_release_send_seq(pid_si, sequence)
select_item_t *pid_si;
unsigned long sequence;
{
register select_item_t *st, *sn, *sh;
register unsigned long	t;

	st = pid_si->ppid.send_wait;
	if (st != 0) {
		sn = st;
		sh = st->link;
		assert((t = MAXLOOP) != 0);
		for (;;) {
			assert(sh != 0);
			assert(sh->method != 0xdead);
			if (sh->value == sequence) {
				if (sh == st) {
					pid_si->ppid.send_wait = sn = 0;
				} else {
					if (sh == sn) {
					    pid_si->ppid.send_wait = st;
					}
					st->link = sn = sh->link;
				}

				mcmsg_trace_debug("rel send seq", 2, sh,
					pid_si, 0, 0);
				mcmsg_send(sh->nextmethod,
					   sh, sequence);

				if (sn != 0 && sn->method != 0) {
					mcmsg_send(sn->method,
						   sn,
						   sn->value);
					sn->method = 0;
				}
				return 1;
			}
			if (sh == sn) {
				break;
			}
			st = sh;
			sh = st->link;
			assert(t-- != 0);
		}
	}
	return 0;
}

/*
 *	mcmsg_release_send_wait
 *
 *	Release first NEW message from send_wait queue.
 *
 *	Returns:
 */
mcmsg_release_send_wait(pid_si)
select_item_t *pid_si;
{
register select_item_t *st, *sn, *sh;
register unsigned long	t;

	st = pid_si->ppid.send_wait;
	if (st != 0) {
		sn = st;
		sh = st->link;
		assert((t = MAXLOOP) != 0);
		for (;;) {
			assert(sh != 0);
			assert(sh->method != 0xdead);
			/*
			 * Look for NEW message.
			 */
			if (sh->method == 0 && 
			    sh->nextmethod == SENDMETH_NX1 &&
			    (!sh->nxrq.vm_ast_pending)) {
				/*
				 * take message out of queue
				 */
				if (sh == st) {
					pid_si->ppid.send_wait = sn = 0;
				} else {
					if (sh == sn) {
					    pid_si->ppid.send_wait = st;
					}
					st->link = sn = sh->link;
				}

				/*
				 * Start the send.
				 */
				mcmsg_trace_debug("rel send wait", 2, sh,
					pid_si, 0, 0);
				sh->nextmethod = SENDMETH_NXN;
				mcmsg_schedule_send(sh);
				return 1;
			}
			if (sh == sn) {
				break;
			}
			st = sh;
			sh = st->link;
			assert(t-- != 0);
		}
	}
	return 0;
}

