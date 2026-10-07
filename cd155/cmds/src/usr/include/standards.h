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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/* @(#)standards.h	1.4  com/inc,3.1,8943 3/4/89 15:26:54 */
/*
 * COMPONENT_NAME: standards.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   
#ifndef _STANDARDS_H_
#define _STANDARDS_H_

#ifdef _POSIX_SOURCE
#define _ANSI_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#define _POSIX_SOURCE
#define _ANSI_C_SOURCE
#endif

#ifdef _AES_SOURCE
#define _XOPEN_SOURCE
#define _POSIX_SOURCE
#define _ANSI_C_SOURCE
#endif

#if (!defined (_XOPEN_SOURCE)) &&  (!defined (_POSIX_SOURCE)) && (!defined (_ANSI_C_SOURCE)) && (!defined (_AES_SOURCE))
#define _AES_SOURCE
#define _XOPEN_SOURCE
#define _POSIX_SOURCE
#define _ANSI_C_SOURCE
#define _OSF_SOURCE
#endif

/* automation for non ANSI compilers */
#if !__STDC__
#ifndef _NO_PROTO
#define _NO_PROTO
#endif
#ifndef _NONSTD_TYPES
#define _NONSTD_TYPES
#endif
#endif

#ifdef _NO_PROTO
#define __(args)	()
#else /* _NO_PROTO */
#define __(args)	args
#endif /* _NO_PROTO */

#endif /* _STANDARDS_H_ */
