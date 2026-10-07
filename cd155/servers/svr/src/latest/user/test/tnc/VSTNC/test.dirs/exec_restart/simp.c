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
 * $Log: simp.c,v $
 * Revision 1.2  1994/11/18  21:06:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:53:40  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:15:58  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:13:44  hao
 * 	Initial check_in.
 * 
 */

#include <stdio.h>
#include <errno.h>

main()
{
	FILE 		*fd;
	int		waitstat, status, pid;
	int	pgrp;

	status = setpgid(0,0);

	if (status == -1) {
		fprintf(stderr, "Can't setpgrp.\n");
		exit(-1);
	}

	fd = fopen("parent.data", "w");

	sleep(15);

	pgrp = getpgrp();
printf("pgrp %d\n", pgrp);
/*	fprintf(fd, "%d", getpgrp()); */
	fprintf(fd, "%d", pgrp);

	fclose(fd);
	exit(0);
}
