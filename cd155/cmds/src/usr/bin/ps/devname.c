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
 * @OSF_COPYRIGHT@
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
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
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/*
 * HISTORY
 * $Log: devname.c,v $
 * Revision 1.5  1994/11/19  01:34:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1992/12/11  01:29:55  shala
 * Support new table call.
 *
 * Revision 2.2  92/06/10  17:36:00  pjg
 * 	Initial check-in in AD.
 * 
 * Revision 1.3  90/10/07  16:51:09  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  15:02:31  gm]
 * 
 * Revision 1.2  90/07/17  11:55:26  devrcs
 * 	New for ps.
 * 	[90/07/10  11:58:51  brezak]
 * 
 * $EndLog$
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: devname.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 01:34:40 $";
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include <paths.h>

#if defined(NLS) || defined(KJI)
#define	NLSKJI 1
#include <NLctype.h>
#include <NLchar.h>
#define printf NLprintf
#define fprintf NLfprintf
#endif
#include <locale.h>

#ifdef MSG
#include <nl_types.h>
#include "ps_msg.h"
extern nl_catd catd;
#define MSGSTR(Num,Str) NLcatgets(catd,MS_PS,Num,Str)
#else
#define MSGSTR(Num,Str) Str
#endif

struct devs {
	struct	devs *next;
	dev_t	dev;
	char	name[NAME_MAX+1];
	mode_t	type;
};

#define	hash(x)	((x)&0xff)
static struct devs *devhash[0xff];

static int devinit;

char *
devname(dev, type)
	dev_t dev;
	mode_t type;
{
	struct devs *devp;

	if (devinit == 0) {
		register struct devs *devpp;
		register struct dirent *entry;
		struct stat sb;
		DIR *dp = opendir(_PATH_DEV);
		int savewd = open(".", O_RDONLY, 0);
		mode_t specialtype;

		if (savewd == -1 || dp == NULL || chdir(_PATH_DEV) == -1)
			return (NULL);
		while ((entry = readdir(dp)) != NULL) {
			if (stat(entry->d_name, &sb) == -1)
				continue;
			switch(sb.st_mode&S_IFMT) {
			case S_IFCHR:
				specialtype = S_IFCHR;
				break;
			case S_IFBLK:
				specialtype = S_IFBLK;
				break;
			default:
				continue;
			}
			devp = (struct devs *)malloc(sizeof (struct devs));
			if (devp == NULL)
				return (NULL);
			devp->type = specialtype;
			devp->dev = sb.st_rdev;
			strcpy(devp->name, entry->d_name);
			devp->next = NULL;
			if ((devpp = devhash[hash(sb.st_rdev)]) == NULL)
				devhash[hash(sb.st_rdev)] = devp;
			else {
				for (;devpp->next != NULL; devpp = devpp->next)
					;
				devpp->next = devp;
			}
		}
		fchdir(savewd);
		close(savewd);
		closedir(dp);
		devinit = 1;
	}
	for (devp = devhash[hash(dev)]; devp != NULL; devp = devp->next)
		if (dev == devp->dev && type == devp->type)
			return(devp->name);

	return (NULL);
}

#ifdef TEST
main() {
	printf(" %s \n", devname(0));
}
#endif
