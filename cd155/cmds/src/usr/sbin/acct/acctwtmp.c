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
static char rcsid[] = "@(#) $RCSfile: acctwtmp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:12 $";
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
static char sccsid[] = "acctwtmp.c	1.2  com/cmd/acct,3.1,8943 10/24/89 10:53:37";
#endif

/*
 *      acctwtmp reason >> /var/adm/wtmp
 *	writes utmp.h record (with current time) to end of std. output
 *      acctwtmp `uname` >> /var/adm/wtmp as part of startup
 *      acctwtmp pm >> /var/adm/wtmp  (taken down for pm, for example)
 */

#include <stdio.h>
#include "acctdef.h"
#include <sys/acct.h>
#include <sys/types.h>

#include <locale.h>
#include "acct_msg.h"
#define	MSGSTR(Num, Str)	NLgetamsg(MF_ACCT, MS_ACCT, Num, Str)


static struct	utmp	wb;

main(int argc, char **argv)
{
	int i;

	(void) setlocale (LC_ALL,"");
	if(argc < 2) {
		(void)fprintf(stderr,MSGSTR( USAGEWTMP, 
			"Usage: %s 'Reason'\n"), argv[0]);
		exit(1);
	}

	(void)strncpy(wb.ut_line, argv[1], LSZ);
	for( i = LSZ-1; i >= 0 && wb.ut_line[i] == ' '; i-- )
		wb.ut_line[i] = '\0';
	wb.ut_type = ACCOUNTING;
	(void)time(&wb.ut_time);
	(void)fseek(stdout, 0L, 2);
	(void)fwrite(&wb, sizeof(wb), 1, stdout);

	return(0);
}
