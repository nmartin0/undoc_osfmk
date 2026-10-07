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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: un_dg_vsops.c,v $
 * Revision 1.6  1995/02/01  22:04:03  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:44:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/01  01:37:22  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.3  1993/07/14  18:35:53  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1993/05/06  19:27:32  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:55  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.4  93/08/26  10:47:31  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.
 * 
 * Revision 3.3  93/08/23  00:07:16  mjl
 * Get rid of i860 compiler warnings.
 * 
 * Revision 3.2  93/06/14  14:01:30  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Fix f_count after the socket has been relocated
 * 
 * Revision 3.1  92/12/11  14:57:15  mjl
 * No more VSOP_VSOCK{CREATE,DESTROY}() virtual socket operations.  Fix comment.
 *
 * Revision 3.0  92/12/10  17:00:16  mjl
 * Virtual socket operations for TNC Unix domain datagram sockets.
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <vsocket/vs_types.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/protosw.h>
#include <sys/user.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/un.h>

#include <tnc/un.h>
#include <tnc/un_dg.h>

#ifdef	UN_DG_DEBUG
int	undgdebug	= 0;
#endif

/*
 *  TNC virtual socket operations for UNIX domain SOCK_DGRAM sockets.
 *
 *  XXX Revise this for new shadowless scheme.
 *
 *  Communication between UNIX domain datagram sockets in non-TNC
 *  systems is accomplished by temporarily making a connection between
 *  the sockets using pointers in system address space.  In a NoRMA
 *  environment sockets will reside on different nodes, and so
 *  temporary connections to a receiving socket must use a surrogate
 *  socket to represent the sender.  The virtual socket operations
 *  below maintain a "shadow" socket for each UNIX datagram socket
 *  created.  The shadow socket acts as the surrogate for any socket
 *  sending data to the shadow's primary socket.  This allows maximum
 *  reuse of existing protocol code.  Also, surrogate socket
 *  allocation is done once when the primary socket is created, and
 *  not for each operation.
 *
 *  A receiving UNIX domain datagram socket maintains a list of all
 *  sockets connecting to it, so that ECONNRESET errors can be posted
 *  when the receiving socket goes away.  (Recall that in the UNIX
 *  domain datagram world, "connection" is a one-way, many-to-one
 *  relationship, with many sending sockets connected to a single
 *  receiving socket that may in turn be connected to yet another
 *  socket.  See <sys/unpcb.h>.)  In OSF/1AD+TNC, connection is
 *  implemented using a Mach port rather than a pointer, so dead-name
 *  notification can be used to inform senders that the connection has
 *  been reset.  No list of connecting sockets need be maintained.
 */


/*
 *  Unix datagram socket virtual operations table.
 */

extern int vs_badop();
extern int un_dg_socreate(struct socket *);
extern int un_dg_soclose(struct socket *);
extern int un_sosend();
extern int un_sorecv();
extern int un_dg_sodequeue(struct socket *, struct socket **,
			   struct mbuf **, int);
extern int un_solisten();
extern int un_soconnect();
extern int un_dg_soconnect2(struct socket *, struct socket *);
extern int un_sobind();
extern int un_soshutdown();
extern int un_sosetopt();
extern int un_sogetopt();
extern int un_notify();
extern int un_dg_relocate();
extern int un_dg_sogetaddr(struct socket *, struct mbuf **, int, int);
extern int un_sosleep();
extern int un_dg_vsockconnect(struct socket *, mach_port_t);
extern int soreserve();

struct vsocket_ops un_dg_vsops = {
	un_dg_socreate,
	un_dg_soclose,
	un_sosend,
	un_sorecv,
	un_dg_sodequeue,
	un_solisten,
	un_soconnect,
	un_dg_soconnect2,
	un_sobind,
	un_soshutdown,
	un_sosetopt,
	un_sogetopt,
	un_notify,
	un_dg_relocate,
	un_dg_sogetaddr,
	un_sosleep,
	un_dg_vsockconnect,
	soreserve
};


/*
 *  TNC virtual socket "constructor" for a UNIX datagram socket.
 */
int 
un_dg_socreate(
	struct socket	*vs)
{
	struct uthread	*uth	= current_thread();
	int		error	= ESUCCESS;
	struct unpcb	*unp	= (struct unpcb *)vs->so_pcb;
	policy_state_t	*ps;

	SOCKET_LOCK(vs);
	INCREMENT_VS_REFCNT(vs,"un_dg_socreate:");

	/*
	 *  Initialize per-socket TNC information
	 */
	ASSERT(uth->uu_opn_filep == MACH_PORT_NULL ||
	       ((struct file *)uth->uu_opn_filep)->f_magic == F_MAGIC);
	vs->vs_fport = uth->uu_opn_filep;
	vs->vs_flags = VS_INITIALIZED;
	vs->vs_data = NULL;

	vs->vs_bindport = MACH_PORT_NULL;
	mutex_init(&vs->vs_seqno_mutex);

