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
#ifndef lint
static char rscid[] = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libc_r/longjmp.c,v 1.2 1994/11/19 02:15:11 mtm Exp $";
#endif

/*
 * Thread safe version of longjmp() that don't restore the
 * signal mask
 */

#include <setjmp.h>

#ifdef longjmp
#undef longjmp
#endif

void
longjmp(jmp_buf env, int val)
{
	_longjmp(env, val);
}
