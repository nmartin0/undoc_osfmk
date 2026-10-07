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
 * $Log: io_port.h,v $
 * Revision 1.5  1994/11/18  20:33:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:24:06  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:22:44  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:22:28  dleslie
 * First R1_0 release
 *
 * Revision 2.1.1.1.2.1  92/03/03  16:15:54  jeffreyh
 * 	Picked up from MK68
 * 	[92/02/26  11:13:47  jeffreyh]
 * 
 * Revision 2.2  92/01/03  20:07:05  dbg
 * 	Created.
 * 	[91/11/07            dbg]
 * 
 */

#ifndef	_I386_IO_PORT_H_
#define	_I386_IO_PORT_H_
/*
 * IO register definitions.
 */
typedef	unsigned short	io_reg_t;

#define	IO_REG_NULL	(0x00ff)	/* reserved */

/*
 * Allocate and destroy io port sets for users to map into
 * threads.
 */
extern void	io_port_create(/* device_t, io_reg_t * */);
extern void	io_port_destroy(/* device_t */);

#endif	/* _I386_IO_PORT_H_ */
