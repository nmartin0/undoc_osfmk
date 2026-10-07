/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
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
 * Copyright 1991 by Intel Corporation,
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
 * $Log: stdarg.h,v $
 * Revision 1.5  1994/11/18  20:32:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:00:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:19:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:10:44  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:11  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:19  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:25  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1992/07/29  08:31:10  rabii
 * 	Temporarily check in this file as stdarg.h (pjg).
 *
 * Revision 2.2  91/12/10  16:31:38  jsb
 * 	New files from Intel
 * 	[91/12/10  16:12:10  jsb]
 * 
 */ 

/* 
* varargs.h
*
*      Copyright 1990, The Portland Group, Incorporated.
*      All rights reserved.
*
*        THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
* This software is supplied under the terms of a license agreement
* or nondisclosure agreement with The Portland Group and may not be
* copied or disclosed except in accordance with the terms of that
* agreement.
*/

#ifndef _VARARGS_H
#define _VARARGS_H

#include <i860/va_list.h>

extern void  __i860_builtin_va_start(va_list *);
extern void *__builtin_va_arg();
#define va_start(ap,junk) __i860_builtin_va_start(&ap)
#define va_arg(ap,type) (*(type *)__builtin_va_arg(&ap, (type *)0))
#define va_alist __builtin_va_alist
#define va_dcl int __builtin_va_alist;

#define va_end(list)

#endif	/* _VARARGS_H */
