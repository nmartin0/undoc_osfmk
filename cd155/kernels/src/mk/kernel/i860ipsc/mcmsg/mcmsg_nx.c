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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_nx.c,v 1.51 1994/11/18 20:41:43 mtm Exp $

/*
 * mcmsg_nx.c
 *
 * NX messages
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_nx.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>

#if 0
int desparate = 0;
#define desparation_printf(x) if (desparate) printf(x)
#undef mcmsg_trace_debug
static mcmsg_trace_debug(s,n,a,b,c,d) char *s; {
	if (desparate) switch (n) {
	case 0:
		printf("%s\n", s);
	case 1:
		printf("%s %08x\n", s, a);
	case 2:
		printf("%s %08x %08x\n", s, a, b);
	case 3:
		printf("%s %08x %08x %08x\n", s, a, b, c);
	default:
		printf("%s %08x %08x %08x %08x\n", s, a, b, c, d);
	}
}
#else
#define desparation_printf(x)
#endif

#define SAYWHAT	MACH_ASSERT

#if	SAYWHAT
#define NWHAT 16384
long whatmsgtype[NWHAT];
long whathappened[NWHAT];
static saywhat(t, w)
	register long	t;
	register long	w;
{
	register long	i;

	i = t % NWHAT;
	if (whatmsgtype[i] != t) {
		whatmsgtype[i] = t;
		whathappened[i] = w;
	} else {
		whathappened[i] += w;
	}
}
#else	SAYWHAT
#define saywhat(t, w)
#endif	SAYWHAT

int _mcmsg_nxn_to_xmsg = 0;	/* debug */

select_item_t *mcmsg_find_nx_recv();

