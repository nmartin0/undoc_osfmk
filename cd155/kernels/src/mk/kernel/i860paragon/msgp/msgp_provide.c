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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_provide.c,v 1.49 1995/03/03 18:46:14 joel Exp $
 */

/*
 * mcmsg_provide.c
 *
 * Provide call
 */

#define	MCMSG_MODULE	MCMSG_MODULE_NX

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>

/* Flags to enable (1) or disable (0) forward and backward coalescing
   of xmsg buffers on provide() calls */
#define FORWARD_COALESCE	1
#define BACKWARD_COALESCE	1

/* xmsg coalescing counters, for debugging / monitoring */
unsigned long	forward_coalesce = 0;
unsigned long	backward_coalesce = 0;
unsigned long	dual_coalesce = 0;
unsigned long	no_coalesce = 0;

/* VERIFY_XMSG is used to enable or disable checking of provided xmsg buffers.
   If enabled (VERIFY_XMSG = 1), all provided xmsg buffers will be checked
   for internal inconsistencies; in particular, the link, backlink,
   prev_adjacent and next_adjacent fields are checked.  Checking will
   seriously degrade kernel performance. */
#define VERIFY_XMSG	0

/* NOTE:  assertmba() is a kludge put in until asserts are fixed. Currently,
   assert() causes the kernel to hang, without the ability to break in
   with scanio.  To use assertmba(), set a breakpoint at xxxbreak() in
   EVERY kernel / node on which you want to debug; this should be REAL fun
   on more that about 16 nodes! */

#if VERIFY_XMSG
unsigned long	xxxassertpoint = 0;

xxxbreak()
{
	return;
}

#define assertmba(x,y) \
	if (!(x)) { \
		xxxassertpoint = (y); \
		xxxbreak(); \
	}

#define verify_xmsg(mt) _verify_xmsg(mt)

_verify_xmsg(mt)
mcmsg_task_t	*mt;
{
xmsg_t		*xmsg;
xmsg_t		*xp;
xmsg_t		*xxp;
int		t;

	xmsg = mt->xmsg_head;
	assert((t = MAXLOOP) != 0);
	while (xmsg != 0) {
		xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
		assertmba(xp != 0,4);
		assertmba(xp->chain_number == 0,5);
		assertmba(xp->state == XMSG_EMPTY,6);
		if (xmsg != mt->xmsg_head) {
			assertmba(xp->backlink != 0,8);
			xxp = (xmsg_t *)mcmsg_validate_line(mt, xp->backlink);
			assertmba(xxp != 0,9);
			assertmba(xxp->link == xmsg,10);
		}
		if (xmsg == mt->xmsg_tail) {
			assertmba(xp->link == 0,11);
		} else {
			assertmba(xp->link != 0,12);
			xxp = (xmsg_t *)mcmsg_validate_line(mt, xp->link);
			assertmba(xxp != 0,13);
			assertmba(xxp->backlink == xmsg,14);
		}
		if (xp->prev_adjacent != 0) {
			xxp = (xmsg_t *)mcmsg_validate_line(mt, xp->prev_adjacent);
			assertmba(xxp != 0,15);
			assertmba((unsigned long)(xp->prev_adjacent) +
			       xxp->size + sizeof(xmsg_t) == (unsigned long)xmsg,
			       16);
			assertmba(xxp->next_adjacent == xmsg,17);
		}
		if (xp->next_adjacent != 0) {
			xxp = (xmsg_t *)mcmsg_validate_line(mt, xp->next_adjacent);
			assertmba(xxp != 0,18);
			assertmba(xxp->prev_adjacent == xmsg,19);
		}
		xmsg = xp->link;
		assertmba(t-- != 0,20);
	}
}

#else VERIFY_XMSG
#define verify_xmsg(mt)
#define assertmba(x,y)
#endif VERIFY_XMSG

