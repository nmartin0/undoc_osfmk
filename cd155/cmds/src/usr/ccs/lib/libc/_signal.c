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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: _signal.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:21 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: _signal
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * _signal.c	1.1  com/lib/c/gen,3.1,8943 9/29/89 11:13:12
 */


/*
 * NOTES: This provides Fortran with the _signal alias entry point
 *	for the signal function.  The normal signal code is included
 *	with __SIGNAL defined which causes it to compile as _signal.
 *	This approach was taken rather than calling signal from here
 *	to make sure Fortran links with this routine.  Otherwise, a
 *	relinked program could end up calling the wrong signal!
 */

#define __SIGNAL
#include "signal.c"