/*
 *	Routine:
 *		mcmsg_recv_nx1
 *
 *	Purpose:
 *		Receive first packet of NX message
 *
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
	register xmsg_t		*xmsg;
	register xmsg_t		*xp;
	register unsigned long	buf;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register mcmsg_task_t	*mt;
	register unsigned long	msg_size;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register unsigned long	resid;
	register nxreq_t	*np;
	register unsigned long	seq;
	register int		give;
	register int		t, buf_ok;
	register int		process_lock;

	recv2(source_pid, take);
	recv2(avail, source_ptype);
	recv2(dest_pid, dest_ptype);
	recv2(msg_type, msg_length);

desparation_printf("mcmsg_recv_nx1()\n");
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, msg_length, msg_type);

	pkt = (hdr2 & 0xFFFF) - 32;
	seq = (hdr2 >> 16);
	give = (hdr1 >> 16) & 0xFFFF;
	assert(pkt >= 0);
	resid = 0;

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush(pkt);
		mcmsg_task = 0;
		return 0;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;
	process_lock = mt->applinfo.process_lock;

	pid_si = mcmsg_lookup_remote(source_pid);
	assert(pid_si != 0);

	pid_si->ppid.send_avail += give;
	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(pid_si);
	}

	assert(avail + take <=
	       pid_si->ppid.recv_total -
	       pid_si->ppid.recv_give);

	/*
	 * Look for a matching receive.
	 */
	si = mcmsg_find_nx_recv(msg_type,
				dest_ptype,
				msg_length,
				pid_si->ppid.node,
				source_ptype);

	if (si != 0) {
		mcmsg_trace_debug("NX1 match", 4, si, msg_type, msg_length, take); 
	}

	if (mt == 0) {
		mcmsg_fifo_flush(pkt);
		return;
	}
	
	if (msg_length <= pkt) {
#if	iPSC860
		resid = (pkt - msg_length) & ~3;
#else	iPSC860
		resid = (pkt - msg_length) & ~7;
#endif	iPSC860
		pkt = msg_length;
	}

	/*
	 * Handle FORCE_TYPE messages
	 */
	if ((si == 0) && (msg_type & FORCE_FLAG)) {
		mcmsg_fifo_flush(pkt);
		if (resid) {
			mcmsg_fifo_flush(resid);
		}
		mcmsg_relinquish(mt, pid_si, take);
		if (msg_length > take - sizeof(xmsg_t)) {
			mcmsg_send(SENDMETH_NXF, pid_si, seq);
		}
		return;
	}

	/*
	 * Setup data buffer pointer.
	 */
	bp1 = 0;
	bp2 = 0;
	if (si != 0) {
		buf = si->nxrq.buf;
		if (pkt > 0) {
			bp1 = mcmsg_validate(buf);
			bp2 = mcmsg_validate(((unsigned long)buf) + pkt - 1);
		}
	}
	buf_ok = (pkt == 0) || (bp1 && bp2);

	if (si && buf_ok) {
		/*
		 *	A receive is posted -AND-  the data buffer is valid.
		 */
		if (pkt > 0) {
			mcmsg_recv_buf(bp1, bp2, pkt);
			if (resid) {
				mcmsg_fifo_flush(resid);
			}
		}
		mcmsg_relinquish(mt, pid_si, take);
		if (process_lock) {
			si->nxrq.take = 0;
			si->nxrq.stop = msg_length;
		} else {
			si->nxrq.take = take;
			si->nxrq.stop = take - sizeof(xmsg_t);
		}
		si->nxrq.pid_si = pid_si;
		si->nxrq.offset = 0;

		if (msg_length <= pkt) {
			/*
			 * Posted Receive complete.
			 */
			np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
			if (np != 0) {
				np->state = NX_COMPLETE;
				mcmsg_trace_debug("nx1 complete", 2, si, np, 0, 0);

				/* check for hrecv handler */
				if (np->handler != 0) {
					mcmsg_trace_debug("hrecv ast nx1", 2, 
					                   np, np->hparam, 0, 0);
					mcmsg_hreq_ast(mt->task, si->nxrq.request);
				}
			}

			saywhat(msg_type, 1000);
			si->nxrq.sequence = 0; /* XXX debug */
			mcmsg_free_select_item(si);

		} else {
			/*
			 * Posted Receive not complete.
			 */
			saywhat(msg_type, 2000);
			si->method = SELMETH_RECV_ANY;
			si->nxrq.sequence = seq;
			si->mcmsg_task = mt;
			mcmsg_install_sequence(source_pid, si);
			if (process_lock &&
			    msg_length > take - sizeof(xmsg_t)) {
				si->nxrq.stop = msg_length;
				mcmsg_send(SENDMETH_RKR,
					   pid_si,
					   si->nxrq.sequence);
			} else if (pkt == si->nxrq.stop) {
				si->nxrq.stop += take - sizeof(xmsg_t);
				mcmsg_send(SENDMETH_NXC,
					   si,
					   si->nxrq.sequence);
			}
		}
	} else {
		register unsigned long buffer_take;
		register unsigned long buffer_whole;	/* boolean */

		/* 
		 *	NO receive is posted -OR- buffer not resident.
		 *  Need to buffer the data in an xmsg.
		 */

		if (mcmsg_appropriate(mt, pid_si, take)) {
			mcmsg_relinquish(mt, pid_si, take);
		}

		saywhat(msg_type, 3000);
		msg_size = ((msg_length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));
		assert(mt->provided >= mt->assigned);
		avail = mt->provided - mt->assigned;


		/*
		 * Build a message select item if needed.
		 */
		if (si == 0 && (msg_length > pkt)) {
			si = mcmsg_alloc_select_item();
			si->method        = SELMETH_RECV_XMSG;
			si->value         = source_pid;
			si->nxrq.offset   = 0;
			si->nxrq.count    = msg_length;
			si->nxrq.request  = 0;
			si->nxrq.sequence = seq;
			si->nxrq.pid_si   = pid_si;
			si->nxrq.msg_type = msg_type;
		}

		/*
		 * If the next xmsg buffer is smaller than msg length,
		 * only ask for 'take' in order to avoid trashing
		 * large xmsg buffers.
		 */
		if (avail >= msg_size && 
		    mcmsg_xmsg_head_size() >= msg_length) {
			buffer_take = msg_length;
			buffer_whole = 1;
		} else {
			buffer_take = take - sizeof(xmsg_t);
			buffer_whole = 0;
		}
		/*
		 * Get an Xmsg.
		 */
		xmsg = mcmsg_alloc_whole_xmsg(buffer_take, avail);
		if (xmsg != 0) {
	
			/* init xmsg info */
			xp = (xmsg_t *)mcmsg_validate_line(xmsg);
			xp->msg_type = msg_type;
			xp->length = msg_length;
			xp->source_node = pid_si->ppid.node;
			xp->source_ptype = source_ptype;
			xp->dest_ptype = dest_ptype;
	
			buf = (unsigned long)(xmsg + 1);
			mcmsg_trace_debug("nx1 alloc xmsg", 4,
			                  si, xmsg, msg_length, buffer_take);
	
			if (msg_length <= pkt) {

				/* message complete */
				xp->state = XMSG_FULL;
				xp->si = 0;
			} else {

				/* message not complete */
				assert(si != 0);
				si->method        = SELMETH_RECV_XMSG;
				si->mcmsg_task    = mt;
				si->nxrq.sequence = seq;
				si->nxrq.pid_si   = pid_si;
				si->nxrq.xmsg     = xmsg;
				si->nxrq.take     = take;
				si->nxrq.stop     = buffer_take;
				xp->si = si;
				mcmsg_install_sequence(source_pid, si);

				if (buffer_whole) {
				    if (msg_length > take - sizeof(xmsg_t)) {
						mcmsg_send(SENDMETH_RKR,
						           pid_si,
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
			 * No room.
			 */
#if 1
			mcmsg_trace_debug("starve: tk avl", 2, 
			                  buffer_take, avail, 0, 0);
			assert(0);
#endif
			mcmsg_trace_drop("space not found", msg_length);
			mcmsg_msg_drop++;
			mcmsg_fifo_flush(pkt);
			if (resid) {
				mcmsg_fifo_flush(resid);
			}
			if (si != 0) {
				mcmsg_free_select_item(si);
			}
			return;
		}

		/*
		 * Receive the data into the Xmsg
		 */
		if (pkt > 0) {
			bp1 = mcmsg_validate(buf);
			bp2 = mcmsg_validate(((unsigned long)buf) + pkt - 1);
			if (bp1 && bp2) {
				mcmsg_recv_buf(bp1, bp2, pkt);
			} else {
				mcmsg_trace_drop("nx1 xbuf invalid", buf);
				mcmsg_msg_drop++;
				mcmsg_fifo_flush(pkt);
			}
			if (resid) {
				mcmsg_fifo_flush(resid);
			}
		}

		if (si != 0) {
			/*
		 	* Set State of NX request
		 	*/
			np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
			if (np != 0) {
				saywhat(msg_type, 10000);
				mcmsg_trace_debug("nx1 buffered", 3, si, np, xmsg, 0);
				np->xmsg = xmsg;
				np->state = NX_BUFFERED;

				if (msg_length <= pkt) {
					/* Check hrecv handler */
					if (np->handler != 0) {
						mcmsg_trace_debug("hrecv ast nx1 b",
						                   2, np, np->hparam, 0, 0);
						mcmsg_hreq_ast(mt->task, si->nxrq.request);
					}
					if (si != 0) {
						mcmsg_free_select_item(si);
					}
				}
			}
		}
	}
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
 */
mcmsg_recv_nxn(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	source_pid;
	register unsigned long	offset, xoff;
	register int		pkt;
	register int		seq;
	register int		give;
	register xmsg_t		*xmsg;
	register xmsg_t		*xp;
	register unsigned long	buf;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register mcmsg_task_t	*mt;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register unsigned long	resid;
	register nxreq_t	*np;
	register unsigned long	*ip;
	register int		process_lock;

	recv2(source_pid, offset);
	assert((offset & (PKT_GRAN-1)) == 0);

	pkt = (hdr2 & 0xFFFF) - 8;
	seq = (hdr2 >> 16);
	give = (hdr1 >> 16) & 0xFFFF;
	assert(pkt >= 0);
	resid = 0;

	si = mcmsg_lookup_sequence(source_pid, seq);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 2, offset, si);
	if (si == 0) {
		mcmsg_trace_drop("seq not found", seq);
		mcmsg_msg_drop++;
		mcmsg_fifo_flush(pkt);
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;
	process_lock = mt->applinfo.process_lock;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	pid_si->ppid.send_avail += give;

	if (give > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(pid_si);
	}

	if (si->nxrq.count - offset < pkt) {
#if	iPSC860
		resid = (pkt - (si->nxrq.count - offset)) & ~3;
#else	iPSC860
		resid = (pkt - (si->nxrq.count - offset)) & ~7;
#endif	iPSC860
		pkt = si->nxrq.count - offset;
	}

	switch (si->method) {

	case SELMETH_RECV_ANY:
		buf = si->nxrq.buf;
		if (pkt > 0) {
			bp1 = mcmsg_validate(((unsigned long)(buf)) + offset);
			bp2 = mcmsg_validate(((unsigned long)(buf))+offset+pkt-1);
			if (bp1 == 0 || bp2 == 0) {
				register unsigned long take;
				/*
				 * Buffer not Present:
				 * At this point, We have been copying data directly 
				 * into the user buffer, now we must start
				 * buffering in Xmsg Buffers.
				 */
				take = si->nxrq.take;

				_mcmsg_nxn_to_xmsg++;	/* debug */
				mcmsg_trace_debug("nxn to xmsg", 3, si, xmsg, take, 0);
				assert(!process_lock);
				assert(take > 0);

				/* Allocate and Validate xmsg */

				mcmsg_appropriate(mt, pid_si, take);
				assert(mt->provided >= mt->assigned);

				xmsg = mcmsg_alloc_whole_xmsg(take - sizeof(xmsg_t),
					mt->provided - mt->assigned);
				xp = (xmsg_t *)mcmsg_validate_line(xmsg);
				if (xp == 0) {
					mcmsg_trace_drop("no valid xmsg for take",
							si->nxrq.take);
					mcmsg_msg_drop++;
					mcmsg_fifo_flush(pkt);
					if (resid) {
						mcmsg_fifo_flush(resid);
					}
					return;
				}

				si->nxrq.xmsg = xmsg;
				si->method = SELMETH_RECV_XMSG;

				xp->length      = si->nxrq.count - offset;
				xp->xmsg_offset = offset;
				xp->xmsg_data   = 0;
				xp->si          = si;
				xp->state       = XMSG_CONT;

				np = (nxreq_t *)
				     mcmsg_validate_long(si->nxrq.request);
				if (np != 0) {
					np->xmsg  = xmsg;
					np->state = NX_BUFFERED;
				} else {
					mcmsg_trace_drop("nxreq invalid",
							 si->nxrq.request);
					mcmsg_msg_drop++;
					mcmsg_fifo_flush(pkt);
					if (resid) {
						mcmsg_fifo_flush(resid);
					}
					return;
				}

				buf = (unsigned long)(xmsg + 1);
				goto do_xmsg;
			}
			mcmsg_recv_buf(bp1, bp2, pkt);
			if (resid) {
				mcmsg_fifo_flush(resid);
			}
		}

		if (si->nxrq.count <= offset + pkt) {

			/* Message Complete */

			nxreq_t	*np;
			np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
			if (np == 0) {
				mcmsg_trace_drop("nxreq invalid",
						 si->nxrq.request);
				mcmsg_msg_drop++;
				mcmsg_fifo_flush(pkt);
				if (resid) {
					mcmsg_fifo_flush(resid);
				}
				return;
			}
			np->state = NX_COMPLETE;
			mcmsg_trace_debug("nxn complete", 2, si, np, 0, 0);

			/* check for hrecv handler */
			if (np->handler != 0) {
				mcmsg_trace_debug("hrecv ast nxn", 1, 
				                   np->hparam, 0, 0, 0);
				mcmsg_hreq_ast(mt->task, si->nxrq.request);
			}
			mcmsg_remove_sequence(source_pid, si);

		} else if (!process_lock && (si->nxrq.stop == offset + pkt)) {

			/*
			 * Message Not complete, if NOT process lock 
			 * and we have a 'take' worth of message, tell 
			 * the sender to continue.
			 */

			si->nxrq.stop += si->nxrq.take - sizeof(xmsg_t);
			mcmsg_send(SENDMETH_NXC, si, si->nxrq.sequence);
		}
		break;

	case SELMETH_RECV_XMSG:
		xmsg = si->nxrq.xmsg;
		xp = (xmsg_t *)mcmsg_validate_line(xmsg);
		if (xp == 0) {
			mcmsg_trace_drop("xmsg not valid", xmsg);
			mcmsg_msg_drop++;
			mcmsg_fifo_flush(pkt);
			if (resid) {
				mcmsg_fifo_flush(resid);
			}
			return;
		}
		buf = (unsigned long)(xmsg + 1);
		if (pkt > 0) {
	do_xmsg:
			if (xp->state == XMSG_CONT) {
				xoff = xp->xmsg_data;
			} else {
				xoff = offset;
			}
			bp1 = mcmsg_validate(((unsigned long)(buf)) + xoff);
			bp2 = mcmsg_validate(((unsigned long)(buf)) + xoff + pkt - 1);
			if (bp1 == 0 || bp2 == 0) {
				mcmsg_trace_drop("xmsg buf not valid",
					((unsigned long)(buf)) + xoff);
				mcmsg_msg_drop++;
				mcmsg_fifo_flush(pkt);
				if (resid) {
					mcmsg_fifo_flush(resid);
				}
				return;
			}
			/*
			 * Recv the data into the XMSG buffer.
			 */
			mcmsg_recv_buf(bp1, bp2, pkt);
			if (resid) {
				mcmsg_fifo_flush(resid);
			}
		}

		if (xp->state == XMSG_CONT) {
			if (si->nxrq.count <= offset + pkt) {

				/* Message Complete */
				mcmsg_trace_debug("nxn xmsg last stop", 
				                  4, si, xmsg, pkt, xp->xmsg_offset);
				xp->length = xp->xmsg_data + pkt;
				xp->xmsg_stop = xp->xmsg_data + pkt;
				xp->state = XMSG_STOP;
				mcmsg_remove_sequence(source_pid, si);

			} else if (si->nxrq.stop == offset + pkt) {

				/* Chunk Complete */
				mcmsg_trace_debug("nxn xmsg stop", 
				                  4, si, xmsg, pkt, xp->xmsg_offset);

				si->nxrq.stop += si->nxrq.take - sizeof(xmsg_t);
				xp->xmsg_stop = xp->xmsg_data + pkt;
				xp->state = XMSG_STOP;

			} else {

				/* Continue Chunk */
				mcmsg_trace_debug("nxn xmsg cont", 
				                  4, si, xmsg, pkt, xp->xmsg_data);
				xp->xmsg_data += pkt;
			}
		} else {
			/* XMSG EMPTY */
			if (si->nxrq.count <= offset + pkt) {

				/* Message Complete */
				mcmsg_trace_debug("nxn xmsg last full", 4, si,xmsg,pkt,offset);
				xp->state = XMSG_FULL;
				mcmsg_remove_sequence(source_pid, si);

			} else if (xp->size == offset + pkt) {

				/* Done with XMSG chunk */
				mcmsg_trace_debug("nxn xmsg full", 4, si, xmsg, pkt, offset);
				si->nxrq.stop += si->nxrq.take - sizeof(xmsg_t);
				xp->state = XMSG_FULL;

			}
		}
		break;

	default:
		assert(0);
	}
}

/*
 *	Routine:
 *		mcmsg_send_nx1
 *
 *	Purpose:
 *		Send first packet of NX message
 *
 */
mcmsg_send_nx1(si, avail)
	select_item_t		*si;
	register unsigned long	avail;
{
	register unsigned long	length;
	register unsigned long	take;
	register unsigned long	pkt;
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;
	register unsigned long	bufp;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register nxreq_t		*np;

	if (!mcmsg_validate_send_buffer(SENDMETH_NX1, si)) {
		return 0;
	}
	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	pkt = mcmsg_task->applinfo.pkt_size;
	length = si->nxrq.count;
	bufp = si->nxrq.buf;
	if (length <= pkt) {
		pkt = (length + PKT_GRAN-1) & ~(PKT_GRAN-1);

		/* verify wired buffer */

		if (length > 0) {
			bp1 = mcmsg_validate(bufp);
			bp2 = mcmsg_validate(bufp + length-1);
			if (bp1 == 0 || bp2 == 0) {
				return 0;
			}
		}
	} else {
		bp1 = mcmsg_validate(bufp);
		bp2 = mcmsg_validate(bufp + pkt-1);
		if (bp1 == 0 || bp2 == 0) {
			mcmsg_trace_drop("nx1 send invalid buffer", bufp);
			mcmsg_free_select_item(si);
			mcmsg_msg_drop++;
			return 0;
		}
		si->nxrq.offset = pkt;
	}
	take = si->nxrq.take;
	hdr1 = MCTRL_NX1 | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = (pkt + 32) | (si->nxrq.sequence << 16);
	hdr3 = mcmsg_task->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2, take, si);

	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, take);
	send2(avail, si->nxrq.source_ptype);
	send2(pid_si->value, si->nxrq.dest_ptype);
	if (length > 0) {
		send2(si->nxrq.msg_type, length);
		mcmsg_send_buf(bp1, bp2, pkt);
	} else {
		send2eod(si->nxrq.msg_type, length);
	}
	if (length > pkt) {
		if (pkt == si->nxrq.stop) {
			register select_item_t *sh;
			register select_item_t *st;

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
			si->nextmethod = SENDMETH_NXN;
			return 0;
		}
		return SENDMETH_NXN;

	} else {

		/* message complete */

		np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
		assert(np != 0);
		np->state = NX_COMPLETE;

		/* check for handler request */

		if (np->handler != 0) {
			mcmsg_trace_debug("hsend ast nx1", 1,
			                   np->hparam, 0, 0, 0);
			mcmsg_hreq_ast(mcmsg_task->task, si->nxrq.request);
		}

		mcmsg_free_select_item(si);

		/* Check for more messages to send to this pid. */
	
		if (pid_si->ppid.send_avail > 0 && pid_si->ppid.send_wait != 0) {
			mcmsg_release_send_wait(pid_si);
		}
	}
	return 0;
}

/*
 *	Routine:
 *		mcmsg_send_nxn
 *
 *	Purpose:
 *		Send subsequent packet of NX message
 *
 */
mcmsg_send_nxn(si, dummy)
	select_item_t		*si;
	register unsigned long	dummy;
{
	register unsigned long	length;
	register unsigned long	pkt;
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;
	register unsigned long	bufp;
	register unsigned long	offset;
	register unsigned long	stop;
	register unsigned long	bp1;
	register unsigned long	bp2;

	if (!mcmsg_validate_send_buffer(SENDMETH_NXN, si)) {
		return 0;
	}
	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);

	pkt = mcmsg_task->applinfo.pkt_size;
	offset = si->nxrq.offset;
	assert(offset < si->nxrq.count);

	stop = si->nxrq.stop;
	assert(offset < stop);
	length = si->nxrq.count - offset;
	bufp = si->nxrq.buf + offset;
	if (length < pkt) {
		pkt = (length + PKT_GRAN-1) & ~(PKT_GRAN-1);
	} else {
		si->nxrq.offset = offset + pkt;
	}

#if 0
	mcmsg_trace_debug("snxn stop off len pkt", 4, stop, offset, length, pkt);
#endif

	/* Verify wired buffers */

	bp1 = mcmsg_validate(bufp);
	bp2 = mcmsg_validate(bufp + pkt - 1);
	if (bp1 == 0 || bp2 == 0) {
		mcmsg_trace_drop("nxn send invalid buffer", bufp);
		mcmsg_free_select_item(si);
		mcmsg_msg_drop++;
		return 0;
	}

	hdr1 = MCTRL_NXN | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = (pkt + 8) | (si->nxrq.sequence << 16);
	hdr3 = mcmsg_task->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2, offset, si);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, offset);
	mcmsg_send_buf(bp1, bp2, pkt);

	assert(offset < si->nxrq.count);
	length = si->nxrq.count - offset;
	if (length > pkt) {
		if (offset + pkt >= stop) {
			register select_item_t *sh;
			register select_item_t *st;

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
			si->nextmethod = SENDMETH_NXN;
			si->nxrq.stop = si->nxrq.count;
			return 0;
		}
		return SENDMETH_NXN;
	}

	{
		nxreq_t	*np;

		np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
		if (np != 0) {
			np->state = NX_COMPLETE;

			/* check for handler request */
			if (np->handler != 0) {
				mcmsg_trace_debug("hsend ast nxn", 1,
				                   np->hparam, 0, 0, 0);
				mcmsg_hreq_ast(mcmsg_task->task, si->nxrq.request);
			}
		}
	}

	mcmsg_trace_debug("send nxn seq done", 1, si->nxrq.sequence, 0, 0, 0);
	mcmsg_free_select_item(si);

	/* Check for more messages to send to this pid. */
	
	if (pid_si->ppid.send_avail > 0 && pid_si->ppid.send_wait != 0) {
		mcmsg_release_send_wait(pid_si);
	}

	return 0;
}

