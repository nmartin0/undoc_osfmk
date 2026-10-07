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
 * $Log: types.h,v $
 * Revision 1.11  1994/11/18  20:42:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1993/11/09  17:52:29  shala
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Bug #7093.
 *  Testing: Compile a test case.
 *  Module(s): svr/server/sys/types.h
 *
 * Defined ssize_t.
 *
 * Revision 1.9  1993/07/14  18:30:12  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.8  1993/07/09  20:22:59  brad
 * Added #define PFS for compiling applications.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:16  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/07  22:16:22  shala
 * Make wchar_t to be long instead of unsigned short to make it compatible
 * with PGI's C compiler.
 *
 * Revision 1.6  1993/05/07  14:36:59  cfj
 * Add a missing #ifdef  OSF1_ADFS.
 *
 * Revision 1.5  1993/05/06  19:20:48  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/01  21:17:37  cfj
 * Remove ifdef OSF1AD around node_t.
 *
 * Revision 1.3  1993/03/25  23:24:15  cfj
 * T9 Merge.
 *
 * Revision 1.2.8.1  1993/03/24  23:41:28  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.2  1992/11/30  22:45:09  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:44  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:48  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/10/22  15:35:08  dbm
 * Added changes for PFS.
 *
 * Revision 2.5  1993/03/03  17:13:29  mjl
 * Add #define constant INVALID_NODE, near node_t definition.
 *
 * Revision 2.4  92/01/05  20:09:47  roy
 * 	1991/11/12  17:29:08  noemi
 * 	Added node_t type.
 * 
 * Revision 2.3  91/11/13  14:44:27  rabii
 * 	Added i860 support
 * 	[Mon Oct 21 10:11:57 PDT 1991] cfj@ssd.intel.com
 *      Added i860 label_t definition.
 *
 * Revision 2.2  91/08/31  14:15:43  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/28  14:02:46  condict
 * Update to OSF 1.0.2.
 * 
 * Revision 1.13.2.4  91/03/11  15:06:07  sgrainge
 * 	 Removed ifdef for va_list.
 * 	[91/03/11  15:05:29  sgrainge]
 * 
 * Revision 1.13.2.3  90/12/20  15:01:45  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.13.2.2  90/12/14  15:58:26  dwm
 * 	POSIX bug 1678 - name space pollution
 * 	[90/12/13  13:14:05  dwm]
 * 
 * Revision 1.13  90/10/31  14:07:21  devrcs
 * 	Change the machine definitions to be ANSI compliant.
 * 	[90/10/22  15:01:31  collins]
 * 
 * Revision 1.12  90/10/07  14:55:58  devrcs
 * 	moved #define TRUE and FALSE under #ifdef _OSF_SOURCE
 * 	[90/10/03  17:16:25  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:48:00  gm]
 * 
 * Revision 1.11  90/08/24  12:27:46  devrcs
 * 	HP/Apollo M68K
 * 	[90/08/13  17:37:40  mcg]
 * 
 * 	Removed 386 Dead code
 * 	[90/08/10  08:37:54  kevins]
 * 
 * 	Removed 386 Dead code
 * 	[90/08/10  08:37:54  kevins]
 * 
 * Revision 1.10  90/08/09  13:29:19  devrcs
 * 	merged 386 changes
 * 	[90/08/07  16:39:17  kevins]
 * 
 * 	Define key_t here instead of ipc.h, as X/OPEN requires.
 * 	[90/08/03  16:27:39  bet]
 * 
 * Revision 1.9  90/07/27  09:08:20  devrcs
 * 	Fixed numerous bogus type mismatches in signal code
 * 	[90/07/13  16:54:34  jeffc]
 * 
 * Revision 1.8  90/06/22  20:54:32  devrcs
 * 	Changed the following types to longs: dev_t, pid_t, uid_t, gid_t, mode_t.
 * 	[90/06/06  13:05:41  ers]
 * 
 * Revision 1.7  90/05/13  20:33:59  devrcs
 * 	Cleanup for gcc
 * 	[90/04/26  12:44:59  brezak]
 * 
 * Revision 1.6  90/04/27  19:27:43  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/18  11:47:46  tom]
 * 
 * Revision 1.5  90/03/13  20:50:21  mbrown
 * 	commented endifs
 * 	[90/02/24  18:04:40  mbrown]
 * 
 * 	added va_list define again, for ANSI
 * 	[90/02/22  11:33:59  mbrown]
 * 
 * 	removal of va_list, now in var/stdarg.h files
 * 	[90/02/21  17:44:48  mbrown]
 * 
 * 	AIX merge first cut.
 * 	[90/02/09  18:06:41  tom]
 * 
 * Revision 1.4  90/02/16  16:53:01  devrcs
 * 	Added clock_t, fixpt_t, NBBY, and FD_SETSIZE from 4.4BSD.
 * 	[90/01/25  18:53:54  coren]
 * 
 * Revision 1.3  90/01/02  20:16:10  gm
 * 	     Fixes for first snapshot.
 * 
 * $EndLog$
 */
/* @(#)types.h  1.45  com/inc/sys,3.1,9013 1/15/90 16:21:51 */
/*
 * COMPONENT_NAME: type declaration header file
 *
 * ORIGIN: IBM, ATT, BSD
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
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *      (#)types.h     7.1 (Berkeley) 6/4/86
 */

#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#ifndef	_KERNEL
#ifndef	PFS
#define	PFS
#endif
#endif

