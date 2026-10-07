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
 * $Log: panic.c,v $
 * Revision 1.5  1994/11/18  21:05:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:04:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:58:51  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:04:55  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.1  92/03/28  10:34:03  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:34:46  jeffreyh]
 * 
 * Revision 2.2  92/02/20  15:58:04  elf
 * 	Created from bootstrap's version.
 * 	[92/02/11            rpd]
 * 
 */

#include <mach/port.h>
#include <varargs.h>

static mach_port_t master_host_port;

panic_init(port)
	mach_port_t port;
{
	master_host_port = port;
}

/*VARARGS1*/
panic(s, va_alist)
	char *s;
	va_dcl
{
	va_list listp;

	printf("panic: ");
	va_start(listp);
	vprintf(s, listp);
	va_end(listp);
	printf("\n");

#define RB_DEBUGGER	0x1000	/* enter debugger NOW */
	(void) host_reboot(master_host_port, RB_DEBUGGER);
}
