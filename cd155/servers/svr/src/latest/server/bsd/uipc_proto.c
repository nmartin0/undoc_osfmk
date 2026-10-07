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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: uipc_proto.c,v $
 * Revision 1.7  1995/02/01  21:30:08  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:28:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/09/20  18:24:48  yazz
 *  Author: Mike Leibensperger
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: Instrumentation for #9421
 *  Testing: Sched EAT, misc networking testing also
 *  Module(s): server/uxkern/const_region.h, server/bsd/init_main.c,
 * 	server/bsd/uipc_proto.c, server/netinet/in_proto.c,
 * 	server/tnc/dvp_init.c
 *
 * Implement a mechanism in assertful servers whereby areas of the data
 * segment known to be constant can be vm_protect()'d against unintended
 * overwriting.  Corrupted jump tables are known to cause #9421.
 *
 * Revision 1.4  1993/07/14  17:50:38  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:51:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:07:42  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:51  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1992/06/26  17:59:30  mjl
 * Use protocol number to select among various virtual socket ops for AF_UNIX.
 *
 * Revision 2.3  92/05/24  14:18:15  pjg
 * 	92/03/23  18:02:40  condict
 * 	Include sys/user.h, for unix_release call.
 * 	[92/05/18            srl]
 * 
 * Revision 2.2  91/08/31  13:24:19  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:31:51  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11  90/10/07  13:20:40  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:03:25  gm]
 * 
 * Revision 1.10  90/09/23  15:43:51  devrcs
 * 	Make funnel op static.
 * 	[90/09/15  14:51:46  tmt]
 * 
 * Revision 1.9  90/09/13  11:42:38  devrcs
 * 	Add "force unfunnel" op, along with sample lock funnel implementation.
 * 	[90/08/28  14:48:16  tmt]
 * 
 * Revision 1.8  90/07/27  08:45:02  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  15:35:04  tmt]
 * 
 * Revision 1.7  90/07/05  23:08:16  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	Splnet() may be a bit extreme for uipc, but is same as before.
 * 	[90/07/03  18:41:15  tmt]
 * 
 * Revision 1.6  90/04/27  18:53:33  devrcs
 * 	Delete trailing commas in declarations.
 * 	[90/04/20  12:13:31  tmt]
 * 
 * Revision 1.5  90/04/14  00:30:27  devrcs
 * 	Add install-only config entry for dynamic domains.
 * 	Strengthen type, eliminate function decls (proto_xxx.h does this).
 * 	[90/04/09  15:37:02  tmt]
 * 
 * Revision 1.4  90/01/18  08:42:28  gm
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
 * Revision 1.3  90/01/03  11:52:17  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:28:06  gm]
 * 
 * Revision 1.2  89/12/26  09:23:54  gm
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
 *	Base:	uipc_proto.c	7.3 (Berkeley) 6/29/88
 *	Merged: uipc_proto.c	7.4 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/user.h" /* F or unix_release() */

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/domain.h"
#include "sys/protosw.h"

#ifdef	TNC
#include "vsocket/vsocket.h"
#endif

/*
 * Definitions of protocols supported in the UNIX domain.
 */

extern	struct domain unixdomain;		/* or at least forward */


#define UIPC_LOCK_FUNNEL (MACH && NETSYNC_SPL && NETSYNC_LOCKTEST)

#if	UIPC_LOCK_FUNNEL
/* Something of a demonstrator. */
#include "kern/lock.h"
static struct {
	lock_data_t	l;	/* the lock */
	int		c;	/* the depth */
} uipc_lock;
#endif

uipc_config()
{
#if	UIPC_LOCK_FUNNEL
	lock_init(&uipc_lock.l, 1);
	uipc_lock.c = 0;
#endif
	return domain_add(&unixdomain);
}

#if	NETSYNC_SPL

#if	MACH
#include "kern/parallel.h"
#endif

static void
uipc_sanity()
{
	panic("uipc unfunnel");
}

