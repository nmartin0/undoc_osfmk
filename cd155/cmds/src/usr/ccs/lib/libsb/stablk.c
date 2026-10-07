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
/*  stablk  --  string table lookup
 *
 *  Usage:  i = stablk (arg,table,quiet);
 *
 *	int i;
 *	char *arg,**table;
 *	int quiet;
 *
 *  Stablk looks for a string in "table" which matches
 *  "arg".  Table is declared like this:
 *    char *table[] = {"string1","string2",...,0};
 *  Each string in the table is checked via stablk() to determine
 *  if its initial characters match arg.  If exactly one such
 *  string matches arg, then the index of that string is returned.
 *  If none match arg, or if several match, then -1 (respectively -2)
 *  is returned.  Also, for either of these errors, if quiet is
 *  FALSE, the user will be asked if he wants a list of the possible
 *  strings.  In the case of multiple matches, the matching strings
 *  will be marked specially.
 *
 *  Originally from klg (Ken Greer) on IUS/SUS UNIX.
 */

#include <strings.h>
#include <stdio.h>

#define NOTFOUND -1
#define AMBIGUOUS -2
#define MAXSTRINGS 500

int stlmatch();
int strcmp();

int stablk (arg,table,quiet)
char *arg, **table;
int quiet;
{
	register int i,ix,count;
	int wantlist;
	char *matches[MAXSTRINGS];
	int exactmatch;

	count = 0;
	exactmatch = 0;
	for (i=0; table[i] != 0 && exactmatch == 0; i++) {
		if (stlmatch (table[i],arg)) {
			ix = i;		/* index of last match */
			matches[count++] = table[i];
			if (strcmp(table[i],arg) == 0)  exactmatch = 1;
		}
	}
	matches[count] = 0;

	if (exactmatch) {	/* i-th entry is exact match */
		--i;		/* (actually, i-1th entry) */
		matches[0] = table[i];
		count = 1;
		for (i=i+1; table[i] != 0; i++) {
			if (strcmp(table[i],arg) == 0)  {
				matches[count++] = table[i];
				ix = i;
			}
		}
		matches[count] = 0;
	}

	if (count == 1)  return (ix);

	if (!quiet) {
		if (strcmp(arg,"?") == 0) {
			wantlist = 1;
		}
		else {
			fprintf (stderr,"%s is %s.  ",arg,(count ? "ambiguous" : "unknown"));
			wantlist = getbool ("Do you want a list?",1);
		}
		if (wantlist) {
			fprintf (stderr,"Must match one of these:\n");
			if (count)  fprstab (stderr,matches);
			else	    fprstab (stderr,table);
		}
	}
	return (count ? AMBIGUOUS : NOTFOUND);
}
