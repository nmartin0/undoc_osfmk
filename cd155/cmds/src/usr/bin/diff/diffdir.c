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
static char rcsid[] = "@(#)$RCSfile: diffdir.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:40 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
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
 * diffdir.c   1.9  com/cmd/files/diff,3.1,9021 5/9/90 15:27:42";
 */

/*
 * diff - directory comparison
 */
#include <stdio.h>
#include <stdlib.h>

#include <NLctype.h>
#define ISDIGIT NCisdigit

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <a.out.h>
#include "diff.h"

extern nl_catd catd;

/*
 * Output format options
 */
extern int	opt;

/*
 * Options on hierarchical diffs.
 */
extern int	lflag;			/* long output format with header */
extern int	rflag;			/* recursively trace directories */
extern int	sflag;			/* announce files which are same */
extern char	*begin;			/* do file only if name >= this */

/*
 * State for exit status.
 */
extern int	status;
extern int oldstatus;
int	anychange;

/*
 * Variables for diffdir.
 */
extern char	**diffargv;	/* option list to pass to recursive diffs */

/*
 * Input file names.
 * With diffdir, file1 and file2 are allocated BUFSIZ space,
 * and padded with a '/', and then efile0 and efile1 point after
 * the '/'.
 */
extern char	*file1, *file2, *efile1, *efile2;
extern struct	stat stb1, stb2;
extern int	done();

#define	ONLY	1		/* Only in this directory */
#define	SAME	2		/* Both places and same */
#define	DIFFER	4		/* Both places and different */
#define	DIRECT	8		/* Directory */
#define INIT	16
#define MAGIC	"/etc/magic"

extern char diffh[], diff[], pr[];
extern	pid_t	fork(), wait();

struct	dirent **setupdir();
int	header;
char	title[2*BUFSIZ], *etitle;

/* 
 * NAME: diffdir
 * FUNCTION: directs diffs between directories
 */
diffdir(argv)
char **argv;
{
	struct dirent **dir1, **dir2;
	int nentries1,nentries2;
	register int i,j;
	int cmp;
	static char blank[] = " ";

	if (opt == DI_IFDEF) {
		fprintf(stderr,MSGSTR(ENOI,
			 "diff: can't specify -D with directories\n"));
		status=2;
		done();
	}
	if (opt == DI_EDIT && (sflag || lflag))
		fprintf(stderr,MSGSTR(ENOSL,
		    "diff: warning: shouldn't give -s or -l with -e\n"));
	title[0] = 0;
	strcpy(title, "diff ");
	for (i = 1; diffargv[i+2]; i++) {
		if (!strcmp(diffargv[i], "-"))
			continue;	/* was -S, dont look silly */
		strcat(title, diffargv[i]);
		strcat(title, " ");
	}
	for (etitle = title; *etitle; etitle++)
		;
	setfile(&file1, &efile1, file1);
	setfile(&file2, &efile2, file2);
	argv[0] = file1;
	argv[1] = file2;
	dir1 = setupdir(file1,&nentries1);
	dir2 = setupdir(file2,&nentries2);
	i = j = 0;
	while (i < nentries1 || j < nentries2) {
		if (i < nentries1 && dir1[i]->d_name[0] && !useless(dir1[i])) {
			i++;
			continue;
		}
		if (j < nentries2 && dir2[j]->d_name[0] && !useless(dir2[j])) {
			j++;
			continue;
		}
		if (i >= nentries1 || dir1[i]->d_name[0] == 0)
			cmp = 1;
		else if (j >= nentries2 || dir2[j]->d_name[0] == 0)
			cmp = -1;
		else
			cmp = strncmp(dir1[i]->d_name,dir2[j]->d_name,NAME_MAX+1);
		if (cmp < 0) {
			if (lflag)
				dir1[i]->d_fileno |= ONLY;
			else if (opt == 0 || opt == 2) {
				only(dir1[i], 1);
				printf(": %.*s\n", NAME_MAX+1, dir1[i]->d_name);
			}
			i++;
		} else if (cmp == 0) {
			compare(dir1[i]);
			i++;
			j++;
		} else {
			if (lflag)
				dir2[j]->d_fileno |= ONLY;
			else if (opt == 0 || opt == 2) {
				only(dir2[j], 2);
				printf(": %.*s\n", NAME_MAX+1, dir2[j]->d_name);
			}
			j++;
		}
	} /* end of while loop */
	if (lflag) {
		scanpr(dir1,ONLY,MSGSTR(EONLY,"Only in %.*s"), file1, efile1,
						nentries1);
		scanpr(dir2,ONLY,MSGSTR(EONLY,"Only in %.*s"), file2, efile2,
						nentries2);
		scanpr(dir1,SAME,MSGSTR(EIDENT,"Common identical files"), 0, 0,
						nentries1);
		scanpr(dir1,DIFFER,MSGSTR(EBINARY,"Binary files which differ")
						, 0, 0, nentries1);
		scanpr(dir1,DIRECT,MSGSTR(ESUBDIR,"Common subdirectories")
						, 0, 0, nentries1);
	}
	if (rflag) {
		if (header && lflag)
			printf("\f");
		for (i = 0; i < nentries1; i++)  {
			if ((dir1[i]->d_fileno & DIRECT) == 0)
				continue;
			strncpy(efile1, dir1[i]->d_name, NAME_MAX+1);
			strncpy(efile2, dir1[i]->d_name, NAME_MAX+1);
			calldiff(0);
		}
	}
}

