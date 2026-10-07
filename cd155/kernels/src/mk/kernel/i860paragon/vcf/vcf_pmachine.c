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
 
#include <i860paragon/vcf/vcf_pmachine.h>
#include <i860paragon/vcf/vcf.h>
#include <i860paragon/vcf/vcf_aux.h>
#include <i860paragon/vcf/vcf_user.h>
#include <i860paragon/vcf/vcf_msgout.h>

int         vcf_rjlist_len, vcf_store_r2;
vcf_xmitinfo  *vcf_rejection_head = NULL, *vcf_rejection_tail = NULL;
vcf_reject  *vcf_rjlist_head = NULL, *vcf_rjlist_tail = NULL;
int         vcf_num_reqs, vcf_num_chans;
vcf_chan  *vcf_free_chan_stack;
unsigned  *vcf_changed_dirbase = NULL;
int  *vcf_dirbase;

vcf_xmitinfo  *vcf_xmit_head = NULL, *vcf_xmit_tail;
vcf_chand     vcf_chan_tbl;

int vcf_retry_rate = 1, vcf_retry_counter = 0;

/* int  vcf_groupid = 0; */
unsigned int  vcf_sequence_number = 1;

typedef void  voidfunc(void);
typedef voidfunc  *pvoidfunc;
#define  VCF_RECV_SWITCH_LEN  256
pvoidfunc  vcf_recv_switch[VCF_RECV_SWITCH_LEN];

static int  startup_successful = 0;

extern void  vcf_atransmit_roc(), vcf_atransmit_rcc();

kern_return_t  vcf_init(mcmsg_task_t *mt, unsigned int nchans,
                        unsigned int nreqs);

static int  dirbase_dummy;

int  vcf_infoblock[16];

#if  !BURST && BUMPERS
bad_config()  {
        vcf_with_bumpers_no_burst_is_illegal = 0;
        please_read_the_comment_for_more_info = 0;
}
/* Note:  VCF is written to work correctly on B step NICs, or on A step NICs
 *   in burst mode.  However, VCF does _NOT_ contain the workarounds needed
 *   to run NIC A's in BIGPKTS mode.  Please either disable VCF by removing
 *   the "+vcf" from your CONFIG, disable bumpers, or turn on burst mode
 *   by adding "+burst".  Sorry for the inconvenience.
 */
#endif  /* !BURST && BUMPERS */


/* Call this when a request with a bad gang ID shows up at your inbound NIC. */
void
vcf_got_rq_badgang(unsigned addr, unsigned seq_no)  {
	vcf_reject  *rj;

	rj = vcf_get_reject_info(addr);
	rj->gang_needed = 1;
	rj->hi_seq = seq_no;
	vcf_enqueue_rj(rj);
}


/* Call this when an ROC shows up at your inbound NIC and your connection table
 *   is full.
 */
void
vcf_got_roc_ctfull(unsigned addr)  {
	vcf_reject  *rj;

	rj = vcf_get_reject_info(addr);
	++rj->ct_full;
	vcf_enqueue_rj(rj);
}


/* Call this when you get an ROC for a connection and the MAYBE bit is set.
 */
void
vcf_got_roc_mayberejected(unsigned addr, vcf_chand c)  {
	vcf_reject  *rj = vcf_get_reject_info(addr);
	int  i;

	if (!(--rj->roc_unknown))  {
		for (i = 0;  i < vcf_num_chans;  ++i)  {
			if ((vcf_chan_tbl[i].peer == addr) && (vcf_chan_tbl+i != c) &&
			    (vcf_chan_tbl[i].status & VCF_CS_SENT_OPEN) &&
			    (vcf_chan_tbl[i].status & VCF_CS_MAYBE))  {
				/* This code lifted from "handle_rjoc()" in
				 *   msgin.c.
				 */
				if (vcf_chan_tbl[i].status & VCF_CS_GOT_USR_OPEN)  {
					*(vcf_chan_tbl[i].open_sbufp) = VCF_ENOCTE;
				}
				vcf_chan_tbl[i].status = VCF_CS_UNUSED;
				vcf_give_chan(vcf_chan_tbl + i);
				/*  Need I mark outstanding sends/recvs with
				 *  VCFECHANDEAD?
				 */
			}
		}
	}
}


