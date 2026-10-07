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
static char rcsid[] = "@(#)$RCSfile: chmod.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:20:17 $";
#endif
/*
 * Copyright (c) 1980, 1988 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 * 
 * chmod.c	5.7 (Berkeley) 4/21/88
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1980, 1988 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */


/*
 * chmod options mode files
 * where
 *	mode is [ugoa][+-=][rwxXstugo] or an octal number
 *	options are -Rf
 */
#include <stdio.h>
#include <sys/types.h>
#ifdef PFS
#include <sys/estat.h>
#else
#include <sys/stat.h>
#endif PFS
#include <sys/dir.h>
#include <locale.h>
#include <sys/errno.h>
#include <nl_types.h>
#include "chmod_msg.h"

#define MSGSTR(num,str) catgets(catd,MS_CHMOD,num,str)  /*MSG*/


static int	fflag, rflag, retval, um;
static char	*modestring, *ms;
nl_catd catd;

main(argc, argv)
	int argc;
	char **argv;
{
	extern char *optarg;
	extern int optind, opterr;
	int ch;

        (void ) setlocale(LC_ALL,"");
        catd = catopen(MF_CHMOD, 0);

	/*
	 * since "-[rwx]" etc. are valid file modes, we don't let getopt(3)
	 * print error messages, and we mess around with optind as necessary.
	 */
	opterr = 0;
	while ((ch = getopt(argc, argv, "Rf")) != EOF)
		switch((char)ch) {
		case 'R':
			rflag++;
			break;
		case 'f':
			fflag++;
			break;
		case '?':
		default:
			--optind;
			goto done;
		}
done:	argv += optind;
	argc -= optind;

	if (argc < 2) {
                fprintf(stderr, MSGSTR(USAGE, "Usage: chmod [-Rf][ugoa][+-=][rwxstugo] file ...\n"));
		exit(-1);
	}

	modestring = *argv;
	um = umask(0);
	(void)newmode((u_short)0);

	while (*++argv)
		change(*argv);
	exit(retval);
}

change(file)
	char *file;
{
	register DIR *dirp;
	register struct dirent *dp;
#ifdef PFS
	struct estat buf;
#else
	struct stat buf;
#endif PFS

#ifdef PFS
	if (_lestat(file, &buf) || chmod(file, newmode(buf.st_mode))) {
#else
	if (lstat(file, &buf) || chmod(file, newmode(buf.st_mode))) {
#endif PFS
		err(file);
		return;
	}
	if (rflag && ((buf.st_mode & S_IFMT) == S_IFDIR)) {
		if (chdir(file) < 0 || !(dirp = opendir("."))) {
			err(file);
			return;
		}
		for (dp = readdir(dirp); dp; dp = readdir(dirp)) {
			if (dp->d_name[0] == '.' && (!dp->d_name[1] ||
			    dp->d_name[1] == '.' && !dp->d_name[2]))
				continue;
			change(dp->d_name);
		}
		closedir(dirp);
		if (chdir("..")) {
			err("..");
			exit(fflag ? 0 : -1);
		}
	}
}

err(s)
	char *s;
{
	if (fflag)
		return;
	fputs("chmod: ", stderr);
	perror(s);
	retval = -1;
}

newmode(nm)
	u_short nm;
{
	register int o, m, b;

	ms = modestring;
	m = abs();
	if (*ms == '\0')
		return (m);
	do {
		m = who();
		while (o = what()) {
			b = where((int)nm);
			switch (o) {
			case '+':
				nm |= b & m;
				break;
			case '-':
				nm &= ~(b & m);
				break;
			case '=':
				nm &= ~m;
				nm |= b & m;
				break;
			}
		}
	} while (*ms++ == ',');
	if (*--ms) {
                fprintf(stderr, MSGSTR(EMODE, "chmod: invalid mode\n"));
		exit(-1);
	}
	return ((int)nm);
}

abs()
{
	register int c, i;

	i = 0;
	while ((c = *ms++) >= '0' && c <= '7')
		i = (i << 3) + (c - '0');
	ms--;
	return (i);
}

#define	USER	05700	/* user's bits */
#define	GROUP	02070	/* group's bits */
#define	OTHER	00007	/* other's bits */
#define	ALL	01777	/* all (note absence of setuid, etc) */

#define	READ	00444	/* read permit */
#define	WRITE	00222	/* write permit */
#define	EXEC	00111	/* exec permit */
#define	SETID	06000	/* set[ug]id */
#define	STICKY	01000	/* sticky bit */

who()
{
	register int m;

	m = 0;
	for (;;) switch (*ms++) {
	case 'u':
		m |= USER;
		continue;
	case 'g':
		m |= GROUP;
		continue;
	case 'o':
		m |= OTHER;
		continue;
	case 'a':
		m |= ALL;
		continue;
	default:
		ms--;
		if (m == 0)
			m = ALL & ~um;
		return (m);
	}
}

what()
{
	switch (*ms) {
	case '+':
	case '-':
	case '=':
		return (*ms++);
	}
	return (0);
}

where(om)
	register int om;
{
	register int m;

 	m = 0;
	switch (*ms) {
	case 'u':
		m = (om & USER) >> 6;
		goto dup;
	case 'g':
		m = (om & GROUP) >> 3;
		goto dup;
	case 'o':
		m = (om & OTHER);
	dup:
		m &= (READ|WRITE|EXEC);
		m |= (m << 3) | (m << 6);
		++ms;
		return (m);
	}
	for (;;) switch (*ms++) {
	case 'r':
		m |= READ;
		continue;
	case 'w':
		m |= WRITE;
		continue;
	case 'x':
		m |= EXEC;
		continue;
	case 'X':
		if ((om & S_IFDIR) || (om & EXEC))
			m |= EXEC;
		continue;
	case 's':
		m |= SETID;
		continue;
	case 't':
		m |= STICKY;
		continue;
	default:
		ms--;
		return (m);
	}
}
