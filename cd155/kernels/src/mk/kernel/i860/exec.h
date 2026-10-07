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
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
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
 * $Log: exec.h,v $
 * Revision 2.6  1994/11/18  20:38:53  mtm
 * Copyright additions/changes
 *
 * Revision 2.5  1993/06/30  22:30:37  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.4  1992/09/26  21:46:46  SSD
 * OSF norma13 version
 *
 * Revision 2.4  91/12/10  16:30:46  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:48  jsb]
 * 
 * Revision 2.3  91/06/18  20:51:01  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:52:53  jsb]
 * 
 * Revision 2.2  91/06/17  15:44:32  jsb
 * 	First checkin.
 * 	[91/06/17  10:31:12  jsb]
 * 
 */
/*
 * Exec stucture for i860 coff file.
 */

#ifndef	_EXEC_
#define	_EXEC_	1

struct exec_hdrs {
	struct filehdr	fh;
	struct aouthdr	ah;
	unsigned long	pad0;
	unsigned long	pad1;
	struct scnhdr	sh0;
	struct scnhdr	sh1;
	struct scnhdr	sh2;
};

#define OMAGIC 0407
#define NMAGIC 0410
#define ZMAGIC 0413

#endif	_EXEC_

