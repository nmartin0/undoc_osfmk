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
 * $Log: proto_net.h,v $
 * Revision 1.6  1995/02/01  21:34:01  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:34:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:08:25  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:04:37  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.3  1993/07/01  19:28:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:56  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:39  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:18  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:52  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.5  93/07/07  10:33:25  mjl
 * [LCC #0314] Add prototype decl for net_threadstart_deferred().
 * 
 * Revision 2.4  1993/03/24  15:38:13  loverso
 * 	netinit() now takes a value.(loverso)
 *
 * Revision 2.3  92/05/24  14:34:37  pjg
 * 	updated prototype for net_threadstart,  now takes char *
 * 	as third param.
 * 	[92/05/20            srl]
 * 
 * Revision 2.2  91/08/31  13:40:31  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:33:46  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12.4.2  91/03/15  17:48:05  tmt
 * 	Correct argument type to sl_uncompress_tcp().
 * 	Update prototypes to match code.
 * 	[91/03/13  19:06:33  tmt]
 * 
 * Revision 1.12  90/10/07  14:32:31  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:11:15  gm]
 * 
 * 	Move arpresolve to net/if_ethersubr.c from netinet/if_ether.c.
 * 	[90/09/29  19:46:10  tmt]
 * 
 * Revision 1.11  90/09/23  15:55:33  devrcs
 * 	Add rtentry parameters to looutput, sloutput. Remove route_funnel.
 * 	[90/09/15  15:35:00  tmt]
 * 
 * 	Put _NO_PROTO back.
 * 	[90/09/05  17:29:36  tmt]
 * 
 * Revision 1.10  90/08/24  12:13:55  devrcs
 * 	Move arpwhohas, arpioctl here for dynamic ARP attach.
 * 	Add arg to rtinithead.
 * 	[90/08/17  18:05:50  tmt]
 * 
 * Revision 1.9  90/07/27  08:59:27  devrcs
 * 	Checkpoint at Reno merge.
 * 	Add and update several routines.
 * 	Look at GNUC as well as STDC so -traditional does prototypes.
 * 	[90/07/20  17:23:11  tmt]
 * 
 * Revision 1.8  90/07/05  23:12:43  devrcs
 * 	Add placeholders for function prototypes to work. Add route_funnel.
 * 	[90/07/03  18:47:56  tmt]
 * 
 * Revision 1.7  90/04/27  19:13:16  devrcs
 * 	Update SLIP, routing init routines. Remove all #ifdef's.
 * 	[90/04/20  12:31:07  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:28  devrcs
 * 	Lotsa new return types, mostly void's. Update generally.
 * 	[90/04/09  16:17:11  tmt]
 * 
 * Revision 1.5  90/03/27  13:21:16  gm
 * 	Look at _NO_PROTO for function prototype select.
 * 	[90/03/09  12:58:12  tmt]
 * 
 * Revision 1.4  90/01/18  08:44:01  gm
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
 * Revision 1.3  90/01/02  20:10:55  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:46:20  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
#if	(defined(__STDC__) || defined(__GNUC__)) && !defined(_NO_PROTO)
# define	P(s) s
/* Avoid scoping problems */
struct sockaddr; struct socket; struct sockproto;
struct mbuf; struct ifnet; struct ifqueue; struct ifaddr;
struct ether_header; struct tty; struct slcompress;
struct ip; struct in_addr; struct arpcom;
struct domain;
struct radix_node; struct radix_node_head;
struct rawcb; struct route; struct rtentry;
struct rt_metrics;
#else
# define P(s) ()
#endif


/* if.c */
void	ifinit P((void));
void	ifubareset P((int));
void	ifreset P((int));
void	if_attach P((struct ifnet *));
struct	ifaddr *ifa_ifwithaddr P((struct sockaddr *));
struct	ifaddr *ifa_ifwithdstaddr P((struct sockaddr *));
struct	ifaddr *ifa_ifwithnet P((struct sockaddr *));
struct	ifaddr *ifa_ifwithaf P((int));
struct	ifaddr *ifaof_ifpforaddr P((struct sockaddr *, struct ifnet *));
void	link_rtrequest P((int, struct rtentry *, struct sockaddr *));
void	if_down P((struct ifnet *));
void	if_qflush P((struct ifqueue *));
int	if_slowtimo P((void));
struct	ifnet *ifunit P((char *));
int	ifioctl P((struct socket *, int, caddr_t));
int	ifconf P((int, caddr_t));

/* if_ethersubr.c */
int	ether_output P((struct ifnet *, struct mbuf *, struct sockaddr *,
			struct rtentry *));
void	ether_input P((struct ifnet *, struct ether_header *, struct mbuf *));
void	arpwhohas P((struct arpcom *, struct in_addr *));
int	arpioctl P((int, caddr_t));
int	arpresolve P((struct arpcom *, struct mbuf *, struct in_addr *,
				u_char *, int *));
char	*ether_sprintf P((u_char *));

/* if_loop.c */
void	loinit P((void));
void	loattach P((void));
int	looutput P((struct ifnet *, struct mbuf *, struct sockaddr *,
			struct rtentry *));
int	loioctl P((struct ifnet *, int, caddr_t));

/* if_sl.c */
void	slattach P((void));
int	slopen P((dev_t, struct tty *));
void	slclose P((struct tty *));
int	sltioctl P((struct tty *, int, caddr_t, int));
int	sloutput P((struct ifnet *, struct mbuf *, struct sockaddr *,
			struct rtentry *));
void	slstart P((struct tty *));
void	slinput P((int, struct tty *));
int	slioctl P((struct ifnet *, int, caddr_t));

/* slcompress.c */
void	sl_compress_init P((struct slcompress *));
int	sl_compress_tcp P((struct mbuf *, struct ip *,struct slcompress *,int));
int	sl_uncompress_tcp P((u_char **, int, int, struct slcompress *));

/* netisr.c */
void	netinit P((boolean_t));
int	netisr_add P((int, void (*)(), struct ifqueue *, struct domain *));
int	netisr_del P((int));
int	netisr_input P((int, struct mbuf *, caddr_t, int));
int	netisr_af P((int));
void	Netintr P((void));
void	net_threadstart P((int (*)(), int, char *));
int	netisr_thread P((void));
#if	defined(TNC) && NETISR_THREAD
void	net_threadstart_deferred P((void));
#endif

/* radix.c */
struct	radix_node *rn_search P((caddr_t, struct radix_node *));
struct	radix_node *rn_search_m P((caddr_t, struct radix_node *, caddr_t));
struct	radix_node *rn_match P((caddr_t, struct radix_node *));
struct	radix_node *rn_newpair P((caddr_t, int, struct radix_node[2]));
struct	radix_node *rn_insert P((caddr_t, struct radix_node *, int *,
			struct radix_node[2]));
struct	radix_node *rn_addmask P((caddr_t, int, int));
struct	radix_node *rn_addroute P((caddr_t, caddr_t, struct radix_node *,
			struct radix_node[2]));
struct	radix_node *rn_delete P((caddr_t, caddr_t, struct radix_node *));
int	rn_inithead P((struct radix_node_head **, int, int));

/* raw_cb.c */
int	raw_attach P((struct socket *, int));
void	raw_detach P((struct rawcb *));
void	raw_disconnect P((struct rawcb *));
int	raw_bind P((struct socket *, struct mbuf *));

/* raw_usrreq.c */
void	raw_init P((void));
int	raw_input P((struct mbuf *, struct sockproto *,
			struct sockaddr *, struct sockaddr *));
void	raw_ctlinput P((int, struct sockaddr *));
int	raw_usrreq P((struct socket *, int, struct mbuf *, struct mbuf *,
			struct mbuf *));

/* route.c */
void	rtinithead P((int, int, void (*)()));
void	rtalloc P((struct route *));
struct	rtentry *rtalloc1 P((struct sockaddr *, int));
void	rtfree P((struct rtentry *));
void	rtredirect P((struct sockaddr *, struct sockaddr *, struct sockaddr *,
			int, struct sockaddr *, struct rtentry **));
int	rtioctl P((struct socket *, int, caddr_t));
struct	ifaddr *ifa_ifwithroute P((int, struct sockaddr *, struct sockaddr *));
int	rtrequest P((int, struct sockaddr *, struct sockaddr *,
			struct sockaddr *, int, struct rtentry **));
void	rt_maskedcopy P((struct sockaddr *, struct sockaddr *,
			struct sockaddr *));
int	rtinit P((struct ifaddr *, int, int));

/* rtsock.c */
int	route_usrreq P((struct socket *, int, struct mbuf *, struct mbuf *,
			struct mbuf *));
int	route_output P((struct mbuf *, struct socket *));
void	rt_setmetrics P((u_long, struct rt_metrics *, struct rt_metrics *));
void	m_copyback P((struct mbuf *, int, int, caddr_t));
void	rt_missmsg P((int, struct sockaddr *, struct sockaddr *,
			struct sockaddr *, struct sockaddr *, int, int));
int	route_config P((void));

#undef P
