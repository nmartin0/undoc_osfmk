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
 * Copyright 1993 by Intel Corporation,
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
 * $Log: ipi_endian.h,v $
 * Revision 1.3  1994/12/16  21:39:48  jerrie
 * Added missing parameter to BITFIELD_6 macro.  Checked in with PTS 11392 fix.
 *
 *  Reviewer:  Arlin Davis
 *  Risk:	    Low.
 *  Benefit or PTS #:  11392
 *  Testing:   Ran Evaluation test suites.
 *  Module(s): The following kernel files are being modified:
 * 		kernel/ipi/ipi_disk.c
 * 		kernel/ipi/ipi_master.c
 * 		kernel/ipi/ipi_misc.c
 * 		kernel/ipi/ipi_endian.h
 *
 * Revision 1.2  1994/11/18  20:50:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:54:28  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_endian.h
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Byte/Bit order issues are solved here.
 */


#ifndef	_IPI_ENDIAN_H_
#define	_IPI_ENDIAN_H_

#define	BIT(x)			(1 << ((x) & 31))


/*
 * Macros to take care of bitfield placement within a byte.
 * It might be possible to extend these to something that
 * takes care of multibyte structures, using perhaps the
 * type ("t") parameter.  Someday.
 */

#if	BYTE_MSF

#define	BITFIELD_2(t,a,b)		t b,a
#define	BITFIELD_3(t,a,b,c)		t c,b,a
#define	BITFIELD_4(t,a,b,c,d)		t d,c,b,a
#define	BITFIELD_5(t,a,b,c,d,e)		t e,d,c,b,a
#define	BITFIELD_6(t,a,b,c,d,e,f)	t f,e,d,c,b,a
#define	BITFIELD_7(t,a,b,c,d,e,f,g)	t g,f,e,d,c,b,a
#define	BITFIELD_8(t,a,b,c,d,e,f,g,h)	t h,g,f,e,d,c,b,a

#else	BYTE_MSF

#define	BITFIELD_2(t,a,b)		t a,b
#define	BITFIELD_3(t,a,b,c)		t a,b,c
#define	BITFIELD_4(t,a,b,c,d)		t a,b,c,d
#define	BITFIELD_5(t,a,b,c,d,e)		t a,b,c,d,e
#define	BITFIELD_6(t,a,b,c,d,e,f)	t a,b,c,d,e,f
#define	BITFIELD_7(t,a,b,c,d,e,f,g)	t a,b,c,d,e,f,g
#define	BITFIELD_8(t,a,b,c,d,e,f,g,h)	t a,b,c,d,e,f,g,h

#endif	BYTE_MSF

#endif	_IPI_ENDIAN_H_
