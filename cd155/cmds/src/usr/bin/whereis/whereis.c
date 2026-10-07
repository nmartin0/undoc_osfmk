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
static char rcsid[] = "@(#)$RCSfile: whereis.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:47:24 $";
#endif
/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
 *
 * FUNCTIONS:
 *
 * ORIGINS: 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * whereis.c   1.5  com/cmd/scan,3.1,9021 12/21/89 12:53:22";
 */


#include <sys/param.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <locale.h>

#include "whereis_msg.h" 
#define MSGSTR(n,s) NLgetamsg(MF_WHEREIS, MS_WHEREIS, n, s) 
nl_catd         catd;

static void findin();
static void find();
static void findv();
static void lookman();
static void lookbin();
static void looksrc();
static void lookup();
static void zerof();
static void getlist();

static char *bindirs[] = {
	"/etc",
	"/etc/nls",
	"/sbin",
	"/usr/bin",
	"/usr/lbin",
	"/usr/lbin/spell",
	"/usr/ccs/lib",
	"/usr/lib",
	"/usr/local",
	"/usr/hosts",
	"/usr/sbin",
	0
};
static char *mandirs[] = {
	"/usr/share/cat/cat1",
	"/usr/share/cat/cat2",
	"/usr/share/cat/cat3",
	"/usr/share/cat/cat4",
	"/usr/share/cat/cat5",
	"/usr/share/cat/cat6",
	"/usr/share/cat/cat7",
	"/usr/share/cat/cat8",
	"/usr/share/man/man1",
	"/usr/share/man/man2",
	"/usr/share/man/man3",
	"/usr/share/man/man4",
	"/usr/share/man/man5",
	"/usr/share/man/man6",
	"/usr/share/man/man7",
	"/usr/share/man/man8",
	0
};
static char *srcdirs[]  = {
	"/usr/src/etc",
	"/usr/src/sbin",
	"/usr/src/bin",
	"/usr/src/lib",
	"/usr/src/local",
	"/usr/src/hosts",
	"/usr/src/sbin",
	0
};

char	sflag = 1;
char	bflag = 1;
char	mflag = 1;
char	**Sflag;
int	Scnt;
char	**Bflag;
int	Bcnt;
char	**Mflag;
int	Mcnt;
char	uflag;
int	count;
int	print;
int	bytes;
int	types = 3;
int	hit;
char	*bufptr;
char	bufout[BUFSIZ];

/*
 *  NAME:  whereis <name>
 *
 *  FUNCTION:  
 * 	look for source, documentation and binaries
 *	      
 */

main(argc, argv)
	int argc;
	char *argv[];
{
	int stop, status; 
	register char *cp;

	(void) setlocale (LC_ALL, "");
	catd = catopen(MF_WHEREIS, 0);
	argc--, argv++;
	stop = 0;
	if (argc == 0) {
		usage();
		status = 1;
	}
	else {
		while(!stop) {
			if (argv[0][0] == '-') {
				cp = argv[0] + 1;
				switch (*cp) {
	
				case 'f':
						stop = 1;
						break;
				case 'S':
						getlist(&argc, &argv, &Sflag, &Scnt);
						break;
				case 'B':
						getlist(&argc, &argv, &Bflag, &Bcnt);
						break;
				case 'M':
						getlist(&argc, &argv, &Mflag, &Mcnt);
						break;
				case 's':
						zerof();
						sflag++;
						types++;
						break;
				case 'u':
						uflag++;
						break;
				case 'b':
						zerof();
						bflag++;
						types++;
						break;
				case 'm':
						zerof();
						mflag++;
						types++;
						break;
				default:
						usage();
						exit(1);
				}
				argc--;
				argv++;
			}
			else
				stop = 1;
		}
		while (argc-- > 0) lookup(*argv++);
		status = 0;
	}
	exit(status);
}

/*
 *  NAME:  usage
 *
 *  FUNCTION:  	If an error in encountered return usage to stdout.
 *
 *  RETURN VALUE:  none, message to stdout. 
 *
 */
usage()
{
	fprintf(stderr, MSGSTR(USAGE, 
"usage: whereis [-ubns] [-BMS directory -f] file...\n")); /*MSG*/
}

/*
 *  NAME:  getlist
 *
 *  FUNCTION:  	If the -B -S -M option is specified, this routine
 *		is called and an alaternate search path will be defined.
 *		This search path is used to locate the specifed file.
 *
 *  RETURN VALUE:  none, path parameter modified.
 *
 */
static void
getlist(argcp, argvp, flagp, cntp)
	char ***argvp;
	int *argcp;
	char ***flagp;
	int *cntp;
{

	(*argvp)++;
	*flagp = *argvp;
	*cntp = 0;
	for ((*argcp)--; 
             *argcp > 0 && **argvp != NULL &&(*argvp)[0][0] != '-'; 
	     (*argcp)--) (*cntp)++, (*argvp)++;
	(*argcp)++;
	(*argvp)--;
	return;
}

