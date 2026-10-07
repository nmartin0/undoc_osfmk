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
static char rcsid[] = "@(#)$RCSfile: cat.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:20:00 $";
#endif
/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 *
 * cat.c  1.13  com/cmd/scan,3.1,9021 12/21/89 12:49:17
 */

#include	<stdio.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include 	<locale.h>

#define 	BUFSIZE BUFSIZ	  /* BUFSIZ buffer for all others */

#if !defined(STANDALONE)
#include	<nl_types.h>
#include	"cat_msg.h"
#define MSGSTR(num,str) catgets(catd,MS_CAT,num,str)  /*MSG*/
	nl_catd catd;
#else
#define MSGSTR(num,str) str
#endif
char	buffer[BUFSIZE];
 
int	silent = 0;
int	bflag = 0;			/* Don't count blank lines.  */
int	eflag = 0;			/* Put $ at the end of lines */
int	nflag = 0;			/* Number lines.             */
int	sflag = 0;			/* Squeeze blank lines to one*/
int	tflag = 0;			/* display tabs as ^I.       */
int	vflag = 0;			/* display all control chars.*/

int	spaced = 0;
int	nline = 0;
static void copyopt();

/*
 * There are just a few changes to make cat faster under certain
 * circumstances. The standard code (scat) is called, if the option
 * are used or input to cat is stdin. Switching between standard 
 * and the fast code is done by 'main()'!                           
 */
 

/*
 * NAME: fcat
 *                                                                    
 * FUNCTION:
 *		Faster cat when no options are specified (except
 *		-s for silent mode).
 *                                                                    
 * RETURN VALUE DESCRIPTION: 
 *
 *		Exits with status 0 if successful, 2 if failure.
 */  

static void
fcat(argc,argv)
int argc;
char **argv;
{
        char *file;
        register int cnt, fd, args;
        int status = 0;
        int dev, ino = -1;
        struct stat statb;
	int tmpI;

        if (fstat((int)fileno(stdout), &statb) < 0) {
		if (!silent)
                        fprintf(stderr, MSGSTR(ESTATOUT,"cat: Cannot stat stdout\n"));
          	exit(status = 2);
        }
 
        statb.st_mode &= S_IFMT;
        if (statb.st_mode != S_IFCHR && statb.st_mode != S_IFBLK) {
        	dev = statb.st_dev;
          	ino = statb.st_ino;
        }

        for (args = 1; args < argc; args++) {
                if ((argv[args][0] == '-') && (argv[args][1] == 's')) {
			silent++;
			continue;
		}
                file = argv[args];
                if (((fd = open(file,O_RDONLY)) == -1) && (!silent)) {
			fprintf(stderr,MSGSTR(EOPEN,"cat: cannot open %s\n"),file);
                        status = 2;
                        continue;
                }
                if (fstat(fd, &statb) < 0) {
                	if (!silent)
                                fprintf(stderr,MSGSTR(ESTAT,"cat: cannot stat %s\n"),file);
                      	status = 2;
                      	continue;
                }
                if (statb.st_dev == dev && statb.st_ino == ino) {
                	if (!silent)
                     		fprintf(stderr,MSGSTR(EINISOUT,"cat: input %s is output\n"),file);
                   	if ((close(fd) != 0) && (!silent)) 
                      		fprintf(stderr,"cat: close error\n");
                   	status = 2;
                   	continue;
                }
                while ((cnt = read(fd, buffer, BUFSIZE)) > 0) {
                	if ( cnt != (tmpI = write(fileno(stdout), buffer, cnt)) ) {
				if (!silent) {
					fprintf(stderr,MSGSTR(EOUTPUT,"cat: output error\n"));
					perror("");
				}
				status = 2;
				break;
			}
		}

                if ((close(fd) != 0) && (!silent)) {
                	fprintf(stderr,MSGSTR(ECLOSE,"cat: close error\n"));
                    	status = 2;
                }
        }

	exit(status);
}
 

/*
 * NAME: scat
 *                                                                    
 * FUNCTION: 
 *		Concatenates files together.  If options besides -s are
 *		used, this code will handle it.  Otherwise fcat is called.
 *                                                                    
 * RETURN VALUE DESCRIPTION: 
 *			    
 *		Exit 0 upon successfull completion, two otherwise.
 */  
