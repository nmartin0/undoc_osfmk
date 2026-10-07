/*
 *
 *$Copyright
 *Copyright 1993, 1994 , 1995 Intel Corporation
 *INTEL CONFIDENTIAL
 *The technical data and computer software contained herein are subject
 *to the copyright notices; trademarks; and use and disclosure
 *restrictions identified in the file located in /etc/copyright on
 *this system.
 *Copyright$
 *
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
 * $Log: bcopy.s,v $
 *Revision 1.6  1994/11/18  20:25:32  mtm
 *Copyright additions/changes
 *
 *Revision 1.5  1993/09/21  20:05:29  cfj
 *Pick up dnoveck@osf.org i860 bcopy enhancements from ad1.0.5
 *
 * Revision 2.4  93/06/16  13:54:58  dnoveck
 * 	Convert to use i860_copy.s.h for high speed copies.
 * 
 * Revision 2.3  92/01/09  15:05:33  roy
 * 	Eliminate unused code.
 * 
 * Revision 2.2  91/12/20  10:32:26  rabii
 * 	Initial check-in
 */

#define _BCOPY_
#if DEBUG
#define _BCOPY_DEBUG_
#define _BCOPY_ERROR1_ _emul_panic
#endif /* DEBUG */
#include <i860/i860_copy.s.h>

