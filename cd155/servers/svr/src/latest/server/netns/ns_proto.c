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
 * $Log: ns_proto.c,v $
 * Revision 1.5  1994/11/18  20:36:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:13:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:36:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:02:24  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:31:16  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:33  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:05  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:46:46  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:38:58  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:38:00  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:19:30  gm]
 * 
 * Revision 1.5  90/09/23  15:56:08  devrcs
 * 	Include kern/parallel.h unconditionally.
 * 	[90/09/15  18:41:53  tmt]
 * 
 * 	Add configuration entry point. (More work to do.)
 * 	Make ns_funnel static.
 * 	[90/09/15  15:40:00  tmt]
 * 
 * Revision 1.4  90/09/13  11:48:26  devrcs
 * 	Add extra initialization for null "force unfunnel" op.
 * 	[90/08/28  11:24:15  tmt]
 * 
 * Revision 1.3  90/07/27  09:03:38  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:15:03  tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1984, 1985, 1986, 1987 Regents of the University of California.
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
 *	Base:	ns_proto.c	7.4 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/errno.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/domain.h"
#include "sys/protosw.h"

#include "netns/ns.h"
#include "netns/ns_config.h"

#if	MACH
#include "sys/sysconfig.h"
#include "kern/parallel.h"
#endif

/*
 * NS protocol family: IDP, ERR, PE, SPP, ROUTE.
 */

extern	struct domain nsdomain;

#if	!MACH
ns_config()
{
	return domain_add(&nsdomain);
}
#else
ns_config(op, indata, indatalen, outdata, outdatalen)
	sysconfig_op_t	op;
	ns_config_t *	indata;
	size_t		indatalen;
	ns_config_t *	outdata;
	size_t		outdatalen;
{
	int error;

	switch (op) {
	case SYSCONFIG_CONFIGURE:
		if (indata != NULL && indatalen == sizeof(ns_config_t) &&
		    indata->version == NS_CONFIG_VERSION_1) {
			/* TODO - Install input parameters, if any */
		}
		if ((error = domain_add(&nsdomain)) == 0) {
			if (outdata != NULL &&
			    outdatalen == sizeof(ns_config_t)) {
				/* TODO - Return some stuff */
			}
		}
		break;
	case SYSCONFIG_UNCONFIGURE:
		/* Will not work, for various reasons, but here anyway. */
		error = domain_del(&nsdomain);
		break;
	default:
		error = EINVAL;
		break;
	}
	return (error);
}
#endif

/*
 * XNS protocols are unmodified for parallelization, and are
 * thus unconditionally funneled with raised spl by the
 * socket layer from above and the netisr's from below.
 */

static void
ns_sanity()
{
	panic("ns unfunnel");
}

static void
ns_unfunnel(dfp)
	struct domain_funnel *dfp;
{
	dfp->unfunnel = ns_sanity;
	NETSPLX(dfp->object.spl);
	unix_release();
}

static void
ns_funnel(dfp)
	struct domain_funnel *dfp;
{
	if (dfp->unfunnel)
		panic("ns funnel");
	dfp->unfunnel = ns_unfunnel;
	unix_master();
	NETSPL(dfp->object.spl,net);
}

/* No "force unfunnel" op required with unix_master/spl */

#define V(x)	(void (*)())x

struct protosw nssw[] = {
{ 0,		&nsdomain,	0,		0,
  0,		idp_output,	0,		0,
  0,
  V(ns_init),	0,		0,		0
},
{ SOCK_DGRAM,	&nsdomain,	0,		PR_ATOMIC|PR_ADDR,
  0,		0,		V(idp_ctlinput),idp_ctloutput,
  idp_usrreq,
  0,		0,		0,		0
},
{ SOCK_STREAM,	&nsdomain,	NSPROTO_SPP,	PR_CONNREQUIRED|PR_WANTRCVD,
  V(spp_input),	0,		V(spp_ctlinput),spp_ctloutput,
  spp_usrreq,
  V(spp_init),	V(spp_fasttimo),V(spp_slowtimo),0
},
{ SOCK_SEQPACKET,&nsdomain,	NSPROTO_SPP,	PR_CONNREQUIRED|PR_WANTRCVD|PR_ATOMIC,
  V(spp_input),	0,		V(spp_ctlinput),spp_ctloutput,
  spp_usrreq_sp,
  0,		0,		0,		0
},
{ SOCK_RAW,	&nsdomain,	NSPROTO_RAW,	PR_ATOMIC|PR_ADDR,
  V(idp_input),	idp_output,	0,		idp_ctloutput,
  idp_raw_usrreq,
  0,		0,		0,		0
},
{ SOCK_RAW,	&nsdomain,	NSPROTO_ERROR,	PR_ATOMIC|PR_ADDR,
  V(idp_ctlinput),idp_output,	0,		idp_ctloutput,
  idp_raw_usrreq,
  0,		0,		0,		0
}
};

struct domain nsdomain =
    { AF_NS, "network systems", 0, 0, 0, 
      nssw, &nssw[sizeof(nssw)/sizeof(nssw[0])],
      0, 0, ns_funnel, 0 };

