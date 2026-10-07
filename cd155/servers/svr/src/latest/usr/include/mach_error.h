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
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: mach_error.h,v $
 * Revision 1.3  1994/11/18  21:10:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:15:21  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:44  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:21  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:36:46  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:30:35  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:50:02  gm]
 * 
 * Revision 1.2  90/01/02  19:16:14  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/12/26  10:10:24  gm
 * 	Current version from CMU.
 * 	[89/12/23            gm]
 * 
 * Revision 1.3  89/05/05  18:44:57  mrt
 * 	Cleanup for Mach 2.5
 * 
 * Revision 0.0  88/05/11            mrt
 * 	Added definition for mach_error_type
 * 	[88/05/11            mrt]
 * 
 * Revision 0.0  88/03/29            mrt
 * 	Corrected args to mach_error and added c_plusplus conditionals
 * 	[88/03/29            mrt]
 * 
 * Revision 0.0  86/10/30            mwyoung
 * 	Created.
 * 	[86/10/30            mwyoung]
 * 
 * $EndLog$
 */ 

#ifndef	_LIB_MACH_ERROR_H_
#define	_LIB_MACH_ERROR_H_	1

#include <mach/error.h>

char		*mach_error_string(
/*
 *	Returns a string appropriate to the error argument given
 */
#if	c_plusplus
	mach_error_t error_value
#endif	c_plusplus
				);

void		mach_error(
/*
 *	Prints an appropriate message on the standard error stream
 */
#if	c_plusplus
	char 		*str,
	mach_error_t	error_value
#endif	c_plusplus
				);

char		*mach_error_type(
/*
 *	Returns a string with the error system, subsystem and code
*/
#if	c_plusplus
	mach_error_t	error_value
#endif  c_plusplus
				);

#endif	_LIB_MACH_ERROR_H_