/*
 *	Routine:	syscall_mcmsg_masktrap
 *
 *	Set hrecv/hsend trap mask value.
 *
 */
syscall_mcmsg_masktrap(mask)
	long	mask;
{
	register task_t		task;
	register mcmsg_task_t	*mt;
	int x;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	

	task = current_task();
	mt = task->mcmsg_task;

	mcmsg_trace_debug("masktrap", 2, mt, mask, 0, 0);

	mt->masktrap = mask;

	/*
	 * If handlers enabled, set AST.
	 */
	if (mask == 0) {
		ast_on(cpu_number(), AST_MCMSG);
	}
	
	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);

	return 0;
}

syscall_mcmsg_nx_send(msg_type, buf, count, node, dest_ptype, source_ptype, nxreq)
	long			msg_type;
	unsigned long		buf;
	long			count;
	unsigned long		node;
	long			dest_ptype;
	long			source_ptype;
	nxreq_t			*nxreq;
{
	register int		app;
	register task_t		task;
	register int		x;
	register long		dest_pid;
	register long		i;
	register mcmsg_task_t	*mt;
	register unsigned long	sequence;
	register select_item_t	*pid_si;
	register select_item_t	*si;
	register select_item_t	*st;
	register select_item_t	*sh;

desparation_printf("syscall_nx_send()\n");

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	task = current_task();
	mt = task->mcmsg_task;
	if (mt == 0) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		mcmsg_trace_debug("nx send: error -1", 0, 0, 0, 0, 0);
		splx(x);
		return -1;
	}
	mcmsg_phys = 0;

	mcmsg_task = mt;

	sequence = mcmsg_send_sequence;
	mcmsg_send_sequence = sequence + 1;

	dest_pid = mcmsg_remote_pid(node, dest_ptype);
	if (dest_pid != -1) {
		pid_si = mcmsg_lookup_remote(dest_pid);
		if (pid_si != 0) {
			if (!pid_si->ppid.send_ready) {
				pid_si = 0; /* Not ready for sends */
			}
		}
	} else {
		if (node < 0 || node > mt->numnodes) {
			assert(mcmsg_reentry--);
			RED_OFF(RED_MSG);
			mcmsg_trace_debug("nx send: error -3", 0, 0, 0, 0, 0);
			splx(x);
			return -3;
		}
		pid_si = 0;
	}

	si = mcmsg_alloc_select_item();
	mcmsg_trace_debug("nx send", 4, msg_type, dest_pid, sequence, si);
	assert(si != 0);
	si->item = (void *)si;
	si->nxrq.request = (void *)nxreq;
	si->value = sequence;
	si->method = 0;
	si->nextmethod = SENDMETH_NXN;
	si->mcmsg_task = mt;
	si->nxrq.pid_si = pid_si;
	si->nxrq.dest_node = node;
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

	if (pid_si != 0) {
		mcmsg_schedule_send(si);
	} else {
		mcmsg_inquire(si);
	}
	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

