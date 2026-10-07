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
 
/* @(#)expreserve.c	5.1 23:20:07 8/15/90 SecureWare */
/*
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: expreserve.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:25 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) expreserve.c
 *
 * FUNCTION: Ignore, Ignorl, main, copyout, mkdigits, mknext, notify
 *
 * ORIGINS: 3, 10, 13, 26, 27
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
 * expreserve.c  1.9  com/cmd/edit/vi,3.1,9013 3/7/90 06:07:34
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include <sys/secdefines.h>

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <locale.h>

#include "ex_msg.h"
nl_catd	ex_catd;
#define msg(id,ds)      catgets(ex_catd, 1, id, ds)
				/* mjm: "/tmp" --> TMP */
#define TMP     "/var/tmp"

#define	HBLKS	2
#define BUFFERSIZ	(2048)

char xstr[1];			/* make loader happy */

/*
 * Expreserve - preserve a file in /var/preserve
 * Bill Joy UCB November 13, 1977
 *
 * This routine is very naive - it doesn't remove anything from
 * /var/preserve... this may mean that we  * stuff there...
 * the danger in doing anything with /var/preserve
 * is that the clock may be screwed up and we may get confused.
 *
 * We are called in two ways - first from the editor with no argumentss
 * and the standard input open on the temp file. Second with an argument
 * to preserve the entire contents of /tmp (root only).
 *
 * BUG: should do something about preserving Rx... (register contents)
 *      temporaries.
 */

#define	LBLKS	900
#define	FNSIZE	1024

struct 	header {
	time_t	Time;			/* Time temp file last updated */
	int	Uid;			/* This users identity */
	int	Flines;				/* Number of lines in file */
	char	Savedfile[FNSIZE];	/* The current file name */
	short	Blocks[LBLKS];		/* Blocks where line pointers stashed */
} H;

#ifdef	lint
#define	ignore(a)	Ignore(a)
#define	ignorl(a)	Ignorl(a)
#else
#define	ignore(a)	a
#define	ignorl(a)	a
#endif

off_t	lseek();
#if SEC_MAC
FILE	*expreserve_popen();
#else
FILE	*popen();
#endif

#define eq(a, b) strcmp(a, b) == 0


main(argc, argv)
	int	argc;
#if SEC_MAC
	char	*argv[];
#else
        char **argv;
#endif
{
	register DIR *tf;
	struct dirent *dir_entr;
	struct stat stbuf;
        char *tmpdir;

	setlocale(LC_ALL,"");		/* required by NLS environment tests */
	ex_catd = catopen(MF_EX,0);

#if SEC_MAC
	set_auth_parameters(argc, argv);
	initprivs();
#endif

	/*
	 * If only one argument, then preserve the standard input.
	 */
	if (argc == 1) {
		if (copyout((char *) 0))
			exit(1);
		exit(0);
	}

#if SEC_MAC
	if (!expreserve_check_auth()) {
		fprintf(stderr, "expreserve: need sysadmin authorization\n");
		exit(1);
	}
	expreserve_full_copy(TMP, "Ex");
#else
	/*
	 * If not super user, then can only preserve standard input.
	 */
	if (getuid()) {
		fprintf(stderr, msg(M_258, "NOT super user\n"));
		exit(1);
	}

        /*
         * ... else preserve all the stuff in /tmp (or the specified dir),
         * removing it as we go.
         */
        tmpdir = argv[1];
        if (eq(argv[1],"-a") || chdir(tmpdir) < 0) {
                tmpdir = TMP;
                if (chdir(tmpdir) < 0) {
                        perror(tmpdir);
                        exit(1);
                }
        }

	tf = opendir(".");
	if (tf == NULL) {
		perror(TMP);
		exit(1);
	}
	while ((dir_entr = readdir(tf)) != NULL) {
		if (dir_entr->d_fileno == 0)
			continue;
		/*
		 * Ex temporaries must begin with Ex;
		 * we check that the 10th character of the name is null
		 * so we won't have to worry about non-null terminated names
		 * later on.
		 */
		if (dir_entr->d_name[0] != 'E' || dir_entr->d_name[1] != 'x')
			continue;
		if (stat(dir_entr->d_name, &stbuf))
			continue;
		if ((stbuf.st_mode & S_IFMT) != S_IFREG)
			continue;
		ignore(copyout(dir_entr->d_name));
	}
	closedir(tf);
#endif
	exit(0);
}

char	pattern[] =	"/var/preserve/Exaa`XXXXX";

/*
 * Copy file name into /var/preserve/...
 * If name is (char *) 0, then do the standard input.
 * We make some checks on the input to make sure it is
 * really an editor temporary, generate a name for the
 * file (this is the slowest thing since we must stat
 * to find a unique name), and finally copy the file.
 */
