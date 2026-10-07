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
 * $Log: un_st_vsops.c,v $
 * Revision 1.10  1995/02/01  22:11:02  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:45:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/09/01  01:37:56  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.7  1993/07/14  18:36:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  20:50:35  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:29:13  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/05/03  17:48:30  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:10:14  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.1  1993/02/16  20:07:04  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:34  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.2  1992/11/06  20:32:34  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 3.6  93/08/26  10:48:45  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.  Get rid of i860
 * compiler warnings.
 * 
 * Revision 3.5  93/06/14  14:07:42  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Moved an FP_UNREF to after magic value set.
 * 
 * Revision 3.4  92/12/11  14:58:10  mjl
 * No more VSOP_VSOCK{CREATE,DESTROY}() virtual socket operations.
 *
 * Revision 3.3  92/12/10  17:25:11  mjl
 * Got rid of VSOP_VSOCKCREATE() virtual socket op.  Deallocate bind port's
 * send right after connection is made in un_st_vsockconnect(); calling
 * routine doesn't do this for us anymore.
 * 
 * Revision 3.2  92/10/27  17:41:41  bhk
 * Aligned the virtual socket operations callout table with the new
 * virtual socket operations (userreq -> getaddr, added soreserve)
 * 
 * Revision 3.1  92/08/17  13:20:35  mjl
 * Add Locus copyright notice and RCS revision history.
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/unpcb.h>
#include <vsocket/vsocket.h>
#include <vsocket/vs_types.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <sys/user.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/un.h>

#include <tnc/un.h>

extern int vs_badop();
extern int un_socreate();
extern int un_soclose();
extern int un_sosend();
extern int un_sorecv();
extern int un_st_sodequeue(struct socket *, struct socket **,
			   struct mbuf **, int);
extern int un_solisten();
extern int un_soconnect();
extern int un_soconnect2();
extern int un_sobind();
extern int un_soshutdown();
extern int un_sosetopt();
extern int un_sogetopt();
extern int un_notify();
extern int un_relocate();
extern int un_sogetaddr();
extern int un_sosleep();
extern int un_st_vsockconnect(struct socket *, mach_port_t);
extern int soreserve();

struct vsocket_ops un_st_vsops = {
	un_socreate,
	un_soclose,
	un_sosend,
	un_sorecv,
	un_st_sodequeue,
	un_solisten,
	un_soconnect,
	un_soconnect2,
	un_sobind,
	un_soshutdown,
	un_sosetopt,
	un_sogetopt,
	un_notify,
	un_relocate,
	un_sogetaddr,
	un_sosleep,
	un_st_vsockconnect,
	soreserve
};


/*
 * if dequeuing the accept socket works, then set up any VSOP
 * related fields in the new accept socket, and allocate a
 * relocation policy structure for the socket pair so that they
 * can follow the first reader from node to node.
 */
int
un_st_sodequeue(
	struct socket	*head,
	struct socket	**so,
	struct mbuf	**nam,
	int		compat_43)
{
	int error = 0;
	struct uthread *uth = current_thread();

	ENTER_UN_VSOP(head, DISCARDLOCK, error);	
	if (error) {
		goto out;
	}

	if( error = sodequeue(head,so,nam,compat_43) ) {
		goto out;
	}

	/* Thou shalt have thy listening socket's virtual socket ops! */
	(*so)->vs_ops = head->vs_ops;

	/*
	 * set up vsocket related fields in socket - un_socreate() takes
	 * its own lock
	 */
	if( error = VSOP_CREATE(*so) ) {
		panic("un_st_sodequeue");
	}

	/*
	 * The connected pair will share a sockinfo structure.
	 *
	 * This routine will allocate one structure and set
	 * both socket vs_data structures to point at it. 
	 * Note that a lock for so must be taken before calling
	 * ALLOCATE_UN_SOCKINFO().
	 */
	SOCKET_LOCK(*so);
	ASSERT((*so)->vs_data == NULL);
	ALLOCATE_UN_SOCKINFO((*so));
	SOCKET_UNLOCK(*so);

 out:
	LEAVE_UN_VSOP(head);
	return(error);
}


/*
 * Connect to socket associated with the vnode that represents the
 * unix domain pathname address we requested to connect to.  This
 * routine is call from unp_connect().
 *
 * It is assumed that so is unlocked as we enter this routine.  It is
 * assumed that when we leave this routine, so is locked regardless of
 * whether an error has occured.
 */
