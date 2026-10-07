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
static char rcsid[] = "@(#) $RCSfile: devtolin.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:41 $";
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
static char sccsid[] = "devtolin.c	1.1  com/cmd/acct/lib,3.1,8935 5/15/89 17:16:35";
#endif
/*
 *	convert device to linename (as in /dev/linename)
 *	return ptr to LSZ-byte string, "?" if not found
 *	device must be character device
 *	maintains small list in tlist for speed
 */

#include <sys/types.h>
#include "acctdef.h"
#include <stdio.h>
#include <sys/dir.h>

#define TSIZE1	50	/* # distinct names, for speed only */
static	tsize1;
static struct tlist {
	char	tname[LSZ];	/* linename */
	dev_t	tdev;		/* device */
} tl[TSIZE1];

static struct dirent *d;

extern dev_t	lintodev();

char *
devtolin(device)
dev_t device;
{
	register struct tlist *tp;
	DIR *fdev;

	for (tp = tl; tp < &tl[tsize1]; tp++)
		if (device == tp->tdev)
			return(tp->tname);

	if ((fdev = opendir("/dev")) == NULL)
		return("?");
	while ( (d = readdir(fdev)) != NULL)
		if (lintodev(d->d_name) == device) {
			if (tsize1 < TSIZE1) {
				tp->tdev = device;
				(void)CPYN(tp->tname, d->d_name);
				tsize1++;
			}
			closedir(fdev);
			return(d->d_name);
		}
	closedir(fdev);
	return("?");
}
