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
static char rcsid[] = "@(#)$RCSfile: nl.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:32:04 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: nl
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
 * nl.c	1.7  com/cmd/files,3.1,9013 11/2/89 15:14:05
 */
#include <stdio.h>	/* Include Standard Header File */
#include <locale.h>

/*	THE FOLLOWING LINES ARE NEEDED FOR
	THE REGEXP(7) ROUTINE			*/

#define INIT		char *ptr = instring;
#define GETC()		((unsigned char)*ptr++)
#define PEEKC()		((unsigned char)*ptr)
#define UNGETC(c)	((unsigned char*)--ptr)
#define RETURN(c)	return((char *)c);
#define ERROR(c)	regerr(c);
#define EXPSIZ		(128*5) /* size of buff used to compile regexp */
#define CONVSIZ		128  /* size of arg array buffer */
#define PATSIZ		128  /* size of buff used for regexp pattern */
#define SEPSIZ		128  /* buf used for separator and/or argv's */

#include <NLctype.h>
#include <NLregexp.h>
#include "nl_msg.h"

nl_catd catd;

#define MSGSTR(N,S)	catgets(catd,MS_NL,N,S)

char nbuf[100];         /* Declare buf size used in convert/pad/cnt routines */
unsigned char bexpbuf[EXPSIZ];   /* Declare the size of the regexp buf */
unsigned char hexpbuf[EXPSIZ];   /* Declare the size of the regexp buf */
unsigned char fexpbuf[EXPSIZ];   /* Declare the size of the regexp buf */
char s1[CONVSIZ];       /* Declare the conversion array */
char format = 'n';      /* Declare the format of numbers to be rt just */
unsigned char delim1 = '\\';     /* Default delimiters. */
unsigned char delim2 = ':';      /* Default delimiters. */
char pad = ' ';         /* Declare the default pad for numbers */
char *s;                /* Declare the temp array for args */
int width = 6;          /* Declare default width of number */
int k;                  /* Declare var for return of convert */
int r;                  /* Declare the arg array ptr for string args */
int q = 2;              /* Initialize arg pointer to drop 1st 2 chars */
int bsave, hsave, fsave;

/*
 * NAME: nl [-htype] [-btype] [-ftype] [-vstart#] [-iincr] [-p]
 *          [-l num] [-s sep] [-w width] [-n format] [-d delim] file
 *                                                                    
 * FUNCTION:  Numbers lines in a file.  Input must be written in logical
 *     pages.  Each logic page has a header, body and footer section
 *     ( you can have empty sections).  To mark the different sections
 *     the first line of the section must contain delimiters only. Default:
 *     Line contents    Start of
 *     \:\:\:           Header
 *     \:\:             Body
 *     \:               Footer
 *
 *     FLAGS:
 *      -btype      chooses which body section lines to number
 *                  a   number all lines
 *                  t   do not number blank lines (default)
 *                  n   do not number any lines
 *                  ppattern   number only lines containing pattern
 *      -dxx        Usess xx as the delimiter for the start of logical
 *                  page sections.
 *      -ftype      Chooses which logical page footer lines to number
 *                  same types as in -b.
 *      -htype      Chooses which logical page header line to number
 *                  same types as in -b.
 *      -inum       Increments logical page line numbers by num.
 *                  default value of num is 1.
 *      -lnum       num is the number of blank lines to count as one.
 *                  can only be used in documnets where the -ba flag
 *                  is used.
 *      -nformat    Uses format as the line numbering format.
 *                  ln  left justified, leading zeroes suppressed.
 *                  rn  right justified, leading zeroes suppressed. (default)
 *                  rz  right justified, leading zeroes kept.
 *      -p          does not restart numbering a logicial page delimiters.
 *      -s[sep]     separates the text from its line number by the sep
 *                  character. default sep = \t.
 *      -vnum       sets the initial logical page line number to num.
 *      -wnum       uses num as the number of characters in the line number.
 *                  default num=6.
 */  
