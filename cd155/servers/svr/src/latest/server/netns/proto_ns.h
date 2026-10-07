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
 * $Log: proto_ns.h,v $
 * Revision 1.5  1994/11/18  20:36:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:14:03  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:36:10  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:02:29  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:31:21  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:16  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:35  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:06  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:46:52  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:01  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:38:04  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:06:22  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:19:37  gm]
 * 
 * Revision 1.3  90/09/23  15:56:11  devrcs
 * 	Add ns_config. Delte ns_funnel.
 * 	[90/09/15  15:40:31  tmt]
 * 
 * 	Put _NO_PROTO back.
 * 	[90/09/05  17:32:31  tmt]
 * 
 * Revision 1.2  90/07/27  09:03:41  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	Look at GNUC as well as STDC so -traditional does prototypes.
 * 	[90/07/19  17:16:58  tmt]
 * 
 * $EndLog$
 */

#if	(defined(__STDC__) || defined(__GNUC__)) && !defined(_NO_PROTO)
# define	P(s) s
/* Avoid scoping problems */
struct mbuf; struct socket; struct ifnet;
struct nspcb; struct ns_ifaddr;
struct sockaddr; struct sockaddr_ns;
struct ns_addr; struct in_addr;
struct route; struct ifnet_en;
struct sppcb; struct spidp; 
#else
# define P(s) ()
#endif


/* idp_usrreq.c */
int	idp_input P((struct mbuf *, struct nspcb *));
int	idp_abort P((struct nspcb *));
struct	nspcb *idp_drop P((struct nspcb *, int));
int	idp_output P((struct nspcb *, struct mbuf *));
int	idp_ctloutput P((int, struct socket *, int, int, struct mbuf **));
int	idp_usrreq P((struct socket *, int, struct mbuf *,
				struct mbuf *, struct mbuf *));
int	idp_raw_usrreq P((struct socket *, int, struct mbuf *,
				struct mbuf *, struct mbuf *));

/* ns.c */
int	ns_control P((struct socket *, int, caddr_t, struct ifnet *));
int	ns_ifscrub P((struct ifnet *, struct ns_ifaddr *));
int	ns_ifinit P((struct ifnet *, struct ns_ifaddr *,
				struct sockaddr_ns *, int));
struct	ns_ifaddr *ns_iaonnetof P((struct ns_addr *));

/* somewhere */
u_short	ns_cksum P((struct mbuf *, int));

/* ns_error.c */
int	ns_err_x P((int));
int	ns_error P((struct mbuf *, int, int));
int	ns_printhost P((struct ns_addr *));
int	ns_err_input P((struct mbuf *));
u_long	nstime P((void));
int	ns_echo P((struct mbuf *));

/* ns_input.c */
int	ns_init P((void));
int	nsintr P((void));
int	idp_ctlinput P((int, caddr_t));
int	idp_forward P((struct mbuf *));
int	idp_do_route P((struct ns_addr *, struct route *));
int	idp_undo_route P((struct route *));
int	ns_watch_output P((struct mbuf *, struct ifnet *));

/* ns_ip.c */
struct	ifnet_en *nsipattach P((void));
int	nsipioctl P((struct ifnet *, int, caddr_t));
int	idpip_input P((struct mbuf *, struct ifnet *));
int	nsipoutput P((struct ifnet_en *, struct mbuf *, struct sockaddr *));
int	nsipstart P((struct ifnet *));
int	nsip_route P((struct mbuf *));
int	nsip_free P((struct ifnet *));
int	nsip_ctlinput P((int, struct sockaddr *));
int	nsip_rtchange P((struct in_addr *));

/* ns_output.c */
int	ns_output P((struct mbuf *, struct route *, int));

/* ns_pcb.c */
int	ns_pcballoc P((struct socket *, struct nspcb *));
int	ns_pcbbind P((struct nspcb *, struct mbuf *));
int	ns_pcbconnect P((struct nspcb *, struct mbuf *));
int	ns_pcbdisconnect P((struct nspcb *));
int	ns_pcbdetach P((struct nspcb *));
int	ns_setsockaddr P((struct nspcb *, struct mbuf *));
int	ns_setpeeraddr P((struct nspcb *, struct mbuf *));
int	ns_pcbnotify P((struct ns_addr *, int, int (*)(), long));
int	ns_rtchange P((struct nspcb *));
struct	nspcb *ns_pcblookup P((struct ns_addr *, u_short, int));

/* ns_proto.c */
int	ns_config ();		/* No prototypes here */

/* spp_debug.c */
int	spp_trace P((int, u_char, struct sppcb *, struct spidp *, int));

/* spp_usrreq.c */
int	spp_init P((void));
int	spp_input P((struct mbuf *, struct nspcb *));
int	spp_reass P((struct sppcb *, struct spidp *));
int	spp_ctlinput P((int, caddr_t));
int	spp_quench P((struct nspcb *));
int	spp_fixmtu P((struct nspcb *));
int	spp_output P((struct sppcb *, struct mbuf *));
int	spp_setpersist P((struct sppcb *));
int	spp_ctloutput P((int, struct socket *, int, int, struct mbuf **));
int	spp_usrreq P((struct socket *, int, struct mbuf *,
				struct mbuf *, struct mbuf *));
int	spp_usrreq_sp P((struct socket *, int, struct mbuf *,
				struct mbuf *, struct mbuf *));
int	spp_template P((struct sppcb *));
struct	sppcb *spp_close P((struct sppcb *));
struct	sppcb *spp_usrclosed P((struct sppcb *));
struct	sppcb *spp_disconnect P((struct sppcb *));
struct	sppcb *spp_drop P((struct sppcb *, int));
int	spp_abort P((struct nspcb *));
int	spp_fasttimo P((void));
int	spp_slowtimo P((void));
struct	sppcb *spp_timers P((struct sppcb *, int));

#undef P
