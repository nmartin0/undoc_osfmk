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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_urdma.c,v 1.4 1995/03/21 21:04:04 lenb Exp $

/*
 * msgp_urdma.c
 *
 * URDMA message module
 */

#define	MCMSG_MODULE	MCMSG_MODULE_URDMA

#include <i860paragon/msgp/msgp_hw.h>
/*
#include <i860paragon/mcmsg/mcmsg_nx.h>
*/

#define	URDMA_SLOTS	100
#define URDMA_MAXACTIVE	20
#define URDMA_MAXREQ	10

#if	BIGPKTS
#define URDMA_PKT	8192
#else	BIGPKTS
#define URDMA_PKT	1728
#endif	BIGPKTS

extern int	urdma_slots;


#define URDMA_INCREQ(slot) {	int	s = slot + 1; \
				if (s < URDMA_MAXREQ) { \
					slot = s; \
				} else { \
					slot = 0; \
				} \
			   }

typedef struct urdma_req {
	unsigned long	buf;
	unsigned long	count;
} urdma_req_t;

#define URDMA_REQOK		16
#define URDMA_CONNECTED		32
#define	URDMA_STATE_DISCO	0
#define URDMA_STATE_WAIT	(URDMA_CONNECTED + 1)
#define	URDMA_STATE_ACCEPT	(URDMA_REQOK + 2)
#define	URDMA_STATE_READY	(URDMA_REQOK + URDMA_CONNECTED + 3)
#define URDMA_STATE_STOP	(URDMA_REQOK + URDMA_CONNECTED + 4)
#define URDMA_STATE_FLUSH	(URDMA_REQOK + URDMA_CONNECTED + 5)

typedef struct urdma_entry {
	pt_entry_t	*dirbase;
	mcmsg_task_t	*mcmsg_task;
	unsigned long	app;
	unsigned short	*statusp;
	unsigned long	node;
	unsigned long	route;
	unsigned short	remote_index;
	unsigned char	state;
	unsigned char	sending;

	unsigned short	send_status;
	unsigned short	recv_status;

	unsigned long	send_slot;
	unsigned long	recv_slot;
	unsigned long	srdy_slot;
	unsigned long	rrdy_slot;

	unsigned long	send_ready_in;
	unsigned long	send_ready_out;

	unsigned long	send_req_ready_count[URDMA_MAXREQ];
	
	urdma_req_t	send_req[URDMA_MAXREQ];
	urdma_req_t	recv_req[URDMA_MAXREQ];
} urdma_entry_t;

int		urdma_slots;
urdma_entry_t	*urdma_tab;
unsigned long	urdma_tabsize;

unsigned long	urdma_blank_hdr2;
unsigned long	urdma_full_hdr2;

int		urdma_active;
mcmsg_task_t	*urdma_active_mt[URDMA_MAXACTIVE];

mcmsg_urdma_init()
{
	int i;

	urdma_slots = getbootint("BOOT_URDMA_SLOTS", URDMA_SLOTS);
	urdma_tab = 0;
	urdma_active = 0;
	urdma_blank_hdr2 = ipsc_physnode << 16;
	urdma_full_hdr2 = urdma_blank_hdr2 | URDMA_PKT;
	return;
}

mcmsg_validate_wired(a, dirbase)
	unsigned long	a;
	unsigned long	*dirbase;
{
	pt_entry_t	pt;

	pt = dirbase[pdenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		return 0;
	}
	pt = ((pt_entry_t *)(pt & INTEL_PTE_PFN))[ptenum(a)];
	if ( (pt & (INTEL_PTE_VALID | INTEL_PTE_WIRED)) != 
	           (INTEL_PTE_VALID | INTEL_PTE_WIRED) ) {
		return 0;
	}

	pt = (pt & INTEL_PTE_PFN) | (a & ~(INTEL_PTE_PFN));
	return pt;
}

