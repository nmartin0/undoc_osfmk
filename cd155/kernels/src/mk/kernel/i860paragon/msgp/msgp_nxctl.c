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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_nxctl.c,v 1.19 1995/03/27 17:05:43 joel Exp $

/*
 * msgp_nxctl.c
 *
 * NX messages (control)
 */

#define	MCMSG_MODULE	MCMSG_MODULE_NX

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/msgp/msgp_hw.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>

select_item_t *mcmsg_find_nx_recv();

/*
 *	mcmsg_send_nxs
 *
 *	Send NXS flow control packet.
 */

mcmsg_send_nxs(mt, ctl, si)
	mcmsg_task_t	*mt;
	select_item_t	*si;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	assert((pid_si->ppid.recv_give & 0x1f) == 0);
	hdr1 = MCTRL_NXS | (pid_si->ppid.recv_give << 11);
	pid_si->ppid.recv_give = 0;
	hdr2 = 24 | (si->nxrq.sequence << 16);
	hdr3 = mt->pid;
	mcmsg_trace_send(hdr1, hdr2, pid_si->value, 2,
			 si->nxrq.count, si);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, si->nxrq.source_ptype);
	send2(pid_si->value, si->nxrq.dest_ptype);
	send2(si->nxrq.msg_type, si->nxrq.count);
	send2eod(si->nxrq.originating_node, 0);
	return;
}

/*
 *	mcmsg_send_nxm
 *
 *	Send NXM flow control packet.
 */

mcmsg_send_nxm(mt, ctl, si)
	mcmsg_task_t	*mt;
	select_item_t	*si;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	assert((pid_si->ppid.recv_give & 0x1f) == 0);
	hdr1 = MCTRL_NXM | (pid_si->ppid.recv_give << 11);
	pid_si->ppid.recv_give = 0;
	hdr2 = 24 | (si->nxrq.sequence << 16);
	hdr3 = mt->pid;
	mcmsg_trace_send(hdr1, hdr2, pid_si->value, 2,
			 si->nxrq.count, si);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, si->nxrq.source_ptype);
	send2(pid_si->value, si->nxrq.dest_ptype);
	send2(si->nxrq.msg_type, si->nxrq.count);
	send2eod(si->nxrq.originating_node, 0);
	return;
}

/*
 *	mcmsg_send_nxq
 *
 *	Send NXQ flow control packet.
 */

mcmsg_send_nxq(mt, ctl, si)
	mcmsg_task_t	*mt;
	select_item_t	*si;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	assert((pid_si->ppid.recv_give & 0x1f) == 0);
	hdr1 = MCTRL_NXQ | (pid_si->ppid.recv_give << 11);
	pid_si->ppid.recv_give = 0;
	hdr2 = 32;
	hdr3 = mt->pid;
	mcmsg_trace_send(hdr1, hdr2, pid_si->value, 1, si, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, si->nxrq.source_ptype);
	send2(pid_si->value, si->nxrq.dest_ptype);
	send2eod(si->nxrq.msg_type, 0);
	return;
}

/*
 *	mcmsg_send_nxc
 *
 *	Send NXC flow control packet.
 */

mcmsg_send_nxc(mt, ctl, si)
	mcmsg_task_t	*mt;
	register select_item_t	*si;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;
	register nxreq_t        *np;


	pid_si = si->nxrq.pid_si;
	if (pid_si == (select_item_t *) 0xdead) {   /* PTS 11968 */
		/* sender must have terminated */
		if (si->nxrq.request) {
		    np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
		    if (np == 0) {
			mcmsg_trace_drop("send_nxc bad nxreq",si->nxrq.request);
		    } else {
			np->err = NX_ERR_NOSENDER;
			np->state = NX_COMPLETE;
			if (np->handler != 0) {
				mcmsg_trace_debug("hrecv ast nxn nosender", 1,
					np->hparam, 0, 0, 0);
				mcmsg_hreq_ast(mt->task, si->nxrq.request);
			}
		    }
		}
		/* the si->value was set to source_pid in mcmsg_recv_prm() */
		mcmsg_remove_sequence(mt, si->value, si);
		return;
	}
	assert(pid_si != 0);
	assert(pid_si->ppid.recv_target != 0 || pid_si->ppid.recv_give == 0);
	assert((pid_si->ppid.recv_give & 0x1f) == 0);
	hdr1 = MCTRL_NXC | (pid_si->ppid.recv_give << 11);
	pid_si->ppid.recv_give = 0;
	hdr2 = 8 | (si->nxrq.sequence << 16);
	hdr3 = mt->pid;
	mcmsg_trace_send(hdr1, hdr2, pid_si->value, 1, si, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2eod(hdr3, pid_si->value);
	return;
}

