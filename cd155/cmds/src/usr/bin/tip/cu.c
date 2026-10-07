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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: cu.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:42:39 $";
#endif
/*
cu.c	1.5  com/cmd/tip,3.1,9013 12/21/89 16:44:44";
 */
/* 
 * COMPONENT_NAME: UUCP cu.c
 * 
 * FUNCTIONS: MSGSTR, cumain 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/* static char sccsid[] = "cu.c	5.2 (Berkeley) 1/13/86"; */

#include "tip.h"

void	cleanup();
void	timeout();

/*
 * Botch the interface to look like cu's
 */
cumain(argc, argv)
	char *argv[];
{
	register int i;
	static char sbuf[12];

	if (argc < 2) {
		printf(MSGSTR(USAGE3, "usage: cu telno [-t] [-s speed] [-a acu] [-l line] [-#]\n")); /*MSG*/
		exit(8);
	}
	CU = DV = NOSTR;
	BR = DEFBR;
	for (; argc > 1; argv++, argc--) {
		if (argv[1][0] != '-')
			PN = argv[1];
		else switch (argv[1][1]) {

		case 't':
			HW = 1, DU = -1;
			--argc;
			continue;

		case 'a':
			CU = argv[2]; ++argv; --argc;
			break;

		case 's':
			if (argc < 3 || speed(atoi(argv[2])) == 0) {
				fprintf(stderr, MSGSTR(NOSUPPORT, "cu: unsupported speed %s\n"), /*MSG*/
					argv[2]);
				exit(3);
			}
			BR = atoi(argv[2]); ++argv; --argc;
			break;

		case 'l':
			DV = argv[2]; ++argv; --argc;
			break;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if (CU)
				CU[strlen(CU)-1] = argv[1][1];
			if (DV)
				DV[strlen(DV)-1] = argv[1][1];
			break;

		default:
			printf(MSGSTR(BADFLAG, "Bad flag %s"), argv[1]); /*MSG*/
			break;
		}
	}
	signal(SIGINT, cleanup);
	signal(SIGQUIT, cleanup);
	signal(SIGHUP, cleanup);
	signal(SIGTERM, cleanup);

	/*
	 * The "cu" host name is used to define the
	 * attributes of the generic dialer.
	 */
	(void)sprintf(sbuf, "cu%d", BR);
	if ((i = hunt(sbuf)) == 0) {
		printf(MSGSTR(ALLBUSY, "all ports busy\n")); /*MSG*/
		exit(3);
	}
	if (i == -1) {
		printf(MSGSTR(LINKDOWN, "link down\n")); /*MSG*/
		uu_unlock(uucplock);
		exit(3);
	}
	setbuf(stdout, NULL);
	loginit();
	user_uid();
	vinit();
	setparity("none");
	boolean(value(VERBOSE)) = 0;
	if (HW)
		ttysetup(speed(BR));
	if (connect()) {
		printf(MSGSTR(CONNECTFAIL, "Connect failed\n")); /*MSG*/
		daemon_uid();
		uu_unlock(uucplock);
		exit(1);
	}
	if (!HW)
		ttysetup(speed(BR));
}
