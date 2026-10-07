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
 * @OSF_COPYRIGHT@
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: mbuf.h,v $
 * Revision 1.7  1995/02/01  21:36:39  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:40:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/01  01:36:06  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.4  1993/07/14  18:25:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:41  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:19:29  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:42:41  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:15  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:11  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:41:51  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.5  93/08/19  13:19:42  mjl
 * [LCCbug #0355] The M_TNC_FORWARD() macro must clear M_WCARD before passing
 * mbuf chain to ip_forward(), otherwise ip_forward() will drop the packet.
 * 
 * Revision 2.4  93/05/25  10:37:56  mjl
 * For TNC, define an mbuf packet header bit that will force IP forwarding.
 * This is intended to allow packets received on an MI interface to be sent
 * out onto the wire---the hop across the multicomputer-internal link doesn't
 * really count as a network hop in this case.
 * 
 * Revision 2.3  93/04/14  14:26:53  durriya
 * 	define MGETX to allocate an external mbuf with caller-supplied data
 * 	[93/04/14            durriya]
 * 
 * Revision 2.2  91/08/31  14:08:19  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:43:44  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16  90/10/07  14:52:05  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:42:04  gm]
 * 
 * 	Include net_malloc.h for Unix build.
 * 	[90/09/29  19:51:23  tmt]
 * 
 * Revision 1.15  90/09/23  15:59:55  devrcs
 * 	Make NMBCLUSTERS configurable.
 * 	[90/09/15  15:40:47  tmt]
 * 
 * Revision 1.14  90/09/13  11:51:24  devrcs
 * 	Restore default MCLBYTES to CLBYTES.
 * 	[90/08/22  14:48:47  tmt]
 * 
 * Revision 1.13  90/08/24  12:26:30  devrcs
 * 	Set MCLBYTES to 1024 if not overridden in param.h. This is
 * 	a fast performance "fix" in lieu of algorithmic tuning in
 * 	TCP. It allows 4 packets to be sent into a 4K window, instead
 * 	of the 2 which is encouraged at full ethernet MTU.
 * 	[90/08/15  13:43:44  tmt]
 * 
 * Revision 1.12  90/07/27  09:07:19  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  15:47:50  tmt]
 * 
 * Revision 1.11  90/07/05  23:14:24  devrcs
 * 	Make mclrefcnt array of shorts. Upgrade MCLMAPPED macro.
 * 	[90/06/28  11:51:52  tmt]
 * 
 * Revision 1.10  90/06/22  20:53:23  devrcs
 * 	Modify mapped cluster macros and refcounting. Restore MCLGET to macro.
 * 	Call m_clalloc with count == 1 and let it decide the actual need.
 * 	[90/06/06  10:01:16  tmt]
 * 
 * Revision 1.9  90/05/13  20:33:46  devrcs
 * 	Check mclrefcnt in MCLFREE. Delete MT_ZOMBIE.
 * 	[90/04/30  14:03:33  tmt]
 * 
 * Revision 1.8  90/04/27  19:26:21  devrcs
 * 	Delete debug paranoia.
 * 	[90/04/20  12:07:04  tmt]
 * 
 * Revision 1.7  90/04/14  00:33:59  devrcs
 * 	Rearrange declarations for module unlinking. Add mfreelater.
 * 	[90/04/09  15:43:45  tmt]
 * 
 * Revision 1.6  90/03/27  13:27:36  gm
 * 	Change MCLBYTES compat to CLBYTES for now.
 * 	Rearrange macros slightly, move MFREE code to m_free.
 * 	Remove function declarations (clash with proto_uipc.h).
 * 	[90/03/19  16:34:35  tmt]
 * 
 * 	Enhance referencing of M_EXT data, allowing arbitrary clusters
 * 	to be reference copied. Fix leading/trailing space calculation
 * 	to avoid using referenced data.  Make MCLGET a subroutine to
 * 	permit allocating some clusters via standard means (kalloc).
 * 	Still gets interrupt and small mbuf clusters from mapped pool.
 * 	[90/03/09  12:35:09  tmt]
 * 
 * Revision 1.5  90/02/05  15:51:36  robert
 * 	Remove and/or rearrange debug paranoia.
 * 	Always call m_clalloc on M_CLALLOC.
 * 	[90/01/19  15:13:30  tmt]
 * 
 * Revision 1.4  90/01/18  08:48:53  gm
 * 	Enable LEADINGSPACE in M_EXT mbufs (clusters).
 * 	[90/01/09  20:04:43  tmt]
 * 
 * 	Make file re-entrant.
 * 	Pull in some header files and defines for compat.
 * 	Re-enable some mbuf types (mostly unused).
 * 	Slight mods to debug paranoia.
 * 	[90/01/08  15:42:15  tmt]
 * 
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/03  12:43:31  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:41:02  gm]
 * 
 * Revision 1.2  89/12/26  10:35:21  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	mbuf.h	7.12 (Berkeley) 9/4/89
 *	Merged: mbuf.h	7.13 (Berkeley) 6/28/90
 */