/*
 *	mcmsg_send_nxf
 *
 *	Send NXF flow control packet.
 */

mcmsg_send_nxf(mt, ctl, pid_si, sequence)
	mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register unsigned long	sequence;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	avail;

	avail = pid_si->ppid.recv_give;
	pid_si->ppid.recv_give = 0;

	assert((avail & 0x1f) == 0);
	hdr1 = MCTRL_NXF | (avail << 11);
	hdr2 = 8 | (sequence << 16);
	mcmsg_trace_send(hdr1, hdr2, pid_si->value, 1, pid_si, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2eod(mt->pid, pid_si->value);
	return;
}

/*
 *	mcmsg_recv_nxs
 *
 *	Receive NXS flow control packet.
 */

mcmsg_recv_nxs(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	dest_pid;
	register long 		dest_ptype;
	register long 		source_ptype;
	register long 		msg_type;
	register unsigned long	sequence;
	register unsigned long	msg_length;
	register unsigned long	msg_size;
	register unsigned long	avail;
	register select_item_t	*pid_si;
	register user_pointer_t	xmsg;
	register xmsg_t		*xp;
	register nxreq_t	*np;
	register unsigned long	*ip;
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register unsigned long	seq_out;
	register unsigned long	originating_node;
	register unsigned long	dummy;

	recv2(source_pid, source_ptype);
	recv2(dest_pid, dest_ptype);
	recv2(msg_type, msg_length);
	recv2(originating_node, dummy);

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, msg_length, 0);

	sequence = hdr2 >> 16;
	if (mcmsg_lookup_sequence(source_pid, sequence) != 0) {
		mcmsg_trace_drop("already got", source_pid);
		return 0;
	}

	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return 0;
	}
	mt = si->mcmsg_task;

	pid_si = mcmsg_lookup_remote(mt, source_pid);
	assert(pid_si != 0);

	/* mark when_recvd */
	pid_si->ppid.when_recvd = ++mcmsg_hw.globtime;

	pid_si->ppid.send_avail += ((hdr1 >> 11) & ~(0x1f));
	assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);

	si = mcmsg_find_nx_recv(mt, 
				msg_type,
				dest_ptype,
				msg_length,
				pid_si->ppid.node,
				source_ptype,
	                        originating_node);

	assert(mt->provided >= mt->assigned);
	avail = mt->provided - mt->assigned;
	msg_size = ((msg_length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));

	if (si != 0) {
		si->method = SELMETH_RECV_ANY;
		si->nxrq.pid_si = pid_si;
		si->nxrq.sequence = sequence;
		si->nxrq.take = 0;
		/* XXX Is the following line necessary ??? */
		si->nxrq.originating_node = originating_node;
		ip = (unsigned long *) mcmsg_validate_long(mt,
			((nxreq_t *)(si->nxrq.request))->localinfo);
		if (ip != 0) {
			ip[0] = msg_type;
			ip[1] = msg_length;
			ip[2] = originating_node;
			ip[3] = source_ptype;
			ip[4] = pid_si->ppid.node;
		}

	} else if (msg_type & FORCE_FLAG) {
		mcmsg_send(mt, MCTRL_NXF, pid_si, hdr2 >> 16);
		return;

	} else if (avail >= msg_size &&
		   (xmsg = mcmsg_alloc_whole_xmsg(mt, msg_length, avail)) != 0) {

		si = mcmsg_alloc_select_item();
		assert(si != 0);
		si->method = SELMETH_RECV_XMSG;
		si->nxrq.xmsg = xmsg;
		si->value = source_pid;
		si->nxrq.pid_si = pid_si;
		si->nxrq.sequence = hdr2 >> 16;
		si->nxrq.buf = 0;
		si->nxrq.count = msg_length;
		si->nxrq.take = 0;
		si->nxrq.stop = msg_length + PKT_GRAN;
		si->nxrq.offset = 0;
		si->nxrq.request = 0;
		si->nxrq.originating_node = originating_node;

		mcmsg_trace_debug("nxs->xmsg", 3, xmsg, msg_size, avail, 0);
		assert(mt->provided >= mt->assigned);
		xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
		xp->length = msg_length;
		xp->si = si;
		xp->dest_ptype = dest_ptype;
		xp->source_node = pid_si->ppid.node;
		xp->originating_node = originating_node;
		xp->source_ptype = source_ptype;
		xp->msg_type = msg_type;

	} else {

		mcmsg_trace_debug("nxs->need", 3,
				msg_length, msg_type, pid_si, 0);
		assert(pid_si->ppid.rk_recv_pid == 0);
		pid_si->ppid.rk_recv_want = msg_length;
		pid_si->ppid.rk_recv_seq = hdr2 >> 16;
		pid_si->ppid.rk_recv_pid = source_pid;
		pid_si->ppid.rk_recv_type = msg_type;
		pid_si->ppid.rk_recv_ptype = source_ptype;
		pid_si->ppid.rk_recv_originating_node = originating_node;
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
		assert(pid_si->method == SELMETH_PID);
		return;
	}
	if (msg_length > 0) {
		mcmsg_install_sequence(mt, source_pid, si);
	} else {
		if (si->method == SELMETH_RECV_ANY) {
			np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
			if (np != 0) {
				np->state = NX_COMPLETE;
				mcmsg_trace_debug("nxs complete", 2,
						si, si->nxrq.request, 0, 0);
			}
		} else {
			mcmsg_trace_debug("nxs xcomplete", 1, xmsg, 0, 0, 0);
			xp->state = XMSG_FULL;
		}
	}
	mcmsg_trace_debug("recv_nxs", 2, pid_si, pid_si->value,0, 0);
	mcmsg_send(mt, MCTRL_NXR, si, si->nxrq.sequence);
}