mcmsg_urdma_setup(mt, nslots, statusp)
	mcmsg_task_t	*mt;
	unsigned	nslots;
	user_pointer_t	statusp;
{
	short	errcode;
	short	*psp;
	int	i, j;

	/*
	 * Translate status pointer
	 */

	if (statusp & 0x03) {
		mcmsg_trace_drop("urdma setup unaligned statusp", statusp);
		return -1;
	}
	psp = (short *)mcmsg_validate_wired(statusp, mt->dirbase);
	if (psp == 0) {
		mcmsg_trace_drop("urdma setup bad statusp", statusp);
		return -1;
	}

	if (urdma_active == URDMA_MAXACTIVE) {
		errcode = -2;
		goto err;
	}
	if (nslots == 0 || nslots > urdma_slots) {
		errcode = -3;
		goto err;
	}
	if (urdma_tab == 0) {
		urdma_tabsize = l2size(urdma_slots*sizeof(urdma_entry_t));
		urdma_tab = (urdma_entry_t *)
		     mcmsg_l2malloc(urdma_tabsize);
		if (urdma_tab == 0) {
			errcode = -4;
			goto err;
		}
		for (i = 0; i < urdma_slots; i++) {
			urdma_tab[i].mcmsg_task = 0;
			urdma_tab[i].app = 0;
			urdma_tab[i].dirbase = 0;
			urdma_tab[i].statusp = 0;
			mcmsg_urdma_disconnect(0, i);
		}
	}

	j = 0;
	for (i = 0; i < urdma_slots; i++) {
		if (i - j == nslots) {
			errcode = j;
			for (i = 0; i < nslots; i++) {
				urdma_tab[i + j].statusp = (short *)
					mcmsg_validate_wired(statusp + 4*i,
							     mt->dirbase);
				if (urdma_tab[i + j].statusp == 0) {
					errcode = -5;
					goto err;
				}
			}
			for (i = 0; i < nslots; i++) {
				urdma_tab[i + j].mcmsg_task = mt;
				urdma_tab[i + j].app = mt->applinfo.app;
				urdma_tab[i + j].dirbase = mt->dirbase;
			}
			urdma_active_mt[urdma_active++] = mt;
			*psp = j;
			return j;
		}
		if (urdma_tab[i].mcmsg_task != 0) {
			j = i+1;
		}
	}
	errcode = -6;

err:
	mcmsg_trace_drop("urdma setup error", -errcode);
	psp[0] = errcode;
	return errcode;
}

mcmsg_urdma_destroy_task(mt)
	mcmsg_task_t		*mt;
{
	int			slot;

	if (urdma_tab == 0) {
		return;
	}

	for (slot = 0; slot < urdma_slots; slot++) {
		if (urdma_tab[slot].mcmsg_task == mt) {
			mcmsg_urdma_disconnect(mt, slot);
			urdma_tab[slot].mcmsg_task = 0;
			urdma_tab[slot].app = 0;
			urdma_tab[slot].dirbase = 0;
			urdma_tab[slot].statusp = 0;
		}
	}
	for (slot = 0; slot < urdma_active; slot++) {
		if (urdma_active_mt[slot] == mt) {
			urdma_active--;
			urdma_active_mt[slot] =
				urdma_active_mt[urdma_active];
			if (urdma_active == 0) {
				mcmsg_l2free(urdma_tab, urdma_tabsize);
				urdma_tab = 0;
			}
		}
	}
}

mcmsg_urdma_accept(mt, slot)
	mcmsg_task_t		*mt;
	unsigned		slot;
{
	urdma_entry_t	*urdma;
	urdma_req_t	*req;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	/*
	 * Fill in and initialize table entry
	 */

	if (slot >= urdma_slots) {
		mcmsg_trace_drop("urdma accept bad slot", slot);
		return -1;
	}
	urdma = &urdma_tab[slot];
	if (urdma->mcmsg_task != mt) {
		mcmsg_trace_drop("urdma accept wrong slot", slot);
		return -1;
	}
	if (urdma->state & URDMA_REQOK) {
		mcmsg_trace_drop("urdma accept already in use", slot);
		return -1;
	}
	if (urdma->state == URDMA_STATE_DISCO) {
		urdma->state = URDMA_STATE_ACCEPT;
	} else if (urdma->state == URDMA_STATE_WAIT) {
		urdma->state = URDMA_STATE_READY;
	} else {
		mcmsg_trace_drop("urdma accept bad state", urdma->state);
		return -1;
	}
	urdma->send_status = 0;
	urdma->recv_status = 0;
	return 0;
}

