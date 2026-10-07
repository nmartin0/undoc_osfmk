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
 * $Log: sgd.h,v $
 * Revision 1.10  1995/02/01  21:53:19  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:44:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/06/18  00:48:37  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.7  1993/07/14  18:34:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:47:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:25:24  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:15  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:09:31  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:23  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:00  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:09  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.2  1992/11/30  22:48:38  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:46:41  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.5  93/06/14  13:57:53  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Changed macro SGDE_HAS_DATA to reflect the fact that TYPE_FILE messages
 * now have data (we now ship file structs across in a relocation).
 * 
 * Revision 3.4  92/12/10  17:12:08  mjl
 * New arrival codes and sgde marker types for TNC datagram socket support.
 * Fix an ASSERT() typo.  MLI_*() macros don't need a sockbuf address anymore.
 *
 * Revision 3.3  92/11/18  12:42:38  mjl
 * Prototypes for i860 compiler happiness.
 * 
 * Revision 3.2  92/08/17  13:08:35  mjl
 * Added new arrival codes and sgd entry data types for FIFO relocation.
 * 
 * Revision 3.1  92/08/08  01:45:44  jdh
 * added some relocation data types, renamed some types to more
 * generic names -- jdh
 * 
 * Revision 3.0  92/06/11  16:02:46  mjl
 * Definitions for socket graph descriptors.
 * 
 */

/*
 *  Socket Graph Descriptor definitions
 *
 *	A socket and its associated data structures constitute a
 *	graph.  In order to send this graph to another node via IPC
 *	(that is, to "relocate" it), the graph must be marshalled into
 *	a flat byte array.  Socket graph descriptors (sgd's) are used
 *	to accomplish marshalling on the sending node, and also to guide
 *	unmarshalling on the receiving node.
 *
 *	An sgd is essentially a vector of <type,base,len> triples,
 *	which are called (amazingly enough) socket graph descriptor
 *	entries (sgde's).  An sgde describes a data structure in
 *	memory.  Since for our purposes the most common type of sgde
 *	corresponds to an mbuf, the sgde_type values are a superset of
 *	the mbuf types MT_xxx defined in <sys/mbuf.h>.
 *
 *	The first sgde in an sgd is always a header entry, and
 *	routines that create sgd's use this entry to store information
 *	about the type of the aggregate object represented by the sgd.
 *	For example, a pipe is one type of sgd; a FIFO is another.
 *	This sgd type is sometimes called the "arrival code", because
 *	when the sgd arrives at the new node it is this type information
 *	that is used to dispatch to use-specific unmarshalling code.
 *
 *	Often, two other vectors in addition to the sgd are needed to
 *	fully describe the data structure graph.  These are a vector of
 *	large mbuf data buffers and a vector of Mach port Rcv rts.  As
 *	the sgd is created (scanned), an sgde type value indicating a
 *	large mbuf or a data structure with an associated port Rcv rt
 *	causes the mbuf buffer or port name to be stored in (retrieved
 *	from) the appropriate vector.
 *
 *	A general rule in creating sgd's is that entries for aggregate
 *	data structures are followed by the entries for their parts.
 *	For example, a socket is followed by its two sockbufs; a sockbuf
 *	(really a marker indicating whether what follows belongs in the
 *	sending or receiving sockbuf) is followed by its mbufs.
 *
 *	Utility routines and macros are provided to append or extract
 *	common aggregate data structures such as sockets.
 *
 *	A lot (but not all) of the elaborateness of this mechanism is
 *	due to inadequacies in the scather/gather abilities of the
 *	underlying RPC interface language.
 */

#ifndef _TNC_SGD_H
#define _TNC_SGD_H

#if	MACH
#include <mach/message.h>
#else
#include <sys/uio.h>
#endif
#include <kern/macro_help.h>
#include <tnc/vector.h>

