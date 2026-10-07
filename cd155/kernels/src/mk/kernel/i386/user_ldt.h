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
 * $Log: user_ldt.h,v $
 * Revision 1.5  1994/11/18  20:34:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:25:34  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:24:13  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:24:10  dleslie
 * First R1_0 release
 *
 * Revision 2.1.1.1.2.1  92/03/03  16:17:03  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:37:04  jeffreyh]
 * 
 * Revision 2.2  92/01/03  20:10:08  dbg
 * 	Created.
 * 	[91/08/20            dbg]
 * 
 */

#ifndef	_I386_USER_LDT_H_
#define	_I386_USER_LDT_H_

/*
 * User LDT management.
 *
 * Each thread in a task may have its own LDT.
 */

#include <i386/seg.h>

struct user_ldt {
	struct real_descriptor	desc;	/* descriptor for self */
	struct real_descriptor	ldt[1];	/* descriptor table (variable) */
};
typedef struct user_ldt *	user_ldt_t;

/*
 * Check code/stack/data selector values against LDT if present.
 */
#define	S_CODE	0		/* code segment */
#define	S_STACK	1		/* stack segment */
#define	S_DATA	2		/* data segment */

extern boolean_t selector_check(/* thread_t thread,
				int sel,
				int type */);

#endif	/* _I386_USER_LDT_H_ */
