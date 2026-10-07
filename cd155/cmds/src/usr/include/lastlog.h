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
