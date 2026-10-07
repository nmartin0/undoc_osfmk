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
/* addrconf.h -- Definitions for getaddressconf() system call
 *
 * HISTORY
 * $Log: addrconf.h,v $
 * Revision 1.4  1994/11/18  20:39:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:23:31  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:33  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/15  11:42:13  roy
 * Fix revision history comments.
 *
 * Revision 2.2  91/10/14  13:01:14  sjs
 *	Initial revision for this tree.
 * 
 * Revision 3.1  91/10/04  16:40:01  sp
 * Add entries for the emulator areas
 * 
 * Revision 3.0  91/09/13  12:46:56  sp
 * included for mmap
 * 
 * Revision 1.6  90/10/07  14:48:45  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:36:38  gm]
 * 
 * Revision 1.5  90/07/27  09:06:36  devrcs
 * 	Add AC_LDR_PRELOAD in place of unused entry.
 * 	[90/07/17  16:27:21  melanie]
 * 
 * Revision 1.4  90/07/05  23:14:12  devrcs
 * 	No more AC_LIB_{TEXT,DATA,BSS} -- replace with AC_LDR_KPT,
 * 	AC_LDR_GLB, and unused entry.
 * 	[90/07/03  10:02:01  lwa]
 * 
 * Revision 1.3  90/06/22  20:52:23  devrcs
 * 	Simplify config record -- eliminate adjaceny info in favor of
 * 	fixed vs floating flag
 * 	[90/06/04  15:58:57  lwa]
 * 
 * Revision 1.2  90/04/27  19:25:19  devrcs
 * 	Initial coding
 * 	[90/03/05  11:45:02  lwa]
 * 
 * $EndLog$
 */

#ifndef _SYS_ADDRCONF_H
#define _SYS_ADDRCONF_H

/* Address-space configuration record, as returned by getaddressconf() */

struct addressconf {
	caddr_t			ac_base; /* base of region */
	unsigned		ac_flags;
};


/* Values for ac_flags */

#define		AC_UPWARD	0x0	/* area grows upward */
#define		AC_DOWNWARD	0x1	/* area grows downward */
#define		AC_FIXED	0x0	/* fixed base address */
#define		AC_FLOAT	0x2	/* base address floats above prev. region */

/* Known areas */

#define	AC_TEXT		0		/* absolute program text */
#define AC_DATA		1		/* absolute program data */
#define AC_BSS		2		/* absolute program bss */
#define AC_STACK	3		/* stack area */
#define AC_LDR_TEXT	4		/* loader text area */
#define AC_LDR_DATA	5		/* loader data area */
#define AC_LDR_BSS	6		/* loader bss area */
#define AC_LDR_PRIV	7		/* loader private data file -- inherited */
#define AC_LDR_GLB	8		/* loader global data file (KPT & preloads ) */
#define AC_LDR_PRELOAD	9		/* loader preloaded library data */
#define AC_MMAP_TEXT	10		/* mmap'ed file text */
#define AC_MMAP_DATA	11		/* mmap'ed file data */
#define AC_MMAP_BSS	12		/* mmap'ed file bss */

#ifdef	OSF1_SERVER
#define	AC_EMUL		13		/* emulator text and data */
#define	AC_EMUL_MFILE	14		/* emulator mapped file I/O space */

#define AC_N_AREAS	15		/* number of known areas */
#else	/* OSF1_SERVER */
#define AC_N_AREAS	13		/* number of known areas */
#endif	/* OSF1_SERVER */

#ifndef	KERNEL
#ifndef _NO_PROTO

extern int getaddressconf(struct addressconf *buf, size_t size);

#else /* _NO_PROTO */

extern int getaddressconf();

#endif /* _NO_PROTO */

#else /* KERNEL */

extern struct addressconf addressconf[AC_N_AREAS];

#endif /* KERNEL */

#endif /* _SYS_ADDRCONF_H */
