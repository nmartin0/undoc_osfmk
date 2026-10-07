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
static char rcsid[] = "@(#)$RCSfile: grep.c,v $ $Revision: 1.4 $ (OSF) $Date: 1995/01/13 10:10:16 $";
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
 * (C) COPYRIGHT International Business Machines Corp. 1989,1990
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
 * grep.c	1.14  com/cmd/scan,3.1,9013 2/21/90 19:22:48
 */

/*
 * grep -- print lines matching (or not matching) a pattern
 *
 *	status returns:
 *		0 - ok, and some matches
 *		1 - ok, but no matches
 *		2 - some error
 */

/*
 * define some macros for [NL]regexp.h
 */

#define INIT	register unsigned char *sp = (void *)instring; /* First arg points to RE string*/
#define GETC()		(*(unsigned char*)sp++)
#define PEEKC()		(*(unsigned char*)sp)
#define UNGETC(c)	(--sp)
#define RETURN(c)	return;
#define ERROR(c)	regerr()


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>
#include <macros.h>
#include <locale.h>
#include <NLchar.h>
#include <NLctype.h>

#include	"grep_msg.h"
nl_catd	catd;
#define MSGSTR(n,s)	catgets(catd,MS_GREP,n,s)
#include <NLregexp.h>

#define	LBSIZE	512
#define ESIZE	(256*5)
#define BSIZE	512
#define BSPACE 5000
#define NUMEXPS 20
#define LINE_SIZE 512		/* max chars on single line  */

long	lnum;			/* current line number       */ 
char	expbuf[ESIZE];		/* Compiled expression ishere*/
int	nflag;			/* Number lines              */
int	iflag;			/* ignore case.              */
int	bflag;			/* display block numbers     */
int	lflag;			/* list file names           */
int	cflag;			/* count line matches        */
int	vflag;			/* the NOT case              */
int	sflag;			/* quiet flag                */
int	qflag;			/* silent flag               */
int	wflag;			/* match word only           */
int	eflag;			/* use next expression       */
int	nfile;			/* how many files to look at */
long	tln;			/* total lines that matched  */
int	nsucc;			/* Return code.  success?    */
long    blkno1;			/* block number              */
int cfsave1;			/* Circumflex.  (beginning)   */
int cfsave2;

/*  The -w flag can be obtained by first looking for the pattern, */
/*  then checking to see if there is a non-alpha character class  */
/*  bordering it or it is at the beginning or end of the line.    */
/*  With these three cases, there are four patterns to match.     */

char wpattern[4] [ESIZE];	/* four special cases */
char wexpbuf[4] [ESIZE];
int wcfsave[4];
char    	*endspace;
char 		*escape();
long    	lnum1;
unsigned        lspace;
int     	pflag;
char 		*regcmp();
char    	*space=0;
char		*stdinstr=NULL;
int		spflag=0;
int 		i;
char    	*terms[NUMEXPS + 1];
static void execute();
static void succeed();