/*
 *	mcmsg_recv_nxm
 *
 *	Receive NXM flow control packet.
 */

mcmsg_recv_nxm(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	dest_pid;
	register long 		dest_ptype;
	register long 		source_ptype;
	register long 		msg_type;
	register unsigned long	msg_length;
	register unsigned long	msg_size;
	register unsigned long	avail;
	register select_item_t	*pid_si;
	register user_pointer_t	xmsg;
	register xmsg_t		*xp;
	register nxreq_t	*np;
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register unsigned long	seq_in;
	register unsigned long	originating_node;
	register unsigned long	dummy;

	recv2(source_pid, source_ptype);
	recv2(dest_pid, dest_ptype);
	recv2(msg_type, msg_length);
	recv2(originating_node, dummy);

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, msg_length, 0);

	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return 0;
	}
	mt = si->mcmsg_task;

	pid_si = mcmsg_lookup_remote(mt, source_pid);
	assert(pid_si != 0);
	pid_si->ppid.send_avail += ((hdr1 >> 11) & ~(0x1f));
	assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);

	if (pid_si->ppid.recv_total != pid_si->ppid.recv_give ||
	    pid_si->ppid.rk_recv_pid == 0) {
		return;
	}

	si = mcmsg_find_nx_recv(mt,
				msg_type,
				dest_ptype,
				msg_length,
				pid_si->ppid.node,
				source_ptype,
	                        originating_node);

	assert(mt->provided >= mt->assigned);
	avail = mt->provided - mt->assigned;
	msg_size = ((msg_length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));

	if (si != 0) {
		si->method = SELMETH_RECV_ANY;
		si->nxrq.pid_si = pid_si;
		si->nxrq.sequence = hdr2 >> 16;
		si->nxrq.originating_node = originating_node;

	} else if (msg_type & FORCE_FLAG) {
		mcmsg_send(mt, MCTRL_NXF, pid_si, hdr2 >> 16);
		return;
	} else {
		/* ignore NXMs that don't have a matching recv */
		return;
	}

	if (msg_length > 0) {
		mcmsg_install_sequence(mt, source_pid, si);
	} else {
		if (si->method == SELMETH_RECV_ANY) {
			np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
			if (np != 0) {
				np->state = NX_COMPLETE;
				mcmsg_trace_debug("nxm complete", 2,
						si, si->nxrq.request, 0, 0);
			}
		} else {
			mcmsg_trace_debug("nxm xcomplete", 1, xmsg, 0, 0, 0);
			xp->state = XMSG_FULL;
		}
	}
	mcmsg_trace_debug("recv_nxm", 2, pid_si, pid_si->value,0, 0);
	mcmsg_send(mt, MCTRL_NXR, si, si->nxrq.sequence);
}

