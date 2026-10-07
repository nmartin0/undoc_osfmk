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
static char rcsid[] = "@(#)$RCSfile: wwchild.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:49:13 $";
#endif
/*
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
 * 
 * wwchild.c	3.9 (Berkeley) 6/29/88
 */


#include <sys/types.h>
#include <sys/wait.h>
#include "ww.h"

wwchild()
{
	extern errno;
	int olderrno;
	register struct ww **wp;
	union wait w;
	int pid;
	char collected = 0;

	olderrno = errno;
	while ((pid = wait3(&w, WNOHANG|WUNTRACED, (struct rusage *)0)) > 0) {
		for (wp = wwindex; wp < &wwindex[NWW]; wp++) {
			if (*wp && (*wp)->ww_state == WWS_HASPROC
			    && (*wp)->ww_pid == pid) {
				(*wp)->ww_state = WWS_DEAD;
				collected = 1;
				break;
			}
		}
	}
	errno = olderrno;
	/* jump out of wwiomux when somebody dies */
	if (collected)
		wwsetintr();
}
