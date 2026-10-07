/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
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
 *	Copyright 1994  Intel Corporation.
 *
 *	$Header: /afs/ssd/i860/CVS/mk/kernel/rpc_rdma/i860paragon/rpc.h,v 1.5 1994/11/18 20:59:16 mtm Exp $
 */

/*
 * SSD HISTORY
 * $Log: rpc.h,v $
 * Revision 1.5  1994/11/18  20:59:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/08/31  21:25:44  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.2.2.1  1994/08/17  06:41:44  andyp
 * Merged in from the mainline the fixes for PTS #10438.
 *
 * Revision 1.3  1994/08/17  05:32:44  andyp
 * Made rpc_send_reply_recv() a single request to the MCP rather
 * than two with a spin loop in the middle.  This change also improves
 * inline Mach message latency by about 8%.
 *
 * Revision 1.2  1994/07/12  21:31:17  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.3  1994/03/29  18:09:22  andyp
 * Added the Larsen/Heisenberg Compensator that resolves races for
 * retroactive RPC callback installation.
 *
 * Revision 1.1.2.2  1994/03/08  22:10:13  andyp
 * Added routines for installing an RPC callback after a request had
 * been posted.  The MCMSG engine already supported this -- I just
 * exported an interface.
 *
 * Revision 1.1.2.1  1994/02/04  07:57:07  andyp
 * RPC flow control now works; RDMA is still under construction.
 *
 * Revision 1.1  1994/01/26  23:22:30  andyp
 * Initial revision
 *
 * Revision 1.1  1994/01/25  00:51:09  andyp
 * First checkin of conforming RPC implementation for the NORMA rewrite.
 *
 *
 * END SSD HISTORY
 */

#ifndef	RPC_ENGINE_H_
#define	RPC_ENGINE_H_

/*
 *	RPC engine type definitions
 */
typedef short			rpc_slot_t;
typedef char			rpc_engine_state_t;
typedef char			rpc_engine_intr_t;
typedef char			rpc_engine_lock_t;
typedef	unsigned int		rpc_token_t;
typedef unsigned int		rpc_datum_t;


/*
 *	Associated with each RPC engine is a single payload bay
 *	of aligned on favorable boundaries.  On Paragon, it is
 *	fixed in size (configured at boot time).
 *
 */
typedef struct rpc_engine_buf {
	rpc_datum_t		rpc_args[1];		/* variable */
} rpc_engine_buf_t;


/*
 *	Each RPC engine has some status bits and a pointer (set during
 *	intialization) to the corresponding payload bay.
 */
typedef struct rpc_engine {
	struct rpc_engine		*rpc_prev;
	struct rpc_engine		*rpc_next;
	rpc_engine_buf_t		*rpc_buffer;
	rpc_token_t			rpc_token;
	unsigned long			rpc_bp1;
	unsigned long			rpc_bp2;
	unsigned long			rpc_route;
	unsigned short			rpc_node;
	unsigned short			rpc_class;

	/*
	 *	I'd really like to move the following five fields
	 *	into their own separate vectors indexed by slot...
	 *	...partly because there are exactly 32 bytes
	 *	in the previous fields.
	 */
	volatile rpc_engine_state_t	rpc_state;
	volatile char			rpc_intr;
	volatile char			rpc_rearm;

	/*
	 *	Two interlocks for handling callback installation
	 *	after-the-fact.
	 */
	volatile rpc_engine_lock_t	rpc_lock_mcp;
	volatile rpc_engine_lock_t	rpc_lock_cpu;

} rpc_engine_t;


#define	RPC_ENGINE_IDLE		((rpc_engine_state_t) 0)
#define	RPC_ENGINE_BUSY		((rpc_engine_state_t) 1)



/*
 *	The following value in the "rpc_prev" field means
 *	that the engine is no longer a candidate for transmission.
 */
#define RPC_PAYLOAD_RECEIVED		((rpc_engine_t *) -1)


/*
 *	RPC engine initialization
 */
extern rpc_return_t	rpc_init_engine( int, int, int );


/*
 *	RPC engine payload slots
 */
extern void	*rpc_engine_alloc_payload( int, int );
#define rpc_engine_free_payload(slot)	((void *) 0)


/*
 *	RPC engine operations
 */
extern void	rpc_engine_send_request( int, int, int, int );
extern void	rpc_engine_recv_request( int, int, int );
extern void	rpc_engine_send_reply( int, int );

/*
 *	Install a callback after an operation has been posted.
 */
extern boolean_t	rpc_engine_set_notify( int );

/*
 *	RPC engine polling interface
 */
extern boolean_t	rpc_engine_status( int );


/*
 *	RPC engine variables shared with coprocessor
 */
extern int		rpc_classes;
extern int		rpc_engine_slots;
extern rpc_engine_t	*rpc_engine;

/*
 *	RPC engine notification ring buffers.
 */
extern rpc_slot_t	*rpc_notify_request_ring;
extern int		rpc_notify_request_in, rpc_notify_request_out;

extern rpc_slot_t	*rpc_notify_reply_ring;
extern int		rpc_notify_reply_in, rpc_notify_reply_out;

extern rpc_slot_t	*rpc_notify_depart_ring;
extern int		rpc_notify_depart_in, rpc_notify_depart_out;


#endif	/* RPC_ENGINE_H_ */
