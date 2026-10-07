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
 * $Log: mode.h,v $
 * Revision 1.4  1994/11/18  20:40:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:26:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:48  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:22  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:18  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:08:28  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:43:47  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:52:15  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:42:18  gm]
 * 
 * Revision 1.3  90/04/27  19:26:28  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/18  11:47:12  tom]
 * 
 * Revision 1.2  90/03/13  20:49:35  mbrown
 * 	Cleanup - move XOPEN defines to Xopen section,
 * 	removed AIX security defines
 * 	[90/02/22  09:29:37  tom]
 * 
 * 	AIX merge first cut - new file.
 * 	[90/02/09  15:17:54  tom]
 * 
 * $EndLog$
 */
/* @(#)mode.h   1.7  com/inc/sys,3.1,9013 3/2/90 12:50:34 */
/*
 * COMPONENT_NAME: mode.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   

#ifndef _SYS_MODE_H_
#define _SYS_MODE_H_


/*
 * POSIX requires that certain values effectively be included in stat.h.
 * It also requires that when _POSIX_SOURCE is defined, only those standard
 * specific values are present.  Since mode.h defines values on behalf of
 * stat.h and since stat.h includes mode.h, this header adheres to the
 * POSIX requirements.
 */

#include <standards.h>
#ifdef _POSIX_SOURCE 

/*
 *	(stat) st_mode bit values
 */

#define S_ISUID		0004000		/* set user id on execution */
#define S_ISGID		0002000		/* set group id on execution */

					/* ->>> /usr/group definitions <<<- */
#define S_IRWXU		0000700		/* read,write,execute perm: owner */
#define S_IRUSR		0000400		/* read permission: owner */
#define S_IWUSR		0000200		/* write permission: owner */
#define S_IXUSR		0000100		/* execute/search permission: owner */
#define S_IRWXG		0000070		/* read,write,execute perm: group */
#define S_IRGRP		0000040		/* read permission: group */
#define S_IWGRP		0000020		/* write permission: group */
#define S_IXGRP		0000010		/* execute/search permission: group */
#define S_IRWXO		0000007		/* read,write,execute perm: other */
#define S_IROTH		0000004		/* read permission: other */
#define S_IWOTH		0000002		/* write permission: other */
#define S_IXOTH		0000001		/* execute/search permission: other */

/*
 *	File type macros
 */

#define S_ISFIFO(m)	(((m)&(0170000)) == (0010000))
#define S_ISDIR(m)	(((m)&(0170000)) == (0040000))
#define S_ISCHR(m)	(((m)&(0170000)) == (0020000))
#define S_ISBLK(m)	(((m)&(0170000)) == (0060000))
#define S_ISREG(m)	(((m)&(0170000)) == (0100000))

#endif /* _POSIX_SOURCE */

#ifdef _XOPEN_SOURCE

/*
 *	Additional mode bit values
 *	
 */

#define S_IFMT		0170000		/* type of file */
#define   S_IFREG	0100000		/*   regular */
#define   S_IFDIR	0040000		/*   directory */
#define   S_IFBLK	0060000		/*   block special */
#define   S_IFCHR	0020000		/*   character special */
#define   S_IFIFO	0010000		/*   fifo */

#define S_ISVTX		0001000		/* save text even after use */
#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE

/*
 *	More mode bit values
 *	(Macros are separated because they're not strictly part of the standard)
 */


#define S_IFSOCK	0140000		/* socket */
#define S_IFLNK		0120000		/* symbolic link */

#define S_ENFMT		S_ISGID		/* record locking enforcement flag */

/*
 * These next three have been WITHDRAWN from the X/Open standard.
 * Use S_IRUSR, S_IWUSR, and S_XUSR instead.
 */

#define S_IREAD		S_IRUSR		/* read permission, owner */
#define S_IWRITE	S_IWUSR		/* write permission, owner */
#define S_IEXEC		S_IXUSR		/* execute/search permission, owner */

#define S_ISLNK(m)	(((m)&(S_IFMT)) == (S_IFLNK))
#define S_ISSOCK(m)	(((m)&(S_IFMT)) == (S_IFSOCK))


/*
 *	Equivalent mode macros (from ufs/inode.h)
 *	Removed -- use S_* instead of *
 *	eg. S_IFMT instead of IFMT.
 */


#endif /* _OSF_SOURCE */
#endif /* _SYS_MODE_H_ */
