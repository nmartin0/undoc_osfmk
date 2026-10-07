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
 * $Log: uipc_domain.c,v $
 * Revision 1.5  1994/11/18  20:28:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:50:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:03:44  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.3  1993/07/01  18:51:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:07:23  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:17:30  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:07:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.7  93/07/07  10:30:21  mjl
 * Use NET_THREADSTART() macro for deferred startup of periodic net threads.
 * 
 * Revision 2.6  1993/04/29  13:59:33  klh
 * 	Revision 2.6  93/03/25  14:42:25  durriya
 * 		domaininit takes a boolean to avoid starting timeout threads on
 * 		fileserver node (durriya)
 *
 * 	Revision 2.5  93/03/24  15:37:21  loverso
 * 		MK changes: fix race condition between protocol initialisation and
 * 		first call to its timeout functions.
 * 		[1993/01/20  20:35:40  emcmanus]
 *
 * 	Revision 2.4  93/03/22  23:58:08  condict
 * 		Fix for bug #174: race condition in protocol initialization.
 * 		[1992/09/23  12:07:54  barbou]
 * 		Corrected above fix according to description in bug #174.
 * 		(loverso/condict)
 *
 * Revision 2.5  93/04/16  14:41:51  mjl
 * Separate startup of pffasttimo and pfslowtimo threads.  For TNC,
 * delay starting timo threads on a node until a network interface
 * is config'ed there.
 * 
 * Revision 2.4  93/03/22  21:09:46  yazz
 * OSF lock changes exposed lock initialization race condition; corrected.
 * 
 * Revision 2.3  92/05/24  14:18:05  pjg
 * 	92/03/31  15:40:12  emcmanus
 * 	Name the pffasttimo and pfslowtimo threads (if used).
 * 	[92/05/18            srl]
 * 
 * Revision 2.2  91/08/31  13:24:09  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:31:31  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11  90/10/07  13:20:29  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:02:59  gm]
 * 
 * 	Change name of ALIGN to avoid clash.
 * 	[90/09/29  18:04:50  tmt]
 * 
 * Revision 1.10  90/07/27  08:44:53  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  15:34:11  tmt]
 * 
 * Revision 1.9  90/07/05  23:08:06  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	[90/07/03  18:40:09  tmt]
 * 
 * 	Call mbuf garbage collector from slow timeout.
 * 	[90/06/28  11:52:35  tmt]
 * 
 * Revision 1.8  90/06/22  20:07:39  devrcs
 * 	Add include of sys/lock_types.h
 * 	[90/06/13  09:00:09  tmt]
 * 
 * 	Fix error returns from domain_del.
 * 	[90/06/09  17:54:42  tmt]
 * 
 * Revision 1.7  90/04/27  18:53:18  devrcs
 * 	Add SPLNET's, domain list now changes!
 * 	[90/04/20  12:11:19  tmt]
 * 
 * Revision 1.6  90/04/14  00:30:19  devrcs
 * 	Dynamic domain attach. Allow insertion and deletion of domains.
 * 	Strengthen some declarations (void).
 * 	[90/04/09  15:32:36  tmt]
 * 
 * Revision 1.5  90/03/27  13:15:12  gm
 * 	Do #undefs before ADDDOMAIN macro ("unix" defined by some cc's).
 * 	[90/03/07  12:26:46  tmt]
 * 
 * Revision 1.4  90/01/18  08:42:18  gm
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
 * Revision 1.3  90/01/03  11:52:09  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:27:58  gm]
 * 
 * Revision 1.2  89/12/26  09:23:33  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
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
 *	Base:	%W% (Berkeley) %G%
 *	Merged: uipc_domain.c	7.7 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/kernel.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/domain.h"
#include "sys/protosw.h"
#include "sys/errno.h"

#define HDRALIGN(x)	(((x)+sizeof(u_long)-1) & ~(sizeof(u_long)-1))

LOCK_ASSERTL_DECL

struct domain	*domains;
#if	NETSYNC_LOCK
lock_data_t	domain_lock;
#endif

int	domain_adding = 0;	/* protected by the global domain lock */

void
domain_threadstart()
{
	static int started;

	if (started)
		return;
	started = 1;
#if	!NETISR_THREAD
	pffasttimo();
	pfslowtimo();
#else
	NET_THREADSTART(pffasttimo, 10, "pffasttimo");
	NET_THREADSTART(pfslowtimo, -1, "pfslowtimo");
#endif
}

void
domaininit(boolean_t fullnode)
{

	domains = 0;
	DOMAIN_LOCKINIT();
	max_linkhdr = 16;		/* XXX */
	max_protohdr = 0;
	max_hdr = max_linkhdr + max_protohdr;
	max_datalen = MHLEN - max_hdr;
	if (fullnode) {
		domain_threadstart();
	}
}

domain_add(dp)
	register struct domain *dp;
{
	register struct domain *tdp;
	register struct protosw *pr;
	DOMAIN_LOCK_DECL()
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_WRITE_LOCK();
	for (tdp = domains; tdp; tdp = tdp->dom_next)
		if (tdp->dom_family == dp->dom_family) {
			DOMAIN_WRITE_UNLOCK();
			return EEXIST;
		}
	DOMAINRC_LOCKINIT(dp);
	dp->dom_refcnt = 0;
	dp->dom_next = domains;
	domains = dp;
	domain_adding++;
	DOMAIN_WRITE_UNLOCK();

	DOMAIN_FUNNEL(dp, f);
	if (dp->dom_init)
		(*dp->dom_init)();
	for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
		if (pr->pr_init)
			(*pr->pr_init)();
	DOMAIN_UNFUNNEL(f);

	DOMAIN_WRITE_LOCK();
	max_linkhdr = HDRALIGN(max_linkhdr);
	max_protohdr = HDRALIGN(max_protohdr);
	max_hdr = max_linkhdr + max_protohdr;
	max_datalen = MHLEN - max_hdr;
	if (max_datalen < 0)
		panic("domain_add max_hdr");
	domain_adding--;
	DOMAIN_WRITE_UNLOCK();
	return 0;
}

