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
#include <i860paragon/vcf/vcf_msgout.h>
#include <i860paragon/vcf/vcf_pmachine.h>
#include <i860paragon/vcf/vcf_user.h>
#include <i860paragon/vcf/vcf_aux.h>
#include <i860paragon/vcf/vcf_cnic.h>

#ifdef  NO_ASSEMBLY
static void  finish_rdac(void), finish_rda(void);
#endif
static int       *g_pbuf;
static unsigned  g_len;
static vcf_chand  g_ch;
static vcf_req    *g_msg;
static int  *g_lastpbuf = NULL;
extern void  vcf_atransmit_rejection();

/*
 *  This variable flags that the pmachine has an outstanding, incomplete 
 *  outgoing RDA/RDAC awaiting completion of an LTU operation.  When non-NULL,
 *  it points to a function that should be called when the LTU completes.
 *  This function will complete the transmission of the RDA/RDAC.  While
 *  this variable is non-NULL, no outgoing transmissions can occur.
 */
void (*out_ltu_active)(void) = NULL;
vcf_xmitinfo  vcf_rejection_out = {NULL, vcf_atransmit_rejection};


#ifdef  NO_ASSEMBLY
/*
 * Send an RDA or RDAC.  If it's an RDAC, dequeue the
 * channel from the xmit_rda queue.
 */
void
transmit_rda(vcf_chand ch)  {
	int  destnode = ch->peer;
	vcf_chand  destch = ch->peer_cte;
	vcf_req  *msg = ch->sendh;
	int  *vbuf = msg->buf;
	int *pbuf;
	unsigned int packetlen, len = msg->len;

	if (g_lastpbuf)  {
		pbuf = g_lastpbuf;
		g_lastpbuf = NULL;
	} else
		pbuf = (int *) my_validate((unsigned long) vbuf, (unsigned long) vcf_task->dirbase);

	/*
	 * Deal with the packetization.
	 * Super-easy algorithm: Never do a send that crosses
	 * packet boundaries.
	 */
	NIC_SEND_START_MSG(destnode);
	if (pbuf == (int *) NULL)  {
		*msg->sbufp = VCF_ENOMEM;
		/* Send zeroes just to keep the damn protocol happy. */
		if (!PACKET_BOUNDARY_CROSSED(pbuf, len))  {
			packetlen = len;
			NIC_SEND_WORDS(VCF_HWORD_RDA, (int)destch);
		} else  {	/* Only an RDAC. */
			packetlen = BYTES_TO_PACKET_BOUNDARY(pbuf);
			NIC_SEND_WORDS(VCF_HWORD_RDAC, (int)destch);
		}
		NIC_SEND_WORDS(packetlen, ch->rd_len);
		while (packetlen)  {
			NIC_SEND_WORDS(0,0);
			packetlen -= 8;
		}
	} else  {
		if (PACKET_BOUNDARY_CROSSED(pbuf, len))  {
			/*  If message crosses a packet boundary, send bytes 
			 *  up to boundary.  This is an RDAC.
			 */
			packetlen = BYTES_TO_PACKET_BOUNDARY(pbuf);
			len -= packetlen;
			NIC_SEND_WORDS(VCF_HWORD_RDAC, (int)destch);
			NIC_SEND_WORDS(packetlen, 0);
			vbuf = (int *)((int)vbuf + packetlen);

			/*  Send bytes to cache-line boundary.  */
			while ((((int)pbuf & LTU_MASK) != 0) &&
			       (packetlen > 0))  {
				NIC_SEND_WORDS(pbuf[0], pbuf[1]);
				pbuf += 2;
				packetlen -= 8;
			}

			if (packetlen)  {
				/*  Remaining bytes in packet start on cache-
				 *  line boundary.  Send using LTU.
				 */
				LTU_SEND_BLOCK(pbuf, packetlen);
				if ((int)vbuf & (VCF_PAGESIZE - 1))  {
					/* Next RDA[C] will be on same page. */
					g_lastpbuf = (int *)((int)pbuf + packetlen);
				} else  {
					/* Next RDA[C] will be on another page.  Compute the pbuf
					 *   now while the LTU is running.
					 */
					g_lastpbuf = (int *) my_validate((unsigned long)vbuf,
					                         (unsigned long)vcf_task->dirbase);
				}
				out_ltu_active = finish_rdac;
				msg->buf = vbuf;
				msg->len = len;
				return;
			} else  {
				/*
				 * An RDAC will always end on a cache line
				 *   boundary, so no need to do individual
				 *   sends after the LTU transfer.
				 */
				NIC_SEND_END_MSG();
				if ((int) vbuf & (VCF_PAGESIZE - 1))  {
					/* Next RDA[C] will be on same page. */
					g_lastpbuf = (int *)((int)pbuf + packetlen);
				}
				msg->buf = vbuf;
				msg->len = len;
				return;
			}
		} else  {
			/*  Message doesn't cross packet boundary.  Send an
			 *  RDA.
			 */
			NIC_SEND_WORDS(VCF_HWORD_RDA, (int)destch);
			NIC_SEND_WORDS(len, ch->rd_len);

			/*  Send bytes to cache-line boundary.  */
			while ((((int)pbuf & LTU_MASK) != 0) && (len != 0))  {
				NIC_SEND_WORDS(pbuf[0], pbuf[1]);
				pbuf += 2;
				len -= 8;
			}

			/*  Send whatever is possible using LTU.  Initiate
			 *  LTU operation, set up completion of packet
			 *  transmission, and then return.
			 */
			if (len & ~LTU_MASK)  {
				LTU_SEND_BLOCK(pbuf, len & ~LTU_MASK);
				pbuf += (len & ~LTU_MASK) / sizeof(*pbuf);
				len &= LTU_MASK;
				out_ltu_active = finish_rda;
				g_pbuf = pbuf;
				g_len = len;
				g_ch = ch;
				g_msg = msg;
				return;
			}

			while (len & LTU_MASK)  {
				NIC_SEND_WORDS(pbuf[0], pbuf[1]);
				pbuf += 2;
				len -= 8;
			}

			/*  The RDA contained no cache-line aligned data, so
			 *  all of it has been sent.
			 */
		}
	}
	NIC_SEND_END_MSG();
	*(msg->sbufp) = ch->rd_len;
	ch->rd_len = 0;
	dq(ch->send_pool, ch->sendh);
	xmit_rda_head = xmit_rda_head->send_next;
	if ((ch->status & (VCF_CS_GOT_RMT_CLOSE|VCF_CS_GOT_USR_CLOSE)) &&
	    (ch->sendh == NULL) && (ch->recvh == NULL))  {
		if (!xmit_ctrl_head)  {
			xmit_ctrl_head = ch;
		}  else  {
			xmit_ctrl_tail->send_next = ch;
		}
		xmit_ctrl_tail = ch;
	}
}