/*
 *	mcmsg_recv_nxq
 *
 *	Receive NXQ flow control packet.
 */

mcmsg_recv_nxq(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register long 		dest_ptype;
	register unsigned long	dest_pid;
	register long 		source_ptype;
	register long 		msg_type;
	register unsigned long	t;
	register unsigned long	avail;
	register mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register select_item_t	*sh;
	register select_item_t	*st;
	register select_item_t	*si;

	recv2(source_pid, source_ptype);
	recv2(dest_pid, dest_ptype);
	recv2(msg_type, t);

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, msg_type, 0);

	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_drop("app not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mt = si->mcmsg_task;

	pid_si = mcmsg_lookup_remote(mt, source_pid);
	assert(pid_si != 0);
	pid_si->ppid.send_avail += ((hdr1 >> 11) & ~(0x1f));
	assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);

	st = pid_si->ppid.send_wait;
	if (st == 0) {
		mcmsg_trace_drop("no send waiting", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	sh = st->link;
	assert(sh != 0);
	si = st;
	assert((t = MAXLOOP) != 0);
	for (;;) {
		if (sh->nxrq.dest_ptype == dest_ptype &&
		    msg_type == -1 ||
		    sh->nxrq.msg_type == msg_type) {
			break;
		}
		if (sh == si) {
			mcmsg_trace_drop("no send waiting", msg_type);
			mcmsg_msg_drop++;
			return;
		}
		st = sh;
		sh = st->link;
		assert(t-- != 0);
	}
	if (sh == st) {
		pid_si->ppid.send_wait = si = 0;
	} else {
		if (sh == si) {
			pid_si->ppid.send_wait = st;
		}
		st->link = si = sh->link;
		assert(si != 0 && si->method != 0xdead);
	}
	assert(sh == (select_item_t *)sh->item);
	mcmsg_send(sh->mcmsg_task, sh->nextmethod, sh);
	if (si != 0) {
		register method = si->method;

		si->method = 0;
		mcmsg_send(si->mcmsg_task, method, si);
	}
}

/*
 *	mcmsg_recv_nxc
 *
 *	Receive NXC flow control packet.
 */

mcmsg_recv_nxc(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	dest_pid;
	register unsigned long	sequence;
	register unsigned long	give;
	register select_item_t	*pid_si;
	register select_item_t	*sh;
	register select_item_t	*st;
	register select_item_t	*sn;
	register mcmsg_task_t	*mt;
	register int		t;

	recv2(source_pid, dest_pid);
	sequence = hdr2 >> 16;
	give     = ((hdr1 >> 11) & ~(0x1f));

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, dest_pid, 0);

	st = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid);
	if (st == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mt = st->mcmsg_task;
	pid_si = (select_item_t *)(st->item);
	assert(pid_si != 0);

	pid_si = mcmsg_lookup_remote(mt, source_pid);
	assert(pid_si != 0);

	/*
	 * Process give
	 */
	if (give > 0) {
		pid_si->ppid.send_avail += give;
		assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);
		if (pid_si->ppid.send_wait != 0) {
			mcmsg_release_send_wait(mt, pid_si);
		}
	}

	/*
	 * Search for sends waiting.
	 */
	st = pid_si->ppid.send_wait;
	if (st == 0) {
		mcmsg_trace_drop("no send waiting", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	sn = st;
	sh = st->link;
	assert(sh != 0);
	assert((t = MAXLOOP) != 0);
	for (;;) {
		if (sh->nxrq.sequence == sequence) {
			break;
		}
		if (sh == sn) {
			mcmsg_trace_drop("no send seq waiting", sequence);
			mcmsg_msg_drop++;
			return;
		}
		st = sh;
		sh = st->link;
		assert(t-- != 0);
	}

	if (sh == st) {
		pid_si->ppid.send_wait = sn = 0;
	} else {
		if (sh == sn) {
			pid_si->ppid.send_wait = st;
		}
		st->link = sn = sh->link;
	}
		
	assert(sh == (select_item_t *)sh->item);

	/*
	 * Adjust Stop count of the request
	 */
	sh->nxrq.stop = sh->nxrq.offset + (sh->nxrq.take - sizeof(xmsg_t));
	if (sh->nxrq.stop > sh->nxrq.count) {
		sh->nxrq.stop = sh->nxrq.count;
	}

	/*
	 * Continue the send.
	 */
	mcmsg_send_tail(mt, MCTRL_NXN, sh, pid_si,
			sh->nxrq.buf + sh->nxrq.offset,
			sh->nxrq.count - sh->nxrq.offset,
			mt->applinfo.pkt_size,
			sh->nxrq.offset,
			sh->nxrq.stop);
}

/*
 *	mcmsg_recv_nxf
 *
 *	Receive NXF flow control packet.
 */

mcmsg_recv_nxf(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register long		source_pid;
	register unsigned long	dest_pid;
	register unsigned long	sequence;
	register mcmsg_task_t	*mt;
	register select_item_t	*st;
	register select_item_t	*sh;
	register select_item_t	*si;
	register select_item_t	*sn;
	register unsigned long	t;
	register select_item_t	*pid_si;
	register long		pid;

	recv2(source_pid, dest_pid);
	sequence = hdr2 >> 16;

	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mt = si->mcmsg_task;

	pid_si = mcmsg_lookup_remote(mt, source_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, mt, pid_si);
	assert(pid_si != 0);
	assert(pid_si->ppid.send_ready);
	pid_si->ppid.send_avail += ((hdr1 >> 11) & ~(0x1f));
	assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);

	/*
	 * Search queued sends
	 */

	si = (select_item_t *)mcmsg_clear_force_sends(pid_si, sequence);

	/*
	 * Search send_wait
	 */

	st = pid_si->ppid.send_wait;
	if (st != 0) {
		sn = st;
		sh = st->link;
		assert(sh != 0);
		assert((t = MAXLOOP) != 0);
		for (;;) {
			if (sh->nxrq.sequence == sequence) {
				if (sh == st) {
					pid_si->ppid.send_wait = sn = 0;
				} else {
					if (sh == sn) {
					    pid_si->ppid.send_wait = st;
					}
					st->link = sn = sh->link;
					assert(sn != 0 &&
					       sn->method != 0xdead);
				}
				mcmsg_trace_debug("nxf done", 2,
						  sh, sn, 0, 0);
				si = sh;
				if (sn != 0 && sn->method != 0) {
					register method = sn->method;

					sn->method = 0;
					mcmsg_send(mt, method, sn);
				}
				break;
			}
			if (sh == sn) {
				break;
			}
			st = sh;
			sh = st->link;
			assert(t-- != 0);
		}
	}
	if (si != 0) {
		nxreq_t	*np;

		np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
		if (np != 0) {
			np->state = NX_COMPLETE;

			mcmsg_trace_debug("nxf complete", 2, si, si->nxrq.request, 0, 0);
			/* check for handler request */
			if (np->handler != 0) {
				mcmsg_hreq_ast(mt->task, si->nxrq.request);
			}
		}
		mcmsg_free_select_item(si);
	}
}

