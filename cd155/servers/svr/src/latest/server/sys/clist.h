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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: clist.h,v $
 * Revision 1.4  1994/11/18  20:40:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:23:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:45  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:21  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:10  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:47  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:02:54  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:42:05  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:49:19  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:07:57  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:37:29  gm]
 * 
 * Revision 1.4  90/02/23  00:32:23  devrcs
 * 	Switch order of c_info and c_quote fields in struct cblock, so
 * 	that they are now the same as the original 4.4bsd version.
 * 	[90/02/20  21:51:22  ers]
 * 
 * Revision 1.3  90/02/16  16:51:59  devrcs
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:02:49  ers]
 * 
 * Revision 1.2  90/01/02  20:15:39  morris
 * 	     Posix tty support: added c_quote field to cblock structure.
 * 
 * Revision 1.1  89/10/16  19:47:56  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:15:27  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.4  89/03/09  22:02:42  rpd
 * 	More cleanup.
 * 
 * Revision 2.3  89/02/25  17:52:17  gm0w
 * 	Got rid of conditional on CMUCS and the code that
 * 	was defined for non-CMUCS case
 * 	[89/02/13            mrt]
 * 
 * Revision 2.2  88/10/11  10:23:32  rpd
 * 	Made declarations be extern.
 * 	[88/10/06  07:55:03  rpd]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)clist.h	7.2 (Berkeley) 10/18/88
 */

#ifndef	_SYS_CLIST_H_
#define _SYS_CLIST_H_

#include <sys/param.h>		/* for CBSIZE */

/*
 * Raw structures for the character list routines.
 */
struct cblock {
	struct cblock *c_next;
	char	c_quote[CBQSIZE];
	char	c_info[CBSIZE];
};

#ifdef	KERNEL

extern struct cblock *cfree;
extern int nclist;
extern struct cblock *cfreelist;
extern int cfreecount;

#endif	KERNEL
#endif	_SYS_CLIST_H_
