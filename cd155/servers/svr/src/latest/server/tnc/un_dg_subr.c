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
 * $Log: un_dg_subr.c,v $
 * Revision 1.6  1995/02/01  22:03:29  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:44:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/01  01:37:13  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.3  1993/07/14  18:35:47  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1993/05/06  19:27:25  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:54  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.1  93/08/26  10:47:17  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.
 * 
 * Revision 3.0  92/12/10  16:59:55  mjl
 * Support routines for TNC Unix domain datagram sockets.
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
#include <tnc/vector.h>
#include <tnc/sgd.h>


/*
 *  Special TNC datagram disconnect routine, needed because:
 *  1. Connections are made with Mach ports and not caddr_t's, and therefore
 *  2. Disconnect errors are detected by potential senders encountering
 *	dead names, and not by receivers posting ECONNRESET to every
 *	socket in their unp_refs/unp_nextref chains.
 */
void
un_dg_disconnect(
	struct unpcb	*unp)
{
	kern_return_t	kr;

	/*
	 *  Sending sockets detect ECONNRESET when they try to use
	 *  (mach_port_t)unp->unp_conn and notice it has become a
	 *  dead name.  Therefore, we don't need to maintain these
	 *  lists of referrencing sockets.
	 */
	ASSERT(unp->unp_refs == NULL && unp->unp_nextref == NULL);

	/*
	 *  To disconnect, get rid of our copy of the other side's
	 *  bind port send right.
	 */
	kr = mach_port_deallocate(mach_task_self(),
				  (mach_port_t)unp->unp_conn);
	if (kr != KERN_SUCCESS)
	    panic("un_dg_disconnect: deallocate: unp_conn 0x%x, kr 0x%x\n",
		  unp->unp_conn, kr);
	unp->unp_conn = (struct unpcb *)MACH_PORT_NULL;
}


/*
 *  Send a datagram to the other side of the connection.  In effect,
 *  relocates the mbufs of the sender's so_snd sockbuf, and they are
 *  sbappendaddr()'ed to the remote so_rcv sockbuf.
 */
int
remote_append_dgram(
	struct socket	*so,
	struct mbuf	*m,
	struct mbuf	*control)
{
	struct socket	*so2;
	int		rval	= ESUCCESS;
	struct unpcb	*unp	= sotounpcb(so);
	sgd_t		*sgdp;
	sgde_t		*hdr, *sgde;
	lmv_t		*lmvp;
	mblist_i_t	mli;
	struct mbuf	*mp;
	int		newchain;


	LOCK_ASSERT("remote_append_dgram", SOCKET_ISLOCKED(so));

	/*
	 *  Efficiency hack!  If we can locate the other side's bind
	 *  port in the port-to-socket map, then the other socket can't
	 *  relocate until we give up the map reference.  It is then
	 *  OK to call local_append_dgram() instead!
	 */
	PORT_TO_SOCKET_LOOKUP((mach_port_t)unp->unp_conn, so2);
	if (so2 != NULL) {
		SOCKET_UNLOCK(so);
		rval = unp_local_connect(so, so2);
		LOCK_ASSERT("remote_append_dgram local!", SOCKET_ISLOCKED(so));
		if (rval == ESUCCESS)
			rval = local_append_dgram(so, so2, m, control);
		SOCKET_LOOKUP_DONE(so2, FALSE);
		return (rval);
	}

	/*
	 *  Allocate expandable vectors for the socket graph descriptor
	 *  and large mbuf vector.
	 */
	if ( (sgdp = SGD_ALLOC()) == NULL )
		panic("un_really_reloc: cannot allocate sgd\n");
	if ( (lmvp = LMV_ALLOC()) == NULL )
		panic("un_really_reloc: cannot allocate lmv\n");

	/*
	 *  Label the sgd's header descriptor---we are sending datagram mbufs
	 */
	hdr = SGD_NEXT(sgdp, 0);
	hdr->sgde_type = SGDE_TYPE_SGDHDR;
	hdr->sgd_type = SGD_TYPE_UN_DG_PKT;	/* datagram packet */
	hdr->sgd_inlsize = 0;
	hdr->sgd_outlcnt = 0;
	hdr->sgd_len = 1;

	/*
	 *  Marshal the sending socket's address, if any.
	 */
	if (unp->unp_addr) {
		sgde = SGD_NEXT(sgdp, 0);
		MAKE_SIMPLE_SGDE(sgdp, sgde, SGDE_TYPE_SOADDR, NULL, 0);
		sgd_mbuf_append(sgdp, lmvp, unp->unp_addr, TRUE);
	}

	/*
	 *  Marshal mbufs with ancillary control information, if any.
	 */
	if (control) {
		sgde = SGD_NEXT(sgdp, 0);
		MAKE_SIMPLE_SGDE(sgdp, sgde, SGDE_TYPE_CTRLMBUFS, NULL, 0);
		MLI_INIT(&mli, &control);
		while ( (mp = MLI_NEXT(&mli, &newchain)) != NULL )
			sgd_mbuf_append(sgdp, lmvp, mp, newchain);
	}

