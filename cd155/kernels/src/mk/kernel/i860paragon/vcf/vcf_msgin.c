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
 
#include <i860paragon/vcf/vcf.h>
#include <i860paragon/vcf/vcf_msgin.h>
#include <i860paragon/vcf/vcf_aux.h>
#include <i860paragon/vcf/vcf_user.h>
#include <i860paragon/vcf/vcf_cnic.h>

static int       *g_pbuf;
static unsigned   g_len;
static vcf_req    *g_msg;
static vcf_chand  g_ch;

/*
 * When g_lastch is non-NULL, g_lastpbuf holds
 *   a valid pbuf for that channel.
 */
static vcf_chand  g_lastch = NULL;
static int       *g_lastpbuf;

/*
 * This variable determines current LTU activity.
 * When the inbound LTU is idle, it is set to NULL.
 * When the inbound LTU is in use, it points to a function that should
 *   be called when the LTU completes.
 * This has the following implication:
 *   - When it is non-NULL, NO control message reception can be initiated.
 */
void  (*in_ltu_active)(void) = NULL;

static void  finish_rda_ltu(void);
static void  finish_rda_noltu(void);
static void  finish_rda_badaddr(void);
static void  finish_rdac_ltu(void);
static void  finish_rdac_noltu(void);
static void  finish_rdac_badaddr(void);
void  vcf_bite_me(int x, int y);




unsigned int
INVERT_PATH(unsigned int path)  {
	/* Don't negate x- or y-displacement if it is zero */
	if (path & 0x7fff0000)
		path ^= 0x80000000;
	if (path & 0x00007fff)
		path ^= 0x00008000;
	return(path);
}

#ifdef  NO_ASSEMBLY
/*
 * An RD (request to receive data) has arrived.
 * Two possibilities:
 *   - A matching send request is already present.  Immediately send off
 *     the data.
 *   - No matching send request is present.  Enqueue the RD.
 */
