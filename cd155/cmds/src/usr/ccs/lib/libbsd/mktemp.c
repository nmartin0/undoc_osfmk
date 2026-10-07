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
 * (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * OSF/1 Release 1.0.4
 */
/*
static char sccsid[] = "@(#)mktemp.c	1.4  com/lib/bsd,3.1,8943 9/7/89 16:01:19";
 *
 * COMPONENT_NAME: (LIBBSD)  Berkeley Compatibility Library
 *
 * FUNCTIONS:  mktemp
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * $Id: mktemp.c,v 3.3 1994/11/19 02:00:53 mtm Exp $
 *
 * HISTORY:
 *
 * $Log: mktemp.c,v $
 * Revision 3.3  1994/11/19  02:00:53  mtm
 * Copyright additions/changes
 *
 * Revision 3.2  1992/04/27  15:50:35  stans
 * apply Shala's fixes.
 *
 */

#include <stdio.h>		/* for NULL		*/
#include <sys/access.h>		/* for access(,F_OK)...	*/

#define	TMPSIZE	6

#ifdef KJI
#include <sys/NLchar.h>
#endif

extern int strlen(), access(), getpid();

/*
 * NAME:	mktemp
 *
 * FUNCTION:	mktemp - construct a unique filename
 *
 * NOTES:	Mktemp expects a string of length at least 6, with
 *		six trailing 'X's, and overwrites the X's with a
 *		(hopefully) unique encoding of the process' pid and
 *		a letter.
 *
 * RETURN VALUE DESCRIPTION:
 *		If a unique filename was not generated, a "/" is returned.
 *		Else the filename is returned.
 */

char *
mktemp(template)
char *template;
{
	char *pt, *s = template;
	pid_t pid;
	char t[TMPSIZE + 1];	/* hold a unique number */
	int i;

  pid = getpid();

  for(i = 1; i <= TMPSIZE; i++)  {
		t[TMPSIZE - i] = (pid % 10) + '0';
		pid /=10;
  }

  t[TMPSIZE] = '\0';

  pt = t;
#ifdef KJI
	char *sp;		/* current data point in s */
	char *xptr = NULL;	/* pointer to first 'X' in string */

	s += strlen(template);	/* point at the terminal null */
	pt += sptrlen(pt);

	/* search forward for the first of the trailing 'X's in s */
	for(sp = template; *sp != '\0'; sp+=NLchrlen(sp))
		if(NCdechr(sp) == 'X') {
			if(xptr == NULL)
				xptr = sp;
		} else
			xptr = NULL;
	if(xptr == NULL)
		xptr = s;

	while(--s >= xptr)
#else
	s += strlen(template);	/* point at the terminal null */
	pt += strlen(pt);	/* get a unique number	*/
	while(*--s == 'X')
#endif
		*s = *--pt;

	s++;
	i = 'a';
	while (access(template, F_OK) != -1) {
		if (i =='z')
			return ("/");
		*s = i++;
	}

	return(template);
}
