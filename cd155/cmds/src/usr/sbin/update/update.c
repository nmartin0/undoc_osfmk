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
static char rcsid[] = "@(#)$RCSfile: update.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:18:44 $";
#endif
/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 * 
 * update.c	4.3 (Berkeley) 3/28/87
 */


/*
 * Update the file system every 30 seconds.
 */

#include <sys/time.h>
#include <sys/signal.h>


main()
{
	struct itimerval	value;
	extern int	sync();

	(void)close(0);
	(void)close(1);
	(void)close(2);
	(void)signal(SIGALRM, (void (*)(int))sync);
	value.it_interval.tv_sec = 30;
	value.it_interval.tv_usec = 0;
	value.it_value = value.it_interval;
	if (setitimer(ITIMER_REAL, &value, (struct itimerval *)NULL)) {
		perror("update: setitimer");
		exit(1);
	}
	for (;;)
		pause();
	/*NOTREACHED*/
}
