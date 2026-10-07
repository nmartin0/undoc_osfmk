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
 * Revision 1.6  1994/11/18  20:31:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  17:59:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:18:19  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:09:42  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:29:57  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.3  1993/04/09  17:30:28  shala
 * MB_LEN_MAX should be 4 instead of 2. This is related to the change for wchar_t
 * in sys/types.h from short to long.
 *
 * Revision 1.2  1992/11/30  22:21:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/11/13  12:21:39  rabii
 * 	Initial checkin
 *
 * Revision 2.2  91/08/31  13:35:05  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:57:12  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6.4.2  91/03/11  13:12:56  lehotsky
 * 	Corrections for ANSI.  Fix representation of unsigned and signed
 * 	values to avoid ANSI automatic conversions.
 * 	[91/03/07  15:40:00  lehotsky]
 * 
 * Revision 1.6  90/10/31  13:56:39  devrcs
 * 	Moved CLOCKS_PER_SEC to machtime.h.
 * 	[90/10/06  11:57:35  rabin]
 * 
 * Revision 1.5  90/10/07  13:42:20  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:36:38  gm]
 * 
 * Revision 1.4  90/09/13  11:44:42  devrcs
 * 	Fix "min values" to be signed, to keep VSX happy.
 * 	[90/08/29  20:44:39  lehotsky]
 * 
 * 	Change CLOCKS_PER_SEC to match that defined in param.c.
 * 	[90/08/21  10:27:11  brezak]
 * 
 * Revision 1.3  90/07/17  11:31:15  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:24:27  kevins]
 * 
 * Revision 1.2  90/04/27  19:07:19  devrcs
 * 	First placed in i386
 * 	[90/04/18  17:13:04  jd]
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

#define	SCHAR_MAX	127		/* max value for a signed char */
#define	SCHAR_MIN	-128		/* min value for a signed char */

#define	UCHAR_MAX	255		/* max value for an unsigned char */
#define	CHAR_MAX	127		/* max value for a char */
#define	CHAR_MIN	-128		/* min value for a char */

#define	USHRT_MAX	65535		/* max value for an unsigned short */
#define	SHRT_MAX	32767		/* max value for a short */
#define	SHRT_MIN	-32768		/* min value for a short */

#define	UINT_MAX	0xFFFFFFFF	/* max value for an unsigned int */
#define	INT_MAX		2147483647	/* max value for an int */
#define	INT_MIN		(-2147483647-1)	/* min value for an int */

#define	ULONG_MAX	UINT_MAX	/* max value for an unsigned long */
#define	LONG_MAX	INT_MAX		/* max value for a long */
#define	LONG_MIN	INT_MIN		/* min value for a long */


/* Must be at least two, for internationalization (NLS/KJI) */
#define MB_LEN_MAX	4		/* multibyte characters */

#endif /* _MACH_MACHLIMITS_H_ */

