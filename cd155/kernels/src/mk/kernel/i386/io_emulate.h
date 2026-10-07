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
 * $Log: io_emulate.h,v $
 * Revision 1.5  1994/11/18  20:33:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:24:02  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:22:41  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:22:24  dleslie
 * First R1_0 release
 *
 * Revision 2.1.1.1.2.1  92/03/03  16:15:43  jeffreyh
 * 	Picked from from MK68
 * 	[92/02/26  11:12:28  jeffreyh]
 * 
 * Revision 2.2  92/01/03  20:06:59  dbg
 * 	Created.
 * 	[91/12/06            dbg]
 * 
 */

#ifndef	_I386_IO_EMULATE_H_
#define	_I386_IO_EMULATE_H_

/*
 * Return codes from IO emulation.
 */
extern int	emulate_io(/*
			struct i386_saved_state *regs,
			int	opcode,
			int	io_port
			   */);

#define	EM_IO_DONE	0	/* IO instruction executed, proceed */
#define	EM_IO_RETRY	1	/* IO port mapped, retry instruction */
#define	EM_IO_ERROR	2	/* IO port not mapped */

#endif	/* _I386_IO_EMULATE_H_ */
