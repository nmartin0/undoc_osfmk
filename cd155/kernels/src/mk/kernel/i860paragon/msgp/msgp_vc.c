/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#define mcmsg_eager_send mcmsg_send
#define mcmsg_eager_send_tail mcmsg_send_tail
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_vc.c,v 1.4 1994/11/18 20:48:04 mtm Exp $

/*
 * msgp_vc.c
 *
 * VC messages
 */

#define	MCMSG_MODULE	MCMSG_MODULE_VC

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_vc.h>

mcmsg_vc_open(mt, node, tag)
	mcmsg_task_t		*mt;
	unsigned long		node;
	long			tag;
{
	register int		app;
	register long		dest_pid;
	register long		i;
	register long		j;
	register long		n;
	register select_item_t	*pid_si;
	register select_item_t	*si;

	if (node < 0 || node > mt->numnodes) {
		return -1;
	}

	/*
	 * Find a free table entry
	 */

	n = mt->vc_base + mt->vc_limit - 1;
	for (i = mt->vc_low_free; vctab[i].state != VC_STATE_FREE; i++) {
		if (i == n) {
			return -2;
		}
	}

	/*
	 * Fill in and initialize table entry
	 */

	vctab[i].state = VC_STATE_TAGGED;
	vctab[i].remote_index = 0;
	vctab[i].route = 0;
	vctab[i].local_freeze = 0;
	vctab[i].remote_freeze = 0;
	vctab[i].freeze_int = 0;
	vctab[i].send_slot = 0;
	vctab[i].recv_slot = 0;
	vctab[i].srdy_slot = 0;
	vctab[i].rrdy_slot = 0;
	vctab[i].send_req_ready_count = tag;
	for (j = 0; j < VC_NREQ; j++) {
		vctab[i].send_req[j].active = 0;
		vctab[i].recv_req[j].active = 0;
	}

	/*
	 * Exchange indices with remote node
	 */

	dest_pid = mcmsg_remote_pid(mt, node, 0);
	if (dest_pid != -1) {
		pid_si = mcmsg_lookup_remote(mt, dest_pid);
	} else {
		pid_si = 0;
	}

	si = mcmsg_alloc_select_item();
	assert(si != 0);
	si->item = (void *)si;
	si->nxrq.request = 0;
	si->value = MCMSG_MODULE_VC;
	si->method = 0;
	si->nextmethod = MCTRL_VCO;
	si->mcmsg_task = mt;
	si->nxrq.pid_si = pid_si;
	si->nxrq.dest_node = node;
	si->nxrq.dest_ptype = 0;
	si->nxrq.source_ptype = 0;
	si->nxrq.msg_type = tag;/* Tag */
	si->nxrq.buf = 0;
	si->nxrq.count = i;	/* Index */
	si->nxrq.offset = 1;	/* Flag: want other end to return its VCO */
	si->nxrq.stop = 0;
	si->nxrq.take = 0;
	si->nxrq.sequence = 0;
	si->nxrq.xmsg = 0;
	si->nxrq.vm_ast_pending = 0;

	vctab[i].recv_req[0].buf = (unsigned long)si;

	if (pid_si != 0) {
		mcmsg_vc_sendopen(mt, si);
	} else {
		mcmsg_inquire(mt, si);
	}
	return i - mt->vc_base;
}

mcmsg_vc_sendopen(mt, si)
	register select_item_t	*si;
	register mcmsg_task_t	*mt;
{
	register select_item_t	*st;
	register select_item_t	*sh;

	mcmsg_send(mt, MCTRL_VCO, si);
}

mcmsg_send_vco(mt, ctl, si)
	mcmsg_task_t	*mt;
	select_item_t		*si;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	hdr1 = MCTRL_VCO | si->nxrq.offset << 16;
	hdr2 = 24;
	hdr3 = mt->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2, si->nxrq.count, pid_si->ppid.node);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, pid_si->value);
	send2(ipsc_physnode, si->nxrq.count);
	send2eod(si->nxrq.msg_type, 0);
	if (!si->nxrq.offset){
		mcmsg_free_select_item(si);
	}
	return;
}

