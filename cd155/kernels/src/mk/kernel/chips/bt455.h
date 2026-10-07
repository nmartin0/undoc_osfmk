/*
 * 
 * $Copyright
 * Copyright 1991, 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * HISTORY
 * $Log: bt455.h,v $
 * Revision 1.5  1994/11/18  20:26:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:17:04  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:14:44  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:14:08  dleslie
 * First R1_0 release
 *
 * Revision 2.2  91/08/24  11:50:24  af
 * 	Cleaned up a bit, described spec sources more precisely.
 * 	[91/08/22  11:03:56  af]
 * 
 * 	Created, from Brooktree specs:
 * 	"Graphics And Imaging Product Databook 1991"
 * 	"Bt454/Bt455 170 Mhz Monolithic CMOS 16 Color Palette RAMDAC"
 * 	Brooktree Corp. San Diego, CA, 3rd Ed.
 * 	[91/07/25            af]
 * 
 */
/*
 *	File: bt455.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	7/91
 *
 *	Defines for the bt455 RAMDAC
 */

typedef struct {
	volatile unsigned char	addr_cmap;
	volatile unsigned char	addr_cmap_data;
	volatile unsigned char	addr_clr;
	volatile unsigned char	addr_ovly;
} bt455_regmap_t;

/*
 * Color Map entries 00-0f are accessed indirectly
 */