mcmsg_urdma_connect(mt, slot, token)
	mcmsg_task_t		*mt;
	unsigned		slot;
	unsigned long		token;
{
	urdma_entry_t	*urdma;
	unsigned long	node;
	unsigned long	dest_slot;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	/*
	 * Fill in and initialize table entry
	 */

	node = token >> 16;
	dest_slot = token & 0xFFFF;
	if (slot >= urdma_slots) {
		mcmsg_trace_drop("urdma connect bad slot", slot);
		return -1;
	}
	urdma = &urdma_tab[slot];
	if (urdma->mcmsg_task != mt) {
		mcmsg_trace_drop("urdma connect wrong slot", slot);
		return -1;
	}
	if (node > mt->task_ptype_list->numnodes) {
		mcmsg_trace_drop("urdma connect bad node", node);
		return -1;
	}
	if (dest_slot >= urdma_slots) {
		mcmsg_trace_drop("urdma connect bad dest slot", dest_slot);
		return -2;
	}
	if (urdma->state != URDMA_STATE_DISCO) {
		mcmsg_trace_drop("urdma connect already attached", slot);
		return -3;
	}
	node = mt->task_ptype_list->phys_node_list[node];
	urdma->state = URDMA_STATE_READY;
	urdma->remote_index = dest_slot;
	urdma->node = node;
	urdma->route = calculate_route(node);
	urdma->send_status = 0;
	urdma->recv_status = 0;
	mcmsg_send(mt, MCTRL_URDMAC, urdma);
}

mcmsg_urdma_flush(mt, slot)
	mcmsg_task_t		*mt;
	long			slot;
{
	long		i;
	urdma_entry_t	*urdma;
	urdma_req_t	*req;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	if (slot >= urdma_slots) {
		mcmsg_trace_drop("urdma flush bad slot", slot);
		return -1;
	}
	urdma = &urdma_tab[slot];
	if (urdma->mcmsg_task != mt) {
		mcmsg_trace_drop("urdma flush wrong slot", slot);
		return -1;
	}
	if (urdma->state != URDMA_STATE_ACCEPT) {
		mcmsg_trace_drop("urdma flush bad state", urdma->state);
		return -1;
	}
	i = urdma->srdy_slot;
	for (;;) {
		req = &urdma->send_req[i];
		if (req->count == 0) {
			break;
		}
		req->count = 0;
		urdma->send_status++;
		(urdma->statusp)[0] = urdma->send_status;
		i++;
		if (i == URDMA_MAXREQ) {
			i = 0;
		}
	}
}

mcmsg_urdma_disconnect(mt, slot)
	mcmsg_task_t		*mt;
	unsigned 		slot;
{
	urdma_entry_t	*urdma;
	long		i;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	if (slot >= urdma_slots) {
		mcmsg_trace_drop("urdma disconnect bad slot", slot);
		return -1;
	}
	urdma = &urdma_tab[slot];
	if (urdma->mcmsg_task != mt) {
		mcmsg_trace_drop("urdma disconnect wrong slot", slot);
		return -1;
	}
	urdma->state = URDMA_STATE_DISCO;
	urdma->sending = 0;
	urdma->remote_index = 0;
	urdma->node = 0;
	urdma->route = 0;
	urdma->send_slot = 0;
	urdma->recv_slot = 0;
	urdma->srdy_slot = 0;
	urdma->rrdy_slot = 0;
	urdma->send_ready_in = 0;
	urdma->send_ready_out = 0;
	for (i = 0; i < URDMA_MAXREQ; i++) {
		urdma->send_req_ready_count[i] = 0;
		urdma->recv_req[i].count = 0;
		urdma->send_req[i].count = 0;
	}
}

