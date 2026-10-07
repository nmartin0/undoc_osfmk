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
 * $Log: socketvar.h,v $
 * Revision 1.19  1995/02/01  21:37:52  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.18  1994/11/18  20:41:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/08/31  22:46:56  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.15.2.1  1994/08/22  21:42:37  nina
 *  Reviewer:yazz, hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7618, 10383. See also 7155 and 10114.
 *  Testing:NFS EATS. TCP/IP EATS
 *  Module(s):
 * 	server/sys/socketvar.h
 * 	server/bsd/uipc_socket.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 *
 * 	cmds_libs/usr/src/sbin/portmap/Makefile
 * 	cmds_libs/usr/src/sbin/portmap/portmap.c
 *
 * 	cmds_libs/usr/src/sbin/mountd/Makefile
 * 	cmds_libs/usr/src/sbin/mountd/mountd.c
 *
 * 	cmds_libs/usr/src/sbin/nfsd/Makefile
 * 	cmds_libs/usr/src/sbin/nfsd/nfsd.c
 *
 * 	cmds_libs/usr/src/sbin/nfsiod/Makefile
 * 	cmds_libs/usr/src/sbin/nfsiod/nfsiod.c
 *
 * 	cmds_libs/usr/src/ccs/lib/libnx/Makefile
 * 	cmds_libs/usr/src/ccs/lib/libnx/tnc_subs.c (NEW)
 *
 * 	cmds_libs/sbin/init.d/nfs
 *
 *
 * Modifications have been made to the server, libnx, the NFS
 * daemons and nfs start/stop script so that each configured
 * network server node runs its own copy of the NFS daemons.
 *
 * Added the VS_BINDLOCAL flag to socketvar.h
 *
 * Revision 1.15  1994/06/15  17:24:01  mjl
 * Add synchronization variables to socket struct, used to break race between
 * close of remote virtual socket and asynchronous status checking RPCs.
 *
 *  Reviewer: Bob Yasi <yazz@locus.com>, Charlie Johnson <cfj@ssd.intel.com>
 *  Risk: Medium
 *  Benefit or PTS #: 9024
 *  Testing: NFS mounts on HiPPI configurations now succeed.
 *  Module(s):
 * 	server/sys/socketvar.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/sys_vsocket.c
 *
 * Revision 1.14  1994/05/04  21:42:05  mjl
 * TNC select rewrite.  Add magic number to socket structure.
 *
 *  Reviewer: Charlie Johnson (Intel), Bob Yasi (Locus)
 *  Risk: Medium
 *  Benefit or PTS #: #7537 + select rewrite
 *  Testing: VSX, EATS, bobtest, Eval
 *  Module(s):
 * 	server/bsd/subr_select.c
 * 	server/sys/select.h
 * 	server/sys/socketvar.h
 * 	server/sys/user.h
 * 	server/tnc/un_debug.c
 * 	server/tnc/un_debug.h
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/bsd_server_side.c
 * 	server/uxkern/fsvr.defs
 * 	server/uxkern/fsvr2_server_side.c
 * 	server/uxkern/fsvr_port.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/port_hash.c
 * 	server/uxkern/port_hash.h
 * 	server/vsocket/mi_config.c
 * 	server/vsocket/sys_vsocket.c
 * 	server/vsocket/two_way_hash.h
 * 	server/vsocket/vs.defs
 * 	server/vsocket/vs_chouse.c
 * 	server/vsocket/vs_debug.c
 * 	server/vsocket/vs_init.c
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_subr.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vsocket.h
 *
 * Revision 1.13  1994/02/04  01:08:42  jlitvin
 * Two attempts to remove a trigraph, give me a break!  Sorry...
 *
 * Revision 1.12  1994/02/03  21:38:35  jlitvin
 * Remove inadvertent trigraph sequence to make lint happy.
 *
 * Revision 1.11  1993/09/21  21:04:37  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.10.2.1  1993/09/21  21:03:14  cfj
 * Raised SB_MAX to (530*1024). PTS #6121.
 *
 * Revision 1.10  1993/09/01  01:36:14  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.9  1993/07/29  21:53:25  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network
 * server slowdown.
 *
 * Revision 1.8  1993/07/16  18:52:22  hobbes
 * Raised the SB_MAX value to support HIPPI.
 *
 * Revision 1.7  1993/07/14  18:27:50  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:05:16  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.3  1993/07/01  20:37:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:20:01  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.5  1993/04/07  22:02:59  cfj
 * Merged with T9.
 *
 * Revision 1.3.4.1  1993/04/05  00:36:22  nandy
 * Added fields to to detect state changes in the vsocket layer.
 *
 * Revision 1.4  1993/04/03  03:07:58  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:05:03  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/15  01:57:01  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1  1992/11/05  22:41:32  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.19  93/08/19  15:05:02  bhk
 * Merged HIPPI support
 * Removed unsued field in the virtual socket (vs_index)
 * 
 * Revision 2.18  93/08/11  15:00:58  mjl
 * [Bug #0340] Turn obsolete VS_SLEEP_CANCELLED flag into VS_CLOSING flag,
 * used when awakening sleeping r_vs_soreadable() threads at close time.
 * 
 * Revision 2.17  93/08/09  15:51:29  bhk
 * added a define for the callback port for virtual sockets
 * 
 * Revision 2.16  93/08/06  17:53:41  bhk
 * Added last sleep command to vsocket structure
 * Added IS sleeping bit to state flags
 * 
 * Revision 2.15  93/08/03  16:58:38  mjl
 * Added VS_IS_SHADOW flag for explicitly testing if a socket is a remote
 * secondary.
 * 
 * Revision 2.14  93/07/29  11:35:16  mjl
 * [Bug #0326] Added VS_SLEEP_CANCELLED flag (and reformatted a bit).
 * 
 * Revision 2.13  93/07/07  16:31:12  bhk
 * Added SOCKET_LOCK_TRY to allow us to unwind from a deadlock situation
 * 
 * Revision 2.12  93/04/03  11:48:03  klh
 * Added fields to allow detection of state transition in the virtual socket
 * layer.
 * 
 * Revision 2.11  92/12/10  17:06:24  mjl
 * Corrected erroneous comment.
 * 
 * Revision 2.10  92/08/08  01:41:50  jdh
 * added support for moving bound socket and vnode ports -- jdh
 * 
 * Revision 2.9  92/07/26  17:29:27  bhk
 * added suport for select on remote sockets
 * 
 * Revision 2.8  92/06/16  18:34:14  bhk
 * added vs_index and vs_pport for remote sleep/wakeup
 * 
 * Revision 2.7  92/06/15  17:32:09  mjl
 * Remove SOSETFP() macro (obsolete).
 * 
 * Revision 2.6  92/04/20  17:28:24  bhk
 * Added virtual sockets fields to the socket structure
 * 
 * Revision 2.5  92/04/06  10:54:17  mjl
 * Added some useful macros, and also some more TNC-specific fields to the
 * end of the socket structure.  Eventually these fields should be moved
 * elsewhere, leaving only vs_ops and vs_addr as additional fields here.
 * 
 * Revision 2.4  92/03/19  10:02:34  mjl
 * Added vs_data pointer to private data for virtual socket layer.
 * 
 * Revision 2.3  92/03/04  15:15:35  bhk
 * added virtual sockets	bhk
 * 
 * Revision 2.2  91/08/31  14:12:48  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:45:17  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.15  90/10/31  14:06:57  devrcs
 * 	Add so_dqlen, so_dq, and SP_CLOSING (see uipc_socket.c).
 * 	Turn off SOCKBUF_LOCK's if !DEBUG. Rearrange socket struct
 * 	to fit new fields. Move comments to uipc_socket.c.
 * 	[90/10/12  08:54:14  tmt]
 * 
 * Revision 1.14  90/10/07  14:54:07  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:45:05  gm]
 * 
 * 	Add SP_PIPE bit (was SS_PIPE) to new "so_special" field.
 * 	Move other SS_* bits to SP_* bits here. Update macros.
 * 	Add SE_POLL bit to socket events.
 * 	[90/09/20  18:39:17  tmt]
 * 
 * Revision 1.13  90/08/24  12:27:30  devrcs
 * 	Fix bit collision in SE_ socket events.
 * 	[90/08/14  19:12:41  tmt]
 * 
 * Revision 1.12  90/07/27  09:08:02  devrcs
 * 	Update to BSD Reno release.
 * 	Rearrange socket struct. Add "sowatomic" macro.
 * 	[90/07/19  15:50:43  tmt]
 * 
 * Revision 1.11  90/07/05  23:14:33  devrcs
 * 	Add sodomain macro.
 * 	[90/07/03  18:58:59  tmt]
 * 
 * Revision 1.10  90/06/22  20:54:07  devrcs
 * 	Add SOHASUAREA macro (of interest to security code and others).
 * 	[90/06/11  16:40:19  tmt]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:46:34  seiden]
 * 
 * 	Add SS_PIPE bit to state (only one more bit... add new?)
 * 	[90/06/06  15:04:39  tmt]
 * 
 * Revision 1.9  90/05/13  20:33:53  devrcs
 * 	Add SS_NOUAREA, XTI wakeup event bits.
 * 	[90/05/04  14:19:41  tmt]
 * 
 * Revision 1.8  90/04/27  19:27:19  devrcs
 * 	Delete and add some sockbuf flags.
 * 	[90/04/20  12:08:37  tmt]
 * 
 * Revision 1.7  90/04/14  00:34:12  devrcs
 * 	Add sockbuf wakeup routine pointer, add SS_LOCKABLE bit, _KERNEL
 * 	[90/04/09  15:46:01  tmt]
 * 
 * Revision 1.6  90/03/27  13:27:51  gm
 * 	Change args to lock_init2() [gmf]
 * 
 * Revision 1.5  90/02/05  15:51:43  robert
 * 	Hide most lock details in macros, consolidate lockinits. No-op the
 * 	sockbuf locks when not testing (see comment). Macros for lock pairs.
 * 	[90/01/19  15:22:03  tmt]
 * 
 * Revision 1.4  90/01/18  08:49:04  gm
 * 	#ifdefs for user code compatability.
 * 	[90/01/16  13:44:36  tmt]
 * 
 * 	Make file re-entrant.
 * 	Pull in some header files and do a typedef for compat.
 * 	Make structure overlay constant regardless of config options.
 * 	Charge for m_ext.ext_size, not MCLBYTES in sballoc.
 * 	[90/01/08  15:43:00  tmt]
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
 * Revision 1.3  90/01/03  12:44:01  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:41:43  gm]
 * 
 * Revision 1.2  89/12/26  10:38:50  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)socketvar.h	2.1 16:08:08 4/20/90 SecureWare */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986, 1990 Regents of the University of California.
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
 *	Base:	socketvar.h	7.6 (Berkeley) 9/4/89
 *	Merged: socketvar.h	7.11 (Berkeley) 6/30/90
 */