void
handle_rd(long mcmsg_junk, vcf_chand  ch)  {
	vcf_req  *msg;
	unsigned  len, seqno;
	int  msg_group, path;

	NIC_RECV_WORDS(&msg_group, &path);
	NIC_RECV_WORDS((int *)&seqno, (int *)&len);
	NIC_RECV_END_MSG();

	if (msg_group != groupid)  {
		/*
		 * The RD is from the wrong group.  Reject it.
		 * Note: You CANNOT access any variables in user memory when
		 *   there is a group mismatch.  You may be in the middle of a
		 *   swap, in which case these are completely unreliable.
		 */
		vcf_got_rq_badgang(INVERT_PATH(path), seqno);
	} else if ((ch->status & VCF_CS_GOT_USR_CLOSE) && !ch->sendh)  {
		/* Channel is dead.  Don't send anything back; the RCC
		 *   will do the rejecting for us.
		 * I don't think that this "|" is necessary, but hell, why not
		 *   leave it in.
		 */
		ch->status |= VCF_CS_CHANDEAD;
	} else  {
		ch->seq_recvd = seqno;
		msg = ch->sendh;
		if (msg == NULL)  {
			/* No sends pending...just record the RD arrival. */
			ch->rd_len = len;
		} else  {
			/*
			 * A send is pending...queue this channel for service.
			 */
			if (msg->len > len)  {
				msg->len = len;
				ch->rd_len = len;
			} else
				ch->rd_len = msg->len;
			/* nq the channel on the rda queue. */
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
 * An RDA (request to receive data acknowledgement) has arrived.
 * This is the data that I requested with the RD.  Just read it in and
 *   dequeue the old RD.
 */
void
handle_rda(long mcmsg_junk, vcf_chand  ch)  {
	vcf_req  *msg;
	unsigned  len, packetlen;
	int  *pbuf, *vbuf, junk;

	msg = ch->recvh;
	NIC_RECV_WORDS((int *) &len, (int *) &(msg->len));

	vbuf = msg->buf;
	if (ch == g_lastch)  {
		/* This channel has the pbuf precomputed. */
		pbuf = (int *) g_lastpbuf;
		g_lastch = NULL;
	} else
		pbuf = (int *) my_validate((unsigned long)vbuf,
		                           (unsigned long)vcf_task->dirbase);

	/* This code is very similar to the stuff in handle_rdac() */
	/* Make sure that the address is valid!
	 * If it isn't, just throw the data away.
	 */
	if (pbuf == (int *) 0)  {
		while (len)  {
			NIC_RECV_WORDS(&junk, &junk);
			len -= 8;
		}
	}

	if ((int)pbuf & LTU_MASK)  {
		/*  Read words directly from NIC until LTU block alignment is
		 *  achieved.
		 */
		do  {
			NIC_RECV_WORDS(pbuf, pbuf+1);
			pbuf += 2;
			vbuf += 2;
			len -= 8;
		} while ((((int)pbuf & LTU_MASK) != 0) && len);
		if (!((int)pbuf & (VCF_PAGESIZE-1)))  {
			pbuf = (int *) my_validate((unsigned long)vbuf,
			                           (unsigned long)vcf_task->dirbase);
			if (!VALID_WRITE_ADDRESS(pbuf))  {
				while (len)  {
					NIC_RECV_WORDS(&junk, &junk);
					len -= 8;
				}
			}
		}
	}

	if (len & ~LTU_MASK)  {
		/* You can use the LTUs for this transaction. */
		if (VCF_PAGEMASK(pbuf) !=
		    VCF_PAGEMASK(pbuf + len/sizeof(*pbuf) - 1)) {
			/* Multiple pages...so multiple LTU transactions. */
			packetlen = ((char *)VCF_PAGEMASK((int)pbuf + VCF_PAGESIZE) -
			             (char *)pbuf);
			LTU_RECV_BLOCK(pbuf, packetlen);
			pbuf += packetlen / sizeof(*pbuf);
			vbuf += packetlen / sizeof(*vbuf);
			len -= packetlen;

			pbuf = (int *) my_validate((unsigned long) vbuf,
			                           (unsigned long) vcf_task->dirbase);

			g_pbuf = pbuf;
			g_len = len;
			g_msg = msg;
			g_ch = ch;
			if (pbuf != (int *)0)  {
				if (len & ~LTU_MASK)
					in_ltu_active = finish_rda_ltu;
				else
					in_ltu_active = finish_rda_noltu;
			} else
				/* BOGUS ADDRESS */
				in_ltu_active = finish_rda_badaddr;
			return;
		}

		/* You can use the LTUs, but you won't cross a page. */
		LTU_RECV_BLOCK(pbuf, len & ~LTU_MASK);
		pbuf += (len & ~LTU_MASK) / sizeof(*pbuf);
		len -= len & ~LTU_MASK;
		g_pbuf = pbuf;
		g_len = len;
		g_msg = msg;
		g_ch = ch;
		in_ltu_active = finish_rda_noltu;
		return;
	}
	/* No LTUs can be used.  This code is inefficient, but correct. */
	g_pbuf = pbuf;
	g_len = len;
	g_msg = msg;
	g_ch = ch;
	finish_rda_noltu();
}


static void
finish_rda_ltu(void)  {
	int  *pbuf = g_pbuf;
	unsigned  len = g_len;

	LTU_RECV_BLOCK(pbuf, len & ~LTU_MASK);
	g_pbuf = pbuf + (len & ~LTU_MASK) / sizeof(*pbuf);
	g_len = len - (len & ~LTU_MASK);
	in_ltu_active = finish_rda_noltu;
}


static void
finish_rda_badaddr(void)  {
  unsigned  len = g_len;
  int junk;

  while (len)  {
    NIC_RECV_WORDS(&junk, &junk);
    len -= 8;
  }
  g_len = 0;
  finish_rda_noltu();
}


static void
finish_rda_noltu(void)  {
	int  *pbuf = g_pbuf;
	unsigned  len = g_len;
	vcf_req  *msg = g_msg;
	vcf_chand  ch = g_ch;

	while (len & LTU_MASK)  {
		NIC_RECV_WORDS(pbuf, pbuf+1);
		pbuf += 2;
		len -= 8;
	}
	NIC_RECV_END_MSG();
	in_ltu_active = NULL;
	/* Fill in sbufp with the nbytes read. */
	*(msg->sbufp) = msg->len;
	dq(ch->recv_pool, ch->recvh);
  
	/*
	 * If this channel had been marked "REJECTED" it should now be
	 *   relabeled as "READY".
	 */
	ch->status &= ~VCF_CS_REJECTED;

	/*
	 * If another RD is waiting to go out, put this channel on the xmit_rda
	 *   queue.
	 */
	if (ch->recvh != NULL)  {
		ch->recv_next = NULL;
		if (!xmit_rd_head)
			xmit_rd_head = ch;
		else
			xmit_rd_tail->recv_next = ch;
		xmit_rd_tail = ch;
	} else if ((ch->status & (VCF_CS_GOT_USR_CLOSE|VCF_CS_GOT_RMT_CLOSE))
		   && (ch->sendh == NULL))  {
		if (!xmit_ctrl_head)
			xmit_ctrl_head = ch;
		else  {
			xmit_ctrl_tail->send_next = ch;
		}
		xmit_ctrl_tail = ch;
	}
}


/*
 * An RDAC (RDA continue) has arrived.
 * This works just like an RDA except that the message is not marked as
 *   "done" and the buffer/length message components are updated to
 *   point to the start of the next packet.
 */
void
handle_rdac(long mcmsg_junk, vcf_chand ch)  {
	vcf_req  *msg;
	unsigned  packetlen, len;
	int  *pbuf, *vbuf, junk;

	NIC_RECV_WORDS((int *) &len, (int *) &junk);
	msg = ch->recvh;
	vbuf = msg->buf;

	if (ch == g_lastch)  {
		/* This channel has the pbuf precomputed. */
		pbuf = g_lastpbuf;
	} else  {
		pbuf = (int *)my_validate((unsigned long)vbuf,
		                          (unsigned long) vcf_task->dirbase);
	}
	g_lastch = NULL;

	msg->buf += len / sizeof(*msg->buf);

	/* This code is very similar to the stuff in handle_rda() */
	/* Make sure that the address is valid!
	 * If it isn't, just throw the data away.
	 */
	if (pbuf == (int *) 0)  {
		while (len)  {
			NIC_RECV_WORDS(&junk, &junk);
			len -= 8;
		}
	}
  
	/*  Read words directly from NIC until LTU block alignment is
	 *  achieved.
	 */
	if ((int)pbuf & LTU_MASK)  {
		/*  Read words directly from NIC until LTU block alignment is
		 *  achieved.
		 */
		vbuf = (int *)(((int)vbuf + LTU_MASK) & ~LTU_MASK);
		do  {
			NIC_RECV_WORDS(pbuf, pbuf+1);
			pbuf += 2;
			len -= 8;
		} while ((((int)pbuf & LTU_MASK) != 0) && len);
		if (!((int)pbuf & (VCF_PAGESIZE-1)))  {
			pbuf = (int *) my_validate((unsigned long)vbuf,
			                           (unsigned long)vcf_task->dirbase);
			if (!VALID_WRITE_ADDRESS(pbuf))  {
				while (len)  {
					NIC_RECV_WORDS(&junk, &junk);
					len -= 8;
				}
			}
		}
	}

	/*  If the block straddles a page boundary, receive to the end of
	 *  the first page.
	 */
	if (len & ~LTU_MASK)  {
		if (VCF_PAGEMASK(pbuf) !=
		    VCF_PAGEMASK(pbuf + (len & ~LTU_MASK)/sizeof(*pbuf) - 1))  {
			packetlen = ((char *)VCF_PAGEMASK(pbuf + VCF_PAGESIZE / sizeof(*pbuf)) -
				     (char *)pbuf);
			LTU_RECV_BLOCK(pbuf, packetlen);
			pbuf += packetlen / sizeof(*pbuf);
			vbuf += packetlen / sizeof(*vbuf);
			len -= packetlen;

			pbuf = (int *) my_validate((unsigned long) vbuf,
						   (unsigned long) vcf_task->dirbase);
			g_lastch = ch;
			g_lastpbuf = (int *)((int)pbuf + len);

			g_pbuf = pbuf;
			g_len = len;
			g_msg = msg;
			g_ch = ch;

			if (pbuf != (int *) 0)  {
				if (len & ~LTU_MASK)
					in_ltu_active = finish_rdac_ltu;
				else
					in_ltu_active = finish_rdac_noltu;
			} else
				in_ltu_active = finish_rdac_badaddr;
			return;
		}
		/* This is LIKE finish_rda_ltu(), but has the extra buffer
		 *   check possibility since it could end on a page boundary.
		 * In any case, here you use the LTU but don't cross a page
		 *   boundary.
		 */
		
		LTU_RECV_BLOCK(pbuf, len & ~LTU_MASK);
		pbuf += (len & ~LTU_MASK) / sizeof(*pbuf);
		in_ltu_active = finish_rdac_noltu;
		if (((int)pbuf & (VCF_PAGESIZE - 1)) == 0)  {
			vbuf += (len & ~LTU_MASK) / sizeof(*vbuf);
			pbuf = (int *)my_validate((unsigned long)vbuf,
			                          (unsigned long)vcf_task->dirbase);
			if (pbuf == (int *)0)
				in_ltu_active = finish_rdac_badaddr;
		}
		len -= len & ~LTU_MASK;

		g_lastch = ch;
		g_lastpbuf = (int *)((int)pbuf + len);
		g_pbuf = pbuf;
		g_len = len;
		g_msg = msg;
		g_ch = ch;
		return;
	}
	g_pbuf = pbuf;
	g_len = len;
	g_msg = msg;
	g_ch = ch;
	if (((int)pbuf & (VCF_PAGESIZE - 1)) == 0)  {
		pbuf = (int *)my_validate((unsigned long) vbuf,
					   (unsigned long) vcf_task->dirbase);
		if (pbuf == (int *) 0)  {
			finish_rdac_badaddr();
			return;
		}
	}
	g_lastch = ch;
	g_lastpbuf = (int *)((int)pbuf + len);
	finish_rdac_noltu();
}


static void
finish_rdac_ltu(void)  {
  int  *pbuf = g_pbuf;
  unsigned  len = g_len;

  LTU_RECV_BLOCK(pbuf, len & ~LTU_MASK);
  g_pbuf = pbuf + (len & ~LTU_MASK) / sizeof(*pbuf);
  g_len = len - (len & ~LTU_MASK);
  in_ltu_active = finish_rdac_noltu;
}


static void
finish_rdac_badaddr(void)  {
  unsigned  len = g_len;
  int junk;

  while (len)  {
    NIC_RECV_WORDS(&junk, &junk);
    len -= 8;
  }
  g_len = 0;
  finish_rdac_noltu();
}


static void
finish_rdac_noltu(void)  {
	int  *pbuf = g_pbuf;
	unsigned  len = g_len;
	vcf_req  *msg = g_msg;
	vcf_chand  ch = g_ch;

	while (len)  {
		NIC_RECV_WORDS(pbuf, pbuf+1);
		pbuf += 2;
		len -= 8;
	}
	NIC_RECV_END_MSG();
	in_ltu_active = NULL;
}
#endif  /* NO_ASSEMBLY */

void
vcf_handle_roc(long mcmsg_junk, vcf_chand rmt_chan)  {
	vcf_chand  chan;
	unsigned int msg_pid;
	unsigned int id, seqno;
	unsigned int source_node;

#if  VCF_DEBUG
	printf("Got an open.\n");
#endif
	NIC_RECV_WORDS((int *)&msg_pid, (int *) &source_node);
	NIC_RECV_WORDS((int *)&seqno, (int *)&id);
	NIC_RECV_END_MSG();
	source_node = INVERT_PATH(source_node);

	if (/* msg_pid != vcf_groupid */ vcf_dirbase == NULL)  {
#if  VCF_DEBUG
		printf("  ***Bad gang.\n");
#endif
		vcf_got_rq_badgang(source_node, seqno);
		return;
	}  else if (!(chan = vcf_find_chan_in_hash(id, source_node)))  {
#if  VCF_DEBUG
		printf("  Not in hash.\n");
#endif
		/*  If this fails, it can only be from lack of CTEs.  */
		if (!(chan = vcf_alloc_chan(id, 0, 0, source_node)))  {
#if  VCF_DEBUG
			printf("  ***CTFull\n");
#endif
			vcf_got_roc_ctfull(source_node);
			return;
		} 
	} else  {
#if  VCF_DEBUG
		printf("  In hash.  Status = 0x%x\n", chan->status);
#endif
	}
#if  VCF_DEBUG
	printf("  ***Good gang.\n");
#endif
	if (chan->status & VCF_CS_MAYBE)  {
		chan->status &= ~VCF_CS_MAYBE;
		vcf_got_roc_mayberejected(source_node, chan);
	}
	chan->status |= VCF_CS_GOT_RMT_OPEN;
	chan->status &= ~VCF_CS_REJECTED;
	chan->peer_cte = rmt_chan;

	/*  If this channel already got a user open and sent an open, 
	 *  then the channel has been successfully opened.  Remove it from the
	 *  ID mapping and return.
	 */
	if (chan->status & VCF_CS_GOT_USR_OPEN)  {
		if (!(chan->status & VCF_CS_SENT_OPEN))  {
			/* This can only happen if the channel got rejected. */
			chan->send_open.next = NULL;
			chan->status |= VCF_CS_SENT_OPEN;
			if (!vcf_xmit_head)  {
				vcf_xmit_head = &(chan->send_open);
			} else  {
				vcf_xmit_tail->next = &(chan->send_open);
			}
			vcf_xmit_tail = &(chan->send_open);
		}

		vcf_remove_chan_from_hash(chan);

		/*  If RDs are queued, we can send them now.  */
		if (chan->recvh)  {
			chan->send_rd.next = NULL;
			if (vcf_xmit_head == NULL)
				vcf_xmit_head = &(chan->send_rd);
			else
				vcf_xmit_tail->next = &(chan->send_rd);
			vcf_xmit_tail = &(chan->send_rd);
		}
		VCF_LOCKIT(*chan->open_sbufp = 1);
		chan->open_sbufp = NULL;
#if  VCF_DEBUG
		printf("  Open successful.\n");
#endif
		return;
	} else  {
		/* Hasn't sent the open yet and not queued either.  Queue it.
		 */
		chan->send_open.next = NULL;
		chan->status |= VCF_CS_SENT_OPEN;
		if (!vcf_xmit_head)  {
			vcf_xmit_head = &(chan->send_open);
		} else  {
			vcf_xmit_tail->next = &(chan->send_open);
		}
		vcf_xmit_tail = &(chan->send_open);

		/*  If RDs are queued, we can send them now.  */
		if (chan->recvh)  {
			chan->send_rd.next = NULL;
			if (!vcf_xmit_head)
				vcf_xmit_head = &(chan->send_rd);
			else
				vcf_xmit_tail->next = &(chan->send_rd);
			vcf_xmit_tail = &(chan->send_rd);
		}
	}
}


void
vcf_handle_rcc(long mcmsg_junk, vcf_chand local_chan)  {
	unsigned int msg_pid, junk;
	vcf_chand rmt_chan;
	unsigned int source_node, seqno;
	vcf_req *req;
	int  need_to_nq = 0;

	NIC_RECV_WORDS((int *)&msg_pid, (int *)&source_node);
	NIC_RECV_WORDS((int *)&seqno, (int *)&junk);
	NIC_RECV_END_MSG();

	source_node = INVERT_PATH(source_node);

	if (/* msg_pid != vcf_groupid */ vcf_dirbase == NULL)  {
		vcf_got_rq_badgang(source_node, seqno);
		return;
	}

	local_chan->status |= VCF_CS_GOT_RMT_CLOSE;
	for (req = local_chan->sendh;  req != NULL;  req = req->next)  {
		VCF_LOCKIT(*req->sbufp = VCF_ECHANDEAD);
		need_to_nq = 1;
	}
	for (req = local_chan->recvh;  req != NULL;  req = req->next)  {
		VCF_LOCKIT(*req->sbufp = VCF_ECHANDEAD);
		need_to_nq = 1;
	}

	/*  If this channel already got a user close and sent a close,
	 *  then the channel has been successfully closed.  Set the status
	 *  field, free the channel, allow a new process to be scheduled, 
	 *  mark the status codes of outstanding messages with CHANDEAD
	 *  and return.
	 */
	if ((local_chan->status & VCF_CS_GOT_USR_CLOSE) &&
			(local_chan->status & VCF_CS_SENT_CLOSE))  {
		VCF_LOCKIT(*local_chan->close_sbufp = 1);
		vcf_give_chan(local_chan);
		return;
	}
	if (need_to_nq && VCF_CS_GOT_USR_CLOSE)  {
		if (!vcf_xmit_head)  {
			vcf_xmit_head = &(local_chan->send_close);
		}  else  {
			vcf_xmit_tail->next = &(local_chan->send_close);
		}
		vcf_xmit_tail = &(local_chan->send_close);
	}
}


void
vcf_handle_rjoc(long mcmsg_junk, vcf_chand ch)  {
	unsigned  addr = INVERT_PATH((unsigned)ch);
	unsigned rejections;
	vcf_reject  *rj = vcf_get_reject_info(addr);
	unsigned  total_chans;
	int  i, junk;

	NIC_RECV_WORDS((int *)&rejections, &junk);
	NIC_RECV_END_MSG();
	total_chans = rj->roc_unknown;
	for (i = 0;  i < vcf_num_chans;  ++i)  {
		if ((vcf_chan_tbl[i].peer == addr) &&
		    (vcf_chan_tbl[i].status & VCF_CS_SENT_OPEN) &&
		    !(vcf_chan_tbl[i].status & VCF_CS_GOT_RMT_OPEN) &&
		    !(vcf_chan_tbl[i].status & VCF_CS_MAYBE))  {
			++total_chans;
			vcf_chan_tbl[i].status |= VCF_CS_MAYBE;
		}
	}
	if (!(rj->roc_unknown = total_chans - rejections))  {
	  for (i = 0;  i < vcf_num_chans;  ++i)  {
	    if ((vcf_chan_tbl[i].peer == addr) &&
		(vcf_chan_tbl[i].status & VCF_CS_SENT_OPEN) &&
		!(vcf_chan_tbl[i].status & VCF_CS_GOT_RMT_OPEN))  {
	      /* This code lifted from "handle_rjoc()" in
	       *   msgin.c.
	       */
	      if (vcf_chan_tbl[i].status & VCF_CS_GOT_USR_OPEN)  {
		*(vcf_chan_tbl[i].open_sbufp) = VCF_ENOCTE;
		VCF_LOCKIT(vcf_chan_tbl[i].open_sbufp = NULL);
	      }
	      vcf_give_chan(vcf_chan_tbl + i);
	      /*  Need I mark outstanding sends/recvs with
	       *  VCFECHANDEAD?
	       */
	    }
	  }
	}
}


/* Call this when you get a gang mismatch rejection. */
void
vcf_handle_rjg(long mcmsg_junk, vcf_chand  ch)  {
	unsigned  addr;
	unsigned  hi_seq;
	int  i, junk;

#if  VCF_DEBUG
	printf("Got an RJG\n");
#endif
	addr = INVERT_PATH((unsigned)ch);
	NIC_RECV_WORDS((int *) &hi_seq, &junk);
	NIC_RECV_END_MSG();
	for (i = 0;  i < vcf_num_chans;  ++i)  {
		if ((vcf_chan_tbl[i].peer == addr) &&
		    (vcf_sequence_number - vcf_chan_tbl[i].seq_no >=
		     vcf_sequence_number - hi_seq))  {
			if (!(vcf_chan_tbl[i].status & VCF_CS_GOT_RMT_OPEN))  {
				vcf_chan_tbl[i].status &= ~VCF_CS_SENT_OPEN;
			} else if (vcf_chan_tbl[i].status & VCF_CS_SENT_CLOSE)  {
				vcf_chan_tbl[i].status &= ~VCF_CS_SENT_CLOSE;
			} else if (!vcf_chan_tbl[i].recvh)  {
				continue;  /* Wasn't REALLY rejected.  It can happen. */
			}
			vcf_chan_tbl[i].send_reject.next = NULL;
			if (vcf_rejection_tail == NULL)
				vcf_rejection_head = &(vcf_chan_tbl[i].send_reject);
			else
				vcf_rejection_tail->next = &(vcf_chan_tbl[i].send_reject);
			vcf_rejection_tail = &(vcf_chan_tbl[i].send_reject);
			if (!(vcf_chan_tbl[i].status & VCF_CS_REJECTED))  {
				vcf_retry_rate = vcf_retry_counter = 1;
				vcf_chan_tbl[i].status |= VCF_CS_REJECTED;
			}
		}
	}
}


int
vcf_invert_path(unsigned x)  {
	return(INVERT_PATH(x));
}
