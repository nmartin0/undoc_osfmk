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
 *	Copyright 1993, The Portland Group, Incorporated.
 *	All rights reserved.
 *
 *	THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with The Portland Group and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/* dummy version of mp_init */

#include <stdio.h>

int _mp_tcpus = 1;		/* total number of CPUS */
int _mp_inparr;			/* in parallel region */
int mp_init_called;

/* init */

void
_mp_init()
{
	_mp_inparr = 0;
	return;
}

void
_mp_bcs()
{
	return;
}

void
_mp_ecs()
{
	return;
}
