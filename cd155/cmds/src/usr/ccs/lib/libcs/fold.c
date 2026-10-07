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
 
#ifndef lint
static char *rcsid = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libcs/fold.c,v 1.2 1994/11/19 02:15:28 mtm Exp $";
#endif


/*
 * $Log: fold.c,v $
 * Revision 1.2  1994/11/19  02:15:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/05/03  23:53:54  stans
 * Initial revision
 *
 *
 */

/*  fold  --  perform case folding
 *
 *  Usage:  p = foldup (out,in);
 *	    p = folddown (out,in);
 *	char *p,*in,*out;
 *
 *  Fold performs case-folding, moving string "in" to
 *  "out" and folding one case to another en route.
 *  Folding may be upper-to-lower case (folddown) or
 *  lower-to-upper case.
 *  Foldup folds to upper case; folddown folds to lower case.
 *  The same string may be specified as both "in" and "out".
 *  The address of "out" is returned for convenience.
 *
 **********************************************************************
 * HISTORY
 * 23-Sep-86  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Unexported original fold routine.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.  The foldup() and folddown() routines
 *	are new.
 *
 **********************************************************************
 */

typedef enum {FOLDUP, FOLDDOWN} FOLDMODE;

static
char *fold (out,in,whichway)
char *in,*out;
FOLDMODE whichway;
{
	register char *i,*o;
	register char lower;
	char upper;
	int delta;

	switch (whichway)
	{
	case FOLDUP:
		lower = 'a';		/* lower bound of range to change */
		upper = 'z';		/* upper bound of range */
		delta = 'A' - 'a';	/* amount of change */
		break;
	case FOLDDOWN:
		lower = 'A';
		upper = 'Z';
		delta = 'a' - 'A';
	}

	i = in;
	o = out;
	do {
		if (*i >= lower && *i <= upper)		*o++ = *i++ + delta;
		else					*o++ = *i++;
	} 
	while (*i);
	*o = '\0';
	return (out);
}

char *foldup (out,in)
char *in,*out;
{
	return (fold(out,in,FOLDUP));
}

char *folddown (out,in)
char *in,*out;
{
	return (fold(out,in,FOLDDOWN));
}
