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
 * @(#) $RCSfile: table.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:30 $
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
/* table.h	1.2  com/cmd/acct,3.1,8943 10/24/89 11:00:24 */

struct table {
	char        **tb_base;      /* Pointer to ptr to space */
	ushort      tb_elsize;      /* Size of one element */
	int         tb_nel;         /* Number of elements */
};

#define INITTABLE(array,nel) \
	{ (char **)&array, sizeof(*array), nel }

extern char *extend();
