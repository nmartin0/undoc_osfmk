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
 
#ifndef _VCF_H_

#define _VCF_H_
#include <i860paragon/mcmsg/mcmsg_hw.h>

#define  VCF_DEBUG_PRINTOUTS  0
#define  SUPERLOCK  1

typedef volatile int vcf_sbuf;

typedef short int boolean;

#ifndef  TRUE
#define TRUE   1
#define FALSE  0
#endif

#ifndef  NULL
#define  NULL  ((void *) 0)
#endif


#if  SUPERLOCK
#define  VCF_LOCKIT(cmd)  do { asm(" lock"); cmd; asm(" unlock"); } while (0)
#else
#define  VCF_LOCKIT(cmd)  cmd
#endif

typedef struct vcf_xmitinfo  {
	struct vcf_xmitinfo  *next;
	void  (*func)(struct vcf_xmitinfo *buf);
} vcf_xmitinfo;

typedef struct vcf_req {
	struct vcf_req *next;
	unsigned int  len;
	vcf_sbuf       *sbufp;
	int           *buf;
} vcf_req;
/*
 * The vcf_chan structure size must be a power of 2 to simplify
 *   calculations which prove a vcf_chanp is valid.  It is currently
 *   exactly 32 words (128 bytes).
 */
typedef struct vcf_chan {
  int status;                                    /* Channel status */
  int peer;                          /* Path to communication peer */
  unsigned seq_no;  /* The seq number of the request outstanding here. */
  unsigned rd_len;   /* Number of bytes requested for transmission */
  struct vcf_chan *peer_cte;  /* CTE address on communication peer  */
  vcf_req *send_pool, *recv_pool;                  /* Request pools */
  vcf_req *sendh, *sendt;                     /* Send request queue */
  vcf_req *recvh, *recvt;                  /* Receive request queue */
	void  *rda_in_pbuf;
	void  *rda_out_pbuf;
	unsigned  seq_recvd;   /* Seq no of the rd that has been recvd. */
	vcf_xmitinfo  send_rda;
	vcf_xmitinfo  send_rd;
	vcf_xmitinfo  send_open;
	vcf_xmitinfo  send_close;
	vcf_sbuf *open_sbufp;    /* Status of open operation on channel */
  vcf_sbuf *close_sbufp;    /* Status of close operation on channel */
  int id;                                            /* Channel id */
	struct vcf_chan  *hash_next, *hash_prev;
	struct vcf_chan  *hash_bucket;
	vcf_xmitinfo  send_reject;
	int  dummy[2];  /* Round out to 128 bytes. */
} *vcf_chand, vcf_chan;

#define  MAKE_CHAND(xi)  ((vcf_chand)((int)(xi) & ~(sizeof(vcf_chan) - 1)))

#define  VCF_CS_UNUSED             0

#define  VCF_CS_GOT_RMT_CLOSE  0x001
#define  VCF_CS_SENT_CLOSE     0x002
#define  VCF_CS_GOT_USR_CLOSE  0x004

#define  VCF_CS_GOT_RMT_OPEN   0x008

#define  VCF_CS_SENT_OPEN      0x020
#define  VCF_CS_GOT_USR_OPEN   0x040

#define  VCF_CS_REJECTED       0x080
#define  VCF_CS_READY          0x100
#define  VCF_CS_CHANDEAD       0x200
#define  VCF_CS_MAYBE          0x400

typedef struct vcf_usr_comm {
	volatile int status;
	union {
		struct {
			volatile int      len;
			/* volatile */ vcf_sbuf *sbufp;
			volatile int      *buf;
			volatile vcf_chand chan;
		} send_req, recv_req;
		struct {
			volatile unsigned int node;
			volatile unsigned int in_reqs;
			volatile unsigned int out_reqs;
			volatile unsigned int id;
			/* volatile */ vcf_sbuf *sbufp;
		} open_req;
		volatile vcf_chand open_resp;
		struct {
			volatile vcf_chand chan;
			/* volatile */ vcf_sbuf *sbufp;
		} close_req;
	} info;
} vcf_usr_comm;

#define VCF_WIN_UNUSED           0
#define VCF_WIN_PREPARING_SEND  -4
#define VCF_WIN_PREPARING_RECV  -8
#define VCF_WIN_PREPARING_INIT -12
#define VCF_WIN_OPEN             4
#define VCF_WIN_CLOSE            8
#define VCF_WIN_SEND            12
#define VCF_WIN_RECV            16

#define VCF_MPNEEDED(x) ((x) > 0)

typedef struct vcf_sys_comm  {
        int status;
        union  {
                struct  {
                        int           groupid;
                        vcf_usr_comm  *comm;
                        vcf_chand      chan_table;
                        unsigned      nreqs, nchans;
                        vcf_xmitinfo  *rejection_head, *rejection_tail;
                        vcf_chand      xmit_rd_head, xmit_rd_tail;
                        vcf_chand      xmit_rda_head, xmit_rda_tail;
                        vcf_chand      xmit_ctrl_head, xmit_ctrl_tail;
                        vcf_chand      free_chan_stack;
                        vcf_req        *free_reqs;
                      } bindings;
                boolean bound;
                struct  {
                        int       memstable;
                        vcf_chand  chanlist;
                      } pageswap_req;
              } info;
      } vcf_sys_comm;

#define  VCF_SYSWIN_UNUSED     0
#define  VCF_SYSWIN_BIND       4
#define  VCF_SYSWIN_UNBIND     8
#define  VCF_SYSWIN_PAGESWAP  12

#define  VCF_EINVAL     -1  /* Invalid argument. */
#define  VCF_ENOBUF     -2  /* No buffers available. */
#define  VCF_ECHANDEAD  -3  /* Channel dead. */
#define  VCF_ENOCTE     -4  /* No CTEs left. */
#define  VCF_ENOMEM     -5  /* No memory available. */
#define  VCF_EMEMFAULT  -6  /* Catastrophic memory fault. */
#define  VCF_EREINIT    -7  /* Init called when VCF is already running. */
#define  VCF_ENOSTART   -8  /* Couldn't start up VCF for some reason. */
                            /* Probably too many nodes.  See NUM_NODES.*/

#define  VCF_PAGESIZE     4096
#define  VCF_PAGEMASK(x)  (((int)(x) & ~(VCF_PAGESIZE - 1)))

#define NUM_NODES 512

#endif /* !_VCF_H_ */
