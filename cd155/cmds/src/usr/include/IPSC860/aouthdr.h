/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/* @(#)$RCSfile: aouthdr.h,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 02:41:23 $ */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*	5.2 SID aouthdr.h	2.4 12/15/82	*/

#ifndef _AOUTHDR_H
#define _AOUTHDR_H

typedef	struct aouthdr {
	short	magic;		/* see sys/exec.h			*/
	short	vstamp;		/* version stamp			*/
	long	tsize;		/* text size in bytes, padded to FW
				   bdry					*/
	long	dsize;		/* initialized data "  "		*/
	long	bsize;		/* uninitialized data "   "		*/
#if u3b
	long	dum1;
	long	dum2;		/* pad to entry point	*/
#endif
#ifdef	ns32000
	long	msize;		/* size of module table, "modsize"	*/
	long	mod_start;	/* start addr of mod table, "modstart"	*/
#endif
	long	entry;		/* entry point, value of "start"	*/
	long	text_start;	/* base of text used for this file	*/
	long	data_start;	/* base of data used for this file	*/
#ifdef	ns32000
	short	entry_mod;	/* module number of entry point, unused	*/
	unsigned short	flags;	/* section alignment and protection	*/
#endif
#ifdef	mips
	long	bss_start;	/* base of bss used for this file	*/
	long	gprmask;	/* general purpose register mask	*/
	long	cprmask[4];	/* co-processor register masks		*/
	long	gp_value;	/* the gp value used for this object    */
#endif	mips
} AOUTHDR;

#define	AOUTHSZ	sizeof(AOUTHDR)

#ifdef	ns32000
	/* flag word definitions for file positioning of raw data	*/
	/* also for segment protections and Unix system type		*/
	/* only the 1024, 4096 and sys flavors are supported		*/

/*#define U_AL		0x07	   section alignment mask		*/
/*#define U_AL_NONE	0x00	   fullword alignment			*/
/*#define U_AL_512	0x01	   512 byte alignment			*/
#define U_AL_1024	0x02	/* 1K alignment				*/
/*#define U_AL_2048	0x03	   2K alignment				*/
#define U_AL_4096	0x04	/* 4K alignment				*/
/*#define U_AL_8192	0x05	   8K alignment				*/
/*#define U_PR		0x38	   section proctections			*/
/*#define U_PR_DATA	0x08	   data section				*/
/*#define U_PR_TEXT	0x10	   text section				*/
/*#define U_PR_MOD	0x20	   module section			*/
#define U_SYS_5		0x100	/* UMAX System V rel 2			*/
#define U_SYS_42	0x200	/* UMAX System 4.2 BSD			*/
#endif


#endif /* _AOUTHDR_H */