static void
uipc_unfunnel(dfp)
	struct domain_funnel *dfp;
{
	dfp->unfunnel = uipc_sanity;
#if	!UIPC_LOCK_FUNNEL
	NETSPLX(dfp->object.spl);
	unix_release();
#else
	if (--uipc_lock.c < 0)
		panic("uipc_unfunnel");
	if (uipc_lock.c == 0)
		lock_clear_recursive(&uipc_lock.l);
	lock_done(&uipc_lock.l);
#endif
}

static void
uipc_funnel(dfp)
	struct domain_funnel *dfp;
{
	if (dfp->unfunnel)
		panic("uipc funnel");
	dfp->unfunnel = uipc_unfunnel;
#if	!UIPC_LOCK_FUNNEL
	unix_master();
	NETSPL(dfp->object.spl,net);
#else
	lock_write(&uipc_lock.l);
	if (++uipc_lock.c == 1)
		lock_set_recursive(&uipc_lock.l);
#endif
}

#if	!UIPC_LOCK_FUNNEL
/* No "force unfunnel" op required with unix_master/spl */
#define	uipc_funfrc	0
#else
static void
uipc_funfrc(dfp)
	struct domain_funnel *dfp;
{
	/* Same function used for both unfunnel and refunnel */
	if (dfp->unfunnel) {
		/* Restore lock(s) to same depth */
		lock_write(&uipc_lock.l);
		lock_set_recursive(&uipc_lock.l);
		while (++uipc_lock.c != dfp->object.spl)
			lock_write(&uipc_lock.l);
	} else {
		/* Release lock(s), saving depth in spl */
		if (uipc_lock.c <= 0)
			panic("uipc_funfrc");
		dfp->unfunnel = uipc_funfrc;
		dfp->object.spl = uipc_lock.c;
		do {
			if (--uipc_lock.c == 0)
				lock_clear_recursive(&uipc_lock.l);
			lock_done(&uipc_lock.l);
		} while (uipc_lock.c);
	}
}
#endif

#else
#define uipc_funnel	0
#define uipc_funfrc	0
#endif

#if MACH_ASSERT
#include <uxkern/const_region.h>
START_CONST_REGION(unixsw);
#endif

CONST struct protosw unixsw[] = {
{ SOCK_STREAM,	&unixdomain,	0,	PR_CONNREQUIRED|PR_WANTRCVD|PR_RIGHTS,
  0,		0,		0,		0,
  uipc_usrreq,
  uipc_init,	0,		0,		0
},
#ifdef	TNC
{ SOCK_STREAM,	&unixdomain, VOS_PIPE,	PR_CONNREQUIRED|PR_WANTRCVD|PR_RIGHTS,
  0,		0,		0,		0,
  uipc_usrreq,
  uipc_init,	0,		0,		0
},
{ SOCK_STREAM,	&unixdomain, VOS_SOCK,	PR_CONNREQUIRED|PR_WANTRCVD|PR_RIGHTS,
  0,		0,		0,		0,
  uipc_usrreq,
  uipc_init,	0,		0,		0
},
{ SOCK_STREAM,	&unixdomain, VOS_FIFO,	PR_CONNREQUIRED|PR_WANTRCVD|PR_RIGHTS,
  0,		0,		0,		0,
  uipc_usrreq,
  uipc_init,	0,		0,		0
},
#endif	/* TNC */
{ SOCK_DGRAM,	&unixdomain,	0,		PR_ATOMIC|PR_ADDR|PR_RIGHTS,
  0,		0,		0,		0,
  uipc_usrreq,
  0,		0,		0,		0
},
{ 0,		0,		0,		0,
  (void (*)())raw_input, 0,	raw_ctlinput,	0,
  raw_usrreq,
  0,		0,		0,		0
}
};

#if MACH_ASSERT
END_CONST_REGION(unixsw);
#endif

struct domain unixdomain =
    { AF_UNIX, "unix", 0, unp_externalize, unp_dispose,
      unixsw, &unixsw[sizeof(unixsw)/sizeof(unixsw[0])],
      0, 0, uipc_funnel, uipc_funfrc };
