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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: kernel.h,v $
 * Revision 1.4  1994/11/18  20:40:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:25:21  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:07  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:51  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:48  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/12/16  18:30:46  roy
 * 	91/10/23  16:38:23  condict
 * 	Move the MAP_TIME definitions from here to sys/time.h, where they now
 * 	belong (because the involve changes to the TIME LOCK macros).
 *
 * 	91/10/17  15:45:19  condict
 * 	Put typecast in def of get_time to avoid compiler error. Delete several
 * 	declarations for more similarity to OSF/1 integrated kernel.
 *
 * Revision 2.2  91/08/31  14:07:20  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:43:20  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:51:26  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:41:00  gm]
 * 
 * Revision 1.6  90/08/09  13:28:47  devrcs
 * 	On mips kcount is an unsigned int.
 * 	[90/07/20  22:06:57  brezak]
 * 
 * Revision 1.5  90/06/22  20:53:11  devrcs
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Changes for gcc.				brezak@osf.org
 * 	Change lbolt to type time_t.			duthie@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Mach 2.5 and Encore 0.6 merge.			gm@osf.org
 * 	Merge early Mach 2.5, Encore parallelization.	alan@encore.com
 * 	No MACH_VFS code, domainname len MAXDOMNAMELEN.	gm0w@cmu.edu
 * 	More cleanup.					rpd@cmu.edu
 * 	KERNEL conditional for entire file.		mrt@cmu.edu
 * 	Vnode support: declare domainname{,len}.	jsb@cmu.edu
 * 	Adjusted include file references.		mwyoung@cmu.edu
 * 	Added include of 'time.h'.			boykin@cmu.edu
 * 	Use MACH conditional.				avie@cmu.edu
 * 	[90/06/12  21:38:34  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kernel.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_KERNEL_H_
#define _SYS_KERNEL_H_

#ifdef	_KERNEL
#include <sys/unix_defs.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>		/* for MAXHOSTNAMELEN */

/*
 * Global variables for the kernel
 */

extern long	rmalloc();

/* 1.1 */
/*
 * The hostname lock protects the hostname, hostid, domainname
 * and uname (SysV) variables.
 */
lock_data_t			hostname_lock;
#define	HOSTNAME_LOCKINIT()	lock_init2(&hostname_lock,TRUE,LTYPE_HOSTNAME)
#define	HOSTNAME_READ_LOCK()	lock_read(&hostname_lock)
#define	HOSTNAME_READ_UNLOCK()	lock_read_done(&hostname_lock)
#define	HOSTNAME_WRITE_LOCK()	lock_write(&hostname_lock)
#define	HOSTNAME_WRITE_UNLOCK()	lock_write_done(&hostname_lock)

extern long	hostid;
extern char	hostname[MAXHOSTNAMELEN];
extern int	hostnamelen;
extern char	domainname[MAXDOMNAMELEN];
extern int	domainnamelen;

/* 1.2 */
extern struct	timeval boottime;
extern struct	timeval time;	/* Must only be accessed inside TIME LOCK's or
				 * it will be out of date -- see time.h
				 */
extern struct	timezone tz;		/* XXX */
extern int	hz;
extern int	phz;			/* alternate clock's frequency */
extern int	tick;
extern time_t	lbolt;			/* awoken once a second */
extern int	realitexpire();

#define LSCALE	1000		/* scaling for "fixed point" arithmetic */
extern	long	avenrun[3];
extern	long	mach_factor[3];

#ifdef	GPROF
extern	int profiling;
extern	char *s_lowpc;
extern	u_long s_textsize;
#ifdef mips
extern	unsigned int *kcount;
#else
extern	u_short *kcount;
#endif
#endif

#endif	/* _KERNEL */
#endif	/* _SYS_KERNEL_H_ */