static void
scat(argc, argv)
int argc;
char **argv;
{
        register FILE *fi;
        register int c;
	extern	int optind;
	int	errflg = 0;
	int	stdinflg = 0;
        int     status = 0;
        int     dev, ino = -1;
        struct  stat statb;
 
#ifdef STANDALONE
        if (argv[0][0] == '\0')
                argc = getargv("cat", &argv, 0);
#endif
	while ((c = getopt(argc, argv, "usrvbnte")) != EOF ) {
		switch (c) {
                	case 'u':
#ifndef STANDALONE
                        	setbuf(stdout, (char *) NULL);
#endif
                        	continue;
			case 'r':		/* replace multiple blank lines with one */
				sflag++;
				continue;
                	case 's':
                        	silent++;
                        	continue;
			case 'v':
				vflag++;
				continue;
			case 'b':
				bflag++;
			case 'n':
				nflag++;
				continue;
			case 't':
				tflag++;
				vflag++;
				continue;
			case 'e':
				eflag++;
				vflag++;
				continue;
			case '?':
				errflg++;
				break;
                }

        	break;
        }

	if (errflg) {
		if (!silent)
			fprintf(stderr,MSGSTR(EUSAGE,"usage: cat [-usr] [-n[b]] [-v[te]] [-|file] ...\n"));
		exit(2);
	}
        if (fstat((int)fileno(stdout), &statb) < 0) {
                if (!silent)
                        fprintf(stderr, MSGSTR(ESTATOUT,"cat: Cannot stat stdout\n"));
                exit(2);
        }
        statb.st_mode &= S_IFMT;
        if (statb.st_mode != S_IFCHR && statb.st_mode != S_IFBLK) {
                dev = statb.st_dev;
                ino = statb.st_ino;
        }
	if (optind == argc) {
		argc++;
		stdinflg++;
        }
	for (argv = &argv[optind];
	     optind < argc && !ferror(stdout); optind++, argv++) {
		if (stdinflg || (*argv)[0] == '-' && (*argv)[1] == '\0')
                        fi = stdin;
                else {
                        if ((fi = fopen(*argv, "r")) == NULL) {
                                if (!silent)
					fprintf(stderr,MSGSTR(EOPEN,"cat: cannot open %s\n"),*argv);
                                status = 2;
                                continue;
                        }
                }
                if (fstat((int)fileno(fi), &statb) < 0) {
                        if (!silent)
                                fprintf(stderr,MSGSTR(ESTAT,"cat: cannot stat %s\n"),*argv);
                        status = 2;
                        continue;
                }
                if (statb.st_dev == dev && statb.st_ino == ino) {
                        if (!silent)
                                fprintf(stderr,MSGSTR(EINISOUT,"cat: input %s is output\n"),
					        stdinflg ? "-" : *argv);
                        if (fclose(fi) != 0 && !silent) 
                                fprintf(stderr, MSGSTR(ECLOSE,"cat: close error\n"));
                        status = 2;
                        continue;
                }

		if (nflag||vflag||sflag)
				copyopt(fi);
		else {
			while ((c = getc(fi)) != EOF) 
				if (putchar(c) == EOF && ferror(stdout)) {
					if (!silent) {
						fprintf(stderr,MSGSTR(EOUTPUT,"cat: output error\n"));
						perror("");
					}
					status = 2; 
					break;
				}
		}

                if (fi != stdin)
                        fflush(stdout);
		if (fclose(fi) != 0) 
			if (!silent)
				fprintf(stderr,MSGSTR(ECLOSE,"cat: close error\n"));
        }

        fflush(stdout);
        if (ferror(stdout)) {
                if (!silent)
                        fprintf(stderr,MSGSTR(EOUTPUT,"cat: output error\n"));
                status = 2;
        }

        exit(status);
}


/*
 * NAME: cat
 *
 * FUNCTION:
 * 	Depending on the option passed, main() switches between the
 * 	standard cat (scat) and the faster version (fcat). 
 *
 * RETURN:
 *	fcat and scat do not return.
 *                                                                    
 */  
main(argc, argv)
int    argc;
char **argv;
{
        int i;

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_CAT, 0);
        for (i = 0; ++i < argc ;)
        	if ((argv[i][0] == '-') && (argv[i][1] == '\0')) {
               		i = -1;
                	break;
            	}

        if ((i != -1) && ((argc >= 2) && (argv[1][0] != '-')) ||
           ((argc > 2) && (argv[1][1] == 's') && (argv[1][2] == '\0') &&
           (argv[2][0] != '-')))
        	fcat(argc, argv);
        else 
            	scat(argc, argv);

	exit(-1);  /* should never get here */
}

/*
 * NAME: copyopt
 *                                                                    
 * FUNCTION: 
 *	Read file f, character by character and copy it to standard output.
 *	Translate special characters and number lines as specified by
 *	flags:
 *		-n	number lines.
 *		-b	don't count spaces.
 *		-r	replace multiple blank lines with only one.
 *		-s	be silent about errors.
 *		-t	print tabs as ^I.
 *		-v	print all control characters.
 *		-e	signify endofline with a $
 *                                                                    
 * RETURN VALUE DESCRIPTION: 
 *		Subroutine, nothing returned.
 */  

static void
copyopt(f)
	register FILE *f;
{
	static lno = 1;
	register int c;

      while ((c=getc(f)) != EOF)
      {
	if (c == '\n') {
		if (nline == 0) {
			if (sflag && spaced)
				continue;
			spaced = 1;
		}
		if (nflag && bflag==0 && nline == 0)
			printf("%6d\t", lno++);
		if (eflag)
			putchar('$');
		putchar('\n');
		nline = 0;
		continue;
	}
	if (nflag && nline == 0)
		printf("%6d\t", lno++);
	nline = 1;
	if (vflag) {
		if (tflag==0 && c == '\t')
			putchar(c);
		else {
			if (iscntrl(c))
				printf("^%c", c=='\177' ? '?': c | 0100);
			else if (isprint(c))
				putchar(c);
			else {
				c = toascii(c);
				if (iscntrl(c))
				    printf("M-^%c", c=='\177' ? '?': c | 0100);
				else
				    printf("M-%c", c);
			}

		}
	} else
		putchar(c);
	spaced = 0;
      }
}
