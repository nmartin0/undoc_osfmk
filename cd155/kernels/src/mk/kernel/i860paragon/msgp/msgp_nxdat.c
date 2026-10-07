/*=======Primitive Timing Stuff=========*/
#define TIMESTUFF 0
#if TIMESTUFF
unsigned long timebuf[32];
#define TIMESTAMP(n) time_stamp(&timebuf[n]);
time_stamp(n)
int n;
{
asm("counter_reg    =   0xFFFF4630"); /* Works on RX or GP node */
asm("       orh ha%counter_reg, r0, r31 // Set up register address");
asm("       fld.d   l%counter_reg(r31), f16 // Read counter into f16-17");
asm("       fst.l   f16, 0(r16)     // Store low order word");
}
db_timebuf()
{
	int i;
	for (i=0; i<32; i+=2)
		db_printf("[%d:%d]: %d nsec\n", i, i+1, (timebuf[i+1]-timebuf[i])*20);
}
#else
#define TIMESTAMP(n)
#endif TIMESTUFF
/*======================================*/

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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_nxdat.c,v 1.58 1995/03/27 17:09:58 joel Exp $
 */

/*
 * msgp_nxdat.c
 *
 * NX messages (data)
 */

#define	MCMSG_MODULE	MCMSG_MODULE_NX

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/msgp/msgp_hw.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>

int _mcmsg_nxn_to_xmsg = 0;	/* debug */

#if    BIGPKTS
#define NX_PKT_GRAN	LTU_ALIGN
/*
 *	Send in progress. Since we can receive control packets
 *	in the midst of sending a packet, we need to be able
 *	to update the control information on the fly.
 */
select_item_t *nx_send_in_progress = (select_item_t *)0;
extern int mcmsg_local_send;
#else  BIGPKTS
#define NX_PKT_GRAN	PKT_GRAN
#endif BIGPKTS

select_item_t *mcmsg_find_nx_recv();

/*
 *	Routine:	mcmsg_masktrap
 *
 *	Set hrecv/hsend trap mask value.
 *
 */
mcmsg_masktrap(mt, mask)
	mcmsg_task_t	*mt;
	long	mask;
{

	mt->masktrap = mask;

	/*
	 * If clearing mask then need to signal cpu since there may be
	 * AST requests for this task waiting to be serviced.
	 */
	if (mask == 0) {
		/*
		 * If Message coprocessor, wake up other cpu to set AST.
		 * Otherwise, set AST on this cpu.
		 */
		if (mcmsg_mp_enable) {
			mp_ast(0);
		} else {
			ast_on(cpu_number(), AST_MCMSG);
		}
	}

	return 0;
}

/*
 *	Routine:
 *		mcmsg_nx_send()
 *		NX send system call.
 *
 *	Arguments:
 *		mt               mcmsg task structure
 *		msg_type         NX message type
 *		buf              message buffer
 *		count            message byte count
 *		node             destination node
 *		dest_ptype       destination ptype
 *		source_ptype     source ptype
 *		nxreq            pointer to NX request structure
 *		originating_node originator of broadcast
 *
 *	Purpose:
 *		The system call is used to initiate an NX send.
 *
 *	Returns:
 *		0 if OK.
 *		-1 if error. 
 */
mcmsg_nx_send(mt, msg_type, buf, count, node, dest_ptype,
              source_ptype, nxreq, originating_node)
	mcmsg_task_t		*mt;
	long			msg_type;
	unsigned long		buf;
	long			count;
	unsigned long		node;
	long			dest_ptype;
	long			source_ptype;
	user_pointer_t		nxreq;
	unsigned long		originating_node;
{
	register long		dest_pid;
	register long		i;
	register unsigned long	sequence;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register select_item_t	*st;
	register select_item_t	*sh;

TIMESTAMP(0) /* START OF SEND */

	sequence = mcmsg_send_sequence;
	mcmsg_send_sequence = sequence + 1;

	/*
	 * Lookup remote process.
	 */
	dest_pid = mcmsg_remote_pid(mt, node, dest_ptype);
	if (dest_pid != -1) {
		pid_si = mcmsg_lookup_remote(mt, dest_pid);
	} else {
		if (node < 0 || node > mt->numnodes) {
			nxreq_t *np;

			mcmsg_trace_debug("nx send: node error", 0, 0, 0, 0, 0);
			np = (nxreq_t *)mcmsg_validate_line(mt, nxreq);
			if (np != 0) {
				np->err = NX_ERR_NODE;
				np->state = NX_COMPLETE;

				/* check for handler request */
				if (np->handler != 0) {
					mcmsg_hreq_ast(mt->task,
						       nxreq);
				}
			}
			return -1;
		}
		pid_si = 0;
	}

#if BIGPKTS
	/*
	 * Send Shortcut: if we meet all of the following conditions:
	 *
	 *     - We are attached to this process.
	 *	   - It is not a local send.
	 *     - We are not busy already sending.
	 *     - No other messages queued for this process.
	 *     - The receiver has room to buffer the message.
	 *	   - The entire message can be sent here.
	 *
	 *	skip allocating and building the si and avoid code 
	 *	path through mcmsg_send().
	 *
	 */
	if ((pid_si != 0) && 
		(pid_si->ppid.route != 0) &&
	    (pid_si->ppid.send_ready != 0) &&
	    (pid_si->ppid.send_wait  == 0))  {

		/* Receiving process is ready */	

		nic_reg	status;
		nxreq_t	*np;
		long	take;
		long	give;

		if ((mcmsg_send_waiting == 0) &&     /* No LTU msg on it's way out. */
			(mcmsg_hw.send_int_enable == 0) && /* Network not busy */
			((take = mcmsg_calculate_take(mt, (count+2*sizeof(xmsg_t)-1) &
			~(sizeof(xmsg_t)-1), pid_si->ppid.send_avail)) >= count) &&
			(count <= mt->applinfo.pkt_size)) { /* Can send whole msg */

			status.full = NIC.status.full;
			if (status.halfs.lo & NIC_STAT_TX_FIFO_EMPTY)  {

				/* Network ready */

				mcmsg_trace_debug("quiksend typ seq pid", 3,
				                  msg_type, sequence, pid_si->value, 0);

				give = pid_si->ppid.recv_give;
				pid_si->ppid.recv_give = 0;
				pid_si->ppid.send_avail -= take;

				if (count == 0)  {
					mcmsg_trace_send(
						(MCTRL_NX1 | (give << 11)),
						0 | (sequence << 16),
						pid_si->value,
						2, take, 0);
					mcmsg_send_hdr10_eod(pid_si->ppid.route,
									MCTRL_NX1 | (give << 11),
									(sequence << 16),
									mt->pid,      take,
									source_ptype, pid_si->value,
									dest_ptype,   msg_type,
									0,            originating_node);
				} else  {	/* Count > 0) */
					register unsigned long  bp1;
					register unsigned long  bp2;
					unsigned long  pkt;

					pkt = (count + NX_PKT_GRAN-1) & ~(NX_PKT_GRAN-1);
					bp1 = mcmsg_validate_read1(buf, pkt, mt->dirbase);
					bp2 = mcmsg_validate2();
					if (bp1 != 0) {
						mcmsg_trace_send(
							(MCTRL_NX1 | (give << 11)),
							pkt | (sequence << 16),
							pid_si->value,
							2, take, 0);
						mcmsg_send_hdr10(pid_si->ppid.route,
							MCTRL_NX1 | (give << 11),
							pkt | (sequence << 16),
							mt->pid,      take,
							source_ptype, pid_si->value,
							dest_ptype,   msg_type,
							count,        originating_node);
						mcmsg_send_buf(bp1, bp2, pkt);
					} else {
						pid_si->ppid.recv_give = give;
						pid_si->ppid.send_avail += take;
						goto no_shortcut;
					}
				}

				/* Send complete */

				np = (nxreq_t *)mcmsg_validate_line(mt, nxreq);
				if (np != 0) {
					np->state = NX_COMPLETE;
				} else {
					mcmsg_trace_drop("nxreq nx1", si->nxrq.request);
					mcmsg_msg_drop++;
					return(0);
				}

				/* check for handler request */

				if (np->handler != 0) {
					mcmsg_trace_debug("hsend ast short", 1,
					                   np->hparam, 0, 0, 0);
					mcmsg_hreq_ast(mt->task, nxreq);
				}

				/*
				 * Check send_wait queue.
				 * Even though send_wait was null when we entered
				 * this shortcut, a send could have been queued
				 * while we were waiting on this send.
				 */
				if (pid_si->ppid.send_avail > 0 && 
				    pid_si->ppid.send_wait != 0) {
					mcmsg_release_send_wait(mt, pid_si);
				}
				return(0);
			}
		}
	}
#endif BIGPKTS

no_shortcut:
	/*
	 * Build the send select_item.
	 */
	si = mcmsg_alloc_select_item();
	assert(si != 0);
	si->item = (void *)si;
	si->nxrq.request = nxreq;
	si->value = MCMSG_MODULE_NX;
	si->method = 0;
	si->nextmethod = MCTRL_NXN;
	si->mcmsg_task = mt;
	si->nxrq.pid_si = pid_si;
	si->nxrq.dest_node = node;
	si->nxrq.originating_node = originating_node;
	si->nxrq.dest_ptype = dest_ptype;
	si->nxrq.source_ptype = source_ptype;
	si->nxrq.msg_type = msg_type;
	si->nxrq.buf = buf;
	si->nxrq.count = count;
	si->nxrq.offset = 0;
	si->nxrq.stop = count;
	si->nxrq.take = 0;
	si->nxrq.sequence = sequence;
	si->nxrq.xmsg = 0;
	si->nxrq.vm_ast_pending = 0;

	/*
	 * If pid is known, schedule the send.
	 * Otherwise initiate INQUIRY sequence.
	 */
	if (pid_si == 0) {
		mcmsg_inquire(mt, si);
	} else {
		mcmsg_schedule_send(mt, si);
	}
	return 0;
}

/*
 *	Routine:
 *		mcmsg_schedule_send
 *
 *	Arguments:
 *		mt		mcmsg_task for task
 *		si		send select_item
 *
 *	Purpose:
 *		Schedule a new NX send with flow control.
 *
 *	Returns:
 *		None.
 */
mcmsg_schedule_send(mt, si)
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
{
	register unsigned long	take;
	register select_item_t	*pid_si;
	register select_item_t	*st;
	register select_item_t	*sh;

	pid_si = si->nxrq.pid_si;

	if (pid_si->ppid.send_ready == 0) {
		/*
		 * The process is not attached.
		 */
		mcmsg_attach(mt, si);
	} else if (pid_si->ppid.send_wait != 0) {

		/* Other sends queued ahead of this one: PTS#8850 */
		/* Queue this send at the tail of send_wait. */

		mcmsg_trace_debug("sched queue", 2, si, si->nxrq.msg_type, 0, 0);
		si->method = 0;
		si->nextmethod = MCTRL_NX1;
		st = pid_si->ppid.send_wait;
		pid_si->ppid.send_wait = si;
		sh = st->link;
		si->link = sh;
		st->link = si;

		/* do the NXM protocol under process lock */

		if (pid_si->ppid.process_lock) {
			mcmsg_send(mt, MCTRL_NXM, si, si->nxrq.sequence);
		} 

		/* attempt to send the head of the send_wait queue. */

		if (pid_si->ppid.send_avail > 0) {
			mcmsg_release_send_wait(mt, pid_si);
		}

	} else {
	    assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);
	    take = mcmsg_calculate_take(mt,
				    (si->nxrq.count + 2*sizeof(xmsg_t)-1) &
		    			~(sizeof(xmsg_t)-1),
				    pid_si->ppid.send_avail);

	    if (take > 0) {
	    	mcmsg_trace_debug("sched send", 4, si, si->nxrq.msg_type, take, 
			pid_si->ppid.send_avail);
		/*
		 * Initiate the send.
		 */
		mcmsg_start_nx_send(mt, si, pid_si, take);
	    } else {
		/*
		 * Queue the send
		 * if PLOCK do a request to send.
		 */
		st = pid_si->ppid.send_wait;
		pid_si->ppid.send_wait = si;
		mcmsg_trace_debug("sched no room", 3, si, st, pid_si, 0);
		if (st == 0) {
			si->link = si;
			if (pid_si->ppid.process_lock) {
				si->method = MCTRL_NXS;
				mcmsg_send(mt,MCTRL_NXS, si, si->nxrq.sequence);
			} else {
				si->method = 0;
				si->nextmethod = MCTRL_NX1;
			}
		} else {
			sh = st->link;
			assert(sh != 0 && sh->method != 0xdead);
			si->link = sh;
			st->link = si;
			if (pid_si->ppid.process_lock) {
				si->method = 0;
				si->nextmethod = MCTRL_NX1;
				mcmsg_send(mt, MCTRL_NXM, si, si->nxrq.sequence);
			} else {
				si->method = 0;
				si->nextmethod = MCTRL_NX1;
			}
		}
	    }
	}
}