/*
 *	mcmsg_send_nxa
 *
 *	NX give buffer space to node.
 */

mcmsg_send_nxa(mt, ctl, pid_si)
	mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;

	assert((pid_si->ppid.recv_give & 0x1f) == 0);
	hdr1 = MCTRL_NXA | (pid_si->ppid.recv_give << 11);
	pid_si->ppid.recv_give = 0;
	hdr2 = 8;
	hdr3 = mt->pid;
	mcmsg_trace_send(hdr1, hdr2, pid_si->value, 2, pid_si, pid_si->ppid.recv_total);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2eod(hdr3, pid_si->value);
	return 0;
}

/*
 *	mcmsg_send_nxr
 *
 *	Send NX release flow control packet.
 */

mcmsg_send_nxr(mt, ctl, si, sequence)
	mcmsg_task_t	*mt;
	register select_item_t	*si;
	register unsigned long	sequence;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;
	register nxreq_t        *np;

	pid_si = si->nxrq.pid_si;
	if (pid_si == (select_item_t *) 0xdead) {   /* PTS 11968 */
		/* sender must have terminated */
		if (si->nxrq.request) {
		    np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
		    if (np == 0) {
			mcmsg_trace_drop("send_nxr bad nxreq",si->nxrq.request);
		    } else {
			np->err = NX_ERR_NOSENDER;
			np->state = NX_COMPLETE;
			if (np->handler != 0) {
				mcmsg_trace_debug("hrecv ast nxn nosender", 1,
					np->hparam, 0, 0, 0);
				mcmsg_hreq_ast(mt->task, si->nxrq.request);
			}
		    }
		}
		
		/* the si->value was set to source_pid in mcmsg_recv_prm() */
		mcmsg_remove_sequence(mt, si->value, si);
		return -1;
	}
	assert(pid_si->ppid.recv_target != 0 || pid_si->ppid.recv_give == 0);
	assert((pid_si->ppid.recv_give & 0x1f) == 0);
	mcmsg_trace_debug("send_nxr pid_si seq", 2, pid_si,sequence,0,0);
	hdr1 = MCTRL_NXR | (pid_si->ppid.recv_give << 11);
	pid_si->ppid.recv_give = 0;
	hdr2 = 8 | (sequence << 16);
	mcmsg_trace_send(hdr1, hdr2, pid_si->value, 1, pid_si, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2eod(mt->pid, pid_si->value);
	return 0;
}

