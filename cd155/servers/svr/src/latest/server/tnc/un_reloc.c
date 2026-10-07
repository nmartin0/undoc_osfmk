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
 * $Log: un_reloc.c,v $
 * Revision 1.11  1995/02/01  22:09:36  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:45:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/10/28  03:13:49  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.8  1993/09/01  01:37:38  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.7  1993/07/14  18:36:27  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:50:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:28:58  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:18  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:10:04  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:58  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:28  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:42  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.7  93/08/26  10:47:47  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.
 * 
 * Revision 3.6  93/08/23  00:09:20  mjl
 * [LCCbug #0370] AF_UNIX virtual sockets must use socketops fileops table
 * (otherwise, using vsocketops table causes a memory leak).
 * 
 * Revision 3.5  93/06/14  14:06:25  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Fixed recreate_filestruct_for_socket() to take an additional argument -
 * the prototype file structure which is now shipped across the wire on a
 * relocation, and use it to set up the f_makesend and f_relsend counts.
 * 
 * Revision 3.4  92/12/10  17:19:53  mjl
 * Moved sgd_unp_socket_*() routines to un_subr.c .  Use new debug macros.
 * Use new port-to-socket mapping macros from un_port_hash.h .
 *
 * Revision 3.3  92/11/18  12:49:26  mjl
 * uth->uu_opn_filep now cleared by common arrival_dispatch() code.
 * 
 * Revision 3.2  92/11/06  12:49:16  mjl
 * Removed many unconditionally printed debug printfs.
 * 
 * Revision 3.1  92/08/17  13:12:18  mjl
 * Make server_port be first arg to relocation RPCs.
 * 
 * Revision 3.0  92/08/08  01:21:16  jdh
 * un_pp_reloc.c was renamed to un_reloc.c and reworked to handle
 * both pipe and stream socket pairs -- jdh
 * 
 * Revision 3.1  92/06/15  17:38:17  mjl
 * Fix bogus ust_really_relocate() return value when relocation is disabled.
 * Macro SOSETFP() is now obsolete.  Initialize uu_opn_filep thread field to
 * prevent assertion failure in ust_socreate().
 * 
 * Revision 3.0  92/06/11  16:04:25  mjl
 * Pipe relocation routines.
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/ucred.h>
#include <sys/file.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <uxkern/syscall_subr.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <vsocket/vs_types.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/errno.h>
#include <net/net_malloc.h>
#include <sys/un.h>

#include <tnc/un.h>
#include <tnc/vector.h>
#include <tnc/sgd.h>
#include <tnc/reloc.h>

extern void		file_port_deallocate(struct file *);
extern int		tnc_lg_mbuf_dealloc(caddr_t, int, caddr_t);
extern struct fileops	socketops;


int
recreate_filestruct_for_socket(
	struct socket	*so,
	mach_port_t	*port,
	struct file	**fpp,
	struct file	*fp_proto)
{
	int		error;
	kern_return_t	kr;
	struct file	*fp;
	mach_port_status_t
			mps;

	if ( error = falloc(&fp) )
		return error;

	file_port_deallocate(fp);
	kr = mach_port_rename( mach_task_self(), *port, (mach_port_t)fp );
	if ( kr != KERN_SUCCESS )
		panic("recreate_fp_for_so: rename fail "
				"kr=0x%x oldname=0x%x newname=0x%x",
				kr, *port, fp);
	*port = (mach_port_t)fp;
	fp->f_magic = F_MAGIC;
	fp->f_seqno = seqno_from_port(fp);

	/*
	 * initialize number of send rights created and released
	 */
	fp->f_makesend = fp_proto->f_makesend;
	fp->f_relsend = fp_proto->f_relsend;
	fp->f_count = fp->f_makesend - fp->f_relsend;

	so->vs_fport = (mach_port_t)fp;

	fp->f_data = (caddr_t)so;
	fp->f_ops = &socketops;
	fp->f_type = DTYPE_SOCKET;
	fp->f_flag = FREAD|FWRITE;
	if ( so->so_state & SS_CANTRCVMORE )
		fp->f_flag &= ~FREAD;
	if ( so->so_state & SS_CANTSENDMORE )
		fp->f_flag &= ~FWRITE;
	ASSERT(fp->f_flag & (FREAD|FWRITE));
#if	SER_COMPAT
	fp->f_funnel = FUNNEL_NULL;
#endif

	*fpp = fp;
	return ESUCCESS;
}


/*
 * commit to relocate -- add file port to local task,
 * and fix up socket bind port.
 */
un_recvreloc_succeeded(
	struct socket *so)
{
	kern_return_t		kr;
	mach_port_urefs_t	rrefs;
	struct unpcb *unp = sotounpcb(so);
	struct vnode *vp;

	/*
	 * Resume file and bound socket service for this socket. 
	 * Relocation and connection both succeeded.
	 */
	if( so->vs_bindport ) {
		MAP_PORT_TO_SOCKET(so->vs_bindport, so);
		ux_server_add_port(so->vs_bindport);
	}
	ux_server_add_port(so->vs_fport);
}


/*
 * Connection or relocation failed late in the game.  Zero out port
 * fields so that they aren't destroyed when the socket is destroyed. 
 * (They will be passed back to the caller in the return mach
 * message.)
 */
un_recvreloc_failed(
	struct socket *so)
{
	struct unpcb *unp = sotounpcb(so);

	so->vs_bindport = MACH_PORT_NULL;
	unp->unp_vnodeport = MACH_PORT_NULL;
}


/*
 * vsock related rights have been shipped to the connection node,
 * so clear out local references to them (this includes the
 * socket port receive right and the vnode port send right).
 */
un_sendreloc_succeeded(
	struct socket *so)
{
	struct unpcb *unp = sotounpcb(so);

	if( so->vs_bindport ) {
		so->vs_bindport = MACH_PORT_NULL;
		unp->unp_vnodeport = MACH_PORT_NULL;
		UNDEBUG(U_RELOC,
	 ("un_sendreloc_succeeded: freeing unp_addr 0x%x: %s, M_TYPE=0x%x\n",
		     unp->unp_addr, (unp->unp_addr->m_dat +2),
		     unp->unp_addr->m_type ));
		m_freem(unp->unp_addr);
		unp->unp_addr = 0;
	}
}


/*
 * do any necesary fixups 
 */
un_sendreloc_failed(
	struct socket *so, 
	portv_t *srights,
	portv_t *rrights)
{
	struct unpcb *unp = sotounpcb(so);
	mach_port_t right;
	int kr;

	if( unp->unp_addr ) {
		/*
		 * fix up modification made by call to
		 * sgd_mbuf_append() in un_socket_append()
		 */
		unp->unp_addr->m_data += (int)unp->unp_addr;
	}

	/* XXX - must fix up mbufs in recv and send sockbufs also! */
}


/*****************************************************************************/
/*
 * This section contains connected socket pair relocation calls
 */
/*****************************************************************************/


/*
 *  Returns ESUCCESS iff socket and partner were successfully
 *  relocated, an error code otherwise.
 */

int
un_really_relocate(
	struct socket *so)
{
	sgd_t		*sgdp;
	sgde_t		*hdr, *sgde;
	lmv_t		*lmvp;
	un_sock_t	*si;
	portv_t		*srights, *rrights;
	struct socket	*so2;
	struct file	*fp, *fp2;
	int		ret;
	mach_port_t	server_port;
	struct unpcb *unp = sotounpcb(so);
	struct unpcb *unp2;

#ifdef	UN_DEBUG
	if ( ! un_enable_relocation ) {
		UNDEBUG(U_RELOC,("un_really_relocate: disabled\n"));
		return EINVAL;
	}
#endif	/* UN_DEBUG */

	if ( (so2 = sopartner(so)) == NULL )
		panic("un_really_relocate: not a socket pair\n");

	fp = (struct file *)so->vs_fport;
	fp2 = (struct file *)so2->vs_fport;
	unp2 = sotounpcb(so2);

	/*
	 *  Allocate expandable vectors for the socket graph
	 *  descriptor, the large mbuf vector, and the port
	 *  vector.
	 */
	if ( (sgdp = SGD_ALLOC()) == NULL )
		panic("un_really_reloc: cannot allocate sgd\n");
	if ( (lmvp = LMV_ALLOC()) == NULL )
		panic("un_really_reloc: cannot allocate lmv\n");
	if ( (rrights = PV_ALLOC()) == NULL )
		panic("un_really_reloc: cannot allocate rright portvec\n");
	if ( (srights = PV_ALLOC()) == NULL )
		panic("un_really_reloc: cannot allocate sright portvec\n");

	/*
	 *  Label the sgd's header descriptor---we are sending a pipe!
	 *  XXX Perhaps this ought to be incorporated into SGD_ALLOC(), eh?
	 */
	hdr = SGD_NEXT(sgdp, 0);
	hdr->sgde_type = SGDE_TYPE_SGDHDR;
	hdr->sgd_type = SGD_TYPE_UN_PAIR;
	hdr->sgd_inlsize = 0;
	hdr->sgd_outlcnt = 0;
	hdr->sgd_len = 1;
	
	/*
	 *  The socket's ucred credentials structure comes
	 *  first.  It must be installed in the dummy proc structure
	 *  on the new node prior to calling socreate(), so that the
	 *  file structs for the new socket get the same cred info as
	 *  on the old node.
	 */
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_UCRED, fp->f_cred, sizeof(struct ucred));

	/*
	 * In the case of stream socket pairs, the ucred structure
	 * is likely to be different.  (It is always the same
	 * for pipe pairs.)  Check for this and send the second
	 * ucred structure if necessary.
	 */
	if( fp->f_cred != fp2->f_cred ) {
		sgde = SGD_NEXT(sgdp, 0);
		MAKE_SIMPLE_SGDE(sgdp, sgde,
		  SGDE_TYPE_UCRED, fp2->f_cred, sizeof(struct ucred));
	}

	/*
	 * Call un_socket_append() for each end of the
	 * socketpair, so that the sgd contains marshalling info for
	 * the whole pair.  Append the un_sock_t data so new node will
	 * know who the reader is, etc.
	 */
	un_socket_append(so, sgdp, lmvp, srights, rrights);
	un_socket_append(so2, sgdp, lmvp, srights, rrights);

	/*
	 *  The un_sock_t information follows the sockets in the sgd...
	 */
	si = GET_UN_SOCKINFO(so);
	ASSERT(si != NULL);
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_UN_INFO, si, sizeof(un_sock_t));

	/*
	 *  Get TNC server port for node
	 */
	ASSERT(si->si_newnode != this_node);
	ret = tnc_get_server_port(si->si_newnode, &server_port);
	if ( ret != ESUCCESS ) {
		printf("un_really_relocate: cannot get node %d TNC port, errno %d\n",
		       si->si_newnode, ret);
	} else {
		/* Relocate the pipe. */
		ret = tnc_relocate(server_port, sgdp, lmvp, rrights, srights);

		if( ret == ESUCCESS ) {
			un_sendreloc_succeeded(so);
			un_sendreloc_succeeded(so2);
		} else {
			PV_RESET(srights);
			PV_RESET(rrights);
			un_sendreloc_failed(so, srights, rrights);
			un_sendreloc_failed(so2, srights, rrights);
		}
	}


	/*
	 *  Whether it succeeded or not, we still need to deallocate
	 *  expandable vectors created here.
	 */
	SGD_DEALLOC(sgdp);
	LMV_DEALLOC(lmvp);
	PV_DEALLOC(rrights);
	PV_DEALLOC(srights);

	return ret;
}


	
/*
 *  Common pipe/socketpair arrival routine invoked by TNC server
 *  on the new node.
 */
