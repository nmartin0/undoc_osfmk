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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.3
 */
/*
 * This file contains the POSIX (P1003.4a) functions for locking and unlocking
 * of stdio file descriptors.
 */

#ifndef lint
static char rcs_id[] = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libc_r/flockfile.c,v 1.2 1994/11/19 02:15:08 mtm Exp $";
#endif

#include <stdio.h>
#include "stdio_lock.h"

#undef flockfile
#undef funlockfile

void
flockfile(FILE *iop)
{
	if (iop->_lock != (void *) NULL) {
		rec_mutex_lock(iop->_lock);
	}
}

void
funlockfile(FILE *iop)
{
	if (iop->_lock != (void *) NULL) {
		rec_mutex_unlock(iop->_lock);
	}
}
