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
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * $Log: boot.h,v $
 * Revision 1.5  1994/11/18  20:36:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:13  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:26:46  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:07  dleslie
 * First R1_0 release
 *
 * Revision 2.1.3.1  92/04/30  11:57:14  bernadat
 * 	Moved from cbus to here, applies to both Corollary
 * 	and SystemPro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.1.9.1  92/02/18  18:34:14  jeffreyh
 * 	Created
 * 	[91/06/27  05:00:05  bernadat]
 * 
 */

/*
 * Corollary 386 MP
 * Copyright (c) 1990 OSF Research Institute 
 */

/*
 * Copyright 1990 by Open Software Foundation,
 * Grenoble, FRANCE
 *
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OSF or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 *   OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Define where to store boot code for slaves
 */

#define MP_BOOT	0x1000			/* address where slave boots are loaded */
#define MP_BOOTSEG	0x100	
#define MP_GDT	0x1200			/* temporary gdt address for boot */
#define MP_BOOTSTACK	0x800		/* stack for boot */
#define MP_MACH_START	MP_BOOTSTACK	/* contains address where to jump
					   after boot */
