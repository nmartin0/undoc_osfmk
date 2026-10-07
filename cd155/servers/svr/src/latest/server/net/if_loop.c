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
 * $Log: if_loop.c,v $
 * Revision 1.9  1995/02/01  21:33:03  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:33:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/09/14  15:31:25  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.6.6.1  1993/09/14  15:29:54  cfj
 * Allow loopback interface to be configured on any network server node,
 * not just the one specified by the NETSERVER bootmagic.
 * Part of fix for PTS bug #6498 and #6492.
 *
 * Revision 1.6  1993/07/14  18:07:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:27:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  18:59:34  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/14  15:13:28  cfj
 * Merge with T9.5
 *
 * Revision 1.2.8.2  1993/04/14  15:03:28  cfj
 * Remove defined(__i860__) where not needed.
 *
 * Revision 1.3  1993/03/25  23:23:13  cfj
 * T9 Merge.
 *
 * Revision 1.2.8.1  1993/03/24  23:40:15  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.1.1.1  1993/05/03  17:32:31  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.6  1993/04/12  15:46:46  nina
 * Don't set if_dvnode unless #ifdef TNC
 *
 * Revision 2.5  93/03/22  19:43:37  nina
 * Modified to deal with new network interface name coventions
 * 
 * Revision 2.4  92/03/09  14:39:29  durriya
 * 	91/12/18  17:16:29  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.3  91/10/14  12:35:29  sjs
 * 	91/10/01  14:10:17  condict
 * 	Protect against multiple calls of loinit.
 * 
 * Revision 2.2  91/08/31  13:39:45  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:33:13  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10  90/10/07  14:31:50  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:10:11  gm]
 * 
 * Revision 1.9  90/09/23  15:55:20  devrcs
 * 	Add rtentry parameter to looutput().
 * 	[90/09/15  15:33:52  tmt]
 * 
 * Revision 1.8  90/07/27  08:58:54  devrcs
 * 	Update to BSD Reno release.
 * 	Handle arbitrary AF's with help from netisr.c.
 * 	[90/07/19  16:34:30  tmt]
 * 
 * Revision 1.7  90/04/27  19:12:10  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:24:17  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:17  devrcs
 * 	Use new netisr_input.
 * 	[90/04/09  16:07:55  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:28  gm
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
 * Revision 1.3  90/01/02  20:10:47  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:44:30  gm
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
 *	Base:	if_loop.c	7.9 (Berkeley) 9/20/89
 *	Merged:	if_loop.c	7.10 (Berkeley) 6/28/90
 */

/*
 * Loopback interface driver for protocol testing and timing.
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/ioctl.h"
#include "sys/errno.h"

#ifdef	OSF1_SERVER
#include <sys/synch.h>
#endif

#include "sys/mbuf.h"
#include "sys/socket.h"

#include "net/if.h"
#include "net/if_types.h"
#include "net/netisr.h"
#include "net/route.h"

#define	LOMTU	(1024+512)

LOCK_ASSERTL_DECL

struct	ifnet loif;

void
loinit()
{
	/* Protect against multiple calls: */
	static int first_time = 1;
	if (!first_time)
		return;
	first_time = 0;
	
	IFQ_LOCKINIT(&(loif.if_snd));
	NETSTAT_LOCKINIT(&(loif.if_slock));
}

void
loattach()
{
	register struct ifnet *ifp = &loif;
#if defined(TNC)
	extern node_t this_node;

	ifp->if_dvnode = this_node;
#endif
	ifp->if_name = "lo";
	ifp->if_mtu = LOMTU;
	ifp->if_flags = IFF_LOOPBACK;
	ifp->if_ioctl = loioctl;
	ifp->if_output = looutput;
	ifp->if_type = IFT_LOOP;
	ifp->if_hdrlen = 0;
	ifp->if_addrlen = 0;
	if_attach(ifp);
}

/* ARGSUSED */
looutput(ifp, m, dst, rt)
	struct ifnet *ifp;
	register struct mbuf *m;
	struct sockaddr *dst;
	struct rtentry *rt;
{
	int s;

	if ((m->m_flags & M_PKTHDR) == 0)
		panic("looutput no HDR");
	m->m_pkthdr.rcvif = ifp;

	s = splimp();
	NETSTAT_LOCK(&ifp->if_slock);
	ifp->if_opackets++;
	ifp->if_obytes += m->m_pkthdr.len;
	ifp->if_ipackets++;
	ifp->if_ibytes += m->m_pkthdr.len;
	NETSTAT_UNLOCK(&ifp->if_slock);
	splx(s);

	return netisr_input(netisr_af((int)dst->sa_family), m, (caddr_t)0, 0);
}

/*
 * Process an ioctl request.
 */
/* ARGSUSED */
loioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	int cmd;
	caddr_t data;
{
	int error = 0;

	switch (cmd) {

	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;
		/*
		 * Everything else is done at a higher level.
		 */
		break;

	default:
		error = EINVAL;
	}
	return (error);
}