/*
 *	Routine:
 *		mcmsg_calculate_take
 *
 *	Arguments:
 *		mt		mcmsg_task structure
 *		take	requested take
 *		avail	amount of uncommitted buffer space
 *
 *	Purpose:
 *		Calculate a take value for an NX message.
 *		The take value specifies how much of the
 *		message the receiver can buffer at this point.
 *
 *	Returns:
 *		amount of take granted
 */
mcmsg_calculate_take(mt, take, avail)
	register mcmsg_task_t	*mt;
	register unsigned long	take;
	register unsigned long	avail;
{

	if (avail >= 
		   mt->applinfo.send_threshold + sizeof(xmsg_t)) {
		if (take > mt->applinfo.send_count + sizeof(xmsg_t)) {
			take = mt->applinfo.send_count + sizeof(xmsg_t);
		}
		if  (take > avail) {
			take = avail;
		}
		return take;
	} else if (avail >=
			mt->applinfo.pkt_size + sizeof(xmsg_t)) {
		if (take > mt->applinfo.pkt_size + sizeof(xmsg_t)) {
			take = mt->applinfo.pkt_size + sizeof(xmsg_t);
		}
		return take;
	} else if (avail >= take) {
		return take;
	}
	return 0;
}

/*
 *      Routine:
 *	      mcmsg_start_nx_send
 *
 *      Arguments:
 *	      mt	      mcmsg_task structure
 *	      si	      message item
 *	      pid_si  destination process item
 *	      take    calculated take for the message
 *
 *      Purpose:
 *	      Initiate a new NX send.
 *
 *      Returns:
 *	      None.
 */
mcmsg_start_nx_send(mt, si, pid_si, take)
	register mcmsg_task_t   *mt;
	register select_item_t  *si;
	register select_item_t  *pid_si;
	register unsigned long  take;
{
	mcmsg_trace_debug("start nx send si tp tk", 3,
		si, si->nxrq.msg_type, take, 0);

	si->nextmethod = MCTRL_NXN;
	si->nxrq.take = take;
	si->nxrq.stop = take - sizeof(xmsg_t);
	assert(pid_si->ppid.send_avail >= take);
	pid_si->ppid.send_avail -= take;

	mcmsg_send(mt, MCTRL_NX1, si);
	return;
}

/*
 *	Routine:
 *		mcmsg_nx_probe()
 *		NX probe system call.
 *
 *	Arguments:
 *		mt               mcmsg task structure
 *		typesel          NX message type selector
 *		nodesel          source node selector
 *		ptypesel         source ptype selector
 *		ptype            destination ptype
 *		nxreq            pointer to NX request structure
 *
 *	Purpose:
 *		The system call is used to probe for buffered message with no
 *	  matching recv().
 *
 *	Returns:
 *		0 if OK.
 *		-1 if error. 
 */
mcmsg_nx_probe(mt, typesel, nodesel, ptypesel, ptype, nxreq)
	mcmsg_task_t	*mt;
	long		typesel;
	long		nodesel;
	long		ptypesel;
	long		ptype;
	user_pointer_t	nxreq;
{
	register user_pointer_t	scanp;
	register user_pointer_t	xmsg;
	register nxreq_t	*np;
	register xmsg_t	*xp;
	int                     xt;
	int                     t;

	np = (nxreq_t *)mcmsg_validate_line(mt, nxreq);
	if (np == 0) {
		mcmsg_trace_drop("recvx probe nxreq", nxreq);
		mcmsg_msg_drop++;
		return -1;
	}
	/*
	 * Look for an XMSG that matches the request.
	 */
	xmsg = 0;
	scanp = mt->xmsg_rdy;
	if (scanp != 0) {
		assert((t = MAXLOOP) != 0);
		while (scanp != mt->xmsg_head) {
			xp = (xmsg_t *)mcmsg_validate_line(mt, scanp);
			if (xp == 0) {
				mcmsg_trace_drop("scan ptr", scanp);
				return -1;
			}
			if (xp->state == XMSG_FREE) {
				mcmsg_trace_drop("scan state", scanp);
				return -1;
			}
			if ((mcmsg_nx_match(typesel,
					   xp->msg_type,
					   nodesel,
					   xp->source_node,
					   ptypesel,
					   xp->source_ptype)) &&
				/* don't match probe to msg with recv */
				/* PTS 7085 */
			    ((xp->si == 0) || (xp->si->nxrq.request == 0))) {

				/* Match */
				mcmsg_trace_debug(" iprobe scan retd", 4,
						scanp, xp->state,
						xp->msg_type, mt->xmsg_head);

				np->xmsg = (xmsg_t *)scanp;
				np->state = NX_BUFFERED;
				return 0;
			}
			/* Skip over chained buffer elements */
			assert((xt = MAXLOOP) != 0);
			while (xp->chain_number != 0) {
				scanp = (user_pointer_t)xp->link;
				xp = (xmsg_t *)mcmsg_validate_line(mt, scanp);
				if (xp == 0) {
					mcmsg_trace_drop("xmsg link", scanp);
					return -1;
				}
				assert(xt-- != 0);
			}
			/*
			 * scanp now points to the last buffer element in a
			 * chain -- now bump it to the start of the next xmsg
			 * buffer chain
			 */
			scanp = (user_pointer_t)xp->link;
			assert(t-- != 0);
		}
	}
	np->xmsg = (xmsg_t *)0;
	np->state = NX_COMPLETE;
	return 0;
}

/*
 *	Routine:
 *		mcmsg_nx_recvx()
 *		NX recv system call.
 *
 *	Arguments:
 *		mt               mcmsg task structure
 *		typesel          NX message type selector
 *		buf              message buffer
 *		count            message byte count
 *		nodesel          source node selector
 *		ptypesel         source ptype selector
 *		ptype            destination ptype
 *		nxreq            pointer to NX request structure
 *
 *	Purpose:
 *		The system call is used to post an NX receive.
 *
 *		It is also used as a probe system call which is
 *		indicated by a 'count' parameter of -1 which immediately
 *	  vectors off to mcmsg_nx_probe().
 *
 *	Returns:
 *		0 if OK.
 *		-1 if error. 
 */
