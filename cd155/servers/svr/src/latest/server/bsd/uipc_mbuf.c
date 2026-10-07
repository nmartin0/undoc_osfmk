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
 * HISTORY
 * $Log: uipc_mbuf.c,v $
 * Revision 1.8  1995/03/18  01:27:06  hobbes
 *  Reviewer: Bernie Keany (fix submitted by Mike Wan @ SDSC)
 *  Risk: Low
 *  Benefit or PTS #: 12598 -- System hung caused bt a PVM application using
 * 			    up all the mbuf
 *  Testing: TCP/IP EATS
 *  Module(s): uipc_mbuf.c
 *
 * Revision 1.7  1994/11/18  20:28:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  17:50:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:51:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:07:33  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:48  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:59:15  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:55:09  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:17:33  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:07:44  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.8  92/11/03  12:21:51  loverso
 * 	Fix typo.
 * 
 * Revision 2.7  92/11/03  11:19:00  loverso
 * 	Upgraded to OSF/1 MK 4.1.
 * Revision 2.8  1992/11/03  12:21:51  loverso
 * 	Fix typo.
 *
 * Revision 2.7  92/11/03  11:19:00  loverso
 * 	Upgraded to OSF/1 MK 4.1.
 * Revision 1.3  1992/10/21  12:53:21  devrcs
 * 	Fix for bug #186: sealed a virtual memory leak by fixing the mbuf allocation
 * 	strategy (m_howmany()).
 * 	[1992/09/23  12:09:56  barbou]
 *
 * Revision 1.2  1992/05/12  13:12:30  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/04  00:14:11  condict]
 * 
 * 	Place spl drop in m_clalloc under NO_INTERRUPT_ALLOCATION.
 * 	Wakeup m_free sleepers after unlock. Cast args to allocbi().
 * 	Add forward reference for m_howmany().
 * 	[91/03/13  18:59:10  tmt]
 * 
 * 	Update pkthdr in correct mbuf in m_adj().
 * 	Save/restore spl more faithfully.
 * 	[91/02/16  15:47:15  tmt]
 * 
 * Revision 3.6  92/01/07  23:33:19  condict
 * 	Keep NO_INTERRUPT_ALLOCATION turned on, although NETISR_THREADS is off.
 * 
 * Revision 3.5  91/12/18  17:15:40  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 3.4  91/10/02  10:51:17  condict
 * 	Remove panic that occurred during mbuf garbage collection (was unnecessary).
 * 	Also, improve code re-use by defining kmem_mb_alloc kmem_mb_init.
 * 
 * Revision 3.3  91/08/12  15:35:53  sp
 * 	Fix bug checking size of mclrefcnt which is dynamically allocated in osf/1
 * 	but a static array for BSD.
 * 
 * Revision 3.2  91/07/31  15:31:42  sp
 * 	Upgrade to 1.0.2
 * 
 * Revision 1.19  90/10/07  13:20:34  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:03:10  gm]
 * 
 * 	Remove spurious include of sys/dir.h under Unix.
 * 	[90/09/29  18:05:17  tmt]
 * 
 * Revision 1.18  90/09/23  15:43:46  devrcs
 * 	Fix typo preventing isr thread from allocating new mbufs.
 * 	[90/09/16  10:54:03  tmt]
 * 
 * 	Add/fix support for non-thread context behavior.
 * 	[90/09/12  20:39:12  tmt]
 * 
 * 	Use tsleep in m_retry. Add "hz" timeout as insurance.
 * 	[90/09/05  17:24:37  tmt]
 * 
 * Revision 1.17  90/08/24  11:19:30  devrcs
 * 	Add NO_INTERRUPT_ALLOCATION option, and use it to decide
 * 	whether to use kmem_alloc and enable garbage collection.
 * 	[90/08/17  18:11:05  tmt]
 * 
 * Revision 1.16  90/07/27  08:44:56  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  15:34:36  tmt]
 * 
 * Revision 1.15  90/07/05  23:08:10  devrcs
 * 	Add mbuf garbage collector (called in slowtimeout). Rearrange
 * 	code to fit. Make mclrefcnt an array of shorts, init to -1.
 * 	[90/06/28  12:05:54  tmt]
 * 
 * Revision 1.14  90/06/29  13:35:08  devrcs
 * 	Change include and MBLKP types for new streams header files.
 * 	[90/06/26  11:46:32  tmt]
 * 
 * Revision 1.13  90/06/22  20:07:42  devrcs
 * 	Clear M_PKTHDR when allocate new mbuf in pullup.
 * 	[90/06/11  11:32:51  tmt]
 * 
 * 	Back out the use of kalloc for mbuf allocation. There is simply
 * 	too much contention on the kernel map for a win. Will use instead
 * 	a new "kmem_mb_free" (TBD) for deallocation.
 * 	[90/06/06  10:02:47  tmt]
 * 
 * Revision 1.12  90/05/24  23:00:29  devrcs
 * 	Updated #include/#define for STREAMS context.
 * 	[90/05/18  12:54:40  nolting]
 * 
 * Revision 1.11  90/05/13  18:34:37  devrcs
 * 	Don't free incoming mbuf/mblk chain if can't allocate wrapper.
 * 	Check mclrefcnt and m_unexpand more closely.
 * 	[90/05/04  15:34:23  tmt]
 * 
 * Revision 1.10  90/04/27  18:53:29  devrcs
 * 	Remove debug paranoia. Add streams/sockets mblk/mbuf copying.
 * 	[90/04/20  12:13:00  tmt]
 * 
 * Revision 1.9  90/04/14  00:30:22  devrcs
 * 	Rearrange code from header files and internally.
 * 	Do netisr_add() for dynamic netisr installation. Mbuf interrupt
 * 	grows mapped cluster freelist, also frees unmapped clusters.
 * 	Doesn't log message on full map (not very meaningful).
 * 	[90/04/10  14:52:12  tmt]
 * 
 * Revision 1.8  90/03/27  13:15:14  gm
 * 	Rearrange MFREE: always do in m_free (macro too long for
 * 	benefit), do m_unexpand there as well. Simplify code.
 * 	Lock m_ext refcnt around free (but not at copy, may
 * 	become dereferenced, but not vice versa, arbitrarily).
 * 	Strengthen type declarations (void).
 * 	[90/03/19  16:33:00  tmt]
 * 
 * 	MCLALLOC in mclget may use M_WAIT/M_DONTWAIT of caller.
 * 	[90/03/09  13:30:31  tmt]
 * 
 * 	Enhance referencing of M_EXT data, allowing arbitrary clusters
 * 	to be reference copied. Fix leading/trailing space calculation
 * 	to avoid using referenced data.  Make MCLGET a subroutine to
 * 	permit allocating some clusters via standard means (kalloc).
 * 	Still gets interrupt and small mbuf clusters from mapped pool.
 * 	[90/03/09  12:37:01  tmt]
 * 
 * Revision 1.7  90/02/16  16:49:22  devrcs
 * 	Don't allocate mclrefcnt array if already there.
 * 	[90/02/12  12:01:41  collins]
 * 
 * Revision 1.6  90/02/05  15:47:50  robert
 * 	Clean up glitches and problems in wait/drain code for buffers.
 * 	Enable drain on m_clalloc's. Change logged msg on full.
 * 	[90/01/25  14:42:41  tmt]
 * 
 * 	Enhance m_clalloc for multiprocessor allocations. Do lowater stuff here.
 * 	Always allocate mclrefcnt array.
 * 	[90/01/19  14:08:37  tmt]
 * 
 * Revision 1.5  90/01/18  08:42:24  gm
 * 	Fix m_unexpand. Leave m_dont_unexpand set for now.
 * 	Release lock while allocating clusters (and logging if full).
 * 	[90/01/08  15:40:56  tmt]
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
 * Revision 1.4  90/01/03  11:52:14  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:28:02  gm]
 * 
 * Revision 1.3  89/12/26  09:23:43  gm
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
 *	Base:	uipc_mbuf.c	7.12 (Berkeley) 9/26/89
 *	Merged: uipc_mbuf.c	7.16 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/time.h"
