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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_provide.c,v 1.29 1994/11/18 20:41:52 mtm Exp $
 */

/*
 * mcmsg_provide.c
 *
 * Provide call
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>

syscall_mcmsg_provide(xmsg)
	xmsg_t		*xmsg;
{
	register task_t		task;
	register xmsg_t		*xp;
	register xmsg_t		*xt;
	register mcmsg_task_t	*mt;
	register int		x;
	register int		i;
	register unsigned long	pkt;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register xmsg_t		*xm;
	register select_item_t	*pid_last;
	register unsigned long	t;

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
	mcmsg_trace_provide(xmsg, xp);
	if (xp == 0) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -2;
	}

	if (mt->xmsg_head == 0) {
		mt->xmsg_head = xmsg;
	} else {
		xt = (xmsg_t *)mcmsg_validate_line(mt->xmsg_tail);
		if (xt == 0) {
			mt->xmsg_head = 0;
			mt->xmsg_tail = 0;
			mt->provided = 0;
			assert(mcmsg_reentry--);
			RED_OFF(RED_MSG);
			splx(x);
			return -3;
		}
		xt->link = xmsg;
	}
	mt->xmsg_tail = xmsg;
	mt->provided += xp->size + sizeof(xmsg_t);

	pid_si = mt->avail_need;
	if (pid_si != 0) {
		pkt = mt->applinfo.pkt_size;
		assert((t = MAXLOOP) != 0);
		for (;;) {
			unsigned long want;
			unsigned long deficit;

			pid_last = pid_si;
			pid_si = pid_si->ppid.avail_link;

			want = pid_si->ppid.rk_recv_want;

			if (pid_si->ppid.rk_recv_pid != 0) {
				/*
				 * We have a 'request to send' (NXS/RKS) from
				 * this process.
				 */
				register unsigned long msg_size;

				msg_size = ((want + 2*sizeof(xmsg_t)-1) &
					   ~(sizeof(xmsg_t)-1));
				assert(mt->provided >= mt->assigned);
				if (mt->provided - mt->assigned >= msg_size) {

					xm = mcmsg_alloc_whole_xmsg(want,
						mt->provided - mt->assigned);

					xp = (xmsg_t *)mcmsg_validate_line(xm);
					xp->length = want;
					xp->msg_type =
						pid_si->ppid.rk_recv_type;
					xp->source_node =
						pid_si->ppid.node;
					xp->source_ptype =
						pid_si->ppid.rk_recv_ptype;

					if (want > 0) {
						si = mcmsg_alloc_select_item();
						assert(si != 0);
						mcmsg_trace_debug("satisfy", 4,
							pid_si->value, want,
							si, xm);
						si->method = SELMETH_RECV_XMSG;
						si->item = xm;
						si->value =
						    pid_si->ppid.rk_recv_pid;
						si->nxrq.pid_si = pid_si;
						si->nxrq.xmsg = xm;
						si->nxrq.sequence =
						    pid_si->ppid.rk_recv_seq;
						si->nxrq.msg_type =
						    pid_si->ppid.rk_recv_type;
						si->nxrq.source_ptype =
						    pid_si->ppid.rk_recv_ptype;
						si->nxrq.buf =
						    (unsigned long)(xm + 1);
						si->nxrq.count = want;
						si->nxrq.take = 0;
						si->nxrq.stop = want;
						si->nxrq.offset = 0;

						mcmsg_install_sequence(
						    pid_si->ppid.rk_recv_pid,
						    si);
					} else {
						mcmsg_trace_debug("satisfy", 4,
							pid_si->value, want,
							0, xm);
						xp->state = XMSG_FULL;
					}

					mcmsg_send(SENDMETH_RKR,
						   pid_si,
						   pid_si->ppid.rk_recv_seq);
					pid_si->ppid.rk_recv_pid = 0;
				} else {
					break;
				}
			}
			if (mt->provided - mt->assigned < pkt) {
				break;
			}

			/*
			 * Give some provided buffer to a sender in need.
			 */
			{
				register unsigned long savail;
				register unsigned long gth;
				register unsigned long inc;

				deficit = pid_si->ppid.recv_target
				        - pid_si->ppid.recv_total;
				assert(deficit <= pid_si->ppid.recv_target);

				inc = pkt;
				if (deficit < pkt) {
					inc = deficit;
				}
				mt->assigned += inc;
				pid_si->ppid.recv_give += inc;
				pid_si->ppid.recv_total += inc;

				deficit -= inc;
				savail = pid_si->ppid.recv_total - pid_si->ppid.recv_give;
				gth = mt->applinfo.give_threshold;

				if (savail <= gth &&
					pid_si->ppid.recv_give > gth) {
					mcmsg_send(SENDMETH_RKA, pid_si, mt);
				}
			}


			if (pid_si->ppid.rk_recv_pid == 0 && deficit == 0) {
				if (pid_last == pid_si) {
					mt->avail_need = 0;
					pid_si->ppid.avail_link = 0;
					break;
				} else {
					pid_last->ppid.avail_link =
					 pid_si->ppid.avail_link;
					if (mt->avail_need == pid_si) {
						mt->avail_need = pid_last;
					}
					pid_si->ppid.avail_link = 0;
					pid_si = pid_last->ppid.avail_link;
				}
			}
			assert(t-- != 0);
		}
	}

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

/*
 *	Function:
 *		mcmsg_xmsg_head_size()
 *
 *	Purpose:
 *		return the size of the first xmsg buffer.
 */
unsigned long
mcmsg_xmsg_head_size()
{
	xmsg_t *xmsg;
	xmsg_t *xp;

	xmsg = mcmsg_task->xmsg_head;
	if (xmsg == 0) {
		return 0;
	}
	xp = (xmsg_t *)mcmsg_validate_line(xmsg);
	assert(xp != 0);
	return (xp->size);
}

