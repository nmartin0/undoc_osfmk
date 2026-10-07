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
/*	@(#)allprint.c	4.1	12/25/82	*/

# include <stdio.h>
allprint(c)
  char c; {
	extern FILE *yyout;
	switch(c){
		case '\n':
			fprintf(yyout,"\\n");
			break;
		case '\t':
			fprintf(yyout,"\\t");
			break;
		case '\b':
			fprintf(yyout,"\\b");
			break;
		case ' ':
			fprintf(yyout,"\\\bb");
			break;
		default:
			if(!printable(c))
				fprintf(yyout,"\\%-3o",c);
			else 
				putc(c,yyout);
			break;
		}
	return;
	}
sprint(s)
  char *s; {
	while(*s)
		allprint(*s++);
	return;
	}
printable(c)
  int c;
	{
	return(040 < c && c < 0177);
	}