#include "sys/kernel.h"

#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#include "sys/mbuf.h"
#include "sys/protosw.h"

#include "net/net_malloc.h"
#include "net/netisr.h"

struct	mbuf *mfree;		/* mbuf free list */
struct	mbuf *mfreelater;	/* mbuf deallocation list */
int	m_want;			/* sleepers on mbufs */
int	nmbclusters;		/* max number of mapped clusters */
union	mcluster *mclfree;	/* mapped cluster free list */
short	*mclrefcnt;		/* mapped cluster reference counts */
static	int nclpp;		/* # clusters per physical page */
int	max_linkhdr;		/* largest link-level header */
int	max_protohdr;		/* largest protocol header */
int	max_hdr;		/* largest link+protocol header */
int	max_datalen;		/* MHLEN - max_hdr */
struct	mbstat mbstat;		/* statistics */
int	mclbytes = MCLBYTES;	/* possibly interesting value */
char	mbfail[] = "mbuf not mapped";
#if	MACH
struct	netmallocstats netmallocstats[M_LAST];
union	mcluster *mbutl;	/* first mapped cluster address */
#endif
#if	NETSYNC_LOCK
simple_lock_data_t	mbuf_slock;
LOCK_ASSERTL_DECL
#endif

static int m_howmany();

#ifdef	OSF1_SERVER
#define NO_INTERRUPT_ALLOCATION	1
#else	OSF1_SERVER
#define NO_INTERRUPT_ALLOCATION	(NETISR_THREAD /* && not testing */)
#endif	OSF1_SERVER

#if	NMBCLUSTERS <= 0
#define NMBCLUSTERS	1024
#endif

#if	MACH

#include <uxkern/import_mach.h>

vm_offset_t	next_mbuf_addr;
vm_offset_t	end_mbuf_addr;

/*
 * Allocate all possible mbuf memory now, since it is structured
 * as an array parallel to mclrefcnt.  Suballocate it later.
 */
kmem_mb_init() {
	register vm_offset_t size;

	size = nmbclusters * CLBYTES;
	(void) vm_allocate(mach_task_self(), &next_mbuf_addr, size, TRUE);
	end_mbuf_addr = next_mbuf_addr + size;

	mbutl = (union mcluster *)next_mbuf_addr;
}

