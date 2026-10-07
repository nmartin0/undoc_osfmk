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
/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)bcmp.c	5.1 (Berkeley) 1/27/87";
#endif LIBC_SCCS and not lint

/*
 * bcmp -- vax cmpc3 instruction
 */
bcmp(b1, b2, length)
	register char *b1, *b2;
	register int length;
{

	if (length == 0)
		return (0);
	do
		if (*b1++ != *b2++)
			break;
	while (--length);
	return(length);
}
