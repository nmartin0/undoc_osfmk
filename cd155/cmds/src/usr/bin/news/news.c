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
static char rcsid[] = "@(#)$RCSfile: news.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:48 $";
#endif
/*
 * COMPONENT_NAME: (CMDCOMM) user to user communication
 *
 * FUNCTIONS: news
 *
 * ORIGINS: 3 27
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
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <sys/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <signal.h>
#include <dirent.h>
#include <pwd.h>

#include "news_msg.h"

nl_catd	catd;
#define MSGSTR(Num, Str) catgets(catd, MS_NEWS, Num, Str)

/* The number of leading spaces on each line of output */
#define INDENT 3

/*
 *	The following items should not be printed.
 */
char *ignore[] = {
	"core",
	NULL
};

struct n_file {
	long n_time;
	char n_name[NAME_MAX+1];
} *n_list;

char NEWS[] = "/usr/news";

int n_count;
char stdbuf[BUFSIZ];

jmp_buf	save_addr;

/*
 * NAME: news [-a] [-n] [-s] [items]
 *
 * FUNCTION:  displays the current news for the system
 *            anyone with rw permission for /usr/news directory can
 *            read the news.
 *	news foo	prints /usr/news/foo
 *	news -a		prints all news items, latest first
 *	news -n		lists names of new items
 *	news -s		tells count of new items only
 *	news		prints items changed since last news
 */
main (argc, argv)
	int argc;
	char **argv;
{
	int print_item(), notify(), count();

	(void ) setlocale(LC_ALL,"");
	catd = catopen(MF_NEWS, 0);
	setbuf (stdout, stdbuf);
	initialize();
	read_dir();
	if (argc <= 1)
		late_news (print_item, 1);
	else if	(	argc == 2 && argv[1][0] == '-'
	    && argv[1][1] != '\0' && argv[1][2] == '\0')
		switch (argv[1][1]) {
		case 'a':
			all_news();
			break;

		case 'n':
			late_news (notify, 0);
			break;

		case 's':
			late_news (count, 0);
			break;

		default:
			fprintf (stderr, MSGSTR(BADOPT,
				"news: bad option %s\n"), argv[1]);
			exit (1);
		}
	else {
		int i;
		for (i=1; i<argc; i++)
			print_item (argv[i]);
	}
	return 0;
}

/*
 * NAME: read_dir
 *
 * FUNCTION:  get the file names and modification dates for the
 *	files in /usr/news into n_list; sort them in reverse by
 *	modification date. We assume /usr/news is the working directory.
 */
read_dir()
{
	struct 	dirent *nf;
	struct stat sbuf;
	char 	fname[NAME_MAX+1];
	DIR 	*fd;
	int i, j;
	char *malloc(), *realloc();

	/* Open the current directory */
	if ((fd = opendir (".")) == NULL) {
		fprintf (stderr, MSGSTR(CANTOPEN, "Cannot open %s\n"), NEWS);
		exit (1);
	}

	/* Read the file names into n_list */
	n_count = 0;
	while ((nf = readdir(fd)) != NULL) {
		strncpy (fname, nf->d_name, NAME_MAX+1);
		if (stat (fname, &sbuf) >= 0
		 && (sbuf.st_mode & S_IFMT) == S_IFREG) {
			register char **p;
			p = ignore;
			while (*p && strncmp (*p, nf->d_name, NAME_MAX+1))
				++p;
			if (!*p) {
				if (n_count++ > 0)
					n_list = (struct n_file *)
						realloc ((char *) n_list,
						(unsigned)
						(sizeof (struct n_file)
						    * n_count));
				else
					n_list = (struct n_file *) malloc
						((unsigned)
						(sizeof (struct n_file) *
						n_count));
				if (n_list == NULL) {
					fprintf (stderr, MSGSTR(NOSTRG,
						"No storage\n"));
					exit (1);
				}
				n_list[n_count-1].n_time = sbuf.st_mtime;
				strncpy (n_list[n_count-1].n_name,
					nf->d_name, nf->d_namlen);
			}
		}
	}

	/* Sort the elements of n_list in decreasing time order */
	for (i=1; i<n_count; i++)
		for (j=0; j<i; j++)
			if (n_list[j].n_time < n_list[i].n_time) {
				struct n_file temp;
				temp = n_list[i];
				n_list[i] = n_list[j];
				n_list[j] = temp;
			}

	/* Clean up */
	closedir (fd);
}

/*
 * NAME: initialize
 *                                                                    
 * FUNCTION: initialize by catching SIGQUIT and change current directory
 *           to news directory
 */  
initialize()
{
	if (signal (SIGQUIT, SIG_IGN) != SIG_IGN)
		signal (SIGQUIT, (void (*)(int))_exit);
	umask ((mode_t)022);
	if (chdir (NEWS) < 0) {
		fprintf (stderr, MSGSTR(CANTCHDIR,
			"Cannot chdir to %s\n"), NEWS);
		exit (1);
	}
}

