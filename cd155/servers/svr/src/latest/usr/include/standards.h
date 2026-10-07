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
 * $Log: standards.h,v $
 * Revision 1.3  1994/11/18  21:10:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:50  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:34  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:04  nandy
 * Initial revision
 *
 * Revision 1.7  92/04/13  14:39:16  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.7  90/10/07  20:36:44  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:59:16  gm]
 * 
 * Revision 1.6  90/09/23  16:43:31  devrcs
 * 	added support for _AES_SOURCE
 * 	[90/09/07  14:07:08  rabin]
 * 
 * Revision 1.5  90/06/29  14:14:04  devrcs
 * 	Only do non-__STDC__ automation if not already defined.
 * 	[90/06/21            gm]
 * 
 * Revision 1.4  90/06/22  21:52:26  devrcs
 * 	change to if NOT__STDC__
 * 	[90/05/24  10:10:33  mbrown]
 * 
 * Revision 1.3  90/05/24  23:08:39  devrcs
 * 	added automation for non-__STDC__
 * 	[90/05/18  11:59:35  mbrown]
 * 
 * Revision 1.2  90/03/13  21:23:29  mbrown
 * 	Added __ macro to hide prototyping.
 * 	[90/02/13  18:31:54  tom]
 * 
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:36:58  tom]
 * 
 * $EndLog$
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
