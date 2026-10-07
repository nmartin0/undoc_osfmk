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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: rforktest.c,v $
 * Revision 1.6  1995/02/02  01:00:26  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  21:04:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:54:55  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:33:20  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:22:48  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:04:58  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:50:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:06:40  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:59:17  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.1  1992/04/14  15:04:30  klh
 * Fix RCS Log.
 *
 * $EndLog$
 */

main(argc, argv, argenv)
int argc;
char *argv[];
char *argenv[];
{
	int rstat;
	int new_node;
	extern int errno;

	new_node = atoi(argv[1]);

	printf("[%d]: Parent task before the rfork; new_node is %d\n",
			getpid(), new_node);
	if( new_node < 0 ) {
		printf("[%d]: This is bogus test aborted.\n", getpid());
		exit(1);
	}

	rstat = rfork(new_node);

	if( rstat < 0 ) {
		printf("[%d]: Parent got an rstat of %d errno=%d doing rfork\n",
				getpid(), rstat, errno);
		exit(1);
	}
	if( rstat == 0 ) {
		printf("[%d]: Child task now running\n", getpid());
		sleep(10);
		printf("[%d]: Child task now exiting\n", getpid());
		exit(0);
	}
	printf("[%d]: Parent task got an rstat (child pid) of %d\n",
			getpid(), rstat);
	sleep(10);
	printf("[%d]: Parent task now exiting\n", getpid());
}