#ifndef	_SYS_SOCKETVAR_H_
#define	_SYS_SOCKETVAR_H_

#if	defined(_KERNEL) && !defined(_NET_GLOBALS_H_)
#include "net/net_globals.h"
#define sb_lock_t	lock_t
#else
typedef struct lock *	sb_lock_t;
#endif

#if	MACH
#include <sys/secdefines.h>
#if	SEC_ARCH
#include <sys/security.h>
#endif
#endif
#if !defined(_VSOCKET_H_)
#define vsocket_ops_t void
#endif

#ifdef TNC

#define	SOCREATE(a,b,c,d)	vsocreate(a,b,c,d)
#define TNC_SOSLEEP_HOOK(s)	( (s)->vs_flags & VS_RESTART ? ERESTART : 0 )
#define IS_RELOCATING(s)	( (s)->vs_flags & VS_RESTART )
#define WAITING_ON_FP_SEQNO(s)  ( (s)->vs_flags & VS_FP_SEQNO_WAIT )
#define WAITING_ON_BP_SEQNO(s)  ( (s)->vs_flags & VS_BP_SEQNO_WAIT )
#define VS_MAGIC		0x19540409

#else
#define	SOCREATE(a,b,c,d)	socreate(a,b,c,d)
#endif


/*
 * Kernel structure per socket.
 * Contains send and receive buffer queues,
 * handle on protocol and pointer to protocol
 * private data and error information.
 */
