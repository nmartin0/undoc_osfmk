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
static char rcsid[] = "@(#)$RCSfile: msem_init.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 02:12:43 $";
#endif

#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>

msemaphore *
msem_init(msemaphore *sem, int initial_value)
{
	switch (initial_value) {
	case	MSEM_LOCKED:
		sem->msem_state = (int) sem;
		break;
	case	MSEM_UNLOCKED:
		sem->msem_state = 0;
		break;
	default:
		errno = EINVAL;
		return(NULL);
	}
	sem->msem_wanted = 0;

	return(sem);
}
