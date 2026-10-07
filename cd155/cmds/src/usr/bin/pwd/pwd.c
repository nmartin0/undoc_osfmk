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
static char rcsid[] = "@(#)$RCSfile: pwd.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:35:05 $";
#endif
/*
 * COMPONENT_NAME: (CMDFS) commands that deal with the file system
 *
 * FUNCTIONS: pwd
 *
 * ORIGINS: 3, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * pwd.c      1.5  com/cmd/fs/progs,3.1,9021 12/13/89 13:20:19";
 */

/*
 * Print working (current) directory
 */
#include <stdio.h>
#include <sys/limits.h>

#define	NLSKJI 1
#include <NLctype.h>
#include <NLchar.h>
#include <locale.h>

char *getcwd();

main()
{
	char pathname[PATH_MAX + 1];

	(void) setlocale (LC_ALL,"");

	if(getcwd(pathname,sizeof(pathname)) == 0) {
		perror("pwd");
		exit(1);
	}
	printf("%s\n", pathname);
	exit(0);
}