	/*
	 *  Finally, marshal the data mbufs to be sent.
	 */
	ASSERT(m);
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde, SGDE_TYPE_SNDSB, NULL, 0);
	MLI_INIT(&mli, &m);
	while ( (mp = MLI_NEXT(&mli, &newchain)) != NULL )
		sgd_mbuf_append(sgdp, lmvp, mp, newchain);

	/*
	 *  Transmit the mbufs to the remote datagram socket.
	 *  We have a send right for its bind port in unp->unp_conn.
	 */
	ASSERT(unp->unp_conn != MACH_PORT_NULL);
	rval = tnc_relocate((mach_port_t)unp->unp_conn, sgdp, lmvp, NULL,NULL);

	/*
	 *  Whether it succeeded or not, we still need to deallocate
	 *  expandable vectors created here.
	 */
	SGD_DEALLOC(sgdp);
	LMV_DEALLOC(lmvp);

	/*
	 *  Success means we can deallocate the control and data mbufs
	 *  on this node---they have been cloned on the remote node.
	 */
	if (rval == ESUCCESS) {
		m_freem(m);
		if (control)
			m_freem(control);
	}

	return (rval);
}


/*
 *  Arrival routine for mbufs transmitted in remote_append_dgram() above.
 */
int
un_dg_pkt_arrival(
	mach_port_t	server_port,
	sgd_t		*sgdp,
	caddr_t		inline_data,
	lmv_t		*lmvp,
	portv_t		*rrights,
	portv_t		*srights)
{
	int		error;
	struct socket	*so;
	sgde_t		*sgde;
	mblist_i_t	mli;
	struct sockaddr	*from;
	struct mbuf	*nam		= NULL;
	struct mbuf	*control	= NULL;
	struct mbuf	*m		= NULL;
	struct mbuf	*m1;

	/*
	 *  Translate bind port to receiving socket.
	 */
	PORT_TO_SOCKET_LOOKUP(server_port, so);
	if (so == NULL) {
		UNDGDEBUG(U_RELOC,("un_dg_pkt_arrival: port 0x%x not mapped\n",
				   server_port));
		return (ECONNRESET);
	}
	ASSERT(so->so_type == SOCK_DGRAM);

	/*
	 *  No port rights are sent, only mbuf data.
	 */
	ASSERT(VECTOR_LEN(rrights) == 0 && VECTOR_LEN(srights) == 0);

	/* Skip over the header. */
	ASSERT(SGD_LOOKAHEAD(sgdp) == SGD_HEADER(sgdp));
	(void) SGD_NEXT(sgdp, 1);

	/*
	 *  Extract all the mbufs from the message.  Socket address
	 *  and control mbufs are optional, data mbufs are not.
	 *  (Markers in the sgd seperate these three different classes
	 *  of mbuf.)
	 */
	while ( (sgde = SGD_NEXT(sgdp, 1)) != NULL ) {
		switch ( sgde->sgde_type ) {

		case SGDE_TYPE_SNDSB:
			/*
			 *  data mbufs
			 */
			ASSERT(m == NULL);
			MLI_INIT(&mli, &m);
			break;

		case SGDE_TYPE_CTRLMBUFS:
			/*
			 *  ancillary control info
			 */
			ASSERT(control == NULL);
			MLI_INIT(&mli, &control);
			break;

		case SGDE_TYPE_SOADDR:
			/*
			 *  address mbuf (should be only one, just grab it)
			 */
			ASSERT(nam == NULL);
			sgde = SGD_NEXT(sgdp, 1);	/* move past marker */
			error = sgd_um_mbuf(sgde, inline_data, lmvp, &nam);
			if (error)
				goto out;
			MLI_DONE(&mli);
			break;

		default:
			/*
			 *  Unmarshal mbuf into whichever mbuf list
			 *  the mli is currently attached to.
			 */
			if ( ! MLI_READY(&mli) )
				panic("un_dg_pkt_arrival: MLI (>1 nam mbuf?)");
			ASSERT(sgde->sgde_type < MT_MAX);
			error = sgd_um_mbuf(sgde, inline_data, lmvp, &m1);
			if (error)
				goto out;
			MLI_PUT(&mli, m1,
				sgde->sgde_xflags & SGDE_MB_CHAINHDR);
			break;
		}
	}

	/*
	 *  Now post the mbuf data to the receiving socket.  We don't
	 *  need to call unp_local_connect(), since all it would do
	 *  for us is lock sockets and update the state of the sending
	 *  socket---we can lock 'em ourselves, and the sending socket
	 *  is just a fake here.
	 *
	 *  In particular the fake tsock has no socklocks structure;
	 *  see the modified LOCK_ASSERT() in local_append_dgram().
	 */
	{
		struct socket	tsock;
		struct unpcb	tunp;

		tsock.so_lock = NULL;
		tsock.so_pcb = (caddr_t)&tunp;
		tunp.unp_addr = nam;
		
		SOCKET_LOCK(so);
		error = local_append_dgram(&tsock, so, m, control);
		SOCKET_UNLOCK(so);
	}

out:
	SOCKET_LOOKUP_DONE(so, TRUE);
	return (error);
}
