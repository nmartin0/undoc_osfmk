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
 * $Log: un_subr.c,v $
 * Revision 1.9  1995/02/01  22:11:47  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:45:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  18:36:44  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:50:43  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:29:24  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:33  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:10:17  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:07:07  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/22  15:46:52  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 1.3  1993/01/15  02:03:38  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1  1992/11/05  22:47:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.3  93/06/14  14:11:35  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Changed the socket marshal/unmarshal code to account for the fact that we
 * now send the file struct across during a relocation.
 * 
 * Revision 3.2  93/01/12  17:35:31  mjl
 * Return ECONNREFUSED if a Unix domain server app has died without removing
 * its address from the file system.
 *
 * Revision 3.1  92/12/10  17:30:49  mjl
 * Make sopartner() return NULL for all datagram sockets; we want the common
 * relocation code to relocate only one socket.  Added new routines,
 * and moved sgd_unp_socket_{append,extract}() routines here under new names.
 * They now also support marshalling/unmarshalling unp->unp_conn, which is a
 * Mach port in the datagram socket case.
 * 
 * Revision 3.0  92/08/08  01:24:23  jdh
 * un_pp_subr.c was renamed, as it's used for all Unix domain
 * sockets -- jdh
 * 
 * Revision 3.2  92/07/08  09:15:30  roman
 * Change node numbers to type node_t.
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.1  92/06/30  17:19:57  mjl
 * Reference count the sockinfo structure.
 * 
 * Revision 3.0  92/06/26  17:58:08  mjl
 * UNix PiPe support subroutines for relocation.
 * 
 * Revision 3.1  92/06/11  16:16:20  mjl
 * Removed stub for ust_really_relocate(), real routine now in ust_reloc.c.
 * Cleaned up ust_policy debug printfs.  Deleted stale debugging code.
 * 
 * Revision 3.0  92/04/06  10:39:31  mjl
 * Support subroutines for AF_UNIX stream socket virtual 
 * operations.
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/time.h>
#include <sys/vnode.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <net/net_malloc.h>
#include <kern/macro_help.h>

#include <tnc/un.h>
#include <tnc/vector.h>
#include <tnc/sgd.h>

/*
 *  Return a pointer to the socket's locally connected peer or NULL.
 *  The socket is assumed to be locked.
 */
struct socket *
sopartner(so)
	struct socket *so;
{
	struct unpcb *unp;
	struct socket *so2;

	/*
	 *  For TNC, a relocating socket may be "as good as locked"
	 *  without actually being locked.  IS_RELOCATING(so) is
	 *  necessary but not sufficient for being "as good as locked",
	 *  but it should keep this assertion from bombing well enough.
	 */
	ASSERT(SOCKET_ISLOCKED(so) || IS_RELOCATING(so));
	ASSERT(so->so_pcb != NULL);

	/*
	 *  For TNC Unix datagram sockets, unp_conn is a Mach port!
	 *  Even though the partner may indeed be local, we don't want
	 *  to return anything here because connected datagram sockets
	 *  don't relocate together---they can be on different nodes.
	 *  Therefore, tell the common un_notify() routine that there
	 *  is no second socket to move.
	 */
	if ( so->so_type == SOCK_DGRAM )
		return (NULL);

	unp = ((struct unpcb *)so->so_pcb)->unp_conn;
	so2 = ( unp ? unp->unp_socket : NULL );
	ASSERT(!unp || so2);
	if ( so2 ) {
		/*
		 *  Sanity check socket pair integrity.
		 */
		ASSERT(so->vs_data == so2->vs_data);
		ASSERT(so->so_lock == so2->so_lock);
	}
	return so2;
}


/*
 *  XXX Make me a macro!
 */
int
un_enter_vsop(so, keeplock)
	struct socket *so;
	int keeplock;
{
	int ret = 0;

	SOCKET_LOCK(so);
	if ( so->vs_flags & VS_RESTART )
		ret = ERESTART;
	if ( ! keeplock )
		SOCKET_UNLOCK(so);
	return(ret);
}


/*
 *  XXX Make me a macro!
 */
int
un_leave_vsop(so)
	struct socket *so;
{
#ifdef	NOTDEF
	SOCKET_LOCK(so);
	SOINCRSEQ(so);
	SOCKET_UNLOCK(so);
#endif
	return 0;
}