void
mbinit()
{
	int s, m;

	if (nclpp)
		return;
	s = splimp();
	nclpp = round_page(MCLBYTES) / MCLBYTES;	/* see mbufgc() */
	if (nclpp < 1) nclpp = 1;
	MBUF_LOCKINIT();
	/* Initialize isr stuff - here because of netisr_add and
	 * because some systems need mbufs before netinit. */
	NETISR_LOCKINIT();
	bzero((caddr_t)softnet_intr, sizeof softnet_intr);

	netisr_add(NETISR_MB, mbufintr,
			(struct ifqueue *)NULL, (struct domain *)NULL);
	if (nmbclusters == 0)
		nmbclusters = NMBCLUSTERS;
	NET_MALLOC(mclrefcnt, short *, nmbclusters * sizeof (short),
					M_TEMP, M_WAITOK);
	if (mclrefcnt == 0)
		panic("mbinit");
	for (m = 0; m < nmbclusters; m++)
		mclrefcnt[m] = -1;
	MBUF_LOCK();
	kmem_mb_init();
	m = m_clalloc(NETNCPUS, M_WAIT);
	MBUF_UNLOCK();
	splx(s);
	if (m == 0 || !m_expand(M_DONTWAIT))
		panic("mbinit");
#if	!NETISR_THREAD
	kfree(kalloc(64),  64);
	kfree(kalloc(128), 128);
	kfree(kalloc(256), 256);
#endif
}

/*
 * Cluster allocation under Mach is from the mb_map to avoid contention
 * on the kernel_map, and to take advantage of the optimization on a
 * uniprocessor to get pages from interrupt level. On multiprocessors
 * it is not possible to allocate with interrupts raised due to
 * pmap contention during TLB shootdown. However, even on uni's, it
 * is desirable to use the non-interrupt allocation, as it simplifies
 * matters greatly and allows the pages to be freed.
 *
 * m_clalloc must be called at splimp().
#if	NETSYNC_LOCK
 * AND with MBUF_LOCK() held.
#endif
 */

/* ARGSUSED */
m_clalloc(ncl, canwait)
	register int ncl;
{
	register union mcluster *mcl;
	register int i;
	vm_size_t size;
	static char doing_alloc;
#if	NO_INTERRUPT_ALLOCATION
	int s;
#endif

	if (doing_alloc || (i = m_howmany()) <= 0)
		goto out;
#if	NO_INTERRUPT_ALLOCATION
	if (canwait == M_DONTWAIT) {
		extern boolean_t netisr_is_init;
		ASSERT(netisr_is_init);
		schednetisr(NETISR_MB);		/* go get more later */
		goto out;
	}
#define kmem_mb_alloc(ignore, size) (					\
	  (next_mbuf_addr + size > end_mbuf_addr) ?			\
		0							\
	: (mtocl((union mcluster *)next_mbuf_addr) > nmbclusters) ?	\
		(panic("m_clalloc"), (union mcluster *)0)		\
	:								\
		(next_mbuf_addr += size,				\
		 (union mcluster *)(next_mbuf_addr - size)		\
		)							\
)
#endif
	doing_alloc = 1;
	MBUF_UNLOCK();
#if	NO_INTERRUPT_ALLOCATION
	s = splhigh();	/* Silly, but spl0 is void */
	spl0();
#endif

	if (ncl < i) ncl = i;
    again:
	size = round_page(ncl * MCLBYTES);
	mcl = (union mcluster *)kmem_mb_alloc(mb_map, size);
	if (mcl == 0 && ncl > 1) {
		ncl = 1;
		goto again;
	}
#if	NO_INTERRUPT_ALLOCATION
	/* When cluster allocation fails, ask protocols to free */
	if (mcl == 0)
		pfreclaim();
	splx(s);	/* Back to splimp */
#endif
	MBUF_LOCK();
	doing_alloc = 0;
	if (mcl) {
		ncl = size / MCLBYTES;
		for (i = 0; i < ncl; i++) {
			if (++mclrefcnt[mtocl(mcl)] != 0)
				panic("m_clalloc already there");
			mcl->mcl_next = mclfree;
			mclfree = mcl++;
		}
		mbstat.m_clfree += ncl;
		mbstat.m_clusters += ncl;
		return (ncl);
	} /* else ... */
#if	NO_INTERRUPT_ALLOCATION
	mbstat.m_drain++;
#endif
out:
	if (mclfree)
		return 1;
	mbstat.m_drops++;
	return 0;
}

#else	/* UNIX */

#if MCLBYTES < 4096
#define NCL_INIT	(4096/MCLBYTES)
#else
#define NCL_INIT	2
#endif

#include "sys/syslog.h"
#include "sys/time.h"
#include "sys/kernel.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/cmap.h"
#include "sys/map.h"
#include "machine/pte.h"

void
mbinit()
{
	int s, m;

	if (nclpp)
		return;
	s = splimp();
	nclpp = CLSIZE / MCLBYTES;
	if (nclpp < 1) nclpp = 1;
	MBUF_LOCKINIT();
	/* Initialize isr stuff - here because of netisr_add and
	 * because some systems do mbufs before netinit. */
	NETISR_LOCKINIT();
	bzero((caddr_t)softnet_intr, sizeof softnet_intr);

	netisr_add(NETISR_MB, mbufintr,
			(struct ifqueue *)NULL, (struct domain *)NULL);
	if (nmbclusters == 0)
		nmbclusters = NMBCLUSTERS;
	NET_MALLOC(mclrefcnt, short *,
		(nmbclusters + CLBYTES/MCLBYTES) * sizeof (short),
		M_TEMP, M_NOWAIT);
	if (mclrefcnt) {
		for (m = 0; m < nmbclusters + CLBYTES/MCLBYTES; m++)
			mclrefcnt[m] = -1;
		mbstat.m_clusters = 1;	/* first page not used */
		MBUF_LOCK();
		m = m_clalloc(NCL_INIT, M_DONTWAIT);
		MBUF_UNLOCK();
		splx(s);
		if (m && m_expand(M_DONTWAIT))
			return;
	}
	panic("mbinit");
}