mcmsg_nx_recvx(mt, typesel, buf, count, nodesel, ptypesel, ptype, nxreq)
	mcmsg_task_t	*mt;
	long		typesel;
	unsigned long	buf;
	long		count;
	long		nodesel;
	long		ptypesel;
	long		ptype;
	user_pointer_t	nxreq;
{
	register int		app;
	register int		i;
	register user_pointer_t	scanp;
	register select_item_t	*si;
	register select_item_t	*pid_si;
	register select_item_t	*pid_last;
	register select_item_t	*sel;
	register select_item_t	*sl;
	register select_item_t	*path;
	register user_pointer_t	xmsg;
	register user_pointer_t	user_xxp;
	register nxreq_t	*np;
	register unsigned long	*ip;
	register xmsg_t         *xxp;
	register xmsg_t         *xprevp;
	register xmsg_t		*xnext;
	int                     t;
	int                     xt;

	if (count == -1) {
		i = mcmsg_nx_probe(mt,typesel,nodesel,ptypesel,ptype,nxreq);
	  return(i);
	}
	/*
	 * Look for an XMSG that matches the request.
	 */
	xmsg = 0;
	scanp = mt->xmsg_rdy;
	if (scanp != 0) {
		register xmsg_t	*xp;

		assert((t = MAXLOOP) != 0);
		while (scanp != mt->xmsg_head) {
			xp = (xmsg_t *)mcmsg_validate_line(mt, scanp);
			if (xp == 0) {
				mcmsg_trace_drop("scan ptr", scanp);
				return -1;
			}
			if (xp->state == XMSG_FREE) {
				mcmsg_trace_drop("scan state", scanp);
				return -1;
			}

			/* Only match xmsg's with no recv attached (PTS #9187) */

			if (((xp->si == 0) || (xp->si->nxrq.request == 0)) &&
				mcmsg_nx_match(typesel,
				               xp->msg_type,
				               nodesel,
				               xp->originating_node,
				               ptypesel,
				               xp->source_ptype)) {

				/* Match */
				mcmsg_trace_debug(" scan retd", 4,
						scanp, xp->state,
						xp->msg_type, mt->xmsg_head);

				/* Find last xmsg in the chain */
				xxp = xp;
				user_xxp = scanp;
				assert((xt = MAXLOOP) != 0);
				for (;;) {
					if (xxp->chain_number == 0)
						break;
					user_xxp = (user_pointer_t)xxp->link;
					xxp = (xmsg_t *)mcmsg_validate_line(
								mt, user_xxp);
					if (xxp == 0) {
						mcmsg_trace_drop("xmsg link",
						                  xxp);
						return -1;
					}
					assert(xt-- != 0);
				}

				/*
			 	* Unlink from xmsg_rdy list
			 	*/
				if (xp->backlink == 0) {
					mt->xmsg_rdy =
				    	(user_pointer_t)xxp->link;
				} else {
					xprevp = (xmsg_t *)
					mcmsg_validate_line(mt, xp->backlink);
					if (xprevp == 0) {
						mcmsg_trace_drop("xmsg backlink",
							xp->backlink);
						return -1;
					}
					xprevp->link = xxp->link;
				}

				if (xxp->link != 0) {
					xnext = (xmsg_t *)
					mcmsg_validate_line(mt, xxp->link);
					if (xnext == 0) {
						mcmsg_trace_drop("xmsg link",
								mt->xmsg_rdy);
							return -1;
					}
					xnext->backlink = xp->backlink;
				}

				if (mt->xmsg_rdy_end == user_xxp) {
					mt->xmsg_rdy_end =
					   (user_pointer_t)xp->backlink;
				}

				xp->backlink = 0;
				xxp->link = 0;

				if ( xp->length <= count &&
				    xp->totalsize < xp->length) {
					si = xp->si;
					if (mcmsg_check_si(mt, si) == -1) {
						return -1;
					}
					/*
					 * If process_lock then set
					 * stop to the end of the
					 * message, but...if the xmsg
					 * is not yet full, we 
					 * are caught in the
					 * WAIT/CONTINUE cycle as if 
					 * process lock were off.
					 */
					if (mt->applinfo.process_lock && 
					    xp->state == XMSG_FULL) {
						si->method = SELMETH_RECV_ANY;
						si->nxrq.stop = count;
						si->nxrq.take = 0;
					/* Set take 0 here so mcmsg_recv_nxn_rq() does not
				 	 * relinquish buffer memory already accounted for in
					 * mcmsg_recv_nx1() when the xmsg was allocated. 
					 */
					} else {
						si->method = SELMETH_RECV_XMSG;
						if (count < xp->totalsize) {
							si->nxrq.stop = count;
						}
					}
					/*
					 * buf cannot point to a split
					 * xmsg buffer here
					 */
					si->nxrq.buf = buf;
					si->nxrq.request = nxreq;
				} else {
					mcmsg_trace_debug("si check recvx",
						4, xp->si, xp->length, xp->totalsize, xp->state);
					if (xp->si != 0) {
						if (mcmsg_check_si(mt, xp->si) == -1) {
							mcmsg_trace_debug("si check recvx b",
								2, xp->si, xp->si->method, 0, 0);
						} else {
							xp->si->nxrq.request = nxreq;
						}
					}
				}

				np = (nxreq_t *)mcmsg_validate_line(mt, nxreq);
				if (np != 0) {
					np->xmsg = (xmsg_t *)scanp;

					/* Copy buffer, provoke sender if more */
					if (xp->state == XMSG_FULL) {
						/* check for handler request */
						if (np->handler != 0) {
						    np->state = NX_BUFFERED;
							mcmsg_trace_debug("hrecv ast recvx i", 
						                   1, np->hparam, 0, 0, 0);
							mcmsg_hreq_ast(mt->task,nxreq);
						} else if ((np->monitored == 0) &&
								   (xp->length > xp->totalsize)) {
							mcmsg_trace_debug("cont ast full recvx",
								4, np, np->xmsg, xp->state, np->state);
							/* protect ast from library completing request */
							mcmsg_recv_cont_ast(mt->task, nxreq);
						} else {
						    np->state = NX_BUFFERED;
						}
					} else if (xp->state == XMSG_STOP) {
						if (np->monitored == 0) {
							/* Request is not monitored by main thread */
							mcmsg_trace_debug("cont ast stop recvx",
								4, np, np->xmsg, xp->state, np->state);
							mcmsg_recv_cont_ast(mt->task, nxreq);
						} else {
							/* monitored by thread, thread transfers buf */
							np->state = NX_BUFFERED;
						}
					}
				} else {
						mcmsg_trace_drop("recvx nxreq", nxreq);
						mcmsg_msg_drop++;
				}
				return 0;

			}

			/* Skip over chained buffer elements */
			assert((xt = MAXLOOP) != 0);
			while (xp->chain_number != 0) {
				scanp = (user_pointer_t)xp->link;
				xp = (xmsg_t *)mcmsg_validate_line(mt, scanp);
				if (xp == 0) {
					mcmsg_trace_drop("xmsg link", scanp);
					return -1;
				}
				assert(xt-- != 0);
			}
			/*
			 * scanp now points to the last buffer element in a
			 * chain -- now bump it to the start of the next xmsg
			 * buffer chain
			 */
			scanp = (user_pointer_t)xp->link;
			assert(t-- != 0);
		}
	} 

	/*
	 * No XMSG was found, construct a request.
	 */
	si = mcmsg_alloc_select_item();
	mcmsg_trace_debug("store recv", 1, si, 0, 0, 0);
	assert(si != 0);
	si->method = SELMETH_RECV_ANY;
	si->item = (void *)si;
	si->nxrq.request = nxreq;
	si->mcmsg_task = mt;
	si->nxrq.pid_si = 0;
	si->nxrq.dest_node = nodesel;
	si->nxrq.dest_ptype = ptypesel;
	si->nxrq.source_ptype = ptype;
	si->nxrq.msg_type = typesel;
	si->nxrq.count = count;
	/* buf cannot point to a split xmsg buffer here */
	si->nxrq.buf = buf;
	si->nxrq.offset = 0;
	si->nxrq.stop = 0;
	si->nxrq.take = 0;
	si->nxrq.xmsg = 0;
	si->nxrq.sequence = 0;

	/*
	 * Search for a process ready to send on avail_need.
	 */
	pid_si = mt->avail_need;
	if (mt->applinfo.process_lock && pid_si != 0) {

		assert((t = MAXLOOP) != 0);
		for (;;) {
			pid_last = pid_si;
			pid_si = pid_si->ppid.avail_link;
			assert(pid_si->method == SELMETH_PID);
			if (pid_si->ppid.rk_recv_pid != 0 &&
			    mcmsg_nx_match(typesel,
					   pid_si->ppid.rk_recv_type,
					   nodesel,
					   pid_si->ppid.node,
					   ptypesel,
					   pid_si->ppid.rk_recv_ptype)) {

				si->nxrq.pid_si = pid_si;
				si->nxrq.sequence = pid_si->ppid.rk_recv_seq;
				si->nxrq.take = 0;
				ip = (unsigned long *)
				     mcmsg_validate_long(mt, 
					((nxreq_t *)(si->nxrq.request))
					 ->localinfo);
				if (ip != 0) {
					ip[0] = pid_si->ppid.rk_recv_type;
					ip[1] = pid_si->ppid.rk_recv_want;
					ip[2] = pid_si->ppid.rk_recv_originating_node;
					ip[3] = pid_si->ppid.rk_recv_ptype;
					ip[4] = pid_si->ppid.node;
				}
				if (pid_si->ppid.rk_recv_want > 0) {
					mcmsg_install_sequence(mt, pid_si->value, si);
				} else {
					np = (nxreq_t *)
					 mcmsg_validate_line(mt, si->nxrq.request);
					if (np != 0) {
						np->state = NX_COMPLETE;

						/* check for hrecv handler */
						if (np->handler != 0) {
							mcmsg_trace_debug("hrecv ast recvx", 1, 
									   np->hparam, 0, 0, 0);
							mcmsg_hreq_ast(mt->task, si->nxrq.request);
						}
							
						mcmsg_trace_debug("rw complete",
								2,
								si, np, 0, 0);
					}
					mcmsg_free_select_item(si);
				}
				mcmsg_send(mt, MCTRL_NXR,
					   si,
					   pid_si->ppid.rk_recv_seq);
				pid_si->ppid.rk_recv_pid = 0;
				if (pid_si->ppid.recv_target ==
				    pid_si->ppid.recv_total ||
				    pid_si->ppid.recv_target == 0) {
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
				}
				return 0;
			}
			if (pid_si == mt->avail_need) {
				break;
			}
			assert(t-- != 0);
		}
	}

	/*
	 * No matching messages for request.
	 * Put recv request on selection_path.
	 */
	path = mt->selection_path;
	if (typesel >= 0) {

		if (path == 0) {
			mt->selection_path = si;
			si->link = si;
			if (nodesel != -1 || ptypesel != -1) {
				si->method = SELMETH_RECV_TYPESRC;
			} else {
				si->method = SELMETH_RECV_TYPE;
			}
			mcmsg_trace_debug("store recv 1st one", 0, 0, 0, 0, 0);
		} else {
			if (path->method != SELMETH_RECV_TYPESEL) {
				sel = mcmsg_alloc_select_item();
				assert(sel != 0);

				sel->method = SELMETH_RECV_TYPESEL;
				sel->value = -1;
				sel->mcmsg_task = mt;
				sel->item = (select_t *)
				      mcmsg_l2malloc(l2size(sizeof(select_t)));
				if (sel->item == 0) {
					mcmsg_trace_drop("no free selector",
							typesel);
					mcmsg_free_select_item(sel);
					mcmsg_free_select_item(si);
					return -1;
				}
				mcmsg_selector_init(sel->item,
						    SELMETH_RECV_TYPESEL);
				sel->link = path->link;
				path->link = sel;
				mt->selection_path = sel;
				path = sel;
				assert((t = MAXLOOP) != 0);
				sl = sel->link; 
				while(sl != path) {
				    if((sl->method == SELMETH_RECV_TYPE) ||
				       (sl->method == SELMETH_RECV_TYPESRC)) {
					sel->link = sl->link;
					mcmsg_selector_install_si(mt,
							path->item,
							sl,
							sl->nxrq.msg_type,
							sl->method);
					sl = sel->link;
				    } else {
					sel = sl; sl = sl->link;
				    }
				    assert(t-- != 0);
				}
			}
			mcmsg_trace_debug("store new recv sel ", 1, 
						path->item,0,0,0); 
			if (nodesel != -1 || ptypesel != -1) {
				si->method = SELMETH_RECV_TYPESRC;
			} else {
				si->method = SELMETH_RECV_TYPE;
			}
			mcmsg_selector_install_si(mt,
						  path->item,
						  si,
						  typesel,
						  si->method);
		}
	} else {
		if (typesel == -1) {
			if (nodesel != -1 || ptypesel != -1) {
				si->method = SELMETH_RECV_SRC;
			} else {
				si->method = SELMETH_RECV_ANY;
			}
		} else {
			si->method = SELMETH_RECV_TYPESET;
		}
		if (path == 0) {
			mt->selection_path = si;
			si->link = si;
		} else {
			si->link = path->link;
			path->link = si;
		}
	}

	/*
	 * If nodesel and ptypesel are specified,
	 * send a Receive Advisory Control Packet (NXQ).
	 */
	if (nodesel != -1 && ptypesel != -1) {
		register long	pid;

		pid = mcmsg_remote_pid(mt, nodesel, ptypesel);
		if (pid != -1) {
			pid_si = mcmsg_lookup_remote(mt, pid);
			if (pid_si != 0 &&
			    pid_si->ppid.recv_total == pid_si->ppid.recv_give) {
				si->nxrq.pid_si = pid_si;
				mcmsg_send(mt, MCTRL_NXQ, si, 0);
			}
		}
	}

	return 0;
}

/*
 *	Routine:
 *		mcmsg_send_continue (send_item)
 *
 *	Arguments:
 *		send_item	send select_item to continue
 *
 *	Purpose:
 *		The system call is used to continue a send that has been
 *		queued. The send select_item must be on the send_wait
 *		queue of the destination pid select_item.
 *
 *		The send may have been queued because the page was
 *		previously not present in physical memory, and now
 *		it should be.
 *
 *	Returns:
 *		0 if OK.
 *		-1 if error. 
 */
mcmsg_nx_send_continue(mt, send_item)
	mcmsg_task_t	*mt;
	select_item_t	*send_item;
{
	register select_item_t  *pid_si;
	register select_item_t  *st, *sh, *si;
	int t;

	/*
	 * Destination pid is pointed to by the send_item.
	 */
	pid_si = send_item->nxrq.pid_si;
	assert(pid_si != 0);

	/* PTS 10821: if pid_si is == 0xdead, remote process
	   must have exited while the ast was pending on this si.  The
	   mcmsg_recv_prm() code would free the pid_si, so we must now
	   free the send_item.
	 */
	if (pid_si == (select_item_t *) 0xdead) {
	        register nxreq_t        *req;
                mcmsg_trace_debug("nx_send_continue: dead pid_si ", 1, pid_si, 0, 0, 0);
		req = (nxreq_t *)mcmsg_validate_long(mt, send_item->nxrq.request);
		if (req == 0) {
                	mcmsg_trace_debug("nx_send_continue: bad request ", 1, send_item->nxrq.request, 0, 0, 0);
			return 0;
		}
		req->state = NX_COMPLETE;
		mcmsg_free_select_item(send_item);
		return 0;
	}




	/*
	 * Find send_item in the send_wait list.
	 */

	st = pid_si->ppid.send_wait;
	if (st == 0) {
		/* List is empty - ERROR */
		mcmsg_trace_drop("no send cont waiting", 0);
		mcmsg_msg_drop++;
		return -1;
	}
	sh = st->link;
	assert(sh != 0);
	si = st;
	assert((t = MAXLOOP) != 0);
	for (;;) {
		if (sh == send_item) {
			/* Found it! */
			break;
		}
		if (sh == si) {
			/* It's not in the list - ERROR */
			mcmsg_trace_drop("no send cont waiting", 1);
			mcmsg_msg_drop++;
			return -1;
		}
		st = sh;
		sh = st->link;
		assert(t-- != 0);
	}

	/* Remove it from the list */
	if (sh == st) {
		pid_si->ppid.send_wait = si = 0;
	} else {
		if (sh == si) {
			pid_si->ppid.send_wait = st;
		}
		st->link = si = sh->link;
		assert(si != 0 && si->method != 0xdead);
	}

	/*
	 * Clear vm_ast_pending.
	 *
	 * NOTE: This assumes that this function was called
	 *       by the VM AST handler.
	 */
	send_item->nxrq.vm_ast_pending = 0;

	/*
	 * Continue the send.
	 */
	if (send_item->nextmethod == MCTRL_NX1) {
		send_item->nextmethod = MCTRL_NXN;
		mcmsg_send(mt, MCTRL_NX1, send_item);
	} else if (send_item->nextmethod == MCTRL_NXN) {
		mcmsg_send(mt, MCTRL_NXN, send_item, pid_si,
			send_item->nxrq.buf + send_item->nxrq.offset,
			send_item->nxrq.count - send_item->nxrq.offset,
			mt->applinfo.pkt_size,
			send_item->nxrq.offset,
			send_item->nxrq.stop);
	} else {
		mcmsg_send(mt, send_item->nextmethod, send_item, send_item->nxrq.sequence);
	}

	/* done */
	
	return 0;
}

/*
 *	Routine:
 *		mcmsg_nx_match
 *
 *	Arguments:
 *		typesel		type selector
 *		type		type to match
 *		nodesel		node selector
 *		node		node to match
 *		ptypesel	ptype selector
 *		ptype		ptype to match
 *
 *	Purpose:
 *		Determine a selection match based on
 *			message type
 *			node
 *			ptype
 *
 *	Returns:
 *		0 no match
 *		1 match
 */
mcmsg_nx_match(typesel, type, nodesel, node, ptypesel, ptype)
	register long	typesel;
	register long	type;
	register long	nodesel;
	register long	node;
	register long	ptypesel;
	register long	ptype;
{

	if (typesel != -1) {
		if (typesel >= 0) {
			if (typesel != type) {
				return 0;
			}
		} else if (type >= 30) {
			if ((typesel & 0x40000000) == 0) {
				return 0;
			}
		} else if ((typesel & (1 << type)) == 0) {
			return 0;
		}
	} else {
		if (type >= RESERVED_BASE_TYPE) {
			return 0;
		}
	}

	if (nodesel != -1 && node != nodesel) {
		return 0;
	}
	if (ptypesel != -1 && (ptype & ~GLOBAL_BIT) != ptypesel) {
		return 0;
	}
	return 1;
}