xmsg_t *
mcmsg_alloc_whole_xmsg(length, avail)
	unsigned long	length;
	unsigned long	avail;
{
	xmsg_t	*xmsg;
	unsigned long diff;
	xmsg_t	*xp;

	xmsg = mcmsg_task->xmsg_head;
	if (xmsg == 0) {
		return xmsg;
	}
	xp = (xmsg_t *)mcmsg_validate_line(xmsg);
	if (xp == 0) {
		mcmsg_task->xmsg_head = 0;
		mcmsg_task->xmsg_tail = 0;
		mcmsg_task->provided = 0;
		return 0;
	}
	if (xp->size < length) {
		xmsg_t	*old = xp;
		xmsg_t	*oldxmsg = xmsg;

		if (avail >= xp->size + length + (2*sizeof(xmsg_t))) {
			xmsg = old->link;
			if (xmsg == 0) {
				return 0;
			}
			xp = (xmsg_t *)mcmsg_validate_line(xmsg);
			if (xp == 0) {
				mcmsg_task->xmsg_head = 0;
				mcmsg_task->xmsg_tail = 0;
				mcmsg_task->provided = 0;
				return 0;
			}
		}
		if (xp->size < length) {
			return 0;
		}
		mcmsg_task->xmsg_head = xmsg;
		mcmsg_trace_debug("trash", 2, oldxmsg, old->size, 0, 0);
		old->state = XMSG_TRASH;
		mcmsg_task->provided -= old->size + sizeof(xmsg_t);
	}

	diff = xp->size - length;
	if (diff >= sizeof(xmsg_t)) {
		xmsg_t *next;
		xmsg_t *np;

		diff = (diff - sizeof(xmsg_t)) & ~(sizeof(xmsg_t)-1);
		next = (xmsg_t *)(((unsigned long)xmsg) + xp->size - diff);
		np = (xmsg_t *)mcmsg_validate_line(next);
		if (np == 0) {
			mcmsg_task->xmsg_head = 0;
			mcmsg_task->xmsg_tail = 0;
			mcmsg_task->provided = 0;
			return 0;
		}
		np->link = xp->link;
		np->size = diff;
		np->state = XMSG_EMPTY;
		xp->link = next;
		xp->size -= diff + sizeof(xmsg_t);
		if (mcmsg_task->xmsg_tail == xmsg) {
			mcmsg_task->xmsg_tail = next;
		}
	}
	mcmsg_task->xmsg_head = xp->link;
	if (mcmsg_task->xmsg_head == 0) {
		mcmsg_task->xmsg_tail = 0;
	}
	mcmsg_task->provided -= xp->size + sizeof(xmsg_t);
	return xmsg;
}

#if	MACH_ASSERT

mcmsg_validate_line_real(a, dirbase, file, line)
	unsigned long a;
	unsigned long	*dirbase;
	char		*file;
	int		line;
{

	if (a & 0x0000001F) {
mcmsg_trace_debug("validate line failed", 3, file, line, a, 0);
		return 0;
	}
	return mcmsg_validate_real(a, dirbase, file, line);
}

mcmsg_validate_long_real(a, dirbase, file, line)
	unsigned long a;
	unsigned long	*dirbase;
	char		*file;
	int		line;
{

	if (a & 0x00000003) {
		mcmsg_trace_debug("validate long failed", 3,
				file, line, a, 0);
		return 0;
	}
	return mcmsg_validate_real(a, dirbase, file, line);

}

mcmsg_validate_real(a, dirbase, file, line)
	unsigned long	a;
	unsigned long	*dirbase;
	char		*file;
	int		line;
{
	pt_entry_t	pt;

	pt = dirbase[pdenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		mcmsg_trace_debug("validate no pde", 4,
				file, line, a, pt);
		return 0;
	}
	pt = ((pt_entry_t *)(pt & INTEL_PTE_PFN))[ptenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		mcmsg_trace_debug("validate no pte", 4,
				file, line, a, pt);
		return 0;
	}

	pt = (pt & INTEL_PTE_PFN) | (a & ~(INTEL_PTE_PFN));
#if	iPSC860
	if (mcmsg_phys) {
#if DEBUG
printf("Validate %08X to %08X physical line %d\n", a, pt, line);
#endif DEBUG
		if (!mcmsg_flush) {
			flush();
			mcmsg_flush = 1;
		}
		return pt;
	}
#if DEBUG
printf("Validate %08X to %08X line %d\n", a, pt, line);
#endif DEBUG
	return a;
#else	iPSC860
	return pt;
#endif	iPSC860
}


#else	MACH_ASSERT

mcmsg_validate_real(a, dirbase)
	unsigned long	a;
	unsigned long	*dirbase;
{
	pt_entry_t	pt;

	pt = dirbase[pdenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		return 0;
	}
	pt = ((pt_entry_t *)(pt & INTEL_PTE_PFN))[ptenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		return 0;
	}

	pt = (pt & INTEL_PTE_PFN) | (a & ~(INTEL_PTE_PFN));
#if	iPSC860
	if (mcmsg_phys) {
#if DEBUG
printf("Validate %08X to %08X physical line %d\n", a, pt, line);
#endif DEBUG
		if (!mcmsg_flush) {
			flush();
			mcmsg_flush = 1;
		}
		return pt;
	}
#if DEBUG
printf("Validate %08X to %08X line %d\n", a, pt, line);
#endif DEBUG
	return a;
#else	iPSC860
	return pt;
#endif	iPSC860
}

#endif	MACH_ASSERT
