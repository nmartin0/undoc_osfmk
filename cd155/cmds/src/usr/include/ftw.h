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