mcmsg_recv_vco(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	dest_pid;
	register long 		node;
	register long 		index;
	register long 		tag;
	register unsigned long	dummy;
	register select_item_t	*pid_si;
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register int		i;
	register int		n;

	recv2(source_pid, dest_pid);
	recv2(node, index);
	recv2(tag, dummy);

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, index, node);

	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mt = si->mcmsg_task;

	/*
	 * See if our open is there yet
	 */

	n = mt->vc_base + mt->vc_limit;
	for (i = mt->vc_base; i < n; i++) {
		if (vctab[i].state == VC_STATE_TAGGED &&
		    vctab[i].send_req_ready_count == tag) {
			mcmsg_trace_debug("VCO match", 2, i, node, 0, 0);
			vctab[i].state = VC_STATE_ACTIVE;
			vctab[i].remote_index = index;
			vctab[i].route = calculate_route(node);
			vctab[i].send_req_ready_count = 0;

			/*
			 * Clear send status word in user status buffer
			 */

			{
				register long *sbp;

				sbp = (long *)mcmsg_validate_long(mt,
				 mt->vc_stbuf + 8*(i - mt->vc_base) + 0);
				if (sbp != 0) {
					*sbp = 0;
				} else {
					mcmsg_trace_drop("vco no stbuf", i);
				}
			}

			si = (select_item_t *)vctab[i].recv_req[0].buf;
			if (hdr1 >> 16) {
				si->nxrq.offset = 0; /* Don't want reply */
				mcmsg_send(mt, MCTRL_VCO, si);
			} else {
				mcmsg_free_select_item(si);
			}
			return;
		}
	}
	return;
}


mcmsg_vc_send(mt, conn, buf, count)
	mcmsg_task_t		*mt;
	int			conn;
	unsigned long		buf;
	unsigned long		count;
{
	register vc_req_t	*req;
	register vc_entry_t	*vc;

	if (conn >= mt->vc_limit) {
		return -1;
	}
	vc = &vctab[conn + mt->vc_base];
	if (vc->local_freeze || vc->remote_freeze) {
		return -2;
	}
	req = &(vc->send_req[vc->send_slot]);
	if (req->active) {
		return -3;
	}
	req->buf = buf;
	req->count = count;
	req->active = 1;
	{
		register slot = vc->send_slot + 1;
		if (slot < VC_NREQ) {
			vc->send_slot = slot;
		} else {
			vc->send_slot = 0;
		}
	}

	if (vc->send_req_ready_count > 0) {
	    mcmsg_eager_send(mt, MCTRL_VCD, vc, &(vc->send_req[vc->srdy_slot]));
	}
	return 0;
}

mcmsg_vc_recv(mt, conn, buf, count)
	mcmsg_task_t		*mt;
	int			conn;
	unsigned long		buf;
	unsigned long		count;
{
	register vc_req_t	*req;
	register vc_entry_t	*vc;

	if (conn >= mt->vc_limit) {
		return -1;
	}
	vc = &vctab[conn + mt->vc_base];
	if (vc->local_freeze || vc->remote_freeze) {
		return -2;
	}
	req = &(vc->recv_req[vc->recv_slot]);
	if (req->active) {
		return -3;
	}
mcmsg_trace_debug("vc recv", 1, vc->recv_slot, 0, 0, 0);
	req->buf = buf;
	req->count = count;
	req->active = 1;
	{
		register slot = vc->recv_slot + 1;
		if (slot < VC_NREQ) {
			vc->recv_slot = slot;
		} else {
			vc->recv_slot = 0;
		}
	}

	mcmsg_send(mt, MCTRL_VCR, vc, req);
	return 0;
}

mcmsg_send_vcr(mt, ctl, vc, req)
	mcmsg_task_t	*mt;
	register vc_req_t	*req;
	register vc_entry_t	*vc;
{
	register unsigned long	hdr1;
	register unsigned long	count;

	hdr1 = MCTRL_VCR | (vc->remote_index << 16);
	count = req->count;
	mcmsg_trace_send(hdr1, count, 0, 2, vc-vctab, req - vc->recv_req);
	send2(vc->route, 0);
	send2eod(hdr1, count);
	return;
}

mcmsg_recv_vcr(hdr1, count)
	register unsigned long	hdr1;
	register unsigned long	count;
{
	mcmsg_task_t	*mt;
	register vc_entry_t	*vc;
	register vc_req_t	*req;

	vc = &vctab[hdr1 >> 16];
	mt = vc->mcmsg_task;
	mcmsg_trace_recv(hdr1, count, 0, 2, hdr1 >> 16, vc->srdy_slot);
	vc->send_req_ready_count += count;
	if (vc->local_freeze || vc->remote_freeze) {
		return;
	}

	req = &(vc->send_req[vc->srdy_slot]);
	if (req->active) {
		mcmsg_eager_send(mt, MCTRL_VCD, vc, req);
	}
	return;
}

