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
 * $Log: limits.h,v $
 * Revision 1.6  1994/11/18  20:40:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/11/09  17:47:02  shala
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Bugs #7090 and #7092.
 *  Testing: Compile a test case.
 *  Module(s): svr/server/sys/limits.h
 *
 * Defined SSIZE_MAX, _POSIX_SSIZE_MAX, _POSIX_STREAM_MAX, _POSIX_TZNAME_MAX,
 * and TZNAME_MAX.
 *
 * Revision 1.4  1993/07/14  18:25:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:19:24  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:42:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:58  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/10/27  16:01:25  cfj
 * Change PID_MAX to UINT_MAX
 *
 * Revision 2.2  1991/08/31  14:07:48  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:43:27  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/31  14:06:38  devrcs
 * 	Add defines for NZERO if you are compiling in the BSD world which works
 * 	with the BSD version of nice()
 * 	[90/10/25  13:00:25  sp]
 * 
 * 	Added #include machtime.h under _OSF_SOURCE, just in case anyone
 * 	was expecting CLOCKS_PER_SEC to be defined in limits.h.
 * 	[90/10/06  11:58:30  rabin]
 * 
 * Revision 1.5  90/10/07  14:51:38  devrcs
 * 	Close the RCSfile comment to shut up pendantic compilers and head off
 * 	future errors.
 * 	[90/10/03  12:41:37  jeffc]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:41:21  gm]
 * 
 * Revision 1.4  90/09/23  15:59:48  devrcs
 * 	added support for _AES_SOURCE
 * 	[90/09/07  14:36:35  rabin]
 * 
 * Revision 1.3  90/08/24  12:26:19  devrcs
 * 	Remove PAGE_SIZE. It cannot be determined at compile time.
 * 	[90/08/10  16:38:36  brezak]
 * 
 * 	Some cleanup:
 * 	  include float.h
 * 	  add PAGE_SIZE, ATEXIT_MAX
 * 	  increase PASS_MAX for AES
 * 	[90/08/10  15:08:12  tom]
 * 
 * Revision 1.2  90/03/13  20:49:31  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/09  15:17:16  tom]
 * 
 * $EndLog$
 */
/* @(#)$RCSfile: limits.h,v $ $Revision: 1.6 $ (OSF) $Date: 1994/11/18 20:40:39 $ */
/*
 * COMPONENT_NAME: limits.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 * 
 * limits.h 1.42  com/inc/sys,3.1,9021 5/18/90 14:35:04
 */                                                                   

#ifndef _SYS_LIMITS_H_
#define _SYS_LIMITS_H_ 
#include <standards.h>

/*
 *
 *      The ANSI and POSIX standards require that certain values be in limits.h.
 *      They also require that if _ANSI_C_SOURCE or _POSIX_SOURCE is 
 *      defined then ONLY those standard specific values are present.
 *      This header includes all the ANSI and POSIX required entries.
 *      In addition other entries for the XIX system are included.
 *
 */
#ifdef _ANSI_C_SOURCE

#include <machine/machlimits.h>


#endif /* _ANSI_C_SOURCE */

/*
 *   The following are values that have historically been in limits.h.
 *
 *   They are a part of the POSIX defined limits.h and therefore are
 *   included when _POSIX_SOURCE is defined.
 *
 */


#ifdef _POSIX_SOURCE


/* limits on a system conforming to the POSIX standard */

#define _POSIX_ARG_MAX		4096
#define _POSIX_CHILD_MAX	6
#define _POSIX_LINK_MAX		8
#define _POSIX_MAX_CANON	255
#define _POSIX_NAME_MAX		14
#define _POSIX_NGROUPS_MAX	0
#define _POSIX_MAX_INPUT	255
#define _POSIX_OPEN_MAX		16
#define _POSIX_PATH_MAX		255
#define _POSIX_PIPE_BUF		512
#define _POSIX_SSIZE_MAX        32767
#define _POSIX_STREAM_MAX       8
#define _POSIX_TZNAME_MAX       3

#define SSIZE_MAX               LONG_MAX
#define TZNAME_MAX              255     /* Doesn't count NUL in string */

/* RUN-TIME INVARIANT VALUES
   The system supports the following fixed magnitude limitations.
*/

#include <sys/syslimits.h>

#endif /* _POSIX_SOURCE */

#ifdef _XOPEN_SOURCE

/* Some of the values in <float.h> are required in limits.h by XPG3, but
 * are not allowed by Posix.1.  The include must remain in _XOPEN_SOURCE.
 * The kernel can't include things not in sys, hence the ifdef.
 */

#ifndef _KERNEL
#include <float.h>      /* DBL_DIG, DBL_MAX, DBL_MIN, */
			/* FLT_DIG, FLT_MAX, FLT_MIN  */
#endif


/* ANSI C requires TMP_MAX to be in stdio.h, however, XOPEN requires it to be
 * in limits.h.  
 */

#ifndef TMP_MAX
#define TMP_MAX		16384		/* The number of unique names 
					   generated by the tmpnam() function */
#endif
					/* max decimal value of a double */
/* defines for message catalog usage */

#define NL_ARGMAX       9               /* max number of printf args */

#define NL_MSGMAX       65535           /* max number of messages per set */

#define NL_SETMAX       65535           /* max number of set per catalog */

#define NL_TEXTMAX      4096            /* max message length in bytes */

#define PASS_MAX	16		/* max number of bytes in a password
					   (not a string length; doesn't include
					   terminating null), >= 8 */
#define NL_LANGMAX	14	/* max number of of bytes in a LANG name */

#define NL_NMAX 	10		/* max number of bytes in N-to-1 mapping
					   characters */
#ifdef _BSD
#define NZERO		0		/* default proc priority (BSD) */
#else
#define NZERO		20		/* default proc priority (SYSV/XOPEN) */
#endif

#define LONG_BIT	32		/* number of bits in a long     */

#define WORD_BIT	32		/* number of bits in a "word" or int */


#endif /* _XOPEN_SOURCE */

#ifdef _AES_SOURCE

#define	ATEXIT_MAX	32

#endif /* _AES_SOURCE */

#ifdef _OSF_SOURCE

#include <machine/machtime.h>		/* to get CLOCKS_PER_SEC */

#define PID_MAX		UINT_MAX	/* max value for a process ID,
					   >=30000	*/ 
#define UID_MAX		USHRT_MAX	/* max value for a user or group ID */

#endif /*_OSF_SOURCE */
#endif /* _SYS_LIMITS_H_ */
