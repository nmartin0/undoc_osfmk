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
 
/*++ dir.4.2.h - 
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/dir.4.2.h,v $
 *
 * DESCRIPTION:
 *
 *	Emulate the Berkeley UNIX directory structure on top
 *	of the System V fixed name length directory structure.
 *
 *	The following comments describe the structure of a Berkeley
 *	UNIX directory:
 *
 *	A directory consists of some number of blocks of DIRBLKSIZ
 *	bytes, where DIRBLKSIZ is chosen such that it can be transferred
 *	to disk in a single atomic operation (e.g. 512 bytes on most machines).
 *
 *	Each DIRBLKSIZ byte block contains some number of directory entry
 *	structures, which are of variable length.  Each directory entry has
 *	a struct direct at the front of it, containing its inode number,
 *	the length of the entry, and the length of the name contained in
 *	the entry.  These are followed by the name padded to a 4 byte boundary
 *	with null bytes.  All names are guaranteed null terminated.
 *	The maximum length of a name in a directory is MAXNAMLEN.
 *
 *	The macro DIRSIZ(dp) gives the amount of space required to represent
 *	a directory entry.  Free space in a directory is represented by
 *	entries which have dp->d_reclen >= DIRSIZ(dp).  All DIRBLKSIZ bytes
 *	in a directory block are claimed by the directory entries.  This
 *	usually results in the last entry in a directory having a large
 *	dp->d_reclen.  When entries are deleted from a directory, the
 *	space is returned to the previous entry in the same directory
 *	block by increasing its dp->d_reclen.  If the first entry of
 *	a directory block is free, then its dp->d_ino is set to 0.
 *	Entries other than the first in a directory do not normally have
 *	dp->d_ino set to 0.
 *
 *
 *	Author:
 *	-------
 *	Kirk McKusick, University of California, Berkeley.
 *	March 10, 1982.
 *
 *	Modifications:
 *	--------------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	September 10, 1985.
 *
 *	  Changed "#include <dir.h>" to read '#include "dir.4.2.h"' since
 *	  we are after the Berkeley definition, and not System V.
 *
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.2 $ $Date: 1994/11/19 02:43:53 $ $State: Exp $)
 * $Log: dir.4.2.h,v $
 * Revision 1.2  1994/11/19  02:43:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:27  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:57:46  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)dir.4.2.h	1.2 (dir.4.2.h OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

/*
 * 
 *	Define the directory block allocation size in bytes.
 *
 *	WARNING:
 *	  Seven system types are defined:  BSD42, BSD43, SGI, SYS52,
 *	  ULTRIX, UNICOS, and UTS.  Accordingly, the flag: SYSTEM_TYPE
 *	  in the Makefile must be appropriately defined.
 *
 *
 */
#if	BSD42 | BSD43 | ULTRIX		/* Berkeley UNIX */
#define	DIRBLKSIZ	512
#else
#if	SGI				/* Silicon Graphics Iris UNIX */
#ifndef DEV_BSIZE
#define	DEV_BSIZE	512
#endif
#define DIRBLKSIZ DEV_BSIZE
#else
#if	SYS52 | OSF				/* AT&T UNIX */
#define	DIRBLKSIZ	512
#else
#if	UTS				/* Amdahl Unix */
#define	DIRBLKSIZ	4096
#else
#if	UNICOS				/* Cray UNIX */
#define	DIRBLKSIZ	4096
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#endif
#endif
#define	MAXNAMLEN	255

#if	BSD42 | BSD43 | ULTRIX | SYS52 | UTS | UNICOS | CRAY1 | OSF
struct	direct {
#if	BSD42 | BSD43 | ULTRIX | SYS52 | UTS | OSF
	unsigned long d_ino;		/* inode number of entry */
#else
#if	UNICOS
#if	CRAY1
	unsigned int d_ino;		/* inode number of entry */
	unsigned int d_dev;		/* Device number of entry */
#else
	ino_t	d_ino;
#endif
#else
BAD SYSTEM TYPE
#endif
#endif
	unsigned short d_reclen;	/* length of this record */
	unsigned short d_namlen;	/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
};

/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in struct direct
 * without the d_name field, plus enough space for the name with a terminating
 * null byte (dp->d_namlen+1), rounded up to a 4 byte boundary.
 */
#undef DIRSIZ
#define DIRSIZ(dp) \
    ((sizeof (struct direct) - (MAXNAMLEN+1)) + (((dp)->d_namlen+1 + 3) &~ 3))

#ifndef KERNEL
/*
 * Definitions for library routines operating on directories.
 */
typedef struct _dirdesc {
	int	dd_fd;
	long	dd_loc;
	long	dd_size;
	char	dd_buf[DIRBLKSIZ];
} DIR;
#ifndef NULL
#define NULL 0
#endif
extern	DIR *opendir();
extern	struct direct *readdir();
extern	long telldir();
extern	void seekdir();
#define rewinddir(dirp)	seekdir((dirp), (long)0)
extern	void closedir();
#endif KERNEL
#endif	BSD42 | BSD43 | ULTRIX | SYS52 | UTS | UNICOS | CRAY1

#ifdef SGI
struct	direct {
	u_long	d_ino;
	short	d_reclen;
	short	d_namlen;
	char	d_name[MAXNAMLEN + 1];
	/* typically shorter */
};

struct _dirdesc {
	int	dd_fd;
	long	dd_loc;
	long	dd_size;
	char	dd_buf[DIRBLKSIZ];
	struct	direct dd_direct;
};

/*
 * useful macros.
 */
#undef DIRSIZ
#define DIRSIZ(dp) \
    ((sizeof(struct direct) - MAXNAMLEN + (dp)->d_namlen + sizeof(ino_t) - 1) &\
    ~(sizeof(ino_t) - 1))
typedef	struct _dirdesc DIR;
#ifndef	NULL
#define	NULL	0
#endif

/*
 * functions defined on directories
 */

#ifdef SVR3

#define rewinddir(dirp)	BSDseekdir((dirp), 0)
#define opendir BSDopendir
#define readdir BSDreaddir
#define telldir BSDtelldir
#define seekdir BSDseekdir
#define closedir BSDclosedir

#else

#define rewinddir(dirp)	seekdir((dirp), 0)

#endif /* SVR3 */

extern DIR *opendir();
extern struct direct *readdir();
extern long telldir();
extern void seekdir();
extern void closedir();
#endif SGI
