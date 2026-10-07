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
static char rcsid[] = "@(#)$RCSfile: initgroups.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:13 $";
#endif
/*
 * COMPONENT_NAME: (LIBCADM) Standard C Library System Admin Functions 
 *
 * FUNCTIONS: initgroups 
 *
 * ORIGINS: 27 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * initgroups.c	1.6  com/lib/c/adm,3.1,8943 9/13/89 10:32:11
 */

#include <stdio.h>
#include <sys/param.h>
#include <grp.h>

#ifdef MSG
#include "libc_msg.h"
#endif

struct group *getgrent();

static int groups[NGROUPS];
int ngroups = 0;

/*                                                                    
 * FUNCTION: initialize group membership list
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - 0 on success
 *	     - 1 on failure
 */
/*
 *
 * initgroups
 */
initgroups(uname, agroup)
	char *uname;
	int agroup;
{
	register struct group *grp;
	register int i;

	if (agroup >= 0)
		groups[ngroups++] = agroup;
	setgrent();
	while (grp = getgrent()) {
		if (grp->gr_gid == agroup)
			continue;
		for (i = 0; grp->gr_mem[i]; i++)
			if (!strcmp(grp->gr_mem[i], uname)) {
				if (ngroups == NGROUPS) {
#ifdef MSG
					fprintf(stderr, NLgetamsg(MF_LIBC, MS_LIBC, M_INITG,
					   "initgroups: %s is in too many groups\n"), uname);
#else
					fprintf(stderr,
					   "initgroups: %s is in too many groups\n", uname);
#endif
					goto toomany;
				}
				groups[ngroups++] = grp->gr_gid;
			}
	}
toomany:
	endgrent();
	if (setgroups(ngroups, groups ) < 0) {
		perror("setgroups");
		return (1);
	}
	return (0);
}
