/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright (c) 1991-1995 Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: rexeclp.c,v $
 * Revision 1.7  1995/02/02  00:14:48  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:56:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  18:52:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.4  1993/05/06  19:30:39  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:19:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/01/22  19:20:37  stefan
 * Merged Locus 01-20-93 code drop.
 *
 * Revision 1.2  1992/11/30  23:02:52  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:13:19  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:04:06  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:58:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.3  1992/12/18  11:48:38  jpaul
 * Redesigned using stdargs; RISC architechures make it impossible
 * to twiddle with the function paramaters expected to be on the stack.
 *
 * Revision 3.2  92/04/16  21:04:57  yazz
 * Moved node arg out of formal parameter list to avoid corrupting stack.
 * 
 * Revision 3.1  92/04/14  14:59:00  klh
 * Fix RCS Log.
 * 
 * $EndLog$
 */

#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#define _POSIX_SOURCE
#include <sys/limits.h>

/* Execute a process on a remote node.  Allow variable number of args.
 * Use stdargs for argument and environment processing. 
 * First argument is program name. Differs from execle function in 
 * that the last paramater (after the environment) must be a 
 * valid node number.  The program will search the environment for the
 * program name.
 */
int rexeclp(char *progname, 
		 ...)
{
	char 	*argarray[_POSIX_ARG_MAX];  /* indexes into arg/env arrays */
	int 	index = 0;
	int 	node;

	va_list ap;

	/* get all the arguments into argarray to pass, stop at null */
	va_start(ap, progname);
	while (argarray[index++] = va_arg(ap, char *))
		if (index >= _POSIX_ARG_MAX)
			return(E2BIG);

	node = va_arg(ap, int);

	va_end(ap);

	return(rexecve(progname, &argarray, NULL, node));

}
