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
 * $Log: vsocket.h,v $
 * Revision 1.20  1995/03/03  19:13:00  toman
 *  Reviewer: Bob Yasi, John Litvin
 *  Risk: Medium (due to number of lines changed)
 *  Benefit or PTS #: 12508
 *  Testing: Tested syscall tracing with multiple netservers
 *  Module(s): server/uxkern/fsvr_subr.c
 *             server/vsocket/vs_ipc.c
 *             server/vsocket/vs_subr.c
 *             server/vsocket/vsocket.h
 *  Made new vsocket syscodes and passed them as parameters to
 *  start_vsockserver_op().  Also added corresponding names to a
 *  list of names in fsvr_thread_initialize(), which are displayed
 *  for extra syscodes (>= 2000) when syscall tracing is enabled.
 *  Also added parameter "serial" to start_vsockserver_op() and
 *  end_vsockserver_op(), but all vsocket operations currently pass
 *  0 for this param.
 *
 * Revision 1.19  1995/02/01  23:38:48  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.18  1994/11/18  20:52:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/07/28  22:19:54  yazz
 *  Reviewer: Mike Leibensperger
 *  Risk: Lo
 *  Benefit or PTS #: 10421
 *  Testing: EATs tcp-ip
 *  Module(s): server/vsocket/vs.defs, .../vs_subr.c, .../vs_ipc.c,
 *             .../vsocket.h
 * Defined VSDEBALLBITS & expanded bit definitions to 8 hex digits.
 *
 * Revision 1.16  1994/05/04  22:30:15  mjl
 * Merge revision 1.13.2.3 from R1_2 branch into main trunk.
 *
 * Revision 1.15  1994/03/17  18:50:04  nina
 *  Reviewer:hobbes
 *  Risk:Low
 *  Benefit or PTS #:8441
 *  Testing:EATS in various configurations
 *  Module(s):in ./server/vsocket: vsocket.h,
 * 	vs_subr.c, vs_netops.c
 *  Merge changes for #8441 from R1.2 in to R1.3
 *
 * Revision 1.13.2.3  1994/04/27  23:11:59  yazz
 *  Reviewer: Charlie Johnson, Bob Yasi
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
 * TNC select rewrite.  Can't have enough debug printf mask bits!
 *
 * Revision 1.13.2.2  1994/03/16  18:34:13  nina
 *  Reviewer:hobbes
 *  Risk:Low
 *  Benefit or PTS #:8441
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_subr.c,
 * 	vs_netops.c
 *  Added #define for VSDEBSOREADABLE
 *
 * Revision 1.14  1994/03/13  17:04:57  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:7294,6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket: vsocket.h, vs_netops.c,
 * 	vs_ipc.c, vs_subr.c, sys_vsocket.c
 *
 * Merge changes from R1.2
 *
 * Revision 1.13.2.1  1994/03/07  23:00:26  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:#7294/#6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket : vsocket.h, vs_ipc.c,
 * 	vs_netops.c, vs_subr.c
 *
 * Added VSDEBUG constants VSDEBFOS and VSDEBCST
 * as part of the work for #7294/#6927.
 *
 * Revision 1.13.2.1  1994/03/07  23:00:26  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:#7294/#6927
 *  Testing:EATS with various network configurations
 *  Module(s):in ./server/vsocket : vsocket.h, vs_ipc.c,
 * 	vs_netops.c, vs_subr.c
 *
 * Added VSDEBUG constants VSDEBFOS and VSDEBCST
 * as part of the work for #7294/#6927.
 *
 * Revision 1.13  1993/09/14  15:24:46  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.12.2.1  1993/09/14  15:23:18  cfj
 * Split the hold/release routines into network oriented and AF_UNIX oriented.
 * (These should be virtual socket operations.)  Fixed the debug output for
 * the VSOP macros.  Part of bug fix for PTS bug #6097.
 *
 * Revision 1.12  1993/09/01  01:42:26  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.11  1993/07/14  18:50:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.7  1993/07/01  21:15:17  cfj
 * Adding new code from vendor
 *
 * Revision 1.10  1993/05/06  19:34:50  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 3.28  93/08/23  00:27:32  mjl
 * Add virtual socket statistics structure.  Nothing too fancy yet....
 * 
 * Revision 3.27  93/08/11  09:17:04  bhk
 * Added VS_SLEEP_RECV Commands 
 * 
 * Revision 3.26  93/08/06  18:01:01  bhk
 * Aligned the lock state flag names to better reflect their meaning.
 * 
 * Revision 3.25  93/07/27  11:43:53  mjl
 * Make VSOCKET_DEBUG depend on MACH_ASSERT.
 * 
 * Revision 3.24  93/06/24  13:35:33  mjl
 * Add VSDEBNODEBIND debug flag to show node bindings of virtual sockets.
 * Fix parenthesization in VSDEBUG() macro so that a mask of debug flags can
 * be used.
 * 
 * Revision 3.23  93/04/23  07:54:34  roman
 * Correct RCS comments.
 * 
 * Revision 3.22  93/04/22  16:45:07  roman
 * Fix typo in comment.
 * 
 * Revision 3.21  93/04/16  11:34:40  bhk
 * fixed remote forwarding routines to unwrap in the
 * same order as the wrap
 * 
 * Revision 3.20  93/04/03  12:14:20  klh
 * Deleted VSOCKET_DEBUG, added VSDEBSELECT constant (klh for bhk).
 * 
 * Revision 3.19  93/03/19  19:53:26  bhk
 * master lock to the macro which is called before a remote operation.
 * server/vsocket/RCS/vs_netops.c,v  -->  server/vsocket/vs_netops.c
 * 
 * Revision 3.18  93/03/19  18:14:39  bhk
 * Added bitfields for the lock operation to be performed on the socket
 * for the remote vs_getstate call.
 * 
 * Revision 3.17  93/02/11  15:20:41  bhk
 * Added debug flags for monitoring credentials ports
 * 
 * Revision 3.16  93/02/10  21:09:57  klh
 * Additional checkin to sync up RCS comments
 * 
 * Revision 3.15  93/02/09  18:22:23  mjl
 * Added new debug flag for enabling clearinghouse debug printfs.
 * 
 * Revision 3.15  93/02/08  16:57:30  bhk
 * added a debug flag just to debug file port send rights
 * 
 * Revision 3.14  92/12/11  15:03:18  mjl
 * No more VSOP_VSOCK{CREATE,DESTROY}() virtual socket operations.  Get rid
 * of unused virtual op selector for datagrams.
 * 
 * Revision 3.13  92/12/10  17:39:46  mjl
 * VSOP_VSOCKDESTROY() no longer needs a vnode argument.
 * 
 * Revision 3.12  92/11/18  12:53:51  mjl
 * Add virtual operation selector for Unix datagram sockets.
 * 
 * Revision 3.11  92/10/27  18:02:44  bhk
 * Removed vsop_usrreq operation, replaced it with vsop_getaddr.
 * Added vsop_reserve operation.
 * 
 * Revision 3.10  92/09/24  17:21:56  bhk
 * *** empty log message ***
 * 
 * Revision 3.9  92/08/17  13:46:01  mjl
 * Got rid of extraneous arg to VSOP_SLEEP().
 * 
 * Revision 3.8  92/08/08  02:01:52  jdh
 * added 3 new vsops -- jdh
 * 
 * Revision 3.7  92/07/26  17:58:59  bhk
 * cleaned up debug
 * 
 * Revision 3.6  92/06/26  18:05:08  mjl
 * Add #defines for AF_UNIX SOCK_STREAM protocol numbers (i.e. virt. sock. ops).
 * 
 * Revision 3.5  92/06/16  18:51:32  bhk
 * added debuging flags
 * 
 * Revision 3.4  92/05/06  16:16:27  bhk
 * added sleep virtual socket operation
 * 
 * Revision 3.3  92/04/22  17:34:41  bhk
 * Added protocol usrreq virtual socket operation for getsockname
 * 
 * Revision 3.2  92/04/20  17:32:54  bhk
 * Added a few debugging flag values
 * 
 * Revision 3.1  92/04/06  11:39:09  mjl
 * Fixed arg count on DECREMENT_VS_REFCNT() macro and vsocket_release().
 * Added missing VSOP_CONNECT2() macro.  Added new VSOP_RELOCATE() virtual
 * socket op.
 * 
 * Revision 3.0  92/03/20  14:42:25  bhk
 * Moved back to vsocket directory
 * 
 * Revision 3.1  92/03/18  17:51:09  mjl
 * Now vsocket_release() takes a func pointer; the function is called on the
 * final release operation.  VSOP_CREATE() now takes a single socket arg, the
 * socket for which vsocket layer creation-time hooks are to be run.  There
 * is no need for VSOP_IOCTL().  Added VSOP_NOTIFY(), which handles migration
 * notification at the storage site.
 * 
 * Revision 3.0  92/03/04  14:48:37  bhk
 * Genesis	bhk
 * 
 * Revision 1.1  92/01/07  14:55:19  bhk
 * Initial revision
 */

