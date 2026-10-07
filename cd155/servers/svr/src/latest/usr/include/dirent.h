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
 * $Log: dirent.h,v $
 * Revision 1.4  1994/11/18  21:09:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/07/19  01:54:24  dbm
 * Added new definition MAXDIRBUF = 65536 to be used by opendir().  It
 * previously used MAXBSIZE which is now too big.
 *
 *  Reviewer:None
 *  Risk:Low
 *  Benefit or PTS #:10129
 *  Testing: Specific test case.
 *  Module(s):
 * 	dirent.h
 * 	opendir.c (libc)
 *
 * Revision 1.2  1992/11/30  23:13:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:20  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:04  nandy
 * Initial revision
 *
 * Revision 1.11  92/04/13  14:35:05  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.11  90/10/31  15:15:13  devrcs
 * 	Backed out name-space changes.  Kept d_name size = 256.
 * 	[90/10/10  17:17:47  rabin]
 * 
 * 	Change dummy name size back to 256.  As long as the kernel
 * 	(and fsck) include this file (which should really be
 * 	fixed), the size of the struct dirent must be large
 * 	enough to include a pathname component (255).
 * 	[90/10/08  15:32:47  gmf]
 * 
 * 	Fix DIRSIZ macro definition to account for d_name change.
 * 	[90/10/08  08:14:02  tmt]
 * 
 * 	Cleaned up name-space pollution.  Controlled visibility of struct
 * 	dirent members and added "__" prefix to struct DIR members.
 * 	Changed length of d_name to 4, since NAME_MAX need not be defined,
 * 	and the type struct dirent should only be used with pointers.  The
 * 	actual length will depend on the file system.  The also allows the
 * 	removal of #include limits.h, which caused name-space pollution.
 * 	[90/10/06  16:23:44  rabin]
 * 
 * Revision 1.10  90/10/07  20:26:55  devrcs
 * 	Removed CMU conditionals.
 * 	[90/10/01  15:57:52  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  20:44:38  gm]
 * 
 * Revision 1.9  90/08/24  13:52:34  devrcs
 * 	Removed the conditional define arround the dd_lock field of the DIR
 * 	structure.
 * 	Added declarations of readdir_r() conditionaly on _REENTRANT or
 * 	_THREAD_SAFE.
 * 	[90/08/17  16:03:38  encore]
 * 
 * 	Moved #define d_fileno to OSF_SOURCE.
 * 	[90/08/16  13:28:19  sgrainge]
 * 
 * 	Inverted  d_fileno and d_ino in #define.
 * 	[90/08/16  12:28:55  sgrainge]
 * 
 * 	Changed d_fileno in dirent structure to be a
 * 	d_ino, it is declared as ino_t not a ulong_t
 * 	as specified in XPG3. Also, moved the #define
 * 	for d_fileno for backward compatability.
 * 	[90/08/16  10:43:40  sgrainge]
 * 
 * 	Delete prototypes for opendir_r and readdir_r
 * 	Add lock field to a DIR structure
 * 	[90/08/13  15:33:49  sp]
 * 
 * 	Delete prototypes for opendir_r and readdir_r
 * 	Add lock field to a DIR structure
 * 	[90/08/13  15:33:49  sp]
 * 
 * Revision 1.8  90/08/09  14:26:07  devrcs
 * 	Change prototype for opendir_r.
 * 	[90/08/02  11:37:57  sp]
 * 
 * 	Changes for reentrant C library
 * 	[90/08/02  09:55:29  sp]
 * 
 * Revision 1.7  90/07/17  12:24:44  devrcs
 * 	Changed include to <sys/limits.h>.
 * 	[90/07/07  08:34:26  gm]
 * 
 * 	Removed definitions that should only be visible to the kernel and
 * 	added _KERNEL conditionals so that this file can be included into
 * 	kernel source files.
 * 	[90/07/06  13:37:08  gm]
 * 
 * Revision 1.6  90/07/06  00:14:31  devrcs
 * 	Changes for making gcc the default compiler.
 * 	[90/07/01  18:40:27  gm]
 * 
 * 	remove that BSD change
 * 	[90/06/27  09:08:50  mbrown]
 * 
 * Revision 1.5  90/06/29  14:13:52  devrcs
 * 	ifdef BSD for direct defines
 * 	[90/06/26  12:54:47  mbrown]
 * 
 * Revision 1.4  90/06/22  21:51:39  devrcs
 * 	fixed typed to use POSIX types in dirent struct.
 * 	[90/06/15  16:55:45  tom]
 * 
 * Revision 1.3  90/04/14  00:45:56  devrcs
 * 	change to prototype for opendir
 * 	[90/03/28  14:23:05  mbrown]
 * 
 * Revision 1.2  90/03/13  21:22:01  mbrown
 * 	replace DIRBLKSIZ in _dirdesc for POSIX
 * 	[90/02/26  20:20:58  mbrown]
 * 
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:08:57  tom]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)dirent.h	1.28  com/inc/sys,3.1,8943 10/18/89 17:01:47 */
/*
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 */
#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <standards.h>
#include <sys/types.h>


