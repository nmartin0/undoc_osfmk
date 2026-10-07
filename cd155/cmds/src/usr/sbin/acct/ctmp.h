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
 * @(#) $RCSfile: ctmp.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:14 $
 */
/*
 * COMPONENT_NAME: (CMDACCT) Command Accounting
 *
 * FUNCTIONS: none
 *
 * ORIGINS: 3,9,27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/* ctmp.h	1.2  com/cmd/acct,3.1,8943 10/24/89 10:54:43 */

/*
 *	connect time record (various intermediate files)
 *	NSZ is defined in acctdef.h .
 */
struct ctmp {
	dev_t	ct_tty;			/* major minor */
	uid_t	ct_uid;			/* userid */
	char	ct_name[NSZ];		/* login name */
	long	ct_con[2];		/* connect time (p/np) secs */
	time_t	ct_start;		/* session start time */
};