struct socket {
	short	so_type;		/* generic type, see socket.h */
	short	so_options;		/* from socket call, see socket.h */
	short	so_linger;		/* time to linger while closing */
	short	so_state;		/* internal state flags SS_*, below */
	caddr_t	so_pcb;			/* protocol control block */
	struct	protosw *so_proto;	/* protocol handle */
	struct	socklocks *so_lock;	/* socket structure lock(s) */
/*
 * Variables for connection queueing.
 * Socket where accepts occur is so_head in all subsidiary sockets.
 * If so_head is 0, socket is not related to an accept.
 * For head socket so_q0 queues partially completed connections,
 * while so_q is a queue of connections ready to be accepted.
 * If a connection is aborted and it has so_head set, then
 * it has to be pulled out of either so_q0 or so_q.
 * We allow connections to queue up based on current queue lengths
 * and limit on number of queued connections for this socket.
 */
	struct	socket *so_head;	/* back pointer to accept socket */
	struct	socket *so_q0;		/* queue of partial connections */
	struct	socket *so_q;		/* queue of incoming connections */
	struct	socket *so_dq;		/* queue of defunct connections */
	short	so_q0len;		/* partials on so_q0 */
	short	so_qlen;		/* number of connections on so_q */
	short	so_qlimit;		/* max number queued connections */
	short	so_dqlen;		/* listener dequeues in progress */
	short	so_timeo;		/* connection timeout */
	u_short	so_error;		/* error affecting connection */
	short	so_special;		/* special state flags SP_*, below */
	short	so_spare;		/* available: pad for pid_t */
	pid_t	so_pgid;		/* pgid for signals */
	u_long	so_oobmark;		/* chars to oob mark */
/*
 * Variables for socket buffering.
 */
	struct	sockbuf {
		u_long	sb_cc;		/* actual chars in buffer */
		u_long	sb_hiwat;	/* max actual char count */
		u_long	sb_mbcnt;	/* chars of mbufs used */
		u_long	sb_mbmax;	/* max chars of mbufs to use */
		long	sb_lowat;	/* low water mark */
		struct	mbuf *sb_mb;	/* the mbuf chain */
		union {			/* process selecting read/write */
			struct proc *sb_selproc;	/* UNIX */
			struct sbselque {		/* MACH */
				struct sbselque *next, *prev;
			} sb_selque;
		} sb_select;
		short	sb_flags;	/* flags, see below */
		short	sb_timeo;	/* timeout for read/write */
		void	(*sb_wakeup)();	/* upcall instead of sowakeup */
		caddr_t	sb_wakearg;	/* (*sb_wakeup)(sb_wakearg, ?); */
		sb_lock_t sb_lock;	/* sockbuf lock (in socklocks) */
	} so_rcv, so_snd;
#define sb_sel		sb_select.sb_selproc
#define sb_selq		sb_select.sb_selque
#define	SB_MAX		(530*1024)	/* default for max chars in sockbuf */
#define	SB_LOCK		0x01		/* lock on data queue */
#define	SB_WANT		0x02		/* someone is waiting to lock */
#define	SB_WAIT		0x04		/* someone is waiting for data/space */
#define	SB_SEL		0x08		/* someone is selecting */
#define	SB_ASYNC	0x10		/* ASYNC I/O, need signals */
#define	SB_NOTIFY	(SB_WAIT|SB_SEL|SB_ASYNC)
#define	SB_COLL		0x20		/* collision selecting (UNIX) */
#define	SB_NOINTR	0x40		/* operations not interruptible */
#define	SB_WAKEONE	0x80		/* wakeup only one on notify */
#define	SB_INHERIT	(SB_NOINTR|SB_WAKEONE)

