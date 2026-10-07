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
static char rcsid[] = "@(#)$RCSfile: users.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:45:10 $";
#endif
/*
 * COMPONENT_NAME: (CMDSTAT) status
 *
 * FUNCTIONS:
 *
 * ORIGINS: 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * users.c	1.3  com/cmd/stat,3.1,9021 12/4/89 16:41:48
 *
 * users - List the login names of the users currently on
 *         the system in a compact, one-line format.
 */

#include <sys/types.h>
#include <stdio.h>
#include <utmp.h>
#include <stdlib.h>
#include <locale.h>
#include <nl_types.h>
#include "users_msg.h"

#define NMAX 		sizeof(utmp.ut_name)
#define MAXUSERS	200


nl_catd catd;
#define MSGSTR(num,str) catgets(catd,MS_USERS,num,str)  

static void	summary();
static void	putline();
int scmp(char **p, char **q);
extern struct utmp *getutent();

struct utmp utmp;
struct utmp *utmp_ptr;		/* read pointer */
char	*names[MAXUSERS];	/* names table */
int	ncnt;			/* count of names */
char	**namp = names;		/* pointer to names table */


main(argc, argv)
char **argv;
int argc;
{

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_USERS, 0);

	setutent();
	while ((utmp_ptr = getutent()) != NULL) {
		if (utmp_ptr->ut_name[0] == '\0' || 
		    utmp_ptr->ut_type != USER_PROCESS)
			continue;
		if (++ncnt > MAXUSERS) {
			ncnt = MAXUSERS;
			fputs(MSGSTR(TOO_MANY, "users: too many users.\n"),
			      stderr);
			break;
		}
		putline();
	}
	endutent();
	summary();
	exit(0);
}

/*
 *  NAME:  putline
 *
 *  FUNCTION:  A valid user name was identified from the utmp file.
 *		Store the users name in memory to be sorted later.
 *	      
 *  RETURN VALUE:  	 none
 */

static void
putline()
{
	char temp[NMAX+1];

	strncpy(temp, utmp_ptr->ut_name, NMAX);
	temp[NMAX] = 0;
	*namp = malloc((size_t)(strlen(temp) + 1));
	if (namp == NULL) {
		perror("malloc");
		exit(1);
	}
	strcpy(*namp++, temp);
}

/*
 *  NAME:  scmp
 *
 *  FUNCTION:  Compare the first elements in two arrays of strings.
 *	      
 *  RETURN VALUE:  	 0   - equal
 *			 1   - not
 */

scmp(char **p, char **q)
{
	return(strcmp(*p, *q));
}

/*
 *  NAME:  summary
 *
 *  FUNCTION:  Sort all the users alphabetacally and print them out.
 *	      
 *  RETURN VALUE:  	 none
 */

static void
summary()
{
	register char **p;

	qsort((void *)names, 
		(size_t)ncnt, 
		(size_t)sizeof(names[0]), 
		(int(*)(const void *, const void *))scmp);
	for (p=names; p < namp; p++) {
		if (p != names)
			putchar(' ');
		fputs(*p, stdout);
	}
	if (namp != names)		/* at least one user */
		putchar('\n');
}
