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
 * $Log: varargs.h,v $
 * Revision 1.4  1994/11/18  20:40:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/07/12  19:19:01  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.2.8.2  1994/03/02  20:01:49  andyp
 * Added back in "PGI" because the X86 build machines don't define _PGC
 * when using cpp.
 *
 * Revision 1.2.8.1  1994/02/25  18:29:33  andyp
 * Purged the old Greenhills support and moved it to a separate file.
 * Removed the "PGI" def and used one that will catch either cpp860
 * or PGI's cpp.
 *
 * Revision 1.2  1993/06/30  22:31:51  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/09/22  17:39:58  SSD
 * Initial revision
 *
 * Revision 2.2  91/12/10  16:31:36  jsb
 * 	New files from Intel
 * 	[91/12/10  16:12:01  jsb]
 * 
 */ 

#ifndef	_VARARGS_H_
#define	_VARARGS_H_ 1

#if defined(__GNUC__)
#include <i860/varargs.gcc.h>
#endif	/* defined(__GNUC__) */

#if defined(__PGC__) || defined(_PGC) || defined(PGI)
#include <i860/varargs.pgi.h>
#endif	/* defined(__PGC__) || defined(_PGC) || defined(PGI) */

#if defined(__GH__)
/* old greenhills support */
#include <i860/varargs.gh.h>
#endif	/* defined(__GH__) */

#endif	/* _VARARGS_H_ */