mcmsg_schedule_send(si)
	register select_item_t	*si;
{
	register mcmsg_task_t	*mt;
	register unsigned long	msg_size;
	register select_item_t	*pid_si;
	register select_item_t	*st;
	register select_item_t	*sh;

	mt = si->mcmsg_task;
	msg_size = (si->nxrq.count + 2*sizeof(xmsg_t)-1) &
		    ~(sizeof(xmsg_t)-1);
	mcmsg_trace_debug("sched send", 3, si, si->nxrq.msg_type, msg_size, 0);

	pid_si = si->nxrq.pid_si;
	if (pid_si->ppid.send_avail >= 
		   mt->applinfo.send_threshold + sizeof(xmsg_t)) {
		if (msg_size > mt->applinfo.send_count + sizeof(xmsg_t)) {
			msg_size = mt->applinfo.send_count + sizeof(xmsg_t);
		}
		if  (msg_size > pid_si->ppid.send_avail) {
			msg_size = pid_si->ppid.send_avail;
		}
		si->nxrq.take = msg_size;
		si->nxrq.stop = msg_size - sizeof(xmsg_t);
		pid_si->ppid.send_avail -= msg_size;
		saywhat(si->nxrq.msg_type, 2);
		mcmsg_send(SENDMETH_NX1, si, pid_si->ppid.send_avail);
	} else if (pid_si->ppid.send_avail >=
			mt->applinfo.pkt_size + sizeof(xmsg_t)) {
		if (msg_size <= mt->applinfo.pkt_size + sizeof(xmsg_t)) {
			si->nxrq.take = msg_size;
		} else {
			si->nxrq.take = mt->applinfo.pkt_size + sizeof(xmsg_t);
		}
		si->nxrq.stop = si->nxrq.take - sizeof(xmsg_t);
		assert(pid_si->ppid.send_avail >= si->nxrq.take);
		pid_si->ppid.send_avail -= si->nxrq.take;
		saywhat(si->nxrq.msg_type, 4);
		mcmsg_send(SENDMETH_NX1, si, pid_si->ppid.send_avail);
	} else if (pid_si->ppid.send_avail >= msg_size) {
		si->nxrq.take = msg_size;
		si->nxrq.stop = msg_size - sizeof(xmsg_t);
		assert(pid_si->ppid.send_avail >= si->nxrq.take);
		pid_si->ppid.send_avail -= si->nxrq.take;
		saywhat(si->nxrq.msg_type, 6);
		mcmsg_send(SENDMETH_NX1, si, pid_si->ppid.send_avail);
	} else {
		saywhat(si->nxrq.msg_type, 8);
		st = pid_si->ppid.send_wait;
		pid_si->ppid.send_wait = si;
		mcmsg_trace_debug("sched no room", 3, si, st, pid_si, 0);
		if (st == 0) {
			si->link = si;
			si->method = 0;
			if (pid_si->ppid.process_lock) {
				mcmsg_send(SENDMETH_NXS, si, si->nxrq.sequence);
			} else {
				si->nextmethod = SENDMETH_NX1;
			}
		} else {
			sh = st->link;
			assert(sh != 0 && sh->method != 0xdead);
			si->link = sh;
			st->link = si;
			if (pid_si->ppid.process_lock) {
				si->method = SENDMETH_NXS;
				mcmsg_send(SENDMETH_NXM, si, si->nxrq.sequence);
			} else {
				si->method = 0;
				si->nextmethod = SENDMETH_NX1;
			}
		}
	}
}