/*
 *	mcmsg_recv_nxa
 *
 *	NX receive buffer space.
 */

mcmsg_recv_nxa(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	dest_pid;
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register select_item_t	*pid_si;
	register unsigned int	give;

	recv2(source_pid, dest_pid);

	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("dest pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mt = si->mcmsg_task;

	pid_si = mcmsg_lookup_remote(mt, source_pid);
	if (pid_si == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("source pid not found", source_pid);
		mcmsg_msg_drop++;
		return;
	}
	give = ((hdr1 >> 11) & ~(0x1f));
	pid_si->ppid.send_avail += give;

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, pid_si->ppid.send_avail, 0);
	assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);

	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(mt, pid_si);
	}
}

/*
 *	mcmsg_recv_nxr
 *
 *	Receive NX release flow control packet.
 */

mcmsg_recv_nxr(hdr1, hdr2)
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

	recv2(source_pid, dest_pid);
	sequence = hdr2 >> 16;
	give = ((hdr1 >> 11) & ~(0x1f));

	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mt = si->mcmsg_task;
	pid_si = mcmsg_lookup_remote(mt, source_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, mt, pid_si);
	assert(pid_si != 0);

	pid_si->ppid.send_avail += give;
	assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);


#if BIGPKTS
	{
		extern select_item_t *nx_send_in_progress;
		/*
		 * Check send in progress
		 */

		if (nx_send_in_progress != 0 &&
		    nx_send_in_progress->nxrq.sequence == sequence) {
			nx_send_in_progress->nxrq.stop = 
				nx_send_in_progress->nxrq.count;
			mcmsg_trace_debug("nxr in progress", 2, 
				nx_send_in_progress, nx_send_in_progress->nxrq.sequence, 0, 0);
			goto recv_nxr_exit;
		}
	}