/*
 *  NAME:  zerof
 *
 *  FUNCTION:  zero all the option flags
 *
 *  RETURN VALUE:  none
 *
 */
static void
zerof()
{

	if (sflag && bflag && mflag)
		types = sflag = bflag = mflag = 0;
	return;
}

/*
 *  NAME:  lookup
 *
 *  FUNCTION:   Take a filename, strip off path and suffix.  Call 
 *		look{bin,src,man} to search path and print out file
 *		location.
 *
 *  RETURN VALUE:  none, subroutines will print out the pathname.
 *
 */
static void
lookup(cp)
	register char *cp;
{
	register char *dp;

	for (dp = cp; *dp; dp++)
		continue;
	for (; dp > cp; dp--) {
		if (*dp == '.') {
			*dp = 0;
			break;
		}
	}
	for (dp = cp; *dp; dp++)
		if (*dp == '/')
			cp = dp + 1;
	if (uflag) {
		print = 0;
		count = 0;
		sprintf(bufout,"%s:",cp);
		bytes = strlen(cp);
		bufptr = (char *) &bufout[1+bytes];	
	} else
		print = 1;
	hit = 0;
again:
	if (print && !uflag) {
		sprintf(bufout,"%s:",cp);
		bytes = strlen(cp);
		bufptr = (char *) &bufout[(1+bytes)];	
	}
	if (sflag) {
		looksrc(cp);
		if (!uflag && print == 0 && count != 1) {
			print = 1;
			goto again;
		}
	}
	count = 0;
	if (bflag) {
		lookbin(cp);
		if (!uflag && print == 0 && count != 1) {
			print = 1;
			goto again;
		}
	}
	count = 0;
	if (mflag) {
		lookman(cp);
		if (!uflag && print == 0 && count != 1) {
			print = 1;
			goto again;
		}
	}
	if ((print) || (hit != types))
		printf("%s\n",bufout);
	return;
}

/*
 *  NAME:  looksrc
 *
 *  FUNCTION:  Check for the file in the Source directories
 */
static void
looksrc(cp)
	char *cp;
{
	if (Sflag == 0) {
		find(srcdirs, cp);
	} else
		findv(Sflag, Scnt, cp);
	return;
}

/*
 *  NAME:  lookbin
 *
 *  FUNCTION:  Check for the file in the Binary directories
 */
static void
lookbin(cp)
	char *cp;
{
	if (Bflag == 0)
		find(bindirs, cp);
	else
		findv(Bflag, Bcnt, cp);
	return;
}

/*
 *  NAME:  lookman
 *
 *  FUNCTION:  Check for the file in the Man page directories
 */

static void
lookman(cp)
	char *cp;
{
	if (Mflag == 0) {
		find(mandirs, cp);
	} else
		findv(Mflag, Mcnt, cp);
	return;
}

/*
 *  NAME:  findv
 *
 *  FUNCTION:  	Check for file using the given path.  This is an
 *		alternate path.
 */
static void
findv(dirv, dirc, cp)
	char **dirv;
	int dirc;
	char *cp;
{

	while (dirc > 0)
		findin(*dirv++, cp), dirc--;
	return;
}

/*
 *  NAME:  find
 *
 *  FUNCTION:  	Check for file using the given path.  
 */
static void
find(dirs, cp)
	char **dirs;
	char *cp;
{

	while (*dirs)
		findin(*dirs++, cp);
	return;
}

/*
 *  NAME:  findin
 *
 *  FUNCTION:  	Actually check to see if the given file exists (or a like file)
 *		in the directory given.  If so, print it out.
 */
static void
findin(dir, cp)
	char *dir, *cp;
{
	DIR *dirp;
	struct dirent *dp;

	dirp = opendir(dir);
	if (dirp == NULL)
		return;
	while ((dp = readdir(dirp)) != NULL) {
		if (itsit(cp, dp->d_name)) {
			count++;
			if((print) || (uflag)) {
				sprintf(bufptr," %s/%s", dir, dp->d_name);
				bytes += 2+strlen(dir)+strlen(dp->d_name);
				bufptr = (char *) &bufout[1+bytes];	
			}
			if(uflag)
				if(count == 2)
					--hit;
				else	
					++hit;	
		}
	}
	closedir(dirp);
	return;
}

/*
 *  NAME:  itsit
 *
 *  FUNCTION:  	Returns 1 if the current file matches the current directory
 *		entry.  0 if no match.
*/
int
itsit(cp, dp)
	register char *cp, *dp;
{
	register int i = strlen(dp);

	if (dp[0] == 's' && dp[1] == '.' && itsit(cp, dp+2))
		return (1);
	while (*cp && *dp && *cp == *dp)
		cp++, dp++, i--;
	if (*cp == 0 && *dp == 0)
		return (1);
	while (isdigit((int)*dp))
		dp++;
	if (*cp == 0 && *dp++ == '.') {
		--i;
		while (i > 0 && *dp)
			if (--i, *dp++ == '.')
				return (*dp++ == 'C' && *dp++ == 0);
		return (1);
	}
	return (0);
}