/*
 * Allocate some number of mbuf clusters
 * and place on cluster free list.
 * Must be called at splimp.
 */
/* ARGSUSED */
m_clalloc(ncl, canwait)
	register int ncl;
{
	int npg, mbx;
	register union mcluster *p;
	register int i;
	static char logged;
	extern struct pte Mbmap[];		/* page tables to map mbutl */

	if (mclfree)
		return 1;
	npg = ncl * CLSIZE;
	mbx = rmalloc(mbmap, (long)npg);
	if (mbx == 0) {
		if (logged == 0) {
			logged++;
			log(LOG_ERR, "mbuf map full\n");
		}
		mbstat.m_drops++;
		return (0);
	}
	p = cltom((mbx * NBPG) / MCLBYTES);
	if (memall(&Mbmap[mbx], npg, proc, CSYS) == 0) {
		rmfree(mbmap, (long)npg, (long)mbx);
		mbstat.m_drops++;
		return (0);
	}
	vmaccess(&Mbmap[mbx], (caddr_t)p, npg);
	ncl = (ncl * CLBYTES) / MCLBYTES;
	for (i = 0; i < ncl; i++) {
		mclrefcnt[mtocl(p)] = 0;
		p->mcl_next = mclfree;
		mclfree = p++;
	}
	mbstat.m_clfree += ncl;
	mbstat.m_clusters += ncl;
	return (ncl);
}

#endif	/* !MACH */

/*
 * The mbuf "isr". Check cluster allocation and free any deferred
 * m_ext mbufs. Both done here to avoid calls from interrupt level.
 */
void
mbufintr()
{
	register struct mbuf *m;
	int s = splimp();

	MBUF_LOCK();
#if	NO_INTERRUPT_ALLOCATION
	(void) m_clalloc(1, M_WAIT);	/* m_howmany() will decide # */
#endif
	m = mfreelater;
	mfreelater = NULL;
	MBUF_UNLOCK();
	splx(s);
	while (m) {
		if (m->m_flags & M_EXT) {
			if (MCLREFERENCED(m) || !m->m_ext.ext_free)
				panic("mfreelater");
			(*(m->m_ext.ext_free))(m->m_ext.ext_buf,
			    m->m_ext.ext_size, m->m_ext.ext_arg);
			m->m_flags &= ~M_EXT;
		}
		m = m_free(m);
	}
}

/*
 * Garbage collect mbufs, coalescing and freeing where possible.
 * First try to get regular mbufs back into clusters, then try
 * to put back unneeded cluster pages. Called every slowtimeout.
 *
 * The strategy is to always coalesce normal mbufs and then to
 * try to coalesce clusters back to pages and free. If the latter
 * fails, the clusters belonging to the incomplete page are linked
 * onto the tail of the freelist and the cycle is broken. If it
 * succeeds we free then break the loop. The idea is to stir the
 * freelist regularly and free gradually as things rise to the top.
 *
 * Note for MCLBYTES >= PAGE_SIZE the coalesce is trivial and the
 * free always happens, which could be considered an optimization.
 * Finally, this calculation is not necessarily constant under MACH(!).
 */
int mbufgcintvl = 5 * PR_SLOWHZ;
#if	NO_INTERRUPT_ALLOCATION
int mbufgcenable = 1;
#endif
void
mbufgc()
{
	register int i, cl;
	static int count;
	int s;

	if (++count != mbufgcintvl)
		return;
	count = 0;
	s = splimp();
	MBUF_LOCK();
	/* Coalesce regular mbufs */
	while (!m_want && mbstat.m_mfree > NMBPCL &&
	       mbstat.m_mfree > (mbstat.m_mbufs >> 2)) {
		register struct mbuf *m = mfree, **mp = &mfree;
		while (m) {		/* Find an unreferenced cluster */
			cl = mtocl(m);
			if (mclrefcnt[cl] <= 1)
				break;
			mp = &m->m_next;
			m = m->m_next;
		}
		if (m == NULL)
			break;
		i = 1;
		if (mclrefcnt[cl] != 1)
			panic("mbufgc mbuf parent cluster not free");
		*mp = m->m_next;
		while (m = m->m_next) {
			if (mtocl(m) == cl)	/* Drop this mbuf */
				++i, *mp = m->m_next;
			else			/* Keep this mbuf */
				mp = &m->m_next;
		}
		if (i != NMBPCL)
			panic("mbufgc mbuf unexpand botch");
		mclrefcnt[cl] = 0;
		cltom(cl)->mcl_next = mclfree;
		mclfree = cltom(cl);
		mbstat.m_clfree++;
		mbstat.m_mfree -= NMBPCL;
		mbstat.m_mbufs -= NMBPCL;
		MBUF_UNLOCK();
		splx(s);
		/* No oinkers */;
		s = splimp();
		MBUF_LOCK();
	}
#if	NO_INTERRUPT_ALLOCATION
	/* Free cluster mbufs */
	if (m_howmany() < 0) {
		register union mcluster *mclhead = mclfree;
		if (mclhead == 0)
			panic("mbufgc no clusters");
		mclfree = mclhead->mcl_next;
		cl = mtocl(mclhead);
		i = 1;
		if (nclpp > 1) {
			register union mcluster *mcl, **mclp, **mclhp;
			mcl = mclfree;
			mclp = &mclfree;
			mclhead->mcl_next = 0;
			mclhp = &mclhead->mcl_next;
			cl -= (cl % nclpp);
			while (mcl) {
				if ((unsigned)(mtocl(mcl) - cl) < nclpp) {
					*mclp = mcl->mcl_next;
					*mclhp = mcl;
					mcl->mcl_next = 0;
					mclhp = &mcl->mcl_next;
					++i;
				} else
					mclp = &mcl->mcl_next;
				mcl = *mclp;
			}
			if (i > nclpp)
				panic("mbufgc too many");
			/* Sort the incomplete ones to the end... */
			if (i != nclpp) {
				*mclp = mclhead;
				goto nogc;
			}
		}
		/* Got one! */
		mbstat.m_clfree -= i;
		mbstat.m_clusters -= i;
		for (i += cl; i-- > cl; )
			if (--mclrefcnt[i] != -1)
				panic("mbufgc cluster not free");
		MBUF_UNLOCK();
		splx(s);
#if	MACH
		/* Since we are completely pageable, we will never deallocate
		 * the unused mbuf memory.  It will just fade away to the
		 * paging space.  Vm_deallocate could be used here if desired.
		 */
#else
		panic("write some code in mbufgc");
#endif
	} else
#endif
	{
nogc:
		MBUF_UNLOCK();
		splx(s);
	}
}

