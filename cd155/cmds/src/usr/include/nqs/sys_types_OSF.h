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
* this file is almost exact copy of the file /scratch/macs/INTEL_COMP/paragon/include/sys/types.h 
*
/

/*
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
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
/*
 * HISTORY
 * $Log: sys_types_OSF.h,v $
 * Revision 1.2  1994/11/19  02:44:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 *
 */

#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_
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
typedef unsigned short  wchar_t;
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

#endif   /* _ANSI_C_SOURCE */

#ifdef   _POSIX_SOURCE

/*
 * shorthand type definitions for unsigned storage classes
 */
typedef	unsigned char	uchar_t;
typedef	unsigned short	ushort_t;
typedef	unsigned int	uint_t;
typedef unsigned long	ulong_t;


#ifdef  __i860__
typedef struct { int r[1]; } *physadr_t;
typedef struct  label_t {
        int     val[30]; /* Matches _setjmp. Must change if _setjmp() changes*/
} label_t;        /*_setjmp() is in mk/src/latest/user/libmach/i860/_setjmp.s*/
#endif  /* __i860__ */

#if	defined(__vax__) || defined(__ns32000__) || defined(__i386__)
typedef	struct	{ int r[1]; } *physadr_t;
typedef	struct	label_t	{
	int	val[14];
} label_t;
#endif	/* defined(__vax__) || defined(__ns32000__) || defined(__i386__) */

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

#if OSF
	/* is defined in NQS v2.0 as long */
#else
typedef	void *		mid_t;	 	/* module ID	*/
#endif

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

#endif  /* ASSEMBLER */

#endif /* _SYS_TYPES_H_ */
