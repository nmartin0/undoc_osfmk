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
static char rcsid[] = "@(#) $RCSfile: namtouid.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:47 $";
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
static char sccsid[] = "namtouid.c	1.1  com/cmd/acct/lib,3.1,8935 5/15/89 17:18:19";
#endif
/*
 *	namtouid converts login names to uids
 *	maintains ulist for speed only
 */
#include <sys/types.h>
#include "acctdef.h"
#include <stdio.h>
#include <pwd.h>
extern char *strncpy();
static	usize;
static	struct ulist {
	char	uname[NSZ];
	uid_t	uuid;
} ul[A_USIZE];
char	ntmp[NSZ+1];

uid_t
namtouid(name)
char	name[NSZ];
{
	register struct ulist *up;
	register uid_t tuid;
	extern struct passwd *getpwnam();
	register struct passwd *pp;

	for (up = ul; up < &ul[usize]; up++)
		if (EQN(up->uname, name))
			return(up->uuid);
	(void)strncpy(ntmp, name, NSZ);
	(void)setpwent();
	if ((pp = getpwnam(ntmp)) == NULL)
		tuid = -1;
	else {
		tuid = pp->pw_uid;
		if (usize < A_USIZE) {
			(void)CPYN(up->uname, name);
			up->uuid = tuid;
			usize++;
		}
	}
	return(tuid);
}