mcmsg_check_si(mt, si)
	mcmsg_task_t	*mt;
	select_item_t	*si;
{

	if ((unsigned long)si < (unsigned long)mcmsg_memory ||
	    (unsigned long)si >=
		(unsigned long)&mcmsg_memory[MMSIZE/sizeof(long)] ||
	    ((unsigned long)si & 0xf) != 0) {
		mcmsg_trace_drop("invalid si", si);
		mcmsg_msg_drop++;
		return -1;
	}
	if (si->method != SELMETH_RECV_XMSG &&
	     si->method != SELMETH_RECV_ANY ||
	    si->mcmsg_task != mt) {
		mcmsg_trace_drop("wrong kind item", si);
		mcmsg_msg_drop++;
		return -1;
	}
	return 0;
}

mcmsg_nx_recv_continue(mt, si)
	mcmsg_task_t	*mt;
	register select_item_t	*si;
{
	register int		i;
	register nxreq_t	*np;
	register xmsg_t		*xp;
	int                     t;

	if (mcmsg_check_si(mt, si) == -1) {
		return -1;
	}
	np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
	if (np == 0) {
		mcmsg_trace_drop("recv cont np", si->nxrq.request);
		return -1;
	}
		
	si->nxrq.xmsg = (user_pointer_t)np->xmsg;
	switch(si->method) {
	case SELMETH_RECV_ANY:
		mcmsg_send(mt, MCTRL_NXR, si, si->nxrq.sequence);
		break;

	case SELMETH_RECV_XMSG:
		mcmsg_send(mt, MCTRL_NXC, si, si->nxrq.sequence);
		break;
	}

	return 0;
}

select_item_t *
mcmsg_selector_detach_request(mt, sel, request)
	register mcmsg_task_t	*mt;
	register select_t	*sel;
	register user_pointer_t	request;
{
	register select_item_t	*si;
	register select_item_t	*st;
	register select_item_t	*sl;
	register void		**selh;
	register unsigned long	hi;
	register nxreq_t	*np;
	register long		msg_type;

	np = (nxreq_t *)mcmsg_validate_line(mt, request);
	if (np == 0) {
		mcmsg_trace_drop("cancel nx no req", request);
		return 0;
	}
	if (np->type < 0) {
		mcmsg_trace_drop("cancel detach neg type", np->type);
		return 0;
	}
	if (np->type == 0) {
		selh = &sel->zero;
	} else {
		hi = SELECT_HASH_FUN(np->type);
		selh = &sel->hash[hi];
	}
	st = *selh;
	assert(st != 0);
	sl = st;
	si = st->link;
	if (si == st) {
		if (si->nxrq.request != request)
			return 0;
		*selh = 0;
	} else {
		unsigned long t;

		assert((t = MAXLOOP) != 0);
		for (;;) {
			if (si->nxrq.request == request)
				break;
			if (si == st)
				return 0;
			sl = si;
			si = si->link;
			assert(t-- != 0);
		}
		sl->link = si->link;
		if (si == st) {
			*selh = sl;
		}
	}
	return si;
}

mcmsg_nx_cancel(mt, request, statusp)
	register mcmsg_task_t	*mt;
	register user_pointer_t	request;
	register long		*statusp;
{
	register int		i;
	register select_t	*sel;
	register select_item_t	*st;
	register select_item_t	*si;
	register select_item_t	*sl;
	register int		t;
	register long		*sp;
        register nxreq_t        *req;

	/*
	 * get the physical address of the send request
	 */
	sp = (long *)mcmsg_validate_long(mt, statusp);
	if (sp == 0) {
		return 0;
	}

	req = (nxreq_t *)mcmsg_validate_long(mt, request);
        if (req == 0) {
		*sp = NX_BUFFERED;
                return 0;
        }
 
        if (req->req == NX_RECV_REQ) {
	    st = mt->selection_path;
	    if (st == 0) {
		mcmsg_trace_debug("can none", 0, 0, 0, 0, 0);
		*sp = NX_BUFFERED;
		return 0;
	    }
	    sl = st;
	    si = sl->link;
	    assert((t = MAXLOOP) != 0);
	    for (;;) {
		mcmsg_trace_debug("can walk", 2, si, 
				si != 0? si->method : 0, 0, 0);
		switch (si->method) {

		case SELMETH_RECV_TYPESEL:
			sel = si->item;
			si = mcmsg_selector_detach_request(mt, sel, request);
			if (si == 0) {
				break;
			}
			goto found_cancel_detached;

		case SELMETH_RECV_TYPESRC:
		case SELMETH_RECV_SRC:
		case SELMETH_RECV_TYPESET:
		case SELMETH_RECV_TYPE:
		case SELMETH_RECV_ANY:
			if (si->nxrq.request != request) {
				break;
			}
			if (si == sl) {
				mt->selection_path = 0;
			} else {
				sl->link = si->link;
				if (mt->selection_path == si) {
					mt->selection_path = sl;
				}
			}
		found_cancel_detached:
			mcmsg_trace_debug("cancel", 1, si, 0, 0, 0);
			req->state = NX_COMPLETE;
			mcmsg_free_select_item(si);
			*sp = NX_BUFFERED;
			return 1;

		default:
			assert(0);
		}
		if (si == st) {
			break;
		}
		sl = si;
		si = si->link;
		assert(t-- != 0);
	    }
	} else {        /* send request */
            st = mt->send_wait_unk;
            if (st == 0) {
                mcmsg_trace_debug("can none", 0, 0, 0, 0, 0);
		*sp = NX_BUFFERED;
                return 0;
            }
            sl = st;
            si = sl->link;
            assert((t = MAXLOOP) != 0);
            for (;;) {
                if (si->nxrq.request == request) {
        		req->state = NX_COMPLETE;
                        if (sl == si) {
                                mt->send_wait_unk = 0;
                        } else {
                                if (mt->send_wait_unk->link == si) {
                                        /* if canceled si at head of list
                                           save next si and send PNQ after
                                           removing canceled si from list
                                         */
                                        sel = (select_t *)si->link;
                                } else {
                                        sel = 0;
                                }
                                sl->link = si->link;
                                if (mt->send_wait_unk == si) {
                                        mt->send_wait_unk = sl;
                                }
                                mcmsg_free_select_item(si);
                                if (sel) {
                                        assert(sel != 0);
                                        sel->method = MCTRL_PNQ;
                                        mcmsg_send(mt, MCTRL_PNQ, sel, 0);
                                }
                        }
                        si->link = 0;
                        break;
                }
                if (si == st) {
                        break;
                }
                sl = si;
                si = si->link;
                assert(t-- != 0);
            }
	}

	*sp = NX_BUFFERED;
	return 0;
}


/*
 *	Routine:
 *		mcmsg_find_nx_recv
 *
 *	Arguments:
 *		mt					current mcmsg_task
 *		msg_type			message type
 *		dest_ptype			destination ptype
 *		msg_length			message length
 *		source_node			source node
 *		source_ptype		source ptype
 *		originating_node	origin of broadcast
 *
 *	Purpose:
 *		Find a matching receive request on the selection path
 *		of the current task.
 *
 *	Returns:
 *		si	of matched recv request
 *		0   if no matched recv request
 */
select_item_t *
mcmsg_find_nx_recv(mt,
		   msg_type,
		   dest_ptype,
		   msg_length,
		   source_node,
		   source_ptype,
                   originating_node)
	register mcmsg_task_t	*mt;
	register long		msg_type;
	register long		dest_ptype;
	register unsigned long	msg_length;
	register unsigned long	source_node;
	register long		source_ptype;
	register unsigned long	originating_node;
{
	register select_t	*sel;
	register select_item_t	*si;
	register select_item_t	*sl;
	register select_item_t	*st;
	register int		t;
	register nxreq_t		*np;

	st = mt->selection_path;
	if (st == 0) {
		return 0;
	}

	/*
	 * Search selection path of task for matching receive.
	 */
	sl = st;
	si = sl->link;
	assert((t = MAXLOOP) != 0);
	for (;;) {
		mcmsg_trace_debug("recv walk", 3, si, 
				si != 0? si->method : 0, msg_type, 0);
		switch (si->method) {

		case SELMETH_RECV_TYPESRC:
			if (si->nxrq.msg_type != msg_type) {
				break;
			}
			/* Fall through */

		case SELMETH_RECV_SRC:
		do_src:
			if (si->nxrq.msg_type == -1 && msg_type >= RESERVED_BASE_TYPE) {
				break;
			}
			if (si->nxrq.dest_node != -1 &&
			    si->nxrq.dest_node != originating_node) {
				break;
			}
			if (si->nxrq.dest_ptype != -1 &&
			    si->nxrq.dest_ptype != (source_ptype & ~GLOBAL_BIT)) {
				break;
			}
			goto found_recv;

		case SELMETH_RECV_TYPESET:
			if (msg_type >= 30) {
				if ((si->nxrq.msg_type & 0x40000000) == 0) {
					break;
				}
			} else {
				if ((si->nxrq.msg_type & (1 << msg_type)) == 0) {
					break;
				}
			}
			goto do_src;

		case SELMETH_RECV_TYPESEL:
			sel = si->item;
			si = mcmsg_select_recvtype_detach(sel, msg_type,
				originating_node, (source_ptype & ~GLOBAL_BIT));
			if (si == 0) {
				return 0;
			}
			goto found_recv_detached;

		case SELMETH_RECV_TYPE:
			if (si->nxrq.msg_type != msg_type) {
				break;
			}
			goto found_recv;

		case SELMETH_RECV_ANY:
			if (msg_type >= RESERVED_BASE_TYPE) {
				break;
			}
		found_recv:
			if (si == sl) {
				mt->selection_path = 0;
			} else {
				sl->link = si->link;
				if (mt->selection_path == si) {
					mt->selection_path = sl;
				}
			}
		found_recv_detached:
			np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
			if (np == 0) {
				mcmsg_trace_drop("recv nx no req",
						 si->nxrq.request);
				mcmsg_free_select_item(si);
				return 0;
			}

			/*
			 * Store info in nxreq.
			 * XXX if we check for NULL np->info we can speed up
			 *     crecvx with info=0, but we impact normal crecv. 
			 */
			np->localinfo[0] = msg_type;
			np->localinfo[1] = msg_length;
			np->localinfo[2] = originating_node;
			np->localinfo[3] = source_ptype;
			np->localinfo[4] = source_node;

			mcmsg_trace_debug("find recv", 3, si, np, 0, 0);

			/*
			 * Mark NXREQ complete here for zero byte
			 * messages since we have validated np in hand.
			 */
			if (msg_length == 0) {
				np->state = NX_COMPLETE;

TIMESTAMP(3) /* END OF RECV 0 */

				if (np->handler != 0) {
					mcmsg_trace_debug("hrecv ast nx1", 2,
						np, np->hparam, 0, 0);
					mcmsg_hreq_ast(mt->task, si->nxrq.request);
				}
				return si;
			}
				
			if (msg_length > si->nxrq.count) {
				mcmsg_trace_drop("recv nx too big", msg_length);
				np->err = NX_ERR_LEN;
				np->state = NX_COMPLETE;

				/* check for hrecv handler */
				if (np->handler != 0) {
						mcmsg_trace_debug("hrecv ast find", 1, 
								   np->hparam, 0, 0, 0);
						mcmsg_hreq_ast(mt->task, si->nxrq.request);
				}

				mcmsg_free_select_item(si);
				return 0;
			}
			si->nxrq.count = msg_length;
			return si;

		default:
			assert(0);
		}
		if (si == st)
			break;
		sl = si;
		si = si->link;
		assert(t-- != 0);
	}
	return 0;
}

/*
 *	Routine:
 *		mcmsg_recv_nx1
 *		Receive NX1 packet method.
 *
 *	Purpose:
 *		Receive first packet of NX message
 *
 *	Return:
 *		None.
 */
