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
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_vc.c,v 1.3 1995/02/10 18:13:39 stans Exp $

/*
 * mcmsg_vc.c
 *
 * Virtual Connections
 */

#define	MCMSG_MODULE	MCMSG_MODULE_VC

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_vc.h>

/*
 * Message structure for interchanging connection information
 */

typedef struct vc_info {
	unsigned long	index;
	unsigned long	node;
} vc_info_t;

/*
 * System initialization
 */

mcmsg_vc_init()
{

	return;
}


/*
 * To save wear and tear on the kernel entry point files, all the 
 * virtual connection system calls come through the same wormhole.
 */

syscall_mcmsg_vc(callid, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
	int	callid;
{

	switch (callid) {

	case VC_INIT:
		return vc_init(arg1, arg2);

	case VC_OPEN_ONE:
		return vc_open_one(arg1, arg2);

	case VC_CLOSE_ONE:
		return vc_close_one(arg1);

	}
	return -1;
}

vc_init(nconn, stbuf)
	int	nconn;
	unsigned long stbuf;
{
	mcmsg_task_t	*mt;
	int		i;
	int		n;

	/*
	 * Validate caller
	 */

	mt = current_task()->mcmsg_task;
	if (mt == 0) {
		return -1;
	}
	if (!mt->applinfo.process_lock) {
		return -1;
	}
	if (mt->vc_limit != 0) {
		return -1;
	}

	/*
	 * Allocate vctab here if this is the first caller
	 * instead of having it statically allocated.
	 * Use a reference count
	 */

	/* XXX */


	/*
	 * Find a contiguous set of unallocated slots in vctab
	 */

	n = 0;
	for (i = vc_low_free; i < VC_MAX; i++) {
		if (vctab[i].mcmsg_task == 0) {
			n++;
			if (n == nconn) {
				for (; n > 0; i--, n--) {
					vctab[i].mcmsg_task = mt;
					vctab[i].state = VC_STATE_FREE;
				}
				i++;
				if (i == vc_low_free) {
					vc_low_free = i + nconn;
				}
				mt->vc_base = i;
				mt->vc_limit = nconn;
				mt->vc_low_free = i;
				mt->vc_stbuf = stbuf;
				mt->vc_tagged = -1;

				return 0;
			}
		} else {
			n = 0;
		}
	}
	return -1;
}

mcmsg_vc_clear_task(mt)
	mcmsg_task_t	*mt;
{

	mcmsg_trace_debug("vc clear task", 1, mt, 0, 0, 0);
	if (mt != 0 && mt->vc_limit != 0) {
		int	i;
		int	n;

		/*
		 * Disconnect this task
		 * from vc
		 */

		n = mt->vc_limit;
		mt->vc_limit = 0;

		/*
		 * XXX Freeze entries here
		 */

		/*
		 * Clear the task's slots in vctab
		 */

		for (i = mt->vc_base; n; i++, n--) {
			vctab[i].mcmsg_task = 0;
		}

		/*
		 * Keep vc_low_free up to date for faster allocation
		 */

		if (mt->vc_base < vc_low_free) {
			vc_low_free = mt->vc_base;
		}
	}
}

vc_open_one(node, tag)
	int	node;
	int	tag;
{
	int	i;

	i = mcmsg_task_call(current_task(), POST_VCOPEN, node, tag);
	return i;
}

vc_close_one(conn)
	int	conn;
{
	mcmsg_task_t	*mt;
	int		i;

	/*
	 * Validate arguments
	 */

	mt = current_task()->mcmsg_task;
	if (mt == 0) {
		return -1;
	}
	if (conn >= mt->vc_limit) {
		return -1;
	}
	i = mt->vc_base + conn;
	if (vctab[i].state == VC_STATE_FREE) {
		return -1;
	}

	/*
	 * XXX Freeze it here
	 */

	/*
	 * Set this connection free
	 * Update task's low_free
	 */

	vctab[i].state = VC_STATE_FREE;
	if (mt->vc_low_free > i) {
		mt->vc_low_free = i;
	}
	return 0;
}

void
mcmsg_db_vc(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	int		i;
	mcmsg_task_t	*mt;

	db_printf(
"  Index   route  Rmt idx   mt    Frz Send_req_rdy State srdy send rrdy recv\n");
	if (have_addr) {
		if (count == -1) {
			count = 1;
		}
		for (i = addr; count; i++, count--) {
			mcmsg_dump_vc_entry(i);
		}
	} else {
		for (i = 0; i < VC_MAX; i++) {
			if (vctab[i].mcmsg_task != 0 &&
			    vctab[i].state != VC_STATE_FREE) {
				mcmsg_dump_vc_entry(i);
			}
		}
		mt = 0;
		for (i = 0; i < VC_MAX; i++) {
			if (vctab[i].mcmsg_task != 0 &&
			    vctab[i].mcmsg_task != mt) {
				mt = vctab[i].mcmsg_task;
db_printf("Mcmsg_task %08X  base %d  limit %d  low_free %d  stbuf %08x  tagged %d\n",
					mt,
					mt->vc_base,
					mt->vc_limit,
					mt->vc_low_free,
					mt->vc_stbuf,
					mt->vc_tagged);
			}
		}
		db_printf("Low free %d\n", vc_low_free);
	}
}

mcmsg_dump_vc_entry(i)
	int		i;
{
	int		j;
	int		n;

	db_printf("%6d. %08X %6d %08X %c%c%c %12d  %c      %d    %d    %d    %d\n",
		i,
		vctab[i].route,
		vctab[i].remote_index,
		vctab[i].mcmsg_task,
		vctab[i].local_freeze? 'L' : 'l',
		vctab[i].remote_freeze? 'R' : 'r',
		vctab[i].freeze_int? 'I' : 'i',
		vctab[i].send_req_ready_count,
		"FTWA"[vctab[i].state],
		vctab[i].srdy_slot,
		vctab[i].send_slot,
		vctab[i].rrdy_slot,
		vctab[i].recv_slot);
	for (j = vctab[i].srdy_slot, n = VC_NREQ;
	     vctab[i].send_req[j].active && n > 0;
	     j = (j + 1) % VC_NREQ, n--) {
		db_printf("        send %08X %12d %c\n",
			vctab[i].send_req[j].buf,
			vctab[i].send_req[j].count,
			vctab[i].send_req[j].intreq ? 'I' : 'i');
	}
	for (j = vctab[i].rrdy_slot, n = VC_NREQ;
	     vctab[i].recv_req[j].active && n > 0;
	     j = (j + 1) % VC_NREQ, n--) {
		db_printf("        recv %08X %12d %c\n",
			vctab[i].recv_req[j].buf,
			vctab[i].recv_req[j].count,
			vctab[i].recv_req[j].intreq ? 'I' : 'i');
	}
}