	caddr_t	so_tpcb;		/* Wisc. protocol control block XXX */
#if	SEC_ARCH
	tag_t	so_tag[SEC_TAG_COUNT];	/* security policy tags */
#endif
#ifdef TNC
	vsocket_ops_t *vs_ops;		/* virtual socket operations */
	caddr_t	vs_data;		/* protocol specific storage */
	caddr_t	vs_opts;		/* protocol specific options holder */
	int	vs_flags;		/* virtual socket flags see VS_ */
	int	vs_refcnt;		/* reference count */
	int	vs_oldqlen;		/* previous queue length	*/
	int	vs_oldstate;		/* previous state		*/
	int	vs_sleep_cmd;		/* last sleep command		*/
	long	vs_magic;		/* sanity check port-to-vs xlation */

	/*
	 *  For coordinating race between async RPCs to secondaries and
	 *  r_vs_soclose() RPC.
	 */
	struct condition vs_can_close;
	struct mutex	vs_close_mutex;
	short	vs_is_closing;
	short	vs_defer_close;

	mach_port_t	vs_bindport;	/* port stored in bound sock's vnode */
	struct mutex	vs_seqno_mutex;	/* lock to coordinate checking */
					/* and updating of file port or */
					/* vnode-bindport sequence count. */
	caddr_t vs_debugfmt;
	mach_port_t		vs_fport;
#define	vs_callback	vs_bindport
#endif	/* TNC */
};

