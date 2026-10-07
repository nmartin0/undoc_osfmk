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
 * $Log: sgd.c,v $
 * Revision 1.9  1995/02/01  21:52:56  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:44:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/09/01  01:36:52  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.6  1993/07/14  18:34:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:47:53  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:25:13  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:10  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:09:28  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:06:20  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/15  02:02:58  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1  1992/11/05  22:46:38  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.4  93/08/23  00:05:34  mjl
 * Get rid of i860 compiler warnings.
 * 
 * Revision 3.3  92/12/10  17:09:28  mjl
 * Use newer debug macros.  The mli_*() routines now only need a (struct mbuf **)
 * and not a whole struct sockbuf.  When marshalling mbufs, don't trash the
 * original's m_data pointer---only change the marshalled copy!
 *
 * Revision 3.2  92/08/17  13:07:23  mjl
 * Delete bogus assertions that would prevent relocation of network sockets.
 * 
 * Revision 3.1  92/06/26  18:02:02  mjl
 * Fix transfer of sockbuf flow control data.
 * 
 * Revision 3.0  92/06/11  16:00:20  mjl
 * Routines for manipulating "socket graph descriptors", which are basically
 * dope vectors used for marshalling and unmarshalling arbitrary data
 * structure graphs.
 * 
 */

/*
 *  Routines for socket graph descriptor (sgd_t) manipulation, and
 *  also routines for condensing (extracting) common aggregate
 *  data structure graphs into (from) sgd's and lmv's.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <sys/domain.h>
#include <sys/unpcb.h>
#include <sys/errno.h>

#include <tnc/sgd.h>
#include <tnc/reloc.h>
#include <tnc/un_debug.h>

/* Forward delcarations */
void	sgd_mbuf_append (sgd_t *, lmv_t *, struct mbuf *, int);
int	sgd_um_mbuf (sgde_t *, caddr_t, lmv_t *, struct mbuf **);


/*
 *  Append entries for a socket and its mbufs to an sgd.
 *
 *  This routine does no locking (we assume any ports referrencing
 *  the socket are no longer being serviced in ux_server_loop()).
 *  For real network protocols like AF_INET, TNC will never try to
 *  relocate a socket with a live connection, so there is also no need
 *  to worry about locking mbuf lists to prevent their being manipulated
 *  from interrupt level.  (XXX Right, Brad?)
 *
 *  (Descriptor creation and actual marshalling could eventually be
 *  done in a single pass, but for now we do seperate passes just to
 *  keep things comprehensible.)
 *
 *  NB that no protocol control block information is placed in the sgd
 *  or lmv vectors, only the socket and its mbufs.  It is the
 *  responsibility of of the use-specific relocation code to add any
 *  additional data to the sgd.  This generic routine cannot know the
 *  size of protocol-specific pcb data.
 */
void
sgd_socket_append(
	sgd_t	*sgdp,
	lmv_t	*lmvp,
	struct socket	*so)
{
	sgde_t		*sgde;
	struct mbuf	*mp;
	mach_port_t	*portp;
	mblist_i_t	mli;
	int		newchain;

	/* SOCKET_LOCK(so); */

	/*
	 *  Descriptor entry for the socket itself
	 */
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde,
			 SGDE_TYPE_SOCKET, so, sizeof(struct socket));
	SGDE_MARK_SOCKET_TYPE(sgde,
			      sodomain(so)->dom_family,
			      so->so_type,
			      so->so_proto->pr_protocol);

	/*
	 *  Dummy descriptor to mark start of send queue mbufs
	 */
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde, SGDE_TYPE_SNDSB, NULL, 0);

	/* SOCKBUF_LOCK(); */
	if ( so->so_snd.sb_mb ) {
		MLI_INIT(&mli, &so->so_snd.sb_mb);
		while ( (mp = MLI_NEXT(&mli, &newchain)) != NULL )
			sgd_mbuf_append(sgdp, lmvp, mp, newchain);
	}
	/* SOCKBUF_UNLOCK(); */

	/*
	 *  Dummy descriptor to mark start of receive queue mbufs
	 */
	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde, SGDE_TYPE_RCVSB, NULL, 0);

	/* SOCKBUF_LOCK(); */
	if ( so->so_rcv.sb_mb ) {
		MLI_INIT(&mli, &so->so_rcv.sb_mb);
		while ( (mp = MLI_NEXT(&mli, &newchain)) != NULL )
			sgd_mbuf_append(sgdp, lmvp, mp, newchain);
	}
	/* SOCKBUF_UNLOCK(); */

	/* SOCKET_UNLOCK(so); */
}


