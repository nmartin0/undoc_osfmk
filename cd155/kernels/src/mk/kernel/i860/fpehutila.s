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
 * $Log: fpehutila.s,v $
 * Revision 1.3  1994/11/18  20:38:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/30  22:30:45  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/08/26  10:45:08  stans
 * Initial revision
 *
 * Revision 2.3  91/06/18  20:51:13  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  18:53:50  jsb]
 * 
 * Revision 2.2  91/06/17  15:44:45  jsb
 * 	From Paul Pierce: created from iPSC/860 NX code.
 * 	[91/06/17  13:41:58  jsb]
 * 
 */

/*
 * updated to match NX version: fpehutila.s 10.1 92/06/24 13:51:00
 *
 * $Id: fpehutila.s,v 1.3 1994/11/18 20:38:58 mtm Exp $
 *
 */

	.file	"fpehutila.s"
	.text
_get_fsr::
	bri r1
	 ld.c fsr,r16

_set_fsr::
	bri r1
	 st.c r16,fsr

_sqrtss::	
	fmov.ds f8,f12
	frsqr.ss f12,f10
	bri      r1
	 fmov.sd  f10, f8
_sqrtsd::
	fmov.ds f8,f12
	frsqr.sd f12,f10
	bri      r1
	 fmov.dd  f10, f8
_sqrtdd::
	frsqr.dd f8,f10
	bri      r1
	 fmov.dd  f10, f8
