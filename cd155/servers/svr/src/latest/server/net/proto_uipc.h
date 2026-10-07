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
 * $Log: proto_uipc.h,v $
 * Revision 1.5  1994/11/18  20:34:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:08:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:28:41  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:58  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:27:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:44  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:21  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:54  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1993/03/25  14:42:13  durriya
 * 	change proto for domaininit - takes a boolean (durriya)
 *
 * Revision 2.3  92/07/29  08:24:12  rabii
 * 	Fix the prototype definitions (norman & pjg).
 * 
 * Revision 2.2  91/08/31  13:40:46  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:33:51  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16.4.2  91/03/15  17:48:13  tmt
 * 	Fix sofree() to be void.
 * 	Update prototypes to match code.
 * 	[91/03/13  19:07:08  tmt]
 * 
 * Revision 1.16  90/10/07  14:32:36  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:11:22  gm]
 * 
 * Revision 1.15  90/09/23  15:55:36  devrcs
 * 	Remove uipc_funnel.
 * 	[90/09/15  15:35:19  tmt]
 * 
 * 	Fix sosleep. Put _NO_PROTO back.
 * 	[90/09/05  17:30:16  tmt]
 * 
 * Revision 1.14  90/09/13  11:48:04  devrcs
 * 	Add sockaddr_{new,old} format converters.
 * 	[90/08/24  12:47:06  tmt]
 * 
 * Revision 1.13  90/08/24  12:14:00  devrcs
 * 	Changes for new syscall interface (and COMPAT_43).
 * 	[90/08/19  19:20:15  tmt]
 * 
 * Revision 1.12  90/07/27  08:59:31  devrcs
 * 	Checkpoint at Reno merge.
 * 	Add and update several routines.
 * 	Look at GNUC as well as STDC so -traditional does prototypes.
 * 	[90/07/19  16:47:48  tmt]
 * 
 * Revision 1.11  90/07/05  23:12:47  devrcs
 * 	Add placeholders for function prototypes to work. Add uipc_funnel.
 * 	[90/07/03  18:48:27  tmt]
 * 
 * Revision 1.10  90/06/22  20:38:41  devrcs
 * 	Fix new types for sbwakeup/sbpoll.
 * 	[90/06/06  14:04:24  tmt]
 * 
 * Revision 1.9  90/05/13  18:44:50  devrcs
 * 	Add the XTI glue routines.
 * 	[90/05/04  14:18:57  tmt]
 * 
 * Revision 1.8  90/04/27  19:13:19  devrcs
 * 	Update sleeps, add new routines, remove all #ifdef's.
 * 	[90/04/20  12:31:45  tmt]
 * 
 * Revision 1.7  90/04/14  00:32:32  devrcs
 * 	Lotsa new return types, mostly void's. Update generally.
 * 	[90/04/09  16:17:52  tmt]
 * 
 * Revision 1.6  90/03/27  13:21:25  gm
 * 	Fix declarations in uipc_mbuf.c
 * 	[90/03/19  16:23:18  tmt]
 * 
 * 	Look at _NO_PROTO for function prototype select.
 * 	Fix soo_select args, add mclget and m_{lead,trail}ingspace.
 * 	[90/03/09  12:59:18  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:05  robert
 * 	Add solockpair(), change name of sounlock().
 * 	[90/01/19  14:41:58  tmt]
 * 
 * Revision 1.4  90/01/18  08:44:04  gm
 * 	Add sopriv. Fix wrapped lines.
 * 	[90/01/08  15:54:30  tmt]
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
 * Revision 1.3  90/01/03  12:40:54  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:04  gm]
 * 
 * Revision 1.2  89/12/26  09:46:30  gm
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
#if	(defined(__STDC__) || defined(__GNUC__)) && !defined(_NO_PROTO)
# define	P(s) s
/* Avoid scoping problems */
struct file; struct uio; struct ucred; struct stat; struct proc;
struct socket; struct sockbuf; struct mbuf;
struct msghdr; struct unpcb;
struct domain;
/* mblk_t */
#else
# define P(s) ()
#endif