mcmsg_give_avail(mt)
	mcmsg_task_t	*mt;
{
	register select_item_t	*pid_si;
	register select_item_t	*pid_last;
	register select_item_t	*dat_pid_si;
	register unsigned long	pkt;
	register unsigned long	t;
	register xmsg_t		*xp;
	register user_pointer_t	xm;
	register select_item_t	*si;

	pid_si = mt->avail_need;
	if (pid_si != 0) {
		pkt = mt->applinfo.pkt_size;
		assert((t = MAXLOOP) != 0);
		for (;;) {
			unsigned long want;
			unsigned long deficit;

			assert(t-- != 0);
			pid_last = pid_si;
			pid_si = pid_si->ppid.avail_link;
	    		assert(pid_si->method == SELMETH_PID);

        		if((pid_si->ppid.sub_pid_si != 0 && 
			   pid_si->ppid.sub_pid_si != (select_item_t *) 1) ||
			   pid_si->ppid.recv_target == 0) {
			    /* pid has been sent a NDT request or is detached */
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
					pid_si = pid_last;
				}
                		continue;
        		}

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

					xm = mcmsg_alloc_whole_xmsg(mt, want,
						mt->provided - mt->assigned);

					xp = (xmsg_t *)mcmsg_validate_line(mt, xm);
					assert(xp != 0);
					xp->length = want;
					xp->msg_type =
						pid_si->ppid.rk_recv_type;
					xp->source_node =
						pid_si->ppid.node;
					xp->originating_node =
						pid_si->ppid.rk_recv_originating_node;
					xp->source_ptype =
						pid_si->ppid.rk_recv_ptype;

					si = mcmsg_alloc_select_item();
					assert(si != 0);
					mcmsg_trace_debug("satisfy", 4,
						pid_si->value, want, si, xm);
					si->method = SELMETH_RECV_XMSG;
					si->item = (void *)xm;
					si->value =
					    pid_si->ppid.rk_recv_pid;
					si->nxrq.request = 0;
					si->nxrq.pid_si = pid_si;
					si->nxrq.xmsg = xm;
					si->nxrq.sequence =
					    pid_si->ppid.rk_recv_seq;
					si->nxrq.msg_type =
					    pid_si->ppid.rk_recv_type;
					si->nxrq.source_ptype =
					    pid_si->ppid.rk_recv_ptype;
					si->nxrq.buf = 0;
					si->nxrq.count = want;
					si->nxrq.take = 0;
					si->nxrq.stop = want;
					si->nxrq.offset = 0;
					xp->si = si;

					if (want > 0) {
						mcmsg_install_sequence(mt,
						    pid_si->ppid.rk_recv_pid,
						    si);
					} else {
						xp->state = XMSG_FULL;
					}

					mcmsg_send(mt, MCTRL_NXR,
						   si,
						   pid_si->ppid.rk_recv_seq);
					if (want <= 0) {
						mcmsg_free_select_item(si);	
					}
					pid_si->ppid.rk_recv_pid = 0;
					if (pid_si->ppid.sub_pid_si == (select_item_t *)1) {
					    pid_si->ppid.sub_pid_si = 0;
					    if (mt->attach_pend) {
						mcmsg_trace_debug("provide nxa NDT to",2,pid_si,mt->attach_pend,0,0);
						pid_si->ppid.sub_pid_si = mt->attach_pend;
						pid_si->ppid.recv_give = pid_si->ppid.recv_target = 0;
						mcmsg_send(mt, MCTRL_NDT, pid_si);
						dat_pid_si = mt->attach_pend;
						mt->attach_pend = dat_pid_si->ppid.att_pend_link;
						dat_pid_si->ppid.att_pend_link = 0;
					    }
					}
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
				assert((pid_si->ppid.recv_total > 0) && 
       				(pid_si->ppid.recv_total <= 
					mt->applinfo.memory_each));
				deficit -= inc;
				savail = pid_si->ppid.recv_total -
					 pid_si->ppid.recv_give;
				gth = mt->applinfo.give_threshold;

				if (savail <= gth &&
				    pid_si->ppid.recv_total > gth) {
					mcmsg_trace_debug("satisfy", 2,
						pid_si->value, savail, 0, 0);
					mcmsg_send(mt, MCTRL_NXA, pid_si);
				}
				if (pid_si->ppid.sub_pid_si == (select_item_t *)1) {
				    pid_si->ppid.sub_pid_si = 0;
				    if (mt->attach_pend) {
					mcmsg_trace_debug("provide nxa NDT to",2,pid_si,mt->attach_pend,0,0);
					pid_si->ppid.sub_pid_si = mt->attach_pend;
					pid_si->ppid.recv_give = pid_si->ppid.recv_target = 0;
					mcmsg_send(mt, MCTRL_NDT, pid_si);
					dat_pid_si = mt->attach_pend;
					mt->attach_pend = dat_pid_si->ppid.att_pend_link;
					dat_pid_si->ppid.att_pend_link = 0;
					deficit = 0;
				    }
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
					pid_si = pid_last;
				}
			}
		}
	}


}

