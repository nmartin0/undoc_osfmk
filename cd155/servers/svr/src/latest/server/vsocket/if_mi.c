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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: if_mi.c,v $
 * Revision 1.15  1995/02/01  23:12:58  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.14  1994/11/18  20:51:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/11/03  01:31:29  slk
 *  Change Description: This is a two part fix.
 *  1)  In server/vsocket/if_mi.c remove stale #ifdef NOTDEF surrounding
 *  a counter decrement.
 *  2)  In server/net/route.c force the proper address to be placed in the
 *  ifaddr function pointer by calling ifa_withroute after the rn_delete.
 *  This causes the code to follow the path to decrementing the counter.
 *
 *  Reviewer(s): Nina Lepak, Mike Leibensperger, Surender Brahmaroutu
 *  Risk: Med
 *  Benefit or PTS #: 10950 H-0
 *       changing or deleting routes corrupts the routing table displayed
 *       by netstat -r
 *  Testing: Configured with one network server on and off the boot node,
 *         two network servers on same and different networks.  For each
 *         of those configurations performed a ping, telnet and rsh into
 *         and out of the paragon, on network servers and non-network server
 *         nodes.
 *         EATS: Lachman, TCP/IP, NFS, EAT, controlc, message, ipd,
 *         os_interfaces, xtrnl (VSX), misc, rmcall.
 *  Module(s): server/net/route.c
 *            server/vsocket/if_mi.c
 *
 * Revision 1.12  1994/10/21  17:46:17  nina
 *  Reviewer:yazz, hobbes
 *  Risk:Low
 *  Benefit or PTS #:10978, 11354
 *  Testing:NFS exercisers with various network configurations
 *  Module(s):./server/vsocket/if_mi.c
 *
 * 	For 10978, move the assertion for len <= MIMTU out of
 * 	the function mistart() to miconfigure(). This check
 * 	needs to be made before the mi_hdr is prepended to the
 * 	message.
 *
 *         For 11354, change the size of the buffer so that it is
 * 	large enough to accomodate MIMTU bytes of data plus an
 * 	mi_hdr plus all required mach headers.
 *
 * Revision 1.11  1994/06/17  15:05:27  paul
 * Changes in support of netstat. Fixed the TBL_RTREE code to talk to all the
 * network servers to get all the routes system wide, rather than just return
 * routes on the node the user happens to randomly get. Also, added a debug
 * version of the TBL_RTREE_NODE table() call which returns invisible MI routes.
 * Also, fixed the table() command to return tnc style <node>interface type
 * interface names i.e. <201>el0.
 *
 *  Reviewer: Bernie Keany
 *  Risk: M
 *  Benefit or PTS #: 7952 8059
 *  Testing: on Plymouth, with 1, 2, & 3 netservers, boot/non-boot configs
 *  Module(s): tnc/dvp_vpsops.c bsd/cmu_syscalls.c sys/table.h vsocket/if_mi.c
 * 	    vfs/vfs_syscalls.c
 *
 * Revision 1.10  1993/09/01  01:39:38  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.9  1993/07/14  18:48:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  21:12:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/05/20  16:04:05  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 3.15  93/08/26  10:50:41  mjl
 * Add some statistics macros.
 * 
 * Revision 3.14  93/08/23  00:10:50  mjl
 * Remove stale code, get rid of i860 compiler warning.
 * 
 * Revision 3.13  93/08/20  19:01:27  mjl
 * [LCCbug #0355] Replace ifa_withrelative() with more versatile
 * ifa_real_with_af() routine.  Add ANSI prototypes to forward decls.
 * 
 * Revision 3.12  93/08/19  16:47:26  mjl
 * Fix typo in obtaining IP packet address for debug printf.
 * 
 * Revision 3.11  93/08/19  13:28:17  mjl
 * [LCCbug #0351, #0352] Don't hide MI ifaddr's by dorking pointer with
 * hide_ifa()/show_ifa() macros, or bad things happen.  Better debug printfs.
 * 
 * Revision 3.10  93/07/27  11:40:14  mjl
 * Clean up some debug printfs.
 * 
 * Revision 3.9  93/05/25  10:39:06  mjl
 * Fix up comments and debug printfs, get rid of unused watchdog routine.
 * Set mbuf packet header flag to force IP forwarding of received MI packets.
 * 
 * Revision 3.8  93/05/13  11:35:34  mjl
 * Use new MIDOT() macro for quiet debugging of packet sends/receives.
 * 
 * Revision 3.7  93/05/07  19:09:24  mjl
 * Fix debugging printfs.  Temporarily make MI routes visible until full SSI
 * for netstat -r is implemented.
 * 
 * Revision 3.6  93/05/06  17:34:12  mjl
 * Clean up MIDEBUG() printfs, other miscellaneous cleanup.
 * 
 * Revision 3.5  93/05/05  22:21:01  mjl
 * Rewritten for MIv3.  Network driver routes; MI packet transport.
 * 
 * Revision 3.4  93/03/22  21:21:07  yazz
 * OSF lock changes.  Cthread_yield() calls become thread_yield().
 * 
 * Revision 3.3  93/02/25  17:52:29  nina
 * Save this_node in the mi ifnet structure; the MI interface's device node is
 * always the node where the ifnet struct resides by definition.  (mjl)
 * 
 * Revision 3.2  93/02/23  17:49:19  mjl
 * Turn off debug printfs.
 * 
 * Revision 3.1  93/02/22  17:21:40  mjl
 * Rewritten as a broadcast rather than a point-to-point interface.
 * 
 * Revision 3.0  92/04/20  17:11:33  bhk
 * Genesis The Mach Ipc network interface pseudo-driver
 * 
 *
 * Genisis 1/20/91 bhk
 *
 */

/*
 *  The MI driver is a pseudo interface driver used to pass information
 *  between network server nodes of a multicomputer using the nodes'
 *  protocol stacks.  It can also be used for regular protocol traffic.
 */

#include <mi.h>
#if NMI > 0

#include "net/net_globals.h"
#define splmi	splimp

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif


#include "sys/param.h"
#include "sys/ioctl.h"
#include "sys/errno.h"

#include "uxkern/import_mach.h"
#include "kern/queue.h"
#include "kern/zalloc.h"
#include "sys/synch.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "net/if.h"
#include "net/if_types.h"
#include "net/route.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"

#include "vsocket/mi_var.h"
#include "vsocket/vs_chouse.h"

#if !defined(INET)
#include "inet.h"
#endif
#if !defined(NS)
#include "ns.h"
#endif

#ifdef	MI_DEBUG
int mi_debug = 0; /* TEST: (MD_CONFIG|MD_ERR|MD_DOTS|MD_INFO) */
#endif

#define	Register	/* register /**/
#define	Static		/* static /**/

/*
 *  Globals
 */
zone_t	mi_msg_zone;
int	invisible_route_count = 0;	/* Since we attempt to keep the
					 * concept of invisible routes
					 * independent from whether or
					 * not an MI driver is present,
					 * invisible_route_count should
					 * be maintained in rtrequest().
					 * Instead we do it here, in
					 * the interests of not
					 * changing OSF/1 base code.
					 * This count is used by
					 * TBL_RTREE_NODE to make
					 * computing the number of
					 * visible routes easier.
					 */

/* Forward decls */
extern int 		mioutput (struct ifnet *,
				  struct mbuf *,
				  struct sockaddr *,
				  struct rtentry *);
extern int		mistart (struct ifnet *);
extern int		miioctl (struct ifnet *, int, caddr_t);
#ifdef	MI_DEBUG
extern void		miprtpkt (char *, unsigned char *, int);
#endif


/* True extern decls */
extern struct mbuf	*mclgetx();
extern node_t		this_node;


/*
 *  Boot-time initialization.
 */
void
miattach()
{
	MIDEBUG(MD_TRACE, ("miattach\n"));

	/*
	 *  Since MI interfaces are attached dynamically long after
	 *  ifinit() is called, we must make sure that the max_linkhdr
	 *  size calculated at boot time by ifinit() reflects the size
	 * of the MI link header.
	 */
	if (sizeof(mi_hdr_t) > max_linkhdr)
		max_linkhdr = sizeof(mi_hdr_t);
}


/*
 *  Initialize ifnet portion of MI interface structure.
 */
int
mi_iface_init(
	struct ifnet	*ifp,
	char		*name,
	iface_info_t	*iip)
{
	Register char	*cp;
	Register int	i;
	char		c;
	extern char	*net_interface_name();

	MIDEBUG(MD_CONFIG|MD_TRACE,
		("mi_iface_init(ifp 0x%x, name %s)\n", ifp, name));

	/*
	 *  When printing IFF_INVISIBLE interfaces, debug programs
	 *  should see "_<3>el0" corresponding to "el" unit 0 on
	 *  node 3.
	 */
#define	NotDigit(x)	((x) < '0' || (x) > '9')
	ifp->if_name = (char *)&((struct mi_softc *)ifp)->sc_ifname;
	cp = net_interface_name(name, NULL, NULL);    /* get past <> prefix */
	while ( NotDigit(*cp) )
		cp++;
	c = *cp;
	*cp = NULL;
	ifp->if_name[0]	= '_';
	strcpy(&ifp->if_name[1], name);		/* Now if_name is "_<x,y>ab" */
	*cp = c;
	for (i = 0; *cp && ! NotDigit(*cp); cp++)
		i = (10 * i) + (*cp - '0');
	ifp->if_unit = i;

	/*
	 *  ...And the other fascinating interface struct data....
	 */
	ifp->if_dvnode 		= this_node;
	ifp->if_mtu		= MIMTU;
	ifp->if_flags		= iip->ii_flags | IFF_INVISIBLE | IFF_RUNNING;
	ifp->if_addrlist	= NULL;
	ifp->if_type		= IFT_MI;
	ifp->if_start		= mistart;
	ifp->if_output		= mioutput;
	ifp->if_done		= NULL;
	ifp->if_ioctl		= miioctl;
	ifp->if_reset		= NULL;
	ifp->if_watchdog	= NULL;
	ifp->if_hdrlen		= sizeof(mi_hdr_t);
	ifp->if_addrlen		= 0;
	ifp->if_snd.ifq_maxlen	= MIMAXQLEN;

	IFQ_LOCKINIT(&(ifp->if_snd));
	NETSTAT_LOCKINIT(&(ifp->if_slock));
	if_attach(ifp);

	return (ESUCCESS);
}


/* ARGSUSED */
mioutput(ifp, m, dst, rt)
	struct ifnet		*ifp;
	Register struct mbuf	*m;
	struct sockaddr		*dst;
	struct rtentry		*rt;
{
	mi_softc_t		*sc = (mi_softc_t *)ifp;
	Register mi_hdr_t	*mih;
	int			s, error = ESUCCESS;
	int			usetrailers = FALSE;
	int			len;
	struct ip		*ip;
	struct ifaddr		*ifa;
	struct sockaddr_in	*sin;
	struct mbuf		*m1;

	MIDEBUG(MD_TRACE,
		("mioutput: %x &mbuf 0x%x &sockaddr 0x%x &rt 0x%x",
		 ifp, m, dst, rt));

	ASSERT(sc && sc->sc_assoc && sc->sc_assoc->ma_data_port);

	len = 0;
	for(m1 = m; m1; m1 = m1->m_next)  {
		MIDEBUG(MD_INFO,
			("mioutput: len 0x%x type 0x%x\n",
			 m1->m_len, m1->m_type));
		len += m1->m_len;
	}
	ASSERT(len <= MIMTU);

	if ( (ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
		return (ENETDOWN);
	if (dst->sa_family != AF_INET) {
		printf("mioutput: %s%d: can't handle address family %d\n",
		       ifp->if_name, ifp->if_unit, dst->sa_family);
		return (EAFNOSUPPORT);
	}

	/*
	 *  Now we have an mbuf chain with an IP packet to send.
	 *  Prepend our own MI header info.
	 */
	M_PREPEND(m, sizeof(mi_hdr_t), M_DONTWAIT);
	if(m == 0)
		return (ENOBUFS);
	MIDEBUG(MD_INFO,("mioutput: before pullup len = %d need %d\n",
			 m->m_len, sizeof(mi_hdr_t)));
	if (m->m_len < sizeof (mi_hdr_t) &&
	    (m = m_pullup(m, sizeof (mi_hdr_t))) == 0) {
		MIDEBUG(MD_ERR, ("mioutput: pullup failed\n"));
		m_freem(m);
		return (ENOBUFS);
	}
	mih = mtod(m, mi_hdr_t *);
	mih->mih_family = dst->sa_family;
	mih->mih_ifid = sc->sc_iface_id;
	mih->mih_dest = sc->sc_assoc->ma_data_port;

	/*
	 *  Enqueue packet for transmission.
	 */
	s = splimp();
	IFQ_LOCK(&ifp->if_snd);
	if (IF_QFULL(&ifp->if_snd)) {
		IF_DROP(&ifp->if_snd);
		IFQ_UNLOCK(&ifp->if_snd);
		splx(s);
		m_freem(m);
		return (ENOBUFS);
	} else {
		IF_ENQUEUE_NOLOCK(&ifp->if_snd, m);
		IFQ_UNLOCK(&ifp->if_snd);
		if ((ifp->if_flags & IFF_OACTIVE) == 0)
			(*ifp->if_start)(ifp);
		splx(s);

		NETSTAT_LOCK(&ifp->if_slock);
		ifp->if_opackets++;
		ifp->if_obytes += m->m_pkthdr.len;
		NETSTAT_UNLOCK(&ifp->if_slock);
	}

	return (ESUCCESS);
}


miinput(ifp, mih, m)
	struct ifnet	*ifp;
	mi_hdr_t	*mih;
	struct mbuf	*m;
{
	int	s;

	s = splimp();
	NETSTAT_LOCK(&ifp->if_slock);
	microtime(&ifp->if_lastchange);
	ifp->if_ibytes += m->m_pkthdr.len + sizeof(mi_hdr_t);
	ifp->if_ipackets++;
	NETSTAT_UNLOCK(&ifp->if_slock);
	splx(s);

	MIDEBUG(MD_RCV, ("miinput: fam %d, ifid 0x%x\n",
			 mih->mih_family, mih->mih_ifid));
	if ( netisr_input(netisr_af(mih->mih_family), m, NULL, 0) ) {
		s = splimp();
		NETSTAT_LOCK(&ifp->if_slock);
		ifp->if_noproto++;
		NETSTAT_UNLOCK(&ifp->if_slock);
		splx(s);
	}
}


/*
 *  Find a real (i.e. non-MI) interface address in the given address
 *  family.  This routine is only called in situations where such an
 *  ifaddr should exist, so it's bad news if nothing is found.
 */
struct ifaddr *
ifa_real_with_af(
	int		af)
{
	struct ifnet	*ifp;
	struct ifaddr	*ifa;

	for (ifp = ifnet; ifp; ifp = ifp->if_next) {
		if (ifp->if_type == IFT_LOOP ||
		    ifp->if_type == IFT_MI)
			continue;
		for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next)
			if (ifa->ifa_addr->sa_family == af)
				return (ifa);
	}
	panic("ifa_real_with_af: no real i/f for af=%d\n", af);
	/*NOTREACHED*/
}


/*
 *  This hook is called whenever a route is added or deleted
 *  for an MI interface.
 */
/*ARGSUSED*/
void
mi_rtrequest(
	int		req,
	struct rtentry	*rt,
	struct sockaddr	*sa)		/* gateway (indirect routes only) */
{
	struct ifaddr	*ifa;
	struct mi_softc	*sc;
	struct rtentry	*host_rt;
	int		rc;

	ASSERT(rt != NULL);
	sc = (struct mi_softc *)rt->rt_ifp;

	MIDEBUG(MD_TRACE,
		("mi_rtrequest(req=%d, rt=0x%x, sa=%s): ifp 0x%x ifa 0x%x\n",
		 req, rt, sockaddr_to_string(sa), sc, ifa));

	/*
	 *  The following two macros are used to prevent protocol code
	 *  from mistakenly finding the ifaddr's that we are hiding in
	 *  sc->sc_{h,n}_rtifa.  Typically protocols check that an
	 *  ifaddr's ifa_ifp field matches some particular interface
	 *  (see, for example, in_pcbconnect() ).  We use sizeof(int)
	 *  to prevent nasty page fault behavior, just in case.
	 */
#ifdef	HIDE_THE_IFADDRS	/* NOTDEF!!! -mjl */
#define hide_ifa(x) \
	{(x)->ifa_ifp = (struct ifnet *)((int)((x)->ifa_ifp) + sizeof(int));}
#define show_ifa(x) \
	{(x)->ifa_ifp = (struct ifnet *)((int)((x)->ifa_ifp) - sizeof(int));}
#else
#define hide_ifa(x)
#define show_ifa(x)
#endif

	switch (req) {
	case RTM_ADD:
		/*
		 *  All MI routes should be invisible, that is, routed
		 *  should not be told about them so that they are not
		 *  propagated outside the multicomputer.  Also, we
		 *  track the number of invisible routes to simplify
		 *  computing table(2) sizes for TBL_RTREE_* table ids.
		 *
		 *  XXX If the MD_INVR debug flag is set, the attempt
		 *  to print the route in the TBL_RTREE_NODE case in
		 *  pps_table() will lose, because it tries to access
		 *  a "hidden" ifp (dorked by hide_ifa() macro above).
		 *
		 *  XXX invisible_route_count must be kept on
		 *  a per address family basis!!!  AF_INET check
		 *  in server/bsd/cmu_syscalls.c:pps_table()
		 *  is temporary while MI driver only supports
		 *  this address family.
		 *
		 *  Now that TBL_RTREE_* checks all the network servers
		 *  for routes, we can mark the invisible ones so that
		 *  programs like netstat can just display the non-mi
		 *  routes.
		 */
		rt->rt_flags |= RTF_INVISIBLE;
		++invisible_route_count;

		/*
		 *  The route's interface rt->rt_ifp points at an MI
		 *  interface, but its address rt->rt_ifa refers to
		 *  the address of the corresponding real interface on
		 *  the remote node (see comment in mi_configure()).
		 *  That's bad, because if rt->rt_ifa is used as the
		 *  packet source address, replies won't be able to
		 *  get back here.  So we fudge it---we choose another
		 *  ifaddr to store in rt->rt_ifa.  This ifaddr should
		 *  (a) correspond to a local real interface, and (b)
		 *  probably be in the same address family as the
		 *  original rt->rt_ifa (though this cannot really be
		 *  guaranteed).  It's a thorny problem, and in the
		 *  interests of meeting my deadline I fudge it in a
		 *  big way.  Yet another place where we're AF_INET
		 *  specific, *sigh*....  XXX
		 */
		ifa = ifa_real_with_af(AF_INET);

		if (rt->rt_flags & RTF_HOST) {
			ASSERT(rt->rt_rmx.rmx_hopcount == 0);
			sc->sc_h_rtifa = rt->rt_ifa;
			rt->rt_ifa = ifa;
			hide_ifa(sc->sc_h_rtifa);
		} else {
			/*
			 *  The rt we are given is a network route to
			 *  the net to which the MI's corresponding
			 *  real interface is connected.  We want to
			 *  increase the hopcount to reflect the hop
			 *  across the MI association to reach that
			 *  network.
			 */
			rt->rt_rmx.rmx_hopcount += 1;
			MIDEBUG(MD_INFO,
				("mi_rtrequest: rt 0x%x hopcount %d (bumped)",
				 rt, rt->rt_rmx.rmx_hopcount));

			/* ...set up network route's ifaddr */
			sc->sc_n_rtifa = rt->rt_ifa;
			rt->rt_ifa = ifa;
			hide_ifa(sc->sc_n_rtifa);
		}
		break;

	case RTM_DELETE:
		--invisible_route_count;
		/*
		 *  Restore saved route ifaddr just in case.
		 */
		if (rt->rt_flags & RTF_HOST) {
			show_ifa(sc->sc_h_rtifa);
			rt->rt_ifa = sc->sc_h_rtifa;
		} else {
			show_ifa(sc->sc_n_rtifa);
			rt->rt_ifa = sc->sc_n_rtifa;
		}
		break;
	}
#undef	hide_ifa
#undef	show_ifa
}


int
miioctl(
	struct ifnet	*ifp,
	int		cmd,
	caddr_t		data)
{
	struct ifaddr	*ifa = (struct ifaddr *)data;

	MIDEBUG(MD_TRACE,
		("miioctl(ifp=0x%x cmd=0x%x data=0x%x)\n", ifp, cmd, data));

	switch (cmd) {
	case SIOCSIFADDR:
		/*
		 *  Set up rtrequest() hook.
		 */
		ifa->ifa_rtrequest = mi_rtrequest;
		break;
	}

	return (ESUCCESS);
}


int
mistart(ifp)
	struct ifnet *ifp;
{
	Register struct mbuf	*m, *m1;
	int			len, bcast = 0;
	caddr_t			addr;
	char			pktbuf[MI_ALLOC];
	kern_return_t		kr;
	mi_hdr_t		*mih;
	struct ip		*ip;

	MIDEBUG(MD_TRACE, ("mistart: &ifp = 0x%x\n",ifp));

	IF_DEQUEUE(&ifp->if_snd, m);
	if (m == 0)
		return ESUCCESS;
	len = 0;
	for(m1 = m; m1; m1 = m1->m_next)  {
		MIDEBUG(MD_INFO,
			("mistart: len 0x%x type 0x%x\n",
			 m1->m_len, m1->m_type));
		len += m1->m_len;
	}

	if(len != m->m_len) {
		/*
		 *  Multiple mbufs, so copy data to contiguous stack space.
		 */
		(void) m_copydata(m, 0, len, (caddr_t)pktbuf);
		mih = (mi_hdr_t *)pktbuf;
	} else {
		mih = mtod(m, mi_hdr_t *);
	}

	/*
	 *  XXX Maybe use the MIG MsgOption cruft to set non-blocking,
	 *  and drop packets (and print a warning) if the RPC would
	 *  block?  This will let us know if we need more MI listener
	 *  threads....
	 */
	kr = r_mi_pkt(mih->mih_dest, (caddr_t)mih, len);
	if ( kr != KERN_SUCCESS ) {
		MIDEBUG(MD_ERR|MD_SND,
			("mistart: mi_pkt(0x%x, 0x%x, %d) DROPPED: kr 0x%x\n",
			 mih->mih_dest, mih, len, kr));
		NETSTAT_LOCK(&ifp->if_slock);
		ifp->if_oerrors++;
		NETSTAT_UNLOCK(&ifp->if_slock);
	} else {
		/* AF_INET assumed here. */
		struct ip *ip = (struct ip *)(mih + 1);

		MIDOT(',');
		MIDEBUG(MD_SND,
		      ("mistart: sent (src=%x dst=%x) to node %d, ifid 0x%x\n",
		       ip->ip_src.s_addr, ip->ip_dst.s_addr,
		       ((struct mi_softc *)ifp)->sc_assoc->ma_remote_node,
		       mih->mih_ifid));
	}
	m_freem(m);
	return (ESUCCESS);
}


mi_mbuf_free(msg)
char	*msg;
{
	ZFREE(mi_msg_zone, (vm_offset_t)msg);
	MISTAT(mis_zfree);
}


S_mi_pkt(
	mach_port_t	server_port,
	char		*data,
	int		len)
{
	/*
	 *  This is just a stub; MI packets are really received by
	 *  a dedicated thread and not via the ux_server port set.
	 */
	panic("S_mi_pkt: packet sent to wrong server port!\n");
}


/* 
 *  NB that MI packets are received here, *not* in S_mi_pkt().
 */
void
mi_isr_thread()
{
	Register mi_mach_pkt_t 	*msg	= NULL;
	Register struct ifnet	*ifp;
	Register mi_data_pkt_t	*pkt;
	struct mi_assoc		*ma;
	kern_return_t		kr;
	struct mbuf		*m;
	int			len;
	struct ip		*ip;

	cthread_set_name(cthread_self(), "mi input thread");

	MIDEBUG(MD_TRACE, ("mi_isr_thread: started\n"));

	while(1) {

		/*
		 *  Allocate a message buffer and receive a message.
		 *  If msg is NULL we still have a buffer from the last
		 *  trip through the loop.
		 */
		if ( msg == NULL ) {
			ZALLOC(mi_msg_zone, msg, mi_mach_pkt_t *);
			if ( msg == NULL ) {
				printf("mi_isr_thread: zone alloc failed\n");
				thread_yield();
				continue;
			}
			MISTAT(mis_zalloc);
		}
		kr = mach_msg((mach_msg_header_t *)msg, MACH_RCV_MSG,
			      0, MI_ALLOC, mi_data_port_set,
			      MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
		if ( kr != MACH_MSG_SUCCESS ) {
			printf("mi_isr_thread: mach_msg: kr 0x%x\n", kr);
#ifdef	NOTDEF
			/* ifp isn't set yet so we don't know where to
			 * record this error!  XXX */
			NETSTAT_LOCK(&ifp->if_slock);
			ifp->if_ierrors++;
			NETSTAT_UNLOCK(&ifp->if_slock);
#endif
			continue;
		}

		/*
		 *  We've got the packet---now we have to figure out
		 *  what to tell the IP layer re. which interface it
		 *  arrived on.  If the packet is destined for this
		 *  node (i.e. for the IP address of a local real
		 *  interface), use that interface.
		 *
#ifdef	MI_IP_FORWARDING
		 *  XXX If not, then we are forwarding this packet,
		 *  and we must use an interface other than the one we
		 *  intend to use to forward the packet (we don't want
		 *  to cause any bogus IP redirects).  That means we
		 *  have to use an MI interface, because we can't be
		 *  guaranteed of finding a non-MI interface.
#endif
		 */
		pkt = (mi_data_pkt_t *)msg->mi_pkt_data;
#ifdef	MI_IP_FORWARDING
		MI_DATAPORT_TO_ASSOC(msg->Head.msgh_local_port, ma);
		ifp = (struct ifnet *)ma->ma_comrads;
#else
		ifp = (struct ifnet *)pkt->mi_hdr.mih_ifid;
#endif
		ASSERT(ifp);
		if (!(ifp->if_flags & IFF_UP))
			continue;
#ifdef	NOTDEF
		MIDEBUG(MD_INFO,
			("mi_isr_thread: msgtl_number %d msgtl_size %d\n",
			 msg->Type.msgtl_number, msg->Type.msgtl_size));
#endif
		if (msg->Type.msgtl_number == 0)
			continue;
		len = msg->Type.msgtl_number - sizeof(mi_hdr_t);

		MIDOT('.');
		ip = (struct ip *)((&pkt->mi_hdr) + 1);
		MIDEBUG(MD_RCV,
			("mi_isr: (src=%x dst=%x) recvd for ifid 0x%x\n",
			 ip->ip_src.s_addr, ip->ip_dst.s_addr, ifp));
		MIDEBUGX(MD_PKT, miprtpkt("contents:\n",
					  (unsigned char *)pkt->mi_data,
					  len));

		m = mclgetx(mi_mbuf_free, (caddr_t)msg,
			    pkt->mi_data, len, M_WAIT);
		if (m == NULL) {
			printf("mi_isr_thread: out of mbufs\n");
			NETSTAT_LOCK(&ifp->if_slock);
			ifp->if_ierrors++;
			NETSTAT_UNLOCK(&ifp->if_slock);
			continue;
		}
		msg = NULL;	/* mbuf now owns the zalloc'ed buffer */

		m->m_pkthdr.len = len;
		m->m_flags |= (M_PKTHDR|M_TNC_FORW);
		m->m_pkthdr.rcvif = ifp;

		miinput(ifp, &pkt->mi_hdr, m);
	}
	/*NOTREACHED*/
}


#ifdef	MI_DEBUG
void
miprtpkt(
	char			*s,
	Register unsigned char	*d,
	Register int		n)
{
	int	i;

	printf("%s:\n",s);
	while (n) {
		for (i = 0; n && i < 16; i++, n--)
			printf(" %02x",*d++);
		printf("\n");
	}
}
#endif	/* MI_DEBUG */
	
#endif	/* NMI > 0 */