mcmsg_recv_nx1(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register long 		source_ptype;
	register unsigned long	dest_pid;
	register long 		dest_ptype;
	register long 		msg_type;
	register unsigned long	msg_length;
	register unsigned long	take;
	register unsigned long	avail;
	register int		pkt;
	register unsigned long	offset;
	register user_pointer_t	xmsg;
	register xmsg_t		*xp;
	register unsigned long	buf;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register mcmsg_task_t	*mt;
	register unsigned long	msg_size;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register nxreq_t	*np;
	register unsigned long	seq;
	register int		give;
	register int		t;
	register user_pointer_t	xxmsg;
	register xmsg_t         *xxp;
	register unsigned long  need;
	register unsigned long  buf_size;
	register unsigned long	originating_node;
	register unsigned long	dummy;

TIMESTAMP(2) /* START OF RECV */

	/*
	 * Receive the remainder of the NX1 header.
	 */
	recv_hdr10(source_pid,   take,
	           source_ptype, dest_pid,
	           dest_ptype,   msg_type,
	           msg_length,   originating_node);

	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, msg_length, msg_type);

	pkt = (hdr2 & 0xFFFF);
	seq = (hdr2 >> 16);
	give = ((hdr1 >> 11) & ~(0x1f));

	/*
	 * Lookup the destination process.
	 */
	if ((si = mcmsg_selector_lookup_si(&mcmsg_local_sel, dest_pid)) == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush_pkt(pkt);
		return;
	}
	mt = si->mcmsg_task;
	assert(mt != 0);

	/*
	 * Lookup the sending process.
	 */
	pid_si = mcmsg_lookup_remote(mt, source_pid);
	assert(pid_si != 0);
	/* mark when_recvd */
	pid_si->ppid.when_recvd += ++mcmsg_hw.globtime;

	/*
	 * Look for a matching receive.
	 */
	si = mcmsg_find_nx_recv(mt,
				msg_type,
				dest_ptype,
				msg_length,
				pid_si->ppid.node,
				source_ptype,
				originating_node);
	if (si != 0) {
		mcmsg_trace_debug("NX1 match", 4, si, msg_type, msg_length, take); 
	}

	/*
	 * Check for zero length msg with a matching recv posted.
	 */
	if (si && msg_length == 0) {

		mcmsg_trace_debug("nx1 complete", 2, si, np, 0, 0);
		/*
		 * Update send_avail and release any blocked sends.
		 */
		if (give > 0 ) {
			pid_si->ppid.send_avail += give;
			assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);
			if (pid_si->ppid.send_wait != 0) {
				mcmsg_release_send_wait(mt, pid_si);
			}
		}
		mcmsg_relinquish(mt, pid_si, take);
		mcmsg_free_select_item(si);

		/*
		 * Done with zero length msg.
		 */
		return;
	}

	if (msg_length <= pkt) {
		pkt = msg_length;
	}

#if BIGPKTS
	/*
	 * Note X: Need to move mcmsg_release_send_wait() to the end, 
	 * to avoid nested receives.
	 */
	if (give > 0 ) {
		pid_si->ppid.send_avail += give;
		assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);
	}
#else BIGPKTS
	/*
	 * Update send_avail and release any blocked sends.
	 */
	if (give > 0 ) {
		pid_si->ppid.send_avail += give;
		assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);
		if (pid_si->ppid.send_wait != 0) {
			mcmsg_release_send_wait(mt, pid_si);
		}
	}
#endif BIGPKTS

	/*
	 * Message length is non-zero.
	 * Validate begin and end packet data pointers.
	 */
	if (si != 0) {
		buf = si->nxrq.buf;
		/* assert(buf != 0); Let the user code crash when it tries
		   To page in this non-resident page. */ 
		/*
		 * Since the recv request already exists, the
		 * receive method must not be XMSG type, so the
		 * buffer must be a contiguous user-supplied buffer.
		 */
		assert(si->method != SELMETH_RECV_XMSG);
		bp1 = mcmsg_validate_write1(buf, pkt, mt->dirbase);
		bp2 = mcmsg_validate2();
	}

	if (si && (bp1 && bp2)) {
		/*
		 *	Receive is posted AND the buffer is valid so receive
		 *	the data directly into users buffer. (Critical path) 
		 */
		mcmsg_recv_buf(bp1, bp2, pkt);

		if (msg_length <= pkt) {
			/*
			 * Posted Receive Complete.
			 * Set state of NX request as complete and
			 * free the select item.
			 */
			np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
			if (np != 0) {
				np->state = NX_COMPLETE;
				mcmsg_trace_debug("nx1 complete", 2, si, np, 0, 0);
				/*
				 * Check for hrecv handler
				 */
				if (np->handler != 0) {
					mcmsg_trace_debug("hrecv ast nx1", 2, 
							   np, np->hparam, 0, 0);
					mcmsg_hreq_ast(mt->task, si->nxrq.request);
				}
			} else {
				/*
				 * Invalid NX request pointer.
				 */
				mcmsg_trace_drop("Invalid NX req", si->nxrq.request);
				mcmsg_msg_drop++;
			}
			mcmsg_relinquish(mt, pid_si, take);
			mcmsg_free_select_item(si);

		} else {
			/*
			 * Posted Receive not complete.
			 * Install packet in sequence selector.
			 */
			if (mt->applinfo.process_lock) {
				/*
				 * Process lock allows us to take the whole
				 * message at once.
				 */
				mcmsg_relinquish(mt, pid_si, take);
				si->nxrq.take = 0;
				si->nxrq.stop = msg_length;
			} else {
				/*
				 * Take the message in chunks.
				 */
				si->nxrq.take = take;
				si->nxrq.stop = take - sizeof(xmsg_t);
			}
			si->nxrq.pid_si = pid_si;
			si->nxrq.offset = 0;
			si->method = SELMETH_RECV_ANY;
			si->nxrq.sequence = seq;
			assert(si->mcmsg_task == mt);
			mcmsg_install_sequence(mt, source_pid, si);

			/*
			 * Provoke the sender to send more packets.
			 */
			if (mt->applinfo.process_lock &&
			    msg_length > take - sizeof(xmsg_t)) {
				si->nxrq.stop = msg_length;
				mcmsg_send(mt, MCTRL_NXR,
					   si,
					   si->nxrq.sequence);
			} else if (pkt == si->nxrq.stop) {
				si->nxrq.stop += take - sizeof(xmsg_t);
				mcmsg_send(mt, MCTRL_NXC,
					   si,
					   si->nxrq.sequence);
			}
		}
	} else {

		/* 
		 *	No receive is posted OR buffer not resident.
		 *  Need to buffer the data in an xmsg.
		 */

		register unsigned long buffer_take;
		register unsigned long buffer_whole;	/* boolean */

		/*
	 	* Flush and Cancel FORCE messages with no receive.
	 	*/
		if ((si == 0) && (msg_type & FORCE_FLAG)) {
			mcmsg_fifo_flush_pkt(pkt);
			mcmsg_relinquish(mt, pid_si, take);
			if (msg_length > take - sizeof(xmsg_t)) {
				mcmsg_send(mt, MCTRL_NXF, pid_si, seq);
			}
			return;
		}

		/*
		 * Deal with buffer appropriation.
		 */
                if (mcmsg_appropriate(mt, pid_si, take)) {
                        mcmsg_relinquish(mt, pid_si, take);
                }

		/*
		 *	Set pointer to NX request structure.
		 */
		if (si != 0) {
			if (si->nxrq.request != 0) {
				np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
			} else {
				mcmsg_trace_drop("NX request pointer", si->nxrq.request);
				mcmsg_msg_drop++;
			}
		} else {
			np = (nxreq_t *)0;
		}

		msg_size = ((msg_length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));
		assert(mt->provided >= mt->assigned);
		avail = mt->provided - mt->assigned;


		/*
		 * Build a message select item if needed.
		 */
		if (si == 0 && (msg_length > pkt)) {
			si = mcmsg_alloc_select_item();
			assert(si != 0);
			si->method        = SELMETH_RECV_XMSG;
			si->value         = source_pid;
			si->nxrq.offset   = 0;
			si->nxrq.count    = msg_length;
			si->nxrq.request  = 0;
			si->nxrq.sequence = seq;
			si->nxrq.pid_si   = pid_si;
			si->nxrq.msg_type = msg_type;
			/* XXX Is the following line necessary ??? */
			si->nxrq.originating_node = originating_node;
		}

		/*
		 * If available xmsg buffer is smaller than msg length,
		 * only ask for 'take' and receive message in chunks.
		 */
		if (avail >= msg_size) {
			buffer_take = msg_length;
			buffer_whole = 1;
		} else {
			buffer_take = take - sizeof(xmsg_t);
			buffer_whole = 0;
		}
		/*
		 * Get an Xmsg.
		 */
		xmsg = mcmsg_alloc_whole_xmsg(mt, buffer_take, avail);
		if (xmsg != 0) {
	
			/* init xmsg info */
			xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
			xp->msg_type = msg_type;
			xp->length = msg_length;
			xp->source_node = pid_si->ppid.node;
			xp->originating_node = originating_node;
			xp->source_ptype = source_ptype;
			xp->dest_ptype = dest_ptype;
	
			buf = ((unsigned long)xmsg) + sizeof(xmsg_t);
			mcmsg_trace_debug("nx1 alloc xmsg", 4,
					  si, xmsg, msg_length, buffer_take);
	
			if (msg_length <= pkt) {
				/*
				 * message complete
				 */
				if (np != 0) {
					mcmsg_update_xmsg_rdy(mt, xmsg);
				}
				xp->state = XMSG_FULL;
				xp->si = 0;
			} else {
				/*
				 * message not complete
				 * install select item in sequence selector
				 */
				assert(si != 0);
				si->method        = SELMETH_RECV_XMSG;
				si->mcmsg_task    = mt;
				si->nxrq.sequence = seq;
				si->nxrq.pid_si   = pid_si;
				si->nxrq.xmsg     = xmsg;
				/* XXX Is the following line necessary ??? */
				si->nxrq.originating_node = originating_node;
				si->nxrq.take     = take;
				si->nxrq.stop     = buffer_take;
				xp->si = si;
				mcmsg_install_sequence(mt, source_pid, si);

				/*
				 * Provoke the sender to send more packets.
				 */
				if (buffer_whole) {
				    if (msg_length > take - sizeof(xmsg_t)) {
						mcmsg_send(mt, MCTRL_NXR,
							   si,
							   si->nxrq.sequence);
					}
				} else {
					if (pkt == buffer_take) {
						si->nxrq.stop += buffer_take;
						xp->state = XMSG_FULL;
					}
				}
			}
			assert(mt->provided >= mt->assigned);
	
		} else {
			/*
			 * No room. Very bad.
			 */
#if 1
			assert(0);
#endif
			mcmsg_trace_drop("space not found", msg_length);
			mcmsg_msg_drop++;
			mcmsg_fifo_flush_pkt(pkt);
			if (si != 0) {
				mcmsg_free_select_item(si);
			}
			if (np != 0) {
				np->err = NX_ERR_NOMEM;
				np->state = NX_COMPLETE;
			}
			return;
		}

		/*
		 * Receive the data into the (potentially split)  Xmsg
		 */
		xxmsg = xmsg;
		xxp = xp;
		if (xxp->size == 0) {
			xxmsg = (user_pointer_t)xxp->link;
			buf = ((unsigned long)xxmsg) + sizeof(xmsg_t);
			xxp = (xmsg_t *)mcmsg_validate_line(mt, xxmsg);
			if (xxp == 0) {
				mcmsg_trace_drop("nx1 xbuf invalid 1",
						  buf);
				mcmsg_msg_drop++;
				mcmsg_fifo_flush_pkt(pkt);
				return;
			}
		}
		need = pkt;
		assert((t = MAXLOOP) != 0);
		while (need > 0) {
			if (need > xxp->size) {
				buf_size = xxp->size;
			} else {
				buf_size = need;
			}
			bp1 = mcmsg_validate_write1(buf, buf_size, mt->dirbase);
			bp2 = mcmsg_validate2();
			if (bp1 && bp2) {
				mcmsg_recv_buf(bp1, bp2, buf_size);
				need -= buf_size;
				if (need > 0) {
					xxmsg = (user_pointer_t)xxp->link;
					buf = ((unsigned long)xxmsg) +
						sizeof(xmsg_t);
					xxp = (xmsg_t *)mcmsg_validate_line(mt, xxmsg);
					if (xxp == 0) {
						mcmsg_trace_drop("nx1 xbuf invalid 2",
						                  buf);
						mcmsg_msg_drop++;
						mcmsg_fifo_flush(need);
						break;
					}
				}
			} else {
				mcmsg_trace_drop("nx1 xbuf invalid 3", buf);
				mcmsg_msg_drop++;
				mcmsg_fifo_flush(need);
				break;
			}
			assert(t-- != 0);
		}

		if (np != 0) {
			/*
		 	* Current state of NX request is active
		 	*/
			mcmsg_trace_debug("nx1 buffered", 3, si, np, xmsg, 0);
			np->xmsg = (xmsg_t *)xmsg;

			if (msg_length <= pkt) {
				/* Message complete */
				np->state = NX_BUFFERED;
				if (np->handler != 0) {
					/*
					 * Invoke hrecv handler
					 */
					mcmsg_trace_debug("hrecv ast nx1 b",
							   2, np, np->hparam, 0, 0);
					mcmsg_hreq_ast(mt->task, si->nxrq.request);
				}
				mcmsg_free_select_item(si);
			} else if (xp->state == XMSG_FULL) {
				/* Chunk complete */
				if (np->monitored == 0) {
					/* Copy buffer and provoke sender to send more packets. */
					mcmsg_trace_debug("cont ast nx1",
						2, np, np->xmsg, 0, 0);
					mcmsg_recv_cont_ast(mt->task, si->nxrq.request);
				} else {
					/* monitored by thread, thread transfers buf */
					np->state = NX_BUFFERED;
				}
			}
		}
	}

