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
 * $Log: assert.h,v $
 * Revision 1.3  1994/11/18  21:09:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:43  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:14:55  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:31  nandy
 * Initial revision
 *
 * Revision 2.7  92/04/13  14:34:34  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.7.4.2  91/01/29  17:04:27  tom
 * 	Remove argument names from function prototype.
 * 	[91/01/29  15:48:20  tom]
 * 
 * Revision 2.7  90/10/31  15:15:10  devrcs
 * 	Renamed _assert() to __assert().
 * 	[90/10/06  16:23:35  rabin]
 * 
 * Revision 2.6  90/10/07  20:26:02  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:43:19  gm]
 * 
 * Revision 2.5  90/08/24  13:52:24  devrcs
 * 	Updated to golden AIX code.
 * 	[90/08/09  16:36:16  tom]
 * 
 * Revision 2.4  90/04/27  23:06:10  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:47:12  tom]
 * 
 * Revision 2.3  90/03/13  21:21:29  mbrown
 * 	fixed assert macro
 * 	[90/03/01  18:44:17  mbrown]
 * 
 * 	AIX merge first cut.
 * 	[90/02/12  17:48:07  tom]
 * 
 * Revision 2.2  90/01/02  19:15:09  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */
/* @(#)$RCSfile: assert.h,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/18 21:09:11 $ */
/*
 * COMPONENT_NAME: (SYSDB) Kernel Debugger
 *
 * FUNCTIONS:
 *
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1990
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * assert.h	1.10  com/inc,3.1,9021 5/11/90 10:03:55 
 */

#ifndef _ASSERT_H_
#define _ASSERT_H_
#include <standards.h>

/*
 *
 *      The ANSI standard requires that certain values be in assert.h.
 *      It also requires that if _ANSI_C_SOURCE is defined then ONLY these
 *      values are present.  This header includes all ANSI required entries.  
 *
 */
#ifdef _ANSI_C_SOURCE

#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else
#ifdef	_NO_PROTO
extern void __assert();
#else
extern void __assert(char *, char *, int);
#endif /* _NO_PROTO */

#if __STDC__ == 1
#define assert(EX) if (##EX) ; else __assert(# EX, __FILE__, __LINE__)
#else
#define assert(EX) if (EX) ; else __assert("EX", __FILE__, __LINE__)
#endif /* __STDC__*/

#endif /* NDEBUG */
#endif /* _ANSI_C_SOURCE */
#endif /* _ASSERT_H_ */
