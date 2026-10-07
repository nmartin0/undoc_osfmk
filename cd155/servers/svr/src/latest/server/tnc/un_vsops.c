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
 * $Log: un_vsops.c,v $
 * Revision 1.11  1995/02/01  22:14:11  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:45:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/10/11  18:59:16  nandy
 * un_sogetaddr() was expecting pointer to mbuf as an argument, but getsockname1()
 * was pasing pointer to a pointer to mbuf.
 *
 * Revision 1.8  1993/09/01  01:38:04  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.7  1993/07/14  18:36:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  20:50:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:29:33  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/05/03  17:48:44  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:10:19  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.1  1993/02/16  20:07:14  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:42  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.2  1992/11/06  20:32:37  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 3.8  93/08/26  10:48:59  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.
 * 
 * Revision 3.7  93/06/14  14:12:42  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Changed some of the file struct counts as part of the changes for sync close.
 * 
 * Revision 3.6  92/12/11  14:59:06  mjl
 * Moved and renamed un_vsockdestroy() to unp_destroy_binding() in
 * bsd/uipc_usrreq.c .
 *
 * Revision 3.5  92/12/10  17:32:42  mjl
 * Add un_vsockcreate(), now a common virtual socket op for both datagram and
 * stream sockets.  Use new debug and port-to-socket mapping macros.
 * 
 * Revision 3.4  92/10/27  17:42:23  bhk
 * Changed the userreq virtual socket operation to the getaddr virutal
 * socket operation
 * 
 * Revision 3.3  92/09/29  08:23:31  roman
 * Change references from "site" to "node".
 * 
 * Revision 3.2  92/09/28  13:33:18  klh
 * Fix sense of test---un_policy() returns an errno, not TRUE/FALSE.
 * (klh for mjl)
 * 
 * Revision 3.1  92/08/17  13:21:11  mjl
 * Get rid of extraneous arg to un_sosleep().
 * 
 * Revision 3.0  92/08/08  01:24:49  jdh
 * most of un_pp_vsops.c was put in here, where it is used by
 * both pipe and unix domain stream sockets -- jdh
 * 
 * Revision 3.1  92/06/30  17:21:42  mjl
 * Fix cleanup of file and socket structures left behind after successful
 * pipe relocation.  The sockinfo structure is now reference counted.
 * 
 * Revision 3.7  92/06/16  18:41:52  bhk
 * moved SOCKET_LOCK before INCREMENT_VS_REFCNT macro
 * 
 * Revision 3.6  92/06/15  17:39:10  mjl
 * In virtual operations that create a socket, get so->vs_fport value from
 * uthread area.
 * 
 * Revision 3.5  92/06/11  16:28:50  mjl
 * Remove bogus assertion on vs_seqno in ust_soconnect2().
 * Call select_wakeup() before relocating().  This effectively places select
 * queue entries in the file port's receive queue; they are then reprocessed
 * once the receive-right arrives on the new node.  Rewrote ust_relocate()
 * for clarity; also, deallocate local resources if relocation succeeds.
 * 
 * Revision 3.4  92/05/06  13:10:48  bhk
 * added sosleep as AF_UNIX virtual socket sleep operation
 * 
 * Revision 3.3  92/04/22  17:00:16  bhk
 * Added support for virtual getsockname call
 * 
 * Revision 3.2  92/04/06  11:01:09  mjl
 * Moved common ust definitions to ust.h.  New {ENTER,LEAVE}_UN_VSOP() macroes.
 * New VSOP_RELOCATE() virtual socket operation.  We can now successfully wait
 * for all outstanding threads operating on a socket pair to complete prior to
 * beginning an actual relocation.
 * 
 * Revision 3.1  92/03/20  17:06:47  bhk
 * moved vsocket.h to vsocket directory
 *  
 * Revision 3.0  92/03/18  17:18:49  mjl
 * Virtual socket layer for AF_UNIX SOCK_STREAM sockets.
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

int 
un_socreate(vs)
struct socket *vs;	/* run TNC socket creation hook using socket */
{
	struct uthread *uth = current_thread();
	SOCKET_LOCK(vs);

	INCREMENT_VS_REFCNT(vs,"un_socreate:");

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

	SOCKET_UNLOCK(vs);
	return 0;
}



int
un_soclose(vs)
struct socket *vs;		/* vsocket to close */
{
	int error = 0;

	ENTER_UN_VSOP(vs, KEEPLOCK, error);
	if (error) {
		SOCKET_UNLOCK(vs);
		LEAVE_UN_VSOP(vs);
		return(error);
	}

