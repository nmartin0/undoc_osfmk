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
 * Copyright (c) 1984 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ttyslot.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:18 $";
#endif

/* @(#)ttyslot.c	5.2 (Berkeley) 3/9/86 */

/*
 * Return the number of the slot in the utmp file
 * corresponding to the current user: try for file 0, 1, 2.
 * 
 */
#include <utmp.h>

char	*ttyname();
char	*rindex();
struct utmp *getutent(void);

#define	NULL	0

ttyslot()
{
	register struct utmp *ubuf;
	register char *tp, *p;
	register s;

	if ((tp = ttyname(0)) == NULL &&
	    (tp = ttyname(1)) == NULL &&
	    (tp = ttyname(2)) == NULL)
		return(0);
	if ((p = rindex(tp, '/')) == NULL)
		p = tp;
	else
		p++;
	setutent();
	s = 0;
	while ((ubuf = getutent()) != NULL) {
		if ((ubuf->ut_type == INIT_PROCESS ||
		     ubuf->ut_type == LOGIN_PROCESS ||
		     ubuf->ut_type == USER_PROCESS ||
		     ubuf->ut_type == DEAD_PROCESS ) &&
		     strncmp(p, ubuf->ut_line, sizeof(ubuf->ut_line)) == 0){

			endutent();
			return(s);
		}
		s++;
	}
	endutent();
	return (0);
}
