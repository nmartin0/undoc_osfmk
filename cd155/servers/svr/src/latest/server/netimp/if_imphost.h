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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * Copyright (c) 1986 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: if_imphost.h,v $
 * Revision 1.5  1994/11/18  20:34:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:10:22  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:29:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:00:50  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:27:44  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:26:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:02  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:19  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:42:10  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:34:32  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:33:32  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:12:50  gm]
 * 
 * Revision 1.3  90/03/27  13:21:48  gm
 * 	Sync to osc.7.
 * 	[90/03/15  11:33:41  tmt]
 * 
 * 	Update to 4.4 alpha version.
 * 	[90/03/05  12:18:11  tmt]
 * 
 * Revision 1.2  90/01/02  20:10:59  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:41:39  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.3  89/03/09  20:40:28  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  18:55:21  gm0w
 * 	Updated copyright.
 * 	[89/02/12            gm0w]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base: if_imphost.h	7.6 (Berkeley) 6/29/88
 */

/*
 * Host structure used with IMP's.
 * Used to hold outgoing packets which
 * would exceed allowed RFNM count.
 *
 * These structures are packed into
 * mbuf's and kept as small as possible.
 */
struct host {
	struct	mbuf *h_q;		/* holding queue */
	u_short	h_timer;		/* used to stay off deletion */
	u_short	h_imp;			/* host's imp number */
	u_char	h_host;			/* host's number on imp */
	u_char	h_qcnt;          	/* size of holding q */
	u_char	h_rfnm;			/* # outstanding rfnm's */
	u_char	h_flags;		/* see below */
};

/*
 * A host structure is kept around (even when there are no
 * references to it) for a spell to avoid constant reallocation
 * and also to reflect IMP status back to sites which aren't
 * directly connected to the IMP.  When structures are marked
 * idle, a timer is started; when the timer expires the structure
 * is deallocated.  A structure may be reused before the timer expires.
 * A structure holds a reference on the containing mbuf when it is marked
 * HF_INUSE.
 */
#define	HF_INUSE	0x1
#define	HF_DEAD		(1<<IMPTYPE_HOSTDEAD)
#define	HF_UNREACH	(1<<IMPTYPE_HOSTUNREACH)

#define	HOSTTIMER	128		/* keep structure around awhile */

/*
 * Mark a host structure free; used if host entry returned by hostlookup 
 * isn't needed.  h_rfnm must be zero.
 */
#define	hostfree(hp) { \
	if ((hp)->h_timer == 0 && (hp)->h_qcnt == 0 && \
	    (hp)->h_flags & HF_INUSE) \
		hostrelease(hp); \
}

/*
 * Release a host entry when last rfnm is received.
 */
#define	hostidle(hp)	{ (hp)->h_timer = HOSTTIMER; }

/*
 * Host structures, as seen inside an mbuf.
 * Hashing on the host and imp is used to
 * select an index into the first mbuf.  Collisions
 * are then resolved by searching successive
 * mbuf's at the same index.  Reclamation is done
 * automatically at the time a structure is freed.
 */
#define	HPMBUF	((MLEN - sizeof(int)) / sizeof(struct host))
/* don't need to swab as long as HPMBUF is odd */
#if defined(notdef) && BYTE_ORDER == BIG_ENDIAN
#define	HOSTHASH(imp, host)	((unsigned)(ntohs(imp)+(host)) % HPMBUF)
#else
#define	HOSTHASH(imp, host)	((unsigned)((imp)+(host)) % HPMBUF)
#endif

/*
 * In-line expansions for queuing operations on
 * host message holding queue.  Queue is maintained
 * as circular list with the head pointing to the
 * last message in the queue.
 */
#define	HOST_ENQUE(hp, m) { \
	register struct mbuf *n; \
	(hp)->h_qcnt++; \
	if ((n = (hp)->h_q) == 0) \
		(hp)->h_q = (m)->m_act = (m); \
	else { \
		(m)->m_act = n->m_act; \
		(hp)->h_q = n->m_act = (m); \
	} \
}
#define	HOST_DEQUE(hp, m) { \
	if ((m) = (hp)->h_q) { \
		if ((m)->m_act == (m)) \
			(hp)->h_q = 0; \
		else { \
			(m) = (m)->m_act; \
			(hp)->h_q->m_act = (m)->m_act; \
		} \
		(hp)->h_qcnt--; \
		(m)->m_act = 0; \
	} \
}

struct hmbuf {
	int	hm_count;		/* # of struct's in use */
	struct	host hm_hosts[HPMBUF];	/* data structures proper */
};

#ifdef KERNEL
struct host *hostlookup();
struct host *hostenter();
#endif
