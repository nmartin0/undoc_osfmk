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
 * $Log: machparam.h,v $
 * Revision 1.4  1994/11/18  20:39:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:31:06  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1993/06/18  17:43:44  stans
 *    We now have a machine independent manner in which one can get the
 *    correct spl (SPLIMP) defines. Some machine independent files in the
 *    'device/' directory want spl definitions so they
 *    "#include <machine/machparam.h>" and expect this to work; It does now.
 *
 * Revision 1.1  1992/10/15  23:54:07  SSD
 * Initial revision
 *
 * Revision 2.3  91/06/18  20:51:32  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:56:52  jsb]
 * 
 * Revision 2.2  90/12/04  14:48:55  jsb
 * 	First checkin.
 * 	[90/12/03  21:31:03  jsb]
 * 
 */

/*
 *	Machine-dependent SPL definitions.
 *
 *	SPLs are true functions on i860; see spl.c.
 */

#ifdef  PARAGON860
#include <i860paragon/spl.h>
#endif

#ifdef  IPSC860
#include <i860ipsc/spl.h>
#endif