/*
 *  Append appropriate sgd and lmv entries for an mbuf.  (The
 *  marshalling routine will take care of transforming the m_data
 *  field into an offset rather than a pointer.)
 */
void
sgd_mbuf_append(
	sgd_t	*sgdp,
	lmv_t	*lmvp,
	struct mbuf *mp,
	int	ischainhdr)
{
	lmve_t	*iov;
	sgde_t	*sgde;

	sgde = SGD_NEXT(sgdp, 0);
	MAKE_SIMPLE_SGDE(sgdp, sgde, mp->m_type, mp, sizeof(struct mbuf));
	sgde->sgde_xflags = ( ischainhdr ? SGDE_MB_CHAINHDR : 0 );

	if ( mp->m_flags & M_EXT ) {
		SGD_HEADER(sgdp)->sgd_outlcnt++;
		iov = LMV_NEXT(lmvp, 0);
		iov->iov_base = mp->m_ext.ext_buf;
		iov->iov_len = mp->m_ext.ext_size;
	}
}


/*
 *  Marshal the data described by an sgd into a pre-allocated buffer.
 *
 *  As sgd entries are marshalled, the sgd itself is modified to
 *  contain offsets into the buffer rather than pointers to memory.
 *  Thus the sgd can be used to unmarshal the buffer when it arrives
 *  on the new node.
 */
void
sgd_data_marshal(
	sgd_t	*sgdp,		/* descriptor to marshal */
	caddr_t	buf)		/* address to marshal into */
{
	register caddr_t m;	/* marshalling pointer */
	register sgde_t *sgde;	/* current sgd entry */
	int decrement;
#define	Mbuf(x)	((struct mbuf *)x)

	m = buf;
	SGD_RESET(sgdp);
	while ( (sgde = SGD_NEXT(sgdp, 1)) != NULL ) {

		/*
		 *  Only do the data marshalling for sgd entries
		 *  that correspond to real data structures (as
		 *  opposed to markers, e.g. SGDE_TYPE_SNDSB).
		 */
		if ( SGDE_HAS_DATA(sgde->sgde_type) ) {

			/* Copy the data... */
			bcopy(sgde->sgde_base, m, sgde->sgde_len);

			/*
			 *  For mbufs, we need to make the marshalled
			 *  m_data pointer be an offset, either into
			 *  the mbuf itself or into its attached
			 *  buffer area (for M_EXT).
			 *
			 *  NB this is just one instance of the problem
			 *  of imbedded pointers.  We don't attempt to
			 *  solve this in a more general way.
			 */
			if ( sgde->sgde_type < MT_MAX ) {
				if ( Mbuf(m)->m_flags & M_EXT ) {
				    decrement = (int)Mbuf(m)->m_ext.ext_buf;
				} else {
				    decrement = (int)sgde->sgde_base;
				}
				Mbuf(m)->m_data -= decrement;
			}

			/*
			 *  The sgde_base was a pointer to data; now make
			 *  it an offset into the marshalled data buffer.
			 */
			sgde->sgde_base = m - (int)buf;

			/* Point at next free byte in the target buffer. */
			m += sgde->sgde_len;
		}
	}
#undef Mbuf
}


/*
 *  Create real sockets, mbufs, etc. from the relocation
 *  data received.  This is the inverse of sgd_socket_append().
 */
int
sgd_socket_extract(
	sgd_t	*sgdp,		/* sgd to guide socket extraction */
	caddr_t	buf,		/* marshalled non-large-mbuf data */
	lmv_t	*lmvp,		/* vector of large mbuf buffers */
	struct socket **aso)	/* resulting extracted socket */
{
	sgde_t		*sgde;
	struct socket	*so = NULL;
	struct sockbuf	*sb;
	struct mbuf	*mp;
	int		error;
	mblist_i_t	mli;

	/*
	 *  Extract the socket itself
	 */
	sgde = SGD_NEXT(sgdp, 1);
	if ( sgde == NULL || sgde->sgde_type != SGDE_TYPE_SOCKET ) {
		UNDEBUG(U_SGD,
			("sgd_socket_extract: sgd out of sync, type %d\n",
			 sgde->sgde_type));
		return EINVAL;	/* XXX MIG_BAD_ARGUMENTS more accurate err? */
	}
	if ( error = sgd_um_socket(sgde, buf, aso) )
		return error;
	so = *aso;

