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
 * $Log: un_ff_ops.c,v $
 * Revision 1.14  1995/02/01  22:05:18  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.13  1995/01/18  19:59:12  slk
 * Disabling FIFO relocation using the built in NO_PIPE_RELOC sometimes
 * causes process to hang.  Stop FIFO relocation earlier on in the file
 * by circumventing all the FIFO relocation setup code.
 *  Reviewer(s): Mike Leibensperger, Johannes
 *  Risk: Low
 *  Benefit or PTS #: 10881
 *  Testing: read and write to FIFO on filesystem node and remote node.
 *  Module(s):
 *
 * Revision 1.12  1994/12/17  00:00:12  slk
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
 * Revision 1.11  1994/11/18  20:44:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1993/07/29  21:54:00  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server slowdown.
 *
 * Revision 1.9  1993/07/14  18:35:59  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:51  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/05/06  19:27:51  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:59  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.7  1993/04/03  03:09:53  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.6  1993/03/29  18:26:39  cfj
 * Merge with T9.
 *
 * Revision 1.5.4.1  1993/03/29  18:17:32  cfj
 * More ux_server_thread_block/unblocking from Locus.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:41  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.5  1993/01/22  15:38:49  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 1.4  1993/01/15  02:03:16  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:31  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.8  93/07/27  11:31:17  mjl
 * [Bug #0308] The seqno mutex shared by all file structs attached to a FIFO is
 * replaced by a r/w lock in accordance with the locking hierarchy (i.e. first
 * take r/w locks, then take mutexes, then take simple spin locks).  Further,
 * the file struct pointer formerly locked on entry to un_ff_relocate() is no
 * longer locked at this point.
 * 
 * Revision 3.7  93/06/24  12:48:16  mjl
 * [LCC bug 0229] Make sure "zombied" vnode and file structs are properly
 * cleaned up after FIFO relocation.  Clear FIFO seqno mutex to allow the
 * server thread doing the relocation to complete without hanging.
 * 
 * Revision 3.6  93/06/14  14:03:30  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Restructured the file marshalling loop for relocation
 * 
 * Revision 3.5  93/03/27  17:04:39  yazz
 * Added some ux_server_thread_blocking/unblocking() calls.
 *
 * Revision 3.4  93/01/12  13:53:44  mjl
 * Use an "extra syscall" number to allow syscall tracing with remote FIFO
 * death RPC.
 * 
 * Revision 3.3  92/12/10  17:17:07  mjl
 * Use new debug macros.
 * 
 * Revision 3.2  92/11/18  12:47:24  mjl
 * Initialize and use shared mutex when computing # of outstanding threads
 * across all file ports referrencing the FIFO.
 * 
 * Revision 3.1  92/09/28  13:30:37  klh
 * Initial working FIFO relocation implementation. (klh for mjl).
 * 
 * Revision 3.0  92/08/17  12:44:01  mjl
 * FIFO relocation hooks, similar to virtual socket operations but these
 * are hooks called from the FIFO vnode ops.
 * 
 */

/*
 *  Hooks for TNC distributed FIFOs.
 *
 *  These routines are similar in spirit to virtual socket operations,
 *  but because we relocate FIFOs at the vnode level rather than the
 *  socket level, we simply call these routines in the appropriate
 *  places in the fifo_vnops routines.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/time.h>
#include <sys/vnode.h>
#include <sys/protosw.h>
#include <sys/user.h>
#include <sys/errno.h>
#include <sys/file.h>

#include <uxkern/syscall_subr.h>
#include <tnc/un_ff.h>

#ifdef	UN_FF_DEBUG
int	unffdebug	= 0; 
#endif

extern node_t		this_node;
extern struct socket	*sopartner(struct socket *);

/*
 *  Hook for VFIFO vnode open operation.
 *
 *  Called from fifo_open() just before it returns.  If fifo_open()
 *  succeeded, we add the opening file pointer to the chain, ensuring
 *  that future FIFO relocation operations will take this file pointer
 *  along.
 */
int
un_ff_open(
	struct vnode	*vp,
	int		mode,
	int		error)
{
	uthread_t	uth;
	struct file	*fp;
	int		refcnt;

	if (error != ESUCCESS)
		return;

	uth = current_thread();
	fp = (struct file *) uth->uu_opn_filep;

	VN_LOCK(vp);

	/*
	 *  Sanity checks:
	 *  - this is a FIFO vnode,
	 *  - capable of storing user data
	 *  - no unexpected I/O modes (very paranoid)
	 */
	ASSERT(vp->v_type == VFIFO &&
	       V_STRG(vp) &&
	       vp->v_socket &&
	       (vp->v_iomode == VIO_BUF || vp->v_iomode == VIO_REQNOTIFY));

	/*
	 *  If no TNC-specific data has yet been allocated, do that
	 *  now.  Deallocation occurs on the final vrele() call.
	 */
	if ( vp->v_tncdata == NULL ) {
		vp->v_tncdata = (caddr_t) malloc(sizeof(un_ff_t));
		un_policy_init(V_PSTATE(vp));
		queue_init(V_FILEQ(vp));
		V_XFLAG(vp) = VX_STRG|VX_FSYS;
		V_FCOUNT(vp) = 0;
		V_FACTIVE(vp) = 0;
		V_NRPORT(vp) = MACH_PORT_NULL;
		V_FSPORT(vp) = MACH_PORT_NULL;
		V_FSNODE(vp) = this_node;
		INIT_SAVEPORT(vp);
		VN_SEQNO_LOCK_INIT(vp);
	}

	/*
	 *  XXX Temporarily, we do some of the work of "death-of-primary-
	 *  reader" detection here.  If the primary reader died, the vnode
	 *  may still be cached.  Reinitialize the policy state if this
	 *  is a new reader.
	 */
	if ( (mode & FREAD) && vp->v_rdcnt == 1 ) {
		UNFFDEBUG(U_POLICY,("un_ff_open: New reader!\n"));
		un_policy_init(V_PSTATE(vp));
	}

	/*
	 *  Here we do the real work of this routine: setting the I/O
	 *  mode to let the emulator know this file port should get
	 *  migrate notification, and adding the file to the vnode's
	 *  chain so that it will relocate along with the vnode.
	 */
	vp->v_iomode = VIO_REQNOTIFY;
	VN_FILEQ_ADD(vp, fp);

	VN_UNLOCK(vp);

       UNFFDEBUG((~0), ("un_ff_open: fp=0x%x vp=0x%x tncdata=0x%x\n",
                       fp, vp, vp->v_tncdata));
}


/*
 *  Hook for VFIFO vnode close operation, called from start of
 *  fifo_close().
 */
int
un_ff_close(
	struct vnode	*vp)
{
	uthread_t	uth;
	struct file	*fp;
	mach_port_t	fs_vnode_port;
	kern_return_t	kr;
	int		status;
	mach_port_seqno_t seqno;

	/*
	 *  Closef() has to store the closing file pointer with the
	 *  thread data, so that we can remove it from the vnode chain.
	 */
	uth = current_thread();
	fp = (struct file *) uth->uu_opn_filep;

	VN_LOCK(vp);

	/*
	 *  If fifo_close() (and hence this routine) is being called
	 *  because of an error in fifo_open(), it is possible for
	 *  there to be no attached TNC data.  This is not a problem.
	 */
	if (vp->v_tncdata == NULL) {
		VN_UNLOCK(vp);
		return;
	}

	VN_FILEQ_REMOVE(vp, fp);

	/*
	 *  If there are other files referrencing the vnode or if
	 *  there is no remote storage vnode, we are done.
	 */
	if ( ! queue_empty(V_FILEQ(vp)) || V_FSYS(vp) ) {
		VN_UNLOCK(vp);
		return;
	}

	/*
	 *  The fp was the last file on the chain and this is a remote
	 *  storage vnode.  If the vnode is being closed because it is
	 *  the "zombie" of a vnode that has just migrated away, we
	 *  are done.  But if this was the last close of a remote
	 *  storage vnode, we want to continue on and destroy it so
	 *  that the FIFO can go back to being a simple, one-vnode
	 *  FIFO.  In other words, there is an implicit relocation
	 *  back to the filesystem site on the last close.
	 */
	if (V_XFLAG(vp) & VX_ZOMBIE) {
		VN_UNLOCK(vp);
		return;
	}

	/*
	 *  The sum of reader and writer counts should be one, i.e.
	 *  only the current thread should be counted.  More than one
	 *  implies an outstanding "half-open", that is, as the
	 *  current thread was doing a close(), another server thread
	 *  did an open() and is now blocked in fifo_open() waiting
	 *  for someone else to open the opposite end of the FIFO.
	 *
	 *  Rather than trying to wake and restart these half-open
	 *  threads, we just bail out and let the FIFO remain split
	 *  across the storage and filesystem nodes.
	 */
	if ( (vp->v_wrcnt + vp->v_rdcnt) > 1 ) {
		UNFFSTAT(halfopen);
		UNFFDEBUG(U_HALFOPEN,
			  ("un_ff_close: half opens: rd %d wr %d\n",
			   vp->v_rdcnt, vp->v_wrcnt));
		VN_UNLOCK(vp);
		return;
	}

	/*
	 *  Last close of a remote storage vnode.  Any remaining data
	 *  is to be discarded (cf. OSF AES Rev. A).  Only the storage
	 *  vnode's receive-right and corresponding make-send count
	 *  need to be sent back to the filesystem vnode.
	 */
	UNFFDEBUG(U_INFO, ("un_ff_close: remote fifo death, vp=0x%x\n", vp));

	/*
	 *  We have to beware of races with redirected namei() lookups
	 *  here!!!  Remove the vnode port from the server port set,
	 *  redirect namei() operations to that port, and then wait
	 *  for any outstanding ones to complete.
	 *
	 *  NB a side effect of calling FIFO_SET_REDIRECT() on a
	 *  remote storage vnode is that an extra send-right is
	 *  created, i.e. in addition to the one held by the
	 *  filesystem vnode.  The extra send-right is deallocated
	 *  later in this routine.
	 */
	ASSERT(V_NRPORT(vp) == MACH_PORT_NULL);
	ux_server_remove_port(vp);
	vp->v_flag |= VRELOCATING;	/* prevents ux_server_add_port */
	FIFO_SET_REDIRECT(vp, KEEPNAME);

	/*
	 *  Now we must wait for any outstanding namei() lookups to
	 *  finish (i.e. to be redirected).
	 *
	 *  XXX This is difficult because unlike file port sequence
	 *  numbers, vnode sequence numbers are incremented when the
	 *  operation begins rather than when it completes.  I'm
	 *  afraid that for now the best we can do is suspend the
	 *  thread briefly and hope that the last namei() redirection
	 *  completes in the meantime.
	 *
	 *  When all is done, there should be only two vnode
	 *  references left, one for the port and one for this current
	 *  executing thread's operation.
	 */
	seqno = seqno_from_port((mach_port_t)vp);
	while (seqno != vp->v_seqno) {
		vp->v_flag |= VNMSWAIT;
		assert_wait((int)&vp->v_seqno, FALSE);
		VN_UNLOCK(vp);
		thread_block();
		VN_LOCK(vp);
	}
	/* There is intentionally no thread_wakeup call for this. */
	assert_wait((int)&vp->v_tncdata, FALSE);
	thread_set_timeout(REMOTE_FIFO_DEATH_DELAY_IN_TICKS);
	VN_UNLOCK(vp);
	thread_block();
	VN_LOCK(vp);
	/* XXX Possibly we could wait for this condition using our */
	/* VXREFWAIT flag! */
	ASSERT(vp->v_usecount == 2);

	/*
	 *  Now at last we are ready to destroy this remote storage
	 *  vnode.  First we get rid of the send-right manufactured
	 *  above via FIFO_SET_REDIRECT(), then ship the the
	 *  receive-right, remaining fs send-right, and mscount back in
	 *  an RPC.
	 *
	 *  It would be nice if we could do an SGD_TYPE_FIFO_REJOIN
	 *  relocation here rather than calling a special RPC, but
	 *  unfortunately at this point there are no remaining file
	 *  ports referrencing the vnode, so the tnc_fsvr_end_op()
	 *  hook cannot be called to trigger the relocation.
	 */
	ASSERT(V_NRPORT(vp) == (mach_port_t)vp);
	kr = mach_port_mod_refs(mach_task_self(), (mach_port_t)vp,
				MACH_PORT_RIGHT_SEND, -1);
	if (kr != KERN_SUCCESS)
		panic("un_ff_close: send-right dealloc: kr 0x%x", kr);

	UNFFPORTINFO(V_NRPORT(vp), "ss port before rfd");

	ux_server_thread_blocking();
	kr = cli_tnc_remote_fifo_death(V_FSPORT(vp),	/* fs sright */
				       uth->uu_procp->p_cred,
				       V_FSPORT(vp),	/* fs sright */
				       (mach_port_t)vp,	/* ss rright */
				       vp->v_mscount,	/* ss mscount */
				       &status);
	ux_server_thread_unblocking();
	if (kr != KERN_SUCCESS)
		panic("un_ff_close: remote death RPC: kr 0x%x", kr);
	if (status != ESUCCESS)
		panic("cli_tnc_remote_fifo_death returned %d", status);

	/* The vnode port is now gone. */
	FIFO_PORT_GONE(vp);

	/*
	 *  Lastly we deallocate the local data structures.  Leave the
	 *  vnode with v_usecount of 1, so that the vrele() call in
	 *  vn_close() will free it.
	 */
	VN_UNLOCK(vp);
	un_ff_set_usecount(vp, 1, FALSE /* vp is not locked */);
}


/*
 *  Server side of last close on a FIFO remote storage vnode.
 *  The fs_vnode_port, which is temporary and exists only while there
 *  is a remote storage vnode, is destroyed.  The ss_vnode_port and
 *  its make-send count are attached to the vnode, which is now both
 *  filesystem site and storage site vnode.
 */
kern_return_t
svr_tnc_remote_fifo_death(
	mach_port_t		fs_vnode_port,	/* filesys. site vnode port */
	mach_port_t		creds_port,	/* credentials */
	mach_port_t		fs_sright,	/* sright for above port! */
	mach_port_t		ss_vnode_port,	/* storage site vnode port */
	mach_port_mscount_t	ss_mscount,	/* mscount for ss port */
	int			*status)	/* return status */
{
	struct vnode	*vp;
	kern_return_t	kr;
	int		wakeup = 0;
	int		vrefs;
	int		error;
	struct uthread	*uth = current_thread();

	UNFFDEBUG(U_NOTIFY,
		  ("svr_tnc_remote_fifo_death(fsp=0x%x, ssp=0x%x, sscnt=%d)\n",
		   fs_vnode_port, ss_vnode_port, ss_mscount));

	uth->uu_syscode = 2012;
	error = start_fsvrmisc_op_with_proc(fs_vnode_port, creds_port);
	if (error)
		panic("svr_tnc_remote_fifo_death: start routine: %d",
		      error);

	ASSERT(fs_sright == fs_vnode_port);

	*status = ESUCCESS;
	PORT_TO_VNODE_LOOKUP(fs_vnode_port, vp);
	if (vp == NULL)
		panic("svr_remote_fifo_death: fs vnode port");
	VN_LOCK(vp);

	/*
	 *  Sanity checks:
	 *  - fs vnode better be a FIFO
	 *  - with TNC data area attached
	 *  - fs vnode should be redirecting namei() lookups...
	 *  - ...to the port given to us as ss_vnode_port.
	 *  - only ss vnode should have send-right for fs vnode port,
	 *	and indeed only one should ever have been manufactured
	 *  - no new send-rights should have been created at the ss node
	 *  - nobody should be calling get_vnode_port() for fs vnode port
	 *
	 *  The v_usecount of a file system vnode becomes stale when the
	 *  FIFO storage relocates away (since processes may open or close
	 *  the FIFO at its storage site).  Thus the value of vp->v_usecount
	 *  is stale here.
	 */
	ASSERT(vp->v_type == VFIFO);
	ASSERT(vp->v_tncdata != NULL);
	ASSERT(vp->v_flag & VREDIRECT);
	ASSERT(V_NRPORT(vp) == ss_vnode_port);
	ASSERT(vp->v_mscount == 1);
	ASSERT((vp->v_flag & (VPORTLOCK|VPORTWAIT)) == 0);

	/*
	 *  Get rid of fs vnode port.  We don't need to worry about
	 *  messages queued in it, because its only real use is as a
	 *  handle for this RPC routine.
	 */
	UNFFPORTINFO((mach_port_t)vp, "destroying fs port");
	destroy_fifo_port(vp);

	/*
	 *  Now we want to destroy the ss vnode port as well.  It's
	 *  related info is in the V_SAVEPORT(vp) area, it is now
	 *  named V_NRPORT(vp), and there is one receive-right and one
	 *  send-right.  We want to reinstall it as the vnode's true
	 *  port (so that no-more-senders works properly for it) and
	 *  then delete it as we did the fs port.
	 *
	 *  We continue to use the VREDIRECT bit to ward off any vnode
	 *  access attempts here---if V_NRPORT(vp) is MACH_PORT_NULL
	 *  then any namei()'s that get through while the vnode is
	 *  unlocked will block waiting for it.
	 *
	 *  NB this vnode lost a v_usecount because of that port
	 *  deletion, so we must adjust for it here when another port
	 *  is attached to the vnode.
	 *
	 *  XXX It might be desirable to leave the storage port around
	 *  to cut down on start-up overhead.  However, this means the
	 *  vnode would never be VOP_INACTIVE()ated.  More thought is
	 *  required, but it's probably best to just destroy the
	 *  storage port as we do currently.
	 */
	ASSERT(vp->v_flag & VREDIRECT);
	V_NRPORT(vp) = MACH_PORT_NULL;
	un_ff_resync_saved_port(vp, ss_mscount);
	UNFFDEBUG(U_RELOC,
	    ("svr_tnc_remote_fifo_death: restore saved port vp=0x%x\n", vp));
	un_ff_restore_saved_port(vp);
	vp->v_usecount++;

	/*
	 *  The former V_NRPORT(vp) has been installed as vp's true
	 *  vnode port.  Because VREDIRECT is on and V_NRPORT(vp) is
	 *  MACH_PORT_NULL, when destroy_fifo_port() reattaches the
	 *  port to the server port set to allow new namei() requests
	 *  to drain, they will immediately block with VX_NRWAIT set.
	 *  When the port is gone we cancel the redirection and
	 *  unblock any namei() requests.
	 *
	 *  XXX In case our assumption that only this TNC FIFO code
	 *  will ever create a vnode port for a VFIFO vnode proves
	 *  false, here is where we would test the VX_HADPORT, and if
	 *  set we would not destroy this port.
	 */
	UNFFPORTINFO((mach_port_t)vp, "destroy returned ss port");
	destroy_fifo_port(vp);
	vp->v_flag &= ~VREDIRECT;
	V_XFLAG(vp) |= VX_FSYS|VX_STRG;
	wakeup = V_XFLAG(vp) & VX_NRWAIT;
	vrefs = vp->v_usecount;
	un_policy_init(V_PSTATE(vp));	/* In case namei() wins race (below) */
	VN_UNLOCK(vp);

	/*
	 *  Give any queued namei() calls a chance to reference the vnode
	 *  before we get rid of known references.
	 */
	if (wakeup)
		thread_wakeup(&V_NRPORT(vp));

	/*
	 *  Regardless of whether any namei() requests were blocked
	 *  waiting on VX_NRWAIT, we will still delete all vnode
	 *  references we knew about.  These VRELE() calls race against
	 *  any namei() threads awakened above.
	 */
	while (vrefs--)
		vrele(vp);

	end_fsvrmisc_op_with_proc(MACH_PORT_NULL, creds_port, ESUCCESS);

	return (KERN_SUCCESS);
}


/*
 *  This routine is the eventual recipient of the emulator's
 *  fsvr_report_migrate() RPC on FIFO file ports.  It calls a policy
 *  routine to decide whether or not to relocate the FIFO.  If
 *  relocation is called for, it prepares the FIFO for relocation by
 *  detaching its vnode port and all associated file ports, waking all
 *  threads sleeping on these data structures, and requeueing any
 *  delayed selects.  The actual relocation is done by the last active
 *  thread; see tnc_fsvr_end_op() in reloc_subr.c and un_ff_relocate()
 *  below.
 *
 *  XXX A comprehensive solution to the problem of half-opens might
 *  have half-open sleepers awakened from here, along with all the
 *  other wakeup calls done here.
 */
int
un_ff_notify(
	struct vnode	*vp,
	pid_t		pid,
	node_t		node)
{
	int		error;
	struct socket	*wso, *rso;
	policy_state_t	*ps;
	struct file	*fp;
	queue_t		vq;
	kern_return_t	kr;
	mach_port_t	port;
	int		active_file_count;

	UNFFDEBUG(U_NOTIFY,
		  ("un_ff_notify(vp=0x%x, pid=%d, node=%d)\n", vp, pid, node));

	VN_LOCK(vp);

	/*
	 *  Only storage-capable FIFO vnodes should receive migrate
	 *  notifications.
	 */
	ASSERT(vp->v_tncdata && V_STRG(vp));

	if ( vp->v_flag & VRELOCATING )
		/*
		 *  Another thread has already begun relocating this FIFO,
		 *  so bail out.  Future implementations might attempt to
		 *  change the destination node up until some commit
		 *  point, but the only case where this would be
		 *  useful is when a remote storage site death races
		 *  against a primary reader open().  Not too likely.
		 *
		 *  See also the comment in un_pp_notify().
		 */
		goto out;

	if ( TRUE ) 
		/* 
		 * This puts an end to FIFO relocation before alot of
		 * setup is done.  When FIFO relocation is too be 
		 * reinstated the above comparison should be replace with,
		 * if ( un_policy(V_PSTATE(vp), pid, node) == FALSE )
		 */
		/*
		 *  Stay where we are, how boring...
		 */
		goto out;

	if ( vp->v_exlockc || vp->v_shlockc ) {
		/*
		 *  XXX Relocating FIFOs with outstanding file locks
		 *  is not yet implemented.
		 */
		UNFFDEBUG((~0),("un_ff_notify: vp 0x%x has file locks\n"));
		goto out;
	}

	UNFFDEBUG(U_NOTIFY,("un_ff_notify: vp 0x%x relocating!\n", vp));
	vp->v_flag |= VRELOCATING;

	/*
	 *  Cease servicing the vnode port, if one exists at this point.
	 */
	if (vp->v_magic == V_MAGIC)
		ux_server_remove_port(vp);

	/*
	 *  Turn on namei() redirection.  Subsequent threads doing
	 *  namei() lookups will get EREMOTE and will eventually be
	 *  sent on to the storage site vnode.  The CHNGNAME flag
	 *  specifies that the port be saved in the V_SAVEPORT(vp)
	 *  area under a different port name.
	 */
	FIFO_SET_REDIRECT(vp, CHNGNAME);

	/*
	 *  Cease servicing the file ports.  As each port goes out of
	 *  service, record the discrepancy between the recorded
	 *  and actual port sequence numbers.  This lets us detect the
	 *  last file operation thread.
	 *
	 *  Mutual exclusion between this block of code and
	 *  file_port_increment_seqno() (which see) is achieved by
	 *  having all files on the chain share the same lock for
	 *  incrementing sequence numbers.
	 */
	VN_SEQNO_LOCK(vp);
	vq = V_FILEQ(vp);
	active_file_count = 0;
	fp = (struct file *) queue_first(vq);
	while ( ! queue_end(vq, (queue_entry_t)fp) ) {
		FP_LOCK(fp);
		ASSERT(fp->f_magic == F_MAGIC);
		ux_server_remove_port(fp);
		active_file_count += seqno_from_port(fp) - fp->f_seqno;
		FP_UNLOCK(fp);
		fp = (struct file *) queue_next(&fp->f_chain);
	}
	V_FACTIVE(vp) = active_file_count;
	VN_SEQNO_UNLOCK(vp);
	UNFFDEBUG(U_RELOC,("un_ff_notify: %d active file ops\n",
			    V_FACTIVE(vp)));

	/*
	 *  Mark both sockets to indicate they are being moved.
	 *  Any subsequent socket operations should result in
	 *  restarted syscalls.
	 */
	wso = vp->v_socket;
	ASSERT(wso);
	SOCKET_LOCK(wso);	/* locks BOTH connected sockets */
	rso = sopartner(wso);
	if ( rso == NULL )
		panic("un_ff_notify: rso");
	ASSERT(wso->so_lock == rso->so_lock);
	wso->vs_flags |= VS_RESTART;
	rso->vs_flags |= VS_RESTART;

	/*
	 *  Wake up blocked socket I/O
	 */
	un_sowakeall(wso);
	un_sowakeall(rso);

	/*
	 *  Requeue delayed select operations.  This saves us the
	 *  trouble of having to relocate the select queue entries
	 *  ourselves; they'll be queued within their respective file
	 *  ports and the microkernel will relocate them for us.
	 */
	SOCKBUF_LOCK(&wso->so_rcv);
	select_wakeup(&wso->so_rcv.sb_selq);
	SOCKBUF_UNLOCK(&wso->so_rcv);
	SOCKBUF_LOCK(&wso->so_snd);
	select_wakeup(&wso->so_snd.sb_selq);
	SOCKBUF_UNLOCK(&wso->so_snd);

	SOCKBUF_LOCK(&rso->so_rcv);
	select_wakeup(&rso->so_rcv.sb_selq);
	SOCKBUF_UNLOCK(&rso->so_rcv);
	SOCKBUF_LOCK(&rso->so_snd);
	select_wakeup(&rso->so_snd.sb_selq);
	SOCKBUF_UNLOCK(&rso->so_snd);

	/* Release socket pair's lock. */
	SOCKET_UNLOCK(wso);

	/*
	 *  File locking on FIFOs is permitted, so we must also
	 *  wake up anyone waiting on the file lock.
	 *
	 *  XXX Not yet implemented!!!  Placing hooks around mpsleep()
	 *  calls in vn_flock() is tricky, gotta do the right thing
	 *  with the flag bits and counts....
	 */
#ifdef	NOTDEF
	wakeup(&vp->v_shlockc);
	wakeup(&vp->v_exlockc);
#endif	/* NOTDEF */

	/*
 	 *  That's it.  The last server thread operating on these
	 *  ports will issue the relocation RPC.
	 */
out:
	VN_UNLOCK(vp);
	return (ESUCCESS);
}

/*
 *  Relocation operation, called from tnc_fsvr_end_op() hook with
 *  the vnode locked.
 *
 *  IF this is called from the last active thread, THEN
 *	IF my relocation attempt failed, THEN
 *	    reattach the vnode and file ports and continue service
 *	    on this node, and return ESUCCESS (indicating that
 *	    the caller should not unlock anything---there is nothing left
 *	    to unlock!)
 *	ELSE
 *	    return error != ESUCCESS (and the caller should unlock vnode
 *	    and file structures, since these stayed).
 *	FI
 *  FI
 *
 *  Note that the V_FACTIVE(vp) count has already been decremented for
 *  the current thread.
 */
int
un_ff_relocate(
	struct vnode	*vp,		/* locked VFIFO to relocate */
	struct file	*active_fp)	/* last active op on this fp */
{
	policy_state_t	*ps;
	int		file_count;
	int		i, error;
	struct file	*fp;
	queue_t		vq;
	struct socket	*wso, *rso;
	int		active_fp_was_on_chain;
	kern_return_t	kr;

	ASSERT(vp->v_type == VFIFO &&
	  (vp->v_flag & (VREDIRECT|VRELOCATING)) == (VREDIRECT|VRELOCATING) &&
	       vp->v_tncdata != NULL &&
	       V_STRG(vp));
	ASSERT(V_NRPORT(vp) == V_SAVEPORT(vp)->pi_name);

	UNFFDEBUG(U_RELOC,
		  ("un_ff_relocate(0x%x): use %d, wr %d, rd %d, afo %d\n",
		   vp,vp->v_usecount,vp->v_wrcnt,vp->v_rdcnt,V_FACTIVE(vp)));

	/*
	 *  If we are not the last active file operation, we should
	 *  not even be here!
	 */
	if ( V_FACTIVE(vp) != 0 )
		panic("un_ff_relocate: %d active file ops", V_FACTIVE(vp));

	/*
	 *  Now we must wait for any "non-fileport" operation that
	 *  took a vref before we could set the VRELOCATING flag.
	 *  Unfortunately, this requires that we block this thread
	 *  until this "extra" vref is released.  This is because
	 *  there is no single place, other than in vrele(), where we
	 *  can detect the completion of such operations, and I wanted
	 *  to keep vrele() changes to a minimum.  Hopefully this
	 *  situation won't happen too often.
	 *
	 *  When all "non-file" ops have finished, there should be one
	 *  v_usecount ref for each file on the chain, plus one for
	 *  for the vnode port.
	 */
	if ( vp->v_usecount > V_FCOUNT(vp) + 1 ) {
		UNFFDEBUG(U_RELOC,
			  ("un_ff_reloc: extra vrefs: use %d, fcnt %d\n",
			   vp->v_usecount, V_FCOUNT(vp)));
		UNFFSTAT(xrefwait);
		vp->v_usecount -= V_FCOUNT(vp) + 1;
		vp->v_flag |= VXREFWAIT;
		while ( vp->v_flag & VXREFWAIT ) {
			assert_wait(&V_FCOUNT(vp), TRUE);
			VN_UNLOCK(vp);
			thread_block();
			VN_LOCK(vp);
		}
	}

	/* One ref for each attached file, plus one for the port. */
	ASSERT(vp->v_usecount == V_FCOUNT(vp) + 1);

	/* Attempt the FIFO relocation.... */

	error = un_ff_really_relocate(vp);

	/*
	 *  Whether relocation succeeded or not, we'll still need to
	 *  clear the socket restart flags so that we can use virtual
	 *  socket ops again, either to reattach or to deallocate the
	 *  sockets.
	 */
	wso = vp->v_socket;
	ASSERT(wso);
	SOCKET_LOCK(wso);
	rso = sopartner(wso);
	ASSERT(rso);
	wso->vs_flags &= ~VS_RESTART;
	rso->vs_flags &= ~VS_RESTART;
	SOCKET_UNLOCK(wso);
	
	/* On failure, reattach file and vnode ports. */
	if (error != ESUCCESS)
		goto reattach;

	/*
	 *  The FIFO storage vnode has successfully moved to the new node.
	 *  Zombies of data structures that moved along with it must be
	 *  deallocated.  First, clean up the zombie file structs.
	 */
	active_fp_was_on_chain = 0;
	vq = V_FILEQ(vp);
	fp = (struct file *) queue_first(vq);
	while ( ! queue_end(vq, (queue_entry_t)fp) ) {
		struct file *nfp;

		/* Get next fp before this fp is removed from the queue. */
		nfp = (struct file *) queue_next(&fp->f_chain);
		
		fp->f_magic = F_RELOC;
		if ( fp == active_fp ) {
			/*
			 *  This is the file pointer that the
			 *  current thread is operating on.  The last
			 *  f_count referrence goes away and the file
			 *  struct is deallocated when this thread
			 *  completes.
			 */
			ASSERT(fp->f_count == 2);
			active_fp_was_on_chain++;
			FP_UNREF(fp);
		} else {
			ASSERT(fp->f_count == 1);
			/*
			 *  For all but the locked_fp, this unref
			 *  causes a closef(fp) call, and the file
			 *  struct is removed from the chain as a side
			 *  effect (see un_ff_close()).  We need to
			 *  release the vnode lock because the close
			 *  code will want it.
			 */
			VN_UNLOCK(vp);
			FP_UNREF(fp);
			VN_LOCK(vp);
		}

		fp = nfp;
	}
	ASSERT(active_fp_was_on_chain);

	/*
	 *  If the storage vnode was splitting away from
	 *  the filesystem vnode, the filesystem vnode must be
	 *  preserved and its port must be reattached; otherwise
	 *  deallocate them.
	 */
	if ( V_FSYS(vp) ) {
		/*
		 *  A filesystem vnode, remaining behind after its
		 *  storage has moved away.  It remains referenced
		 *  only by its vnode port, a send-right for which
		 *  is held by the just-relocated storage vnode.
		 *
		 *  Clear that VRELOCATING flag to allow FIFO file
		 *  operations to proceed, and also unlock the vnode
		 *  seqno lock that was held by the now-relocated active_fp.
		 *
		 *  The v_usecount should be set to two, one for the
		 *  vnode port and one for the last active file struct
		 *  (active_fp).
		 */
		vp->v_flag &= ~VRELOCATING;
		VN_SEQNO_UNLOCK(vp);
		ASSERT(vp->v_usecount == 2);
		ux_server_add_port(vp);
	} else {
		/*
		 *  The remote storage vnode has relocated, and its
		 *  local incarnation can be deallocated.  Note we
		 *  have to leave one referrence, because vrele()
		 *  will be called at the completion of this thread
		 *  (via FP_UNREF() on the active_fp).
		 */
		FIFO_PORT_GONE(vp);
		un_ff_set_usecount(vp, 1, TRUE /*vp is already locked*/);
	}
	return (ESUCCESS);

reattach:
	/*
	 *  For whatever reason, the relocation failed.  Since we're
	 *  the last thread that knows anything about this vnode and
	 *  its file structures, we'd better put everything back the
	 *  way it was before....
	 *
	 *  Our caller expects these structures to be either locked or
	 *  gone, so we relock the vnode.
	 *
	 *  Note: The un_ff_really_relocate() routine takes care of
	 *  releasing (and possibly reacquiring) the FP_LOCK()
	 *  acquired in tnc_fsvr_end_op(), simply to cut down on the
	 *  number of file chain traversals.  It can check each file
	 *  struct for FP_LOCK_HOLDER(fp), unlocking prior to relocation
	 *  and relocking if relocation fails.
	 */

	UNFFDEBUG(U_RELOC,("un_ff_relocate: attempt got errno %d\n", error));

	/*
	 *  We aren't relocating anymore.  Clear namei() redirection
	 *  (it was set in un_ff_notify()).
	 */
	vp->v_flag &= ~VRELOCATING;
	FIFO_CLEAR_REDIRECT(vp);

	/*
	 *  If there was a saved port and the relocation failed, we have
	 *  to clean it up.  If only one ref on it, we created it ourselves
	 *  back in un_ff_notify(), so we can destroy it.
	 */
	if ( V_SAVEPORT(vp)->pi_name != MACH_PORT_NULL ) {
		UNFFDEBUG(U_RELOC,
			  ("un_ff_relocate(vp=0x%x): restore_saved\n", vp));
		un_ff_restore_saved_port(vp);
		if ( V_SREFS(vp) == 1 ) {
			UNFFDEBUG(U_RELOC, ("un_ff_relocate(vp=0x%x): reattach"
					    ": destroy restored port\n", vp));
			destroy_fifo_port(vp);
		} else {
			kr = mach_port_mod_refs(mach_task_self(),
						(mach_port_t)vp,
						MACH_PORT_RIGHT_SEND, -1);
			if ( kr != KERN_SUCCESS )
				panic("un_ff_relocate: reattach: m_p_mod_refs "
				      "vp=0x%x kr=0x%x\n", vp, kr);
		}
	}

	/*
	 * The f_count should still be 2
	 */
	ASSERT(active_fp->f_count == 2);

	/*
	 *  Reattach vnode and file ports.
	 */
	if ( vp->v_magic == V_MAGIC )
		ux_server_add_port(vp);
	vq = V_FILEQ(vp);
	fp = (struct file *) queue_first(vq);
	while ( ! queue_end(vq, (queue_entry_t)fp) ) {
		ASSERT(fp->f_magic == F_MAGIC);
		ux_server_add_port(fp);
		fp = (struct file *) queue_next(&fp->f_chain);
	}

	return (error);
}