#ifdef _POSIX_SOURCE
/*
 * The POSIX standard way of returning directory entries is in directory entry
 * structures, which are of variable length.  Each directory entry has
 * a struct direct at the front of it, containing its inode number,
 * the length of the entry, and the length of the name contained in
 * the entry.  These are followed by the name padded to a 4 byte boundary
 * with null bytes.  All names are guaranteed null terminated.
 * The maximum length of a name in a directory is returned by
 * pathconf(directory_pathname, _PC_NAME_MAX).  The type struct dirent is
 * a template for the beginning of the actual structure, since the
 * length is variable, and so should only be used to declare
 * struct dirent * pointer types.
 */

struct  dirent {
        ino_t    d_ino;               /* file number of entry */
        ushort_t d_reclen;            /* length of this record */
        ushort_t d_namlen;            /* length of string in d_name */
        char    d_name[256];		/* DUMMY NAME LENGTH */
					/* the real maximum length is */
					/* returned by pathconf() */
					/* At this time, this MUST */
					/* be 256 -- the kernel */
					/* requires it */
};

#ifndef _KERNEL
/*
 * Definitions for library routines operating on directories.
 */
typedef struct dirdesc {
        int     dd_fd;        /* file descriptor associated with directory */
        long    dd_loc;       /* offset in current buffer */
        long    dd_size;      /* amount of data returned by getdirentries */
        char    *dd_buf;      /* data buffer */
	int	dd_bufsiz;
        int     dd_len;       /* size of data buffer */
        long    dd_seek;      /* magic cookie returned by getdirentries */
	void	*dd_lock;	/* for inter-thread locking */
} DIR;

#ifdef _NO_PROTO
extern	DIR *opendir();
extern	struct dirent *readdir();
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern int readdir_r();
#endif
extern	int closedir();
extern  void rewinddir();

#else /* _NO_PROTO */

extern  DIR *opendir(const char *);
extern  struct dirent *readdir(DIR *);
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern int readdir_r(DIR *, struct dirent *);
#endif
extern  int closedir(DIR *);
extern  void rewinddir(DIR *);

#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif /* _POSIX_SOURCE */

#ifdef _XOPEN_SOURCE

#ifndef _KERNEL
#ifdef _NO_PROTO 
extern	long telldir();
extern	void seekdir();
#else /* _NO_PROTO */
extern void seekdir(DIR *, long );
extern long telldir(DIR *);
#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE
#define MAXDIRBUF	65536		/* Defines maximum buffer size to use
					 * when using the opendir() function
					 * to read directories.
					 */
#define d_fileno d_ino          /* backward compatibility */

/* For BSD compatibility */
#ifdef _BSD

#ifndef _KERNEL
#define direct dirent		/* backward compatibility */
#define MAXNAMLEN	NAME_MAX
#define dirfd(dirp)     ((dirp)->dd_fd)
#endif	/* _KERNEL */

/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in struct dirent
 * without the d_name field, plus enough space for the name with a terminating
 * null byte (dp->d_namlen+1), rounded up to a 4 byte boundary.
 * XXX The "256" below (the size of d_name) is subject to compiler alignment.
 */
#define DIRSIZ(dp) \
    ((sizeof (struct dirent) - 256) + (((dp)->d_namlen+1 + 3) &~ 3))

#ifndef _KERNEL
#ifdef _NO_PROTO
extern int scandir();
#else
extern int scandir(const char *, struct dirent ***, int (*)(), int (*)());
#endif
#endif /* _KERNEL */

#endif /* _BSD */

#endif /* _OSF_SOURCE */

#endif /* _DIRENT_H_ */