#ifndef	_SYS_MBUF_H_
#define	_SYS_MBUF_H_

#if	defined(_KERNEL) && !defined(_NET_GLOBALS_H_)
#include "net/net_globals.h"
#endif

#if	!defined(MACH) && defined(_KERNEL) && !defined(M_WAITOK)
#include "net/net_malloc.h"
#endif

#if	!defined(MCLBYTES)
/* Compat for now */
#define MSIZE		128
#define MCLBYTES	CLBYTES
#define MAPPED_MBUFS	/* safe */
#endif
#if	defined(_KERNEL) && !defined(NMBCLUSTERS)
#include <mbclusters.h>
#endif

/*
 * Mbufs are of a single size, MSIZE (machine/machparam.h), which
 * includes overhead.  An mbuf may add a single "mbuf cluster" of size
 * MCLBYTES (also in machine/machparam.h), which has no additional overhead
 * and is used instead of the internal data area; this is done when
 * at least MINCLSIZE of data must be stored.
 */

#define	MLEN		(MSIZE - sizeof(struct m_hdr))	/* normal data len */
#define	MHLEN		(MLEN - sizeof(struct pkthdr))	/* data len w/pkthdr */

#define	MINCLSIZE	(MHLEN + MLEN)	/* smallest amount to put in cluster */
#define	M_MAXCOMPRESS	(MHLEN / 2)	/* max amount to copy for compression */

#define NMBPCL		(sizeof(union mcluster) / sizeof(struct mbuf))

/*
 * Macros for type conversion
 * mtod(m,t) -	convert mbuf pointer to data pointer of correct type
 * dtom(x) -	convert data pointer within mbuf to mbuf pointer (XXX)
 * mtocl(x) -	convert pointer within cluster to cluster index #
 * cltom(x) -	convert cluster # to ptr to beginning of cluster
 */
#define mtod(m,t)	((t)((m)->m_data))
#define	dtom(x)		((struct mbuf *)((u_long)(x) & ~(MSIZE-1)))
#define	mtocl(x)	((union mcluster *)(x) - (union mcluster *)mbutl)
#define	cltom(x)	((union mcluster *)(mbutl + (x)))
#ifdef	DEBUG
#define MCLMAPPED(p)	(((unsigned)mtocl(p) < nmbclusters && \
			  mclrefcnt[mtocl(p)] >= 0) || panic(mbfail))
#define MCLREF(p)	(MCLMAPPED(p) && ++mclrefcnt[mtocl(p)])
#define MCLUNREF(p)	(MCLMAPPED(p) && --mclrefcnt[mtocl(p)] == 0)
#else
#define MCLMAPPED(p)	1
#define MCLREF(p)	(++mclrefcnt[mtocl(p)])
#define MCLUNREF(p)	(--mclrefcnt[mtocl(p)] == 0)
#endif

/* header at beginning of each mbuf: */
struct m_hdr {
	struct	mbuf *mh_next;		/* next buffer in chain */
	struct	mbuf *mh_nextpkt;	/* next chain in queue/record */
	long	mh_len;			/* amount of data in this mbuf */
	caddr_t	mh_data;		/* location of data */
	short	mh_type;		/* type of data in this mbuf */
	short	mh_flags;		/* flags; see below */
};

/* record/packet header in first mbuf of chain; valid if M_PKTHDR set */
struct	pkthdr {
	long	len;		/* total packet length */
	struct	ifnet *rcvif;	/* rcv interface */
};

/* description of external storage mapped into mbuf, valid if M_EXT set */
struct m_ext {
	caddr_t	ext_buf;		/* start of buffer */
	void	(*ext_free)();		/* free routine if not the usual */
	u_long	ext_size;		/* size of buffer, for ext_free */
	caddr_t	ext_arg;		/* additional ext_free argument */
	struct	ext_refq {		/* reference list */
		struct ext_refq *forw, *back;
	} ext_ref;
};