#if BIGPKTS
	/*
	 * This was moved from up above to ensure no nested
	 * receive here. (See Note X above)
	 */
	if (pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(mt, pid_si);
	}
#endif BIGPKTS

	/*
	 * Done with NX1 packet.
	 */
	return;
}

/*
 *	Routine:
 *		mcmsg_recv_nxn
 *
 *	Purpose:
 *		Receive subsequent packet of NX message
 *
 *	Return:
 *		None.
 */

#if	HANDCODE && !BIGPKTS
/* See msgp_nx.s */
#else	HANDCODE && !BIGPKTS
mcmsg_recv_nxn(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	offset;
	register int		pkt;
	register int		sequence;
	register int		give;
	register select_item_t	*si;
	register select_item_t	*pid_si;
	register mcmsg_task_t	*mt;

	/*
	 * Receive remainder of NXN header.
	 */
	recv_hdr4(source_pid, offset);

	assert((offset & (NX_PKT_GRAN-1)) == 0);

	pkt = (hdr2 & 0xFFFF);
	sequence = (hdr2 >> 16);

	/*
	 * Lookup sequence.
	 */

	si = mcmsg_lookup_sequence(source_pid, sequence);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, offset, si);
	if (si == 0) {
		mcmsg_trace_drop("seq not found", 0);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush_pkt(pkt);
		return;
	}
	give = ((hdr1 >> 11) & ~(0x1f));

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);

	/*
	 * Update send avail.
	 */
	pid_si->ppid.send_avail += give;
	assert(si->nxrq.count - offset >= pkt);
	mt = pid_si->mcmsg_task;
	assert(mt != 0);
	assert(pid_si->ppid.send_avail <= mt->applinfo.memory_each);

	if (si->method != SELMETH_RECV_XMSG) {
		mcmsg_recv_nxn_rq(si, mt, offset, pkt, give, pid_si, source_pid);
	} else {
		mcmsg_recv_nxn_xm(si, mt, offset, pkt, give, pid_si, source_pid);
	}
}
#endif	HANDCODE && !BIGPKTS

/*
 *	Routine:
 *		mcmsg_recv_nxn_rq
 *
 *	Purpose:
 *		Receive subsequent packet of NX message
 *
 *	Return:
 *		None.
 */

mcmsg_recv_nxn_rq(si, mt, offset, pkt, give, pid_si, source_pid)
	register select_item_t	*si;
	register mcmsg_task_t	*mt;
	register unsigned long	offset;
	register int		pkt;
	register int		give;
	register select_item_t	*pid_si;
	register unsigned long	source_pid;
{
	register unsigned long	xoff;
	register user_pointer_t	xmsg;
	register xmsg_t		*xp;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register nxreq_t	*np;
	register unsigned long	*ip;
	int                     t;

	/*
	 * Receive data into user buffer.
	 */

	if (pkt > 0) {

		bp1 = mcmsg_validate_write1(((unsigned long)(si->nxrq.buf)) + offset,
				      pkt,
				      mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0 || bp2 == 0) {
			goto transition;
		}

		/*
		 * Receive data into user buffer.
		 */
		mcmsg_recv_buf_even(bp1, bp2, pkt);
	}

	if (si->nxrq.count <= offset + pkt) {

		/* Message Complete */

		nxreq_t	*np;
		np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
		if (np == 0) {
			mcmsg_trace_drop("nxreq invalid",
					 si->nxrq.request);
			mcmsg_msg_drop++;
			mcmsg_fifo_flush_pkt(pkt);
			goto norecv;
		}

		mcmsg_relinquish(mt, pid_si, si->nxrq.take);
		si->nxrq.take = 0;
		np->state = NX_COMPLETE;
		mcmsg_trace_debug("nxn complete", 2, si, np, 0, 0);

		/* check for hrecv handler */
		if (np->handler != 0) {
			mcmsg_trace_debug("hrecv ast nxn", 1, 
					   np->hparam, 0, 0, 0);
			mcmsg_hreq_ast(mt->task, si->nxrq.request);
		}
		mcmsg_remove_sequence(mt, source_pid, si);

	} else if (si->nxrq.stop == offset + pkt) {

		/* Message Not complete */

		if (!mt->applinfo.process_lock) {
			si->nxrq.stop += si->nxrq.take - sizeof(xmsg_t);
		}
		mcmsg_send(mt, MCTRL_NXC, si, si->nxrq.sequence);
	}

norecv:
	/*
	 * Release (continue) any blocked sends.
	 */

	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(mt, pid_si);
	}
	return;

transition:
	mcmsg_recv_nxn_transition(si, mt, offset, pkt, give,
				  pid_si, source_pid);
	return;
}

/*
 *	Routine:
 *		mcmsg_recv_nxn_transition
 *
 *	Purpose:
 *		Transition from request buffer to xmsg buffer
 *		because request buffer wasn't there
 *
 *	Return:
 *		None.
 */

mcmsg_recv_nxn_transition(si, mt, offset, pkt, give, pid_si, source_pid)
	register select_item_t	*si;
	register mcmsg_task_t	*mt;
	register unsigned long	offset;
	register int		pkt;
	register int		give;
	register select_item_t	*pid_si;
	register unsigned long	source_pid;
{
	register user_pointer_t	xmsg;
	register xmsg_t		*xp;
	register nxreq_t	*np;
	int                     t;
	register unsigned long take;
	/*
	 * Buffer not Present:
	 * At this point, We have been copying data directly 
	 * into the user buffer, now we must start
	 * buffering in Xmsg Buffers.
	 */
	take = si->nxrq.take;

	assert(!mt->applinfo.process_lock);
	assert(take > 0);

	/*
	 * Allocate and Validate xmsg
	 */
        if (mcmsg_appropriate(mt, pid_si, take)) {
                mcmsg_relinquish(mt, pid_si, take);
        }
	assert(mt->provided >= mt->assigned);

	xmsg = mcmsg_alloc_whole_xmsg(mt, take - sizeof(xmsg_t),
		mt->provided - mt->assigned);

	_mcmsg_nxn_to_xmsg++;	/* debug */
	mcmsg_trace_debug("nxn to xmsg", 3, si, xmsg, take, 0);

	xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
	if (xp == 0) {
		mcmsg_trace_drop("no valid xmsg for take",
				si->nxrq.take);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush_pkt(pkt);
		goto norecv;
	}

	/*
	 * Change the select_item to indicate that
	 * we are now receiving into an xmsg buffer.
	 */
	si->nxrq.xmsg = xmsg;
	si->method = SELMETH_RECV_XMSG;

	xp->length      = si->nxrq.count - offset;
	xp->xmsg_offset = offset;
	xp->xmsg_data   = 0;
	xp->si          = si;
	xp->state       = XMSG_CONT;

	/*
	 * Set the state of the request.
	 */
	np = (nxreq_t *)
	     mcmsg_validate_long(mt, si->nxrq.request);
	if (np != 0) {
		np->xmsg  = (xmsg_t *)xmsg;
	} else {
		mcmsg_trace_drop("nxreq invalid",
				 si->nxrq.request);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush_pkt(pkt);
		goto norecv;
	}
	mcmsg_recv_nxn_xm(si, mt, offset, pkt, give,
			  pid_si, source_pid);
	return;

norecv:
	/*
	 * Release (continue) any blocked sends.
	 */

	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(mt, pid_si);
	}
}

/*
 *	Routine:
 *		mcmsg_recv_nxn_xm
 *
 *	Purpose:
 *		Receive subsequent packet of NX message
 *
 *	Return:
 *		None.
 */

mcmsg_recv_nxn_xm(si, mt, offset, pkt, give, pid_si, source_pid)
	register select_item_t	*si;
	register mcmsg_task_t	*mt;
	register unsigned long	offset;
	register int		pkt;
	register int		give;
	register select_item_t	*pid_si;
	register unsigned long	source_pid;
{
	register unsigned long	xoff;
	register user_pointer_t	xmsg;
	register xmsg_t		*xp;
	register unsigned long	buf;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register nxreq_t	*np;
	register unsigned long	*ip;
	int                     t;

	/*
	 * Receive data into xmsg buffer.
	 */

	xmsg = si->nxrq.xmsg;
	xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
	if (xp == 0) {
		mcmsg_trace_drop("xmsg not valid", xmsg);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush_pkt(pkt);
		goto norecv;
	}

	/*
	 * This label assumes that xmsg and xp are valid.
	 */
	if (pkt > 0) {
		register unsigned long  need;
		register user_pointer_t	xxmsg = xmsg;
		register xmsg_t         *xxp = xp;
		register unsigned long  buf_size;

		if (xxp->state == XMSG_CONT) {
			xoff = xxp->xmsg_data;
		} else {
			xoff = offset;
		}

		/*
		 * Find the correct element and offset within
		 * the element of this chain of xmsg buffers.
		 * xxmsg and xxp point to the correct element in the chain.
		 * xoff is byte offset into that chain element.
		 */
		assert((t = MAXLOOP) != 0);
		while (xoff >= xxp->size) {
			assert(xxp->chain_number != 0);
			xoff -= xxp->size;
			xxmsg = (user_pointer_t)xxp->link;
			xxp = (xmsg_t *)mcmsg_validate_line(mt, xxmsg);
			if (xxp == 0) {
				mcmsg_trace_drop("xmsg buf not valid",
						  xxmsg);
				mcmsg_msg_drop++;
				mcmsg_fifo_flush_pkt(pkt);
				goto norecv;
			}
			assert(t-- != 0);
		}

		/*
		 * Receive the data into the (potentially chained) xmsg.
		 * It is possible that one packet may span more than
		 * one element in a chain.
		 */
		need = pkt;
		assert((t = MAXLOOP) != 0);
		while (need > 0) {
			if (need > xxp->size - xoff)
				buf_size = xxp->size - xoff;
			else
				buf_size = need;
			buf = ((unsigned long)xxmsg) + sizeof(xmsg_t) + xoff;
			bp1 = mcmsg_validate_write1(buf, buf_size, mt->dirbase);
			bp2 = mcmsg_validate2();
			if (bp1 && bp2) {
				mcmsg_recv_buf(bp1, bp2, buf_size);
				need -= buf_size;
				if (need > 0) {
					xxmsg = (user_pointer_t)xxp->link;
					xxp = (xmsg_t *)
						   mcmsg_validate_line(mt, xxmsg);
					if (xxp == 0) {
						mcmsg_trace_drop(
						     "nxn xbuf invalid",
								  buf);
						mcmsg_msg_drop++;
						mcmsg_fifo_flush(need);
						goto norecv;
					}
					xoff = 0;
				}
			}
			else {
				mcmsg_trace_drop("xmsg buf not valid",
						  buf);
				mcmsg_msg_drop++;
				mcmsg_fifo_flush(need);
				goto norecv;
			}
			assert(t-- != 0);
		}  /* while (need > 0) */
	}  /* if (pkt > 0) */

	if (xp->state == XMSG_CONT) {
		if (si->nxrq.count <= offset + pkt) {
			/*
			 * Message Complete
			 */
			mcmsg_trace_debug("nxn xmsg last stop", 
					  4, si, xmsg, pkt, xp->xmsg_offset);
			xp->length = xp->xmsg_data + pkt;
			xp->xmsg_stop = xp->xmsg_data + pkt;
			xp->state = XMSG_STOP;
			if (si->nxrq.request != 0 &&
			    (np = (nxreq_t *)
				  mcmsg_validate_line(mt, si->nxrq.request))
					!= 0) {
				mcmsg_update_xmsg_rdy(mt, xmsg);
			}
		} else if (si->nxrq.stop == offset + pkt) {
			/*
			 * Chunk Complete
			 */
			mcmsg_trace_debug("nxn xmsg stop", 
					  4, si, xmsg, pkt, xp->xmsg_offset);

			si->nxrq.stop += si->nxrq.take - sizeof(xmsg_t);
			xp->xmsg_stop = xp->xmsg_data + pkt;
			xp->state = XMSG_STOP;
			if (si->nxrq.request != 0 &&
			    (np = (nxreq_t *)
				  mcmsg_validate_line(mt, si->nxrq.request))
					!= 0) {
				if (np->monitored == 0) {
					np->state = NX_ACTIVE;
					mcmsg_trace_debug("cont ast nxn a",
							2, np, np->xmsg, 0, 0);
					mcmsg_recv_cont_ast(mt->task, si->nxrq.request);
				} else {
					np->state = NX_BUFFERED;
				}
			}
			goto norecv;  /* Don't drop into check for hrecv handler */

		} else {
			/*
			 * Continue Chunk
			 */
			mcmsg_trace_debug("nxn xmsg cont", 
					  4, si, xmsg, pkt, xp->xmsg_data);
			xp->xmsg_data += pkt;
			goto norecv;  /* Don't drop into check for hrecv handler */
		}
	} else {
		/*
		 * XMSG BUSY
		 */
		if (si->nxrq.count <= offset + pkt) {
			/*
			 * Message Complete
			 */
			mcmsg_trace_debug("nxn xmsg last full", 4, si,xmsg,pkt,offset);
			xp->state = XMSG_FULL;
			if (si->nxrq.request != 0 &&
			    (np = (nxreq_t *)
				  mcmsg_validate_line(mt, si->nxrq.request))
					!= 0) {
				mcmsg_update_xmsg_rdy(mt, xmsg);
			}
		} else {
			/*
			 * Message not complete.
			 */
			if (xp->totalsize == offset + pkt) {
				/*
				 * Done with XMSG chunk
				 */
				mcmsg_trace_debug("nxn xmsg full", 4,
						 si, xmsg, pkt, offset);
				si->nxrq.stop += si->nxrq.take
					       - sizeof(xmsg_t);
				xp->state = XMSG_FULL;
				if (si->nxrq.request != 0 &&
				    (np = (nxreq_t *)
				  mcmsg_validate_line(mt, si->nxrq.request))
						!= 0) {
					if (np->monitored == 0) {
						np->state = NX_ACTIVE;
						mcmsg_trace_debug("cont ast nxn b",
								2, np, np->xmsg, 0, 0);
						mcmsg_recv_cont_ast(mt->task, si->nxrq.request);
					} else {
						np->state = NX_BUFFERED;
					}
				}
			}
			goto norecv;	/* Don't drop into check for hrecv handler */
		}
	}

	/* Message complete: check for hrecv handler. */
	if (si->nxrq.request != 0 &&
		(np = (nxreq_t *) mcmsg_validate_line(mt, si->nxrq.request)) != 0) {
		/* main or hrecv thread to copy last buffer */
		np->state = NX_BUFFERED;
		if (np->handler != 0) {
			/* Invoke hrecv handler */
			mcmsg_trace_debug("hrecv ast nxn xmsg", 1,
					np->hparam, 0, 0, 0);
			mcmsg_hreq_ast(mt->task, si->nxrq.request);
		}
	}

	/* Indicate select item no longer valid for xmsg */
	xp->si = 0;
	mcmsg_remove_sequence(mt, source_pid, si);

norecv:
	/*
	 * Release (continue) any blocked sends.
	 */

	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(mt, pid_si);
	}
}

