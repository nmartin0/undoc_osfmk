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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
static char sccsid[] = "@(#)show.c  1.7  com/lib/curses,3.1,9008 12/4/89 21:02:48";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   show
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <signal.h>
#include "curses.h"

#ifdef DEBUGfoo
#undef LINES
#define LINES 5
#endif

/*
 * NAME:        show
 */

main(argc, argv)
char **argv;
{
	FILE *fd;
	char linebuf[512];
	int line;
	int done();

	if (argc < 2) {
		(void) fprintf(stderr, "Usage: show file\n");
		exit(1);
	}
	fd = fopen(argv[1], "r");
	if (fd == NULL) {
		perror(argv[1]);
		exit(2);
	}
	signal((int)SIGINT, ((void (*)(int))(int) done));

	initscr();			/* initialize curses */
	noecho();			/* turn off tty echo */
	cbreak();			/* enter cbreak mode */
	nonl();				/* allow more optimizations */
	idlok(stdscr, TRUE);		/* allow insert/delete line */

	for (;;) {			/* for each screen full */
		(void) move(0, 0);
		/* werase(stdscr); */
		for (line=0; line<LINES; line++) {
			if (fgets(linebuf, (int)(sizeof linebuf), fd) == NULL) {
				clrtobot();
				done();
			}
			(void) mvprintw(line, 0, "%s", linebuf);
		}
		(void) refresh();	/* sync screen */
		if(getch() == 'q')	/* wait for user to read it */
			done();
	}
}

/*
 * Clean up and exit.
 */
done()
{
	(void) move(LINES-1,0);		/* to lower left corner */
	clrtoeol();			/* clear bottom line */
	(void) refresh();		/* flush out everything */
	endwin();			/* curses cleanup */
	exit(0);
}
