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
 * $Log: domain.h,v $
 * Revision 1.5  1994/11/18  20:40:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:24:17  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:18:41  nandy
 * ad103+tnc merged with Intel code.
 *
 *
 * Revision 1.1.2.1  1992/11/05  22:39:22  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:06  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:03:52  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:42:21  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11  90/10/07  14:49:54  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:38:23  gm]
 * 
 * Revision 1.10  90/09/13  11:51:14  devrcs
 * 	Add "force unfunnel" operation.
 * 	[90/08/28  11:25:03  tmt]
 * 
 * Revision 1.9  90/07/27  09:07:03  devrcs
 * 	Update to BSD Reno release.
 * 	Merge domain locks with NETSPL.
 * 	[90/07/19  15:47:11  tmt]
 * 
 * Revision 1.8  90/07/05  23:14:15  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	[90/07/03  18:58:34  tmt]
 * 
 * Revision 1.7  90/06/22  20:52:44  devrcs
 * 	Add _KERNEL to locking #if.
 * 	[90/06/09  17:50:33  tmt]
 * 
 * Revision 1.6  90/04/27  19:25:39  devrcs
 * 	Rearrange REF/UNREF macros.
 * 	[90/04/20  12:05:23  tmt]
 * 
 * Revision 1.5  90/04/14  00:33:56  devrcs
 * 	Add dynamic domain attach elements.
 * 	[90/04/09  15:42:11  tmt]
 * 
 * Revision 1.4  90/01/18  08:48:50  gm
 * 	Make file re-entrant.
 * 	[90/01/08  15:41:43  tmt]
 * 
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/02  20:15:44  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:33:44  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	domain.h	7.3 (Berkeley) 6/27/88
 *	Merged: domain.h	7.4 (Berkeley) 6/28/90
 */

#ifndef	_SYS_DOMAIN_H_
#define _SYS_DOMAIN_H_

/*
 * Structure per communications domain.
 */
struct	domain {
	int	dom_family;		/* AF_xxx */
	char	*dom_name;
	void	(*dom_init)();		/* initialize domain data structures */
	int	(*dom_externalize)();	/* externalize access rights */
	void	(*dom_dispose)();	/* dispose of internalized rights */
	struct	protosw *dom_protosw, *dom_protoswNPROTOSW;
	struct	domain *dom_next;
	int	dom_refcnt;		/* # sockets in this domain */
	void	(*dom_funnel)();	/* uniprocessor compat */
	void	(*dom_funfrc)();	/* uniprocessor compat */
#if	defined(_KERNEL) && NETSYNC_LOCK
	simple_lock_data_t	dom_rc_lock;
#endif
};

#ifdef	_KERNEL
extern	struct domain *domains;
#if	NETSYNC_LOCK
extern	lock_data_t domain_lock;
#define DOMAIN_LOCKINIT()	lock_init2(&domain_lock, TRUE, LTYPE_DOMAIN)
#define DOMAIN_LOCK_DECL()	NETSPL_DECL(_ds)
#define DOMAIN_READ_LOCK()	{ NETSPL(_ds,net); lock_read(&domain_lock); }
#define DOMAIN_WRITE_LOCK()	{ NETSPL(_ds,net); lock_write(&domain_lock); }
#define DOMAIN_READ_UNLOCK()	{ lock_done(&domain_lock); NETSPLX(_ds); }
#define DOMAIN_WRITE_UNLOCK()	{ lock_done(&domain_lock); NETSPLX(_ds); }
#define	DOMAINRC_LOCKINIT(dp)	simple_lock_init(&((dp)->dom_rc_lock))
#define	DOMAINRC_LOCK(dp)	simple_lock(&((dp)->dom_rc_lock))
#define	DOMAINRC_UNLOCK(dp)	simple_unlock(&((dp)->dom_rc_lock))
#else	/* !NETSYNC_LOCK */
#define DOMAIN_LOCKINIT()
#define DOMAIN_LOCK_DECL()	NETSPL_DECL(_ds)
#define DOMAIN_READ_LOCK()	NETSPL(_ds,net)
#define DOMAIN_WRITE_LOCK()	NETSPL(_ds,net)
#define DOMAIN_READ_UNLOCK()	NETSPLX(_ds)
#define DOMAIN_WRITE_UNLOCK()	NETSPLX(_ds)
#define	DOMAINRC_LOCKINIT(dp)
#define	DOMAINRC_LOCK(dp)
#define	DOMAINRC_UNLOCK(dp)
#endif	/* !NETSYNC_LOCK */

#define	DOMAINRC_REF(dp) \
	{ DOMAINRC_LOCK(dp); (dp)->dom_refcnt++; DOMAINRC_UNLOCK(dp); }

#define	DOMAINRC_UNREF(dp) \
	{ DOMAINRC_LOCK(dp); (dp)->dom_refcnt--; DOMAINRC_UNLOCK(dp); }

/*
 * Uniprocessor compatibility: allows per-domain funnel operation.
 * Replaces all socket-level spl's, for instance.
 */
struct domain_funnel {
	void (*unfunnel)();		/* unfunnel operation or NULL */
	union {				/* local storage for operation */
		int	spl;			/* previous spl */
		caddr_t	other;			/* recursive lock_t, e.g. */
	} object;
};

#define DOMAIN_FUNNEL_DECL(f) \
	struct domain_funnel f;

#define DOMAIN_FUNNEL(dp, f) \
	{ (f).unfunnel = 0; if ((dp)->dom_funnel) (*(dp)->dom_funnel)(&(f)); }

#define DOMAIN_UNFUNNEL(f) \
	{ if ((f).unfunnel) (*(f).unfunnel)(&(f)); }

/* Forced unfunnel is used before sleeping in sosleep() */
#define DOMAIN_UNFUNNEL_FORCE(dp, f) \
	{ (f).unfunnel = 0; \
	  if ((dp)->dom_funfrc) (*(dp)->dom_funfrc)(&(f)); }

#endif
#endif
