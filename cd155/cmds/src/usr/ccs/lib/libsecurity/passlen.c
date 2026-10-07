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
 * Copyright (c) 1987-1990 SecureWare, Inc.  All Rights Reserved.
 */

#ident "@(#)passlen.c	2.1 16:18:06 4/20/90 SecureWare"
/*
 * Based on:
 *   "@(#)passlen.c	2.2.3.1 13:11:47 12/3/89 SecureWare"
 */

/*LINTLIBRARY*/

/*
 * This file contains a routine used to make programs
 * more secure.  Specifically, this routine will compute the
 * minimum password length based on such parameters as:
 * the password lifetime, guess rate,
 * probability of guessing a password, and the alphabet size.
 *
 * The equations are taken directly from the DoD Password
 * Management Guideline (Green Book), CSC-STD-002-85, 12 April 1985.
 *
 * The one-character symbols used herein match those of the Green Book.
 */

#include <sys/secdefines.h>

#if SEC_BASE /*{*/

#include <sys/types.h>
#include <math.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>

#define	P		(1e-6)	/* prob of guessing passwd during lifetime */


/*
 * Compute and return the minimum acceptable password length allowable
 * given the password lifetime parameters and the login delay, both in
 * seconds.
 */
int
passlen(life_dur, login_delay, alphabet_size)
	time_t life_dur;
	time_t login_delay;
	int alphabet_size;
{
	double p;
	double l;
	double r;
	double g;
	double s;
	double a;
	double m;

	check_auth_parameters();

	if (login_delay == 0 )
		login_delay = (time_t) 1;

	/*
	 * All quantities dealing with time deal with minutes.
	 */
	l = (double) life_dur / 60.0;	/* l is in minutes */
	p = P;
	r = 60.0 / login_delay;
	a = (double) alphabet_size;


	/*
	 * Total guesses in lifetime.
	 */
	g = l * r;

	/*
	 * Total combinations of passwords possible.
	 */
	s = g/p;

	/*
	 * S = A^M, so  ln S = M ln A, solving for M, ==>
	 */
	m = log(s) / log(a);

	/*
	 * ==> the answer!   (Note that on fractional answers, we round UP
	 * to err on the side of extra security.
	 */
	return (int) ceil(m);
}
#endif /*} SEC_BASE */