/*
 * NAME: setfile
 * FUNCTION: finds the end of the path name and adds a / and epp points at
 *   the character following the added /.
 */
setfile(fpp, epp, file)
	char **fpp, **epp;
	char *file;
{
	register char *cp;

	*fpp = malloc((size_t)BUFSIZ);
	if (*fpp == 0) {
		fprintf(stderr,MSGSTR(EMEM, "diff: ran out of memory\n"));
		status=2;
		exit(1);
	}
	strcpy(*fpp, file);
	for (cp = *fpp; *cp; cp++)
		continue;
	*cp++ = '/';
	*epp = cp;
}

/*
 * NAME: scanpr
 * FUNCTION: scan the list of files in a directory and prints out its
 *   name if it passes the test.
 */
scanpr(dp, test, title, file, efile,nentries)
register struct dirent **dp;
int test;
char *title, *file, *efile;
int nentries;
{
	int titled = 0,i;

	for (i=0;i<nentries;i++) {
		if (dp[i]->d_fileno & test) {
			if (titled == 0) {
				if (header == 0) {
					if (anychange)
						printf("\f");
					header = 1;
				} else
					printf("\n");
				printf(title, efile - file - 1, file);
				printf(":\n");
				titled = 1;
			}
			ptname(dp[i]);
		}
	}
}

/*
 * NAME: only
 * FUNCTION: prints outs the message associated with a file that is in only
 *   one of the directories.
 */ 
only(dp, which)
	struct dirent *dp;
	int which;
{
	char *file = which == 1 ? file1 : file2;
	char *efile = which == 1 ? efile1 : efile2;

	printf(MSGSTR(EONLY,"Only in %.*s"),
		 efile - file - 1, file, NAME_MAX+1, dp->d_name);
	oldstatus = status = 1;
}

/*
 * NAME: ptname
 * FUNCTION: prints out the name of a file
 */
ptname(dp)
	struct dirent *dp;
{

	printf("\t%.*s\n", NAME_MAX+1, dp->d_name);
}


/*
 * NAME: setupdir
 * FUNCTION: gets all the valid entires for the directory cp
 */
struct dirent **
setupdir(cp,nentries)
char *cp;
int *nentries;
{
	struct dirent **queue;
	int useless(),alphasort(),i;

	if ((*nentries = scandir(cp,&queue,useless,alphasort)) < 0) {
		perror(cp);
		status=2;
		exit(1);
	}
	if (lflag || rflag)
		for (i = 0; i < *nentries; i++)
			queue[i]->d_fileno = INIT;
	return(queue);
}


/*
 * NAME: compare
 * FUNCTION: compares two files to determine if they are different or the 
 *    same.
 */
