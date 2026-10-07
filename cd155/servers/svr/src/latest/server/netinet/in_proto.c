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
 * $Log: in_proto.c,v $
 * Revision 1.6  1994/12/23  01:18:31  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:10356
 *  Testing:Test case and TCP/IP EATS with various network configs
 *  Module(s):
 *
 * 	net/rtsock.c
 * 	netinet/in_proto.c
 * 	vsocket/vs_chouse.h
 * 	vsocket/vs_chouse.c
 * 	vsocket/vs.defs
 * 	vsocket/vs_ipc.c
 * 	vsocket/vs_subr.c
 * 	vsocket/vs_netops.c
 *
 *   The was only seen in systems with multiple network configurations
 *   when a user attempted to bind to INADDR_ANY/port 0. In this
 *   situation, the system is supposed to pick the port number for
 *   the caller. The problem occurred because the VSOCKET code
 *   didn't ensure that the primary socket and each secondary socket used
 *   the same port number.
 *
 *   Now, a bind to port 0 results in an RPC to the clearinghouse to
 *   pick a port number, based on the domain and protocol of the socket.
 *
 * Revision 1.5  1994/11/18  20:34:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/09/20  18:26:22  yazz
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
 * Revision 1.3  1993/05/06  20:26:19  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:39  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1993/02/02  15:23:12  rabii
 * 	Remove CONST from definition of protosw so afs can substitute it's
 * 	vectors (rabii)
 *
 * Revision 1.2  1992/11/30  22:28:46  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:48  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/05/24  14:36:13  pjg
 * 	92/03/23  18:03:47  condict
 * 	Include sys/user.h, for unix_master/release.
 * 	[92/05/18            srl]
 * 
 * Revision 2.2  91/08/31  13:43:11  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:35:31  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12  90/10/07  14:34:33  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:14:19  gm]
 * 
 * Revision 1.11  90/09/23  15:55:48  devrcs
 * 	Add configuration entry points. (More work to do.)
 * 	Make inet_funnel static.
 * 	[90/09/15  15:37:02  tmt]
 * 
 * Revision 1.10  90/09/13  11:48:11  devrcs
 * 	Add extra initialization for null "force unfunnel" op.
 * 	[90/08/28  11:23:05  tmt]
 * 
 * Revision 1.9  90/08/24  12:14:28  devrcs
 * 	Test NS option from file, not make line.
 * 	[90/08/19  14:17:29  tmt]
 * 
 * Revision 1.8  90/07/27  09:00:57  devrcs
 * 	Update to BSD Reno release.
 * 	Move rtinithead() to ip_init().
 * 	Add NSIP entry if available.
 * 	[90/07/19  17:30:29  tmt]
 * 
 * Revision 1.7  90/07/05  23:13:14  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	[90/07/03  18:52:29  tmt]
 * 
 * Revision 1.6  90/04/27  19:15:42  devrcs
 * 	Add routing initialization to inet_config. Delete commas.
 * 	[90/04/20  12:51:53  tmt]
 * 
 * Revision 1.5  90/04/14  00:33:09  devrcs
 * 	Add config install entry for dynamic domains.
 * 	Delete function declarations.
 * 	[90/04/09  16:29:31  tmt]
 * 
 * Revision 1.4  90/01/18  08:45:13  gm
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
 * Revision 1.3  90/01/02  20:11:29  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:11:10  gm
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
 *	Base:	in_proto.c	7.4 (Berkeley) 4/22/89
 *	Merged:	in_proto.c	7.5 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/errno.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/domain.h"
#include "sys/protosw.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/inet_config.h"

#if	MACH
#include "sys/sysconfig.h"
#if	NETSYNC_SPL
#include "kern/parallel.h"
#endif
#endif

/* NSIP encapsulation option check */
#ifndef	NS
#include <ns.h>
#endif
#if	NS > 0
#include "netns/proto_ns.h"
#endif

/*
 * TCP/IP protocol family: IP, ICMP, UDP, TCP.
 */

extern	struct domain inetdomain;

