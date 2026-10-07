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
static char rcsid[] = "@(#)$RCSfile: srchscore.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:12:13 $";
#endif
/*  srchscore --  perform approximate string matching
 *
 *  int srchscore (big,small);
 *  tells how well "small" matches substrings of "big".
 *  The score ranges from 0 to (strlen(small) ** 2).
 *  The score is something like the sum, over the longest
 *  matching contiguous substrings, of the square of the
 *  length of the substring minus some penalty for
 *  not-quite-exactly-matching substrings.  Case is not significant.
 *
 *  The value ranges from 0 to strlen(small)**2.
 */

#define INSERTCOST 1	/* penalty to insert 1 char in small */
#define DELETECOST 1	/* penalty to delete 1 char from small */
#define CHANGECOST 3	/* penalty to change 1 char in small */

static char *newsmall;	/* next character to match in "small" */

static int smatch (big,small)
char *big,*small;
{
	register char *b,*s;	/* current chars in big and small */
	register int goon;	/* TRUE while still looping */
	int nmatch, penalty;	/* match count; total penalty */

	nmatch = 0;
	penalty = 0;
	goon = 1;
	b = big;
	s = small;

	while (*b && *s && goon) {
		if (*b == *s) {
			/* just do stuff at end of loop */
		}
		else if (*(b+1) == *s) {
			b++;
			penalty += INSERTCOST;
		}
		else if (*b == *(s+1)) {
			s++;
			penalty += DELETECOST;
		}
		else if (*(b+1) == *(s+1)) {
			b++; s++;
			penalty += CHANGECOST;
		}
		else {
			goon = 0;
		}
		if (goon) {	/* something matched */
			b++; s++;
			nmatch++;
		}
	}
	newsmall = s;
	return ((nmatch * nmatch) - penalty);
}

int srchscore (big,small)
char *big,*small;
{
	register int score,bestscore,total;
	char *b,*s,*bestsmall;

	total = 0;

	if (*big) {
		s = small;
		while (*s) {
			bestscore = -1;
			for (b=big; *b; b++) {
				score = smatch (b,s);
				if (score > bestscore) {
					bestscore = score;
					bestsmall = newsmall;
				}
			}
			if (bestsmall > s) {
				total += bestscore;
				s = bestsmall;
			}
			else {
				if (total >= DELETECOST-1)  total -= DELETECOST;
				s++;
			}
		}
	}
	if (total < 0)  total = 0;
	return (total);
}
