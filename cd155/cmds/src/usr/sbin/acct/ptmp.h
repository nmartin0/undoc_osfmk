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
 * @(#) $RCSfile: ptmp.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:26 $
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
/* ptmp.h	1.2  com/cmd/acct,3.1,8943 10/24/89 10:58:50 */

/*
 *	per process temporary data
 *	NSZ is defined in acctdef.h .
 */
struct ptmp {
	uid_t	pt_uid;			/* userid */
	char	pt_name[NSZ];		/* login name */
	long	pt_cpu[2];		/* CPU (sys+usr) P/NP time tics */
	long    pt_io;                  /* chars transferred P/NP */
	long    pt_rw;                  /* blocks read/written P/NP */
	unsigned pt_mem;		/* avg. memory size */
};	
