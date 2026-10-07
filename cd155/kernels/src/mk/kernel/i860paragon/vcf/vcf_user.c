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
 
#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/vcf/vcf_pmachine.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/vcf/vcf.h>
#include <i860paragon/vcf/vcf_aux.h>
#include <i860paragon/vcf/vcf_msgin.h>

mcmsg_task_t *vcf_task;


#ifdef  NO_ASSEMBLY
/*
 * The user wants to send data.
 * First, nq this attempt to send.
 * Then if an RD has already come in, immediately shift this channel to the
 *   "transmit_send" queue.
 */
void
handle_send()  {
	vcf_chand  ch;
	vcf_req  *msg;
	unsigned  len;
	int  *sbufp;
	int  *buf;

	len =   vcf_ usrcomm->info.send_req.len;
	sbufp = vcf_ usrcomm->info.send_req.sbufp;
	buf =   (int *)vcf_ usrcomm->info.send_req.buf;
	ch =    vcf_ usrcomm->info.send_req.chan;

	sbufp = (vcf_sbuf *)vcf_validate((unsigned long)sbufp,
	                                 (unsigned long)vcf_dirbase);
	if (sbufp == NULL)
		return;
	sbufp = (int *)((int)sbufp & ~0x3);
	if (!VALID_WRITE_ADDRESS(sbufp))
		return;
	vcf_ usrcomm->status = VCF_WIN_UNUSED;

	len &= ~7;

	/* Check "ch" argument. */
	if ((ch < vcf_chan_tbl) || (ch >= vcf_chan_tbl + vcf_num_chans))  {
		*sbufp = VCF_EINVAL;
		return;
	}
	ch = (vcf_chand)((int)ch & ~(sizeof(*ch) - 1));

	/* If you closed this connection, you can't send data over it.
	 * If your peer closed this connection, he can't recv over it.
	 */
	if (ch->status & (VCF_CS_GOT_USR_CLOSE | VCF_CS_GOT_RMT_CLOSE))  {
		*sbufp = VCF_ECHANDEAD;
		return;
	}

	/* nq the request to send. */
	msg = ch->send_pool;
	if (msg == NULL)	/* No space to nq. */
		*sbufp = VCF_ENOBUF;
	else  {
		ch->send_pool = msg->next;
		msg->len = len;
		msg->sbufp = sbufp;
		msg->buf = buf;
		nq(ch->sendh, ch->sendt, msg);
		if ((ch->rd_len != 0) && (ch->sendh == msg))  {
			/* Put this channel on the "xmit_rda" queue. */
			ch->send_next = NULL;
			if (xmit_rda_head == NULL)
				xmit_rda_head = ch;
			else  {
				xmit_rda_tail->send_next = ch;
			}
			xmit_rda_tail = ch;
		}
	}
}


/*
 * The user wants to receive data.
 * Try to nq the recv.  If there's no space, report an error through sbufp.
 *   If there is space, check; if you are the only recv pending, put this
 *   channel into the "xmit_rd" queue.  Otherwise wait until you hear back
 *   from all the RDs ahead of you.
 */