main(argc, argv)
char **argv;
int argc;
{
	register int c;
	register char *arg ;
	int errflg = 0;
	extern int optind;
	extern char *optarg;
	char *pattern;
	char lowercase[LINE_SIZE * 2 + 1];	/* Map upper to lowercase if iflag set*/

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_GREP,0);

	while((c=getopt(argc, argv, "blciynsqvwe:p:")) != EOF) switch(c) {
		case 'v':
			vflag++;
			break;


		case 'i':
		case 'y':
			iflag++;
			break;

		case 'c':
			cflag++;
			break;

		case 'n':
			nflag++;
			break;

		case 'b':
			bflag++;
			break;

		case 's':
			sflag++;
			break;

		case 'q':
			qflag++;
			break;

		case 'l':
			lflag++;
			break;

		case 'w':
			wflag++;
			continue;

		case 'e':
			eflag++;
			pattern = optarg;
			continue;

		case 'p':
			if (pflag >= NUMEXPS)
				errexit(MSGSTR(PARG,			/*MSG*/
				"too many `p' arguments\n"), (char *) NULL);

			if ((terms[pflag] = malloc((size_t)ESIZE)) == NULL)
			{
				perror ("grep:malloc ");
				exit (2);
			}
		/* getopt allows no arguments after a minus argument or a
		   mandatory argument after a minus argument, but no optional
			   argument, so work around this problem */
			if (optarg != argv[optind-1])
				compile(optarg, terms[pflag], (terms[pflag] + ESIZE), (int) '\0');
			else {
				compile("^$", terms[pflag], (terms[pflag] + ESIZE), (int) '\0');
				--optind;
			}
			cfsave1 = circf;
			++pflag;
			if (space == 0)
				if ((space = malloc((size_t)(lspace = BSPACE))) == NULL)
				{
					perror ("grep malloc ");
					exit (2);
				}
			break;

		case '?':
			errflg++;
		}
	argc -= optind;
	if(errflg || (!eflag && (argc<=0))) errexit(MSGSTR(USAGE,	 /*MSG*/
	 "usage: grep -bcilnqsvw [-e pattern] [-p parasep] pattern file . . .\n"), (char *)NULL);
	if (!eflag)	{
		pattern = argv[optind++];
		nfile = --argc;
	}
	else
		nfile = argc;

	argv = &argv[optind];
	if (strchr(pattern,'\n'))
		regerr();
	if (iflag) {
		NLchar tmp;
		char *result;

		arg = pattern;
		result = lowercase;

		while (*arg != NULL)
		{
			arg += NCdecode (arg, &tmp);
			if (NCisupper((int) tmp))
				tmp = _NCtolower((int) tmp);
			result += NCencode ( &tmp, result);
		}
		*result = '\0';
		pattern = lowercase;
	}
	/* This is what w flag wants but regexp.h doesn't handle it */
	/*
	if (wflag)
		sprintf(wpattern[0],"[[^[:alnum:]]^]%s[[^[:alnum:]]$]",pattern);

	pattern = wpattern[0];
	*/
	compile( pattern,  expbuf,  &expbuf[ESIZE], (int) '\0');
	cfsave2 = circf;
	if (wflag)
	/* The -w case is a little more complicated.  We first check the
	 * line for the simple pattern in the file.  Then check for the
	 * following four cases:
		1) <non-alphabetic character><pattern><non-alpha> e.g. "2dean2"
		2) <beginning of line><pattern><non-alpha>	e.g. "dean2"
		3) <non-alpha><pattern><eol>			e.g. "2dean"
		4) <begin of line><pattern><eol>		e.g. "dean"

	*/
	{
		sprintf (wpattern [0],"[[^[:alnum:]]^]%s[[^[:alnum:]]$]",pattern);
		sprintf (wpattern [0],"[^[:alnum:]]%s[^[:alnum:]]",pattern);
		sprintf (wpattern [1],"^%s[^[:alnum:]]",pattern);
		sprintf (wpattern [2],"[^[:alnum:]]%s$",pattern);
		sprintf (wpattern [3],"^%s$",pattern);

		compile( wpattern[0],  wexpbuf[0],  &wexpbuf[0][ESIZE], (int) '\0');
		wcfsave[0] = circf;
		compile( wpattern[1],  wexpbuf[1],  &wexpbuf[1][ESIZE], (int) '\0');
		wcfsave[1] = circf;
		compile( wpattern[2],  wexpbuf[2],  &wexpbuf[2][ESIZE], (int) '\0');
		wcfsave[2] = circf;
		compile( wpattern[3],  wexpbuf[3],  &wexpbuf[3][ESIZE], (int) '\0');
		wcfsave[3] = circf;
	}
	if (space == 0)
		if ((space = malloc((size_t)(lspace = LINE_SIZE + 1))) == NULL)
		{
			perror ("grep malloc ");
			exit (2);
		}
	endspace = space + lspace - LINE_SIZE;
	if (argc<=0)
		execute((char *)NULL);
	else while (--argc >= 0) {
		execute(*argv++);
	}
	exit(nsucc == 2 ? 2 : nsucc == 0);  /* two means it's an error */
}


/*
 * NAME: execute
 *                                                                    
 * FUNCTION:	For each file in the agument list, search each line
 *		for characters matching the user's pattern.  Multibyte
 *		charcters are converted if the iflag is set.
 *                                                                    
 * RETURN VALUE: void
 *
 */  

