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
 * $Log: simp1.c,v $
 * Revision 1.2  1994/11/18  21:07:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:56:14  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:18:11  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:29:03  hao
 * 	Initial check-in.
 * 
 */
#include <stdio.h>
#include <errno.h>

int write_out();

main()
{
	int		waitstat, status, pid;
	int		(*on_restart_fnpt)();

	status = setpgid(0, 0);
	if (status == -1) {
		fprintf(stderr, "Error: Can't setpgrp.\n");
		exit(-1);
	}

	on_restart_fnpt = write_out;
	if (on_restart(on_restart_fnpt) < 0) {
		fprintf(stderr, "Error: can't set on_restart.\n");
		exit(-1);
	}

	sleep(10);
	exit(0);

}

int
write_out()
{
	FILE 		*fd;
	fd = fopen("parent.data", "w");
	fprintf(fd, "%d", getpgrp());
	fclose(fd);
	return(0);
}