/*
 * NAME: all_news
 *                                                                    
 * FUNCTION: display all of the news items
 */  
all_news()
{
	int i;

	for (i=0; i<n_count; i++)
		print_item (n_list[i].n_name);
}

/*
 * NAME: print_item
 *                                                                    
 * FUNCTION: print news item
 */  
print_item (f)
	char *f;
{
	FILE *fd;
	char fname[NAME_MAX+1];
	static int firstitem = 1;
	int onintr(void);

	if (f == NULL) {
		return;
	}
	strncpy (fname, f, NAME_MAX+1);
	if ((fd = fopen (fname, "r")) == NULL)
		printf (MSGSTR(OPENERR, "Cannot open %s/%s\n"), NEWS, fname);
	else {
		register int c, ip, op;
		struct stat sbuf;
		char *NLctime();
		struct passwd *pw;

		fstat ((int)fileno(fd), &sbuf);
		if (firstitem) {
			firstitem = 0;
			putchar ('\n');
		}
		if (setjmp(save_addr))
			goto finish;
		if (signal(SIGINT, SIG_IGN) != SIG_IGN)
			signal(SIGINT, (void (*)(int))onintr);
		printf ("%s ", fname);
		pw = getpwuid (sbuf.st_uid);
		if (pw)
			printf ("(%s)", pw->pw_name);
		else
			printf (".....");
		printf (" %s\n", NLctime(&sbuf.st_mtime));
		op = 0;
		ip = INDENT;
		while ((c = getc (fd)) != EOF) {
			switch (c) {

			case '\r':
			case '\n':
				putchar (c);
				op = 0;
				ip = INDENT;
				break;

			case ' ':
				ip++;
				break;

			case '\b':
				if (ip > INDENT)
					ip--;
				break;

			case '\t':
				ip = ((ip - INDENT + 8) & -8) + INDENT;
				break;

			default:
				while (ip < op) {
					putchar ('\b');
					op--;
				}
				while ((ip & -8) > (op & -8)) {
					putchar ('\t');
					op = (op + 8) & -8;
				}
				while (ip > op) {
					putchar (' ');
					op++;
				}
				putchar (c);
				ip++;
				op++;
				break;
			}
		}
		fflush (stdout);
finish:
		putchar ('\n');
		fclose (fd);
		if (signal(SIGINT, SIG_IGN) != SIG_IGN)
			signal(SIGINT, SIG_DFL);
	}
}

/*
 * NAME: late_news
 *                                                                    
 * FUNCTION: display the news not read yet
 */  
late_news (emit, update)
	int (*emit)(), update;
{
	long cutoff;
	int i;
	char fname[50], *cp;
	struct stat newstime;
	int fd;
	struct {
		long actime, modtime;
	} utb;

	/* Determine the time when last called */
	cp = getenv ("HOME");
	if (cp == NULL) {
		fprintf (stderr, MSGSTR(NOHOME, "Cannot find HOME variable\n"));
		exit (1);
	}
	strcpy (fname, cp);
	strcat (fname, "/");
	strcat (fname, ".news_time");
	cutoff = stat (fname, &newstime) < 0? 0: newstime.st_mtime;

	/* Print the recent items */
	for (i=0; i<n_count && n_list[i].n_time > cutoff; i++)
		(*emit) (n_list[i].n_name);
	(*emit) ((char *) NULL);
	fflush (stdout);

	if (update) {
		/* Re-create the file and refresh the update time */
		if (n_count > 0 && (fd = creat (fname, 0666)) >= 0) {
			utb.actime = utb.modtime = n_list[0].n_time;
			close (fd);
			utime (fname, &utb);
		}
	}
}

/*
 * NAME: notify
 *                                                                    
 * FUNCTION: notify user of s news item
 */  
notify (s)
	char *s;
{
	static int first = 1;

	if (s) {
		if (first) {
			first = 0;
			printf (MSGSTR(NEWSMSG, "news:"), NEWS);
		}
		printf (" %.14s", s);
	} else if (!first)
		putchar ('\n');
}

/*
 * NAME: count
 *                                                                    
 * FUNCTION: display the number of news intems unread
 */  
/*ARGSUSED*/
count (s)
	char *s;
{
	static int nitems = 0;

	if (s)
		nitems++;
	else if (nitems) {
		if (nitems == 1)
			printf (MSGSTR(NEWSITM, "%d news item.\n"), nitems);
		else
			printf (MSGSTR(NEWSITMS, "%d news items.\n"), nitems);
	}

}

/*
 * NAME: onintr
 *                                                                    
 * FUNCTION: on interrupt pause  for 2 seconds the continue
 */  
onintr(void)
{
	sleep((unsigned)2);
	longjmp(save_addr, 1);
}
