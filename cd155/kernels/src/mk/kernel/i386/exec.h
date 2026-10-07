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
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: exec.h,v $
 * Revision 1.5  1994/11/18  20:32:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:23:36  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:22:14  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:21:49  dleslie
 * First R1_0 release
 *
 * Revision 2.4  91/05/14  16:07:10  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:11:34  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:31:52  mrt]
 * 
 * Revision 2.2  90/05/03  15:25:11  dbg
 * 	Adapted for i386 a.out format.
 * 	[90/02/15            dbg]
 * 
 * 12-Sep-86  Jonathan J. Chew (jjc) at Carnegie-Mellon University
 *	Changed first long word to two shorts for the Sun to give
 *	the machine type along with the magic number.
 *	Added machine types for Sun.
 *	Made file includable more than once.
 *
 */
/*
 * exec stucture in an a.out file derived from FSF's
 * a.out.gnu.h file.
 */

#ifndef	_EXEC_
#define	_EXEC_	1

/*
 * Header prepended to each a.out file.
 */
struct exec
{
#ifdef	sun
    unsigned short  a_machtype; /* machine type */
    unsigned short  a_info;	/* Use macros N_MAGIC, etc for access */
#else	sun
    unsigned long   a_info;	/* Use macros N_MAGIC, etc for access */
#endif	sun
  unsigned long a_text;		/* length of text, in bytes */
  unsigned long a_data;		/* length of data, in bytes */
  unsigned long a_bss;		/* length of uninitialized data area for file, in bytes */
  unsigned long a_syms;		/* length of symbol table data in file, in bytes */
  unsigned long a_entry;	/* start address */
  unsigned long a_trsize;	/* length of relocation info for text, in bytes */
  unsigned long a_drsize;	/* length of relocation info for data, in bytes */
};

/* Code indicating object file or impure executable.  */
#define OMAGIC 0407
/* Code indicating pure executable.  */
#define NMAGIC 0410
/* Code indicating demand-paged executable.  */
#define ZMAGIC 0413

#ifdef sun
/* Sun machine types */

#define M_OLDSUN2	0	/* old sun-2 executable files */
#define M_68010		1	/* runs on either 68010 or 68020 */
#define M_68020		2	/* runs only on 68020 */
#endif sun

#endif	_EXEC_
