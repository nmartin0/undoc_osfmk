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
/* ldr_errno.c
 * error message handling for loader
 *
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ldr_status.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:47 $";
#endif

#include <sys/types.h>
#include <errno.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>


/* All loader internal errno's have magnitudes > SYSTEM_ERRNO_MAX */

#define	SYSTEM_ERRNO_MAX	0x3fffffff


/* This table translates internal loader status codes to errno's */

const static int stat2errno[] = {
	0,				/* 0 */
	EINVAL,				/* LDR_ENOMODULE */
	EINVAL,				/* LDR_ENOMAIN */
	ENOMEM				/* LDR_EALLOC */
	};

#define	LDR_MAXSTATUS	(sizeof(stat2errno) / sizeof(int))


int
ldr_status_to_errno(int rc)

/* Translate a loader error status to a system errno.  If loader error status
 * is non-negative, returns 0 (success); if negative and magnitude is less
 * than SYSTEM_ERRNO_MAX, returns negative of loader error status; otherwise
 * uses table lookup to translate.
 */
{
	if (rc >= 0)
		return(rc);
	else if (rc >= -SYSTEM_ERRNO_MAX)
		return(-rc);
	else {
		rc = -rc - SYSTEM_ERRNO_MAX;
		if (rc >= LDR_MAXSTATUS)
			return(EINVAL);
		return(stat2errno[rc]);
	}
}