syscall_mcmsg_nx_recvx(typesel, buf, count, nodesel, ptypesel,
		       ptype, nxreq, scanp)
	long		typesel;
	unsigned long	buf;
	unsigned long	count;
	long		nodesel;
	long		ptypesel;
	long		ptype;
	nxreq_t		*nxreq;
	xmsg_t		*scanp;
{
	register int		app;
	register task_t		task;
	register int		x;
	register int		i;
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register select_item_t	*pid_si;
	register select_item_t	*pid_last;
	register select_item_t	*sel;
	register select_item_t	*path;
	register xmsg_t		*xmsg;
	register nxreq_t	*np;
	register unsigned long	*ip;
	int			t;

desparation_printf("syscall_nx_recvx()\n");

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	task = current_task();
	mt = task->mcmsg_task;
	if (mt == 0) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return 1;
	}
	mcmsg_phys = 0;

	mcmsg_task = mt;

	mcmsg_trace_debug("nx recv", 4, typesel, buf, nxreq, scanp);

	/*
	 * Look for an XMSG that matches the request.
	 */
	xmsg = 0;
	if (scanp != 0) {
		register xmsg_t	*xp;

		assert((t = MAXLOOP) != 0);
		while (scanp != mt->xmsg_head) {
			xp = (xmsg_t *)mcmsg_validate_line(scanp);
			if (xp == 0) {
				mcmsg_trace_drop("scan ptr", scanp);
				assert(mcmsg_reentry--);
				RED_OFF(RED_MSG);
				splx(x);
				return 2;
			}
			if (xp->state == XMSG_FREE) {
				mcmsg_trace_drop("scan state", scanp);
				assert(mcmsg_reentry--);
				RED_OFF(RED_MSG);
				splx(x);
				return 3;
			}
			if (xp->state != XMSG_TRASH &&
			    mcmsg_nx_match(typesel,
					   xp->msg_type,
					   nodesel,
					   xp->source_node,
					   ptypesel,
					   xp->source_ptype)) {
				/* Match */
				mcmsg_trace_debug(" scan retd", 4,
						scanp, xp->state,
						xp->msg_type, mt->xmsg_head);
				saywhat(typesel, 10);
				if ( xp->length <= count &&
				    xp->size < xp->length) {
					si = xp->si;
					if (mcmsg_check_si(si) == -1) {
						assert(mcmsg_reentry--);
						RED_OFF(RED_MSG);
						splx(x);
						return 4;
					}
					/*
					 * If process_lock then set stop to the end of the
					 * message, but...if the xmsg is not yet full, we 
					 * are caught in the WAIT/CONTINUE cycle as if 
					 * process lock were off.
					 */
					if (mt->applinfo.process_lock && 
					    xp->state == XMSG_FULL) {
						si->method = SELMETH_RECV_ANY;
						si->nxrq.stop = count;
					} else {
						si->method = SELMETH_RECV_XMSG;
						if (count < xp->size) {
							si->nxrq.stop = count;
						}
					}
					si->nxrq.buf = buf;
					si->nxrq.request = (void *)nxreq;
				}
				nxreq->xmsg = scanp;
				nxreq->state = NX_BUFFERED;

				/* check for handler request */
				if (nxreq->handler != 0) {
					mcmsg_trace_debug("hrecv ast recvx i", 1, 
					                   nxreq->hparam, 0, 0, 0);
					mcmsg_hreq_ast(task, nxreq);
				}

				assert(mcmsg_reentry--);
				RED_OFF(RED_MSG);
				splx(x);
				return 0;
			}
			scanp = xp->link;
#if DANGEROUS
assert(scanp != 0);
#endif DANGEROUS
			assert(t-- != 0);
		}
	}
	saywhat(typesel, 20);

	/*
	 * No XMSG was found, construct a request.
	 */
	si = mcmsg_alloc_select_item();
	mcmsg_trace_debug("s recv", 1, si, 0, 0, 0);
	assert(si != 0);
	si->method = SELMETH_RECV_ANY;
	si->item = (void *)si;
	si->nxrq.request = (void *)nxreq;
	si->mcmsg_task = mt;
	si->nxrq.pid_si = 0;
	si->nxrq.dest_node = nodesel;
	si->nxrq.dest_ptype = ptypesel;
	si->nxrq.source_ptype = ptype;
	si->nxrq.msg_type = typesel;
	si->nxrq.count = count;
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
				     mcmsg_validate_long(
					((nxreq_t *)(si->nxrq.request))
					 ->localinfo);
				if (ip != 0) {
					ip[0] = pid_si->ppid.rk_recv_type;
					ip[1] = pid_si->ppid.rk_recv_want;
					ip[2] = pid_si->ppid.node;
					ip[3] = pid_si->ppid.rk_recv_ptype;
				}
				if (pid_si->ppid.rk_recv_want > 0) {
					mcmsg_install_sequence(pid_si->value, si);
				} else {
					np = (nxreq_t *)
					 mcmsg_validate_line(si->nxrq.request);
					if (np != 0) {
						np->state = NX_COMPLETE;

						/* check for hrecv handler */
						if (np->handler != 0) {
							mcmsg_trace_debug("hrecv ast recvx", 1, 
							                   np->hparam, 0, 0, 0);
							mcmsg_hreq_ast(task, si->nxrq.request);
						}
							
						mcmsg_trace_debug("rw complete",
								2,
								si, np, 0, 0);
					}
					mcmsg_free_select_item(si);
				}
				mcmsg_send(SENDMETH_RKR,
					   pid_si,
					   pid_si->ppid.rk_recv_seq);
				pid_si->ppid.rk_recv_pid = 0;
				if (pid_si->ppid.recv_target ==
				    pid_si->ppid.recv_total) {
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
				assert(mcmsg_reentry--);
				RED_OFF(RED_MSG);
				splx(x);
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
		} else {
			if (path->method != SELMETH_RECV_TYPESEL) {
				sel = mcmsg_alloc_select_item();
				assert(sel != 0);

				sel->method = SELMETH_RECV_TYPESEL;
				sel->value = -1;
				sel->item = (select_t *)
				      mcmsg_l2malloc(l2size(sizeof(select_t)));
				if (sel->item == 0) {
					mcmsg_trace_drop("no free selector",
							typesel);
					mcmsg_free_select_item(sel);
					mcmsg_free_select_item(si);
					assert(mcmsg_reentry--);
					RED_OFF(RED_MSG);
					splx(x);
					return 5;
				}
				mcmsg_selector_init(sel->item,
						SELMETH_RECV_TYPESEL);
				sel->link = path->link;
				path->link = sel;
				mt->selection_path = sel;
				path = sel;
			}
			mcmsg_selector_install_si(path->item,
						  si,
						  typesel,
						  SELMETH_RECV_ANY);
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

	if (nodesel != -1 && ptypesel != -1) {
		register long	pid;

		pid = mcmsg_remote_pid(nodesel, ptypesel);
		if (pid != -1) {
			pid_si = mcmsg_lookup_remote(pid);
			if (pid_si != 0 &&
			    pid_si->ppid.recv_total == pid_si->ppid.recv_give) {
				si->nxrq.pid_si = pid_si;
				mcmsg_send(SENDMETH_NXQ, si, 0);
			}
		}
	}

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

/*
 *	Routine:
 *		syscall_mcmsg_send_continue (send_item)
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
syscall_mcmsg_nx_send_continue(send_item)
	select_item_t	*send_item;
{
	register task_t         task;
    register mcmsg_task_t   *mt;
	register select_item_t  *pid_si;
	register select_item_t  *st, *sh, *si;
	int x;
	int t;

	x = spldcm();
	RED_ON(RED_MSG);

	task = current_task();
	mt = task->mcmsg_task;
	assert (mt != 0);
	mcmsg_task = mt;

	mcmsg_trace_debug("nx send cont", 2, mt, send_item, 0, 0);

	/*
	 * Destination pid is pointed to by the send_item.
	 */
	pid_si = send_item->nxrq.pid_si;
	assert(pid_si != 0);

	/*
	 * Find send_item in the send_wait list.
	 */

	st = pid_si->ppid.send_wait;
	if (st == 0) {
		/* List is empty - ERROR */
		mcmsg_trace_drop("no send cont waiting", 0);
		mcmsg_msg_drop++;
		RED_OFF(RED_MSG);
		splx(x);
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
			RED_OFF(RED_MSG);
			splx(x);
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
	if (send_item->nextmethod == SENDMETH_NX1) {
		send_item->nextmethod = SENDMETH_NXN;
		mcmsg_send(SENDMETH_NX1, send_item, pid_si->ppid.send_avail);
	} else {
		mcmsg_send(send_item->nextmethod, send_item, send_item->nxrq.sequence);
	}

	/* done */
	
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

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

mcmsg_check_si(si)
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
	    si->mcmsg_task != mcmsg_task) {
		mcmsg_trace_drop("wrong kind item", si);
		mcmsg_msg_drop++;
		return -1;
	}
	return 0;
}

