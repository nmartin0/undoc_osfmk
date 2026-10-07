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
static char rcsid[] = "@(#)$RCSfile: msem_lock.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 02:12:43 $";
#endif
#include <sys/types.h>
#include <errno.h>
#include <sys/mman.h>

int
msem_lock(msemaphore *sem, int condition)
{
	/*
	 * Check to see if the semaphore has been removed or if the
	 * condition parameter is bad.
	 */
	if ((sem->msem_wanted == -1) ||
	    (!((condition == MSEM_IF_NOWAIT) || (condition == 0)))) {
		errno = EINVAL;
		return(-1);
	}

	while (_msem_tas(&sem->msem_state) != 0) {

		if (condition == MSEM_IF_NOWAIT) {
			errno = EAGAIN;
			return(-1);
		}

		if (msleep(sem) != 0) {
			return(-1);
		}

		/*
		 * we have to check to see if the semaphore is still valid
		 * as someone may have removed it while we were asleep
		 */
		if (sem->msem_wanted == -1) {
			errno = EINVAL;
			return(-1);
		}
		
	}
	
	return(0);
}