/*
 * Cluster freelist allocation check. Mbuf lock/splimp must be held.
 * Ensure hysteresis between hi/lo.
 */
#define MINCL	max(16, NETNCPUS)
static int
m_howmany()
{
	register int i;

	/* Under minimum */
	if (mbstat.m_clusters < MINCL)
		return (MINCL - mbstat.m_clusters);
	/* Too few (free < 1/8 total) and not over maximum */
	if (mbstat.m_clusters < nmbclusters &&
	    (i = (mbstat.m_clusters >> 3) - mbstat.m_clfree) > 0)
		return i;
#if	NO_INTERRUPT_ALLOCATION
	/* Not a good time to free anything */
	if (m_want || !mbufgcenable)
		return 0;
	/* Don't free below minimum */
	if (mbstat.m_clusters - nclpp < MINCL)
		return 0;
#ifdef	OSF1_SERVER
#else	/* OSF1_SERVER */
	/* Too many (free > 1/2 total) */
	i = (mbstat.m_clusters >> 1) - mbstat.m_clfree;
	if (i < 0)
		return i;
#endif	/* OSF1_SERVER */
#endif
	return 0;
}

/*
 * Add more free mbufs by cutting up a cluster.
 */
m_expand(canwait)
	int canwait;
{
	register caddr_t mcl;

	MCLALLOC(mcl, canwait);
	if (mcl) {
		register struct mbuf *m = (struct mbuf *)mcl;
		register int i = NMBPCL;
		int s = splimp();
		MBUF_LOCK();
		mbstat.m_mfree += i;
		mbstat.m_mbufs += i;
		while (i--) {
			m->m_type = MT_FREE;
			m->m_next = mfree;
			mfree = m++;
		}
		i = m_want;
		m_want = 0;
		MBUF_UNLOCK();
		splx(s);
		if (i) wakeup((caddr_t)&mfree);
		return 1;
	}
	return 0;
}

/*
 * When MGET fails, ask protocols to free space when short of memory,
 * then re-attempt to allocate an mbuf.
 */

struct mbuf *
m_retry(canwait, type)
	int canwait, type;
{
#define	m_retry(h, t)	0
	register struct mbuf *m;
	int wait, s;

	for (;;) {
		(void) m_expand(canwait);
		MGET(m, XXX, type);
		if (m || canwait == M_DONTWAIT)
			break;
		s = splimp();
		MBUF_LOCK();
		wait = m_want++;
		if (wait == 0)
			mbstat.m_drain++;
		else {
			assert_wait((caddr_t)&mfree, FALSE);
			mbstat.m_wait++;
		}
		MBUF_UNLOCK();
		if (wait == 0) {
			splx(s);
			pfreclaim();
		} else {
			/* Sleep with a small timeout as insurance */
#if	!MACH
			(void) tsleep((caddr_t)&mfree, PZERO-1, "m_retry", hz);
#else
			(void) tsleep((caddr_t)0, PZERO-1, "m_retry", hz);
#endif
			splx(s);
		}
	}
	return (m);
#undef	m_retry
}

/*
 * As above; retry an MGETHDR.
 */
struct mbuf *
m_retryhdr(canwait, type)
	int canwait, type;
{
	register struct mbuf *m;

	if (m = m_retry(canwait, type)) {
		m->m_flags |= M_PKTHDR;
		m->m_data = m->m_pktdat;
	}
	return (m);
}

/*
 * Space allocation routines.
 * These are also available as macros
 * for critical paths.
 */
struct mbuf *
m_get(canwait, type)
	int canwait, type;
{
	register struct mbuf *m;

	MGET(m, canwait, type);
	return (m);
}

struct mbuf *
m_gethdr(canwait, type)
	int canwait, type;
{
	register struct mbuf *m;

	MGETHDR(m, canwait, type);
	return (m);
}

struct mbuf *
m_getclr(canwait, type)
	int canwait, type;
{
	register struct mbuf *m;

	MGET(m, canwait, type);
	if (m)
		bzero(mtod(m, caddr_t), MLEN);
	return (m);
}

