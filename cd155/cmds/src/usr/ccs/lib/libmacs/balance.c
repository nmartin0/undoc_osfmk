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
 * File:	balance.c
 *
 * mac_balance 
 *
 * Abstract:	Open a TCP/IP connection to MACD and gets an user 
 *		account information
 *
 * Arguments:	agid -	account id
 *		uid -	user id
 *
 * Return value:i
 *		>NULL	valid struct cpuctrl *, if successful
 *		NULL	failure
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#include "mac.h"
#include "ipc.h"

extern errno;

struct cpu_ctrl *mac_balance (agid, uid)
int	agid;
int	uid;
{
	struct cpu_ctrl *user_data;

/*
 * open connection to MACD
 */

	if (c_open() < 0) return (NULL);

/*
 * Fill in information for GETLIMIT request
 */

	if ((user_data = (struct cpu_ctrl *)
		calloc (1, sizeof (struct cpu_ctrl))) == NULL) 
		return (NULL);

	user_data->uid = getuid();
	user_data->agid = agid;
	user_data->info.id = uid;

	if (cpuctl (C_GETLIMIT, user_data) < 0) {
		errno = EACCES;
		(void) c_close();
		return (NULL);
	}

	(void) c_close();
	return (user_data);
}
