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
static char rcsid[] = "@(#)$RCSfile: split.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:40:12 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: split
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
 * split.c     1.10  com/cmd/files,3.1,9021 3/1/90 15:55:31
 */
#include <stdio.h>
#include <locale.h>
#include <sys/limits.h>
#include <sys/dir.h>

#ifdef MSG
#include "split_msg.h"
#define MSGSTR(Num, Str) NLgetamsg(MF_SPLIT, MS_SPLIT, Num, Str)
#else
#define MSGSTR(Num, Str) Str
#endif

#define LNCNT	1000
#define MAX_OUTF 676
#define NUM_ALPHA 26
char	*strrchr();
unsigned count = LNCNT;
int	fnumber;
char	fname[PATH_MAX];
char	*ifil;
char	*ofil;
char	*tail;
FILE	*is;
FILE	*os;

/*
 * NAME: split [-num] [file [name]]
 *                                                                    
 * FUNCTION: Splits a file into pieces.  Default line count = 1000
 *           -num   changes the number of lines that go into each file
 *            name  prefix name not longer than NAME_MAX - 2 characters.
 * 
 * RETURN VALUES:  0 if no errors
 *                 1 if error
 */  
main(argc, argv)
char *argv[];
{
	int i, c, f;
	int iflg = 0;

	(void) setlocale (LC_ALL,"");
	for(i=1; i<argc; i++)      /* get arguments */
		if(argv[i][0] == '-')
			switch(argv[i][1]) {
		
			case '\0':
				iflg = 1;
				continue;
		
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':          /* get number of lines */
				count = atoi(argv[i]+1);
				continue;
			case '0':
				count = atoi(argv[i]+1);
				if (count != 0) /* can't have 0 lines in file */
					continue;
				/* FALL THROUGH */
			default:
				fprintf(stderr,MSGSTR(USAGE, "usage: split [-num] [file [name]]\n"));
				exit(1);
			
			}
		else if(iflg)          /* get prefix */
			ofil = argv[i];
		else {                /* get file to be split */
			ifil = argv[i];
			iflg = 2;
		}
	if(iflg != 2)
		is = stdin;
	else          /* open file */
		if((is=fopen(ifil,"r")) == NULL) {
			fprintf(stderr,MSGSTR(INPOPNER, "cannot open input file\n"));
			exit(1);
		}
	if(ofil == 0)
		ofil = "x";
	if ((tail = strrchr(ofil, '/')) == NULL)
		tail = ofil;
	else
		tail++;
	if(strlen(tail)>NAME_MAX-2) {
		fprintf(stderr, MSGSTR(OUTNMSIZ, 
		   "more than %d characters in output file name\n"),NAME_MAX-2);
		exit(1); }

	for (;;)
	{
		f = 1;
		for(i=0; i<count; i++)      /* break file */
		do {
			c = getc(is);
			if(c == EOF) {       /* put parts in prefix?? */
				if(f == 0)
					fclose(os);
				exit(0);
			}
			if(f) {              /* name parts file */
				for(f=0; ofil[f]; f++)
					fname[f] = ofil[f];
				fname[f++] = fnumber/NUM_ALPHA + 'a';
				fname[f++] = fnumber%NUM_ALPHA + 'a';
				fname[f] = '\0';
				if(++fnumber > MAX_OUTF) {
					fprintf(stderr, MSGSTR(ABRTSPLT, 
							"more than aa-zz output files needed, aborting split\n"));
					exit(1); }
				if((os=fopen(fname,"w")) == NULL) {
					fprintf(stderr, MSGSTR(OUTCRTERR, "Cannot create output\n"));
					exit(1);
				}
				f = 0;
			}
			putc(c, os);
		} while(c != '\n');
		fclose(os);
	}
}
