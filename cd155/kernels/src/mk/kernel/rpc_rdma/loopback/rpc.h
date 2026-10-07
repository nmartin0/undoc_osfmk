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
 * HISTORY:
 * $Log: rpc.h,v $
 * Revision 1.3  1994/11/18  20:59:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:31:22  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.1  1994/02/16  18:12:31  rkl
 *  Added copyright header
 *
 *
 */

#ifndef	RPC_ENGINE_H_
#define	RPC_ENGINE_H_

/*
 *	RPC engine type definitions
 */
typedef	unsigned int		rpc_token_t;

#define	RPC_ENGINE_IDLE		0
#define	RPC_ENGINE_BUSY		1

/*
 *	RPC engine initialization
 */
extern rpc_return_t	rpc_init_engine( int, int, int );


/*
 *	RPC engine payload slots
 */
extern void	*rpc_engine_alloc_payload( int, int );
extern void	*rpc_engine_free_payload( int );

/*
 *	RPC engine operations
 */
extern void	rpc_engine_send_request( int, int, int, int );
extern void	rpc_engine_recv_request( int, int, int );
extern void	rpc_engine_send_reply( int, int );

/*
 *	RPC engine polling interface
 */
extern boolean_t	rpc_engine_status( int );

#endif	/* RPC_ENGINE_H_ */