main(argc,argv)
int argc;
char *argv[];
{
	int j;
	int i = 0;
	unsigned char *p;
	char header = 'n';    /* default line numbering */
	char body = 't';
	char footer = 'n';
	char line[BUFSIZ];
	char tempchr;	/* Temporary holding variable. */
	int temp1;	/* another temp holding variable */
	char swtch = 'n';
	char cntck = 'n';
	char type;
	int cnt;	/* line counter */
	int pass1 = 1;	/* First pass flag. 1=pass1, 0=additional passes. */
	char sep[SEPSIZ];
	unsigned char pat[PATSIZ];
	unsigned char *ptr ;
	int startcnt=1;
	int increment=1;
	int blank=1;
	int blankctr = 0;
	int c;
	char last;
	FILE *iptr=stdin;
	FILE *optr=stdout;
	sep[0] = '\t';
	sep[1] = '\0';

	(void) setlocale(LC_ALL,"");     /* set tables up for current lang */
/*		OPEN MESSAGE CATALOG			*/
	catd = catopen(MF_NL,0);

/*		DO WHILE THERE IS AN ARGUMENT
		CHECK ARG, SET IF GOOD, ERR IF BAD	*/

for (j = 1; j < argc; j++) {
	if (argv[j][i] == '-' && (c = argv[j][i + 1])) {
		switch(c) {
			case 'h':    /* header lines */
				switch(argv[j][i + 2]) {
					case 'n':
						header = 'n';
						break;
					case 't':
						header = 't';
						break;
					case 'a':
						header = 'a';
						break;
					case 'p':
						s=argv[j];
						q=3;
						r=0;
						while (s[q] != '\0'){
							pat[r] = s[q];
							r++;
							q++;
						}
						pat[r] = '\0';
						header = 'h';
						ptr = pat;
					compile((char*)pat, (char*)hexpbuf, (char*)&hexpbuf[EXPSIZ], '\0');
					hsave = circf;
						break;
					case '\0':
						header = 'n';
						break;
					default:
						printf(MSGSTR( HEADER1, "HEADER: ")); /*MSG*/
						optmsg(argv[j]);
				}
				break;
			case 'b':      /* body lines */
				switch(argv[j][i + 2]) {
					case 't':
						body = 't';
						break;
					case 'a':
						body = 'a';
						break;
					case 'n':
						body = 'n';
						break;
					case 'p':
						s=argv[j];
						q=3;
						r=0;
						while (s[q] != '\0'){
							pat[r] = s[q];
							r++;
							q++;
						}
						pat[r] = '\0';
						body = 'b';
						ptr = pat;
						compile((char*)pat, (char*)bexpbuf, (char*)&bexpbuf[EXPSIZ], '\0');
					bsave = circf;
						break;
					case '\0':
						body = 't';
						break;
					default:
						printf(MSGSTR( BODY1, "BODY: ")); /*MSG*/
						optmsg(argv[j]);
				}
				break;
			case 'f':     /* footer lines */
				switch(argv[j][i + 2]) {
					case 'n':
						footer = 'n';
						break;
					case 't':
						footer = 't';
						break;
					case 'a':
						footer = 'a';
						break;
					case 'p':
						s=argv[j];
						q=3;
						r=0;
						while (s[q] != '\0'){
							pat[r] = s[q];
							r++;
							q++;
						}
						pat[r] = '\0';
						footer = 'f';
						ptr = pat;
						compile( (char*)pat, (char*)fexpbuf, (char*)&fexpbuf[EXPSIZ], '\0');
						fsave = circf;
						break;
					case '\0':
						footer = 'n';
						break;
					default:
						printf(MSGSTR( FOOTER1, "FOOTER: ")); /*MSG*/
						optmsg(argv[j]);
				}
				break;
			case 'p':     /* do not restart numbering */
				if (argv[j][i+2] == '\0')
				cntck = 'y';
				else
				{
				optmsg(argv[j]);
				}
				break;
			case 'v':      /* set initial logical page number */
				if (argv[j][i+2] == '\0')
				startcnt = 1;
				else
				startcnt = convert(argv[j]);
				break;
			case 'i':  /* set lin incrementor value */
				if (argv[j][i+2] == '\0')
				increment = 1;
				else
				increment = convert(argv[j]);
				break;
			case 'w': /* set num of spaces used for line number */
				if (argv[j][i+2] == '\0')
				width = 6;
				else
				width = convert(argv[j]);
				break;
			case 'l': /* set num of blank lines to count as one */
				if (argv[j][i+2] == '\0')
				blank = 1;
				else
				blank = convert(argv[j]);
				break;
			case 'n':   /* numbering format */
				switch (argv[j][i+2]) {
					case 'l':
						if (argv[j][i+3] == 'n')
						format = 'l';
						else
							optmsg(argv[j]);
						break;
					case 'r':
						if (argv[j][i+3] == 'n' || argv[j][i+3] == 'z')
						format = argv[j][i+3];
						else
							optmsg(argv[j]);
						break;
					case '\0':
						format = 'n';
						break;
					default:
				optmsg(argv[j]);
					break;
				}
				break;
			case 's':     /* set separator character */
				s = argv[j];
				q = 2;
				r = 0;
				while (s[q] != '\0') {
					sep[r] = s[q];
					r++;
					q++;
				}
				sep[r] = '\0';
				break;
			case 'd': /* set delimiter */
				tempchr = argv[j][i+2];
				if(tempchr == '\0')break;
				delim1 = tempchr;

				tempchr = argv[j][i+3];
				if (NCisshift (delim1)) { 
					/* check for dangling shift */
					_NCdec2 (delim1, tempchr, temp1);
					if (!NCisNLchar(temp1)) optmsg (argv[j]);
				}
				if(tempchr == '\0')break;
				delim2 = tempchr;
				if(argv[j][i+4] != '\0')optmsg(argv[j]);
				break;
			default:
				optmsg(argv[j]);
			}
		continue; /* If it got here, a valid -xx option was found.
				Now, start next pass of FOR loop. */
		}
		else
			if ((iptr = fopen(argv[j],"r")) == NULL)  {
				printf(MSGSTR( CANTOPEN, "CANNOT OPEN FILE %s\n"), argv[j]); /*MSG*/
				exit(1);
			}
}		/* Closing brace of "for" */

	/* ON FIRST PASS ONLY, SET LINE COUNTER (cnt) = startcnt &
		SET DEFAULT BODY TYPE TO NUMBER ALL LINES.	*/
	if(pass1){cnt = startcnt; type = body; last = 'b'; pass1 = 0;}

/*		DO WHILE THERE IS INPUT
		CHECK TO SEE IF LINE IS NUMBERED,
		IF SO, CALCULATE NUM, PRINT NUM,
		THEN OUTPUT SEPERATOR CHAR AND LINE	*/

	while (( p = (unsigned char *)fgets(line,(int)sizeof(line),iptr)) != NULL) {
	if (p[0] == delim1 && p[1] == delim2) {
		if (p[2] == delim1 && p[3] == delim2 && p[4]==delim1 && p[5]==delim2 && p[6] == '\n') {
			if ( cntck != 'y')
				cnt = startcnt;
			type = header;
			last = 'h';
			swtch = 'y';
		}
		else {
			if (p[2] == delim1 && p[3] == delim2 && p[4] == '\n') {
				if ( cntck != 'y' && last != 'h')
				cnt = startcnt;
				type = body;
				last = 'b';
				swtch = 'y';
			}
			else {
				if (p[0] == delim1 && p[1] == delim2 && p[2] == '\n') {
				if ( cntck != 'y' && last == 'f')
				cnt = startcnt;
					type = footer;
					last = 'f';
					swtch = 'y';
				}
			}
		}
	}
	if (swtch == 'y') {
		swtch = 'n';
		fprintf(optr,"\n");
	}
	else {
		switch(type) {    /* check if line is numbered */
			case 'n':
				npad(width,sep);
				break;
			case 't':
				if (p[0] != '\n') {
					pnum(cnt,sep);
					cnt+=increment;
				}
				else {
					npad(width,sep);
				}
				break;
			case 'a':
				if (p[0] == '\n') {
					blankctr++;
					if (blank == blankctr) {
						blankctr = 0;
						pnum(cnt,sep);
						cnt+=increment;
					}
					else npad(width,sep);
				}
				else {
					blankctr = 0;
					pnum(cnt,sep);
					cnt+=increment;
				}
				break;
			case 'b':
				circf = bsave;
				if (step((char*)p, (char*)bexpbuf)) {
					pnum(cnt,sep);
					cnt+=increment;
				}
				else {
					npad(width,sep);
				}
				break;
			case 'h':
				circf = hsave;
				if (step((char*)p, (char*)hexpbuf)) {
					pnum(cnt,sep);
					cnt+=increment;
				}
				else {
					npad(width,sep);
				}
				break;
			case 'f':
				circf = fsave;
				if (step( (char*)p, (char*)fexpbuf)) {
					pnum(cnt,sep);
					cnt+=increment;
				}
				else {
					npad(width,sep);
				}
				break;
		}
		fprintf(optr,"%s",line);

	}	/* Closing brace of "else" (~ line 307). */
	}	/* Closing brace of "while". */
	fclose(iptr);
	exit(0);
}