static int
vcf_init_tables(unsigned int nchans, unsigned int nreqs,
                unsigned int map_size)  {
	int index;
	vcf_req  *req_pool;
	static char  *prev_buf = NULL;
	static int  prev_size = 0;

	if (prev_buf != NULL)  {
		vm_deallocate(kernel_map, prev_buf, prev_size);
		prev_buf = NULL;
	}
	if (vm_allocate(kernel_map, &req_pool, nreqs * sizeof(vcf_req),
	                TRUE) != KERN_SUCCESS)
		return(VCF_ENOMEM);
	vcf_infoblock[5] = (int)req_pool;
	prev_buf = (char *)req_pool;
	prev_size = nreqs * sizeof(vcf_req);
	if (vm_wire(1, kernel_map, req_pool, nreqs * sizeof(vcf_req),
							1) != KERN_SUCCESS)  {
		vm_deallocate(kernel_map, prev_buf, prev_size);
		prev_buf = NULL;
		return(VCF_ENOMEM);
	}

	/*
	 * printf("Chan table address: 0x%x (%d)\n", vcf_chan_tbl, vcf_chan_tbl);
	 * printf("Info address: 0x%x\n", vcf_info);
	 * vcf_info[0] = vcf_info[1] = vcf_info[2] = vcf_info[3] = 0;
	 * vcf_info[5] = (int)(mcmsg_user_post_page[0]);
	 * vcf_info[6] = (int)(mcmsg_user_post_page_out);
	 */
	vcf_infoblock[4] = (int)vcf_chan_tbl;
	for (index = 0;  index < nchans;  index++)  {
		vcf_chan_tbl[index].status = VCF_CS_UNUSED;
		vcf_chan_tbl[index].rd_len = 1;
		vcf_chan_tbl[index].send_pool = NULL;
		vcf_chan_tbl[index].recv_pool = NULL;
		vcf_chan_tbl[index].sendh = NULL;
		vcf_chan_tbl[index].recvh = NULL;
		vcf_chan_tbl[index].rda_in_pbuf = NULL;
		vcf_chan_tbl[index].rda_out_pbuf = NULL;
		vcf_chan_tbl[index].seq_recvd = 0;
		vcf_chan_tbl[index].send_rda.func = vcf_transmit_rda;
		vcf_chan_tbl[index].send_rd.func = vcf_transmit_rd;
		vcf_chan_tbl[index].send_open.func = vcf_atransmit_roc;
		vcf_chan_tbl[index].send_close.func = vcf_atransmit_rcc;
		vcf_chan_tbl[index].open_sbufp = NULL;
		vcf_chan_tbl[index].close_sbufp = NULL;
		if (index < nchans - 1)  {
			vcf_chan_tbl[index].hash_next = &(vcf_chan_tbl[index+1]);
		}
		vcf_chan_tbl[index].hash_bucket = NULL;
	}
	vcf_chan_tbl[nchans - 1].send_open.next = NULL;
	vcf_chan_tbl[nchans - 1].hash_next = NULL;

	for (index = 0;  index < nreqs - 1;  index++)
		req_pool[index].next = &req_pool[index + 1];
	req_pool[nreqs - 1].next = NULL;

	vcf_free_chan_stack = &(vcf_chan_tbl[0]);
	vcf_free_reqs = req_pool;

	return(0);
}


kern_return_t  syscall_vcf_init(unsigned int nchans, unsigned int nreqs)  {
	task_t  my_task;
	mcmsg_task_t  *user_task;
	kern_return_t  ret;

#if  VCF_DEBUG
	printf("Init.\n");
#endif
	my_task = current_task();
	user_task = my_task->mcmsg_task;
	ret = vcf_init(user_task, nchans, nreqs);
	return(ret);
}


kern_return_t
vcf_init(mcmsg_task_t *mt, unsigned int nchans, unsigned int nreqs)  {
	unsigned int  *new_ptable, *new_dirbase, *old_dirbase;
	int  index, *temp, *myd;
	static int  first_time = 0, physaddr;
	static char  *prev_buf = NULL;
	static int  prev_size = 0;

	if (vcf_dirbase != NULL)  {
		return(VCF_EREINIT);
	}

	if (!startup_successful)  {
		return(VCF_ENOSTART);
	}

	vcf_task = mt;

	if (prev_buf != NULL)  {
		vm_deallocate(kernel_map, prev_buf, prev_size);
		prev_buf = NULL;
	}
	if (vm_allocate(kernel_map, &vcf_chan_tbl, nchans * sizeof(vcf_chan),
	                TRUE) != KERN_SUCCESS)
		return(VCF_ENOMEM);
	prev_buf = (char *)vcf_chan_tbl;
	prev_size = nchans * sizeof(vcf_chan);
	if (vm_wire(1, kernel_map, vcf_chan_tbl, nchans * sizeof(vcf_chan),
							1) != KERN_SUCCESS)  {
		vm_deallocate(kernel_map, prev_buf, prev_size);
		prev_buf = NULL;
		return(VCF_ENOMEM);
	}

	vcf_map_size = (nchans - 1) | 1;

	if (vcf_init_tables(nchans, nreqs, vcf_map_size))  {
		vm_deallocate(kernel_map, prev_buf, prev_size);
		prev_buf = NULL;
		return(VCF_ENOMEM);
	}
	vcf_num_chans = nchans;
	vcf_num_reqs = nreqs;
	vcf_dirbase = (int *)mt->dirbase;
	return(1);
}


