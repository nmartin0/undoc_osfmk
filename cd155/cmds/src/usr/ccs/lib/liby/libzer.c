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
 * Copyright (c) 1985 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ifndef lint
#	ifndef _NOIDENT
static char rcsid[] = "@(#) $RCSfile: libzer.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:38:56 $";
#	endif
#endif

/* static char sccsid[] = "@(#)libzer.c	5.1 (Berkeley) 8/9/85"; */

# include <stdio.h>

void yyerror(s)
	char *s;
{

	fprintf(stderr, "%s\n", s);
}
