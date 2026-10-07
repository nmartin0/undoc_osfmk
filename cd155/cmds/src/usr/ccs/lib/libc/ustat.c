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
static char rcsid[] = "@(#)$RCSfile: ustat.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:25 $";
#endif

/*
 * ustat (from SVID) as a library routine in OSF/1.
 * Algorithm:
 *	Collect statfs information on all mounted filesystems.
 *	Find the one corresponding to the device number passed in, and
 *	if there's a match, fill in the buffer supplied.
 * Return values:
 *	 0 upon success
 *	-1 upon failure (errno set to EINVAL)
 * Deficiencies:
 *	Could put information in f_fname and f_fpack fields.
 */

#include <sys/param.h>
#include <sys/mount.h>
#include <ustat.h>
#include <errno.h>

ustat(dev, buf)
	dev_t dev;
	struct ustat *buf;
{
	int mntsize, i, ret = -1;
	struct statfs *mntbuf;

	mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
	for (i = 0; i < mntsize; i++) {
		register struct statfs *stp = &mntbuf[i];
		if (stp->f_fsid.val[0] == dev) {
			buf->f_tfree = stp->f_bavail;
			buf->f_tinode = stp->f_ffree;
			/*
			 * XXX
			 * Since the information in the f_fname and
			 * f_fpack are optional, we'll leave them
			 * blank.  We could put something useful in
			 * the f_fname field, but...
			 */
			buf->f_fname[0] = '\0';
			buf->f_fpack[0] = '\0';
			ret = 0;
			break;
		}
	}
	if (ret)
		errno = EINVAL;
	return(ret);
}