struct mbuf {
	struct	m_hdr m_hdr;
	union {
		struct {
			struct	pkthdr MH_pkthdr;	/* M_PKTHDR set */
			union {
				struct	m_ext MH_ext;	/* M_EXT set */
				char	MH_databuf[MHLEN];
			} MH_dat;
		} MH;
		char	M_databuf[MLEN];		/* !M_PKTHDR, !M_EXT */
	} M_dat;
};
#define	m_next		m_hdr.mh_next
#define	m_len		m_hdr.mh_len
#define	m_data		m_hdr.mh_data
#define	m_type		m_hdr.mh_type
#define	m_flags		m_hdr.mh_flags
#define	m_nextpkt	m_hdr.mh_nextpkt
#define	m_act		m_nextpkt
#define	m_pkthdr	M_dat.MH.MH_pkthdr
#define	m_ext		M_dat.MH.MH_dat.MH_ext
#define	m_pktdat	M_dat.MH.MH_dat.MH_databuf
#define	m_dat		M_dat.M_databuf

/* mbuf flags */
#define	M_EXT		0x0001	/* has associated external storage */
#define	M_PKTHDR	0x0002	/* start of record */
#define	M_EOR		0x0004	/* end of record */

/* mbuf pkthdr flags, also in m_flags */
#define	M_BCAST		0x0100	/* send/received as link-level broadcast */
#define	M_MCAST		0x0200	/* send/received as link-level multicast */
#define	M_WCARD		0x0400	/* received as network-level broadcast */

#ifdef	TNC
#define M_TNC_FORW	0x0800	/* force forwarding of MI packets */
#define M_TNC_FORWARD(m)	(((m)->m_flags &= ~M_WCARD), \
				 ((m)->m_flags & M_TNC_FORW))
#else
#define M_TNC_FORW	0x0000	/* no-op when OR'ed w/ other M_COPYFLAGS */
#define M_TNC_FORWARD(m)	(FALSE)
#endif

/* does mbuf hold a broadcast packet? */
#define m_broadcast(m)	((m)->m_flags & (M_BCAST|M_MCAST|M_WCARD))

/* flags copied when copying m_pkthdr */
#define	M_COPYFLAGS	(M_PKTHDR|M_EOR|M_BCAST|M_MCAST|M_WCARD|M_TNC_FORW)

/* mbuf types */
#define	MT_FREE		0	/* should be on free list */
#define	MT_DATA		1	/* dynamic (data) allocation */
#define	MT_HEADER	2	/* packet header */
#define	MT_SOCKET	3	/* socket structure */
#define	MT_PCB		4	/* protocol control block */
#define	MT_RTABLE	5	/* routing tables */
#define	MT_HTABLE	6	/* IMP host tables */
#define	MT_ATABLE	7	/* address resolution tables */
#define	MT_SONAME	8	/* socket name */
#define	MT_SOOPTS	10	/* socket options */
#define	MT_FTABLE	11	/* fragment reassembly header */
#define	MT_RIGHTS	12	/* access rights */
#define	MT_IFADDR	13	/* interface address */
#define MT_CONTROL	14	/* extra-data protocol message */
#define MT_OOBDATA	15	/* expedited data  */
#define MT_MAX		32	/* e.g. */

/* flags to m_get/MGET */
#define	M_DONTWAIT	0
#define	M_WAIT		1

/*
 * mbuf allocation/deallocation macros:
 *
 *	MGET(struct mbuf *m, int how, int type)
 * allocates an mbuf and initializes it to contain internal data.
 *
 *	MGETHDR(struct mbuf *m, int how, int type)
 * allocates an mbuf and initializes it to contain a packet header
 * and internal data.
 */
#define _MBGET(m, type) { \
	int _s = splimp(); \
	MBUF_LOCK(); \
	if ((m) = mfree) { \
		(void)MCLREF(m); \
		mbstat.m_mfree--; \
		mbstat.m_mtypes[type]++; \
		mfree = (m)->m_next; \
	} \
	MBUF_UNLOCK(); \
	splx(_s); \
}

