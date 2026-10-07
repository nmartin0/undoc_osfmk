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
/* ldr_atexit.c
 * Implementations of the ldr_atexit() routine
 *
 * ldr_atexit() is called from exit() to perform loader cleanup.
 * Specifically, it calls all the termination functions for each
 * loader object module.
 *
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ldr_atexit.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:45 $";
#endif

#include <sys/types.h>
#include <sys/errno.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>


int
ldr_atexit(void)

/* Run any per-module termination routines for this process' loader
 * context.  Intended to be called only from the exit() procedure
 * during normal process exit.  Returns 0 on success,
 * or a negative error status on error.
 */
{
	int			rc;

	rc = ldr_context_atexit(ldr_process_context);
	if (rc != 0)
		errno = ldr_status_to_errno(rc);
	return(rc);
}