mcmsg_send_vcd(mt, ctl, vc, req)
	mcmsg_task_t	*mt;
	register vc_entry_t	*vc;
	register vc_req_t	*req;
{
	register unsigned long	hdr1;
	register unsigned long	count;
	register unsigned long	pkt;
	register unsigned long	buf;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register int		srdy_slot;

	buf = req->buf;
	count = mt->applinfo.pkt_size;
	if (count > vc->send_req_ready_count) {
		count = vc->send_req_ready_count;
	}
	if (count < req->count) {
		req->buf += count;
		req->count -= count;
		vc->send_req_ready_count -= count;
		hdr1 = MCTRL_VCD | (vc->remote_index << 16);

		/* verify wired buffer */

		bp1 = mcmsg_validate_read1(buf, count, mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			mcmsg_trace_drop("vc send buf", buf);
			return;
		}

		mcmsg_trace_send(hdr1, count, 0, 2, vc-vctab, req);
#if 1
		mcmsg_send_pkt2(mt, 0, bp1, bp2, count, vc->route, hdr1, count);
#else
		pkt = (count + PKT_GRAN-1) & ~(PKT_GRAN-1);
		send2(vc->route, 0);
		send2(hdr1, count);
		mcmsg_send_buf(bp1, bp2, pkt);
#endif
		if (vc->send_req_ready_count > 0) {
			assert(req->active);
			mcmsg_eager_send_tail(mt, MCTRL_VCD, vc, req);
		}

	} else {

		count = req->count;
		vc->send_req_ready_count -= count;
		hdr1 = MCTRL_VCD | (vc->remote_index << 16);
		pkt = (count + PKT_GRAN-1) & ~(PKT_GRAN-1);

		/* verify wired buffer */

		bp1 = mcmsg_validate_read1(buf, count, mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			mcmsg_trace_drop("vc send buf", buf);
			return;
		}

		mcmsg_trace_send(hdr1, count, 0, 2, vc-vctab, req);
		send2(vc->route, 0);
		send2(hdr1, count);
		mcmsg_send_buf(bp1, bp2, pkt);

		/*
		 * Increment send status word in user status buffer
		 */

		{
			register long *sbp;

			sbp = (long *)mcmsg_validate_long(mt,
			 mt->vc_stbuf + 8*((vc - vctab) - mt->vc_base) + 0);
			if (sbp != 0) {
				(*sbp)++;
			} else {
				mcmsg_trace_drop("vc send no stbuf", vc-vctab);
			}
		}
		req->active = 0;

		/*
		 * Step to the next send slot
		 */

		srdy_slot = vc->srdy_slot + 1;
		if (srdy_slot == VC_NREQ) {
			srdy_slot = 0;
		}
		vc->srdy_slot = srdy_slot;
		if (vc->send_req_ready_count > 0) {
			req = &(vc->send_req[srdy_slot]);
			if (req->active) {
				mcmsg_eager_send_tail(mt, MCTRL_VCD, vc, req);
			}
		}
	}
	return;
}

mcmsg_recv_vcd(hdr1, count)
	register unsigned long	hdr1;
	register unsigned long	count;
{
	mcmsg_task_t	*mt;
	register vc_entry_t	*vc;
	register vc_req_t	*req;
	register unsigned long	buf;
	register unsigned long	bp1;
	register unsigned long	bp2;

	vc = &vctab[hdr1 >> 16];
	mt = vc->mcmsg_task;
	mcmsg_trace_recv(hdr1, count, 0, 2, hdr1 >> 16, vc->rrdy_slot);
	do {
		req = &(vc->recv_req[vc->rrdy_slot]);
		assert(req->active);
		buf = req->buf;
		if (req->count > count) {

			req->buf += count;
			req->count -= count;
			bp1 = mcmsg_validate_read1(buf, count, mt->dirbase);
			bp2 = mcmsg_validate2();
			if (bp1 == 0) {
				mcmsg_trace_drop("vc recv buf", buf);
				mcmsg_fifo_flush(count);
				return;
			}
#if 1
			mcmsg_recv_buf_even(bp1, bp2, count);
#else
			mcmsg_recv_buf(bp1, bp2, count);
#endif
			return;

		} else {
			register unsigned long	n;

			n = req->count;
			req->active = 0;
			{
				register slot = vc->rrdy_slot + 1;
				if (slot < VC_NREQ) {
					vc->rrdy_slot = slot;
				} else {
					vc->rrdy_slot = 0;
				}
			}

			/*
			 * Increment receive status word in user status buffer
			 */

			{
				register long *sbp;

				sbp = (long *)mcmsg_validate_long(mt,
				 mt->vc_stbuf + 8*((vc - vctab) - mt->vc_base) + 4);
				if (sbp != 0) {
					(*sbp)++;
				} else {
				 mcmsg_trace_drop("vc recv no stbuf", vc-vctab);
				}
			}

			bp1 = mcmsg_validate_read1(buf, n, mt->dirbase);
			bp2 = mcmsg_validate2();
			if (bp1 == 0) {
				mcmsg_trace_drop("vc recv buf", buf);
				return;
			}
			mcmsg_recv_buf(bp1, bp2, n);
			count -= n;
		}
	} while (count > 0);

	return;
}