mcmsg_provide(mt, xmsg)
	mcmsg_task_t	*mt;
	user_pointer_t	xmsg;
{
	register xmsg_t		*xp;
	register xmsg_t		*xt;
	register int		i;
	register xmsg_t		*xxp;
	register xmsg_t		*xxxp;
	register unsigned long	t;
	unsigned long		coalesced;
	unsigned long		adjust_tail;
	unsigned long		size;
	register xmsg_t		*xrdy_end;

	xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
	mcmsg_trace_provide(xmsg, xp);
	if (xp == 0) {
		return -2;
	}

	/* Ensure that buffer size is a multiple of xmsg_t bytes */
	if ((xp->size & (sizeof(xmsg_t) - 1)) || (xp->size == 0)) {
		return -4;
	}

	xp->state = XMSG_EMPTY;
	xp->link = 0;
	xp->backlink = 0;

	if (mt->xmsg_head == 0) {
		if (mt->xmsg_rdy != 0) {
			xrdy_end = (xmsg_t *)
			               mcmsg_validate_line(mt, mt->xmsg_rdy_end);
			if (xrdy_end == 0) {
				mt->xmsg_rdy = 0;
				mt->xmsg_rdy_end = 0;
				mt->xmsg_head = 0;
				mt->xmsg_tail = 0;
				mt->provided = 0;
				return -3;
			}
			xrdy_end->link = (xmsg_t*)xmsg;
			xp->backlink = (xmsg_t *)mt->xmsg_rdy_end;
		}
		mt->xmsg_head = xmsg;
		mt->xmsg_tail = xmsg;
		mt->provided += xp->size + sizeof(xmsg_t);
		no_coalesce++;
	} else {
		coalesced = 0;
		/* Save the current size; xp->size will be modified
		   if we do a forward coalesce, but we need the original
		   size to add to mt->provided */
		size = xp->size;
#if FORWARD_COALESCE
		if ((xp->next_adjacent != 0) &&
		    (xp->next_adjacent != (xmsg_t *)(mt->xmsg_head))) {
			xxp = (xmsg_t *)mcmsg_validate_line(mt, xp->next_adjacent);
			if (xxp == 0) {
				mt->xmsg_rdy = 0;
				mt->xmsg_rdy_end = 0;
				mt->xmsg_head = 0;
				mt->xmsg_tail = 0;
				mt->provided = 0;
				return -3;
			}
			if (xxp->state == XMSG_EMPTY) {
				/* Forward coalesce */
				forward_coalesce++;
				/* mcmsg_trace_debug("forward coalesce", 4,
                   		     xmsg, xp, mt->xmsg_head, mt->xmsg_tail); */
				if (mt->xmsg_tail ==
					(user_pointer_t)(xp->next_adjacent)) {
					adjust_tail = 1;
				} else {
					adjust_tail = 0;
				}
				xp->link = xxp->link;
				xp->size += xxp->size + sizeof(xmsg_t);
				xp->backlink = xxp->backlink;
				xp->next_adjacent = xxp->next_adjacent;
				if (xxp->next_adjacent != 0) {
					xxxp = (xmsg_t *)
					 mcmsg_validate_line(mt, xxp->next_adjacent);
					if (xxxp == 0) {
						mt->xmsg_rdy = 0;
						mt->xmsg_rdy_end = 0;
						mt->xmsg_head = 0;
						mt->xmsg_tail = 0;
						mt->provided = 0;
						return -3;
					}
					xxxp->prev_adjacent = (xmsg_t *)xmsg;
				}
				/* NOTE:  if the code is ever changed to allow
				   forward coalescing to xmsg_head, it will be
				   valid for xxp->backlink to be 0 */
				assertmba(xxp->backlink != 0,2);
				if (xxp->backlink != 0) {
					xxxp = (xmsg_t *)
					 mcmsg_validate_line(mt, xxp->backlink);
					if (xxxp == 0) {
						mt->xmsg_rdy = 0;
						mt->xmsg_rdy_end = 0;
						mt->xmsg_head = 0;
						mt->xmsg_tail = 0;
						mt->provided = 0;
						return -3;
					}
					xxxp->link = (xmsg_t *)xmsg;
				}
				if (xxp->link != 0) {
					xxxp = (xmsg_t *)
					 mcmsg_validate_line(mt, xxp->link);
					if (xxxp == 0) {
						mt->xmsg_rdy = 0;
						mt->xmsg_rdy_end = 0;
						mt->xmsg_head = 0;
						mt->xmsg_tail = 0;
						mt->provided = 0;
						return -3;
					}
					xxxp->backlink = (xmsg_t *)xmsg;
				}	
				if (adjust_tail) {
					mt->xmsg_tail = xmsg;
				}
				coalesced++;
			}
		}
#endif FORWARD_COALESCE

#if BACKWARD_COALESCE
		if (xp->prev_adjacent != 0) {
			xxp = (xmsg_t *)mcmsg_validate_line(mt, xp->prev_adjacent);
			if (xxp == 0) {
				mt->xmsg_rdy = 0;
				mt->xmsg_rdy_end = 0;
				mt->xmsg_head = 0;
				mt->xmsg_tail = 0;
				mt->provided = 0;
				return -3;
			}
			if (xxp->state == XMSG_EMPTY) {
				/* Backward coalesce */
				if (coalesced) {
					forward_coalesce--;
					dual_coalesce++;
					/* mcmsg_trace_debug("dual coalesce", 4,
                   			      xmsg, xp,
					      mt->xmsg_head, mt->xmsg_tail); */
					/* xp is already in EMPTY list,
					   so first remove it from the list.
					   The subsequent backward coalesce
					   will automatically add it back in */
					if (xp->backlink != 0) {
						xxxp = (xmsg_t *)
							mcmsg_validate_line(mt, xp->backlink);
						if (xxxp == 0) {
							mt->xmsg_rdy = 0;
							mt->xmsg_rdy_end = 0;
							mt->xmsg_head = 0;
							mt->xmsg_tail = 0;
							mt->provided = 0;
							return -3;
						}
						xxxp->link = xp->link;
					}
					if (xp->link != 0) {
						xxxp = (xmsg_t *)
							mcmsg_validate_line(mt, xp->link);
						if (xxxp == 0) {
							mt->xmsg_rdy = 0;
							mt->xmsg_rdy_end = 0;
							mt->xmsg_head = 0;
							mt->xmsg_tail = 0;
							mt->provided = 0;
							return -3;
						}
						xxxp->backlink = xp->backlink;
					}
					if (mt->xmsg_tail == xmsg) {
						mt->xmsg_tail = (user_pointer_t)
							(xp->backlink);
					}
				} else {
					backward_coalesce++;
					/* mcmsg_trace_debug("backward coalesce",
					       4, xmsg, xp, mt->xmsg_head,
					       mt->xmsg_tail); */
				}
				xxp->size += xp->size + sizeof(xmsg_t);
				xxp->next_adjacent = xp->next_adjacent;
				if (xp->next_adjacent != 0) {
					xxxp = (xmsg_t *)
					 mcmsg_validate_line(mt, xp->next_adjacent);
					if (xxxp == 0) {
						mt->xmsg_rdy = 0;
						mt->xmsg_rdy_end = 0;
						mt->xmsg_head = 0;
						mt->xmsg_tail = 0;
						mt->provided = 0;
						return -3;
					}
					xxxp->prev_adjacent = xp->prev_adjacent;
				}
				if (mt->xmsg_tail == xmsg) {
					mt->xmsg_tail = (user_pointer_t)
						(xp->prev_adjacent);
				}
				coalesced++;
			}
		}
#endif BACKWARD_COALESCE

		if (!coalesced) {
			no_coalesce++;
			xt = (xmsg_t *)mcmsg_validate_line(mt, mt->xmsg_tail);
			if (xt == 0) {
				mt->xmsg_rdy = 0;
				mt->xmsg_rdy_end = 0;
				mt->xmsg_head = 0;
				mt->xmsg_tail = 0;
				mt->provided = 0;
				return -3;
			}
			xp->link = 0;
			xt->link = (xmsg_t *)xmsg;
			xp->backlink = (xmsg_t *)(mt->xmsg_tail);
			mt->xmsg_tail = xmsg;
		}

		/* Since this buffer is not at the head of the free list, we
		   will (probably) lose xmsg_t bytes in
		   mcmsg_alloc_whole_xmsg() when we chain from the previous
		   buffer to this one, so assume we're actually provided
		   with xmsg_t fewer bytes to account for the loss */
		mt->provided += size + (coalesced * sizeof(xmsg_t));
	}

	if (mt->xmsg_rdy == 0) {
		mt->xmsg_rdy = mt->xmsg_head;
	}

	verify_xmsg(mt);
	mcmsg_give_avail(mt);
	return 0;
}

