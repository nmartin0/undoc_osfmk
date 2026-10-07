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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: aouthdr.h,v $
 * Revision 1.6  1994/11/18  20:42:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/02/01  21:21:11  jlitvin
 * Whoops!  I accidently added the lint warning I removed to the log message!
 *
 * Revision 1.4  1994/02/01  00:59:21  jlitvin
 * Remove lint warnings about "\* encountered inside a comment."
 *
 * Revision 1.3  1993/07/14  18:31:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:41:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:46:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:38:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:44  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:17:22  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/07/26  16:22:53  jose
 *       Unchanged copy from 1.0.2
 * 
 * Revision 1.3  90/10/07  14:57:37  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:50:42  gm]
 * 
 * Revision 1.2  90/01/02  20:16:37  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:49:03  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.5  89/05/30  13:28:39  rvb
 * 	Added Mips defs.
 * 	[89/04/20            af]
 * 
 * Revision 2.4  89/03/09  22:11:28  rpd
 * 	More cleanup.
 * 
 * Revision 2.3  89/02/25  20:09:35  gm0w
 * 	Get rid of u3b definitions.
 * 	[89/02/07            dlb]
 * 
 * $EndLog$
 */
/*	5.2 SID aouthdr.h	2.4 12/15/82	*/

typedef	struct aouthdr {
	short	magic;		/* see magic.h				*/
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

/*#define U_AL		0x07	\* section alignment mask		*/
/*#define U_AL_NONE	0x00	\* fullword alignment			*/
/*#define U_AL_512	0x01	\* 512 byte alignment			*/
#define U_AL_1024	0x02	/* 1K alignment				*/
/*#define U_AL_2048	0x03	\* 2K alignment				*/
#define U_AL_4096	0x04	/* 4K alignment				*/
/*#define U_AL_8192	0x05	\* 8K alignment				*/
/*#define U_PR		0x38	\* section proctections			*/
/*#define U_PR_DATA	0x08	\* data section				*/
/*#define U_PR_TEXT	0x10	\* text section				*/
/*#define U_PR_MOD	0x20	\* module section			*/
#define U_SYS_5		0x100	/* UMAX System V rel 2			*/
#define U_SYS_42	0x200	/* UMAX System 4.2 BSD			*/
#endif