int
un_alloc_sockinfo(so)
	struct socket *so;
{
	struct socket *so2;
	un_sock_t *si;

	LOCK_ASSERT("un_alloc_sockinfo", SOCKET_ISLOCKED(so));

	if ( (so2 = sopartner(so)) == NULL )
		panic("un_alloc_sockinfo: no partner");
	NET_MALLOC(so->vs_data, caddr_t, sizeof(un_sock_t), M_TEMP, M_WAITOK);
	so2->vs_data = so->vs_data;

	si = (un_sock_t *)so->vs_data;
	si->si_reader = 0;
	si->si_rdrnode = INVALID_NODE;
	si->si_newnode = INVALID_NODE;
	si->si_refcnt = 2;

#ifdef	UN_DEBUG
	un_si_alloced++;
#endif

#ifdef	USE_MBUFS
	ASSERT(sizeof(un_sock_t) < MLEN);
	And do all sorts of other mbuf-ish things...
#endif
}


int
un_free_sockinfo(so)
	struct socket *so;
{
	struct socket *so2;
	un_sock_t *si;

	LOCK_ASSERT("un_free_sockinfo", SOCKET_ISLOCKED(so));
	si = (un_sock_t *)so->vs_data;
	ASSERT(si && si->si_refcnt > 0);
	if ( --si->si_refcnt == 0 ) {
		NET_FREE(si, M_TEMP);
		so->vs_data = NULL;
#ifdef	UN_DEBUG
		un_si_freed++;
#endif
	}
}



/*
 *  Wake up everybody and anybody who could possibly be waiting on
 *  any channel associated with this socket.
 */
int
un_wakeall(so)
	struct socket *so;
{
	LOCK_ASSERT("un_wakeall", SOCKET_ISLOCKED(so));
	ASSERT(so->so_rcv.sb_wakeup == NULL && so->so_snd.sb_wakeup == NULL);

	wakeup((caddr_t)&so->so_timeo);

	wakeup((caddr_t)&so->so_snd.sb_flags);
	wakeup((caddr_t)&so->so_snd.sb_cc);
/*	sbwakeup(0, &so->so_snd, 1); /* XXX */

	wakeup((caddr_t)&so->so_rcv.sb_flags);
	wakeup((caddr_t)&so->so_rcv.sb_cc);
/*	sbwakeup(0, &so->so_rcv, 1); /* XXX */
}


/*
 *  In TNC, value stored in a type VSOCK vnode is a send right
 *  for a socket's bind port.  We want to make a new send right
 *  to pass back to VSOP_VSOCKCONNECT() or ???.   XXX
 */
int
get_bind_port_from_vnode(
	struct vnode	*vp,	/* IN  - vnode holding bind port send right */
	mach_port_t	*portp)	/* OUT - bind port send right, +1 uref */
{
	kern_return_t	kr;
	mach_port_t	sockid;

	ASSERT(vp->v_type == VSOCK);

	*portp = MACH_PORT_NULL;	/* Neatness counts! */

	/*
	 *  If the socket port is gone, the server app must have died.
	 */
	sockid = (mach_port_t)vp->v_socket;
	if (sockid == MACH_PORT_NULL)
		return (ECONNREFUSED);

	/*
	 *  Increase the send right's user reference before returning it.
	 */
	kr = mach_port_mod_refs(mach_task_self(), sockid,
				MACH_PORT_RIGHT_SEND, +1);
	if (kr != KERN_SUCCESS) {
		/*
		 * When does this return an error?  One definite
		 * possibility is that the send right has turned into
		 * a dead name, because the other side has destroyed
		 * the receive rights for this port.  In any case,
		 * this is no longer a valid send right.
		 * 
		 * Assume that sockid is now a dead name, and 
		 * allow this dead name to be deallocated in the
		 * exit code below.
		 *
		 * Return an ENOTSOCK error.
		 */
		UNDEBUG(U_BINDPORT,
			("get_bind_port_from_vnode: kr 0x%x\n", kr));
		vp->v_socket = (struct socket *)MACH_PORT_NULL;
		return (ENOTSOCK);
	}

	UNDEBUG(U_PORT,("Got bind port 0x%x from vnode 0x%x.\n", sockid, vp));

	*portp = sockid;
	return (ESUCCESS);
}


