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
static char rcsid[] = "@(#)$RCSfile: rand.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:39 $";
#endif
/*
 * FUNCTIONS: rand, srand
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
 * rand.c	1.11  com/lib/c/gen,3.1,8943 9/9/89 09:27:25
 */

#include <stdlib.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex _rand_rmutex;
#endif

/*
 * NAME:	srand
 *
 * FUNCTION:	The srand function uses the arguement as a seed for a new
 *		sequence of pseudo-random numbers to be returned by 
 *		subsequent calls to rand.
 *                                                                    
 * RETURN VALUE DESCRIPTION:	
 *		returns no value
 *
 */                                                                   

static long randx = 1;

void	
srand(unsigned int seed)
{
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_rand_rmutex);
#endif

	randx = seed;

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_rand_rmutex);
#endif
}

/*
 * NAME:	rand
 *
 * FUNCTION:	The rand function computes a sequence of pseudo-random
 *		integers in the range 0 to RAND_MAX.
 *                                                                    
 * RETURN VALUE DESCRIPTION:	
 *		returns a psuedo-random integer
 *
 */                                                                   

int 
rand(void)
{
	int	retval;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_rand_rmutex);
#endif
	retval = ((randx = randx * 1103515245L + 12345)>>16) & RAND_MAX;

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_rand_rmutex);
#endif
	return(retval);
}

#ifdef _THREAD_SAFE
int
rand_r(unsigned int *seed, int *randval)
{
	if ((randval == NULL) || (seed == NULL)) {
		seterrno(EINVAL);
		return(-1);
	}
	*randval = ((*seed = *seed * 1103515245L + 12345)>>16) & RAND_MAX;
	return(0);
}
#endif