#if	!MACH
inet_config()
{
	return domain_add(&inetdomain);
}
#else
inet_config(op, indata, indatalen, outdata, outdatalen)
	sysconfig_op_t	op;
	inet_config_t *	indata;
	size_t		indatalen;
	inet_config_t *	outdata;
	size_t		outdatalen;
{
	int error;

	switch (op) {
	case SYSCONFIG_CONFIGURE:
		if (indata != NULL && indatalen == sizeof(inet_config_t) &&
		    indata->version == INET_CONFIG_VERSION_1) {
			/* TODO - Install input parameters, if any */
		}
		if ((error = domain_add(&inetdomain)) == 0) {
			if (outdata != NULL &&
			    outdatalen == sizeof(inet_config_t)) {
				/* TODO - Return some stuff */
			}
		}
		break;
	case SYSCONFIG_UNCONFIGURE:
		/* Will not work, for various reasons, but here anyway. */
		error = domain_del(&inetdomain);
		break;
	default:
		error = EINVAL;
		break;
	}
	return (error);
}
#endif

#if	NETSYNC_SPL
#include <sys/user.h>  /* For unix_master/unix_release */
static void
inet_sanity()
{
	panic("inet unfunnel");
}

static void
inet_unfunnel(dfp)
	struct domain_funnel *dfp;
{
	dfp->unfunnel = inet_sanity;
	NETSPLX(dfp->object.spl);
	unix_release();
}

static void
inet_funnel(dfp)
	struct domain_funnel *dfp;
{
	if (dfp->unfunnel)
		panic("inet funnel");
	dfp->unfunnel = inet_unfunnel;
	unix_master();
	NETSPL(dfp->object.spl,net);
}

/* No "force unfunnel" op required with unix_master/spl */

#else
#define inet_funnel	0
#endif

struct protosw inetsw[] = {
{ 0,		&inetdomain,	0,		0,
  0,		ip_output,	0,		0,
  0,
  ip_init,	0,		ip_slowtimo,	ip_drain
},
{ SOCK_DGRAM,	&inetdomain,	IPPROTO_UDP,	PR_ATOMIC|PR_ADDR,
  udp_input,	0,		udp_ctlinput,	ip_ctloutput,
  udp_usrreq,
  udp_init,	0,		0,		0,
},
{ SOCK_STREAM,	&inetdomain,	IPPROTO_TCP,	PR_CONNREQUIRED|PR_WANTRCVD,
  tcp_input,	0,		tcp_ctlinput,	tcp_ctloutput,
  tcp_usrreq,
  tcp_init,	tcp_fasttimo,	tcp_slowtimo,	tcp_drain
},
{ SOCK_RAW,	&inetdomain,	IPPROTO_RAW,	PR_ATOMIC|PR_ADDR,
  rip_input,	rip_output,	0,		rip_ctloutput,
  rip_usrreq,
  0,		0,		0,		0
},
{ SOCK_RAW,	&inetdomain,	IPPROTO_ICMP,	PR_ATOMIC|PR_ADDR,
  icmp_input,	rip_output,	0,		rip_ctloutput,
  rip_usrreq,
  0,		0,		0,		0
},
#if	(NS > 0) && NETSYNC_SPL
#define V(x)	(void (*)())x
{ SOCK_RAW,	&inetdomain,	IPPROTO_IDP,	PR_ATOMIC|PR_ADDR,
  V(idpip_input),rip_output,	V(nsip_ctlinput),0,
  rip_usrreq,
  0,		0,		0,		0
},
#endif
	/* raw wildcard */
{ SOCK_RAW,	&inetdomain,	0,		PR_ATOMIC|PR_ADDR,
  rip_input,	rip_output,	0,		rip_ctloutput,
  rip_usrreq,
  0,		0,		0,		0
}
};

int size_inetsw = sizeof(inetsw)/sizeof(struct protosw);

struct domain inetdomain =
    { AF_INET, "internet", 0, 0, 0, 
      inetsw, &inetsw[sizeof(inetsw)/sizeof(inetsw[0])],
      0, 0, inet_funnel, 0 };
