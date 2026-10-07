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
 * File:	close.c
 *
 * This file contains the call to close the  TCP_IP connection
 * from a client to MACD.
 */

#include <unistd.h>

int c_close()
{
	extern int c_sock;

	if (c_sock >= 0) (void) close(c_sock);
	c_sock = -1;
	return(0);
}