compare(dp)
	register struct dirent *dp;
{
	register int i, j;
	int f1, f2, fmt1, fmt2;
	struct stat stb1, stb2;
	int flag = 0;
	char buf1[BUFSIZ], buf2[BUFSIZ];


	strncpy(efile1, dp->d_name, NAME_MAX+1);
	strncpy(efile2, dp->d_name, NAME_MAX+1);
	if (lflag)
		dp->d_fileno = SAME;
	f1 = open(file1, 0);
	if (f1 < 0) {
		perror(file1);
		status=2;
		return;
	}
	f2 = open(file2, 0);
	if (f2 < 0) {
		perror(file2);
		status=2;
		close(f1);
		return;
	}
	fstat(f1, &stb1);
 	fstat(f2, &stb2);
	fmt1 = stb1.st_mode & S_IFMT;
	fmt2 = stb2.st_mode & S_IFMT;
	if (fmt1 != S_IFREG || fmt2 != S_IFREG) {
		if (fmt1 == fmt2) {
			if (fmt1 != S_IFDIR && stb1.st_rdev == stb2.st_rdev)
				goto same;
			if (fmt1 == S_IFDIR) {
				if (slinkcmp(file1, file2, dp)) goto closem;
				if (lflag || opt == DI_EDIT || rflag) {
					dp->d_fileno = DIRECT;
					goto closem;
				}
				printf(MSGSTR(SUBDIR,
					"Common subdirectories: %s and %s\n"),
				    file1, file2);
				goto closem;
			}
		}
		goto notsame;
	}
	if (stb1.st_size != stb2.st_size)
		goto notsame;
	for (;;) {
		i = read(f1, buf1, BUFSIZ);
		j = read(f2, buf2, BUFSIZ);
		if (i < 0 || j < 0 || i != j)
			goto notsame;
		if (i == 0 && j == 0)
			goto same;
		for (j = 0; j < i; j++)
			if (buf1[j] != buf2[j])
				goto notsame;
	}
same:
	if (sflag == 0)
		goto closem;
	if (lflag)
		dp->d_fileno = SAME;
	else
		printf(MSGSTR(FILES,
			"Files %s and %s are identical\n"), file1, file2);
	goto closem;
notsame:
	oldstatus = status=1;
	if (!ascii(f1) || !ascii(f2)) {
		if (lflag)
			dp->d_fileno = DIFFER;
		else if (opt == DI_NORMAL || opt == DI_CONTEXT)
			printf(MSGSTR(DBINARY,"Binary files %s and %s differ\n")
			,file1, file2);
		goto closem;
	}
	if (lflag)
		dp->d_fileno = INIT;
	close(f1); 
	close(f2);
	anychange = 1;
	if (lflag)
		calldiff(title);
	else {
		if (opt == DI_EDIT) {
			printf("ed - %.*s << '-*-END-*-'\n",
			    NAME_MAX+1, dp->d_name);
			calldiff(0);
		} else {
			printf("%s%s %s\n", title, file1, file2);
			calldiff(0);
		}
		if (opt == DI_EDIT)
			printf("w\nq\n-*-END-*-\n");
	}
	return;
closem:
	close(f1); 
	close(f2);
}

char	*prargs[] = { "pr", "-h", 0, "-f", 0, 0 };

/*
 * NAME: calldiff
 * FUNCTION: fork a second process and call diff again 
 */
calldiff(wantpr)
char *wantpr;
{
	int status1, status2, pv[2];
	pid_t pid;

	prargs[2] = wantpr;
	fflush(stdout);
	if (wantpr) {
		sprintf(etitle, "%s %s", file1, file2);
		pipe(pv);
		pid = fork();
		if (pid == -1) {
			perror("diff");
			status=2;
			done();
		}
		if (pid == 0) {
			close(0);
			dup(pv[0]);
			close(pv[0]);
			close(pv[1]);
			execv(pr+4, prargs);
			execv(pr, prargs);
			perror(pr);
			status=2;
			done();
		}
	}
	pid = fork();
	if (pid == -1) {
		perror("diff");
		status=2;
		done();
	}
	if (pid == 0) {
		if (wantpr) {
			close(1);
			dup(pv[1]);
			close(pv[0]);
			close(pv[1]);
		}
		execv(diff+4, diffargv);
		execv(diff+5, diffargv);
		execv(diff, diffargv);
		perror(diff);
		status=2;
		done();
	}
	close(pv[0]);
	close(pv[1]);
	while (wait(&status1) != pid)
		continue;
	while (wait(&status2) != -1)
		continue;
	status = status1 >> 8;
    if (oldstatus < status)
		oldstatus = status;
	else
		status = oldstatus;
/*
	if ((status >> 8) >= 2)
		done();
*/
}

/*
 * NAME: ascii
 * FUNCITON: checks to see if file is ascii
 */
ascii(f)
int f;
{
	char buf[BUFSIZ], cmpbuf[BUFSIZ], s1[BUFSIZ], msg[BUFSIZ];
	char type[40], svalue[40], *tmp;
	FILE *fd;
	int prevoff, offset;
	long nvalue;

	if ((fd = fopen(MAGIC,"r")) == NULL) {
		fprintf(stderr,MSGSTR(DMAGIC,"Can not open %s file\n"),MAGIC);
		return(ascii_bsd(f));
	}
	prevoff = -1;
	while ((fgets(buf,BUFSIZ,fd)) != NULL) {
		if (!ISDIGIT(buf[0]))
			continue;
		sscanf(buf,"%s %s %s %s",s1,type,svalue,msg);
		offset = strtol(s1,&tmp,0);
		if (strcmp("short",type) != 0 &&
		    strcmp("long",type) != 0 &&
		    strcmp("byte",type) != 0 )
			continue;
		nvalue = strtol(svalue,&tmp,0);
		if (offset != prevoff) {
			lseek(f, (long) offset, 0);
			bzero(cmpbuf, BUFSIZ);
			read(f, cmpbuf, BUFSIZ);
			prevoff = offset;
		}
		if (strcmp("short",type) == 0) {
			if (nvalue != (long)(*(unsigned short *) cmpbuf))
				continue;
		} else if (strcmp("long",type) == 0) {
			if (nvalue != (*(long *) cmpbuf))
				continue;
		} else if (nvalue != (long)(*(unsigned char *) cmpbuf))
			continue;
		(void) fclose(fd);
		return(0);
	}
	(void) fclose(fd);
	return (1);
}


