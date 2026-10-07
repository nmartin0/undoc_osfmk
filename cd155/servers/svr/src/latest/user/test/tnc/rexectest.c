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
 * $Log: rexectest.c,v $
 * Revision 1.6  1995/02/02  00:59:25  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  21:04:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:54:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:33:17  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:22:43  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:04:55  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:50:40  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:06:36  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:59:14  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.1  1992/04/14  15:04:16  klh
 * Fix RCS Log.
 *
 * $EndLog$
 */
#include <stdio.h>
main(argc, argv, envp)
int argc;
char *argv[];
char *envp[];
{
	int rstat;
	int new_node;
	extern int errno;
	extern migrate();
	char *argvhost[2];
	char **argptr = argvhost;
	char **envptr = envp;
	char *pathname = "/bin/hostname";	/* default one */

	argvhost[0] = "hostname";
	argvhost[1] = 0;

	new_node = atoi(argv[1]);

	if( argc >= 3 ) {
		pathname = argv[2];
		argptr = &argv[2];
	}

	printf("[%d]: before rexec of '%s' new_node=%d &new_node=0x%x\n",
			getpid(), pathname, new_node, &new_node);
	if( new_node < 0 ) {
		printf("[%d]: Node is bogus, test aborted.\n", getpid());
		exit(1);
	}
	fflush(stdout);

	rstat = rexecve(pathname, argptr, envptr, new_node);

	if( rstat != 0 ) {
		printf("[%d]: rstat=%d, errno=%d doing rexec\n",
				getpid(), rstat, errno);
		exit(1);
	}
}
