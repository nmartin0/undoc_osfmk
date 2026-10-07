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
 * $Log: reloc_subr.c,v $
 * Revision 1.11  1995/02/01  21:46:57  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/12/17  00:00:04  slk
 *  Reviewer(s): Mike Leibensperger, John Litvin, Susan Lively Klug
 *  Risk: Medium, many lines of code changed, and turned off FIFO relocation.
 *  Benefit or PTS #: Fix mandatory PTS #10881
 *    Disabled FIFO relocation by defining NO_PIPE_RELOC.
 *    Improved the clarity of the FIFO relocation code.
 *    Restore saved port information if FIFO relocation fails (or is disabled).
 *  Testing: Ran three test cases from bug report on filesystem node, and
 *    non-filesystem node.  Split the read and write calls and ran them on
 *    different nodes, both filesystem and non-filesystem.  All of the above
 *    with the test FIFO file created new, and already existing for each test
 *    case. Selected VSX and EATS.
 *  Module(s):
 *         server/tnc/reloc_subr.c
 *         server/tnc/un_ff_ops.c
 *         server/tnc/un_ff_reloc.c
 *         server/tnc/un_ff_subr.c
 *
 * Revision 1.9  1994/11/18  20:43:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/07/29  21:53:55  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server slowdown.
 *
 * Revision 1.7  1993/07/14  18:33:14  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:06:34  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.3  1993/07/01  20:45:35  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:22:46  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:45:37  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:08:49  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:11  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:02:55  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:02:12  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.8  93/07/27  11:27:30  mjl
 * [Bug #0308] Break tnc_fsvr_end_op() routine into two: tnc_should_relocate()
 * and tnc_relocate_try().  Also, remove some stale code, mark some other code
 * as (freshly!) stale.
 * 
 * Revision 3.7  93/07/07  16:40:55  bhk
 * Detect a deadlock and return an error, otherwise process as usuall
 * 
 * Revision 3.6  93/06/14  13:56:23  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Changed tnc_fsvr_end_op so that if the file is already closed, to just
 * return without trying to relocate it.
 * 
 * Revision 3.5  92/12/10  17:07:28  mjl
 * Use new debug macros.  The print_port_info() routine moved to un_debug.c .
 *
 * Revision 3.4  92/11/18  12:41:32  mjl
 * Get rid of no-op tnc_fsvr_start_op() hook.  Modify tnc_fsvr_end_op() to
 * deal with new preconditions: FP_LOCK() held, shared FIFO mutex held.
 * End-op hook now called from file_port_increment_seqno().
 * 
 * Revision 3.3  92/09/28  13:25:54  klh
 * Cosmetic changes to debugging printfs.  Got rid of stale debug code.
 * (klh for mjl)
 * 
 * Revision 3.2  92/08/17  13:04:37  mjl
 * For DTYPE_VNODE files, don't increment file port sequence number until the
 * vnode lock is held.  Don't special case FIFO filesystem vnodes here, that
 * will be dealt with at a lower level.
 * 
 * Revision 3.1  92/08/08  01:45:06  jdh
 * policy moving structure related routines rewritten by mjl
 * mjl added fifo support (commented out in this version)
 * jdh added unix domain sockets support -- code to allow a
 * connecting socket to move when all other threads are finished -- jdh
 * 
 * Revision 3.0  92/06/11  15:59:47  mjl
 * Support subroutines for UNIX domain socket/pipe/FIFO relocation.
 * 
 */


#include <sys/param.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <sys/socketvar.h>
#include <sys/file.h>
#include <uxkern/syscall_subr.h>
#include <net/net_malloc.h>

#include <tnc/reloc.h>
#include <tnc/un.h>
#include <tnc/un_ff.h>

#define	Register	/* register */
#define Static		/* static */


/*
 *  This hook is placed in file_port_increment_seqno(), a routine
 *  called at the end of *every* file port operation.  At this
 *  point fp->f_seqno has been incremented, the lock pointed to
 *  by fp->f_seqno_lock is still held, and FP_LOCK() is held.
 *
 *  Examine the file structure's seqno to determine whether the
 *  current thread ought to perform a relocation.  Called from
 *  file_port_increment_seqno() after fp->f_seqno has been incremented
 *  and while the FP_LOCK() is still held.
 */
int
tnc_should_relocate(
	struct file	*fp)
{
	struct socket	*so;
	struct vnode	*vp;
	int		flags;

	/*
	 *  Current thread might have closed the file.  No problem.
	 */
	if (!fp->f_data)
		return (FALSE);

	switch ( fp->f_type ) {

	case DTYPE_SOCKET:
		so = (struct socket *)fp->f_data;
		if (IS_RELOCATING(so)) {
			if ( ! WAITING_ON_FP_SEQNO(so) ) {
				return (TRUE);
			} else {
				/*
				 * A specific thread wants to handle
				 * the relocate.
				 *
				 * If this is the second to last
				 * thread about to exit (there's only
				 * one outstanding operation on this
				 * port), then unlock the mutex that
				 * the other thread is waiting to relock
				 * so this other thread recheck the
				 * seqno and do the relocate.
				 */
				if( seqno_from_port(fp) == (fp->f_seqno + 1) )
					/*
					 * assumption -- it is ok to
					 * unlock a mutex lock that
					 * may not actually be locked.
					 */
					mutex_unlock(&so->vs_seqno_mutex);
			}
		}
		break;

	case DTYPE_VNODE:
		vp = (struct vnode *)fp->f_data;
		VN_LOCK(vp);
		if ( (vp->v_flag & VRELOCATING) &&
		      (--V_FACTIVE(vp) == 0) ) {
			VN_UNLOCK(vp);
			return (TRUE);
		}
		VN_UNLOCK(vp);
		break;
	}
	return (FALSE);
}


/*
 *  Attempt to relocate the file and other related files.  
 *  If the relocation operation succeeds, the final call to
 *  vrele() will deallocate this vnode.
 */
int
tnc_relocate_try(
	struct file	*fp)
{
	switch (fp->f_type) {

	case DTYPE_VNODE:
	    {
		Register struct vnode *vp = (struct vnode *)fp->f_data;

		ASSERT(vp->v_type == VFIFO);
		VN_LOCK(vp);
		un_ff_relocate(vp, fp); 
		VN_UNLOCK(vp);
	    }
	    break;

	case DTYPE_SOCKET:
	    {
		Register struct socket *so = (struct socket *)fp->f_data;

		SOCKET_LOCK(so);
		if ( VSOP_RELOCATE(so) == FALSE )
			SOCKET_UNLOCK(so);
	    }
	    break;
	}
}


#ifdef	STALE_CODE
/*
 *  When a socket or FIFO is to be relocated, this checks to ensure
 *  that the last active thread does the work.
 *  If the relocation routine returns TRUE, no unlocking should be
 *  performed---the locked data has migrated to a new node and there
 *  is no longer anything to unlock.
 */
int
tnc_fsvr_end_op(
	struct file	*fp)
{
	struct socket	*so;
	struct vnode	*vp;
	mach_port_t	port;

	if(!fp->f_data) return (FALSE);

	switch ( fp->f_type ) {
	case DTYPE_SOCKET:
		so = (struct socket *)fp->f_data;
		SOCKET_LOCK(so);
		if ( IS_RELOCATING(so) ) {
			if( WAITING_ON_FP_SEQNO(so) ) {
				/*
				 * A specific thread wants to handle
				 * the relocate.
				 *
				 * If this is the second to last
				 * thread about to exit (there's only
				 * one outstanding operation on this
				 * port), then unlock the mutex that
				 * the other thread is waiting to relock
				 * so this other thread recheck the
				 * seqno and do the relocate.
				 */
				if( seqno_from_port(fp) == (fp->f_seqno + 1) )
					/*
					 * assumption -- it is ok to
					 * unlock a mutex lock that
					 * may not actually be locked.
					 */
					mutex_unlock(&so->vs_seqno_mutex);
			} else if( VSOP_RELOCATE(so) ) {
				/*
				 * Socket was successfully cloned, all
				 * related ports have been relocated,
				 * the existence reference count on
				 * the file port was removed.  (This
				 * means only the current operation's
				 * reference count is outstanding, as
				 * we know all other operations have
				 * completed.  This operation's
				 * reference count will be removed by
				 * the routine that called this
				 * routine.)
				 *
				 * Socket and file port locks were
				 * unlocked in the relocate call, so
				 * no need to unlock here.
				 */
				UNDEBUG(U_RELOC,
				    ("tnc_fsvr_end_op: relocation done\n"));
				return (TRUE);	/* Relocated---don't unlock. */
			}
		}
		SOCKET_UNLOCK(so);
		break;
	case DTYPE_VNODE:
		vp = (struct vnode *)fp->f_data;
		VN_LOCK(vp);
		if ( vp->v_flag & VRELOCATING ) {
			if ( --V_FACTIVE(vp) == 0 &&
			     un_ff_relocate(vp, fp) == ESUCCESS )
				return (TRUE);	/* Relocated---don't unlock. */
		}
		VN_UNLOCK(vp);
		break;
	}
	return (FALSE);
}
#endif	/* STALE_CODE */


void
un_policy_init(
	policy_state_t	*ps)
{
	ASSERT(ps);

	ps->ps_reader = (pid_t)0;
	ps->ps_rdrnode = INVALID_NODE;
	ps->ps_newnode = INVALID_NODE;
	ps->ps_refcnt = 1;
	/* ps->ps_flag remains as before */
}


/*
 *  Allocate space for relocation policy state, or initialize an
 *  uninitialized policy state, or bump the refcount on an initialized
 *  policy state structure.
 *
 *  Routines wishing to allocate their own policy state structure
 *  should set the ps_reader field to zero and then call this routine
 *  to initialize the rest of the structure.
 */
int
un_policy_alloc(
	policy_state_t **aps)
{
	policy_state_t *ps;

	if ( aps == NULL )
		return (EINVAL);

	if ( (ps = *aps) == NULL ) {
		/*
		 *  Allocate space and mark as uninitialized.
		 */
		NET_MALLOC(ps, policy_state_t *, sizeof(policy_state_t),
			   M_TEMP, M_WAITOK);
		ps->ps_reader = (pid_t)0;
		ps->ps_flag = PLCY_HEAP;
	}

	if ( ps->ps_reader == (pid_t)0 ) {
		/*
		 *  If uninitialized, init the remaining fields.
		 */
		un_policy_init(ps);
	} else {
		/*
		 *  Bump the ref count on the valid policy state.
		 */
		ASSERT(ps->ps_refcnt > 0);
		ps->ps_refcnt++;
	}

	*aps = ps;
	return (ESUCCESS);
}


int
un_policy_free(
	policy_state_t **aps)
{
	policy_state_t *ps;

	if ( aps == NULL || (ps = *aps) == NULL )
		return (EINVAL);

	ASSERT(ps->ps_refcnt > 0);
	if ( --ps->ps_refcnt == 0 ) {
		if ( ps->ps_flag & PLCY_HEAP ) {
			NET_FREE(ps, M_TEMP);
		}
		*aps = NULL;
	}
	return (ESUCCESS);
}


/*
 *  Relocation policy routine.  Decide if pid's migration to node
 *  should cause this socket to change its "storage node".  If so,
 *  this routine stores the new storage node with the socket for
 *  later, when the actual relocation is performed.
 */
int
un_policy(
	policy_state_t *ps,
	pid_t pid,
	node_t node)
{
	/*
	 * After a connect() but before an accept() call, two sockets
	 * are connected, but not ready to move anywhere.  Not until
	 * the accept() takes place is the connected pair freed from
	 * the listening socket's queue.  This can be detected by
	 * checking that no u_sock_t structure is allocated, because
	 * this is not done until the accept() has been successful.
	 */
	if( ps == NULL )
		return FALSE;

	PRINT_POLICY(ps,"entry");

	if ( ps->ps_reader == 0 ) {
		ps->ps_reader = pid;
		if ( ps->ps_rdrnode == INVALID_NODE ) {
			/*
			 *  This is the very first migrate notification
			 *  for this socket.  Set the reader's current node,
			 *  and check whether this node is the same node
			 *  where the object was created.
			 */
			ps->ps_rdrnode = this_node;
			if ( node == this_node ) {
				/*
				 *  First notification, on object creation
				 *  node.  Stay here.
				 */
				ps->ps_newnode = INVALID_NODE;
				PRINT_POLICY(ps,"1st ntfy, post-create <F>");
				return FALSE;
			} else {
				/*
				 *  Process migrated since object was created,
				 *  so follow it.
				 */
				ps->ps_newnode = node;
				PRINT_POLICY(ps,"1st ntfy <T>");
				return TRUE;
			}
			/*NOTREACHED*/
		} else if ( ps->ps_rdrnode == node ) {
			/*
			 *  A new primary reader, but we're already
			 *  located where we want to be.
			 */
			ps->ps_newnode = INVALID_NODE;
			PRINT_POLICY(ps,"new rdr <F>");
			return FALSE;
		} else {
			/*
			 *  A new primary reader.  We want to
			 *  relocate to hir node.
			 */
			ps->ps_newnode = node;
			PRINT_POLICY(ps,"new rdr <T>");
			return TRUE;
		}
		/*NOTREACHED*/
	}
	if ( pid == ps->ps_reader && node != ps->ps_rdrnode ) {
		if ( node == INVALID_NODE ) {
			/*
			 *  Primary reader exited!  Make sure next
			 *  notification will produce a new primary
			 *  reader, and don't move anywhere yet.
			 *
			 *  XXX Still have to arrange for this flavor
			 *  of ..._policy() call to be made!
			 */
			ps->ps_reader = 0;
			ps->ps_newnode = INVALID_NODE;
			PRINT_POLICY(ps,"rdr exited <F>");
			return FALSE;
		} else {
			/*
			 *  Primary reader migrated!
			 */
			ps->ps_newnode = node;
			PRINT_POLICY(ps,"rdr migrated <T>");
			return TRUE;
		}
	}

	/*
	 *  If we get here, then we must not give a hoot about this
	 *  migration.
	 */
	PRINT_POLICY(ps,"!rdr or already there <F>");
	return FALSE;

#undef PRINT_POLICY
}


/*
 *  Wake up everybody and anybody who could possibly be waiting on
 *  any channel associated with this socket.
 */
int
un_sowakeall(so)
	struct socket *so;
{
	LOCK_ASSERT("un_sowakeall", SOCKET_ISLOCKED(so));

	/*
	 *  XXX We don't yet handle these XTI wakeup routines.
	 */
	ASSERT(so->so_rcv.sb_wakeup == NULL && so->so_snd.sb_wakeup == NULL);

	wakeup((caddr_t)&so->so_timeo);

	wakeup((caddr_t)&so->so_snd.sb_flags);
	wakeup((caddr_t)&so->so_snd.sb_cc);

	wakeup((caddr_t)&so->so_rcv.sb_flags);
	wakeup((caddr_t)&so->so_rcv.sb_cc);
}
