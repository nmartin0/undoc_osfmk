/*
 * 
 * $Copyright
 * Copyright 1991, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: defs.h,v $
 * Revision 1.6  1994/11/18  20:24:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  22:16:11  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/05/27  21:17:14  richardg
 * changed MAXBSIZE to 65536.
 *
 * Revision 1.3  1993/04/27  20:13:43  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:13:15  dleslie
 * First R1_0 release
 *
 * Revision 2.4  91/05/14  15:22:01  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:01:05  mrt
 * 	Changed to new copyright
 * 	[91/01/28  14:54:37  mrt]
 * 
 * Revision 2.2  90/08/27  21:45:05  dbg
 * 	Created.
 * 	[90/07/16            dbg]
 * 
 */

/*
 * Common definitions for Berkeley Fast File System.
 */
#include <sys/types.h>
#include <device/param.h>

typedef	unsigned short	uid_t;
typedef	unsigned short	gid_t;
typedef	unsigned long	ino_t;

#define	NBBY	8

/*
 * The file system is made out of blocks of at most MAXBSIZE units,
 * with smaller units (fragments) only in the last direct block.
 * MAXBSIZE primarily determines the size of buffers in the buffer
 * pool.  It may be made larger without any effect on existing
 * file systems; however, making it smaller may make some file
 * systems unmountable.
 *
 * Note that the disk devices are assumed to have DEV_BSIZE "sectors"
 * and that fragments must be some multiple of this size.
 */
#ifdef PARAGON860
#define	MAXBSIZE	65536
#else
#define	MAXBSIZE	8192
#endif
#define	MAXFRAG		8

/*
 * MAXPATHLEN defines the longest permissible path length
 * after expanding symbolic links.
 *
 * MAXSYMLINKS defines the maximum number of symbolic links
 * that may be expanded in a path name.  It should be set
 * high enough to allow all legitimate uses, but halt infinite
 * loops reasonably quickly.
 */

#define	MAXPATHLEN	1024
#define	MAXSYMLINKS	8