syscall_mcmsg_nx_recv_continue(si)
	register select_item_t	*si;
{
	register task_t		task;
	register int		x;
	register int		i;
	register mcmsg_task_t	*mt;
	register nxreq_t	*np;
	register xmsg_t		*xp;

desparation_printf("syscall_nx_recv_continue()\n");

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

	if (mcmsg_check_si(si) == -1) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}
	switch(si->method) {
	case SELMETH_RECV_ANY:
		mcmsg_trace_debug("nx con recv", 3,
				  si, si->nxrq.request, si->nxrq.xmsg, 0);
		np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
		if (np != 0) {
			np->state = NX_ACTIVE;
		}
		xp = (xmsg_t *)mcmsg_validate_line(si->nxrq.xmsg);
		if (xp != 0) {
			xp->state = XMSG_TRASH;
		}
		mcmsg_send(SENDMETH_RKR, si->nxrq.pid_si, si->nxrq.sequence);
		break;

	case SELMETH_RECV_XMSG:
		mcmsg_trace_debug("nx con xmsg", 2, si, si->nxrq.xmsg, 0, 0);
		mcmsg_send(SENDMETH_NXC, si, si->nxrq.sequence);
		break;
	}

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

select_item_t *
mcmsg_selector_detach_request(sel, request)
	register select_t	*sel;
	register nxreq_t	*request;
{
	register select_item_t	*si;
	register select_item_t	*st;
	register select_item_t	*sl;
	register void		**selh;
	register unsigned long	hi;
	register nxreq_t	*np;
	register long		msg_type;

	np = (nxreq_t *)mcmsg_validate_line(request);
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
	assert(sl != 0);
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

/*
 * find a recieve posted for this local send: if a matching recieve request
 * is found, return the pointer to the recieve request structure
 */
syscall_mcmsg_nx_local_send(request)
	register nxreq_t	*request;
{
	register nxreq_t	*np;
	register select_item_t	*si;
	register int		x;
	register int		nxreq;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);

	/*
	 * get the physical address of the send request
	 */
	np = (nxreq_t *)mcmsg_validate_line(request);
	if (np == 0) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return 0;
	}

	/*
	 * find the matching request: return 0 if one is not found
	 */
	si = mcmsg_find_nx_recv(np->type,
				np->ptype,
				np->bcount,
				np->node,
				np->ptype);

	if (!si) {
		mcmsg_trace_debug("local send no match", 3, 
		                   request, np->type, np->bcount, 0);
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return 0;
	}

	nxreq = (int)si->nxrq.request;
	mcmsg_free_select_item(si);

	mcmsg_trace_debug("local send match", 2, 
	                  nxreq, np->type, 0, 0);

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);

	return nxreq;
}

/*
 * schedule the async send or recieve ast
*/
syscall_mcmsg_nx_local_ast(request)
	register nxreq_t	*request;
{
	register task_t		task;
	register int		x;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);

	task = current_task();
	mcmsg_trace_debug("local ast", 2, task, request, 0, 0);
	mcmsg_hreq_ast(task, request);

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

syscall_mcmsg_nx_cancel(request)
	register nxreq_t	*request;
{
	register task_t		task;
	register int		x;
	register int		i;
	register mcmsg_task_t	*mt;
	register select_t	*sel;
	register select_item_t	*st;
	register select_item_t	*si;
	register select_item_t	*sl;
	register int		t;

desparation_printf("syscall_nx_cancel()\n");

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

	st = mt->selection_path;
	if (st == 0) {
		mcmsg_trace_debug("can none", 0, 0, 0, 0, 0);
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
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
			si = mcmsg_selector_detach_request(sel, request);
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
			mcmsg_free_select_item(si);
			assert(mcmsg_reentry--);
			RED_OFF(RED_MSG);
			splx(x);
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

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}


select_item_t *
mcmsg_find_nx_recv(msg_type,
		   dest_ptype,
		   msg_length,
		   source_node,
		   source_ptype)
	register long		msg_type;
	register long		dest_ptype;
	register unsigned long	msg_length;
	register unsigned long	source_node;
	register long		source_ptype;
{
	register mcmsg_task_t	*mt;
	register select_t	*sel;
	register select_item_t	*si;
	register select_item_t	*sl;
	register select_item_t	*st;
	register long		i;
	register int		t;
	register nxreq_t		*np;
	register unsigned long	*ip;

desparation_printf("mcmsg_find_nx_recv()\n");

	mt = mcmsg_task;
	st = mt->selection_path;
	if (st == 0) {
		return 0;
	}
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
			if (msg_type >= RESERVED_BASE_TYPE) {
				break;
			}
			if (si->nxrq.dest_node != -1 &&
			    si->nxrq.dest_node != source_node) {
				break;
			}
			if (si->nxrq.dest_ptype != -1 &&
			    si->nxrq.dest_ptype != (source_ptype & ~GLOBAL_BIT)) {
				break;
			}
			saywhat(msg_type, 400);
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
			si = mcmsg_selector_detach(sel, msg_type);
			if (si == 0) {
				return 0;
			}
			saywhat(msg_type, 100);
			goto found_recv_detached;

		case SELMETH_RECV_TYPE:
			if (si->nxrq.msg_type != msg_type) {
				break;
			}
			saywhat(msg_type, 200);
			goto found_recv;

		case SELMETH_RECV_ANY:
			saywhat(msg_type, 300);
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
			np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
			if (np == 0) {
				mcmsg_trace_drop("recv nx no req",
						 si->nxrq.request);
				mcmsg_free_select_item(si);
				return 0;
			}
			ip = (unsigned long *)
			     mcmsg_validate_long(np->localinfo);
			if (ip != 0) {
				ip[0] = msg_type;
				ip[1] = msg_length;
				ip[2] = source_node;
				ip[3] = source_ptype;
			}
			if (msg_length > si->nxrq.count) {
				mcmsg_trace_drop("recv nx too big", msg_length);
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
			mcmsg_trace_debug("find recv", 3, si, np, ip, 0);
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

mcmsg_send_nxs(si, sequence)
	select_item_t	*si;
	register unsigned long	sequence;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	hdr1 = MCTRL_NXS | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = 24 | (sequence << 16);
	hdr3 = mcmsg_task->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2,
			 si->nxrq.count, si);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, si->nxrq.source_ptype);
	send2(pid_si->value, si->nxrq.dest_ptype);
	send2eod(si->nxrq.msg_type, si->nxrq.count);
	return 0;
}

