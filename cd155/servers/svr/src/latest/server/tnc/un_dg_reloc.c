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
 * $Log: un_dg_reloc.c,v $
 * Revision 1.7  1995/02/01  22:02:52  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:44:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/01  01:37:07  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.4  1993/07/14  18:35:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:27:19  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:52  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1993/03/07  01:27:52  cfj
 * Merge from T9.
 *
 * Revision 1.1.8.1  1993/03/07  01:15:41  cfj
 * Fix to make TNC work with OSF sync. close.
 *
 * Revision 1.1  1993/01/15  02:26:29  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 3.4  93/08/26  10:47:01  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.
 * 
 * Revision 3.3  93/08/23  00:06:53  mjl
 * Get rid of extraneous extern decl.
 * 
 * Revision 3.2  93/06/14  13:59:50  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Added another argument to recreate_filestruct_for_socket()
 * 
 * Revision 3.1  93/03/10  14:18:30  yazz
 * Disable pipe relocationtemporarily until synchronous close code
 * can be written for vsockets/tnc.
 *
 * Revision 3.0  92/12/10  16:59:27  mjl
 * Socket relocation for TNC Unix domain datagram sockets.
 * 
 */


#include <sys/uio.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <uxkern/syscall_subr.h>
#include <sys/socket.h>
#include <sys/mbuf.h>
#include <vsocket/vsocket.h>
#include <vsocket/vs_types.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/errno.h>
#include <net/net_malloc.h>
#include <sys/un.h>

#include <tnc/un.h>
#include <tnc/un_dg.h>
#include <tnc/vector.h>
#include <tnc/sgd.h>
#include <tnc/reloc.h>

extern void		file_port_deallocate(struct file *);
extern int		tnc_lg_mbuf_dealloc(caddr_t, int, caddr_t);
extern int		recreate_filestruct_for_socket(struct socket *,
						       mach_port_t *,
						       struct file **,
							struct file *);

#ifdef NO_PIPE_RELOC
int un_dg_enable_relocation	= 0;
#else
int un_dg_enable_relocation	= 1;
#endif


/*
 *  Returns ESUCCESS iff socket was successfully relocated, an error
 *  code otherwise.  For datagram sockets, this is quite similar to
 *  the socket pair routine un_really_relocate(), except that we are
 *  only moving one socket.
 */
int
un_dg_really_relocate(
	struct socket *so)
{
	sgd_t		*sgdp;
	sgde_t		*hdr, *sgde;
	lmv_t		*lmvp;
	portv_t		*srights, *rrights;
	struct file	*fp;
	int		ret = ESUCCESS;
	mach_port_t	server_port;
	struct unpcb *unp = sotounpcb(so);
	policy_state_t	*ps;

	if ( ! un_dg_enable_relocation ) {
		UNDGDEBUG(U_RELOC,("un_really_relocate: disabled\n"));
		return EINVAL;
	}

	UNDGDEBUG(U_RELOC,
		  ("un_really_relocate: really relocating, so 0x%x\n", so));

	fp = (struct file *)so->vs_fport;

	/*
	 *  Allocate expandable vectors for the socket graph
	 *  descriptor, the large mbuf vector, and the port
	 *  vector.
	 */
	if ( (sgdp = SGD_ALLOC()) == NULL )
		panic("un_dg_really_reloc: cannot allocate sgd\n");
	if ( (lmvp = LMV_ALLOC()) == NULL )
		panic("un_dg_really_reloc: cannot allocate lmv\n");
	if ( (rrights = PV_ALLOC()) == NULL )
		panic("un_dg_really_reloc: cannot allocate rright portvec\n");
	if ( (srights = PV_ALLOC()) == NULL )
		panic("un_dg_really_reloc: cannot allocate sright portvec\n");

