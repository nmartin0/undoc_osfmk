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
 
#ifndef _PMACHINE_H_
#define _PMACHINE_H_

#include <i860paragon/vcf/vcf.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>

/* These are dummy functions to check the validity of an address. */

/* The LTU can only transfer on 32 byte boundaries. */
#define  LTU_MASK  (32 - 1)


/* dq a message and shift it to the empty list. */
#define  dq(empty, head) \
do { \
       vcf_req  *tmp = (head); \
	 (head) = tmp->next; \
	   tmp->next = (empty); \
	     (empty) = tmp; \
	     } while (0)


/*
 * nq a message to the list given.  Note that this is not quite dq's
 *   inverse; the program must remove the message from the empty list
 *   before filling it and nq'ing it.
 */
#define  nq(head, tail, msg) \
do { \
       msg->next = NULL; \
	 if (head == NULL) \
	   head = msg; \
	 else \
	   tail->next = msg; \
	     tail = msg; \
	     } while (0)


/*
 * Dummy values for header words of messages.  These will definitely
 *   change in the final implementation, probably into pointers to sections
 *   of code that deal with the message type in question.
 */
/*
 * Message types:
 *   RD: Request for data.  Sent from receiver to sender.
 *  RDA: RD Acknowledge.  Contains data.  Sent from sender to receiver.
 * RDAC: RDA Continue.  Contains a packet of data but does not complete
 *       transaction.  It will be followed by another RDAC or an RDA.
 * RJDP: RD Reject Channel dead.  Don't do any retries.
 * RJDG: RD Reject Group mismatch.  Retry later.
 *
 * Message formats:
 *   RD: -RD  -Receiver Channel-Gang-Path-Seqno-Length-End-
 *  ROC: -ROC -Sender Channel  -Gang-Path-Seqno-Id-End-
 *  RCC: -RCC -Receiver Channel-Gang-Path-Seqno-End-
 *  RDA: -RDA -Receiver Channel-Length-Total length-Data-End-
 * RDAC: -RDAC-Receiver Channel-Packet Length--Data-End-
 *  RJG: -RJG -Peer-Hi Seq-End-
 * RJOC: -RJOC-Peer-Count-End-
 */
#if 0
#define  VCF_HWORD_RD    80
#define  VCF_HWORD_RDA   81
#define  VCF_HWORD_RDAC  82
#define  VCF_HWORD_ROC   83
#define  VCF_HWORD_RCC   84
#define  VCF_HWORD_RJG   85
#define  VCF_HWORD_RJOC  86
#endif
/* Maximum outstanding rejections allowed at any given moment. */
typedef struct vcf_reject  {
  unsigned  peer;
  int  gang_needed;
  /* Rejections that I am waiting to send.  The lo_seq and hi_seq are
   *   for gang mismatches; the ct_full is for channel table full errors.
   */
  unsigned  hi_seq, ct_full;
  /* Rejections that I have received from this node.  This is the number
   *  of roc's that may or may not have succeeded.  Every time I receive
   *  a rejection I subtract this, every time I send an ROC to an already
   *  rejected node I increment it, every time I get an acceptance I
   *  decrement this.  If I decrement/subtract it to zero I mark all the
   *  outstanding ROCs as rejected.
   */
  unsigned  roc_unknown;
  /* The list of peers that need rejections sent to them. */
  struct vcf_reject  *next;
} vcf_reject;


/*
 * Variables in constant memory - that is, that don't change when a process
 *   gets swapped out.
 */
extern int  vcf_retry_rate, vcf_retry_counter;
/* extern int  vcf_groupid; */
extern int  *vcf_dirbase;

/*
 * Variables in process memory.  They should go in & out with each
 *   context swap.
 */
extern vcf_xmitinfo  *vcf_xmit_head, *vcf_xmit_tail;
extern vcf_chan  *vcf_free_chan_stack;
extern vcf_xmitinfo  *vcf_rejection_head, *vcf_rejection_tail;
extern vcf_chand  vcf_chan_tbl;
extern int vcf_num_chans, vcf_num_reqs;
extern vcf_reject  vcf_rjlist[];
extern int  vcf_rjlist_len;
extern unsigned  vcf_sequence_number;
extern vcf_reject  *vcf_rjlist_head,  *vcf_rjlist_tail;

extern volatile vcf_sys_comm *vcf_syscomm;
extern volatile vcf_usr_comm *vcf_usrcomm;

extern void vcf_start_pmachine(int *shm_start, int sem_id);

#ifdef TEST
extern void pmachine(void);
#endif

extern void  vcf_got_rq_badgang(unsigned addr, unsigned seq_no);
extern void  vcf_got_rq_badgang_swapout(unsigned addr);
extern void  vcf_got_roc_ctfull(unsigned addr);
extern void  vcf_got_rj_badgang(unsigned addr, unsigned lo_seq,
                                unsigned hi_seq);
extern void  vcf_got_rj_ctfull(unsigned addr, unsigned rejections);
extern void  vcf_got_roc_mayberejected(unsigned addr, vcf_chand c);
extern void  vcf_sending_roc(unsigned addr, vcf_chand c);
extern vcf_reject  *vcf_get_reject_info(unsigned addr);
extern void  vcf_enqueue_rj(vcf_reject *rj);

#endif /*  !_PMACHINE_H_  */
