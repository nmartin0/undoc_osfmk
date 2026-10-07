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
 *	$Id: rpc.h,v 1.3 1994/11/18 20:59:11 mtm Exp $
 */

#ifndef	RPC_H_
#define	RPC_H_

/*
 *	RPC type definitions
 */
typedef int		rpc_node_t;
typedef int		rpc_return_t;
typedef int		rpc_class_t;
typedef int		rpc_group_t;
typedef int		rpc_handle_t;
typedef void		*rpc_args_t;
typedef void		*rpc_notify_t;
typedef int		rpc_state_t;


/*
 *	RPC return codes
 */
#define	RPC_OK			((rpc_return_t) 0)
#define	RPC_SHORTAGE		((rpc_return_t) 1)
#define	RPC_INVALID_GROUP	((rpc_return_t) 2)
#define	RPC_HANDLE_SHORTAGE	((rpc_return_t) 3)

#define RPC_GROUP_EMPTY		((rpc_handle_t)	-1)


/*
 *	RPC initialization
 */
extern rpc_return_t	rpc_init( int, int, int );
extern rpc_return_t	rpc_group_alloc( rpc_group_t, int );

/*
 *	RPC handle management
 */
extern rpc_handle_t	rpc_handle_alloc( rpc_group_t, boolean_t, int );
extern void		rpc_handle_free( rpc_handle_t );
extern rpc_args_t	rpc_arguments( rpc_handle_t );

/*
 *	RPC operations
 */
extern void	rpc_send_request( /* node, class, handle, callback, callarg */
			rpc_node_t, rpc_class_t,
			rpc_handle_t,
			void (*)( rpc_handle_t, rpc_notify_t ), rpc_notify_t );

extern void	rpc_recv_request( /* class, handle, callback, callarg */
			rpc_class_t,
			rpc_handle_t,
			void (*)( rpc_handle_t, rpc_notify_t ), rpc_notify_t );

extern void	rpc_send_reply( /* handle, callback, callarg */
			rpc_handle_t,
			void (*)( rpc_handle_t, rpc_notify_t ), rpc_notify_t );

extern void	rpc_send_reply_recv( /* class, handle, callback, callarg */
			rpc_class_t,
			rpc_handle_t,
			void (*)( rpc_handle_t, rpc_notify_t ), rpc_notify_t );

/*
 *	RPC polling interfaces
 */
extern boolean_t	rpc_send_ready( rpc_handle_t );
extern boolean_t	rpc_recv_ready( rpc_handle_t );
extern boolean_t	rpc_reply_ready( rpc_handle_t );

/*
 *	Conditionally install a callback after an operation has been posted.
 */
extern boolean_t	rpc_set_callback(
				rpc_handle_t,
				void (*)( rpc_handle_t, rpc_notify_t ),
				rpc_notify_t );

/*
 *	Platform-specific RPC engine
 */
#if	MCMSG_ENG
#include <rpc_rdma/i860paragon/rpc.h>
#endif	/* MCMSG_ENG */

#if	LOOPBACK_ENG
#include <rpc_rdma/loopback/rpc.h>
#endif	/* LOOPBACK_ENG */

#endif	/* RPC_H_ */
