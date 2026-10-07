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
static char rcsid[] = "@(#)$RCSfile: msgs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:41:02 $";
#endif
/* 
 * COMPONENT_NAME: TCPIP msgs.c
 * 
 * FUNCTIONS: disp_msg, end_msgs, start_msgs 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/* msgs.c	1.3  com/sockcmd/talk,3.1,9021 10/8/89 17:29:10 */
/*
#ifndef lint
static char sccsid[] = "msgs.c	5.4 (Berkeley) 6/29/88";
#endif  not lint */

/* 
 * A package to display what is happening every MSG_INTERVAL seconds
 * if we are slow connecting.
 */

#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include "talk.h"

#define MSG_INTERVAL 4

char	*current_state;
int	current_line = 0;

disp_msg()
{

	message(current_state);
}

start_msgs()
{
	struct itimerval itimer;

	message(current_state);
	signal(SIGALRM, (void (*)(int))disp_msg);
	itimer.it_value.tv_sec = itimer.it_interval.tv_sec = MSG_INTERVAL;
	itimer.it_value.tv_usec = itimer.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &itimer, (struct itimerval *)0);
}

end_msgs()
{
	struct itimerval itimer;

	timerclear(&itimer.it_value);
	timerclear(&itimer.it_interval);
	setitimer(ITIMER_REAL, &itimer, (struct itimerval *)0);
	signal(SIGALRM, SIG_DFL);
}
