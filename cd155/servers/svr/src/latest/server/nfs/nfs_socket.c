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
 * $Log: nfs_socket.c,v $
 * Revision 1.12  1995/02/01  21:35:36  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/11/18  20:37:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/03/31  18:24:21  dbm
 * Merge of R1.2 version 1.8.4.2
 *
 * Revision 1.9  1994/01/14  23:53:49  yazz
 *  Reviewer:jlitvin
 *  Risk:Medium
 *  Benefit or PTS #:4807
 *  Testing:Error cases filed with bug report, other stress tests
 *  Module(s):./server/nfs/nfs_socket.c
 *
 *
 * In several cases, the function nfs_timer() was incrementing the
 * retransmit count for an nfs request even if the request was not
 * retransmitted. In addition, a timer in the nfs request was
 * always incremented even if the state flags in the request had
 * disabled timing.
 *
 * Revision 1.8.4.2  1994/03/31  18:22:20  dbm
 * Fixed a bug in the nfs_timout() function that was causing requests to
 * get lost.
 *  Reviewer: nina, bernie.
 *  Risk:M
 *  Benefit or PTS #:8492,7992,8554
 *  Testing: Specific test cases.
 *  Module(s):
 * 	nfs_socket.c
 *
 * Revision 1.8.4.1  1994/01/11  23:13:33  nina
 *  Reviewer:jlitvin
 *  Risk:Medium
 *  Benefit or PTS #:4807
 *  Testing:Error cases filed with bug report, other stress tests
 *  Module(s):./server/nfs/nfs_socket.c
 *
 * Revision 1.8  1993/08/11  15:47:52  rkl
 * Made NFS client requests use privileged UDP sockets for compatibility with
 * other NFS implementations.  This fix came from Durriya at OSF.
 *
 * Revision 1.7  1993/07/14  18:16:16  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/09  15:05:01  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.4  1993/07/01  19:38:13  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  20:29:26  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:35:42  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:06:46  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:00:33  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  02:58:17  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:32:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:27:42  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:30:15  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.13  93/07/07  10:34:33  mjl
 * [LCC #0314] Use NET_THREADSTART() macro for deferred startup of net threads.
 * 
 * Revision 2.12  92/10/27  17:38:32  bhk
 * Virtualized calls to the socket layer. (socreate, soclose, sobind,
 * soconnect, sogetaddr ...)
 * 
 * Revision 2.11  92/06/05  13:58:18  klh
 * 	Revision 2.9  92/05/24  14:36:39  pjg
 * 		92/03/31  15:40:53  emcmanus
 * 		Name the nfs_timer thread (if used).
 * 		[92/05/19            srl]
 * 
 * Revision 2.10  92/04/14  10:23:58  roman
 * Remove conditional dependency on TNC not defined. Add missing extra parameter
 * 	to VPOP_CTTY_GETATTR().
 * 
 * Revision 2.9  92/04/06  19:08:18  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.7  92/04/05  16:57:00  pjg
 * 	Added support for remote tprintf handling (rabii)
 * 
 * Revision 2.6  92/03/09  12:14:52  durriya
 * 	Revision 3.3  91/12/18  17:17:28  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.5  91/11/22  15:01:42  rabii
 * 	Locus Merge
 * 	VPOP_CTTY_GETATTR supersedes VPOP_GET_CTTY. (chrisp)
 * 
 * Revision 2.4  91/10/04  14:56:36  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  16:41:28  rabii
 * 	Merge of V2.0 and Locus (locus check-in by roman)
 * 	Minor change accessing the controlling tty due to vprocs being 
 * 	added to system.
 * 
 * Revision 2.2  91/08/31  13:51:40  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  17:29:11  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.4.4  91/07/30  13:11:12  tmt
 * 	Don't increment send count when not retransmitting due to congestion.
 * 	[91/07/29  08:55:12  tmt]
 * 
 * Revision 1.13.4.2  91/06/20  14:58:27  tmt
 * 	Keep RPC error (EINTR, ETIMEDOUT) in request, not socket, to
 * 	correctly abort the affected RPC. Clip min timeout in backoff,
 * 	don't retransmit on congestion until ready. Report errors to
 * 	user more sensibly.
 * 	[91/06/20  14:32:20  tmt]
 * 
 * Revision 1.13  90/10/07  14:39:12  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:21:37  gm]
 * 
 * 	Changed the group management to separate cr_gid from the cr_groups
 * 	 array.
 * 	[90/09/21  11:08:04  collins]
 * 
 * Revision 1.12  90/08/24  12:15:45  devrcs
 * 	Prevent null dereference in nfs_disconnect.
 * 	Remove unnecessary socket lock.
 * 	[90/08/19  16:03:15  tmt]
 * 
 * Revision 1.11  90/07/05  23:13:42  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	Merge SPL's and NFSREQ_LOCK macros. Remove nfsnoconnect.
 * 	Rearrange user "not responding" printf to avoid unnecessary noise.
 * 	Use sogetaddr for server match.
 * 	[90/07/03  18:58:10  tmt]
 * 
 * Revision 1.10  90/06/22  20:40:05  devrcs
 * 	Move ++ side effects out of fxdr macro - for inline.
 * 	Remove unix_master - unnecessary.
 * 	[90/06/14  11:55:21  tmt]
 * 
 * 	nags merge
 * 	[90/06/12  21:35:53  gmf]
 * 
 * Revision 1.9  90/05/13  18:45:25  devrcs
 * 	Rearrange socket lock/unlock to avoid deadlock with nfsreq
 * 	lock. Simplify sockbuf dequeue code in process.
 * 	[90/05/05  18:05:41  tmt]
 * 
 * 	Fix sbunlock after failed sosblock or sosbwait.
 * 	Do better RPC length checking.
 * 	[90/05/04  14:39:18  tmt]
 * 
 * 	Do parallel locking on request list. Use thread for nfs_timer.
 * 	Add init entry for above. Cleanup. Use <> on includes.
 * 	[90/04/30  10:23:04  tmt]
 * 
 * Revision 1.8  90/04/27  19:20:28  devrcs
 * 	Modify calls of sosleep, sosbwait, sosblock for new flags, behavior.
 * 	Do outstanding request list better.
 * 	[90/04/20  16:35:30  tmt]
 * 
 * Revision 1.7  90/04/14  00:33:52  devrcs
 * 	Refine timer routine, interruptibility and printf's. Remove
 * 	old networking compat code. #ifdef UNIX domain socket support.
 * 	[90/04/10  11:36:40  tmt]
 * 
 * Revision 1.6  90/03/27  13:24:43  gm
 * 	Check signals against u_sigintr when interruptible (tmt)
 * 	[90/03/19  14:14:04  pam]
 * 
 * 	Changes from George at Encore
 * 	[90/03/15  17:17:13  pam]
 * 
 * Revision 1.6  90/03/08  12:51:05  gmf
 * 	nm_host to nmp->nm_mountp->....
 * 
 * Revision cthon  90/02/17  17:34:10  tmt
 * 	Fix r_timerinit bug.
 * 	nfs_noconnect flag for Encore compat (security hole!). rearrange
 * 		uprintf.
 * 	Add idempotency test on request timeouts.
 * 	Tune RTO algorithm (more conservative). Do #ifdef's for compat
 * 		differently, better. No flush of request list on unmount
 * 		(buggy, and shouldn't happen).
 * 	Cleanup and such.
 * 
 * Revision 1.5  90/02/05  15:51:18  robert
 * 	Fix bug in m_getclr args. Do user printf on retries.
 * 	[90/01/25  14:44:48  tmt]
 * 
 * 	Refine code for UNIX-domain client bind. Add macros for request list
 * 	locks, to be done later. Fix bugs in wakeups on errors, also allow
 * 	requests to interrupt if NFSMNT_INT. Consolidate code.
 * 	[90/01/19  15:24:52  tmt]
 * 
 * Revision 1.4  90/01/18  08:48:37  gm
 * 	Added NETSYNC_LOCK around in_losing_lock.
 * 	[90/01/16  20:06:40  tmt]
 * 
 * 	Fix some misc. sockaddr problems, do in_losing on retransmit timeout.
 * 	[90/01/08  15:00:28  tmt]
 * 
 * 	Remove and/or rearrange req, mntp, and hostinfo entries. Move socket
 * 	handling code to nfs_socket. Improve transport independence.
 * 	[90/01/02  14:40:03  tmt]
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
 * Revision 1.3  90/01/02  20:22:32  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:19:36  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base: nfs_socket.c	7.4 (Berkeley) 11/3/89
 */

/*
 * Socket operations for use by nfs (similar to uipc_socket.c, but never
 * with copies to/from a uio vector)
 * NB: For now, they only work for datagram sockets.
 * (Use on stream sockets would require some record boundary mark in the
 *  stream as defined by "RPC: Remote Procedure Call Protocol
 *  Specification" RFC1057 Section 10)
 *  and different versions of send, receive and reply that do not assume
 *  an atomic protocol
 */

#include <uxkern/bsd_types_gen.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <sys/lock_types.h>
#if	MACH
#include <kern/zalloc.h>
#else
#include "sys/malloc.h"
#endif
#include <netinet/in.h>

#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#include <sys/mbuf.h>
#ifdef TNC
#include <vsocket/vsocket.h>
#else
#include <sys/so_defs.h>
#endif
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/domain.h>
#include <sys/protosw.h>

#include <nfs/rpcv2.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <nfs/xdr_subs.h>
#include <nfs/nfsm_subs.h>
#include <nfs/nfsmount.h>

#include <sys/syslog.h>
#define nfs_log(message, host)	log(LOG_ERR, message, host)

#if	NETSYNC_LOCK
LOCK_ASSERTL_DECL
lock_data_t	nfsreq_lock;
#define NFSREQ_LOCKINIT()	lock_init2(&nfsreq_lock, TRUE, LTYPE_NFSREQ)
#define NFSREQ_LOCK_DECL()	NETSPL_DECL(ns)
#define NFSREQ_LOCK()		{ NETSPL(ns,net); lock_write(&nfsreq_lock); }
#define NFSREQ_UNLOCK()		{ lock_done(&nfsreq_lock); NETSPLX(ns); }
#else
#define NFSREQ_LOCKINIT()
#define NFSREQ_LOCK_DECL()	NETSPL_DECL(ns)
#define NFSREQ_LOCK()		NETSPL(ns,net)
#define NFSREQ_UNLOCK()		NETSPLX(ns)
#endif

/*
 * External data, mostly RPC constants in XDR form
 */
extern	mach_port_t	root_fs_port;
extern	node_t		this_node;
extern	node_t		root_fs_node;

extern u_long rpc_reply, rpc_msgdenied, rpc_mismatch, rpc_vers, rpc_auth_unix,
	rpc_msgaccepted, rpc_call;
extern u_long nfs_prog, nfs_vers;
int	nfsrv_null(),
	nfsrv_getattr(),
	nfsrv_setattr(),
	nfsrv_lookup(),
	nfsrv_readlink(),
	nfsrv_read(),
	nfsrv_write(),
	nfsrv_create(),
	nfsrv_remove(),
	nfsrv_rename(),
	nfsrv_link(),
	nfsrv_symlink(),
	nfsrv_mkdir(),
	nfsrv_rmdir(),
	nfsrv_readdir(),
	nfsrv_statfs(),
	nfsrv_noop();

int (*nfsrv_procs[NFS_NPROCS])() = {
	nfsrv_null,
	nfsrv_getattr,
	nfsrv_setattr,
	nfsrv_noop,
	nfsrv_lookup,
	nfsrv_readlink,
	nfsrv_read,
	nfsrv_noop,
	nfsrv_write,
	nfsrv_create,
	nfsrv_remove,
	nfsrv_rename,
	nfsrv_link,
	nfsrv_symlink,
	nfsrv_mkdir,
	nfsrv_rmdir,
	nfsrv_readdir,
	nfsrv_statfs
};

struct	nfshost *nfshosth;
struct	nfsreq *nfsreqh;
int	nfsrexmtthresh = NFS_FISHY;

void
nfs_socketinit()
{
	nfshosth = NULL;
	nfsreqh = NULL;
	NFSREQ_LOCKINIT();
}

/*
 * Initialize sockets and per-host congestion for a new NFS connection.
 */
nfs_connect(nmp, saddr)
	register struct nfsmount *nmp;
	struct mbuf *saddr;
{
	int error, srvaddrlen;
	struct mbuf *m;
	register struct nfshost *nfshp;
	NFSREQ_LOCK_DECL()
	static char initialized;

	nmp->nm_so = 0;
	if (error = SOCREATE(mtod(saddr, struct sockaddr *)->sa_family,
				&nmp->nm_so, SOCK_DGRAM, 0))
		goto bad;
	/* These sleepers get woken up in a different way (nfs_timer) */
	nmp->nm_so->so_rcv.sb_flags |= SB_NOINTR;
	nmp->nm_so->so_snd.sb_flags |= SB_NOINTR;

	switch(mtod(saddr, struct sockaddr *)->sa_family) {
#ifdef	UNIX_DOMAIN
		case AF_UNIX :
	/* Unix sockets do not provide a local bind for server reply */
		struct sockaddr *sa;
		static char client[] = "/tmp/.nfs/nfsclient##";
		static int serial;
		int firstserial;
		m = m_getclr(M_WAIT, MT_SONAME);
		if (m == NULL) {
			error = ENOBUFS;
			goto bad;
		}
		m->m_len = sizeof (client) + 2;
		sa = mtod(m, struct sockaddr *);
		sa->sa_family = AF_UNIX;
		sa->sa_len = m->m_len;
		bcopy(client, sa->sa_data, sizeof(client));
		firstserial = serial;
		do {
			if (++serial >= 100) serial = 0;
			sa->sa_data[19] = (serial / 10) + '0';
			sa->sa_data[20] = (serial % 10) + '0';
			error = VSOP_BIND(nmp->nm_so, m);
			if (firstserial == serial) break;
		} while (error == EADDRINUSE);
		m_freem(m);
		if (error)
			goto bad;
		break;
#endif
		case AF_INET :
		if (nmp->nm_so->so_state & SS_PRIV) {
			struct sockaddr_in *sin;
			short port = 800;
			/*
			 * The standard "reference" NFS code binds to a
			 * privileged port when mounted by root. There
			 * are configurations which insist on this, so
			 * (sigh) we'll do it too. Note this may use
			 * otherwise well-known ports, and limits the
			 * maximum number of nfsmounts.
			 */
			m = m_getclr(M_WAIT, MT_SONAME);
			if (m == NULL) {
				error = ENOBUFS;
				goto bad;
			}
			m->m_len = sizeof *sin;
			sin = mtod(m, struct sockaddr_in *);
			sin->sin_len = sizeof *sin;
			sin->sin_family = AF_INET;
			sin->sin_addr.s_addr = INADDR_ANY;
			do {
				sin->sin_port = htons(port);
				if ((error = VSOP_BIND(nmp->nm_so, m)) == 0 ||
				++port >= IPPORT_RESERVED)
					break;
			} while (error == EADDRINUSE);
			(void) m_free(m);
			if (error == 0)
				break;
			if (error != EADDRINUSE)
				goto bad;
		}
		/*
		 * if we are not PRIV or could not bind to a port < 1024 ,
		 * we fall through
		 */
		default :
		if (nmp->nm_flag & NFSMNT_NOCONN) {
			if (error = VSOP_BIND(nmp->nm_so, (struct mbuf *)0))
				goto bad;
		}
		break;
	}

	/*
	 * NFSMNT_NOCONN opens a security hole where random RPC replies
	 * from the server may be accepted. Normally, we would expect the
	 * port we connect to be the responder, but there exist exceptions.
	 */
	if (!(nmp->nm_flag & NFSMNT_NOCONN))
		if (error = VSOP_CONNECT(nmp->nm_so, saddr))
			goto bad;
	if (error = VSOP_RESERVE(nmp->nm_so,	/* get space ! */
				 nmp->nm_wsize + 1024,		/* one out */
				(nmp->nm_rsize + 1024) * 4))	/* four in */
		goto bad;

	/*
	 * Search mount list for existing server entry.
	 *
	 * Note, even though we have a sockaddr, it is not quite reliable
	 * enough to bcmp against. For instance, a sockaddr_in has a 
	 * sin_zero field which is not reliably zeroed by user code (e.g.
	 * mount). So what we do as an attempt at transport independence
	 * is to get the peeraddr of our connected socket into a zeroed
	 * sockaddr. Then we cache that and compare against it. This is
	 * not exactly perfect. However it is not critical that it be, if
	 * we cannot match the sockaddr we will simply allocate a new nfshp
	 * per mount, which will disable the per-host congestion but
	 * everything else will work as normal.
	 */
	if (!(nmp->nm_flag & NFSMNT_NOCONN) &&
	    VSOP_GETADDR(nmp->nm_so, &m, 1, 0) == 0) {
		m_freem(saddr);
		saddr = m;
	}
	srvaddrlen = saddr->m_len;

	NFSREQ_LOCK();

	for (nfshp = nfshosth; nfshp; nfshp = nfshp->nh_next) {
		if (srvaddrlen != nfshp->nh_salen)
			continue;
		if (!bcmp(mtod(saddr,caddr_t),mtod(nfshp->nh_sockaddr,caddr_t),
				srvaddrlen))
			break;
	}
	if (nfshp)		/* Have an existing mount host */
		m_freem(saddr);
	else {
#if	MACH
		nfshp = (struct nfshost *)zalloc(nfsmount_zone);
#else
		MALLOC(nfshp,struct nfshost *,sizeof *nfshp,M_NFSMNT,M_WAITOK);
#endif
		bzero((caddr_t)nfshp, sizeof *nfshp);
		nfshp->nh_sockaddr = saddr;
		nfshp->nh_salen = srvaddrlen;
		/* Initialize other non-zero congestion variables */
		nfshp->nh_currto = NFS_TIMEO;
		nfshp->nh_window = 1;		    /* Initial send window */
		nfshp->nh_ssthresh = NFS_MAXWINDOW; /* Slowstart threshold */
		if (nfshosth) nfshosth->nh_prev = nfshp;	/* Chain in */
		nfshp->nh_next = nfshosth;
		nfshosth = nfshp;
	}
	nfshp->nh_refcnt++;

	/* This is ugly but NFS is initialized before the networking... */
	if (!initialized) {
		int nfs_timer();
		initialized = 1;
		NFSREQ_UNLOCK();
#if	!NETISR_THREAD
		nfs_timer();
#else
		NET_THREADSTART(nfs_timer, 0, "nfs_timer");
#endif
	} else
		NFSREQ_UNLOCK();
	nmp->nm_hostinfo = nfshp;
	nmp->nm_srvaddr = (nmp->nm_flag & NFSMNT_NOCONN) ?
							nfshp->nh_sockaddr : 0;
	if (nmp->nm_rto == NFS_TIMEO) {
		nmp->nm_rto = nfshp->nh_currto;
		nmp->nm_rttvar = nmp->nm_rto << 1;
	}
	return 0;

bad:
	if (nmp->nm_so)
		(void) VSOP_CLOSE(nmp->nm_so);
	nmp->nm_so = 0;
	m_freem(saddr);
	return error;
}

/*
 * NFS disconnect. Clean up and unlink.
 */
void
nfs_disconnect(nmp)
	register struct nfsmount *nmp;
{
	register struct nfshost *nfshp;

	if (nmp->nm_so)
		(void) VSOP_CLOSE(nmp->nm_so);
	nmp->nm_so = 0;
	if (nfshp = nmp->nm_hostinfo) {
		NFSREQ_LOCK_DECL()
		NFSREQ_LOCK();
		if (--nfshp->nh_refcnt <= 0) {
			if (nfshp->nh_next)
				nfshp->nh_next->nh_prev = nfshp->nh_prev;
			if (nfshp->nh_prev)
				nfshp->nh_prev->nh_next = nfshp->nh_next;
			else
				nfshosth = nfshp->nh_next;
			if (nfshp->nh_sockaddr) {
#ifdef	UNIX_DOMAIN
				/* If unix family, remove nfsclient from /tmp */
				if (mtod(nfshp->nh_sockaddr,
				    struct sockaddr *)->sa_family == AF_UNIX) {
					/* Lookup sa_data, do VOP_REMOVE... */
				}
#endif
				m_freem(nfshp->nh_sockaddr);
				nfshp->nh_sockaddr = 0;
			}
#if	MACH
			ZFREE(nfsmount_zone, nfshp);
#else
			FREE(nfshp, M_NFSMNT);
#endif
		}
		nmp->nm_hostinfo = 0;
		NFSREQ_UNLOCK();
	}
}

/*
 * This is a stripped down non-interruptible version of sosend()
 * used by NFS clients.
 */
nfs_send(so, nam, top, flags, siz)
	register struct socket *so;
	struct mbuf *nam;
	struct mbuf *top;
	int flags;
	int siz;
{
	int error;
	DOMAIN_FUNNEL_DECL(f)

	top->m_pkthdr.len = siz;
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	for (;;) {
		if (error = sosblock(&so->so_snd, so))
			goto out;
		if (error = nfs_sockerr(so, 1))
			break;
		if (sbspace(&so->so_snd) >= siz) {
			error = (*so->so_proto->pr_usrreq)(so, PRU_SEND, top,
				nam, (struct mbuf *)0, (struct mbuf *)0);
			top = 0;
			break;
		}
		if (error = sosbwait(&so->so_snd, so))
			goto out;
	}
	sbunlock(&so->so_snd);
out:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	if (top) m_freem(top);
	return (error);
}

/*
 * This is a stripped down datagram specific version of soreceive()
 * used by NFS servers.
 */
/*ARGSUSED*/
nfs_dgreceive(so, msk, mtch, aname, mp)
	register struct socket *so;
	u_long msk;
	u_long mtch;
	struct mbuf **aname;
	struct mbuf **mp;
{
	register struct mbuf *m;
	int error = 0;
	struct mbuf *nextrecord;
	DOMAIN_FUNNEL_DECL(f)

	if (aname)
		*aname = 0;

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	for (;;) {
		if (error = sosblock(&so->so_rcv, so))
			goto out;
		if (so->so_rcv.sb_cc == 0) {
			if (error = nfs_sockerr(so, 0)) {
				so->so_error = 0;
				break;
			}
			if (error = sosbwait(&so->so_rcv, so))
				goto out;
			continue;
		}
		m = so->so_rcv.sb_mb;
		if (m == 0)
			panic("nfs_dgreceive 1");
		nextrecord = m->m_nextpkt;
		/* Save sender's address */
		if (m->m_type != MT_SONAME)
			panic("nfs_dgreceive 1a");
		sbfree(&so->so_rcv, m);
		if (aname) {
			*aname = m;
			so->so_rcv.sb_mb = m->m_next;
			m->m_next = 0;
		} else {
			MFREE(m, so->so_rcv.sb_mb);
		}
		m = so->so_rcv.sb_mb;
		/* Drop control mbuf's */
		while (m && m->m_type != MT_DATA && m->m_type != MT_HEADER) {
			if (m->m_type == MT_RIGHTS)
				panic("nfs_dgreceive 2");
			sbfree(&so->so_rcv, m);
			MFREE(m, so->so_rcv.sb_mb);
			m = so->so_rcv.sb_mb;
		}
		/* Dequeue packet from sockbuf */
		*mp = m;
		while (m) {
			if (m->m_type != MT_DATA && m->m_type != MT_HEADER)
				panic("nfs_dgreceive 3");
			sbfree(&so->so_rcv, m);
			m = so->so_rcv.sb_mb = m->m_next;
		}
		so->so_rcv.sb_mb = nextrecord;
		/* Return */
		break;
	}
	sbunlock(&so->so_rcv);
out:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

struct rpc_replyhead {
	u_long	r_xid;
	u_long	r_rep;
};

/*
 * Implement NFS client side datagram receive.
 * We depend on the way that records are added to the sockbuf
 * by sbappend*.  In particular, each record (mbufs linked through m_next)
 * must begin with an address, followed by optional MT_CONTROL mbuf
 * and then zero or more mbufs of data.
 * We must search through the list of received datagrams matching them
 * with outstanding requests using the xid, until ours is found.
 */
nfs_dgreply(so, mntp, myrep)
	register struct socket *so;
	struct nfsmount *mntp;
	struct nfsreq *myrep;
{
	register struct mbuf *m, *mp;
	register struct nfsreq *rep;
	register int error = 0;
	int logged = 0, first = 0;
	struct mbuf *nextrecord;
	struct rpc_replyhead replyh;
	NFSREQ_LOCK_DECL()
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
restart:
	if (myrep->r_mrep || myrep->r_error ||
	    (error = sosblock(&so->so_rcv, so))) {
		if (myrep->r_error) {
			error = myrep->r_error;
			if (error == ETIMEDOUT)
				nfsstats.rpctimeouts++;
		}
giveup:
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		NFSREQ_LOCK();
		if (myrep->r_flags & R_TIMING) {
			myrep->r_flags &= ~R_TIMING;
			mntp->nm_rtt = -1;
		}
		if (myrep->r_flags & R_SENT) {
			myrep->r_flags &= ~R_SENT;
			--mntp->nm_hostinfo->nh_sent;
			/* If count now 0, want to initiate new req */
		}
		NFSREQ_UNLOCK();
		goto release;
	}
	if (first++ && mntp->nm_rexmit >= nfsrexmtthresh && logged++ == 0)
		nfs_tprintf(myrep,
			"NFS server %s not responding, retrying\n",
			mntp->nm_mountp->m_stat.f_mntfromname);

	m = so->so_rcv.sb_mb;
	if (m == 0) {
		if (so->so_rcv.sb_cc)
			panic("nfs_soreply 1");
		if (error = nfs_sockerr(so, 0)) {
			so->so_error = 0;
			sbunlock(&so->so_rcv);
			goto giveup;
		}
		if (error = sosbwait(&so->so_rcv, so))
			goto giveup;
		goto restart;
	}

	/*
	 * Take off the address, check for rights and ditch any control
	 * mbufs.
	 */
	nextrecord = m->m_nextpkt;
	while (m && m->m_type != MT_DATA && m->m_type != MT_HEADER) {
		if (m->m_type == MT_RIGHTS)
			panic("nfs reply RIGHTS");
		sbfree(&so->so_rcv, m);
		MFREE(m, so->so_rcv.sb_mb);
		m = so->so_rcv.sb_mb;
	}
	if (m == NULL) {
		so->so_rcv.sb_mb = nextrecord;
		sbunlock(&so->so_rcv);
		goto restart;
	}
	m->m_nextpkt = nextrecord;

	/*
	 * Get the xid and check that it is an rpc reply
	 */
	mp = m;
	if (m->m_len >= sizeof replyh)
		bcopy(mtod(m, caddr_t), (caddr_t)&replyh, sizeof replyh);
	else {
		caddr_t cp = (caddr_t)&replyh;
		int cnt = sizeof replyh;
		do {
			if (mp->m_len > 0) {
				int xfer = (mp->m_len >= cnt) ? cnt : mp->m_len;
				bcopy(mtod(mp, caddr_t), cp, xfer);
				cnt -= xfer;
				cp += xfer;
			}
			if (cnt > 0)
				mp = mp->m_next;
		} while (mp && cnt > 0);
	}
	if (!mp || replyh.r_rep != rpc_reply) {	/* Too short or not a reply */
		nfsstats.rpcinvalid++;
		sbdroprecord(&so->so_rcv);
		sbunlock(&so->so_rcv);
		goto restart;
	}
	mp = m;
	while (m) {
		if (m->m_type != MT_DATA && m->m_type != MT_HEADER)
			panic("nfs_soreply 3");
		sbfree(&so->so_rcv, m);
		m = so->so_rcv.sb_mb = m->m_next;
	}
	so->so_rcv.sb_mb = nextrecord;
	sbunlock(&so->so_rcv);
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);

	/*
	 * Loop through the request list to match up the reply
	 * If no match, just drop the datagram
	 */
	NFSREQ_LOCK();
	if (rep = nfsreqh) do {
		/* The socket, being connected, will only queue matches */
		/* If NFSMNT_NOCONN, we'll just have to believe the XID */
		if (replyh.r_xid == rep->r_xid && so == rep->r_mntp->nm_so) {
			if (rep->r_mrep)	/* Already there - duplicate */
				break;
			rep->r_mrep = mp;
			if (rep->r_flags & R_TIMING) {
				nfs_updatetimer(mntp);
				rep->r_flags &= ~R_TIMING;
				mntp->nm_rtt = -1;	/* re-arm timer */
			}
			if (rep->r_flags & R_SENT) {
				rep->r_flags &= ~R_SENT;
				--mntp->nm_hostinfo->nh_sent;
				/* If count now 0, want to initiate new req */
			}
			NFSREQ_UNLOCK();
			if (rep == myrep)		/* This is success */
				goto release;
			/* Else wake up other sleeper and wait for next */
			DOMAIN_FUNNEL(sodomain(so), f);
			SOCKET_LOCK(so);
			sorwakeup(so);
			goto restart;
		}
		rep = rep->r_next;
	} while (rep != nfsreqh);
	NFSREQ_UNLOCK();
	/* If not matched to request, drop it */
	m_freem(mp);
	nfsstats.rpcunexpected++;
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	goto restart;

release:
	if (error == 0 && logged)
		nfs_tprintf(myrep, "NFS server %s responded\n", 
			mntp->nm_mountp->m_stat.f_mntfromname);
	return (error);
}

/*
 * nfs_request - goes something like this
 *	- fill in request struct
 *	- links it into list
 *	- calls nfs_sosend() for first transmit
 *	- calls nfs_soreceive() to get reply
 *	- break down rpc header and return with nfs reply pointed to
 *	  by mrep or error
 * nb: always frees up mreq mbuf list
 */
nfs_request(vp, mreq, xid, idem, mp, mrp, mdp, dposp)
	struct vnode *vp;
	struct mbuf *mreq;
	u_long xid;
	int idem;
	struct mount *mp;
	struct mbuf **mrp;
	struct mbuf **mdp;
	caddr_t *dposp;
{
	register struct mbuf *m, *mrep;
	register struct nfsreq *rep;
	register u_long *p;
	register int len;
	struct nfsmount *mntp;
	struct mbuf *md;
	caddr_t dpos;
	char *cp2;
	int t1;
	int error;
	NFSREQ_LOCK_DECL()

	mntp = vfs_to_nfs(mp);
	m = mreq;
#if	MACH
	ZALLOC(nfsreq_zone, rep, struct nfsreq *);
#else
	MALLOC(rep, struct nfsreq *, sizeof(struct nfsreq), M_NFSREQ, M_WAITOK);
#endif
	rep->r_xid = xid;
	rep->r_mntp = mntp;
	rep->r_vp = vp;
	rep->r_procp = u.u_procp;	/* Wrong if nfsiod handles, fix! */
	if (mntp->nm_flag & NFSMNT_SOFT)
		rep->r_retry = mntp->nm_retry;
	else
		rep->r_retry = NFS_MAXREXMIT + 1;	/* past clip limit */
	rep->r_flags = rep->r_rexmit = 0;
	rep->r_error = 0;
	/* Idempotency: add N * MINTIMEO to requests if not, else use 0 */
	rep->r_timer = rep->r_timerinit = -(idem * NFS_MINTIMEO);
	rep->r_mrep = NULL;
	rep->r_mreq = m;
	len = 0;
	while (m) {
		len += m->m_len;
		m = m->m_next;
	}
	rep->r_msiz = len;

	/*
	 * Do the client side RPC.
	 */
	nfsstats.rpcrequests++;
	NFSREQ_LOCK();
	/* Chain request into list of outstanding requests. Be sure
	 * to put it LAST so timer finds oldest requests first. */
	if (nfsreqh == NULL)
		nfsreqh = rep->r_next = rep->r_prev = rep;
	else {
		nfsreqh->r_prev->r_next = rep;
		rep->r_prev = nfsreqh->r_prev;
		nfsreqh->r_prev = rep;
		rep->r_next = nfsreqh;
	}

	/*
	 * If backing off another request or avoiding congestion, don't
	 * send this one now but let timer do it. If not timing a request,
	 * do it now.
	 */
	if (mntp->nm_hostinfo->nh_sent > 0 &&
	    (mntp->nm_hostinfo->nh_currexmit != 0 ||
	     mntp->nm_hostinfo->nh_sent >= mntp->nm_hostinfo->nh_window)) {
		NFSREQ_UNLOCK();
		goto skipsend;
	}
	++mntp->nm_hostinfo->nh_sent;	/* Inconsistent if can't NFSMCOPY */
	rep->r_flags |= R_SENT;		/* But not a catastrophe */
	if (mntp->nm_rtt == -1) {
		mntp->nm_rtt = 0;
		rep->r_flags |= R_TIMING;
	}
	NFSREQ_UNLOCK();

	/*
	 * If we can get a packet to send, send it off...
	 * otherwise the timer will retransmit later
	 */
	m = NFSMCOPY(mreq, 0, M_COPYALL, M_WAIT);
	if (m != NULL)
		(void) nfs_send(mntp->nm_so, mntp->nm_srvaddr, m, 0, len);

	/*
	 * Wait for the reply from our send or the timer's.
	 */
skipsend:
	error = nfs_dgreply(mntp->nm_so, mntp, rep);

	/*
	 * RPC done, unlink the request.
	 */
	NFSREQ_LOCK();
	if (rep->r_next == rep)
		nfsreqh = NULL;
	else {
		if (nfsreqh == rep)
			nfsreqh = rep->r_next;
		rep->r_prev->r_next = rep->r_next;
		rep->r_next->r_prev = rep->r_prev;
	}
	NFSREQ_UNLOCK();
	m_freem(rep->r_mreq);
	mrep = md = rep->r_mrep;
#if	MACH
	ZFREE(nfsreq_zone, rep);
#else
	FREE((caddr_t)rep, M_NFSREQ);
#endif
	if (error && !mrep)
		return error;

	/*
	 * break down the rpc header and check if ok
	 */
	dpos = mtod(md, caddr_t);
	nfsm_disect(p, u_long *, 5*NFSX_UNSIGNED);
	p += 2;
	if (*p++ == rpc_msgdenied) {
		if (*p == rpc_mismatch)
			error = EOPNOTSUPP;
		else
			error = EACCES;
		m_freem(mrep);
		return (error);
	}
	/*
	 * skip over the auth_verf, someday we may want to cache auth_short's
	 * for nfs_reqhead(), but for now just dump it
	 */
	if (*++p != 0) {
		len = nfsm_rndup(fxdr_unsigned(long, *p));
		nfsm_adv(len);
	}
	nfsm_disect(p, u_long *, NFSX_UNSIGNED);
	/* 0 == ok */
	if (*p == 0) {
		nfsm_disect(p, u_long *, NFSX_UNSIGNED);
		if (*p != 0) {
			error = fxdr_unsigned(int, *p);
			m_freem(mrep);
			return (error);
		}
		*mrp = mrep;
		*mdp = md;
		*dposp = dpos;
		return (0);
	}
	m_freem(mrep);
	return (EPROTONOSUPPORT);
nfsmout:
	return (error);
}

/*
 * Get a request for the server main loop
 * - receive a request via. nfs_soreceive()
 * - verify it
 * - fill in the cred struct.
 */
nfs_getreq(so, prog, vers, maxproc, nam, mrp, mdp, dposp, retxid, proc, cr,
	   msk, mtch)
	struct socket *so;
	u_long prog;
	u_long vers;
	int maxproc;
	struct mbuf **nam;
	struct mbuf **mrp;
	struct mbuf **mdp;
	caddr_t *dposp;
	u_long *retxid;
	u_long *proc;
	register struct ucred *cr;
	u_long msk;
	u_long mtch;
{
	register int i;
	register u_long *p;
	register long t1;
	caddr_t dpos, cp2;
	int error = 0;
	struct mbuf *mrep, *md;
	int len;

	if (error = nfs_dgreceive(so, msk, mtch, nam, &mrep))
		return (error);
	md = mrep;
	dpos = mtod(mrep, caddr_t);
	nfsm_disect(p, u_long *, 10*NFSX_UNSIGNED);
	*retxid = *p++;
	if (*p++ != rpc_call) {
		m_freem(mrep);
		return (ERPCMISMATCH);
	}
	if (*p++ != rpc_vers) {
		m_freem(mrep);
		return (ERPCMISMATCH);
	}
	if (*p++ != prog) {
		m_freem(mrep);
		return (EPROGUNAVAIL);
	}
	if (*p++ != vers) {
		m_freem(mrep);
		return (EPROGMISMATCH);
	}
	*proc = fxdr_unsigned(u_long, *p);
	p++;
	if (*proc == NFSPROC_NULL) {
		*mrp = mrep;
		return (0);
	}
	if (*proc > maxproc || *p++ != rpc_auth_unix) {
		m_freem(mrep);
		return (EPROCUNAVAIL);
	}
	len = fxdr_unsigned(int, *p);
	if (len < 0 || len > RPCAUTH_MAXSIZ) {
		m_freem(mrep);
		return (EBADRPC);
	}
	p += 2;
	len = fxdr_unsigned(int, *p);
	if (len < 0 || len > NFS_MAXNAMLEN) {
		m_freem(mrep);
		return (EBADRPC);
	}
	nfsm_adv(nfsm_rndup(len));
	nfsm_disect(p, u_long *, 3*NFSX_UNSIGNED);
	cr->cr_uid = fxdr_unsigned(uid_t, *p);
	p++;
	cr->cr_gid = fxdr_unsigned(gid_t, *p);
	p++;
	len = fxdr_unsigned(int, *p);
	if (len < 0 || len > RPCAUTH_UNIXGIDS) {
		m_freem(mrep);
		return (EBADRPC);
	}
	nfsm_disect(p, u_long *, (len + 2)*NFSX_UNSIGNED);
	for (i = 0; i < len; i++) {
		if (i < NGROUPS)
			cr->cr_groups[i] = fxdr_unsigned(gid_t, *p);
		p++;
	}
	cr->cr_ngroups = (len > NGROUPS) ? NGROUPS : len;
	/*
	 * Do we have any use for the verifier.
	 * According to the "Remote Procedure Call Protocol Spec." it
	 * should be AUTH_NULL, but some clients make it AUTH_UNIX?
	 * For now, just skip over it
	 */
	p++;
	len = fxdr_unsigned(int, *p);
	if (len < 0 || len > RPCAUTH_MAXSIZ) {
		m_freem(mrep);
		return (EBADRPC);
	}
	if (len > 0)
		nfsm_adv(nfsm_rndup(len));
	*mrp = mrep;
	*mdp = md;
	*dposp = dpos;
	return (0);
nfsmout:
	return (error);
}

/*
 * Generate the rpc reply header
 * siz arg. is used to decide if adding a cluster is worthwhile
 */
nfs_rephead(siz, retxid, err, mrq, mbp, bposp)
	int siz;
	u_long retxid;
	int err;
	struct mbuf **mrq;
	struct mbuf **mbp;
	caddr_t *bposp;
{
	register u_long *p;
	register long t1;
	caddr_t bpos;
	struct mbuf *mreq, *mb, *mb2;

	NFSMGETHDR(mreq);
	mb = mreq;
	if ((siz+RPC_REPLYSIZ) > MHLEN)
		NFSMCLGET(mreq, M_WAIT);
	p = mtod(mreq, u_long *);
	mreq->m_len = 6*NFSX_UNSIGNED;
	bpos = ((caddr_t)p)+mreq->m_len;
	*p++ = retxid;
	*p++ = rpc_reply;
	if (err == ERPCMISMATCH) {
		*p++ = rpc_msgdenied;
		*p++ = rpc_mismatch;
		*p++ = txdr_unsigned(2);
		*p = txdr_unsigned(2);
	} else {
		*p++ = rpc_msgaccepted;
		*p++ = 0;
		*p++ = 0;
		switch (err) {
		case EPROGUNAVAIL:
			*p = txdr_unsigned(RPC_PROGUNAVAIL);
			break;
		case EPROGMISMATCH:
			*p = txdr_unsigned(RPC_PROGMISMATCH);
			nfsm_build(p, u_long *, 2*NFSX_UNSIGNED);
			*p++ = txdr_unsigned(2);
			*p = txdr_unsigned(2);	/* someday 3 */
			break;
		case EPROCUNAVAIL:
			*p = txdr_unsigned(RPC_PROCUNAVAIL);
			break;
		default:
			*p = 0;
			if (err != VNOVAL) {
				nfsm_build(p, u_long *, NFSX_UNSIGNED);
				*p = txdr_unsigned(err);
			}
			break;
		};
	}
	*mrq = mreq;
	*mbp = mb;
	*bposp = bpos;
	if (err != 0 && err != VNOVAL)
		nfsstats.srvrpc_errs++;
	return (0);
}

/*
 * Nfs timer routine
 * Scan the nfsreq list and retranmit any requests that have timed out
 * To avoid retransmission attempts on STREAM sockets (in the future) make
 * sure to set the r_retry field to 0 (implies nm_retry == 0).
 */
nfs_timer()
{
	register struct nfsreq *rep;
	register struct mbuf *m;
	register struct socket *so;
	register struct nfsmount *mntp;
	int error;
	int resending;
	NFSREQ_LOCK_DECL()
	DOMAIN_FUNNEL_DECL(f)

	NFSREQ_LOCK();
	if (rep = nfsreqh) do {
		mntp = rep->r_mntp;
		if (rep->r_flags & R_TIMING)	/* update rtt in mount */
			mntp->nm_rtt++;
		/* If not timed out or reply already received, skip */
		if (rep->r_mrep) 	  /* Reply receive already */
			goto next;
		else if ((++rep->r_timer<mntp->nm_rto) && 
			 (rep->r_flags & R_SENT))   /* Sent the request */
			goto next;
		/* Do backoff and save new timeout in mount */
		if (rep->r_flags & R_TIMING) {
			/*
			 * Only backoff the timer if the request has
			 * actually been sent.
			 */
			if (rep->r_flags & R_SENT) {
				nfs_backofftimer(mntp);
				resending = 1;
			} else {
				resending = 0;
			}
			rep->r_flags &= ~R_TIMING;
			mntp->nm_rtt = -1;
		}
		if (rep->r_flags & R_SENT) {
			rep->r_flags &= ~R_SENT;
			--mntp->nm_hostinfo->nh_sent;
		}
		/* Check state of socket, cf nfs_send */
		so = mntp->nm_so;
		DOMAIN_FUNNEL(sodomain(so), f);
		SOCKET_LOCK(so);
		if (error = nfs_sockerr(so, 1))
			goto wakeup;
		if ((mntp->nm_flag & NFSMNT_INT) && nfs_sigintr(rep->r_procp)){
			rep->r_error = EINTR;
			error = -1;
			goto wakeup;
		}
		/* Check for too many retries, cf nfs_dgreply */
		if ( (resending) &&
		     ((rep->r_rexmit + 1) > NFS_MAXREXMIT) )	/* clip */
			rep->r_rexmit = NFS_MAXREXMIT;
		if ( (rep->r_rexmit + 1 )  > rep->r_retry) {	/* too many */
			rep->r_error = ETIMEDOUT;
			error = -1;
			goto wakeup;
		}
		if ( (rep->r_rexmit + 1) >= nfsrexmtthresh)
			error = -1;
		/* Check for congestion control, cf nfs_request */
		if (mntp->nm_hostinfo->nh_sent >= mntp->nm_hostinfo->nh_window)
			goto wakeup;
		SOCKBUF_LOCK(&so->so_snd);
		if (sbspace(&so->so_snd) < rep->r_msiz) {
			SOCKBUF_UNLOCK(&so->so_snd);
			goto skip;
		}
		SOCKBUF_UNLOCK(&so->so_snd);
		/* Send it! */
		m = NFSMCOPY(rep->r_mreq, 0, M_COPYALL, M_DONTWAIT);
		if (m) {
			m->m_pkthdr.len = rep->r_msiz;
			(void)(*so->so_proto->pr_usrreq)(so, PRU_SEND, m,
			  mntp->nm_srvaddr, (struct mbuf *)0, (struct mbuf *)0);
			/*
			 * Don't inc the counters unless you actually
			 * REsend the request.  It is possible to
			 * come thru here to send requests that have
			 * been waiting to get sent for the first time.
			 */
			if(resending) {
				rep->r_rexmit++; 
	                        nfsstats.rpcretries++;
			}
		}
		++mntp->nm_hostinfo->nh_sent;
		rep->r_flags |= R_SENT;
skip:
		/* We need to time the request even though we're
		 * retransmitting, in order to maintain backoff. */
		mntp->nm_rtt = 0;
		rep->r_flags |= R_TIMING;
		rep->r_timer = rep->r_timerinit;
wakeup:
		if (error) {
			if (error > 0) so->so_error = error;
			sorwakeup(so);
		}
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
next:
		rep = rep->r_next;
	} while (rep != nfsreqh);
	NFSREQ_UNLOCK();
#if	!NETISR_THREAD
	timeout(nfs_timer, (caddr_t)0, hz/NFS_HZ);
#else
	return (hz/NFS_HZ);
#endif
}

/*
 * NFS timer update and backoff. The "Jacobson/Karels/Karn" scheme is
 * used here. The timer state is held in the nfsmount structure and
 * a single request is used to clock the response. When successful
 * the rtt smoothing in nfs_updatetimer is used, when failed the backoff
 * is done by nfs_backofftimer. We also log failure messages in these
 * routines.
 *
 * Congestion variables are held in the nfshost structure which
 * is referenced by nfsmounts and shared per-server. This separation
 * makes it possible to do per-mount timing which allows varying disk
 * access times to be dealt with, while preserving a network oriented
 * congestion control scheme.
 *
 * The windowing implements the Jacobson/Karels slowstart algorithm
 * with adjusted scaling factors. We start with one request, then send
 * 4 more after each success until the ssthresh limit is reached, then
 * we increment at a rate proportional to the window. On failure, we
 * remember 3/4 the current window and clamp the send limit to 1. Note
 * ICMP source quench is not reflected in so->so_error so we ignore that
 * for now.
 *
 * NFS behaves much more like a transport protocol with these changes,
 * shedding the teenage pedal-to-the-metal tendencies of "other"
 * implementations.
 *
 * Timers and congestion avoidance by Tom Talpey, Open Software Foundation.
 */

/* This turned out to be not forgiving enough... because the NFS server
 * responds only after performing lookups/diskio/etc, we have to be
 * more prepared to accept a spiky variance. Cf tcp algorithm.
#define NFS_RTO(mntp)	((((mntp)->nm_srtt >> 2) + (mntp)->nm_rttvar) >> 1)
*/

#define NFS_RTO(mntp)	(((mntp)->nm_srtt >> 3) + (mntp)->nm_rttvar)

nfs_updatetimer(mntp)
	register struct nfsmount *mntp;
{
	register struct nfshost *nfshp = mntp->nm_hostinfo;

	/* If retransmitted, clear and return */
	if (mntp->nm_rexmit || nfshp->nh_currexmit) {
		if (nfshp->nh_currexmit >= nfsrexmtthresh)
			nfs_log("NFS server %s OK\n", 
				mntp->nm_mountp->m_stat.f_mntfromname);
		mntp->nm_rexmit = nfshp->nh_currexmit = 0;
		return;
	}
	/* If have a measurement, do smoothing */
	if (mntp->nm_srtt) {
		register short delta;
		delta = mntp->nm_rtt - (mntp->nm_srtt >> 3);
		if ((mntp->nm_srtt += delta) <= 0)
			mntp->nm_srtt = 1;
		if (delta < 0)
			delta = -delta;
		delta -= (mntp->nm_rttvar >> 2);
		if ((mntp->nm_rttvar += delta) <= 0)
			mntp->nm_rttvar = 1;
	/* Else initialize */
	} else {
		mntp->nm_rttvar = mntp->nm_rtt << 1;
		if (mntp->nm_rttvar == 0) mntp->nm_rttvar = 2;
		mntp->nm_srtt = mntp->nm_rttvar << 2;
	}
	/* Compute new Retransmission TimeOut and clip */
	mntp->nm_rto = NFS_RTO(mntp);
	if (mntp->nm_rto < NFS_MINTIMEO)
		mntp->nm_rto = NFS_MINTIMEO;
	else if (mntp->nm_rto > NFS_MAXTIMEO)
		mntp->nm_rto = NFS_MAXTIMEO;
	nfshp->nh_currto = mntp->nm_rto;

	/* Update window estimate */
	if (nfshp->nh_window < nfshp->nh_ssthresh)	/* quickly */
		nfshp->nh_window += 4;
	else {						/* slowly */
		register long incr = ++nfshp->nh_winext;
		incr = (incr * incr) / nfshp->nh_window;
		if (incr > 0) {
			nfshp->nh_winext = 0;
			++nfshp->nh_window;
		}
	}
	if (nfshp->nh_window > NFS_MAXWINDOW)
		nfshp->nh_window = NFS_MAXWINDOW;
}

nfs_backofftimer(mntp)
	register struct nfsmount *mntp;
{
	register struct nfshost *nfshp = mntp->nm_hostinfo;
	register unsigned long newrto;

	/* Clip shift count */
	if (++mntp->nm_rexmit > 8 * sizeof mntp->nm_rto)
		mntp->nm_rexmit = 8 * sizeof mntp->nm_rto;
	/* Back off RTO exponentially */
	newrto = NFS_RTO(mntp);
	newrto <<= (mntp->nm_rexmit - 1);
	if (newrto == 0 || newrto > NFS_MAXTIMEO)
		newrto = NFS_MAXTIMEO;
	else if (newrto < NFS_MINTIMEO)
		newrto = NFS_MINTIMEO;
	mntp->nm_rto = nfshp->nh_currto = newrto;

	/* If too many retries, message, assume a bogus RTT and re-measure */
	if (nfshp->nh_currexmit < mntp->nm_rexmit) {
		nfshp->nh_currexmit = mntp->nm_rexmit;
		if (nfshp->nh_currexmit >= nfsrexmtthresh) {
			if (nfshp->nh_currexmit == nfsrexmtthresh) {
				nfs_log("NFS server %s not responding\n",
					 mntp->nm_mountp->m_stat.f_mntfromname);
				mntp->nm_rttvar += (mntp->nm_srtt >> 2);
				mntp->nm_srtt = 0;
			}
			/* The routing invalidation should be a usrreq PRU */
#ifdef	INET
			if (mtod(nfshp->nh_sockaddr,
				struct sockaddr *)->sa_family == AF_INET) {
				DOMAIN_FUNNEL_DECL(f)
				DOMAIN_FUNNEL(sodomain(mntp->nm_so), f);
#if	!NETSYNC_LOCK
				in_losing(mntp->nm_so->so_pcb);
#else
				in_losing_lock(mntp->nm_so->so_pcb);
#endif
				DOMAIN_UNFUNNEL(f);
			}
#endif
		}
	}
	/* Close down window but remember this point (3/4 current) for later */
	nfshp->nh_ssthresh = ((nfshp->nh_window << 1) + nfshp->nh_window) >> 2;
	nfshp->nh_window = 1;
	nfshp->nh_winext = 0;
}

/*
 * Test for a termination signal pending on procp.
 * This is used for NFSMNT_INT mounts.
 */
nfs_sigintr(p)
	register struct proc *p;
{
	return (p && p->p_sig &&
	    (((p->p_sig &~ p->p_sigmask) &~ p->p_sigignore) & NFSINT_SIGMASK));
}

/*
 * Notify user of nfs server error, if can find tty.
 * NOTE new BSD4.4 changes tprintf ttyp to ttyvp!!
 */
nfs_tprintf(rep, str1, str2)
	struct nfsreq *rep;
	char *str1, *str2;
{
	struct tty *ttyp = NULL;

	if (rep->r_procp) {
#if	defined(OSF1_ADFS)
		tprintf(rep->r_procp->p_pid, str1, str2);
#else	/* OSF1_ADFS */
		if (VPOP_CTTY_GETATTR(rep->r_procp->p_vproc, 0, 0, 0, &ttyp)) {
			if (ttyp) {
				tprintf(ttyp, str1, str2);
			}
		}
#endif	/* OSF1_ADFS */
	}
}

/*
 * Not all errors are fatal. The closed checks deal
 * with errors a little strangely.
 */

nfs_sockerr(so, sending)
	struct socket *so;
	int sending;
{
	if (sending && (so->so_state & SS_CANTSENDMORE)) {
		so->so_error = EPIPE;
		return EPIPE;
	}

	switch (so->so_error) {			/* inhibit certain errors */
	case ENETDOWN:
	case ENETUNREACH:
	case EHOSTDOWN:
	case EHOSTUNREACH:
		so->so_error = 0;
	case 0:
		break;
	default:				/* return all others */
		return so->so_error;
	}

	if (!sending && (so->so_state & SS_CANTRCVMORE)) {
		so->so_error = 0;		/* (no error) */
		return EPIPE;
	}
	return so->so_error;
}