/*
 * NAME: regerr
 *                                                                    
 * FUNCTION:	REGEXP ERR ROUTINE
 */  
regerr(c)
int c;
{
printf(MSGSTR( REGERR1, "%d This is the error code\n"),c); /*MSG*/
printf(MSGSTR( REGERR2, "Illegal Regular Expression\n")); /*MSG*/
exit(1);
}

/*
 * NAME: pnum
 *                                                                    
 * FUNCTION: convert integer to string.
 */  
pnum(n,sep)
int	n;
char *	sep;
{
	int	i;

		if (format == 'z') {
			pad = '0';
		}
	for ( i = 0; i < width; i++)
		nbuf[i] = pad;
		num(n,width - 1);
	if (format == 'l') {
		while (nbuf[0]==' ') {
			for ( i = 0; i < width; i++)
				nbuf[i] = nbuf[i+1];
			nbuf[width-1] = ' ';
		}
	}
		printf("%s%s",nbuf,sep);
}

/*
 * NAME: procname1
 *                                                                    
 * FUNCTION: 	Convert integer to character.
 *              IF NUM > 10, THEN USE THIS CALCULATE ROUTINE
 */  
num(v,p)
int v,p;
{
	if (v < 10)
		nbuf[p] = v + '0' ;
	else {
		nbuf[p] = (v % 10) + '0' ;
		if (p>0) num(v / 10,p - 1);
	}
}

