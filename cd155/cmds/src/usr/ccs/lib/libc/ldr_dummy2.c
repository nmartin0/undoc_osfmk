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
static char rcsid[] = "@(#)$RCSfile: ldr_dummy2.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:46 $";
#endif
/* ldr_dummy2.c
 * Dummies of the exported loader system call ldr_atexit(), for static libc
 *
 * This file contains dummy versions of the exported loader system
 * calls.  It is intended to be included in a statically-linked
 * version of the C library, from which the loader functions are
 * not available.
 *
 */

#include <sys/types.h>
#include <errno.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>


ldr_context_t	ldr_process_context = NULL;

int
ldr_context_atexit(ldr_context_t ctxt)

/* Run any per-module termination routines for this process' loader
 * context.  Intended to be called only from the exit() procedure
 * during normal process exit.  Returns LDR_SUCCESS on success,
 * or a negative error status on error.
 */
{
	return(0);
}