mcmsg_send_nxm(si, sequence)
	select_item_t	*si;
	register unsigned long	sequence;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	hdr1 = MCTRL_NXM | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = 24 | (sequence << 16);
	hdr3 = mcmsg_task->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 2,
			 si->nxrq.count, si);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, si->nxrq.source_ptype);
	send2(pid_si->value, si->nxrq.dest_ptype);
	send2eod(si->nxrq.msg_type, si->nxrq.count);
	return 0;
}

mcmsg_send_nxq(si, dummy)
	select_item_t	*si;
	register unsigned long	dummy;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	hdr1 = MCTRL_NXQ | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = 32;
	hdr3 = mcmsg_task->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 1, si, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2(hdr3, si->nxrq.source_ptype);
	send2(pid_si->value, si->nxrq.dest_ptype);
	send2eod(si->nxrq.msg_type, 0);
	return 0;
}

mcmsg_send_nxc(si, sequence)
	register select_item_t	*si;
	register unsigned long	sequence;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register select_item_t	*pid_si;

	pid_si = si->nxrq.pid_si;
	assert(pid_si != 0);
	hdr1 = MCTRL_NXC | (pid_si->ppid.recv_give << 16);
	pid_si->ppid.recv_give = 0;
	hdr2 = 8 | (si->nxrq.sequence << 16);
	hdr3 = mcmsg_task->pid;
	mcmsg_trace_send(hdr1, hdr2, hdr3, 1, si, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2eod(hdr3, pid_si->value);
	return 0;
}

mcmsg_send_nxf(pid_si, sequence)
	register select_item_t	*pid_si;
	register unsigned long	sequence;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	hdr3;
	register unsigned long	avail;
	register mcmsg_task_t	*mt;

	mt = mcmsg_task;

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
	}
	pid_si->ppid.recv_give = 0;

	hdr1 = MCTRL_NXF | (avail << 16);
	hdr2 = 8 | (sequence << 16);
	mcmsg_trace_send(hdr1, hdr2, mt->pid, 1, pid_si, 0);
	send2(pid_si->ppid.route, 0);
	send2(hdr1, hdr2);
	send2eod(mt->pid, pid_si->value);
	return 0;
}

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
	register xmsg_t		*xmsg;
	register xmsg_t		*xp;
	register nxreq_t	*np;
	register unsigned long	*ip;
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register unsigned long	seq_out;

	recv2(source_pid, source_ptype);
	recv2(dest_pid, dest_ptype);
	recv2(msg_type, msg_length);

desparation_printf("mcmsg_recv_nxs()\n");
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, msg_length, 0);

	sequence = hdr2 >> 16;
	if (mcmsg_lookup_sequence(source_pid, sequence) != 0) {
		mcmsg_trace_drop("already got", source_pid);
		return 0;
	}

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		mcmsg_task = 0;
		return 0;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

	pid_si = mcmsg_lookup_remote(source_pid);
	assert(pid_si != 0);
	pid_si->ppid.send_avail += hdr1 >> 16;
	seq_out = pid_si->ppid.retire_seq_out;
	while (seq_out != pid_si->ppid.retire_seq_in) {
		if (pid_si->ppid.retire_seq[seq_out] == sequence) {
			seq_out++;
			if (seq_out == RETIRE_MAX) {
				seq_out = 0;
			}
			pid_si->ppid.retire_seq_out = seq_out;
			mcmsg_trace_debug("nxs retired", 3,
					pid_si->ppid.retire_seq_in,
					pid_si->ppid.retire_seq_out,
					pid_si->ppid.retire_seq[seq_out], 0);
			return;
		}
		seq_out++;
		if (seq_out == RETIRE_MAX) {
			seq_out = 0;
		}
	}
	pid_si->ppid.retire_seq_out = seq_out;

	si = mcmsg_find_nx_recv(msg_type,
				dest_ptype,
				msg_length,
				pid_si->ppid.node,
				source_ptype);
	mt = mcmsg_task;
	if (mt == 0) {
		return;
	}

	assert(mt->provided >= mt->assigned);
	avail = mt->provided - mt->assigned;
	msg_size = ((msg_length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));

	if (si != 0) {
		si->method = SELMETH_RECV_ANY;
		si->nxrq.pid_si = pid_si;
		si->nxrq.sequence = sequence;
		si->nxrq.take = 0;
		ip = (unsigned long *) mcmsg_validate_long(
			((nxreq_t *)(si->nxrq.request))->localinfo);
		if (ip != 0) {
			ip[0] = msg_type;
			ip[1] = msg_length;
			ip[2] = pid_si->ppid.node;
			ip[3] = source_ptype;
		}

	} else if (msg_type & FORCE_FLAG) {
		mcmsg_send(SENDMETH_NXF, pid_si, hdr2 >> 16);
		return;

	} else if (avail >= msg_size &&
		   (xmsg = mcmsg_alloc_whole_xmsg(msg_length, avail)) != 0) {

		si = mcmsg_alloc_select_item();
		assert(si != 0);
		si->method = SELMETH_RECV_XMSG;
		si->nxrq.xmsg = xmsg;
		si->value = source_pid;
		si->nxrq.pid_si = pid_si;
		si->nxrq.sequence = hdr2 >> 16;
		si->nxrq.buf = (unsigned long)(xmsg + 1);
		si->nxrq.count = msg_length;
		si->nxrq.take = 0;
		si->nxrq.stop = msg_length + PKT_GRAN;
		si->nxrq.offset = 0;
		si->nxrq.request = 0;

		mcmsg_trace_debug("nxs->xmsg", 3, xmsg, msg_size, avail, 0);
		assert(mt->provided >= mt->assigned);
		xp = (xmsg_t *)mcmsg_validate_line(xmsg);
		xp->length = msg_length;
		xp->si = si;
		xp->dest_ptype = dest_ptype;
		xp->source_node = pid_si->ppid.node;
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
		return;
	}
	if (msg_length > 0) {
		mcmsg_install_sequence(source_pid, si);
	} else {
		if (si->method == SELMETH_RECV_ANY) {
			np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
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
	mcmsg_send(SENDMETH_RKR, pid_si, si->nxrq.sequence);
}

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
	register xmsg_t		*xmsg;
	register xmsg_t		*xp;
	register nxreq_t	*np;
	register mcmsg_task_t	*mt;
	register select_item_t	*si;
	register unsigned long	seq_in;

	recv2(source_pid, source_ptype);
	recv2(dest_pid, dest_ptype);
	recv2(msg_type, msg_length);

desparation_printf("mcmsg_recv_nxm()\n");
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, msg_length, 0);

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		mcmsg_task = 0;
		return 0;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	mt = mcmsg_task;

	pid_si = mcmsg_lookup_remote(source_pid);
	assert(pid_si != 0);
	pid_si->ppid.send_avail += hdr1 >> 16;

	si = mcmsg_find_nx_recv(msg_type,
				dest_ptype,
				msg_length,
				pid_si->ppid.node,
				source_ptype);
	mt = mcmsg_task;
	if (mt == 0) {
		return;
	}

	assert(mt->provided >= mt->assigned);
	avail = mt->provided - mt->assigned;
	msg_size = ((msg_length + 2*sizeof(xmsg_t)-1) & ~(sizeof(xmsg_t)-1));

	if (si != 0) {
		si->method = SELMETH_RECV_ANY;
		si->nxrq.pid_si = pid_si;
		si->nxrq.sequence = hdr2 >> 16;

	} else if (msg_type & FORCE_FLAG) {
		mcmsg_send(SENDMETH_NXF, pid_si, hdr2 >> 16);
		return;

	} else if (avail >= msg_size &&
		   (xmsg = mcmsg_alloc_whole_xmsg(msg_length, avail)) != 0) {

		si = mcmsg_alloc_select_item();
		assert(si != 0);
		si->method = SELMETH_RECV_XMSG;
		si->nxrq.xmsg = xmsg;
		si->value = source_pid;
		si->nxrq.pid_si = pid_si;
		si->nxrq.sequence = hdr2 >> 16;
		si->nxrq.buf = (unsigned long)(xmsg + 1);
		si->nxrq.count = msg_length;
		si->nxrq.take = 0;
		si->nxrq.stop = msg_length + PKT_GRAN;
		si->nxrq.offset = 0;
		si->nxrq.request = 0;

		mcmsg_trace_debug("nxm->xmsg", 1, xmsg, 0, 0, 0);
		assert(mt->provided >= mt->assigned);
		xp = (xmsg_t *)mcmsg_validate_line(xmsg);
		xp->length = msg_length;
		xp->si = si;
		xp->dest_ptype = dest_ptype;
		xp->source_node = pid_si->ppid.node;
		xp->source_ptype = source_ptype;
		xp->msg_type = msg_type;
	} else {
		return;
	}

	if (msg_length > 0) {
		mcmsg_install_sequence(source_pid, si);
	} else {
		if (si->method == SELMETH_RECV_ANY) {
			np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
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
	seq_in = pid_si->ppid.retire_seq_in + 1;
	if (seq_in == RETIRE_MAX) {
		seq_in = 0;
	}
	if (seq_in != pid_si->ppid.retire_seq_out) {
		pid_si->ppid.retire_seq[pid_si->ppid.retire_seq_in] =
			si->nxrq.sequence;
		pid_si->ppid.retire_seq_in = seq_in;
	}
	mcmsg_send(SENDMETH_RKR, pid_si, si->nxrq.sequence);
}

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
	register select_item_t	*pid_si;
	register select_item_t	*sh;
	register select_item_t	*st;
	register select_item_t	*si;

	recv2(source_pid, source_ptype);
	recv2(dest_pid, dest_ptype);
	recv2(msg_type, t);

desparation_printf("mcmsg_recv_nxq()\n");
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, msg_type, 0);

	if (mcmsg_selector_lookup(&mcmsg_pid_sel, dest_pid) == 0) {
		mcmsg_trace_drop("app not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;

	pid_si = mcmsg_lookup_remote(source_pid);
	assert(pid_si != 0);
	pid_si->ppid.send_avail += hdr1 >> 16;

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
	mcmsg_send(sh->nextmethod, sh, sh->nxrq.sequence);
	if (si != 0) {
		register method = si->method;

		si->method = 0;
		mcmsg_send(method, si, si->value);
	}
}

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
	register int		t;

	recv2(source_pid, dest_pid);
	sequence = hdr2 >> 16;
	give     = hdr1 >> 16;

desparation_printf("mcmsg_recv_nxc()\n");
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, dest_pid, 0);

	pid_si = (select_item_t *)mcmsg_selector_lookup(&mcmsg_pid_sel,
							dest_pid);
	if (pid_si == 0) {
		mcmsg_trace_drop("pid not found", dest_pid);
		mcmsg_msg_drop++;
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;

	pid_si = mcmsg_lookup_remote(source_pid);
	assert(pid_si != 0);

	/*
	 * Process give
	 */
	if (give > 0) {
		pid_si->ppid.send_avail += give;
		if (pid_si->ppid.send_wait != 0) {
			mcmsg_release_send_wait(pid_si);
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
	mcmsg_send(SENDMETH_NXN, sh, sequence);
}

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
	register unsigned char	s_out;

	recv2(source_pid, dest_pid);
	sequence = hdr2 >> 16;

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
	pid_si->ppid.send_avail += hdr1 >> 16;

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
			zap_send_store(s_out);
			mcmsg_free_select_item(st);
			return;
		}
		s_out++;
		assert(t-- != 0);
	}

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
			if (sh->value == sequence) {
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
				mcmsg_free_select_item(sh);
				if (sn != 0 && sn->method != 0) {
					register method = sn->method;

					sn->method = 0;
					mcmsg_send(method,
						   sn,
						   sn->value);
				}
				return;
			}
			if (sh == sn) {
				break;
			}
			st = sh;
			sh = st->link;
			assert(t-- != 0);
		}
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
 *		give sender buffer space via RKA
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
	pid_si->ppid.recv_give += take;
	new_send_avail = old_send_avail - take;
	if (new_send_avail <= mt->applinfo.give_threshold &&
	    old_send_avail > mt->applinfo.give_threshold) {
		mcmsg_send(SENDMETH_RKA, pid_si, mt);
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
		mt->assigned -= take;
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
	}
	return 0;
}

