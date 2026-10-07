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
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

/*
 * platform specific optimal hardware transfer alignment
 */

/*
 * HISTORY:
 * $Log: rdma.h,v $
 * Revision 1.3  1994/11/18  20:59:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:31:16  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.9  1994/07/06  20:16:25  andyp
 * Prototyped (and left disabled) a new RDMA function rdma_undo()
 * that will be used by the fast-path enqueue.
 *
 * Revision 1.1.2.8  1994/03/18  18:06:29  andyp
 * Added a queue for incoming readys (rather than a single ready count).
 * The single count caused problems under some conditions when two readys
 * of different sizes were recieved prior to the first send (the sender
 * could send too much data).
 *
 * Revision 1.1.2.7  1994/02/18  01:54:28  andyp
 * Added rdma_seqid_t's for two new polling routines.
 *
 * Revision 1.1.2.6  1994/02/11  23:51:20  andyp
 * Added flag to the engine state indicating that it is faulting.
 * Added some extra assertions.
 *
 * Revision 1.1.2.5  1994/02/11  17:43:44  andyp
 * Added a count field to the rdma_fault_info_t structure.  Transmission
 * faults can now be resumed.
 *
 * Revision 1.1.2.4  1994/02/07  20:20:40  andyp
 * Rounded out RDMA interfaces and added user-mode bindings for testing.
 *
 * Revision 1.1.2.3  1994/02/04  07:57:02  andyp
 * RPC flow control now works; RDMA is still under construction.
 *
 * Revision 1.1.2.2  1994/02/02  16:52:54  rkl
 *  Set RDMA_ALIGN 1 from 64 until we know best alignment.  At this point in
 *  time, the receive engine does not do aligment in all places nessasary.
 *
 * Revision 1.1.2.1  1994/01/27  01:50:38  stans
 *  Added the Maximum number of RDMA requests that can be pending.
 *
 * Revision 1.1  1994/01/24  18:40:15  stans
 *   Platform/hardware specific RDMA alignment specification.
 *
 *
 */

#ifndef	_I860PARAGON_RDMA_H_
#define	_I860PARAGON_RDMA_H_


/*
 *	Favorable hardware alignment
 */
#define	RDMA_ALIGN		1

/*
 *	Maximum number of RDMA requests that can be posted.
 */
#define RDMA_MAXREQ     8



/*
 *	RDMA engine type definitions
 */
typedef short	rdma_slot_t;

typedef struct rdma_engine_req {
	pt_entry_t	*dirbase;	/* used during validation */
	vm_map_t	map;		/* used only when faulting */
	vm_offset_t	buf;		/* buffer pointer */
	unsigned int	count:29,	/* byte count */
			sent:1,
			active:1,
			notify:1;
} rdma_engine_req_t;


typedef struct rdma_engine_status {
	rdma_seqid_t	send_in;
	rdma_seqid_t	recv_in;
	rdma_seqid_t	send_out;
	rdma_seqid_t	recv_out;
} rdma_engine_status_t;


typedef struct rdma_engine {
	unsigned long		rdma_route;	/* route to node */
	unsigned short		rdma_node;	/* remote node */
	rdma_slot_t		rdma_slot;	/* remote node's slot */

	rdma_engine_req_t	rdma_send[RDMA_MAXREQ];
	rdma_engine_req_t	rdma_recv[RDMA_MAXREQ];
	unsigned long		rdma_ready[RDMA_MAXREQ];

	unsigned char		rdma_send_head;
	unsigned char		rdma_send_tail;
	unsigned char		rdma_recv_head;
	unsigned char		rdma_recv_tail;
	unsigned char		rdma_ready_head;
	unsigned char		rdma_ready_tail;

	unsigned char		rdma_state;
	unsigned char		rdma_sending;
	unsigned char		rdma_faulting;


} rdma_engine_t;

#define RDMA_ENG_REQOK		(1 << 4)
#define RDMA_ENG_CONNECTED	(1 << 5)
#define RDMA_STATE_DISCO	0
#define RDMA_STATE_WAIT		1
#define RDMA_STATE_ACCEPT	(RDMA_ENG_REQOK + 2)
#define RDMA_STATE_CONNECT	(RDMA_ENG_REQOK + 3)
#define RDMA_STATE_READY	(RDMA_ENG_REQOK + RDMA_ENG_CONNECTED + 4)
#define RDMA_STATE_STOP		(RDMA_ENG_REQOK + RDMA_ENG_CONNECTED + 5)
#define RDMA_STATE_FLUSH	(RDMA_ENG_REQOK + RDMA_ENG_CONNECTED + 6)


/*
 *	RDMA VM fault information.
 */
typedef struct rdma_fault_info {
	vm_map_t		map;
	vm_offset_t		addr;
	vm_size_t		count;
} rdma_fault_info_t;


/*
 *	RDMA engine interfaces
 */
extern rdma_token_t	rdma_engine_token( rdma_slot_t );
extern rdma_node_t	rdma_engine_crack_token( rdma_token_t );
extern void		rdma_engine_accept( rdma_slot_t );
extern void		rdma_engine_connect( rdma_token_t, rdma_slot_t );
extern void		rdma_engine_disconnect( rdma_slot_t );
extern int		rdma_engine_flush( rdma_slot_t );
extern void		rdma_engine_flush_endpoint( rdma_token_t );
extern void		rdma_engine_undo( rdma_slot_t);
extern rdma_seqid_t	rdma_engine_send(rdma_slot_t, vm_offset_t, vm_size_t,
				boolean_t, vm_map_t);
extern rdma_seqid_t	rdma_engine_recv(rdma_slot_t, vm_offset_t, vm_size_t,
				boolean_t, vm_map_t);

/*
 *	RDMA faulting interfaces
 */
extern void	rdma_recv_fault_info( rdma_handle_t,
				rdma_fault_info_t *);
extern void	rdma_send_fault_info( rdma_handle_t,
				rdma_fault_info_t *);
extern void	rdma_resume_send( rdma_handle_t );
extern void	rdma_resume_recv( rdma_handle_t );


/*
 *	RDMA engine polling interfaces
 */
extern boolean_t	rdma_engine_send_busy( rdma_slot_t );
extern boolean_t	rdma_engine_send_ready( rdma_slot_t );
extern boolean_t	rdma_engine_send_done( rdma_slot_t );
extern rdma_seqid_t	rdma_engine_send_complete( rdma_slot_t );
extern boolean_t	rdma_engine_recv_busy( rdma_slot_t );
extern boolean_t	rdma_engine_recv_ready( rdma_slot_t );
extern boolean_t	rdma_engine_recv_done( rdma_slot_t );
extern rdma_seqid_t	rdma_engine_recv_complete( rdma_slot_t );


/*
 *	RDMA engine variables shared with the coprocessor
 */
extern rdma_engine_t		*rdma_engine;
extern rdma_engine_status_t	*rdma_engine_status;
extern int			rdma_engine_slots;
extern int			rdma_engine_packet_size;

extern rdma_slot_t	*rdma_notify_send_ring;
extern int		rdma_notify_send_in, rdma_notify_send_out;

extern rdma_slot_t	*rdma_notify_recv_ring;
extern int		rdma_notify_recv_in, rdma_notify_recv_out;

extern rdma_slot_t	*rdma_notify_send_fault_ring;
extern int		rdma_notify_send_fault_in, rdma_notify_send_fault_out;

extern rdma_slot_t	*rdma_notify_recv_fault_ring;
extern int		rdma_notify_recv_fault_in, rdma_notify_recv_fault_out;


#endif	/* _I860PARAGON_RDMA_H_ */
