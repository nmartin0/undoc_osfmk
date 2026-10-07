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
 * Copyright (c) 1991..93 Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: rexecl.c,v $
 * Revision 3.4  1994/11/19  02:33:26  mtm
 * Copyright additions/changes
 *
 * Revision 3.3  1993/07/13  15:17:54  shala
 * New version from LOCUS.
 *
 * Revision 1.4  1993/05/06  19:30:29  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1993/01/22  19:22:11  stefan
 * Merged Locus 01-20-93 code drop.
 *
 * Revision 1.2  1992/11/30  23:02:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:13:15  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:03:52  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:58:24  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.3  1992/12/18  11:47:45  jpaul
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
 * Use stdargs for argument processing. 
 * First argument is program name. Differs from execl function in 
 * that the last paramater (after the paramaters) must be a 
 * valid node number.
 */
int rexecl(char *progname, 
		 ...)
{
	char 	*argarray[_POSIX_ARG_MAX];  /* arg/env arrays */
	char 	*envarray[_POSIX_ARG_MAX];
	int 	index = 0; /* index into arg/env arrays */
	int 	node;	   /* node to execute on */

	va_list ap;

	/* get all the arguments into argarray to pass, stop at null */
	va_start(ap, progname);
	while (argarray[index++] = va_arg(ap, char *))
		if (index >= _POSIX_ARG_MAX)  /* limit args per POSIX */
			return(E2BIG);

	/* get node from end of arg list */
	node = va_arg(ap, int);  /* caller must verify data types */

	va_end(ap);

	return(rexecve(progname, &argarray, NULL, node));

}
