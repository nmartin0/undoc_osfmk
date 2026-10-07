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
 * $Log: stab.h,v $
 * Revision 1.3  1994/11/18  21:10:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:58  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:48  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:31  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:01  nandy
 * Initial revision
 *
 * Revision 2.4  92/04/13  14:39:12  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.4  90/10/07  20:36:40  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:59:10  gm]
 * 
 * Revision 2.3  90/08/09  14:28:24  devrcs
 * 	Restored from backup
 * 	[90/07/27  14:14:09  pam]
 * 
 * 	Removed stray comment terminator.
 * 	[90/07/18  08:46:29  kim]
 * 
 * 	Added N_VFP (virtual frame pointer) and N_RMASK (register mask). These
 * 	stabs fill in for the procedure descriptor table in the Third-Eye
 * 	(MIPS) symbol table format.
 * 	[90/07/14  13:59:06  boot]
 * 
 * 	Added N_VFP (virtual frame pointer) and N_RMASK (register mask). These
 * 	stabs fill in for the procedure descriptor table in the Third-Eye
 * 	(MIPS) symbol table format.
 * 	[90/07/14  13:59:06  boot]
 * 
 * Revision 2.2  90/01/02  19:15:48  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)stab.h	5.1 (Berkeley) 5/30/85
 */

/* IF YOU ADD DEFINITIONS, ADD THEM TO nm.c as well */
/*
 * This file gives definitions supplementing <a.out.h>
 * for permanent symbol table entries.
 * These must have one of the N_STAB bits on,
 * and are subject to relocation according to the masks in <a.out.h>.
 */
/*
 * for symbolic debugger, sdb(1):
 */
#define	N_GSYM	0x20		/* global symbol: name,,0,type,0 */
#define	N_FNAME	0x22		/* procedure name (f77 kludge): name,,0 */
#define	N_FUN	0x24		/* procedure: name,,0,linenumber,address */
#define	N_STSYM	0x26		/* static symbol: name,,0,type,address */
#define	N_LCSYM	0x28		/* .lcomm symbol: name,,0,type,address */
#define	N_RSYM	0x40		/* register sym: name,,0,type,register */
#define	N_SLINE	0x44		/* src line: 0,,0,linenumber,address */
#define	N_SSYM	0x60		/* structure elt: name,,0,type,struct_offset */
#define	N_SO	0x64		/* source file name: name,,0,0,address */
#define	N_LSYM	0x80		/* local sym: name,,0,type,offset */
#define	N_SOL	0x84		/* #included file name: name,,0,0,address */
#define	N_PSYM	0xa0		/* parameter: name,,0,type,offset */
#define	N_ENTRY	0xa4		/* alternate entry: name,linenumber,address */
#define	N_LBRAC	0xc0		/* left bracket: 0,,0,nesting level,address */
#define	N_RBRAC	0xe0		/* right bracket: 0,,0,nesting level,address */
#define	N_BCOMM	0xe2		/* begin common: name,, */
#define	N_ECOMM	0xe4		/* end common: name,, */
#define	N_ECOML	0xe8		/* end common (local name): ,,address */
#define	N_LENG	0xfe		/* second stab entry with length information */

/*
 * New stabs for Mach-O/ROSE  format (used by debugger if found)
 */
#define N_RMASK 0xc4            /* register mask,0,address */
#define N_VFP   0xd4            /* virtual frame pointer,,,address */
/*
 * for the berkeley pascal compiler, pc(1):
 */
#define	N_PC	0x30		/* global pascal symbol: name,,0,subtype,line */