/*
 *	Routine:
 *		mcmsg_update_xmsg_rdy
 *
 *	Purpose:
 *		Take an xmsg that is ready to be FREE'ed out of
 *		the xmsg_rdy list.
 *
 *	Args:
 *		mt	current task
 *		xp	the xmsg to extract from the xmsg_rdy list.
 *
 *	Returns:
 *		0	xp not found in ready
 *		1	xp removed from ready
 *		-1	error
 */
mcmsg_update_xmsg_rdy(mt, xmsg)
	mcmsg_task_t	*mt;
	user_pointer_t	xmsg;
{
	user_pointer_t	prev,   next;
	xmsg_t		*xp, *xxp, *xnext, *xprev;
	user_pointer_t	user_xxp;
	int		t;

	xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
	if (xp == 0) {
		return -1;
	}

	xxp = xp;
	user_xxp = xmsg;

	assert((t = MAXLOOP) != 0);
	while (xxp->chain_number != 0) {
		user_xxp = (user_pointer_t)xxp->link;
		xxp = (xmsg_t *)mcmsg_validate_line(mt, user_xxp);
		if (xxp == 0) {
			return -1;
		}
		assert(t-- != 0);
	}

	prev = (user_pointer_t)xp->backlink;
	next = (user_pointer_t)xxp->link;

	if (prev != 0) {
		xprev = (xmsg_t *)mcmsg_validate_line(mt, prev);
		if (xprev == 0) {
			return -1;
		}
		xprev->link = (xmsg_t *)next;
	}

	if (next != 0) {
		xnext = (xmsg_t *)mcmsg_validate_line(mt, next);
		if (xnext == 0) {
			return -1;
		}
		xnext->backlink = (xmsg_t *)prev;
	}

	if (mt->xmsg_rdy_end == user_xxp) {
		mt->xmsg_rdy_end = (user_pointer_t)xp->backlink;
	}
	
	xxp->link = 0;
	xp->backlink = 0;

	if (mt->xmsg_rdy == xmsg) {
		assert(xnext->backlink == 0);
		mt->xmsg_rdy = next;
	}

	return 1;	
}

/*
 *	Routine:
 *		mcmsg_send_nx1()
 *		Send NX1 packet method.
 *
 *	Purpose:
 *		Send first packet of NX message
 *
 *	Parameters:
 *		mt		current mcmsg task structure
 *		ctl		always MCTRL_NX1
 *		si		send select item
 *		avail	current uncommitted buffer
 *
 *	Returns:
 *		None.
 */
mcmsg_send_nx1(mt, ctl, si)
	register mcmsg_task_t	*mt;
	unsigned long			ctl;
	register select_item_t	*si;
{
	register unsigned long	length;
	register unsigned long	pkt;
	register select_item_t	*pid_si;
	register unsigned long	bufp;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register nxreq_t		*np;
	unsigned long			give;

	length = si->nxrq.count;
	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);

	/*
	 * Validate send packet buffer.
	 */
	if (length > 0) {

		pkt = mt->applinfo.pkt_size;
		if (length < pkt) {
			pkt = (length + NX_PKT_GRAN-1) & ~(NX_PKT_GRAN-1);
		}

page_validate_retry:
		bp1 = mcmsg_validate_read1(si->nxrq.buf, pkt, mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0 || bp2 == 0) {
			/*
			 * Invalid or non-present buffer address.
			 */

			if ((pkt > length) &&
				(((si->nxrq.buf + pkt - 1) & MSG_PAGE_MASK) !=
				((si->nxrq.buf + length - 1) & MSG_PAGE_MASK)) ) {
				/*
				 * Buffer doesn't cross page!  pkt has sent us into a
				 * page past the end of the buffer.  Reset pkt size just
				 * to reach the end of page.
				 */
				pkt = ((si->nxrq.buf + pkt) & MSG_PAGE_MASK) - si->nxrq.buf;
				goto page_validate_retry;
			}

			if (mt->applinfo.process_lock) {
				mcmsg_trace_drop("nx1 send invalid buffer", si->nxrq.buf);
				mcmsg_msg_drop++;
			} else {
				mcmsg_post_vm_send(mt, MCTRL_NX1, si);
			}
			return;
		}
	} else {
		pkt = 0;
	}
				
	/*
	 * Prepare to send the packet.
	 */
	assert((pid_si->ppid.recv_give & sizeof(xmsg_t)-1) == 0);
	give = pid_si->ppid.recv_give;
	pid_si->ppid.recv_give = 0;

	/*
	 * Send the packet.
	 */
	mcmsg_trace_send(MCTRL_NX1 | (give << 11), 
			 pkt | (si->nxrq.sequence << 16), 
			 pid_si->value, 2, si->nxrq.take, si);
	if (length == 0) {
		mcmsg_send_hdr10_eod(
			pid_si->ppid.route,
			MCTRL_NX1 | (give << 11),
			(si->nxrq.sequence << 16),
			mt->pid,
			si->nxrq.take,
			si->nxrq.source_ptype,
			pid_si->value,
			si->nxrq.dest_ptype,
			si->nxrq.msg_type,
			0,
			si->nxrq.originating_node);
	} else {
#if    BIGPKTS
		nx_send_in_progress = si;
		mcmsg_local_send = (pid_si->ppid.route == 0);
#endif BIGPKTS
		mcmsg_send_hdr10(
			pid_si->ppid.route,
			MCTRL_NX1 | (give << 11),
			pkt | (si->nxrq.sequence << 16),
			mt->pid,
			si->nxrq.take,
			si->nxrq.source_ptype,
			pid_si->value,
			si->nxrq.dest_ptype,
			si->nxrq.msg_type,
			length,
			si->nxrq.originating_node);
		mcmsg_send_buf(bp1, bp2, pkt);
#if    BIGPKTS
		nx_send_in_progress = (select_item_t *)0;
		mcmsg_local_send = 0;
#endif BIGPKTS
	}

	if (length > pkt) {
		/*
		 * Send not complete.
		 */
		si->nxrq.offset = pkt;
		if (pkt == si->nxrq.stop) {
			/*
			 * Put send on send_wait queue if time
			 * to stop.
			 */
			register select_item_t *sh;
			register select_item_t *st;

		mcmsg_trace_debug("queue send_wait", 2, si, si->nxrq.sequence, 0, 0);

			st = pid_si->ppid.send_wait;
			pid_si->ppid.send_wait = si;
			if (st == 0) {
				si->link = si;
			} else {
				sh = st->link;
				assert(sh != 0 && sh->method != 0xdead);
				si->link = sh;
				st->link = si;
			}
			si->method = 0;
			si->nxrq.stop = si->nxrq.count;
			si->nextmethod = MCTRL_NXN;

			if (pid_si->ppid.send_avail > 0 && pid_si->ppid.send_wait != 0) {
				mcmsg_release_send_wait(mt, pid_si);
			}
			return;
		} else {
			/*
			 * Send next packet if not time to stop.
			 */
			mcmsg_send_tail(mt, MCTRL_NXN, si, pid_si,
					si->nxrq.buf + pkt,
					si->nxrq.count - pkt,
					pkt,
					pkt,
					si->nxrq.stop);
			return;
		}
	} else {

		/*
		 * Send complete
		 */
		np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
		if (np == 0) {
			mcmsg_trace_drop("nxreq nx1", si->nxrq.request);
			mcmsg_msg_drop++;
			return;
		}
		np->state = NX_COMPLETE;

TIMESTAMP(1)	/* END OF SEND */

		/*
		 * Check for Hsend
		 */
		if (np->handler != 0) {
			mcmsg_trace_debug("hsend ast nx1", 1,
					   np->hparam, 0, 0, 0);
			mcmsg_hreq_ast(mt->task, si->nxrq.request);
		}

		mcmsg_free_select_item(si);

		/*
		 * Check for messages waiting to send to this dest pid.
		 */
		if (pid_si->ppid.send_avail > 0 && pid_si->ppid.send_wait != 0) {
			mcmsg_release_send_wait(mt, pid_si);
		}
	}
	return;
}

/*
 *	Routine:
 *		mcmsg_send_nxn
 *		Send NXN packet method.
 *
 *	Purpose:
 *		Send subsequent packet of NX message
 *
 *	Parameters:
 *		mt		current mcmsg task structure
 *		ctl		always MCTRL_NXN
 *		si		send select item
 *
 *	Returns:
 *		None.
 */
#if    BIGPKTS
mcmsg_send_nxn(mt, ctl, si, pid_si, bufp, length, pkt, offset, stop)
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register select_item_t	*pid_si;
	register unsigned long	bufp;
	register unsigned long	length;
	register unsigned long	pkt;
	register unsigned long	offset;
	register unsigned long	stop;
{
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	hdr1, hdr2;

	if (length > pkt) {

		/*
		 * Validate send packet buffer.
		 */

		bp1 = mcmsg_validate_read1(bufp, pkt, mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 != 0) {

			/*
			 * Build header.
			 */

			assert((pid_si->ppid.recv_give & sizeof(xmsg_t)-1) == 0);
			mcmsg_trace_send(
				MCTRL_NXN | (pid_si->ppid.recv_give << 11),
				pkt | (si->nxrq.sequence << 16),
				pid_si->value, 2, offset, si);

			/*
			 * Send header and data.
			 */

			hdr1 = MCTRL_NXN | (pid_si->ppid.recv_give << 11);
			pid_si->ppid.recv_give = 0;
			hdr2 = pkt | (si->nxrq.sequence << 16);

			nx_send_in_progress = si;
			mcmsg_local_send = (pid_si->ppid.route == 0);
			mcmsg_send_hdr4(pid_si->ppid.route,
			                hdr1, hdr2,
			                mt->pid, offset);

			mcmsg_send_buf(bp1, bp2, pkt);
			nx_send_in_progress = (select_item_t *)0;
			mcmsg_local_send = 0;

			offset += pkt;
			/*
			 * Stop may have changed on the fly.
			 */
			stop = si->nxrq.stop;

			/*
			 * Check if time to stop.
			 */

			if (offset >= stop) {
				mcmsg_send_nxn_stop(mt, offset, si, pid_si);
				return;
			}

		} else {

			/*
			 * Invalid or non-present buffer address.
			 */

			if (mt->applinfo.process_lock) {
				mcmsg_trace_drop("nxn send invalid buffer",
						si->nxrq.buf);
				mcmsg_msg_drop++;
			} else {
				si->nxrq.offset = offset;
				mcmsg_post_vm_send(mt, MCTRL_NXN, si);
			}
			return;
		}

	} else {
		mcmsg_send_nxn_done(mt, ctl, si, pid_si, bufp, length,
				    length, offset, stop);
		return;
	}

	mcmsg_send_tail(mt, MCTRL_NXN,
	                si, pid_si,
	                bufp + pkt,
	                length - pkt,
	                pkt, offset, stop);
}

