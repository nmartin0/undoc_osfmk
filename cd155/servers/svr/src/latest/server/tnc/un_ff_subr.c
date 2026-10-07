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
 * $Log: un_ff_subr.c,v $
 * Revision 1.12  1995/02/01  22:06:43  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/12/17  00:00:25  slk
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
 * Revision 1.10  1994/11/18  20:44:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/10/28  03:12:48  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.8  1993/07/14  18:36:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:50:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  19:28:19  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:08  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:09:58  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/25  23:26:32  cfj
 * T9 Merge.
 *
 * Revision 1.4.4.1  1993/03/24  23:42:41  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:50  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:23  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:39  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.5  93/06/24  12:55:39  mjl
 * The make-send count associated with a receive-right is *supposed* to be
 * zeroed when the receive-right is sent to another task, so what was thought
 * to be a workaround for an MK bug is really the expected behavior.
 * 
 * Revision 3.4  93/02/22  17:20:28  mjl
 * Delete stale code.
 *
 * Revision 3.3  92/12/10  17:18:00  mjl
 * Use new debug macros.
 * 
 * Revision 3.2  92/11/18  12:48:36  mjl
 * Prototypes for i860 compiler happiness.
 * 
 * Revision 3.1  92/09/28  13:32:43  klh
 * Initial working FIFO relocation implementation (klh for mjl).
 * 
 * Revision 3.0  92/08/17  12:45:19  mjl
 * Support routines for TNC FIFO relocation/migration.
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/errno.h>
#include <kern/macro_help.h>
#include <kern/queue.h>

#include <tnc/un_ff.h>

#ifdef	UN_FF_DEBUG
int	unfflvalue;
#endif	/* UN_FF_DEBUG */

/* Forward Declarations */
int	un_ff_save_port(struct vnode *);


void
un_ff_attach_file_port(
	struct file	*fp,
	mach_port_t	*portp)
{
	kern_return_t	kr;

	ASSERT(fp->f_magic == F_MAGIC);
	file_port_deallocate(fp);
	kr = mach_port_rename( mach_task_self(), *portp, (mach_port_t)fp );
	if ( kr != KERN_SUCCESS )
		panic("un_ff_attach_file_port: rename fail "
				"kr=0x%x oldname=0x%x newname=0x%x",
				kr, *portp, fp);
	*portp = (mach_port_t)fp;
	fp->f_magic = F_MAGIC;
	fp->f_seqno = seqno_from_port(fp);
}


void
un_ff_free_fileq(
	struct vnode	*vp,
	int		keep_ports)
{
	queue_t		vq;
	struct file	*fp;

	ASSERT(vp->v_tncdata);

	vq = V_FILEQ(vp);
	fp = (struct file *) queue_first(vq);
	while ( ! queue_end(vq, (queue_entry_t)fp) ) {

		if (keep_ports)
			fp->f_magic = F_RELOC;
		fdealloc(fp);

		fp = (struct file *) queue_next(&fp->f_chain);
	}
}

void
un_ff_set_usecount(
	struct vnode	*vp,
	long		new_count,
	int		dont_lock)
{
	ASSERT(!dont_lock || new_count > 0);

	if (!dont_lock)
		VN_LOCK(vp);

	/*
	 *  VXREFWAIT means a relocation attempt is waiting for vnode
	 *  references other than for the vnode port or file structs
	 *  to go away.  We shouldn't be here, dorking the usecount,
	 *  if we haven't already finished waiting for this.
	 */
	ASSERT( (vp->v_flag & VXREFWAIT) == 0 );

	if (new_count == 0) {
		/*
		 *  Freeing the vnode.  Be sure the port is already
		 *  gone!  Also, zero make-send count and seqno to
		 *  make vrele() happy.  This could be done by our
		 *  caller, but if they zeroed v_magic we do it for
		 *  them.
		 */
		ASSERT(vp->v_magic == 0);
		vp->v_mscount = 0;
		vp->v_seqno = 0;
		vp->v_usecount = 1;
		VN_UNLOCK(vp);
		vrele(vp);
	} else {
		/* Just set it. */
		vp->v_usecount = new_count;
		if (!dont_lock)
			VN_UNLOCK(vp);
	}
}


