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
 * $Log: net_malloc.h,v $
 * Revision 1.5  1994/11/18  20:33:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:08:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:28:21  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:49  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:50  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:26  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:10  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:45  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1992/03/09  14:40:02  durriya
 * 	92/01/07  23:34:20  condict
 * 	Keep the use of kalloc (=zalloc) even though NETISR_THREADS is now off.
 *
 * Revision 2.2  91/08/31  13:40:16  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:33:31  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:32:12  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:10:46  gm]
 * 
 * Revision 1.7  90/07/27  08:59:13  devrcs
 * 	Checkpoint at Reno merge.
 * 	[90/07/19  16:40:51  tmt]
 * 
 * Revision 1.6  90/04/27  19:12:58  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:27:59  tmt]
 * 
 * Revision 1.5  90/04/14  00:32:20  devrcs
 * 	Move netmalloc stats to uipc_mbuf.c
 * 	[90/04/09  16:08:24  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:45  gm
 * 	Fix casting problem which forced less efficient macro code (kchunk *).
 * 	[90/01/08  15:52:25  tmt]
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
 * Revision 1.3  90/01/02  20:10:52  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:45:19  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Malloc header file for OSF/1 networking.
 *
 *	NET_MALLOC(pointer, cast, size, type, flag)
 *	NET_FREE(pointer, type)
 *	(types, flags, etc defined)
 *
 * If Unix (4.4) we expect malloc.h to define the support.
 * Else if Mach, we use zones and other mechanisms.
 */

#if	!MACH

#ifndef	M_WAITOK
#include "sys/malloc.h"
#endif

#define	NET_MALLOC(p,c,s,t,f)	MALLOC(p,c,s,t,f)
#define NET_FREE(p,t)		FREE(p,t)

#else	/* MACH */

/*
 * Types of memory to be allocated (not all are used by net)
 */
#define	M_FREE		0	/* should be on free list */
#define M_MBUF		1	/* mbuf */
#define M_DEVBUF	2	/* device driver memory */
#define	M_SOCKET	3	/* socket structure */
#define	M_PCB		4	/* protocol control block */
#define	M_RTABLE	5	/* routing tables */
#define	M_HTABLE	6	/* IMP host tables */
#define	M_FTABLE	7	/* fragment reassembly header */
#define	M_IFADDR	9	/* interface address */
#define	M_SOOPTS	10	/* socket options */
#define	M_SONAME	11	/* socket name */
#define M_MOUNT		20	/* vfs mount struct */
#define M_FHANDLE	21	/* network file handle */
#define	M_NFSREQ	22	/* NFS request header */
#define	M_NFSMNT	23	/* NFS mount structure */
#define M_TEMP		31	/* misc temporary data buffers (was 49) */
#define M_LAST		32

struct netmallocstats {
	long	ns_inuse;	/* # of packets of this type currently in use */
	long	ns_calls;	/* total packets of this type ever allocated */
	long	ns_fail;	/* total times failed to get if M_NOWAIT */
	long	ns_maxsize;	/* biggest size allocated */
};

#include "kern/kalloc.h"

/*
 * "flags" to NET_MALLOC actually are routines to get memory. Note when
 * running in a thread context, we may block for the pmap routines to
 * fetch more memory. When in a software interrupt context, this is
 * not the case. The objects fetched by NET_MALLOC are of variable
 * length, but are largely < 256 bytes. Native zones may replace the
 * kalloc facility. Mbufs always come from native blocks (clusters).
 *
 * When using "kget" there is a danger of failing at many points. A zone
 * cannot be expanded until it is full, and then only when the caller
 * is willing to block. This causes problems all over the code. An
 * entry point to expand a zone would be a prerequisite to fixing this.
 */
#define M_WAITOK	kalloc
#if	NETISR_THREAD || OSF1_SERVER
#define M_NOWAIT	M_WAITOK
#else
#define M_NOWAIT	kget
#endif

/* Define a "kchunk" to save the length while preserving alignment */
typedef struct { int len; /* data follows */ } kchunk;

#define NET_MALLOC(addr, cast, size, type, flags) {		\
	netmallocstats[type].ns_calls++;			\
	if ((size) > netmallocstats[type].ns_maxsize)		\
		netmallocstats[type].ns_maxsize = (size);	\
	if ((addr) = (cast)flags((size)+sizeof(kchunk))) {	\
		((kchunk *)(addr))->len = ((size)+sizeof(kchunk));\
		(addr) = (cast)(((kchunk *)(addr))+1);		\
		netmallocstats[type].ns_inuse++;		\
	} else							\
		netmallocstats[type].ns_fail++;			\
}

#define NET_FREE(addr, type) {					\
	kfree((caddr_t)(((kchunk *)(addr))-1), (((kchunk *)(addr))-1)->len);\
	--netmallocstats[type].ns_inuse;			\
}

#ifdef	_KERNEL
extern struct	netmallocstats netmallocstats[M_LAST];
#endif
#endif	/* MACH */
