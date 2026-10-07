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
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: Ovfork.c,v $
 * Revision 1.5  1994/11/18  21:04:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:02:50  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:57:03  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:02:43  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.1  92/03/28  10:17:52  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:24:37  jeffreyh]
 * 
 * Revision 2.2  92/01/16  00:00:54  rpd
 * 	Moved from user collection to mk collection.
 * 
 * Revision 2.2  91/03/26  17:46:29  mrt
 * 	First checkin
 * 
 */
/*
 *	Just calls fork, as the vfork kernel call no longer
 *	does anything different from fork.  The fork program calls
 *	mach_init for the child.
 */

int vfork()
{
	return(fork());
}