/*
 *  This routine forces namei() lookups on the specified FIFO vnode to
 *  be redirected to a special port.  This "namei redirection port" or
 *  nrport eventually points at the FIFO's remote storage site.
 *
 *  The port used as the nrport is obtained with get_vnode_port().
 *  Thus if the vnode's port already exists it is used as the
 *  redirection port, and if not then one is created.  We need a
 *  send-right for the port no matter what, and that's why we use
 *  get_vnode_port().
 *
 *  This routine must be called with the vnode lock held, but
 *  get_vnode_port() wants the vnode unlocked.  Unlocking it here
 *  opens a window in which namei()'s can get through, but by setting
 *  VREDIRECT first we make namei() wait for a valid redirection port.
 *  Once the redirection port is in place, namei() lookups will queue
 *  there.
 *
 *  This routine is called for the storage site vnode prior to all
 *  relocation attempts, regardless of whether the vnode is *also* the
 *  filesystem vnode.  This redirection technique is useful for
 *  keeping namei() lookups at bay while we shuffle vnodes from node
 *  to node.  Hence, this routine must be atomic with respect to
 *  namei() lookups.
 *
 *  The v_usecount may be incremented as a side effect of
 *  get_vnode_port(), if it is necessary to create a receive-right.
 *  After this routine is called, the vnode *will* have one usecount
 *  reference corresponding to a port.  Beware of this when creating
 *  the temporary filesystem vnode port!!!
 */
un_ff_set_redirect(
	struct vnode		*vp,	/* locked FIFO vnode */
	int			flag)
{
	kern_return_t		kr;
	mach_port_t		port;

	ASSERT(vp->v_type == VFIFO &&
	       vp->v_tncdata != NULL &&
	       (vp->v_flag & (VRELOCATING|VREDIRECT)) == VRELOCATING &&
	       V_NRPORT(vp) == MACH_PORT_NULL);
	UNFFSTAT(enable_redirect);

	if (vp->v_magic == 0) {
		UNFFSTAT(portcreate);
		ASSERT(V_SREFS(vp) == 0);
		ASSERT(vp->v_mscount == 0);
	}

	vp->v_flag |= VREDIRECT;
	VN_UNLOCK(vp);
	get_vnode_port(vp, &port);
	VN_LOCK(vp);
	V_SREFS(vp)++;

	if (flag == CHNGNAME) {
		UNFFDEBUG(U_RELOC,
			  ("un_ff_set_redirect: saving port, vp=0x%x\n", vp));
		un_ff_save_port(vp);
		V_NRPORT(vp) = V_SAVEPORT(vp)->pi_name;
	} else {
		/*
		 *  We aren't to rename the port.
		 */
		V_NRPORT(vp) = (mach_port_t)vp;
	}

	if (V_XFLAG(vp) & VX_NRWAIT) {
		/*
		 *  While the vnode was unlocked, one or more namei()
		 *  calls found it and are now waiting for a valid
		 *  redirection port.  We now awaken them to be redirected.
		 *
		 *  I'm tempted to use a count of waiting threads
		 *  rather than just setting the VX_NRWAIT flag, and
		 *  to wait here for all of them to be redirected, but
		 *  waiting for the v_usecount to stabilize (via
		 *  VXREFWAIT flag and vrele() mods) in
		 *  un_ff_relocate() should accomplish the same thing.
		 *  The assertion condition here is necessary (but not
		 *  sufficient) for such a wait to occur.
		 */
		ASSERT(vp->v_usecount > V_FCOUNT(vp) + 1);
		thread_wakeup(&V_NRPORT(vp));
	}
}


/*
 *  Trigger a no-more-senders message for the FIFO's attached vnode port,
 *  and wait for the port to be deallocated.
 */
void
destroy_fifo_port(
	struct vnode		*vp)		/* locked FIFO vnode */
{
	kern_return_t		kr;
	int			port_exists;
	int			wakeup;
	mach_port_mscount_t	old_mscount;
	mach_port_status_t	mps;
#ifdef	UN_FF_DEBUG
	mach_port_urefs_t	srefs;

	kr = mach_port_get_refs(mach_task_self(), (mach_port_t)vp,
				MACH_PORT_RIGHT_SEND, &srefs);
	if (kr != KERN_SUCCESS)
		panic("destroy_fifo_port: get refs: kr 0x%x", kr);
	if (srefs != 1)
		panic("destroy_fifo_port: no nms because %d > 1 sright",
		      srefs);
#endif	/* UN_FF_DEBUG */

	ASSERT(vp->v_type == VFIFO &&
	       vp->v_tncdata != NULL &&
	       vp->v_magic == V_MAGIC &&
	       V_SREFS(vp) == 1);

	/*
	 *  When a receive-right moves between tasks, Mach zeroes its
	 *  associated make-send count.  Since we just got this
	 *  vnode's receive-right back from a remote server, we must
	 *  resynchronize the software-maintained make-send count so
	 *  that the vnode_port_no_senders() will indeed deallocate
	 *  the vnode port.
	 */
	old_mscount = vp->v_mscount;
	kr = mach_port_get_receive_status(mach_task_self(),
					  (mach_port_t)vp,
					  &mps);
	if (kr != KERN_SUCCESS)
		panic("destroy_fifo_port: get_receive_status: kr 0x%x", kr);

	vp->v_mscount = mps.mps_mscount;
	UNFFDEBUG(U_RELOC,
		  ("destroy_fifo_port: mscount resync: was %d, now %d\n",
		   old_mscount, vp->v_mscount));

