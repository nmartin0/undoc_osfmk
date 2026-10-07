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
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
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
 * HISTORY
 * $Log: setjmp.h,v $
 * Revision 2.6  1994/11/18  20:39:29  mtm
 * Copyright additions/changes
 *
 * Revision 2.5  1993/06/30  22:31:23  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.4  1992/09/22  17:35:00  SSD
 * *** empty log message ***
 *
 * Revision 2.4  91/08/28  11:12:23  jsb
 * 	From Intel SSD: new definitions.
 * 	[91/08/26  16:16:46  jsb]
 * 
 * Revision 2.3  91/06/18  20:51:47  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:58:28  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:19  jsb
 * 	First checkin.
 * 	[90/12/03  21:33:20  jsb]
 * 
 */

/*
 * Setjmp/longjmp buffer for i860.
 */

#ifndef	_I860_SETJMP_H_
#define	_I860_SETJMP_H_

#define JMPBUFSZ	(16+8)

#define JBUF_PC		1
#define JBUF_SP		2
#define JBUF_FP		3

typedef	struct jmp_buf {
	int	jmp_buf[JMPBUFSZ];  /* r0-r15, f0-f7 */
} jmp_buf_t;

#endif	_I860_SETJMP_H_