copyout(name)
	char *name;
{
	int i;
	static int reenter;
	char buf[BUFFERSIZ];

	/*
	 * The first time we put in the digits of our
	 * process number at the end of the pattern.
	 */
	if (reenter == 0) {
		mkdigits(pattern);
		reenter++;
	}

	/*
	 * If a file name was given, make it the standard
	 * input if possible.
	 */
	if (name != 0) {
		ignore(close(0));
		/*
		 * Need read/write access for arcane reasons
		 * (see below).
		 */
		if (open(name, 2) < 0)
			return (-1);
	}

	/*
	 * Get the header block.
	 */
	ignorl(lseek(0, 0l, 0));
	if (read(0, (char *) &H, sizeof H) != sizeof H) {
format:
		if (name == 0)
                        fprintf(stderr, msg(M_259, "Buffer format error\n"));
		return (-1);
	}

	/*
	 * Consistency checsks so we don't copy out garbage.
	 */
	if (H.Flines < 0) {
#ifdef DEBUG
		fprintf(stderr, "Negative number of lines\n");
#endif
		goto format;
	}
	if (H.Blocks[0] != HBLKS || H.Blocks[1] != HBLKS+1) {
#ifdef DEBUG
		fprintf(stderr, "Blocks %d %d\n", H.Blocks[0], H.Blocks[1]);
#endif
		goto format;
	}
	if (name == 0 && H.Uid != getuid()) {
#ifdef DEBUG
		fprintf(stderr, "Wrong user-id\n");
#endif
		goto format;
	}
	if (lseek(0, 0l, 0)) {
#ifdef DEBUG
		fprintf(stderr, "Negative number of lines\n");
#endif
		goto format;
	}

	/*
	 * If no name was assigned to the file, then give it the name
	 * LOST, by putting this in the header.
	 */
	if (H.Savedfile[0] == 0) {
		strcpy(H.Savedfile, "LOST");
		ignore(write(0, (char *) &H, sizeof H));
		H.Savedfile[0] = 0;
		lseek(0, 0l, 0);
	}

	/*
	 * File is good.  Get a name and create a file for the copy.
	 */
	mknext(pattern);
	ignore(close(1));
#if SEC_MAC
	if (expreserve_create_file(name, pattern, H.Uid) < 0)
#else
	if (creat(pattern, 0600) < 0)
#endif
	{
		if (name == 0)
			perror(pattern);
		return (1);
	}

#if !SEC_MAC
	/*
	 * Make the target be owned by the owner of the file.
	 */
	ignore(chown(pattern, H.Uid, 0));
#endif

	/*
	 * Copy the file.
	 */
	for (;;) {
		i = read(0, buf, BUFFERSIZ);
		if (i < 0) {
			if (name)
				perror(msg(M_260, "Buffer read error"));
#if SEC_MAC
			expreserve_unlink(pattern);
#else
			ignore(unlink(pattern));
#endif
			return (-1);
		}
		if (i == 0) {
			if (name)
				ignore(unlink(name));
			notify(H.Uid, H.Savedfile, (int) name);
			return (0);
		}
		if (write(1, buf, i) != i) {
			if (name == 0)
				perror(pattern);
#if SEC_MAC
			expreserve_unlink(pattern);
#else
			unlink(pattern);
#endif
			return (-1);
		}
	}
}

/*
 * Blast the last 5 characters of cp to be the process number.
 */
mkdigits(cp)
	char *cp;
{
	register int i, j;

	for (i = getpid(), j = 5, cp += strlen(cp); j > 0; i /= 10, j--)
		*--cp = i % 10 | '0';
}

/*
 * Make the name in cp be unique by clobbering up to
 * three alphabetic characters into a sequence of the form 'aab', 'aac', etc.
 * Mktemp gets weird names too quickly to be useful here.
 */
mknext(cp)
	char *cp;
{
	char *dcp;
	struct stat stb;

	dcp = cp + strlen(cp) - 1;
	while (isdigit(*dcp))
		dcp--;
whoops:
	if (dcp[0] == 'z') {
		dcp[0] = 'a';
		if (dcp[-1] == 'z') {
			dcp[-1] = 'a';
			if (dcp[-2] == 'z')
				fprintf(stderr, msg(M_261, "Can't find a name\t"));
			dcp[-2]++;
		} else
			dcp[-1]++;
	} else
		dcp[0]++;
	if (stat(cp, &stb) == 0)
		goto whoops;
}

/*
 * Notify user uid that his file fname has been saved.
 */
notify(uid, fname, flag)
	int uid;
	char *fname;
{
	struct passwd *pp = getpwuid(uid);
	register FILE *mf;
	char cmd[BUFFERSIZ];

	if (pp == NULL)
		return;
	sprintf(cmd, "mail %s", pp->pw_name);
	setuid(getuid());
#if SEC_MAC
	mf = expreserve_popen(cmd);
#else
	mf = popen(cmd, "w");
#endif
	if (mf == NULL)
		return;
	setbuffer(mf, cmd, sizeof(cmd));
	if (fname[0] == 0) {
		fprintf(mf, flag ?
msg(M_262, "A copy of an editor buffer of yours was saved when the system went down.\n") :
msg(M_263, "A copy of an editor buffer of yours was saved when the editor was killed.\n"));
		fprintf(mf,
msg(M_264, "No name was associated with this buffer so it has been named \"LOST\".\n"));
	} else
		/*
		 * "the editor was killed" is perhaps still not an ideal
		 * error message.  Usually, either it was forcably terminated
		 * or the phone was hung up, but we don't know which.
		 */
		fprintf(mf, flag ?
msg(M_265, "A copy of an editor buffer of your file \"%s\"\nwas saved when the system went down.\n") :
msg(M_266, "A copy of an editor buffer of your file \"%s\"\nwas saved when the editor was killed.\n"), fname);
	fprintf(mf,
msg(M_267, "This buffer can be retrieved using the \"recover\" command of the editor.\n\
An easy way to do this is to give the command \"ex -r %s\".\n\
This works for \"edit\" and \"vi\" also.\n"), fname);
#if SEC_MAC
	expreserve_pclose(mf);
#else
	pclose(mf);
#endif
}

/*
 *	people making love
 *	never exactly the same
 *	just like a snowflake 
 */

#ifdef lint
Ignore(a)
	int a;
{

	a = a;
}

Ignorl(a)
	long a;
{

	a = a;
}
#endif
