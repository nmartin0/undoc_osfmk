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
 * $Log: kmsg_parser.h,v $
 * Revision 1.3  1994/11/18  20:58:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:30:15  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.1  1994/02/01  01:00:04  rkl
 *  Added common logging macros
 *
 * Revision 1.1  1994/01/25  19:42:05  rkl
 *  Initial checkin.
 *
 *
 */

#ifndef	_KMSG_PARSE_H_
#define	_KMSG_PARSE_H_
/*
 *  ipc_kmsg body parsing table.
 */
typedef	struct kmsg_parse_tbl {
	kern_return_t (*inline_port)();
	kern_return_t (*ool_port)();
	kern_return_t (*inline_data)();
	kern_return_t (*ool_data)();
} kmsg_parse_tbl_t;

/*
 *  kmsg_parse.c prototypes
 */
extern	mach_msg_type_long_t*	norma_parse_kmsg( int*,
						  mach_msg_type_long_t*,
						  vm_size_t*,
						  kmsg_parse_tbl_t*,
						  kern_return_t*,
						  void* );

#endif	/* _KMSG_PARSE_H_ */
