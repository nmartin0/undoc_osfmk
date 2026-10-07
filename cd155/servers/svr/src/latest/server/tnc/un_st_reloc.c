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
 * $Log: un_st_reloc.c,v $
 * Revision 1.11  1995/02/01  22:10:14  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:45:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/05/12  15:51:04  nina
 *  Reviewer:mjl
 *  Risk:Low
 *  Benefit or PTS #:6530
 *  Testing:Test case specified in bug report
 *  Module(s):./server/tnc/un_st_reloc.c
 *
 *  If the connect() request fails, the function
 *  un_vsock_recvconnect() must return the file
 *  port for the socket to the caller.
 *
 * Revision 1.8  1993/09/01  01:37:46  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.7  1993/07/14  18:36:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:50:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:29:07  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:22  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:10:10  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:07:01  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:30  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:45  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.5  93/08/26  10:48:04  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.
 * 
 * Revision 3.4  93/08/23  00:09:51  mjl
 * Get rid of extraneous extern decl.
 * 
 * Revision 3.3  92/12/10  17:21:02  mjl
 * Use new debug macros.  Use new port-to-socket mapping macros.
 *
 * Revision 3.2  92/11/18  12:49:49  mjl
 * uth->uu_opn_filep now cleared by common arrival_dispatch() code.
 * 
 * Revision 3.1  92/08/17  13:12:50  mjl
 * Make server port (socket id) be first arg to relocation RPCs.
 * 
 * Revision 3.0  92/08/08  01:22:13  jdh
 * relocation code for connecting stream sockets -- jdh
 * 
 * Revision 3.1  92/06/15  17:38:17  mjl
 * Fix bogus un_really_relocate() return value when relocation is disabled.
 * Macro SOSETFP() is now obsolete.  Initialize uu_opn_filep thread field to
 * prevent assertion failure in un_socreate().
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



/*****************************************************************************/
/*
 * This section contains unix-domain stream socket connect calls
 */
/*****************************************************************************/

/*
 * This routine bundles and sends the connecting socket to the
 * listening socket's node, where the remote routine
 * (un_vsock_recvconnect()) creates a local version of the socket,
 * and completes the connection
 */
int
un_vsock_sendconnect(
	struct socket *so,		/* connecting socket */
	mach_port_t sockid)		/* socket port for listening socket */
{
	sgd_t		*sgdp;
	sgde_t		*hdr, *sgde;
	lmv_t		*lmvp;
	portv_t		*srights;
	portv_t		*rrights;
	struct file	*fp;
	int		ret;
	struct unpcb *unp = sotounpcb(so);

#ifdef	UN_DEBUG
	if ( ! un_enable_relocation ) {
		UNDEBUG(U_RELOC,
			("un_vsock_sendconnect: relocation disabled\n"));
		return EINVAL;
	}
#endif	/* UN_DEBUG */

	UNDEBUG(U_RELOC,("un_vsock_sendconnect: so 0x%x fp 0x%x sockid 0x%x\n",
			 so, so->vs_fport, sockid));

	fp = (struct file *)so->vs_fport;

	/*
	 *  Allocate expandable vectors for the socket graph
	 *  descriptor, the large mbuf vector, and the port
	 *  vector.
	 */
	if ( (sgdp = SGD_ALLOC()) == NULL )
		panic("un_vsock_sendconnect: cannot allocate sgd\n");
	if ( (lmvp = LMV_ALLOC()) == NULL )
		panic("un_vsock_sendconnect: cannot allocate lmv\n");
	if ( (srights = PV_ALLOC()) == NULL )
		panic("un_vsock_sendconnect: cannot allocate srights\n");
	if ( (rrights = PV_ALLOC()) == NULL )
		panic("un_vsock_sendconnect: cannot allocate rrights\n");


	/*
	 *  Label the sgd's header descriptor---we are sending a VSOCK connect
	 *  request.
	 */
	hdr = SGD_NEXT(sgdp, 0);
	hdr->sgde_type = SGDE_TYPE_SGDHDR;
	hdr->sgd_type = SGD_TYPE_VSOCKCONN;
	hdr->sgd_inlsize = 0;
	hdr->sgd_outlcnt = 0;
	hdr->sgd_len = 1;

	/*
	 *  The sockets ucred credentials structure comes
	 *  first.  It must be installed in the dummy proc structure
	 *  on the new node prior to calling socreate(), so that the
	 *  file struct for the new socket gets the same cred info as
	 *  on the old node.
	 */
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_UCRED, fp->f_cred, sizeof(struct ucred));


	un_socket_append(so, sgdp, lmvp, srights, rrights);

	/*
	 * Don't need un_sock_t structure.  If the connect succeeds,
	 * the current structure will no longer be valid.  It says to
	 * follow one of the readers of this socket.  If the connect
	 * succeeds, this socket will end up sharing the un_sock_t
	 * of the socket which accepts this connection.  The local
	 * un_sock_t will be deallocated along with everything else
	 * when this file structure and socket are deallocated.
	 */

	UNPORTINFO(U_PORT,
		   ((mach_port_t)so->vs_bindport, "bindport before relocate"));
	UNPORTINFO(U_PORT,(so->vs_fport, "fp before relocate"));
	UNPORTINFO(U_PORT,(unp->unp_vnodeport, "vsock before relocate"));

	/* Move connecting socket to listening socket's node. */
	ret = tnc_relocate(sockid, sgdp, lmvp, rrights, srights);

	UNPORTINFO(U_PORT,
		   ((mach_port_t)so->vs_bindport, "bindport after relocate"));
	UNPORTINFO(U_PORT,(so->vs_fport, "fp after relocate"));
	UNPORTINFO(U_PORT,(unp->unp_vnodeport, "vsock after relocate"));

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
	SGD_DEALLOC(sgdp);
	LMV_DEALLOC(lmvp);
	PV_DEALLOC(rrights);
	PV_DEALLOC(srights);

	return ret;
}


