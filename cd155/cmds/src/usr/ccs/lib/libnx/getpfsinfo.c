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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	Copyright 1993  Intel Corporation.
 *
 *
 * HISTORY:
 * $Log: getpfsinfo.c,v $
 * Revision 1.4  1994/11/19  02:29:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/06  01:07:07  brad
 * Removed dummy fsbuf arg to statpfs ... now pass NULL instead.  Cleaned
 * up line wraps.
 *
 * Revision 1.2  1993/04/02  22:41:59  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2  1993/03/11  00:27:07  dbm
 * Fixed up so that error returned -1, also made fixes so that proper name
 * returned when more than one PFS file system was mounted.
 *
 * Revision 1.1.2.1  1993/02/19  01:04:24  brad
 * Corrected argument list and header comment.
 *
 * Revision 1.1  1993/01/12  00:52:16  dbm
 * Initial revision
 *
 *
 * getpfsinfo.c
 *
 * Description:
 *	This function returns an array of pfsmntinfo structures describing
 *	the stripe attributes of each currently mounted PFS file system.
 *	The user supplied attrbufp parameter is set to a malloc()ed buffer
 *	containing the array.  All information is contained in a static area,
 *	so it must be copied if it is to be saved.
 *	
 * Parameters:
 *	attrbufp	Pointer to buffer in which the array of pfsmntinfo 
 *			structures is to be placed.
 *
 * Returns:
 *	If successful, the number of elements in the array pointed to by 
 *	attrbufp.  Otherwise -1.
 */

#include <sys/types.h>
#include <sys/mount.h>
#include <sys/errno.h>

#define PFS_DEF_STAT_SIZE 1024

/*
 * Return information about mounted PFS filesystems.
 */
int
getpfsinfo(attrbufp)
	struct pfsmntinfo **attrbufp;
{
	struct statfs *mntbuf;
	struct statfs *tmpbuf;
	struct pfsmntinfo *tmppfsptr;
	static char *pfsbuf;
	static int pfsbufsize;
	char *newbuf;
	int bufsize = 0;
	int mntsize, pfs_count;
	int bytes_remaining = 0;
	int i, j, offset, status;

	/*
	 * Get mount info for all mounted filesystems.
	 */
	if ((mntsize = getmntinfo(&mntbuf, MNT_NOWAIT)) < 0) {
		return(-1);
	}

	/*
	 * Determine the number of PFS filesystems and allocate initial space
	 * required.
	 */
        for (i=0, tmpbuf=mntbuf, pfs_count=0; i<mntsize; i++, tmpbuf++) {
                if (tmpbuf->f_type == MOUNT_PFS) {
                        pfs_count++;
                }
        }
	if ((pfsbufsize <= 0) &&
	    (pfsbuf = (char *)malloc(pfs_count * PFS_DEF_STAT_SIZE)) == NULL) {
		errno = ENOMEM;
		return(-1);
	}
	pfsbufsize = bytes_remaining = pfs_count*PFS_DEF_STAT_SIZE;

	/*
	 * Now loop again getting the actual data for each PFS filesystem.
	 */
	for (i=0, offset=0, tmpbuf=mntbuf; i<mntsize; i++, tmpbuf++) {
		if (tmpbuf->f_type == MOUNT_PFS) { 
			tmppfsptr = (struct pfsmntinfo *)&pfsbuf[offset];

			/* call statpfs if space remains for some data */
			if (bytes_remaining < sizeof(struct pfsmntinfo)) {
				/* allocate a new large buffer for data */
				newbuf = (char *)
					malloc(pfsbufsize + PFS_DEF_STAT_SIZE);
				if (newbuf == NULL) {
					errno = ENOMEM;
					return(-1);
				}

				/* copy old large buffer into new, free old */
				bcopy(pfsbuf, newbuf, offset);
				free(pfsbuf);
				pfsbuf = newbuf;
				pfsbufsize += PFS_DEF_STAT_SIZE;
				bytes_remaining += PFS_DEF_STAT_SIZE;
				tmppfsptr = (struct pfsmntinfo *)
					&pfsbuf[offset];
			}

			/* copy mount point pathname into pfsmntinfo */
			for (j=0; j < sizeof(tmpbuf->f_mntonname); j++) 
				pfsbuf[offset+j] = tmpbuf->f_mntonname[j];
			offset += sizeof(tmppfsptr->m_mntonname);
			bytes_remaining -= sizeof(tmppfsptr->m_mntonname);
			status = statpfs(tmpbuf->f_mntonname, NULL,
					 &pfsbuf[offset], bytes_remaining);
			if (status < 0)
				return(-1);

			/* make sure enough space was available to get all 
			   of this specific filesystems data */
			while (pfsbufsize <= 
			       (offset + tmppfsptr->m_statpfs.p_reclen)) {

				/* allocate a new large buffer for data */
				newbuf = (char *)
					malloc(pfsbufsize + PFS_DEF_STAT_SIZE);
				if (newbuf == NULL) {
					errno = ENOMEM;
					return(-1);
				}
				/* copy old large buffer into new, free old */
				bcopy(pfsbuf, newbuf, pfsbufsize);
				free(pfsbuf);
				pfsbuf = newbuf;
				pfsbufsize += PFS_DEF_STAT_SIZE;
				bytes_remaining += PFS_DEF_STAT_SIZE;

				/* repeat call to get PFS filesystem data */ 
				tmppfsptr = (struct pfsmntinfo *)
					&pfsbuf[offset];
				status = statpfs(tmpbuf->f_mntonname,
						 NULL,
						 &pfsbuf[offset],
						 bytes_remaining);
				if (status < 0)
					return(-1);
			}

			/* update counts and pointers for large data buffer */
			offset += tmppfsptr->m_statpfs.p_reclen;
			bytes_remaining = pfsbufsize-offset;
		}
	}

	*attrbufp = (struct pfsmntinfo *)pfsbuf;
	return(pfs_count);
}
