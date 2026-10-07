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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: extras.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:30 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: wait_csh memcpyc
 *
 * ORIGINS: 10,26,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

#include <sys/types.h>
#include <sys/times.h>
#include <wait.h>
#include <sys/vtimes.h>

wait_csh(wp, flags, vtimep)
register struct vtimes *vtimep;
register union wait *wp;
{
	register short pid;
	struct tms	before, after;
	int wstatus;

	times(&before);
	pid = wait(&wstatus);
	if (pid <= 0)
		return(pid);
	if ((wstatus&0377) == 0177) {			/* child stopped */
		wp->w_stopval = WSTOPPED;
		wp->w_stopsig = (wstatus>>8)&0377;
	} else if ((wstatus&0377) == 0) {		/* child exited */
		wp->w_termsig = 0;
		wp->w_coredump = 0;
		wp->w_retcode = (wstatus>>8)&0377;
	} else {					/* child killed */
		wp->w_retcode = 0;
		if (wstatus & 0200)
			wp->w_coredump = 1;
		else
			wp->w_coredump = 0;
		wp->w_termsig = (wstatus&0177);
	}
	times(&after);
	if (vtimep == (struct vtimes *)0)
		return(pid);

	/*
	 * I don't really know if this is correct.  
	 * But I sleep better thinking that I 
	 * did something with it.
	 * Watch out for this!
	 */
	vtimep->vm_utime	= after.tms_cutime - before.tms_cutime;
	vtimep->vm_stime	= after.tms_cstime - before.tms_cstime;
	vtimep->vm_idsrss	= 0;
	vtimep->vm_ixrss	= 0;
	vtimep->vm_maxrss	= 0;
	vtimep->vm_majflt	= 0;
	vtimep->vm_minflt	= 0;
	vtimep->vm_nswap	= 0;
	vtimep->vm_inblk	= 0;
	vtimep->vm_oublk	= 0;
	return(pid);
}

uchar_t *
memcpyc(s1, s2, n)
register uchar_t *s1, *s2;
register int n;
{
	register uchar_t *os1 = s1;

	while (--n >= 0)
		*s1++ = *s2++;
	return (os1);
}

