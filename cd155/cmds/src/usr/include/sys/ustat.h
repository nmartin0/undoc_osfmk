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
/* @(#)ustat.h	1.5  com/inc/sys,3.1,8943 10/11/89 15:36:28 */

#ifndef _SYS_USTAT_H_
#define _SYS_USTAT_H_

#include <sys/types.h>

#define FS_NMLN		6

struct  ustat
{
	daddr_t	f_tfree;		/* total free */
	ino_t	f_tinode;		/* total inodes free */
	char	f_fname[FS_NMLN];	/* filsys name */
	char	f_fpack[FS_NMLN];	/* filsys pack name */
};

extern int ustat();

#endif /* _SYS_USTAT_H_ */

