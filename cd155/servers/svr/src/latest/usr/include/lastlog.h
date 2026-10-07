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
 * $Log: lastlog.h,v $
 * Revision 1.3  1994/11/18  21:09:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:10  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:51  nandy
 * Initial revision
 *
 * Revision 2.4  92/04/13  14:36:01  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.4  90/10/07  20:28:24  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:46:49  gm]
 * 
 * Revision 2.3  90/03/13  21:22:31  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  17:57:41  tom]
 * 
 * Revision 2.2  90/01/02  19:15:18  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 * 
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)lastlog.h	1.1  com/inc,3.1,8943 7/21/89 11:31:41 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	lastlog.h	5.1 (Berkeley) 5/30/85
 */

#ifndef _LASTLOG_H_
#define _LASTLOG_H_

/* for lastlog structure */
#define LL_LINELEN      8               /* terminal path length */
#define LL_HOSTLEN      16              /* hostname length */

struct lastlog {
	time_t	ll_time;
	char	ll_line[LL_LINELEN];
	char	ll_host[LL_HOSTLEN];		/* same as in utmp */
};

#endif  /* _LASTLOG_H_ */