#define	MGET(m, how, type) { \
	_MBGET(m, type); \
	if (m) { \
		(m)->m_next = (m)->m_nextpkt = 0; \
		(m)->m_type = (type); \
		(m)->m_data = (m)->m_dat; \
		(m)->m_flags = 0; \
	} else \
		(m) = m_retry((how), (type)); \
}

#define	MGETHDR(m, how, type) { \
	_MBGET(m, type); \
	if (m) { \
		(m)->m_next = (m)->m_nextpkt = 0; \
		(m)->m_type = (type); \
		(m)->m_data = (m)->m_pktdat; \
		(m)->m_flags = M_PKTHDR; \
	} else \
		(m) = m_retryhdr((how), (type)); \
}

/*
 * Mbuf cluster macros.
 * MCLALLOC(caddr_t p, int how) allocates an mbuf cluster.
 * MCLGET adds such clusters to a normal mbuf;
 * the flag M_EXT is set upon success.
 * MCLFREE releases a reference to a cluster allocated by MCLALLOC,
 * freeing the cluster if the reference count has reached 0.
 *
 * Normal mbuf clusters are normally treated as character arrays
 * after allocation, but use the first word of the buffer as a free list
 * pointer while on the free list.
 */
union mcluster {
	union	mcluster *mcl_next;
	char	mcl_buf[MCLBYTES];
};

#define	MCLALLOC(p, how) \
	{ int ms = splimp(); \
	  MBUF_LOCK(); \
	  (void)m_clalloc(1, (how)); \
	  if ((p) = (caddr_t)mclfree) { \
		(void)MCLREF(p); \
		mbstat.m_clfree--; \
		mclfree = ((union mcluster *)(p))->mcl_next; \
	  } \
	  MBUF_UNLOCK(); \
	  splx(ms); \
	}

#define	MCLGET(m, how) \
	{ MCLALLOC((m)->m_ext.ext_buf, (how)); \
	  if ((m)->m_ext.ext_buf) { \
		(m)->m_data = (m)->m_ext.ext_buf; \
		(m)->m_flags |= M_EXT; \
		(m)->m_ext.ext_size = MCLBYTES; \
		(m)->m_ext.ext_free = 0; \
		(m)->m_ext.ext_arg  = 0; \
		(m)->m_ext.ext_ref.forw = (m)->m_ext.ext_ref.back = \
			&(m)->m_ext.ext_ref; \
	  } \
	}

#define	MCLFREE(p) \
	{ int ms = splimp(); \
	  MBUF_LOCK(); \
	  if (MCLUNREF(p)) { \
		  ((union mcluster *)(p))->mcl_next = mclfree; \
		  mclfree = (union mcluster *)(p); \
		  mbstat.m_clfree++; \
	  } \
	  MBUF_UNLOCK(); \
	  splx(ms); \
	}

#define MCLREFERENCED(m) \
	((m)->m_ext.ext_ref.forw != &((m)->m_ext.ext_ref))

#ifdef OSF1_ADFS
/*
 * MGETX(struct mbuf *m, void f(), (caddr_t)a, (caddr_t)d, (size_t)s, int how)
 * allocates an mbuf with external data - no packet hdr is allocated for this 
 * mbuf
 * Args : m - pointer to allocated mbuf
 *        f - pointer to the free func that should be called when free'ing 'm'
 *        a - first argument to be passed to 'f'
 *        d - external data area provided by caller
 *        s - size of data area 'd'
 *        how - M_WAIT or M_DONTWAIT
 */
#define MGETX(m, f, a, d, s, how) {\
	MGET(m, how, MT_DATA); \
	if (m) { \
		(m)->m_data = d; \
		(m)->m_flags |= M_EXT; \
		(m)->m_len = s; \
		(m)->m_ext.ext_buf  = a; \
		(m)->m_ext.ext_size = s; \
		(m)->m_ext.ext_free = f; \
		(m)->m_ext.ext_arg = 0; \
		(m)->m_ext.ext_ref.forw = (m)->m_ext.ext_ref.back = \
			&(m)->m_ext.ext_ref; \
	  } \
	}
#endif /* OSF1_ADFS */

/*
 * MFREE(struct mbuf *m, struct mbuf *n)
 * Free a single mbuf and associated external storage.
 * Place the successor, if any, in n.
 */
#define	MFREE(m, n) (n) = m_free(m)

/*
 * Copy mbuf pkthdr from from to to.
 * from must have M_PKTHDR set, and to must be empty.
 */