/*
 * Return mbuf to freelist, freeing associated cluster if present.
 * If cluster requires special action, place whole mbuf on mfreelater
 * and schedule later freeing (so as not to free from interrupt level).
 */
struct mbuf *
m_free(m)
	struct mbuf *m;
{
	struct mbuf *n = m->m_next;
	int i, s;

	if (m->m_type == MT_FREE)
		panic("freeing free mbuf");
	s = splimp();
	MBUF_LOCK();
	if (m->m_flags & M_EXT) {
		if (MCLREFERENCED(m)) {		/* Unlink with lock held */
			remque(&m->m_ext.ext_ref);
		} else if (m->m_ext.ext_free == NULL) {
			union mcluster *mcl= (union mcluster *)m->m_ext.ext_buf;
			if (MCLUNREF(mcl)) {
				mcl->mcl_next = mclfree;
				mclfree = mcl;
				++mbstat.m_clfree;
			} else	/* sanity check - not referenced this way */
				panic("m_free m_ext cluster not free");
		} else {
			m->m_next = mfreelater;
			mfreelater = m;
			MBUF_UNLOCK();
			schednetisr(NETISR_MB);
			splx(s);
			return n;
		}
	}

	mbstat.m_mfree++;
	mbstat.m_mtypes[m->m_type]--;
	(void) MCLUNREF(m);
	m->m_type = MT_FREE;
	m->m_flags = 0;
	m->m_next = mfree;
	mfree = m;
	i = m_want;
	m_want = 0;
	MBUF_UNLOCK();
	splx(s);
	if (i) wakeup((caddr_t)&mfree);
	return (n);
}

void
m_freem(m)
	register struct mbuf *m;
{
	while (m)
		m = m_free(m);
}

/*
 * Mbuffer utility routines.
 */

/*
 * Compute the amount of space available
 * before the current start of data in an mbuf.
 */
m_leadingspace(m)
register struct mbuf *m;
{
	if (m->m_flags & M_EXT) {
		if (MCLREFERENCED(m))
			return 0;
		return (m->m_data - m->m_ext.ext_buf);
	}
	if (m->m_flags & M_PKTHDR)
		return (m->m_data - m->m_pktdat);
	return (m->m_data - m->m_dat);
}

/*
 * Compute the amount of space available
 * after the end of data in an mbuf.
 */
m_trailingspace(m)
register struct mbuf *m;
{
	if (m->m_flags & M_EXT) {
		if (MCLREFERENCED(m))
			return 0;
		return (m->m_ext.ext_buf + m->m_ext.ext_size -
			(m->m_data + m->m_len));
	}
	return (&m->m_dat[MLEN] - (m->m_data + m->m_len));
}

/*
 * Lesser-used path for M_PREPEND:
 * allocate new mbuf to prepend to chain,
 * copy junk along.
 */
struct mbuf *
m_prepend(m, len, how)
	register struct mbuf *m;
	int len, how;
{
	struct mbuf *mn;

	MGET(mn, how, m->m_type);
	if (mn == (struct mbuf *)NULL) {
		m_freem(m);
		return ((struct mbuf *)NULL);
	}
	if (m->m_flags & M_PKTHDR) {
		M_COPY_PKTHDR(mn, m);
		m->m_flags &= ~M_PKTHDR;
	}
	mn->m_next = m;
	m = mn;
	if (len < MHLEN)
		MH_ALIGN(m, len);
	m->m_len = len;
	return (m);
}

/*
 * Make a copy of an mbuf chain starting "off0" bytes from the beginning,
 * continuing for "len" bytes.  If len is M_COPYALL, copy to end of mbuf.
 * The wait parameter is a choice of M_WAIT/M_DONTWAIT from caller.
 */
struct mbuf *
m_copym(m, off0, len, wait)
	register struct mbuf *m;
	int off0, wait;
	register int len;
{
	register struct mbuf *n, **np;
	register int off = off0;
	struct mbuf *top;
	int copyhdr = 0;

	if (off < 0 || len < 0)
		panic("m_copym");
	if (off == 0 && m->m_flags & M_PKTHDR)
		copyhdr = 1;
	while (off > 0) {
		if (m == 0)
			panic("m_copym");
		if (off < m->m_len)
			break;
		off -= m->m_len;
		m = m->m_next;
	}
	np = &top;
	top = 0;
	while (len > 0) {
		if (m == 0) {
			if (len != M_COPYALL)
				panic("m_copym");
			break;
		}
		MGET(n, wait, m->m_type);
		*np = n;
		if (n == 0)
			goto nospace;
		if (copyhdr) {
			M_COPY_PKTHDR(n, m);
			if (len == M_COPYALL)
				n->m_pkthdr.len -= off0;
			else
				n->m_pkthdr.len = len;
			copyhdr = 0;
		}
		n->m_len = MIN(len, m->m_len - off);
		if (m->m_flags & M_EXT) {
			int s = splimp();
			MBUF_LOCK();
			n->m_ext = m->m_ext;
			insque(&n->m_ext.ext_ref, &m->m_ext.ext_ref);
			MBUF_UNLOCK();
			splx(s);
			n->m_data = m->m_data + off;
			n->m_flags |= M_EXT;
		} else
			bcopy(mtod(m, caddr_t)+off, mtod(n, caddr_t),
			    (unsigned)n->m_len);
		if (len != M_COPYALL)
			len -= n->m_len;
		off = 0;
		m = m->m_next;
		np = &n->m_next;
	}
	return (top);
nospace:
	m_freem(top);
	return (0);
}

