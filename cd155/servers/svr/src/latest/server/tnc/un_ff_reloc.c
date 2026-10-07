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
 * $Log: un_ff_reloc.c,v $
 * Revision 1.13  1995/02/01  22:06:03  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.12  1994/12/17  00:00:19  slk
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
 * Revision 1.11  1994/11/18  20:44:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1993/10/28  03:12:24  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.9  1993/07/29  21:54:09  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server slowdown.
 *
 * Revision 1.8  1993/07/14  18:36:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  19:28:07  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:09:55  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/07  01:28:00  cfj
 * Merge from T9.
 *
 * Revision 1.4.4.1  1993/03/07  01:15:44  cfj
 * Fix to make TNC work with OSF sync. close.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:46  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:19  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:34  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.7  93/07/27  11:32:56  mjl
 * [Bug #0308] Initialize shared file struct seqno lock (aka VN_SEQNO_LOCK()).
 * 
 * Revision 3.6  93/06/24  12:53:48  mjl
 * [LCC bug 0229] Remove bogus assertions on vp->v_usecount.  Assert total
 *     number of file refs is one more than number of files (one ref for
 *     current op in progress).
 * 
 * Revision 3.5  93/06/14  14:04:49  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Fixed relocation code to actually send the file struct across the wire
 * during a relocation. 
 * 
 * Revision 3.4  93/03/10  14:19:04  yazz
 * Disable pipe relocation temporarily until synchronous close code
 * can be written for vsockets/tnc.
 *
 * Revision 3.3  92/12/10  17:17:34  mjl
 * Use new debug macros.
 * 
 * Revision 3.2  92/11/18  12:48:11  mjl
 * Make sure newly arrived FIFO starts out with a clean shared mutex.
 * 
 * Revision 3.1  92/09/28  13:31:55  klh
 * Initial working FIFO relocation implementation (klh for mjl).
 * 
 * Revision 3.0  92/08/17  12:44:53  mjl
 * Low level code for TNC FIFO relocation.
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/protosw.h>
#include <sys/time.h>
#include <sys/vnode.h>
#include <sys/user.h>
#include <sys/errno.h>
#include <sys/file.h>

#include <uxkern/syscall_subr.h>
#include <tnc/reloc.h>
#include <tnc/un_ff.h>
#include <tnc/sgd.h>

#define	NO_PIPE_RELOC	1 
# ifdef NO_PIPE_RELOC
int	un_ff_disable	= 1;
# else
int	un_ff_disable	= 0;
# endif

extern node_t		this_node;
extern struct socket	*sopartner(struct socket *);

static int	un_ff_common_arrival(struct vnode *, sgd_t *, caddr_t,
				     lmv_t *, portv_t *);
void		un_ff_um_vnode(struct vnode *, struct vnode *);
void		un_ff_um_unffdata(un_ff_t *, un_ff_t *);
void		un_ff_destroy_vnode_port(struct vnode *);


/*
 *  Attempt to relocate the FIFO.  At this point, the VRELOCATING
 *  vnode flag is set and all blocked operations were awakened and
 *  restarted.  The vnode's file queue is quiescent, and all file
 *  ports and the vnode port are no longer in service.  New namei()
 *  lookups are being queued in the namei redirection port,
 *  V_NRPORT(vp).
 *
 *  The vnode lock must be held upon entry.
 *
 *  There are two (client-side) cases of concern here: are we
 *  relocating away from the FIFO filesystem node, thus splitting the
 *  FIFO into a filesystem vnode and a remote storage vnode, or are we
 *  simply relocating an existing remote storage vnode?  If the
 *  latter, we also check to see if the storage vnode is rejoining the
 *  filesystem vnode or not and select an appropriate arrival code;
 *  these are the server-side cases.
 */
int
un_ff_really_relocate(
	struct vnode	*vp)
{
	int		ret;
	kern_return_t	kr;
	int		splitting;
	sgd_t		*sgdp;
	sgde_t		*hdr, *sgde;
	lmv_t		*lmvp;
	policy_state_t	*ps;
	portv_t		*rrights;
	portv_t		*srights;
	mach_port_t	temp_fs_port;
	struct file	*fp;
	queue_t		vq;
	struct socket	*so;
	mach_port_t	server_port;
	port_info_t	*pi;

	if ( un_ff_disable ) {
		UNFFDEBUG(U_RELOC, 
			("un_ff_really_relocate: FIFO relocation disabled.\n"));
		return (EINVAL);
	}

	UNFFDEBUG(U_RELOC,("un_ff_really_relocate: entry\n"));

	/*
	 *  Allocate expandable vectors for the socket graph
	 *  descriptor, the large mbuf vector, and the port
	 *  vectors.
	 */
	if ( (sgdp = SGD_ALLOC()) == NULL )
		panic("un_pp_really_reloc: cannot allocate sgd");
	if ( (lmvp = LMV_ALLOC()) == NULL )
		panic("un_pp_really_reloc: cannot allocate lmv");
	if ( (rrights = PV_ALLOC()) == NULL )
		panic("un_pp_really_reloc: cannot allocate rrights");
	if ( (srights = PV_ALLOC()) == NULL )
		panic("un_pp_really_reloc: cannot allocate srights");

	ASSERT(vp->v_tncdata != NULL &&
	  (vp->v_flag & (VREDIRECT|VRELOCATING)) == (VREDIRECT|VRELOCATING) &&
	       vp->v_usecount == V_FCOUNT(vp) + 1);

	/*
	 *  Since we are currently redirecting, we know the storage
	 *  site's vnode port receive-right is the redirection port.
	 */
	* PV_NEXT(rrights, 0) = V_NRPORT(vp);
	
	/*
	 *  We also want to ship a send-right to the filesystem vnode
	 *  port.  If we are relocating away from the filesystem
	 *  vnode, then we have to create one and pass it along; if we
	 *  are relocating a remote storage vnode then we already have
	 *  one.
	 */
	if ( V_FSYS(vp) ) {
		/*
		 *  Create the temporary filesystem vnode port, used
		 *  while the FIFO is split between two vnodes.
		 *
		 *  Call get_vnode_port() to create the temporary port
		 *  for us.  Afterwards, get rid of the extra vnode
		 *  referrence (since turning on redirection left
		 *  intact the referrence corresponding to the
		 *  original port).  This is highly dependent on how
		 *  get_vnode_port() is implemented.
		 *
		 *  NB no worries about racing namei() since
		 *  redirection is on.
		 *
		 *  We have to add the resulting receive-right to the
		 *  server's port set ourselves, since get_vnode_port()
		 *  won't do it when VRELOCATING is set.
		 */
		ASSERT(vp->v_magic == 0);
		VN_UNLOCK(vp);
		get_vnode_port(vp, &temp_fs_port);
		* PV_NEXT(srights, 0) = temp_fs_port;
		vrele(vp);
		VN_LOCK(vp);
		ASSERT(vp->v_mscount == 1);
		V_SREFS(vp) = 1;
		ux_server_add_port(temp_fs_port);
	} else {
		/*
		 *  Just moving away from a storage-only node; we
		 *  already have the filesystem vnode's send-right.
		 */
		* PV_NEXT(srights, 0) = V_FSPORT(vp);
	}


	/*
	 *  Now we begin creating an sgd for the FIFO.
	 *
	 *  Label the sgd's header descriptor.  The label depends
	 *  on whether we are returning to the original filesystem
	 *  node or not.  The handling of these two cases are
	 *  seperated into different arrival routines.
	 */
	hdr = SGD_NEXT(sgdp, 0);
	hdr->sgde_type = SGDE_TYPE_SGDHDR;
	ps = V_PSTATE(vp);
	UNFFDEBUG(U_RELOC,("un_ff_reloc: new node %d, fs node %d -- %s\n",
			    ps->ps_newnode, V_FSNODE(vp),
			    (ps->ps_newnode == V_FSNODE(vp)
			     ? "rejoin" : "rss")));
	if ( ps->ps_newnode == V_FSNODE(vp) ) {
		ASSERT(V_STRG_ONLY(vp));
		hdr->sgd_type = SGD_TYPE_FIFO_REJOIN;
	} else {
		hdr->sgd_type = SGD_TYPE_FIFO;
	}
	hdr->sgd_inlsize = 0;
	hdr->sgd_outlcnt = 0;
	hdr->sgd_len = 1;
	

	/*
	 *  First comes the vnode itself, and its associated TNC data.
	 */
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_VNODE, vp, sizeof(struct vnode));
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_FIFO_INFO, vp->v_tncdata, sizeof(un_ff_t));

	/*
	 *  Next come the file structs and their associated ucred
	 *  structs and file ports.  All we really need from the
	 *  file structs themselves is the mode info in fp->f_flag.
	 *  Actually, we now ship the entire file struct across so
	 *  that synchronous close can keep the number of references
	 *  straight.
	 */
	vq = V_FILEQ(vp);
	fp = (struct file *) queue_first(vq);
	while ( ! queue_end(vq, (queue_entry_t)fp) ) {

		sgde = SGD_NEXT(sgdp, 0);
		MAKE_SIMPLE_SGDE(sgdp, sgde,
				 SGDE_TYPE_FILE, fp, sizeof(*fp));

		sgde->sgde_xflags = fp->f_flag;

		sgde = SGD_NEXT(sgdp, 0);
		MAKE_SIMPLE_SGDE(sgdp, sgde,
				 SGDE_TYPE_UCRED,
				 fp->f_cred, sizeof(struct ucred));

		* PV_NEXT(rrights, 0) = (mach_port_t)fp;

		fp = (struct file *) queue_next(&fp->f_chain);
	}

	/*
	 *  Last come the sockets and mbufs.
	 */
	so = vp->v_socket;
	sgd_socket_append(sgdp, lmvp, so);	/* writing end */
	so = sopartner(so);
	sgd_socket_append(sgdp, lmvp, so);	/* reading end */

	/*
	 *  If this was a remote storage node with make-send count
	 *  greather than one, then an extra send-right (that is, one
	 *  in addition to the send-right held at the filesystem node)
	 *  for the port was created when the notify routine turned on
	 *  namei() redirection (i.e. as a side effect of
	 *  un_ff_set_redirect() ).  We get rid of it here, ensuring
	 *  that only one storage vnode send-right exists unless a
	 *  relocation is in progress.
	 *
	 *  (Setting redirection with the CHNGNAME flag moved all the
	 *  port-related info to the saved port area V_SAVEPORT(vp).)
	 */
	pi = V_SAVEPORT(vp);
	ASSERT(pi->pi_name && pi->pi_name == V_NRPORT(vp));
	if (pi->pi_srefs > 1 && V_STRG_ONLY(vp)) {
		ASSERT(pi->pi_srefs == 2);
		UNFFPORTINFO(V_NRPORT(vp), "extra sright dealloc");
		kr = mach_port_mod_refs(mach_task_self(), V_NRPORT(vp),
					MACH_PORT_RIGHT_SEND, -1);
		if (kr != KERN_SUCCESS)
			panic("un_ff_really: extra sright dealloc kr 0x%x",
			      kr);
	}

	/* Relocate the FIFO. */
	ret = tnc_get_server_port(ps->ps_newnode, &server_port);
	if (ret != ESUCCESS)
		panic("un_ff_really: tnc_get_server_port (%d)", ret);
	ret = tnc_relocate(server_port, sgdp, lmvp, rrights, srights);

	/*
	 *  Whether we succeeded or not, we still need to deallocate
	 *  expandable vectors created here.
	 */
	SGD_DEALLOC(sgdp);
	LMV_DEALLOC(lmvp);
	PV_DEALLOC(rrights);
	PV_DEALLOC(srights);

	/*
	 *  Examine the relocation RPC's error code and clean up
	 *  accordingly.
	 */
	if (ret == ESUCCESS) {
		/*
		 *  This is no longer the storage site vnode, but a
		 *  zombie waiting to be closed.
		 */
		V_XFLAG(vp) &= ~VX_STRG;
		V_XFLAG(vp) |= VX_ZOMBIE;
	} else if (V_FSYS(vp)) {
		/*
		 *  If there was a failure relocating away from the
		 *  filesystem vnode, deallocate the temporary
		 *  filesystem vnode port.
		 */
		UNFFDEBUG(U_RELOC,("un_ff_really_relocate: relocate failed destroy fifo port\n"));
		destroy_fifo_port(vp);

		/*
		 *  Turn off namei() redirection and restore the
		 *  original vnode port and its make-send count if one
		 *  existed.  Otherwise (i.e. if the port was
		 *  originally created by un_ff_set_redirect())
		 *  triggers a no-more-senders message to deallocate
		 *  the port.
		 *
		 *  XXX In case our assumption that only this TNC FIFO code
		 *  will ever create a vnode port for a VFIFO vnode proves
		 *  false, here is where we would test a VX_HADPORT flag,
		 *  and if set we would not destroy this port.
		 */
		FIFO_CLEAR_REDIRECT(vp);
		ASSERT(vp->v_magic == 0);
		UNFFDEBUG(U_RELOC,
		    ("un_ff_really_relocate: restore_saved, vp=0x%x errno=%d\n",
		     vp, ret));
		un_ff_restore_saved_port(vp);
		ASSERT(vp->v_magic == V_MAGIC);
		vp->v_usecount++;
		destroy_fifo_port(vp);
	}
	else UNFFDEBUG(U_RELOC,
		       ("un_ff_relocate(0x%x): err=%d on non-FSYS node\n",
			vp, ret));

	return ret;
}