	/*
	 *  May need to deallocate a un_sock_t structure.
	 */
	if( vs->vs_data )
		FREE_UN_SOCKINFO(vs);

	SOCKET_UNLOCK(vs);

	/* This calls soclose() when vs_refcnt goes to zero. */
	DECREMENT_VS_REFCNT(vs,"un_soclose:");

	ASSERT(vs->vs_bindport == NULL);

	LEAVE_UN_VSOP(vs);  /* XXX Currently a no-op.  Socket may be gone! */
	return(error);
}
		


int
un_sosend(vs,nam,uio,top,control,flags)
struct socket *vs;		/* vsocket to send on */
struct mbuf *nam;	/* name to send to */
struct uio *uio;		/* data to send */
struct mbuf *top;	/* top of mbuf chain (no uio) */
struct mbuf *control;	/* control message */
int flags;		/* flags */
{
	int error = 0;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = sosend(vs,nam,uio,top,control,flags);
	LEAVE_UN_VSOP(vs);
	return(error);
}

int
un_sorecv(vs,nam,uio,topp,controlp,flagsp)
struct socket *vs;		/* vsocket to receive on */
struct mbuf **nam;	/* name received from  */
struct uio *uio;		/* data to send */
struct mbuf **topp;	/* top of mbuf chain (no uio) */
struct mbuf **controlp;	/* control message */
int *flagsp;		/* flags */
{
	int error = 0;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = soreceive(vs,nam,uio,topp,controlp,flagsp);
	LEAVE_UN_VSOP(vs);
	return(error);
}

int
un_solisten(vs,backlog)
struct socket *vs;		/* vsocket to listen on */
int backlog;		/* how many connections to queue */
{
	int error = 0;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = solisten(vs,backlog);
	LEAVE_UN_VSOP(vs);
	return(error);
}

int
un_soconnect(vs,nam)
struct socket *vs;		/* vsocket to connect */
struct mbuf *nam;	/* who to connect to */
{
	int error;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = soconnect(vs,nam);

	LEAVE_UN_VSOP(vs);
	return(error);

}

int
un_soconnect2(vs1,vs2)
struct socket *vs1;		/* half of socket pair to connect */
struct socket *vs2;		/* other half of socket pair */
{
	int error = 0;

	ENTER_UN_VSOP(vs1,DISCARDLOCK,error);
	if (!error)
		error = soconnect2(vs1,vs2); 

	if (!error) {
		SOCKET_LOCK(vs1);
		ALLOCATE_UN_SOCKINFO(vs1);	/* shared by paired socks */
		SOCKET_UNLOCK(vs1);
	}

	LEAVE_UN_VSOP(vs1);
	return(error);
}


int
un_sobind(vs,nam)
struct socket *vs;		/* vsocket to bind */
struct mbuf *nam;	/* name to bind to */
{
	int error = 0;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = sobind(vs,nam);
	LEAVE_UN_VSOP(vs);
	return(error);
}

int
un_soshutdown(vs,how)
struct socket *vs;		/* vsocket to shut down */
int how;		/* how to shut it down */
{
	int error = 0;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = soshutdown(vs,how);
	LEAVE_UN_VSOP(vs);
	return(error);
}

int
un_sosetopt(vs,level,optname,m0)
struct socket *vs;	/* vsocket to set option on */
int level;		/* protocol for option */
int optname;		/* name of option */
struct mbuf *m0;	/* value to set option to */
{
	int error = 0;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = sosetopt(vs,level,optname,m0);
	LEAVE_UN_VSOP(vs);
	return(error);
}

int
un_sogetopt(vs,level,optname,m0)
struct socket *vs;		/* vsocket to get option on */
int level;		/* protocol for option */
int optname;		/* name of option */
struct mbuf **m0;	/* value of option */

{
	int error = 0;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = sogetopt(vs,level,optname,m0);
	LEAVE_UN_VSOP(vs);
	return(error);
}