mcmsg_urdma_send(mt, slot, buf, count)
	mcmsg_task_t		*mt;
	unsigned		slot;
	unsigned long		buf;
	unsigned long		count;
{
	urdma_req_t	*req;
	urdma_entry_t	*urdma;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	if (slot >= urdma_slots) {
		mcmsg_trace_drop("urdma send bad slot", slot);
		return -1;
	}
	urdma = &urdma_tab[slot];
	if (urdma->mcmsg_task != mt) {
		mcmsg_trace_drop("urdma send wrong slot", slot);
		return -2;
	}
	if ((urdma->state & URDMA_REQOK) == 0) {
		mcmsg_trace_drop("urdma send wrong slot", slot);
		return -3;
	}
	if (count == 0) {
		mcmsg_trace_drop("urdma send zero count", count);
		return -4;
	}
	req = &(urdma->send_req[urdma->send_slot]);
	if (req->count != 0) {
		mcmsg_trace_drop("urdma send too many", slot);
		return -5;
	}
	req->buf = buf;
	req->count = count;
	URDMA_INCREQ(urdma->send_slot);
	if (urdma->state == URDMA_STATE_READY &&
	    !urdma->sending &&
	    urdma->send_ready_in != urdma->send_ready_out) {
mcmsg_trace_debug("urdma send start", 2, urdma-urdma_tab, urdma->srdy_slot, 0, 0);
		urdma->sending = 1;
		mcmsg_send_tail(mt,
				MCTRL_URDMAD,
				urdma,
				&(urdma->send_req[urdma->srdy_slot]));
		return;
	}
	if (urdma->state == URDMA_STATE_FLUSH) {
		req->count = 0;
		urdma->send_status++;
		(urdma->statusp)[0] = urdma->send_status;
	}
}

mcmsg_urdma_recv(mt, slot, buf, count)
	mcmsg_task_t		*mt;
	unsigned long		slot;
	unsigned long		buf;
	unsigned long		count;
{
	urdma_req_t	*req;
	urdma_entry_t	*urdma;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	if (slot >= urdma_slots) {
		mcmsg_trace_drop("urdma recv bad slot", slot);
		return -1;
	}
	urdma = &urdma_tab[slot];
	if (urdma->mcmsg_task != mt) {
		mcmsg_trace_drop("urdma recv wrong slot", slot);
		return -2;
	}
	if ((urdma->state & URDMA_REQOK) == 0) {
		mcmsg_trace_drop("urdma recv wrong slot", slot);
		return -3;
	}
	if (count == 0) {
		mcmsg_trace_drop("urdma recv zero count", count);
		return -4;
	}
	req = &(urdma->recv_req[urdma->recv_slot]);
	if (req->count != 0) {
		mcmsg_trace_drop("urdma recv too many", slot);
		return -5;
	}
	if (urdma->state == URDMA_STATE_FLUSH) {
		return 0;
	}
	req->buf = buf;
	req->count = count;
	URDMA_INCREQ(urdma->recv_slot);
	if (urdma->state == URDMA_STATE_READY) {
		mcmsg_send_tail(mt, MCTRL_URDMAR, urdma, count);
	}
}

mcmsg_urdma_accept_send(mt, slot, buf, count)
	mcmsg_task_t		*mt;
	unsigned long		slot;
	unsigned long		buf;
	unsigned long		count;
{

	mcmsg_urdma_accept(mt, slot);
	mcmsg_urdma_send(mt, slot, buf, count);
}

mcmsg_urdma_accept_recv(mt, slot, buf, count)
	mcmsg_task_t		*mt;
	unsigned long		slot;
	unsigned long		buf;
	unsigned long		count;
{

	mcmsg_urdma_accept(mt, slot);
	mcmsg_urdma_recv(mt, slot, buf, count);
}

mcmsg_urdma_reconnect_send(mt, slot, token, buf, count)
	mcmsg_task_t		*mt;
	unsigned long		slot;
	unsigned long		token;
	unsigned long		buf;
	unsigned long		count;
{

	mcmsg_urdma_disconnect(mt, slot);
	mcmsg_urdma_connect(mt, slot, token);
	mcmsg_urdma_send(mt, slot, buf, count);
}