	/*
	 *  Label the sgd's header descriptor---we are sending a pipe!
	 *  XXX Perhaps this ought to be incorporated into SGD_ALLOC(), eh?
	 */
	hdr = SGD_NEXT(sgdp, 0);
	hdr->sgde_type = SGDE_TYPE_SGDHDR;
	hdr->sgd_type = SGD_TYPE_UN_DG;
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

	/* Make SGD entries for the socket and its mbufs. */
	un_socket_append(so, sgdp, lmvp, srights, rrights);

	/*
	 *  The policy state information follows the socket in the sgd...
	 */
	ps = (policy_state_t *)so->vs_data;
	ASSERT(ps != NULL);
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_UN_INFO, ps, sizeof(policy_state_t));

	/*
	 *  Get TNC server port for node
	 */
	ASSERT(ps->ps_newnode != this_node);
	ret = tnc_get_server_port(ps->ps_newnode, &server_port);
	if ( ret != ESUCCESS ) {
		printf(
		  "un_really_reloc: tnc_get_server_port(node=%d): errno %d\n",
		       ps->ps_newnode, ret);
		goto out;
	}

	/* Relocate the pipe. */
	ret = tnc_relocate(server_port, sgdp, lmvp, rrights, srights);

	if( ret == ESUCCESS ) {
		un_sendreloc_succeeded(so);
	} else {
		PV_RESET(srights);
		PV_RESET(rrights);
		un_sendreloc_failed(so, srights, rrights);
	}

	/*
	 *  Whether it succeeded or not, we still need to deallocate
	 *  expandable vectors created here.
	 */
out:
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
un_dg_arrival(
	mach_port_t	server_port,
	sgd_t	*sgdp,
	caddr_t	inline_data,
	lmv_t	*lmvp,
	portv_t	*rrights,
	portv_t	*srights)
{
	int		error;
	uthread_t	uth;
	sgde_t		*sgde;
	struct socket	*so = NULL;
	policy_state_t	*ps;

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
	UNDGDEBUG(U_RELOC,("un_dg_arrival: replacing p_rcred 0x%x\n",
			   uth->uu_procp->p_rcred));
	if ( uth->uu_procp->p_rcred != NOCRED )
		crfree(uth->uu_procp->p_rcred);
	uth->uu_procp->p_rcred = crdup(DataAddress(sgde));

	/*
	 *  Create sockets and their mbufs, based on the copies of
	 *  sockets and mbufs received in the relocation request
	 *  message.
	 */
	error = un_socket_extract(&so, sgdp, inline_data, lmvp, 
				  srights, rrights);
	if ( error != ESUCCESS ) {
		UNDEBUG(U_RELOC,("un_dg_arrival: so extract error %d\n",
				 error));
		return (error);
	}

	/*
	 *  Extract TNC socket pair relocation policy info.
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde && sgde->sgde_type == SGDE_TYPE_UN_INFO);
	ASSERT(sgde->sgde_len == sizeof(policy_state_t));
	ps = (policy_state_t *)DataAddress(sgde);

	/*
	 *  That should be the last entry in the sgd.
	 */
	ASSERT(SGD_NEXT(sgdp, 1) == NULL);

	/*
	 *  Cred info we installed earlier should have been inherited
	 *  by the file structs.
	 */
#ifdef	UN_DEBUG
	{
		struct file *fp = (struct file *)so->vs_fport;
		uid_t uid = uth->uu_procp->p_rcred->cr_uid;
		gid_t gid = uth->uu_procp->p_rcred->cr_gid;

		ASSERT(uid == fp->f_cred->cr_uid && gid == fp->f_cred->cr_gid);
	}
#endif	/* UN_DEBUG */

	/*
	 * commit to relocate -- fix up any remaining fields
	 * that need to be fixed.
	 */
	un_recvreloc_succeeded(so);

	UNDEBUG(U_RELOC,
		("un_dg_arrival: OK, so 0x%x fp 0x%x\n", so, so->vs_fport));

	return ESUCCESS;
#undef DataAddress
}

