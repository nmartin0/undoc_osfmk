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
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: fcntl.h,v $
 * Revision 1.7  1994/11/18  20:40:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/14  17:42:47  brad
 * Added R1.3 support for getting, setting, and temporarily mapping
 * per-file PFS stripe attributes via new F_GETSATTR and F_SETSATTR
 * fcntl() requests.  Also added an F_GETFULLSATTR request for use by ls
 * in getting full (including stripe file pathnames) stripe attributes.
 * Also modified default attributes given a file when it is created: the
 * start stripe directory is now a random member of the stripe group,
 * rather than always the first member, so small files consume disk space
 * evenly across the stripe group.
 *
 *  Reviewer: rlg (in progress)
 *  Risk: Med
 *  Benefit or PTS #: 7593, 9686
 *  Testing: Developer tests, PFS EATs on 64 nodes
 *  Module(s): server/{sys/fcntl.h,pfs/pfs.h}
 *             emulator/{pfs_fdt.h,fsvr_user_side.c,pfs2_user_side.c}
 *
 * Revision 1.5  1994/03/18  19:04:52  cfj
 * Merged revision 1.4.4.1 from R1_2 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.4.4.1  1994/03/18  19:03:20  cfj
 * Moved the bit used for O_RMTDEV so that it no longer conflicts with
 * O_SYNC.
 *
 *  Reviewer:jlitvin
 *  Risk:L
 *  Benefit or PTS #:8527
 *  Testing:VSX EAT, test case
 *  Module(s):server/sys/fcntl.h
 *
 * Revision 1.4  1993/07/14  18:24:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:18:58  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:41:56  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:20  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:41:19  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.5  1993/03/10  10:47:23  mmp
 * 	Define FUNREFWAIT flag for close.
 *
 * Revision 2.4  93/01/06  10:38:03  loverso
 * 	Fix notice.
 * 
 * Revision 2.3  92/01/05  20:06:42  roy
 * 	1991/11/12  17:23:48  noemi
 * 	Added FNMSWAIT and O_RMTDEV definitions.
 * 
 * Revision 2.2  91/08/31  14:04:46  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:42:42  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11  90/10/31  14:06:29  devrcs
 * 	Seperate NDELAY and NONBLOCK.
 * 	[90/10/12  10:18:41  jvs]
 * 
 * 	Added F_ENFRCD for Xenix file locking compatibility.
 * 	[90/10/08  13:40:51  swallace]
 * 
 * Revision 1.10  90/10/07  14:50:12  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:38:50  gm]
 * 
 * Revision 1.9  90/09/23  15:59:45  devrcs
 * 	Deleted FFLCK.  The VLOCKS bit in the v_flag of the vnode keeps
 * 	track of locks on a file.
 * 	[90/09/06  14:37:20  swallace]
 * 
 * Revision 1.8  90/09/13  11:51:21  devrcs
 * 	Changed l_pid from short to pid_t (bug 313).
 * 	[90/08/29  16:27:42  collins]
 * 
 * Revision 1.7  90/08/24  12:26:07  devrcs
 * 	Corrected O_NOCTTY to be a proper octal value.  Also had to change
 * 	O_SYNC value.
 * 	[90/08/12  12:13:03  ers]
 * 
 * Revision 1.6  90/07/27  09:07:12  devrcs
 * 	Clone driver support, O_SYNC file writing.
 * 	[90/07/20  17:06:18  nags]
 * 
 * Revision 1.5  90/06/29  13:53:44  devrcs
 * 	Added FFLCK flag to mark locked files.
 * 	[90/06/26  11:26:01  gmf]
 * 
 * Revision 1.4  90/06/22  20:52:52  devrcs
 * 	nags merge
 * 	[90/06/12  21:38:13  gmf]
 * 
 * 	Changed l_pid to short in struct flock, as per SVID.  Eliminated
 * 	AIX-specific fields l_sysid and l_vfs.  Eliminated unused F* flag
 * 	definitions.
 * 	[90/06/06  13:04:49  ers]
 * 
 * Revision 1.3  90/04/27  19:25:46  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/18  11:46:23  tom]
 * 
 * Revision 1.2  90/03/13  20:49:09  mbrown
 * 	removed #ifdef _KERNEL from F____ ioctl macros
 * 	[90/03/01  18:20:53  mbrown]
 * 
 * 	Move FNDELAY and friends out from under #ifdef KERNEL.
 * 	[90/03/01  14:22:31  tom]
 * 
 * 	Added LOCK_x defines for OSF_SOURCE, from AIX flock.h.
 * 	[90/02/22  16:52:41  tom]
 * 
 * 	Added flock structure as it is not in flock.h for OSF/1.
 * 	This was moved from file.h.
 * 	[90/02/21  16:10:23  tom]
 * 
 * 	AIX merge first cut - new file.
 * 	[90/02/09  15:14:58  tom]
 * 
 * $EndLog$
 */
/* @(#)fcntl.h  1.20  com/inc,3.1,9013 3/9/90 18:13:41 */
/*
 * COMPONENT_NAME: SYSLFS - Logical File System
 *
 * ORIGINS: 27, 3, 26
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _SYS_FCNTL_H_
#define _SYS_FCNTL_H_


#include <standards.h>
#include <sys/types.h>

/*
 * POSIX requires that certain values be included in fcntl.h and that only
 * these values be present when _POSIX_SOURCE is defined.  This header
 * includes all the POSIX required entries.
 */

#ifdef _POSIX_SOURCE

