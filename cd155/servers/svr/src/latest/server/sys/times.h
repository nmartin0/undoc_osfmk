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
/* @(#)times.h	1.6  com/inc/sys,3.1,8943 3/7/89 11:27:08 */

/*
 * COMPONENT_NAME: times.h
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
 */                                                                   
/*
 * HISTORY
 * $Log: times.h,v $
 * Revision 1.4  1994/11/18  20:41:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:29:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:48  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:26  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:26  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:14:44  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:46:15  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:55:22  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:47:05  gm]
 * 
 * Revision 1.3  90/03/13  20:50:07  mbrown
 * 	AIX merge first cut.
 * 	[90/02/09  18:05:34  tom]
 * 
 * Revision 1.2  90/01/02  20:16:07  gm
 * 	     Fixes for first snapshot.
 * 
 * $EndLog$
 */

#ifndef _SYS_TIMES_H_
#define _SYS_TIMES_H_

#include <standards.h>
#include <sys/types.h>			/* for clock_t */


#ifdef _POSIX_SOURCE

/*
 * POSIX requires that certain values be included in times.h.  It also
 * requires that when _POSIX_SOURCE is defined only those standard
 * specific values are present.  This header includes all the POSIX
 * required entries.
 */

/*
 * Structure returned by times()
 */
struct tms {
	clock_t	tms_utime;		/* user time */
	clock_t	tms_stime;		/* system time */
	clock_t	tms_cutime;		/* user time, children */
	clock_t	tms_cstime;		/* system time, children */
};

#ifdef _NO_PROTO
extern clock_t times();
#else
extern clock_t times(struct tms *);
#endif /* _NO_PROTO */

#endif /* _POSIX_SOURCE */
#endif	/* _SYS_TIMES_H_ */