mcmsg_urdma_reconnect_recv(mt, slot, token, buf, count)
	mcmsg_task_t		*mt;
	unsigned long		slot;
	unsigned long		token;
	unsigned long		buf;
	unsigned long		count;
{

	mcmsg_urdma_disconnect(mt, slot);
	mcmsg_urdma_connect(mt, slot, token);
	mcmsg_urdma_recv(mt, slot, buf, count);
}

mcmsg_urdma_put(mt, slot, token, sbuf, rbuf, count)
	mcmsg_task_t		*mt;
	unsigned long		slot;
	unsigned long		token;
	user_pointer_t		sbuf;
	user_pointer_t		rbuf;
	unsigned long		count;
{
	urdma_entry_t		*urdma;
	unsigned long		node;
	unsigned long		dest_slot;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	/*
	 * Fill in and initialize table entry
	 */

	node = token >> 16;
	dest_slot = token & 0xFFFF;
	if (slot >= urdma_slots) {
		mcmsg_trace_drop("urdma put bad slot", slot);
		return -1;
	}
	urdma = &urdma_tab[slot];
	if (urdma->mcmsg_task != mt) {
		mcmsg_trace_drop("urdma put wrong slot", slot);
		return -1;
	}
	if (node > mt->task_ptype_list->numnodes) {
		mcmsg_trace_drop("urdma put bad node", node);
		return -1;
	}
	if (dest_slot >= urdma_slots) {
		mcmsg_trace_drop("urdma put bad dest slot", dest_slot);
		return -2;
	}
	if (urdma->state != URDMA_STATE_ACCEPT) {
		mcmsg_trace_drop("urdma put wrong state", slot);
		return -3;
	}
	node = mt->task_ptype_list->phys_node_list[node];
	urdma->route = calculate_route(node);
	mcmsg_send(mt, MCTRL_URDMAP,
		   urdma, urdma->route,
		   MCTRL_URDMAP | (dest_slot << 16),
		   sbuf, rbuf, count);
}

mcmsg_send_urdmac(mt, ctl, urdma)
	mcmsg_task_t	*mt;
	int		ctl;
	urdma_entry_t	*urdma;
{
	unsigned long	hdr1;
	unsigned long	hdr2;
	int		app;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	hdr1 = MCTRL_URDMAC | (urdma->remote_index << 16);
	hdr2 = urdma - urdma_tab | ipsc_physnode << 16;
	app = urdma->app;
	mcmsg_trace_send(hdr1, hdr2, urdma->node, 2, urdma-urdma_tab, app);
	mcmsg_send_hdr4_eod(urdma->route, hdr1, hdr2, app, 0);
}

mcmsg_recv_urdmac(hdr1, hdr2)
	unsigned long	hdr1;
	unsigned long	hdr2;
{
	int		app;
	int		i;
	urdma_entry_t	*urdma;
	urdma_req_t	*req;
	unsigned long	count;

	recv_hdr4(app, i);
	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	mcmsg_trace_recv(hdr1, hdr2, hdr2 >> 16, 2, hdr1 >> 16, app);
	urdma = &urdma_tab[hdr1 >> 16];
	if (urdma->mcmsg_task == 0 ||
	    urdma->app != app) {
		mcmsg_trace_drop("urdma connect wrong app", urdma->app);
		return;
	}
	if (urdma->state == URDMA_STATE_DISCO) {
		urdma->remote_index = hdr2 & 0xFFFF;
		urdma->node = hdr2 >> 16;
		urdma->route = calculate_route(hdr2 >> 16);
		urdma->state = URDMA_STATE_WAIT;
	} else if (urdma->state == URDMA_STATE_ACCEPT) {
		urdma->remote_index = hdr2 & 0xFFFF;
		urdma->node = hdr2 >> 16;
		urdma->route = calculate_route(hdr2 >> 16);
		urdma->state = URDMA_STATE_READY;
		for (i = 0; i < URDMA_MAXREQ; i++) {
			count = urdma->recv_req[i].count;
			if (count == 0) {
				break;
			}
			mcmsg_send(urdma->mcmsg_task, MCTRL_URDMAR, urdma, count);
		}
	} else {
		mcmsg_trace_drop("urdma recv connect wrong state",
				urdma->state);
		return;
	}
}

