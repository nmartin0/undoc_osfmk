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
 * $Log: mach_error.c,v $
 * Revision 1.6  1994/11/18  21:04:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  23:03:12  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/10  20:08:34  stans
 *    Removed extra includes and referenced to 'stderr' as we were including
 *    files from '/usr/include' on the build machine. References to 'stderr'
 *    were rewritten to use sprintf() and 'write(2,buff,strlen(buff))'.
 *
 * Revision 1.3  1993/04/27  20:57:29  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:03:14  dleslie
 * First R1_0 release
 *
 * Revision 2.4.2.1  92/03/28  10:24:20  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:28:39  jeffreyh]
 * 
 * Revision 2.4  92/02/19  15:10:52  elf
 * 	Moved mach_error_string and mach_error_type to mach_error_string.c.
 * 	[92/02/11            rpd]
 * 
 * Revision 2.3  92/01/23  15:22:06  rpd
 * 	Changed <servers/errorlib.h> to <errorlib.h>.
 * 	[92/01/16            rpd]
 * 
 * Revision 2.2  92/01/16  00:08:03  rpd
 * 	Moved from user collection to mk collection.
 * 
 * Revision 2.3  91/08/29  15:51:50  rpd
 * 	Changed IPC_MIG_MOD to MACH_IPC_MIG_MOD, to get the new error strings.
 * 	[91/08/22            rpd]
 * 
 * Revision 2.2  91/03/27  16:06:29  mrt
 * 	Changed include of "errorlib.h" to <servers/errorlib.h>
 * 	Added new copyright
 * 	[91/03/20            mrt]
 * 
 */
/*
 * 	File:	mach_error.c
 *	Author:	Douglas Orr, Carnegie Mellon University
 *	Date:	Mar 1988
 *
 *      interprets structured mach error codes and prints
 *      or returns a descriptive string.
 */

#include <mach_error.h>
#include <mach/boolean.h>

extern char * mach_error_string_int();

void
mach_error( str, err )	
	char		*str;
	mach_error_t	err;
{
	char 		*err_str;
	boolean_t	diag;
	char		msg[1024];

	err_str=mach_error_string_int(err, &diag);

	if ( diag ) {
		sprintf(msg, "%s %s %s (0x%x)\n", str, mach_error_type(err),
						err_str, err );
	}
	else {
		sprintf(msg, "%s %s\n", str, err_str);
	}
	/*
	 * write to stderr without having to include (stdio.h>
	 */
	write( 2, msg, strlen(msg) );
}