void
vcf_c_handle_recv()  {
	vcf_chand  ch;
	vcf_req  *msg;
	unsigned  len;
	int  *sbufp;
	int  *buf;

	len =    vcf_ usrcomm->info.recv_req.len;
	sbufp = (vcf_sbuf *) my_validate((unsigned long) vcf_ usrcomm->info.recv_req.sbufp, (unsigned long) vcf_task->dirbase);
	if (sbufp == (vcf_sbuf *) NULL)  {
		return;
	}
	sbufp = (int *)((int)sbufp & ~0x3);
	buf =    (int *)vcf_ usrcomm->info.recv_req.buf;
	ch =     vcf_ usrcomm->info.recv_req.chan;

	vcf_ usrcomm->status = VCF_WIN_UNUSED;

	len &= ~7;

	if (!VALID_WRITE_ADDRESS(sbufp))
		return;

	/* Check "ch" argument. */
	if ((ch < vcf_chan_tbl) || (ch >= vcf_chan_tbl+vcf_num_chans))  {
		*sbufp = VCF_EINVAL;
		return;
	}

	/*  If you closed this connection, you can't receive data over it.
	 *  If your peer closed this connection, you can't send an RD over it.
	 */
	if (ch->status & (VCF_CS_GOT_USR_CLOSE | VCF_CS_GOT_RMT_CLOSE))  {
		*sbufp = VCF_ECHANDEAD;
		return;
	}

	msg = ch->recv_pool;
	if (msg == NULL)  {
		*sbufp = VCF_ENOBUF;
	} else  {
		ch->recv_pool = msg->next;
		msg->len = len;
		msg->sbufp = sbufp;
		msg->buf = buf;
		nq(ch->recvh, ch->recvt, msg);
		/*
		 * If you are at the head of the recv queue, you can send an RD
		 *   immediately.  Otherwise you have to wait until a response
		 *   comes back to the recvs ahead of you.
		 */
		if ((ch->recvh == msg) && (ch->status & VCF_CS_GOT_RMT_OPEN) &&
		    (ch->status & VCF_CS_SENT_OPEN))  {
			/* Put this channel on the "xmit_rd" queue. */
			ch->send_rd.next = NULL;
			if (vcf_xmit_head == NULL)
				vcf_xmit_head = &(ch->send_rd);
			else
				vcf_xmit_tail->next = &(ch->send_rd);
			vcf_xmit_tail = &(ch->send_rd);
		}
	}
}

#endif  /* NO_ASSEMBLY */


/*  Queue an open request on a channel.  Needs to be 
 *  called by handle_uprocreq().
 */
int
vcf_open(mcmsg_task_t *mt, int node, unsigned int id,
	 unsigned int in_reqs, unsigned int out_reqs, vcf_sbuf *sbufp,
	 volatile vcf_chan **chanout)  {
  vcf_chan *chan;
  vcf_req *request;
#if  VCF_DEBUG
  static vcf_chan  *last_found = NULL;
#endif
  
#if  VCF_DEBUG
  printf("Open started...dest=%d, id=%d, out=%d, in=%d\n",
	 node, id, out_reqs, in_reqs);
#endif
  
  if (vcf_dirbase == NULL)  {
#if  VCF_DEBUG
    printf("No dirbase!\n");
#endif
    return(0);
  }
  
  if ((node < 0) || (node >= vcf_task->task_ptype_list->numnodes))
    node = 0;
  else
    node = calculate_route(vcf_task->task_ptype_list->phys_node_list[node]);
#if  VCF_DEBUG
  printf("  Node route = 0x%x\n", node);
#endif
  sbufp = (vcf_sbuf *)my_validate((unsigned long)sbufp, vcf_dirbase);
#if  VCF_DEBUG
  printf("  sbufp = 0x%x\n", sbufp);
#endif
  chanout = (vcf_chan **)my_validate((unsigned long)chanout, vcf_dirbase);
#if  VCF_DEBUG
  printf("  chanout = 0x%x\n", chanout);
#endif
  
  if ((node == 0) ||
      (sbufp == (vcf_sbuf *)NULL) ||
      (chanout == (vcf_chan **)NULL))  {
    printf("BOGUS OPEN!\n");
    return(0);
  }
  
  /*  If a channel awaiting user open cannot be found, we must
   *  allocate one.
   */
  
  if (!(chan = vcf_find_chan_in_hash(id, node)))  {
#if  VCF_DEBUG
    printf("  Not in hash.\n");
#endif
    if (!(chan = vcf_alloc_chan(id, out_reqs, in_reqs, node)))  {
      VCF_LOCKIT(*sbufp = vcf_errno);
      return(0);
    }
#if  VCF_DEBUG
    if (chan == last_found)  {
      printf("*** chan == last found; vcf_alloc_chan(%d, %d, %d, %d)\n",
	     id, out_reqs, in_reqs, node);
    }
    last_found = chan;
#endif
  }  else  {
#if  VCF_DEBUG
    if (chan == last_found)  {
      printf("*** chan == last found; vcf_find_chan_in_hash(%d, %d, %d, %d)\n",
	     id, out_reqs, in_reqs, node);
    }
    last_found = chan;
#endif
#if  VCF_DEBUG
    printf("  In hash; status=0x%x\n", chan->status);
#endif
    if (chan->status & VCF_CS_GOT_USR_OPEN)  {
      VCF_LOCKIT(*sbufp = VCF_EINVAL);
      return(0);
    }
    if (vcf_give_pools(chan, out_reqs, in_reqs))  {
      vcf_remove_chan_from_hash(chan);
      vcf_give_chan(chan);
      /*  Need I mark outstanding sends/recvs with
       *  VCFECHANDEAD?
       */
      vcf_got_roc_ctfull(node);
      VCF_LOCKIT(*sbufp = VCF_ENOBUF);
      return(0);
    }
  }
  
  VCF_LOCKIT(*chanout = chan);
  chan->status |= VCF_CS_GOT_USR_OPEN;
  chan->open_sbufp = sbufp;
  if (!(chan->status & VCF_CS_SENT_OPEN)) {
    /*  The channel didn't send an open, whether or not it got a remote
     *  open.  Move the channel to the xmit_ctrl queue if it's not there
     *  already.  
     */
    chan->send_open.next = NULL;
    chan->status |= VCF_CS_SENT_OPEN;
    if (!vcf_xmit_head)  {
      vcf_xmit_head = &(chan->send_open);
    } else {
      vcf_xmit_tail->next = &(chan->send_open);
    }
    vcf_xmit_tail = &(chan->send_open);
  }
  
  /*  If this channel already got a remote open and sent an open,
   *  then the channel has been successfully opened.  Remove it from the
   *  ID mapping, set the response field, and return.
   */
  if ((chan->status & VCF_CS_SENT_OPEN) &&
      (chan->status & VCF_CS_GOT_RMT_OPEN))  {
    vcf_remove_chan_from_hash(chan);
    VCF_LOCKIT(*sbufp = 1);
    chan->open_sbufp = NULL;
  }
  
  /*  Set the response field, and return.  */
  return(0);
}