#endif BIGPKTS
	if (!mcmsg_release_send_seq(pid_si, sequence)) {

		/*
		 * Search queued sends
		 */

		mcmsg_release_send_store(pid_si, sequence);
	}

recv_nxr_exit:

	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(mt, pid_si);
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
			if (sh->nxrq.sequence == sequence) {
				/* Update stop value due to receipt of NXR for this message */
				sh->nxrq.stop = sh->nxrq.count;
				mcmsg_trace_debug("rel send seq", 3, sh,
					pid_si, sh->nxrq.vm_ast_pending, 0);
				/* If vm paging request already outstanding, then don't send
				 * now.  This send will be continued from the vm paging thread
				 * after the thread has faulted in pages of the send buffer.
				 */
				if (!sh->nxrq.vm_ast_pending) {
					if (sh == st) {
						pid_si->ppid.send_wait = sn = 0;
					} else {
						if (sh == sn) {
						    pid_si->ppid.send_wait = st;
						}
						st->link = sn = sh->link;
					}

					mcmsg_send(sh->mcmsg_task, MCTRL_NXN,
						sh, pid_si,
						sh->nxrq.buf + sh->nxrq.offset,
						sh->nxrq.count - sh->nxrq.offset,
						sh->mcmsg_task->applinfo.pkt_size,
						sh->nxrq.offset,
						sh->nxrq.stop);
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
mcmsg_release_send_wait(mt, pid_si)
mcmsg_task_t	*mt;
select_item_t *pid_si;
{
	register select_item_t *st,  *sh;
	register unsigned long	t;
	register unsigned long	take;

	if (pid_si->ppid.in_rel_send_wait) {
		mcmsg_trace_debug("relsendwait reentry pid_si",1,pid_si,0,0,0);
		return;
	}
	pid_si->ppid.in_rel_send_wait = 1;

again:
	st = pid_si->ppid.send_wait;
/* Once we call mcmsg_start_nx_send(), it may block allowing recv interrupts 
 * which may (via mcmsg_recv_nxc() etc) remove an item from the send_wait 
 * queue invalidating all our local variables. We must restart the loop with
 * new local variables.
 */
	if (st != 0) {
	        sh = st->link;
		assert((t = MAXLOOP) != 0);
		for (;;) {
			assert(sh != 0);
			assert(sh->method != 0xdead);
			/*
			 * Look for NEW message.
			 */

	    		if (sh->method == MCTRL_NXS) {
				break;
			}
			if (sh->method == 0 &&
			   sh->nextmethod == MCTRL_NX1 &&
			   (!sh->nxrq.vm_ast_pending)) {
				/*
				 * Check if enough avail to start send
				 */
				take = mcmsg_calculate_take(mt,
					(sh->nxrq.count + 2*sizeof(xmsg_t)-1) &
						~(sizeof(xmsg_t)-1),
					pid_si->ppid.send_avail);
				if (take > 0) {
					/*
					 * take message out of queue
					 */
					if (sh == st) {
						pid_si->ppid.send_wait = 0;
					} else {
						if (sh == pid_si->ppid.send_wait) {
						    pid_si->ppid.send_wait = st;
						}
						st->link = sh->link;
					}

					/*
					 * Start the send.
					 */
					mcmsg_trace_debug("rel send wait",2, sh,
						pid_si, 0, 0);
					mcmsg_start_nx_send(mt,sh,pid_si,take);
					if (pid_si->ppid.send_wait == 0) 
						break;
					/* restart the loop */
					st = pid_si->ppid.send_wait;
					sh = st->link;
					assert((t = MAXLOOP) != 0);
					continue; /* goto again */
				} else {
					pid_si->ppid.in_rel_send_wait = 0;
					return;	
				}
			}
			if (pid_si->ppid.send_wait == 0) 
				break;
			if (sh == pid_si->ppid.send_wait)
				break;
			st = sh;
			sh = st->link;
			assert(t-- != 0);
		}
	}
	pid_si->ppid.in_rel_send_wait = 0;
	return;
}
