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
 *
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: mknod.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:37:05 $";
#endif

/*
 * FUNCTIONS: mknod
 *
 * DESCRIPTION:
 *	SVID.3 compatible mknod.
 *	Currently picks up S_IFREG and S_IFDIR
 *	and passes off the rest to the mknod system call.
 */
#include <sys/syscall.h>
#include <sys/mode.h>
#include <fcntl.h>
#include <errno.h>

/*ARGSUSED*/
int
mknod(char *path, int mode, int dev)
{
	/* need to be root unless creating FIFO */
	if (!S_ISFIFO(mode) && geteuid()) {
		errno = EPERM;
		return (-1);
	}
	switch (mode & S_IFMT) {
	case S_IFDIR:			/* Directory file	*/
		return (mkdir(path,(mode & 07777)));

	case 0:				/* FALLTHROUGH */
	case S_IFREG:			/* Regular file		*/
		if ((dev = open(path, O_WRONLY|O_CREAT|O_EXCL,
				(mode & 07777))) == -1)
			return (-1);
		close(dev);
		return (0);

	default:			/* Everything Else	*/
		return (syscall(SYS_mknod, path, mode, dev));
	}
}