#ifdef	TNC
/*
 *  Flag values for so->vs_flags
 */
#define	VS_RESTART		0x0001	/* Force ERESTART error from sosleep */
#define VS_NOTHREADS		0x0002	/* No more threads active on socket */
#define	VS_INITIALIZED		0x0004	/* For ustdebug consistency checking */
#define VS_ISBOUND		0x0008	/* Socket has a network server */
#define	VS_IS_REMOTE		0x0010	/* Network server is remote */
#define	VS_IS_MULTI		0x0020	/* socket has >1 network servers */
#define VS_IS_PORT		0x0080	/* socket is a port	*/
#define VS_SELECT_PORT		0x0100	/* socket has select port */
#define VS_FP_SEQNO_WAIT	0x0200	/* socket is waiting for file port's */
					/*  seqno to be updated */
#define VS_BP_SEQNO_WAIT	0x0400	/* socket is waiting for bound */
					/*  socket port's seqno to be updated */
#define VS_CLOSING		0x0800	/* remote socket is being closed */
#define	VS_IS_SHADOW		0x1000	/* a "shadow" or "secondary" socket */
#define VS_DEFER_CLOSE		0x2000	/* async RPC will want SOCKET_LOCK */
#define VS_CLOSE_DEFERRED	0x4000	/* remote close waits for async RPC */
					/*  to take the SOCKET_LOCK */
#define VS_BINDLOCAL		0x8000  /* bind only to local network server */
#endif	/* TNC */

/*
 * Socket state bits.
 */
#define	SS_NOFDREF		0x001	/* no file table ref any more */
#define	SS_ISCONNECTED		0x002	/* socket connected to a peer */
#define	SS_ISCONNECTING		0x004	/* in process of connecting to peer */
#define	SS_ISDISCONNECTING	0x008	/* in process of disconnecting */
#define	SS_CANTSENDMORE		0x010	/* can't send more data to peer */
#define	SS_CANTRCVMORE		0x020	/* can't receive more data from peer */
#define	SS_RCVATMARK		0x040	/* at mark on input */

#define	SS_PRIV			0x080	/* privileged for broadcast, raw... */
#define	SS_NBIO			0x100	/* non-blocking ops */
#define	SS_ASYNC		0x200	/* async i/o notify */
#define	SS_ISCONFIRMING		0x400	/* deciding to accept connection req */

/*
 * Special socket state bits.
 */
#define SP_PIPE			0x0001	/* socket is (unnamed) pipe */
#define SP_WATOMIC		0x0002	/* pipe write atomicity */
#define SP_NOUAREA		0x0004	/* no u-area available (XTI - XXX) */
#define SP_LOCKABLE		0x0008	/* socket of parallel domain */
#define SP_CLOSING		0x0010	/* closing a listening socket */
#define SP_FREEABLE		0x8000	/* free socket on unlock */
#define SP_INHERIT		(SP_PIPE|SP_WATOMIC|SP_NOUAREA|SP_LOCKABLE)

#ifdef	_KERNEL

/*
 * Macros for sockets, socket locking and socket buffering.
 */

