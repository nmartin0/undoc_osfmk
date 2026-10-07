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
 * $Log: main.h,v $
 * Revision 1.2  1994/11/19  03:01:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/05/19  22:09:40  stans
 * Initial revision
 *
 * Revision 1.2  1992/10/21  13:13:54  devrcs
 * 	new for V4.1 release
 * 	[1992/10/01  17:06:07  david]
 *
 * Revision 2.3  91/03/19  12:17:04  mrt
 * 	Changed to new copyright
 * 
 * Revision 2.2  90/09/12  16:30:01  rpd
 * 	First check-in.
 * 	[90/09/11            rpd]
 * 
 * $EndLog$
 */

#ifndef	_MAIN_H_
#define _MAIN_H_

#include <mach/boolean.h>

extern char *program;

extern boolean_t Verbose;
extern boolean_t MDebug;

#endif	_MAIN_H_
