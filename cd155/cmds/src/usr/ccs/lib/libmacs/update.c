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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	update.c
 *
 * mac_update
 *
 * Abstract:	This routine updates account attributes in MACD
 *		database by sending an appropriate request message
 *		to MACD.  It assumes an open connection to MACD.
 *
 * Arguments:	agid -	account id
 *		amount -in seconds 
 *		sign -	0 set amount, 1 increment, -1 decrement
 *		wtflg -	priority weight (1) or not (0), -1 no 
 *		killflg -	kill jobs (1) or not (0), -1 no change
 *		lockflg -	lock killflag(1) or not(0), -1 no change
 *		unlimit - 	unlimited(1) or not(0), -1 no change
 *		maxnodes -	maximum number of nodes, < -1 no change
 *
 * Return value:	0  	successful
 *			-1 	failure
 */

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <sys/errno.h>
#include "mac.h"

extern errno;


mac_update (agid, amount, sign, wtflg, killflg, lockflg, unlimit, maxnodes)

int	agid;
double	amount;
int	sign;
int 	wtflg;
int 	killflg;
int 	lockflg;
int 	unlimit;
int 	maxnodes;

{

	struct cpu_ctrl acct_data,user_data;
	double prev_alloc;
	extern void bzero();

/*
 * Fill in information for GETACCT and SETACCT requests
 */

	(void) bzero ((char*)&acct_data, sizeof(acct_data));
	acct_data.uid = getuid();
	acct_data.agid = agid;
	acct_data.info.id = agid;

/* 
 * If not a super-user, ding them
*/

	if (acct_data.uid !=0 && killflg == -1) {
		errno = EACCES;
		macerr();
		return (-1);
	}

	if (cpuctl (C_GETACCT, &acct_data) < 0) {
		macerr();
		return (-1);
	}

/*
 * Fill in information for SETACCT request
 */

	if (killflg != -1) {
		if (killflg != 0 && killflg != 1) {
			(void) fprintf (stderr,
			"Invalid killjobs flag value\n");
			return (-1);
		}
		acct_data.info.killjobs = killflg;

/*  if not root, no other attributes can be set before call */

		if (acct_data.uid !=0) {

			(void) bzero ((char*)&user_data, sizeof(user_data));
			user_data.uid = getuid();
			user_data.agid = agid;
			user_data.info.id = user_data.uid;

/*
 * Get the potential pi's entry from MACD
 */

			if (cpuctl (C_GETLIMIT, &user_data) < 0) {
				macerr();
				return (-1);
			}

/*
 * If the user's cpulimit entry doesn't have the modify bit set ding them 
 */

			if (user_data.info.modify == 0 || 
			user_data.info.transfer == 0) {
				errno = EACCES;
				macerr();
				return (-1);
			}

			if (cpuctl (C_SETACCT, &acct_data) < 0) {
				macerr();
				return (-1);
			}
			else
				return(0);
		}

	}
		
	if (lockflg != -1) {
		if (lockflg != 0 && lockflg != 1) {
			(void) fprintf (stderr,
			"Invalid lockjobs flag value\n");
			return (-1);
		}
		acct_data.info.lockjobs = lockflg;
	}
		
	if (wtflg != -1) {
		if (wtflg != 0 && wtflg != 1) {
			(void) fprintf (stderr,
			"Invalid weight flag value\n");
			return (-1);
		}
		acct_data.info.weight = wtflg;
	}

	if (unlimit != -1) {
		if (unlimit != 0 && unlimit != 1) {
			(void) fprintf (stderr,
			"Invalid unlimit flag value\n");
			return (-1);
		}
		acct_data.info.unlimit = unlimit;
		acct_data.info.inhibit = 0;
	}

	if (maxnodes >= 0 || maxnodes == -1) acct_data.info.maxnodes = maxnodes;
		
	if (amount >= 0 ) {
		if (sign==0) 
			acct_data.info.authorized = amount;
		else {
			if (sign > 0) 
				acct_data.info.authorized += amount;
			else if (amount > acct_data.info.authorized) {
                        	(void) fprintf (stderr,
                        	"Reduction amount exceeds allocation\n");
                        	return (-1);
                	}
			else
				acct_data.info.authorized -= amount;
		}	
	}
	if (acct_data.info.authorized < 0.0) acct_data.info.authorized = 0.0;

	if (cpuctl (C_SETACCT, &acct_data) < 0) {
		macerr();
		return (-1);
	}

	return (0);
}