#ifdef TNC

#ifndef _VSOCKET_H_
#define _VSOCKET_H_

#include <mach_assert.h>
#if	MACH_ASSERT
#define VSOCKET_DEBUG	1
#endif

#ifdef VSOCKET_DEBUG

extern int vsdebug;

#define	DECREMENT_VSNET_REFCNT(z, str) 		\
	(vsocket_db4(z, "RELEASE", str, -1), 	\
	vs_netrelease(z))

#define	INCREMENT_VSNET_REFCNT(z, str)		\
	(vsocket_db4(z, "HOLD", str, +1), 	\
	vs_nethold(z))

#define	DECREMENT_VS_REFCNT(z, str) 		\
	(vsocket_db4(z, "RELEASE", str, -1), 	\
	vsocket_release(z))

#define	INCREMENT_VS_REFCNT(z, str)		\
	(vsocket_db4(z, "HOLD", str, +1), 	\
	vsocket_hold(z))

/*
 * debuging flags for vsdebug
 */
#define	VSDEBENTRY	0x00000001
#define	VSDEBOPS	0x00000002
#define	VSDEBRET	0x00000004
#define	VSDEBREFCNT	0x00000008
#define	VSDEBCONTROL	0x00000010
#define	VSDEBERROR	0x00000020
#define VSDEBMISC	0x00000040
#define	VSDEBIO		0x00000080
#define	VSDEBPORT	0x00000100
#define	VSDEBFILEPORT	0x00000200
#define	VSDEBCREDPORT	0x00000400
#define VSDEBCLRHOUSE	0x00000800
#define VSDEBSELECT	0x00001000
#define VSDEBNODEBIND	0x00002000
#define VSDEBCST	0x00004000	/* Collapse state info */
#define VSDEBFOS	0x00008000	/* trace find_one_server() */
#define VSDEBSOREADABLE 0x00010000	/* trace vs_soreadable() */
#define VSDEBSELFREF	0x00020000	/* select(2) file ref debugging */
#define VSDEBALLBITS	0xffffffff	/* everything */

