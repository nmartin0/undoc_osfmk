/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: i860_exception.c,v $
 * Revision 1.5  1994/11/18  20:31:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:58:38  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:18:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:09:25  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:29:52  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:21:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:58  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/11/13  12:17:57  rabii
 * 	Initial checkin
 *
 * Revision 2.2  91/09/03  11:12:29  jsb
 * 	First checkin. Derived from i386 sources by Intel SSD.
 * 	[91/09/03  10:16:52  jsb]
 * 
 * Revision 2.2  90/05/21  13:52:42  dbg
 * 	First checkin.
 * 	[90/05/03  12:34:43  dbg]
 * 
 * Revision 1.3  89/02/26  12:35:25  gm0w
 * 	Changes for cleanup.
 * 
 * 31-Dec-88  Robert Baron (rvb) at Carnegie-Mellon University
 *	Derived from MACH2.0 vax release.
 *
 *  2-Nov-87  David Golub (dbg) at Carnegie-Mellon University
 *	Created (from mmax file).
 *
 */

#include <mach/boolean.h>
#include <mach/exception.h>
#include <mach/kern_return.h>
#include <sys/signal.h>

/*
 *	machine_exception translates a mach exception to a unix exception
 *	and code.  This handles all the hardware-specific exceptions for
 *	the i860.  unix_exception() handles the machine-independent ones.
 */

boolean_t machine_exception(exception, code, subcode, unix_signal, unix_code)
int	exception, code, subcode;
int	*unix_signal, *unix_code;
{
	switch(exception) {

	    case EXC_BAD_INSTRUCTION:
	        *unix_signal = SIGILL;
		break;

	    case EXC_ARITHMETIC:
	        *unix_signal = SIGFPE;
		break;

	    case EXC_BREAKPOINT:
		*unix_signal = SIGTRAP;
		break;

	    default:
		return(FALSE);
	}
	return(TRUE);
}
