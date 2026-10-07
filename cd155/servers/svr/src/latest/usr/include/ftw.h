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
 * $Log: ftw.h,v $
 * Revision 1.3  1994/11/18  21:09:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:21  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:27  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:54  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:33  nandy
 * Initial revision
 *
 * Revision 1.6  92/04/13  14:35:36  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.6  90/10/07  20:27:35  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:45:38  gm]
 * 
 * Revision 1.5  90/09/23  16:42:50  devrcs
 * 	added support for _AES_SOURCE
 * 	[90/09/07  14:37:03  rabin]
 * 
 * Revision 1.4  90/09/13  12:32:05  devrcs
 * 	Added FTW_SL (symlink type) for AES compliance
 * 	[90/08/31  03:05:14  glenn]
 * 
 * Revision 1.3  90/08/09  14:26:15  devrcs
 * 	Make insensitive to header-file inclusion order
 * 	[90/07/25  14:58:08  lehotsky]
 * 
 * Revision 1.2  90/03/13  21:22:20  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:21:08  tom]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)ftw.h	1.6  com/inc,3.1,8943 10/11/89 14:17:05 */
/*
 *	Codes for the third argument to the user-supplied function
 *	which is passed as the second argument to ftw
 */
#ifndef _FTW_H_
#define _FTW_H_

#include <standards.h>
#include <sys/stat.h>

#ifdef _XOPEN_SOURCE

#define	FTW_F	0	/* file */
#define	FTW_D	1	/* directory */
#define	FTW_DNR	2	/* directory without read permission */
#define	FTW_NS	3	/* unknown type, stat failed */

#ifdef _AES_SOURCE
#define FTW_SL	4	/* symbolic link */
#endif

#ifdef _NO_PROTO
extern int ftw();
#else
extern int ftw(char *, int (*fn)(char *,struct stat *, int), int );
#endif /* _NO_PROTO */

#endif /* _XOPEN_SOURCE */

#endif /* _FTW_H_ */