/*
 *  Create a socket port for the socket, with one send right.
 *  Enter the <port,socket> pair in the port-to-socket map.
 *  Called from unp_bind() to obtain port to be stored in a
 *  type VSOCK vnode (the "bind port").
 */
void
create_socket_bind_port(
	struct socket	*so,	/* IN  - socket needing a bind port */
	mach_port_t	*portp)	/* OUT - resulting bind port */
{
	kern_return_t	kr;
	int		error;
	mach_port_t	bindport;

	LOCK_ASSERT("create_socket_bind_port", !SOCKET_ISLOCKED(so));
	SOCKET_LOCK(so);

	/* Should only call this for unbound sockets. */
	ASSERT(so->vs_bindport == MACH_PORT_NULL);

	*portp = MACH_PORT_NULL;

	/* Create the port. */
	kr = mach_port_allocate(mach_task_self(),
				MACH_PORT_RIGHT_RECEIVE,
				&bindport);
	if (kr != KERN_SUCCESS)
		panic("create_socket_bind_port: port allocate: kr 0x%x\n", kr);
	UNDEBUG(U_BINDPORT, ("create_socket_bind_port: so 0x%x rright 0x%x\n",
			     so, bindport));

	/* Make one send right. */
	kr = mach_port_insert_right(mach_task_self(),
				    bindport, bindport,
				    MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
	    panic("create_socket_bind_port: mach_port_insert_right: kr 0x%x\n",
		  kr);
	UNDEBUG(U_BINDPORT,("create_socket_bind_port: so 0x%x sright 0x%x\n",
			    so, bindport));

	/* Port creation complete! */
	*portp = so->vs_bindport = bindport;

	/*
	 *  Enter the <port,socket_addr> pair into the map, so that
	 *  incoming IPC on the socket port can locate the socket.
	 *  Then begin service for the port.  NB a port can be in
	 *  service iff it is in the map!
	 */
	MAP_PORT_TO_SOCKET(bindport, so);
	ux_server_add_port(bindport);

	SOCKET_UNLOCK(so);
}


/*
 *  Destroy a socket's bind port, removing it from the
 *  port-to-socket map.  Called from unp_bind() in case
 *  of an error trying to create the VSOCK vnode.
 */
void
destroy_socket_bind_port(
	struct socket	*so)	/* INOUT  - socket with bind port */
{
	kern_return_t	kr;
	mach_port_t	bindport;

	LOCK_ASSERT("destroy_socket_bind_port", !SOCKET_ISLOCKED(so));
	SOCKET_LOCK(so);

	/* Should only call this for sockets with bind ports. */
	bindport = so->vs_bindport;
	ASSERT(bindport != MACH_PORT_NULL);

	/*
	 *  Remove the port from the server port set and also
	 *  remove the <port,socket_addr> pair from the map.
	 */
	ux_server_remove_port(bindport);
	UNMAP_PORT_TO_SOCKET(bindport, so);

	/*
	 *  Because we know we are being called because of an error
	 *  trying to create the VSOCK vnode, we know that no send
	 *  rights for this bind port have been given out.  That
	 *  means it's safe to use mach_port_destroy().
	 */
	kr = mach_port_destroy(mach_task_self(), bindport);
	if (kr != KERN_SUCCESS)
		panic("destroy_socket_bind_port: kr 0x%x\n", kr);

	so->vs_bindport = MACH_PORT_NULL;

	SOCKET_UNLOCK(so);
}


/*
 * Append a unix domain socket to structures about
 * to be shipped.
 *
 * Append the socket's file port, unp structure,
 * the socket port, vnodeport and unp addr if the
 * socket is bound to an address, and the socket
 * itself along with its sockbufs.
 */
un_socket_append(
	struct socket	*so, 
	sgd_t		*sgdp,
	lmv_t		*lmvp,
	portv_t		*srights,
	portv_t		*rrights)
{
	sgde_t		*sgde;
	struct file	*fp;
	struct unpcb	*unp = sotounpcb(so);

	/*
	 *  Record port name for the socket's file port
	 *  in the port vector.
	 */
	fp = (struct file *)so->vs_fport;
	* PV_NEXT(rrights, 0) = (mach_port_t)fp;

	/*
	 * Send over the socket's unp 
	 */
	ASSERT( unp->unp_refs == 0 && unp->unp_nextref == 0 );
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_UNPCB, unp, sizeof(struct unpcb));

	/*
	 * Send the file structure to be used as a prototype on the
	 * other end (mostly to preserve makesend and relsend)
	 */
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_FILE, fp, sizeof(struct file));

	/*
	 * If there is a socket port associated with this socket,
	 * there should also be a vnode port pointing to the vnode
	 * which contains a send right to the socket port.  There
	 * should also be an address associated with the socket.  Send
	 * over the socket port, vnode port and socket bound address.
	 */
	if( so->vs_bindport ) {
		ASSERT(unp->unp_vnodeport != MACH_PORT_NULL );
		ASSERT(unp->unp_addr != 0 );

		/* record socket port receive rights */
		* PV_NEXT(rrights, 0) = so->vs_bindport;

		/* record vnode send right */
		* PV_NEXT(srights, 0) = unp->unp_vnodeport;
	}

	/*
	 * Send over this socket's address.  There may be an address
	 * associated with the socket even if it isn't bound to the
	 * address.  In particular, an accept socket gets a copy of
	 * the listening parent socket's address , but does not
	 * reference the vnode representing this address, nor does
	 * the vnode know about this socket.
	 *
	 * Note: it is assumed that a unp_addr will never be more
	 * than a single mbuf.
	 *
	 * WARNING: don't move this after sgd_socket_append(), as
	 * the socket extract code may end up linking this mbuf into
	 * the socket send or receive sockbuf mbuf chain.  (This
	 * mbuf is implicity SGDE_TYPE_SOADDR, but because it is
	 * seperate from the other mbufs it is not necessary to
	 * use an explicit SGDE_TYPE_SOADDR marker.)
	 */
	if( unp->unp_addr ) {
		ASSERT( unp->unp_addr->m_next == NULL );
		sgd_mbuf_append(sgdp, lmvp, unp->unp_addr, 0);
	}

	/*
	 *  Call sgd_socket_append() for the socket, so that
	 *  the sgd contains marshalling info for the whole socket.
	 */
	sgd_socket_append(sgdp, lmvp, so);

	/*
	 *  For datagram sockets, unp->unp_conn is a send right to
	 *  another socket's bind port.
	 */
	if ( so->so_type == SOCK_DGRAM && unp->unp_conn != MACH_PORT_NULL )
		* PV_NEXT(srights, 0) = (mach_port_t)unp->unp_conn;
}



