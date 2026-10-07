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
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: i386_exception.c,v $
 * Revision 1.4  1994/11/18  20:30:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:55:47  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:02  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:33:59  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/01/17  12:10:06  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
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
 *	the vax.  unix_exception() handles the machine-independent ones.
 */

boolean_t machine_exception(exception, code, subcode, unix_signal, unix_code)
int	exception, code, subcode;
int	*unix_signal, *unix_code;
{
	switch(exception) {

	    case EXC_BAD_INSTRUCTION:
	        *unix_signal = SIGILL;
		switch (code) {
		    case EXC_I386_INVOP:
			*unix_code = ILL_RESOP_FAULT;
			break;
		    default:
			return(FALSE);
		}
		break;

	    case EXC_ARITHMETIC:
	        *unix_signal = SIGFPE;
		switch (code) {
		    case EXC_I386_INTO:
			*unix_code = FPE_INTOVF_TRAP;
			break;
		    case EXC_I386_DIV:
			*unix_code = FPE_INTDIV_TRAP;
			break;
		    case EXC_I386_BOUND:
			*unix_code = FPE_SUBRNG_TRAP;
			break;
		    case EXC_I386_NOEXT:
		    case EXC_I386_EXTOVR:
		    case EXC_I386_EXTERR:
		    case EXC_I386_EMERR:
			*unix_code = 0;
			break;
		    default:
			return(FALSE);
		}
		break;

	    case EXC_BREAKPOINT:
		*unix_signal = SIGTRAP;
		break;

	    default:
		return(FALSE);
	}
	return(TRUE);
}
