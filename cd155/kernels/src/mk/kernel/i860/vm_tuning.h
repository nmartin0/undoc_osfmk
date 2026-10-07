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
 * $Log: vm_tuning.h,v $
 * Revision 2.7  1994/11/18  20:40:06  mtm
 * Copyright additions/changes
 *
 * Revision 2.6  1993/06/30  22:31:54  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.5  1993/04/27  20:28:42  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 2.4.2.3  1993/04/27  00:18:04  dleslie
 * Patch release of April 23
 *
 * Revision 2.7  1993/04/13  22:10:08  SSD
 * set reservation count to 8 from 10.
 *
 * Revision 2.6  1993/04/12  16:59:37  SSD
 * second code dropt for pager flow control fixes.
 *
 * Revision 2.4  1993/01/24  02:36:35  andyp
 * removed definition of VM_PAGE_INACTIVE_TARGET for T7.1
 * checked in by regnier aka andyp
 *
 * Revision 2.3  1992/09/22  17:41:07  SSD
 * *** empty log message ***
 *
 * Revision 2.3  91/06/18  20:52:13  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:00:35  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:51  jsb
 * 	First checkin.
 * 	[90/12/03  21:36:14  jsb]
 * 
 */
/*
 *	File:	i860/vm_tuning.h
 *
 *	VM tuning parameters for the i860 (without reference bits).
 */

#define VM_PAGEOUT_INTERNAL_RESERVATION_MAX	8	/* default pager */

#define VM_PAGEOUT_EXTERNAL_RESERVATION_MAX	8	/* vnode pager */

#define VM_PAGEOUT_LOCK_REQUEST_RESERVATION_MAX	8