/* sys_socket.c */
int	soo_read P((struct file *, struct uio *, struct ucred *));
int	soo_write P((struct file *, struct uio *, struct ucred *));
int	soo_ioctl P((struct file *, int, caddr_t));
#if	MACH	/* poll interface */
int	soo_select P((struct file *, short *, short *, int));
#else		/* select interface */
int	soo_select P((struct file *, int));
#endif
int	soo_stat P((struct socket *, struct stat *));
int	soo_close P((struct file *));

/* uipc_domain.c */
void	domaininit P((boolean_t));
int	domain_add P((struct domain *));
int	domain_del P((struct domain *));
struct	protosw *pffindtype P((int, int));
struct	protosw *pffindproto P((int, int, int));
void	pfctlinput P((int, struct sockaddr *));
void	pfreclaim P((void));
int	pfslowtimo P((void));
int	pffasttimo P((void));

/* uipc_mbuf.c */
void	mbinit P((void));
int	m_clalloc P((int, int));
void	mbufintr P((void));
int	m_expand P((int));
void	mclget P((struct mbuf *, int));
struct	mbuf *m_retry P((int, int));
struct	mbuf *m_retryhdr P((int, int));
struct	mbuf *m_get P((int, int));
struct	mbuf *m_gethdr P((int, int));
struct	mbuf *m_getclr P((int, int));
struct	mbuf *m_free P((struct mbuf *));
void	m_freem P((struct mbuf *));
int	m_leadingspace P((struct mbuf *));
int	m_trailingspace P((struct mbuf *));
struct	mbuf *m_prepend P((struct mbuf *, int, int));
struct	mbuf *m_copym P((struct mbuf *, int, int, int));
void	m_copydata P((struct mbuf *, int, int, caddr_t));
void	m_cat P((struct mbuf *, struct mbuf *));
void	m_adj P((struct mbuf *, int));
struct	mbuf *m_pullup P((struct mbuf *, int));
/*
mblk_t	*mbuf_to_mblk P((struct mbuf *, int));
struct	mbuf *mblk_to_mbuf P((mblk_t *, int));
*/

/* uipc_proto.c */
int	uipc_config P((void));

/* uipc_socket.c */
int	socreate P((int, struct socket **, int, int));
int	sobind P((struct socket *, struct mbuf *));
int	solisten P((struct socket *, int));
void	sofree P((struct socket *));
void	solockpair P((struct socket *, struct socket *));
int	sounlock P((struct socket *));
int	soclose P((struct socket *));
int	soabort P((struct socket *));
int	soaccept P((struct socket *, struct mbuf *));
int	soconnect P((struct socket *, struct mbuf *));
int	soconnect2 P((struct socket *, struct socket *));
int	sodisconnect P((struct socket *));
int	sosend P((struct socket *, struct mbuf *, struct uio *,
				struct mbuf *, struct mbuf *, int));
int	soreceive P((struct socket *, struct mbuf **, struct uio *,
				struct mbuf **, struct mbuf **, int *));
int	soshutdown P((struct socket *, int));
void	sorflush P((struct socket *));
void	sopriv P((struct socket *));
int	sosetopt P((struct socket *, int, int, struct mbuf *));
int	sogetopt P((struct socket *, int, int, struct mbuf **));
void	sohasoutofband P((struct socket *));
int	sodequeue P((struct socket *, struct socket **, struct mbuf **, int));
int	sogetaddr P((struct socket *, struct mbuf **, int, int));

/* uipc_socket2.c */
void	soisconnecting P((struct socket *));
void	soisconnected P((struct socket *));
void	soisdisconnecting P((struct socket *));
void	soisdisconnected P((struct socket *));
struct	socket *sonewsock P((struct socket *, int));
void	soqinsque P((struct socket *, struct socket *, int));
int	soqremque P((struct socket *, int));
void	socantsendmore P((struct socket *));
void	socantrcvmore P((struct socket *));
void	sbselqueue P((struct sockbuf *));
void	sbseldequeue P((struct sockbuf *));
int	sosblock P((struct sockbuf *, struct socket *));
void	sbunlock P((struct sockbuf *));
int	sosbwait P((struct sockbuf *, struct socket *));
int	sosleep P((struct socket *, caddr_t, int, int));
void	sowakeup P((struct socket *, struct sockbuf *));
int	sbwakeup P((struct socket *, struct sockbuf *, int));
int	sbpoll P((struct socket *, struct sockbuf *));
int	soreserve P((struct socket *, u_long, u_long));
int	sbreserve P((struct sockbuf *, u_long));
void	sbrelease P((struct sockbuf *));
void	sbappend P((struct sockbuf *, struct mbuf *));
void	sbappendrecord P((struct sockbuf *, struct mbuf *));
void	sbinsertoob P((struct sockbuf *, struct mbuf *));
int	sbappendaddr P((struct sockbuf *, struct sockaddr *,
				struct mbuf *, struct mbuf *));