/*
 *  This arrival routine is called when the FIFO arrives on a node
 *  other than its filesystem node.
 */
/*ARGSUSED*/
int
un_ff_arrival(
	mach_port_t	server_port,
	sgd_t		*sgdp,
	caddr_t		inline_data,
	lmv_t		*lmvp,
	portv_t		*rrights,
	portv_t		*srights)
{
	int		error;
	kern_return_t	kr;
	sgde_t		*sgde;
	struct vnode	*vp, *ovp;
	mach_port_t	*vnode_portp;
	port_info_t	*pi;
	extern struct vnodeops fifo_vnodeops;

	ASSERT(SGD_LOOKAHEAD(sgdp) == SGD_HEADER(sgdp));
	(void) SGD_NEXT(sgdp, 1);	/* Skip over the header. */

	UNFFDEBUG(U_RELOC,("un_ff_arrival: entry\n"));
	/*
	 *  Allocate a vnode from the dead vfs to act as the storage
	 *  vnode on this site.
	 */
	if ( (error = getnewvnode(VT_NON, &fifo_vnodeops, &vp)) != ESUCCESS )
		return (error);
	insmntque(vp, DEADMOUNT);

	/*
	 *  Attach the vnode receive-right, unmarshal the vnode data
	 *  into the new vnode.
	 */
	VN_LOCK(vp);
	vp->v_magic = V_MAGIC;
	vnode_portp = PV_NEXT(rrights, 1);
	kr = mach_port_rename(mach_task_self(), *vnode_portp, (mach_port_t)vp);
	if ( kr != KERN_SUCCESS )
		panic("un_ff_arrival: port rename fail kr=0x%x oldname=0x%x "
				"newname=0x%x", kr, *vnode_portp, vp);
	*vnode_portp = (mach_port_t)vp;

	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde->sgde_type == SGDE_TYPE_VNODE);
	un_ff_um_vnode((struct vnode *)SGDE_ADDR(sgde,inline_data), vp);

	/*
	 *  Now get the TNC FIFO-specific data and modify it to
	 *  indicate this is now a remote storage site vnode.
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde->sgde_type == SGDE_TYPE_FIFO_INFO);
	vp->v_tncdata = (caddr_t) malloc(sizeof(un_ff_t));
	un_ff_um_unffdata((un_ff_t *)SGDE_ADDR(sgde,inline_data),
			  (un_ff_t *)vp->v_tncdata);
	VN_SEQNO_LOCK_INIT(vp);

	/*
	 *  Now fix up both the vnode and its connected TNC data so
	 *  all is consistent, keeping in mind that this is a remote
	 *  storage node.
	 */
	vp->v_flag &= ~(VREDIRECT|VRELOCATING);
	V_NRPORT(vp) = MACH_PORT_NULL;
	vp->v_seqno = seqno_from_port((mach_port_t)vp);
	pi = V_SAVEPORT(vp);	/* This either the fs node's nrport info,
				 * or the rss node's ss port info---either
				 * way it is port info for the storage
				 * vnode's port.
				 */
	vp->v_mscount = pi->pi_mscount;
	V_SREFS(vp) = pi->pi_srefs;
	V_XFLAG(vp) = VX_STRG;
	V_FSPORT(vp) = * PV_NEXT(srights, 1);
	INIT_SAVEPORT(vp);

	/*
	 *  Common arrival code sets up file structs etc.
	 */
	error = un_ff_common_arrival(vp, sgdp, inline_data, lmvp, rrights);
	if (error != ESUCCESS) {
		/* Free the vnode we allocated above. */
		FIFO_PORT_GONE(vp);
		VN_UNLOCK(vp);
		un_ff_set_usecount(vp, 0, FALSE /*vp is not locked*/);
	} else
		VN_UNLOCK(vp);

	return (error);
}