	/*
	 *  Deallocate the last send-right.
	 */
	kr = mach_port_mod_refs(mach_task_self(),
				(mach_port_t)vp,
				MACH_PORT_RIGHT_SEND, -1); 
	if (kr != KERN_SUCCESS)
		panic("un_ff_destroy_vnode_port: send-right dealloc (0x%x)",
		      kr);

	V_SREFS(vp)--;

	/*
	 *  The port receive-right must be attached in order to get
	 *  the no-more-senders notification.
	 */
	ux_server_add_port((mach_port_t)vp);

	/*
	 *  Now wait for the port to really die.
	 */
	UNFFDEBUG(U_RELOC,
		  ("destroy_fifo_port: wait for port to really die\n"));

	while (vp->v_magic == V_MAGIC) {
		V_XFLAG(vp) |= VX_PDWAIT;
		assert_wait(V_PDCHAN(vp), FALSE);
		VN_UNLOCK(vp);
		thread_block();
		VN_LOCK(vp);
	}
}


/*
 *  Called by vnode port no-more-senders processing just after the
 *  vnode port has been destroyed.
 */
void
tnc_vnode_port_destroyed(
	struct vnode	*vp)
{
	VN_LOCK(vp);
	if (vp->v_tncdata == 0) {
		VN_UNLOCK(vp);
		return;
	}
	if (V_XFLAG(vp) & VX_PDWAIT) {
		ASSERT(vp->v_magic == 0);
		V_XFLAG(vp) &= ~VX_PDWAIT;
		VN_UNLOCK(vp);
		thread_wakeup(V_PDCHAN(vp));
		return;
	}
	VN_UNLOCK(vp);
}


/*
 *  Refresh various fields in the saved port area that may have changed
 *  while the port's receive-right was on another node.  The software
 *  maintained make-send count is passed in, but the port sequence
 *  number is gotten from the microkernel.
 */
int
un_ff_resync_saved_port(
	struct vnode		*vp,
	mach_port_mscount_t	mscount)
{
	port_info_t		*pi;
	kern_return_t		kr;
	mach_port_status_t	mps;

	ASSERT(vp->v_tncdata);
	pi = V_SAVEPORT(vp);

	kr = mach_port_get_receive_status(mach_task_self(),
					  pi->pi_name,
					  &mps);
	if (kr != KERN_SUCCESS)
		panic("un_ff_resync_saveport: get rcv status: kr 0x%x", kr);
	pi->pi_seqno = mps.mps_seqno;

	/* Software maintained make-send count, *not* MK maintained one. */
	pi->pi_mscount = mscount;

	kr = mach_port_get_refs(mach_task_self(),
				pi->pi_name,
				MACH_PORT_RIGHT_SEND,
				&pi->pi_srefs);
	if (kr != KERN_SUCCESS)
		panic("un_ff_resync_saveport: get refs: kr 0x%x", kr);
}


int
un_ff_save_port(
	struct vnode	*vp)
{
	port_info_t	*pi;
	kern_return_t	kr;

	ASSERT(vp->v_tncdata);
	pi = V_SAVEPORT(vp);
	ASSERT(pi->pi_name == MACH_PORT_NULL);

	/*
	 *  Move installed vnode port info to the saved port area.
	 */
	kr = mach_port_rename(mach_task_self(),
			      (mach_port_t)vp, (mach_port_t)pi);
	if (kr != KERN_SUCCESS)
		panic("un_ff_save_port: rename fail "
				"kr=0x%x oldname=0x%x newname=0x%x",
				kr, vp, pi);
	pi->pi_name = (mach_port_t)pi;

	pi->pi_seqno = vp->v_seqno;
	pi->pi_mscount = vp->v_mscount;
	pi->pi_srefs = V_SREFS(vp);

	FIFO_PORT_GONE(vp);
}


int
un_ff_restore_saved_port(
	struct vnode	*vp)
{
	kern_return_t	kr;
	port_info_t	*pi;

	ASSERT(vp->v_tncdata && vp->v_magic == 0);
	pi = V_SAVEPORT(vp);
	ASSERT(pi->pi_name != MACH_PORT_NULL);

	/*
	 *  Move saved port to become installed vnode port.
	 */
	if (pi->pi_name != (mach_port_t)vp) {
		kr = mach_port_rename(mach_task_self(),
				      pi->pi_name,
				      (mach_port_t)vp);
		if (kr != KERN_SUCCESS)
			panic("un_ff_restore_saved_port: rename fail "
					"kr=0x%x oldname=0x%x newname=0x%x",
					kr, pi->pi_name, vp);
	}
	vp->v_magic = V_MAGIC;
	vp->v_seqno = pi->pi_seqno;
	vp->v_mscount = pi->pi_mscount;
	V_SREFS(vp) = pi->pi_srefs;
	INIT_SAVEPORT(vp);
}