mcmsg_send_urdmar(mt, ctl, urdma, count)
	mcmsg_task_t	*mt;
	urdma_entry_t	*urdma;
	unsigned long	count;
{
	unsigned long	hdr1;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	hdr1 = MCTRL_URDMAR | (urdma->remote_index << 16);
	mcmsg_trace_send(hdr1, count, urdma->node, 2, urdma-urdma_tab, count);
	mcmsg_send_hdr2_eod(urdma->route, hdr1, count);
}

mcmsg_recv_urdmar(hdr1, count)
	unsigned long	hdr1;
	unsigned long	count;
{
	urdma_entry_t	*urdma;
	urdma_req_t	*req;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	recv_hdr2();
	urdma = &urdma_tab[hdr1 >> 16];
	mcmsg_trace_recv(hdr1, count, urdma->node, 2, hdr1 >> 16, count);
	if (!(urdma->state & URDMA_CONNECTED)) {
		mcmsg_trace_drop("recv urdmar not connected", hdr1 >> 16);
		return;
	}
	urdma->send_req_ready_count[urdma->send_ready_in] = count;
	URDMA_INCREQ(urdma->send_ready_in);

	req = &(urdma->send_req[urdma->srdy_slot]);
	if (!urdma->sending && req->count != 0) {
		urdma->sending = 1;
		mcmsg_send_tail(urdma->mcmsg_task, MCTRL_URDMAD, urdma, req);
	}
}

