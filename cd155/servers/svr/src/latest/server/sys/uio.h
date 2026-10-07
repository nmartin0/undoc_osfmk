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
 * $Log: uio.h,v $
 * Revision 1.4  1994/11/18  20:42:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:30:23  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:26  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:54  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:15:58  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:47:03  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:56:06  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:48:13  gm]
 * 
 * Revision 1.5  90/07/27  09:08:25  devrcs
 * 	Added UIO_PHYSSPACE to uio_seg enum.
 * 	[90/06/06  13:05:49  ers]
 * 
 * Revision 1.4  90/03/13  20:50:23  mbrown
 * 	AIX merge first cut.
 * 	[90/02/09  18:07:00  tom]
 * 
 * Revision 1.3  90/01/02  20:16:12  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:41:57  gm
 * 	BSD4.4 changes.
 * 	[89/12/25            gm]
 * 
 * Revision 2.7  89/04/22  15:32:36  gm0w
 * 	Removed MACH_VFS changes.
 * 	[89/04/14            gm0w]
 * 
 * Revision 2.6  89/03/09  22:09:37  rpd
 * 	More cleanup.
 * 
 * Revision 2.5  89/02/25  17:57:49  gm0w
 * 	Made CMUCS and MACH_VFS conditional code always
 * 	true.
 * 	[89/02/14            mrt]
 * 
 * Revision 2.4  89/01/18  01:20:06  jsb
 * 	Vnode support: define uio_seg as alias for uio_segflg (for Sun-derived
 * 	code which uses uio_seg).
 * 	[89/01/13            jsb]
 * 
 * Revision 2.3  88/08/24  02:51:09  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:26:52  mwyoung]
 * 
 * Revision 0.0  88/02/26            kirschen
 * 	     Added #include of types.h to get caddr_t.
 * 	[88/02/26            kirschen]
 * 
 * Revision 0.0  88/01/06            jjk
 * 	Added declarations for __STDC__.
 * 	[88/01/06            jjk]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)uio.h	7.4 (Berkeley) 5/9/89
 */

#ifndef	_SYS_UIO_H_
#define _SYS_UIO_H_

#include <sys/types.h>

struct iovec {
	caddr_t	iov_base;
	int	iov_len;
};

enum	uio_rw { UIO_READ, UIO_WRITE };

/*
 * Segment flag values.
 */
enum	uio_seg {
	UIO_USERSPACE,		/* from user data space */
	UIO_SYSSPACE,		/* from system space */
	UIO_USERISPACE,		/* from user I space */
	UIO_PHYSSPACE		/* from physical address space */
};

struct uio {
	struct	iovec *uio_iov;
	int	uio_iovcnt;
	off_t	uio_offset;
	int	uio_resid;
	enum	uio_seg uio_segflg;
	enum	uio_rw uio_rw;
};

 /*
  * Limits
  */
#define UIO_MAXIOV	1024		/* max 1K of iov's */
#define UIO_SMALLIOV	8		/* 8 on stack, else malloc */

#ifndef _KERNEL
#ifdef _NO_PROTO 
extern int readv();
extern int writev();
#else 	/*_NO_PROTO */
extern int readv(int, struct iovec *, int);
extern int writev(int, struct iovec *, int);
#endif	/* _NO_PROTO */
#endif	/* _KERNEL */

#endif	/* _SYS_UIO_H_ */
