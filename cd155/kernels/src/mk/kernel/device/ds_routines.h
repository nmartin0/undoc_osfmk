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
 * $Log: ds_routines.h,v $
 * Revision 1.6  1994/11/18  20:31:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  22:22:15  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:27:10  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:20:34  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:20:22  dleslie
 * First R1_0 release
 *
 * Revision 2.5  91/08/28  11:11:22  jsb
 * 	Page list support: device_write_dealloc returns a boolean.
 * 	[91/08/05  17:32:28  dlb]
 * 
 * Revision 2.4  91/05/14  15:47:56  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:09:33  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:29:15  mrt]
 * 
 * Revision 2.2  89/09/08  11:24:24  dbg
 * 	Created.
 * 	[89/08/04            dbg]
 * 
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	8/89
 *
 *	Device service utility routines.
 */

#ifndef	DS_ROUTINES_H
#define	DS_ROUTINES_H

#include <vm/vm_map.h>
#include <device/device_types.h>

/*
 * Map for device IO memory.
 */
vm_map_t	device_io_map;

kern_return_t	device_read_alloc();
kern_return_t	device_write_get();
boolean_t	device_write_dealloc();

boolean_t	ds_open_done();
boolean_t	ds_read_done();
boolean_t	ds_write_done();
 
/*
 * Internal i/o routines allowing caller to optionally have the i/o appear
 * synchronous, avoiding the messaging overhead on the reply.
 */
io_return_t     ds_device_read_common();
io_return_t     ds_device_write_common();

#endif	DS_ROUTINES_H
