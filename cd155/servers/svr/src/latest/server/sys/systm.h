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
 * $Log: systm.h,v $
 * Revision 1.4  1994/11/18  20:41:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:28:59  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:31  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:07  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/03/01  18:39:19  pjg
 * 	Remove old def'n of calloc.  (loverso)
 *
 * Revision 2.2  91/08/31  14:13:54  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/07/31  15:45:44  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:54:49  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:46:11  gm]
 * 
 * Revision 1.5  90/06/22  20:54:17  devrcs
 * 	nags merge
 * 	[90/06/12  21:39:25  gmf]
 * 
 * Revision 1.4  90/02/23  00:33:10  devrcs
 * 	Removed erroneous declaration of bmap function.
 * 	[90/02/18  16:29:47  noemi]
 * 
 * Revision 1.3  90/01/03  12:44:06  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:41:56  gm]
 * 
 * Revision 1.2  89/12/26  10:41:08  gm
 * 	Merged 4.4BSD and Mach changes.
 * 	[89/12/25            noemi]
 * 
 * Revision 6.1  89/07/26  14:23:45  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.5  89/05/23  07:45:34  boykin
 * 	Merged MMAX_MP with Release 2.5.
 * 	Encore contributors to date:
 * 		Alan Langerman		(alan@encore.com)
 * 		Sue LoVerso		(sue@encore.com)
 * 	Encore changes to date:
 * 		MMAX_MP:  don't use global read-ahead variables.
 * 		Put include of "cpus.h" inside the #ifdef KERNEL.
 * 
 * Revision 2.10  89/04/22  15:32:30  gm0w
 * 	Changed extern boolean_t rpause() to rpsleep().
 * 	[89/04/20            gm0w]
 * 	Removed MACH_VFS changes.
 * 	[89/04/14            gm0w]
 * 
 * Revision 2.9  89/03/09  22:08:11  rpd
 * 	More cleanup.
 * 
 * Revision 2.8  89/02/25  17:56:45  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.7  89/01/30  22:08:31  rpd
 * 	Made declarations use "extern".
 * 	[89/01/25  15:23:46  rpd]
 * 
 * Revision 2.6  89/01/23  22:29:05  af
 * 	MIPS: externs to make compiler happy.
 * 	[89/01/10            af]
 * 
 * Revision 2.5  89/01/18  01:19:49  jsb
 * 	Vnode support: added bdevvp and specvp functions.
 * 	[89/01/13            jsb]
 * 
 * Revision 2.4  88/08/24  02:45:36  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:23:41  mwyoung]
 * 
 * Revision 0.0  88/03/29            mwyoung
 * 	MACH: Eliminated unused variables.
 * 	[88/03/29            mwyoung]
 * 
 * Revision 0.0  87/03/19            avie
 * 	Make redefinitions of insque/remque always ifdef'ed on lint.
 * 	[87/03/19            avie]
 * 
 * Revision 0.0  87/02/01            avie
 * 	Moved runrun and curpri definitions to sched.h for MACH.
 * 	[87/02/01            avie]
 * 
 * Revision 0.0  86/01/19            dlb
 * 	MACH: runrun and curpri are now per cpu.  Removed
 * 	runin, runout, wantin, and noproc.
 * 	[86/01/19            dlb]
 * 
 * Revision 0.0  86/12/17            mwyoung
 * 	Removed declaration of icode under MACH.
 * 	[86/12/17            mwyoung]
 * 
 * Revision 0.0  86/11/12            dlb
 * 	ns32000: #ifdef lint'ed insque and remque casts.  These bogosities
 * 	ought to be permanently #ifdef lint'ed for all systems.
 * 	[86/11/12            dlb]
 * 
 * Revision 0.0  86/10/07            dlb
 * 	Merged Multimax changes.
 * 	[86/10/07            dlb]
 * 
 * Revision 0.0  86/09/24            mwyoung
 * 	Changed to directly import declaration of boolean.
 * 	[86/09/24            mwyoung]
 * 
 * Revision 0.0  86/08/29            mwyoung
 * 	Converted from "bool" type to "boolean_t" where necessary.
 * 	[86/08/29            mwyoung]
 * 
 * Revision 0.0  86/07/16            bolosky
 * 	MACH: Removed definitions of swapdev and argdev.
 * 	[86/07/16            bolosky]
 * 
 * Revision 0.0  86/03/22            avie
 * 	Merged VM and Romp versions.
 * 	[86/03/22            avie]
 * 
 * Revision 0.0  86/02/18            bolosky
 * 	romp: Added new definition of icode[].
 * 	[86/02/18            bolosky]
 * 
 * Revision 0.0  86/01/25            avie
 * 	Upgraded to 4.3.
 * 	[86/01/25            avie]
 * 
 * Revision 0.0  85/11/04            avie
 * 	Add sy_parallel flag to the system call entries to specify
 * 	whether or not the system call can be executed in parallel.
 * 	[85/11/04            avie]
 * 
 * Revision 0.0  85/08/03            mja
 * 	CS_RPAUSE:  Added rpause() and fspause() declarations.
 * 	[85/08/03            mja]
 * 
 * Revision 0.0  85/06/20            mja
 * 	CMUCS:  Added bootdev definition.
 * 	[V1(1)]
 * 	[85/06/20            mja]
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
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)systm.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_SYSTM_H_
#define _SYS_SYSTM_H_

/*
 * Structure of the system-entry table
 */
struct sysent
{
	short	sy_narg;		/* total number of arguments */
	short	sy_parallel;		/* can execute in parallel */
	int	(*sy_call)();		/* handler */
};

#ifdef	_KERNEL
#include <mach/boolean.h>
#include <sys/types.h>

/*
 * Random set of variables
 * used by more than one
 * routine.
 */
extern char	version[];		/* system version */

/*
 * Nblkdev is the number of entries
 * (rows) in the block switch. It is
 * set in binit/bio.c by making
 * a pass over the switch.
 * Used in bounds checking on major
 * device numbers.
 */
extern int	nblkdev;

/*
 * Number of character switch entries.
 * Set by cinit/prim.c
 */
extern int	nchrdev;

extern int	mpid;			/* generic for unique process id's */
extern char	kmapwnt;		/* Make #if cleaner */

extern int	maxmem;			/* actual max memory per process */
extern int	physmem;		/* physical memory on this CPU */

extern int	updlock;		/* lock for sync */
extern int	intstack[];		/* stack for interrupts */
extern dev_t	rootdev;		/* device of the root */
extern struct vnode *rootvp;		/* vnode of root filesystem */
extern dev_t	dumpdev;		/* device to take dumps on */
extern long	dumplo;			/* offset into dumpdev */

extern struct sysent sysent[];
extern char	*panicstr;
#ifndef	OSF1_SERVER
extern int	boothowto;		/* reboot flags, from console subsys */
#else	OSF1_SERVER
int	boothowto;
#endif	OSF1_SERVER
extern int	show_space;
extern u_long	bootdev;		/* boot dev, from bootstrap subsys */
extern int	selwait;

#ifdef	lint
/* casts to keep lint happy */
#define insque(q,p)	_insque((caddr_t)q,(caddr_t)p)
#define remque(q)	_remque((caddr_t)q)
#endif	lint

#endif	/* _KERNEL */
#endif	/* _SYS_SYSTM_H_ */