int	sbappendcontrol P((struct sockbuf *, struct mbuf *, struct mbuf *));
void	sbcompress P((struct sockbuf *, struct mbuf *, struct mbuf *));
void	sbflush P((struct sockbuf *));
void	sbdrop P((struct sockbuf *, int));
void	sbdroprecord P((struct sockbuf *));
int	lock_socheck P((struct socket *));
int	unlock_socheck P((struct socket *));
int	lock_sbcheck P((struct sockbuf *));
int	unlock_sbcheck P((struct sockbuf *));

/* uipc_syscalls.c */
int	socket P((struct proc *, void *, int *));
int	bind P((struct file *, void *, int *));
int	listen P((struct file *, void *, int *));
int	accept P((struct file *, void *, int *));
int	oaccept P((struct file *, void *, int *));
int	connect P((struct file *, void *, int *));
int	socketpair P((struct proc *, void *, int *));
int	sendto P((struct file *, void *, int *));
int	osend P((struct file *, void *, int *));
int	osendmsg P((struct file *, void *, int *));
int	sendmsg P((struct file *, void *, int *));
int	recvfrom P((struct file *, void *, int *));
int	orecvfrom P((struct file *, void *, int *));
int	orecv P((struct file *, void *, int *));
int	orecvmsg P((struct file *, void *, int *));
int	recvmsg P((struct file *, void *, int *));
int	shutdown P((struct file *, void *, int *));
int	setsockopt P((struct file *, void *, int *));
int	getsockopt P((struct file *, void *, int *));
int	pipe P((struct proc *, void *, int *));
int	getsockname P((struct file *, void *, int *));
int	ogetsockname P((struct file *, void *, int *));
int	getpeername P((struct file *, void *, int *));
int	ogetpeername P((struct file *, void *, int *));
int	sendit P((struct file *, struct msghdr *, int, int *));
int	recvit P((struct file *, struct msghdr *, caddr_t, int *));
#ifdef	COMPAT_43
int	accept1 P((struct file *, void *, int *, int));
int	recvfrom1 P((int));
int	getsockname1 P((struct file *, void *, int *, int));
int	getpeername1 P((struct file *, void *, int *, int));
#endif
void	sockaddr_new P((struct mbuf *));
void	sockaddr_old P((struct mbuf *));
int	sockargs P((struct mbuf **, caddr_t, int, int));
struct	file *getsock P((struct file *, int *));

/* uipc_usrreq.c */
void	uipc_init P((void));
int	uipc_usrreq P((struct socket *, int, struct mbuf *, struct mbuf *,
				struct mbuf *));
int	unp_attach P((struct socket *));
void	unp_detach P((struct unpcb *));
int	unp_bind P((struct unpcb *, struct mbuf *));
int	unp_connect P((struct socket *, struct mbuf *));
int	unp_connect2 P((struct socket *, struct socket *));
void	unp_disconnect P((struct unpcb *));
void	unp_abort P((struct unpcb *));
void	unp_usrclosed P((struct unpcb *));
void	unp_drop P((struct unpcb *, int));
void	unp_drain P((void));
int	unp_externalize P((struct mbuf *));
int	unp_internalize P((struct mbuf *));
void	unp_gc P((void));
void	unp_dispose P((struct mbuf *));
void	unp_scan P((struct mbuf *, void (*)()));
void	unp_mark P((struct file *));
void	unp_discard P((struct file *));

#undef P
