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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_ipc.h,v 1.12 1994/11/18 20:44:22 mtm Exp $
 */

/*
 * mcmsg_ipc.h
 *
 * Data structures for Mach IPC
 */

#define MAXBV		3
#define INVALID_NODE	65535

typedef
struct ipcreq {
	struct ipcreq	*link;
	unsigned long	total;
	unsigned short	node;
	unsigned short	count;
	struct netvec	bv[MAXBV];
} ipcreq_t;

#define TRACE_IPC	PARAGON860

int		ipcreq_receiving;
int		ipcreq_recv_count;	/* Outstanding IPC req's */
int		ipcreq_send_count;

#define IPC_COPY_SEND	1	/* XXX REMOVE WHEN NORMA IS FIXED */
#define IPC_CSUM	0	/* BEWARE - DOESN'T WORK WITH NORMA14 */
#define IPC_DEBUG	0
#define CKDATA		0	/* extra debug -- needs IPC_CSUM */

#if	IPC_DEBUG
#define	IPC_TRACE_DEBUG(a,b,c,d,e,f)	mcmsg_trace_debug(a,b,c,d,e,f)
#else	IPC_DEBUG
#define	IPC_TRACE_DEBUG(a,b,c,d,e,f)
#endif	IPC_DEBUG

#if	CKDATA
#define	IPC_TRACE_CKDATA(a,b,c,d,e,f)	mcmsg_trace_debug(a,b,c,d,e,f)
#else	CKDATA
#define	IPC_TRACE_CKDATA(a,b,c,d,e,f)
#endif	CKDATA

#define	IS_LOCKED_ONTO(n)	((n) != -1)
#define	FROM_NODE(n)		((n) >> 16)
#define	FIRST_PACKET(h)		(((h) >> 16) == 0)

/*
 *	packetized NORMA ipc support
 */
ipcreq_t	ipcreq_send_request;
ipcreq_t	ipcreq_recv_request;
long	ipcreq_packet_size;	/* 56 lines * 32 bytes each */
long	ipcreq_recv_node;
long	ipcreq_length_remaining;
long	ipcreq_send_route;
long	ipcreq_send_node;
long	ipcreq_sending;
int	ipcreq_recv_posted;
#define IPC_PKT_SIZE	ipcreq_packet_size

/*
 *	3-trip protocol support (RTS, CTS, SEND).
 */
#ifdef DYNAMIC_NUM_NODES
int max_rts_reqs;     /* set this in model_dep.c */
#define MAX_RTS_REQS max_rts_reqs      /* This will do the auto replace :-) */
struct ipc_sender_item *ipc_rts_cts_table;
#else  DYNAMIC_NUM_NODES
#define MAX_RTS_REQS 576
struct ipc_sender_item ipc_rts_cts_table[MAX_RTS_REQS]; /* XXX */
#endif DYNAMIC_NUM_NODES

struct ipc_sender_item {
	unsigned short		ipc_node;	/* remote node */
	unsigned short		ipc_length;	/* length of message */
	struct ipc_sender_item	*ipc_next;	/* next node */
};
struct ipc_sender_item *ipc_rts_head;		/* head of rts queue */
struct ipc_sender_item *ipc_rts_tail;		/* tail of rts queue */
struct ipc_sender_item *ipc_rts_free;		/* free list of queue items */

int	ipcreq_grants_given;			/* grants outstanding */
int	ipcreq_recently_granted;		/* most-recently granted */
int	ipc_rts_count, ipc_rts_count_max;	/* count and max of the queue */
int	ipc_rts_free_count;			/* count of free list */
int	ipcreq_rts_cts;				/* use 3-trip? */
unsigned long	ipcreq_ipcq_hits;		/* queue empty, recv posted */

/*
 *	some alignment and transfer statistics
 */
unsigned long	ipcreq_best_alignment_hits;
unsigned long	ipcreq_poor_alignment_hits;
unsigned long	ipcreq_cross_alignment_hits;
unsigned long	ipcreq_dump_alignment_hits;
unsigned long	ipcreq_fifo_out_aligned_hits;
unsigned long	ipcreq_fifo_out_aligned_misses;
unsigned long	ipcreq_fifo_out_eod_aligned_hits;
unsigned long	ipcreq_fifo_out_eod_aligned_misses;

/*
 *	some flushing statistics
 */
unsigned long	ipcreq_fifo_flush_count;
unsigned long	ipcreq_flushed_console_stopped;
unsigned long	ipcreq_flushed_already_receiving;
unsigned long	ipcreq_flushed_other_node;

/*
 *	RTS enqueuing statistics
 */
unsigned long	ipcq_no_post;
unsigned long	ipcq_recv_in_progress;
unsigned long	ipcq_grants_given;
unsigned long	ipcq_locked_or_entries;

#if	IPC_CSUM
/*
 *	perform checksums? (debugging)
 */
#define	IPC_CHECKSUM_MAGIC	0xfeed1ace

int		ipcreq_ck_count;		/* checksum failures */
int ipcreq_ignore_bad_checksums;
int ipcreq_stop_on_csum_failure;

#define	IPC_INCREMENTAL_CSUM(csum, addr, n)	\
		(csum) = ipcreq_incremental_checksum((csum), (addr), (n))
#else	IPC_CSUM
/*
 *	Don't do checksumming...
 */
#define	IPC_INCREMENTAL_CSUM(csum, addr, n)
#endif	IPC_CSUM


#if	IPC_COPY_SEND
/*
 *	copy data to a private buffer before sending?
 *	(just like ipc_ether.c)
 *
 *	XXX SERIOUS NORMA WORKAROUND GOING ON HERE XXX
 */
#endif	IPC_COPY_SEND
/* even if compiled in, can turn off the copy code */
int	ipcreq_copy_send_enabled;


/*
 *	for transfers that are multiples of 4 bytes,
 *	this is the pad word used.
 */
#define	IPC_DANGLE_MAGIC	0xc0da1963


/*
 *	additional switches...
 */
int ipcreq_apply_back_pressure;	/* disable rx interrupts? */
