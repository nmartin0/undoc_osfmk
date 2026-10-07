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
 * $Log: ns_output.c,v $
 * Revision 1.5  1994/11/18  20:36:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:13:30  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:35:45  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:02:09  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:31:09  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:22  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:01  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:46:27  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:38:45  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:37:45  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:19:09  gm]
 * 
 * Revision 1.3  90/07/27  09:03:27  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:13:19  tmt]
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
 *	Base:	ns_output.c	7.7 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/errno.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/socketvar.h"

#include "net/if.h"
#include "net/route.h"

#include "netns/ns.h"
#include "netns/ns_if.h"
#include "netns/idp.h"
#include "netns/idp_var.h"

int ns_hold_output = 0;
int ns_copy_output = 0;
int ns_output_cnt = 0;
struct mbuf *ns_lastout;

ns_output(m0, ro, flags)
	struct mbuf *m0;
	struct route *ro;
	int flags;
{
	register struct idp *idp = mtod(m0, struct idp *);
	register struct ifnet *ifp = 0;
	int error = 0;
	struct route idproute;
	struct sockaddr_ns *dst;
	extern int idpcksum;

	if (ns_hold_output) {
		if (ns_lastout) {
			(void)m_free(ns_lastout);
		}
		ns_lastout = m_copy(m0, 0, (int)M_COPYALL);
	}
	/*
	 * Route packet.
	 */
	if (ro == 0) {
		ro = &idproute;
		bzero((caddr_t)ro, sizeof (*ro));
	}
	dst = (struct sockaddr_ns *)&ro->ro_dst;
	if (ro->ro_rt == 0) {
		dst->sns_family = AF_NS;
		dst->sns_len = sizeof (*dst);
		dst->sns_addr = idp->idp_dna;
		dst->sns_addr.x_port = 0;
		/*
		 * If routing to interface only,
		 * short circuit routing lookup.
		 */
		if (flags & NS_ROUTETOIF) {
			struct ns_ifaddr *ia = ns_iaonnetof(&idp->idp_dna);

			if (ia == 0) {
				error = ENETUNREACH;
				goto bad;
			}
			ifp = ia->ia_ifp;
			goto gotif;
		}
		rtalloc(ro);
	} else if ((ro->ro_rt->rt_flags & RTF_UP) == 0) {
		/*
		 * The old route has gone away; try for a new one.
		 */
		rtfree(ro->ro_rt);
		ro->ro_rt = NULL;
		rtalloc(ro);
	}
	if (ro->ro_rt == 0 || (ifp = ro->ro_rt->rt_ifp) == 0) {
		error = ENETUNREACH;
		goto bad;
	}
	ro->ro_rt->rt_use++;
	if (ro->ro_rt->rt_flags & (RTF_GATEWAY|RTF_HOST))
		dst = (struct sockaddr_ns *)ro->ro_rt->rt_gateway;
gotif:

	/*
	 * Look for multicast addresses and
	 * and verify user is allowed to send
	 * such a packet.
	 */
	if (dst->sns_addr.x_host.c_host[0]&1) {
		if ((ifp->if_flags & IFF_BROADCAST) == 0) {
			error = EADDRNOTAVAIL;
			goto bad;
		}
		if ((flags & NS_ALLOWBROADCAST) == 0) {
			error = EACCES;
			goto bad;
		}
	}

	if (htons(idp->idp_len) <= ifp->if_mtu) {
		ns_output_cnt++;
		if (ns_copy_output) {
			ns_watch_output(m0, ifp);
		}
		error = (*ifp->if_output)(ifp, m0,
					(struct sockaddr *)dst, ro->ro_rt);
		goto done;
	} else error = EMSGSIZE;


bad:
	if (ns_copy_output) {
		ns_watch_output(m0, ifp);
	}
	m_freem(m0);
done:
	if (ro == &idproute && (flags & NS_ROUTETOIF) == 0 && ro->ro_rt) {
		RTFREE(ro->ro_rt);
		ro->ro_rt = 0;
	}
	return (error);
}