mcmsg_send_urdmad(mt, ctl, urdma, req)
	mcmsg_task_t	*mt;
	urdma_entry_t	*urdma;
	urdma_req_t	*req;
{
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	count;
	unsigned long	*rc;
	unsigned long	buf;
	unsigned long	bp1;
	unsigned long	bp2;
	nic_reg		t;

	if (urdma_tab == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	if (req->count != 0) {
		rc = &urdma->send_req_ready_count[urdma->send_ready_out];
		if (req->count <= *rc) {
			count = req->count;
		} else {
			count = *rc;
		}
		buf = req->buf;
		hdr1 = MCTRL_URDMAD | (urdma->remote_index << 16);
		if (count > URDMA_PKT) {

			hdr2 = urdma_full_hdr2;

			bp1 = mcmsg_validate_read1(buf, URDMA_PKT, urdma->dirbase);
			bp2 = mcmsg_validate2();
			if (bp1 == 0) {
				return;
			}
send_next:
			mcmsg_trace_send(hdr1, hdr2, urdma->node, 2,
					 urdma-urdma_tab, req);
			mcmsg_send_pkt2(mt, 0, bp1, bp2, URDMA_PKT,
					urdma->route, hdr1, hdr2);

			buf += URDMA_PKT;
			req->count -= URDMA_PKT;
			*rc -= URDMA_PKT;
			count -= URDMA_PKT;

			if (count > URDMA_PKT) {
				bp1 = mcmsg_validate_read1(buf, URDMA_PKT,
							urdma->dirbase);
				bp2 = mcmsg_validate2();
				if (bp1 == 0) {
					return;
				}
				for (;;) {

					t.full = NIC.status.full;

					/*
					 * Check for receive
					 */

					if ((t.halfs.lo & mcmsg_hw.recv_status) != 0) {
						req->buf = buf;
						mcmsg_save_send(mt,
								mcmsg_send_urdmad,
								urdma, req);
						return;
					}

					/*
					 * Check for TX FIFO ready.
					 */

					if ((t.halfs.lo & SEND_INTR_MODE) != 0) {

						DISABLE_TX_FIFO;
						goto send_next;
					}
				}
			}
			req->buf = buf;
			mcmsg_send_tail(mt, MCTRL_URDMAD, urdma, req);
			return;
		}

		*rc -= count;
		if (*rc == 0) {
			URDMA_INCREQ(urdma->send_ready_out);
		}
		req->buf = buf + count;
		req->count -= count;
		hdr2 = count | urdma_blank_hdr2;

		bp1 = mcmsg_validate_read1(buf, count, urdma->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 != 0) {
			mcmsg_trace_send(hdr1, hdr2, urdma->node, 2,
					 urdma-urdma_tab, req);
			mcmsg_send_pkt2(mt, 0, bp1, bp2, count,
					urdma->route, hdr1, hdr2);

			if (req->count == 0) {
				urdma->send_status++;
				(urdma->statusp)[0] = urdma->send_status;

				/*
				 * Step to the next send slot
				 */

				URDMA_INCREQ(urdma->srdy_slot);
				if (urdma->srdy_slot == urdma->send_slot) {
					urdma->sending = 0;
					return;
				}
				req = &(urdma->send_req[urdma->srdy_slot]);
			}

			if (urdma->send_ready_in !=
			    urdma->send_ready_out) {
				mcmsg_send_tail(mt, MCTRL_URDMAD,
						urdma, req);
				return;
			}
		}
	}
	urdma->sending = 0;
}

mcmsg_recv_urdmad(hdr1, hdr2)
	unsigned long	hdr1;
	unsigned long	hdr2;
{
	unsigned long	count;
	urdma_entry_t	*urdma;
	urdma_req_t	*req;
	unsigned long	buf;
	unsigned long	bp1;
	unsigned long	bp2;
	int		slot;

	recv_hdr2();
	slot = hdr1 >> 16;
	count = hdr2 & 0xFFFF;
	if (urdma_tab == 0) {
		mcmsg_trace_drop("recv urdmad no urdma", 0);
		mcmsg_fifo_flush(count);
		return;
	}

	urdma = &urdma_tab[slot];
	mcmsg_trace_recv(hdr1, hdr2, hdr2 >> 16, 2, hdr1 >> 16, urdma->rrdy_slot);
	if (!(urdma->state & URDMA_CONNECTED)) {
		mcmsg_trace_drop("recv urdmad not connected", hdr1 >> 16);
		mcmsg_fifo_flush(count);
		return;
	}
	req = &(urdma->recv_req[urdma->rrdy_slot]);
#if	MACH_ASSERT
	if (req->count == 0) {
		mcmsg_trace_drop("recv rdmad no matching request", 0);
		mcmsg_fifo_flush(count);
		return;
	}
#endif	MACH_ASSERT
	buf = req->buf;
	if (req->count > count) {

		req->buf += count;
		req->count -= count;
		bp1 = mcmsg_validate_read1(buf, count, urdma->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			mcmsg_trace_drop("urdma recv buf", buf);
			mcmsg_fifo_flush(count);
			return;
		}
		mcmsg_recv_buf_even(bp1, bp2, count);
		return;

	} else {

		if (count != req->count) {
			mcmsg_trace_drop("recv urdmar not exact", count);
			mcmsg_fifo_flush(count);
			return;
		}

		bp1 = mcmsg_validate_read1(buf, count, urdma->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			mcmsg_trace_drop("urdma recv buf", buf);
			return;
		}
		mcmsg_recv_buf_even(bp1, bp2, count);

		req->count = 0;
		urdma->recv_status++;
		(urdma->statusp)[1] = urdma->recv_status;
		URDMA_INCREQ(urdma->rrdy_slot);
	}

	return;
}

mcmsg_send_urdmap(mt, ctl, urdma, route, hdr1, sbuf, rbuf, count)
	mcmsg_task_t	*mt;
	urdma_entry_t	*urdma;
	unsigned long	route;
	unsigned long	hdr1;
	user_pointer_t	sbuf;
	user_pointer_t	rbuf;
	unsigned long	count;
{
	unsigned long	hdr2;
	unsigned long	bp1;
	unsigned long	bp2;
	nic_reg		t;

	if (urdma_tab == 0 || urdma->mcmsg_task == 0) {
		mcmsg_trace_drop("no urdma", 0);
		return;
	}

	hdr2 = urdma_full_hdr2;
	if (count > URDMA_PKT) {

		bp1 = mcmsg_validate_read1(sbuf, URDMA_PKT, urdma->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			return;
		}
send_next:
		mcmsg_trace_send(hdr1, hdr2, urdma->node, 2,
				 urdma-urdma_tab, sbuf);
		mcmsg_send_pkt4(mt, 0, bp1, bp2, URDMA_PKT, route,
				hdr1, hdr2, urdma->app, rbuf);
		sbuf += URDMA_PKT;
		rbuf += URDMA_PKT;
		count -= URDMA_PKT;
		if (count > URDMA_PKT) {
			bp1 = mcmsg_validate_read1(sbuf, URDMA_PKT, urdma->dirbase);
			bp2 = mcmsg_validate2();
			if (bp1 == 0) {
				return;
			}
			for (;;) {

				t.full = NIC.status.full;

				/*
				 * Check for receive
				 */

				if ((t.halfs.lo & mcmsg_hw.recv_status) != 0) {
					mcmsg_save_send(mt,
							mcmsg_send_urdmap,
							urdma, route, hdr1,
							sbuf, rbuf, count);
					return;
				}

				/*
				 * Check for TX FIFO ready.
				 */

				if ((t.halfs.lo & SEND_INTR_MODE) != 0) {

					DISABLE_TX_FIFO;
					goto send_next;
				}
			}
		}
		mcmsg_send_tail(mt, MCTRL_URDMAP,
				urdma, route, hdr1,
				sbuf, rbuf, count);
		return;
	}
	hdr2 = count | urdma_blank_hdr2 | (1 << 15);

	if (count > 0) {
		bp1 = mcmsg_validate_read1(sbuf, count, urdma->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			return;
		}
		mcmsg_trace_send(hdr1, hdr2, urdma->node, 2,
				 urdma-urdma_tab, sbuf);
		mcmsg_send_pkt4(mt, 0, bp1, bp2, count, route,
				hdr1, hdr2, urdma->app, rbuf);
	} else {
		mcmsg_trace_send(hdr1, hdr2, urdma->node, 2,
				 urdma-urdma_tab, sbuf);
		mcmsg_send_hdr4_eod(route, hdr1, hdr2, urdma->app, rbuf);
	}
	urdma->send_status++;
	(urdma->statusp)[0] = urdma->send_status;
}

mcmsg_recv_urdmap(hdr1, hdr2)
	unsigned long	hdr1;
	unsigned long	hdr2;
{
	unsigned long	app;
	user_pointer_t	buf;
	int		last;
	int		slot;
	unsigned long	count;
	urdma_entry_t	*urdma;
	unsigned long	bp1;
	unsigned long	bp2;

	recv_hdr4(app, buf);
	slot = hdr1 >> 16;
	count = hdr2 & 0x7FFF;
	last = (hdr2 & (1 << 15)) != 0;
	if (urdma_tab == 0) {
		mcmsg_trace_drop("recv urdmap no urdma", 0);
		if (count > 0) {
			mcmsg_fifo_flush(count);
		}
		return;
	}

	urdma = &urdma_tab[slot];
	mcmsg_trace_recv(hdr1, hdr2, hdr2 >> 16, 2, hdr1 >> 16, buf);

	if (urdma->app != app) {
		mcmsg_trace_drop("recv urdmad wrong app", urdma->app);
		if (count > 0) {
			mcmsg_fifo_flush(count);
		}
		return;
	}
	if (urdma->state != URDMA_STATE_ACCEPT) {
		mcmsg_trace_drop("recv urdmad wrong state", urdma->state);
		if (count > 0) {
			mcmsg_fifo_flush(count);
		}
		return;
	}

	if (count > 0) {
		bp1 = mcmsg_validate_read1(buf, count, urdma->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			mcmsg_trace_drop("urdma recv buf", buf);
			mcmsg_fifo_flush(count);
			return;
		}
		mcmsg_recv_buf_even(bp1, bp2, count);
	}

	if (last) {
		urdma->recv_status++;
		(urdma->statusp)[1] = urdma->recv_status;
	}
	return;
}
