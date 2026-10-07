/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
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
 *      Copyright 1992 Intel Corporation.
 */

/*
 * HISTORY
 * $Log: estat.h,v $
 * Revision 1.3  1994/11/18  20:40:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  22:41:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/10  15:52:22  cfj
 * Put into NX branch.
 *
 * Revision 1.1  1992/11/05  22:39:27  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.1  1992/10/22  15:32:44  dbm
 * New for PFS functionality.
 *
 * Revision 1.1  1992/07/31  11:43:33  brad
 * Initial revision
 *
 *
 */
#ifndef _SYS_ESTAT_H_
#define _SYS_ESTAT_H_

/*
 * Extended file structure definitions.
 *
 */

#include <standards.h>
#include <sys/types.h>
#include <sys/mode.h>

#ifndef	_ESIZE_T
#define	_ESIZE_T
/*
 * Extended size structure.
 *
 */
struct s_size {
	long	slow;
	long	shigh;
};
typedef struct s_size esize_t;
#endif

/*
 * Extended stat structure.
 *
 */
struct estat {
	dev_t	st_dev;		/* ID of device containing a directory */
				/*   entry for this file.  File serial */
				/*   no + device ID uniquely identify */
				/*   the file within the system */
	ino_t	st_ino;		/* File serial number */
	mode_t	st_mode;	/* File mode; see #define's below */
	nlink_t	st_nlink;	/* Number of links */
	uid_t	st_uid;		/* User ID of the file's owner */
	gid_t	st_gid;		/* Group ID of the file's group */
	dev_t	st_rdev;	/* ID of device */
				/*   This entry is defined only for */
				/*   character or block special files */
	esize_t	st_size;	/* File size in bytes */
	time_t	st_atime;	/* Time of last access */
	int	st_spare1;
	time_t	st_mtime;	/* Time of last data modification */
	int	st_spare2;     
	time_t	st_ctime;	/* Time of last file status change */
	int	st_spare3;
				/* Time measured in seconds since */
				/*   00:00:00 GMT, Jan. 1, 1970 */
	ulong_t	st_blksize;	/* Size of block in file */
	long	st_blocks;	/* blocks allocated for file */
	ulong_t	st_flags;	/* user defined flags for file */
	ulong_t	st_gen;		/* file generation number */

};

#endif /* _SYS_ESTAT_H_ */