#define	VSDEBUG(mask, output) \
	if (vsdebug & (mask)) \
		printf output

#define VSDEBUGX(mask, action)	if (vsdebug & (mask)) { action ; }

#else	/* ! VSOCKET_DEBUG */

#define	DECREMENT_VSNET_REFCNT(z, str)	vs_netrelease(z)
#define	INCREMENT_VSNET_REFCNT(z, str)	vs_nethold(z)
#define	DECREMENT_VS_REFCNT(z, str)	vsocket_release(z)
#define	INCREMENT_VS_REFCNT(z, str)	vsocket_hold(z)
#define VSDEBUG(mask, output)
#define VSDEBUGX(mask, action)

#endif /* ! VSOCKET_DEBUG */

#define	VSOCKET_HOLD(v, str)	INCREMENT_VS_REFCNT((v), (str))
#define	VSOCKET_RELEASE(v, str)	DECREMENT_VS_REFCNT((v), (str))


#define	IN
#define OUT
#define INOUT

/*
 * The following structure defines the virutal socket operations table.
 */
struct vsocket_ops {
	int (*vsop_create) (
		INOUT struct socket *vs);	/* vsocket to create auxilliaries for */
	int (*vsop_close) (
		IN struct socket  *vs);		/* vsocket to close */
	int (*vsop_send) (
		IN struct socket  *vs,		/* vsocket to send on */
		IN struct mbuf *nam,		/* name to send to */
		IN struct uio *uio,		/* data to send */
		IN struct mbuf *top,		/* top of mbuf chain (no uio) */
		IN struct mbuf *control,	/* control message */
		IN int flags);			/* flags */
	int (*vsop_receive) (
		IN struct socket  *vs,		/* vsocket to receive on */
		OUT struct mbuf **nam,		/* name received from  */
		OUT struct uio *uio,		/* data to send */
		OUT struct mbuf **topp,		/* top of mbuf chain (no uio) */
		OUT struct mbuf **controlp,	/* control message */
		OUT int *flagsp);		/* flags */
	int (*vsop_dequeue) (
		IN struct socket  *vs,		/* vsocket for accept */
		OUT struct socket **avs,	/* new accepted socket */
		OUT struct mbuf **nam,		/* address of remote */
		IN int compat);			/* BSD compatibility */
	int (*vsop_listen) (
		IN struct socket  *vs,		/* vsocket to listen on */
		IN int backlog);		/* how many connections to queue */
	int (*vsop_connect) (
		struct socket  *vs,			/* vsocket to connect */
		struct mbuf *nam);		/* who to connect to */
	int (*vsop_connect2) (
		IN struct socket  *vs1,		/* half or socket pair to connect */
		IN struct socket  *vs2);		/* other half of socket pair */
	int (*vsop_bind) (
		IN struct socket  *vs,		/* vsocket to bind */
		IN struct mbuf *nam);		/* name to bind to */
	int (*vsop_shutdown) (
		IN struct socket  *vs,		/* vsocket to shut down */
		IN int how );			/* how to shut it down */
	int (*vsop_setopt) (
		IN struct socket  *vs,		/* vsocket to set option on */
		IN int level,			/* protocol for option */
		IN int optname,			/* name of option */
		IN struct mbuf *m0 );		/* value to set option to */
	int (*vsop_getopt) (
		IN struct socket  *vs,		/* vsocket to get option on */
		IN int level,			/* protocol for option */
		IN int optname,			/* name of option */
		OUT struct mbuf **m0 );		/* value of option */
	int (*vsop_notify) (
		IN struct socket *vs,		/* vsocket to be notified */
		IN pid_t pid,			/* pid that migrated */
		IN int new_node);		/* pid's new execution node */
	int (*vsop_relocate) (
		IN struct socket *vs);		/* attempt to "relocate" this*/
	int (*vsop_getaddr) (
		IN struct socket *vs,		/* vsocket for address */
		OUT struct mbuf **nam,		/* where to put address */
		IN int which,			/* local or remote ? */
		IN int compat_43);		/* BSD 4.3 compatibility */
	int (*vsop_sleep) (
		IN struct socket *vs,		/* vsocket to sleep */
		IN caddr_t	 addr,		/* what to sleep on */
		IN int	pri,			/* priority */
		IN int	tmo);			/* timeout */
	int (*vsop_vsockconnect) (
		IN struct socket *vs,		/* connecting vsocket */
		IN mach_port_t sockid);		/* listening vsocket port */
	int (*vsop_reserve) (
		IN struct socket *vs,		/* vsocket to reserve space */
		IN u_long sendcc,		/* send count */
		IN u_long recvcc);		/* receive count */
};