/*
 *  For sgd entries that correspond to mbufs, sgde_type and sgde_flags
 *  are simply copies of mh_type and mh_flags.  For other entries, a
 *  unique type is assigned starting at (MT_MAX+n), and the flag
 *  values defined below are used.
 */

struct sgd_header_entry {
	short		she_outlcnt;	/* # of large mbufs described */
	ulong_t		she_inlsize;	/* non-large-mbuf bytes described */
	ulong_t		she_arvlcode;	/* arrival code */
	int		she_len;	/* number of entries in sgd */
};
#define sgd_outlcnt	sgde_u.hdr.she_outlcnt
#define sgd_inlsize	sgde_u.hdr.she_inlsize
#define sgd_type	sgde_u.hdr.she_arvlcode
#define sgd_len		sgde_u.hdr.she_len

struct sgd_regular_entry {
	short		sre_flags;
	ulong_t		sre_xflags;
	caddr_t		sre_base;
	int		sre_len;
};
#define sgde_flags	sgde_u.entry.sre_flags
#define sgde_xflags	sgde_u.entry.sre_xflags
#define sgde_base	sgde_u.entry.sre_base
#define sgde_len	sgde_u.entry.sre_len

typedef struct sgd_entry {
	short		sgde_type;
	union {
		struct sgd_header_entry		hdr;
		struct sgd_regular_entry	entry;
	} sgde_u;
} sgde_t;

/*
 *  Generic vector manipulation for socket graph descriptor vector
 */
typedef vector_t	sgd_t;
#define SGD_ALLOC()	vec_alloc(0, sizeof(sgde_t))
#define SGD_DEALLOC(v)	vec_dealloc(v)
#define SGD_CAST(buf) \
	vec_cast((caddr_t)(buf), ((sgde_t *)buf)->sgd_len, sizeof(sgde_t))
#define SGD_RESET(v)	vec_reset(v)
#define SGD_NEXT(v,s)	((sgde_t *)vec_next((v),(s)))
#define SGD_HEADER(v)	((sgde_t *)VECTOR_FIRST(v))
#define SGD_GATHER(v,b)						\
MACRO_BEGIN							\
	(void) vec_gather((v),(b));				\
	ASSERT(((sgde_t *)(b))->sgd_len == VECTOR_LEN(v));	\
MACRO_END

#define SGD_GATHER_SIZE(v) ((VECTOR_GATHER_SIZE(v) + 3) & ~3)
#define SGD_LOOKAHEAD(v) ((sgde_t *)VECTOR_LOOKAHEAD(v))

/*
 *  SGD types, a.k.a. arrival codes
 *
 * SGD_TYPE_PIPE and SGD_TYPE_UN_ST both use the default
 * unix domain socket relocation routines.
 * #define SGD_TYPE_PIPE		1
 * #define SGD_TYPE_UN_ST		4
 */
#define SGD_TYPE_INVALID	0	/* don't use zero */
#define SGD_TYPE_UN_PAIR	1
#define SGD_TYPE_FIFO		2
#define SGD_TYPE_UN_DG		3
#define SGD_TYPE_NETWORK	4
#define SGD_TYPE_FIFO_REJOIN	5
#define SGD_TYPE_VSOCKCONN	6
#define SGD_TYPE_UN_DG_PKT	7

#define SGD_MAX_TYPE		7

/*
 *  Macro for performing the most common sgde initializations.
 */
/* #define SGDE_DEBUG	1	*/
#ifdef	SGDE_DEBUG
#define MAKE_SIMPLE_SGDE(sgdp,sgde,type,base,len)	\
MACRO_BEGIN						\
	sgde_t *hdr = SGD_HEADER(sgdp);			\
	(sgde)->sgde_type = (type);			\
	(sgde)->sgde_base = (caddr_t)(base);		\
	(sgde)->sgde_len  = (len);			\
	(sgde)->sgde_flags = 0;				\
	(sgde)->sgde_xflags = 0;			\
	hdr->sgd_len++;					\
	hdr->sgd_inlsize += (sgde)->sgde_len;		\
	printf("sgde 0x%x type %d base 0x%x len %d [hdr 0x%x, inl %d]\n", \
	       (sgde), (sgde)->sgde_type, (sgde)->sgde_base,		  \
	       (sgde)->sgde_len,					  \
	       SGD_HEADER(sgdp), SGD_HEADER(sgdp)->sgd_inlsize);	  \