int
un_notify(so,pid,new_node)
struct socket *so;		/* vsocket to be told of process migration */
pid_t pid;			/* process that migrated */
int new_node;			/* process' new execution node */
{
	struct socket *so2;
	int error = 0;

	UNDEBUG(U_NOTIFY,("un_notify(so=0x%x, pid=%d, node=%d)\n",
			  so, pid, new_node));

	ENTER_UN_VSOP(so, KEEPLOCK, error);
	if (error && error != ERESTART) {
		panic("un_notify: bogus error %d on enter\n", error);
	}

	/*
	 *  error == ERESTART means that a previous notification has
	 *  started to relocate this socket.  That's OK, and in fact we
	 *  really ought to call the policy routine to see if this new
	 *  notification would cause the storage node to be any different.
	 *  For now, though, just let the original un_notify() call do
	 *  its work.
	 */
	if (error == ERESTART) {
		/* Don't restart the notification! */
		UNDEBUG(U_NOTIFY,("un_notify: would restart\n"));
		error = 0;
		goto out;
	}

	/*
	 *  Call the policy routine to see if a relocation is in order.
	 */
	if ( ! un_policy((policy_state_t *)so->vs_data, pid, new_node) ) {
		/*
		 *  Stay where we are, how boring...
		 */
		goto out;
	}

	UNDEBUG(U_NOTIFY,("un_notify: so 0x%x relocation started!\n", so));

	/*
	 *  We're going to relocate this socket (and maybe its partner)!
	 *  Step One:  Remove file ports and bound socket ports from 
	 *  server port set.
	 */
	ux_server_remove_port(so->vs_fport);
	if( so->vs_bindport ) {
		ux_server_remove_port((mach_port_t)so->vs_bindport);
	}
	if ( (so2 = sopartner(so)) != NULL ) {
		ux_server_remove_port(so2->vs_fport);
		if( so2->vs_bindport ) {
			ux_server_remove_port((mach_port_t)so2->vs_bindport);
		}
	}

	/*
	 *  Step Two:  Force outstanding active system calls to restart.
	 */
	so->vs_flags |= VS_RESTART;
	so->vs_flags &= ~VS_NOTHREADS;
	if (so2) {
		so2->vs_flags |= VS_RESTART;
		so2->vs_flags &= ~VS_NOTHREADS;
	}

	/*
	 *  Step Three:  Force outstanding blocked system calls to restart.
	 */
	un_wakeall(so);
	if (so2)
		un_wakeall(so2);

	/*
	 *  Step Four:  Restart queued select operations.  These will
	 *  be satisfied or requeued on the new node.
	 *
	 *  XXX Should we call sowakeup() instead of calling
	 *  select_wakeup() directly?
	 *  XXX It's probably not necessary to do this for all four
	 *  sockbufs.
	 */
	SOCKBUF_LOCK(&so->so_rcv);
	select_wakeup(&so->so_rcv.sb_selq);
	SOCKBUF_UNLOCK(&so->so_rcv);
	SOCKBUF_LOCK(&so->so_snd);
	select_wakeup(&so->so_snd.sb_selq);
	SOCKBUF_UNLOCK(&so->so_snd);
	if (so2) {
		SOCKBUF_LOCK(&so2->so_rcv);
		select_wakeup(&so2->so_rcv.sb_selq);
		SOCKBUF_UNLOCK(&so2->so_rcv);
		SOCKBUF_LOCK(&so2->so_snd);
		select_wakeup(&so2->so_snd.sb_selq);
		SOCKBUF_UNLOCK(&so2->so_snd);
	}

	/*
	 *  Step Five:  wait for outstanding connect requests
	 *  on bound sockets to complete.  (Note that there
	 *  are unlikely to be any.)
	 */
	if( so->vs_bindport )
		UNMAP_PORT_TO_SOCKET(so->vs_bindport, so);
	if( so2 && so2->vs_bindport )
		UNMAP_PORT_TO_SOCKET(so2->vs_bindport, so);

	/*
 	 *  That's it.  The last of the fileserver threads operating
	 *  on these sockets will issue the relocation RPC.
	 *  See tnc_fsvr_end_op(), called from end_fileserver_op().
	 */

out:
	SOCKET_UNLOCK(so);
	LEAVE_UN_VSOP(so);
	return(error);
}


