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
/* @(#)$RCSfile: assert.h,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:40:01 $ */
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
#define assert(value) (((value)? (void)0 : __assert(#value,__FILE__,__LINE__)))
#else
#define assert(EX) if (EX) ; else __assert("EX", __FILE__, __LINE__)
#endif /* __STDC__*/

#endif /* NDEBUG */
#endif /* _ANSI_C_SOURCE */
#endif /* _ASSERT_H_ */
