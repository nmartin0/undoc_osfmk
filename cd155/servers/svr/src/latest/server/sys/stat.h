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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: stat.h,v $
 * Revision 1.5  1994/11/18  20:41:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:28:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:20:10  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:44:03  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:38  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:39  cfj
 * Bump major revision number.
 * Revision 1.1.1.1  1993/05/03  17:42:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1992/07/29  08:22:40  rabii
 * 	Ifdef _KERNEL the definition of the lstat prototype (pjg).
 *
 * Revision 2.3  92/04/05  17:01:18  pjg
 * 	Define struct devstat (durriya).
 * 
 * Revision 2.2  91/08/31  14:13:13  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:45:25  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  14:54:17  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:45:20  gm]
 * 
 * Revision 1.8  90/09/23  16:00:14  devrcs
 * 	added support for _AES_SOURCE
 * 	[90/09/07  14:36:56  rabin]
 * 
 * Revision 1.7  90/09/13  11:51:28  devrcs
 * 	Changed st_nlink to be nlink_t as per POSIX.
 * 	[90/08/24  16:00:57  lehotsky]
 * 
 * Revision 1.6  90/06/29  13:53:58  devrcs
 * 	Changed u_long to ulong_t in stat struct. (For POSIX).
 * 	[90/06/22  10:26:42  tom]
 * 
 * Revision 1.5  90/03/13  20:49:50  mbrown
 * 	Cleanup - removed AIX value defines for stat fields: st_dev and
 * 	st_flags
 * 	[90/02/22  09:32:05  tom]
 * 
 * 	AIX merge first cut.
 * 	[90/02/09  18:04:18  tom]
 * 
 * Revision 1.4  90/02/23  00:33:07  devrcs
 * 	Defined S_IFIFO and changed definition of S_ISFIFO.
 * 	[90/02/18  14:02:54  ers]
 * 
 * Revision 1.3  90/01/02  20:16:03  gm
 * 	     Fixes for first snapshot.
 * 
 * $EndLog$
 */
/* @(#)stat.h	1.16  com/inc/sys,3.1,8943 10/11/89 17:16:16 */
/*
 * COMPONENT_NAME: SYSLFS - Logical File System
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <standards.h>
#include <sys/types.h>
#include <sys/mode.h>

/*
 * POSIX requires that certain values be included in stat.h.  It also
 * requires that when _POSIX_SOURCE is defined only those standard
 * specific values are present.  This header includes all the POSIX
 * required entries.
 */

#ifdef _POSIX_SOURCE 

/*
 *	Stat structure
 *
 */

struct  stat
{
	dev_t	st_dev;			/* ID of device containing a directory*/
					/*   entry for this file.  File serial*/
					/*   no + device ID uniquely identify */
					/*   the file within the system */
	ino_t	st_ino;			/* File serial number */
	mode_t	st_mode;		/* File mode; see #define's below */
	nlink_t	st_nlink;		/* Number of links */
	uid_t	st_uid;			/* User ID of the file's owner */
	gid_t	st_gid;			/* Group ID of the file's group */
	dev_t	st_rdev;		/* ID of device */
					/*   This entry is defined only for */
					/*   character or block special files */
	off_t	st_size;		/* File size in bytes */
	time_t	st_atime;		/* Time of last access */
	int	st_spare1;
	time_t	st_mtime;		/* Time of last data modification */
	int	st_spare2;
	time_t	st_ctime;		/* Time of last file status change */
	int	st_spare3;
					/* Time measured in seconds since */
					/*   00:00:00 GMT, Jan. 1, 1970 */
	ulong_t	st_blksize;		/* Size of block in file */
        long    st_blocks;              /* blocks allocated for file */
        ulong_t  st_flags;               /* user defined flags for file */
        ulong_t  st_gen;                 /* file generation number */

};
			/* End of the stat structure */

#ifdef OSF1_ADFS
/*
 * the devstat strcuture is used in the devstat call to return information
 * for char and block special files. 
 */
struct devstat {
        node_t   dst_node;              /* node # of where the device lives */
        ulong_t  dst_mountid;           /* unique mountid, if the partition 
                                         * corr to this block spec. file is 
                                         * mounted. 0xffffffff otherwise.
                                         */
        ulong_t  dst_spare1;
        ulong_t  dst_spare2;
        ulong_t  dst_spare3;
};
#endif  

#ifndef _KERNEL
#ifdef _NO_PROTO
	extern int	mkdir(); 
	extern mode_t	umask(); 
	extern int	stat();
	extern int	fstat();
	extern int	chmod();
	extern int	mkfifo();
#else				/* use POSIX required prototypes */
	extern int	mkdir(char *, mode_t ); 
	extern mode_t	umask(mode_t ); 
	extern int	stat(char *, struct stat *);
	extern int	fstat(int , struct stat *);
	extern int	chmod(char *, mode_t );
	extern int	mkfifo(char *, mode_t );
#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif /* _POSIX_SOURCE */

#ifdef _AES_SOURCE

#ifndef _KERNEL
#ifdef _NO_PROTO
	extern int	lstat();
#else 
	extern int 	lstat(const char *, struct stat *);
#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif /* _AES_SOURCE */

#ifdef _OSF_SOURCE

#define S_BLKSIZE       512     /* block size used in the stat struct */

#endif /* _OSF_SOURCE */
#endif /* _SYS_STAT_H_ */
