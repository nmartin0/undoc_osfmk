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
static char rcsid[] = "@(#)$RCSfile: cmp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:20:36 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: cmp
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
 * cmp.c	1.10  com/cmd/files,3.1,9021 3/28/90 14:40:56
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<locale.h>
#include "cmp_msg.h"
#define	MSGSTR(Num, Str) NLgetamsg(MF_CMP,MS_CMP,Num,Str)

FILE	*file1,*file2;              /* the files to be compared */

char	*arg;                       /* argument holder */

int	eflg = 0;                   /* difference flag */
int	lflg = 1;                   /* long output flag */
int	sflg = 0;                   /* short output flag */

long	line = 1;                   /* line number */
long	chr = 0;                    /* char number */

/*
 * NAME: cmp [-l] [-s] file1 file2 
 *                                                                    
 * FUNCTION: Compares two files
 *                                                                    
 * NOTES:   Compares two files sending the differences to standard out.
 *          If a '-' is given for file1 then cmp reads from standard in.
 *          The default is that cmp displays nothing if the files are the
 *          same.  If the files differ cmp displays the byte and line number
 *          at which the first difference occurs.
 *          -l    Displays, for each difference, the byte number in
 *                decimal and the differing bytes in octal.
 *          -s    Returns only an exit value.
 *
 * RETURN VALUE DESCRIPTION: 
 *          0  indicates identical files.
 *          1  indicates different files.
 *          2  indicates inaccessible files or a missing argument.
 */  
main(argc, argv)
int argc;
char **argv;
{
	extern char 	*optarg;
	extern int	optind;
	int 	ch;
	int c1, c2;           /* current charcters from file1 and file2 */

	(void) setlocale(LC_ALL,"");
	while ((ch = getopt(argc, argv, "ls")) != EOF)
		switch(ch) {
		case 'l':		/* print all differences */
			lflg++;
			break;
		case 's':		/* silent run */
			sflg++;
			break;
		case '?':
		default:
			narg();
		}
	argv += optind;
	argc -= optind;

	if (argc != 2 )
		narg();
	arg = argv[0];
	if( arg[0]=='-' && arg[1]==0 )               /* file1 from stdin */
		file1 = stdin;
	else if((file1 = fopen(arg, "r")) == NULL)
		barg();                                      /* bad file */
	arg = argv[1];
	if((file2 = fopen(arg, "r")) == NULL)
		barg();                                      /* bad file */
	while(1) {                                  /* compare the files */
		chr++;
		c1 = getc(file1);
		c2 = getc(file2);
		if(c1 == c2) {
			if (c1 == '\n')
				line++;
			if(c1 == EOF) {
				if(eflg)
					exit(1);
				exit(0);
			}
		}
		else {
			if(sflg > 0)
				exit(1);
			if(c1 == EOF) {
				arg = argv[0];
				earg();
			}
			if(c2 == EOF)
				earg();
			if(lflg == 1) {
				printf(MSGSTR(DIFF,			/*MSG*/
				 "%s %s differ: byte %ld, line %ld\n"),
						argv[0], arg, chr, line);/*MSG*/
			exit(1);
			}
			eflg = 1;
			printf("%6ld %3o %3o\n", chr, c1, c2);
		}
	} /* end of while */
}

/*
 * NAME: narg
 *                                                                    
 * FUNCTION:  print out usage statement and exit program.
 */  
narg()
{
	fprintf(stderr,MSGSTR(USG,"usage: cmp [-l] [-s] file1 file2\n"));/*MSG*/
	exit(2);
}

/*
 * NAME: barg
 *                                                                    
 * FUNCTION: print out error message and exit program.
 */  
barg()
{
	if (sflg == 0)
		fprintf(stderr, MSGSTR(NOTOPEN,				/*MSG*/
					"cmp: cannot open %s\n"), arg);	/*MSG*/
	exit(2);
}

/*
 * NAME: earg
 *                                                                    
 * FUNCTION: file arg was shorter than the other file, print message
 *           and exit.
 */  
earg()
{
	fprintf(stderr, MSGSTR(EOFMSG,"cmp: EOF on %s\n"), arg);	/*MSG*/
	exit(1);
}
