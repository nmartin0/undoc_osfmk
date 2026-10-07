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
 */

/*
 * SSD HISTORY
 * $Log: rdma.h,v $
 * Revision 1.3  1994/11/18  20:59:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:31:09  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.10  1994/07/06  20:16:16  andyp
 * Prototyped (and left disabled) a new RDMA function rdma_undo()
 * that will be used by the fast-path enqueue.
 *
 * Revision 1.1.2.9  1994/02/18  01:54:14  andyp
 * Added rdma_seqid_t's for two new polling routines.
 *
 * Revision 1.1.2.8  1994/02/16  18:20:57  rkl
 *  Added support for loopback engine configurations.
 *
 * Revision 1.1.2.7  1994/02/04  07:56:37  andyp
 * RPC flow control now works; RDMA is still under construction.
 *
 * Revision 1.1.2.6  1994/01/27  01:49:29  stans
 *   Added rdma_fault_info
 *
 * Revision 1.1.2.5  1994/01/26  21:40:35  stans
 *  'rdma_handle_alloc()' needed a ";" at the end.
 *
 * Revision 1.1.2.4  1994/01/26  18:38:34  andyp
 * Added some debugging routines and added external definitions
 * to rdma.h to conform with the spec.
 *
 * Revision 1.1.2.3  1994/01/24  18:42:48  stans
 *   Support platform specific alignment definition.
 *
 * Revision 1.1.2.2  1994/01/24  17:40:47  rkl
 *  Changed RDMA_BEST_xxx to RDMA_xxx
 *
 * Revision 1.1.2.1  1994/01/24  17:26:50  stans
 *   Update copyright year, make sure this file is on the NORMA2 branch.
 *
 * Revision 1.1  1994/01/24  17:17:18  stans
 *   RDMA (Retargetable DMA) interface header file.
 *
 * END SSD HISTORY
 */

#ifndef	RDMA_H_
#define	RDMA_H_

/*
 *	RDMA type definitions
 */
typedef	int		rdma_node_t;
typedef int		rdma_return_t;
typedef int		rdma_group_t;
typedef int		rdma_handle_t;
typedef void		*rdma_notify_t;
typedef int		rdma_state_t;
typedef unsigned int	rdma_token_t;
typedef unsigned long	rdma_seqid_t;


/*
 *	RDMA return codes
 */
#define RDMA_SUCCESS		((rdma_return_t) 0)
#define RDMA_SHORTAGE		((rdma_return_t) 1)
#define RDMA_INVALID_GROUP	((rdma_return_t) 2)
#define RDMA_HANDLE_SHORTAGE	((rdma_return_t) 3)
#define RDMA_CONNECTED		((rdma_return_t) 4)

#define RDMA_GROUP_EMPTY	((rdma_handle_t) -1)


/*
 *	RDMA initialization
 */
extern rdma_return_t	rdma_init( int, int );
extern rdma_return_t	rdma_group_alloc( rdma_group_t, int );

/*
 *	RDMA handle management
 */
extern rdma_handle_t	rdma_handle_alloc( rdma_group_t, boolean_t, rdma_node_t);
extern void		rdma_handle_free( rdma_handle_t );


/*
 *	RDMA tokens and endpoints
 */
extern rdma_token_t	rdma_token( rdma_handle_t );
extern rdma_node_t	rdma_endpointof( rdma_token_t );


/*
 *	RDMA connections
 */
extern void		rdma_accept( rdma_handle_t );
extern void		rdma_connect( rdma_token_t, rdma_handle_t );
extern void		rdma_disconnect( rdma_handle_t );
extern rdma_return_t	rdma_flush( rdma_handle_t );
extern void		rdma_flush_endpoint( rdma_token_t );
extern void		rdma_undo( rdma_handle_t );


/*
 *	RDMA callback operations
 */
extern void	rdma_set_send_callback(	/* handle, callback, callarg */
			rdma_handle_t,
			void (*)( rdma_handle_t, rdma_notify_t ),
			rdma_notify_t );

extern void	rdma_set_recv_callback(	/* handle, callback, callarg */
			rdma_handle_t,
			void (*)( rdma_handle_t, rdma_notify_t ),
			rdma_notify_t );


/*
 *	RDMA transfer operations
 */
extern rdma_seqid_t rdma_send( rdma_handle_t,
			vm_offset_t, vm_size_t, boolean_t, vm_map_t );
extern rdma_seqid_t rdma_recv( rdma_handle_t,
			vm_offset_t, vm_size_t, boolean_t, vm_map_t );

/*
 *	RDMA polling interfaces
 */
extern boolean_t	rdma_send_busy( rdma_handle_t );
extern boolean_t	rdma_send_ready( rdma_handle_t );
extern boolean_t	rdma_send_done( rdma_handle_t );
extern rdma_seqid_t	rdma_send_complete( rdma_handle_t );
extern boolean_t	rdma_recv_busy( rdma_handle_t );
extern boolean_t	rdma_recv_ready( rdma_handle_t );
extern boolean_t	rdma_recv_done( rdma_handle_t );
extern rdma_seqid_t	rdma_recv_complete( rdma_handle_t );


/*
 *	RDMA alignment macros
 *
 *		provide optimal hardware alignment.
 */
#if	MCMSG_ENG
#include <rpc_rdma/i860paragon/rdma.h>
#endif	/* MCMSG_ENG */

#if	LOOPBACK_ENG
#include <rpc_rdma/loopback/rdma.h>
#endif	/* LOOPBACK_ENG */

#define	RDMA_ALIGN_MASK		(RDMA_ALIGN - 1)

#define RDMA_TRANSFER_ALIGNMENT(addr)   ((vm_offset_t)(addr) & ~RDMA_ALIGN_MASK)

#define RDMA_TRANSFER_LENGTH(offset, len)                                    \
((len) ? (((len) + ((vm_offset_t)(offset) - RDMA_TRANSFER_ALIGNMENT(offset)) + \
                                 RDMA_ALIGN_MASK) &                          \
                                ~RDMA_ALIGN_MASK)                            \
        : 0)

#endif	/* RDMA_H_ */
