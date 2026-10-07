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
 * $Log: ptest.h,v $
 * Revision 1.2  1994/11/19  03:01:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/05/19  22:10:07  stans
 * Initial revision
 *
 * Revision 1.2  1992/10/21  13:14:48  devrcs
 * 	new for V4.1 release
 * 	[1992/10/01  17:07:33  david]
 *
 * Revision 2.3  91/03/19  12:17:57  mrt
 * 	Changed to new copyright
 * 
 * Revision 2.2  90/09/12  16:30:49  rpd
 * 	First check-in.
 * 	[90/09/11            rpd]
 * 
 * $EndLog$
 */

#ifndef	_PTEST_H_
#define _PTEST_H_

extern void set_test();
extern void port_type_test();

extern void rename_test_1();
extern void rename_test_2();
extern void rename_test_3();
extern void rename_test_4();

extern void extract_receive_test_1();
extern void extract_receive_test_2();

#endif	_PTEST_H_