/*
 * Copy data from an mbuf chain starting "off" bytes from the beginning,
 * continuing for "len" bytes, into the indicated buffer.
 */
void
m_copydata(m, off, len, cp)
	register struct mbuf *m;
	register int off;
	register int len;
	caddr_t cp;
{
	register unsigned count;

	if (off < 0 || len < 0)
		panic("m_copydata");
	while (off > 0) {
		if (m == 0)
			panic("m_copydata");
		if (off < m->m_len)
			break;
		off -= m->m_len;
		m = m->m_next;
	}
	while (len > 0) {
		if (m == 0)
			panic("m_copydata");
		count = MIN(m->m_len - off, len);
		bcopy(mtod(m, caddr_t) + off, cp, count);
		len -= count;
		cp += count;
		off = 0;
		m = m->m_next;
	}
}

/*
 * Concatenate mbuf chain n to m.
 * Both chains must be of the same type (e.g. MT_DATA).
 * Any m_pkthdr is not updated.
 */
void
m_cat(m, n)
	register struct mbuf *m, *n;
{
	while (m->m_next)
		m = m->m_next;
	while (n) {
		if (m->m_flags & M_EXT ||
		    m->m_data + m->m_len + n->m_len >= &m->m_dat[MLEN]) {
			/* just join the two chains */
			m->m_next = n;
			return;
		}
		/* splat the data from one into the other */
		bcopy(mtod(n, caddr_t), mtod(m, caddr_t) + m->m_len,
		    (u_int)n->m_len);
		m->m_len += n->m_len;
		n = m_free(n);
	}
}

void
m_adj(mp, req_len)
	struct mbuf *mp;
{
	register int len = req_len;
	register struct mbuf *m;
	register count;

	if ((m = mp) == NULL)
		return;
	if (len >= 0) {
		/*
		 * Trim from head.
		 */
		while (m != NULL && len > 0) {
			if (m->m_len <= len) {
				len -= m->m_len;
				m->m_len = 0;
				m = m->m_next;
			} else {
				m->m_len -= len;
				m->m_data += len;
				len = 0;
			}
		}
		if ((m = mp)->m_flags & M_PKTHDR)
			m->m_pkthdr.len -= (req_len - len);
	} else {
		/*
		 * Trim from tail.  Scan the mbuf chain,
		 * calculating its length and finding the last mbuf.
		 * If the adjustment only affects this mbuf, then just
		 * adjust and return.  Otherwise, rescan and truncate
		 * after the remaining size.
		 */
		len = -len;
		count = 0;
		for (;;) {
			count += m->m_len;
			if (m->m_next == (struct mbuf *)0)
				break;
			m = m->m_next;
		}
		if (m->m_len >= len) {
			m->m_len -= len;
			if ((m = mp)->m_flags & M_PKTHDR)
				m->m_pkthdr.len -= len;
			return;
		}
		count -= len;
		if (count < 0)
			count = 0;
		/*
		 * Correct length for chain is "count".
		 * Find the mbuf with last data, adjust its length,
		 * and toss data from remaining mbufs on chain.
		 */
		if ((m = mp)->m_flags & M_PKTHDR)
			m->m_pkthdr.len = count;
		for (; m; m = m->m_next) {
			if (m->m_len >= count) {
				m->m_len = count;
				break;
			}
			count -= m->m_len;
		}
		while (m = m->m_next)
			m->m_len = 0;
	}
}

/*
 * Rearange an mbuf chain so that len bytes are contiguous
 * and in the data area of an mbuf (so that mtod and dtom
 * will work for a structure of size len).  Returns the resulting
 * mbuf chain on success, frees it and returns null on failure.
 * If there is room, it will add up to max_protohdr-len extra bytes to the
 * contiguous region in an attempt to avoid being called next time.
 */
struct mbuf *
m_pullup(n, len)
	register struct mbuf *n;
	int len;
{
	register struct mbuf *m;
	register int count;
	int space;

	/*
	 * If first mbuf has no cluster, and has room for len bytes
	 * without shifting current data, pullup into it,
	 * otherwise allocate a new mbuf to prepend to the chain.
	 */
	if ((n->m_flags & M_EXT) == 0 &&
	    n->m_data + len < &n->m_dat[MLEN] && n->m_next) {
		if (n->m_len >= len)
			return (n);
		m = n;
		n = n->m_next;
		len -= m->m_len;
	} else {
		if (len > MHLEN)
			goto bad;
		MGET(m, M_DONTWAIT, n->m_type);
		if (m == 0)
			goto bad;
		m->m_len = 0;
		if (n->m_flags & M_PKTHDR) {
			M_COPY_PKTHDR(m, n);
			n->m_flags &= ~M_PKTHDR;
		}
	}
	space = &m->m_dat[MLEN] - (m->m_data + m->m_len);
	do {
		count = min(min(max(len, max_protohdr), space), n->m_len);
		bcopy(mtod(n, caddr_t), mtod(m, caddr_t) + m->m_len,
		  (unsigned)count);
		len -= count;
		m->m_len += count;
		n->m_len -= count;
		space -= count;
		if (n->m_len)
			n->m_data += count;
		else
			n = m_free(n);
	} while (len > 0 && n);
	if (len > 0) {
		(void) m_free(m);
		goto bad;
	}
	m->m_next = n;
	return (m);
bad:
	m_freem(n);
	return (0);
}