	/*
	 *  Extract mbufs for the send and receive queues, doing
	 *  lookahead to detect the end of the mbuf sgde's.
	 */
	MLI_DONE(&mli);
	while ( (sgde = SGD_LOOKAHEAD(sgdp)) != NULL ) {
		switch ( sgde->sgde_type ) {

		case SGDE_TYPE_RCVSB:
			MLI_INIT(&mli, &(so->so_rcv.sb_mb));
			break;

		case SGDE_TYPE_SNDSB:
			MLI_INIT(&mli, &(so->so_snd.sb_mb));
			break;

		default:
			if ( sgde->sgde_type < MT_MAX ) {
				ASSERT(MLI_READY(&mli));
				error = sgd_um_mbuf(sgde, buf, lmvp, &mp);
				if ( error != ESUCCESS )
					return error;
				MLI_PUT(&mli, mp,
					sgde->sgde_xflags & SGDE_MB_CHAINHDR);
			} else {
				/*
				 *  Looking at a non-mbuf, so we are done
				 */
				return ESUCCESS;
			}
			break;
		} /* switch */

		/*
		 *  Consume the sgde just extracted
		 */
		sgde = SGD_NEXT(sgdp, 1);
	} /* while */

	return ESUCCESS;
}


/*
 *  Unmarshal a socket.  Allocates a socket locally and copies relevant
 *  data fields from the socket received from the old node.
 */
int
sgd_um_socket(sgde, buf, aso)
	sgde_t *sgde;
	caddr_t buf;
	struct socket **aso;
{
	int error;
	register struct socket *oso;
	register struct socket *nso;

	ASSERT(sgde->sgde_type == SGDE_TYPE_SOCKET);

	oso = (struct socket *)&buf[(int)sgde->sgde_base];

	/*
	 *  The sockbuf wakeup up-call routines are only used by XTI, the
	 *  streams module that translates stream operations to socket
	 *  operations.  Since a stream's file port will never receive
	 *  a migrate notification, we expect these function pointers
	 *  to be NULL.
	 */
	ASSERT(oso->so_snd.sb_wakeup == NULL && oso->so_rcv.sb_wakeup == NULL);

	error = SOCREATE( SGDE_SO_FAMILY(sgde),
			  aso,
			  SGDE_SO_TYPE(sgde),
			  SGDE_SO_PROTO(sgde) );
	if ( error ) {
		UNDEBUG(U_SGD,
			("sgd_um_socket: SOCREATE failed: %d\n", error));
		*aso = NULL;
		return error;
	}

	nso = *aso;
	sgde->sgde_base = (caddr_t)nso;
	sgde->sgde_flags |= SGDE_UNMARSHALLED;

#define COPYMEMBER(member)	nso->member = oso->member

	COPYMEMBER(so_options);
	COPYMEMBER(so_linger);
	COPYMEMBER(so_state);
	COPYMEMBER(so_timeo);
	COPYMEMBER(so_error);
	COPYMEMBER(so_special);
	COPYMEMBER(so_pgid);
	COPYMEMBER(so_oobmark);

	/*
	 *  WARNING: Values for sb_hiwat, sb_lowat, sb_max, and
	 *  sb_mbmax reflect mbuf pool usage on the old node.
	 *  Currently, the OSF/1AD soreserve() and sbreserve()
	 *  routines don't actually have any side effects beyond the
	 *  socket and sockbuf structures; for instance, they don't
	 *  cause any kernel heaps to be expanded.  When porting this
	 *  code to other operating systems, however, be sure to check
	 *  for such side effects in sbreserve() and soreserve().  If
	 *  any such side effects exist, it may be necessary to record
	 *  any setsockopt(2) system calls and replay them on the new
	 *  node after socket relocation.
	 */

	COPYMEMBER(so_snd.sb_cc);
	COPYMEMBER(so_snd.sb_hiwat); /* */
	COPYMEMBER(so_snd.sb_mbcnt); /* */
	COPYMEMBER(so_snd.sb_mbmax); /* */
	COPYMEMBER(so_snd.sb_lowat); /* */
	COPYMEMBER(so_snd.sb_flags);
	COPYMEMBER(so_snd.sb_timeo);

	COPYMEMBER(so_rcv.sb_cc);
	COPYMEMBER(so_rcv.sb_hiwat); /* */
	COPYMEMBER(so_rcv.sb_mbcnt); /* */
	COPYMEMBER(so_rcv.sb_mbmax); /* */
	COPYMEMBER(so_rcv.sb_lowat); /* */
	COPYMEMBER(so_rcv.sb_flags);
	COPYMEMBER(so_rcv.sb_timeo);

#ifdef	NOTDEF
	/*
	 *  XXX New security tags are initialized for the new socket
	 *  in SOCREATE() above.  Are the tag values transferrable
	 *  from node to node in the multicomputer?  Must they be
	 *  mutated in some way in order for them to be valid on the
	 *  new node?
	 */
#ifdef	SEC_ARCH
	for ( i = 0; i < SEC_TAG_COUNT; i++ )
		COPYMEMBER(so_tag[i]);
#endif
#endif	/* NOTDEF */

#undef COPYMEMBER

