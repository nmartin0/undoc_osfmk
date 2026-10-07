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
 *		Copyright (c) Locus Computing, 1991-92
 * 		This is UNPUBLISHED source code that is
 * 		the property of Locus Computing, containing
 *		proprietary secrets of LCC.  Any disclosure
 *		is strictly prohibited.  Locus makes no warantee,
 *		explicit or implicit, on the functionality of this code.
 */

/* 
 * HISTORY
 * $Log: simp_pgrp.c,v $
 * Revision 1.2  1994/11/18  21:08:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:57:27  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:19:09  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:36:18  hao
 * 	Initial check-in.
 * 
 */
#include <stdio.h>

main() 
{
	int		pid;

	setpgid(0, 0);

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "test program can't fork.\n");
		exit(-1);
	}

	if (pid == 0) {
		sleep(10);
		exit(0);
	}

	wait();
	printf("pgrp %d\n", getpgrp());
	exit(0);
}
