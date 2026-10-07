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
static char rcsid[] = "@(#) $RCSfile: acctcon2.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:05 $";
#endif

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
#if !lint
static char sccsid[] = "acctcon2.c	1.3  com/cmd/acct,3.1,8943 10/24/89 10:50:10";
#endif

/*
 *	acctcon2 <ctmp >ctacct
 *	reads std. input (ctmp.h/ascii format)
 *	converts to tacct.h form, writes to std. output
 */

#include <sys/types.h>
#include "acctdef.h"
#include <sys/acct.h>
#include <stdio.h>
#include "ctmp.h"
#include "tacct.h"

struct	ctmp	cb;
struct	tacct	tb;

main(int argc, char **argv)
{
	unsigned long devtemp;

	tb.ta_sc = 1;
	while (scanf("%lu\t%lu\t%s\t%lu\t%lu\t%lu\t%*[^\n]",
		&devtemp,
		&cb.ct_uid,
		cb.ct_name,
		&cb.ct_con[0],
		&cb.ct_con[1],
		&cb.ct_start) != EOF) {

		cb.ct_tty = (dev_t)devtemp;
		tb.ta_uid = cb.ct_uid;
		(void)CPYN(tb.ta_name, cb.ct_name);
		tb.ta_con[0] = MINS(cb.ct_con[0]);
		tb.ta_con[1] = MINS(cb.ct_con[1]);
		(void)fwrite(&tb, sizeof(tb), 1, stdout);
	}
	return(0);
}
