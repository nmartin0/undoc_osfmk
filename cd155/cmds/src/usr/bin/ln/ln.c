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
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ln.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:18 $";
#endif
/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/*
#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1987 Regents of the University of California.\n\
 All rights reserved.\n";
#endif 

#ifndef lint
static char sccsid[] = "ln.c	4.13 (Berkeley) 6/19/90";
#endif
*/

#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>
#include <nl_types.h>
#include "ln_msg.h"

nl_catd	catd;

#define MSGSTR(Num,Str) catgets(catd,MS_LN,Num,Str)

static linkit();
static usage();

static int	fflag=0,			/* force flag */
		sflag=0,			/* symbolic, not hard, link */
		(*linkf)();			/* system link call */

main(argc, argv)
	int	argc;
	char	**argv;
{
	extern int optind;
	struct stat buf;
	int ch, exitval, link(), symlink();
	char *sourcedir;

	(void)setlocale(LC_ALL, "");
	catd = catopen(MF_LN, 0);

	while ((ch = getopt(argc, argv, "fs")) != EOF)
		switch((char)ch) {
		case 'f':
			fflag = 1;
			break;
		case 's':
			sflag = 1;
			break;
		case '?':
		default:
			usage();
		}

	argv += optind;
	argc -= optind;

	linkf = sflag ? symlink : link;

	switch(argc) {
	case 0:
		usage();
	case 1:				/* ln target */
		exit(linkit(argv[0], ".", 1));
	case 2:				/* ln target source */
		exit(linkit(argv[0], argv[1], 0));
	default:			/* ln target1 target2 directory */
		sourcedir = argv[argc - 1];
		if (stat(sourcedir, &buf)) {
			perror(sourcedir);
			exit(1);
		}
		if ((buf.st_mode & S_IFMT) != S_IFDIR)
			usage();
		for (exitval = 0; *argv != sourcedir; ++argv)
			exitval |= linkit(*argv, sourcedir, 1);
		exit(exitval);
	}
	/*NOTREACHED*/
}

static
linkit(target, source, isdir)
	char	*target, *source;
	int	isdir;
{
	extern int	errno;
	struct stat	buf;
	char	path[MAXPATHLEN],
		*cp, *rindex(), *strcpy();

	if (!sflag) {
		/* if target doesn't exist, quit now */
		if (stat(target, &buf)) {
			perror(target);
			return(1);
		}
		/* only symbolic links to directories, unless -F option used */
		if ((buf.st_mode & S_IFMT) == S_IFDIR) {
			printf(MSGSTR(ISDIR, "%s is a directory.\n"), target);
			return(1);
		}
	}

	/* if the source is a directory, append the target's name */
	if (isdir || !stat(source, &buf) && (buf.st_mode & S_IFMT) == S_IFDIR) {
		if (!(cp = rindex(target, '/')))
			cp = target;
		else
			++cp;
		(void)sprintf(path, "%s/%s", source, cp);
		source = path;
	}

	if (fflag && !stat(source, &buf))
		if (unlink(source) < 0) {
			perror(source);
			return(1);
		}

	if ((*linkf)(target, source)) {
		perror(source);
		return(1);
	}
	return(0);
}

static
usage()
{
	fputs(MSGSTR(USAGE, "usage:\tln [-fs] targetname [sourcename]\n\tln [-fs] targetname1 targetname2 [... targetnameN] sourcedirectory\n"), stderr);
	exit(1);
}
