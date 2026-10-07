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
 * $Log: ucred.h,v $
 * Revision 1.5  1995/02/17  18:09:25  jlitvin
 * If we can't fix the credentials reference count leak, at least give it
 * more room before it causes a problem :^(.  I'm increasing the size of
 * the reference count field from an unsigned short to an unsigned long.
 * Hopefully, the next checkin will fix the underlying problem.  (This is
 * the same thing that dbm did for Tensor).
 *
 *  Reviewer: brad
 *  Risk: low
 *  Benefit or PTS #: 10589
 *  Testing: Concur EAT with debugging instrumentation
 *  Module(s): server/sys/ucred.h
 *
 * Revision 1.4  1994/11/18  20:42:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:30:16  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:11  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:46  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:50  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/10/14  17:18:29  cfj
 * NX integration.
 *
 * Revision 2.3  1991/12/16  18:11:11  roy
 * 	91/10/30  17:43:26  bernadat
 * 	Fixed an #ifdef MACH_AFS statement.
 *
 * Revision 2.2  91/08/31  14:15:50  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:46:58  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:56:02  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:48:07  gm]
 * 
 * 	Changed the group management to separate cr_gid from the cr_groups
 * 	 array.
 * 	[90/09/21  11:09:39  collins]
 * 
 * Revision 1.6  90/08/24  12:27:52  devrcs
 * 	Use consistent credentials macros.
 * 	[90/08/19  01:19:52  nags]
 * 
 * Revision 1.5  90/08/09  13:29:22  devrcs
 * 	Remove mach_afs dependency.
 * 	[90/08/03  17:52:08  tmt]
 * 
 * Revision 1.4  90/06/22  20:54:35  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Parallelized for OSF/1.					nags@encore.com
 * 	Fixes for first snapshot.				gm@osf.org
 * 	[90/06/12  21:39:47  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
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
 *	@(#)ucred.h	7.3 (Berkeley) 8/26/89
 */

#ifndef _SYS_UCRED_H_
#define	_SYS_UCRED_H_

#ifdef	_KERNEL
#include <sys/unix_defs.h>
#endif

/*
 * Credentials.
 *
 * By convention, credentials are never modified, only copied.
 * Thus, it is always possible to examine a credential entry
 * without holding the structure locked.
 *
 * Credentials in the u-area are changed by building a new
 * credentials structure and substituting it for the old one
 * pointed to by the proc structure's p_rcred field.
 */
struct ucred {
#if	MACH_ASSERT
	long	cr_dummy;		/* the zone free element ptr. */
#endif
	u_long	cr_ref;			/* reference count */
	uid_t	cr_uid;			/* effective user id */
	short	cr_ngroups;		/* number of groups */
#ifdef NX
	gid_t	cr_acctid;		/* account id */
#endif /* NX */
	gid_t	cr_gid;			/* effective group id */
	gid_t	cr_groups[NGROUPS];	/* groups */
#if	defined(_KERNEL) && MACH_AFS
	long	cr_pag;			/* AFS process authentication group */
#endif
#ifdef	_KERNEL
	udecl_simple_lock_data(,cr_lock)
#endif
};
#define NOCRED	((struct ucred *)-1)
#define	NOUID	((uid_t) -1)
#define	NOGID	((gid_t) -1)


#ifdef	_KERNEL

#define	CR_LOCK(cr)		usimple_lock(&(cr)->cr_lock)
#define	CR_UNLOCK(cr)		usimple_unlock(&(cr)->cr_lock)
#define	CR_LOCK_INIT(cr)	usimple_lock_init(&(cr)->cr_lock)

/*
 * Inline references for non-debug kernels.
 * Could inline crfree, too.
 */
#if	MACH_ASSERT
#define crhold(cr) cr_ref(cr)
#else
#define	crhold(cr)							\
MACRO_BEGIN								\
	CR_LOCK(cr);							\
	(cr)->cr_ref++;							\
	CR_UNLOCK(cr);							\
MACRO_END
#endif	/* MACH_ASSERT */

struct ucred *crget();
struct ucred *crcopy();
struct ucred *crdup();

#endif

#endif /* _SYS_UCRED_H_ */
