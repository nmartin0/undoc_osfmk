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
 * $Log: proto_inet.h,v $
 * Revision 1.4  1994/11/18  20:35:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:26:52  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:00  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:37  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:21:27  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/10/22  23:05:41  hobbes
 * Added the RFC_1323 extensions.
 *
 * Revision 2.2  1991/08/31  13:43:55  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:36:20  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10.4.2  91/03/15  17:49:14  tmt
 * 	Update prototypes to match code.
 * 	[91/03/13  19:11:25  tmt]
 * 
 * Revision 1.10  90/10/07  14:35:18  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:15:39  gm]
 * 
 * 	Move arpresolve to net.
 * 	[90/09/29  19:47:38  tmt]
 * 
 * Revision 1.9  90/09/23  15:55:58  devrcs
 * 	De-prototype inet_config, delete inet_funnel.
 * 	[90/09/15  15:37:35  tmt]
 * 
 * 	Put _NO_PROTO back.
 * 	[90/09/05  17:31:33  tmt]
 * 
 * Revision 1.8  90/08/24  12:14:47  devrcs
 * 	Move arpwhohas, arpioctl out to net/ (for dynamic attach).
 * 	[90/08/17  18:09:31  tmt]
 * 
 * Revision 1.7  90/07/27  09:01:32  devrcs
 * 	Checkpoint at Reno merge.
 * 	Add and update several routines.
 * 	Look at GNUC as well as STDC so -traditional does prototypes.
 * 	[90/07/19  17:00:59  tmt]
 * 
 * Revision 1.6  90/07/05  23:13:22  devrcs
 * 	Add placeholders for function prototypes to work. Add inet_funnel.
 * 	Add #define to hide tcp_seq typedef, evaluate efficacy.
 * 	[90/07/03  18:54:19  tmt]
 * 
 * Revision 1.5  90/06/22  20:39:16  devrcs
 * 	Add in_pcbfree(), arpoutput(), fix iptime() parameter.
 * 	[90/06/12  18:08:03  tmt]
 * 
 * Revision 1.4  90/04/27  19:16:23  devrcs
 * 	Add mach_net_xxx routines. Delete #ifdef's.
 * 	[90/04/20  12:58:44  tmt]
 * 
 * Revision 1.3  90/04/14  00:33:19  devrcs
 * 	Lotsa new return types, mostly void's. Update generally.
 * 	[90/04/09  16:32:57  tmt]
 * 
 * Revision 1.2  90/03/27  13:22:47  gm
 * 	Changed name to proto_inet.h for 14-char filename.
 * 	[90/03/16  10:49:00  tmt]
 * 
 * 	Look at _NO_PROTO for function prototype select.
 * 	[90/03/09  12:59:42  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:42  robert
 * 	Add argument to tcp_pulloutofband().
 * 	[90/01/10  10:37:44  tmt]
 * 
 * 	in_pcb.c disconnect, detach routines no longer need global lock pointer.
 * 	Update ip, tcp reassembly parameters and returns.
 * 	[90/01/08  16:09:12  tmt]
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
 * Revision 1.3  90/01/02  20:12:16  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:13:35  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
#if	(defined(__STDC__) || defined(__GNUC__)) && !defined(_NO_PROTO)
# define	P(s) s
/* Avoid scoping problems */
struct arpcom; struct in_addr; struct arptab;
struct mbuf; struct ifnet;
struct sockaddr; struct sockaddr_in; struct socket;
struct in_ifaddr; struct inpcb; struct ipq;
struct ipasfrag; struct route; struct ip;
struct tcpcb; struct tcpiphdr;
/* The tcp_seq typedef is a problem. Better to cast or rearrange? */
#define proto_tcp_seq	u_long
#else
# define P(s) ()
#endif


/* if_ether.c */
int	arptimer P((void));
void	arpinit P((void));
void	arpintr P((void));
void	arpinput P((struct arpcom *, struct mbuf *));
void	in_arpinput P((struct arpcom *, struct mbuf *));
void	arpoutput P((struct arpcom *, struct mbuf *, u_char *, u_short));
void	arptfree P((struct arptab *));
struct	arptab *arptnew P((struct in_addr *, struct ifnet *));
/* arpwhohas, arpioctl are in net/if_ethersubr.c */

/* in.c */
struct	in_addr in_makeaddr P((u_long, u_long));
u_long	in_netof P((struct in_addr));
void	in_sockmaskof P((struct in_addr, struct sockaddr_in *));
u_long	in_lnaof P((struct in_addr));
int	in_localaddr P((struct in_addr));
int	in_canforward P((struct in_addr));
int	in_control P((struct socket *, int, caddr_t, struct ifnet *));
void	in_ifscrub P((struct ifnet *, struct in_ifaddr *));
int	in_ifinit P((struct ifnet *, struct in_ifaddr *, struct sockaddr_in *,
				int));
struct	in_ifaddr *in_iaonnetof P((u_long));
int	in_broadcast P((struct in_addr));

/* in_cksum.c or machine dependent */
int	in_cksum P((struct mbuf *, int));

/* in_pcb.c */
int	in_pcballoc P((struct socket *, struct inpcb *));
int	in_pcbbind P((struct inpcb *, struct mbuf *));
int	in_pcbconnect P((struct inpcb *, struct mbuf *));
void	in_pcbdisconnect P((struct inpcb *));
void	in_pcbdetach P((struct inpcb *));
void	in_pcbfree P((struct inpcb *));
void	in_setsockaddr P((struct inpcb *, struct mbuf *));
void	in_setpeeraddr P((struct inpcb *, struct mbuf *));
void	in_pcbnotify P((struct inpcb *, struct sockaddr *, u_short,
				struct in_addr, u_short, int, void (*)()));
void	in_losing P((struct inpcb *));
void	in_rtchange P((struct inpcb *));
struct	inpcb *in_pcblookup P((struct inpcb *, struct in_addr,
				u_short, struct in_addr, u_short, int));
struct	inpcb *in_pcbmatch P((struct inpcb *, struct in_addr,
				u_short, struct in_addr, u_short));

/* in_proto.c */
int	inet_config ();		/* No prototypes here */

/* ip_icmp.c */
void	icmp_error P((struct mbuf *, int, int, struct in_addr));
void	icmp_input P((struct mbuf *, int));
void	icmp_reflect P((struct mbuf *));
struct	in_ifaddr *ifptoia P((struct ifnet *));
void	icmp_send P((struct mbuf *, struct mbuf *));
n_time	iptime P((void));

/* ip_input.c */
void	ip_init P((void));
void	ipintr P((void));
struct	mbuf *ip_reass P((struct mbuf *, struct ipq *));
void	ip_freef P((struct ipq *));
void	ip_enq P((struct ipasfrag *, struct ipasfrag *));
void	ip_deq P((struct ipasfrag *));
void	ip_slowtimo P((void));
void	ip_drain P((void));
int	ip_dooptions P((struct mbuf *));
struct	in_ifaddr *ip_rtaddr P((struct in_addr));
void	save_rte P((u_char *, struct in_addr));
struct	mbuf *ip_srcroute P((void));
void	ip_stripoptions P((struct mbuf *, struct mbuf *));
void	ip_forward P((struct mbuf *, int));
void	mach_net_ipinit P((void));
void	mach_net_ipdone P((void));
int	mach_net_ipsend P((caddr_t, int));
struct	mbuf *mach_net_ipreceive P((struct mbuf *, int));

/* ip_output.c */
int	ip_output P((struct mbuf *, struct mbuf *, struct route *, int));
struct	mbuf *ip_insertoptions P((struct mbuf *, struct mbuf *, int *));
int	ip_optcopy P((struct ip *, struct ip *));
int	ip_ctloutput P((int, struct socket *, int, int, struct mbuf **));
int	ip_pcbopts P((struct mbuf **, struct mbuf *));

/* raw_ip.c */
void	rip_input P((struct mbuf *));
int	rip_output P((struct mbuf *, struct socket *));
int	rip_ctloutput P((int, struct socket *, int, int, struct mbuf **));
int	rip_usrreq P((struct socket *, int, struct mbuf *, struct mbuf *,
				struct mbuf *));

/* tcp_debug.c */
void	tcp_trace P((int, int, struct tcpcb *, struct tcpiphdr *, int));

/* tcp_input.c */
int	tcp_reass P((struct tcpcb *, struct tcpiphdr *, struct mbuf *));
void	tcp_input P((struct mbuf *, int));
#ifdef RFC_1323
void	tcp_dooptions P((struct tcpcb *, struct mbuf *, struct tcpiphdr *,
						int *, u_long *, ulong *));
#else
void	tcp_dooptions P((struct tcpcb *, struct mbuf *, struct tcpiphdr *));
#endif
void	tcp_pulloutofband P((struct socket *, struct tcpiphdr *,
				struct mbuf *));
#ifdef RFC_1323
void	tcp_xmit_timer P((struct tcpcb *, short));
#else
void	tcp_xmit_timer P((struct tcpcb *));
#endif

int	tcp_mss P((struct tcpcb *, u_short));

/* tcp_output.c */
int	tcp_output P((struct tcpcb *));
void	tcp_setpersist P((struct tcpcb *));

/* tcp_subr.c */
void	tcp_init P((void));
void	tcp_template P((struct tcpcb *));
void	tcp_respond P((struct tcpcb *, struct tcpiphdr *, struct mbuf *,
				proto_tcp_seq, proto_tcp_seq, int));
struct	tcpcb *tcp_newtcpcb P((struct inpcb *));
struct	tcpcb *tcp_drop P((struct tcpcb *, int));
struct	tcpcb *tcp_close P((struct tcpcb *));
void	tcp_drain P((void));
void	tcp_notify P((struct inpcb *, int));
void	tcp_ctlinput P((int, struct sockaddr *, struct ip *));
void	tcp_quench P((struct inpcb *));

/* tcp_timer.c */
void	tcp_fasttimo P((void));
void	tcp_slowtimo P((void));
void	tcp_canceltimers P((struct tcpcb *));
struct	tcpcb *tcp_timers P((struct tcpcb *, int));

/* tcp_usrreq.c */
int	tcp_usrreq P((struct socket *, int, struct mbuf *, struct mbuf *,
				struct mbuf *));
int	tcp_ctloutput P((int, struct socket *, int, int, struct mbuf **));
int	tcp_attach P((struct socket *));
struct	tcpcb *tcp_disconnect P((struct tcpcb *));
struct	tcpcb *tcp_usrclosed P((struct tcpcb *));

/* udp_usrreq.c */
void	udp_init P((void));
void	udp_input P((struct mbuf *, int));
struct	mbuf *udp_saveopt P((caddr_t, int, int));
void	udp_notify P((struct inpcb *, int));
void	udp_ctlinput P((int, struct sockaddr *, struct ip *));
int	udp_output P((struct inpcb *, struct mbuf *, struct mbuf *,
				struct mbuf *));
int	udp_usrreq P((struct socket *, int, struct mbuf *, struct mbuf *,
				struct mbuf *));

#undef P