#include <standards.h>

#if     !defined(ASSEMBLER) && !defined(LOCORE)

#ifdef   _ANSI_C_SOURCE
/*
 * ANSI C required typedefs
 */

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef int             ptrdiff_t;
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef long wchar_t;
#endif

#ifndef _FPOS_T
#define _FPOS_T
typedef long            fpos_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef long            time_t;
#endif

#ifndef _CLOCK_T
#define _CLOCK_T
typedef int             clock_t;
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long   size_t;
#endif

#ifdef	PFS
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
#endif	PFS

#endif   /* _ANSI_C_SOURCE */

#ifdef   _POSIX_SOURCE

typedef long    ssize_t;
/*
 * shorthand type definitions for unsigned storage classes
 */
typedef	unsigned char	uchar_t;
typedef	unsigned short	ushort_t;
typedef	unsigned int	uint_t;
typedef unsigned long	ulong_t;


#if	defined(__vax__) || defined(__ns32000__) || defined(__i386__)
typedef	struct	{ int r[1]; } *physadr_t;
typedef	struct	label_t	{
	int	val[14];
} label_t;
#endif	/* defined(__vax__) || defined(__ns32000__) || defined(__i386__) */

#ifdef  __i860__
typedef struct { int r[1]; } *physadr_t;
typedef struct  label_t {
        int     val[30]; /* Matches _setjmp. Must change if _setjmp() changes*/
} label_t;        /*_setjmp() is in mk/src/latest/user/libmach/i860/_setjmp.s*/
#endif  /* __i860__ */

#ifdef	__ibmrt__
typedef	struct	{ int r[1]; } *physadr_t;
typedef	struct	label_t	{
	int	val[16];
} label_t;
#endif	/* __ibmrt__ */

#if	defined(__mc68000__) || defined(__mc68000)
typedef struct  { short r[1]; } *physadr_t;
typedef struct  label_t {
        int     val[13];
} label_t;
#endif	/* __mc68000__ */

#ifdef __sparc__
typedef struct  { int r[1]; } *physadr_t;
typedef struct label_t {
        int     val[2];
} label_t;
#endif	/* __sparc__ */

#ifdef	__mips__
typedef	struct	{ int r[1]; } *physadr_t;
/*
 * WARNING:
 * this must match the definition of kernel jmpbuf's in machine/pcb.h
 */
typedef	struct	label_t	{
	int	val[12];
} label_t;
#endif	/* __mips__ */

typedef long		level_t;
typedef	long		daddr_t;	/* disk address */
typedef	char *		caddr_t;	/* "core" (i.e. memory) address */
typedef long *  qaddr_t;        /* should be typedef quad * qaddr_t; */
typedef	ulong_t		ino_t;		/* inode number (filesystem) */
typedef short		cnt_t;
typedef long		dev_t;		/* device number (major+minor) */
typedef	long		chan_t;		/* channel number (minor's minor) */

#ifdef  _KERNEL
typedef ulong_t  off_t;			/* file offset */
#else   /* _KERNEL */
typedef long    off_t;			/* file offset */
#endif  /* _KERNEL */

typedef	long		paddr_t;
typedef	ushort_t	nlink_t;
typedef long    	key_t;		/* ipc key type */
typedef	ulong_t		mode_t;		/* file mode */
typedef ulong_t		uid_t;		/* user ID */
typedef ulong_t		gid_t;		/* group ID */
typedef	void *		mid_t;		/* module ID	*/
typedef	long		pid_t;		/* process ID */
typedef char		slab_t[12];	/* security label */


/* typedef for signal mask */
typedef unsigned int 	sigset_t;

#endif /* _POSIX_SOURCE */

#ifdef _OSF_SOURCE

#ifndef NULL
#define	NULL	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

typedef void (*sig_t)();

/*
 * Basic system types and major/minor device constructing/busting macros.
 */

/* major part of a device */
#define major(x)        ((int)(((unsigned)(x)>>8)&0377))

/* minor part of a device */
#define minor(x)        ((int)((x)&0377))

/* make a device number */
#define makedev(x,y)    ((dev_t)(((x)<<8) | (y)))

#include <mach/machine/vm_types.h>

/*
 * shorthand type definitions for unsigned storage classes
 */
typedef	uchar_t		uchar;
typedef	ushort_t	ushort;
typedef	uint_t		uint;
typedef ulong_t		ulong;

typedef	physadr_t	physadr;


/* typedefs for BSD unsigned things */
typedef	uchar_t		u_char;
typedef	ushort_t 	u_short;
typedef	uint_t		u_int;
typedef	ulong_t		u_long;

#ifdef  _KERNEL
typedef struct  _quad { u_long val[2]; } quad;
#else   /* _KERNEL */
typedef struct  _quad { long val[2]; } quad;
#endif  /* _KERNEL */

typedef	long	swblk_t;
typedef u_long	fixpt_t;

/* We don't really want to include all of limits.h to get the 
   real value of this, so we leave this ugly constant here */
#define	NBBY	8		/* number of bits in a byte */

/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= OPEN_MAX (limits.h).
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	256
#endif

typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */

#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))


#endif   /* _OSF_SOURCE */

#ifdef	OSF1_ADFS
typedef	long	node_t;				/* node # */
#define INVALID_NODE	((node_t)(~0))
#endif

#endif  /* ASSEMBLER */

#endif /* _SYS_TYPES_H_ */