/*
 *
 *	Routine:
 *		mcmsg_validate_send_buffer()
 *
 *	Purpose:
 *		Check to see if the buffer we are about to send is resident
 *		in physical memory.
 *		If the buffer is not resident
 *		    Set an AST request. This requests the user mode thread
 *		    that is listening to nx_port to make the page(s) resident.
 *
 *	Parameters:
 *		method      method of send
 *		send_item	select_item that represents the current send.
 *		
 *	Returns:
 *		0	if NOT ready to send
 *		1	if ready to send (pages are resident)
 */

int mcmsg_send_hits = 0;
int mcmsg_send_misses = 0;

int
mcmsg_validate_send_buffer(method, send_item)
	int           method;
	select_item_t *send_item;
{
	register task_t			task;
	register mcmsg_task_t	*mt;
	unsigned long	buf_start, buf_end;		/* for current packet */
	unsigned long	msg_left, pkt_size;
	unsigned long	bp1, bp2;
	nxreq_t      	*nxreq;

	task      = send_item->mcmsg_task->task;
	mt        = send_item->mcmsg_task;
	/*
	 * Any data? Data guaranteed wired?
	 */
	if (mt->applinfo.process_lock ||
	    send_item->nxrq.count == 0) {
		return 1;
	}

	buf_start = send_item->nxrq.buf + send_item->nxrq.offset;
	msg_left  = send_item->nxrq.count - send_item->nxrq.offset;
	pkt_size  = send_item->mcmsg_task->applinfo.pkt_size;
	if (msg_left < pkt_size) {
		buf_end = buf_start + msg_left;
		buf_end = ((buf_end + PKT_GRAN-1) & ~(PKT_GRAN-1)) -1;
	} else {
		buf_end   = buf_start + pkt_size - 1;
	}

	/*  Check bounds  */
	bp1 = mcmsg_validate(buf_start);
	if (bp1 != 0) {
		bp2 = mcmsg_validate(buf_end);
	}

	if (bp1 == 0 || bp2 == 0) {

		/* Pages not present */

		if ( !send_item->nxrq.vm_ast_pending ) {
			mcmsg_trace_debug("vm ast not pending", 0, 0, 0, 0, 0);
		}

		/* NX Request Structures MUST be wired */
		nxreq = (nxreq_t *)mcmsg_validate_line(send_item->nxrq.request);
		assert(nxreq != 0);

		/*
		 *  Update the nxreq structure to reflect the total
		 *  remainder of the buffer to be sent, not just the
		 *  current packet. This will be inspected by the user
		 *	process to decide how many pages to make present.
		 *
		 *  XXX (work remains here for scatter/gather)
		 */
		nxreq->buf     = buf_start;
		nxreq->bsize   = msg_left;
		nxreq->bcount  = 1;
		nxreq->boffset = 0;

		/* set the vm ast request */

		mcmsg_trace_debug("set send vm ast", 3, task, nxreq, send_item, 0);
		mcmsg_vm_ast (task, send_item->nxrq.request, send_item);

		/* Update the item */
		send_item->nextmethod = method;
		send_item->nxrq.vm_ast_pending = 1;

		/* Store send on send waiting */
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

		mcmsg_send_misses++;
		return 0;
	}

	/*  Pages present  */
	mcmsg_send_hits++;
	return 1;
}
