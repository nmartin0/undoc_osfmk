/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_FREE_COPYRIGHT@
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
 * $Log: kalloc.h,v $
 * Revision 1.2  1994/11/19  02:38:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/23  01:02:15  stans
 * Initial revision
 *
 * Revision 1.2  1992/05/12  14:49:00  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/04  07:55:19  condict]
 *
 * Revision 2.2  91/07/06  15:13:43  jsb
 * 	First checkin.
 * 
 * $EndLog$
 */
/*
 *	File:	kalloc.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	User-level kalloc/kfree definitions.
 */

#include <mach.h>

extern vm_offset_t kalloc();
extern void kfree();