	*aso = nso;
	return ESUCCESS;
}


/*
 *  Unmarshal an mbuf.  Allocates an mbuf on the new node and
 *  overwrites it with the data from the old node mbuf.
 *
 *  If it is a large mbuf (M_EXT flag set), we get its buffer space
 *  from the lmv vector and supply our own deallocation routine
 *  (because we know the buffer arrived as an out-of-line memory
 *  region in a Mach IPC call, and must be vm_deallocate()'ed when
 *  the mbuf is freed).
 *
 *  Recall that the m_data field is shipped as a buffer offset, not
 *  a memory address.
 */
int
sgd_um_mbuf(
	sgde_t		*sgde,
	caddr_t		buf,
	lmv_t		*lmvp,
	struct mbuf	**mpp)
{
	struct mbuf	*mp, *omp;
	struct sockbuf	*sb;
	lmve_t		*iov;
	extern void	tnc_lg_mbuf_dealloc();

	ASSERT(sgde->sgde_type < MT_MAX);

	MGET(mp, M_WAIT, sgde->sgde_type);
	omp = (struct mbuf *)&buf[(int)sgde->sgde_base];
	sgde->sgde_base = (caddr_t)mp;
	sgde->sgde_flags |= SGDE_UNMARSHALLED;
	bcopy(omp, mp, sizeof(struct mbuf));
	if ( mp->m_flags & M_EXT ) {
		if ( (iov = LMV_NEXT(lmvp, 1)) == NULL )
			return EINVAL;
		mp->m_ext.ext_buf = iov->iov_base;
		mp->m_ext.ext_size = iov->iov_len;
		mp->m_data += (int)mp->m_ext.ext_buf;
		mp->m_ext.ext_free = tnc_lg_mbuf_dealloc;
	} else {
		mp->m_data += (int)mp;
	}
	*mpp = mp;
	return ESUCCESS;
}


/*
 *  Mbuf list walking routines.
 *  When they're all debugged, make 'em macros!!!
 */
void
mli_init(
	mblist_i_t	*mli,
	struct mbuf	**mpp)
{
	mli->mli_head	= mpp;
	mli->mli_chain	= *mpp;
	mli->mli_curr	= *mpp;
	mli->mli_flags	= MLIF_INITED;
}


struct mbuf *
mli_next(
	mblist_i_t	*mli,
	int		*ischainhdr)
{
	struct mbuf *mp;

	if ( mli->mli_flags & MLIF_EOL )
		return NULL;

	mp = mli->mli_curr;
	if ( ischainhdr != NULL )
		*ischainhdr = ( mp == mli->mli_chain ? 1 : 0 );

	if (mli->mli_curr && mli->mli_curr->m_next) {
		mli->mli_curr = mli->mli_curr->m_next;
	} else if (mli->mli_chain && mli->mli_chain->m_nextpkt) {
		mli->mli_chain = mli->mli_chain->m_nextpkt;
		mli->mli_curr = mli->mli_chain;
	} else {
		/* Could not get the next mbuf! */
		mli->mli_flags |= MLIF_EOL;
	}
	return mp;
}


#ifdef	NOTDEF
struct mbuf *
mli_curr(mli)
	mblist_i_t *mli;
{
	return mli->mli_curr;
}
#endif


void
mli_put(
	mblist_i_t	*mli,
	struct mbuf	*mp,
	int		newchain)
{
	/*
	 *  mli_curr is the last mbuf that was mli_put, or NULL.
	 *  Likewise, mli_chain is the last chain in the list or NULL.
	 */
	ASSERT(!mli->mli_curr || mli->mli_curr->m_next == NULL);
	ASSERT(!mli->mli_chain || mli->mli_chain->m_nextpkt == NULL);

	if (mli->mli_curr) {
		ASSERT(mli->mli_chain);
		if (newchain) {
			mli->mli_chain->m_nextpkt = mp;
			mli->mli_chain = mp;
		} else {
			mli->mli_curr->m_next = mp;
			mli->mli_curr = mp;
		}
	} else {
		/*
		 *  First mbuf on the list.
		 */
		ASSERT(mli->mli_head != NULL);
		ASSERT(mli->mli_chain == NULL
		       && *(mli->mli_head) == NULL
		       && newchain);
		mli->mli_curr = mp;
		mli->mli_chain = mp;
		*(mli->mli_head) = mp;
	}
	mp->m_nextpkt = 0;
	mp->m_next = 0;
}