/*
 * NAME: convert
 *                                                                    
 * FUNCTION:	CONVERT ARG STRINGS TO STRING ARRAYS
 */  
convert(argv)
char **argv;
{
	s = (char*)argv;
	q=2;
	r=0;
	while (s[q] != '\0') {
		if (s[q] >= '0' && s[q] <= '9')
		{
		s1[r] = s[q];
		r++;
		q++;
		}
		else
				{
				optmsg(argv);
				}
	}
	s1[r] = '\0';
	k = atoi(s1);
	return(k);
}

/*
 * NAME: npad
 *                                                                    
 * FUNCTION: 	CALCULATE LENGTH OF NUM/TEXT SEPRATOR
 */  
npad(width,sep)
	int	width;
	char *	sep;
{
	int i;

	pad = ' ';
	for ( i = 0; i < width; i++)
		nbuf[i] = pad;
	printf("%s",nbuf);


	if(sep[0] == '\t')
		printf("\t");
	else
	for(i=0; i < strlen(sep); i++)
		printf(" ");
}

/*
 * NAME: optmsg
 *                                                                    
 * FUNCTION: print option error massage.
 */  
optmsg(option)
char *option;
{
	printf(MSGSTR( OPTINVAL, "INVALID OPTION (%s) - PROCESSING TERMINATED\n"),option); /*MSG*/
	exit(1);
}