	/*
	 *  Allocate and initialize datagram-specific data.
	 */
	ps = (policy_state_t *)malloc(sizeof(policy_state_t));
	un_policy_init(ps);
	vs->vs_data = (caddr_t)ps;

	ASSERT(unp && unp->unp_conn == 0);

	SOCKET_UNLOCK(vs);
	return (error);
}


/*
 *  TNC virtual socket "destructor" for a UNIX datagram socket.
 *  Called only on last close.
 */
int
un_dg_soclose(
	struct socket	*vs)		/* vsocket to close */
{
	kern_return_t	kr;
	int		error	= ESUCCESS;
	struct unpcb	*unp	= (struct unpcb *)vs->so_pcb;
	mach_port_t	connport;

	ENTER_UN_VSOP(vs, KEEPLOCK, error);
	if (error) {
		SOCKET_UNLOCK(vs);
		LEAVE_UN_VSOP(vs);
		return(error);
	}

	/*
	 *  Deallocate datagram-specific data area.
	 */
	ASSERT(vs->vs_data);
	(void) free(vs->vs_data);

	SOCKET_UNLOCK(vs);

	/* This calls soclose() when vs_refcnt goes to zero. */
	DECREMENT_VS_REFCNT(vs,"un_dg_soclose:");

	ASSERT(vs->vs_bindport == NULL);

	LEAVE_UN_VSOP(vs);  /* XXX Currently a no-op.  Socket may be gone! */
	return(error);
}


int
un_dg_sodequeue(
	struct socket	*head,
	struct socket	**so,
	struct mbuf	**nam,
	int		compat_43)
{
	panic("un_dg_sodequeue\n");
}


/*
 *  Connect two Unix domain datagram sockets, using the sure knowledge
 *  that they are located on the same node.  NB connection is asymmetrical;
 *  we are connecting s1 to s2 but not vice versa.
 */
int
un_dg_soconnect2(
	struct socket	*s1,		/* half of socket pair to connect */
	struct socket	*s2)		/* other half of socket pair */
{
	kern_return_t	kr;
	struct unpcb	*unp;
	mach_port_t	sockid;
	int		error	= ESUCCESS;

	ENTER_UN_VSOP(s1,DISCARDLOCK,error);
	if (error)
		goto out;

	SOCKET_LOCK2(s1, s2);
	if (s1->so_type != s2->so_type) {
		SOCKET_UNLOCK2(s1, s2);
		error = EPROTOTYPE;
		goto out;
	}

	/*
	 *  First create a bind port for s2, even though no bind(2)
	 *  call was made on it.  This code looks a lot like that in
	 *  create_socket_bind_port(), except we don't create a VSOCK
	 *  vnode.
	 *
	 *  XXX Potentially this will cause problems if bind(s2, ...)
	 *  is called later, as that code may expect unp_vnodeport to
	 *  be set, and here we don't do anything with vnodes.  Oh well....
	 */
	ASSERT(s2->vs_bindport == MACH_PORT_NULL);
	kr = mach_port_allocate(mach_task_self(),
				MACH_PORT_RIGHT_RECEIVE, &sockid);
	if (kr != KERN_SUCCESS)
		panic("un_dg_soconnect2: _allocate: kr 0x%x\n", kr);
	kr = mach_port_insert_right(mach_task_self(),
				    sockid, sockid,
				    MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
		panic("un_dg_soconnect2: _insert_right: kr 0x%x\n", kr);

	/*
	 *  Make s1 hold s2's bind port send right!
	 */
	unp = (struct unpcb *)s1->so_pcb;
	ASSERT(unp && unp->unp_conn == 0);
	s2->vs_bindport = sockid;
	unp->unp_conn = (struct unpcb *)sockid;

	/*
	 *  Enter the <port,socket_addr> pair into the map, so that
	 *  incoming IPC on the socket port can locate the socket.
	 *  Then begin service for the port.  NB a port can be in
	 *  service iff it is in the map!
	 */
	MAP_PORT_TO_SOCKET(sockid, s2);
	ux_server_add_port(sockid);

	/*
	 *  Why don't you do that voodoo that PRU_CONNECT2 do so well!
	 *  (Here we do whatever else a PRU_CONNECT2 would do... :-)
	 */
	soisconnected(s1);

	SOCKET_UNLOCK2(s1, s2);
out:
	LEAVE_UN_VSOP(s1);
	return(error);
}


/*
 *  Attempt to relocate a Unix datagram socket.  Returns TRUE iff the
 *  relocation succeeded (and thus the caller does not need to do any
 *  socket or file struct unlocking).
 *
 *  This virtual socket operation is (potentially) called from our
 *  TNC hook routine executed at the end of each fileserver op.
 *
 *  Both the SOCKET_LOCK(so) and FP_LOCK(so->vs_fport) are already
 *  held upon entry.  If the relocation fails, they should still be
 *  be held upon return.  If it succeeds, then we deallocate both
 *  sockets and file structs here, so the caller should not try
 *  to unlock.
 */
int
un_dg_relocate(so)
	struct socket *so;
{
	struct file	*fp;
	int		error;

