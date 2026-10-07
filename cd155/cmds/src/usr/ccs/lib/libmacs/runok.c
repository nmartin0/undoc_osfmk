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
 * File:	runok.c
 *
 * mac_runok
 *
 * Abstract:	This routine validates an user/account/nodes 
 *		request with MACD
 *
 * Arguments:	agid -	account id
 *		uid -	user id
 *		nnodes - number of nodes requested
 *
 * Return value:	TRUE	- ok
 *			FALSE	- fail in query macd
 *			-1	- account invalid
 *			-2	- account not allowed to use
 *			-3	- overrun allocation
 *			-4	- exceed max. number of nodes
 *			-5	- other, check errno
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include "mac.h"
#include "ipc.h"

int mac_runok (agid, uid, nnodes)
  int	agid;
  int	uid;
  int	nnodes;
{
	int rval, reason;
	struct cpu_ctrl *user_data;

/*
 * open connection to MACD
 */

        if (c_open() < 0) return (0);

/*
 * Fill in information for C_VALID request
 */

	if ((user_data = (struct cpu_ctrl *)
		calloc (1, sizeof (struct cpu_ctrl))) == NULL) 
		return (-5);

	user_data->uid = getuid();
	user_data->agid = agid;
	user_data->info.id = uid;
	user_data->info.maxnodes = nnodes;

	rval = cpuctl (C_VALID, user_data);
	reason = errno;
	(void) free (user_data);
	(void) c_close();

	if (rval < 0) {
		errno = EACCES;
		return (-5);
	}

	if (rval) return (1);

	return (reason);
}