/*  Queue a close request or acknowledgement on a channel.  Needs to be
 *  called by handle_uprocreq().
 */

int
vcf_close(mcmsg_task_t *mt, vcf_chan *chan, vcf_sbuf *sbufp)  {
  chan->close_sbufp = (vcf_sbuf *)my_validate((unsigned long)sbufp,
					      (unsigned long)vcf_dirbase);
  if (chan->close_sbufp == (vcf_sbuf *)NULL)  {
    printf("BOGUS CLOSE!\n");
    return(0);
  }
  chan->status |= VCF_CS_GOT_USR_CLOSE;
  
  /*  If this channel already got a remote close and sent a close,
   *  then the channel has been successfully closed.  Free the channel
   *  and return.
   */
  if ((chan->status & VCF_CS_SENT_CLOSE) &&
      (chan->status & VCF_CS_GOT_RMT_CLOSE))  {
    *chan->close_sbufp = 1;
    vcf_give_chan(chan);
    return(0);
  }	else if (!chan->sendh && !chan->recvh)  {
    /*  The channel didn't send a close, whether or not it got a remote
     *  close.  If there are no pending sends or receives, move the
     *  channel to the xmit_ctrl queue and return.  (If there are pending
     *  sends/recvs, the functions that handle them will move the channel.)
     */
    chan->send_close.next = NULL;
    
    if (!vcf_xmit_head)  {
      vcf_xmit_head = &(chan->send_close);
    }  else  {
      vcf_xmit_tail->next = &(chan->send_close);
    }
    vcf_xmit_tail = &(chan->send_close);
    return(0);
  }
}


#ifdef  NO_ASSEMBLY
/*
 * Check the vcf_usrcomm data structure to determine the type of request that
 *   has been filed.  Branch to the appropriate subfunction.
 */
void
vcf_handle_uprocreq()  {
  switch(vcf_usrcomm->status)  {
  case VCF_WIN_OPEN:
    handle_open();
    break;
  case VCF_WIN_CLOSE:
    vcf_handle_close();
    break;
  case VCF_WIN_SEND:
    handle_send();
    break;
  case VCF_WIN_RECV:
    handle_recv();
    break;
#ifdef VCF_PROF
  case VCF_WIN_PROF:
    handle_prof();
    break;
#endif VCF_PROF
  default:
  }
}
#endif
