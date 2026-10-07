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
 *      INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *      This software is supplied under the terms of a license 
 *      agreement or nondisclosure agreement with Intel Corporation
 *      and may not be copied or disclosed except in accordance with
 *      the terms of that agreement.
 *      Copyright (c) 1991,1992,1993  Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/watchdog/watchdog.c,v 1.2 1994/11/21 16:48:26 mtm Exp $
 *
 * $Log: watchdog.c,v $
 * Revision 1.2  1994/11/21  16:48:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/08  18:24:23  steved
 * Initial check-in
 *
 * 
 *
 */

#include <stdio.h>

main(argc, argv)
int argc;
char **argv;
{
	int on=0;

	if (argc < 2)
		usage(argv[0]);
	if (!strncmp(argv[1], "on", 2)) on=1;
	else if (!strncmp(argv[1], "off", 3)) on=0;
	else
		usage(argv[0]);

	if (on)
	{
		printf("Starting Watchdog...\n");
		asm("adds l%1, r0, r16");
		asm("trap r0,r7,r0");
	}
	else
	{
		printf("Stopping Watchdog...\n");
		asm("adds l%0, r0, r16");
		asm("trap r0,r7,r0");
	}
	exit(0);
}

usage(prog)
char *prog;
{
	printf("Usage:  %s [on | off]\n",prog);
	exit(0);
}