typedef struct vsocket_ops vsocket_ops_t;

/*
 * commands for the vs_sosleep call
 */
#define	VS_SLEEP_TIMEO			0x001
#define	VS_SLEEP_RECV			0x002

#define	VS_SLEEP_CANCEL(x)		(x << 8)
#define VS_SLEEP_TIMEO_CANCELLED	(VS_SLEEP_CANCEL(VS_SLEEP_TIMEO))
#define	VS_SLEEP_RECV_CANCELLED		(VS_SLEEP_CANCEL(VS_SLEEP_RECV))


/*
 * lock states for vs_getstate()  and vs_soreadable call
 */
#define	VSSTATE_LOCK	1
#define	VSSTATE_UNLOCK	2
/*
 * The following macros are used to invoke the vsocket operations:
 */
#define VSOP_CREATE(vs) \
	_VSOP_(vsop_create, (vs), (vs), \
		"(vs=0x%x)")
#define VSOP_CLOSE(vs) \
	_VSOP_(vsop_close, vs, (vs), \
		"(0x%x)")
#define VSOP_SEND(vs,nam,uio,top,control,flags) \
	_VSOP_(vsop_send, vs, (vs,nam,uio,top,control,flags), \
		"(0x%x nam=0x%x uio=0x%x top=0x%x control=0x%x flags=0x%x)")