static void
finish_rdac(void)  {
	/*  RDAs end on packet boundaries.  There is no non-cache-line-aligned
	 *  trailer left to send.
	 */
	NIC_SEND_END_MSG();
	out_ltu_active = NULL;
}


static void
finish_rda(void)  {
	int       *pbuf = g_pbuf;
	unsigned  len  = g_len;
	vcf_chand  ch   = g_ch;
	vcf_req    *msg = g_msg;

	/*  Send the non-cache-line-aligned remainder of the packet.  */
	while (len)  {
		NIC_SEND_WORDS(pbuf[0], pbuf[1]);
		pbuf += 2;
		len -= 8;
	}
	NIC_SEND_END_MSG();
	out_ltu_active = NULL;
	*(msg->sbufp) = ch->rd_len;
	ch->rd_len = 0;
	dq(ch->send_pool, ch->sendh);
	xmit_rda_head = xmit_rda_head->send_next;
	if ((ch->status & (VCF_CS_GOT_RMT_CLOSE|VCF_CS_GOT_USR_CLOSE)) &&
	    (ch->sendh == NULL) && (ch->recvh == NULL))  {
		if (!xmit_ctrl_head)  {
			xmit_ctrl_head = ch;
		}  else  {
			xmit_ctrl_tail->send_next = ch;
		}
		xmit_ctrl_tail = ch;
	}
}



/*
 * Send out an rd, then dequeue the channel.  Nothing tricky here.
 */
