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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: umount.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:37:13 $";
#endif

/*
 * FUNCTIONS: umount
 *
 * DESCRIPTION:
 *	System V compatible umount.
 */

#include <sys/syscall.h>
#include <sys/mount.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

int
umount(char *spec)
{
struct	statfs	*mntbuf;
	int	mntsize;
	int	type;
	char	*dirname;
	int	i;

	/*
	 * SVVS fix: 'argument has a component that is not a directory'
	 * expects ENOTDIR, but gets EINVAL from getmntinfo/strcmp()
	 * below since it can't be mounted if it doesn't exist.
	 * Otherwise the unmount syscall would do the correct thing.
	 */
	{	struct stat sb;
		extern int errno;
		if (stat(spec,&sb) == -1 &&
				(errno == ENOTDIR || errno == ENOENT))
			return(-1);
	}

	if ((mntsize = getmntinfo(&mntbuf, MNT_NOWAIT)) == 0) {
		errno = ENXIO;
		return(-1);
	}

	for (i = 0; i < mntsize; i++) {
		if (!strcmp(mntbuf[i].f_mntfromname, spec)) {
			type = mntbuf[i].f_type;
			dirname = mntbuf[i].f_mntonname;
			break;
		}
	}
	if (i == mntsize) {
		errno = EINVAL;
		return(-1);
	}

	return(syscall(SYS_unmount, dirname, 0));
}
