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
 
/**************************************************************
 *
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	deluser.c
 *
 * res_deluser 
 *
 * Abstract:	This routine removes an user from an account in MACD 
 *		database by sending an appropriate request message to
 *		MACD.  It assumes an open connection to MACD.
 *
 * Arguments:	uid -	user id to be removed from the account
 *		agid -	account id from which the user is to be removed
 *
 * Return value:
 *		0  --  successful
 *		-1 --  failure
 *
 ****************************************************************/

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
#include "ipc.h"

extern errno;


int mac_deluser(agid, uid)
  int	agid;
  int	uid;
{

	struct cpu_ctrl user_data;
	extern void bzero();

/*
 * Fill in information for DELLIMIT request
 */
	(void) bzero ((char*)&user_data, sizeof(user_data));
	user_data.uid = getuid();
	user_data.agid = agid;
	user_data.info.id = uid;

/* 
 * If not a super-user, ding them
*/
	if (user_data.uid !=0) {
		errno = EACCES;
		macerr();
		return (-1);
	}

	if (cpuctl (C_DELLIMIT, &user_data) < 0) {
		macerr();
		return (-1);
	}

	return (0);
}