struct  flock   {
        short   l_type;
        short   l_whence;
        off_t   l_start;
        off_t   l_len;          /* len = 0 means until end of file */
        pid_t   l_pid;
};
/* file segment locking set data type - information passed to system by user */

/* file segment locking types */
#define F_RDLCK 1       /* Read (shared) lock */
#define F_WRLCK 2       /* Write (exclusive) lock */
#define F_UNLCK 8       /* Remove lock(s) */
#ifdef i386
#define F_ENFRCD 4      /* Xenix enforcement mode lock(s) - used only by the
			   Xenix compatibility package */
#endif


#ifdef _NO_PROTO
extern int open();
extern int creat();
extern int fcntl();

#else 		/* use POSIX required prototypes */
#ifndef _KERNEL
extern int open(char *, int , ...);
extern int creat(char *, mode_t );
extern int fcntl(int , int ,...);
#endif /* _KERNEL */
#endif /* _NO_PROTO */

/* File status flags accessible to open(2) and fcntl(2) */
#define O_NONBLOCK 00000004	/* non-blocking I/O, POSIX style */
#define	O_APPEND   00000010  	/* append (writes guaranteed at the end) */

/* Mask for use with file access modes */
#define O_ACCMODE	3		

/* Flag values accessible to open(2) */
#define	O_RDONLY 00000000
#define	O_WRONLY 00000001
#define	O_RDWR	 00000002
 
/* Flag values accessible only to open(2) */
#define	O_CREAT	 00001000    /*  open with file create (uses third open arg)*/
#define	O_TRUNC	 00002000    /* open with truncation		*/
#define	O_EXCL	 00004000    /* exclusive open			*/
#define O_NOCTTY 00010000    /* POSIX REQUIRED */
#ifdef	_KERNEL
#define	O_DOCLONE 00020000    /* make a cloned device */
#endif
#ifdef	OSF1_ADFS
#define	O_RMTDEV  00200000    /* remote special device */
#endif

/* File descriptor flags used for fcntl() */
/* POSIX REQUIRED */
#define FD_CLOEXEC      1	/* Close this file during exec */

/* fcntl() requests */
#define	 F_DUPFD	0	/* Duplicate fildes		*/
#define	 F_GETFD	1	/* Get fildes flags		*/
#define	 F_SETFD	2	/* Set fildes flags		*/
#define	 F_GETFL	3	/* Get file flags		*/
#define	 F_SETFL	4	/* Set file flags		*/
#define	 F_GETLK	7	/* Get file lock	POSIX REQUIRED	*/
#define	 F_SETLK	8	/* Set file lock	POSIX REQUIRED	*/
#define	 F_SETLKW	9	/* Set file lock and waitPOSIX REQUIRED	*/

#endif /* _POSIX_SOURCE */

#ifdef _XOPEN_SOURCE

/* XOPEN REQUIRED  */
#define O_SYNC		00040000	/* synchronous write option */

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE

/* Additional fcntl requests */

#define	 F_GETOWN	5	/* get async I/O owner		*/
#define	 F_SETOWN	6	/* set async I/O owner		*/
#ifdef	PFS
#define	 F_GETSATTR	10	/* get user-settable stripe attributes	*/
#define	 F_SETSATTR	11	/* set user-settable stripe attributes	*/
#define	 F_GETFULLSATTR	12	/* get full stripe attributes		*/
#endif

/*
 * Flock call.
 */
#define LOCK_SH         F_RDLCK       /* shared lock */
#define LOCK_EX         F_WRLCK       /* exclusive lock */
#define LOCK_NB         4       	/* don't block when locking */
#define LOCK_UN         F_UNLCK       /* unlock */


/*
 * Flag values accessible to open(2) and fcntl(2)
 */

/* O_NDELAY was defined as O_NONBLOCK.  The two definitions have been
 * seperated but the code has been modified so both flags still exhibit
 * the same behavior.  They will exhibit seperate behaviors in a future
 * release
 */
#define O_DEFER		00000040	/* defered update		*/
#define O_NDELAY	00100000	/* Non-blocking I/O		*/


#define FNDELAY		O_NDELAY
#define	FAPPEND		O_APPEND
#define FASYNC		00100
#define	FCREAT		O_CREAT
#define	FTRUNC		O_TRUNC
#define	FSYNC		O_SYNC
#define	FEXCL		O_EXCL

#define	FNONBLOCK 	O_NONBLOCK
#define FDEFER		O_DEFER

/*
 * These may be AIX only values and should possibly be removed
 */

/* Flag values accessible only to open(2) */
#define	FOPEN		(-1)
#define	FREAD		00001
#define	FWRITE		00002

#define FMARK           00020           /* mark during gc() */
#define FSHLOCK         00200           /* shared lock present */
#define FEXLOCK         00400           /* exclusive lock present */

/* FFCNTL is all the bits that may be set via fcntl. */
#define	FFCNTL	(FNONBLOCK|FNDELAY|FAPPEND|FSYNC|FASYNC) 

/* bits to save after open */
#define FMASK           (FSYNC|FASYNC|FAPPEND|FNDELAY|FNONBLOCK|FWRITE|FREAD)
#define FCNTLCANT       (FREAD|FWRITE|FMARK|FDEFER|FSHLOCK|FEXLOCK)

#ifdef	OSF1_ADFS
/* Misc bits set by fileservers */
#define	FNMSWAIT	0x1000
#define	FUNREFWAIT	0x2000
#endif

#endif	/* _OSF_SOURCE	*/
#endif	/* _SYS_FCNTL_H_ */
