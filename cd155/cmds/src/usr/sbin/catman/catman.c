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

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: catman.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:04:47 $";
#endif

/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: doit
 *
 * ORIGINS: 26, 27 
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * catman.c	1.5  com/cmd/man,3.1,9021 1/15/90 15:33:01
 */

/*
 * NAME: catman [ -p ] [ -n ] [ -w ] [-M path] [sections]
 *
 * FUNCTION: 
 *	The catman command creates the preformatted versions of the on-line
 *	manual from the nroff input files.  Each manual page is examined and
 * 	those whose preformatted versions are missing or out of date are 
 *	recreated. If any changes are made, catman will recreate the whatis
 *  	datebase.
 * FLAGS:
 *	-n	prevents creations of the whatis datebase.
 *	-p 	prints what would be done instead of doing it.
 *	-w	causes only the whatis datebase to be created. No manual
 *		reformatting is done.
 *	-M	updates manual pages located in the set of directories
 *		specified by path.
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>

#include <dirent.h>
#include <ctype.h>
#include <locale.h>

#include "catman_msg.h"
nl_catd catd;
#define MSGSTR(n, s) catgets(catd, MS_CATMAN, n, s)

#define USE() \
	printf(MSGSTR(USAGE, "usage: catman [-p] [-n] [-w] [-M path] [sections]\n")); \
	exit(-1)

#define WHATIS() \
	sprintf(buf, "%s %s", makewhatis, mandir); \
	if (pflag) \
		printf("%s\n", buf); \
	else if ((status = system(buf)) != 0) { \
		fprintf(stderr, MSGSTR(STATUS, "catman: %s: exit status %d\n"), buf, status); \
		exstat = 1; \
		}


char	buf[BUFSIZ];
char	pflag;
char	nflag;
char	wflag;
char	man[NAME_MAX+6] = "manx/";
int	exstat = 0;
char	cat[NAME_MAX+6] = "catx/";
char	lncat[NAME_MAX+9] = "../catx/";
char	*manpath = "/usr/share/man";
char	*sections = "1234567";
char	*makewhatis = "/usr/lbin/mkwhatis";
char	*index(), *rindex();
char	*NLstrcpy();

main(ac, av)
	int ac;
	char *av[];
{
	char *mp, *nextp;

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_CATMAN, 0);

	if ((mp = getenv("MANPATH")) != NULL)
		manpath = mp;

	ac--, av++;
	while (ac > 0 && av[0][0] == '-') {
		switch (av[0][1]) {

		case 'p':
			pflag++;
			break;

		case 'n':
			nflag++;
			break;

		case 'w':
			wflag++;
			break;

		case 'M':
			if (ac <= 1) {
				fprintf(stderr, MSGSTR(MISSPATH, "catman: -M option missing path\n"));
				exit(1);
			}
			ac--, av++;
			manpath = *av;
			break;

		default:
			USE();
			break;
		}
		ac--, av++;
	}
	if (ac > 1) { USE(); }
	if (ac == 1)
		sections = *av;
	for (mp = manpath; mp && ((nextp = index(mp, ':')), 1); mp = nextp) {
		if (nextp)
			*nextp++ = '\0';
		doit(mp);
	}
	exit(exstat);
}

/*
 * NAME: doit
 *
 * FUNCTION: for each man file in each manual section , performs a nroff 
 *	     fuuction on it.  The resulting files are cat files.
 *
 */
