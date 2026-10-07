/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1992 by Intel Corporation,
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
 * $Log: expansion.h,v $
 * Revision 1.4  1994/11/18  20:42:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/10/27  20:00:11  andyp
 * Initial memory expansion card support.
 *
 *
 *     Expansion board definitions
 */


/* Common ID byte location for all expansion boards */

#define EXP_ID_ADDR 	0xbfffffe0

/*
 *	Identification byte for expansion boards. 
 *
 *	Some MIO cards have an ID byte, some don't.
 *
 *	7 6 5 4   3 2 1 0    ID Board
 *	-------   -------  ---- -----
 *	1 1 0 0   0 x x x  0xc0 MIO (byte may be 0xc0 or 0xff)
 *	1 1 0 0   1 x x x  0xc8 SCSI
 *	1 1 0 1   0 x x x  0xd0 HIPPI
 *	1 1 1 1   0 x x x  0xf0 MEMX  32MB
 *	1 1 1 1   1 x x x  0xf8 MEMX 128MB
 * 	1 1 1 0   0 x x x  0xe0 MEMX  16MB
 *	1 1 1 0   1 x x x  0xe8 MEMX  64MB
 *     |-----------|-----|
 *	   type      rev
 */

#define	EXP_ID_MASK	0xf8

#define	EXP_ID_HIPPI	0xd0
#define	EXP_ID_MIO	0xc0 
#define	EXP_ID_SCSI	0xc8
#define	EXP_ID_MEMX16	0xe0
#define	EXP_ID_MEMX32	0xf0
#define	EXP_ID_MEMX64	0xe8
#define	EXP_ID_MEMX128	0xf8