int
un_arrival(
	mach_port_t	server_port,
	sgd_t		*sgdp,
	caddr_t		inline_data,
	lmv_t		*lmvp,
	portv_t		*rrights,
	portv_t		*srights)
{
	int		error, err2;
	uthread_t	uth;
	sgde_t		*sgde;
	struct socket	*so = NULL, *so2 = NULL;
	un_sock_t	*si = NULL, *si2 = NULL;
	struct ucred	*fp2_cred = NOCRED;

#define DataAddress(sgde)	(&inline_data[(int)sgde->sgde_base])

	uth = current_thread();
	ASSERT(uth->uu_procp);	/* Should have dummy proc structure by now */

	ASSERT(SGD_LOOKAHEAD(sgdp) == SGD_HEADER(sgdp));
	(void) SGD_NEXT(sgdp, 1);	/* Skip over the header. */

	/*
	 *  Install the ucred credentials.
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde->sgde_type == SGDE_TYPE_UCRED);
	UNDEBUG(U_RELOC,("un_arrival: replacing p_rcred 0x%x\n",
			 uth->uu_procp->p_rcred));
	if ( uth->uu_procp->p_rcred != NOCRED )
		crfree(uth->uu_procp->p_rcred);
	uth->uu_procp->p_rcred = crdup(DataAddress(sgde));


	/*
	 * There may be a second ucred structure for the
	 * second socket.
	 */
	if( SGD_LOOKAHEAD(sgdp)->sgde_type == SGDE_TYPE_UCRED ) {
		sgde = SGD_NEXT(sgdp, 1);
		fp2_cred = crdup(DataAddress(sgde));
	}


	/*
	 *  Create sockets and their mbufs, based on the copies of
	 *  sockets and mbufs received in the relocation request
	 *  message.
	 */
	error = un_socket_extract(&so, sgdp, inline_data, lmvp, 
	  srights, rrights);
	if ( error != ESUCCESS ) {
		UNDEBUG(U_RELOC,("un_arrival: so extract error %d\n", error));
		goto out1;
	}
	error = un_socket_extract(&so2, sgdp, inline_data, lmvp, 
	  srights, rrights);
	if ( error != ESUCCESS ) {
		UNDEBUG(U_RELOC,("un_arrival: so2 extract error %d\n", error));
		goto out2;
	}

	/*
	 *  Extract TNC socket pair relocation policy info.
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde && sgde->sgde_type == SGDE_TYPE_UN_INFO);
	ASSERT(sgde->sgde_len == sizeof(un_sock_t));
	si = (un_sock_t *)DataAddress(sgde);

	/*
	 *  That should be the last entry in the sgd.
	 */
	ASSERT(SGD_NEXT(sgdp, 1) == NULL);

	/*
	 * install second ucred in second socket if it exists.
	 */
	if( fp2_cred != NOCRED )
		((struct file *)so2->vs_fport)->f_cred = fp2_cred;

	/*
	 *  Reconnect the sockets.
	 *  Ensure that the associated TNC relocation policy info
	 *  is correct.
	 */
	if ( error = VSOP_CONNECT2(so, so2) )
		panic("un_arrival: VSOP_CONNECT2(0x%x,0x%x) failed: %d\n",
		      so, so2, error);

	ASSERT(so->so_lock == so2->so_lock);
	si2 = GET_UN_SOCKINFO(so2);
	ASSERT(si->si_newnode == this_node);
	si2->si_reader = si->si_reader;
	si2->si_rdrnode = this_node;
	si2->si_newnode = INVALID_NODE;

	/*
	 *  Cred info we installed earlier should have been inherited
	 *  by the file structs.
	 */
#ifdef	UN_DEBUG
	{
		struct file *fp = (struct file *)so->vs_fport;
		struct file *fp2 = (struct file *)so2->vs_fport;
		uid_t uid = uth->uu_procp->p_rcred->cr_uid;
		gid_t gid = uth->uu_procp->p_rcred->cr_gid;

		if ( uid != fp->f_cred->cr_uid ||
		    (fp2_cred == NOCRED && uid != fp2->f_cred->cr_uid) ||
		    gid != fp->f_cred->cr_gid ||
		    (fp2_cred == NOCRED && gid != fp2->f_cred->cr_gid) )
			printf("un_arrival: uid or gid mismatch!");
			/* panic("un_arrival: ucred mismatch!"); */
	}
#endif	/* UN_DEBUG */

	/*
	 * connect succeeded
	 * commit to relocate -- fix up any remaining fields
	 * that need to be fixed.
	 */
	un_recvreloc_succeeded(so);
	un_recvreloc_succeeded(so2);

	UNDEBUG(U_RELOC,("un_arrival: OK, so 0x%x fp 0x%x so2 0x%x fp2 0x%x\n",
			 so, so->vs_fport, so2, so2->vs_fport));

	return ESUCCESS;

	/*
	 *  Error unwind.
	 */
 out2:
	un_recvreloc_failed(so);
	fdealloc((struct file *)so->vs_fport);
	if ( err2 = VSOP_CLOSE(so) ) {
		UNDEBUG(U_RELOC,("un_arrival: VSOP_CLOSE(0x%x) error: %d\n",
				 so2, err2));
	}
 out1:
	if( fp2_cred )
		crfree(fp2_cred);

	return error;

#undef DataAddress
}

