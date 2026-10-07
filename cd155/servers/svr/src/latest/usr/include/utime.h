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
 * $Log: utime.h,v $
 * Revision 1.3  1994/11/18  21:11:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:46  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:40  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:42  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:57  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:40:16  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:39:00  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:02:44  gm]
 * 
 * Revision 1.2  90/03/13  21:24:05  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:40:33  tom]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)utime.h	1.4  com/inc,3.1,8943 9/15/89 10:02:46 */
/*
 * COMPONENT_NAME: (CMDFS) commands that deal with the file system
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

#ifndef _UTIME_H_
#define _UTIME_H_

#include <standards.h>
#include <sys/types.h>

/*
 * POSIX requires that certain values be included in utime.h.  It also
 * requires that when _POSIX_SOURCE is defined only those standard
 * specific values are present.  This header includes all the POSIX
 * required entries.
 */

#ifdef _POSIX_SOURCE

struct utimbuf {
	time_t	actime;			/* access time */
	time_t	modtime;		/* modification time */
	};

#ifdef _NO_PROTO
extern int utime();
#else
extern int utime(char *, struct utimbuf *); 
#endif /* _NO_PROTO */

#endif /* _POSIX_SOURCE */
#endif /* _UTIME_H_ */
