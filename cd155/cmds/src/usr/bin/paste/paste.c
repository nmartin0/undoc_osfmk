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
static char rcsid[] = "@(#)$RCSfile: paste.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:32:58 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * paste.c	1.8  com/cmd/files,3.1,9008 11/18/89 12:41:40
 */

#include <sys/limits.h>
#include "paste_msg.h"
#include <stdio.h>
#include <locale.h>
#include <NLchar.h>

#define MSGSTR(id,ds) catgets(catd, MS_PASTE, id, ds)
static nl_catd catd;

#define MAXOPNF 12  	/* maximal no. of open files (not with -s option) */
#define RUB  '\177'
char del[PATH_MAX] = {"\t"};
  
/*
 * NAME: paste file1 file2 ...
 *       paste -dlist file1 file2 ...
 *       paste -s [-dlist] file1 ...
 *                                                                    
 * FUNCTION: Merges the lines of several files or subsequent lines in 
 *           one file.
 *                                                                    
 * NOTES:  Reads standard input if - is given a file.
 *         Default is to treat each file as a column and join them
 *         horizontally.
 *         -dlist    list of delimiters to separate fields
 *         -s        merges subsequent lines from the first file
 *                   horizontally
 *
 * DATA STRUCTURES: Effects on global data structures, similar to NOTES.
 *
 * RETURN VALUE DESCRIPTION: What this code returns (NONE, if it has no 
 *			     return value)
 */  
main(argc, argv)
int argc;
char **argv;
{
	int i, j, k, eofcount, nfiles, maxline, glue;
	int delcount = { 1 } ;
	int onefile  = { 0 } ;
	int c ;
	char outbuf[PATH_MAX], l, t;
	char *p;
	FILE *inptr[MAXOPNF];
  
	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_PASTE, 0);

	maxline = PATH_MAX -2;
 
	while (argc > 1 && argv[1][0] == '-' && (c = argv[1][1]) != '\0'){
		switch (c) {
			case 's' :  onefile++;
				c = argv[1][2];
				argv[1]++;
				break;
			case 'd' : argv[1] += 2;
				if((delcount = move(argv[1], &del[0])) == 0)  diag(MSGSTR(M_NODEL,"no delimiters\n"),1);
				break;
			default :
				diag(MSGSTR(M_USAGE, "Usage: paste [-s] [-d<delimiterstring>] file1 file2 ...\n"), 1);
				break;
		}
		--argc;
		++argv;
	} /* end options */
	--argc;
 
	if ( ! onefile) {	/* not -s option: parallel line merging */
		for (i = 0; argc >0 && i < MAXOPNF; i++) {
			if (argv[i + 1][0] == '-') {
				inptr[i] = stdin;
			} else inptr[i] = fopen(argv[i + 1], "r");
			if (inptr[i] == NULL) {
				diag(argv[i + 1], 0);
				diag(MSGSTR(M_NOPEN, " : cannot open\n"), 1);
			}
		argc--;
		}
		if (argc > 0) diag(MSGSTR(M_2MANY, "too many files\n"),1);
		nfiles = i;
  
		do {
			p = &outbuf[0];
			eofcount = 0;
			j = k = 0;
			for (i = 0; i < nfiles; i++) {
				while((c = getc(inptr[i])) != '\n' && c != EOF)   {
					if (++j <= maxline) *p++ = c ;
					else {
					diag(MSGSTR(M_2LONG, "line too long\n"),1);
					}
				}
				if ((l = del[k]) != RUB) {

				    /* The following line is to reduce the */
				    /* occurrence of */
				    /* the bug of output buffer overflow */
				    /* because the counter was not being */
				    /* incremented when a delimiter character */
				    /* is added to the output buffer. */
				    j++;	
				    *p++ = l;
				    /* Test for a shift byte. */
				    if (NCisshift(l)) {

					/* Move the second byte of the two- */
					/* byte NLS delimiter character. */
					k = (k + 1) % delcount;
					j++;
					*p++ = del[k];
				    } /* end if */
				} /* end if */
				k = (k + 1) % delcount;
				if( c == EOF) eofcount++;
			}
                        if(NCisshift(l)){ p-=2;  *p = '\n';}
			else if (l != RUB) *--p = '\n'; 
                        else  *p = '\n';
			*++p = 0;
			if (eofcount < nfiles) fputs(outbuf, stdout);
		}while (eofcount < nfiles);
  
	} else {	/* -s option: serial file pasting (old 127 paste command) */
		p = &outbuf[0];
		glue = 0;
		j = 0;
		k = 0;
		t = 0;
		for (i = 1; i <= argc; i++) {
			glue = 0;			/* IBM bug fix */
			if (argv[i][0] == '-') {
				inptr[0] = stdin;
			} else inptr[0] = fopen(argv[i], "r");
			if (inptr[0] == NULL) {
				diag(argv[i], 0);
				diag(MSGSTR(M_NOPEN, " : cannot open\n"), 1);
			}
	  
			while((c = getc(inptr[0])) != EOF)   {
				if (j >= maxline) {
					t = *--p;
					*++p = 0;
					fputs(outbuf, stdout);
					p = &outbuf[0];
					j = 0;
				}
				if (glue) {
					glue = 0;
					l = del[k];
					if (l != RUB) {
						*p++ = l ;
						t = l ;
						j++;
				        /* Test for a shift byte. */
				        if (NCisshift(l)) {

					  /* Move the second byte of the two- */
					  /* byte NLS delimiter character. */
					    k = (k + 1) % delcount;
				    	    *p++ = del[k];
					    t = del[k];
					    j++;
					} /* end if */
					}
					k = (k + 1) % delcount;
				}
				if(c != '\n') {
					*p++ = c;
					t = c;
					j++;
				} else glue++;
			}
			if (t != '\n') {
				*p++ = '\n';
				j++;
			}
			if (j > 0) {
				*p = 0;
				fputs(outbuf, stdout);
				p = &outbuf[0];		/* IBM bug fix */
			}
		}
	}
	exit(0);	/* all ok */
}

/*
 * NAME: diag
 *                                                                    
 * FUNCTION: Display error message.
 */  
diag(s,r)
char *s;
int r;
{
	static first_time = 1;

	if (first_time)	{
		fputs("paste : ", stderr);
		first_time = 0;
	}
	fputs(s, stderr);
	if(r != 0) exit(r);
}
  
/*
 * NAME: move
 *                                                                    
 * FUNCTION: move one string to another checking for special characters
 *           along the way.
 *
 * RETURN: return the length of the copied string.
 */  
move(from, to)
char *from, *to;
{
int c, i;
	i = 0;
	do {
		c = *from++;
		i++;
		if (c != '\\') *to++ = c;
		else { c = *from++;
			switch (c) {
				case '0' : *to++ = RUB;
						break;
				case 't' : *to++ = '\t';
						break;
				case 'n' : *to++ = '\n';
						break;
				default  : *to++ = c;
						break;
			}
		}
	} while (c) ;
return(--i);
}