#if	NETSYNC_LOCK

struct  socklocks {
	lock_data_t	sock_lock;
	lock_data_t	snd_lock;
	lock_data_t	rcv_lock;
	int		refcnt;
};

#define SOCKET_LOCKINIT(so, lp)  { \
	(so)->so_lock = (lp); \
	(so)->so_snd.sb_lock = &((lp)->snd_lock); \
	(so)->so_rcv.sb_lock = &((lp)->rcv_lock); \
	(lp)->refcnt = 0; \
	lock_init2(&((lp)->sock_lock), TRUE, LTYPE_SOCKET);  \
	lock_init2(&((lp)->snd_lock),  TRUE, LTYPE_SOCKBUF); \
	lock_init2(&((lp)->rcv_lock),  TRUE, LTYPE_SOCKBUF); \
}

/* The socket is considered locked for assertions if not lockable */
#define SOCKET_ISLOCKED(so)  (!(so->so_special & SP_LOCKABLE) || \
				lock_islocked(&((so)->so_lock->sock_lock)))

/* Lock order of unpaired sockets is critical to avoid deadlock. */
#define SOCKET_LOCK2(so1, so2) { \
	if ((so1) < (so2)) { \
		SOCKET_LOCK(so1); \
		if ((so1)->so_lock != (so2)->so_lock) \
			SOCKET_LOCK(so2); \
	} else { \
		SOCKET_LOCK(so2); \
		if ((so1)->so_lock != (so2)->so_lock) \
			SOCKET_LOCK(so1); \
	} \
}
#define SOCKET_UNLOCK2(so1, so2) { \
	if ((so1) < (so2)) { \
		if ((so1)->so_lock != (so2)->so_lock) \
			SOCKET_UNLOCK(so2); \
		SOCKET_UNLOCK(so1); \
	} else { \
		if ((so1)->so_lock != (so2)->so_lock) \
			SOCKET_UNLOCK(so1); \
		SOCKET_UNLOCK(so2); \
	} \
}

#define SOCKET_LOCK_TRY(so)		lock_try_write(&((so)->so_lock->sock_lock))

#if     NETSYNC_LOCKTEST

#define SOCKET_LOCK(so) \
	{ if (lock_socheck(so)) lock_write(&((so)->so_lock->sock_lock)); }
#define SOCKET_UNLOCK(so) \
	{ if (unlock_socheck(so) && sounlock(so)) so = 0; }


/* Sockbuf locks may be enabled for testing */
#define SOCKBUF_LOCKTEST	DEBUG

#if	SOCKBUF_LOCKTEST
#define SOCKBUF_LOCK(sb) \
	{ if (lock_sbcheck(sb)) lock_write((sb)->sb_lock); }
#define SOCKBUF_UNLOCK(sb) \
	{ if (unlock_sbcheck(sb)) lock_done((sb)->sb_lock); }
#define SOCKBUF_ISLOCKED(sb) \
	lock_islocked((sb)->sb_lock)
#else
#define SOCKBUF_LOCK(sb)
#define SOCKBUF_UNLOCK(sb)
#define SOCKBUF_ISLOCKED(sb)	1
#endif

#else	/* NETSYNC_LOCK && !NETSYNC_LOCKTEST */

#define SOCKET_LOCK(so)		lock_write(&((so)->so_lock->sock_lock))
#define SOCKET_UNLOCK(so)	(void)sounlock(so)
/* Sockbuf locks are disabled in non-debug kernel */
#define SOCKBUF_LOCKTEST	0
#define SOCKBUF_LOCK(sb)	/*lock_write((sb)->sb_lock)*/
#define SOCKBUF_UNLOCK(sb)	/*lock_done((sb)->sb_lock)*/
#define SOCKBUF_ISLOCKED(sb)	1 /*lock_islocked((sb)->sb_lock)*/

#endif

#else	/* !NETSYNC_LOCK */