MACRO_END
#else	/* ! SGDE_DEBUG */
#define MAKE_SIMPLE_SGDE(sgdp,sgde,type,base,len)	\
MACRO_BEGIN						\
	sgde_t *hdr = SGD_HEADER(sgdp);			\
	(sgde)->sgde_type = (type);			\
	(sgde)->sgde_base = (caddr_t)(base);		\
	(sgde)->sgde_len  = (len);			\
	(sgde)->sgde_flags = 0;				\
	(sgde)->sgde_xflags = 0;			\
	hdr->sgd_len++;					\
	hdr->sgd_inlsize += (sgde)->sgde_len;		\
MACRO_END
#endif	/* ! SGDE_DEBUG */
	
/*
 *  SGDE types
 */
/* 0 <= sgde_type <= MT_MAX indicates an mbuf */
#define SGDE_TYPE_SGDHDR	(MT_MAX+1)
#define SGDE_TYPE_SOCKET	(MT_MAX+2)
#define SGDE_TYPE_RCVSB		(MT_MAX+3)	/* precedes 1st rcvq mbuf */
#define SGDE_TYPE_SNDSB		(MT_MAX+4)	/* precedes 1st sndq mbuf */
#define SGDE_TYPE_UN_INFO	(MT_MAX+5)
#define SGDE_TYPE_UCRED		(MT_MAX+6)
#define SGDE_TYPE_VNODE		(MT_MAX+7)
#define SGDE_TYPE_FILE		(MT_MAX+8)
#define SGDE_TYPE_FIFO_INFO	(MT_MAX+9)
#define SGDE_TYPE_UNPCB		(MT_MAX+10)
#define SGDE_TYPE_SOADDR	(MT_MAX+11)	/* precedes sockaddr mbuf */
#define SGDE_TYPE_CTRLMBUFS	(MT_MAX+12)	/* precedes 1st control mbuf */

/*
 *  Some sgde types are just markers and do not have any real data
 *  associated with them.
 */
#define SGDE_HAS_DATA(t) (!((t) == SGDE_TYPE_RCVSB || \
			    (t) == SGDE_TYPE_SNDSB || \
			    (t) == SGDE_TYPE_SGDHDR || \
			    (t) == SGDE_TYPE_SOADDR || \
			    (t) == SGDE_TYPE_CTRLMBUFS))

/*
 *  Useful macro for extracting data out of a marshalled buffer (aka blob).
 */
#define SGDE_ADDR(x,blobp)		(&(blobp)[(int)((x)->sgde_base)])

/*
 *  Generic flag values (sgde_flags).
 *
 *  On the new node, if the SGDE_UNMARSHALLED bit is set then sgde_base
 *  is the address in memory of the unmarshalled item; otherwise sgde_base
 *  is an offset into the data buffer being unmarshalled.
 */
#define SGDE_UNMARSHALLED	0x01	/* flag bit rsvd, not yet used */
#define SGDE_LGMBUFS_INLINE	0x02	/* flag bit rsvd, not yet used */

/*
 *  Extended flag values (sgde_xflags).
 *  These are sgde_type -dependent.
 *
 *  - Sockets store domain/type/protocol info in the sgde.
 *  - Mbufs record whether they begin a new chain or not.
 *  - Files are only interesting for their mode bits.
 */
