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
static char rcsid[] = "@(#)$RCSfile: bsearch.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:43 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: bsearch
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
 * bsearch.c	1.11  com/lib/c/gen,3.1,8943 9/8/89 08:38:17
 */

#include <stdio.h> /* for size_t and NULL */

/*
 * FUNCTION:	Binary search algorithm, generalized from Knuth
 *		(6.2.1) Algorithm B.
 *
 * NOTES:	Bsearch searches 'base', an array of 'nmemb' objects
 *		of size 'size', for a member that matches 'key'.  The
 *		function pointed to by 'compar' is used for comparing
 *		'key' to an element of 'base'.  'Compar' is called with
 *		2 arguments, the first of which is the 'key' and second
 *		of which is an array member.  It must return:
 *			< 0: if the key compares less than the member
 *			= 0: if the key compares equal than the member
 *			> 0: if the key compares greater than the member
 *
 * RETURN VALUE DESCRIPTION:	A pointer is returned to the element
 *		in 'base' matching 'key'.  If 'key' cannot be found
 *		in 'base', NULL is returned.
 *
 */  

void *
bsearch(const void *key, const void *base, size_t nmemb, size_t size,                   int(*compar)(const void *, const void *))
{
	size_t two_size = size + size;
	void *last = (char *)base + size * (nmemb - 1); /* Last element in table */

	while (last >= base) {
		void *p = (char *)base + size * (((char *)last - (char *)base)/two_size);
		int res = (*compar)(key, p);

		if (res == 0)
			return (p);	/* Key found */
		if (res < 0)
			last = (char *)p - size;
		else
			base = (char *)p + size;
	}
	return (NULL);		/* Key not found */
}