struct  socklocks { int refcnt; };
#define SOCKET_LOCKINIT(so, lp)
#define SOCKET_ISLOCKED(so)	1
#define SOCKBUF_ISLOCKED(sb)	1
#define SOCKET_LOCK2(so1, so2)
#define SOCKET_LOCK(so)
#define SOCKET_UNLOCK2(so1, so2)
#define SOCKET_UNLOCK(so)
#define SOCKBUF_LOCKTEST	0
#define SOCKBUF_LOCK(sb)
#define SOCKBUF_UNLOCK(sb)

#endif	/* !NETSYNC_LOCK */

/* Bits for network events to sb_wakeup */
#define SE_ERROR	0x0001	/* so_error non-0 */
#define SE_HAVEDATA	0x0002	/* data in send or recv q */
#define SE_HAVEOOB	0x0004	/* oob data in recv q */
#define SE_DATAFULL	0x0008	/* send or recv q is full */
#define SE_CONNOUT	0x0010	/* outgoing connect complete (connect) */
#define SE_CONNIN	0x0020	/* incoming connect complete (listen)  */
#define SE_SENDCONN	0x0040	/* connected for send */
#define SE_RECVCONN	0x0080	/* connected for recv */
#define SE_POLL		0x4000	/* wakeup is synchronous poll */
#define SE_STATUS	0x8000	/* above status bits valid */

/* does socket have a valid u-area associated? (for stats, security, etc) */
#define SOHASUAREA(so)	(!((so)->so_special & SP_NOUAREA))

/* what is the domain associated with this socket? */
#define sodomain(so)	((so)->so_proto->pr_domain)

/*
 * How much space is there in a socket buffer (so->so_snd or so->so_rcv)?
 * This is problematical if the fields are unsigned, as the space might
 * still be negative (cc > hiwat or mbcnt > mbmax).  Should detect
 * overflow and return 0.  Should use "lmin" but it doesn't exist now.
 */
#define	sbspace(sb) \
    ((long) imin((int)((sb)->sb_hiwat - (sb)->sb_cc), \
	 (int)((sb)->sb_mbmax - (sb)->sb_mbcnt)))

/* do we have to send all at once on a socket? */
#define	sosendallatonce(so) \
    ((so)->so_proto->pr_flags & PR_ATOMIC)

/*
 * Is write of len bytes to be atomic (for pipes)? Since sb_hiwat
 * is adjusted by uipc_usrreq, it's not usable for this purpose,
 * so we look straight at syslimits.h with the danger of
 * PIPE_BUF > PIPSIZ.
 */
#define sowatomic(so, len) \
    (((so)->so_special & SP_WATOMIC) && ((len) <= PIPE_BUF))

/* can we read something from so? */
#define	soreadable(so) \
    ((so)->so_rcv.sb_cc >= (so)->so_rcv.sb_lowat || \
	((so)->so_state & SS_CANTRCVMORE) || \
	(so)->so_qlen || (so)->so_error)

/* can we write something to so? */
#define	sowriteable(so) \
    (sbspace(&(so)->so_snd) >= (so)->so_snd.sb_lowat && \
	(((so)->so_state&SS_ISCONNECTED) || \
	  ((so)->so_proto->pr_flags&PR_CONNREQUIRED)==0) || \
     ((so)->so_state & SS_CANTSENDMORE) || \
     (so)->so_error)

/* adjust counters in sb reflecting allocation of m */
#define	sballoc(sb, m) { \
	(sb)->sb_cc += (m)->m_len; \
	(sb)->sb_mbcnt += MSIZE; \
	if ((m)->m_flags & M_EXT) \
		(sb)->sb_mbcnt += (m)->m_ext.ext_size; \
}

/* adjust counters in sb reflecting freeing of m */
#define	sbfree(sb, m) { \
	(sb)->sb_cc -= (m)->m_len; \
	(sb)->sb_mbcnt -= MSIZE; \
	if ((m)->m_flags & M_EXT) \
		(sb)->sb_mbcnt -= (m)->m_ext.ext_size; \
}

#define	sorwakeup(so)	sowakeup((so), &(so)->so_rcv)
#define	sowwakeup(so)	sowakeup((so), &(so)->so_snd)

/* to catch callers missing new second argument to sonewconn: */
#define sonewconn(head, connstatus)	sonewsock((head), (connstatus))
extern u_long	sb_max;
#endif	/* _KERNEL */

#endif
