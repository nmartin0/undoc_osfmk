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
 * OSF/1 Release 1.0.2
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)ulimit.h	1.9  com/inc,3.1,8943 9/15/89 10:02:22 */
/*
 * COMPONENT_NAME: (LIBCSYS) Standard C Library System Functions
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _ULIMIT_H_
#define _ULIMIT_H_

#include <standards.h>

#ifdef _XOPEN_SOURCE

#ifdef _NO_PROTO
extern long ulimit();
#else
extern long ulimit(int, ...);
#endif

#define UL_GETFSIZE	1	/* get max file size limit */
#define UL_SETFSIZE	2	/* set max file size limit */

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE
#define UL_GETBREAK	3	/* get max break (data space) limit */
#define UL_GETNOFILES	4	/* get max break (data space) limit */
#endif /* _OSF_SOURCE */


#endif /* _ULIMIT_H_ */ 