/*
 * This arrival routine invoked by TNC server on the new node (through
 * either svr_tnc_relocate_inline() or svr_tnc_relocate_ool().
 *
 * This routine unbundles the connecting socket and creates a local
 * version of the socket and file struct associated with it.  It then
 * calls unp_local_connect() to complete the connection.
 */
int
un_vsock_recvconnect(
	mach_port_t	sockid,
	sgd_t	*sgdp,
	caddr_t	inline_data,
	lmv_t	*lmvp,
	portv_t	*rrights,
	portv_t	*srights)
{
	int		error, err2;
	uthread_t	uth;
	kern_return_t	kr;
	sgde_t		*sgde;
	struct socket	*so = NULL, *so2 = NULL;
	mach_port_t	*fpport;

#define DataAddress(sgde)	(&inline_data[(int)sgde->sgde_base])

	UNDEBUG(U_RELOC,("un_vsock_recvconnect(sockid=0x%x)\n", sockid));

	/*
	 *  Get port to socket mapping for with this socket port.
	 *  If there is none, something funny's going on...
	 *
	 *  An earlier revision of this routine tried to check for so2
	 *  closing and return ECONNREFUSED, but unfortunately that check
	 *  is not possible until sonewconn() is actually called.  It
	 *  all has to do with the SP_CLOSING flag in so->so_special.
	 */
	PORT_TO_SOCKET_LOOKUP(sockid, so2);
	if (so2 == NULL)
		panic("un_vsock_recvconnect: socket port 0x%x not mapped!\n",
		      sockid);


	uth = current_thread();
	ASSERT(uth->uu_procp);	/* Should have dummy proc structure by now */

	ASSERT(SGD_LOOKAHEAD(sgdp) == SGD_HEADER(sgdp));
	(void) SGD_NEXT(sgdp, 1);	/* Skip over the header. */

	/*
	 *  Install the ucred credentials.
	 */
	sgde = SGD_NEXT(sgdp, 1);
	ASSERT(sgde->sgde_type == SGDE_TYPE_UCRED);
	UNDEBUG(U_RELOC,("un_vsock_recvconnect: replacing p_rcred 0x%x\n",
			 uth->uu_procp->p_rcred));
	if ( uth->uu_procp->p_rcred != NOCRED )
		crfree(uth->uu_procp->p_rcred);
	uth->uu_procp->p_rcred = crdup(DataAddress(sgde));


	error = un_socket_extract(&so, sgdp, inline_data, 
				  lmvp, srights, rrights);
	if ( error != ESUCCESS ) {
		UNDEBUG(U_RELOC,("un_vsock_recvconnect: so extract error %d\n",
				 error));
		goto out0;
	}


	/*
	 *  That should be the last entry in the sgd.
	 */
	ASSERT(SGD_NEXT(sgdp, 1) == NULL);

	/*
	 * sockinfo was not shipped over, as this socket will use the
	 * sockinfo of the socket to which it is connecting if the
	 * connect is successful.
	 */
	so->vs_data = NULL;

#ifdef	UN_DEBUG
	/*
	 *  Cred info we installed earlier should have been inherited
	 *  by the file struct.
	 */
	{
		struct file *fp = (struct file *)so->vs_fport;
		uid_t uid = uth->uu_procp->p_rcred->cr_uid;
		gid_t gid = uth->uu_procp->p_rcred->cr_gid;

		if (uid != fp->f_cred->cr_uid ||
		    gid != fp->f_cred->cr_gid)
			/* ASSERT("uid or gid mismatch!"); */
			panic("un_vsock_recvconnect: ucred mismatch!");
	}
#endif	/* UN_DEBUG */

	/*
	 *  Perform the socket connection.
	 *
	 * When this routine returns, so will be locked.  If the
	 * connection was successful, the new accept socket will also
	 * be locked.  Both sockets must be unlocked before returning
	 * to the calling node.  They share a lock structure, so just 
	 * unlocking so will be sufficient in all cases.
	 */
	if( error = unp_local_connect(so, so2) )
		goto out1;


	/*
	 * connect succeeded
	 * commit to relocate -- fix up any remaining fields
	 * that need to be fixed.
	 */
	un_recvreloc_succeeded(so);

	UNDEBUG(U_RELOC,
		("un_vsock_recvconnect: OK, so 0x%x fp 0x%x so2 0x%x\n",
		 so, so->vs_fport, so2));

	SOCKET_UNLOCK(so);

	SOCKET_LOOKUP_DONE(so2, TRUE);

	return ESUCCESS;

/*
 * error unwind
 */
 out1:
	SOCKET_UNLOCK(so);

	un_recvreloc_failed(so);

	/*
	 * The operation failed. We need to return the
	 * file port to the caller. Therefore, set F_RELOC
	 * to prevent fdealloc() from destroying it.
	 */
        ASSERT( ((struct file *)so->vs_fport)->f_magic == F_MAGIC );
        ((struct file *)so->vs_fport)->f_magic = F_RELOC;
	
	fdealloc((struct file *)so->vs_fport);
	if ( err2 = VSOP_CLOSE(so) ) {
	    UNDEBUG(U_RELOC,("un_arrival: VSOP_CLOSE(0x%x) error: %d\n",
			     so2, err2));
	}
 out0:
	SOCKET_LOOKUP_DONE(so2, TRUE);
	return error;

#undef DataAddress
}