doit(mandir)
	char *mandir;
{
	register char *msp, *csp, *sp;
	int changed = 0;
	int status;

	if (wflag) { 
		WHATIS() 
                return;
	}

	if (chdir(mandir) < 0) {
		sprintf(buf, MSGSTR(MANDIR, "catman: %s"), mandir);
		perror(buf);
		return;
	}
	if (pflag)
		printf(MSGSTR(CDMANDIR, "cd %s\n"), mandir);
	msp = &man[5];
	csp = &cat[5];
	(void) umask((mode_t)0);

/* looks in a list of manual sections */
	for (sp = sections; *sp; sp++) {
		register DIR *mdir;
		register struct dirent *dir;
		struct stat sbuf;

		man[3] = cat[3] = *sp;
		*msp = *csp = '\0';

/* opens a man directory, makes a cat directory and checks its accessing modes */ 
		if ((mdir = opendir(man)) == NULL) {
			sprintf(buf, MSGSTR(OPENDIR, "catman: opendir: %s"), man);
			perror(buf);
			continue;
		}
		if (stat(cat, &sbuf) < 0) {
			register char *cp;

			(void) NLstrcpy(buf, cat);
		        cp = rindex(buf, '/');
			if (cp && cp[1] == '\0')
				*cp = '\0';
			if (pflag)
				printf(MSGSTR(MKDIR, "mkdir %s\n"), buf);
			else if (mkdir(buf, (mode_t)0777) < 0) {
				sprintf(buf, MSGSTR(MKDIRCAT, "catman: mkdir: %s"), cat);
				perror(buf);
				exstat = 1;
				continue;
			}
			(void) stat(cat, &sbuf);
		}
		if (access(cat, R_OK|W_OK|X_OK) == -1) {
			sprintf(buf, MSGSTR(CAT, "catman: %s"), cat);
			perror(buf);
			exstat = 1;
			continue;
		}
		if ((sbuf.st_mode & S_IFMT) != S_IFDIR) {
			fprintf(stderr, MSGSTR(NOTDIR, "catman: %s: not a directory\n"), cat);
			exstat = 1;
			continue;
		}

/* reads in each file in one manual section and creates the cat file for it */
		while ((dir = readdir(mdir)) != NULL) {
			time_t time;
			register char *tsp;
			FILE *inf;
			int  makelink;

			if ((dir->d_fileno == 0) || (0 == NLstrcmp(dir->d_name, ".")) || (0 == NLstrcmp(dir->d_name, "..")))
				continue;
			/*
			 * Make sure this is a man file, i.e., that it
			 * ends in .[0-9] or .[0-9][a-z]
			 */
			tsp = rindex(dir->d_name, '.');
			if (tsp == NULL)
				continue;
			if (!isdigit((int)*++tsp) && *tsp != *sp)
				continue;
			if (*++tsp && !isalpha((int)*tsp))
				continue;
			if (*tsp && *++tsp)
				continue;
			(void) NLstrcpy(msp, dir->d_name);
			if ((inf = fopen(man, "r")) == NULL) {
				sprintf(buf, "catman: %s");
				perror(buf);
				exstat = 1;
				continue;
			}
			makelink = 0;
			if (getc(inf) == '.' && getc(inf) == 's'
			    && getc(inf) == 'o') {
				if (getc(inf) != ' ' ||
				    fgets(lncat+3, (int)(sizeof(lncat)-3), inf)==NULL) {
					fclose(inf);
					continue;
				}
				if (lncat[strlen(lncat)-1] == '\n')
					lncat[strlen(lncat)-1] = '\0';
				if (strncmp(lncat+3, "man", 3) != 0) {
					fclose(inf);
					continue;
				}
				bcopy("../cat", lncat, sizeof("../cat")-1);
				makelink = 1;
			}
			fclose(inf);

			(void) NLstrcpy(csp, dir->d_name);
			if (stat(cat, &sbuf) >= 0) {
				time = sbuf.st_mtime;
				(void) stat(man, &sbuf);
				if (time >= sbuf.st_mtime)
					continue;
				(void) unlink(cat);
			}
			if (makelink) {
				/*
				 * Don't unlink a directory by accident.
				 */
				if (stat(lncat+3, &sbuf) >= 0 &&
				    (((sbuf.st_mode&S_IFMT)==S_IFREG) ||
				     ((sbuf.st_mode&S_IFMT)==S_IFLNK)))
					(void) unlink(cat);
				if (pflag)
					printf(MSGSTR(LNCAT, "ln -s %s %s\n"), lncat, cat);
				else
					if (symlink(lncat, cat) == -1) {
						sprintf(buf, MSGSTR(SYMLINK, "catman: symlink: %s"), cat);
						perror(buf);
						exstat = 1;
						continue;
					}
			}
			else {
				sprintf(buf, MSGSTR(NROFF, "nroff -man %s > %s"), man, cat);
				if (pflag)
					printf("%s\n", buf);
				else if ((status = system(buf)) != 0) {
					fprintf(stderr, MSGSTR(BADMAN, "catman: nroff: %s: exit status %d\n")
					, cat, status);
					exstat = 1;
					continue;
				}
			}
			changed = 1;
		}
		closedir(mdir);
	}

/* updates the whatis datebase */
	if (changed && !nflag) { WHATIS() }

	return;
}
