/*
 *
 *$Copyright
 *Copyright 1993, 1994 , 1995 Intel Corporation
 *INTEL CONFIDENTIAL
 *The technical data and computer software contained herein are subject
 *to the copyright notices; trademarks; and use and disclosure
 *restrictions identified in the file located in /etc/copyright on
 *this system.
 *Copyright$
 *
 */
 
/*
 * @OSF_COPYRIGHT@
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
 * $Log: user_bcopy.s,v $
 *Revision 1.5  1994/11/18  20:32:15  mtm
 *Copyright additions/changes
 *
 *Revision 1.4  1993/09/21  20:02:17  cfj
 *Pick up the dnoveck@osf.org i860 bcopy enhancements from ad1.0.5
 *
 * Revision 2.3  93/06/16  13:49:29  dnoveck
 * 	Convert to i860_copy.s.h to get high-speed copy support.
 * 
 * Revision 2.2  92/11/17  19:51:59  loverso
 * 	New for server
 * 
 * $EndLog$
 */

/*
 * The user_bcopy and user_bcopy2 routines as well as the fault
 * handling code is in the included file <i860/i860_copy.s.h>.
 */
#include <i860/asm.h>



/*
 * Marks the starting address of functions that can fault on access to user
 * addresses.  Any exception between here and uacc_end will result in the
 * server branching us to the common error recovery point, uacc_err.
 *
 * THUS, it is extremely important that all routines are common on
 * enter and return!
 *
 * Note that uacc_end and uacc_err are in i860_copy.s.h.
 */
ENTRY(uacc_start)

#define _USER_BCOPY_
#include <i860/i860_copy.s.h>

//eof


