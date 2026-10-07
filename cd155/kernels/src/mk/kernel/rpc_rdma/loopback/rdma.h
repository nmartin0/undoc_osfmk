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
 * $Log: rdma.h,v $
 * Revision 1.2  1994/11/18  20:59:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/02/16  18:10:25  rkl
 * Initial revision
 *
 *
 *
 */

#ifndef	RDMA_ENG_H_
#define	RDMA_ENG_H_

/*
 *	Favorable hardware alignment
 */
#define	RDMA_ALIGN		1

/*
 *	Maximum number of RDMA requests that can be posted.
 */
#define RDMA_MAXREQ     16

#if	PARAGON860
/*
 *	RDMA VM fault information.
 */
typedef struct rdma_fault_info {
	vm_map_t		map;
	vm_offset_t		addr;
	vm_size_t		count;
} rdma_fault_info_t;
#endif

#endif /* RDMA_ENG_H_ */
