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
 * @(#) $RCSfile: tacct.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:30 $
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
/* tacct.h	1.2  com/cmd/acct,3.1,8943 10/24/89 11:00:40 */

/*
 *	total accounting (for acct period), also for day
 */

/* Float arrays below contain prime and non-prime components */
/* NSZ is define in acctdef.h as (sizeof(((struct utmp *)0)->ut_user)) */
struct	tacct	{
	uid_t		ta_uid;		/* userid */
	char		ta_name[NSZ];	/* login name */
	double          ta_cpu[2];      /* cum. cpu time (mins) */
	double          ta_kcore[2];    /* cum kcore-mins */
	double          ta_io[2];       /* cum. chars xferred (512s) */
	double          ta_rw[2];       /* cum. blocks read/written */
	double          ta_con[2];      /* cum. connect time (mins) */
	double		ta_du;		/* cum. disk usage */
	long            ta_qsys;        /* queueing sys charges (pgs) */
	double		ta_fee; 	/* fee for special services */
	long		ta_pc;		/* count of processes */
	unsigned short	ta_sc;		/* count of login sessions */
	unsigned short	ta_dc;		/* count of disk samples */
};
