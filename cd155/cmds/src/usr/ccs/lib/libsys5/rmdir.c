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
static char rcsid[] = "@(#)$RCSfile: rmdir.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:37:12 $";
#endif

/*
 * FUNCTIONS: rmdir
 *
 * DESCRIPTION:
 *	SVID2 compliant rmdir()
 */

#include	<sys/syscall.h>
#include	<errno.h>


int
rmdir(const char *path)
{
	int	ret_val;

	/*
	 * SVID2V3[RMDIR(BA_OS):p87]
	 * EEXIST: if the directory to be removed contains directory entries
	 * other than "." and "..".'
	 */
	if ((ret_val = syscall(SYS_rmdir, path)) == -1 && errno == ENOTEMPTY)
		errno = EEXIST;
	return (ret_val);
}
