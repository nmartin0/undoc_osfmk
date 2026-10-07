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
 * $Log: machlimits.h,v $
 * Revision 1.3  1994/11/18  21:11:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:18:18  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:01:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:20:05  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:28:49  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:28:47  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/31  15:15:27  devrcs
 * 	Get rid of CLOCKS_PER_SEC which now properly comes from
 * 	machtime.h (in kernel/i386/...)
 * 	[90/10/23  12:00:21  lehotsky]
 * 
 * 	Make *_MIN values properly signed quantities.
 * 	[90/10/18  13:59:48  lehotsky]
 * 
 * Revision 1.3  90/10/07  20:28:07  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:46:23  gm]
 * 
 * Revision 1.2  90/06/22  21:51:45  devrcs
 * 	Addition for SS4
 * 	[90/06/14  22:42:30  kevins]
 * 
 * Revision 1.3  90/03/13  20:48:40  mbrown
 * 	AIX merge first cut.
 * 	[90/02/13  16:15:08  tom]
 * 
 * Revision 1.2  90/01/03  11:57:15  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:31:58  gm]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)machlimits.h	7.1 (Berkeley) 2/15/89
 */
#ifndef _MACH_MACHLIMITS_H_
#define _MACH_MACHLIMITS_H_

#define	CHAR_BIT	8		/* number of bits in a char */

#define	SCHAR_MIN	-128		/* max value for a signed char */
#define	SCHAR_MAX	0x7f		/* min value for a signed char */

#define	UCHAR_MAX	0xff		/* max value for an unsigned char */
#define	CHAR_MAX	0x7f		/* max value for a char */
#define	CHAR_MIN	-128		/* min value for a char */

#define	USHRT_MAX	0xffff		/* max value for an unsigned short */
#define	SHRT_MAX	0x7fff		/* max value for a short */
#define	SHRT_MIN	-32768		/* min value for a short */

#define	UINT_MAX	0xffffffff	/* max value for an unsigned int */
#define	INT_MAX		0x7fffffff	/* max value for an int */
#define	INT_MIN		0x80000000	/* min value for an int */

#define	ULONG_MAX	0xffffffff	/* max value for an unsigned long */
#define	LONG_MAX	0x7fffffff	/* max value for a long */
#define	LONG_MIN	0x80000000	/* min value for a long */

/* Must be at least two, for internationalization (NLS/KJI) */
#define MB_LEN_MAX	2		/* multibyte characters */

#endif /* _MACH_MACHLIMITS_H_ */