int
un_st_vsockconnect(
	struct socket	*so,		/* connecting socket */
	mach_port_t	sockid)		/* socket port for listening socket */
{
	int		error = 0;
	struct file	*fp;
	kern_return_t	kr;

#ifdef	UN_DEBUG
	if (undebug & U_RELOC)
		printf("un_st_vsockconnect(so=0x%x, sockid=0x%x)\n",
		       so, sockid);
#endif
	ASSERT(sockid != MACH_PORT_NULL);

	ENTER_UN_VSOP(so, DISCARDLOCK, error);
	if (error) {
		LEAVE_UN_VSOP(so);
		return(error);
	}

	fp  = (struct file *)so->vs_fport;

	FP_LOCK(fp);		/* XXX Changed lock order here. -mjl */
	SOCKET_LOCK(so);

	/*
	 * make sure another thread isn't already performing
	 * a connect().
	 */
	if( so->so_state & SS_ISCONNECTING ) {
		/* SOCKET_UNLOCK(so); /* XXX Julia, how does this jive w/ comment above? */
		FP_UNLOCK(fp);
		error = EISCONN;
		goto out;
	}
	soisconnecting(so);

	/*
	 * Step One:  Remove file ports from server port set.
	 * This is so no more requests can be accepted on this port.
	 * Also remove bound socket port if there is one.
	 */
	ux_server_remove_port((mach_port_t)fp);
	if( so->vs_bindport ) {
		ux_server_remove_port((mach_port_t)so->vs_bindport);
	}

	/*
	 * Step Two:  Force outstanding active system calls to restart.
	 */
	so->vs_flags |= VS_RESTART;

	/*
	 * Step Three:  Force outstanding blocked system calls to restart.
	 */
	un_wakeall(so);

	/*
	 *  Step Four:  Restart queued select operations.  These will
	 *  be satisfied or requeued on the new node.
	 *
	 * There should be no problem with select operations waiting
	 * on the socket buffers, as the socket is not currently
	 * connected to anything.
	 */

	/*
	 *  Step Five:  wait for outstanding connect requests
	 *  on a bound socket to complete.
	 */
	if( so->vs_bindport ) {
		UNMAP_PORT_TO_SOCKET(so->vs_bindport, so);
	}

	/*
	 * Check socket for outstanding active threads.
	 * Only the current thread should be outstanding, so the
	 * sequence count should be off by 1.  
	 */
	so->vs_flags |= VS_FP_SEQNO_WAIT;
	mutex_lock(&so->vs_seqno_mutex);
	while ( seqno_from_port(fp) != (fp->f_seqno + 1) ) {
		/*
		 * Threads still active on this socket.  Cannot
		 * relocate yet.  As each thread exits, it
		 * will unlock this lock, which will allow this
		 * thread to check the count again.
		 */
		mutex_lock(&so->vs_seqno_mutex);
	}
	/*
	 * assumption -- it is ok to unlock a mutex lock that may not
	 * actually be locked.
	 */
	mutex_unlock(&so->vs_seqno_mutex);
	so->vs_flags &= ~VS_FP_SEQNO_WAIT;

	/*
	 * At this point, we know there are no more references
	 * to the file port for this socket, so we no longer
	 * need to keep the locks.
	 */
	SOCKET_UNLOCK(so);
	FP_UNLOCK(fp);

	/*
	 * Package up entire socket entity -- file port
	 * rights, mbufs hanging off this socket, rights for
	 * the socket port, etc.  to the remote socket's node. 
	 * At the remote node, this socket will be unpacked,
	 * recreated, and then a regular vsock type connect
	 * will be done, calling unp_local_connect().
	 */
	error = un_vsock_sendconnect(so, sockid);
	/*
	 * must be holding socket lock when we
	 * exit this routine.
	 */
	SOCKET_LOCK(so);
	/* turn off VS_RESTART and VS_IS_SLEEPING flags */
	so->vs_flags = VS_INITIALIZED;

	if( error == ESUCCESS ) {
		/*
		 * The socket has been successfully cloned on the
		 * new node, and the connection was accepted.  So
		 * we have to close the original.  Remove the
		 * existence reference to the file port.  Setting
		 * f_magic to F_RELOC prevents fdealloc() from
		 * trying to destroy the file port, which has
		 * migrated to the new node.
		 */
#ifdef	UN_DEBUG
		if (undebug & U_RELOC)
			printf("un_st_vsockconnect: fp->f_count = %d\n",
			 fp->f_count);
#endif	/* UN_DEBUG */

		fp->f_magic = F_RELOC;
		FP_UNREF(fp);

		/*
		 * This socket is about to be released, but first it
		 * passes back through connect(), so the flags must be
		 * set right so that the correct return value is
		 * passed back to the process making the connect call.
		 */
		so->so_state |= SS_ISCONNECTED;
		so->so_state &= ~SS_ISCONNECTING;
	} else {
		/*
		 *  For whatever reason, the relocation failed.  Since we're
		 *  the last thread that knows anything about this socket
		 *  and its file structure, we'd better reattach the returned
		 *  receive rights for this file port to the server port set.
		 *
		 *  Reset vs_flags relocation state bits, and turn
		 *  off ISCONNECTING flag in so_flags.
		 *
		 *  XXX Right now it is not possible to distinguish
		 *  between a node failure and deallocation of the bind
		 *  port receive right (i.e. the target socket of the
		 *  connect operation has gone away).  Both cases return
		 *  EBADRPC here; but if the latter case, we want to
		 *  return ENOTSOCK to indicate that the address
		 *  specified to connect() is now bogus.
		 */
		if (error == EBADRPC)
			error = ENOTSOCK;
#ifdef	UN_DEBUG
		if (undebug & U_RELOC)
			printf("un_st_vsockconnect: attempt got errno %d\n",
			       error);
		if (undebug & U_SETERROR)
			so->so_error = error;
#endif	/* UN_DEBUG */

		FP_LOCK(fp);
		ux_server_add_port((mach_port_t)fp);
		FP_UNLOCK(fp);
		so->so_state &= ~SS_ISCONNECTING;
		if( so->vs_bindport ) {
			MAP_PORT_TO_SOCKET(so->vs_bindport, so);
			ux_server_add_port((mach_port_t)so->vs_bindport);
		}
	}

out:
	/*
	 * finished using this send right in the VSOP_VSOCKCONNECT()
	 * call.  Get rid of it.
	 */
	kr = mach_port_deallocate(mach_task_self(), sockid);
	if (kr != KERN_SUCCESS)
		panic("un_st_vsockconnect: mach_port_deallocate: kr 0x%x", kr);

	/* socket lock is kept on exit */
	LOCK_ASSERT("un_st_vsockconnect", SOCKET_ISLOCKED(so));

	LEAVE_UN_VSOP(so);
	return(error);
}
