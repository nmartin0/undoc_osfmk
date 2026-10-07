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
 *	$Id: dipc_blocked.h,v 1.2 1994/11/18 20:57:30 mtm Exp $
 */

extern void	dipc_blocked_sender_destroy( ipc_port_t );
extern void	dipc_blocked_sender_add( ipc_port_t, rdma_node_t );
extern int	dipc_blocked_sender_remove( ipc_port_t, rdma_node_t );

extern boolean_t	dipc_blocked_sender_select( ipc_port_t, rdma_node_t *);