/*
 * Allocate a "funny" mbuf, that is, one whose data is owned by someone else.
 *
 * A temporary hack to coincide with mach2.5 mclgetx
 * check with rick
 */
struct mbuf *
mclgetx(fun, arg, addr, len, wait)
        void (*fun)();
        int arg, len, wait;
        caddr_t addr;
{
        register struct mbuf *m;

        MGETHDR(m, wait, MT_DATA);
        if (m == 0)
                return (0);
        m->m_data = addr ;
        m->m_len = len;
        m->m_ext.ext_free = fun;
        m->m_ext.ext_size = len;
        m->m_ext.ext_buf = (caddr_t)arg;
        m->m_flags |= M_EXT;
        m->m_ext.ext_ref.forw = m->m_ext.ext_ref.back =
          &m->m_ext.ext_ref;

        return (m);
}


/*
 * Copy an mbuf to the contiguous area pointed to by cp.
 * Skip <off> bytes and copy <len> bytes.
 * Returns the number of bytes not transferred.
 * The mbuf is NOT changed.
 */
int
m_cpytoc(m, off, len, cp)
	register struct mbuf *m;
	register int off, len;
	register caddr_t cp;
{
	register int ml;

	if (m == NULL || off < 0 || len < 0 || cp == NULL)
		panic("m_cpytoc");
	while (off && m)
		if (m->m_len <= off) {
			off -= m->m_len;
			m = m->m_next;
			continue;
		} else
			break;
	if (m == NULL)
		return (len);

	ml = imin(len, m->m_len - off);
	bcopy(mtod(m, caddr_t)+off, cp, (u_int)ml);
	cp += ml;
	len -= ml;
	m = m->m_next;

	while (len && m) {
		ml = m->m_len;
		bcopy(mtod(m, caddr_t), cp, (u_int)ml);
		cp += ml;
		len -= ml;
		m = m->m_next;
	}

	return (len);
}


#if	MACH

#include <streams.h>
#if	STREAMS

/* Mbuf <-> Mblk conversion routines */
/*
 * Copy mbufs to and from mblks. Note these structures are very
 * similar but do not _quite_ line up.
 * A semi-complete list:
 *	MBUF		MBLK
 *	m_data		b_rptr
 *	m_len		b_wptr-b_rptr
 *	m_data+m_len	b_wptr
 *	m_next		b_cont
 *	m_nextpkt	b_next
 *	m_ext		mh_dblk
 *
 *	m_get()		allocb()/allocbi()
 *	m_freem()	freeb()
 *
 * When copying, for now we simply wrap each buffer in the chain
 * inside a buffer of the destination. This is to 1) avoid bcopies,
 * 2) simplify, 3) support only the necessary stuff, and could well
 * be changed.
 */

#include <sys/stream.h>

/*ARGSUSED*/
static void
m_freeb(p, size, bp)
	caddr_t p, bp;
	int size;
{
	(void) freeb((mblk_t *)bp);
}


mblk_t *
mbuf_to_mblk(m, pri)
	struct mbuf *m;
{
	mblk_t *bp, *top = 0, **bpp = &top;
	struct mbuf *nm;

	while (m) {
		int front = m_leadingspace(m);
		int back  = m_trailingspace(m);
		bp = allocbi(front + m->m_len + back, pri,
			(pfi_t)m_free, (char *)m, (u_char *)(m->m_data - front));
		if (bp == 0) {
			while (top) {
				++top->b_datap->db_ref;
				bp = top->b_cont;
				(void) freeb(top);
				top = bp;
			}
			break;
		}
		bp->b_rptr = (unsigned char *)m->m_data;
		bp->b_wptr = (unsigned char *)(m->m_data + m->m_len);
		*bpp = bp;
		bpp = &bp->b_cont;
		nm = m->m_next;
		/*m->m_next = 0;*/
		m = nm;
	}
	return top;
}

struct mbuf *
mblk_to_mbuf(bp, canwait)
	mblk_t *bp;
{
	struct mbuf *m, *top = 0;
	struct mbuf **mp = &top;
	mblk_t *nbp;
	int len = 0;

	while (bp) {
		if (top == 0)
			m = m_gethdr(canwait, MT_DATA);
		else
			m = m_get(canwait, MT_DATA);
		if (m == 0) {
			while (top) {
				top->m_flags &= ~M_EXT;
				top = m_free(top);
			}
			break;
		}
		m->m_data = (caddr_t)bp->b_rptr;
		m->m_len = bp->b_wptr - bp->b_rptr;
		m->m_flags |= M_EXT;
		if (bp->b_datap->db_ref > 1) {		/* referenced */
			m->m_ext.ext_buf = m->m_data;
			m->m_ext.ext_size = m->m_len;
		} else {				/* available */
			m->m_ext.ext_buf = (caddr_t)bp->b_datap->db_base;
			m->m_ext.ext_size =
				bp->b_datap->db_lim - bp->b_datap->db_base;
		}
		m->m_ext.ext_free = m_freeb;
		m->m_ext.ext_arg = (caddr_t)bp;
		m->m_ext.ext_ref.forw = m->m_ext.ext_ref.back = 
			&m->m_ext.ext_ref;
		*mp = m;
		mp = &m->m_next;
		len += m->m_len;
		nbp = bp->b_cont;
		/*bp->b_cont = 0;*/
		bp = nbp;
	}
	if (top) {
		top->m_pkthdr.len = len;
		top->m_pkthdr.rcvif = 0;
	}
	return top;
}

#endif	/* STREAMS */
#endif	/* MACH */

