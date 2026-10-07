/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * OSF/1 Release 1.0
 *
 *
 * $Id: _sigpending.s,v 1.6 1994/11/19 02:10:16 mtm Exp $
 *
 * HISTORY:
 * $Log: _sigpending.s,v $
 * Revision 1.6  1994/11/19  02:10:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/08/04  23:19:30  stans
 *    Cleanup CVS log messages, set 'cvs admin -c" * "' CVS log message leader.
 *
 * Revision 1.4  1993/04/28  22:01:17  shala
 * Support Stack frame.
 * 
 * Revision 1.3  1993/04/05  22:52:40  shala
 * Removed the syscall for sigpending, since there is C version stub for it.
 * 
 * Revision 1.2  1992/05/07  11:00:38  stans
 * added "sigpending" in addition to "_sigpending"; same syscall.
 */
/*
 * Copyright 1988, 1989, 1990, 1991, 1992, by Intel Corporation,
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

        .file   "_sigpending.s"

#include   "SYS.h"

RPSEUDO(_sigpending,sigpending)
