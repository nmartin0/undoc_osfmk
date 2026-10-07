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
 * OSF/1 Release 1.0.3
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: cut.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:22:07 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: cut
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
 * cut.c	1.9  com/cmd/files,3.1,9013 2/13/90 11:22:55
 *
 */
/* cut : cut and paste columns of a table (projection of a relation) */
/* Release 1.5; handles single backspaces as produced by nroff    */

# include <stdio.h>
# include <locale.h>
# include <sys/limits.h>
# include <NLchar.h>
# include "cut_msg.h"

nl_catd	catd;
#define MSGSTR(Num, Str) catgets(catd,MS_CUT, Num, Str)

# define NFIELDS 512	/* max no of fields or resulting line length */
# define BACKSPACE 8

/*
 * NAME: cut -clist [file1 file2 ...]
 *       cut -flist [-dchar] [-s] [file1 file2 ...]
 *                                                                    
 * FUNCTION: Writes out selected fields form each line of a file.
 *                                                                    
 * NOTES:
 *     You must specify either the -c or -f flag.
 *      -clist   specifies the character postions that cut writes out.
 *      -flist   specifies the field postions that cut writes out.
 *         -dchar   used with -flist, char denotes a delimiter between fields.
 *         -s       used with -flist, suppresses lines with out delimiters.
 */  
main(argc, argv)
int argc; char **argv;
{
	int del = '\t';            /* delimiter */
	int i, j, count, poscnt, r, s, t;
	int endflag, supflag, cflag, fflag, backflag, filenr;
	int sel[NFIELDS];
	int c;
	int lc;
	int cnls;
	char *p1;
	char *p2, outbuf[NFIELDS];
	char *temp;
	FILE *inptr;

	/* make sure the sel array is initialized */
	for( r = 0; r < NFIELDS; r++ ) sel[r] = 0;

	endflag = supflag = cflag = fflag = 0;

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_CUT,0);
 
while (argc > 1 && argv[1][0] == '-'){         /* get options */
	for (i = 1; (c = argv[1][i]) != '\0'; i++) {
		switch(c) {
			case 'd' : del = argv[1][++i];     /* get delimiter */
				if (del == '\0') {
				   if ( argv[2] )          /* In next arg?  */
					{
					del = *(argv[2]);  /* get it and then */
					argv++;            /* skip over it    */
					argc--;
					i = 0;             /* it's in 0 postn */
					}
				   if ( del == '\0') diag(
				   MSGSTR(NODELIMITER, "no delimiter\n")); /*MSG*/
				}

				if (NCisshift(del)) {
					c = argv[1][++i];
					_NCdec2 (del, c, del);
				}
				break;
			case 's': supflag++ ;
				break;
			case 'c': cflag++ ;
				break;
			case 'f': fflag++ ;
				break;
			default : diag(
					MSGSTR(USAGE,"Usage: cut [-s] [-d<char>] {-c<list> | -f<list>} file ...\n")); /*MSG*/
				break;
		}
		if (!endflag && (cflag || fflag)) {
			endflag = 1;
			r = s = t = 0;

			if ( argv[1][2] == '\0')  /* lone -f / -c flag?  */
			   if ( argv[2])          /* following argument? */
				{
				argv++;           /* Yes:  make it list  */
				argc--;
				i = -1;	          /* (-1) start at zero  */
				}

			do {	c = argv[1][++i];
				switch(c) {
					case '-' : if (r) diagl();
						r = 1;
						if (t == 0)  s = 1;
						else {
							s = t; 
							t = 0;
						}
						continue;
					case '\0' :
					case ','  : if (t >= NFIELDS) diagl();
						if (r) { 
							if (t == 0) t = NFIELDS - 1;
							if (t<s) diagl();
							for(j = s; j <= t; j++) sel[j] = 1;
							}
						else sel[t] = (t > 0 ? 1 : 0);
						r = s = t = 0;
						if (c == '\0') {
							i--; 
							break;
						}
						continue;
					default :
						if (c< '0' || c> '9') diagl();
						t = 10*t + c - '0';
						continue;
				}
				for (j = t = 0; j < NFIELDS; j++) t += sel[j];
				if (t == 0) diag(
					MSGSTR(NOFIELDS, "no fields\n")); /*MSG*/
			} while (c != '\0');
		}
	}
	--argc;
	++argv;
} /* end options */
if (!(cflag || fflag)) diagl();  /* if niether c or f flag was used exit */

--argc;
filenr = 1;
do {	/* for all input files */
	if (argc > 0) inptr = fopen(argv[filenr], "r"); /* if file open */
	else inptr = stdin;
  
	if (inptr == NULL) {
		fprintf(stderr,
		MSGSTR(CANTOPEN, "cut : cannot open : %s\n"), /*MSG*/
		argv[filenr]);
		exit (1);
	}
	endflag = 0;
	do {	/* for all lines of a file */
		count = poscnt = backflag = 0;
		p1 = &outbuf[0] - 1 ;
		p2 = p1;
		lc = 0;
		do { 	/* for all char of the line */
			c = fgetc(inptr);
			if (c == EOF) {
				endflag = 1;
				break;
				}
			if (count == NFIELDS - 1) 
				diag(
				MSGSTR(LONGLINE, "line too long\n")); /*MSG*/
			if (c != '\n') {
				*++p1 = c;
				if (NCisshift (lc))
					_NCdec2 (lc, c, cnls);
				else
					cnls = c;
			}
			if (cflag && (c == BACKSPACE)) 
				backflag++ ;
			else {
				if (!backflag) {
				 	if (!NCisshift(lc))
					poscnt += 1 ;
				}
				
				else
					backflag-- ;
			}
			if ( backflag > 1 ) 
				diag(
				MSGSTR(BCKSPC,"cannot handle multiple adjacent backspaces\n")); /*MSG*/
			if ( ((c == '\n') && count > 0)  || cnls == del || cflag) {
				count += 1;
				if (fflag) poscnt = count  ;
				if (sel[poscnt]) p2 = p1; else p1 = p2;
			}
		lc = c;
		}while (c != '\n');
		if ( !endflag && (count > 0 || !supflag)) {
			temp = p1 - 1;
			if ((temp >= &outbuf[0]) && (*p1 & 0x80) &&
				(NCisshift(*temp))) {
				cnls = NCdechr (temp);
				if (cnls == del)
					*--p1 = '\0';
				else
					*++p1 = '\0';
			}
			else
			if ( fflag && (*p1 == del))
				*p1 = '\0';
			else
				*++p1 = '\0';
			puts(outbuf);
		}
	} while (!endflag) ;
fclose(inptr);
} while(++filenr <= argc);
exit(0);
}

/*
 * NAME: diag
 *                                                                    
 * FUNCTION: Display error message and then exit.
 */  
diag(s)
char *s;
{
	write(2, "cut : ", 6);
	while(*s)
		write(2,s++,1);
	exit(2);
}

/*
 * NAME: diagl
 *                                                                    
 * FUNCTION: Send error massage to diag.
 */  
diagl()
{
diag( MSGSTR(BADLIST, "bad list for c/f option\n")); /*MSG*/
}
