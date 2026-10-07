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
static char rcsid[] = "@(#)$RCSfile: toupper.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:15 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: toupper
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * toupper.c	1.13  com/lib/c/gen,3.1,9013 2/28/90 10:25:26
 */

#include <ctype.h>
#include <NLctype.h>
#include <sys/localedef.h>

/*
 * The ANSI C standard defines several functions useful for testing and
 * mapping characters.  The behavior of these functions is affected by
 * the current locale.
 *
 * FUNCTION:	If arg is lower-case, return upper-case, otherwise return arg.
 *                                                                    
 * RETURN VALUE DESCRIPTION:	returns upper case of the arg if the
 *		args is lower case, else returns the arg
 */

#ifdef toupper
#undef toupper
#endif
#ifdef islower
#undef islower
#endif

int
toupper(int c)
{
#ifdef KJI
	if ( (unsigned int)(c) > 0xff ) {
		if ( isjlower(c) ) c = tojupper(c);
		return(c);
	}
	if ( islower(c) ) c -= ( 'a' - 'A');
	return(c);
#else
	if (!islower(c))
		return(c);
	else
		return(((_locp->lc_chrtbl)->lc_caseconv+1)[c]); 
#endif
}
