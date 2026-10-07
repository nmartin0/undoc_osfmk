/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  , 1994, 1995 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mio/booters/net/coff.h,v 0.4 1994/11/18 20:45:29 mtm Exp $
 *
 * $Log: coff.h,v $
 * Revision 0.4  1994/11/18  20:45:29  mtm
 * Copyright additions/changes
 *
 * Revision 0.3  1993/06/30  22:40:02  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.2  1993/04/27  20:32:00  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 0.1.6.2  1993/04/22  18:31:46  dleslie
 * First R1_0 release
 *
 * Revision 0.1  1992/07/09  16:58:50  andyp
 * Initial checkin of Paragon files.
 *
 * Revision 1.2  92/03/03  09:05:25  shala
 * Add Header and Log markers.
 * 
 *
 */
/*
 *	coff.h
 *
 *	Data structures for loading coff files
 *
 * HISTORY
 *
 * Don Cameron 10/90	
 *	Added STYP_BSS.
 *	Added I860MAGIC.
 *
 * $Log: coff.h,v $
 * Revision 0.4  1994/11/18  20:45:29  mtm
 * Copyright additions/changes
 *
 * Revision 0.3  1993/06/30  22:40:02  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.2  1993/04/27  20:32:00  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 0.1.6.2  1993/04/22  18:31:46  dleslie
 * First R1_0 release
 *
 * Revision 0.1  1992/07/09  16:58:50  andyp
 * Initial checkin of Paragon files.
 *
 * Revision 1.2  92/03/03  09:05:25  shala
 * Add Header and Log markers.
 * 
 * Revision 1.1  92/02/27  16:10:27  shala
 * Initial revision
 * 
 * Revision 2.1.1.1  90/02/09  17:43:37  rvb
 * 	Put into rcs tree
 * 	[90/02/09            rvb]
 * 
 */

struct filehdr {
	unsigned short	f_magic;	/* magic number */
	unsigned short	f_nscns;	/* number of sections */
	long		f_timdat;	/* time & date stamp */
	long		f_symptr;	/* file pointer to symtab */
	long		f_nsyms;	/* number of symtab entries */
	unsigned short	f_opthdr;	/* sizeof(optional hdr) */
	unsigned short	f_flags;	/* flags */
};

#define  I386MAGIC	0514
#define  I860MAGIC	0515

struct aouthdr {
	short	magic;		/* see magic.h				*/
	short	vstamp;		/* version stamp			*/
	long	tsize;		/* text size in bytes			*/
	long	dsize;		/* initialized data "  "		*/
	long	bsize;		/* uninitialized data "   "		*/
	long	entry;		/* entry pt.				*/
	long	text_start;	/* base of text used for this file	*/
	long	data_start;	/* base of data used for this file	*/
};


struct scnhdr {
	char		s_name[8];	/* section name */
	long		s_paddr;	/* physical address, aliased s_nlib */
	long		s_vaddr;	/* virtual address */
	long		s_size;		/* section size */
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation */
	long		s_lnnoptr;	/* file ptr to line numbers */
	unsigned short	s_nreloc;	/* number of relocation entries */
	unsigned short	s_nlnno;	/* number of line number entries */
	long		s_flags;	/* flags */
};

#define STYP_TEXT	0x20		/* section contains text only */
#define STYP_DATA	0x40		/* section contains data only */
#define STYP_BSS	0x80		/* section contains uninitialized
					   data only */

typedef union {				/* Header of Mach executable file */
	struct {			/* COFF header */
		struct filehdr	mainhdr;
		struct aouthdr	opthdr;
	} coff;
} ldf_hdr_t;

