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
 
/*****************************************************************
 *
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	delacct.c
 *
 * mac_delacct
 *
 * Abstract:	This routine removes an account from MACD database 
 *		by sending an approriate request message to MACD.
 *		It assumes a open connection to MACD.
 *
 * Arguments:	agid -	account id
 *
 * Return value:
 *		0  --  successful
 *		-1 --  failure
 *
 ********************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#ifdef CRAY_YMP
#include <udb.h>
#endif
#include <sys/errno.h>
#include "mac.h"

extern errno;


int mac_delacct(agid)
  int	agid;
{

	struct cpu_ctrl acct_data;
	extern void bzero();

/*
 * Fill in information for DELACCT request
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

	if (cpuctl (C_DELACCT, &acct_data) < 0) {
		macerr();
		return (-1);
	}

	return (0);
}
