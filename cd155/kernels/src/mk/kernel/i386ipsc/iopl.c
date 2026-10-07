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
 * $Log: iopl.c,v $
 * Revision 1.5  1994/11/18  20:37:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:56  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:27  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:58  dleslie
 * First R1_0 release
 *
 * Revision 2.3.2.1  92/03/03  16:17:43  jeffreyh
 * 	Created iopl_device to keep i386/io_emulate.c happy.
 * 	[92/02/26            jeffreyh]
 * 
 * Revision 2.3  91/06/18  20:50:20  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  20:06:41  jsb]
 * 
 * Revision 2.2  90/12/04  14:47:21  jsb
 * 	First checkin.
 * 	[90/12/04  10:56:56  jsb]
 * 
 */

#include <mach/vm_prot.h>
#include <mach/i386/vm_types.h>
#include <mach/i386/vm_param.h>
#include <device/io_req.h>
#include <ipc/ipc_port.h>

/*
 * IOPL device.
 */
ipc_port_t	iopl_device_port = IP_NULL;
device_t	iopl_device = 0;

int
ioplopen(dev, flag, ior)
	int	dev;
	int	flag;
	io_req_t ior;
{
	iopl_device_port = ior->io_device->port;
	return (0);
}

/*ARGSUSED*/
int
ioplclose(dev, flag)
	int	dev;
	int	flag;
{
	iopl_device_port = IP_NULL;
	return (0);
}

/*ARGSUSED*/
int
ioplmmap(dev, off, prot)
	int		dev;
	vm_offset_t	off;
	vm_prot_t	prot;
{
	if (off >= 0x60000)
	    return (-1);
	return (i386_btop(0xa0000+off));
}