/*
 * Extract a unix domain socket from the structures 
 * shipped over.
 *
 * Extract the socket's file port, unp structure,
 * the socket port, vnodeport and unp addr if the
 * socket is bound to an address, and the socket
 * itself along with its sockbufs.
 */
un_socket_extract(
	struct socket	**sop, 
	sgd_t		*sgdp,
	caddr_t		inline_data,
	lmv_t		*lmvp,
	portv_t		*srights,
	portv_t		*rrights)
{
	sgde_t		*sgde;
	mach_port_t	sockid = MACH_PORT_NULL;
	struct socket	*so;
	struct unpcb	*recv_unp, *unp;
	mach_port_t	*fp_portp; 
	struct file	*fp = NULL;
	struct file	*fp_proto;
	int		error, err2;
	kern_return_t	kr;

#define DataAddress(sgde)	(&inline_data[(int)sgde->sgde_base])
	/*
	 * extract pointer to file port receive right for fp
	 * associated with this socket
	 */
	fp_portp = (mach_port_t *) PV_NEXT(rrights, 1);

	/*
	 * Extract socket's unp 
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde->sgde_type == SGDE_TYPE_UNPCB);
	recv_unp = (struct unpcb *)DataAddress(sgde);

	/*
	 * Get a pointer to the file structure, used by
	 * recreate_filestruct_for_socket() in order to
	 * build the new file structure
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde->sgde_type == SGDE_TYPE_FILE);
	fp_proto = (struct file *)DataAddress(sgde);

	/*
	 * if there is a bound vnode associated with this
	 * socket, then extract the vnodeport send rights,
	 * the socket port receive rights, and the address
	 * to which this socket is bound.
	 */
	if( recv_unp->unp_vnodeport ) {

		/* extract vnode send rights */
		recv_unp->unp_vnodeport = *PV_NEXT(srights,1);

		/*
		 * Extract receive rights for the socket port of the
		 * socket that was shipped over.  The vnode pointed to
		 * by this unp contains send rights to this socket
		 * port.
		 */
		sockid = (mach_port_t) *PV_NEXT(rrights, 1);

		/*
		 * The arriving socket's bound address should
		 * exist if there is a unp_vnodeport.
		 */
		ASSERT(SGD_LOOKAHEAD(sgdp)->sgde_type <= MT_MAX);
	}

	/*
	 * Extract arriving socket's bound address if it exists.  It
	 * should exist if there is a unp_vnodeport.  It will also
	 * exist if this is a connected accept socket.  Note: it is
	 * assumed that a unp_addr will never be more than a single
	 * mbuf.
	 */
	if( recv_unp->unp_addr ) {
		error = sgd_um_mbuf(SGD_NEXT(sgdp, 1), inline_data, lmvp, 
				    &(recv_unp->unp_addr));
		UNDEBUG(U_SGD,("un_socket_extract: unp_addr=0x%x: %s\n",
			       recv_unp->unp_addr,
			       (recv_unp->unp_addr->m_dat +2)));
		if ( error != ESUCCESS )
			goto out0;
	}

	/*
	 *  Create socket and its mbufs, based on the copy of
	 *  socket and mbufs received in the relocation request
	 *  message.
	 */
	error = sgd_socket_extract(sgdp, inline_data, lmvp, &so);
	if (error)
		goto out1;

	/*
	 *  For connected datagram sockets, unp->unp_conn is a
	 *  send right for another socket's bind port.
	 */
	if (so->so_type == SOCK_DGRAM &&
	    recv_unp->unp_conn != MACH_PORT_NULL)
		recv_unp->unp_conn = (struct unpcb *) * PV_NEXT(srights, 1);

	/*
	 *  Now we have a socket and its mbufs.
	 *  Allocate a file structure for the socket, and replace
	 *  the implicitly allocated port with the one that just arrived.
	 */
	error = recreate_filestruct_for_socket(so, fp_portp, &fp, fp_proto);
	if ( error != ESUCCESS ) {
		UNDEBUG(U_SGD,("un_socket_extract: falloc failed: %d\n",
			       error));
		goto out2;
	}

	/*
	 * At this point we assume that the extract has succeeded,
	 * so we fill in the port name fields.  (This must not
	 * be done if there is a possibility of destroying the
	 * the socket, as the ports must not be destroyed -- they
	 * are shipped back to the user if there is an error.)
	 */

	/*
	 * fix up socket port name
	 */
	so->vs_bindport = sockid;

	/*
	 *  Fix up protocol control block.  For AF_UNIX, copies of
	 *  sb_cc and sb_mbcnt fields of receiving sockbuf are kept in
	 *  the protocol control block.
	 *
	 *  XXX Make an sgd_um_unpcb() routine!
	 */
#if 0
	ASSERT(so->so_rcv.sb_cc == recv_unp->unp_cc );
	ASSERT(so->so_rcv.sb_mbcnt == recv_unp->unp_mbcnt );
#endif

	/* unp->unp_socket was set up with socreate in sgd_socket_extract */
	unp = sotounpcb(so);
	unp->unp_vno = 0;	/* XXX - let a new fake vnode be selected? */
	unp->unp_addr = recv_unp->unp_addr;
	/* vnode port name field */
	unp->unp_vnodeport = recv_unp->unp_vnodeport;
	unp->unp_cc = recv_unp->unp_cc;
	unp->unp_mbcnt = recv_unp->unp_mbcnt;
	unp->unp_conn = recv_unp->unp_conn;

	*sop = so;
	return(ESUCCESS);

 out2:
	if ( err2 = VSOP_CLOSE(so) ) {
		UNDEBUG(U_SGD,
			("un_socket_extract: VSOP_CLOSE(0x%x) error %d\n",
			 so, err2));
	}
 out1:
	if( recv_unp->unp_addr )
		/* free mbuf allocated in sgd_um_mbuf() call */
		m_freem(recv_unp->unp_addr);

 out0:
	return(error);
#undef DataAddress
}
