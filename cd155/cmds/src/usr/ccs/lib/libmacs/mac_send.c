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
 * File:	mac_msg.c
 *
 * mac_send 
 *
 * Abstract:	open a TCP/IP connection to MACD, send the message 
 *		to macd and disconnect.
 *
 * Arguments:	type -	request type
 *		data -	data to be send
 *
 * Return value:	0  	successful
 *			-1	failure
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include "mac.h"
#include "ipc.h"

extern errno;

int mac_msg (type, data)
int	type;
char *	data;
{
/*
 * Super-user privilege is required
 */

        if (getuid() != 0) {
#ifdef DEBUG
                (void) fprintf(stderr,"cpuctl:  real user id = root required\n")
;
#endif
                errno = EACCES;
                return(-1);
        }


/*
 * open connection to MACD
 */

	if (c_open() < 0) return (-1);

	/*
	  Fill in the client request and send the data to the MACD
	*/

	if (cpuctl (type, data) != 0) {
                (void) c_close();
                return (-1);
        }

        (void) c_close();
	return (0);
}