#else  BIGPKTS

mcmsg_send_nxn(mt, ctl, si, pid_si, bufp, length, pkt, offset, stop)
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register select_item_t	*pid_si;
	register unsigned long	bufp;
	register unsigned long	length;
	register unsigned long	pkt;
	register unsigned long	offset;
	register unsigned long	stop;
{
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	hdr1;
	nic_reg		t;

	if (length > pkt) {

		/*
		 * Validate send packet buffer.
		 */

		bp1 = mcmsg_validate_read1(bufp, pkt, mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 != 0) {

			/*
			 * Build header.
			 */

			assert((pid_si->ppid.recv_give & sizeof(xmsg_t)-1) == 0);
			hdr1 = MCTRL_NXN | (pid_si->ppid.recv_give << 11);
			pid_si->ppid.recv_give = 0;
send_next:
			mcmsg_trace_send(
				MCTRL_NXN | (pid_si->ppid.recv_give << 11),
				pkt | (si->nxrq.sequence << 16),
				mt->pid, 2, offset, si);

			/*
			 * Send header and data.
			 */

			mcmsg_send_pkt4(mt, 0,
					bp1, bp2, pkt,
					pid_si->ppid.route,
					hdr1,
					pkt | (si->nxrq.sequence << 16),
					mt->pid, offset);

			offset += pkt;

			/*
			 * Check if time to stop.
			 */

			if (offset >= stop) {
				mcmsg_send_nxn_stop(mt, offset, si, pid_si);
				return;
			}

			bufp += pkt;
			length -= pkt;
			if (length > pkt) {
				hdr1 = MCTRL_NXN;
				bp1 = mcmsg_validate_read1(bufp, pkt, mt->dirbase);
				bp2 = mcmsg_validate2();
				if (bp1 == 0) {
					goto no_buf;
				}
				for (;;) {

					t.full = NIC.status.full;

					/*
					 * Check for receive
					 */
					if ((t.halfs.lo & mcmsg_hw.recv_status) != 0) {
						mcmsg_save_send(mt, mcmsg_send_nxn,
								si, pid_si, bufp,
								length, pkt, offset, stop);
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

			mcmsg_send_tail(mt, MCTRL_NXN,
					si, pid_si,
					bufp, length,
					pkt, offset, stop);
		} else {
no_buf:
			/*
			 * Invalid or non-present buffer address.
			 */

			if (mt->applinfo.process_lock) {
				mcmsg_trace_drop("nxn send invalid buffer",
						si->nxrq.buf);
				mcmsg_msg_drop++;
			} else {
				si->nxrq.offset = offset;
				mcmsg_post_vm_send(mt, MCTRL_NXN, si);
			}
			return;
		}

	} else {
		mcmsg_send_nxn_done(mt, ctl, si, pid_si, bufp, length,
				    length, offset, stop);
		return;
	}
}
#endif BIGPKTS

mcmsg_send_nxn_done(mt, ctl, si, pid_si, bufp, length, pkt, offset, stop)
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register select_item_t	*pid_si;
	register unsigned long	bufp;
	register unsigned long	length;
	register unsigned long	pkt;
	register unsigned long	offset;
	register unsigned long	stop;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register nxreq_t	*np;


	/*
	 * Validate send packet buffer.
	 */

	if (length > 0) {
		bp1 = mcmsg_validate_read1(bufp, pkt, mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			/*
			 * Invalid or non-present buffer address.
			 */
			if (mt->applinfo.process_lock) {
				mcmsg_trace_drop("nxn send invalid buffer",
						si->nxrq.buf);
				mcmsg_msg_drop++;
			} else {
				si->nxrq.offset = offset;
				mcmsg_post_vm_send(mt, MCTRL_NXN, si);
			}
			return;
		}
	}

	/*
	 * Build header.
	 */

	assert((pid_si->ppid.recv_give & sizeof(xmsg_t)-1) == 0);
	hdr1 = MCTRL_NXN | (pid_si->ppid.recv_give << 11);
	pid_si->ppid.recv_give = 0;
	hdr2 = pkt | (si->nxrq.sequence << 16);
	hdr3 = mt->pid;
	mcmsg_trace_send(hdr1, hdr2, pid_si->value, 2, offset, si);

mcmsg_trace_debug("NXN stop si.stop", 2, stop, si->nxrq.stop, 0, 0); 

	/*
	 * Send header and data.
	 */

	if (length > 0) {

#if    BIGPKTS
		nx_send_in_progress = si;
		mcmsg_local_send = (pid_si->ppid.route == 0);
		mcmsg_send_hdr4(pid_si->ppid.route,
		                hdr1, hdr2,
		                hdr3, offset);

		mcmsg_send_buf(bp1, bp2, pkt);
		nx_send_in_progress = (select_item_t *)0;
		mcmsg_local_send = 0;
#else  BIGPKTS
		mcmsg_send_pkt4(mt, 0,
				bp1, bp2, pkt,
				pid_si->ppid.route,
				hdr1, hdr2, hdr3, offset);
#endif BIGPKTS
	} else {

		mcmsg_send_hdr4_eod(pid_si->ppid.route,
		                    hdr1, hdr2,
		                    hdr3, offset);
	}

	/*
	 * Send complete.
	 */

	np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
	if (np != 0) {
		np->state = NX_COMPLETE;

		/*
		 * Check for Hsend
		 */
		if (np->handler != 0) {
			mcmsg_trace_debug("hsend ast nxn", 1,
					   np->hparam, 0, 0, 0);
			mcmsg_hreq_ast(mt->task, si->nxrq.request);
		}
	}
	mcmsg_trace_debug("send nxn seq done", 1, si->nxrq.sequence, 0, 0, 0);
	mcmsg_free_select_item(si);

	/*
	 * Check for messages waiting to send to this pid.
	 */

	if (pid_si->ppid.send_avail > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(mt, pid_si);
	}
}


mcmsg_send_nxn_stop(mt, offset, si, pid_si)
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register select_item_t	*pid_si;
	register unsigned long	offset;
{
	register select_item_t *sh;
	register select_item_t *st;
	/*
	 * Queue on send_wait if time to stop.
	 */
	si->nxrq.offset = offset;
	mcmsg_trace_debug("  send wait", 2, si, pid_si, 0, 0);
	st = pid_si->ppid.send_wait;
	pid_si->ppid.send_wait = si;
	if (st == 0) {
		si->link = si;
	} else {
		sh = st->link;
		assert(sh != 0 && sh->method != 0xdead);
		si->link = sh;
		st->link = si;
	}
	si->method = 0;
	si->nextmethod = MCTRL_NXN;

	/*
	 * Check for messages waiting to send to this pid.
	 */

	if (pid_si->ppid.send_avail > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(mt, pid_si);
	}
}

/*
 *	mcmsg_relinquish
 *
 *	Relinquish buffer space to sender.
 *
 *	Add take to senders recv_give
 *	Calculate send_avail for a remote sender
 *	if the senders send_avail has crossed our give_threshold
 *		give sender buffer space via NXA
 *
 */
mcmsg_relinquish(mt, pid_si, take)
	register mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register unsigned long	take;
{
	register unsigned long	old_send_avail;
	register unsigned long	new_send_avail;
	register select_item_t	*sh;
	register select_item_t	*st;

	if (take == 0) {
		return;
	}

	old_send_avail = pid_si->ppid.recv_total -
			 pid_si->ppid.recv_give;
	if (pid_si->ppid.recv_target == 0) {
	    /* recv_target becomes 0 when NDT sent to pid_si */

	    /* sub_pid_si = 1 only when pid_si needs memory to send 1st msg */
	    assert(pid_si->ppid.sub_pid_si != (select_item_t *) 1);

	    /* return memory to replacement pid_si */
	    /* don't put in recv_give until ready to send NAA */
	    if (pid_si->ppid.sub_pid_si) {
		 pid_si->ppid.sub_pid_si->ppid.recv_total += take;
	    } else { 
		mt->assigned -= take;
		mcmsg_give_avail(mt);
	    }
	} else {
	    pid_si->ppid.recv_give += take;
	    new_send_avail = old_send_avail - take;
	    if (new_send_avail <= mt->applinfo.give_threshold &&
	        old_send_avail > mt->applinfo.give_threshold) {
	            mcmsg_send(mt, MCTRL_NXA, pid_si, mt);
	    }
	}
}

/*
 *	mcmsg_appropriate
 *
 *	appropriate buffer space for a receive
 *
 *	if avail (provided - assigned) has no room for take,
 *	then dip into the memory assigned to the sender,
 *	and put him on the 'avail_need' list.
 *
 *	return:
 *		1	Take is taken from avail
 *		0	Take is taken from recv_total of sender
 *			and sender is put on avail_need list.
 */
int
mcmsg_appropriate(mt, pid_si, take)
	register mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register unsigned long	take;
{
	register unsigned long	old_send_avail;
	register unsigned long	new_send_avail;
	register select_item_t	*sh;
	register select_item_t	*st;

	if (take == 0) {
		return 0;
	}
	if (mt->provided - mt->assigned > take) {
		return 1;
	} else {
		pid_si->ppid.recv_total -= take;
		assert((pid_si->ppid.recv_total >= 0) && 
       		    (pid_si->ppid.recv_total <= mt->applinfo.memory_each));
		mt->assigned -= take;
		if (pid_si->ppid.recv_target > pid_si->ppid.recv_total) {
		    /* don't add a pid that has been sent NDT to avail_need */
		    if (pid_si->ppid.avail_link == 0) {
			if (mt->avail_need == 0) {
				pid_si->ppid.avail_link = pid_si;
			} else {
				st = mt->avail_need;
				sh = st->ppid.avail_link;
				st->ppid.avail_link = pid_si;
				pid_si->ppid.avail_link = sh;
			}
			mt->avail_need = pid_si;
		    }
		    assert(pid_si->method == SELMETH_PID);
		}
	}
	return 0;
}

/*
 *
 *	Routine:
 *		mcmsg_post_vm_send
 *
 *	Purpose:
 *		Called when a packet to be sent is not resident in
 *		physical memory and plock is off. It posts an AST
 *		for the user process, which in turn will cause the
 *		send buffer to be paged in.
 *
 *		Sends are actually continued via mcmsg_send_continue().
 *		
 *	Parameters:
 *		method      method of send
 *		send_item	select_item that represents the current send.
 *		
 *	Returns:
 *		None.
 */

int mcmsg_send_misses = 0;	/* debug */

int
mcmsg_post_vm_send(mt, method, send_item)
	mcmsg_task_t	*mt;
	int		method;
	select_item_t *send_item;
{
	register task_t	task;
	unsigned long	buf_start, msg_left;
	nxreq_t      	*nxreq;

	task      = mt->task;
	buf_start = send_item->nxrq.buf + send_item->nxrq.offset;
	msg_left  = send_item->nxrq.count - send_item->nxrq.offset;

	/*
	 *  Update the nxreq structure to reflect the total
	 *  remainder of the buffer to be sent, not just the
	 *  current packet. This will be inspected by the user
	 *	ast handler thread to decide how many pages to make 
	 *	present.
	 */
	nxreq = (nxreq_t *)mcmsg_validate_line(mt, send_item->nxrq.request);
	if (nxreq == 0) {
		mcmsg_trace_drop("val nxreq", nxreq);
		return 0;
	}
	nxreq->buf     = buf_start;
	nxreq->bsize   = msg_left;
	nxreq->bcount  = 1;
	nxreq->boffset = 0;

	/*
	 * set the vm ast request
	 */
	mcmsg_trace_debug("set send vm ast", 3, task, nxreq, send_item, 0);
	mcmsg_vm_ast (task, send_item->nxrq.request, send_item);

	/*
	 * Update the item
	 */
	send_item->nextmethod = method;
	send_item->nxrq.vm_ast_pending = 1;

	/*
	 * Store send on send waiting
	 */
	{
		register select_item_t *pid_si;
		register select_item_t *waithead, *waittail;

		pid_si = send_item->nxrq.pid_si;
		assert(pid_si != 0);
		waittail = pid_si->ppid.send_wait;
		pid_si->ppid.send_wait = send_item;
		if (waittail == 0) {
			send_item->link = send_item;
		} else {
			waithead = waittail->link;
			send_item->link = waithead;
			waittail->link = send_item;
		}
	}
	mcmsg_send_misses++;	/* debug */
	return;
}