user_pointer_t
mcmsg_alloc_whole_xmsg(mt, length, avail)
	mcmsg_task_t	*mt;
	unsigned long	length;
	unsigned long	avail;
{
	register user_pointer_t	xmsg;	/* user-space pointer to xmsg structure */
	register xmsg_t	*xp;	/* kernel-space equivalent to xmsg */
	register user_pointer_t	head;	/* user-space "saved" value of
	                           mt->xmsg_head */
	register xmsg_t	*hp;	/* kernel-space equivalent to head */
	register user_pointer_t	next;	/* user-space value of xmsg->link */
	register xmsg_t	*np;	/* kernel-space equivalent to next */
	register xmsg_t	*nnp;
	register unsigned long need;
	register char	hand_in_a_glove;
	register unsigned char chain_number;
	static unsigned char max_chain_number = (unsigned char)
	                          ((1 << (sizeof(xp->chain_number) * 8)) - 1);
	int		t;

	/* Round length up to next multiple of xmsg_t size, since all
	   allocations must occur in multiples of this size */
	length = (length + sizeof(xmsg_t) - 1) & ~(sizeof(xmsg_t) - 1);

	/* The following case should never happen -- should we assert here? */
	assert((length + sizeof(xmsg_t) <= avail) &&
	       (length + sizeof(xmsg_t) <= mt->provided - mt->assigned));

	xmsg = mt->xmsg_head;
	xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
	if (xp == 0) {
		mt->xmsg_rdy = 0;
		mt->xmsg_rdy_end = 0;
		mt->xmsg_head = 0;
		mt->xmsg_tail = 0;
		mt->provided = 0;
		return 0;
	}

	/* Save initial xmsg and xp pointers for later */
	head = xmsg;
	hp = xp;

	need = length;
	hand_in_a_glove = 0;
	chain_number = 1;
	/* Need to go through loop at least once, in case length == 0 */
	assert((t = MAXLOOP) != 0);
	do {
		if (xp->size >= need) {
			/* Current xmsg buffer fragment is large enough to
			   hold the amount we still need */

			if (xp->size == need) {
				/* Exact fit; the next buffer is at xp->link */

				next = (user_pointer_t)xp->link;
				if (next != 0) {
					/* Fits like a hand in a glove; we
					   won't need to chain this piece
					   across xmsg buffers, and we've
					   completely filled this buffer,
					   so we pick up xmsg_t bytes of
					   provided space.  Set this flag
					   here for later calculations */
					np = (xmsg_t *)mcmsg_validate_line(mt, next);
					if (np == 0) {
						mt->xmsg_rdy = 0;
						mt->xmsg_rdy_end = 0;
						mt->xmsg_head = 0;
						mt->xmsg_tail = 0;
						mt->provided = 0;
						return 0;
					}
					hand_in_a_glove = 1;
				}
			} else {
				/* Current buffer is larger than we need */
				next = (user_pointer_t)
					(((unsigned long)xmsg)
			                               /* skip current header */
			                                + sizeof(xmsg_t)
			                               /* skip current data */
			                                + need);
				np = (xmsg_t *)mcmsg_validate_line(mt, next);
				if (np == 0) {
					mt->xmsg_rdy = 0;
					mt->xmsg_rdy_end = 0;
					mt->xmsg_head = 0;
					mt->xmsg_tail = 0;
					mt->provided = 0;
					return 0;
				}
				np->link = xp->link;
				np->size = xp->size - need - sizeof(xmsg_t);
				np->chain_number = 0;
				np->state = XMSG_EMPTY;
				np->prev_adjacent = (xmsg_t *)xmsg;
				np->next_adjacent = xp->next_adjacent;
				np->backlink = (xmsg_t *)xmsg;
				xp->next_adjacent = (xmsg_t *)next;
				if (np->next_adjacent != 0) {
					nnp = (xmsg_t *)
						mcmsg_validate_line(mt, np->next_adjacent);
					if (nnp == 0) {
						mt->xmsg_rdy = 0;
						mt->xmsg_rdy_end = 0;
						mt->xmsg_head = 0;
						mt->xmsg_tail = 0;
						mt->provided = 0;
						return 0;
					}
					nnp->prev_adjacent = (xmsg_t *)next;
				}
				if (np->link != 0) {
					nnp = (xmsg_t *)
						mcmsg_validate_line(mt, np->link);
					if (nnp == 0) {
						mt->xmsg_rdy = 0;
						mt->xmsg_rdy_end = 0;
						mt->xmsg_head = 0;
						mt->xmsg_tail = 0;
						mt->provided = 0;
						return 0;
					}
					nnp->backlink = (xmsg_t *)next;
				}
			}
			if (chain_number > 1)
				xp->state = XMSG_SPLIT;
			xp->chain_number = 0;  /* last element in chain */
			xp->link = (xmsg_t *)next;
			xp->size = need;
			need = 0;
		} else {  /* xp->size < need */
			/* Split into a buffer chain */
			next = (user_pointer_t)xp->link;
			np = (xmsg_t *)mcmsg_validate_line(mt, next);
			if (np == 0) {
				mt->xmsg_rdy = 0;
				mt->xmsg_rdy_end = 0;
				mt->xmsg_head = 0;
				mt->xmsg_tail = 0;
				mt->provided = 0;
				return 0;
			}
			if (chain_number > 1)
				xp->state = XMSG_SPLIT;
			xp->chain_number = chain_number;
			/* Hopefully this will always be true ! */
			if (chain_number < max_chain_number)
				chain_number++;
			need -= xp->size;
			xmsg = next;
			xp = np;
		}
		assert(t-- != 0);
	} while (need);

	/* xmsg and xp now point to the last buffer fragment in the chain
	   to be returned */
	/* next points to what will become the head of the free list */
	/* np will be a validated version of next, unless we had an exact
	   fit in the last chain */

	mt->xmsg_head = next;

	if (mt->xmsg_tail == xmsg)
		mt->xmsg_tail = next;

	if (hand_in_a_glove)
		/* Perfect fit at end of one xmsg buffer; because one chain
		   is not required, we pick up xmsg_t bytes of provided
		   space that was taken off at the time of the provide
		   (syscall_mcmsg_provide() call) */
		mt->provided -= length;
	else
		mt->provided -= length + sizeof(xmsg_t);

#if 0
	/* Now check for the special case where the first element of the
	   chain has zero size and a non-zero data size was requested.
	   This happens if the the original xmsg buffer (head) had exactly
	   xmsg_t bytes remaining.  If so, simply skip over the first
	   buffer chain element. */
	if ((hp->size == 0) && (length != 0)) {
		assert(hp->chain_number != 0);
		hp->state = XMSG_TRASH;
		hp->chain_number = 0;
		head = hp->link;
		hp = (xmsg_t *)mcmsg_validate_line(mt, head);
		if (hp == 0) {
			mt->xmsg_rdy = 0;
			mt->xmsg_rdy_end = 0;
			mt->xmsg_head = 0;
			mt->xmsg_tail = 0;
			mt->provided = 0;
			return 0;
		}
	}
#endif

	hp->state = XMSG_BUSY;
	hp->totalsize = length;
	mt->xmsg_rdy_end = xmsg;

	if (chain_number > 1) {
		mcmsg_trace_debug("alloc whole xmsg split",
		                  2, head, chain_number - 1, 0, 0);
	}

	return head;
}
