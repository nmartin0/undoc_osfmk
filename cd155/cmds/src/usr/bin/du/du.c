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
static char rcsid[] = "@(#)$RCSfile: du.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:23:02 $";
#endif
/*
 * COMPONENT_NAME: (CMDFS) commands that deal with the file system
 *
 * FUNCTIONS: du
 *
 * ORIGINS: 3, 27
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
 * du.c	1.15 	  com/cmd/fs/progs,3.1,9021 4/4/90 15:18:47";
 */
/*
**	du -- summarize disk usage
**              du [-arsl] [name ...]
*/

#include	<stdio.h>
#include	<string.h>
#include	<sys/limits.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<dirent.h>

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif 
#ifndef NAME_MAX
#define NAME_MAX MAXNAMLEN
#endif

/* #define UBLOCKS(n) (((n.st_size) + UBSIZE - 1)/UBSIZE) */
#define REPORT_BSIZE    UBSIZE
#define UBLOCKS(x)  ((x.st_blocks * S_BLKSIZE ) / REPORT_BSIZE)

#define	NLSKJI 1
#include <NLctype.h>
#include <NLchar.h>

#include <locale.h>

#include <nl_types.h>
#include "du_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_DU,Num,Str)


char    path[PATH_MAX];

#define EQ(x,y)	(strcmp(x,y)==0)
#define ISDIR() ((Statb.st_mode&S_IFMT)==S_IFDIR)
#define ML	500

struct 	{
	dev_t	dev;
	ino_t	ino;
} ml[ML];
int	linkc = 0;

struct	stat	Statb;

char    aflag = 0;
char    rflag = 0;
char    sflag = 0;
char    lflag = 0;

char    nodotdot[] = "du: fatal error - can't cd to .. (%s)\n";


long	descend();

main(argc, argv)
char **argv;
{
	long blocks = 0;
	char userdir[PATH_MAX+2];

	(void) setlocale (LC_ALL,"");
	catd = catopen((char *)MF_DU,0);

#ifdef STANDALONE
	if (argv[0][0] == '\0')
		argc = getargv("du", &argv, 0);
#endif
	--argc;
	while (argv[1] && (argv[1][0] == '-' && argv[1][1] != '\0') ) {
		argv++;
		while(*++*argv)
			switch(**argv) {
			case 'a':
				aflag++;
				continue;

			case 'r':
				rflag++;
				continue;

			case 's':
				sflag++;
				continue;

			case 'l':
				lflag++;
				continue;

			default:
				fprintf(stderr,MSGSTR(USAGE,"usage: du [-arsl] [name ...]\n"));
				exit(2);
			}
		argc--;
	}
	if(argc == 0) {
		argc = 1;
		argv[1] = ".";
	}

	if ((char *)getcwd(userdir,PATH_MAX+2) == NULL) {
		fprintf(stderr,MSGSTR(CANTFIND,"du: can't find current directory\n"));
		exit(2);
	}

	while(argc--) {
		int	len;

		if ((len = strlen(*++argv)) >= sizeof(path)) {
			fprintf(stderr,MSGSTR(NAMETOOLONG,"du: %s: name too long\n"), *argv);
			continue;
		}
		strcpy(path, *argv);

		if(lstat(path,&Statb)<0) {
			error(path);
			continue;
		}

		if (ISDIR() && (chdir(path) < 0)) {
			error(path);
			continue;
		}
		blocks = descend(path + len);
		if(sflag)
			printf("%ld\t%s\n", blocks, path);

		if (chdir(userdir)) {
			error(userdir);
			exit(2);
		}
	}

	exit(0);
}

struct elem {
	struct elem *next;
	char *data;
};

struct list {
	struct elem *first;
	struct elem *last;
};

list_empty(listp)
struct list *listp;
{
	listp->first = listp->last = NULL;
}

list_append(listp, data)
struct list *listp;
char *data;
{
	struct elem *ep;
	int len;
	char *malloc();

	ep = (struct elem *) calloc(1, sizeof(struct elem));
	if (ep == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	ep->data = malloc(len = (strlen(data) + 1));
	bcopy(data, ep->data, len);
	if (listp->first == NULL)
		listp->first = ep;
	else
		listp->last->next = ep;
	listp->last = ep;
}

char *
list_first(listp)
struct list *listp;
{
	struct elem *ep;
	char *data;

	if ((ep = listp->first) == NULL)
		return(NULL);
	listp->first = ep->next;
	data = ep->data;
	free((char *)ep);
	return(data);
}

/* On entry, endofname points to the NUL at the end of path and
 * Statb contains a stat of the file whose full name is in path.
 * If path is a directory, it is the working directory.
 */

long descend(endofname)
char *endofname;
{
	register struct	dirent	*dp;
	register char	*c1, *c2;
	long blocks = 0;
	DIR	*dir;			/* open directory */
	int	i;
	struct list list;
	char *elem;

	blocks = UBLOCKS(Statb);

	if (!ISDIR()) {
		if(Statb.st_nlink > 1) {
			if (lflag) {
				blocks += Statb.st_nlink - 1;
				blocks /= Statb.st_nlink;
			} else if (linkc<ML) {
				for(i = 0; i <= linkc; ++i) {
					if(ml[i].ino==Statb.st_ino &&
					   ml[i].dev==Statb.st_dev)
						return 0;
				}
				ml[linkc].dev = Statb.st_dev;
				ml[linkc].ino = Statb.st_ino;
				++linkc;
			}
		}
		if(aflag)
			printf("%ld\t%s\n", blocks, path);
		return(blocks);
	}

	if (endofname + (NAME_MAX+1) >= &path[sizeof(path)]) {
		fprintf(stderr,MSGSTR(TOODEEP,"du: %s: TOO DEEP!\n"), path);
		return(blocks);
	}

	if ((dir=opendir(".")) == NULL) {
		error(path);
		return(0);
	}
	list_empty(&list);
	while ((dp=readdir(dir)) != NULL) {
		if(dp->d_fileno==0
			|| EQ(dp->d_name, ".") || EQ(dp->d_name, "..")
			|| dp->d_name[0] == 0)      /* ?? */
				continue;
		list_append(&list, dp->d_name);
	}
	closedir(dir);
	while ((elem = list_first(&list)) != NULL) {
		/* each directory entry */
		c1 = endofname;
		*c1++ = '/';
		c2 = c1;
		c1 = strncpy(c1, elem, (size_t)NAME_MAX);
		c1 += strlen(elem);
		free(elem);
		if(lstat(c2, &Statb) < 0) {
			error(path);
			return(0);
		}
		if (ISDIR()) {
			if (chdir(c2)) {
				error(path);
				continue;
			}
			blocks += descend(c1);
			*endofname = '\0';
			if (chdir("..")) {
				fprintf(stderr, MSGSTR(NODOTDOT,nodotdot), path);
				exit(2);
			}
		}
		else blocks += descend(c1);
	}   /* End of loop over entries in a directory */
	*endofname = '\0';
	if(!sflag)
		printf("%ld\t%s\n", blocks, path);
	return(blocks);
}

error(s)
char *s;
{       if (rflag) {
		fprintf(stderr, MSGSTR(DU,"du: "));
		perror(s);
	}
}
