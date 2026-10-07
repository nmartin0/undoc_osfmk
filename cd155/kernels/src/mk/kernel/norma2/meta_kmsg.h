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
 * Copyright 1992 by Intel Corporation,
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
 *	$Id: meta_kmsg.h,v 1.3 1994/11/18 20:58:39 mtm Exp $
 */


#ifndef	_IPC_META_KMSG_H_
#define	_IPC_META_KMSG_H_

/*
 * Meta kernel message
 *
 *	+===============================================================+
 *	|			Queue link forward			|
 *	+---------------------------------------------------------------+
 *	|			Queue link backwards			|
 *	+---------------------------------------------------------------+
 *	| T |		size (in bytes) of kmsg body			|
 *	+---------------------------------------------------------------+
 *	|			RDMA token				|
 *	+---------------------------------------------------------------+
 *	|		destination uid/ipc_port_t token		|
 *	+===============================================================+
 *	^								^
 *	|------------------ sizeof(longword) ---------------------------|
 *
 * A "meta" kmsg MUST look exactly like a local Mach IPC kmsg for the first
 * three long words. The two beginning longwords comprise linked list Q
 * pointers. The next lowngword is divided into two fields:
 *	T == a 2-bit type field
 *		0 == local Mach IPC kmsg
 *		1 == meta-kmsg
 *		2 == net-kmsg
 *
 *	size == # of bytes in the Mach message (kmsg) body.
 */

typedef struct meta_kmsg {
	struct meta_kmsg	*mkm_next, *mkm_prev;
	struct {
		unsigned int
			mkmst_size:30,          /* bytes in kmsg body */
			mkmst_kmsg_type:2;
						/* 0 == local kmsg
						 * 1 == meta kmsg
						 * 2 == net kmsg
						 * 3 == unused.
						 */
	} mkm_st;
	rdma_token_t	mkm_rdma_token;		/* sender's RDMA token */
	ipc_port_t	mkm_dest_port;		/* needed for port sets */
} meta_kmsg_data;

typedef struct meta_kmsg *meta_kmsg_t;

#define	META_KMSG_NULL ((meta_kmsg_t)0)
#define META_KMSG_SIZE (sizeof(struct meta_kmsg))

#define	mkm_size	mkm_st.mkmst_size
#define	mkm_kmsg_type	mkm_st.mkmst_kmsg_type


extern kern_return_t	meta_kmsg_init( int );
extern meta_kmsg_t	meta_kmsg_alloc( rdma_token_t, vm_size_t, boolean_t );
extern int		meta_kmsg_free( meta_kmsg_t );

#endif	/* _IPC_META_KMSG_H_ */