/*
 *  This arrival routine is called when the FIFO storage vnode arrives
 *  on the node where its corresponding filesystem vnode lives.
 */
/*ARGSUSED*/
int
un_ff_rejoin(
	mach_port_t	server_port,
	sgd_t		*sgdp,
	caddr_t		inline_data,
	lmv_t		*lmvp,
	portv_t		*rrights,
	portv_t		*srights)
{
	int		error;
	kern_return_t	kr;
	sgde_t		*sgde;
	struct vnode	*vp, *ovp;
	struct vnode	vtemp;
	un_ff_t		fftemp;
	un_ff_t		*ss_tdp;
	mach_port_t	*ss_portp;
	mach_port_t	*fs_portp;
	port_info_t	*pi;
	int		wakeup;

	ASSERT(SGD_LOOKAHEAD(sgdp) == SGD_HEADER(sgdp));
	(void) SGD_NEXT(sgdp, 1);	/* Skip over the header. */

	UNFFDEBUG(U_RELOC,("un_ff_rejoin: entry\n"));
	/*
	 *  Obtain a pointer to the filesystem FIFO vnode that
	 *  we are rejoining.  Make sure it looks groovy.
	 */
	fs_portp = PV_NEXT(srights, 1);
	PORT_TO_VNODE_LOOKUP(*fs_portp, vp);
	if (vp == NULL)
		panic("un_ff_rejoin: not on fs node");
	VN_LOCK(vp);

	/*
         *  Sanity checks:
	 *  - this is a FIFO vnode
	 *  - with TNC data area already attached
	 *  - with no local storage
	 *  - and only one send-right (held by the storage vnode)
	 *  - no files on the file queue; only storage vnodes have them
	 *  - namei() redirection is enabled.
	 *
	 *  The v_usecount of a file system vnode becomes stale when the
	 *  FIFO storage relocates away (since processes may open or close
	 *  the FIFO at its storage site).  Thus the value of vp->v_usecount
	 *  is stale here.
	 */
	ASSERT(vp->v_type == VFIFO &&
	       vp->v_tncdata != NULL &&
	       V_FSYS_ONLY(vp) &&
	       vp->v_mscount == 1 &&
	       queue_empty(V_FILEQ(vp)) &&
	       (vp->v_flag & VREDIRECT) &&
	       V_NRPORT(vp) != MACH_PORT_NULL);

	/* Save all vnode data in case we need to back out. */
	vtemp = *vp;
	fftemp = *(un_ff_t *)vp->v_tncdata;

	/*
	 *  The storage site vnode's port comes first in the
	 *  receive-rights vector.  Get it now, before calling
	 *  un_ff_common_arrival().  This should be the same port
	 *  currently being used for namei() redirection.
	 */
	ss_portp = PV_NEXT(rrights, 1);
	ASSERT(*ss_portp == V_NRPORT(vp));

	/*
	 *  Unmarshal vnode data from the storage vnode and fix up
	 *  the flags.  Leave namei() redirection enabled for now.
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde->sgde_type == SGDE_TYPE_VNODE);
	un_ff_um_vnode((struct vnode *)SGDE_ADDR(sgde,inline_data), vp);

	/*
	 *  Now get the TNC FIFO-specific data.
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde->sgde_type == SGDE_TYPE_FIFO_INFO && vp->v_tncdata);
	ASSERT(V_FSYS_ONLY(vp));
	un_ff_um_unffdata((un_ff_t *)SGDE_ADDR(sgde,inline_data),
			  (un_ff_t *)vp->v_tncdata);
	VN_SEQNO_LOCK_INIT(vp);

	/*
	 *  Common arrival code for rejoins and regular arrivals.
	 */
	error = un_ff_common_arrival(vp, sgdp, inline_data, lmvp, rrights);
	if (error != ESUCCESS) {
		/*
		 *  Restore the vnode and its TNC data.
		 *  No port manipulation is required.
		 */
		*vp = vtemp;
		bcopy(&fftemp, vp->v_tncdata, sizeof(un_ff_t));
		VN_UNLOCK(vp);
		VNODE_LOOKUP_DONE(vp);
		return (error);
	}

	/*
	 *  Now we want to straighten out the vnode port situation.
	 *  (This is much like svr_tnc_remote_fifo_death().)  We have
	 *  to get rid of the temporary filesystem vnode port, then
	 *  install the storage vnode port in its place, then destroy
	 *  that port as well (allowing queued messages to drain).
	 *
	 *  First some sanity checks:
	 *  - still redirecting namei() requests
	 *  - fs port still attached so mscount should be 1
	 */
	ASSERT(vp->v_flag & VREDIRECT);
	ASSERT(vp->v_mscount == 1);

	/*
	 *  Get rid of fs vnode port.  We don't need to worry about
	 *  messages queued in it, because its only real use is as a
	 *  handle for the tnc_remote_fifo_death() RPC routine.
	 */
	UNFFPORTINFO((mach_port_t)vp, "rejoin destroys fs port");
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
	pi = V_SAVEPORT(vp);	/* This was rss node's vnode port info */
	pi->pi_name = *ss_portp;
	un_ff_resync_saved_port(vp, pi->pi_mscount);
	UNFFDEBUG(U_RELOC, ("un_ff_rejoin: restore_saved, vp=0x%x\n", vp));
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
	 *  false, here is where we would test a VX_HADPORT flag,
	 *  and if set we would not destroy this port.
	 */
	UNFFPORTINFO((mach_port_t)vp, "destroy returned ss port");
	destroy_fifo_port(vp);
	FIFO_CLEAR_REDIRECT(vp);
	V_XFLAG(vp) |= VX_FSYS|VX_STRG;
	vp->v_flag &= ~VRELOCATING;
	wakeup = V_XFLAG(vp) & VX_NRWAIT;

	vp->v_usecount = V_FCOUNT(vp) + 1 + (vp->v_magic ? 1 : 0 );
			/* Number of attached files plus one for the
			 * vref we took via PORT_TO_VNODE_LOOKUP(),
			 * plus another for the vnode port if it still
			 * exists after calling un_ff_clr_redirect().
			 */

	VN_UNLOCK(vp);
	VNODE_LOOKUP_DONE(vp);
	if (wakeup)
		thread_wakeup(&V_NRPORT(vp));
	return (ESUCCESS);
}


