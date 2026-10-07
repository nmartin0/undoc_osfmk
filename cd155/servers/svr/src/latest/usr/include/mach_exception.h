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
 * $Log: mach_exception.h,v $
 * Revision 1.3  1994/11/18  21:10:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:15:24  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:47  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:24  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:36:51  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:30:40  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:50:09  gm]
 * 
 * Revision 1.2  90/01/02  19:16:14  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/12/26  10:10:26  gm
 * 	Current version from CMU.
 * 	[89/12/23            gm]
 * 
 * Revision 1.3  89/05/05  18:45:18  mrt
 * 	Cleanup for Mach 2.5
 * 
 * Revision 0.0  88/04/25            hauth
 * 	Created.
 * 	[88/04/25            hauth]
 * 
 * $EndLog$
 */ 

#ifndef	_MACH_EXCEPTION_H_
#define	_MACH_EXCEPTION_H_	1

#include <mach/kern_return.h>

char		*mach_exception_string(
/*
 *	Returns a string appropriate to the error argument given
 */
#if	c_plusplus
	int	exception
#endif	c_plusplus
				);


void		mach_exception(
/*
 *	Prints an appropriate message on the standard error stream
 */
#if	c_plusplus
	char	*str,
	int	exception
#endif	c_plusplus
				);


char		*mach_sun3_exception_string(
/*
 *	Returns a string appropriate to the error argument given
 */
#if	c_plusplus
	int	exception,
	int	code,
	int	subcode
#endif	c_plusplus
				);


void		mach_sun3_exception(
/*
 *	Prints an appropriate message on the standard error stream
 */
#if	c_plusplus
	char	*str,
	int	exception,
	int	code,
	int	subcode
#endif	c_plusplus
				);

char		*mach_romp_exception_string(
/*
 *	Returns a string appropriate to the error argument given
 */
#if	c_plusplus
	int	exception,
	int	code,
	int	subcode
#endif	c_plusplus
				);

void		mach_romp_exception(
/*
 *	Prints an appropriate message on the standard error stream
 */
#if	c_plusplus
	char	*str,
	int	exception,
	int	code,
	int	subcode
#endif	c_plusplus
				);

char		*mach_vax_exception_string(
/*
 *	Returns a string appropriate to the error argument given
 */
#if	c_plusplus
	int	exception,
	int	code,
	int	subcode
#endif	c_plusplus
				);

void		mach_vax_exception(
/*
 *	Prints an appropriate message on the standard error stream
 */
#if	c_plusplus
	char	*str,
	int	exception,
	int	code,
	int	subcode
#endif	c_plusplus
				);

#endif	_MACH_EXCEPTION_H_
