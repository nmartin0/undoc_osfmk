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
static char rcsid[] = "@(#)$RCSfile: psignal.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:24 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: psignal
 *
 * ORIGINS: 26 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * psignal.c	1.6  com/lib/c/gen,3.1,8943 10/16/89 08:52:42
 */

#include <signal.h>

/*
 * FUNCTION: Print the name of the signal indicated along with the supplied message.
 *
 */
/*LINTLIBRARY*/


extern	char *sys_siglist[];

#ifdef MSG
#include "libc_msg.h"
#endif

psignal(sig, s)
	unsigned sig;
	char *s;
{
	register char *c;
	register n;

#ifdef MSG
	if (sig < SIGMAX+1)
		c = NLgetamsg( MF_LIBC, MS_LIBC, (int)(M_PSIGNAL + sig),
			sys_siglist[sig] );
	else
		c = NLgetamsg( MF_LIBC, MS_LIBC, M_PSIGNAL + NSIG,
		 	"Unknown signal" );
#else
	if (sig < SIGMAX+1)
		c = sys_siglist[sig];
	else
		c = "Unknown signal";
#endif
	n = strlen(s);
	if (n) {
		write(2, s, n);
		write(2, ": ", 2);
	}
	write(2, c, strlen(c));
	write(2, "\n", 1);
}
