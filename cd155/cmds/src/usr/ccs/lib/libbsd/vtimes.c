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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)vtimes.c	1.2  com/lib/bsd,3.1,8943 9/7/89 16:13:19";
/*
 * COMPONENT_NAME: (LIBBSD)  Berkeley Compatibility Library
 *
 * FUNCTIONS: vtimes
 *
 * ORIGINS: 26 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/vtimes.h>

/*
 * Backwards compatible vtimes.
 */
#define SYSHZ	100		/* System clock tick rate */
#define WALLHZ	60		/* Wall power line frequency */

static int getvtimes(), scale60() ;

/*
 * NAME: vtimes
 * FUNCTION: returns accounting information for the current process and for the 
 *  terminated child  processes of the current process.
 */
vtimes(par, chi)
	register struct vtimes *par, *chi;
{
	struct rusage ru;

	if (par) {
		if (getrusage(RUSAGE_SELF, &ru) < 0)
			return (-1);
		getvtimes(&ru, par);
	}
	if (chi) {
		if (getrusage(RUSAGE_CHILDREN, &ru) < 0)
			return (-1);
		getvtimes(&ru, chi);
	}
	return (0);
}

static
getvtimes(aru, avt)
	register struct rusage *aru;
	register struct vtimes *avt;
{

	avt->vm_utime = scale60(&aru->ru_utime);
	avt->vm_stime = scale60(&aru->ru_stime);
	avt->vm_idsrss = ((aru->ru_idrss+aru->ru_isrss) / SYSHZ) * WALLHZ;
	avt->vm_ixrss = aru->ru_ixrss / SYSHZ * WALLHZ;
	avt->vm_maxrss = aru->ru_maxrss;
	avt->vm_majflt = aru->ru_majflt;
	avt->vm_minflt = aru->ru_minflt;
	avt->vm_nswap = aru->ru_nswap;
	avt->vm_inblk = aru->ru_inblock;
	avt->vm_oublk = aru->ru_oublock;
}

static
scale60(tvp)
	register struct timeval *tvp;
{

	return (tvp->tv_sec * WALLHZ + (tvp->tv_usec * 1000000) / WALLHZ);
}
