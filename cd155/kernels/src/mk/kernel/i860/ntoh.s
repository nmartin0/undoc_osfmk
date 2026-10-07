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
 * $Log: ntoh.s,v $
 * Revision 2.4  1994/11/18  20:39:16  mtm
 * Copyright additions/changes
 *
 * Revision 2.3  1993/06/30  22:31:11  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.2  1992/09/26  21:39:50  SSD
 * OSF norma13 version.
 *
 * Revision 2.3  91/06/18  20:51:34  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:57:02  jsb]
 * 
 * Revision 2.2  90/12/04  14:48:57  jsb
 * 	First checkin.
 * 	[90/12/03  21:31:19  jsb]
 * 
 */

/*
 * Function deduced from i386 code:
 * htonl(0xddccbbaa) = 0xaabbccdd
 */
	.text
	.align	8
	.globl	_htonl
_htonl:
	shr	16,r0,r0	// rotate right 16 bits
	shrd	r16,r16,r16
	shl	8,r16,r17	// shift left 8 bits and
	andnoth	255,r17,r17	// mask with 0xFF00FF00
	andnot	255,r17,r17	
	shr	8,r16,r16	// shift original value right 8
	andnoth	65280,r16,r16	// mask with 0x00FF00FF
	andnot	65280,r16,r16
	or	r16,r17,r16	// combine partial results
	bri	r1
	nop
