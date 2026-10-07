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
static char rcsid[] = "@(#)$RCSfile: ssignal.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:23 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: ssignal, gsignal
 *
 * ORIGINS: 3 27
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
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * ssignal.c	1.10  com/lib/c/gen,3.1,8943 10/24/89 10:31:08
 */

#include <signal.h>		/* for SIG_DFL */

/* Highest allowable user signal number */
#define MAXSIG 16

/* Lowest allowable signal number (lowest user number is always 1) */
#define MINSIG (-4)

/* Table of signal values */
static void (*sigs[MAXSIG-MINSIG+1])(int);

/*
 * NAME:	ssignal
 *                                                                    
 * FUNCTION:	set software signal
 *                                                                    
 * NOTES:	Ssignal sets a software signal.  The signal can
 *		either be set to SIG_DFL, SIG_IGN or can be set
 *		to call a function when the signal is raised.
 *
 * DATA STRUCTURES:	'sigs' is modified
 *
 * RETURN VALUE DESCRIPTION:	The previous value of the signal
 *		is returned.
 */  

void
(*ssignal(int sig, void (*fn)(int)))(int)
{
	void (*savefn)(int);	/* previous value of 'sig'	*/

	if (sig >= MINSIG && sig <= MAXSIG) {
		savefn = sigs[sig-MINSIG];
		sigs[sig-MINSIG] = fn;
	} else
		savefn = SIG_DFL;

	return(savefn);
}

/*
 * NAME:	gsignal
 *                                                                    
 * FUNCTION:	gsignal - raise a software signal.
 *                                                                    
 * NOTES:	Gsignal 'raises' a software signal.  In effect, the
 *		software signal is then processed.  If the value of
 *		signal is currently a function, the function is called.
 *
 * RETURN VALUE DESCRIPTION:	0 if the value of the function is SIG_DFL,
 *		1 if the value is SIG_IGN, else call the function and
 *		return the value returned by the function.
 */  

int
gsignal(sig)
int sig;
{
	int (*sigfn)(int);

	if (sig < MINSIG || sig > MAXSIG
	 || (sigfn = (int (*)(int))sigs[sig-MINSIG]) == (int (*)(int))SIG_DFL)
		return(0);
	else if (sigfn == (int (*)(int))SIG_IGN)
		return(1);
	else {
		sigs[sig-MINSIG] = SIG_DFL;
		return((*sigfn)(sig));
	}
}
