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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: trace.h,v $
 * Revision 1.4  1994/11/18  20:41:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:29:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:43  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:50  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:28  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:29  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:14:51  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:46:21  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:55:26  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:09:02  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:47:11  gm]
 * 
 * Revision 1.3  90/03/27  13:28:00  gm
 * 	Changes from George at Encore
 * 	[90/03/15  17:44:11  pam]
 * 
 * Revision 1.2  90/03/08  12:05:31  gmf
 * 	change pack macro so that it can take vp as argument
 * 
 * Revision 1.1  90/03/08  12:04:35  gmf
 * 	Initial revision
 * 
 * Revision 1.2  90/01/02  20:16:08  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:41  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:25:35  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.5  89/03/09  22:08:54  rpd
 * 	More cleanup.
 * 
 * Revision 2.4  89/02/25  17:57:17  gm0w
 * 	Made all CMUCS conditional stuff always true. Changed
 * 	romp to ibmrt.
 * 	[89/02/14            mrt]
 * 
 * Revision 2.3  88/11/23  16:36:57  rpd
 * 	Added fd driver trace points from Acis.
 * 	[88/11/04  17:56:32  rpd]
 * 
 * Revision 2.2  88/08/24  02:49:44  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:26:00  mwyoung]
 * 
 * Revision 0.0  87/08/24            sanzi
 * 	Added several trace values for the romp from the 4.3 ACIS code.
 * 	[87/08/24            sanzi]
 * 
 * Revision 0.0  86/10/14            avie
 * 	Conditionalize the previous bug fix on CS_BUGFIX.
 * 	[86/10/14            avie]
 * 
 * Revision 0.0  86/10/07            dlb
 * 	trace(a,b,b) -> trace(a,b,c) bug fix.
 * 	[86/10/07            dlb]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)trace.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_TRACE_H_
#define _SYS_TRACE_H_

/*
 * File system buffer tracing points; all trace <pack(dev, size), bn>
 */
#define TR_BREADHIT	0	/* buffer read found in cache */
#define TR_BREADMISS	1	/* buffer read not in cache */
#define TR_BWRITE	2	/* buffer written */
#define TR_BREADHITRA	3	/* buffer read-ahead found in cache */
#define TR_BREADMISSRA	4	/* buffer read-ahead not in cache */
#define TR_XFODMISS	5	/* exe fod read */
#define TR_XFODHIT	6	/* exe fod read */
#define TR_BRELSE	7	/* brelse */
#define TR_BREALLOC	8	/* expand/contract a buffer */

/*
 * Memory allocator trace points; all trace the amount of memory involved
 */
#define TR_MALL		10	/* memory allocated */

/*
 * Paging trace points: all are <vaddr, pid>
 */
#define TR_INTRANS	20	/* page intransit block */
#define TR_EINTRANS	21	/* page intransit wait done */
#define TR_FRECLAIM	22	/* reclaim from free list */
#define TR_RECLAIM	23	/* reclaim from loop */
#define TR_XSFREC	24	/* reclaim from free list instead of drum */
#define TR_XIFREC	25	/* reclaim from free list instead of fsys */
#define TR_WAITMEM	26	/* wait for memory in pagein */
#define TR_EWAITMEM	27	/* end memory wait in pagein */
#define TR_ZFOD		28	/* zfod page fault */
#define TR_EXFOD	29	/* exec fod page fault */
#define TR_VRFOD	30	/* vread fod page fault */
#define TR_CACHEFOD	31	/* fod in file system cache */
#define TR_SWAPIN	32	/* drum page fault */
#define TR_PGINDONE	33	/* page in done */
#define TR_SWAPIO	34	/* swap i/o request arrives */

/*
 * System call trace points.
 */
#define TR_VADVISE	40	/* vadvise occurred with <arg, pid> */

/*
 * Miscellaneous
 */
#define TR_STAMP	45	/* user said vtrace(VTR_STAMP, value); */
#ifdef	ibmrt
#define TR_RWIP		49	/* rwip() */

/*
 * Disk device driver (EESDI only at present) trace points
 */
#define TR_D_ENQ	50	/* enqueue - strategy called */
#define TR_D_GO		51	/* go - cmd given to adapter */
#define TR_D_INT	52	/* interrupt - from adapter */
/*
 * Floppy device driver trace points
 */
#define TR_F_STR	60	/* enqueue - fdstrategy called */
#define TR_F_STA	61	/* fdstart called */
#define TR_F_DGO	62	/* fddgo called */
#define TR_F_INT	63	/* interrupt from adapter - fdint() */
#define TR_F_AUT	64	/* fdautodensity - top of loop */
#define TR_F_REL	65	/* releasing a buffer */
#define TR_F_OPE	66	/* fdopen */
#define TR_F_CLO	67	/* fdclose */
#define TR_F_WAI	68	/* waiting for i/o complete on a buffer */
#define TR_F_FOR	69	/* fdformat */
#endif	ibmrt

/*
 * This defines the size of the trace flags array.
 */
#define TR_NFLAGS	100	/* generous */

#define TRCSIZ		4096

/*
 * Specifications of the vtrace() system call, which takes one argument.
 */
#define VTRACE		64+51

#define VTR_DISABLE	0		/* set a trace flag to 0 */
#define VTR_ENABLE	1		/* set a trace flag to 1 */
#define VTR_VALUE	2		/* return value of a trace flag */
#define VTR_UALARM	3		/* set alarm to go off (sig 16) */
					/* in specified number of hz */
#define VTR_STAMP	4		/* user specified stamp */

#ifdef	KERNEL
#ifdef	TRACE
extern char	traceflags[TR_NFLAGS];
extern struct	proc *traceproc;
extern int	tracebuf[TRCSIZ];
extern unsigned tracex;
extern int	tracewhich;

#define pack(a,b)	(((v)->v_mount->m_stat.f_fsid[0])<<16)|(b)
#define trace(a,b,c)	if (traceflags[a]) trace1(a,b,c)
#else	TRACE
#define trace(a,b,c)
#endif	TRACE

#endif	KERNEL
#endif	_SYS_TRACE_H_