/*
 *  Once the new storage vnode (which may also be the filesystem
 *  vnode) is set up, this routine is called to attach the associated
 *  file structures, ucred structures, file ports, and sockets.
 *  And to do other common arrival things....
 */	 
static int
un_ff_common_arrival(
	struct vnode	*vp,		/* locked vnode at arrival site */
	sgd_t		*sgdp,		/* descriptor for arriving data */
	caddr_t		inline_data,	/* buffer of marshalled data */
	lmv_t		*lmvp,		/* large mbuf vector */
	portv_t		*rrights)	/* file port receive-rights */
{
	int		error;
	uthread_t	uth = current_thread();
	sgde_t		*sgde;
	int		file_flags;
	int		file_count;
	int		fref_count;
	struct ucred	*saved_ucred = NULL;
	struct socket	*wso, *rso;
	struct unpcb	*unp;
	queue_t		vq;
	struct file	*fp;
	struct file	*fp_proto;	/* Points at transported file struct */
	extern struct fileops vnops;

	ASSERT(uth->uu_procp);
	if ( uth->uu_procp->p_rcred != NOCRED ) {
		UNFFDEBUG(U_RELOC,("ff_cmn_arvl: ucred saved\n"));
		saved_ucred = uth->uu_procp->p_rcred;
	}
	UNFFDEBUG(U_RELOC,("un_ff_common_arrival: entry\n"));

	ASSERT(vp->v_tncdata != NULL && queue_empty(V_FILEQ(vp)));
	file_count = V_FCOUNT(vp);
	V_FCOUNT(vp) = 0;
	fref_count = 0;

	while ( SGD_LOOKAHEAD(sgdp)->sgde_type == SGDE_TYPE_FILE ) {

		/*
		 *  Here we get a pointer to the file struct which was
		 *  shipped over. The original code sent a copy of the
		 *  f_flag field in the sgde_xflags word. Now that we
		 *  ship the entire structure, this is redundant, but
		 *  for now we have left it in, and we test to see that
		 *  the data matches.
		 */
		sgde = SGD_NEXT(sgdp, 1);
		fp_proto = (struct file *)SGDE_ADDR(sgde,inline_data);
		file_flags = sgde->sgde_xflags;
		ASSERT(file_flags == fp_proto->f_flag);

		/*
		 *  Credentials must be installed before calling falloc().
		 */
		sgde = SGD_NEXT(sgdp, 1);
		ASSERT(sgde->sgde_type == SGDE_TYPE_UCRED);
		uth->uu_procp->p_rcred = crdup(SGDE_ADDR(sgde,inline_data));

		if ( error = falloc(&fp) )
			goto out2;
		un_ff_attach_file_port(fp, PV_NEXT(rrights, 1));
		fp->f_data = (caddr_t)vp;
		fp->f_ops = &vnops;
		fp->f_type = DTYPE_VNODE;
		fp->f_flag = file_flags;

		/*
		 *  Initialize number of send rights created and released
		 *  by using the copy of the file struct which was sent
		 *  over from the source node. We calculate the f_count
		 *  field rather than use the value in the prototype.
		 *  This is because it's difficult to keep track of whether
		 *  a particular f_count may have been bumped by the
		 *  file operation envelope routines.
		 */
		fp->f_makesend = fp_proto->f_makesend;
		fp->f_relsend = fp_proto->f_relsend;
		fp->f_count = fp->f_makesend - fp->f_relsend;
		fref_count += fp_proto->f_count;

#if	SER_COMPAT
		fp->f_funnel = FUNNEL_NULL;
#endif
		/* Don't need to lock fp since we just alloc'ed it. */
		VN_FILEQ_ADD(vp,fp);
	}

	ASSERT(file_count == V_FCOUNT(vp));
	ASSERT(fref_count == file_count + 1);

	/*
	 *  Now unmarshal and attach the FIFO sockets.
	 */
	error = sgd_socket_extract(sgdp, inline_data, lmvp, &wso);
	if ( error != ESUCCESS ) {
		UNFFDEBUG(U_RELOC,("un_ff_arvl: wso extract err %d\n",error));
		goto out2;
	}
	error = sgd_socket_extract(sgdp, inline_data, lmvp, &rso);
	if ( error != ESUCCESS ) {
		UNFFDEBUG(U_RELOC,("un_ff_arvl: rso extract err %d\n",error));
		goto out3;
	}

	/*
	 *  That should be the last entry in the sgd.
	 */
	ASSERT(SGD_NEXT(sgdp, 1) == NULL);

	/*
	 *  Connect the sockets to each other and to the storage vnode.
	 */
	if ( error = soconnect2(wso, rso) )
		panic("un_ff_arvl: soconnect2(0x%x,0x%x) failed: %d",
		      wso, rso, error);
	vp->v_socket = wso;

	/*
	 *  Fix up the flow control counts in the UNIX protocol
	 *  control blocks.
	 */
	unp = (struct unpcb *)wso->so_pcb;
	unp->unp_cc = wso->so_rcv.sb_cc;
	unp->unp_mbcnt = wso->so_rcv.sb_mbcnt;
	unp->unp_conn->unp_cc = rso->so_rcv.sb_cc;
	unp->unp_conn->unp_mbcnt = rso->so_rcv.sb_mbcnt;

	/*
	 *  Update the relocation policy state information to
	 *  reflect our arrival here.
	 */
	V_PSTATE(vp)->ps_rdrnode = this_node;
	V_PSTATE(vp)->ps_newnode = INVALID_NODE;

	/*
	 *  Lastly, begin servicing vnode port and file ports.
	 */
	vq = V_FILEQ(vp);
	fp = (struct file *) queue_first(vq);
	while ( ! queue_end(vq, (queue_entry_t)fp) ) {
		ux_server_add_port((mach_port_t)fp);
		fp = (struct file *) queue_next(&fp->f_chain);
	}
	ux_server_add_port((mach_port_t)vp);

	if (saved_ucred)
		uth->uu_procp->p_rcred = saved_ucred;
	return (ESUCCESS);
out3:
	soclose(wso);
out2:
	un_ff_free_fileq(vp, 1/*keep_ports*/);
out1:
	if (saved_ucred)
		uth->uu_procp->p_rcred = saved_ucred;
	return (error);
}


