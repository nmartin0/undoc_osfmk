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
 * HISTORY
 * $Log: if_siop.h,v $
 * Revision 1.4.8.1  1995/06/11  18:32:44  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.4  1995/04/07  21:14:41  jerrie
 * Changed SLOG macro to use db_printf() instead of printf().
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low
 *  Benefit or PTS #: Prevents debug output logging to the console
 *  Testing:	   Ran the debug kernel with SLOGs enabled
 *  Module(s):	   if_siop.h
 *
 * Revision 1.3  1995/03/28  01:22:12  jerrie
 * Comment out trigger register output on every SIOP function entry.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low
 *  Benefit or PTS #: 12827
 *  Testing:	   Developer
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:43:33  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	if_siop.h
 *	Author:	 Vineet Kumar
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	5/94
 *
 *	SCSI-16 debugging macros
 */


#ifndef	_SIOP_IF_H_
#define	_SIOP_IF_H_

#define S_BIT(x)	(1 << x)
#define SLOG_ENTER	S_BIT(0)
#define SLOG_LOW	S_BIT(1)
#define SLOG_MEDIUM	S_BIT(2)
#define SLOG_HIGH	S_BIT(3)

#if	MACH_ASSERT

#define SLOG(lev, args) \
        if((SLOG_level & lev) && (SLOG_id & _this_id)) db_printf args

#define SLOG_ENTRY(id, args)		   \
        long  _this_id = S_BIT(id);	   \
	/* outl(SDB_TRIGGER, (long)id); */ \
        SLOG(SLOG_ENTER, args)

#define SLOG_EXIT(args)     \
        SLOG(SLOG_ENTER, args)

#else	MACH_ASSERT

#define SLOG(l, a)
#define SLOG_ENTRY(i, a)
#define SLOG_EXIT(a)

#endif	MACH_ASSERT

#endif	_SIOP_IF_H_