#define SGDE_SO_FAMILY_MASK	0xFF000000
#define SGDE_SO_TYPE_MASK	0x00FF0000
#define SGDE_SO_PROTO_MASK	0x0000FF00
#define SGDE_SO_FAMILY(x)	(((x)->sgde_xflags&SGDE_SO_FAMILY_MASK) >> 24)
#define SGDE_SO_TYPE(x)		(((x)->sgde_xflags&SGDE_SO_TYPE_MASK) >> 16)
#define SGDE_SO_PROTO(x)	(((x)->sgde_xflags&SGDE_SO_PROTO_MASK) >> 8)
#define SGDE_MARK_SOCKET_TYPE(sgde, fam, type, proto)			\
	(sgde)->sgde_xflags = (((fam)<<24)|((type)<<16)|((proto)<<8)	\
			       | ((sgde)->sgde_xflags & 0xFF))

#define SGDE_MB_CHAINHDR	0x01

/*
 *  Macros for the large mbuf data buffer vector.
 *
 *  For Mach, it's convenient to have vector entries appear as
 *  an IPC out-of-line region rather than an iovec.
 */

#if	MACH

typedef struct lg_mbuf_vector_entry {
	mach_msg_type_long_t	lmve_type;
	caddr_t			lmve_base;
} lmve_t;
#define iov_base	lmve_base
#define iov_len		lmve_type.msgtl_number

#else	/* ! MACH */

typedef struct iovec lmve_t;

#endif	/* ! MACH */

typedef vector_t	lmv_t;
#define LMV_ALLOC()	vec_alloc(0, sizeof(lmve_t))
#define LMV_CAST(buf,cnt) vec_cast((caddr_t)(buf), cnt, sizeof(lmve_t))
#define LMV_DEALLOC(v)	vec_dealloc(v)
#define LMV_RESET(v)	vec_reset(v)
#define LMV_FIRST(v)	((lmve_t *)VECTOR_FIRST(v))
#define LMV_NEXT(v,s)	((lmve_t *)vec_next((v),(s)))

/*
 *  Macros for the Rcv rt port name vector
 */
typedef vector_t	portv_t;
#define PV_ALLOC()	vec_alloc(RELOC_PORT_ARRAY_SIZE, sizeof(mach_port_t))
#define PV_CAST(buf,cnt) vec_cast((caddr_t)(buf), cnt, sizeof(mach_port_t))
#define PV_DEALLOC(v)	vec_dealloc(v)
#define PV_RESET(v)	vec_reset(v)
#define PV_NEXT(v,s)	((mach_port_t *)vec_next((v),(s)))
#define PV_GATHER(v,b)	vec_gather((v),(b))

/*
 *  Data structure and macros for ordered walking of mbuf lists
 */
typedef struct mbuf_list_iterator {
	struct mbuf **mli_head;
	struct mbuf *mli_chain;
	struct mbuf *mli_curr;
	int mli_flags;
} mblist_i_t;

#define MLIF_EOL	0x01	/* iterator at end of list */
#define MLIF_SAW_EOR	0x02	/* saw end of record (obsolete!) */
#define MLIF_INITED	0x04	/* iterator initialized */

extern void		mli_init(mblist_i_t *, struct mbuf **);
extern struct mbuf *	mli_next(mblist_i_t *, int *);
extern void		mli_put (mblist_i_t *, struct mbuf *, int);

#define MLI_INIT(mli, mpp)		mli_init(mli, mpp)
#define MLI_NEXT(mli, ischainp)		mli_next(mli, ischainp)
#define MLI_CURR(mli)			((mli)->mli_curr)
#define MLI_PUT(mli, mp, ischain)	mli_put(mli, mp, ischain)
#define MLI_DONE(mli)			((mli)->mli_flags &= ~MLIF_INITED)
#define MLI_READY(mli)			((mli)->mli_flags & MLIF_INITED)

/*
 *  Function prototypes for useful sgd routines
 */
extern void sgd_socket_append(
	sgd_t	*,
	lmv_t	*,
	struct socket *);

#endif	/* ! _TNC_SGD_H */