static void
execute(file)
char *file;
{
	register char *p1;
	register int c;
	int match;
	int i;
	char *cp;
	int rbflag = bflag;
	FILE *temp;
	int chklline = 0;
	int nlflag = 0;
	char *linebuf,*lbuffer,linebuffer[LINE_SIZE * 2  +  1];

	stdinstr = MSGSTR(STDINSTR, "(standard input)");
	lbuffer=linebuffer;
	if (file == NULL)
		file = stdinstr;
	else {
		if ( (temp = fopen(file, "r")) == NULL) {
			if (!sflag)
				fprintf(stderr,(MSGSTR(OPERR,		/*MSG*/
					"grep: can't open %s\n")), file);
			nsucc = 2;
			return;
			}
		else {  fclose(temp);
			freopen(file, "r", stdin);
		     }
	}
	lnum = 0;
	tln = 0;
	match = 0;
	cp = space;
	lnum1 = 0;
	for (;;) {
next:           if (chklline)
			return;
		lnum++;
		p1 = cp;
		linebuf = lbuffer;
		while (((c = getchar()) != '\n') && (c != '\0')) {
			if (c == EOF) {
				if (pflag && cp != space && (match ^ vflag)
				    && !chklline) 
					succeed(file);
				if (cflag && !chklline) {
					if (!nlflag)
						goto checklast;
					if (nfile>1)
						printf("%s:", file);
					printf("%ld\n", tln);
				}
				/* Force last line with no newline to be checked
				 * for match . If last line has a newline, then
				 * check has already been made.
				 */
				if (chklline++ == 0 && !nlflag)
					goto checklast;
				return;
			}
			nlflag = 0;
			*p1++ = c;
		if (iflag)
		{
		   	NLchar dbl;
			char twoch[2];

			twoch[0] = c;
			twoch[1] = '0';
			if (NCisshift (c))
				*p1++ = twoch[1] = getchar();
			dbl = (NLchar) NCdechr (twoch);
			if (NCisupper((int) dbl))
				dbl = _NCtolower((int) dbl);
			linebuf += NCencode (&dbl, linebuf);
		  }
			else
				*linebuf++ = c;

		  if (p1 - cp >= LINE_SIZE )  /* Test max char limit*/
			  break;
		}
checklast:	nlflag++;
		*p1 = '\0';
		*linebuf = '\0';
		if (lnum1 == 0) {
			lnum1 = lnum;
			if (rbflag)
				blkno1 = (ftell(stdin) - 1) / BSIZE;
		}
		if (pflag) {
			for (i = 0; terms[i]; ++i) {
				circf = cfsave1;
				if (step(lbuffer, terms[i])) {
                                       *cp = 0; *lbuffer = 0;
					if (match ^ vflag && cp != space)
						succeed(file);
					cp = space;
					lnum1 = 0;
					match = 0;
					goto next;
				}
			}
		}
		if (!match) {
		     circf = cfsave2;
		     if (step(lbuffer, expbuf))
		     {
			if (wflag)
				match = match_word (lbuffer);
			else
				match = 1;
		     }
		}
		if (pflag){
			*p1++ = '\n';
			*linebuf = '\n';
		}
		*p1 = 0;
		*linebuf = 0;

		if (!pflag) {
			if (vflag ^ match)
				succeed(file);
			lnum1 = 0;
			match = 0;
		} else {
			cp = p1;
			linebuf = lbuffer;
			if (cp >= endspace) {
				fprintf(stderr,(MSGSTR(OVFLO,		/*MSG*/
					"paragraph space overflow\n")), file);
				*cp = 0; *lbuffer = 0;
				if (match ^ vflag && cp != space)
					succeed(file);
				cp = space;
				lnum1 = 0;
				match = 0;
				goto next;
			}
		}
	}
}

/*
 * NAME: match_word
 *                                                                    
 * FUNCTION: Check to see if the given buffer contains a given
 *    		pattern.  Then -w option searches for a pattern
 *		as if \< \> surrounded the pattern.  regexp does
 *		not handle this correctly, so it is simulated by
 *		four quick pattern checks as outlined in earlier comments.
 *                                                                    
 * RETURN VALUE:
 *		1 a match was found
 *		0 no match.
 */  
int
match_word (lbuffer)
char *lbuffer;
{
	circf = wcfsave[0];		/* case 1 */
	if (!step (lbuffer, wexpbuf[0]))
	{	
		circf = wcfsave[1];	/* case 2 */
		if (!step (lbuffer, wexpbuf[1]))
		{
			circf = wcfsave[2];		/* case 3 */
			if (!step (lbuffer, wexpbuf[2]))
			{
				circf = wcfsave[3];	/* case 4 */
				if (!step (lbuffer, wexpbuf[3]))
					return (0);
			}
		}
	}
	return (1);
}


/*
 * NAME: succeed
 *                                                                    
 * FUNCTION: A match was found.  Now print out all the information
 *		the user requested such as block number, file name,
 *		inc line count (printed at end only), or do nothing.
 *                                                                   
 * (Globals:) 
 *	nsucc is set to one if there hasn't been any previous errors.
 *	tln  is incremented.
 *
 */  

static void
succeed(f)
char *f;
{
	nsucc = (nsucc == 2) ? 2 : 1;
	if (qflag)
		return;
	if (cflag) {
		tln++;
		return;
	}
	if (lflag) {
		if (f == stdinstr) 		/* don't print stdin again */
			if (spflag)
				return;
			else 
				spflag++;
		printf("%s\n", f);
		fseek(stdin, 0l, 2);
		return;
	}
	if (nfile > 1)
		printf("%s:", f);
	if (bflag)
		printf("%ld:", blkno1);
	if (nflag)
		printf("%ld:", lnum1);
	if (pflag && (nfile > 1 || nflag || bflag))
		putchar('\n');
	printf("%s\n", space);
}


/*
 * NAME: regrrr
 *                                                                    
 * FUNCTION: 	Regular expression error was identified.
 *		Error printed and then execution continues.
 *                                                                    
 * RETURN VALUE: 	0
 */  

int 
regerr()
{
	errexit(MSGSTR(REERR,"RE error\n"), (char *)NULL);
	return(0);
}

/*
 * NAME: errexit
 *                                                                    
 * FUNCTION: Error printed and program terminates.
 *                                                                    
 * RETURN VALUE:  Terminates with exit code 2
 *		
 */  
errexit(s, f)
char *s, *f;
{
	fprintf(stderr, s, f);
	exit(2);
}
