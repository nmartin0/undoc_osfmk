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
 * HISTORY
 * $Log: mach_i386_types.h,v $
 * Revision 1.5  1994/11/18  20:54:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:48:26  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:39:49  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:41:09  dleslie
 * First R1_0 release
 *
 * Revision 2.1.1.1.2.1  92/03/03  16:21:41  jeffreyh
 * 	New file from TRUNK
 * 	[92/02/26  11:59:15  jeffreyh]
 * 
 * Revision 2.2  92/01/03  20:20:01  dbg
 * 	Created.
 * 	[91/08/20            dbg]
 * 
 */

/*
 * Type definitions for i386 interface routines.
 */

#ifndef	_MACH_MACH_I386_TYPES_H_
#define	_MACH_MACH_I386_TYPES_H_

/*
 * Array of devices.
 */
typedef	device_t	*device_list_t;

/*
 * i386 segment descriptor.
 */
struct descriptor {
	unsigned int	low_word;
	unsigned int	high_word;
};

typedef struct descriptor descriptor_t;
typedef	struct descriptor *descriptor_list_t;

#endif	/* _MACH_MACH_I386_TYPES_H_ */