/*
 *  Vnode unmarshalling.  Note we don't touch any of the port-related
 *  data such as v_seqno, v_mscount, v_magic.
 */
void
un_ff_um_vnode(
	struct vnode	*from,		/* vnode data to be unmarshalled */
	struct vnode	*to)		/* new "live" vnode (locked) */
{
#define COPYMEMBER(member)	to->member = from->member

	COPYMEMBER(v_iomode);
	COPYMEMBER(v_flag);
	COPYMEMBER(v_usecount);
	COPYMEMBER(v_holdcnt);
/*	COPYMEMBER(v_shlockc);/**/
/*	COPYMEMBER(v_exlockc);/**/
	COPYMEMBER(v_lastr);
	COPYMEMBER(v_id);
	COPYMEMBER(v_numoutput);
	COPYMEMBER(v_outflag);
	COPYMEMBER(v_type);
	COPYMEMBER(v_rdcnt);
	COPYMEMBER(v_wrcnt);

#undef COPYMEMBER
}


/*
 *  Unmarshal TNC-specific FIFO data.  The following fields are
 *  explicitly *not* copied here, as they can only be correctly set by
 *  the caller:
 *
 *	uf_xflag [V_XFLAG(vp)]		-- TNC flag bits
 *	uf_nrport [V_NRPORT(vp)]	-- namei() redirection port
 *	uf_fsport [V_FSPORT(vp)]	-- temporary filesystem vnode port
 *	uf_srefs [V_SREFS(vp)]		-- send-right reference count
 */
void
un_ff_um_unffdata(
	un_ff_t	*from,
	un_ff_t	*to)
{
	long		save_xflag;
	mach_port_t	save_nrport;
	mach_port_t	save_fsport;
	mach_port_urefs_t save_srefs;

	ASSERT(from->uf_factive == 0 && to->uf_factive == 0);

	save_xflag = to->uf_xflag;
	save_nrport = to->uf_nrport;
	save_fsport = to->uf_fsport;
	save_srefs = to->uf_srefs;

	bcopy(from, to, sizeof(un_ff_t));
	queue_init(&to->uf_fileq);

	to->uf_xflag = save_xflag;
	to->uf_nrport = save_nrport;
	to->uf_fsport = save_fsport;
	to->uf_srefs = save_srefs;
}