domain_del(dp)
	struct domain *dp;
{
	int error = 0;
	DOMAIN_LOCK_DECL()

	DOMAIN_WRITE_LOCK();
	DOMAINRC_LOCK(dp);
	if (dp->dom_refcnt == 0) {
		if (dp == domains)
			domains = dp->dom_next;
		else {
			register struct domain *tdp;
			for (tdp = domains; tdp; tdp = tdp->dom_next)
				if (tdp->dom_next == dp) {
					tdp->dom_next = dp->dom_next;
					break;
				}
			if (tdp == NULL)
				error = ENOENT;
		}
	} else
		error = EADDRINUSE;
	DOMAINRC_UNLOCK(dp);
	DOMAIN_WRITE_UNLOCK();
	return error;
}

struct protosw *
pffindtype(family, type)
	int family, type;
{
	register struct domain *dp;
	register struct protosw *pr;
	DOMAIN_LOCK_DECL()

	DOMAIN_READ_LOCK();
	for (dp = domains; dp; dp = dp->dom_next)
		if (dp->dom_family == family)
			goto found;
	DOMAIN_READ_UNLOCK();
	return (0);
found:
	for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
		if (pr->pr_type && pr->pr_type == type) {
			DOMAINRC_REF(dp);
			DOMAIN_READ_UNLOCK();
			return (pr);
		}
	DOMAIN_READ_UNLOCK();
	return (0);
}

struct protosw *
pffindproto(family, protocol, type)
	int family, protocol, type;
{
	register struct domain *dp;
	register struct protosw *pr;
	struct protosw *maybe = 0;
	DOMAIN_LOCK_DECL()

	if (family == 0)
		return (0);
	DOMAIN_READ_LOCK();
	for (dp = domains; dp; dp = dp->dom_next)
		if (dp->dom_family == family)
			goto found;
	DOMAIN_READ_UNLOCK();
	return (0);
found:
	for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++) {
		if ((pr->pr_protocol == protocol) && (pr->pr_type == type)) {
			maybe = pr;
			break;
		}
		if (type == SOCK_RAW && pr->pr_type == SOCK_RAW &&
		    pr->pr_protocol == 0 && maybe == (struct protosw *)0)
			maybe = pr;
	}
	if (maybe)
		DOMAINRC_REF(dp);
	DOMAIN_READ_UNLOCK();
	return (maybe);
}

void
pfctlinput(cmd, sa)
	int cmd;
	struct sockaddr *sa;
{
	register struct domain *dp;
	register struct protosw *pr;
	DOMAIN_LOCK_DECL()
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_READ_LOCK();
	for (dp = domains; dp; dp = dp->dom_next) {
		DOMAIN_FUNNEL(dp, f);
		for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
			if (pr->pr_ctlinput)
				(*pr->pr_ctlinput)(cmd, sa, (caddr_t) 0);
		DOMAIN_UNFUNNEL(f);
	}
	DOMAIN_READ_UNLOCK();
}

void
pfreclaim()
{
	register struct domain *dp;
	register struct protosw *pr;
	DOMAIN_LOCK_DECL()
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_READ_LOCK();
	for (dp = domains; dp; dp = dp->dom_next) {
		DOMAIN_FUNNEL(dp, f);
		for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
			if (pr->pr_drain)
				(*pr->pr_drain)();
		DOMAIN_UNFUNNEL(f);
	}
	DOMAIN_READ_UNLOCK();
}

pfslowtimo()
{
	register struct domain *dp;
	register struct protosw *pr;
	DOMAIN_LOCK_DECL()
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_READ_LOCK();
	if (!domain_adding)
		for (dp = domains; dp; dp = dp->dom_next) {
			DOMAIN_FUNNEL(dp, f);
			for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
				if (pr->pr_slowtimo)
					(*pr->pr_slowtimo)();
			DOMAIN_UNFUNNEL(f);
		}
	DOMAIN_READ_UNLOCK();
	mbufgc();		/* uipc_mbuf garbage collection */
#if	!NETISR_THREAD
	timeout(pfslowtimo, (caddr_t)0, hz/PR_SLOWHZ);
#else
	return (hz/PR_SLOWHZ);
#endif
}

pffasttimo()
{
	register struct domain *dp;
	register struct protosw *pr;
	DOMAIN_LOCK_DECL()
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_READ_LOCK();
	if (!domain_adding)
		for (dp = domains; dp; dp = dp->dom_next) {
			DOMAIN_FUNNEL(dp, f);
			for (pr = dp->dom_protosw; pr < dp->dom_protoswNPROTOSW; pr++)
				if (pr->pr_fasttimo)
					(*pr->pr_fasttimo)();
			DOMAIN_UNFUNNEL(f);
		}
	DOMAIN_READ_UNLOCK();
#if	!NETISR_THREAD
	timeout(pffasttimo, (caddr_t)0, hz/PR_FASTHZ);
#else
	return (hz/PR_FASTHZ);
#endif
}
