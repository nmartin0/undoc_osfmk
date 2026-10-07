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
 * $Log: reboot.h,v $
 * Revision 1.4  1994/11/18  20:41:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:26:55  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:24  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:56  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:10:31  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.3  91/07/31  15:44:35  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:53:10  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:43:41  gm]
 * 
 * Revision 1.3  90/08/24  12:26:55  devrcs
 * 	HP/Apollo M68K
 * 	[90/08/13  17:37:23  mcg]
 * 
 * Revision 1.2  90/01/03  12:43:51  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:41:29  gm]
 * 
 * Revision 1.1  89/10/16  19:48:29  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:22:18  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.8  89/09/09  16:23:38  rvb
 * 	RB_KDB must ALWAYS be defined even if mach_kdb is off.
 * 	[89/09/07            rvb]
 * 
 * Revision 2.7  89/03/09  22:07:02  rpd
 * 	More cleanup.
 * 
 * Revision 2.6  89/02/25  17:55:48  gm0w
 * 	Changed to generic machine names and eliminated include
 * 	of cputypes.h. Made the MACH_KDB conditional always be
 * 	be used outside the kernel.
 * 	[89/02/14            mrt]
 * 
 * Revision 2.5  88/11/23  16:45:46  rpd
 * 	romp: Added RB_SUSPEND from Acis.
 * 	[88/11/04  18:05:05  rpd]
 * 
 * Revision 2.4  88/08/24  02:41:12  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:21:13  mwyoung]
 * 
 * Revision 2.3  88/08/09  17:59:57  rvb
 * 	Added RB_ALTBOOT and RB_UNIPROC.
 * 
 * Revision 0.0  87/01/08            beck
 * 	Add Sequent specific stuff under #ifdef BALANCE.
 * 	[87/01/08            beck]
 * 
 * Revision 0.0  86/10/21            jjc
 * 	Added RB_NOBOOTRC for SUN.
 * 	[86/10/21            jjc]
 * 
 * Revision 0.0  86/10/20            dlb
 * 	Added include of cputypes.h to pick up MULTIMAX definition
 * 	[86/10/20            dlb]
 * 
 * Revision 0.0  86/10/14            avie
 * 	Didn't special case RB_KDB for Multimax, this wasn't necessary
 * 	since the Multimax code uses the RB_DEBUG mnemonic.
 * 	[86/10/14            avie]
 * 
 * Revision 0.0  86/10/07            dlb
 * 	Added Multimax boot flag definitions.  Multimax uses a different
 * 	debugger flag than other systems.
 * 	[86/10/07            dlb]
 * 
 * Revision 0.0  86/01/25            avie
 * 	Upgraded to 4.3.
 * 	[86/01/25            avie]
 * 
 * Revision 0.0  85/10/22            avie
 * 	CMUCS_KDB:  Added RB_KDB flag for kernel debugger.  This is currently
 * 	defined at 0x04 (RB_NOSYNC) to be backward compatible, but
 * 	should be changed in the future (when it is convenient to
 * 	update the boot programs).
 * 	[85/10/22            avie]
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
 *	@(#)reboot.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_REBOOT_H_
#define _SYS_REBOOT_H_

#ifdef	KERNEL
#ifndef	OSF1_SERVER
#include <mach_kdb.h>
#endif	OSF1_SERVER
#endif	KERNEL

/*
 * Arguments to reboot system call.
 * These are passed to boot program in r11,
 * and on to init.
 */

#define RB_AUTOBOOT	0	/* flags for system auto-booting itself */

#define RB_ASKNAME	0x01	/* ask for file name to reboot from */
#define RB_SINGLE	0x02	/* reboot to single user only */
#define RB_NOSYNC	0x04	/* dont sync before reboot */
#define RB_KDB		0x04	/* load kernel debugger */
#define RB_HALT		0x08	/* don't reboot, just halt */
#define RB_INITNAME	0x10	/* name given for /etc/init */
#define RB_DFLTROOT	0x20	/* use compiled-in rootdev */
#define RB_ALTBOOT	0x40	/* use /boot.old vs /boot */
#define RB_UNIPROC	0x80	/* don't start slaves */
#define RB_DEBUGGER     0x1000  /* enter debugger NOW */
#define RB_PANIC	0	/* reboot due to panic */
#define RB_BOOT		1	/* reboot due to boot() */

/*
 * Constants for converting boot-style device number to type,
 * adaptor (uba, mba, etc), unit number and partition number.
 * Type (== major device number) is in the low byte
 * for backward compatibility.  Except for that of the "magic
 * number", each mask applies to the shifted value.
 */
#define B_ADAPTORSHIFT	24
#define B_ADAPTORMASK	0x0f
#define B_UNITSHIFT	16
#define B_UNITMASK	0xff
#define B_PARTITIONSHIFT 8
#define B_PARTITIONMASK	0xff
#define B_TYPESHIFT	0
#define B_TYPEMASK	0xff
#define B_MAGICMASK	0xf0000000
#define B_DEVMAGIC	0xa0000000

#ifdef	ibmrt
#define RB_SUSPEND	0x40	/* (6152) suspend unix */
#endif	ibmrt
#if	defined(sun3) || defined(sun4)
#define RB_NOBOOTRC	0x20	/* don't run '/etc/rc.boot' */
#endif	defined(sun3) || defined(sun4)
#if	multimax
/* Additional boot flags on multimax, plus bit defs for standard flags
	Note that multimax uses a different bit for debugger. */

#define RB_B_ASKNAME	0	/* Ask for file name to reboot from */
#define RB_B_SINGLE	1	/* Reboot to single user only */
#define RB_B_NOSYNC	2	/* Don't sync before reboot */
#define RB_B_HALT	3	/* Don't reboot, just halt */
#define RB_B_INITNAME	4	/* Name given for /etc/init */

#define RB_B_PROFILED	28	/* OS Profiling */
#define RB_PROFILED	(1 << RB_B_PROFILED)
#define RB_B_MULTICPU	29	/* Multiprocessor boot */
#define RB_MULTICPU	(1 << RB_B_MULTICPU)
#define RB_B_INTERACT	30	/* Interactive boot */
#define RB_INTERACT	(1 << RB_B_INTERACT)
#define RB_B_DEBUG	31	/* Debug mode boot */
#define RB_DEBUG	(1 << RB_B_DEBUG)
#endif	multimax

#if	balance
/*
 * Sequent specific "reboot" flags.
 */
#define RB_NO_CTRL	0x20	/* for FIRMWARE, don't start controller */
#define RB_NO_INIT	0x40	/* for FIRMWARE, don't init system */
#define RB_AUXBOOT	0x80	/* Boot auxiliary boot name */
#define RB_DUMP		RB_AUXBOOT
#define RB_CONFIG	0x100	/* for FIRMWARE, only build cfg table */
#endif	balance

#if	defined(__hp_osf)
#define RB_TTY		0x100	/* use tty port instead of display */
#define RB_LOADONLY	0x200	/* load the image, do not execute */
#endif

#endif	_SYS_REBOOT_H_
