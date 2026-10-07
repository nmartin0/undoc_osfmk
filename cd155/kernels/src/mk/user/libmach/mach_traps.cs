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
 * Copyright (c) 1991,1990,1989,1988,1987,1986 Carnegie Mellon University
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
 * $Log: mach_traps.cs,v $
 * Revision 1.5  1994/11/18  21:05:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:03:37  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:57:57  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:03:51  dleslie
 * First R1_0 release
 *
 * Revision 2.4  91/05/14  17:53:36  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/14  14:17:58  mrt
 * 	Added new Mach copyright
 * 	[91/02/13  12:45:30  mrt]
 * 
 * Revision 2.2  89/08/04  15:08:51  rwd
 * 	Created.  All the work gets done in "syscall_sw.h" now.
 * 	[86/09/01            mwyoung]
 * 
 * Revision 2.1  89/08/04  15:08:34  rwd
 * Created.
 * 
 */

#include <mach/syscall_sw.h>