	LOCK_ASSERT("un_relocate", SOCKET_ISLOCKED(so));
	ASSERT(IS_RELOCATING(so));

	fp  = (struct file *)so->vs_fport;

	/*
	 * Check the socket for outstanding active threads.  Proceed
	 * only if all operations have finished.  (When the seqno in
	 * the socket == the seqno in the Mach port, this means all
	 * operations are complete.)  At this point, the current
	 * thread must be the last thread on the socket; it has
	 * already completed the operation it was performing and has
	 * incremented seqno to indicate this.
	 */
	if ( seqno_from_port(fp) == fp->f_seqno )
		so->vs_flags |= VS_NOTHREADS;
	if ( ! (so->vs_flags & VS_NOTHREADS) )
		/*
		 *  Threads still active on this socket.
		 *  Cannot relocate yet.
		 */
		return FALSE;

	/*
	 *  The current thread is the last active thread.
	 *  Attempt to relocate.
	 */
	SOCKET_UNLOCK(so);
	FP_UNLOCK(fp);

	error = un_dg_really_relocate(so);
	if ( error != ESUCCESS )
		goto reattach;

	/*
	 *  The socket and its file struct have been
	 *  successfully cloned on the new node.  Now we have to close
	 *  and deallocate the originals.  This happens as a side
	 *  effect of the final FP_UNREF() on the file pointer.
	 *
	 *  This is the last thread that holds any reference to the
	 *  "old node" file struct and socket.  When this thread
	 *  completes, FILE_LOOKUP_DONE() in end_fileserver_op() will
	 *  expect to call FP_UNREF() on the file pointer, so we must
	 *  leave *fp with a reference count of one.
	 *
	 *  Before any FP_UNREF()ing can be done, however, we have to
	 *  set f_magic to F_RELOC.  This prevents fdealloc() from
	 *  trying to deallocate the file port, which was just
	 *  migrated.  Also, we must clear the VS_RESTART flag, since
	 *  otherwise no virtual socket operations can affect this
	 *  socket (e.g. VSOP_CLOSE()).
	 */
	fp->f_magic = F_RELOC;
	so->vs_flags &= ~VS_RESTART;
	fp->f_count = 1; /* gets closed by end_fileserver_op */
	return TRUE;

reattach:
	/*
	 *  For whatever reason, the relocation failed.  Since we're
	 *  the last thread that knows anything about this socket and
	 *  its file structure, we'd better reattach it to the server
	 *  port set.
	 *
	 *  Our caller expects these structures to be either locked or
	 *  gone, so we relock.
	 *
	 *  Also, got to reset vs_flags relocation state bits.
	 */
	UNDGDEBUG(U_RELOC,("un_dg_relocate: attempt got errno %d\n", error));
#ifdef	UN_DEBUG
	if (undebug & U_SETERROR)
		so->so_error = error;
#endif	/* UN_DEBUG */

	FP_LOCK((struct file *)(so->vs_fport));
	SOCKET_LOCK(so);

	so->vs_flags = VS_INITIALIZED;
	ux_server_add_port(so->vs_fport);
	if( so->vs_bindport ) {
		MAP_PORT_TO_SOCKET(so->vs_bindport, so);
		ux_server_add_port((mach_port_t)so->vs_bindport);
	}

	return FALSE;
}


int
un_dg_sogetaddr(
	struct socket	*so,
	struct mbuf	**nam,
	int		which,
	int		compat_43)
{
	/*
	 *  If which == FALSE then this is a PRU_SOCKADDR and can be
	 *  done locally.
	 */
	if (!which)
		return (sogetaddr(so, nam, which, compat_43));

	/*
	 *  This is a PRU_PEERADDR, so we have to function ship to the
	 *  other side and do a PRU_SOCKADDR there.
	 */
	printf("un_dg_sogetaddr\n");	/* XXX */
	return ENOTCONN;
}


/*
 *  Save away the port leading to the socket we are now connected to.
 *  Unlike stream sockets, which use this port to relocate and then
 *  connect locally on the remote node, datagram sockets keep a send
 *  right for the other socket's bind port around for the duration of
 *  the connection, and use it to transmit data.
 */
int
un_dg_vsockconnect(
	struct socket	*so,		/* connecting socket */
	mach_port_t	connport)	/* bind port of socket to connect to */
{
	struct unpcb	*unp;
	kern_return_t	kr;

	SOCKET_LOCK(so);
	unp = sotounpcb(so);
	ASSERT(unp->unp_conn == 0);
	unp->unp_conn = (struct unpcb *)connport;
	/* unp_connect() wants us to leave the socket locked. */
	return (ESUCCESS);
}
