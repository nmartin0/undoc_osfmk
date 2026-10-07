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
static char rcsid[] = "@(#)$RCSfile: whoami.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:47:33 $";
#endif
/*
 * COMPONENT_NAME: (CMDSTAT) status
 *
 * FUNCTIONS:
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
 * whoami.c	1.4  com/cmd/stat,3.1,9021 12/4/89 20:13:41 
 */

#include <pwd.h>
#include <locale.h>

#include "whoami_msg.h"

nl_catd catd;
#define MSGSTR(Num, Str) catgets(catd, MS_WHOAMI, Num, Str)

/*
 * whoami
 */

main()
{
	register struct passwd *pp;

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_WHOAMI,0);

	pp = getpwuid((uid_t)geteuid());
	if (pp == 0) {
		printf(MSGSTR(INTRUDER,"Intruder alert.\n"));
		exit(1);
	}
	printf("%s\n", pp->pw_name);
	exit(0);
}
