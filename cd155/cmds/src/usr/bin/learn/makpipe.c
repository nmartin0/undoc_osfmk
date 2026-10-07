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
static char rcsid[] = "@(#)$RCSfile: makpipe.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:00 $";
#endif

/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: makpipe
 *
 * ORIGINS: 26, 27 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * makpipe.c	1.3  com/cmd/man/learn,3.1,9021 9/14/89 06:47:14
 */

#include "stdio.h"

#include "learn_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_LEARN,n,s) 

makpipe()
{
	int f[2];

	pipe(f);
	if (fork()==0) {
		close(f[1]);
		close(0);
		dup(f[0]);
		close(f[0]);
		execl ("/usr/bin/sh", "sh", "-i", 0);
		write(2, MSGSTR(LEXECERROR, "Exec error\n"), strlen(MSGSTR(LEXECERROR, "Exec error\n"))); /*MSG*/
	}
	close(f[0]);
	sleep(2);	/* so shell won't eat up too much input */
	return(f[1]);
}
