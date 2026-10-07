/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	Copyright (c) Locus Computing, 1991-92
 * 	This is UNPUBLISHED source code that is
 * 	the property of Locus Computing, containing
 *	proprietary secrets of LCC.  Any disclosure
 *	is strictly prohibited.  Locus makes no warantee,
 *	explicit or implicit, on the functionality of this code.
 */
/*
 * HISTORY
 * $Log: vu.c,v $
 * Revision 1.2  1994/11/18  21:05:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:50:43  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:19:26  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/06/10  11:55:55  chrisp
 * 	$EndLog$
 * 
 */
#define SHOW_UTT
#include <stdio.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <uxkern/import_mach.h>
#include <tnc/chkpnt.h>

int	global_int = 0;

main(int argc, char *argv[])
{
	int		error = 0;
	int		stack_int;

	global_int = -1; stack_int = -1;
	printf("vu %s %s: global int: %d, stack int: %d\n",
		argv[1], argv[2], global_int, stack_int);

	printf("Waiting 5 more seconds ...\n");
	for (;global_int < 5; global_int++, stack_int++) {
		printf("Counting: global int: %d, stack int: %d\n",
			global_int, stack_int);
		sleep(1);
	}
	printf("restarting\n");
		
	error = exec_restart(argv[1], atoi(argv[2]));

	printf("Whoops, error=%d, errno=%d\n", error, errno);

	exit(1);
}
