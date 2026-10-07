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
 * OSF/1 Release 1.0.1
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: mktemp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:37:08 $";
#endif

/*
 * FUNCTIONS: mktemp
 *
 * DESCRIPTION:
 *	SVID2 compliant mktemp()
 */

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define	TRAILER	6	/* number of trailing chars to be replaced */


/*---------------------------------------------------------------------------*
 * Synopsis	: generate a unique filename
 * Parameters	:
 *		inout	template for filename
 * Return	: template or NULL if name cannot be generated
 * Fatal Errors	: none
 * Description	:
 *		Replace the trailing 6 'X's in the template string with
 *		a unique filename using a letter and the pid.
 * Notes	:
 *		SVID2 requires this (restricted) particular implementation.
 *		Making mktemp() `thread safe' would replace getpid() which we
 *		must have for SVID2, so caller must beware.
 *		Assumes the pid is less than 6 digits.
 *---------------------------------------------------------------------------*/
char *
mktemp(char *template)		
{
	int	offset;		/* offset to start of trailer */
	char	*s = template;
	char	*s0;
	int	pid;
	int	i;

	/* check template size */
	if ((offset = strlen(template) - TRAILER) < 0) {
		*template = NULL;
		return (template);
	}
	s += offset;

	/* check trailer is all 'X's */
	for (s0 = s; *s0 == 'X'; s0++);
	if (*s0) {	/* found a non 'X' before end */
		*template = NULL;
		return (template);
	}

	/* overwrite last 'X's with pid as a 5 digit number */
	pid = getpid();
	for (i = 0, s0--; i < TRAILER; i++, s0--) {	/* for speed */
		*s0 = pid % 10 + '0';
		pid /= 10;
	}

	/* loop trying names with different letters */
	for (*s = 'a'; *s <= 'z'; ++*s) {
		if (access(template, F_OK) == -1)
			return (template);
	}

	/* all names used */
	*template = NULL;
	return (template);
}
