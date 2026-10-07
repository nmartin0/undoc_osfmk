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
static char rcsid[] = "@(#)$RCSfile: msem_remove.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 02:12:44 $";
#endif

#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>

int msem_remove(msemaphore *sem)
{
	int	wanted;

	if (sem->msem_wanted == -1) {
		errno = EINVAL;
		return(-1);
	}
	/*
	 * The wanted count is also used to show if the semaphore is removed.
	 * Save away whether there are waiters. We must mark the semaphore
	 * as removed before we wake up the waiters in case their priority is
	 * higher than ours
	 */
	wanted = sem->msem_wanted;
	sem->msem_wanted = -1;	
	if (wanted > 0)
		mwakeup(sem);

	sem->msem_state = -1;
	return(0);
}