#define VSOP_RECEIVE(vs, nam, uio, topp, contolp, flagsp) \
	_VSOP_(vsop_receive, vs, (vs, nam, uio, topp, contolp, flagsp), \
		"(0x%x, nam=0x%x uio=0x%x topp=0x%x controlp=0x%x flagsp=0x%x)")
#define VSOP_DEQUEUE(vs, avs, nam, compat) \
	_VSOP_(vsop_dequeue, vs, (vs, avs, nam, compat), \
		"(0x%x, avs=0x%x, &nam=0x%x, compat=%d)")
#define VSOP_LISTEN(vs, backlog) \
	_VSOP_(vsop_listen, vs, (vs, backlog), \
		"(0x%x, backlog=%d)")
#define VSOP_CONNECT(vs, nam) \
	_VSOP_(vsop_connect, vs, (vs, nam),\
		"(0x%x, &nam=0x%x)")
#define VSOP_CONNECT2(vs1, vs2) \
	_VSOP_(vsop_connect2, vs1, (vs1, vs2),\
		"(0x%x, 0x%x)")
#define VSOP_BIND(vs, nam) \
	_VSOP_(vsop_bind, vs, (vs, nam), \
		"(0x%x, &nam=%x)")
#define VSOP_SHUTDOWN(vs, how) \
	_VSOP_(vsop_shutdown, vs, (vs, how), \
		"(0x%x, how=%d)")
#define	VSOP_SETOPT(vs, level, optname, m0) \
	_VSOP_(vsop_setopt, vs, (vs, level, optname, m0), \
		"(0x%x, level=%d option=0x%x &m0=0x%x)")
#define	VSOP_GETOPT(vs, level, optname, m0) \
	_VSOP_(vsop_getopt, vs, (vs, level, optname, m0), \
		"(0x%x, level=%d option=0x%x &m0=0x%x)")
#define	VSOP_NOTIFY(vs, pid, node) \
	_VSOP_(vsop_notify, vs, (vs, pid, node), \
		"(0x%x, pid=%d node=%d)")
#define       VSOP_RELOCATE(vs) \
	_VSOP_(vsop_relocate, vs, (vs), \
		"(vs=0x%x)")
#define VSOP_SLEEP(vs, addr, pri, tmo) \
	_VSOP_(vsop_sleep, vs, (vs, addr, pri, tmo), \
		"(0x%x, addr=0x%x, pri=%d tmo=%d)")
