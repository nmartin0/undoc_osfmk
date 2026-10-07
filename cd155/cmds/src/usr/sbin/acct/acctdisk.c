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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: acctdisk.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:07 $";
#endif

/*
 * COMPONENT_NAME: (CMDACCT) Command Accounting
 *
 * FUNCTIONS: 
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
#if !lint
static char sccsid[] = "acctdisk.c	1.3  com/cmd/acct,3.1,8943 10/24/89 10:50:54";
#endif

/*
 *      acctdisk <dtmp >dacct
 *	reads std.input & converts to tacct.h format, writes to output
 *	input:
 *	uid	name	#blocks
 */

#include <sys/types.h>
#include "acctdef.h"
#include <sys/acct.h>
#include <stdio.h>
#include "tacct.h"

struct	tacct	tb;
char	ntmp[NSZ+1];

main(argc, argv)
char **argv;
{
	tb.ta_dc = 1;
	while(scanf("%lu \t%s \t%le",
		&tb.ta_uid,
		ntmp,
		&tb.ta_du) != EOF) {

		(void)CPYN(tb.ta_name, ntmp);
		(void)fwrite(&tb, sizeof(tb), 1, stdout);
	}
	return(0);
}
