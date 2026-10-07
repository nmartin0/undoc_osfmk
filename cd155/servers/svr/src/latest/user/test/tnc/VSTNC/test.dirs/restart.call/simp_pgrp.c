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
 * Revision 1.2  1994/11/18  21:07:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:57:00  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:18:39  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:33:51  hao
 * 	Initial check-in.
 * 
 */
#include <stdio.h>
#include <errno.h>

main()
{
	FILE 		*fd;
	int		waitstat, status, pid;

	status = setpgid(0,0);
	if (status == -1) {
		fprintf(stderr, "Can't setpgrp.\n");
		exit(-1);
	}

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Can't fork.\n");
		exit(-1);
	}

	if (pid == 0) {
		fd = fopen("child.data", "w");
		/*
		 * Chkpnt should occure here.  Might need to be tuned to
		 * make sure it happens here.
		 */
		sleep(10);
		printf("Child writing to child.data.\n");
		fprintf(fd, "%d", getpgrp());
		fclose(fd);
		exit(0);
	}

	fd = fopen("parent.data", "w");
	wait(&waitstat);
	fprintf(fd, "%d", getpgrp());
	fclose(fd);
	exit(0);
}