#define VSOP_VSOCKCREATE(vs, vp) \
	_VSOP_(vsop_vsockcreate, vs, (vs, vp), \
		"(0x%x, 0x%x)")
#define VSOP_VSOCKCONNECT(vs, sockid) \
	_VSOP_(vsop_vsockconnect, vs, (vs, sockid),\
		"(0x%x, 0x%x)")
#define VSOP_VSOCKDESTROY(vs) \
	_VSOP_(vsop_vsockdestroy, vs, (vs), \
		"(vs=0x%x)")
#define VSOP_RESERVE(vs,sendcc,recvcc) \
	_VSOP_(vsop_reserve, vs, (vs, sendcc, recvcc) , \
		"(0x%x %d %d)")
#define VSOP_GETADDR(vs,nam,which,compat43) \
	_VSOP_(vsop_getaddr, vs, (vs, nam, which, compat43) , \
		"(0x%x 0x%x 0x%x 0x%x)")
/*
 * To debug vsocket ops or not, that is the question...
 */
#ifdef	VSOCKET_DEBUG
#define _VSOP_(op, v, args, argfmt) \
	(vsocket_db1(v,#op,argfmt), vsocket_db2 args, \
	((v)->vs_ops->op)? vsocket_db3((*(v)->vs_ops->op) args):panic("Bad vsocketop op\n")) 
#else	/* VSOCKET_DEBUG */
#define _VSOP_(op, v, args, argfmt)	(*(v)->vs_ops->op) args
#endif	/* VSOCKET_DEBUG */

/*
 *  Virtual operation selectors for AF_UNIX SOCK_STREAM
 */
#define VOS_NORMAL	0	/* No TNC storage site migration */
#define VOS_PIPE	1	/* Use pipe virtual ops */
#define VOS_SOCK	2	/* Use bind/connect stream virtual ops */
#define VOS_FIFO	3	/* Use FIFO virtual ops */


#define	VSOP_SET_FORW(oipp,port)	\
MACRO_BEGIN				\
	OIP_SET_FORW(oipp,port);	\
	ux_server_thread_blocking();	\
MACRO_END

#define	VSOP_END_FORW(oipp)		\
MACRO_BEGIN				\
	ux_server_thread_unblocking();	\
	OIP_END_FORW(oipp);		\
MACRO_END

/*
 *  Virtual socket statistics
 */
extern struct vs_stats {
	int	vss_optsallocated;
	int	vss_optsfreed;
} vsstats;

#define	VSSTAT_LOCK
#define VSSTAT_UNLOCK

/*
 * Extra syscodes for tracing vsocket server ops.
 */
#define SYS_VSOCK_socreate	2015
#define SYS_VSOCK_soclose	2016
#define SYS_VSOCK_soconnect	2017
#define SYS_VSOCK_sosetopt	2018
#define SYS_VSOCK_sogetopt	2019
#define SYS_VSOCK_sel_check	2020
#define SYS_VSOCK_sel_enqueue	2021
#define SYS_VSOCK_sel_dequeue	2022
#define SYS_VSOCK_scrub_rem_sel	2023	
#define SYS_VSOCK_sobind	2024
#define SYS_VSOCK_sosend_short	2025
#define SYS_VSOCK_sosend_long	2026
#define SYS_VSOCK_sorecv	2027
#define SYS_VSOCK_solisten	2028
#define SYS_VSOCK_sodequeue	2029
#define SYS_VSOCK_ioctl		2030
#define SYS_VSOCK_global_ioctl	2031
#define SYS_VSOCK_sogetaddr	2032
#define SYS_VSOCK_getstate_imm	2033
#define SYS_VSOCK_getstate_del	2034
#define SYS_VSOCK_soreadable	2035

#endif  /* _VSOCKET_H_ */
#endif /* TNC */
