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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: main.c,v $
 * Revision 1.2  1994/11/19  01:31:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/18  14:47:00  shala
 * Files for new version of awk command.
 *
 * Revision 2.5.3.2  91/01/07  19:50:22  dwm
 * 	Fix initialization of lexprog (avoid null argv[1]).
 * 	[90/12/28  16:23:50  dwm]
 * 
 * Revision 2.5  90/12/06  14:10:10  devrcs
 * 	Fixed up comment headers.
 * 	[90/12/03  13:31:39  gm]
 * 
 * Revision 2.4  90/09/13  12:11:32  devrcs
 * 	-F without a field-separator argument causes SIGSEGV
 * 	[90/09/04  08:14:56  lehotsky]
 * 
 * 	Cleanup, RCS header, NLS, MSG
 * 	[90/08/29  09:59:02  knight]
 * 
 * Revision 2.3  90/07/05  23:25:51  devrcs
 * 	Use AIX source
 * 	[90/06/29  21:49:50  lehotsky]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: main.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:31 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) main.c
 *
 * FUNCTIONS: main
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984, 19885, 1986, 1987 AT&T
 * 	All Rights Reserved
 * 
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.
 * 
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 *
 * main.c	1.7  com/cmd/edit/awk,3.1,9021 12/19/89 17:23:36
 */

/*
Copyright (c) 1984, 19885, 1986, 1987 AT&T
	All Rights Reserved

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.

The copyright notice above does not evidence any
actual or intended publication of such source code.
*/

#include <stdio.h>
#include <signal.h>
#include <locale.h>
#include "awk.h"
#include "awk.g.h"


extern int dbg	= 0;
char	*cmdname;	/* gets argv[0] for error messages */
extern	FILE *yyin;	/* lex input file */
char	*lexprog = "";	/* points to program argument if it exists */
extern	int errorflag;	/* non-zero if any syntax errors; set by yyerror */
extern int compile_time = 1;	/* 0 when machine starts.  for error printing */


main(argc, argv)
	int argc;
	char *argv[];
{
	char *fs = NULL;
	extern int fpecatch(int);


	setlocale(LC_ALL,"");		/* required by NLS environment tests */
	catd = catopen(MF_AWK,0);

	cmdname = argv[0];
 	if (argc == 1)
		ERROR MSGSTR(MUSAGE, "Usage: %s [-F sep ] [-f source | 'cmds'] [files]"), cmdname FATAL;
	yyin = NULL;
	while (argc > 1 && argv[1][0] == '-' && argv[1][1] != '\0') {
		switch (argv[1][1]) {
		case 'f':	/* next argument is program filename */
			argc--;
			argv++;
			if ((yyin = fopen(argv[1], "r")) == NULL)
				ERROR MSGSTR(MOPEN, "can't open file %s"), argv[1] FATAL;
			break;
		case 'F':	/* set field separator */
			if (argv[1][2] != 0) {	/* arg is -Fsomething */
				if (argv[1][2] == 't' && argv[1][3] == 0)	/* wart: t=>\t */
					fs = "\t";
				else
					fs = &argv[1][2];
			} else {	/* it's -F (space) something */
				argc--;
				argv++;
				if (!argv[1]) {
				  ERROR MSGSTR(MUSAGE
					       , "Usage: %s [-F sep] [-f source | 'cmds'] [files]")
				    , cmdname FATAL;
				}

				if (argv[1][0] == 't' && argv[1][1] == 0)
					fs = "\t";
				else
					fs = &argv[1][0];
			}
			break;
		case 'd':
			dbg = 1;
			break;
		}
		argc--;
		argv++;
	}
	if (yyin == NULL) {	/* no -f; first argument is program */
		lexprog = (argv[1] ? argv[1] : "");
		dprintf("program = |%s|\n", lexprog);
		argc--;
		argv++;
	}
	syminit();
	while (argc > 1) {	/* do leading "name=val" */
		if (!isclvar(argv[1]))
			break;
		setclvar(argv[1]);
		argc--;
		argv++;

	}
	argv[0] = cmdname;	/* put prog name at front of arglist */
	signal(SIGFPE, (void (*)(int))fpecatch);
	dprintf("argc=%d, argv[0]=%s\n", argc, argv[0]);
	arginit(argc, argv);
	yyparse();
	dprintf("errorflag=%d\n", errorflag, NULL, NULL);
	if (fs)
		*FS = tostring(qstring(fs, '\0'));
	*FILENAME = (uuchar *) argv[1];	/* initial file name */
	if (argc == 1)		/* no filenames; use stdin */
		initgetrec();
	if (errorflag == 0) {
		compile_time = 0;
		run(winner);
	} else
		bracecheck();
	exit(errorflag);
}
