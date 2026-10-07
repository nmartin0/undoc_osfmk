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
static char rcsid[] = "@(#)$RCSfile: isgraph.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:18 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: isgraph
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
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
 * isgraph.c	1.11  com/lib/c/gen,3.1,9013 2/27/90 20:32:19
 */

#include <ctype.h>

/*
 *                                                                    
 * The ANSI C standard defines several functions useful for testing and
 * mapping characters.  The behavior of these functions is affected by
 * the current locale.
 * 
 * isgraph() tests for any printing character except space
 */

#ifdef isgraph
#undef isgraph
#endif

#define NLctyp 		(_locp->lc_chrtbl)->lc_ctype

int
#ifdef _NO_PROTO
isgraph(c)
int c;
#else
isgraph(int c)
#endif
{
	return( ((NLctyp+1)[c] & (_P|_A|_U|_L|_N|_G)) && !((NLctyp+1)[c] & _B));
}