/*
 *  Attempt to relocate the socket and its connected partner.
 *  Returns TRUE iff the relocation succeeded (and thus the
 *  caller does not need to do any socket or file struct unlocking).
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
un_relocate(so)
	struct socket *so;
{
	struct socket	*so2;
	struct file	*fp, *fp2;
	int		error, err2;

	LOCK_ASSERT("un_relocate", SOCKET_ISLOCKED(so));
	ASSERT(IS_RELOCATING(so));

	/*
	 *  Not relocating unpaired sockets as yet...
	 */
	if ( (so2 = sopartner(so)) == NULL )
		return FALSE;
	fp  = (struct file *)so->vs_fport;
	fp2 = (struct file *)so2->vs_fport;

	/*
	 * Check both sockets for outstanding active threads.  We
	 * should proceed only if all operations on both sockets have
	 * finished.  (When the seqno in the socket == the seqno in
	 * the mach port, this means all operations are complete.) At
	 * this point, the current thread must be the last thread on
	 * one of the sockets; it has already completed the operation
	 * it was performing and has incremented seqno to indicate
	 * this.
	 */
	if ( seqno_from_port(fp) == fp->f_seqno )
		so->vs_flags |= VS_NOTHREADS;
	if ( seqno_from_port(fp2) == fp2->f_seqno )
		so2->vs_flags |= VS_NOTHREADS;
	if ( ! (so->vs_flags & so2->vs_flags & VS_NOTHREADS) )
		/*
		 *  Threads still active on this socket or its partner.
		 *  Cannot relocate yet.
		 */
		return FALSE;

	/*
	 *  The current thread is the last active thread.
	 *  Attempt to relocate.
	 */
	SOCKET_UNLOCK(so);
	FP_UNLOCK(fp);

	error = un_really_relocate(so);
	if ( error != ESUCCESS )
		goto reattach;

	/*
	 *  The paired sockets and their file structs have been
	 *  successfully cloned on the new node.  Now we have to close
	 *  and deallocate the originals.  This happens as a side
	 *  effect of the final FP_UNREF() on the respective file
	 *  pointers.
	 *
	 *  This is the last thread that holds any reference to the
	 *  "old node" file structs and sockets.  When this thread
	 *  completes, FILE_LOOKUP_DONE() in end_fileserver_op() will
	 *  expect to call FP_UNREF() on the file pointer
	 *  corresponding to our argument socket, so we must leave
	 *  that file struct with a reference count of one.
	 *
	 *  Before any FP_UNREF()ing can be done, however, we have to
	 *  set f_magic to F_RELOC.  This prevents fdealloc() from
	 *  trying to deallocate the file port, which was just
	 *  migrated.  Also, we must clear the VS_RESTART flags, since
	 *  otherwise no virtual socket operations can affect these
	 *  sockets (e.g. VSOP_CLOSE()).
	 */
	fp->f_magic = fp2->f_magic = F_RELOC;
	so->vs_flags &= ~VS_RESTART;
	so2->vs_flags &= ~VS_RESTART;

	fp->f_count = 1; /* gets closed by end_fileserver_op */
	fp2->f_count = 1;
	FP_UNREF(fp2);	/* will invoke closef() */

	return TRUE;

reattach:
	/*
	 *  For whatever reason, the relocation failed.  Since we're
	 *  the last thread that knows anything about these sockets
	 *  and their file structures, we'd better reattach them to
	 *  the server port set.
	 *
	 *  Our caller expects these structures to be either locked or
	 *  gone, so we relock.
	 *
	 *  Also, got to reset vs_flags relocation state bits.
	 */

	UNDEBUG(U_RELOC,("un_relocate: attempt got errno %d\n", error));
#ifdef	UN_DEBUG
	if (undebug & U_SETERROR)
		so->so_error = so2->so_error = error;
#endif
	FP_LOCK((struct file *)(so->vs_fport));
	SOCKET_LOCK(so);

	so->vs_flags = VS_INITIALIZED;
	so2->vs_flags = VS_INITIALIZED;
	ux_server_add_port(so->vs_fport);
	ux_server_add_port(so2->vs_fport);
	if( so->vs_bindport ) {
		MAP_PORT_TO_SOCKET(so->vs_bindport, so);
		ux_server_add_port((mach_port_t)so->vs_bindport);
	}
	if( so2->vs_bindport ) {
		MAP_PORT_TO_SOCKET(so2->vs_bindport, so2);
		ux_server_add_port((mach_port_t)so2->vs_bindport);
	}

	return FALSE;
}


int
un_sogetaddr(vs,nam,which,compat_43)
	struct socket *vs;
	int	which, compat_43;
	struct mbuf **nam;
{
	int error = 0;

	ENTER_UN_VSOP(vs, DISCARDLOCK, error);
	if (!error)
		error = sogetaddr(vs,nam,which,compat_43);
	LEAVE_UN_VSOP(vs);
	return(error);
}



int
un_sosleep(
	struct socket *so,
	caddr_t addr,
	int pri, 
	int tmo)
{
	int error;

	UNDEBUG(U_NOTIFY,
		("un_sosleep(so=0x%x, addr=0x%x, pri=0x%x, tmo=0x%x)\n", 
		 so, addr, pri, tmo));

	/* use this test rather than the ENTER_UN_VSOP() macro */
	if ( error = TNC_SOSLEEP_HOOK(so) ) {
		/*
		 * Only one thread is allowed to sleep if this
		 * socket is relocating, and this ain't it.
		 */
		return error;
	}

	return(sosleep(so, addr, pri, tmo));
}
