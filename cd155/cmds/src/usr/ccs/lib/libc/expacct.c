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
 * expacct.c	1.1  com/cmd/acct/lib,3.1,8935 5/15/89 17:17:04
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: expacct.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:36 $";
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/acct.h>

/*
 *			ATTENTION
 *	This expacct() routine is the "inverse" function of the
 *	compress() routine in kernel/bsd/kern_acct.c, which
 *	converts the kernel values into the pseudo floating
 *	point format.
 *	Changes of the compress() routine affect expacct()!
 */

double
expacct(t)
comp_t t;
{
	register long nt;
	register exp;
	double e;

	nt = t&017777;
	exp = (t >> 13) & 07;
	while (exp-- > 0)
		nt <<= 3;

	e = (double) (nt/AHZ) + (((double)(nt%AHZ))/AHZ);
	return(e);
}