/*#include <a.out.h>*/

#ifdef	multimax
#define	BADMAG(X) (X.f_magic != NS32GMAGIC && X.f_magic != NS32SMAGIC)
#ifdef	MACHO
#define	COFF	1
#endif
#endif	/* multimax */

#ifdef	mips
#define	BADMAG(X) (!(ISCOFF(X.f_magic)))
#ifdef	MACHO
#define	COFF	1
#endif
#endif	/* mips */

#ifdef	i386
#ifdef	PS2
#define BADMAG(X) ((X.f_magic != I386MAGIC) && (X.f_magic != COFF386MAGIC))
#ifdef	MACHO
#define	COFF	1
#endif
#endif  /* PS2 */
#endif	/* i386 */

#ifdef	i860
#define BADMAG(X) (X.f_magic != I860MAGIC)
#define	COFF	1
#endif	/* i860 */

ascii_bsd(f)
	int f;
{
	char buf[BUFSIZ];
	register int cnt;
	register char *cp;

	lseek(f, (long)0, 0);
	cnt = read(f, buf, BUFSIZ);
#ifdef	COFF
	if (cnt >= sizeof (struct filehdr)) {
		struct filehdr hdr;
		hdr = *(struct filehdr *)buf;
		if (!BADMAG(hdr))
#else	/* COFF */
	if (cnt >= sizeof (struct exec)) {
		struct exec hdr;
		hdr = *(struct exec *)buf;
		if (!N_BADMAG(hdr))
#endif	/* COFF */
			return (0);
	}
	cp = buf;
	while (--cnt >= 0)
		if (*cp++ & 0200)
			return (0);
	return (1);
}

/*
 * NAME: useless
 * FUNCTION: checks to see if a file should be included in current list
 *  of files for the current directory.
 */
useless(cp)
struct dirent *cp;
{

	if (cp->d_name[0] == '.') {
		if (cp->d_name[1] == '\0')
			return (0);	/* directory "." */
		if (cp->d_name[1] == '.' && cp->d_name[2] == '\0')
			return (0);	/* directory ".." */
	}
	if (begin && strcmp(begin, cp->d_name) > 0)
		return (0);
	return (1);
}

/* 
 * NAME: slinkcmp
 * FUNCTION: trace symbolic links, determine if link to other directory.
 */
slinkcmp(file1, file2, dp)
char *file1, *file2;
struct dirent *dp;
{
	int sfmt1, sfmt2;
	struct stat symb1, symb2;

	lstat(file1, &symb1); lstat(file2, &symb2);
	sfmt1 = symb1.st_mode & S_IFMT;
	sfmt2 = symb2.st_mode & S_IFMT;
	if (sfmt1 == sfmt2 && sfmt1 == S_IFLNK) {
		char sbuf1[NAME_MAX+1 + 1], sbuf2[NAME_MAX+1 + 1];
		if (sfmt1 == S_IFLNK) readlink(file1, sbuf1, NAME_MAX+1);
		if (sfmt2 == S_IFLNK) readlink(file2, sbuf2, NAME_MAX+1);
		if( (strcmp(sbuf1,".") == 0 || strcmp(sbuf1,"..") == 0) &&
		    (strcmp(sbuf2,".") == 0 || strcmp(sbuf2,"..") == 0)) {
			printf(MSGSTR(SUBARE,"Common subdirectories are "));
			printf(MSGSTR(SYMLINK,"symbolic link files:\n\t"));
			if (strcmp(sbuf1,sbuf2) == 0) {
				if (lflag)
					dp->d_fileno = SAME;
				else {
					printf(MSGSTR(IDENT,
					   "%s and %s are identical\n"),
					   file1, file2);
				}
			} else {
				if (lflag)
					dp->d_fileno = DIFFER;
				else {
				   printf("%s --> %s ",
				     file1, sbuf1);
				   printf(MSGSTR(DDIFFER2,
				     " and %s --> %s differ\n"),file2, sbuf2);
				}
			}
			return(1);
		 }
	}
	return(0);
}
