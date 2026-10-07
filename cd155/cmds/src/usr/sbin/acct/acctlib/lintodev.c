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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: lintodev.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:47 $";
#endif
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !lint
static char sccsid[] = "lintodev.c	1.1  com/cmd/acct/lib,3.1,8935 5/15/89 17:18:05";
#endif
/*
 *	convert linename to device
 *	return -1 if nonexistent or not character device
 */
#include "acctdef.h"
#include <sys/types.h>
#include <sys/stat.h>
extern char *strncpy();
static	char	devtty[5+LSZ+1]	= "/dev/xxxxxxxx";

dev_t
lintodev(linename)
char linename[LSZ];
{
	struct stat sb;
	(void)strncpy(&devtty[5], linename, LSZ);
	if (stat(devtty, &sb) != -1 && (sb.st_mode&S_IFMT) == S_IFCHR)
		return((dev_t)sb.st_rdev);
	return((dev_t)-1);
}