void
vcf_enqueue_rj(vcf_reject *rj)  {
	int  already_on_xmit_q = 1;

	/* If it's already enqueued, then don't do it again! */
	if ((vcf_rjlist_tail != rj) && (rj->next == NULL))  {
		/* OK, you DO have to enqueue it. */
		if (vcf_rjlist_head == NULL)  {
			already_on_xmit_q = 0;
			vcf_rjlist_head = rj;
		} else
			vcf_rjlist_tail->next = rj;
		vcf_rjlist_tail = rj;
		/* Put in on the main outbound work queue. */
		if (!already_on_xmit_q)  {
			vcf_rejection_out.next = NULL;
			if (!vcf_xmit_head)
				vcf_xmit_head = &vcf_rejection_out;
			else
				vcf_xmit_tail->next = &vcf_rejection_out;
			vcf_xmit_tail = &vcf_rejection_out;
		}
	}
}


static void
vcf_swap(vcf_reject *left, vcf_reject *right)  {
	vcf_reject middle;

	middle = *left;
	*left = *right;
	*right = middle;
}


static void
vcf_bsort(vcf_reject *rjlist, unsigned int size)  {
	int index1, index2;

	for (index1 = size - 1;  index1 > 0;  index1--)
		for (index2 = 0;  index2 < index1;  index2++)
			if (rjlist[index2].peer > rjlist[index2 + 1].peer)
				vcf_swap(&rjlist[index2], &rjlist[index2 + 1]);
}


void
vcf_init_rjlist(int num_nodes)  {
	if (num_nodes > NUM_NODES)  {
		printf("*ERROR* VCF cannot set up rjlist for >%d nodes.\n", NUM_NODES);
		num_nodes = NUM_NODES;
	} else
		startup_successful = 1;
	vcf_rjlist_len = num_nodes;
	while (num_nodes--)  {
		vcf_rjlist[num_nodes].peer = calculate_route(num_nodes);
		vcf_rjlist[num_nodes].gang_needed = 0;
		vcf_rjlist[num_nodes].ct_full = 0;
		vcf_rjlist[num_nodes].roc_unknown = 0;
		vcf_rjlist[num_nodes].next = NULL;
	}
	vcf_bsort(vcf_rjlist, vcf_rjlist_len);
	vcf_rejection_head = NULL;
}


/* Clean up for when VCF exits.
 *   Basically all you have to do is empty the rejection list.
 */
void  vcf_exit_cleanup(void)  {
	vcf_reject  *rjtop;
	int  i;

	while (vcf_rjlist_head != NULL)  {
		rjtop = vcf_rjlist_head->next;
		vcf_rjlist_head->next = NULL;
		vcf_rjlist_head = rjtop;
	}
	vcf_rjlist_tail = NULL;
}

	
/*
 * Shift the list of "rejected channels" over to the list of "channels with
 *   RDs to send out".
 */
void
vcf_retry_failures(void)  {
	vcf_chand  ch;
	vcf_xmitinfo  *nextout;

	/* Make it a longer period of time before the next retry. */
	vcf_retry_rate = (vcf_retry_rate << 1) | 1;
	vcf_retry_counter = vcf_retry_rate;
#if  VCF_DEBUG
	printf("Retrying failures...time = %d\n", vcf_retry_counter);
#endif
	/* We've been swapped out!  Don't bother to retry. */
	if (vcf_dirbase == NULL)
		return;
	/* Shuffle all the rejections to an xmit queue. */
	while (vcf_rejection_head != NULL)  {
		ch = MAKE_CHAND(vcf_rejection_head);
		vcf_rejection_head = ch->send_reject.next;
		if (!(ch->status & VCF_CS_REJECTED))
			continue;
#if  VCF_DEBUG
		printf("  Ch: 0x%8x...status=0x%8x\n", ch, ch->status);
#endif
		if (!(ch->status & VCF_CS_SENT_OPEN))  {
			ch->status |= VCF_CS_SENT_OPEN;
			nextout = &(ch->send_open);
#if  VCF_DEBUG
			printf("  Qing an ROC.\n");
#endif
		} else if (ch->recvh)  {
#if  VCF_DEBUG
			printf("  Qing an RD.\n");
#endif
			nextout = &(ch->send_rd);
		} else  {
#if  VCF_DEBUG
			printf("  Qing an RCC.\n");
#endif
			nextout = &(ch->send_close);
		}
		nextout->next = NULL;
		if (vcf_xmit_head == NULL)
			vcf_xmit_head = nextout;
		else
			vcf_xmit_tail->next = nextout;
		vcf_xmit_tail = nextout;
	}
	vcf_rejection_tail = NULL;
#if  VCF_DEBUG
	printf("  Done.\n");
#endif
}


int  vcf_temp;
vcf_foo(x,y)  {
	printf("FOO: %8x %8x   (%d %d)\n",x,y,x,y);
}