void
transmit_rd(vcf_chand ch)  {
	NIC_SEND_START_MSG(ch->peer);
	NIC_SEND_WORDS(VCF_HWORD_RD, (int)ch->peer_cte);
	NIC_SEND_WORDS(0 /* groupid */, ch->peer);
	NIC_SEND_WORDS(++sequence_number, ch->recvh->len);
	NIC_SEND_END_MSG();

	ch->seq_no = sequence_number;
	xmit_rd_head = xmit_rd_head->recv_next;
}

#endif  /* NO_ASSEMBLY */


void
vcf_transmit_roc(vcf_xmitinfo *xi)  {
	vcf_chand  chan = MAKE_CHAND(xi);
	unsigned int peer = chan->peer;
	unsigned int id = chan->id;

#if  VCF_DEBUG
	printf("Sending ROC.\n");
#endif
	NIC_SEND_START_MSG(peer);
	NIC_SEND_WORDS(VCF_HWORD_ROC, (int) chan);
	NIC_SEND_WORDS(0 /* vcf_groupid */, peer);
	NIC_SEND_WORDS(++vcf_sequence_number, id);
	NIC_SEND_END_MSG();
	chan->seq_no = vcf_sequence_number;
}


void
vcf_transmit_rcc(vcf_xmitinfo *xi)  {
	vcf_chand  chan = MAKE_CHAND(xi);
	unsigned int peer = chan->peer;

	NIC_SEND_START_MSG(peer);
	NIC_SEND_WORDS(VCF_HWORD_RCC, (int)chan->peer_cte);
	NIC_SEND_WORDS(0 /* vcf_groupid */, peer);
	NIC_SEND_WORDS(++vcf_sequence_number, 0);
	NIC_SEND_END_MSG();

	chan->seq_no = vcf_sequence_number;
	chan->status |= VCF_CS_SENT_CLOSE;

	/*  If this channel already got a remote close and a user close,
	 *  then it has been successfully closed.  Set the status field,
	 *  free the channel, and return.
	 */
	if ((chan->status & VCF_CS_GOT_RMT_CLOSE) &&
	    (chan->status & VCF_CS_GOT_USR_CLOSE))    {
 		VCF_LOCKIT(*chan->close_sbufp = 1);
		vcf_give_chan(chan);
		return;
	}
}


/*
 * Send an RJ from the queue.  It is also necessary to dequeue the
 *   rejection to prevent it from getting set off again.
 */
void
vcf_transmit_rejection(vcf_xmitinfo *xi)  {
	vcf_reject  *rj = vcf_rjlist_head;

#if  VCF_DEBUG
	printf("Sending a rejection...0x%x\n", rj);
#endif
	NIC_SEND_START_MSG(rj->peer);
	if (rj->gang_needed)  {
#if  VCF_DEBUG
		printf("  Sending gang...\n");
#endif
		rj->gang_needed = 0;
		/* Send a gang. */
		NIC_SEND_WORDS(VCF_HWORD_RJG, rj->peer);
		NIC_SEND_WORDS(rj->hi_seq, 0);
		NIC_SEND_END_MSG();
		if (rj->ct_full == 0)  {
			if ((vcf_rjlist_head = rj->next) == NULL)  {
				vcf_rjlist_tail = NULL;
			}
			rj->next = NULL;
		}
	} else  {
		/* Send a CT full. */
#if  VCF_DEBUG
		printf("  Sending a CT full\n");
#endif
		NIC_SEND_WORDS(VCF_HWORD_RJOC, rj->peer);
		NIC_SEND_WORDS(rj->ct_full, 0);
		NIC_SEND_END_MSG();
		rj->ct_full = 0;
		if ((vcf_rjlist_head = rj->next) == NULL)
			vcf_rjlist_tail = NULL;
		rj->next = NULL;
	}
	vcf_rejection_out.next = NULL;
	if (vcf_rjlist_head != NULL)  {
#if  VCF_DEBUG
		printf("  ReQ rejection");
#endif
		if (!vcf_xmit_head)
			vcf_xmit_head = &vcf_rejection_out;
		else
			vcf_xmit_tail->next = &vcf_rejection_out;
		vcf_xmit_tail = &vcf_rejection_out;
	} else  {
#if  VCF_DEBUG
		printf("  No more rjs left\n");
#endif
	}
}
