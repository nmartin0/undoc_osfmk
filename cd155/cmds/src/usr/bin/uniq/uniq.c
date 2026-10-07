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
static char rcsid[] = "@(#)$RCSfile: uniq.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:44:54 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: uniq
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
 * uniq.c	1.8  com/cmd/files,3.1,9008 11/2/89 16:22:26
 */
#include <stdio.h>
#include <locale.h>
#include <NLctype.h>
#include "uniq_msg.h"

#define MSGSTR(n,s)	catgets(catd,MS_UNIQ,n,s)

nl_catd catd;

int	fields = 0;
int	letters = 0;
int	linec = 0;
char	mode;
int	uniq = 0;
char	*skip();

/*
 * NAME: uniq [-ucd [+n] [-n]] [input [output]]
 *                                                                    
 * FUNCTION: Deletes repeated lines in a file.  Repeated lines must be
 *           in consecutive lines in order for them to be found.
 *         -c     Precedes each output line with the number of times it
 *                appear in the file.
 *         -d     Displays only the repeated lines.
 *         -u     Displays only the unrepeated lines.
 *         -num   Skips over the first num fields.
 *         +num   Skips over the first num characters.
 *                                                                    
 */  
main(argc, argv)
int argc;
char *argv[];
{
	static char b1[1000], b2[1000];
	FILE *temp;

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_UNIQ,0);

	while(argc > 1) {
		if(*argv[1] == '-') {
			if (isdigit((int)argv[1][1]))
				fields = atoi(&argv[1][1]);
			else if (argv[1][2]!='\0' ||
				 strchr("cdu",argv[1][1])==NULL) {
			    printe(MSGSTR(ILLOPT,			/*MSG*/
			    	"uniq: Illegal option %s\n"),argv[1]);	/*MSG*/
			    exit(1);
			}
			else mode = argv[1][1];
			argc--;
			argv++;
			continue;
		}
		if(*argv[1] == '+') {
			letters = atoi(&argv[1][1]);
			argc--;
			argv++;
			continue;
		}
		if ( (temp = fopen(argv[1], "r")) == NULL)
			printe(MSGSTR(OPERR,"cannot open %s\n"), argv[1]); /*MSG*/
		else {  fclose(temp);
			freopen(argv[1], "r", stdin);
		     }
		break;
	}
	if(argc > 2 && freopen(argv[2], "w", stdout) == NULL)
		printe(MSGSTR(CRERR,"cannot create %s\n"), argv[2]);	/*MSG*/

	if(gline(b1))    /* get first line if file is empty exit program */
		exit(0);
	for(;;) {
		linec++;
		if(gline(b2)) {   /* get next line */
			pline(b1);  /* print last line */
			exit(0);
		}
		if(!equal(b1, b2)) {
			pline(b1);     /* print current line */
			linec = 0;
			do {
				linec++;
				if(gline(b1)) {       /* get next line */
					pline(b2);   /* print current line */
					exit(0);
				}
			} while(equal(b1, b2));
			pline(b2);                /* print next line */
			linec = 0;
		}
	}
}

/*
 * NAME: gline
 *                                                                    
 * FUNCTION: get the next line from the file checking for EOF.
 *                                                                    
 * RETURN VALUE DESCRIPTION:
 *         0 - successful
 *         1 - EOF 
 */  
gline(buf)
char buf[];
{
	int c;

	while((c = getchar()) != '\n') {
		if(c == EOF)
			return(1);
		*buf++ = c;
	}
	*buf = 0;
	return(0);
}

/*
 * NAME: pline
 *                                                                    
 * FUNCTION: check mode and print current line if needed.
 */  
pline(buf)
char buf[];
{

	switch(mode) {

	case 'u':
		if(uniq) {
			uniq = 0;
			return;
		}
		break;

	case 'd':
		if(uniq) break;
		return;

	case 'c':
		printf("%4d ", linec);
	}
	uniq = 0;
	fputs(buf, stdout);
	putchar('\n');
}

/*
 * NAME: equal
 *                                                                    
 * FUNCTION: compare two strings, skipping num characters or num fields
 *           first if that is the mode.
 *
 * RETURN VALUE DESCRIPTION: 
 *         0 - if equal
 *         1 - if not equal
 */  
equal(b1, b2)
char b1[], b2[];
{
	char c;

	b1 = skip(b1);
	b2 = skip(b2);
	while((c = *b1++) != 0)
		if(c != *b2++) return(0);
	if(*b2 != 0)
		return(0);
	uniq++;
	return(1);
}

/*
 * NAME: skip
 *                                                                    
 * FUNCTION:  Skip num fields or num characters.
 *
 * RETURN VALUE DESCRIPTION:  return the rest of the string.
 *   
 */  
char *
skip(s)
char *s;
{
	int nf, nl;

	nf = nl = 0;
	while(nf++ < fields) {
		while (NCisspace(NCdechr(s)))
			s += NLchrlen(s);
		while ( !(NCisspace(NCdechr(s)) || *s == 0) )
			s += NLchrlen(s);
	}
	while(nl++ < letters && *s != 0) 
			s += NLchrlen(s);	
	return(s);
}

/*
 * NAME: printe
 *                                                                    
 * FUNCTION: Print error message.
 */  
printe(p,s)
char *p,*s;
{
	fprintf(stderr, p, s);
	exit(1);
}
