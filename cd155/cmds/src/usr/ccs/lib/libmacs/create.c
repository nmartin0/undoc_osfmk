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
 
/*************************************************************************
 *
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	create.c
 *
 * mac_create 
 *
 * Abstract:	This routine creates an new account entry in MACD database
 *		by sending an approriate request message to MACD.
 *		An open connection to MACD is assumed.
 *
 * Arguments:	agid -	account id
 *		amount -	CPU allocation in node-seconds 
 *		wtflg -		priority weight (1) or not(0)
 *		killflg -	kill jobs when allocation run-out (1) 
 *				or not (0)
 *		lockflg -	lock against change account attributes (1) 
 *				or not(0)
 *		unlflg -	unlimited allocation (1) or not(0)
 *		maxnodes -	maximum number of nodes allowable
 *
 * Return value:
 *		0  --  successful
 *		-1 --  failure
 *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#ifdef CRAY_YMP
#include <udb.h>
#endif

#include "mac.h"
#include "macerr.h"
#include "ipc.h"


int mac_create(agid, amount, wtflg, killflg, lockflg, unlflg, maxnodes)
  int		agid;
  double	amount;
  int		wtflg;
  int		killflg;
  int		lockflg;
  int		unlflg;
  int		maxnodes;
{

	struct cpu_ctrl acct_data;
	extern void bzero();

/*
 * Fill in information for SETACCT request
 */
	(void) bzero ((char*)&acct_data, sizeof(acct_data));
	acct_data.uid = getuid();
	acct_data.agid = agid;
	acct_data.info.id = agid;

/* 
 * If not a super-user, ding them
*/
	if (acct_data.uid !=0) {
		errno = EACCES;
		macerr();
		return (-1);
	}

	if (amount < 0.0) {
		(void) fprintf (stdout,
		"Invalid account allocation amount\n");
		return (-1);
	}
	if (wtflg != 0 && wtflg != 1) {
		(void) fprintf (stdout,
		"Invalid weight flag value\n");
		return (-1);
	}
	if (unlflg != 0 && unlflg != 1) {
		(void) fprintf (stdout,
		"Invalid unlimit-flag value\n");
		return (-1);
	}
	if (maxnodes < -1) {
		(void) fprintf (stdout,
		"Invalid number of maxnodes\n");
		return (-1);
	}
	acct_data.info.maxnodes = maxnodes;
	acct_data.info.weight = wtflg;
	acct_data.info.unlimit = unlflg;
	acct_data.info.authorized = amount;

	if (killflg != -1) {
		if (killflg != 0 && killflg != 1) {
			(void) fprintf (stdout,
			"Invalid killjobs flag value\n");
			return (-1);
		}
		acct_data.info.killjobs = killflg;
	}
	if (lockflg != -1) {
		if (lockflg != 0 && lockflg != 1) {
			(void) fprintf (stdout,
			"Invalid lockjobs flag value\n");
			return (-1);
		}
		acct_data.info.lockjobs = lockflg;
	}
		
	if (cpuctl (C_CREACCT, &acct_data) < 0) {
		macerr();
		return (-1);
	}

	return (0);
}
