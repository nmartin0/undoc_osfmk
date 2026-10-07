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
static char rcsid[] = "@(#)$RCSfile: lsearch.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:57 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: lsearch
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
 * lsearch.c	1.9  com/lib/c/gen,3.1,8943 10/12/89 10:09:09
 */

#include <memory.h>			/* for memcpy */


/*
 * NAME:	lsearch
 *                                                                    
 * FUNCTION:	search/maintain a linear list.
 *                                                                    
 * NOTES:	Linear search algorithm, generalized from Knuth
 *		(6.1) Algorithm Q.  This version no longer has anything
 *		to do with Knuth's Algorithm Q, which first copies the
 *		new element into the table, then looks for it.  The
 *		assumption there was that the cost of checking for the
 *		end of the table before each comparison outweighed the
 *		cost of the comparison, which isn't true when an arbitrary
 *		comparison function must be called and when the copy itself
 *		takes a significant number of cycles.  Actually, it has
 *		now reverted to Algorithm S, which is "simpler."
 *
 *		'Compar' is a bsearch-type function.
 *		
 * RETURN VALUE DESCRIPTION:	either a pointer to the value it
 *		found within the list, or a pointer to the value it
 *		added within the list
 */  

/*
 * This function is now declared as specified by the XOPEN standard.
 */

typedef char *POINTER;			/* pointer type for lsearch() */

void *
lsearch(key, base, nelp, width, compar)
void *key;			/* Key to be located */
void *base;			/* Beginning of table */
size_t *nelp;			/* Pointer to current table size */
size_t width;			/* Width of an element (bytes) */
int (*compar)();		/* Comparison function */
{
	POINTER next = (char *)base + *nelp * width;	/* End of table */

	for ( ; (POINTER)base < next; base = (char *)base + width)
		if ((*compar)(key, base) == 0)
			return (base);	/* Key found */
	++*nelp;			/* Not found, add to table */

	return (memcpy(base, key, (size_t)width));	/* base now == next */
}
