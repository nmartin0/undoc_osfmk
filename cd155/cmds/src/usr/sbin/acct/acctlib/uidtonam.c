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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: uidtonam.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:53 $";
#endif
#if !lint
static char sccsid[] = "uidtonam.c	1.2  com/cmd/acct/lib,3.1,8935 7/11/89 11:17:55";
#endif
/*
 * convert uid to login name; interface to getpwuid that keeps up to USIZE1
 * names to avoid unnecessary accesses to passwd file
 * returns ptr to NSZ-byte name (not necessarily null-terminated)
 * returns ptr to "?" if cannot convert
 */

#include <sys/types.h>
#include "acctdef.h"
#include <stdio.h>
#include <pwd.h>

#define USIZE1	50
static	usize1;
static struct ulist {
	char	uname[NSZ];
	uid_t	uuid;
} ul[USIZE1];

char *
uidtonam(uid)
uid_t	uid;
{
	register struct ulist *up;
	register struct passwd *pp;

	for (up = ul; up < &ul[usize1]; up++)
		if (uid == up->uuid)
			return(up->uname);
	(void)setpwent();
	if ((pp = getpwuid(uid)) == NULL)
		return("?");
	else {
		if (usize1 < USIZE1) {
			up->uuid = uid;
			(void)CPYN(up->uname, pp->pw_name);
			usize1++;
		}
		return(pp->pw_name);
	}
}