#define	M_COPY_PKTHDR(to, from) { \
	(to)->m_pkthdr = (from)->m_pkthdr; \
	(to)->m_flags = (from)->m_flags & M_COPYFLAGS; \
	(to)->m_data = (to)->m_pktdat; \
}

/*
 * Set the m_data pointer of a newly-allocated mbuf (m_get/MGET) to place
 * an object of the specified size at the end of the mbuf, longword aligned.
 */
#define	M_ALIGN(m, len) \
	{ (m)->m_data += (MLEN - (len)) &~ (sizeof(long) - 1); }
/*
 * As above, for mbufs allocated with m_gethdr/MGETHDR
 * or initialized by M_COPY_PKTHDR.
 */
#define	MH_ALIGN(m, len) \
	{ (m)->m_data += (MHLEN - (len)) &~ (sizeof(long) - 1); }

/*
 * Compute the amount of space available
 * before the current start of data in an mbuf.
 * Subroutine - data not available if certain references.
 */
#define	M_LEADINGSPACE(m)	m_leadingspace(m)

/*
 * Compute the amount of space available
 * after the end of data in an mbuf.
 * Subroutine - data not available if certain references.
 */
#define	M_TRAILINGSPACE(m)	m_trailingspace(m)

/*
 * Arrange to prepend space of size plen to mbuf m.
 * If a new mbuf must be allocated, how specifies whether to wait.
 * If how is M_DONTWAIT and allocation fails, the original mbuf chain
 * is freed and m is set to NULL.
 */
#define	M_PREPEND(m, plen, how) { \
	if (M_LEADINGSPACE(m) >= (plen)) { \
		(m)->m_data -= (plen); \
		(m)->m_len += (plen); \
	} else \
		(m) = m_prepend((m), (plen), (how)); \
	if ((m) && (m)->m_flags & M_PKTHDR) \
		(m)->m_pkthdr.len += (plen); \
}

/* change mbuf to new type */
#define MCHTYPE(m, t) { \
	MBSTAT((mbstat.m_mtypes[(m)->m_type]--, mbstat.m_mtypes[t]++)); \
	(m)->m_type = t;\
}

/* length to m_copy to copy all */
#define	M_COPYALL	1000000000

/* compatiblity with 4.3 */
#define  m_copy(m, o, l)	m_copym((m), (o), (l), M_DONTWAIT)

/*
 * Mbuf statistics.
 */
struct mbstat {
	u_long	m_mbufs;	/* mbufs obtained from page pool */
	u_long	m_clusters;	/* clusters obtained from page pool */
	u_long	m_mfree;	/* free mbufs */
	u_long	m_clfree;	/* free clusters */
	u_long	m_drops;	/* times failed to find space */
	u_long	m_wait;		/* times waited for space */
	u_long	m_drain;	/* times drained protocols for space */
	u_short	m_mtypes[MT_MAX];	/* type specific mbuf allocations */
};

#ifdef	_KERNEL
#if	NETSYNC_LOCK
extern	simple_lock_data_t mbuf_slock;
#define MBUF_LOCKINIT()	simple_lock_init(&mbuf_slock)
#define MBUF_LOCK()	simple_lock(&mbuf_slock)
#define MBUF_UNLOCK()	simple_unlock(&mbuf_slock)
#define MBSTAT(x)	{int _ns = splimp(); MBUF_LOCK(); \
			  x; MBUF_UNLOCK(); splx(_ns); }
#else
#define MBUF_LOCKINIT()
#define MBUF_LOCK()
#define MBUF_UNLOCK()
#define MBSTAT(x)	x
#endif
#if	!MACH
extern	char mbutl[][MCLBYTES];		/* virtual address of mclusters */
#else
extern	union mcluster *mbutl;		/* ditto */
#endif
extern struct	mbuf *mfree;		/* mbuf free list */
extern struct	mbstat mbstat;		/* statistics */
extern int	nmbclusters;		/* number of mapped clusters */
extern union	mcluster *mclfree;	/* free mapped cluster list */
extern short	*mclrefcnt;		/* mapped cluster reference counts */
extern int	max_linkhdr;		/* largest link-level header */
extern int	max_protohdr;		/* largest protocol header */
extern int	max_hdr;		/* largest link+protocol header */
extern int	max_datalen;		/* MHLEN - max_hdr */
extern char	mbfail[];		/* "mbuf not mapped" (DEBUG) */
#endif
#endif
