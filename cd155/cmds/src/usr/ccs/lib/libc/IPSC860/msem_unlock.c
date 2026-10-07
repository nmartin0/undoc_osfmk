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
static char rcsid[] = "@(#)$RCSfile: msem_unlock.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 02:12:48 $";
#endif

#include <sys/types.h>
#include <errno.h>
#include <sys/mman.h>

int
msem_unlock(msemaphore *sem, int condition)
{
	
	if ((sem->msem_wanted == -1) ||
	    (!((condition == MSEM_IF_WAITERS) || (condition ==0)))) {
		errno = EINVAL;
		return(-1);
	}

	if ((condition == MSEM_IF_WAITERS) && (sem->msem_wanted == 0)) {
		errno = EAGAIN;
		return(-1);
	}
	sem->msem_state = 0;		/* Clear the lock. */
	
	if (sem->msem_wanted != 0) {	/* See if anyone is waiting. */
		return(mwakeup(sem));
	}
		
	return(0);
}
