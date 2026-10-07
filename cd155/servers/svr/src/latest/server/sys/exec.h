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
 * $Log: exec.h,v $
 * Revision 1.4  1994/11/18  20:40:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:24:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:50  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:28  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:14  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:04:18  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:42:32  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:50:04  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:38:37  gm]
 * 
 * Revision 1.3  90/08/24  12:25:57  devrcs
 * 	HP/Apollo M68K
 * 	[90/08/13  17:37:02  mcg]
 * 
 * Revision 1.2  90/01/02  20:15:48  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:05  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:16:37  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.5  89/05/30  10:42:59  rvb
 * 	Keyed byteorder dependencies on BYTE_MFS.
 * 	[89/05/18            af]
 * 
 * Revision 2.4  89/03/09  22:03:50  rpd
 * 	More cleanup.
 * 
 * Revision 2.3  89/02/25  17:53:12  gm0w
 * 	Changed #ifdef sun to #if defined(sun3) || defined(sun4)
 * 	[89/02/13            mrt]
 * 
 * Revision 2.2  89/01/23  22:25:47  af
 * 	Added definitions for Mips.  Might be extended to cover all COFF
 * 	based systems.
 * 	[89/01/09            af]
 * 
 * Revision 0.0  86/09/12            jjc
 * 	Changed first long word to two shorts for the Sun to give
 * 	the machine type along with the magic number.
 * 	Added machine types for Sun.
 * 	Made file includable more than once.
 * 	[86/09/12            jjc]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)exec.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_EXEC_H_
#define _SYS_EXEC_H_

/*
 * Header prepended to each a.out file.
 */

#if	!defined(mips)

struct exec {

#if	defined(sun3) || defined(sun4) || defined(__hp_osf)
	unsigned short  a_machtype;     /* machine type */
	unsigned short  a_magic;        /* magic number */
#else	defined(sun3) || defined(sun4) || defined(__hp_osf)
	long	a_magic;	/* magic number */
#endif	defined(sun3) || defined(sun4) || defined(__hp_osf)
	unsigned long	a_text;		/* size of text segment */
	unsigned long	a_data;		/* size of initialized data */
	unsigned long	a_bss;		/* size of uninitialized data */
	unsigned long	a_syms;		/* size of symbol table */
	unsigned long	a_entry;	/* entry point */
	unsigned long	a_trsize;	/* size of text relocation */
	unsigned long	a_drsize;	/* size of data relocation */
};
#endif	!defined(mips)


#define OMAGIC	0407		/* old impure format */
#define NMAGIC	0410		/* read-only text */
#define ZMAGIC	0413		/* demand load format */


#ifdef	mips

#if	BYTE_MSF
#define MIPSMAGIC	MIPSEBMAGIC
#else
#define MIPSMAGIC	MIPSELMAGIC
#endif
/*
 * The macro N_TXTOFF() takes pointers to file header
 * [struct filehdr*] and optional header [struct aouthdr *] and returns
 * the file offset to the start of the raw data for the .text section.
 * The raw data for the three data sections follows the start of the .text
 * section by the value of tsize in the optional header.
 */
/* SCNROUND is the size that section headers are rounded off to */
#define SCNROUND ((long)16)
#define OSCNRND  ((long)8)

#define N_TXTOFF(f, a) \
 ((a).magic == ZMAGIC ? 0 : \
  ((a).vstamp < 23 ? \
   ((FILHSZ + AOUTHSZ + (f).f_nscns * SCNHSZ + OSCNRND-1) & ~(OSCNRND-1)) : \
   ((FILHSZ + AOUTHSZ + (f).f_nscns * SCNHSZ + SCNROUND-1) & ~(SCNROUND-1))))

/*
 * for compatibility
 */
#define a_data	ex_o.dsize
#define a_text	ex_o.tsize
#define a_bss	ex_o.bsize
#define a_entry	ex_o.entry
#define a_magic	ex_o.magic	/* Will be [ONZ]MAGIC */

#endif	mips

#if	defined(sun3) || defined(sun4) || defined(__hp_osf)
/* Sun machine types */

#define M_OLDSUN2	0	/* old sun-2 executable files */
#define M_68010		1	/* runs on either 68010 or 68020 */
#define M_68020		2	/* runs only on 68020 */
#endif	defined(sun3) || defined(sun4) || defined(__hp_osf)

#endif	_SYS_EXEC_H_
