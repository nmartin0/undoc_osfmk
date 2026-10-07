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
static char rcsid[] = "@(#)$RCSfile: rewinddir.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:54:38 $";
#endif

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/file.h>

#undef	rewinddir
/*
 * Since seekdir is unsafe, we need an explicit routine for rewinddir
 */
void
rewinddir(dirp)
	register DIR *dirp;
{
	long curloc, base, offset;
	struct direct *dp;
	extern long lseek();

	if (dirp == NULL || dirp->dd_buf == NULL)
		return;
	dirp->dd_loc = 0;
	(void) lseek(dirp->dd_fd, (off_t)0, L_SET);
}

