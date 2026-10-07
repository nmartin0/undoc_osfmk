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
static char rcsid[] = "@(#)$RCSfile: mount.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:37:08 $";
#endif

/*
 * FUNCTIONS: mount
 *
 * DESCRIPTION:
 *	System V compatible mount.  Currently this assumes that the caller
 * wants to mount a BSD file system.  To change this default, change the
 * ufs_args structure and the MOUNT_UFS argument to the appropriate file
 * system's definition.
 */

#include <sys/syscall.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int
mount(spec, dir, rwflag)
	char	*spec;
	char	*dir;
	int	rwflag;
{
struct	ufs_args	args;
	caddr_t		argp;
	int		mntflags = 0;

	/*
	 * SVVS fix: 'Mounting a named special file that does not exist.'
	 *	mount syscall returns ENODEV where ENOENT is wanted.
	 */
	{	struct stat sb;
		extern int errno; /* pthread interference warning! */

		if (stat(spec, &sb) == -1 && errno == ENOENT)
			return(-1);
	}

	args.fspec = spec;
	args.exflags = 0;
	args.exroot = 0;
	argp = (caddr_t)&args;

	if (rwflag & 1)
		mntflags = M_RDONLY;

	return(syscall(SYS_mount, MOUNT_UFS, dir, mntflags, argp));
}
